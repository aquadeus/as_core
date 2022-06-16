////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "World.h"
#include "ObjectMgr.h"
#include "GroupMgr.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "QuestDef.h"
#include "GossipDef.h"
#include "Player.h"
#include "PoolMgr.h"
#include "Opcodes.h"
#include "Log.h"
#include "LootMgr.h"
#include "MapManager.h"
#include "CreatureAI.h"
#include "CreatureAISelector.h"
#include "Formulas.h"
#include "WaypointMovementGenerator.h"
#include "InstanceScript.h"
#include "BattlegroundMgr.hpp"
#include "Util.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "OutdoorPvPMgr.h"
#include "GameEventMgr.h"
#include "CreatureGroups.h"
#include "Vehicle.h"
#include "SpellAuraEffects.h"
#include "Group.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "WildBattlePet.h"
#include "Transport.h"
#include "POIMgr.hpp"
#include "SHA1.h"
#include <algorithm>

#ifndef CROSS
# include "GarrisonNPCAI.hpp"
#endif

TrainerSpell const* TrainerSpellData::Find(uint32 spell_id) const
{
    TrainerSpellMap::const_iterator itr = spellList.find(spell_id);
    if (itr != spellList.end())
        return &itr->second;

    return NULL;
}

bool VendorItemData::RemoveItem(uint32 item_id, uint8 type)
{
    bool found = false;
    for (VendorItemList::iterator i = m_items.begin(); i != m_items.end();)
    {
        if ((*i)->item == item_id && (*i)->Type == type)
        {
            i = m_items.erase(i++);
            found = true;
        }
        else
            ++i;
    }
    return found;
}

VendorItem const* VendorItemData::FindItemCostPair(uint32 item_id, uint32 extendedCost, uint8 type) const
{
    for (VendorItemList::const_iterator i = m_items.begin(); i != m_items.end(); ++i)
        if ((*i)->item == item_id && (*i)->ExtendedCost == extendedCost && (*i)->Type == type)
            return *i;
    return NULL;
}

uint32 CreatureTemplate::GetRandomValidModelId() const
{
    uint8 c = 0;
    uint32 modelIDs[4];

    for (uint32 l_I = 0; l_I < MAX_CREATURE_MODELS; ++l_I)
    {
        if (uint32 l_ModelID = Modelid[l_I])
            modelIDs[c++] = l_ModelID;
    }

    return ((c>0) ? modelIDs[urand(0, c - 1)] : 0);
}

uint32 CreatureTemplate::GetFirstValidModelId() const
{
    for (uint32 l_I = 0; l_I < MAX_CREATURE_MODELS; ++l_I)
    {
        if (uint32 l_ModelID = Modelid[l_I])
            return l_ModelID;
    }

    return 0;
}

bool AssistDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    if (Unit* victim = Unit::GetUnit(m_owner, m_victim))
    {
        while (!m_assistants.empty())
        {
            Creature* assistant = Unit::GetCreature(m_owner, *m_assistants.begin());
            m_assistants.pop_front();

            if (assistant && assistant->CanAssistTo(&m_owner, victim))
            {
                assistant->SetNoCallAssistance(true);
                assistant->CombatStart(victim);
                if (assistant->IsAIEnabled)
                    assistant->AI()->AttackStart(victim);
            }
        }
    }
    return true;
}

CreatureBaseStats const* CreatureBaseStats::GetBaseStats(uint8 level, uint8 unitClass)
{
    return sObjectMgr->GetCreatureBaseStats(level, unitClass);
}

bool ForcedDespawnDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    m_owner.DespawnOrUnsummon();    // since we are here, we are not TempSummon as object type cannot change during runtime
    return true;
}

bool SetPhaseDelayEvent::Execute(uint64 /*p_ETime*/, uint32 /*p_Time*/)
{
    m_Owner.SetPhaseMask(m_Phase, true);
    m_Owner.RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
    m_Owner.SetReactState(ReactStates::REACT_AGGRESSIVE);
    return true;
}

Creature::Creature(bool isWorldObject) : Unit(isWorldObject), MapObject(),
lootForPickPocketed(false), lootForBody(false), m_groupLootTimer(0), m_corpseRemoveTime(0), m_respawnTime(0),
m_respawnDelay(300), m_corpseDelay(60), m_respawnradius(0.0f), m_reactState(REACT_AGGRESSIVE),
m_defaultMovementType(IDLE_MOTION_TYPE), m_DBTableGuid(0), m_equipmentId(0), m_OriginalEquipmentId(0), m_AlreadyCallAssistance(false),
m_AlreadySearchedAssistance(false), m_regenHealth(true), m_AI_locked(false), m_meleeDamageSchoolMask(SPELL_SCHOOL_MASK_NORMAL),
m_creatureInfo(NULL), m_NativeCreatureInfo(nullptr), m_creatureData(NULL), m_CreatureScript(nullptr), m_path_id(0), m_formation(NULL), m_respawnChallenge(0),
m_HasDifficultyEntry(false), m_IsCosmetic(false), m_FlagExtras(0)
{
    m_NeedRespawn = false;
    m_RespawnFrameDelay = 0;

    m_valuesCount = UNIT_END;
    _dynamicValuesCount = UNIT_DYNAMIC_END;

    m_HealthRegenTimer = CREATURE_REGEN_HEALTH_INTERVAL;
    m_RegenPowerTimer  = CREATURE_REGEN_INTERVAL;

    for (uint8 i = 0; i < MAX_CREATURE_SPELLS; ++i)
        m_spells[i] = 0;

    m_CreatureSpellCooldowns.clear();
    m_CreatureCategoryCooldowns.clear();
    DisableReputationGain = false;

    m_SightDistance = sWorld->getFloatConfig(CONFIG_SIGHT_MONSTER);
    m_CombatDistance = 0;//MELEE_RANGE;

    m_LOSCheckTimer = DEFAULT_VISIBILITY_NOTIFY_PERIOD;
    m_LOSCheck_player = false;
    m_LOSCheck_creature = false;

    ResetLootMode(); // restore default loot mode
    TriggerJustRespawned = false;
    m_isTempWorldObject = false;

    m_playerCount        = 0;
    m_StartEncounterTime = 0;
    m_DumpGroupTimer     = 0;

    m_MovingUpdateTimer = 0;
    m_NotMovingUpdateTimer = 0;

    m_BonusLootSpellID = 0;

    m_spawnMode = 0;

    m_BossID = 0;

    m_IsKillable = true;

    m_SkipUpdateCounter = 0;
    m_DelayedUpdateTime = 0;

    m_VignetteID      = 0;
    m_TrackingQuestID = 0;
}

Creature::~Creature()
{
    m_vendorItemCounts.clear();

    delete i_AI;
    i_AI = NULL;
    IsAIEnabled = false;

    //if (m_uint32Values)
    //    sLog->outError(LOG_FILTER_UNITS, "Deconstruct Creature Entry = %u", GetEntry());
}

void Creature::AddToWorld()
{
    ///- Register the creature for guid lookup
    if (!IsInWorld())
    {
        if (m_zoneScript)
        {
            m_zoneScript->OnCreatureCreate(this);
            m_zoneScript->OnCreatureCreateForScript(this);
        }
        sObjectAccessor->AddObject(this);
        Unit::AddToWorld();
        SearchFormation();
        AIM_Initialize();
        if (IsVehicle())
            GetVehicleKit()->Install();
    }
}

void Creature::RemoveFromWorld()
{
    if (IsInWorld())
    {
        /// InstanceMap::i_data (m_zoneScript pointer) is deleted in InstanceMap::~InstanceMap()
        /// Creature::RemoveFromWorld() can be called by Map::~Map() after i_data is deleted
        /// So we have to update it otherwise m_zoneScript can be a deleted pointer!
        SetZoneScript();

        if (m_zoneScript)
        {
            m_zoneScript->OnCreatureRemove(this);
            m_zoneScript->OnCreatureRemoveForScript(this);
        }
        if (m_formation)
            sFormationMgr->RemoveCreatureFromGroup(m_formation, this);
        Unit::RemoveFromWorld();
        sObjectAccessor->RemoveObject(this);
    }
}

void Creature::DisappearAndDie()
{
    DestroyForNearbyPlayers();
    //SetVisibility(VISIBILITY_OFF);
    //ObjectAccessor::UpdateObjectVisibility(this);
    if (isAlive())
        setDeathState(JUST_DIED);
    RemoveCorpse(false);
}

void Creature::SearchFormation()
{
    if (isSummon())
        return;

    uint32 lowguid = GetDBTableGUIDLow();
    if (!lowguid)
        return;

    CreatureGroupInfoType::iterator frmdata = sFormationMgr->CreatureGroupMap.find(lowguid);
    if (frmdata != sFormationMgr->CreatureGroupMap.end())
        sFormationMgr->AddCreatureToGroup(frmdata->second->leaderGUID, this);
}

void Creature::RemoveCorpse(bool setSpawnTime, bool p_HomePosAsRespawn /*= false*/)
{
    if (getDeathState() != CORPSE)
        return;

    m_corpseRemoveTime = time(NULL);
    setDeathState(DEAD);
    RemoveAllAuras();
    UpdateObjectVisibility();
    ClearSaveThreatTarget();
    ClearLootContainers();
    uint32 respawnDelay = m_respawnDelay;
    if (IsAIEnabled)
        AI()->CorpseRemoved(respawnDelay);

    // Should get removed later, just keep "compatibility" with scripts
    if (setSpawnTime)
        m_respawnTime = time(NULL) + respawnDelay;

    float x, y, z, o;

    if (p_HomePosAsRespawn)
        GetHomePosition(x, y, z, o);
    else
        GetRespawnPosition(x, y, z, &o);

    SetHomePosition(x, y, z, o);
    GetMap()->CreatureRelocation(this, x, y, z, o);
}

/**
 * change the entry of creature until respawn
 */
bool Creature::InitEntry(uint32 Entry, uint32 /*team*/, const CreatureData* data)
{
    CreatureTemplate const* normalInfo = sObjectMgr->GetCreatureTemplate(Entry);
    if (!normalInfo)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature::InitEntry creature entry %u does not exist.", Entry);
        return false;
    }

    m_NativeCreatureInfo = normalInfo;

    for (uint8 l_I = 0; l_I < Difficulty::MaxDifficulties; ++l_I)
    {
        if (m_NativeCreatureInfo->DifficultyEntry[l_I] != 0)
        {
            m_HasDifficultyEntry = true;
            break;
        }
    }

    // get difficulty 1 mode entry
    // Si l'entry heroic du mode de joueur est introuvable, on utilise l'entry du mode normal correpondant au nombre de joueurs du mode
    CreatureTemplate const* cinfo = nullptr;
    DifficultyEntry const* difficultyEntry = sDifficultyStore.LookupEntry(GetMap()->GetSpawnMode());

    while (!cinfo && difficultyEntry)
    {
        int32 idx = difficultyEntry->ID - 1;
        if (idx == -1)
            break;
        if (normalInfo->DifficultyEntry[idx])
        {
            cinfo = sObjectMgr->GetCreatureTemplate(normalInfo->DifficultyEntry[idx]);
            break;
        }
        if (!difficultyEntry->FallbackDifficultyID)
            break;

        difficultyEntry = sDifficultyStore.LookupEntry(difficultyEntry->FallbackDifficultyID);
    }

    if (!cinfo)
        cinfo = normalInfo;

    //Save spawn mode
    m_spawnMode = GetMap()->GetSpawnMode();

    SetEntry(Entry);                                        // normal entry always
    m_creatureInfo = cinfo;                                 // map mode related always

    // equal to player Race field, but creature does not have race
    SetByteValue(UNIT_FIELD_SEX, UNIT_BYTES_0_OFFSET_RACE, 0);

    // known valid are: CLASS_WARRIOR, CLASS_PALADIN, CLASS_ROGUE, CLASS_MAGE
    SetByteValue(UNIT_FIELD_SEX, UNIT_BYTES_0_OFFSET_CLASS, uint8(cinfo->unit_class));

    // Cancel load if no model defined
    if (!(cinfo->GetFirstValidModelId()))
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has no model defined in table `creature_template`, can't load. ", Entry);
        return false;
    }

    uint32 displayID = sObjectMgr->ChooseDisplayId(0, GetCreatureTemplate(), data);
    CreatureModelInfo const* minfo = sObjectMgr->GetCreatureModelRandomGender(&displayID);
    if (!minfo)                                             // Cancel load if no model defined
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has no model defined in table `creature_template`, can't load. ", Entry);
        return false;
    }

    float _scale = 0.f;
    InitDireBeastDisplayIdAndScale(displayID, _scale);

    SetDisplayId(displayID);
    SetNativeDisplayId(displayID);
    SetGender(minfo->gender);

    // Load creature equipment
    if (!data || data->equipmentId == 0)                    // use default from the template
        LoadEquipment(m_OriginalEquipmentId ? m_OriginalEquipmentId : -1);  ///< If no original equip, try to find a random one
    else if (data && data->equipmentId != 0)                // override, 0 means no equipment
    {
        m_OriginalEquipmentId = data->equipmentId;
        LoadEquipment(data->equipmentId);
    }

    SetName(normalInfo->Name[0]);                              // at normal entry always

    SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, minfo->bounding_radius);
    SetFloatValue(UNIT_FIELD_COMBAT_REACH, minfo->combat_reach);

    SetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED, 1.0f);
    SetFloatValue(UNIT_FIELD_MOD_SPELL_HASTE, 1.0f);

    SetSpeed(MOVE_WALK,     cinfo->speed_walk);
    SetSpeed(MOVE_RUN,      cinfo->speed_run);
    SetSpeed(MOVE_SWIM,     1.0f); // using 1.0 rate
    SetSpeed(MOVE_FLIGHT,   cinfo->speed_fly);

    SetObjectScale(_scale ? _scale : cinfo->scale);

    SetFloatValue(UNIT_FIELD_HOVER_HEIGHT, cinfo->HoverHeight);

    // checked at loading
    m_defaultMovementType = MovementGeneratorType(cinfo->MovementType);
    if (!m_respawnradius && m_defaultMovementType == RANDOM_MOTION_TYPE)
        m_defaultMovementType = IDLE_MOTION_TYPE;

    for (uint8 i=0; i < MAX_CREATURE_SPELLS; ++i)
        m_spells[i] = GetCreatureTemplate()->spells[i];

    m_VignetteID      = m_creatureInfo->VignetteID;
    m_TrackingQuestID = m_creatureInfo->TrackingQuestID;

    SetNameTag(data ? data->NameTag : "");

    return true;
}

bool Creature::UpdateEntry(uint32 p_Entry, uint32 p_Team, const CreatureData* p_SpawnData)
{
    if (!InitEntry(p_Entry, p_Team, p_SpawnData))
        return false;

    const CreatureTemplate * l_CreatureTemplate = GetCreatureTemplate();

    m_regenHealth = l_CreatureTemplate->RegenHealth;

    /// Creatures always have melee weapon ready if any unless specified otherwise
    if (!GetCreatureAddon())
        SetSheath(SHEATH_STATE_MELEE);

    SelectLevel(GetCreatureTemplate());
    setFaction(m_NativeCreatureInfo->faction);

    uint32 l_NpcFlag1       = 0;
    uint32 l_NpcFlag2       = 0;
    uint32 l_UnitFlags1     = 0;
    uint32 l_UnitFlags2     = 0;
    uint32 l_UnitFlags3     = 0;
    uint32 l_DynamicFlags   = 0;

    ObjectMgr::ChooseCreatureFlags(m_NativeCreatureInfo, l_NpcFlag1, l_NpcFlag2, l_UnitFlags1, l_UnitFlags2, l_UnitFlags3, l_DynamicFlags, p_SpawnData);

    if (GetFlagsExtra() & CREATURE_FLAG_EXTRA_WORLDEVENT)
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS, l_NpcFlag1 | sGameEventMgr->GetNPCFlag(this));
    else
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS, l_NpcFlag1);

    SetFloatValue(UNIT_FIELD_MOD_TIME_RATE, 1.0f);

    SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, l_NpcFlag2);
    SetUInt32Value(UNIT_FIELD_STATE_WORLD_EFFECT_ID, l_CreatureTemplate->WorldEffectID);

    SetAttackTime(WeaponAttackType::BaseAttack,  l_CreatureTemplate->baseattacktime);
    SetAttackTime(WeaponAttackType::OffAttack,   l_CreatureTemplate->baseattacktime);
    SetAttackTime(WeaponAttackType::RangedAttack, l_CreatureTemplate->rangeattacktime);

    SetUInt32Value(UNIT_FIELD_FLAGS, l_UnitFlags1);
    SetUInt32Value(UNIT_FIELD_FLAGS_2, l_UnitFlags2);
    SetUInt32Value(UNIT_FIELD_FLAGS_3, l_UnitFlags3);

    SetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, l_DynamicFlags);

    if (isDead())
    {
        AddUnitState(UNIT_STATE_DIED);
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetReactState(REACT_PASSIVE);
    }

    /// Setup those fields to 0 for mythic mode, for fixed health values
    bool l_IsMythic = GetMap()->IsMythic();

    if (l_CreatureTemplate->ScaleLevelMin)
        SetUInt32Value(UNIT_FIELD_SCALING_LEVEL_MIN, l_IsMythic ? 0 : l_CreatureTemplate->ScaleLevelMin);
    if (l_CreatureTemplate->ScaleLevelMax)
        SetUInt32Value(UNIT_FIELD_SCALING_LEVEL_MAX, l_IsMythic ? 0 : l_CreatureTemplate->ScaleLevelMax);
    if (l_CreatureTemplate->ScaleLevelDelta)
        SetInt32Value(UNIT_FIELD_SCALING_LEVEL_DELTA, l_IsMythic ? 0 : l_CreatureTemplate->ScaleLevelDelta);

    RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);

    SetMeleeDamageSchool(SpellSchools(l_CreatureTemplate->dmgschool));
    CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(getLevel(), l_CreatureTemplate->unit_class);
    float armor = (float)stats->GenerateArmor(l_CreatureTemplate); // TODO: Why is this treated as uint32 when it's a float?
    SetModifierValue(UNIT_MOD_ARMOR,             BASE_VALUE, armor);
    SetModifierValue(UNIT_MOD_RESISTANCE_HOLY,   BASE_VALUE, float(l_CreatureTemplate->resistance[SPELL_SCHOOL_HOLY]));
    SetModifierValue(UNIT_MOD_RESISTANCE_FIRE,   BASE_VALUE, float(l_CreatureTemplate->resistance[SPELL_SCHOOL_FIRE]));
    SetModifierValue(UNIT_MOD_RESISTANCE_NATURE, BASE_VALUE, float(l_CreatureTemplate->resistance[SPELL_SCHOOL_NATURE]));
    SetModifierValue(UNIT_MOD_RESISTANCE_FROST,  BASE_VALUE, float(l_CreatureTemplate->resistance[SPELL_SCHOOL_FROST]));
    SetModifierValue(UNIT_MOD_RESISTANCE_SHADOW, BASE_VALUE, float(l_CreatureTemplate->resistance[SPELL_SCHOOL_SHADOW]));
    SetModifierValue(UNIT_MOD_RESISTANCE_ARCANE, BASE_VALUE, float(l_CreatureTemplate->resistance[SPELL_SCHOOL_ARCANE]));

    SetCanModifyStats(true);
    UpdateAllStats();

    // checked and error show at loading templates
    if (FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(l_CreatureTemplate->faction))
    {
        if (factionTemplate->Flags & FACTION_TEMPLATE_FLAG_PVP)
            SetPvP(true);
        else
            SetPvP(false);
    }

    // trigger creature is always not selectable and can not be attacked
    if (isTrigger())
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    InitializeReactState();

    if (GetFlagsExtra() & CREATURE_FLAG_EXTRA_NO_TAUNT)
    {
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
    }

    if (l_CreatureTemplate->InhabitType & INHABIT_ROOT)
        SetControlled(true, UNIT_STATE_ROOT);

    UpdateMovementFlags();
    LoadCreaturesAddon();

    // TODO: Shouldn't we check whether or not the creature is in water first?
    if (l_CreatureTemplate->InhabitType & INHABIT_WATER && IsInWater())
        AddUnitMovementFlag(MOVEMENTFLAG_SWIMMING);

    return true;
}

