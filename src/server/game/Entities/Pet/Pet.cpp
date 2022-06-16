////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Player.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Pet.h"
#include "Formulas.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "CreatureAI.h"
#include "Unit.h"
#include "Util.h"
#include "Group.h"
#ifdef CROSS
#include "InterRealmMgr.h"
#endif /* CROSS */

#define PET_XP_FACTOR 0.05f

Pet::Pet(Player* owner, PetType type) : Guardian(NULL, owner, true),
m_removed(false), m_owner(owner),
m_petType(type), m_duration(0),
m_loading(false), m_specialization(0), m_declinedname(NULL)
{
    m_unitTypeMask |= UNIT_MASK_PET;
    if (type == HUNTER_PET)
        m_unitTypeMask |= UNIT_MASK_HUNTER_PET;

    if (!(m_unitTypeMask & UNIT_MASK_CONTROLABLE_GUARDIAN))
    {
        m_unitTypeMask |= UNIT_MASK_CONTROLABLE_GUARDIAN;
        InitCharmInfo();
    }

    m_name = "Pet";
    m_RegenPowerTimer = PET_FOCUS_REGEN_INTERVAL;
    m_Stampeded = false;
}

Pet::~Pet()
{
    delete m_declinedname;
}

void Pet::AddToWorld()
{
    ///- Register the pet for guid lookup
    if (!IsInWorld())
    {
        ///- Register the pet for guid lookup
        sObjectAccessor->AddObject(this);
        Unit::AddToWorld();
        AIM_Initialize();
    }

    // Prevent stuck pets when zoning. Pets default to "follow" when added to world
    // so we'll reset flags and let the AI handle things
    if (GetCharmInfo() && GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
    {
        GetCharmInfo()->SetIsCommandAttack(false);
        GetCharmInfo()->SetIsCommandFollow(false);
        GetCharmInfo()->SetIsAtStay(false);
        GetCharmInfo()->SetIsFollowing(false);
        GetCharmInfo()->SetIsReturning(false);
    }

    Player* l_Owner = m_owner ? m_owner->ToPlayer() : nullptr;
    // Hack fix for Soul link, we need to buff pet and player if player has a talent
    if (l_Owner && l_Owner->HasSpell(108415))
    {
        CastSpell(m_owner, 108446, true);
        m_owner->CastSpell(this, 108446, true);
    }

    /// Command Demon
    if (l_Owner && l_Owner->getClass() == CLASS_WARLOCK)
    {
        if (m_owner->HasAura(108503))
            m_owner->RemoveAura(108503);

        /// Threatening Presence - special ability for some warlock pets
        if (GetEntry() == WarlockPet::FelGuard || GetEntry() == WarlockPet::WrathGuard)
            LearnSpell(134477);
        else if (GetEntry() == WarlockPet::VoidWalker || GetEntry() == WarlockPet::VoidLord)
            LearnSpell(112042);

        // Supplant Command Demon
        if (m_owner->getLevel() >= 56)
        {
            int32 bp = 0;

            m_owner->RemoveAura(119904);

            bp = GetCommandDemonSpellByEntry(GetEntry());

            if (bp)
                m_owner->CastCustomSpell(m_owner, 119904, &bp, NULL, NULL, true);
        }
    }
    /// Flanking Strikes
    else if (l_Owner && l_Owner->getClass() == Classes::CLASS_HUNTER && l_Owner->GetActiveSpecializationID() == SpecIndex::SPEC_HUNTER_SURVIVAL)
        l_Owner->CastSpell(l_Owner, 240152, true);
}

void Pet::RemoveFromWorld()
{
    ///- Remove the pet from the accessor
    if (IsInWorld())
    {
        ///- Don't call the function for Creature, normal mobs + totems go in a different storage
        Unit::RemoveFromWorld();
        sObjectAccessor->RemoveObject(this);
    }
}

void Pet::LoadPetFromDB(Player* owner, uint32 petentry, uint32 /*petnumber*/, bool current, PetSlot slotID, bool stampeded, PetQueryHolder* holder, std::function<void(Pet*, bool)> p_Callback)
{
    m_loading = true;

    if (owner->IsPlayer())
    {
        /// Hack for water elemental
        /// Pet should be saved for all specs, but can be summoned only by frost mages
        if (owner->getClass() == CLASS_MAGE && owner->ToPlayer()->GetActiveSpecializationID() != SPEC_MAGE_FROST)
        {
            m_loading = false;
            p_Callback(this, false);
            return;
        }
        /// Hack for Raise dead
        /// Pet should be saved for all specs, but can be summoned only by unholy dk
        if (owner->getClass() == CLASS_DEATH_KNIGHT && owner->ToPlayer()->GetActiveSpecializationID() != SPEC_DK_UNHOLY)
        {
            m_loading = false;
            p_Callback(this, false);
            return;
        }
    }

    if (slotID == PET_SLOT_ACTUAL_PET_SLOT)
        slotID = owner->m_currentPetSlot;


    PreparedQueryResult result = holder->GetPetResult();

    if (!result)
    {
        m_loading = false;
        p_Callback(this, false);
        return;
    }

    Field* fields = result->Fetch();

    // update for case of current pet "slot = 0"
    petentry = fields[1].GetUInt32();
    if (!petentry)
    {
        p_Callback(this, false);
        return;
    }

    uint32 summon_spell_id = fields[14].GetUInt32();
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(summon_spell_id);

    bool is_temporary_summoned = spellInfo && spellInfo->GetDuration() > 0;

    // check temporary summoned pets like mage water elemental
    if (current && is_temporary_summoned)
    {
        m_loading = false;
        p_Callback(this, false);
        return;
    }

    PetType pet_type = PetType(fields[15].GetUInt8());
    if (pet_type == HUNTER_PET)
    {
        CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(petentry);
        if (!creatureInfo || !creatureInfo->isTameable(owner->CanTameExoticPets()))
        {
            p_Callback(this, false);
            return;
        }
    }

    uint32 pet_number = fields[0].GetUInt32();

    if (current && owner->IsPetNeedBeTemporaryUnsummoned())
    {
        owner->SetTemporaryUnsummonedPetNumber(pet_number);
        p_Callback(this, false);
        return;
    }

    Map* map = owner->GetMap();
    uint32 guid = sObjectMgr->GenerateLowGuid(HIGHGUID_PET);

    auto& l_Phases = owner->GetPhases();
    if (!Create(guid, map, owner->GetPhaseMask(), l_Phases, petentry))
    {
        p_Callback(this, false);
        return;
    }

    float px, py, pz;
    owner->GetClosePoint(px, py, pz, GetObjectSize(), PET_FOLLOW_DIST, GetFollowAngle());
    Relocate(px, py, pz, owner->GetOrientation());

    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_PETS, "Pet (guidlow %d, entry %d) not loaded. Suggested coordinates isn't valid (X: %f Y: %f)",
            GetGUIDLow(), GetEntry(), GetPositionX(), GetPositionY());
        p_Callback(this, false);
        return;
    }

    setPetType(pet_type);
    setFaction(owner->getFaction());
    SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, summon_spell_id);

    CreatureTemplate const* cinfo = GetCreatureTemplate();
    if (cinfo->type == CREATURE_TYPE_CRITTER)
    {
        map->AddToMap(this->ToCreature());
        p_Callback(this, true);
        return;
    }

    m_charmInfo->SetPetNumber(pet_number, IsPermanentPetFor(owner));

    ///< Summon Water Elemental
    uint32 l_DisplayID = fields[3].GetUInt32();
    if (summon_spell_id == 31687)
        l_DisplayID = 525;

    SetDisplayId(l_DisplayID);
    SetNativeDisplayId(l_DisplayID);
    uint32 petlevel = fields[4].GetUInt16();
    SetUInt32Value(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_NONE);
    SetName(fields[8].GetString());

    Powers powerType = POWER_MANA;
    switch (cinfo->unit_class)
    {
        case CLASS_WARRIOR:
            powerType = POWER_RAGE;
            break;
        case CLASS_ROGUE:
            powerType = POWER_ENERGY;
            break;
    }

    switch (getPetType())
    {
        case SUMMON_PET:
            petlevel = owner->getLevel();

            SetByteValue(UNIT_FIELD_SEX, UNIT_BYTES_0_OFFSET_CLASS, CLASS_MAGE);
            SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
                                                            // this enables popup window (pet dismiss, cancel)
            if (owner && owner->getClass() == CLASS_WARLOCK)
            {
                SetUInt32Value(UNIT_FIELD_SEX, 0x400); // class = rogue
                setPowerType(POWER_ENERGY); // Warlock's pets have energy
            }

            if (cinfo && (cinfo->Entry == FelGuard || cinfo->Entry == WrathGuard))
            {
                if (owner && owner->IsPlayer() && owner->HasAura(56246))
                {
                    /// Get item template for Fel Guard weapon
                    if (ItemTemplate const* l_ItemTemplate = sObjectMgr->GetItemTemplate(12784))
                    {
                        uint32 l_RandomItemId = owner->ToPlayer()->GetRandomWeaponFromPrimaryBag(l_ItemTemplate);
                        LoadSpecialEquipment(l_RandomItemId);
                    }
                }
            }
            break;
        case HUNTER_PET:
        {
            SetByteValue(UNIT_FIELD_SEX, UNIT_BYTES_0_OFFSET_CLASS, CLASS_WARRIOR);
            SetGender(GENDER_NONE);

            SetSheath(SHEATH_STATE_MELEE);
            SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PET_FLAGS, fields[9].GetBool() ? UNIT_CAN_BE_ABANDONED : UNIT_CAN_BE_RENAMED | UNIT_CAN_BE_ABANDONED);

            SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
            // this enables popup window (pet abandon, cancel)
            setPowerType(POWER_FOCUS);
            break;
        }
        default:
            if (!IsPetGhoulOrAbomination())
                sLog->outError(LOG_FILTER_PETS, "Pet have incorrect type (%u) for pet loading.", getPetType());
            break;
    }

    SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL))); // cast can't be helped here
    SetCreatorGUID(owner->GetGUID());

    InitStatsForLevel(petlevel);
    SetUInt32Value(UNIT_FIELD_PET_EXPERIENCE, fields[5].GetUInt32());

    SynchronizeLevelWithOwner();

    SetReactState(ReactStates(fields[6].GetUInt8()));
    SetCanModifyStats(true);

    owner->SetMinion(this, true, slotID == PET_SLOT_UNK_SLOT ? PET_SLOT_OTHER_PET : slotID, stampeded);
    map->AddToMap(this->ToCreature());

    m_slot = fields[7].GetUInt8();

    if (getPetType() == SUMMON_PET && !current && owner && owner->getClass() != CLASS_WARLOCK)  //all (?) summon pets come with full health when called, but not when they are current
        SetPower(POWER_MANA, GetMaxPower(POWER_MANA));
    else
    {
        uint32 savedhealth = fields[10].GetUInt32();
        uint32 savedmana = fields[11].GetUInt32();
        if (!savedhealth && getPetType() == HUNTER_PET)
            setDeathState(JUST_DIED);
        else if (owner && owner->getClass() != CLASS_WARLOCK)
        {
            SetHealth(savedhealth > GetMaxHealth() ? GetMaxHealth() : savedhealth);
            SetPower(POWER_MANA, savedmana > uint32(GetMaxPower(POWER_MANA)) ? GetMaxPower(POWER_MANA) : savedmana);
        }
        else
        {
            if (savedhealth > GetMaxHealth() || savedhealth < 1)
                savedhealth = GetMaxHealth();
            SetHealth(savedhealth);
            uint32 l_CreatePower = 0;
            PetStatInfo const* l_PetStatInfo = GetPetStat(GetEntry());
            if (!l_PetStatInfo || l_PetStatInfo->m_CreatePower == -1)
                l_CreatePower = GetCreatePowers(Powers::POWER_ENERGY);
            else
            {
                // Negative number, it's fix value
                if (l_PetStatInfo->m_CreatePower < 0.0f)
                    l_CreatePower = l_PetStatInfo->m_CreatePower * -1;
                // Positive number, it's percentage of owner power
                else if (owner != nullptr)
                    l_CreatePower = float(owner->GetMaxPower(owner->getPowerType()) * l_PetStatInfo->m_CreatePower);
            }
            SetMaxPower(POWER_ENERGY, l_CreatePower);
            SetPower(POWER_ENERGY, l_CreatePower);
        }
    }

    uint32 timediff = uint32(time(NULL) - fields[13].GetUInt32());

    PreparedQueryResult auraResult          = holder->GetPreparedResult(PET_LOGIN_QUERY_LOADAURA);
    PreparedQueryResult auraEffectResult    = holder->GetPreparedResult(PET_LOGIN_QUERY_LOADAURAEFFECT);
    PreparedQueryResult spellResult         = holder->GetPreparedResult(PET_LOGIN_QUERY_LOADSPELL);
    PreparedQueryResult spellCooldownResult = holder->GetPreparedResult(PET_LOGIN_QUERY_LOADSPELLCOOLDOWN);

    _LoadAuras(auraResult, auraEffectResult, timediff);

    if (owner->IsPlayer() && owner->ToPlayer()->InArena())
        RemoveArenaAuras();

    // load action bar, if data broken will fill later by default spells.
    m_charmInfo->LoadPetActionBar(fields[12].GetString());

    _LoadSpells(spellResult);
    _LoadSpellCooldowns(spellCooldownResult);
    LearnPetPassives();
    InitLevelupSpellsForLevel();
    CastPetAuras(current);

    sLog->outDebug(LOG_FILTER_PETS, "New Pet has guid %u", GetGUIDLow());

    uint16 specId = fields[16].GetUInt32();
    if (ChrSpecializationsEntry const* petSpec = sChrSpecializationsStore.LookupEntry(specId))
        specId = GetChrSpecializationByIndex(owner->HasAuraType(SPELL_AURA_OVERRIDE_PET_SPECS) ? PET_SPEC_OVERRIDE_CLASS_INDEX : 0, petSpec->OrderIndex)->ID;

    SetSpecialization(specId);

    /// Remove unknown spells from action bar after load
    CleanupActionBar();

    // The SetSpecialization function will run these functions if the pet's spec is not 0
    if (!GetSpecialization())
        owner->PetSpellInitialize();

    owner->PetSpellInitialize();

    if (owner->GetGroup())
        owner->SetGroupUpdateFlag(GROUP_UPDATE_PET);

    if (getPetType() == HUNTER_PET)
    {
        PreparedQueryResult result = holder->GetPreparedResult(PET_LOGIN_QUERY_DECLINED_NAME);

        if (result)
        {
            delete m_declinedname;
            m_declinedname = new DeclinedName;
            Field* fields2 = result->Fetch();
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                m_declinedname->name[i] = fields2[i].GetString();
        }
    }

    //set last used pet number (for use in BG's)
    if (owner->IsPlayer() && isControlled() && !isTemporarySummoned() && (getPetType() == SUMMON_PET || getPetType() == HUNTER_PET))
        owner->ToPlayer()->SetLastPetNumber(pet_number);

    m_loading = false;
    p_Callback(this, true);

    if (owner->IsPlayer())
        sScriptMgr->OnSummonPet(owner->ToPlayer());
}

