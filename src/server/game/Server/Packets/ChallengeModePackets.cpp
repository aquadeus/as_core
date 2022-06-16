////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ChallengeModePackets.h"
#include "WowTime.hpp"

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::ChallengeMode::ModeAttempt const& modeAttempt)
{
    data << modeAttempt.InstanceRealmAddress;
    data << modeAttempt.AttemptID;
    data << modeAttempt.CompletionTime;
    data << uint32(MS::Utilities::WowTime::Encode(modeAttempt.CompletionDate));
    data << modeAttempt.MedalEarned;
    data << static_cast<uint32>(modeAttempt.Members.size());
    for (auto const& map : modeAttempt.Members)
    {
        data << map.VirtualRealmAddress;
        data << map.NativeRealmAddress;
        data.appendPackGUID(map.Guid);
        data << map.SpecializationID;
    }

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::ChallengeMode::ItemReward const& itemReward)
{
    data << itemReward.ItemID;
    data << itemReward.ItemDisplayID;
    data << itemReward.Quantity;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::ChallengeMode::MapChallengeModeReward const& mapChallengeModeReward)
{
    data << static_cast<uint32>(mapChallengeModeReward.Rewards.size());
    for (auto const& map : mapChallengeModeReward.Rewards)
    {
        data << static_cast<uint32>(map.ItemRewards.size());
        data << static_cast<uint32>(map.CurrencyRewards.size());
        data << map.Money;

        for (auto const& item : map.ItemRewards)
            data << item;

        for (auto const& currency : map.CurrencyRewards)
        {
            data << currency.CurrencyID;
            data << currency.Quantity;
        }
    }

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::ChallengeMode::ChallengeModeMap const& challengeModeMap)
{
    data << challengeModeMap.MapId;
    data << challengeModeMap.ChallengeID;
    data << challengeModeMap.BestCompletionMilliseconds;
    data << challengeModeMap.LastCompletionMilliseconds;
    data << challengeModeMap.CompletedChallengeLevel;
    data << uint32(MS::Utilities::WowTime::Encode(challengeModeMap.BestMedalDate));

    data << static_cast<uint32>(challengeModeMap.BestSpecID.size());

    for (auto const& v : challengeModeMap.Affixes)
        data << v;

    for (auto const& map : challengeModeMap.BestSpecID)
        data << map;

    return data;
}

void WorldPackets::ChallengeMode::RequestLeaders::Read()
{
    _worldPacket >> MapId;
    _worldPacket >> ChallengeID;

    LastGuildUpdate = _worldPacket.read<uint32>();
    LastRealmUpdate = _worldPacket.read<uint32>();
}

WorldPacket const* WorldPackets::ChallengeMode::RequestLeadersResult::Write()
{
    _worldPacket << MapID;
    _worldPacket << ChallengeID;

    _worldPacket << static_cast<uint32>(LastGuildUpdate);
    _worldPacket << static_cast<uint32>(LastRealmUpdate);

    _worldPacket << static_cast<uint32>(GuildLeaders.size());
    _worldPacket << static_cast<uint32>(RealmLeaders.size());

    for (auto const& realmLeaders : RealmLeaders)
        _worldPacket << realmLeaders;

    for (auto const& guildLeaders : GuildLeaders)
        _worldPacket << guildLeaders;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::ChallengeMode::AllMapStats::Write()
{
    _worldPacket << static_cast<uint32>(ChallengeModeMaps.size());
    for (auto const& map : ChallengeModeMaps)
        _worldPacket << map;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::ChallengeMode::ChallengeModeReset::Write()
{
    _worldPacket << MapID;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::ChallengeMode::ChallengeModeStart::Write()
{
    _worldPacket << MapID;
    _worldPacket << ChallengeID;
    _worldPacket << StartedChallengeLevel;

    for (uint32 v : Affixes)
        _worldPacket << v;

    _worldPacket << uint32(0);
    _worldPacket << uint32(0);

    _worldPacket.WriteBit(IsKeyCharged);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::ChallengeMode::ChallengeModeComplete::Write()
{
    _worldPacket << CompletionMilliseconds;
    _worldPacket << MapID;
    _worldPacket << ChallengeID;
    _worldPacket << RewardLevel;

    _worldPacket.WriteBit(IsCompletedInTimer);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::ChallengeMode::ChallengeModeNewPlayerRecord::Write()
{
    _worldPacket << CompletionMilliseconds;
    _worldPacket << MapID;
    _worldPacket << StartedChallengeLevel;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::ChallengeMode::ChallengeModeMapStatsUpdate::Write()
{
    _worldPacket << Stats;

    return &_worldPacket;
}

void WorldPackets::ChallengeMode::StartChallengeMode::Read()
{
    _worldPacket >> IsKeyCharged;
    _worldPacket >> UnkInt;
    _worldPacket.readPackGUID(GameObjectGUID);
}

WorldPacket const* WorldPackets::ChallengeMode::ChangePlayerDifficultyResult::Write()
{
    _worldPacket.FlushBits();

    _worldPacket.WriteBits(Result, 4);
    switch (Result)
    {
        case 5:
        case 8:
            _worldPacket.WriteBit(Cooldown);
            _worldPacket.FlushBits();
            _worldPacket << CooldownReason;
            break;
        case 11:
            _worldPacket << InstanceMapID;
            _worldPacket << DifficultyRecID;
            break;
        case 2:
            _worldPacket << MapID;
            break;
        case 4:
            _worldPacket.appendPackGUID(Guid);
            break;
        default:
            break;
    }

    return &_worldPacket;
}

WorldPacket const* WorldPackets::ChallengeMode::ChallengeModeUpdateDeathCount::Write()
{
    _worldPacket << DeathCount;
    return &_worldPacket;
}
