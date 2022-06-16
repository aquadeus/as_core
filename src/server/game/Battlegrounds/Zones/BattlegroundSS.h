////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __BATTLEGROUNDSS_H
#define __BATTLEGROUNDSS_H

#include "Battleground.h"
#include "OutdoorPvP.h"
#include "CreatureTextMgr.h"
#include "HelperDefines.h"

enum eShoreBattlegroundScores
{
    MaxTeamScore                = 1500,
    AzeriteRespawnTime          = 120000,
    GunshipDropTime             = 300000
};

enum eShoreGameObjectTypes
{
    AllianceCollisionWallType               = 9,
    HordeCollisionWallType                  = 10,
    AllianceGunshipType                     = 11,
    HordeGunshipType                        = 12,

    GameObjectTypeMax                       = 13
};

enum eShoreGameObjects
{
    /// Azerite Nodes
    AzeriteNode                             = 281307,
    RichAzeriteNode                         = 272471,

    /// Collisions
    AllianceCollisionWall                   = 281224, ///< 1
    HordeCollisionWall                      = 281226, ///< 0

    /// Gunships
    HordeGunship                            = 279254,
    AllianceGunship                         = 278407
};

enum eShoreCreatureTypes
{
    ///< REORDER ME
    MasterMathiasType = 10,
    NathanosType,
    CreatureTypeMax
};

enum eShoreCreatures
{
    AzeriteFissure      = 125253,
    MasterMathias       = 130532,
    Nathanos            = 131773
};

enum eShoreWorldStates
{
    MaxAzeriteWorldState    = 13846,
    AzeriteDisplayAlliance  = 13845,
    AzeriteDisplayHorde     = 13844
};

enum eShoreAchievements
{
    AGoodStart          = 12406,
    BloodAndSand        = 12411,
    ClaimJumper         = 12404,
    DeathFromAbove      = 12405,
    Domination          = 12408,
    Victory             = 12409,
    Perfection          = 12407,
    Veteran             = 12410,
    Master              = 12412
};

enum eShoreSpells
{
    AchievementCredit   = 261968,
    Parachute           = 252766,
    UnchartedTerritory  = 262086,
    RocketParachute1    = 262359,
    RocketParachute2    = 250917,
    RocketParachute3    = 250921,
    AzeriteKnockBack    = 262385
};

enum eShoreVignettes
{
    Fissure             = 2408,
    Azerite             = 2405,
    CrashSite           = 2606,
    Waterfall           = 2607,
    Ruins               = 2608,
    Overlook            = 2609,
    Plunge              = 2610,
    Tower               = 2611,
    TidePools           = 2612,
    Temple              = 2613,
    SoonAzerite         = 2614,
    Ridge               = 2615,
    TarPits             = 2616
};

enum eShoreStrings
{
    FissureAppeared     = 1376
};

class BattlegroundSSScore : public BattlegroundScore
{
    public:
        BattlegroundSSScore() : MinedAzerite(0) {};
        virtual ~BattlegroundSSScore() {};
        uint32 MinedAzerite;
};

class BattlegroundSS : public Battleground
{
    public:
        BattlegroundSS();
        ~BattlegroundSS();

        virtual void PostUpdateImpl(uint32 p_Diff) override;
        virtual void FillInitialWorldStates(ByteBuffer& p_Data) override;
        void UpdatePlayerScore(Player* p_Source, Player* p_Victim, uint32 p_Type, uint32 p_Value, bool p_DoAddHonor = true, MS::Battlegrounds::RewardCurrencyType::Type p_RewardType = MS::Battlegrounds::RewardCurrencyType::Type::None) override;
        virtual void EventPlayerClickedOnFlag(Player* p_Player, GameObject* p_Gameobject) override;