void Pet::SavePetToDB(PetSlot mode, bool stampeded, bool ownerLogout /* = false*/)
{
    if (!GetEntry())
        return;

    // save only fully controlled creature
    if (!isControlled())
        return;

    if (stampeded)
        return;

    // not save not player pets
    if (!IS_PLAYER_GUID(GetOwnerGUID()))
        return;

    Player* owner = (Player*)GetOwner();
    if (!owner)
        return;

#ifdef CROSS
    if (owner->GetSession()->GetInterRealmClient() == nullptr)
        return;

    InterRealmDatabasePool* l_Database = GetOwner()->GetRealmDatabase();
#else
    auto l_Database = &CharacterDatabase;
#endif

    if ((static_cast<int8>(mode) == PET_SLOT_ACTUAL_PET_SLOT) && !ownerLogout && !owner->GetSession()->PlayerLogoutWithSave())
        mode = owner->m_currentPetSlot;

    SetSlot(mode);

    // not save pet as current if another pet temporary unsummoned
    if (mode == owner->m_currentPetSlot && owner->GetTemporaryUnsummonedPetNumber() &&
        owner->GetTemporaryUnsummonedPetNumber() != m_charmInfo->GetRealmPetNumber())
    {
        // pet will lost anyway at restore temporary unsummoned
        if (getPetType() == HUNTER_PET)
            return;

        // for warlock case
        mode = PET_SLOT_OTHER_PET;
    }

    uint64 curhealth = GetHealth();
    uint32 curmana = GetPower(POWER_MANA);

    SQLTransaction trans = l_Database->BeginTransaction();

    // save auras before possibly removing them
    _SaveAuras(trans);

    // stable and not in slot saves
    if (mode > PET_SLOT_HUNTER_LAST && getPetType() == HUNTER_PET)
        RemoveAllAuras();

    _SaveSpells(trans);
    _SaveSpellCooldowns(trans);
    l_Database->CommitTransaction(trans);

    // current/stable/not_in_slot
    if (mode >= PET_SLOT_HUNTER_FIRST)
    {
        uint32 ownerLowGUID = GetOwner()->GetRealGUIDLow();
        std::string name = m_name;
        l_Database->EscapeString(name);
        trans = l_Database->BeginTransaction();

        // remove current data
        trans->PAppend("DELETE FROM character_pet WHERE owner = '%u' AND id = '%u'", ownerLowGUID, m_charmInfo->GetRealmPetNumber());

        // save pet
        std::ostringstream ss;
        ss  << "INSERT INTO character_pet (id, entry, owner, modelid, level, exp, Reactstate, slot, name, renamed, curhealth, curmana, abdata, savetime, CreatedBySpell, PetType, specialization) "
            << "VALUES ("
            << m_charmInfo->GetRealmPetNumber() << ','
            << GetEntry() << ','
            << ownerLowGUID << ','
            << GetNativeDisplayId() << ','
            << uint32(getLevel()) << ','
            << GetUInt32Value(UNIT_FIELD_PET_EXPERIENCE) << ','
            << uint32(GetReactState()) << ','
            << uint32(mode) << ", '"
            << name.c_str() << "', "
            << uint32(HasByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PET_FLAGS, UNIT_CAN_BE_RENAMED) ? 0 : 1) << ','
            << curhealth << ','
            << curmana << ", '";

        for (uint32 i = ACTION_BAR_INDEX_START; i < ACTION_BAR_INDEX_END; ++i)
        {
            ss << uint32(m_charmInfo->GetActionBarEntry(i)->GetType()) << ' '
                << uint32(m_charmInfo->GetActionBarEntry(i)->GetAction()) << ' ';
        };

        ss  << "', "
            << time(NULL) << ','
            << GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL) << ','
            << uint32(getPetType()) << ','
            << GetSpecialization()
            << ')';

        trans->Append(ss.str().c_str());

        uint64 l_PlayerGUID = owner->GetGUID();
        MS::Utilities::CallBackPtr l_CallBack = std::make_shared<MS::Utilities::Callback>([l_PlayerGUID](bool p_Success) -> void
        {
            Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGUID);
            if (!l_Player)
                return;

            l_Player->GetSession()->SendStablePet(0);
        });

        l_Database->CommitTransaction(trans, owner->GetQueryCallbackMgr(), l_CallBack);
    }
    // delete
    else
    {
        if (owner->m_currentPetSlot >= PET_SLOT_HUNTER_FIRST && owner->m_currentPetSlot <= PET_SLOT_HUNTER_LAST)
            owner->setPetSlotUsed(owner->m_currentPetSlot, false);
        RemoveAllAuras();

#ifdef CROSS
        uint32 l_RealmID = owner->GetSession()->GetInterRealmNumber();
#else
        uint32 l_RealmID = g_RealmID;
#endif

        DeleteFromDB(m_charmInfo->GetRealmPetNumber(), l_RealmID);
    }
}

