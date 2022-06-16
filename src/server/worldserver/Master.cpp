////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#include <ace/Sig_Handler.h>
#include <thread>

#include "Common.h"
#include "GitRevision.h"
#include "SignalHandler.h"
#include "World.h"
#include "WorldRunnable.h"
#ifndef CROSS
#include "WorldSocket.h"
#include "RealmSocketMgr.h"
#include "InstanceSocketMgr.h"
#endif /* not CROSS */
#include "Configuration/Config.h"
#include "Database/DatabaseEnv.h"
#include "Database/DatabaseWorkerPool.h"
#ifndef CROSS
#include "PlayerDump.h"
#endif /* not CROSS */
#include "Player.h"
#include "ObjectMgr.h"
#include "CinematicPathMgr.h"
#include "TemporarySummon.h"

#include "CliRunnable.h"
#include "Log.h"
#include "Master.h"
#include "RARunnable.h"
#include "TCSoap.h"
#include "Timer.h"
#include "Util.h"

#include "BigNumber.h"
#include "Reporter.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include "DatabaseLoader.h"
#include "CharacterAuctionBotRunnable.h"

#include "MapManager.h"

#include "TemporarySummon.h"

#ifdef CROSS
#include "Cross/IRSocketMgr.h"

#endif /* CROSS */
#ifdef _WIN32
#include "ServiceWin32.h"
extern int m_ServiceStatus;
#endif

enum RealmFlags
{
    REALM_FLAG_NONE                              = 0x00,
    REALM_FLAG_INVALID                           = 0x01,
    REALM_FLAG_OFFLINE                           = 0x02,
    REALM_FLAG_SPECIFYBUILD                      = 0x04,
    REALM_FLAG_UNK1                              = 0x08,
    REALM_FLAG_UNK2                              = 0x10,
    REALM_FLAG_RECOMMENDED                       = 0x20,
    REALM_FLAG_NEW                               = 0x40,
    REALM_FLAG_FULL                              = 0x80
};

/// Handle worldservers's termination signals
class WorldServerSignalHandler : public JadeCore::SignalHandler
{
    public:
        virtual void HandleSignal(int SigNum)
        {
            switch (SigNum)
            {
                case SIGINT:
                    World::StopNow(RESTART_EXIT_CODE);
                    break;
                case SIGTERM:
                #ifdef _WIN32
                case SIGBREAK:
                    if (m_ServiceStatus != 1)
                #endif /* _WIN32 */
                    World::StopNow(SHUTDOWN_EXIT_CODE);
                    break;
            }
        }
};

class FreezeDetector : public ACE_Based::Runnable
{
public:
    FreezeDetector(uint32 p_MaxCoreStuckTime)
        : m_WorldLoopCounter(0), m_LastChangeMsTime(0), m_MaxCoreStuckTimeInMs(p_MaxCoreStuckTime), m_CanStop(false) { }

    void run(void)
    {
        if (!m_MaxCoreStuckTimeInMs)
            return;

        sLog->outInfo(LOG_FILTER_WORLDSERVER, "Starting up anti-freeze thread (%u seconds max stuck time)...", m_MaxCoreStuckTimeInMs / IN_MILLISECONDS);

        while (!World::IsStopped())
        {
            ACE_Based::Thread::Sleep(1000);

            uint32 l_CurrTime = getMSTime();
            uint32 l_WorldLoopCounter = World::m_worldLoopCounter;
            if (m_WorldLoopCounter != l_WorldLoopCounter)
            {
                m_LastChangeMsTime = l_CurrTime;
                m_WorldLoopCounter = l_WorldLoopCounter;
            }
            // possible freeze
            else if (getMSTimeDiff(m_LastChangeMsTime, l_CurrTime) > m_MaxCoreStuckTimeInMs)
            {
                sLog->outError(LOG_FILTER_WORLDSERVER, "World Thread hangs, kicking out server!");
                assert(false);
                abort();
            }
        }

        /// Protect against freeze on shutdown
        uint32 l_WorldStopTime = time(nullptr);

        while (!m_CanStop)
        {
            ACE_Based::Thread::Sleep(1000);

            if ((time(nullptr) - l_WorldStopTime) > 60)
            {
                sLog->outError(LOG_FILTER_WORLDSERVER, "Freeze on shutdown, kill the server!");
                assert(false);
                abort();
            }
        }

        sLog->outInfo(LOG_FILTER_WORLDSERVER, "Anti-freeze thread exiting without problems.");
    }

