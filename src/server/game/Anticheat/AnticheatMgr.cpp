////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "AnticheatMgr.h"
#include "AnticheatScripts.h"
#include "MapManager.h"

#define CLIMB_ANGLE 1.9f

AnticheatMgr::AnticheatMgr()
{
}

AnticheatMgr::~AnticheatMgr()
{
    m_Players.clear();
}

void AnticheatMgr::FlyHackDetection(Player* player, MovementInfo const& movementInfo)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & FLY_HACK_DETECTION) == 0)
        return;

    /// A few areas cannot use the height check
    if (player->IsInNoPathArea())
        return;

    uint32 key = player->GetGUIDLow();
    AnticheatData& l_PlayerData = m_Players[key];
    AnticheatReportReseter l_Reseter(l_PlayerData, FLY_HACK_DETECTION);

    bool l_CanFly = l_PlayerData.HasGmFly() ||
        l_PlayerData.IsInSpellSpline() ||
        l_PlayerData.HasCustomFlyingAura() ||
        player->HasAuraType(SPELL_AURA_FLY) ||
        player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
        player->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED);

    /// Check if it caused by a fall
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) || player->IsInWater())
        return;

    /// Check if players is above floor without MOVEMENTFLAG_FLYING
    if ((movementInfo.HasMovementFlag(MOVEMENTFLAG_FLYING) && l_CanFly) ||
        (std::abs(player->GetPositionZ() - player->GetMap()->GetHeight(player->GetPhaseMask(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()))) < 2.0f)
        return;

    if (std::abs(player->GetMap()->GetWaterLevel(player->GetPositionX(), player->GetPositionY())) < 2.0f &&
        player->HasAuraType(SPELL_AURA_WATER_WALK))
        return;

    if (l_PlayerData.GetPotentialReportCount(FLY_HACK_DETECTION) > 4)
    {
#ifdef CROSS
        l_PlayerData.AddReport(FLY_HACK_REPORT);
        if (sWorld->GetUptime() - l_PlayerData.GetLastSlackNotificationTime() > 10)
        {
            //sLog->outExtChat("#terminaporc", "danger", true, "Player GUID (low) %u (%s)(Realm: %u) triggered FlyHack Detection, report count : %u, total report count : %u",
                //key, player->GetName(), player->GetSession()->GetInterRealmNumber(), l_PlayerData.GetReportCount(FLY_HACK_REPORT), l_PlayerData.GetTotalReportCount());
            l_PlayerData.SetLastSlackNotifiactionTime(sWorld->GetUptime());
        }
#endif
        BuildReport(player, FLY_HACK_REPORT);
        return;
    }

    l_Reseter.CancelReset();
}

void AnticheatMgr::TeleportPlaneHackDetection(Player* player, MovementInfo const& movementInfo)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & TELEPORT_PLANE_HACK_DETECTION) == 0)
        return;

    uint32 key = player->GetGUIDLow();

    if (movementInfo.GetMovementFlags() != m_Players[key].GetLastMovementInfo().GetMovementFlags())
        return;

    if (player->GetMapId() == 585)
        return;

    /// Deeprun Tram
    /// Cannot handle transports correctly now
    if (player->GetMapId() == 369)
        return;

    uint32 distance2D = (uint32)movementInfo.pos.GetExactDist2d(&m_Players[key].GetLastMovementInfo().pos);
    if (distance2D > /*sWorld->getIntConfig(CONFIG_ANTICHEAT_TELEPORTHACKDETECTION_DISTANCE)*/50.f && !player->GetTeleportFlagForAnticheat())
        BuildReport(player, TELEPORT_PLANE_HACK_REPORT);

    player->SetTeleportFlagForAnticheat(false);

    /*if (m_Players[key].GetLastMovementInfo().pos.GetPositionZ() != 0 ||
        movementInfo.pos.GetPositionZ() != 0)
        return;

    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING))
        return;

    //DEAD_FALLING was deprecated
    //if (player->getDeathState() == DEAD_FALLING)
    //    return;
    float x, y, z;
    player->GetPosition(x, y, z);
    float ground_Z = player->GetMap()->GetHeight(x, y, z);
    float z_diff = fabs(ground_Z - z);

    // we are not really walking there
    if (z_diff > 1.0f)
    {
        //sLog->outError("AnticheatMgr:: Teleport To Plane - Hack detected player GUID (low) %u",player->GetGUIDLow());
        BuildReport(player,TELEPORT_PLANE_HACK_REPORT);
    }*/
}

