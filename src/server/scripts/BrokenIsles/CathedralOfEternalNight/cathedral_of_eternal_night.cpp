#include "cathedral_of_eternal_night.hpp"

/// Maiev Shadowsong - 119147
class npc_coen_maiev_shadowsong : public CreatureScript
{
    public:
        npc_coen_maiev_shadowsong() : CreatureScript("npc_coen_maiev_shadowsong")
        {}

        enum eSpells
        {
            SpellVanish     = 240727,
        };

        enum eTalks
        {
            TalkAfterAgronox    = 6,
        };

        struct npc_coen_maiev_shadowsong_AI : public ScriptedAI
        {
            explicit npc_coen_maiev_shadowsong_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void DamageTaken(Unit* /**/, uint32 & p_Damage, SpellInfo const* /**/) override
            {
                p_Damage = 0;
            }

            void DamageDealt(Unit* /**/, uint32& p_Damage, DamageEffectType /**/, SpellInfo const* /**/) override
            {
                p_Damage = 0;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eSharedActions::ActionConversationIntro:
                    {
                        Talk(eSharedTalks::TalkIntroConversation);
                        me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointIntroConversation, g_MaievFirstPath.data(), g_MaievFirstPath.size(), false);
                        break;
                    }

                    case eSharedActions::ActionUpToScornful:
                    {
                        while (Unit* l_Victim = me->getVictim())
                            me->Kill(l_Victim);

                        AddTimedDelayedOperation(IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eSharedTalks::TalkIntroConversation);
                            me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointScornfulRoom, g_MaievStairsScornful.data(), g_MaievStairsScornful.size(), false);
                        });

                        break;
                    }

                    case eSharedActions::ActionUpToKhadgar:
                    {
                        Talk(eSharedTalks::TalkUpToKhadgar);
                        me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointKhadgarIntro, g_MaievLastPath.data(), g_MaievLastPath.size(), false);
                        break;
                    }

                    case eSharedActions::ActionMephistrothKilled:
                    {
                        me->SetVisible(true);
                        Position l_AegisRoomPos = { -553.0278f, 2520.760f, 534.026f };
                        me->GetMotionMaster()->MovePoint(eSharedPoints::PointAegisRoom, l_AegisRoomPos);
                        break;
                    }

                    default: break;
                }
            }

            void SpellHitTarget(Unit* /**/, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellVanish)
                    me->SetVisible(false);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case eSharedPoints::PointIntroConversation:
                    {
                        me->SetFacingTo(6.193173);

                        AddTimedDelayedOperation(16 * IN_MILLISECONDS, [this] () -> void
                        {
                            DelayTalk(2, eSharedTalks::TalkIllidanIntroMephistroth);
                            me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointEndIntro, g_MaievPathToMephistroth.data(), g_MaievPathToMephistroth.size(), false);
                        });
                        
                        break;
                    }

                    case eSharedPoints::PointEndIntro:
                    {
                        DoCast(me, eSpells::SpellVanish);
                        break;
                    }

                    case eSharedPoints::PointScornfulRoom:
                    {
                        me->DelayedCastSpell(me, eSpells::SpellVanish, true, IN_MILLISECONDS);
                        break;
                    }

                    case eSharedPoints::PointKhadgarIntro:
                    {
                        me->SetFacingTo(5.499035f);

                        if (InstanceScript* l_Instance = me->GetInstanceScript())
                        {
                            Unit* l_Khadgar = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::NpcImageOfKhadgar));

                            if (l_Khadgar != nullptr && l_Khadgar->IsAIEnabled)
                            {
                                me->SetFacingToObject(l_Khadgar);
                                l_Khadgar->GetAI()->DoAction(eSharedActions::ActionUpToKhadgar);
                                DelayTalk(30, eSharedTalks::TalkDefendStairs);
                            }

                        }

                        break;
                    }

                    case eSharedPoints::PointAegisRoom:
                    {
                        DelayTalk(2, eSharedTalks::TalkStartAegwynn);
                        me->SetFacingTo(0.401711f);
                        DelayTalk(90, eSharedTalks::TalkOutroAegwynn);
                        break;
                    }

                    default: break;
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
            return new npc_coen_maiev_shadowsong_AI(p_Me);
        }
};

/// Illidan Stormrage - 120792
class npc_coen_illidan_stormrage : public CreatureScript
{
    public:
        npc_coen_illidan_stormrage() : CreatureScript("npc_coen_illidan_stormrage")
        {}

        enum eSpells
        {
            SpellLeap                   = 240710,
            SpellIllidanSpawns          = 239484,
            SpellIllidanLeaves          = 239485,
            SpellExpelShadowsPowerBar   = 234875,
            SpellExpelShadowsAoe        = 215466,
        };

        enum eTalks
        {
            TalkFlyMephistrothRoom  = 1,
            TalkProtectAegis        = 6,
            TalkMephistrothShowed   = 7,
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->ADD_GOSSIP_ITEM_DB(21173, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            if (p_Action != (GOSSIP_ACTION_INFO_DEF + 1))
                return false;

            p_Player->TeleportTo(eLocations::LocExit);

            return true;
        }

        struct npc_coen_illidan_stormrage_AI : public ScriptedAI
        {
            explicit npc_coen_illidan_stormrage_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_MephistrothCombat = false;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eSharedActions::ActionConversationIntro:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointIntroConversation, g_IllidanFirstPath.data(), g_IllidanFirstPath.size(), false);
                        break;
                    }

