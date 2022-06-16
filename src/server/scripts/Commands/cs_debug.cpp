////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: debug_commandscript
%Complete: 100
Comment: All debug related commands
Category: commandscripts
EndScriptData */

#include "Common.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.hpp"
#include "Chat.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "GossipDef.h"
#include "MapManager.h"
#include "DisableMgr.h"
#include "Group.h"
#include "LFGMgr.h"
#include "World.h"
#include "ChallengeMgr.h"
#include "POIMgr.hpp"
#include "ScenarioMgr.h"
#include "Player.h"
#include "ContributionMgr.hpp"

#ifndef CROSS
#include "InterRealmOpcodes.h"
#include "InterRealmSession.h"
#endif /* not CROSS */

#include <fstream>
#include "BattlegroundPacketFactory.hpp"

#ifdef __SANITIZE_ADDRESS__
testlul
#include <sanitizer/lsan_interface.h>
#endif

#define UNITSTATE(u) {u, #u}
std::map<UnitState, char const*> const g_Unitstates =
{
    UNITSTATE(UNIT_STATE_DIED),
    UNITSTATE(UNIT_STATE_MELEE_ATTACKING),
    UNITSTATE(UNIT_STATE_STUNNED),
    UNITSTATE(UNIT_STATE_ROAMING),
    UNITSTATE(UNIT_STATE_CHASE),
    UNITSTATE(UNIT_STATE_FLEEING),
    UNITSTATE(UNIT_STATE_IN_FLIGHT),
    UNITSTATE(UNIT_STATE_FOLLOW),
    UNITSTATE(UNIT_STATE_ROOT),
    UNITSTATE(UNIT_STATE_CONFUSED),
    UNITSTATE(UNIT_STATE_DISTRACTED),
    UNITSTATE(UNIT_STATE_ISOLATED),
    UNITSTATE(UNIT_STATE_ATTACK_PLAYER),
    UNITSTATE(UNIT_STATE_CASTING),
    UNITSTATE(UNIT_STATE_POSSESSED),
    UNITSTATE(UNIT_STATE_CHARGING),
    UNITSTATE(UNIT_STATE_JUMPING),
    UNITSTATE(UNIT_STATE_ONVEHICLE),
    UNITSTATE(UNIT_STATE_MOVE),
    UNITSTATE(UNIT_STATE_ROTATING),
    UNITSTATE(UNIT_STATE_EVADE),
    UNITSTATE(UNIT_STATE_ROAMING_MOVE),
    UNITSTATE(UNIT_STATE_CONFUSED_MOVE),
    UNITSTATE(UNIT_STATE_FLEEING_MOVE),
    UNITSTATE(UNIT_STATE_CHASE_MOVE),
    UNITSTATE(UNIT_STATE_FOLLOW_MOVE),
    UNITSTATE(UNIT_STATE_UNATTACKABLE),
    UNITSTATE(UNIT_STATE_MOVING),
    UNITSTATE(UNIT_STATE_CONTROLLED),
    UNITSTATE(UNIT_STATE_LOST_CONTROL),
    UNITSTATE(UNIT_STATE_SIGHTLESS),
    UNITSTATE(UNIT_STATE_CANNOT_AUTOATTACK),
    UNITSTATE(UNIT_STATE_CANNOT_TURN),
    UNITSTATE(UNIT_STATE_NOT_MOVE),
    UNITSTATE(UNIT_STATE_ALL_STATE)
};

class debug_commandscript: public CommandScript
{
    public:

        debug_commandscript() : CommandScript("debug_commandscript")
        {
        }

        ChatCommand* GetCommands() const
        {
            static ChatCommand debugPlayCommandTable[] =
            {
                { "cinematic",      SEC_MODERATOR,          SEC_MODERATOR,      false, &HandleDebugPlayCinematicCommand,   "", NULL },
                { "movie",          SEC_MODERATOR,          SEC_MODERATOR,      false, &HandleDebugPlayMovieCommand,       "", NULL },
                { "sound",          SEC_MODERATOR,          SEC_MODERATOR,      false, &HandleDebugPlaySoundCommand,       "", NULL },
                { "scene",          SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugPlaySceneCommand,       "", NULL },
                { "sscene",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugPlaySSceneCommand,      "", NULL },
                { "oneshotanimkit", SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugPlayOneShotAnimKit,     "", NULL },
                { "spellvisualkit", SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugPlaySpellVisualKit,     "", NULL },
                { "orphanvisual",   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugPlayOrphanSpellVisual,  "", NULL },
                { "visual",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugPlayVisual,             "", NULL },
                { "conversation",   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugPlayConversation,       "", NULL },
                { "cosmeticevent",  SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugPlayCosmeticEvent,      "", NULL },
                { NULL,             SEC_PLAYER,             SEC_PLAYER,         false, NULL,                               "", NULL }
            };
            static ChatCommand debugSendCommandTable[] =
            {
                { "buyerror",       SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendBuyErrorCommand,       "", NULL },
                { "channelnotify",  SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendChannelNotifyCommand,  "", NULL },
                { "chatmmessage",   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendChatMsgCommand,        "", NULL },
                { "equiperror",     SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendEquipErrorCommand,     "", NULL },
                { "largepacket",    SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendLargePacketCommand,    "", NULL },
                { "opcode",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendOpcodeCommand,         "", NULL },
                { "qpartymsg",      SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendQuestPartyMsgCommand,  "", NULL },
                { "qinvalidmsg",    SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendQuestInvalidMsgCommand,"", NULL },
                { "sellerror",      SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendSellErrorCommand,      "", NULL },
                { "setphaseshift",  SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendSetPhaseShiftCommand,  "", NULL },
                { "spellfail",      SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendSpellFailCommand,      "", NULL },
                { "playerchoice",   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleDebugSendPlayerChoiceCommand,   "", NULL },
                { NULL,             SEC_PLAYER,             SEC_PLAYER,         false, NULL,                                  "", NULL }
            };
            static ChatCommand debugCommandTable[] =
            {
                { "setbit",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSet32BitCommand,             "", NULL },
                { "threat",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugThreatListCommand,           "", NULL },
                { "hostil",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugHostileRefListCommand,       "", NULL },
                { "anim",                        SEC_GAMEMASTER,            SEC_GAMEMASTER,                 false, &HandleDebugAnimCommand,                 "", NULL },
                { "arena",                       SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugArenaCommand,                "", NULL },
                { "bg",                          SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugBattlegroundCommand,         "", NULL },
                { "getitemstate",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugGetItemStateCommand,         "", NULL },
                { "getvalue",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugGetValueCommand,             "", NULL },
                { "getitemvalue",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugGetItemValueCommand,         "", NULL },
                { "Mod32Value",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugMod32ValueCommand,           "", NULL },
                { "play",                        SEC_MODERATOR,             SEC_MODERATOR,                  false, NULL,                                    "", debugPlayCommandTable },
                { "send",                        SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, NULL,                                    "", debugSendCommandTable },
                { "setaurastate",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSetAuraStateCommand,         "", NULL },
                { "setitemvalue",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSetItemValueCommand,         "", NULL },
                { "setvalue",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSetValueCommand,             "", NULL },
                { "spawnvehicle",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSpawnVehicleCommand,         "", NULL },
                { "setvid",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSetVehicleIdCommand,         "", NULL },
                { "entervehicle",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugEnterVehicleCommand,         "", NULL },
                { "uws",                         SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugUpdateWorldStateCommand,     "", NULL },
                { "evalworldstateexp",           SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugEvalWorldStateExpCommand,    "", NULL },
                { "update",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugUpdateCommand,               "", NULL },
                { "itemexpire",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugItemExpireCommand,           "", NULL },
                { "areatriggers",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugAreaTriggersCommand,         "", NULL },
                { "los",                         SEC_MODERATOR,             SEC_MODERATOR,                  false, &HandleDebugLoSCommand,                  "", NULL },
                { "phase",                       SEC_MODERATOR,             SEC_MODERATOR,                  false, &HandleDebugPhaseCommand,                "", NULL },
                { "tradestatus",                 SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleSendTradeStatus,                  "", NULL },
                { "mailstatus",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleSendMailStatus,                   "", NULL },
                { "jump",                        SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugMoveJump,                    "", NULL },
                { "backward",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugMoveBackward,                "", NULL },
                { "load_z",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugLoadZ,                       "", NULL },
                { "joinratedbg",                 SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleJoinRatedBg,                      "", NULL },
                { "SetMaxMapDiff",               SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSetMaxMapDiff,               "", NULL },
                { "packet",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugPacketCommand,               "", NULL },
                { "guildevent",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugGuildEventCommand,           "", NULL },
                { "log",                         SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugLogCommand,                  "", NULL },
                { "movement",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugMoveCommand,                 "", NULL },
                { "lfg",                         SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugLfgCommand,                  "", NULL },
                { "at",                          SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugATCommand,                   "", NULL },
                { "wp",                          SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugWPCommand,                   "", NULL },
                { "atpolygon",                   SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugATPolygonCommand,            "", NULL },
                { "pvptemplate",                 SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugPvPTemplateCommand,          "", NULL },
                { "toy",                         SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugToyCommand,                  "", NULL },
                { "charge",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugClearSpellCharges,           "", NULL },
                { "bgstart",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugBattlegroundStart,           "", NULL },
                { "crashtest",                   SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              true,  &HandleDebugCrashTest,                   "", NULL },
                { "bgaward",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugBgAward,                     "", NULL },
                { "heirloom",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugHeirloom,                    "", NULL },
                { "vignette",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugVignette,                    "", NULL },
                { "setaianimkit",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSetAIAnimKit,                "", NULL },
                { "dumpchartemplate",            SEC_CONSOLE,               SEC_CONSOLE,                    true,  &HandleDebugDumpCharTemplate,            "", NULL },
                { "dumprewardlessmissions",      SEC_CONSOLE,               SEC_CONSOLE,                    true,  &HandleDebugDumpRewardlessMissions,      "", NULL },
                { "dumpspellrewardlessmissions", SEC_CONSOLE,               SEC_CONSOLE,                    true,  &HandleSpellDebugDumpRewardlessMissions, "", NULL },
                { "playercondition",             SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugPlayerCondition,             "", NULL },
                { "packetprofiler",              SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugPacketProfiler,              "", NULL },
                { "hotfix",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleHotfixOverride,                   "", NULL },
                { "adjustspline",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugAdjustSplineCommand,         "", NULL },
                { "splinesync",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSplineSyncCommand,           "", NULL },
                { "mirror",                      SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugMirrorCommand,               "", NULL },
                { "pvelogs",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugPvELogsCommand,              "", NULL },
                { "questlog",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugQuestLogsCommand,            "", NULL },
                { "addunitstate",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugAddUnitStateCommand,         "", NULL },
                { "getunitstate",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugGetUnitStatesCommand,        "", NULL },
                { "removeunitstate",             SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugRemoveUnitStateCommand,      "", NULL },
                { "setinstancesize",             SEC_GM_LEADER,             SEC_GM_LEADER,                  false, &HandleDebugSetInstanceSizeCommand,      "", NULL },

#ifndef CROSS
                { "stresstest",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              true,  &HandleDebugStressTestCommand,           "", NULL },
#endif /* not CROSS */
                { "showequiperror",              SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugShowEquipErrorCommand,       "", NULL },
                { "critical",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugCriticalCommand,             "", NULL },
                { "haste",                       SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugHasteCommand,                "", NULL },
                { "mastery",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugMasteryCommand,              "", NULL },
                { "versatility",                 SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugVersatilityCommand,          "", NULL },
                { "dodge",                       SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugDodgeCommand,                "", NULL },
                { "setaura",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugAuraCommand,                 "", NULL },
                { "cleardr",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugCancelDiminishingReturn,     "", NULL },
                { "scenario",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugScenarioCommand,             "", NULL },
                { "rewardpack",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugRewardPack,                  "", NULL },
                { "dailypoint",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugDailyPointCommand,           "", NULL },
                { "mesuremapdiff",               SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugMesureMapDiffCommand,        "", NULL },
                { "curvevalue",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugCurveValueCommand,           "", NULL },
                { "logdr",                       SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugLogDRCommand,                "", NULL },
                { "artefact",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugArtefactCommand,             "", NULL },
                { "forcechallengerotation",      SEC_MODERATOR,             SEC_MODERATOR,                  false, &HandleDebugForceChallengeRotation,      "", NULL },
                { "updatekeystone",              SEC_MODERATOR,             SEC_MODERATOR,                  false, &HandleDebugUpdateKeystone,              "", NULL },
                { "updatekeystonemap",           SEC_MODERATOR,             SEC_MODERATOR,                  false, &HandleDebugUpdateKeyStoneCurrentMap,    "", NULL },
                { "forceclasshallchest",         SEC_MODERATOR,             SEC_MODERATOR,                  false, &HandleDebugForceClassHallChest,         "", NULL },
                { "generatearenareward",         SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugGenerateArenarRewardCommand, "", NULL },
                { "forceexitvehicle",            SEC_MODERATOR,             SEC_MODERATOR,                  false, &HandleDebugForceExitVehicle,            "", NULL },
                { "lootbox",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugLootBoxCommand,              "", NULL },
                { "bossloot",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugBossLootCommand,             "", NULL },
                { "pvploot",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugPvpLootCommand,              "", NULL },
                { "cosevent",             SEC_CONFIRMED_GAMEMASTER,         SEC_CONFIRMED_GAMEMASTER,       false, &HandleDebugCosEvent,                    "", NULL },
                { "areapoiupdate",               SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugAreaPoiUpdate,               "", NULL },
                { "addareapoi",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugAddAreaPoi,                  "", NULL },
                { "endareapoi",                  SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugEndAreaPoi,                  "", NULL },
                { "showscenariostepcriteria",    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleShowScenarioStepCriteriaCommand,  "", NULL },
                { "contribution",                SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugContributionCommand,         "", NULL },
                { "showachievementcriteria",     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleShowAchievementCriteriaCommand,   "", NULL },
                { "position",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugPositionCommand,             "", NULL },
                { "sendeventscenario",           SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugSendEventScenarioCommand,    "", NULL },
                { "grids",                       SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugGridsCommand,                "", NULL },
                { "criteria",                    SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugCriteriaCommand,             "", NULL },
                { "argus",                       SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugArgusCommand,                "", NULL },
                { "eonar",                       SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugEonarCommand,                "", NULL },
                { "checkgrouprole",              SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugCheckGroupRolesCommand,      "", NULL },
                { "mapinfo",                     SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              false, &HandleDebugMapInfoCommand,              "", NULL },
                { "showleaks",                   SEC_ADMINISTRATOR,         SEC_ADMINISTRATOR,              true,  &HandleShowLeaksCommand,                 "", NULL },
                { NULL,                          SEC_PLAYER,                SEC_PLAYER,                     false, NULL,                                    "", NULL }
            };
            static ChatCommand commandTable[] =
            {
                { "debug",          SEC_MODERATOR,          SEC_MODERATOR,      true,  NULL,                  "", debugCommandTable },
                { "wpgps",          SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleWPGPSCommand,                "", NULL },
                { NULL,             SEC_PLAYER,             SEC_PLAYER,         false, NULL,                  "",              NULL }
            };
            return commandTable;
        }

        static bool HandleDebugMapInfoCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            if (!l_Player)
                return false;

            uint32 l_MapID        = l_Player->GetMap()->GetId();
            bool l_HaveOutdoorPvP = l_Player->GetMap()->GetOutdoorPvP() != nullptr;
            bool l_HaveOutdoorPvE = l_Player->GetMap()->GetOudoorPvE() != nullptr;
            bool l_IsClosed       = l_Player->GetMap()->IsClosed();
            uint32 l_ScenarioGuid = l_Player->GetMap()->GetScenarioGuid();
            auto l_Scenario       = sScenarioMgr->GetScenario(l_ScenarioGuid);
            uint32 l_InstanceZoneId = l_Player->GetMap()->GetInstanceZoneId();