void AnticheatMgr::TeleportHackDetection(Player* player, MovementInfo const& movementInfo)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & TELEPORT_PLANE_HACK_DETECTION) == 0)
        return;

    uint32 key = player->GetGUIDLow();

    if (player->GetMapId() == 585)
        return;

    /// Deeprun Tram
    /// Cannot handle transports correctly now
    if (player->GetMapId() == 369)
        return;

    if (movementInfo.GetMovementFlags() != m_Players[key].GetLastMovementInfo().GetMovementFlags())
        return;

    if (m_Players[key].HasGmFly() || player->isGameMaster())
        return;

    if (!player->GetTeleportFlagForAnticheat())
    {
        if (m_Players[key].CurrentMovedDelta > /*sWorld->getIntConfig(CONFIG_ANTICHEAT_TELEPORTHACKDETECTION_DISTANCE)*/50.f)
        {
            if (player->GetCharmer() && player->GetCharmer()->IsPlayer())
            {
                BuildReport(player->GetCharmer()->ToPlayer(), TELEPORT_PLANE_HACK_REPORT);
            }
            else
                BuildReport(player, TELEPORT_PLANE_HACK_REPORT);
        }
    }

    player->SetTeleportFlagForAnticheat(false);
}

void AnticheatMgr::StartHackDetection(Player* p_Player, MovementInfo const& p_MoveInfos, uint32 p_Opcode)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        return;

    if (p_Player->GetSession()->GetSecurity() > AccountTypes::SEC_PLAYER)
        return;

    if (p_Player->GetMapId() == 1712)
        return;

    uint32 key = p_Player->GetGUIDLow();

    if (p_Player->isInFlight() || p_Player->GetTransport() || p_Player->GetVehicle() || p_MoveInfos.t_guid != 0LL)
    {
        m_Players[key].SetLastMovementInfo(p_MoveInfos);

        m_Players[key].SetLastOpcode(p_Opcode);
        return;
    }

    SpeedHackDetection(p_Player,p_MoveInfos, p_Opcode); ///< Working
    FlyHackDetection(p_Player,p_MoveInfos);             ///< Working
    TeleportPlaneHackDetection(p_Player, p_MoveInfos);  ///< To test
    ClimbHackDetection(p_Player,p_MoveInfos,p_Opcode);  ///< To test
    TeleportHackDetection(p_Player, p_MoveInfos);

    m_Players[key].SetLastMovementInfo(p_MoveInfos);
    m_Players[key].SetLastOpcode(p_Opcode);

    m_Players[key].CurrentMovedDelta = p_Player->GetExactDist2d(&p_MoveInfos.pos);
}

// basic detection
void AnticheatMgr::ClimbHackDetection(Player *player, MovementInfo const& movementInfo, uint32 opcode)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & CLIMB_HACK_DETECTION) == 0)
        return;

    uint32 key = player->GetGUIDLow();

    if (opcode != CMSG_MOVE_HEARTBEAT ||
        m_Players[key].GetLastOpcode() != CMSG_MOVE_HEARTBEAT)
        return;

    // in this case we don't care if they are "legal" flags, they are handled in another parts of the Anticheat Manager.
    if (player->IsInWater() ||
        player->IsFlying() ||
        player->IsFalling())
        return;

    Position playerPos;
    player->GetPosition(&playerPos);

    float deltaZ = fabs(playerPos.GetPositionZ() - movementInfo.pos.GetPositionZ());
    float deltaXY = movementInfo.pos.GetExactDist2d(&playerPos);

    float angle = player->NormalizeOrientation(tan(deltaZ/deltaXY));

    if (angle > CLIMB_ANGLE)
    {
        //sLog->outError("AnticheatMgr:: Climb-Hack detected player GUID (low) %u", player->GetGUIDLow());
        BuildReport(player,CLIMB_HACK_REPORT);
    }
}

