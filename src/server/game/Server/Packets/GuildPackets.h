////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef GUILD_PACKETS_H
# define GUILD_PACKETS_H

# include "Packet.h"

namespace WorldPackets
{
    namespace Guild
    {
        class GuildBankMoveItemsPlayerBank final : public ClientPacket
        {
            public:
                GuildBankMoveItemsPlayerBank(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MOVE_ITEMS_PLAYER_BANK, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 PlayerSlot;
                uint8 PlayerBag = 0;
        };

        class GuildBankMoveItemsBankPlayer final : public ClientPacket
        {
            public:
                GuildBankMoveItemsBankPlayer(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 PlayerSlot;
                uint8 PlayerBag = 0;
        };

        class GuildBankMoveItemsBankBank final : public ClientPacket
        {
            public:
                GuildBankMoveItemsBankBank(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_BANK, packet) { }

                void Read() override;

                uint64 Banker = 0;
                uint8 BankTab = 0;
                uint8 BankSlot = 0;
                uint8 NewBankTab = 0;
                uint8 NewBankSlot = 0;
        };

        class GuildBankMoveItemsAuto final : public ClientPacket
        {
            public:
                GuildBankMoveItemsAuto(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER_AUTO, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
        };

        class GuildBankMoveItemsPlayerBankCount final : public ClientPacket
        {
            public:
                GuildBankMoveItemsPlayerBankCount(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MOVE_ITEMS_PLAYER_BANK_COUNT, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 PlayerSlot;
                uint8 PlayerBag;
                uint32 StackCount;
        };

        class GuildBankMoveItemsBankPlayerCount final : public ClientPacket
        {
            public:
                GuildBankMoveItemsBankPlayerCount(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_PLAYER_COUNT, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 PlayerSlot;
                uint8 PlayerBag;
                uint32 StackCount;
        };

        class GuildBankMoveItemsBankBankCount final : public ClientPacket
        {
            public:
                GuildBankMoveItemsBankBankCount(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MOVE_ITEMS_BANK_BANK_COUNT, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 NewBankTab;
                uint8 NewBankSlot;
                uint32 StackCount;
        };

        class GuildBankMergeItemsPlayerBank final : public ClientPacket
        {
            public:
                GuildBankMergeItemsPlayerBank(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MERGE_ITEMS_PLAYER_BANK, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 PlayerSlot;
                uint8 PlayerBag;
                uint32 StackCount;
        };

        class GuildBankMergeItemsBankPlayer final : public ClientPacket
        {
            public:
                GuildBankMergeItemsBankPlayer(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MERGE_ITEMS_BANK_PLAYER, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 PlayerSlot;
                uint8 PlayerBag;
                uint32 StackCount;
        };

        class GuildBankMergeItemsBankBank final : public ClientPacket
        {
            public:
                GuildBankMergeItemsBankBank(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_MERGE_ITEMS_BANK_BANK, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 NewBankTab;
                uint8 NewBankSlot;
                uint32 StackCount;
        };

        class GuildBankSwapItemsBankPlayer final : public ClientPacket
        {
            public:
                GuildBankSwapItemsBankPlayer(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_SWAP_ITEMS_BANK_PLAYER, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 PlayerSlot;
                uint8 PlayerBag = 0;
        };

        class GuildBankSwapItemsBankBank final : public ClientPacket
        {
            public:
                GuildBankSwapItemsBankBank(WorldPacket& packet) : ClientPacket(CMSG_GUILD_BANK_SWAP_ITEMS_BANK_BANK, packet) { }

                void Read() override;

                uint64 Banker;
                uint8 BankTab;
                uint8 BankSlot;
                uint8 NewBankTab;
                uint8 NewBankSlot;
        };
    }
}

#endif ///< GUILD_PACKETS_H
