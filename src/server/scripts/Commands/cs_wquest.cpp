////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Chat.h"
#include "ScriptMgr.h"
#include "WorldQuestMgr.h"
#include "POIMgr.hpp"
#include <fstream>

class wquest_commandscript : public CommandScript
{
public:
    wquest_commandscript() : CommandScript("quest_commandscript") { }

    ChatCommand* GetCommands() const override
    {
        static ChatCommand wquestCommandTable[] =
        {
            { "add",                SEC_CONSOLE,        SEC_CONSOLE,  true, &HandleWQuestAdd,                           "", nullptr },
            { "addall",             SEC_CONSOLE,        SEC_CONSOLE,  true, &HandleWQuestAddAll,                        "", nullptr },
            { "remove",             SEC_CONSOLE,        SEC_CONSOLE,  true, &HandleWQuestRemove,                        "", nullptr },
            { "reward",             SEC_CONSOLE,        SEC_CONSOLE,  false, &HandleWQuestReward,                       "", nullptr },
            { "dumpcategory",       SEC_CONSOLE,        SEC_CONSOLE,  true, &HandleWQuestDumpCategory,                  "", nullptr },
            { "dumploot",           SEC_CONSOLE,        SEC_CONSOLE,  true, &HandleWQuestDumpLoot,                      "", nullptr },
            { "resetallrewards",    SEC_CONSOLE,        SEC_CONSOLE,  false, &HandleWQuestResetAllRewards,              "", nullptr },
            { "doreset"        ,    SEC_CONSOLE,        SEC_CONSOLE,  false, &HandleWQuestDoReset,                      "", nullptr }
        };

        static ChatCommand commandTable[] =
        {
            { "wquest",         SEC_CONSOLE,            SEC_CONSOLE,  true, nullptr,                         "", wquestCommandTable },
            { nullptr,          SEC_CONSOLE,            SEC_CONSOLE,  true, nullptr,                                    "", nullptr }
        };
        return commandTable;
    }

    static bool HandleWQuestAdd(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!p_Args)
            return false;

        char* l_QuestIDStr = strtok(const_cast<char*>(p_Args), " ");

        if (!l_QuestIDStr)
            return false;

        uint32 l_QuestID = atoi(l_QuestIDStr);

        char* l_TimeRemainingStr = strtok(nullptr, " ");

        uint32 l_TimeRemaining = l_TimeRemainingStr ? atoi(l_TimeRemainingStr) : 0;

        sWorldQuestGenerator->AddQuest(MS::WorldQuest::Template::GetTemplate(l_QuestID), 0, l_TimeRemaining, true);
        return true;
    }

    static bool HandleWQuestAddAll(ChatHandler* p_Handler, char const* p_Args)
    {
        auto l_WQuests = MS::WorldQuest::Template::GetTemplates();

        for (auto l_Quest : l_WQuests)
            sWorldQuestGenerator->AddQuest(l_Quest.second, 0, 0, true);

        return true;
    }

    static bool HandleWQuestRemove(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!p_Args)
            return false;

        uint32 l_QuestID = atoi(p_Args);
        return sWorldQuestGenerator->ExpireQuest(l_QuestID);
    }

    static bool HandleWQuestReward(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!p_Args)
            return false;

        uint32 l_QuestID = atoi(p_Args);
        auto l_Player = p_Handler->GetSession()->GetPlayer();

        l_Player->GetWorldQuestManager()->Reward(l_QuestID, 0, false);
        return true;
    }

    static bool HandleWQuestDumpCategory(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!p_Args)
            return false;

        uint32 l_Category = atoi(p_Args);
        std::ofstream l_Stream;

        l_Stream.open("w_quest_dump.txt", std::ios_base::app | std::ios_base::out);

        if (!l_Stream.is_open())
            return false;

        for (uint32 l_ID = 0; l_ID < sItemXBonusTreeStore.GetNumRows(); ++l_ID)
        {
            auto l_Entry = sItemXBonusTreeStore.LookupEntry(l_ID);
            if (!l_Entry)
                continue;

            if (l_Entry->ItemBonusTreeCategory != l_Category)
                continue;

            auto l_Item = sObjectMgr->GetItemTemplate(l_Entry->ItemId);
            if (!l_Item)
                continue;

            if (l_Item->Bonding != ItemBondingType::BIND_WHEN_PICKED_UP)
                continue;

            l_Stream << "(x, " << "1, " << l_Entry->ItemId << ", 1, 1, 0)," << std::endl;
        }

        return true;
    }

    static bool HandleWQuestDumpLoot(ChatHandler* p_Handler, char const* p_Args)
    {
        if (!p_Args)
            return false;

        uint32 l_Type = atoi(p_Args);
        std::ofstream l_Stream;

        l_Stream.open("w_quest_dump_loot.txt", std::ios_base::app | std::ios_base::out);

        if (!l_Stream.is_open())
            return false;

        for (auto l_Iter : MS::WorldQuest::Template::GetTemplates())
        {
            if (l_Iter.second->GetWorldQuestType() != l_Type)
                continue;

            switch (l_Iter.second->GetWorldQuestType())
            {
                case MS::WorldQuest::WorldQuestType::RegularWorldQuest:
                {
                    l_Stream << "(" << l_Iter.second->GetID() << ", 1, 3)," << std::endl;
                    l_Stream << "(" << l_Iter.second->GetID() << ", 2, 3)," << std::endl;
                    l_Stream << "(" << l_Iter.second->GetID() << ", 3, 3)," << std::endl;
                    l_Stream << "(" << l_Iter.second->GetID() << ", 4, 3)," << std::endl;
                    l_Stream << "(" << l_Iter.second->GetID() << ", 5, 1)," << std::endl;
                    break;
                }
                default:
                    break;
            }
        }

        return true;
    }

    static bool HandleWQuestResetAllRewards(ChatHandler* p_Handler, char const* p_Args)
    {
        auto l_Player = p_Handler->GetSession()->GetPlayer();
        auto l_Mgr = l_Player->GetWorldQuestManager();

        for (auto l_Iter : MS::WorldQuest::Template::GetTemplates())
        {
            l_Player->RemoveRewardedQuest(l_Iter.second->GetID());
            l_Mgr->Expired(l_Iter.second->GetID());
        }

        l_Player->SendWorldQuestUpdate();
        l_Mgr->OnLogin();
        return true;
    }

    static bool HandleWQuestDoReset(ChatHandler* p_Handler, char const* p_Args)
    {
        sWorldQuestGenerator->ModifyAllDurations(-(12 * HOUR));
        sWorldQuestGenerator->Update(0);
        sWorldQuestGenerator->Reset(false);
        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_wquest_commandscript()
{
    new wquest_commandscript();
}
#endif