            ChatHandler(l_Player).PSendSysMessage("MapId : %u, OutdoorPvP : %u, OutdoorPvE : %u, IsClosed : %u, ScenarioGuid : %u, l_Scenario : %u, InstanceZoneId : %u", l_MapID, l_HaveOutdoorPvP, l_HaveOutdoorPvE, l_IsClosed, l_ScenarioGuid, l_Scenario != nullptr, l_InstanceZoneId);
            return true;
        }

        static bool HandleDebugEonarCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
            {
                if (Creature* l_Eonar = Creature::GetCreature(*l_Player, l_Instance->GetData64(122500)))
                {
                    if (l_Eonar->IsAIEnabled)
                        l_Eonar->AI()->DoAction(5);
                }
            }

            return true;
        }

        static bool HandleDebugArgusCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
            {
                if (Creature* l_Argus = Creature::GetCreature(*l_Player, l_Instance->GetData64(124828)))
                {
                    if (l_Argus->IsAIEnabled)
                        l_Argus->AI()->EnterEvadeMode();
                }
            }

            return true;
        }

        static bool HandleDebugPositionCommand(ChatHandler* handler, char const* /*args*/)
        {
            Unit* target = handler->getSelectedUnit();

            if (!target || !handler->GetSession()->GetPlayer()->GetSelection())
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (target->IsPlayer())
                return false;

            if (!target->isAlive())
                return false;

            target->ToggleDebugPosition();
            return true;
        }

        static bool HandleDebugCriteriaCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            char* l_Criteria = strtok((char*)p_Args, " ");
            if (!l_Criteria)
                return false;

            uint32 l_CriteriaType = atoi(l_Criteria);

            p_Handler->GetSession()->GetPlayer()->UpdateCriteria(CriteriaTypes(l_CriteriaType), 1);
            return true;
        }

        static bool HandleDebugGridsCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            float size = 50.0f;
            if (*p_Args)
                size = float(atoi(p_Args));

            Player* player = p_Handler->GetSession()->GetPlayer();
            //10045

            Map* l_Map = player->GetMap();
            float l_Step = 5.0f;

            float l_CurrX = player->m_positionX - size;
            float l_CurrY = player->m_positionY - size;

            std::set<uint64> l_SpawnedCells;
            std::set<uint64> l_SpawnedGrids;

            auto l_Convert = [](float axisCoord) -> float { return axisCoord * SIZE_OF_GRID_CELL - MAP_HALFSIZE; };
            auto l_ConvertGrid = [](float axisCoord) -> float { return axisCoord * SIZE_OF_GRIDS - MAP_HALFSIZE; };

            /// Process X axis
            do
            {
                /// Process Y axis
                do
                {
                    CellCoord l_CellCoord = JadeCore::ComputeCellCoord(l_CurrX, l_CurrY);
                    if (l_SpawnedCells.find(MAKE_PAIR64(l_CellCoord.x_coord, l_CellCoord.y_coord)) == l_SpawnedCells.end())
                    {
                        l_SpawnedCells.insert(MAKE_PAIR64(l_CellCoord.x_coord, l_CellCoord.y_coord));
                        for (float l_Z = player->m_positionZ - 5.0f; l_Z < player->m_positionZ + 5.0f; l_Z += 5.0f)
                        {
                            for (float l_X = l_Convert(l_CellCoord.x_coord); l_X < l_Convert(l_CellCoord.x_coord + 1); l_X += 2.0f)
                            {
                                player->SummonCreature(10045, l_X, l_Convert(l_CellCoord.y_coord), l_Z);
                                player->SummonCreature(10045, l_X, l_Convert(l_CellCoord.y_coord + 1), l_Z+2.5f);
                            }
                            for (float l_Y = l_Convert(l_CellCoord.y_coord); l_Y < l_Convert(l_CellCoord.y_coord + 1); l_Y += 2.0f)
                            {
                                player->SummonCreature(10045, l_Convert(l_CellCoord.x_coord), l_Y, l_Z);
                                player->SummonCreature(10045, l_Convert(l_CellCoord.x_coord + 1), l_Y, l_Z+2.5f);
                            }
                        }
                    }
                    GridCoord l_GridCoord = JadeCore::ComputeGridCoord(l_CurrX, l_CurrY);
                    if (l_SpawnedGrids.find(MAKE_PAIR64(l_GridCoord.x_coord, l_GridCoord.y_coord)) == l_SpawnedGrids.end())
                    {
                        l_SpawnedGrids.insert(MAKE_PAIR64(l_GridCoord.x_coord, l_GridCoord.y_coord));
                        for (float l_Z = player->m_positionZ + 5.0f; l_Z < player->m_positionZ + 10.0f; l_Z += 5.0f)
                        {
                            for (float l_X = l_ConvertGrid(l_GridCoord.x_coord); l_X < l_ConvertGrid(l_GridCoord.x_coord + 1); l_X += 2.0f)
                            {
                                player->SummonCreature(10053, l_X, l_ConvertGrid(l_GridCoord.y_coord), l_Z);
                                player->SummonCreature(10053, l_X, l_ConvertGrid(l_GridCoord.y_coord + 1), l_Z + 2.5f);
                            }
                            for (float l_Y = l_ConvertGrid(l_GridCoord.y_coord); l_Y < l_ConvertGrid(l_GridCoord.y_coord + 1); l_Y += 2.0f)
                            {
                                player->SummonCreature(10053, l_ConvertGrid(l_GridCoord.x_coord), l_Y, l_Z);
                                player->SummonCreature(10053, l_ConvertGrid(l_GridCoord.x_coord + 1), l_Y, l_Z + 2.5f);
                            }
                        }
                    }

                    l_CurrY += l_Step;
                } while (l_CurrY <  player->m_positionY + size);

                l_CurrY = player->m_positionY - size;
                l_CurrX += l_Step;
            } while (l_CurrX < player->m_positionX + size);

            return true;
        }

        static bool HandleDebugContributionCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            char* p_ContributionID = strtok((char*)p_Args, " ");
            char* p_State = strtok(NULL, " ");
            char* p_Progress = strtok(NULL, " ");
            char* p_OccurenceValue = strtok(NULL, " ");

            if (!p_ContributionID || !p_State || !p_Progress || !p_OccurenceValue)
                return false;

            uint32 l_ContributionID = (uint32)atoi(p_ContributionID);
            uint32 l_State          = (uint32)atoi(p_State);
            uint32 l_Progress       = (uint32)atoi(p_Progress);
            uint32 l_OccurenceValue = (uint32)atoi(p_OccurenceValue);

            sContributionMgr->DebugContribution(l_ContributionID, l_State, l_Progress, l_OccurenceValue);

            return true;
        }

        static bool HandleDebugSendPlayerChoiceCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            int32 choiceId = atoi(p_Args);
            Player* player = p_Handler->GetSession()->GetPlayer();

            player->SendPlayerChoice(player->GetGUID(), choiceId);
            return true;
        }

        static bool HandleDebugAreaPoiUpdate(ChatHandler* p_Handler, char const* p_Args)
        {
            WorldSession* l_Session = p_Handler->GetSession();
            WorldPacket l_bab;
            l_Session->HandleRequestAreaPOIUpdate(l_bab);
            return true;
        }

        static bool HandleDebugAddAreaPoi(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            uint32 l_AreaPoiId = atoi((char*)p_Args);

            sPOIMgr->AddEventForPlayer(l_AreaPoiId, l_Player->GetGUID());
            return true;
        }

        static bool HandleDebugEndAreaPoi(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            uint32 l_AreaPoiId = atoi((char*)p_Args);

            sPOIMgr->EndEventForPlayer(l_AreaPoiId, l_Player->GetGUID());
            return true;
        }

        static std::string GetInventoryTypeString(uint32 p_InventoryType)
        {
            switch (p_InventoryType)
            {
                case INVTYPE_NON_EQUIP:      return "INVTYPE_NON_EQUIP";
                case INVTYPE_HEAD:           return "INVTYPE_HEAD";
                case INVTYPE_NECK:           return "INVTYPE_NECK";
                case INVTYPE_SHOULDERS:      return "INVTYPE_SHOULDERS";
                case INVTYPE_BODY:           return "INVTYPE_BODY";
                case INVTYPE_CHEST:          return "INVTYPE_CHEST";
                case INVTYPE_WAIST:          return "INVTYPE_WAIST";
                case INVTYPE_LEGS:           return "INVTYPE_LEGS";
                case INVTYPE_FEET:           return "INVTYPE_FEET";
                case INVTYPE_WRISTS:         return "INVTYPE_WRISTS";
                case INVTYPE_HANDS:          return "INVTYPE_HANDS";
                case INVTYPE_FINGER:         return "INVTYPE_FINGER";
                case INVTYPE_TRINKET:        return "INVTYPE_TRINKET";
                case INVTYPE_WEAPON:         return "INVTYPE_WEAPON";
                case INVTYPE_SHIELD:         return "INVTYPE_SHIELD";
                case INVTYPE_RANGED:         return "INVTYPE_RANGED";
                case INVTYPE_CLOAK:          return "INVTYPE_CLOAK";
                case INVTYPE_2HWEAPON:       return "INVTYPE_2HWEAPON";
                case INVTYPE_BAG:            return "INVTYPE_BAG";
                case INVTYPE_TABARD:         return "INVTYPE_TABARD";
                case INVTYPE_ROBE:           return "INVTYPE_ROBE";
                case INVTYPE_WEAPONMAINHAND: return "INVTYPE_WEAPONMAINHAND";
                case INVTYPE_WEAPONOFFHAND:  return "INVTYPE_WEAPONOFFHAND;";
                case INVTYPE_HOLDABLE:       return "INVTYPE_HOLDABLE";
                case INVTYPE_AMMO:           return "INVTYPE_AMMO";
                case INVTYPE_THROWN:         return "INVTYPE_THROWN";
                case INVTYPE_RANGEDRIGHT:    return "INVTYPE_RANGEDRIGHT";
                case INVTYPE_QUIVER:         return "INVTYPE_QUIVER";
                case INVTYPE_RELIC:          return "INVTYPE_RELIC";
                default:                     return "INVTYPE_UNK";
            }
        }

        static bool HandleDebugCosEvent(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player    = p_Handler->GetSession()->GetPlayer();
            uint32 l_EventIndex = atoi((char*)p_Args);

            if (l_Player == nullptr || l_Player->GetMapId() != 1571 || l_EventIndex > 7) ///< Court of Stars dungeon
                return false;

            /*
                Event Index explanations:

                Index 0 : NpcArcanePowerConduit,
                Index 1 : NpcBazaarGoods,
                Index 2 : NpcLifesizedNightborneStatue,
                Index 3 : NpcWoundedNightborneCivilian,
                Index 4 : NpcFelOrb,
                Index 5 : NpcInfernalTome,
                Index 6 : NpcMagicalLantern,
                Index 7 : NpcWaterloggedScroll
            */

            if (l_Player->GetInstanceScript())
                l_Player->GetInstanceScript()->SetData(6, l_EventIndex);

            return true;
        }

        static bool HandleDebugForceExitVehicle(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            if (l_Player == nullptr)
                return false;

            l_Player->ExitVehicle(&*l_Player);

            return true;
        }

        static bool HandleDebugPvpLootCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            char* l_StrCount = strtok((char*)p_Args, " ");
            uint32 l_Count = atoi(l_StrCount);

            std::map<uint32, uint32> l_ItemTypeCount;
            for (int l_I = 0; l_I < l_Count; l_I++)
            {
                auto l_Reward = sObjectMgr->GetItemRewardForPvp(l_Player->GetActiveSpecializationID(), MS::Battlegrounds::RewardGearType::Regular, l_Player);
                if (!l_Reward)
                    continue;

                if (l_ItemTypeCount.find(l_Reward->InventoryType) == l_ItemTypeCount.end())
                    l_ItemTypeCount[l_Reward->InventoryType] = 0;

                l_ItemTypeCount[l_Reward->InventoryType]++;
            }

            for (auto l_Itr : l_ItemTypeCount)
            {
                std::string l_Msg = GetInventoryTypeString(l_Itr.first) + " : " + std::to_string(l_Itr.second);
                p_Handler->SendSysMessage(l_Msg.c_str());
            }

            return true;
        }

        static bool HandleDebugBossLootCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            char* l_StrEntry = strtok((char*)p_Args, " ");
            if (l_StrEntry == nullptr)
                return false;

            char* l_StrIsGoB = strtok(nullptr, " ");

            uint32 l_Entry = atoi(l_StrEntry);
            bool l_IsGob = false;
            if (l_StrIsGoB != nullptr)
                l_IsGob = atoi(l_StrIsGoB) != 0;

            std::map<uint32, uint32> l_LootCounts;
            uint32 l_TotalItemCount = 0;

            if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
            {
                if (l_IsGob)
                {
                    GameObject* l_Gob = GameObject::GetGameObject(*l_Player, l_Instance->GetData64(l_Entry));
                    if (l_Gob == nullptr)
                        l_Gob = l_Player->FindNearestGameObject(l_Entry, 500.0f);

                    if (l_Gob != nullptr)
                    {
                        for (uint32 l_I = 0; l_I < 1000; ++l_I)
                        {
                            Loot*  l_Loot = nullptr;
                            uint64 l_LootContainerGuid = l_Player->GetGUID();

                            l_Loot = &l_Gob->m_LootContainers[l_LootContainerGuid];
                            l_Loot->m_PersonalLootFilter = true;
                            l_Loot->SetSource(l_Gob->GetGUID());
                            l_Loot->Context = l_Instance->instance->GetLootItemContext(l_Player);
                            l_Loot->SpawnMode = l_Instance->instance->GetSpawnMode();

                            if (uint32 l_LootID = l_Gob->GetGOInfo()->GetLootId())
                                l_Loot->FillLoot(l_LootID, LootTemplates_Gameobject, l_Player, false, false, l_Gob->GetLootMode());

                            for (auto l_Iter : l_Loot->Items)
                            {
                                ++l_TotalItemCount;

                                if (l_LootCounts.find(l_Iter.itemid) == l_LootCounts.end())
                                    l_LootCounts[l_Iter.itemid] = 1;
                                else
                                    l_LootCounts[l_Iter.itemid]++;
                            }

                            l_Gob->m_LootContainers.clear();
                        }
                    }
                }
                else
                {
                    Creature* l_Boss = Creature::GetCreature(*l_Player, l_Instance->GetData64(l_Entry));
                    if (l_Boss == nullptr)
                        l_Boss = l_Player->FindNearestCreature(l_Entry, 500.0f);

                    if (l_Boss != nullptr)
                    {
                        for (uint32 l_I = 0; l_I < 1000; ++l_I)
                        {
                            Loot*  l_Loot = nullptr;
                            uint64 l_LootContainerGuid = l_Player->GetGUID();

                            l_Loot = &l_Boss->m_LootContainers[l_LootContainerGuid];
                            l_Loot->m_PersonalLootFilter = true;
                            l_Loot->SetSource(l_Boss->GetGUID());
                            l_Loot->Context = l_Instance->instance->GetLootItemContext(l_Player);
                            l_Loot->SpawnMode = l_Instance->instance->GetSpawnMode();

                            if (uint32 l_LootID = l_Boss->GetCreatureTemplate()->lootid)
                                l_Loot->FillLoot(l_LootID, LootTemplates_Creature, l_Player, false, false, l_Boss->GetLootMode());

                            uint32 l_MinGold = l_Boss->GetCreatureTemplate()->mingold;
                            uint32 l_MaxGold = l_Boss->GetCreatureTemplate()->maxgold;

                            l_Boss->GenerateMinMaxGold(l_MinGold, l_MaxGold);
                            l_Loot->generateMoneyLoot(l_MinGold, l_MaxGold);

                            for (auto l_Iter : l_Loot->Items)
                            {
                                ++l_TotalItemCount;

                                if (l_LootCounts.find(l_Iter.itemid) == l_LootCounts.end())
                                    l_LootCounts[l_Iter.itemid] = 1;
                                else
                                    l_LootCounts[l_Iter.itemid]++;
                            }

                            l_Boss->ClearLootContainers();
                        }
                    }
                }
            }

            if (l_IsGob)
                p_Handler->PSendSysMessage("Processing 1000 loots for gameobject %u...", l_Entry);
            else
                p_Handler->PSendSysMessage("Processing 1000 loots for creature %u...", l_Entry);

            uint32 l_TotalCount = 0;
            for (auto l_Iter : l_LootCounts)
            {
                uint32 l_ItemID = l_Iter.first;
                float l_Percentage = float(l_Iter.second) / 1000.0f * 100.0f;

                l_TotalCount += l_Iter.second;

                if (ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(l_ItemID))
                    p_Handler->PSendSysMessage("Item %u (%s), looted %u times (%.2f percent)", l_ItemID, l_Template->Name1->Get(sWorld->GetDefaultDb2Locale()), l_Iter.second, l_Percentage);
            }

            p_Handler->PSendSysMessage("Total looted items: %u over 1000 tries, that's a %.2f overall drop rate", l_TotalCount, float(float(l_TotalCount) / 1000.0f * 100.0f));
            return true;
        }

        static void GiveLootBoxFiller(Player* p_Player)
        {
            /// Money
            uint64 l_Money = urand(500, 3000) * GOLD;
            p_Player->ModifyMoney(l_Money, "lootbox");
            p_Player->SendDisplayToast(0, l_Money, 0, DISPLAY_TOAST_METHOD_CURRENCY_OR_GOLD, TOAST_TYPE_MONEY, false, false);

            /// AP
            uint32 l_APItems[5] = { 140396, 140384, 141701, 141854, 141852 };
            uint8 l_I = 0;

            while (l_I < 4 && urand(0, 100) > 50)
                l_I++;

            p_Player->AddItem(l_APItems[l_I], 1);
            p_Player->SendDisplayToast(l_I, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            /// Sargeras
            uint32 l_Count = urand(1, 5);
            p_Player->AddItem(124124, l_Count);
            p_Player->SendDisplayToast(l_I, l_Count, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);

            /// Legendary roll
            p_Player->CompleteLegendaryActivity(LegendaryActivity::LootBox);
        }

        static void GiveRandomMount(Player* p_Player)
        {
            std::vector<uint32> l_Mounts;

            for (uint32 l_I = 0; l_I < sMountStore.GetNumRows(); l_I++)
            {
                MountEntry const* l_Mount = sMountStore.LookupEntry(l_I);
                if (!l_Mount || l_Mount->PlayerConditionID)
                    continue;

                l_Mounts.push_back(l_Mount->SpellID);
            }

            uint32 l_SpellId = l_Mounts[rand() % (l_Mounts.size() - 1)];

            if (p_Player->HasSpell(l_SpellId))
                GiveLootBoxFiller(p_Player);
            else
                p_Player->learnSpell(l_SpellId, false);
        }

        static void GiveRandomGear(Player* p_Player)
        {
            uint32 l_DungeonMapLoot = JadeCore::Containers::SelectRandomContainerElement(*GetChallengeMaps());
            std::vector<uint32> l_PossibleLoots = GetItemLootPerMap(l_DungeonMapLoot);

            std::random_device l_RandomDevice;
            std::mt19937 l_RandomGenerator(l_RandomDevice());
            std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

            for (uint32 l_ItemId : l_PossibleLoots)
            {
                ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
                if (!l_ItemTpl)
                    continue;

                if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                    continue;

                p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, true, 0, true, true, ItemContext::DungeonMythic);
                break;
            }
        }

        static bool HandleDebugLootBoxCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            if (!l_Player)
                return false;

            l_Player->m_IsInLootboxProcess = true;

            if (std::string(p_Args) == std::string("random"))
            {
                GiveLootBoxFiller(l_Player);

                if (urand(0, 100) < 30)
                    GiveRandomMount(l_Player);

                if (urand(0, 100) < 20)
                    GiveRandomGear(l_Player);
            }

            if (std::string(p_Args) == std::string("gear"))
            {
                GiveLootBoxFiller(l_Player);
                GiveRandomGear(l_Player);

                /// Gear box have a extra legendary roll
                l_Player->CompleteLegendaryActivity(LegendaryActivity::LootBox);
            }

            if (std::string(p_Args) == std::string("mount"))
            {
                GiveLootBoxFiller(l_Player);
                GiveRandomMount(l_Player);

                /// Mount box have a extra legendary roll
                l_Player->CompleteLegendaryActivity(LegendaryActivity::LootBox);
            }

            l_Player->m_IsInLootboxProcess = false;

            return true;
        }

        static bool HandleDebugForceClassHallChest(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            uint32 l_ChallengeLevel = atoi((char*)p_Args);

            if (l_ChallengeLevel > ChallengeLevels::Max || !l_Player)
                return false;

            sChallengeMgr->DeleteOploteLoot(l_Player->GetGUID(), false);
            sChallengeMgr->GenerateOploteLoot(l_Player->GetGUID(), l_ChallengeLevel);

            return true;
        }

        static bool HandleDebugGenerateArenarRewardCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            Battleground* l_Battleground = l_Player->GetBattleground();
            if (!l_Battleground)
                return false;

            l_Battleground->RewardPlayerWithOtherRewards(l_Player, true);
            return true;
        }

        static bool HandleDebugUpdateKeyStoneCurrentMap(ChatHandler* p_Handler, char const* p_Args)
        {
            if (p_Handler == nullptr || p_Args == nullptr)
                return false;

            if (!*p_Args)
                return false;

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            uint32 l_ChallengeLevel = atoi(const_cast<char*>(p_Args));

            if (l_ChallengeLevel > ChallengeLevels::Max || l_Player == nullptr)
                return false;

            Map* l_Map = l_Player->GetMap();
            Item* l_Item = l_Player->GetItemByEntry(138019);

            if (!l_Item || l_Map == nullptr)
                return false;

            MapChallengeModeEntry const* l_MapChallenge = nullptr;
            MapChallengeModeEntryContainer const & l_ChallengeDungeons = *GetLegionChallengeModeEntries();

            auto l_Itr = l_ChallengeDungeons.find(l_Map->GetId());

            if (l_Itr == l_ChallengeDungeons.end())
                return false;

            if (l_Itr->second == nullptr)
                return false;

            l_MapChallenge = l_Itr->second;

            l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID, l_MapChallenge->ID);
            l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL, l_ChallengeLevel);

            if (l_ChallengeLevel > 3)
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1, sChallengeMgr->GetCurrentAffixes()[0]);
            else
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1, 0);

            if (l_ChallengeLevel > 6)
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2, sChallengeMgr->GetCurrentAffixes()[1]);
            else
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2, 0);

            if (l_ChallengeLevel > 9)
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3, sChallengeMgr->GetCurrentAffixes()[2]);
            else
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3, 0);

            l_Item->SetState(ITEM_CHANGED, l_Player);

            return true;
        }

        static bool HandleDebugUpdateKeystone(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            uint32 l_ChallengeLevel = atoi((char*)p_Args);

            if (l_ChallengeLevel > ChallengeLevels::Max || !l_Player)
                return false;

            Item* l_Item = l_Player->GetItemByEntry(138019);
            if (!l_Item)
                return false;

            MapChallengeModeEntry const* l_MapChallenge = nullptr;
            while (!l_MapChallenge)
                l_MapChallenge = JadeCore::Containers::SelectRandomContainerElement(*GetLegionChallengeModeEntries()).second;

            l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID, l_MapChallenge->ID);
            l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL, l_ChallengeLevel);

            if (l_ChallengeLevel > 3)
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1, sChallengeMgr->GetCurrentAffixes()[0]);
            else
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1, 0);

            if (l_ChallengeLevel > 6)
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2, sChallengeMgr->GetCurrentAffixes()[1]);
            else
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2, 0);

            if (l_ChallengeLevel > 9)
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3, sChallengeMgr->GetCurrentAffixes()[2]);
            else
                l_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3, 0);

            l_Item->SetState(ITEM_CHANGED, l_Player);

            return true;
        }

        static bool HandleDebugForceChallengeRotation(ChatHandler* p_Handler, char const* p_Args)
        {
            sChallengeMgr->GenerateCurrentWeekAffixes();
            return true;
        }

        static bool HandleDebugArtefactCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            std::map<uint8, std::vector<uint32>> l_ArtifactsPerClass =
            {
                {
                    Classes::CLASS_WARRIOR,
                    { 128289, 128908, 128910 }
                },
                {
                    Classes::CLASS_PALADIN,
                    { 128823, 128866, 120978 }
                },
                {
                    Classes::CLASS_HUNTER,
                    { 128861, 128826, 128808 }
                },
                {
                    Classes::CLASS_ROGUE,
                    { 128476, 128872, 128870 }
                },
                {
                    Classes::CLASS_PRIEST,
                    { 128827, 128825, 128868 }
                },
                {
                    Classes::CLASS_DEATH_KNIGHT,
                    { 128402, 128292, 128403 }
                },
                {
                    Classes::CLASS_SHAMAN,
                    { 128911, 128819, 128935 }
                },
                {
                    Classes::CLASS_MAGE,
                    { 127857, 128820, 128862 }
                },
                {
                    Classes::CLASS_WARLOCK,
                    { 128941, 128943, 128942 }
                },
                {
                    Classes::CLASS_MONK,
                    { 128938, 128937, 128940 }
                },
                {
                    Classes::CLASS_DRUID,
                    { 128858, 128860, 128821, 128306 }
                },
                {
                    Classes::CLASS_DEMON_HUNTER,
                    { 127829, 128832 }
                }
            };

            auto l_Iter = l_ArtifactsPerClass.find(l_Player->getClass());
            if (l_Iter == l_ArtifactsPerClass.end())
            {
                p_Handler->PSendSysMessage("Artifacts not found for your class!");
                return false;
            }

            for (uint32 l_ID : l_Iter->second)
            {
                /// Adding items
                uint32 l_Count = 1;
                uint32 l_NoSpaceForCount = 0;

                /// Check space and find places
                ItemPosCountVec l_Dest;
                InventoryResult l_MSG = l_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Dest, l_ID, l_Count, &l_NoSpaceForCount);

                /// Convert to possible store amount
                if (l_MSG != EQUIP_ERR_OK)
                    l_Count -= l_NoSpaceForCount;

                /// Can't add any
                if (l_Count == 0 || l_Dest.empty())
                {
                    p_Handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, l_ID, l_NoSpaceForCount);
                    p_Handler->SetSentErrorMessage(true);
                    return false;
                }

                if (Item* l_Item = l_Player->StoreNewItem(l_Dest, l_ID, true, Item::GenerateItemRandomPropertyId(l_ID)))
                {
                    uint32 l_ArtifactID = l_Item->GetTemplate()->ArtifactID;

                    if (MS::Artifact::Manager* l_Mgr = l_Player->GetArtifactHolder(l_ArtifactID))
                    {
                        l_Mgr->ModifyArtifactPower(0xEFFFFFFFFFFFFFFF);

                        for (uint32 l_I = 0; l_I < sArtifactPowerStore.GetNumRows(); ++l_I)
                        {
                            if (ArtifactPowerEntry const* l_Entry = sArtifactPowerStore.LookupEntry(l_I))
                            {
                                if (l_Entry->ArtifactID != l_Mgr->GetArtifactId())
                                    continue;

                                for (uint8 l_J = 0; l_J < l_Entry->MaxPurchasableRank; ++l_J)
                                    l_Mgr->AddOrUpgradeTrait(l_Entry->ID);
                            }
                        }
                    }
                }
            }

            return true;
        }

        static bool HandleDebugLogDRCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (sSpellMgr->IsInDRDebug())
            {
                p_Handler->PSendSysMessage("DR debug mode is now disable");
                sSpellMgr->SetDRDebug(false);
            }
            else
            {
                sSpellMgr->SetDRDebug(true);
                p_Handler->PSendSysMessage("DR debug mode is now enable");
            }

            return true;
        }

        static bool HandleDebugPlayConversation(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            uint32 l_ConversationID = atoi((char*)p_Args);

            Conversation* l_Conversation = new Conversation;
            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), l_ConversationID, l_Player, nullptr, *l_Player))
                delete l_Conversation;

            p_Handler->PSendSysMessage("HandleConversationCommand ConversationID %u", l_ConversationID);
            return true;
        }

        static bool HandleDebugPlayCosmeticEvent(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args || !p_Handler->GetSession())
                return false;

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            uint32 l_EventId = atoi((char*)p_Args);

            l_Player->PlayCosmeticEvent(l_EventId);

            p_Handler->PSendSysMessage("HandleCosmeticEventCommand EventID %u", l_EventId);
            return true;
        }

        static bool HandleDebugMesureMapDiffCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            sMapMgr->m_LogMapPerformance = !sMapMgr->m_LogMapPerformance;
            return true;
        }

        static bool HandleDebugDailyPointCommand(ChatHandler* p_Handler, char const* p_Args)
        {
#ifndef CROSS
            WorldSession* l_Session = p_Handler->GetSession();
            time_t l_NowTime        = time(nullptr);

            time_t l_LastClaimTime  = l_Session->GetLastClaim();

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
            }