void Creature::UpdateMovementFlags()
{
    /// Do not update movement flags if creature is controlled by a player (charm/vehicle)
    if (m_movedPlayer)
        return;

    if (!sWorld->getBoolConfig(CONFIG_ENABLE_CREATURE_UPDATE_MOVEMENT_FLAGS))
        return;

    /// Set the movement flags if the creature is in that mode. (Only fly if actually in air, only swim if in water, etc)
    float l_Ground = GetMap()->GetHeight(GetPhaseMask(), GetPositionX(), GetPositionY(), GetPositionZ());

    bool l_IsInAir = (G3D::fuzzyGt(GetPositionZ(), l_Ground + 0.05f) || G3D::fuzzyLt(GetPositionZ(), l_Ground - 0.05f)); // Can be underground too, prevent the falling

    if (!IsAIEnabled || !AI()->IgnoreFlyingMovementFlagsUpdate())
    {
        if (GetCreatureTemplate()->InhabitType & INHABIT_AIR && l_IsInAir && !IsFalling())
        {
            if (GetCreatureTemplate()->InhabitType & INHABIT_GROUND)
                SetCanFly(true);
            else
                SetDisableGravity(true);
        }
        else
        {
            SetCanFly(false);
            SetDisableGravity(false);
        }
    }

    if (!l_IsInAir)
        SetFall(false);

    SetSwim(GetCreatureTemplate()->InhabitType & INHABIT_WATER && IsInWater());
}

void Creature::Update(uint32 diff)
{
    volatile uint32 _creatureEntry = GetEntry();

    GetMap()->m_LastCreatureUpdatedEntry = _creatureEntry;
    GetMap()->m_LastCreatureUpdatedX = GetPositionX();
    GetMap()->m_LastCreatureUpdatedY = GetPositionY();
    GetMap()->m_LastCreatureUpdatedZ = GetPositionZ();

    if (GetMap()->GetEntry()->IsContinent() && !isInCombat() && !m_Events.HasScheduledEvent() && m_SkipUpdateCounter < sWorld->getIntConfig(CONFIG_CREATURE_SKIP_UPDATE_MAX))
    {
        m_SkipUpdateCounter++;
        m_DelayedUpdateTime += diff;
        return;
    }

    diff += m_DelayedUpdateTime;
    m_DelayedUpdateTime = 0;
    m_SkipUpdateCounter = 0;

    bool l_ShouldLog = GetZoneId(false) == 7637 || sMapMgr->m_LogMapPerformance || GetMap()->IsRaid();
    uint32 l_StartUpdateTime = l_ShouldLog ? getMSTime() : 0;

    auto l_CreatureEntryCount = GetMap()->m_CreatureEntryCount.find(GetEntry());
    if (l_CreatureEntryCount == GetMap()->m_CreatureEntryCount.end())
        GetMap()->m_CreatureEntryCount[GetEntry()] = 1;
    else
        l_CreatureEntryCount->second++;

    if (m_LOSCheckTimer <= diff)
    {
        m_LOSCheck_player = true;
        m_LOSCheck_creature = true;
        m_LOSCheckTimer = DEFAULT_VISIBILITY_NOTIFY_PERIOD*2;
    }
    else
        m_LOSCheckTimer -= diff;

    if (m_DumpGroupTimer > GROUP_DUMP_TIMER)
    {
        DumpGroup();
        m_DumpGroupTimer = 0;
    }
    else
        m_DumpGroupTimer += diff;

    if (m_NeedRespawn)
    {
        m_RespawnFrameDelay -= diff;

        if (m_RespawnFrameDelay <= 0)
        {
            m_NeedRespawn = false;
            DoRespawn();
        }
    }

    if (IsAIEnabled && TriggerJustRespawned)
    {
        TriggerJustRespawned = false;
        AI()->JustRespawned();
        if (m_vehicleKit)
            m_vehicleKit->Reset();
    }

    m_MovingUpdateTimer -= diff;
    m_NotMovingUpdateTimer -= diff;

    bool l_IsSplineMoving = IsMoving() || IsSplineEnabled() || IsFalling();

    if ((l_IsSplineMoving && m_MovingUpdateTimer <= 0) || (!l_IsSplineMoving && m_NotMovingUpdateTimer <= 0))
    {
        UpdateMovementFlags();

        m_MovingUpdateTimer = 500;
        m_NotMovingUpdateTimer = 5000;
    }

    if (GetMap()->GetSpawnMode() != m_spawnMode && m_deathState != ALIVE)
    {
        m_spawnMode = GetMap()->GetSpawnMode();
        Respawn(true);

        AddDelayedEvent([=]()->void { UpdateStat(); }, 2000);
    }

    switch (m_deathState)
    {
        case JUST_RESPAWNED:
            // Must not be called, see Creature::setDeathState JUST_RESPAWNED -> ALIVE promoting.
            sLog->outError(LOG_FILTER_UNITS, "Creature (GUID: %u Entry: %u) in wrong state: JUST_RESPAWNED (4)", GetGUIDLow(), GetEntry());
            break;
        case JUST_DIED:
            // Must not be called, see Creature::setDeathState JUST_DIED -> CORPSE promoting.
            sLog->outError(LOG_FILTER_UNITS, "Creature (GUID: %u Entry: %u) in wrong state: JUST_DEAD (1)", GetGUIDLow(), GetEntry());
            break;
        case DEAD:
        {
            time_t now = time(NULL);
            if (m_respawnTime <= now)
            {
                bool allowed = IsAIEnabled ? AI()->CanRespawn() : true;     // First check if there are any scripts that object to us respawning
                if (!allowed)                                               // Will be rechecked on next Update call
                    break;

                uint64 dbtableHighGuid = MAKE_NEW_GUID(m_DBTableGuid, GetEntry(), HIGHGUID_UNIT);
                time_t linkedRespawntime = GetMap()->GetLinkedRespawnTime(dbtableHighGuid);
                if (!linkedRespawntime)             // Can respawn
                    Respawn();
                else                                // the master is dead
                {
                    uint64 targetGuid = sObjectMgr->GetLinkedRespawnGuid(dbtableHighGuid);
                    if (targetGuid == dbtableHighGuid) // if linking self, never respawn (check delayed to next day)
                        SetRespawnTime(DAY);
                    else
                        m_respawnTime = (now > linkedRespawntime ? now : linkedRespawntime)+urand(5, MINUTE); // else copy time from master and add a little
                    SaveRespawnTime(); // also save to DB immediately
                }
            }
            break;
        }
        case CORPSE:
        {
            Unit::Update(diff);
            // deathstate changed on spells update, prevent problems
            if (m_deathState != CORPSE)
                break;

            if (m_groupLootTimer)
            {
                if (m_groupLootTimer <= diff)
                {
                    for (auto& l_Itr : m_LootContainers)
                    {
                        if (!IS_GROUP(l_Itr.first))
                            continue;

                        GroupPtr group = sGroupMgr->GetGroupByGUID(l_Itr.first);
                        if (group)
                            group->EndRoll(&l_Itr.second);
                    }
                    m_groupLootTimer = 0;
                }
                else
                    m_groupLootTimer -= diff;
            }
            else if (m_corpseRemoveTime <= time(NULL))
            {
                RemoveCorpse(false);
                sLog->outDebug(LOG_FILTER_UNITS, "Removing corpse... %u ", GetUInt32Value(OBJECT_FIELD_ENTRY_ID));
            }
            break;
        }
        case ALIVE:
        {
            Unit::Update(diff);

            // creature can be dead after Unit::Update call
            // CORPSE/DEAD state will processed at next tick (in other case death timer will be updated unexpectedly)
            if (!isAlive())
                break;

            // if creature is charmed, switch to charmed AI
            if (NeedChangeAI)
            {
                UpdateCharmAI();
                NeedChangeAI = false;
                IsAIEnabled = true;
            }

            if (!IsInEvadeMode() && IsAIEnabled)
            {
                // do not allow the AI to be changed during update
                m_AI_locked = true;

                i_AI->UpdateAI(diff);

                m_AI_locked = false;
            }

            // creature can be dead after UpdateAI call
            // CORPSE/DEAD state will processed at next tick (in other case death timer will be updated unexpectedly)
            if (!isAlive())
                break;

            //Check current difficulty map for change stats
            if (GetMap()->GetInstanceId() != 0 && !isPet())
            {
                if (GetMap()->GetSpawnMode() != m_spawnMode)
                    UpdateStat();
                else if (GetMap()->IsNeedRecalc() && GetMap()->GetPlayerCount() > 10 && GetMap()->GetPlayerCount() > GetPlayerCount()) //For dynamic stats
                {
                    m_playerCount = GetMap()->GetPlayerCount();
                    UpdateMaxHealth();
                    UpdateAttackPowerAndDamage();
                }
            }

            /// - Handle regen timers
            {
                if (m_RegenPowerTimer > 0)
                {
                    if (diff >= m_RegenPowerTimer)
                        m_RegenPowerTimer = 0;
                    else
                        m_RegenPowerTimer -= diff;
                }

                if (m_HealthRegenTimer > 0)
                {
                    if (diff >= m_HealthRegenTimer)
                        m_HealthRegenTimer = 0;
                    else
                        m_HealthRegenTimer -= diff;
                }

                if (m_RegenPowerTimer == 0 && HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER))
                {
                    if (getPowerType() == POWER_ENERGY)
                    {
                        if (!IsVehicle() ||
                            (GetVehicleKit()->GetVehicleInfo()->PowerDisplayID[0] != POWER_PYRITE &&
                            GetVehicleKit()->GetVehicleInfo()->PowerDisplayID[0] != POWER_HEAT))
                            Regenerate(POWER_ENERGY);
                    }
                    else
                        RegenerateMana();

                    m_RegenPowerTimer = CREATURE_REGEN_INTERVAL;
                }

                if (m_HealthRegenTimer == 0)
                {
                    bool bInCombat = isInCombat() && (!getVictim() ||                                        // if isInCombat() is true and this has no victim
                        !getVictim()->GetCharmerOrOwnerPlayerOrPlayerItself() ||                             // or the victim/owner/charmer is not a player
                        !getVictim()->GetCharmerOrOwnerPlayerOrPlayerItself()->isGameMaster());              // or the victim/owner/charmer is not a GameMaster

                    if ((!bInCombat || IsPolymorphed() || HasAuraType(SPELL_AURA_MOD_REGEN_DURING_COMBAT)) && !HealthRegenIsDisable()) // regenerate health if not in combat or if polymorphed
                        RegenerateHealth();

                    m_HealthRegenTimer = CREATURE_REGEN_HEALTH_INTERVAL;
                }
            }

            break;
        }
        default:
            break;
    }

    /// Copy is intentional, m_MovementInform can change in CreatureAI::MovementInform
    std::list<std::pair<uint32, uint32>> l_MovementInforms = m_MovementInform;
    for (auto l_MovementInformInfo : l_MovementInforms)
    {
        if (CreatureAI* l_AI = AI())
            l_AI->MovementInform(l_MovementInformInfo.first, l_MovementInformInfo.second);
    }

    m_MovementInform.clear();

    if (!isAlive())
    {
        if (GetMap()->GetInstanceId() != 0 && !isPet())
        {
            if (GetMap()->GetSpawnMode() != m_spawnMode || GetMap()->IsNeedRespawn(m_respawnChallenge))
            {
                m_respawnChallenge = GetMap()->m_respawnChallenge;
                Respawn(true);
            }
        }
    }


    sScriptMgr->OnCreatureUpdate(this, diff);

    if (l_ShouldLog)
    {
        uint32 l_TimeTaken = getMSTime() - l_StartUpdateTime;
        if (l_TimeTaken > 3)
            sLog->outAshran("Creature::Update %u (mapid %u zoneid %u) taked %u ms ", GetEntry(), GetMapId(), GetZoneId(), l_TimeTaken);
    }
}

void Creature::RegenerateMana()
{
    uint32 l_CurValue = GetPower(POWER_MANA);
    uint32 l_MaxValue = GetMaxPower(POWER_MANA);

    if (l_CurValue >= l_MaxValue)
        return;

    float l_Addvalue = 0;

    // Combat and any controlled creature
    if (isInCombat() || GetCharmerOrOwnerGUID())
    {
        /// - Pet have 60 % of owner mana regen
        Unit* l_Owner = GetOwner();
        if (l_Owner && l_Owner->IsPlayer())
            l_Addvalue = 0.6f * (isInCombat() ? l_Owner->GetFloatValue(UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER) : l_Owner->GetFloatValue(UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER)) * 5.0f;
    }
    else
        l_Addvalue = l_MaxValue / 3;

    // Apply modifiers (if any).
    AuraEffectList const& l_ModPowerRegenPCTAuras = GetAuraEffectsByType(SPELL_AURA_MOD_POWER_REGEN_PERCENT);
    for (AuraEffectList::const_iterator l_Iterator = l_ModPowerRegenPCTAuras.begin(); l_Iterator != l_ModPowerRegenPCTAuras.end(); ++l_Iterator)
    {
        if ((*l_Iterator)->GetMiscValue() == POWER_MANA)
            AddPct(l_Addvalue, (*l_Iterator)->GetAmount());
    }

    l_Addvalue += GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_POWER_REGEN, POWER_MANA) * CREATURE_REGEN_INTERVAL / (5 * IN_MILLISECONDS);

    int32 l_IntValue = std::floor(l_Addvalue);

    if (IsAIEnabled)
        AI()->RegeneratePower(Powers::POWER_MANA, l_IntValue);

    ModifyPower(POWER_MANA, l_IntValue);
}

void Creature::RegenerateHealth()
{
    if (!isRegeneratingHealth())
        return;

    uint64 l_CurValue = GetHealth();
    uint64 l_MaxValue = GetMaxHealth();

    if (l_CurValue >= l_MaxValue)
        return;

    uint32 l_AddValue = 0;
    bool   l_Fight    = HasAuraType(SPELL_AURA_MOD_REGEN_DURING_COMBAT);

    // Not only pet, but any controlled creature
    if (GetCharmerOrOwnerGUID() || l_Fight)
    {
        float l_HealthIncreaseRate = sWorld->getRate(RATE_HEALTH);

        l_AddValue = l_HealthIncreaseRate; ///< l_AddValue is never read 01/18/16

        if (getLevel() < 15)
            l_AddValue = 106.0f * l_HealthIncreaseRate;                                             ///< I've try level 1 to 5, it's always 106 at each tick
        else
            l_AddValue = 0.027f * (float)GetMaxHealth() * l_HealthIncreaseRate;                     ///< Sure about this one, hunter lvl 90 without any stuff/buff/spec

        l_AddValue += GetTotalAuraModifier(SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT);
    }
    else
        l_AddValue = l_MaxValue / 3;

    // Apply modifiers (if any).
    AuraEffectList const& l_ModPowerRegenPCTAuras = GetAuraEffectsByType(SPELL_AURA_MOD_HEALTH_REGEN_PERCENT);
    for (AuraEffectList::const_iterator l_Iterator = l_ModPowerRegenPCTAuras.begin(); l_Iterator != l_ModPowerRegenPCTAuras.end(); ++l_Iterator)
        AddPct(l_AddValue, (*l_Iterator)->GetAmount());

    if (l_Fight)
        ApplyPct(l_AddValue, GetTotalAuraModifier(SPELL_AURA_MOD_REGEN_DURING_COMBAT));

    l_AddValue += GetTotalAuraModifier(SPELL_AURA_MOD_REGEN) * CREATURE_REGEN_INTERVAL  / (5 * IN_MILLISECONDS);

    ModifyHealth(l_AddValue);
}

