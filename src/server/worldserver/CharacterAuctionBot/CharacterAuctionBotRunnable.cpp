////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2017 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "World.h"
#include "Database/DatabaseEnv.h"
#include "Timer.h"
#include "CharacterAuctionBotRunnable.h"
#include "Config.h"
#include "PlayerDump.h"

#define AuctionBotInterval 5 * IN_MILLISECONDS

void CharacterAuctionBotRunnable::run()
{
#ifndef CROSS
    const uint32 l_CharactersPerUpdate = ConfigMgr::GetIntDefault("CharacterAuctionBot.CharactersPerUpdate", 0);
    const uint32 l_UpdateTimer         = ConfigMgr::GetIntDefault("CharacterAuctionBot.UpdateTimer", 10) * MINUTE;
    const uint32 l_MinimumItemlevel    = ConfigMgr::GetIntDefault("CharacterAuctionBot.MinimumItemlevel", 840);
    const uint32 l_MaximumItemlevel    = ConfigMgr::GetIntDefault("CharacterAuctionBot.MaximumItemlevel", 865);
    const uint32 l_BotAccountId        = ConfigMgr::GetIntDefault("CharacterAuctionBot.AccountId", 0);

    uint32 l_NextTime = getMSTime() + (l_UpdateTimer * IN_MILLISECONDS);
    while (!sWorld->IsStopped())
    {
        uint32 l_TimeCurrent = getMSTime();

        /// see if time has passed
        if (l_TimeCurrent < l_NextTime)
        {
            /// check again few seconds
            ACE_Based::Thread::Sleep(std::min<uint32>(l_NextTime - l_TimeCurrent, AuctionBotInterval));
            continue;
        }

        for (int l_I = 0; l_I < l_CharactersPerUpdate; l_I++)
        {
            uint32 l_Ilevel = urand(l_MinimumItemlevel, l_MaximumItemlevel);

            QueryResult l_Result = CharacterDatabase.PQuery("SELECT guid, price FROM character_sell_data WHERE ilvl = %u ORDER BY rand() LIMIT 1", l_Ilevel);
            while (!l_Result)
            {
                /// Avoid CPU burn if the table is empty
                ACE_Based::Thread::Sleep(10);

                l_Ilevel = urand(l_MinimumItemlevel, l_MaximumItemlevel);
                l_Result = CharacterDatabase.PQuery("SELECT guid, price FROM character_sell_data WHERE ilvl = %u and lastsell < %u ORDER BY rand() LIMIT 1", l_Ilevel, time(nullptr) - 604800);
            }

            Field* l_Fields = l_Result->Fetch();

            std::string l_CharDump;
            if (PlayerDumpWriter().GetDump(l_Fields[0].GetUInt32(), 0, l_CharDump, false))
            {
                std::string l_Path = ConfigMgr::GetStringDefault("TransfersPath", "pdump/");

                std::ostringstream l_Filename;
                l_Filename << l_Path << l_Fields[0].GetUInt32() << "_" << time(nullptr);

                FILE* l_File = fopen(l_Filename.str().c_str(), "w");
                if (!l_File)
                    continue;

                fprintf(l_File, "%s\n", l_CharDump.c_str());
                fclose(l_File);

                uint32 l_Guid = 0;
                DumpReturn l_Error = PlayerDumpReader().LoadDump(l_Filename.str(), 0, std::string("#characterauction-") + std::to_string(l_Fields[0].GetUInt32()), l_Guid, true);

                if (l_Error == DumpReturn::DUMP_SUCCESS)
                {
                    WebDatabase.DirectPExecute
                    (
                        "INSERT INTO api_character_sales (ExpansionID, RealmID, AccountID, GUID, Price, Description, EndDate, AccountIDBuyer) VALUE (%u, %u, %u, %u, %u, '', DATE_ADD(NOW(), INTERVAL 7 DAY), 0)",
                        Expansion::EXPANSION_LEGION,
                        g_RealmID,
                        l_BotAccountId,
                        l_Guid,
                        l_Fields[1].GetUInt32()
                    );

                    CharacterDatabase.DirectPExecute("UPDATE character_sell_data SET lastsell = %u WHERE guid = %u", time(nullptr), l_Fields[0].GetUInt32());
                }

                remove(l_Filename.str().c_str());
            }
        }

        l_NextTime = getMSTime() + (l_UpdateTimer * IN_MILLISECONDS);
    }
#endif
}
