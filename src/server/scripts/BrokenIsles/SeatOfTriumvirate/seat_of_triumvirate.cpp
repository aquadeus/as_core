////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2020 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "seat_of_triumvirate.hpp"

/// Shadowguard Subjugator - 124171
class npc_sot_shadowguard_subjugator : public CreatureScript
{
    public:
        npc_sot_shadowguard_subjugator() : CreatureScript("npc_sot_shadowguard_subjugator")
        {}

        enum eSpells
        {
            SpellNegatingBrand      = 246697,
            SpellSupressionField    = 246677,
            SpellVoidContainment    = 246922
        };

        enum eEvents
        {
            EventNegatingBrand  = 1,
            EventSupressionField,
        };

        struct npc_sot_shadowguard_subjugator_AI : public ScriptedAI
        {
            explicit npc_sot_shadowguard_subjugator_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_IsZuraalGuard = false;
            }

            void Reset() override
            {
                events.Reset();

                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    ActivateShieldOnZuraal();
                });
            }

            void ActivateShieldOnZuraal()
            {
                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                Unit* l_Boss = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::BossZuraalTheAscended));

                if (l_Boss != nullptr && l_Boss->GetDistance2d(me) <= 20.f)
                {
                    m_IsZuraalGuard = true;
                    DoCast(me, eSpells::SpellVoidContainment, true);
                }
            }

            uint32 GetData(uint32 p_Id) override
            {
                if (p_Id == eData::DataBossIntro)
                    return m_IsZuraalGuard ? 2 : 1;

                return 0;
            }

            void JustReachedHome() override
            {
                AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                {
                    ActivateShieldOnZuraal();
                });

                ScriptedAI::JustReachedHome();
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventNegatingBrand, urand(5, 8) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSupressionField, urand(18, 20) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventSupressionField:
                    {
                        DoCastAOE(eSpells::SpellSupressionField);
                        events.ScheduleEvent(eEvents::EventSupressionField, urand(18, 20) * IN_MILLISECONDS);
                        break;
                    }
                    
                    case eEvents::EventNegatingBrand:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellNegatingBrand);

                        events.ScheduleEvent(eEvents::EventNegatingBrand, urand(5, 7) * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
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

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            bool m_IsZuraalGuard;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_shadowguard_subjugator_AI(p_Me);
        }
};

/// Locus Walker Zuraal Outro - 123744
class npc_sot_locus_walker_zuraal : public CreatureScript
{
    public:
        npc_sot_locus_walker_zuraal() : CreatureScript("npc_sot_locus_walker_zuraal")
        {}

        struct npc_sot_locus_walker_zuraal_AI : public ScriptedAI
        {
            explicit npc_sot_locus_walker_zuraal_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionCreatureEvent)
                {
                    me->SetVisible(true);
                    m_Intro = false;

                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                        l_Instance->SetData(eCreatures::NpcLocusWalkerZuraal, EncounterState::IN_PROGRESS);
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who == nullptr)
                    return;

                if (!p_Who->IsPlayer() || p_Who->GetDistance2d(me) >= 5.f || m_Intro)
                    return;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    m_Intro = true;
                    if (l_Instance->GetData(eCreatures::NpcLocusWalkerZuraal) == EncounterState::IN_PROGRESS)
                        me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointWallToSaprish, g_FirstWalkerPath.data(), g_FirstWalkerPath.size(), false);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == eSharedPoints::PointWallToSaprish)
                {
                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                        l_Instance->SetData(me->GetEntry(), EncounterState::DONE);

                    me->SetFacingTo(1.805717f);
                    DelayTalk(10, eSharedTalks::TalkZuraalOutro);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                ScriptedAI::UpdateAI(p_Diff);
            }

            bool m_Intro;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_locus_walker_zuraal_AI(p_Me);
        }
};

/// Locus Walker Saprish Outro - 125840
class npc_sot_locus_walker_saprish : public CreatureScript
{
    public:
        npc_sot_locus_walker_saprish() : CreatureScript("npc_sot_locus_walker_saprish")
        {}

        enum ePoints
        {
            PointSaprishStaff   = 10,
        };

