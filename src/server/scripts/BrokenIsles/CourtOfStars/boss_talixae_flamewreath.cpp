////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "court_of_stars.hpp"

enum Says
{
    SAY_AGGRO_BAD   = 3,
    SAY_KILL        = 4,
    SAY_AGGRO       = 5,
    SAY_DEATH       = 6,
    SAY_BURNING     = 7
};

enum Spells
{
    SPELL_BURNING_INTENSITY     = 207906,
    SPELL_WITHERING_SOUL        = 208165,
    SPELL_INFERNAL_ERUPTION     = 207881,
    SPELL_INFERNAL_ERUPTION_TRIGGER = 207883,
    SPELL_INFERNAL_ERUPTION_DMG = 207887,

    SPELL_BOND_OF_FLAME         = 209722, ///< Real buff ID...
    SPELL_BOND_OF_STRENGTH      = 207850, ///< Real buff ID...
    SPELL_BOND_OF_CRUELTY       = 209719, ///< Real buff ID...
    SPELL_BOND_OF_CUNNING       = 209713, ///< Real buff ID...

    SPELL_BOND_OF_FLAME_AT      = 209723, ///< Boss
    SPELL_BOND_OF_STRENGTH_AT   = 207819, ///< NPC_JAZSHARIU
    SPELL_BOND_OF_CRUELTY_AT    = 209717, ///< NPC_BAALGAR_THE_WATCHFUL
    SPELL_BOND_OF_CUNNING_AT    = 209712, ///< NPC_IMACUTYA

    SPELL_WHIRLING_BLADES       = 209378,
    SPELL_DISINTEGRATION_BEAM   = 207980,
    SPELL_SHOCKWAVE             = 207979
};

enum eEvents
{
    EVENT_BURNING_INTENSITY     = 1,
    EVENT_WITHERING_SOUL        = 2,
    EVENT_INFERNAL_ERUPTION     = 3
};

Position const g_BossPos[3] =
{
    { 1081.21f, 3313.43f, 25.05f, 0.23f }, ///< NPC_JAZSHARIU
    { 1083.43f, 3307.68f, 24.98f, 0.81f }, ///< NPC_BAALGAR_THE_WATCHFUL
    { 1088.88f, 3306.01f, 25.05f, 1.43f }  ///< NPC_IMACUTYA
};

Position const g_PatrolPos[4] = ///< NPC_FELBOUND_ENFORCER
{
    { 1161.85f, 3279.33f, 20.00f, 3.82f }, ///< Left
    { 1041.14f, 3388.77f, 19.96f, 3.27f }, ///< Right
    { 1182.30f, 3350.27f, 20.00f, 1.57f }, ///< Left
    { 1069.85f, 3410.11f, 19.83f, 3.89f }  ///< Right
};

Position const g_CheckPos[3] =
{
    { 1133.87f, 3278.71f, 20.10f, 0.0f },
    { 1156.15f, 3327.12f, 20.90f, 0.0f },
    { 1053.71f, 3357.99f, 19.84f, 0.0f }
};

const uint8 g_TacticEnforcerLeft [] = { 0, 1, 2, 3 };
const uint8 g_TacticEnforcerRight[] = { 1, 0, 3, 2 };

static bool BringAndDistractNearestGuardian(Creature* p_Creature, bool InstKill = false)
{
    std::list<Creature*> l_Creatures;

    if (Creature* l_Creature = GetClosestCreatureWithEntry(p_Creature, NPC_JAZSHARIU, 150.0f))
        if (!l_Creature->isInCombat() && l_Creature->HasAura(Spells::SPELL_BOND_OF_STRENGTH))
            l_Creatures.push_back(l_Creature);

    if (Creature* l_Creature = GetClosestCreatureWithEntry(p_Creature, NPC_IMACUTYA, 150.0f))
        if (!l_Creature->isInCombat() && l_Creature->HasAura(Spells::SPELL_BOND_OF_CUNNING))
            l_Creatures.push_back(l_Creature);

    if (Creature* l_Creature = GetClosestCreatureWithEntry(p_Creature, NPC_BAALGAR_THE_WATCHFUL, 150.0f))
        if (!l_Creature->isInCombat() && l_Creature->HasAura(Spells::SPELL_BOND_OF_CRUELTY))
            l_Creatures.push_back(l_Creature);

    if (l_Creatures.empty())
        return false;

    JadeCore::RandomResizeList(l_Creatures, uint32(1));
    l_Creatures.front()->SetHomePosition(*p_Creature);
    ///< 9/4/2018 Fixed affix for Mythic+
    l_Creatures.front()->RemoveAurasDueToSpell(SPELL_BOND_OF_FLAME);
    l_Creatures.front()->RemoveAurasDueToSpell(SPELL_BOND_OF_STRENGTH);
    l_Creatures.front()->RemoveAurasDueToSpell(SPELL_BOND_OF_CUNNING);
    l_Creatures.front()->RemoveAurasDueToSpell(SPELL_BOND_OF_CRUELTY);

    l_Creatures.front()->GetMotionMaster()->MovePoint(1, *p_Creature);

    if (InstKill)
    {
        if (l_Creatures.front()->AI())
            l_Creatures.front()->AI()->DoAction(0);
    }

    return true;
}

/// Talixae Flamewreath <The Emissary> - 104217
class boss_talixae_flamewreath : public CreatureScript
{
    public:
        boss_talixae_flamewreath() : CreatureScript("boss_talixae_flamewreath") { }

