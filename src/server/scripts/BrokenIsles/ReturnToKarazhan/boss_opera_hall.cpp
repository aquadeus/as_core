////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "return_to_karazhan.hpp"

struct boss_opera_encounters : public BossAI
{
    boss_opera_encounters(Creature* p_Creature) : BossAI(p_Creature, eData::OperaHall) { }

    enum eFactions
    {
        Aggresive = 16
    };

    void Reset() override
    {
        _Reset();
    }

    void EnterCombat(Unit* /*p_Who*/) override
    {
        _EnterCombat();
    }

    void EnterEvadeMode() override
    {
        BossAI::EnterEvadeMode();
    }

    void UpdateAI(uint32 const p_Diff) override
    {
        if (me->GetPositionX() > -10873.0f)
            EnterEvadeMode();

        if (me->GetPositionX() < -10915.0f && me->GetPositionY() < -1770.0f)
            EnterEvadeMode();
    }

    void JustDied(Unit* /*p_Killer*/) override
    {
        if (instance && instance->GetData(eData::OperaHallScene) == eData::OperaHallBeautifulBeast)
        {
            if (Creature* l_Coggleston = instance->instance->GetCreature(instance->GetData64(eCreatures::Coggleston)))
            {
                if (l_Coggleston->IsAIEnabled)
                    l_Coggleston->AI()->DoAction(ACTION_3);
            }

            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }
        else
            _JustDied();
    }

    void DoAction(int32 const p_Action) override
    {
        switch (p_Action)
        {
            case ACTION_1:
            {
                summons.DespawnAll();
                me->DespawnOrUnsummon(100);
                break;
            }
            /// Westfall event
            case ACTION_2:
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->setFaction(eFactions::Aggresive);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat();
                break;
            }
            default:
                break;
        }
    }
};

/// Last Update 7.1.5 Build 234520
/// Galindre - 114251
class boss_rtk_galindre : public CreatureScript
{
    public:
        boss_rtk_galindre() : CreatureScript("boss_rtk_galindre") { }

        struct boss_rtk_galindreAI : public boss_opera_encounters
        {
            boss_rtk_galindreAI(Creature* p_Creature) : boss_opera_encounters(p_Creature)
            {
                me->SetReactState(REACT_DEFENSIVE);
            }

            enum eTalks
            {
                Aggro               = 0,
                TalkMagnificent     = 1,
                TalkMagnificentWarn = 2,
                PlayerKilled        = 3
            };

            enum eSpells
            {
                WikketBond                  = 231869,
                SharedHealth                = 229010,
                NaughtyIntroConversation    = 227864,
                MagicMagnificent            = 232235, ///< Intro visual
                FlashyBolt                  = 227341,
                WindrousRadiance            = 227410,
                MagicMagnificentImmune      = 227346,
                MagicMagnificentDamage      = 227776,
                MagicMagnificentUnknown     = 227347
            };

            enum eEvents
            {
                Event1                  = 1,
                Event2                  = 2,
                Event3                  = 3,
                Event4                  = 4,
                EventFlashyBolt         = 5,
                EventWondrousRadiance   = 6,
                EventMagicMagnificent   = 7,
                AggroTalk               = 8
            };

            bool m_Intro = false;

            void AttackStart(Unit* p_Target, bool /*p_Melee*/) override
            {
                if (!p_Target)
                    return;

                if (me->Attack(p_Target, false))
                    DoStartNoMovement(p_Target);
            }

            void Reset() override
            {
                boss_opera_encounters::Reset();
                events.Reset();
                DoCast(me, eSpells::WikketBond, true);
                DoCast(me, eSpells::SharedHealth, true);

                if (instance && instance->GetData(eData::OperaHallIntro) != IN_PROGRESS)
                {
                    SetFlyMode(true);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    DoCast(me, eSpells::MagicMagnificent, true);

                    events.ScheduleEvent(eEvents::Event1, 500);
                    events.ScheduleEvent(eEvents::Event2, 12000);
                }
            }

            void EnterCombat(Unit* p_Who) override
            {
                boss_opera_encounters::EnterCombat(p_Who);

                events.ScheduleEvent(eEvents::AggroTalk, 3000);
                events.ScheduleEvent(eEvents::EventFlashyBolt, 1000);
                events.ScheduleEvent(eEvents::EventWondrousRadiance, 8000);
                events.ScheduleEvent(eEvents::EventMagicMagnificent, 45000);
            }

            void EnterEvadeMode() override
            {
                boss_opera_encounters::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                boss_opera_encounters::JustDied(nullptr);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE)
                {
                    if (p_ID == 1)
                    {
                        me->SetFacingTo(4.61f);
                        events.ScheduleEvent(eEvents::Event3, 6000);
                    }
                }
            }

            void MagnificentEvent()
            {
                me->NearTeleportTo(-10892.7f, -1762.93f, 90.47f, me->GetOrientation());

                me->m_Functions.AddFunction([this]() -> void
                {
                    if (me && me->isAlive())
                    {
                        Talk(eTalks::TalkMagnificent);
                        Talk(eTalks::TalkMagnificentWarn);
                        DoCast(me, eSpells::MagicMagnificentImmune, true);
                        DoCast(eSpells::MagicMagnificentDamage);
                    }
                }, me->m_Functions.CalculateTime(100));

                /// Remove Immune
                me->m_Functions.AddFunction([this]() -> void
                {
                    if (me && me->isAlive())
                        me->RemoveAurasDueToSpell(eSpells::MagicMagnificentImmune);
                }, me->m_Functions.CalculateTime(5000));
            }

