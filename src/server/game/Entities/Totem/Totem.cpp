////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Totem.h"
#include "WorldPacket.h"
#include "Log.h"
#include "Group.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "SpellInfo.h"

Totem::Totem(SummonPropertiesEntry const* properties, Unit* owner) : Minion(properties, owner, false)
{
    m_unitTypeMask |= UNIT_MASK_TOTEM;
    m_duration = 0;
    m_type = TOTEM_PASSIVE;
}

void Totem::Update(uint32 time)
{
    Unit* l_Owner = GetSummoner();
    if ((!l_Owner || !l_Owner->isAlive()) || !isAlive())
    {
        UnSummon();                                         // remove self
        return;
    }

    if (m_duration <= time)
    {
        UnSummon();                                         // remove self
        return;
    }
    else
        m_duration -= time;

    Creature::Update(time);
}

void Totem::InitStats(uint32 duration, Player* p_PlayerSummoner /*= nullptr*/)
{
    uint32 spellId1 = 0;
    uint32 spellId2 = 0;
    uint32 spellId3 = 0;
    uint32 spellId4 = 0;

    Unit* l_Owner = GetSummoner();
    if (l_Owner != nullptr)
    {
        std::vector<Totem*> m_TotemUnsummonList;
        int32 l_HealingStreamTotemCount = 0;

        for (Unit* l_Controlled : l_Owner->m_Controlled)
        {
            /// Earthquake Totem is not supposed to be unique
            if (l_Controlled->GetEntry() == 95255)
                continue;

            if (l_Controlled->GetEntry() == 3527 && l_Owner->HasAura(108283))   /// Echo of the Elements allow to spawn 2 Healing Stream Totem
            {
                ++l_HealingStreamTotemCount;
                if (l_HealingStreamTotemCount < 2 || l_Controlled->GetEntry() != GetEntry())
                    continue;
            }

            if (l_Controlled->GetGUID() != GetGUID() && l_Controlled->GetEntry() == GetEntry() && l_Controlled->ToTotem())
                m_TotemUnsummonList.push_back(l_Controlled->ToTotem());
        }

        for (Totem* l_Totem : m_TotemUnsummonList)
        {
            if (l_Totem == nullptr)
                continue;
            l_Totem->UnSummon();
        }

        // set display id depending on caster's race
        if (l_Owner->getClass() == CLASS_SHAMAN)
            SetDisplayId(l_Owner->GetModelForTotem(uint32(m_Properties->Id)));
    }

    if (l_Owner && l_Owner->IsPlayer()
        && m_Properties->Slot >= SUMMON_SLOT_TOTEM
        && m_Properties->Slot < MAX_TOTEM_SLOT)
    {
        // Light's Hammer
        if (GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL) == 122773)
            SetDisplayId(11686);
    }

    Minion::InitStats(duration, p_PlayerSummoner);

    // Get spell cast by totem
    if (SpellInfo const* totemSpell = sSpellMgr->GetSpellInfo(GetSpell()))
        if (totemSpell->CalcCastTime())   // If spell has cast time -> its an active totem
            m_type = TOTEM_ACTIVE;

    m_duration = duration;

    SetLevel(l_Owner ? l_Owner->getLevel() : 1);

    // Totems must receive stamina from owner
    switch (GetEntry())
    {
        case STONECLAW_TOTEM_ENTRY:
            SetModifierValue(UNIT_MOD_STAT_STAMINA, BASE_VALUE, float(l_Owner ? l_Owner->GetStat(STAT_STAMINA) : 1) * 0.1f);
            break;
        default:
            break;
    }

    if (spellId1 && l_Owner)
        l_Owner->CastSpell(l_Owner, spellId1, true); // Fake Fire Totem
    if (spellId2 && l_Owner)
        l_Owner->CastSpell(l_Owner, spellId2, true); // Fake Earth Totem
    if (spellId3 && l_Owner)
        l_Owner->CastSpell(l_Owner, spellId3, true); // Fake Water Totem
    if (spellId4 && l_Owner)
        l_Owner->CastSpell(l_Owner, spellId4, true); // Fake Wind Totem
}

void Totem::InitSummon()
{
    TempSummon::InitSummon();

    if (m_type == TOTEM_PASSIVE && GetSpell())
        CastSpell(this, GetSpell(), true);

    // Some totems can have both instant effect and passive spell
    if (GetSpell(1))
        CastSpell(this, GetSpell(1), true);
}

void Totem::UnSummon(uint32 msTime)
{
    if (msTime)
    {
        m_Events.AddEvent(new ForcedUnsummonDelayEvent(*this), m_Events.CalculateTime(msTime));
        return;
    }

    /// Call creature just died function
    if (IsAIEnabled)
        ToCreature()->AI()->JustDespawned();

    Unit* l_Owner = GetSummoner();

    CombatStop();
    RemoveAurasDueToSpell(GetSpell(), GetGUID());

    // clear owner's totem slot
    for (int i = SUMMON_SLOT_TOTEM; i < MAX_TOTEM_SLOT * 2; ++i)
    {
        if (l_Owner && l_Owner->m_SummonSlot[i] == GetGUID())
        {
            l_Owner->m_SummonSlot[i] = 0;
            break;
        }
    }

    if (l_Owner != nullptr)
        l_Owner->RemoveAurasDueToSpell(GetSpell(), GetGUID());

    //remove aura all party members too
    if (l_Owner != nullptr)
    {
        if (Player* owner = l_Owner->ToPlayer())
        {
            owner->SendAutoRepeatCancel(this);

            if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL)))
                owner->SendCooldownEvent(spell, 0, NULL, false);

            if (GroupPtr group = owner->GetGroup())
            {
                group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Member) -> void
                {
                    Player* target = l_Member->player;
                    if (target && group->SameSubGroup(owner, target))
                        target->RemoveAurasDueToSpell(GetSpell(), GetGUID());
                });
            }
        }
    }

    AddObjectToRemoveList();
}

bool Totem::IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const
{
    /// Grounding Totem and Void Tendrils
    if (GetEntry() == 5925 || GetEntry() == 65282)
        return false;

    switch (spellInfo->Effects[index].ApplyAuraName)
    {
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_SCHOOL_DAMAGE:
        case SPELL_AURA_PERIODIC_LEECH:
        case SPELL_AURA_MOD_FEAR:
        case SPELL_AURA_MOD_FEAR_2:
        case SPELL_AURA_TRANSFORM:
            return true;
        default:
            break;
    }

    return Creature::IsImmunedToSpellEffect(spellInfo, index);
}

uint32 Totem::GetSpell(uint8 slot)
{
    return m_spells[slot];
}