    void SetCanStop() { m_CanStop = true; }

private:
    uint32 m_WorldLoopCounter;
    uint32 m_LastChangeMsTime;
    uint32 m_MaxCoreStuckTimeInMs;
    bool m_CanStop;
};

#ifndef CROSS
class GmLogToDBRunnable : public ACE_Based::Runnable
{
public:

    void run ()
    {
        while (!World::IsStopped())
        {
            GmCommand * command;
            while(!GmLogQueue.empty())
            {
                GmLogQueue.next(command);
                CharacterDatabase.EscapeString(command->accountName[0]);
                CharacterDatabase.EscapeString(command->accountName[1]);
                CharacterDatabase.EscapeString(command->characterName[0]);
                CharacterDatabase.EscapeString(command->characterName[1]);
                CharacterDatabase.EscapeString(command->command);
                //No sql injections. Strings are escaped.

                //au cas ou on a pas les infos...
                if (command->accountName[0] == "" && command->accountID[0] != 0)
                {
                    QueryResult result = LoginDatabase.PQuery("SELECT username FROM account WHERE id = %u", command->accountID[0]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        command->accountName[0] = fields[0].GetString();
                        CharacterDatabase.EscapeString(command->accountName[0]);
                    }
                }

                if (command->accountName[1] == "" && command->accountID[1] != 0)
                {
                    QueryResult result = LoginDatabase.PQuery("SELECT username FROM account WHERE id = %u", command->accountID[1]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        command->accountName[1] = fields[0].GetString();
                        CharacterDatabase.EscapeString(command->accountName[1]);
                    }
                }

                if (command->characterName[0] == "" && command->characterID[0] != 0)
                {
                    QueryResult result = CharacterDatabase.PQuery("SELECT name FROM character WHERE guid = %u", command->characterID[0]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        command->characterName[0] = fields[0].GetString();
                        CharacterDatabase.EscapeString(command->characterName[0]);
                    }
                }

                if (command->characterName[0] == "" && command->characterID[0] != 0)
                {
                    QueryResult result = CharacterDatabase.PQuery("SELECT name FROM character WHERE guid = %u", command->characterID[0]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        command->characterName[0] = fields[0].GetString();
                        CharacterDatabase.EscapeString(command->characterName[0]);
                    }
                }

                if (command->characterName[1] == "" && command->characterID[1] != 0)
                {
                    QueryResult result = CharacterDatabase.PQuery("SELECT name FROM character WHERE guid = %u", command->characterID[1]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        command->characterName[1] = fields[0].GetString();
                        CharacterDatabase.EscapeString(command->characterName[1]);
                    }
                }

                std::string last_ip = " ";

                QueryResult result = LoginDatabase.PQuery("SELECT last_ip FROM account WHERE id = %u", command->accountID[0]);
                if (result)
                {
                    Field *fields_ip = result->Fetch();
                    last_ip = fields_ip[0].GetString();
                }

                if (command->accountID[1] == 0 && command->characterID[1] == 0)
                {
                    command->accountID[1] = command->accountID[0];
                    command->characterID[1] = command->characterID[0];
                    command->accountName[1] = command->accountName[0];
                    command->characterName[1] = command->characterName[0];
                }

                PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_GM_LOG);
                l_Statement->setUInt32(0, command->accountID[0]);
                l_Statement->setString(1, command->accountName[0].c_str());
                l_Statement->setUInt32(2, command->characterID[0]);
                l_Statement->setString(3, command->characterName[0].c_str());
                l_Statement->setString(4, last_ip.c_str());
                l_Statement->setUInt32(5, command->accountID[1]);
                l_Statement->setString(6, command->accountName[1].c_str());
                l_Statement->setUInt32(7, command->characterID[1]);
                l_Statement->setString(8, command->characterName[1].c_str());
                l_Statement->setString(9, command->command.c_str());
                LogDatabase.Execute(l_Statement);

                delete command;
            }
            ACE_Based::Thread::Sleep(1000);
        }
    }
};

