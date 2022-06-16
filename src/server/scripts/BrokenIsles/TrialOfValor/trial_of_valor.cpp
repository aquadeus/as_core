////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "trial_of_valor.hpp"

std::vector<G3D::Vector3> g_HallsOfValorPoints =
{
    { 2444.887f, 776.004f, 253.217f },
    { 2471.361f, 803.702f, 252.203f },
    { 2504.944f, 800.402f, 256.357f },
    { 2486.049f, 781.400f, 260.162f }
};

std::vector<G3D::Vector3> g_TrialOfValorPoints =
{
    { 2333.979f, 920.336f, 252.203f },
    { 2301.537f, 929.034f, 257.313f },
    { 2309.382f, 943.766f, 260.162f },
    { 2323.442f, 943.570f, 260.162f }
};

/// Worthy Vyrkul - 110973
class npc_tov_worthy_vyrkul : public CreatureScript
{
    public:
        npc_tov_worthy_vyrkul() : CreatureScript("npc_tov_worthy_vyrkul") { }

        struct npc_tov_worthy_vyrkulAI : public ScriptedAI
        {
            npc_tov_worthy_vyrkulAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            enum eSpells
            {
                TheChosen = 220807
            };

            enum eEvents
            {
                EventStartMoving = 1,
                EventRespawn
            };

            enum eGameObjects
            {
                GameObjectTOVEntrance = 253127,
                GameObjectHOVEntrance = 252237
            };

            InstanceScript* m_Instance;