        struct boss_talixae_flamewreathAI : public BossAI
        {
            boss_talixae_flamewreathAI(Creature* p_Creature) : BossAI(p_Creature, DATA_TALIXAE)
            {
                SummonEventCreature();
            }

            uint64 m_GuardsGuid[3];

            uint8 m_DiedCount;
            uint8 m_GuardsDiedCount;
            uint64 m_EnforcerGuid[4];
            uint8 m_TacticEnforcer;

            std::list<uint64> m_HitTargets;
            uint8 CountSummCombat;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void Reset() override
            {
                _Reset();

                me->RemoveAurasDueToSpell(SPELL_BOND_OF_FLAME);
                if (m_GuardsDiedCount < 3)
                    DoCast(me, SPELL_BOND_OF_FLAME_AT, true);

                m_HitTargets.clear();

                if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_BAALGAR_THE_WATCHFUL, 150.0f))
                    if (!me->HasAura(Spells::SPELL_BOND_OF_CRUELTY))
                        l_Creature->AddAura(SPELL_BOND_OF_CRUELTY, me);

                if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_IMACUTYA, 150.0f))
                    if (!me->HasAura(Spells::SPELL_BOND_OF_CUNNING))
                        l_Creature->AddAura(SPELL_BOND_OF_CUNNING, me);

                if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_JAZSHARIU, 150.0f))
                    if (!me->HasAura(Spells::SPELL_BOND_OF_STRENGTH))
                        l_Creature->AddAura(Spells::SPELL_BOND_OF_STRENGTH, me);
            }

            void SummonEventCreature()
            {
                m_DiedCount = 0;
                m_GuardsDiedCount = 0;
                m_TacticEnforcer = urand(0, 1);

                if (Creature* l_Guard = me->SummonCreature(NPC_JAZSHARIU, g_BossPos[0]))
                    m_GuardsGuid[2] = l_Guard->GetGUID();
                if (Creature* l_Guard = me->SummonCreature(NPC_BAALGAR_THE_WATCHFUL, g_BossPos[1]))
                    m_GuardsGuid[1] = l_Guard->GetGUID();
                if (Creature* l_Guard = me->SummonCreature(NPC_IMACUTYA, g_BossPos[2]))
                    m_GuardsGuid[0] = l_Guard->GetGUID();

                for (int8 l_I = 0; l_I < 4; l_I++)
                {
                    if (Creature* l_Enforcer = me->SummonCreature(NPC_FELBOUND_ENFORCER, g_PatrolPos[l_I]))
                    {
                        m_EnforcerGuid[l_I] = l_Enforcer->GetGUID();
                        l_Enforcer->SetDynamicValue(UNIT_DYNAMIC_FIELD_WORLD_EFFECTS, 0, 2101);
                        if (m_TacticEnforcer != l_I)
                        {
                            l_Enforcer->SetVisible(false);
                            l_Enforcer->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        }
                    }
                }
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /*p_Killer*/) override
            {
                switch (p_Summon->GetEntry())
                {
                    case NPC_FELBOUND_ENFORCER:
                    {
                        if (p_Summon->IsAIEnabled)
                            p_Summon->AI()->Talk(0);

                        const uint8 *l_EnforcerTypes = (m_TacticEnforcer == 1 ? g_TacticEnforcerRight : g_TacticEnforcerLeft);
                        for (int8 l_I = 0; l_I < 4; l_I++)
                        {
                            if (Creature* l_Enforcer = Creature::GetCreature(*me, m_EnforcerGuid[l_EnforcerTypes[l_I]]))
                            {
                                if (l_Enforcer->isAlive() && !l_Enforcer->isInCombat())
                                {
                                    l_Enforcer->SetVisible(true);
                                    l_Enforcer->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                    break;
                                }
                            }
                        }

                        if (BringAndDistractNearestGuardian(p_Summon))
                        {
                            Talk(m_DiedCount);
                            m_DiedCount++;
                        }
                        break;
                    }
                    case NPC_JAZSHARIU:
                    {
                        m_GuardsDiedCount++;

                        if (m_GuardsDiedCount == 3)
                        {
                            me->RemoveAurasDueToSpell(SPELL_BOND_OF_FLAME_AT);
                            me->RemoveAurasDueToSpell(SPELL_BOND_OF_FLAME);
                        }

                        /// Proper handling of buff auras
                        {
                            /// NPC_BAALGAR_THE_WATCHFUL
                            if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[1]))
                                l_Creature->RemoveAura(SPELL_BOND_OF_STRENGTH);

                            /// NPC_IMACUTYA
                            if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[0]))
                                l_Creature->RemoveAura(SPELL_BOND_OF_STRENGTH);

                            me->RemoveAura(SPELL_BOND_OF_STRENGTH);
                        }
                        break;
                    }
                    case NPC_BAALGAR_THE_WATCHFUL:
                    {
                        m_GuardsDiedCount++;

                        if (m_GuardsDiedCount == 3)
                        {
                            me->RemoveAurasDueToSpell(SPELL_BOND_OF_FLAME_AT);
                            me->RemoveAurasDueToSpell(SPELL_BOND_OF_FLAME);
                        }

                        /// Proper handling of buff auras
                        {
                            /// NPC_JAZSHARIU
                            if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[2]))
                                l_Creature->RemoveAura(SPELL_BOND_OF_CRUELTY);

                            /// NPC_IMACUTYA
                            if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[0]))
                                l_Creature->RemoveAura(SPELL_BOND_OF_CRUELTY);

                            me->RemoveAura(SPELL_BOND_OF_CRUELTY);
                        }
                        break;
                    }
                    case NPC_IMACUTYA:
                    {
                        m_GuardsDiedCount++;

                        if (m_GuardsDiedCount == 3)
                        {
                            me->RemoveAurasDueToSpell(SPELL_BOND_OF_FLAME_AT);
                            me->RemoveAurasDueToSpell(SPELL_BOND_OF_FLAME);
                        }

                        /// Proper handling of buff auras
                        {
                            /// NPC_BAALGAR_THE_WATCHFUL
                            if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[1]))
                                l_Creature->RemoveAura(SPELL_BOND_OF_CUNNING);

                            /// NPC_JAZSHARIU
                            if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[2]))
                                l_Creature->RemoveAura(SPELL_BOND_OF_CUNNING);

                            me->RemoveAura(SPELL_BOND_OF_CUNNING);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterCombat(Unit* p_Who) override
            {
                if (m_GuardsDiedCount < 3)
                    Talk(SAY_AGGRO_BAD);
                else
                    Talk(SAY_AGGRO);

                _EnterCombat();

                for (int8 l_I = 0; l_I < 3; l_I++)
                {
                    if (Creature* l_Guard = me->GetCreature(*me, m_GuardsGuid[l_I]))
                    {
                        if (l_Guard && l_Guard->IsAIEnabled && l_Guard->isAlive())
                            l_Guard->AI()->AttackStart(p_Who);
                    }
                }
                
                CountSummCombat = 0;
                if (me->HasAura(Spells::SPELL_BOND_OF_CRUELTY))
                    CountSummCombat += 1;
                if (me->HasAura(Spells::SPELL_BOND_OF_CUNNING))
                    CountSummCombat += 2;
                if (me->HasAura(Spells::SPELL_BOND_OF_STRENGTH))
                    CountSummCombat += 4;

                events.ScheduleEvent(EVENT_BURNING_INTENSITY, 6000);
                events.ScheduleEvent(EVENT_WITHERING_SOUL, 12000);
                events.ScheduleEvent(EVENT_INFERNAL_ERUPTION, 20000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(SAY_DEATH);

                _JustDied();

                if (InstanceScript* l_InsCript = me->GetInstanceScript())
                {
                    if (Creature* l_Lilet = me->FindNearestCreature(NPC_LYLETH_LUNASTRE, 100.0f, true))
                    {
                        l_Lilet->DeMorph();
                        l_Lilet->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_DISARMED | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);

                        if (CreatureAI* l_AI = l_Lilet->AI())
                            l_AI->Talk(0);
                    }
                }

                /// Proper handling of buff auras
                {
                    /// NPC_BAALGAR_THE_WATCHFUL
                    if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[1]))
                        l_Creature->RemoveAura(SPELL_BOND_OF_FLAME);

                    /// NPC_IMACUTYA
                    if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[0]))
                        l_Creature->RemoveAura(SPELL_BOND_OF_FLAME);

                    /// NPC_JAZSHARIU
                    if (Creature* l_Creature = Creature::GetCreature(*me, m_GuardsGuid[2]))
                        l_Creature->RemoveAura(SPELL_BOND_OF_FLAME);

                    me->RemoveAura(SPELL_BOND_OF_FLAME);
                }
            }

            void KilledUnit(Unit* p_Killed) override
            {
                if (!p_Killed->IsPlayer())
                    return;

                Talk(SAY_KILL);
            }
            
            void JustRespawned() override
            {
                if (CountSummCombat & 1)
                    if (Creature* l_Guard = me->SummonCreature(NPC_BAALGAR_THE_WATCHFUL, g_BossPos[1]))
                        m_GuardsGuid[1] = l_Guard->GetGUID();

                if (CountSummCombat & 2)
                    if (Creature* l_Guard = me->SummonCreature(NPC_IMACUTYA, g_BossPos[2]))
                        m_GuardsGuid[0] = l_Guard->GetGUID();

                if (CountSummCombat & 4)
                    if (Creature* l_Guard = me->SummonCreature(NPC_JAZSHARIU, g_BossPos[0]))
                        m_GuardsGuid[2] = l_Guard->GetGUID();
            }

            void EnterEvadeMode() override
            {
                m_DiedCount = 0;
                m_GuardsDiedCount = 0;
                for (int8 l_I = 0; l_I < 3; l_I++)
                {
                    if (Creature* l_Guard = me->GetCreature(*me, m_GuardsGuid[l_I]))
                    {
                        if (l_Guard && l_Guard->isAlive())
                            l_Guard->DespawnOrUnsummon();
                    }
                }
                BossAI::EnterEvadeMode();
                me->NearTeleportTo(me->GetHomePosition(), false);
                me->Respawn(true, true, 20 * TimeConstants::IN_MILLISECONDS);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo != nullptr)
                {
                    if (p_SpellInfo->Id == SPELL_INFERNAL_ERUPTION)
                    {
                        m_HitTargets.clear();

                        DoCast(SPELL_INFERNAL_ERUPTION_TRIGGER, false);

                        std::list<Player*> l_ListPlayers;
                        me->GetPlayerListInGrid(l_ListPlayers, 100.0f);

                        for (std::list<Player*>::iterator l_Iter = l_ListPlayers.begin(); l_Iter != l_ListPlayers.end(); ++l_Iter)
                        {
                            Player* l_Itr = (*l_Iter);

                            Position l_Pos = *l_Itr;

                            me->SendPlaySpellVisual(55258, l_Itr, 2.0f, l_Pos, false, false);

                            uint64 l_Guid = l_Itr->GetGUID();

                            AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this, l_Guid, l_Pos]() -> void
                            {
                                if (!l_Guid)
                                    return;

                                /*
                                if (std::find(m_HitTargets.begin(), m_HitTargets.end(), l_Guid) != m_HitTargets.end())
                                    return;

                                m_HitTargets.push_back(l_Guid);
                                */

                                if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                                {
                                    me->CastSpell(l_Pos, SPELL_INFERNAL_ERUPTION_DMG, true);
                                }
                            });
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_BURNING_INTENSITY:
                    {
                        DoCast(SPELL_BURNING_INTENSITY);
                        Talk(SAY_BURNING);
                        events.ScheduleEvent(EVENT_BURNING_INTENSITY, 20000);
                        break;
                    }
                    case EVENT_WITHERING_SOUL:
                    {
                        DoCast(SPELL_WITHERING_SOUL);
                        events.ScheduleEvent(EVENT_WITHERING_SOUL, 15000);
                        break;
                    }
                    case EVENT_INFERNAL_ERUPTION:
                    {
                        DoCast(SPELL_INFERNAL_ERUPTION);
                        events.ScheduleEvent(EVENT_INFERNAL_ERUPTION, 32000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_talixae_flamewreathAI(p_Creature);
        }
};

/// Jazshariu - 104273
/// Baalgar the Watchful - 104274
/// Imacu'tya - 104275
class npc_talixae_guards : public CreatureScript
{
    public:
        npc_talixae_guards() : CreatureScript("npc_talixae_guards") { }

        struct npc_talixae_guardsAI : public ScriptedAI
        {
            npc_talixae_guardsAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool I_InstantKill = false;

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void Reset() override
            {
                events.Reset();
                I_InstantKill = false;

                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                {
                    switch (me->GetEntry())
                    {
                        case NPC_JAZSHARIU:
                        {
                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_BAALGAR_THE_WATCHFUL, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_STRENGTH, l_Creature);

                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_IMACUTYA, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_STRENGTH, l_Creature);
                            
                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_TALIXAE, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_STRENGTH, l_Creature);

                            DoCast(me, SPELL_BOND_OF_STRENGTH_AT, true);
                            me->AddAura(SPELL_BOND_OF_STRENGTH, me);
                            break;
                        }
                        case NPC_BAALGAR_THE_WATCHFUL:
                        {
                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_JAZSHARIU, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_CRUELTY, l_Creature);

                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_IMACUTYA, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_CRUELTY, l_Creature);
                            
                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_TALIXAE, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_CRUELTY, l_Creature);

                            DoCast(me, SPELL_BOND_OF_CRUELTY_AT, true);
                            me->AddAura(SPELL_BOND_OF_CRUELTY, me);
                            break;
                        }
                        case NPC_IMACUTYA:
                        {
                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_BAALGAR_THE_WATCHFUL, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_CUNNING, l_Creature);

                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_JAZSHARIU, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_CUNNING, l_Creature);
                           
                            if (Creature* l_Creature = GetClosestCreatureWithEntry(me, NPC_TALIXAE, 150.0f))
                                if (l_Creature->isAlive())
                                    me->AddAura(SPELL_BOND_OF_CUNNING, l_Creature);

                            DoCast(me, SPELL_BOND_OF_CUNNING_AT, true);
                            me->AddAura(SPELL_BOND_OF_CUNNING, me);
                            break;
                        }
                        default:
                            break;
                    }
                });
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == 0)
                    I_InstantKill = true;
            }

            void MovementInform(uint32 p_Type, uint32 p_PointId) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                if (p_PointId == 1 && I_InstantKill) ///< Special for Discarded Junk
                    me->Kill(me);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                switch (me->GetEntry())
                {
                    case NPC_JAZSHARIU:
                    {
                        events.ScheduleEvent(EVENT_3, 7000);
                        break;
                    }
                    case NPC_BAALGAR_THE_WATCHFUL:
                    {
                        events.ScheduleEvent(EVENT_2, 3000);
                        break;
                    }
                    case NPC_IMACUTYA:
                    {
                        events.ScheduleEvent(EVENT_1, 3000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                Reset();
                _EnterEvadeMode();
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo != nullptr)
                {
                    switch (p_SpellInfo->Id)
                    {
                        case SPELL_SHOCKWAVE:
                        {
                            AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                            {
                                SetCombatMovement(true);

                                me->SetReactState(REACT_AGGRESSIVE);
                                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                            });
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    return;
                }
                else
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(SPELL_WHIRLING_BLADES);
                        events.ScheduleEvent(EVENT_1, 17000);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, SPELL_DISINTEGRATION_BEAM, false);

                        events.ScheduleEvent(EVENT_2, 13000);
                        break;
                    }
                    case EVENT_3:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 15.0f, true))
                        {
                            me->SetFacingToObject(l_Target);

                            me->SetReactState(REACT_PASSIVE);
                            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);

                            SetCombatMovement(false);

                            me->CastSpell(me, SPELL_SHOCKWAVE);
                        }

                        events.ScheduleEvent(EVENT_3, 11000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_talixae_guardsAI(p_Creature);
        }
};