                    case eSharedActions::ActionUpToKhadgar:
                    {
                        DelayTalk(10, eSharedTalks::TalkUpToKhadgar);
                        me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointKhadgarIntro, g_IllidanPathToKhadgar.data(), g_IllidanPathToKhadgar.size(), false);
                        break;
                    }

                    case eSharedActions::ActionMephistrothAppear:
                    {
                        if (m_MephistrothCombat)
                        {
                            AddTimedDelayedOperation(3100, [this]() -> void
                            {
                                PrepareExpelShadows();
                            });
                        }
                        else
                        {
                            m_MephistrothCombat = true;
                            me->CastSpell(g_AegisOfAggramarPos, eSpells::SpellLeap, true);
                        }

                        break;
                    }

                    case eSharedActions::ActionDomatraxKilled:
                    {
                        me->SetVisible(true);
                        me->GetMotionMaster()->MovePoint(eSharedPoints::PointAegisRoom, g_AegisOfAggramarPos);
                        break;
                    }
                    
                    case eSharedActions::ActionMephistrothKilled:
                    {
                        InstanceScript* l_Instance = me->GetInstanceScript();
                        me->SetVisible(true);

                        if (l_Instance != nullptr)
                            l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                        me->RemoveAllAuras();
                        me->CastStop();
                        Position l_Pos = { -547.947f, 2532.030f, 534.026f };
                        me->NearTeleportTo(l_Pos);

                        AddTimedDelayedOperation(6 * IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            Talk(eSharedTalks::TalkStartAegwynn);
                            me->SetFacingTo(4.874665f);
                            DelayTalk(90, eSharedTalks::TalkOutroAegwynn);
                        });

                        break;
                    }
                }

            }

            void SetData(uint32 p_Id, uint32 p_State) override
            {
                if (p_Id == eCreatures::BossMephistroth)
                {
                    if (p_State == EncounterState::FAIL)
                    {
                        m_MephistrothCombat = false;
                        me->RemoveAllAuras();
                        DoCast(me, eSpells::SpellIllidanLeaves, true);

                        InstanceScript* l_Instance = me->GetInstanceScript();

                        if (l_Instance != nullptr)
                            l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    }
                    
                }
            }

            void PrepareExpelShadows()
            {
                me->SetVisible(true);
                Talk(eTalks::TalkProtectAegis);
                me->CastStop();
                me->RemoveAllAuras();
                me->SetPower(Powers::POWER_ALTERNATE_POWER, 0);
                me->DelayedCastSpell(me, eSpells::SpellExpelShadowsPowerBar, true, 6000);

                InstanceScript* l_Instance = me->GetInstanceScript();

                if (l_Instance != nullptr)
                    l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                switch (p_Id)
                {
                    case eSharedPoints::PointIntroConversation:
                    {
                        me->SetFacingTo(6.193173);
                        AddTimedDelayedOperation(8 * IN_MILLISECONDS, [this] () -> void
                        {
                            Talk(eSharedTalks::TalkIntroConversation);
                            me->GetMotionMaster()->MoveSmoothPath(eSharedPoints::PointEndIntro, g_IllidanPathToMephistroth.data(), g_IllidanPathToMephistroth.size(), false);
                        });

                        break;
                    }

                    case eSharedPoints::PointEndIntro:
                    {
                        DoCast(me, eSpells::SpellLeap);
                        break;
                    }

                    case eSharedPoints::PointKhadgarIntro:
                    {
                        me->SetFacingTo(0.770937f);
                        DelayTalk(20, eSharedTalks::TalkDefendStairs);
                        break;
                    }

                    case eSharedPoints::PointAegisRoom:
                    {
                        Talk(eTalks::TalkFlyMephistrothRoom);
                        me->DelayedCastSpell(me, eSpells::SpellIllidanLeaves, false, 3500);
                        break;
                    }

                    case eSpells::SpellLeap:
                    {
                        if (m_MephistrothCombat)
                            PrepareExpelShadows();
                        
                        break;
                    }

                    case eSpells::SpellExpelShadowsAoe:
                    {
                        Talk(eTalks::TalkMephistrothShowed);

                        InstanceScript* l_Instance = me->GetInstanceScript();

                        if (l_Instance != nullptr)
                            l_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                        break;
                    }

                    default: break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellLeap && !m_MephistrothCombat)
                    me->SetVisible(false);
                else if (p_Spell->Id == eSpells::SpellExpelShadowsAoe)
                {
                    Talk(eTalks::TalkMephistrothShowed);
                    me->SetVisible(false);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                ScriptedAI::UpdateAI(p_Diff);
            }

            private:
                bool m_MephistrothCombat;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_illidan_stormrage_AI(p_Me);
        }
};

/// Image of Khadgar - 118797
class npc_coen_image_of_khadgar : public CreatureScript
{
    public:
        npc_coen_image_of_khadgar() : CreatureScript("npc_coen_image_of_khadgar")
        {}

        enum eSpells
        {
            SpellSpawnKhadgarImage  = 243274,
        };

        struct npc_coen_image_of_khadgar_AI : public ScriptedAI
        {
            explicit npc_coen_image_of_khadgar_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetVisible(false);
                me->SetReactState(REACT_PASSIVE);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionUpToKhadgar)
                {
                    me->SetVisible(true);
                    me->CastSpell(me, eSpells::SpellSpawnKhadgarImage, true);
                    DelayTalk(6, eSharedTalks::TalkUpToKhadgar);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_image_of_khadgar_AI(p_Me);
        }
};

/// Image of Aegwynn - 121234
class npc_coen_image_of_aegwynn : public CreatureScript
{
    public:
        npc_coen_image_of_aegwynn() : CreatureScript("npc_coen_image_of_aegwynn")
        {}

        enum eTalks
        {
            TalkIntro,
            TalkMidIntro,
            TalkMid2Intro,
            TalkOutro,
        };

        enum eSpells
        {
            SpellSpawnKhadgarImage = 243274,
        };

        struct npc_coen_image_of_aegwynn_AI : public ScriptedAI
        {
            explicit npc_coen_image_of_aegwynn_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetVisible(false);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionMephistrothKilled)
                {
                    me->SetVisible(true);
                    me->DelayedCastSpell(me, eSpells::SpellSpawnKhadgarImage, true, 500);
                    DelayTalk(12, eTalks::TalkIntro);
                    DelayTalk(25, eTalks::TalkMidIntro);
                    DelayTalk(45, eTalks::TalkMid2Intro);
                    DelayTalk(70, eTalks::TalkOutro);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_image_of_aegwynn_AI(p_Me);
        }
};

/// Mephistroth Intro - 120793
class npc_coen_mephistroth_intro : public CreatureScript
{
    public:
        npc_coen_mephistroth_intro() : CreatureScript("npc_coen_mephistroth_intro")
        {}

        enum eSpells
        {
            SpellMephistrothLeap        = 187564,
            SpellMephistrothDissolve    = 240738,
        };

