////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
Name: achievement_commandscript
%Complete: 100
Comment: All achievement related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "Chat.h"

class achievement_commandscript: public CommandScript
{
public:
    achievement_commandscript() : CommandScript("achievement_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand achievementCommandTable[] =
        {
            { "add",            SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleAchievementAddCommand,            "", NULL },
            { "generatereward", SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleAchievementGenerateRewardCommand, "", NULL },
            { "info",           SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false,  &HandleAchievementInfoCommand,           "", NULL },
            { NULL,             0,                      0,                  false,  NULL,                                    "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "achievement",    SEC_ADMINISTRATOR,      SEC_ADMINISTRATOR,  false, NULL,            "", achievementCommandTable },
            { NULL,             0,                      0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleAchievementAddCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 achievementId = atoi((char*)args);
        if (!achievementId)
        {
            if (char* id = handler->extractKeyFromLink((char*)args, "Hachievement"))
                achievementId = atoi(id);
            if (!achievementId)
                return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (AchievementEntry const* achievementEntry = sAchievementStore.LookupEntry(achievementId))
            target->CompletedAchievement(achievementEntry);

        return true;
    }

    static bool HandleAchievementGenerateRewardCommand(ChatHandler* /*p_ChatHandler*/, char const* /*p_Args*/)
    {
        FILE* l_File = fopen("AchievementReward.sql", "w+");
        if (l_File == nullptr)
            return false;

        for (uint32 l_I = 0; l_I < sAchievementStore.GetNumRows(); l_I++)
        {
            auto l_AchievementEntry = sAchievementStore.LookupEntry(l_I);
            if (l_AchievementEntry == nullptr)
                continue;

            std::string l_RewardText = l_AchievementEntry->RewardLang->Get(sWorld->GetDefaultDb2Locale());
            if (l_RewardText.empty())
                continue;


            /// - Handle title
            std::string l_Match = "Title Reward: ";
            if (l_RewardText.find(l_Match) != std::string::npos)
            {
                std::string l_TitleNameToMatch = l_RewardText.substr(l_Match.size());

                for (uint32 l_TitleIdx = 0; l_TitleIdx < sCharTitlesStore.GetNumRows(); l_TitleIdx++)
                {
                    auto l_TitleEntry = sCharTitlesStore.LookupEntry(l_TitleIdx);
                    if (l_TitleEntry == nullptr)
                        continue;

                    std::string l_TitleName = l_TitleEntry->NameMale->Get(sWorld->GetDefaultDb2Locale());
                    if (l_TitleName.find(l_TitleNameToMatch) != std::string::npos)
                    {
                        std::ostringstream l_Query;
                        l_Query << "INSERT INTO achievement_reward (entry, title_A, title_H) VALUES (" << l_AchievementEntry->ID << ", " << l_TitleEntry->ID << ", " << l_TitleEntry->ID << ");\r\n";
                        fputs(l_Query.str().c_str(), l_File);
                        break;
                    }
                }
            }
        }

        fflush(l_File);
        fclose(l_File);

        return true;
    }

    static bool HandleAchievementInfoCommand(ChatHandler* p_Handler, char const* p_Args)
    {
        char* l_Achievement = p_Handler->extractKeyFromLink((char*)p_Args, "Hachievement");
        if (!l_Achievement)
            return false;

        uint32 l_AchievementID = atol(l_Achievement);
        if (l_AchievementID == 0)
        {
            p_Handler->PSendSysMessage("Please input an Achievement ID different than 0");
            return false;
        }

        Player* l_Sender = p_Handler->GetSession() ? p_Handler->GetSession()->GetPlayer() : nullptr;
        if (!l_Sender)
            return false;

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_ACHIEVEMENTS_COUNT);
        l_Stmt->setUInt32(0, l_AchievementID);

        CharacterDatabase.AsyncQuery(l_Stmt, p_Handler->GetSession()->GetPlayer()->GetQueryCallbackMgr(), [l_Sender, l_AchievementID](PreparedQueryResult const& p_Result)
        {
            if (!p_Result)
            {
                ChatHandler(l_Sender->GetSession()).PSendSysMessage("No Results found for Achievement %u", l_AchievementID);
                return;
            }

            uint8 l_Count = 0;
            do
            {
                Field* l_Data = p_Result->Fetch();
                uint64 l_AchievCount = l_Data[0].GetUInt64();

                ChatHandler(l_Sender->GetSession()).PSendSysMessage("Achievement %u has been obtained by " UI64FMTD " characters on the realm", l_AchievementID, l_AchievCount);

                l_Count++;
            } while (p_Result->NextRow() && l_Count <= 1);
        });

        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_achievement_commandscript()
{
    new achievement_commandscript();
}
#endif