/// Lyleth Lunastre - 106468
class npc_talixae_lyleth : public CreatureScript
{
    public:
        npc_talixae_lyleth() : CreatureScript("npc_talixae_lyleth") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (p_Player->HasAura(213213) || p_Player->HasAura(213304) || p_Player->HasAura(213233))
                return false;

            p_Creature->CastSpell(p_Player, 213213, true); ///< Disguise

            return true;
        }
};

/// Infernal Eruption - 207887
class spell_talixae_infernal_eruption_sum_imp : public SpellScriptLoader
{
    public:
        spell_talixae_infernal_eruption_sum_imp() : SpellScriptLoader("spell_talixae_infernal_eruption_sum_imp") { }

        class spell_talixae_infernal_eruption_sum_imp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_talixae_infernal_eruption_sum_imp_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    WorldLocation* l_Loc = GetHitDest();
                    if (l_Loc == nullptr)
                        return;

                    /// should only spawn adds from infernal eruption on non LoS targets
                    if (l_Caster->IsWithinLOS(l_Loc->m_positionX, l_Loc->m_positionY, l_Loc->m_positionZ))
                        return;

                    l_Caster->SummonCreature(NPC_INFERNAL_IMP, l_Loc->GetPositionX(), l_Loc->GetPositionY(), l_Loc->GetPositionZ());
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_talixae_infernal_eruption_sum_imp_SpellScript::HandleScriptEffect, EFFECT_3, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_talixae_infernal_eruption_sum_imp_SpellScript();
        }
};

