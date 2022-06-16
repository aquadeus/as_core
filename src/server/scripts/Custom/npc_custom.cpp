////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "GameObjectAI.h"
#include "NPCHandler.h"
#include "Vehicle.h"
#include "PhaseMgr.h"
#include "Common.h"
#include "ObjectMgr.h"
#include "World.h"
#include "PetAI.h"
#include "PassiveAI.h"
#include "CombatAI.h"
#include "GameEventMgr.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "SpellAuras.h"
#include "Vehicle.h"
#include "Player.h"
#include "SpellScript.h"
#include "MapManager.h"
#include "Chat.h"

/// 300007 - World Boss Teleporter
/// No SQL commit with this script, must stay on PTR.
/*
REPLACE INTO `creature_template` (`entry`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `femaleName`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `exp_req`, `faction`, `npcflag`, `npcflag2`, `speed_walk`, `speed_run`, `speed_fly`, `scale`, `rank`, `dmgschool`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `baseVariance`, `rangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `type_flags2`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Mana_mod_extra`, `Armor_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `TrackingQuestID`, `VignetteID`, `RegenHealth`, `equipment_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `BuildVerified`) VALUES('300007','0','0','35231','0','0','0','World Boss Teleporter','','','','0','100','100','6','0','35','1','0','1','1.14286','1.14286','1','0','0','1','0','0','1','1','1','518','0','0','0','0','0','0','0','0','0','10','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','1','1','1','1','0','0','0','0','0','0','0','0','0','0','1','0','0','0','npc_world_boss_gossip','1');
DELETE FROM creature WHERE id = 300007;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `protec_anti_doublet`) VALUES('300007','0','12','7486','1','4294967295','0','0','-9083.16','396.379','93.6052','2.18698','300','0','0','0','0','0','0','0','0','2048','0','0','0','0',NULL);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `protec_anti_doublet`) VALUES('300007','1','14','4982','1','4294967295','0','0','1366.45','-4355.5','26.5794','4.62173','300','0','0','0','0','0','0','0','0','2048','0','0','0','0',NULL);
REPLACE INTO `trinity_string` (`entry`, `content_default`, `content_loc2`) VALUE ('14097','Hey, great adventurer, come to me in your faction capital, to access fastly our last testable content ! Newest raids, bosses, get directly teleported to the destination you are looking for.',NULL,'Hey, cher aventurier, venez me voir dans la capitale de votre faction, pour acceder rapidement a notre dernier contenu testable ! Derniers raids, boss, soyez directement teleporte a la destination de votre choix.');
*/
class npc_world_boss_gossip : public CreatureScript
{
    public:
        npc_world_boss_gossip() : CreatureScript("npc_world_boss_gossip")
        {
        }

        enum eActions
        {
            SelectRukhmar     = 1001,
            SelectTarlna      = 1002,
            SelectDrov        = 1003,
            SelectBlackrock   = 1004,
            TpAloneRukhmar    = 1005,
            TpGroupRukhmar    = 1006,
            TpAloneTarlna     = 1007,
            TpGroupTarlna     = 1008,
            TpAloneDrov       = 1009,
            TpGroupDrov       = 1010,
            TpAloneBlackrock  = 1011,
            TpGroupBlackrock  = 1012,
        };

        enum Destinations
        {
            DestinationRukhmar          = 1,
            DestinationTarlna           = 2,
            DestinationDrov             = 3,
            DestinationBlackRockFoundry = 4
        };

        void TeleportPlayer(Player* p_Player, uint8 p_Destination)
        {
            switch (p_Destination)
            {
                case Destinations::DestinationRukhmar:
                    p_Player->TeleportTo(1116, 167.2388f, 2655.39f, 68.58f, 4.105f);
                    break;
                case Destinations::DestinationTarlna:
                    p_Player->TeleportTo(1116, 4918.54f, 1292.16f, 120.28f, 4.018f);
                    break;
                case Destinations::DestinationDrov:
                    p_Player->TeleportTo(1116, 7330.3f, 1455.87f, 81.76f, 6.08f);
                    break;
                case Destinations::DestinationBlackRockFoundry:
                    p_Player->TeleportTo(1116, 7856.9667f, 555.5454f, 124.1314f, 6.08f);
                    break;
            }
        }

        void TeleportGroup(Player* p_Player, uint8 p_Destination)
        {
            std::list<Unit*> l_PlayerList;
            p_Player->GetPartyMembers(l_PlayerList);

            for (Unit* l_GroupMember : l_PlayerList)
            {
                if (l_GroupMember->IsPlayer())
                {
                    switch (p_Destination)
                    {
                        case Destinations::DestinationRukhmar:
                            TeleportPlayer(l_GroupMember->ToPlayer(), Destinations::DestinationRukhmar);
                            break;
                        case Destinations::DestinationTarlna:
                            TeleportPlayer(l_GroupMember->ToPlayer(), Destinations::DestinationTarlna);
                            break;
                        case Destinations::DestinationDrov:
                            TeleportPlayer(l_GroupMember->ToPlayer(), Destinations::DestinationDrov);
                            break;
                        case Destinations::DestinationBlackRockFoundry:
                            p_Player->TeleportTo(1116, 8084.64f, 855.51f, 34.3623f, 6.037f);
                            break;
                    }
                }
            }
        }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I would like to go to Rukhmar, please.", GOSSIP_SENDER_MAIN, eActions::SelectRukhmar);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I would like to go to Tarlna, please.", GOSSIP_SENDER_MAIN, eActions::SelectTarlna);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I would like to go to Drov, please.", GOSSIP_SENDER_MAIN, eActions::SelectDrov);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I would like to go Blackrock Foundry, please.", GOSSIP_SENDER_MAIN, eActions::SelectBlackrock);
            p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->PlayerTalkClass->SendCloseGossip();

            switch (p_Action)
            {
                case eActions::SelectRukhmar:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to fight Rukhmar alone.", GOSSIP_SENDER_MAIN, eActions::TpAloneRukhmar);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to fight Rukhmar with my group.", GOSSIP_SENDER_MAIN, eActions::TpGroupRukhmar);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectTarlna:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to fight Tarlna alone.", GOSSIP_SENDER_MAIN, eActions::TpAloneTarlna);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to fight Tarlna with my group.", GOSSIP_SENDER_MAIN, eActions::TpGroupTarlna);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectDrov:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to fight Drov alone.", GOSSIP_SENDER_MAIN, eActions::TpAloneDrov);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to fight Drov with my group.", GOSSIP_SENDER_MAIN, eActions::TpGroupDrov);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectBlackrock:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to test Blackrock Foundry alone.", GOSSIP_SENDER_MAIN, eActions::TpAloneBlackrock);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to fight Blackrock Foundry with my group.", GOSSIP_SENDER_MAIN, eActions::TpGroupBlackrock);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::TpAloneRukhmar:
                    TeleportPlayer(p_Player, Destinations::DestinationRukhmar);
                    break;
                case eActions::TpGroupRukhmar:
                    TeleportGroup(p_Player, Destinations::DestinationRukhmar);
                    break;
                case eActions::TpAloneTarlna:
                    TeleportPlayer(p_Player, Destinations::DestinationTarlna);
                    break;
                case eActions::TpGroupTarlna:
                    TeleportGroup(p_Player, Destinations::DestinationTarlna);
                    break;
                case eActions::TpAloneDrov:
                    TeleportPlayer(p_Player, Destinations::DestinationDrov);
                    break;
                case eActions::TpGroupDrov:
                    TeleportGroup(p_Player, Destinations::DestinationDrov);
                    break;
                case eActions::TpAloneBlackrock:
                    TeleportPlayer(p_Player, Destinations::DestinationBlackRockFoundry);
                    break;
                case eActions::TpGroupBlackrock:
                    TeleportGroup(p_Player, Destinations::DestinationBlackRockFoundry);
                    break;
                default:
                    break;
            }

            return true;
        }

        struct npc_world_boss_gossipAI : public ScriptedAI
        {
            npc_world_boss_gossipAI(Creature* creature) : ScriptedAI(creature)
            {
                m_YellTimer = 300 * IN_MILLISECONDS;
            }

            uint32 m_YellTimer;

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_YellTimer)
                {
                    if (m_YellTimer <= p_Diff)
                    {
                        /// "Hey $N, come to me in your faction capital, to access fastly our last testable content ! Newest raids, boss, get directly teleported to the destination you are looking for."
                        me->YellToZone(14097, LANG_UNIVERSAL, 0);
                        m_YellTimer = 300 * IN_MILLISECONDS;
                    }
                    else
                        m_YellTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_world_boss_gossipAI(creature);
        }
};

enum eHighmaulDests
{
    DestinationKargath,
    DestinationButcher,
    DestinationBrackenspore,
    DestinationTectus,
    DestinationTwinOgron,
    DestinationKoragh,
    DestinationMargok,
    MaxHighmaulDests
};