        enum eTalks
        {
            TalkSaprishOutro,
            TalkAfterStaff,
            TalkViceroy,
            TalkGoodBye,
        };

        enum eWeapons
        {
            SaprishStaff    = 124381,
        };

        struct npc_sot_locus_walker_saprish_AI : public ScriptedAI
        {
            explicit npc_sot_locus_walker_saprish_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /**/) override
            {
                AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this] ()-> void
                {
                    Creature* l_SaprishStaff = me->FindNearestCreature(eCreatures::NpcSaprishStaff, 150.f);

                    if (l_SaprishStaff != nullptr)
                    {
                        Talk(eTalks::TalkSaprishOutro);
                        me->GetMotionMaster()->MovePoint(ePoints::PointSaprishStaff, l_SaprishStaff->GetPosition());
                    }
                });
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_Id == ePoints::PointSaprishStaff)
                {
                    DelayTalk(5, eTalks::TalkAfterStaff);
                    
                    AddTimedDelayedOperation(IN_MILLISECONDS, [this] () -> void
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 0, eWeapons::SaprishStaff);

                        Creature* l_SaprishStaff = me->FindNearestCreature(eCreatures::NpcSaprishStaff, 150.f);

                        if (l_SaprishStaff != nullptr)
                            l_SaprishStaff->DespawnOrUnsummon(2000);
                    });

                    DelayTalk(15, eTalks::TalkViceroy);
                    DelayTalk(30, eTalks::TalkGoodBye);
                    me->DespawnOrUnsummon(70 * IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                ScriptedAI::UpdateAI(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_locus_walker_saprish_AI(p_Me);
        }
};

/// Locus Walker Nezhar Outro - 125099
class npc_sot_locus_walker_nezhar : public CreatureScript
{
    public:
        npc_sot_locus_walker_nezhar() : CreatureScript("npc_sot_locus_walker_nezhar")
        {}

        enum eTalks
        {
            TalkNezharOutro,
        };

        struct npc_sot_locus_walker_nezhar_AI : public ScriptedAI
        {
            explicit npc_sot_locus_walker_nezhar_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionCreatureEvent)
                {
                    me->SetVisible(true);
                    me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointLuraDoor, g_DoorPath.data(), g_DoorPath.size(), true);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == eSharedPoints::PointLuraDoor)
                {
                    DelayTalk(4, eTalks::TalkNezharOutro);
                    me->DelayedCastSpell(me, eSharedSpells::SpellOpenLuraDoor, true, 9000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_locus_walker_nezhar_AI(p_Me);
        }
};

/// Locus Walker Lura - 125872
class npc_sot_locus_walker_lura : public CreatureScript
{
public:
    npc_sot_locus_walker_lura() : CreatureScript("npc_sot_locus_walker_lura")
    {}

    enum eTalks
    {
        TalkLuraIntro,
        TalkLuraIntro2,
        TalkLuraOutro,
        TalkLuraOutro2,
        TalkLuraOutro3,
        TalkVoidSever,
    };

    struct npc_sot_locus_walker_lura_AI : public ScriptedAI
    {
        explicit npc_sot_locus_walker_lura_AI(Creature* p_Me) : ScriptedAI(p_Me)
        {}

        void DoAction(int32 const p_Action) override
        {
            if (p_Action == eSharedActions::ActionLuraIntro)
            {
                me->SetVisible(true);
                DelayTalk(14, eTalks::TalkLuraIntro);
                DelayTalk(30, eTalks::TalkLuraIntro2);
            }
            else if (p_Action == eSharedActions::ActionLuraOutro)
            {
                DelayTalk(2, eTalks::TalkLuraOutro);
                DelayTalk(25, eTalks::TalkLuraOutro2);
                DelayTalk(40, eTalks::TalkLuraOutro3);
            }
            else if (p_Action == eSharedActions::ActionVoidPortal)
                Talk(eTalks::TalkVoidSever);
        }
    };

    CreatureAI* GetAI(Creature* p_Me) const override
    {
        return new npc_sot_locus_walker_lura_AI(p_Me);
    }
};

