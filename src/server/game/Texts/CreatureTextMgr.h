////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_CREATURE_TEXT_MGR_H
#define TRINITY_CREATURE_TEXT_MGR_H

#include "Creature.h"
#include "GridNotifiers.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"

struct CreatureTextEntry
{
    uint32 entry;
    uint8 groupId;
    uint8 id;
    std::string text;
    ChatMsg type;
    Language lang;
    float probability;
    Emote emote;
    uint32 duration;
    uint32 sound;
    uint32 BroadcastTextID;
};

enum TextRange
{
    TEXT_RANGE_NORMAL   = 0,
    TEXT_RANGE_AREA     = 1,
    TEXT_RANGE_ZONE     = 2,
    TEXT_RANGE_MAP      = 3,
    TEXT_RANGE_WORLD    = 4
};

struct CreatureTextLocale
{
    StringVector Text;
};

struct CreatureTextId
{
    CreatureTextId(uint32 e, uint32 g, uint32 i) :  entry(e), textGroup(g), textId(i)
    {
    }

    bool operator<(CreatureTextId const& right) const
    {
        return memcmp(this, &right, sizeof(CreatureTextId)) < 0;
    }

    uint32 entry;
    uint32 textGroup;
    uint32 textId;
};

typedef std::vector<CreatureTextEntry> CreatureTextGroup;              //texts in a group
typedef std::unordered_map<uint8, CreatureTextGroup> CreatureTextHolder;    //groups for a creature by groupid
typedef std::unordered_map<uint32, CreatureTextHolder> CreatureTextMap;     //all creatures by entry

typedef std::map<CreatureTextId, CreatureTextLocale> LocaleCreatureTextMap;

//used for handling non-repeatable random texts
typedef std::vector<uint8> CreatureTextRepeatIds;
typedef std::unordered_map<uint8, CreatureTextRepeatIds> CreatureTextRepeatGroup;
typedef std::unordered_map<uint64, CreatureTextRepeatGroup> CreatureTextRepeatMap;//guid based

class CreatureTextMgr
{
    friend class ACE_Singleton<CreatureTextMgr, ACE_Null_Mutex>;
    CreatureTextMgr() {};

    public:
        ~CreatureTextMgr() {};
        void LoadCreatureTexts();
        void LoadCreatureTextLocales();
        CreatureTextMap  const& GetTextMap() const { return mTextMap; }

        void SendSound(Creature* source, uint32 sound, ChatMsg msgType, uint64 whisperGuid, TextRange range, Team team, bool gmOnly, bool p_Personal = false);
        void SendEmote(Unit* p_Source, uint32 p_EmoteId, uint64 p_ReceiverGuid = 0);

        //if sent, returns the 'duration' of the text else 0 if error
        uint32 SendChat(Creature* source, uint8 textGroup, uint64 whisperGuid = 0, ChatMsg msgType = CHAT_MSG_ADDON, Language language = LANG_ADDON, TextRange range = TEXT_RANGE_NORMAL, uint32 sound = 0, Team team = TEAM_OTHER, bool gmOnly = false, Player* srcPlr = NULL, bool p_Personal = false);
        bool TextExist(uint32 sourceEntry, uint8 textGroup);
        std::string GetLocalizedChatString(uint32 entry, uint8 gender, uint8 textGroup, uint32 id, LocaleConstant locale) const;

        template<class Builder>
        void SendChatPacket(WorldObject* source, Builder const& builder, ChatMsg msgType, uint64 whisperGuid = 0, TextRange range = TEXT_RANGE_NORMAL, Team team = TEAM_OTHER, bool gmOnly = false, bool p_Personal = false) const;
    private:
        CreatureTextRepeatIds GetRepeatGroup(Creature* source, uint8 textGroup);
        void SetRepeatId(Creature* source, uint8 textGroup, uint8 id);

        void SendNonChatPacket(WorldObject* source, WorldPacket* data, ChatMsg msgType, uint64 whisperGuid, TextRange range, Team team, bool gmOnly) const;
        float GetRangeForChatType(ChatMsg msgType) const;

        CreatureTextMap mTextMap;
        CreatureTextRepeatMap mTextRepeatMap;
        LocaleCreatureTextMap mLocaleTextMap;
        std::mutex m_TextRepeatLock;
};

