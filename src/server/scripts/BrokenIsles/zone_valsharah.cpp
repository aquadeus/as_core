////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "zone_valsharah.h"

/// Archmage Khadgar - 91172
class npc_archmage_khadgar_valsharah : public CreatureScript
{
    public:
        npc_archmage_khadgar_valsharah() : CreatureScript("npc_archmage_khadgar_valsharah") { }

        enum eQuests
        {
            TyingUpLooseEnds    = 39861
        };

        enum eCreatures
        {
            Hippogryph          = 267676,
            QuestCredit         = 97545
        };

        enum eSpells
        {
            MountUpHippogryph   = 59119
        };

        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, const Quest* p_Quest) override
        {
            if (p_Quest->GetQuestId() == eQuests::TyingUpLooseEnds)
            {
                if (!p_Quest || !p_Creature || !p_Player)
                    return false;

                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->PlayerTalkClass->SendCloseGossip();

                Creature* l_Hippogryph = p_Creature->SummonCreature(eCreatures::Hippogryph, p_Player->GetPosition(), TEMPSUMMON_CORPSE_DESPAWN);
                p_Player->KilledMonsterCredit(eCreatures::QuestCredit);
                p_Player->CastSpell(l_Hippogryph, eSpells::MountUpHippogryph, true);

                if (l_Hippogryph->IsAIEnabled)
                    l_Hippogryph->AI()->DoAction(1);
            }

            return true;
        }
};

/// Creature Malfurion - 91109
class npc_malfurion_grove : public CreatureScript
{
    public:
        npc_malfurion_grove() : CreatureScript("npc_malfurion_grove") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (p_Player->GetQuestStatus(ValsharahQuests::eQuests::Quest_Emerald_Queen) == QUEST_STATUS_INCOMPLETE)
            {
                p_Player->ADD_GOSSIP_ITEM_DB(ValsharahGossips::eGossips::Gossip_Malfurion_Grove, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                p_Player->SEND_GOSSIP_MENU(911090, p_Creature->GetGUID());
                return true;
            }

            return false;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->PlayerTalkClass->SendCloseGossip();

            if (p_Action == GOSSIP_ACTION_INFO_DEF)
            {
                p_Player->PlayStandaloneScene(ValsharahScenes::eScenes::Scene_Summon_Ysera, 17, { 2612.87f, 6750.05f, 107.49f, 1.56f });
                p_Player->CompleteQuest(ValsharahQuests::eQuests::Quest_Emerald_Queen);
            }

            return true;
        }
};

/// Creature Tyrande CUSTOM - 993559
class npc_tyrande_waypoint : public CreatureScript
{
    public:
        npc_tyrande_waypoint() : CreatureScript("npc_tyrande_waypoint") { }

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_tyrande_waypointAI(p_Creature);
        }

        struct npc_tyrande_waypointAI : public ScriptedAI
        {
            npc_tyrande_waypointAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                events.ScheduleEvent(ValsharahEvents::eEvents::Event_Tyrande_Starsong_Ride, 1000);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 4:
                    {
                        me->ForcedDespawn();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case ValsharahEvents::eEvents::Event_Tyrande_Starsong_Ride:
                    {
                        me->GetMotionMaster()->MoveSmoothPath(ValsharahMoves::eMoves::Move_Tyrande_Starsong, ValsharahWaypoints::g_Path_Tyrande_Starsong.data(), ValsharahWaypoints::g_Path_Tyrande_Starsong.size(), false);
                        break;
                    }
                    default:
                        break;
                }
            }
        };
};

/// Creature Ysera - 100602 CUSTOM
class npc_ysera_grove : public CreatureScript
{
    public:
        npc_ysera_grove() : CreatureScript("npc_ysera_grove") { }

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_ysera_groveAI(p_Creature);
        }

        struct npc_ysera_groveAI : public ScriptedAI
        {
            npc_ysera_groveAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                events.ScheduleEvent(ValsharahEvents::eEvents::Event_Ysera_Talk, 6000);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case ValsharahEvents::eEvents::Event_Ysera_Talk:
                    {
                        Talk(3);
                        events.ScheduleEvent(ValsharahEvents::eEvents::Event_Ysera_Talk2, 6000);
                        break;
                    }
                    case ValsharahEvents::eEvents::Event_Ysera_Talk2:
                    {
                        Talk(5);
                        events.ScheduleEvent(ValsharahEvents::eEvents::Event_Ysera_Talk3, 9000);
                        break;
                    }
                    case ValsharahEvents::eEvents::Event_Ysera_Talk3:
                    {
                        Talk(4);
                        events.ScheduleEvent(ValsharahEvents::eEvents::Event_Ysera_Die, 6000);
                        break;
                    }
                    case ValsharahEvents::eEvents::Event_Ysera_Die:
                    {
                        me->ForcedDespawn();
                        break;
                    }

                    default:
                        break;
                }
            }
        };
};

