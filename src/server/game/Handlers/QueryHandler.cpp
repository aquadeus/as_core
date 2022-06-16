////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Language.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "UpdateMask.h"
#include "NPCHandler.h"
#include "Pet.h"
#include "MapManager.h"
#include "InstanceScript.h"

enum NameQueryResponse
{
    NAME_QUERY_RESULT_OK    = 0,
    NAME_QUERY_RESULT_DENY  = 1,
    NAME_QUERY_RESULT_RETRY = 2,
};

void WorldSession::SendNameQueryOpcode(uint64 guid, bool atLeave)
{
#ifndef CROSS
    Player* player = ObjectAccessor::FindPlayer(guid);
    CharacterInfo  const* nameData = sWorld->GetCharacterInfo(GUID_LOPART(guid));
#else /* CROSS */
    Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(guid);
    InterRealmClient* playerClient = player ? player->GetSession()->GetInterRealmClient() : nullptr;
#endif /* CROSS */

    WorldPacket data(SMSG_NAME_QUERY_RESPONSE);

#ifndef CROSS
    data << uint8(nameData ? NAME_QUERY_RESULT_OK : NAME_QUERY_RESULT_DENY);
#else /* CROSS */
    data << uint8(playerClient ? NAME_QUERY_RESULT_OK : NAME_QUERY_RESULT_DENY);
#endif /* CROSS */
    data.appendPackGUID(guid);

#ifndef CROSS
    if (nameData)
#else /* CROSS */
    if (playerClient)
#endif /* CROSS */
    {
        data.WriteBit(false);   ///< Is deleted
#ifndef CROSS
        data.WriteBits(nameData->Name.size(), 6);
#else /* CROSS */
        data.WriteBits(strlen(player->GetName()), 6);
#endif /* CROSS */

        if (DeclinedName const* names = (player ? player->GetDeclinedNames() : NULL))
        {
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                data.WriteBits(names->name[i].size(), 7);
        }
        else
        {
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                data.WriteBits(0, 7);
        }

        data.FlushBits();

        if (DeclinedName const* names = (player ? player->GetDeclinedNames() : NULL))
        {
            for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
                if (names->name[i].size())
                    data.append(names->name[i].c_str(), names->name[i].size());
        }

        data.appendPackGUID(player ? player->GetSession()->GetWoWAccountGUID()  : 0);
        data.appendPackGUID(player ? player->GetSession()->GetBNetAccountGUID() : 0);
        data.appendPackGUID(guid);

#ifndef CROSS
        data << uint32(g_RealmID);
        data << uint8(nameData->Race);
        data << uint8(nameData->Sex);
        data << uint8(nameData->Class);
        data << uint8(nameData->Level);
#else /* CROSS */
        data << uint32(playerClient->GetRealmId());
        data << uint8(player->getRace());
        data << uint8(player->getGender());
        data << uint8(player->getClass());
        data << uint8(player->getLevel());
#endif /* CROSS */

#ifndef CROSS
        data.WriteString(nameData->Name);
#else /* CROSS */
        data.WriteString(player->GetName());
#endif /* CROSS */
    }

    SendPacket(&data);
}

void WorldSession::HandleNameQueryOpcode(WorldPacket& recvData)
{
    uint64 guid;
    recvData.readPackGUID(guid);

    SendNameQueryOpcode(guid);
}

void WorldSession::HandleQueryTimeOpcode(WorldPacket& /*recvData*/)
{
    SendQueryTimeResponse();
}

void WorldSession::SendQueryTimeResponse()
{
    WorldPacket l_Data(SMSG_QUERY_TIME_RESPONSE, 4);
    l_Data << uint32(time(nullptr));
    SendPacket(&l_Data);
}