#define sCreatureTextMgr ACE_Singleton<CreatureTextMgr, ACE_Null_Mutex>::instance()

template<class Builder>
class CreatureTextLocalizer
{
    public:
        CreatureTextLocalizer(Builder const& builder, ChatMsg msgType) : _builder(builder), _msgType(msgType)
        {
            _packetCache.resize(TOTAL_LOCALES, NULL);
        }

        ~CreatureTextLocalizer()
        {
            for (size_t i = 0; i < _packetCache.size(); ++i)
            {
                delete _packetCache[i];
            }
        }

        void operator()(Player* player)
        {
            LocaleConstant loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
            WorldPacket* messageTemplate;

            uint64 tguid = 0LL;
            switch (_msgType)
            {
                case CHAT_MSG_MONSTER_WHISPER:
                case CHAT_MSG_RAID_BOSS_WHISPER:
                    tguid = player->GetGUID();
                    break;
                default:
                    break;
            }
            // create if not cached yet
            if (!_packetCache[loc_idx])
            {
                messageTemplate = new WorldPacket(SMSG_CHAT, 200);
                _builder(messageTemplate, loc_idx, tguid);
                _packetCache[loc_idx] = messageTemplate;
            }
            else
                messageTemplate = _packetCache[loc_idx];

            WorldPacket data(*messageTemplate);

            player->SendDirectMessage(&data);
        }

    private:
        std::vector<WorldPacket*> _packetCache;
        Builder const& _builder;
        ChatMsg _msgType;
};

template<class Builder>
void CreatureTextMgr::SendChatPacket(WorldObject* source, Builder const& builder, ChatMsg msgType, uint64 whisperGuid, TextRange range, Team team, bool gmOnly, bool p_Personal) const
{
    if (!source)
        return;

    CreatureTextLocalizer<Builder> localizer(builder, msgType);

    switch (msgType)
    {
        case CHAT_MSG_MONSTER_WHISPER:
        case CHAT_MSG_RAID_BOSS_WHISPER:
        {
            if (range == TEXT_RANGE_NORMAL) //ignores team and gmOnly
            {
                Player* player = ObjectAccessor::FindPlayer(whisperGuid);
                if (!player || !player->GetSession())
                    return;

                localizer(player);
                return;
            }
            break;
        }
        default:
        {
            if (source->MustBeVisibleOnlyForSomePlayers())
            {
                std::list<uint64> personalVisionList;
                source->GetMustBeVisibleForPlayersList(personalVisionList);
                if (!personalVisionList.empty())
                {
                    for (auto& guid : personalVisionList)
                    {
                        if (auto player = ObjectAccessor::GetPlayer(*source, guid))
                            localizer(player);
                    }
                    return;
                }
            }
            break;
        }
    }

    if (p_Personal)
    {
        Player* l_Player = ObjectAccessor::FindPlayer(whisperGuid);
        if (!l_Player || !l_Player->GetSession())
            return;

        localizer(l_Player);
        return;
    }

    switch (range)
    {
        case TEXT_RANGE_AREA:
        {
            uint32 areaId = source->GetAreaId();
            Map::PlayerList const& players = source->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if (itr->getSource()->GetAreaId() == areaId && itr->getSource()->InSamePhase(source) && (!team || Team(itr->getSource()->GetTeam()) == team) && (!gmOnly || itr->getSource()->isGameMaster()))
                    localizer(itr->getSource());
            return;
        }
        case TEXT_RANGE_ZONE:
        {
            uint32 zoneId = source->GetZoneId();
            Map::PlayerList const& players = source->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if (itr->getSource()->GetZoneId() == zoneId && (!team || Team(itr->getSource()->GetTeam()) == team) && (!gmOnly || itr->getSource()->isGameMaster()))
                    localizer(itr->getSource());
            return;
        }
        case TEXT_RANGE_MAP:
        {
            Map::PlayerList const& players = source->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if ((!team || Team(itr->getSource()->GetTeam()) == team) && (!gmOnly || itr->getSource()->isGameMaster()))
                    localizer(itr->getSource());
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
            {
                if (Player* player = iter->second)
#endif /* CROSS */
                    if (player->GetSession()  && (!team || Team(player->GetTeam()) == team) && (!gmOnly || player->isGameMaster()))
                        localizer(player);
#ifdef CROSS
            }
#endif /* CROSS */
            sWorld->GetSessionsLock().release();
            return;
        }
        case TEXT_RANGE_NORMAL:
        default:
            break;
    }

    float dist = GetRangeForChatType(msgType);
    JadeCore::PlayerChatDistWorker<CreatureTextLocalizer<Builder> > worker(source, dist, localizer);
    source->VisitNearbyWorldObject(dist, worker);
}