#endif

            return true;
        }

        static bool HandleDebugCancelDiminishingReturn(ChatHandler* handler, char const* args)
        {
            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            unit->ClearDiminishings();
            return true;
        }

        static bool HandleDebugAuraCommand(ChatHandler* handler, char const* args)
        {
            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 spellId = handler->extractSpellIdFromLink((char*)args);

            if (Aura* aura = unit->AddAura(spellId, unit))
                aura->SetDuration(HOUR * IN_MILLISECONDS);
            else
            {
                handler->PSendSysMessage("Failed to add aura %d", spellId);
                handler->SetSentErrorMessage(true);
                return false;
            }

            return true;
        }

        static bool HandleDebugCriticalCommand(ChatHandler* handler, char const* args)
        {
            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (Aura* l_Combustion = unit->AddAura(190319, unit))
                l_Combustion->SetDuration(HOUR * IN_MILLISECONDS);

            return true;
        }

        static bool HandleDebugHasteCommand(ChatHandler* handler, char const* args)
        {
            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            int pct = atoi(args);
            if (pct == 0)
                pct = 100;

            if (Aura* rapidCast = unit->AddAura(8215, unit))
            {
                rapidCast->SetDuration(HOUR * IN_MILLISECONDS);
                rapidCast->GetEffect(EFFECT_0)->ChangeAmount(pct);
            }

            return true;
        }

        static bool HandleDebugMasteryCommand(ChatHandler* handler, char const* args)
        {
            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            int pct = atoi(args);
            if (pct == 0)
                pct = 50000;

            if (Aura* perfectAim = unit->AddAura(114585, unit))
            {
                perfectAim->SetDuration(HOUR * IN_MILLISECONDS);
                perfectAim->GetEffect(EFFECT_0)->ChangeAmount(pct);
            }

            return true;
        }

        static bool HandleDebugVersatilityCommand(ChatHandler* handler, char const* args)
        {
            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            int pct = atoi(args);
            if (pct == 0)
                pct = 50000;

            if (Aura* perfectAim = unit->AddAura(40402, unit))
            {
                perfectAim->SetDuration(HOUR * IN_MILLISECONDS);
                perfectAim->GetEffect(EFFECT_0)->ChangeAmount(pct);
            }

            return true;
        }

        static bool HandleDebugDodgeCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Unit* l_Unit = p_Handler->getSelectedUnit();
            if (!l_Unit)
            {
                p_Handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            int l_Pct = atoi(p_Args);
            if (l_Pct == 0)
                l_Pct = 100;

            if (Aura* l_Dodge = l_Unit->AddAura(29045, l_Unit))
            {
                l_Dodge->SetDuration(HOUR * IN_MILLISECONDS);
                l_Dodge->GetEffect(EFFECT_0)->ChangeAmount(l_Pct);
            }

            return true;
        }

        static bool HandleDebugShowEquipErrorCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            uint32 l_ErrorID = (uint32)atoi((char*)p_Args);

            Player* l_Target = p_Handler->getSelectedPlayer();
            if (!l_Target)
            {
                p_Handler->SendSysMessage(LANG_SELECT_CREATURE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            l_Target->SendEquipError((InventoryResult)l_ErrorID, nullptr, nullptr);
            return true;
        }

        static bool HandleDebugRewardPack(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 l_ID = atoi(p_Args);

            p_Handler->GetSession()->GetPlayer()->GiveRewardPack(l_ID);
            return true;
        }

        static bool HandleDebugStressTestCommand(ChatHandler* p_Handler, char const* p_Args)
        {
#ifndef CROSS
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            std::string l_StrVal = std::string(strtok((char*)p_Args, " "));

            /// Disconnect all bots
            if (l_StrVal == "off")
            {
                SessionMap const& l_Sessions = sWorld->GetAllSessions();
                for (auto l_Session : l_Sessions)
                {
                    if (l_Session.second->IsStressTest())
                        l_Session.second->SetStressTest(false);
                }
                return true;
            }

            /// Add new bots
            if (l_StrVal == "on")
            {
                char* l_StrNumber = strtok(NULL, " ");
                if (l_StrNumber == nullptr)
                    return false;

                uint32 l_Number = atoi(l_StrNumber);

                if (l_Number > 1000)
                    return false;

                std::ostringstream l_Query;
                l_Query << "SELECT account, guid FROM characters WHERE account NOT IN(0";

                SessionMap const& l_Sessions = sWorld->GetAllSessions();
                for (auto l_Session : l_Sessions)
                    l_Query << "," << l_Session.first;

                l_Query << ") GROUP BY account ORDER BY RAND() LIMIT " << l_Number;

                QueryResult l_Result = CharacterDatabase.PQuery(l_Query.str().c_str());

                if (l_Result)
                {
                    do
                    {
                        Field* l_Fields = l_Result->Fetch();

                        uint32 l_AccountId = l_Fields[0].GetUInt32();
                        uint32 l_Guid      = l_Fields[1].GetUInt32();

                        WorldSession* l_NewSession = new WorldSession(l_AccountId, nullptr, SEC_ADMINISTRATOR, false, false, 0, 5, 0, LOCALE_frFR, 0, false, 0, 0, 0);

                        l_NewSession->SetStressTest(true);
                        l_NewSession->LoadGlobalAccountData();
                        l_NewSession->LoadTutorialsData();
                        sWorld->AddSession(l_NewSession);

                        l_NewSession->LoginPlayer(l_Guid);
                    }
                    while (l_Result->NextRow());
                }
            }
#endif

            return true;
        }

        static bool HandleDebugAdjustSplineCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            char* l_StrVal = strtok((char*)p_Args, " ");
            float l_Value = atof(l_StrVal);

            if (Unit* l_Target = p_Handler->getSelectedUnit())
            {
                l_Target->SendAdjustSplineDuration(l_Value);
                return true;
            }
            else
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }
        }

        static bool HandleDebugSplineSyncCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            char* l_StrVal = strtok((char*)p_Args, " ");
            float l_Value = atof(l_StrVal);

            if (Unit* l_Target = p_Handler->getSelectedUnit())
            {
                l_Target->SendFlightSplineSync(l_Value);
                return true;
            }
            else
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }
        }

        static bool HandleDebugClearSpellCharges(ChatHandler* handler, char const* args)
        {
            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 id = atoi((char*)args);

            if (SpellCategoryEntry const* l_Category = sSpellCategoryStore.LookupEntry(id))
            {
                if (Player* l_Player = handler->GetSession()->GetPlayer())
                    l_Player->ResetCharges(l_Category);

                return true;
            }
            else
            {
                handler->PSendSysMessage("Spell %u doesn't exist !", id);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        static bool HandleDebugToyCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            char* l_StrID = strtok(NULL, " ");
            uint32 l_ID = l_StrID ? uint32(atoi(l_StrID)) : 0;

            p_Handler->GetSession()->GetPlayer()->AddDynamicValue(PLAYER_DYNAMIC_FIELD_TOYS, l_ID);
            return true;
        }

        static bool HandleDebugPlaySceneCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 id = atoi((char*)args);

            if (!sSceneScriptPackageStore.LookupEntry(id))
            {
                handler->PSendSysMessage("Scene %u doesnt exist !", id);
                handler->SetSentErrorMessage(true);
                return false;
            }

            handler->PSendSysMessage("Start playing scene %u - %s !", id, sSceneScriptPackageStore.LookupEntry(id)->Name->Get(DEFAULT_LOCALE));
            handler->GetSession()->GetPlayer()->PlayScene(id, handler->GetSession()->GetPlayer());
            return true;
        }

        static bool HandleDebugPlaySSceneCommand(ChatHandler* p_Handler, char const* args)
        {
            if (!*args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 id = atoi((char*)args);

            if (!sSceneScriptPackageStore.LookupEntry(id))
            {
                p_Handler->PSendSysMessage("Scene %u doesnt exist !", id);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            Player * l_Player = p_Handler->GetSession()->GetPlayer();

            Position l_Location;
            l_Location.m_positionX = l_Player->m_positionX;
            l_Location.m_positionY = l_Player->m_positionY;
            l_Location.m_positionZ = l_Player->m_positionZ;
            l_Location.m_orientation = l_Player->m_orientation;

            p_Handler->PSendSysMessage("Start playing standalone scene %u - %s !", id, sSceneScriptPackageStore.LookupEntry(id)->Name->Get(DEFAULT_LOCALE));
            p_Handler->GetSession()->GetPlayer()->PlayStandaloneScene(id, 16, l_Location);

            return true;
        }

        static bool HandleDebugPlayOneShotAnimKit(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 l_ID = atoi((char*)p_Args);
            if (!l_ID)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (p_Handler->getSelectedUnit())
                p_Handler->getSelectedUnit()->PlayOneShotAnimKitId(l_ID);
            else
                p_Handler->GetSession()->GetPlayer()->PlayOneShotAnimKitId(l_ID);

            return true;
        }

        static bool HandleDebugSetAIAnimKit(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 l_ID = atoi((char*)p_Args);
            if (!l_ID)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (p_Handler->getSelectedUnit())
                p_Handler->getSelectedUnit()->SetAIAnimKitId(l_ID);
            else
                p_Handler->GetSession()->GetPlayer()->SetAIAnimKitId(l_ID);

            return true;
        }

        static bool HandleDebugPlaySpellVisualKit(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            char* l_StrID = strtok((char*)p_Args, " ");
            char* l_StrType = strtok(NULL, " ");

            if (!l_StrID || !l_StrType)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 l_ID = (uint32)atoi(l_StrID);
            uint32 l_Type = (uint32)atoi(l_StrType);

            if (p_Handler->getSelectedUnit())
                p_Handler->getSelectedUnit()->SendPlaySpellVisualKit(l_ID, l_Type);
            else
                p_Handler->GetSession()->GetPlayer()->SendPlaySpellVisualKit(l_ID, l_Type);

            return true;
        }

        static bool HandleDebugPlayOrphanSpellVisual(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            char* l_StrID = strtok((char*)p_Args, " ");
            char* l_StrSpeed = strtok(NULL, " ");

            if (!l_StrID || !l_StrSpeed)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 l_ID = (uint32)atoi(l_StrID);
            float l_Speed = (float)atof(l_StrSpeed);
            if (!l_ID)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            if (l_Player == nullptr)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            Unit* l_Target = l_Player->GetSelectedUnit();
            if (l_Target == nullptr)
                l_Target = l_Player;

            WorldPacket l_Data(Opcodes::SMSG_PLAY_ORPHAN_SPELL_VISUAL, 100);

            G3D::Vector3 l_Source(l_Player->m_positionX, l_Player->m_positionY, l_Player->m_positionZ);
            G3D::Vector3 l_Dest(l_Target->m_positionX, l_Target->m_positionY, l_Target->m_positionZ);
            G3D::Vector3 l_Orientation (0.0f, 0.0f, 0.0f);

            l_Data.WriteVector3(l_Source);
            l_Data.WriteVector3(l_Orientation);
            l_Data.WriteVector3(l_Dest);
            l_Data.appendPackGUID(0);

            l_Data << int32(l_ID);
            l_Data << float(l_Speed);
            l_Data << float(0.0f);

            l_Data.WriteBit(true);
            l_Data.FlushBits();

            p_Handler->GetSession()->SendPacket(&l_Data);
            return true;
        }

        static bool HandleDebugPlayVisual(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            char* l_StrID = strtok((char*)p_Args, " ");
            char* l_StrSpeed = strtok(NULL, " ");

            if (!l_StrID || !l_StrSpeed)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 l_ID = (uint32)atoi(l_StrID);
            float l_Speed = (float)atof(l_StrSpeed);
            if (!l_ID)
            {
                p_Handler->SendSysMessage(LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            if (Unit* l_Target = p_Handler->getSelectedUnit())
            {
                p_Handler->GetSession()->GetPlayer()->SendPlaySpellVisual(l_ID, l_Target, l_Speed, 0.0f, Position());
            }
            else
                p_Handler->GetSession()->GetPlayer()->SendPlaySpellVisual(l_ID, p_Handler->GetSession()->GetPlayer(), l_Speed, 0.0f, Position());

            return true;
        }

        static bool HandleJoinRatedBg(ChatHandler* handler, char const* /*args*/)
        {
            // ignore if we already in BG or BG queue
            if (handler->GetSession()->GetPlayer()->InBattleground())
                return false;

            uint32 personalRating = 0;
            uint32 matchmakerRating = 0;

            //check existance
            Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(MS::Battlegrounds::BattlegroundType::RatedBg10v10);
            if (!bg)
            {
                sLog->outError(LOG_FILTER_NETWORKIO, "Battleground: template bg (10 vs 10) not found");
                return false;
            }

            if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_RATED_10_VS_10, NULL))
                return false;

            BattlegroundTypeId bgTypeId = bg->GetTypeID();
            MS::Battlegrounds::BattlegroundType::Type bgQueueTypeId = MS::Battlegrounds::GetTypeFromId(bgTypeId, 0);

            MS::Battlegrounds::Bracket const* bracketEntry = MS::Battlegrounds::Brackets::FindForLevel(handler->GetSession()->GetPlayer()->getLevel());
            if (!bracketEntry)
                return false;

            GroupJoinBattlegroundResult err = ERR_BATTLEGROUND_NONE;

            GroupPtr grp = handler->GetSession()->GetPlayer()->GetGroup();

            // no group found, error
            if (!grp)
                return false;

            if (grp->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID())
                return false;

            uint32 playerDivider = 0;
            grp->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr) -> void
            {
                if (Player const* groupMember = l_Itr->player)
                {
                    personalRating += groupMember->GetArenaPersonalRating((uint8)BattlegroundBracketType::Battleground10v10);
                    matchmakerRating += groupMember->GetArenaMatchMakerRating((uint8)BattlegroundBracketType::Battleground10v10);

                    ++playerDivider;
                }
            });

            if (!playerDivider)
                return false;

            personalRating /= playerDivider;
            matchmakerRating /= playerDivider;

            if (personalRating <= 0)
                personalRating = 1;
            if (matchmakerRating <= 0)
                matchmakerRating = 1;

            MS::Battlegrounds::BattlegroundScheduler& l_Scheduler = sBattlegroundMgr->GetScheduler();
            MS::Battlegrounds::BattlegroundInvitationsMgr& l_InvitationsMgr = sBattlegroundMgr->GetInvitationsMgr();

            uint32 avgTime = 0;
            GroupQueueInfo* ginfo;

            err = grp->CanJoinBattlegroundQueue(bg, bgQueueTypeId, 2);
            if (!err)
            {
                sLog->outDebug(LOG_FILTER_BATTLEGROUND, "Battleground: leader %s queued", handler->GetSession()->GetPlayer()->GetName());

                ginfo = l_Scheduler.AddGroup(handler->GetSession()->GetPlayer(), grp, bgQueueTypeId, nullptr, bracketEntry, ArenaType::None, true, personalRating, matchmakerRating, false);
                avgTime = l_InvitationsMgr.GetAverageQueueWaitTime(ginfo, bracketEntry->m_Id);
            }

            grp->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr) -> void
            {
                Player* member = l_Itr->player;
                if (!member)
                    return;

                if (err)
                {
                    WorldPacket data;
                    MS::Battlegrounds::PacketFactory::StatusFailed(&data, bg, handler->GetSession()->GetPlayer(), 0, err);
                    member->GetSession()->SendPacket(&data);
                    return;
                }

                 // add to queue
                uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);

                // add joined time data
                member->AddBattlegroundQueueJoinTime(bgQueueTypeId, ginfo->m_JoinTime);

                WorldPacket data; // send status packet (in queue)
                MS::Battlegrounds::PacketFactory::Status(&data, bg, member, queueSlot, STATUS_WAIT_QUEUE, avgTime, ginfo->m_JoinTime, ginfo->m_ArenaType, false);
                member->GetSession()->SendPacket(&data);

                sLog->outDebug(LOG_FILTER_BATTLEGROUND, "Battleground: player joined queue for rated battleground as group bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, member->GetGUIDLow(), member->GetName());
            });

            //sBattlegroundMgr->ScheduleQueueUpdate(matchmakerRating, 0, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());

            return true;
        }

        static bool HandleDebugMoveCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* result = strtok((char*)args, " ");
            if (!result)
                return false;

            bool apply = bool(atoi(result));

            char* param = strtok(NULL, " ");
            if (!param)
                return false;

            Position pos;
            pos.m_positionX = float(atoi(param));

            param = strtok(NULL, " ");
            if (!param)
                return false;

            pos.m_positionY = float(atoi(param));

            param = strtok(NULL, " ");
            if (!param)
                return false;

            pos.m_positionZ = float(atoi(param));

            param = strtok(NULL, " ");
            if (!param)
                return false;

            float force = float(atoi(param));

            uint64 l_FakeGuid = MAKE_NEW_GUID(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), 6452, HIGHGUID_AREATRIGGER);
            handler->GetSession()->GetPlayer()->SendApplyMovementForce(l_FakeGuid, apply, pos, force, 1);

            return true;
        }

        static bool HandleDebugLogCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* result = strtok((char*)args, " ");
            if (!result)
                return false;

            uint32 opcodeId  = uint32(atoi(result));
            if (opcodeId == 0 && *result != '0')
                return false;

            WorldPacket data(Opcodes(opcodeId), 10);
            ObjectGuid playerGuid = handler->GetSession()->GetPlayer()->GetGUID();
            Player* player = handler->GetSession()->GetPlayer();

            switch (opcodeId)
            {
                case 1440:
                {
                    std::string name = player->GetName();

                    data << uint64(playerGuid);
                    data.WriteBit(true);

                    for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                        data.WriteBits(0, 7);

                    data.WriteBits(name.size(), 8);

                    data << uint32(3117);
                    data.append(name.c_str(), name.size());

                    break;
                }
                /// @todo Update me ?
                case SMSG_SERVER_FIRST_ACHIEVEMENT:
                {
                    data.WriteBits(1, 21);

                    uint8 bits[8] = { 7, 5, 0, 3, 6, 2, 1, 4 };
                    data.WriteBitInOrder(playerGuid, bits);

                    data.WriteByteSeq(playerGuid[6]);
                    data.WriteByteSeq(playerGuid[3]);
                    data << uint32(3117); // Faucheur de la Mort "Prem's" du royaume
                    data.WriteByteSeq(playerGuid[4]);
                    data.WriteByteSeq(playerGuid[0]);
                    data.WriteByteSeq(playerGuid[1]);
                    data.WriteByteSeq(playerGuid[2]);
                    data.WriteByteSeq(playerGuid[7]);
                    data.WriteByteSeq(playerGuid[5]);
                    break;
                }
                case 4101:
                {
                    data.WriteBit(false);   // Inversed, Unk
                    data.WriteBit(false);   // Inversed, Unk
                    data.WriteBit(false);   // Inversed, has UInt32 - bit8

                    uint8 bits[8] = { 7, 4, 5, 3, 0, 2, 6, 1 };
                    data.WriteBitInOrder(playerGuid, bits);

                    uint8 bits2[8] = { 6, 4, 2, 0, 5, 7, 3, 1 };
                    data.WriteBitInOrder(playerGuid, bits2);

                    data.WriteBit(false);   // Inversed, Unk
                    data.WriteBit(false);   // Inversed, Unk

                    data << uint32(100784);

                    uint8 bytes[8] = { 1, 3, 4, 7, 5, 0, 6, 2 };
                    data.WriteBytesSeq(playerGuid, bytes);

                    data << float(1.0f);    // Unk

                    uint8 bytes2[8] = { 3, 5, 7, 2, 4, 6, 0, 1 };
                    data.WriteBytesSeq(playerGuid, bytes2);

                    data << float(1.0f);    // Unk
                    break;
                }
                case 4385:
                {
                    uint8 bits[8] = { 2, 5, 0, 1, 3, 6, 4, 7 };
                    data.WriteBitInOrder(playerGuid, bits);

                    data.WriteByteSeq(playerGuid[0]);
                    data.WriteByteSeq(playerGuid[1]);
                    data.WriteByteSeq(playerGuid[4]);
                    data.WriteByteSeq(playerGuid[5]);
                    data.WriteByteSeq(playerGuid[6]);
                    data << uint8(2);
                    data.WriteByteSeq(playerGuid[3]);
                    data << uint32(100784);
                    data.WriteByteSeq(playerGuid[2]);
                    data.WriteByteSeq(playerGuid[7]);
                    break;
                }
                case 4501:
                {
                    // Display channel bar
                    data.WriteBit(false);
                    data.WriteBit(true);    // Unk bit56

                    uint8 bits[8] = { 6, 0, 4, 1, 2, 3, 5, 7 };
                    data.WriteBitInOrder(playerGuid, bits);

                    data << uint32(100784);
                    data << uint32(100784);
                    data.WriteByteSeq(playerGuid[5]);
                    data.WriteByteSeq(playerGuid[6]);
                    data.WriteByteSeq(playerGuid[4]);
                    data << uint32(100784);
                    data.WriteByteSeq(playerGuid[7]);
                    data.WriteByteSeq(playerGuid[1]);
                    data.WriteByteSeq(playerGuid[3]);
                    data << uint32(100784);
                    data.WriteByteSeq(playerGuid[2]);
                    data.WriteByteSeq(playerGuid[0]);
                    break;
                }
                case 4518:
                {
                    // Show cast bar
                    data.WriteBit(playerGuid[2]);
                    data.WriteBit(playerGuid[1]);
                    data.WriteBit(true);    // Unk bit40
                    data.WriteBit(playerGuid[1]);
                    data.WriteBit(playerGuid[4]);
                    data.WriteBit(playerGuid[0]);
                    data.WriteBit(playerGuid[3]);
                    data.WriteBit(playerGuid[2]);
                    data.WriteBit(playerGuid[3]);
                    data.WriteBit(playerGuid[5]);
                    data.WriteBit(playerGuid[7]);
                    data.WriteBit(playerGuid[6]);
                    data.WriteBit(playerGuid[0]);
                    data.WriteBit(playerGuid[6]);
                    data.WriteBit(playerGuid[5]);
                    data.WriteBit(playerGuid[4]);
                    data.WriteBit(playerGuid[7]);

                    data.WriteByteSeq(playerGuid[0]);
                    data.WriteByteSeq(playerGuid[5]);
                    data.WriteByteSeq(playerGuid[1]);
                    data.WriteByteSeq(playerGuid[1]);
                    data.WriteByteSeq(playerGuid[5]);
                    data.WriteByteSeq(playerGuid[6]);
                    data << uint32(100784);
                    data << uint32(100784);
                    data << uint32(100784);
                    data << uint32(100784);
                    data.WriteByteSeq(playerGuid[0]);
                    data.WriteByteSeq(playerGuid[2]);
                    data.WriteByteSeq(playerGuid[2]);
                    data.WriteByteSeq(playerGuid[3]);
                    data.WriteByteSeq(playerGuid[7]);
                    data.WriteByteSeq(playerGuid[3]);
                    data.WriteByteSeq(playerGuid[4]);
                    data.WriteByteSeq(playerGuid[7]);
                    data << uint32(100784);
                    data.WriteByteSeq(playerGuid[4]);
                    data.WriteByteSeq(playerGuid[6]);
                    break;
                }
                default:
                    break;
            }

            handler->GetSession()->SendPacket(&data, true);

            return true;
        }

        static bool HandleDebugGuildEventCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* result = strtok((char*)args, " ");
            if (!result)
                return false;

            uint32 opcodeId = uint32(atoi(result));
            if (opcodeId == 0 && *result != '0')
                return false;

            switch (opcodeId)
            {
                case 0x20:
                {
                    ///@todo update me ?
                    // La migration de votre guilde est termin�e. Rendez-vous sur [cette page Internet] pour de plus amples informations.
                    Player* player = handler->GetSession()->GetPlayer();
                    WorldPacket data(Opcodes(1346), 20);
                    ObjectGuid playerGuid = player->GetGUID();

                    data.WriteBit(playerGuid[3]);
                    data.WriteBit(playerGuid[2]);
                    data.WriteBit(playerGuid[6]);
                    data.WriteBit(playerGuid[1]);
                    data.WriteBits(strlen(player->GetName()), 8);
                    data.WriteBit(playerGuid[5]);
                    data.WriteBit(playerGuid[7]);
                    data.WriteBit(playerGuid[0]);
                    data.WriteBit(playerGuid[4]);

                    data.WriteByteSeq(playerGuid[0]);
                    data.WriteByteSeq(playerGuid[2]);
                    data.WriteByteSeq(playerGuid[3]);
                    data.WriteByteSeq(playerGuid[1]);
                    data.WriteByteSeq(playerGuid[6]);
                    data.WriteByteSeq(playerGuid[4]);
                    data.WriteByteSeq(playerGuid[5]);
                    data.WriteByteSeq(playerGuid[7]);
                    if (strlen(player->GetName()))
                        data.append(player->GetName(), strlen(player->GetName()));

                    player->GetSession()->SendPacket(&data, true);

                    break;
                }
                case 0x2F:
                {
                    // ??
                    Player* player = handler->GetSession()->GetPlayer();
                    WorldPacket data(Opcodes(1375), 20);
                    ObjectGuid playerGuid = player->GetGUID();

                    data.WriteBit(playerGuid[3]);
                    data.WriteBit(playerGuid[1]);
                    data.WriteBit(playerGuid[0]);
                    data.WriteBit(playerGuid[5]);
                    data.WriteBits(strlen(player->GetName()), 8);
                    data.WriteBit(playerGuid[2]);
                    data.WriteBit(playerGuid[4]);
                    data.WriteBit(playerGuid[7]);
                    data.WriteBit(1);               // Unk
                    data.WriteBit(playerGuid[6]);

                    data.WriteByteSeq(playerGuid[3]);
                    data.WriteByteSeq(playerGuid[1]);
                    if (strlen(player->GetName()))
                        data.append(player->GetName(), strlen(player->GetName()));
                    data.WriteByteSeq(playerGuid[6]);
                    data.WriteByteSeq(playerGuid[7]);
                    data.WriteByteSeq(playerGuid[5]);
                    data.WriteByteSeq(playerGuid[2]);
                    data.WriteByteSeq(playerGuid[4]);
                    data.WriteByteSeq(playerGuid[0]);

                    player->GetSession()->SendPacket(&data, true);

                    break;
                }
                case 0x55:
                {
                    // ??
                    Player* player = handler->GetSession()->GetPlayer();
                    WorldPacket data(Opcodes(3179), 20);
                    ObjectGuid playerGuid = player->GetGUID();

                    data.WriteBit(playerGuid[4]);
                    data.WriteBit(playerGuid[0]);
                    data.WriteBit(playerGuid[7]);
                    data.WriteBit(playerGuid[3]);
                    data.WriteBits(strlen(player->GetName()), 7);
                    data.WriteBit(playerGuid[6]);
                    data.WriteBit(playerGuid[5]);
                    data.WriteBit(playerGuid[1]);
                    data.WriteBit(playerGuid[2]);

                    data.WriteByteSeq(playerGuid[0]);
                    data.WriteByteSeq(playerGuid[4]);
                    data.WriteByteSeq(playerGuid[5]);
                    data.WriteByteSeq(playerGuid[1]);
                    data.WriteByteSeq(playerGuid[7]);
                    data.WriteByteSeq(playerGuid[2]);
                    if (strlen(player->GetName()))
                        data.append(player->GetName(), strlen(player->GetName()));
                    data.WriteByteSeq(playerGuid[3]);
                    data.WriteByteSeq(playerGuid[6]);

                    player->GetSession()->SendPacket(&data, true);

                    break;
                }
                default:
                    break;
            }

            return true;
        }

        static bool HandleDebugPacketCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* result = strtok((char*)args, " ");
            if (!result)
                return false;

            uint32 opcodeId  = uint32(atoi(result));
            if (opcodeId == 0 && *result != '0')
                return false;

            char* value = strtok(NULL, " ");
            uint32 sendValue = value ? uint32(atoi(value)) : 0;

            WorldPacket data(Opcodes(opcodeId), 4);
            data << uint32(sendValue);
            handler->GetSession()->SendPacket(&data, true);

            return true;
        }

        static bool HandleSendTradeStatus(ChatHandler* handler, char const* args)
        {
            // USAGE: .debug play cinematic #cinematicid
            // #cinematicid - ID decimal number from CinemaicSequences.db2 (1st column)
            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 id = atoi((char*)args);

            handler->GetSession()->SendTradeStatus(TradeStatus(id));
            return true;
        }

        static bool HandleSendMailStatus(ChatHandler* handler, char const* args)
        {
            // USAGE: .debug play cinematic #cinematicid
            // #cinematicid - ID decimal number from CinemaicSequences.db2 (1st column)
            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 id = atoi((char*)args);
            uint32 id2 = atoi((char*)args);

            handler->GetSession()->GetPlayer()->SendMailResult(0, MailResponseType(id), MailResponseResult(id2));
            return true;
        }

        static bool HandleDebugPlayCinematicCommand(ChatHandler* handler, char const* args)
        {
            // USAGE: .debug play cinematic #cinematicid
            // #cinematicid - ID decimal number from CinemaicSequences.db2 (1st column)
            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 id = atoi((char*)args);

            if (!sCinematicSequencesStore.LookupEntry(id))
            {
                handler->PSendSysMessage(LANG_CINEMATIC_NOT_EXIST, id);
                handler->SetSentErrorMessage(true);
                return false;
            }

            handler->GetSession()->GetPlayer()->SendCinematicStart(id);
            return true;
        }

        static bool HandleDebugPlayMovieCommand(ChatHandler* handler, char const* args)
        {
            // USAGE: .debug play movie #movieid
            // #movieid - ID decimal number from Movie.db2 (1st column)
            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 id = atoi((char*)args);

            if (!sMovieStore.LookupEntry(id))
            {
                handler->PSendSysMessage(LANG_MOVIE_NOT_EXIST, id);
                handler->SetSentErrorMessage(true);
                return false;
            }

            handler->GetSession()->GetPlayer()->SendMovieStart(id);
            return true;
        }

        //Play sound
        static bool HandleDebugPlaySoundCommand(ChatHandler* handler, char const* args)
        {
            // USAGE: .debug playsound #soundid
            // #soundid - ID decimal number from SoundEntries.db2 (1st column)
            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 soundId = atoi((char*)args);

            if (!sSoundKitStore.LookupEntry(soundId))
            {
                handler->PSendSysMessage(LANG_SOUND_NOT_EXIST, soundId);
                handler->SetSentErrorMessage(true);
                return false;
            }

            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (handler->GetSession()->GetPlayer()->GetSelection())
                unit->PlayDistanceSound(unit, soundId, handler->GetSession()->GetPlayer());
            else
                unit->PlayDirectSound(soundId, handler->GetSession()->GetPlayer());

            handler->PSendSysMessage(LANG_YOU_HEAR_SOUND, soundId);
            return true;
        }

        static bool HandleDebugSendSpellFailCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* result = strtok((char*)args, " ");
            if (!result)
                return false;

            uint32 failNum = atoi(result);
            if (failNum == 0 && *result != '0')
                return false;

            char* fail1 = strtok(NULL, " ");
            uint32 failArg1 = fail1 ? atoi(fail1) : 0;

            char* fail2 = strtok(NULL, " ");
            uint32 failArg2 = fail2 ? atoi(fail2) : 0;

            WorldPacket data(SMSG_CAST_FAILED, 5);

            data << uint32(133);        ///< SpellID
            data << uint32(failNum);    ///< Problem

            if (fail1 || fail2)
                data << uint32(failArg1);
            if (fail2)
                data << uint32(failArg2);

            data << uint8(0);           ///< CastCount

            handler->GetSession()->SendPacket(&data);

            return true;
        }

        static bool HandleDebugSendEquipErrorCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            InventoryResult msg = InventoryResult(atoi(args));
            handler->GetSession()->GetPlayer()->SendEquipError(msg, NULL, NULL);
            return true;
        }

        static bool HandleDebugSendSellErrorCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            SellResult msg = SellResult(atoi(args));
            handler->GetSession()->GetPlayer()->SendSellError(msg, 0, 0);
            return true;
        }

        static bool HandleDebugSendBuyErrorCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            BuyResult msg = BuyResult(atoi(args));
            handler->GetSession()->GetPlayer()->SendBuyError(msg, 0, 0, 0);
            return true;
        }

        static bool HandleDebugSendOpcodeCommand(ChatHandler* handler, char const* /*args*/)
        {
            Unit* unit = handler->getSelectedUnit();
            Player* player = NULL;
            if (!unit || (unit->GetTypeId() != TYPEID_PLAYER))
                player = handler->GetSession()->GetPlayer();
            else
                player = (Player*)unit;

            if (!unit)
                unit = player;

            std::ifstream ifs("opcode.txt");
            if (ifs.bad())
                return false;

            // remove comments from file
            std::stringstream parsedStream;
            while (!ifs.eof())
            {
                char commentToken[2];
                ifs.get(commentToken[0]);
                if (commentToken[0] == '/' && !ifs.eof())
                {
                    ifs.get(commentToken[1]);
                    // /* comment
                    if (commentToken[1] == '*')
                    {
                        while (!ifs.eof())
                        {
                            ifs.get(commentToken[0]);
                            if (commentToken[0] == '*' && !ifs.eof())
                            {
                                ifs.get(commentToken[1]);
                                if (commentToken[1] == '/')
                                    break;
                                else
                                    ifs.putback(commentToken[1]);
                            }
                        }
                        continue;
                    }
                    // line comment
                    else if (commentToken[1] == '/')
                    {
                        std::string str;
                        getline(ifs, str);
                        continue;
                    }
                    // regular data
                    else
                        ifs.putback(commentToken[1]);
                }
                parsedStream.put(commentToken[0]);
            }
            ifs.close();

            uint32 opcode;
            parsedStream >> opcode;

            WorldPacket data(Opcodes(opcode), 0);

            while (!parsedStream.eof())
            {
                std::string type;
                parsedStream >> type;

                if (type == "")
                    break;

                if (type == "uint8")
                {
                    uint16 val1;
                    parsedStream >> val1;
                    data << uint8(val1);
                }
                else if (type == "uint16")
                {
                    uint16 val2;
                    parsedStream >> val2;
                    data << val2;
                }
                else if (type == "uint32")
                {
                    uint32 val3;
                    parsedStream >> val3;
                    data << val3;
                }
                else if (type == "uint64")
                {
                    uint64 val4;
                    parsedStream >> val4;
                    data << val4;
                }
                else if (type == "float")
                {
                    float val5;
                    parsedStream >> val5;
                    data << val5;
                }
                else if (type == "string")
                {
                    std::string val6;
                    parsedStream >> val6;
                    data << val6;
                }
                else if (type == "appitsguid")
                {
                    data.append(unit->GetPackGUID());
                }
                else if (type == "appmyguid")
                {
                    data.append(player->GetPackGUID());
                }
                else if (type == "appgoguid")
                {
                    GameObject* obj = handler->GetNearbyGameObject();
                    if (!obj)
                    {
                        handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, 0);
                        handler->SetSentErrorMessage(true);
                        ifs.close();
                        return false;
                    }
                    data.append(obj->GetPackGUID());
                }
                else if (type == "goguid")
                {
                    GameObject* obj = handler->GetNearbyGameObject();
                    if (!obj)
                    {
                        handler->PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, 0);
                        handler->SetSentErrorMessage(true);
                        ifs.close();
                        return false;
                    }
                    data << uint64(obj->GetGUID());
                }
                else if (type == "myguid")
                {
                    data << uint64(player->GetGUID());
                }
                else if (type == "itsguid")
                {
                    data << uint64(unit->GetGUID());
                }
                else if (type == "itspos")
                {
                    data << unit->GetPositionX();
                    data << unit->GetPositionY();
                    data << unit->GetPositionZ();
                }
                else if (type == "mypos")
                {
                    data << player->GetPositionX();
                    data << player->GetPositionY();
                    data << player->GetPositionZ();
                }
                else
                {
                    sLog->outError(LOG_FILTER_GENERAL, "Sending opcode that has unknown type '%s'", type.c_str());
                    break;
                }
            }
            sLog->outDebug(LOG_FILTER_NETWORKIO, "Sending opcode %u", data.GetOpcode());
            data.hexlike();
            player->GetSession()->SendPacket(&data, true);
            handler->PSendSysMessage(LANG_COMMAND_OPCODESENT, data.GetOpcode(), unit->GetName());
            return true;
        }

        static bool HandleDebugUpdateWorldStateCommand(ChatHandler* handler, char const* args)
        {
            char* w = strtok((char*)args, " ");
            char* s = strtok(NULL, " ");

            if (!w || !s)
                return false;

            uint32 world = (uint32)atoi(w);
            uint32 state = (uint32)atoi(s);
            handler->GetSession()->GetPlayer()->SendUpdateWorldState(world, state);
            return true;
        }

        static bool HandleDebugEvalWorldStateExpCommand(ChatHandler* p_Handler, char const* args)
        {
            char* l_WorldStateID = strtok((char*)args, " ");

            if (!l_WorldStateID)
                return false;

            WorldStateExpressionEntry const* l_WorldStateExpressionEntry = sWorldStateExpressionStore.LookupEntry((uint32)atoi(l_WorldStateID));
            if (!l_WorldStateExpressionEntry)
            {
                p_Handler->SendSysMessage("WorldStateID not found in DB2");
                return false;
            }

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            if (!l_Player)
                return false;

            std::vector<std::string> l_StringVector;
            l_WorldStateExpressionEntry->Eval(l_Player, &l_StringVector);

            for (auto l_Str : l_StringVector)
                p_Handler->SendSysMessage(l_Str.c_str());

            return true;
        }

        static bool HandleDebugAreaTriggersCommand(ChatHandler* handler, char const* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();
            if (!player->isDebugAreaTriggers)
            {
                handler->PSendSysMessage(LANG_DEBUG_AREATRIGGER_ON);
                player->isDebugAreaTriggers = true;
            }
            else
            {
                handler->PSendSysMessage(LANG_DEBUG_AREATRIGGER_OFF);
                player->isDebugAreaTriggers = false;
            }
            return true;
        }

        static bool HandleDebugPvPTemplateCommand(ChatHandler* p_Handler, char const * /*p_Args*/)
        {
            if (sSpellMgr->IsInPvPTemplateDebug())
            {
                p_Handler->PSendSysMessage("PvP Template debug mode is now disable");
                sSpellMgr->SetPvPTemplateDebug(false);
            }
            else
            {
                sSpellMgr->SetPvPTemplateDebug(true);
                p_Handler->PSendSysMessage("PvP Template debug mode is now enable");
            }

            return true;
        }

        //Send notification in channel
        static bool HandleDebugSendChannelNotifyCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char const* name = "test";
            uint8 code = atoi(args);

            WorldPacket data(SMSG_CHANNEL_NOTIFY, (1+10));
            data << code;                                           // notify type
            data << name;                                           // channel name
            data << uint32(0);
            data << uint32(0);
            handler->GetSession()->SendPacket(&data);
            return true;
        }

        //Send notification in chat
        static bool HandleDebugSendChatMsgCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char const* msg = "testtest";
            uint8 type = atoi(args);
            WorldPacket data;
            ChatHandler::FillMessageData(&data, handler->GetSession(), type, 0, "chan", handler->GetSession()->GetPlayer()->GetGUID(), msg, handler->GetSession()->GetPlayer());
            handler->GetSession()->SendPacket(&data);
            return true;
        }

        static bool HandleDebugSendQuestPartyMsgCommand(ChatHandler* handler, char const* args)
        {
            uint32 msg = atol((char*)args);
            handler->GetSession()->GetPlayer()->SendPushToPartyResponse(handler->GetSession()->GetPlayer(), msg);
            return true;
        }

        static bool HandleDebugSendQuestInvalidMsgCommand(ChatHandler* handler, char const* args)
        {
            uint32 msg = atol((char*)args);
            handler->GetSession()->GetPlayer()->SendCanTakeQuestResponse(msg);
            return true;
        }

        static bool HandleDebugGetItemStateCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            std::string itemState = args;

            ItemUpdateState state = ITEM_UNCHANGED;
            bool listQueue = false;
            bool checkAll = false;

            if (itemState == "unchanged")
                state = ITEM_UNCHANGED;
            else if (itemState == "changed")
                state = ITEM_CHANGED;
            else if (itemState == "new")
                state = ITEM_NEW;
            else if (itemState == "removed")
                state = ITEM_REMOVED;
            else if (itemState == "queue")
                listQueue = true;
            else if (itemState == "check_all")
                checkAll = true;
            else
                return false;

            Player* player = handler->getSelectedPlayer();
            if (!player)
                player = handler->GetSession()->GetPlayer();

            if (!listQueue && !checkAll)
            {
                itemState = "The player has the following " + itemState + " items: ";
                handler->SendSysMessage(itemState.c_str());
                for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; ++i)
                {
                    if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                        continue;

                    if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    {
                        if (Bag* bag = item->ToBag())
                        {
                            for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                                if (Item* item2 = bag->GetItemByPos(j))
                                    if (item2->GetState() == state)
                                        handler->PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item2->GetSlot(), item2->GetGUIDLow(), GUID_LOPART(item2->GetOwnerGUID()));
                        }
                        else if (item->GetState() == state)
                            handler->PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()));
                    }
                }
            }

            if (listQueue)
            {
                std::vector<Item*>& updateQueue = player->GetItemUpdateQueue();
                for (size_t i = 0; i < updateQueue.size(); ++i)
                {
                    Item* item = updateQueue[i];
                    if (!item)
                        continue;

                    Bag* container = item->GetContainer();
                    uint8 bagSlot = container ? container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);

                    std::string st;
                    switch (item->GetState())
                    {
                        case ITEM_UNCHANGED:
                            st = "unchanged";
                            break;
                        case ITEM_CHANGED:
                            st = "changed";
                            break;
                        case ITEM_NEW:
                            st = "new";
                            break;
                        case ITEM_REMOVED:
                            st = "removed";
                            break;
                    }

                    handler->PSendSysMessage("bag: %d slot: %d guid: %d - state: %s", bagSlot, item->GetSlot(), item->GetGUIDLow(), st.c_str());
                }
                if (updateQueue.empty())
                    handler->PSendSysMessage("The player's updatequeue is empty");
            }

            if (checkAll)
            {
                bool error = false;
                std::vector<Item*>& updateQueue = player->GetItemUpdateQueue();
                for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; ++i)
                {
                    if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                        continue;

                    Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
                    if (!item)
                        continue;

                    if (item->GetSlot() != i)
                    {
                        handler->PSendSysMessage("Item with slot %d and guid %d has an incorrect slot value: %d", i, item->GetGUIDLow(), item->GetSlot());
                        error = true;
                        continue;
                    }

                    if (item->GetOwnerGUID() != player->GetGUID())
                    {
                        handler->PSendSysMessage("The item with slot %d and itemguid %d does have non-matching owner guid (%d) and player guid (%d) !", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                        error = true;
                        continue;
                    }

                    if (Bag* container = item->GetContainer())
                    {
                        handler->PSendSysMessage("The item with slot %d and guid %d has a container (slot: %d, guid: %d) but shouldn't!", item->GetSlot(), item->GetGUIDLow(), container->GetSlot(), container->GetGUIDLow());
                        error = true;
                        continue;
                    }

                    if (item->IsInUpdateQueue())
                    {
                        uint16 qp = item->GetQueuePos();
                        if (qp > updateQueue.size())
                        {
                            handler->PSendSysMessage("The item with slot %d and guid %d has its queuepos (%d) larger than the update queue size! ", item->GetSlot(), item->GetGUIDLow(), qp);
                            error = true;
                            continue;
                        }

                        if (updateQueue[qp] == NULL)
                        {
                            handler->PSendSysMessage("The item with slot %d and guid %d has its queuepos (%d) pointing to NULL in the queue!", item->GetSlot(), item->GetGUIDLow(), qp);
                            error = true;
                            continue;
                        }

                        if (updateQueue[qp] != item)
                        {
                            handler->PSendSysMessage("The item with slot %d and guid %d has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", item->GetSlot(), item->GetGUIDLow(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUIDLow());
                            error = true;
                            continue;
                        }
                    }
                    else if (item->GetState() != ITEM_UNCHANGED)
                    {
                        handler->PSendSysMessage("The item with slot %d and guid %d is not in queue but should be (state: %d)!", item->GetSlot(), item->GetGUIDLow(), item->GetState());
                        error = true;
                        continue;
                    }

                    if (Bag* bag = item->ToBag())
                    {
                        for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                        {
                            Item* item2 = bag->GetItemByPos(j);
                            if (!item2)
                                continue;

                            if (item2->GetSlot() != j)
                            {
                                handler->PSendSysMessage("The item in bag %d and slot %d (guid: %d) has an incorrect slot value: %d", bag->GetSlot(), j, item2->GetGUIDLow(), item2->GetSlot());
                                error = true;
                                continue;
                            }

                            if (item2->GetOwnerGUID() != player->GetGUID())
                            {
                                handler->PSendSysMessage("The item in bag %d at slot %d and with itemguid %d, the owner's guid (%d) and the player's guid (%d) don't match!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), GUID_LOPART(item2->GetOwnerGUID()), player->GetGUIDLow());
                                error = true;
                                continue;
                            }

                            Bag* container = item2->GetContainer();
                            if (!container)
                            {
                                handler->PSendSysMessage("The item in bag %d at slot %d with guid %d has no container!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow());
                                error = true;
                                continue;
                            }

                            if (container != bag)
                            {
                                handler->PSendSysMessage("The item in bag %d at slot %d with guid %d has a different container(slot %d guid %d)!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), container->GetSlot(), container->GetGUIDLow());
                                error = true;
                                continue;
                            }

                            if (item2->IsInUpdateQueue())
                            {
                                uint16 qp = item2->GetQueuePos();
                                if (qp > updateQueue.size())
                                {
                                    handler->PSendSysMessage("The item in bag %d at slot %d having guid %d has a queuepos (%d) larger than the update queue size! ", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), qp);
                                    error = true;
                                    continue;
                                }

                                if (updateQueue[qp] == NULL)
                                {
                                    handler->PSendSysMessage("The item in bag %d at slot %d having guid %d has a queuepos (%d) that points to NULL in the queue!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), qp);
                                    error = true;
                                    continue;
                                }

                                if (updateQueue[qp] != item2)
                                {
                                    handler->PSendSysMessage("The item in bag %d at slot %d having guid %d has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUIDLow());
                                    error = true;
                                    continue;
                                }
                            }
                            else if (item2->GetState() != ITEM_UNCHANGED)
                            {
                                handler->PSendSysMessage("The item in bag %d at slot %d having guid %d is not in queue but should be (state: %d)!", bag->GetSlot(), item2->GetSlot(), item2->GetGUIDLow(), item2->GetState());
                                error = true;
                                continue;
                            }
                        }
                    }
                }

                for (size_t i = 0; i < updateQueue.size(); ++i)
                {
                    Item* item = updateQueue[i];
                    if (!item)
                        continue;

                    if (item->GetOwnerGUID() != player->GetGUID())
                    {
                        handler->PSendSysMessage("queue(%zu): For the item with guid %d, the owner's guid (%d) and the player's guid (%d) don't match!", i, item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                        error = true;
                        continue;
                    }

                    if (item->GetQueuePos() != i)
                    {
                        handler->PSendSysMessage("queue(%zu): For the item with guid %d, the queuepos doesn't match it's position in the queue!", i, item->GetGUIDLow());
                        error = true;
                        continue;
                    }

                    if (item->GetState() == ITEM_REMOVED)
                        continue;

                    Item* test = player->GetItemByPos(item->GetBagSlot(), item->GetSlot());

                    if (test == NULL)
                    {
                        handler->PSendSysMessage("queue(%zu): The bag(%d) and slot(%d) values for the item with guid %d are incorrect, the player doesn't have any item at that position!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUIDLow());
                        error = true;
                        continue;
                    }

                    if (test != item)
                    {
                        handler->PSendSysMessage("queue(%zu): The bag(%d) and slot(%d) values for the item with guid %d are incorrect, an item which guid is %d is there instead!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUIDLow(), test->GetGUIDLow());
                        error = true;
                        continue;
                    }
                }
                if (!error)
                    handler->SendSysMessage("All OK!");
            }

            return true;
        }

#ifndef CROSS
        static bool HandleDebugBattlegroundCommand(ChatHandler* handler, char const* /*args*/)
#else /* CROSS */
        static bool HandleDebugBattlegroundCommand(ChatHandler* /*handler*/, char const* /*args*/)
#endif /* CROSS */
        {
#ifndef CROSS
            if (sWorld->getBoolConfig(CONFIG_INTERREALM_ENABLE))
            {
                InterRealmSession* tunnel = sWorld->GetInterRealmSession();
                if (!tunnel || !tunnel->IsTunnelOpened())
                {
                    handler->PSendSysMessage(LANG_INTERREALM_DISABLED);
                    return false;
                }

                WorldPacket pckt(IR_CMSG_DEBUG_BG, 1);
                pckt << uint8(1);
                tunnel->SendPacket(&pckt);
                return true;
            }
#endif /* not CROSS */
            sBattlegroundMgr->ToggleTesting();
#ifndef CROSS

#endif /* not CROSS */
            return true;
        }

#ifndef CROSS
        static bool HandleDebugArenaCommand(ChatHandler* handler, char const* /*args*/)
#else /* CROSS */
        static bool HandleDebugArenaCommand(ChatHandler* /*handler*/, char const* /*args*/)
#endif /* CROSS */
        {
#ifndef CROSS
            if (sWorld->getBoolConfig(CONFIG_INTERREALM_ENABLE))
            {
                InterRealmSession* tunnel = sWorld->GetInterRealmSession();
                if (!tunnel || !tunnel->IsTunnelOpened())
                {
                    handler->PSendSysMessage(LANG_INTERREALM_DISABLED);
                    return false;
                }

                WorldPacket pckt(IR_CMSG_DEBUG_ARENA, 1);
                pckt << uint8(1);
                tunnel->SendPacket(&pckt);
                return true;
            }
#endif /* not CROSS */
            sBattlegroundMgr->ToggleArenaTesting();
            return true;
        }

        static bool HandleDebugThreatListCommand(ChatHandler* handler, char const* /*args*/)
        {
            Creature* target = handler->getSelectedCreature();
            if (!target || target->isTotem() || target->isPet())
                return false;

            std::list<HostileReference*>& threatList = target->getThreatManager().getThreatList();
            std::list<HostileReference*>::iterator itr;
            uint32 count = 0;
            handler->PSendSysMessage("Threat list of %s (guid %u)", target->GetName(), target->GetGUIDLow());
            for (itr = threatList.begin(); itr != threatList.end(); ++itr)
            {
                Unit* unit = (*itr)->getTarget();
                if (!unit)
                    continue;
                ++count;
                handler->PSendSysMessage("   %u.   %s   (guid %u)  - threat %f", count, unit->GetName(), unit->GetGUIDLow(), (*itr)->getThreat());
            }
            handler->SendSysMessage("End of threat list.");
            return true;
        }

        static bool HandleDebugHostileRefListCommand(ChatHandler* handler, char const* /*args*/)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)
                target = handler->GetSession()->GetPlayer();
            HostileReference* ref = target->getHostileRefManager().getFirst();
            uint32 count = 0;
            handler->PSendSysMessage("Hostil reference list of %s (guid %u)", target->GetName(), target->GetGUIDLow());
            while (ref)
            {
                if (Unit* unit = ref->getSource()->getOwner())
                {
                    ++count;
                    handler->PSendSysMessage("   %u.   %s   (guid %u)  - threat %f", count, unit->GetName(), unit->GetGUIDLow(), ref->getThreat());
                }
                ref = ref->next();
            }
            handler->SendSysMessage("End of hostil reference list.");
            return true;
        }

        static bool HandleDebugSetVehicleIdCommand(ChatHandler* handler, char const* args)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target || target->IsVehicle())
                return false;

            if (!args)
                return false;

            char* i = strtok((char*)args, " ");
            if (!i)
                return false;

            uint32 id = (uint32)atoi(i);
            //target->SetVehicleId(id);
            handler->PSendSysMessage("Vehicle id set to %u", id);
            return true;
        }

        static bool HandleDebugEnterVehicleCommand(ChatHandler* handler, char const* args)
        {
            Unit* target = handler->getSelectedUnit();
            if (!target)// || !target->IsVehicle())
                return false;

            if (!args)
                return false;

            char* i = strtok((char*)args, " ");
            if (!i)
                return false;

            char* j = strtok(NULL, " ");

            int32 entry = (int32)atoi(i);
            int8 seatId = j ? (int8)atoi(j) : -1;

            if (entry == -1)
                target->EnterVehicle(handler->GetSession()->GetPlayer(), seatId);
            else if (!entry)
                handler->GetSession()->GetPlayer()->EnterVehicle(target, seatId);
            else
            {
                Creature* passenger = NULL;
                JadeCore::AllCreaturesOfEntryInRange check(handler->GetSession()->GetPlayer(), entry, 20.0f);
                JadeCore::CreatureSearcher<JadeCore::AllCreaturesOfEntryInRange> searcher(handler->GetSession()->GetPlayer(), passenger, check);
                handler->GetSession()->GetPlayer()->VisitNearbyObject(30.0f, searcher);
                if (!passenger || passenger == target)
                    return false;
                passenger->EnterVehicle(target, seatId);
            }

            handler->PSendSysMessage("Unit %u entered vehicle %d", entry, (int32)seatId);
            return true;
        }

        static bool HandleDebugSpawnVehicleCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* e = strtok((char*)args, " ");
            char* i = strtok(NULL, " ");

            if (!e)
                return false;

            uint32 entry = (uint32)atoi(e);

            float x, y, z, o = handler->GetSession()->GetPlayer()->GetOrientation();
            handler->GetSession()->GetPlayer()->GetClosePoint(x, y, z, handler->GetSession()->GetPlayer()->GetObjectSize());

            if (!i)
                return handler->GetSession()->GetPlayer()->SummonCreature(entry, x, y, z, o);

            uint32 id = (uint32)atoi(i);

            CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(entry);

            if (!ci)
                return false;

            VehicleEntry const* ve = sVehicleStore.LookupEntry(id);

            if (!ve)
                return false;

            Creature* v = new Creature;

            Map* map = handler->GetSession()->GetPlayer()->GetMap();

            if (!v->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_VEHICLE), map, handler->GetSession()->GetPlayer()->GetPhaseMask(), entry, id, handler->GetSession()->GetPlayer()->GetTeam(), x, y, z, o))
            {
                delete v;
                return false;
            }

            map->AddToMap(v->ToCreature());

            return true;
        }

        static bool HandleDebugSendLargePacketCommand(ChatHandler* handler, char const* /*args*/)
        {
            const char* stuffingString = "This is a dummy string to push the packet's size beyond 128000 bytes. ";
            std::ostringstream ss;
            while (ss.str().size() < 128000)
                ss << stuffingString;
            handler->SendSysMessage(ss.str().c_str());
            return true;
        }

        static bool HandleDebugSendSetPhaseShiftCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* t = strtok((char*)args, " ");
            char* p = strtok(NULL, " ");
            if (!t)
                return false;

            std::set<uint32> terrainswap;
            std::set<uint32> phaseId;
            std::set<uint32> inactiveTerrainSwap;

            terrainswap.insert((uint32)atoi(t));

            if (p)
                phaseId.insert((uint32)atoi(p));

            handler->GetSession()->SendSetPhaseShift(phaseId, terrainswap, inactiveTerrainSwap);
            return true;
        }

        static bool HandleDebugGetItemValueCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* e = strtok((char*)args, " ");
            char* f = strtok(NULL, " ");

            if (!e || !f)
                return false;

            uint32 guid = (uint32)atoi(e);
            uint32 index = (uint32)atoi(f);

            Item* i = handler->GetSession()->GetPlayer()->GetItemByGuid(MAKE_NEW_GUID(guid, 0, HIGHGUID_ITEM));

            if (!i)
                return false;

            if (index >= i->GetValuesCount())
                return false;

            uint32 value = i->GetUInt32Value(index);

            handler->PSendSysMessage("Item %u: value at %u is %u", guid, index, value);

            return true;
        }

        static bool HandleDebugSetItemValueCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* e = strtok((char*)args, " ");
            char* f = strtok(NULL, " ");
            char* g = strtok(NULL, " ");

            if (!e || !f || !g)
                return false;

            uint32 guid = (uint32)atoi(e);
            uint32 index = (uint32)atoi(f);
            uint32 value = (uint32)atoi(g);

            Item* i = handler->GetSession()->GetPlayer()->GetItemByGuid(MAKE_NEW_GUID(guid, 0, HIGHGUID_ITEM));

            if (!i)
                return false;

            if (index >= i->GetValuesCount())
                return false;

            i->SetUInt32Value(index, value);

            return true;
        }

        static bool HandleDebugItemExpireCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* e = strtok((char*)args, " ");
            if (!e)
                return false;

            uint32 guid = (uint32)atoi(e);

            Item* i = handler->GetSession()->GetPlayer()->GetItemByGuid(MAKE_NEW_GUID(guid, 0, HIGHGUID_ITEM));

            if (!i)
                return false;

            handler->GetSession()->GetPlayer()->DestroyItem(i->GetBagSlot(), i->GetSlot(), true);
            sScriptMgr->OnItemExpire(handler->GetSession()->GetPlayer(), i->GetTemplate());

            return true;
        }

        //show animation
        static bool HandleDebugAnimCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            uint32 animId = atoi((char*)args);
            handler->GetSession()->GetPlayer()->HandleEmoteCommand(animId);
            return true;
        }

        static bool HandleDebugLoSCommand(ChatHandler* handler, char const* /*args*/)
        {
            if (Unit* unit = handler->getSelectedUnit())
                handler->PSendSysMessage("Unit %s (GuidLow: %u) is %sin LoS", unit->GetName(), unit->GetGUIDLow(), handler->GetSession()->GetPlayer()->IsWithinLOSInMap(unit) ? "" : "not ");
            return true;
        }

        static bool HandleDebugSetAuraStateCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
            {
                handler->SendSysMessage(LANG_BAD_VALUE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            int32 state = atoi((char*)args);
            if (!state)
            {
                // reset all states
                for (int i = 1; i <= 32; ++i)
                    unit->ModifyAuraState(AuraStateType(i), false);
                return true;
            }

            unit->ModifyAuraState(AuraStateType(abs(state)), state > 0);
            return true;
        }

        static bool HandleDebugSetValueCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* x = strtok((char*)args, " ");
            char* y = strtok(NULL, " ");
            char* z = strtok(NULL, " ");

            if (!x || !y)
                return false;

            WorldObject* target = handler->getSelectedObject();
            if (!target)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 guid = target->GetGUID();

            uint32 opcode = (uint32)atoi(x);
            if (opcode >= target->GetValuesCount())
            {
                handler->PSendSysMessage(LANG_TOO_BIG_INDEX, opcode, GUID_LOPART(guid), target->GetValuesCount());
                return false;
            }

            bool isInt32 = true;
            if (z)
                isInt32 = (bool)atoi(z);

            if (isInt32)
            {
                uint32 value = (uint32)atoi(y);
                target->SetUInt32Value(opcode , value);
                handler->PSendSysMessage(LANG_SET_UINT_FIELD, GUID_LOPART(guid), opcode, value);
            }
            else
            {
                float value = (float)atof(y);
                target->SetFloatValue(opcode , value);
                handler->PSendSysMessage(LANG_SET_FLOAT_FIELD, GUID_LOPART(guid), opcode, value);
            }

            return true;
        }

        static bool HandleDebugGetValueCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* x = strtok((char*)args, " ");
            char* z = strtok(NULL, " ");

            if (!x)
                return false;

            Unit* target = handler->getSelectedUnit();
            if (!target)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 guid = target->GetGUID();

            uint32 opcode = (uint32)atoi(x);
            if (opcode >= target->GetValuesCount())
            {
                handler->PSendSysMessage(LANG_TOO_BIG_INDEX, opcode, GUID_LOPART(guid), target->GetValuesCount());
                return false;
            }

            bool isInt32 = true;
            if (z)
                isInt32 = (bool)atoi(z);

            if (isInt32)
            {
                uint32 value = target->GetUInt32Value(opcode);
                handler->PSendSysMessage(LANG_GET_UINT_FIELD, GUID_LOPART(guid), opcode, value);
            }
            else
            {
                float value = target->GetFloatValue(opcode);
                handler->PSendSysMessage(LANG_GET_FLOAT_FIELD, GUID_LOPART(guid), opcode, value);
            }

            return true;
        }

        static bool HandleDebugMod32ValueCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            char* x = strtok((char*)args, " ");
            char* y = strtok(NULL, " ");

            if (!x || !y)
                return false;

            uint32 opcode = (uint32)atoi(x);
            int value = atoi(y);

            if (opcode >= handler->GetSession()->GetPlayer()->GetValuesCount())
            {
                handler->PSendSysMessage(LANG_TOO_BIG_INDEX, opcode, handler->GetSession()->GetPlayer()->GetGUIDLow(), handler->GetSession()->GetPlayer()->GetValuesCount());
                return false;
            }

            int currentValue = (int)handler->GetSession()->GetPlayer()->GetUInt32Value(opcode);

            currentValue += value;
            handler->GetSession()->GetPlayer()->SetUInt32Value(opcode, (uint32)currentValue);

            handler->PSendSysMessage(LANG_CHANGE_32BIT_FIELD, opcode, currentValue);

            return true;
        }

        static bool HandleDebugUpdateCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            uint32 updateIndex;
            uint32 value;

            char* index = strtok((char*)args, " ");

            Unit* unit = handler->getSelectedUnit();
            if (!unit)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (!index)
                return true;

            updateIndex = atoi(index);
            //check updateIndex
            if (unit->IsPlayer())
            {
                if (updateIndex >= PLAYER_END)
                    return true;
            }
            else if (updateIndex >= UNIT_END)
                return true;

            char* val = strtok(NULL, " ");
            if (!val)
            {
                value = unit->GetUInt32Value(updateIndex);

                handler->PSendSysMessage(LANG_UPDATE, unit->GetGUIDLow(), updateIndex, value);
                return true;
            }

            value = atoi(val);

            handler->PSendSysMessage(LANG_UPDATE_CHANGE, unit->GetGUIDLow(), updateIndex, value);

            unit->SetUInt32Value(updateIndex, value);

            return true;
        }

        static bool HandleDebugSet32BitCommand(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            WorldObject* target = handler->getSelectedObject();
            if (!target)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* x = strtok((char*)args, " ");
            char* y = strtok(NULL, " ");

            if (!x || !y)
                return false;

            uint32 opcode = (uint32)atoi(x);
            uint32 val = (uint32)atoi(y);
            if (val > 32)                                         //uint32 = 32 bits
                return false;

            uint32 value = val ? 1 << (val - 1) : 0;
            target->SetUInt32Value(opcode,  value);

            handler->PSendSysMessage(LANG_SET_32BIT_FIELD, opcode, value);
            return true;
        }

        static bool HandleWPGPSCommand(ChatHandler* handler, char const* /*args*/)
        {
            Player* player = handler->GetSession()->GetPlayer();

            sLog->outInfo(LOG_FILTER_SQL_DEV, "(@PATH, XX, %.3f, %.3f, %.5f, 0, 0, 0, 100, 0),", player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

            handler->PSendSysMessage("Waypoint SQL written to SQL Developer log");
            return true;
        }

        static bool HandleDebugPhaseCommand(ChatHandler* handler, char const* /*args*/)
        {
            Unit* unit = handler->getSelectedUnit();
            Player* player = handler->GetSession()->GetPlayer();
            if (unit && unit->IsPlayer())
                player = unit->ToPlayer();

            player->GetPhaseMgr().SendDebugReportToPlayer(handler->GetSession()->GetPlayer());
            return true;
        }

        static bool HandleDebugMoveJump(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            WorldObject* target = handler->getSelectedObject();
            if (!target || !target->ToUnit())
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* cx        = strtok((char*)args, " ");
            char* cy        = strtok(NULL, " ");
            char* cz        = strtok(NULL, " ");
            char* cspeedXY  = strtok(NULL, " ");
            char* cspeedZ   = strtok(NULL, " ");

            if (!cx || !cy || !cz || !cspeedXY || !cspeedZ)
                return false;

            float x         = (float)atof(cx);
            float y         = (float)atof(cy);
            float z         = (float)atof(cz);
            float speedXY   = (float)atof(cspeedXY);
            float speedZ    = (float)atof(cspeedZ);

            target->ToUnit()->GetMotionMaster()->MoveJump(x, y,z, speedXY, speedZ);
            return true;
        }

        static bool HandleDebugMoveBackward(ChatHandler* handler, char const* args)
        {
            if (!*args)
                return false;

            WorldObject* target = handler->getSelectedObject();
            if (!target || !target->ToUnit())
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            char* cx        = strtok((char*)args, " ");
            char* cy        = strtok(NULL, " ");
            char* cz        = strtok(NULL, " ");

            if (!cx || !cy || !cz)
                return false;

            return true;
        }

        static bool HandleDebugLoadZ(ChatHandler* handler, char const* /*args*/)
        {
            for (auto gameobject: sObjectMgr->_gameObjectDataStore)
            {
                GameObjectData data = gameobject.second;

                if (!data.posZ)
                {
                    Map* map = sMapMgr->FindMap(data.mapid, 0);

                    if (!map)
                        map = sMapMgr->CreateMap(data.mapid, 0, handler->GetSession()->GetPlayer());

                    if (map)
                    {
                        float newPosZ = map->GetHeight(data.phaseMask, data.posX, data.posY, MAX_HEIGHT, true);

                        if (newPosZ && newPosZ != -200000.0f)
                            WorldDatabase.PExecute("UPDATE gameobject SET position_z = %f WHERE guid = %u", newPosZ, gameobject.first);
                    }
                }
            }

            return true;
        }

        static bool HandleDebugSetMaxMapDiff(ChatHandler* handler, char const* /*args*/)
        {
            sMapMgr->SetMapDiffLimit(!sMapMgr->HaveMaxDiff());
            if (sMapMgr->HaveMaxDiff())
                handler->PSendSysMessage("Max creature::update diff limit activate !");
            else
                handler->PSendSysMessage("Max creature::update diff limit disable !");

            return true;
        }

        static bool HandleDebugLfgCommand(ChatHandler* p_Handler, char const * /*p_Args*/)
        {
            if (sLFGMgr->IsInDebug())
            {
                p_Handler->PSendSysMessage("Lfg debug mode is now disable");
                sLFGMgr->SetDebug(false);
            }
            else
            {
                sLFGMgr->SetDebug(true);
                p_Handler->PSendSysMessage("Lfg debug mode is now enable");
            }

            return true;
        }

        static bool HandleDebugATCommand(ChatHandler* p_Handler, char const * /*p_Args*/)
        {
            if (sSpellMgr->IsInATDebug())
            {
                p_Handler->PSendSysMessage("at debug mode is now disable");
                sSpellMgr->SetATDebug(false);
            }
            else
            {
                sSpellMgr->SetATDebug(true);
                p_Handler->PSendSysMessage("at debug mode is now enable");
            }

            return true;
        }

        static bool HandleDebugWPCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            if (sObjectMgr->IsInWPDebug())
            {
                p_Handler->PSendSysMessage("waypoint debug mode is now disable");
                sObjectMgr->SetWPDebug(false);
            }
            else
            {
                sObjectMgr->SetWPDebug(true);
                p_Handler->PSendSysMessage("waypoint debug mode is now enable");
            }

            return true;
        }

        static bool HandleDebugATPolygonCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            if (sSpellMgr->IsInATPolygonDebug())
            {
                p_Handler->PSendSysMessage("at polygon debug mode is now disable");
                sSpellMgr->SetATPolygonDebug(false);
            }
            else
            {
                sSpellMgr->SetATPolygonDebug(true);
                p_Handler->PSendSysMessage("at polygon debug mode is now enable");
            }

            return true;
        }

        static bool HandleDebugBattlegroundStart(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Battleground* l_Battleground = p_Handler->GetSession()->GetPlayer()->GetBattleground();
            if (l_Battleground == nullptr)
            {
                p_Handler->PSendSysMessage("You're not in a battleground !");
                return false;
            }

            l_Battleground->FastStart();
            return true;
        }


        /// This can be reported by static analyse, yes l_Pig is free and make it crash that the point !
        static bool HandleDebugCrashTest(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
#ifndef __clang_analyzer__
            p_Handler->PSendSysMessage("You've crash the server by adding pigs in farm that doesn't exists!");
            Player* l_CrashPlayer = nullptr;
            uint64 l_Guid         = GUID_LOPART(l_CrashPlayer->GetPetGUID());

            p_Handler->PSendSysMessage("You've crash the server ! (%lu)", l_Guid);

            Player* l_Pig = new Player(p_Handler->GetSession());
            delete l_Pig;

            l_Pig->isAFK();
            return true;
#endif
        }

        static bool HandleDebugBgAward(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Battleground* l_Battleground = p_Handler->GetSession()->GetPlayer()->GetBattleground();

            if (!l_Battleground)
            {
                p_Handler->PSendSysMessage("You're not in a battleground!");
                return false;
            }

            l_Battleground->AwardTeams(l_Battleground->GetOtherTeam(p_Handler->GetSession()->GetPlayer()->GetTeam()));
            return true;
        }

        static bool HandleDebugHeirloom(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!p_Args)
                return false;

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            char* arg1 = strtok((char*)p_Args, " ");
            char* arg2 = strtok(NULL, " ");

            if (!arg1 || !arg2)
                return false;

            int32 l_ID = atoi(arg1);
            int32 l_Flags = atoi(arg2);

            l_Player->SetDynamicValue(PLAYER_DYNAMIC_FIELD_HEIRLOOMS, l_Player->GetDynamicValues(PLAYER_DYNAMIC_FIELD_HEIRLOOMS).size(), l_ID);
            l_Player->SetDynamicValue(PLAYER_DYNAMIC_FIELD_HEIRLOOM_FLAGS, l_Player->GetDynamicValues(PLAYER_DYNAMIC_FIELD_HEIRLOOM_FLAGS).size(), l_Flags);

            return true;
        }

        static bool HandleDebugVignette(ChatHandler* p_Handler, char const* p_Args)
        {
            char* l_VignetteIDStr = strtok((char*)p_Args, " ");
            if (!l_VignetteIDStr)
                return false;

            Unit* l_SelectedUnit = p_Handler->GetSession()->GetPlayer()->GetSelectedUnit();
            if (!l_SelectedUnit)
                return false;

            uint32 l_VignetteID = atoi(l_VignetteIDStr);
            uint64 l_VignetteGUID = MAKE_NEW_GUID(l_SelectedUnit->GetGUIDLow(), l_VignetteID, HIGHGUID_VIGNETTE);


            WorldPacket l_Data(SMSG_VIGNETTE_UPDATE);
            l_Data.WriteBit(true);                                 ///< ForceUpdate
            l_Data << uint32(0);                                   ///< RemovedCount

            //for ()
            //    l_Data.appendPackGUID(IDs);

            l_Data << uint32(1);                                   ///< Added count

//            for ()
                l_Data.appendPackGUID(l_VignetteGUID);

            l_Data << uint32(1);
            {
                l_Data << float(l_SelectedUnit->GetPositionX());
                l_Data << float(l_SelectedUnit->GetPositionY());
                l_Data << float(l_SelectedUnit->GetPositionZ());
                l_Data.appendPackGUID(l_VignetteGUID);
                l_Data << uint32(l_VignetteID);
                l_Data << uint32(0);                               ///< unk
            }

            l_Data << uint32(0);                                   ///< UpdateCount
            {
            }

            l_Data << uint32(0);                                   ///< UpdateDataCounft

            p_Handler->GetSession()->SendPacket(&l_Data);
            return true;
        }

        static bool HandleDebugDumpCharTemplate(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!p_Args)
                return false;

            FILE* l_Output = fopen("./templates.sql", "w+");
            if (!l_Output)
                return false;

            std::ostringstream l_StrBuilder;
            l_StrBuilder << "TRUNCATE TABLE character_template;" << std::endl;
            l_StrBuilder << "TRUNCATE TABLE character_template_item;" << std::endl;
            l_StrBuilder << "TRUNCATE TABLE character_template_spell;" << std::endl;
            l_StrBuilder << "TRUNCATE TABLE character_template_reputation;" << std::endl << std::endl;

            bool l_FirstEntry = true;
            l_StrBuilder << "INSERT INTO character_template VALUES";
            for (int32 l_ClassId = CLASS_WARRIOR; l_ClassId < MAX_CLASSES; l_ClassId++)
            {
                ChrClassesEntry const* l_ClassEntry = sChrClassesStore.LookupEntry(l_ClassId);
                l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                             << "("
                             << l_ClassId << ", "
                             << l_ClassId << ", "
                             << "\"" << l_ClassEntry->NameLang << " - Level 100\", \"\", 100, "
                             << "100000000, -8833.07, 622.778, 93.9317, 0.6771, 0, 1569.97, -4397.41, 16.0472, 0.543025, 1, 0)";

                l_FirstEntry = false;
            }
            l_StrBuilder << ";" << std::endl << std::endl;

            const uint32 l_HordeMask = 0xFE5FFBB2;
            const uint32 l_AllianceMask = 0xFD7FFC4D;
            const uint32 l_ItemLevel = 820;
            const uint32 l_ItemType = 3; ///< 1 Shop PvE, 2 Shop PvP, 3 Test PvE, 4 Test PvP
            const uint32 l_BagItemId = 114821;

            std::string l_SearchString = p_Args; /// Case sensitive search

            std::list<uint32> l_ArmorSlotFind[3];
            std::list<uint32> l_ClassWeaponFind[3];
            std::list<uint8> l_CloaksFind[3];
            std::map<uint8, uint8> l_TrinketsFind[3];
            std::map<uint8, uint8> l_FingersFind[3];

            l_FirstEntry = true;
            l_StrBuilder << "INSERT INTO character_template_item (id, itemID, faction, count, type) VALUES ";

            ItemTemplateContainer const* l_Store = sObjectMgr->GetItemTemplateStore();
            for (ItemTemplateContainer::const_iterator l_Iter = l_Store->begin(); l_Iter != l_Store->end(); ++l_Iter)
            {
                ItemTemplate const* l_Template = &l_Iter->second;

                uint8 l_Faction = 0;

                if (l_Template->AllowableRace == l_HordeMask)
                    l_Faction = 1;
                else if (l_Template->AllowableRace == l_AllianceMask)
                    l_Faction = 2;

                if (l_Template->RequiredReputationFaction != 0)
                    continue;

                if (l_Template->Quality != ItemQualities::ITEM_QUALITY_RARE)
                    continue;

                if (l_Template->ItemId > 140000)
                    continue;

                if (l_Template->InventoryType == INVTYPE_BAG)
                {
                    uint32 l_Count = 4;

                    if (l_Template->ItemId != l_BagItemId)
                        continue;

                    for (int32 l_ClassId = CLASS_WARRIOR; l_ClassId < MAX_CLASSES; l_ClassId++)
                    {
                        int32 l_ClassMask = 1 << (l_ClassId - 1);
                        if (l_Template->AllowableClass & l_ClassMask)
                        {
                            l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_BagItemId << ", "
                                         << "1, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";
                            l_StrBuilder << "," << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_BagItemId << ", "
                                         << "2, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";

                            l_FirstEntry = false;
                        }
                    }
                }

                /// Must after bags
                if (sSpellMgr->GetItemSourceSkills(l_Template->ItemId) != nullptr)
                    continue;

                //printf("%s == %s\n", l_Template->Name1->Get(sWorld->GetDefaultDb2Locale()), l_SearchString.c_str());
                if (std::string(l_Template->Name1->Get(sWorld->GetDefaultDb2Locale())).find(l_SearchString) == std::string::npos)
                    continue;

                if (l_Template->InventoryType == INVTYPE_FINGER)
                {
                    uint32 l_Count = 1;

                    if (l_Template->ItemLevel != l_ItemLevel)
                        continue;

                    for (int32 l_ClassId = CLASS_WARRIOR; l_ClassId < MAX_CLASSES; l_ClassId++)
                    {
                        int32 l_ClassMask = 1 << (l_ClassId - 1);
                        if (l_Template->AllowableClass & l_ClassMask)
                        {
                            if (!l_Template->HasClassSpec(l_ClassId, 100))
                                continue;

                            if (l_FingersFind[l_Faction].find(l_ClassId) != l_FingersFind[l_Faction].end() && l_FingersFind[l_Faction][l_ClassId] == 2)
                                continue;

                            if (l_FingersFind[l_Faction].find(l_ClassId) == l_FingersFind[l_Faction].end())
                                l_FingersFind[l_Faction][l_ClassId] = 1;
                            else
                                l_FingersFind[l_Faction][l_ClassId] = 2;

                            l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                                << "("
                                << l_ClassId << ", "
                                << l_Template->ItemId << ", "
                                << "1, "
                                << l_Count << ", "
                                << l_ItemType
                                << ")";
                            l_StrBuilder << "," << std::endl
                                << "("
                                << l_ClassId << ", "
                                << l_Template->ItemId << ", "
                                << "2, "
                                << l_Count << ", "
                                << l_ItemType
                                << ")";

                            l_FirstEntry = false;
                        }
                    }
                }

                if (l_Template->InventoryType == INVTYPE_TRINKET)
                {
                    uint32 l_Count = 1;

                    if (l_Template->ItemLevel != l_ItemLevel)
                        continue;

                    for (int32 l_ClassId = CLASS_WARRIOR; l_ClassId < MAX_CLASSES; l_ClassId++)
                    {
                        int32 l_ClassMask = 1 << (l_ClassId - 1);
                        if (l_Template->AllowableClass & l_ClassMask)
                        {
                            if (!l_Template->HasClassSpec(l_ClassId, 100))
                                continue;

                            if (l_TrinketsFind[l_Faction].find(l_ClassId) != l_TrinketsFind[l_Faction].end() && l_TrinketsFind[l_Faction][l_ClassId] == 2)
                                continue;

                            if (l_TrinketsFind[l_Faction].find(l_ClassId) == l_TrinketsFind[l_Faction].end())
                                l_TrinketsFind[l_Faction][l_ClassId] = 1;
                            else
                                l_TrinketsFind[l_Faction][l_ClassId] = 2;

                            l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_Template->ItemId << ", "
                                         << "1, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";
                            l_StrBuilder << "," << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_Template->ItemId << ", "
                                         << "2, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";

                             l_FirstEntry = false;
                        }
                    }
                }

                if (l_Template->InventoryType == INVTYPE_CLOAK)
                {
                    uint32 l_Count = 1;

                    if (l_Template->ItemLevel != l_ItemLevel)
                        continue;

                    for (int32 l_ClassId = CLASS_WARRIOR; l_ClassId < MAX_CLASSES; l_ClassId++)
                    {
                        int32 l_ClassMask = 1 << (l_ClassId - 1);
                        if (l_Template->AllowableClass & l_ClassMask)
                        {
                            if (!l_Template->HasClassSpec(l_ClassId, 100))
                                continue;

                            if (std::find(l_CloaksFind[l_Faction].begin(), l_CloaksFind[l_Faction].end(), l_ClassId) != l_CloaksFind[l_Faction].end())
                                continue;

                            l_CloaksFind[l_Faction].push_back(l_ClassId);

                            l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_Template->ItemId << ", "
                                         << "1, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";
                            l_StrBuilder << "," << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_Template->ItemId << ", "
                                         << "2, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";

                            l_FirstEntry = false;
                        }
                    }
                }

                /*if (l_Template->Class == ITEM_CLASS_WEAPON)
                {
                    uint32 l_Count = 1;

                    if (l_Template->ItemLevel != l_ItemLevel)
                        continue;

                    for (int32 l_ClassId = CLASS_WARRIOR; l_ClassId < MAX_CLASSES; l_ClassId++)
                    {
                        int32 l_ClassMask = 1 << (l_ClassId - 1);
                        if (l_Template->AllowableClass & l_ClassMask)
                        {
                            if (!l_Template->HasClassSpec(l_ClassId, 100))
                                continue;

                            if (std::find(l_ClassWeaponFind[l_Faction].begin(), l_ClassWeaponFind[l_Faction].end(), l_ClassId | l_Template->SubClass << 16) != l_ClassWeaponFind[l_Faction].end())
                                continue;

                            l_ClassWeaponFind[l_Faction].push_back(l_ClassId | l_Template->SubClass << 16);

                            l_Count = l_Template->IsOneHanded() || (l_Template->IsTwoHandedWeapon() && l_ClassId == CLASS_WARRIOR) ? 2 : 1;
                            l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_Template->ItemId << ", "
                                         << "1, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";
                            l_StrBuilder << "," << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_Template->ItemId << ", "
                                         << "2, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";

                            l_FirstEntry = false;
                        }
                    }
                }*/

                /// Armor
                //if (std::string(l_Template->Name1->Get(LOCALE_enUS)).find(l_SearchString) != std::string::npos)
                {
                    if (l_Template->Class != ITEM_CLASS_ARMOR ||
                        l_Template->InventoryType == INVTYPE_BAG ||
                        l_Template->InventoryType == INVTYPE_TRINKET ||
                        l_Template->InventoryType == INVTYPE_CLOAK)
                        continue;

                    uint32 l_Count = 1;

                    if (l_Template->ItemLevel != l_ItemLevel)
                        continue;

                    for (uint32 l_SpecId = 0; l_SpecId < sChrSpecializationsStore.GetNumRows(); l_SpecId++)
                    {
                        auto l_Specialization = sChrSpecializationsStore.LookupEntry(l_SpecId);
                        if (!l_Specialization)
                            continue;

                        int32 l_ClassId = l_Specialization->ClassID;

                        int32 l_ClassMask = 1 << (l_ClassId - 1);
                        if (l_Template->AllowableClass & l_ClassMask)
                        {
                            if (l_Template->Class == ITEM_CLASS_ARMOR)
                            {
                                switch (l_Template->SubClass)
                                {
                                    case ITEM_SUBCLASS_ARMOR_CLOTH:
                                        switch (l_ClassId)
                                        {
                                            case CLASS_PRIEST:
                                            case CLASS_MAGE:
                                            case CLASS_WARLOCK:
                                                break;
                                            default:
                                                continue;
                                        }
                                        break;
                                    case ITEM_SUBCLASS_ARMOR_LEATHER:
                                        switch (l_ClassId)
                                        {
                                            case CLASS_ROGUE:
                                            case CLASS_MONK:
                                            case CLASS_DRUID:
                                            case CLASS_DEMON_HUNTER:
                                                break;
                                            default:
                                                continue;
                                        }
                                        break;
                                    case ITEM_SUBCLASS_ARMOR_MAIL:
                                        switch (l_ClassId)
                                        {
                                            case CLASS_HUNTER:
                                            case CLASS_SHAMAN:
                                                break;
                                            default:
                                                continue;
                                        }
                                        break;
                                    case ITEM_SUBCLASS_ARMOR_PLATE:
                                        switch (l_ClassId)
                                        {
                                            case CLASS_PALADIN:
                                            case CLASS_WARRIOR:
                                            case CLASS_DEATH_KNIGHT:
                                                break;
                                            default:
                                                continue;
                                        }
                                        break;
                                    default:
                                        break;
                                }
                            }

                            if (!l_Template->HasSpec((SpecIndex)l_SpecId, 100))
                                continue;

                            if (std::find(l_ArmorSlotFind[l_Faction].begin(), l_ArmorSlotFind[l_Faction].end(), l_SpecId | l_Template->InventoryType << 16) != l_ArmorSlotFind[l_Faction].end())
                                continue;

                            l_ArmorSlotFind[l_Faction].push_back(l_SpecId | l_Template->InventoryType << 16);

                            l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_Template->ItemId << ", "
                                         << "1, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";
                            l_StrBuilder << "," << std::endl
                                         << "("
                                         << l_ClassId << ", "
                                         << l_Template->ItemId << ", "
                                         << "2, "
                                         << l_Count << ", "
                                         << l_ItemType
                                         << ")";

                            l_FirstEntry = false;
                        }
                    }
                }
            }

            const uint32 l_Count = 200;
            const uint32 l_TomeOfTheClearMindId = 79249;

            for (int32 l_ClassId = CLASS_WARRIOR; l_ClassId < MAX_CLASSES; l_ClassId++)
            {
                l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                            << "("
                            << l_ClassId << ", "
                            << l_TomeOfTheClearMindId << ", "
                            << "1, "
                            << l_Count << ", "
                            << l_ItemType
                            << ")";
                l_StrBuilder << "," << std::endl
                            << "("
                            << l_ClassId << ", "
                            << l_TomeOfTheClearMindId << ", "
                            << "2, "
                            << l_Count << ", "
                            << l_ItemType
                            << ")";
            }

            l_StrBuilder << ";" << std::endl << std::endl;

            l_FirstEntry = true;
            l_StrBuilder << "INSERT INTO character_template_spell VALUES";

