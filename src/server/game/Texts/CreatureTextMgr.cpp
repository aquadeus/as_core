////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CreatureTextMgr.h"

void CreatureTextMgr::LoadCreatureTexts()
{
    uint32 l_OldMSTime = getMSTime();

    mTextMap.clear();       ///< For reload case
    mTextRepeatMap.clear(); ///< Reset all currently used temp texts

    PreparedStatement* l_Statement = WorldDatabase.GetPreparedStatement(WorldDatabaseStatements::WORLD_SEL_CREATURE_TEXT);
    PreparedQueryResult l_Result = WorldDatabase.Query(l_Statement);

    if (!l_Result)
    {
        sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded 0 ceature texts. DB table `creature_texts` is empty.");
        return;
    }

    uint32 l_TextCount = 0;
    uint32 l_CreatureCount = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();
        uint8 l_Index = 0;

        CreatureTextEntry l_TempTextEntry;

        l_TempTextEntry.entry              = l_Fields[l_Index++].GetUInt32();
        l_TempTextEntry.groupId            = l_Fields[l_Index++].GetUInt8();
        l_TempTextEntry.id                 = l_Fields[l_Index++].GetUInt8();
        l_TempTextEntry.text               = l_Fields[l_Index++].GetString();
        l_TempTextEntry.type               = ChatMsg(l_Fields[l_Index++].GetUInt8());
        l_TempTextEntry.lang               = Language(l_Fields[l_Index++].GetUInt8());
        l_TempTextEntry.probability        = l_Fields[l_Index++].GetFloat();
        l_TempTextEntry.emote              = Emote(l_Fields[l_Index++].GetUInt32());
        l_TempTextEntry.duration           = l_Fields[l_Index++].GetUInt32();
        l_TempTextEntry.sound              = l_Fields[l_Index++].GetUInt32();
        l_TempTextEntry.BroadcastTextID    = l_Fields[l_Index++].GetUInt32();

        if (l_TempTextEntry.sound)
        {
            if (!sSoundKitStore.LookupEntry(l_TempTextEntry.sound))
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "CreatureTextMgr:  Entry %u, Group %u in table `creature_texts` has Sound %u but sound does not exist.", l_TempTextEntry.entry, l_TempTextEntry.groupId, l_TempTextEntry.sound);
                l_TempTextEntry.sound = 0;
            }
        }

        if (!GetLanguageDescByID(l_TempTextEntry.lang))
        {
            sLog->outError(LogFilterType::LOG_FILTER_SQL, "CreatureTextMgr:  Entry %u, Group %u in table `creature_texts` using Language %u but Language does not exist.", l_TempTextEntry.entry, l_TempTextEntry.groupId, uint32(l_TempTextEntry.lang));
            l_TempTextEntry.lang = Language::LANG_UNIVERSAL;
        }

        if (l_TempTextEntry.type >= ChatMsg::MAX_CHAT_MSG_TYPE)
        {
            sLog->outError(LogFilterType::LOG_FILTER_SQL, "CreatureTextMgr:  Entry %u, Group %u in table `creature_texts` has Type %u but this Chat Type does not exist.", l_TempTextEntry.entry, l_TempTextEntry.groupId, uint32(l_TempTextEntry.type));
            l_TempTextEntry.type = ChatMsg::CHAT_MSG_SAY;
        }

        if (l_TempTextEntry.emote)
        {
            if (!sEmotesStore.LookupEntry(l_TempTextEntry.emote))
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "CreatureTextMgr:  Entry %u, Group %u in table `creature_texts` has Emote %u but emote does not exist.", l_TempTextEntry.entry, l_TempTextEntry.groupId, uint32(l_TempTextEntry.emote));
                l_TempTextEntry.emote = Emote::EMOTE_ONESHOT_NONE;
            }
        }

        if (l_TempTextEntry.BroadcastTextID)
        {
            if (!sBroadcastTextStore.LookupEntry(l_TempTextEntry.BroadcastTextID))
            {
                sLog->outError(LogFilterType::LOG_FILTER_SQL, "CreatureTextMgr: Entry %u, Group %u, Id %u in table `creature_text` has non - existing or incompatible BroadcastTextID %u.", l_TempTextEntry.entry, l_TempTextEntry.groupId, l_TempTextEntry.id, l_TempTextEntry.BroadcastTextID);
                l_TempTextEntry.BroadcastTextID = 0;
            }
        }

        /// Entry not yet added, add empty TextHolder (list of groups)
        if (mTextMap.find(l_TempTextEntry.entry) == mTextMap.end())
            ++l_CreatureCount;

        /// Add the text into our entry's group
        mTextMap[l_TempTextEntry.entry][l_TempTextEntry.groupId].push_back(l_TempTextEntry);

        ++l_TextCount;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature texts for %u creatures in %u ms", l_TextCount, l_CreatureCount, GetMSTimeDiffToNow(l_OldMSTime));
}