/// Only _static_ data is sent in this packet !!!
void WorldSession::HandleCreatureQueryOpcode(WorldPacket& recvData)
{
    uint32 entry;
    recvData >> entry;

    if (CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(entry))
    {
        std::string Name[MAX_CREATURE_NAMES];
        std::string NameAlt[MAX_CREATURE_NAMES];
        std::string Title = creatureInfo->Title;
        std::string TitleAlt = creatureInfo->TitleAlt;

        for (uint32 i = 0; i < MAX_CREATURE_NAMES; ++i)
        {
            Name[i] = creatureInfo->Name[i];
            NameAlt[i] = creatureInfo->NameAlt[i];
        }

        LocaleConstant locale = GetSessionDbLocaleIndex();
        if (locale >= 0)
        {
            if (CreatureLocale const* creatureLocale = sObjectMgr->GetCreatureLocale(entry))
            {
                for (uint32 i = 0; i < MAX_CREATURE_NAMES; ++i)
                {
                    ObjectMgr::GetLocaleString(creatureLocale->Name[i], locale, Name[i]);
                    ObjectMgr::GetLocaleString(creatureLocale->NameAlt[i], locale, NameAlt[i]);
                }

                ObjectMgr::GetLocaleString(creatureLocale->Title, locale, Title);
                ObjectMgr::GetLocaleString(creatureLocale->TitleAlt, locale, TitleAlt);
            }
        }

        uint8 itemCount = 0;
        for (uint32 i = 0; i < MAX_CREATURE_QUEST_ITEMS; ++i)
            if (creatureInfo->questItems[i])
                itemCount++;                                           ///< itemId[6], quest drop

        WorldPacket data(SMSG_QUERY_CREATURE_RESPONSE, 1 * 1024);

        data << uint32(entry);
        data.WriteBit(creatureInfo != nullptr);
        data.FlushBits();

        data.WriteBits(Title.size() ? Title.size() + 1 : 0, 11);
        data.WriteBits(TitleAlt.size() ? TitleAlt.size() + 1 : 0, 11);
        data.WriteBits(creatureInfo->CursorName.size() ? creatureInfo->CursorName.size() + 1 : 0, 6);
        data.WriteBit(creatureInfo->RacialLeader);

        for (uint32 i = 0; i < MAX_CREATURE_NAMES; ++i)
        {
            data.WriteBits(Name[i].size() ? Name[i].size() + 1 : 0, 11);
            data.WriteBits(NameAlt[i].size() ? NameAlt[i].size() + 1 : 0, 11);
        }

        data.FlushBits();

        for (uint32 i = 0; i < MAX_CREATURE_NAMES; ++i)
        {
            if (!Name[i].empty())
                data << Name[i];

            if (!NameAlt[i].empty())
                data << NameAlt[i];
        }

        data << uint32(creatureInfo->type_flags);
        data << uint32(creatureInfo->type_flags2);
        data << uint32(creatureInfo->type);
        data << uint32(creatureInfo->family);
        data << uint32(creatureInfo->rank);
        data << uint32(creatureInfo->KillCredit[0]);
        data << uint32(creatureInfo->KillCredit[1]);

        for (uint32 i = 0; i < MAX_CREATURE_MODELS; ++i)
            data << uint32(creatureInfo->Modelid[i]);

        data << float(creatureInfo->ModHealth);
        data << float(creatureInfo->ModMana);
        data << uint32(itemCount);
        data << uint32(creatureInfo->movementId);
        data << uint32(creatureInfo->expansion);
        data << uint32(creatureInfo->RequiredExpansion);
        data << uint32(creatureInfo->VignetteID);

        if (!Title.empty())
            data << Title;

        if (!TitleAlt.empty())
            data << TitleAlt;

        if (!creatureInfo->CursorName.empty())
            data << creatureInfo->CursorName;

        for (uint32 i = 0; i < MAX_CREATURE_QUEST_ITEMS && itemCount > 0; ++i)
        {
            if (creatureInfo->questItems[i])
            {
                data << uint32(creatureInfo->questItems[i]);
                itemCount--;
            }
        }

        SendPacket(&data);
    }
    else
    {
        WorldPacket data(SMSG_QUERY_CREATURE_RESPONSE, 4 + 1);
        data << uint32(entry);
        data.WriteBit(0);                               ///< Has no valid data
        data.FlushBits();

        SendPacket(&data);
    }
}