        //virtual void StartingEventOpenDoors() override;
        /*virtual void AddPlayer(Player* p_Player) override;
        virtual void StartingEventCloseDoors() override;

        void RemovePlayer(Player* p_Player, uint64 p_GUID, uint32 p_Team) override;
        void EndBattleground(uint32 p_Winner);
        bool SetupBattleground() override;

        virtual void FillInitialWorldStates(ByteBuffer& p_Data) override;*/

        virtual void StartingEventOpenDoors() override;
        virtual WorldSafeLocsEntry const* GetClosestGraveYard(Player* p_Player) override;
        virtual void RepopPlayerAtGraveyard(uint64 p_GUID) override;
        bool SetupBattleground() override;
        void StartBattleground();
        void Reset() override;
        void EndBattleground(uint32 p_Winner);
        void UpdateTeamScore(Player* p_Player, uint32 p_Points);
        uint32 GetTeamScore(uint32 p_TeamID) const override { return m_TeamScores[GetTeamIndexByTeamId(p_TeamID)]; }
        uint32 GetMaxScore() const override { return eShoreBattlegroundScores::MaxTeamScore; }
        bool IsScoreIncremental() const override { return true; }
        void HandleAchievementProgress(Player* p_Player, uint32 p_Points);

        void AddPlayer(Player* player) override;
        void HandleKillPlayer(Player* p_Killed, Player* p_Killer) override;
        void HandlePlayerResurrect(Player* player) override;
        void RemovePlayer(Player* player, uint64 guid, uint32 team) override;

    private:
        int32 m_LastAzeriteTimer;
        int32 m_FallCheckTimer;
        int32 m_FallDownCheckTimer;
        bool m_Started;
        bool m_FissureSpawned;
        bool m_AzeriteNodesSpawned;
        bool m_FirstThirtySeconds;
        bool m_ObjectsAttached;
        bool m_FissuredQueued;
        bool m_Locked;
        bool m_FirstTime;
        int32 m_FissureCheckTimer;
        int32 m_CheatersCheckTimer;
        std::map<uint32, Vignette::Entity*> m_ActiveVignettes;
        std::set<uint64> m_FallingPlayers;
        std::set<uint64> m_LandedPlayers;
        uint64 m_GunshipHordeGUID;
        uint64 m_GunshipAllianceGUID;

        std::map<uint32, Position> g_AzeriteFissureSpawns =
        {
            { 0, { 1243.377f, 2721.179f, 11.884f, 0.753f } },
            { 1, { 1340.854f, 2785.177f, 2.5710f, 4.169f } },
            { 2, { 1442.257f, 2698.792f, 9.8216f, 2.743f } },
            { 3, { 1454.260f, 2598.627f, 15.195f, 4.719f } },
            { 4, { 1259.397f, 2572.349f, 8.6337f, 0.839f } }, ///< Feralas
            { 5, { 1461.326f, 2823.139f, 31.659f, 3.733f } },
            { 6, { 1346.963f, 2920.539f, 33.204f, 4.035f } },
            { 7, { 1113.932f, 2887.386f, 38.455f, 4.365f } },
            { 8, { 1127.821f, 2781.984f, 30.829f, 5.881f } }
        };

        std::map<uint8, Position> g_SpiritGuardSpawns =
        {
            { TEAM_ALLIANCE, { 0.0f, 0.0f, 0.0f, 0.0f } },
            { TEAM_HORDE,    { 0.0f, 0.0f, 0.0f, 0.0f } }
        };

        std::map<uint8, Position> g_GunshipPositions =
        {
            { TEAM_ALLIANCE, { 1347.993f, 2598.253f, 298.7958f, 0.226616f } },
            { TEAM_HORDE, { 1247.917f, 2874.73f, 244.8418f, 3.565888f } }
        };

        void SpawnAzeriteFissures(int8 p_Amount);
        void ConvertFissures();
        bool IsAzeriteSpawned();
        bool IsFissureSpawned();
        void UpdateVignettes();
        void SetOnGunship(Player* p_Player);
        uint8 GetActiveNodesAndFissures();
};

#endif