void Pet::DeleteFromDB(uint32 guidlow, uint32 realmId)
{
#ifdef CROSS
    InterRealmDatabasePool* l_Database = sInterRealmMgr->GetClientByRealmNumber(realmId)->GetDatabase();
#else
    auto l_Database = &CharacterDatabase;
#endif

    SQLTransaction trans = l_Database->BeginTransaction();

    trans->PAppend("DELETE FROM character_pet WHERE id = '%u'", guidlow);
    trans->PAppend("DELETE FROM character_pet_declinedname WHERE id = '%u'", guidlow);
    trans->PAppend("DELETE FROM pet_aura WHERE guid = '%u'", guidlow);
    trans->PAppend("DELETE FROM pet_spell WHERE guid = '%u'", guidlow);
    trans->PAppend("DELETE FROM pet_spell_cooldown WHERE guid = '%u'", guidlow);

    l_Database->CommitTransaction(trans);
}

void Pet::setDeathState(DeathState s)                       // overwrite virtual Creature::setDeathState and Unit::setDeathState
{
    Creature::setDeathState(s);

    if (getDeathState() == CORPSE)
    {
        if (getPetType() == HUNTER_PET)
        {
            // pet corpse non lootable and non skinnable
            SetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
            RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
        }
    }
    else if (getDeathState() == ALIVE)
    {
        if (getPetType() == HUNTER_PET)
        {
            CastPetAuras(true);

            if (Unit* owner = GetOwner())
                if (Player* player = owner->ToPlayer())
                    player->StopCastingCharm();
        }
        else
        {
            CastPetAuras(true);
        }
    }
}

void Pet::Update(uint32 diff)
{
    if (m_removed)                                           // pet already removed, just wait in remove queue, no updates
        return;

    if (m_loading)
        return;

    if (getPetType() == HUNTER_PET) ///< Mend pet override (revive pet)
    {
        Player* l_Owner = GetOwner();
        if (l_Owner != nullptr)
        {
            if (getDeathState() == ALIVE && !l_Owner->HasAura(157863))
                l_Owner->CastSpell(l_Owner, 157863, true);
            else if (l_Owner->HasAura(157863) && getDeathState() != ALIVE)
                l_Owner->RemoveAura(157863);
        }
    }

    /// Remove bolstering from pets
    if (HasAura(209859))
        RemoveAura(209859);

    switch (m_deathState)
    {
        case CORPSE:
        {
            if (getPetType() != HUNTER_PET || m_corpseRemoveTime <= time(NULL))
            {
                Remove(PET_SLOT_ACTUAL_PET_SLOT, false, m_Stampeded);               //hunters' pets never get removed because of death, NEVER!
                return;
            }
            break;
        }
        case ALIVE:
        {
            // unsummon pet that lost owner
            Player* owner = GetOwner();
            if (!owner || (!IsWithinDistInMap(owner, GetMap()->GetVisibilityRange()) && !isPossessed()) || (isControlled() && !owner->GetPetGUID()))
            {
                Remove(PET_SLOT_ACTUAL_PET_SLOT, true, m_Stampeded);
                return;
            }

            if (isControlled())
            {
                if (owner->GetPetGUID() != GetGUID() && !m_Stampeded) // Stampede
                {
                    sLog->outError(LOG_FILTER_PETS, "Pet %u is not pet of owner %s, removed", GetEntry(), m_owner->GetName());
                    Remove(getPetType() == HUNTER_PET ? PET_SLOT_DELETED : PET_SLOT_ACTUAL_PET_SLOT, true, m_Stampeded);
                    return;
                }
            }

            if (m_duration > 0)
            {
                if (uint32(m_duration) > diff)
                    m_duration -= diff;
                else
                {
                    Remove(getPetType() != SUMMON_PET ? PET_SLOT_DELETED : PET_SLOT_ACTUAL_PET_SLOT, false, m_Stampeded);
                    return;
                }
            }

            if (Unit* l_Victim = getVictim())
            {
                if (!HasUnitState(UNIT_STATE_CASTING) && GetCharmInfo() && !GetCharmInfo()->IsMoveTo() && !GetCharmInfo()->IsMoveToStay())
                {
                    if (NeedToChase(l_Victim))
                        GetMotionMaster()->MoveChase(l_Victim);
                    else
                        GetMotionMaster()->Clear();
                }
            }

            //regenerate focus for hunter pets or energy for deathknight's ghoul
            if (m_RegenPowerTimer)
            {
                if (m_RegenPowerTimer > diff)
                    m_RegenPowerTimer -= diff;
                else
                {
                    switch (getPowerType())
                    {
                        case POWER_FOCUS:
                            Regenerate(POWER_FOCUS);
                            m_RegenPowerTimer += PET_FOCUS_REGEN_INTERVAL - diff;
                            if (!m_RegenPowerTimer)
                                ++m_RegenPowerTimer;

                            // Reset if large diff (lag) causes focus to get 'stuck'
                            if (m_RegenPowerTimer > PET_FOCUS_REGEN_INTERVAL)
                                m_RegenPowerTimer = PET_FOCUS_REGEN_INTERVAL;

                            break;
                        default:
                            m_RegenPowerTimer = 0;
                            break;
                    }
                }
            }
            break;
        }
        default:
            break;
    }

    Creature::Update(diff);
}

void Creature::Regenerate(Powers power)
{
    uint32 curValue = GetPower(power);
    uint32 maxValue = GetMaxPower(power);

    if (curValue >= maxValue)
        return;

    float addvalue = 0.0f;

    switch (power)
    {
        case POWER_FOCUS:
        {
            // For hunter pets - Pets regen focus 125% more faster than owners
            if (GetOwner())
            {
                /// Calculate owners haste
                float l_OwnerHastePct = 1.0f / GetOwner()->GetFloatValue(UNIT_FIELD_MOD_HASTE);
                float l_OwnerBaseRegen = 10.0f;
                /// Calculate regenerate for 1 second and multiply for update interval
                addvalue += (l_OwnerBaseRegen * l_OwnerHastePct) * PET_FOCUS_REGEN_INTERVAL / IN_MILLISECONDS * sWorld->getRate(RATE_POWER_FOCUS);
                addvalue *= 1.25f;
            }
            break;
        }
        case POWER_ENERGY:
        {
            // For deathknight's ghoul and Warlock's pets
            addvalue = 20;

            if (GetOwner())
            {
                /// Calculate owners haste
                float l_OwnerHastePct = 1.0f / GetOwner()->GetFloatValue(UNIT_FIELD_MOD_HASTE);
                float l_OwnerBaseRegen = 10.0f;

                switch (GetEntry())
                {
                    case WarlockPet::Imp:
                    case WarlockPet::FellImp:
                        l_OwnerBaseRegen = 12.0f;
                        break;
                    case WarlockPet::FelGuard:
                        l_OwnerBaseRegen = 11.0f;
                        break;
                    case WarlockPet::FelHunter:
                    case WarlockPet::Observer:
                        l_OwnerBaseRegen = 10.0f;
                        break;
                }

                /// Calculate regenerate for 1 second and multiply for update interval
                addvalue += (l_OwnerBaseRegen * l_OwnerHastePct) * PET_FOCUS_REGEN_INTERVAL / IN_MILLISECONDS * sWorld->getRate(RATE_POWER_ENERGY);
            }
            break;
        }
        default:
            return;
    }

    // Apply modifiers (if any).
    AuraEffectList const& ModPowerRegenPCTAuras = GetAuraEffectsByType(SPELL_AURA_MOD_POWER_REGEN_PERCENT);
    for (AuraEffectList::const_iterator i = ModPowerRegenPCTAuras.begin(); i != ModPowerRegenPCTAuras.end(); ++i)
        if (Powers((*i)->GetMiscValue()) == power)
            AddPct(addvalue, (*i)->GetAmount());

    addvalue += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, power) * (isHunterPet()? PET_FOCUS_REGEN_INTERVAL : CREATURE_REGEN_INTERVAL) / (5 * IN_MILLISECONDS);

    int32 intAddValue = int32(addvalue);

    if (IsAIEnabled)
        AI()->RegeneratePower(power, intAddValue);

    ModifyPower(power, intAddValue);
}