/// Only _static_ data is sent in this packet !!!
void WorldSession::HandleGameObjectQueryOpcode(WorldPacket& recvData)
{
    uint32 l_GobEntry;
    uint64 l_GobGUID;

    recvData >> l_GobEntry;
    recvData.readPackGUID(l_GobGUID);

    ByteBuffer l_GobData(2 * 1024);

    if (const GameObjectTemplate* l_GobInfo = sObjectMgr->GetGameObjectTemplate(l_GobEntry))
    {
        std::string l_Name;
        std::string l_IconeName;
        std::string l_CastBarCaption;

        l_Name              = l_GobInfo->name;
        l_IconeName         = l_GobInfo->IconName;
        l_CastBarCaption    = l_GobInfo->castBarCaption;

        int l_LocaleIndex = GetSessionDbLocaleIndex();

        if (l_LocaleIndex >= 0)
        {
            if (GameObjectLocale const* l_GobLocale = sObjectMgr->GetGameObjectLocale(l_GobEntry))
            {
                ObjectMgr::GetLocaleString(l_GobLocale->Name, l_LocaleIndex, l_Name);
                ObjectMgr::GetLocaleString(l_GobLocale->CastBarCaption, l_LocaleIndex, l_CastBarCaption);
            }
        }

        l_GobData << uint32(l_GobInfo->type);
        l_GobData << uint32(l_GobInfo->displayId);
        l_GobData << l_Name;
        l_GobData << "";
        l_GobData << "";
        l_GobData << "";
        l_GobData << l_IconeName;                                       // 2.0.3, string. Icon name to use instead of default icon for go's (ex: "Attack" makes sword)
        l_GobData << l_CastBarCaption;                                  // 2.0.3, string. Text will appear in Cast Bar when using GO (ex: "Collecting")
        l_GobData << "";

        for (int i = 0; i < MAX_GAMEOBJECT_DATA; i++)
            l_GobData << uint32(l_GobInfo->raw.data[i]);

        l_GobData << float(l_GobInfo->size);                            // go size

        uint8 l_QuestItemCount = 0;

        for (uint32 i = 0; i < MAX_GAMEOBJECT_QUEST_ITEMS; ++i)
            if (l_GobInfo->questItems[i])
                l_QuestItemCount++;

        l_GobData << uint8(l_QuestItemCount);

        for (int i = 0; i < MAX_GAMEOBJECT_QUEST_ITEMS && l_QuestItemCount > 0; i++)
        {
            if (l_GobInfo->questItems[i])
            {
                l_GobData << uint32(l_GobInfo->questItems[i]);          // itemId[6], quest drop
                l_QuestItemCount--;
            }
        }

        l_GobData << uint32(l_GobInfo->RequiredLevel);
    }

    WorldPacket l_Response(SMSG_GAMEOBJECT_QUERY_RESPONSE, 4 + 1 + 4 + l_GobData.size());

    l_Response << uint32(l_GobEntry);

    l_Response.WriteBit(l_GobData.size() != 0);
    l_Response.FlushBits();

    l_Response << uint32(l_GobData.size());

    l_Response.append(l_GobData);

    SendPacket(&l_Response);
}

void WorldSession::HandleCorpseLocationFromClientQueryOpcode(WorldPacket& p_RecvData)
{
    uint64 l_Guid;
    p_RecvData.readPackGUID(l_Guid);

    SendCorpseLocation(l_Guid);
}