/// Alleria Windrunner Zuraal Outro - 123743
class npc_sot_alleria_windrunner_zuraal : public CreatureScript
{
    public:
        npc_sot_alleria_windrunner_zuraal() : CreatureScript("npc_sot_alleria_windrunner_zuraal")
        {}

        struct npc_sot_alleria_windrunner_zuraal_AI : public ScriptedAI
        {
            explicit npc_sot_alleria_windrunner_zuraal_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_ZuraalPosEvent = false;
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who == nullptr || m_ZuraalPosEvent)
                    return;

                if (!p_Who->IsPlayer() || p_Who->GetDistance2d(me) >= 5.f)
                    return;

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetData(eCreatures::NpcAlleriaZuraal) == EncounterState::IN_PROGRESS)
                    {
                        m_ZuraalPosEvent = true;
                        Talk(eSharedTalks::TalkAlleriaZuraalOutro2);
                        me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointWallToSaprish, g_FirstAlleriaPath.data(), g_FirstAlleriaPath.size(), false);
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionCreatureEvent)
                {
                    me->SetVisible(true);
                    Talk(eSharedTalks::TalkZuraalOutro);

                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                        l_Instance->SetData(me->GetEntry(), EncounterState::IN_PROGRESS);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == eSharedPoints::PointWallToSaprish)
                {
                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                    {
                        l_Instance->SetData(me->GetEntry(), EncounterState::DONE);
                        
                        if (Creature* l_AlleriaWall = sObjectAccessor->GetCreature(*me, l_Instance->GetData64(eCreatures::NpcAlleriaSaprish)))
                        {
                            if (l_AlleriaWall->IsAIEnabled)
                                l_AlleriaWall->GetAI()->DoAction(eSharedActions::ActionCreatureEvent);

                            AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this] () -> void
                            {
                                me->SetVisible(false);
                            });
                        }
                    }

                    me->SetFacingTo(3.8119f);
                    DelayTalk(2, eSharedTalks::TalkDesactivateWall);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                ScriptedAI::UpdateAI(p_Diff);
            }

            private:
                bool m_ZuraalPosEvent;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_alleria_windrunner_zuraal_AI(p_Me);
        }
};

/// Alleria Windrunner Saprish - 125840
class npc_sot_alleria_windrunner_saprish : public CreatureScript
{
    public:
        npc_sot_alleria_windrunner_saprish() : CreatureScript("npc_sot_alleria_windrunner_saprish")
        {}

        enum eTalks
        {
            TalkOpenWall,
            TalkWallOpened,
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (InstanceScript* l_Instance = p_Creature->GetInstanceScript())
            {
                if (l_Instance->GetData(eCreatures::NpcAlleriaSaprish) == EncounterState::DONE)
                    return false;
            }

            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->ADD_GOSSIP_ITEM_DB(21383, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            if (p_Action != (GOSSIP_ACTION_INFO_DEF + 1))
                return false;

            InstanceScript* l_Instance = p_Creature->GetInstanceScript();

            if (l_Instance)
            {
                if (l_Instance->GetData(eCreatures::NpcAlleriaSaprish) == EncounterState::DONE)
                    return false;

                l_Instance->SetData(eCreatures::NpcAlleriaSaprish, EncounterState::DONE);

                uint64 l_WallGuid = l_Instance->GetData64(eGameObjects::GoSaprishEntranceDoor);
                uint64 l_SaprishGuid = l_Instance->GetData64(eCreatures::BossSaprish);

                l_Instance->HandleGameObject(l_WallGuid, true, nullptr);

                if (Creature* l_Saprish = sObjectAccessor->GetCreature(*p_Creature, l_SaprishGuid))
                {
                    if (l_Saprish->IsAIEnabled)
                        l_Saprish->GetAI()->DoAction(eSharedActions::ActionSaprishIntro);
                }
            }

            p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            if (p_Creature->IsAIEnabled)
                p_Creature->AI()->DelayTalk(5, eTalks::TalkWallOpened);

            return true;
        }