Position const g_HighmaulDests[eHighmaulDests::MaxHighmaulDests] =
{
    { 3475.60f, 7590.64f, 55.30f, 4.062f }, ///< Kargath
    { 3698.73f, 7601.06f, 30.03f, 1.178f }, ///< Butcher
    { 3983.92f, 7742.95f, 5.009f, 0.019f }, ///< Brackenspore
    { 3492.97f, 7907.10f, 68.40f, 0.679f }, ///< Tectus
    { 3923.18f, 8310.01f, 322.1f, 0.836f }, ///< Twin Ogron
    { 3864.26f, 8554.24f, 367.6f, 0.903f }, ///< Ko'ragh
    { 4067.41f, 8582.56f, 572.6f, 3.084f }  ///< Imperator Mar'gok
};

/// This class is used to activate tests for Highmaul
class HighmaulTestEnable : public BasicEvent
{
    enum eHighmaulData
    {
        TestsActivated = 1
    };

    public:
        HighmaulTestEnable(uint64 p_Guid) : BasicEvent(), m_Guid(p_Guid) { }
        virtual ~HighmaulTestEnable() {}

        virtual bool Execute(uint64 /*p_EndTime*/, uint32 /*p_Time*/)
        {
            if (Player* l_Player = HashMapHolder<Player>::Find(m_Guid))
            {
                if (InstanceScript* l_Highmaul = l_Player->GetInstanceScript())
                {
                    if (l_Highmaul->GetData(eHighmaulData::TestsActivated))
                        return false;

                    l_Highmaul->SetData(eHighmaulData::TestsActivated, true);
                }
            }

            return true;
        }

        virtual void Abort(uint64 /*p_EndTime*/) { }

    private:
        uint64 m_Guid;
};

class npc_pve_tests_manager : public CreatureScript
{
    public:
        npc_pve_tests_manager() : CreatureScript("npc_pve_tests_manager") { }

        enum eActions
        {
            /// Gossip select
            /// Highmaul
            SelectHighmaul      = 1000,
            SelectKargath       = 1001,
            SelectButcher       = 1002,
            SelectBrackenspore  = 1003,
            SelectTectus        = 1004,
            SelectTwinOgron     = 1005,
            SelectKoragh        = 1006,
            SelectMargok        = 1007,
            /// Blackrock Foundry
            SelectBlackrock     = 1008,
            SelectBRFAlone      = 1009,
            SelectBRFGroup      = 1010,
            /// Boss select
            /// Highmaul
            KargathGroup        = 10001,
            ButcherGroup        = 10002,
            BrackensporeGroup   = 10003,
            TectusGroup         = 10004,
            TwinOgronGroup      = 10005,
            KoraghGroup         = 10006,
            MargokGroup         = 10007
        };

        enum eData
        {
            HighmaulMapID = 1228
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to do some tests in Highmaul.", GOSSIP_SENDER_MAIN, eActions::SelectHighmaul);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to do some tests in Blackrock Foundry.", GOSSIP_SENDER_MAIN, eActions::SelectBlackrock);
            p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->PlayerTalkClass->SendCloseGossip();

            if (!p_Creature->IsAIEnabled)
                return true;

            npc_pve_tests_manager::npc_pve_tests_managerAI* l_AI = CAST_AI(npc_pve_tests_manager::npc_pve_tests_managerAI, p_Creature->GetAI());
            if (l_AI == nullptr)
                return true;

            switch (p_Action)
            {
                case eActions::SelectHighmaul:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight against Kargath Bladefist.", GOSSIP_SENDER_MAIN, eActions::SelectKargath);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight against The Butcher.", GOSSIP_SENDER_MAIN, eActions::SelectButcher);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight against Brackenspore.", GOSSIP_SENDER_MAIN, eActions::SelectBrackenspore);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight against Tectus.", GOSSIP_SENDER_MAIN, eActions::SelectTectus);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight against the Twin Ogron.", GOSSIP_SENDER_MAIN, eActions::SelectTwinOgron);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight against Ko'ragh.", GOSSIP_SENDER_MAIN, eActions::SelectKoragh);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight against Imperator Mar'gok.", GOSSIP_SENDER_MAIN, eActions::SelectMargok);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectKargath:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight Kargath Bladefist with my group.", GOSSIP_SENDER_MAIN, eActions::KargathGroup);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectButcher:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight The Butcher with my group.", GOSSIP_SENDER_MAIN, eActions::ButcherGroup);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectBrackenspore:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight Brackenspore with my group.", GOSSIP_SENDER_MAIN, eActions::BrackensporeGroup);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectTectus:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight Tectus with my group.", GOSSIP_SENDER_MAIN, eActions::TectusGroup);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectTwinOgron:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight the Twin Ogron with my group.", GOSSIP_SENDER_MAIN, eActions::TwinOgronGroup);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectKoragh:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight Ko'ragh with my group.", GOSSIP_SENDER_MAIN, eActions::KoraghGroup);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectMargok:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to fight Imperator Mar'gok with my group.", GOSSIP_SENDER_MAIN, eActions::MargokGroup);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::SelectBlackrock:
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to be teleported to Blackrock Foundry alone.", GOSSIP_SENDER_MAIN, eActions::SelectBRFAlone);
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "I want to be teleported to Blackrock Foundry with my group.", GOSSIP_SENDER_MAIN, eActions::SelectBRFGroup);
                    p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
                    break;
                case eActions::KargathGroup:
                    l_AI->TeleportGroup(p_Player, eHighmaulDests::DestinationKargath);
                    break;
                case eActions::ButcherGroup:
                    l_AI->TeleportGroup(p_Player, eHighmaulDests::DestinationButcher);
                    break;
                case eActions::BrackensporeGroup:
                    l_AI->TeleportGroup(p_Player, eHighmaulDests::DestinationBrackenspore);
                    break;
                case eActions::TectusGroup:
                    l_AI->TeleportGroup(p_Player, eHighmaulDests::DestinationTectus);
                    break;
                case eActions::TwinOgronGroup:
                    l_AI->TeleportGroup(p_Player, eHighmaulDests::DestinationTwinOgron);
                    break;
                case eActions::KoraghGroup:
                    l_AI->TeleportGroup(p_Player, eHighmaulDests::DestinationKoragh);
                    break;
                case eActions::MargokGroup:
                    l_AI->TeleportGroup(p_Player, eHighmaulDests::DestinationMargok);
                    break;
                case eActions::SelectBRFAlone:
                    p_Player->TeleportTo(1116, 8084.64f, 855.51f, 34.3623f, 6.037f);
                    break;
                case eActions::SelectBRFGroup:
                    l_AI->TeleportGroup(p_Player, 8084.64f, 855.51f, 34.3623f, 6.037f);
                    break;
                default:
                    break;
            }

            return true;
        }

        struct npc_pve_tests_managerAI : public ScriptedAI
        {
            npc_pve_tests_managerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_YellTimer = 300 * IN_MILLISECONDS;
            }

            uint32 m_YellTimer;

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_YellTimer)
                {
                    if (m_YellTimer <= p_Diff)
                    {
                        /// "Hey $N, come to me in your faction capital, to access fastly our last testable content ! Newest raids, boss, get directly teleported to the destination you are looking for."
                        me->YellToZone(TrinityStrings::WarnRaidTests, LANG_UNIVERSAL, 0);
                        m_YellTimer = 300 * IN_MILLISECONDS;
                    }
                    else
                        m_YellTimer -= p_Diff;
                }
            }

            void TeleportPlayer(Player* p_Player, uint8 p_Destination)
            {
                if (p_Destination >= eHighmaulDests::MaxHighmaulDests)
                    return;

                /// This will prevent players to be re-teleported depending on bosses states
                p_Player->BeginSummon();
                p_Player->TeleportTo(eData::HighmaulMapID, g_HighmaulDests[p_Destination]);

                p_Player->m_Events.AddEvent(new HighmaulTestEnable(p_Player->GetGUID()), 3 * TimeConstants::IN_MILLISECONDS);
            }

            void TeleportGroup(Player* p_Player, uint8 p_Destination)
            {
                if (p_Player->GetGroup() == nullptr)
                {
                    me->MonsterSay("Sorry, but you must be in a raid group to test this boss.", LANG_UNIVERSAL, 0);
                    return;
                }

                std::list<Unit*> l_PlayerList;
                p_Player->GetRaidMembers(l_PlayerList);

                for (Unit* l_GroupMember : l_PlayerList)
                {
                    if (l_GroupMember->GetTypeId() == TypeID::TYPEID_PLAYER)
                        TeleportPlayer(l_GroupMember->ToPlayer(), p_Destination);
                }
            }

            void TeleportGroup(Player* p_Player, float p_X, float p_Y, float p_Z, float p_O)
            {
                if (p_Player->GetGroup() == nullptr)
                {
                    me->MonsterSay("Sorry, but you must be in a raid group to test this boss.", LANG_UNIVERSAL, 0);
                    return;
                }

                std::list<Unit*> l_PlayerList;
                p_Player->GetRaidMembers(l_PlayerList);

                for (Unit* l_GroupMember : l_PlayerList)
                {
                    if (l_GroupMember->GetTypeId() == TypeID::TYPEID_PLAYER)
                        l_GroupMember->ToPlayer()->TeleportTo(1116, p_X, p_Y, p_Z, p_O);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_pve_tests_managerAI(p_Creature);
        }
};