void WorldSession::SendCorpseLocation(uint64 p_Guid)
{
    Player* l_Player = sObjectAccessor->FindPlayer(p_Guid);

    if (!l_Player || !l_Player->GetCorpse() || !l_Player->IsInSameRaidWith(l_Player))
    {
        WorldPacket l_Data(SMSG_CORPSE_LOCATION);

        l_Data.WriteBit(0);                                 ///< Valid
        l_Data.FlushBits();

        l_Data.appendPackGUID(p_Guid);                      ///< Guid
        l_Data << uint32(0);                                ///< Map ID
        l_Data << float(0);                                 ///< X
        l_Data << float(0);                                 ///< Y
        l_Data << float(0);                                 ///< Z
        l_Data << uint32(0);                                ///< Actual map id
        l_Data.appendPackGUID(0);                           ///< Transport

        SendPacket(&l_Data);

        return;
    }

    Corpse* l_Corpse = l_Player->GetCorpse();

    uint32 l_MapID = l_Corpse->GetMapId();
    uint32 l_CorpseMapID = l_MapID;
    float l_X = l_Corpse->GetTransport() ? l_Corpse->GetTransOffsetX() : l_Corpse->GetPositionX();
    float l_Y = l_Corpse->GetTransport() ? l_Corpse->GetTransOffsetY() : l_Corpse->GetPositionY();
    float l_Z = l_Corpse->GetTransport() ? l_Corpse->GetTransOffsetZ() : l_Corpse->GetPositionZ();

    /// If corpse at different map
    if (l_MapID != l_Player->GetMapId())
    {
        /// Search entrance map for proper show entrance
        if (const MapEntry * l_CorpseMapEntry = sMapStore.LookupEntry(l_MapID))
        {
            if (l_CorpseMapEntry->IsDungeon() && l_CorpseMapEntry->CorpseMapID >= 0)
            {
                /// If corpse map have entrance
                uint32 l_Zone = sMapMgr->GetZoneId(l_CorpseMapEntry->CorpseMapID, l_CorpseMapEntry->CorpseX, l_CorpseMapEntry->CorpseY, l_Z);
                if (Map const* entranceMap = sMapMgr->CreateBaseMap(l_CorpseMapEntry->CorpseMapID, l_Zone))
                {
                    l_MapID = l_CorpseMapEntry->CorpseMapID;
                    l_X = l_CorpseMapEntry->CorpseX;
                    l_Y = l_CorpseMapEntry->CorpseY;
                    l_Z = entranceMap->GetHeight(l_Player->GetPhaseMask(), l_X, l_Y, MAX_HEIGHT);
                }
            }
        }
    }

    WorldPacket l_Data(SMSG_CORPSE_LOCATION);

    l_Data.WriteBit(1);                                 ///< Valid
    l_Data.FlushBits();

    l_Data.appendPackGUID(p_Guid);                      ///< Guid
    l_Data << uint32(l_CorpseMapID);                    ///< Map ID
    l_Data << float(l_X);                               ///< X
    l_Data << float(l_Y);                               ///< Y
    l_Data << float(l_Z);                               ///< Z
    l_Data << uint32(l_MapID);                          ///< Actual map id
    l_Data.appendPackGUID(l_Corpse->GetTransGUID());    ///< Transport

    SendPacket(&l_Data);
}

void WorldSession::HandleCemeteryListOpcode(WorldPacket& /*recvData*/)
{
    GetPlayer()->SendCemeteryList(false);
}

void WorldSession::HandleForcedReactionsOpcode(WorldPacket& /*recvData*/)
{
    GetPlayer()->GetReputationMgr().SendForceReactions();
}

#define DEFAULT_GREETINGS_GOSSIP      68