class CreatureTextBuilder
{
    public:
        CreatureTextBuilder(WorldObject* obj, uint8 gender, ChatMsg msgtype, uint8 textGroup, uint32 id, uint32 language, uint64 targetGUID, uint32 broadcastId = 0, bool forceGender = false)
            : _source(obj), _gender(gender), _msgType(msgtype), _textGroup(textGroup), _textId(id), _language(language), _targetGUID(targetGUID), _broadcastId(broadcastId), _forceGender(forceGender)
        {
        }

        void operator()(WorldPacket* data, LocaleConstant locale, uint64 tguid) const
        {
            auto GetText = [](BroadcastTextEntry const* broadcastText, LocaleConstant locale, uint8 gender, bool forceGender)
            {
                if (gender == GENDER_FEMALE && (forceGender || broadcastText->FemaleText->Str[DEFAULT_LOCALE][0] != '\0'))
                {
                    if (broadcastText->FemaleText->Str[locale][0] != '\0')
                        return broadcastText->FemaleText->Str[locale];

                    return broadcastText->FemaleText->Str[DEFAULT_LOCALE];
                }

                if (broadcastText->MaleText->Str[locale][0] != '\0')
                    return broadcastText->MaleText->Str[locale];

                return broadcastText->MaleText->Str[DEFAULT_LOCALE];
            };

            if (auto bct = sBroadcastTextStore.LookupEntry(_broadcastId))
            {
                std::string text = GetText(bct, locale, _source->ToUnit() ? _source->ToUnit()->getGender() : 0, false);
                char const* localizedName = _source->GetNameForLocaleIdx(locale);
                _source->BuildMonsterChat(data, _msgType, text.c_str(), _language, localizedName, tguid > 0 ? tguid : _targetGUID);
            }
            else
            {
                std::string text = sCreatureTextMgr->GetLocalizedChatString(_source->GetEntry(), _gender, _textGroup, _textId, locale);
                char const* localizedName = _source->GetNameForLocaleIdx(locale);
                _source->BuildMonsterChat(data, _msgType, text.c_str(), _language, localizedName, tguid > 0 ? tguid : _targetGUID);
            }
        }

        WorldObject* _source;
        ChatMsg _msgType;
        uint8 _textGroup;
        uint32 _textId;
        uint32 _language;
        uint64 _targetGUID;
        uint8 _gender;
        uint32 _broadcastId;
        bool _forceGender;
};

class PlayerTextBuilder
{
    public:
        PlayerTextBuilder(Player* p_Source, ChatMsg p_MsgType, uint32 p_BroadcastTextID, uint32 p_Language, uint64 p_TargetGUID = 0)
            : m_Source(p_Source), m_Gender(p_Source->getGender()), m_BroadcastTextID(p_BroadcastTextID), m_MsgType(p_MsgType), m_Language(p_Language), m_TargetGUID(p_TargetGUID) { }

        void operator()(WorldPacket* p_Data, LocaleConstant p_Locale, uint64 p_TargetGuid) const
        {
            BroadcastTextEntry const* l_BroadcastText = sBroadcastTextStore.LookupEntry(m_BroadcastTextID);
            if (!l_BroadcastText)
                return;

            std::string l_Text = GetBroadcastTextValue(l_BroadcastText, p_Locale, m_Gender);

            char const* localizedName = m_Source->GetNameForLocaleIdx(p_Locale);

            m_Source->BuildMonsterChat(p_Data, m_MsgType, l_Text.c_str(), m_Language, localizedName, p_TargetGuid > 0 ? p_TargetGuid : m_TargetGUID);
        }

        WorldObject* m_Source;
        ChatMsg m_MsgType;
        uint32 m_BroadcastTextID;
        uint32 m_Language;
        uint64 m_TargetGUID;
        uint8 m_Gender;
};

#endif