        struct npc_coen_mephistroth_intro_AI : public ScriptedAI
        {
            explicit npc_coen_mephistroth_intro_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSharedActions::ActionConversationIntro)
                {
                    Creature* l_Felguard = me->FindNearestCreature(eCreatures::NpcFelguardDestroyer, 50.f);
                    Creature* l_Wrathguard = me->FindNearestCreature(eCreatures::NpcWrathGuardInvader, 50.f);
                    Position l_JumpPos = { -573.682f, 2526.43f, 334.705f };

                    if (l_Felguard != nullptr)
                        l_Felguard->CastSpell(l_JumpPos, eSpells::SpellMephistrothLeap, true);

                    if (l_Wrathguard != nullptr)
                        l_Wrathguard->CastSpell(l_JumpPos, eSpells::SpellMephistrothLeap, true);

                    AddTimedDelayedOperation(3 * IN_MILLISECONDS, [this]() -> void
                    {
                        Position l_JumpPos = { -573.682f, 2526.43f, 334.705f };
                        me->CastSpell(l_JumpPos, eSpells::SpellMephistrothLeap, true);
                    });
                }
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellMephistrothDissolve)
                {
                    me->SetVisible(false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Id == eSpells::SpellMephistrothLeap)
                {
                    Talk(eSharedTalks::TalkIntroConversation);

                    AddTimedDelayedOperation(8 * IN_MILLISECONDS, [this]() -> void
                    {
                        me->CastSpell(me, eSpells::SpellMephistrothDissolve, false);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_mephistroth_intro_AI(p_Me);
        }
};

/// Felguard Destroyer - 105682
class npc_coen_felguard_destroyer : public CreatureScript
{
    public:
        npc_coen_felguard_destroyer() : CreatureScript("npc_coen_felguard_destroyer")
        {}

        enum eEvents
        {
            EventShadowWall = 1,
        };

        enum eSpells
        {
            SpellShadowWall = 241598,
            SpellMephistrothLeap = 187564,
        };

        struct npc_coen_felguard_destroyer_AI : public ScriptedAI
        {
            explicit npc_coen_felguard_destroyer_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* p_Target) override
            {
                if (!p_Target->IsPlayer())
                    return;

                events.ScheduleEvent(eEvents::EventShadowWall, urand(10, 30) * IN_MILLISECONDS);
            }

            void MovementInform(uint32 /**/, uint32 p_Id) override
            {
                if (p_Id == eSpells::SpellMephistrothLeap)
                {
                    AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                    {
                        Position l_Pos = { -632.351f, 2527.350f, 334.437f };
                        me->GetMotionMaster()->MovePoint(0, l_Pos);
                    });
                }
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventShadowWall:
                    {
                        DoCast(me, eSpells::SpellShadowWall);
                        events.ScheduleEvent(eEvents::EventShadowWall, urand(15, 30) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_felguard_destroyer_AI(p_Me);
        }
};

/// Wrathguard Invaders - 101946
class npc_coen_wrathguard_invader : public CreatureScript
{
    public:
        npc_coen_wrathguard_invader() : CreatureScript("npc_coen_wrathguard_invader")
        {}

        enum eEvents
        {
            EventFelStrike  = 1,
        };

        enum eSpells
        {
            SpellMephistrothLeap = 187564,
            SpellFelStrike  = 236737,
        };

        struct npc_coen_wrathguard_invader_AI : public ScriptedAI
        {
            explicit npc_coen_wrathguard_invader_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void MovementInform(uint32 /**/, uint32 p_Id) override
            {
                if (p_Id == eSpells::SpellMephistrothLeap)
                {
                    AddTimedDelayedOperation(2 * IN_MILLISECONDS, [this]() -> void
                    {
                        Position l_Pos = { -632.351f, 2527.350f, 334.437f };
                        me->GetMotionMaster()->MovePoint(0, l_Pos);
                    });
                }
            }

            void EnterCombat(Unit* p_Target) override
            {
                if (!p_Target->IsPlayer())
                    return;

                events.ScheduleEvent(eEvents::EventFelStrike, urand(10, 20) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventFelStrike:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellFelStrike);
                        
                        events.ScheduleEvent(eEvents::EventFelStrike, urand(10, 20) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_wrathguard_invader_AI(p_Me);
        }
};

/// Hellblaze Soulmender - 119923
class npc_coen_hellblaze_soul_mender : public CreatureScript
{
    public:
        npc_coen_hellblaze_soul_mender() : CreatureScript("npc_coen_hellblaze_soul_mender")
        {}

        enum eEvents
        {
            EventDemonicMending = 1,
            EventFelBolt,
        };

        enum eSpells
        {
            SpellDemonicMending = 238543,
            SpellFelBolt        = 237578,
        };

        struct npc_coen_hellblaze_soul_mender_AI : public ScriptedAI
        {
            explicit npc_coen_hellblaze_soul_mender_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void EnterCombat(Unit* p_Target) override
            {
                if (!p_Target->IsPlayer())
                    return;

                events.ScheduleEvent(eEvents::EventDemonicMending, urand(15, 25) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFelBolt, urand(8, 14) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventDemonicMending:
                    {
                        DoCastAOE(eSpells::SpellDemonicMending);
                        events.ScheduleEvent(eEvents::EventDemonicMending, urand(10, 25) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelBolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellFelBolt);

                        events.ScheduleEvent(eEvents::EventFelBolt, urand(8, 12) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_hellblaze_soul_mender_AI(p_Me);
        }
};

/// Hellblaze Felbringer - 120779
class npc_coen_hellblaze_felbringer : public CreatureScript
{
    public:

        npc_coen_hellblaze_felbringer() : CreatureScript("npc_coen_hellblaze_felbringer")
        {}

        enum eEvents
        {
            EventBurningCelerity    = 1,
            EventDevastatingSwipe,
        };

        enum eSpells
        {
            SpellBurningCelerity    = 237573,
            SpellDevastatingSwipe   = 237599
        };

        struct npc_coen_hellblaze_felbringer_AI : public ScriptedAI
        {
            explicit npc_coen_hellblaze_felbringer_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void EnterCombat(Unit* p_Target) override
            {
                if (!p_Target->IsPlayer())
                    return;

                events.ScheduleEvent(eEvents::EventBurningCelerity, urand(8, 30) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventDevastatingSwipe, urand(10, 14) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                me->SetReactState(REACT_AGGRESSIVE);

                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventBurningCelerity:
                    {
                        DoCast(me, eSpells::SpellBurningCelerity);
                        events.ScheduleEvent(eEvents::EventBurningCelerity, urand(15, 30) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventDevastatingSwipe:
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();
                        DoCastAOE(eSpells::SpellDevastatingSwipe);

                        AddTimedDelayedOperation(1600, [this]() -> void
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                        });

                        events.ScheduleEvent(eEvents::EventDevastatingSwipe, urand(10, 20) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_hellblaze_felbringer_AI(p_Me);
        }
};

/// Dreadwing - 119930
class npc_coen_dreadwing : public CreatureScript
{
    public:
        npc_coen_dreadwing() : CreatureScript("npc_coen_dreadwing")
        {}

        enum eEvents
        {
            EventShadowSwipe    = 1
        };

        enum eSpells
        {
            SpellShadowSwipe    = 239558
        };

        struct npc_coen_dreadwing_AI : public ScriptedAI
        {
            explicit npc_coen_dreadwing_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventShadowSwipe, urand(8, 15) * IN_MILLISECONDS);
            }

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void Reset() override
            {
                me->SetCanFly(true, true);
                me->SetDisableGravity(true);

                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventShadowSwipe:
                    {
                        DoCastVictim(eSpells::SpellShadowSwipe);
                        events.ScheduleEvent(eEvents::EventShadowSwipe, urand(8, 12) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_dreadwing_AI(p_Me);
        }
};

/// Felblight Stalkers - 118700
class npc_coen_felblight_stalker : public CreatureScript
{
    public:
        npc_coen_felblight_stalker() : CreatureScript("npc_coen_felblight_stalker")
        {}

        enum eEvents
        {
            EventShadowFangs    = 1
        };

        enum eSpells
        {
            SpellShadowFangs    = 236740,
            SpellDevourMagic    = 238583,
        };

        struct npc_coen_felblight_stalker_AI : public ScriptedAI
        {
            explicit npc_coen_felblight_stalker_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventShadowFangs, urand(8, 15) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellShadowFangs)
                    me->DelayedCastSpell(me, eSpells::SpellDevourMagic, false, IN_MILLISECONDS);
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventShadowFangs:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellShadowFangs);

                        events.ScheduleEvent(eEvents::EventShadowFangs, urand(8, 12) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_felblight_stalker_AI(p_Me);
        }
};

/// Dul'zak - 118704
class npc_coen_dulzak : public CreatureScript
{
    public:
        npc_coen_dulzak() : CreatureScript("npc_coen_dulzak")
        {}

        enum eEvents
        {
            EventShadowWave = 1
        };

        enum eSpells
        {
            SpellShadowWave = 238653
        };

        struct npc_coen_dulzak_AI : public ScriptedAI
        {
            explicit npc_coen_dulzak_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_HealthMod) override
            {
                p_HealthMod = 2.112f;
            }

            void EnterCombat(Unit* p_Target) override
            {
                if (!p_Target->IsPlayer())
                    return;

                Talk(0);
                events.ScheduleEvent(eEvents::EventShadowWave, urand(6, 16) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventShadowWave:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellShadowWave);

                        events.ScheduleEvent(eEvents::EventShadowWave, urand(12, 20) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_dulzak_AI(p_Me);
        }
};

/// Legionfall Soldier - 119768
class npc_coen_legionfall_soldier : public CreatureScript
{
    public:
        npc_coen_legionfall_soldier() : CreatureScript("npc_coen_legionfall_soldier")
        {}

        enum eSpells
        {
            SpellFeignDeath = 230246,
        };

        struct npc_coen_legionfall_soldier_AI : public ScriptedAI
        {
            explicit npc_coen_legionfall_soldier_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    me->CastSpell(me, eSpells::SpellFeignDeath, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_legionfall_soldier_AI(p_Me);
        }
};

/// Vileblark Walker - 121569
class npc_coen_vilebrark_walker : public CreatureScript
{
    public:
        npc_coen_vilebrark_walker() : CreatureScript("npc_coen_vilebrark_walker")
        {}

        enum eEvents
        {
            EventLumberingCrash = 1,
            EventVileRoot
        };

        enum eSpells
        {
            SpellLumberingCrash = 242760,
            SpellVileRoot       = 242772
        };

        struct npc_coen_vilebrark_walker_AI : public ScriptedAI
        {
            explicit npc_coen_vilebrark_walker_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventLumberingCrash, urand(6, 16) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventVileRoot, urand(10, 20) * IN_MILLISECONDS);
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 1.3451f;
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventLumberingCrash:
                    {
                        DoCastAOE(eSpells::SpellLumberingCrash);
                        events.ScheduleEvent(eEvents::EventLumberingCrash, urand(12, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventVileRoot:
                    {
                        DoCastAOE(eSpells::SpellVileRoot);
                        events.ScheduleEvent(eEvents::EventVileRoot, urand(10, 15) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_vilebrark_walker_AI(p_Me);
        }
};

/// Hellblaze Temptress - 120366
class npc_coen_hellblaze_temptress : public CreatureScript
{
public:
    npc_coen_hellblaze_temptress() : CreatureScript("npc_coen_hellblaze_temptress")
    {}

    enum eEvents
    {
        EventAlluringAroma = 1,
    };

    enum eSpells
    {
        SpellAlluringAroma  = 237391
    };

    enum eTalks
    {
        TalkAggro,
    };

    struct npc_coen_hellblaze_temptress_AI : public ScriptedAI
    {
        explicit npc_coen_hellblaze_temptress_AI(Creature* p_Me) : ScriptedAI(p_Me)
        {}

        void GetMythicHealthMod(float& p_ModHealth) override
        {
            p_ModHealth = 2.031f;
        }

        void EnterCombat(Unit* /**/) override
        {
            Talk(eTalks::TalkAggro);
            events.ScheduleEvent(eEvents::EventAlluringAroma, urand(15, 30) * IN_MILLISECONDS);
        }

        void Reset() override
        {
            events.Reset();

            if (IsMythic())
                me->UpdateStatsForLevel();
        }

        void JustDied(Unit* /**/) override
        {
            events.Reset();
        }

        void ExecuteEvent(uint32 p_EventId)
        {
            switch (p_EventId)
            {
                case eEvents::EventAlluringAroma:
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                        me->CastSpell(l_Target, eSpells::SpellAlluringAroma, false);

                    events.ScheduleEvent(eEvents::EventAlluringAroma, urand(12, 20) * IN_MILLISECONDS);
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

            while (uint32 l_EventId = events.ExecuteEvent())
                ExecuteEvent(l_EventId);

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Me) const override
    {
        return new npc_coen_hellblaze_temptress_AI(p_Me);
    }
};

/// Felborne Botanist - 120373
class npc_coen_felborne_botanist : public CreatureScript
{
    public:
        npc_coen_felborne_botanist() : CreatureScript("npc_coen_felborne_botanist")
        {}

        enum eEvents
        {
            EventBlisteringRain     = 1,
            EventFelRejuvenation,
            EventFelBolt,
        };

        enum eSpells
        {
            SpellBlisteringRain     = 237565,
            SpellFelRejuvenation    = 237558,
            SpellFelbolt            = 237578,
        };

        struct npc_coen_felborne_botanist_AI : public ScriptedAI
        {
            explicit npc_coen_felborne_botanist_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventFelBolt, urand(3, 5) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBlisteringRain, urand(8, 16) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFelRejuvenation, urand(20, 30) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventFelRejuvenation:
                    {
                        DoCast(me, eSpells::SpellFelRejuvenation);
                        events.ScheduleEvent(eEvents::EventFelRejuvenation, urand(20, 30) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventBlisteringRain:
                    {
                        DoCast(me, eSpells::SpellBlisteringRain);
                        events.ScheduleEvent(eEvents::EventBlisteringRain, urand(14, 30) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelBolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellFelbolt);

                        events.ScheduleEvent(eEvents::EventFelBolt, urand(4, 8) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
            
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_felborne_botanist_AI(p_Me);
        }
};

/// Stranglevine Lashers - 119977
class npc_coen_stranglevine_lasher : public CreatureScript
{
public:
    npc_coen_stranglevine_lasher() : CreatureScript("npc_coen_stranglevine_lasher")
    {}

    enum eEvents
    {
        EventChokingVines = 1
    };

    enum eSpells
    {
        SpellChokingVines = 238688,
    };

    enum eActions
    {
        ActionTargetReached = 1
    };

    struct npc_coen_stranglevine_lasher_AI : public ScriptedAI
    {
        explicit npc_coen_stranglevine_lasher_AI(Creature* p_Me) : ScriptedAI(p_Me)
        {}

        void GetMythicHealthMod(float& p_ModHealth) override
        {
            p_ModHealth = 2.031576f;
        }

        void EnterCombat(Unit* /**/) override
        {
            me->RemoveAllAuras();
            events.ScheduleEvent(eEvents::EventChokingVines, urand(6, 16) * IN_MILLISECONDS);
        }

        void Reset() override
        {
            events.Reset();

            if (IsMythic())
                me->UpdateStatsForLevel();
        }

        void JustDied(Unit* /**/) override
        {
            events.Reset();
        }

        void ExecuteEvent(uint32 p_EventId)
        {
            switch (p_EventId)
            {
                case eEvents::EventChokingVines:
                {
                    Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true, -eSpells::SpellChokingVines);

                    if (l_Target)
                        DoCast(l_Target, eSpells::SpellChokingVines);
                    else
                    {
                        l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true);

                        if (l_Target)
                            DoCast(l_Target, eSpells::SpellChokingVines);
                        else
                            events.RescheduleEvent(eEvents::EventChokingVines, 1 * IN_MILLISECONDS);
                    }


                    events.RescheduleEvent(eEvents::EventChokingVines, urand(15, 20) * IN_MILLISECONDS);
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

            while (uint32 l_EventId = events.ExecuteEvent())
                ExecuteEvent(l_EventId);

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* p_Me) const override
    {
        return new npc_coen_stranglevine_lasher_AI(p_Me);
    }
};

/// Gazerax - 118723
class npc_coen_gazerax : public CreatureScript
{
    public:
        npc_coen_gazerax() : CreatureScript("npc_coen_gazerax")
        {}

        enum eEvents
        {
            EventBlindingGare   = 1,
            EventFelGlare,
            EventFocusedDestruction,
        };

        enum eSpells
        {
            SpellBlindingGare   = 239232,
            SpellFelGlare       = 224982,
            SpellFocusedDestruction = 239235
        };

        enum ePoints
        {
            PointGazeraxPath = 80,
        };

        struct npc_coen_gazerax_AI : public ScriptedAI
        {
            explicit npc_coen_gazerax_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                events.Reset();
                ClearDelayedOperations();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void EnterCombat(Unit* /**/) override
            {
                ClearDelayedOperations();

                events.ScheduleEvent(eEvents::EventBlindingGare, urand(8, 16) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFelGlare, urand(20, 30) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFocusedDestruction, urand(40, 50) * IN_MILLISECONDS);
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
                ClearDelayedOperations();
                me->GetMotionMaster()->Clear();
                me->StopMoving();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventFocusedDestruction:
                    {
                        DoCast(me, eSpells::SpellFocusedDestruction, false);
                        events.ScheduleEvent(eEvents::EventFocusedDestruction, urand(40, 50) * IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBlindingGare:
                    {
                        DoCast(me, eSpells::SpellBlindingGare);
                        events.ScheduleEvent(eEvents::EventBlindingGare, urand(20, 30) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelGlare:
                    {
                        DoCast(me, eSpells::SpellFelGlare);
                        events.ScheduleEvent(eEvents::EventFelGlare, urand(14, 30) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                {
                    ExecuteEvent(l_EventId);

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_gazerax_AI(p_Me);
        }
};

/// Fel Portal Imp Dummy - 120373
class npc_coen_fel_portal_imp : public CreatureScript
{
    public:
        npc_coen_fel_portal_imp() : CreatureScript("npc_coen_fel_portal_imp")
        {}

        struct npc_coen_fel_portal_imp_AI : public ScriptedAI
        {
            explicit npc_coen_fel_portal_imp_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void DoAction(int32 const p_Action)
            {
                if (p_Action == eSharedActions::ActionScornfulKilled)
                {
                    std::list<Creature*> l_Imps;

                    me->GetCreatureListWithEntryInGridAppend(l_Imps, eCreatures::NpcHellblazeImpTrash, 100.f);
                    me->GetCreatureListWithEntryInGridAppend(l_Imps, eCreatures::NpcHellblazeImpTrash2, 100.f);
                    me->GetCreatureListWithEntryInGridAppend(l_Imps, eCreatures::NpcHellblazeImpTrash, 100.f);

                    if (!l_Imps.empty())
                    {
                        Creature*& l_Imp = l_Imps.front();

                        if (l_Imp != nullptr && l_Imp->IsAIEnabled)
                            l_Imp->AI()->Talk(eSharedTalks::TalkDefendStairs);
                    }

                    for (Creature* l_Itr : l_Imps)
                    {
                        if (l_Itr == nullptr || l_Itr->isDead())
                            continue;

                        l_Itr->GetMotionMaster()->MovePoint(eSharedPoints::PointImpPortal, me->GetPosition());
                    }

                }
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_fel_portal_imp_AI(p_Me);
        }
};

/// Hellblaze Imp Trash - 120371, 118717
class npc_coen_hellblaze_imp_trash : public CreatureScript
{
    public:
        npc_coen_hellblaze_imp_trash() : CreatureScript("npc_coen_hellblaze_imp_trash")
        {}

        enum eEvents
        {
            EventFelBolt    = 1,
        };

        enum eSpells
        {
            SpellFelBolt    = 229912,
            SpellFeignDeath  = 193723
        };

        struct npc_coen_hellblaze_imp_trash_AI : public ScriptedAI
        {
            explicit npc_coen_hellblaze_imp_trash_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type == POINT_MOTION_TYPE && p_Id == eSharedPoints::PointImpPortal)
                    me->DespawnOrUnsummon(IN_MILLISECONDS);
            }

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventFelBolt, urand(8, 10) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventFelBolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellFelBolt);

                        events.ScheduleEvent(eEvents::EventFelBolt, urand(8, 10) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_hellblaze_imp_trash_AI(p_Me);
        }
};

/// Wyrmtongue Scavenger - 118719
class npc_coen_wyrmtongue_scavenger : public CreatureScript
{
    public:
        npc_coen_wyrmtongue_scavenger() : CreatureScript("npc_coen_wyrmtongue_scavenger")
        {}

        enum eEvents
        {
            EventArcaneTome = 1,
            EventEternalWinter,
            EventEverlastingSilence,
        };

        enum eSpells
        {
            SpellArcaneTome  = 239101,
            SpellEternalWinterTome  = 239145,
            SpellEternalWinterTomeStun  = 239156,
            SpellEverlastingSilenceTome = 239162,
            SpellFeignDeath = 193723
        };

        struct npc_coen_wyrmtongue_scavenger_AI : public ScriptedAI
        {
            explicit npc_coen_wyrmtongue_scavenger_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventArcaneTome, urand(8, 10) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventEternalWinter, urand(15, 25) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventEverlastingSilence, urand(20, 40) * IN_MILLISECONDS);
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 2.031f;
            }

            void Reset() override
            {
                events.Reset();
                m_EntombedPlayers.clear();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                for (uint64 & l_Itr : m_EntombedPlayers)
                {
                    if (Unit* l_Target = sObjectAccessor->GetUnit(*me, l_Itr))
                        l_Target->RemoveAurasDueToSpell(eSpells::SpellEternalWinterTomeStun);
                }

                m_EntombedPlayers.clear();
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_Spell) override
            {
                if (p_Target == nullptr || p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellEternalWinterTomeStun)
                    m_EntombedPlayers.push_back(p_Target->GetGUID());
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventArcaneTome:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellArcaneTome);

                        events.ScheduleEvent(eEvents::EventArcaneTome, urand(10, 12) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventEternalWinter:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellEternalWinterTome);

                        events.ScheduleEvent(eEvents::EventEternalWinter, urand(15, 25) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventEverlastingSilence:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellEverlastingSilenceTome);

                        events.ScheduleEvent(eEvents::EventEverlastingSilence, urand(20, 30) * IN_MILLISECONDS);
                        break;
                    }

                    default : break;
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            private:
                std::vector<uint64> m_EntombedPlayers;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_wyrmtongue_scavenger_AI(p_Me);
        }
};

/// Felstrider Orbcaster - 112712
class npc_coen_felstrider_orbcaster : public CreatureScript
{
    public:
        npc_coen_felstrider_orbcaster() : CreatureScript("npc_coen_felstrider_orbcaster")
        {}

        enum eEvents
        {
            EventFelblazeOrb    = 1,
            EventBurningStrands,
            EventFelBolt
        };

        enum eSpells
        {
            SpellFelblazeOrb    = 239320,
            SpellFelBolt        = 237578,
            SpellBurningStrands = 239288
        };

        struct npc_coen_felstrider_orbcaster_AI : public ScriptedAI
        {
            explicit npc_coen_felstrider_orbcaster_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventFelBolt, urand(6, 12) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventFelblazeOrb, urand(12, 20) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBurningStrands, urand(20, 30) * IN_MILLISECONDS);
            }

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon == nullptr)
                    return;

                if (p_Summon->GetEntry() == eCreatures::NpcFelblazeOrb)
                {
                    Position l_Pos;
                    p_Summon->GetFirstCollisionPosition(l_Pos, 200.f, 0);
                    p_Summon->GetMotionMaster()->MovePoint(0, l_Pos);
                }
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventFelblazeOrb:
                    {
                        DoCast(me, eSpells::SpellFelblazeOrb);
                        events.ScheduleEvent(eEvents::EventFelblazeOrb, urand(10, 15) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventBurningStrands:
                    {
                        DoCastVictim(eSpells::SpellBurningStrands);
                        events.ScheduleEvent(eEvents::EventBurningStrands, urand(14, 27) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelBolt:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellFelBolt);

                        events.ScheduleEvent(eEvents::EventFelBolt, urand(8, 12) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_felstrider_orbcaster_AI(p_Me);
        }

};

/// Felstrider Enforcer - 118713
class npc_coen_felstrider_enforcer : public CreatureScript
{
    public:
        npc_coen_felstrider_enforcer() : CreatureScript("npc_coen_felstrider_enforcer")
        {}

        enum eEvents
        {
            EventSerratedCleave = 1
        };

        enum eSpells
        {
            SpellSerratedCleave = 236975
        };

        struct npc_coen_felstrider_enforcer_AI : public ScriptedAI
        {
            explicit npc_coen_felstrider_enforcer_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void GetMythicHealthMod(float& p_ModHealth) override
            {
                p_ModHealth = 2.031f;
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventSerratedCleave, urand(12, 20) * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                events.Reset();
            }

            void ExecuteEvent(uint32 p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventSerratedCleave:
                    {
                        DoCastAOE(eSpells::SpellSerratedCleave);
                        events.ScheduleEvent(eEvents::EventSerratedCleave, urand(10, 15) * IN_MILLISECONDS);
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

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_felstrider_enforcer_AI(p_Me);
        }
};

/// Felblaze Orb Dummy - 120110
class npc_coen_felblaze_orb : public CreatureScript
{
    public:
        npc_coen_felblaze_orb() : CreatureScript("npc_coen_felblaze_orb")
        {}

        enum eSpells
        {
            SpellFelblazeOrbAt  = 239323
        };

        struct npc_coen_felblaze_orb_AI : public ScriptedAI
        {
            explicit npc_coen_felblaze_orb_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->AddAura(eSpells::SpellFelblazeOrbAt, me);
            }

            void Reset() override
            {
                Position l_Pos;
                me->GetFirstCollisionPosition(l_Pos, 30.f, 0);
                me->GetMotionMaster()->MovePoint(10, l_Pos, true);
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type == POINT_MOTION_TYPE && p_Id == 10)
                {
                    Position l_Pos;
                    me->GetFirstCollisionPosition(l_Pos, 30.f, 0);
                    me->GetMotionMaster()->MovePoint(10, l_Pos, true);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_felblaze_orb_AI(p_Me);
        }
};

/// Nalasha - 118705
class npc_coen_nalasha : public CreatureScript
{
    public:
        npc_coen_nalasha() : CreatureScript("npc_coen_nalasha")
        {}

        enum eEvents
        {
            EventSpiderCall = 1,
            EventVanish,
            EventVenomStorm,
        };

        enum eSpells
        {
            SpellVanish     = 237395,
            SpellSpiderCall = 239052,
            SpellSpiderCallSummon   = 239053,
            SpellVenomStorm = 239266,
            SpellVenomStormMissile   = 239269,
            SpellVemonSpiderlingWebThread   = 218072,
        };
        

        struct npc_coen_nalasha_AI : public BossAI
        {
            explicit npc_coen_nalasha_AI(Creature* p_Me) : BossAI(p_Me, eData::DataNalasha)
            {}

            void GetMythicHealthMod(float& p_HealthMod) override
            {
                p_HealthMod = 2.112f;
            }

            void EnterCombat(Unit* /**/) override
            {
                events.ScheduleEvent(eEvents::EventVanish, urand(8, 12) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventSpiderCall, urand(14, 16) * IN_MILLISECONDS);
                _EnterCombat();
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellVanish)
                    events.ScheduleEvent(eEvents::EventVenomStorm, IN_MILLISECONDS);
                else if (p_Spell->Id == eSpells::SpellSpiderCall)
                {
                    for (uint8 l_Itr = 0; l_Itr < 3; ++l_Itr)
                    {
                        Position l_Pos;
                        me->GetNearPosition(l_Pos, frand(12.5f, 17.5f), frand(0, float(M_PI)));
                        me->SummonCreature(eCreatures::NpcVenomousSpiderLing, l_Pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10 * IN_MILLISECONDS);
                    }
                }
                else if (p_Spell->Id == eSpells::SpellVenomStorm)
                {
                    Position l_Pos;
                    float l_Angle = 0;

                    for (uint8 l_Itr = 0; l_Itr < 6; ++l_Itr, l_Angle += float(M_PI)/12.f)
                    {
                        me->GetNearPosition(l_Pos, 15.f, l_Angle);
                        me->CastSpell(l_Pos, eSpells::SpellVenomStormMissile, true);
                    }

                    for (uint8 l_Itr = 0; l_Itr < 6; ++l_Itr, l_Angle -= float(M_PI) / 12.f)
                    {
                        me->GetNearPosition(l_Pos, 15.f, l_Angle);
                        me->CastSpell(l_Pos, eSpells::SpellVenomStormMissile, true);
                    }
                }
            }

            void Reset() override
            {
                _Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void JustDied(Unit* /**/) override
            {
                _JustDied();
            }

            void ExecuteEvent(const uint32 p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventVanish:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellVanish);

                        events.ScheduleEvent(eEvents::EventVanish, urand(25, 35) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventSpiderCall:
                    {
                        DoCast(me, eSpells::SpellSpiderCall);
                        events.ScheduleEvent(eEvents::EventSpiderCall, urand(15, 20) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventVenomStorm:
                    {
                        me->CastSpell(me, eSpells::SpellVenomStorm, false);
                        break;
                    }

                    default: break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                BossAI::UpdateAI(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_coen_nalasha_AI(p_Me);
        }
};

/// Fel Strike - 236737
class spell_coen_fel_strike : public SpellScriptLoader
{
    public:
        spell_coen_fel_strike() : SpellScriptLoader("spell_coen_fel_strike")
        {}

        enum eSpells
        {
            SpellFelStrikeMissile = 240276
        };

        class spell_coen_fel_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_coen_fel_strike_SpellScript);

            void HandleDummy(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                Position l_Pos;

                Unit* l_Caster = GetCaster();

                l_Caster->GetNearPosition(l_Pos, 20.f, 0.f);

                G3D::Vector2 l_Src = { l_Caster->m_positionX, l_Caster->m_positionY };
                G3D::Vector2 l_Tgt = { l_Pos.m_positionX, l_Pos.m_positionY };
                G3D::Vector2 l_Dir = l_Tgt - l_Src;
                
                float l_Pct = 0.1f;

                for (uint8 l_Itr = 0; l_Itr < 10; ++l_Itr)
                {
                    l_Tgt = (l_Src + (l_Dir * l_Pct));

                    l_Pos.m_positionX = l_Tgt.x;
                    l_Pos.m_positionY = l_Tgt.y;
                    l_Pct += 0.2f;

                    l_Caster->CastSpell(l_Pos, eSpells::SpellFelStrikeMissile, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_coen_fel_strike_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_coen_fel_strike_SpellScript();
        }
};

/// Blinding Gare - 239217
class spell_coen_blinding_gare : public SpellScriptLoader
{
    public:
        spell_coen_blinding_gare() : SpellScriptLoader("spell_coen_blinding_gare")
        {}

        class spell_coen_blinding_gare_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_coen_blinding_gare_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (GetCaster() == nullptr || p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();

                p_Targets.remove_if([this, &l_Caster] (WorldObject*& p_Itr)
                {
                    return p_Itr->isInBack(l_Caster, float(M_PI));
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_coen_blinding_gare_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_coen_blinding_gare_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_coen_blinding_gare_SpellScript::HandleTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_coen_blinding_gare_SpellScript();
        }
};

/// Focused Destruction - 239236
class spell_coen_focused_destruction : public SpellScriptLoader
{
public:
    spell_coen_focused_destruction() : SpellScriptLoader("spell_coen_focused_destruction")
    {}

    enum eSpells
    {
        SpellFocusedDestructionMissile  = 239237
    };

    class spell_coen_focused_destruction_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_coen_focused_destruction_SpellScript);

        void HandleOnHit(SpellEffIndex /**/)
        {
            if (GetHitUnit() == nullptr || GetCaster() == nullptr)
                return;

            GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellFocusedDestructionMissile, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_coen_focused_destruction_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_coen_focused_destruction_SpellScript();
    }
};

/// Shadow Wave - 238653
class spell_coen_shadow_wave : public SpellScriptLoader
{
    public:
        spell_coen_shadow_wave() : SpellScriptLoader("spell_coen_shadow_wave")
        {}

        enum eSpells
        {
            SpellShadowWaveTrigger  = 238654
        };

        class spell_coen_shadow_wave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_coen_shadow_wave_SpellScript);

            void HandleOnCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                Position l_RightEdge, l_LeftEdge, l_Mid;

                l_Caster->GetFirstCollisionPosition(l_LeftEdge, 30, float(M_PI) / 12.f);
                l_Caster->GetFirstCollisionPosition(l_RightEdge, 30, -float(M_PI) / 12.f);
                l_Caster->GetFirstCollisionPosition(l_Mid, 30, 0);

                G3D::Vector3 l_Start        = l_Caster->GetPosition().AsVector3();
                G3D::Vector3 l_FinalMid     = l_Mid.AsVector3();
                G3D::Vector3 l_RightLimit   = l_RightEdge.AsVector3();
                G3D::Vector3 l_LeftLimit    = l_LeftEdge.AsVector3();

                for (uint8 l_Itr = 2; l_Itr < 10; ++l_Itr)
                {
                    G3D::Vector3 l_Dir = l_FinalMid - l_Start;
                    G3D::Vector3 l_Tgt = (l_Start + (l_Dir * (l_Itr / 10.f)));

                    l_Caster->CastSpell(l_Tgt, eSpells::SpellShadowWaveTrigger, true);
                }

                for (uint8 l_Itr = 2; l_Itr < 10; ++l_Itr)
                {
                    G3D::Vector3 l_Dir = l_RightLimit - l_Start;
                    G3D::Vector3 l_Tgt = (l_Start + (l_Dir * (l_Itr / 10.f)));

                    l_Caster->CastSpell(l_Tgt, eSpells::SpellShadowWaveTrigger, true);
                }

                for (uint8 l_Itr = 2; l_Itr < 10; ++l_Itr)
                {
                    G3D::Vector3 l_Dir = l_LeftLimit - l_Start;
                    G3D::Vector3 l_Tgt = (l_Start + (l_Dir * (l_Itr / 10.f)));

                    l_Caster->CastSpell(l_Tgt, eSpells::SpellShadowWaveTrigger, true);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_coen_shadow_wave_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript()  const override
        {
            return new spell_coen_shadow_wave_SpellScript();
        }
};

/// Demonic Mending - 238543
class spell_coen_demonic_mending : public SpellScriptLoader
{
    public:
        spell_coen_demonic_mending() : SpellScriptLoader("spell_coen_demonic_mending")
        {}

        class spell_demonic_mending_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_demonic_mending_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([] (WorldObject*& p_Itr) -> bool
                {
                    Unit* l_Target = p_Itr->ToUnit();
                    
                    return l_Target == nullptr || l_Target->GetCreatureType() != CreatureType::CREATURE_TYPE_DEMON;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_demonic_mending_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_demonic_mending_SpellScript();
        }
};

/// Book of Eternal Winter - 240384
class spell_coen_throw_frost_tome : public SpellScriptLoader
{
    public:
        spell_coen_throw_frost_tome() : SpellScriptLoader("spell_coen_throw_frost_tome")
        {}

        enum eSpells
        {
            SpellEternalWinterTomeStun = 239156,
        };

        class spell_coen_throw_frost_tome_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_coen_throw_frost_tome_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                if (!GetHitUnit()->HasAura(eSpells::SpellEternalWinterTomeStun))
                    GetCaster()->CastSpell(GetHitUnit(), eSpells::SpellEternalWinterTomeStun, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_coen_throw_frost_tome_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_coen_throw_frost_tome_SpellScript();
        }
};

/// Illidan, Maiev Khadgar Conversation
class eventobject_khadgar_conversation : public EventObjectScript
{
    public:
        eventobject_khadgar_conversation() : EventObjectScript("eventobject_khadgar_conversation")
        {}

        bool OnTrigger(Player* p_Target, EventObject* p_Trigger) override
        {
            if (p_Target == nullptr || p_Trigger == nullptr)
                return false;

            InstanceScript* l_Instance = p_Target->GetInstanceScript();

            if (l_Instance == nullptr)
                return false;

            if (l_Instance->GetData(eData::DataIllidanMaievKhadgar) == EncounterState::DONE)
                return true;

            Creature* l_Maiev = p_Target->FindNearestCreature(eCreatures::NpcMaievShadowsong, 25.f);
            Creature* l_Illidan = p_Target->FindNearestCreature(eCreatures::NpcIllidanStormrage, 25.f);

            if (l_Maiev != nullptr && l_Illidan != nullptr)
            {
                l_Instance->SetData(eData::DataIllidanMaievKhadgar, EncounterState::DONE);

                if (l_Maiev->IsAIEnabled)
                    l_Maiev->GetAI()->DoAction(eSharedActions::ActionUpToKhadgar);

                if (l_Illidan->IsAIEnabled)
                    l_Illidan->GetAI()->DoAction(eSharedActions::ActionUpToKhadgar);
            }

            return true;
        }
};

/// Intro Instance Event
class eventobject_intro_entrance : public EventObjectScript
{
    public:
        eventobject_intro_entrance() : EventObjectScript("eventobject_intro_entrance")
        {}

        bool OnTrigger(Player* p_Target, EventObject* /**/) override
        {
            if (p_Target == nullptr)
                return false;

            InstanceScript* l_Instance = p_Target->GetInstanceScript();

            if (l_Instance == nullptr)
                return false;

            if (l_Instance->GetData(eData::DataIllidanMaievIntro) == EncounterState::DONE)
                return true;

            Creature* l_Maiev = p_Target->FindNearestCreature(eCreatures::NpcMaievShadowsong, 25.f);
            Creature* l_Illidan = p_Target->FindNearestCreature(eCreatures::NpcIllidanStormrage, 25.f);
            Creature* l_Mephistroth = sObjectAccessor->GetCreature(*p_Target, l_Instance->GetData64(eCreatures::NpcMephistrothIntro));

            if (l_Maiev != nullptr && l_Illidan != nullptr && l_Mephistroth != nullptr)
            {
                l_Instance->SetData(eData::DataIllidanMaievIntro, EncounterState::DONE);

                if (l_Maiev->IsAIEnabled)
                    l_Maiev->GetAI()->DoAction(eSharedActions::ActionConversationIntro);

                if (l_Illidan->IsAIEnabled)
                    l_Illidan->GetAI()->DoAction(eSharedActions::ActionConversationIntro);

                if (l_Mephistroth->IsAIEnabled)
                    l_Mephistroth->GetAI()->DoAction(eSharedActions::ActionConversationIntro);
            }

            return true;
        }
};

/// Dul'zak Checker
class eventobject_dulzak_checker : public EventObjectScript
{
    public:
        eventobject_dulzak_checker() : EventObjectScript("eventobject_dulzak_checker")
        {}

        bool OnTrigger(Player* p_Target, EventObject* /**/)
        {
            if (p_Target == nullptr)
                return false;

            InstanceScript* l_Instance = p_Target->GetInstanceScript();

            if (l_Instance != nullptr)
            {
                Creature* l_Dulzak = sObjectAccessor->GetCreature(*p_Target, l_Instance->GetData64(eCreatures::NpcDulzak));

                if (l_Dulzak != nullptr && (l_Dulzak->isAlive() && !l_Dulzak->isInCombat()))
                {
                    l_Dulzak->GetMotionMaster()->MovePoint(0, *p_Target);
                    return true;
                }
            }

            return true;
        }
};

/// Maiev Stairs
class eventobject_maiev_agronox : public EventObjectScript
{
    public:
        eventobject_maiev_agronox() : EventObjectScript("eventobject_maiev_agronox")
        {}

        bool OnTrigger(Player* p_Target, EventObject* /**/) override
        {
            if (p_Target == nullptr)
                return false;

            InstanceScript* l_Instance = p_Target->GetInstanceScript();

            if (l_Instance == nullptr)
                return false;

            if (l_Instance->GetData(eData::DataIllidanMaievStairs) == EncounterState::DONE)
                return false;

            Creature* l_Maiev = p_Target->FindNearestCreature(eCreatures::NpcMaievShadowsongScornful, 100.f);

            if (l_Maiev != nullptr && l_Maiev->IsAIEnabled)
            {
                l_Maiev->GetAI()->DoAction(eSharedActions::ActionUpToScornful);
                l_Instance->SetData(eData::DataIllidanMaievStairs, EncounterState::DONE);
            }

            return true;
        }
};

/// Shadow Wall - 241958
class at_coen_shadow_wall : public AreaTriggerEntityScript
{
    public:
        at_coen_shadow_wall() : AreaTriggerEntityScript("at_coen_shadow_wall")
        {}

        enum eSpells
        {
            SpellShadowWallAura = 241937
        };

        void OnRemove(AreaTrigger* p_At, uint32 /**/) override
        {
            for (auto & l_Guid : m_Targets)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*p_At, l_Guid);

                if (l_Target == nullptr)
                    continue;

                l_Target->RemoveAurasDueToSpell(eSpells::SpellShadowWallAura);
            }
        }

        bool OnRemoveTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return true;

            if (p_Target->IsFriendlyTo(p_At->GetCaster()))
            {
                m_Targets.remove(p_Target->GetGUID());
                p_Target->RemoveAurasDueToSpell(eSpells::SpellShadowWallAura);
            }

            return true;
        }

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return true;

            if (p_Target->GetGUID() == p_At->GetTargetGuid())
                return true;

            if (p_Target->IsFriendlyTo(p_At->GetCaster()))
            {
                m_Targets.push_back(p_Target->GetGUID());
                p_Target->CastSpell(p_Target, eSpells::SpellShadowWallAura, true);
            }
            
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_coen_shadow_wall();
        }

    private:
        std::list<uint64> m_Targets;
};

/// Felblaze Orb - 239623
class at_coen_felblaze_orb : public AreaTriggerEntityScript
{
    public:
        at_coen_felblaze_orb() : AreaTriggerEntityScript("at_coen_felblaze_orb")
        {}

        enum eSpells
        {
            SpellFelblazeOrbDot = 239326
        };

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target != nullptr && p_Target->IsPlayer())
                p_Target->AddAura(eSpells::SpellFelblazeOrbDot, p_Target);

            return true;
        }

        bool OnRemoveTarget(AreaTrigger* /**/, Unit* p_Target) override
        {
            if (p_Target != nullptr && p_Target->IsPlayer())
                p_Target->RemoveAurasDueToSpell(eSpells::SpellFelblazeOrbDot);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_coen_felblaze_orb();
        }
};

#ifndef __clang_analyzer__
void AddSC_cathedral_of_eternal_night()
{
    /// Creatures
    new npc_coen_mephistroth_intro();
    new npc_coen_maiev_shadowsong();
    new npc_coen_illidan_stormrage();
    new npc_coen_image_of_khadgar();
    new npc_coen_image_of_aegwynn();
    new npc_coen_felguard_destroyer();
    new npc_coen_wrathguard_invader();
    new npc_coen_hellblaze_soul_mender();
    new npc_coen_hellblaze_felbringer();
    new npc_coen_legionfall_soldier();
    new npc_coen_dreadwing();
    new npc_coen_felblight_stalker();
    new npc_coen_dulzak();
    new npc_coen_stranglevine_lasher();
    new npc_coen_vilebrark_walker();
    new npc_coen_hellblaze_temptress();
    new npc_coen_felborne_botanist();
    new npc_coen_gazerax();
    new npc_coen_fel_portal_imp();
    new npc_coen_hellblaze_imp_trash();
    new npc_coen_wyrmtongue_scavenger();
    new npc_coen_felstrider_orbcaster();
    new npc_coen_felstrider_enforcer();
    new npc_coen_nalasha();
    new npc_coen_felblaze_orb();

    /// Spells
    new spell_coen_fel_strike();
    new spell_coen_focused_destruction();
    new spell_coen_blinding_gare();
    new spell_coen_shadow_wave();
    new spell_coen_demonic_mending();
    new spell_coen_throw_frost_tome();

    /// EventObjects
    new eventobject_intro_entrance();
    new eventobject_maiev_agronox();
    new eventobject_khadgar_conversation();
    new eventobject_dulzak_checker();

    /// AreaTriggers
    new at_coen_felblaze_orb();
    new at_coen_shadow_wall();
}
#endif