        struct npc_sot_alleria_windrunner_saprish_AI : public ScriptedAI
        {
            explicit npc_sot_alleria_windrunner_saprish_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eData::DataZuraal) == EncounterState::DONE)
                        me->SetVisible(true);
                    else
                        me->SetVisible(false);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionCreatureEvent)
                {
                    AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this] () -> void
                    {
                        me->SetVisible(true);
                        DelayTalk(3, eTalks::TalkOpenWall);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                ScriptedAI::UpdateAI(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_alleria_windrunner_saprish_AI(p_Me);
        }
};

/// Alleria Windrunner Nezhar - 123187
class npc_sot_alleria_windrunner_nezhar : public CreatureScript
{
    public:
        npc_sot_alleria_windrunner_nezhar() : CreatureScript("npc_sot_alleria_windrunner_nezhar")
        {}

        enum eTalks
        {
            TalkNezharOutro,
            TalkStartOpenDoor,
            TalkDoorOpened,
        };

        struct npc_sot_alleria_windrunner_nezhar_AI : public ScriptedAI
        {
            explicit npc_sot_alleria_windrunner_nezhar_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSharedSpells::SpellOpenLuraDoor)
                {
                    Talk(eTalks::TalkStartOpenDoor);

                    AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                    {
                        me->CastStop();
                        me->RemoveAllAuras();
                        Talk(eTalks::TalkDoorOpened);

                        InstanceScript* l_Instance = me->GetInstanceScript();

                        if (l_Instance != nullptr)
                        {
                            if (Unit* l_Walker = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::NpcLocusWalkerNezhar)))
                            {
                                l_Walker->CastStop();
                                l_Walker->RemoveAllAuras();

                                l_Walker->AddDelayedEvent([l_Walker]() -> void
                                {
                                    l_Walker->SetVisible(false);
                                }, 10 * IN_MILLISECONDS);
                            }

                            if (Unit* l_Stalker = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::NpcLuraDoorStalker)))
                                l_Stalker->RemoveAllAuras();

                            l_Instance->SetData(eCreatures::NpcLocusWalkerNezhar, EncounterState::DONE);
                            l_Instance->SetData(eCreatures::NpcAlleriaNezhar, EncounterState::DONE);
                            
                            uint64 l_LuraDoor = l_Instance->GetData64(eGameObjects::GoLuraEntranceDoor);

                            l_Instance->HandleGameObject(l_LuraDoor, true, nullptr);
                        }

                        AddTimedDelayedOperation(10 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetVisible(false);
                        });
                    });
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == eSharedPoints::PointLuraDoor)
                {
                    DelayTalk(5, eTalks::TalkNezharOutro);
                    me->DelayedCastSpell(me, eSharedSpells::SpellOpenLuraDoor, true, 12 * IN_MILLISECONDS);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionCreatureEvent)
                {
                    Creature* l_Portal = me->FindNearestCreature(eCreatures::NpcVoidPortalNezhar, 15.f);

                    if (l_Portal != nullptr)
                    {
                        l_Portal->SetVisible(true);

                        l_Portal->AddDelayedEvent([l_Portal]() -> void
                        {
                            l_Portal->SetVisible(false);
                        }, 2 * IN_MILLISECONDS);
                    }

                    me->SetVisible(true);
                    Position l_Pos = { 6098.911f, 10355.386f, 19.661f };

                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(eSharedPoints::PointLuraDoor, l_Pos, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                ScriptedAI::UpdateAI(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_alleria_windrunner_nezhar_AI(p_Me);
        }
};

/// Alleria Windrunner - 125871
class npc_sot_alleria_windrunner_lura : public CreatureScript
{
    public:
        npc_sot_alleria_windrunner_lura() : CreatureScript("npc_sot_alleria_windrunner_lura")
        {}

        enum eTalks
        {
            TalkLuraIntro,
            TalkLuraOutro,
            TalkLuraOutro2,
            TalkVoidSever,
            TalkCallOfTheVoid,
        };

        enum eEvents
        {
            EventCheckVoidPortals = 1,
        };

        struct npc_sot_alleria_windrunner_lura_AI : public ScriptedAI
        {
            explicit npc_sot_alleria_windrunner_lura_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                ScriptedAI::Reset();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eSharedActions::ActionCreatureEvent:
                    {
                        me->SetVisible(true);

                        Talk(eTalks::TalkLuraIntro);

                        InstanceScript* l_Instance = me->GetInstanceScript();

                        if (l_Instance != nullptr)
                            l_Instance->SetData(me->GetEntry(), EncounterState::IN_PROGRESS);

                        if (Creature* l_Walker = me->FindNearestCreature(eCreatures::NpcLocusWalkerLura, 50.f))
                        {
                            if (l_Walker->IsAIEnabled)
                                l_Walker->GetAI()->DoAction(eSharedActions::ActionLuraIntro);
                        }
                        break;
                    }

                    case eSharedActions::ActionLuraOutro:
                    {
                        me->CastSpell(me, eSharedSpells::SpellAlleriaVoidForm, true);
                        DelayTalk(12, eTalks::TalkLuraOutro);
                        DelayTalk(35, eTalks::TalkLuraOutro2);
                        break;
                    }

                    case eSharedActions::ActionVoidPortal:
                    {
                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            Unit* l_Boss = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::BossLura));
                            me->DelayedCastSpell(l_Boss, eSharedSpells::SpellDarkTorrent, false, 100);
                            DelayTalk(5, eTalks::TalkVoidSever);
                        }

                        break;
                    }

                    case eSharedActions::ActionCallOfTheVoid:
                    {
                        Talk(eTalks::TalkCallOfTheVoid);
                        me->DelayedCastSpell(me, eSharedSpells::SpellCheckUnboundRifts, true, 2 * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == eSharedPoints::PointWallToSaprish)
                {
                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                        l_Instance->SetData(me->GetEntry(), EncounterState::DONE);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_alleria_windrunner_lura_AI(p_Me);
        }
};

