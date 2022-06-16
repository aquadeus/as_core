////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_CHALLENGEMGR_H
#define TRINITY_CHALLENGEMGR_H

struct ChallengeMember
{
    uint64 Guid;
    uint16 SpecId;
    uint32 Date;                    ///< time when recorde done
    uint32 ChallengeLevel;          ///< 2-15
    uint32 ChestID;
    uint32 GuildId;

    bool operator < (const ChallengeMember& i) const { return Guid > i.Guid; }
    bool operator == (const ChallengeMember& i) const { return Guid == i.Guid; }
};

typedef std::set<ChallengeMember> ChallengeMemberList;

struct ChallengeData
{
    std::array<uint32, 3> Affixes;  ///< key modifiers
    uint32 GuildID;                 ///< is it guild group
    uint32 ID;                      ///< challenge id
    uint32 RecordTime;              ///< time taken for complite challenge
    uint32 Date;                    ///< time when recorde done
    uint32 ChallengeLevel;          ///< 2-15
    uint16 MapID;
    uint32 ChallengeID;
    uint8  TimerLevel;              ///< like 0 - not in time; 1-2-3 'timer deal' by db2 data 1-2-3 chests
    uint32 ChestID;

    ChallengeMemberList member;
};

struct OploteLoot
{
    uint64 Guid;
    uint32 Date;
    uint32 ChallengeLevel;
    uint64 ChestGUID = 0;
    bool NeedSave = true;
};

typedef std::unordered_map<uint16 /*p_ChallengeID*/, ChallengeData*> ChallengeByMap;
typedef std::unordered_map<uint32 /*ID*/, ChallengeData*> ChallengeMap;
typedef std::unordered_map<uint64 /*MemberGUID*/, ChallengeByMap> ChallengesOfMember;
typedef std::unordered_map<uint32 /*guild*/, ChallengeByMap> GuildBestRecord;
typedef std::unordered_map<uint64 /*MemberGUID*/, std::set<ChallengeData*>> ChallengeWeekListMap;
typedef std::unordered_map<uint64 /*MemberGUID*/, OploteLoot> OploteLootMap;

class ChallengeMgr
{
    ChallengeMgr();
    ~ChallengeMgr();

    public:
        static ChallengeMgr* instance()
        {
            static ChallengeMgr instance;
            return &instance;
        }

        void LoadFromDB();
        void SaveChallengeToDB(ChallengeData const* p_ChallengeData);

        void CheckBestMapId(ChallengeData* p_ChallengeData);
        void CheckBestGuildMapId(ChallengeData* p_ChallengeData);
        bool CheckBestMemberMapId(uint64 const& p_Guid, ChallengeData* p_ChallengeData);

        ChallengeData* BestServerChallenge(uint32 p_ChallengeID);
        ChallengeData* BestGuildChallenge(uint32 const& p_GuildId, uint32 p_ChallengeID);
        void SetChallengeMapData(uint32 const& p_ID, ChallengeData* p_Data) { m_ChallengeMap[p_ID] = p_Data; }
        ChallengeByMap* BestForMember(uint64 const& p_Guid);
        ChallengeByMap* LastForMember(uint64 const& p_Guid);
        ChallengeData* LastForMemberMap(uint64 const& p_Guid, uint32 p_ChallengeID);
        bool HasOploteLoot(uint64 const& p_Guid);
        OploteLoot* GetOploteLoot(uint64 const& p_Guid);
        void SaveOploteLootToDB();
        void DeleteOploteLoot(uint64 const& p_Guid, bool p_OnlyFromDatabase);
        void GenerateOploteLoot(uint64 const& p_Guid, uint32 p_ChallengeLevel);
        void GenerateOploteLoot();
        bool HasRecordThisWeek(uint64 p_Guid);
        bool HasRecordLastWeek(uint64 p_Guid);
        bool GetStartPosition(uint32 p_MapChallengeID, float& p_PositionX, float& p_PositionY, float& p_PositionZ, float& p_Orientation);

        void GenerateCurrentWeekAffixes();
        std::array<uint32, 3> GetCurrentAffixes() const { return m_CurrentWeekAffixes; }

    protected:
        ChallengeMap m_ChallengeMap;
        ChallengesOfMember m_LastForMember;
        ChallengesOfMember m_ChallengesOfMember;
        ChallengeByMap m_BestForMap;
        GuildBestRecord m_GuildBest;
        std::array<uint32, 3> m_CurrentWeekAffixes;
        ChallengeWeekListMap m_ChallengeWeekList;
        std::set<uint32> m_ChallengersOfLastWeek;
        OploteLootMap m_OploteWeekLoot;
};

#define sChallengeMgr ChallengeMgr::instance()

#endif