class npc_cos_infernal_imp : public VehicleScript
{
    public:
        npc_cos_infernal_imp() : VehicleScript("npc_cos_infernal_imp") { }

        struct npc_cos_infernal_impAI : public ScriptedAI
        {
            npc_cos_infernal_impAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
            }

            enum eData
            {
                EventDriftingEmbers    = 1,
                EventFirebolt,

                SpellTheMawMustFeed    = 215377,
                SpellDriftingEmbersDmg = 224377,
                SpellDriftingEmbers    = 224375,
                SpellDriftingEmbersDmy = 224376,
                SpellFirebolt          = 224374
            };

            void Reset() override
            {
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddAura(eData::SpellTheMawMustFeed, me);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.Reset();

                events.ScheduleEvent(eData::EventDriftingEmbers, 3 * IN_MILLISECONDS);
                events.ScheduleEvent(eData::EventFirebolt, 8 * IN_MILLISECONDS);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eData::SpellDriftingEmbersDmy)
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                        me->CastSpell(l_Target, eData::SpellDriftingEmbersDmg, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eData::EventDriftingEmbers:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(me, eData::SpellDriftingEmbers, false);
                        events.ScheduleEvent(eData::EventDriftingEmbers, urand(9, 13) * IN_MILLISECONDS);
                        break;
                    }
                    case eData::EventFirebolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eData::SpellFirebolt, false);

                        if (me->HasUnitState(UNIT_STATE_ROOT))
                            me->ClearUnitState(UNIT_STATE_ROOT);

                        events.ScheduleEvent(eData::EventFirebolt, urand(9, 13) * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_infernal_impAI(p_Creature);
        }
};