/// Void Portal Stalker Saprish PreEvent - 123767
class npc_sot_void_portal_stalker_saprish : public CreatureScript
{
    public:
        npc_sot_void_portal_stalker_saprish() : CreatureScript("npc_sot_void_portal_stalker_saprish")
        {}

        struct npc_sot_void_portal_stalker_saprish_AI : public ScriptedAI
        {
            explicit npc_sot_void_portal_stalker_saprish_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }

            void SpellHit(Unit* /**/, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSharedSpells::SpellAmplifyVoidTear)
                    me->DelayedCastSpell(me, eSharedSpells::SpellAmplifyVoidTearAOE, true, 30 * IN_MILLISECONDS);
            }

            void OnRemoveAura(uint32 p_SpellId, AuraRemoveMode /**/) override
            {
                if (p_SpellId == eSharedSpells::SpellAmplifyVoidTear)
                    me->m_Events.KillAllEvents(true);
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSharedSpells::SpellAmplifyVoidTearAOE)
                    me->DelayedCastSpell(me, eSharedSpells::SpellAmplifyVoidTearAOE, true, 30 * IN_MILLISECONDS);
            }

            void SendPortalQuote()
            {
                for (auto const & l_Itr : g_VoidTearsData)
                {
                    if (me->GetDistance2d(l_Itr.first.GetPositionX(), l_Itr.first.GetPositionY()) <= 5.f)
                    {
                        Talk(l_Itr.second);
                        break;
                    }
                }
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionSaprishPortalOpen)
                {
                    me->CastSpell(me, eSharedSpells::SpellVoidPortalVisual, true);
                    me->CastSpell(me, eSharedSpells::SpellVoidPortalAT, true);
                    me->CastSpell(me, eSharedSpells::SpellWaveRift, true);

                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                    {
                        Position l_Pos;
                        
                        me->GetNearestPosition(l_Pos, 14.f, 0.f);
                        
                        Creature* l_Warden = me->SummonCreature(eCreatures::NpcRiftWardenSaprish, l_Pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200 * IN_MILLISECONDS);

                        if (l_Warden != nullptr)
                        {
                            l_Warden->SetDynamicValue(UNIT_DYNAMIC_FIELD_WORLD_EFFECTS, 0, 2101);

                            if (l_Warden->IsAIEnabled)
                                l_Warden->GetAI()->SetGUID(me->GetGUID(), eCreatures::NpcVoidPortalStalker);

                            l_Warden->SetHomePosition(l_Pos);
                            l_Warden->CastSpell(me, eSharedSpells::SpellAmplifyVoidTear, true);
                            SendPortalQuote();
                        }
                    }
                }
                else if (p_Action == eSharedActions::ActionSaprishPortalClosed)
                {
                    me->RemoveAllAuras();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_void_portal_stalker_saprish_AI(p_Me);
        }
};