void WorldSession::HandleNpcTextQueryOpcode(WorldPacket& p_Packet)
{
    uint32 l_TextID;
    uint64 l_Guid;

    p_Packet >> l_TextID;
    p_Packet.readPackGUID(l_Guid);

    //GetPlayer()->SetSelection(l_Guid);

    //////////////////////////////////////////////////////////////////////////

    GossipText const* l_Gossip = sObjectMgr->GetGossipText(l_TextID);

    if (!l_Gossip)
    {
        l_Gossip = sObjectMgr->GetGossipText(DEFAULT_GREETINGS_GOSSIP);
        l_TextID = DEFAULT_GREETINGS_GOSSIP;
    }

    bool l_IsLegacy = l_Gossip->BroadcastTextID[0] == 0;

    //////////////////////////////////////////////////////////////////////////

    bool l_Allow = true;

    WorldPacket l_Data(SMSG_NPC_TEXT_UPDATE, 100);
    l_Data << l_TextID;
    l_Data.WriteBit(l_Allow);
    l_Data.FlushBits();

    //////////////////////////////////////////////////////////////////////////

    ByteBuffer l_Buffer;

    if (l_Gossip->BroadcastTextID[0] != 0)
    {
        /// Text probability
        for (size_t l_I = 0; l_I < MAX_GOSSIP_TEXT_OPTIONS; l_I++)
            l_Buffer << float(l_Gossip->Probability[l_I]);

        /// Text ID
        for (size_t l_I = 0; l_I < MAX_GOSSIP_TEXT_OPTIONS - 1; l_I++)
            l_Buffer << uint32(l_Gossip->BroadcastTextID[l_I]);
    }
    else
    {
        /// Text probability
        for (size_t l_I = 0; l_I < MAX_GOSSIP_TEXT_OPTIONS; l_I++)
            l_Buffer << float(0);

        /// Text ID
        l_Buffer << uint32(l_TextID | LEGACY_BROADCAST_ENTRY_FLAG);
        for (size_t l_I = 0; l_I < MAX_GOSSIP_TEXT_OPTIONS - 1; l_I++)
            l_Buffer << uint32(0);
    }

    //////////////////////////////////////////////////////////////////////////

    l_Data << uint32(l_Buffer.size());
    l_Data.append(l_Buffer);

    SendPacket(&l_Data);
}

void WorldSession::SendBroadcastTextDb2Reply(uint32 p_Entry)
{
    ByteBuffer l_ReplyContent;

    WorldPacket data(SMSG_DB_REPLY, 2 * 1024);
    data << uint32(DB2_REPLY_BROADCAST_TEXT);
    data << uint32(p_Entry);
    data << uint32(time(nullptr));
    data.WriteBit(1);                                                         ///< Found ???
    data.FlushBits();

    //////////////////////////////////////////////////////////////////////////

    GossipText const* l_Gossip = sObjectMgr->GetGossipText(p_Entry & ~LEGACY_BROADCAST_ENTRY_FLAG);

    if (!l_Gossip)
        l_Gossip = sObjectMgr->GetGossipText(DEFAULT_GREETINGS_GOSSIP);

    std::string l_DefaultText = "Greetings, $N";
    std::string l_Text1 = l_Gossip ? l_Gossip->Text_0 : l_DefaultText;
    std::string l_Text2 = l_Gossip ? l_Gossip->Text_1 : l_DefaultText;

    int l_LocaleIndex = GetSessionDbLocaleIndex();
    if (l_LocaleIndex >= 0 && l_LocaleIndex <= MAX_LOCALES)
    {
        if (NpcTextLocale const* l_NpcTextLocale = sObjectMgr->GetNpcTextLocale(p_Entry & ~LEGACY_BROADCAST_ENTRY_FLAG))
        {
            ObjectMgr::GetLocaleString(l_NpcTextLocale->Text_0[0], l_LocaleIndex, l_Text1);
            ObjectMgr::GetLocaleString(l_NpcTextLocale->Text_1[0], l_LocaleIndex, l_Text2);
        }
    }

    l_ReplyContent << std::string(l_Text1);
    l_ReplyContent << std::string(l_Text2);

    if (l_Gossip)
    {
        for (int l_J = 0; l_J < MAX_GOSSIP_TEXT_EMOTES; ++l_J)
            l_ReplyContent << uint16(l_Gossip->Emotes[l_J]._Emote);
        for (int l_J = 0; l_J < MAX_GOSSIP_TEXT_EMOTES; ++l_J)
            l_ReplyContent << uint16(l_Gossip->Emotes[l_J]._Delay);
    }
    else
    {
        for (int l_J = 0; l_J < MAX_GOSSIP_TEXT_EMOTES; ++l_J)
        {
            l_ReplyContent << uint16(0);
            l_ReplyContent << uint16(0);
        }
    }

    l_ReplyContent << uint16(0);                                                                ///< UnkEmoteID
    l_ReplyContent << uint8(l_Gossip != 0 ? l_Gossip->Language : LANG_UNIVERSAL);               ///< Language
    l_ReplyContent << uint8(0x01);                                                              ///< Type

    /// Try to send retail sound ID with retail data from BroadcastTextID
    l_ReplyContent << uint32(l_Gossip->SoundID);                                                ///< Gossip sound male
    l_ReplyContent << uint32(l_Gossip->SoundID);                                                ///< Gossip sound female

    l_ReplyContent << uint32(0);                                                                ///< Player condition ID

    //////////////////////////////////////////////////////////////////////////

    data << uint32(l_ReplyContent.size());
    data.append(l_ReplyContent);

    SendPacket(&data);
}