            void KilledUnit(Unit* p_KilledUnit) override
            {
                if (p_KilledUnit->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_opera_encounters::UpdateAI(p_Diff);

                if (!UpdateVictim() && m_Intro)
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckHomeDistToEvade(p_Diff, 25.0f, -10893.10f, -1763.78f, 90.47f))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::Event1:
                    {
                        DoCast(me, eSpells::NaughtyIntroConversation, true);
                        break;
                    }
                    case eEvents::Event2:
                    {
                        me->GetMotionMaster()->MoveTakeoff(1, -10891.55f, -1761.89f, 90.47f);
                        break;
                    }
                    case eEvents::Event3:
                    {
                        SetFlyMode(false);
                        me->RemoveAurasDueToSpell(eSpells::MagicMagnificent);
                        events.ScheduleEvent(eEvents::Event4, 10000);
                        break;
                    }
                    case eEvents::Event4:
                    {
                        m_Intro = true;

                        if (instance)
                            instance->SetData(eData::OperaHallWikket, SPECIAL);

                        break;
                    }
                    case eEvents::AggroTalk:
                    {
                        Talk(eTalks::Aggro);
                        break;
                    }
                    case eEvents::EventFlashyBolt:
                    {
                        DoCast(eSpells::FlashyBolt);
                        events.ScheduleEvent(eEvents::EventFlashyBolt, 4000);
                        break;
                    }
                    case eEvents::EventWondrousRadiance:
                    {
                        DoCastVictim(eSpells::WindrousRadiance);
                        events.ScheduleEvent(eEvents::EventWondrousRadiance, 10000);
                        break;
                    }
                    case eEvents::EventMagicMagnificent:
                    {
                        ///DoCast(me, 227538, true); //Offlike
                        MagnificentEvent();
                        events.ScheduleEvent(eEvents::EventMagicMagnificent, 45000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_rtk_galindreAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 234520
/// Elfyra - 114284
class boss_rtk_elfyra : public CreatureScript
{
    public:
        boss_rtk_elfyra() : CreatureScript("boss_rtk_elfyra") { }

        struct boss_rtk_elfyraAI : public boss_opera_encounters
        {
            boss_rtk_elfyraAI(Creature* p_Creature) : boss_opera_encounters(p_Creature) { }

            enum eTalks
            {
                Aggro           = 0,
                TalkGravity     = 1,
                Assistants      = 2,
                Death           = 3,
                PlayerKilled    = 4
            };

            enum eSpells
            {
                WikketBond      = 231869,
                SharedHealth    = 229010,
                RideVehBroom    = 227821,
                DrearyBolt      = 227543,
                SummonAssistant = 227477,
                ModScale        = 214567,
                DefyGravity     = 227447
            };

            enum eEvents
            {
                EventDrearyBolt         = 1,
                EventSummonAssistants   = 2,
                EventSummonAssistants2  = 3,
                EventDefyGravity        = 4
            };

            void Reset() override
            {
                boss_opera_encounters::Reset();
                events.Reset();
                DoCast(me, eSpells::WikketBond, true);
                DoCast(me, eSpells::SharedHealth, true);

                if (instance && instance->GetData(eData::OperaHallIntro) != IN_PROGRESS)
                {
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);

                    /// Summon Vehicle
                    me->m_Functions.AddFunction([this]() -> void
                    {
                        if (Creature* l_Broom = me->SummonCreature(eCreatures::CroneBroom, me->GetPosition()))
                        {
                            DoCast(me, eSpells::RideVehBroom, true);

                            if (l_Broom->IsAIEnabled)
                                l_Broom->AI()->SetFlyMode(true);

                            l_Broom->GetMotionMaster()->MovePath(11448600, false);
                        }
                    }, me->m_Functions.CalculateTime(100));
                }
            }

            void EnterCombat(Unit* p_Who) override
            {
                boss_opera_encounters::EnterCombat(p_Who);
                Talk(eTalks::Aggro);

                events.ScheduleEvent(eEvents::EventDrearyBolt, 4000);
                events.ScheduleEvent(eEvents::EventSummonAssistants, 30000);
                events.ScheduleEvent(eEvents::EventDefyGravity, 11000);
            }

            void KilledUnit(Unit* p_KilledUnit) override
            {
                if (p_KilledUnit->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void EnterEvadeMode() override
            {
                boss_opera_encounters::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                boss_opera_encounters::JustDied(nullptr);
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                Talk(eTalks::Death);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_opera_encounters::UpdateAI(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckHomeDistToEvade(p_Diff, 25.0f, -10893.10f, -1763.78f, 90.47f))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventDrearyBolt:
                    {
                        DoCastVictim(eSpells::DrearyBolt);
                        events.ScheduleEvent(eEvents::EventDrearyBolt, 4000);
                        break;
                    }
                    case eEvents::EventSummonAssistants:
                    {
                        DoCast(eSpells::SummonAssistant);
                        Talk(eTalks::Assistants);
                        events.ScheduleEvent(eEvents::EventSummonAssistants, 32000);
                        events.ScheduleEvent(eEvents::EventSummonAssistants2, 1000);
                        break;
                    }
                    case eEvents::EventSummonAssistants2:
                    {
                        Position l_Pos;
                        for (uint8 l_I = 0; l_I < 3; l_I++)
                        {
                            me->GetRandomNearPosition(l_Pos, frand(5.0f, 25.0f));

                            if (Creature* l_Assistant = me->SummonCreature(eCreatures::WingedAssistant, l_Pos))
                            {
                                l_Assistant->CastSpell(l_Assistant, eSpells::ModScale, true);

                                if (l_Assistant->IsAIEnabled)
                                    l_Assistant->AI()->DoZoneInCombat(l_Assistant, 60.0f);
                            }
                        }

                        break;
                    }
                    case eEvents::EventDefyGravity:
                    {
                        Talk(eTalks::TalkGravity);
                        DoCast(eSpells::DefyGravity);
                        events.ScheduleEvent(eEvents::EventDefyGravity, 13000);
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
            return new boss_rtk_elfyraAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Defy Gravity - 227447
class spell_rtk_defy_gravity : public SpellScriptLoader
{
    public:
        spell_rtk_defy_gravity() : SpellScriptLoader("spell_rtk_defy_gravity") { }

        class spell_rtk_defy_gravity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_defy_gravity_SpellScript);

            enum eSpells
            {
                DefyGravity = 227444
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 2);
            }

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DefyGravity, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_rtk_defy_gravity_SpellScript::HandleOnHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_defy_gravity_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_defy_gravity_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Flashy Bolt - 227341
class spell_rtk_flashy_bolt : public SpellScriptLoader
{
    public:
        spell_rtk_flashy_bolt() : SpellScriptLoader("spell_rtk_flashy_bolt") { }

        class spell_rtk_flashy_bolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_flashy_bolt_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rtk_flashy_bolt_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_flashy_bolt_SpellScript();
        }
};

/// Toe Knee <Defias Brotherhood> - 114261
class boss_rtk_toe_knee : public CreatureScript
{
    public:
        boss_rtk_toe_knee() : CreatureScript("boss_rtk_toe_knee") { }

        struct boss_rtk_toe_kneeAI : public boss_opera_encounters
        {
            boss_rtk_toe_kneeAI(Creature* p_Creature) : boss_opera_encounters(p_Creature)
            {
                me->SetReactState(REACT_DEFENSIVE);
            }

            bool m_IsInConversation;

            enum eTalks
            {
                ToeAggro            = 0,
                ToeTimeOut          = 1,
                ToeReturnFight      = 2,
                ToeFlameGale        = 3,
                ToeDeath            = 4,
                ToeBurningLegSweep  = 5,
                PlayerKilled        = 6,
                Wipe                = 7,
                ConvTalk1           = 8,
                ConvTalk2           = 9,
                ConvTalk3           = 10,
                ConvTalk4           = 11
            };

            enum eEvents
            {
                EventBurningLegSweep        = 1,
                EventDashingFlameGale       = 2,
                EventDashingFlameGaleCharge = 3
            };

            enum eSpells
            {
                BurningLegSweep         = 227568,
                DashingFlameGale        = 227449,
                DahsingFlameGaleFilter  = 227452,
                DashingFlameGaleCharge  = 227453,
                DashingFlameGaleAT      = 227467
            };

            enum eActions
            {
                StartFight = 4
            };

            uint8 m_LimitHealthPct = 51;
            std::vector<uint64> l_RuffiansGUIDs;

            void Reset() override
            {
                boss_opera_encounters::Reset();

                me->DespawnCreaturesInArea(eCreatures::GangRuffian);
            }

            void EnterCombat(Unit* p_Who) override
            {
                boss_opera_encounters::EnterCombat(p_Who);
                summons.DoZoneInCombat(eCreatures::GangRuffian);

                Talk(eTalks::ToeAggro);
                RunEvents();

                m_IsInConversation = true;

                Creature* l_Mrrgria = me->FindNearestCreature(eCreatures::Mrrgria, 50.0f);
                if (l_Mrrgria && l_Mrrgria->IsAIEnabled)
                    l_Mrrgria->GetAI()->DoAction(eActions::StartFight);

                AddTimedDelayedOperation(11 * IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::ConvTalk1);
                });
                AddTimedDelayedOperation(18 * IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::ConvTalk2);
                });
                AddTimedDelayedOperation(24 * IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::ConvTalk3);
                });
                AddTimedDelayedOperation(36 * IN_MILLISECONDS, [this]() -> void
                {
                    Talk(eTalks::ConvTalk4);
                });
                AddTimedDelayedOperation(45 * IN_MILLISECONDS, [this]() -> void
                {
                    m_IsInConversation = false;
                });
            }

            void RunEvents()
            {
                events.ScheduleEvent(eEvents::EventBurningLegSweep, 10000);
            }

            void EnterEvadeMode() override
            {
                Talk(eTalks::Wipe);
                boss_opera_encounters::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(eTalks::ToeDeath);
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                if (instance)
                {
                    if (Creature* l_Mrrgria = instance->instance->GetCreature(instance->GetData64(eCreatures::Mrrgria)))
                    {
                        if (l_Mrrgria->IsAIEnabled)
                            l_Mrrgria->AI()->DoAction(ACTION_3);
                    }
                }
            }

            void KilledUnit(Unit* p_KilledUnit) override
            {
                if (p_KilledUnit->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void JustReachedHome() override
            {
                if (me->isInCombat())
                    me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_EMOTE_DANCE);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::DahsingFlameGaleFilter:
                    {
                        DoCast(p_Target, eSpells::DashingFlameGaleCharge, true);
                        break;
                    }
                    case eSpells::DashingFlameGaleCharge:
                    {
                        DoCast(p_Target, eSpells::DashingFlameGaleAT, true);
                        events.ScheduleEvent(eEvents::EventDashingFlameGaleCharge, 100);
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::BurningLegSweep:
                    {
                        events.ScheduleEvent(eEvents::EventDashingFlameGale, 2000);
                        if (!m_IsInConversation)
                            Talk(eTalks::ToeBurningLegSweep);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(m_LimitHealthPct))
                {
                    m_LimitHealthPct = 0;
                    events.Reset();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->StopAttack();
                    me->GetMotionMaster()->MoveTargetedHome();
                    Talk(eTalks::ToeTimeOut);

                    if (Creature* l_Mrrgria = instance->instance->GetCreature(instance->GetData64(eCreatures::Mrrgria)))
                    {
                        if (l_Mrrgria->IsAIEnabled)
                            l_Mrrgria->AI()->DoAction(ACTION_2);
                    }
                }
            }

            uint64 GetData64(uint32 /*p_ID*/) override
            {
                if (l_RuffiansGUIDs.empty())
                {
                    uint32 l_I = 0;
                    for (uint64 l_GUID : summons)
                    {
                        if (l_I >= 3)
                            break;

                        Creature* l_Creature = Creature::GetCreature(*me, l_GUID);
                        if (!l_Creature || l_Creature->GetEntry() != eCreatures::GangRuffian)
                            continue;

                        l_RuffiansGUIDs.push_back(l_GUID);
                        l_I++;
                    }
                }

                if (l_RuffiansGUIDs.empty())
                    return 0;

                uint64 l_GUID = l_RuffiansGUIDs[l_RuffiansGUIDs.size() - 1];
                l_RuffiansGUIDs.pop_back();

                return l_GUID;
            }

            void DoAction(int32 const p_Action) override
            {
                boss_opera_encounters::DoAction(p_Action);

                switch (p_Action)
                {
                    case ACTION_2:
                    {
                        Talk(eTalks::ToeReturnFight);
                        me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, 0);
                        RunEvents();
                        break;
                    }
                    case ACTION_3:
                    {
                        boss_opera_encounters::JustDied(nullptr);
                        me->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        break;
                    }
                    case ACTION_4:
                    {
                        EntryCheckPredicate l_Pred(eCreatures::GangRuffian);
                        summons.DoAction(ACTION_1, l_Pred); ///< Surrend
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_opera_encounters::UpdateAI(p_Diff);

                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckHomeDistToEvade(p_Diff, 25.0f, -10893.10f, -1763.78f, 90.47f))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBurningLegSweep:
                    {
                        DoCast(eSpells::BurningLegSweep);
                        events.ScheduleEvent(eEvents::EventBurningLegSweep, 20000);
                        break;
                    }
                    case eEvents::EventDashingFlameGale:
                    {
                        if (!m_IsInConversation)
                            Talk(eTalks::ToeFlameGale);
                        DoCast(eSpells::DashingFlameGale);
                        events.ScheduleEvent(eEvents::EventDashingFlameGaleCharge, 100);
                        break;
                    }
                    case eEvents::EventDashingFlameGaleCharge:
                    {
                        DoCast(eSpells::DahsingFlameGaleFilter);
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
            return new boss_rtk_toe_kneeAI(p_Creature);
        }
};

/// Mrrgria <The Fins> - 114260
class boss_rtk_mrrgria : public CreatureScript
{
    public:
        boss_rtk_mrrgria() : CreatureScript("boss_rtk_mrrgria") { }

        struct boss_rtk_mrrgriaAI : public boss_opera_encounters
        {
            boss_rtk_mrrgriaAI(Creature* p_Creature) : boss_opera_encounters(p_Creature)
            {
                me->SetReactState(REACT_DEFENSIVE);
            }

            enum eFactions
            {
                NeutralMurloc = 2220
            };

            enum eTalks
            {
                Conv1Talk1 = 1,
                Conv1Talk2 = 2,
                Conv1Talk3 = 3,
                Conv1Talk4 = 4,

                Conv2Talk1 = 5,
                Conv2Talk2 = 6,
                Conv2Talk3 = 7
            };

            enum eEvents
            {
                EventThunderRitual  = 1,
                EventWashAway       = 2
            };

            enum eSpells
            {
                ThunderRitual  = 227777,
                WashAwayFilter = 227783,
                WashAwayAT     = 227790,
                Stun           = 78320,
            };

            enum eActions
            {
                StartFight = 4,
                MoveMiddle = 5
            };

            bool m_Surrend = false;
            uint8 m_LimitHealthPct = 51;

            std::set<uint64> m_MrrgriaAndSummons;

            void Reset() override
            {
                boss_opera_encounters::Reset();

                me->DespawnCreaturesInArea(eCreatures::ShorelineTidespeaker);

                for (uint8 l_I = 22; l_I < 34; l_I++)
                    me->SummonCreature(eCreatures::WashAway, g_OperaSpawnPos[l_I]);
            }

            void EnterCombat(Unit* p_Who) override
            {
                boss_opera_encounters::EnterCombat(p_Who);

                EntryCheckPredicate l_Pred(eCreatures::ShorelineTidespeaker);
                summons.DoAction(ACTION_2, l_Pred); ///< Go Fight

                events.ScheduleEvent(eEvents::EventThunderRitual, 8000);
                events.ScheduleEvent(eEvents::EventWashAway, 16000);
            }

            void EnterEvadeMode() override
            {
                boss_opera_encounters::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override { }

            void DoAction(int32 const p_Action) override
            {
                boss_opera_encounters::DoAction(p_Action);

                if (p_Action == eActions::StartFight)
                {
                    AddTimedDelayedOperation(15 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::Conv1Talk1);
                    });
                    AddTimedDelayedOperation(21 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::Conv1Talk2);
                    });
                    AddTimedDelayedOperation(33 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::Conv1Talk3);
                    });
                    AddTimedDelayedOperation(42 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::Conv1Talk4);
                    });
                }
                else if (p_Action == ACTION_3 && m_Surrend && instance)
                {
                    if (Creature* l_Toe = instance->instance->GetCreature(instance->GetData64(eCreatures::ToeKnee)))
                    {
                        l_Toe->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        if (l_Toe->IsAIEnabled)
                            l_Toe->AI()->DoAction(ACTION_4);
                    }

                    StartOutroConversation();
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPct(m_LimitHealthPct))
                {
                    m_LimitHealthPct = 0;

                    if (Creature* l_Toe = instance->instance->GetCreature(instance->GetData64(eCreatures::ToeKnee)))
                    {
                        if (l_Toe->IsAIEnabled)
                            l_Toe->AI()->DoAction(ACTION_2);
                    }
                }

