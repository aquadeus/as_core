////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "MapManager.h"
#ifndef CROSS
#include "TicketMgr.h"
#endif /* not CROSS */
#include "Chat.h"

class at_commandscript: public CommandScript
{
    public:
        at_commandscript() : CommandScript("at_commandscript") { }

        ChatCommand* GetCommands() const
        {
            static ChatCommand atSetCommandTable[] =
            {
                { "scale",          SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, &HandleAreatriggerSetScaleCommand,  "", nullptr          },
                { "value",          SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, &HandleAreaTriggerSetValueCommand,  "", nullptr          },
                { NULL,             0,                  0,                  false, NULL,                               "", nullptr          }
            };

            static ChatCommand atCommandTable[] =
            {
                { "set",            SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, NULL,                              "", atSetCommandTable },
                { "near",           SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, &HandleAreatriggerNearCommand,     "", nullptr           },
                { "reshape",        SEC_ADMINISTRATOR,  SEC_ADMINISTRATOR,  false, &HandleAreatriggerReShapeCommand,  "", nullptr           },
                { NULL,             0,                  0,                  false, NULL,                              "", nullptr           }
            };

            static ChatCommand commandTable[] =
            {
                { "at",             SEC_MODERATOR,      SEC_MODERATOR,      false, NULL,                               "", atCommandTable   },
                { NULL,             0,                  0,                  false, NULL,                               "", nullptr          }
            };

            return commandTable;
        }

        static bool HandleAreaTriggerSetValueCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            uint32 l_Guid = GetAreaTriggerGUIDLow(p_Handler, p_Args);
            if (!l_Guid)
                return false;

            char* l_X = strtok(nullptr, " ");
            char* l_Y = strtok(nullptr, " ");
            char* l_Z = strtok(nullptr, " ");

            if (!l_X || !l_Y)
                return false;

            std::list<AreaTrigger*> l_ATList;
            AreaTrigger* l_AT = nullptr;
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            l_Player->GetAreatriggerListInRange(l_ATList, 50.0f);

            for (AreaTrigger* l_AreaTrigger : l_ATList)
            {
                if (l_AreaTrigger->GetGUIDLow() != l_Guid)
                    continue;

                l_AT = l_AreaTrigger;
                break;
            }

            if (l_AT == nullptr)
            {
                p_Handler->PSendSysMessage("No areatrigger found with guid %u.", l_Guid);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            uint64 guid = l_AT->GetGUID();

            uint32 opcode = (uint32)atoi(l_X);
            if (opcode >= l_AT->GetValuesCount())
            {
                p_Handler->PSendSysMessage(LANG_TOO_BIG_INDEX, opcode, GUID_LOPART(guid), l_AT->GetValuesCount());
                return false;
            }

            bool isInt32 = true;
            if (l_Z)
                isInt32 = (bool)atoi(l_Z);

            if (isInt32)
            {
                uint32 value = (uint32)atoi(l_Y);
                l_AT->SetUInt32Value(opcode, value);
                p_Handler->PSendSysMessage(LANG_SET_UINT_FIELD, GUID_LOPART(guid), opcode, value);
            }
            else
            {
                float value = (float)atof(l_Y);
                l_AT->SetFloatValue(opcode, value);
                p_Handler->PSendSysMessage(LANG_SET_FLOAT_FIELD, GUID_LOPART(guid), opcode, value);
            }

            return true;
        }

        static uint32 GetAreaTriggerGUIDLow(ChatHandler* p_Handler, char const* p_Args)
        {
            if (*p_Args)
            {
                // number or [name] Shift-click form |color|Hareatrigger:at_guid|h[name]|h|r
                char* l_ID = p_Handler->extractKeyFromLink((char*)p_Args, "Hareatrigger");
                if (!l_ID)
                    return 0;

                return atoi(l_ID);
            }

            return 0;
        }

        static bool HandleAreatriggerSetScaleCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            uint32 l_Guid = GetAreaTriggerGUIDLow(p_Handler, p_Args);
            if (!l_Guid)
                return false;

