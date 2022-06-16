////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "AnticheatData.h"

AnticheatData::AnticheatData()
{
    lastOpcode = 0;
    totalReports = 0;
    for (uint8 i = 0; i < MAX_REPORT_TYPES; i++)
    {
        typeReports[i] = 0;
        tempReports[i] = 0;
        tempReportsTimer[i] = 0;
        m_ReportCounts[i] = 0;
    }
    average = 0;
    creationTime = 0;
    hasDailyReport = false;

    m_LastSplineStart = 0;
    m_CanFlyFromSpellSpline = false;
    m_LastSlackNotification = 0;
    m_JumpStartTime = 0;
    m_ResetJumpStart = false;
    m_IsInKnockback = false;
    m_KnockbackEnd = 0;

    CurrentMovedDelta = 0.0f;
}

AnticheatData::~AnticheatData()
{
}

void AnticheatData::SetDailyReportState(bool b)
{
    hasDailyReport = b;
}

bool AnticheatData::GetDailyReportState()
{
    return hasDailyReport;
}

void AnticheatData::SetLastOpcode(uint32 opcode)
{
    lastOpcode = opcode;
}

void AnticheatData::SetPosition(float x, float y, float z, float o)
{
    lastMovementInfo.pos.m_positionX = x;
    lastMovementInfo.pos.m_positionY = y;
    lastMovementInfo.pos.m_positionZ = z;
    lastMovementInfo.pos.m_orientation = o;
}

uint32 AnticheatData::GetLastOpcode() const
{
    return lastOpcode;
}

const MovementInfo& AnticheatData::GetLastMovementInfo() const
{
    return lastMovementInfo;
}

void AnticheatData::SetLastMovementInfo(MovementInfo const& moveInfo)
{
    lastMovementInfo = moveInfo;
}

uint32 AnticheatData::GetTotalReports() const
{
    return totalReports;
}

void AnticheatData::SetTotalReports(uint32 _totalReports)
{
    totalReports = _totalReports;
}

void AnticheatData::SetTypeReports(uint32 type, uint32 amount)
{
    typeReports[type] = amount;
}

uint32 AnticheatData::GetTypeReports(uint32 type) const
{
    return typeReports[type];
}

float AnticheatData::GetAverage() const
{
    return average;
}

void AnticheatData::SetAverage(float _average)
{
    average = _average;
}

uint32 AnticheatData::GetCreationTime() const
{
    return creationTime;
}

void AnticheatData::SetCreationTime(uint32 _creationTime)
{
    creationTime = _creationTime;
}

void AnticheatData::SetTempReports(uint32 amount, uint8 type)
{
    tempReports[type] = amount;
}

uint32 AnticheatData::GetTempReports(uint8 type)
{
    return tempReports[type];
}

void AnticheatData::SetTempReportsTimer(uint32 time, uint8 type)
{
    tempReportsTimer[type] = time;
}

uint32 AnticheatData::GetTempReportsTimer(uint8 type)
{
    return tempReportsTimer[type];
}

void AnticheatData::AddReportToHistory()
{
    m_ReportsHistory.push_back(time(nullptr));
}

uint32 AnticheatData::GetReportCountInLastSecs(uint32 p_Secondes)
{
    time_t l_Now = time(nullptr);

    return (uint32)std::count_if(m_ReportsHistory.begin(), m_ReportsHistory.end(), [l_Now, p_Secondes](time_t const& p_Timestamp) -> bool
    {
        if ((l_Now - p_Secondes) <= p_Timestamp)
            return true;

        return false;
    });
}

uint32 AnticheatData::GetLastSplineStart() const
{
    return m_LastSplineStart;
}

void AnticheatData::SetLastSplineStart(uint32 p_Time)
{
    m_LastSplineStart = p_Time;
}

void AnticheatData::SetCanFlyFromSpellSpline(bool p_Apply)
{
    m_CanFlyFromSpellSpline = p_Apply;
}

bool AnticheatData::IsInSpellSpline() const
{
    return m_CanFlyFromSpellSpline;
}

void AnticheatData::AddCustomFlyingAura(uint32 p_SpellID)
{
    m_CustomFlyingAuras.insert(p_SpellID);
}

void AnticheatData::RemoveCustomFlyingAura(uint32 p_SpellID)
{
    m_CustomFlyingAuras.erase(p_SpellID);
}

bool AnticheatData::HasCustomFlyingAura() const
{
    return !m_CustomFlyingAuras.empty();
}