void Creature::DoFleeToGetAssistance()
{
    if (!getVictim())
        return;

    if (HasAuraType(SPELL_AURA_PREVENTS_FLEEING))
        return;

    float radius = sWorld->getFloatConfig(CONFIG_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS);
    if (radius >0)
    {
        Creature* creature = NULL;

        CellCoord p(JadeCore::ComputeCellCoord(GetPositionX(), GetPositionY()));
        Cell cell(p);
        cell.SetNoCreate();
        JadeCore::NearestAssistCreatureInCreatureRangeCheck u_check(this, getVictim(), radius);
        JadeCore::CreatureLastSearcher<JadeCore::NearestAssistCreatureInCreatureRangeCheck> searcher(this, creature, u_check);

        TypeContainerVisitor<JadeCore::CreatureLastSearcher<JadeCore::NearestAssistCreatureInCreatureRangeCheck>, GridTypeMapContainer > grid_creature_searcher(searcher);

        cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);

        SetNoSearchAssistance(true);
        UpdateSpeed(MOVE_RUN, false);

        if (!creature)
            //SetFeared(true, getVictim()->GetGUID(), 0, sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_FLEE_DELAY));
            //TODO: use 31365
            SetControlled(true, UNIT_STATE_FLEEING);
        else
            GetMotionMaster()->MoveSeekAssistance(creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ());
    }
}

bool Creature::AIM_Initialize(CreatureAI* ai)
{
    // make sure nothing can change the AI during AI update
    if (m_AI_locked)
    {
        return false;
    }

    UnitAI* oldAI = i_AI;

    Motion_Initialize();

    i_AI = ai ? ai : FactorySelector::selectAI(this);
    delete oldAI;
    IsAIEnabled = true;
    i_AI->InitializeAI();
    // Initialize vehicle
    if (GetVehicleKit())
        GetVehicleKit()->Reset();
    return true;
}

void Creature::Motion_Initialize()
{
    if (!m_formation)
        i_motionMaster.Initialize();
    else if (m_formation->getLeader() == this)
    {
        m_formation->FormationReset(false);
        i_motionMaster.Initialize();
    }
    else if (m_formation->isFormed())
        i_motionMaster.MoveIdle(); //wait the order of leader
    else
        i_motionMaster.Initialize();
}

bool Creature::Create(uint32 guidlow, Map* map, uint32 phaseMask, uint32 Entry, uint32 vehId, uint32 team, float x, float y, float z, float ang, const CreatureData* data)
{
    if (data)
        sLog->outInfo(LOG_FILTER_POINTERS, "Pointer 0x%" PRIxPTR " -> Creature Entry: %u \t\t LinkedID: %s", std::uintptr_t(this), Entry, data->linkedId.c_str());
    else
        sLog->outInfo(LOG_FILTER_POINTERS, "Pointer 0x%" PRIxPTR " -> Creature Entry: %u", std::uintptr_t(this), Entry);

    ASSERT(map);
    SetMap(map);

    SetPhaseMask(phaseMask, false);

    if (data && data->phaseID)
        SetInPhase(data->phaseID, true, true);

    CreatureTemplate const* cinfo = sObjectMgr->GetCreatureTemplate(Entry);
    if (!cinfo)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature::Create(): creature template (guidlow: %u, entry: %u) does not exist.", guidlow, Entry);
        return false;
    }

    //! Relocate before CreateFromProto, to initialize coords and allow
    //! returning correct zone id for selecting OutdoorPvP/Battlefield script
    Relocate(x, y, z, ang);

    //oX = x;     oY = y;    dX = x;    dY = y;    m_moveTime = 0;    m_startMove = 0;
    if (!CreateFromProto(guidlow, Entry, vehId, team, data))
        return false;

    if (!IsPositionValid())
    {
        sLog->outError(LOG_FILTER_UNITS, "Creature::Create(): given coordinates for creature (guidlow %d, entry %d) are not valid (X: %f, Y: %f, Z: %f, O: %f)", guidlow, Entry, x, y, z, ang);
        return false;
    }

    switch (GetCreatureTemplate()->rank)
    {
        case CREATURE_ELITE_RARE:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_RARE);
            break;
        case CREATURE_ELITE_ELITE:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_ELITE);
            break;
        case CREATURE_ELITE_RAREELITE:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_RAREELITE);
            break;
        case CREATURE_ELITE_WORLDBOSS:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_WORLDBOSS);
            break;
        default:
            m_corpseDelay = sWorld->getIntConfig(CONFIG_CORPSE_DECAY_NORMAL);
            break;
    }

    LoadCreaturesAddon();

    //! Need to be called after LoadCreaturesAddon - MOVEMENTFLAG_HOVER is set there
    if (HasUnitMovementFlag(MOVEMENTFLAG_HOVER))
    {
        z += GetFloatValue(UNIT_FIELD_HOVER_HEIGHT);
        //! Relocate again with updated Z coord
        Relocate(x, y, z, ang);
    }

    uint32 displayID = GetNativeDisplayId();
    CreatureModelInfo const* minfo = sObjectMgr->GetCreatureModelRandomGender(&displayID);
    if (minfo && !isTotem())                               // Cancel load if no model defined or if totem
    {
        SetDisplayId(displayID);
        SetNativeDisplayId(displayID);
        SetGender(minfo->gender);
    }

    LastUsedScriptID = GetScriptId();

    // TODO: Replace with spell, handle from DB
    if (isSpiritHealer() || isSpiritGuide())
    {
        m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
        m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
    }

    if (Entry == VISUAL_WAYPOINT)
        SetVisible(false);

    SetUInt32Value(EUnitFields::UNIT_FIELD_SCALE_DURATION, 500);
    SetUInt32Value(EUnitFields::UNIT_FIELD_LOOK_AT_CONTROLLER_ID, -1);

    m_FlagExtras = GetCreatureTemplate()->flags_extra;

    if (GetFlagsExtra() & CREATURE_FLAG_EXTRA_IGNORE_PATHFINDING)
        AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

    if (GetFlagsExtra() & CREATURE_FLAG_EXTRA_IMMUNITY_KNOCKBACK)
    {
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
    }

    SetFloatValue(UNIT_FIELD_MOD_TIME_RATE, 1.0f);

    sScriptMgr->OnWorldObjectCreate(this);
    return true;
}

bool Creature::isCanTrainingOf(Player* player, bool msg) const
{
    if (!isTrainer())
        return false;

    TrainerSpellData const* trainer_spells = GetTrainerSpells();

    if ((!trainer_spells || trainer_spells->spellList.empty()) && GetCreatureTemplate()->trainer_type != TRAINER_TYPE_PETS)
        return false;

    switch (GetCreatureTemplate()->trainer_type)
    {
        case TRAINER_TYPE_CLASS:
            if (player->getClass() != GetCreatureTemplate()->trainer_class)
            {
                if (msg)
                {
                    player->PlayerTalkClass->ClearMenus();
                    switch (GetCreatureTemplate()->trainer_class)
                    {
                        case CLASS_DRUID:  player->PlayerTalkClass->SendGossipMenu(4913, GetGUID()); break;
                        case CLASS_HUNTER: player->PlayerTalkClass->SendGossipMenu(10090, GetGUID()); break;
                        case CLASS_MAGE:   player->PlayerTalkClass->SendGossipMenu(328, GetGUID()); break;
                        case CLASS_PALADIN:player->PlayerTalkClass->SendGossipMenu(1635, GetGUID()); break;
                        case CLASS_PRIEST: player->PlayerTalkClass->SendGossipMenu(4436, GetGUID()); break;
                        case CLASS_ROGUE:  player->PlayerTalkClass->SendGossipMenu(4797, GetGUID()); break;
                        case CLASS_SHAMAN: player->PlayerTalkClass->SendGossipMenu(5003, GetGUID()); break;
                        case CLASS_WARLOCK:player->PlayerTalkClass->SendGossipMenu(5836, GetGUID()); break;
                        case CLASS_WARRIOR:player->PlayerTalkClass->SendGossipMenu(4985, GetGUID()); break;
                    }
                }
                return false;
            }
            break;
        case TRAINER_TYPE_PETS:
            if (player->getClass() != CLASS_HUNTER)
            {
                player->PlayerTalkClass->ClearMenus();
                player->PlayerTalkClass->SendGossipMenu(3620, GetGUID());
                return false;
            }
            break;
        case TRAINER_TYPE_MOUNTS:
            if (GetCreatureTemplate()->trainer_race && player->getRace() != GetCreatureTemplate()->trainer_race)
            {
                if (msg)
                {
                    player->PlayerTalkClass->ClearMenus();
                    switch (GetCreatureTemplate()->trainer_race)
                    {
                    case RACE_DWARF:        player->PlayerTalkClass->SendGossipMenu(5865, GetGUID()); break;
                    case RACE_GNOME:        player->PlayerTalkClass->SendGossipMenu(4881, GetGUID()); break;
                    case RACE_HUMAN:        player->PlayerTalkClass->SendGossipMenu(5861, GetGUID()); break;
                    case RACE_NIGHTELF:     player->PlayerTalkClass->SendGossipMenu(5862, GetGUID()); break;
                    case RACE_ORC:          player->PlayerTalkClass->SendGossipMenu(5863, GetGUID()); break;
                    case RACE_TAUREN:       player->PlayerTalkClass->SendGossipMenu(5864, GetGUID()); break;
                    case RACE_TROLL:        player->PlayerTalkClass->SendGossipMenu(5816, GetGUID()); break;
                    case RACE_UNDEAD_PLAYER:player->PlayerTalkClass->SendGossipMenu(624, GetGUID()); break;
                    case RACE_BLOODELF:     player->PlayerTalkClass->SendGossipMenu(5862, GetGUID()); break;
                    case RACE_DRAENEI:      player->PlayerTalkClass->SendGossipMenu(5864, GetGUID()); break;
                    //case RACE_PANDAREN_HORDE player->PlayerTalkClass->SendGossipMenu( ???, GetGUID()); break; //TODO Find gossipID
                    //case RACE_PANDAREN_ALLIANCE  player->PlayerTalkClass->SendGossipMenu( ???, GetGUID()); break; //TODO Find gossipID
                    }
                }
                return false;
            }
            break;
        case TRAINER_TYPE_TRADESKILLS:
            if (GetCreatureTemplate()->trainer_spell && !player->HasSpell(GetCreatureTemplate()->trainer_spell))
            {
                if (msg)
                {
                    player->PlayerTalkClass->ClearMenus();
                    player->PlayerTalkClass->SendGossipMenu(11031, GetGUID());
                }
                return false;
            }
            break;
        default:
            return false;                                   // checked and error output at creature_template loading
    }
    return true;
}

bool Creature::isCanInteractWithBattleMaster(Player* player, bool msg) const
{
    if (!isBattleMaster())
        return false;

    BattlegroundTypeId bgTypeId = sBattlegroundMgr->GetBattleMasterBG(GetEntry());
    if (!msg)
        return player->GetBGAccessByLevel(bgTypeId);

    if (!player->GetBGAccessByLevel(bgTypeId))
    {
        player->PlayerTalkClass->ClearMenus();
        switch (bgTypeId)
        {
            case BATTLEGROUND_AV:  player->PlayerTalkClass->SendGossipMenu(7616, GetGUID()); break;
            case BATTLEGROUND_WS:  player->PlayerTalkClass->SendGossipMenu(7599, GetGUID()); break;
            case BATTLEGROUND_AB:  player->PlayerTalkClass->SendGossipMenu(7642, GetGUID()); break;
            case BATTLEGROUND_EY:
            case BATTLEGROUND_NA:
            case BATTLEGROUND_TV:
            case BATTLEGROUND_TTP:
            case BATTLEGROUND_BE:
            case BATTLEGROUND_AA:
            case BATTLEGROUND_RL:
            case BATTLEGROUND_SA:
            case BATTLEGROUND_DS:
            case BATTLEGROUND_RV: player->PlayerTalkClass->SendGossipMenu(10024, GetGUID()); break;
            default: break;
        }
        return false;
    }
    return true;
}

bool Creature::isCanTrainingAndResetTalentsOf(Player* player) const
{
    return player->getLevel() >= 10
        && GetCreatureTemplate()->trainer_type == TRAINER_TYPE_CLASS
        && player->getClass() == GetCreatureTemplate()->trainer_class;
}

void Creature::AI_SendMoveToPacket(float x, float y, float z, uint32 time, uint32 /*MovementFlags*/, uint8 /*type*/)
{
    /*    uint32 timeElap = getMSTime();
        if ((timeElap - m_startMove) < m_moveTime)
        {
            oX = (dX - oX) * ((timeElap - m_startMove) / m_moveTime);
            oY = (dY - oY) * ((timeElap - m_startMove) / m_moveTime);
        }
        else
        {
            oX = dX;
            oY = dY;
        }

        dX = x;
        dY = y;
        m_orientation = std::atan2((oY - dY), (oX - dX));

        m_startMove = getMSTime();
        m_moveTime = time;*/
    float speed = GetDistance(x, y, z) / ((float)time * 0.001f);
    MonsterMoveWithSpeed(x, y, z, speed);
}

GarrisonNPCAI* Creature::ToGarrisonNPCAI() const
{
    return reinterpret_cast<GarrisonNPCAI*>(i_AI);
}

bool Creature::isTappedBy(Player const* player) const
{
    if (m_LootContainers.find(player->GetGUID()) != m_LootContainers.end())
        return true;

    if (GroupPtr l_Group = const_cast<Player*>(player)->GetGroup())
    {
        if (m_LootContainers.find(l_Group->GetGUID()) != m_LootContainers.end())
            return true;
    }

    return false;
}

void Creature::SaveToDB()
{
    // this should only be used when the creature has already been loaded
    // preferably after adding to map, because mapid may not be valid otherwise
    CreatureData const* data = sObjectMgr->GetCreatureData(m_DBTableGuid);
    if (!data)
    {
        sLog->outError(LOG_FILTER_UNITS, "Creature::SaveToDB failed, cannot get creature data!");
        return;
    }

    uint32 mapId = GetTransport() ? GetTransport()->GetGOInfo()->moTransport.SpawnMap : GetMapId();
    SaveToDB(mapId, data->spawnMask, GetPhaseMask());
}