/// Only _static_ data is sent in this packet !!!
void WorldSession::HandlePageTextQueryOpcode(WorldPacket& p_Packet)
{
    uint64 l_ItemGUID   = 0;
    uint32 l_PageTextID = 0;

    p_Packet >> l_PageTextID;
    p_Packet.readPackGUID(l_ItemGUID);

    PageText const* l_PageText = sObjectMgr->GetPageText(l_PageTextID);
    uint32 l_Count = 0;

    WorldPacket l_Data(SMSG_PAGE_TEXT_QUERY_RESPONSE, 2 * 1024);
    l_Data << uint32(l_PageTextID);                             ///< Page Text ID
    l_Data.WriteBit(l_PageText != nullptr);                     ///< Allow
    l_Data.FlushBits();

    size_t l_Pos = l_Data.wpos();
    l_Data << uint32(0);

    while (l_PageTextID)
    {
        l_PageText = sObjectMgr->GetPageText(l_PageTextID);

        if (l_PageText)
        {
            std::string l_Text = l_PageText->Text;

            int l_Locale = GetSessionDbLocaleIndex();
            if (l_Locale >= 0)
            {
                if (PageTextLocale const* l_PageTxtLocale = sObjectMgr->GetPageTextLocale(l_PageTextID))
                    ObjectMgr::GetLocaleString(l_PageTxtLocale->Text, l_Locale, l_Text);
            }

            l_Data << uint32(l_PageTextID);                         ///< ID
            l_Data << uint32(l_PageText->NextPageID);               ///< NextPageID
            l_Data << uint32(l_PageText->m_PlayerConditionID);
            l_Data << uint8(l_PageText->m_Flags);

            l_Data.WriteBits(l_Text.size(), 12);                    ///< Text
            l_Data.FlushBits();

            l_Data.WriteString(l_Text);                             ///< Text
        }

        if (l_PageText)
            l_PageTextID = l_PageText->NextPageID;
        else
            l_PageTextID = 0;

        ++l_Count;
    }

    l_Data.put<uint32>(l_Pos, l_Count);
    SendPacket(&l_Data);
}

void WorldSession::HandleCorpseTransportQueryOpcode(WorldPacket & p_Packet)
{
    uint64 l_TransportGUID = 0;
    uint64 l_PlayerGUID = 0;

    p_Packet.readPackGUID(l_PlayerGUID);
    p_Packet.readPackGUID(l_TransportGUID);

    GameObject* l_TransportGOB = HashMapHolder<GameObject>::Find(l_TransportGUID);
    Player* l_Player = sObjectAccessor->GetPlayer(*m_Player, l_PlayerGUID);
    Corpse* l_Corpse = nullptr;

    WorldLocation l_Loc;

    if (l_Player)
        l_Player->GetCorpse();

    if (l_Player && GetPlayer()->IsInSameRaidWith(l_Player) && l_Corpse && !l_Corpse->GetTransGUID() && l_Corpse->GetTransGUID() == l_TransportGUID)
        l_Loc.Relocate(l_TransportGOB);

    WorldPacket data(SMSG_CORPSE_TRANSPORT_QUERY, 4+4+4+4);
    data.appendPackGUID(l_PlayerGUID);
    data << float(l_Loc.GetPositionX());
    data << float(l_Loc.GetPositionY());
    data << float(l_Loc.GetPositionZ());
    data << float(l_Loc.GetOrientation());
    SendPacket(&data);
}