void AnticheatData::SetGmFly(bool p_Enable)
{
    m_GmFly = p_Enable;
}

bool AnticheatData::HasGmFly() const
{
    return m_GmFly;
}

uint32 AnticheatData::GetLastSlackNotificationTime() const
{
    return m_LastSlackNotification;
}

void AnticheatData::SetLastSlackNotifiactionTime(uint32 p_Time)
{
    m_LastSlackNotification = p_Time;
}

void AnticheatData::ResetPotentialReport(uint8 p_DetectionType)
{
    m_PotentialReports[p_DetectionType] = 0;
}

void AnticheatData::AddPotentialReport(uint8 p_DetectionType)
{
    ++m_PotentialReports[p_DetectionType];
}

uint32 AnticheatData::GetPotentialReportCount(uint8 p_DetectionType) const
{
    return m_PotentialReports[p_DetectionType];
}

void AnticheatData::SetRecentSpeedRate(UnitMoveType p_MoveType, float p_Speed, uint32 p_Time, uint32 p_Opcode, uint32 p_Latency)
{
    if (m_ResetJumpStart)
    {
        m_ResetJumpStart = false;
        m_JumpStartTime = 0;
    }

    switch (p_Opcode)
    {
        case CMSG_MOVE_JUMP:
        {
            if (!m_JumpStartTime)
                m_JumpStartTime = p_Time;

            break;
        }
        case CMSG_MOVE_FALL_LAND:
        {
            m_ResetJumpStart = true;
            break;
        }
        default:
            break;
    }

    ClearOldSpeedRecords(p_MoveType, p_Time, p_Speed, p_Latency);

    m_RecentSpeedRates[p_MoveType].insert(std::map<float, time_t>::value_type(p_Speed, p_Time));
}

void AnticheatData::ClearOldSpeedRecords(UnitMoveType p_MoveType, uint32 p_Time, float p_Speed, uint32 p_Latency)
{
    for (auto l_Itr = m_RecentSpeedRates[p_MoveType].begin(); l_Itr != m_RecentSpeedRates[p_MoveType].end();)
    {
        if (((!m_JumpStartTime || l_Itr->second < m_JumpStartTime) && (p_Time - l_Itr->second > (SPEED_SAVE_TIME + p_Latency))) ||
            (p_Speed && p_Speed >= l_Itr->first))
            l_Itr = m_RecentSpeedRates[p_MoveType].erase(l_Itr);
        else
            ++l_Itr;
    }
}

uint32 AnticheatData::GetHighestRecentSpeedRate(UnitMoveType p_MoveType) const
{
    uint32 l_MaxSpeed = 0;
    for (auto const& l_Pair : m_RecentSpeedRates[p_MoveType])
    {
        if (l_Pair.first > l_MaxSpeed)
            l_MaxSpeed = l_Pair.first;
    }

    return l_MaxSpeed;
}

void AnticheatData::SetIsTeleporting(bool p_Apply)
{
    m_IsTeleporting = p_Apply;

    if (!m_IsTeleporting)
        m_IgnorePacketCount = 2;
}

bool AnticheatData::NeedToIgnoreCheck()
{
    if (m_IsTeleporting)
        return true;

    if (m_IgnorePacketCount)
    {
        m_IgnorePacketCount--;
        return true;
    }

    return false;
}

void AnticheatData::AddReport(uint8 p_Type)
{
    m_ReportCounts[p_Type]++;
}

uint32 AnticheatData::GetReportCount(uint8 p_Type) const
{
    return m_ReportCounts[p_Type];
}

uint32 AnticheatData::GetTotalReportCount() const
{
    uint32 l_Count = 0;
    for (uint32 l_I = 0; l_I < MAX_REPORT_TYPES; ++l_I)
        l_Count += m_ReportCounts[l_I];

    return l_Count;
}

void AnticheatData::SetInKnockback(bool p_Apply, int32 p_Time /*= 0*/)
{
    if (p_Apply)
    {
        m_IsInKnockback = true;
        m_KnockbackEnd = 0;
        return;
    }
    else if (!m_IsInKnockback)
        return;

    if (!m_KnockbackEnd)
    {
        m_KnockbackEnd = p_Time + 500;
        return;
    }

    if (p_Time - m_KnockbackEnd > 0)
    {
        m_IsInKnockback = false;
        m_KnockbackEnd = 0;
    }
}

bool AnticheatData::IsInKnockback() const
{
    return m_IsInKnockback;
}
