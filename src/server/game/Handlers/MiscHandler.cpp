////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "POIMgr.hpp"
#include "Common.h"
#include "Language.h"
#include "DatabaseEnv.h"
#include "WorldPacket.h"
#include "QuestPackets.h"
#include "Opcodes.h"
#include "Log.h"
#include "CUFProfiles.h"
#include "Player.h"
#include "GossipDef.h"
#include "World.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "WorldSession.h"
#include "BigNumber.h"
#include "SHA1.h"
#include "UpdateData.h"
#include "LootMgr.h"
#include "Chat.h"
#include "zlib.h"
#include "ObjectAccessor.h"
#include "Object.h"
#include "Battleground.h"
#include "OutdoorPvP.h"
#include "Pet.h"
#include "SocialMgr.h"
#include "CellImpl.h"
#include "AccountMgr.h"
#include "Vehicle.h"
#include "CreatureAI.h"
#include "DBCEnums.h"
#include "ScriptMgr.h"
#include "MapManager.h"
#include "InstanceScript.h"
#include "GameObjectAI.h"
#include "Group.h"
#include "AccountMgr.h"
#include "Spell.h"
#include "BattlegroundMgr.hpp"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "ContributionMgr.hpp"
#ifndef CROSS
#include "TicketMgr.h"
#endif /* not CROSS */
#include "OutdoorPvP.h"
#include "OutdoorPvPMgr.h"
#ifndef CROSS
#include "../Garrison/GarrisonMgr.hpp"
#else /* CROSS */
#include "InterRealmMgr.h"
#endif /* CROSS */

#include "BattlegroundPacketFactory.hpp"

void WorldSession::HandlePingOpcode(WorldPacket & recvPacket)
{
}

void WorldSession::HandleRepopRequestOpcode(WorldPacket& p_RecvData)
{
    bool l_CheckInstance = p_RecvData.ReadBit(); ///< l_checkInstance is never read 01/18/16

    if (m_Player->isAlive() || m_Player->HasFlag(EPlayerFields::PLAYER_FIELD_PLAYER_FLAGS, PlayerFlags::PLAYER_FLAGS_GHOST))
        return;

    /// Silently return, client should display the error by itself
    if (m_Player->HasAuraType(AuraType::SPELL_AURA_PREVENT_RESURRECTION))
        return;

    InstanceScript* l_Instance = m_Player->GetInstanceScript();
    if (l_Instance && l_Instance->IsEncounterInProgress() && !l_Instance->IsReleaseAllowed())
    {
        m_Player->SendForcedDeathUpdate();
        return;
    }

    /// The world update order is sessions, players, creatures
    /// The netcode runs in parallel with all of these
    /// Creatures can kill players
    /// So if the server is lagging enough the player can
    /// Release spirit after he's killed but before he is updated
    if (m_Player->getDeathState() == DeathState::JUST_DIED)
    {
        sLog->outDebug(LogFilterType::LOG_FILTER_NETWORKIO, "HandleRepopRequestOpcode: got request after player %s(%d) was killed and before he was updated", m_Player->GetName(), m_Player->GetGUIDLow());
        m_Player->KillPlayer();
    }

    /// This is spirit release confirm?
    m_Player->RemovePet(nullptr, PetSlot::PET_SLOT_OTHER_PET, true, true);
    m_Player->BuildPlayerRepop();
    m_Player->RepopAtGraveyard();
}

void WorldSession::HandleGossipSelectOptionOpcode(WorldPacket& recvData)
{
    uint32 gossipListId;
    uint32 textId;
    uint64 guid;
    uint32 codeLen = 0;
    std::string code = "";

    recvData.readPackGUID(guid);
    recvData >> textId >> gossipListId;
    codeLen = recvData.ReadBits(8);
    code = recvData.ReadString(codeLen);

    Creature* unit = NULL;
    GameObject* go = NULL;
    Player* player = nullptr;
    Item* item = nullptr;

    if (IS_CRE_OR_VEH_GUID(guid))
    {
        unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
        if (!unit)
            return;
    }
    else if (IS_GAMEOBJECT_GUID(guid))
    {
        go = m_Player->GetMap()->GetGameObject(guid);

        if (!go)
            return;
    }
    else if (IS_PLAYER_GUID(guid))
    {
        player = sObjectAccessor->FindPlayer(guid);
        if (!player)
            return;
    }
    else if (IS_ITEM_GUID(guid))
    {
        item = m_Player->GetItemByGuid(guid);
        if (!item)
            return;
    }
    else
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if ((unit && unit->GetCreatureTemplate()->ScriptID != unit->LastUsedScriptID) || (go && go->GetGOInfo()->ScriptId != go->LastUsedScriptID))
    {
        /// Script reloaded while in use, ignoring and set new scipt id
        if (unit)
            unit->LastUsedScriptID = unit->GetCreatureTemplate()->ScriptID;
        if (go)
            go->LastUsedScriptID = go->GetGOInfo()->ScriptId;
        m_Player->PlayerTalkClass->SendCloseGossip();
        return;
    }

    uint32 menuId = m_Player->PlayerTalkClass->GetGossipMenu().GetMenuId();

    if (!code.empty())
    {
        if (unit)
        {
            unit->AI()->sGossipSelectCode(m_Player, menuId, gossipListId, code.c_str());
            if (!sScriptMgr->OnGossipSelectCode(m_Player, unit, m_Player->PlayerTalkClass->GetGossipOptionSender(gossipListId), m_Player->PlayerTalkClass->GetGossipOptionAction(gossipListId), code.c_str()))
                m_Player->OnGossipSelect(unit, gossipListId, menuId);
        }
        else if (go)
        {
            go->AI()->GossipSelectCode(m_Player, menuId, gossipListId, code.c_str());
            sScriptMgr->OnGossipSelectCode(m_Player, go, m_Player->PlayerTalkClass->GetGossipOptionSender(gossipListId), m_Player->PlayerTalkClass->GetGossipOptionAction(gossipListId), code.c_str());
        }
    }
    else
    {
        if (unit)
        {
            unit->AI()->sGossipSelect(m_Player, menuId, gossipListId);
            if (!sScriptMgr->OnGossipSelect(m_Player, unit, m_Player->PlayerTalkClass->GetGossipOptionSender(gossipListId), m_Player->PlayerTalkClass->GetGossipOptionAction(gossipListId)))
                m_Player->OnGossipSelect(unit, gossipListId, menuId);
        }
        else if (go)
        {
            go->AI()->GossipSelect(m_Player, menuId, gossipListId);
            if (!sScriptMgr->OnGossipSelect(m_Player, go, m_Player->PlayerTalkClass->GetGossipOptionSender(gossipListId), m_Player->PlayerTalkClass->GetGossipOptionAction(gossipListId)))
                m_Player->OnGossipSelect(go, gossipListId, menuId);
        }
        else if (item)
        {
            sScriptMgr->OnGossipSelect(m_Player, item, m_Player->PlayerTalkClass->GetGossipOptionSender(gossipListId), m_Player->PlayerTalkClass->GetGossipOptionAction(gossipListId));
        }
    }
}