            void Reset() override
            {
                events.Reset();

                me->SetReactState(ReactStates::REACT_PASSIVE);

                events.ScheduleEvent(eEvents::EventStartMoving, irand(1 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS));

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == EFFECT_MOTION_TYPE)
                {
                    me->CastSpell(me, eSpells::TheChosen);
                    events.ScheduleEvent(eEvents::EventRespawn, 1 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventStartMoving:
                    {
                        me->GetMotionMaster()->Clear();

                        if (me->FindNearestGameObject(GameObjectTOVEntrance, 15.0f))
                        {
                            me->GetMotionMaster()->MoveSmoothPath(1, g_TrialOfValorPoints.data(), g_TrialOfValorPoints.size(), true);
                        }
                        else if (me->FindNearestGameObject(GameObjectHOVEntrance, 15.0f))
                            me->GetMotionMaster()->MoveSmoothPath(1, g_HallsOfValorPoints.data(), g_HallsOfValorPoints.size(), true);
                        break;
                    }
                    case eEvents::EventRespawn:
                    {
                        me->NearTeleportTo(me->GetHomePosition());
                        me->Respawn(true, true, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_worthy_vyrkulAI(p_Creature);
        }
};

/// Stormforged Sentinal - 114789
class npc_tov_stormforged_sentinal : public CreatureScript
{
    public:
        npc_tov_stormforged_sentinal() : CreatureScript("npc_tov_stormforged_sentinal") { }

        struct npc_tov_stormforged_sentinalAI : public ScriptedAI
        {
            npc_tov_stormforged_sentinalAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            enum eSpells
            {
                ValarjarGhostVisual = 193515,
                Salute              = 220291
            };

            enum eEvents
            {
                EventReactiveCanSalute = 1
            };

            enum eTimers
            {
                TimerSalute = 5 * TimeConstants::IN_MILLISECONDS
            };

            InstanceScript* m_Instance;

            bool m_CanSalute;

            uint32 m_SafeDiff;

            void Reset() override
            {
                m_CanSalute = true;

                me->LoadEquipment(1, true);

                m_SafeDiff = 1 * TimeConstants::IN_MILLISECONDS;

                /// me->CastSpell(me, eSpells::ValarjarGhostVisual, true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC  | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (m_CanSalute)
                {
                    if (m_SafeDiff <= p_Diff)
                    {
                        if (me->FindNearestPlayer(15.0f))
                        {
                            m_CanSalute = false;

                            me->CastSpell(me, eSpells::Salute);
                        }

                        m_SafeDiff = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_SafeDiff -= p_Diff;
                }

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventReactiveCanSalute:
                    {
                        m_CanSalute = true;
                        events.ScheduleEvent(eEvents::EventReactiveCanSalute, eTimers::TimerSalute + 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_stormforged_sentinalAI(p_Creature);
        }
};

/// Bonespeaker Soulbinder - 114532
class npc_tov_bonespeaker_soulbinder : public CreatureScript
{
    public:
        npc_tov_bonespeaker_soulbinder() : CreatureScript("npc_tov_bonespeaker_soulbinder") { }

        struct npc_tov_bonespeaker_soulbinderAI : public ScriptedAI
        {
            npc_tov_bonespeaker_soulbinderAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
                m_Soulbinder = false;
            }

            enum eTimers
            {
                TimerBindSpirit    = 10 * TimeConstants::IN_MILLISECONDS,
                TimerShatteredRune = 3 * TimeConstants::IN_MILLISECONDS
            };

            enum eEvents
            {
                EventBindSpirit = 1,
                EventShatteredRune
            };

            enum eSpells
            {
                SpellBindSpiritAbsorb      = 228395,
                SpellBindSpiritHeal        = 228394,
                SpellBindSpiritDum         = 228470,
                SpellShatteredRuneWepDmg   = 195474,
                SpellShatteredRuneWepDmg02 = 195474,
                SpellShatteredRuneDmg      = 198962,
                SpellShatteredRuneDmg02    = 228391,
                SpellKneeling              = 197227
            };

            InstanceScript* m_Instance;

            uint64 m_BindSpiritGuid;

            uint32 m_BindSpiritDiff;

            bool m_Soulbinder;

            void Reset() override
            {
                events.Reset();

                m_BindSpiritGuid = 0;

                m_BindSpiritDiff = 1 * TimeConstants::IN_MILLISECONDS;
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->RemoveAura(eSpells::SpellKneeling);

                events.ScheduleEvent(eEvents::EventBindSpirit, eTimers::TimerBindSpirit);
                events.ScheduleEvent(eEvents::EventShatteredRune, eTimers::TimerShatteredRune);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                /// Bind Spirit
                if (m_BindSpiritGuid)
                {
                    if (m_BindSpiritDiff <= p_Diff)
                    {
                        if (Creature* l_BindSpiritTarget = Creature::GetCreature(*me, m_BindSpiritGuid))
                        {
                            if (!l_BindSpiritTarget->HasAura(eSpells::SpellBindSpiritAbsorb))
                            {
                                m_BindSpiritGuid = 0;
                                return;
                            }

                            if (l_BindSpiritTarget->GetHealthPct() <= 1.0f)
                            {
                                m_BindSpiritGuid = 0;

                                l_BindSpiritTarget->CastSpell(l_BindSpiritTarget, 211967); ///< Bind Spirit shit explosion vis

                                l_BindSpiritTarget->RemoveAura(228395);

                                l_BindSpiritTarget->SetHealth(l_BindSpiritTarget->GetMaxHealth());

                                l_BindSpiritTarget->CastSpell(l_BindSpiritTarget, 228393, false);

                                l_BindSpiritTarget->ToCreature()->SetReactState(ReactStates::REACT_AGGRESSIVE);

                                if (Player* l_Target = l_BindSpiritTarget->FindNearestPlayer(20.0f))
                                {
                                    if (UnitAI* l_Ai = l_BindSpiritTarget->GetAI())
                                        l_Ai->AttackStart(l_Target, true);
                                }
                            }
                        }

                        m_BindSpiritDiff = 1 * TimeConstants::IN_MILLISECONDS;
                    }
                    else
                        m_BindSpiritDiff -= p_Diff;
                }

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBindSpirit:
                    {
                        if (!m_Soulbinder)
                        {
                            std::list<Creature*> l_ListCreaturesTemp;
                            me->GetCreatureListInGrid(l_ListCreaturesTemp, 15.0f);

                            l_ListCreaturesTemp.remove_if([this](Creature* p_Creature) -> bool
                            {
                                if (p_Creature == nullptr || p_Creature->isDead() || p_Creature->HasAura(eSpells::SpellBindSpiritAbsorb))
                                    return true;

                                if (p_Creature->GetEntry() == eTovCreatures::CreatureBonespeakerSoulBinder ||
                                    p_Creature->GetEntry() == eTovCreatures::CreatureKvaldirSpiritTender ||
                                    p_Creature->GetEntry() == eTovCreatures::CreatureKvaldirReefcaller ||
                                    p_Creature->GetEntry() == eTovCreatures::CreatureKvaldirCoralMaiden)
                                  return false;

                                return false;
                            });

                            if (!l_ListCreaturesTemp.empty())
                            {
                                l_ListCreaturesTemp.sort(JadeCore::UnitHealthState(false));

                                std::list<Creature*>::const_iterator l_Itr = l_ListCreaturesTemp.begin();

                                if ((*l_Itr) != nullptr)
                                {
                                    m_Soulbinder = true;

                                    m_BindSpiritGuid = (*l_Itr)->GetGUID();

                                    /// Choose target
                                    (*l_Itr)->AddAura(eSpells::SpellBindSpiritAbsorb, (*l_Itr));
                                }
                            }

                            events.ScheduleEvent(eEvents::EventBindSpirit, eTimers::TimerBindSpirit);
                        }

                        break;
                    }
                    case eEvents::EventShatteredRune:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            me->CastSpell(l_Target, eSpells::SpellShatteredRuneDmg02, false);

                        events.ScheduleEvent(eEvents::EventShatteredRune, eTimers::TimerShatteredRune);
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
            return new npc_tov_bonespeaker_soulbinderAI(p_Creature);
        }
};

/// Helhound - 114534
class npc_tov_hellhound : public CreatureScript
{
    public:
        npc_tov_hellhound() : CreatureScript("npc_tov_hellhound") { }

        struct npc_tov_hellhoundAI : public ScriptedAI
        {
            npc_tov_hellhoundAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            enum eEvents
            {
                EventBreathOfDread = 1
            };

            enum eTimers
            {
                TimerBreathOfDread = 9 * TimeConstants::IN_MILLISECONDS
            };

            enum eSpells
            {
                SpellBreathOfDreadTriggerMissile    = 228368,
                SpellBreathOfDreadAt                = 228370,
                SpellBreathOfDreadDot               = 228371
            };

            InstanceScript* m_Instance;

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventBreathOfDread, eTimers::TimerBreathOfDread);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBreathOfDread:
                    {
                        me->CastSpell(me, eSpells::SpellBreathOfDreadTriggerMissile);

                        events.ScheduleEvent(eEvents::EventBreathOfDread, eTimers::TimerBreathOfDread * 2);
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
            return new npc_tov_hellhoundAI(p_Creature);
        }
};

/// Risen Bonethrall - 114546
class npc_tov_risen_bonethrall : public CreatureScript
{
    public:
        npc_tov_risen_bonethrall() : CreatureScript("npc_tov_risen_bonethrall") { }

        struct npc_tov_risen_bonethrallAI : public ScriptedAI
        {
            npc_tov_risen_bonethrallAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            enum eEvents
            {
                EventUnyieldingRend = 1,
                EventBoneChillingScream
            };

            enum eTimers
            {
                TimerBoneChillingScream = 5 * TimeConstants::IN_MILLISECONDS,
                TimerUnyieldingRend = 6 * TimeConstants::IN_MILLISECONDS
            };

            enum eSpells
            {
                UnyieldingRend = 223805,
                Sleep          = 107674
            };

            InstanceScript* m_Instance;

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->RemoveAura(eSpells::Sleep);

                events.ScheduleEvent(eEvents::EventUnyieldingRend, eTimers::TimerUnyieldingRend);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventUnyieldingRend:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.0f, true, -eSpells::UnyieldingRend))
                            me->CastSpell(l_Target, eSpells::UnyieldingRend);

                        events.ScheduleEvent(eEvents::EventUnyieldingRend, eTimers::TimerUnyieldingRend);
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
            return new npc_tov_risen_bonethrallAI(p_Creature);
        }
};

/// Kvaldir Spirittender - 114538
class npc_tov_kvaldir_spirittender : public CreatureScript
{
    public:
        npc_tov_kvaldir_spirittender() : CreatureScript("npc_tov_kvaldir_spirittender") { }

        struct npc_tov_kvaldir_spirittenderAI : public ScriptedAI
        {
            npc_tov_kvaldir_spirittenderAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            enum eEvents
            {
                EventHewSpell = 1,
                EventFireBomb
            };

            enum eTimers
            {
                TimerHewSpell = 3 * TimeConstants::IN_MILLISECONDS,
                TimerFireBomb = 2 * TimeConstants::IN_MILLISECONDS
            };

            enum eSpells
            {
                SpellHewSpellDmg        = 228363,
                SpellHewSpellMultiEff01 = 199061,
                SpellFireBombTriggerMis = 228321,
                SpellFireBombDmg        = 228325,
                GrabFireDummyAur        = 228359,
                GrabFire02              = 228341,
                NearBrazier             = 228317
            };

            enum eSpellVisual
            {
                FireBombExplosion = 38945 ///< travel speed 40
            };

            InstanceScript* m_Instance;

            uint32 m_LookForBraizerDiff;

            uint8 m_Mode; /// 1 - hew, 2 - firebomb

            void Reset() override
            {
                events.Reset();

                m_Mode = 0;

                m_LookForBraizerDiff = 3 * TimeConstants::IN_MILLISECONDS;

                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->CallForHelp(15.f);

                me->AddAura(eSpells::GrabFireDummyAur, me);

                SetCombatMovement(true);

                if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.f, true))
                {
                    me->Attack(l_Target, true);
                    events.ScheduleEvent(eEvents::EventFireBomb, 4 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                /*
                /// Chang.
                if (GameObject* l_Bonfire = me->FindNearestGameObject(0, 10.f))
                    SetCombatMovement(false);
                else
                    SetCombatMovement(false);
                */

                if (m_LookForBraizerDiff <= p_Diff)
                {
                    /// Movement changes for campfire behavior to firebombs.
                    if (Creature* l_Brazier = me->FindNearestCreature(eTovCreatures::CreatureBrazierStalker, 15.f))
                    {
                        if (m_Mode != 2)
                        {
                            m_Mode = 2;

                            me->StopMoving();

                            me->AddAura(eSpells::NearBrazier, me);

                            me->AddAura(eSpells::GrabFireDummyAur, me);

                            events.CancelEvent(eEvents::EventHewSpell);

                            events.CancelEvent(eEvents::EventFireBomb);

                            events.ScheduleEvent(eEvents::EventFireBomb, eTimers::TimerFireBomb);

                            if (!me->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED))
                                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);
                        }
                    }
                    else
                    {
                        if (m_Mode != 1)
                        {
                            m_Mode = 1;

                            me->RemoveAura(eSpells::NearBrazier);

                            if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.f, true))
                                me->GetMotionMaster()->MoveChase(l_Target);

                            if (me->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED))
                                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_SERVER_CONTROLLED);

                            events.CancelEvent(eEvents::EventFireBomb);
                            events.CancelEvent(eEvents::EventHewSpell);

                            events.ScheduleEvent(eEvents::EventHewSpell, eTimers::TimerHewSpell);
                        }
                    }

                    m_LookForBraizerDiff = 1 * TimeConstants::IN_MILLISECONDS;
                }
                else
                    m_LookForBraizerDiff -= p_Diff;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventHewSpell:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO, 0, 100.f, true))
                        {
                            if(l_Target->IsWithinMeleeRange(me))
                              me->CastSpell(l_Target, eSpells::SpellHewSpellMultiEff01);
                        }