class npc_cos_wounded_nightborne_citizen : public CreatureScript
{
    public:
        npc_cos_wounded_nightborne_citizen() : CreatureScript("npc_cos_wounded_nightborne_citizen") { }

        bool IsHealer(SpecIndex p_Spec)
        {
            switch (p_Spec)
            {
                case SPEC_DRUID_RESTORATION:
                case SPEC_PRIEST_HOLY:
                case SPEC_PRIEST_DISCIPLINE:
                case SPEC_PALADIN_HOLY:
                case SPEC_SHAMAN_RESTORATION:
                case SPEC_MONK_MISTWEAVER:
                    return true;
                    break;
                default:
                    break;
            }
            return false;
        }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_wounded_nightborne_citizen::npc_cos_wounded_nightborne_citizenAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_wounded_nightborne_citizen::npc_cos_wounded_nightborne_citizenAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (IsHealer((SpecIndex)p_Player->GetActiveSpecializationID()) || p_Player->HasSkill(SKILL_TAILORING) || p_Player->HasSkill(SKILL_FIRST_AID));
            if (m_Ok)
            {
                l_AI->m_Fixed = true;
                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                l_AI->DoAction(0);
            }

            return m_Ok;
        }

        struct npc_cos_wounded_nightborne_citizenAI : public ScriptedAI
        {
            npc_cos_wounded_nightborne_citizenAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                Reset();
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                events.Reset();
                m_Fixed = false;
                me->DisableHealthRegen();
                me->SetHealth(me->CountPctFromMaxHealth(20));
                me->setFaction(FACTION_FRIENDLY);
                me->CastSpell(me, 204237, true); ///< Dead state.
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetDisplayId(69927);
            }

            void DoAction(int32 const p_Action) override
            {
                events.ScheduleEvent(1, 1 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);
                switch (events.ExecuteEvent())
                {
                    case 1:
                    {
                        Talk(0);
                        events.ScheduleEvent(2, 3 * IN_MILLISECONDS);
                        break;
                    }
                    case 2:
                    {
                        Talk(1);
                        events.ScheduleEvent(3, 3 * IN_MILLISECONDS);
                        me->SetHealth(me->GetMaxHealth());
                        break;
                    }
                    case 3:
                    {
                        Talk(2);
                        BringAndDistractNearestGuardian(me);
                        me->RemoveAura(204237);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_wounded_nightborne_citizenAI(p_Creature);
        }
};

class npc_cos_fel_orb : public CreatureScript
{
    public:
        npc_cos_fel_orb() : CreatureScript("npc_cos_fel_orb") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_fel_orb::npc_cos_fel_orbAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_fel_orb::npc_cos_fel_orbAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->getClass() == CLASS_DEMON_HUNTER || p_Player->getClass() == CLASS_WARLOCK || p_Player->getClass() == CLASS_PALADIN || p_Player->getClass() == CLASS_PRIEST);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;

                if (InstanceScript *l_InstanceScript = p_Player->GetInstanceScript())
                    l_InstanceScript->DoAddAuraOnPlayers(211081); ///< Succulent Cuisine

                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return m_Ok;
        }

        struct npc_cos_fel_orbAI : public ScriptedAI
        {
            npc_cos_fel_orbAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Fixed = false;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                me->RemoveAllAuras();
                me->SetDisplayId(11686);
                me->CastSpell(me, 208275);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_fel_orbAI(p_Creature);
        }
};

