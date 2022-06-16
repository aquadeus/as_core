////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "Chat.h"
#include "RecruitAFriendMgr.hpp"
#include "Group.h"

class raf_commandscript : public CommandScript
{
public:
    raf_commandscript() : CommandScript("raf_commandscript")
    {
    }

    ChatCommand* GetCommands() const
    {
        static ChatCommand rafCommandTable[] =
        {
            { "send",                   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,          true,  &HandleRAFSendPacket,                "", NULL },
            { "recruitAccount",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,          true,  &HandleRAFRecruitAccount,            "", NULL },
            { "enableNotification",     SEC_PLAYER,             SEC_PLAYER,                 true,  &HandleRAFEnableNotification,        "", NULL },
            { "fscsummon",              SEC_PLAYER,             SEC_PLAYER,                 true,  &HandleRAFSummon,                    "", NULL },
            { "fscselectreward",        SEC_PLAYER,             SEC_PLAYER,                 true,  &HandleRAFSelectReward,              "", NULL },
            { "fscbegin",               SEC_PLAYER,             SEC_PLAYER,                 true,  &FSCHandleRAFRequestBegin,           "", NULL },
            { "fscappendnote",          SEC_PLAYER,             SEC_PLAYER,                 true,  &FSCHandleRAFRequestAppendNote,      "", NULL },
            { "fscend",                 SEC_PLAYER,             SEC_PLAYER,                 true,  &FSCHandleRAFRequestEnd,             "", NULL },
            { "fscselectchar",          SEC_PLAYER,             SEC_PLAYER,                 true,  &FSCHandleRAFSelectCharacter,        "", NULL },
            { NULL,                     SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,          false, NULL,                                "", NULL }
        };

        static ChatCommand fsCommandTable[] =
        {
            { "load",                  SEC_PLAYER,              SEC_PLAYER,                 true,  &HandleFSLoad,                      "", NULL },
        };

        static ChatCommand commandTable[] =
        {
            { "raf",            SEC_ADMINISTRATOR,              SEC_ADMINISTRATOR,         true,  NULL,                                "", rafCommandTable },
            { "fs",             SEC_ADMINISTRATOR,              SEC_ADMINISTRATOR,         true,  NULL,                                "", fsCommandTable },
            { NULL,             SEC_ADMINISTRATOR,              SEC_ADMINISTRATOR,         false, NULL,                                "", NULL }
        };

        return commandTable;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    static bool HandleRAFEnableNotification(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();

        if (!l_Player)
            return true;

        std::string p_StringArgs(p_Args);
        std::string p_True = "true";

        if (p_StringArgs == p_True)
            sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventSetNotificationsEnabled, l_Player->GetRealGUID(), true);
        else
            sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventSetNotificationsEnabled, l_Player->GetRealGUID(), false);