/// Easter Egg <Firestorm Easter Event> - 666667
ACE_Based::LockedMap<uint32 /*accountid*/, std::set<uint32> /*EggGuid*/> g_LootedEgg;

class npc_easter_egg_event : public CreatureScript
{
public:
    npc_easter_egg_event() : CreatureScript("npc_easter_egg_event") { }

    bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
    {
        enum eItems
        {
            /// Equipments
            WhiteSpringCirclet  = 44803,
            BlackSpringCirclet  = 74282,
            PinkSpringCirclet   = 74283,

            /// Troll Items
            Deadmole            = 90557,
            CursedRabbitsFoot   = 155905
        };

        enum ePets
        {
            NoblegardenBunny            = 141532,
            NoblegardenBunnySpell       = 227113,

            Eggbert                     = 32616,
            EggbertSpell                = 40614,

            CorgiPup                    = 136925,
            CorgiPupSpell               = 210701,

            SpringRabbitsFood           = 44794,
            SpringRabbitsFoodSpell      = 61725,

            MysticalSpringBouquet       = 116258,
            MysticalSpringBouquetSpell  = 171222
        };

        enum eMounts
        {
            BigBlizzardBear             = 43599,
            BigBlizzardBearSpell        = 58983,

            DarkmoonDancingBear         = 73766,
            DarkmoonDancingBearSpell    = 103081,

            DarkmoonDirigible           = 153485,
            DarkmoonDirigibleSpell      = 247448,

            MagicRoosterEgg             = 49290,
            MagicRoosterEggSpell        = 65917,

            SwiftSpringstriderReins     = 72145,
            SwiftSpringstriderSpell     = 102349,

            SwiftForestStriderReins     = 72140,
            SwiftForestStriderSpell     = 102346,

            SnowfeatherHunterSpell      = 213163
        };

        enum eToys
        {
            SelfieCamera            = 122637,
            SelfieCameraSpell       = 181765,

            Pepe                    = 122293,
            PepeSpell               = 181943,

            EggTransformer          = 141879,
            EggTransformerSpell     = 228290,

            WholeBodyShrinka        = 97919,
            WholeBodyShrinkaSpell   = 141917
        };

        enum ePoints
        {
            LoyaltyMinRewardAmount = 1,
            LoyaltyMaxRewardAmount = 5
        };

        uint32 l_AccountID = p_Player->GetSession()->GetAccountId();
        uint64 l_GUID = p_Player->GetGUID();
        uint32 l_BunnyDBGuid = p_Creature->GetDBTableGUIDLow();
        uint64 l_BunnyGuid = p_Creature->GetGUID();

        if (g_LootedEgg[l_AccountID].find(l_BunnyDBGuid) != g_LootedEgg[l_AccountID].end())
        {
            ChatHandler(p_Player).SendSysMessage("You already got the reward for this egg!");
            return true;
        }

        g_LootedEgg[l_AccountID].insert(l_BunnyDBGuid);

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EASTER_EVENT);

        l_Stmt->setUInt64(0, l_BunnyDBGuid);
        l_Stmt->setUInt32(1, l_AccountID);

        CharacterDatabase.AsyncQuery(l_Stmt, p_Player->GetQueryCallbackMgr(), [l_BunnyDBGuid, l_AccountID, l_GUID, l_BunnyGuid](PreparedQueryResult const& p_Result)
        {
            Player* l_Player = sObjectAccessor->FindPlayer(l_GUID);
            if (!l_Player)
                return;

            Creature* l_Bunny = sObjectAccessor->FindCreature(l_BunnyGuid);
            if (!l_Bunny)
                return;

            if (p_Result)
            {
                ChatHandler(l_Player).SendSysMessage("You already got the reward for this egg!");
                return;
            }

            uint32 l_LPAmount = ePoints::LoyaltyMinRewardAmount;

            while (urand(0, 1) == 1 && l_LPAmount < ePoints::LoyaltyMaxRewardAmount)
                l_LPAmount += 1;

            l_Player->GetSession()->AddLoyaltyPoints(l_LPAmount, "FirestormEasterEvent2018 - 2");

            if (MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact())
                l_Manager->ModifyArtifactPower(urand(100, 300), 1);

            if (roll_chance_i(15))
            {
                uint8 l_Reward = urand(0, 100);

                /// Items 35% chance
                if (l_Reward < 35)
                {
                    std::array<uint32, 5> l_Items = { { eItems::WhiteSpringCirclet, eItems::BlackSpringCirclet, eItems::PinkSpringCirclet, eItems::Deadmole, eItems::CursedRabbitsFoot } };
                    std::vector<uint32> l_PossibleItems;
                    for (uint32 l_Item : l_Items)
                    {
                        if (!l_Player->HasItemCount(l_Item, 1, true))
                            l_PossibleItems.push_back(l_Item);
                    }

                    if (l_PossibleItems.size())
                        l_Player->AddItem(l_PossibleItems[urand(0, l_PossibleItems.size() - 1)], 1);
                }
                /// Mounts 15%
                else if (l_Reward < 50)
                {

                    std::array<uint32, 7> l_Mounts = { { eMounts::MagicRoosterEggSpell, eMounts::SwiftSpringstriderSpell, eMounts::SwiftForestStriderSpell, eMounts::SnowfeatherHunterSpell, eMounts::BigBlizzardBearSpell, eMounts::DarkmoonDancingBearSpell, eMounts::DarkmoonDirigibleSpell } };
                    std::vector<uint32> l_PossibleMounts;
                    for (uint32 l_Mount : l_Mounts)
                    {
                        if (!l_Player->HasSpell(l_Mount))
                            l_PossibleMounts.push_back(l_Mount);
                    }

                    if (l_PossibleMounts.size())
                        l_Player->learnSpell(l_PossibleMounts[urand(0, l_PossibleMounts.size() - 1)], false);
                }
                /// Pets 25%
                else if (l_Reward < 75)
                {
                    std::array<uint32, 5> l_Pets = { { ePets::NoblegardenBunnySpell, ePets::EggbertSpell, ePets::CorgiPupSpell, ePets::SpringRabbitsFoodSpell, ePets::MysticalSpringBouquetSpell } };
                    std::vector<uint32> l_PossiblePets;
                    for (uint32 l_Pet : l_Pets)
                    {
                        if (!l_Player->HasSpell(l_Pet))
                            l_PossiblePets.push_back(l_Pet);
                    }

                    if(l_PossiblePets.size())
                        l_Player->learnSpell(l_PossiblePets[urand(0, l_PossiblePets.size() - 1)], false);
                }
                /// Toys 25%
                else
                {
                    std::array<uint32, 4> l_Toys = { { eToys::EggTransformerSpell, eToys::PepeSpell, eToys::WholeBodyShrinkaSpell, eToys::SelfieCameraSpell } };
                    std::vector<uint32> l_PossibleToys;
                    for (uint32 l_Toy : l_PossibleToys)
                    {
                        if (!l_Player->HasSpell(l_Toy))
                            l_PossibleToys.push_back(l_Toy);
                    }

                    if (l_PossibleToys.size())
                        l_Player->learnSpell(l_PossibleToys[urand(0, l_PossibleToys.size() - 1)], false);
                }
            }

            PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_EASTER_EVENT);
            l_Stmt->setUInt64(0, l_BunnyDBGuid);
            l_Stmt->setUInt32(1, l_AccountID);
            CharacterDatabase.Execute(l_Stmt);
        });

        return true;
    }
};

/// Boris <Winter Veil helper> - 179317
class npc_boris : public CreatureScript
{
    public:
        npc_boris() : CreatureScript("npc_boris") { }

        enum eMenuIDs
        {
            MenuLevelUp     = 179317,
            MenuUnable      = 179319
        };

        enum eTalks
        {
            TalkNoSpace,
            TalkLevelOK,
            TalkAlreadyUsed
        };

        enum eVisual
        {
            MountID = 29344
        };

        static uint8 GetMaxLevel(Player* p_Player)
        {
            if (p_Player->getClass() == Classes::CLASS_DEATH_KNIGHT)
                return 55;

            if (p_Player->getClass() == Classes::CLASS_DEMON_HUNTER)
                return 98;

            if (IsAlliedRace(p_Player))
                return 20;

            return 10;
        }