                if (p_Damage >= me->GetHealth())
                {
                    me->SetHealth(1);

                    p_Damage = 0;

                    if (!m_Surrend && instance)
                    {
                        m_Surrend = true;
                        events.Reset();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->StopAttack();
                        DoCast(me, eSpells::Stun, true);

                        if (Creature* l_Toe = instance->instance->GetCreature(instance->GetData64(eCreatures::ToeKnee)))
                        {
                            if (l_Toe && !l_Toe->isAlive() && l_Toe->IsAIEnabled)
                            {
                                l_Toe->AI()->DoAction(ACTION_4);
                                l_Toe->AI()->DoAction(ACTION_3);
                                me->RemoveAurasDueToSpell(eSpells::Stun);
                                StartOutroConversation();
                            }
                        }
                    }
                }
            }

            void StartOutroConversation()
            {
                EntryCheckPredicate l_Pred(eCreatures::ShorelineTidespeaker);
                summons.DoAction(ACTION_1, l_Pred); ///< Surrend

                me->RemoveAurasDueToSpell(eSpells::Stun);
                me->setFaction(eFactions::NeutralMurloc);
                me->GetMotionMaster()->Clear();
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(1, -10895.20f, -1782.38f, 90.56f);

                if (instance)
                    instance->SetBossState(eData::OperaHall, EncounterState::DONE);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == 1)
                {
                    me->SetOrientation(4.56f);
                    AddTimedDelayedOperation(1 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::Conv2Talk1);
                    });
                    AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                    {
                        m_MrrgriaAndSummons.clear();

                        Talk(eTalks::Conv2Talk2);

                        Creature* l_ToeKnee = me->FindNearestCreature(eCreatures::ToeKnee, 50.0f, false);
                        if (l_ToeKnee && l_ToeKnee->IsAIEnabled)
                        {
                            std::set<uint64> l_GUIDs;
                            for (uint32 l_I = 0; l_I < 3; ++l_I)
                                l_GUIDs.insert(l_ToeKnee->GetAI()->GetData64(0));

                            std::vector<Position> l_RuffiansPositions =
                            {
                                { -10900.13f, -1769.76f, 90.48f, 4.57f },
                                { -10897.64f, -1767.24f, 90.48f, 4.57f },
                                { -10902.30f, -1766.52f, 90.48f, 4.57f }
                            };

                            uint32 l_I = 0;
                            for (uint64 l_GUID : l_GUIDs)
                            {
                                m_MrrgriaAndSummons.insert(l_GUID);
                                Creature* l_Creature = Creature::GetCreature(*me, l_GUID);
                                if (l_Creature)
                                {
                                    l_Creature->GetMotionMaster()->Clear();
                                    l_Creature->SetWalk(false);
                                    l_Creature->GetMotionMaster()->MovePoint(0, l_RuffiansPositions[l_I]);
                                }
                                l_I++;
                            }
                        }

                        std::set<uint64> l_GUIDs;
                        uint32 l_I = 0;
                        for (uint64 l_GUID : summons)
                        {
                            if (l_I >= 3)
                                break;

                            Creature* l_Creature = Creature::GetCreature(*me, l_GUID);
                            if (!l_Creature || l_Creature->GetEntry() != eCreatures::ShorelineTidespeaker)
                                continue;

                            l_GUIDs.insert(l_GUID);
                            l_I++;
                        }

                        std::vector<Position> l_TidespeakersPositions =
                        {
                            { -10887.21f, -1771.63f, 90.48f, 4.57f },
                            { -10884.47f, -1769.11f, 90.48f, 4.57f },
                            { -10889.23f, -1768.66f, 90.48f, 4.57f }
                        };

                        l_I = 0;
                        for (uint64 l_GUID : l_GUIDs)
                        {
                            Creature* l_Creature = Creature::GetCreature(*me, l_GUID);
                            if (l_Creature)
                            {
                                m_MrrgriaAndSummons.insert(l_GUID);
                                l_Creature->GetMotionMaster()->Clear();
                                l_Creature->SetWalk(false);
                                l_Creature->GetMotionMaster()->MovePoint(0, l_TidespeakersPositions[l_I]);
                            }
                            l_I++;
                        }
                    });

                    AddTimedDelayedOperation(8 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::Conv2Talk3);
                    });

                    AddTimedDelayedOperation(12 * IN_MILLISECONDS, [this]() -> void
                    {
                        m_MrrgriaAndSummons.insert(me->GetGUID());
                        Position const l_ExitDoor = { -10873.55f, -1779.07f, 90.48f, 4.57f };
                        for (uint64 l_GUID : m_MrrgriaAndSummons)
                        {
                            Creature* l_Creature = Creature::GetCreature(*me, l_GUID);
                            if (!l_Creature)
                                continue;

                            l_Creature->GetMotionMaster()->Clear();
                            l_Creature->SetWalk(true);
                            l_Creature->GetMotionMaster()->MovePoint(2, l_ExitDoor);
                        }
                    });
                }

                if (p_Id == 2)
                    me->DespawnOrUnsummon(0);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::WashAwayFilter:
                    {
                        p_Target->CastSpell(p_Target, eSpells::WashAwayAT, true, nullptr, nullptr, me->GetGUID());
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_opera_encounters::UpdateAI(p_Diff);

                UpdateOperations(p_Diff);

                if (!m_Surrend && !UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckHomeDistToEvade(p_Diff, 25.0f, -10893.10f, -1763.78f, 90.47f))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventThunderRitual:
                    {
                        DoCast(eSpells::ThunderRitual);
                        events.ScheduleEvent(eEvents::EventThunderRitual, 12000);
                        break;
                    }
                    case eEvents::EventWashAway:
                    {
                        DoCast(eSpells::WashAwayFilter);
                        events.ScheduleEvent(eEvents::EventWashAway, 23000);
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
            return new boss_rtk_mrrgriaAI(p_Creature);
        }
};