            std::list<AreaTrigger*> l_ATList;
            AreaTrigger* l_AT = nullptr;
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            l_Player->GetAreatriggerListInRange(l_ATList, 50.0f);

            for (AreaTrigger* l_AreaTrigger : l_ATList)
            {
                if (l_AreaTrigger->GetGUIDLow() != l_Guid)
                    continue;

                l_AT = l_AreaTrigger;
                break;
            }

            if (l_AT == nullptr)
            {
                p_Handler->PSendSysMessage("No areatrigger found with guid %u.", l_Guid);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            char* l_Value = strtok(NULL, " ");
            if (!l_Value)
                return false;

            float l_Scale = atof(l_Value);
            l_AT->SetFloatValue(OBJECT_FIELD_SCALE, l_Scale);
            p_Handler->PSendSysMessage("Set areatrigger scale %f", l_Scale);
            return true;
        }

        static bool HandleAreatriggerNearCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            float l_Distance = (!*p_Args) ? 10.0f : (float)(atof(p_Args));
            if (l_Distance > 50.0f)
                l_Distance = 50.0f;

            std::list<AreaTrigger*> l_ATList;
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            l_Player->GetAreatriggerListInRange(l_ATList, l_Distance);

            for (AreaTrigger* l_AreaTrigger : l_ATList)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_AreaTrigger->GetSpellId());
                if (!l_SpellInfo)
                {
                    p_Handler->PSendSysMessage("Areatrigger (Entry: %u, Spell: %u) has not existing spellInfo", l_AreaTrigger->GetEntry(), l_AreaTrigger->GetSpellId());
                    continue;
                }

                p_Handler->PSendSysMessage("%u (Entry: %u, Spell: %u) - |cffffffff|Hareatrigger:%u|h[%s X:%f Y:%f Z:%f MapID:%u]|h|r",
                    l_AreaTrigger->GetGUIDLow(),
                    l_AreaTrigger->GetEntry(),
                    l_AreaTrigger->GetSpellId(),
                    l_AreaTrigger->GetGUIDLow(),
                    sSpellMgr->GetSpellInfo(l_AreaTrigger->GetSpellId())->SpellName->Get(DEFAULT_LOCALE),
                    l_AreaTrigger->m_positionX,
                    l_AreaTrigger->m_positionY,
                    l_AreaTrigger->m_positionZ,
                    l_AreaTrigger->GetMapId());
            }

            p_Handler->PSendSysMessage("Nearest AreaTriggers in %f yards : %lu.", l_Distance, l_ATList.size());
            return true;
        }

        static bool HandleAreatriggerReShapeCommand(ChatHandler* p_Handler, char const* p_Args)
        {
            if (!*p_Args)
                return false;

            uint32 l_Guid = GetAreaTriggerGUIDLow(p_Handler, p_Args);
            if (!l_Guid)
                return false;

            char* l_CharNewShape = strtok(nullptr, " ");
            char* l_CharBit     = strtok(nullptr, " ");

            int32   l_NewShape = 0;
            bool    l_Bit = false;

            if (l_CharNewShape)
                l_NewShape  = atoi(l_CharNewShape);
            if (l_CharBit)
                l_Bit       = atoi(l_CharBit);

            std::list<AreaTrigger*> l_ATList;
            AreaTrigger* l_AT = nullptr;
            Player* l_Player = p_Handler->GetSession()->GetPlayer();
            l_Player->GetAreatriggerListInRange(l_ATList, l_Player->GetMap()->GetVisibilityRange());

            for (AreaTrigger* l_AreaTrigger : l_ATList)
            {
                if (l_AreaTrigger->GetGUIDLow() != l_Guid)
                    continue;

                l_AT = l_AreaTrigger;
                break;
            }

            if (l_AT == nullptr)
            {
                p_Handler->PSendSysMessage("No areatrigger found with guid %u.", l_Guid);
                p_Handler->SetSentErrorMessage(true);
                return false;
            }

            l_AT->ReShape(l_NewShape, l_Bit);

            return true;
        }
};

#ifndef __clang_analyzer__
void AddSC_at_commandscript()
{
    new at_commandscript();
}
#endif