        static bool IsAlliedRace(Player* p_Player)
        {
            switch (p_Player->getRace())
            {
                case Races::RACE_HIGHMOUNTAIN_TAUREN:
                case Races::RACE_LIGHTFORGED_DRAENEI:
                case Races::RACE_NIGHTBORNE:
                case Races::RACE_VOID_ELF:
                    return true;
                default:
                    return false;
            }

            return false;
        }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player->GetSession()->HasCustomFlags(AccountCustomFlags::FreePremadeLegion))
            {
                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->SEND_GOSSIP_MENU(eMenuIDs::MenuUnable, p_Creature->GetGUID());
                return true;
            }

            if (p_Player->getLevel() > GetMaxLevel(p_Player))
            {
                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->SEND_GOSSIP_MENU(eMenuIDs::MenuUnable, p_Creature->GetGUID());
                return true;
            }

            if (p_Player->GetSession()->HasCustomFlags(AccountCustomFlags::FreePremadeLegion))
            {
                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->ADD_GOSSIP_ITEM_DB(eMenuIDs::MenuLevelUp, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                p_Player->SEND_GOSSIP_MENU(eMenuIDs::MenuLevelUp, p_Creature->GetGUID());
                return true;
            }
            return false;
        }

        struct npc_borisAI : public ScriptedAI
        {
            npc_borisAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetUInt32Value(EUnitFields::UNIT_FIELD_MOUNT_DISPLAY_ID, eVisual::MountID);
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            static void FillItemList(std::list<ItemTemplate const*> &p_ItemList, Player* p_Player)
            {
                std::vector<std::vector<uint32>>    l_Items;
                std::set<uint32>                    l_ItemsToAdd;


                uint32 l_SpecID = p_Player->GetActiveSpecializationID();
                uint32 l_ItemCount = 0;

                l_Items.resize(InventoryType::MAX_INVTYPE);

                /// Fill possible equipment for each slots
                for (uint8 l_Iter = 0; l_Iter < InventoryType::MAX_INVTYPE; ++l_Iter)
                {
                    l_Items[l_Iter].clear();

                    for (auto l_Itr : *GetChallengeMaps())
                    {
                        std::vector<uint32> l_PossibleLoots = GetItemLootPerMap(l_Itr);

                        for (uint32 l_ItemId : l_PossibleLoots)
                        {
                            ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
                            if (!l_ItemTpl)
                                continue;

                            if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                                continue;

                            if (l_ItemTpl->InventoryType != l_Iter)
                                continue;

                            if (p_Player->CanUseItem(l_ItemTpl) != InventoryResult::EQUIP_ERR_OK)
                                continue;

                            l_Items[l_Iter].push_back(l_ItemTpl->ItemId);

                            break;
                        }
                    }

                    std::random_shuffle(l_Items[l_Iter].begin(), l_Items[l_Iter].end());
                }

                for (uint8 l_Iter = 0; l_Iter < InventoryType::MAX_INVTYPE; ++l_Iter)
                {
                    uint8 l_Count = 1;

                    switch (l_Iter)
                    {
                        case InventoryType::INVTYPE_FINGER:
                        case InventoryType::INVTYPE_TRINKET:
                        case InventoryType::INVTYPE_WEAPON:
                            l_Count++;
                            break;
                        default:
                            break;
                    }

                    for (uint32 l_Item : l_Items[l_Iter])
                    {
                        if (!l_Count)
                            break;

                        if (l_ItemsToAdd.find(l_Item) != l_ItemsToAdd.end())
                            continue;

                        l_ItemsToAdd.insert(l_Item);
                        --l_Count;
                        ++l_ItemCount;
                    }
                }

                for (auto l_Itr : l_ItemsToAdd)
                    p_ItemList.push_back(sObjectMgr->GetItemTemplate(l_Itr));
            }

            void sGossipSelect(Player* p_Player, uint32 /*p_MenuID*/, uint32 /*p_Action*/) override
            {
                if (p_Player->GetSession()->HasCustomFlags(AccountCustomFlags::FreePremadeLegion) && p_Player->getLevel() <= GetMaxLevel(p_Player))
                {
                    npc_boris::npc_borisAI* l_AI = CAST_AI(npc_boris::npc_borisAI, me->GetAI());

                    p_Player->GiveLevel(110, true);

                    std::list<ItemTemplate const*> l_Items;
                    FillItemList(l_Items, p_Player);

                    /// Bags
                    for (int i = 0; i < 4; i++)
                        l_Items.push_back(sObjectMgr->GetItemTemplate(933));

                    /// Mounts
                    uint32 l_MountItemId = 0;
                    switch (p_Player->getRace())
                    {
                        case Races::RACE_HUMAN:            l_MountItemId = 5656;  break;
                        case Races::RACE_GNOME:            l_MountItemId = 13321; break;
                        case Races::RACE_DWARF:            l_MountItemId = 5872;  break;
                        case Races::RACE_NIGHTELF:         l_MountItemId = 8632;  break;
                        case Races::RACE_ORC:              l_MountItemId = 5668;  break;
                        case Races::RACE_TROLL:            l_MountItemId = 8588;  break;
                        case Races::RACE_UNDEAD_PLAYER:    l_MountItemId = 13333; break;
                        case Races::RACE_BLOODELF:         l_MountItemId = 29220; break;
                        case Races::RACE_DRAENEI:          l_MountItemId = 29743; break;
                        case Races::RACE_PANDAREN_ALLIANCE:
                        case Races::RACE_PANDAREN_HORDE:
                        case Races::RACE_PANDAREN_NEUTRAL: l_MountItemId = 87800; break;
                        case Races::RACE_GOBLIN:           l_MountItemId = 62461; break;
                        case Races::RACE_WORGEN:           l_MountItemId = 73838; break;
                        case Races::RACE_TAUREN:           l_MountItemId = 15290; break;
                    }

                    if (l_MountItemId)
                        l_Items.push_back(sObjectMgr->GetItemTemplate(l_MountItemId));

                    if (p_Player->GetBagsFreeSlots() < l_Items.size())
                    {
                        int l_FreeBagSlots = 0;
                        for (int l_Slot = INVENTORY_SLOT_BAG_START; l_Slot < INVENTORY_SLOT_BAG_END; ++l_Slot)
                        {
                            if (!p_Player->GetBagByPos(l_Slot))
                                l_FreeBagSlots++;
                        }

                        if (l_FreeBagSlots < 2)
                        {
                            l_AI->Talk(eTalks::TalkNoSpace, p_Player->GetGUID());
                            return;
                        }
                    }

                    /// Mount skill
                    p_Player->learnSpell(33388, false);

                    for (ItemTemplate const* l_ItemTemplate : l_Items)
                        p_Player->StoreNewItemInBestSlots(l_ItemTemplate->ItemId, 1, ItemContext::WorldQuest10);

                    p_Player->GetSession()->UnsetCustomFlags(AccountCustomFlags::FreePremadeLegion);
                    l_AI->Talk(eTalks::TalkLevelOK, p_Player->GetGUID());

                    if (p_Player->getRace() == Races::RACE_PANDAREN_NEUTRAL)
                        p_Player->ShowNeutralPlayerFactionSelectUI();
                    else if (p_Player->GetTeamId() == TeamId::TEAM_HORDE)
                    {
                        WorldLocation location(1, 1366.730f, -4371.248f, 26.070f, 3.1266f);
                        p_Player->TeleportTo(location);
                        p_Player->SetHomebind(location, 363);
                    }
                    else if (p_Player->GetTeamId() == TeamId::TEAM_ALLIANCE)
                    {
                        WorldLocation location(0, -9096.236f, 411.380f, 92.257f, 3.649f);
                        p_Player->TeleportTo(location);
                        p_Player->SetHomebind(location, 9);
                    }

                    p_Player->SaveToDB();
                }
                p_Player->PlayerTalkClass->SendCloseGossip();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_borisAI(p_Creature);
        }
};

class npc_fun_gold_vendor : public CreatureScript
{
    public:
        npc_fun_gold_vendor() : CreatureScript("npc_fun_gold_vendor") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Give me 50 000 golds! ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());
            return true;
        }

        struct npc_fun_gold_vendorAI : public ScriptedAI
        {
            npc_fun_gold_vendorAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void sGossipSelect(Player* p_Player, uint32 p_MenuID, uint32 p_Action) override
            {
                p_Player->ModifyMoney(50000 * MoneyConstants::GOLD, "npc_fun_gold_vendor");
                p_Player->PlayerTalkClass->SendCloseGossip();
            }
        };

        CreatureAI * GetAI(Creature* p_Creature) const override
        {
            return new npc_fun_gold_vendorAI(p_Creature);
        }
};

class npc_fun_transmo_vendor : public CreatureScript
{
    public:
        npc_fun_transmo_vendor()
            : CreatureScript("npc_fun_transmo_vendor")
        {
        }

        /// Called when a player opens a gossip dialog with the creature.
        /// @p_Player   : Source player instance
        /// @p_Creature : Target creature instance
        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            static const uint8 l_VendorClass[] = {
                CLASS_DEATH_KNIGHT,
                CLASS_DRUID,
                CLASS_HUNTER,
                CLASS_MAGE,
                CLASS_MONK,
                CLASS_PALADIN,
                CLASS_PRIEST,
                CLASS_ROGUE,
                CLASS_SHAMAN,
                CLASS_WARLOCK,
                CLASS_WARRIOR,
            };

            uint8 l_ClassOffset = p_Creature->GetEntry() - 900003;

            if ((l_ClassOffset >= 0 && l_ClassOffset < sizeof(l_VendorClass) && l_VendorClass[l_ClassOffset] == p_Player->getClass()) || p_Player->isGameMaster())
            {
                p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "PvP Transmog", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "PvE Transmog", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            }