void WorldSession::HandleQuestPOIQuery(WorldPacket& p_Packet)
{
    uint32 l_Count;

    p_Packet >> l_Count;

    if (l_Count >= MAX_QUEST_LOG_SIZE)
    {
        p_Packet.rfinish();
        return;
    }

    std::list<uint32> l_QuestList;

    for (uint32 i = 0; i < l_Count; ++i)
        l_QuestList.push_back(p_Packet.read<uint32>());

    p_Packet.rfinish();

    WorldPacket l_Data(SMSG_QUEST_POIQUERY_RESPONSE, 15 * 1024);
    l_Data << uint32(l_Count); // count
    l_Data << uint32(l_Count);

    for (auto l_CurrentQuest : l_QuestList)
    {
        uint32 l_QuestID = l_CurrentQuest;
        uint16 l_QuestSlot = m_Player->FindQuestSlot(l_QuestID);
        bool l_QuestOK = false;

        if (l_QuestSlot != MAX_QUEST_LOG_SIZE)
            l_QuestOK =m_Player->GetQuestSlotQuestId(l_QuestSlot) == l_QuestID;

        if (l_QuestOK)
        {
            QuestPOIVector const* l_POIs = sObjectMgr->GetQuestPOIVector(l_QuestID);

            if (l_POIs)
            {
                l_Data << uint32(l_QuestID);
                l_Data << uint32(l_POIs->size());     ///< Num Blobs

                const Quest * l_Quest = sObjectMgr->GetQuestTemplate(l_QuestID);

                for (QuestPOIVector::const_iterator l_It = l_POIs->begin(); l_It != l_POIs->end(); ++l_It)
                {
                    const QuestObjective * l_Objective = l_Quest->GetQuestObjectiveXIndex(l_It->ObjectiveIndex);

                    uint32 l_QuestObjectiveID   = 0;
                    uint32 l_QuestObjectID      = 0;

                    if (l_Objective)
                    {
                        l_QuestObjectiveID = l_Objective->ID;
                        l_QuestObjectID = l_Objective->ObjectID;
                    }

                    l_Data << uint32(l_It->BlobIndex);            ///< BlobIndex
                    l_Data << int32(l_It->ObjectiveIndex);        ///< Objective Index
                    l_Data << uint32(l_QuestObjectiveID);         ///< Quest Objective ID
                    l_Data << uint32(l_QuestObjectID);            ///< Quest Object ID
                    l_Data << uint32(l_It->MapID);                ///< Map ID
                    l_Data << uint32(l_It->WorldMapAreaID);       ///< World Map Area ID
                    l_Data << uint32(l_It->Floor);                ///< Floor
                    l_Data << uint32(l_It->Priority);             ///< Priority
                    l_Data << uint32(l_It->Flags);                ///< Flags
                    l_Data << uint32(l_It->WorldEffectID);        ///< World Effect ID
                    l_Data << uint32(l_It->PlayerConditionID);    ///< Player Condition ID
                    l_Data << uint32(l_It->UnkWoD1);              ///< Unk Wod
                    l_Data << uint32(l_It->points.size());

                    for (std::vector<QuestPOIPoint>::const_iterator l_PointIT = l_It->points.begin(); l_PointIT != l_It->points.end(); ++l_PointIT)
                    {
                        l_Data << int32(l_PointIT->X);            ///< X
                        l_Data << int32(l_PointIT->Y);            ///< Y
                    }

                    l_Data.WriteBit(l_It->AlwaysAllowMergingBlobs);  ///< Unk 7.3.5
                    l_Data.FlushBits();

                }
            }
            else
            {
                l_Data << uint32(l_QuestID);  ///< Quest ID
                l_Data << uint32(0);          ///< Num Blobs
            }
        }
        else
        {
            l_Data << uint32(l_QuestID);      ///< Quest ID
            l_Data << uint32(0);              ///< Num Blobs
        }
    }

    SendPacket(&l_Data);
}

