////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: account_commandscript
%Complete: 100
Comment: All account related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "Chat.h"
#ifndef CROSS
#include "BattlepayPacketFactory.h"

using namespace Battlepay::PacketFactory;
#endif /* not CROSS */

class account_commandscript: public CommandScript
{
public:
    account_commandscript() : CommandScript("account_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand accountSetCommandTable[] =
        {
            { "addon",          SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,  &HandleAccountSetAddonCommand,     "", NULL },
            { NULL,             SEC_PLAYER,             SEC_PLAYER,         false, NULL,                              "", NULL }
        };
        static ChatCommand accountCommandTable[] =
        {
            { "addon",          SEC_MODERATOR,          SEC_MODERATOR,      false, &HandleAccountAddonCommand,         "", NULL },
            { "onlinelist",     SEC_CONSOLE,            SEC_CONSOLE,        true,  &HandleAccountOnlineListCommand,    "", NULL },
            { "set",            SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  true,  NULL,                                "", accountSetCommandTable  },
#ifndef CROSS
            { "updatebalance",  SEC_CONSOLE,            SEC_CONSOLE,        true,  &HandleAccountUpdateBalanceCommand, "", NULL },
#endif /* not CROSS */
            { "",               SEC_PLAYER,             SEC_PLAYER,         false, &HandleAccountCommand,              "", NULL },
            { NULL,             SEC_PLAYER,             SEC_PLAYER,         false, NULL,                               "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "account",        SEC_PLAYER,             SEC_PLAYER,         true,  NULL,                                "", accountCommandTable  },
            { NULL,             SEC_PLAYER,             SEC_PLAYER,         false, NULL,                                "", NULL }
        };
        return commandTable;
    }

    static bool HandleAccountAddonCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            handler->SendSysMessage(LANG_CMD_SYNTAX);
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* exp = strtok((char*)args, " ");

        uint32 accountId = handler->GetSession()->GetAccountId();

        int expansion = atoi(exp); //get int anyway (0 if error)
        if (expansion < 0 || uint8(expansion) > sWorld->getIntConfig(CONFIG_EXPANSION))
        {
            handler->SendSysMessage(LANG_IMPROPER_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_EXPANSION);

        stmt->setUInt8(0, uint8(expansion));
        stmt->setUInt32(1, accountId);

        LoginDatabase.Execute(stmt);

        handler->PSendSysMessage(LANG_ACCOUNT_ADDON, expansion);
        return true;
    }

    /// Display info on users currently in the realm
    static bool HandleAccountOnlineListCommand(ChatHandler* handler, char const* /*args*/)
    {
        ///- Get the list of accounts ID logged to the realm
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_ONLINE);

        PreparedQueryResult result = CharacterDatabase.Query(stmt);

        if (!result)
        {
            handler->SendSysMessage(LANG_ACCOUNT_LIST_EMPTY);
            return true;
        }

        ///- Display the list of account/characters online
        handler->SendSysMessage(LANG_ACCOUNT_LIST_BAR_HEADER);
        handler->SendSysMessage(LANG_ACCOUNT_LIST_HEADER);
        handler->SendSysMessage(LANG_ACCOUNT_LIST_BAR);

        ///- Cycle through accounts
        do
        {
            Field* fieldsDB = result->Fetch();
            std::string name = fieldsDB[0].GetString();
            uint32 account = fieldsDB[1].GetUInt32();

            ///- Get the username, last IP and GM level of each account
            // No SQL injection. account is uint32.
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_INFO);
            stmt->setUInt32(0, account);
            PreparedQueryResult resultLogin = LoginDatabase.Query(stmt);

            if (resultLogin)
            {
                Field* fieldsLogin = resultLogin->Fetch();
                handler->PSendSysMessage(LANG_ACCOUNT_LIST_LINE,
                    fieldsLogin[0].GetCString(), name.c_str(), fieldsLogin[1].GetCString(),
                    fieldsDB[2].GetUInt16(), fieldsDB[3].GetUInt16(), fieldsLogin[3].GetUInt8(),
                    fieldsLogin[2].GetUInt8());
            }
            else
                handler->PSendSysMessage(LANG_ACCOUNT_LIST_ERROR, name.c_str());
        }
        while (result->NextRow());

        handler->SendSysMessage(LANG_ACCOUNT_LIST_BAR);
        return true;
    }

    static bool HandleAccountCommand(ChatHandler* handler, char const* /*args*/)
    {
        AccountTypes gmLevel = handler->GetSession()->GetSecurity();
        handler->PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmLevel));
        return true;
    }

    /// Set/Unset the expansion level for an account
    static bool HandleAccountSetAddonCommand(ChatHandler* handler, char const* args)
    {
        ///- Get the command line arguments
        char* account = strtok((char*)args, " ");
        char* exp = strtok(NULL, " ");

        if (!account)
            return false;

        std::string accountName;
        uint32 accountId;

        if (!exp)
        {
            Player* player = handler->getSelectedPlayer();
            if (!player)
                return false;

            accountId = player->GetSession()->GetAccountId();
            AccountMgr::GetName(accountId, accountName);
            exp = account;
        }
        else
        {
            ///- Convert Account name to Upper Format
            accountName = account;
            if (!AccountMgr::normalizeString(accountName))
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            accountId = AccountMgr::GetId(accountName);
            if (!accountId)
            {
                handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        // Let set addon state only for lesser (strong) security level
        // or to self account
        if (handler->GetSession() && handler->GetSession()->GetAccountId() != accountId &&
            handler->HasLowerSecurityAccount(NULL, accountId, true))
            return false;

        int expansion = atoi(exp); //get int anyway (0 if error)
        if (expansion < 0 || uint8(expansion) > sWorld->getIntConfig(CONFIG_EXPANSION))
            return false;

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_EXPANSION);

        stmt->setUInt8(0, expansion);
        stmt->setUInt32(1, accountId);

        LoginDatabase.Execute(stmt);

        handler->PSendSysMessage(LANG_ACCOUNT_SETADDON, accountName.c_str(), accountId, expansion);
        return true;
    }

#ifndef CROSS
    static bool HandleAccountUpdateBalanceCommand(ChatHandler* /*p_Handler*/, const char* p_Args)
    {
        uint32 l_AccountID  = 0;
        uint32 l_NewBalance = 0;

        try
        {
            l_AccountID  = std::atoi(strtok((char*)p_Args, " "));
            l_NewBalance = std::atoi(strtok(NULL, " "));
        }
        catch (...)
        {
            return false;
        }

        sBattlepayMgr->OnPaymentSucess(l_AccountID, l_NewBalance);
        return false;
    }
#endif /* not CROSS */
};

#ifndef __clang_analyzer__
void AddSC_account_commandscript()
{
    new account_commandscript();
}
#endif
