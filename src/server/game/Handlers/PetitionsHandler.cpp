////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CROSS
#include "Common.h"
#include "Language.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Guild.h"
#include "Arena.h"
#include "GossipDef.h"
#include "SocialMgr.h"
#include "WordFilterMgr.h"

#define CHARTER_DISPLAY_ID 16161

// Charters ID in item_template
enum CharterItemIDs
{
    GUILD_CHARTER                                 = 5863,
    ARENA_TEAM_CHARTER_2v2                        = 23560,
    ARENA_TEAM_CHARTER_3v3                        = 23561,
    ARENA_TEAM_CHARTER_5v5                        = 23562
};

enum CharterCosts
{
    GUILD_CHARTER_COST                            = 1000,
    ARENA_TEAM_CHARTER_2v2_COST                   = 800000,
    ARENA_TEAM_CHARTER_3v3_COST                   = 1200000,
    ARENA_TEAM_CHARTER_5v5_COST                   = 2000000
};

void WorldSession::HandlePetitionBuyOpcode(WorldPacket& p_Packet)
{
    uint64 l_UnitGUID = 0;
    uint32 l_TitleLenght = 0;
    std::string l_Title;

    l_TitleLenght = p_Packet.ReadBits(7);
    p_Packet.readPackGUID(l_UnitGUID);

    l_Title = p_Packet.ReadString(l_TitleLenght);

    /// Prevent cheating
    Creature* l_Creature = GetPlayer()->GetNPCIfCanInteractWith(l_UnitGUID, UNIT_NPC_FLAG_PETITIONER);

    if (!l_Creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandlePetitionBuyOpcode - Unit (GUID: %u) not found or you can't interact with him.", GUID_LOPART(l_UnitGUID));
        return;
    }

    /// Remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    uint32 l_CharterID = 0;
    uint32 l_Cost = 0;
    uint32 l_Type = 0;

    if (l_Creature->isTabardDesigner())
    {
        /// If tabard designer, then trying to buy a guild charter.
        /// do not let if already in guild.
        if (m_Player->GetGuildId())
            return;

        l_CharterID = GUILD_CHARTER;
        l_Cost      = GUILD_CHARTER_COST;
        l_Type      = GUILD_CHARTER_TYPE;
    }

    if (l_Type == GUILD_CHARTER_TYPE)
    {
        if (sGuildMgr->GetGuildByName(l_Title))
        {
            Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_EXISTS_S, l_Title);
            return;
        }
        if (sObjectMgr->IsReservedName(l_Title) || !ObjectMgr::IsValidCharterName(l_Title)
            || (sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE) && !sWordFilterMgr->FindBadWord(l_Title).empty()))
        {
            Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_INVALID, l_Title);
            return;
        }
    }

    const ItemTemplate * l_ItemTemplate = sObjectMgr->GetItemTemplate(l_CharterID);
    if (!l_ItemTemplate)
    {
        m_Player->SendBuyError(BUY_ERR_CANT_FIND_ITEM, NULL, l_CharterID, 0);
        return;
    }

    if (!m_Player->HasEnoughMoney(uint64(l_Cost)))
    {
        /// Player hasn't got enough money
        m_Player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, l_Creature, l_CharterID, 0);
        return;
    }

    ItemPosCountVec l_Destination;
    InventoryResult l_Msg = m_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, l_CharterID, l_ItemTemplate->BuyCount);

    if (l_Msg != EQUIP_ERR_OK)
    {
        m_Player->SendEquipError(l_Msg, NULL, NULL, l_CharterID);
        return;
    }

    m_Player->ModifyMoney(-(int32)l_Cost, "HandlePetitionBuyOpcode");

    Item * l_Charter = m_Player->StoreNewItem(l_Destination, l_CharterID, true);

    if (!l_Charter)
        return;

    l_Charter->SetUInt32Value(ITEM_FIELD_ENCHANTMENT, l_Charter->GetGUIDLow());

    /// ITEM_FIELD_ENCHANTMENT_1_1 is guild/arenateam id
    /// ITEM_FIELD_ENCHANTMENT_1_1+1 is current signatures count (showed on item)
    l_Charter->SetState(ITEM_CHANGED, m_Player);

    m_Player->SendNewItem(l_Charter, 1, true, false);

    /// A petition is invalid, if both the owner and the type matches
    /// we checked above, if this player is in an arenateam, so this must be
    /// datacorruption
    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_BY_OWNER);
    l_Stmt->setUInt32(0, m_Player->GetGUIDLow());
    l_Stmt->setUInt8(1, l_Type);

    uint64 l_PlayerGuid     = m_Player->GetGUID();
    uint32 l_CharterGuidLow = l_Charter->GetGUIDLow();

    CharacterDatabase.AsyncQuery(l_Stmt, m_Player->GetQueryCallbackMgr(), [l_Title, l_Type, l_PlayerGuid, l_CharterGuidLow](PreparedQueryResult const& p_Result)
    {
        Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
        if (!l_Player)
            return;

        std::ostringstream l_InvalidPetitionGUIDsStringStream;

        if (p_Result)
        {
            do
            {
                Field * l_Fields = p_Result->Fetch();
                l_InvalidPetitionGUIDsStringStream << '\'' << l_Fields[0].GetUInt32() << "', ";
            } while (p_Result->NextRow());
        }

        /// Delete petitions with the same guid as this one
        l_InvalidPetitionGUIDsStringStream << '\'' << l_CharterGuidLow << '\'';

        std::string l_TmpTitle = l_Title;

        CharacterDatabase.EscapeString(l_TmpTitle);

        SQLTransaction l_Transaction = CharacterDatabase.BeginTransaction();
        l_Transaction->PAppend("DELETE FROM petition WHERE petitionguid IN (%s)", l_InvalidPetitionGUIDsStringStream.str().c_str());
        l_Transaction->PAppend("DELETE FROM petition_sign WHERE petitionguid IN (%s)", l_InvalidPetitionGUIDsStringStream.str().c_str());

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PETITION);
        l_Stmt->setUInt32(0, l_Player->GetGUIDLow());
        l_Stmt->setUInt32(1, l_CharterGuidLow);
        l_Stmt->setString(2, l_TmpTitle);
        l_Stmt->setUInt8(3, uint8(l_Type));
        l_Transaction->Append(l_Stmt);

        CharacterDatabase.CommitTransaction(l_Transaction);
    });
}

