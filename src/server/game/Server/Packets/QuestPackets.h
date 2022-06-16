////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2018 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef QuestPackets_h__
#define QuestPackets_h__

#include "Common.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Packet.h"
#include "ItemPacketsCommon.h"

struct PlayerChoice;
struct PlayerChoiceResponse;
struct PlayerChoiceResponseReward;

namespace WorldPackets
{
    namespace Quest
    {
        struct PlayerChoiceResponseRewardEntry
        {
            WorldPackets::WPItem::ItemInstance Item;
            int32 Quantity = 0;
        };

        struct PlayerChoiceResponseReward
        {
            int32 TitleID = 0;
            int32 PackageID = 0;
            int32 SkillLineID = 0;
            uint32 SkillPointCount = 0;
            uint32 ArenaPointCount = 0;
            uint32 HonorPointCount = 0;
            uint64 Money = 0;
            uint32 Xp = 0;
            std::vector<PlayerChoiceResponseRewardEntry> Items;
            std::vector<PlayerChoiceResponseRewardEntry> Currencies;
            std::vector<PlayerChoiceResponseRewardEntry> Factions;
            std::vector<PlayerChoiceResponseRewardEntry> ItemChoices;
        };

        struct PlayerChoiceResponse
        {
            int32 ResponseID = 0;
            int32 ChoiceArtFileID = 0;
            std::string Answer;
            std::string Header;
            std::string Description;
            std::string Confirmation;
            Optional<PlayerChoiceResponseReward> Reward;
        };

        class DisplayPlayerChoice final : public ServerPacket
        {
            public:
                DisplayPlayerChoice() : ServerPacket(SMSG_DISPLAY_PLAYER_CHOICE) { }

                WorldPacket const* Write() override;

                uint64 SenderGUID = 0;
                int32 ChoiceID = 0;
                int32 UiTextureKitID = 0;
                std::string Question;
                std::vector<PlayerChoiceResponse> Responses;
                bool CloseChoiceFrame = false;
                bool HideWarboardHeader = false;
        };

        class ChoiceResponse final : public ClientPacket
        {
            public:
                ChoiceResponse(WorldPacket& packet) : ClientPacket(CMSG_DISPLAY_PLAYER_CHOICE_RESPONSE, packet) { }

                void Read() override;

                int32 ChoiceID = 0;
                int32 ResponseID = 0;
        };

        class ContributionGetState final : public ClientPacket
        {
            public:
                ContributionGetState(WorldPacket& p_Packet) : ClientPacket(CMSG_CONTRIBUTION_GET_STATE, p_Packet) { }

                void Read() override;

                uint32 ContributionID = 0;
                uint32 OccurenceValue = 0;
        };

        class ContributionContribute final : public ClientPacket
        {
            public:
                ContributionContribute(WorldPacket& p_Packet) : ClientPacket(CMSG_CONTRIBUTION_CONTRIBUTE, p_Packet) { }

                void Read() override;

                uint64 CreatureGUID = 0;
                uint32 ContributionID = 0;
        };

        class ContributionSendState final : public ServerPacket
        {
            public:
                ContributionSendState() : ServerPacket(SMSG_CONTRIBUTION_SEND_STATE) { }

                WorldPacket const* Write() override;

                uint32 Progress = 0;
                uint32 ContributionID = 0;
                uint32 OccurenceValue = 0;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, PlayerChoiceResponse const& response);
ByteBuffer& operator<<(ByteBuffer& data, PlayerChoiceResponseReward const& reward);

#endif // QuestPackets_h__