void WorldSession::HandleWhoOpcode(WorldPacket& p_RecvData)
{
    time_t l_Now = time(NULL);
    if (l_Now - timeLastWhoCommand < 5)
        return;
    else
        timeLastWhoCommand = l_Now;

    uint32 l_MatchCount = 0;

    /// -  data
    uint32 l_MinLevel;
    uint32 l_MaxLevel;
    uint32 l_AreasCount;
    uint32 l_WordsCount;
    int64  l_RaceFilter;
    int32  l_ClassFilter;
    uint32 l_ZoneIDs[15];

    /// - Bool
    bool   l_Unk1;
    bool   l_Unk2;
    bool   l_Bit725;
    bool   l_HasWhoRequestServerInfo;

    /// - Player name & realm name
    uint8       l_NameLen;
    std::string l_Name;
    uint8       l_VirtualRealmNameLen;
    std::string l_VirtualRealmName;

    /// - Guild name & realm name
    uint8        l_GuildLen;
    std::string  l_Guild;
    uint8        l_GuildVirtualRealmNameLen;
    std::string  l_GuildVirtualRealmName;


    l_AreasCount               = p_RecvData.ReadBits(4);                    ///< area count, client limit = 10 (2.0.10)

    p_RecvData                 >> l_MinLevel;                               ///< maximal player level, default 0
    p_RecvData                 >> l_MaxLevel;                               ///< minimal player level, default 123 (MAX_LEVEL)
    p_RecvData                 >> l_RaceFilter;                             ///< race mask, default -1
    p_RecvData                 >> l_ClassFilter;                            ///< class mask, default -1

    l_NameLen                  = p_RecvData.ReadBits(6);                    ///< player name size
    l_VirtualRealmNameLen      = p_RecvData.ReadBits(9);                    ///< player realm size
    l_GuildLen                 = p_RecvData.ReadBits(7);                    ///< guild name size
    l_GuildVirtualRealmNameLen = p_RecvData.ReadBits(9);                    ///< guild realm
    l_WordsCount               = p_RecvData.ReadBits(3);                    ///< Words count

    l_Unk1                     = p_RecvData.ReadBit();                      ///< is never read 01/18/16
    l_Unk2                     = p_RecvData.ReadBit();                      ///< is never read 01/18/16
    l_Bit725                   = p_RecvData.ReadBit();                      ///< is never read 01/18/16
    l_HasWhoRequestServerInfo  = p_RecvData.ReadBit();                      ///< HasWhoRequestServerInfo

    p_RecvData.ResetBitReading();

    /// - Read player & guilds strings
    {
        if (l_NameLen > 0)
            l_Name = p_RecvData.ReadString(l_NameLen);

        if (l_VirtualRealmNameLen > 0)
            l_VirtualRealmName = p_RecvData.ReadString(l_VirtualRealmNameLen);

        if (l_GuildLen > 0)
            l_Guild = p_RecvData.ReadString(l_GuildLen);

        if (l_GuildVirtualRealmNameLen > 0)
            l_GuildVirtualRealmName = p_RecvData.ReadString(l_GuildVirtualRealmNameLen);
    }

    std::vector<uint8>        l_WordsLens(l_WordsCount, 0);
    std::vector<std::wstring> l_Words(l_WordsCount);

    /// - Fill Words
    for (uint8 l_WordCounter = 0; l_WordCounter < l_WordsCount; ++l_WordCounter)
    {
        l_WordsLens[l_WordCounter] = p_RecvData.ReadBits(7);
        p_RecvData.ResetBitReading();

        if (l_WordsLens[l_WordCounter] > 0)
        {
            std::string l_Temp = p_RecvData.ReadString(l_WordsLens[l_WordCounter]); // user entered string, it used as universal search pattern(guild+player name)?
            if (!Utf8toWStr(l_Temp, l_Words[l_WordCounter]))
                continue;

            wstrToLower(l_Words[l_WordCounter]);
        }
    }

    if (l_HasWhoRequestServerInfo)
    {
        int32  FactionGroup;
        int32  Locale;
        uint32 RequesterVirtualRealmAddress;

        p_RecvData >> FactionGroup >> Locale >> RequesterVirtualRealmAddress;
    }

    for (uint8 l_WordCounter = 0; l_WordCounter < l_AreasCount; ++l_WordCounter)
        p_RecvData >> l_ZoneIDs[l_WordCounter];

    std::wstring l_WQueryerPlayerName;
    std::wstring l_WQueryerPlayerGuildName;

    if (!(Utf8toWStr(l_Name, l_WQueryerPlayerName) && Utf8toWStr(l_Guild, l_WQueryerPlayerGuildName)))
        return;

    wstrToLower(l_WQueryerPlayerName);
    wstrToLower(l_WQueryerPlayerGuildName);

    /// Client send in case not set max level value 100 but Trinity supports 255 max level,
    /// update it to show GMs with characters after 100 level
    if (l_MaxLevel >= MAX_LEVEL)
        l_MaxLevel = STRONG_MAX_LEVEL;

    uint32 l_QueryerPlayerTeam = m_Player->GetTeam();
    uint32 l_Security = GetSecurity();
    bool l_AllowTwoSideWhoList = sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_WHO_LIST);
    uint32 l_GMLevelInWhoList  = sWorld->getIntConfig(CONFIG_GM_LEVEL_IN_WHO_LIST);
    uint8 l_MemberCount = 0;

    WorldPacket l_Data(SMSG_WHO, 5 * 1024);
    ByteBuffer l_Buffer(5 * 1024);

    TRINITY_READ_GUARD(HashMapHolder<Player>::LockType, *HashMapHolder<Player>::GetLock());
    HashMapHolder<Player>::MapType const& l_PlayersMap = sObjectAccessor->GetPlayers();

    for (HashMapHolder<Player>::MapType::const_iterator l_It = l_PlayersMap.begin(); l_It != l_PlayersMap.end(); ++l_It)
    {
        if (AccountMgr::IsPlayerAccount(l_Security))
        {
            /// Player can see member of other team only if CONFIG_ALLOW_TWO_SIDE_WHO_LIST
            if (l_It->second->GetTeam() != l_QueryerPlayerTeam && !l_AllowTwoSideWhoList)
                continue;

            /// Player can see MODERATOR, GAME MASTER, ADMINISTRATOR only if CONFIG_GM_IN_WHO_LIST
            if ((l_It->second->GetSession()->GetSecurity() > AccountTypes(l_GMLevelInWhoList)))
                continue;
        }

#ifdef CROSS
        /// Do not process players which are not in world
        if (!(l_It->second->IsInWorld()))
            continue;

#endif /* CROSS */
        /// check if target is globally visible for player
        if (!(l_It->second->IsVisibleGloballyFor(m_Player)))
            continue;

        uint32  l_PlayerClass   = l_It->second->getClass();
        uint32  l_PlayerRace    = l_It->second->getRace();
#ifndef CROSS
        uint32  l_AreaID        = l_It->second->GetSession()->GetInterRealmBG();

        if (!l_AreaID)
            l_AreaID = l_It->second->GetZoneId();

#else /* CROSS */
        uint32  l_AreaID        = l_It->second->GetZoneId();
#endif /* CROSS */
        uint8   l_PlayerLevel   = l_It->second->getLevel();
        uint8   l_PlayerSex     = l_It->second->getGender();

        /// Check if target's level is in level range
        if (l_PlayerLevel < l_MinLevel || l_PlayerLevel > l_MaxLevel)
            continue;

        /// Check if class matches classmask
        if (!(l_ClassFilter & (1 << l_PlayerClass)))
            continue;

        // check if race matches racemask
        if (!(l_RaceFilter & (uint64(1) << uint64(l_PlayerRace))))
            continue;

        bool l_ZoneShow = true;
        for (uint32 i = 0; i < l_AreasCount; ++i)
        {
            if (l_ZoneIDs[i] == l_AreaID)
            {
                l_ZoneShow = true;
                break;
            }

            l_ZoneShow = false;
        }

        if (!l_ZoneShow)
            continue;

        std::string  l_PlayerName = l_It->second->GetName();
        std::wstring l_WPlayerName;

        if (!Utf8toWStr(l_PlayerName, l_WPlayerName))
            continue;

        wstrToLower(l_WPlayerName);

        if (!(l_WQueryerPlayerName.empty() || l_WPlayerName.find(l_WQueryerPlayerName) != std::wstring::npos))
            continue;

#ifndef CROSS
        std::string  l_GuildName = sGuildMgr->GetGuildNameById(l_It->second->GetGuildId());
#else /* CROSS */
        InterRealmGuild* l_Guild = l_It->second->GetGuild();

        std::string  l_GuildName = l_Guild ? l_Guild->GetName() : "";
#endif /* CROSS */
        std::wstring l_WGuildName;

        if (!Utf8toWStr(l_GuildName, l_WGuildName))
            continue;

        wstrToLower(l_WGuildName);

        if (!(l_WQueryerPlayerGuildName.empty() || l_WGuildName.find(l_WQueryerPlayerGuildName) != std::wstring::npos))
            continue;

        std::string aname;
        if (AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(l_It->second->GetZoneId()))
            aname = areaEntry->AreaNameLang->Get(GetSessionDb2Locale());

        bool s_show = true;
        for (uint32 i = 0; i < l_WordsCount; ++i)
        {
            if (!l_Words[i].empty())
            {
                if (l_WGuildName.find(l_Words[i]) != std::wstring::npos ||
                    l_WPlayerName.find(l_Words[i]) != std::wstring::npos ||
                    Utf8FitTo(aname, l_Words[i]))
                {
                    s_show = true;
                    break;
                }
                s_show = false;
            }
        }
        if (!s_show)
            continue;

        /// 49 is maximum player count sent to client - can be overridden
        /// through config, but is unstable
        if ((l_MatchCount++) >= sWorld->getIntConfig(CONFIG_MAX_WHO))
        {
            if (sWorld->getBoolConfig(CONFIG_LIMIT_WHO_ONLINE))
                break;
            else
                continue;

            break;
        }

        uint64 l_GuildGUID = l_It->second->GetGuild() ? l_It->second->GetGuild()->GetGUID() : 0;

        l_Buffer.WriteBit(false);                                                                       ///< Is Deleted
        l_Buffer.WriteBits(l_PlayerName.size(), 6);                                                     ///< Name length

        if (DeclinedName const* l_DeclinedNames = l_It->second->GetDeclinedNames())
        {
            for (uint8 l_I = 0; l_I < MAX_DECLINED_NAME_CASES; ++l_I)
                l_Buffer.WriteBits(l_DeclinedNames->name[l_I].size(), 7);                               ///< DeclinedName[l_I] length
        }
        else
        {
            for (uint8 l_I = 0; l_I < MAX_DECLINED_NAME_CASES; ++l_I)                                   ///< DeclinedName[l_I] length
                l_Buffer.WriteBits(0, 7);
        }
        l_Buffer.FlushBits();

        if (DeclinedName const* l_DeclinedNames = l_It->second->GetDeclinedNames())
        {
            for (uint8 l_I = 0; l_I < MAX_DECLINED_NAME_CASES; ++l_I)
                l_Buffer.WriteString(l_DeclinedNames->name[l_I]);                                       ///< DeclinedName[l_I]
        }

        l_Buffer.appendPackGUID(l_It->second ? l_It->second->GetSession()->GetWoWAccountGUID() : 0);    ///< WoW account GUID
        l_Buffer.appendPackGUID(l_It->second ? l_It->second->GetSession()->GetBNetAccountGUID() : 0);   ///< BNet account GUID
        l_Buffer.appendPackGUID(l_It->second->GetGUID());                                               ///< Player GUID
        l_Buffer << uint32(g_RealmID);                                                                  ///< Virtual Realm Address
        l_Buffer << uint8(l_PlayerRace);                                                                ///< Race
        l_Buffer << uint8(l_PlayerSex);                                                                 ///< Sex
        l_Buffer << uint8(l_PlayerClass);                                                               ///< Class ID
        l_Buffer << uint8(l_PlayerLevel);                                                               ///< Level

        l_Buffer.WriteString(l_PlayerName);                                                             ///< Name

        l_Buffer.appendPackGUID(l_GuildGUID);                                                           ///< Guild GUID
        l_Buffer << uint32(g_RealmID);                                                                  ///< Guild Virtual Realm Address
        l_Buffer << uint32(l_AreaID);                                                                   ///< Area ID

        l_Buffer.WriteBits(l_GuildName.size(), 7);                                                      ///< Guild Name length
        l_Buffer.WriteBit(l_It->second->isGameMaster());                                                ///< Is Game Master
        l_Buffer.FlushBits();

        l_Buffer.WriteString(l_GuildName);                                                              ///< Guild Name

        ++l_MemberCount;
    }

    l_Data.WriteBits(l_MemberCount, 6);
    l_Data.FlushBits();
    l_Data.append(l_Buffer);

    SendPacket(&l_Data);
}

void WorldSession::HandleLogoutRequestOpcode(WorldPacket& /*recvData*/)
{
    if (uint64 lguid = GetPlayer()->GetLootGUID())
        DoLootRelease(lguid);

    uint32 reason = 0;

    if (GetPlayer()->isInCombat())
        reason = 1;
    else if (GetPlayer()->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR))
        reason = 3;                                         // is jumping or falling
    else if (GetPlayer()->m_Duel || GetPlayer()->InArena() ||GetPlayer()->HasAura(9454)) ///< is dueling, in arena or frozen by GM via freeze command
        reason = 2;                                         // FIXME - Need the correct value

    if (reason)
    {
        WorldPacket data(SMSG_LOGOUT_RESPONSE, 1+4);
        data << uint32(reason);
        data.WriteBit(0);
        data.FlushBits();
        SendPacket(&data);
        LogoutRequest(0);
        return;
    }

    // Instant logout in taverns/cities or on taxi or for admins, gm's, mod's if its enabled in worldserver.conf
    if (GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_RESTING) || GetPlayer()->isInFlight() ||
#ifndef CROSS
        GetSecurity() >= AccountTypes(sWorld->getIntConfig(CONFIG_INSTANT_LOGOUT)) || GetPlayer()->IsInDraenorGarrison() || IsPremium())
#else /* CROSS */
        GetSecurity() >= AccountTypes(sWorld->getIntConfig(CONFIG_INSTANT_LOGOUT)))
#endif /* CROSS */
    {
        /// avoid a duplication exploit with async queries in guild bank
        if ((m_TimeLastMoveBankGuildItems + 5) < time(nullptr))
        {
            WorldPacket data(SMSG_LOGOUT_RESPONSE, 1 + 4);
            data << uint32(reason);
            data.WriteBit(1);           // instant logout
            data.FlushBits();
            SendPacket(&data);

            SetforceExit(true);
            return;
        }
    }

    // not set flags if player can't free move to prevent lost state at logout cancel
    if (GetPlayer()->CanFreeMove())
    {
        GetPlayer()->SetStandState(UNIT_STAND_STATE_SIT);
        GetPlayer()->SetRooted(true);
        GetPlayer()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }

    WorldPacket data(SMSG_LOGOUT_RESPONSE, 1+4);
    data << uint32(0);
    data.WriteBit(0);
    data.FlushBits();
    SendPacket(&data);
    LogoutRequest(time(NULL));
}

void WorldSession::HandleLogoutCancelOpcode(WorldPacket& /*recvData*/)
{
    // Player have already logged out serverside, too late to cancel
    if (!GetPlayer())
        return;

    LogoutRequest(0);

    WorldPacket data(SMSG_LOGOUT_CANCEL_ACK, 0);
    SendPacket(&data);

    // not remove flags if can't free move - its not set in Logout request code.
    if (GetPlayer()->CanFreeMove())
    {
        //!we can move again
        GetPlayer()->SetRooted(false);

        //! Stand Up
        GetPlayer()->SetStandState(UNIT_STAND_STATE_STAND);

        //! DISABLE_ROTATE
        GetPlayer()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }

    GetPlayer()->PetSpellInitialize();
}

void WorldSession::HandleSetPvP(WorldPacket& recvData)
{
    // this opcode can be used in two ways: Either set explicit new status or toggle old status
    if (recvData.size() == 1)
    {
        bool newPvPStatus = recvData.ReadBit();

        GetPlayer()->ApplyModFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP, newPvPStatus);
        GetPlayer()->ApplyModFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_PVP_TIMER, !newPvPStatus);
    }
    else
    {
        GetPlayer()->ToggleFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
        GetPlayer()->ToggleFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_PVP_TIMER);
    }

    if (GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP))
    {
        if (!GetPlayer()->IsPvP() || GetPlayer()->pvpInfo.endTimer != 0)
            GetPlayer()->UpdatePvP(true, true);
    }
    else
    {
        if (!GetPlayer()->pvpInfo.inHostileArea && GetPlayer()->IsPvP())
            GetPlayer()->pvpInfo.endTimer = time(NULL);     // start toggle-off
    }

    //if (OutdoorPvP* pvp = _player->GetOutdoorPvP())
    //    pvp->HandlePlayerActivityChanged(_player);
}

void WorldSession::HandleZoneUpdateOpcode(WorldPacket& recvData)
{
    uint32 newZone;
    recvData >> newZone;

    // use server size data
    uint32 newzone, newarea;
    GetPlayer()->GetZoneAndAreaId(newzone, newarea, true);
    GetPlayer()->UpdateZone(newzone, newarea);
    //GetPlayer()->SendInitWorldStates(true, newZone);
}

