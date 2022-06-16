////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ChallengeModePacketsWorld_h__
#define ChallengeModePacketsWorld_h__

#include "Packet.h"

namespace WorldPackets
{
    namespace ChallengeMode
    {
        struct ModeAttempt
        {
            struct Member
            {
                uint64 Guid = 0;
                uint32 VirtualRealmAddress = 0;
                uint32 NativeRealmAddress = 0;
                uint32 SpecializationID = 0;
            };

            uint32 InstanceRealmAddress = 0;
            uint32 AttemptID = 0;
            uint32 CompletionTime = 0;
            time_t CompletionDate = time(nullptr);
            uint32 MedalEarned = 0;
            std::vector<Member> Members;
        };

        struct ItemReward
        {
            uint32 ItemID = 0;
            uint32 ItemDisplayID = 0;
            uint32 Quantity = 0;
        };

        struct CurrencyReward
        {
            CurrencyReward(uint32 ID, uint32 count) : CurrencyID(ID), Quantity(count) { }

            uint32 CurrencyID = 0;
            uint32 Quantity = 0;
        };

        struct MapChallengeModeReward
        {
            struct ChallengeModeReward
            {
                std::vector<ItemReward> ItemRewards;
                uint32 Money = 0;
                std::vector<CurrencyReward> CurrencyRewards;
            };

            uint32 MapId = 0;
            std::vector<ChallengeModeReward> Rewards;
        };

        struct ChallengeModeMap
        {
            uint32 MapId = 0;
            uint32 BestCompletionMilliseconds = 0;
            uint32 LastCompletionMilliseconds = 0;
            uint32 CompletedChallengeLevel = 0;
            uint32 ChallengeID = 0;
            time_t BestMedalDate = time(nullptr);
            std::vector<uint16> BestSpecID;
            std::array<uint32, 3> Affixes;
        };

        class RequestLeaders final : public ClientPacket
        {
        public:
            RequestLeaders(WorldPacket& packet) : ClientPacket(CMSG_CHALLENGE_MODE_REQUEST_LEADERS, packet) { }

            void Read() override;

            uint32 MapId = 0;
            uint32 ChallengeID = 0;
            time_t LastGuildUpdate = time(nullptr);
            time_t LastRealmUpdate = time(nullptr);
        };

        class RequestLeadersResult final : public ServerPacket
        {
        public:
            RequestLeadersResult() : ServerPacket(SMSG_CHALLENGE_MODE_REQUEST_LEADERS_RESULT, 20 + 8) { }

            WorldPacket const* Write() override;

            uint32 MapID = 0;
            uint32 ChallengeID = 0;
            uint32 CompletitionTime = 0;
            time_t LastGuildUpdate = time(nullptr);
            time_t LastRealmUpdate = time(nullptr);
            std::vector<ModeAttempt> GuildLeaders;
            std::vector<ModeAttempt> RealmLeaders;
        };

        class AllMapStats final : public ServerPacket
        {
        public:
            AllMapStats() : ServerPacket(SMSG_CHALLENGE_MODE_ALL_MAP_STATS, 4) { }

            WorldPacket const* Write() override;

            std::vector<ChallengeModeMap> ChallengeModeMaps;
        };

        class ChallengeModeReset final : public ServerPacket
        {
        public:
            ChallengeModeReset(uint32 mapID) : ServerPacket(SMSG_CHALLENGE_MODE_RESET, 4), MapID(mapID) { }

            WorldPacket const* Write() override;

           uint32 MapID = 0;
        };

        class ChallengeModeStart final : public ServerPacket
        {
        public:
            ChallengeModeStart() : ServerPacket(SMSG_CHALLENGE_MODE_START, 23) { }

            WorldPacket const* Write() override;

            std::array<uint32, 3> Affixes;
            uint32 MapID = 0;
            int32 ChallengeID = 0;
            uint32 StartedChallengeLevel = 2;
            bool IsKeyCharged = false;
        };

        class ChallengeModeComplete final : public ServerPacket
        {
        public:
            ChallengeModeComplete() : ServerPacket(SMSG_CHALLENGE_MODE_COMPLETE, 17) { }

            WorldPacket const* Write() override;

            uint32 MapID = 0;
            int32 CompletionMilliseconds = 0;
            int32 RewardLevel = 0;
            uint32 ChallengeID = 0;
            bool IsCompletedInTimer = false;
        };

        class ChallengeModeNewPlayerRecord final : public ServerPacket
        {
        public:
            ChallengeModeNewPlayerRecord() : ServerPacket(SMSG_CHALLENGE_MODE_NEW_PLAYER_RECORD, 12) { }

            WorldPacket const* Write() override;

            uint32 MapID = 0;
            int32 CompletionMilliseconds = 0;
            int32 StartedChallengeLevel = 0;
        };

        class ChallengeModeMapStatsUpdate final : public ServerPacket
        {
        public:
            ChallengeModeMapStatsUpdate() : ServerPacket(SMSG_CHALLENGE_MODE_MAP_STATS_UPDATE, 12) { }

            WorldPacket const* Write() override;

            ChallengeModeMap Stats;
        };

        class ChangePlayerDifficultyResult final : public ServerPacket
        {
        public:
            ChangePlayerDifficultyResult() : ServerPacket(SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT, 1 + 1 + 4 + 4 + 4 + 4 + 16) { }

            WorldPacket const* Write() override;

            uint64 Guid = 0;
            uint32 CooldownReason = 0;
            uint32 InstanceMapID = 0;
            uint32 DifficultyRecID = 0;
            uint32 MapID = 0;
            uint8 Result = 0;
            bool Cooldown = false;
        };

        class StartChallengeMode final : public ClientPacket
        {
        public:
            StartChallengeMode(WorldPacket& packet) : ClientPacket(CMSG_START_CHALLENGE_MODE, packet) { }

            void Read() override;

            uint64 GameObjectGUID = 0;
            uint32 UnkInt = 0;
            bool IsKeyCharged = false;
        };

        class ResetChallengeMode final : public ClientPacket
        {
        public:
            ResetChallengeMode(WorldPacket& packet) : ClientPacket(CMSG_RESET_CHALLENGE_MODE, packet) { }

            void Read() override { }
        };

        class ChallengeModeUpdateDeathCount final : public ServerPacket
        {
        public:
            ChallengeModeUpdateDeathCount() : ServerPacket(SMSG_CHALLENGE_MODE_UPDATE_DEATH_COUNT, 4) { }

            WorldPacket const* Write() override;

            uint32 DeathCount = 0;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::ChallengeMode::ModeAttempt const& modeAttempt);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::ChallengeMode::ItemReward const& itemReward);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::ChallengeMode::MapChallengeModeReward const& mapChallengeModeReward);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::ChallengeMode::ChallengeModeMap const& challengeModeMap);

#endif ///< ChallengeModePacketsWorld_h__