            p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());
            return true;
        }
        /// Called when a player selects a gossip item in the creature's gossip menu.
        /// @p_Player   : Source player instance
        /// @p_Creature : Target creature instance
        /// @p_Sender   : Sender menu
        /// @p_Action   : Action
        virtual bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->SendCloseGossip();
            p_Player->GetSession()->SendListInventory(p_Creature->GetGUID(), p_Action == (GOSSIP_ACTION_INFO_DEF + 2), true);
            return false;
        }

};

/// NPC ID: 90014
class npc_legendary_transmogrificator : public CreatureScript
{
    public:
        npc_legendary_transmogrificator() : CreatureScript("npc_legendary_transmogrificator") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->getLevel() < 19)
            {
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                return true;
            }

            WorldSession* session = player->GetSession();
            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_TABARD; slot++) // EQUIPMENT_SLOT_END
            {
                if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                {
                    uint32 quality = newItem->GetTemplate()->Quality;
                    if (quality == ITEM_QUALITY_UNCOMMON || quality == ITEM_QUALITY_RARE || quality == ITEM_QUALITY_EPIC || quality == ITEM_QUALITY_HEIRLOOM || quality == ITEM_QUALITY_LEGENDARY || quality == ITEM_QUALITY_ARTIFACT)
                    {
                        if (const char* slotName = GetSlotName(slot, session))
                            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, slotName, EQUIPMENT_SLOT_END, slot);
                    }
                }
            }
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, session->GetTrinityString(LANG_OPTION_REMOVE_ALL), EQUIPMENT_SLOT_END+2, 0, session->GetTrinityString(LANG_POPUP_REMOVE_ALL), 0, false);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, session->GetTrinityString(LANG_OPTION_UPDATE_MENU), EQUIPMENT_SLOT_END+1, 0);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 uiAction)
        {
            WorldSession* session = player->GetSession();
            player->PlayerTalkClass->ClearMenus();
            switch(sender)
            {
                case EQUIPMENT_SLOT_END: // Show items you can use
                {
                    if (Item* oldItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, uiAction))
                    {
                        uint32 lowGUID = player->GetGUIDLow();
                        _items[lowGUID].clear();
                        uint32 limit = 0;
                        for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
                        {
                            if (limit > 30)
                                break;
                            if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                            {
                                uint32 display = newItem->GetTemplate()->DisplayInfoID;

                                if (Item::CanTransmogrifyItemWithItem(oldItem->GetTemplate(), newItem->GetTemplate(), true))
                                {
                                    if (_items[lowGUID].find(display) == _items[lowGUID].end())
                                    {
                                        limit++;
                                        _items[lowGUID][display] = newItem;
                                        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, GetItemName(newItem, session), uiAction, display, session->GetTrinityString(LANG_POPUP_TRANSMOGRIFY)+GetItemName(newItem, session), GetFakePrice(oldItem), false);
                                    }
                                }
                            }
                        }

                        for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
                        {
                            if (Bag* bag = player->GetBagByPos(i))
                            {
                                for (uint32 j = 0; j < bag->GetBagSize(); j++)
                                {
                                    if (limit > 30)
                                        break;
                                    if (Item* newItem = player->GetItemByPos(i, j))
                                    {
                                        uint32 display = newItem->GetTemplate()->DisplayInfoID;
                                        if (Item::CanTransmogrifyItemWithItem(oldItem->GetTemplate(), newItem->GetTemplate(), true))
                                        {
                                            if (_items[lowGUID].find(display) == _items[lowGUID].end())
                                            {
                                                limit++;
                                                _items[lowGUID][display] = newItem;
                                                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, GetItemName(newItem, session), uiAction, display, session->GetTrinityString(LANG_POPUP_TRANSMOGRIFY)+GetItemName(newItem, session), GetFakePrice(oldItem), false);
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        char popup[250];
                        snprintf(popup, 250, session->GetTrinityString(LANG_POPUP_REMOVE_ONE), GetSlotName(uiAction, session));
                        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, session->GetTrinityString(LANG_OPTION_REMOVE_ONE), EQUIPMENT_SLOT_END+3, uiAction, popup, 0, false);
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, session->GetTrinityString(LANG_OPTION_BACK), EQUIPMENT_SLOT_END+1, 0);
                        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                    }
                    else
                        OnGossipHello(player, creature);

                    break;
                }
                case EQUIPMENT_SLOT_END+1: // Back
                {
                    OnGossipHello(player, creature);

                    break;
                }
                case EQUIPMENT_SLOT_END+2: // Remove Transmogrifications
                {
                    bool removed = false;
                    for (uint8 Slot = EQUIPMENT_SLOT_START; Slot < EQUIPMENT_SLOT_END; Slot++)
                    {
                        if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, Slot))
                        {
                            if (newItem->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS) != 0 && !removed)
                            {
                                newItem->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS, 0);
                                player->SetVisibleItemSlot(Slot, newItem);

                                removed = true;
                            }
                        }
                    }
                    if (removed)
                    {
                        ChatHandler(session).PSendSysMessage("%s", session->GetTrinityString(LANG_REM_TRANSMOGRIFICATIONS_ITEMS));
                        player->PlayDirectSound(3337);
                    }
                    else
                        session->SendNotification("%s", session->GetTrinityString(LANG_ERR_NO_TRANSMOGRIFICATIONS));

                    OnGossipHello(player, creature);

                    break;
                }
                case EQUIPMENT_SLOT_END+3: // Remove Transmogrification from single item
                {
                    if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, uiAction))
                    {
                        if (newItem->GetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS) != 0)
                        {
                            newItem->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS, 0);

                            player->SetVisibleItemSlot(uiAction, newItem);

                            ChatHandler(session).PSendSysMessage(session->GetTrinityString(LANG_REM_TRANSMOGRIFICATION_ITEM), GetSlotName(uiAction, session));
                            player->PlayDirectSound(3337);
                        }
                        else
                            session->SendNotification(session->GetTrinityString(LANG_ERR_NO_TRANSMOGRIFICATION), GetSlotName(uiAction, session));
                    }

                    OnGossipSelect(player, creature, EQUIPMENT_SLOT_END, uiAction);

                    break;
                }
                default: // Transmogrify
                {
                    uint32 lowGUID = player->GetGUIDLow();
                    if (Item* oldItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, sender))
                    {
                        if (_items[lowGUID].find(uiAction) != _items[lowGUID].end() && _items[lowGUID][uiAction]->IsInWorld())
                        {
                            Item* newItem = _items[lowGUID][uiAction];
                            if (newItem->GetOwnerGUID() == player->GetGUID() && (newItem->IsInBag() || newItem->GetBagSlot() == INVENTORY_SLOT_BAG_0)
                                && Item::CanTransmogrifyItemWithItem(oldItem->GetTemplate(), newItem->GetTemplate(), true))
                            {
                                auto l_Apparence = GetItemModifiedAppearance(newItem->GetEntry(), 0);
                                if (!l_Apparence)
                                {
                                    session->SendNotification("%s", session->GetTrinityString(LANG_ERR_NO_ITEM_SUITABLE));
                                    return true;
                                }

                                player->ModifyMoney(-1*GetFakePrice(oldItem), "npc_legendary_transmogrificator"); // take cost

                                /// Reseting the appearance to the base appaearance first IN EVERY APPEARANCE SLOT. Without this block, it would cause the legendary transmogrificator to not work, but still take the money (eg: an item can have 1 appearance in current spec, + an all spec legendary appearance = RIP)
                                oldItem->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS, 0);
                                oldItem->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_1, 0);
                                oldItem->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_2, 0);
                                oldItem->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_3, 0);
                                oldItem->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_SPEC_4, 0);

                                oldItem->SetModifier(ItemModifiers::ITEM_MODIFIER_TRANSMOG_APPEARANCE_ALL_SPECS, l_Apparence->ID);

                                player->SetVisibleItemSlot(sender, oldItem);

                                oldItem->UpdatePlayedTime(player);

                                oldItem->SetOwnerGUID(player->GetGUID());
                                oldItem->SetNotRefundable(player);
                                oldItem->ClearSoulboundTradeable(player);

                                if (newItem != nullptr)
                                {
                                    if (newItem->GetTemplate()->Bonding == BIND_WHEN_EQUIPED || newItem->GetTemplate()->Bonding == BIND_WHEN_USE)
                                        newItem->SetBinding(true);

                                    newItem->SetOwnerGUID(player->GetGUID());
                                    newItem->SetNotRefundable(player);
                                    newItem->ClearSoulboundTradeable(player);
                                    newItem->SetState(ITEM_CHANGED, player);
                                }

                                oldItem->SetState(ITEM_CHANGED, player);

                                player->PlayDirectSound(3337);

                                ChatHandler(session).PSendSysMessage(session->GetTrinityString(LANG_ITEM_TRANSMOGRIFIED), GetSlotName(sender, session));
                            }
                            else
                                session->SendNotification("%s", session->GetTrinityString(LANG_ERR_NO_ITEM_SUITABLE));
                        }
                        else
                            session->SendNotification("%s", session->GetTrinityString(LANG_ERR_NO_ITEM_EXISTS));
                    }
                    else
                        session->SendNotification("%s", session->GetTrinityString(LANG_ERR_EQUIP_SLOT_EMPTY));

                    _items[lowGUID].clear();
                    OnGossipSelect(player, creature, EQUIPMENT_SLOT_END, sender);

                    break;
                }
            }
            return true;
        }

    private:
        std::map<uint64, std::map<uint32, Item*> > _items; // _items[lowGUID][DISPLAY] = item

        const char * GetSlotName(uint8 slot, WorldSession* session)
        {
            switch(slot)
            {
                case EQUIPMENT_SLOT_HEAD      : return session->GetTrinityString(LANG_SLOT_NAME_HEAD);
                case EQUIPMENT_SLOT_SHOULDERS : return session->GetTrinityString(LANG_SLOT_NAME_SHOULDERS);
                case EQUIPMENT_SLOT_BODY      : return session->GetTrinityString(LANG_SLOT_NAME_BODY);
                case EQUIPMENT_SLOT_CHEST     : return session->GetTrinityString(LANG_SLOT_NAME_CHEST);
                case EQUIPMENT_SLOT_WAIST     : return session->GetTrinityString(LANG_SLOT_NAME_WAIST);
                case EQUIPMENT_SLOT_LEGS      : return session->GetTrinityString(LANG_SLOT_NAME_LEGS);
                case EQUIPMENT_SLOT_FEET      : return session->GetTrinityString(LANG_SLOT_NAME_FEET);
                case EQUIPMENT_SLOT_WRISTS    : return session->GetTrinityString(LANG_SLOT_NAME_WRISTS);
                case EQUIPMENT_SLOT_HANDS     : return session->GetTrinityString(LANG_SLOT_NAME_HANDS);
                case EQUIPMENT_SLOT_BACK      : return session->GetTrinityString(LANG_SLOT_NAME_BACK);
                case EQUIPMENT_SLOT_MAINHAND  : return session->GetTrinityString(LANG_SLOT_NAME_MAINHAND);
                case EQUIPMENT_SLOT_OFFHAND   : return session->GetTrinityString(LANG_SLOT_NAME_OFFHAND);
                case EQUIPMENT_SLOT_RANGED    : return session->GetTrinityString(LANG_SLOT_NAME_RANGED);
                case EQUIPMENT_SLOT_TABARD    : return session->GetTrinityString(LANG_SLOT_NAME_TABARD);
                default: return NULL;
            }
        }

        std::string GetItemName(Item* item, WorldSession* session)
        {
            std::string name = item->GetTemplate()->Name1->Get(session->GetSessionDbLocaleIndex());
            return name;
        }

        int32 GetFakePrice(Item* item)
        {
            return item->GetTemplate()->ItemLevel * 12.5f * GOLD;
        }
};