                        events.ScheduleEvent(eEvents::EventHewSpell, eTimers::TimerHewSpell);
                        break;
                    }
                    case eEvents::EventFireBomb:
                    {
                        me->AddAura(228341, me);

                        if (Unit* l_Victim = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 100.f, true))
                            me->CastSpell(l_Victim, eSpells::SpellFireBombTriggerMis);

                        me->RemoveAura(eSpells::GrabFireDummyAur);

                        events.ScheduleEvent(eEvents::EventFireBomb, eTimers::TimerFireBomb + 10 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tov_kvaldir_spirittenderAI(p_Creature);
        }
};

/// Dark Seraph - 114922
class npc_tov_dark_seraph : public CreatureScript
{
    public:
        npc_tov_dark_seraph() : CreatureScript("npc_tov_dark_seraph") { }

        struct npc_tov_dark_seraphAI : public ScriptedAI
        {
            npc_tov_dark_seraphAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventUnconsecratedBlow = 1,
                EventUnholyReckoning
            };

            enum eSpells
            {
                SpellUnholyReckoning    = 228883,
                SpellUnconsecratedBlow  = 228875
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvents::EventUnconsecratedBlow, 5 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventUnholyReckoning, 15 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventUnconsecratedBlow:
                    {
                        DoCastVictim(eSpells::SpellUnconsecratedBlow);
                        events.ScheduleEvent(eEvents::EventUnconsecratedBlow, 12 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventUnholyReckoning:
                    {
                        if (Player* l_Player = SelectPlayerTarget(eTargetTypeMask::TypeMaskAll))
                            DoCast(l_Player, eSpells::SpellUnholyReckoning);

                        events.ScheduleEvent(eEvents::EventUnholyReckoning, 15 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tov_dark_seraphAI(p_Creature);
        }
};

/// Deepbrine Monstruosity - 114932
class npc_tov_deepbrine_monstruosity : public CreatureScript
{
    public:
        npc_tov_deepbrine_monstruosity() : CreatureScript("npc_tov_deepbrine_monstruosity") { }

        struct npc_tov_deepbrine_monstruosityAI : public ScriptedAI
        {
            npc_tov_deepbrine_monstruosityAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvent
            {
                EventDarkUndertow = 1
            };

            enum eSpell
            {
                SpellDarkUndertow = 228889
            };

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eEvent::EventDarkUndertow, 6 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvent::EventDarkUndertow:
                    {
                        DoCast(me, eSpell::SpellDarkUndertow);
                        events.ScheduleEvent(eEvent::EventDarkUndertow, 15 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tov_deepbrine_monstruosityAI(p_Creature);
        }
};

/// Kvaldir Reefcaller - 114539
class npc_tov_kvaldir_reefcaller : public CreatureScript
{
    public:
        npc_tov_kvaldir_reefcaller() : CreatureScript("npc_tov_kvaldir_reefcaller") { }

        struct npc_tov_kvaldir_reefcallerAI : public ScriptedAI
        {
            npc_tov_kvaldir_reefcallerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventBrackishBolt = 1,
                EventHorrorOfTheDrowned
            };

            enum eSpells
            {
                SpellBrackishBolt       = 228373,
                SpellHorrorOfTheDrowned = 228374
            };

            enum eTimers
            {
                TimerBrackishBolt = 1 * TimeConstants::IN_MILLISECONDS,
            };

            void Reset() override
            {
                events.Reset();

                if (me->GetPositionZ() <= 5.0f)
                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_STATE_ANIM_ID, Anim::ANIM_KNEEL_LOOP);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_STATE_ANIM_ID, 0);

                events.ScheduleEvent(eEvents::EventBrackishBolt, 1 * TimeConstants::IN_MILLISECONDS + 500);
                events.ScheduleEvent(eEvents::EventHorrorOfTheDrowned, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventBrackishBolt:
                    {
                        DoCastVictim(eSpells::SpellBrackishBolt);

                        events.ScheduleEvent(eEvents::EventBrackishBolt, 2 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventHorrorOfTheDrowned:
                    {
                        if (Unit* l_Player = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 100.f, true))
                            DoCast(l_Player, eSpells::SpellHorrorOfTheDrowned);

                        events.ScheduleEvent(eEvents::EventHorrorOfTheDrowned, 12 * TimeConstants::IN_MILLISECONDS);
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
            return new npc_tov_kvaldir_reefcallerAI(p_Creature);
        }
};

/// Odyn - 116760
class npc_tov_cosmetic_odyn : public CreatureScript
{
    public:
        npc_tov_cosmetic_odyn() : CreatureScript("npc_tov_cosmetic_odyn") { }

        bool OnGossipSelect(Player* p_Player, Creature* /*p_Creature*/, uint32 /*p_Sender*/, uint32 /*p_Action*/) override
        {
            p_Player->TeleportTo(eToVLocations::LocHelheimRaidExitTarget);
            return true;
        }

        struct npc_tov_cosmetic_odynAI : public ScriptedAI
        {
            npc_tov_cosmetic_odynAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SpellOdynTeleport       = 232580,
                SpellOdynTeleportVisual = 232581
            };

            void Reset() override
            {
                SetFlyMode(true);

                DoCast(me, eSpells::SpellOdynTeleport, true);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveJump({ 498.5955f, 640.2118f, 2.073754f, me->m_orientation }, 30.0f, 20.0f, 1);
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                if (p_ID == 1)
                {
                    SetFlyMode(false);

                    DoCast(me, eSpells::SpellOdynTeleportVisual, true);

                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 4180, me, nullptr, me->GetPosition()))
                            delete l_Conversation;
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
            return new npc_tov_cosmetic_odynAI(p_Creature);
        }
};

/// Hyrja <Chosen of Eyir> - 116830
class npc_tov_cosmetic_hyrja : public CreatureScript
{
    public:
        npc_tov_cosmetic_hyrja() : CreatureScript("npc_tov_cosmetic_hyrja") { }

        struct npc_tov_cosmetic_hyrjaAI : public ScriptedAI
        {
            npc_tov_cosmetic_hyrjaAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                SetFlyMode(true);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, { 538.2222f, 618.4114f, 28.94947f, me->m_orientation });
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_cosmetic_hyrjaAI(p_Creature);
        }
};

/// Eyir - 116818
class npc_tov_cosmetic_eyir : public CreatureScript
{
    public:
        npc_tov_cosmetic_eyir() : CreatureScript("npc_tov_cosmetic_eyir") { }

        struct npc_tov_cosmetic_eyirAI : public ScriptedAI
        {
            npc_tov_cosmetic_eyirAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                SetFlyMode(true);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, { 519.8871f, 630.7708f, 19.2822f, me->m_orientation });
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_cosmetic_eyirAI(p_Creature);
        }
};

/// Chosen of Eyir - 116762
class npc_tov_cosmetic_chosen_of_eyir : public CreatureScript
{
    public:
        npc_tov_cosmetic_chosen_of_eyir() : CreatureScript("npc_tov_cosmetic_chosen_of_eyir") { }

        struct npc_tov_cosmetic_chosen_of_eyirAI : public ScriptedAI
        {
            npc_tov_cosmetic_chosen_of_eyirAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint8 m_JumpID = 0;

            std::array<Position const, 2> m_JumpPos =
            {
                {
                    { 555.6875f, 650.2153f, 24.27172f, 5.2269260f },
                    { 542.7587f, 600.1007f, 24.40419f, 0.5324915f }
                }
            };

            void Reset() override
            {
                SetFlyMode(true);

                AddTimedDelayedOperation(10, [this]() -> void
                {
                    if (m_JumpID >= 2)
                        m_JumpID = 0;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, m_JumpPos[m_JumpID]);
                });
            }

            void SetData(uint32 /*p_ID*/, uint32 p_Value) override
            {
                m_JumpID = p_Value;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_cosmetic_chosen_of_eyirAI(p_Creature);
        }
};

/// Stormforged Sentinel - 116763
class npc_tov_cosmetic_stormforged_sentinel : public CreatureScript
{
    public:
        npc_tov_cosmetic_stormforged_sentinel() : CreatureScript("npc_tov_cosmetic_stormforged_sentinel") { }

        struct npc_tov_cosmetic_stormforged_sentinelAI : public ScriptedAI
        {
            npc_tov_cosmetic_stormforged_sentinelAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpell
            {
                SpellChosenTeleport = 232583
            };

            void Reset() override
            {
                AddTimedDelayedOperation(10, [this]() -> void
                {
                    DoCast(me, eSpell::SpellChosenTeleport, true);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_cosmetic_stormforged_sentinelAI(p_Creature);
        }
};

/// Hymdall - 116761
class npc_tov_cosmetic_hymdall : public CreatureScript
{
    public:
        npc_tov_cosmetic_hymdall() : CreatureScript("npc_tov_cosmetic_hymdall") { }

        struct npc_tov_cosmetic_hymdallAI : public ScriptedAI
        {
            npc_tov_cosmetic_hymdallAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpell
            {
                SpellChosenTeleport = 232582
            };

            void Reset() override
            {
                AddTimedDelayedOperation(10, [this]() -> void
                {
                    DoCast(me, eSpell::SpellChosenTeleport, true);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_tov_cosmetic_hymdallAI(p_Creature);
        }
};

/// Unholy Reckoning - 228883
class spell_tov_unholy_reckoning : public SpellScriptLoader
{
    public:
        spell_tov_unholy_reckoning() : SpellScriptLoader("spell_tov_unholy_reckoning") { }

        class spell_tov_unholy_reckoning_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_unholy_reckoning_AuraScript);

            enum eSpell
            {
                SpellUnholyReckoningKill = 228884
            };

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetTarget() || GetTargetApplication()->GetRemoveMode() != AuraRemoveMode::AURA_REMOVE_BY_EXPIRE)
                    return;

                GetTarget()->CastSpell(GetTarget(), eSpell::SpellUnholyReckoningKill, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_tov_unholy_reckoning_AuraScript::AfterRemove, SpellEffIndex::EFFECT_1, AuraType::SPELL_AURA_SCHOOL_HEAL_ABSORB, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_unholy_reckoning_AuraScript();
        }
};

/// Bind Spirit  - 228395
class spell_tov_bind_spirit_on_absorb : public SpellScriptLoader
{
    public:
        spell_tov_bind_spirit_on_absorb() : SpellScriptLoader("spell_tov_bind_spirit_on_absorb") { }

        class spell_tov_bind_spirit_on_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_bind_spirit_on_absorb_AuraScript);

            void Absorb(AuraEffect* /*p_AurEff*/, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (l_Target->GetHealthPct() > 2)
                        return;
               }
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_tov_bind_spirit_on_absorb_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_bind_spirit_on_absorb_AuraScript();
        }
};

/// Breath of Dread - 228371
class spell_tov_breath_of_dread : public SpellScriptLoader
{
    public:
        spell_tov_breath_of_dread() : SpellScriptLoader("spell_tov_breath_of_dread") { }

        class spell_tov_breath_of_dread_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tov_breath_of_dread_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetCaster() || GetCaster()->isDead())
                {
                    Remove();
                    return;
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tov_breath_of_dread_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tov_breath_of_dread_AuraScript();
        }
};

/// Bifrost Aura activation - 45
class eventobject_the_ephemeral_way_gate_to_odyn : public EventObjectScript
{
    public:
        eventobject_the_ephemeral_way_gate_to_odyn() : EventObjectScript("eventobject_the_ephemeral_way_gate_to_odyn") { }

        bool OnTrigger(Player* p_Player, EventObject* /*p_EventObject*/) override
        {
            if (p_Player->m_positionX >= 3165.f || p_Player->m_positionX <= 2600.f)
                return false;

            if (!p_Player->HasAura(eTovSpells::SpellBelfrostAura))
                p_Player->CastSpell(p_Player, eTovSpells::SpellBelfrostAura, true);

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_trial_of_valor()
{
    /// Creatures
    new npc_tov_worthy_vyrkul();
    new npc_tov_stormforged_sentinal();
    new npc_tov_bonespeaker_soulbinder();
    new npc_tov_hellhound();
    new npc_tov_risen_bonethrall();
    new npc_tov_kvaldir_spirittender();
    new npc_tov_dark_seraph();
    new npc_tov_deepbrine_monstruosity();
    new npc_tov_kvaldir_reefcaller();
    new npc_tov_cosmetic_odyn();
    new npc_tov_cosmetic_hyrja();
    new npc_tov_cosmetic_eyir();
    new npc_tov_cosmetic_chosen_of_eyir();
    new npc_tov_cosmetic_stormforged_sentinel();
    new npc_tov_cosmetic_hymdall();

    /// Spell
    new spell_tov_unholy_reckoning();
    new spell_tov_breath_of_dread();

    /// Event Object Script
    new eventobject_the_ephemeral_way_gate_to_odyn();
}
#endif
