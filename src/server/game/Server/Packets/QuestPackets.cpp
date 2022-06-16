////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2018 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "QuestPackets.h"

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Quest::PlayerChoiceResponseRewardEntry const& playerChoiceResponseRewardEntry)
{
    data << playerChoiceResponseRewardEntry.Item;
    data << int32(playerChoiceResponseRewardEntry.Quantity);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Quest::PlayerChoiceResponseReward const& playerChoiceResponseReward)
{
    data << int32(playerChoiceResponseReward.TitleID);
    data << int32(playerChoiceResponseReward.PackageID);
    data << int32(playerChoiceResponseReward.SkillLineID);
    data << uint32(playerChoiceResponseReward.SkillPointCount);
    data << uint32(playerChoiceResponseReward.ArenaPointCount);
    data << uint32(playerChoiceResponseReward.HonorPointCount);
    data << uint64(playerChoiceResponseReward.Money);
    data << uint32(playerChoiceResponseReward.Xp);
    data << uint32(playerChoiceResponseReward.Items.size());
    data << uint32(playerChoiceResponseReward.Currencies.size());
    data << uint32(playerChoiceResponseReward.Factions.size());
    data << uint32(playerChoiceResponseReward.ItemChoices.size());

    for (WorldPackets::Quest::PlayerChoiceResponseRewardEntry const& item : playerChoiceResponseReward.Items)
        data << item;

    for (WorldPackets::Quest::PlayerChoiceResponseRewardEntry const& currency : playerChoiceResponseReward.Currencies)
        data << currency;

    for (WorldPackets::Quest::PlayerChoiceResponseRewardEntry const& faction : playerChoiceResponseReward.Factions)
        data << faction;

    for (WorldPackets::Quest::PlayerChoiceResponseRewardEntry const& itemChoice : playerChoiceResponseReward.ItemChoices)
        data << itemChoice;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Quest::PlayerChoiceResponse const& playerChoiceResponse)
{
    data << int32(playerChoiceResponse.ResponseID);
    data << int32(playerChoiceResponse.ChoiceArtFileID);

    data.WriteBits(playerChoiceResponse.Answer.length(), 9);
    data.WriteBits(playerChoiceResponse.Header.length(), 9);
    data.WriteBits(playerChoiceResponse.Description.length(), 11);
    data.WriteBits(playerChoiceResponse.Confirmation.length(), 7);
    data.WriteBit(playerChoiceResponse.Reward.is_initialized());
    data.FlushBits();

    if (playerChoiceResponse.Reward)
        data << *playerChoiceResponse.Reward;

    data.WriteString(playerChoiceResponse.Answer);
    data.WriteString(playerChoiceResponse.Header);
    data.WriteString(playerChoiceResponse.Description);
    data.WriteString(playerChoiceResponse.Confirmation);
    return data;
}

WorldPacket const* WorldPackets::Quest::DisplayPlayerChoice::Write()
{
    _worldPacket << int32(ChoiceID);
    _worldPacket << uint32(Responses.size());
    _worldPacket.appendPackGUID(SenderGUID);
    _worldPacket << int32(UiTextureKitID);
    _worldPacket.WriteBits(Question.length(), 8);
    _worldPacket.WriteBit(CloseChoiceFrame);
    _worldPacket.WriteBit(HideWarboardHeader);
    _worldPacket.FlushBits();

    for (PlayerChoiceResponse const& response : Responses)
        _worldPacket << response;

    _worldPacket.WriteString(Question);
    return &_worldPacket;
}

void WorldPackets::Quest::ChoiceResponse::Read()
{
    _worldPacket >> ChoiceID;
    _worldPacket >> ResponseID;
}

void WorldPackets::Quest::ContributionGetState::Read()
{
    _worldPacket >> ContributionID;
    _worldPacket >> OccurenceValue;
}

void WorldPackets::Quest::ContributionContribute::Read()
{
    _worldPacket.readPackGUID(CreatureGUID);
    _worldPacket >> ContributionID;
}

WorldPacket const* WorldPackets::Quest::ContributionSendState::Write()
{
    _worldPacket << uint32(Progress);
    _worldPacket << uint32(ContributionID);
    _worldPacket << uint32(OccurenceValue);

    return &_worldPacket;
}