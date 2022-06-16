///////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2014-2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Common.h"
#include "DB2Stores.h"
#include "WorldQuestTemplate.h"
#include <unordered_map>

namespace MS { namespace WorldQuest
{
    namespace Globals
    {
        enum : int32
        {
            UnlockQuestId               = 41694,
            UnlockQuestBit              = 24589,
            RequiredLevel               = 110,
            UpdateInterval              = 5000, ///< So the quests dont get queried one by one
            WorldQuestCriticalTime      = 15 * 60,
            BaseItemLevel               = 880,
            ItemMaxItemLevelRatedPvP    = 930
        };
    }

    struct WorldQuest
    {
        uint32 m_ID;
        Template const* m_Template;
        uint32 m_Duration;
        uint32 m_CreationTime;
    };

    using WorldQuestPtr = std::shared_ptr<WorldQuest>;
    using ActiveWorldQuestsList = ACE_Based::LockedMap<uint32, WorldQuestPtr>;

    class Generator
    {

    public:
        Generator();

        bool AddQuest(Template const* p_Entry, uint32 p_CreationTime = 0, uint32 p_Duration = 0, bool p_SendUpdateToEveryone = false, bool p_Loading = false);
        bool HasQuestActive(uint32 p_ID);
        void SendWorldStateUpdateToAll(uint32 p_WorldState, uint32 p_Value, uint32 p_QuestID = 0);
        void SendWorldQuestsUpdateToAll();
        void Update(uint32 const p_Diff);
        bool CanBePicked(uint32 p_QuestID);
        bool ExpireQuest(uint32 p_QuestID);
        int32 GetTimeRemaining(uint32 p_QuestID);
        void ModifyAllDurations(int32 l_Duration);
        void Load();
        void Reset(bool p_Morning);
        void LoadTimer();

        ActiveWorldQuestsList const& GetActiveWorldQuests();

    private:

        ActiveWorldQuestsList m_ActiveWorldQuests;
        IntervalTimer m_UpdateTimer;
        uint32 m_NextResetTime;
    };


    class Manager
    {
    public:

        struct WorldQuestReward
        {
            WorldQuestReward();

            struct ItemReward
            {
                ItemReward(uint32 p_ID, uint32 p_Count, int8 p_BonusIndex, std::vector<uint32> p_Bonuses);

                uint32 m_ID;
                uint32 m_Count;
                int8 m_ItemContext;
                std::vector<uint32> m_Bonuses;
            };

            struct CurrencyReward
            {
                CurrencyReward(uint32 p_ID, uint32 p_Count);

                uint32 m_ID;
                uint32 m_Count;
            };

            uint64 m_Money;
            std::list<ItemReward> m_ItemRewards;
            std::list<CurrencyReward> m_CurrencyRewards;

        };

        Manager(Player* p_Player);

        bool HasRewarded(uint32 p_QuestID) const;
        bool CanPickQuest(uint32 p_QuestID) const;
        void GenerateReward(uint32 p_QuestID);
        void GenerateWeeklyRatedPvpRewards();
        void Reward(uint32 p_Quest, uint32 p_ChooseReward, bool p_Remove = true);
        WorldQuestReward const* GetReward(uint32 p_Quest) const;
        bool CanBeNotifiedToClient(uint32 p_Quest) const;
        bool CanHaveWorldQuest(uint32 p_QuestID) const;
        void Expired(uint32 p_QuestID);
        void OnLogin();

        void LoadFromDB(PreparedQueryResult p_Result);
        void SaveToDB(SQLTransaction& p_Trans);
        uint8 GenerateItemContext(WorldQuestType p_Type, ItemTemplate const* p_Item, std::vector<uint32>& p_Bonuses, BattlegroundBracketType p_Bracket = BattlegroundBracketType::Max) const;

    private:
        Player* m_Player;

        using RewardList = std::unordered_map<uint32, WorldQuestReward>;
        RewardList m_RewardList;

    };
}
}

#define sWorldQuestGenerator ACE_Singleton<MS::WorldQuest::Generator, ACE_Null_Mutex>::instance()