/// Last Update - 7.1.5 Build 23420
/// Gang Ruffian <Defias Brotherhood> - 114265
class npc_rtk_gang_ruffian : public CreatureScript
{
    public:
        npc_rtk_gang_ruffian() : CreatureScript("npc_rtk_gang_ruffian") { }

        struct npc_rtk_gang_ruffianAI : public ScriptedAI
        {
            npc_rtk_gang_ruffianAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->DisableEvadeMode();
                me->SetReactState(REACT_DEFENSIVE);
            }

            enum eFactions
            {
                NeutralGangster = 1828,
                Aggresive = 16
            };

            enum eSpells
            {
                PoisonousShankFilter    = 228065,
                PoisonousShankTP        = 227319,
                PoisonousShankDamage    = 227325
            };

            bool m_Surrend = false;

            void Reset() override { }

            void EnterCombat(Unit* p_Who) override
            {
                events.ScheduleEvent(EVENT_1, urand(5,8) * IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::PoisonousShankFilter:
                    {
                        DoCast(p_Target, eSpells::PoisonousShankTP, true);
                        DoCast(p_Target, eSpells::PoisonousShankDamage, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == ACTION_1)
                {
                    if (!m_Surrend)
                        GoHome();
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;
                    me->SetHealth(1);

                    if (!m_Surrend)
                        GoHome();
                }
            }

            void GoHome()
            {
                m_Surrend = true;
                events.Reset();
                me->StopAttack();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustReachedHome() override
            {
                me->setFaction(eFactions::NeutralGangster);
                me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_EMOTE_DANCE);
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
                    case EVENT_1:
                    {
                        DoCast(eSpells::PoisonousShankFilter);
                        events.ScheduleEvent(EVENT_1, 10 * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == 2)
                    me->DespawnOrUnsummon(0);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_gang_ruffianAI(p_Creature);
        }
};

/// Last Update - 7.1.5 Build 23420
/// Shoreline Tidespeaker <The Fins> - 114266
class npc_rtk_shoreline_tidespeaker : public CreatureScript
{
    public:
        npc_rtk_shoreline_tidespeaker() : CreatureScript("npc_rtk_shoreline_tidespeaker") { }

        struct npc_rtk_shoreline_tidespeakerAI : public Scripted_NoMovementAI
        {
            npc_rtk_shoreline_tidespeakerAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                me->DisableEvadeMode();
                me->SetReactState(REACT_DEFENSIVE);
                me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_EMOTE_DANCE);
            }

            enum eFactions
            {
                NeutralMurloc = 2220,
                Aggresive = 16
            };

            enum eSpells
            {
                BubbleBlast = 227420,
            };

            bool m_Surrend = false;

            void Reset() override { }

            void EnterCombat(Unit* p_Who) override
            {
                events.ScheduleEvent(EVENT_1, 5 * IN_MILLISECONDS);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                    {
                        if (!m_Surrend)
                            GoHome();
                        break;
                    }
                    case ACTION_2:
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->setFaction(eFactions::Aggresive);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, 0);
                        DoZoneInCombat(me, 60.0f);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;

                    if (!m_Surrend)
                        GoHome();
                }
            }