class GmChatLogToDBRunnable : public ACE_Based::Runnable
{
public:

    void run ()
    {
        while (!World::IsStopped())
        {
            GmChat * ChatLog;
            while(!GmChatLogQueue.empty())
            {
                GmChatLogQueue.next(ChatLog);
                CharacterDatabase.EscapeString(ChatLog->accountName[0]);
                CharacterDatabase.EscapeString(ChatLog->accountName[1]);
                CharacterDatabase.EscapeString(ChatLog->characterName[0]);
                CharacterDatabase.EscapeString(ChatLog->characterName[1]);
                CharacterDatabase.EscapeString(ChatLog->message);
                //No sql injections. Strings are escaped.

                /// Don't log .ticket command / anti cheat command, they are spammed by the gm addon
                if (boost::contains(ChatLog->message, ".ticket") || boost::contains(ChatLog->message, ".ant g"))
                    return;

                //au cas ou on a pas les infos...
                if (ChatLog->accountName[0] == "" && ChatLog->accountID[0] != 0)
                {
                    QueryResult result = LoginDatabase.PQuery("SELECT username FROM account WHERE id = %u", ChatLog->accountID[0]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        ChatLog->accountName[0] = fields[0].GetString();
                        CharacterDatabase.EscapeString(ChatLog->accountName[0]);
                    }
                }

                if (ChatLog->accountName[1] == "" && ChatLog->accountID[1] != 0)
                {
                    QueryResult result = LoginDatabase.PQuery("SELECT username FROM account WHERE id = %u", ChatLog->accountID[1]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        ChatLog->accountName[1] = fields[0].GetString();
                        CharacterDatabase.EscapeString(ChatLog->accountName[1]);
                    }
                }

                if (ChatLog->characterName[0] == "" && ChatLog->characterID[0] != 0)
                {
                    QueryResult result = CharacterDatabase.PQuery("SELECT name FROM character WHERE guid = %u", ChatLog->characterID[0]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        ChatLog->characterName[0] = fields[0].GetString();
                        CharacterDatabase.EscapeString(ChatLog->characterName[0]);
                    }
                }

                if (ChatLog->characterName[1] == "" && ChatLog->characterID[1] != 0)
                {
                    QueryResult result = CharacterDatabase.PQuery("SELECT name FROM character WHERE guid = %u", ChatLog->characterID[1]);
                    if (result)
                    {
                        Field *fields = result->Fetch();
                        ChatLog->characterName[1] = fields[0].GetString();
                        CharacterDatabase.EscapeString(ChatLog->characterName[1]);
                    }
                }

                PreparedStatement* l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_GM_CHAT_LOG);
                l_Statement->setUInt32(0, ChatLog->type);
                l_Statement->setUInt32(1, ChatLog->accountID[0]);
                l_Statement->setString(2, ChatLog->accountName[0].c_str());
                l_Statement->setUInt32(3, ChatLog->characterID[0]);
                l_Statement->setString(4, ChatLog->characterName[0]);
                l_Statement->setUInt32(5, ChatLog->accountID[1]);
                l_Statement->setString(6, ChatLog->accountName[1].c_str());
                l_Statement->setUInt32(7, ChatLog->characterID[1]);
                l_Statement->setString(8, ChatLog->characterName[1]);
                l_Statement->setString(9, ChatLog->message.c_str());
                LogDatabase.Execute(l_Statement);

                delete ChatLog;
            }
            ACE_Based::Thread::Sleep(1000);
        }
    }
};

#define TransfersPeriodicTimer      10 * MINUTE * IN_MILLISECONDS
#define TransfersPeriodicInterval   5 * IN_MILLISECONDS

class InterExpTransfersRunnable : public ACE_Based::Runnable
{
public:

