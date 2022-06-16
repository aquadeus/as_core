////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SC_ACDATA_H
#define SC_ACDATA_H

#include "AnticheatMgr.h"

#define MAX_REPORT_TYPES 6
#define SPEED_SAVE_TIME 500

class AnticheatData
{
    public:
        AnticheatData();
        ~AnticheatData();

        void SetLastOpcode(uint32 opcode);
        uint32 GetLastOpcode() const;

        const MovementInfo& GetLastMovementInfo() const;
        void SetLastMovementInfo(MovementInfo const& moveInfo);

        void SetPosition(float x, float y, float z, float o);

        /*
        bool GetDisableACCheck() const;
        void SetDisableACCheck(bool check);

        uint32 GetDisableACTimer() const;
        void SetDisableACTimer(uint32 timer);*/

        uint32 GetTotalReports() const;
        void SetTotalReports(uint32 _totalReports);

        uint32 GetTypeReports(uint32 type) const;
        void SetTypeReports(uint32 type, uint32 amount);

        float GetAverage() const;
        void SetAverage(float _average);

        uint32 GetCreationTime() const;
        void SetCreationTime(uint32 creationTime);

        void SetTempReports(uint32 amount, uint8 type);
        uint32 GetTempReports(uint8 type);

        void SetTempReportsTimer(uint32 time, uint8 type);
        uint32 GetTempReportsTimer(uint8 type);

        void SetDailyReportState(bool b);
        bool GetDailyReportState();

        void AddReportToHistory();
        uint32 GetReportCountInLastSecs(uint32 p_Secondes);

        uint32 GetLastSplineStart() const;
        void SetLastSplineStart(uint32 p_Time);

        void SetCanFlyFromSpellSpline(bool p_Apply);
        bool IsInSpellSpline() const;

        void AddCustomFlyingAura(uint32 p_SpellID);
        void RemoveCustomFlyingAura(uint32 p_SpellID);
        bool HasCustomFlyingAura() const;

        void SetGmFly(bool p_Enable);
        bool HasGmFly() const;

        void SetInKnockback(bool p_Apply, int32 p_Time = 0);
        bool IsInKnockback() const;

        uint32 GetLastSlackNotificationTime() const;
        void SetLastSlackNotifiactionTime(uint32 p_Time);

        void ResetPotentialReport(uint8 p_DetectionType);
        void AddPotentialReport(uint8 p_DetectionType);
        uint32 GetPotentialReportCount(uint8 p_DetectionType) const;

        /// SetRecentSpeedRate must be called just before GetHighestRecentSpeedRate in order to work properly
        void SetRecentSpeedRate(UnitMoveType p_MoveType, float p_Speed, uint32 p_Time, uint32 p_Opcode, uint32 p_Latency);
        void ClearOldSpeedRecords(UnitMoveType p_MoveType, uint32 p_Time, float p_Speed, uint32 p_Latency);
        uint32 GetHighestRecentSpeedRate(UnitMoveType p_MoveType) const;

        void SetIsTeleporting(bool p_Apply);
        bool NeedToIgnoreCheck();

        void AddReport(uint8 p_Type);
        uint32 GetReportCount(uint8 p_Type) const;
        uint32 GetTotalReportCount() const;

        float CurrentMovedDelta;

    private:
        uint32 lastOpcode;
        MovementInfo lastMovementInfo;
        //bool disableACCheck;
        //uint32 disableACCheckTimer;
        uint32 totalReports;
        uint32 typeReports[MAX_REPORT_TYPES];
        float average;
        uint32 creationTime;
        uint32 tempReports[MAX_REPORT_TYPES];
        uint32 tempReportsTimer[MAX_REPORT_TYPES];
        bool hasDailyReport;

        uint32 m_LastSplineStart;
        bool m_CanFlyFromSpellSpline;
        std::set<uint32> m_CustomFlyingAuras;
        bool m_GmFly;
        uint32 m_JumpStartTime;
        bool m_ResetJumpStart;
        bool m_IsInKnockback;
        int32 m_KnockbackEnd;
        uint32 m_LastSlackNotification;
        uint32 m_PotentialReports[MAX_REPORT_TYPES];
        std::set<std::pair<float, uint32>> m_RecentSpeedRates[MAX_MOVE_TYPE];
        bool m_IsTeleporting;
        uint8 m_IgnorePacketCount;
        uint32 m_ReportCounts[MAX_REPORT_TYPES];

        std::list<time_t> m_ReportsHistory;
};

class AnticheatReportReseter
{
    public:
        AnticheatReportReseter(AnticheatData& p_OwnerData, uint8 p_DetectionType) :
            m_OwnerData(&p_OwnerData), m_DetectionType(p_DetectionType), m_Reset(true) { }

        ~AnticheatReportReseter()
        {
            if (!m_OwnerData)
                return;

            if (m_Reset)
                m_OwnerData->ResetPotentialReport(m_DetectionType);
            else
                m_OwnerData->AddPotentialReport(m_DetectionType);
        }

        void CancelReset() { m_Reset = false; }

    private:
        bool m_Reset;
        uint8 m_DetectionType;
        AnticheatData* m_OwnerData;
};

#endif