void WorldSession::HandleReturnToGraveyard(WorldPacket& /*recvPacket*/)
{
    if (GetPlayer()->isAlive() || !GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    // Don't enable Return to Graveyard in Argus fight phase 3
    if (InstanceScript* l_Instance = m_Player->GetInstanceScript())
    {
        constexpr uint32 ANTORUS_RAID = 1712;
        constexpr uint32 ARGUS_DATA = 10;

        if (l_Instance->instance->GetId() == ANTORUS_RAID &&
            l_Instance->GetBossState(ARGUS_DATA) == EncounterState::IN_PROGRESS)
            return;
    }

    //TODO: unk32, unk32
    GetPlayer()->RepopAtGraveyard();
}

void WorldSession::HandleSetSelectionOpcode(WorldPacket& p_RecvData)
{
    uint64 l_NewTargetGuid;

    p_RecvData.readPackGUID(l_NewTargetGuid);

    m_Player->SetSelection(l_NewTargetGuid);
}

void WorldSession::HandleStandStateChangeOpcode(WorldPacket& recvData)
{
    uint32 l_StandState;
    recvData >> l_StandState;

    m_Player->SetStandState(l_StandState);
}

void WorldSession::HandleContactListOpcode(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    uint32 l_Flags;

    p_RecvData >> l_Flags;

#ifndef CROSS
    if (l_Flags & 1)
#else /* CROSS */
    if (l_Flags & 1 && m_Player->GetSocial())
#endif /* CROSS */
        m_Player->GetSocial()->SendSocialList(m_Player);
}

void WorldSession::HandleAddFriendOpcode(WorldPacket& p_RecvData)
{
    time_t l_Now = time(NULL);
    if (l_Now - m_TimeLastAddFriend < 2)
    {
        sLog->outAshran("WorldSession::HandleAddFriendOpcode acc %u", GetAccountId());
        return;
    }
    else
        m_TimeLastAddFriend = l_Now;

    std::string l_FriendName = GetTrinityString(LANG_FRIEND_IGNORE_UNKNOWN);
    std::string l_FriendNote;

    uint32 l_NameLen = p_RecvData.ReadBits(9);
    uint32 l_NoteLen = p_RecvData.ReadBits(10);

    l_FriendName = p_RecvData.ReadString(l_NameLen);
    l_FriendNote = p_RecvData.ReadString(l_NoteLen);

    if (!normalizePlayerName(l_FriendName))
        return;

    PreparedStatement* l_Stmt = SessionRealmDatabase.GetPreparedStatement(CHAR_SEL_GUID_RACE_ACC_BY_NAME);

    l_Stmt->setString(0, l_FriendName);
    l_Stmt->setString(1, l_FriendName);

    _addFriendCallback.SetParam(l_FriendNote);
    _addFriendCallback.SetFutureResult(SessionRealmDatabase.AsyncQuery(l_Stmt));
}

void WorldSession::HandleAddFriendOpcodeCallBack(PreparedQueryResult result, std::string friendNote)
{
    if (!GetPlayer())
        return;

    uint64 friendGuid;
    uint32 friendAccountId;
    uint32 team;
    FriendsResult friendResult;

    friendResult = FRIEND_NOT_FOUND;
    friendGuid = 0;

    if (result)
    {
        Field* fields = result->Fetch();

        friendGuid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
        team = Player::TeamForRace(fields[1].GetUInt8());
        friendAccountId = fields[2].GetUInt32();

        if (!AccountMgr::IsPlayerAccount(GetSecurity()) || sWorld->getBoolConfig(CONFIG_ALLOW_GM_FRIEND) || AccountMgr::IsPlayerAccount(sWorld->GetAccountSecurity(friendAccountId)))
        {
            if (friendGuid)
            {
                if (friendGuid == GetPlayer()->GetGUID())
                    friendResult = FRIEND_SELF;
                else if (GetPlayer()->GetTeam() != team && !sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND) && AccountMgr::IsPlayerAccount(GetSecurity()))
                    friendResult = FRIEND_ENEMY;
#ifndef CROSS
                else if (GetPlayer()->GetSocial()->HasFriend(GUID_LOPART(friendGuid)))
#else /* CROSS */
                else if (GetPlayer()->GetSocial() && GetPlayer()->GetSocial()->HasFriend(GUID_LOPART(friendGuid)))
#endif /* CROSS */
                    friendResult = FRIEND_ALREADY;
                else
                {
                    Player* pFriend = ObjectAccessor::FindPlayer(friendGuid);
                    if (pFriend && pFriend->IsInWorld() && pFriend->IsVisibleGloballyFor(GetPlayer()))
                        friendResult = FRIEND_ADDED_ONLINE;
                    else
                        friendResult = FRIEND_ADDED_OFFLINE;
#ifndef CROSS
                    if (!GetPlayer()->GetSocial()->AddToSocialList(GUID_LOPART(friendGuid), false))
#else /* CROSS */
                    if (GetPlayer()->GetSocial() && !GetPlayer()->GetSocial()->AddToSocialList(GUID_LOPART(friendGuid), false))
#endif /* CROSS */
                    {
                        friendResult = FRIEND_LIST_FULL;
                    }
                }
#ifndef CROSS
                GetPlayer()->GetSocial()->SetFriendNote(GUID_LOPART(friendGuid), friendNote);
#else /* CROSS */

                if (GetPlayer()->GetSocial())
                    GetPlayer()->GetSocial()->SetFriendNote(GUID_LOPART(friendGuid), friendNote);
#endif /* CROSS */
            }
        }
    }

    sSocialMgr->SendFriendStatus(GetPlayer(), friendResult, GUID_LOPART(friendGuid), false);
}

void WorldSession::HandleDelFriendOpcode(WorldPacket& recvData)
{
    uint64 l_Guid;

    uint32 l_VirtualRealmAddress;

    recvData >> l_VirtualRealmAddress;
    recvData.readPackGUID(l_Guid);

#ifdef CROSS
    if (!m_Player->GetSocial())
        return;

#endif /* CROSS */
    m_Player->GetSocial()->RemoveFromSocialList(GUID_LOPART(l_Guid), false);

    sSocialMgr->SendFriendStatus(GetPlayer(), FRIEND_REMOVED, GUID_LOPART(l_Guid), false);
}

void WorldSession::HandleAddIgnoreOpcode(WorldPacket& p_RecvData)
{
    time_t l_Now = time(NULL);
    if (l_Now - m_TimeLastAddIgnoreFriend < 2)
    {
        sLog->outAshran("WorldSession::HandleAddIgnoreOpcode acc %u", GetAccountId());
        return;
    }
    else
        m_TimeLastAddIgnoreFriend = l_Now;

    std::string l_IgnoreName = GetTrinityString(LANG_FRIEND_IGNORE_UNKNOWN);

    uint32 l_IgnoreNameLen = p_RecvData.ReadBits(9);

    l_IgnoreName = p_RecvData.ReadString(l_IgnoreNameLen);

    if (!normalizePlayerName(l_IgnoreName))
        return;

    PreparedStatement* l_Stmt = SessionRealmDatabase.GetPreparedStatement(CHAR_SEL_GUID_BY_NAME);

    l_Stmt->setString(0, l_IgnoreName);
    l_Stmt->setString(1, l_IgnoreName);

    m_AddIgnoreCallback = SessionRealmDatabase.AsyncQuery(l_Stmt);
}

void WorldSession::HandleAddIgnoreOpcodeCallBack(PreparedQueryResult result)
{
    if (!GetPlayer())
        return;

    uint64 IgnoreGuid;
    FriendsResult ignoreResult;

    ignoreResult = FRIEND_IGNORE_NOT_FOUND;
    IgnoreGuid = 0;

    if (result)
    {
        IgnoreGuid = MAKE_NEW_GUID((*result)[0].GetUInt32(), 0, HIGHGUID_PLAYER);

        if (IgnoreGuid)
        {
            if (IgnoreGuid == GetPlayer()->GetGUID())              //not add yourself
                ignoreResult = FRIEND_IGNORE_SELF;
#ifndef CROSS
            else if (GetPlayer()->GetSocial()->HasIgnore(GUID_LOPART(IgnoreGuid)))
#else /* CROSS */
            else if (GetPlayer()->GetSocial() && GetPlayer()->GetSocial()->HasIgnore(GUID_LOPART(IgnoreGuid)))
#endif /* CROSS */
                ignoreResult = FRIEND_IGNORE_ALREADY;
            else
            {
                ignoreResult = FRIEND_IGNORE_ADDED;

                // ignore list full
#ifndef CROSS
                if (!GetPlayer()->GetSocial()->AddToSocialList(GUID_LOPART(IgnoreGuid), true))
#else /* CROSS */
                if (GetPlayer()->GetSocial() && !GetPlayer()->GetSocial()->AddToSocialList(GUID_LOPART(IgnoreGuid), true))
#endif /* CROSS */
                    ignoreResult = FRIEND_IGNORE_FULL;
            }
        }
    }

    sSocialMgr->SendFriendStatus(GetPlayer(), ignoreResult, GUID_LOPART(IgnoreGuid), false);
}

void WorldSession::HandleDelIgnoreOpcode(WorldPacket& recvData)
{
    uint64 l_Guid;

    uint32 l_VirtualRealmAddress;

    recvData >> l_VirtualRealmAddress;
    recvData.readPackGUID(l_Guid);

#ifdef CROSS
    if (!GetPlayer()->GetSocial())
        return;

#endif /* CROSS */
    m_Player->GetSocial()->RemoveFromSocialList(GUID_LOPART(l_Guid), true);

    sSocialMgr->SendFriendStatus(GetPlayer(), FRIEND_IGNORE_REMOVED, GUID_LOPART(l_Guid), false);
}

void WorldSession::HandleSetContactNotesOpcode(WorldPacket& p_RecvData)
{
    uint64 l_Guid;
    uint32 l_VirtualRealmAddress;

    std::string l_Notes;

    p_RecvData >> l_VirtualRealmAddress;
    p_RecvData.readPackGUID(l_Guid);

    l_Notes = p_RecvData.ReadString(p_RecvData.ReadBits(10));

#ifdef CROSS
    if (!GetPlayer()->GetSocial())
        return;

#endif /* CROSS */
    m_Player->GetSocial()->SetFriendNote(GUID_LOPART(l_Guid), l_Notes);
}

void WorldSession::HandleReportBugOpcode(WorldPacket& p_RecvData)
{
    float l_PosX, l_PosY, l_PosZ, l_Orientation;
    uint32 l_ContentLen, l_MapID;
    std::string l_Content;

    p_RecvData >> l_MapID;
    p_RecvData >> l_PosX >> l_PosY >> l_PosZ >> l_Orientation;

    l_ContentLen = p_RecvData.ReadBits(10);
    p_RecvData.FlushBits();
    l_Content = p_RecvData.ReadString(l_ContentLen);

    PreparedStatement* l_Statement = SessionRealmDatabase.GetPreparedStatement(CHAR_INS_BUG_REPORT);
    l_Statement->setString(0, "Bug");
    l_Statement->setString(1, l_Content);
    SessionRealmDatabase.Execute(l_Statement);
}

void WorldSession::HandleReportSuggestionOpcode(WorldPacket& p_RecvData)
{
    float l_PosX, l_PosY, l_PosZ, l_Orientation;
    uint32 l_ContentLen, l_MapID;
    std::string l_Content;

    p_RecvData >> l_MapID;
    p_RecvData >> l_PosX >> l_PosY >> l_PosZ >> l_Orientation;

    l_ContentLen = p_RecvData.ReadBits(10);
    p_RecvData.FlushBits();
    l_Content = p_RecvData.ReadString(l_ContentLen);

    PreparedStatement* l_Statement = SessionRealmDatabase.GetPreparedStatement(CHAR_INS_BUG_REPORT);
    l_Statement->setString(0, "Suggestion");
    l_Statement->setString(1, l_Content);
    SessionRealmDatabase.Execute(l_Statement);
}

void WorldSession::HandleRequestGmTicket(WorldPacket& /*recvPakcet*/)
{
#ifndef CROSS
    // Notify player if he has a ticket in progress
    if (GmTicket* ticket = sTicketMgr->GetTicketByPlayer(GetPlayer()->GetGUID()))
    {
        if (ticket->IsCompleted())
            ticket->SendResponse(this);
        else
            sTicketMgr->SendTicket(this, ticket);
    }
#endif /* not CROSS */
}