void WorldSession::HandleDBQueryBulk(WorldPacket& p_RecvPacket)
{
    uint32 l_Type = 0;
    uint32 l_Count = 0;

    p_RecvPacket >> l_Type;
    l_Count = p_RecvPacket.ReadBits(13);

    for (uint32 l_I = 0; l_I < l_Count; ++l_I)
    {
        uint64 l_GUID;
        uint32 l_Entry;

        p_RecvPacket.readPackGUID(l_GUID);
        p_RecvPacket >> l_Entry;

        /// Specific case, localized string not yet supported by the system
        if (l_Type == DB2_REPLY_BROADCAST_TEXT && (l_Entry & LEGACY_BROADCAST_ENTRY_FLAG) != 0)
            SendBroadcastTextDb2Reply(l_Entry);
        else
            m_HotfixQueue.push(std::make_pair(l_Type, l_Entry));
    }
}

void WorldSession::SendAvailableHotfixes()
{
    auto& l_HotfixData = sObjectMgr->GetHotfixData();

    WorldPacket l_Packet(SMSG_AVAILABLE_HOTFIXES);
    l_Packet << uint32(1);                              ///< HotfixCacheVersion
    l_Packet << uint32(l_HotfixData.size());            ///< HotfixCount

    for (auto& l_Hotfix : l_HotfixData)
        l_Packet << uint64((uint64)l_Hotfix.Type << 32 | (uint64)l_Hotfix.Entry);

    SendPacket(&l_Packet);
}

void WorldSession::HandleHotfixRequest(WorldPacket& p_RecvPacket)
{
    uint32 l_HotfixCount = p_RecvPacket.read<uint32>();

    if (!l_HotfixCount || l_HotfixCount > sObjectMgr->GetHotfixData().size())
        return;

    WorldPacket l_Packet(SMSG_HOTFIX_RESPONSE);
    l_Packet << uint32(l_HotfixCount);

    uint32 l_Count = 0;

    for (uint32 l_I = 0; l_I < l_HotfixCount; l_I++)
    {
        uint64 l_HotfixID = p_RecvPacket.read<uint64>();
        int32 l_Entry    = l_HotfixID & 0xFFFFFFFF;
        uint32 l_Type    = (l_HotfixID >> 32) & 0xFFFFFFFF;

        l_Packet.FlushBits();

        auto l_Itr = sDB2PerHash.find(l_Type);
        if (l_Itr == sDB2PerHash.end())
            continue;                             ///< Malformed packet? Hack attempt?

        DB2StorageBase* l_DB2Store = l_Itr->second;

        if (l_DB2Store)
        {
            ByteBuffer l_ResponseData(2 * 1024);
            bool l_Allowed = l_Entry & HOTFIX_REMOVE_ROW_ENTRY_FLAG ? false : l_DB2Store->WriteRecord(l_Entry, l_ResponseData, GetSessionDbLocaleIndex(), sObjectMgr->HotfixTableIndexInfos[l_Type]);

            l_Packet << uint64(l_HotfixID);
            l_Packet << uint32(l_Entry & 0x00FFFFFF);

            l_Packet.WriteBit(l_Allowed);
            l_Packet << uint32(l_ResponseData.size());

            if (l_ResponseData.size())
                l_Packet.append(l_ResponseData);

            l_Count++;
        }
    }

    l_Packet.put<uint32>(0, l_Count);

    SendPacket(&l_Packet);
}