void CreatureTextMgr::LoadCreatureTextLocales()
{
    uint32 oldMSTime = getMSTime();

    mLocaleTextMap.clear(); // for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, textGroup, id, text_loc1, text_loc2, text_loc3, text_loc4, text_loc5, text_loc6, text_loc7, text_loc8, text_loc9, text_loc10 FROM locales_creature_text");

    if (!result)
        return;

    uint32 textCount = 0;

    do
    {
        Field* fields = result->Fetch();
        CreatureTextLocale& loc = mLocaleTextMap[CreatureTextId(fields[0].GetUInt32(), uint32(fields[1].GetUInt8()), fields[2].GetUInt32())];
        for (uint8 i = 1; i < TOTAL_LOCALES; ++i)
        {
            LocaleConstant locale = LocaleConstant(i);
            ObjectMgr::AddLocaleString(fields[3 + i - 1].GetString(), locale, loc.Text);
        }

        ++textCount;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature localized texts in %u ms", textCount, GetMSTimeDiffToNow(oldMSTime));

}

uint32 CreatureTextMgr::SendChat(Creature* source, uint8 textGroup, uint64 whisperGuid /*= 0*/, ChatMsg msgType /*= CHAT_MSG_ADDON*/, Language language /*= LANG_ADDON*/, TextRange range /*= TEXT_RANGE_NORMAL*/, uint32 sound /*= 0*/, Team team /*= TEAM_OTHER*/, bool gmOnly /*= false*/, Player* srcPlr /*= NULL*/, bool p_Personal)
{
    if (!source)
        return 0;

    CreatureTextMap::const_iterator sList = mTextMap.find(source->GetEntry());
    if (sList == mTextMap.end())
        return 0;

    CreatureTextHolder const& textHolder = sList->second;
    CreatureTextHolder::const_iterator itr = textHolder.find(textGroup);
    if (itr == textHolder.end())
        return 0;

    CreatureTextGroup const& textGroupContainer = itr->second;  //has all texts in the group
    CreatureTextRepeatIds repeatGroup = GetRepeatGroup(source, textGroup);//has all textIDs from the group that were already said
    CreatureTextGroup tempGroup;//will use this to talk after sorting repeatGroup

    for (CreatureTextGroup::const_iterator giter = textGroupContainer.begin(); giter != textGroupContainer.end(); ++giter)
        if (std::find(repeatGroup.begin(), repeatGroup.end(), giter->id) == repeatGroup.end())
            tempGroup.push_back(*giter);

    if (tempGroup.empty())
    {
        std::lock_guard<std::mutex> l_Guard(m_TextRepeatLock);

        CreatureTextRepeatMap::iterator mapItr = mTextRepeatMap.find(source->GetGUID());
        if (mapItr != mTextRepeatMap.end())
        {
            CreatureTextRepeatGroup::iterator groupItr = mapItr->second.find(textGroup);
            groupItr->second.clear();
        }

        tempGroup = textGroupContainer;
    }

    uint8 count = 0;
    float lastChance = -1;
    bool isEqualChanced = true;

    float totalChance = 0;

    for (CreatureTextGroup::const_iterator iter = tempGroup.begin(); iter != tempGroup.end(); ++iter)
    {
        if (lastChance >= 0 && lastChance != iter->probability)
            isEqualChanced = false;

        lastChance = iter->probability;
        totalChance += iter->probability;
        ++count;
    }

    int32 offset = -1;
    if (!isEqualChanced)
    {
        for (CreatureTextGroup::const_iterator iter = tempGroup.begin(); iter != tempGroup.end(); ++iter)
        {
            uint32 chance = uint32(iter->probability);
            uint32 r = urand(0, 100);
            ++offset;
            if (r <= chance)
                break;
        }
    }

    uint32 pos = 0;
    if (isEqualChanced || offset < 0)
        pos = urand(0, count - 1);
    else if (offset >= 0)
        pos = offset;

    CreatureTextGroup::const_iterator iter = tempGroup.begin() + pos;

    ChatMsg finalType = (msgType == CHAT_MSG_ADDON) ? iter->type : msgType;
    Language finalLang = (language == LANG_ADDON) ? iter->lang : language;
    uint32 finalSound = sound ? sound : iter->sound;

    if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(iter->BroadcastTextID))
    {
        uint8 l_Gender = source->getGender();
        if (l_Gender < Gender::GENDER_NONE && l_BroadcastText->SoundID[l_Gender])
            finalSound = l_BroadcastText->SoundID[l_Gender];
        else if (l_Gender == Gender::GENDER_NONE)
            finalSound = l_BroadcastText->SoundID[0] ? l_BroadcastText->SoundID[0] : l_BroadcastText->SoundID[1];
    }

    if (finalSound)
        SendSound(source, finalSound, finalType, whisperGuid, range, team, gmOnly, p_Personal);

    Unit* finalSource = source;
    if (srcPlr)
        finalSource = srcPlr;

    if (iter->emote)
        SendEmote(finalSource, iter->emote, p_Personal ? whisperGuid : 0);

    CreatureTextBuilder builder(finalSource, finalSource->getGender(), finalType, iter->groupId, iter->id, finalLang, whisperGuid, iter->BroadcastTextID, false);
    SendChatPacket(finalSource, builder, finalType, whisperGuid, range, team, gmOnly, p_Personal);
    if (isEqualChanced || (!isEqualChanced && totalChance == 100.0f))
        SetRepeatId(source, textGroup, iter->id);

    return iter->duration;
}