class npc_cos_nightmare_refreshmenet : public CreatureScript
{
    public:
        npc_cos_nightmare_refreshmenet() : CreatureScript("npc_cos_nightmare_refreshmenet") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_nightmare_refreshmenet::npc_cos_nightmare_refreshmenetAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_nightmare_refreshmenet::npc_cos_nightmare_refreshmenetAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->getRace() == RACE_PANDAREN_ALLIANCE || p_Player->getRace() == RACE_PANDAREN_HORDE || p_Player->GetPureSkillValue(SKILL_COOKING) >= 800);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;

                if (InstanceScript *l_InstanceScript = p_Player->GetInstanceScript())
                    l_InstanceScript->DoAddAuraOnPlayers(211102); ///< Succulent Cuisine

                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return m_Ok;
        }

        struct npc_cos_nightmare_refreshmenetAI : public ScriptedAI
        {
            npc_cos_nightmare_refreshmenetAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Fixed = false;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(11686);
                me->CastSpell(me, 208561);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_nightmare_refreshmenetAI(p_Creature);
        }
};

class npc_cos_magical_lantern : public CreatureScript
{
    public:
        npc_cos_magical_lantern() : CreatureScript("npc_cos_magical_lantern") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_magical_lantern::npc_cos_magical_lanternAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_magical_lantern::npc_cos_magical_lanternAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->getClass() == CLASS_MAGE || p_Player->getRace() == RACE_NIGHTELF || p_Player->getRace() == RACE_BLOODELF ||
                p_Player->GetPureSkillValue(SKILL_ENCHANTING) >= 80);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;

                if (InstanceScript *l_InstanceScript = p_Player->GetInstanceScript())
                    l_InstanceScript->DoAddAuraOnPlayers(211093); ///< Arcane Infusion

                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return m_Ok;
        }

        struct npc_cos_magical_lanternAI : public ScriptedAI
        {
            npc_cos_magical_lanternAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Fixed = false;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(11686);
                me->CastSpell(me, 211095, true);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_magical_lanternAI(p_Creature);
        }
};

class npc_cos_infernal_tome : public CreatureScript
{
    public:
        npc_cos_infernal_tome() : CreatureScript("npc_cos_infernal_tome") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_infernal_tome::npc_cos_infernal_tomeAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_infernal_tome::npc_cos_infernal_tomeAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->getClass() == CLASS_PALADIN || p_Player->getClass() == CLASS_PRIEST || p_Player->getClass() == CLASS_DEMON_HUNTER);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;

                if (InstanceScript *l_InstanceScript = p_Player->GetInstanceScript())
                    l_InstanceScript->DoAddAuraOnPlayers(211080); ///< Arcane Infusion

                p_Creature->AddAura(211082, p_Creature);
                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return m_Ok;
        }

        struct npc_cos_infernal_tomeAI : public ScriptedAI
        {
            npc_cos_infernal_tomeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Fixed = false;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(69311);
                me->CastSpell(me, 209756, true);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_infernal_tomeAI(p_Creature);
        }
};

class npc_cos_starlight_red_rose : public CreatureScript
{
    public:
        npc_cos_starlight_red_rose() : CreatureScript("npc_cos_starlight_red_rose") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_starlight_red_rose::npc_cos_starlight_red_roseAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_starlight_red_rose::npc_cos_starlight_red_roseAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->getClass() == CLASS_DEATH_KNIGHT || p_Player->getClass() == CLASS_MONK);
            if (m_Ok)
            {
               l_AI->m_Fixed = true;

                if (InstanceScript *l_InstanceScript = p_Player->GetInstanceScript())
                    l_InstanceScript->DoAddAuraOnPlayers(211071); ///< Star light Rose Brew

                p_Creature->RemoveAura(211063);
                p_Creature->CastSpell(p_Creature, 210265, true);
                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return m_Ok;
        }

        struct npc_cos_starlight_red_roseAI : public ScriptedAI
        {
            npc_cos_starlight_red_roseAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Fixed = false;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(11686);
                me->CastSpell(me, 211063, true);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_starlight_red_roseAI(p_Creature);
        }
};

