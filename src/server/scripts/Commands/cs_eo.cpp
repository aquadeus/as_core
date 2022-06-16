////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"

class eo_commandscript : public CommandScript
{
    public:
        eo_commandscript() : CommandScript("eo_commandscript") { }

        ChatCommand* GetCommands() const
        {
            static ChatCommand eoCommandTable[] =
            {
                { "near",           SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, &HandleEONearCommand,               "", NULL },
                { "add",            SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, &HandleEOAddCommand,                "", NULL },
                { "delete",         SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, &HandleEODeleteCommand,             "", NULL },
                { NULL,             0,                  0,                  false, NULL,                               "", NULL }
            };
            static ChatCommand commandTable[] =
            {
                { "eventobject",    SEC_MODERATOR,      SEC_MODERATOR,      false, NULL,                    "",  eoCommandTable },
                { NULL,             0,                  0,                  false, NULL,                               "", NULL }
            };
            return commandTable;
        }

        /// Add spawn of creature
        static bool HandleEOAddCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            char* l_EoID = strtok((char*)p_Args, " ");
            if (!l_EoID)
                return false;

            char* l_RadiusStr = strtok(nullptr, " ");
            char* l_SpellStr = strtok(nullptr, " ");
            char* l_WorldSafeStr = strtok(nullptr, " ");

            uint32 l_ID  = atoi(l_EoID);
            float l_Radius  = l_RadiusStr ? atof(l_RadiusStr) : 1.0f;
            uint32 l_Spell  = l_SpellStr ? atoi(l_SpellStr) : 0;
            uint32 l_WorldSafe = l_WorldSafeStr ? atoi(l_WorldSafeStr) : 0;

            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            float l_X = l_Player->GetPositionX();
            float l_Y = l_Player->GetPositionY();
            float l_Z = l_Player->GetPositionZ();
            float l_O = l_Player->GetOrientation();
            Map* l_Map = l_Player->GetMap();

            if (l_Player->GetTransport())
            {
                p_Handler->SendSysMessage("Error, you on transport");
                return true;
            }

            EventObject* l_EventObject = new EventObject();
            if (!l_EventObject->Create(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_EVENTOBJECT), l_Map, l_Player->GetPhaseMgr().GetPhaseMaskForSpawn(), l_ID, l_X, l_Y, l_Z, l_O, l_Radius, l_Spell, l_WorldSafe))
            {
                delete l_EventObject;
                return false;
            }

            l_EventObject->SaveToDB(l_Map->GetId(), (1 << l_Map->GetSpawnMode()), l_Player->GetPhaseMgr().GetPhaseMaskForSpawn());

            uint32 l_DbGuid = l_EventObject->GetDBTableGUIDLow();

            delete l_EventObject;
            l_EventObject = new EventObject();
            if (!l_EventObject->LoadEventObjectFromDB(l_DbGuid, l_Map))
            {
                delete l_EventObject;
                return false;
            }

            sObjectMgr->AddEventObjectToGrid(l_DbGuid, sObjectMgr->GetEventObjectData(l_DbGuid));
            return true;
        }

        static bool HandleEODeleteCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            Creature* l_Unit = nullptr;

            if (*p_Args)
            {
                char* l_EoID = strtok((char*)p_Args, " ");
                if (!l_EoID)
                {
                    p_Handler->SendSysMessage("Error, need low guid");
                    return false;
                }

                uint32 l_LowGuid = strtoull(l_EoID, nullptr, 10);
                if (!l_LowGuid)
                {
                    p_Handler->SendSysMessage("Error, need low guid");
                    return false;
                }

                WorldDatabase.PExecute("DELETE FROM eventobject WHERE guid = %u", l_LowGuid);
            }
            else
                p_Handler->SendSysMessage("Error, need low guid");

            return true;
        }

        static bool HandleEONearCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            float l_Distance = (!*p_Args) ? 10.0f : (float)(atof(p_Args));
            uint32 l_Count = 0;

            Player* l_Player = p_Handler->GetSession()->GetPlayer();

            uint8 l_Index = 0;
            PreparedStatement* l_Statement = WorldDatabase.GetPreparedStatement(WORLD_SEL_EVENTOBJECT_NEAREST);
            l_Statement->setFloat(l_Index++, l_Player->GetPositionX());
            l_Statement->setFloat(l_Index++, l_Player->GetPositionY());
            l_Statement->setFloat(l_Index++, l_Player->GetPositionZ());
            l_Statement->setUInt32(l_Index++, l_Player->GetMapId());
            l_Statement->setFloat(l_Index++, l_Player->GetPositionX());
            l_Statement->setFloat(l_Index++, l_Player->GetPositionY());
            l_Statement->setFloat(l_Index++, l_Player->GetPositionZ());
            l_Statement->setFloat(l_Index++, l_Distance * l_Distance);

            PreparedQueryResult l_Result = WorldDatabase.Query(l_Statement);

            if (l_Result)
            {
                do
                {
                    l_Index = 0;

                    Field* l_Fields = l_Result->Fetch();
                    uint32 l_Guid = l_Fields[l_Index++].GetUInt32();
                    uint32 l_Entry = l_Fields[l_Index++].GetUInt32();
                    float l_X = l_Fields[l_Index++].GetFloat();
                    float l_Y = l_Fields[l_Index++].GetFloat();
                    float l_Z = l_Fields[l_Index++].GetFloat();
                    uint16 l_MapID = l_Fields[l_Index++].GetUInt16();

                    EventObjectTemplate const* l_EoInfo = sObjectMgr->GetEventObjectTemplate(l_Entry);
                    if (!l_EoInfo)
                        continue;

                    p_Handler->PSendSysMessage(LANG_NPC_LIST_CHAT, l_Guid, l_Entry, l_Guid, l_EoInfo->Name.c_str(), l_X, l_Y, l_Z, l_MapID);

                    ++l_Count;
                }
                while (l_Result->NextRow());
            }

            p_Handler->PSendSysMessage(LANG_COMMAND_NEAROBJMESSAGE, l_Distance, l_Count);
            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_eo_commandscript()
{
    new eo_commandscript();
}
#endif