/// Creature Hippogryph - 267676 CUSTOM
class npc_hippogryph_valsharah : public CreatureScript
{
    public:
        npc_hippogryph_valsharah() : CreatureScript("npc_hippogryph_valsharah") { }

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_hippogryph_valsharahAI(p_Creature);
        }

        struct npc_hippogryph_valsharahAI : public ScriptedAI
        {
            npc_hippogryph_valsharahAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 5:
                    {
                        me->ForcedDespawn();
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case 1:
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothFlyPath(ValsharahMoves::eMoves::Move_Hippogryph, ValsharahWaypoints::g_Path_Valsharah_Intro.data(), ValsharahWaypoints::g_Path_Valsharah_Intro.size());
                        break;
                    default:
                        break;
                }
            }
        };
};

/// Creature Ysera - 100906 CUSTOM
class npc_ysera_quest : public CreatureScript
{
    public:
        npc_ysera_quest() : CreatureScript("npc_ysera_quest") { }

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_ysera_questAI (p_Creature);
        }

        struct npc_ysera_questAI : public ScriptedAI
        {
            npc_ysera_questAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                events.ScheduleEvent(ValsharahEvents::eEvents::Event_Mount, 300);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 1:
                    {
                        me->ForcedDespawn();
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case ValsharahEvents::eEvents::Event_Mount:
                    {
                        Player* l_Player = me->SelectNearestPlayer(5.0f);

                        if (l_Player == nullptr)
                            return;

                        l_Player->CastSpell(me, 59119, true);
                        events.ScheduleEvent(ValsharahEvents::eEvents::Event_Flight, 600);
                        break;
                    }

                    case ValsharahEvents::eEvents::Event_Flight:
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothFlyPath(ValsharahMoves::eMoves::Move_Ysera, ValsharahWaypoints::g_Path_Ysera.data(), ValsharahWaypoints::g_Path_Ysera.size());
                        events.ScheduleEvent(ValsharahEvents::eEvents::Event_Fly_Talk_1, 3000);
                        break;
                    }

                    case ValsharahEvents::eEvents::Event_Fly_Talk_1:
                    {
                        Talk(0);
                        events.ScheduleEvent(ValsharahEvents::eEvents::Event_Fly_Talk_2, 10000);
                        break;
                    }

                    case ValsharahEvents::eEvents::Event_Fly_Talk_2:
                    {
                        Talk(1);
                        break;
                    }

                    default:
                        break;
                }
            }
        };
};

/// Creature Demons Trail Bunny - 100045 CUSTOM
class npc_malfurion_trigger : public CreatureScript
{
    public:
        npc_malfurion_trigger() : CreatureScript("npc_malfurion_trigger") { }

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_malfurion_triggerAI(p_Creature);
        }

        struct npc_malfurion_triggerAI : public ScriptedAI
        {
            npc_malfurion_triggerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetVisible(false);
            }

            std::set<uint64> m_HandledPlayers;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (m_HandledPlayers.find(p_Who->GetGUID()) != m_HandledPlayers.end())
                    return;

                Player* p_Player = me->SelectNearestPlayer(5.0f);

                if (p_Player != nullptr &&  p_Player->HasQuest(38753))
                {
                    m_HandledPlayers.insert(p_Player->GetGUID());
                    p_Player->PlayStandaloneScene(ValsharahScenes::eScenes::Scene_Arrival, 17, { 2892.59f, 5895.35f, 297.11f, 6.27f });
                }
            }
        };
};

/// Creature Tyrande Whisperwind - 100051 CUSTOM
class npc_tyrande_whisperwind_valsharah : public CreatureScript
{
    public:
        npc_tyrande_whisperwind_valsharah() : CreatureScript("npc_tyrande_whisperwind_valsharah") { }

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_tyrande_whisperwind_valsharahAI(p_Creature);
        }

        struct npc_tyrande_whisperwind_valsharahAI : public ScriptedAI
        {
            npc_tyrande_whisperwind_valsharahAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                events.ScheduleEvent(ValsharahEvents::eEvents::Event_Tyrande_Wp, 10000);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case 2:
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(2922.64f, 5879.79f, 299.53f, 30.0f, 10.0f);
                        events.ScheduleEvent(ValsharahEvents::eEvents::Event_Tyrande_Wp2, 1500);
                        break;
                    }
                    case 3:
                    {
                        std::list<Player*> l_PlayerList;

                        me->GetPlayerListInGrid(l_PlayerList, 15.0f, false);

                        for (Player* l_Player : l_PlayerList)
                        {
                            if (l_Player->HasQuest(ValsharahQuests::eQuests::Quest_Demons_Trail))
                            {
                                l_Player->CompleteQuest(ValsharahQuests::eQuests::Quest_Demons_Trail);
                                l_Player->RemoveAura(ValsharahSpells::eSpells::Spell_Xavius_Root);
                                me->ForcedDespawn();
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case ValsharahEvents::eEvents::Event_Tyrande_Wp:
                    {
                        me->AddAura(ValsharahSpells::eSpells::Spell_Saber_Mount, me);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothPath(ValsharahMoves::eMoves::Move_Tyrande, ValsharahWaypoints::g_Path_Tyrande.data(), ValsharahWaypoints::g_Path_Tyrande.size(), false);
                        break;
                    }

                    case ValsharahEvents::eEvents::Event_Tyrande_Wp2:
                    {
                        Talk(0);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothPath(ValsharahMoves::eMoves::Move_Tyrande2, ValsharahWaypoints::g_Path_Tyrande_2.data(), ValsharahWaypoints::g_Path_Tyrande_2.size(), false);

                        break;
                    }

                    default:
                        break;
                }
            }
        };
};