        return true;
    }

    static bool HandleRAFSummon(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        if (!l_Player)
            return true;

        Player* l_Target = sObjectAccessor->FindPlayerByName(p_Args);
        if (!l_Target)
            return true;

        if (!l_Player->IsInSameGroupWith(l_Target))
            return true;

        sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventSummonRAFFriend, l_Player->GetRealGUID(), l_Target->GetRealGUID());

        return true;
    }

    static bool FSCHandleRAFRequestBegin(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();

        if (!l_Player)
            return true;

        std::string l_Mail = p_Args;
        sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventStartRequest, l_Player->GetRealGUID(), l_Mail.c_str());
        return true;
    }

    static bool FSCHandleRAFRequestAppendNote(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();

        if (!l_Player)
            return true;

        static std::function<void(std::string&, const std::string&, const std::string&)> l_ReplaceAll = [](std::string& str, const std::string& from, const std::string& to) -> void
        {
            if (from.empty())
                return;
            size_t start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
            }
        };

        std::string l_Message = p_Args;
        l_ReplaceAll(l_Message, "$$n", "\n");

        sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventAppendNote, l_Player->GetRealGUID(), l_Message.c_str());
        return true;
    }

    static bool FSCHandleRAFRequestEnd(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        if (!l_Player)
            return true;

        sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventEndRequest, l_Player->GetRealGUID());
        return true;
    }

    static bool FSCHandleRAFSelectCharacter(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        if (!l_Player)
            return true;

        sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventSelectCharacter, l_Player->GetRealGUID());
        return true;
    }

    static bool HandleRAFSelectReward(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        if (!l_Player)
            return true;

        char* l_Str = strtok((char*)p_Args, " ");
        if (!l_Str)
            return true;

        uint32 l_RewardID = atoi(l_Str);
        if (!l_RewardID)
            return false;

        sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventSelectReward, l_Player->GetRealGUID(), l_RewardID);
        return true;
    }

    static bool HandleRAFSendPacket(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        if (!l_Player)
            return false;

        char* p_Opcode = strtok((char*)p_Args, " ");
        if (!p_Opcode)
            return false;

        std::vector<std::string> l_Args;
        while (char* p_Arg = strtok(nullptr, " "))
            l_Args.push_back(p_Arg);

        l_Player->SendCustomMessage(p_Opcode, l_Args);

        return true;
    }

    static bool HandleRAFRecruitAccount(ChatHandler* p_Handler, char const* p_Args)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        if (!l_Player)
            return false;

        char* l_AccString = strtok((char*)p_Args, " ");
        if (!l_AccString)
            return false;

        uint32 l_AccountId = std::atoi(l_AccString);
        if (!l_AccountId)
            return false;

        PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_INS_RAF_ACC);
        l_Statement->setUInt32(0, g_RealmID);
        l_Statement->setUInt32(1, l_Player->GetSession()->GetAccountId());
        l_Statement->setUInt32(2, l_AccountId);
        l_Statement->setUInt8(3, l_Player->getFaction());
        LoginDatabase.Execute(l_Statement);

        l_Player->SendSysMessage("You and the recruited account must log out to activate the RAF");

        return true;
    }

    static bool HandleFSLoad(ChatHandler* p_Handler, char const* /*p_Args*/)
    {
        Player* l_Player = p_Handler->GetSession()->GetPlayer();
        if (!l_Player)
            return false;

        std::vector<std::string> l_String;
        l_String.push_back("");

        std::vector<std::string> const& l_AddonFunctions = sObjectMgr->GetAddonFunctions();
        std::vector<std::string> const& l_LocalizedAddonFunctions = sObjectMgr->GetLocalizedAddonFunctions(l_Player->GetSession()->GetSessionDbLocaleIndex());
        for (std::string const& l_LocaleFunction : l_LocalizedAddonFunctions)
        {
            l_String[0] = l_LocaleFunction;
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
        }

        for (std::string const& l_Function : l_AddonFunctions)
        {
            l_String[0] = l_Function;
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
        }

        l_String[0] = "Start()";
        l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);

        std::vector<std::string> const& l_ChangelogGeneratedFunctions = sObjectMgr->GetLiveChangelogFunctions();
        for (std::string const& l_Function : l_ChangelogGeneratedFunctions)
        {
            l_String[0] = l_Function;
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
        }

        std::vector<std::string> const& l_NewsFeedGeneratedFunctions = sObjectMgr->GetNewsFeedFunctions(l_Player->GetSession()->GetSessionDbLocaleIndex());
        for (std::string const& l_Function : l_NewsFeedGeneratedFunctions)
        {
            l_String[0] = l_Function;
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
        }

        std::vector<std::string> const& l_AlertGeneratedFunction = sObjectMgr->GetAlertButtonFunctions(l_Player->GetSession()->GetSessionDbLocaleIndex());
        for (std::string const& l_Function : l_AlertGeneratedFunction)
        {
            l_String[0] = l_Function;
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);
        }

        if (l_Player->CanReceiveLiveChangelog())
        {
            l_String[0] = "_G[\"SplashFrame\"]:Show()";
            l_Player->SendCustomMessage("FSC_LUA_EXECUTE", l_String);

            l_Player->SetChangelogReceived();
        }

        sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventPlayerLogin, l_Player->GetRealGUID());

        return true;
    }
};

class group_refer_a_friend : public GroupScript
{
    public:
        group_refer_a_friend() : GroupScript("group_refer_a_friend") { }

        uint64 GetRealGUID(uint64 p_GUID)
        {
#ifdef CROSS
            Player* l_Player = sObjectAccessor->FindPlayer(p_GUID);
            if (!l_Player)
                return 0;

            return l_Player->GetRealGUID();
#else
            return p_GUID;
#endif
        }

        void OnAddMember(GroupPtr p_Group, uint64 p_GUID) override
        {
            p_GUID = GetRealGUID(p_GUID);

            p_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_MemberSlot) -> void
            {
                sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventSetInGroup, GetRealGUID(l_MemberSlot->guid), p_GUID, true);
            });
        }

        void OnRemoveMember(GroupPtr p_Group, uint64 p_GUID, RemoveMethod p_Method, uint64 p_KickerGUID, const char* p_Reason) override
        {
            p_GUID = GetRealGUID(p_GUID);

            p_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_MemberSlot) -> void
            {
                sRecruitAFriendMgr->AddEvent(eRecruitAFriendMgrEvents::EventSetInGroup, GetRealGUID(l_MemberSlot->guid), p_GUID, false);
            });
        }
};

#ifndef __clang_analyzer__
void AddSC_raf_commandscript()
{
    new raf_commandscript();
    new group_refer_a_friend();
}
#endif