    void run()
    {
        uint32 l_NextTime = getMSTime() + TransfersPeriodicTimer;

        while (!World::IsStopped())
        {
            uint32 l_TimeCurrent = getMSTime();

            /// see if time has passed
            if (l_TimeCurrent < l_NextTime)
            {
                /// check again few seconds
                ACE_Based::Thread::Sleep(std::min<uint32>(l_NextTime - l_TimeCurrent, TransfersPeriodicInterval));
                continue;
            }

            PreparedStatement* l_Stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_TRANSFERS_EXP_LOAD);
            l_Stmt->setUInt32(0, sLog->GetRealmID());
            PreparedQueryResult l_Result = LoginDatabase.Query(l_Stmt);

            if (l_Result)
            {
                do
                {
                    uint32 l_Timestamp      = getMSTime();
                    Field* l_Field          = l_Result->Fetch();
                    uint32 l_Transaction    = l_Field[0].GetUInt32();
                    uint32 l_AccountID      = l_Field[1].GetUInt32();
                    uint32 l_CharGUID       = l_Field[2].GetUInt32();
                    std::string l_CharDump  = l_Field[3].GetString();
                    uint32 l_StartRealm     = l_Field[4].GetUInt32();

                    /// Transfers aren't allowed with legacy account
                    if (l_AccountID & 0x40000000)
                        continue;

                    std::string l_Path = ConfigMgr::GetStringDefault("TransfersPath", "pdump/");

                    std::ostringstream l_Filename;
                    l_Filename << l_Path << l_AccountID << "_" << l_CharGUID << "_" << l_Timestamp;

                    FILE* l_File = fopen(l_Filename.str().c_str(), "w");
                    if (!l_File)
                        continue;

                    fprintf(l_File, "%s\n", l_CharDump.c_str());
                    fclose(l_File);

                    uint32 l_Guid = 0;

                    DumpReturn l_Error = PlayerDumpReader().LoadDump(l_Filename.str(), l_AccountID, "#Transfer", l_Guid, true, AtLoginFlags::AT_LOGIN_RENAME | AtLoginFlags::AT_LOGIN_DELETE_INVALID_SPELL, l_Transaction, l_StartRealm);
                    remove(l_Filename.str().c_str());

                    if (l_Error != DUMP_SUCCESS)
                    {
                        LoginDatabase.PQuery("UPDATE webshop_delivery_interexp_transfer SET error = %u, state = 0, nb_attempt = nb_attempt + 1 WHERE id = %u", (uint32)l_Error, l_Transaction);
                        if (l_Error != DUMP_TOO_MANY_CHARS)
                            sLog->outExtChat("#jarvis", "danger", true, "Inter Exp Transfer to realm [%u] on account [%u] failed. ErrorCode [%u]", g_RealmID, l_AccountID, l_Error);
                    }
                } while (l_Result->NextRow());
            }

            QueryResult l_ToDump = LoginDatabase.PQuery("SELECT `id`, `account`, `guid`, `exp` FROM webshop_delivery_interexp_transfer WHERE `startrealm` = %u AND state = 0", g_RealmID);

            if (l_ToDump)
            {
                do
                {
                    Field* l_Fields = l_ToDump->Fetch();

                    uint32 l_Id = l_Fields[0].GetUInt32();
                    uint32 l_Account = l_Fields[1].GetUInt32();
                    uint32 l_Guid = l_Fields[2].GetUInt32();
                    uint32 l_Expansion = l_Fields[3].GetUInt32();

                    /// Don't dump online character
                    if (Player * l_Player = sObjectMgr->GetPlayerByLowGUID(l_Guid))
                        continue;

                    std::string l_Dump;
                    if (PlayerDumpWriter().GetDump(l_Guid, l_Account, l_Dump, l_Expansion))
                    {
                        PreparedStatement* l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_UPD_TRANSFER_EXP);
                        l_Statement->setString(0, l_Dump);
                        l_Statement->setUInt32(1, l_Id);
                        LoginDatabase.Execute(l_Statement);
                    }
                }
                while (l_ToDump->NextRow());
            }

            l_NextTime = getMSTime() + TransfersPeriodicTimer;
        }
    }
};

#endif /* not CROSS */
const char* dumpTables[32] =
{
    "characters",
    "character_account_data",
    "character_achievement",
    "character_achievement_progress",
    "character_action",
    "character_aura",
    "character_aura_effect",
    "character_currency",
    //{ "character_cuf_profiles",
    "character_declinedname",
    "character_equipmentsets",
    "character_gifts",
    "character_glyphs",
    "character_homebind",
    "character_inventory",
    "character_pet",
    "character_pet_declinedname",
    "character_queststatus",
    "character_queststatus_rewarded",
    "character_rates",
    "character_reputation",
    "character_skills",
    "character_spell",
    "character_spell_cooldown",
    "character_talent",
    "character_void_storage",
    "item_instance",
    "mail",
    "mail_items",
    "pet_aura",
    "pet_aura_effect",
    "pet_spell",
    "pet_spell_cooldown"
};