float CreatureTextMgr::GetRangeForChatType(ChatMsg msgType) const
{
    float dist = sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY);
    switch (msgType)
    {
        case CHAT_MSG_MONSTER_YELL:
            dist = sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL);
            break;
        case CHAT_MSG_MONSTER_EMOTE:
        case CHAT_MSG_RAID_BOSS_EMOTE:
            dist = sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE);
            break;
        default:
            break;
    }

    return dist;
}

void CreatureTextMgr::SendSound(Creature * p_Source, uint32 p_SoundKitID, ChatMsg p_MsgType, uint64 p_WhisperGuid, TextRange p_Range, Team p_Team, bool p_GMOnly, bool p_Personal)
{
    if (!p_SoundKitID || !p_Source)
        return;

    WorldPacket l_Data(SMSG_PLAY_SOUND, 2 + 16 + 4);
    l_Data << uint32(p_SoundKitID);
    l_Data.appendPackGUID(p_Source->GetGUID());

    if (p_Personal)
    {
        Player* l_Player = ObjectAccessor::FindPlayer(p_WhisperGuid);
        if (!l_Player || !l_Player->GetSession())
            return;

        l_Player->GetSession()->SendPacket(&l_Data);
    }
    else
        SendNonChatPacket(p_Source, &l_Data, p_MsgType, p_WhisperGuid, p_Range, p_Team, p_GMOnly);
}

void CreatureTextMgr::SendNonChatPacket(WorldObject* source, WorldPacket* data, ChatMsg msgType, uint64 whisperGuid, TextRange range, Team team, bool gmOnly) const
{
    switch (msgType)
    {
        case CHAT_MSG_MONSTER_WHISPER:
        case CHAT_MSG_RAID_BOSS_WHISPER:
        {
            if (range == TEXT_RANGE_NORMAL)//ignores team and gmOnly
            {
                Player* player = ObjectAccessor::FindPlayer(whisperGuid);
                if (!player || !player->GetSession())
                    return;
                player->GetSession()->SendPacket(data);
                return;
            }
            break;
        }
        default:
            break;
    }

    switch (range)
    {
        case TEXT_RANGE_AREA:
        {
            uint32 areaId = source->GetAreaId();
            Map::PlayerList const& players = source->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if (itr->getSource()->GetAreaId() == areaId && itr->getSource()->InSamePhase(source) && (!team || Team(itr->getSource()->GetTeam()) == team) && (!gmOnly || itr->getSource()->isGameMaster()))
                    itr->getSource()->GetSession()->SendPacket(data);
            return;
        }
        case TEXT_RANGE_ZONE:
        {
            uint32 zoneId = source->GetZoneId();
            Map::PlayerList const& players = source->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if (itr->getSource()->GetZoneId() == zoneId && (!team || Team(itr->getSource()->GetTeam()) == team) && (!gmOnly || itr->getSource()->isGameMaster()))
                    itr->getSource()->GetSession()->SendPacket(data);
            return;
        }
        case TEXT_RANGE_MAP:
        {
            Map::PlayerList const& players = source->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if ((!team || Team(itr->getSource()->GetTeam()) == team) && (!gmOnly || itr->getSource()->isGameMaster()))
                    itr->getSource()->GetSession()->SendPacket(data);
            return;
        }
        case TEXT_RANGE_WORLD:
        {
            sWorld->GetSessionsLock().acquire_read();
#ifndef CROSS
            SessionMap const& smap = sWorld->GetAllSessions();
            for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
                if (Player* player = iter->second->GetPlayer())
#else /* CROSS */
            PlayerMap const& players = sWorld->GetAllPlayers();
            for (PlayerMap::const_iterator iter = players.begin(); iter != players.end(); ++iter)
                if (Player* player = iter->second)
#endif /* CROSS */
                    if (player->GetSession()  && (!team || Team(player->GetTeam()) == team) && (!gmOnly || player->isGameMaster()))
                        player->GetSession()->SendPacket(data);
            sWorld->GetSessionsLock().release();
            return;
        }
        case TEXT_RANGE_NORMAL:
        default:
            break;
    }

    source->SendMessageToSet(data, true);
}