void WorldSession::HandlePetitionShowSignOpcode(WorldPacket& p_Packet)
{
    uint64 l_ItemGUID = 0;

    p_Packet.readPackGUID(l_ItemGUID);

    /// Solve (possible) some strange compile problems with explicit use GUID_LOPART(petitionguid) at some GCC versions (wrong code optimization in compiler?)
    uint32 l_PetitionGuidLow = GUID_LOPART(l_ItemGUID);

    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_TYPE);
    l_Stmt->setUInt32(0, l_PetitionGuidLow);

    uint64 l_PlayerGuid = m_Player->GetGUID();

    CharacterDatabase.AsyncQuery(l_Stmt, m_Player->GetQueryCallbackMgr(), [l_PlayerGuid, l_ItemGUID, l_PetitionGuidLow](PreparedQueryResult const& p_Result)
    {
        if (!p_Result)
        {
            sLog->outDebug(LOG_FILTER_PLAYER_ITEMS, "Petition %u is not found for player " UI64FMTD "", GUID_LOPART(l_ItemGUID), l_PlayerGuid);
            return;
        }

        Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
        if (!l_Player)
            return;

        Field* l_Fields = p_Result->Fetch();
        uint32 l_Type = l_Fields[0].GetUInt8();

        /// If guild petition and has guild => error, return;
        if (l_Type == GUILD_CHARTER_TYPE && l_Player->GetGuildId())
            return;

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);
        l_Stmt->setUInt32(0, l_PetitionGuidLow);

        CharacterDatabase.AsyncQuery(l_Stmt, l_Player->GetQueryCallbackMgr(), [l_PlayerGuid, l_ItemGUID, l_PetitionGuidLow](PreparedQueryResult const& p_Result)
        {
            Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
            if (!l_Player)
                return;

            uint8 l_SignCount = 0;

            /// result == NULL also correct in case no sign yet
            if (p_Result)
                l_SignCount = uint8(p_Result->GetRowCount());

            WorldPacket l_Data(SMSG_PETITION_SHOW_SIGNATURES);

            l_Data.appendPackGUID(l_ItemGUID);
            l_Data.appendPackGUID(l_Player->GetGUID());
            l_Data.appendPackGUID(l_Player->GetSession()->GetBNetAccountGUID());

            l_Data << uint32(l_PetitionGuidLow);
            l_Data << uint32(l_SignCount);

            for (uint8 l_I = 1; l_I <= l_SignCount; ++l_I)
            {
                Field* l_SignFields = p_Result->Fetch();
                uint32 l_LowGUID = l_SignFields[0].GetUInt32();

                uint64 l_SignerGUID = MAKE_NEW_GUID(l_LowGUID, 0, HIGHGUID_PLAYER);

                l_Data.appendPackGUID(l_SignerGUID);
                l_Data << uint32(0);    ///< Sinature Choice

                p_Result->NextRow();
            }

            l_Player->GetSession()->SendPacket(&l_Data);
        });
    });
}

