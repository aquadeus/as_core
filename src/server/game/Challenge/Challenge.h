////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "Common.h"
#include "Scenario.h"
#include "InstanceScript.h"

struct ChallengeEntry;

enum ChallengeSpells : uint32
{
    ChallengersMight              = 206150, ///< generic creature aura
    ChallengersBurden             = 206151, ///< generic player aura
    ChallengerBolstering          = 209859,
    ChallengerNecrotic            = 209858,
    ChallengerOverflowing         = 221772,
    ChallengerSanguine            = 226489,
    ChallengerRaging              = 228318,
    ChallengerSummonVolcanicPlume = 209861,
    ChallengerVolcanicPlume       = 209862,
    ChallengerBurst               = 240443,
    ChallengerGrievousWound       = 240559,
    ChallengerQuake               = 240447
};

enum ChallengeNpcs : uint32
{
    NpcVolcanicPlume = 105877,
};

enum MiscChallengeData : uint32
{
    ChallengeDelayTimer = 10,
};

class Challenge : public InstanceScript
{
    public:
        Challenge(Map* p_Map, Player* p_Player, uint32 p_InstanceID, uint32 p_MapChallengeID);
        ~Challenge();

        void OnPlayerEnterForScript(Player* p_Player);
        void OnPlayerLeaveForScript(Player* p_Player);
        void OnPlayerDiesForScript(Player* p_Player);
        void OnCreatureCreateForScript(Creature* p_Creature);
        void OnCreatureRemoveForScript(Creature* p_Creature);
        void OnCreatureUpdateDifficulty(Creature* p_Creature);
        void EnterCombatForScript(Creature* p_Creature, Unit* p_Enemy);
        void CreatureDiesForScript(Creature* p_Creature, Unit* p_Killer);
        void OnGameObjectCreateForScript(GameObject* p_Go) {}
        void OnGameObjectRemoveForScript(GameObject* p_Go) {}

        void Update(uint32 p_Diff);

        void ModifyKeystoneLevel(uint8 p_Level);
        void Start();
        void Complete();

        void BroadcastPacket(WorldPacket const* p_Data) const;

        void HitTimer();

        uint32 GetChallengeLevel() const { return m_ChallengeLevel; }
        std::array<uint32, 3> GetAffixes() const { return m_Affixes; }
        bool HasAffix(Affixes affix) { return m_AffixesTest.test(size_t(affix));}

        uint32 GetChallengeTimerToNow() const { return (getMSTime() - m_ChallengeTimer) / IN_MILLISECONDS; }
        void ModChallengeTimer(uint32 p_Timer);
        uint32 GetChallengeProgresTime();

        Player* GetKeystoneOwner() const { return ObjectAccessor::FindPlayer(m_ownerGuid); }
        Item* GetKeystone() const { return GetKeystoneOwner() ? GetKeystoneOwner()->GetItemByGuid(m_itemGuid) : nullptr; }

        void ResetGo();
        void SendStartTimer(Player* p_Player = nullptr);
        void SendStartElapsedTimer(Player* p_Player = nullptr);
        void SendChallengeModeStart(Player* p_Player = nullptr);
        void SendChallengeModeNewPlayerRecord(Player* p_Player);
        void SendChallengeModeMapStatsUpdate(Player* p_Player);
        void SummonWall(Player* p_Player);

        void SetInstanceScript(InstanceScript* p_InstanceScript) { m_InstanceScript = p_InstanceScript; }
        InstanceScript* GetInstanceScript() const { return m_InstanceScript; }

        bool IsLuckyChallengerForChest(uint64 p_PlayerGuid, uint32 p_ChestEntry) { return m_LuckyChallengersChest[p_ChestEntry].count(p_PlayerGuid); }
        uint8 GetAssignedLootAmount(uint64 p_PlayerGuid) { return m_AssignedLoot[p_PlayerGuid]; }

        void SetWorldStates(bool p_Enable);
        void SetWorldStatesForPlayer(bool p_Enable, Player* p_Player);
        void IncreaseDeathCounter() { ++m_deathCounter; }
        uint32 GetDeathCounter() { return m_deathCounter; }
        void SendDeathCount(Player* player = nullptr);

        inline bool IsComplete() const { return m_Complete; }
        inline bool IsRunning() const { return m_Run; }

        uint8 GetRewardLevel() const { return m_RewardLevel; }

        GuidUnorderedSet m_Challengers;
        bool m_CanRun;
        bool m_Run;
        bool m_Complete;

        uint64 m_ownerGuid;
        uint64 m_itemGuid;

        uint32 m_ChallengeTimer;
        uint32 m_QuakeTimer;

        FunctionProcessor m_Functions;

    private:
        std::unordered_map<uint32, std::set<uint64>> m_LuckyChallengersChest;    ///< Challengers that will have 100% chance to get a gear loot in the chest
        std::map<uint64, uint8> m_AssignedLoot;
        uint64 m_Creator;
        std::array<uint32, 3> m_Affixes;
        std::bitset<size_t(Affixes::MaxAffixes)> m_AffixesTest;
        uint16 m_ChestTimers[3];
        Item* m_Item;
        Map* m_Map;
        InstanceScript* m_InstanceScript;
        MapChallengeModeEntry const* m_ChallengeEntry;
        uint32 m_ChallengeLevel;
        uint32 m_InstanceID;
        uint32 m_MapID;
        uint8 m_RewardLevel;
        bool m_OutOfTime;
        uint32 m_deathCounter;
};

#endif