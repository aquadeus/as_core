////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

/// Maiden of Virtue - 113971
class boss_maiden_of_virtue_legion : public CreatureScript
{
    public:
        boss_maiden_of_virtue_legion() : CreatureScript("boss_maiden_of_virtue_legion") { }

        struct boss_maiden_of_virtue_legionAI : public BossAI
        {
            boss_maiden_of_virtue_legionAI(Creature* p_Creature) : BossAI(p_Creature, eData::MaidenVirtue) { }

            enum eTalks
            {
                Aggro               = 0,
                TalkHolyBolt        = 1,
                TalkSacredGround    = 2,
                TalkHolyShock       = 3,
                TalkMassRepentance  = 4,
                TalkRepentanceWarn  = 5,
                TalkHolyBulwark     = 6,
                TalkHolyWrath       = 7,
                Death               = 8
            };

            enum eSpells
            {
                HolyBolt            = 227809,
                SacredGround        = 227789,
                HolyShock           = 227800,
                MassRepentance      = 227508,
                HolyBulwark         = 227817,
                HolyWrath           = 227823
            };

            enum eEvents
            {
                EventHolyBolt       = 1,
                EventSacredGround   = 2,
                EventHolyShock      = 3,
                EventMassRepentance = 4,
                EventHolyBulwark    = 5,
                EventHolyWrath      = 6
            };

            void GetMythicHealthMod(float& p_Mod) override
            {
                if (IsChallengeMode())
                    p_Mod = 1.7f;
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(eData::MaidenVirtue, EncounterState::FAIL);
            }

            void Reset() override
            {
                _Reset();
                if (IsChallengeMode())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::Aggro);
                _EnterCombat();

                DefaultEvents();
            }

            void DefaultEvents()
            {
                events.ScheduleEvent(eEvents::EventHolyBolt, 9000);
                events.ScheduleEvent(eEvents::EventSacredGround, 11000);
                events.ScheduleEvent(eEvents::EventHolyShock, 16000);
                events.ScheduleEvent(eEvents::EventMassRepentance, 50000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);
                _JustDied();
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::MassRepentance:
                        events.ScheduleEvent(eEvents::EventHolyBulwark, 100);
                        break;
                    case eSpells::HolyBulwark:
                        events.ScheduleEvent(eEvents::EventHolyWrath, 1000);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckHomeDistToEvade(p_Diff, 38.0f))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventHolyBolt:
                    {
                        if (urand(0, 1))
                            Talk(eTalks::TalkHolyBolt);
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(l_Target, eSpells::HolyBolt, false);
                        events.ScheduleEvent(eEvents::EventHolyBolt, 14000);
                        break;
                    }
                    case eEvents::EventSacredGround:
                    {
                        if (urand(0, 1))
                            Talk(eTalks::TalkSacredGround);
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(l_Target, eSpells::SacredGround, false);
                        events.ScheduleEvent(eEvents::EventSacredGround, 23000);
                        break;
                    }
                    case eEvents::EventHolyShock:
                    {
                        if (urand(0, 1))
                            Talk(eTalks::TalkHolyShock);
                        DoCastVictim(eSpells::HolyShock);
                        events.ScheduleEvent(eEvents::EventHolyShock, 13000);
                        break;
                    }
                    case eEvents::EventMassRepentance:
                    {
                        Talk(eTalks::TalkMassRepentance);
                        Talk(eTalks::TalkRepentanceWarn);
                        events.DelayEvents(6000);
                        DoStopAttack();
                        me->SetReactState(REACT_PASSIVE);
                        DoCast(eSpells::MassRepentance);
                        events.ScheduleEvent(eEvents::EventMassRepentance, 50000);
                        break;
                    }
                    case eEvents::EventHolyBulwark:
                    {
                        Talk(eTalks::TalkHolyBulwark);
                        DoCast(eSpells::HolyBulwark);
                        break;
                    }
                    case eEvents::EventHolyWrath:
                    {
                        Talk(eTalks::TalkHolyWrath);
                        DoCast(eSpells::HolyWrath);
                        me->SetReactState(REACT_AGGRESSIVE);
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
            return new boss_maiden_of_virtue_legionAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Sacred Ground - 8812
class at_rtk_sacred_ground : public AreaTriggerEntityScript
{
    public:
        at_rtk_sacred_ground() : AreaTriggerEntityScript("at_rtk_sacred_ground") { }

        enum eSpells
        {
            SacredGroundDot = 227848
        };

        void OnUpdate(AreaTrigger* p_Areatrigger, uint32 p_Diff) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SacredGroundDot);
            if (!l_Caster || !l_SpellInfo)
                return;

            GuidList* l_AffectedGUIDs = p_Areatrigger->GetAffectedPlayers();
            for (uint64 l_GUID : *l_AffectedGUIDs)
            {
                Player* l_Player = sObjectAccessor->GetPlayer(*l_Caster, l_GUID);
                if (!l_Player)
                    continue;

                Aura* l_Aura = l_Player->GetAura(eSpells::SacredGroundDot);
                if (l_Aura && l_Aura->GetDuration() > (l_SpellInfo->GetMaxDuration() - l_SpellInfo->Effects[EFFECT_0].Amplitude))
                    continue;

                l_Caster->CastSpell(l_Player, eSpells::SacredGroundDot, true);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_rtk_sacred_ground();
        }
};

/// Last Update 7.1.5 Build 23420
/// Medivh - 115487
class npc_maiden_medivh : public CreatureScript
{
    public:
        npc_maiden_medivh() : CreatureScript("npc_maiden_medivh") { }

