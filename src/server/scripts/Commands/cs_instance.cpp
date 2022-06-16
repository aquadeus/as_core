////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: instance_commandscript
%Complete: 100
Comment: All instance related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "Chat.h"
#include "Group.h"
#include "InstanceSaveMgr.h"
#include "InstanceScript.h"
#include "MapManager.h"

class instance_commandscript: public CommandScript
{
public:
    instance_commandscript() : CommandScript("instance_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand instanceCommandTable[] =
        {
            { "listbinds",      SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleInstanceListBindsCommand,    "", NULL },
            { "unbind",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleInstanceUnbindCommand,       "", NULL },
            { "stats",          SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   &HandleInstanceStatsCommand,        "", NULL },
            { "savedata",       SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleInstanceSaveDataCommand,     "", NULL },
            { "unloadonempty",  SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleInstanceSetUnloadEmpty,      "", NULL },
            { "setbossstate",   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleInstanceSetBossState,        "", NULL },
            { "getbossstate",   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleInstanceGetBossState,        "", NULL },
            { "bossstateinfo",  SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleInstanceBossStateInfo,       "", NULL },
            { NULL,             0,                      0,                  false,  NULL,                               "", NULL }
        };

        static ChatCommand commandTable[] =
        {
            { "instance",       SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                               "", instanceCommandTable },
            { NULL,             0,                      0,                  false,  NULL,                               "", NULL }
        };

        return commandTable;
    }

    static std::string GetTimeString(uint64 time)
    {
        uint64 days = time / DAY, hours = (time % DAY) / HOUR, minute = (time % HOUR) / MINUTE;
        std::ostringstream ss;
        if (days)
            ss << days << "d ";
        if (hours)
            ss << hours << "h ";
        ss << minute << 'm';
        return ss.str();
    }

    static bool HandleInstanceListBindsCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        uint32 counter = 0;
        for (uint8 i = 0; i < Difficulty::MaxDifficulties; ++i)
        {
            Player::BoundInstancesMap &binds = player->GetBoundInstances(Difficulty(i));
            binds.safe_foreach(true, [&](Player::BoundInstancesMap::const_iterator* itr) -> void
            {
                InstanceSavePtr save = (*itr)->second->save;
                std::string timeleft = GetTimeString(save->GetResetTime() - time(NULL));
                handler->PSendSysMessage("map: %d inst: %d perm: %s diff: %d canReset: %s TTR: %s", (*itr)->first, save->GetInstanceId(), (*itr)->second->perm ? "yes" : "no", save->GetDifficultyID(), save->CanReset() ? "yes" : "no", timeleft.c_str());
                counter++;
            });
        }
        handler->PSendSysMessage("player binds: %d", counter);

        counter = 0;
        if (GroupPtr group = player->GetGroup())
        {
            for (uint8 i = 0; i < Difficulty::MaxDifficulties; ++i)
            {
                Group::BoundInstancesMap &binds = group->GetBoundInstances(Difficulty(i));
                binds.safe_foreach(true, [&](Group::BoundInstancesMap::const_iterator* itr) -> void
                {
                    InstanceSavePtr save = (*itr)->second->save;
                    std::string timeleft = GetTimeString(save->GetResetTime() - time(NULL));
                    handler->PSendSysMessage("map: %d inst: %d perm: %s diff: %d canReset: %s TTR: %s", (*itr)->first, save->GetInstanceId(), (*itr)->second->perm ? "yes" : "no", save->GetDifficultyID(), save->CanReset() ? "yes" : "no", timeleft.c_str());
                    counter++;
                });
            }
        }
        handler->PSendSysMessage("group binds: %d", counter);

