////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "GuildPackets.h"

void WorldPackets::Guild::GuildBankMoveItemsPlayerBank::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> PlayerSlot;
    _worldPacket >> PlayerBag;

    if (PlayerBag)
        _worldPacket >> PlayerBag;
}

void WorldPackets::Guild::GuildBankMoveItemsBankPlayer::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> PlayerSlot;
    _worldPacket >> PlayerBag;

    if (PlayerBag)
        _worldPacket >> PlayerBag;
}

void WorldPackets::Guild::GuildBankMoveItemsBankBank::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> NewBankTab;
    _worldPacket >> NewBankSlot;
}

void WorldPackets::Guild::GuildBankMoveItemsAuto::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
}

void WorldPackets::Guild::GuildBankMoveItemsPlayerBankCount::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> PlayerSlot;
    _worldPacket >> StackCount;
    _worldPacket >> PlayerBag;

    if (PlayerBag)
        _worldPacket >> PlayerBag;
}

void WorldPackets::Guild::GuildBankMoveItemsBankPlayerCount::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> PlayerSlot;
    _worldPacket >> StackCount;
    _worldPacket >> PlayerBag;

    if (PlayerBag)
        _worldPacket >> PlayerBag;
}

void WorldPackets::Guild::GuildBankMoveItemsBankBankCount::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> NewBankTab;
    _worldPacket >> NewBankSlot;
    _worldPacket >> StackCount;
}

void WorldPackets::Guild::GuildBankMergeItemsPlayerBank::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> PlayerSlot;
    _worldPacket >> StackCount;
    _worldPacket >> PlayerBag;

    if (PlayerBag)
        _worldPacket >> PlayerBag;
}

void WorldPackets::Guild::GuildBankMergeItemsBankPlayer::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> PlayerSlot;
    _worldPacket >> StackCount;
    _worldPacket >> PlayerBag;

    if (PlayerBag)
        _worldPacket >> PlayerBag;
}

void WorldPackets::Guild::GuildBankMergeItemsBankBank::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> NewBankTab;
    _worldPacket >> NewBankSlot;
    _worldPacket >> StackCount;
}

void WorldPackets::Guild::GuildBankSwapItemsBankPlayer::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> PlayerSlot;
    _worldPacket >> PlayerBag;

    if (PlayerBag)
        _worldPacket >> PlayerBag;
}

void WorldPackets::Guild::GuildBankSwapItemsBankBank::Read()
{
    _worldPacket.readPackGUID(Banker);
    _worldPacket >> BankTab;
    _worldPacket >> BankSlot;
    _worldPacket >> NewBankTab;
    _worldPacket >> NewBankSlot;
}