void WorldSession::HandleReclaimCorpseOpcode(WorldPacket& p_Packet)
{
    uint64 l_CorpseGUID = 0;

    p_Packet.readPackGUID(l_CorpseGUID);

    if (GetPlayer()->isAlive())
        return;

    /// Do not allow corpse reclaim in arena
    if (GetPlayer()->InArena())
        return;

    /// Body not released yet
    if (!GetPlayer()->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
        return;

    Corpse * l_Corpse = GetPlayer()->GetCorpse();

    if (!l_Corpse)
        return;

    /// Prevent resurrect before 30-sec delay after body release not finished
    if (time_t(l_Corpse->GetGhostTime() + GetPlayer()->GetCorpseReclaimDelay(l_Corpse->GetType() == CORPSE_RESURRECTABLE_PVP)) > time_t(time(NULL)))
        return;

    if (!l_Corpse->IsWithinDistInMap(GetPlayer(), CORPSE_RECLAIM_RADIUS, true))
        return;

    /// Resurrect
    GetPlayer()->ResurrectPlayer(GetPlayer()->InBattleground() ? 1.0f : 0.5f);

    /// Spawn bones
    GetPlayer()->SpawnCorpseBones();
}

void WorldSession::HandleResurrectResponseOpcode(WorldPacket& p_RecvData)
{
    uint64 l_Guid = 0;
    p_RecvData.readPackGUID(l_Guid);
    uint32 l_Status = p_RecvData.read<uint32>();

    if (m_Player->isAlive())
    {
        /// Resurrecting - 60s aura preventing client from new res spells
        m_Player->RemoveAura(160029);
        return;
    }

    /// Accept: 0, Decline: 1, Timeout: 2
    if (l_Status != 0)
    {
        /// Resurrecting - 60s aura preventing client from new res spells
        m_Player->RemoveAura(160029);

        m_Player->ClearResurrectRequestData();           // reject
        return;
    }

    if (!m_Player->IsRessurectRequestedBy(l_Guid))
    {
        /// Resurrecting - 60s aura preventing client from new res spells
        m_Player->RemoveAura(160029);
        return;
    }

    m_Player->ResurectUsingRequestData();
}

void WorldSession::HandleAreaTriggerOpcode(WorldPacket& p_RecvData)
{
    uint32 l_ID       =0;
    bool l_Enter      = false;
    bool l_FromClient = false;

    p_RecvData >> l_ID;
    l_Enter = p_RecvData.ReadBit();
    l_FromClient = p_RecvData.ReadBit(); ///< l_fromclient is never read 01/18/16

    Player* l_Player = GetPlayer();
    if (l_Player->isInFlight())
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) in flight, ignore Area Trigger ID:%u",
            l_Player->GetName(), l_Player->GetGUIDLow(), l_ID);
        return;
    }

    AreaTriggerEntry const* l_ATEntry = sAreaTriggerStore.LookupEntry(l_ID);
    if (!l_ATEntry)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) send unknown (by DB2) Area Trigger ID:%u",
            l_Player->GetName(), l_Player->GetGUIDLow(), l_ID);
        return;
    }

    if (l_Player->GetMapId() != l_ATEntry->ContinentID)
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) too far (trigger map: %u player map: %u), ignore Area Trigger ID: %u",
            l_Player->GetName(), l_ATEntry->ContinentID, l_Player->GetMapId(), l_Player->GetGUIDLow(), l_ID);
        return;
    }

    if (l_Enter && !l_Player->IsInAreaTriggerRadius(l_ATEntry))
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "HandleAreaTriggerOpcode: Player '%s' (GUID: %u) too far (1/2 box X: %f 1/2 box Y: %f 1/2 box Z: %f ignore Area Trigger ID: %u",
            l_Player->GetName(), l_Player->GetGUIDLow(), l_ATEntry->BoxLength / 2, l_ATEntry->BoxWidth / 2, l_ATEntry->BoxHeight / 2, l_ID);
        return;
    }

    if (l_Player->isDebugAreaTriggers)
        ChatHandler(l_Player).PSendSysMessage(LANG_DEBUG_AREATRIGGER_REACHED, l_ID, l_Enter);

    if (sScriptMgr->OnAreaTrigger(l_Player, l_ATEntry))
        return;

    if (l_Enter)
        sScriptMgr->OnEnterAreaTrigger(l_Player, l_ATEntry);
    else
        sScriptMgr->OnExitAreaTrigger(l_Player, l_ATEntry);

    if (l_Player->isAlive())
        l_Player->QuestObjectiveSatisfy(l_ID, 1, QUEST_OBJECTIVE_TYPE_AREATRIGGER, l_Player->GetGUID());

    if (sObjectMgr->IsTavernAreaTrigger(l_ID))
    {
        /// set resting flag we are in the inn
        l_Player->SetRestFlag(REST_FLAG_IN_TAVERN, l_ATEntry->ID);

        if (sWorld->IsFFAPvPRealm())
            l_Player->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_FFA_PVP);

        return;
    }

    if (Battleground* l_Battleground = l_Player->GetBattleground())
    {
        if (l_Battleground->GetStatus() == STATUS_IN_PROGRESS)
        {
            l_Battleground->HandleAreaTrigger(l_Player, l_ID, l_Enter);
            return;
        }
    }

    if (OutdoorPvP* l_OutdoorPvP = l_Player->GetOutdoorPvP())
    {
        if (l_OutdoorPvP->HandleAreaTrigger(m_Player, l_ID, l_Enter))
            return;
    }

    AreaTriggerStruct const* l_AreatriggerStruct = sObjectMgr->GetAreaTrigger(l_ID);
    if (!l_AreatriggerStruct)
        return;

    bool l_Teleported = false;
    if (l_Player->GetMapId() != l_AreatriggerStruct->target_mapId)
    {
        if (!sMapMgr->CanPlayerEnter(l_AreatriggerStruct->target_mapId, l_Player, false))
            return;

        if (GroupPtr l_Group = l_Player->GetGroup())
        {
            if (l_Group->isLFGGroup() && l_Player->GetMap()->IsDungeon())
                l_Teleported = l_Player->TeleportToBGEntryPoint();
        }
    }

    if (!l_Teleported)
        l_Player->TeleportTo(l_AreatriggerStruct->target_mapId, l_AreatriggerStruct->target_X, l_AreatriggerStruct->target_Y, l_AreatriggerStruct->target_Z, l_AreatriggerStruct->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
}

void WorldSession::HandleUpdateAccountData(WorldPacket& p_Packet)
{
    uint64 l_CharacterGUID      = 0;
    int32  l_Time               = 0;
    uint32 l_DataType           = 0;
    uint32 l_CompressedSize     = 0;
    uint32 l_UncompressedSize   = 0;

    p_Packet.readPackGUID(l_CharacterGUID);
    p_Packet >> l_Time;
    p_Packet >> l_UncompressedSize;

    l_DataType = p_Packet.ReadBits(3);

    p_Packet.FlushBits();

    p_Packet >> l_CompressedSize;

    /// Erase
    if (l_UncompressedSize == 0)
    {
        SetAccountData(AccountDataType(l_DataType), 0, "");
        return;
    }

    if (l_UncompressedSize > 0xFFFF)
    {
        /// Unneeded warning Spam in this case
        p_Packet.rfinish();
        return;
    }

    ByteBuffer l_Data;
    l_Data.resize(l_UncompressedSize);

    uLongf l_RealSize = l_UncompressedSize;
    if (uncompress(const_cast<uint8*>(l_Data.contents()), &l_RealSize, const_cast<uint8*>(p_Packet.contents() + p_Packet.rpos()), p_Packet.size() - p_Packet.rpos()) != Z_OK)
    {
        /// Unneeded warning Spam in this case
        p_Packet.rfinish();
        return;
    }

    std::string l_AccountData = l_Data.ReadString(l_UncompressedSize);
    SetAccountData(AccountDataType(l_DataType), l_Time, l_AccountData);
}

void WorldSession::HandleRequestAccountData(WorldPacket& p_Packet)
{
    uint64 l_CharacterGuid = 0;
    uint32 l_Type = 0;

    p_Packet.readPackGUID(l_CharacterGuid);
    l_Type = p_Packet.ReadBits(3);

    if (l_Type > NUM_ACCOUNT_DATA_TYPES)
        return;

    AccountData* l_AccountData = GetAccountData(AccountDataType(l_Type));

    uint32 l_Size       = l_AccountData->Data.size();
    uLongf l_DestSize   = compressBound(l_Size);

    ByteBuffer l_CompressedData;
    l_CompressedData.resize(l_DestSize);

    if (l_Size && compress(const_cast<uint8*>(l_CompressedData.contents()), &l_DestSize, (uint8*)l_AccountData->Data.c_str(), l_Size) != Z_OK)
        return;

    l_CompressedData.resize(l_DestSize);

    WorldPacket l_Response(SMSG_UPDATE_ACCOUNT_DATA, 4+4+4+3+3+5+8+l_DestSize);

    l_Response.appendPackGUID(l_CharacterGuid);
    l_Response << uint32(l_AccountData->Time);      ///< Unix time
    l_Response << uint32(l_Size);                   ///< Decompressed length
    l_Response.WriteBits(l_Type, 3);
    l_Response.FlushBits();
    l_Response << uint32(l_DestSize);               ///< Compressed length
    l_Response.append(l_CompressedData);            ///< Compressed data

    SendPacket(&l_Response);
}

int32 WorldSession::HandleEnableNagleAlgorithm()
{
    return 0;
}

void WorldSession::HandleSetActionButtonOpcode(WorldPacket& p_RecvData)
{
    uint64 l_PackedData = 0;

    uint8 l_Button = 0;

    p_RecvData >> l_PackedData;
    p_RecvData >> l_Button;

    uint8   l_Type      = ACTION_BUTTON_TYPE(l_PackedData);
    uint32  l_ActionID  = ACTION_BUTTON_ACTION(l_PackedData);

    if (!l_PackedData)
    {
        GetPlayer()->removeActionButton(l_Button);
    }
    else
    {
        GetPlayer()->addActionButton(l_Button, l_ActionID, l_Type);
    }
}

void WorldSession::HandleCompleteCinematic(WorldPacket& /*recvData*/)
{
    m_Player->StopCinematic();
}

void WorldSession::HandleNextCinematicCamera(WorldPacket& /*recvData*/)
{
}

void WorldSession::HandleCompleteMovieOpcode(WorldPacket& /*p_Packet*/)
{
    if (!m_Player || m_Player->CurrentPlayedMovie == 0)
        return;

    m_Player->SetStunned(false);

    m_Player->MovieDelayedTeleportMutex.lock();

    auto l_It = std::find_if(m_Player->MovieDelayedTeleports.begin(), m_Player->MovieDelayedTeleports.end(), [this](const Player::MovieDelayedTeleport & p_Elem) -> bool
    {
        if (p_Elem.MovieID == m_Player->CurrentPlayedMovie)
            return true;

        return false;
    });

    if (l_It != m_Player->MovieDelayedTeleports.end())
    {
        Player::MovieDelayedTeleport l_TeleportData = (*l_It);
        m_Player->MovieDelayedTeleports.erase(l_It);

        if (l_TeleportData.MapID == m_Player->GetMapId())
            m_Player->NearTeleportTo(l_TeleportData.X, l_TeleportData.Y, l_TeleportData.Z, l_TeleportData.O, false);
        else
            m_Player->TeleportTo(l_TeleportData.MapID, l_TeleportData.X, l_TeleportData.Y, l_TeleportData.Z, l_TeleportData.O, false);
    }

    m_Player->MovieDelayedTeleportMutex.unlock();

    sScriptMgr->OnMovieComplete(m_Player, m_Player->CurrentPlayedMovie);

    m_Player->CurrentPlayedMovie = 0;
}

void WorldSession::HandleSceneTriggerEventOpcode(WorldPacket& p_Packet)
{
    if (!m_Player)
        return;

    uint8 l_EventLenght         = p_Packet.ReadBits(6);
    uint32 l_SceneInstanceID    = p_Packet.read<uint32>();
    std::string l_Event         = p_Packet.ReadString(l_EventLenght);

    if (m_Player)
        m_Player->TrigerScene(l_SceneInstanceID, l_Event);
}

void WorldSession::HandleSceneCancelOpcode(WorldPacket& p_Packet)
{
    if (!m_Player)
        return;

    uint32 l_SceneInstanceID = p_Packet.read<uint32>();

    sScriptMgr->OnSceneCancel(m_Player, l_SceneInstanceID);

    m_Player->RemoveAuraIDBySceneInstance(l_SceneInstanceID);
}

void WorldSession::HandleSceneCompleteOpcode(WorldPacket& p_Packet)
{
    if (!m_Player)
        return;

    uint32 l_SceneInstanceID = p_Packet.read<uint32>();

    sScriptMgr->OnSceneComplete(m_Player, l_SceneInstanceID);

    m_Player->RemoveAuraIDBySceneInstance(l_SceneInstanceID);
}

void WorldSession::HandleMoveTimeSkippedOpcode(WorldPacket& p_Packet)
{
    uint64 p_MoverGUID;
    uint32 p_Time;

    p_Packet.readPackGUID(p_MoverGUID);
    p_Packet >> p_Time;

    //TODO!

    /*
    uint64 guid;
    uint32 time_skipped;
    recvData >> guid;
    recvData >> time_skipped;
    sLog->outDebug(LOG_FILTER_PACKETIO, "WORLD: CMSG_MOVE_TIME_SKIPPED");

    /// TODO
    must be need use in Trinity
    We substract server Lags to move time (AntiLags)
    for exmaple
    GetPlayer()->ModifyLastMoveTime(-int32(time_skipped));
    */
}

void WorldSession::HandleMoveWaterWalkAck(WorldPacket & recvPacket)
{
}

void WorldSession::HandleFeatherFallAck(WorldPacket& recvData)
{
    // no used
    recvData.rfinish();                       // prevent warnings spam
}

void WorldSession::HandleMoveUnRootAck(WorldPacket& recvData)
{
    // no used
    recvData.rfinish();                       // prevent warnings spam
}

void WorldSession::HandleMoveRootAck(WorldPacket& recvData)
{
    // no used
    recvData.rfinish();                       // prevent warnings spam
}

void WorldSession::HandleLookingForGroup(WorldPacket & recvPacket)
{
}