Master::Master() { }

Master::~Master() { }

/// Main function
int Master::Run()
{
    init_sfmt();
    BigNumber seed1;
    seed1.SetRand(16 * 8);

    sLog->outInfo(LOG_FILTER_WORLDSERVER, "%s (worldserver-daemon)", GitRevision::GetFullVersion());
    sLog->outInfo(LOG_FILTER_WORLDSERVER, "<Ctrl-C> to stop.\n");

    sLog->outInfo(LOG_FILTER_WORLDSERVER,"   _                _             _____                ");
    sLog->outInfo(LOG_FILTER_WORLDSERVER,"  | |              (_)           /  __ \\               ");
    sLog->outInfo(LOG_FILTER_WORLDSERVER,"  | |     ___  __ _ _  ___  _ __ | /  \\/ ___  _ __ ___ ");
    sLog->outInfo(LOG_FILTER_WORLDSERVER,"  | |    / _ \\/ _` | |/ _ \\| '_ \\| |    / _ \\| '__/ _ \\");
    sLog->outInfo(LOG_FILTER_WORLDSERVER,"  | |___|  __/ (_| | | (_) | | | | \\__/\\ (_) | | |  __/");
    sLog->outInfo(LOG_FILTER_WORLDSERVER,"  \\_____/\\___|\\__, |_|\\___/|_| |_|\\____/\\___/|_|  \\___|");
    sLog->outInfo(LOG_FILTER_WORLDSERVER,"               __/ |                                   ");
    sLog->outInfo(LOG_FILTER_WORLDSERVER,"              |___/                                    ");
    sLog->outInfo(LOG_FILTER_WORLDSERVER, " MILLENIUM STUDIO SARL\n");

    /// worldserver PID file creation
    std::string pidfile = ConfigMgr::GetStringDefault("PidFile", "");
    if (!pidfile.empty())
    {
        uint32 pid = CreatePIDFile(pidfile);
        if (!pid)
        {
            sLog->outError(LOG_FILTER_WORLDSERVER, "Cannot create PID file %s.\n", pidfile.c_str());
            return 1;
        }

        sLog->outInfo(LOG_FILTER_WORLDSERVER, "Daemon PID: %u\n", pid);
    }

    ///- Start the databases
    if (!_StartDB())
        return 1;

    ExecutePendingRequests();

    // set server offline (not connectable)
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = (flag & ~%u) | %u WHERE id = '%d'", REALM_FLAG_OFFLINE, REALM_FLAG_INVALID, g_RealmID);

    ///- Initialize the World
    sWorld->SetInitialWorldSettings();

    ///- Initialize the signal handlers
    WorldServerSignalHandler SignalINT, SignalTERM;
    #ifdef _WIN32
    WorldServerSignalHandler SignalBREAK;
    #endif /* _WIN32 */

    ///- Register worldserver's signal handlers
    ACE_Sig_Handler Handler;
    Handler.register_handler(SIGINT, &SignalINT);
    Handler.register_handler(SIGTERM, &SignalTERM);
    #ifdef _WIN32
    Handler.register_handler(SIGBREAK, &SignalBREAK);
    #endif /* _WIN32 */

    ///- Initializing the Reporter.
    sLog->outInfo(LOG_FILTER_WORLDSERVER, "Reporter: Initializing instance...");

    sReporter->SetAddress(ConfigMgr::GetStringDefault("Reporting.Address", "http://127.0.0.1:9200/"));
    sReporter->SetIndex(ConfigMgr::GetStringDefault("Reporting.Index", "firestorm/guild_ranking/?pretty"));

    /// Thread which repeat reporting.
    std::thread l_Reporter([]()
    {
        while (!World::IsStopped())
        {
            if (sReporter->HasReports())
                sReporter->ScheduleNextReport();

            ACE_Based::Thread::current()->Sleep(1);
        }
    });

    ///- Launch WorldRunnable thread
    ACE_Based::Thread world_thread(new WorldRunnable, "WorldRunnable");
    world_thread.setPriority(ACE_Based::Highest);

    ACE_Based::Thread* cliThread = NULL;

#ifdef _WIN32
    if (ConfigMgr::GetBoolDefault("Console.Enable", true) && (m_ServiceStatus == -1)/* need disable console in service mode*/)
#else
    if (ConfigMgr::GetBoolDefault("Console.Enable", true))
#endif
    {
        ///- Launch CliRunnable thread
        cliThread = new ACE_Based::Thread(new CliRunnable, "CliRunnable");
    }

    ACE_Based::Thread rar_thread(new RARunnable, "RARunnable");
#ifndef CROSS
    ACE_Based::Thread gmLogToDB_thread(new GmLogToDBRunnable, "GmLogToDBRunnable");
    ACE_Based::Thread gmChatLogToDB_thread(new GmChatLogToDBRunnable, "GmChatLogToDBRunnable");
    ACE_Based::Thread maps_updater_thread(new MapsUpdaterRunnable, "MapsUpdateRunnable");
    maps_updater_thread.setPriority(ACE_Based::Highest);
#endif /* not CROSS */

    ///- Handle affinity for multiple processors and process priority on Windows
    #ifdef _WIN32
    {
        HANDLE hProcess = GetCurrentProcess();

        uint32 Aff = ConfigMgr::GetIntDefault("UseProcessors", 0);
        if (Aff > 0)
        {
            ULONG_PTR appAff;
            ULONG_PTR sysAff;

            if (GetProcessAffinityMask(hProcess, &appAff, &sysAff))
            {
                ULONG_PTR curAff = Aff & appAff;            // remove non accessible processors

                if (!curAff)
                {
                    sLog->outError(LOG_FILTER_WORLDSERVER, "Processors marked in UseProcessors bitmask (hex) %x are not accessible for the worldserver. Accessible processors bitmask (hex): %x", Aff, appAff);
                }
                else
                {
                    if (SetProcessAffinityMask(hProcess, curAff))
                        sLog->outInfo(LOG_FILTER_WORLDSERVER, "Using processors (bitmask, hex): %x", curAff);
                    else
                        sLog->outError(LOG_FILTER_WORLDSERVER, "Can't set used processors (hex): %x", curAff);
                }
            }
        }

        bool Prio = ConfigMgr::GetBoolDefault("ProcessPriority", false);

        //if (Prio && (m_ServiceStatus == -1)  /* need set to default process priority class in service mode*/)
        if (Prio)
        {
            if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
                sLog->outInfo(LOG_FILTER_WORLDSERVER, "worldserver process priority class set to HIGH");
            else
                sLog->outError(LOG_FILTER_WORLDSERVER, "Can't set worldserver process priority class.");
        }
    }
    #endif

    //Start soap serving thread
    std::thread* l_SoapThread = nullptr;

    if (ConfigMgr::GetBoolDefault("SOAP.Enabled", false))
    {
        l_SoapThread = new std::thread(TCSoapThread, ConfigMgr::GetStringDefault("SOAP.IP", "127.0.0.1"), uint16(ConfigMgr::GetIntDefault("SOAP.Port", 7878)));
    }

    ///- Start up freeze catcher thread
    FreezeDetector* l_FreezeDetector = nullptr;

    if (uint32 l_CoreStuckTime = ConfigMgr::GetIntDefault("MaxCoreStuckTime", 0))
    {
        l_FreezeDetector = new FreezeDetector(l_CoreStuckTime * IN_MILLISECONDS);
        ACE_Based::Thread freeze_thread(l_FreezeDetector, "FreezeDetector");
        freeze_thread.setPriority(ACE_Based::Highest);
    }

#ifndef CROSS
    ///- Launch the world listener socket
    uint16 l_RealmPort = sWorld->getIntConfig(CONFIG_PORT_WORLD);
    std::string bind_ip = ConfigMgr::GetStringDefault("BindIP", "0.0.0.0");
    if (sRealmSocketMgr->StartNetwork(l_RealmPort, bind_ip.c_str()) == -1)
    {
        sLog->outError(LOG_FILTER_WORLDSERVER, "Failed to start network");
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }

    uint16 l_InstancePort = sWorld->getIntConfig(CONFIG_PORT_INSTANCE);
    if (sInstanceSocketMgr->StartNetwork(l_InstancePort, bind_ip.c_str()) == -1)
    {
        sLog->outError(LOG_FILTER_WORLDSERVER, "Failed to start network");
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }
#else /* CROSS */
    uint16 wsport = ConfigMgr::GetIntDefault("InterRealmServer.Port", 12345);
    if (sIRSocketMgr->StartNetwork(wsport) == -1)
    {
        sLog->outError(LOG_FILTER_INTERREALM, "Failed to start network");
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }
#endif

#ifndef CROSS
    // set server online (allow connecting now)
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = flag & ~%u, population = 0 WHERE id = '%u'", REALM_FLAG_INVALID, g_RealmID);

    InterRealmSession* irt = new InterRealmSession();
    ACE_Based::Thread interrealm_thread(irt, "InterRealm");
    interrealm_thread.setPriority(ACE_Based::Highest);
    sWorld->SetInterRealmSession(irt);

    ACE_Based::Thread l_InterExpTransfers(new InterExpTransfersRunnable, "InterExpTransfersRunnable");
    ACE_Based::Thread l_CharacterAuctionBot(new CharacterAuctionBotRunnable, "CharacterAuctionBotRunnable");
#endif /* not CROSS */

    sLog->outInfo(LOG_FILTER_WORLDSERVER, "%s (worldserver-daemon) ready...", GitRevision::GetFullVersion());

    // when the main thread closes the singletons get unloaded
    // since worldrunnable uses them, it will crash if unloaded after master
#ifndef CROSS
    interrealm_thread.wait();
#endif

    world_thread.wait();
    rar_thread.wait();
    l_Reporter.join();

#ifndef CROSS
    l_InterExpTransfers.wait();
    l_CharacterAuctionBot.wait();
    maps_updater_thread.wait();
#endif

    if (l_SoapThread != nullptr)
    {
        l_SoapThread->join();
        delete l_SoapThread;
    }

    // set server offline
    LoginDatabase.DirectPExecute("UPDATE realmlist SET flag = flag | %u WHERE id = '%d'", REALM_FLAG_OFFLINE, g_RealmID);

    ///- Clean database before leaving
    ClearOnlineAccounts();

    _StopDB();

    sLog->outInfo(LOG_FILTER_WORLDSERVER, "Halting process...");

    if (cliThread)
    {
        #ifdef _WIN32

        // this only way to terminate CLI thread exist at Win32 (alt. way exist only in Windows Vista API)
        //_exit(1);
        // send keyboard input to safely unblock the CLI thread
        INPUT_RECORD b[5];
        HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        b[0].EventType = KEY_EVENT;
        b[0].Event.KeyEvent.bKeyDown = TRUE;
        b[0].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[0].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[0].Event.KeyEvent.wRepeatCount = 1;

        b[1].EventType = KEY_EVENT;
        b[1].Event.KeyEvent.bKeyDown = FALSE;
        b[1].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[1].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[1].Event.KeyEvent.wRepeatCount = 1;

        b[2].EventType = KEY_EVENT;
        b[2].Event.KeyEvent.bKeyDown = TRUE;
        b[2].Event.KeyEvent.dwControlKeyState = 0;
        b[2].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[2].Event.KeyEvent.wRepeatCount = 1;
        b[2].Event.KeyEvent.wVirtualScanCode = 0x1c;

        b[3].EventType = KEY_EVENT;
        b[3].Event.KeyEvent.bKeyDown = FALSE;
        b[3].Event.KeyEvent.dwControlKeyState = 0;
        b[3].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[3].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[3].Event.KeyEvent.wVirtualScanCode = 0x1c;
        b[3].Event.KeyEvent.wRepeatCount = 1;
        DWORD numb;
        WriteConsoleInput(hStdIn, b, 4, &numb);

        cliThread->wait();

        #else

        cliThread->destroy();

        #endif

        delete cliThread;
    }

    // for some unknown reason, unloading scripts here and not in worldrunnable
    // fixes a memory leak related to detaching threads from the module
    //UnloadScriptingModule();

    if (l_FreezeDetector)
        l_FreezeDetector->SetCanStop();

    // Exit the process with specified return value
    return World::GetExitCode();
}