            void GoHome()
            {
                m_Surrend = true;
                events.Reset();
                me->StopAttack();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void JustReachedHome() override
            {
                me->setFaction(eFactions::NeutralMurloc);
                me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_EMOTE_DANCE);
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
                    case EVENT_1:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                        {
                            DoResetThreat();
                            me->AddThreat(l_Target, 10000.0f);
                            me->SetFacingToObject(l_Target);
                            DoCast(l_Target, eSpells::BubbleBlast, true);
                        }
                        events.ScheduleEvent(EVENT_1, 5 * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == 2)
                    me->DespawnOrUnsummon(0);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_shoreline_tidespeakerAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Coggleston <Loyal Majordomo> - 114328
class boss_rtk_coggleston : public CreatureScript
{
    public:
        boss_rtk_coggleston() : CreatureScript("boss_rtk_coggleston") { }

        struct boss_rtk_cogglestonAI : public boss_opera_encounters
        {
            boss_rtk_cogglestonAI(Creature* p_Creature) : boss_opera_encounters(p_Creature)
            {
                me->SetReactState(REACT_DEFENSIVE);
            }

            enum eTalks
            {
                Aggro           = 0,
                Summon          = 1,
                Death           = 2,
                PlayerKilled    = 3,
                Wipe            = 4,
                Conv1Talk       = 5,
                Conv2Talk       = 6
            };

            enum eSpells
            {
                Eminence            = 228729,
                SpectralService     = 232156,
                DentArmor           = 227985,
                DinnerBell          = 227987,
                KaraKazham          = 232153
            };

            enum eEvents
            {
                EventDentArmor      = 1,
                EventSummonHelpers  = 2,
                EventDinnerBell     = 3
            };

            enum eActions
            {
                FirstConversation,
                MiddleFight,
                Outro
            };

            uint8 m_DiedCountDB;

            void Reset() override
            {
                boss_opera_encounters::Reset();
                m_DiedCountDB = 0;
            }

            void EnterCombat(Unit* p_Who) override
            {
                boss_opera_encounters::EnterCombat(p_Who);

                DoCast(me, eSpells::SpectralService, true);

                Creature* l_Bella = me->FindNearestCreature(eCreatures::Bella, 50.0f);
                Creature* l_Brute = me->FindNearestCreature(eCreatures::Brute, 50.0f);
                if (l_Bella && l_Brute && l_Bella->IsAIEnabled && l_Brute->IsAIEnabled)
                {
                    l_Bella->GetAI()->DoAction(eActions::FirstConversation);
                    l_Brute->GetAI()->DoAction(eActions::FirstConversation);
                    AddTimedDelayedOperation(31 * IN_MILLISECONDS, [this]() -> void
                    {
                        Talk(eTalks::Conv1Talk);
                    });
                }

                Talk(eTalks::Aggro);
            }

            void EnterEvadeMode() override
            {
                Talk(eTalks::Wipe);
                boss_opera_encounters::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();
                Talk(eTalks::Death);
                Creature* l_Bella = me->FindNearestCreature(eCreatures::Bella, 50.0f);
                Creature* l_Brute = me->FindNearestCreature(eCreatures::Brute, 50.0f);
                if (l_Bella && l_Brute && l_Bella->IsAIEnabled && l_Brute->IsAIEnabled)
                {
                    l_Bella->GetAI()->DoAction(eActions::Outro);
                    l_Brute->GetAI()->DoAction(eActions::Outro);
                }
            }

            void KilledUnit(Unit* p_KilledUnit) override
            {
                if (p_KilledUnit->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void DoAction(int32 const p_Action) override
            {
                boss_opera_encounters::DoAction(p_Action);

                if (p_Action == ACTION_3 && instance)
                {
                    uint32 l_BossID[4] = { eCreatures::Coggleston, eCreatures::Luminore, eCreatures::Babblet, eCreatures::Cauldrons };

                    for (uint8 l_I = 0; l_I < 4; l_I++)
                    {
                        if (Creature* l_Boss = instance->instance->GetCreature(instance->GetData64(l_BossID[l_I])))
                        {
                            if (l_Boss->isAlive())
                                l_Boss->CastSpell(l_Boss, eSpells::Eminence, true);
                        }
                    }

                    if (m_DiedCountDB == 1)
                    {
                        Creature* l_Bella = me->FindNearestCreature(eCreatures::Bella, 50.0f);
                        Creature* l_Brute = me->FindNearestCreature(eCreatures::Brute, 50.0f);
                        if (l_Bella && l_Brute && l_Bella->IsAIEnabled && l_Brute->IsAIEnabled)
                        {
                            l_Bella->GetAI()->DoAction(eActions::MiddleFight);
                            l_Brute->GetAI()->DoAction(eActions::MiddleFight);
                            AddTimedDelayedOperation(23 * IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(eTalks::Conv2Talk);
                            });
                        }
                    }

                    if (m_DiedCountDB == 2)
                    {
                        me->RemoveAurasDueToSpell(eSpells::SpectralService);

                        events.ScheduleEvent(eEvents::EventDentArmor, 17000);
                        events.ScheduleEvent(eEvents::EventSummonHelpers, 2000);
                        events.ScheduleEvent(eEvents::EventDinnerBell, 28000);
                    }

                    m_DiedCountDB++;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::KaraKazham)
                    Talk(eTalks::Summon);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_opera_encounters::UpdateAI(p_Diff);

                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventDentArmor:
                    {
                        DoCastVictim(eSpells::DentArmor);
                        events.ScheduleEvent(eEvents::EventDentArmor, 30000);
                        break;
                    }
                    case eEvents::EventSummonHelpers:
                    {
                        DoCast(eSpells::KaraKazham);
                        events.ScheduleEvent(eEvents::EventSummonHelpers, 30000);
                        break;
                    }
                    case eEvents::EventDinnerBell:
                    {
                        DoCast(eSpells::DinnerBell);
                        events.ScheduleEvent(eEvents::EventDinnerBell, 20000);
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
            return new boss_rtk_cogglestonAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Luminore <Keeper of Candles> - 114329
class boss_rtk_luminore : public CreatureScript
{
    public:
        boss_rtk_luminore() : CreatureScript("boss_rtk_luminore") { }

        struct boss_rtk_luminoreAI : public boss_opera_encounters
        {
            boss_rtk_luminoreAI(Creature* p_Creature) : boss_opera_encounters(p_Creature)
            {
                me->SetReactState(REACT_DEFENSIVE);
            }

            enum eTalks
            {
                TalkHeatWave    = 0,
                TalkDeath       = 1,
                PlayerKilled    = 2
            };

            enum eSpells
            {
                CandleHat       = 227497,
                BurningBlaze    = 228193,
                HeatWave        = 228025
            };

            enum eEvents
            {
                EventBurningBlaze = 1,
                EventHeatWave = 2,
            };

            uint8 m_CastCount = 0;

            void Reset() override
            {
                boss_opera_encounters::Reset();

                DoCast(me, eSpells::CandleHat, true);
            }

            void EnterCombat(Unit* p_Who) override
            {
                boss_opera_encounters::EnterCombat(p_Who);

                events.ScheduleEvent(eEvents::EventBurningBlaze, 2000);
                events.ScheduleEvent(eEvents::EventHeatWave, 30000);
            }

            void EnterEvadeMode() override
            {
                boss_opera_encounters::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                boss_opera_encounters::JustDied(nullptr);
                Talk(eTalks::TalkDeath);
            }

            void KilledUnit(Unit* p_KilledUnit) override
            {
                if (p_KilledUnit->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_opera_encounters::UpdateAI(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                {
                    if (me->IsStopped())
                    {
                        if (Spell const* l_CurrentGenericSpell = me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                        {
                            if (SpellInfo const* l_SpellInfo = l_CurrentGenericSpell->GetSpellInfo())
                            {
                                if (l_SpellInfo->Id == eSpells::BurningBlaze)
                                    me->GetMotionMaster()->MoveChase(me->getVictim());
                            }
                        }
                    }
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBurningBlaze:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(l_Target, eSpells::BurningBlaze, false);

                        m_CastCount++;

                        if (m_CastCount > 2)
                        {
                            m_CastCount = 0;
                            events.ScheduleEvent(eEvents::EventBurningBlaze, 9000);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventBurningBlaze, 3000);

                        break;
                    }
                    case eEvents::EventHeatWave:
                    {
                        if (urand(0, 1))
                            Talk(eTalks::TalkHeatWave);
                        DoCast(eSpells::HeatWave);
                        events.ScheduleEvent(eEvents::EventHeatWave, 30000);
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
            return new boss_rtk_luminoreAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Babblet <Head Maid> - 114330
class boss_rtk_babblet : public CreatureScript
{
    public:
        boss_rtk_babblet() : CreatureScript("boss_rtk_babblet") { }

        struct boss_rtk_babbletAI : public boss_opera_encounters
        {
            boss_rtk_babbletAI(Creature* p_Creature) : boss_opera_encounters(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            enum eTalks
            {
                PlayerKilled    = 0,
                Death           = 1
            };

            enum eSpells
            {
                MaidAttire              = 227505,
                SevereDusting           = 228213,
                SevereDustingConfuse    = 228215,
                Fixate                  = 228221
            };

            enum eEvents
            {
                EventFixate        = 1,
                EventFollowTarget  = 2,
            };

            uint64 m_PlayerGuid;

            void Reset() override
            {
                boss_opera_encounters::Reset();

                DoCast(me, eSpells::MaidAttire, true);
            }

            void EnterCombat(Unit* p_Who) override
            {
                boss_opera_encounters::EnterCombat(p_Who);

                DoCast(me, eSpells::SevereDusting, true);

                events.ScheduleEvent(eEvents::EventFixate, 1000);
            }

            void EnterEvadeMode() override
            {
                boss_opera_encounters::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                boss_opera_encounters::JustDied(nullptr);
                Talk(eTalks::Death);
            }

            void KilledUnit(Unit* p_KilledUnit) override
            {
                if (p_KilledUnit->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void SpellHitTarget(Unit* /*p_Target*/, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SevereDustingConfuse:
                    {
                        me->InterruptNonMeleeSpells(true);
                        events.CancelEvent(eEvents::EventFixate);
                        events.ScheduleEvent(eEvents::EventFixate, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_opera_encounters::UpdateAI(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventFixate:
                    {
                        m_PlayerGuid = 0;
                        me->StopMoving();
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true, -eSpells::Fixate))
                        {
                            m_PlayerGuid = l_Target->GetGUID();
                            DoCast(l_Target, eSpells::Fixate, true);
                        }
                        events.ScheduleEvent(eEvents::EventFixate, 11000);
                        events.ScheduleEvent(eEvents::EventFollowTarget, 1000);
                        break;
                    }
                    case eEvents::EventFollowTarget:
                    {
                        if (Unit* l_Target = ObjectAccessor::GetUnit(*me, m_PlayerGuid))
                        {
                            if (!l_Target || !l_Target->isAlive())
                            {
                                events.ScheduleEvent(eEvents::EventFixate, 1000);
                                break;
                            }
                            else
                                me->GetMotionMaster()->MovePoint(1, l_Target->GetPosition());
                        }
                        events.ScheduleEvent(eEvents::EventFollowTarget, 500);
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
            return new boss_rtk_babbletAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Mrs. Cauldrons <Sous Chef> - 114522
class boss_rtk_cauldrons : public CreatureScript
{
    public:
        boss_rtk_cauldrons() : CreatureScript("boss_rtk_cauldrons") { }

        struct boss_rtk_cauldronsAI : public boss_opera_encounters
        {
            boss_rtk_cauldronsAI(Creature* p_Creature) : boss_opera_encounters(p_Creature)
            {
                me->SetReactState(REACT_DEFENSIVE);
            }

            enum eSpells
            {
                Dressing        = 227898,
                SoupSpray       = 228011,
                Drenched        = 228013,
                Leftovers       = 228019,
                LeftoversVisual = 228015
            };

            enum eTalks
            {
                TalkSoupSpray   = 0,
                TalkDeath       = 1,
                PlayerKilled    = 2
            };

            enum eEvents
            {
                EventSoupSpray = 1,
                EventLeftovers = 2,
            };

            int32 m_LeftoversCooldown;

            void AttackStart(Unit* p_Target, bool /*p_Melee*/) override
            {
                if (!p_Target)
                    return;

                if (me->Attack(p_Target, false))
                    DoStartNoMovement(p_Target);
            }

            void Reset() override
            {
                boss_opera_encounters::Reset();

                DoCast(me, eSpells::Dressing, true);
            }

            void EnterCombat(Unit* p_Who) override
            {
                boss_opera_encounters::EnterCombat(p_Who);

                events.ScheduleEvent(eEvents::EventSoupSpray, 1000);

                m_LeftoversCooldown = 0;
            }

            void EnterEvadeMode() override
            {
                boss_opera_encounters::EnterEvadeMode();
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                boss_opera_encounters::JustDied(NULL);
                Talk(eTalks::TalkDeath);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::SoupSpray)
                    DoCast(p_Target, eSpells::Drenched, true);
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::Leftovers)
                {
                    for (uint8 l_I = 0; l_I < 20; l_I++)
                        DoCast(me, eSpells::LeftoversVisual, true);
                }
            }

            void KilledUnit(Unit* p_KilledUnit) override
            {
                if (p_KilledUnit->IsPlayer())
                    Talk(eTalks::PlayerKilled);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                boss_opera_encounters::UpdateAI(p_Diff);

                if (!UpdateVictim())
                    return;

                m_LeftoversCooldown -= p_Diff;
                events.Update(p_Diff);
                std::list<Player*> l_Players;
                if (m_LeftoversCooldown < 0)
                {
                    me->GetPlayerListInGrid(l_Players, 10.0f, false);
                    if (!l_Players.empty())
                    {
                        events.ScheduleEvent(eEvents::EventLeftovers, 1);
                        m_LeftoversCooldown = 18 * IN_MILLISECONDS;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventSoupSpray:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                        {
                            if (!urand(0, 6))
                                Talk(eTalks::TalkSoupSpray);
                            DoCast(l_Target, eSpells::SoupSpray, false);
                        }
                        events.ScheduleEvent(eEvents::EventSoupSpray, 3000);
                        break;
                    }
                    case eEvents::EventLeftovers:
                    {
                        DoCast(eSpells::Leftovers);
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
            return new boss_rtk_cauldronsAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Burning Blaze - 8843
class at_rtk_burning_blaze : public AreaTriggerEntityScript
{
    public:
        at_rtk_burning_blaze() : AreaTriggerEntityScript("at_rtk_burning_blaze") { }

        enum eSpells
        {
            Drenched = 228013
        };

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            if (p_Target && p_Target->HasAura(eSpells::Drenched))
            {
                p_Areatrigger->Remove(false);
                return true;
            }

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_rtk_burning_blaze();
        }
};

/// Last Update - 7.1.5 Build 23420
/// Bella - 114545
class npc_rtk_bella : public CreatureScript
{
    public:
        npc_rtk_bella() : CreatureScript("npc_rtk_bella") { }

        struct npc_rtk_bellaAI : public ScriptedAI
        {
            npc_rtk_bellaAI(Creature* p_Creature) : ScriptedAI(p_Creature) {}

            enum eActions
            {
                FirstConversation,
                MiddleFight,
                Outro
            };

            enum eTalks
            {
                Conv1Talk1,
                Conv1Talk2,

                Conv2Talk1,
                Conv2Talk2,
                Conv2Talk3,

                Conv3Talk1,
                Conv3Talk2,
                Conv3Talk3
            };

            enum eSpells
            {
                TrueLove = 228233
            };

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::FirstConversation:
                    {
                        AddTimedDelayedOperation(9 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv1Talk1);
                        });

                        AddTimedDelayedOperation(18 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv1Talk2);
                        });
                        break;
                    }
                    case eActions::MiddleFight:
                    {
                        Talk(eTalks::Conv2Talk1);

                        AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv2Talk2);
                        });

                        AddTimedDelayedOperation(19500, [this]() -> void
                        {
                            Talk(eTalks::Conv2Talk3);
                        });
                        break;
                    }
                    case eActions::Outro:
                    {
                        AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv3Talk1);
                        });

                        AddTimedDelayedOperation(19 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->AddAura(eSpells::TrueLove, me);
                        });

                        AddTimedDelayedOperation(22500, [this]() -> void
                        {
                            Talk(eTalks::Conv3Talk2);
                        });

                        AddTimedDelayedOperation(28 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv3Talk3);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_bellaAI(p_Creature);
        }
};

/// Last Update - 7.1.5 Build 23420
/// Brute - 114551
class npc_rtk_brute : public CreatureScript
{
    public:
        npc_rtk_brute() : CreatureScript("npc_rtk_brute") { }

        struct npc_rtk_bruteAI : public ScriptedAI
        {
            npc_rtk_bruteAI(Creature* p_Creature) : ScriptedAI(p_Creature) {}

            enum eActions
            {
                FirstConversation,
                MiddleFight,
                Outro
            };

            enum eTalks
            {
                Conv1Talk1,
                Conv1Talk2,

                Conv2Talk1,
                Conv2Talk2,

                Conv3Talk1,
                Conv3Talk2,
                Conv3Talk3 = 0
            };

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::FirstConversation:
                    {
                        AddTimedDelayedOperation(13 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv1Talk1);
                        });

                        AddTimedDelayedOperation(24 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv1Talk2);
                        });
                        break;
                    }
                    case eActions::MiddleFight:
                    {
                        AddTimedDelayedOperation(4 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv2Talk1);
                        });

                        AddTimedDelayedOperation(13 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv2Talk2);
                        });
                        break;
                    }
                    case eActions::Outro:
                    {
                        AddTimedDelayedOperation(5 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv3Talk1);
                        });

                        AddTimedDelayedOperation(19 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv3Talk2);
                        });

                        AddTimedDelayedOperation(22 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->UpdateEntry(eCreatures::Adem);
                        });

                        AddTimedDelayedOperation(25 * IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::Conv3Talk3);
                        });
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_bruteAI(p_Creature);
        }
};