class npc_cos_umbral_boom : public CreatureScript
{
    public:
        npc_cos_umbral_boom() : CreatureScript("npc_cos_umbral_boom") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_umbral_boom::npc_cos_umbral_boomAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_umbral_boom::npc_cos_umbral_boomAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->getClass() == CLASS_DRUID || p_Player->GetPureSkillValue(SKILL_HERBALISM) >= 80);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;

                if (InstanceScript *l_InstanceScript = p_Player->GetInstanceScript())
                    l_InstanceScript->DoAddAuraOnPlayers(211110); ///< Umbral Spores

                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return m_Ok;
        }

        struct npc_cos_umbral_boomAI : public ScriptedAI
        {
            npc_cos_umbral_boomAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Fixed = false;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(11686);
                me->CastSpell(me, 226753, true);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_umbral_boomAI(p_Creature);
        }
};

class npc_cos_waterlogged_scroll : public CreatureScript
{
    public:
        npc_cos_waterlogged_scroll() : CreatureScript("npc_cos_waterlogged_scroll") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_waterlogged_scroll::npc_cos_waterlogged_scrollAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_waterlogged_scroll::npc_cos_waterlogged_scrollAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->getClass() == CLASS_SHAMAN || p_Player->HasSkill(SKILL_SKINNING) || p_Player->GetPureSkillValue(SKILL_INSCRIPTION) >= 80);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;

                if (InstanceScript *l_InstanceScript = p_Player->GetInstanceScript())
                    l_InstanceScript->DoAddAuraOnPlayers(211084); ///< Umbral Spores

                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return m_Ok;
        }

        struct npc_cos_waterlogged_scrollAI : public ScriptedAI
        {
            npc_cos_waterlogged_scrollAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Fixed = false;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(11686);
                me->CastSpell(me, 211087);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_waterlogged_scrollAI(p_Creature);
        }
};

class npc_cos_arcane_condult : public CreatureScript
{
    public:
        npc_cos_arcane_condult() : CreatureScript("npc_cos_arcane_condult") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_arcane_condult::npc_cos_arcane_condultAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_arcane_condult::npc_cos_arcane_condultAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->getRace() == RACE_GNOME || p_Player->getRace() == RACE_GOBLIN || p_Player->GetPureSkillValue(SKILL_ENGINEERING) >= 80);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;

                std::list<Creature*> l_ListCreatures;
                p_Creature->GetCreatureListWithEntryInGrid(l_ListCreatures, eCreatures::NPC_GUARDIAN_CONSTRUCT, 350.0f);

                for (Creature* l_Itr : l_ListCreatures)
                {
                    l_Itr->setFaction(2735);
                    l_Itr->CastSpell(l_Itr, 200046); ///< Perma stun
                    l_Itr->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }
 
                p_Creature->RemoveAura(210466);
                p_Creature->AddAura(208274, p_Creature);
                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            return m_Ok;
        }

        struct npc_cos_arcane_condultAI : public ScriptedAI
        {
            npc_cos_arcane_condultAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Fixed = false;
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(11686);
                me->CastSpell(me, 210466);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_arcane_condultAI(p_Creature);
        }
};

class npc_cos_bazaar_goods : public CreatureScript
{
    public:
        npc_cos_bazaar_goods() : CreatureScript("npc_cos_bazaar_goods") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_bazaar_goods::npc_cos_bazaar_goodsAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_bazaar_goods::npc_cos_bazaar_goodsAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->GetPureSkillValue(SKILL_LEATHERWORKING) >= 80 || p_Player->getClass() == CLASS_ROGUE || p_Player->getClass() == CLASS_WARRIOR);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;
                p_Creature->RemoveAura(211129);
                p_Creature->AddAura(210106, p_Creature);
                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                BringAndDistractNearestGuardian(p_Creature);
            }

            return m_Ok;
        }

        struct npc_cos_bazaar_goodsAI : public ScriptedAI
        {
            npc_cos_bazaar_goodsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
               me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
               m_Fixed = false;
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(11686);
                me->CastSpell(me, 211129);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_bazaar_goodsAI(p_Creature);
        }
};

class npc_cos_lifesized_nightborne_statue : public CreatureScript
{
    public:
        npc_cos_lifesized_nightborne_statue() : CreatureScript("npc_cos_lifesized_nightborne_statue") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_lifesized_nightborne_statue::npc_cos_lifesized_nightborne_statueAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_lifesized_nightborne_statue::npc_cos_lifesized_nightborne_statueAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->HasSkill(SKILL_MINING) || p_Player->GetPureSkillValue(SKILL_JEWELCRAFTING) >= 80);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;

                p_Creature->RemoveAura(210319);
                p_Creature->AddAura(211130, p_Creature);
                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                BringAndDistractNearestGuardian(p_Creature);
            }

            return m_Ok;
        }

        struct npc_cos_lifesized_nightborne_statueAI : public ScriptedAI
        {
            npc_cos_lifesized_nightborne_statueAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
                m_Fixed = false;
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetDisplayId(11686);
                me->CastSpell(me, 210319);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_lifesized_nightborne_statueAI(p_Creature);
        }
};

