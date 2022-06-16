////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: quest_commandscript
%Complete: 100
Comment: All quest related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"

class quest_commandscript: public CommandScript
{
public:
    quest_commandscript() : CommandScript("quest_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand questCommandTable[] =
        {
            { "add",            SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleQuestAdd,                    "", NULL },
            { "complete",       SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleQuestComplete,               "", NULL },
            { "remove",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleQuestRemove,                 "", NULL },
            { "reward",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleQuestReward,                 "", NULL },
            { "info",           SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleQuestInfo,                   "", NULL },
            { "clearjournal",   SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleQuestClearJournal,           "", NULL },
            { "status",         SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, &HandleQuestStatus,                 "", NULL },
            { NULL,             SEC_PLAYER,             SEC_PLAYER,         false, NULL,                               "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "quest",          SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, NULL,                  "", questCommandTable },
            { NULL,             SEC_PLAYER,             SEC_PLAYER,         false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleQuestAdd(ChatHandler* handler, const char* args)
    {
        auto player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        // .addquest #entry'
        // number or [name] Shift-click form |color|Hquest:quest_id:quest_level|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
            return false;

        uint32 entry = atol(cId);

        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);

        if (!quest)
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check item starting quest (it can work incorrectly if added without item in inventory)
        ItemTemplateContainer const* itc = sObjectMgr->GetItemTemplateStore();
        ItemTemplateContainer::const_iterator result = find_if(itc->begin(), itc->end(), Finder<uint32, ItemTemplate>(entry, &ItemTemplate::StartQuest));

        if (result != itc->end())
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_STARTFROMITEM, entry, result->second.ItemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // ok, normal (creature/GO starting) quest
        if (player->CanAddQuest(quest, true))
        {
            player->AddQuest(quest, NULL);

            if (player->CanCompleteQuest(entry))
                player->CompleteQuest(entry);
        }

        return true;
    }

    static bool HandleQuestRemove(ChatHandler* handler, const char* args)
    {
        auto player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        // .removequest #entry'
        // number or [name] Shift-click form |color|Hquest:quest_id:quest_level|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
            return false;

        uint32 entry = atol(cId);

        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);

        if (!quest)
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // remove all quest entries for 'entry' from quest log
        for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        {
            uint32 logQuest = player->GetQuestSlotQuestId(slot);
            if (logQuest == entry)
            {
                player->SetQuestSlot(slot, 0);

                // we ignore unequippable quest items in this case, its' still be equipped
                player->TakeQuestSourceItem(logQuest, false);
            }
        }

        player->RemoveActiveQuest(entry);
        player->RemoveRewardedQuest(entry);

        handler->SendSysMessage(LANG_COMMAND_QUEST_REMOVED);
        return true;
    }

    static bool HandleQuestComplete(ChatHandler* handler, const char* args)
    {
        auto l_Player = handler->getSelectedPlayer();
        if (!l_Player)
            l_Player = handler->GetSession()->GetPlayer();

        // .quest complete #entry
        // number or [name] Shift-click form |color|Hquest:quest_id:quest_level|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
            return false;

        uint32 entry = atol(cId);

        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);

        // If player doesn't have the quest
        if (!quest || l_Player->GetQuestStatus(entry) == QUEST_STATUS_NONE)
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        for (QuestObjective l_Objective : quest->QuestObjectives)
        {
            if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_ITEM)
            {
                uint32 id = l_Objective.ObjectID;
                uint32 count = l_Objective.Amount;

                if (!id || !count)
                    continue;

                uint32 curItemCount = l_Player->GetItemCount(id, true);

                ItemPosCountVec dest;
                uint8 msg = l_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count - curItemCount);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = l_Player->StoreNewItem(dest, id, true);
                    l_Player->SendNewItem(item, count - curItemCount, true, false);
                }
            }
            else if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_NPC)
            {
                int32 creature = l_Objective.ObjectID;
                uint32 creaturecount = l_Objective.Amount;

                if (CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(creature))
                {
                    for (uint16 z = 0; z < creaturecount; ++z)
                        l_Player->KilledMonster(cInfo, 0);
                }
            }
            else if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_GO)
            {
                for (uint16 z = 0; z < l_Objective.Amount; ++z)
                    l_Player->CastedCreatureOrGO(l_Objective.ObjectID, 0, 0);
            }
            else if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_SPELL)
            {
                /// @TODO
            }
            else if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_CURRENCY)
            {
                if (!l_Objective.ObjectID || !l_Objective.Amount)
                    continue;

                l_Player->ModifyCurrency(l_Objective.ObjectID, l_Objective.Amount);
            }
            else if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_FACTION_MIN || l_Objective.Type == QUEST_OBJECTIVE_TYPE_FACTION_MAX)
            {
                if (l_Player->GetReputationMgr().GetReputation(l_Objective.ObjectID) < l_Objective.Amount)
                {
                    if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(l_Objective.ObjectID))
                        l_Player->GetReputationMgr().SetReputation(factionEntry, l_Objective.Amount);
                }
            }
            else if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_MONEY)
            {
                l_Player->ModifyMoney(l_Objective.Amount, "HandleQuestComplete " + std::to_string(entry));
            }
            else if (l_Objective.Type == QUEST_OBJECTIVE_TYPE_CRITERIA_TREE)
            {
                l_Player->QuestObjectiveSatisfy(l_Objective.ObjectID, l_Objective.Amount, l_Objective.Type);
            }
        }

        l_Player->CompleteQuest(entry);
        return true;
    }

    static bool HandleQuestReward(ChatHandler* handler, char const* args)
    {
        auto player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        // .quest reward #entry
        // number or [name] Shift-click form |color|Hquest:quest_id:quest_level|h[name]|h|r
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
            return false;

        uint32 entry = atol(cId);

        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);

        // If player doesn't have the quest
        if (!quest || player->GetQuestStatus(entry) != QUEST_STATUS_COMPLETE)
        {
            handler->PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->RewardQuest(quest, 0, player);
        return true;
    }

    static bool HandleQuestInfo(ChatHandler* p_Handler, char const* p_Args)
    {
        char* l_Quest = p_Handler->extractKeyFromLink((char*)p_Args, "Hquest");
        if (!l_Quest)
            return false;

        uint32 l_QuestID = atol(l_Quest);
        if (l_QuestID == 0)
        {
            p_Handler->PSendSysMessage("Please input a Quest ID different than 0");
            return false;
        }

        Player* l_Sender = p_Handler->GetSession() ? p_Handler->GetSession()->GetPlayer() : nullptr;
        if (!l_Sender)
            return false;

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_QUESTSTATUSREW_COUNT);
        l_Stmt->setUInt32(0, l_QuestID);

        CharacterDatabase.AsyncQuery(l_Stmt, p_Handler->GetSession()->GetPlayer()->GetQueryCallbackMgr(), [l_Sender, l_QuestID](PreparedQueryResult const& p_Result)
        {
            if (!p_Result)
            {
                ChatHandler(l_Sender->GetSession()).PSendSysMessage("No Results found for Quest %u", l_QuestID);
                return;
            }

            uint8 l_Count = 0;
            do
            {
                Field* l_Data = p_Result->Fetch();
                uint64 l_QuestCount = l_Data[0].GetUInt64();

                ChatHandler(l_Sender->GetSession()).PSendSysMessage("Quest %u has been completed " UI64FMTD " times on the realm", l_QuestID, l_QuestCount);

                l_Count++;
            } while (p_Result->NextRow() && l_Count <= 1);
        });

        return true;
    }

    static bool HandleQuestClearJournal(ChatHandler* handler, char const* args)
    {
        auto player = handler->GetSession()->GetPlayer();
        if (!player)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
        {
            auto logQuest = player->GetQuestSlotQuestId(slot);
            player->SetQuestSlot(slot, 0);
            player->TakeQuestSourceItem(logQuest, false);
            player->RemoveActiveQuest(logQuest);
            player->RemoveRewardedQuest(logQuest);
        }

        return true;
    }

    static bool HandleQuestStatus(ChatHandler* handler, char const* args)
    {
        auto player = handler->getSelectedPlayer();
        if (!player)
            player = handler->GetSession()->GetPlayer();

        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
            return false;

        uint32 entry = atol(cId);
        auto quest = sObjectMgr->GetQuestTemplate(entry);
        if (!quest)
        {
            handler->PSendSysMessage(13000);
            return true;
        }

        switch (player->GetQuestStatus(entry))
        {
            case QUEST_STATUS_INCOMPLETE:
                handler->PSendSysMessage(13001, entry);
                break;
            case QUEST_STATUS_COMPLETE:
                handler->PSendSysMessage(13002, entry);
                break;
            case QUEST_STATUS_REWARDED:
                handler->PSendSysMessage(13003, entry);
                break;
            case QUEST_STATUS_NONE:
                handler->PSendSysMessage(13004, entry);
                break;
            case QUEST_STATUS_FAILED:
                handler->PSendSysMessage(13005, entry);
                break;
            default:
                break;
        }
        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_quest_commandscript()
{
    new quest_commandscript();
}
#endif