void Pet::Remove(PetSlot mode, bool returnreagent, bool stampeded)
{
    m_owner->RemovePet(this, mode, returnreagent, stampeded);
}

void Pet::GivePetXP(uint32 xp)
{
    if (getPetType() != HUNTER_PET)
        return;

    if (xp < 1)
        return;

    if (!isAlive())
        return;

    uint8 maxlevel = std::min((uint8)sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL), GetOwner()->getLevel());
    uint8 petlevel = getLevel();

    // If pet is detected to be at, or above(?) the players level, don't hand out XP
    if (petlevel >= maxlevel)
       return;

    uint32 nextLvlXP = GetUInt32Value(UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE);
    uint32 curXP = GetUInt32Value(UNIT_FIELD_PET_EXPERIENCE);
    uint32 newXP = curXP + xp;

    // Check how much XP the pet should receive, and hand off have any left from previous levelups
    while (newXP >= nextLvlXP && petlevel < maxlevel)
    {
        // Subtract newXP from amount needed for nextlevel, and give pet the level
        newXP -= nextLvlXP;
        ++petlevel;

        GivePetLevel(petlevel);

        nextLvlXP = GetUInt32Value(UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE);
    }
    // Not affected by special conditions - give it new XP
    SetUInt32Value(UNIT_FIELD_PET_EXPERIENCE, petlevel < maxlevel ? newXP : 0);
}

void Pet::GivePetLevel(uint8 level)
{
    if (!level || level == getLevel())
        return;

    if (getPetType()==HUNTER_PET)
    {
        SetUInt32Value(UNIT_FIELD_PET_EXPERIENCE, 0);
        SetUInt32Value(UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE, uint32(sObjectMgr->GetXPForLevel(level)*PET_XP_FACTOR));
    }

    InitStatsForLevel(level);
    InitLevelupSpellsForLevel();
}

bool Pet::CreateBaseAtCreature(Creature* p_Creature, std::set<uint32> p_Phases)
{
    ASSERT(p_Creature);

    if (!CreateBaseAtTamed(p_Creature->GetCreatureTemplate(), p_Creature->GetMap(), p_Creature->GetPhaseMask(), p_Phases))
        return false;

    Relocate(p_Creature->GetPositionX(), p_Creature->GetPositionY(), p_Creature->GetPositionZ(), p_Creature->GetOrientation());

    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_PETS, "Pet (guidlow %d, entry %d) not created base at creature. Suggested coordinates isn't valid (X: %f Y: %f)",
            GetGUIDLow(), GetEntry(), GetPositionX(), GetPositionY());
        return false;
    }

    CreatureTemplate const* cinfo = GetCreatureTemplate();
    if (!cinfo)
    {
        sLog->outError(LOG_FILTER_PETS, "CreateBaseAtCreature() failed, creatureInfo is missing!");
        return false;
    }

    SetDisplayId(p_Creature->GetDisplayId());

    if (CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(cinfo->family))
        SetName(cFamily->Name->Get(sWorld->GetDefaultDb2Locale()));
    else
        SetName(p_Creature->GetNameForLocaleIdx(sWorld->GetDefaultDb2Locale()));

    return true;
}

bool Pet::CreateBaseAtCreatureInfo(CreatureTemplate const* cinfo, Unit* owner)
{
    auto& l_Phases = owner->GetPhases();
    if (!CreateBaseAtTamed(cinfo, owner->GetMap(), owner->GetPhaseMask(), l_Phases))
        return false;

    if (CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(cinfo->family))
        SetName(cFamily->Name->Get(sWorld->GetDefaultDb2Locale()));

    Relocate(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), owner->GetOrientation());

    return true;
}

bool Pet::CreateBaseAtTamed(CreatureTemplate const* cinfo, Map* map, uint32 phaseMask, std::set<uint32> const& p_Phases)
{
    sLog->outDebug(LOG_FILTER_PETS, "Pet::CreateBaseForTamed");
    uint32 guid = sObjectMgr->GenerateLowGuid(HIGHGUID_PET);
    if (!Create(guid, map, phaseMask, p_Phases, cinfo->Entry))
        return false;

    setPowerType(POWER_FOCUS);
    SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
    SetUInt32Value(UNIT_FIELD_PET_EXPERIENCE, 0);
    SetUInt32Value(UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE, uint32(sObjectMgr->GetXPForLevel(getLevel()+1)*PET_XP_FACTOR));
    SetUInt32Value(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_NONE);

    if (cinfo->type == CREATURE_TYPE_BEAST)
    {
        SetByteValue(UNIT_FIELD_SEX, UNIT_BYTES_0_OFFSET_CLASS, CLASS_WARRIOR);
        SetGender(GENDER_NONE);
        setPowerType(POWER_FOCUS);
        SetSheath(SHEATH_STATE_MELEE);
        SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PET_FLAGS, UNIT_CAN_BE_RENAMED | UNIT_CAN_BE_ABANDONED);
    }

    return true;
}

// TODO: Move stat mods code to pet passive auras
bool Guardian::InitStatsForLevel(uint8 p_PetLevel)
{
    CreatureTemplate const* l_CreatureTemplate = GetCreatureTemplate();
    ASSERT(l_CreatureTemplate);

    SetLevel(p_PetLevel);

    // Determine pet type
    PetType l_PetType = MAX_PET_TYPE;

    Unit* l_Owner = GetSummoner();
    if (l_Owner && isPet() && l_Owner->IsPlayer())
    {
        switch (l_Owner->getClass())
        {
            case CLASS_WARLOCK:
            case CLASS_MONK:
            case CLASS_SHAMAN:          // Fire Elemental
            case CLASS_PRIEST:          // Shadowfiend and Mindbender
            case CLASS_DEATH_KNIGHT:    // Risen Ghoul
            case CLASS_MAGE:            // Water Elemental
                l_PetType = SUMMON_PET;
                break;
            case CLASS_HUNTER:
                l_PetType = HUNTER_PET;
                m_unitTypeMask |= UNIT_MASK_HUNTER_PET;
                break;
            default:
                sLog->outError(LOG_FILTER_PETS, "Unknown type pet %u is summoned by player class %u", GetEntry(), l_Owner->getClass());
                break;
        }
    }

    uint32 l_PetStatEntry = l_CreatureTemplate->Entry;
    if (l_PetType == HUNTER_PET)
        l_PetStatEntry = 1;

    PetStatInfo const* l_PetStat = sObjectMgr->GetPetStatInfo(l_PetStatEntry);

    // Default stat
    if (l_PetStat == nullptr)
        l_PetStat = sObjectMgr->GetPetStatInfo(0);

    SetMeleeDamageSchool(SpellSchools(l_CreatureTemplate->dmgschool));

    SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, (l_Owner ? l_Owner->GetArmor() : 1) * l_PetStat->m_ArmorCoef);

    uint32 l_AttackTime = l_PetStat->m_AttackSpeed * IN_MILLISECONDS;

    if (l_Owner)
    {
        if (Player* l_Player = l_Owner->ToPlayer())
            l_AttackTime *= l_Player->GetFloatValue(UNIT_FIELD_MOD_HASTE);
    }

    SetAttackTime(WeaponAttackType::BaseAttack,     l_AttackTime);
    SetAttackTime(WeaponAttackType::OffAttack,      l_AttackTime);
    SetAttackTime(WeaponAttackType::RangedAttack,   l_AttackTime);

    SetFloatValue(UNIT_FIELD_MOD_TIME_RATE,                         1.0f);
    SetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED,                     1.0f);
    SetFloatValue(UNIT_FIELD_MOD_SPELL_HASTE,                       1.0f);
    SetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER,                 1.0f);
    SetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER,             0.0f);
    SetFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER, 0.0f);

    if (l_PetType == HUNTER_PET)
    {
        if (sObjectMgr->GetCreatureModelInfo(GetDisplayId()) && l_Owner)
        {
            SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, l_Owner->GetFloatValue(UNIT_FIELD_BOUNDING_RADIUS));
            SetFloatValue(UNIT_FIELD_COMBAT_REACH, l_Owner->GetFloatValue(UNIT_FIELD_COMBAT_REACH));
        }
    }

    // Speed
    for (uint8 l_I = 0; l_I < MAX_MOVE_TYPE; ++l_I)
        SetSpeed(UnitMoveType(l_I), l_PetStat->m_Speed, true);

    // Resistance
    for (uint8 l_I = SPELL_SCHOOL_HOLY; l_I < MAX_SPELL_SCHOOL; ++l_I)
        SetModifierValue(UnitMods(UNIT_MOD_RESISTANCE_START + l_I), BASE_VALUE, float(l_CreatureTemplate->resistance[l_I]));

    Powers l_PetPower    = l_PetStat->m_Power;
    uint32 l_CreatePower = 0;

    if (l_PetStat->m_CreatePower == -1)
        l_CreatePower = GetCreatePowers(l_PetStat->m_Power);
    else
    {
        // Negative number, it's fix value
        if (l_PetStat->m_CreatePower < 0.0f)
            l_CreatePower = l_PetStat->m_CreatePower * -1;
        // Positive number, it's percentage of owner power
        else if (l_Owner != nullptr)
            l_CreatePower = float(l_Owner->GetMaxPower(l_Owner->getPowerType()) * l_PetStat->m_CreatePower);
    }

    SetCreateMana(l_PetPower == Powers::POWER_MANA ? l_CreatePower : 0);

    setPowerType(l_PetStat->m_Power);
    SetMaxPower(l_PetPower, l_CreatePower);
    SetPower(l_PetPower, l_CreatePower);

    // Base physical damage are 0-1 for every pet since WoD
    SetBaseWeaponDamage(WeaponAttackType::BaseAttack, MINDAMAGE, 1);
    SetBaseWeaponDamage(WeaponAttackType::BaseAttack, MAXDAMAGE, 1);

    if (l_PetType == HUNTER_PET && !ToPet()->m_Stampeded)
        SetUInt32Value(UNIT_FIELD_PET_NEXT_LEVEL_EXPERIENCE, uint32(sObjectMgr->GetXPForLevel(p_PetLevel) * PET_XP_FACTOR));

    if (l_Owner != nullptr)
    {
        SetCreateHealth(l_Owner->GetMaxHealth() * l_PetStat->m_HealthCoef);
        SetMaxHealth(l_Owner->GetMaxHealth() * l_PetStat->m_HealthCoef);
    }

    UpdateAllStats();

    SetFullHealth();

    if (GetEntry() == DeathKnightPet::Ghoul)
    {
        if (l_Owner && l_Owner->HasAura(58640))  ///< Glyph of the Geist
            CastSpell(this, 121916, true);
        if (l_Owner && l_Owner->HasAura(146652)) ///< Glyph of the Skeleton
            CastSpell(this, 147157, true);
    }

    if (l_PetType == HUNTER_PET)
    {
        if (l_Owner && l_Owner->HasAura(152244) && HasAura(20782))
        {
            RemoveAura(20782);
            CastSpell(this, 156843, true); ///<  Adaptation
        }
        CastSpell(this, 88680, true, nullptr, nullptr, l_Owner ? l_Owner->GetGUID() : 0); ///< Kindred Spirits
    }

    return true;
}