void WorldSession::HandlePetitionQueryOpcode(WorldPacket& p_RecvData)
{
    uint64 l_ItemGUID;

    uint32 l_PetitionID;

    p_RecvData >> l_PetitionID;
    p_RecvData.readPackGUID(l_ItemGUID);

    SendPetitionQueryOpcode(uint64(l_PetitionID));
}

void WorldSession::SendPetitionQueryOpcode(uint64 l_PetitionGUID)
{
    if (!m_Player)
        return;

    PreparedStatement * l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION);
    l_Stmt->setUInt32(0, GUID_LOPART(l_PetitionGUID));

    uint32 l_AccountId = GetAccountId();
    CharacterDatabase.AsyncQuery(l_Stmt, m_Player->GetQueryCallbackMgr(), [l_AccountId, l_PetitionGUID](PreparedQueryResult const& p_Result)
    {
        uint64 l_OwnerGUID = 0;
        uint32 l_Type;
        std::string l_GuildName = "NO_NAME_FOR_GUID";

        if (p_Result)
        {
            Field * l_Fields = p_Result->Fetch();

            l_OwnerGUID = MAKE_NEW_GUID(l_Fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
            l_GuildName = l_Fields[1].GetString();
            l_Type = l_Fields[2].GetUInt8();
        }
        else
        {
            sLog->outDebug(LOG_FILTER_NETWORKIO, "CMSG_PETITION_QUERY failed for petition (GUID: %u)", GUID_LOPART(l_PetitionGUID));
            return;
        }

        WorldSession* l_Session = sWorld->FindSession(l_AccountId);
        if (!l_Session)
            return;

        WorldPacket l_Data(SMSG_QUERY_PETITION_RESPONSE);

        l_Data << uint32(GUID_LOPART(l_PetitionGUID));
        l_Data.WriteBit(l_Type == GUILD_CHARTER_TYPE);
        l_Data.FlushBits();

        if (l_Type == GUILD_CHARTER_TYPE)
        {
            l_Data << uint32(GUID_LOPART(l_PetitionGUID));
            l_Data.appendPackGUID(l_OwnerGUID);
            l_Data << uint32(4); ///< minSignatures
            l_Data << uint32(4); ///< maxSignatures
            l_Data << uint32(0); ///< deadLine
            l_Data << uint32(0); ///< issueDate
            l_Data << uint32(0); ///< allowedGuildID
            l_Data << uint32(0); ///< allowedClasses
            l_Data << uint32(0); ///< allowedRaces
            l_Data << uint16(0); ///< allowedGender
            l_Data << uint32(0); ///< allowedMinLevel
            l_Data << uint32(0); ///< allowedMaxLevel
            l_Data << uint32(0); ///< numChoices
            l_Data << uint32(0); ///< staticTypes
            l_Data << uint32(0); ///< Muid

            l_Data.WriteBits(l_GuildName.size(), 7);
            l_Data.WriteBits(0, 12);                  ///< Body Text

            for (int i = 0; i < 10; ++i)
                l_Data.WriteBits(0, 6);               ///< M_choicetext[i]

            l_Data.FlushBits();

            l_Data.WriteString(l_GuildName);
        }

        l_Session->SendPacket(&l_Data);
    });
}