void WorldSession::HandleSetActionBarToggles(WorldPacket& p_Packet)
{
    uint8 l_ActionBar;

    p_Packet >> l_ActionBar;

    if (!m_Player)                                        // ignore until not logged (check needed because STATUS_AUTHED)
    {
        if (l_ActionBar != 0)
            sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetActionBarToggles in not logged state with value: %u, ignored", uint32(l_ActionBar));
        return;
    }

    m_Player->SetByteValue(PLAYER_FIELD_NUM_RESPECS, PLAYER_FIELD_BYTES_OFFSET_ACTION_BAR_TOGGLES, l_ActionBar);
}

void WorldSession::HandlePlayedTime(WorldPacket& recvData)
{
    bool l_TriggerScriptEvent = recvData.ReadBit();                 // 0 or 1 expected

    WorldPacket data(SMSG_PLAYED_TIME, 4 + 4 + 1);
    data << uint32(m_Player->GetTotalPlayedTime());
    data << uint32(m_Player->GetLevelPlayedTime());
    data.WriteBit(l_TriggerScriptEvent);                            // 0 - will not show in chat frame
    data.FlushBits();
    SendPacket(&data);
}

void WorldSession::HandleInspectOpcode(WorldPacket& p_RecvData)
{
    uint64 l_PlayerGuid = 0;
    p_RecvData.readPackGUID(l_PlayerGuid);

    //m_Player->SetSelection(l_PlayerGuid);

    Player* l_Player = ObjectAccessor::FindPlayer(l_PlayerGuid);
    if (!l_Player)
        return;

    uint32 l_TalentCount    = 0;
    uint32 l_GlyphCount     = 0;
    uint32 l_EquipmentCount = 0;
    uint32 l_PvPTalentCount = 0;
    bool l_HasGuild = false;

    //////////////////////////////////////////////////////////////////////////

    ByteBuffer l_ItemsData;
    for (uint8 l_Iter = 0; l_Iter < EQUIPMENT_SLOT_END; ++l_Iter)
    {
        Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Iter);
        if (!l_Item)
            continue;

        uint64 l_ItemCreator = l_Item->GetGuidValue(ITEM_FIELD_CREATOR);
        ++l_EquipmentCount;

        ByteBuffer l_EnchantDatas;
        uint32 l_EnchantCount = 0;
        for (uint8 l_EnchantSlot = 0; l_EnchantSlot < MAX_ENCHANTMENT_SLOT; ++l_EnchantSlot)
        {
            uint32 l_EnchantID = l_Item->GetEnchantmentId(EnchantmentSlot(l_EnchantSlot));
            if (!l_EnchantID)
                continue;

            ++l_EnchantCount;

            l_EnchantDatas << uint32(l_EnchantID);
            l_EnchantDatas << uint8(l_EnchantSlot);
        }

        ByteBuffer l_GemDatas;
        uint32 l_GemCount = 0;

        for (ItemDynamicFieldGems const& l_GemData : l_Item->GetGems())
        {
            if (sObjectMgr->GetItemTemplate(l_GemData.ItemId))
            {
                std::vector<uint32> l_Bonuses;
                for (uint16 l_BonusListId : l_GemData.BonusListIDs)
                {
                    if (l_BonusListId != 0 && GetItemBonusesByID(l_BonusListId))
                        l_Bonuses.push_back(l_BonusListId);
                }

                l_GemDatas << uint8(l_GemCount);
                Item::BuildDynamicItemDatas(l_GemDatas, l_GemData.ItemId, l_Bonuses);
                ++l_GemCount;
            }
        }

        l_ItemsData.appendPackGUID(l_ItemCreator);
        l_ItemsData << uint8(l_Iter);

        Item::BuildDynamicItemDatas(l_ItemsData, l_Item);

        l_ItemsData.WriteBit(true);      ///< Usable
        l_ItemsData.WriteBits(l_EnchantCount, 4);
        l_ItemsData.WriteBits(l_GemCount, 2);
        l_ItemsData.FlushBits();

        if (l_GemDatas.size())
            l_ItemsData.append(l_GemDatas);

        if (l_EnchantDatas.size())
            l_ItemsData.append(l_EnchantDatas);
    }

    //////////////////////////////////////////////////////////////////////////

    ByteBuffer l_GlyphsData;
    for (uint32 l_GlyphID : l_Player->GetGlyphs(l_Player->GetActiveTalentGroup()))
    {
        if (std::vector<uint32> const* l_BindableSpells = GetGlyphBindableSpells(l_GlyphID))
        {
            for (uint32 l_BindableSpell : *l_BindableSpells)
            {
                if (l_Player->HasSpell(l_BindableSpell) && l_Player->m_overrideSpells.find(l_BindableSpell) == l_Player->m_overrideSpells.end())
                {
                    l_GlyphsData << uint16(l_GlyphID);

                    ++l_GlyphCount;
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////

    ByteBuffer l_TalentsData;
    for (auto l_Talent : *l_Player->GetTalentMap(l_Player->GetActiveTalentGroup()))
    {
        l_TalentsData << uint16(l_Talent.first);
        ++l_TalentCount;
    }

    //////////////////////////////////////////////////////////////////////////

    ByteBuffer l_PvPTalentsData;
    for (uint32 l_I = 0; l_I < MAX_PVP_TALENT_SPELLS; ++l_I)
    {
        uint16 l_Talent = l_Player->GetPvPTalentAtTier(l_I);

        if (l_Talent)
        {
            l_PvPTalentsData << uint16(l_Talent);
            ++l_PvPTalentCount;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    ByteBuffer l_GuildDatas;

#ifndef CROSS
    Guild* l_Guild = sGuildMgr->GetGuildById(l_Player->GetGuildId());
    l_HasGuild = l_Guild != nullptr;

    if (l_Guild != nullptr)
    {
        uint64 l_GuildGuid = l_Guild->GetGUID();
        l_GuildDatas.appendPackGUID(l_GuildGuid);             ///< GuildGUID
        l_GuildDatas << uint32(l_Guild->GetMembersCount());   ///< NumGuildMembers
        l_GuildDatas << uint32(l_Guild->GetAchievementMgr().GetAchievementPoints());  ///< AchievementPoints
    }
#else /* CROSS */
    uint64 guildGuid = MAKE_NEW_GUID(l_Player->GetGuildId(), 0, HIGHGUID_GUILD);
    InterRealmGuild* guild = sGuildMgr->GetInterRealmGuild(l_Player->GetSession()->GetInterRealmNumber(), guildGuid);
    l_HasGuild = guild != nullptr;

    if (guild != nullptr)
    {
        uint64 l_GuildGuid = guild->GetGUID();
        l_GuildDatas.appendPackGUID(l_GuildGuid);              ///< GuildGUID
        l_GuildDatas << uint32(guild->GetMembersCount());      ///< NumGuildMembers
        l_GuildDatas << uint32(guild->GetAchievementPoints()); ///< AchievementPoints
    }
#endif /* CROSS */

    //////////////////////////////////////////////////////////////////////////

    WorldPacket l_Data(Opcodes::SMSG_INSPECT_TALENT, 1024);
    l_Data.appendPackGUID(l_PlayerGuid);
    l_Data << uint32(l_EquipmentCount);
    l_Data << uint32(l_GlyphCount);
    l_Data << uint32(l_TalentCount);
    l_Data << uint32(l_PvPTalentCount);
    l_Data << int32(l_Player->getClass());
    l_Data << int32(l_Player->GetActiveSpecializationID());
    l_Data << int32(l_Player->getGender());

    if (l_GlyphsData.size())
        l_Data.append(l_GlyphsData);

    if (l_TalentsData.size())
        l_Data.append(l_TalentsData);

    if (l_PvPTalentsData.size())
        l_Data.append(l_PvPTalentsData);

    l_Data.WriteBit(l_HasGuild);
    l_Data.FlushBits();

    if (l_ItemsData.size())
        l_Data.append(l_ItemsData);

    if (l_GuildDatas.size())
        l_Data.append(l_GuildDatas);

    SendPacket(&l_Data);
}

void WorldSession::HandleInspectHonorStatsOpcode(WorldPacket& p_RecvData)
{
    uint64 l_Guid = 0;
    p_RecvData.readPackGUID(l_Guid);

    Player* l_Player = ObjectAccessor::FindPlayer(l_Guid);
    if (!l_Player)
        return;

    WorldPacket l_Data(SMSG_INSPECT_HONOR_STATS);
    l_Data.appendPackGUID(l_Guid);
    l_Data << uint8(l_Player->GetByteValue(PLAYER_FIELD_NUM_RESPECS, PLAYER_FIELD_BYTES_OFFSET_LIFETIME_MAX_PVP_RANK));
    l_Data << uint16(l_Player->GetUInt16Value(PLAYER_FIELD_YESTERDAY_HONORABLE_KILLS, 0));
    l_Data << uint16(l_Player->GetUInt16Value(PLAYER_FIELD_YESTERDAY_HONORABLE_KILLS, 1));
    l_Data << uint32(l_Player->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS));
    SendPacket(&l_Data);
}

void WorldSession::HandleInspectPVPStatsOpcode(WorldPacket& p_RecvData)
{
    uint32 l_InspectRealmAddress;
    uint64 l_Guid = 0;

    p_RecvData.readPackGUID(l_Guid);
    p_RecvData >> l_InspectRealmAddress;

    Player* l_Player = ObjectAccessor::FindPlayer(l_Guid);
    if (!l_Player)
        return;

    WorldPacket l_Data(SMSG_INSPECT_PVP);
    l_Data.appendPackGUID(l_Guid);
    l_Data.WriteBits((uint8)BattlegroundBracketType::Max, 3);

    for (uint8 l_Iter = 0; l_Iter < (uint8)BattlegroundBracketType::Max; ++l_Iter)
    {
        l_Data << int32(l_Player->GetArenaPersonalRating(l_Iter));
        l_Data << int32(0);                                 ///< Rank
        l_Data << int32(l_Player->GetWeekGames(l_Iter));
        l_Data << int32(l_Player->GetWeekWins(l_Iter));
        l_Data << int32(l_Player->GetSeasonGames(l_Iter));
        l_Data << int32(l_Player->GetSeasonWins(l_Iter));
        l_Data << int32(l_Player->GetBestRatingOfWeek(l_Iter));
        l_Data << int32(0);
        l_Data << l_Iter;
    }

    SendPacket(&l_Data);
}

void WorldSession::HandleWorldTeleportOpcode(WorldPacket& recvData)
{
    uint32 time;
    uint32 mapid;
    float PositionX;
    float PositionY;
    float PositionZ;
    float Orientation;

    recvData >> time;                                      // time in m.sec.
    recvData >> mapid;
    recvData >> PositionX;
    recvData >> Orientation;
    recvData >> PositionY;
    recvData >> PositionZ;                          // o (3.141593 = 180 degrees)

    if (GetPlayer()->isInFlight())
    {
        sLog->outDebug(LOG_FILTER_NETWORKIO, "Player '%s' (GUID: %u) in flight, ignore worldport command.", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow());
        return;
    }

    if (AccountMgr::IsAdminAccount(GetSecurity()))
        GetPlayer()->TeleportTo(mapid, PositionX, PositionY, PositionZ, Orientation);
    else
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
}

void WorldSession::HandleWhoisOpcode(WorldPacket& recvData)
{
#ifndef CROSS
    uint32 textLength = recvData.ReadBits(6);
    recvData.FlushBits();
    std::string charname = recvData.ReadString(textLength);

    if (!AccountMgr::IsAdminAccount(GetSecurity()))
    {
        SendNotification(LANG_YOU_NOT_HAVE_PERMISSION);
        return;
    }

    if (charname.empty() || !normalizePlayerName (charname))
    {
        SendNotification(LANG_NEED_CHARACTER_NAME);
        return;
    }

    Player* player = sObjectAccessor->FindPlayerByName(charname.c_str());

    if (!player)
    {
        SendNotification(LANG_PLAYER_NOT_EXIST_OR_OFFLINE, charname.c_str());
        return;
    }

    uint32 accid = player->GetSession()->GetAccountId();

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_WHOIS);

    stmt->setUInt32(0, accid);

    PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (!result)
    {
        SendNotification(LANG_ACCOUNT_FOR_PLAYER_NOT_FOUND, charname.c_str());
        return;
    }

    Field* fields = result->Fetch();
    std::string acc = fields[0].GetString();
    if (acc.empty())
        acc = "Unknown";
    std::string email = fields[1].GetString();
    if (email.empty())
        email = "Unknown";
    std::string lastip = fields[2].GetString();
    if (lastip.empty())
        lastip = "Unknown";

    std::string msg = charname + "'s " + "account is " + acc + ", e-mail: " + email + ", last ip: " + lastip;

    WorldPacket l_Data(SMSG_WHOIS, msg.size()+1);
    l_Data.WriteBits(msg.size(), 11);
    l_Data.FlushBits();
    l_Data.WriteString(msg);
    SendPacket(&l_Data);
#endif
}

void WorldSession::HandleComplainOpcode(WorldPacket& /*recvData*/)
{
    // recvData is not empty, but all data are unused in core
    // NOTE: all chat messages from this spammer automatically ignored by spam reporter until logout in case chat spam.
    // if it's mail spam - ALL mails from this spammer automatically removed by client

    // Complaint Received message
    WorldPacket data(SMSG_COMPLAIN_RESULT, 2);
    data << uint32(0);  ///< ComplaintType // value 0xC generates a "CalendarError" in client.
    data << uint8(0);   ///< Result        // value 1 resets CGChat::m_complaintsSystemStatus in client. (unused?)
    SendPacket(&data);
}

#ifndef CROSS
void WorldSession::HandleDisplayPlayerChoiceResponse(WorldPacket& p_RecvData)
{
    int32 l_ChoiceID;
    int32 l_ResponseID;

    p_RecvData >> l_ChoiceID;
    p_RecvData >> l_ResponseID;

    if (m_Player->PlayerTalkClass->GetInteractionData().PlayerChoiceId != uint32(l_ChoiceID))
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Error in CMSG_CHOICE_RESPONSE: Player (GUID: %u) tried to respond to invalid player choice %d (allowed %u) (possible packet-hacking detected)",
            GetGuidLow(), l_ChoiceID, m_Player->PlayerTalkClass->GetInteractionData().PlayerChoiceId);
        return;
    }

    PlayerChoice const* l_PlayerChoice = sObjectMgr->GetPlayerChoice(l_ChoiceID);
    if (!l_PlayerChoice)
        return;

    PlayerChoiceResponse const* l_PlayerChoiceResponse = l_PlayerChoice->GetResponse(l_ResponseID);
    if (!l_PlayerChoiceResponse)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Error in CMSG_CHOICE_RESPONSE: Player (GUID: %u) tried to select invalid player choice response %d (possible packet-hacking detected)",
            GetGuidLow(), l_ResponseID);
        return;
    }

    sScriptMgr->OnPlayerChoiceResponse(GetPlayer(), l_ChoiceID, l_ResponseID);

    if (l_PlayerChoiceResponse->Reward)
    {
        if (l_PlayerChoiceResponse->Reward->Unique)
            m_Player->SetPlayerChoiceResponseRewarded(l_ChoiceID, l_ResponseID);

        if (l_PlayerChoiceResponse->Reward->TitleID)
            m_Player->SetTitle(sCharTitlesStore.LookupEntry(l_PlayerChoiceResponse->Reward->TitleID), false);

        if (l_PlayerChoiceResponse->Reward->PackageID)
            m_Player->RewardQuestPackage(l_PlayerChoiceResponse->Reward->PackageID);

        if (l_PlayerChoiceResponse->Reward->SkillLineID && m_Player->HasSkill(l_PlayerChoiceResponse->Reward->SkillLineID))
            m_Player->UpdateSkillPro(l_PlayerChoiceResponse->Reward->SkillLineID, 1000, l_PlayerChoiceResponse->Reward->SkillPointCount);

        if (l_PlayerChoiceResponse->Reward->HonorPointCount)
            m_Player->GiveHonor(l_PlayerChoiceResponse->Reward->HonorPointCount);

        if (l_PlayerChoiceResponse->Reward->Money)
        {
            std::stringstream l_Ss;
            l_Ss << "Player::HandleDisplayChoiceResponse, ChoiceID : " << l_ChoiceID << ", l_ResponseID : " << l_ResponseID;
            m_Player->ModifyMoney(l_PlayerChoiceResponse->Reward->Money, l_Ss.str());
        }

        if (l_PlayerChoiceResponse->Reward->Xp)
            m_Player->GiveXP(l_PlayerChoiceResponse->Reward->Xp, nullptr, 0.0f);

        for (PlayerChoiceResponseRewardItem const& l_Item : l_PlayerChoiceResponse->Reward->Items)
        {
            ItemPosCountVec l_Dest;
            if (m_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Dest, l_Item.Id, l_Item.Quantity) == EQUIP_ERR_OK)
            {
                Item* newItem = m_Player->StoreNewItem(l_Dest, l_Item.Id, true, Item::GenerateItemRandomPropertyId(l_Item.Id));
                m_Player->SendNewItem(newItem, l_Item.Quantity, true, false);
            }
        }

        for (PlayerChoiceResponseRewardEntry const& l_Currency : l_PlayerChoiceResponse->Reward->Currency)
            m_Player->ModifyCurrency(l_Currency.Id, l_Currency.Quantity);

        for (PlayerChoiceResponseRewardEntry const& l_Faction : l_PlayerChoiceResponse->Reward->Faction)
            m_Player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(l_Faction.Id), l_Faction.Quantity);

        for (uint32 const& l_Spell : l_PlayerChoiceResponse->Reward->Spells)
            m_Player->CastSpell(m_Player, l_Spell, true);
    }
}
#endif