bool Pet::HaveInDiet(ItemTemplate const* item) const
{
    if (item->SubClass != ITEM_SUBCLASS_FOOD_DRINK)
        return false;

    CreatureTemplate const* cInfo = GetCreatureTemplate();
    if (!cInfo)
        return false;

    CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(cInfo->family);
    if (!cFamily)
        return false;

    uint32 diet = cFamily->petFoodMask;
    uint32 FoodMask = 1 << (item->FoodType-1);
    return diet & FoodMask;
}

uint32 Pet::GetCurrentFoodBenefitLevel(uint32 itemlevel)
{
    // -5 or greater food level
    if (getLevel() <= itemlevel + 5)                         //possible to feed level 60 pet with level 55 level food for full effect
        return 35000;
    // -10..-6
    else if (getLevel() <= itemlevel + 10)                   //pure guess, but sounds good
        return 17000;
    // -14..-11
    else if (getLevel() <= itemlevel + 14)                   //level 55 food gets green on 70, makes sense to me
        return 8000;
    // -15 or less
    else
        return 0;                                           //food too low level
}

void Pet::_LoadSpellCooldowns(PreparedQueryResult resultCooldown)
{
    m_CreatureSpellCooldowns.clear();
    m_CreatureCategoryCooldowns.clear();

    PreparedQueryResult result = resultCooldown;
    Player* owner = GetOwner() ? GetOwner()->ToPlayer() : NULL;

    if (result)
    {
        time_t curTime = time(NULL);
        ObjectGuid petGuid = GetGUID();

        do
        {
            Field* fields = result->Fetch();

            uint32 spell_id = fields[0].GetUInt32();
            time_t db_time  = time_t(fields[1].GetUInt32());

            if (!sSpellMgr->GetSpellInfo(spell_id))
            {
                sLog->outError(LOG_FILTER_PETS, "Pet %u have unknown spell %u in `pet_spell_cooldown`, skipping.", m_charmInfo->GetPetNumber(), spell_id);
                continue;
            }

            // skip outdated cooldown
            if (db_time <= curTime)
                continue;

            if (owner)
                owner->SendSpellCooldown(spell_id, 0, uint32(db_time - curTime) * IN_MILLISECONDS);

            _AddCreatureSpellCooldown(spell_id, db_time);

            sLog->outDebug(LOG_FILTER_PETS, "Pet (Number: %u) spell %u cooldown loaded (%u secs).", m_charmInfo->GetPetNumber(), spell_id, uint32(db_time-curTime));
        }
        while (result->NextRow());
    }
}

void Pet::_SaveSpellCooldowns(SQLTransaction& trans)
{
#ifdef CROSS
    if (!GetOwner())
        return;

    InterRealmDatabasePool* l_Database = GetOwner()->GetRealmDatabase();
#else
    auto l_Database = &CharacterDatabase;
#endif

    PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_DEL_PET_SPELL_COOLDOWNS);
    stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
    trans->Append(stmt);

    time_t curTime = time(NULL);

    // remove oudated and save active
    for (CreatureSpellCooldowns::iterator itr = m_CreatureSpellCooldowns.begin(); itr != m_CreatureSpellCooldowns.end();)
    {
        if (itr->second <= curTime)
            m_CreatureSpellCooldowns.erase(itr++);
        else
        {
            stmt = l_Database->GetPreparedStatement(CHAR_INS_PET_SPELL_COOLDOWN);
            stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
            stmt->setUInt32(1, itr->first);
            stmt->setUInt32(2, uint32(itr->second));
            trans->Append(stmt);

            ++itr;
        }
    }
}

void Pet::_LoadSpells(PreparedQueryResult resultSpell)
{
    PreparedQueryResult result = resultSpell;

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            AddSpell(fields[0].GetUInt32(), ActiveStates(fields[1].GetUInt8()), PETSPELL_UNCHANGED);
        }
        while (result->NextRow());
    }
}