void WorldSession::HandlePetitionRenameOpcode(WorldPacket & p_Packet)
{
    uint64 l_PetitionGUID = 0;

    uint32 l_NewGuildNameLenght = 0;

    std::string l_NewGuildName;

    p_Packet.readPackGUID(l_PetitionGUID);
    l_NewGuildNameLenght    = p_Packet.ReadBits(7);
    l_NewGuildName          = p_Packet.ReadString(l_NewGuildNameLenght);

    Item* l_Item = m_Player->GetItemByGuid(l_PetitionGUID);
    if (!l_Item)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "CMSG_PETITION_QUERY failed for petition (GUID: %u)", GUID_LOPART(l_PetitionGUID));
        return;
    }

    if (sGuildMgr->GetGuildByName(l_NewGuildName))
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_EXISTS_S, l_NewGuildName);
        return;
    }
    if (sObjectMgr->IsReservedName(l_NewGuildName) || !ObjectMgr::IsValidCharterName(l_NewGuildName)
        || (sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE) && !sWordFilterMgr->FindBadWord(l_NewGuildName).empty()))
    {
        Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NAME_INVALID, l_NewGuildName);
        return;
    }

    PreparedStatement * l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PETITION_NAME);
    l_Stmt->setString(0, l_NewGuildName);
    l_Stmt->setUInt32(1, GUID_LOPART(l_PetitionGUID));

    CharacterDatabase.Execute(l_Stmt);

    WorldPacket l_Data(SMSG_PETITION_RENAME);
    l_Data.appendPackGUID(l_PetitionGUID);
    l_Data.WriteBits(l_NewGuildNameLenght, 7);
    l_Data.FlushBits();

    l_Data.WriteString(l_NewGuildName);

    SendPacket(&l_Data);
}

void WorldSession::HandlePetitionSignOpcode(WorldPacket& p_Packet)
{
    uint64 l_OwnerGUID    = 0;
    uint64 l_PetitionGUID = 0;
    uint8 l_Choice = 0;

    p_Packet.readPackGUID(l_PetitionGUID);
    p_Packet >> l_Choice;

    uint64 l_PlayerGuid = m_Player->GetGUID();

    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_OWNER_BY_GUID);
    l_Stmt->setUInt32(0, GUID_LOPART(l_PetitionGUID));
    CharacterDatabase.AsyncQuery(l_Stmt, m_Player->GetQueryCallbackMgr(),[l_PetitionGUID, l_Choice, l_PlayerGuid](PreparedQueryResult const& p_Result)
    {
        if (!p_Result)
            return;

        Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
        if (!l_Player)
            return;

        Field * l_Fields = p_Result->Fetch();
        uint64 l_OwnerGUID = MAKE_NEW_GUID(l_Fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURES);
        l_Stmt->setUInt32(0, GUID_LOPART(l_PetitionGUID));
        l_Stmt->setUInt32(1, GUID_LOPART(l_PetitionGUID));

        CharacterDatabase.AsyncQuery(l_Stmt, l_Player->GetQueryCallbackMgr(), [l_PetitionGUID, l_OwnerGUID, l_Choice, l_PlayerGuid](PreparedQueryResult const& p_Result)
        {
            if (!p_Result)
                return;

            Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
            if (!l_Player)
                return;

            Field* l_Fields = p_Result->Fetch();

            uint64 l_SignCount = l_Fields[1].GetUInt64();
            uint8  l_Type = l_Fields[2].GetUInt8();

            uint32 l_PlayerGuidLow = l_Player->GetGUIDLow();

            if (GUID_LOPART(l_OwnerGUID) == l_PlayerGuidLow)
                return;

            // not let enemies sign guild charter
            if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD) && l_Player->GetTeam() != sObjectMgr->GetPlayerTeamByGUID(l_OwnerGUID))
            {
                if (l_Type == GUILD_CHARTER_TYPE)
                    Guild::SendCommandResult(l_Player->GetSession(), GUILD_CREATE_S, ERR_GUILD_NOT_ALLIED);
                return;
            }

            if (l_Type == GUILD_CHARTER_TYPE)
            {
                if (l_Player->GetGuildId())
                {
                    Guild::SendCommandResult(l_Player->GetSession(), GUILD_INVITE_S, ERR_ALREADY_IN_GUILD_S, l_Player->GetName());
                    return;
                }
                if (l_Player->GetGuildIdInvited())
                {
                    Guild::SendCommandResult(l_Player->GetSession(), GUILD_INVITE_S, ERR_ALREADY_INVITED_TO_GUILD_S, l_Player->GetName());
                    return;
                }
            }

            /// Client signs maximum
            if (++l_SignCount > l_Type)
                return;

            /// Client doesn't allow to sign petition two times by one character, but not check sign by another character from same account
            /// not allow sign another player from already sign player account
            PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIG_BY_ACCOUNT);
            l_Stmt->setUInt32(0, l_Player->GetSession()->GetAccountId());
            l_Stmt->setUInt32(1, GUID_LOPART(l_PetitionGUID));

            CharacterDatabase.AsyncQuery(l_Stmt, l_Player->GetQueryCallbackMgr(), [l_PetitionGUID, l_OwnerGUID, l_Choice, l_PlayerGuid](PreparedQueryResult const& p_Result)
            {
                Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
                if (!l_Player)
                    return;

                if (p_Result)
                {
                    l_Player->GetSession()->SendPetitionSignResult(l_Player->GetGUID(), l_PetitionGUID, PETITION_SIGN_ALREADY_SIGNED);
                    return;
                }

                PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PETITION_SIGNATURE);

                l_Stmt->setUInt32(0, GUID_LOPART(l_OwnerGUID));
                l_Stmt->setUInt32(1, GUID_LOPART(l_PetitionGUID));
                l_Stmt->setUInt32(2, l_PlayerGuid);
                l_Stmt->setUInt32(3, l_Player->GetSession()->GetAccountId());

                CharacterDatabase.Execute(l_Stmt);

                l_Player->GetSession()->SendPetitionSignResult(l_Player->GetGUID(), l_PetitionGUID, PETITION_SIGN_OK);

                Player * l_Owner = ObjectAccessor::FindPlayer(l_OwnerGUID);
                if (l_Owner)
                    l_Owner->GetSession()->SendPetitionSignResult(l_Player->GetGUID(), l_PetitionGUID, PETITION_SIGN_OK);
            });
        });
    });
}

