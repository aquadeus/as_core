////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "Chat.h"

class cosmetic_commandscript : public CommandScript
{
    public:
        cosmetic_commandscript() : CommandScript("cosmetic_commandscript") { }

        ChatCommand* GetCommands() const
        {
            static ChatCommand cosmeticCommandTable[] =
            {
                { NULL,             SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,         false, NULL,                                "", NULL }
            };

            static ChatCommand commandTable[] =
            {
                { "cosmetic",       SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,         true,  NULL,                                "", cosmeticCommandTable },
                { NULL,             SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,         false, NULL,                                "", NULL }
            };

            return commandTable;
        }
};

#ifndef __clang_analyzer__
void AddSC_cosmetic_commandscript()
{
    new cosmetic_commandscript();
}
#endif