/// Scene for Chapter 4 Beginning
class player_script_ysera_summoning_scene : public PlayerScript
{
    public:
        player_script_ysera_summoning_scene() : PlayerScript("player_script_ysera_summoning_scene") { }

        void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceId) override
        {
            if (p_Player->GetSceneInstanceIDByPackage(ValsharahScenes::eScenes::Scene_Summon_Ysera) == p_SceneInstanceId)
            {
                p_Player->SummonCreature(ValsharahCreatures::eCreatures::Npc_Malfurion_Scene, 2608.79f, 6712.15f, 104.871f, 0.246f, TEMPSUMMON_CORPSE_DESPAWN);
            }
        }

        void OnSceneComplete(Player* p_Player, uint32 p_SceneInstanceId) override
        {
            if (p_Player->GetSceneInstanceIDByPackage(ValsharahScenes::eScenes::Scene_Summon_Ysera) == p_SceneInstanceId)
            {
                p_Player->SummonCreature(ValsharahCreatures::eCreatures::Npc_Malfurion_Scene, 2608.79f, 6712.15f, 104.871f, 0.246f, TEMPSUMMON_CORPSE_DESPAWN);
            }
        }
};

/// Scenes for Chapter 4 Ending - Chapter 5 Beginning
class player_script_malfurion_scene : public PlayerScript
{
    public:
        player_script_malfurion_scene() : PlayerScript("player_script_malfurion_scene") { }

        void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceID) override
        {
            if (p_Player->GetSceneInstanceIDByPackage(ValsharahScenes::eScenes::Scene_Arrival) == p_SceneInstanceID)
            {
                p_Player->SendMovieStart(ValsharahScenes::eScenes::Scene_Corruption);
            }
        }

        void OnSceneComplete(Player* p_Player, uint32 p_SceneInstanceID) override
        {
            if (p_Player->GetSceneInstanceIDByPackage(ValsharahScenes::eScenes::Scene_Arrival) == p_SceneInstanceID)
            {
                p_Player->SendMovieStart(ValsharahScenes::eScenes::Scene_Corruption);
            }
        }

        void OnMovieComplete(Player* p_Player, uint32 p_MovieId) override
        {
            if (ValsharahScenes::eScenes::Scene_Corruption == p_MovieId)
            {
                p_Player->PlayStandaloneScene(ValsharahScenes::eScenes::Scene_Xavius_Escape, 17, { 2892.59f, 5895.35f, 297.11f, 6.27f });
                p_Player->TeleportTo(1220, 2892.59f, 5895.35f, 297.11f, 6.27f);
                p_Player->AddAura(ValsharahSpells::eSpells::Spell_Xavius_Root, p_Player);
                p_Player->SummonCreature(ValsharahCreatures::eCreatures::Npc_Tyrande_custom, 2929.89f, 5909.58f, 304.34f, 5.12f, TEMPSUMMON_CORPSE_DESPAWN);
            }
        }

        void OnLogin(Player* p_Player) override
        {
            if (p_Player->HasAura(ValsharahSpells::eSpells::Spell_Xavius_Root))
                p_Player->RemoveAura(ValsharahSpells::eSpells::Spell_Xavius_Root);
        }
};

#ifndef __clang_analyzer__
void AddSC_zone_valsharah()
{
    /// Creature Scripts
    new npc_archmage_khadgar_valsharah();
    new npc_ysera_quest();
    new npc_malfurion_trigger();
    new npc_tyrande_whisperwind_valsharah();
    new npc_tyrande_waypoint();
    new npc_ysera_grove();
    new npc_malfurion_grove();
    new npc_hippogryph_valsharah();

    /// Player Scripts
    new player_script_ysera_summoning_scene();
    new player_script_malfurion_scene();
}
#endif