/// Jeanne <Loyalty Point> - 250000
class npc_loyalty_point : public CreatureScript
{
    public:
        npc_loyalty_point() : CreatureScript("npc_loyalty_point") { }

        enum eMenuIDs
        {
            GiveLoyaltyPoint = 92007,
            AlreadyEarnPoint = 92008
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player)
                return false;

            WorldSession* l_Session = p_Player->GetSession();
            time_t l_NowTime = time(nullptr);

            time_t l_LastClaimTime = l_Session->GetLastClaim();

            auto l_Time = localtime(&l_NowTime);
            struct tm l_Now = *l_Time;
            auto l_LastClaim = localtime(&l_LastClaimTime);

            /// If now is a different day than the last event reset day, then clear event history
            if (l_LastClaim->tm_year != l_Now.tm_year || l_LastClaim->tm_mday != l_Now.tm_mday || l_LastClaim->tm_mon != l_Now.tm_mon)
            {
                uint32 l_Points = 5;

                if (l_Session->GetLastBan() < (time(nullptr) - (MONTH * 6)))
                    l_Points += 2;

                if (l_Session->HaveAlreadyPurchasePoints())
                    l_Points += 1;

                if (l_Session->IsEmailValidated())
                    l_Points += 1;

                if (l_Session->GetActivityDays() > 13)
                    l_Points += 1;

                if (l_Session->GetActivityDays() > 59)
                    l_Points += 1;

                if (l_Session->GetActivityDays() > 179)
                    l_Points += 1;

                if (l_Session->GetActivityDays() > 359)
                    l_Points += 1;

                l_Session->AddLoyaltyPoints(l_Points, "NPC Daily reward");
                l_Session->SetLastClaim(time(nullptr));

                PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACC_LOYALTY);
                l_Statement->setUInt32(0, l_Session->GetAccountId());
                l_Statement->setUInt32(1, l_Session->GetLastClaim());
                l_Statement->setUInt32(2, l_Session->GetLastEventReset());
                LoginDatabase.Execute(l_Statement);

                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->SEND_GOSSIP_MENU(eMenuIDs::GiveLoyaltyPoint, p_Creature->GetGUID());
                return true;
            }

            p_Player->SEND_GOSSIP_MENU(eMenuIDs::AlreadyEarnPoint, p_Creature->GetGUID());
            return true;
        }
};

/// Jeanne <Loyalty Point Event Reward> - 930500
class npc_event_loyalty_point : public CreatureScript
{
    public:
        npc_event_loyalty_point() : CreatureScript("npc_event_loyalty_point") { }

        enum eMenuIDs
        {
            GiveLoyaltyPoint = 92007
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player)
                return false;

            WorldSession* l_Session = p_Player->GetSession();

            uint32 l_Points = 70;

            l_Session->AddLoyaltyPoints(l_Points, "NPC Daily reward");

            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->SEND_GOSSIP_MENU(eMenuIDs::GiveLoyaltyPoint, p_Creature->GetGUID());

            WorldLocation l_TeleportDestination = p_Player->GetTeamId() == TeamId::TEAM_HORDE ? WorldLocation(1220, -731.21f, 4359.23f, 737.61f, 2.38f) : WorldLocation(1220, -934.98f, 4578.56f, 729.27f, 5.61f);
            p_Player->TeleportTo(l_TeleportDestination);

            return true;
        }
};

/// Jeanne <Loyalty Point Event Reward> - 930501
class npc_event_loyalty_point_two : public CreatureScript
{
    public:
        npc_event_loyalty_point_two() : CreatureScript("npc_event_loyalty_point_two") { }

        enum eMenuIDs
        {
            GiveLoyaltyPoint = 92007
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player)
                return false;

            WorldSession* l_Session = p_Player->GetSession();

            uint32 l_Points = 100;

            l_Session->AddLoyaltyPoints(l_Points, "NPC Daily reward");

            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->SEND_GOSSIP_MENU(eMenuIDs::GiveLoyaltyPoint, p_Creature->GetGUID());

            WorldLocation l_TeleportDestination = p_Player->GetTeamId() == TeamId::TEAM_HORDE ? WorldLocation(1220, -731.21f, 4359.23f, 737.61f, 2.38f) : WorldLocation(1220, -934.98f, 4578.56f, 729.27f, 5.61f);
            p_Player->TeleportTo(l_TeleportDestination);

            return true;
        }
};

/// Cosmeticator 2000 - 300008
class npc_cosmetic_vendor : public CreatureScript
{
    public:
        npc_cosmetic_vendor() : CreatureScript("npc_cosmetic_vendor") { }

        enum eMenus
        {
            MenuAura        = GOSSIP_ACTION_INFO_DEF + 1,
            MenuMorph       = GOSSIP_ACTION_INFO_DEF + 2,
            MenuTestAura    = GOSSIP_ACTION_INFO_DEF + 3,
            MenuTestMorph   = GOSSIP_ACTION_INFO_DEF + 4,
            MenuBuyAura     = GOSSIP_ACTION_INFO_DEF + 5,
            MenuBuyMorph    = GOSSIP_ACTION_INFO_DEF + 6,
            MenuGetItem     = GOSSIP_ACTION_INFO_DEF + 7,
            MenuLootBox     = GOSSIP_ACTION_INFO_DEF + 8,

            MenusMax
        };

        enum eGossipMenus
        {
            MenuBase        = 300000,
            SubMenuAura     = 300001,
            SubMenuMorph    = 300002,
            AuraListTest    = 300003,
            MorphListTest   = 300004,
            AuraListBuy     = 300005,
            MorphListBuy    = 300006,
            SubMenuLootBox  = 300007
        };

        enum eGossipMenuOptions
        {
            OptionAura          = 0,
            OptionMorph         = 1,
            OptionTestAura      = 2,
            OptionBuyAura       = 3,
            OptionTestMorph     = 4,
            OptionBuyMorph      = 5,
            OptionGetItem       = 6,
            OptionLootBox       = 7
        };

        enum eItems
        {
            CosmeticLootbox = 310000
        };

        std::vector<CosmeticEntry> m_Auras;
        std::vector<CosmeticEntry> m_Morphs;
        std::vector<std::pair<uint32, std::pair<uint32, uint32>>> m_Prices;