void WorldSession::HandlePetitionDeclineOpcode(WorldPacket& p_Packet)
{
    uint64 l_PetitionGUID = 0;

    p_Packet.readPackGUID(l_PetitionGUID);

    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_OWNER_BY_GUID);
    l_Stmt->setUInt32(0, GUID_LOPART(l_PetitionGUID));

    uint64 l_PlayerGuid = m_Player->GetGUID();
    CharacterDatabase.AsyncQuery(l_Stmt, m_Player->GetQueryCallbackMgr(), [l_PlayerGuid](PreparedQueryResult const& p_Result)
    {
        if (!p_Result)
            return;

        Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
        if (!l_Player)
            return;

        Field * l_Fields = p_Result->Fetch();
        uint64 l_OwnerGUID = MAKE_NEW_GUID(l_Fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);

        Player* l_Owner = ObjectAccessor::FindPlayer(l_OwnerGUID);

        if (l_Owner)
            l_Owner->GetSession()->SendPetitionDeclined(l_Player->GetGUID());
    });
}

void WorldSession::HandleOfferPetitionOpcode(WorldPacket & p_Packet)
{
    uint64 l_PetitionGUID     = 0;
    uint64 l_TargetPlayerGUID = 0;

    uint32 l_PetitionGuidLow = 0;
    uint32 l_Type            = 0;


    Player * l_Player;

    p_Packet.readPackGUID(l_PetitionGUID);
    p_Packet.readPackGUID(l_TargetPlayerGUID);

    l_Player = ObjectAccessor::FindPlayer(l_TargetPlayerGUID);

    if (!l_Player)
        return;

    l_Type = GUILD_CHARTER_TYPE;

    l_PetitionGuidLow = GUID_LOPART(l_PetitionGUID);

    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD) && GetPlayer()->GetTeam() != l_Player->GetTeam())
    {
        if (l_Type == GUILD_CHARTER_TYPE)
            Guild::SendCommandResult(this, GUILD_CREATE_S, ERR_GUILD_NOT_ALLIED);
        return;
    }

    if (l_Type == GUILD_CHARTER_TYPE)
    {
        if (l_Player->GetGuildIdInvited())
        {
            SendPetitionSignResult(m_Player->GetGUID(), l_PetitionGUID, PETITION_SIGN_ALREADY_SIGNED_OTHER);
            return;
        }

        if (l_Player->GetGuildId())
        {
            SendPetitionSignResult(m_Player->GetGUID(), l_PetitionGUID, PETITION_SIGN_ALREADY_IN_GUILD);
            return;
        }
    }

    uint64 l_PlayerGuid = m_Player->GetGUID();

    PreparedStatement * l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);
    l_Stmt->setUInt32(0, l_PetitionGuidLow);
    CharacterDatabase.AsyncQuery(l_Stmt, m_Player->GetQueryCallbackMgr(), [l_TargetPlayerGUID, l_PlayerGuid, l_PetitionGuidLow, l_PetitionGUID](PreparedQueryResult const& p_Result)
    {
        Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
        if (!l_Player)
            return;

        Player* l_TargetPlayer = sObjectAccessor->FindPlayer(l_TargetPlayerGUID);
        if (!l_TargetPlayer)
            return;

        std::vector<uint32> l_PlayerGuids;
        uint8 l_SignCount = 0;

        // result == NULL also correct charter without signs
        if (p_Result)
        {
            l_SignCount = uint8(p_Result->GetRowCount());

            do
            {
                Field * l_Fields = p_Result->Fetch();
                l_PlayerGuids.push_back(l_Fields[0].GetUInt32());
            } while (p_Result->NextRow());
        }

        for (auto itr : l_PlayerGuids)
        {
            if (GUID_LOPART(l_TargetPlayerGUID) == itr)
            {
                l_TargetPlayer->GetSession()->SendAlreadySigned(l_TargetPlayerGUID);
                return;
            }
        }

        WorldPacket l_Data(SMSG_PETITION_SHOW_SIGNATURES);

        l_Data.appendPackGUID(l_PetitionGUID);
        l_Data.appendPackGUID(l_Player->GetGUID());
        l_Data.appendPackGUID(l_Player->GetSession()->GetBNetAccountGUID());

        l_Data << uint32(l_PetitionGuidLow);
        l_Data << uint32(l_SignCount);

        for (uint8 l_I = 1; l_I <= l_SignCount; ++l_I)
        {
            Field* l_SignFields = p_Result->Fetch();
            uint32 l_LowGUID = l_PlayerGuids[l_I - 1];

            uint64 l_SignerGUID = MAKE_NEW_GUID(l_LowGUID, 0, HIGHGUID_PLAYER);

            l_Data.appendPackGUID(l_SignerGUID);
            l_Data << uint32(0); ///< Signature Choice

            p_Result->NextRow();
        }

        l_TargetPlayer->GetSession()->SendPacket(&l_Data);
    });
}

