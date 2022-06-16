////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#include "AchievementPackets.h"
#include "WowTime.hpp"

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Achievement::EarnedAchievement const& earned)
{
    data << uint32(earned.Id);
    data << uint32(MS::Utilities::WowTime::Encode(earned.Date));
    data.appendPackGUID(earned.Owner);
    data << uint32(earned.VirtualRealmAddress);
    data << uint32(earned.NativeRealmAddress);
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Achievement::AllAchievements const& allAchievements)
{
    data << uint32(allAchievements.Earned.size());
    data << uint32(allAchievements.Progress.size());

    for (WorldPackets::Achievement::EarnedAchievement const& earned : allAchievements.Earned)
        data << earned;

    for (WorldPackets::Achievement::CriteriaProgress const& progress : allAchievements.Progress)
        data << progress;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& _worldPacket, WorldPackets::Achievement::CriteriaProgress const& progress)
{
    _worldPacket << uint32(progress.Id);
    _worldPacket << uint64(progress.Quantity);
    _worldPacket.appendPackGUID(progress.Player);
    _worldPacket << uint32(MS::Utilities::WowTime::Encode(progress.Date));
    _worldPacket << uint32(progress.TimeFromStart);
    _worldPacket << uint32(progress.TimeFromCreate);
    _worldPacket.WriteBits(progress.Flags, 4);
    _worldPacket.FlushBits();

    return _worldPacket;
}

WorldPacket* WorldPackets::Achievement::AllAchievementData::Write()
{
    _worldPacket << Data;

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::AllAccountCriteria::Write()
{
    _worldPacket << uint32(Progress.size());

    for (WorldPackets::Achievement::CriteriaProgress const& progress : Progress)
    {
        _worldPacket << uint32(progress.Id);
        _worldPacket << uint64(progress.Quantity);
        _worldPacket.appendPackGUID(progress.Player);
        _worldPacket << uint32(MS::Utilities::WowTime::Encode(progress.Date));
        _worldPacket << uint32(progress.TimeFromStart);
        _worldPacket << uint32(progress.TimeFromCreate);
        _worldPacket.WriteBits(progress.Flags, 4);
        _worldPacket.FlushBits();
    }

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::RespondInspectAchievements::Write()
{
    _worldPacket.appendPackGUID(Player);
    _worldPacket << Data;

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::CriteriaUpdate::Write()
{
    _worldPacket << uint32(CriteriaID);
    _worldPacket << uint64(Quantity);
    _worldPacket.appendPackGUID(PlayerGUID);
    _worldPacket << uint32(Flags);
    _worldPacket << uint32(MS::Utilities::WowTime::Encode(CurrentTime));
    _worldPacket << uint32(ElapsedTime);
    _worldPacket << uint32(CreationTime);

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::AccountCriteriaUpdate::Write()
{
    _worldPacket << uint32(CriteriaID);
    _worldPacket << uint64(Quantity);
    _worldPacket.appendPackGUID(PlayerGUID);
    _worldPacket << uint32(MS::Utilities::WowTime::Encode(CurrentTime));
    _worldPacket << uint32(ElapsedTime);
    _worldPacket << uint32(CreationTime);

    _worldPacket.WriteBits(Flags, 4);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::CriteriaDeleted::Write()
{
    _worldPacket << uint32(CriteriaID);

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::AchievementDeleted::Write()
{
    _worldPacket << uint32(AchievementID);
    _worldPacket << uint32(Immunities);

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::AchievementEarned::Write()
{
    _worldPacket.appendPackGUID(Sender);
    _worldPacket.appendPackGUID(Earner);
    _worldPacket << uint32(AchievementID);
    _worldPacket << uint32(MS::Utilities::WowTime::Encode(Time));
    _worldPacket << uint32(EarnerNativeRealm);
    _worldPacket << uint32(EarnerVirtualRealm);
    _worldPacket.WriteBit(Initial);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::ServerFirstAchievement::Write()
{
    _worldPacket.WriteBits(Name.length(), 7);
    _worldPacket.WriteBit(GuildAchievement);
    _worldPacket.appendPackGUID(PlayerGUID);
    _worldPacket << AchievementID;
    _worldPacket.WriteString(Name);

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::GuildCriteriaUpdate::Write()
{
    _worldPacket << uint32(Progress.size());

    for (GuildCriteriaProgress const& progress : Progress)
    {
        _worldPacket << int32(progress.CriteriaID);
        _worldPacket << uint32(progress.DateCreated);
        _worldPacket << uint32(progress.DateStarted);
        _worldPacket << uint32(MS::Utilities::WowTime::Encode(progress.DateUpdated));
        _worldPacket << uint64(progress.Quantity);
        _worldPacket.appendPackGUID(progress.PlayerGUID);
        _worldPacket << int32(progress.Flags);
    }

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::GuildCriteriaDeleted::Write()
{
    _worldPacket.appendPackGUID(GuildGUID);
    _worldPacket << int32(CriteriaID);

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::GuildAchievementDeleted::Write()
{
    _worldPacket.appendPackGUID(GuildGUID);
    _worldPacket << uint32(AchievementID);
    _worldPacket << uint32(MS::Utilities::WowTime::Encode(TimeDeleted));

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::GuildAchievementEarned::Write()
{
    _worldPacket.appendPackGUID(GuildGUID);
    _worldPacket << uint32(AchievementID);
    _worldPacket << uint32(MS::Utilities::WowTime::Encode(TimeEarned));

    return &_worldPacket;
}

WorldPacket* WorldPackets::Achievement::AllGuildAchievements::Write()
{
    _worldPacket << uint32(Earned.size());

    for (EarnedAchievement const& earned : Earned)
        _worldPacket << earned;

    return &_worldPacket;
}