void Creature::SaveToDB(uint32 mapid, uint32 spawnMask, uint32 phaseMask)
{
    /// Update in loaded data
    if (!m_DBTableGuid)
        m_DBTableGuid = GetGUIDLow();

    CreatureData & l_SpawnData = sObjectMgr->NewOrExistCreatureData(m_DBTableGuid);

    uint32 l_DisplayID      = GetNativeDisplayId();
    uint32 l_NpcFlags1      = GetUInt32Value(UNIT_FIELD_NPC_FLAGS);
    uint32 l_NpcFlags2      = GetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1);
    uint32 l_UnitFlags1     = GetUInt32Value(UNIT_FIELD_FLAGS);
    uint32 l_UnitFlags2     = GetUInt32Value(UNIT_FIELD_FLAGS_2);
    uint32 l_UnitFlags3     = GetUInt32Value(UNIT_FIELD_FLAGS_3);
    uint32 l_Dynamicflags   = GetUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS);
    uint32 l_WorldEffectID  = GetUInt32Value(UNIT_FIELD_STATE_WORLD_EFFECT_ID);

    /// Check if it's a custom model and if not, use 0 for displayId
    const CreatureTemplate * l_CreatureTemplate = GetCreatureTemplate();
    if (l_CreatureTemplate)
    {
        if (l_DisplayID == l_CreatureTemplate->Modelid[0] || l_DisplayID == l_CreatureTemplate->Modelid[1] ||
            l_DisplayID == l_CreatureTemplate->Modelid[2] || l_DisplayID == l_CreatureTemplate->Modelid[3])
            l_DisplayID = 0;

        if (l_NpcFlags1 == l_CreatureTemplate->NpcFlags1)
            l_NpcFlags1 = 0;

        if (l_NpcFlags2 == l_CreatureTemplate->NpcFlags2)
            l_NpcFlags2 = 0;

        if (l_UnitFlags1 == l_CreatureTemplate->UnitFlags1)
            l_UnitFlags1 = 0;

        if (l_UnitFlags2 == l_CreatureTemplate->UnitFlags2)
            l_UnitFlags2 = 0;

        if (l_UnitFlags3 == l_CreatureTemplate->UnitFlags3)
            l_UnitFlags3 = 0;

        if (l_Dynamicflags == l_CreatureTemplate->dynamicflags)
            l_Dynamicflags = 0;

        if (l_WorldEffectID == l_CreatureTemplate->WorldEffectID)
            l_WorldEffectID = 0;
    }

    uint32 zoneId = 0;
    uint32 areaId = 0;
    sMapMgr->GetZoneAndAreaId(zoneId, areaId, mapid, GetPositionX(), GetPositionY(), GetPositionZ());

    // data->guid = guid must not be updated at save
    l_SpawnData.id = GetEntry();
    l_SpawnData.mapid = mapid;
    l_SpawnData.zoneId = zoneId;
    l_SpawnData.areaId = areaId;
    l_SpawnData.phaseMask = phaseMask;
    l_SpawnData.displayid = l_DisplayID;
    l_SpawnData.equipmentId = GetCurrentEquipmentId();

    if (!GetTransport())
    {
        l_SpawnData.posX = GetPositionX();
        l_SpawnData.posY = GetPositionY();
        l_SpawnData.posZ = GetPositionZH();
        l_SpawnData.orientation = GetOrientation();
    }
    else
    {
        l_SpawnData.posX = GetTransOffsetX();
        l_SpawnData.posY = GetTransOffsetY();
        l_SpawnData.posZ = GetTransOffsetZ();
        l_SpawnData.orientation = GetTransOffsetO();
    }

    l_SpawnData.spawntimesecs = m_respawnDelay;
    // prevent add data integrity problems
    l_SpawnData.spawndist = GetDefaultMovementType() == IDLE_MOTION_TYPE ? 0.0f : m_respawnradius;
    l_SpawnData.currentwaypoint = 0;
    l_SpawnData.curhealth = GetHealth();
    l_SpawnData.curmana = GetPower(POWER_MANA);
    // prevent add data integrity problems
    l_SpawnData.movementType = !m_respawnradius && GetDefaultMovementType() == RANDOM_MOTION_TYPE
        ? IDLE_MOTION_TYPE : GetDefaultMovementType();
    l_SpawnData.spawnMask = spawnMask;
    l_SpawnData.NpcFlags1 = l_NpcFlags1;
    l_SpawnData.NpcFlags2 = l_NpcFlags2;
    l_SpawnData.UnitFlags1 = l_UnitFlags1;
    l_SpawnData.UnitFlags2 = l_UnitFlags2;
    l_SpawnData.UnitFlags3 = l_UnitFlags3;
    l_SpawnData.dynamicflags = l_Dynamicflags;
    l_SpawnData.WorldEffectID = l_WorldEffectID;
    l_SpawnData.isActive = isActiveObject();
    l_SpawnData.linkedId = ComputeLinkedId();

    // update in DB
    SQLTransaction trans = WorldDatabase.BeginTransaction();

    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_CREATURE);
    stmt->setUInt32(0, m_DBTableGuid);
    trans->Append(stmt);

    uint8 index = 0;

    stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_CREATURE);
    stmt->setUInt32(index++, m_DBTableGuid);
    stmt->setUInt32(index++, GetEntry());
    stmt->setUInt16(index++, uint16(mapid));
    stmt->setUInt32(index++, zoneId);
    stmt->setUInt32(index++, areaId);
    stmt->setUInt8(index++,  spawnMask);
    stmt->setUInt32(index++, uint32(GetPhaseMask()));
    stmt->setUInt32(index++, l_DisplayID);
    stmt->setInt32(index++,  int32(GetCurrentEquipmentId()));
    stmt->setFloat(index++,  GetPositionX());
    stmt->setFloat(index++,  GetPositionY());
    stmt->setFloat(index++,  GetPositionZH());
    stmt->setFloat(index++,  GetOrientation());
    stmt->setUInt32(index++, m_respawnDelay);
    stmt->setFloat(index++,  m_respawnradius);
    stmt->setUInt32(index++, 0);
    stmt->setUInt64(index++, GetHealth());
    stmt->setUInt32(index++, GetPower(POWER_MANA));
    stmt->setUInt8(index++,  uint8(GetDefaultMovementType()));
    stmt->setUInt32(index++, l_NpcFlags1);
    stmt->setUInt32(index++, l_NpcFlags2);
    stmt->setUInt32(index++, l_UnitFlags1);
    stmt->setUInt32(index++, l_UnitFlags2);
    stmt->setUInt32(index++, l_UnitFlags3);
    stmt->setUInt32(index++, l_Dynamicflags);
    stmt->setUInt32(index++, l_WorldEffectID);
    stmt->setUInt32(index++, uint8(isActiveObject()));
    trans->Append(stmt);

    WorldDatabase.CommitTransaction(trans);
}

void Creature::SelectLevel(const CreatureTemplate* cinfo)
{
    uint32 rank = isPet()? 0 : cinfo->rank;

    uint32 bg_minlevel = 0;
    uint32 bg_maxlevel = 0;
    uint32 bg_level = 0;

    if (GetMap()->IsBattleground())
    {
        Battleground* bg = NULL;
        bg = reinterpret_cast<BattlegroundMap*>(GetMap())->GetBG();
        if (bg)
        {
            bg_minlevel = bg->GetMinLevel();
            bg_maxlevel = bg->GetMaxLevel();
            if (bg_minlevel > 0 && bg_maxlevel > 2)
            {
                switch (rank)
                {
                    case CREATURE_ELITE_NORMAL:
                    {
                        if (bg_minlevel < 110)
                        {
                            if (IsVehicle())
                                bg_level = bg_maxlevel;
                            else
                                bg_level = urand(bg_minlevel, bg_minlevel + 2);
                        }
                        else
                            bg_level = 110;
                        break;
                    }
                    case CREATURE_ELITE_ELITE:
                    case CREATURE_ELITE_RAREELITE:
                    {
                        if (bg_minlevel < 110)
                            bg_level = urand(bg_maxlevel - 2, bg_maxlevel);
                        else
                            bg_level = 112;
                        break;
                    }
                    case CREATURE_ELITE_WORLDBOSS:
                    {
                        bg_level = bg_maxlevel + 3;
                        break;
                    }
                }
            }
        }
    }

    // level
    uint8 minlevel = std::min(cinfo->maxlevel, cinfo->minlevel);
    uint8 maxlevel = std::max(cinfo->maxlevel, cinfo->minlevel);

    uint8 level = minlevel == maxlevel ? minlevel : urand(minlevel, maxlevel);

    if (cinfo->ScaleLevelMax > level)
        level = cinfo->ScaleLevelMax;

    if (bg_level)
        level = bg_level;

    SetLevel(level);
    UpdateStatsForLevel();
}

void Creature::UpdateStatsForLevel()
{
    CreatureTemplate  const*     l_CreatureTemplate = GetCreatureTemplate();
    CreatureBaseStats const*     l_Stats            = sObjectMgr->GetCreatureBaseStats(GetUInt32Value(UNIT_FIELD_LEVEL), l_CreatureTemplate->unit_class);
    CreatureGroupSizeStat const* l_GroupSizeStat    = sObjectMgr->GetCreatureGroupSizeStat(GetEntry(), GetMap()->GetDifficultyID());
    float const*                 l_ScalingPct       = nullptr;

    uint32 l_Rank = isPet() ? 0 : l_CreatureTemplate->rank;

    /// Health
    float l_HeathMod = _GetHealthMod(l_Rank) * (m_HasDifficultyEntry ? 1.0f : _GetHealthModForDiff());

    uint64 l_BaseHP = uint64(l_Stats->GenerateHealth(l_CreatureTemplate));
    uint64 l_Health = l_BaseHP * l_HeathMod;

    if (uint64 const* l_DiffHealth = sObjectMgr->GetDiffHealthValue(GetEntry(), GetMap()->GetDifficultyID()))
        l_Health = *l_DiffHealth;

    /// Used for WoD Raids
    if (l_GroupSizeStat != nullptr)
    {
        uint32 l_GroupSizeHealth = l_GroupSizeStat->GetHealthFor(GetMap()->GetPlayersCountExceptGMs());
        if (l_GroupSizeHealth != 0)
            l_Health = l_GroupSizeHealth * l_HeathMod;
    }
    /// Legion Raids (except LFR, fixed health)
    else if (GetMap()->IsRaid() && GetMap()->Expansion() == Expansion::EXPANSION_LEGION && !GetMap()->IsLFR() && !GetMap()->IsMythic() &&
        (!IsAIEnabled || (IsAIEnabled && AI()->CanScale())))
    {
        uint32 l_Size = GetMap()->GetPlayersCountExceptGMs();
        if (l_Size > 10)
        {
            for (uint32 l_I = 11; l_I <= l_Size; ++l_I)
            {
                if ((l_ScalingPct = sObjectMgr->GetCreatureScalingPct(l_I)))
                    AddPct(l_Health, *l_ScalingPct);
            }
        }
    }

    SetCreateHealth(l_Health);
    SetMaxHealth(l_Health);
    SetHealth(l_Health);

    /// Mana
    uint32 l_Mana = l_Stats->GenerateMana(l_CreatureTemplate);
    SetCreateMana(l_Mana);

    switch (getClass())
    {
        case CLASS_WARRIOR:
            setPowerType(POWER_RAGE);
            SetMaxPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
            SetPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
            SetModifierValue(UNIT_MOD_RAGE, BASE_VALUE, (float)GetCreatePowers(POWER_RAGE));
            break;
        case CLASS_ROGUE:
            setPowerType(POWER_ENERGY);
            SetMaxPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
            SetPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
            SetModifierValue(UNIT_MOD_ENERGY, BASE_VALUE, (float)GetCreatePowers(POWER_ENERGY));
            break;
        default:
            setPowerType(POWER_MANA);
            SetMaxPower(POWER_MANA, l_Mana);
            SetPower(POWER_MANA, l_Mana);
            break;
    }

    SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)l_Health);
    SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, (float)l_Mana);

    /// Damage
    float l_BaseDamage          = l_Stats->GenerateBaseDamage(l_CreatureTemplate) * (ToTempSummon() != nullptr ? _GetDamageModForDiff() / 2.0f : _GetDamageModForDiff());
    float l_WeaponBaseMinDamage = l_BaseDamage;
    float l_WeaponBaseMaxDamage = l_BaseDamage * 1.5f;

    SetBaseWeaponDamage(WeaponAttackType::BaseAttack, MINDAMAGE, l_WeaponBaseMinDamage);
    SetBaseWeaponDamage(WeaponAttackType::BaseAttack, MAXDAMAGE, l_WeaponBaseMaxDamage);

    SetBaseWeaponDamage(WeaponAttackType::OffAttack, MINDAMAGE, l_WeaponBaseMinDamage);
    SetBaseWeaponDamage(WeaponAttackType::OffAttack, MAXDAMAGE, l_WeaponBaseMaxDamage);

    SetBaseWeaponDamage(WeaponAttackType::RangedAttack, MINDAMAGE, l_WeaponBaseMinDamage);
    SetBaseWeaponDamage(WeaponAttackType::RangedAttack, MAXDAMAGE, l_WeaponBaseMaxDamage);

    SetModifierValue(UNIT_MOD_ATTACK_POWER,        BASE_VALUE, l_Stats->AttackPower);
    SetModifierValue(UNIT_MOD_ATTACK_POWER_RANGED, BASE_VALUE, l_Stats->RangedAttackPower);

    if (!l_CreatureTemplate->ScaleLevelMin || !l_CreatureTemplate->ScaleLevelMax)
    {
        CreatureLevelStat& l_LevelStat = m_levelStat[GetUInt32Value(UNIT_FIELD_LEVEL)];
        l_LevelStat.baseHP            = l_BaseHP;
        l_LevelStat.baseMP            = l_Mana;
        l_LevelStat.healthMax         = l_Health;
        l_LevelStat.baseMinDamage     = l_WeaponBaseMinDamage;
        l_LevelStat.baseMaxDamage     = l_WeaponBaseMaxDamage;
        l_LevelStat.AttackPower       = l_Stats->AttackPower;
        l_LevelStat.RangedAttackPower = l_Stats->RangedAttackPower;
        l_LevelStat.BaseArmor         = l_Stats->GenerateArmor(l_CreatureTemplate);
    }
    else
        GenerateScaleLevelStat(l_CreatureTemplate);
}

void Creature::GenerateScaleLevelStat(const CreatureTemplate* p_Template)
{
    uint32 l_Rank = isPet() ? 0 : p_Template->rank;
    float l_Healthmod = _GetHealthMod(l_Rank);

    for (uint8 l_Level = p_Template->ScaleLevelMin; l_Level <= p_Template->ScaleLevelMax; ++l_Level)
    {
        CreatureBaseStats const* l_Stats = sObjectMgr->GetCreatureBaseStats(l_Level, p_Template->unit_class);

        /// Health
        uint32 l_BaseHP = l_Stats->GenerateHealth(p_Template);
        uint32 l_Health = uint32(l_BaseHP * l_Healthmod);

        /// Shouldn't be more. Check stats.
        if (l_BaseHP > 0x7FFFFFFF)
            l_Health = 1;

        /// Mana
        uint32 l_Mana = l_Stats->GenerateMana(p_Template);

        /// Damage
        float l_BaseDamage = l_Stats->GenerateBaseDamage(p_Template);

        /// Armor
        uint32 l_Armor = l_Stats->GenerateArmor(p_Template);

        float l_DiffMultiplier = (ToTempSummon() != nullptr ? _GetDamageModForDiff() / 2.0f : _GetDamageModForDiff());

        CreatureLevelStat& l_LevelStat = m_levelStat[l_Level];
        l_LevelStat.baseHP              = l_BaseHP;
        l_LevelStat.baseMP              = l_Mana;
        l_LevelStat.healthMax           = l_Health;
        l_LevelStat.baseMinDamage       = l_BaseDamage * l_DiffMultiplier;
        l_LevelStat.baseMaxDamage       = l_BaseDamage * 1.5f * l_DiffMultiplier;
        l_LevelStat.AttackPower         = l_Stats->AttackPower;
        l_LevelStat.RangedAttackPower   = l_Stats->RangedAttackPower;
        l_LevelStat.BaseArmor           = l_Armor;
    }
}

void Creature::UpdateStat()
{
    m_spawnMode = GetMap()->GetSpawnMode();

    CreatureTemplate const* cInfo = GetCreatureTemplate();

    // level
    uint8 level =  cInfo->maxlevel;

    SetLevel(level);

    CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(level, cInfo->unit_class);

    // health
    float healthmod = _GetHealthMod(cInfo->rank) * (m_HasDifficultyEntry ? 1.0f : _GetHealthModForDiff());

    uint64 basehp = stats->GenerateHealth(cInfo);
    uint64 health = uint64(basehp * healthmod);

    SetCreateHealth(health);
    SetMaxHealth(health);
    SetHealth(health);
    SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)health);

    UpdateMaxHealth();
    UpdateAttackPowerAndDamage();

    if (m_zoneScript)
        m_zoneScript->OnCreatureUpdateDifficulty(this);
}

CreatureLevelStat const* Creature::GetScaleLevelStat(uint8 p_Level)
{
    CreatureLevelStatContainer::const_iterator l_It = m_levelStat.find(p_Level);
    if (l_It != m_levelStat.end())
        return &(l_It->second);

    return nullptr;
}

void Creature::UpdateGroupSizeStats()
{
    CreatureTemplate      const* l_CreatureTemplate = GetCreatureTemplate();
    CreatureGroupSizeStat const* l_GroupSizeStat    = sObjectMgr->GetCreatureGroupSizeStat(GetEntry(), GetMap()->GetDifficultyID());
    CreatureBaseStats     const* l_Stats            = sObjectMgr->GetCreatureBaseStats(GetUInt32Value(UNIT_FIELD_LEVEL), l_CreatureTemplate->unit_class);
    float                 const* l_ScalingPct       = nullptr;

    /// - Update health
    uint32 l_Rank            = isPet() ? 0 : l_CreatureTemplate->rank;
    float  l_HealthMod       = _GetHealthMod(l_Rank) * (m_HasDifficultyEntry ? 1.0f : _GetHealthModForDiff());
    float  l_ActualHealthPct = GetHealthPct();
    uint64 l_Health          = uint64(l_Stats->GenerateHealth(l_CreatureTemplate) * l_HealthMod);

    if (uint64 const* l_DiffHealth = sObjectMgr->GetDiffHealthValue(GetEntry(), GetMap()->GetDifficultyID()))
        l_Health = *l_DiffHealth;

    /// WoD Raids
    if (l_GroupSizeStat != nullptr)
    {
        uint32 l_GroupSizeHealth = l_GroupSizeStat->GetHealthFor(GetMap()->GetPlayersCountExceptGMs());
        if (l_GroupSizeHealth != 0)
            l_Health = uint64(l_GroupSizeHealth * l_HealthMod);
    }
    /// Legion Raids (except LFR, fixed health)
    else if (GetMap()->IsRaid() && GetMap()->Expansion() == Expansion::EXPANSION_LEGION && !GetMap()->IsLFR() && !GetMap()->IsMythic() &&
        (!IsAIEnabled || (IsAIEnabled && AI()->CanScale())))
    {
        uint32 l_Size = GetMap()->GetPlayersCountExceptGMs();
        if (l_Size > 10)
        {
            for (uint32 l_I = 11; l_I <= l_Size; ++l_I)
            {
                if ((l_ScalingPct = sObjectMgr->GetCreatureScalingPct(l_I)))
                    AddPct(l_Health, *l_ScalingPct);
            }
        }
    }

    if (!l_Health)
        return;

    /// - Set Base health
    SetCreateHealth(l_Health);
    SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)l_Health);

    /// - Calculate max health with aura, bonus ... etc
    UpdateMaxHealth();

    /// - Set to the same health %
    SetHealth(GetMaxHealth() * (l_ActualHealthPct / 100));
}