enum RealmQueryNameResponse
{
    REALM_QUERY_NAME_RESPONSE_OK        = 0,
    REALM_QUERY_NAME_RESPONSE_DENY      = 1,
    REALM_QUERY_NAME_RESPONSE_RETRY     = 2,
    REALM_QUERY_NAME_RESPONSE_OK_TEMP   = 3,
};

void WorldSession::HandleRealmQueryNameOpcode(WorldPacket& p_Packet)
{
    uint32 l_RealmID = 0;

    p_Packet >> l_RealmID;

    WorldPacket l_Data(SMSG_REALM_QUERY_RESPONSE);

#ifndef CROSS
    /// Cheater ?
    if (l_RealmID != g_RealmID)
        return;

    l_Data << uint32(g_RealmID);                                    ///< Virtual Realm Address
    l_Data << uint8(REALM_QUERY_NAME_RESPONSE_OK);                  ///< Lookup State
    l_Data.WriteBit(true);                                          ///< Is Locale
    l_Data.WriteBit(false);                                         ///< Unk
    l_Data.WriteBits(sWorld->GetRealmName().size(), 8);             ///< Realm Name Actual
    l_Data.WriteBits(sWorld->GetNormalizedRealmName().size(), 8);   ///< Realm Name Normalized
    l_Data.FlushBits();

    l_Data.WriteString(sWorld->GetRealmName());                     ///< Realm Name Actual
    l_Data.WriteString(sWorld->GetNormalizedRealmName());           ///< Realm Name Normalized
#else /* CROSS */
    const InterRealmDatabaseConfig* l_RealmConfig = sInterRealmMgr->GetConfig(l_RealmID);

    if (l_RealmConfig == nullptr)
        return;

    std::string l_RealmName = l_RealmConfig->name;

    l_Data << uint32(l_RealmID);                                    ///< Virtual Realm Address
    l_Data << uint8(REALM_QUERY_NAME_RESPONSE_OK);                  ///< Lookup State
    l_Data.WriteBit(false);                                         ///< Is Locale
    l_Data.WriteBit(false);                                         ///< Unk
    l_Data.WriteBits(l_RealmName.size(), 8);                        ///< Realm Name Actual
    l_Data.WriteBits(l_RealmName.size(), 8);                        ///< Realm Name Normalized
    l_Data.FlushBits();
    l_Data.WriteString(l_RealmName);                                ///< Realm Name Actual
    l_Data.WriteString(l_RealmName);                                ///< Realm Name Normalized
#endif /* CROSS */

    SendPacket(&l_Data);
}

void WorldSession::HandleFarSightOpcode(WorldPacket& p_Packet)
{
    bool l_Apply = p_Packet.ReadBit();

    if (!l_Apply)
    {
        m_Player->SetSeer(m_Player);
    }
    else
    {
        if (WorldObject * l_Target = m_Player->GetViewpoint())
            m_Player->SetSeer(l_Target);
        else
            sLog->outError(LOG_FILTER_NETWORKIO, "Player %s requests non-existing seer " UI64FMTD, m_Player->GetName(), m_Player->GetGuidValue(PLAYER_FIELD_FARSIGHT_OBJECT));
    }

    GetPlayer()->UpdateObjectVisibility();
}

void WorldSession::HandleCloseInteraction(WorldPacket& p_RecvData)
{
    if (!m_Player)
        return;

    uint64 l_GUID = 0;
    p_RecvData.readPackGUID(l_GUID);

    m_Player->SetInteractionStatus(l_GUID, InteractionStatus::None);

    Creature* l_Creature = m_Player->GetMap()->GetCreature(l_GUID);

    if (l_Creature == nullptr)
        return;

    sScriptMgr->OnCloseGossip(m_Player, l_Creature);
}

void WorldSession::HandleSetTitleOpcode(WorldPacket& p_Packet)
{
    uint32 l_Title = 0;

    p_Packet >> l_Title;

    /// -1 at none
    if (l_Title > 0 && l_Title < MAX_TITLE_INDEX)
    {
        if (!GetPlayer()->HasTitle(l_Title))
            return;
    }
    else
        l_Title = 0;

    GetPlayer()->SetUInt32Value(PLAYER_FIELD_PLAYER_TITLE, l_Title);
}

void WorldSession::HandleTimeSyncResp(WorldPacket& p_RecvData)
{
    uint32 l_Counter, l_ClientTicks;
    p_RecvData >> l_Counter >> l_ClientTicks;

    m_Player->m_timeSyncClient = l_ClientTicks;
}

void WorldSession::HandleResetInstancesOpcode(WorldPacket& /*p_RecvData*/)
{
    if (auto group = m_Player->GetGroup())
    {
        if (group->IsLeader(m_Player->GetGUID()))
        {
            if (group->m_challengeEntry)
                group->m_challengeEntry = nullptr;

            group->ResetInstances(INSTANCE_RESET_ALL, false, false, m_Player);
            if (group->GetDungeonDifficultyID() == DifficultyMythicKeystone)
                group->SetDungeonDifficultyID(DifficultyMythic);
        }
        else
            return;
    }
    else
    {
        m_Player->m_challengeEntry = nullptr;
        m_Player->ResetInstances(INSTANCE_RESET_ALL, false, false);
        if (m_Player->GetDungeonDifficultyID() == DifficultyMythicKeystone)
            m_Player->SetDungeonDifficultyID(DifficultyMythic);
    }
}

void WorldSession::HandleSetDungeonDifficultyOpcode(WorldPacket & recvData)
{
    uint32 mode;
    recvData >> mode;

    DifficultyEntry const* difficultyEntry = sDifficultyStore.LookupEntry(mode);
    if (!difficultyEntry)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: %d sent an invalid instance mode %d!", m_Player->GetGUIDLow(), mode);
        return;
    }

    if (difficultyEntry->InstanceType != MAP_INSTANCE)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: %d sent an non-dungeon instance mode %d!", m_Player->GetGUIDLow(), difficultyEntry->ID);
        return;
    }

    if (!(difficultyEntry->Flags & DIFFICULTY_FLAG_CAN_SELECT))
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: %d sent unselectable instance mode %d!", m_Player->GetGUIDLow(), difficultyEntry->ID);
        return;
    }

    Difficulty difficultyID = Difficulty(difficultyEntry->ID);
    if (difficultyID == m_Player->GetDungeonDifficultyID())
        return;

    // cannot reset while in an instance
    Map* map = m_Player->GetMap();
    if (map && map->IsDungeon())
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: player (Name: %s, GUID: %u) tried to reset the instance while player is inside!", m_Player->GetName(), m_Player->GetGUIDLow());
        return;
    }

    GroupPtr group = m_Player->GetGroup();
    if (group)
    {
        if (group->IsLeader(m_Player->GetGUID()))
        {
            bool l_CanChange = true;

            group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Member)
            {
                Player* groupGuy = l_Member->player;

                if (!groupGuy)
                    return;

                if (!groupGuy->IsInMap(groupGuy))
                {
                    l_CanChange = false;
                    return;
                }

                if (groupGuy->GetMap()->IsNonRaidDungeon())
                {
                    l_CanChange = false;
                    sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: player %d tried to reset the instance while group member (Name: %s, GUID: %u) is inside!", m_Player->GetGUIDLow(), groupGuy->GetName(), groupGuy->GetGUIDLow());
                    return;
                }
            });

            if (l_CanChange)
            {
                group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false, false, m_Player);
                group->SetDungeonDifficultyID(difficultyID);
                m_Player->SendDungeonDifficulty();
            }
        }
    }
    else
    {
        m_Player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, false, false);
        m_Player->SetDungeonDifficultyID(difficultyID);
        m_Player->SendDungeonDifficulty();
    }
}