/// Rift Warden Saprish PreEvent - 122571
class npc_sot_rift_warden_saprish : public CreatureScript
{
    public:
        npc_sot_rift_warden_saprish() : CreatureScript("npc_sot_rift_warden_saprish")
        {}

        enum eEvents
        {
            EventDarkenedRemnant = 1,
            EventStygianBlast,
        };

        enum eSpells
        {
            SpellStygianBlast       = 248133,
            SpellDarkenedRemnant    = 248128
        };

        struct npc_sot_rift_warden_saprish_AI : public ScriptedAI
        {
            explicit npc_sot_rift_warden_saprish_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_PortalBoundedGuid = 0;
            }

            void SetGUID(uint64 p_Guid, int32 p_Id) override
            {
                if (p_Id == eCreatures::NpcVoidPortalStalker)
                    m_PortalBoundedGuid = p_Guid;
            }

            uint64 GetGUID(int32 p_Id) override
            {
                if (p_Id == eCreatures::NpcVoidPortalStalker)
                    return m_PortalBoundedGuid;

                return 0;
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat(me, 50.f);

                events.ScheduleEvent(eEvents::EventDarkenedRemnant, urand(5, 8) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventStygianBlast, urand(12, 16) * IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventStygianBlast:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellStygianBlast);

                        events.ScheduleEvent(eEvents::EventStygianBlast, urand(15, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDarkenedRemnant:
                    {
                        DoCastAOE(eSpells::SpellDarkenedRemnant);
                        events.ScheduleEvent(eEvents::EventDarkenedRemnant, urand(6, 8) * IN_MILLISECONDS);
                        break;
                    }

                    default: break;
                }
            }

            void JustReachedHome() override
            {
                ScriptedAI::JustReachedHome();

                Creature* l_PortalBounded = sObjectAccessor->GetCreature(*me, m_PortalBoundedGuid);

                if (l_PortalBounded != nullptr)
                    me->DelayedCastSpell(l_PortalBounded, eSharedSpells::SpellAmplifyVoidTear, true, IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();

                Creature* l_PortalBounded = sObjectAccessor->GetCreature(*me, m_PortalBoundedGuid);

                if (l_PortalBounded != nullptr)
                    l_PortalBounded->DespawnOrUnsummon();
            }

            uint64 m_PortalBoundedGuid;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sot_rift_warden_saprish_AI(p_Me);
        }
};

/// Void Containment - 246922
class spell_sot_void_containment : public SpellScriptLoader
{
    public:
        spell_sot_void_containment() : SpellScriptLoader("spell_sot_void_containment")
        {}

    class spell_sot_void_containment_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sot_void_containment_SpellScript);

        void HandleTargets(std::list<WorldObject*>& p_Targets)
        {
            if (p_Targets.empty())
                return;

            p_Targets.remove_if([] (WorldObject*& p_Itr)
            {
                return p_Itr->GetEntry() != eCreatures::BossZuraalTheAscended;
            });

            p_Targets.push_back(GetCaster());
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sot_void_containment_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sot_void_containment_SpellScript();
    }
};

/// Open Lura Door - 248461
class spell_sot_open_lura_door : public SpellScriptLoader
{
    public:
        spell_sot_open_lura_door() : SpellScriptLoader("spell_sot_open_lura_door")
        {}

        class spell_sot_open_lura_door_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sot_open_lura_door_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if(JadeCore::UnitEntryCheck(eCreatures::NpcLuraDoorStalker, 0, 0));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sot_open_lura_door_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sot_open_lura_door_SpellScript();
        }
};

/// Amplify Void Tear - 250182
class spell_sot_amplify_void_tear : public SpellScriptLoader
{
public:
    spell_sot_amplify_void_tear() : SpellScriptLoader("spell_sot_amplify_void_tear")
    {}

