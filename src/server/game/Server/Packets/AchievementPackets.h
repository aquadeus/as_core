////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Packet.h"

namespace WorldPackets
{
    namespace Achievement
    {
        struct EarnedAchievement
        {
            uint32 Id = 0;
            time_t Date = time_t(0);
            uint64 Owner;
            uint32 VirtualRealmAddress = 0;
            uint32 NativeRealmAddress = 0;
        };

        struct CriteriaProgress
        {
            uint32 Id = 0;
            uint64 Quantity = 0;
            uint64 Player;
            uint32 Flags = 0;
            time_t Date = time_t(0);
            uint32 TimeFromStart = 0;
            uint32 TimeFromCreate = 0;
        };

        struct AllAchievements
        {
            std::vector<EarnedAchievement> Earned;
            std::vector<CriteriaProgress> Progress;
        };

        class AllAchievementData final : public ServerPacket
        {
        public:
            AllAchievementData() : ServerPacket(SMSG_ALL_ACHIEVEMENT_DATA) { }

            WorldPacket* Write() override;

            AllAchievements Data;
        };

        class AllAccountCriteria final : public ServerPacket
        {
        public:
            AllAccountCriteria() : ServerPacket(SMSG_ALL_ACCOUNT_CRITERIA) { }

            WorldPacket* Write() override;

            std::vector<CriteriaProgress> Progress;
        };

        class RespondInspectAchievements final : public ServerPacket
        {
        public:
            RespondInspectAchievements() : ServerPacket(SMSG_RESPOND_INSPECT_ACHIEVEMENTS) { }

            WorldPacket* Write() override;

            uint64 Player;
            AllAchievements Data;
        };

        class CriteriaUpdate final : public ServerPacket
        {
        public:
            CriteriaUpdate() : ServerPacket(SMSG_CRITERIA_UPDATE, 4 + 8 + 16 + 4 + 4 + 4 + 4) { }

            WorldPacket* Write() override;

            uint32 CriteriaID = 0;
            uint64 Quantity = 0;
            uint64 PlayerGUID;
            uint32 Flags = 0;
            time_t CurrentTime = time_t(0);
            uint32 ElapsedTime = 0;
            uint32 CreationTime = 0;
        };

        class AccountCriteriaUpdate final : public ServerPacket
        {
        public:
            AccountCriteriaUpdate() : ServerPacket(SMSG_ACCOUNT_CRITERIA_UPDATE, 4 + 8 + 16 + 4 + 4 + 4 + 4) { }

            WorldPacket* Write() override;

            uint32 CriteriaID = 0;
            uint64 Quantity = 0;
            uint64 PlayerGUID;
            uint32 Flags = 0;
            time_t CurrentTime = time_t(0);
            uint32 ElapsedTime = 0;
            uint32 CreationTime = 0;
        };

        class CriteriaDeleted final : public ServerPacket
        {
        public:
            CriteriaDeleted() : ServerPacket(SMSG_CRITERIA_DELETED, 4) { }

            WorldPacket* Write() override;

            uint32 CriteriaID = 0;
        };

        class AchievementDeleted final : public ServerPacket
        {
        public:
            AchievementDeleted() : ServerPacket(SMSG_ACHIEVEMENT_DELETED, 4) { }

            WorldPacket* Write() override;

            uint32 AchievementID = 0;
            uint32 Immunities = 0; // this is just garbage, not used by client
        };

        class AchievementEarned final : public ServerPacket
        {
        public:
            AchievementEarned() : ServerPacket(SMSG_ACHIEVEMENT_EARNED, 16 + 4 + 4 + 4 + 4 + 1 + 16) { }

            WorldPacket* Write() override;

            uint64 Earner;
            uint32 EarnerNativeRealm = 0;
            uint32 EarnerVirtualRealm = 0;
            uint32 AchievementID = 0;
            time_t Time = time_t(0);
            bool Initial = false;
            uint64 Sender;
        };

        class ServerFirstAchievement final : public ServerPacket
        {
        public:
            ServerFirstAchievement() : ServerPacket(SMSG_SERVER_FIRST_ACHIEVEMENT) { }

            WorldPacket* Write() override;

            uint64 PlayerGUID;
            std::string Name;
            uint32 AchievementID = 0;
            bool GuildAchievement = false;
        };

        struct GuildCriteriaProgress
        {
            int32 CriteriaID = 0;
            uint32 DateCreated = 0;
            uint32 DateStarted = 0;
            time_t DateUpdated = 0;
            uint64 Quantity = 0;
            uint64 PlayerGUID;
            int32 Flags = 0;
        };

        class GuildCriteriaUpdate final : public ServerPacket
        {
        public:
            GuildCriteriaUpdate() : ServerPacket(SMSG_GUILD_CRITERIA_UPDATE) { }

            WorldPacket* Write() override;

            std::vector<GuildCriteriaProgress> Progress;
        };

        class GuildCriteriaDeleted final : public ServerPacket
        {
        public:
            GuildCriteriaDeleted() : ServerPacket(SMSG_GUILD_CRITERIA_DELETED, 16 + 4) { }

            WorldPacket* Write() override;

            uint64 GuildGUID;
            int32 CriteriaID = 0;
        };

        class GuildAchievementDeleted final : public ServerPacket
        {
        public:
            GuildAchievementDeleted() : ServerPacket(SMSG_GUILD_ACHIEVEMENT_DELETED, 16 + 4 + 4) { }

            WorldPacket* Write() override;

            uint64 GuildGUID;
            uint32 AchievementID = 0;
            time_t TimeDeleted = time_t(0);
        };

        class GuildAchievementEarned final : public ServerPacket
        {
        public:
            GuildAchievementEarned() : ServerPacket(SMSG_GUILD_ACHIEVEMENT_EARNED, 16 + 4 + 4) { }

            WorldPacket* Write() override;

            uint32 AchievementID = 0;
            uint64 GuildGUID;
            time_t TimeEarned = time_t(0);
        };

        class AllGuildAchievements final : public ServerPacket
        {
        public:
            AllGuildAchievements() : ServerPacket(SMSG_ALL_GUILD_ACHIEVEMENTS) { }

            WorldPacket* Write() override;

            std::vector<EarnedAchievement> Earned;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Achievement::CriteriaProgress const& achieve);
