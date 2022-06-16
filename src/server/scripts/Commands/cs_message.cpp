////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: message_commandscript
%Complete: 100
Comment: All message related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "Chat.h"
#include "ChannelMgr.h"

class message_commandscript: public CommandScript
{
public:
    message_commandscript() : CommandScript("message_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand channelSetCommandTable[] =
        {
            { "ownership",      SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleChannelSetOwnership,         "", NULL },
            { "slowmode",       SEC_MODERATOR,          SEC_MODERATOR,      false,  &HandleChannelSetSlowmode,          "", NULL },
            { "lockdown",       SEC_MODERATOR,          SEC_MODERATOR,      false,  &HandleChannelSetLockdown,          "", NULL },
            { NULL,             0,                      0,                  false,  NULL,                               "", NULL }
        };
        static ChatCommand channelCommandTable[] =
        {
            { "set",            SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                               "", channelSetCommandTable },
            { NULL,             0,                      0,                  false,  NULL,                               "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "channel",        SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,   NULL,                               "", channelCommandTable  },
            { "nameannounce",   SEC_MODERATOR,          SEC_MODERATOR,      true,   &HandleNameAnnounceCommand,         "", NULL },
            { "gmnameannounce", SEC_MODERATOR,          SEC_MODERATOR,      true,   &HandleGMNameAnnounceCommand,       "", NULL },
            { "announce",       SEC_MODERATOR,          SEC_MODERATOR,      true,   &HandleAnnounceCommand,             "", NULL },
            { "gmannounce",     SEC_MODERATOR,          SEC_MODERATOR,      true,   &HandleGMAnnounceCommand,           "", NULL },
            { "notify",         SEC_MODERATOR,          SEC_MODERATOR,      true,   &HandleNotifyCommand,               "", NULL },
            { "gmnotify",       SEC_MODERATOR,          SEC_MODERATOR,      true,   &HandleGMNotifyCommand,             "", NULL },
            { "whispers",       SEC_MODERATOR,          SEC_MODERATOR,      false,  &HandleWhispersCommand,             "", NULL },
            { NULL,             0,                      0,                  false,  NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleChannelSetOwnership(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;
        char const* channelStr = strtok((char*)args, " ");
        char const* argStr = strtok(NULL, "");

        if (!channelStr || !argStr)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        Channel* channcel = NULL;

        if (ChannelMgr* cMgr = channelMgr(player->GetTeam()))
            channcel = cMgr->GetChannel(channelStr, player);

        if (strcmp(argStr, "on") == 0)
        {
            if (channcel)
                channcel->SetOwnership(true);
            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHANNEL_OWNERSHIP);
            stmt->setUInt8 (0, 1);
            stmt->setString(1, channelStr);
            CharacterDatabase.Execute(stmt);
            handler->PSendSysMessage(LANG_CHANNEL_ENABLE_OWNERSHIP, channelStr);
        }
        else if (strcmp(argStr, "off") == 0)
        {
            if (channcel)
                channcel->SetOwnership(false);
            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHANNEL_OWNERSHIP);
            stmt->setUInt8 (0, 0);
            stmt->setString(1, channelStr);
            CharacterDatabase.Execute(stmt);
            handler->PSendSysMessage(LANG_CHANNEL_DISABLE_OWNERSHIP, channelStr);
        }
        else
            return false;

        return true;
    }

    static bool HandleNameAnnounceCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string name("Console");
        if (WorldSession* session = handler->GetSession())
            name = session->GetPlayer()->GetName();

        sWorld->SendWorldText(LANG_ANNOUNCE_COLOR, name.c_str(), args);
        return true;
    }

    static bool HandleGMNameAnnounceCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string name("Console");
        if (WorldSession* session = handler->GetSession())
            name = session->GetPlayer()->GetName();

        sWorld->SendGMText(LANG_GM_ANNOUNCE_COLOR, name.c_str(), args);
        return true;
    }
    // global announce
    static bool HandleAnnounceCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char buff[2048];
        sprintf(buff, handler->GetTrinityString(LANG_SYSTEMMESSAGE), args);
        sWorld->SendServerMessage(SERVER_MSG_STRING, buff);
        return true;
    }
    // announce to logged in GMs
    static bool HandleGMAnnounceCommand(ChatHandler* /*handler*/, char const* args)
    {
        if (!*args)
            return false;

        sWorld->SendGMText(LANG_GM_BROADCAST, args);
        return true;
    }
    // notification player at the screen
    static bool HandleNotifyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string str = handler->GetTrinityString(LANG_GLOBAL_NOTIFY);
        str += args;

        WorldPacket l_Data(SMSG_PRINT_NOTIFICATION, 2 + str.length());
        l_Data.WriteBits(str.length(), 12);
        l_Data.FlushBits();
        l_Data.WriteString(str);
        sWorld->SendGlobalMessage(&l_Data);

        return true;
    }
    // notification GM at the screen
    static bool HandleGMNotifyCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        std::string str = handler->GetTrinityString(LANG_GM_NOTIFY);
        str += args;

        WorldPacket l_Data(SMSG_PRINT_NOTIFICATION, 2 + str.length());
        l_Data.WriteBits(str.length(), 12);
        l_Data.FlushBits();
        l_Data.WriteString(str);
        sWorld->SendGlobalGMMessage(&l_Data);

        return true;
    }
    // Enable\Dissable accept whispers (for GM)
    static bool HandleWhispersCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            handler->PSendSysMessage(LANG_COMMAND_WHISPERACCEPTING, handler->GetSession()->GetPlayer()->IsAcceptWhispers() ?  handler->GetTrinityString(LANG_ON) : handler->GetTrinityString(LANG_OFF));
            return true;
        }

        std::string argStr = (char*)args;
        // whisper on
        if (argStr == "on")
        {
            handler->GetSession()->GetPlayer()->SetAcceptWhispers(true);
            handler->SendSysMessage(LANG_COMMAND_WHISPERON);
            return true;
        }

        // whisper off
        if (argStr == "off")
        {
            // Remove all players from the Gamemaster's whisper whitelist
            handler->GetSession()->GetPlayer()->ClearWhisperWhiteList();
            handler->GetSession()->GetPlayer()->SetAcceptWhispers(false);
            handler->SendSysMessage(LANG_COMMAND_WHISPEROFF);
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleChannelSetSlowmode(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!*p_Args)
            return false;

        char const* l_ChannelStr = strtok(const_cast<char*>(p_Args), " ");
        char const* l_SlowModeIntervalStr = strtok(nullptr, "");
        if (!l_ChannelStr || !l_SlowModeIntervalStr)
            return false;

        uint32 l_SlowModeInterval = static_cast<uint32>(atoi(l_SlowModeIntervalStr));
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        Channel* l_Channel = nullptr;

        if (ChannelMgr* l_ChannelMgr = channelMgr(l_Player->GetTeam()))
            l_Channel = l_ChannelMgr->GetChannel(l_ChannelStr, l_Player);

        if (!l_Channel)
        {
            p_Handler->PSendSysMessage("Please enter a valid channel name.");
            return true;
        }

        if (l_SlowModeInterval != 0 && l_SlowModeInterval <= 2)
        {
            p_Handler->PSendSysMessage("Please enter a valid slowmode interval. (Above 2 seconds or 0 seconds for disabling slowmode).");
            return true;
        }
        l_Channel->SetSlowMode(l_SlowModeInterval);

        bool l_Enabled = l_SlowModeInterval != 0;
        p_Handler->PSendSysMessage("%s slowmode for channel '%s'.%s", l_Enabled ? "Enabled" : "Disabled", l_Channel->GetName().c_str(),
            l_Enabled ? (" Slow mode interval is now " + std::to_string(l_SlowModeInterval) + " seconds.").c_str() : "");
        return true;
    }

    static bool HandleChannelSetLockdown(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!*p_Args)
            return false;

        char const* l_ChannelStr = strtok(const_cast<char*>(p_Args), " ");
        char const* l_LockDownToggle = strtok(nullptr, "");
        if (!l_ChannelStr || !l_LockDownToggle)
            return false;

        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        Channel* l_Channel = nullptr;

        uint8 l_Toggle = std::strcmp(l_LockDownToggle, "on") == 0;
        if (std::strcmp(l_LockDownToggle, "off") == 0)
            l_Toggle = 2;

        if (!l_Toggle)
        {
            p_Handler->PSendSysMessage("Invalid input. Please enter \"on/off\" as command argument.");
            return true;
        }

        if (ChannelMgr* l_ChannelMgr = channelMgr(l_Player->GetTeam()))
            l_Channel = l_ChannelMgr->GetChannel(l_ChannelStr, l_Player);

        if (!l_Channel)
        {
            p_Handler->PSendSysMessage("Please enter a valid channel name. Channel not found.");
            return true;
        }

        bool l_Enabled = l_Toggle == 1;
        l_Channel->SetLockedDown(l_Enabled);
        p_Handler->PSendSysMessage("Channel '%s' is %s under lockdown.", l_Channel->GetName().c_str(), l_Enabled ? "is now" : "is no longer");
        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_message_commandscript()
{
    new message_commandscript();
}
#endif