float Creature::_GetHealthModForDiff()
{
    switch (GetMap()->GetDifficultyID())
    {
        case Difficulty::DifficultyRaidLFR:
        case Difficulty::DifficultyRaidTool:
            return 1.8f;
        case Difficulty::DifficultyHCScenario:
        case Difficulty::DifficultyHeroic:
        {
            /// 7.3 update : dungeon in heroic difficulty now have mythic stats for health/damage
            if (GetMap()->IsNonRaidDungeon() && GetMap()->GetEntry()->ExpansionID == Expansion::EXPANSION_LEGION)
                return 2.11f;

            return 1.30f;
        }
        case Difficulty::DifficultyMythicKeystone:
        case Difficulty::DifficultyMythic:
        {
            float l_Mod = 2.75f;

            if (IsAIEnabled)
                AI()->GetMythicHealthMod(l_Mod);

            return l_Mod;
        }
        case Difficulty::DifficultyRaidHeroic:
            return 1.55f;
        case Difficulty::DifficultyRaidMythic:
        {
            float l_Mod = 4.0f;

            if (IsAIEnabled)
                AI()->GetMythicHealthMod(l_Mod);

            return l_Mod;
        }
        case Difficulty::Difficulty25HC:
            return 5.0f;
        case Difficulty::Difficulty25N:
            return 3.0f;
        case Difficulty::DifficultyNormal:
        {
            /// 7.3 update : dungeon in heroic difficulty now have mythic stats for health/damage
            if (GetMap()->IsNonRaidDungeon() && GetMap()->GetEntry()->ExpansionID == Expansion::EXPANSION_LEGION)
                return 1.625f;

            return 1.0f;
        }
        default:
            return 1.0f;
    }
}

float Creature::_GetDamageModForDiff()
{
    int32 l_Level              = getLevel();
    int32 l_LevelMax           = GetCreatureTemplate()->ScaleLevelMax;
    int32 l_ScaleLevelDelta    = GetCreatureTemplate()->ScaleLevelDelta;

    if (l_Level < (l_LevelMax + l_ScaleLevelDelta))
        l_Level = l_LevelMax + l_ScaleLevelDelta;

    // TODO: check calculations
    // it was 'Level - MAX_LEVEL', but level can be lower than MAX_LEVEL
    uint32 levelDiff = l_Level > MAX_LEVEL ? l_Level - MAX_LEVEL : 1;
    switch (GetMap()->GetDifficultyID())
    {
        case Difficulty::DifficultyRaidLFR:
            return 1.0f + (1.0f + (1.0f * float(GetCreatureTemplate()->rank) * levelDiff));
        case Difficulty::DifficultyHCScenario:
        case Difficulty::DifficultyHeroic:
        {
            /// 7.3 update : dungeon in heroic difficulty now have mythic stats for health/damage
            if (GetMap()->IsNonRaidDungeon() && GetMap()->GetEntry()->ExpansionID == Expansion::EXPANSION_LEGION)
                return 4.0f + (3.0f * levelDiff);

            return 2.0f + (3.0f * levelDiff);
        }
        case Difficulty::DifficultyMythicKeystone:
        case Difficulty::DifficultyMythic:
            return 5.0f + (3.0f * levelDiff);
        case Difficulty::DifficultyRaidNormal:
            return 3.0f + (1.0f * float(GetCreatureTemplate()->rank) * levelDiff);
        case Difficulty::DifficultyRaidHeroic:
            return 4.0f + (1.25f * float(GetCreatureTemplate()->rank) * levelDiff);
        case Difficulty::DifficultyRaidMythic:
            return 7.0f + (2.25f * float(GetCreatureTemplate()->rank) * levelDiff);
        case Difficulty::Difficulty10HC:
        case Difficulty::Difficulty25HC:
            return 2.0f;
        case Difficulty::Difficulty25N:
            return 3.0f;
        case Difficulty::DifficultyRaidTool:
            return 0.6f;
        case Difficulty::DifficultyNormal:
        {
            /// 7.3 update : dungeon in normal difficulty now have heroic stats for health/damage
            if (GetMap()->IsNonRaidDungeon() && GetMap()->GetEntry()->ExpansionID == Expansion::EXPANSION_LEGION)
                return 3.0f + (3.0f * levelDiff);

            return 1.0f;
        }
        default:
            return 1.0f;
    }
}

float Creature::_GetHealthMod(int32 Rank)
{
    switch (Rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld->getRate(RATE_CREATURE_NORMAL_HP);
        case CREATURE_ELITE_ELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_HP);
        case CREATURE_ELITE_RAREELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RAREELITE_HP);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld->getRate(RATE_CREATURE_ELITE_WORLDBOSS_HP);
        case CREATURE_ELITE_RARE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RARE_HP);
        default:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_HP);
    }
}

float Creature::_GetHealthModPersonal(uint32& p_Count)
{
    float l_Modifier = 0.0f;

    if (IsAIEnabled && AI()->GetHealthModPersonal(l_Modifier, p_Count))
        return l_Modifier;

    if (CreatureTemplate const* l_Template = GetCreatureTemplate())
    {
        switch (l_Template->rank)
        {
            case CREATURE_ELITE_NORMAL:
            case CREATURE_ELITE_ELITE:
            case CREATURE_ELITE_RAREELITE:
            case CREATURE_ELITE_RARE:
            {
                p_Count -= 1; ///< first player
                l_Modifier = 0.7f;  ///< From WoD hp increment 70% by player
                break;
            }
            case CREATURE_ELITE_WORLDBOSS:
            {
                if (l_Template->expansion < EXPANSION_WARLORDS_OF_DRAENOR)
                    break;

                if (GetMap()->IsNeedRecalc() && p_Count > 10) ///< Base hp for min player if > 10
                {
                    p_Count -= 10;
                    l_Modifier = 0.05f;
                    break;
                }
            }
            default:
                break;
        }
    }

    return l_Modifier;
}

float Creature::_GetDamageMod(int32 Rank)
{
    switch (Rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld->getRate(RATE_CREATURE_NORMAL_DAMAGE);
        case CREATURE_ELITE_ELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_DAMAGE);
        case CREATURE_ELITE_RAREELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RAREELITE_DAMAGE);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld->getRate(RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE);
        case CREATURE_ELITE_RARE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RARE_DAMAGE);
        default:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_DAMAGE);
    }
}

float Creature::GetSpellDamageMod(int32 Rank) const
{
    switch (Rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld->getRate(RATE_CREATURE_NORMAL_SPELLDAMAGE);
        case CREATURE_ELITE_ELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE);
        case CREATURE_ELITE_RAREELITE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld->getRate(RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE);
        case CREATURE_ELITE_RARE:
            return sWorld->getRate(RATE_CREATURE_ELITE_RARE_SPELLDAMAGE);
        default:
            return sWorld->getRate(RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE);
    }
}

bool Creature::CreateFromProto(uint32 guidlow, uint32 Entry, uint32 vehId, uint32 team, const CreatureData* data)
{
    SetZoneScript();
    if (m_zoneScript && data)
    {
        Entry = m_zoneScript->GetCreatureEntry(guidlow, data);
        if (!Entry)
            return false;
    }

    CreatureTemplate const* cinfo = sObjectMgr->GetCreatureTemplate(Entry);
    if (!cinfo)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature::CreateFromProto(): creature template (guidlow: %u, entry: %u) does not exist.", guidlow, Entry);
        return false;
    }

    SetOriginalEntry(Entry);

    if (!vehId)
        vehId = cinfo->VehicleId;

    if (vehId && !CreateVehicleKit(vehId, Entry))
        vehId = 0;

    Object::_Create(guidlow, Entry, vehId ? HIGHGUID_VEHICLE : HIGHGUID_UNIT);

    if (!UpdateEntry(Entry, team, data))
        return false;

    return true;
}

bool Creature::LoadCreatureFromDB(uint32 guid, Map* map, bool addToMap, bool p_GenerateNewGuid)
{
    CreatureData const* data = sObjectMgr->GetCreatureData(guid);

    if (!data)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (GUID: %u) not found in table `creature`, can't load. ", guid);
        return false;
    }

    m_DBTableGuid = guid;

    if (p_GenerateNewGuid)
        guid = sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT);

    uint16 team = 0;
    if (!Create(guid, map, data->phaseMask, data->id, 0, team, data->posX, data->posY, data->posZ, data->orientation, data))
        return false;

    //We should set first home position, because then AI calls home movement
    SetHomePosition(data->posX, data->posY, data->posZ, data->orientation);

    m_respawnradius = data->spawndist;

    m_respawnDelay = data->spawntimesecs;
    m_corpseDelay = std::min(m_respawnDelay * 9 / 10, m_corpseDelay);
    m_deathState = ALIVE;

    m_respawnTime  = GetMap()->GetCreatureRespawnTime(m_DBTableGuid);
    if (m_respawnTime)                          // respawn on Update
    {
        m_deathState = DEAD;
        if (CanFly())
        {
            float tz = map->GetHeight(GetPhaseMask(), data->posX, data->posY, data->posZ, false);
            if (data->posZ - tz > 0.1f)
                Relocate(data->posX, data->posY, tz);
        }
    }

    uint64 curhealth;

    if (!m_regenHealth)
    {
        curhealth = data->curhealth;
        if (curhealth)
        {
            curhealth = curhealth * _GetHealthMod(GetCreatureTemplate()->rank);
            if (curhealth < 1)
                curhealth = 1;
        }

        switch (getClass())
        {
            case CLASS_WARRIOR:
                setPowerType(POWER_RAGE);
                SetMaxPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
                SetPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
                break;
            case CLASS_ROGUE:
                setPowerType(POWER_ENERGY);
                SetMaxPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
                SetPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
                break;
            default:
                setPowerType(POWER_MANA);
                SetMaxPower(POWER_MANA, GetCreatePowers(POWER_MANA));
                SetPower(POWER_MANA, data->curmana);
                break;
        }
    }
    else
    {
        curhealth = GetMaxHealth();

        switch (getClass())
        {
            case CLASS_WARRIOR:
                setPowerType(POWER_RAGE);
                SetMaxPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
                SetPower(POWER_RAGE, GetCreatePowers(POWER_RAGE));
                break;
            case CLASS_ROGUE:
                setPowerType(POWER_ENERGY);
                SetMaxPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
                SetPower(POWER_ENERGY, GetCreatePowers(POWER_ENERGY));
                break;
            default:
                setPowerType(POWER_MANA);
                SetMaxPower(POWER_MANA, GetCreatePowers(POWER_MANA));
                SetPower(POWER_MANA, data->curmana);
                break;
        }
    }

    SetHealth(m_deathState == ALIVE ? curhealth : 0);

    // checked at creature_template loading
    m_defaultMovementType = MovementGeneratorType(data->movementType);

    m_creatureData = data;

    setActive(data->isActive);

    if (addToMap && !GetMap()->AddToMap(this))
        return false;

    GetMap()->GetWildBattlePetPools()->OnAddToMap(this);

    return true;
}

void Creature::LoadEquipment(int8 p_ID, bool p_Force /*= true*/)
{
    if (p_ID == 0)
    {
        if (p_Force)
        {
            for (uint8 l_Iter = 0; l_Iter < MAX_EQUIPMENT_ITEMS * 2; ++l_Iter)
                SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + l_Iter, 0);

            m_equipmentId = 0;
        }
        return;
    }

    EquipmentInfo const* l_EquipInfos = sObjectMgr->GetEquipmentInfo(GetEntry(), p_ID);
    if (!l_EquipInfos)
        return;

    m_equipmentId = p_ID;

    SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 0, l_EquipInfos->ItemEntry[0]);
    SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 2, l_EquipInfos->ItemEntry[1]);
    SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 4, l_EquipInfos->ItemEntry[2]);

    /// Check if creature has two weapons, and set dual wield
    if (l_EquipInfos->ItemEntry[0] && l_EquipInfos->ItemEntry[1])
        m_canDualWield = true;
}

void Creature::LoadSpecialEquipment(uint32 p_First, uint32 p_Second, uint32 p_Third)
{
    int8 p_ID = -1;

    EquipmentInfo const* l_EquipInfos = sObjectMgr->GetEquipmentInfo(GetEntry(), p_ID);
    if (!l_EquipInfos)
        return;

    m_equipmentId = p_ID;

    SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS, p_First);
    SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 2, p_Second);
    SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 4, p_Third);

    /// Check if creature has two weapons, and set dual wield
    if (p_First && p_Second)
        m_canDualWield = true;
}

bool Creature::hasQuest(uint32 quest_id) const
{
    QuestRelationBounds qr = sObjectMgr->GetCreatureQuestRelationBounds(GetEntry());
    for (QuestRelations::const_iterator itr = qr.first; itr != qr.second; ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}

bool Creature::hasInvolvedQuest(uint32 quest_id) const
{
    QuestRelationBounds qir = sObjectMgr->GetCreatureQuestInvolvedRelationBounds(GetEntry());
    for (QuestRelations::const_iterator itr = qir.first; itr != qir.second; ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}

void Creature::DeleteFromDB()
{
    if (!m_DBTableGuid)
    {
        sLog->outError(LOG_FILTER_UNITS, "Trying to delete not saved creature! LowGUID: %u, Entry: %u", GetGUIDLow(), GetEntry());
        return;
    }

    GetMap()->RemoveCreatureRespawnTime(m_DBTableGuid);
    sObjectMgr->DeleteCreatureData(m_DBTableGuid);

    m_creatureData = nullptr;

    SQLTransaction trans = WorldDatabase.BeginTransaction();

    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_CREATURE);
    stmt->setUInt32(0, m_DBTableGuid);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_CREATURE_ADDON);
    stmt->setUInt32(0, m_DBTableGuid);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAME_EVENT_CREATURE);
    stmt->setUInt32(0, m_DBTableGuid);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAME_EVENT_MODEL_EQUIP);
    stmt->setUInt32(0, m_DBTableGuid);
    trans->Append(stmt);

    WorldDatabase.CommitTransaction(trans);
}

bool Creature::IsInvisibleDueToDespawn() const
{
    if (Unit::IsInvisibleDueToDespawn())
        return true;

    if (isAlive() || m_corpseRemoveTime > time(NULL))
        return false;

    return true;
}

bool Creature::CanAlwaysSee(WorldObject const* obj) const
{
    if (IsAIEnabled && AI()->CanSeeAlways(obj))
        return true;

    return false;
}

bool Creature::canStartAttack(Unit const* p_Who, bool p_Forced) const
{
    if (isCivilian())
        return false;

    // This set of checks is should be done only for creatures
    if ((HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC) && p_Who->GetTypeId() != TYPEID_PLAYER)                                   // flag is valid only for non player characters
        || (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC) && p_Who->IsPlayer())                                 // immune to PC and target is a player, return false
        || (p_Who->GetOwner() && p_Who->GetOwner()->IsPlayer() && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))) // player pets are immune to pc as well
        return false;

    // Do not attack non-combat pets
    if (p_Who->GetTypeId() == TYPEID_UNIT && p_Who->GetCreatureType() == CREATURE_TYPE_NON_COMBAT_PET)
        return false;

    if (!CanFly() && (GetDistanceZ(p_Who) > CREATURE_Z_ATTACK_RANGE + m_CombatDistance))
        //|| who->IsControlledByPlayer() && who->IsFlying()))
        // we cannot check flying for other creatures, too much map/vmap calculation
        // TODO: should switch to range attack
        return false;

    if (Player const* l_Player = p_Who->ToPlayer())
    {
        /// Prevent creatures from attacking teleported players
        if (l_Player->IsBeingTeleportedNear())
            return false;
    }

    if (!p_Forced)
    {
        if (!_IsTargetAcceptable(p_Who))
            return false;

        if (p_Who->isInCombat() && IsWithinDist(p_Who, ATTACK_DISTANCE))
        {
            if (Unit* l_Victim = p_Who->getAttackerForHelper())
            {
                if (IsWithinDistInMap(l_Victim, sWorld->getFloatConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS)))
                    p_Forced = true;
            }
        }

        if (!p_Forced && (IsNeutralToAll() || !IsWithinDistInMap(p_Who, GetAttackDistance(p_Who) + m_CombatDistance)))
            return false;
    }

    if (!canCreatureAttack(p_Who, p_Forced))
        return false;

    return IsWithinLOSInMap(p_Who);
}