void WorldSession::HandleSetRaidDifficultyOpcode(WorldPacket& p_RecvData)
{
    uint32 l_Difficulty;
    uint8 l_IsLegacyDifficulty;

    p_RecvData >> l_Difficulty;
    p_RecvData >> l_IsLegacyDifficulty;

    DifficultyEntry const* difficultyEntry = sDifficultyStore.LookupEntry(l_Difficulty);
    if (!difficultyEntry)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: %d sent an invalid instance mode %u!",
                       m_Player->GetGUIDLow(), l_Difficulty);
        return;
    }

    if (difficultyEntry->InstanceType != MAP_RAID)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: %d sent an non-dungeon instance mode %u!",
                       m_Player->GetGUIDLow(), difficultyEntry->ID);
        return;
    }

    if (!(difficultyEntry->Flags & DIFFICULTY_FLAG_CAN_SELECT))
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: %d sent unselectable instance mode %u!",
                       m_Player->GetGUIDLow(), difficultyEntry->ID);
        return;
    }

    if (((difficultyEntry->Flags & DIFFICULTY_FLAG_LEGACY) >> 5) != l_IsLegacyDifficulty)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetDungeonDifficultyOpcode: %d sent not matching legacy difficulty %u!",
                       m_Player->GetGUIDLow(), difficultyEntry->ID);
        return;
    }

    Difficulty difficultyID = Difficulty(difficultyEntry->ID);
    if (difficultyID == (l_IsLegacyDifficulty ? m_Player->GetLegacyRaidDifficultyID() : m_Player->GetRaidDifficultyID()))
        return;

    // cannot reset while in an instance
    Map* l_Map = m_Player->GetMap();
    if (l_Map && l_Map->IsDungeon())
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetRaidDifficultyOpcode: player %d tried to reset the instance while inside!", m_Player->GetGUIDLow());
        return;
    }

    GroupPtr group = m_Player->GetGroup();
    if (group)
    {
        if (group->IsLeader(m_Player->GetGUID()))
        {
            bool l_Error = group->GetMemberSlots().exist([&](Group::MemberSlotPtr l_Member) -> bool
            {
                Player* groupGuy = l_Member->player;
                if (!groupGuy)
                    return false;

                if (!groupGuy->IsInMap(groupGuy))
                    return true;

                if (groupGuy->GetMap()->IsRaid())
                {
                    sLog->outError(LOG_FILTER_NETWORKIO, "WorldSession::HandleSetRaidDifficultyOpcode: player %d tried to reset the instance while inside!", m_Player->GetGUIDLow());
                    return true;
                }

                return false;
            });

            if (l_Error)
                return;

            // the difficulty is set even if the instances can't be reset
            group->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true, l_IsLegacyDifficulty, m_Player);

            if (l_IsLegacyDifficulty)
                group->SetLegacyRaidDifficultyID(Difficulty(l_Difficulty));
            else
                group->SetRaidDifficultyID(Difficulty(l_Difficulty));
        }
    }
    else
    {
        m_Player->ResetInstances(INSTANCE_RESET_CHANGE_DIFFICULTY, true, l_IsLegacyDifficulty);

        if (l_IsLegacyDifficulty)
            m_Player->SetLegacyRaidDifficultyID(Difficulty(l_Difficulty));
        else
            m_Player->SetRaidDifficultyID(Difficulty(l_Difficulty));

        m_Player->SendRaidDifficulty(l_IsLegacyDifficulty);
    }
}

void WorldSession::HandleMoveSetCanFlyAckOpcode(WorldPacket & recvData)
{
}

void WorldSession::HandleCancelMountAuraOpcode(WorldPacket& /*recvData*/)
{
    // If player is not mounted, so go out :)
    if (!m_Player->IsMounted())                              // not blizz like; no any messages on blizz
    {
        ChatHandler(this).SendSysMessage(LANG_CHAR_NON_MOUNTED);
        return;
    }

#ifndef CROSS
    if (auto l_GarrisonMgr = m_Player->GetDraenorGarrison())
    {
        if (l_GarrisonMgr->IsTrainingMount())
            return;
    }
#endif

    if (m_Player->isInFlight())                               // not blizz like; no any messages on blizz
    {
        ChatHandler(this).SendSysMessage(LANG_YOU_IN_FLIGHT);
        return;
    }

    m_Player->RemoveAurasByType(SPELL_AURA_MOUNTED);
    m_Player->RemoveAurasByType(SPELL_AURA_MOUNTED_VISUAL);

    WorldPacket l_Data(SMSG_DISMOUNT);
    l_Data.appendPackGUID(m_Player->GetGUID());
    m_Player->SendMessageToSet(&l_Data, true);

}

void WorldSession::HandleRequestPetInfoOpcode(WorldPacket& /*recvData */)
{
    /*
    sLog->outDebug(LOG_FILTER_PACKETIO, "WORLD: CMSG_REQUEST_PET_INFO");
    recvData.hexlike();
    */
}

void WorldSession::HandleSetTaxiBenchmarkOpcode(WorldPacket& recvData)
{
    bool mode = recvData.ReadBit(); ///< mode is unused
}

void WorldSession::HandleQueryInspectAchievements(WorldPacket& p_RecvData)
{
    uint64 l_Guid = 0;
    p_RecvData.readPackGUID(l_Guid);

    Player* l_Player = ObjectAccessor::GetPlayer(*m_Player, l_Guid);
    if (!l_Player)
        return;

    l_Player->SendRespondInspectAchievements(m_Player);
}

void WorldSession::HandleGuildAchievementProgressQuery(WorldPacket& p_Packet)
{
#ifndef CROSS
    uint32 l_AchievementID = 0;

    p_Packet >> l_AchievementID;

    if (Guild * l_Guild = sGuildMgr->GetGuildById(m_Player->GetGuildId()))
        l_Guild->GetAchievementMgr().SendAchievementInfo(m_Player, l_AchievementID);
#endif
}

void WorldSession::HandleWorldStateUITimerUpdate(WorldPacket& /*p_RecvData*/)
{
    WorldPacket l_Data(SMSG_UI_TIME, 4);
    l_Data << uint32(time(nullptr));         ///< Time
    SendPacket(&l_Data);
}

/// - Blizzard have merge CMSG_GET_UNDELETE_CHARACTER_COOLDOWN_STATUS & CMSG_READY_FOR_ACCOUNT_DATA_TIMES (see history of CharacterSelect_OnShow function in "GlueXML/CharacterSelect.lua" of WoW UI Source)
void WorldSession::HandleUndeleteCharacter(WorldPacket& /*p_RecvData*/)
{
    SendAccountDataTimes(GLOBAL_CACHE_MASK);
}

void WorldSession::SendSetPhaseShift(const std::set<uint32>& p_PhaseIds, const std::set<uint32>& p_VisibleMapIDsCount, const std::set<uint32>& l_PreloadMapIDsCount, const std::set<uint32>& p_UIWorldMapAreaIDSwaps)
{
    if (!m_Player || !m_Player->IsInWorld())
        return;

    WorldPacket l_ShiftPacket(SMSG_PHASE_SHIFT_CHANGE, 500);
    l_ShiftPacket.appendPackGUID(m_Player->GetGUID());           ///< ClientGUID
    l_ShiftPacket << uint32(0x18);                               ///< PhaseShiftFlags
    l_ShiftPacket << uint32(p_PhaseIds.size());                  ///< PhaseShiftCount
    l_ShiftPacket.appendPackGUID(0);                             ///< PersonalGUID

    for (std::set<uint32>::const_iterator l_It = p_PhaseIds.begin(); l_It != p_PhaseIds.end(); ++l_It)
    {
        l_ShiftPacket << uint16(1);                              ///< PhaseFlags
        l_ShiftPacket << uint16(*l_It);                          ///< PhaseID
    }

    /// Active terrain swaps
    l_ShiftPacket << uint32(p_VisibleMapIDsCount.size() * 2);    ///< UI map swaps size (in bytes)
    for (std::set<uint32>::const_iterator l_It = p_VisibleMapIDsCount.begin(); l_It != p_VisibleMapIDsCount.end(); ++l_It)
        l_ShiftPacket << uint16(*l_It);

    /// Inactive terrain swaps
    l_ShiftPacket << uint32(l_PreloadMapIDsCount.size() * 2);    ///< PreloadMapIDsCount (in bytes)
    for (std::set<uint32>::const_iterator l_It = l_PreloadMapIDsCount.begin(); l_It != l_PreloadMapIDsCount.end(); ++l_It)
        l_ShiftPacket << uint16(*l_It);

    /// UI WorldMapAreaID swaps
    l_ShiftPacket << uint32(p_UIWorldMapAreaIDSwaps.size() * 2); ///< UIWorldMapAreaIDSwapsCount (in bytes)
    for (std::set<uint32>::const_iterator l_It = p_UIWorldMapAreaIDSwaps.begin(); l_It != p_UIWorldMapAreaIDSwaps.end(); ++l_It)
       l_ShiftPacket << uint16(*l_It);

    SendPacket(&l_ShiftPacket);
}

// Battlefield and Battleground
void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket& p_Packet)
{
    uint64 l_Healer = 0;
    p_Packet.readPackGUID(l_Healer);

    Creature * l_Unit = m_Player->GetMap()->GetCreature(l_Healer);
    if (!l_Unit)
        return;

    /// It's not spirit service
    if (!l_Unit->isSpiritService())
        return;

    if (Battleground* l_Battleground = m_Player->GetBattleground())
        MS::Battlegrounds::PacketFactory::AreaSpiritHealerQuery(m_Player, l_Battleground, l_Healer);
    else if (Battlefield * l_Battlefield = sBattlefieldMgr->GetBattlefieldToZoneId(m_Player->GetZoneId()))
        l_Battlefield->SendAreaSpiritHealerQueryOpcode(m_Player, l_Healer);
    else if (OutdoorPvP* l_OutdoorPvP = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(m_Player->GetZoneId()))
        l_OutdoorPvP->SendAreaSpiritHealerQueryOpcode(m_Player, l_Healer);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket& p_Packet)
{
    uint64 l_Healer = 0;
    p_Packet.readPackGUID(l_Healer);

    Creature* l_Unit = m_Player->GetMap()->GetCreature(l_Healer);
    if (!l_Unit)
        return;

    /// it's not spirit service
    if (!l_Unit->isSpiritService())
        return;

    if (Battleground* l_Battleground = m_Player->GetBattleground())
        l_Battleground->AddPlayerToResurrectQueue(l_Healer, m_Player->GetGUID());
    else if (Battlefield * l_Battlefield = sBattlefieldMgr->GetBattlefieldToZoneId(m_Player->GetZoneId()))
        l_Battlefield->AddPlayerToResurrectQueue(l_Healer, m_Player->GetGUID());
    else if (OutdoorPvP* l_OutdoorPvP = sOutdoorPvPMgr->GetOutdoorPvPToZoneId(m_Player->GetZoneId()))
        l_OutdoorPvP->AddPlayerToResurrectQueue(l_Healer, m_Player->GetGUID());
}

void WorldSession::HandleHearthAndResurrect(WorldPacket& /*recvData*/)
{
    if (m_Player->isInFlight())
        return;

    if (/*Battlefield* bf =*/ sBattlefieldMgr->GetBattlefieldToZoneId(m_Player->GetZoneId()))
    {
        // bf->PlayerAskToLeave(_player);                   //@todo: FIXME
        return;
    }

    AreaTableEntry const* atEntry = sAreaTableStore.LookupEntry(m_Player->GetAreaId());
    if (!atEntry || !(atEntry->Flags & AREA_FLAG_CAN_HEARTH_AND_RESURRECT))
        return;

    m_Player->BuildPlayerRepop();
    m_Player->ResurrectPlayer(100);
    m_Player->TeleportTo(m_Player->m_homebindMapId, m_Player->m_homebindX, m_Player->m_homebindY, m_Player->m_homebindZ, m_Player->GetOrientation());
}

void WorldSession::HandleUpdateMissileTrajectory(WorldPacket& recvPacket)
{
    uint64 guid;
    uint32 spellId;
    float elevation, speed;
    float curX, curY, curZ;
    float targetX, targetY, targetZ;
    uint8 moveStop;

    recvPacket >> guid >> spellId >> elevation >> speed;
    recvPacket >> curX >> curY >> curZ;
    recvPacket >> targetX >> targetY >> targetZ;
    recvPacket >> moveStop;

    Unit* caster = ObjectAccessor::GetUnit(*m_Player, guid);
    Spell* spell = caster ? caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) : NULL;
    if (!spell || spell->m_spellInfo->Id != spellId || !spell->m_targets.HasDst() || !spell->m_targets.HasSrc())
    {
        recvPacket.rfinish();
        return;
    }

    Position pos = *spell->m_targets.GetSrcPos();
    pos.Relocate(curX, curY, curZ);
    spell->m_targets.ModSrc(pos);

    pos = *spell->m_targets.GetDstPos();
    pos.Relocate(targetX, targetY, targetZ);
    spell->m_targets.ModDst(pos);

    spell->m_targets.SetElevation(elevation);
    spell->m_targets.SetSpeed(speed);

    if (moveStop)
    {
        uint32 opcode;
        recvPacket >> opcode;
        recvPacket.SetOpcode(CMSG_MOVE_STOP); // always set to MSG_MOVE_STOP in client SetOpcode
        HandleMovementOpcodes(recvPacket);
    }
}