        uint32 m_ListAuraBegin;
        uint32 m_ListMorphBegin;
        uint32 m_BuyAuraBegin;
        uint32 m_BuyMorphBegin;
        uint32 m_BuyLootBoxBegin;

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            m_Auras = sObjectMgr->GetActiveCosmetics(eCosmetics::CosmeticAura);
            m_Morphs = sObjectMgr->GetActiveCosmetics(eCosmetics::CosmeticMorph);

            m_Prices =
            {
                { 1,  {  0,  15 } },
                { 5,  {  1,  75 } },
                { 10, {  3, 150 } },
                { 20, {  8, 300 } },
                { 40, { 20, 600 } }
            };

            m_ListAuraBegin = eMenus::MenusMax;
            m_ListMorphBegin = m_ListAuraBegin + m_Auras.size();
            m_BuyAuraBegin = m_ListMorphBegin + m_Morphs.size();
            m_BuyMorphBegin = m_BuyAuraBegin + m_Auras.size();
            m_BuyLootBoxBegin = m_BuyMorphBegin + m_Morphs.size();

            p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionAura, GOSSIP_SENDER_MAIN, eMenus::MenuAura);
            p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionMorph, GOSSIP_SENDER_MAIN, eMenus::MenuMorph);
            p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionGetItem, GOSSIP_SENDER_MAIN, eMenus::MenuGetItem);

            if (!sObjectMgr->GetLootBoxCosmeticPool().empty())
                p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionLootBox, GOSSIP_SENDER_MAIN, eMenus::MenuLootBox);

            p_Player->SEND_GOSSIP_MENU(eGossipMenus::MenuBase, p_Creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            switch (p_Action)
            {
                case eMenus::MenuGetItem:
                {
                    if (!p_Player->GetItemByEntry(ITEM_COSMETIC))
                        p_Player->AddItem(ITEM_COSMETIC, 1);

                    p_Player->PlayerTalkClass->ClearMenus();
                    p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionAura, GOSSIP_SENDER_MAIN, eMenus::MenuAura);
                    p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionMorph, GOSSIP_SENDER_MAIN, eMenus::MenuMorph);
                    p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionGetItem, GOSSIP_SENDER_MAIN, eMenus::MenuGetItem);
                    p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionLootBox, GOSSIP_SENDER_MAIN, eMenus::MenuLootBox);
                    p_Player->SEND_GOSSIP_MENU(eGossipMenus::MenuBase, p_Creature->GetGUID());
                    break;
                }
                case eMenus::MenuAura:
                {
                    p_Player->PlayerTalkClass->ClearMenus();
                    p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::SubMenuAura, eGossipMenuOptions::OptionTestAura, GOSSIP_SENDER_MAIN, eMenus::MenuTestAura);
                    p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::SubMenuAura, eGossipMenuOptions::OptionBuyAura, GOSSIP_SENDER_MAIN, eMenus::MenuBuyAura);
                    p_Player->SEND_GOSSIP_MENU(eGossipMenus::SubMenuAura, p_Creature->GetGUID());
                    break;
                }
                case eMenus::MenuMorph:
                {
                    p_Player->PlayerTalkClass->ClearMenus();
                    p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::SubMenuMorph, eGossipMenuOptions::OptionTestMorph, GOSSIP_SENDER_MAIN, eMenus::MenuTestMorph);
                    p_Player->ADD_GOSSIP_ITEM_DB(eGossipMenus::SubMenuMorph, eGossipMenuOptions::OptionBuyMorph, GOSSIP_SENDER_MAIN, eMenus::MenuBuyMorph);
                    p_Player->SEND_GOSSIP_MENU(eGossipMenus::SubMenuMorph, p_Creature->GetGUID());
                    break;
                }
                case eMenus::MenuTestAura:
                {
                    GenerateAuraList(p_Player, p_Creature, true);
                    break;
                }
                case eMenus::MenuTestMorph:
                {
                    GenerateMorphList(p_Player, p_Creature, true);
                    break;
                }
                case eMenus::MenuBuyAura:
                {
                    GenerateAuraList(p_Player, p_Creature, false);
                    break;
                }
                case eMenus::MenuBuyMorph:
                {
                    GenerateMorphList(p_Player, p_Creature, false);
                    break;
                }
                case eMenus::MenuLootBox:
                {
                    DisplayLootboxList(p_Player, p_Creature);
                    break;
                }
                default:
                {
                    p_Player->DeMorph();
                    for (auto l_Aura : m_Auras)
                        p_Player->RemoveAura(l_Aura.ID);

                    if (p_Action >= m_ListAuraBegin && p_Action < m_ListMorphBegin)
                    {
                        Aura* l_Aura = p_Creature->AddAura(m_Auras[p_Action - m_ListAuraBegin].ID, p_Player);
                        if (!l_Aura)
                            break;

                        GenerateAuraList(p_Player, p_Creature, true);
                        break;
                    }

                    if (p_Action >= m_ListMorphBegin && p_Action < m_BuyAuraBegin)
                    {
                        p_Player->SetDisplayId(m_Morphs[p_Action - m_ListMorphBegin].ID);

                        GenerateMorphList(p_Player, p_Creature, true);
                        break;
                    }

                    if (p_Action >= m_BuyAuraBegin && p_Action < m_BuyMorphBegin)
                    {
                        if (!p_Player->HasCosmetic(eCosmetics::CosmeticAura, m_Auras[p_Action - m_BuyAuraBegin].ID))
                        {
                            CosmeticEntry l_Entry = m_Auras[p_Action - m_BuyAuraBegin];

                            HandleBuyingProcess(p_Player, l_Entry);
                        }
                        else if (WorldSession* l_Session = p_Player->GetSession())
                            ChatHandler(l_Session).PSendSysMessage(TrinityStrings::AlreadyOwnedAura);

                        GenerateAuraList(p_Player, p_Creature, false);
                        break;
                    }

                    if (p_Action >= m_BuyMorphBegin && p_Action < m_BuyLootBoxBegin)
                    {
                        if (!p_Player->HasCosmetic(eCosmetics::CosmeticMorph, m_Morphs[p_Action - m_BuyMorphBegin].ID))
                        {
                            CosmeticEntry l_Entry = m_Morphs[p_Action - m_BuyMorphBegin];

                            HandleBuyingProcess(p_Player, l_Entry);
                        }
                        else if (WorldSession* l_Session = p_Player->GetSession())
                            ChatHandler(l_Session).PSendSysMessage(TrinityStrings::AlreadyOwnedMorph);

                        GenerateMorphList(p_Player, p_Creature, false);
                        break;
                    }

                    if (p_Action >= m_BuyLootBoxBegin && p_Action < (m_BuyLootBoxBegin + m_Prices.size()))
                    {
                        HandleLootBoxBuyingProcess(p_Player, p_Action - m_BuyLootBoxBegin);
                        DisplayLootboxList(p_Player, p_Creature);
                        break;
                    }

                    break;
                }
            }
            return false;
        }

        void OnCloseGossip(Player* p_Player, Creature* p_Creature) override
        {
            p_Player->DeMorph();
            for (auto l_Aura : m_Auras)
                p_Player->RemoveAura(l_Aura.ID);

            p_Player->EnableCosmetic(p_Player->GetMap()->GetEntry()->IsWorldMap());
        }

        void GenerateAuraList(Player* p_Player, Creature* p_Creature, bool p_Test)
        {
            uint8 l_Itr = 0;
            p_Player->PlayerTalkClass->ClearMenus();

            for (auto l_Aura : m_Auras)
            {
                std::string l_Name = sObjectMgr->GetCosmeticName(p_Player, l_Aura.Type, l_Aura.ID);
                std::stringstream ss;
                ss << l_Aura.Cost << "points F  -- " << l_Name;

                std::string l_ConfirmString = sObjectMgr->GetFormatedTrinityString(TrinityStrings::ConfirmBuyCosmetic, p_Player->GetSession()->GetSessionDbLocaleIndex(), l_Name.c_str(), l_Aura.Cost);

                if (p_Test)
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ss.str().c_str(), GOSSIP_SENDER_MAIN, m_ListAuraBegin + l_Itr);
                else
                    p_Player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, ss.str().c_str(), GOSSIP_SENDER_MAIN, m_BuyAuraBegin + l_Itr, l_ConfirmString.c_str(), 0, false);
                ++l_Itr;
            }

            p_Player->SEND_GOSSIP_MENU((p_Test ? eGossipMenus::AuraListTest : eGossipMenus::AuraListBuy), p_Creature->GetGUID());
        }

        void GenerateMorphList(Player* p_Player, Creature* p_Creature, bool p_Test)
        {
            uint8 l_Itr = 0;
            p_Player->PlayerTalkClass->ClearMenus();

            for (auto l_Morph : m_Morphs)
            {
                std::string l_Name = sObjectMgr->GetCosmeticName(p_Player, l_Morph.Type, l_Morph.ID);
                std::stringstream ss;
                ss << l_Morph.Cost << "F  " << l_Name;

                std::string l_ConfirmString = sObjectMgr->GetFormatedTrinityString(TrinityStrings::ConfirmBuyCosmetic, p_Player->GetSession()->GetSessionDbLocaleIndex(), l_Name.c_str(), l_Morph.Cost);

                if (p_Test)
                    p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ss.str().c_str(), GOSSIP_SENDER_MAIN, m_ListMorphBegin + l_Itr);
                else
                    p_Player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, ss.str().c_str(), GOSSIP_SENDER_MAIN, m_BuyMorphBegin + l_Itr, l_ConfirmString.c_str(), 0, false);
                ++l_Itr;
            }

            p_Player->SEND_GOSSIP_MENU((p_Test ? eGossipMenus::MorphListTest : eGossipMenus::MorphListBuy), p_Creature->GetGUID());
        }

        void DisplayLootboxList(Player* p_Player, Creature* p_Creature)
        {
            p_Player->PlayerTalkClass->ClearMenus();

            uint32 l_I = 0;
            for (auto& l_Pair : m_Prices)
            {
                std::string l_Item;
                if (l_Pair.first == 1)
                    l_Item = sObjectMgr->GetFormatedTrinityString(TrinityStrings::BuyCosmeticLootBox, p_Player->GetSession()->GetSessionDbLocaleIndex(), l_Pair.first, l_Pair.second.second);
                else
                    l_Item = sObjectMgr->GetFormatedTrinityString(TrinityStrings::BuyCosmeticLootBoxes, p_Player->GetSession()->GetSessionDbLocaleIndex(), l_Pair.first, l_Pair.second.first, l_Pair.second.second);

                std::stringstream ss;
                ss << l_Item;

                std::string l_ConfirmString = sObjectMgr->GetFormatedTrinityString(TrinityStrings::ConfirmBuyCosmeticLootBox, p_Player->GetSession()->GetSessionDbLocaleIndex(), l_Pair.first, l_Pair.second.first, l_Pair.second.second);

                p_Player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, l_Item.c_str(), GOSSIP_SENDER_MAIN, m_BuyLootBoxBegin + l_I, l_ConfirmString.c_str(), 0, false);

                l_I++;
            }

            p_Player->SEND_GOSSIP_MENU(eGossipMenus::SubMenuLootBox, p_Creature->GetGUID());
        }

        void HandleBuyingProcess(Player* p_Player, CosmeticEntry p_CosmeticEntry)
        {
            uint32 l_Cost = p_CosmeticEntry.Cost;

            if (l_Cost)
            {
                WorldSession* l_Session = p_Player->GetSession();
                uint32 l_AccountId = l_Session->GetAccountId();
                l_Session->FetchFirestormPoints([p_CosmeticEntry, l_AccountId](PreparedQueryResult const& p_Result) -> void
                {
                    if (!p_Result)
                        return;

                    WorldSession* l_Session = sWorld->FindSession(l_AccountId);
                    if (!l_Session)
                        return;

                    Player* l_Player = l_Session->GetPlayer();
                    if (!l_Player)
                        return;

                    uint32 l_CosmeticID = p_CosmeticEntry.ID;
                    eCosmetics l_Cosmetic = p_CosmeticEntry.Type;
                    std::string l_LogName = p_CosmeticEntry.Locale_enUS;
                    uint32 l_Cost = p_CosmeticEntry.Cost;

                    std::string l_Name = sObjectMgr->GetCosmeticName(l_Player, p_CosmeticEntry.Type, p_CosmeticEntry.ID);

                    Field* l_Fields = p_Result->Fetch();
                    int32 l_Amount = std::atoi(l_Fields[0].GetCString());
                    if (l_Amount < static_cast<int32>(p_CosmeticEntry.Cost))
                    {
                        ChatHandler(l_Session).PSendSysMessage(TrinityStrings::NotEnoughPoints, l_Amount, p_CosmeticEntry.Cost);
                        return;
                    }

                    std::stringstream l_Ss;
                    l_Ss << "Purchased " << l_LogName << " in game";
                    l_Session->SaveCosmeticPurchase(p_CosmeticEntry);

                    l_Player->AddCosmetic(l_Cosmetic, l_CosmeticID);

                    ChatHandler(l_Session).PSendSysMessage(TrinityStrings::PurchasedCosmeticFor, l_Name.c_str(), l_Cost);
                });
            }
        }

        void HandleLootBoxBuyingProcess(Player* p_Player, uint32 p_Index)
        {
            uint32 l_Cost = m_Prices[p_Index].second.second;
            uint32 l_Count = m_Prices[p_Index].first;
            uint32 l_Bonus = m_Prices[p_Index].second.first;

            if (l_Cost)
            {
                WorldSession* l_Session = p_Player->GetSession();
                uint32 l_AccountId = l_Session->GetAccountId();
                l_Session->FetchFirestormPoints([l_Count, l_Bonus, l_Cost, l_AccountId](PreparedQueryResult const& p_Result) -> void
                {
                    if (!p_Result)
                        return;

                    WorldSession* l_Session = sWorld->FindSession(l_AccountId);
                    if (!l_Session)
                        return;

                    Player* l_Player = l_Session->GetPlayer();
                    if (!l_Player)
                        return;

                    Field* l_Fields = p_Result->Fetch();
                    int32 l_Amount = std::atoi(l_Fields[0].GetCString());
                    if (l_Amount < static_cast<int32>(l_Cost))
                    {
                        ChatHandler(l_Session).PSendSysMessage(TrinityStrings::NotEnoughPoints, l_Amount, l_Cost);
                        return;
                    }

                    l_Session->SaveCosmeticLootboxPurchase(l_Count + l_Bonus, l_Cost);

                    for (uint32 l_I = 0; l_I < l_Count + l_Bonus; ++l_I)
                    {
                        l_Player->AddItem(eItems::CosmeticLootbox, 1, {}, true, 0, false, true);
                        l_Player->SendDisplayToast(eItems::CosmeticLootbox, l_Count + l_Bonus, 0, DisplayToastMethod::DISPLAY_TOAST_METHOD_LOOT, ToastTypes::TOAST_TYPE_NEW_ITEM, false, false);
                    }

                    ChatHandler(l_Session).PSendSysMessage(TrinityStrings::PurchasedCosmeticLootboxFor, l_Count, l_Bonus, l_Cost);
                });
            }
        }
};