float Creature::GetAttackDistance(Unit const* player) const
{
    float aggroRate = sWorld->getRate(RATE_CREATURE_AGGRO);
    if (aggroRate == 0)
        return 0.0f;

    uint32 playerlevel   = player->getLevelForTarget(this);
    uint32 creaturelevel = getLevelForTarget(player);

    int32 leveldif       = int32(playerlevel) - int32(creaturelevel);

    // "The maximum Aggro Radius has a cap of 25 levels under. Example: A level 30 char has the same Aggro Radius of a level 5 char on a level 60 mob."
    if (leveldif < - 25)
        leveldif = -25;

    // "The aggro radius of a mob having the same level as the player is roughly 20 yards"
    float RetDistance = 10;

    // "Aggro Radius varies with level difference at a rate of roughly 1 yard/level"
    // radius grow if playlevel < creaturelevel
    RetDistance -= (float)leveldif;

    if (creaturelevel+5 <= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
    {
        // detect range auras
        RetDistance += GetTotalAuraModifier(SPELL_AURA_MOD_DETECT_RANGE);

        // detected range auras
        RetDistance += player->GetTotalAuraModifier(SPELL_AURA_MOD_DETECTED_RANGE);
    }

    // "Minimum Aggro Radius for a mob seems to be combat range (5 yards)"
    if (RetDistance < 5)
        RetDistance = 5;

    if (IsAIEnabled)
        AI()->OnCalculateAttackDistance(RetDistance);

    return (RetDistance*aggroRate);
}

uint32 Creature::CalculateDynamicResapawnTime()
{
    uint32 l_RespawnTime = m_respawnDelay;
    if (!sWorld->getBoolConfig(CONFIG_CREATURE_RESPAWN_DYNAMIC) || GetMap()->Instanceable() || GetCreatureTemplate()->rank == CREATURE_ELITE_WORLDBOSS)
        return l_RespawnTime;

    float l_Radius = sWorld->getIntConfig(CONFIG_CREATURE_RESPAWN_SEARCHER_RANGE);

    std::list<Player*> l_Player;
    JadeCore::AnyUnitInObjectRangeCheck l_Check(this, l_Radius);
    JadeCore::PlayerListSearcher<JadeCore::AnyUnitInObjectRangeCheck> searcher(this, l_Player, l_Check);
    VisitNearbyObject(l_Radius, searcher);

    if (l_Player.empty())
        return l_RespawnTime;

    if (l_Player.size() < sWorld->getIntConfig(CONFIG_CREATURE_RESPAWN_DYNAMIC_MINIUM))
        return l_RespawnTime;

    float l_Coef = sWorld->getFloatConfig(CONFIG_CREATURE_RESPAWN_DYNAMIC_COEF);
    float l_MaxPctDown = sWorld->getFloatConfig(CONFIG_CREATURE_RESPAWN_MAX_PCT_DOWN);

    uint32 l_TimeDown = 0;
    float l_Pct = l_Player.size() * l_Coef;

    l_Pct = std::min(l_Pct, l_MaxPctDown);
    l_Pct = std::min(l_Pct, 100.0f);

    l_TimeDown = l_RespawnTime - CalculatePct(l_RespawnTime, l_Pct);
    return l_TimeDown;
}

void Creature::setDeathState(DeathState s)
{
    Unit::setDeathState(s);

    if (s == JUST_DIED)
    {
        /// Disable hover after die
        if (HasUnitMovementFlag(MOVEMENTFLAG_HOVER))
            SetHover(false);

        uint32 l_RepawnTime = CalculateDynamicResapawnTime();

        m_corpseRemoveTime = time(NULL) + std::min(l_RepawnTime, m_corpseDelay);
        m_respawnTime      = time(NULL) + CalculateDynamicResapawnTime();

        // always save boss respawn time at death to prevent crash cheating
        if (sWorld->getBoolConfig(CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY) || isWorldBoss())
            SaveRespawnTime();

        SetTarget(0);                // remove target selection in any cases (can be set at aura remove in Unit::setDeathState)
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_NONE);

        SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
        SetUInt32Value(UNIT_FIELD_EMOTE_STATE, 0);

        if (!IsCosmetic())
            setActive(false);

        if (!isPet() && GetCreatureTemplate()->SkinLootId)
            if (LootTemplates_Skinning.HaveLootFor(GetCreatureTemplate()->SkinLootId))
                if (!m_LootContainers.empty())
                    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

        if (HasSearchedAssistance())
        {
            SetNoSearchAssistance(false);
            UpdateSpeed(MOVE_RUN, false);
        }

        //Dismiss group if is leader
        if (m_formation && m_formation->getLeader() == this)
            m_formation->FormationReset(true);

        /// Don't fall if the mob is in water
        if ((CanFly() || IsFlying() || (GetByteValue(UNIT_FIELD_ANIM_TIER, 3) & UNIT_BYTE1_FLAG_HOVER)) && !IsInWater())
            i_motionMaster.MoveFall();

        Unit::setDeathState(CORPSE);
    }
    else if (s == JUST_RESPAWNED)
    {
        //if (isPet())
        //    setActive(true);
        SetFullHealth();
        ClearLootContainers();

        UpdateMovementFlags();

        CreatureTemplate const* cinfo = GetCreatureTemplate();

        SetUInt32Value(UNIT_FIELD_NPC_FLAGS, cinfo->NpcFlags1);
        SetUInt32Value(UNIT_FIELD_NPC_FLAGS + 1, cinfo->NpcFlags2);
        ClearUnitState(uint32(UNIT_STATE_ALL_STATE & ~UNIT_STATE_IGNORE_PATHFINDING));
        SetMeleeDamageSchool(SpellSchools(cinfo->dmgschool));

        Motion_Initialize();
        if (GetCreatureData() && GetPhaseMask() != GetCreatureData()->phaseMask)
            SetPhaseMask(GetCreatureData()->phaseMask, false);

        Unit::setDeathState(ALIVE);
        LoadCreaturesAddon(); ///< Must be done after setDeathState(ALIVE)
    }
}

void Creature::Respawn(bool force, bool p_HomePosAsRespawn /*= false*/, uint32 p_RespawnTime /*= 2 * TimeConstants::IN_MILLISECONDS*/)
{
    if (m_NeedRespawn)
        return;

    DestroyForNearbyPlayers();

    if (force)
    {
        if (isAlive())
            setDeathState(JUST_DIED);
        else if (getDeathState() != CORPSE)
            setDeathState(CORPSE);
    }

    RemoveCorpse(false, p_HomePosAsRespawn);

    UpdateObjectVisibility();

    if (!m_NeedRespawn)
    {
        m_NeedRespawn = true;
        m_RespawnFrameDelay = p_RespawnTime;
    }

    m_AllowedTrackingQuestLooters.clear();
}

void Creature::DoRespawn()
{
    if (getDeathState() == DEAD)
    {
        if (m_DBTableGuid)
            GetMap()->RemoveCreatureRespawnTime(m_DBTableGuid);

        sLog->outDebug(LOG_FILTER_UNITS, "Respawning creature %s (GuidLow: %u, Full GUID: " UI64FMTD " Entry: %u)", GetName(), GetGUIDLow(), GetGUID(), GetEntry());
        m_respawnTime = 0;
        lootForPickPocketed = false;
        lootForBody = false;

        if (m_originalEntry != GetEntry())
            UpdateEntry(m_originalEntry);

        CreatureTemplate const* cinfo = GetCreatureTemplate();
        SelectLevel(cinfo);

        setDeathState(JUST_RESPAWNED);

        uint32 displayID = GetNativeDisplayId();
        CreatureModelInfo const* minfo = sObjectMgr->GetCreatureModelRandomGender(&displayID);
        if (minfo)                                             // Cancel load if no model defined
        {
            SetDisplayId(displayID);
            SetNativeDisplayId(displayID);
            SetGender(minfo->gender);
        }

        GetMotionMaster()->InitDefault();

        //Call AI respawn virtual function
        if (IsAIEnabled)
            TriggerJustRespawned = true;//delay event to next tick so all creatures are created on the map before processing

        uint32 poolid = GetDBTableGUIDLow() ? sPoolMgr->IsPartOfAPool<Creature>(GetDBTableGUIDLow()) : 0;
        if (poolid)
            sPoolMgr->UpdatePool<Creature>(poolid, GetDBTableGUIDLow());

        //Re-initialize reactstate that could be altered by movementgenerators
        InitializeReactState();
    }

    UpdateObjectVisibility();
}

void Creature::ForcedDespawn(uint32 timeMSToDespawn)
{
    if (timeMSToDespawn)
    {
        ForcedDespawnDelayEvent* pEvent = new ForcedDespawnDelayEvent(*this);

        m_Events.AddEvent(pEvent, m_Events.CalculateTime(timeMSToDespawn));
        return;
    }

    /// Call creature just died function
    if (IsAIEnabled)
        AI()->JustDespawned();

    if (isAlive())
        setDeathState(JUST_DIED);

    RemoveCorpse(false);
}

void Creature::SendAddFollowerQuery(Player* p_Player, uint32 p_Sender, uint32 p_Action, char const* p_FollowerName)
{
    char l_Text[MAX_QUERY_LEN];
    snprintf(l_Text, MAX_QUERY_LEN, "Allow %s to join our cause ?", p_FollowerName);

    p_Player->PlayerTalkClass->GetGossipMenu().AddMenuItem(-1, GOSSIP_ICON_CHAT, "Would you like to join our cause ?", p_Sender, p_Action, l_Text, 0, false);
}

void Creature::DespawnOrUnsummon(uint32 msTimeToDespawn /*= 0*/)
{
    if (TempSummon* summon = this->ToTempSummon())
        summon->UnSummon(msTimeToDespawn);
    else
        ForcedDespawn(msTimeToDespawn);
}

void Creature::DespawnCreaturesInArea(uint32 p_Entry, float p_Range)
{
    std::vector<uint32> l_Entries;
    l_Entries.push_back(p_Entry);

    DespawnCreaturesInArea(l_Entries, p_Range);
}

void Creature::DespawnAreaTriggersInArea(uint32 p_SpellID, float p_Range)
{
    std::vector<uint32> l_SpellIDs;
    l_SpellIDs.push_back(p_SpellID);

    DespawnAreaTriggersInArea(l_SpellIDs, p_Range);
}

void Creature::DespawnCreaturesInArea(std::vector<uint32> p_Entry, float p_Range)
{
    std::list<Creature*> l_CreatureList;

    for (std::vector<uint32>::iterator l_Itr = p_Entry.begin(); l_Itr != p_Entry.end(); ++l_Itr)
        GetCreatureListWithEntryInGrid(l_CreatureList, *l_Itr, p_Range);

    for (std::list<Creature*>::iterator l_Itr = l_CreatureList.begin(); l_Itr != l_CreatureList.end(); ++l_Itr)
        (*l_Itr)->DespawnOrUnsummon();
}

void Creature::DespawnAreaTriggersInArea(std::vector<uint32> p_SpellIDs, float p_Range /*= 100.0f*/, bool p_ClearOthers /*= true*/)
{
    std::list<AreaTrigger*> l_AreaTriggerList;

    for (uint32 l_SpellID : p_SpellIDs)
        GetAreaTriggerListWithSpellIDInRange(l_AreaTriggerList, l_SpellID, p_Range, p_ClearOthers);

    for (AreaTrigger* l_AT : l_AreaTriggerList)
        l_AT->SetDuration(0);
}

bool Creature::IsImmunedToSpell(SpellInfo const* spellInfo)
{
    if (!spellInfo)
        return false;

    if (spellInfo->HasAttribute(SPELL_ATTR5_CANT_IMMUNITY_SPELL))
        return false;

    // Creature is immune to main mechanic of the spell
    if (ToPet() == nullptr && GetCreatureTemplate()->MechanicImmuneMask & (1LL << (spellInfo->Mechanic - 1)))
        return true;

    // This check must be done instead of 'if (GetCreatureTemplate()->MechanicImmuneMask & (1LL << (spellInfo->Mechanic - 1)))' for not break
    // the check of mechanic immunity on DB (tested) because GetCreatureTemplate()->MechanicImmuneMask and m_spellImmune[IMMUNITY_MECHANIC] don't have same data.
    bool immunedToAllEffects = true;
    for (uint8 i = 0; i < spellInfo->EffectCount; ++i)
    {
        if (!spellInfo->Effects[i].IsEffect())
            continue;

        if (!IsImmunedToSpellEffect(spellInfo, i))
        {
            immunedToAllEffects = false;
            break;
        }
    }
    if (immunedToAllEffects)
        return true;

    return Unit::IsImmunedToSpell(spellInfo);
}

bool Creature::IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const
{
    /// Sindragosa's Fury shouldn't slow immuned creatures/bosses
    if (spellInfo->Id != 190780 && spellInfo->HasAttribute(SPELL_ATTR5_CANT_IMMUNITY_SPELL))
        return false;

    if (GetCreatureTemplate()->MechanicImmuneMask & (1LL << (spellInfo->Effects[index].Mechanic - 1)))
        return true;

    return Unit::IsImmunedToSpellEffect(spellInfo, index);
}

SpellInfo const* Creature::reachWithSpellAttack(Unit* victim)
{
    if (!victim)
        return NULL;

    for (uint32 i=0; i < MAX_CREATURE_SPELLS; ++i)
    {
        if (!m_spells[i])
            continue;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(m_spells[i]);
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_UNITS, "WORLD: unknown spell id %i", m_spells[i]);
            continue;
        }

        bool bcontinue = true;
        for (uint8 j = 0; j < spellInfo->EffectCount; j++)
        {
            if ((spellInfo->Effects[j].Effect == SPELL_EFFECT_SCHOOL_DAMAGE)       ||
                (spellInfo->Effects[j].Effect == SPELL_EFFECT_INSTAKILL)            ||
                (spellInfo->Effects[j].Effect == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE) ||
                (spellInfo->Effects[j].Effect == SPELL_EFFECT_HEALTH_LEECH)
                )
            {
                bcontinue = false;
                break;
            }
        }

        if (bcontinue)
            continue;

        bool ok = false;
        for (auto itr : spellInfo->SpellPowers)
        {
            if (itr->Cost > uint32(GetPower(Powers(itr->PowerType))))
                continue;

            ok = true;
        }

        if (!ok)
            continue;

        float range = spellInfo->GetMaxRange(false);
        float minrange = spellInfo->GetMinRange(false);
        float dist = GetDistance(victim);
        if (dist > range || dist < minrange)
            continue;

        if (spellInfo->PreventionType & (SpellPreventionMask::Silence) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            continue;

        if (spellInfo->PreventionType & (SpellPreventionMask::Pacify) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
            continue;

        return spellInfo;
    }
    return NULL;
}

SpellInfo const* Creature::reachWithSpellCure(Unit* victim)
{
    if (!victim)
        return NULL;

    for (uint32 i=0; i < MAX_CREATURE_SPELLS; ++i)
    {
        if (!m_spells[i])
            continue;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(m_spells[i]);
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_UNITS, "WORLD: unknown spell id %i", m_spells[i]);
            continue;
        }

        bool bcontinue = true;
        for (uint8 j = 0; j < spellInfo->EffectCount; j++)
        {
            if ((spellInfo->Effects[j].Effect == SPELL_EFFECT_HEAL))
            {
                bcontinue = false;
                break;
            }
        }

        if (bcontinue)
            continue;

        bool ok = false;
        for (auto itr : spellInfo->SpellPowers)
        {
            if (itr->Cost > (uint32)GetPower(Powers(itr->PowerType)))
                continue;

            ok = true;
        }

        if (!ok)
            continue;

        float range = spellInfo->GetMaxRange(true);
        float minrange = spellInfo->GetMinRange(true);
        float dist = GetDistance(victim);
        //if (!isInFront(victim, range) && spellInfo->AttributesEx)
        //    continue;
        if (dist > range || dist < minrange)
            continue;
        if (spellInfo->PreventionType & (SpellPreventionMask::Silence) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
            continue;
        if (spellInfo->PreventionType & (SpellPreventionMask::Pacify) && HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
            continue;
        return spellInfo;
    }
    return NULL;
}

