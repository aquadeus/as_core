////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: go_commandscript
%Complete: 100
Comment: All go related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "MapManager.h"
#ifndef CROSS
#include "TicketMgr.h"
#endif /* not CROSS */
#include "Chat.h"

class go_commandscript: public CommandScript
{
public:
    go_commandscript() : CommandScript("go_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand goCommandTable[] =
        {
            { "creature",       SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoCreatureCommand,          "", NULL },
            { "graveyard",      SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoGraveyardCommand,         "", NULL },
            { "grid",           SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoGridCommand,              "", NULL },
            { "object",         SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoObjectCommand,            "", NULL },
            { "taxinode",       SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoTaxinodeCommand,          "", NULL },
            { "trigger",        SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoTriggerCommand,           "", NULL },
            { "zonexy",         SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoZoneXYCommand,            "", NULL },
            { "xyz",            SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoXYZCommand,               "", NULL },
            { "ticket",         SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoTicketCommand,            "", NULL },
            { "z",              SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoZCommand,                 "", NULL },
            { "forward",        SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoForwardCommand,           "", NULL },
            { "location",       SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoLocationCommand,          "", NULL },
            { "left",           SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoLeftCommand,              "", NULL },
            { "right",          SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoRightCommand,             "", NULL },
            { "orientation",    SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoOrientationCommand,       "", NULL },
            { "",               SEC_MODERATOR,      SEC_MODERATOR,      false, &HandleGoXYZCommand,               "", NULL },
            { NULL,             0,                  0,                  false, NULL,                              "", NULL }
        };

        static ChatCommand commandTable[] =
        {
            { "go",             SEC_MODERATOR,      SEC_MODERATOR,      false, NULL,                     "", goCommandTable },
            { NULL,             0,                  0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    /** \brief Teleport the GM to the specified creature
    *
    * .gocreature <GUID>      --> TP using creature.guid
    * .gocreature azuregos    --> TP player to the mob with this name
    *                             Warning: If there is more than one mob with this name
    *                                      you will be teleported to the first one that is found.
    * .gocreature id 6109     --> TP player to the mob, that has this creature_template.entry
    *                             Warning: If there is more than one mob with this "id"
    *                                      you will be teleported to the first one that is found.
    */
    //teleport to creature
    static bool HandleGoCreatureCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        // "id" or number or [name] Shift-click form |color|Hcreature_entry:creature_id|h[name]|h|r
        char* param1 = handler->extractKeyFromLink((char*)args, "Hcreature");
        if (!param1)
            return false;

        std::ostringstream whereClause;

        // User wants to teleport to the NPC's template entry
        if (strcmp(param1, "id") == 0)
        {
            // Get the "creature_template.entry"
            // number or [name] Shift-click form |color|Hcreature_entry:creature_id|h[name]|h|r
            char* tail = strtok(NULL, "");
            if (!tail)
                return false;
            char* id = handler->extractKeyFromLink(tail, "Hcreature_entry");
            if (!id)
                return false;

            int32 entry = atoi(id);
            if (!entry)
                return false;

            whereClause << "WHERE id = '" << entry << '\'';
        }
        else
        {
            int32 guid = atoi(param1);

            // Number is invalid - maybe the user specified the mob's name
            if (!guid)
            {
                handler->SendSysMessage("You must specify a creature ID or guid, names are not allowed anymore.");
                handler->SetSentErrorMessage(true);
                return false;
            }
            else
                whereClause <<  "WHERE guid = '" << guid << '\'';
        }

        QueryResult result = WorldDatabase.PQuery("SELECT position_x, position_y, position_z, orientation, map, guid, id FROM creature %s", whereClause.str().c_str());
        if (!result)
        {
            handler->SendSysMessage(LANG_COMMAND_GOCREATNOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }
        if (result->GetRowCount() > 1)
            handler->SendSysMessage(LANG_COMMAND_GOCREATMULTIPLE);

        Field* fields = result->Fetch();
        float x = fields[0].GetFloat();
        float y = fields[1].GetFloat();
        float z = fields[2].GetFloat();
        float ort = fields[3].GetFloat();
        int mapId = fields[4].GetUInt16();
        uint32 guid = fields[5].GetUInt32();
        uint32 id = fields[6].GetUInt32();

        // if creature is in same map with caster go at its current location
        if (Creature* creature = sObjectAccessor->GetCreature(*player, MAKE_NEW_GUID(guid, id, HIGHGUID_UNIT)))
        {
            x = creature->GetPositionX();
            y = creature->GetPositionY();
            z = creature->GetPositionZ();
            ort = creature->GetOrientation();
        }

        if (!MapManager::IsValidMapCoord(mapId, x, y, z, ort))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(mapId, x, y, z, ort);
        return true;
    }

    static bool HandleGoGraveyardCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!*args)
            return false;

        char* gyId = strtok((char*)args, " ");
        if (!gyId)
            return false;

        int32 graveyardId = atoi(gyId);

        if (!graveyardId)
            return false;

        WorldSafeLocsEntry const* gy = sWorldSafeLocsStore.LookupEntry(graveyardId);
        if (!gy)
        {
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDNOEXIST, graveyardId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!MapManager::IsValidMapCoord(gy->MapID, gy->x, gy->y, gy->z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, gy->x, gy->y, gy->MapID);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(gy->MapID, gy->x, gy->y, gy->z, gy->o);
        return true;
    }

    //teleport to grid
    static bool HandleGoGridCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* gridX = strtok((char*)args, " ");
        char* gridY = strtok(NULL, " ");
        char* id = strtok(NULL, " ");

        if (!gridX || !gridY)
            return false;

        uint32 mapId = id ? (uint32)atoi(id) : player->GetMapId();

        // center of grid
        float x = ((float)atof(gridX) - CENTER_GRID_ID + 0.5f) * SIZE_OF_GRIDS;
        float y = ((float)atof(gridY) - CENTER_GRID_ID + 0.5f) * SIZE_OF_GRIDS;

        if (!MapManager::IsValidMapCoord(mapId, x, y))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        Map const* map = sMapMgr->CreateBaseMap(mapId, 0);
        float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));