void WorldSession::HandleTurnInPetitionOpcode(WorldPacket& p_Packet)
{
    // Get petition guid from packet
    uint64 l_PetitionGuid = 0;

    p_Packet.readPackGUID(l_PetitionGuid);

    // Check if player really has the required petition charter
    Item* item = m_Player->GetItemByGuid(l_PetitionGuid);
    if (!item)
        return;

    uint64 l_PlayerGuid = m_Player->GetGUID();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION);
    stmt->setUInt32(0, GUID_LOPART(l_PetitionGuid));
    CharacterDatabase.AsyncQuery(stmt, m_Player->GetQueryCallbackMgr(), [l_PetitionGuid, l_PlayerGuid](PreparedQueryResult const& p_Result)
    {
        // Get petition data from db
        uint32 ownerguidlo;
        uint32 type;
        std::string name;

        Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
        if (!l_Player)
            return;

        if (p_Result)
        {
            Field* fields = p_Result->Fetch();
            ownerguidlo = fields[0].GetUInt32();
            name = fields[1].GetString();
            type = fields[2].GetUInt8(); ///< type is never read 01/18/16
        }
        else
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "Player %s (guid: %u) tried to turn in petition (guid: %u) that is not present in the database", l_Player->GetName(), l_Player->GetGUIDLow(), GUID_LOPART(l_PetitionGuid));
            return;
        }

        // Only the petition owner can turn in the petition
        if (l_Player->GetGUIDLow() != ownerguidlo)
            return;

        // Check if player is already in a guild
        if (l_Player->GetGuildId())
        {
            WorldPacket l_Data;
            l_Data.Initialize(SMSG_TURN_IN_PETITION_RESULTS, 4);
            l_Data.WriteBits(PETITION_TURN_ALREADY_IN_GUILD, 4);
            l_Data.FlushBits();
            l_Player->GetSession()->SendPacket(&l_Data);
            return;
        }

        // Check if guild name is already taken
        if (sGuildMgr->GetGuildByName(name))
        {
            Guild::SendCommandResult(l_Player->GetSession(), GUILD_CREATE_S, ERR_GUILD_NAME_EXISTS_S, name);
            return;
        }

        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);
        stmt->setUInt32(0, GUID_LOPART(l_PetitionGuid));
        CharacterDatabase.AsyncQuery(stmt, l_Player->GetQueryCallbackMgr(), [l_PlayerGuid, l_PetitionGuid, name](PreparedQueryResult const& p_Result)
        {
            // Get petition signatures from db
            uint8 signatures;

            if (p_Result)
                signatures = uint8(p_Result->GetRowCount());
            else
                signatures = 0;

            uint32 requiredSignatures;
            requiredSignatures = sWorld->getIntConfig(CONFIG_MIN_PETITION_SIGNS);

            Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGuid);
            if (!l_Player)
                return;

            // Notify player if signatures are missing
            if (signatures < requiredSignatures)
            {
                WorldPacket l_Data;
                l_Data.Initialize(SMSG_TURN_IN_PETITION_RESULTS, 4);
                l_Data.WriteBits(PETITION_TURN_NEED_MORE_SIGNATURES, 4);
                l_Data.FlushBits();
                l_Player->GetSession()->SendPacket(&l_Data);
                return;
            }

            // Proceed with guild/arena team creation
            Item* l_Item = l_Player->GetItemByGuid(l_PetitionGuid);
            if (!l_Item)
                return;

            // Delete charter item
            l_Player->DestroyItem(l_Item->GetBagSlot(), l_Item->GetSlot(), true);

            // Create guild
            Guild* guild = new Guild;

            if (!guild->Create(l_Player, name))
            {
                delete guild;
                return;
            }

            // Register guild and add guild master
            sGuildMgr->AddGuild(guild);

            // Add members from signatures
            for (uint8 i = 0; i < signatures; ++i)
            {
                Field* fields = p_Result->Fetch();
                guild->AddMember(MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER));
                p_Result->NextRow();
            }

            SQLTransaction trans = CharacterDatabase.BeginTransaction();

            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PETITION_BY_GUID);
            stmt->setUInt32(0, GUID_LOPART(l_PetitionGuid));
            trans->Append(stmt);

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PETITION_SIGNATURE_BY_GUID);
            stmt->setUInt32(0, GUID_LOPART(l_PetitionGuid));
            trans->Append(stmt);

            CharacterDatabase.CommitTransaction(trans);

            WorldPacket l_Data;
            l_Data.Initialize(SMSG_TURN_IN_PETITION_RESULTS, 4);
            l_Data.WriteBits(PETITION_TURN_OK, 4);
            l_Data.FlushBits();
            l_Player->GetSession()->SendPacket(&l_Data);
        });
    });
}