void CreatureTextMgr::SendEmote(Unit* p_Source, uint32 p_EmoteId, uint64 p_ReceiverGuid)
{
    if (!p_Source)
        return;

    p_Source->HandleEmoteCommand(p_EmoteId, p_ReceiverGuid);
}

void CreatureTextMgr::SetRepeatId(Creature* source, uint8 textGroup, uint8 id)
{
    if (!source)
        return;

    std::lock_guard<std::mutex> l_Guard(m_TextRepeatLock);

    CreatureTextRepeatIds& repeats = mTextRepeatMap[source->GetGUID()][textGroup];
    if (std::find(repeats.begin(), repeats.end(), id) == repeats.end())
        repeats.push_back(id);
    else
        sLog->outError(LOG_FILTER_SQL, "CreatureTextMgr: TextGroup %u for Creature(%s) GuidLow %u Entry %u, id %u already added", uint32(textGroup), source->GetName(), source->GetGUIDLow(), source->GetEntry(), uint32(id));
}

CreatureTextRepeatIds CreatureTextMgr::GetRepeatGroup(Creature* source, uint8 textGroup)
{
    ASSERT(source);//should never happen
    CreatureTextRepeatIds ids;

    std::lock_guard<std::mutex> l_Guard(m_TextRepeatLock);

    CreatureTextRepeatMap::const_iterator mapItr = mTextRepeatMap.find(source->GetGUID());
    if (mapItr != mTextRepeatMap.end())
    {
        CreatureTextRepeatGroup::const_iterator groupItr = (*mapItr).second.find(textGroup);
        if (groupItr != mapItr->second.end())
            ids = groupItr->second;
    }
    return ids;
}

bool CreatureTextMgr::TextExist(uint32 sourceEntry, uint8 textGroup)
{
    if (!sourceEntry)
        return false;

    CreatureTextMap::const_iterator sList = mTextMap.find(sourceEntry);
    if (sList == mTextMap.end())
        return false;

    CreatureTextHolder const& textHolder = sList->second;
    CreatureTextHolder::const_iterator itr = textHolder.find(textGroup);
    if (itr == textHolder.end())
        return false;

    return true;
}

std::string CreatureTextMgr::GetLocalizedChatString(uint32 entry, uint8 gender, uint8 textGroup, uint32 id, LocaleConstant locale) const
{
    CreatureTextMap::const_iterator mapitr = mTextMap.find(entry);
    if (mapitr == mTextMap.end())
        return "";

    CreatureTextHolder::const_iterator holderItr = mapitr->second.find(textGroup);
    if (holderItr == mapitr->second.end())
        return "";

    CreatureTextGroup::const_iterator groupItr = holderItr->second.begin();
    for (; groupItr != holderItr->second.end(); ++groupItr)
        if (groupItr->id == id)
            break;

    if (groupItr == holderItr->second.end())
        return "";

    if (locale > MAX_LOCALES)
        locale = DEFAULT_LOCALE;

    std::string baseText = groupItr->text;
    if (BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(groupItr->BroadcastTextID))
        baseText = GetBroadcastTextValue(l_BroadcastText, locale, gender);
    else
    {
        LocaleCreatureTextMap::const_iterator locItr = mLocaleTextMap.find(CreatureTextId(entry, uint32(textGroup), id));
        if (locItr != mLocaleTextMap.end())
            ObjectMgr::GetLocaleString(locItr->second.Text, locale, baseText);
    }

    return baseText;
}