        player->TeleportTo(mapId, x, y, z, player->GetOrientation());
        return true;
    }

    //teleport to gameobject
    static bool HandleGoObjectCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        // "id" or number or [name] Shift-click form |color|Hgameobject_entry:go_entry|h[name]|h|r
        char* param1 = handler->extractKeyFromLink((char*)args, "Hgameobject");
        if (!param1)
            return false;

        std::ostringstream whereClause;

        // User wants to teleport to the NPC's template entry
        if (strcmp(param1, "id") == 0)
        {
            // Get the "creature_template.entry"
            // number or [name] Shift-click form |color|Hgameobject_entry:go_entry|h[name]|h|r
            char* tail = strtok(NULL, "");
            if (!tail)
                return false;
            char* id = handler->extractKeyFromLink(tail, "Hgameobject_entry");
            if (!id)
                return false;

            int32 entry = atoi(id);
            if (!entry)
                return false;

            whereClause << "WHERE id = '" << entry << '\'';
        }
        else
        {
            int32 guid = atoi(param1);

            // Number is invalid - maybe the user specified the mob's name
            if (!guid)
            {
                std::string name = param1;
                WorldDatabase.EscapeString(name);
                whereClause << ", gameobject_template WHERE gameobject.id = gameobject_template.entry AND gameobject_template.name " _LIKE_ " '" << name << '\'';
            }
            else
                whereClause << "WHERE guid = '" << guid << '\'';
        }

        QueryResult result = WorldDatabase.PQuery("SELECT position_x, position_y, position_z, orientation, map, guid, id FROM gameobject %s", whereClause.str().c_str());
        if (!result)
        {
            handler->SendSysMessage("No GameObject found.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (result->GetRowCount() > 1)
            handler->SendSysMessage("More than one GameObject found, you're teleported on the first.");

        Field* fields = result->Fetch();
        float x = fields[0].GetFloat();
        float y = fields[1].GetFloat();
        float z = fields[2].GetFloat();
        float ort = fields[3].GetFloat();
        int mapId = fields[4].GetUInt16();
        uint32 guid = fields[5].GetUInt32();
        uint32 id = fields[6].GetUInt32();

        // if gameobject is in same map with caster go at its current location
        if (GameObject* l_Go = sObjectAccessor->GetGameObject(*player, MAKE_NEW_GUID(guid, id, HIGHGUID_GAMEOBJECT)))
        {
            x = l_Go->GetPositionX();
            y = l_Go->GetPositionY();
            z = l_Go->GetPositionZ();
            ort = l_Go->GetOrientation();
        }

        if (!MapManager::IsValidMapCoord(mapId, x, y, z, ort))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(mapId, x, y, z, ort);
        return true;
    }

    static bool HandleGoTaxinodeCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!*args)
            return false;

        char* id = handler->extractKeyFromLink((char*)args, "Htaxinode");
        if (!id)
            return false;

        int32 nodeId = atoi(id);
        if (!nodeId)
            return false;

        TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(nodeId);
        if (!node)
        {
            handler->PSendSysMessage(LANG_COMMAND_GOTAXINODENOTFOUND, nodeId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if ((node->x == 0.0f && node->y == 0.0f && node->z == 0.0f) ||
            !MapManager::IsValidMapCoord(node->MapID, node->x, node->y, node->z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, node->x, node->y, node->MapID);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(node->MapID, node->x, node->y, node->z, player->GetOrientation());
        return true;
    }

    static bool HandleGoTriggerCommand(ChatHandler* handler, char const* args)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!*args)
            return false;

        char* id = strtok((char*)args, " ");
        if (!id)
            return false;

        int32 areaTriggerId = atoi(id);

        if (!areaTriggerId)
            return false;

        AreaTriggerEntry const* at = sAreaTriggerStore.LookupEntry(areaTriggerId);
        if (!at)
        {
            handler->PSendSysMessage(LANG_COMMAND_GOAREATRNOTFOUND, areaTriggerId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!MapManager::IsValidMapCoord(at->ContinentID, at->Pos[0], at->Pos[1], at->Pos[2]))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, at->Pos[0], at->Pos[0], at->ContinentID);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(at->ContinentID, at->Pos[0], at->Pos[1], at->Pos[2], player->GetOrientation());
        return true;
    }

    //teleport at coordinates
    static bool HandleGoZoneXYCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* zoneX = strtok((char*)args, " ");
        char* zoneY = strtok(NULL, " ");
        char* tail = strtok(NULL, "");

        char* id = handler->extractKeyFromLink(tail, "Harea");       // string or [name] Shift-click form |color|Harea:area_id|h[name]|h|r

        if (!zoneX || !zoneY)
            return false;

        float x = (float)atof(zoneX);
        float y = (float)atof(zoneY);

        // prevent accept wrong numeric args
        if ((x == 0.0f && *zoneX != '0') || (y == 0.0f && *zoneY != '0'))
            return false;

        uint32 areaId = id ? (uint32)atoi(id) : player->GetZoneId();

        AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(areaId);

        if (x < 0 || x > 100 || y < 0 || y > 100 || !areaEntry)
        {
            handler->PSendSysMessage(LANG_INVALID_ZONE_COORD, x, y, areaId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // update to parent zone if exist (client map show only zones without parents)
        AreaTableEntry const* zoneEntry = areaEntry->ParentAreaID ? sAreaTableStore.LookupEntry(areaEntry->ParentAreaID) : areaEntry;

        Map const* map = sMapMgr->CreateBaseMap(zoneEntry->ContinentID, 0);

        if (map->Instanceable())
        {
            handler->PSendSysMessage(LANG_INVALID_ZONE_MAP, areaEntry->ID, areaEntry->AreaNameLang, map->GetId(), map->GetMapName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        Zone2MapCoordinates(x, y, zoneEntry->ID);

        if (!MapManager::IsValidMapCoord(zoneEntry->ContinentID, x, y))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, zoneEntry->ContinentID);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));

        player->TeleportTo(zoneEntry->ContinentID, x, y, z, player->GetOrientation());
        return true;
    }

    //teleport at coordinates, including Z and orientation
    static bool HandleGoXYZCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* goX = strtok((char*)args, " ");
        char* goY = strtok(NULL, " ");
        char* goZ = strtok(NULL, " ");
        char* id = strtok(NULL, " ");
        char* port = strtok(NULL, " ");

        if (!goX || !goY)
            return false;

        float x = (float)atof(goX);
        float y = (float)atof(goY);
        float z;
        float ort = port ? (float)atof(port) : player->GetOrientation();
        uint32 mapId = id ? (uint32)atoi(id) : player->GetMapId();

        if (goZ)
        {
            z = (float)atof(goZ);
            if (!MapManager::IsValidMapCoord(mapId, x, y, z))
            {
                handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            if (!MapManager::IsValidMapCoord(mapId, x, y))
            {
                handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
                handler->SetSentErrorMessage(true);
                return false;
            }
            Map const* map = sMapMgr->CreateBaseMap(mapId, 0);
            z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(mapId, x, y, z, ort);
        return true;
    }

    static bool HandleGoTicketCommand(ChatHandler* handler, char const* args)
    {
#ifndef CROSS
        if (!*args)
            return false;

        char* id = strtok((char*)args, " ");
        if (!id)
            return false;

        uint32 ticketId = atoi(id);
        if (!ticketId)
            return false;

        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        else
            player->SaveRecallPosition();

        ticket->TeleportTo(player);
#endif
        return true;
    }

    //teleport at coordinates, including Z and orientation
    static bool HandleGoZCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* goZ = NULL;

        bool relative = false;
        bool add = false;

        if (*args == '+' || *args == '-')
        {
            relative = true;
            add = (*args == '+');
            goZ = strtok((char*)(args + 1), " ");
        }
        else
            goZ = strtok((char*)args, " ");

        if (!goZ)
            return false;

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        uint32 mapId = player->GetMapId();

        float z = 0.0f;
        float paramZ = (float)atof(goZ);

        if (relative)
        {
            if (add)
                z = player->GetPositionZ() + paramZ;
            else
                z = player->GetPositionZ() - paramZ;
        }
        else
            z = paramZ;

        if (!MapManager::IsValidMapCoord(mapId, x, y, z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(mapId, x, y, z, player->GetOrientation());
        return true;
    }

    static bool HandleGoForwardCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* goDistance = NULL;

        goDistance = strtok((char*)args, " ");

        if (!goDistance)
            return false;

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float o = player->GetOrientation();
        uint32 mapId = player->GetMapId();

        float dist = (float)atof(goDistance);

        x = x + (dist * cos(o));
        y = y + (dist * sin(o));

        if (!MapManager::IsValidMapCoord(mapId, x, y, z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(mapId, x, y, z, o);
        return true;
    }

    static bool HandleGoLeftCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* goDistance = NULL;

        goDistance = strtok((char*)args, " ");

        if (!goDistance)
            return false;

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float o = player->GetOrientation();
        uint32 mapId = player->GetMapId();

        float dist = (float)atof(goDistance);

        x = x + (dist * cos(o + M_PI_2));
        y = y + (dist * sin(o + M_PI_2));

        if (!MapManager::IsValidMapCoord(mapId, x, y, z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(mapId, x, y, z, o);
        return true;
    }

    static bool HandleGoRightCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* goDistance = NULL;

        goDistance = strtok((char*)args, " ");

        if (!goDistance)
            return false;

        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();
        float o = player->GetOrientation();
        uint32 mapId = player->GetMapId();

        float dist = (float)atof(goDistance);

        x = x + (dist * cos(o - M_PI_2));
        y = y + (dist * sin(o - M_PI_2));

        if (!MapManager::IsValidMapCoord(mapId, x, y, z))
        {
            handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(mapId, x, y, z, o);
        return true;
    }

    static bool HandleGoOrientationCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        char* goOrientation = NULL;

        goOrientation = strtok((char*)args, " ");

        if (!goOrientation)
            return false;

        float l_Orientation = (float)atof(goOrientation);
        player->SetOrientation(l_Orientation);
        player->NearTeleportTo(*player);
        return true;
    }

    static bool HandleGoLocationCommand(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!*p_Args)
            return false;

        Player* l_Player = p_Handler->GetSession()->GetPlayer();

        char* l_LocStr = strtok((char*)p_Args, " ");
        if (l_LocStr == nullptr)
        {
            p_Handler->PSendSysMessage("Please verify your location ID.");
            return false;
        }

        uint32 l_Location = atoi(l_LocStr);
        if (!l_LocStr)
        {
            p_Handler->PSendSysMessage("Please verify your location ID.");
            return false;
        }

        if (sWorldSafeLocsStore.LookupEntry(l_Location) == nullptr)
        {
            p_Handler->PSendSysMessage("This world location doesn't exist.");
            return false;
        }

        l_Player->TeleportTo(l_Location);
        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_go_commandscript()
{
    new go_commandscript();
}
#endif