// select nearest hostile unit within the given distance (regardless of threat list).
Unit* Creature::SelectNearestTarget(float dist) const
{
    CellCoord p(JadeCore::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit* target = NULL;

    {
        if (dist == 0.0f)
            dist = MAX_VISIBILITY_DISTANCE;

        JadeCore::NearestHostileUnitCheck u_check(this, dist);
        JadeCore::UnitLastSearcher<JadeCore::NearestHostileUnitCheck> searcher(this, target, u_check);

        TypeContainerVisitor<JadeCore::UnitLastSearcher<JadeCore::NearestHostileUnitCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
        TypeContainerVisitor<JadeCore::UnitLastSearcher<JadeCore::NearestHostileUnitCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

        cell.Visit(p, world_unit_searcher, *GetMap(), *this, dist);
        cell.Visit(p, grid_unit_searcher, *GetMap(), *this, dist);
    }

    return target;
}

// select nearest hostile unit within the given distance and without cc on it (regardless of threat list).
Unit* Creature::SelectNearestTargetNoCC(float dist) const
{
    CellCoord p(JadeCore::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit *target = NULL;

    {
        if (dist == 0.0f)
            dist = MAX_VISIBILITY_DISTANCE;

        JadeCore::NearestHostileNoCCUnitCheck u_check(this, dist);
        JadeCore::UnitLastSearcher<JadeCore::NearestHostileNoCCUnitCheck> searcher(this, target, u_check);

        TypeContainerVisitor<JadeCore::UnitLastSearcher<JadeCore::NearestHostileNoCCUnitCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
        TypeContainerVisitor<JadeCore::UnitLastSearcher<JadeCore::NearestHostileNoCCUnitCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

        cell.Visit(p, world_unit_searcher, *GetMap(), *this, dist);
        cell.Visit(p, grid_unit_searcher, *GetMap(), *this, dist);
    }

    return target;
}

// select nearest hostile unit within the given attack distance (i.e. distance is ignored if > than ATTACK_DISTANCE), regardless of threat list.
Unit* Creature::SelectNearestTargetInAttackDistance(float dist) const
{
    CellCoord p(JadeCore::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit* target = NULL;

    if (dist > MAX_VISIBILITY_DISTANCE)
    {
        sLog->outError(LOG_FILTER_UNITS, "Creature (GUID: %u Entry: %u) SelectNearestTargetInAttackDistance called with dist > MAX_VISIBILITY_DISTANCE. Distance set to ATTACK_DISTANCE.", GetGUIDLow(), GetEntry());
        dist = ATTACK_DISTANCE;
    }

    {
        JadeCore::NearestHostileUnitInAttackDistanceCheck u_check(this, dist);
        JadeCore::UnitLastSearcher<JadeCore::NearestHostileUnitInAttackDistanceCheck> searcher(this, target, u_check);

        TypeContainerVisitor<JadeCore::UnitLastSearcher<JadeCore::NearestHostileUnitInAttackDistanceCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
        TypeContainerVisitor<JadeCore::UnitLastSearcher<JadeCore::NearestHostileUnitInAttackDistanceCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

        cell.Visit(p, world_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE > dist ? ATTACK_DISTANCE : dist);
        cell.Visit(p, grid_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE > dist ? ATTACK_DISTANCE : dist);
    }

    return target;
}

Player* Creature::SelectNearestPlayer(float distance) const
{
    Player* target = NULL;

    JadeCore::NearestPlayerInObjectRangeCheck checker(this, distance);
    JadeCore::PlayerLastSearcher<JadeCore::NearestPlayerInObjectRangeCheck> searcher(this, target, checker);
    VisitNearbyObject(distance, searcher);

    return target;
}

Player* Creature::SelectNearestPlayerNotGM(float distance) const
{
    Player* target = NULL;

    JadeCore::NearestPlayerNotGMInObjectRangeCheck checker(this, distance);
    JadeCore::PlayerLastSearcher<JadeCore::NearestPlayerNotGMInObjectRangeCheck> searcher(this, target, checker);
    VisitNearbyObject(distance, searcher);

    return target;
}

void Creature::SendAIReaction(AiReaction p_ReactionType)
{
    WorldPacket l_Data(SMSG_AI_REACTION, 16 + 2 + 4);
    l_Data.appendPackGUID(GetGUID());
    l_Data << uint32(p_ReactionType);

    ((WorldObject*)this)->SendMessageToSet(&l_Data, true);
}

void Creature::CallAssistance(float p_Radius /*= 0*/, bool p_CheckFaction /*= true*/)
{
    if (!m_AlreadyCallAssistance && getVictim() && !isPet() && !isCharmed())
    {
        SetNoCallAssistance(true);

        float radius = p_Radius ? p_Radius : sWorld->getFloatConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS);

        if (radius > 0)
        {
            std::list<Creature*> assistList;

            {
                CellCoord p(JadeCore::ComputeCellCoord(GetPositionX(), GetPositionY()));
                Cell cell(p);
                cell.SetNoCreate();

                JadeCore::AnyAssistCreatureInRangeCheck u_check(this, getVictim(), radius, p_CheckFaction);
                JadeCore::CreatureListSearcher<JadeCore::AnyAssistCreatureInRangeCheck> searcher(this, assistList, u_check);

                TypeContainerVisitor<JadeCore::CreatureListSearcher<JadeCore::AnyAssistCreatureInRangeCheck>, GridTypeMapContainer >  grid_creature_searcher(searcher);

                cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);
            }

            if (!assistList.empty())
            {
                AssistDelayEvent* e = new AssistDelayEvent(getVictim()->GetGUID(), *this);
                while (!assistList.empty())
                {
                    // Pushing guids because in delay can happen some creature gets despawned => invalid pointer
                    e->AddAssistant((*assistList.begin())->GetGUID());
                    assistList.pop_front();
                }
                m_Events.AddEvent(e, m_Events.CalculateTime(sWorld->getIntConfig(CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY)));
            }
        }
    }
}

void Creature::CallForHelp(float radius)
{
    if (radius <= 0.0f || !getVictim() || isPet() || isCharmed())
        return;

    CellCoord p(JadeCore::ComputeCellCoord(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    JadeCore::CallOfHelpCreatureInRangeDo u_do(this, getVictim(), radius);
    JadeCore::CreatureWorker<JadeCore::CallOfHelpCreatureInRangeDo> worker(this, u_do);

    TypeContainerVisitor<JadeCore::CreatureWorker<JadeCore::CallOfHelpCreatureInRangeDo>, GridTypeMapContainer >  grid_creature_searcher(worker);

    cell.Visit(p, grid_creature_searcher, *GetMap(), *this, radius);
}

bool Creature::CanAssistTo(const Unit* u, const Unit* enemy, bool checkfaction /*= true*/) const
{
    // is it true?
    if (!HasReactState(REACT_AGGRESSIVE))
        return false;

    // only creature not moving home
    if (GetMotionMaster()->GetCurrentMovementGeneratorType() == HOME_MOTION_TYPE)
        return false;

    // we don't need help from zombies :)
    if (!isAlive())
        return false;

    // we don't need help from non-combatant ;)
    if (isCivilian())
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC))
        return false;

    // skip fighting creature
    if (isInCombat())
        return false;

    // only free creature
    if (GetCharmerOrOwnerGUID())
        return false;

    // only from same creature faction
    if (checkfaction)
    {
        if (getFaction() != u->getFaction())
            return false;
    }
    else
    {
        if (!IsFriendlyTo(u))
            return false;
    }

    // skip non hostile to caster enemy creatures
    if (!IsHostileTo(enemy))
        return false;

    return true;
}

// use this function to avoid having hostile creatures attack
// friendlies and other mobs they shouldn't attack
bool Creature::_IsTargetAcceptable(const Unit* target) const
{
    ASSERT(target);

    // if the target cannot be attacked, the target is not acceptable
    if (IsFriendlyTo(target)
        || !target->isTargetableForAttack(false)
        || (m_vehicle && (IsOnVehicle(target) || m_vehicle->GetBase()->IsOnVehicle(target))))
        return false;

    if (target->HasUnitState(UNIT_STATE_DIED))
    {
        // guards can detect fake death
        if (isGuard() && target->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
            return true;
        else
            return false;
    }

    const Unit* myVictim = getAttackerForHelper();
    const Unit* tarGetVictim = target->getAttackerForHelper();

    // if I'm already fighting target, or I'm hostile towards the target, the target is acceptable
    if (myVictim == target || tarGetVictim == this || IsHostileTo(target))
        return true;

    // if the target's victim is friendly, and the target is neutral, the target is acceptable
    if (tarGetVictim && IsFriendlyTo(tarGetVictim))
        return true;

    // if the target's victim is not friendly, or the target is friendly, the target is not acceptable
    return false;
}

void Creature::StopAttack()
{
    SetReactState(REACT_PASSIVE);
    AttackStop();
    GetMotionMaster()->MoveIdle();
}

void Creature::SaveRespawnTime()
{
    if (isSummon() || !m_DBTableGuid || (m_creatureData && !m_creatureData->dbData))
        return;

    GetMap()->SaveCreatureRespawnTime(m_DBTableGuid, m_respawnTime);
}

// this should not be called by petAI or
bool Creature::canCreatureAttack(Unit const* victim, bool /*force*/) const
{
    if (!victim->IsInMap(this))
        return false;

    if (!IsValidAttackTarget(victim))
        return false;

    if (!victim->isInAccessiblePlaceFor(this))
        return false;

    if (IsAIEnabled && !AI()->CanAIAttack(victim))
        return false;

    if (IsAIEnabled && AI()->CanBypassResetRange())
        return true;

    if (sMapStore.LookupEntry(GetMapId())->IsDungeon())
        return true;

    //Use AttackDistance in distance check if threat radius is lower. This prevents creature bounce in and out of combat every update tick.
    float dist = std::max(GetAttackDistance(victim), sWorld->getFloatConfig(CONFIG_THREAT_RADIUS)) + m_CombatDistance;

    if (Unit* unit = GetCharmerOrOwner())
        return victim->IsWithinDist(unit, dist);
    else
        return victim->IsInDist(&m_homePosition, dist);
}

bool Creature::IsInCombatWithPlayer()
{
    for (HostileReference* l_Reference : getThreatManager().getThreatList())
    {
        Unit* l_Target = l_Reference->getTarget();
        if (l_Target && l_Target->GetSpellModOwner())
            return true;
    }

    return false;
}

CreatureAddon const* Creature::GetCreatureAddon() const
{
    if (m_DBTableGuid)
    {
        if (CreatureAddon const* addon = sObjectMgr->GetCreatureAddon(m_DBTableGuid))
            return addon;
    }

    // dependent from difficulty mode entry
    return sObjectMgr->GetCreatureTemplateAddon(GetCreatureTemplate()->Entry);
}

CreatureScript* Creature::GetCreatureScript()
{
    if (m_CreatureScript == nullptr)
        m_CreatureScript = sScriptMgr->GetCreatureScriptByID(GetScriptId());

    return m_CreatureScript;
}

/// creature_addon table
bool Creature::LoadCreaturesAddon()
{
    CreatureAddon const* l_CreatureAddon = GetCreatureAddon();
    if (!l_CreatureAddon)
        return false;

    if (l_CreatureAddon->Mount != 0)
        Mount(l_CreatureAddon->Mount);

    if (l_CreatureAddon->Bytes1 != 0)
    {
        /// 0 StandState
        /// 1 FreeTalentPoints   Pet only, so always 0 for default creature
        /// 2 StandFlags
        /// 3 StandMiscFlags

        SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_STAND_STATE, uint8(l_CreatureAddon->Bytes1 & 0xFF));
        ///SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_PET_TALENTS, uint8((cainfo->bytes1 >> 8) & 0xFF));
        SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_PET_TALENTS, 0);
        SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_VIS_FLAG, uint8((l_CreatureAddon->Bytes1 >> 16) & 0xFF));
        SetByteValue(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_ANIM_TIER, uint8((l_CreatureAddon->Bytes1 >> 24) & 0xFF));

        /// Suspected correlation between UNIT_FIELD_BYTES_1, offset 3, value 0x2:
        /// If no inhabittype_fly (if no MovementFlag_DisableGravity flag found in sniffs)
        /// Set MovementFlag_Hover. Otherwise do nothing.
        if (GetByteValue(UNIT_FIELD_ANIM_TIER, 3) & UNIT_BYTE1_FLAG_HOVER && isAlive())
            SetHover(true);
    }

    if (l_CreatureAddon->Bytes2 != 0)
    {
        /// 0 SheathState
        /// 1 PvpFlags
        /// 2 PetFlags           Pet only, so always 0 for default creature
        /// 3 ShapeshiftForm     Must be determined/set by shapeshift spell/aura

        SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_SHEATH_STATE, uint8(l_CreatureAddon->Bytes2 & 0xFF));
        ///SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, uint8((cainfo->bytes2 >> 8) & 0xFF));
        ///SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PET_FLAGS, uint8((cainfo->bytes2 >> 16) & 0xFF));
        SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PET_FLAGS, 0);
        ///SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_SHAPESHIFT_FORM, uint8((cainfo->bytes2 >> 24) & 0xFF));
        SetByteValue(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_SHAPESHIFT_FORM, 0);
    }

    if (l_CreatureAddon->Emote != 0)
        SetUInt32Value(UNIT_FIELD_EMOTE_STATE, l_CreatureAddon->Emote);

    if (l_CreatureAddon->aiAnimKit != 0)
        SetAIAnimKitId(l_CreatureAddon->aiAnimKit);

    if (l_CreatureAddon->movementAnimKit != 0)
        SetAIAnimKitId(l_CreatureAddon->movementAnimKit);

    if (l_CreatureAddon->meleeAnimKit != 0)
        SetAIAnimKitId(l_CreatureAddon->meleeAnimKit);

    /// Load Path
    if (l_CreatureAddon->PathID != 0)
        m_path_id = l_CreatureAddon->PathID;

    if (!l_CreatureAddon->Auras.empty())
    {
        for (std::vector<uint32>::const_iterator l_Iter = l_CreatureAddon->Auras.begin(); l_Iter != l_CreatureAddon->Auras.end(); ++l_Iter)
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(*l_Iter);
            if (!l_SpellInfo)
                continue;

            /// Skip already applied aura
            if (HasAura(*l_Iter))
                continue;

            bool l_Delay = false;

            if (l_SpellInfo->HasAura(AuraType::SPELL_AURA_AREATRIGGER))
                l_Delay = true;

            if (l_Delay)
            {
                uint32 l_SpellID = *l_Iter;
                AddDelayedEvent([this, l_SpellID]() -> void
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_SpellID);
                    if (!l_SpellInfo)
                        return;

                    /// Some auras doesn't start channeling by AddAura, so, we need to cast it
                    if (l_SpellInfo->AttributesEx & (SPELL_ATTR1_CHANNELED_1 | SPELL_ATTR1_CHANNELED_2))
                        CastSpell(this, l_SpellID, true);
                    else
                        AddAura(l_SpellID, this);
                }, 1);
            }
            else
            {
                /// Some auras doesn't start channeling by AddAura, so, we need to cast it
                if (l_SpellInfo->AttributesEx & (SPELL_ATTR1_CHANNELED_1 | SPELL_ATTR1_CHANNELED_2))
                    CastSpell(this, *l_Iter, true);
                else
                    AddAura(*l_Iter, this);
            }
        }
    }

    return true;
}

/// Send a message to LocalDefense channel for players opposition team in the zone
void Creature::SendZoneUnderAttackMessage(Player* attacker)
{
    uint32 enemy_team = attacker->GetTeam();

    WorldPacket data(SMSG_ZONE_UNDER_ATTACK, 4);
    data << uint32(GetAreaId());
    sWorld->SendGlobalMessage(&data, NULL, (enemy_team == ALLIANCE ? HORDE : ALLIANCE));
}

void Creature::SetInCombatWithZone()
{
    if (!CanHaveThreatList())
    {
        sLog->outError(LOG_FILTER_UNITS, "Creature entry %u call SetInCombatWithZone but creature cannot have threat list.", GetEntry());
        return;
    }

    Map* map = GetMap();

    if (!map->IsDungeon())
    {
        sLog->outError(LOG_FILTER_UNITS, "Creature entry %u call SetInCombatWithZone for map (id: %u) that isn't an instance.", GetEntry(), map->GetId());
        return;
    }

    Map::PlayerList const &PlList = map->GetPlayers();

    if (PlList.isEmpty())
        return;

    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
    {
        if (Player* player = i->getSource())
        {
            if (player->isGameMaster())
                continue;

            if (player->isAlive())
            {
                this->SetInCombatWith(player);
                player->SetInCombatWith(this);
                AddThreat(player, 0.0f);
            }
        }
    }
}

void Creature::_AddCreatureSpellCooldown(uint32 spell_id, time_t end_time)
{
    m_CreatureSpellCooldowns[spell_id] = end_time;
}

void Creature::_AddCreatureCategoryCooldown(uint32 category, time_t apply_time)
{
    m_CreatureCategoryCooldowns[category] = apply_time;
}

void Creature::AddCreatureSpellCooldown(uint32 spellid)
{
    auto spellInfo = sSpellMgr->GetSpellInfo(spellid);
    if (!spellInfo)
        return;

    uint32 baseCD = 6000;
    uint32 l_Cooldown = spellInfo->GetRecoveryTime();

    auto IsUsePower = [=]()
    {
        for (auto power : spellInfo->SpellPowers)
            return (power->Cost || power->CostBasePercentage || power->CostPerSecondPercentage || power->CostMaxPercentage);
        return false;
    };

    auto SendSpellCooldown = [=](uint32 cooldown)
    {
        if (auto l_CharmInfo = GetCharmInfo())
        {
            if (l_CharmInfo->GetCharmType() == CharmType::CHARM_TYPE_POSSESS)
            {
                if (auto l_Charmer = GetCharmer())
                {
                    if (auto l_CharmerPlayer = l_Charmer->ToPlayer())
                        l_CharmerPlayer->SendSpellCooldown(spellid, 0, cooldown);
                }
            }
        }
    };

    if (spellInfo->CalcCastTime() || IsUsePower())
        baseCD = 0;

    if (auto modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellid, SpellModOp::SPELLMOD_COOLDOWN, l_Cooldown);

    if (l_Cooldown)
    {
        _AddCreatureSpellCooldown(spellid, time(nullptr) + l_Cooldown / IN_MILLISECONDS);
        SendSpellCooldown(l_Cooldown);
    }
    else if (baseCD)
    {
        _AddCreatureSpellCooldown(spellid, time(nullptr) + baseCD / IN_MILLISECONDS);
        SendSpellCooldown(baseCD);
    }

    if (spellInfo->CategoryEntry)
        _AddCreatureCategoryCooldown(spellInfo->GetCategory(), time(nullptr));
}

bool Creature::HasCategoryCooldown(uint32 spell_id) const
{
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell_id);
    if (!spellInfo)
        return false;

    CreatureSpellCooldowns::const_iterator itr = m_CreatureCategoryCooldowns.find(spellInfo->GetCategory());
    return(itr != m_CreatureCategoryCooldowns.end() && time_t(itr->second + (spellInfo->CategoryRecoveryTime / IN_MILLISECONDS)) > time(NULL));
}

bool Creature::HasSpellCooldown(uint32 spell_id) const
{
    CreatureSpellCooldowns::const_iterator itr = m_CreatureSpellCooldowns.find(spell_id);
    return (itr != m_CreatureSpellCooldowns.end() && itr->second > time(NULL)) || HasCategoryCooldown(spell_id);
}

void Creature::ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs, uint32 p_SpellID /*= 0*/)
{
    time_t curTime = time(NULL);
    for (uint8 i = 0; i < MAX_CREATURE_SPELLS; ++i)
    {
        if (m_spells[i] == 0)
            continue;

        uint32 unSpellId = m_spells[i];
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(unSpellId);
        if (!spellInfo)
        {
            ASSERT(spellInfo);
            continue;
        }

        // Not send cooldown for this spells
        if (spellInfo->IsCooldownStartedOnEvent())
            continue;

        if ((spellInfo->PreventionType & (SpellPreventionMask::Silence)) == 0)
            continue;

        if ((idSchoolMask & spellInfo->GetSchoolMask()) && GetCreatureSpellCooldownDelay(unSpellId) < unTimeMs)
        {
            _AddCreatureSpellCooldown(unSpellId, curTime + unTimeMs/IN_MILLISECONDS);
            if (UnitAI* ai = GetAI())
                ai->SpellInterrupted(unSpellId, unTimeMs);
        }
    }

    if (!p_SpellID)
        return;

    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_SpellID);
    if (!l_SpellInfo)
        return;

    if (l_SpellInfo->IsCooldownStartedOnEvent())
        return;

    if ((l_SpellInfo->PreventionType & (SpellPreventionMask::Silence)) == 0)
        return;

    if ((idSchoolMask & l_SpellInfo->GetSchoolMask()) && GetCreatureSpellCooldownDelay(p_SpellID) < unTimeMs)
    {
        _AddCreatureSpellCooldown(p_SpellID, curTime + unTimeMs / IN_MILLISECONDS);
        if (UnitAI* ai = GetAI())
            ai->SpellInterrupted(p_SpellID, unTimeMs);
    }
}