    class spell_sot_amplify_void_tear_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sot_amplify_void_tear_SpellScript);

        void HandleTargets(std::list<WorldObject*>& p_Targets)
        {
            if (p_Targets.empty())
                return;

            p_Targets.remove_if([](WorldObject*& p_Itr)
            {
                return p_Itr->GetEntry() != eCreatures::NpcVoidPortalStalker;
            });

            //p_Targets.push_back(GetCaster());
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sot_amplify_void_tear_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_sot_amplify_void_tear_SpellScript();
    }
};

/// Wave Rift AOE - 246860
class spell_sot_wave_rift_aoe : public SpellScriptLoader
{
    public:
        spell_sot_wave_rift_aoe() : SpellScriptLoader("spell_sot_wave_rift_aoe")
        {}

        class spell_sot_wave_rift_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sot_wave_rift_aoe_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Itr) -> bool
                {
                    return p_Itr->GetEntry() != eCreatures::NpcWaveRift;
                });

                Unit* l_Caster = GetCaster();

                std::list<Creature*> l_Targets;

                l_Caster->GetCreatureListWithEntryInGrid(l_Targets, eCreatures::NpcWaveRift, 20.f);

                for (Creature* l_Itr : l_Targets)
                {
                    if (l_Itr == nullptr)
                        continue;

                    p_Targets.push_back(l_Itr);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sot_wave_rift_aoe_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sot_wave_rift_aoe_SpellScript();
        }
};

/// L'ura Trash Event
class eventobject_lura_trash_event : public EventObjectScript
{
    public:
        eventobject_lura_trash_event() : EventObjectScript("eventobject_lura_trash_event")
        {}

        bool OnTrigger(Player* p_Target, EventObject* /**/) override
        {
            if (p_Target == nullptr)
                return false;

            InstanceScript* l_Instance = p_Target->GetInstanceScript();

            if (l_Instance == nullptr)
                return false;
            
            if (l_Instance->GetData(eCreatures::NpcLuraDoorStalker) != EncounterState::DONE)
            {
                l_Instance->SetData(eCreatures::NpcLuraDoorStalker, EncounterState::DONE);
                std::list<Creature*> l_Targets;

                p_Target->GetCreatureListWithEntryInGrid(l_Targets, eCreatures::NpcLashingVoidling, 200.f);

                for (Creature* l_Itr : l_Targets)
                    l_Itr->GetMotionMaster()->MovePoint(eSharedPoints::PointLuraDoor, g_EndLuraTrashPos);
            }

            return true;
        }
};

// 250820
class spell_sott_growing_pressure : public AuraScript
{
    PrepareAuraScript(spell_sott_growing_pressure);

    void OnTick(AuraEffect const* p_AurEff)
    {
        if (auto aura = GetAura())
        {
            if (aura->GetStackAmount() >= 10)
            {
                if (auto target = GetTarget())
                {
                    target->CastSpell(target, SpellCrushingDarkness, true);
                    target->RemoveAura(SpellGrowingPressure);
                }
            }
        }  
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sott_growing_pressure::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

#ifndef __clang_analyzer__
void AddSC_seat_of_triumvirate()
{
    RegisterAuraScript(spell_sott_growing_pressure);
    /// Creatures
    new npc_sot_shadowguard_subjugator();
    new npc_sot_alleria_windrunner_zuraal();
    new npc_sot_alleria_windrunner_saprish();
    new npc_sot_alleria_windrunner_nezhar();
    new npc_sot_alleria_windrunner_lura();
    new npc_sot_locus_walker_zuraal();
    new npc_sot_locus_walker_saprish();
    new npc_sot_locus_walker_nezhar();
    new npc_sot_locus_walker_lura();
    new npc_sot_void_portal_stalker_saprish();
    new npc_sot_rift_warden_saprish();

    /// Spells
    new spell_sot_amplify_void_tear();
    new spell_sot_wave_rift_aoe();
    new spell_sot_void_containment();
    new spell_sot_open_lura_door();

    new eventobject_lura_trash_event();
}
#endif
