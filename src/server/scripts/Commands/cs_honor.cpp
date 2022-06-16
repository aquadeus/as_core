////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: honor_commandscript
%Complete: 100
Comment: All honor related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"

class honor_commandscript: public CommandScript
{
public:
    honor_commandscript() : CommandScript("honor_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand honorCommandTable[] =
        {
            { "add",            SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, &HandleHonorAddCommand,             "", nullptr },
            { NULL,             0,                  0,                  false, NULL,                               "", NULL }
        };

        static ChatCommand commandTable[] =
        {
            { "honor",          SEC_GAMEMASTER,     SEC_GAMEMASTER,     false, NULL,                  "", honorCommandTable },
            { NULL,             0,                  0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleHonorAddCommand(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!*p_Args)
            return false;

        Player* l_Target = p_Handler->getSelectedPlayer();
        if (!l_Target)
        {
            p_Handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            p_Handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 l_Amount = (uint32)atoi(p_Args);
        l_Target->GiveHonor(l_Amount);
        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_honor_commandscript()
{
    new honor_commandscript();
}
#endif