/// Initialize connection to the databases
bool Master::_StartDB()
{
    MySQL::Library_Init();

    // Load databases
    DatabaseLoader loader("server.worldserver", DatabaseLoader::DATABASE_NONE);
    loader
        .AddDatabase(HotfixDatabase, "Hotfix")
        .AddDatabase(WorldDatabase, "World")
        .AddDatabase(CharacterDatabase, "Character")
        .AddDatabase(LoginDatabase, "Login")
        .AddDatabase(LogDatabase, "Logs");

    if (ConfigMgr::GetBoolDefault("WebDatabase.enable", false))
        loader.AddDatabase(WebDatabase, "Web");

    if (!loader.Load())
        return false;

    if (g_DatabaseUpdaterOnly)
        return true;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get the realm Id from the configuration file
    g_RealmID = ConfigMgr::GetIntDefault("RealmID", 0);
    if (!g_RealmID)
    {
        sLog->outError(LOG_FILTER_WORLDSERVER, "Realm ID not defined in configuration file");
        return false;
    }
    sLog->outInfo(LOG_FILTER_WORLDSERVER, "Realm running as realm ID %d", g_RealmID);

    sLog->SetRealmID(g_RealmID);

    /// Clean the database before starting
    ClearOnlineAccounts();

    /// Insert version info into DB
    WorldDatabase.PExecute("UPDATE version SET core_version = '%s', core_revision = '%s'", GitRevision::GetFullVersion(), GitRevision::GetHash());  ///< One-time query

    sWorld->LoadDBVersion();

    sLog->outInfo(LOG_FILTER_WORLDSERVER, "Using World DB: %s", sWorld->GetDBVersion());
    return true;
}