/// Barnes <The Stage Manager> - 114339
class npc_rtk_barnes_encounter_starter : public CreatureScript
{
    public:
        npc_rtk_barnes_encounter_starter() : CreatureScript("npc_rtk_barnes_encounter_starter") { }

        struct npc_rtk_barnes_encounter_starterAI : public ScriptedAI
        {
            npc_rtk_barnes_encounter_starterAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                me->SetWalk(true);
            }

            enum eTalks
            {
                WarmingUp,
                IntroWikket,
                IntroWestfall,
                CharactersWestfall,
                PitchWestfall,
                LaunchWestfall,
                IntroBeautifulBeast,
                PitchBeatifulBeast,
                CharactersBeautifulBeast,
                LaunchBeautifulBeast
            };

            enum eSpells
            {
                FeignDeath = 196391,
                BlinkVisual = 232198
            };

            enum eEvents
            {
                StartTalk       = 1,
                EndTalk         = 2,
                EventTeleport   = 3,
                Wikket1         = 4,
                Wikket2         = 5,
                Wikket3         = 6,
                EventWestfall   = 7,
                BeautifulBeast  = 8
            };

            InstanceScript* m_Instance;

            bool m_StartEvent = false;

            void sGossipSelect(Player* /*p_Player*/, uint32 p_Sender, uint32 /*p_Action*/) override
            {
                if (!m_StartEvent && p_Sender == 20468 && m_Instance)
                {
                    m_StartEvent = true;
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    m_Instance->SetData(eData::OperaHallIntro, SPECIAL);
                    events.ScheduleEvent(eEvents::StartTalk, 1 * IN_MILLISECONDS);

                    switch (m_Instance->GetData(eData::OperaHallScene))
                    {
                        case eData::OperaHallWikket:
                        {
                            events.ScheduleEvent(eEvents::Wikket1, 10 * IN_MILLISECONDS);
                            break;
                        }
                        case eData::OperaHallWestfall:
                        {
                            events.ScheduleEvent(eEvents::EventWestfall, 10 * IN_MILLISECONDS);
                            break;
                        }
                        case eData::OperaHallBeautifulBeast:
                        {
                            events.ScheduleEvent(eEvents::BeautifulBeast, 10 * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void Reset() override
            {
                if (m_Instance && m_Instance->GetBossState(eData::OperaHall) == DONE)
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == POINT_MOTION_TYPE)
                {
                    if (p_ID == 1)
                    {
                        DoCast(me, eSpells::BlinkVisual, true);
                        events.ScheduleEvent(eEvents::EventTeleport, 1 * IN_MILLISECONDS);
                    }
                }
            }

            /// Summon Spotlight and text delay event
            void TextEvent(uint32 p_SpotlightTimer, uint8 p_TextID)
            {
                if (Creature* l_Spotlight = me->SummonCreature(eCreatures::TheatreSpotlight, me->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, p_SpotlightTimer))
                    l_Spotlight->CastSpell(l_Spotlight, 25824, true);

                /// Only Opera scene: "Westfall" and "Beautiful Beast".
                if (!p_TextID)
                    return;

                bool l_Westfall = m_Instance ? m_Instance->GetData(eData::OperaHallScene) == eData::OperaHallWestfall : false;

                for (uint8 l_I = 0; l_I < 4; ++l_I)
                    DelayTalk((l_Westfall ? 8 : 12) * l_I, l_I + p_TextID);

                events.ScheduleEvent(eEvents::EndTalk, (l_Westfall ? 38 : 44) * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::StartTalk:
                    {
                        Talk(eTalks::WarmingUp);
                        break;
                    }
                    case eEvents::Wikket1:
                    {
                        me->SummonCreature(eCreatures::TheMonkeyKing, -10897.90f, -1805.59f, 91.55f, 1.46f);
                        TextEvent(5000, eTalks::WarmingUp);
                        events.ScheduleEvent(eEvents::Wikket2, 1 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::Wikket2:
                    {
                        Talk(eTalks::IntroWikket);
                        events.ScheduleEvent(eEvents::Wikket3, 4 * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::Wikket3:
                    {
                        DoCast(me, eSpells::FeignDeath, true);
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                        me->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        break;
                    }
                    case eEvents::EventWestfall:
                    {
                        TextEvent(40000, eTalks::IntroWestfall);
                        break;
                    }
                    case eEvents::BeautifulBeast:
                    {
                        TextEvent(42000, eTalks::IntroBeautifulBeast);
                        break;
                    }
                    case eEvents::EndTalk:
                    {
                        me->GetMotionMaster()->MovePoint(1, -10876.70f, -1779.33f, 90.47f);
                        break;
                    }
                    case eEvents::EventTeleport:
                    {
                        me->NearTeleportTo(-10863.10f, -1782.65f, 90.46f, 1.48f);

                        if (m_Instance)
                            m_Instance->SetData(eData::OperaHallIntro, IN_PROGRESS);

                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_barnes_encounter_starterAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// The Monkey King - 115022
class npc_rtk_monkey_king : public CreatureScript
{
    public:
        npc_rtk_monkey_king() : CreatureScript("npc_rtk_monkey_king") { }

        struct npc_rtk_monkey_kingAI : public ScriptedAI
        {
            npc_rtk_monkey_kingAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
            }

            enum eAnim
            {
                KingPunch = 9782
            };

            enum eSpells
            {
                Shadowstep = 108611
            };

            InstanceScript* m_Instance;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, eSpells::Shadowstep, true);
                events.ScheduleEvent(EVENT_1, 1 * IN_MILLISECONDS);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE)
                {
                    switch (p_ID)
                    {
                        case 1:
                        {
                            events.ScheduleEvent(EVENT_2, 1 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_3, 2 * IN_MILLISECONDS);
                            break;
                        }
                        case 2:
                        {
                            for (uint8 l_I = 0; l_I < 5; ++l_I)
                            {
                                me->m_Functions.AddFunction([this, l_I]() -> void
                                {
                                    Talk(l_I);
                                }, me->m_Functions.CalculateTime(l_I * 6000));
                            }
                            events.ScheduleEvent(EVENT_5, 30 * IN_MILLISECONDS);
                            break;
                        }
                        case 3:
                        {
                            events.ScheduleEvent(EVENT_6, 2 * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }

                if (p_Type == POINT_MOTION_TYPE)
                {
                    if (p_ID == 1)
                        me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->GetMotionMaster()->MoveJump(-10894.74f, -1778.19f, 90.47f, 20.0f, 10.0f, 0.0f, 1);
                        break;
                    }
                    case EVENT_2:
                    {
                        me->SetFacingTo(4.62f);
                        break;
                    }
                    case EVENT_3:
                    {
                        me->PlayOneShotAnimKitId(eAnim::KingPunch);
                        events.ScheduleEvent(EVENT_4, 2 * IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_4:
                    {
                        me->GetMotionMaster()->MoveJump(-10895.53f, -1785.79f, 92.34f, 10.0f, 10.0f, 0.0f, 2);
                        break;
                    }
                    case EVENT_5:
                    {
                        me->GetMotionMaster()->MoveJump(-10896.03f, -1792.25f, 78.39f, 15.0f, 15.0f, 0.0f, 3);
                        break;
                    }
                    case EVENT_6:
                    {
                        if (m_Instance)
                            m_Instance->SetData(eData::OperaHallIntro, IN_PROGRESS);

                        me->GetMotionMaster()->MovePoint(1, -10868.11f, -1793.59f, 78.39f);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_rtk_monkey_kingAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Bubble Blast - 227420
class spell_rtk_bubble_blast : public SpellScriptLoader
{
    public:
        spell_rtk_bubble_blast() : SpellScriptLoader("spell_rtk_bubble_blast") { }

        class spell_rtk_bubble_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rtk_bubble_blast_SpellScript);

            void HandleBeforeCast()
            {
                GetSpell()->destTarget = &*GetExplTargetUnit();
            }

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                    GetSpell()->destTarget = &*l_Caster;
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_rtk_bubble_blast_SpellScript::HandleBeforeCast);
                OnEffectHit += SpellEffectFn(spell_rtk_bubble_blast_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_CREATE_AREATRIGGER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rtk_bubble_blast_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_opera_hall()
{
    /// Wikket
    new boss_rtk_galindre();
    new boss_rtk_elfyra();
    new spell_rtk_defy_gravity();
    new spell_rtk_flashy_bolt();

    /// Westfall
    new boss_rtk_toe_knee();
    new boss_rtk_mrrgria();
    new npc_rtk_gang_ruffian();
    new npc_rtk_shoreline_tidespeaker();
    new spell_rtk_bubble_blast();

    /// Beautiful and the beast
    new boss_rtk_coggleston();
    new boss_rtk_luminore();
    new boss_rtk_babblet();
    new boss_rtk_cauldrons();
    new at_rtk_burning_blaze();
    new npc_rtk_bella();
    new npc_rtk_brute();

    /// Creatures
    new npc_rtk_barnes_encounter_starter();
    new npc_rtk_monkey_king();

}
#endif