/// Infernal Eruption - 207887
class spell_cos_infernal_eruption : public SpellScriptLoader
{
    public:
        spell_cos_infernal_eruption() : SpellScriptLoader("spell_cos_infernal_eruption") { }

        class spell_cos_infernal_eruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cos_infernal_eruption_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                bool l_Ok = false;

                if (GetSpellInfo() != nullptr)
                {
                    if (Unit* l_Caster = GetCaster())
                    {
                        if (Unit* l_HitUnit = GetHitUnit())
                        {
                            if (UnitAI* l_AI = l_Caster->GetAI())
                            {
                                if (boss_talixae_flamewreath::boss_talixae_flamewreathAI* l_LinkAI = CAST_AI(boss_talixae_flamewreath::boss_talixae_flamewreathAI, l_AI))
                                {
                                    if (std::find(l_LinkAI->m_HitTargets.begin(), l_LinkAI->m_HitTargets.end(), l_HitUnit->GetGUID()) == l_LinkAI->m_HitTargets.end())
                                    {
                                        l_Ok = true;
                                        l_LinkAI->m_HitTargets.push_back(l_HitUnit->GetGUID());
                                    }
                                }
                            }
                        }
                    }

                if(!l_Ok)
                   SetHitDamage(0);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_cos_infernal_eruption_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cos_infernal_eruption_SpellScript();
        }
};

///< Felbound Enforcer 104278
class npc_cos_felbound_enforcer : public CreatureScript
{
    public:
        npc_cos_felbound_enforcer() : CreatureScript("npc_cos_felbound_enforcer") { }

        struct npc_cos_felbound_enforcerAI : public ScriptedAI
        {
            npc_cos_felbound_enforcerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eData
            {
                EventFelDetonation  = 1,
                SPELL_FELDETONATION = 211464
            };

            bool CanFly() override
            {
                return false;
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                events.ScheduleEvent(eData::EventFelDetonation, urand(5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
            }

            void EnterEvadeMode() override
            {
                Reset();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->LoadCreaturesAddon();
                me->ClearLootContainers();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eData::EventFelDetonation:
                    {
                        DoCastAOE(SPELL_FELDETONATION);
                        events.ScheduleEvent(eData::EventFelDetonation, urand(7 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_felbound_enforcerAI(p_Creature);
        }
};

/// Called by Infernal Eruption - 207887
class spell_at_talixae_infernal_eruption : public AreaTriggerEntityScript
{
    public:
        spell_at_talixae_infernal_eruption() : AreaTriggerEntityScript("spell_at_talixae_infernal_eruption") {}

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            float l_Radius = p_AreaTrigger->GetRadius();
            std::list<Player*> l_PlayerList;
            JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
            JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

            l_PlayerList.remove_if([l_Caster](Player* p_Player) -> bool
            {
                if (!p_Player)
                    return true;

                if (!l_Caster->IsValidAttackTarget(p_Player))
                    return true;

                return false;
            });

            if (!l_PlayerList.empty())
            {
                for (Player* l_Player : l_PlayerList)
                {
                    if (!l_Player)
                        continue;

                    l_Caster->CastSpell(l_Player, 211457, true);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_talixae_infernal_eruption();
        }
};

class npc_cos_discarded_junk : public CreatureScript
{
    public:
        npc_cos_discarded_junk() : CreatureScript("npc_cos_discarded_junk") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            npc_cos_discarded_junk::npc_cos_discarded_junkAI* l_AI = nullptr;

            if (p_Creature->IsAIEnabled)
                l_AI = CAST_AI(npc_cos_discarded_junk::npc_cos_discarded_junkAI, p_Creature->GetAI());

            if (l_AI == nullptr || l_AI->m_Fixed)
                return false;

            bool m_Ok = (p_Player->GetPureSkillValue(SKILL_BLACKSMITHING) >= 80 || p_Player->getClass() == CLASS_HUNTER);
            if (m_Ok)
            {
                l_AI->m_Fixed = true;
                p_Creature->RemoveAllAuras();
                p_Creature->AddAura(208378, p_Creature);
                p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                BringAndDistractNearestGuardian(p_Creature, true);
            }

            return m_Ok;
        }

        struct npc_cos_discarded_junkAI : public ScriptedAI
        {
            npc_cos_discarded_junkAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_SELECTION_DISABLED);
                m_Fixed = false;
            }

            bool m_Fixed;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_discarded_junkAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_talixae_flamewreath()
{
    /// Boss
    new boss_talixae_flamewreath();

    /// Creature
    new npc_talixae_guards();
    new npc_cos_infernal_imp();
    new npc_talixae_lyleth();
    new npc_cos_felbound_enforcer();

    /// Props
    new npc_cos_fel_orb();
    new npc_cos_nightmare_refreshmenet();
    new npc_cos_wounded_nightborne_citizen();
    new npc_cos_infernal_tome();
    new npc_cos_magical_lantern();
    new npc_cos_starlight_red_rose();
    new npc_cos_umbral_boom();
    new npc_cos_waterlogged_scroll();
    new npc_cos_bazaar_goods();
    new npc_cos_arcane_condult();
    new npc_cos_lifesized_nightborne_statue();
    new npc_cos_discarded_junk();

    /// Spell
    new spell_cos_infernal_eruption();
    new spell_talixae_infernal_eruption_sum_imp();
    
    ///< AT
    new spell_at_talixae_infernal_eruption();
}
#endif