void Master::_StopDB()
{
    CharacterDatabase.Close();
    WorldDatabase.Close();
    LoginDatabase.Close();

    MySQL::Library_End();
}

/// Clear 'online' status for all accounts with characters in this realm
void Master::ClearOnlineAccounts()
{
    // Reset online status for all accounts with characters on the current realm
    LoginDatabase.DirectPExecute("UPDATE account SET online = 0 WHERE online > 0 AND id IN (SELECT acctid FROM realmcharacters WHERE realmid = %d)", g_RealmID);

    // Reset online status for all characters
    CharacterDatabase.DirectExecute("UPDATE characters SET online = 0 WHERE online <> 0");

    // Battleground instance ids reset at server restart
    CharacterDatabase.DirectExecute("UPDATE character_battleground_data SET instanceId = 0");
}

/// Execute pending SQL requests
#define PENDING_SQL_FILENAME "PendingSQL_Characters.sql"

void Master::ExecutePendingRequests()
{
    if (FILE* l_PendingRequestsFile = fopen(PENDING_SQL_FILENAME, "rb"))
    {
        fseek(l_PendingRequestsFile, 0, SEEK_END);
        long l_Size = ftell(l_PendingRequestsFile);
        rewind(l_PendingRequestsFile);

        if (l_Size > 0)
        {
            std::vector<char> l_Content(l_Size + 1);
            fread(l_Content.data(), l_Size, 1, l_PendingRequestsFile);
            l_Content[l_Size] = 0;

            CharacterDatabase.DirectExecute(l_Content.data());
            sLog->outInfo(LOG_FILTER_WORLDSERVER, PENDING_SQL_FILENAME " has been executed with success"); ///< Because if the above failed, the core would crash
        }
        else
            sLog->outInfo(LOG_FILTER_WORLDSERVER, PENDING_SQL_FILENAME " is empty, ignoring.");

        fclose(l_PendingRequestsFile);

        /// Clear file
        if ((l_PendingRequestsFile = fopen(PENDING_SQL_FILENAME, "w"))) ///< Using the result of an assignment as a condition without parentheses
            fclose(l_PendingRequestsFile);
    }
    else
        sLog->outInfo(LOG_FILTER_WORLDSERVER, "Unable to open " PENDING_SQL_FILENAME ", ignoring.");
}