void Pet::_SaveSpells(SQLTransaction& trans)
{
#ifdef CROSS
    if (!GetOwner())
        return;

    InterRealmDatabasePool* l_Database = GetOwner()->GetRealmDatabase();
#else
    auto l_Database = &CharacterDatabase;
#endif

    for (PetSpellMap::iterator itr = m_spells.begin(), next = m_spells.begin(); itr != m_spells.end(); itr = next)
    {
        ++next;

        // prevent saving family passives to DB
        if (itr->second.type == PETSPELL_FAMILY)
            continue;

        PreparedStatement* stmt;

        switch (itr->second.state)
        {
            case PETSPELL_REMOVED:
                stmt = l_Database->GetPreparedStatement(CHAR_DEL_PET_SPELL_BY_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
                stmt->setUInt32(1, itr->first);
                trans->Append(stmt);

                itr = m_spells.erase(itr);
                continue;
            case PETSPELL_CHANGED:
                stmt = l_Database->GetPreparedStatement(CHAR_DEL_PET_SPELL_BY_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
                stmt->setUInt32(1, itr->first);
                trans->Append(stmt);

                stmt = l_Database->GetPreparedStatement(CHAR_INS_PET_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
                stmt->setUInt32(1, itr->first);
                stmt->setUInt8(2, itr->second.active);
                trans->Append(stmt);

                break;
            case PETSPELL_NEW:
                stmt = l_Database->GetPreparedStatement(CHAR_DEL_PET_SPELL_BY_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
                stmt->setUInt32(1, itr->first);
                trans->Append(stmt);

                stmt = l_Database->GetPreparedStatement(CHAR_INS_PET_SPELL);
                stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
                stmt->setUInt32(1, itr->first);
                stmt->setUInt8(2, itr->second.active);
                trans->Append(stmt);
                break;
            case PETSPELL_UNCHANGED:
                continue;
        }
        itr->second.state = PETSPELL_UNCHANGED;
    }
}

void Pet::_LoadAuras(PreparedQueryResult auraResult, PreparedQueryResult auraEffectResult, uint32 timediff)
{
    sLog->outDebug(LOG_FILTER_PETS, "Loading auras for pet %u", GetGUIDLow());

    PreparedQueryResult result = auraResult;
    PreparedQueryResult resultEffect = auraEffectResult;

    std::list<auraEffectData> auraEffectList;
    if (resultEffect)
    {
        do
        {
            Field* fields = resultEffect->Fetch();
            uint8 slot = fields[0].GetUInt8();
            uint8 effect = fields[1].GetUInt8();
            uint32 amount = fields[2].GetUInt32();
            uint32 baseamount = fields[3].GetUInt32();

            auraEffectList.push_back(auraEffectData(slot, effect, amount, baseamount));
        }
        while (resultEffect->NextRow());
    }

    if (result)
    {
        do
        {
            int32 damage[32];
            int32 baseDamage[32];
            Field* fields = result->Fetch();
            uint8 slot = fields[0].GetUInt8();
            uint64 caster_guid = fields[1].GetUInt64();
            // NULL guid stored - pet is the caster of the spell - see Pet::_SaveAuras
            if (!caster_guid)
                caster_guid = GetGUID();
            uint32 spellid = fields[2].GetUInt32();
            uint32 effmask = fields[3].GetUInt32();
            uint32 recalculatemask = fields[4].GetUInt32();
            uint8 stackcount = fields[5].GetUInt8();
            int32 maxduration = fields[6].GetInt32();
            int32 remaintime = fields[7].GetInt32();
            uint8 remaincharges = fields[8].GetUInt8();

            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellid);
            if (!spellInfo)
            {
                sLog->outError(LOG_FILTER_PETS, "Unknown aura (spellid %u), ignore.", spellid);
                continue;
            }

            // Does not need to load Aura Into Phase 2 if the player isn't in Black Temple Scenario
            if (spellInfo->Id == 80224 && GetOwner() && GetOwner()->GetMapId() != 1112)
                continue;

            // negative effects should continue counting down after logout
            if (remaintime != -1 && !spellInfo->IsPositive())
            {
                if (remaintime/IN_MILLISECONDS <= int32(timediff))
                    continue;

                remaintime -= timediff*IN_MILLISECONDS;
            }

            // prevent wrong values of remaincharges
            if (spellInfo->ProcCharges)
            {
                if (remaincharges <= 0 || remaincharges > spellInfo->ProcCharges)
                    remaincharges = spellInfo->ProcCharges;
            }
            else
                remaincharges = 0;

            for (std::list<auraEffectData>::iterator itr = auraEffectList.begin(); itr != auraEffectList.end(); ++itr)
            {
                if (itr->_slot == slot)
                {
                    damage[itr->_effect] = itr->_amount;
                    baseDamage[itr->_effect] = itr->_baseamount;
                }
            }

            Aura* aura = Aura::TryCreate(spellInfo, effmask, this, NULL, &baseDamage[0], NULL, caster_guid);
            if (aura != nullptr)
            {
                if (!aura->CanBeSaved())
                {
                    aura->Remove();
                    continue;
                }
                aura->SetLoadedState(maxduration, remaintime, remaincharges, stackcount, recalculatemask, &damage[0]);
                aura->ApplyForTargets();
                sLog->outInfo(LOG_FILTER_PETS, "Added aura spellid %u, effectmask %u", spellInfo->Id, effmask);
            }
        }
        while (result->NextRow());
    }
}

void Pet::_SaveAuras(SQLTransaction& trans)
{
#ifdef CROSS
    if (!GetOwner())
        return;

    InterRealmDatabasePool* l_Database = GetOwner()->GetRealmDatabase();
#else
    auto l_Database = &CharacterDatabase;
#endif


    PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_DEL_PET_AURAS);
    stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
    trans->Append(stmt);

    stmt = l_Database->GetPreparedStatement(CHAR_DEL_PET_AURAS_EFFECTS);
    stmt->setUInt32(0, m_charmInfo->GetRealmPetNumber());
    trans->Append(stmt);

    for (AuraMap::const_iterator itr = m_ownedAuras.begin(); itr != m_ownedAuras.end(); ++itr)
    {
        // check if the aura has to be saved
        if (!itr->second->CanBeSaved() || IsPetAura(itr->second))
            continue;

        Aura* aura = itr->second;
        AuraApplication * foundAura = GetAuraApplication(aura->GetId(), aura->GetCasterGUID(), aura->GetCastItemGUID());

        if (!foundAura)
            continue;

        int32 damage[SpellEffIndex::MAX_EFFECTS];
        int32 baseDamage[SpellEffIndex::MAX_EFFECTS];
        uint32 effMask = 0;
        uint32 recalculateMask = 0;
        uint8 index = 0;
        for (uint8 i = 0; i < aura->GetEffectCount(); ++i)
        {
            if (aura->GetEffect(i))
            {
                index = 0;
                stmt = l_Database->GetPreparedStatement(CHAR_INS_PET_AURA_EFFECT);
                stmt->setUInt32(index++, m_charmInfo->GetRealmPetNumber());
                stmt->setUInt8(index++, foundAura->GetSlot());
                stmt->setUInt8(index++, i);
                stmt->setInt32(index++, aura->GetEffect(i)->GetBaseAmount());
                stmt->setInt32(index++, aura->GetEffect(i)->GetAmount());

                trans->Append(stmt);

                baseDamage[i] = aura->GetEffect(i)->GetBaseAmount();
                damage[i] = aura->GetEffect(i)->GetAmount();
                effMask |= (1<<i);
                if (aura->GetEffect(i)->CanBeRecalculated())
                    recalculateMask |= (1<<i);
            }
            else
            {
                baseDamage[i] = 0;
                damage[i] = 0;
            }
        }

        // don't save guid of caster in case we are caster of the spell - guid for pet is generated every pet load, so it won't match saved guid anyways
        uint64 casterGUID = (itr->second->GetCasterGUID() == GetGUID()) ? 0 : itr->second->GetCasterGUID();

        index = 0;
        PreparedStatement* stmt = l_Database->GetPreparedStatement(CHAR_INS_PET_AURA);
        stmt->setUInt32(index++, m_charmInfo->GetRealmPetNumber());
        stmt->setUInt8(index++, foundAura->GetSlot());
        stmt->setUInt64(index++, casterGUID);
        stmt->setUInt32(index++, itr->second->GetId());
        stmt->setUInt8(index++, effMask);
        stmt->setUInt8(index++, recalculateMask);
        stmt->setUInt8(index++, itr->second->GetStackAmount());
        stmt->setInt32(index++, itr->second->GetMaxDuration());
        stmt->setInt32(index++, itr->second->GetDuration());
        stmt->setUInt8(index++, itr->second->GetCharges());

        trans->Append(stmt);
    }
}


bool Pet::AddSpell(uint32 p_SpellID, ActiveStates p_ActiveState /*= ActiveStates::ACT_DECIDE*/, PetSpellState p_PetSpellState /*= PetSpellState::PETSPELL_NEW*/, PetSpellType p_PetSpellType /*= PetSpellType::PETSPELL_NORMAL*/)
{
    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellID);
    if (!l_SpellInfo)
        return false;

    PetSpellMap::iterator l_It = m_spells.find(p_SpellID);
    if (l_It != m_spells.end())
    {
        if (l_It->second.state == PETSPELL_REMOVED)
        {
            m_spells.erase(l_It);
            p_PetSpellState = PETSPELL_CHANGED;
        }
        else if (p_PetSpellState == PETSPELL_UNCHANGED && l_It->second.state != PETSPELL_UNCHANGED)
        {
            /// Can be in case spell loading but learned at some previous spell loading
            l_It->second.state = PETSPELL_UNCHANGED;

            if (p_ActiveState == ACT_ENABLED)
                ToggleAutocast(l_SpellInfo, true);
            else if (p_ActiveState == ACT_DISABLED)
                ToggleAutocast(l_SpellInfo, false);

            return false;
        }
        else
            return false;
    }

    PetSpell l_NewSpell;
    l_NewSpell.state = p_PetSpellState;
    l_NewSpell.type = p_PetSpellType;

    if (p_ActiveState == ACT_DECIDE)                               // active was not used before, so we save it's autocast/passive state here
    {
        if (l_SpellInfo->IsAutocastable())
            l_NewSpell.active = ACT_DISABLED;
        else
            l_NewSpell.active = ACT_PASSIVE;
    }
    else
        l_NewSpell.active = p_ActiveState;

    if (l_SpellInfo->IsRanked())
    {
        for (PetSpellMap::const_iterator l_It = m_spells.begin(); l_It != m_spells.end(); ++l_It)
        {
            if (l_It->second.state == PETSPELL_REMOVED)
                continue;

            SpellInfo const* l_OldRankSpellInfo = sSpellMgr->GetSpellInfo(l_It->first);

            if (!l_OldRankSpellInfo)
                continue;

            if (l_SpellInfo->IsDifferentRankOf(l_OldRankSpellInfo))
            {
                /// Replace by new high rank
                if (l_SpellInfo->IsHighRankOf(l_OldRankSpellInfo))
                {
                    l_NewSpell.active = l_It->second.active;

                    if (l_NewSpell.active == ACT_ENABLED)
                        ToggleAutocast(l_OldRankSpellInfo, false);

                    UnlearnSpell(l_It->first, false, false);
                    break;
                }
                /// Ignore new lesser rank
                else
                    return false;
            }
        }
    }

    m_spells[p_SpellID] = l_NewSpell;

    if (l_SpellInfo->IsPassive() && (!l_SpellInfo->CasterAuraState || HasAuraState(AuraStateType(l_SpellInfo->CasterAuraState))))
        CastSpell(this, p_SpellID, true);
    else
        m_charmInfo->AddSpellToActionBar(l_SpellInfo);

    if (l_NewSpell.active == ACT_ENABLED)
        ToggleAutocast(l_SpellInfo, true);

    return true;
}
/// Learn spell for the pet
/// @p_SpellID : Spell ID to learn
bool Pet::LearnSpell(uint32 p_SpellID)
{
    /// Prevent duplicated entires in spell book
    if (!AddSpell(p_SpellID))
        return false;

    if (!m_loading)
    {
        WorldPacket l_Data(SMSG_PET_LEARNED_SPELLS, 4 + 4);
        l_Data << uint32(1);            ///< Count
        l_Data << uint32(p_SpellID);    ///< SpellID

        m_owner->GetSession()->SendPacket(&l_Data);
        m_owner->PetSpellInitialize();
    }

    return true;
}
/// Learn spells for the pet
/// @p_SpellsToLearn : Spell IDs to learn
void Pet::LearnSpells(std::unordered_set<uint32> const& p_SpellsToLearn)
{
    std::unordered_set<uint32> l_SpellToSend;
    l_SpellToSend.reserve(p_SpellsToLearn.size());  ///< Reserve memory

    for (uint32 l_CurrentSpellID : p_SpellsToLearn)
    {
        if (!AddSpell(l_CurrentSpellID))
            continue;

        l_SpellToSend.emplace(l_CurrentSpellID);
    }

    /// If we learned new spells, we send them
    if (!l_SpellToSend.empty() && !m_loading)
    {
        WorldPacket l_Data(SMSG_PET_LEARNED_SPELLS, 4 + (4 * l_SpellToSend.size()));
        l_Data << uint32(l_SpellToSend.size());     ///< Count

        for (uint32 l_CurrentSpellID : l_SpellToSend)
            l_Data << uint32(l_CurrentSpellID);    ///< SpellID

        m_owner->GetSession()->SendPacket(&l_Data);
        m_owner->PetSpellInitialize();
    }
}

/// Remove a spell for the pet
/// @p_SpellID          : Spell ID to unlearn
/// @p_LearnPrev        : Should learn old rank on delete ?
/// @p_ClearActionBar   : Clean action bar at remove ?
bool Pet::RemoveSpell(uint32 spell_id, bool p_LearnPrev, bool p_ClearActionBar)
{
    PetSpellMap::iterator l_It = m_spells.find(spell_id);
    if (l_It == m_spells.end())
        return false;

    if (l_It->second.state == PETSPELL_REMOVED)
        return false;

    if (l_It->second.state == PETSPELL_NEW)
        m_spells.erase(l_It);
    else
        l_It->second.state = PETSPELL_REMOVED;

    RemoveAurasDueToSpell(spell_id);

    if (p_LearnPrev)
    {
        if (uint32 l_PrevSpellID = sSpellMgr->GetPrevSpellInChain(spell_id))
            LearnSpell(l_PrevSpellID);
        else
            p_LearnPrev = false;
    }

    /// If remove last rank or non-ranked then update action bar at server and client if need
    if (p_ClearActionBar && !p_LearnPrev && m_charmInfo->RemoveSpellFromActionBar(spell_id))
    {
        if (!m_loading)
        {
            /// Need update action bar for last removed rank
            if (m_owner && m_owner->IsPlayer())
                m_owner->ToPlayer()->PetSpellInitialize();
        }
    }

    return true;
}
/// Unlearn a spell for the pet
/// @p_SpellID          : Spell ID to unlearn
/// @p_LearnPrev        : Should learn old rank on delete ?
/// @p_ClearActionBar   : Clean action bar at remove ?
bool Pet::UnlearnSpell(uint32 p_SpellID, bool p_LearnPrev, bool p_ClearActionBar)
{
    if (RemoveSpell(p_SpellID, p_LearnPrev, p_ClearActionBar))
    {
        if (!m_loading)
        {
            WorldPacket l_Data(SMSG_PET_UNLEARNED_SPELLS, 4 + 4);
            l_Data << uint32(1);            ///< count
            l_Data << uint32(p_SpellID);    ///< SpellID

            m_owner->GetSession()->SendPacket(&l_Data);
        }

        return true;
    }

    return false;
}
/// Unlearn a set of spell for the pet
/// @p_SpellsToUnlearn  : Spells ID to unlearn
/// @p_LearnPrev        : Should learn old rank on delete ?
/// @p_ClearActionBar   : Clean action bar at remove ?
void Pet::UnlearnSpells(std::unordered_set<uint32> const& p_SpellsToUnlearn, bool p_LearnPrev, bool p_ClearActionBar)
{
    std::unordered_set<uint32> l_SpellToSend;
    l_SpellToSend.reserve(p_SpellsToUnlearn.size());    ///< Reserve memory

    for (uint32 l_CurrentSpellID : p_SpellsToUnlearn)
    {
        /// p_ClearActionBar is overrides here to clean spell bar only once
        if (!RemoveSpell(l_CurrentSpellID, p_LearnPrev, false))
            continue;

        l_SpellToSend.emplace(l_CurrentSpellID);
    }

    /// If we unlearned old spells, we send them
    if (!l_SpellToSend.empty() && !m_loading)
    {
        WorldPacket l_Data(SMSG_PET_UNLEARNED_SPELLS, 4 + (4 * l_SpellToSend.size()));
        l_Data << uint32(l_SpellToSend.size());         ///< Count

        for (uint32 l_CurrentSpellID : l_SpellToSend)
            l_Data << uint32(l_CurrentSpellID);         ///< SpellID

        m_owner->GetSession()->SendPacket(&l_Data);

        /// Clean action bar once
        if (p_ClearActionBar)
            m_owner->PetSpellInitialize();
    }
}

void Pet::InitLevelupSpellsForLevel()
{
    uint8 l_Level = getLevel();

    if (PetLevelupSpellSet const* l_LevelupSpells = GetCreatureTemplate()->family ? sSpellMgr->GetPetLevelupSpellList(GetCreatureTemplate()->family) : NULL)
    {
        /// PetLevelupSpellSet ordered by levels, process in reversed order
        for (PetLevelupSpellSet::const_reverse_iterator l_It = l_LevelupSpells->rbegin(); l_It != l_LevelupSpells->rend(); ++l_It)
        {
            SpellInfo const* l_SpellEntry = sSpellMgr->GetSpellInfo(l_It->second);
            if (!l_SpellEntry)
                continue;

            if (l_SpellEntry->IsHiddenForPetEntry(GetEntry()))
            {
                if (HasSpell(l_SpellEntry->Id))
                    UnlearnSpell(l_SpellEntry->Id, false, true);

                continue;
            }

            /// Will called first if level down
            if (l_It->first > l_Level)
                UnlearnSpell(l_It->second, true);                    ///< Will learn prev rank if any
            /// Will called if level up
            else
                LearnSpell(l_SpellEntry->Id);                            ///< Will unlearn prev rank if any
        }
    }

    // default spells (can be not learned if pet level (as owner level decrease result for example) less first possible in normal game)
    if (PetDefaultSpellsEntry const* l_DefaultSpells = sSpellMgr->GetPetDefaultSpellsEntry(int32(GetEntry())))
    {
        for (uint8 l_I = 0; l_I < MAX_CREATURE_SPELL_DATA_SLOT; ++l_I)
        {
            SpellInfo const* l_SpellEntry = sSpellMgr->GetSpellInfo(l_DefaultSpells->spellid[l_I]);
            if (!l_SpellEntry)
                continue;

            if (l_SpellEntry->IsHiddenForPetEntry(GetEntry()))
            {
                if (HasSpell(l_SpellEntry->Id))
                    UnlearnSpell(l_SpellEntry->Id, false, true);

                continue;
            }

            /// This prevent to add spells with no cooldown - cheating !
            if (!l_SpellEntry->RecoveryTime && !l_SpellEntry->StartRecoveryCategory &&
                !l_SpellEntry->StartRecoveryTime && !l_SpellEntry->CategoryRecoveryTime)
                continue;

            /// Will called first if level down
            if (l_SpellEntry->SpellLevel > l_Level)
                UnlearnSpell(l_SpellEntry->Id, true);
            /// Will called if level up
            else
                LearnSpell(l_SpellEntry->Id);
        }
    }
}

void Pet::CleanupActionBar()
{
    for (uint8 i = 0; i < MAX_UNIT_ACTION_BAR_INDEX; ++i)
    {
        if (UnitActionBarEntry const* ab = m_charmInfo->GetActionBarEntry(i))
        {
            if (ab->GetAction() && ab->IsActionBarForSpell())
            {
                if (!HasSpell(ab->GetAction()))
                    m_charmInfo->SetActionBar(i, 0, ACT_PASSIVE);
                else if (ab->GetType() == ACT_ENABLED)
                {
                    if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(ab->GetAction()))
                        ToggleAutocast(spellInfo, true);
                }
            }
        }
    }
}