//            /// Learn Dual spec for all classes
//            for (int32 l_ClassId = CLASS_WARRIOR; l_ClassId < MAX_CLASSES; l_ClassId++)
//            {
//                l_StrBuilder << std::endl;
//                l_StrBuilder << "(" << l_ClassId << ", 63644)," << std::endl;
//                l_StrBuilder << "(" << l_ClassId << ", 63645),";
//            }

            for (uint32 l_ID = 0; l_ID < sSpellStore.GetNumRows(); ++l_ID)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_ID);
                if (!l_SpellInfo)
                    continue;

                SpellEffectInfo const* l_EffectInfo = l_SpellInfo->GetEffectByType(SPELL_EFFECT_APPLY_GLYPH);
                if (!l_EffectInfo)
                    continue;

                if (GlyphPropertiesEntry const* l_Glyph = sGlyphPropertiesStore.LookupEntry(l_EffectInfo->MiscValue))
                {
                    if (SpellInfo const* l_GlyphInfo = sSpellMgr->GetSpellInfo(l_Glyph->SpellID))
                    {
                        /// First value should be the template id, but with us, it's the same as classid
                        uint32 l_ClassId = l_GlyphInfo->GetClassIDBySpellFamilyName();

                        l_StrBuilder << (l_FirstEntry ? "" : ",") << std::endl
                                     << "("
                                     << l_ClassId << ", "
                                     << l_SpellInfo->Id
                                     << ")";

                        l_FirstEntry = false;
                    }
                }
            }

            l_StrBuilder << ";" << std::endl;
            std::string l_FinalString = l_StrBuilder.str();

            fwrite(l_FinalString.c_str(), l_FinalString.length(), 1, l_Output);
            fflush(l_Output);
            fclose(l_Output);

            p_Handler->PSendSysMessage("Characters templates generated !");

            return true;
        }

        static bool HandleDebugDumpRewardlessMissions(ChatHandler* /*p_Handler*/, char const* p_Args)
        {
            if (!p_Args)
                return false;

            /// @TODO(2CC3CACB) GarrMissionReward is gone
            ///FILE* l_Output = fopen("./rewardless_ids.txt", "w+");
            ///if (!l_Output)
            ///    return false;
            ///
            ///std::ostringstream l_StrBuilder;
            ///std::list<uint32> l_AlreadyInserted;
            ///
            ///for (uint32 l_ID = 0; l_ID < sGarrMissionRewardStore.GetNumRows(); l_ID++)
            ///{
            ///    GarrMissionRewardEntry const* l_Entry = sGarrMissionRewardStore.LookupEntry(l_ID);
            ///
            ///    if (!l_Entry)
            ///        continue;
            ///
            ///    if (!l_Entry->ItemID)
            ///        continue;
            ///
            ///    ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(l_Entry->ItemID);
            ///
            ///    if (!l_Template)
            ///        continue;
            ///
            ///    if (!(l_Template->Flags & int32(ItemFlags::HAS_LOOT)))
            ///        continue;
            ///
            ///    if (LootTemplates_Item.HaveLootFor(l_Template->ItemId))
            ///        continue;
            ///
            ///    if (std::find(l_AlreadyInserted.begin(), l_AlreadyInserted.end(), l_Entry->ItemID) != l_AlreadyInserted.end())
            ///        continue;
            ///
            ///    l_AlreadyInserted.push_back(l_Entry->ItemID);
            ///    l_StrBuilder << l_Entry->ItemID << " \\ " << l_Template->Name1->Get(LOCALE_enUS) << "\n";
            ///}
            ///
            ///std::string l_String = l_StrBuilder.str();
            ///fwrite(l_String.c_str(), l_String.length(), 1, l_Output);
            ///fflush(l_Output);
            ///fclose(l_Output);

            return true;
        }

        static bool HandleSpellDebugDumpRewardlessMissions(ChatHandler* /*p_Handler*/, char const* p_Args)
        {
            if (!p_Args)
                return false;

            /// @TODO(2CC3CACB) GarrMissionReward is gone
            ///FILE* l_Output = fopen("./rewardless_spell_ids.txt", "w+");
            ///if (!l_Output)
            ///    return false;
            ///
            ///FILE* l_SQL = fopen("./temp_loot_table.sql", "w+");
            ///if (!l_SQL)
            ///    return false;
            ///
            ///std::ostringstream l_StrBuilder;
            ///std::ostringstream l_StrBuilder2;
            ///std::list<uint32> l_AlreadyInserted;
            ///
            ///for (uint32 l_ID = 0; l_ID < sGarrMissionRewardStore.GetNumRows(); l_ID++)
            ///{
            ///    GarrMissionRewardEntry const* l_Entry = sGarrMissionRewardStore.LookupEntry(l_ID);
            ///
            ///    if (!l_Entry)
            ///        continue;
            ///
            ///    if (!l_Entry->ItemID)
            ///        continue;
            ///
            ///    ItemTemplate const* l_Template = sObjectMgr->GetItemTemplate(l_Entry->ItemID);
            ///
            ///    if (!l_Template)
            ///        continue;
            ///
            ///    uint32 l_SpellId = l_Template->Spells[0].SpellId;
            ///
            ///    if (!l_SpellId)
            ///        continue;
            ///
            ///    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_SpellId);
            ///
            ///    if (!l_SpellInfo)
            ///        continue;
            ///
            ///    if (!l_SpellInfo->HasEffect(SPELL_EFFECT_CREATE_LOOT))
            ///        continue;
            ///
            ///    if (LootTemplates_Spell.HaveLootFor(l_SpellId))
            ///        continue;
            ///
            ///    if (std::find(l_AlreadyInserted.begin(), l_AlreadyInserted.end(), l_Entry->ItemID) != l_AlreadyInserted.end())
            ///        continue;
            ///
            ///    l_AlreadyInserted.push_back(l_Entry->ItemID);
            ///    l_StrBuilder << l_Entry->ItemID << " \\ " << l_Template->Name1->Get(LOCALE_enUS) << "\n";
            ///    l_StrBuilder2 << "UPDATE temp_loot_table SET entry = " << l_SpellId << " WHERE entry = " << l_Entry->ItemID << ";\n";
            ///    l_StrBuilder2 << "DELETE FROM spell_loot_tempalte WHERE entry = " << l_SpellId << ";\n";
            ///}
            ///
            ///std::string l_String = l_StrBuilder.str();
            ///fwrite(l_String.c_str(), l_String.length(), 1, l_Output);
            ///fflush(l_Output);
            ///fclose(l_Output);
            ///
            ///std::string l_String2 = l_StrBuilder2.str();
            ///fwrite(l_String2.c_str(), l_String2.length(), 1, l_SQL);
            ///fflush(l_SQL);
            ///fclose(l_SQL);

            return true;
        }

        static bool HandleDebugPlayerCondition(ChatHandler* p_Handler, char const* p_Args)
        {
            char* l_ArgStr = strtok((char*)p_Args, " ");
            if (!l_ArgStr)
                return false;

            uint32 l_ConditionID = atoi(l_ArgStr);

            auto l_Result = p_Handler->GetSession()->GetPlayer()->EvalPlayerCondition(l_ConditionID);

            if (l_Result.first)
                p_Handler->PSendSysMessage("Condition %u is satisfied", l_ConditionID);
            else
                p_Handler->PSendSysMessage("Condition %u failed => %s", l_ConditionID,  l_Result.second.c_str());

            return true;
        }

        static bool HandleDebugPacketProfiler(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            /*gPacketProfilerMutex.lock();
            p_Handler->PSendSysMessage("----------------");

            for (auto l_Pair : gPacketProfilerData)
            {
                p_Handler->PSendSysMessage("%s => %u", GetOpcodeNameForLogging((Opcodes)l_Pair.first, WOW_SERVER_TO_CLIENT).c_str(), l_Pair.second);
            }

            p_Handler->PSendSysMessage("----------------");
            gPacketProfilerMutex.unlock();*/

            return true;
        }

        static bool HandleHotfixOverride(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!p_Args)
                return false;

            auto l_SendHotfixPacket = [&p_Handler](DB2StorageBase* p_Store, uint32 p_Entry) -> void
            {
                ByteBuffer l_ResponseData(2 * 1024);
                if (p_Store->WriteRecord(p_Entry, l_ResponseData, p_Handler->GetSessionDbLocaleIndex(), sObjectMgr->HotfixTableIndexInfos[p_Store->GetHash()]))
                {
                    WorldPacket l_Data(SMSG_DB_REPLY, 4 + 4 + 4 + 4 + l_ResponseData.size());
                    l_Data << uint32(p_Store->GetHash());
                    l_Data << uint32(p_Entry);
                    l_Data << uint32(sObjectMgr->GetHotfixDate(p_Entry, p_Store->GetHash()));
                    l_Data.WriteBit(1);                                                         ///< Found ???
                    l_Data << uint32(l_ResponseData.size());
                    l_Data.append(l_ResponseData);

                    p_Handler->GetSession()->SendPacket(&l_Data);
                }
                else
                {
                    WorldPacket l_Data(SMSG_DB_REPLY, 4 + 4 + 4 + 4);
                    l_Data << uint32(p_Store->GetHash());
                    l_Data << uint32(-int32(p_Entry));
                    l_Data << uint32(time(NULL));
                    l_Data.WriteBit(0);                                                         ///< Not Found ???
                    l_Data << uint32(0);

                    p_Handler->GetSession()->SendPacket(&l_Data);
                }
            };
            char* arg1 = strtok((char*)p_Args, " ");
            char* arg2 = strtok(NULL, " ");

            if (!arg1 || !arg2)
                return false;

            std::string l_HotfixType = arg1;
            uint32 l_HotfixEntry = atoi(arg2);
            if (l_HotfixType == "item")
            {
                l_SendHotfixPacket(&sItemStore, l_HotfixEntry);
                l_SendHotfixPacket(&sItemSparseStore, l_HotfixEntry);

                for (uint32 i = 0; i < sItemEffectStore.GetNumRows(); ++i)
                {
                    ItemEffectEntry const* l_Entry = sItemEffectStore.LookupEntry(i);
                    if (!l_Entry || l_Entry->ItemID != l_HotfixEntry)
                        continue;

                    l_SendHotfixPacket(&sItemEffectStore, i);
                }

                std::vector<uint32> l_Appearances;
                for (uint32 i = 0; i < sItemModifiedAppearanceStore.GetNumRows(); ++i)
                {
                    ItemModifiedAppearanceEntry const* l_Entry = sItemModifiedAppearanceStore.LookupEntry(i);
                    if (!l_Entry || l_Entry->ItemID != l_HotfixEntry)
                        continue;

                    l_SendHotfixPacket(&sItemModifiedAppearanceStore, i);
                    l_Appearances.push_back(i);
                }

                for (uint32 l_AppearanceID : l_Appearances)
                {
                    ItemAppearanceEntry const* l_Entry = sItemAppearanceStore.LookupEntry(l_AppearanceID);
                    if (!l_Entry)
                        continue;

                    l_SendHotfixPacket(&sItemAppearanceStore, l_AppearanceID);
                }

            }

            return true;
        }

        static bool HandleDebugMirrorCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player*   l_Player = p_Handler->GetSession()->GetPlayer();
            Creature* l_Target = p_Handler->getSelectedCreature();

            if (l_Target == nullptr || l_Player == nullptr)
                return false;

            ///                                                   0   1       2        3         4     5           6         7           8
            QueryResult l_Result = LoginDatabase.PQuery("SELECT race, gender, class, skinColor, face, hairStyle, hairColor, facialHair, equipment FROM character_renderer_queue ORDER BY RAND() LIMIT 1");
            if (!l_Result)
                return false;

            Field* l_Fields = l_Result->Fetch();

            ChrRacesEntry const* l_RaceEntry = sChrRacesStore.LookupEntry(l_Fields[0].GetUInt8());
            if (!l_RaceEntry)
                return false;

            uint32 l_Display = l_Fields[1].GetUInt8() == 0 ? l_RaceEntry->MaleDisplayID : l_RaceEntry->FemaleDisplayID;
            std::list<uint32> l_Displays;

            Tokenizer l_Equipements(l_Fields[8].GetString(), ']');
            for (Tokenizer::const_iterator l_It = l_Equipements.begin(); l_It != l_Equipements.end(); ++l_It)
            {
                std::string l_DisplayIdTxt = (*l_It);
                if (l_DisplayIdTxt == ",")
                    continue;

                for (auto l_Idx = l_DisplayIdTxt.size(); l_Idx != 0; l_Idx--)
                {
                    if (l_DisplayIdTxt[l_Idx] == ',')
                    {
                        l_Displays.push_back(atoi(l_DisplayIdTxt.substr(l_Idx + 1, std::string::npos).c_str()));
                        break;
                    }
                }
            }

            /// Update display & add mirror image flags
            l_Target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
            l_Target->SetDisplayId(l_Display);

            /// Forge SMSG_UPDATE_OBJECT, client need to receive it before SMSG_MIRROR_IMAGE_COMPONENTED_DATA
            UpdateData  l_UpdateData(l_Target->GetMapId());
            WorldPacket l_Packet;

            l_Target->BuildValuesUpdateBlockForPlayer(&l_UpdateData, l_Player);

            if (l_UpdateData.BuildPacket(&l_Packet))
                l_Player->GetSession()->SendPacket(&l_Packet);


            /// Forge SMSG_MIRROR_IMAGE_COMPONENTED_DATA
            WorldPacket data(SMSG_MIRROR_IMAGE_COMPONENTED_DATA, 76);

            data.appendPackGUID(l_Target->GetGUID());
            data << uint32(l_Display);
            data << uint8(l_Fields[0].GetUInt8());
            data << uint8(l_Fields[1].GetUInt8());
            data << uint8(l_Fields[2].GetUInt8());
            data << uint8(l_Fields[3].GetUInt8());      // skin
            data << uint8(l_Fields[4].GetUInt8());      // face
            data << uint8(l_Fields[5].GetUInt8());      // hair
            data << uint8(l_Fields[6].GetUInt8());      // haircolor
            data << uint8(l_Fields[7].GetUInt8());      // facialhair
            data.appendPackGUID(0);

            data << uint32(l_Displays.size());

            for (auto l_DisplayId : l_Displays)
                data << uint32(l_DisplayId);

            /*static EquipmentSlots const itemSlots[] =
            {
                EQUIPMENT_SLOT_HEAD,
                EQUIPMENT_SLOT_SHOULDERS,
                EQUIPMENT_SLOT_BODY,
                EQUIPMENT_SLOT_CHEST,
                EQUIPMENT_SLOT_WAIST,
                EQUIPMENT_SLOT_LEGS,
                EQUIPMENT_SLOT_FEET,
                EQUIPMENT_SLOT_WRISTS,
                EQUIPMENT_SLOT_HANDS,
                EQUIPMENT_SLOT_TABARD,
                EQUIPMENT_SLOT_BACK,
                EQUIPMENT_SLOT_END
            };

            // Display items in visible slots
            for (EquipmentSlots const* itr = itemSlots; *itr != EQUIPMENT_SLOT_END; ++itr)
            {
                if (*itr == EQUIPMENT_SLOT_HEAD && l_Player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM))
                    data << uint32(0);
                else if (*itr == EQUIPMENT_SLOT_BACK && l_Player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_HIDE_CLOAK))
                    data << uint32(0);
                else if (ItemTemplate const* l_Proto = sObjectMgr->GetItemTemplate(l_Player->GetUInt32Value(PLAYER_FIELD_VISIBLE_ITEMS + (*itr * 3))))
                    data << uint32(l_Proto->DisplayInfoID);
                else
                    data << uint32(0);
            }*/

            l_Player->GetSession()->SendPacket(&data);

            return true;
        }

        static bool HandleDebugPvELogsCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            EncounterDatas l_Datas;

            l_Datas.EncounterID     = 1704;                                         ///< BlackHand
            l_Datas.Expansion       = Expansion::EXPANSION_WARLORDS_OF_DRAENOR;
            l_Datas.RealmID         = g_RealmID;
            l_Datas.GuildID         = 9;
            l_Datas.GuildFaction    = TeamId::TEAM_HORDE;
            l_Datas.GuildName = l_Player->GetGuildName();
            l_Datas.MapID           = 1205;                                         ///< Blackrock Foundry
            l_Datas.DifficultyID    = 16;                                           ///< Mythic mode
            l_Datas.StartTime       = time(nullptr);
            l_Datas.CombatDuration  = 7 * TimeConstants::MINUTE;
            l_Datas.EndTime         = l_Datas.StartTime + l_Datas.CombatDuration;
            l_Datas.Success         = true;

            RosterData l_Data;
            l_Data.GuidLow      = l_Player->GetGUIDLow();
            l_Data.Name         = l_Player->GetName();
            l_Data.Level        = l_Player->getLevel();
            l_Data.Class        = l_Player->getClass();
            l_Data.SpecID       = l_Player->GetActiveSpecializationID();
            l_Data.Role         = l_Player->GetRoleForGroup();
            l_Data.ItemLevel    = l_Player->GetAverageItemLevelEquipped();

            l_Datas.RosterDatas.push_back(l_Data);

            sScriptMgr->OnEncounterEnd(&l_Datas);
            return true;
        }

        static bool HandleDebugQuestLogsCommand(ChatHandler* p_Handler, char const* /*p_Args*/)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            if (!l_Player->m_IsDebugQuestLogs)
            {
                p_Handler->PSendSysMessage(LANG_DEBUG_QUEST_LOGS_ON);
                l_Player->m_IsDebugQuestLogs = true;
            }
            else
            {
                p_Handler->PSendSysMessage(LANG_DEBUG_QUEST_LOGS_OFF);
                l_Player->m_IsDebugQuestLogs = false;
            }
            return true;
        }

        static bool HandleDebugAddUnitStateCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(TrinityStrings::LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            /// Returns and send error if no Unit is selected
            Unit* l_Unit = p_Handler->getSelectedUnit();
            if (!l_Unit)
            {
                p_Handler->SendSysMessage(TrinityStrings::LANG_SELECT_CHAR_OR_CREATURE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            int32 l_State = atoi((char*)p_Args);

            /// When p_Args == 0, all unit states are cleared
            if (!l_State)
            {
                l_Unit->ClearUnitState(UnitState::UNIT_STATE_ALL_STATE);
                p_Handler->PSendSysMessage("Unit states cleared");
                return true;
            }

            bool l_Found = false;
            auto l_Itr = g_Unitstates.find((UnitState)l_State);

            if (l_Itr != g_Unitstates.end())
            {
                /// If selected Unit has not the p_Args state, it's added
                /// If the state doesn't exist, it returns
                if (!l_Unit->HasUnitState(l_State))
                {
                    for (auto l_UnitState : g_Unitstates)
                    {
                        if (l_UnitState.first == (UnitState)l_State)
                        {
                            l_Found = true;
                            break;
                        }
                    }

                    if (!l_Found)
                    {
                        p_Handler->PSendSysMessage("The unit_state you're looking for doesn't exist.");
                        return true;
                    }

                    l_Unit->AddUnitState((UnitState)l_State);
                    p_Handler->PSendSysMessage("Unit state %s has been added", l_Itr->second);
                }
                else
                    p_Handler->PSendSysMessage("This unit has already the state %s", l_Itr->second);
            }

            return true;
        }

        static bool HandleDebugGetUnitStatesCommand(ChatHandler* p_Handler, const char* /*p_Args*/)
        {
            Unit* l_Unit = p_Handler->getSelectedUnit();
            if (!l_Unit)
            {
                p_Handler->SendSysMessage(TrinityStrings::LANG_SELECT_CHAR_OR_CREATURE);
                p_Handler->SetSentErrorMessage(true);
                return true;
            }

            /// Checks every unit_state for the Unit selected, and displays it in the handler's chatbox
            for (auto l_UnitState : g_Unitstates)
            {
                if (l_Unit->HasUnitState(uint32(l_UnitState.first)))
                    p_Handler->PSendSysMessage("Selected unit has %s", l_UnitState.second);
            }

            return true;
        }

        static bool HandleDebugRemoveUnitStateCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
            {
                p_Handler->SendSysMessage(TrinityStrings::LANG_BAD_VALUE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            Unit* l_Unit = p_Handler->getSelectedUnit();
            if (!l_Unit)
            {
                p_Handler->SendSysMessage(TrinityStrings::LANG_SELECT_CHAR_OR_CREATURE);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            /// Checks if the p_Arg has a correct value
            int32 l_State = atoi((char*)p_Args);
            if (!l_State)
            {
                p_Handler->PSendSysMessage("There is no unit_state with this value.");
                return true;
            }

            /// Removes the state to the Unit if it has it
            auto l_Itr = g_Unitstates.find((UnitState)l_State);

            if (l_Itr != g_Unitstates.end())
            {
                if (l_Unit->HasUnitState(l_Itr->first))
                {
                    l_Unit->ClearUnitState(l_Itr->first);
                    p_Handler->PSendSysMessage("Unit_state %s has been removed", l_Itr->second);
                }
                else
                    p_Handler->PSendSysMessage("This unit doesn't have the unit state %s", l_Itr->second);
            }

            return true;
        }

        static bool HandleDebugSetInstanceSizeCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            if (!l_Player)
                return false;

            Map* l_Map = l_Player->GetMap();
            if (!l_Map || l_Map->IsMythic())
                return false;

            MapDifficultyEntry const* l_MapDifficultyEntry = l_Map->GetMapDifficulty();
            InstanceScript* l_Instance = l_Player->GetInstanceScript();
            if (!l_Instance || !l_MapDifficultyEntry)
                return false;

            uint32 l_Size = (uint32)atoi((char*)p_Args);
            if (l_Size > l_MapDifficultyEntry->MaxPlayers)
                l_Size = l_MapDifficultyEntry->MaxPlayers;

            l_Map->SetCustomMapSize(l_Size);
            l_Instance->UpdateCreatureGroupSizeStats();

            if (l_Size == 0)
            {
                l_Size = l_Map->GetPlayersCountExceptGMs();
                p_Handler->PSendSysMessage("This instance's size is not longer custom: it will scale normally");
            }
            else
                p_Handler->PSendSysMessage("This instance's size has been set to %u", l_Size);

            l_Player->SendInstanceGroupSizeChanged(l_Size);
            return true;
        }

        static bool HandleDebugScenarioCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            uint32 l_ScenarioID = (uint32)atoi((char*)p_Args);

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            if (l_Player == nullptr)
                return false;

            WorldPacket l_Data(SMSG_SCENARIO_STATE);

            l_Data << int32(l_ScenarioID);
            l_Data << int32(0);
            l_Data << uint32(Difficulty::DifficultyMythicKeystone);
            l_Data << uint32(0);
            l_Data << uint32(0);
            l_Data << uint32(0);

            l_Data << uint32(0);
            l_Data << uint32(0);

            l_Data.WriteBit(false);
            l_Data.FlushBits();

            l_Player->SendDirectMessage(&l_Data);
            return true;
        }

        static bool HandleDebugCurveValueCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            char* arg1 = strtok((char*)p_Args, " ");
            char* arg2 = strtok(NULL, " ");

            if (!arg1 || !arg2)
                return false;

            uint32 l_CurveID = std::atoi(arg1);
            float  l_X       = atof(arg2);

            float l_Result = GetCurveValueAt(l_CurveID, l_X);
            std::ostringstream l_Txt;
            l_Txt << l_Result;

            p_Handler->SendGlobalSysMessage(l_Txt.str().c_str());

            return true;
        }

        static bool HandleShowScenarioStepCriteriaCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            uint32 l_ScenarioID = (uint32)atoi((char*)p_Args);

            ScenarioEntry const* l_ScenarioEntry = sScenarioStore.LookupEntry(l_ScenarioID);
            if (!l_ScenarioEntry)
                return false;

            std::ostringstream l_ScenarioDetails;
            l_ScenarioDetails << "Scenario ID: " << l_ScenarioEntry->ID << " name " << l_ScenarioEntry->NameLang->Str[0] << " steps details:";

            p_Handler->SendSysMessage(l_ScenarioDetails.str().c_str());

            ScenarioSteps const* l_Steps = sScenarioMgr->GetScenarioSteps(l_ScenarioID);
            if (!l_Steps)
                return false;

            for (uint32 l_I = 0; l_I < l_Steps->size(); l_I++)
            {
                ScenarioStepEntry const* l_Step = l_Steps->at(l_I);
                if (!l_Step)
                    continue;

                std::ostringstream l_StepDetails;
                l_StepDetails << "--> Step [" << std::to_string((uint32)l_Step->Step) << "]: Name: " << l_Step->TitleLang->Str[0] << " Criteria Tree: " << std::to_string(l_Step->CriteriaTreeID);
                p_Handler->SendSysMessage(l_StepDetails.str().c_str());

                CriteriaTree const* l_Tree = sCriteriaMgr->GetCriteriaTree(l_Step->CriteriaTreeID);
                if (l_Tree)
                {
                    for (auto l_Child : l_Tree->Children)
                    {
                        if (l_Child->Criteria && l_Child->Criteria->Entry)
                        {
                            std::ostringstream l_CriteriaDetails;
                            l_CriteriaDetails << "----> CriteriaID " << l_Child->Criteria->Entry->ID << " Type " << sCriteriaMgr->GetCriteriaTypeString(l_Child->Criteria->Entry->Type) << " Asset " << std::to_string(l_Child->Criteria->Entry->Asset.ID);
                            p_Handler->SendSysMessage(l_CriteriaDetails.str().c_str());
                        }
                    }
                }
            }

            return true;
        }

        static bool HandleShowAchievementCriteriaCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            uint32 l_AchievementID = (uint32)atoi((char*)p_Args);

            AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(l_AchievementID);
            if (!l_AchievementEntry)
                return false;

            std::ostringstream l_AchievementDetails;
            l_AchievementDetails << "Achievement ID: " << l_AchievementEntry->ID << " name " << l_AchievementEntry->TitleLang->Str[0] << " details:";
            p_Handler->SendSysMessage(l_AchievementDetails.str().c_str());

            std::function<void(CriteriaTree const* p_Tree)> l_ExploreChildren = [&](CriteriaTree const* p_Tree)->void
            {
                if (!p_Tree)
                    return;

                std::ostringstream l_CriteriaDetails;
                l_CriteriaDetails << "----> CriteriaTreeID " << p_Tree->ID << " Amount " << p_Tree->Entry->Amount;
                p_Handler->SendSysMessage(l_CriteriaDetails.str().c_str());
                for (auto l_Child : p_Tree->Children)
                {
                    if (l_Child->Criteria && l_Child->Criteria->Entry)
                    {
                        std::ostringstream l_CriteriaDetails2;
                        l_CriteriaDetails2 << "----> CriteriaID " << l_Child->Criteria->Entry->ID << " Type " << sCriteriaMgr->GetCriteriaTypeString(l_Child->Criteria->Entry->Type) << " Asset " << std::to_string(l_Child->Criteria->Entry->Asset.ID);
                        p_Handler->SendSysMessage(l_CriteriaDetails2.str().c_str());
                    }
                    l_ExploreChildren(l_Child);
                }
            };

            CriteriaTree const* l_Tree = sCriteriaMgr->GetCriteriaTree(l_AchievementEntry->CriteriaTree);
            if (l_Tree)
                l_ExploreChildren(l_Tree);

            return true;
        }

        static bool HandleDebugSendEventScenarioCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            uint32 l_EventID = (uint32)atoi((char*)p_Args);

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            if (!l_Player)
                return false;

            Map* l_Map = l_Player->GetMap();
            if (!l_Map)
                return false;

            Scenario* l_Scenario = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
            if (!l_Scenario)
                return false;

            l_Scenario->UpdateAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_SEND_EVENT_SCENARIO, l_EventID, 0, 0, l_Player, l_Player);

            return true;
        }

        static bool HandleDebugCheckGroupRolesCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            uint32 l_IsOk = 0;

            for (int l_GroupRoll = 0; l_GroupRoll < 500; l_GroupRoll++)
            {
                LfgRolesMap l_Roles;
                int l_GroupSize = urand(6, 25);
                for (int l_I = 0; l_I < l_GroupSize; l_I++)
                    l_Roles[l_I] = urand(1, 7) * 2;

                auto l_Roles2 = l_Roles;
                if (sLFGMgr->CheckGroupRoles(l_Roles, LfgCategory::LFG_CATEGORIE_RAID, 0))
                {
                    l_IsOk++;
                    printf("======================= NEW GROUP ===========================\n");
                    std::map<int8, int8> l_RolesFound = { { LFG_ROLEMASK_TANK , 0}, { LFG_ROLEMASK_DAMAGE , 0}, { LFG_ROLEMASK_HEALER , 0}};
                    for (auto l_Itr : l_Roles)
                        l_RolesFound[l_Itr.second]++;

                    for (auto l_Itr : l_RolesFound)
                        printf("%u : %u\n", l_Itr.first, l_Itr.second);
                    printf("============================================================\n");

                    sLFGMgr->CheckGroupRoles(l_Roles2, LfgCategory::LFG_CATEGORIE_RAID, 0);
                }
            }
            printf("IsOk : %u\n", l_IsOk);

            return true;
        }

        static bool HandleShowLeaksCommand(ChatHandler* p_Handler, char const* p_Args)
        {
#ifdef __SANITIZE_ADDRESS__
            std::thread([]() -> void {
                __lsan_do_recoverable_leak_check();
            });
#endif
            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_debug_commandscript()
{
    new debug_commandscript();
}
#endif