        return true;
    }

    static bool HandleInstanceUnbindCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        char* map = strtok((char*)args, " ");
        char* pDiff = strtok(NULL, " ");
        int8 diff = -1;
        if (pDiff)
            diff = atoi(pDiff);
        uint16 counter = 0;
        uint16 MapId = 0;

        if (strcmp(map, "all"))
        {
            MapId = uint16(atoi(map));
            if (!MapId)
                return false;
        }

        for (uint8 i = 0; i < Difficulty::MaxDifficulties; ++i)
        {
            Player::BoundInstancesMap &binds = player->GetBoundInstances(Difficulty(i));
            binds.safe_foreach(false, [&](Player::BoundInstancesMap::iterator* itr) -> void
            {
                InstanceSavePtr save = (*itr)->second->save;
                if ((*itr)->first != player->GetMapId() && (!MapId || MapId == (*itr)->first) && (diff == -1 || diff == save->GetDifficultyID()))
                {
                    std::string timeleft = GetTimeString(save->GetResetTime() - time(NULL));
                    handler->PSendSysMessage("unbinding map: %d inst: %d perm: %s diff: %d canReset: %s TTR: %s", (*itr)->first, save->GetInstanceId(), (*itr)->second->perm ? "yes" : "no", save->GetDifficultyID(), save->CanReset() ? "yes" : "no", timeleft.c_str());
                    player->UnbindInstance((*itr), Difficulty(i));
                    counter++;
                }
                else
                    ++(*itr);
            });
        }

        uint32 lootedCount = 0;
        if (sWorld->getBoolConfig(CONFIG_IS_TEST_SERVER))
        {
            lootedCount = uint32(player->GetBossLooted().size());
            player->ResetBossLooted();
        }

        handler->PSendSysMessage("instances unbound: %d. boss looted clear: %u records", counter, lootedCount);

        return true;
    }

    static bool HandleInstanceStatsCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->PSendSysMessage("instances loaded: %d", sMapMgr->GetNumInstances());
        handler->PSendSysMessage("players in instances: %d", sMapMgr->GetNumPlayersInInstances());
        handler->PSendSysMessage("instance saves: %d", sInstanceSaveMgr->GetNumInstanceSaves());
        handler->PSendSysMessage("players bound: %d", sInstanceSaveMgr->GetNumBoundPlayersTotal());
        handler->PSendSysMessage("groups bound: %d", sInstanceSaveMgr->GetNumBoundGroupsTotal());

        return true;
    }

    static bool HandleInstanceSaveDataCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Map* map = player->GetMap();
        if (!map->IsDungeon())
        {
            handler->PSendSysMessage("Map is not a dungeon.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!((InstanceMap*)map)->GetInstanceScript())
        {
            handler->PSendSysMessage("Map has no instance data.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        ((InstanceMap*)map)->GetInstanceScript()->SaveToDB();

        return true;
    }

    static bool HandleInstanceSetUnloadEmpty(ChatHandler* p_Handler, char const* /*p_Args*/)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        Map* const l_Map = l_Player->GetMap();
        if (!l_Player || !l_Map)
            return false;

        /// handle unload
        if (InstanceMap* l_Instance = l_Map->ToInstanceMap())
        {
            l_Instance->SetUnloadAfterEmpty();
            p_Handler->PSendSysMessage("Instance map [%u] - [ID: %u] - [MODE: %u] set to unload immediately after emptied from players.", l_Instance->GetId(), l_Instance->GetInstanceId(), l_Instance->GetDifficultyID());
            return true;
        }

        return false;
    }

    static std::string GetStateName(uint32 State)
    {
        std::string stateName = std::to_string(State);
        switch (State)
        {
            case 0:
                 stateName += "(NOT_STARTED)";
                 break;
            case 1:
                 stateName += "(IN_PROGRESS)";
                 break;
            case 2:
                 stateName += "(FAIL)";
                 break;
            case 3:
                 stateName += "(DONE)";
                 break;
            case 4:
                 stateName += "(SPECIAL)";
                 break;
            case 5:
                 stateName += "(TO_BE_DECIDED)";
                 break;
            default: break;
        }

        return stateName;
    }

    static std::string GetEncounterNameForAntorus(uint32 EncounterId)
    {
        std::string EncounterName = std::to_string(EncounterId);
        switch (EncounterId)
        {
            case 0:
                 EncounterName += "(DataGarothiWorldbreaker)";
                 break;
            case 1:
                 EncounterName += "(DataFelhoundsOfSargeras)";
                 break;
            case 2:
                 EncounterName += "(DataAntoranHighCommand)";
                 break;
            case 3:
                 EncounterName += "(DataPortalKeeperHasabel)";
                 break;
            case 4:
                 EncounterName += "(DataEonarTheLifeBinder)";
                 break;
            case 5:
                 EncounterName += "(DataImonarTheSoulhunter)";
                 break;
            case 6:
                 EncounterName += "(DataKinGaroth)";
                 break;
            case 7:
                 EncounterName += "(DataVarimathras)";
                 break;
            case 8:
                 EncounterName += "(DataTheCovenOfShivarra)";
                 break;
            case 9:
                 EncounterName += "(DataAggramar)";
                 break;
            case 10:
                 EncounterName += "(DataArgusTheUnmaker)";
                 break;
            default: break;
        }

        return EncounterName;
    }

    static bool HandleInstanceSetBossState(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* param1 = strtok((char*)args, " ");
        char* param2 = strtok(NULL, " ");

        if (!param1 || !param2)
        {
            handler->PSendSysMessage(LANG_CMD_SYNTAX);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* l_Player = handler->getSelectedPlayer();
        if (!l_Player)
            l_Player = handler->GetSession()->GetPlayer();

        if (!l_Player)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Map* l_Map = l_Player->GetMap();
        if (!l_Map || !l_Map->IsDungeon())
        {
            handler->PSendSysMessage("Map is not a dungeon.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        InstanceScript* l_Instance = l_Map->ToInstanceMap() ? l_Map->ToInstanceMap()->GetInstanceScript() : nullptr;
        if (!l_Instance)
        {
            handler->PSendSysMessage("Map has no instance data.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 l_EncounterId = atoi(param1);
        int32 l_State = atoi(param2);

        if (l_State > TO_BE_DECIDED || l_EncounterId > l_Instance->GetEncounterCount())
        {
            handler->PSendSysMessage("Value EncounterState or EncounterId incorrect");
            handler->SetSentErrorMessage(true);
            return false;
        }

        l_Instance->SetBossState(l_EncounterId, (EncounterState)l_State);
        if (l_Map->GetId() == 1712) ///<Antorus
            handler->PSendSysMessage("You changed SetBossState: %s for EncounterName: %s", GetStateName(l_State).c_str(), GetEncounterNameForAntorus(l_EncounterId).c_str());
        else
            handler->PSendSysMessage("You changed SetBossState: %s for EncounterId: %u", GetStateName(l_State).c_str(), l_EncounterId);

        return true;
    }

    static bool HandleInstanceGetBossState(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* param1 = strtok((char*)args, " ");

        if (!param1)
        {
            handler->PSendSysMessage(LANG_CMD_SYNTAX);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* l_Player = handler->getSelectedPlayer();
        if (!l_Player)
            l_Player = handler->GetSession()->GetPlayer();

        if (!l_Player)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Map* l_Map = l_Player->GetMap();
        if (!l_Map || !l_Map->IsDungeon())
        {
            handler->PSendSysMessage("Map is not a dungeon.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        InstanceScript* l_Instance = l_Map->ToInstanceMap() ? l_Map->ToInstanceMap()->GetInstanceScript() : nullptr;
        if (!l_Instance)
        {
            handler->PSendSysMessage("Map has no instance data.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 l_EncounterId = atoi(param1);

        if (l_EncounterId > l_Instance->GetEncounterCount())
        {
            handler->PSendSysMessage("Value EncounterId incorrect");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 l_State = l_Instance->GetBossState(l_EncounterId);

        if (l_Map->GetId() == 1712) ///<Antorus
            handler->PSendSysMessage("Target a have GetBossState: %s for EncounterName: %s", GetStateName(l_State).c_str(), GetEncounterNameForAntorus(l_EncounterId).c_str());
        else
            handler->PSendSysMessage("Target a have GetBossState: %s for EncounterId: %u", GetStateName(l_State).c_str(), l_EncounterId);

        return true;
    }

    static bool HandleInstanceBossStateInfo(ChatHandler* handler, char const* /*args*/)
    {
        Player* l_Player = handler->getSelectedPlayer();
        if (!l_Player)
            l_Player = handler->GetSession()->GetPlayer();

        if (!l_Player)
        {
            handler->PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Map* l_Map = l_Player->GetMap();
        if (!l_Map || !l_Map->IsDungeon())
        {
            handler->PSendSysMessage("Map is not a dungeon.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        InstanceScript* l_Instance = l_Map->ToInstanceMap() ? l_Map->ToInstanceMap()->GetInstanceScript() : nullptr;
        if (!l_Instance)
        {
            handler->PSendSysMessage("Map has no instance data.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage("All Boss State for Instance Map:");
        for (uint8 i = 0; i < l_Instance->GetEncounterCount(); ++i)
        {
            uint32 l_State = l_Instance->GetBossState(i);
            if (l_Map->GetId() == 1712) ///<Antorus
                handler->PSendSysMessage("EncounterName: %s, State: %s", GetEncounterNameForAntorus(i).c_str(), GetStateName(l_State).c_str());
            else
                handler->PSendSysMessage("EncounterId: %u, State: %s, ", i, GetStateName(l_State).c_str());
        }

        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_instance_commandscript()
{
    new instance_commandscript();
}
#endif