void AnticheatMgr::SpeedHackDetection(Player* player, MovementInfo const& movementInfo, uint32 p_Opcode)
{
    if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & SPEED_HACK_DETECTION) == 0)
        return;

    uint32 key = player->GetGUIDLow();
    AnticheatData& l_PlayerData = m_Players[key];

    if (l_PlayerData.NeedToIgnoreCheck())
        return;

    if (!(movementInfo.flags & (MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR)))
        l_PlayerData.SetInKnockback(false, movementInfo.time);

    if (l_PlayerData.IsInKnockback())
        return;

    // We also must check the map because the movementFlag can be modified by the client.
    // If we just check the flag, they could always add that flag and always skip the speed hacking detection.
    // 369 == DEEPRUN TRAM
    if (player->GetMapId() == 369)
        return;

    if (l_PlayerData.IsInSpellSpline())
        return;

    /// Don't report speed hack if player is using Fel Rush (DH Dash)
    if (player->getClass() == Classes::CLASS_DEMON_HUNTER && player->GetCastingSpell() == 195072)
        return;

    uint32 distance2D = (uint32)movementInfo.pos.GetExactDist2d(&l_PlayerData.GetLastMovementInfo().pos);
    UnitMoveType moveType = MOVE_WALK;

    // we need to know HOW is the player moving
    // TO-DO: Should we check the incoming movement flags?
    if (player->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
        moveType = MOVE_SWIM;
    else if (player->IsFlying())
        moveType = MOVE_FLIGHT;
    else if (player->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
        moveType = MOVE_WALK;
    else
        moveType = MOVE_RUN;

    // how many yards the player can do in one sec.
    l_PlayerData.SetRecentSpeedRate(moveType, player->GetSpeed(moveType), movementInfo.time, (movementInfo.flags & MOVEMENTFLAG_FALLING) ? CMSG_MOVE_JUMP : p_Opcode, player->GetSession()->GetLatency());
    uint32 speedRate = static_cast<uint32>(l_PlayerData.GetHighestRecentSpeedRate(moveType) + movementInfo.j_xyspeed);

    // how long the player took to move to here.
    uint32 timeDiff = getMSTimeDiff(l_PlayerData.GetLastMovementInfo().time, movementInfo.time);

    if (!timeDiff)
        timeDiff = 1;

    // this is the distance doable by the player in 1 sec, using the time done to move to this point.
    uint32 clientSpeedRate = distance2D * 1000 / timeDiff;

    // we did the (uint32) cast to accept a margin of tolerance
    if (clientSpeedRate > speedRate)
    {
#ifdef CROSS
        l_PlayerData.AddReport(SPEED_HACK_REPORT);
        if (l_PlayerData.GetTotalReportCount() > 10)
        {
            if (sWorld->GetUptime() - l_PlayerData.GetLastSlackNotificationTime() > 10)
            {
                // sLog->outExtChat("#terminaporc", "danger", true, "Player GUID (low) %u (%s) (Realm: %u) triggered SpeedHack Detection, speedDiff = %u, report count : %u, total report count : %u",
                    // key, player->GetName(), player->GetSession()->GetInterRealmNumber(), clientSpeedRate - speedRate, l_PlayerData.GetReportCount(SPEED_HACK_REPORT), l_PlayerData.GetTotalReportCount());
                l_PlayerData.SetLastSlackNotifiactionTime(sWorld->GetUptime());
            }
        }
#endif
        BuildReport(player, SPEED_HACK_REPORT);
        //sLog->outError("AnticheatMgr:: Speed-Hack detected player GUID (low) %u",player->GetGUIDLow());
    }
}

#ifndef __clang_analyzer__
void AnticheatMgr::StartScripts()
{
    new AnticheatScripts();
}
#endif

void AnticheatMgr::HandlePlayerLogin(Player* /*player*/)
{
    /*
    // we must delete this to prevent errors in case of crash
    CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid=%u",player->GetGUIDLow());
    // we initialize the pos of lastMovementPosition var.
    m_Players[player->GetGUIDLow()].SetPosition(player->GetPositionX(),player->GetPositionY(),player->GetPositionZ(),player->GetOrientation());
    QueryResult resultDB = CharacterDatabase.PQuery("SELECT * FROM daily_players_reports WHERE guid=%u;",player->GetGUIDLow());

    if (resultDB)
        m_Players[player->GetGUIDLow()].SetDailyReportState(true);*/
}

void AnticheatMgr::HandlePlayerLogout(Player* player)
{
    // TO-DO Make a table that stores the cheaters of the day, with more detailed information.

    // We must also delete it at logout to prevent have data of offline players in the db when we query the database (IE: The GM Command)
#ifndef CROSS
    CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid=%u",player->GetGUIDLow());
#endif

    // Delete not needed data from the memory.
    m_Players.erase(player->GetGUIDLow());
}

void AnticheatMgr::SavePlayerData(Player* player)
{
#ifndef CROSS
    CharacterDatabase.PExecute("REPLACE INTO players_reports_status (guid,average,total_reports,speed_reports,fly_reports,jump_reports,waterwalk_reports,teleportplane_reports,climb_reports,creation_time) VALUES (%u,%f,%u,%u,%u,%u,%u,%u,%u,%u);",player->GetGUIDLow(),m_Players[player->GetGUIDLow()].GetAverage(),m_Players[player->GetGUIDLow()].GetTotalReports(), m_Players[player->GetGUIDLow()].GetTypeReports(SPEED_HACK_REPORT),m_Players[player->GetGUIDLow()].GetTypeReports(FLY_HACK_REPORT),m_Players[player->GetGUIDLow()].GetTypeReports(JUMP_HACK_REPORT),m_Players[player->GetGUIDLow()].GetTypeReports(WALK_WATER_HACK_REPORT),m_Players[player->GetGUIDLow()].GetTypeReports(TELEPORT_PLANE_HACK_REPORT),m_Players[player->GetGUIDLow()].GetTypeReports(CLIMB_HACK_REPORT),m_Players[player->GetGUIDLow()].GetCreationTime());
#else
    player->GetRealmDatabase()->PExecute("REPLACE INTO players_reports_status (guid,average,total_reports,speed_reports,fly_reports,jump_reports,waterwalk_reports,teleportplane_reports,climb_reports,creation_time) VALUES (%u,%f,%u,%u,%u,%u,%u,%u,%u,%u);", player->GetRealGUIDLow(), m_Players[player->GetGUIDLow()].GetAverage(), m_Players[player->GetGUIDLow()].GetTotalReports(), m_Players[player->GetGUIDLow()].GetTypeReports(SPEED_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(FLY_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(JUMP_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(WALK_WATER_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(TELEPORT_PLANE_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(CLIMB_HACK_REPORT), m_Players[player->GetGUIDLow()].GetCreationTime());
#endif
}

uint32 AnticheatMgr::GetTotalReports(uint32 lowGUID)
{
    return m_Players[lowGUID].GetTotalReports();
}

float AnticheatMgr::GetAverage(uint32 lowGUID)
{
    return m_Players[lowGUID].GetAverage();
}

uint32 AnticheatMgr::GetTypeReports(uint32 lowGUID, uint8 type)
{
    return m_Players[lowGUID].GetTypeReports(type);
}

bool AnticheatMgr::MustCheckTempReports(uint8 type)
{
    if (type == JUMP_HACK_REPORT || type == TELEPORT_PLANE_HACK_REPORT)
        return false;

    return true;
}

std::string AnticheatMgr::ReportTypeToString(uint8 type)
{
    switch (type)
    {
        case SPEED_HACK_REPORT:
            return "SPEED HACK";
        case FLY_HACK_REPORT:
            return "FLY HACK";
        case TELEPORT_PLANE_HACK_REPORT:
             return "TELEPORT_PLANE_HACK_REPORT";
        case WALK_WATER_HACK_REPORT:
            return "WALK_WATER_HACK_REPORT";
        case JUMP_HACK_REPORT:
            return "JUMP_HACK_REPORT";
        case CLIMB_HACK_REPORT:
            return "CLIMB_HACK_REPORT";
    }

    return std::string();
}

void AnticheatMgr::BuildReport(Player* p_Mover, uint8 p_ReportType)
{
    Player * l_Player = p_Mover;
    if (l_Player->m_movedPlayer)
        l_Player = l_Player->m_movedPlayer; ///< In case of controll we have to ban our mover, not the moved player. For instance MK + Speedhack\FlyHack

    if (l_Player->GetTransport() != nullptr)
        return;

#ifndef CROSS
    uint32 l_Key = l_Player->GetGUIDLow();

    if (MustCheckTempReports(p_ReportType))
    {
        uint32 l_ActualTime = getMSTime();

        if (!m_Players[l_Key].GetTempReportsTimer(p_ReportType))
            m_Players[l_Key].SetTempReportsTimer(l_ActualTime,p_ReportType);

        if (getMSTimeDiff(m_Players[l_Key].GetTempReportsTimer(p_ReportType),l_ActualTime) < 3000)
        {
            m_Players[l_Key].SetTempReports(m_Players[l_Key].GetTempReports(p_ReportType)+1,p_ReportType);

            if (m_Players[l_Key].GetTempReports(p_ReportType) < 3)
                return;
        }
        else
        {
            m_Players[l_Key].SetTempReportsTimer(l_ActualTime,p_ReportType);
            m_Players[l_Key].SetTempReports(1,p_ReportType);
            return;
        }
    }

    // generating creationTime for average calculation
    if (!m_Players[l_Key].GetTotalReports())
        m_Players[l_Key].SetCreationTime(getMSTime());

    // increasing total_reports
    m_Players[l_Key].SetTotalReports(m_Players[l_Key].GetTotalReports()+1);

    // increasing specific cheat report
    m_Players[l_Key].SetTypeReports(p_ReportType,m_Players[l_Key].GetTypeReports(p_ReportType)+1);

    // diff time for average calculation
    uint32 l_DiffTime = getMSTimeDiff(m_Players[l_Key].GetCreationTime(),getMSTime()) / IN_MILLISECONDS;

    if (l_DiffTime > 0)
    {
        // Average == Reports per second
        float average = float(m_Players[l_Key].GetTotalReports()) / float(l_DiffTime);
        m_Players[l_Key].SetAverage(average);
    }

    //if (sWorld->getIntConfig(CONFIG_ANTICHEAT_MAX_REPORTS_FOR_DAILY_REPORT) < m_Players[key].GetTotalReports())
    {
        if (!m_Players[l_Key].GetDailyReportState())
        {
            CharacterDatabase.PExecute("REPLACE INTO daily_players_reports (guid,average,total_reports,speed_reports,fly_reports,jump_reports,waterwalk_reports,teleportplane_reports,climb_reports,creation_time) VALUES (%u,%f,%u,%u,%u,%u,%u,%u,%u,%u);",l_Player->GetGUIDLow(),m_Players[l_Player->GetGUIDLow()].GetAverage(),m_Players[l_Player->GetGUIDLow()].GetTotalReports(), m_Players[l_Player->GetGUIDLow()].GetTypeReports(SPEED_HACK_REPORT),m_Players[l_Player->GetGUIDLow()].GetTypeReports(FLY_HACK_REPORT),m_Players[l_Player->GetGUIDLow()].GetTypeReports(JUMP_HACK_REPORT),m_Players[l_Player->GetGUIDLow()].GetTypeReports(WALK_WATER_HACK_REPORT),m_Players[l_Player->GetGUIDLow()].GetTypeReports(TELEPORT_PLANE_HACK_REPORT),m_Players[l_Player->GetGUIDLow()].GetTypeReports(CLIMB_HACK_REPORT),m_Players[l_Player->GetGUIDLow()].GetCreationTime());
            m_Players[l_Key].SetDailyReportState(true);
        }
    }

    m_Players[l_Key].AddReportToHistory();

    if (m_Players[l_Key].GetReportCountInLastSecs(sWorld->getIntConfig(CONFIG_ANTICHEAT_BAN_CHECK_TIME_RANGE)) > sWorld->getIntConfig(CONFIG_ANTICHEAT_MAX_REPORTS_BEFORE_BAN)
        || (p_ReportType == TELEPORT_PLANE_HACK_REPORT && m_Players[l_Key].GetTypeReports(TELEPORT_PLANE_HACK_REPORT) > sWorld->getIntConfig(CONFIG_ANTICHEAT_REPORTS_INGAME_NOTIFICATION_TELEPORTHACKDETECTION)))
    {
        l_Player->GetSession()->KickPlayer();

        InformGMs("[ANTICHEAT]|cFF00FFFF[|cFF60FF00%s|cFF00FFFF] found cheating! %s detected!", l_Player->GetName(), ReportTypeToString(p_ReportType).c_str());

        std::string l_Msg = "Player " + std::string(l_Player->GetName()) + " was kicked by the anti cheat for reason ";
        ChatHandler(l_Player).SendGlobalFormatedGMSysMessage("%s %s", l_Msg.c_str(), ReportTypeToString(p_ReportType).c_str());
    }
#else
    if (InterRealmClient* client = l_Player->GetSession()->GetInterRealmClient())
        client->SendAnticheatReport(l_Player->GetGUIDLow(), p_ReportType);
#endif
}

void AnticheatMgr::InformGMs(const char *format, ...)
{
    va_list ap;
    char szStr[1024];
    szStr[0] = '\0';
    va_start(ap, format);
    vsnprintf(szStr, 1024, format, ap);
    va_end(ap);

    std::string str = std::string(szStr);

    size_t len = strlen(str.c_str());
    WorldPacket l_Data(SMSG_PRINT_NOTIFICATION, 2 + len);
    l_Data.WriteBits(len, 12);
    l_Data.FlushBits();
    l_Data.append(str.c_str(), len);
    sWorld->SendGlobalGMMessage(&l_Data);
}

void AnticheatMgr::AnticheatGlobalCommand(ChatHandler* p_Handler)
{
    std::list<AnticheatData> l_TopCheatersList;
    for (auto l_CheatData : m_Players)
    {
        uint32 l_TotalReports = l_CheatData.second.GetTotalReports();
        if (l_TotalReports == 0)
            continue;

        l_TopCheatersList.push_back(l_CheatData.second);
    }

    l_TopCheatersList.sort([](AnticheatData const& p_CheatData1, AnticheatData const& p_CheatData2) -> bool
    {
        return p_CheatData1.GetTotalReports() > p_CheatData2.GetTotalReports();
    });

    if (l_TopCheatersList.empty())
        return;

    p_Handler->SendSysMessage("=============================");
    p_Handler->PSendSysMessage("Players with the more reports:");

    uint32 l_Counter = 0;
    for (auto l_CheatData : l_TopCheatersList)
    {

        if (Player* l_Player = sObjectMgr->GetPlayerByLowGUID(GUID_LOPART(l_CheatData.GetLastMovementInfo().guid)))
            p_Handler->PSendSysMessage("Player: %s Total Reports: %u Average: %f", l_Player->GetName(), l_CheatData.GetTotalReports(), l_CheatData.GetAverage());

        l_Counter++;
        if (l_Counter == 3)
            break;
    }
}

void AnticheatMgr::AnticheatDeleteCommand(uint32 guid)
{
    if (!guid)
    {
        for (AnticheatPlayersDataMap::iterator it = m_Players.begin(); it != m_Players.end(); ++it)
        {
            (*it).second.SetTotalReports(0);
            (*it).second.SetAverage(0);
            (*it).second.SetCreationTime(0);
            for (uint8 i = 0; i < MAX_REPORT_TYPES; i++)
            {
                (*it).second.SetTempReports(0,i);
                (*it).second.SetTempReportsTimer(0,i);
                (*it).second.SetTypeReports(i,0);
            }
        }
#ifndef CROSS
        CharacterDatabase.PExecute("DELETE FROM players_reports_status;");
#endif
    }
    else
    {
        m_Players[guid].SetTotalReports(0);
        m_Players[guid].SetAverage(0);
        m_Players[guid].SetCreationTime(0);
        for (uint8 i = 0; i < MAX_REPORT_TYPES; i++)
        {
            m_Players[guid].SetTempReports(0,i);
            m_Players[guid].SetTempReportsTimer(0,i);
            m_Players[guid].SetTypeReports(i,0);
        }
#ifndef CROSS
        CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid=%u;",guid);
#endif
    }
}

void AnticheatMgr::ResetDailyReportStates()
{
    for (AnticheatPlayersDataMap::iterator it = m_Players.begin(); it != m_Players.end(); ++it)
        m_Players[(*it).first].SetDailyReportState(false);
}

void AnticheatMgr::HandleSplineDone(uint32 p_GUIDLow, uint32 p_Latency)
{
    if (p_Latency + m_Players[p_GUIDLow].GetLastSplineStart() < getMSTime())
        m_Players[p_GUIDLow].SetCanFlyFromSpellSpline(false);
}

void AnticheatMgr::HandleStartSpline(uint32 p_GUIDLow)
{
    m_Players[p_GUIDLow].SetCanFlyFromSpellSpline(true);
    m_Players[p_GUIDLow].SetLastSplineStart(getMSTime());
}

void AnticheatMgr::HandleAddAura(uint32 p_GUIDLow, uint32 p_SpellID)
{
    switch (p_SpellID)
    {
        case 195072: ///< Fel Rush
        case 244072: ///< Antorus' Felhounds of Sargeras 'Molten Touch'
            m_Players[p_GUIDLow].AddCustomFlyingAura(p_SpellID);
            break;
        default:
            break;
    }
}

void AnticheatMgr::HandleRemoveAura(Player* p_Player, uint32 p_SpellID)
{
    if (p_Player->HasAura(p_SpellID))
        return;

    m_Players[p_Player->GetGUIDLow()].RemoveCustomFlyingAura(p_SpellID);
}

void AnticheatMgr::HandleGmFly(uint32 p_GUIDLow, bool p_Enable)
{
    m_Players[p_GUIDLow].SetGmFly(p_Enable);
}

void AnticheatMgr::HandleTeleport(uint32 p_GUIDLow, bool p_Apply)
{
    m_Players[p_GUIDLow].SetIsTeleporting(p_Apply);
}

void AnticheatMgr::HandleStartKnockback(uint32 p_GUIDLow)
{
    m_Players[p_GUIDLow].SetInKnockback(true);
}