void Pet::InitPetCreateSpells()
{
    m_charmInfo->InitPetActionBar();
    m_spells.clear();

    LearnPetPassives();
    InitLevelupSpellsForLevel();

    CastPetAuras(false);
}

void Pet::ToggleAutocast(SpellInfo const* spellInfo, bool apply)
{
    if (!spellInfo->IsAutocastable())
        return;

    uint32 spellid = spellInfo->Id;

    PetSpellMap::iterator itr = m_spells.find(spellid);
    if (itr == m_spells.end())
        return;

    uint32 i;

    if (apply)
    {
        for (i = 0; i < m_autospells.size() && m_autospells[i] != spellid; ++i)
            ;                                               // just search

        if (i == m_autospells.size())
        {
            m_autospells.push_back(spellid);

            if (itr->second.active != ACT_ENABLED)
            {
                itr->second.active = ACT_ENABLED;
                if (itr->second.state != PETSPELL_NEW)
                    itr->second.state = PETSPELL_CHANGED;
            }
        }
    }
    else
    {
        AutoSpellList::iterator itr2 = m_autospells.begin();
        for (i = 0; i < m_autospells.size() && m_autospells[i] != spellid; ++i, ++itr2)
            ;                                               // just search

        if (i < m_autospells.size())
        {
            m_autospells.erase(itr2);
            if (itr->second.active != ACT_DISABLED)
            {
                itr->second.active = ACT_DISABLED;
                if (itr->second.state != PETSPELL_NEW)
                    itr->second.state = PETSPELL_CHANGED;
            }
        }
    }
}