/// Services npc (change player level) for Fun server - 900122
class npc_fun_services : public CreatureScript
{
    public:
        npc_fun_services() : CreatureScript("npc_fun_services") { }

        enum eActions
        {
            /// Gossip select
            /// Change level
            Level1 = 1,
            Level30 = 30,
            Level50 = 50,
            Level60 = 60,
            Level70 = 70,
            Level80 = 80,
            Level90 = 90,
            Level100 = 100,
            Level110 = 110
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            /// To be sure that this npc will work only on Fun server
            if (sWorld->getIntConfig(CONFIG_START_PLAYER_LEVEL) < 110)
                return false;

            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 1.", GOSSIP_SENDER_MAIN, eActions::Level1);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 30.", GOSSIP_SENDER_MAIN, eActions::Level30);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 50.", GOSSIP_SENDER_MAIN, eActions::Level50);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 60.", GOSSIP_SENDER_MAIN, eActions::Level60);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 70.", GOSSIP_SENDER_MAIN, eActions::Level70);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 80.", GOSSIP_SENDER_MAIN, eActions::Level80);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 90.", GOSSIP_SENDER_MAIN, eActions::Level90);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 100.", GOSSIP_SENDER_MAIN, eActions::Level100);
            p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to change my level to 110.", GOSSIP_SENDER_MAIN, eActions::Level110);
            p_Player->SEND_GOSSIP_MENU(1, p_Creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            bool l_FoundItems = false;

            /// Check for items, don't continue if found any
            for (uint32 l_Slot = EquipmentSlots::EQUIPMENT_SLOT_START; l_Slot < EquipmentSlots::EQUIPMENT_SLOT_END; l_Slot++)
            {
                Item* l_Item = p_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Slot);

                if (l_Item)
                {
                    l_FoundItems = true;
                    break;
                }
            }

            if (l_FoundItems)
            {
                p_Player->GetSession()->SendNotification("%s", p_Player->GetSession()->GetTrinityString(TrinityStrings::WarnUnequipItems));
                p_Creature->Whisper(TrinityStrings::WarnUnequipItems, p_Player->GetGUID());
            }
            else
            {
                uint8 l_Level = uint8(p_Action);
                p_Player->SetLevel(l_Level);
            }

            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->PlayerTalkClass->SendCloseGossip();

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_npc_custom()
{
///    new npc_world_boss_gossip();
    ///new npc_pve_tests_manager();
    new npc_easter_egg_event();
    new npc_boris();
    ///new npc_fun_gold_vendor();
    ///new npc_fun_transmo_vendor();
    new npc_legendary_transmogrificator();
    new npc_loyalty_point();
    new npc_event_loyalty_point_two();
    new npc_event_loyalty_point();
    new npc_cosmetic_vendor();
    new npc_fun_services();
}
#endif