void WorldSession::HandlePetitionShowListOpcode(WorldPacket & p_Packet)
{
    uint64 l_PetitionUnitGUID = 0;
    p_Packet.readPackGUID(l_PetitionUnitGUID);

    SendPetitionShowList(l_PetitionUnitGUID);
}

void WorldSession::SendPetitionShowList(uint64 p_GUID)
{
    Creature * l_Creature = GetPlayer()->GetNPCIfCanInteractWith(p_GUID, UNIT_NPC_FLAG_PETITIONER);

    if (!l_Creature)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: HandlePetitionShowListOpcode - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(p_GUID)));
        return;
    }

    WorldPacket l_Data(SMSG_PETITION_SHOW_LIST, 8 + 1 + 4 * 6);
    l_Data.appendPackGUID(p_GUID);
    l_Data << uint32(GUILD_CHARTER_COST);                 // charter cost

    SendPacket(&l_Data);
}

void WorldSession::SendNotInArenaTeamPacket(uint8 type)
{
}

void WorldSession::SendPetitionSignResult(uint64 p_PlayerGUID, uint64 p_ItemGUID, uint8 p_Result)
{
    WorldPacket l_Data(SMSG_PETITION_SIGN_RESULTS);

    l_Data.appendPackGUID(p_ItemGUID);
    l_Data.appendPackGUID(p_PlayerGUID);
    l_Data.WriteBits(PetitionSigns(p_Result), 4);
    l_Data.FlushBits();

    SendPacket(&l_Data);
}

void WorldSession::SendAlreadySigned(uint64 p_PlayerGUID)
{
    WorldPacket l_Data(SMSG_PETITION_ALREADY_SIGNED);
    l_Data.appendPackGUID(p_PlayerGUID);

    SendPacket(&l_Data);
}

void WorldSession::SendPetitionDeclined(uint64 p_PlayerGuid)
{
    WorldPacket l_Data(SMSG_PETITION_DECLINED);
    l_Data.appendPackGUID(p_PlayerGuid);    ///< Decliner

    SendPacket(&l_Data);
}
#endif