bool Pet::IsPermanentPetFor(Player* owner)
{
    switch (getPetType())
    {
        case SUMMON_PET:
            switch (owner->getClass())
            {
                case CLASS_WARLOCK:
                    return GetCreatureTemplate()->type == CREATURE_TYPE_DEMON;
                case CLASS_DEATH_KNIGHT:
                    return GetCreatureTemplate()->type == CREATURE_TYPE_UNDEAD;
                case CLASS_MAGE:
                    return GetCreatureTemplate()->type == CREATURE_TYPE_ELEMENTAL;
                default:
                    return false;
            }
        case HUNTER_PET:
            return true;
        default:
            return false;
    }
}

bool Pet::Create(uint32 p_Guidlow, Map* p_Map, uint32 p_PhaseMask, std::set<uint32> const& p_Phases, uint32 p_Entry)
{
    ASSERT(p_Map);
    SetMap(p_Map);

    SetPhaseMask(p_PhaseMask, false);

    for (uint32 l_Phase : p_Phases)
        SetInPhase(l_Phase, false, true);

    Object::_Create(p_Guidlow, p_Entry, HIGHGUID_PET);

    m_DBTableGuid = p_Guidlow;
    m_originalEntry = p_Entry;

    if (!InitEntry(p_Entry))
        return false;

    // Force regen flag for player pets, just like we do for players themselves
    SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
    SetSheath(SHEATH_STATE_MELEE);

    return true;
}

bool Pet::HasSpell(uint32 spell) const
{
    PetSpellMap::const_iterator itr = m_spells.find(spell);
    return itr != m_spells.end() && itr->second.state != PETSPELL_REMOVED;
}

// Get all passive spells in our skill line
void Pet::LearnPetPassives()
{
    CreatureTemplate const* cInfo = GetCreatureTemplate();
    if (!cInfo)
        return;

    CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(cInfo->family);
    if (!cFamily)
        return;

    PetFamilySpellsStore::const_iterator petStore = sPetFamilySpellsStore.find(cFamily->ID);
    if (petStore != sPetFamilySpellsStore.end())
    {
        // For general hunter pets skill 270
        // Passive 01~10, Passive 00 (20782, not used), Ferocious Inspiration (34457)
        // Scale 01~03 (34902~34904, bonus from owner, not used)
        for (PetFamilySpellsSet::const_iterator petSet = petStore->second.begin(); petSet != petStore->second.end(); ++petSet)
            AddSpell(*petSet, ACT_DECIDE, PETSPELL_NEW, PETSPELL_FAMILY);
    }
}

void Pet::CastPetAuras(bool current)
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!IsPermanentPetFor(owner->ToPlayer()))
        return;

    for (PetAuraSet::const_iterator itr = owner->m_petAuras.begin(); itr != owner->m_petAuras.end();)
    {
        PetAura const* pa = *itr;
        ++itr;

        if (!current && pa->IsRemovedOnChangePet())
            owner->RemovePetAura(pa);
        else
            CastPetAura(pa);
    }
}

void Pet::CastPetAura(PetAura const* aura)
{
    uint32 auraId = aura->GetAura(GetEntry());
    if (!auraId)
        return;

    CastSpell(this, auraId, true);
}

bool Pet::IsPetAura(Aura const* aura)
{
    Unit* owner = GetOwner();

    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return false;

    // if the owner has that pet aura, return true
    for (PetAuraSet::const_iterator itr = owner->m_petAuras.begin(); itr != owner->m_petAuras.end(); ++itr)
    {
        if ((*itr)->GetAura(GetEntry()) == aura->GetId())
            return true;
    }
    return false;
}

void Pet::learnSpellHighRank(uint32 spellid)
{
    LearnSpell(spellid);

    if (uint32 next = sSpellMgr->GetNextSpellInChain(spellid))
        learnSpellHighRank(next);
}

void Pet::SynchronizeLevelWithOwner()
{
    Unit* owner = GetOwner();
    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
        return;

    switch (getPetType())
    {
        case SUMMON_PET:
        case HUNTER_PET:
            GivePetLevel(owner->getLevel());
            break;
        default:
            break;
    }
}

void Pet::LearnSpecializationSpells()
{
    std::unordered_set<uint32> l_LearnedSpells;

    if (std::vector<SpecializationSpellEntry const*> const* l_SpecializationSpells = GetSpecializationSpells(GetSpecialization()))
    {
        for (size_t l_J = 0; l_J < l_SpecializationSpells->size(); ++l_J)
        {
            SpecializationSpellEntry const* specSpell = (*l_SpecializationSpells)[l_J];

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(specSpell->SpellID);
            if (!l_SpellInfo || l_SpellInfo->SpellLevel > getLevel())
                continue;

            l_LearnedSpells.emplace(specSpell->SpellID);
        }
    }

    LearnSpells(l_LearnedSpells);
}

void Pet::RemoveSpecializationSpells(bool clearActionBar)
{
    std::unordered_set<uint32> l_UnlearnedSpells;

    for (uint32 l_I = 0; l_I < MAX_SPECIALIZATIONS; ++l_I)
    {
        if (ChrSpecializationsEntry const* l_Specialization = GetChrSpecializationByIndex(0, l_I))
        {
            if (std::vector<SpecializationSpellEntry const*> const* l_SpecializationSpells = GetSpecializationSpells(l_Specialization->ID))
            {
                for (size_t l_J = 0; l_J < l_SpecializationSpells->size(); ++l_J)
                {
                    SpecializationSpellEntry const* specSpell = (*l_SpecializationSpells)[l_J];
                    l_UnlearnedSpells.emplace(specSpell->SpellID);
                }
            }
        }

        if (ChrSpecializationsEntry const* l_Specialization = GetChrSpecializationByIndex(PET_SPEC_OVERRIDE_CLASS_INDEX, l_I))
        {
            if (std::vector<SpecializationSpellEntry const*> const* l_SpecializationSpells = GetSpecializationSpells(l_Specialization->ID))
            {
                for (size_t l_J = 0; l_J < l_SpecializationSpells->size(); ++l_J)
                {
                    SpecializationSpellEntry const* specSpell = (*l_SpecializationSpells)[l_J];
                    l_UnlearnedSpells.emplace(specSpell->SpellID);
                }
            }
        }
    }

    UnlearnSpells(l_UnlearnedSpells, true, clearActionBar);
}

void Pet::SetSpecialization(uint16 spec)
{
    if (GetSpecialization() == spec)
        return;

    // remove all the old spec's specalization spells, set the new spec, then add the new spec's spells
    // clearActionBars is false because we'll be updating the pet actionbar later so we don't have to do it now
    RemoveSpecializationSpells(false);
    if (!sChrSpecializationsStore.LookupEntry(spec))
    {
        m_specialization = 0;
        return;
    }

    m_specialization = spec;
    LearnSpecializationSpells();

    // resend SMSG_PET_SPELLS_MESSAGE to remove old specialization spells from the pet action bar
    CleanupActionBar();
    GetOwner()->PetSpellInitialize();

    WorldPacket data(SMSG_SET_PET_SPECIALIZATION);
    data << uint16(m_specialization);     ///< SpecId

    GetOwner()->GetSession()->SendPacket(&data);
}

void Pet::ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs, uint32 p_SpellID /*= 0*/)
{
    Player* owner = GetOwner();
    time_t curTime = time(NULL);
    for (PetSpellMap::const_iterator itr = m_spells.begin(); itr != m_spells.end(); ++itr)
    {
        if (itr->second.state == PETSPELL_REMOVED)
            continue;
        uint32 unSpellId = itr->first;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(unSpellId);
        ASSERT(spellInfo);

        // Not send cooldown for this spells
        if (spellInfo->IsCooldownStartedOnEvent())
            continue;

        if ((spellInfo->PreventionType & (SpellPreventionMask::Silence)) == 0)
            continue;

        if ((idSchoolMask & spellInfo->GetSchoolMask()) && GetCreatureSpellCooldownDelay(unSpellId) < unTimeMs)
        {
            if (owner)
                owner->SendSpellCooldown(unSpellId, 0, unTimeMs);

            _AddCreatureSpellCooldown(unSpellId, curTime + unTimeMs/IN_MILLISECONDS);
        }
    }
}

bool Pet::NeedToChase(Unit* p_Victim)
{
    float l_Range = 0.0f;
    for (uint32 l_I = 0; l_I < GetPetAutoSpellSize(); ++l_I)
    {
        uint32 spellID = GetPetAutoSpellOnPos(l_I);
        if (!spellID)
            continue;

        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(spellID);
        if (!l_SpellInfo)
            continue;

        if (l_SpellInfo->GetRecoveryTime())
            continue;

        l_Range = l_Range > l_SpellInfo->GetMaxRange() ? l_Range : l_SpellInfo->GetMaxRange();
    }

    l_Range *= 0.80f; ///< So pet don't start to cast when at exact range and get interrupt after cast because target moved 1 step away

    return (!l_Range && !IsWithinMeleeRange(p_Victim)) || !IsWithinDistInMap(p_Victim, l_Range);
}