void WorldSession::HandleViolenceLevel(WorldPacket& recvPacket)
{
    uint8 violenceLevel;
    recvPacket >> violenceLevel;

    // do something?
}

void WorldSession::HandleObjectUpdateFailedOpcode(WorldPacket& p_Packet)
{
    uint64 l_ObjectGUID = 0;

    p_Packet.readPackGUID(l_ObjectGUID);

    if (m_Player->GetGUID() == l_ObjectGUID)
        return;

    /// Force visibility update
    m_Player->m_clientGUIDs_lock.acquire_write();
    m_Player->m_clientGUIDs.erase(l_ObjectGUID);
    m_Player->m_clientGUIDs_lock.release();

    m_Player->UpdateObjectVisibility();

    WorldObject* l_WorldObject = ObjectAccessor::GetWorldObject(*m_Player, l_ObjectGUID);
    sLog->outAshran("Object update failed for object " UI64FMTD " (%s) for player %s (%u), on map %u",
        uint64(l_ObjectGUID), l_WorldObject ? l_WorldObject->GetName() : "object-not-found", GetPlayerName().c_str(), GetGuidLow(), m_Player->GetMapId());
}

// DestrinyFrame.xml : lua function NeutralPlayerSelectFaction
#define JOIN_THE_ALLIANCE 1
#define JOIN_THE_HORDE    0

void WorldSession::HandleSetFactionOpcode(WorldPacket& recvPacket)
{
    uint32 choice = recvPacket.read<uint32>();

    if (m_Player->getRace() != RACE_PANDAREN_NEUTRAL)
        return;

    if (choice == JOIN_THE_HORDE)
    {
        m_Player->SetByteValue(UNIT_FIELD_SEX, UNIT_BYTES_0_OFFSET_RACE, RACE_PANDAREN_HORDE);
        m_Player->SetOriginalRace(RACE_PANDAREN_HORDE);
        m_Player->setFactionForRace(RACE_PANDAREN_HORDE);
        m_Player->SaveToDB();
        WorldLocation location(1, 1366.730f, -4371.248f, 26.070f, 3.1266f);

        if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
        {
            location = WorldLocation(1, 1640.66f, -4358.37f, 26.75f, 0.498f);

            /// Add mounts
            m_Player->AddItem(49046, 1);
            m_Player->AddItem(107951, 1);
        }

        m_Player->TeleportTo(location);
        m_Player->SetHomebind(location, 363);
        m_Player->learnSpell(669, false); // Language Orcish
        m_Player->learnSpell(108127, false); // Language Pandaren
    }
    else if (choice == JOIN_THE_ALLIANCE)
    {
        m_Player->SetByteValue(UNIT_FIELD_SEX, UNIT_BYTES_0_OFFSET_RACE, RACE_PANDAREN_ALLIANCE);
        m_Player->SetOriginalRace(RACE_PANDAREN_ALLIANCE);
        m_Player->setFactionForRace(RACE_PANDAREN_ALLIANCE);
        m_Player->SaveToDB();
        WorldLocation location(0, -9096.236f, 411.380f, 92.257f, 3.649f);

        if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
        {
            location = WorldLocation(0, -8383.959f, 251.794f, 155.34f, 0.66f);

            /// Add mounts
            m_Player->AddItem(47180, 1);
            m_Player->AddItem(98259, 1);
        }

        m_Player->TeleportTo(location);
        m_Player->SetHomebind(location, 9);
        m_Player->learnSpell(668, false); // Language Common
        m_Player->learnSpell(108127, false); // Language Pandaren
    }

    if (m_Player->GetQuestStatus(31450) == QUEST_STATUS_INCOMPLETE)
        m_Player->KilledMonsterCredit(64594);

    sScriptMgr->OnPlayerFactionChanged(m_Player);

    if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
        return;

    m_Player->SendMovieStart(116);
}

void WorldSession::HandleCategoryCooldownOpcode(WorldPacket& /*recvPacket*/)
{
    Unit::AuraEffectList const& list = GetPlayer()->GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_CATEGORY_COOLDOWN);

    WorldPacket data(SMSG_CATEGORY_COOLDOWN, 4 + (int(list.size()) * 8));
    data << uint32(list.size());
    for (Unit::AuraEffectList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
    {
        AuraEffect* effect = *itr;
        if (!effect)
            continue;

        data << uint32(effect->GetMiscValue());
        data << int32(-effect->GetAmount());
    }

    SendPacket(&data);
}

void WorldSession::HandleSaveCUFProfiles(WorldPacket& p_RecvPacket)
{
    uint32 l_ProfileCount;

    p_RecvPacket >> l_ProfileCount;

    if (l_ProfileCount > MAX_CUF_PROFILES)
    {
        p_RecvPacket.rfinish();
        return;
    }

    CUFProfiles& l_Profiles = GetPlayer()->m_cufProfiles;
    l_Profiles.resize(l_ProfileCount);

    for (uint32 l_I = 0; l_I < l_ProfileCount; ++l_I)
    {
        CUFProfile& l_Profile = l_Profiles[l_I];
        CUFProfileData& l_ProfileData = l_Profile.data;

        l_Profile.l_NameLen = p_RecvPacket.ReadBits(7);

        if (l_Profile.l_NameLen > MAX_CUF_PROFILE_NAME_LENGTH)
        {
            p_RecvPacket.rfinish();
            return;
        }

        l_ProfileData.KeepGroupsTogether            = p_RecvPacket.ReadBit();
        l_ProfileData.DisplayPets                   = p_RecvPacket.ReadBit();
        l_ProfileData.DisplayMainTankAndAssist      = p_RecvPacket.ReadBit();
        l_ProfileData.DisplayHealPrediction         = p_RecvPacket.ReadBit();
        l_ProfileData.DisplayAggroHighlight         = p_RecvPacket.ReadBit();
        l_ProfileData.DisplayOnlyDispellableDebuffs = p_RecvPacket.ReadBit();
        l_ProfileData.DisplayPowerBar               = p_RecvPacket.ReadBit();
        l_ProfileData.DisplayBorder                 = p_RecvPacket.ReadBit();
        l_ProfileData.UseClassColors                = p_RecvPacket.ReadBit();
        l_ProfileData.HorizontalGroups              = p_RecvPacket.ReadBit();
        l_ProfileData.DisplayNonBossDebuffs         = p_RecvPacket.ReadBit();
        l_ProfileData.DynamicPosition               = p_RecvPacket.ReadBit();
        l_ProfileData.Locked                        = p_RecvPacket.ReadBit();
        l_ProfileData.Shown                         = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivate2Players          = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivate3Players          = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivate5Players          = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivate10Players         = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivate15Players         = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivate25Players         = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivate40Players         = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivateSpec1             = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivateSpec2             = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivateSpec3             = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivateSpec4             = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivatePvP               = p_RecvPacket.ReadBit();
        l_ProfileData.AutoActivatePvE               = p_RecvPacket.ReadBit();

        p_RecvPacket >> l_ProfileData.FrameHeight;
        p_RecvPacket >> l_ProfileData.FrameWidth;
        p_RecvPacket >> l_ProfileData.SortBy;
        p_RecvPacket >> l_ProfileData.HealthText;
        p_RecvPacket >> l_ProfileData.TopPoint;
        p_RecvPacket >> l_ProfileData.BottomPoint;
        p_RecvPacket >> l_ProfileData.LeftPoint;
        p_RecvPacket >> l_ProfileData.TopOffset;
        p_RecvPacket >> l_ProfileData.BottomOffset;
        p_RecvPacket >> l_ProfileData.LeftOffset;

        l_Profile.Name = p_RecvPacket.ReadString(l_Profile.l_NameLen);
    }

    m_Player->SendCUFProfiles();

    SQLTransaction l_Transaction = SessionRealmDatabase.BeginTransaction();

    PreparedStatement* l_Stmt = SessionRealmDatabase.GetPreparedStatement(CHAR_DEL_CUF_PROFILE);
    l_Stmt->setUInt32(0, GetPlayer()->GetRealGUIDLow());
    l_Transaction->Append(l_Stmt);

    for (uint32 l_I = 0; l_I < l_ProfileCount; ++l_I)
    {
        CUFProfile& profile = l_Profiles[l_I];
        CUFProfileData data = profile.data;

        l_Stmt = SessionRealmDatabase.GetPreparedStatement(CHAR_INS_CUF_PROFILE);
        l_Stmt->setUInt32(0, GetPlayer()->GetRealGUIDLow());
        l_Stmt->setString(1, profile.Name);
        l_Stmt->setString(2, PackDBBinary(&data, sizeof(CUFProfileData)));
        l_Transaction->Append(l_Stmt);
    }

    SessionRealmDatabase.CommitTransaction(l_Transaction);
}

void WorldSession::HandleMountSetFavoriteOpcode(WorldPacket & p_Packet)
{
    uint32  l_MountSpellID = 0;
    bool    l_IsFavorite   = false;

    p_Packet >> l_MountSpellID;
    l_IsFavorite = p_Packet.ReadBit();

    if (!m_Player)
        return;

    m_Player->MountSetFavorite(l_MountSpellID, l_IsFavorite);
}

void WorldSession::HandleRequestTwitterStatus(WorldPacket& /*p_RecvData*/)
{
    SendTwitterStatus(true);
}

void WorldSession::SendTwitterStatus(bool p_Enabled)
{
    if (!m_Player || !m_Player->IsInWorld())
        return;

    {
        WorldPacket l_Data(SMSG_OAUTH_SAVED_DATA);
        l_Data << uint32(0); ///< Hax0red struct not needed
        l_Data.FlushBits();
        SendPacket(&l_Data);
    }

    {
        WorldPacket l_Data(SMSG_TWITTER_STATUS);
        l_Data.WriteBit(p_Enabled); ///< Enabled
        l_Data.FlushBits();
        l_Data << uint32(2);        ///< Unk loop 1 (uint32)
        l_Data << uint32(2);        ///< Unk loop 2 (1 bit)
        l_Data << uint32(69776);
        l_Data << uint32(88584);
        l_Data.WriteBit(0);
        l_Data.WriteBit(0);
        l_Data.FlushBits();
        SendPacket(&l_Data);
    }
}

void WorldSession::HandlePrestige(WorldPacket& p_RecvData)
{
    m_Player->Prestige(true);
}

void WorldSession::HandleRequestCrowdControlSpell(WorldPacket& p_RecvData)
{
    uint64 l_Guid = 0;

    p_RecvData.readPackGUID(l_Guid);

    Player* l_Player = Player::GetPlayer(*m_Player, l_Guid);
    if (!l_Player)
        return;

    if (Battleground* l_BG = m_Player->GetBattleground())
        l_BG->SendArenaCrowdControlSpells(m_Player, l_Player);
}

void WorldSession::HandleRequestAreaPOIUpdate(WorldPacket& p_RecvData)
{
    WorldPacket l_Data;
    sPOIMgr->BuildAreaPOIUpdatePacket(l_Data, GetPlayer()->GetGUID());

    SendPacket(&l_Data);
}

#ifdef CROSS
void WorldSession::HandleQueryGuildInfoOpcode(WorldPacket& p_Packet)
{
    uint64 l_GuildGuid  = 0;
    uint64 l_PlayerGuid = 0;

    p_Packet.readPackGUID(l_GuildGuid);
    p_Packet.readPackGUID(l_PlayerGuid);

    Player* targetPlayer = sObjectAccessor->FindPlayerInOrOutOfWorld(l_PlayerGuid);
    if (!targetPlayer)
        return;

    sGuildMgr->HandleInterRealmGuildQuery(GetPlayer(), targetPlayer->GetSession()->GetInterRealmNumber(), l_GuildGuid);
}
#endif

void WorldSession::HandleContributionGetState(WorldPacket& p_RecvPacket)
{
    WorldPackets::Quest::ContributionGetState l_Packet(p_RecvPacket);
    l_Packet.Read();

    WorldPackets::Quest::ContributionSendState l_Response;
    l_Response.Progress = sContributionMgr->GetStartTime(l_Packet.ContributionID);
    l_Response.OccurenceValue = sContributionMgr->GetOccurenceValue(l_Packet.ContributionID);
    l_Response.ContributionID = l_Packet.ContributionID;

    SendPacket(l_Response.Write());
}

void WorldSession::HandleContributionContribute(WorldPacket& p_RecvPacket)
{
    WorldPackets::Quest::ContributionContribute l_Packet(p_RecvPacket);
    l_Packet.Read();

    sContributionMgr->PlayerContribution(GetPlayer(), l_Packet.ContributionID);
}