bool Creature::HasSpell(uint32 spellID) const
{
    uint8 i;
    for (i = 0; i < MAX_CREATURE_SPELLS; ++i)
        if (spellID == m_spells[i])
            break;
    return i < MAX_CREATURE_SPELLS;                         //broke before end of iteration of known spells
}

time_t Creature::GetRespawnTimeEx() const
{
    time_t now = time(NULL);
    if (m_respawnTime > now)
        return m_respawnTime;
    else
        return now;
}

void Creature::GetRespawnPosition(float &x, float &y, float &z, float* ori, float* dist) const
{
    if (m_DBTableGuid)
    {
        if (CreatureData const* data = sObjectMgr->GetCreatureData(GetDBTableGUIDLow()))
        {
            x = data->posX;
            y = data->posY;
            z = data->posZ;
            if (ori)
                *ori = data->orientation;
            if (dist)
                *dist = data->spawndist;

            return;
        }
    }

    x = GetPositionX();
    y = GetPositionY();
    z = GetPositionZ();
    if (ori)
        *ori = GetOrientation();
    if (dist)
        *dist = 0;
}

void Creature::AllLootRemovedFromCorpse()
{
    if (!HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
    {
        time_t now = time(NULL);
        if (m_corpseRemoveTime <= now)
            return;

        float decayRate;
        CreatureTemplate const* cinfo = GetCreatureTemplate();

        decayRate = sWorld->getRate(RATE_CORPSE_DECAY_LOOTED);
        uint32 diff = uint32((m_corpseRemoveTime - now) * decayRate);

        m_respawnTime -= diff;

        // corpse skinnable, but without skinning flag, and then skinned, corpse will despawn next update
        if (cinfo && cinfo->SkinLootId)
            m_corpseRemoveTime = time(NULL);
        else
            m_corpseRemoveTime -= diff;
    }
}

uint8 Creature::getLevelForTarget(WorldObject const* target) const
{
    if (!isWorldBoss() || (target && !target->ToUnit()) || !target)
        return Unit::getLevelForTarget(target);

    uint16 level = target->ToUnit()->getLevel() + sWorld->getIntConfig(CONFIG_WORLD_BOSS_LEVEL_DIFF);
    if (level < 1)
        return 1;
    if (level > 255)
        return 255;
    return uint8(level);
}

std::string Creature::GetAIName() const
{
    return m_NativeCreatureInfo->AIName;
}

std::string Creature::GetScriptName() const
{
    return sObjectMgr->GetScriptName(GetScriptId());
}

uint32 Creature::GetScriptId() const
{
    if (CreatureData const* l_CreatureData = GetCreatureData())
        return l_CreatureData->ScriptId;

    return m_NativeCreatureInfo->ScriptID;
}

VendorItemData const* Creature::GetVendorItems() const
{
    return sObjectMgr->GetNpcVendorItemList(GetEntry());
}

uint32 Creature::GetVendorItemCurrentCount(VendorItem const* vItem)
{
    if (!vItem->maxcount)
        return vItem->maxcount;

    VendorItemCounts::iterator itr = m_vendorItemCounts.begin();
    for (; itr != m_vendorItemCounts.end(); ++itr)
        if (itr->itemId == vItem->item)
            break;

    if (itr == m_vendorItemCounts.end())
        return vItem->maxcount;

    VendorItemCount* vCount = &*itr;

    time_t ptime = time(NULL);

    if (time_t(vCount->lastIncrementTime + vItem->incrtime) <= ptime)
    {
        ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(vItem->item);

        uint32 diff = uint32((ptime - vCount->lastIncrementTime)/vItem->incrtime);
        if ((vCount->count + diff * pProto->BuyCount) >= vItem->maxcount)
        {
            m_vendorItemCounts.erase(itr);
            return vItem->maxcount;
        }

        vCount->count += diff * pProto->BuyCount;
        vCount->lastIncrementTime = ptime;
    }

    return vCount->count;
}

uint32 Creature::UpdateVendorItemCurrentCount(VendorItem const* vItem, uint32 used_count)
{
    if (!vItem->maxcount)
        return 0;

    VendorItemCounts::iterator itr = m_vendorItemCounts.begin();
    for (; itr != m_vendorItemCounts.end(); ++itr)
        if (itr->itemId == vItem->item)
            break;

    if (itr == m_vendorItemCounts.end())
    {
        uint32 new_count = vItem->maxcount > used_count ? vItem->maxcount-used_count : 0;
        m_vendorItemCounts.push_back(VendorItemCount(vItem->item, new_count));
        return new_count;
    }

    VendorItemCount* vCount = &*itr;

    time_t ptime = time(NULL);

    if (time_t(vCount->lastIncrementTime + vItem->incrtime) <= ptime)
    {
        ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(vItem->item);

        uint32 diff = uint32((ptime - vCount->lastIncrementTime)/vItem->incrtime);
        if ((vCount->count + diff * pProto->BuyCount) < vItem->maxcount)
            vCount->count += diff * pProto->BuyCount;
        else
            vCount->count = vItem->maxcount;
    }

    vCount->count = vCount->count > used_count ? vCount->count-used_count : 0;
    vCount->lastIncrementTime = ptime;
    return vCount->count;
}

TrainerSpellData const* Creature::GetTrainerSpells() const
{
    return sObjectMgr->GetNpcTrainerSpells(GetEntry());
}

// overwrite WorldObject function for proper name localization
const char* Creature::GetNameForLocaleIdx(LocaleConstant loc_idx) const
{
    if (loc_idx != DEFAULT_LOCALE)
    {
        uint8 uloc_idx = uint8(loc_idx);
        CreatureLocale const* cl = sObjectMgr->GetCreatureLocale(GetEntry());
        if (cl)
        {
            if (cl->Name[0].size() > uloc_idx && !cl->Name[0][uloc_idx].empty())
                return cl->Name[0][uloc_idx].c_str();
        }
    }

    return GetName();
}

//Do not if this works or not, moving creature to another map is very dangerous
void Creature::FarTeleportTo(Map* map, float X, float Y, float Z, float O)
{
# ifndef CROSS
    CleanupBeforeRemoveFromMap(false);
# else
    if (GetMapSwitchDestination() == -1)
        CleanupBeforeRemoveFromMap(false);
# endif

    GetMap()->RemoveFromMap(this, false);
    uint64 l_CreatureGuid = GetGUID();

    map->AddTask([l_CreatureGuid, X, Y, Z, O, map]() -> void
    {
        Creature* l_Creature = sObjectAccessor->FindCreature(l_CreatureGuid);
        if (!l_Creature)
            return;

        l_Creature->Relocate(X, Y, Z, O);
        l_Creature->SetMap(map);
        map->AddToMap(l_Creature);
    });
}

void Creature::SetPosition(float x, float y, float z, float o)
{
    // prevent crash when a bad coord is sent by the client
    if (!JadeCore::IsValidMapCoord(x, y, z, o))
    {
        sLog->outDebug(LOG_FILTER_UNITS, "Creature::SetPosition(%f, %f, %f) .. bad coordinates!", x, y, z);
        return;
    }

    GetMap()->CreatureRelocation(ToCreature(), x, y, z, o);
    if (IsVehicle())
        GetVehicleKit();
}

bool Creature::IsDungeonBoss() const
{
    CreatureTemplate const* cinfo = sObjectMgr->GetCreatureTemplate(GetEntry());
    return cinfo && (GetFlagsExtra() & CREATURE_FLAG_EXTRA_DUNGEON_BOSS);
}

bool Creature::IsPersonal() const
{
    return (m_TrackingQuestID || isWorldBoss() || (GetFlagsExtra() & CREATURE_FLAG_EXTRA_PERSONAL_LOOT)) && GetCreatureTemplate()->expansion >= EXPANSION_MISTS_OF_PANDARIA;
}

bool Creature::IsPersonalForQuest(Player const* player) const
{
    if (GetCreatureTemplate()->QuestPersonalLoot)
        return player->HasQuestForCreature(GetCreatureTemplate());
    return false;
}

float Creature::GetAggroRange(Unit const* target) const
{
    // Determines the aggro range for creatures (usually pets), used mainly for aggressive pet target selection.
    // Based on data from wowwiki due to lack of 3.3.5a data

    if (target && this->isPet())
    {
        uint32 targetLevel = 0;

        if (target->IsPlayer())
            targetLevel = target->getLevelForTarget(this);
        else if (target->GetTypeId() == TYPEID_UNIT)
            targetLevel = target->ToCreature()->getLevelForTarget(this);

        uint32 myLevel = getLevelForTarget(target);
        int32 levelDiff = int32(targetLevel) - int32(myLevel);

        // The maximum Aggro Radius is capped at 45 yards (25 level difference)
        if (levelDiff < -25)
            levelDiff = -25;

        // The base aggro radius for mob of same level
        float aggroRadius = 20;

        // Aggro Radius varies with level difference at a rate of roughly 1 yard/level
        aggroRadius -= (float)levelDiff;

        // detect range auras
        aggroRadius += GetTotalAuraModifier(SPELL_AURA_MOD_DETECT_RANGE);

        // detected range auras
        aggroRadius += target->GetTotalAuraModifier(SPELL_AURA_MOD_DETECTED_RANGE);

        // Just in case, we don't want pets running all over the map
        if (aggroRadius > MAX_AGGRO_RADIUS)
            aggroRadius = MAX_AGGRO_RADIUS;

        // Minimum Aggro Radius for a mob seems to be combat range (5 yards)
        //  hunter pets seem to ignore minimum aggro radius so we'll default it a little higher
        if (aggroRadius < 10)
            aggroRadius = 10;

        return (aggroRadius);
    }

    // Default
    return 0.0f;
}

Unit* Creature::SelectNearestHostileUnitInAggroRange(bool useLOS) const
{
    // Selects nearest hostile target within creature's aggro range. Used primarily by
    //  pets set to aggressive. Will not return neutral or friendly targets.

    Unit* target = NULL;

    {
        JadeCore::NearestHostileUnitInAggroRangeCheck u_check(this, useLOS);
        JadeCore::UnitSearcher<JadeCore::NearestHostileUnitInAggroRangeCheck> searcher(this, target, u_check);

        VisitNearbyGridObject(MAX_AGGRO_RADIUS, searcher);
    }

    return target;
}

void Creature::DumpGroup()
{
    Map* l_Map = GetMap();

    /// Only work in dungeon / raid
    if (!l_Map->IsDungeon() || (GetFlagsExtra() & CREATURE_FLAG_EXTRA_LOG_GROUP_DMG) == 0)
        return;

    std::ostringstream l_Dump;

    l_Dump << "Timestamp : " << time(nullptr) << std::endl;

    const Map::PlayerList& l_Players = l_Map->GetPlayers();
    for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
    {
        Player* l_Player = l_Itr->getSource();
        l_Dump << "[Player " << l_Player->GetName() << " " << l_Player->GetGUIDLow() << " aura list : ]" << std::endl;

        for (uint16 i = 0; i < TOTAL_AURAS; ++i)
        {
            Unit::AuraEffectList const& auraList = l_Player->GetAuraEffectsByType(AuraType(i));
            if (auraList.empty())
                continue;

            for (Unit::AuraEffectList::const_iterator itr = auraList.begin(); itr != auraList.end(); ++itr)
                l_Dump << "----> " << (*itr)->GetAuraType() << " [" << (*itr)->GetSpellInfo()->SpellName << " (" << (*itr)->GetId() << ")] amount : " << (*itr)->GetAmount() << std::endl;
        }
    }

    GroupDump l_GroupDump;
    l_GroupDump.Time = time(nullptr);
    l_GroupDump.Dump = l_Dump.str();

    AddGroupDump(l_GroupDump);
}

void Creature::ClearLootContainers()
{
    m_LootContainers.clear();
    RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE|UNIT_DYNFLAG_TAPPED);
}

bool Creature::IsDireBeast() const
{
    for (uint32 l_SpellId : sSpellMgr->GetDireBeastSpells())
    {
        CreatureTemplate const* l_CreatureTemplate = sObjectMgr->GetCreatureTemplate(sSpellMgr->GetSpellInfo(l_SpellId)->Effects[EFFECT_0].MiscValue);
        if (!l_CreatureTemplate)
            continue;

        if (l_CreatureTemplate->Entry == GetEntry())
            return true;
    }

    return false;
}

bool Creature::IsHati() const
{
    switch (ToCreature()->GetEntry())
    {
        case 100324:
            return true;
        default:
            return false;
    }

    return false;
}

std::string SHA1Hash(std::string const& content)
{
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)content.c_str(), content.length(), (unsigned char*)&digest);

    return ByteArrayToHexStr(digest, SHA_DIGEST_LENGTH);
}

std::string Creature::ComputeLinkedId() const
{
    if (!m_creatureData)
        return "";

    int l_X     = (float)(std::round(m_creatureData->posX / 0.25f));
    int l_Y     = (float)(std::round(m_creatureData->posY / 0.25f));
    int l_Z     = (float)(std::round(m_creatureData->posZ / 0.25f));
    int l_Entry = GetEntry();

    std::string l_LinkedHash = std::to_string(l_X) + " " + std::to_string(l_Y) + " " + std::to_string(l_Z) + " " + std::to_string(m_creatureData->id) + " " + std::to_string(m_creatureData->mapid) + " ";
    l_LinkedHash += std::to_string(m_creatureData->phaseID) + " " + std::to_string(m_creatureData->phaseMask) + " " + std::to_string(m_creatureData->spawnMask);

    std::string l_Hash = SHA1Hash(l_LinkedHash);
    std::transform(l_Hash.begin(), l_Hash.end(), l_Hash.begin(), ::toupper);
    return l_Hash;
}

bool Creature::CanFly() const
{
    return (GetCreatureTemplate()->InhabitType & INHABIT_AIR) && (!IsAIEnabled || AI()->CanFly());
}

void Creature::AddToGrid(GridRefManager<Creature>& m)
{
    GridObject<Creature>::AddToGrid(m);
    m_Cells = Cell::CalculateCellArea(m_positionX, m_positionY, GetCombatReach());
    if (!!m_Cells)
    {
        RemoveNearbyFromGrids();
        AddToNearbyGrids();
    }
}

void Creature::AddToNearbyGrids()
{
    CellCoord l_CellCoords = JadeCore::ComputeCellCoord(m_positionX, m_positionY);

    // loop the cell range
    for (uint32 l_X = m_Cells.low_bound.x_coord; l_X <= m_Cells.high_bound.x_coord; ++l_X)
    {
        for (uint32 l_Y = m_Cells.low_bound.y_coord; l_Y <= m_Cells.high_bound.y_coord; ++l_Y)
        {
            CellCoord l_NearbyCellCoords(l_X, l_Y);
            //lets skip standing cell since we already added to it
            if (l_NearbyCellCoords != l_CellCoords)
            {
                Cell r_zone(l_NearbyCellCoords);
                GetMap()->AddNearbyToGrid(this, r_zone);
            }
        }
    }
}

void Creature::UpdatePresenceForNearbyGrids()
{
    CellCoord cellCoord = JadeCore::ComputeCellCoord(m_positionX, m_positionY);
    CellArea l_Cells = Cell::CalculateCellArea(m_positionX, m_positionY, GetCombatReach());
    if (l_Cells == m_Cells)
        return;

    m_Cells = l_Cells;

    if (!!l_Cells)
    {
        RemoveNearbyFromGrids();
        AddToNearbyGrids();
    }
}

void Creature::SetFlagExtra(uint32 p_Flag, bool p_Apply /*= true*/)
{
    if (p_Apply)
        m_FlagExtras |= p_Flag;
    else
        m_FlagExtras &= ~p_Flag;
}

void Creature::InitDireBeastDisplayIdAndScale(uint32& displayId, float& scale)
{
    auto& iter = sSpellMgr->GetDireBeastEntries();

    if (iter.find(GetEntry()) == iter.end())
        return;

    auto owner = GetAnyOwner();
    if (!owner)
        return;

    auto player = owner->ToPlayer();
    if (!player)
        return;

    if (player->HasAura(219183)) // Glyph of the Dire Stable
    {
        if (!player->m_stablePetModels.empty())
        {
            auto rand = JadeCore::Containers::SelectRandomContainerElement(player->m_stablePetModels);

            displayId = rand.first;
            auto direBeastEntry = rand.second;

            auto cInfoBeast = sObjectMgr->GetCreatureTemplate(direBeastEntry);
            if (cInfoBeast)
            {
                auto cFamily = sCreatureFamilyStore.LookupEntry(cInfoBeast->family);
                if (cFamily && cFamily->minScale > 0.0f)
                {
                    // Hack
                    switch (cFamily->ID)
                    {
                        case 39:
                            scale = 0.3f - 0.05f;
                            if ((cInfoBeast->flags_extra & CREATURE_FLAG_EXTRA_DUNGEON_BOSS) || cInfoBeast->rank == CREATURE_ELITE_WORLDBOSS)
                                scale = 0.1f;
                            break;
                        case 45:
                            scale = 0.5f + 0.5f * float(getLevel() - cFamily->minScaleLevel) / float(cFamily->maxScaleLevel - cFamily->minScaleLevel);
                            break;
                        default:
                            if (getLevel() >= cFamily->maxScaleLevel)
                                scale = 1.0f;
                            else if (getLevel() <= cFamily->minScaleLevel)
                                scale = 0.5f;
                            else
                                scale = 0.5f + 0.5f * float(getLevel() - cFamily->minScaleLevel) / float(cFamily->maxScaleLevel - cFamily->minScaleLevel);
                            break;
                    }
                }
            }
        }
    }
}