        struct npc_maiden_medivhAI : public ScriptedAI
        {
            npc_maiden_medivhAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                if (me->m_positionZ < 400.0f)
                    me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_EMOTE_SLEEP);
                else
                    me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_DEAD);
            }

            enum eSpells
            {
                Vision = 229485
            };

            void Reset() override
            {
                me->AddAura(eSpells::Vision, me);
            }

            void UpdateAI(uint32 const p_Diff) override
            {

            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_maiden_medivhAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Prince Llane Wrynn - 115490
class npc_maiden_llane_wrynn : public CreatureScript
{
    public:
        npc_maiden_llane_wrynn() : CreatureScript("npc_maiden_llane_wrynn") { }

        struct npc_maiden_llane_wrynnAI : public ScriptedAI
        {
            npc_maiden_llane_wrynnAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetWalk(true);
                me->SetVisible(false);
                me->SetSheath(SheathState::SHEATH_STATE_UNARMED);
            }

            std::vector<G3D::Vector3> m_WalkingPath = 
            {
                { -10907.18f, -2057.18f, 92.18f },
                { -10905.78f, -2063.03f, 92.18f },
                { -10862.42f, -2095.05f, 90.50f },
                { -10858.39f, -2099.28f, 91.75f },
                { -10856.87f, -2101.39f, 92.17f },
                { -10855.21f, -2103.73f, 92.17f },
                { -10853.93f, -2105.45f, 92.17f },
                { -10852.49f, -2105.92f, 92.17f }
            };

            enum eSpells
            {
                Vision = 229485
            };

            enum eTalks
            {
                ConvSceneTalk1,
                ConvSceneTalk2
            };

            enum eAnimKit
            {
                LlaneWrynn = 12154
            };

            void Reset() override
            {
                me->AddAura(eSpells::Vision, me);
                AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                {
                    me->SetVisible(true);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePath(10, m_WalkingPath.data(), m_WalkingPath.size(), true);
                });

                AddTimedDelayedOperation(19500, [this]() -> void
                {
                    Talk(eTalks::ConvSceneTalk1);
                });

                AddTimedDelayedOperation(37500, [this]() -> void
                {
                    Talk(eTalks::ConvSceneTalk2);
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_ID == 10)
                {
                    Creature* l_Medivh = me->FindNearestCreature(eCreatures::MedivhMaiden, 10.0f);
                    if (!l_Medivh)
                        return;

                    me->SetFacingToObject(l_Medivh);

                    AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                    {
                        me->SetAIAnimKitId(eAnimKit::LlaneWrynn);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_maiden_llane_wrynnAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_maiden_of_virtue_legion()
{
    /// Boss
    new boss_maiden_of_virtue_legion();

    /// Areatrigger
    new at_rtk_sacred_ground();

    /// Scene
    new npc_maiden_medivh();
    new npc_maiden_llane_wrynn();
}
#endif
