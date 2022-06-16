////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "GroupMgr.h"
#include "SpellMgr.h"
#include "UpdateMask.h"
#include "World.h"
#include "Arena.h"
#include "Transport.h"
#include "Language.h"
#include "GameEventMgr.h"
#include "Spell.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "InstanceSaveMgr.h"
#include "SpellAuras.h"
#include "Util.h"
#include "WaypointManager.h"
#include "GossipDef.h"
#include "Vehicle.h"
#include "AchievementMgr.h"
#include "DisableMgr.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "PoolMgr.h"
#include "DB2Structure.h"
#include "DB2Stores.h"
#include "Configuration/Config.h"
#include "VMapFactory.h"
#include "GameTables.h"
#include "ItemPrototype.h"
#include "WordFilterMgr.h"
#include "POIMgr.hpp"
#include "PathGenerator.h"
#include <locale.h>
#include "GarbageCollector.h"
#include "LegionCombatAI.hpp"
#ifndef CROSS
# include "GarrisonMgr.hpp"
#endif

ScriptMapMap sQuestEndScripts;
ScriptMapMap sQuestStartScripts;
ScriptMapMap sSpellScripts;
ScriptMapMap sGameObjectScripts;
ScriptMapMap sEventScripts;
ScriptMapMap sWaypointScripts;
VisibleDistanceMap sVisibleDistance[TYPE_VISIBLE_MAX];

std::string GetScriptsTableNameByType(ScriptsType type)
{
    std::string res = "";
    switch (type)
    {
        case SCRIPTS_QUEST_END:     res = "quest_end_scripts";  break;
        case SCRIPTS_QUEST_START:   res = "quest_start_scripts";break;
        case SCRIPTS_SPELL:         res = "spell_scripts";      break;
        case SCRIPTS_GAMEOBJECT:    res = "gameobject_scripts"; break;
        case SCRIPTS_EVENT:         res = "event_scripts";      break;
        case SCRIPTS_WAYPOINT:      res = "waypoint_scripts";   break;
        default: break;
    }
    return res;
}

ScriptMapMap* GetScriptsMapByType(ScriptsType type)
{
    ScriptMapMap* res = NULL;
    switch (type)
    {
        case SCRIPTS_QUEST_END:     res = &sQuestEndScripts;    break;
        case SCRIPTS_QUEST_START:   res = &sQuestStartScripts;  break;
        case SCRIPTS_SPELL:         res = &sSpellScripts;       break;
        case SCRIPTS_GAMEOBJECT:    res = &sGameObjectScripts;  break;
        case SCRIPTS_EVENT:         res = &sEventScripts;       break;
        case SCRIPTS_WAYPOINT:      res = &sWaypointScripts;    break;
        default: break;
    }
    return res;
}

std::string GetScriptCommandName(ScriptCommands command)
{
    std::string res = "";
    switch (command)
    {
        case SCRIPT_COMMAND_TALK: res = "SCRIPT_COMMAND_TALK"; break;
        case SCRIPT_COMMAND_EMOTE: res = "SCRIPT_COMMAND_EMOTE"; break;
        case SCRIPT_COMMAND_FIELD_SET: res = "SCRIPT_COMMAND_FIELD_SET"; break;
        case SCRIPT_COMMAND_MOVE_TO: res = "SCRIPT_COMMAND_MOVE_TO"; break;
        case SCRIPT_COMMAND_FLAG_SET: res = "SCRIPT_COMMAND_FLAG_SET"; break;
        case SCRIPT_COMMAND_FLAG_REMOVE: res = "SCRIPT_COMMAND_FLAG_REMOVE"; break;
        case SCRIPT_COMMAND_TELEPORT_TO: res = "SCRIPT_COMMAND_TELEPORT_TO"; break;
        case SCRIPT_COMMAND_QUEST_EXPLORED: res = "SCRIPT_COMMAND_QUEST_EXPLORED"; break;
        case SCRIPT_COMMAND_KILL_CREDIT: res = "SCRIPT_COMMAND_KILL_CREDIT"; break;
        case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT: res = "SCRIPT_COMMAND_RESPAWN_GAMEOBJECT"; break;
        case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE: res = "SCRIPT_COMMAND_TEMP_SUMMON_CREATURE"; break;
        case SCRIPT_COMMAND_OPEN_DOOR: res = "SCRIPT_COMMAND_OPEN_DOOR"; break;
        case SCRIPT_COMMAND_CLOSE_DOOR: res = "SCRIPT_COMMAND_CLOSE_DOOR"; break;
        case SCRIPT_COMMAND_ACTIVATE_OBJECT: res = "SCRIPT_COMMAND_ACTIVATE_OBJECT"; break;
        case SCRIPT_COMMAND_REMOVE_AURA: res = "SCRIPT_COMMAND_REMOVE_AURA"; break;
        case SCRIPT_COMMAND_CAST_SPELL: res = "SCRIPT_COMMAND_CAST_SPELL"; break;
        case SCRIPT_COMMAND_PLAY_SOUND: res = "SCRIPT_COMMAND_PLAY_SOUND"; break;
        case SCRIPT_COMMAND_CREATE_ITEM: res = "SCRIPT_COMMAND_CREATE_ITEM"; break;
        case SCRIPT_COMMAND_DESPAWN_SELF: res = "SCRIPT_COMMAND_DESPAWN_SELF"; break;
        case SCRIPT_COMMAND_LOAD_PATH: res = "SCRIPT_COMMAND_LOAD_PATH"; break;
        case SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT: res = "SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT"; break;
        case SCRIPT_COMMAND_KILL: res = "SCRIPT_COMMAND_KILL"; break;
        case SCRIPT_COMMAND_ORIENTATION: res = "SCRIPT_COMMAND_ORIENTATION"; break;
        case SCRIPT_COMMAND_EQUIP: res = "SCRIPT_COMMAND_EQUIP"; break;
        case SCRIPT_COMMAND_MODEL: res = "SCRIPT_COMMAND_MODEL"; break;
        case SCRIPT_COMMAND_CLOSE_GOSSIP: res = "SCRIPT_COMMAND_CLOSE_GOSSIP"; break;
        case SCRIPT_COMMAND_PLAYMOVIE: res = "SCRIPT_COMMAND_PLAYMOVIE"; break;
        case SCRIPT_COMMAND_SET_ANIMKIT: res = "SCRIPT_COMMAND_SET_ANIMKIT"; break;
        default:
        {
            char sz[32];
            sprintf(sz, "Unknown command: %d", command);
            res = sz;
            break;
        }
    }
    return res;
}

std::string ScriptInfo::GetDebugInfo() const
{
    char sz[256];
    sprintf(sz, "%s ('%s' script id: %u)", GetScriptCommandName(command).c_str(), GetScriptsTableNameByType(type).c_str(), id);
    return std::string(sz);
}

bool normalizePlayerName(std::string& p_Name)
{
    if (p_Name.empty())
        return false;

    if (p_Name[0] == -61 && p_Name[1] == -97) // Interdiction d'utiliser ce caractere au debut, il fait planter l'affichage cote client
        return false;

    wchar_t wstr_buf[MAX_INTERNAL_PLAYER_NAME+1];
    size_t wstr_len = MAX_INTERNAL_PLAYER_NAME;

    if (!Utf8toWStr(p_Name, &wstr_buf[0], wstr_len))
        return false;

    wstr_buf[0] = wcharToUpper(wstr_buf[0]);
    for (size_t i = 1; i < wstr_len; ++i)
        wstr_buf[i] = wcharToLower(wstr_buf[i]);

    if (!WStrToUtf8(wstr_buf, wstr_len, p_Name))
        return false;

    std::string l_NewName = "";
    for (uint32 l_I = 0; l_I < p_Name.length(); l_I++)
    {
        if (p_Name[l_I] == '-')
            break;

        l_NewName += p_Name[l_I];
    }

    p_Name = l_NewName;
    return true;
}

LanguageDesc lang_description[LANGUAGES_COUNT] =
{
    { LANG_ADDON,             0,  0                            },
    { LANG_UNIVERSAL,         0,  0                            },
    { LANG_ORCISH,          669,  SKILL_LANG_ORCISH            },
    { LANG_DARNASSIAN,      671,  SKILL_LANG_DARNASSIAN        },
    { LANG_TAURAHE,         670,  SKILL_LANG_TAURAHE           },
    { LANG_DWARVISH,        672,  SKILL_LANG_DWARVEN           },
    { LANG_COMMON,          668,  SKILL_LANG_COMMON            },
    { LANG_DEMONIC,         815,  SKILL_LANG_DEMON_TONGUE      },
    { LANG_TITAN,           816,  SKILL_LANG_TITAN             },
    { LANG_THALASSIAN,      813,  SKILL_LANG_THALASSIAN        },
    { LANG_DRACONIC,        814,  SKILL_LANG_DRACONIC          },
    { LANG_KALIMAG,         817,  SKILL_LANG_OLD_TONGUE        },
    { LANG_GNOMISH,        7340,  SKILL_LANG_GNOMISH           },
    { LANG_TROLL,          7341,  SKILL_LANG_TROLL             },
    { LANG_GUTTERSPEAK,   17737,  SKILL_LANG_FORSAKEN          },
    { LANG_DRAENEI,       29932,  SKILL_LANG_DRAENEI           },
    { LANG_ZOMBIE,            0,  0                            },
    { LANG_GNOMISH_BINARY,    0,  0                            },
    { LANG_GOBLIN_BINARY,     0,  0                            },
    { LANG_GILNEAN,       69270,  SKILL_LANG_GILNEAN           },
    { LANG_GOBLIN,        69269,  SKILL_LANG_GOBLIN            },
    { LANG_PANDAREN_N,   108127,  SKILL_LANG_PANDAREN_NEUTRAL  },
    { LANG_PANDAREN_A,   108130,  SKILL_LANG_PANDAREN_ALLIANCE },
    { LANG_PANDAREN_H,   108131,  SKILL_LANG_PANDAREN_HORDE    },
    { LANG_SHATHYAR,          0,  0                            },
    { LANG_NERGLISH,          0,  0                            },
    { LANG_MOONKIN,           0,  0                            },
    { LANG_SHALASSIAN,   262439,  SKILL_LANGUAGE_SHALASSIAN    },
    { LANG_THALASSIAN_2, 262454,  SKILL_LANG_THALASSIAN        },
};

LanguageDesc const* GetLanguageDescByID(uint32 lang)
{
    for (uint8 i = 0; i < LANGUAGES_COUNT; ++i)
    {
        if (uint32(lang_description[i].lang_id) == lang)
            return &lang_description[i];
    }

    return nullptr;
}

bool SpellClickInfo::IsFitToRequirements(Unit const* clicker, Unit const* clickee) const
{
    Player const* playerClicker = clicker->ToPlayer();
    if (!playerClicker)
        return true;

    Unit const* summoner = nullptr;
    // Check summoners for party
    if (clickee->isSummon())
        summoner = clickee->ToTempSummon()->GetSummoner();
    if (!summoner)
        summoner = clickee;

    // This only applies to players
    switch (userType)
    {
        case SPELL_CLICK_USER_FRIEND:
            if (!playerClicker->IsFriendlyTo(summoner))
                return false;
            break;
        case SPELL_CLICK_USER_RAID:
            if (!playerClicker->IsInRaidWith(summoner))
                return false;
            break;
        case SPELL_CLICK_USER_PARTY:
            if (!playerClicker->IsInPartyWith(summoner))
                return false;
            break;
        default:
            break;
    }

    return true;
}

float GetVisibleDistance(uint32 type, uint32 id)
{
    VisibleDistanceMap::const_iterator itr = sVisibleDistance[type].find(id);
    if (itr != sVisibleDistance[type].end())
        return itr->second;

    return 0.0f;
}

struct ItemSpecStats
{
    uint32 ItemType;
    std::array<uint32, MAX_ITEM_PROTO_STATS> ItemSpecStatTypes;
    uint32 ItemSpecStatCount;

    ItemSpecStats(ItemEntry const* item, ItemSparseEntry const* sparse) : ItemType(0), ItemSpecStatCount(0)
    {
        ItemSpecStatTypes.fill(-1);

        if (item == nullptr || sparse == nullptr)
            return;

        if (item->ClassId == ITEM_CLASS_WEAPON)
        {
            ItemType = 5;
            switch (item->SubclassId)
            {
            case ITEM_SUBCLASS_WEAPON_AXE:
                AddStat(ITEM_SPEC_STAT_ONE_HANDED_AXE);
                break;
            case ITEM_SUBCLASS_WEAPON_AXE2:
                AddStat(ITEM_SPEC_STAT_TWO_HANDED_AXE);
                break;
            case ITEM_SUBCLASS_WEAPON_BOW:
                AddStat(ITEM_SPEC_STAT_BOW);
                break;
            case ITEM_SUBCLASS_WEAPON_GUN:
                AddStat(ITEM_SPEC_STAT_GUN);
                break;
            case ITEM_SUBCLASS_WEAPON_MACE:
                AddStat(ITEM_SPEC_STAT_ONE_HANDED_MACE);
                break;
            case ITEM_SUBCLASS_WEAPON_MACE2:
                AddStat(ITEM_SPEC_STAT_TWO_HANDED_MACE);
                break;
            case ITEM_SUBCLASS_WEAPON_POLEARM:
                AddStat(ITEM_SPEC_STAT_POLEARM);
                break;
            case ITEM_SUBCLASS_WEAPON_SWORD:
                AddStat(ITEM_SPEC_STAT_ONE_HANDED_SWORD);
                break;
            case ITEM_SUBCLASS_WEAPON_SWORD2:
                AddStat(ITEM_SPEC_STAT_TWO_HANDED_SWORD);
                break;
            case ITEM_SUBCLASS_WEAPON_WARGLAIVES:
                AddStat(ITEM_SPEC_STAT_WARGLAIVES);
                break;
            case ITEM_SUBCLASS_WEAPON_STAFF:
                AddStat(ITEM_SPEC_STAT_STAFF);
                break;
            case ITEM_SUBCLASS_WEAPON_FIST_WEAPON:
                AddStat(ITEM_SPEC_STAT_FIST_WEAPON);
                break;
            case ITEM_SUBCLASS_WEAPON_DAGGER:
                AddStat(ITEM_SPEC_STAT_DAGGER);
                break;
            case ITEM_SUBCLASS_WEAPON_THROWN:
                AddStat(ITEM_SPEC_STAT_THROWN);
                break;
            case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                AddStat(ITEM_SPEC_STAT_CROSSBOW);
                break;
            case ITEM_SUBCLASS_WEAPON_WAND:
                AddStat(ITEM_SPEC_STAT_WAND);
                break;
            default:
                break;
            }
        }
        else if (item->ClassId == ITEM_CLASS_ARMOR && item->SubclassId != 5 && item->SubclassId <= 11 && sparse->InventoryType != INVTYPE_TRINKET)
        {
            switch (item->SubclassId)
            {
                case ITEM_SUBCLASS_ARMOR_CLOTH:
                    if (sparse->InventoryType != INVTYPE_CLOAK)
                    {
                        ItemType = 1;
                        break;
                    }

                    ItemType = 0;
                    AddStat(ITEM_SPEC_STAT_CLOAK);
                    break;
                case ITEM_SUBCLASS_ARMOR_LEATHER:
                    ItemType = 2;
                    break;
                case ITEM_SUBCLASS_ARMOR_MAIL:
                    ItemType = 3;
                    break;
                case ITEM_SUBCLASS_ARMOR_PLATE:
                    ItemType = 4;
                    break;
                default:
                    ItemType = 6;
                    if (item->SubclassId == ITEM_SUBCLASS_ARMOR_SHIELD)
                        AddStat(ITEM_SPEC_STAT_SHIELD);
                    else if (item->SubclassId > ITEM_SUBCLASS_ARMOR_SHIELD && item->SubclassId <= ITEM_SUBCLASS_ARMOR_RELIC)
                        AddStat(ITEM_SPEC_STAT_RELIC);
                    break;
            }
        }
        else if (item->ClassId == ITEM_CLASS_GEM)
        {
            if (GemPropertiesEntry const* gem = sGemPropertiesStore.LookupEntry(sparse->GemProperties))
            {
                if (gem->Type & SOCKET_COLOR_RELIC_IRON)
                    AddStat(ITEM_SPEC_STAT_RELIC_IRON);
                if (gem->Type & SOCKET_COLOR_RELIC_BLOOD)
                    AddStat(ITEM_SPEC_STAT_RELIC_BLOOD);
                if (gem->Type & SOCKET_COLOR_RELIC_SHADOW)
                    AddStat(ITEM_SPEC_STAT_RELIC_SHADOW);
                if (gem->Type & SOCKET_COLOR_RELIC_FEL)
                    AddStat(ITEM_SPEC_STAT_RELIC_FEL);
                if (gem->Type & SOCKET_COLOR_RELIC_ARCANE)
                    AddStat(ITEM_SPEC_STAT_RELIC_ARCANE);
                if (gem->Type & SOCKET_COLOR_RELIC_FROST)
                    AddStat(ITEM_SPEC_STAT_RELIC_FROST);
                if (gem->Type & SOCKET_COLOR_RELIC_FIRE)
                    AddStat(ITEM_SPEC_STAT_RELIC_FIRE);
                if (gem->Type & SOCKET_COLOR_RELIC_WATER)
                    AddStat(ITEM_SPEC_STAT_RELIC_WATER);
                if (gem->Type & SOCKET_COLOR_RELIC_LIFE)
                    AddStat(ITEM_SPEC_STAT_RELIC_LIFE);
                if (gem->Type & SOCKET_COLOR_RELIC_WIND)
                    AddStat(ITEM_SPEC_STAT_RELIC_WIND);
                if (gem->Type & SOCKET_COLOR_RELIC_HOLY)
                    AddStat(ITEM_SPEC_STAT_RELIC_HOLY);

                ItemType = 7;
            }
         }
        else
            ItemType = 0;

        for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
            if (sparse->ItemStatType[i] != -1)
                AddModStat(sparse->ItemStatType[i]);
    }

    void AddStat(ItemSpecStat statType)
    {
        if (ItemSpecStatCount >= MAX_ITEM_PROTO_STATS)
            return;

        for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
            if (ItemSpecStatTypes[i] == uint32(statType))
                return;

        ItemSpecStatTypes[ItemSpecStatCount++] = statType;
    }

    void AddModStat(int32 itemStatType)
    {
        switch (itemStatType)
        {
            case int32(ItemModType::AGILITY):
                AddStat(ITEM_SPEC_STAT_AGILITY);
                break;
            case int32(ItemModType::STRENGTH):
                AddStat(ITEM_SPEC_STAT_STRENGTH);
                break;
            case int32(ItemModType::INTELLECT):
                AddStat(ITEM_SPEC_STAT_INTELLECT);
                break;
            case int32(ItemModType::DODGE_RATING):
                AddStat(ITEM_SPEC_STAT_DODGE);
                break;
            case int32(ItemModType::PARRY_RATING):
                AddStat(ITEM_SPEC_STAT_PARRY);
                break;
            case int32(ItemModType::CRIT_RATING):
                AddStat(ITEM_SPEC_STAT_CRIT);
                break;
            case int32(ItemModType::HASTE_RATING):
                AddStat(ITEM_SPEC_STAT_HASTE);
                break;
            case int32(ItemModType::HIT_RATING):
                AddStat(ITEM_SPEC_STAT_HIT);
                break;
            case int32(ItemModType::EXTRA_ARMOR):
                AddStat(ITEM_SPEC_STAT_BONUS_ARMOR);
                break;
            case int32(ItemModType::AGILITY_OR_STRENGTH_OR_INTELLECT):
                AddStat(ITEM_SPEC_STAT_AGILITY);
                AddStat(ITEM_SPEC_STAT_STRENGTH);
                AddStat(ITEM_SPEC_STAT_INTELLECT);
                break;
            case int32(ItemModType::AGILITY_OR_STRENGTH):
                AddStat(ITEM_SPEC_STAT_AGILITY);
                AddStat(ITEM_SPEC_STAT_STRENGTH);
                break;
            case int32(ItemModType::AGILITY_OR_INTELLECT):
                AddStat(ITEM_SPEC_STAT_AGILITY);
                AddStat(ITEM_SPEC_STAT_INTELLECT);
                break;
            case int32(ItemModType::STRENGTH_OR_INTELLECT):
                AddStat(ITEM_SPEC_STAT_STRENGTH);
                AddStat(ITEM_SPEC_STAT_INTELLECT);
                break;
        }
    }
};

#ifndef CROSS
ObjectMgr::ObjectMgr(): _auctionId(1),
    _itemTextId(1), _voidItemId(1), _hiCharGuid(1),
#else /* CROSS */
ObjectMgr::ObjectMgr(): _auctionId(1), _equipmentSetGuid(1),
    _itemTextId(1), _mailId(1), _hiPetNumber(1), _voidItemId(1), _hiCharGuid(1),
#endif /* CROSS */
    _hiCreatureGuid(1), _hiPetGuid(1), _hiVehicleGuid(1),
    _hiGoGuid(1), _hiDoGuid(1), _hiCorpseGuid(1), _hiAreaTriggerGuid(1), _hiMoTransGuid(1), m_HiVignetteGuid(1),
    m_HighConversationGuid(1), m_HighEventObjectGuid(1), m_EncounterAttemptID(1), m_WPDebug(false)
{
#ifndef CROSS
    m_HighItemGuid     = 1;
    m_MailId           = 1;
    m_PetNumber        = 1;
    m_EquipmentSetGuid = 1;
    m_ScenarioID       = 1;
#else /* CROSS */
    m_HighItemGuid = 1;
#endif /* CROSS */
}

ObjectMgr::~ObjectMgr()
{
    for (QuestMap::iterator i = _questTemplates.begin(); i != _questTemplates.end(); ++i)
        delete i->second;

    for (int race = 0; race < MAX_RACES; ++race)
        for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
            delete[] _playerInfo[race][class_].levelInfo;

    for (CacheVendorItemContainer::iterator itr = _cacheVendorItemStore.begin(); itr != _cacheVendorItemStore.end(); ++itr)
        itr->second.Clear();

    _cacheTrainerSpellStore.clear();

    for (DungeonEncounterContainer::iterator itr =_dungeonEncounterStore.begin(); itr != _dungeonEncounterStore.end(); ++itr)
        for (DungeonEncounterList::iterator encounterItr = itr->second.begin(); encounterItr != itr->second.end(); ++encounterItr)
            delete *encounterItr;

    for (CharacterTemplates::iterator l_Iter = m_CharacterTemplatesStore.begin(); l_Iter != m_CharacterTemplatesStore.end(); l_Iter++)
        delete l_Iter->second;
}

void ObjectMgr::AddLocaleString(std::string const& s, LocaleConstant locale, StringVector& data)
{
    if (!s.empty())
    {
        if (data.size() <= size_t(locale))
            data.resize(locale + 1);

        data[locale] = s;
    }
}

void ObjectMgr::LoadCreatureWDBLocales()
{
    uint32 oldMSTime = getMSTime();

    _creatureLocaleStore.clear();                              // need for reload case

    //                                               0   1       2      3         4      5      6      7      8         9         10        11
    QueryResult result = WorldDatabase.Query("SELECT ID, Locale, Title, TitleAlt, Name1, Name2, Name3, Name4, NameAlt1, NameAlt2, NameAlt3, NameAlt4 FROM creature_template_wdb_locale");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        LocaleConstant locale = GetLocaleByName(fields[1].GetString());
        if (locale == TOTAL_LOCALES)
            continue;

        CreatureLocale& data = _creatureLocaleStore[fields[0].GetUInt32()];
        AddLocaleString(fields[2].GetString(), locale, data.Title);
        AddLocaleString(fields[3].GetString(), locale, data.TitleAlt);

        for (uint8 i = 0; i < MAX_CREATURE_NAMES; ++i)
            AddLocaleString(fields[4 + i].GetString(), locale, data.Name[i]);

        for (uint8 i = 0; i < MAX_CREATURE_NAMES; ++i)
            AddLocaleString(fields[8 + i].GetString(), locale, data.NameAlt[i]);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu creature locale strings in %u ms", (unsigned long)_creatureLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGossipMenuItemsLocales()
{
    uint32 oldMSTime = getMSTime();

    _gossipMenuItemsLocaleStore.clear();                              // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT menu_id, id, "
        "option_text_loc1, box_text_loc1, option_text_loc2, box_text_loc2, "
        "option_text_loc3, box_text_loc3, option_text_loc4, box_text_loc4, "
        "option_text_loc5, box_text_loc5, option_text_loc6, box_text_loc6, "
        "option_text_loc7, box_text_loc7, option_text_loc8, box_text_loc8, "
        "option_text_loc9, box_text_loc9, option_text_loc10, box_text_loc10 "
        "FROM locales_gossip_menu_option");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 menuId   = fields[0].GetUInt32();
        uint16 id       = fields[1].GetUInt16();

        GossipMenuItemsLocale& data = _gossipMenuItemsLocaleStore[MAKE_PAIR64(menuId, id)];

        for (uint8 i = 1; i < TOTAL_LOCALES; ++i)
        {
            LocaleConstant locale = (LocaleConstant) i;
            AddLocaleString(fields[2 + 2 * (i - 1)].GetString(), locale, data.OptionText);
            AddLocaleString(fields[2 + 2 * (i - 1) + 1].GetString(), locale, data.BoxText);
        }
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu gossip_menu_option locale strings in %u ms", (unsigned long)_gossipMenuItemsLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPointOfInterestLocales()
{
    uint32 oldMSTime = getMSTime();

    _pointOfInterestLocaleStore.clear();                              // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, icon_name_loc1, icon_name_loc2, icon_name_loc3, icon_name_loc4, icon_name_loc5, icon_name_loc6, icon_name_loc7, icon_name_loc8, icon_name_loc9, icon_name_loc10 FROM locales_points_of_interest");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        PointOfInterestLocale& data = _pointOfInterestLocaleStore[entry];

        for (uint8 i = 1; i < TOTAL_LOCALES; ++i)
            AddLocaleString(fields[i].GetString(), LocaleConstant(i), data.IconName);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu points_of_interest locale strings in %u ms", (unsigned long)_pointOfInterestLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadWDBCreatureTemplates()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query(
    //        0      1      2      3      4       5         6        7         8         9      10        11         12      13         14          15
    "SELECT Entry, Name1, Name2, Name3, Name4, NameAlt1, NameAlt2, NameAlt3, NameAlt4, Title, TitleAlt, CursorName, Type, TypeFlags, TypeFlags2, RequiredExpansion, "
    //16           17              18          19        20         21        22           23           24        25          26          27          28
    "Family, Classification, MovementInfoID, HpMulti, PowerMulti, Leader, KillCredit1, KillCredit2, VignetteID, DisplayId1, DisplayId2, DisplayId3, DisplayId4, "
    //29           30           31          32          33          34          35          36        37        38          39
    "FlagQuest, QuestItem1, QuestItem2, QuestItem3, QuestItem4, QuestItem5, QuestItem6, QuestItem7, QuestItem8, QuestItem9, QuestItem10 FROM creature_template_wdb;");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature template definitions. DB table `creature_template_wdb` is empty.");
        return;
    }

    m_CreatureTemplateStore.rehash(l_Result->GetRowCount());

    do
    {
        uint8 l_Index = 0;
        Field* l_Fields = l_Result->Fetch();

        uint32 l_Entry = l_Fields[l_Index++].GetUInt32();
        CreatureTemplate& l_CreatureTemplate = m_CreatureTemplateStore[l_Entry];

        l_CreatureTemplate.Entry = l_Entry;

        for (uint8 l_I = 0; l_I < MAX_CREATURE_NAMES; ++l_I)
            l_CreatureTemplate.Name[l_I] = l_Fields[l_Index++].GetString();

        for (uint8 l_I = 0; l_I < MAX_CREATURE_NAMES; ++l_I)
            l_CreatureTemplate.NameAlt[l_I] = l_Fields[l_Index++].GetString();

        l_CreatureTemplate.Title             = l_Fields[l_Index++].GetString();
        l_CreatureTemplate.TitleAlt          = l_Fields[l_Index++].GetString();
        l_CreatureTemplate.CursorName        = l_Fields[l_Index++].GetString();
        l_CreatureTemplate.type              = uint32(l_Fields[l_Index++].GetUInt8());
        l_CreatureTemplate.type_flags        = l_Fields[l_Index++].GetUInt32();
        l_CreatureTemplate.type_flags2       = l_Fields[l_Index++].GetUInt32();
        l_CreatureTemplate.RequiredExpansion = uint32(l_Fields[l_Index++].GetInt16());
        l_CreatureTemplate.family            = CreatureFamily(l_Fields[l_Index++].GetUInt32());
        l_CreatureTemplate.rank              = uint32(l_Fields[l_Index++].GetUInt8());
        l_CreatureTemplate.movementId        = l_Fields[l_Index++].GetUInt32();
        l_CreatureTemplate.ModHealth         = l_Fields[l_Index++].GetFloat();
        l_CreatureTemplate.ModMana           = l_Fields[l_Index++].GetFloat();
        l_CreatureTemplate.RacialLeader      = l_Fields[l_Index++].GetBool();

        for (uint8 l_I = 0; l_I < MAX_KILL_CREDIT; ++l_I)
        {
            l_CreatureTemplate.KillCredit[l_I] = l_Fields[l_Index++].GetUInt32();
            if (l_CreatureTemplate.KillCredit[l_I] && (l_CreatureTemplate.expansion >= EXPANSION_WARLORDS_OF_DRAENOR || l_CreatureTemplate.RequiredExpansion == -1))
                l_CreatureTemplate.QuestPersonalLoot = true;
        }

        l_CreatureTemplate.VignetteID = l_Fields[l_Index++].GetUInt32();
        for (uint8 l_I = 0; l_I < MAX_CREATURE_MODELS; ++l_I)
            l_CreatureTemplate.Modelid[l_I] = l_Fields[l_Index++].GetUInt32();

        uint32 l_QuestFlags = l_Fields[l_Index++].GetUInt8();

        for (uint8 l_I = 0; l_I < MAX_CREATURE_QUEST_ITEMS; ++l_I)
        {
            l_CreatureTemplate.questItems[l_I] = l_Fields[l_Index++].GetUInt32();
            if (l_CreatureTemplate.questItems[l_I] && (l_CreatureTemplate.expansion >= EXPANSION_WARLORDS_OF_DRAENOR || l_CreatureTemplate.RequiredExpansion == -1))
                l_CreatureTemplate.QuestPersonalLoot = true;
        }

        /// If you don't have VignetteID you can assign TrackingQuestID in the field, mostly used for WorldBoss
        if (VignetteEntry const* vignette = sVignetteStore.LookupEntry(l_CreatureTemplate.VignetteID))
            l_CreatureTemplate.TrackingQuestID = vignette->QuestID;
        else
            l_CreatureTemplate.TrackingQuestID = l_CreatureTemplate.VignetteID;
    }
    while (l_Result->NextRow());

    for (CreatureTemplateContainer::iterator::value_type l_Iter : m_CreatureTemplateStore)
        CheckCreatureTemplateWDB(&l_Iter.second);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature WDB templates in %u ms", static_cast<uint32>(m_CreatureTemplateStore.size()), GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadCombatAIEvents()
{
    uint32 oldMSTime = getMSTime();

    m_CombatAIEvents.clear();                              // need for reload case

    //                                                 0      1          2          3           4           5            6         7            8            9            10
    QueryResult l_Result = WorldDatabase.Query("SELECT entry, start_min, start_max, repeat_min, repeat_max, repeat_fail, spell_id, event_check, event_flags, attack_dist, difficulty_mask FROM combat_ai_events");
    if (!l_Result)
        return;

    do
    {
        uint32 l_Idx = 0;
        Field* l_Fields = l_Result->Fetch();

        uint32 l_Entry          = l_Fields[l_Idx++].GetUInt32();

        CombatAIEventData l_EventData;

        l_EventData.StartMin        = l_Fields[l_Idx++].GetUInt32();
        l_EventData.StartMax        = l_Fields[l_Idx++].GetUInt32();
        l_EventData.RepeatMin       = l_Fields[l_Idx++].GetUInt32();
        l_EventData.RepeatMax       = l_Fields[l_Idx++].GetUInt32();
        l_EventData.RepeatFail      = l_Fields[l_Idx++].GetUInt32();
        l_EventData.SpellID         = l_Fields[l_Idx++].GetUInt32();
        l_EventData.EventCheck      = l_Fields[l_Idx++].GetUInt32();
        l_EventData.EventFlags      = l_Fields[l_Idx++].GetUInt32();
        l_EventData.AttackDist      = l_Fields[l_Idx++].GetFloat();
        l_EventData.DifficultyMask  = l_Fields[l_Idx++].GetUInt32();

        if (l_EventData.EventCheck >= eCombatAIEventsChecks::EventCheckMax)
        {
            sLog->outError(LOG_FILTER_SERVER_LOADING, "combat_ai_events for creature %u has unknown event_check index (%u), skipped", l_Entry, l_EventData.EventCheck);
            continue;
        }

        m_CombatAIEvents.insert(std::make_pair(l_Entry, l_EventData));

    }
    while (l_Result->NextRow());

    m_LastCombatAIUpdate = oldMSTime;

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu combat ai events in %u ms", (unsigned long)m_CombatAIEvents.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::CheckCreatureTemplateWDB(CreatureTemplate* cInfo)
{
    if (!cInfo)
        return;

    CreatureDisplayInfoEntry const* displayScaleEntry = nullptr;
    if (cInfo->Modelid[0])
    {
        CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(cInfo->Modelid[0]);
        if (!displayEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists non-existing DisplayId[0] id (%u), this can crash the client.", cInfo->Entry, cInfo->Modelid[0]);
            cInfo->Modelid[0] = 0;
        }
        else if (!displayScaleEntry)
            displayScaleEntry = displayEntry;

        CreatureModelInfo const* modelInfo = GetCreatureModelInfo(cInfo->Modelid[0]);
        if (!modelInfo)
            sLog->outError(LOG_FILTER_SQL, "No model data exist for `DisplayId[0]` = %u listed by creature (Entry: %u).", cInfo->Modelid[0], cInfo->Entry);
    }

    if (cInfo->Modelid[1])
    {
        CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(cInfo->Modelid[1]);
        if (!displayEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists non-existing DisplayId[1] id (%u), this can crash the client.", cInfo->Entry, cInfo->Modelid[1]);
            cInfo->Modelid[1] = 0;
        }
        else if (!displayScaleEntry)
            displayScaleEntry = displayEntry;

        CreatureModelInfo const* modelInfo = GetCreatureModelInfo(cInfo->Modelid[1]);
        if (!modelInfo)
            sLog->outError(LOG_FILTER_SQL, "No model data exist for `DisplayId[1]` = %u listed by creature (Entry: %u).", cInfo->Modelid[1], cInfo->Entry);
    }

    if (cInfo->Modelid[2])
    {
        CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(cInfo->Modelid[2]);
        if (!displayEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists non-existing DisplayId[2] id (%u), this can crash the client.", cInfo->Entry, cInfo->Modelid[2]);
            cInfo->Modelid[2] = 0;
        }
        else if (!displayScaleEntry)
            displayScaleEntry = displayEntry;

        CreatureModelInfo const* modelInfo = GetCreatureModelInfo(cInfo->Modelid[2]);
        if (!modelInfo)
            sLog->outError(LOG_FILTER_SQL, "No model data exist for `DisplayId[2]` = %u listed by creature (Entry: %u).", cInfo->Modelid[2], cInfo->Entry);
    }

    if (cInfo->Modelid[3])
    {
        CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(cInfo->Modelid[3]);
        if (!displayEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists non-existing DisplayId[3] id (%u), this can crash the client.", cInfo->Entry, cInfo->Modelid[3]);
            cInfo->Modelid[3] = 0;
        }
        else if (!displayScaleEntry)
            displayScaleEntry = displayEntry;

        CreatureModelInfo const* modelInfo = GetCreatureModelInfo(cInfo->Modelid[3]);
        if (!modelInfo)
            sLog->outError(LOG_FILTER_SQL, "No model data exist for `DisplayId[3]` = %u listed by creature (Entry: %u).", cInfo->Modelid[3], cInfo->Entry);
    }

    if (!displayScaleEntry)
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) does not have any existing display id in DisplayId1/DisplayId/DisplayId3/DisplayId[3].", cInfo->Entry);

    for (uint8 k = 0; k < MAX_KILL_CREDIT; ++k)
        if (cInfo->KillCredit[k])
            if (!GetCreatureTemplate(cInfo->KillCredit[k]))
            {
                sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists non-existing creature entry %u in `KillCredit%d`.", cInfo->Entry, cInfo->KillCredit[k], k + 1);
                cInfo->KillCredit[k] = 0;
            }

    if (cInfo->type && !sCreatureTypeStore.LookupEntry(cInfo->type))
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid creature type (%u) in `type`.", cInfo->Entry, cInfo->type);
        cInfo->type = CREATURE_TYPE_HUMANOID;
    }

    if (cInfo->family && !sCreatureFamilyStore.LookupEntry(cInfo->family) && cInfo->family != CREATURE_FAMILY_HORSE_CUSTOM)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid creature family (%u) in `family`.", cInfo->Entry, cInfo->family);
        cInfo->family = CreatureFamily(0);
    }

    if (cInfo->RequiredExpansion > (MAX_EXPANSION - 1))
    {
        sLog->outError(LOG_FILTER_SQL, "Table `creature_template` lists creature (Entry: %u) with `exp` %u. Ignored and set to 0.", cInfo->Entry, cInfo->RequiredExpansion);
        cInfo->RequiredExpansion = 0;
    }
}

void ObjectMgr::LoadCreatureTemplates()
{
    uint32 oldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query(
    ///           0         1             2         3       4      5        6        7          8           9         10       11       12           13              14              15
        "SELECT entry, gossip_menu_id, minlevel, maxlevel, exp, faction, npcflag, npcflag2, speed_walk, speed_run, speed_fly, scale, dmgschool, dmg_multiplier, baseattacktime, rangeattacktime, "
    ///       16             17          18           19          20           21           22             23             24            25             26            27          28          29
        "baseVariance, rangeVariance, unit_class, unit_flags, unit_flags2, unit_flags3, dynamicflags, WorldEffectID, trainer_type, trainer_spell, trainer_class, trainer_race, lootid, pickpocketloot, "
    ///    30          31           32           33           34           35           36         37      38     39       40      41      42      43      44       45        46       47
        "skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, VehicleId, mingold, maxgold, "
    ///    48          49          50            51          52            53           54           55                    56          57         58              59
        "AIName, MovementType, InhabitType, HoverHeight, Mana_mod_extra, Armor_mod, RegenHealth, mechanic_immune_mask, flags_extra, ScriptName, ScaleLevelMin, ScaleLevelMax, "
    ///         60
        "ScaleLevelDelta FROM creature_template;");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature template definitions. DB table `creature_template` is empty.");
        return;
    }

    QueryResult l_MaxResult = WorldDatabase.Query("SELECT MAX(entry) from creature_template");
    if (!l_MaxResult)
        return;

    m_LastCreatureTemplateEntry = l_MaxResult->Fetch()[0].GetUInt32() + 1; ///< Start from 0

    uint32 count = 0;
    do
    {
        uint8 index = 0;
        Field* fields = l_Result->Fetch();

        uint32 l_Entry = fields[index++].GetUInt32();

        if (!m_CreatureTemplateStore.count(l_Entry))
            continue;

        CreatureTemplate& l_CreatureTemplate = m_CreatureTemplateStore[l_Entry];

        l_CreatureTemplate.Entry = l_Entry;

        for (uint8 i = 0; i < Difficulty::MaxDifficulties; ++i)
            l_CreatureTemplate.DifficultyEntry[i] = 0;

        l_CreatureTemplate.GossipMenuId      = fields[index++].GetUInt32();
        l_CreatureTemplate.minlevel          = fields[index++].GetUInt8();
        l_CreatureTemplate.maxlevel          = fields[index++].GetUInt8();
        l_CreatureTemplate.expansion         = int32(fields[index++].GetInt16());
        l_CreatureTemplate.faction           = uint32(fields[index++].GetUInt16());
        l_CreatureTemplate.NpcFlags1         = fields[index++].GetUInt32();
        l_CreatureTemplate.NpcFlags2         = fields[index++].GetUInt32();
        l_CreatureTemplate.speed_walk        = fields[index++].GetFloat();
        l_CreatureTemplate.speed_run         = fields[index++].GetFloat();
        l_CreatureTemplate.speed_fly         = fields[index++].GetFloat();
        l_CreatureTemplate.scale             = fields[index++].GetFloat();
        l_CreatureTemplate.dmgschool         = uint32(fields[index++].GetInt8());
        l_CreatureTemplate.dmg_multiplier    = fields[index++].GetFloat();
        l_CreatureTemplate.baseattacktime    = fields[index++].GetUInt32();
        l_CreatureTemplate.rangeattacktime   = fields[index++].GetUInt32();
        l_CreatureTemplate.baseVariance      = fields[index++].GetFloat();
        l_CreatureTemplate.rangeVariance     = fields[index++].GetFloat();
        l_CreatureTemplate.unit_class        = uint32(fields[index++].GetUInt8());
        l_CreatureTemplate.UnitFlags1        = fields[index++].GetUInt32();
        l_CreatureTemplate.UnitFlags2        = fields[index++].GetUInt32();
        l_CreatureTemplate.UnitFlags3        = fields[index++].GetUInt32();
        l_CreatureTemplate.dynamicflags      = fields[index++].GetUInt32();
        l_CreatureTemplate.WorldEffectID     = fields[index++].GetUInt32();
        l_CreatureTemplate.trainer_type      = uint32(fields[index++].GetUInt8());
        l_CreatureTemplate.trainer_spell     = fields[index++].GetUInt32();
        l_CreatureTemplate.trainer_class     = uint32(fields[index++].GetUInt8());
        l_CreatureTemplate.trainer_race      = uint32(fields[index++].GetUInt8());
        l_CreatureTemplate.lootid            = fields[index++].GetUInt32();
        l_CreatureTemplate.pickpocketLootId  = fields[index++].GetUInt32();
        l_CreatureTemplate.SkinLootId        = fields[index++].GetUInt32();

        for (uint8 i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
            l_CreatureTemplate.resistance[i] = fields[index++].GetInt16();

        for (uint8 i = 0; i < MAX_CREATURE_SPELLS; ++i)
            l_CreatureTemplate.spells[i] = fields[index++].GetUInt32();

        l_CreatureTemplate.VehicleId            = fields[index++].GetUInt32();
        l_CreatureTemplate.mingold              = fields[index++].GetUInt32();
        l_CreatureTemplate.maxgold              = fields[index++].GetUInt32();
        l_CreatureTemplate.AIName               = fields[index++].GetString();
        l_CreatureTemplate.MovementType         = uint32(fields[index++].GetUInt8());
        l_CreatureTemplate.InhabitType          = uint32(fields[index++].GetUInt8());
        l_CreatureTemplate.HoverHeight          = fields[index++].GetFloat();
        l_CreatureTemplate.ModManaExtra         = fields[index++].GetFloat();
        l_CreatureTemplate.ModArmor             = fields[index++].GetFloat();
        l_CreatureTemplate.RegenHealth          = fields[index++].GetBool();
        l_CreatureTemplate.MechanicImmuneMask   = fields[index++].GetUInt32();
        l_CreatureTemplate.flags_extra          = fields[index++].GetUInt32();
        l_CreatureTemplate.ScriptID             = GetScriptId(fields[index++].GetCString());
        l_CreatureTemplate.ScaleLevelMin        = fields[index++].GetUInt8();
        l_CreatureTemplate.ScaleLevelMax        = fields[index++].GetUInt8();
        l_CreatureTemplate.ScaleLevelDelta      = fields[index++].GetInt8();

        ++count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureTemplatesDifficulties()
{
    uint32 l_OldMSTime = getMSTime();

    //                                                  0           1          2
    QueryResult l_Result = WorldDatabase.Query("SELECT entry, CONVERT(difficulty, UNSIGNED), difficulty_entry FROM creature_template_difficulty");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature template difficulties definitions. DB table `creature_template_difficulty` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        uint8 l_Index = 0;
        Field* l_Fields = l_Result->Fetch();

        uint32 l_Entry           = l_Fields[l_Index++].GetUInt32();

        if (!m_CreatureTemplateStore.count(l_Entry))
            continue;

        uint32 l_DifficultyIndex = l_Fields[l_Index++].GetUInt64() - 2;
        uint32 l_DifficultyEntry = l_Fields[l_Index++].GetUInt32();

        if (l_DifficultyIndex >= Difficulty::MaxDifficulties)
            continue;

        CreatureTemplate& l_CreatureTemplate = m_CreatureTemplateStore[l_Entry];

        l_CreatureTemplate.DifficultyEntry[l_DifficultyIndex] = l_DifficultyEntry;

        ++l_Count;
    }
    while (l_Result->NextRow());

    // Checking needs to be done after loading because of the difficulty self referencing
    CreatureTemplateContainer const* l_Templates = GetCreatureTemplates();
    for (auto l_Iter = l_Templates->begin(); l_Iter != l_Templates->end(); ++l_Iter)
        CheckCreatureTemplate(&l_Iter->second);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature difficulties definitions in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadCreatureTemplateAddons()
{
    uint32 l_OldMSTime = getMSTime();

    ///                                                 0       1       2      3       4       5          6       7                 8            9
    QueryResult l_Result = WorldDatabase.Query("SELECT entry, path_id, mount, bytes1, bytes2, emote, aiAnimKit, movementAnimKit, meleeAnimKit, auras FROM creature_template_addon");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature template addon definitions. DB table `creature_template_addon` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 l_Entry = l_Fields[0].GetUInt32();

        if (!sObjectMgr->GetCreatureTemplate(l_Entry))
            continue;

        CreatureAddon& l_CreatureAddon = _creatureTemplateAddonStore[l_Entry];

        l_CreatureAddon.PathID          = l_Fields[1].GetUInt32();
        l_CreatureAddon.Mount           = l_Fields[2].GetUInt32();
        l_CreatureAddon.Bytes1          = l_Fields[3].GetUInt32();
        l_CreatureAddon.Bytes2          = l_Fields[4].GetUInt32();
        l_CreatureAddon.Emote           = l_Fields[5].GetUInt32();
        l_CreatureAddon.aiAnimKit       = l_Fields[6].GetUInt16();
        l_CreatureAddon.movementAnimKit = l_Fields[7].GetUInt16();
        l_CreatureAddon.meleeAnimKit    = l_Fields[8].GetUInt16();

        Tokenizer l_Tokens(l_Fields[9].GetString(), ' ');
        uint8 l_Index = 0;
        l_CreatureAddon.Auras.resize(l_Tokens.size());
        for (Tokenizer::const_iterator l_Iter = l_Tokens.begin(); l_Iter != l_Tokens.end(); ++l_Iter)
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(uint32(atol(*l_Iter)));
            if (!l_SpellInfo)
                continue;

            l_CreatureAddon.Auras[l_Index++] = uint32(atol(*l_Iter));
        }

        if (l_CreatureAddon.Mount && !sCreatureDisplayInfoStore.LookupEntry(l_CreatureAddon.Mount))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid Mount (%u) defined in `creature_template_addon`.", l_Entry, l_CreatureAddon.Mount);
            l_CreatureAddon.Mount = 0;
        }

        if (l_CreatureAddon.Emote && !sEmotesStore.LookupEntry(l_CreatureAddon.Emote))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid Emote (%u) defined in `creature_template_addon`.", l_Entry, l_CreatureAddon.Emote);
            l_CreatureAddon.Emote = 0;
        }

        if (l_CreatureAddon.aiAnimKit && !sAnimKitStore.LookupEntry(l_CreatureAddon.aiAnimKit))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid aiAnimKit (%u) defined in `creature_template_addon`.", l_Entry, l_CreatureAddon.aiAnimKit);
            l_CreatureAddon.aiAnimKit = 0;
        }

        if (l_CreatureAddon.movementAnimKit && !sAnimKitStore.LookupEntry(l_CreatureAddon.movementAnimKit))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid movementAnimKit (%u) defined in `creature_template_addon`.", l_Entry, l_CreatureAddon.movementAnimKit);
            l_CreatureAddon.movementAnimKit = 0;
        }

        if (l_CreatureAddon.meleeAnimKit && !sAnimKitStore.LookupEntry(l_CreatureAddon.meleeAnimKit))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid meleeAnimKit (%u) defined in `creature_template_addon`.", l_Entry, l_CreatureAddon.meleeAnimKit);
            l_CreatureAddon.meleeAnimKit = 0;
        }

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature template addons in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::CheckCreatureTemplate(CreatureTemplate const* cInfo)
{
    if (!cInfo)
        return;

    for (uint32 diff = 0; diff < Difficulty::MaxDifficulties - 1; ++diff)
    {
        if (!cInfo->DifficultyEntry[diff])
            continue;

        CreatureTemplate const* difficultyInfo = GetCreatureTemplate(cInfo->DifficultyEntry[diff]);
        if (!difficultyInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has `difficulty_entry_%u`=%u but creature entry %u does not exist.",
                cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff], cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (_creaturesDiffEntry.find(cInfo->Entry) != _creaturesDiffEntry.end())
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) is listed as `difficulty_entry` of another creature.",
                cInfo->Entry);
            continue;
        }

        if (_creaturesOrigEntry.find(cInfo->DifficultyEntry[diff]) != _creaturesOrigEntry.end())
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) is listed as `difficulty_entry` of another creature.",
                cInfo->Entry);
            continue;
        }

        if (cInfo->unit_class != difficultyInfo->unit_class)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u, class %u) has different `unit_class` in difficulty %u mode (Entry: %u, class %u).",
                cInfo->Entry, cInfo->unit_class, diff + 1, cInfo->DifficultyEntry[diff], difficultyInfo->unit_class);
            continue;
        }

        if (cInfo->NpcFlags1 != difficultyInfo->NpcFlags1)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has different `npcflag` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (cInfo->NpcFlags2 != difficultyInfo->NpcFlags2)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has different `npcflag2` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (cInfo->trainer_class != difficultyInfo->trainer_class)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has different `trainer_class` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (cInfo->trainer_race != difficultyInfo->trainer_race)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has different `trainer_race` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (cInfo->trainer_type != difficultyInfo->trainer_type)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has different `trainer_type` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (cInfo->trainer_spell != difficultyInfo->trainer_spell)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has different `trainer_spell` in difficulty %u mode (Entry: %u).", cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (!difficultyInfo->AIName.empty())
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists difficulty %u mode entry %u with `AIName` filled in. `AIName` of difficulty 0 mode creature is always used instead.",
                cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        if (difficultyInfo->ScriptID)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists difficulty %u mode entry %u with `ScriptName` filled in. `ScriptName` of difficulty 0 mode creature is always used instead.",
                cInfo->Entry, diff + 1, cInfo->DifficultyEntry[diff]);
            continue;
        }

        _hasDifficultyEntries[diff].insert(cInfo->Entry);
        _difficultyEntries[diff].insert(cInfo->DifficultyEntry[diff]);
        _creaturesOrigEntry.insert(cInfo->Entry);
        _creaturesDiffEntry.insert(cInfo->DifficultyEntry[diff]);
    }

    FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->faction);
    if (!factionTemplate)
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has non-existing faction template (%u).", cInfo->Entry, cInfo->faction);

    // used later for scale
    CreatureDisplayInfoEntry const* displayScaleEntry = NULL;

    for (uint32 l_I = 0; l_I < MAX_CREATURE_MODELS; ++l_I)
    {
        if (uint32 l_ModelID = cInfo->Modelid[l_I])
        {
            CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(l_ModelID);
            if (!displayEntry)
            {
                sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists non-existing Modelid1 id (%u), this can crash the client.", cInfo->Entry, l_ModelID);
                const_cast<CreatureTemplate*>(cInfo)->Modelid[l_I] = 0;
                l_ModelID = 0;
            }
            else if (!displayScaleEntry)
                displayScaleEntry = displayEntry;

            CreatureModelInfo const* modelInfo = GetCreatureModelInfo(l_ModelID);
            if (!modelInfo)
                sLog->outError(LOG_FILTER_SQL, "No model data exist for `Modelid1` = %u listed by creature (Entry: %u).", l_ModelID, cInfo->Entry);
        }
    }

    if (!displayScaleEntry)
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) does not have any existing display id in Modelid1/Modelid2/Modelid3/Modelid4.", cInfo->Entry);

    for (int k = 0; k < MAX_KILL_CREDIT; ++k)
    {
        if (cInfo->KillCredit[k])
        {
            if (!GetCreatureTemplate(cInfo->KillCredit[k]))
            {
                sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) lists non-existing creature entry %u in `KillCredit%d`.", cInfo->Entry, cInfo->KillCredit[k], k + 1);
                const_cast<CreatureTemplate*>(cInfo)->KillCredit[k] = 0;
            }
        }
    }

    if (!cInfo->unit_class || ((1 << (cInfo->unit_class-1)) & CLASSMASK_ALL_CREATURES) == 0)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid unit_class (%u) in creature_template. Set to 1 (UNIT_CLASS_WARRIOR).", cInfo->Entry, cInfo->unit_class);
        const_cast<CreatureTemplate*>(cInfo)->unit_class = UNIT_CLASS_WARRIOR;
    }

    if (cInfo->dmgschool >= MAX_SPELL_SCHOOL)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid spell school value (%u) in `dmgschool`.", cInfo->Entry, cInfo->dmgschool);
        const_cast<CreatureTemplate*>(cInfo)->dmgschool = SPELL_SCHOOL_NORMAL;
    }

    if (cInfo->baseattacktime == 0)
        const_cast<CreatureTemplate*>(cInfo)->baseattacktime  = BASE_ATTACK_TIME;

    if (cInfo->rangeattacktime == 0)
        const_cast<CreatureTemplate*>(cInfo)->rangeattacktime = BASE_ATTACK_TIME;

    if ((cInfo->NpcFlags1 & UNIT_NPC_FLAG_TRAINER) && cInfo->trainer_type >= MAX_TRAINER_TYPE)
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has wrong trainer type %u.", cInfo->Entry, cInfo->trainer_type);

    if (cInfo->type && !sCreatureTypeStore.LookupEntry(cInfo->type))
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid creature type (%u) in `type`.", cInfo->Entry, cInfo->type);
        const_cast<CreatureTemplate*>(cInfo)->type = CREATURE_TYPE_HUMANOID;
    }

    // must exist or used hidden but used in data horse case
    if (cInfo->family && !sCreatureFamilyStore.LookupEntry(cInfo->family) && cInfo->family != CREATURE_FAMILY_HORSE_CUSTOM)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has invalid creature family (%u) in `family`.", cInfo->Entry, cInfo->family);
        const_cast<CreatureTemplate*>(cInfo)->family = CREATURE_FAMILY_NONE;
    }

    if (cInfo->InhabitType <= 0 || cInfo->InhabitType > INHABIT_ANYWHERE)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has wrong value (%u) in `InhabitType`, creature will not correctly walk/swim/fly.", cInfo->Entry, cInfo->InhabitType);
        const_cast<CreatureTemplate*>(cInfo)->InhabitType = INHABIT_ANYWHERE;
    }

    if (cInfo->HoverHeight < 0.0f)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has wrong value (%f) in `HoverHeight`", cInfo->Entry, cInfo->HoverHeight);
        const_cast<CreatureTemplate*>(cInfo)->HoverHeight = 1.0f;
    }

    if (cInfo->VehicleId)
    {
        VehicleEntry const* vehId = sVehicleStore.LookupEntry(cInfo->VehicleId);
        if (!vehId)
        {
             sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has a non-existing VehicleId (%u). This *WILL* cause the client to freeze!", cInfo->Entry, cInfo->VehicleId);
             const_cast<CreatureTemplate*>(cInfo)->VehicleId = 0;
        }
    }

    for (uint8 j = 0; j < MAX_CREATURE_SPELLS; ++j)
    {
        if (cInfo->spells[j] && !sSpellMgr->GetSpellInfo(cInfo->spells[j]))
        {
            WorldDatabase.PExecute("UPDATE creature_template SET spell%d = 0 WHERE entry = %u", j+1, cInfo->Entry);
            sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has non-existing Spell%d (%u), set to 0.", cInfo->Entry, j+1, cInfo->spells[j]);
            const_cast<CreatureTemplate*>(cInfo)->spells[j] = 0;
        }
    }

    if (cInfo->MovementType >= MAX_DB_MOTION_TYPE)
    {
        sLog->outError(LOG_FILTER_SQL, "Creature (Entry: %u) has wrong movement generator type (%u), ignored and set to IDLE.", cInfo->Entry, cInfo->MovementType);
        const_cast<CreatureTemplate*>(cInfo)->MovementType = IDLE_MOTION_TYPE;
    }

    /// if not set custom creature scale then load scale from CreatureDisplayInfo.db2
    if (cInfo->scale <= 0.0f)
    {
        if (displayScaleEntry)
            const_cast<CreatureTemplate*>(cInfo)->scale = displayScaleEntry->CreatureModelScale;
        else
            const_cast<CreatureTemplate*>(cInfo)->scale = 1.0f;
    }

    if (cInfo->expansion > MAX_CREATURE_BASE_HP)
    {
        sLog->outError(LOG_FILTER_SQL, "Table `creature_template` lists creature (Entry: %u) with `exp` %u. Ignored and set to 0.", cInfo->Entry, cInfo->expansion);
        const_cast<CreatureTemplate*>(cInfo)->expansion = 0;
    }

    // -1 is used in the client for auto-updating the levels
    // having their expansion set to it to the latest one
    if (cInfo->expansion == -1)
    {
        const_cast<CreatureTemplate*>(cInfo)->minlevel = MAX_LEVEL;
        const_cast<CreatureTemplate*>(cInfo)->maxlevel = MAX_LEVEL;
        const_cast<CreatureTemplate*>(cInfo)->expansion = MAX_CREATURE_BASE_HP;
    }

    if (cInfo->RequiredExpansion > MAX_CREATURE_BASE_HP)
    {
        sLog->outError(LOG_FILTER_SQL, "Table `creature_template` lists creature (Entry: %u) with `exp_req` %u. Ignored and set to 0.", cInfo->Entry, cInfo->RequiredExpansion);
        const_cast<CreatureTemplate*>(cInfo)->RequiredExpansion = 0;
    }

    if (uint32 badFlags = (cInfo->flags_extra & ~CREATURE_FLAG_EXTRA_DB_ALLOWED))
    {
        sLog->outError(LOG_FILTER_SQL, "Table `creature_template` lists creature (Entry: %u) with disallowed `flags_extra` %u, removing incorrect flag.", cInfo->Entry, badFlags);
        const_cast<CreatureTemplate*>(cInfo)->flags_extra &= CREATURE_FLAG_EXTRA_DB_ALLOWED;
    }

    if (cInfo->VignetteID && !sVignetteStore.LookupEntry(cInfo->VignetteID))
    {
        /// For Legion, some rares/worldbosses have VignetteID = TrackingQuestID (for WorldQuest system)
        if (cInfo->VignetteID != cInfo->TrackingQuestID || cInfo->expansion < Expansion::EXPANSION_LEGION)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature(Entry: %u) has a non - existing vignette id (%u)", cInfo->Entry, cInfo->VignetteID);
            const_cast<CreatureTemplate*>(cInfo)->VignetteID = 0;
        }
    }

    const_cast<CreatureTemplate*>(cInfo)->dmg_multiplier *= Creature::_GetDamageMod(cInfo->rank);
}

void ObjectMgr::LoadCreatureAddons()
{
    uint32 l_OldMSTime = getMSTime();

    ///                                                  0      1       2      3       4       5      6            7               8            9           10
    QueryResult l_Result = WorldDatabase.Query("SELECT guid, linked_id, path_id, mount, bytes1, bytes2, emote, aiAnimKit, movementAnimKit, meleeAnimKit, auras FROM creature_addon");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature addon definitions. DB table `creature_addon` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 guid = l_Fields[0].GetUInt32();
        std::string l_LinkedId = l_Fields[1].GetString();

        CreatureData const* l_CreatureData = GetCreatureData(guid ? guid : GetGUIDByLinkedId(l_LinkedId));
        if (!l_CreatureData)
            continue;

        CreatureAddon& l_CreatureAddon = !guid ? _creatureAddonStoreByLinked[l_LinkedId] : _creatureAddonStoreByGuid[guid];

        l_CreatureAddon.PathID = l_Fields[2].GetUInt32();
        if (l_CreatureData->movementType == WAYPOINT_MOTION_TYPE && !l_CreatureAddon.PathID)
            const_cast<CreatureData*>(l_CreatureData)->movementType = IDLE_MOTION_TYPE;

        l_CreatureAddon.Mount           = l_Fields[3].GetUInt32();
        l_CreatureAddon.Bytes1          = l_Fields[4].GetUInt32();
        l_CreatureAddon.Bytes2          = l_Fields[5].GetUInt32();
        l_CreatureAddon.Emote           = l_Fields[6].GetUInt32();
        l_CreatureAddon.aiAnimKit       = l_Fields[7].GetUInt16();
        l_CreatureAddon.movementAnimKit = l_Fields[8].GetUInt16();
        l_CreatureAddon.meleeAnimKit    = l_Fields[9].GetUInt16();

        Tokenizer l_Tokens(l_Fields[10].GetString(), ' ');
        uint8 l_Index = 0;
        l_CreatureAddon.Auras.resize(l_Tokens.size());
        for (Tokenizer::const_iterator l_Iter = l_Tokens.begin(); l_Iter != l_Tokens.end(); ++l_Iter)
        {
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(uint32(atol(*l_Iter)));
            if (!l_SpellInfo)
                continue;

            l_CreatureAddon.Auras[l_Index++] = uint32(atol(*l_Iter));
        }

        if (l_CreatureAddon.Mount && !sCreatureDisplayInfoStore.LookupEntry(l_CreatureAddon.Mount))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (GUID: %u) has invalid Mount (%u) defined in `creature_template_addon`.", sObjectMgr->GetGUIDByLinkedId(l_LinkedId), l_CreatureAddon.Mount);
            l_CreatureAddon.Mount = 0;
        }

        if (l_CreatureAddon.Emote && !sEmotesStore.LookupEntry(l_CreatureAddon.Emote))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (GUID: %u) has invalid Emote (%u) defined in `creature_template_addon`.", sObjectMgr->GetGUIDByLinkedId(l_LinkedId), l_CreatureAddon.Emote);
            l_CreatureAddon.Emote = 0;
        }

        if (l_CreatureAddon.aiAnimKit && !sAnimKitStore.LookupEntry(l_CreatureAddon.aiAnimKit))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (GUID: %u) has invalid aiAnimKit (%u) defined in `creature_template_addon`.", sObjectMgr->GetGUIDByLinkedId(l_LinkedId), l_CreatureAddon.aiAnimKit);
            l_CreatureAddon.aiAnimKit = 0;
        }

        if (l_CreatureAddon.movementAnimKit && !sAnimKitStore.LookupEntry(l_CreatureAddon.movementAnimKit))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (GUID: %u) has invalid movementAnimKit (%u) defined in `creature_template_addon`.", sObjectMgr->GetGUIDByLinkedId(l_LinkedId), l_CreatureAddon.movementAnimKit);
            l_CreatureAddon.movementAnimKit = 0;
        }

        if (l_CreatureAddon.meleeAnimKit && !sAnimKitStore.LookupEntry(l_CreatureAddon.meleeAnimKit))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature (GUID: %u) has invalid meleeAnimKit (%u) defined in `creature_template_addon`.", sObjectMgr->GetGUIDByLinkedId(l_LinkedId), l_CreatureAddon.meleeAnimKit);
            l_CreatureAddon.meleeAnimKit = 0;
        }

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature addons in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

CreatureAddon const* ObjectMgr::GetCreatureAddon(uint32 lowguid)
{
    CreatureDataContainer::const_iterator l_Itr = _creatureDataStore.find(lowguid);
    if (l_Itr == _creatureDataStore.end())
        return nullptr;

    CreatureAddonContainer::const_iterator itr = _creatureAddonStoreByGuid.find(lowguid);
    if (itr != _creatureAddonStoreByGuid.end())
        return &(itr->second);

    std::unordered_map<std::string, CreatureAddon>::const_iterator l_NewItr = _creatureAddonStoreByLinked.find(l_Itr->second.linkedId);
    if (l_NewItr != _creatureAddonStoreByLinked.end())
        return &(l_NewItr->second);

    return nullptr;
}

CreatureAddon const* ObjectMgr::GetCreatureTemplateAddon(uint32 entry)
{
    CreatureAddonContainer::const_iterator itr = _creatureTemplateAddonStore.find(entry);
    if (itr != _creatureTemplateAddonStore.end())
        return &(itr->second);

    return NULL;
}

EquipmentInfo const* ObjectMgr::GetEquipmentInfo(uint32 p_Entry, int8& p_ID)
{
    EquipmentInfoContainer::const_iterator itr = _equipmentInfoStore.find(p_Entry);
    if (itr == _equipmentInfoStore.end())
        return NULL;

    if (itr->second.empty())
        return NULL;

    if (p_ID == -1) // Select a random element
    {
        EquipmentInfoContainerInternal::const_iterator l_Iter = itr->second.begin();
        std::advance(l_Iter, urand(0, itr->second.size() - 1));
        p_ID = (int8)std::distance(itr->second.begin(), l_Iter) + 1;
        return &l_Iter->second;
    }
    else
    {
        EquipmentInfoContainerInternal::const_iterator l_Iter = itr->second.find(p_ID);
        if (l_Iter != itr->second.end())
            return &l_Iter->second;
    }

    return NULL;
}

void ObjectMgr::LoadEquipmentTemplates()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0     1       2           3           4
    QueryResult result = WorldDatabase.Query("SELECT entry, id, itemEntry1, itemEntry2, itemEntry3 FROM creature_equip_template");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature equipment templates. DB table `creature_equip_template` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint8 l_Index = 0;

        uint32 entry = fields[l_Index++].GetUInt32();

        if (!sObjectMgr->GetCreatureTemplate(entry))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature template (Entry: %u) does not exist but has a record in `creature_equip_template`", entry);
            continue;
        }

        uint8 id = fields[l_Index++].GetUInt8();
        if (!id)
        {
            sLog->outError(LOG_FILTER_SQL, "Creature equipment template with ID 0 found for creature %u, skipped.", entry);
            continue;
        }

        EquipmentInfo& equipmentInfo = _equipmentInfoStore[entry][id];

        equipmentInfo.ItemEntry[0] = fields[l_Index++].GetUInt32();
        equipmentInfo.ItemEntry[1] = fields[l_Index++].GetUInt32();
        equipmentInfo.ItemEntry[2] = fields[l_Index++].GetUInt32();

        for (uint8 i = 0; i < MAX_EQUIPMENT_ITEMS; ++i)
        {
            if (!equipmentInfo.ItemEntry[i])
                continue;

            ItemEntry const* dbcItem = sItemStore.LookupEntry(equipmentInfo.ItemEntry[i]);

            if (!dbcItem)
            {
                sLog->outError(LOG_FILTER_SQL, "Unknown item (entry=%u) in creature_equip_template.itemEntry%u for entry = %u and id=%u, forced to 0.",
                    equipmentInfo.ItemEntry[i], i + 1, entry, id);
                equipmentInfo.ItemEntry[i] = 0;
                continue;
            }

            if (dbcItem->InventoryType != INVTYPE_WEAPON &&
                dbcItem->InventoryType != INVTYPE_SHIELD &&
                dbcItem->InventoryType != INVTYPE_RANGED &&
                dbcItem->InventoryType != INVTYPE_2HWEAPON &&
                dbcItem->InventoryType != INVTYPE_WEAPONMAINHAND &&
                dbcItem->InventoryType != INVTYPE_WEAPONOFFHAND &&
                dbcItem->InventoryType != INVTYPE_HOLDABLE &&
                dbcItem->InventoryType != INVTYPE_THROWN &&
                dbcItem->InventoryType != INVTYPE_RANGEDRIGHT)
            {
                sLog->outError(LOG_FILTER_SQL, "Item (entry=%u) in creature_equip_template.itemEntry%u for entry = %u and id = %u is not equipable in a hand, forced to 0.",
                    equipmentInfo.ItemEntry[i], i + 1, entry, id);
                equipmentInfo.ItemEntry[i] = 0;
            }
        }

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u equipment templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

CreatureModelInfo const* ObjectMgr::GetCreatureModelInfo(uint32 modelId)
{
    CreatureModelContainer::const_iterator itr = _creatureModelStore.find(modelId);
    if (itr != _creatureModelStore.end())
        return &(itr->second);

    return NULL;
}

uint32 ObjectMgr::ChooseDisplayId(uint32 /*team*/, const CreatureTemplate* cinfo, const CreatureData* data /*= NULL*/)
{
    // Load creature model (display id)
    uint32 display_id = 0;

    if (!data || data->displayid == 0)
    {
        display_id = cinfo->GetRandomValidModelId();
    }
    else
        return data->displayid;

    return display_id;
}

void ObjectMgr::ChooseCreatureFlags(const CreatureTemplate* p_CreatureTemplate, uint32 & p_NpcFlags1, uint32 & p_NpcFlags2, uint32 & p_UnitFlags1, uint32 & p_UnitFlags2, uint32 & p_UnitFlags3, uint32 & p_Dynamicflags, const CreatureData* p_Data)
{
    p_NpcFlags1     = p_CreatureTemplate->NpcFlags1;
    p_NpcFlags2     = p_CreatureTemplate->NpcFlags2;
    p_UnitFlags1    = p_CreatureTemplate->UnitFlags1;
    p_UnitFlags2    = p_CreatureTemplate->UnitFlags2;
    p_UnitFlags3    = p_CreatureTemplate->UnitFlags3;
    p_Dynamicflags  = p_CreatureTemplate->dynamicflags;

    if (p_Data)
    {
        if (p_Data->NpcFlags1)
            p_NpcFlags1 = p_Data->NpcFlags1;

        if (p_Data->NpcFlags2)
            p_NpcFlags2 = p_Data->NpcFlags2;

        if (p_Data->UnitFlags1)
            p_UnitFlags1 = p_Data->UnitFlags1;

        if (p_Data->UnitFlags2)
            p_UnitFlags2 = p_Data->UnitFlags2;

        if (p_Data->UnitFlags3)
            p_UnitFlags3 = p_Data->UnitFlags3;

        if (p_Data->dynamicflags)
            p_Dynamicflags = p_Data->dynamicflags;
    }
}

CreatureModelInfo const* ObjectMgr::GetCreatureModelRandomGender(uint32* displayID)
{
    CreatureModelInfo const* modelInfo = GetCreatureModelInfo(*displayID);
    if (!modelInfo)
        return NULL;

    /// If a model for another gender exists, 50% chance to use it
    if (modelInfo->displayId_other_gender != 0 && urand(0, 1) == 0)
    {
        CreatureModelInfo const* minfo_tmp = GetCreatureModelInfo(modelInfo->displayId_other_gender);
        if (!minfo_tmp)
            sLog->outError(LOG_FILTER_SQL, "Model (Entry: %u) has displayId_other_gender %u not found in table `creature_model_info`. ", *displayID, modelInfo->displayId_other_gender);
        else
        {
            /// DisplayID changed
            *displayID = modelInfo->displayId_other_gender;
            return minfo_tmp;
        }
    }

    return modelInfo;
}

void ObjectMgr::LoadCreatureModelInfo()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT DisplayID, BoundingRadius, CombatReach, DisplayID_Other_Gender FROM creature_model_info");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature model definitions. DB table `creature_model_info` is empty.");
        return;
    }

    _creatureModelStore.rehash(result->GetRowCount());
    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 displayId = fields[0].GetUInt32();

        CreatureDisplayInfoEntry const* creatureDisplay = sCreatureDisplayInfoStore.LookupEntry(displayId);
        if (!creatureDisplay)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature_model_info` has model for not existed display id (%u).", displayId);
            continue;
        }

        CreatureModelInfo& modelInfo = _creatureModelStore[displayId];

        modelInfo.bounding_radius        = fields[1].GetFloat();
        modelInfo.combat_reach           = fields[2].GetFloat();
        modelInfo.displayId_other_gender = fields[3].GetUInt32();
        modelInfo.gender                 = creatureDisplay->Gender;

        if (modelInfo.gender != creatureDisplay->Gender)
            modelInfo.gender = creatureDisplay->Gender;

        // Checks
        /// To remove when the purpose of GENDER_UNKNOWN is known
        if (modelInfo.gender == GENDER_UNKNOWN)
        {
            ///sLog->outError(LOG_FILTER_SQL, "Table `creature_model_info` has wrong gender (%u) for display id (%u).", uint32(modelInfo.gender), displayId);
            modelInfo.gender = GENDER_MALE;
        }

        if (modelInfo.displayId_other_gender && !sCreatureDisplayInfoStore.LookupEntry(modelInfo.displayId_other_gender))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature_model_info` has not existed DisplayID_Other_Gender (ID: %u) for existed display (ID: %u).", modelInfo.displayId_other_gender, displayId);
            modelInfo.displayId_other_gender = 0;
        }

        if (modelInfo.combat_reach < 0.1f)
            modelInfo.combat_reach = DEFAULT_COMBAT_REACH;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature model based info in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadLinkedRespawn()
{
    uint32 oldMSTime = getMSTime();

    _linkedRespawnStore.clear();
    //                                                 0        1          2
    QueryResult result = WorldDatabase.Query("SELECT guid, linkedGuid, linkType FROM linked_respawn ORDER BY guid ASC");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 linked respawns. DB table `linked_respawn` is empty.");

        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 guidLow = fields[0].GetUInt32();
        uint32 linkedGuidLow = fields[1].GetUInt32();
        uint8  linkType = fields[2].GetUInt8();

        uint64 guid = 0, linkedGuid = 0;
        bool error = false;
        switch (linkType)
        {
            case CREATURE_TO_CREATURE:
            {
                const CreatureData* slave = GetCreatureData(guidLow);
                if (!slave)
                {
                    sLog->outError(LOG_FILTER_SQL, "Couldn't get creature data for GUIDLow %u", guidLow);
                    error = true;
                    break;
                }

                const CreatureData* master = GetCreatureData(linkedGuidLow);
                if (!master)
                {
                    sLog->outError(LOG_FILTER_SQL, "Couldn't get creature data for GUIDLow %u", linkedGuidLow);
                    error = true;
                    break;
                }

                const MapEntry* const map = sMapStore.LookupEntry(master->mapid);
                if (!map || !map->Instanceable() || (master->mapid != slave->mapid))
                {
                    sLog->outError(LOG_FILTER_SQL, "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
                {
                    sLog->outError(LOG_FILTER_SQL, "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                guid = MAKE_NEW_GUID(guidLow, slave->id, HIGHGUID_UNIT);
                linkedGuid = MAKE_NEW_GUID(linkedGuidLow, master->id, HIGHGUID_UNIT);
                break;
            }
            case CREATURE_TO_GO:
            {
                const CreatureData* slave = GetCreatureData(guidLow);
                if (!slave)
                {
                    sLog->outError(LOG_FILTER_SQL, "Couldn't get creature data for GUIDLow %u", guidLow);
                    error = true;
                    break;
                }

                const GameObjectData* master = GetGOData(linkedGuidLow);
                if (!master)
                {
                    sLog->outError(LOG_FILTER_SQL, "Couldn't get gameobject data for GUIDLow %u", linkedGuidLow);
                    error = true;
                    break;
                }

                const MapEntry* const map = sMapStore.LookupEntry(master->mapid);
                if (!map || !map->Instanceable() || (master->mapid != slave->mapid))
                {
                    sLog->outError(LOG_FILTER_SQL, "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
                {
                    sLog->outError(LOG_FILTER_SQL, "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                guid = MAKE_NEW_GUID(guidLow, slave->id, HIGHGUID_UNIT);
                linkedGuid = MAKE_NEW_GUID(linkedGuidLow, master->id, HIGHGUID_GAMEOBJECT);
                break;
            }
            case GO_TO_GO:
            {
                const GameObjectData* slave = GetGOData(guidLow);
                if (!slave)
                {
                    sLog->outError(LOG_FILTER_SQL, "Couldn't get gameobject data for GUIDLow %u", guidLow);
                    error = true;
                    break;
                }

                const GameObjectData* master = GetGOData(linkedGuidLow);
                if (!master)
                {
                    sLog->outError(LOG_FILTER_SQL, "Couldn't get gameobject data for GUIDLow %u", linkedGuidLow);
                    error = true;
                    break;
                }

                const MapEntry* const map = sMapStore.LookupEntry(master->mapid);
                if (!map || !map->Instanceable() || (master->mapid != slave->mapid))
                {
                    sLog->outError(LOG_FILTER_SQL, "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
                {
                    sLog->outError(LOG_FILTER_SQL, "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                guid = MAKE_NEW_GUID(guidLow, slave->id, HIGHGUID_GAMEOBJECT);
                linkedGuid = MAKE_NEW_GUID(linkedGuidLow, master->id, HIGHGUID_GAMEOBJECT);
                break;
            }
            case GO_TO_CREATURE:
            {
                const GameObjectData* slave = GetGOData(guidLow);
                if (!slave)
                {
                    sLog->outError(LOG_FILTER_SQL, "Couldn't get gameobject data for GUIDLow %u", guidLow);
                    error = true;
                    break;
                }

                const CreatureData* master = GetCreatureData(linkedGuidLow);
                if (!master)
                {
                    sLog->outError(LOG_FILTER_SQL, "Couldn't get creature data for GUIDLow %u", linkedGuidLow);
                    error = true;
                    break;
                }

                const MapEntry* const map = sMapStore.LookupEntry(master->mapid);
                if (!map || !map->Instanceable() || (master->mapid != slave->mapid))
                {
                    sLog->outError(LOG_FILTER_SQL, "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
                {
                    sLog->outError(LOG_FILTER_SQL, "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
                    error = true;
                    break;
                }

                guid = MAKE_NEW_GUID(guidLow, slave->id, HIGHGUID_GAMEOBJECT);
                linkedGuid = MAKE_NEW_GUID(linkedGuidLow, master->id, HIGHGUID_UNIT);
                break;
            }
        }

        if (!error)
            _linkedRespawnStore[guid] = linkedGuid;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded " UI64FMTD " linked respawns in %u ms", uint64(_linkedRespawnStore.size()), GetMSTimeDiffToNow(oldMSTime));
}

bool ObjectMgr::SetCreatureLinkedRespawn(uint32 guidLow, uint32 linkedGuidLow)
{
    if (!guidLow)
        return false;

    const CreatureData* master = GetCreatureData(guidLow);
    uint64 guid = MAKE_NEW_GUID(guidLow, master->id, HIGHGUID_UNIT);

    if (!linkedGuidLow) // we're removing the linking
    {
        _linkedRespawnStore.erase(guid);
        PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_CRELINKED_RESPAWN);
        stmt->setUInt32(0, guidLow);
        WorldDatabase.Execute(stmt);
        return true;
    }

    const CreatureData* slave = GetCreatureData(linkedGuidLow);

    const MapEntry* const map = sMapStore.LookupEntry(master->mapid);
    if (!map || !map->Instanceable() || (master->mapid != slave->mapid))
    {
        sLog->outError(LOG_FILTER_SQL, "Creature '%u' linking to '%u' on an unpermitted map.", guidLow, linkedGuidLow);
        return false;
    }

    if (!(master->spawnMask & slave->spawnMask))  // they must have a possibility to meet (normal/heroic difficulty)
    {
        sLog->outError(LOG_FILTER_SQL, "LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guidLow, linkedGuidLow);
        return false;
    }

    uint64 linkedGuid = MAKE_NEW_GUID(linkedGuidLow, slave->id, HIGHGUID_UNIT);

    _linkedRespawnStore[guid] = linkedGuid;
    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_REP_CREATURE_LINKED_RESPAWN);
    stmt->setUInt32(0, guidLow);
    stmt->setUInt32(1, linkedGuidLow);
    WorldDatabase.Execute(stmt);
    return true;
}

void ObjectMgr::LoadTempSummons()
{
    uint32 oldMSTime = getMSTime();

    _tempSummonDataStore.clear();   ///< needed for reload case

    //                                               0           1             2        3      4           5           6            7            8           9         10        11          12
    QueryResult result = WorldDatabase.Query("SELECT summonerId, summonerType, groupId, entry, position_x, position_y, position_z, orientation, count, actionType, distance, summonType, summonTime FROM creature_summon_groups");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 temp summons. DB table `creature_summon_groups` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint8 l_Index = 0;

        uint32 summonerId               = fields[l_Index++].GetUInt32();
        SummonerType summonerType       = SummonerType(fields[l_Index++].GetUInt8());
        uint8 group                     = fields[l_Index++].GetUInt8();

        switch (summonerType)
        {
            case SUMMONER_TYPE_CREATURE:
                if (!GetCreatureTemplate(summonerId))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `creature_summon_groups` has summoner with non existing entry %u for creature summoner type, skipped.", summonerId);
                    continue;
                }
                break;
            case SUMMONER_TYPE_GAMEOBJECT:
                if (!GetGameObjectTemplate(summonerId))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `creature_summon_groups` has summoner with non existing entry %u for gameobject summoner type, skipped.", summonerId);
                    continue;
                }
                break;
            case SUMMONER_TYPE_MAP:
                if (!sMapStore.LookupEntry(summonerId))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `creature_summon_groups` has summoner with non existing entry %u for map summoner type, skipped.", summonerId);
                    continue;
                }
                break;
            default:
                sLog->outError(LOG_FILTER_SQL, "Table `creature_summon_groups` has unhandled summoner type %u for summoner %u, skipped.", summonerType, summonerId);
                continue;
        }

        TempSummonData data;
        data.entry                      = fields[l_Index++].GetUInt32();

        if (!GetCreatureTemplate(data.entry))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature_summon_groups` has creature in group [Summoner ID: %u, Summoner Type: %u, Group ID: %u] with non existing creature entry %u, skipped.", summonerId, summonerType, group, data.entry);
            continue;
        }

        float posX                      = fields[l_Index++].GetFloat();
        float posY                      = fields[l_Index++].GetFloat();
        float posZ                      = fields[l_Index++].GetFloat();
        float orientation               = fields[l_Index++].GetFloat();

        data.pos.Relocate(posX, posY, posZ, orientation);

        data.count                      = fields[l_Index++].GetUInt8();
        data.actionType                 = fields[l_Index++].GetUInt8();
        data.distance                   = fields[l_Index++].GetFloat();
        data.type                       = TempSummonType(fields[l_Index++].GetUInt8());

        if (data.type > TEMPSUMMON_MANUAL_DESPAWN)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature_summon_groups` has unhandled temp summon type %u in group [Summoner ID: %u, Summoner Type: %u, Group ID: %u] for creature entry %u, skipped.", data.type, summonerId, summonerType, group, data.entry);
            continue;
        }

        data.time                       = fields[l_Index++].GetUInt32();

        TempSummonGroupKey key(summonerId, summonerType, group);
        _tempSummonDataStore[key].push_back(data);

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u temp summons in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadConversations()
{
    uint32 l_OldMStime = getMSTime();

    //                                                  0    1   2    3       4       5           6           7           8            9          10         11
    QueryResult l_Result = WorldDatabase.Query("SELECT guid, id, map, zoneId, areaId, position_x, position_y, position_z, orientation, spawnMask, phaseMask, PhaseId "
                                               "FROM conversation ORDER BY `map` ASC, `guid` ASC");

    if (!l_Result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creatures. DB table `conversation` is empty.");
        return;
    }

    /// Build single time for check spawnmask
    std::map<uint32, uint32> l_SpawnMasks;
    for (auto& l_MapDifficultyPair : _mapDifficulties)
    {
        for (auto& _DifficultyPair : l_MapDifficultyPair.second)
            l_SpawnMasks[l_MapDifficultyPair.first] |= (1 << _DifficultyPair.first);
    }

    m_ConversationSpawnDataStore.rehash(l_Result->GetRowCount());
    std::map<uint32, ConversationSpawnData*> l_LastConversationSpawns;

    uint32 l_LoadedCount = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint8 l_ColIndex = 0;

        uint32 l_Guid = l_Fields[l_ColIndex++].GetUInt32();
        uint32 l_Entry = l_Fields[l_ColIndex++].GetUInt32();

        std::vector<ConversationData>       const* l_ConversationDatas     = sObjectMgr->GetConversationData(l_Entry);
        std::vector<ConversationCreature>   const* l_ConversationCreatures = sObjectMgr->GetConversationCreature(l_Entry);
        std::vector<ConversationActor>      const* l_ConversationActors    = sObjectMgr->GetConversationActor(l_Entry);

        bool l_HasActorData     = l_ConversationActors      && !l_ConversationActors->empty();
        bool l_HasCreatureData  = l_ConversationCreatures   && !l_ConversationCreatures->empty();
        bool l_HasData          = l_ConversationDatas       && !l_ConversationDatas->empty();

        if (!l_HasData || (!l_HasActorData && !l_HasCreatureData))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `conversation` has conversation (GUID: %u) with non existing conversation data %u, skipped.", l_Guid, l_Entry);
            continue;
        }

        ConversationSpawnData& l_SpawnData = m_ConversationSpawnDataStore[l_Guid];
        l_SpawnData.GUID                = l_Guid;
        l_SpawnData.ConversationEntry   = l_Entry;
        l_SpawnData.MapID               = l_Fields[l_ColIndex++].GetUInt16();
        l_SpawnData.ZoneID              = l_Fields[l_ColIndex++].GetUInt16();
        l_SpawnData.AreaID              = l_Fields[l_ColIndex++].GetUInt16();
        l_SpawnData.PositionX           = l_Fields[l_ColIndex++].GetFloat();
        l_SpawnData.PositionY           = l_Fields[l_ColIndex++].GetFloat();
        l_SpawnData.PositionZ           = l_Fields[l_ColIndex++].GetFloat();
        l_SpawnData.Orientation         = l_Fields[l_ColIndex++].GetFloat();
        l_SpawnData.SpawnMask           = l_Fields[l_ColIndex++].GetUInt32();
        l_SpawnData.PhaseMask           = l_Fields[l_ColIndex++].GetUInt32();

        Tokenizer l_PhasesToken(l_Fields[l_ColIndex++].GetString(), ' ', 100);
        for (Tokenizer::const_iterator::value_type itr : l_PhasesToken)
        {
            if (PhaseEntry const* l_PhaseEntry = sPhaseStore.LookupEntry(uint32(strtoull(itr, nullptr, 10))))
                l_SpawnData.PhaseID.insert(l_PhaseEntry->ID);
        }

        /// Check near npc with same entry.
        auto l_LastConversationSpawnsIt = l_LastConversationSpawns.find(l_Entry);
        if (l_LastConversationSpawnsIt != l_LastConversationSpawns.end())
        {
            if (l_SpawnData.MapID == l_LastConversationSpawnsIt->second->MapID)
            {
                float l_XDelta = l_LastConversationSpawnsIt->second->PositionX - l_SpawnData.PositionX;
                float l_YDelta = l_LastConversationSpawnsIt->second->PositionY - l_SpawnData.PositionY;
                float l_ZDelta = l_LastConversationSpawnsIt->second->PositionZ - l_SpawnData.PositionZ;

                float l_SquaredDist = l_XDelta*l_XDelta + l_YDelta*l_YDelta + l_ZDelta*l_ZDelta;

                if (l_SquaredDist < 0.5f)
                {
                    /// Split phaseID
                    for (auto l_CurrentPhaseID : l_SpawnData.PhaseID)
                        l_LastConversationSpawnsIt->second->PhaseID.insert(l_CurrentPhaseID);

                    l_LastConversationSpawnsIt->second->PhaseMask |= l_SpawnData.PhaseMask;
                    l_LastConversationSpawnsIt->second->SpawnMask |= l_SpawnData.SpawnMask;

                    WorldDatabase.PExecute("UPDATE conversation SET phaseMask = %u, spawnMask = %u WHERE guid = %u", l_LastConversationSpawnsIt->second->PhaseMask, l_LastConversationSpawnsIt->second->SpawnMask, l_LastConversationSpawnsIt->second->GUID);
                    WorldDatabase.PExecute("DELETE FROM conversation WHERE guid = %u", l_Guid);

                    sLog->outError(LOG_FILTER_SQL, "Table `conversation` have clone npc %u witch stay too close (dist: %f). original npc guid %lu. npc with guid %u will be deleted.", l_Entry, l_SquaredDist, l_LastConversationSpawnsIt->second->GUID, l_Guid);
                    continue;
                }
            }
            else
                l_LastConversationSpawns[l_Entry] = &l_SpawnData;
        }
        else
            l_LastConversationSpawns[l_Entry] = &l_SpawnData;

        MapEntry const* l_MapEntry = sMapStore.LookupEntry(l_SpawnData.MapID);
        if (!l_MapEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `conversation` have conversation (GUID: %u) that spawned at not existed map (Id: %u), skipped.", l_Guid, l_SpawnData.MapID);
            continue;
        }

        if (l_SpawnData.SpawnMask & ~l_SpawnMasks[l_SpawnData.MapID])
        {
            sLog->outError(LOG_FILTER_SQL, "Table `conversation` have conversation (GUID: %u) that have wrong spawn mask %u including not supported difficulty modes for map (Id: %u) spawnMasks[data.mapid]: %u.", l_Guid, l_SpawnData.SpawnMask, l_SpawnData.MapID, l_SpawnMasks[l_SpawnData.MapID]);
            WorldDatabase.PExecute("UPDATE conversation SET spawnMask = %u WHERE guid = %u", l_SpawnMasks[l_SpawnData.MapID], l_Guid);
        }

        if (l_SpawnData.PhaseMask == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `conversation` have conversation (GUID: %u Entry: %u) with `phaseMask`=0 (not visible for anyone), set to 1.", l_Guid, l_SpawnData.ConversationEntry);
            l_SpawnData.PhaseMask = 1;
        }

        /// Add to grid if not managed by the game event or pool system
        AddConversationToGrid(l_Guid, &l_SpawnData);

        if (!l_SpawnData.ZoneID || !l_SpawnData.AreaID)
        {
            uint32 l_ComputedZoneID = 0;
            uint32 l_ComputedAreaID = 0;

            sMapMgr->GetZoneAndAreaId(l_ComputedZoneID, l_ComputedAreaID, l_SpawnData.MapID, l_SpawnData.PositionX, l_SpawnData.PositionY, l_SpawnData.PositionZ);
            WorldDatabase.PExecute("UPDATE conversation SET zoneId = %u, areaId = %u WHERE guid = %u", l_ComputedZoneID, l_ComputedAreaID, l_Guid);
        }

        ++l_LoadedCount;
    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u conversation in %u ms", l_LoadedCount, GetMSTimeDiffToNow(l_OldMStime));
}

void ObjectMgr::LoadCreatures()
{
    uint32 oldMSTime = getMSTime();

    //                                      0        1       2   3       4      5       6          7            8           9            10            11            12          13
    std::string l_Query = "SELECT creature.guid, linked_id, id, map, zoneId, areaId, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, "
        //        14            15         16       17            18         19      20          21            22                23           24                  25                     26                     27                      28                   29             30
        "currentwaypoint, curhealth, curmana, MovementType, spawnMask, phaseMask, phaseID, eventEntry, pool_entry, creature.npcflag, creature.npcflag2, creature.unit_flags, creature.unit_flags2,  creature.unit_flags3, creature.dynamicflags, creature.WorldEffectID, creature.isActive, creature.ScriptName, creature.NameTag "
        "FROM creature "
        "LEFT OUTER JOIN game_event_creature ON creature.guid = game_event_creature.guid "
        "LEFT OUTER JOIN pool_creature ON creature.guid = pool_creature.guid";

    if (sWorld->getBoolConfig(CONFIG_ENABLE_ONLY_SPECIFIC_MAP))
    {
        std::vector<uint32> l_MapsToLoad = sWorld->GetMapsToLoad();
        std::string l_TempQueryEnding = " WHERE creature.map IN (";
        uint8 l_Counter = 0;

        for (uint32 l_MapID : l_MapsToLoad)
        {
            l_TempQueryEnding += std::to_string(l_MapID);
            ++l_Counter;

            if (l_Counter < l_MapsToLoad.size())
                l_TempQueryEnding += ", ";
            else
                l_TempQueryEnding += ")";
        }

        l_Query += l_TempQueryEnding;
    }

    QueryResult result = WorldDatabase.Query(l_Query.c_str());

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creatures. DB table `creature` is empty.");

        return;
    }

    // Build single time for check spawnmask
    std::map<uint32, uint32> spawnMasks;
    for (auto& mapDifficultyPair : _mapDifficulties)
        for (auto& difficultyPair : mapDifficultyPair.second)
            spawnMasks[mapDifficultyPair.first] |= (1 << difficultyPair.first);

    //_creatureDataStore.rehash(result->GetRowCount());
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint8 index = 0;

        uint32 guid              = fields[index++].GetUInt32();
        std::string linked_id    = fields[index++].GetString();
        uint32 entry             = fields[index++].GetUInt32();

        CreatureTemplate const* cInfo = GetCreatureTemplate(entry);
        if (!cInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature` has creature (GUID: %u) with non existing creature entry %u, skipped.", guid, entry);
            continue;
        }

        m_LinkedIdXGuid[linked_id] = guid;
        CreatureData& data = _creatureDataStore[guid];
        data.linkedId       = linked_id;
        data.id             = entry;
        data.mapid          = fields[index++].GetUInt16();
        data.zoneId         = fields[index++].GetUInt16();
        data.areaId         = fields[index++].GetUInt16();
        data.displayid      = fields[index++].GetUInt32();
        data.equipmentId    = fields[index++].GetInt8();
        data.posX           = fields[index++].GetFloat();
        data.posY           = fields[index++].GetFloat();
        data.posZ           = fields[index++].GetFloat();
        data.orientation    = fields[index++].GetFloat();
        data.spawntimesecs  = fields[index++].GetUInt32();
        data.spawndist      = fields[index++].GetFloat();
        data.currentwaypoint= fields[index++].GetUInt32();
        data.curhealth      = fields[index++].GetUInt64();
        data.curmana        = fields[index++].GetUInt32();
        data.movementType   = fields[index++].GetUInt8();
        data.spawnMask      = fields[index++].GetUInt32();
        data.phaseMask      = fields[index++].GetUInt32();
        data.phaseID        = fields[index++].GetUInt32();
        int16 gameEvent     = fields[index++].GetInt8();
        uint32 PoolId       = fields[index++].GetUInt32();
        data.NpcFlags1      = fields[index++].GetUInt32();
        data.NpcFlags2      = fields[index++].GetUInt32();
        data.UnitFlags1     = fields[index++].GetUInt32();
        data.UnitFlags2     = fields[index++].GetUInt32();
        data.UnitFlags3     = fields[index++].GetUInt32();
        data.dynamicflags   = fields[index++].GetUInt32();
        data.WorldEffectID  = fields[index++].GetUInt32();
        data.isActive       = fields[index++].GetBool();
        data.ScriptId       = GetScriptId(fields[index++].GetCString());
        data.NameTag        = fields[index++].GetString();

        if (!data.ScriptId)
            data.ScriptId = cInfo->ScriptID;

        MapEntry const* mapEntry = sMapStore.LookupEntry(data.mapid);
        if (!mapEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (GUID: %u) that spawned at not existed map (Id: %u), skipped.", guid, data.mapid);
            continue;
        }

        if (data.spawnMask & ~spawnMasks[data.mapid])
            sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (GUID: %u) that have wrong spawn mask %u including not supported difficulty modes for map (Id: %u) spawnMasks[data.mapid]: %u.", guid, data.spawnMask, data.mapid, spawnMasks[data.mapid]);

        if (_creaturesDiffEntry.find(data.id) != _creaturesDiffEntry.end())
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (GUID: %u) that listed as creature_template_difficulty (entry: %u) in `creature_template`, skipped.",
                guid, data.id);
            continue;
        }

        // -1 random, 0 no equipment,
        if (data.equipmentId != 0)
        {
            if (!GetEquipmentInfo(data.id, data.equipmentId))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (Entry: %u) with equipment_id %u not found in table `creature_equip_template`, set to no equipment.", data.id, data.equipmentId);
                data.equipmentId = 0;
            }
        }

        if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_INSTANCE_BIND)
        {
            if (!mapEntry || !mapEntry->IsDungeon())
                sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (GUID: %u Entry: %u) with `creature_template`.`flags_extra` including CREATURE_FLAG_EXTRA_INSTANCE_BIND but creature are not in instance.", guid, data.id);
        }

        if (data.spawndist < 0.0f)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (GUID: %u Entry: %u) with `spawndist`< 0, set to 0.", guid, data.id);
            data.spawndist = 0.0f;
        }
        else if (data.movementType == RANDOM_MOTION_TYPE)
        {
            if (data.spawndist == 0.0f)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (GUID: %u Entry: %u) with `MovementType`=1 (random movement) but with `spawndist`=0, replace by idle movement type (0).", guid, data.id);
                data.movementType = IDLE_MOTION_TYPE;
            }
        }
        else if (data.movementType == IDLE_MOTION_TYPE)
        {
            if (data.spawndist != 0.0f)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (GUID: %u Entry: %u) with `MovementType`=0 (idle) have `spawndist`<>0, set to 0.", guid, data.id);
                data.spawndist = 0.0f;
            }
        }

        if (data.phaseMask == 0 && data.phaseID == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature` have creature (GUID: %u Entry: %u) with `phaseMask`=0 AND `phaseID` =0 (not visible for anyone), set to 1.", guid, data.id);
            data.phaseMask = 1;
        }

        // Add to grid if not managed by the game event or pool system
        if (gameEvent == 0 && PoolId == 0)
            AddCreatureToGrid(guid, &data);

        if ((!data.zoneId || !data.areaId) && !sWorld->getBoolConfig(CONFIG_IS_TEST_SERVER))
        {
            uint32 zoneId = 0;
            uint32 areaId = 0;

            sMapMgr->GetZoneAndAreaId(zoneId, areaId, data.mapid, data.posX, data.posY, data.posZ);
            if (zoneId || areaId)
                WorldDatabase.PExecute("UPDATE creature SET zoneId = %u, areaId = %u WHERE guid = %u", zoneId, areaId, guid);
        }

        /// Container only needed to load some World Quest Positions
        if (data.mapid == 1220)
            m_CreatureIdXAreaId.insert(std::multimap<uint32, uint32>::value_type(data.id, data.areaId));

        count++;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creatures in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::AddConversationToGrid(uint32 guid, ConversationSpawnData const* data)
{
    uint32 mask = data->SpawnMask;
    for (uint32 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = JadeCore::ComputeCellCoord(data->PositionX, data->PositionY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->MapID, i)][cellCoord.GetId()];
            cell_guids.conversation.insert(guid);
        }
    }
}

void ObjectMgr::AddCreatureToGrid(uint32 guid, CreatureData const* data)
{
    uint32 mask = data->spawnMask;
    for (uint32 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = JadeCore::ComputeCellCoord(data->posX, data->posY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->mapid, i)][cellCoord.GetId()];
            cell_guids.creatures.insert(guid);
        }
    }
}

void ObjectMgr::RemoveCreatureFromGrid(uint32 guid, CreatureData const* data)
{
    uint32 mask = data->spawnMask;
    for (uint32 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = JadeCore::ComputeCellCoord(data->posX, data->posY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->mapid, i)][cellCoord.GetId()];
            cell_guids.creatures.erase(guid);
        }
    }
}

bool ObjectMgr::AddGOData(uint32 p_LowGuid, uint32 entry, uint32 mapId, float x, float y, float z, float o, uint32 spawntimedelay, float rotation0, float rotation1, float rotation2, float rotation3)
{
    GameObjectTemplate const* goinfo = GetGameObjectTemplate(entry);
    if (!goinfo)
        return false;

    uint32 l_Zone = sMapMgr->GetZoneId(mapId, x, y, z);

    /// Specific Outdoor case (Ashran/Dalaran)
    if (mapId == 1502 || mapId == 1191)
        l_Zone = 0;

    std::list<Map*> maps = sMapMgr->GetMapsPerZoneId(mapId, l_Zone);
    if (maps.empty())
        return false;

    for (Map* l_Map : maps)
    {
        GameObjectData& data = NewGOData(p_LowGuid);
        data.id = entry;
        data.mapid = mapId;
        data.posX = x;
        data.posY = y;
        data.posZ = z;
        data.orientation = o;
        data.rotation0 = rotation0;
        data.rotation1 = rotation1;
        data.rotation2 = rotation2;
        data.rotation3 = rotation3;
        data.spawntimesecs = spawntimedelay;
        data.animprogress = 100;
        data.spawnMask = 1;
        data.go_state = GO_STATE_READY;
        data.phaseMask = PHASEMASK_NORMAL;
        data.artKit = goinfo->type == GAMEOBJECT_TYPE_CONTROL_ZONE ? 21 : 0;
        data.dbData = false;

        AddGameobjectToGrid(p_LowGuid, &data);

        // Spawn if necessary (loaded grids only)
        // We use spawn coords to spawn
        if (!l_Map->Instanceable() && l_Map->IsGridLoaded(x, y))
        {
            GameObject* go = new GameObject;
            if (!go->LoadGameObjectFromDB(p_LowGuid, l_Map, true, false))
            {
                sLog->outError(LOG_FILTER_GENERAL, "AddGOData: cannot add gameobject entry %u to map", entry);
                go->CleanBeforeGC();
                sGarbageCollector->Add(go);
                return false;
            }
        }

        sLog->outDebug(LOG_FILTER_MAPS, "AddGOData: dbguid %u entry %u map %u x %f y %f z %f o %f", p_LowGuid, entry, mapId, x, y, z, o);
    }

    return true;
}

uint32 ObjectMgr::AddCreData(uint32 entry, uint32 /*team*/, uint32 mapId, float x, float y, float z, float o, uint32 spawntimedelay)
{
    CreatureTemplate const* l_CreatureTemplate = GetCreatureTemplate(entry);
    if (!l_CreatureTemplate)
        return 0;

    uint32 level = l_CreatureTemplate->minlevel == l_CreatureTemplate->maxlevel ? l_CreatureTemplate->minlevel : urand(l_CreatureTemplate->minlevel, l_CreatureTemplate->maxlevel); // Only used for extracting creature base stats
    CreatureBaseStats const* stats = GetCreatureBaseStats(level, l_CreatureTemplate->unit_class);

    uint32 guid = GenerateLowGuid(HIGHGUID_UNIT);

    CreatureData& l_Data = NewOrExistCreatureData(guid);

    l_Data.id = entry;
    l_Data.mapid = mapId;
    l_Data.displayid = 0;

    if (_equipmentInfoStore.find(entry) != _equipmentInfoStore.end())
        l_Data.equipmentId = 1; // Assuming equipmentId is 1
    else
        l_Data.equipmentId = 0;

    l_Data.posX           = x;
    l_Data.posY           = y;
    l_Data.posZ           = z;
    l_Data.orientation    = o;
    l_Data.spawntimesecs  = spawntimedelay;
    l_Data.spawndist      = 0;
    l_Data.currentwaypoint= 0;
    l_Data.curhealth      = stats->GenerateHealth(l_CreatureTemplate);
    l_Data.curmana        = stats->GenerateMana(l_CreatureTemplate);
    l_Data.movementType   = l_CreatureTemplate->MovementType;
    l_Data.spawnMask      = 1;
    l_Data.phaseMask      = PHASEMASK_NORMAL;
    l_Data.phaseID        = 0;
    l_Data.dbData         = false;
    l_Data.NpcFlags1      = l_CreatureTemplate->NpcFlags1;
    l_Data.NpcFlags2      = l_CreatureTemplate->NpcFlags2;
    l_Data.UnitFlags1     = l_CreatureTemplate->UnitFlags1;
    l_Data.UnitFlags2     = l_CreatureTemplate->UnitFlags2;
    l_Data.UnitFlags3     = l_CreatureTemplate->UnitFlags3;
    l_Data.dynamicflags   = l_CreatureTemplate->dynamicflags;
    l_Data.WorldEffectID  = l_CreatureTemplate->WorldEffectID;
    l_Data.linkedId       = "";

    AddCreatureToGrid(guid, &l_Data);

    // Spawn if necessary (loaded grids only)
    uint32 l_Zone = sMapMgr->GetZoneId(mapId, l_Data.posX, l_Data.posY, l_Data.posZ);
    if (Map* map = sMapMgr->CreateBaseMap(mapId, l_Zone))
    {
        // We use spawn coords to spawn
        if (!map->Instanceable() && !map->IsRemovalGrid(x, y))
        {
            Creature* creature = new Creature;
            if (!creature->LoadCreatureFromDB(guid, map, true, false))
            {
                sLog->outError(LOG_FILTER_GENERAL, "AddCreature: cannot add creature entry %u to map", entry);
                creature->CleanBeforeGC();
                sGarbageCollector->Add(creature);
                return 0;
            }

            l_Data.linkedId = creature->ComputeLinkedId();
        }
    }

    return guid;
}

void ObjectMgr::LoadGameobjects()
{
    uint32 oldMSTime = getMSTime();

    uint32 count = 0;

    //                                                0                1   2    3         4           5           6        7           8
    std::string l_Query = "SELECT gameobject.guid, id, map, zoneId, areaId, position_x, position_y, position_z, orientation, "
        //      9          10         11          12         13          14             15      16         17           18         19        20          21      22         23
        "rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state, isActive, custom_flags, spawnMask, phaseMask, eventEntry, pool_entry, phaseID, ScriptName "
        "FROM gameobject LEFT OUTER JOIN game_event_gameobject ON gameobject.guid = game_event_gameobject.guid "
        "LEFT OUTER JOIN pool_gameobject ON gameobject.guid = pool_gameobject.guid";

    if (sWorld->getBoolConfig(CONFIG_ENABLE_ONLY_SPECIFIC_MAP))
    {
        std::vector<uint32> l_MapsToLoad = sWorld->GetMapsToLoad();
        std::string l_TempQueryEnding = " WHERE gameobject.map IN (";
        uint8 l_Counter = 0;

        for (uint32 l_MapID : l_MapsToLoad)
        {
            l_TempQueryEnding += std::to_string(l_MapID);
            ++l_Counter;

            if (l_Counter < l_MapsToLoad.size())
                l_TempQueryEnding += ", ";
            else
                l_TempQueryEnding += ")";
        }

        l_Query += l_TempQueryEnding;
    }

    QueryResult result = WorldDatabase.Query(l_Query.c_str());

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 gameobjects. DB table `gameobject` is empty.");
        return;
    }

    // build single time for check spawnmask
    std::map<uint32, uint32> spawnMasks;
    for (auto& mapDifficultyPair : _mapDifficulties)
        for (auto& difficultyPair : mapDifficultyPair.second)
            spawnMasks[mapDifficultyPair.first] |= (1 << difficultyPair.first);

    //_gameObjectDataStore.rehash(result->GetRowCount());
    do
    {
        Field* fields = result->Fetch();

        uint32 guid         = fields[0].GetUInt32();
        uint32 entry        = fields[1].GetUInt32();

        GameObjectTemplate const* gInfo = GetGameObjectTemplate(entry);
        if (!gInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u) with non existing gameobject entry %u, skipped.", guid, entry);
            continue;
        }

        if (!gInfo->displayId)
        {
            switch (gInfo->type)
            {
                case GAMEOBJECT_TYPE_TRAP:
                case GAMEOBJECT_TYPE_SPELL_FOCUS:
                    break;
                default:
                    sLog->outError(LOG_FILTER_SQL, "Gameobject (GUID: %u Entry %u GoType: %u) doesn't have a displayId (%u), not loaded.", guid, entry, gInfo->type, gInfo->displayId);
                    break;
            }
        }

        if (gInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(gInfo->displayId))
        {
            sLog->outError(LOG_FILTER_SQL, "Gameobject (GUID: %u Entry %u GoType: %u) has an invalid displayId (%u), not loaded.", guid, entry, gInfo->type, gInfo->displayId);
            continue;
        }

        GameObjectData& data = _gameObjectDataStore[guid];

        data.id             = entry;
        data.mapid          = fields[2].GetUInt16();
        data.zoneId         = fields[3].GetUInt16();
        data.areaId         = fields[4].GetUInt16();
        data.posX           = fields[5].GetFloat();
        data.posY           = fields[6].GetFloat();
        data.posZ           = fields[7].GetFloat();
        data.orientation    = fields[8].GetFloat();
        data.rotation0      = fields[9].GetFloat();
        data.rotation1      = fields[10].GetFloat();
        data.rotation2      = fields[11].GetFloat();
        data.rotation3      = fields[12].GetFloat();
        data.spawntimesecs  = fields[13].GetInt32();

        MapEntry const* mapEntry = sMapStore.LookupEntry(data.mapid);
        if (!mapEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u Entry: %u) spawned on a non-existed map (Id: %u), skip", guid, data.id, data.mapid);
            continue;
        }

        if (!data.zoneId || !data.areaId)
        {
            uint32 zoneId = 0;
            uint32 areaId = 0;

            sMapMgr->GetZoneAndAreaId(zoneId, areaId, data.mapid, data.posX, data.posY, data.posZ);
            if (zoneId || areaId)
                WorldDatabase.PExecute("UPDATE gameobject SET zoneId = %u, areaId = %u WHERE guid = %u", zoneId, areaId, guid);
        }

        if (data.spawntimesecs == 0 && gInfo->IsDespawnAtAction())
        {
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u Entry: %u) with `spawntimesecs` (0) value, but the gameobejct is marked as despawnable at action.", guid, data.id);
        }

        data.animprogress   = fields[14].GetUInt8();
        data.artKit         = 0;

        uint32 go_state     = fields[15].GetUInt8();
        if (go_state >= MAX_GO_STATE)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u Entry: %u) with invalid `state` (%u) value, skip", guid, data.id, go_state);
            continue;
        }

        data.go_state       = GOState(go_state);

        data.isActive       = fields[16].GetBool();
        data.CustomFlags    = fields[17].GetUInt32();
        data.spawnMask      = fields[18].GetUInt32();

        if (data.spawnMask & ~spawnMasks[data.mapid])
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u Entry: %u) that has wrong spawn mask %u including not supported difficulty modes for map (Id: %u), skip", guid, data.id, data.spawnMask, data.mapid);

        data.phaseMask      = fields[19].GetUInt32();
        int16 gameEvent     = fields[20].GetInt8();
        uint32 PoolId       = fields[21].GetUInt32();
        data.PhaseID        = fields[22].GetUInt32();
        data.ScriptId       = GetScriptId(fields[23].GetCString());
        if (!data.ScriptId)
            data.ScriptId = gInfo->ScriptId;

        if (data.rotation2 < -M_PI || data.rotation2 > M_PI)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u Entry: %u) with invalid rotation2 (%f) value, skip", guid, data.id, data.rotation2);
            continue;
        }

        if (data.rotation3 < -M_PI || data.rotation3 > M_PI)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u Entry: %u) with invalid rotation3 (%f) value, skip", guid, data.id, data.rotation3);
            continue;
        }

        if (!MapManager::IsValidMapCoord(data.mapid, data.posX, data.posY, data.posZ, data.orientation))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u Entry: %u) with invalid coordinates, skip", guid, data.id);
            continue;
        }

        if (data.phaseMask == 0 && data.PhaseID == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject` has gameobject (GUID: %u Entry: %u) with `phaseMask`=0 (not visible for anyone), set to 1.", guid, data.id);
            data.phaseMask = 1;
        }

        if (gameEvent == 0 && PoolId == 0)                      // if not this is to be managed by GameEvent System or Pool system
            AddGameobjectToGrid(guid, &data);

        /// Container only needed to load some World Quest Positions
        if (data.mapid == 1220)
            m_GameObjectIdXAreaId.insert(std::multimap<uint32, uint32>::value_type(data.id, data.areaId));

        if (data.CustomFlags & CustomFlagBig)
            sMapMgr->RegisterBigObjectPositionForMap(data.mapid, std::make_pair(data.posX, data.posY));

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu gameobjects in %u ms", (unsigned long)_gameObjectDataStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::AddGameobjectToGrid(uint32 guid, GameObjectData const* data)
{
    uint32 mask = data->spawnMask;
    for (uint32 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = JadeCore::ComputeCellCoord(data->posX, data->posY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->mapid, i)][cellCoord.GetId()];
            cell_guids.gameobjects.insert(guid);
        }
    }
}

void ObjectMgr::RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data)
{
    uint32 mask = data->spawnMask;
    for (uint32 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellCoord cellCoord = JadeCore::ComputeCellCoord(data->posX, data->posY);
            CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(data->mapid, i)][cellCoord.GetId()];
            cell_guids.gameobjects.erase(guid);
        }
    }
}

Player* ObjectMgr::GetPlayerByLowGUID(uint32 lowguid) const
{
    uint64 guid = MAKE_NEW_GUID(lowguid, 0, HIGHGUID_PLAYER);
    return ObjectAccessor::FindPlayer(guid);
}

#ifndef CROSS
// name must be checked to correctness (if received) before call this function
uint64 ObjectMgr::GetPlayerGUIDByName(std::string name) const
{
    return sWorld->GetCharacterGuidByName(name);
}

#endif /* not CROSS */
bool ObjectMgr::GetPlayerNameByGUID(uint64 guid, std::string &name) const
{
    // prevent DB access for online player
    if (Player* player = ObjectAccessor::FindPlayer(guid))
    {
        name = player->GetName();
        return true;
    }

#ifndef CROSS
    if (CharacterInfo const* l_CharacterInfo = sWorld->GetCharacterInfo(guid))
    {
        name = l_CharacterInfo->Name;
        return true;
    }

#endif /* not CROSS */
    return false;
}

#ifndef CROSS
uint32 ObjectMgr::GetPlayerTeamByGUID(uint64 guid) const
{
    if (CharacterInfo const* l_CharacterInfo = sWorld->GetCharacterInfo(guid))
        return Player::TeamForRace(l_CharacterInfo->Race);

    return 0;
}

#endif /* not CROSS */
uint32 ObjectMgr::GetPlayerAccountIdByGUID(uint64 guid) const
{
#ifndef CROSS
    if (CharacterInfo const* l_CharacterInfo = sWorld->GetCharacterInfo(guid))
        return l_CharacterInfo->AccountId;

    return 0;
}

uint32 ObjectMgr::GetPlayerAccountIdByPlayerName(const std::string& name) const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_BY_NAME);

    stmt->setString(0, name);
    stmt->setString(1, name);

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
#else /* CROSS */
    // prevent DB access for online player
    if (Player* player = ObjectAccessor::FindPlayer(guid))
#endif /* CROSS */
    {
#ifndef CROSS
        uint32 acc = (*result)[0].GetUInt32();
        return acc;
#else /* CROSS */
        return player->GetSession()->GetAccountId();
#endif /* CROSS */
    }

    return 0;
}

uint32 FillItemArmor(uint32 itemlevel, uint32 itemClass, uint32 itemSubclass, uint32 quality, uint32 inventoryType)
{
    if (quality > ITEM_QUALITY_ARTIFACT)
        return 0;

    if (itemClass != ITEM_CLASS_ARMOR)
        return 0;

    // all items but shields
    if (itemSubclass != ITEM_SUBCLASS_ARMOR_SHIELD)
    {
        ItemArmorQualityEntry const* armorQuality = sItemArmorQualityStore.LookupEntry(itemlevel);
        ItemArmorTotalEntry const* armorTotal = sItemArmorTotalStore.LookupEntry(itemlevel);
        if (!armorQuality || !armorTotal)
            return 0;

        if (inventoryType == INVTYPE_ROBE)
            inventoryType = INVTYPE_CHEST;

        ArmorLocationEntry const* location = sArmorLocationStore.LookupEntry(inventoryType);
        if (!location)
            return 0;

        if (itemSubclass < ITEM_SUBCLASS_ARMOR_CLOTH)
            return 0;

        return uint32(armorQuality->Value[quality] * armorTotal->Value[itemSubclass - 1] * location->Value[itemSubclass - 1] + 0.5f);
    }

    // shields
    ItemArmorShieldEntry const* shield = sItemArmorShieldStore.LookupEntry(itemlevel);
    if (!shield)
        return 0;

    return uint32(shield->Value[quality] + 0.5f);
}

uint32 FillMaxDurability(uint32 itemClass, uint32 itemSubClass, uint32 inventoryType, uint32 quality, uint32 itemLevel)
{
    if (itemClass != ITEM_CLASS_ARMOR && itemClass != ITEM_CLASS_WEAPON)
        return 0;

    static float const qualityMultipliers[MAX_ITEM_QUALITY] =
    {
        0.92f, 0.92f, 0.92f, 1.11f, 1.32f, 1.61f, 0.0f, 0.0f
    };

    static float const armorMultipliers[MAX_INVTYPE] =
    {
        0.00f, // INVTYPE_NON_EQUIP
        0.60f, // INVTYPE_HEAD
        0.00f, // INVTYPE_NECK
        0.60f, // INVTYPE_SHOULDERS
        0.00f, // INVTYPE_BODY
        1.00f, // INVTYPE_CHEST
        0.33f, // INVTYPE_WAIST
        0.72f, // INVTYPE_LEGS
        0.48f, // INVTYPE_FEET
        0.33f, // INVTYPE_WRISTS
        0.33f, // INVTYPE_HANDS
        0.00f, // INVTYPE_FINGER
        0.00f, // INVTYPE_TRINKET
        0.00f, // INVTYPE_WEAPON
        0.72f, // INVTYPE_SHIELD
        0.00f, // INVTYPE_RANGED
        0.00f, // INVTYPE_CLOAK
        0.00f, // INVTYPE_2HWEAPON
        0.00f, // INVTYPE_BAG
        0.00f, // INVTYPE_TABARD
        1.00f, // INVTYPE_ROBE
        0.00f, // INVTYPE_WEAPONMAINHAND
        0.00f, // INVTYPE_WEAPONOFFHAND
        0.00f, // INVTYPE_HOLDABLE
        0.00f, // INVTYPE_AMMO
        0.00f, // INVTYPE_THROWN
        0.00f, // INVTYPE_RANGEDRIGHT
        0.00f, // INVTYPE_QUIVER
        0.00f, // INVTYPE_RELIC
    };

    static float const weaponMultipliers[MAX_ITEM_SUBCLASS_WEAPON] =
    {
        0.91f, // ITEM_SUBCLASS_WEAPON_AXE
        1.00f, // ITEM_SUBCLASS_WEAPON_AXE2
        1.00f, // ITEM_SUBCLASS_WEAPON_BOW
        1.00f, // ITEM_SUBCLASS_WEAPON_GUN
        0.91f, // ITEM_SUBCLASS_WEAPON_MACE
        1.00f, // ITEM_SUBCLASS_WEAPON_MACE2
        1.00f, // ITEM_SUBCLASS_WEAPON_POLEARM
        0.91f, // ITEM_SUBCLASS_WEAPON_SWORD
        1.00f, // ITEM_SUBCLASS_WEAPON_SWORD2
        1.00f, // ITEM_SUBCLASS_WEAPON_WARGLAIVES
        1.00f, // ITEM_SUBCLASS_WEAPON_STAFF
        0.00f, // ITEM_SUBCLASS_WEAPON_EXOTIC
        0.00f, // ITEM_SUBCLASS_WEAPON_EXOTIC2
        0.66f, // ITEM_SUBCLASS_WEAPON_FIST_WEAPON
        0.00f, // ITEM_SUBCLASS_WEAPON_MISCELLANEOUS
        0.66f, // ITEM_SUBCLASS_WEAPON_DAGGER
        0.00f, // ITEM_SUBCLASS_WEAPON_THROWN
        0.00f, // ITEM_SUBCLASS_WEAPON_SPEAR
        1.00f, // ITEM_SUBCLASS_WEAPON_CROSSBOW
        0.66f, // ITEM_SUBCLASS_WEAPON_WAND
        0.66f, // ITEM_SUBCLASS_WEAPON_FISHING_POLE
    };

    float levelPenalty = 1.0f;
    if (itemLevel <= 28)
        levelPenalty = 0.966f - float(28u - itemLevel) / 54.0f;

    if (itemClass == ITEM_CLASS_ARMOR)
    {
        if (inventoryType > INVTYPE_ROBE)
            return 0;

        return 5 * uint32(round(25.0f * qualityMultipliers[quality] * armorMultipliers[inventoryType] * levelPenalty));
    }

    return 5 * uint32(round(18.0f * qualityMultipliers[quality] * weaponMultipliers[itemSubClass] * levelPenalty));
};

void ObjectMgr::LoadItemTemplates()
{
    uint32 oldMSTime = getMSTime();
    uint32 sparseCount = 0;

    for (auto sparse : sItemSparseStore)
    {
        if (ItemEntry const* db2Data = sItemStore.LookupEntry(sparse->ID))
        {
            ItemTemplate& itemTemplate = _itemTemplateStore[sparse->ID];

            itemTemplate.ItemId = sparse->ID;
            itemTemplate.Class = db2Data->ClassId;
            itemTemplate.SubClass = db2Data->SubclassId;
            itemTemplate.SoundOverrideSubclass = db2Data->SoundOverride_subclassId;
            itemTemplate.Name1 = sparse->Name;
            itemTemplate.DisplayInfoID = db2Data->IconFileDataId;
            itemTemplate.DisplayInfoID = GetItemDisplayID(sparse->ID, 0);
            itemTemplate.Quality = sparse->Quality;
            itemTemplate.Flags = sparse->Flags;
            itemTemplate.Flags2 = sparse->Flags2;
            itemTemplate.Flags3 = sparse->Flags3;
            itemTemplate.Flags4 = sparse->Flags4;
            itemTemplate.PriceRandomValue = sparse->PriceRandomValue;
            itemTemplate.PriceVariance = sparse->PriceVariance;
            itemTemplate.BuyCount = std::max(sparse->BuyCount, 1u);
            itemTemplate.BuyPrice = sparse->BuyPrice;
            itemTemplate.SellPrice = sparse->SellPrice;
            itemTemplate.InventoryType = db2Data->InventoryType;
            itemTemplate.AllowableClass = sparse->AllowableClass;
            itemTemplate.AllowableRace = sparse->AllowableRace ? sparse->AllowableRace : -1;
            itemTemplate.ItemLevel = sparse->ItemLevel;
            itemTemplate.RequiredLevel = sparse->RequiredLevel;
            itemTemplate.RequiredSkill = sparse->RequiredSkill;
            itemTemplate.RequiredSkillRank = sparse->RequiredSkillRank;
            itemTemplate.RequiredSpell = sparse->RequiredSpell;
            itemTemplate.RequiredHonorRank = sparse->RequiredHonorRank;
            itemTemplate.RequiredCityRank = sparse->RequiredCityRank;
            itemTemplate.RequiredReputationFaction = sparse->RequiredReputationFaction;
            itemTemplate.RequiredReputationRank = sparse->RequiredReputationRank;
            itemTemplate.MaxCount = sparse->MaxCount;
            itemTemplate.Stackable = sparse->Stackable;
            itemTemplate.ContainerSlots = sparse->ContainerSlots;

            for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
            {
                itemTemplate.ItemStat[i].ItemStatType = sparse->ItemStatType[i];
                itemTemplate.ItemStat[i].ItemStatValue = sparse->ItemStatValue[i];
                itemTemplate.ItemStat[i].ScalingValue = sparse->ItemStatAllocation[i];
                itemTemplate.ItemStat[i].SocketCostRate = sparse->ItemStatSocketCostMultiplier[i];
            }

            itemTemplate.ScalingStatDistribution = sparse->ScalingStatDistribution;

            itemTemplate.DamageType = sparse->DamageType;
            itemTemplate.Armor = FillItemArmor(sparse->ItemLevel, db2Data->ClassId, db2Data->SubclassId, sparse->Quality, sparse->InventoryType);
            itemTemplate.Delay = sparse->Delay;
            itemTemplate.RangedModRange = sparse->RangedModRange;
            
            std::vector<uint32>& l_EffectsIndex = sItemEffectsByItemID[sparse->ID];

            /// Commented to avoid spam in ashran.log
            /// ASSERT(l_EffectsIndex.size() < MAX_ITEM_PROTO_SPELLS);

            memset(&itemTemplate.Spells[0], 0, sizeof(_Spell) * MAX_ITEM_PROTO_SPELLS);

            for (uint32 l_I = 0; l_I < l_EffectsIndex.size(); ++l_I)
            {
                const ItemEffectEntry* l_Entry = sItemEffectStore.LookupEntry(l_EffectsIndex[l_I]);
                if (!l_Entry)
                    continue;

                itemTemplate.Spells[l_Entry->EffectIndex].SpellId = l_Entry->SpellID;
                itemTemplate.Spells[l_Entry->EffectIndex].SpellTrigger = l_Entry->SpellTrigger;
                itemTemplate.Spells[l_Entry->EffectIndex].SpellCharges = l_Entry->SpellCharge;
                itemTemplate.Spells[l_Entry->EffectIndex].SpellCooldown = l_Entry->SpellCooldown;
                itemTemplate.Spells[l_Entry->EffectIndex].SpellCategory = l_Entry->SpellCategory;
                itemTemplate.Spells[l_Entry->EffectIndex].SpellCategoryCooldown = l_Entry->SpellCategoryCooldown;

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_Entry->SpellID))
                {
                    uint8 l_EffectIndex = l_SpellInfo->GetEffectIndex(SPELL_EFFECT_GIVE_ARTIFACT_POWER);
                    if (l_EffectIndex != 255)
                    {
                        l_EffectIndex = l_SpellInfo->GetEffectIndex(SPELL_EFFECT_GIVE_ARTIFACT_POWER);
                        if (l_EffectIndex != 255)
                        {
                            itemTemplate.NeedScaleOnArtifactKnowledge = true;
                        }
                    }
                }
            }

            itemTemplate.SpellPPMRate = 0.0f;
            itemTemplate.Bonding = sparse->Bonding;
            itemTemplate.Description = sparse->Description;
            itemTemplate.PageText = sparse->PageText;
            itemTemplate.LanguageID = sparse->LanguageID;
            itemTemplate.PageMaterial = sparse->PageMaterial;
            itemTemplate.StartQuest = sparse->StartQuest;
            itemTemplate.LockID = sparse->LockID;
            itemTemplate.Material = sparse->Material;
            itemTemplate.Sheath = sparse->Sheath;
            itemTemplate.RandomProperty = sparse->RandomProperty;
            itemTemplate.RandomSuffix = sparse->RandomSuffix;
            itemTemplate.ItemSet = sparse->ItemSet;
            itemTemplate.MaxDurability = FillMaxDurability(db2Data->ClassId, db2Data->SubclassId, sparse->InventoryType, sparse->Quality, sparse->ItemLevel);
            itemTemplate.Area = sparse->Area;
            itemTemplate.Map = sparse->Map;
            itemTemplate.BagFamily = sparse->BagFamily;
            itemTemplate.TotemCategory = sparse->TotemCategory;
            for (uint32 i = 0; i < MAX_ITEM_PROTO_SOCKETS; ++i)
            {
                itemTemplate.Socket[i].Color = sparse->SocketColor[i];
                itemTemplate.Socket[i].Content = 0;//sparse->Content[i];
            }

            itemTemplate.socketBonus = sparse->SocketBonus;
            itemTemplate.GemProperties = sparse->GemProperties;

            itemTemplate.ArmorDamageModifier = sparse->ArmorDamageModifier;
            itemTemplate.Duration = sparse->Duration;
            itemTemplate.ItemLimitCategory = sparse->ItemLimitCategory;
            itemTemplate.HolidayId = sparse->HolidayId;
            itemTemplate.StatScalingFactor = sparse->StatScalingFactor;
            itemTemplate.CurrencySubstitutionId = sparse->CurrencySubstitutionId;
            itemTemplate.CurrencySubstitutionCount = sparse->CurrencySubstitutionCount;
            itemTemplate.ItemNameDescriptionID = sparse->ItemNameDescriptionID;
            itemTemplate.ArtifactID = sparse->ArtifactID;
            itemTemplate.RequiredExpansion = sparse->RequiredExpansion;
            itemTemplate.ScriptId = 0;
            itemTemplate.FoodType = 0;
            itemTemplate.MinMoneyLoot = 0;
            itemTemplate.MaxMoneyLoot = 0;
            itemTemplate.FlagsCu = 0;
            itemTemplate.ItemSpecClassMask = 0;

            if (std::vector<ItemSpecOverrideEntry const*> const* itemSpecOverrides = GetItemSpecOverrides(sparse->ID))
            {
                for (ItemSpecOverrideEntry const* itemSpecOverride : *itemSpecOverrides)
                    if (ChrSpecializationsEntry const* specialization = sChrSpecializationsStore.LookupEntry(itemSpecOverride->specID))
                        itemTemplate.Specializations[0].set(ItemTemplate::CalculateItemSpecBit(specialization));

                itemTemplate.Specializations[1] |= itemTemplate.Specializations[0];
            }
            else
            {
                ItemSpecStats itemSpecStats(db2Data, sparse);

                for (auto itemSpec : sItemSpecStore)
                {
                    if (itemSpecStats.ItemType != itemSpec->ItemType)
                        continue;

                    bool hasPrimary = false;
                    bool hasSecondary = itemSpec->SecondaryStat == ITEM_SPEC_STAT_NONE;
                    for (uint32 i = 0; i < itemSpecStats.ItemSpecStatCount; ++i)
                    {
                        if (itemSpecStats.ItemSpecStatTypes[i] == itemSpec->PrimaryStat)
                            hasPrimary = true;
                        if (itemSpecStats.ItemSpecStatTypes[i] == itemSpec->SecondaryStat)
                            hasSecondary = true;
                    }

                    if (!hasPrimary || !hasSecondary)
                        continue;

                    if (ChrSpecializationsEntry const* specialization = sChrSpecializationsStore.LookupEntry(itemSpec->SpecializationID))
                    {
                        if ((1 << (specialization->ClassID - 1)) & sparse->AllowableClass)
                        {
                            itemTemplate.ItemSpecClassMask |= 1 << (specialization->ClassID - 1);
                            itemTemplate.Specializations[itemSpec->MaxLevel > 40].set(ItemTemplate::CalculateItemSpecBit(specialization));
                        }
                    }
                }
            }

            auto l_Iter = g_PvPItemStoreLevels.find(sparse->ID);
            if (l_Iter != g_PvPItemStoreLevels.end())
                itemTemplate.PvPScalingLevel = l_Iter->second;
            else
                itemTemplate.PvPScalingLevel = 0;
            ++sparseCount;
        }
    }

    // Check if item templates for DB2 referenced character start outfit are present
    std::set<uint32> notFoundOutfit;
    for (uint32 i = 1; i < sCharStartOutfitStore.GetNumRows(); ++i)
    {
        CharStartOutfitEntry const* entry = sCharStartOutfitStore.LookupEntry(i);
        if (!entry)
            continue;

        for (int j = 0; j < MAX_OUTFIT_ITEMS; ++j)
        {
            if (entry->ItemId[j] <= 0)
                continue;

            uint32 item_id = entry->ItemId[j];

            if (!GetItemTemplate(item_id))
                notFoundOutfit.insert(item_id);
        }
    }

    for (std::set<uint32>::const_iterator itr = notFoundOutfit.begin(); itr != notFoundOutfit.end(); ++itr)
        sLog->outError(LOG_FILTER_SQL, "Item (Entry: %u) does not exist in `item_template` but is referenced in `CharStartOutfit.db2`", *itr);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u item templates from ItemSparse.db2 in %u ms", sparseCount, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPvpItemRewards()
{
    std::map<PvpSeasons, ItemSeasonDescriptions> g_ItemDescriptionsPerSeason =
    {
        { PvpSeasons::LegionSeason3, ItemSeasonDescriptions(13295, 13296) },
        { PvpSeasons::LegionSeason4, ItemSeasonDescriptions(13297, 13298) },
        { PvpSeasons::LegionSeason5, ItemSeasonDescriptions(13299, 13300) },
        { PvpSeasons::LegionSeason6, ItemSeasonDescriptions(13311, 13313) },
        { PvpSeasons::LegionSeason7,  ItemSeasonDescriptions(13312, 13314) }
    };

    for (ItemTemplateContainer::const_iterator l_Iter = _itemTemplateStore.begin(); l_Iter != _itemTemplateStore.end(); ++l_Iter)
    {
        ItemTemplate const* l_Item = &l_Iter->second;

        /// Items with level 1 are cosmetic items (transmog cloaks, tabards etc.)
        if (l_Item->ItemLevel == 1)
            continue;

        /// Only process gear & relics
        if ((!l_Item->isArmorOrWeapon() && !l_Item->IsArtifactRelic()))
            continue;

        for (auto l_ItemDescriptionForSeason : g_ItemDescriptionsPerSeason)
        {
            if (l_Item->ItemNameDescriptionID != l_ItemDescriptionForSeason.second.RegularGearDescriptionID && l_Item->ItemNameDescriptionID != l_ItemDescriptionForSeason.second.EliteGearDescriptionID)
                continue;

            if (l_ItemDescriptionForSeason.first == PvpSeasons::CurrentSeason)
            {
                /// Add OBLITERATABLE flag, current season items should be obliterable since 7.2
                if (ItemSparseEntry const* l_Sparse = sItemSparseStore.LookupEntry(l_Item->ItemId))
                    const_cast<ItemSparseEntry*>(l_Sparse)->Flags3 |= (uint32)ItemFlagsEX2::OBLITERATABLE;

                const_cast<ItemTemplate*>(l_Item)->Flags3 |= (uint32)ItemFlagsEX2::OBLITERATABLE;
                sLog->outAshran("INSERT INTO _hotfixs (Entry, Hash, Date, Comment) VALUES (%u, 2442913102, 1548173096, 'Custom ItemSparse - Add OBLITERABLE flag for Season 7 PvP Items');", l_Item->ItemId);
            }

            ItemTemplateListBySpec& l_List = l_Item->ItemNameDescriptionID == l_ItemDescriptionForSeason.second.EliteGearDescriptionID ? _elitePvpGearRewards[l_ItemDescriptionForSeason.first] : (l_Item->Quality == ItemQualities::ITEM_QUALITY_EPIC ? _ratedPvpGearRewards[l_ItemDescriptionForSeason.first] : _regularPvpGearRewards[l_ItemDescriptionForSeason.first]);

            for (ChrSpecializationsEntry const* l_Spec : sChrSpecializationsStore)
            {
                if (!l_Spec->ClassID)
                    continue;

                if (l_Item->HasSpec(SpecIndex(l_Spec->ID), l_Item->IsArtifactRelic() ? LevelLimit::MAX_LEVEL : l_Item->RequiredLevel))
                {
                    if (l_Item->IsArtifactRelic())
                    {
                        if (l_Item->ItemNameDescriptionID == l_ItemDescriptionForSeason.second.EliteGearDescriptionID)
                            _elitePvpGearRewards[l_ItemDescriptionForSeason.first][l_Spec->ID].push_back(l_Item);
                        else
                        {
                            _regularPvpGearRewards[l_ItemDescriptionForSeason.first][l_Spec->ID].push_back(l_Item);
                            _ratedPvpGearRewards[l_ItemDescriptionForSeason.first][l_Spec->ID].push_back(l_Item);
                        }
                    }
                    else
                        l_List[l_Spec->ID].push_back(l_Item);
                }
            }
        }
    }
}

void ObjectMgr::LoadAvailableLegendaryItems()
{
    QueryResult l_Result = WorldDatabase.PQuery("SELECT itemId, relinquished FROM item_legendary_available");
    if (!l_Result)
        return;

    do
    {
        Field* l_Fields = l_Result->Fetch();
        uint32 l_ItemID = l_Fields[0].GetUInt32();
        bool l_Relinquished = l_Fields[1].GetUInt8();

        ItemTemplate const* l_Item = GetItemTemplate(l_ItemID);
        if (!l_Item)
            continue;

        for (uint32 l_I = 0; l_I <= sChrSpecializationsStore.GetNumRows(); ++l_I)
        {
            ChrSpecializationsEntry const* l_Spec = sChrSpecializationsStore.LookupEntry(l_I);
            if (!l_Spec)
                continue;

            if (!l_Spec->ClassID)
                continue;

            if (l_Item->HasSpec((SpecIndex)l_I, l_Item->RequiredLevel))
            {
                g_LegendariesAvailable[l_I].push_back(l_Item);
                g_ClassesLegendariesAvailable[l_Spec->ClassID].push_back(l_Item);

                if (l_Relinquished)
                    g_RelinquishedLegendariesAvailable[l_I].push_back(l_Item);
            }
        }

        _LegionLegendariesSet.insert(l_ItemID);
        if (l_Relinquished)
            _LegionRelinquishedLegendaries.insert(l_ItemID);
    }
    while (l_Result->NextRow());
}

void ObjectMgr::LoadItemTemplateCorrections()
{
    for (ItemTemplateContainer::const_iterator l_Iter = _itemTemplateStore.begin(); l_Iter != _itemTemplateStore.end(); l_Iter++)
    {
        ItemTemplate& l_ItemTemplate = const_cast<ItemTemplate&>(l_Iter->second);

        switch (l_ItemTemplate.ItemId)
        {
            // Prevent people opening strongboxed they cant use & abuse some other stuff
            case 120354: ///< Gold Strongbox A
            case 120355: ///< Silver Strongbox A
            case 120356: ///< Bronze Strongbox A
            case 120353: ///< Steel Strongbox A
            case 118065: ///< Gleaming Ashmaul Strongbox (A)
            case 126905: ///< Steel Strongbox A - S2
            case 126908: ///< Bronze Strongbox A - S2
            case 126907: ///< Silver Strongbox A - S2
            case 126906: ///< Gold Strongbox A - S2
            case 126919: ///< Champion's Strongbox A (RBG)
                l_ItemTemplate.Flags2 |= int32(ItemFlagsEX::ALLIANCE);
                l_ItemTemplate.RequiredLevel = 100;
                break;
            case 111598: ///< Gold Strongbox H
            case 111599: ///< Silver strongbox H
            case 111600: ///< Bronze Strongbox H
            case 119330: ///< Steel StrongBox H
            case 120151: ///< Gleaming Ashmaul Strongbox (H)
            case 126904: ///< Steel Strongbox H - S2
            case 126903: ///< Bronze Strongbox H - S2
            case 126902: ///< Silver Strongbox H - S2
            case 126901: ///< Gold Strongbox H - S2
            case 126920: ///< Champion's Strongbox H (RBG)
                l_ItemTemplate.Flags2 |= int32(ItemFlagsEX::HORDE);
                l_ItemTemplate.RequiredLevel = 100;
                break;
            /// Class hall paladins items
            case 140558:
            case 140555:
            case 140547:
            case 140535:
            /// Class hall DK items
            case 140538:
            case 140554:
            /// Class hall DH item
            case 140560:
            /// Class hall Druid items
            case 140540:
            case 140542:
            case 140549:
            /// Class hall hunter
            case 140539:
            case 140544:
            case 140548:
            /// Class hall mage
            case 140550:
            /// Class hall monk
            case 140543:
            case 140551:
            case 140564:
            /// Class hall priest
            case 140552:
            /// Class hall rogue
            case 140541:
            case 140556:
            /// Class hall shaman
            case 140536:
            case 140545:
            case 140546:
            /// Class hall warlock
            case 140553:
            /// Class hall warrior
            case 140537:
            case 140557:
            case 140559:
            /// Misc this id come from all itemsparse
            case 121745:
            case 128826:
            case 128944:
            case 133580:
            case 139905:
            case 139909:
            case 139934:
            case 139946:
            case 139965:
            case 139997:
            case 140005:
            case 140687:
            case 140688:
            case 140689:
            case 140690:
            case 140691:
            case 140692:
            case 140693:
            case 140694:
            case 140695:
            case 140696:
            case 140697:
            case 140698:
            case 140699:
            case 140700:
            case 140701:
            case 140702:
            case 140703:
            case 140704:
            case 140705:
            case 140706:
            case 140707:
            case 140708:
            case 140709:
            case 140710:
            case 140711:
            case 140712:
            case 140713:
            case 140714:
            case 140715:
            case 140716:
            case 140717:
            case 140718:
            case 140719:
            case 140720:
            case 140736:
            case 140737:
            case 140738:
            case 140739:
            case 140740:
            case 141604:
            case 142526:
                l_ItemTemplate.RequiredLevel = 98;
                break;
        }
    }
}

void ObjectMgr::LoadItemTemplateAddon()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = WorldDatabase.Query("SELECT Id, FlagsCu, FoodType, MinMoneyLoot, MaxMoneyLoot, SpellPPMChance FROM item_template_addon");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 itemId = fields[0].GetUInt32();
            if (!GetItemTemplate(itemId))
            {
                sLog->outError(LOG_FILTER_SQL, "Item %u specified in `item_template_addon` does not exist, skipped.", itemId);
                continue;
            }

            uint32 minMoneyLoot = fields[3].GetUInt32();
            uint32 maxMoneyLoot = fields[4].GetUInt32();
            if (minMoneyLoot > maxMoneyLoot)
            {
                sLog->outError(LOG_FILTER_SQL, "Minimum money loot specified in `item_template_addon` for item %u was greater than maximum amount, swapping.", itemId);
                std::swap(minMoneyLoot, maxMoneyLoot);
            }
            ItemTemplate& itemTemplate = _itemTemplateStore[itemId];
            itemTemplate.FlagsCu = fields[1].GetUInt32();
            itemTemplate.FoodType = fields[2].GetUInt8();
            itemTemplate.MinMoneyLoot = minMoneyLoot;
            itemTemplate.MaxMoneyLoot = maxMoneyLoot;
            itemTemplate.SpellPPMRate = fields[5].GetFloat();
            ++count;
        }
        while (result->NextRow());
    }

    for (uint32 l_Entry = 0; l_Entry < sItemSparseStore.GetNumRows(); l_Entry++)
    {
        auto l_Itr = _itemTemplateStore.find(l_Entry);
        if (l_Itr == _itemTemplateStore.end())
            continue;

        ItemTemplate& l_ItemTemplate = l_Itr->second;

        if (l_ItemTemplate.Quality == ItemQualities::ITEM_QUALITY_HEIRLOOM)
            l_ItemTemplate.FlagsCu |= int32(ItemFlagsCustom::ITEM_FLAGS_CU_CANT_BE_SELL);
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u item addon templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadItemScriptNames()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;

    QueryResult result = WorldDatabase.Query("SELECT Id, ScriptName FROM item_script_names");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 itemId = fields[0].GetUInt32();
            if (!GetItemTemplate(itemId))
            {
                sLog->outError(LOG_FILTER_SQL, "Item %u specified in `item_script_names` does not exist, skipped.", itemId);
                continue;
            }

            _itemTemplateStore[itemId].ScriptId = GetScriptId(fields[1].GetCString());
            ++count;
        }
        while (result->NextRow());
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u item script names in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadItemSpecs()
{
    uint32 l_Count = 0;
    uint32 l_OldMSTime = getMSTime();

    for (ItemTemplateContainer::iterator l_Itr = _itemTemplateStore.begin(); l_Itr != _itemTemplateStore.end(); ++l_Itr)
    {
        ItemTemplate& l_ItemTemplate = l_Itr->second;
        if (l_ItemTemplate.HasSpec())
            continue;

        ItemSpecStats itemSpecStats(sItemStore.LookupEntry(l_ItemTemplate.ItemId), sItemSparseStore.LookupEntry(l_ItemTemplate.ItemId));

        if (itemSpecStats.ItemSpecStatCount)
        {
            for (uint32 l_SpecIndex = 0; l_SpecIndex < sItemSpecStore.GetNumRows(); l_SpecIndex++)
            {
                ItemSpecEntry const* itemSpec = sItemSpecStore.LookupEntry(l_SpecIndex);
                if (itemSpec == nullptr)
                    continue;

                if (itemSpecStats.ItemType != itemSpec->ItemType)
                    continue;

                bool hasPrimary = false;
                bool hasSecondary = itemSpec->SecondaryStat == ITEM_SPEC_STAT_NONE;

                for (uint32 i = 0; i < itemSpecStats.ItemSpecStatCount; ++i)
                {
                    if (itemSpecStats.ItemSpecStatTypes[i] == itemSpec->PrimaryStat)
                        hasPrimary = true;
                    if (itemSpecStats.ItemSpecStatTypes[i] == itemSpec->SecondaryStat)
                        hasSecondary = true;
                }

                if (!hasPrimary)
                    hasPrimary = hasSecondary && itemSpecStats.ItemSpecStatCount == 1 && !itemSpec->PrimaryStat; ///< If only 1 was found, it doesnt have any stats, therefore ignore primary stat check

                if (!hasPrimary || !hasSecondary)
                    continue;

                if (l_ItemTemplate.RequiredLevel > 40 && l_ItemTemplate.InventoryType != InventoryType::INVTYPE_CLOAK && l_ItemTemplate.Class == ItemClass::ITEM_CLASS_ARMOR)
                {
                    uint8 l_Class = GetClassBySpec(itemSpec->SpecializationID);
                    switch (l_ItemTemplate.SubClass)
                    {
                        case ITEM_SUBCLASS_ARMOR_CLOTH:
                            if (l_Class != Classes::CLASS_PRIEST && l_Class != Classes::CLASS_MAGE && l_Class != Classes::CLASS_WARLOCK)
                                continue;
                            break;
                        case ITEM_SUBCLASS_ARMOR_LEATHER:
                            if (l_Class != Classes::CLASS_DRUID && l_Class != Classes::CLASS_ROGUE && l_Class != Classes::CLASS_MONK && l_Class != Classes::CLASS_DEMON_HUNTER)
                                continue;
                            break;
                        case ITEM_SUBCLASS_ARMOR_MAIL:
                            if (l_Class != Classes::CLASS_HUNTER && l_Class != Classes::CLASS_SHAMAN)
                                continue;
                            break;
                        case ITEM_SUBCLASS_ARMOR_PLATE:
                            if (l_Class != Classes::CLASS_DEATH_KNIGHT && l_Class != Classes::CLASS_WARRIOR && l_Class != Classes::CLASS_PALADIN)
                                continue;
                            break;
                    }
                }
            }
        }
    }
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u item specs in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadItemExtendedCostAddon()
{
    uint32 l_OldMSTime = getMSTime();
    uint32 l_Count = 0;

    QueryResult l_Result = WorldDatabase.PQuery("SELECT entry, overrideBuyPrice FROM item_extended_cost_addon");

    if (!l_Result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 item extended cost addon. DB table `item_extended_cost_addon` is empty.");
        return;
    }

    do
    {
        Field* l_Field = l_Result->Fetch();

        uint32 l_Entry            = l_Field[0].GetUInt32();
        uint64 l_OverrideBuyPrice = l_Field[1].GetUInt64();

        ItemExtendedCostAddon& l_Addon = m_ItemExtendedCostAddon[l_Entry];
        l_Addon.Entry            = l_Entry;
        l_Addon.OverrideBuyPrice = l_OverrideBuyPrice;

        l_Count++;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u item extended cost addon in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

ItemTemplate const* ObjectMgr::GetItemTemplate(uint32 entry)
{
    ItemTemplateContainer::const_iterator itr = _itemTemplateStore.find(entry);
    if (itr != _itemTemplateStore.end())
        return &(itr->second);

    return nullptr;
}

ItemTemplate const * ObjectMgr::GetItemRewardForPvp(uint16 p_Spec, MS::Battlegrounds::RewardGearType p_Type, Player* p_Source, InventoryType p_InventoryType /*= InventoryType::INVTYPE_NON_EQUIP*/, PvpSeasons p_Season /*= PvpSeasons::CurrentSeason*/)
{
    ItemTemplateListBySpec* l_Spec = nullptr;

    switch (p_Type)
    {
        case MS::Battlegrounds::RewardGearType::Regular:
            l_Spec = &_regularPvpGearRewards[p_Season];
            break;
        case MS::Battlegrounds::RewardGearType::Rated:
            l_Spec = &_ratedPvpGearRewards[p_Season];
            break;
        case MS::Battlegrounds::RewardGearType::Elite:
            l_Spec = &_elitePvpGearRewards[p_Season];
            break;
        default:
            return nullptr;
    }

    auto l_Itr = l_Spec->find(p_Spec);

    if (l_Itr == l_Spec->end())
        return nullptr;

    if (!l_Itr->second.size())
        return nullptr;

    std::map<uint32, std::vector<ItemTemplate const*>> l_ItemGroupPerSpec;

    for (ItemTemplate const* l_Item : l_Itr->second)
    {
        if (p_Source->CanUseItem(l_Item) == InventoryResult::EQUIP_ERR_OK && (p_InventoryType == InventoryType::INVTYPE_NON_EQUIP || l_Item->InventoryType == p_InventoryType))
            l_ItemGroupPerSpec[l_Item->InventoryType].push_back(l_Item);
    }

    if (l_ItemGroupPerSpec.empty())
        return nullptr;

    auto l_It = l_ItemGroupPerSpec.begin();
    std::advance(l_It, urand(0, l_ItemGroupPerSpec.size() - 1));
    return l_It->second[rand() % l_It->second.size()];
}

ItemTemplate const* ObjectMgr::GetItemLegendary(uint16 p_Spec, Player* p_Source, bool p_Relinquished /*= false*/, InventoryType p_InventoryType /*= InventoryType::INVTYPE_NON_EQUIP*/)
{
    ItemTemplateListBySpec& l_LegendariesPoolPerSpec = p_Relinquished ? g_RelinquishedLegendariesAvailable : g_LegendariesAvailable;
    std::vector<ItemTemplate const*> l_AvailableLegendaries = l_LegendariesPoolPerSpec[p_Spec];

    /// Search per spec first
    std::vector<ItemTemplate const*> l_PossibleLegendaries;
    for (ItemTemplate const* l_ItemTemplate : l_AvailableLegendaries)
    {
        if ((p_InventoryType != InventoryType::INVTYPE_NON_EQUIP  && l_ItemTemplate->InventoryType != p_InventoryType) || p_Source->HasItemCount(l_ItemTemplate->ItemId, 1, true, true, true))
            continue;

        l_PossibleLegendaries.push_back(l_ItemTemplate);
    }

    /// Probably because the player already have all the legendary of his spec, fallback on others specs
    if (l_PossibleLegendaries.empty())
    {
        auto& l_CharSpecializations = GetChrSpecializationsForClass(p_Source->getClass());
        for (ChrSpecializationsEntry const* l_Spec : l_CharSpecializations)
        {
            if (l_Spec->ID == p_Spec)
                continue;

            l_AvailableLegendaries = l_LegendariesPoolPerSpec[l_Spec->ID];

            for (ItemTemplate const* l_ItemTemplate : l_AvailableLegendaries)
            {
                if ((p_InventoryType != InventoryType::INVTYPE_NON_EQUIP  && l_ItemTemplate->InventoryType != p_InventoryType) || p_Source->HasItemCount(l_ItemTemplate->ItemId, 1, true, true, true))
                    continue;

                l_PossibleLegendaries.push_back(l_ItemTemplate);
            }
        }
    }

    /// The player have all legendaries for his class -> Roll other class token. 7.3.5 feature
    if (l_PossibleLegendaries.empty())
    {
        if (!sWorld->getBoolConfig(CONFIG_ENABLE_LEGION_LEGENDARY_TOKENS))
            return nullptr;

        std::list<uint32> legendaryTokens = { 147294, 147295, 147296, 147297, 147298, 147299, 147300, 147301, 147302, 147303, 147304, 147305 };
        auto classMask = p_Source->getClassMask();
        std::vector<ItemTemplate const*> possibleTokens;
        for (auto i : legendaryTokens)
        {
            if (auto itemTemplate = GetItemTemplate(i))
            {
                if (itemTemplate->AllowableClass & classMask)
                    continue;
                possibleTokens.push_back(itemTemplate);
            }
        }
        if (!possibleTokens.empty())
            JadeCore::Containers::RandomShuffle(possibleTokens);
        return possibleTokens.empty() ? nullptr : JadeCore::Containers::SelectRandomContainerElement(possibleTokens);
    }

    ItemTemplate const* l_Item = l_PossibleLegendaries[urand(0, l_PossibleLegendaries.size() - 1)];
    uint32 l_Count = 0;

    while (p_Source->CanUseItem(l_Item) != EQUIP_ERR_OK)
    {
        /// Shouldn't happen, but more safe
        if (l_Count == 200)
            return nullptr;

        l_Item = l_PossibleLegendaries[urand(0, l_PossibleLegendaries.size() - 1)];
        l_Count++;
    }

    return l_Item;
}

ItemTemplate const* ObjectMgr::GetItemLegendaryForClass(uint32 p_ClassID, Player* p_Source)
{
    ItemTemplateListBySpec::iterator l_Itr = g_ClassesLegendariesAvailable.find(p_ClassID);
    if (l_Itr == g_ClassesLegendariesAvailable.end())
        return nullptr;

    if (!l_Itr->second.size())
        return nullptr;

    ItemTemplate const* l_Item = l_Itr->second[urand(0, l_Itr->second.size() - 1)];
    if (!l_Item)
        return nullptr;

    uint32 l_Count = 0;

    while ((p_Source->CanUseItem(l_Item) != InventoryResult::EQUIP_ERR_OK || p_Source->HasItemCount(l_Item->ItemId, 1, true, true, true)))
    {
        /// Shouldn't happen, but more safe
        if (l_Count == 200)
            return nullptr;

        l_Item = l_Itr->second[urand(0, l_Itr->second.size() - 1)];
        l_Count++;
    }

    return l_Item;
}

bool ObjectMgr::IsLegionLegendaryItem(uint32 p_ItemId, bool p_Relinquished /*= false*/) const
{
    if (p_Relinquished)
        return _LegionRelinquishedLegendaries.find(p_ItemId) != _LegionRelinquishedLegendaries.end();
    else
        return _LegionLegendariesSet.find(p_ItemId) != _LegionLegendariesSet.end();
}

void ObjectMgr::LoadVehicleTemplateAccessories()
{
    uint32 oldMSTime = getMSTime();

    _vehicleTemplateAccessoryStore.clear();                           // needed for reload case

    uint32 count = 0;

    //                                                     0              1              2          3           4             5              6          7          8          9
    QueryResult result = WorldDatabase.Query("SELECT `EntryOrAura`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `offsetX`, `offsetY`, `offsetZ`, `offsetO` FROM `vehicle_template_accessory`");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 vehicle template accessories. DB table `vehicle_template_accessory` is empty.");

        return;
    }

    do
    {
        Field* fields           = result->Fetch();
        uint8 l_Index           = 0;

        int32  uiEntryOrAura    = fields[l_Index++].GetInt32();
        uint32 uiAccessory      = fields[l_Index++].GetUInt32();
        int8   uiSeat           = fields[l_Index++].GetInt32();
        bool   bMinion          = fields[l_Index++].GetBool();
        uint8  uiSummonType     = fields[l_Index++].GetUInt8();
        uint32 uiSummonTimer    = fields[l_Index++].GetUInt32();
        float offsetX           = fields[l_Index++].GetFloat();
        float offsetY           = fields[l_Index++].GetFloat();
        float offsetZ           = fields[l_Index++].GetFloat();
        float offsetO           = fields[l_Index++].GetFloat();

        if (uiEntryOrAura > 0 && !sObjectMgr->GetCreatureTemplate(uiEntryOrAura))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `vehicle_template_accessory`: creature template entry %u does not exist.", uiEntryOrAura);
            continue;
        }

        if (uiEntryOrAura < 0 && !sSpellMgr->GetSpellInfo(std::abs(uiEntryOrAura)))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `vehicle_template_accessory`: aura %u does not exist.", uiEntryOrAura);
            continue;
        }

        if (!sObjectMgr->GetCreatureTemplate(uiAccessory))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `vehicle_template_accessory`: Accessory %u does not exist.", uiAccessory);
            continue;
        }

        if (uiEntryOrAura > 0 && _spellClickInfoStore.find(uiEntryOrAura) == _spellClickInfoStore.end())
        {
            sLog->outError(LOG_FILTER_SQL, "Table `vehicle_template_accessory`: creature template entry %u has no data in npc_spellclick_spells", uiEntryOrAura);
            continue;
        }

        _vehicleTemplateAccessoryStore[uiEntryOrAura].push_back(VehicleAccessory(uiAccessory, uiSeat, bMinion, uiSummonType, uiSummonTimer, Position { offsetX, offsetY, offsetZ, offsetO }));

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Vehicle Template Accessories in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadAreaTriggerActionsAndData()
{
    uint32 l_oldMSTime = getMSTime();

    _areaTriggerData.clear();
    _areaTriggerDataByEntry.clear();

    QueryResult result = WorldDatabase.Query("SELECT `entry`, `spellId`, `customEntry`, `Radius`, `RadiusTarget`, `Height`, `HeightTarget`, `Float4`, `Float5`,"
        "`MoveCurveID`, `ElapsedTime`, `windX`, `windY`, `windZ`, `windSpeed`, `windType`, `polygon`, `MorphCurveID`, `FacingCurveID`, `ScaleCurveID`,"
        "`ShapeID1`, `ShapeID2`, `HasUnkBit50`, `UnkBit50`, `HasFollowsTerrain`, `HasAttached`, `HasAbsoluteOrientation`, `HasDynamicShape`, `HasFaceMovementDir`,"
        "`hasAreaTriggerBox`, `RollPitchYaw1X`, `RollPitchYaw1Y`, `RollPitchYaw1Z`, `TargetRollPitchYawX`, `TargetRollPitchYawY`, `TargetRollPitchYawZ`, `DecalPropertiesId`, `Distance`, `isMoving`, `IsCheckingWithObjectSize`, `IsCheckingNeighbours`, `ScriptName`, `VisualID` FROM areatrigger_template");

    if (result)
    {
        uint32 counter = 0;
        do
        {
            Field* fields = result->Fetch();

            uint8 i = 0;
            uint32 id = fields[i++].GetUInt32();
            uint32 spellId = fields[i++].GetUInt32();
            uint32 customEntry = fields[i++].GetUInt32();
            AreaTriggerInfo info;
            info.spellId = spellId ? spellId : 0;
            info.customEntry = customEntry;
            info.Radius = fields[i++].GetFloat();
            info.RadiusTarget = fields[i++].GetFloat();
            info.Polygon.Height = fields[i++].GetFloat();
            info.Polygon.HeightTarget = fields[i++].GetFloat();
            info.LocationZOffset = fields[i++].GetFloat();
            info.LocationZOffsetTarget = fields[i++].GetFloat();
            info.MoveCurveID = fields[i++].GetUInt32();
            info.ElapsedTime = fields[i++].GetUInt32();
            info.windX = fields[i++].GetFloat();
            info.windY = fields[i++].GetFloat();
            info.windZ = fields[i++].GetFloat();
            info.windSpeed = fields[i++].GetFloat();
            info.windType = fields[i++].GetUInt32();
            info.polygon = fields[i++].GetUInt32();
            info.MorphCurveID = fields[i++].GetUInt32();
            info.FacingCurveID = fields[i++].GetUInt32();
            info.ScaleCurveID = fields[i++].GetUInt32();
            info.ShapeID1 = fields[i++].GetInt32();
            info.ShapeID2 = fields[i++].GetInt32();
            info.HasUnkBit50 = fields[i++].GetBool();
            info.UnkBit50 = fields[i++].GetBool();
            info.HasFollowsTerrain = fields[i++].GetUInt32();
            info.HasAttached = fields[i++].GetUInt32();
            info.HasAbsoluteOrientation = fields[i++].GetUInt32();
            info.HasDynamicShape = fields[i++].GetUInt32();
            info.HasFaceMovementDir = fields[i++].GetUInt32();
            info.hasAreaTriggerBox = fields[i++].GetUInt32();
            info.RollPitchYaw1X = fields[i++].GetFloat();
            info.RollPitchYaw1Y = fields[i++].GetFloat();
            info.RollPitchYaw1Z = fields[i++].GetFloat();
            info.TargetRollPitchYawX = fields[i++].GetFloat();
            info.TargetRollPitchYawY = fields[i++].GetFloat();
            info.TargetRollPitchYawZ = fields[i++].GetFloat();
            info.DecalPropertiesId = fields[i++].GetUInt32();
            info.Distance = fields[i++].GetFloat();
            info.isMoving = fields[i++].GetBool();
            info.isCheckingWithObjectSize = fields[i++].GetBool();
            info.isCheckingNeighbours = fields[i++].GetBool();
            info.ScriptId = sObjectMgr->GetScriptId(fields[i++].GetCString());
            info.VisualID = fields[i++].GetUInt32();

            if (info.polygon && info.customEntry)
            {
                if (QueryResult resultPolygon = WorldDatabase.PQuery("SELECT `id`, `x`, `y` FROM areatrigger_polygon WHERE `entry` = '%u' AND `spellId` = '%u' AND `type` = 1 ORDER BY id ASC", info.customEntry, info.spellId))
                {
                    int32 l_LastId = -1;

                    do
                    {
                        Field* fieldP = resultPolygon->Fetch();
                        int32 l_ID = fieldP[0].GetUInt32();
                        if (++l_LastId != l_ID)
                        {
                            sLog->outError(LOG_FILTER_SQL, "Table `areatrigger_polygon` has non-consecutives vertices ID for polygon (entry = %u and spellId = %u)", info.customEntry, info.spellId);
                            info.Polygon.Vertices.clear();
                            break;
                        }

                        info.Polygon.Vertices[l_ID] = G3D::Vector2(fieldP[1].GetFloat(), fieldP[2].GetFloat());
                    } while (resultPolygon->NextRow());
                }

                if (QueryResult resultPolygonTarget = WorldDatabase.PQuery("SELECT `id`, `x`, `y` FROM areatrigger_polygon WHERE `entry` = '%u' AND `spellId` = '%u' AND `type` = 2 ORDER BY id ASC", info.customEntry, info.spellId))
                {
                    int32 l_LastId = -1;

                    do
                    {
                        Field* fieldP = resultPolygonTarget->Fetch();
                        int32 l_ID = fieldP[0].GetUInt32();
                        if (++l_LastId != l_ID)
                        {
                            sLog->outError(LOG_FILTER_SQL, "Table `areatrigger_polygon` has non-consecutives vertices target ID for polygon (entry = %u and spellId = %u)", info.customEntry, info.spellId);
                            info.Polygon.Vertices.clear();
                            break;
                        }

                        info.Polygon.VerticesTarget[l_ID] = G3D::Vector2(fieldP[1].GetFloat(), fieldP[2].GetFloat());
                    } while (resultPolygonTarget->NextRow());
                }
            }

            //Create polygon for Box
            if (!info.polygon && info.hasAreaTriggerBox)
            {
                info.polygon = 1;
                info.Polygon.Vertices[0] = G3D::Vector2(info.Radius, info.Polygon.Height);
                info.Polygon.Vertices[1] = G3D::Vector2(info.RadiusTarget, -info.Polygon.Height);
                info.Polygon.Vertices[2] = G3D::Vector2(-info.Radius, -info.Polygon.HeightTarget);
                info.Polygon.Vertices[3] = G3D::Vector2(-info.RadiusTarget, info.Polygon.HeightTarget);
            }

            if (info.windSpeed)
            {
                AreaTriggerAction actionEnter;
                actionEnter.id = 0;
                actionEnter.moment = AT_ACTION_MOMENT_ENTER;
                actionEnter.actionType = AT_ACTION_TYPE_APPLY_MOVEMENT_FORCE;
                actionEnter.targetFlags = AreaTriggerTargetFlags(AT_TARGET_FLAG_PLAYER);
                actionEnter.spellId = info.spellId;
                actionEnter.maxCharges = 0;
                actionEnter.chargeRecoveryTime = 0;
                actionEnter.aura = 0;
                actionEnter.hasspell = 0;
                actionEnter.scale = 0;
                actionEnter.hitMaxCount = 0;
                actionEnter.amount = 0;
                actionEnter.onDespawn = 0;
                info.actions.push_back(actionEnter);

                AreaTriggerAction actionLeave;
                actionLeave.id = 1;
                actionLeave.moment = AreaTriggerActionMoment(AT_ACTION_MOMENT_LEAVE | AT_ACTION_MOMENT_DESPAWN | AT_ACTION_MOMENT_REMOVE);
                actionLeave.actionType = AT_ACTION_TYPE_REMOVE_MOVEMENT_FORCE;
                actionLeave.targetFlags = AT_TARGET_FLAG_PLAYER;
                actionLeave.spellId = info.spellId;
                actionLeave.maxCharges = 0;
                actionLeave.chargeRecoveryTime = 0;
                actionLeave.aura = 0;
                actionLeave.hasspell = 0;
                actionLeave.scale = 0;
                actionLeave.hitMaxCount = 0;
                actionLeave.amount = 0;
                actionLeave.onDespawn = 0;
                info.actions.push_back(actionLeave);
            }

            if (id)
                _areaTriggerData.insert(std::make_pair(id, info));
            else
                _areaTriggerDataByEntry.insert(std::make_pair(customEntry, info));

            ++counter;
        }
        while (result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u areatrigger template in %u ms.", counter, GetMSTimeDiffToNow(l_oldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 areatrigger template. DB table `areatrigger_template` is empty.");

    result = WorldDatabase.Query("SELECT `entry`, `customEntry`, `spellId`, `moveType`, `speed`, `activationDelay`, `updateDelay`, `scalingDelay`, `maxCount`, `hitType`, `waitTime`, `AngleToCaster`, `maxActiveTargets`, `Param`, `timeToTarget` FROM areatrigger_data");
    if (result)
    {
        uint32 counter = 0;
        do
        {
            Field* fields = result->Fetch();

            uint8 i = 0;
            uint32 id = fields[i++].GetUInt32();
            uint32 customEntry = fields[i++].GetUInt32();
            uint32 spellId = fields[i++].GetUInt32();
            AreaTriggerInfo info;
            info.moveType = fields[i++].GetUInt32();
            info.speed = fields[i++].GetFloat();
            info.activationDelay = fields[i++].GetUInt32();
            info.updateDelay = fields[i++].GetUInt32();
            info.scalingDelay = fields[i++].GetUInt32();
            info.maxCount = fields[i++].GetUInt8();
            info.hitType = fields[i++].GetUInt32();
            info.waitTime = fields[i++].GetUInt32();
            info.AngleToCaster = fields[i++].GetFloat();
            info.maxActiveTargets = fields[i++].GetUInt8();
            info.Param = fields[i++].GetFloat();
            info.timeToTarget = fields[i++].GetUInt32();

            AreaTriggerInfo const* l_Info = GetAreaTriggerInfo(id ? id : customEntry, spellId);
            if (l_Info != nullptr)
            {
                const_cast<AreaTriggerInfo*>(l_Info)->moveType          = info.moveType;
                const_cast<AreaTriggerInfo*>(l_Info)->speed             = info.speed;
                const_cast<AreaTriggerInfo*>(l_Info)->activationDelay   = info.activationDelay;
                const_cast<AreaTriggerInfo*>(l_Info)->updateDelay       = info.updateDelay;
                const_cast<AreaTriggerInfo*>(l_Info)->scalingDelay      = info.scalingDelay;
                const_cast<AreaTriggerInfo*>(l_Info)->maxCount          = info.maxCount;
                const_cast<AreaTriggerInfo*>(l_Info)->hitType           = info.hitType;
                const_cast<AreaTriggerInfo*>(l_Info)->waitTime          = info.waitTime;
                const_cast<AreaTriggerInfo*>(l_Info)->AngleToCaster     = info.AngleToCaster;
                const_cast<AreaTriggerInfo*>(l_Info)->maxActiveTargets  = info.maxActiveTargets;
                const_cast<AreaTriggerInfo*>(l_Info)->Param             = info.Param;
                const_cast<AreaTriggerInfo*>(l_Info)->timeToTarget      = info.timeToTarget;
            }
            else if (id)
                _areaTriggerData.insert(std::make_pair(id, info));
            else
                _areaTriggerDataByEntry.insert(std::make_pair(customEntry, info));

            ++counter;
        }
        while (result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u areatrigger data in %u ms", counter, GetMSTimeDiffToNow(l_oldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 areatrigger data. DB table `areatrigger_data` is empty.");

    //                                                0         1                2        3     4           5         6            7           8               9         10       11     12         13        14       15
    QueryResult result2 = WorldDatabase.Query("SELECT entry, summon_spellId, customEntry, id, moment, actionType, targetFlags, spellId, maxCharges, chargeRecoveryTime, aura, hasspell, scale, hitMaxCount, amount, onDespawn FROM areatrigger_actions");
    if (result2)
    {
        uint32 counter = 0;
        do
        {
            Field* fields = result2->Fetch();

            uint8 i = 0;
            AreaTriggerAction action;
            uint32 entry = fields[i++].GetUInt32();
            uint32 l_SpellID = fields[i++].GetUInt32();
            uint32 customEntry = fields[i++].GetUInt32();
            action.id = fields[i++].GetUInt8();
            action.moment = (AreaTriggerActionMoment)fields[i++].GetUInt32();
            action.actionType = (AreaTriggerActionType)fields[i++].GetUInt8();
            action.targetFlags = (AreaTriggerTargetFlags)fields[i++].GetUInt32();
            action.spellId = fields[i++].GetUInt32();
            action.maxCharges = fields[i++].GetInt8();
            action.chargeRecoveryTime = fields[i++].GetUInt32();
            action.aura = fields[i++].GetInt32();
            action.hasspell = fields[i++].GetInt32();
            action.scale = fields[i++].GetFloat();
            action.hitMaxCount = fields[i++].GetInt32();
            action.amount = fields[i++].GetInt32();
            action.onDespawn = fields[i++].GetUInt8();

            if (action.actionType >= AT_ACTION_TYPE_MAX)
            {
                sLog->outError(LOG_FILTER_SQL, "DB table `areatrigger_actions` has invalid action type '%u' for areatrigger entry %u",
                    action.actionType, entry);
                continue;
            }

            if (action.actionType == AT_ACTION_TYPE_CHANGE_AMOUNT_FROM_HEALT && !(action.targetFlags & AT_TARGET_FLAG_NOT_FULL_HP))
                action.targetFlags = AreaTriggerTargetFlags(action.targetFlags | AT_TARGET_FLAG_NOT_FULL_HP);

            bool l_ActionPushed = false;

            for (uint8 l_I = Difficulty::DifficultyNone; l_I < Difficulty::MaxDifficulties; ++l_I)
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(action.spellId, Difficulty(l_I));
                if (action.actionType == AT_ACTION_TYPE_CAST_SPELL && !l_SpellInfo)
                {
                    sLog->outError(LOG_FILTER_SQL, "DB table `areatrigger_actions` has non-existant spell id '%u' for areatrigger entry %u",
                        action.spellId, entry);
                    continue;
                }

                if (l_SpellInfo)
                {
                    /// This is used for pets avoidance
                    const_cast<SpellInfo*>(l_SpellInfo)->AttributesCu |= SPELL_ATTR0_CU_IS_CUSTOM_AOE_SPELL;
                    /// This can prevent some debuffs to stay forever after leaving a map (battleground, raid etc.)
                    const_cast<SpellInfo*>(l_SpellInfo)->AuraInterruptFlags |= SpellAuraInterruptFlags::AURA_INTERRUPT_FLAG_CHANGE_MAP;
                }

                AreaTriggerInfo const* l_Info = entry ? GetAreaTriggerInfo(entry, l_SpellID) : GetAreaTriggerInfoByEntry(customEntry, l_SpellID);
                if (!l_ActionPushed && l_Info != nullptr)
                {
                    l_ActionPushed = true;
                    const_cast<AreaTriggerInfo*>(l_Info)->actions.push_back(action);
                }

                if (SpellInfo const* l_AreaTriggerSpell = sSpellMgr->GetSpellInfo(l_Info ? l_Info->spellId : 0, Difficulty(l_I)))
                {
                    if (l_SpellInfo && !l_SpellInfo->IsPositive())
                        const_cast<SpellInfo*>(l_AreaTriggerSpell)->AttributesCu |= SPELL_ATTR0_CU_NEGATIVE_AREATRIGGER;
                }
            }

            ++counter;
        }
        while (result2->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u areatrigger actions in %u ms.", counter, GetMSTimeDiffToNow(l_oldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 areatrigger actions. DB table `areatrigger_actions` is empty.");

    m_AreaTriggerMoveSplines.clear();
    //                                                      0                1          2        3        4
    QueryResult l_Result = WorldDatabase.Query("SELECT `move_curve_id`, `path_id`, `path_x`, `path_y`, `path_z` FROM `areatrigger_move_splines`");
    if (l_Result)
    {
        uint32 counter = 0;
        do
        {
            Field* l_Fields = l_Result->Fetch();
            uint8 l_Index = 0;

            AreaTriggerMoveSplines l_MoveSplines;
            l_MoveSplines.m_move_id = l_Fields[l_Index++].GetUInt32();
            l_MoveSplines.m_path_id = l_Fields[l_Index++].GetUInt32();
            l_MoveSplines.m_path_x = l_Fields[l_Index++].GetFloat();
            l_MoveSplines.m_path_y = l_Fields[l_Index++].GetFloat();
            l_MoveSplines.m_path_z = l_Fields[l_Index++].GetFloat();

            m_AreaTriggerMoveSplines.insert(std::make_pair(std::make_pair(l_MoveSplines.m_move_id, l_MoveSplines.m_path_id), l_MoveSplines));

            ++counter;
        }
        while (l_Result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u areatriger move splines in %u ms.", counter, GetMSTimeDiffToNow(l_oldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Areatrigger Move Splines.  DB table `areatrigger_move_splines` is empty.");

    m_AreaTriggerCircleData.clear();

    ///                                     0        1           2          3         4           6       7         8         9              11
    l_Result = WorldDatabase.Query("SELECT entry, CenterType, Clockwise, byte45, TimeToTarget, dword2C, dword30, Radius, BlendFormRadius, ZOffset FROM areatrigger_circle");
    if (l_Result)
    {
        uint32 l_Counter = 0;
        do
        {
            Field* l_Fields                 = l_Result->Fetch();
            uint8 l_Index                   = 0;

            uint32 l_Entry                  = l_Fields[l_Index++].GetUInt32();
            AreaTriggerCircle& l_CircleData = m_AreaTriggerCircleData[l_Entry];

            l_CircleData.Type               = eAreaTriggerCircleTypes(l_Fields[l_Index++].GetUInt8());
            l_CircleData.Clockwise          = l_Fields[l_Index++].GetBool();
            l_CircleData.byte45             = l_Fields[l_Index++].GetBool();
            l_CircleData.TimeToTarget       = l_Fields[l_Index++].GetUInt32();
            l_CircleData.dword2C            = l_Fields[l_Index++].GetUInt32();
            l_CircleData.dword30            = l_Fields[l_Index++].GetUInt32();
            l_CircleData.Radius             = l_Fields[l_Index++].GetFloat();
            l_CircleData.BlendFormRadius    = l_Fields[l_Index++].GetFloat();
            l_CircleData.ZOffset            = l_Fields[l_Index++].GetFloat();

            if (AreaTriggerInfo const* l_Info = GetAreaTriggerInfo(l_Entry))
                l_CircleData.InitialAngle = l_Info->AngleToCaster;

            l_CircleData.Initialized = true;

            ++l_Counter;
        }
        while (l_Result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Areatrigger Circle Data in %u ms.", l_Counter, GetMSTimeDiffToNow(l_oldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Areatrigger Circle Data. DB table `areatrigger_circle` is empty.");
}

AreaTriggerInfo const* ObjectMgr::GetAreaTriggerInfo(uint32 entry, uint32 p_SpellID /*= 0*/)
{
    if (!p_SpellID)
    {
        auto l_Iter = _areaTriggerData.find(entry);
        return l_Iter != _areaTriggerData.end() ? &l_Iter->second : nullptr;
    }
    else
    {
        auto l_Data = _areaTriggerData.equal_range(entry);
        for (auto l_Iter = l_Data.first; l_Iter != l_Data.second; ++l_Iter)
        {
            if (l_Iter->second.spellId == p_SpellID)
                return &l_Iter->second;
        }

        return nullptr;
    }
}

AreaTriggerInfo const* ObjectMgr::GetAreaTriggerInfoByEntry(uint32 entry, uint32 p_SpellID /*= 0*/)
{
    if (!p_SpellID)
    {
        auto l_Iter = _areaTriggerDataByEntry.find(entry);
        return l_Iter != _areaTriggerDataByEntry.end() ? &l_Iter->second : nullptr;
    }
    else
    {
        auto l_Data = _areaTriggerDataByEntry.equal_range(entry);
        for (auto l_Iter = l_Data.first; l_Iter != l_Data.second; ++l_Iter)
        {
            if (l_Iter->second.spellId == p_SpellID)
                return &l_Iter->second;
        }

        return nullptr;
    }
}

AreaTriggerCircle const* ObjectMgr::GetAreaTriggerCircle(uint32 p_Entry) const
{
    AreaTriggerCircleMap::const_iterator l_Iter = m_AreaTriggerCircleData.find(p_Entry);
    return l_Iter != m_AreaTriggerCircleData.end() ? &l_Iter->second : nullptr;
}

void ObjectMgr::LoadSpellAuraNotSaved()
{
    uint32 oldMSTime = getMSTime();

    m_spellAuraNotSavedStore.clear();                           // needed for reload case

    uint32 count = 0;

    //                                                  0
    QueryResult result = WorldDatabase.Query("SELECT `AuraID` FROM `spell_aura_not_saved`");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Spell Aura Not Saved in %u ms", GetMSTimeDiffToNow(oldMSTime));
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 l_SpellID = fields[0].GetUInt32();

        if (!sSpellMgr->GetSpellInfo(l_SpellID))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `spell_aura_not_saved`: Spell %u does not exist.", l_SpellID);
            continue;
        }

        m_spellAuraNotSavedStore.insert(l_SpellID);

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Spell Aura Not Saved in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPetStatInfo()
{
    uint32 l_OldMSTime = getMSTime();

    //                                                   0     1       2             3           4          5            6            7            8           9            10
    QueryResult l_Result = WorldDatabase.Query("SELECT entry, speed, powerstatbase, armor_coef, apsp_coef, health_coef, damage_coef, attackspeed, powertype, createpower, secondarystat_coef FROM pet_stats");
    if (!l_Result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 level pet stats definitions. DB table `pet_levelstats` is empty.");
        return;
    }

    /// - Load databases stats

    uint32 l_Count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();
        uint32 l_Index  = 0;
        uint32 l_Entry  = l_Fields[l_Index++].GetUInt32();

        PetStatInfo l_PetStat;
        l_PetStat.m_Speed             = l_Fields[l_Index++].GetFloat();
        l_PetStat.m_PowerStat         = PetStatInfo::PowerStatBase(l_Fields[l_Index++].GetUInt32());
        l_PetStat.m_ArmorCoef         = l_Fields[l_Index++].GetFloat();
        l_PetStat.m_APSPCoef          = l_Fields[l_Index++].GetFloat();
        l_PetStat.m_HealthCoef        = l_Fields[l_Index++].GetFloat();
        l_PetStat.m_DamageCoef        = l_Fields[l_Index++].GetFloat();
        l_PetStat.m_AttackSpeed       = l_Fields[l_Index++].GetFloat();
        l_PetStat.m_Power             = Powers(l_Fields[l_Index++].GetUInt32());
        l_PetStat.m_CreatePower       = l_Fields[l_Index++].GetFloat();
        l_PetStat.m_SecondaryStatCoef = l_Fields[l_Index++].GetFloat();

        m_PetInfoStore.insert(std::make_pair(std::move(l_Entry), std::move(l_PetStat)));

        ++l_Count;
    }
    while (l_Result->NextRow());

    /// - Create default stat for pet we don't have informations

    PetStatInfo l_DefaultPetStat;
    l_DefaultPetStat.m_Speed             = 1.14f;
    l_DefaultPetStat.m_PowerStat         = PetStatInfo::PowerStatBase::SpellPower;
    l_DefaultPetStat.m_ArmorCoef         = 1.0f;
    l_DefaultPetStat.m_APSPCoef          = 1.0f;
    l_DefaultPetStat.m_HealthCoef        = 0.7f;
    l_DefaultPetStat.m_DamageCoef        = 0.85f;
    l_DefaultPetStat.m_AttackSpeed       = 2.0f;
    l_DefaultPetStat.m_Power             = Powers::POWER_MANA;
    l_DefaultPetStat.m_CreatePower       = -1;
    l_DefaultPetStat.m_SecondaryStatCoef = 1.0f;

    m_PetInfoStore.insert(std::make_pair(0, std::move(l_DefaultPetStat)));

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u pet stats definitions in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::PlayerCreateInfoAddItemHelper(uint32 race_, uint32 class_, uint32 itemId, int32 count)
{
    if (count > 0)
        _playerInfo[race_][class_].item.push_back(PlayerCreateInfoItem(itemId, count));
    else
    {
        if (count < -1)
            sLog->outError(LOG_FILTER_SQL, "Invalid count %i specified on item %u be removed from original player create info (use -1)!", count, itemId);

        bool doneOne = false;
        for (uint32 i = 1; i < sCharStartOutfitStore.GetNumRows(); ++i)
        {
            if (CharStartOutfitEntry const* entry = sCharStartOutfitStore.LookupEntry(i))
            {
                if ((entry->RaceID == race_ && entry->ClassID == class_) || (entry->RaceID == race_ && entry->ClassID == class_ && entry->SexID == 1))
                {
                    bool found = false;
                    for (uint8 x = 0; x < MAX_OUTFIT_ITEMS; ++x)
                    {
                        if (entry->ItemId[x] > 0 && uint32(entry->ItemId[x]) == itemId)
                        {
                            found = true;
                            const_cast<CharStartOutfitEntry*>(entry)->ItemId[x] = 0;
                            break;
                        }
                    }

                    if (!found)
                        sLog->outError(LOG_FILTER_SQL, "Item %u specified to be removed from original create info not found in db2!", itemId);

                    if (!doneOne)
                        doneOne = true;
                    else
                        break;
                }
            }
        }
    }
}

void ObjectMgr::LoadPlayerInfo()
{
    // Load playercreate
    {
        uint32 oldMSTime = getMSTime();
        //                                                0     1      2    3        4          5           6
        QueryResult result = WorldDatabase.Query("SELECT race, class, map, zone, position_x, position_y, position_z, orientation FROM playercreateinfo");

        if (!result)
        {

            sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 player create definitions. DB table `playercreateinfo` is empty.");
            exit(1);
        }
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race  = fields[0].GetUInt8();
                uint32 current_class = fields[1].GetUInt8();
                uint32 mapId         = fields[2].GetUInt16();
                uint32 areaId        = fields[3].GetUInt32(); // zone
                float  positionX     = fields[4].GetFloat();
                float  positionY     = fields[5].GetFloat();
                float  positionZ     = fields[6].GetFloat();
                float  orientation   = fields[7].GetFloat();

                if (current_race >= MAX_RACES)
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong race %u in `playercreateinfo` table, ignoring.", current_race);
                    continue;
                }

                ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
                if (!rEntry)
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong race %u in `playercreateinfo` table, ignoring.", current_race);
                    continue;
                }

                if (current_class >= MAX_CLASSES)
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong class %u in `playercreateinfo` table, ignoring.", current_class);
                    continue;
                }

                if (!sChrClassesStore.LookupEntry(current_class))
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong class %u in `playercreateinfo` table, ignoring.", current_class);
                    continue;
                }

                // accept DB data only for valid position (and non instanceable)
                if (!MapManager::IsValidMapCoord(mapId, positionX, positionY, positionZ, orientation))
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong home position for class %u race %u pair in `playercreateinfo` table, ignoring.", current_class, current_race);
                    continue;
                }

                if (sMapStore.LookupEntry(mapId)->Instanceable())
                {
                    sLog->outError(LOG_FILTER_SQL, "Home position in instanceable map for class %u race %u pair in `playercreateinfo` table, ignoring.", current_class, current_race);
                    continue;
                }

                PlayerInfo* pInfo = &_playerInfo[current_race][current_class];

                pInfo->mapId       = mapId;
                pInfo->areaId      = areaId;
                pInfo->positionX   = positionX;
                pInfo->positionY   = positionY;
                pInfo->positionZ   = positionZ;
                pInfo->orientation = orientation;

                pInfo->displayId_m = rEntry->MaleDisplayID;
                pInfo->displayId_f = rEntry->FemaleDisplayID;

                ++count;
            }
            while (result->NextRow());

            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u player create definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Load playercreate items
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Player Create Items Data...");
    {
        uint32 oldMSTime = getMSTime();
        //                                                0     1      2       3
        QueryResult result = WorldDatabase.Query("SELECT race, class, itemid, amount FROM playercreateinfo_item");

        if (!result)
        {
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 custom player create items. DB table `playercreateinfo_item` is empty.");
        }
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt8();
                if (current_race >= MAX_RACES)
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong race %u in `playercreateinfo_item` table, ignoring.", current_race);
                    continue;
                }

                uint32 current_class = fields[1].GetUInt8();
                if (current_class >= MAX_CLASSES)
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong class %u in `playercreateinfo_item` table, ignoring.", current_class);
                    continue;
                }

                uint32 item_id = fields[2].GetUInt32();

                if (!GetItemTemplate(item_id))
                {
                    sLog->outError(LOG_FILTER_SQL, "Item id %u (race %u class %u) in `playercreateinfo_item` table but not listed in `item_template`, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                int32 amount   = fields[3].GetInt8();

                if (!amount)
                {
                    sLog->outError(LOG_FILTER_SQL, "Item id %u (class %u race %u) have amount == 0 in `playercreateinfo_item` table, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                if (!current_race || !current_class)
                {
                    uint32 min_race = current_race ? current_race : 1;
                    uint32 max_race = current_race ? current_race + 1 : MAX_RACES;
                    uint32 min_class = current_class ? current_class : 1;
                    uint32 max_class = current_class ? current_class + 1 : MAX_CLASSES;
                    for (uint32 r = min_race; r < max_race; ++r)
                        for (uint32 c = min_class; c < max_class; ++c)
                            PlayerCreateInfoAddItemHelper(r, c, item_id, amount);
                }
                else
                    PlayerCreateInfoAddItemHelper(current_race, current_class, item_id, amount);

                ++count;
            }
            while (result->NextRow());

            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u custom player create items in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Load playercreate skills
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Player Create Skill Data...");
    {
        uint32 oldMSTime = getMSTime();

        for (uint32 l_I = 0; l_I < sSkillRaceClassInfoStore.GetNumRows(); ++l_I)
        {
            SkillRaceClassInfoEntry const* l_RCInfo = sSkillRaceClassInfoStore.LookupEntry(l_I);

            if (!l_RCInfo)
                continue;

            if (l_RCInfo->Availability == 1)
            {
                for (uint32 l_RaceIndex = RACE_HUMAN; l_RaceIndex < MAX_RACES; ++l_RaceIndex)
                {
                    if (l_RCInfo->RaceMask == -1 || ((UI64LIT(1) << (l_RaceIndex - 1)) & l_RCInfo->RaceMask))
                    {
                        for (uint32 l_ClassIndex = CLASS_WARRIOR; l_ClassIndex < MAX_CLASSES; ++l_ClassIndex)
                        {
                            if (l_RCInfo->ClassMask == -1 || ((1 << (l_ClassIndex - 1)) & l_RCInfo->ClassMask))
                            {
                                if (PlayerInfo* l_PInfo = &_playerInfo[l_RaceIndex][l_ClassIndex])
                                    l_PInfo->skills.push_back(l_RCInfo);
                            }
                        }
                    }
                }
            }
        }

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded player create skills in %u ms", GetMSTimeDiffToNow(oldMSTime));
    }

    // Load playercreate spells
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Player Create Custom Spell Data...");
    {
        uint32 oldMSTime = getMSTime();

        QueryResult result = WorldDatabase.PQuery("SELECT racemask, classmask, Spell FROM playercreateinfo_spell_custom");

        if (!result)
        {
            sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 player create spells. DB table `playercreateinfo_spell_custom` is empty.");
        }
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();
                uint64 raceMask = fields[0].GetUInt64();
                uint32 classMask = fields[1].GetUInt32();
                uint32 spellId = fields[2].GetUInt32();

                if (raceMask != 0 && !(raceMask & RACEMASK_ALL_PLAYABLE))
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong RaceMask %lu in `playercreateinfo_spell_custom` table, ignoring.", raceMask);
                    continue;
                }

                if (classMask != 0 && !(classMask & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong class %u in `playercreateinfo_spell_custom` table, ignoring.", classMask);
                    continue;
                }

                for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
                {
                    if (raceMask == 0 || ((1LL << (raceIndex - 1)) & raceMask))
                    {
                        for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
                        {
                            if (classMask == 0 || ((1 << (classIndex - 1)) & classMask))
                            {
                                if (PlayerInfo* info = &_playerInfo[raceIndex][classIndex])
                                {
                                    info->customSpells.push_back(spellId);
                                    ++count;
                                }
                                // We need something better here, the check is not accounting for spells used by multiple races/classes but not all of them.
                                // Either split the masks per class, or per race, which kind of kills the point yet.
                                // else if (raceMask != 0 && classMask != 0)
                                //     sLog->outError(LOG_FILTER_SQL,  "Racemask/classmask (%u/%u) combination was found containing an invalid race/class combination (%u/%u) in `%s` (Spell %u), ignoring.", raceMask, classMask, raceIndex, classIndex, tableName.c_str(), spellId);
                            }
                        }
                    }
                }
            }
            while (result->NextRow());

            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u player create spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Load playercreate cast spell
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Player Create Cast Spell Data...");
    {
        uint32 oldMSTime = getMSTime();

        QueryResult result = WorldDatabase.PQuery("SELECT raceMask, classMask, spell FROM playercreateinfo_cast_spell");

        if (!result)
            sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 player create cast spells. DB table `playercreateinfo_cast_spell` is empty.");
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields       = result->Fetch();
                uint64 raceMask     = fields[0].GetUInt64();
                uint32 classMask    = fields[1].GetUInt32();
                uint32 spellId      = fields[2].GetUInt32();

                if (raceMask != 0 && !(raceMask & RACEMASK_ALL_PLAYABLE))
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong race mask %lu in `playercreateinfo_cast_spell` table, ignoring.", raceMask);
                    continue;
                }

                if (classMask != 0 && !(classMask & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong class mask %u in `playercreateinfo_cast_spell` table, ignoring.", classMask);
                    continue;
                }

                for (uint32 raceIndex = RACE_HUMAN; raceIndex < MAX_RACES; ++raceIndex)
                {
                    if (raceMask == 0 || ((1LL << (raceIndex - 1)) & raceMask))
                    {
                        for (uint32 classIndex = CLASS_WARRIOR; classIndex < MAX_CLASSES; ++classIndex)
                        {
                            if (classMask == 0 || ((1 << (classIndex - 1)) & classMask))
                            {
                                if (PlayerInfo* l_Info = &_playerInfo[raceIndex][classIndex])
                                {
                                    l_Info->castSpells.push_back(spellId);
                                    ++count;
                                }
                            }
                        }
                    }
                }
            } while (result->NextRow());

            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u player create cast spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    // Load playercreate actions
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Player Create Action Data...");
    {
        uint32 oldMSTime = getMSTime();

        //                                                0     1      2       3       4
        QueryResult result = WorldDatabase.Query("SELECT race, class, button, action, type FROM playercreateinfo_action");

        if (!result)
        {
            sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 player create actions. DB table `playercreateinfo_action` is empty.");

        }
        else
        {
            uint32 count = 0;

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt8();
                if (current_race >= MAX_RACES)
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong race %u in `playercreateinfo_action` table, ignoring.", current_race);
                    continue;
                }

                uint32 current_class = fields[1].GetUInt8();
                if (current_class >= MAX_CLASSES)
                {
                    sLog->outError(LOG_FILTER_SQL, "Wrong class %u in `playercreateinfo_action` table, ignoring.", current_class);
                    continue;
                }

                PlayerInfo* pInfo = &_playerInfo[current_race][current_class];
                pInfo->action.push_back(PlayerCreateInfoAction(fields[2].GetUInt16(), fields[3].GetUInt32(), fields[4].GetUInt16()));

                ++count;
            }
            while (result->NextRow());

            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u player create actions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
        }
    }

    /// Loading levels data (class/race dependent)
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Player Create Level Stats Data...");
    {
        uint32 oldMSTime = getMSTime();

        ///                                                0     1      2      3    4    5    6
        QueryResult result  = WorldDatabase.Query("SELECT race, class, level, str, agi, sta, inte FROM player_levelstats");

        if (!result)
        {
            sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 level stats definitions. DB table `player_levelstats` is empty.");

            exit(1);
        }

        uint32 count = 0;

        do
        {
            Field* fields = result->Fetch();

            uint32 current_race = fields[0].GetUInt8();
            if (current_race >= MAX_RACES)
            {
                sLog->outError(LOG_FILTER_SQL, "Wrong race %u in `player_levelstats` table, ignoring.", current_race);
                continue;
            }

            uint32 current_class = fields[1].GetUInt8();
            if (current_class >= MAX_CLASSES)
            {
                sLog->outError(LOG_FILTER_SQL, "Wrong class %u in `player_levelstats` table, ignoring.", current_class);
                continue;
            }

            uint32 current_level = fields[2].GetUInt8();
            if (current_level > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)        ///< Hardcoded level maximum
                    sLog->outError(LOG_FILTER_SQL, "Wrong (> %u) level %u in `player_levelstats` table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                {
                    sLog->outInfo(LOG_FILTER_GENERAL, "Unused (> MaxPlayerLevel in worldserver.conf) level %u in `player_levelstats` table, ignoring.", current_level);
                    ++count;                                ///< Make result loading percent "expected" correct in case disabled detail mode for example.
                }
                continue;
            }

            PlayerInfo* pInfo = &_playerInfo[current_race][current_class];

            if (!pInfo->levelInfo)
                pInfo->levelInfo = new PlayerLevelInfo[sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)];

            PlayerLevelInfo* pLevelInfo = &pInfo->levelInfo[current_level-1];

            for (int i = 0; i < MAX_STATS; i++)
            {
                pLevelInfo->stats[i] = fields[i+3].GetUInt16();
            }

            ++count;
        }
        while (result->NextRow());

        /// Fill gaps and check integrity
        for (int race = 0; race < MAX_RACES; ++race)
        {
            /// Skip non existed races
            if (!sChrRacesStore.LookupEntry(race))
                continue;

            for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
            {
                /// Skip non existed classes
                if (!sChrClassesStore.LookupEntry(class_))
                    continue;

                PlayerInfo* pInfo = &_playerInfo[race][class_];

                /// Skip non loaded combinations
                if (!pInfo->displayId_m || !pInfo->displayId_f)
                    continue;

                /// Skip expansion races if not playing with expansion
                if (sWorld->getIntConfig(CONFIG_EXPANSION) < EXPANSION_THE_BURNING_CRUSADE && (race == RACE_BLOODELF || race == RACE_DRAENEI))
                    continue;

                /// Skip expansion classes if not playing with expansion
                if (sWorld->getIntConfig(CONFIG_EXPANSION) < EXPANSION_WRATH_OF_THE_LICH_KING && class_ == CLASS_DEATH_KNIGHT)
                    continue;

                /// Skip expansion classes / races if not playing with expansion
                if (sWorld->getIntConfig(CONFIG_EXPANSION) < EXPANSION_CATACLYSM && (race == RACE_GOBLIN || race == RACE_WORGEN))
                    continue;

                /// Skip expansion classes / races if not playing with expansion
                if (sWorld->getIntConfig(CONFIG_EXPANSION) < EXPANSION_MISTS_OF_PANDARIA && (class_ == CLASS_MONK || race == RACE_PANDAREN_NEUTRAL || race == RACE_PANDAREN_ALLIANCE || race == RACE_PANDAREN_HORDE))
                    continue;

                if (sWorld->getIntConfig(CONFIG_EXPANSION) < EXPANSION_LEGION && class_ == CLASS_DEMON_HUNTER)
                    continue;

                /// Fatal error if no level 1 data
                if (!pInfo->levelInfo || pInfo->levelInfo[0].stats[0] == 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Race %i Class %i Level 1 does not have stats data!", race, class_);
                    exit(1);
                }

                /// Fill level gaps
                for (uint8 level = 1; level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
                {
                    if (pInfo->levelInfo[level].stats[0] == 0)
                    {
                        sLog->outError(LOG_FILTER_SQL, "Race %i Class %i Level %i does not have stats data. Using stats data of level %i.", race, class_, level+1, level);
                        pInfo->levelInfo[level] = pInfo->levelInfo[level-1];
                    }
                }
            }
        }

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u level stats definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    }

    // Loading xp per level data
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Player Create XP Data...");
    {
        uint32 oldMSTime = getMSTime();

        _playerXPperLevel.resize(g_XPTable.GetTableRowCount() + 1, 0);

        for (auto xpItr : g_XPTable.GetData())
        {
            if (xpItr != nullptr)
                _playerXPperLevel[xpItr->m_Level] = xpItr->m_Required;
        }

        //                                                 0        1
        QueryResult result = WorldDatabase.Query("SELECT Level, Experience FROM player_xp_for_level");
        uint32 count = 0;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();

                uint32 current_level = fields[0].GetUInt8();
                uint32 current_xp    = fields[1].GetUInt32();

                if (current_level >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
                {
                    if (current_level > STRONG_MAX_LEVEL)        // hardcoded level maximum
                        sLog->outError(LOG_FILTER_SQL, "Wrong (> %u) level %u in `g_XPTable` table, ignoring.", STRONG_MAX_LEVEL, current_level);
                    else
                    {
                        sLog->outInfo(LOG_FILTER_GENERAL, "Unused (> MaxPlayerLevel in worldserver.conf) level %u in `g_XPTable` table, ignoring.", current_level);
                        ++count;                                // make result loading percent "expected" correct in case disabled detail mode for example.
                    }
                    continue;
                }

                _playerXPperLevel[current_level] = current_xp;
                ++count;
            } while (result->NextRow());
        }

        // fill level gaps - only accounting levels > MAX_LEVEL
        for (uint8 level = 1; level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
        {
            if (_playerXPperLevel[level] == 0)
            {
                sLog->outInfo(LOG_FILTER_GENERAL, "Level %i does not have XP for level data. Using data of level [%i] + 12000.", level + 1, level);
                _playerXPperLevel[level] = _playerXPperLevel[level - 1] + 12000;
            }
        }

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u xp for level definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
    }
}

PetStatInfo const* ObjectMgr::GetPetStatInfo(uint32 p_Entry) const
{
    PetStatInfoContainer::const_iterator l_Iterator = m_PetInfoStore.find(p_Entry);
    if (l_Iterator != m_PetInfoStore.end())
        return &l_Iterator->second;

    return nullptr;
}

void ObjectMgr::GetPlayerClassLevelInfo(uint32 class_, uint8 level, uint32& baseHP, uint32& baseMana) const
{
    if (level < 1 || class_ >= MAX_CLASSES)
        return;

    BaseManaTableEntry const* mp = g_BaseManaTable.LookupEntry(level);

    if (!mp)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Tried to get non-existant Class-Level combination data for base hp/mp. Class %u Level %u", class_, level);
        return;
    }

    baseMana = uint32(mp->m_ClassScaling.GetScalingForClassID(class_));

}

void ObjectMgr::GetPlayerLevelInfo(uint32 race, uint32 class_, uint8 level, PlayerLevelInfo* info) const
{
    if (level < 1 || race >= MAX_RACES || class_ >= MAX_CLASSES)
        return;

    PlayerInfo const* pInfo = &_playerInfo[race][class_];
    if (pInfo->displayId_m == 0 || pInfo->displayId_f == 0)
        return;

    if (level <= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        *info = pInfo->levelInfo[level-1];
    else
        BuildPlayerLevelInfo(race, class_, level, info);
}

/// @TODOLEGION the data are now in GameTable remove this
void ObjectMgr::BuildPlayerLevelInfo(uint8 race, uint8 _class, uint8 level, PlayerLevelInfo* info) const
{
    // base data (last known level)
    *info = _playerInfo[race][_class].levelInfo[sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)-1];

    // if conversion from uint32 to uint8 causes unexpected behaviour, change lvl to uint32
    for (uint8 lvl = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)-1; lvl < level; ++lvl)
    {
        switch (_class)
        {
            case CLASS_WARRIOR:
                info->stats[STAT_STRENGTH]  += (lvl > 23 ? 2: (lvl > 1  ? 1: 0));
                info->stats[STAT_STAMINA]   += (lvl > 23 ? 2: (lvl > 1  ? 1: 0));
                info->stats[STAT_AGILITY]   += (lvl > 36 ? 1: (lvl > 6 && (lvl%2) ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 9 && !(lvl%2) ? 1: 0);
                break;
            case CLASS_PALADIN:
                info->stats[STAT_STRENGTH]  += (lvl > 3  ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 33 ? 2: (lvl > 1 ? 1: 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 1: (lvl > 7 && !(lvl%2) ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 6 && (lvl%2) ? 1: 0);
                break;
            case CLASS_HUNTER:
                info->stats[STAT_STRENGTH]  += (lvl > 4  ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 4  ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 33 ? 2: (lvl > 1 ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 8 && (lvl%2) ? 1: 0);
                break;
            case CLASS_ROGUE:
            case CLASS_DEMON_HUNTER:
                info->stats[STAT_STRENGTH]  += (lvl > 5  ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 4  ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 16 ? 2: (lvl > 1 ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 8 && !(lvl%2) ? 1: 0);
                break;
            case CLASS_PRIEST:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 5  ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 1: (lvl > 8 && (lvl%2) ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 22 ? 2 : (lvl > 1 ? 1 : 0));
                break;
            case CLASS_SHAMAN:
                info->stats[STAT_STRENGTH]  += (lvl > 34 ? 1: (lvl > 6 && (lvl%2) ? 1: 0));
                info->stats[STAT_STAMINA]   += (lvl > 4 ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 7 && !(lvl%2) ? 1: 0);
                info->stats[STAT_INTELLECT] += (lvl > 5 ? 1: 0);
                break;
            case CLASS_MAGE:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 5  ? 1: 0);
                info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_INTELLECT] += (lvl > 24 ? 2: (lvl > 1 ? 1: 0));
                break;
            case CLASS_WARLOCK:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_STAMINA]   += (lvl > 38 ? 2: (lvl > 3 ? 1: 0));
                info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl%2) ? 1: 0);
                info->stats[STAT_INTELLECT] += (lvl > 33 ? 2: (lvl > 2 ? 1: 0));
                break;
            case CLASS_DRUID:
                info->stats[STAT_STRENGTH]  += (lvl > 38 ? 2: (lvl > 6 && (lvl%2) ? 1: 0));
                info->stats[STAT_STAMINA]   += (lvl > 32 ? 2: (lvl > 4 ? 1: 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 2: (lvl > 8 && (lvl%2) ? 1: 0));
                info->stats[STAT_INTELLECT] += (lvl > 38 ? 3: (lvl > 4 ? 1: 0));
            case CLASS_MONK:
                info->stats[STAT_STRENGTH]  += (lvl > 38 ? 2 : (lvl > 6 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_STAMINA]   += (lvl > 32 ? 2 : (lvl > 4 ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 2 : (lvl > 8 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 38 ? 3 : (lvl > 4 ? 1 : 0));
                break;
        }
    }
}

/* Need to add new fields
        RequiredFactionKills        after RequiredFactionValue2
        RequiredFactionKillsCount   after RequiredFactionKills
        RequiredPetBattleWith       after RequiredFactionKillsCount
        RewardXPId                  after RewardXPId
        move EndText                after RequestItemsText
        Add RequiredNpcOrGo 5 to 10 after RequiredNpcOrGo4
        RequiredNpcOrGoCount 5 to 10 after RequiredNpcOrGoCount4
        RequiredItemId 7 to 10      after RequiredItemId6
        RequiredItemCount 7 to 10   after RequiredItemCount6
        RequiredSpellCast 5 to 6    after RequiredSpellCast4
        ObjectiveText 5 to 10       after ObjectiveText4
        remove QuestGiverTargetName
*/
void ObjectMgr::LoadQuests()
{
    uint32 oldMSTime = getMSTime();

    bool l_Reload = !_questTemplates.empty();

    // For reload case
    for (QuestMap::const_iterator itr=_questTemplates.begin(); itr != _questTemplates.end(); ++itr)
        delete itr->second;

    _questTemplates.clear();

    mExclusiveQuestGroups.clear();

    QueryResult result = WorldDatabase.Query("SELECT "
        "Id, Method, Level, MaxScalingLevel, MinLevel, MaxLevel, PackageID, ZoneOrSort, Type, InProgressPhaseID, CompletedPhaseID, SuggestedPlayers, LimitTime, AreaGroupId, QuestRewardID, Expansion, RequiredTeam, RequiredClasses, RequiredRaces, RequiredSkillId, RequiredSkillPoints, "
        "RequiredMinRepFaction, RequiredMaxRepFaction, RequiredMinRepValue, RequiredMaxRepValue, "
        "PrevQuestId, NextQuestId, ExclusiveGroup, NextQuestIdChain, RewardXPId, RewardXPMultiplier, RewardMoney, RewardMoneyDifficulty, RewardMoneyMultiplier, RewardMoneyMaxLevel, RewardSpell1, RewardSpell2, RewardSpell3, RewardSpellCast, RewardHonor, RewardHonorMultiplier, RewardArtifactXP, RewardArtifactXPMultiplier, RewardArtifactXPDifficulty,"
        "RewardMailTemplateId, RewardMailDelay, SourceItemId, SourceSpellId, Flags, Flags2, SpecialFlags, MinimapTargetMark, RewardTitleId, RewardTalents, "
        "RewardSkillId, RewardSkillPoints, RewardReputationMask, QuestGiverPortrait, QuestTurnInPortrait, RewardItemId1, RewardItemId2, RewardItemId3, RewardItemId4, RewardItemCount1, RewardItemCount2, RewardItemCount3, RewardItemCount4, "
        "RewardChoiceItemId1, RewardChoiceItemId2, RewardChoiceItemId3, RewardChoiceItemId4, RewardChoiceItemId5, RewardChoiceItemId6, RewardChoiceItemCount1, RewardChoiceItemCount2, RewardChoiceItemCount3, RewardChoiceItemCount4, RewardChoiceItemCount5, RewardChoiceItemCount6, "
        "RewardFactionId1, RewardFactionId2, RewardFactionId3, RewardFactionId4, RewardFactionId5, RewardFactionValueId1, RewardFactionValueId2, RewardFactionValueId3, RewardFactionValueId4, RewardFactionValueId5, "
        "RewardFactionValueIdOverride1, RewardFactionValueIdOverride2, RewardFactionValueIdOverride3, RewardFactionValueIdOverride4, RewardFactionValueIdOverride5, RewardFactionCapIn1, RewardFactionCapIn2, RewardFactionCapIn3, RewardFactionCapIn4, RewardFactionCapIn5, "
        "PointMapId, PointX, PointY, PointOption, Title, Objectives, Details, EndText, CompletedText, OfferRewardText, RequestItemsText, "
        "RequiredSourceItemId1, RequiredSourceItemId2, RequiredSourceItemId3, RequiredSourceItemId4, RequiredSourceItemCount1, RequiredSourceItemCount2, RequiredSourceItemCount3, RequiredSourceItemCount4, "
        "RewardCurrencyId1, RewardCurrencyId2, RewardCurrencyId3, RewardCurrencyId4, RewardCurrencyCount1, RewardCurrencyCount2, RewardCurrencyCount3, RewardCurrencyCount4, "
        "QuestGiverTextWindow, QuestGiverTargetName, QuestTurnTextWindow, QuestTurnTargetName, SoundAccept, SoundTurnIn, "
        "DetailsEmote1, DetailsEmote2, DetailsEmote3, DetailsEmote4, DetailsEmoteDelay1, DetailsEmoteDelay2, DetailsEmoteDelay3, DetailsEmoteDelay4, EmoteOnIncomplete, EmoteOnComplete, "
        "OfferRewardEmote1, OfferRewardEmote2, OfferRewardEmote3, OfferRewardEmote4, OfferRewardEmoteDelay1, OfferRewardEmoteDelay2, OfferRewardEmoteDelay3, OfferRewardEmoteDelay4, "
        "StartScript, CompleteScript, BuildVerified"
        " FROM quest_template");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 quests definitions. DB table `quest_template` is empty.");

        return;
    }

    // Create multimap previous quest for each existed quest
    // some quests can have many previous maps set by NextQuestId in previous quest
    // for example set of race quests can lead to single not race specific quest
    do
    {
        Field* fields = result->Fetch();

        Quest* newQuest = new Quest(fields);
        _questTemplates[newQuest->GetQuestId()] = newQuest;

        if (newQuest->Method == QUEST_METHOD_AUTO_SUBMITED && newQuest->AreaGroupId > 0)
        {
            auto areaGroupMembers = GetAreasForGroup(newQuest->AreaGroupId);
            for (auto& areaId : areaGroupMembers)
            {
                auto legionMap = false;
                if (auto areaTable = sAreaTableStore.LookupEntry(areaId))
                    legionMap = areaTable->ContinentID == 1220;
                if ((!legionMap && newQuest->Type == QUEST_INFO_PVP_WORLD_QUEST) || !(newQuest->Type >= QUEST_INFO_WORLD_QUEST && newQuest->Type <= QUEST_INFO_MAX))
                {
                    _questAreaTaskStore[areaId].insert(newQuest);
                    _questAreaTaskQuests.insert(newQuest->Id);
                }
            }
        }
    }
    while (result->NextRow());

    std::map<uint32, uint32> usedMailTemplates;

    std::list<uint32> l_QuestToRemove;

    // Post processing
    for (QuestMap::iterator iter = _questTemplates.begin(); iter != _questTemplates.end(); ++iter)
    {
        // Skip post-loading checks for disabled quests
        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_QUEST, iter->first, NULL))
            continue;

        Quest* qinfo = iter->second;

        // Additional quest integrity checks (GO, creature_template and item_template must be loaded already)
        if (qinfo->GetQuestMethod() >= 4)
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `Method` = %u, expected values are 0, 1, 2 or 3.", qinfo->GetQuestId(), qinfo->GetQuestMethod());

        if (qinfo->IsAutoComplete() && qinfo->IsRepeatable())
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u is auto-complete and is repeatable", qinfo->GetQuestId());
            l_QuestToRemove.push_back(iter->first);
            continue;
        }

        if (qinfo->SpecialFlags & ~QUEST_SPECIAL_FLAGS_DB_ALLOWED)
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `SpecialFlags` = %u > max allowed value. Correct `SpecialFlags` to value <= %u",
                qinfo->GetQuestId(), qinfo->SpecialFlags, QUEST_SPECIAL_FLAGS_DB_ALLOWED);
            qinfo->SpecialFlags &= QUEST_SPECIAL_FLAGS_DB_ALLOWED;
        }

        if (qinfo->Flags & QUEST_FLAGS_DAILY && qinfo->Flags & QUEST_FLAGS_WEEKLY)
        {
            sLog->outError(LOG_FILTER_SQL, "Weekly Quest %u is marked as daily quest in `Flags`, removed daily flag.", qinfo->GetQuestId());
            qinfo->Flags &= ~QUEST_FLAGS_DAILY;
        }

        if (qinfo->Flags & QUEST_FLAGS_DAILY)
        {
            if (!(qinfo->SpecialFlags & QUEST_SPECIAL_FLAGS_REPEATABLE))
            {
                sLog->outError(LOG_FILTER_SQL, "Daily Quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->SpecialFlags |= QUEST_SPECIAL_FLAGS_REPEATABLE;
            }
        }

        if (qinfo->Flags & QUEST_FLAGS_WEEKLY)
        {
            if (!(qinfo->SpecialFlags & QUEST_SPECIAL_FLAGS_REPEATABLE))
            {
                sLog->outError(LOG_FILTER_SQL, "Weekly Quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->SpecialFlags |= QUEST_SPECIAL_FLAGS_REPEATABLE;
            }
        }

        if (qinfo->Flags & QUEST_SPECIAL_FLAGS_MONTHLY)
        {
            if (!(qinfo->Flags & QUEST_SPECIAL_FLAGS_REPEATABLE))
            {
                sLog->outError(LOG_FILTER_SQL, "Monthly quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->Flags |= QUEST_SPECIAL_FLAGS_REPEATABLE;
            }
        }

        if (qinfo->Flags & QUEST_FLAGS_AUTO_REWARDED)
        {
            // At auto-reward can be rewarded only RewardChoiceItemId[0]
            for (int j = 1; j < QUEST_REWARD_CHOICES_COUNT; ++j)
            {
                if (uint32 id = qinfo->RewardChoiceItemId[j])
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardChoiceItemId%d` = %u but item from `RewardChoiceItemId%d` can't be rewarded with quest flag QUEST_FLAGS_AUTO_REWARDED.",
                        qinfo->GetQuestId(), j + 1, id, j + 1);
                    // No changes, quest ignore this data
                }
            }
        }

        if (qinfo->MinLevel == uint32(-1) || qinfo->MinLevel > MAX_LEVEL)
        {
            /// We dont need this log, blizzard send this on data.
            ///sLog->outError(LOG_FILTER_SQL, "Quest %u should be disabled because `MinLevel` = %i", qinfo->GetQuestId(), int32(qinfo->MinLevel));
            // No changes needed, sending -1 in SMSG_QUEST_QUERY_RESPONSE is valid
        }

        // Client quest log visual (area case)
        if (qinfo->ZoneOrSort > 0)
        {
            if (!sAreaTableStore.LookupEntry(qinfo->ZoneOrSort))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `ZoneOrSort` = %u (zone case) but zone with this id does not exist.",
                    qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // No changes, quest not dependent from this value but can have problems at client
            }
        }
        // Client quest log visual (sort case)
        if (qinfo->ZoneOrSort < 0)
        {
            QuestSortEntry const* qSort = sQuestSortStore.LookupEntry(-int32(qinfo->ZoneOrSort));
            if (!qSort)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `ZoneOrSort` = %i (sort case) but quest sort with this id does not exist.",
                    qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // No changes, quest not dependent from this value but can have problems at client (note some may be 0, we must allow this so no check)
            }
            // Check for proper RequiredSkillId value (skill case)
            if (uint32 skill_id = SkillByQuestSort(-int32(qinfo->ZoneOrSort)))
            {
                if (qinfo->RequiredSkillId != skill_id)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `ZoneOrSort` = %i but `RequiredSkillId` does not have a corresponding value (%d).",
                        qinfo->GetQuestId(), qinfo->ZoneOrSort, skill_id);
                    // Override, and force proper value here?
                }
            }
        }

        // RequiredClasses, can be 0/CLASSMASK_ALL_PLAYABLE to allow any class
        if (qinfo->RequiredClasses)
        {
            if (!(qinfo->RequiredClasses & CLASSMASK_ALL_PLAYABLE))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u does not contain any playable classes in `RequiredClasses` (%u), value set to 0 (all classes).", qinfo->GetQuestId(), qinfo->RequiredClasses);
                qinfo->RequiredClasses = 0;
            }
        }
        // RequiredRaces, can be 0/RACEMASK_ALL_PLAYABLE to allow any race
        if (qinfo->RequiredRaces)
        {
            if (!(qinfo->RequiredRaces & RACEMASK_ALL_PLAYABLE))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u does not contain any playable races in `RequiredRaces` (%ld), value set to 0 (all races).", qinfo->GetQuestId(), qinfo->RequiredRaces);
                qinfo->RequiredRaces = 0;
            }
        }
        // RequiredSkillId, can be 0
        if (qinfo->RequiredSkillId)
        {
            if (!sSkillLineStore.LookupEntry(qinfo->RequiredSkillId))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredSkillId` = %u but this skill does not exist",
                    qinfo->GetQuestId(), qinfo->RequiredSkillId);
            }
        }

        if (qinfo->RequiredSkillPoints)
        {
            if (qinfo->RequiredSkillPoints > sWorld->GetConfigMaxSkillValue())
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredSkillPoints` = %u but max possible skill is %u, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->RequiredSkillPoints, sWorld->GetConfigMaxSkillValue());
                // No changes, quest can't be done for this requirement
            }
        }
        // Else Skill quests can have 0 skill level, this is ok

        if (qinfo->RequiredMinRepFaction && !sFactionStore.LookupEntry(qinfo->RequiredMinRepFaction))
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredMinRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RequiredMinRepFaction, qinfo->RequiredMinRepFaction);
            // No changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMaxRepFaction && !sFactionStore.LookupEntry(qinfo->RequiredMaxRepFaction))
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredMaxRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RequiredMaxRepFaction, qinfo->RequiredMaxRepFaction);
            // No changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMinRepValue > ReputationMgr::Reputation_Cap)
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredMinRepValue` = %d but max reputation is %u, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RequiredMinRepValue, ReputationMgr::Reputation_Cap);
            // No changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMaxRepValue && qinfo->RequiredMaxRepValue <= qinfo->RequiredMinRepValue)
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredMaxRepValue` = %d and `RequiredMinRepValue` = %d, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RequiredMaxRepValue, qinfo->RequiredMinRepValue);
            // No changes, quest can't be done for this requirement
        }

        if (!qinfo->RequiredMinRepFaction && qinfo->RequiredMinRepValue != 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredMinRepValue` = %d but `RequiredMinRepFaction` is 0, value has no effect",
                qinfo->GetQuestId(), qinfo->RequiredMinRepValue);
            // Warning
        }

        if (!qinfo->RequiredMaxRepFaction && qinfo->RequiredMaxRepValue != 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredMaxRepValue` = %d but `RequiredMaxRepFaction` is 0, value has no effect",
                qinfo->GetQuestId(), qinfo->RequiredMaxRepValue);
            // Warning
        }

        if (qinfo->RewardTitleId && !sCharTitlesStore.LookupEntry(qinfo->RewardTitleId))
        {
            sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardTitleId` = %u but CharTitle Id %u does not exist, quest can't be rewarded with title.",
                qinfo->GetQuestId(), qinfo->GetCharTitleId(), qinfo->GetCharTitleId());
            qinfo->RewardTitleId = 0;
            // Quest can't reward this title
        }

        if (qinfo->SourceItemId)
        {
            if (!sObjectMgr->GetItemTemplate(qinfo->SourceItemId))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `SourceItemId` = %u but item with entry %u does not exist, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->SourceItemId, qinfo->SourceItemId);
                qinfo->SourceItemId = 0;                       // Qquest can't be done for this requirement
            }
        }

        if (qinfo->SourceSpellid)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(qinfo->SourceSpellid);
            if (!spellInfo)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `SourceSpellid` = %u but spell %u doesn't exist, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->SourceSpellid, qinfo->SourceSpellid);
                qinfo->SourceSpellid = 0;                      // Quest can't be done for this requirement
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `SourceSpellid` = %u but spell %u is broken, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->SourceSpellid, qinfo->SourceSpellid);
                qinfo->SourceSpellid = 0;                      // Quest can't be done for this requirement
            }
        }

        for (uint8 j = 0; j < QUEST_SOURCE_ITEM_IDS_COUNT; ++j)
        {
            uint32 id = qinfo->RequiredSourceItemId[j];
            if (id)
            {
                if (!sObjectMgr->GetItemTemplate(id))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredSourceItemId%d` = %u but item with entry %u does not exist, quest can't be done.",
                        qinfo->GetQuestId(), j+1, id, id);
                    // No changes, quest can't be done for this requirement
                }
            }
            else
            {
                if (qinfo->RequiredSourceItemCount[j]>0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RequiredSourceItemId%d` = 0 but `RequiredSourceItemCount%d` = %u.",
                        qinfo->GetQuestId(), j+1, j+1, qinfo->RequiredSourceItemCount[j]);
                    // No changes, quest ignore this data
                }
            }
        }

        for (uint8 j = 0; j < QUEST_REWARD_CHOICES_COUNT; ++j)
        {
            uint32 id = qinfo->RewardChoiceItemId[j];
            if (id)
            {
                if (!sObjectMgr->GetItemTemplate(id))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardChoiceItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                        qinfo->GetQuestId(), j+1, id, id);
                    qinfo->RewardChoiceItemId[j] = 0;          // No changes, quest will not reward this
                }

                if (!qinfo->RewardChoiceItemCount[j])
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardChoiceItemId%d` = %u but `RewardChoiceItemCount%d` = 0, quest can't be done.",
                        qinfo->GetQuestId(), j+1, id, j+1);
                    // No changes, quest can't be done
                }
            }
            else if (qinfo->RewardChoiceItemCount[j]>0)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardChoiceItemId%d` = 0 but `RewardChoiceItemCount%d` = %u.",
                    qinfo->GetQuestId(), j+1, j+1, qinfo->RewardChoiceItemCount[j]);
                // No changes, quest ignore this data
            }
        }

        for (uint8 j = 0; j < QUEST_REWARDS_COUNT; ++j)
        {
            uint32 id = qinfo->RewardItemId[j];
            if (id)
            {
                if (!sObjectMgr->GetItemTemplate(id))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                        qinfo->GetQuestId(), j+1, id, id);
                    qinfo->RewardItemId[j] = 0;                // No changes, quest will not reward this item
                }

                if (!qinfo->RewardItemIdCount[j])
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardItemId%d` = %u but `RewardItemCount%d` = 0, quest will not reward this item.",
                        qinfo->GetQuestId(), j+1, id, j+1);
                    // No changes
                }
            }
            else if (qinfo->RewardItemIdCount[j]>0)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardItemId%d` = 0 but `RewardItemCount%d` = %u.",
                    qinfo->GetQuestId(), j+1, j+1, qinfo->RewardItemIdCount[j]);
                // No changes, quest ignore this data
            }
        }

        for (uint8 j = 0; j < QUEST_REPUTATIONS_COUNT; ++j)
        {
            if (qinfo->RewardFactionId[j])
            {
                if (abs(qinfo->RewardFactionValueId[j]) > 9)
                {
               sLog->outError(LOG_FILTER_SQL, "Quest %u has RewardFactionValueId%d = %i. That is outside the range of valid values (-9 to 9).", qinfo->GetQuestId(), j+1, qinfo->RewardFactionValueId[j]);
                }
                if (!sFactionStore.LookupEntry(qinfo->RewardFactionId[j]))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardFactionId%d` = %u but raw faction (faction.db2) %u does not exist, quest will not reward reputation for this faction.", qinfo->GetQuestId(), j+1, qinfo->RewardFactionId[j], qinfo->RewardFactionId[j]);
                    qinfo->RewardFactionId[j] = 0;             // Quest will not reward this
                }
            }

            else if (qinfo->RewardFactionValueIdOverride[j] != 0)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardFactionId%d` = 0 but `RewardFactionValueIdOverride%d` = %i.",
                    qinfo->GetQuestId(), j+1, j+1, qinfo->RewardFactionValueIdOverride[j]);
                // No changes, quest ignore this data
            }
        }

        for (uint32 l_I = 0; l_I < QUEST_REWARD_SPELL_COUNT; l_I++)
        {
            if (qinfo->RewardSpells[l_I])
            {
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(qinfo->RewardSpells[l_I]);

                if (!spellInfo)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSpell%i` = %u but spell %u does not exist, spell removed as display reward.",
                        qinfo->GetQuestId(), l_I + 1, qinfo->RewardSpells[l_I], qinfo->RewardSpells[l_I]);
                    qinfo->RewardSpells[l_I] = 0;                        // No spell reward will display for this quest
                }

                else if (!SpellMgr::IsSpellValid(spellInfo))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSpell%i` = %u but spell %u is broken, quest will not have a spell reward.",
                        qinfo->GetQuestId(), l_I + 1, qinfo->RewardSpells[l_I], qinfo->RewardSpells[l_I]);
                    qinfo->RewardSpells[l_I] = 0;                        // No spell reward will display for this quest
                }
            }
        }

        if (qinfo->RewardSpellCast > 0)
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(qinfo->RewardSpellCast);

            if (!spellInfo)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSpellCast` = %u but spell %u does not exist, quest will not have a spell reward.",
                    qinfo->GetQuestId(), qinfo->RewardSpellCast, qinfo->RewardSpellCast);
                qinfo->RewardSpellCast = 0;                    // No spell will be casted on player
            }

            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSpellCast` = %u but spell %u is broken, quest will not have a spell reward.",
                    qinfo->GetQuestId(), qinfo->RewardSpellCast, qinfo->RewardSpellCast);
                qinfo->RewardSpellCast = 0;                    // No spell will be casted on player
            }

            /*else if (GetTalentSpellCost(qinfo->RewardSpellCast))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSpell` = %u but spell %u is talent, quest will not have a spell reward.",
                    qinfo->GetQuestId(), qinfo->RewardSpellCast, qinfo->RewardSpellCast);
                qinfo->RewardSpellCast = 0;                    // No spell will be casted on player
            }*/
        }

        if (qinfo->RewardMailTemplateId)
        {
            if (!sMailTemplateStore.LookupEntry(qinfo->RewardMailTemplateId))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardMailTemplateId` = %u but mail template  %u does not exist, quest will not have a mail reward.",
                    qinfo->GetQuestId(), qinfo->RewardMailTemplateId, qinfo->RewardMailTemplateId);
                qinfo->RewardMailTemplateId = 0;               // No mail will send to player
                qinfo->RewardMailDelay      = 0;               // No mail will send to player
            }
            else if (usedMailTemplates.find(qinfo->RewardMailTemplateId) != usedMailTemplates.end())
            {
                std::map<uint32, uint32>::const_iterator used_mt_itr = usedMailTemplates.find(qinfo->RewardMailTemplateId);
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardMailTemplateId` = %u but mail template  %u already used for quest %u, quest will not have a mail reward.",
                    qinfo->GetQuestId(), qinfo->RewardMailTemplateId, qinfo->RewardMailTemplateId, used_mt_itr->second);
                qinfo->RewardMailTemplateId = 0;               // No mail will send to player
                qinfo->RewardMailDelay      = 0;               // No mail will send to player
            }
            else
                usedMailTemplates[qinfo->RewardMailTemplateId] = qinfo->GetQuestId();
        }

        if (qinfo->NextQuestIdChain)
        {
            QuestMap::iterator qNextItr = _questTemplates.find(qinfo->NextQuestIdChain);
            if (qNextItr == _questTemplates.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `NextQuestIdChain` = %u but quest %u does not exist, quest chain will not work.",
                    qinfo->GetQuestId(), qinfo->NextQuestIdChain, qinfo->NextQuestIdChain);
                qinfo->NextQuestIdChain = 0;
            }
            else
                qNextItr->second->prevChainQuests.push_back(qinfo->GetQuestId());
        }

        for (uint8 j = 0; j < QUEST_REWARD_CURRENCY_COUNT; ++j)
        {
            if (qinfo->RewardCurrencyId[j])
            {
                if (qinfo->RewardCurrencyCount[j] == 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardCurrencyId%d` = %u but `RewardCurrencyCount%d` = 0, quest can't be done.",
                        qinfo->GetQuestId(), j + 1, qinfo->RewardCurrencyId[j], j + 1);
                    // No changes, quest can't be done for this requirement
                }

                if (!sCurrencyTypesStore.LookupEntry(qinfo->RewardCurrencyId[j]))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardCurrencyId%d` = %u but currency with entry %u does not exist, quest can't be done.",
                        qinfo->GetQuestId(), j + 1, qinfo->RewardCurrencyId[j], qinfo->RewardCurrencyId[j]);
                    qinfo->RewardCurrencyCount[j] = 0;         // Prevent incorrect work of quest
                }
            }
            else if (qinfo->RewardCurrencyCount[j] > 0)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardCurrencyId%d` = 0 but `RewardCurrencyCount%d` = %u, quest can't be done.",
                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewardCurrencyCount[j]);
                qinfo->RewardCurrencyCount[j] = 0;             // Prevent incorrect work of quest
            }
        }

        if (qinfo->SoundAccept)
        {
            if (!sSoundKitStore.LookupEntry(qinfo->SoundAccept))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `SoundAccept` = %u but sound %u does not exist, set to 0.",
                    qinfo->GetQuestId(), qinfo->SoundAccept, qinfo->SoundAccept);
                qinfo->SoundAccept = 0;                        // No sound will be played
            }
        }

        if (qinfo->SoundTurnIn)
        {
            if (!sSoundKitStore.LookupEntry(qinfo->SoundTurnIn))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `SoundTurnIn` = %u but sound %u does not exist, set to 0.",
                    qinfo->GetQuestId(), qinfo->SoundTurnIn, qinfo->SoundTurnIn);
                qinfo->SoundTurnIn = 0;                        // No sound will be played
            }
        }

        if (qinfo->RewardSkillId)
        {
            if (!sSkillLineStore.LookupEntry(qinfo->RewardSkillId))
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSkillId` = %u but this skill does not exist",
                    qinfo->GetQuestId(), qinfo->RewardSkillId);
            }
            if (!qinfo->RewardSkillPoints)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSkillId` = %u but `RewardSkillPoints` is 0",
                    qinfo->GetQuestId(), qinfo->RewardSkillId);
            }
        }

        if (qinfo->RewardSkillPoints)
        {
            if (qinfo->RewardSkillPoints > sWorld->GetConfigMaxSkillValue())
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSkillPoints` = %u but max possible skill is %u, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->RewardSkillPoints, sWorld->GetConfigMaxSkillValue());
                // No changes, quest can't be done for this requirement
            }
            if (!qinfo->RewardSkillId)
            {
                sLog->outError(LOG_FILTER_SQL, "Quest %u has `RewardSkillPoints` = %u but `RewardSkillId` is 0",
                    qinfo->GetQuestId(), qinfo->RewardSkillPoints);
            }
        }

        // Fill additional data stores
        if (qinfo->PrevQuestId)
        {
            if (_questTemplates.find(abs(qinfo->GetPrevQuestId())) == _questTemplates.end())
                sLog->outError(LOG_FILTER_SQL, "Quest %d has PrevQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetPrevQuestId());
            else
                qinfo->prevQuests.push_back(qinfo->PrevQuestId);
        }

        if (qinfo->NextQuestId)
        {
            QuestMap::iterator qNextItr = _questTemplates.find(abs(qinfo->GetNextQuestId()));
            if (qNextItr == _questTemplates.end())
                sLog->outError(LOG_FILTER_SQL, "Quest %d has NextQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetNextQuestId());
            else
            {
                int32 signedQuestId = qinfo->NextQuestId < 0 ? -int32(qinfo->GetQuestId()) : int32(qinfo->GetQuestId());
                qNextItr->second->prevQuests.push_back(signedQuestId);
            }
        }

        if (qinfo->ExclusiveGroup)
            mExclusiveQuestGroups.insert(std::pair<int32, uint32>(qinfo->ExclusiveGroup, qinfo->GetQuestId()));

        if (qinfo->LimitTime)
            qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAGS_TIMED);

        if (l_Reload)
        {
            if (MS::WorldQuest::Template* l_Quest = const_cast<MS::WorldQuest::Template*>(MS::WorldQuest::Template::GetTemplate(qinfo->Id)))
                l_Quest->OverrideQuestTemplate(qinfo);
        }
    }

    for (uint32 l_QuestID : l_QuestToRemove)
    {
        if (_questTemplates.find(l_QuestID) != _questTemplates.end())
            _questTemplates.erase(l_QuestID);   ///< Disable auto complete quests which are repeatable
    }

    // Check QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT for spell with SPELL_EFFECT_QUEST_COMPLETE
    for (uint32 i = 0; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i);
        if (!spellInfo)
            continue;

        for (uint8 j = 0; j < spellInfo->EffectCount; ++j)
        {
            if (spellInfo->Effects[j].Effect != SPELL_EFFECT_QUEST_COMPLETE)
                continue;

            uint32 quest_id = spellInfo->Effects[j].MiscValue;

            Quest const* quest = GetQuestTemplate(quest_id);

            // Some quest referenced in spells not exist (outdated spells)
            if (!quest)
                continue;

            if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
            {
                sLog->outError(LOG_FILTER_SQL, "Spell (id: %u) have SPELL_EFFECT_QUEST_COMPLETE for quest %u, but quest not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT. Quest flags must be fixed, quest modified to enable objective.", spellInfo->Id, quest_id);

                // This will prevent quest completing without objective
                const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);
                m_ScriptedQuests.push_back(quest->Id);
            }
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu quests definitions in %u ms", (unsigned long)_questTemplates.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestLocales()
{
    uint32 oldMSTime = getMSTime();

    _questLocaleStore.clear();                                 // Need for reload case

                                                               //                                               0     1
    QueryResult result = WorldDatabase.Query("SELECT Id, locale, "
        //      2           3                 4                5                 6                  7                   8                   9                  10           11
        "LogTitle, LogDescription, QuestDescription, AreaDescription, PortraitGiverText, PortraitGiverName, PortraitTurnInText, PortraitTurnInName, QuestCompletionLog, OfferRewardText "
        " FROM quest_template_locale");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        LocaleConstant locale = GetLocaleByName(fields[1].GetString());

        QuestLocale& data = _questLocaleStore[fields[0].GetUInt32()];
        AddLocaleString(fields[2].GetString(), locale, data.Title);
        AddLocaleString(fields[3].GetString(), locale, data.Objectives);
        AddLocaleString(fields[4].GetString(), locale, data.Details);
        AddLocaleString(fields[5].GetString(), locale, data.EndText);
        AddLocaleString(fields[6].GetString(), locale, data.QuestGiverTextWindow);
        AddLocaleString(fields[7].GetString(), locale, data.QuestGiverTargetName);
        AddLocaleString(fields[8].GetString(), locale, data.QuestTurnTextWindow);
        AddLocaleString(fields[9].GetString(), locale, data.QuestTurnTargetName);
        AddLocaleString(fields[10].GetString(), locale, data.CompletedText);
        AddLocaleString(fields[11].GetString(), locale, data.OfferRewardText);
    }
    while (result->NextRow());


    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu Quest locale strings in %u ms", (unsigned long)_questLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadScripts(ScriptsType type)
{
    uint32 oldMSTime = getMSTime();

    ScriptMapMap* scripts = GetScriptsMapByType(type);
    if (!scripts)
        return;

    std::string tableName = GetScriptsTableNameByType(type);
    if (tableName.empty())
        return;

    if (sScriptMgr->IsScriptScheduled())                    // function cannot be called when scripts are in use.
        return;

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading %s...", tableName.c_str());

    scripts->clear();                                       // need for reload support

    bool isSpellScriptTable = (type == SCRIPTS_SPELL);
    //                                                 0    1       2         3         4          5    6  7  8  9
    QueryResult result = WorldDatabase.PQuery("SELECT id, delay, command, datalong, datalong2, dataint, x, y, z, o%s FROM %s", isSpellScriptTable ? ", effIndex" : "", tableName.c_str());

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 script definitions. DB table `%s` is empty!", tableName.c_str());
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        ScriptInfo tmp;
        tmp.type      = type;
        tmp.id           = fields[0].GetUInt32();
        if (isSpellScriptTable)
            tmp.id      |= fields[10].GetUInt8() << 24;
        tmp.delay        = fields[1].GetUInt32();
        tmp.command      = ScriptCommands(fields[2].GetUInt32());
        tmp.Raw.nData[0] = fields[3].GetUInt32();
        tmp.Raw.nData[1] = fields[4].GetUInt32();
        tmp.Raw.nData[2] = fields[5].GetInt32();
        tmp.Raw.fData[0] = fields[6].GetFloat();
        tmp.Raw.fData[1] = fields[7].GetFloat();
        tmp.Raw.fData[2] = fields[8].GetFloat();
        tmp.Raw.fData[3] = fields[9].GetFloat();

        // generic command args check
        switch (tmp.command)
        {
            case SCRIPT_COMMAND_TALK:
            {
                if (tmp.Talk.ChatType > CHAT_TYPE_WHISPER && tmp.Talk.ChatType != CHAT_MSG_RAID_BOSS_WHISPER)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid talk type (datalong = %u) in SCRIPT_COMMAND_TALK for script id %u",
                        tableName.c_str(), tmp.Talk.ChatType, tmp.id);
                    continue;
                }
                if (!tmp.Talk.TextID)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid talk text id (dataint = %i) in SCRIPT_COMMAND_TALK for script id %u",
                        tableName.c_str(), tmp.Talk.TextID, tmp.id);
                    continue;
                }
                if (tmp.Talk.TextID < MIN_DB_SCRIPT_STRING_ID || tmp.Talk.TextID >= MAX_DB_SCRIPT_STRING_ID)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has out of range text id (dataint = %i expected %u-%u) in SCRIPT_COMMAND_TALK for script id %u",
                        tableName.c_str(), tmp.Talk.TextID, MIN_DB_SCRIPT_STRING_ID, MAX_DB_SCRIPT_STRING_ID, tmp.id);
                    continue;
                }

                break;
            }

            case SCRIPT_COMMAND_EMOTE:
            {
                if (!sEmotesStore.LookupEntry(tmp.Emote.EmoteID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid emote id (datalong = %u) in SCRIPT_COMMAND_EMOTE for script id %u",
                        tableName.c_str(), tmp.Emote.EmoteID, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_TELEPORT_TO:
            {
                if (!sMapStore.LookupEntry(tmp.TeleportTo.MapID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid map (Id: %u) in SCRIPT_COMMAND_TELEPORT_TO for script id %u",
                        tableName.c_str(), tmp.TeleportTo.MapID, tmp.id);
                    continue;
                }

                if (!JadeCore::IsValidMapCoord(tmp.TeleportTo.DestX, tmp.TeleportTo.DestY, tmp.TeleportTo.DestZ, tmp.TeleportTo.Orientation))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid coordinates (X: %f Y: %f Z: %f O: %f) in SCRIPT_COMMAND_TELEPORT_TO for script id %u",
                        tableName.c_str(), tmp.TeleportTo.DestX, tmp.TeleportTo.DestY, tmp.TeleportTo.DestZ, tmp.TeleportTo.Orientation, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_QUEST_EXPLORED:
            {
                Quest const* quest = GetQuestTemplate(tmp.QuestExplored.QuestID);
                if (!quest)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",
                        tableName.c_str(), tmp.QuestExplored.QuestID, tmp.id);
                    continue;
                }

                if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, but quest not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT in quest flags. Script command or quest flags wrong. Quest modified to require objective.",
                        tableName.c_str(), tmp.QuestExplored.QuestID, tmp.id);

                    // this will prevent quest completing without objective
                    const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);

                    // continue; - quest objective requirement set and command can be allowed
                }

                if (float(tmp.QuestExplored.Distance) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",
                        tableName.c_str(), tmp.QuestExplored.Distance, tmp.id);
                    continue;
                }

                if (tmp.QuestExplored.Distance && float(tmp.QuestExplored.Distance) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, max distance is %f or 0 for disable distance check",
                        tableName.c_str(), tmp.QuestExplored.Distance, tmp.id, DEFAULT_VISIBILITY_DISTANCE);
                    continue;
                }

                if (tmp.QuestExplored.Distance && float(tmp.QuestExplored.Distance) < INTERACTION_DISTANCE)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has too small distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, min distance is %f or 0 for disable distance check",
                        tableName.c_str(), tmp.QuestExplored.Distance, tmp.id, INTERACTION_DISTANCE);
                    continue;
                }

                break;
            }

            case SCRIPT_COMMAND_KILL_CREDIT:
            {
                if (!GetCreatureTemplate(tmp.KillCredit.CreatureEntry))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_KILL_CREDIT for script id %u",
                        tableName.c_str(), tmp.KillCredit.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT:
            {
                GameObjectData const* data = GetGOData(tmp.RespawnGameobject.GOGuid);
                if (!data)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid gameobject (GUID: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                        tableName.c_str(), tmp.RespawnGameobject.GOGuid, tmp.id);
                    continue;
                }

                GameObjectTemplate const* info = GetGameObjectTemplate(data->id);
                if (!info)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                        tableName.c_str(), tmp.RespawnGameobject.GOGuid, data->id, tmp.id);
                    continue;
                }

                if (info->type == GAMEOBJECT_TYPE_FISHINGNODE ||
                    info->type == GAMEOBJECT_TYPE_FISHINGHOLE ||
                    info->type == GAMEOBJECT_TYPE_DOOR        ||
                    info->type == GAMEOBJECT_TYPE_BUTTON      ||
                    info->type == GAMEOBJECT_TYPE_TRAP)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` have gameobject type (%u) unsupported by command SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                        tableName.c_str(), info->entry, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE:
            {
                if (!JadeCore::IsValidMapCoord(tmp.TempSummonCreature.PosX, tmp.TempSummonCreature.PosY, tmp.TempSummonCreature.PosZ, tmp.TempSummonCreature.Orientation))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid coordinates (X: %f Y: %f Z: %f O: %f) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u",
                        tableName.c_str(), tmp.TempSummonCreature.PosX, tmp.TempSummonCreature.PosY, tmp.TempSummonCreature.PosZ, tmp.TempSummonCreature.Orientation, tmp.id);
                    continue;
                }

                if (!GetCreatureTemplate(tmp.TempSummonCreature.CreatureEntry))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u",
                        tableName.c_str(), tmp.TempSummonCreature.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_OPEN_DOOR:
            case SCRIPT_COMMAND_CLOSE_DOOR:
            {
                GameObjectData const* data = GetGOData(tmp.ToggleDoor.GOGuid);
                if (!data)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has invalid gameobject (GUID: %u) in %s for script id %u",
                        tableName.c_str(), tmp.ToggleDoor.GOGuid, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                GameObjectTemplate const* info = GetGameObjectTemplate(data->id);
                if (!info)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in %s for script id %u",
                        tableName.c_str(), tmp.ToggleDoor.GOGuid, data->id, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                if (info->type != GAMEOBJECT_TYPE_DOOR)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has gameobject type (%u) non supported by command %s for script id %u",
                        tableName.c_str(), info->entry, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                break;
            }

            case SCRIPT_COMMAND_REMOVE_AURA:
            {
                if (!sSpellMgr->GetSpellInfo(tmp.RemoveAura.SpellID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` using non-existent spell (id: %u) in SCRIPT_COMMAND_REMOVE_AURA for script id %u",
                        tableName.c_str(), tmp.RemoveAura.SpellID, tmp.id);
                    continue;
                }
                if (tmp.RemoveAura.Flags & ~0x1)                    // 1 bits (0, 1)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` using unknown flags in datalong2 (%u) in SCRIPT_COMMAND_REMOVE_AURA for script id %u",
                        tableName.c_str(), tmp.RemoveAura.Flags, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_CAST_SPELL:
            {
                if (!sSpellMgr->GetSpellInfo(tmp.CastSpell.SpellID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` using non-existent spell (id: %u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tableName.c_str(), tmp.CastSpell.SpellID, tmp.id);
                    continue;
                }
                if (tmp.CastSpell.Flags > 4)                      // targeting type
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` using unknown target in datalong2 (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tableName.c_str(), tmp.CastSpell.Flags, tmp.id);
                    continue;
                }
                if (tmp.CastSpell.Flags != 4 && tmp.CastSpell.CreatureEntry & ~0x1)                      // 1 bit (0, 1)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` using unknown flags in dataint (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tableName.c_str(), tmp.CastSpell.CreatureEntry, tmp.id);
                    continue;
                }
                else if (tmp.CastSpell.Flags == 4 && !GetCreatureTemplate(tmp.CastSpell.CreatureEntry))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` using invalid creature entry in dataint (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                        tableName.c_str(), tmp.CastSpell.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

            case SCRIPT_COMMAND_CREATE_ITEM:
            {
                if (!GetItemTemplate(tmp.CreateItem.ItemEntry))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` has nonexistent item (entry: %u) in SCRIPT_COMMAND_CREATE_ITEM for script id %u",
                        tableName.c_str(), tmp.CreateItem.ItemEntry, tmp.id);
                    continue;
                }
                if (!tmp.CreateItem.Amount)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `%s` SCRIPT_COMMAND_CREATE_ITEM but amount is %u for script id %u",
                        tableName.c_str(), tmp.CreateItem.Amount, tmp.id);
                    continue;
                }
                break;
            }
            default:
                break;
        }

        if (scripts->find(tmp.id) == scripts->end())
        {
            ScriptMap emptyMap;
            (*scripts)[tmp.id] = emptyMap;
        }
        (*scripts)[tmp.id].insert(std::pair<uint32, ScriptInfo>(tmp.delay, tmp));

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u script definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGameObjectScripts()
{
    LoadScripts(SCRIPTS_GAMEOBJECT);

    // check ids
    for (ScriptMapMap::const_iterator itr = sGameObjectScripts.begin(); itr != sGameObjectScripts.end(); ++itr)
    {
        if (!GetGOData(itr->first))
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject_scripts` has not existing gameobject (GUID: %u) as script id", itr->first);
    }
}

void ObjectMgr::LoadQuestEndScripts()
{
    LoadScripts(SCRIPTS_QUEST_END);

    // check ids
    for (ScriptMapMap::const_iterator itr = sQuestEndScripts.begin(); itr != sQuestEndScripts.end(); ++itr)
    {
        if (!GetQuestTemplate(itr->first))
            sLog->outError(LOG_FILTER_SQL, "Table `quest_end_scripts` has not existing quest (Id: %u) as script id", itr->first);
    }
}

void ObjectMgr::LoadQuestStartScripts()
{
    LoadScripts(SCRIPTS_QUEST_START);

    // check ids
    for (ScriptMapMap::const_iterator itr = sQuestStartScripts.begin(); itr != sQuestStartScripts.end(); ++itr)
    {
        if (!GetQuestTemplate(itr->first))
            sLog->outError(LOG_FILTER_SQL, "Table `quest_start_scripts` has not existing quest (Id: %u) as script id", itr->first);
    }
}

void ObjectMgr::LoadSpellScripts()
{
    LoadScripts(SCRIPTS_SPELL);

    // check ids
    for (ScriptMapMap::const_iterator itr = sSpellScripts.begin(); itr != sSpellScripts.end(); ++itr)
    {
        uint32 spellId = uint32(itr->first) & 0x00FFFFFF;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);

        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `spell_scripts` has not existing spell (Id: %u) as script id", spellId);
            continue;
        }

        uint8 i = (uint8)((uint32(itr->first) >> 24) & 0x000000FF);
        //check for correct spellEffect
        if (!spellInfo->Effects[i].Effect || (spellInfo->Effects[i].Effect != SPELL_EFFECT_SCRIPT_EFFECT && spellInfo->Effects[i].Effect != SPELL_EFFECT_DUMMY))
            sLog->outError(LOG_FILTER_SQL, "Table `spell_scripts` - spell %u effect %u is not SPELL_EFFECT_SCRIPT_EFFECT or SPELL_EFFECT_DUMMY", spellId, i);
    }
}

void ObjectMgr::LoadEventScripts()
{
    LoadScripts(SCRIPTS_EVENT);

    std::set<uint32> evt_scripts;
    // Load all possible script entries from gameobjects
    GameObjectTemplateContainer const* gotc = sObjectMgr->GetGameObjectTemplates();
    for (GameObjectTemplateContainer::const_iterator itr = gotc->begin(); itr != gotc->end(); ++itr)
        if (uint32 eventId = itr->second.GetEventScriptId())
            evt_scripts.insert(eventId);

    // Load all possible script entries from spells
    for (uint32 i = 1; i < sSpellMgr->GetSpellInfoStoreSize(); ++i)
        if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(i))
            for (uint8 j = 0; j < spell->EffectCount; ++j)
                if (spell->Effects[j].Effect == SPELL_EFFECT_SEND_EVENT)
                    if (spell->Effects[j].MiscValue)
                        evt_scripts.insert(spell->Effects[j].MiscValue);

    for (size_t path_idx = 0; path_idx < sTaxiPathNodesByPath.size(); ++path_idx)
    {
        for (size_t node_idx = 0; node_idx < sTaxiPathNodesByPath[path_idx].size(); ++node_idx)
        {
            TaxiPathNodeEntry const* node = sTaxiPathNodesByPath[path_idx][node_idx];

            if (node == nullptr)
                continue;

            if (node->ArrivalEventID)
                evt_scripts.insert(node->ArrivalEventID);

            if (node->DepartureEventID)
                evt_scripts.insert(node->DepartureEventID);
        }
    }

    // Then check if all scripts are in above list of possible script entries
    for (ScriptMapMap::const_iterator itr = sEventScripts.begin(); itr != sEventScripts.end(); ++itr)
    {
        std::set<uint32>::const_iterator itr2 = evt_scripts.find(itr->first);
        if (itr2 == evt_scripts.end())
            sLog->outError(LOG_FILTER_SQL, "Table `event_scripts` has script (Id: %u) not referring to any gameobject_template type 10 data2 field, type 3 data6 field, type 13 data 2 field or any spell effect %u",
                itr->first, SPELL_EFFECT_SEND_EVENT);
    }
}

//Load WP Scripts
void ObjectMgr::LoadWaypointScripts()
{
    LoadScripts(SCRIPTS_WAYPOINT);

    std::set<uint32> actionSet;

    for (ScriptMapMap::const_iterator itr = sWaypointScripts.begin(); itr != sWaypointScripts.end(); ++itr)
        actionSet.insert(itr->first);

    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_WAYPOINT_DATA_ACTION);
    PreparedQueryResult result = WorldDatabase.Query(stmt);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 action = fields[0].GetUInt32();

            actionSet.erase(action);
        }
        while (result->NextRow());
    }

    for (std::set<uint32>::iterator itr = actionSet.begin(); itr != actionSet.end(); ++itr)
        sLog->outError(LOG_FILTER_SQL, "There is no waypoint which links to the waypoint script %u", *itr);
}

void ObjectMgr::LoadSpellScriptNames()
{
    uint32 oldMSTime = getMSTime();

    _spellScriptsStore.clear();                            // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT spell_id, ScriptName FROM spell_script_names");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell script names. DB table `spell_script_names` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {

        Field* fields = result->Fetch();

        int32 spellId          = fields[0].GetInt32();
        const char* scriptName = fields[1].GetCString();

        bool allRanks = false;
        if (spellId <= 0)
        {
            allRanks = true;
            spellId = -spellId;
        }

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Scriptname:`%s` spell (spell_id:%d) does not exist in `Spell.db2`.", scriptName, fields[0].GetInt32());
            continue;
        }

        if (allRanks)
        {
            if (sSpellMgr->GetFirstSpellInChain(spellId) != uint32(spellId))
            {
                sLog->outError(LOG_FILTER_SQL, "Scriptname:`%s` spell (spell_id:%d) is not first rank of spell.", scriptName, fields[0].GetInt32());
                continue;
            }
            while (spellInfo)
            {
                _spellScriptsStore.insert(SpellScriptsContainer::value_type(spellInfo->Id, GetScriptId(scriptName)));
                spellInfo = sSpellMgr->GetSpellInfo(spellInfo->Id)->GetNextRankSpell();
            }
        }
        else
            _spellScriptsStore.insert(SpellScriptsContainer::value_type(spellInfo->Id, GetScriptId(scriptName)));
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell script names in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::ValidateSpellScripts()
{
    uint32 oldMSTime = getMSTime();

    if (_spellScriptsStore.empty())
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Validated 0 scripts.");
        return;
    }

    uint32 count = 0;

    for (SpellScriptsContainer::iterator itr = _spellScriptsStore.begin(); itr != _spellScriptsStore.end();)
    {
        SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(itr->first);
        std::vector<std::pair<SpellScriptLoader*, SpellScriptsContainer::iterator> > SpellScriptLoaders;
        sScriptMgr->CreateSpellScriptLoaders(itr->first, SpellScriptLoaders);
        itr = _spellScriptsStore.upper_bound(itr->first);

        for (std::vector<std::pair<SpellScriptLoader*, SpellScriptsContainer::iterator> >::iterator sitr = SpellScriptLoaders.begin(); sitr != SpellScriptLoaders.end(); ++sitr)
        {
            SpellScript* spellScript = sitr->first->GetSpellScript();
            AuraScript* auraScript = sitr->first->GetAuraScript();
            bool valid = true;
            if (!spellScript && !auraScript)
            {
                sLog->outError(LOG_FILTER_TSCR, "Functions GetSpellScript() and GetAuraScript() of script `%s` do not return objects - script skipped",  GetScriptName(sitr->second->second));
                valid = false;
            }
            if (spellScript)
            {
                spellScript->_Init(&sitr->first->GetName(), spellEntry->Id);
                spellScript->_Register();
                if (!spellScript->_Validate(spellEntry))
                    valid = false;
                delete spellScript;
            }
            if (auraScript)
            {
                auraScript->_Init(&sitr->first->GetName(), spellEntry->Id);
                auraScript->_Register();
                if (!auraScript->_Validate(spellEntry))
                    valid = false;
                delete auraScript;
            }
            if (!valid)
            {
                _spellScriptsStore.erase(sitr->second);
            }
        }
        ++count;
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Validated %u scripts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPageTexts()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0      1       2            3             4
    QueryResult result = WorldDatabase.Query("SELECT entry, Text, NextPageID, PlayerConditionID, Flags FROM page_text");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 page texts. DB table `page_text` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        PageText& pageText = _pageTextStore[fields[0].GetUInt32()];

        pageText.Text                   = fields[1].GetString();
        pageText.NextPageID             = fields[2].GetUInt32();
        pageText.m_PlayerConditionID    = fields[3].GetUInt32();
        pageText.m_Flags                = fields[4].GetUInt8();

        ++count;
    }
    while (result->NextRow());

    for (PageTextContainer::const_iterator itr = _pageTextStore.begin(); itr != _pageTextStore.end(); ++itr)
    {
        if (itr->second.NextPageID)
        {
            PageTextContainer::const_iterator itr2 = _pageTextStore.find(itr->second.NextPageID);
            if (itr2 == _pageTextStore.end())
                sLog->outError(LOG_FILTER_SQL, "Page text (Id: %u) has not existing NextPageID (Id: %u)", itr->first, itr->second.NextPageID);

        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u page texts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

PageText const* ObjectMgr::GetPageText(uint32 pageEntry)
{
    PageTextContainer::const_iterator itr = _pageTextStore.find(pageEntry);
    if (itr != _pageTextStore.end())
        return &(itr->second);

    return NULL;
}

void ObjectMgr::LoadPageTextLocales()
{
    uint32 oldMSTime = getMSTime();

    _pageTextLocaleStore.clear();                             // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, text_loc1, text_loc2, text_loc3, text_loc4, text_loc5, text_loc6, text_loc7, text_loc8, text_loc9, text_loc10 FROM locales_page_text");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        PageTextLocale& data = _pageTextLocaleStore[entry];

        for (uint8 i = 1; i < TOTAL_LOCALES; ++i)
            AddLocaleString(fields[i].GetString(), LocaleConstant(i), data.Text);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu PageText locale strings in %u ms", (unsigned long)_pageTextLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadInstanceTemplate()
{
    uint32 oldMSTime = getMSTime();

    //                                                0     1       2        4
    QueryResult result = WorldDatabase.Query("SELECT map, parent, script, allowMount FROM instance_template");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 instance templates. DB table `page_text` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint16 mapID = fields[0].GetUInt16();

        if (!MapManager::IsValidMAP(mapID, true))
        {
            sLog->outError(LOG_FILTER_SQL, "ObjectMgr::LoadInstanceTemplate: bad mapid %d for template!", mapID);
            continue;
        }

        InstanceTemplate instanceTemplate;

        instanceTemplate.AllowMount = fields[3].GetBool();
        instanceTemplate.Parent     = uint32(fields[1].GetUInt16());
        instanceTemplate.ScriptId   = sObjectMgr->GetScriptId(fields[2].GetCString());

        _instanceTemplateStore[mapID] = instanceTemplate;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u instance templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

InstanceTemplate const* ObjectMgr::GetInstanceTemplate(uint32 mapID)
{
    InstanceTemplateContainer::const_iterator itr = _instanceTemplateStore.find(uint16(mapID));
    if (itr != _instanceTemplateStore.end())
        return &(itr->second);

    return NULL;
}

void ObjectMgr::LoadInstanceEncounters()
{
    uint32 l_OldMSTime = getMSTime();
    uint32 l_Counter = 0;

    for (uint32 l_Index = 0; l_Index <= sDungeonEncounterStore.GetNumRows(); ++l_Index)
    {
        if (DungeonEncounterEntry const* l_Encounter = sDungeonEncounterStore.LookupEntry(l_Index))
        {
            DungeonEncounterList& l_Encounters = _dungeonEncounterStore[MAKE_PAIR32(l_Encounter->MapID, l_Encounter->DifficultyID)];
            l_Encounters.push_back(new DungeonEncounter(l_Encounter, EncounterCreditType(0), 0, 0));
            ++l_Counter;
        }
    }

    ///                                                 0         1            2                3
    QueryResult l_Result = WorldDatabase.Query("SELECT entry, creditType, creditEntry, lastEncounterDungeon FROM instance_encounters");

    if (!l_Result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 instance encounters, table is empty!");

        return;
    }

    std::set<uint32> l_InsertedEncounters;
    std::map<uint32, DungeonEncounterEntry const*> l_DungeonLastBosses;
    do
    {
        Field* l_Fields                                 = l_Result->Fetch();
        uint32 l_EncounterID                            = l_Fields[0].GetUInt32();
        uint8 l_CreditType                              = l_Fields[1].GetUInt8();
        uint32 l_CreditEntry                            = l_Fields[2].GetUInt32();
        uint16 l_LastEncounterDungeon                   = l_Fields[3].GetUInt16();
        DungeonEncounterEntry const* l_DungeonEncounter = sDungeonEncounterStore.LookupEntry(l_EncounterID);

        if (!l_DungeonEncounter)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `instance_encounters` has an invalid encounter id %u, skipped!", l_EncounterID);
            continue;
        }

        if (l_LastEncounterDungeon && !sLFGDungeonStore.LookupEntry(l_LastEncounterDungeon))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `instance_encounters` has an encounter %u (%s) marked as final for invalid dungeon id %u, skipped!", l_EncounterID, l_DungeonEncounter->NameLang->Get(DEFAULT_LOCALE), l_LastEncounterDungeon);
            continue;
        }

        std::map<uint32, DungeonEncounterEntry const*>::const_iterator l_Itr = l_DungeonLastBosses.find(l_LastEncounterDungeon);
        if (l_LastEncounterDungeon)
        {
            if (l_Itr != l_DungeonLastBosses.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `instance_encounters` specified encounter %u (%s) as last encounter but %u (%s) is already marked as one, skipped!", l_EncounterID, l_DungeonEncounter->NameLang->Get(DEFAULT_LOCALE), l_Itr->second->ID, l_Itr->second->NameLang->Get(DEFAULT_LOCALE));
                continue;
            }

            l_DungeonLastBosses[l_LastEncounterDungeon] = l_DungeonEncounter;
        }

        switch (l_CreditType)
        {
            case ENCOUNTER_CREDIT_KILL_CREATURE:
            {
                CreatureTemplate const* l_CreatureInfo = GetCreatureTemplate(l_CreditEntry);
                if (!l_CreatureInfo)
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `instance_encounters` has an invalid creature (entry %u) linked to the encounter %u (%s), skipped!", l_CreditEntry, l_EncounterID, l_DungeonEncounter->NameLang->Get(DEFAULT_LOCALE));
                    continue;
                }
                const_cast<CreatureTemplate*>(l_CreatureInfo)->flags_extra |= CREATURE_FLAG_EXTRA_DUNGEON_BOSS;
                break;
            }
            case ENCOUNTER_CREDIT_CAST_SPELL:
                if (!sSpellMgr->GetSpellInfo(l_CreditEntry))
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `instance_encounters` has an invalid spell (entry %u) linked to the encounter %u (%s), skipped!", l_CreditEntry, l_EncounterID, l_DungeonEncounter->NameLang->Get(DEFAULT_LOCALE));
                    continue;
                }
                break;
            default:
                sLog->outError(LOG_FILTER_SQL, "Table `instance_encounters` has an invalid credit type (%u) for encounter %u (%s), skipped!", l_CreditType, l_EncounterID, l_DungeonEncounter->NameLang->Get(DEFAULT_LOCALE));
                continue;
        }

        uint32 l_DungeonStoreIndex = MAKE_PAIR32(l_DungeonEncounter->MapID, l_DungeonEncounter->DifficultyID);

        if (_dungeonEncounterStore.find(l_DungeonStoreIndex) != _dungeonEncounterStore.end())
        {
            DungeonEncounterList& l_Encounters = _dungeonEncounterStore[l_DungeonStoreIndex];

            if (l_InsertedEncounters.find(l_EncounterID) == l_InsertedEncounters.end())
            {
                for (auto& l_Encounter : l_Encounters)
                {
                    if (l_Encounter->dbcEntry && l_Encounter->dbcEntry->ID == l_EncounterID)
                    {
                        delete l_Encounter;
                        l_Encounter = new DungeonEncounter(l_DungeonEncounter, EncounterCreditType(l_CreditType), l_CreditEntry, l_LastEncounterDungeon);
                        l_InsertedEncounters.insert(l_EncounterID);
                        break;
                    }
                }
            }
            else
                l_Encounters.push_back(new DungeonEncounter(l_DungeonEncounter, EncounterCreditType(l_CreditType), l_CreditEntry, l_LastEncounterDungeon));
        }
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u instance encounters in %u ms", l_Counter, GetMSTimeDiffToNow(l_OldMSTime));
}

GossipText const* ObjectMgr::GetGossipText(uint32 Text_ID) const
{
    GossipTextContainer::const_iterator itr = _gossipTextStore.find(Text_ID);
    if (itr != _gossipTextStore.end())
        return &itr->second;
    return NULL;
}

void ObjectMgr::LoadGossipText()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT * FROM npc_text");

    int count = 0;
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u npc texts", count);
        return;
    }
    //_gossipTextStore.rehash(result->GetRowCount());

    int cic;

    do
    {
        ++count;
        cic = 0;

        Field* fields = result->Fetch();

        uint32 Text_ID    = fields[cic++].GetUInt32();
        if (!Text_ID)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `npc_text` has record wit reserved id 0, ignore.");
            continue;
        }

        GossipText& gText = _gossipTextStore[Text_ID];

        gText.Text_0    = fields[cic++].GetString();
        gText.Text_1    = fields[cic++].GetString();
        gText.Language  = fields[cic++].GetUInt8();

        for (uint8 j = 0; j < MAX_GOSSIP_TEXT_EMOTES; ++j)
        {
            gText.Emotes[j]._Delay = fields[cic++].GetUInt16();
            gText.Emotes[j]._Emote = fields[cic++].GetUInt16();
        }

        gText.SoundID = fields[cic++].GetUInt32();

        for (int i = 0; i < MAX_GOSSIP_TEXT_OPTIONS; i++)
        {
            gText.BroadcastTextID[i]    = fields[cic++].GetUInt32();
            gText.Probability[i]        = fields[cic++].GetFloat();

        }
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u npc texts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadNpcTextLocales()
{
    uint32 oldMSTime = getMSTime();

    _npcTextLocaleStore.clear();                              // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, "
        "Text0_0_loc1,  Text0_1_loc1, "
        "Text0_0_loc2,  Text0_1_loc2, "
        "Text0_0_loc3,  Text0_1_loc3, "
        "Text0_0_loc4,  Text0_1_loc4, "
        "Text0_0_loc5,  Text0_1_loc5, "
        "Text0_0_loc6,  Text0_1_loc6, "
        "Text0_0_loc7,  Text0_1_loc7, "
        "Text0_0_loc8,  Text0_1_loc8, "
        "Text0_0_loc9,  Text0_1_loc9, "
        "Text0_0_loc10, Text0_1_loc10 "
        " FROM locales_npc_text");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        NpcTextLocale& data = _npcTextLocaleStore[entry];

        for (uint8 i = TOTAL_LOCALES - 1; i > 0; --i)
        {
            LocaleConstant locale = (LocaleConstant)i;
            AddLocaleString(fields[1 + 2 * (i - 1)].GetString(), locale, data.Text_0[0]);
            AddLocaleString(fields[1 + 2 * (i - 1) + 1].GetString(), locale, data.Text_1[0]);
        }
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu NpcText locale strings in %u ms", (unsigned long)_npcTextLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

#ifndef CROSS
//not very fast function but it is called only once a day, or on starting-up
void ObjectMgr::ReturnOrDeleteOldMails(bool serverUp)
{
    uint32 oldMSTime = getMSTime();

    time_t curTime = time(NULL);
    tm* lt = localtime(&curTime);
    uint64 basetime(curTime);
    sLog->outInfo(LOG_FILTER_GENERAL, "Returning mails current time: hour: %d, minute: %d, second: %d ", lt->tm_hour, lt->tm_min, lt->tm_sec);

    // Delete all old mails without item and without body immediately, if starting server
    if (!serverUp)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_EMPTY_EXPIRED_MAIL);
        stmt->setUInt64(0, basetime);
        CharacterDatabase.Execute(stmt);
    }
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXPIRED_MAIL);
    stmt->setUInt64(0, basetime);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> No expired mails found.");
        return;                                             // any mails need to be returned or deleted
    }

    std::map<uint32 /*messageId*/, MailItemInfoVec> itemsCache;
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXPIRED_MAIL_ITEMS);
    stmt->setUInt32(0, (uint32)basetime);
    if (PreparedQueryResult items = CharacterDatabase.Query(stmt))
    {
        MailItemInfo item;
        do
        {
            Field* fields = items->Fetch();
            item.item_guid = fields[0].GetUInt32();
            item.item_template = fields[1].GetUInt32();
            uint32 mailId = fields[2].GetUInt32();
            itemsCache[mailId].push_back(item);
        }
        while (items->NextRow());
    }

    uint32 deletedCount = 0;
    uint32 returnedCount = 0;
    do
    {
        Field* fields = result->Fetch();
        Mail* m = new Mail;
        m->messageID      = fields[0].GetUInt32();
        m->messageType    = fields[1].GetUInt8();
        m->sender         = fields[2].GetUInt32();
        m->receiver       = fields[3].GetUInt32();
        bool has_items    = fields[4].GetBool();
        m->expire_time    = time_t(fields[5].GetUInt32());
        m->deliver_time   = 0;
        m->COD            = fields[6].GetUInt64();
        m->checked        = fields[7].GetUInt8();
        m->mailTemplateId = fields[8].GetInt16();

        Player* player = NULL;
        if (serverUp)
            player = ObjectAccessor::FindPlayer((uint64)m->receiver);

        if (player)
        {                                                   // this code will run very improbably (the time is between 4 and 5 am, in game is online a player, who has old mail
            // his in mailbox and he has already listed his mails)
            delete m;
            continue;
        }

        // Delete or return mail
        if (has_items)
        {
            // read items from cache
            m->items.swap(itemsCache[m->messageID]);

            // if it is mail from non-player, or if it's already return mail, it shouldn't be returned, but deleted
            if (m->messageType != MAIL_NORMAL || (m->checked & (MAIL_CHECK_MASK_COD_PAYMENT | MAIL_CHECK_MASK_RETURNED)))
            {
                // mail open and then not returned
                for (MailItemInfoVec::iterator itr2 = m->items.begin(); itr2 != m->items.end(); ++itr2)
                {
                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ITEM_INSTANCE);
                    stmt->setUInt32(0, itr2->item_guid);
                    CharacterDatabase.Execute(stmt);
                    sLog->outAshran("ObjectMgr::ReturnOrDeleteOldMails delete item %u", itr2->item_guid);
                }
            }
            else
            {
                // Mail will be returned
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_MAIL_RETURNED);
                stmt->setUInt32(0, m->receiver);
                stmt->setUInt32(1, m->sender);
                stmt->setUInt32(2, basetime + 30 * DAY);
                stmt->setUInt32(3, basetime);
                stmt->setUInt8 (4, uint8(MAIL_CHECK_MASK_RETURNED));
                stmt->setUInt32(5, m->messageID);
                CharacterDatabase.Execute(stmt);
                for (MailItemInfoVec::iterator itr2 = m->items.begin(); itr2 != m->items.end(); ++itr2)
                {
                    // Update receiver in mail items for its proper delivery, and in instance_item for avoid lost item at sender delete
                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_MAIL_ITEM_RECEIVER);
                    stmt->setUInt32(0, m->sender);
                    stmt->setUInt32(1, itr2->item_guid);
                    CharacterDatabase.Execute(stmt);

                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ITEM_OWNER);
                    stmt->setUInt32(0, m->sender);
                    stmt->setUInt32(1, itr2->item_guid);
                    CharacterDatabase.Execute(stmt);
                }
                delete m;
                ++returnedCount;
                continue;
            }
        }

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_MAIL_BY_ID);
        stmt->setUInt32(0, m->messageID);
        CharacterDatabase.Execute(stmt);
        delete m;
        ++deletedCount;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Processed %u expired mails: %u deleted and %u returned in %u ms", deletedCount + returnedCount, deletedCount, returnedCount, GetMSTimeDiffToNow(oldMSTime));
}

#endif /* not CROSS */
void ObjectMgr::LoadQuestAreaTriggers()
{
    uint32 oldMSTime = getMSTime();

    _questAreaTriggerStore.clear();                           // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT id, quest FROM areatrigger_involvedrelation");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 quest trigger points. DB table `areatrigger_involvedrelation` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 trigger_ID = fields[0].GetUInt32();
        uint32 quest_ID   = fields[1].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(trigger_ID);
        if (!atEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Area trigger (ID:%u) does not exist in `AreaTrigger.db2`.", trigger_ID);
            continue;
        }

        Quest const* quest = GetQuestTemplate(quest_ID);

        if (!quest)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `areatrigger_involvedrelation` has record (id: %u) for not existing quest %u", trigger_ID, quest_ID);
            continue;
        }

        if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `areatrigger_involvedrelation` has record (id: %u) for not quest %u, but quest not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT. Trigger or quest flags must be fixed, quest modified to require objective.", trigger_ID, quest_ID);

            // this will prevent quest completing without objective
            const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);

            // continue; - quest modified to required objective and trigger can be allowed.
        }

        _questAreaTriggerStore[trigger_ID] = quest_ID;

    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u quest trigger points in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadTavernAreaTriggers()
{
    uint32 oldMSTime = getMSTime();

    _tavernAreaTriggerStore.clear();                          // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT id FROM areatrigger_tavern");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 tavern triggers. DB table `areatrigger_tavern` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 Trigger_ID      = fields[0].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Area trigger (ID:%u) does not exist in `AreaTrigger.db2`.", Trigger_ID);
            continue;
        }

        _tavernAreaTriggerStore.insert(Trigger_ID);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u tavern triggers in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadAreaTriggerScripts()
{
    uint32 oldMSTime = getMSTime();

    _areaTriggerScriptStore.clear();                            // need for reload case
    QueryResult result = WorldDatabase.Query("SELECT entry, ScriptName FROM areatrigger_scripts");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 areatrigger scripts. DB table `areatrigger_scripts` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 Trigger_ID      = fields[0].GetUInt32();
        const char* scriptName = fields[1].GetCString();

        //AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        //if (!atEntry)
        //{
       //     sLog->outError(LOG_FILTER_SQL, "Area trigger (ID:%u) does not exist in `AreaTrigger.db2`.", Trigger_ID);
        //    continue;
        //}
        _areaTriggerScriptStore[Trigger_ID] = GetScriptId(scriptName);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u areatrigger scripts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

uint32 ObjectMgr::GetNearestTaxiNode(float x, float y, float z, uint32 mapid, uint32 team)
{
    bool found = false;
    float dist = 10000;
    uint32 id = 0;

    std::map<uint32, uint32> l_MapOverrides;

#ifndef CROSS
    /// Special case for taxi in garrison phased map
    for (uint32 l_I = 0; l_I < sGarrSiteLevelStore.GetNumRows(); ++l_I)
    {
        const GarrSiteLevelEntry* l_Entry = sGarrSiteLevelStore.LookupEntry(l_I);

        if (l_Entry && l_Entry->SiteID != 0)
        {
            l_MapOverrides[l_Entry->MapID] = MS::Garrison::GDraenor::Globals::BaseMap;

            if (l_Entry->MapID == mapid)
                mapid = MS::Garrison::GDraenor::Globals::BaseMap;
        }
    }


#endif /* not CROSS */
    uint32 requireFlag = (team == ALLIANCE) ? TAXI_NODE_FLAG_ALLIANCE : TAXI_NODE_FLAG_HORDE;
    for (uint32 i = 0; i < sTaxiNodesStore.GetNumRows(); ++i)
    {
        TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(i);

        if (!node || !(node->Flags & requireFlag))
            continue;

        if (node->MapID != mapid)
        {
            if (l_MapOverrides.find(node->MapID) != l_MapOverrides.end() && l_MapOverrides[node->MapID] != mapid)
                continue;
        }

        uint8  field   = (uint8)((node->ID - 1) / 8);
        uint32 submask = 1 << ((node->ID - 1) % 8);

        // skip not taxi network nodes
        if ((sTaxiNodesMask[field] & submask) == 0)
            continue;

        float dist2 = (node->x - x)*(node->x - x) + (node->y - y)*(node->y - y) + (node->z - z)*(node->z - z);
        if (found)
        {
            if (dist2 < dist)
            {
                dist = dist2;
                id = node->ID;
            }
        }
        else
        {
            found = true;
            dist = dist2;
            id = node->ID;
        }
    }

    return id;
}


void ObjectMgr::GetTaxiPath(uint32 source, uint32 destination, uint32 &path, uint32 &cost)
{
    TaxiPathSetBySource::iterator src_i = sTaxiPathSetBySource.find(source);
    if (src_i == sTaxiPathSetBySource.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    TaxiPathSetForSource& pathSet = src_i->second;

    TaxiPathSetForSource::iterator dest_i = pathSet.find(destination);
    if (dest_i == pathSet.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    cost = dest_i->second.price;
    path = dest_i->second.ID;
}

uint32 ObjectMgr::GetTaxiMountDisplayId(uint32 id, uint32 team, bool allowed_alt_team /* = false */, uint32 p_PreferredMountDisplay /*= 0*/)
{
    uint32 mount_id = 0;

    // select mount creature id
    TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(id);
    if (node)
    {
        /// Wrong flag I think, because preverrd mounts are used for flying from Dalaran but those nodes have no TaxiNodeFlags::TAXI_NODE_FLAG_USE_FAVORITE_MOUNT (0x10)
        if ((node->Flags & TaxiNodeFlags::TAXI_NODE_FLAG_USE_FAVORITE_MOUNT || node->ID == 1774 || node->ID == 1849) && p_PreferredMountDisplay)
            return p_PreferredMountDisplay;

        uint32 mount_entry = 0;
        if (team == ALLIANCE)
            mount_entry = node->MountCreatureID[1];
        else
            mount_entry = node->MountCreatureID[0];

        // Fix for Alliance not being able to use Acherus taxi
        // only one mount type for both sides
        if (mount_entry == 0 && allowed_alt_team)
        {
            // Simply reverse the selection. At least one team in theory should have a valid mount ID to choose.
            mount_entry = team == ALLIANCE ? node->MountCreatureID[0] : node->MountCreatureID[1];
        }

        CreatureTemplate const* mount_info = GetCreatureTemplate(mount_entry);
        if (mount_info)
        {
            mount_id = mount_info->GetRandomValidModelId();
            if (!mount_id)
            {
                sLog->outError(LOG_FILTER_SQL, "No displayid found for the taxi mount with the entry %u! Can't load it!", mount_entry);
                return false;
            }
        }
    }

    // minfo is not actually used but the mount_id was updated
    GetCreatureModelRandomGender(&mount_id);

    return mount_id;
}

void ObjectMgr::LoadGraveyardZones()
{
    uint32 oldMSTime = getMSTime();

    GraveYardStore.clear();                                  // need for reload case

    //                                                0       1         2
    QueryResult result = WorldDatabase.Query("SELECT id, ghost_zone, faction FROM game_graveyard_zone");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 graveyard-zone links. DB table `game_graveyard_zone` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 safeLocId = fields[0].GetUInt32();
        uint32 zoneId = fields[1].GetUInt32();
        uint32 team   = fields[2].GetUInt16();

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(safeLocId);
        if (!entry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `game_graveyard_zone` has a record for not existing graveyard (WorldSafeLocs.db2 id) %u, skipped.", safeLocId);
            continue;
        }

        AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(zoneId);
        if (!areaEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `game_graveyard_zone` has a record for not existing zone id (%u), skipped.", zoneId);
            continue;
        }

        if (areaEntry->ParentAreaID != 0)
        {
            switch(zoneId)
            {
                case 33:
                case 5287:
                case 6170:
                case 6176:
                case 6450:
                case 6451:
                case 6452:
                case 6453:
                case 6454:
                case 6455:
                case 6456:
                case 6457:
                case 7004:
                case 7078:
                case 4755: ///< Gilneas
                    break;
                default:
                {
                    sLog->outError(LOG_FILTER_SQL, "Table `game_graveyard_zone` has a record for subzone id (%u) instead of zone, skipped.", zoneId);
                    continue;
                }
            }
        }

        if (team != 0 && team != HORDE && team != ALLIANCE)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `game_graveyard_zone` has a record for non player faction (%u), skipped.", team);
            continue;
        }

        if (!AddGraveYardLink(safeLocId, zoneId, team, false))
            sLog->outError(LOG_FILTER_SQL, "Table `game_graveyard_zone` has a duplicate record for Graveyard (ID: %u) and Zone (ID: %u), skipped.", safeLocId, zoneId);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u graveyard-zone links in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

WorldSafeLocsEntry const* ObjectMgr::GetDefaultGraveYard(uint32 team)
{
    enum DefaultGraveyard
    {
        HORDE_GRAVEYARD = 10, // Crossroads
        ALLIANCE_GRAVEYARD = 4, // Westfall
    };

    if (team == HORDE)
        return sWorldSafeLocsStore.LookupEntry(HORDE_GRAVEYARD);
    else if (team == ALLIANCE)
        return sWorldSafeLocsStore.LookupEntry(ALLIANCE_GRAVEYARD);
    else return NULL;
}

WorldSafeLocsEntry const* ObjectMgr::GetClosestGraveYard(float x, float y, float z, uint32 MapId, uint32 team)
{
    // search for zone associated closest graveyard
    uint32 zoneId = sMapMgr->GetZoneId(MapId, x, y, z);

    if (!zoneId)
    {
        if (z > -500)
        {
            sLog->outError(LOG_FILTER_GENERAL, "ZoneId not found for map %u coords (%f, %f, %f)", MapId, x, y, z);
            return GetDefaultGraveYard(team);
        }
    }

    // Simulate std. algorithm:
    //   found some graveyard associated to (ghost_zone, ghost_map)
    //
    //   if mapId == graveyard.mapId (ghost in plain zone or city or battleground) and search graveyard at same map
    //     then check faction
    //   if mapId != graveyard.mapId (ghost in instance) and search any graveyard associated
    //     then check faction
    GraveYardContainer::const_iterator graveLow  = GraveYardStore.lower_bound(zoneId);
    GraveYardContainer::const_iterator graveUp   = GraveYardStore.upper_bound(zoneId);
    MapEntry const* map = sMapStore.LookupEntry(MapId);
    // not need to check validity of map object; MapId _MUST_ be valid here

    if (graveLow == graveUp && !map->IsBattleArena())
    {
        sLog->outError(LOG_FILTER_SQL, "Table `game_graveyard_zone` incomplete: Zone %u Team %u does not have a linked graveyard.", zoneId, team);
        return GetDefaultGraveYard(team);
    }

    // at corpse map
    bool foundNear = false;
    float distNear = 10000;
    WorldSafeLocsEntry const* entryNear = NULL;

    // at entrance map for corpse map
    bool foundEntr = false;
    float distEntr = 10000;
    WorldSafeLocsEntry const* entryEntr = NULL;

    // some where other
    WorldSafeLocsEntry const* entryFar = NULL;

    MapEntry const* mapEntry = sMapStore.LookupEntry(MapId);

    for (GraveYardContainer::const_iterator itr = graveLow; itr != graveUp; ++itr)
    {
        GraveYardData const& data = itr->second;

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(data.safeLocId);
        if (!entry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `game_graveyard_zone` has record for not existing graveyard (WorldSafeLocs.db2 id) %u, skipped.", data.safeLocId);
            continue;
        }

        // skip enemy faction graveyard
        // team == 0 case can be at call from .neargrave
        if (data.team != 0 && team != 0 && data.team != team)
            continue;

        // find now nearest graveyard at other map
        if (MapId != entry->MapID)
        {
            // if find graveyard at different map from where entrance placed (or no entrance data), use any first
            if (!mapEntry
                || mapEntry->CorpseMapID < 0
                || uint32(mapEntry->CorpseMapID) != entry->MapID
                || (mapEntry->CorpseX == 0 && mapEntry->CorpseY == 0))
            {
                // not have any corrdinates for check distance anyway
                entryFar = entry;
                continue;
            }

            // at entrance map calculate distance (2D);
            float dist2 = (entry->x - mapEntry->CorpseX)*(entry->x - mapEntry->CorpseX)
                + (entry->y - mapEntry->CorpseY)*(entry->y - mapEntry->CorpseY);

            if (foundEntr)
            {
                if (dist2 < distEntr)
                {
                    distEntr = dist2;
                    entryEntr = entry;
                }
            }
            else
            {
                foundEntr = true;
                distEntr = dist2;
                entryEntr = entry;
            }
        }
        // find now nearest graveyard at same map
        else
        {
            float dist2 = (entry->x - x)*(entry->x - x)+(entry->y - y)*(entry->y - y)+(entry->z - z)*(entry->z - z);
            if (foundNear)
            {
                if (dist2 < distNear)
                {
                    distNear = dist2;
                    entryNear = entry;
                }
            }
            else
            {
                foundNear = true;
                distNear = dist2;
                entryNear = entry;
            }
        }
    }

    if (entryNear)
        return entryNear;

    if (entryEntr)
        return entryEntr;

    return entryFar;
}

GraveYardData const* ObjectMgr::FindGraveYardData(uint32 id, uint32 zoneId)
{
    GraveYardContainer::const_iterator graveLow  = GraveYardStore.lower_bound(zoneId);
    GraveYardContainer::const_iterator graveUp   = GraveYardStore.upper_bound(zoneId);

    for (GraveYardContainer::const_iterator itr = graveLow; itr != graveUp; ++itr)
    {
        if (itr->second.safeLocId == id)
            return &itr->second;
    }

    return NULL;
}

bool ObjectMgr::AddGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool persist /*= true*/)
{
    if (FindGraveYardData(id, zoneId))
        return false;

    // add link to loaded data
    GraveYardData data;
    data.safeLocId = id;
    data.team = team;

    GraveYardStore.insert(GraveYardContainer::value_type(zoneId, data));

    // add link to DB
    if (persist)
    {
        PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_GRAVEYARD_ZONE);

        stmt->setUInt32(0, id);
        stmt->setUInt32(1, zoneId);
        stmt->setUInt16(2, uint16(team));

        WorldDatabase.Execute(stmt);
    }

    return true;
}

void ObjectMgr::RemoveGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool persist /*= false*/)
{
    GraveYardContainer::iterator graveLow  = GraveYardStore.lower_bound(zoneId);
    GraveYardContainer::iterator graveUp   = GraveYardStore.upper_bound(zoneId);
    if (graveLow == graveUp)
    {
        //sLog->outError(LOG_FILTER_SQL, "Table `game_graveyard_zone` incomplete: Zone %u Team %u does not have a linked graveyard.", zoneId, team);
        return;
    }

    bool found = false;

    GraveYardContainer::iterator itr;

    for (itr = graveLow; itr != graveUp; ++itr)
    {
        GraveYardData & data = itr->second;

        // skip not matching safezone id
        if (data.safeLocId != id)
            continue;

        // skip enemy faction graveyard at same map (normal area, city, or battleground)
        // team == 0 case can be at call from .neargrave
        if (data.team != 0 && team != 0 && data.team != team)
            continue;

        found = true;
        break;
    }

    // no match, return
    if (!found)
        return;

    // remove from links
    GraveYardStore.erase(itr);

    // remove link from DB
    if (persist)
    {
        PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GRAVEYARD_ZONE);

        stmt->setUInt32(0, id);
        stmt->setUInt32(1, zoneId);
        stmt->setUInt16(2, uint16(team));

        WorldDatabase.Execute(stmt);
    }
}

void ObjectMgr::LoadAreaTriggerTeleports()
{
    uint32 oldMSTime = getMSTime();

    _areaTriggerStore.clear();                                  // need for reload case

    //                                                        0            1                  2                  3                  4                   5
    QueryResult result = WorldDatabase.Query("SELECT id,  target_map, target_position_x, target_position_y, target_position_z, target_orientation FROM areatrigger_teleport");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 area trigger teleport definitions. DB table `areatrigger_teleport` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        ++count;

        uint32 Trigger_ID = fields[0].GetUInt32();

        AreaTriggerStruct at;

        at.source_mapId             = 0;
        at.target_mapId             = fields[1].GetUInt16();
        at.target_X                 = fields[2].GetFloat();
        at.target_Y                 = fields[3].GetFloat();
        at.target_Z                 = fields[4].GetFloat();
        at.target_Orientation       = fields[5].GetFloat();
        at.radius                   = 0.0f; ///< unused in case of areatrigger

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Area trigger (ID:%u) does not exist in `AreaTrigger.db2`.", Trigger_ID);
            continue;
        }

        MapEntry const* mapEntry = sMapStore.LookupEntry(at.target_mapId);
        if (!mapEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Area trigger (ID:%u) target map (ID: %u) does not exist in `Map.db2`.", Trigger_ID, at.target_mapId);
            continue;
        }

        if (at.target_X == 0 && at.target_Y == 0 && at.target_Z == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Area trigger (ID:%u) target coordinates not provided.", Trigger_ID);
            continue;
        }

        _areaTriggerStore[Trigger_ID] = at;

    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u area trigger teleport definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadAreaTriggerForces()
{
    uint32 l_OldMSTime = getMSTime();

    _areaTriggerForceMap.clear();

    //                                                  0             1          2        3        4         5            6           7          8          9
    QueryResult result = WorldDatabase.Query("SELECT `AuraID`, `CustomEntry`, `windX`, `windY`, `windZ`, `windType`, `windSpeed`, `centerX`, `centerY`, `centerZ` FROM `areatrigger_force`");
    if (result)
    {
        uint32 counter = 0;
        do
        {
            Field* fields = result->Fetch();

            uint8 i = 0;
            AreaTriggerForce data;
            data.AuraID = fields[i++].GetUInt32();
            data.CustomEntry = fields[i++].GetUInt32();
            float windX = fields[i++].GetFloat();
            float windY = fields[i++].GetFloat();
            float windZ = fields[i++].GetFloat();
            data.wind = G3D::Vector3(windX, windY, windZ);
            data.windType = fields[i++].GetUInt8();
            data.windSpeed = fields[i++].GetFloat();
            float centerX = fields[i++].GetFloat();
            float centerY = fields[i++].GetFloat();
            float centerZ = fields[i++].GetFloat();
            data.center = G3D::Vector3(centerX, centerY, centerZ);

            _areaTriggerForceMap[data.AuraID].push_back(data);
            ++counter;
        }
        while (result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u areatrigger_force data in %u ms.", counter, GetMSTimeDiffToNow(l_OldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 areatrigger_force data. DB table `areatrigger_force` is empty.");
}

std::vector<AreaTriggerForce> const* ObjectMgr::GetAreaTriggerForce(uint32 AuraID) const
{
    AreaTriggerForceMap::const_iterator itr = _areaTriggerForceMap.find(AuraID);
    return itr != _areaTriggerForceMap.end() ? &(itr->second) : NULL;
}

void ObjectMgr::LoadAccessRequirements()
{
    uint32 oldMSTime = getMSTime();

    _accessRequirementStore.clear();                                  // need for reload case

    //                                               0      1           2          3          4     5      6             7             8                      9                     10             11               12
    QueryResult result = WorldDatabase.Query("SELECT mapid, difficulty, level_min, level_max, item, item2, quest_done_A, quest_done_H, completed_achievement, leader_achievement, itemlevel_min, itemlevel_max, quest_failed_text FROM access_requirement");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 access requirement definitions. DB table `access_requirement` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        ++count;

        uint32 mapid = fields[0].GetUInt32();
        uint8 difficulty = fields[1].GetUInt8();
        uint32 requirement_ID = MAKE_PAIR32(mapid, difficulty);

        AccessRequirement ar;

        ar.levelMin                 = fields[2].GetUInt8();
        ar.levelMax                 = fields[3].GetUInt8();
        ar.item                     = fields[4].GetUInt32();
        ar.item2                    = fields[5].GetUInt32();
        ar.quest_A                  = fields[6].GetUInt32();
        ar.quest_H                  = fields[7].GetUInt32();
        ar.achievement              = fields[8].GetUInt32();
        ar.leader_achievement       = fields[9].GetUInt32();
        ar.itemlevelMin             = fields[10].GetUInt32();
        ar.itemlevelMax             = fields[11].GetUInt32();
        ar.questFailedText          = fields[12].GetString();

        if (ar.item)
        {
            ItemTemplate const* pProto = GetItemTemplate(ar.item);
            if (!pProto)
            {
                sLog->outError(LOG_FILTER_GENERAL, "Key item %u does not exist for map %u difficulty %u, removing key requirement.", ar.item, mapid, difficulty);
                ar.item = 0;
            }
        }

        if (ar.item2)
        {
            ItemTemplate const* pProto = GetItemTemplate(ar.item2);
            if (!pProto)
            {
                sLog->outError(LOG_FILTER_GENERAL, "Second item %u does not exist for map %u difficulty %u, removing key requirement.", ar.item2, mapid, difficulty);
                ar.item2 = 0;
            }
        }

        if (ar.quest_A)
        {
            if (!GetQuestTemplate(ar.quest_A))
            {
                sLog->outError(LOG_FILTER_SQL, "Required Alliance Quest %u not exist for map %u difficulty %u, remove quest done requirement.", ar.quest_A, mapid, difficulty);
                ar.quest_A = 0;
            }
        }

        if (ar.quest_H)
        {
            if (!GetQuestTemplate(ar.quest_H))
            {
                sLog->outError(LOG_FILTER_SQL, "Required Horde Quest %u not exist for map %u difficulty %u, remove quest done requirement.", ar.quest_H, mapid, difficulty);
                ar.quest_H = 0;
            }
        }

        if (ar.achievement)
        {
            if (!sAchievementStore.LookupEntry(ar.achievement))
            {
                sLog->outError(LOG_FILTER_SQL, "Required Achievement %u not exist for map %u difficulty %u, remove quest done requirement.", ar.achievement, mapid, difficulty);
                ar.achievement = 0;
            }
        }

        if (ar.leader_achievement)
        {
            if (!sAchievementStore.LookupEntry(ar.leader_achievement))
            {
                sLog->outError(LOG_FILTER_SQL, "Required Leader achievement %u not exist for map %u difficulty %u, remove quest done requirement.", ar.leader_achievement, mapid, difficulty);
                ar.leader_achievement = 0;
            }
        }

        _accessRequirementStore[requirement_ID] = ar;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u access requirement definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadLFRAccessRequirements()
{
    uint32 l_OldMSTime = getMSTime();

    /// Needed for reload case
    m_LFRAccessRequirements.clear();

    ///                                                    0            1         2       3      4      5         6          7              8               9         10           11
    QueryResult l_Result = WorldDatabase.Query("SELECT dungeon_id, level_min, level_max, item, item2, quest_A, quest_H, achievement, leader_achievement, ilvl_min, ilvl_max, quest_failed_text FROM lfr_access_requirement");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 LFR access requirement definitions. DB table `lfr_access_requirement` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        LFRAccessRequirement l_LFRAccessReq;

        Field* l_Fields                     = l_Result->Fetch();
        uint32 l_Index                      = 0;
        uint32 l_DungeonID                  = l_Fields[l_Index++].GetUInt32();

        l_LFRAccessReq.LevelMin             = l_Fields[l_Index++].GetUInt8();
        l_LFRAccessReq.LevelMax             = l_Fields[l_Index++].GetUInt8();
        l_LFRAccessReq.Item                 = l_Fields[l_Index++].GetUInt32();
        l_LFRAccessReq.Item2                = l_Fields[l_Index++].GetUInt32();
        l_LFRAccessReq.QuestA               = l_Fields[l_Index++].GetUInt32();
        l_LFRAccessReq.QuestH               = l_Fields[l_Index++].GetUInt32();
        l_LFRAccessReq.Achievement          = l_Fields[l_Index++].GetUInt32();
        l_LFRAccessReq.LeaderAchievement    = l_Fields[l_Index++].GetUInt32();
        l_LFRAccessReq.ItemLevelMin         = l_Fields[l_Index++].GetUInt32();
        l_LFRAccessReq.ItemLevelMax         = l_Fields[l_Index++].GetUInt32();
        l_LFRAccessReq.QuestFailedText      = l_Fields[l_Index++].GetString();

        if (l_LFRAccessReq.Item)
        {
            ItemTemplate const* l_ItemTemplate = GetItemTemplate(l_LFRAccessReq.Item);
            if (!l_ItemTemplate)
            {
                sLog->outError(LOG_FILTER_GENERAL, "Key item %u does not exist for dungeon ID %u, removing key requirement.", l_LFRAccessReq.Item, l_DungeonID);
                l_LFRAccessReq.Item = 0;
            }
        }

        if (l_LFRAccessReq.Item2)
        {
            ItemTemplate const* l_ItemTemplate = GetItemTemplate(l_LFRAccessReq.Item2);
            if (!l_ItemTemplate)
            {
                sLog->outError(LOG_FILTER_GENERAL, "Second item %u does not exist for dungeon ID %u, removing key requirement.", l_LFRAccessReq.Item2, l_DungeonID);
                l_LFRAccessReq.Item2 = 0;
            }
        }

        if (l_LFRAccessReq.QuestA)
        {
            if (!GetQuestTemplate(l_LFRAccessReq.QuestA))
            {
                sLog->outError(LOG_FILTER_SQL, "Required Alliance Quest %u not exist for dungeon ID %u, remove quest done requirement.", l_LFRAccessReq.QuestA, l_DungeonID);
                l_LFRAccessReq.QuestA = 0;
            }
        }

        if (l_LFRAccessReq.QuestH)
        {
            if (!GetQuestTemplate(l_LFRAccessReq.QuestH))
            {
                sLog->outError(LOG_FILTER_SQL, "Required Horde Quest %u not exist for dungeon ID %u, remove quest done requirement.", l_LFRAccessReq.QuestH, l_DungeonID);
                l_LFRAccessReq.QuestH = 0;
            }
        }

        if (l_LFRAccessReq.Achievement)
        {
            if (!sAchievementStore.LookupEntry(l_LFRAccessReq.Achievement))
            {
                sLog->outError(LOG_FILTER_SQL, "Required Achievement %u not exist for dungeon ID %u, remove quest done requirement.", l_LFRAccessReq.Achievement, l_DungeonID);
                l_LFRAccessReq.Achievement = 0;
            }
        }

        if (l_LFRAccessReq.LeaderAchievement)
        {
            if (!sAchievementStore.LookupEntry(l_LFRAccessReq.LeaderAchievement))
            {
                sLog->outError(LOG_FILTER_SQL, "Required Leader achievement %u not exist for dungeon ID %u, remove quest done requirement.", l_LFRAccessReq.LeaderAchievement, l_DungeonID);
                l_LFRAccessReq.LeaderAchievement = 0;
            }
        }

        m_LFRAccessRequirements[l_DungeonID] = l_LFRAccessReq;
        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u lfr access requirement definitions in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

/*
 * Searches for the areatrigger which teleports players out of the given map with instance_template.parent field support
 */
AreaTriggerStruct const* ObjectMgr::GetGoBackTrigger(uint32 Map) const
{
    bool useParentDbValue = false;
    uint32 parentId = 0;
    const MapEntry* mapEntry = sMapStore.LookupEntry(Map);
    if (!mapEntry)
        return NULL;

    if (mapEntry->IsDungeon())
    {
        const InstanceTemplate* iTemplate = sObjectMgr->GetInstanceTemplate(Map);

        if (!iTemplate)
            return NULL;

        parentId = iTemplate->Parent;
        useParentDbValue = true;
    }
    else if (mapEntry->CorpseMapID < 0)
        return nullptr;

    uint32 entrance_map = uint32(mapEntry->CorpseMapID);
    for (AreaTriggerContainer::const_iterator itr = _areaTriggerStore.begin(); itr != _areaTriggerStore.end(); ++itr)
    {
        if ((!useParentDbValue && itr->second.target_mapId == entrance_map) || (useParentDbValue && itr->second.target_mapId == parentId))
        {
            AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(itr->first);
            if (atEntry && atEntry->ContinentID == Map)
                return &itr->second;
        }
    }

    return NULL;
}

/**
 * Searches for the areatrigger which teleports players to the given map
 */
AreaTriggerStruct const* ObjectMgr::GetMapEntranceTrigger(uint32 Map) const
{
    for (AreaTriggerContainer::const_iterator itr = _areaTriggerStore.begin(); itr != _areaTriggerStore.end(); ++itr)
    {
        if (itr->second.target_mapId == Map)
        {
            AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(itr->first);
            if (atEntry)
                return &itr->second;
        }
    }

    /// Should be unique structs for Dungeons AT-Teleport
    for (auto& l_Iter : m_GameobjectAreaTriggerStore)
    {
        if (l_Iter.second.target_mapId == Map)
            return &l_Iter.second;
    }

    return NULL;
}

AreaTriggerMoveSplines ObjectMgr::GetAreaTriggerMoveSplines(uint32 p_move_curve_id, uint32 p_path_id)
{
    auto l_itr = m_AreaTriggerMoveSplines.find(std::make_pair(p_move_curve_id, p_path_id));
    if (l_itr != m_AreaTriggerMoveSplines.end())
        return l_itr->second;

    return AreaTriggerMoveSplines();
}

uint32 ObjectMgr::GetAreaTriggerPathSize(uint32 p_move_curve_id)
{
    uint32 l_PathSize = 0;
    for (auto l_Itr = m_AreaTriggerMoveSplines.begin(); l_Itr != m_AreaTriggerMoveSplines.end(); ++l_Itr)
    {
        if (l_Itr->first.first == p_move_curve_id)
            ++l_PathSize;
    }
    return l_PathSize;
}

uint32 ObjectMgr::GetAreatriggerMoveDuration(SpellInfo const* p_SpellInfo)
{
    if (p_SpellInfo)
        return p_SpellInfo->GetDuration();
    return 0;
}

void ObjectMgr::SetHighestGuids()
{
#ifndef CROSS
    QueryResult result = CharacterDatabase.Query("SELECT MAX(guid) FROM characters");
    if (result)
        _hiCharGuid = (*result)[0].GetUInt32()+1;

    result = WorldDatabase.Query("SELECT MAX(guid) FROM creature");
#else /* CROSS */
    QueryResult result = WorldDatabase.Query("SELECT MAX(guid) FROM creature");
#endif /* CROSS */
    if (result)
        _hiCreatureGuid = (*result)[0].GetUInt32()+1;

    _hiCreatureGuid += 10000;

#ifndef CROSS
    /// Prevent item guid counter issues
    CharacterDatabase.PQuery("DELETE FROM item_instance WHERE guid = 4294967295");

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM item_instance");
    if (result)
        m_HighItemGuid = (*result)[0].GetUInt32()+1;

#endif /* not CROSS */
    result = WorldDatabase.Query("SELECT MAX(guid) FROM gameobject");
    if (result)
        _hiGoGuid = (*result)[0].GetUInt32()+1;

    result = WorldDatabase.Query("SELECT MAX(guid) FROM transports");
    if (result)
        _hiMoTransGuid = (*result)[0].GetUInt32()+1;

#ifndef CROSS
    result = CharacterDatabase.Query("SELECT MAX(id) FROM auctionhouse");
    if (result)
        _auctionId = (*result)[0].GetUInt32()+1;

    result = CharacterDatabase.Query("SELECT MAX(id) FROM mail");
    if (result)
        m_MailId = (*result)[0].GetUInt32()+1;

    /// Prevent deleted mails to decrease the max id of mails and cause duplicate key SQL errors
    result = CharacterDatabase.Query("SELECT MAX(id) FROM log_mail");
    if (result)
        m_MailId = std::max((*result)[0].GetUInt32() + 1, (uint32)m_MailId);

    result = CharacterDatabase.Query("SELECT MAX(corpseGuid) FROM corpse");
    if (result)
        _hiCorpseGuid = (*result)[0].GetUInt32()+1;

    result = CharacterDatabase.Query("SELECT MAX(setguid) FROM character_equipmentsets");
    if (result)
        m_EquipmentSetGuid = (*result)[0].GetUInt64()+1;

    result = CharacterDatabase.Query("SELECT MAX(guildId) FROM guild");
    if (result)
        sGuildMgr->SetNextGuildId((*result)[0].GetUInt32()+1);

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM groups");
    if (result)
        sGroupMgr->SetGroupDbStoreSize((*result)[0].GetUInt32()+1);

    result = CharacterDatabase.Query("SELECT MAX(itemId) from character_void_storage");
    if (result)
        _voidItemId = (*result)[0].GetUInt64()+1;

    result = CharacterDatabase.Query("SELECT MAX(id) from character_garrison");
    if (result)
        m_GarrisonID = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) from character_garrison_draenor_building");
    if (result)
        m_GarrisonBuildingID = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) from character_garrison_follower");
    if (result)
        m_GarrisonFollowerID = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) from character_garrison_mission");
    if (result)
        m_GarrisonMissionID = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) from character_garrison_work_order");
    if (result)
        m_GarrisonWorkOrderID = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) FROM challenge");
    if (result)
        m_ScenarioID = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(AttemptID) FROM guild_encounter_data");
    if (result)
        m_EncounterAttemptID = (*result)[0].GetUInt64() + 1;

#else /* CROSS */
    _hiCharGuid                 = 1;
    m_HighItemGuid              = 1;
    _auctionId                  = 1;
    _mailId                     = 1;
    _hiCorpseGuid               = 1;
    _equipmentSetGuid           = 1;
    _voidItemId                 = 1;
    m_GarrisonID                = 1;
    m_GarrisonBuildingID        = 1;
    m_GarrisonFollowerID        = 1;
    m_GarrisonMissionID         = 1;
    m_GarrisonWorkOrderID       = 1;
#endif /* CROSS */
    m_StandaloneSceneInstanceID = 1;
#ifdef CROSS

    m_GarrisonSiteBaseID = 1;

    sGroupMgr->SetGroupDbStoreSize(1);
#endif /* CROSS */
}

uint32 ObjectMgr::GenerateAuctionID()
{
    if (_auctionId >= 0xFFFFFFFE)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Auctions ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return _auctionId++;
}

#ifndef CROSS
uint64 ObjectMgr::GenerateEquipmentSetGuid(uint32 p_Range)
#else /* CROSS */
uint64 ObjectMgr::GenerateEquipmentSetGuid()
#endif /* CROSS */
{
#ifndef CROSS
    if (m_EquipmentSetGuid >= uint64(0xFFFFFFFFFFFFFFFELL))
#else /* CROSS */
    if (_equipmentSetGuid >= uint64(0xFFFFFFFFFFFFFFFELL))
#endif /* CROSS */
    {
        sLog->outError(LOG_FILTER_GENERAL, "EquipmentSet guid overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
#ifndef CROSS
    return m_EquipmentSetGuid.fetch_add(p_Range);
#else /* CROSS */
    return _equipmentSetGuid++;
#endif /* CROSS */
}

#ifndef CROSS
uint32 ObjectMgr::GenerateMailID(uint32 p_Range)
#else /* CROSS */
uint32 ObjectMgr::GenerateMailID()
#endif /* CROSS */
{
#ifndef CROSS
    if (m_MailId >= 0xFFFFFFFE)
#else /* CROSS */
    if (_mailId >= 0xFFFFFFFE)
#endif /* CROSS */
    {
        sLog->outError(LOG_FILTER_GENERAL, "Mail ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
#ifndef CROSS

    return m_MailId.fetch_add(p_Range);
}

uint32 ObjectMgr::GenerateLowGuid(HighGuid p_GuidHigh, uint32 p_Range)
{
    switch (p_GuidHigh)
    {
        case HIGHGUID_ITEM:
            ASSERT(m_HighItemGuid < 0xFFFFFFFE && "Item guid overflow!");
            return m_HighItemGuid.fetch_add(p_Range);
            break;
        case HIGHGUID_MAIL:
            return GenerateMailID(p_Range);
            break;
        case HIGHGUID_PET_NUMBER:
            return GeneratePetNumber(p_Range);
            break;
        case HIGHGUID_EQUIPMENT_SET:
            return GenerateEquipmentSetGuid(p_Range);
            break;
        default:
            break;
    }

    ASSERT(false);
    return 0;
#else /* CROSS */
    return _mailId++;
#endif /* CROSS */
}

uint32 ObjectMgr::GenerateLowGuid(HighGuid guidhigh)
{
    switch (guidhigh)
    {
        case HIGHGUID_ITEM:
        {
            ASSERT(m_HighItemGuid < 0xFFFFFFFE && "Item guid overflow!");
            return m_HighItemGuid.fetch_add(1);
        }
        case HIGHGUID_UNIT:
        {
            ASSERT(_hiCreatureGuid < 0xFFFFFFFE && "Creature guid overflow!");
            return _hiCreatureGuid++;
        }
        case HIGHGUID_PET:
        {
            ASSERT(_hiPetGuid < 0xFFFFFFFE && "Pet guid overflow!");
            return _hiPetGuid++;
        }
        case HIGHGUID_VEHICLE:
        {
            ASSERT(_hiVehicleGuid < 0xFFFFFFFE && "Vehicle guid overflow!");
            return _hiVehicleGuid++;
        }
        case HIGHGUID_PLAYER:
        {
            ASSERT(_hiCharGuid < 0xFFFFFFFE && "Player guid overflow!");
            return _hiCharGuid++;
        }
        case HIGHGUID_GAMEOBJECT:
        {
            ASSERT(_hiGoGuid < 0xFFFFFFFE && "Gameobject guid overflow!");
            return _hiGoGuid++;
        }
        case HIGHGUID_CORPSE:
        {
            ASSERT(_hiCorpseGuid < 0xFFFFFFFE && "Corpse guid overflow!");
            return _hiCorpseGuid++;
        }
        case HIGHGUID_DYNAMICOBJECT:
        {
            ASSERT(_hiDoGuid < 0xFFFFFFFE && "DynamicObject guid overflow!");
            return _hiDoGuid++;
        }
        case HIGHGUID_AREATRIGGER:
        {
            ASSERT(_hiAreaTriggerGuid < 0xFFFFFFFE && "AreaTrigger guid overflow!");
            return _hiAreaTriggerGuid++;
        }
        case HIGHGUID_MO_TRANSPORT:
        {
            ASSERT(_hiMoTransGuid < 0xFFFFFFFE && "MO Transport guid overflow!");
            return _hiMoTransGuid++;
        }
        case HIGHGUID_CONVERSATION:
        {
            ASSERT(m_HighConversationGuid < 0xFFFFFFFE && "Conversation guid overflow!");
            return m_HighConversationGuid++;
        }
        case HIGHGUID_EVENTOBJECT:
        {
            ASSERT(m_HighEventObjectGuid < 0xFFFFFFFE && "EventObject guid overflow!");
            return m_HighEventObjectGuid++;
        }
        default:
            ASSERT(false && "ObjectMgr::GenerateLowGuid - Unknown HIGHGUID type");
            return 0;
    }
}

void ObjectMgr::LoadGameObjectLocales()
{
    uint32 oldMSTime = getMSTime();

    _gameObjectLocaleStore.clear();                           // need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, "
        "name_loc1, name_loc2, name_loc3, name_loc4, name_loc5, name_loc6, name_loc7, name_loc8, name_loc9, name_loc10, "
        "castbarcaption_loc1, castbarcaption_loc2, castbarcaption_loc3, castbarcaption_loc4, "
        "castbarcaption_loc5, castbarcaption_loc6, castbarcaption_loc7, castbarcaption_loc8, castbarcaption_loc9, castbarcaption_loc10 FROM locales_gameobject");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        GameObjectLocale& data = _gameObjectLocaleStore[entry];

        for (uint8 i = 1; i < TOTAL_LOCALES; ++i)
            AddLocaleString(fields[i].GetString(), LocaleConstant(i), data.Name);

        for (uint8 i = 1; i < TOTAL_LOCALES; ++i)
            AddLocaleString(fields[i + (TOTAL_LOCALES - 1)].GetString(), LocaleConstant(i), data.CastBarCaption);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu gameobject locale strings in %u ms", (unsigned long)_gameObjectLocaleStore.size(), GetMSTimeDiffToNow(oldMSTime));
}

inline void CheckGOLockId(GameObjectTemplate const* goInfo, uint32 dataN, uint32 N)
{
    if (sLockStore.LookupEntry(dataN))
        return;

    sLog->outError(LOG_FILTER_SQL, "Gameobject (Entry: %u GoType: %u) have data%d=%u but lock (Id: %u) not found.",
        goInfo->entry, goInfo->type, N, goInfo->door.open, goInfo->door.open);
}

inline void CheckGOLinkedTrapId(GameObjectTemplate const* goInfo, uint32 dataN, uint32 N)
{
    if (GameObjectTemplate const* trapInfo = sObjectMgr->GetGameObjectTemplate(dataN))
    {
        if (trapInfo->type != GAMEOBJECT_TYPE_TRAP)
            sLog->outError(LOG_FILTER_SQL, "Gameobject (Entry: %u GoType: %u) have data%d=%u but GO (Entry %u) have not GAMEOBJECT_TYPE_TRAP (%u) type.",
            goInfo->entry, goInfo->type, N, dataN, dataN, GAMEOBJECT_TYPE_TRAP);
    }
}

inline void CheckGOSpellId(GameObjectTemplate const* goInfo, uint32 dataN, uint32 N)
{
    if (sSpellMgr->GetSpellInfo(dataN))
        return;

    sLog->outError(LOG_FILTER_SQL, "Gameobject (Entry: %u GoType: %u) have data%d=%u but Spell (Entry %u) not exist.",
        goInfo->entry, goInfo->type, N, dataN, dataN);
}

inline void CheckAndFixGOChairHeightId(GameObjectTemplate const* goInfo, uint32 const& dataN, uint32 N)
{
    if (dataN <= (UNIT_STAND_STATE_SIT_HIGH_CHAIR-UNIT_STAND_STATE_SIT_LOW_CHAIR))
        return;

    sLog->outError(LOG_FILTER_SQL, "Gameobject (Entry: %u GoType: %u) have data%d=%u but correct chair height in range 0..%i.",
        goInfo->entry, goInfo->type, N, dataN, UNIT_STAND_STATE_SIT_HIGH_CHAIR-UNIT_STAND_STATE_SIT_LOW_CHAIR);

    // prevent client and server unexpected work
    const_cast<uint32&>(dataN) = 0;
}

inline void CheckGONoDamageImmuneId(GameObjectTemplate* goTemplate, uint32 dataN, uint32 N)
{
    // 0/1 correct values
    if (dataN <= 1)
        return;

    sLog->outError(LOG_FILTER_SQL, "Gameobject (Entry: %u GoType: %u) have data%d=%u but expected boolean (0/1) noDamageImmune field value.", goTemplate->entry, goTemplate->type, N, dataN);
}

inline void CheckGOConsumable(GameObjectTemplate const* goInfo, uint32 dataN, uint32 N)
{
    // 0/1 correct values
    if (dataN <= 1)
        return;

    sLog->outError(LOG_FILTER_SQL, "Gameobject (Entry: %u GoType: %u) have data%d=%u but expected boolean (0/1) consumable field value.",
        goInfo->entry, goInfo->type, N, dataN);
}

void ObjectMgr::LoadGameObjectTemplate()
{
    uint32 oldMSTime = getMSTime();

    //                                                 0      1      2        3       4             5          6      7       8     9        10         11          12
    QueryResult result = WorldDatabase.Query("SELECT entry, type, displayId, name, IconName, castBarCaption, unk1, faction, flags, size, questItem1, questItem2, questItem3, "
    //                                            13          14          15       16     17     18     19     20     21     22     23     24     25      26      27      28
                                             "questItem4, questItem5, questItem6, data0, data1, data2, data3, data4, data5, data6, data7, data8, data9, data10, data11, data12, "
    //                                          29      30      31      32      33      34      35      36      37      38      39      40      41      42      43      44
                                             "data13, data14, data15, data16, data17, data18, data19, data20, data21, data22, data23, data24, data25, data26, data27, data28, "
    //                                          45      46      47       48       49             50            51        52
                                             "data29, data30, data31,  data32, RequiredLevel, WorldEffectID, AIName, ScriptName "
                                             "FROM gameobject_template");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 gameobject definitions. DB table `gameobject_template` is empty.");
        return;
    }

    //_gameObjectTemplateStore.rehash(result->GetRowCount());
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        GameObjectTemplate& got = _gameObjectTemplateStore[entry];

        got.entry          = entry;
        got.type           = uint32(fields[1].GetUInt8());
        got.displayId      = fields[2].GetUInt32();
        got.name           = fields[3].GetString();
        got.IconName       = fields[4].GetString();
        got.castBarCaption = fields[5].GetString();
        got.unk1           = fields[6].GetString();
        got.faction        = uint32(fields[7].GetUInt16());
        got.flags          = fields[8].GetUInt32();
        got.size           = got.CanScale() ? fields[9].GetFloat() : 1.0f;

        for (uint8 i = 0; i < MAX_GAMEOBJECT_QUEST_ITEMS; ++i)
            got.questItems[i] = fields[10 + i].GetUInt32();

        for (uint8 i = 0; i < MAX_GAMEOBJECT_DATA; ++i)
            got.raw.data[i] = fields[16 + i].GetUInt32();

        got.RequiredLevel = fields[49].GetInt32();
        got.WorldEffectID = fields[50].GetUInt32();
        got.AIName = fields[51].GetString();
        got.ScriptId = GetScriptId(fields[52].GetCString());

        // Checks

        switch (got.type)
        {
            case GAMEOBJECT_TYPE_DOOR:                      //0
            {
                if (got.door.open)
                    CheckGOLockId(&got, got.door.open, 1);
                CheckGONoDamageImmuneId(&got, got.door.noDamageImmune, 3);
                break;
            }
            case GAMEOBJECT_TYPE_BUTTON:                    //1
            {
                if (got.button.open)
                    CheckGOLockId(&got, got.button.open, 1);
                CheckGONoDamageImmuneId(&got, got.button.noDamageImmune, 4);
                break;
            }
            case GAMEOBJECT_TYPE_QUESTGIVER:                //2
            {
                if (got.questgiver.open)
                    CheckGOLockId(&got, got.questgiver.open, 0);
                CheckGONoDamageImmuneId(&got, got.questgiver.noDamageImmune, 5);
                break;
            }
            case GAMEOBJECT_TYPE_CHEST:                     //3
            {
                if (got.chest.open)
                    CheckGOLockId(&got, got.chest.open, 0);

                CheckGOConsumable(&got, got.chest.consumable, 3);

                if (got.chest.linkedTrap)              // linked trap
                    CheckGOLinkedTrapId(&got, got.chest.linkedTrap, 7);
                break;
            }
            case GAMEOBJECT_TYPE_TRAP:                      //6
            {
                if (got.trap.open)
                    CheckGOLockId(&got, got.trap.open, 0);
                break;
            }
            case GAMEOBJECT_TYPE_CHAIR:                     //7
                CheckAndFixGOChairHeightId(&got, got.chair.chairheight, 1);
                break;
            case GAMEOBJECT_TYPE_SPELL_FOCUS:               //8
            {
                if (got.spellFocus.spellFocusType)
                {
                    if (!sSpellFocusObjectStore.LookupEntry(got.spellFocus.spellFocusType))
                        sLog->outError(LOG_FILTER_SQL, "GameObject (Entry: %u GoType: %u) have data0=%u but SpellFocus (Id: %u) not exist.",
                        entry, got.type, got.spellFocus.spellFocusType, got.spellFocus.spellFocusType);
                }

                if (got.spellFocus.linkedTrap)        // linked trap
                    CheckGOLinkedTrapId(&got, got.spellFocus.linkedTrap, 2);
                break;
            }
            case GAMEOBJECT_TYPE_GOOBER:                    //10
            {
                if (got.goober.open)
                    CheckGOLockId(&got, got.goober.open, 0);

                CheckGOConsumable(&got, got.goober.consumable, 3);

                if (got.goober.pageID)                  // pageId
                {
                    if (!GetPageText(got.goober.pageID))
                        sLog->outError(LOG_FILTER_SQL, "GameObject (Entry: %u GoType: %u) have data7=%u but PageText (Entry %u) not exist.",
                        entry, got.type, got.goober.pageID, got.goober.pageID);
                }
                CheckGONoDamageImmuneId(&got, got.goober.noDamageImmune, 11);
                if (got.goober.linkedTrap)            // linked trap
                    CheckGOLinkedTrapId(&got, got.goober.linkedTrap, 12);
                break;
            }
            case GAMEOBJECT_TYPE_AREADAMAGE:                //12
            {
                if (got.areaDamage.open)
                    CheckGOLockId(&got, got.areaDamage.open, 0);
                break;
            }
            case GAMEOBJECT_TYPE_CAMERA:                    //13
            {
                if (got.camera.open)
                    CheckGOLockId(&got, got.camera.open, 0);
                break;
            }
            case GAMEOBJECT_TYPE_MAP_OBJ_TRANSPORT:              //15
            {
                if (got.moTransport.taxiPathID)
                {
                    if (got.moTransport.taxiPathID >= sTaxiPathNodesByPath.size() || sTaxiPathNodesByPath[got.moTransport.taxiPathID].empty())
                        sLog->outError(LOG_FILTER_SQL, "GameObject (Entry: %u GoType: %u) have data0=%u but TaxiPath (Id: %u) not exist.",
                        entry, got.type, got.moTransport.taxiPathID, got.moTransport.taxiPathID);
                }
                break;
            }
            case GAMEOBJECT_TYPE_RITUAL:          //18
                break;
            case GAMEOBJECT_TYPE_SPELLCASTER:               //22
            {
                // always must have spell
                CheckGOSpellId(&got, got.spellCaster.spell, 0);
                break;
            }
            case GAMEOBJECT_TYPE_FLAGSTAND:                 //24
            {
                if (got.flagStand.open)
                    CheckGOLockId(&got, got.flagStand.open, 0);
                CheckGONoDamageImmuneId(&got, got.flagStand.noDamageImmune, 5);
                break;
            }
            case GAMEOBJECT_TYPE_FISHINGHOLE:               //25
            {
                if (got.fishingHole.open)
                    CheckGOLockId(&got, got.fishingHole.open, 4);
                break;
            }
            case GAMEOBJECT_TYPE_FLAGDROP:                  //26
            {
                if (got.flagDrop.open)
                    CheckGOLockId(&got, got.flagDrop.open, 0);
                CheckGONoDamageImmuneId(&got, got.flagDrop.noDamageImmune, 3);
                break;
            }
            case GAMEOBJECT_TYPE_BARBER_CHAIR:              //32
                CheckAndFixGOChairHeightId(&got, got.barberChair.chairheight, 0);
                break;
        }

       ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u game object templates in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGarrisonPlotBuildingContent()
{
    uint32 l_StartTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query("SELECT id, plot_type_or_building, faction_index, creature_or_gob, x, y, z, o FROM garrison_plot_content");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 garrison plot building content. DB table `garrison_plot_building_content` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();

        GarrisonPlotBuildingContent l_Content;
        l_Content.DB_ID                 = l_Fields[0].GetUInt32();
        l_Content.PlotTypeOrBuilding    = l_Fields[1].GetInt32();
        l_Content.FactionIndex          = l_Fields[2].GetUInt32();
        l_Content.CreatureOrGob         = l_Fields[3].GetInt32();
        l_Content.X                     = l_Fields[4].GetFloat();
        l_Content.Y                     = l_Fields[5].GetFloat();
        l_Content.Z                     = l_Fields[6].GetFloat();
        l_Content.O                     = l_Fields[7].GetFloat();

        m_GarrisonPlotBuildingContents.push_back(l_Content);

        ++l_Count;
    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u garrison plot building content in %u ms", l_Count, GetMSTimeDiffToNow(l_StartTime));
}

void ObjectMgr::AddGarrisonPlotBuildingContent(GarrisonPlotBuildingContent & p_Data)
{
    WorldDatabase.PQuery("INSERT INTO garrison_plot_content(plot_type_or_building, faction_index, creature_or_gob, x, y, z, o) VALUES "
        "(%d, %u, %d, %f, %f, %f, %f) ", p_Data.PlotTypeOrBuilding, p_Data.FactionIndex, p_Data.CreatureOrGob, p_Data.X, p_Data.Y, p_Data.Z, p_Data.O);

    QueryResult l_Result = WorldDatabase.PQuery("SELECT id FROM garrison_plot_content WHERE plot_type_or_building=%d AND faction_index=%u AND creature_or_gob=%d AND x BETWEEN %f AND %f AND y BETWEEN %f AND %f AND z BETWEEN %f AND %f",
                                                p_Data.PlotTypeOrBuilding, p_Data.FactionIndex, p_Data.CreatureOrGob, p_Data.X - 0.5f, p_Data.X + 0.5f, p_Data.Y - 0.5f, p_Data.Y + 0.5f, p_Data.Z - 0.5f, p_Data.Z + 0.5f);

    if (!l_Result)
        return;

    Field* l_Fields = l_Result->Fetch();

    p_Data.DB_ID = l_Fields[0].GetUInt32();

    m_GarrisonPlotBuildingContents.push_back(p_Data);
}

void ObjectMgr::DeleteGarrisonPlotBuildingContent(GarrisonPlotBuildingContent & p_Data)
{
    auto l_It = std::find_if(m_GarrisonPlotBuildingContents.begin(), m_GarrisonPlotBuildingContents.end(), [p_Data](const GarrisonPlotBuildingContent & p_Elem) -> bool
    {
        return p_Elem.DB_ID == p_Data.DB_ID;
    });

    if (l_It != m_GarrisonPlotBuildingContents.end())
    {
        WorldDatabase.PQuery("DELETE FROM garrison_plot_content WHERE id=%u", p_Data.DB_ID);
        m_GarrisonPlotBuildingContents.erase(l_It);
    }
}

std::vector<GarrisonPlotBuildingContent> ObjectMgr::GetGarrisonPlotBuildingContent(int32 p_PlotTypeOrBuilding, uint32 p_FactionIndex)
{
    std::vector<GarrisonPlotBuildingContent> l_Data;

    for (uint32 l_I = 0; l_I < m_GarrisonPlotBuildingContents.size(); ++l_I)
    {
        if (m_GarrisonPlotBuildingContents[l_I].PlotTypeOrBuilding == p_PlotTypeOrBuilding && m_GarrisonPlotBuildingContents[l_I].FactionIndex == p_FactionIndex)
            l_Data.push_back(m_GarrisonPlotBuildingContents[l_I]);
    }

    return l_Data;
}

void ObjectMgr::LoadGarrisonMissionReward()
{
    uint32 l_StartTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query("SELECT `ID`, `GarrMissionID`, `FollowerXP`, `ItemID`, `ItemQuantity`, `CurrencyType`, `CurrencyQuantity`, `TreasureChance`, `TreasureXP`, `TreasureQuality`, `GarrMssnBonusAbilityID` FROM garrison_mission_rewards");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 garrison mission reward content. DB table `garrison_mission_rewards` is empty.");
        return;
    }

    uint32 l_Count = 0;

    enum RandomRewardType
    {
        None,
        FollowerXP,
        Item,
        Gold,
        Reputation,
        Max
    };

    static const uint8 RandomRewardTypeWeight[] =
    {
        100,    ///< None
        100,
        90,
        50,
        25,
        0       ///< Max
    };

    static const uint32 s_TokenList[] =
    {
        139020, ///< Valarjar
        139021, ///< Dreamweaver
        139023, ///< Court of Farondis
        139024, ///< Highmountain
        139025, ///< Wardens
        139026, ///< Nightfallen
    };
    static const uint32 s_TokenCount = sizeof(s_TokenList) / sizeof(s_TokenList[0]);

    do
    {
        Field* l_Fields = l_Result->Fetch();

        GarrisonMissionReward l_Content;
        l_Content.FollowerXP                = l_Fields[2].GetInt32();
        l_Content.ItemID                    = l_Fields[3].GetInt32();
        l_Content.ItemQuantity              = l_Fields[4].GetInt32();
        l_Content.CurrencyType              = l_Fields[5].GetInt32();
        l_Content.CurrencyQuantity          = l_Fields[6].GetInt32();
        l_Content.TreasureChance            = l_Fields[7].GetInt32();
        l_Content.TreasureXP                = l_Fields[8].GetInt32();
        l_Content.TreasureQuality           = l_Fields[9].GetInt32();
        l_Content.GarrMssnBonusAbilityID    = l_Fields[10].GetInt32();

        auto l_Mission = sGarrMissionStore.LookupEntry(l_Fields[1].GetInt32());

        if (l_Content.FollowerXP == 0 && l_Content.ItemID == 0 && l_Content.CurrencyQuantity == 0 && l_Content.GarrMssnBonusAbilityID == 0)
        {
            uint32 l_RewardType = RandomRewardType::None;

            for (auto l_I = l_RewardType; l_I < RandomRewardType::Max; ++l_I)
            {
                if (roll_chance_i(RandomRewardTypeWeight[l_I + 1]))
                    l_RewardType++;
                else
                    break;
            }

            if (l_Mission && l_Mission->GarrisonType == MS::Garrison::GarrisonType::GarrisonDraenor)
                l_RewardType = RandomRewardType::FollowerXP;

            switch (static_cast<RandomRewardType>(l_RewardType))
            {
                case FollowerXP:
                {
                    static const uint32 s_XPList[] =
                    {
                        100,
                        200,
                        500
                    };
                    static const uint32 s_XPCount = sizeof(s_XPList) / sizeof(s_XPList[0]);

                    l_Content.FollowerXP = s_XPList[urand(0, s_XPCount - 1)];

                    if (!l_Mission)
                        break;

                    if (l_Mission->RequiredLevel >= 105)
                        l_Content.FollowerXP = 1000;
                    else if (l_Mission->RequiredLevel == 110)
                        l_Content.FollowerXP = 3000;

                    break;
                }

                case Item:
                {
                    static const uint32 s_ItemList[] =
                    {
                        141928,     ///< (Artiufact power) Reaver's Harpoon Head
                        136412,     ///< (Equipment) Heavy Armor Set
                        139813,     ///< (Equipment) Swift Boots
                        140582,     ///< (Equipment) Bottomless Flask
                        139835,     ///< (Equipment) Marauder's Vestige
                        140572,     ///< (Equipment) Hasty Pocketwatch
                        139801      ///< (Equipment) Lucky Doodad
                    };
                    static const uint32 s_ItemCount = sizeof(s_ItemList) / sizeof(s_ItemList[0]);

                    l_Content.ItemID = s_ItemList[urand(0, s_ItemCount - 1)];
                    l_Content.ItemQuantity = 1;
                    break;
                }

                case Gold:
                {
                    static const uint32 s_MinGold = 375;
                    static const uint32 s_MaxGold = 699;

                    l_Content.CurrencyQuantity = urand(s_MinGold, s_MaxGold) * GOLD;
                    break;
                }

                case Reputation:
                {
                    l_Content.ItemID = s_TokenList[urand(0, s_TokenCount - 1)];
                    l_Content.ItemQuantity = 1;
                    break;
                }

                default:
                    break;
            }
        }

        if (l_Mission && l_Mission->GarrisonType == MS::Garrison::GarrisonType::GarrisonBrokenIsles)
        {
            l_Content.BonusGold = 0;
            l_Content.BonusItemID = 0;

            /// Gold
            if (l_Content.CurrencyType == 0 && l_Content.CurrencyQuantity)
                l_Content.BonusGold = static_cast<int>((l_Content.CurrencyQuantity / GOLD) / 25) * 25 * GOLD;   ///< Retail formula   OriginalGold - (OriginalGold % 25)
            else if (l_Content.ItemID)
            {
                /// Reputation token
                if (std::find(std::begin(s_TokenList), std::end(s_TokenList), l_Content.ItemID) != std::end(s_TokenList))
                    l_Content.BonusItemID = 141344;     ///< Tribute of the Broken Isles - http://www.wowhead.com/item=141344/tribute-of-the-broken-isles
                else if (l_Content.ItemID == 124124)
                    l_Content.BonusItemID = 124124;     ///< Blood of Sargeras - http://www.wowhead.com/item=124124/blood-of-sargeras
                else if (const ItemTemplate* l_ItemTemplate = sObjectMgr->GetItemTemplate(l_Content.ItemID))
                {
                    if (l_ItemTemplate->Spells[0].SpellId)
                    {
                        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_ItemTemplate->Spells[0].SpellId);

                        if (l_SpellInfo)
                        {
                            switch (l_SpellInfo->Effects[0].Effect)
                            {
                                case SPELL_EFFECT_GIVE_ARTIFACT_POWER:
                                case SPELL_EFFECT_EARN_ARTIFACT_POWER:
                                    if (l_Mission->RequiredLevel == 110)
                                        l_Content.BonusItemID = 141024;     ///< Seal of Leadership - http://www.wowhead.com/item=141024/seal-of-leadership
                                    else
                                        l_Content.BonusItemID = 141023;     ///< Seal of Victory - http://www.wowhead.com/item=141023/seal-of-victory
                                    break;

                                case SPELL_EFFECT_INCREASE_FOLLOWER_ITEM_LEVEL:
                                    l_Content.BonusItemID = 139879;     ///< Crate of Champion Equipment - http://www.wowhead.com/item=139879/crate-of-champion-equipment
                                    break;

                                case SPELL_EFFECT_TEACH_FOLLOWER_ABILITY:
                                    l_Content.BonusItemID = 136412;     ///< Heavy Armor Set - http://www.wowhead.com/item=136412/heavy-armor-set
                                    break;
                            }
                        }
                    }
                }
            }
            else if (l_Content.FollowerXP)
                l_Content.BonusItemID = 141028;         ///< Grimoire of Knowledge - http://www.wowhead.com/item=141028/grimoire-of-knowledge
        }

        m_GarrisonMissionRewards[l_Fields[1].GetInt32()].push_back(l_Content);

        ++l_Count;
    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u garrison mission reward content in %u ms", l_Count, GetMSTimeDiffToNow(l_StartTime));
}

std::vector<GarrisonMissionReward> const* ObjectMgr::GetMissionRewards(uint32 p_MissionID)
{
    auto const& l_Iter = m_GarrisonMissionRewards.find(p_MissionID);
    if (l_Iter != m_GarrisonMissionRewards.end())
        return &l_Iter->second;

    return nullptr;
}

void ObjectMgr::LoadNpcRecipesConditions()
{
    uint32 l_StartTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query("SELECT NpcEntry, RecipeID, PlayerConditionID FROM npc_recipe_condition");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 npc recipe condition. DB table `npc_recipe_condition` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();
        RecipesConditions l_Conditions;

        uint32 l_NpcID                 = l_Fields[0].GetUInt32();
        l_Conditions.RecipeID          = l_Fields[1].GetUInt32();
        l_Conditions.PlayerConditionID = l_Fields[2].GetUInt32();

        _NpcRecipesConditions[l_NpcID].push_back(l_Conditions);

        ++l_Count;
    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u npc recipe condition in %u ms", l_Count, GetMSTimeDiffToNow(l_StartTime));
}

void ObjectMgr::LoadExplorationBaseXP()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT level, basexp FROM exploration_basexp");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 BaseXP definitions. DB table `exploration_basexp` is empty.");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint8 level  = fields[0].GetUInt8();
        uint32 basexp = fields[1].GetInt32();
        _baseXPTable[level] = basexp;
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u BaseXP definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

uint32 ObjectMgr::GetBaseXP(uint8 level)
{
    return _baseXPTable[level] ? _baseXPTable[level] : 0;
}

uint32 ObjectMgr::GetXPForLevel(uint8 level) const
{
    if (level + 1 < _playerXPperLevel.size())
        return _playerXPperLevel[level];
    return 0;
}

void ObjectMgr::LoadPetNames()
{
    uint32 oldMSTime = getMSTime();
    //                                                0     1      2
    QueryResult result = WorldDatabase.Query("SELECT word, entry, half FROM pet_name_generation");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 pet name parts. DB table `pet_name_generation` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        std::string word = fields[0].GetString();
        uint32 entry     = fields[1].GetUInt32();
        bool   half      = fields[2].GetBool();
        if (half)
            _petHalfName1[entry].push_back(word);
        else
            _petHalfName0[entry].push_back(word);
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u pet name parts in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPetNumber()
{
    uint32 oldMSTime = getMSTime();

#ifndef CROSS
    QueryResult result = CharacterDatabase.Query("SELECT MAX(id) FROM character_pet");
    if (result)
    {
        Field* fields = result->Fetch();
        m_PetNumber = fields[0].GetUInt32()+1;
    }
#else /* CROSS */
    _hiPetNumber = 1;
#endif /* CROSS */

#ifndef CROSS
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded the max pet number: %d in %u ms", m_PetNumber-1, GetMSTimeDiffToNow(oldMSTime));
#else /* not CROSS */
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded the max pet number: %d in %u ms", _hiPetNumber-1, GetMSTimeDiffToNow(oldMSTime));
#endif
}

std::string ObjectMgr::GeneratePetName(uint32 entry)
{
    StringVector & list0 = _petHalfName0[entry];
    StringVector & list1 = _petHalfName1[entry];

    if (list0.empty() || list1.empty())
    {
        CreatureTemplate const* cinfo = GetCreatureTemplate(entry);
        const char* petname = GetPetName(cinfo->family, sWorld->GetDefaultDb2Locale());
        if (!petname)
            return cinfo->Name[0];

        return std::string(petname);
    }

    return *(list0.begin()+urand(0, list0.size()-1)) + *(list1.begin()+urand(0, list1.size()-1));
}

#ifndef CROSS
uint32 ObjectMgr::GeneratePetNumber(uint32 p_Range)
#else /* CROSS */
uint32 ObjectMgr::GeneratePetNumber()
#endif /* CROSS */
{
#ifndef CROSS
    if (m_PetNumber >= 0xFFFFFFFE)
    {
        sLog->outError(LOG_FILTER_GENERAL, "Mail ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }

    return m_PetNumber.fetch_add(p_Range);
#else /* CROSS */
    return ++_hiPetNumber;
#endif /* CROSS */
}

uint64 ObjectMgr::GenerateVoidStorageItemId()
{
    return ++_voidItemId;
}

uint64 ObjectMgr::GenerateEncounterAttemptID()
{
    return ++m_EncounterAttemptID;;
}

#ifndef CROSS
void ObjectMgr::LoadCorpses()
{
    //        0     1     2     3            4      5          6          7       8       9      10        11    12          13          14          15         16
    // SELECT posX, posY, posZ, orientation, mapId, displayId, itemCache, bytes1, bytes2, flags, dynFlags, time, corpseType, instanceId, phaseMask, corpseGuid, guid FROM corpse WHERE corpseType <> 0

    uint32 oldMSTime = getMSTime();

    PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_CORPSES));
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 corpses. DB table `corpse` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint32 guid = fields[16].GetUInt32();
        CorpseType type = CorpseType(fields[12].GetUInt8());
        if (type >= MAX_CORPSE_TYPE)
        {
            sLog->outError(LOG_FILTER_GENERAL, "Corpse (guid: %u) have wrong corpse type (%u), not loading.", guid, type);
            continue;
        }

        Corpse* corpse = new Corpse(type);
        if (!corpse->LoadCorpseFromDB(guid, fields))
        {
            corpse->CleanBeforeGC();
            sGarbageCollector->Add(corpse);
            continue;
        }

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u corpses in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

#endif /* not CROSS */
void ObjectMgr::LoadReputationRewardRate()
{
    uint32 oldMSTime = getMSTime();

    _repRewardRateStore.clear();                             // for reload case

    uint32 count = 0; //                                0          1            2             3
    QueryResult result = WorldDatabase.Query("SELECT faction, quest_rate, creature_rate, spell_rate FROM reputation_reward_rate");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SQL, ">> Loaded `reputation_reward_rate`, table is empty!");

        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 factionId        = fields[0].GetUInt32();

        RepRewardRate repRate;

        repRate.quest_rate      = fields[1].GetFloat();
        repRate.creature_rate   = fields[2].GetFloat();
        repRate.spell_rate      = fields[3].GetFloat();

        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);
        if (!factionEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `reputation_reward_rate`", factionId);
            continue;
        }

        if (repRate.quest_rate < 0.0f)
        {
            sLog->outError(LOG_FILTER_SQL, "Table reputation_reward_rate has quest_rate with invalid rate %f, skipping data for faction %u", repRate.quest_rate, factionId);
            continue;
        }

        if (repRate.creature_rate < 0.0f)
        {
            sLog->outError(LOG_FILTER_SQL, "Table reputation_reward_rate has creature_rate with invalid rate %f, skipping data for faction %u", repRate.creature_rate, factionId);
            continue;
        }

        if (repRate.spell_rate < 0.0f)
        {
            sLog->outError(LOG_FILTER_SQL, "Table reputation_reward_rate has spell_rate with invalid rate %f, skipping data for faction %u", repRate.spell_rate, factionId);
            continue;
        }

        _repRewardRateStore[factionId] = repRate;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u reputation_reward_rate in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCurrencyOnKill()
{
    uint32 oldMSTime = getMSTime();

    _curOnKillStore.clear();

    uint32 count = 0;

    QueryResult result = WorldDatabase.Query("SELECT `creature_id`, `CurrencyId1`,  `CurrencyId2`,  `CurrencyId3`, `CurrencyCount1`, `CurrencyCount2`, `CurrencyCount3` FROM `creature_loot_currency`");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature currency definitions. DB table `creature_currency` is empty.");
        return;
    }

    do
    {
        Field*l_Fields = result->Fetch();

        uint32 l_Creature_id = l_Fields[0].GetUInt32();

        CurrencyOnKillEntry l_CurrOnKill;
        l_CurrOnKill[l_Fields[1].GetUInt16()] = l_Fields[4].GetInt32();
        l_CurrOnKill[l_Fields[2].GetUInt16()] = l_Fields[5].GetInt32();
        l_CurrOnKill[l_Fields[3].GetUInt16()] = l_Fields[6].GetInt32();

        if (!GetCreatureTemplate(l_Creature_id))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature_creature` have data for not existed creature entry (%u), skipped", l_Creature_id);
            continue;
        }

        for (CurrencyOnKillEntry::const_iterator i = l_CurrOnKill.begin(); i != l_CurrOnKill.end(); ++i)
        {
            if (!i->first)
                continue;

            if (!sCurrencyTypesStore.LookupEntry(i->first))
            {
                sLog->outError(LOG_FILTER_SQL, "CurrencyType (CurrencyTypes.db2) %u does not exist but is used in `creature_currency`", i->first);
                continue;
            }
        }

        _curOnKillStore[l_Creature_id] = l_CurrOnKill;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature currency definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPersonnalCurrencyOnKill()
{
    uint32 l_OldMSTime = getMSTime();

    m_PersonnalCurrOnKillStore.clear();

    uint32 l_Count = 0;

    QueryResult l_Result = WorldDatabase.Query("SELECT `creature_id`, `CurrencyId1`,  `CurrencyId2`,  `CurrencyId3`, `CurrencyCount1`, `CurrencyCount2`, `CurrencyCount3` FROM `creature_loot_currency_personnal`");
    if (!l_Result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 personnal creature currency definitions. DB table `creature_loot_currency_personnal` is empty.");
        return;
    }

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 l_Entry = l_Fields[0].GetUInt32();

        CurrencyOnKillEntry l_CurrOnKill;
        l_CurrOnKill[l_Fields[1].GetUInt16()] = l_Fields[4].GetInt32();
        l_CurrOnKill[l_Fields[2].GetUInt16()] = l_Fields[5].GetInt32();
        l_CurrOnKill[l_Fields[3].GetUInt16()] = l_Fields[6].GetInt32();

        if (!GetCreatureTemplate(l_Entry))
            continue;

        for (CurrencyOnKillEntry::const_iterator i = l_CurrOnKill.begin(); i != l_CurrOnKill.end(); ++i)
        {
            if (!sCurrencyTypesStore.LookupEntry(i->first))
                continue;
        }

        m_PersonnalCurrOnKillStore[l_Entry] = l_CurrOnKill;
        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u personnal creature currency definitions in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadReputationOnKill()
{
    uint32 oldMSTime = getMSTime();

    // For reload case
    _repOnKillStore.clear();

    uint32 count = 0;

    //                                                0            1                     2
    QueryResult result = WorldDatabase.Query("SELECT creature_id, RewOnKillRepFaction1, RewOnKillRepFaction2, "
    //   3             4             5                   6             7             8                   9
        "IsTeamAward1, MaxStanding1, RewOnKillRepValue1, IsTeamAward2, MaxStanding2, RewOnKillRepValue2, TeamDependent "
        "FROM creature_onkill_reputation");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature award reputation definitions. DB table `creature_onkill_reputation` is empty.");

        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 creature_id = fields[0].GetUInt32();

        ReputationOnKillEntry repOnKill;
        repOnKill.RepFaction1          = fields[1].GetInt16();
        repOnKill.RepFaction2          = fields[2].GetInt16();
        repOnKill.IsTeamAward1        = fields[3].GetBool();
        repOnKill.ReputationMaxCap1  = fields[4].GetUInt8();
        repOnKill.RepValue1            = fields[5].GetInt32();
        repOnKill.IsTeamAward2        = fields[6].GetBool();
        repOnKill.ReputationMaxCap2  = fields[7].GetUInt8();
        repOnKill.RepValue2            = fields[8].GetInt32();
        repOnKill.TeamDependent       = fields[9].GetUInt8();

        if (!GetCreatureTemplate(creature_id))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `creature_onkill_reputation` have data for not existed creature entry (%u), skipped", creature_id);
            continue;
        }

        if (repOnKill.RepFaction1)
        {
            FactionEntry const* factionEntry1 = sFactionStore.LookupEntry(repOnKill.RepFaction1);
            if (!factionEntry1)
            {
                sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `creature_onkill_reputation`", repOnKill.RepFaction1);
                continue;
            }
        }

        if (repOnKill.RepFaction2)
        {
            FactionEntry const* factionEntry2 = sFactionStore.LookupEntry(repOnKill.RepFaction2);
            if (!factionEntry2)
            {
                sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `creature_onkill_reputation`", repOnKill.RepFaction2);
                continue;
            }
        }

        _repOnKillStore[creature_id] = repOnKill;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature award reputation definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadReputationSpilloverTemplate()
{
    uint32 oldMSTime = getMSTime();

    _repSpilloverTemplateStore.clear();                      // for reload case

    uint32 count = 0; //                                0         1        2       3        4       5       6         7        8      9        10       11     12        13       14     15
    QueryResult result = WorldDatabase.Query("SELECT faction, faction1, rate_1, rank_1, faction2, rate_2, rank_2, faction3, rate_3, rank_3, faction4, rate_4, rank_4, faction5, rate_5, rank_5 FROM reputation_spillover_template");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded `reputation_spillover_template`, table is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 factionId                = fields[0].GetUInt16();

        RepSpilloverTemplate repTemplate;

        repTemplate.faction[0]          = fields[1].GetUInt16();
        repTemplate.faction_rate[0]     = fields[2].GetFloat();
        repTemplate.faction_rank[0]     = fields[3].GetUInt8();
        repTemplate.faction[1]          = fields[4].GetUInt16();
        repTemplate.faction_rate[1]     = fields[5].GetFloat();
        repTemplate.faction_rank[1]     = fields[6].GetUInt8();
        repTemplate.faction[2]          = fields[7].GetUInt16();
        repTemplate.faction_rate[2]     = fields[8].GetFloat();
        repTemplate.faction_rank[2]     = fields[9].GetUInt8();
        repTemplate.faction[3]          = fields[10].GetUInt16();
        repTemplate.faction_rate[3]     = fields[11].GetFloat();
        repTemplate.faction_rank[3]     = fields[12].GetUInt8();
        repTemplate.faction[4]          = fields[13].GetUInt16();
        repTemplate.faction_rate[4]     = fields[14].GetFloat();
        repTemplate.faction_rank[4]     = fields[15].GetUInt8();

        FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

        if (!factionEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `reputation_spillover_template`", factionId);
            continue;
        }

        if (factionEntry->ParentFactionID == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u in `reputation_spillover_template` does not belong to any team, skipping", factionId);
            continue;
        }

        for (uint32 i = 0; i < MAX_SPILLOVER_FACTIONS; ++i)
        {
            if (repTemplate.faction[i])
            {
                FactionEntry const* factionSpillover = sFactionStore.LookupEntry(repTemplate.faction[i]);

                if (!factionSpillover)
                {
                    sLog->outError(LOG_FILTER_SQL, "Spillover faction (faction.db2) %u does not exist but is used in `reputation_spillover_template` for faction %u, skipping", repTemplate.faction[i], factionId);
                    continue;
                }

                if (factionSpillover->ReputationIndex < 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Spillover faction (faction.db2) %u for faction %u in `reputation_spillover_template` can not be listed for client, and then useless, skipping", repTemplate.faction[i], factionId);
                    continue;
                }

                if (repTemplate.faction_rank[i] >= MAX_REPUTATION_RANK)
                {
                    sLog->outError(LOG_FILTER_SQL, "Rank %u used in `reputation_spillover_template` for spillover faction %u is not valid, skipping", repTemplate.faction_rank[i], repTemplate.faction[i]);
                    continue;
                }
            }
        }

        FactionEntry const* factionEntry0 = sFactionStore.LookupEntry(repTemplate.faction[0]);
        if (repTemplate.faction[0] && !factionEntry0)
        {
            sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[0]);
            continue;
        }
        FactionEntry const* factionEntry1 = sFactionStore.LookupEntry(repTemplate.faction[1]);
        if (repTemplate.faction[1] && !factionEntry1)
        {
            sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[1]);
            continue;
        }
        FactionEntry const* factionEntry2 = sFactionStore.LookupEntry(repTemplate.faction[2]);
        if (repTemplate.faction[2] && !factionEntry2)
        {
            sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[2]);
            continue;
        }
        FactionEntry const* factionEntry3 = sFactionStore.LookupEntry(repTemplate.faction[3]);
        if (repTemplate.faction[3] && !factionEntry3)
        {
            sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[3]);
            continue;
        }
        FactionEntry const* factionEntry4 = sFactionStore.LookupEntry(repTemplate.faction[4]);
        if (repTemplate.faction[4] && !factionEntry4)
        {
            sLog->outError(LOG_FILTER_SQL, "Faction (faction.db2) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[4]);
            continue;
        }

        _repSpilloverTemplateStore[factionId] = repTemplate;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u reputation_spillover_template in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadPointsOfInterest()
{
    uint32 oldMSTime = getMSTime();

    _pointsOfInterestStore.clear();                              // need for reload case

    uint32 count = 0;

    //                                                  0   1  2   3      4     5       6
    QueryResult result = WorldDatabase.Query("SELECT entry, x, y, icon, flags, data, icon_name FROM points_of_interest");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Points of Interest definitions. DB table `points_of_interest` is empty.");

        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 point_id = fields[0].GetUInt32();

        PointOfInterest POI;
        POI.x = fields[1].GetFloat();
        POI.y = fields[2].GetFloat();
        POI.icon = fields[3].GetUInt32();
        POI.flags = fields[4].GetUInt32();
        POI.data = fields[5].GetUInt32();
        POI.icon_name = fields[6].GetString();

        if (!JadeCore::IsValidMapCoord(POI.x, POI.y))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `points_of_interest` (Entry: %u) have invalid coordinates (X: %f Y: %f), ignored.", point_id, POI.x, POI.y);
            continue;
        }

        _pointsOfInterestStore[point_id] = POI;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Points of Interest definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadQuestPOI()
{
    uint32 oldMSTime = getMSTime();

    _questPOIStore.clear();                              // need for reload case

    uint32 count = 0;

    //                                                   0        1        2            3           4                 5           6         7            8       9       10         11              12             13          14
    QueryResult result = WorldDatabase.Query("SELECT QuestID, BlobIndex, Idx1, ObjectiveIndex, QuestObjectiveID, QuestObjectID, MapID, WorldMapAreaId, Floor, Priority, Flags, WorldEffectID, PlayerConditionID, WoDUnk1, AlwaysAllowMergingBlobs  FROM quest_poi order by QuestID, Idx1");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 quest POI definitions. DB table `quest_poi` is empty.");
        return;
    }

    //                                                0        1    2  3
    QueryResult points = WorldDatabase.Query("SELECT QuestID, Idx1, X, Y FROM quest_poi_points ORDER BY QuestID DESC, Idx1, Idx2");

    std::vector<std::vector<std::vector<QuestPOIPoint>>> POIs;

    if (points)
    {
        // The first result should have the highest questId
        Field* fields = points->Fetch();
        uint32 questIdMax = fields[0].GetInt32();
        POIs.resize(questIdMax + 1);

        do
        {
            fields = points->Fetch();

            int32 QuestID = fields[0].GetInt32();
            int32 Idx1 = fields[1].GetInt32();
            int32 X = fields[2].GetInt32();
            int32 Y = fields[3].GetInt32();

            if (int32(POIs[QuestID].size()) <= Idx1 + 1)
                POIs[QuestID].resize(Idx1 + 10);

            QuestPOIPoint point(X, Y);
            POIs[QuestID][Idx1].push_back(point);
        } while (points->NextRow());
    }

    do
    {
        Field* fields = result->Fetch();

        int32 QuestID = fields[0].GetInt32();
        int32 BlobIndex = fields[1].GetInt32();
        int32 Idx1 = fields[2].GetInt32();
        int32 ObjectiveIndex = fields[3].GetInt32();
        int32 QuestObjectiveID = fields[4].GetInt32();
        int32 QuestObjectID = fields[5].GetInt32();
        int32 MapID = fields[6].GetInt32();
        int32 WorldMapAreaId = fields[7].GetInt32();
        int32 Floor = fields[8].GetInt32();
        int32 Priority = fields[9].GetInt32();
        int32 Flags = fields[10].GetInt32();
        int32 WorldEffectID = fields[11].GetInt32();
        int32 PlayerConditionID = fields[12].GetInt32();
        int32 WoDUnk1 = fields[13].GetInt32();
        bool AlwaysAllowMergingBlobs = fields[14].GetBool();

        QuestPOI POI(BlobIndex, ObjectiveIndex, QuestObjectiveID, QuestObjectID, MapID, WorldMapAreaId, Floor, Priority, Flags, WorldEffectID, PlayerConditionID, WoDUnk1, AlwaysAllowMergingBlobs);
        if (QuestID < int32(POIs.size()) && Idx1 < int32(POIs[QuestID].size()))
        {
            POI.points = POIs[QuestID][Idx1];
            _questPOIStore[QuestID].push_back(POI);
        }
        else
            sLog->outError(LOG_FILTER_SERVER_LOADING, "Table quest_poi references unknown quest points for quest %i POI id %i", QuestID, BlobIndex);

        ++count;
    } while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u quest POI definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadScenarioPOI()
{
    uint32 oldMSTime = getMSTime();

    _scenarioPOIStore.clear();                          // need for reload case

    uint32 count = 0;
    uint32 criteriaTreeIdMax = 0;

    //                                                      0           1      2          3            4         5       6          7                8
    QueryResult result = WorldDatabase.Query("SELECT criteriaTreeId, BlobID, MapID, WorldMapAreaID, `Floor`, Priority, Flags, WorldEffectID, PlayerConditionID FROM scenario_poi order by criteriaTreeId");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 scenario POI definitions. DB table `scenario_poi` is empty.");
        return;
    }

    //                                               0               1   2  3
    QueryResult points = WorldDatabase.Query("SELECT criteriaTreeId, id, x, y FROM scenario_poi_points ORDER BY criteriaTreeId DESC, idx");

    std::vector<std::vector<std::vector<ScenarioPOIPoint> > > POIs;

    if (points)
    {
        // The first result should have the highest questId
        Field* fields = points->Fetch();
        criteriaTreeIdMax = fields[0].GetUInt32();
        POIs.resize(criteriaTreeIdMax + 1);

        do
        {
            fields = points->Fetch();

            uint32 criteriaTreeId     = fields[0].GetUInt32();
            uint32 id                 = fields[1].GetUInt32();
            int32  x                  = fields[2].GetInt32();
            int32  y                  = fields[3].GetInt32();

            if (POIs[criteriaTreeId].size() <= id + 1)
                POIs[criteriaTreeId].resize(id + 10);

            ScenarioPOIPoint point(x, y);
            POIs[criteriaTreeId][id].push_back(point);
        } while (points->NextRow());
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 criteriaTreeId     = fields[0].GetUInt32();
        uint32 BlobID             = fields[1].GetUInt32();
        uint32 MapID              = fields[2].GetUInt32();
        uint32 WorldMapAreaID     = fields[3].GetUInt32();
        uint32 Floor              = fields[4].GetUInt32();
        uint32 Priority           = fields[5].GetUInt32();
        uint32 Flags              = fields[6].GetUInt32();
        uint32 WorldEffectID      = fields[7].GetUInt32();
        uint32 PlayerConditionID  = fields[8].GetUInt32();

        ScenarioPOI POI(BlobID, MapID, WorldMapAreaID, Floor, Priority, Flags, WorldEffectID, PlayerConditionID);

        if(criteriaTreeId <= criteriaTreeIdMax && POIs[criteriaTreeId].size() > 0)
            POI.points = POIs[criteriaTreeId][BlobID];

        _scenarioPOIStore[criteriaTreeId].push_back(POI);

        ++count;
    } while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u scenario POI definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadNPCSpellClickSpells()
{
    uint32 oldMSTime = getMSTime();

    _spellClickInfoStore.clear();
    //                                                0          1         2            3
    QueryResult result = WorldDatabase.Query("SELECT npc_entry, spell_id, cast_flags, user_type FROM npc_spellclick_spells");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spellclick spells. DB table `npc_spellclick_spells` is empty.");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 npc_entry = fields[0].GetUInt32();
        CreatureTemplate const* cInfo = GetCreatureTemplate(npc_entry);
        if (!cInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Table npc_spellclick_spells references unknown creature_template %u. Skipping entry.", npc_entry);
            continue;
        }

        uint32 spellid = fields[1].GetUInt32();
        SpellInfo const* spellinfo = sSpellMgr->GetSpellInfo(spellid);
        if (!spellinfo)
        {
            sLog->outError(LOG_FILTER_SQL, "Table npc_spellclick_spells references unknown spellid %u. Skipping entry.", spellid);
            continue;
        }

        uint8 userType = fields[3].GetUInt16();
        if (userType >= SPELL_CLICK_USER_MAX)
            sLog->outError(LOG_FILTER_SQL, "Table npc_spellclick_spells references unknown user type %u. Skipping entry.", uint32(userType));

        uint8 castFlags = fields[2].GetUInt8();
        SpellClickInfo info;
        info.spellId = spellid;
        info.castFlags = castFlags;
        info.userType = SpellClickUserTypes(userType);
        _spellClickInfoStore.insert(SpellClickInfoContainer::value_type(npc_entry, info));

        ++count;
    }
    while (result->NextRow());

    // all spellclick data loaded, now we check if there are creatures with NPC_FLAG_SPELLCLICK but with no data
    // NOTE: It *CAN* be the other way around: no spellclick flag but with spellclick data, in case of creature-only vehicle accessories
    CreatureTemplateContainer const* l_CreatureTemplates = sObjectMgr->GetCreatureTemplates();
    for (auto l_Iter = l_CreatureTemplates->begin(); l_Iter != l_CreatureTemplates->end(); ++l_Iter)
    {
        if ((l_Iter->second.NpcFlags1 & UNIT_NPC_FLAG_SPELLCLICK) && _spellClickInfoStore.find(l_Iter->second.Entry) == _spellClickInfoStore.end())
        {
            sLog->outError(LOG_FILTER_SQL, "npc_spellclick_spells: Creature template %u has UNIT_NPC_FLAG_SPELLCLICK but no data in spellclick table! Removing flag", l_Iter->second.Entry);
            m_CreatureTemplateStore[l_Iter->second.Entry].NpcFlags1 &= ~UNIT_NPC_FLAG_SPELLCLICK;
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spellclick definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::DeleteCreatureData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    CreatureData const* data = GetCreatureData(guid);
    if (data)
        RemoveCreatureFromGrid(guid, data);

    _creatureDataStore.erase(guid);
}

void ObjectMgr::DeleteGOData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    GameObjectData const* data = GetGOData(guid);
    if (data)
        RemoveGameobjectFromGrid(guid, data);

    _gameObjectDataStore.erase(guid);
}

const std::vector<ConversationData>* ObjectMgr::GetConversationData(uint32 entry) const
{
    ConversationDataMap::const_iterator itr = m_ConversationData.find(entry);
    return itr != m_ConversationData.end() ? &(itr->second) : NULL;
}

const std::vector<ConversationCreature>* ObjectMgr::GetConversationCreature(uint32 entry) const
{
    ConversationCreatureMap::const_iterator itr = m_ConversationCreature.find(entry);
    return itr != m_ConversationCreature.end() ? &(itr->second) : NULL;
}

const std::vector<ConversationActor>* ObjectMgr::GetConversationActor(uint32 entry) const
{
    ConversationActorMap::const_iterator itr = m_ConversationActor.find(entry);
    return itr != m_ConversationActor.end() ? &(itr->second) : NULL;
}


void ObjectMgr::AddCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid, uint32 instance)
{
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(mapid, 0)][cellid];
    cell_guids.corpses[player_guid] = instance;
}

void ObjectMgr::DeleteCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid)
{
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    CellObjectGuids& cell_guids = _mapObjectGuidsStore[MAKE_PAIR32(mapid, 0)][cellid];
    cell_guids.corpses.erase(player_guid);
}

void ObjectMgr::LoadQuestRelationsHelper(QuestRelations& map, std::string table, bool starter, bool go)
{
    uint32 oldMSTime = getMSTime();

    map.clear();                                            // need for reload case

    uint32 count = 0;

    QueryResult result = WorldDatabase.PQuery("SELECT id, quest, pool_entry FROM %s qr LEFT JOIN pool_quest pq ON qr.quest = pq.entry", table.c_str());

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 quest relations from `%s`, table is empty.", table.c_str());

        return;
    }

    PooledQuestRelation* poolRelationMap = go ? &sPoolMgr->mQuestGORelation : &sPoolMgr->mQuestCreatureRelation;
    if (starter)
        poolRelationMap->clear();

    do
    {
        uint32 id     = result->Fetch()[0].GetUInt32();
        uint32 quest  = result->Fetch()[1].GetUInt32();
        uint32 poolId = result->Fetch()[2].GetUInt32();

        if (_questTemplates.find(quest) == _questTemplates.end())
        {
            sLog->outError(LOG_FILTER_SQL, "Table `%s`: Quest %u listed for entry %u does not exist.", table.c_str(), quest, id);
            continue;
        }

        if (!poolId || !starter)
            map.insert(QuestRelations::value_type(id, quest));
        else if (starter)
            poolRelationMap->insert(PooledQuestRelation::value_type(quest, id));

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u quest relations from %s in %u ms", count, table.c_str(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGameobjectQuestStarters()
{
    LoadQuestRelationsHelper(_goQuestRelations, "gameobject_queststarter", true, true);

    for (QuestRelations::iterator itr = _goQuestRelations.begin(); itr != _goQuestRelations.end(); ++itr)
    {
        GameObjectTemplate const* goInfo = GetGameObjectTemplate(itr->first);
        if (!goInfo)
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject_queststarter` have data for not existed gameobject entry (%u) and existed quest %u", itr->first, itr->second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject_queststarter` have data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadGameobjectQuestEnders()
{
    LoadQuestRelationsHelper(_goQuestInvolvedRelations, "gameobject_questender", false, true);

    for (QuestRelations::iterator itr = _goQuestInvolvedRelations.begin(); itr != _goQuestInvolvedRelations.end(); ++itr)
    {
        GameObjectTemplate const* goInfo = GetGameObjectTemplate(itr->first);
        if (!goInfo)
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject_questender` have data for not existed gameobject entry (%u) and existed quest %u", itr->first, itr->second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            sLog->outError(LOG_FILTER_SQL, "Table `gameobject_questender` have data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadCreatureQuestStarters()
{
    LoadQuestRelationsHelper(_creatureQuestRelations, "creature_queststarter", true, false);

    for (QuestRelations::const_iterator l_Iterator = _creatureQuestRelations.begin(); l_Iterator != _creatureQuestRelations.end(); ++l_Iterator)
    {
        CreatureTemplate const* cInfo = GetCreatureTemplate(l_Iterator->first);
        if (!cInfo)
            sLog->outError(LOG_FILTER_SQL, "Table `creature_queststarter` have data for not existed creature entry (%u) and existed quest %u", l_Iterator->first, l_Iterator->second);
        else if (!(cInfo->NpcFlags1 & UNIT_NPC_FLAG_QUESTGIVER))
            sLog->outError(LOG_FILTER_SQL, "Table `creature_queststarter` has creature entry (%u) for quest %u, but npcflag does not include UNIT_NPC_FLAG_QUESTGIVER", l_Iterator->first, l_Iterator->second);

        Quest* l_Quest = const_cast<Quest*>(GetQuestTemplate(l_Iterator->second));
        if (l_Quest != nullptr)
            l_Quest->completionsNpcs.push_back(l_Iterator->first);
    }
}

void ObjectMgr::LoadCreatureQuestEnders()
{
    LoadQuestRelationsHelper(_creatureQuestInvolvedRelations, "creature_questender", false, false);

    for (QuestRelations::iterator itr = _creatureQuestInvolvedRelations.begin(); itr != _creatureQuestInvolvedRelations.end(); ++itr)
    {
        CreatureTemplate const* cInfo = GetCreatureTemplate(itr->first);
        if (!cInfo)
            sLog->outError(LOG_FILTER_SQL, "Table `creature_questender` have data for not existed creature entry (%u) and existed quest %u", itr->first, itr->second);
        else if (!(cInfo->NpcFlags1 & UNIT_NPC_FLAG_QUESTGIVER))
            sLog->outError(LOG_FILTER_SQL, "Table `creature_questender` has creature entry (%u) for quest %u, but npcflag does not include UNIT_NPC_FLAG_QUESTGIVER", itr->first, itr->second);
    }
}

#ifndef CROSS
void ObjectMgr::LoadReservedPlayersNames()
{
    uint32 oldMSTime = getMSTime();

    _reservedNamesStore.clear();                                // need for reload case

    QueryResult result = CharacterDatabase.Query("SELECT name FROM reserved_name");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 reserved player names. DB table `reserved_name` is empty!");
        return;
    }

    uint32 count = 0;

    Field* fields;
    do
    {
        fields = result->Fetch();
        std::string name= fields[0].GetString();

        std::wstring wstr;
        if (!Utf8toWStr (name, wstr))
        {
            sLog->outError(LOG_FILTER_GENERAL, "Table `reserved_name` have invalid name: %s", name.c_str());
            continue;
        }

        wstrToLower(wstr);

        _reservedNamesStore.insert(wstr);
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u reserved player names in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

#endif /* not CROSS */
bool ObjectMgr::IsReservedName(const std::string& name) const
{
    std::wstring wstr;
    if (!Utf8toWStr (name, wstr))
        return false;

    wstrToLower(wstr);

    return _reservedNamesStore.find(wstr) != _reservedNamesStore.end();
}

enum LanguageType
{
    LT_BASIC_LATIN    = 0x0000,
    LT_EXTENDEN_LATIN = 0x0001,
    LT_CYRILLIC       = 0x0002,
    LT_EAST_ASIA      = 0x0004,
    LT_ANY            = 0xFFFF
};

static LanguageType GetRealmLanguageType(bool create)
{
    switch (sWorld->getIntConfig(CONFIG_REALM_ZONE))
    {
        case REALM_ZONE_UNKNOWN:                            // any language
        case REALM_ZONE_DEVELOPMENT:
        case REALM_ZONE_TEST_SERVER:
        case REALM_ZONE_QA_SERVER:
            return LT_ANY;
        case REALM_ZONE_UNITED_STATES:                      // extended-Latin
        case REALM_ZONE_OCEANIC:
        case REALM_ZONE_LATIN_AMERICA:
        case REALM_ZONE_ENGLISH:
        case REALM_ZONE_GERMAN:
        case REALM_ZONE_FRENCH:
        case REALM_ZONE_SPANISH:
            return LT_EXTENDEN_LATIN;
        case REALM_ZONE_KOREA:                              // East-Asian
        case REALM_ZONE_TAIWAN:
        case REALM_ZONE_CHINA:
            return LT_EAST_ASIA;
        case REALM_ZONE_RUSSIAN:                            // Cyrillic
            return LT_CYRILLIC;
        default:
            return create ? LT_BASIC_LATIN : LT_ANY;        // basic-Latin at create, any at login
    }
}

bool isValidString(std::wstring& wstr, uint32 strictMask, bool numericOrSpace, bool create = false)
{
    if (strictMask == 0)                                       // any language, ignore realm
    {
        if (isExtendedLatinString(wstr, numericOrSpace))
            return true;
        if (isCyrillicString(wstr, numericOrSpace))
            return true;
        if (isEastAsianString(wstr, numericOrSpace))
            return true;
        return false;
    }

    if (strictMask & 0x2)                                    // realm zone specific
    {
        LanguageType lt = GetRealmLanguageType(create);
        if (lt & LT_EXTENDEN_LATIN)
            if (isExtendedLatinString(wstr, numericOrSpace))
                return true;
        if (lt & LT_CYRILLIC)
            if (isCyrillicString(wstr, numericOrSpace))
                return true;
        if (lt & LT_EAST_ASIA)
            if (isEastAsianString(wstr, numericOrSpace))
                return true;
    }

    if (strictMask & 0x1)                                    // basic Latin
    {
        if (isBasicLatinString(wstr, numericOrSpace))
            return true;
    }

    return false;
}

uint8 ObjectMgr::CheckPlayerName(const std::string& name, bool create)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return CHAR_NAME_INVALID_CHARACTER;

    if (wname.size() > MAX_PLAYER_NAME)
        return CHAR_NAME_TOO_LONG;

    uint32 minName = sWorld->getIntConfig(CONFIG_MIN_PLAYER_NAME);
    if (wname.size() < minName)
        return CHAR_NAME_TOO_SHORT;

    uint32 strictMask = sWorld->getIntConfig(CONFIG_STRICT_PLAYER_NAMES);
    if (!isValidString(wname, strictMask, false, create))
        return CHAR_NAME_MIXED_LANGUAGES;

    wstrToLower(wname);
    for (size_t i = 2; i < wname.size(); ++i)
        if (wname[i] == wname[i-1] && wname[i] == wname[i-2])
            return CHAR_NAME_THREE_CONSECUTIVE;

    if (sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE) && !sWordFilterMgr->FindBadWord(name).empty())
        return CHAR_NAME_PROFANE;

    return CHAR_NAME_SUCCESS;
}

bool ObjectMgr::IsValidCharterName(const std::string& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    if (wname.size() > MAX_CHARTER_NAME)
        return false;

    uint32 minName = sWorld->getIntConfig(CONFIG_MIN_CHARTER_NAME);
    if (wname.size() < minName)
        return false;

    uint32 strictMask = sWorld->getIntConfig(CONFIG_STRICT_CHARTER_NAMES);

    return isValidString(wname, strictMask, true);
}

PetNameInvalidReason ObjectMgr::CheckPetName(const std::string& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return PET_NAME_INVALID;

    if (wname.size() > MAX_PET_NAME)
        return PET_NAME_TOO_LONG;

    uint32 minName = sWorld->getIntConfig(CONFIG_MIN_PET_NAME);
    if (wname.size() < minName)
        return PET_NAME_TOO_SHORT;

    uint32 strictMask = sWorld->getIntConfig(CONFIG_STRICT_PET_NAMES);
    if (!isValidString(wname, strictMask, false))
        return PET_NAME_MIXED_LANGUAGES;

    if (sWorld->getBoolConfig(CONFIG_WORD_FILTER_ENABLE) && !sWordFilterMgr->FindBadWord(name).empty())
        return PET_NAME_PROFANE;

    return PET_NAME_SUCCESS;
}

void ObjectMgr::LoadGameObjectForQuests()
{
    uint32 oldMSTime = getMSTime();

    _gameObjectForQuestStore.clear();                         // need for reload case

    if (sObjectMgr->GetGameObjectTemplates()->empty())
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 GameObjects for quests");
        return;
    }

    uint32 count = 0;

    // collect GO entries for GO that must activated
    GameObjectTemplateContainer const* gotc = sObjectMgr->GetGameObjectTemplates();
    for (GameObjectTemplateContainer::const_iterator itr = gotc->begin(); itr != gotc->end(); ++itr)
    {
        switch (itr->second.type)
        {
            // scan GO chest with loot including quest items
            case GAMEOBJECT_TYPE_CHEST:
            {
                uint32 loot_id = (itr->second.GetLootId());

                // find quest loot for GO
                if (itr->second.chest.questID || LootTemplates_Gameobject.HaveQuestLootFor(loot_id))
                {
                    _gameObjectForQuestStore.insert(itr->second.entry);
                    ++count;
                }
                break;
            }
            case GAMEOBJECT_TYPE_GENERIC:
            {
                if (itr->second.generic.questID > 0)            //quests objects
                {
                    _gameObjectForQuestStore.insert(itr->second.entry);
                    count++;
                }
                break;
            }
            case GAMEOBJECT_TYPE_GOOBER:
            {
                if (itr->second.goober.questID > 0)              //quests objects
                {
                    _gameObjectForQuestStore.insert(itr->second.entry);
                    count++;
                }
                break;
            }
            default:
                break;
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u GameObjects for quests in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGameObjectAreatriggerTeleport()
{
    uint32 l_OldMSTime = getMSTime();

    m_GameobjectAreaTriggerStore.clear();                                  // need for reload case

    //                                                0            1          2        3                4                   5             6                7                  8
    QueryResult l_Result = WorldDatabase.Query("SELECT entry, target_map, zone_id, source_map, target_position_x, target_position_y, target_position_z, target_orientation, radius FROM gameobject_areatrigger_teleport");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 gameobject area trigger teleport definitions. DB table `gameobject_areatrigger_teleport` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();

        ++l_Count;

        uint8 l_Index = 0;

        uint32 l_GobEntry = l_Fields[l_Index++].GetUInt32();

        AreaTriggerStruct l_AreaTrigger;

        l_AreaTrigger.target_mapId          = l_Fields[l_Index++].GetUInt16();
        l_AreaTrigger.source_zoneId         = l_Fields[l_Index++].GetUInt32();
        l_AreaTrigger.source_mapId          = l_Fields[l_Index++].GetUInt16();
        l_AreaTrigger.target_X              = l_Fields[l_Index++].GetFloat();
        l_AreaTrigger.target_Y              = l_Fields[l_Index++].GetFloat();
        l_AreaTrigger.target_Z              = l_Fields[l_Index++].GetFloat();
        l_AreaTrigger.target_Orientation    = l_Fields[l_Index++].GetFloat();
        l_AreaTrigger.radius                = l_Fields[l_Index++].GetFloat();

        GameObjectTemplate const* l_GameObjectTemplate = sObjectMgr->GetGameObjectTemplate(l_GobEntry);
        if (!l_GameObjectTemplate)
        {
            sLog->outError(LOG_FILTER_SQL, "GameObject AreaTrigger teleport (ID:%u) target map (ID: %u) does not exist in `gameobject_template`.", l_GobEntry, l_AreaTrigger.target_mapId);
            continue;
        }

        MapEntry const* l_MapEntry = sMapStore.LookupEntry(l_AreaTrigger.target_mapId);
        if (!l_MapEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Game Object (ID:%u) target map (ID: %u) does not exist in `Map.db2`.", l_GobEntry, l_AreaTrigger.target_mapId);
            continue;
        }

        m_GameobjectAreaTriggerStore[std::make_pair(l_GobEntry, std::make_pair(l_AreaTrigger.source_mapId, l_AreaTrigger.source_zoneId))] = l_AreaTrigger;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u area trigger teleport definitions in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

bool ObjectMgr::LoadTrinityStrings(const char* table, int32 min_value, int32 max_value)
{
    uint32 oldMSTime = getMSTime();

    int32 start_value = min_value;
    int32 end_value   = max_value;
    // some string can have negative indexes range
    if (start_value < 0)
    {
        if (end_value >= start_value)
        {
            sLog->outError(LOG_FILTER_SQL, "Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }

        // real range (max+1, min+1) exaple: (-10, -1000) -> -999...-10+1
        std::swap(start_value, end_value);
        ++start_value;
        ++end_value;
    }
    else
    {
        if (start_value >= end_value)
        {
            sLog->outError(LOG_FILTER_SQL, "Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }
    }

    // cleanup affected map part for reloading case
    for (TrinityStringLocaleContainer::iterator itr = _trinityStringLocaleStore.begin(); itr != _trinityStringLocaleStore.end();)
    {
        if (itr->first >= start_value && itr->first < end_value)
            _trinityStringLocaleStore.erase(itr++);
        else
            ++itr;
    }

    QueryResult result = WorldDatabase.PQuery("SELECT entry, content_default, content_loc1, content_loc2, content_loc3, content_loc4, content_loc5, content_loc6, content_loc7, content_loc8, content_loc9, content_loc10 FROM %s", table);

    if (!result)
    {
        if (min_value == MIN_TRINITY_STRING_ID)              // error only in case internal strings
            sLog->outError(LOG_FILTER_SERVER_LOADING, ">>  Loaded 0 trinity strings. DB table `%s` is empty. Cannot continue.", table);
        else
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 string templates. DB table `%s` is empty.", table);

        return false;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        int32 entry = fields[0].GetInt32();

        if (entry == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `%s` contain reserved entry 0, ignored.", table);
            continue;
        }
        else if (entry < start_value || entry >= end_value)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `%s` contain entry %i out of allowed range (%d - %d), ignored.", table, entry, min_value, max_value);
            continue;
        }

        TrinityStringLocale& data = _trinityStringLocaleStore[entry];

        if (!data.Content.empty())
        {
            sLog->outError(LOG_FILTER_SQL, "Table `%s` contain data for already loaded entry  %i (from another table?), ignored.", table, entry);
            continue;
        }

        data.Content.resize(1);
        ++count;

        for (uint8 i = 0; i < TOTAL_LOCALES; ++i)
            AddLocaleString(fields[i + 1].GetString(), LocaleConstant(i), data.Content);
    }
    while (result->NextRow());

    if (min_value == MIN_TRINITY_STRING_ID)
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Trinity strings from table %s in %u ms", count, table, GetMSTimeDiffToNow(oldMSTime));
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u string templates from %s in %u ms", count, table, GetMSTimeDiffToNow(oldMSTime));

    return true;
}

const char* ObjectMgr::GetTrinityString(int32 entry, LocaleConstant locale_idx) const
{
    if (TrinityStringLocale const* msl = GetTrinityStringLocale(entry))
    {
        if (msl->Content.size() > size_t(locale_idx) && !msl->Content[locale_idx].empty())
            return msl->Content[locale_idx].c_str();

        return msl->Content[DEFAULT_LOCALE].c_str();
    }

    if (entry > 0)
        sLog->outError(LOG_FILTER_SQL, "Entry %i not found in `trinity_string` table.", entry);
    else
        sLog->outError(LOG_FILTER_SQL, "Trinity string entry %i not found in DB.", entry);
    return "<error>";
}

std::string ObjectMgr::GetFormatedTrinityString(uint32 p_Entry, int32 p_Locale, ...)
{
    LocaleConstant l_Locale = static_cast<LocaleConstant>(p_Locale);
    char const* l_TrinityString = GetTrinityString(p_Entry, l_Locale);
    va_list l_VaList;

    char l_Buffer[2048];
    va_start(l_VaList, p_Locale);
    vsnprintf(l_Buffer, 2048, l_TrinityString, l_VaList);
    va_end(l_VaList);

    return std::string(l_Buffer);
}

std::vector<DisplayChoiceData> const* ObjectMgr::GetDisplayChoiceData(uint32 ChoiceID) const
{
    DisplayChoiceMap::const_iterator itr = _displayChoiceMap.find(ChoiceID);
    return itr != _displayChoiceMap.end() ? &(itr->second) : NULL;
}

void ObjectMgr::LoadFishingBaseSkillLevel()
{
    uint32 oldMSTime = getMSTime();

    _fishingBaseForAreaStore.clear();                            // for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, skill FROM skill_fishing_base_level");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 areas for fishing base skill level. DB table `skill_fishing_base_level` is empty.");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 entry  = fields[0].GetUInt32();
        int32 skill   = fields[1].GetInt16();

        AreaTableEntry const* fArea = sAreaTableStore.LookupEntry(entry);
        if (!fArea)
        {
            sLog->outError(LOG_FILTER_SQL, "AreaId %u defined in `skill_fishing_base_level` does not exist", entry);
            continue;
        }

        _fishingBaseForAreaStore[entry] = skill;
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u areas for fishing base skill level in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadSkillTiers()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT ID, Value1, Value2, Value3, Value4, Value5, Value6, Value7, Value8, Value9, Value10, "
        " Value11, Value12, Value13, Value14, Value15, Value16 FROM skill_tiers");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 skill max values. DB table `skill_tiers` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 id = fields[0].GetUInt32();
        SkillTiersEntry& tier = _skillTiers[id];
        for (uint32 i = 0; i < MAX_SKILL_STEP; ++i)
            tier.Value[i] = fields[1 + i].GetUInt32();

    } while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u skill max values in %u ms", uint32(_skillTiers.size()), GetMSTimeDiffToNow(oldMSTime));
}

bool ObjectMgr::CheckDeclinedNames(std::wstring w_ownname, DeclinedName const& names)
{
    // get main part of the name
    std::wstring mainpart = GetMainPartOfName(w_ownname, 0);
    // prepare flags
    bool x = true;
    bool y = true;

    // check declined names
    for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
    {
        std::wstring wname;
        if (!Utf8toWStr(names.name[i], wname))
            return false;

        if (mainpart != GetMainPartOfName(wname, i+1))
            x = false;

        if (w_ownname != wname)
            y = false;
    }
    return (x || y);
}

uint32 ObjectMgr::GetAreaTriggerScriptId(uint32 trigger_id)
{
    AreaTriggerScriptContainer::const_iterator i = _areaTriggerScriptStore.find(trigger_id);
    if (i!= _areaTriggerScriptStore.end())
        return i->second;
    return 0;
}

SpellScriptsBounds ObjectMgr::GetSpellScriptsBounds(uint32 spell_id)
{
    return SpellScriptsBounds(_spellScriptsStore.lower_bound(spell_id), _spellScriptsStore.upper_bound(spell_id));
}

SkillRangeType GetSkillRangeType(SkillRaceClassInfoEntry const* rcEntry)
{
    SkillLineEntry const* skill = sSkillLineStore.LookupEntry(rcEntry->SkillID);
    if (!skill)
        return SKILL_RANGE_NONE;

    if (sObjectMgr->GetSkillTier(rcEntry->SkillTierID))
        return SKILL_RANGE_RANK;

    if (rcEntry->SkillID == SKILL_RUNEFORGING)
        return SKILL_RANGE_MONO;

    switch (skill->CategoryID)
    {
        case SKILL_CATEGORY_ARMOR:
            return SKILL_RANGE_MONO;
        case SKILL_CATEGORY_LANGUAGES:
            return SKILL_RANGE_LANGUAGE;
    }

    return SKILL_RANGE_LEVEL;
}

void ObjectMgr::LoadGameTele()
{
    uint32 oldMSTime = getMSTime();

    _gameTeleStore.clear();                                  // for reload case

    //                                                0       1           2           3           4        5     6
    QueryResult result = WorldDatabase.Query("SELECT id, position_x, position_y, position_z, orientation, map, name FROM game_tele");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">>  Loaded 0 GameTeleports. DB table `game_tele` is empty!");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 id         = fields[0].GetUInt32();

        GameTele gt;

        gt.position_x     = fields[1].GetFloat();
        gt.position_y     = fields[2].GetFloat();
        gt.position_z     = fields[3].GetFloat();
        gt.orientation    = fields[4].GetFloat();
        gt.mapId          = fields[5].GetUInt16();
        gt.name           = fields[6].GetString();

        if (!MapManager::IsValidMapCoord(gt.mapId, gt.position_x, gt.position_y, gt.position_z, gt.orientation))
        {
            sLog->outError(LOG_FILTER_SQL, "Wrong position for id %u (name: %s) in `game_tele` table, ignoring.", id, gt.name.c_str());
            continue;
        }

        if (!Utf8toWStr(gt.name, gt.wnameLow))
        {
            sLog->outError(LOG_FILTER_SQL, "Wrong UTF8 name for id %u in `game_tele` table, ignoring.", id);
            continue;
        }

        wstrToLower(gt.wnameLow);

        _gameTeleStore[id] = gt;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u GameTeleports in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

GameTele const* ObjectMgr::GetGameTele(const std::string& name) const
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return NULL;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    // Alternative first GameTele what contains wnameLow as substring in case no GameTele location found
    const GameTele* alt = NULL;
    for (GameTeleContainer::const_iterator itr = _gameTeleStore.begin(); itr != _gameTeleStore.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
            return &itr->second;
        else if (alt == NULL && itr->second.wnameLow.find(wname) != std::wstring::npos)
            alt = &itr->second;
    }

    return alt;
}

bool ObjectMgr::AddGameTele(GameTele& tele)
{
    // find max id
    uint32 new_id = 0;
    for (GameTeleContainer::const_iterator itr = _gameTeleStore.begin(); itr != _gameTeleStore.end(); ++itr)
        if (itr->first > new_id)
            new_id = itr->first;

    // use next
    ++new_id;

    if (!Utf8toWStr(tele.name, tele.wnameLow))
        return false;

    wstrToLower(tele.wnameLow);

    _gameTeleStore[new_id] = tele;

    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_GAME_TELE);

    stmt->setUInt32(0, new_id);
    stmt->setFloat(1, tele.position_x);
    stmt->setFloat(2, tele.position_y);
    stmt->setFloat(3, tele.position_z);
    stmt->setFloat(4, tele.orientation);
    stmt->setUInt16(5, uint16(tele.mapId));
    stmt->setString(6, tele.name);

    WorldDatabase.Execute(stmt);

    return true;
}

bool ObjectMgr::DeleteGameTele(const std::string& name)
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    for (GameTeleContainer::iterator itr = _gameTeleStore.begin(); itr != _gameTeleStore.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
        {
            PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAME_TELE);

            stmt->setString(0, itr->second.name);

            WorldDatabase.Execute(stmt);

            _gameTeleStore.erase(itr);
            return true;
        }
    }

    return false;
}

void ObjectMgr::LoadMailLevelRewards()
{
    uint32 oldMSTime = getMSTime();

    _mailLevelReward.clear();                           // for reload case

    //                                                 0        1             2            3       4    5
    QueryResult result = WorldDatabase.Query("SELECT entry, level, raceMask, classMask, subject, body, gold, items FROM mail_level_reward");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">>  Loaded 0 level dependent mail rewards. DB table `mail_level_reward` is empty.");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry            = fields[0].GetUInt32();
        uint8 level             = fields[1].GetUInt8();
        uint64 raceMask         = fields[2].GetUInt64();
        uint32 classMask        = fields[3].GetUInt32();
        std::string subject     = fields[4].GetString();
        std::string body        = fields[5].GetString();
        uint32 gold             = fields[6].GetUInt32();
        std::string itemsString = fields[7].GetString();

        if (level > MAX_LEVEL)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `mail_level_reward` have data for level %u that more supported by client (%u), ignoring.", level, MAX_LEVEL);
            continue;
        }

        if (raceMask && !(raceMask & RACEMASK_ALL_PLAYABLE))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `mail_level_reward` have raceMask (%lu) for level %u that not include any player races, ignoring.", raceMask, level);
            continue;
        }

        if (classMask && !(classMask & CLASSMASK_ALL_PLAYABLE))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `mail_level_reward` have classMask (%u) for level %u that not include any player classes, ignoring.", classMask, level);
            continue;
        }

        if (subject.empty())
        {
            sLog->outError(LOG_FILTER_SQL, "Table `mail_level_reward` have empty subject for level %u, ignoring.", level);
            continue;
        }

        if (body.empty())
        {
            sLog->outError(LOG_FILTER_SQL, "Table `mail_level_reward` have empty body for level %u, ignoring.", level);
            continue;
        }

        if (!GetCreatureTemplate(MailLevelReward::Sender))
        {
            sLog->outError(LOG_FILTER_SQL, "Table `mail_level_reward` have not existed sender creature entry (%u) for level %u, ignoring.", MailLevelReward::Sender, level);
            continue;
        }

        std::vector<uint32> items;
        if (!itemsString.empty())
        {
            Tokenizer tokens(itemsString, ',');
            for (uint8 i = 0; i < tokens.size(); )
                items.push_back(atoi(tokens[i++]));
        }
        _mailLevelReward[level].emplace_back(entry, raceMask, classMask, subject, body, gold, items);

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u level dependent mail rewards in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadMailRewardsLocale()
{
    uint32 oldMSTime = getMSTime();

    _mailRewardLocale.clear();                       // Need for reload case

    QueryResult result = WorldDatabase.Query("SELECT entry, subject_loc1, body_loc1, subject_loc2, body_loc2, subject_loc3, body_loc3, subject_loc4, body_loc4, "
        "subject_loc5, body_loc5, subject_loc6, body_loc6, subject_loc7, body_loc7, subject_loc8, body_loc8, subject_loc9, body_loc9,"
        "subject_loc10, body_loc10 FROM locales_mail_level_reward");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 mail reward locale strings.  DB table `locales_mail_level_reward` is empty");
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        MailRewardLocale& data = _mailRewardLocale[entry];

        for (int i = 1; i < TOTAL_LOCALES; ++i)
        {
            LocaleConstant locale = (LocaleConstant)i;
            ObjectMgr::AddLocaleString(fields[1 + 2 * (i - 1)].GetString(), locale, data.subject);
            ObjectMgr::AddLocaleString(fields[1 + 2 * (i - 1) + 1].GetString(), locale, data.body);
        }
    } while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu mail reward locale strings in %u ms", (unsigned long)_mailRewardLocale.size(), GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureBonusLoots()
{
    uint32 l_OldMSTime = getMSTime();
    uint32 l_Count = 0;

    m_CreatureBonusLootData.clear();

    QueryResult l_Result = WorldDatabase.Query("SELECT Entry, SpellID FROM creature_loot_bonus");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature loot bonus.  DB table `creature_loot_bonus` is empty");
        return;
    }

    do
    {
        uint8 l_Index       = 0;
        Field* l_Fields     = l_Result->Fetch();

        uint32 l_Entry      = l_Fields[l_Index++].GetUInt32();

        m_CreatureBonusLootData[l_Entry] = l_Fields[l_Index++].GetUInt32();

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature bonus loots in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::AddSpellToTrainer(uint32 entry, uint32 spell, uint32 spellCost, uint32 reqSkill, uint32 reqSkillValue, uint32 reqLevel)
{
    if (entry >= TRINITY_TRAINER_START_REF)
        return;

    CreatureTemplate const* cInfo = GetCreatureTemplate(entry);
    if (!cInfo)
    {
        sLog->outError(LOG_FILTER_SQL, "Table `npc_trainer` contains an entry for a non-existing creature template (Entry: %u), ignoring", entry);
        return;
    }

    if (!(cInfo->NpcFlags1 & UNIT_NPC_FLAG_TRAINER))
    {
        sLog->outError(LOG_FILTER_SQL, "Table `npc_trainer` contains an entry for a creature template (Entry: %u) without trainer flag, ignoring", entry);
        return;
    }

    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(spell);
    if (!l_SpellInfo)
    {
        sLog->outError(LOG_FILTER_SQL, "Table `npc_trainer` contains an entry (Entry: %u) for a non-existing spell (Spell: %u), ignoring", entry, spell);
        return;
    }

    if (!SpellMgr::IsSpellValid(l_SpellInfo))
    {
        sLog->outError(LOG_FILTER_SQL, "Table `npc_trainer` contains an entry (Entry: %u) for a broken spell (Spell: %u), ignoring", entry, spell);
        return;
    }

    TrainerSpellData& data = _cacheTrainerSpellStore[entry];

    TrainerSpell& trainerSpell = data.spellList[spell];
    trainerSpell.spell         = spell;
    trainerSpell.spellCost     = spellCost;
    trainerSpell.reqSkill      = reqSkill;
    trainerSpell.reqSkillValue = reqSkillValue;
    trainerSpell.reqLevel      = reqLevel;

    if (!trainerSpell.reqLevel)
        trainerSpell.reqLevel = l_SpellInfo->SpellLevel;

    // calculate learned spell for profession case when stored cast-spell
    trainerSpell.learnedSpell[0] = spell;
    for (uint8 i = 0; i < l_SpellInfo->EffectCount; ++i)
    {
        if (l_SpellInfo->Effects[i].Effect != SPELL_EFFECT_LEARN_SPELL)
            continue;
        if (trainerSpell.learnedSpell[0] == spell)
            trainerSpell.learnedSpell[0] = 0;
        // player must be able to cast spell on himself
        if (l_SpellInfo->Effects[i].TargetA.GetTarget() != 0 && l_SpellInfo->Effects[i].TargetA.GetTarget() != TARGET_UNIT_TARGET_ALLY
            && l_SpellInfo->Effects[i].TargetA.GetTarget() != TARGET_UNIT_TARGET_ANY && l_SpellInfo->Effects[i].TargetA.GetTarget() != TARGET_UNIT_CASTER)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `npc_trainer` has spell %u for trainer entry %u with learn effect which has incorrect target type, ignoring learn effect!", spell, entry);
            continue;
        }

        trainerSpell.learnedSpell[i] = l_SpellInfo->Effects[i].TriggerSpell;

        if (trainerSpell.learnedSpell[i])
        {
            SpellInfo const* learnedSpellInfo = sSpellMgr->GetSpellInfo(trainerSpell.learnedSpell[i]);
            if (learnedSpellInfo && learnedSpellInfo->IsProfession())
                data.trainerType = 2;
        }
    }

    return;
}

void ObjectMgr::LoadTrainerSpell()
{
    uint32 oldMSTime = getMSTime();

    // For reload case
    _cacheTrainerSpellStore.clear();

    QueryResult result = WorldDatabase.Query("SELECT b.entry, a.spell, a.spellcost, a.reqskill, a.reqskillvalue, a.reqlevel FROM npc_trainer AS a "
                                             "INNER JOIN npc_trainer AS b ON a.entry = -(b.spell) "
                                             "UNION SELECT * FROM npc_trainer WHERE spell > 0");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">>  Loaded 0 Trainers. DB table `npc_trainer` is empty!");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry         = fields[0].GetUInt32();
        uint32 spell         = fields[1].GetUInt32();
        uint32 spellCost     = fields[2].GetUInt32();
        uint32 reqSkill      = fields[3].GetUInt16();
        uint32 reqSkillValue = fields[4].GetUInt16();
        uint32 reqLevel      = fields[5].GetUInt8();

        AddSpellToTrainer(entry, spell, spellCost, reqSkill, reqSkillValue, reqLevel);

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %d Trainers in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

int ObjectMgr::LoadReferenceVendor(int32 vendor, int32 item, uint8 type, std::set<uint32>* skip_vendors)
{
    // find all items from the reference vendor
    PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_NPC_VENDOR_REF);
    stmt->setUInt32(0, uint32(item));
    stmt->setUInt8(1, type);
    PreparedQueryResult result = WorldDatabase.Query(stmt);

    if (!result)
        return 0;

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        int32 item_id = fields[0].GetInt32();

        // if item is a negative, its a reference
        if (item_id < 0)
            count += LoadReferenceVendor(vendor, -item_id, type, skip_vendors);
        else
        {
            int32  maxcount             = fields[1].GetUInt32();
            uint32 incrtime             = fields[2].GetUInt32();
            uint32 ExtendedCost         = fields[3].GetUInt32();
            uint8  type                 = fields[4].GetUInt8();
            uint32 l_PlayerConditionID  = fields[5].GetUInt32();
            std::string l_BonusesStr    = fields[6].GetCString();

            std::vector<uint32> l_Bonuses;

            Tokenizer l_BonusesSplit(l_BonusesStr, ' ');

            for (Tokenizer::const_iterator l_It = l_BonusesSplit.begin(); l_It != l_BonusesSplit.end(); ++l_It)
                l_Bonuses.push_back(atol(*l_It));

            if (!IsVendorItemValid(vendor, item_id, maxcount, incrtime, ExtendedCost, type, NULL, skip_vendors))
                continue;

            VendorItemData& vList = _cacheVendorItemStore[vendor];

            vList.AddItem(item_id, maxcount, incrtime, ExtendedCost, type, l_PlayerConditionID, l_Bonuses);
            ++count;
        }
    }
    while (result->NextRow());

    return count;
}

void ObjectMgr::LoadVendors()
{
    uint32 oldMSTime = getMSTime();

    // For reload case
    for (CacheVendorItemContainer::iterator itr = _cacheVendorItemStore.begin(); itr != _cacheVendorItemStore.end(); ++itr)
        itr->second.Clear();
    _cacheVendorItemStore.clear();

    std::set<uint32> skip_vendors;

    QueryResult result = WorldDatabase.Query("SELECT entry, item, maxcount, incrtime, ExtendedCost, type, PlayerConditionID, Bonuses FROM npc_vendor ORDER BY entry, slot ASC");
    if (!result)
    {

        sLog->outError(LOG_FILTER_SERVER_LOADING, ">>  Loaded 0 Vendors. DB table `npc_vendor` is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields           = result->Fetch();

        uint32 entry            = fields[0].GetUInt32();
        int32 item_id           = fields[1].GetInt32();

        // if item is a negative, its a reference
        if (item_id < 0)
            count += LoadReferenceVendor(entry, -item_id, 0, &skip_vendors);
        else
        {
            uint32 maxcount            = fields[2].GetUInt32();
            uint32 incrtime            = fields[3].GetUInt32();
            uint32 ExtendedCost        = fields[4].GetUInt32();
            uint8  type                = fields[5].GetUInt8();
            uint32 l_PlayerConditionID = fields[6].GetUInt32();
            std::string l_BonusesStr   = fields[7].GetCString();

            std::vector<uint32> l_Bonuses;

            Tokenizer l_BonusesSplit(l_BonusesStr, ' ');

            for (Tokenizer::const_iterator l_It = l_BonusesSplit.begin(); l_It != l_BonusesSplit.end(); ++l_It)
                l_Bonuses.push_back(atol(*l_It));

            if (!IsVendorItemValid(entry, item_id, maxcount, incrtime, ExtendedCost, type, NULL, &skip_vendors))
                continue;

            VendorItemData& vList = _cacheVendorItemStore[entry];

            vList.AddItem(item_id, maxcount, incrtime, ExtendedCost, type, l_PlayerConditionID, l_Bonuses);
            ++count;
        }
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %d Vendors in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGossipMenu()
{
    uint32 oldMSTime = getMSTime();

    _gossipMenusStore.clear();

    QueryResult result = WorldDatabase.Query("SELECT entry, text_id FROM gossip_menu");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0  gossip_menu entries. DB table `gossip_menu` is empty!");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        GossipMenus gMenu;

        gMenu.entry             = fields[0].GetUInt32();
        gMenu.text_id           = fields[1].GetUInt32();

        if (!GetGossipText(gMenu.text_id))
        {
            sLog->outError(LOG_FILTER_SQL, "Table gossip_menu entry %u are using non-existing text_id %u", gMenu.entry, gMenu.text_id);
            continue;
        }

        _gossipMenusStore.insert(GossipMenusContainer::value_type(gMenu.entry, gMenu));

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u gossip_menu entries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadGossipMenuItems()
{
    uint32 oldMSTime = getMSTime();

    _gossipMenuItemsStore.clear();

    QueryResult result = WorldDatabase.Query(
        //          0    1        2           3             4           5
        "SELECT menu_id, id, option_icon, option_text, option_id, npc_option_npcflag, "
        //      6              7            8         9          10             11                      12
        "action_menu_id, action_poi_id, box_coded, box_money, box_text, OptionBroadcastTextID, BoxBroadcastTextID "
        "FROM gossip_menu_option ORDER BY menu_id, id");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 gossip_menu_option entries. DB table `gossip_menu_option` is empty!");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint8 l_Index = 0;

        GossipMenuItems gMenuItem;

        gMenuItem.MenuId                = fields[l_Index++].GetUInt32();
        gMenuItem.OptionIndex           = fields[l_Index++].GetUInt16();
        gMenuItem.OptionIcon            = fields[l_Index++].GetUInt32();
        gMenuItem.OptionText            = fields[l_Index++].GetString();
        gMenuItem.OptionType            = fields[l_Index++].GetUInt8();
        gMenuItem.OptionNpcflag         = fields[l_Index++].GetUInt32();
        gMenuItem.ActionMenuId          = fields[l_Index++].GetUInt32();
        gMenuItem.ActionPoiId           = fields[l_Index++].GetUInt32();
        gMenuItem.BoxCoded              = fields[l_Index++].GetBool();
        gMenuItem.BoxMoney              = fields[l_Index++].GetUInt32();
        gMenuItem.BoxText               = fields[l_Index++].GetString();
        gMenuItem.OptionBroadcastTextID = fields[l_Index++].GetUInt32();
        gMenuItem.BoxBroadcastTextID    = fields[l_Index++].GetUInt32();

        if (gMenuItem.OptionIcon >= GOSSIP_ICON_MAX)
        {
            sLog->outError(LOG_FILTER_SQL, "Table gossip_menu_option for menu %u, id %u has unknown icon id %u. Replacing with GOSSIP_ICON_CHAT", gMenuItem.MenuId, gMenuItem.OptionIndex, gMenuItem.OptionIcon);
            gMenuItem.OptionIcon = GOSSIP_ICON_CHAT;
        }

        if (gMenuItem.OptionType >= GOSSIP_OPTION_MAX)
            sLog->outError(LOG_FILTER_SQL, "Table gossip_menu_option for menu %u, id %u has unknown option id %u. Option will not be used", gMenuItem.MenuId, gMenuItem.OptionIndex, gMenuItem.OptionType);

        if (gMenuItem.ActionPoiId && !GetPointOfInterest(gMenuItem.ActionPoiId))
        {
            sLog->outError(LOG_FILTER_SQL, "Table gossip_menu_option for menu %u, id %u use non-existing action_poi_id %u, ignoring", gMenuItem.MenuId, gMenuItem.OptionIndex, gMenuItem.ActionPoiId);
            gMenuItem.ActionPoiId = 0;
        }

        if (gMenuItem.OptionBroadcastTextID)
        {
            if (!sBroadcastTextStore.LookupEntry(gMenuItem.OptionBroadcastTextID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `gossip_menu_option` for menu %u, id %u has non-existing or incompatible OptionBroadcastTextId %u, ignoring.", gMenuItem.MenuId, gMenuItem.OptionIndex, gMenuItem.OptionBroadcastTextID);
                gMenuItem.OptionBroadcastTextID = 0;
            }
        }

        if (gMenuItem.BoxBroadcastTextID)
        {
            if (!sBroadcastTextStore.LookupEntry(gMenuItem.BoxBroadcastTextID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `gossip_menu_option` for menu %u, id %u has non-existing or incompatible BoxBroadcastTextID %u, ignoring.", gMenuItem.MenuId, gMenuItem.OptionIndex, gMenuItem.BoxBroadcastTextID);
                gMenuItem.BoxBroadcastTextID = 0;
            }
        }

        _gossipMenuItemsStore.insert(GossipMenuItemsContainer::value_type(gMenuItem.MenuId, gMenuItem));
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u gossip_menu_option entries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::AddVendorItem(uint32 entry, uint32 item, int32 maxcount, uint32 incrtime, uint32 extendedCost, uint8 type, bool persist /*= true*/, uint32 p_PlayerConditionID /* = 0*/)
{
    VendorItemData& vList = _cacheVendorItemStore[entry];
    vList.AddItem(item, maxcount, incrtime, extendedCost, type, p_PlayerConditionID, {});

    if (persist)
    {
        PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_NPC_VENDOR);

        stmt->setUInt32(0, entry);
        stmt->setUInt32(1, item);
        stmt->setUInt8(2, maxcount);
        stmt->setUInt32(3, incrtime);
        stmt->setUInt32(4, extendedCost);
        stmt->setUInt8(5, type);
        stmt->setUInt32(6, p_PlayerConditionID);

        WorldDatabase.Execute(stmt);
    }
}

bool ObjectMgr::RemoveVendorItem(uint32 entry, uint32 item, uint8 type, bool persist /*= true*/)
{
    CacheVendorItemContainer::iterator  iter = _cacheVendorItemStore.find(entry);
    if (iter == _cacheVendorItemStore.end())
        return false;

    if (!iter->second.RemoveItem(item, type))
        return false;

    if (persist)
    {
        PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_NPC_VENDOR);

        stmt->setUInt32(0, entry);
        stmt->setUInt32(1, item);
        stmt->setUInt8(2, type);

        WorldDatabase.Execute(stmt);
    }

    return true;
}

bool ObjectMgr::IsVendorItemValid(uint32 vendor_entry, uint32 id, int32 maxcount, uint32 incrtime, uint32 ExtendedCost, uint8 type, Player* player, std::set<uint32>* skip_vendors, uint32 ORnpcflag) const
{
    CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(vendor_entry);
    if (!cInfo)
    {
        if (player)
            ChatHandler(player).SendSysMessage(LANG_COMMAND_VENDORSELECTION);
        else
            sLog->outError(LOG_FILTER_SQL, "Table `(game_event_)npc_vendor` have data for not existed creature template (Entry: %u), ignore", vendor_entry);
        return false;
    }

    if (!((cInfo->NpcFlags1 | ORnpcflag) & UNIT_NPC_FLAG_VENDOR))
    {
        if (!skip_vendors || skip_vendors->count(vendor_entry) == 0)
        {
            if (player)
                ChatHandler(player).SendSysMessage(LANG_COMMAND_VENDORSELECTION);
            else
                sLog->outError(LOG_FILTER_SQL, "Table `(game_event_)npc_vendor` have data for not creature template (Entry: %u) without vendor flag, ignore", vendor_entry);

            if (skip_vendors)
                skip_vendors->insert(vendor_entry);
        }
        return false;
    }

    if ((type == ITEM_VENDOR_TYPE_ITEM && !sObjectMgr->GetItemTemplate(id)) ||
        (type == ITEM_VENDOR_TYPE_CURRENCY && !sCurrencyTypesStore.LookupEntry(id)))
    {
        if (player)
            ChatHandler(player).PSendSysMessage(LANG_ITEM_NOT_FOUND, id, type);
        else
            sLog->outError(LOG_FILTER_SQL, "Table `(game_event_)npc_vendor` for Vendor (Entry: %u) have in item list non-existed item (%u, type %u), ignore", vendor_entry, id, type);
        return false;
    }

    if (ExtendedCost && !sItemExtendedCostStore.LookupEntry(ExtendedCost))
    {
        if (player)
            ChatHandler(player).PSendSysMessage(LANG_EXTENDED_COST_NOT_EXIST, ExtendedCost);
        else
            sLog->outError(LOG_FILTER_SQL, "Table `(game_event_)npc_vendor` have Item (Entry: %u) with wrong ExtendedCost (%u) for vendor (%u), ignore", id, ExtendedCost, vendor_entry);
        return false;
    }

    if (type == ITEM_VENDOR_TYPE_ITEM) // not applicable to currencies
    {
        if (maxcount > 0 && incrtime == 0)
        {
            if (player)
                ChatHandler(player).PSendSysMessage("MaxCount != 0 (%u) but IncrTime == 0", maxcount);
            else
                sLog->outError(LOG_FILTER_SQL, "Table `(game_event_)npc_vendor` has `maxcount` (%u) for item %u of vendor (Entry: %u) but `incrtime`=0, ignore", maxcount, id, vendor_entry);
            return false;
        }
        else if (maxcount == 0 && incrtime > 0)
        {
            if (player)
                ChatHandler(player).PSendSysMessage("MaxCount == 0 but IncrTime<>= 0");
            else
                sLog->outError(LOG_FILTER_SQL, "Table `(game_event_)npc_vendor` has `maxcount`=0 for item %u of vendor (Entry: %u) but `incrtime`<>0, ignore", id, vendor_entry);
            return false;
        }
    }

    VendorItemData const* vItems = GetNpcVendorItemList(vendor_entry);
    if (!vItems)
        return true;                                        // later checks for non-empty lists

    if (vItems->FindItemCostPair(id, ExtendedCost, type))
    {
        if (player)
            ChatHandler(player).PSendSysMessage(LANG_ITEM_ALREADY_IN_LIST, id, ExtendedCost, type);
        else
            sLog->outError(LOG_FILTER_SQL, "Table `npc_vendor` has duplicate items %u (with extended cost %u, type %u) for vendor (Entry: %u), ignoring", id, ExtendedCost, type, vendor_entry);
        return false;
    }

    if (type == ITEM_VENDOR_TYPE_CURRENCY && maxcount == 0)
    {
        sLog->outError(LOG_FILTER_SQL, "Table `(game_event_)npc_vendor` have Item (Entry: %u, type: %u, ExtendedCost: %u) with missing maxcount for vendor (%u), ignore", id, type, ExtendedCost, vendor_entry); ///<  Data argument not used by format string
        return false;
    }

    return true;
}

void ObjectMgr::LoadScriptNames()
{
    uint32 oldMSTime = getMSTime();

    _scriptNamesStore.push_back("");
    QueryResult result = WorldDatabase.Query(
      "SELECT DISTINCT(ScriptName) FROM criteria_data WHERE ScriptName <> '' AND type = 11 "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM achievement_criteria_data WHERE ScriptName <> '' AND type = 11 "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM creature WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM battleground_template WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM gameobject WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM creature_template WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM gameobject_template WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM item_script_names WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM areatrigger_scripts WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM spell_script_names WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM transports WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM game_weather WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM conditions WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM outdoorpvp_template WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(script) FROM instance_template WHERE script <> ''"
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM areatrigger_template WHERE ScriptName <> ''"
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM eventobject_template WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM spell_scene WHERE ScriptName <> '' "
      "UNION "
      "SELECT DISTINCT(ScriptName) FROM outdoor_pve_template WHERE ScriptName <> '' ");

    if (!result)
    {

        sLog->outError(LOG_FILTER_SQL, ">> Loaded empty set of Script Names!");
        return;
    }

    uint32 count = 1;

    do
    {
        _scriptNamesStore.push_back((*result)[0].GetString());
        ++count;
    }
    while (result->NextRow());

    std::sort(_scriptNamesStore.begin(), _scriptNamesStore.end());
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %d Script Names in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

uint32 ObjectMgr::GetScriptId(const char* name)
{
    // use binary search to find the script name in the sorted vector
    // assume "" is the first element
    if (!name)
        return 0;

    ScriptNameContainer::const_iterator itr = std::lower_bound(_scriptNamesStore.begin(), _scriptNamesStore.end(), name);
    if (itr == _scriptNamesStore.end() || *itr != name)
        return 0;

    return uint32(itr - _scriptNamesStore.begin());
}

void ObjectMgr::CheckScripts(ScriptsType type, std::set<int32>& ids)
{
    ScriptMapMap* scripts = GetScriptsMapByType(type);
    if (!scripts)
        return;

    for (ScriptMapMap::const_iterator itrMM = scripts->begin(); itrMM != scripts->end(); ++itrMM)
    {
        for (ScriptMap::const_iterator itrM = itrMM->second.begin(); itrM != itrMM->second.end(); ++itrM)
        {
            switch (itrM->second.command)
            {
                case SCRIPT_COMMAND_TALK:
                {
                    if (!GetTrinityStringLocale (itrM->second.Talk.TextID))
                        sLog->outError(LOG_FILTER_SQL, "Table `%s` references invalid text id %u from `db_script_string`, script id: %u.", GetScriptsTableNameByType(type).c_str(), itrM->second.Talk.TextID, itrMM->first);

                    if (ids.find(itrM->second.Talk.TextID) != ids.end())
                        ids.erase(itrM->second.Talk.TextID);
                }
                default:
                    break;
            }
        }
    }
}

void ObjectMgr::LoadDbScriptStrings()
{
    LoadTrinityStrings("db_script_string", MIN_DB_SCRIPT_STRING_ID, MAX_DB_SCRIPT_STRING_ID);

    std::set<int32> ids;

    for (int32 i = MIN_DB_SCRIPT_STRING_ID; i < MAX_DB_SCRIPT_STRING_ID; ++i)
        if (GetTrinityStringLocale(i))
            ids.insert(i);

    for (int type = SCRIPTS_FIRST; type < SCRIPTS_LAST; ++type)
        CheckScripts(ScriptsType(type), ids);

    for (std::set<int32>::const_iterator itr = ids.begin(); itr != ids.end(); ++itr)
        sLog->outError(LOG_FILTER_SQL, "Table `db_script_string` has unused string id  %u", *itr);
}

bool LoadTrinityStrings(const char* table, int32 start_value, int32 end_value)
{
    // MAX_DB_SCRIPT_STRING_ID is max allowed negative value for scripts (scrpts can use only more deep negative values
    // start/end reversed for negative values
    if (start_value > MAX_DB_SCRIPT_STRING_ID || end_value >= start_value)
    {
        sLog->outError(LOG_FILTER_SQL, "Table '%s' load attempted with range (%d - %d) reserved by Trinity, strings not loaded.", table, start_value, end_value+1);
        return false;
    }

    return sObjectMgr->LoadTrinityStrings(table, start_value, end_value);
}

CreatureBaseStats const* ObjectMgr::GetCreatureBaseStats(uint8 level, uint8 unitClass)
{
    CreatureBaseStatsContainer::const_iterator it = _creatureBaseStatsStore.find(MAKE_PAIR16(level, unitClass));

    if (it != _creatureBaseStatsStore.end())
        return &(it->second);

    struct DefaultCreatureBaseStats : public CreatureBaseStats
    {
        DefaultCreatureBaseStats()
        {
            BaseArmor = 1;
            AttackPower = 1;
            RangedAttackPower = 1;
            for (uint8 j = 0; j < MAX_EXPANSION; ++j)
            {
                BaseHealth[j] = 1;
                BaseDamage[j] = 1;
            }
            BaseMana = 0;
        }
    };
    static const DefaultCreatureBaseStats def_stats;
    return &def_stats;
}

void ObjectMgr::LoadCreatureClassLevelStats()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT level, class, basemana, attackpower, rangedattackpower FROM creature_classlevelstats");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature base stats. DB table `creature_classlevelstats` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* l_Field = result->Fetch();

        uint8 l_ColIt = 0;

        uint8 l_Level = l_Field[l_ColIt++].GetInt8();
        uint8 l_Class = l_Field[l_ColIt++].GetInt8();

        //////////////////////////////////////////////////////////////////////////

        ArmorMitigationByLvlTableEntry  const* l_ArmorMitigationEntry   = g_ArmorMitigationByLvlTable.LookupEntry(l_Level);
        BaseManaTableEntry              const* l_BaseMPEntry            = g_BaseManaTable.LookupEntry(l_Level);

        CreatureBaseStats l_Stats;
        l_Stats.BaseMana            = l_BaseMPEntry             ? l_BaseMPEntry->m_ClassScaling.GetScalingForClassID(l_Class)   : l_Field[2].GetUInt32();
        l_Stats.BaseArmor           = l_ArmorMitigationEntry    ? uint32(l_ArmorMitigationEntry->m_Multiplier / 2.6f)           : 1;
        l_Stats.AttackPower         = l_Field[3].GetUInt16();
        l_Stats.RangedAttackPower   = l_Field[4].GetUInt16();

        if (!l_Class || ((1 << (l_Class - 1)) & CLASSMASK_ALL_CREATURES) == 0)
            sLog->outError(LOG_FILTER_SQL, "Creature base stats for level %u has invalid class %u", l_Level, l_Class);

        for (uint8 l_I = 0; l_I < MAX_EXPANSION; ++l_I)
        {
            l_Stats.BaseHealth[l_I] = g_NpcTotalHpGameTable[l_I].LookupEntry(l_Level)->m_ClassScaling.GetScalingForClassID(l_Class);
            l_Stats.BaseDamage[l_I] = g_NpcDamageByClassGameTable[l_I].LookupEntry(l_Level)->m_ClassScaling.GetScalingForClassID(l_Class);
        }

        _creatureBaseStatsStore[MAKE_PAIR16(l_Level, l_Class)] = l_Stats;

        ++count;
    }
    while (result->NextRow());

    CreatureTemplateContainer const* l_CreatureTemplates = sObjectMgr->GetCreatureTemplates();
    for (CreatureTemplateContainer::const_iterator l_Iter = l_CreatureTemplates->begin(); l_Iter != l_CreatureTemplates->end(); ++l_Iter)
    {
        for (uint16 lvl = l_Iter->second.minlevel; lvl <= l_Iter->second.maxlevel; ++lvl)
        {
            if (_creatureBaseStatsStore.find(MAKE_PAIR16(lvl, l_Iter->second.unit_class)) == _creatureBaseStatsStore.end())
                sLog->outError(LOG_FILTER_SQL, "Missing base stats for creature class %u level %u", l_Iter->second.unit_class, lvl);
        }
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature base stats in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadCreatureGroupSizeStats()
{
    uint32 l_OldMSTime = getMSTime();

    m_CreatureGroupSizeStore.clear();

    QueryResult l_Result = WorldDatabase.Query("SELECT entry, difficulty, groupSize, health FROM creature_groupsizestats");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature group size stats. DB table `creature_groupsizestats` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 l_CreatureEntry = l_Fields[0].GetUInt32();
        uint32 l_Difficulty    = l_Fields[1].GetUInt32();
        uint32 l_GroupSize     = l_Fields[2].GetUInt32();
        uint32 l_Health        = l_Fields[3].GetUInt32();

        if (!sObjectMgr->GetCreatureTemplate(l_CreatureEntry))
        {
            sLog->outError(LOG_FILTER_SQL, "Creature template entry (entry: %u) used in `creature_groupsizestats` does not exist.", l_CreatureEntry);
            continue;
        }

        if (l_Difficulty >= MaxDifficulties)
        {
            sLog->outError(LOG_FILTER_SQL, "Difficulty %u (entry %u) used in `creature_groupsizestats` is invalid.", l_Difficulty, l_CreatureEntry);
            continue;
        }

        if (l_GroupSize >= MAX_GROUP_SCALING)
        {
            sLog->outError(LOG_FILTER_SQL, "Group size %u (entry %u) used in `creature_groupsizestats` is invalid.", l_GroupSize, l_CreatureEntry);
            continue;
        }

        CreatureGroupSizeStat& l_CreatureGroupSizeStat = m_CreatureGroupSizeStore[l_CreatureEntry][l_Difficulty];
        l_CreatureGroupSizeStat.Healths[l_GroupSize] = l_Health;
        ++l_Count;
    }
    while (l_Result->NextRow());

    m_CreatureScalingPctStore.clear();

    l_Result = WorldDatabase.Query("SELECT GroupSize, Pct FROM creature_scaling_pct");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature scaling pcts. DB table `creature_scaling_pct` is empty.");
        return;
    }

    do
    {
        Field* l_Fields     = l_Result->Fetch();

        uint32 l_GroupSize  = l_Fields[0].GetUInt32();
        float  l_Percent    = l_Fields[1].GetFloat();

        if (l_GroupSize >= MAX_GROUP_SCALING)
        {
            sLog->outError(LOG_FILTER_SQL, "Group size %u used in `creature_scaling_pct` is invalid.", l_GroupSize);
            continue;
        }

        float& l_CreatureGroupSizeStat  = m_CreatureScalingPctStore[l_GroupSize];
        l_CreatureGroupSizeStat         = l_Percent;

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature group size stats and scaling percentages in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadCreatureDiffHealth()
{
    uint32 l_OldMSTime = getMSTime();

    /// For reload case
    m_CreatureDiffHealthStore.clear();

    QueryResult l_Result = WorldDatabase.Query("SELECT Entry, DifficultyID, Health FROM creature_diff_health");
    if (!l_Result)
    {
        sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded 0 creature diff health. DB table `creature_diff_health` is empty.");
        return;
    }

    uint32 l_Count = 0;

    do
    {
        Field* l_Fields         = l_Result->Fetch();

        uint8 l_Index           = 0;

        uint32 l_Entry          = l_Fields[l_Index++].GetUInt32();
        uint32 l_DifficultyID   = l_Fields[l_Index++].GetUInt32();
        uint64 l_Health         = l_Fields[l_Index++].GetUInt64();

        auto& l_DiffIter = m_CreatureDiffHealthStore[l_Entry];

        l_DiffIter[l_DifficultyID] = l_Health;

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LogFilterType::LOG_FILTER_SERVER_LOADING, ">> Loaded %u creature diff health in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadFactionChangeAchievements()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_achievement");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 faction change achievement pairs. DB table `player_factionchange_achievement` is empty.");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sAchievementStore.LookupEntry(alliance))
            sLog->outError(LOG_FILTER_SQL, "Achievement %u referenced in `player_factionchange_achievement` does not exist, pair skipped!", alliance);
        else if (!sAchievementStore.LookupEntry(horde))
            sLog->outError(LOG_FILTER_SQL, "Achievement %u referenced in `player_factionchange_achievement` does not exist, pair skipped!", horde);
        else
            FactionChange_Achievements[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u faction change achievement pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeItems()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_items");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 faction change item pairs. DB table `player_factionchange_items` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!GetItemTemplate(alliance))
            sLog->outError(LOG_FILTER_SQL, "Item %u referenced in `player_factionchange_items` does not exist, pair skipped!", alliance);
        else if (!GetItemTemplate(horde))
            sLog->outError(LOG_FILTER_SQL, "Item %u referenced in `player_factionchange_items` does not exist, pair skipped!", horde);
        else
            FactionChange_Items[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u faction change item pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeSpells()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_spells");

    if (!result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 faction change spell pairs. DB table `player_factionchange_spells` is empty.");

        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sSpellMgr->GetSpellInfo(alliance))
            sLog->outError(LOG_FILTER_SQL, "Spell %u referenced in `player_factionchange_spells` does not exist, pair skipped!", alliance);
        else if (!sSpellMgr->GetSpellInfo(horde))
            sLog->outError(LOG_FILTER_SQL, "Spell %u referenced in `player_factionchange_spells` does not exist, pair skipped!", horde);
        else
            FactionChange_Spells[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u faction change spell pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeReputations()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_reputations");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 faction change reputation pairs. DB table `player_factionchange_reputations` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sFactionStore.LookupEntry(alliance))
            sLog->outError(LOG_FILTER_SQL, "Reputation %u referenced in `player_factionchange_reputations` does not exist, pair skipped!", alliance);
        else if (!sFactionStore.LookupEntry(horde))
            sLog->outError(LOG_FILTER_SQL, "Reputation %u referenced in `player_factionchange_reputations` does not exist, pair skipped!", horde);
        else
            FactionChange_Reputation[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u faction change reputation pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadFactionChangeTitles()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT alliance_id, horde_id FROM player_factionchange_titles");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 faction change title pairs. DB table `player_factionchange_title` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sCharTitlesStore.LookupEntry(alliance))
            sLog->outError(LOG_FILTER_SQL, "Title %u referenced in `player_factionchange_title` does not exist, pair skipped!", alliance);
        else if (!sCharTitlesStore.LookupEntry(horde))
            sLog->outError(LOG_FILTER_SQL, "Title %u referenced in `player_factionchange_title` does not exist, pair skipped!", horde);
        else
            FactionChange_Titles[alliance] = horde;

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u faction change title pairs in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadHotfixData(bool p_Reload)
{
    uint32 oldMSTime = getMSTime();

    auto l_ProcessHotifx = [this](HotfixInfo p_Info, bool p_Reloaded) -> void
    {
        if (p_Reloaded)
        {
            auto l_It = std::find_if(_hotfixData.begin(), _hotfixData.end(), [p_Info](HotfixInfo const& p_Elem) -> bool
            {
                return p_Elem.Type == p_Info.Type && p_Elem.Entry == p_Info.Entry;
            });

            if (l_It == _hotfixData.end())
                _hotfixData.push_back(p_Info);
        }
        else
            _hotfixData.push_back(p_Info);
    };

    QueryResult result = HotfixDatabase.Query("SELECT Entry, Hash, Date FROM _hotfixs");

    uint32 l_Count = 0;
    bool l_HaveError = false;
    std::set<uint32> l_ReportedError;

    if (result)
    {
        if (!p_Reload)
        {
            _hotfixData.clear();
            _hotfixData.reserve(result->GetRowCount());
        }

        do
        {
            Field* l_Fields = result->Fetch();

            HotfixInfo l_Infos;
            l_Infos.Entry       = l_Fields[0].GetInt32();
            l_Infos.Type        = l_Fields[1].GetUInt32();
            l_Infos.Timestamp   = l_Fields[2].GetUInt32();

            if (l_Infos.Entry < 0)
            {
                l_Infos.Entry = -l_Infos.Entry;
                l_Infos.Entry |= HOTFIX_REMOVE_ROW_ENTRY_FLAG;
            }

            l_ProcessHotifx(l_Infos, p_Reload);

            if (sDB2PerHash.find(l_Infos.Type) == sDB2PerHash.end() && l_ReportedError.find(l_Infos.Type) == l_ReportedError.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Error db2 0x%08X have hotfix but not implemented in the core", l_Infos.Type);
                l_ReportedError.emplace(l_Infos.Type);
                l_HaveError = true;
            }

            ++l_Count;
        }
        while (result->NextRow());
    }

    if (l_HaveError)
    {
        l_ReportedError.clear();    ///< for debug breakpoint
        ///exit(EXIT_FAILURE);
    }

    result = HotfixDatabase.Query("SELECT ID FROM _custom_items");

    if (result)
    {
        do
        {
            Field* l_Fields = result->Fetch();
            uint32 l_ItemID = l_Fields[0].GetUInt32();

            if (sItemStore.LookupEntry(l_ItemID))
            {
                HotfixInfo l_Infos;
                l_Infos.Type         = sItemStore.GetHash();
                l_Infos.Timestamp    = time(nullptr);
                l_Infos.Entry        = l_ItemID;
                l_ProcessHotifx(l_Infos, p_Reload);
            }

            if (sItemSparseStore.LookupEntry(l_ItemID))
            {
                HotfixInfo l_Infos;
                l_Infos.Type        = sItemSparseStore.GetHash();
                l_Infos.Timestamp   = time(nullptr);
                l_Infos.Entry       = l_ItemID;
                l_ProcessHotifx(l_Infos, p_Reload);
            }

            for (uint32 l_I = 0; l_I < sPvpItemStore.GetNumRows(); ++l_I)
            {
                PvpItemEntry const* l_Entry = sPvpItemStore.LookupEntry(l_I);
                if (l_Entry == nullptr || l_Entry->itemId != l_ItemID)
                    continue;

                HotfixInfo l_Infos;
                l_Infos.Type        = sPvpItemStore.GetHash();
                l_Infos.Timestamp   = time(nullptr);
                l_Infos.Entry       = l_I;
                l_ProcessHotifx(l_Infos, p_Reload);
            }

            for (uint32 l_I = 0; l_I < sItemEffectStore.GetNumRows(); ++l_I)
            {
                ItemEffectEntry const* l_Entry = sItemEffectStore.LookupEntry(l_I);

                if (!l_Entry || l_Entry->ItemID != l_ItemID)
                    continue;

                HotfixInfo l_Infos;
                l_Infos.Type        = sItemEffectStore.GetHash();
                l_Infos.Timestamp   = time(nullptr);
                l_Infos.Entry       = l_I;
                l_ProcessHotifx(l_Infos, p_Reload);
            }

            for (uint32 l_I = 0; l_I < sItemModifiedAppearanceStore.GetNumRows(); ++l_I)
            {
                ItemModifiedAppearanceEntry const* l_Entry = sItemModifiedAppearanceStore.LookupEntry(l_I);

                if (!l_Entry || l_Entry->ItemID != l_ItemID)
                    continue;

                HotfixInfo l_Infos;
                l_Infos.Type        = sItemModifiedAppearanceStore.GetHash();
                l_Infos.Timestamp   = time(nullptr);
                l_Infos.Entry       = l_I;
                l_ProcessHotifx(l_Infos, p_Reload);

                if (sItemAppearanceStore.LookupEntry(l_Entry->AppearanceID))
                {
                    l_Infos.Type        = sItemAppearanceStore.GetHash();
                    l_Infos.Timestamp   = time(nullptr);
                    l_Infos.Entry       = l_Entry->AppearanceID;
                    l_ProcessHotifx(l_Infos, p_Reload);
                }
            }
            ++l_Count;
        } while (result->NextRow());
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u hotfix info entries in %u ms", l_Count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadHotfixTableHashs()
{
    QueryResult l_Result = HotfixDatabase.Query("SELECT ID, Name FROM _hashs");

    if (!l_Result)
        return;

    HotfixTableID.clear();

    do
    {
        Field* l_Fields = l_Result->Fetch();

        HotfixTableID[l_Fields[1].GetString()] = l_Fields[0].GetUInt32();
    } while (l_Result->NextRow());

    l_Result = HotfixDatabase.Query("SELECT Hash, IndexInFields FROM _index_infos");

    if (!l_Result)
        return;

    HotfixTableIndexInfos.clear();

    do
    {
        Field* l_Fields = l_Result->Fetch();

        HotfixTableIndexInfos[l_Fields[0].GetUInt32()] = l_Fields[1].GetUInt8();
    } while (l_Result->NextRow());
}

void ObjectMgr::LoadPhaseDefinitions()
{
    _PhaseDefinitionStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                                 0       1       2         3            4           5,                 6
    QueryResult result = WorldDatabase.Query("SELECT zoneId, entry, phasemask, phaseId, terrainswapmap, uiworldmapareaswap, flags FROM `phase_definitions` ORDER BY `entry` ASC");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 phasing definitions. DB table `phase_definitions` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        PhaseDefinition pd;

        pd.zoneId                = fields[0].GetUInt32();
        pd.entry                 = fields[1].GetUInt32();
        pd.phasemask             = fields[2].GetUInt32();
        pd.phaseId               = fields[3].GetUInt32();
        pd.terrainswapmap        = fields[4].GetUInt32();
        pd.uiworldmaparea        = fields[5].GetUInt32();
        pd.flags                 = fields[6].GetUInt8();

        // Checks
        if ((pd.flags & PHASE_FLAG_OVERWRITE_EXISTING) && (pd.flags & PHASE_FLAG_NEGATE_PHASE))
        {
            sLog->outError(LOG_FILTER_SQL, "Flags defined in phase_definitions in zoneId %d and entry %u does contain PHASE_FLAG_OVERWRITE_EXISTING and PHASE_FLAG_NEGATE_PHASE. Setting flags to PHASE_FLAG_OVERWRITE_EXISTING", pd.zoneId, pd.entry);
            pd.flags &= ~PHASE_FLAG_NEGATE_PHASE;
        }

        _PhaseDefinitionStore[pd.zoneId].push_back(pd);

        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u phasing definitions in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadSpellPhaseInfo()
{
    _SpellPhaseStore.clear();

    uint32 oldMSTime = getMSTime();

    //                                               0       1            2                3
    QueryResult result = WorldDatabase.Query("SELECT id, phasemask, terrainswapmap, uiworldmapareaswap FROM `spell_phase`");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spell db2 infos. DB table `spell_phase` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        SpellPhaseInfo spellPhaseInfo;
        spellPhaseInfo.spellId                = fields[0].GetUInt32();

        SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellPhaseInfo.spellId);
        if (!spell)
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u defined in `spell_phase` does not exists, skipped.", spellPhaseInfo.spellId);
            continue;
        }

        if (!spell->HasAura(SPELL_AURA_PHASE))
        {
            sLog->outError(LOG_FILTER_SQL, "Spell %u defined in `spell_phase` does not have aura effect type SPELL_AURA_PHASE, useless value.", spellPhaseInfo.spellId);
            continue;
        }

        spellPhaseInfo.phasemask              = fields[1].GetUInt32();
        spellPhaseInfo.terrainswapmap         = fields[2].GetUInt32();
        spellPhaseInfo.uiworldmaparea         = fields[3].GetUInt32();

        _SpellPhaseStore[spellPhaseInfo.spellId] = spellPhaseInfo;

        ++count;
    }
    while (result->NextRow());
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spell db2 infos in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadConversationData()
{
    uint32 l_oldMSTime = getMSTime();
    m_ConversationData.clear();
    m_ConversationCreature.clear();
    m_ConversationActor.clear();

    //                                                  0       1       2       3       4
    QueryResult l_Result = WorldDatabase.Query("SELECT `entry`, `id`, `textId`, `unk1`, `unk2` FROM `conversation_data` ORDER BY idx");
    if (l_Result)
    {
        uint32 l_Counter = 0;
        do
        {
            Field* l_Fields = l_Result->Fetch();
            uint8 l_ColIndex = 0;

            ConversationData l_Data;
            l_Data.Entry    = l_Fields[l_ColIndex++].GetUInt32();
            l_Data.ID       = l_Fields[l_ColIndex++].GetUInt32();
            l_Data.TextID   = l_Fields[l_ColIndex++].GetUInt32();
            l_Data.Unk1     = l_Fields[l_ColIndex++].GetUInt32();
            l_Data.Unk2     = l_Fields[l_ColIndex++].GetUInt32();

            m_ConversationData[l_Data.Entry].push_back(l_Data);
            ++l_Counter;
        }
        while (l_Result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u conversation data in %u ms", l_Counter, GetMSTimeDiffToNow(l_oldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 conversation data. DB table `conversation_data` is empty.");

    //                                      0      1         2               3          4       5         6
    l_Result = WorldDatabase.Query("SELECT `entry`, `id`, `creatureId`, `creatureGuid`, `unk1`, `unk2`, `duration` FROM `conversation_creature` ORDER BY id");
    if (l_Result)
    {
        uint32 l_Counter = 0;
        do
        {
            Field* l_Fields = l_Result->Fetch();
            uint8 l_ColIndex = 0;

            ConversationCreature l_CreatureData;
            l_CreatureData.Entry        = l_Fields[l_ColIndex++].GetUInt32();
            l_CreatureData.ID           = l_Fields[l_ColIndex++].GetUInt32();
            l_CreatureData.CreatureID   = l_Fields[l_ColIndex++].GetUInt32();
            l_CreatureData.CreatureGuid = l_Fields[l_ColIndex++].GetUInt32();
            l_CreatureData.Unk1         = l_Fields[l_ColIndex++].GetUInt32();
            l_CreatureData.Unk2         = l_Fields[l_ColIndex++].GetUInt32();
            l_CreatureData.Duration     = l_Fields[l_ColIndex++].GetUInt32();

            m_ConversationCreature[l_CreatureData.Entry].push_back(l_CreatureData);

            ++l_Counter;
        }
        while (l_Result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u conversation creature data in %u ms.", l_Counter, GetMSTimeDiffToNow(l_oldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 conversation creature data. DB table `conversation_creature` is empty.");

    //                                      0      1        2           3             4         5       6       7         8
    l_Result = WorldDatabase.Query("SELECT `entry`, `id`, `actorId`, `creatureId`, `displayId`, `unk1`, `unk2`, `unk3`, `duration` FROM `conversation_actor` ORDER BY id");
    if (l_Result)
    {
        uint32 l_Counter = 0;
        do
        {
            Field* l_Fields = l_Result->Fetch();
            uint8 l_ColIndex = 0;

            ConversationActor l_ActorData;
            l_ActorData.entry       = l_Fields[l_ColIndex++].GetUInt32();
            l_ActorData.ID          = l_Fields[l_ColIndex++].GetUInt32();
            l_ActorData.ActorID     = l_Fields[l_ColIndex++].GetUInt32();
            l_ActorData.CreatureID  = l_Fields[l_ColIndex++].GetUInt32();
            l_ActorData.DisplayID   = l_Fields[l_ColIndex++].GetUInt32();
            l_ActorData.Unk1        = l_Fields[l_ColIndex++].GetUInt32();
            l_ActorData.Unk2        = l_Fields[l_ColIndex++].GetUInt32();
            l_ActorData.Unk3        = l_Fields[l_ColIndex++].GetUInt32();
            l_ActorData.Duration    = l_Fields[l_ColIndex++].GetUInt32();

            m_ConversationActor[l_ActorData.entry].push_back(l_ActorData);

            ++l_Counter;
        }
        while (l_Result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u conversation actor data in %u ms.", l_Counter, GetMSTimeDiffToNow(l_oldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 conversation actor data. DB table `conversation_actor` is empty.");
}

void ObjectMgr::LoadBattlePetTemplate()
{
    uint32 oldMSTime = getMSTime();

    _battlePetTemplateStore.clear();

    QueryResult result = WorldDatabase.Query("SELECT species, breed, quality, level FROM battlepet_template");

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 battlepet template. DB table `battlepet_template` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        BattlePetTemplate temp;
        temp.Species = fields[0].GetUInt32();
        temp.Breed = fields[1].GetUInt32();
        temp.Quality = fields[2].GetUInt32();
        temp.Level = fields[3].GetUInt32();
        _battlePetTemplateStore[temp.Species] = temp;
        count += 1;
    } while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u battlepet template in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

void ObjectMgr::LoadBattlePetNpcTeamMember()
{
    uint32 l_OldMSTime = getMSTime();

    m_BattlePetNpcTeamMembers.clear();

    QueryResult l_Result = WorldDatabase.Query("SELECT NpcID, Specie, Level, Ability1, Ability2, Ability3 FROM battlepet_npc_team_member");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 battlepet npc team member. DB table `battlepet_npc_team_member` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();

        BattlePetNpcTeamMember l_Current;
        l_Current.Specie        = l_Fields[1].GetUInt32();
        l_Current.Level         = l_Fields[2].GetUInt32();
        l_Current.Ability[0]    = l_Fields[2].GetUInt32();
        l_Current.Ability[1]    = l_Fields[3].GetUInt32();
        l_Current.Ability[2]    = l_Fields[4].GetUInt32();

        m_BattlePetNpcTeamMembers[l_Fields[0].GetUInt32()].push_back(l_Current);
        l_Count += 1;

    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u battlepet npc team member in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

#include <fstream>
#include <iostream>

/// Compute battle pet spawns
void ObjectMgr::ComputeBattlePetSpawns()
{
    uint32 l_OldMSTime = getMSTime();
    QueryResult l_Result = WorldDatabase.Query("SELECT CritterEntry, BattlePetEntry FROM temp_battlepet_spawn_relation a");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> ComputeBattlePetSpawns No battlepet relation");
        return;
    }

    std::map<uint32, uint32> l_BattlePetToCritter;
    do
    {
        Field* l_Fields = l_Result->Fetch();
        l_BattlePetToCritter[l_Fields[1].GetUInt32()] = l_Fields[0].GetUInt32();
    } while (l_Result->NextRow());

    l_Result = WorldDatabase.Query("SELECT MapID, a.Zone, BattlePetNPCID, XPos, YPos, MinLevel, MaxLevel FROM temp_battlepet_tocompute a");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> ComputeBattlePetSpawns No data");
        return;
    }

    struct PoolInfo
    {
        uint32 ZoneID;
        std::map<uint32, uint32> CountPerBattlePetTemplateEntry;
        uint32 MinLevel;
        uint32 MaxLevel;
    };

    std::map<uint32, PoolInfo> l_PoolInfosPerZoneID;
    std::map<uint32, uint32> l_MissingCorelations;

    std::ofstream l_OutSpawns;
    l_OutSpawns.open("BattlePetSpawns.sql");

    do
    {
        Field* l_Fields = l_Result->Fetch();
        uint32 l_MapID = l_Fields[0].GetUInt32();
        uint32 l_ZoneID = l_Fields[1].GetUInt32();
        uint32 l_BattlePetNpcID = l_Fields[2].GetUInt32();
        double l_XPos = l_Fields[3].GetDouble();
        double l_YPos = l_Fields[4].GetDouble();
        uint32 l_MinLevel = l_Fields[5].GetUInt32();
        uint32 l_MaxLevel = l_Fields[6].GetUInt32();

        if (l_BattlePetToCritter.find(l_BattlePetNpcID) == l_BattlePetToCritter.end())
        {
            l_MissingCorelations[l_BattlePetNpcID] = 1;
            continue;
        }

        if (!MapManager::IsValidMapCoord(l_MapID, l_XPos, l_YPos))
        {
            printf("Map %u Zone %u Npc %u X %f Y %F invalid map coord\n", l_MapID, l_ZoneID, l_BattlePetNpcID, l_XPos, l_YPos);
            continue;
        }

        Map const* l_Map = sMapMgr->CreateBaseMap(l_MapID, 0);
        float l_ZPos = l_Map->GetHeight(l_XPos, l_YPos, MAX_HEIGHT) + 0.5f;

        std::string l_Query = "INSERT INTO creature(id, map, zoneID, spawnMask, phaseMask, position_x, position_y, position_z, spawntimesecs) VALUES (";
        l_Query += std::to_string(l_BattlePetToCritter[l_BattlePetNpcID]) + ", " + std::to_string(l_MapID) + ", " + std::to_string(l_ZoneID) + ", 1, 1, " + std::to_string(l_XPos) + ", " + std::to_string(l_YPos) + ", " + std::to_string(l_ZPos) + ", 120);\n";

        l_OutSpawns << l_Query << std::flush;

        l_PoolInfosPerZoneID[l_ZoneID].ZoneID = l_ZoneID;
        l_PoolInfosPerZoneID[l_ZoneID].MinLevel = l_MinLevel;
        l_PoolInfosPerZoneID[l_ZoneID].MaxLevel = l_MaxLevel;

        if (l_PoolInfosPerZoneID[l_ZoneID].CountPerBattlePetTemplateEntry.find(l_BattlePetNpcID) == l_PoolInfosPerZoneID[l_ZoneID].CountPerBattlePetTemplateEntry.end())
            l_PoolInfosPerZoneID[l_ZoneID].CountPerBattlePetTemplateEntry[l_BattlePetNpcID] = 1;
        else
            l_PoolInfosPerZoneID[l_ZoneID].CountPerBattlePetTemplateEntry[l_BattlePetNpcID] = 1 + l_PoolInfosPerZoneID[l_ZoneID].CountPerBattlePetTemplateEntry[l_BattlePetNpcID];
    } while (l_Result->NextRow());

    for (std::map<uint32, uint32>::iterator l_Current = l_MissingCorelations.begin(); l_Current != l_MissingCorelations.end(); l_Current++)
        printf("Npc %u no critter npc found\n", l_Current->first);

    l_OutSpawns.close();

    std::ofstream l_OutPools;
    l_OutPools.open("BattlePetPools.sql");
    for (std::map<uint32, PoolInfo>::iterator l_Current = l_PoolInfosPerZoneID.begin(); l_Current != l_PoolInfosPerZoneID.end(); l_Current++)
    {
        PoolInfo& l_PoolInfo = l_Current->second;

        for (std::map<uint32, uint32>::iterator l_CurrentTemplate = l_PoolInfo.CountPerBattlePetTemplateEntry.begin(); l_CurrentTemplate != l_PoolInfo.CountPerBattlePetTemplateEntry.end(); l_CurrentTemplate++)
        {
            uint32 l_RespawnTime = 60;
            uint32 l_Replace = l_BattlePetToCritter[l_CurrentTemplate->first];
            uint32 l_Max = float(l_CurrentTemplate->second) > 1 ? (float(l_CurrentTemplate->second) * 0.95f) : 1;

            if (l_BattlePetToCritter[l_CurrentTemplate->first] == l_CurrentTemplate->first)
                l_Max = l_CurrentTemplate->second;

            uint32 l_Species = 0;

            for (std::size_t l_I = 0; l_I < sBattlePetSpeciesStore.GetNumRows(); ++l_I)
            {
                BattlePetSpeciesEntry const* l_Entry = sBattlePetSpeciesStore.LookupEntry(l_I);

                if (!l_Entry || l_Entry->CreatureID != l_CurrentTemplate->first)
                    continue;

                l_Species = l_Entry->ID;
                break;
            }

            if (l_Species == 0 || l_Replace == 0)
            {
                printf("No species or replacement for npc %u found\n", l_CurrentTemplate->first);
                continue;
            }

            std::string l_Query = "INSERT INTO `wild_battlepet_zone_pool` (`Zone`, `Species`, `Replace`, `Max`, `RespawnTime`, `MinLevel`, `MaxLevel`, `Breed0`, `Breed1`, `Breed2`, `Breed3`, `Breed4`, `Breed5`, `Breed6`, `Breed7`, `Breed8`, `Breed9`) VALUES (";
            l_Query += std::to_string(l_PoolInfo.ZoneID) + ", " + std::to_string(l_Species) + ", " + std::to_string(l_Replace) + ", " + std::to_string(l_Max) + ", " + std::to_string(l_RespawnTime) + ", " + std::to_string(l_PoolInfo.MinLevel) + ", " + std::to_string(l_PoolInfo.MaxLevel) + ", '3', '3', '3', '3', '3', '3', '3', '3', '3', '3');\n";

            l_OutPools << l_Query << std::flush;
        }
    }

    l_OutPools.close();

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> ComputeBattlePetSpawns %u ms.", GetMSTimeDiffToNow(l_OldMSTime));

#ifdef _WIN32
    system("pause");
#endif
}

GameObjectTemplate const* ObjectMgr::GetGameObjectTemplate(uint32 entry)
{
    GameObjectTemplateContainer::const_iterator itr = _gameObjectTemplateStore.find(entry);
    if (itr != _gameObjectTemplateStore.end())
        return &(itr->second);

    return NULL;
}

void ObjectMgr::LoadEventObjectTemplates()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query(
    ///       0      1      2       3            4             5
    "SELECT entry, name, radius, SpellID, WorldSafeLocID, ScriptName FROM eventobject_template;");

    uint32 l_Count = 0;
    if (l_Result)
    {
        do
        {
            uint8 l_Index = 0;
            Field* l_Fields = l_Result->Fetch();
            uint32 l_Entry = l_Fields[l_Index++].GetUInt32();

            EventObjectTemplate& l_EventObjectTemplate = m_EventObjectTemplateMap[l_Entry];

            l_EventObjectTemplate.Entry             = l_Entry;
            l_EventObjectTemplate.Name              = l_Fields[l_Index++].GetString();
            l_EventObjectTemplate.Radius            = l_Fields[l_Index++].GetFloat();
            l_EventObjectTemplate.SpellID           = l_Fields[l_Index++].GetUInt32();
            l_EventObjectTemplate.WorldSafeLocID    = l_Fields[l_Index++].GetUInt32();
            l_EventObjectTemplate.ScriptID          = GetScriptId(l_Fields[l_Index++].GetCString());

            ++l_Count;
        }
        while (l_Result->NextRow());
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u eventobject template in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadEventObjects()
{
    uint32 l_OldMSTime = getMSTime();

    ///                                                0    1   2      3      4       5           6           7           8            9            10        11
    QueryResult l_Result = WorldDatabase.Query("SELECT guid, id, map, zoneId, areaId, position_x, position_y, position_z, orientation, spawnMask, phaseMask, PhaseId "
        "FROM eventobject ORDER BY `map` ASC, `guid` ASC");

    if (!l_Result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 eventobject. DB table `eventobject` is empty.");
        return;
    }

    /// Build single time for check spawnmask
    std::map<uint32, uint32> l_SpawnMasks;
    for (uint32 l_I = 0; l_I < sMapDifficultyStore.GetNumRows(); ++l_I)
    {
        if (MapDifficultyEntry const* l_MapDiff = sMapDifficultyStore.LookupEntry(l_I))
            l_SpawnMasks[l_MapDiff->MapID] |= (1 << l_MapDiff->DifficultyID);
    }

    m_EventObjectData.rehash(l_Result->GetRowCount());
    std::map<uint32, EventObjectData*> l_LastEntry;

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint8 l_Index = 0;

        uint32 l_Guid = l_Fields[l_Index++].GetUInt32();
        uint32 l_Entry = l_Fields[l_Index++].GetUInt32();

        EventObjectData& l_EoData = m_EventObjectData[l_Guid];

        l_EoData.Guid           = l_Guid;
        l_EoData.ID             = l_Entry;
        l_EoData.MapID          = l_Fields[l_Index++].GetUInt16();
        l_EoData.ZoneID         = l_Fields[l_Index++].GetUInt16();
        l_EoData.AreaID         = l_Fields[l_Index++].GetUInt16();
        l_EoData.PosX           = l_Fields[l_Index++].GetFloat();
        l_EoData.PosY           = l_Fields[l_Index++].GetFloat();
        l_EoData.PosZ           = l_Fields[l_Index++].GetFloat();
        l_EoData.Orientation    = l_Fields[l_Index++].GetFloat();
        l_EoData.SpawnMask      = l_Fields[l_Index++].GetUInt32();
        l_EoData.PhaseMask      = l_Fields[l_Index++].GetUInt32();
        l_EoData.PhaseID        = l_Fields[l_Index++].GetUInt32();

        /// Check near npc with same entry.
        auto l_LastObject = l_LastEntry.find(l_Entry);
        if (l_LastObject != l_LastEntry.end())
        {
            if (l_EoData.MapID == l_LastObject->second->MapID)
            {
                float l_Dx1 = l_LastObject->second->PosX - l_EoData.PosX;
                float l_Dy1 = l_LastObject->second->PosY - l_EoData.PosY;
                float l_Dz1 = l_LastObject->second->PosZ - l_EoData.PosZ;

                float l_DistSq1 = l_Dx1 * l_Dx1 + l_Dy1 * l_Dy1 + l_Dz1 * l_Dz1;
                if (l_DistSq1 < 0.5f)
                {
                    l_LastObject->second->PhaseMask |= l_EoData.PhaseMask;
                    l_LastObject->second->SpawnMask |= l_EoData.SpawnMask;
                    WorldDatabase.PExecute("UPDATE eventobject SET phaseMask = %u, spawnMask = %u WHERE guid = %u", l_LastObject->second->PhaseMask, l_LastObject->second->SpawnMask, l_LastObject->second->Guid);
                    WorldDatabase.PExecute("DELETE FROM eventobject WHERE guid = %u", l_Guid);
                    sLog->outError(LOG_FILTER_SQL, "Table `eventobject` have clone npc %u witch stay too close (dist: %f). original npc guid %u. npc with guid %u will be deleted.", l_Entry, l_DistSq1, l_LastObject->second->Guid, l_Guid);
                    continue;
                }
            }
            else
                l_LastEntry[l_Entry] = &l_EoData;
        }
        else
            l_LastEntry[l_Entry] = &l_EoData;

        MapEntry const* l_MapEntry = sMapStore.LookupEntry(l_EoData.MapID);
        if (!l_MapEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `eventobject` have eventobject (GUID: %u) that spawned at not existed map (Id: %u), skipped.", l_Guid, l_EoData.MapID);
            continue;
        }

        if (l_EoData.SpawnMask & ~l_SpawnMasks[l_EoData.MapID])
        {
            sLog->outError(LOG_FILTER_SQL, "Table `eventobject` have eventobject (GUID: %u) that have wrong spawn mask %u including not supported difficulty modes for map (Id: %u) spawnMasks[data.mapid]: %u.", l_Guid, l_EoData.SpawnMask, l_EoData.MapID, l_SpawnMasks[l_EoData.MapID]);
            WorldDatabase.PExecute("UPDATE eventobject SET spawnMask = %u WHERE guid = %u", l_SpawnMasks[l_EoData.MapID], l_Guid);
            l_EoData.SpawnMask = l_SpawnMasks[l_EoData.MapID];
        }

        if (l_EoData.PhaseMask == 0)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `eventobject` have eventobject (GUID: %u Entry: %u) with `phaseMask`=0 (not visible for anyone), set to 1.", l_Guid, l_EoData.ID);
            l_EoData.PhaseMask = 1;
        }

        /// Add to grid if not managed by the game event or pool system
        AddEventObjectToGrid(l_Guid, &l_EoData);

        if (!l_EoData.ZoneID || !l_EoData.AreaID)
        {
            uint32 l_ZoneID = 0;
            uint32 l_AreaID = 0;

            sMapMgr->GetZoneAndAreaId(l_ZoneID, l_AreaID, l_EoData.MapID, l_EoData.PosX, l_EoData.PosY, l_EoData.PosZ);

            l_EoData.ZoneID = l_ZoneID;
            l_EoData.AreaID = l_AreaID;

            WorldDatabase.PExecute("UPDATE eventobject SET zoneId = %u, areaId = %u WHERE guid = %u", l_ZoneID, l_AreaID, l_Guid);
        }

        ++l_Count;

    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u eventobject in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

EventObjectTemplate const* ObjectMgr::GetEventObjectTemplate(uint32 p_Entry)
{
    EventObjectTemplateContainer::const_iterator l_Iter = m_EventObjectTemplateMap.find(p_Entry);
    if (l_Iter != m_EventObjectTemplateMap.end())
        return &(l_Iter->second);

    return nullptr;
}

EventObjectTemplate const* ObjectMgr::AddEventObjectTemplate(uint32 p_Entry, float p_Radius, uint32 p_SpellID, uint32 p_WorldSafe)
{
    EventObjectTemplate& l_EventObjectTemplate  = m_EventObjectTemplateMap[p_Entry];
    l_EventObjectTemplate.Name                  = "";
    l_EventObjectTemplate.Radius                = p_Radius;
    l_EventObjectTemplate.SpellID               = p_SpellID;
    l_EventObjectTemplate.WorldSafeLocID        = p_WorldSafe;
    l_EventObjectTemplate.ScriptID              = 0;

    WorldDatabase.PExecute("INSERT INTO eventobject_template SET SpellID = %u, radius = %f, WorldSafeLocID = %u, entry = %u", p_SpellID, p_Radius, p_WorldSafe, p_Entry);

    return &l_EventObjectTemplate;
}

void ObjectMgr::AddEventObjectToGrid(uint32 const& p_Guid, EventObjectData const* p_Data)
{
    uint32 l_Mask = p_Data->SpawnMask;
    for (uint32 l_I = 0; l_Mask != 0; l_I++, l_Mask >>= 1)
    {
        if (l_Mask & 1)
        {
            CellCoord l_CellCoord = JadeCore::ComputeCellCoord(p_Data->PosX, p_Data->PosY);
            CellObjectGuids& l_CellGuids = _mapObjectGuidsStore[MAKE_PAIR32(p_Data->MapID, l_I)][l_CellCoord.GetId()];
            l_CellGuids.eventobject.insert(p_Guid);
        }
    }
}

void ObjectMgr::LoadAreaTriggers()
{
    uint32 l_OldMSTime = getMSTime();

    ///                                                0     1      2            3        4    5       6       7          8          9        10          11          12          13            14          15          16      17              18              19
    QueryResult l_Result = WorldDatabase.Query("SELECT guid, entry, customEntry, spellId, map, zoneId, areaId, spawnMask, phaseMask, PhaseId, position_x, position_y, position_z, orientation, radius, radiusTarget, Height, heightTarget, DecalPropertiesId, MorphCurveId "
        "FROM areatrigger ORDER BY `map` ASC, `guid` ASC");

    if (!l_Result)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 areatrigger. DB table `areatrigger` is empty.");
        return;
    }

    /// Build single time for check spawnmask
    std::map<uint32, uint32> l_SpawnMasks;
    for (uint32 l_I = 0; l_I < sMapDifficultyStore.GetNumRows(); ++l_I)
    {
        if (MapDifficultyEntry const* l_MapDiff = sMapDifficultyStore.LookupEntry(l_I))
            l_SpawnMasks[l_MapDiff->MapID] |= (1 << l_MapDiff->DifficultyID);
    }

    m_AreaTriggerData.rehash(l_Result->GetRowCount());
    std::map<uint32, AreaTriggerData*> l_LastEntry;

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint8 l_Index = 0;

        uint32 l_Guid = l_Fields[l_Index++].GetUInt32();

        AreaTriggerData& l_ATData = m_AreaTriggerData[l_Guid];

        l_ATData.Guid               = l_Guid;
        l_ATData.Entry              = l_Fields[l_Index++].GetUInt32();
        l_ATData.CustomEntry        = l_Fields[l_Index++].GetUInt32();
        l_ATData.SpellID            = l_Fields[l_Index++].GetUInt32();
        l_ATData.MapID              = l_Fields[l_Index++].GetUInt16();
        l_ATData.ZoneID             = l_Fields[l_Index++].GetUInt16();
        l_ATData.AreaID             = l_Fields[l_Index++].GetUInt16();
        l_ATData.SpawnMask          = l_Fields[l_Index++].GetUInt32();
        l_ATData.PhaseMask          = l_Fields[l_Index++].GetUInt32();
        l_ATData.PhaseID            = l_Fields[l_Index++].GetUInt32();
        l_ATData.PosX               = l_Fields[l_Index++].GetFloat();
        l_ATData.PosY               = l_Fields[l_Index++].GetFloat();
        l_ATData.PosZ               = l_Fields[l_Index++].GetFloat();
        l_ATData.Orientation        = l_Fields[l_Index++].GetFloat();
        l_ATData.Radius             = l_Fields[l_Index++].GetFloat();
        l_ATData.RadiusTarget       = l_Fields[l_Index++].GetFloat();
        l_ATData.Height             = l_Fields[l_Index++].GetFloat();
        l_ATData.HeightTarget       = l_Fields[l_Index++].GetFloat();
        l_ATData.DecalPropertiesId  = l_Fields[l_Index++].GetUInt32();
        l_ATData.MorphCurveId       = l_Fields[l_Index++].GetUInt32();

        MapEntry const* l_MapEntry = sMapStore.LookupEntry(l_ATData.MapID);
        if (!l_MapEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `areatrigger` have areatrigger (GUID: %u) that spawned at not existed map (Id: %u), skipped.", l_Guid, l_ATData.MapID);
            continue;
        }

        if (!l_ATData.Entry && !l_ATData.CustomEntry)
        {
            sLog->outError(LOG_FILTER_SQL, "Table `areatrigger` has areatrigger (GUID: %u) that spawned with null entry, skipped.", l_Guid);
            continue;
        }

        if (l_ATData.SpawnMask & ~l_SpawnMasks[l_ATData.MapID])
        {
            sLog->outError(LOG_FILTER_SQL, "Table `areatrigger` have areatrigger (GUID: %u) that have wrong spawn mask %u including not supported difficulty modes for map (Id: %u) spawnMasks[data.mapid]: %u.", l_Guid, l_ATData.SpawnMask, l_ATData.MapID, l_SpawnMasks[l_ATData.MapID]);
            WorldDatabase.PExecute("UPDATE areatrigger SET spawnMask = %u WHERE guid = %u", l_SpawnMasks[l_ATData.MapID], l_Guid);
            l_ATData.SpawnMask = l_SpawnMasks[l_ATData.MapID];
        }

        /// Add to grid if not managed by the game event or pool system
        AddAreaTriggerToGrid(l_Guid, &l_ATData);

        if (!l_ATData.ZoneID || !l_ATData.AreaID)
        {
            uint32 l_ZoneID = 0;
            uint32 l_AreaID = 0;

            sMapMgr->GetZoneAndAreaId(l_ZoneID, l_AreaID, l_ATData.MapID, l_ATData.PosX, l_ATData.PosY, l_ATData.PosZ);

            l_ATData.ZoneID = l_ZoneID;
            l_ATData.AreaID = l_AreaID;

            WorldDatabase.PExecute("UPDATE areatrigger SET zoneId = %u, areaId = %u WHERE guid = %u", l_ZoneID, l_AreaID, l_Guid);
        }

        ++l_Count;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u areatrigger in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::AddAreaTriggerToGrid(uint32 const& p_Guid, AreaTriggerData const* p_Data)
{
    uint32 l_Mask = p_Data->SpawnMask;
    for (uint32 l_I = 0; l_Mask != 0; l_I++, l_Mask >>= 1)
    {
        if (l_Mask & 1)
        {
            CellCoord l_CellCoord = JadeCore::ComputeCellCoord(p_Data->PosX, p_Data->PosY);
            CellObjectGuids& l_CellGuids = _mapObjectGuidsStore[MAKE_PAIR32(p_Data->MapID, l_I)][l_CellCoord.GetId()];
            l_CellGuids.areatriggers.insert(p_Guid);
        }
    }
}

CreatureTemplate const* ObjectMgr::GetCreatureTemplate(uint32 p_Entry)
{
    if (!m_CreatureTemplateStore.count(p_Entry))
        return nullptr;

    return &m_CreatureTemplateStore[p_Entry];
}

CreatureTemplate const* ObjectMgr::GetRandomTemplate(CreatureType p_Type)
{
    CreatureTemplate const* l_Template = nullptr;

    for (uint32 i = 0; i < m_LastCreatureTemplateEntry; i++)
    {
        if (l_Template != nullptr && l_Template->type == p_Type)
            return l_Template;
        else
            l_Template = GetCreatureTemplate(urand(0, m_LastCreatureTemplateEntry));
    }

    return l_Template;
}

VehicleAccessoryList const* ObjectMgr::GetVehicleAccessoryList(Vehicle* veh) const
{
    VehicleAccessoryContainer::const_iterator itr = _vehicleTemplateAccessoryStore.find(veh->GetCreatureEntry());
    if (itr != _vehicleTemplateAccessoryStore.end())
        return &itr->second;

    return NULL;
}

 void ObjectMgr::LoadResearchSiteZones()
{
    uint32 l_OldMSTime = getMSTime();

    auto result = WorldDatabase.Query("SELECT site_id, zone_id, branch_id FROM archaeology_zones");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 archaeology zones. DB table `archaeology_zones` is empty.");
        return;
    }

    do
    {
        auto fields = result->Fetch();

        auto site_id = fields[0].GetUInt32();
        auto zone_id = fields[1].GetUInt32();

        auto itr = sResearchSiteDataMap.find(site_id);
        if (itr == sResearchSiteDataMap.end())
        {
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, "DB table `archaeology_zones` has data for nonexistant site id %u", site_id);
            continue;
        }

        auto& _data = itr->second;
        _data.zone = zone_id;

        for (auto area : sAreaTableStore)
        {
            if (area->ParentAreaID == zone_id)
            {
                _data.level = area->ExplorationLevel;
                break;
            }
        }

    } while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu Archeology research site zones in %u ms.", (unsigned long)sResearchSiteDataMap.size(), GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadResearchSiteLoot()
{
    QueryResult result = WorldDatabase.Query("SELECT site_id, x, y, z, race FROM research_loot");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Archeology site loot. DB table `research_loot` is empty.");
        return;
    }

    uint32 l_OldMSTime = getMSTime();

    do
    {
        ResearchLootEntry dg;
        {
            Field* fields = result->Fetch();

            dg.id = uint16(fields[0].GetUInt32());
            dg.x = fields[1].GetFloat();
            dg.y = fields[2].GetFloat();
            dg.z = fields[3].GetFloat();
            dg.ResearchBranchID = fields[4].GetUInt32();
        }

        _researchLoot.push_back(dg);
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %lu Archeology research site loot in %u ms.", (unsigned long)_researchLoot.size(), GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::RestructCreatureGUID(uint32 nbLigneToRestruct)
{
    QueryResult result = WorldDatabase.PQuery("SELECT guid FROM creature ORDER BY guid DESC LIMIT %u;", nbLigneToRestruct);

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Soucis lors du select de la fonction 'restructCreatureGUID' (nombre de lignes : %u)", nbLigneToRestruct);
        return;
    }

    std::vector<uint32> guidList;

    do
    {
        Field* fields = result->Fetch();
        guidList.push_back(fields[0].GetUInt32());
    }
    while (result->NextRow());

    uint32 upperGUID = 0;
    uint32 lowerGUID = 0;

    std::map<uint32, uint32> newGUIDList;

    for (int32 i = guidList.size() - 2; i >= 0; --i)
    {
        upperGUID = guidList[i];
        lowerGUID = guidList[i + 1];

        if (upperGUID != lowerGUID + 1)
        {
            newGUIDList[upperGUID] = lowerGUID + 1;
            guidList[i] = lowerGUID + 1;
        }
    }

    uint32 oldGUID   = 0;
    uint32 newGUID   = 0;

    SQLTransaction worldTrans = WorldDatabase.BeginTransaction();

    for (std::map<uint32, uint32>::iterator Itr = newGUIDList.begin(); Itr != newGUIDList.end(); ++Itr)
    {
        oldGUID = Itr->first;
        newGUID = Itr->second;

        // World Database
        std::ostringstream creature_ss;
        creature_ss << "UPDATE creature SET guid = "                        << newGUID << " WHERE guid = "          << oldGUID << "; ";
        worldTrans->Append(creature_ss.str().c_str());

        /*std::ostringstream formation1_ss;
        formation1_ss << "UPDATE creature_formations SET leaderGUID = "     << newGUID << " WHERE leaderGUID = "    << oldGUID << "; ";
        worldTrans->Append(formation1_ss.str().c_str());

        std::ostringstream formation2_ss;
        formation2_ss << "UPDATE creature_formations SET memberGUID = "     << newGUID << " WHERE memberGUID = "    << oldGUID << "; ";
        worldTrans->Append(formation2_ss.str().c_str());

        std::ostringstream game_event_ss;
        game_event_ss << "UPDATE game_event_creature SET guid = "           << newGUID << " WHERE guid = "          << oldGUID << "; ";
        worldTrans->Append(game_event_ss.str().c_str());

        std::ostringstream pool_ss;
        pool_ss << "UPDATE pool_creature SET guid = "                       << newGUID << " WHERE guid = "          << oldGUID << "; ";
        worldTrans->Append(pool_ss.str().c_str());*/
    }

    std::ostringstream increment_ss;
    // Le dernier newGUID est le plus haut
    increment_ss << "ALTER TABLE creature AUTO_INCREMENT = "                << newGUID << ";";
    worldTrans->Append(increment_ss.str().c_str());

    WorldDatabase.DirectCommitTransaction(worldTrans);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "%u lignes ont ete restructuree.", nbLigneToRestruct);
}

void ObjectMgr::RestructGameObjectGUID(uint32 nbLigneToRestruct)
{
    QueryResult result = WorldDatabase.PQuery("SELECT guid FROM gameobject ORDER BY guid DESC LIMIT %u;", nbLigneToRestruct);

    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Soucis lors du select de la fonction 'RestructGameObjectGUID' (nombre de lignes : %u)", nbLigneToRestruct);
        return;
    }

    std::vector<uint32> guidList;

    do
    {
        Field* fields = result->Fetch();
        guidList.push_back(fields[0].GetUInt32());
    }
    while (result->NextRow());

    uint32 upperGUID = 0;
    uint32 lowerGUID = 0;

    std::map<uint32, uint32> newGUIDList;

    for (int32 i = guidList.size() - 2; i >= 0; --i)
    {
        upperGUID = guidList[i];
        lowerGUID = guidList[i + 1];

        if (upperGUID != lowerGUID + 1)
        {
            newGUIDList[upperGUID] = lowerGUID + 1;
            guidList[i] = lowerGUID + 1;
        }
    }

    uint32 oldGUID   = 0;
    uint32 newGUID   = 0;

    SQLTransaction worldTrans = WorldDatabase.BeginTransaction();

    for (std::map<uint32, uint32>::iterator Itr = newGUIDList.begin(); Itr != newGUIDList.end(); ++Itr)
    {
        oldGUID = Itr->first;
        newGUID = Itr->second;

        // World Database
        std::ostringstream gameobject_ss;
        gameobject_ss << "UPDATE gameobject SET guid = "            << newGUID << " WHERE guid = " << oldGUID << "; ";
        worldTrans->Append(gameobject_ss.str().c_str());

        std::ostringstream game_event_ss;
        game_event_ss << "UPDATE game_event_gameobject SET guid = " << newGUID << " WHERE guid = " << oldGUID << "; ";
        worldTrans->Append(game_event_ss.str().c_str());

        std::ostringstream pool_ss;
        pool_ss << "UPDATE pool_gameobject SET guid = "             << newGUID << " WHERE guid = " << oldGUID << "; ";
        worldTrans->Append(pool_ss.str().c_str());
    }

    std::ostringstream increment_ss;
    // Le dernier newGUID est le plus haut
    increment_ss << "ALTER TABLE creature AUTO_INCREMENT = "        << newGUID << ";";
    worldTrans->Append(increment_ss.str().c_str());

    WorldDatabase.DirectCommitTransaction(worldTrans);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, "%u lignes ont ete restructuree.", nbLigneToRestruct);
}
#ifndef CROSS

void ObjectMgr::LoadGuildChallengeRewardInfo()
{
    uint32 oldMSTime = getMSTime();
    QueryResult result = WorldDatabase.Query("SELECT Type, Experience, Gold, Gold2, Count FROM guild_challenge_reward");
    if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 guild challenge reward data.");
        return;
    }

    _challengeRewardData.reserve(result->GetRowCount());

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 type = fields[0].GetUInt32();
        if (type >= ChallengeMax)
        {
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> guild_challenge_reward has unknown challenge type %u, skip.", type);
            continue;
        }

        GuildChallengeReward reward;
        {
            reward.Experience = fields[1].GetUInt32();
            reward.Gold = fields[2].GetUInt32();
            reward.Gold2 = fields[3].GetUInt32();
            reward.ChallengeCount = fields[4].GetUInt32();
        }

        _challengeRewardData.push_back(reward);
        ++count;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u guild challenge reward data in %u ms.", count, GetMSTimeDiffToNow(oldMSTime));
}

#endif /* not CROSS */

void ObjectMgr::LoadCharacterTemplateData()
{
    uint32 l_OldMSTime = getMSTime();
    QueryResult l_Result = WorldDatabase.Query("SELECT id, class, name, description, level, money, alianceX, alianceY, alianceZ, alianceO, alianceMap, hordeX, hordeY, hordeZ, hordeO, hordeMap, hordeDefaultRace, allianceDefaultRace FROM character_template WHERE disabled = 0");
    uint32 l_Count = 0;

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 character templates.");
        return;
    }

    do
    {
        Field* l_Fields = l_Result->Fetch();
        uint32 l_ID = l_Fields[0].GetUInt32();

        CharacterTemplate* l_CharacterTemplate = new CharacterTemplate();
        {
            l_CharacterTemplate->m_ID = l_ID;
            l_CharacterTemplate->m_PlayerClass = l_Fields[1].GetUInt8();
            l_CharacterTemplate->m_Name = l_Fields[2].GetString();
            l_CharacterTemplate->m_Description = l_Fields[3].GetString();
            l_CharacterTemplate->m_Level = l_Fields[4].GetUInt8();
            l_CharacterTemplate->m_Money = l_Fields[5].GetUInt64();
            l_CharacterTemplate->m_AliancePos.Relocate(l_Fields[6].GetFloat(), l_Fields[7].GetFloat(), l_Fields[8].GetFloat(), l_Fields[9].GetFloat());
            l_CharacterTemplate->m_AlianceMapID = l_Fields[10].GetInt16();
            l_CharacterTemplate->m_HordePos.Relocate(l_Fields[11].GetFloat(), l_Fields[12].GetFloat(), l_Fields[13].GetFloat(), l_Fields[14].GetFloat());
            l_CharacterTemplate->m_HordeMapID = l_Fields[15].GetInt16();
            l_CharacterTemplate->m_HordeDefaultRace = l_Fields[16].GetUInt8();
            l_CharacterTemplate->m_AllianceDefaultRace = l_Fields[17].GetUInt8();
        }

        if (!sChrClassesStore.LookupEntry(l_CharacterTemplate->m_PlayerClass))
        {
            sLog->outError(LOG_FILTER_SQL, "Template %u defined in `character_template` with class %u does not exists, skipped.", l_ID, l_CharacterTemplate->m_PlayerClass);
            delete l_CharacterTemplate;
            continue;
        }

        if (!l_ID)
        {
            sLog->outError(LOG_FILTER_SQL, "Template %u defined in `character_template` cannot have null index, skipped.", l_ID);
            delete l_CharacterTemplate;
            continue;
        }

        if (!l_CharacterTemplate->m_Level)
            l_CharacterTemplate->m_Level = 1;

        QueryResult l_ItemResult = WorldDatabase.PQuery("SELECT itemID, faction, count, type FROM character_template_item WHERE id = %i OR id = %i", l_ID, MAX_CLASSES);
        if (l_ItemResult)
        {
            do
            {
                Field* l_ItemFields = l_ItemResult->Fetch();

                CharacterTemplate::TemplateItem l_TemplateItem;
                {
                    l_TemplateItem.m_ItemID = l_ItemFields[0].GetUInt32();
                    l_TemplateItem.m_Faction = l_ItemFields[1].GetUInt32();
                    l_TemplateItem.m_Count = l_ItemFields[2].GetUInt32();
                    l_TemplateItem.m_Type = l_ItemFields[3].GetUInt8();
                }

                ItemTemplate const* l_ItemTemplate = GetItemTemplate(l_TemplateItem.m_ItemID);

                if (!l_ItemTemplate)
                {
                    sLog->outError(LOG_FILTER_SQL, "ItemID %u defined in `character_template_item` does not exist, ignoring.", l_TemplateItem.m_ItemID);
                    continue;
                }

                /// Premade items can't be sell
                if (sWorld->getBoolConfig(CONFIG_FUN_ENABLE))
                    const_cast<ItemTemplate*>(l_ItemTemplate)->FlagsCu |= int32(ItemFlagsCustom::ITEM_FLAGS_CU_CANT_BE_SELL);

                l_CharacterTemplate->m_TemplateItems.push_back(l_TemplateItem);
            }
            while (l_ItemResult->NextRow());
        }

        QueryResult l_SpellsResult = WorldDatabase.PQuery("SELECT spellId FROM character_template_spell WHERE id = %i OR id = %i", l_ID, MAX_CLASSES);
        if (l_SpellsResult)
        {
            do
            {
                Field* l_SpellFields = l_SpellsResult->Fetch();
                uint32 l_SpellID = l_SpellFields[0].GetUInt32();

                if (!sSpellMgr->GetSpellInfo(l_SpellID))
                {
                    sLog->outError(LOG_FILTER_SQL, "SpellId %u defined in `character_template_spell` does not exist, ignoring.", l_SpellID);
                    continue;
                }

                l_CharacterTemplate->m_SpellIDs.push_back(l_SpellID);
            }
            while (l_SpellsResult->NextRow());
        }

        // NYI - will fix later
        QueryResult l_ReputationResult = WorldDatabase.PQuery("SELECT factionID, reputation FROM character_template_reputation WHERE id = %i OR id = %i", l_ID, MAX_CLASSES);
        if (l_ReputationResult)
        {
            do
            {
                Field* l_ReputationFields = l_ReputationResult->Fetch();
                uint32 l_FactionID = l_ReputationFields[0].GetUInt32();

                if (!sFactionStore.LookupEntry(l_FactionID))
                {
                    sLog->outError(LOG_FILTER_SQL, "FactionID %u defined in `character_template_reputation` does not exist, ignoring.", l_FactionID);
                    continue;
                }

                CharacterTemplate::TemplateFaction l_TemplateReputation;
                l_TemplateReputation.m_FactionID = l_FactionID;
                l_TemplateReputation.m_Reputaion = l_ReputationFields[1].GetUInt32();
                l_CharacterTemplate->m_TemplateFactions.push_back(l_TemplateReputation);
            }
            while (l_ReputationResult->NextRow());
        }

        m_CharacterTemplatesStore[l_ID] = l_CharacterTemplate;
        l_Count++;
    }

    while (l_Result->NextRow());
    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u character templates %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

CharacterTemplate const* ObjectMgr::GetCharacterTemplate(uint32 p_ID) const
{
    CharacterTemplates::const_iterator l_Iter = m_CharacterTemplatesStore.find(p_ID);
    return l_Iter != m_CharacterTemplatesStore.end() ? l_Iter->second : nullptr;
}

void ObjectMgr::LoadQuestObjectives()
{
    uint32 l_OldMSTime = getMSTime();

    m_questObjectiveLookup.clear();
    for (auto& l_Quest : _questTemplates)
    {
        l_Quest.second->QuestObjectives.clear();

        for (int l_I = 0; l_I < QUEST_OBJECTIVE_TYPE_END; ++l_I)
            l_Quest.second->QuestObjecitveTypeCount[l_I] = 0;
    }

    QueryResult l_Result = WorldDatabase.Query("SELECT `ID`,`QuestID`,`Type`,`Index`,`ObjectID`,`Amount`,`Flags`,`ProgressBarWeight`,`Description`,`VisualEffects` FROM quest_template_objective ORDER BY QuestID ASC");
    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Quest Objectives. DB table `quest_objective` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32  l_ObjectiveID                = l_Fields[0].GetUInt32();
        uint32  l_ObjectiveQuestId           = l_Fields[1].GetUInt32();
        uint8   l_ObjectiveType              = l_Fields[2].GetUInt8();
        uint8   l_ObjectiveIndex             = l_Fields[3].GetUInt8();
        uint32  l_ObjectiveObjectID          = l_Fields[4].GetInt32();
        int32   l_ObjectiveAmount            = l_Fields[5].GetInt32();
        uint32  l_ObjectiveFlags             = l_Fields[6].GetUInt32();
        float   l_ObjectiveProgressBarWeight = l_Fields[7].GetFloat();
        std::string l_ObjectiveDescription   = l_Fields[8].GetString();
        std::string l_ObjectiveVisualEffects = l_Fields[9].GetString();

        if (_questTemplates.find(l_ObjectiveQuestId) == _questTemplates.end())
        {
            sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has non existant Quest Id %u! Skipping.", l_ObjectiveID, l_ObjectiveQuestId);
            m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
            continue;
        }

        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_QUEST, l_ObjectiveQuestId, NULL))
        {
            m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
            continue;
        }

        if (l_ObjectiveType >= QUEST_OBJECTIVE_TYPE_END)
        {
            sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has invalid type %u! Skipping.", l_ObjectiveID, l_ObjectiveType);
            m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
            continue;
        }

        Quest* l_Quest = _questTemplates.find(l_ObjectiveQuestId)->second;

        switch (l_ObjectiveType)
        {
            case QUEST_OBJECTIVE_TYPE_NPC:
            case QUEST_OBJECTIVE_TYPE_NPC_INTERACT:
            case QUEST_OBJECTIVE_TYPE_PET_TRAINER_DEFEAT:
            {
                if (!GetCreatureTemplate(l_ObjectiveObjectID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has non existant Creature Id %u! Skipping.", l_ObjectiveID, l_ObjectiveObjectID);
                    continue;
                }

                if (l_ObjectiveAmount <= 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has valid Creature Id %u but amount %u is invalid! Skipping.", l_ObjectiveID, l_ObjectiveObjectID, l_ObjectiveAmount);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                l_Quest->SetSpecialFlag(QUEST_SPECIAL_FLAGS_KILL | QUEST_SPECIAL_FLAGS_CAST);
                if (CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(l_ObjectiveObjectID))
                {
                    if (cInfo->expansion >= EXPANSION_WARLORDS_OF_DRAENOR)
                        const_cast<CreatureTemplate*>(cInfo)->QuestPersonalLoot = true;
                }

                break;
            }
            case QUEST_OBJECTIVE_TYPE_ITEM:
            {
                if (!GetItemTemplate(l_ObjectiveObjectID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has non existant Item Id %u! Skipping.", l_ObjectiveID, l_ObjectiveObjectID);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                if (l_ObjectiveAmount <= 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has valid Item Id %u but amount %u is invalid! Skipping.", l_ObjectiveID, l_ObjectiveObjectID, l_ObjectiveAmount);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                break;
            }
            case QUEST_OBJECTIVE_TYPE_GO:
            {
                if (!GetGameObjectTemplate(l_ObjectiveObjectID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has non existant GameObject Id %u! Skipping.", l_ObjectiveID, l_ObjectiveObjectID);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                if (l_ObjectiveAmount <= 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has valid GameObject Id %u but amount %u is invalid! Skipping.", l_ObjectiveID, l_ObjectiveObjectID, l_ObjectiveAmount);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                l_Quest->SetSpecialFlag(QUEST_SPECIAL_FLAGS_KILL | QUEST_SPECIAL_FLAGS_CAST);

                break;
            }
            case QUEST_OBJECTIVE_TYPE_CURRENCY:
            {
                if (!sCurrencyTypesStore.LookupEntry(l_ObjectiveObjectID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has non existant Currency Id %u! Skipping.", l_ObjectiveID, l_ObjectiveObjectID);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                break;
            }
            case QUEST_OBJECTIVE_TYPE_SPELL:
            {
                const SpellInfo* l_Spell = sSpellMgr->GetSpellInfo(l_ObjectiveObjectID);

                if (!l_Spell)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has non existant Spell Id %u! Skipping.", l_ObjectiveID, l_ObjectiveObjectID);
                    continue;
                }

                else if (!SpellMgr::IsSpellValid(l_Spell))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has invalid Spell Id %u! Skipping.", l_ObjectiveID, l_ObjectiveObjectID);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                if (l_ObjectiveAmount <= 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has valid Spell Id %u but amount %u is invalid! Skipping.", l_ObjectiveID, l_ObjectiveObjectID, l_ObjectiveAmount);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                break;
            }
            case QUEST_OBJECTIVE_TYPE_FACTION_MIN:
            case QUEST_OBJECTIVE_TYPE_FACTION_MAX:
            {
                if (!sFactionStore.LookupEntry(l_ObjectiveObjectID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has non existant Faction Id %u! Skipping.", l_ObjectiveID, l_ObjectiveObjectID);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                break;
            }
            case QUEST_OBJECTIVE_TYPE_MONEY:
            {
                if (l_Quest->GetQuestObjectiveCountType(l_ObjectiveType) >= 1)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u is invalid, Quest %u already has the max amount of Quest Objective type %u! Skipping.", l_ObjectiveID, l_ObjectiveQuestId, l_ObjectiveType);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                break;
            }
            case QUEST_OBJECTIVE_TYPE_PET_BATTLE_ELITE:
            {
                if (!sBattlePetSpeciesStore.LookupEntry(l_ObjectiveObjectID))
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has non existant Battle Pet Species %u! Skipping.", l_ObjectiveID, l_ObjectiveObjectID);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                ///if (!BattlePetSpeciesHasFlag(objectId, BATTLE_PET_FLAG_ELITE))
                ///{
                ///    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has invalid Battle Pet Species %u, it doesn't have BATTLE_PET_FLAG_ELITE flag! Skipping.", id, objectId);
                ///    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                ///    continue;
                ///}

                if (l_ObjectiveAmount <= 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has valid Item Id %u but amount %u is invalid! Skipping.", l_ObjectiveID, l_ObjectiveObjectID, l_ObjectiveAmount);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                break;
            }
            case QUEST_OBJECTIVE_TYPE_PET_BATTLE_PVP:
            {
                if (l_Quest->GetQuestObjectiveCountType(l_ObjectiveType) >= 1)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u is invalid, Quest %u already has the max amount of Quest Objective type %u! Skipping.", l_ObjectiveID, l_ObjectiveQuestId, l_ObjectiveType);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                if (l_ObjectiveAmount <= 0)
                {
                    sLog->outError(LOG_FILTER_SQL, "Quest Objective %u has invalid Pet Battle PvP win amount %u! Skipping.", l_ObjectiveID, l_ObjectiveAmount);
                    m_IgnoredQuestObjectives.push_back(l_ObjectiveID);
                    continue;
                }

                break;
            }
            case QUEST_OBJECTIVE_TYPE_CRITERIA_TREE:
                if (!sCriteriaTreeStore.LookupEntry(l_ObjectiveObjectID))
                    sLog->outError(LOG_FILTER_SQL, "Quest %u has not exist CriteriaTreeID: %u in ObjectID field ", l_Quest->GetQuestId(), l_ObjectiveObjectID);
                break;
            default:
                break;
        }

        if (m_questObjectiveLookup.find(l_ObjectiveID) == m_questObjectiveLookup.end())
            m_questObjectiveLookup.insert(std::make_pair(l_ObjectiveID, l_ObjectiveQuestId));

        QuestObjective l_Objective;
        l_Objective.ID                  = l_ObjectiveID;
        l_Objective.QuestID             = l_ObjectiveQuestId;
        l_Objective.Type                = l_ObjectiveType;
        l_Objective.Index               = l_ObjectiveIndex;
        l_Objective.ObjectID            = l_ObjectiveObjectID;
        l_Objective.Amount              = l_ObjectiveAmount;
        l_Objective.Flags               = l_ObjectiveFlags;
        l_Objective.Flags2              = 0;
        l_Objective.ProgressBarWeight   = l_ObjectiveProgressBarWeight;
        l_Objective.Description         = l_ObjectiveDescription;

        if (!l_ObjectiveVisualEffects.empty())
        {
            Tokenizer l_Specializations(l_ObjectiveVisualEffects, ' ');

            for (Tokenizer::const_iterator l_It = l_Specializations.begin(); l_It != l_Specializations.end(); ++l_It)
                l_Objective.VisualEffects.push_back(atol(*l_It));
        }

        l_Quest->QuestObjectives.push_back(l_Objective);
        m_QuestObjectiveByType[l_ObjectiveType].push_back(l_Objective);
        l_Quest->QuestObjecitveTypeCount[l_ObjectiveType]++;

        l_Count++;
    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Quest Objectives in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadQuestObjectiveLocales()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query("SELECT `ID`, `Locale`, `Description` FROM `locales_quest_template_objective` ORDER BY `id` ASC");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Quest Objective locale descriptions. DB table `locales_quest_objective` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 l_ObjectiveID = l_Fields[0].GetUInt32();
        uint8 l_Locale = l_Fields[1].GetUInt8();
        std::string l_Description = l_Fields[2].GetString();

        if (std::find(m_IgnoredQuestObjectives.begin(), m_IgnoredQuestObjectives.end(), l_ObjectiveID) != m_IgnoredQuestObjectives.end())
            continue;

        if (!QuestObjectiveExists(l_ObjectiveID))
        {
            sLog->outError(LOG_FILTER_SQL, "Quest Objective locale Id %u has invalid Quest Objective %u! Skipping.", l_Locale, l_ObjectiveID);
            continue;
        }

        if (l_Locale >= TOTAL_LOCALES)
        {
            sLog->outError(LOG_FILTER_SQL, "Locale Id %u for Quest Objective %u is invalid! Skipping.", l_Locale, l_ObjectiveID);
            continue;
        }

        AddLocaleString(l_Description, (LocaleConstant)l_Locale, m_questObjectiveLocaleStore[l_ObjectiveID].Description);
    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Quest Objective visual effects in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadFollowerQuests()
{
    const ObjectMgr::QuestMap & l_QuestTemplates = GetQuestTemplates();
    for (ObjectMgr::QuestMap::const_iterator l_It = l_QuestTemplates.begin(); l_It != l_QuestTemplates.end(); ++l_It)
    {
        Quest* l_Quest = l_It->second;

        uint32 l_SpellID = l_Quest->RewardSpellCast;

        if (!l_SpellID)
            continue;

        const SpellInfo* l_Info = sSpellMgr->GetSpellInfo(l_SpellID);

        if (!l_Info)
            continue;

        if (l_Info->Effects[EFFECT_0].Effect != SPELL_EFFECT_ADD_GARRISON_FOLLOWER)
            continue;

        FollowerQuests.push_back(l_Quest->Id);
    }
}

void ObjectMgr::LoadQuestForItem()
{
    const ObjectMgr::QuestMap & l_QuestTemplates = GetQuestTemplates();
    for (ObjectMgr::QuestMap::const_iterator l_It = l_QuestTemplates.begin(); l_It != l_QuestTemplates.end(); ++l_It)
    {
        Quest* l_Quest = l_It->second;

        for (auto l_Objective : l_Quest->QuestObjectives)
        {
            if (l_Objective.Type != QUEST_OBJECTIVE_TYPE_ITEM)
                continue;

            QuestForItem[l_Objective.ObjectID].push_back(std::pair<uint32, uint8>(l_Quest->Id, l_Objective.Index));
        }
    }
}

QuestObjectiveLocale const* ObjectMgr::GetQuestObjectiveLocale(uint32 objectiveId) const
{
    QuestObjectiveLocaleContainer::const_iterator l_It = m_questObjectiveLocaleStore.find(objectiveId);

    if (l_It == m_questObjectiveLocaleStore.end())
        return NULL;

    return &l_It->second;
}

bool ObjectMgr::QuestObjectiveExists(uint32 objectiveId) const
{
    if (m_questObjectiveLookup.find(objectiveId) == m_questObjectiveLookup.end())
        return false;

    return true;
}

uint32 ObjectMgr::GetQuestObjectiveQuestId(uint32 objectiveId) const
{
    QuestObjectiveLookupMap::const_iterator l_It = m_questObjectiveLookup.find(objectiveId);

    if (l_It == m_questObjectiveLookup.end())
        return 0;

    return l_It->second;
}

CreatureGroupSizeStat const* ObjectMgr::GetCreatureGroupSizeStat(uint32 p_Entry, uint32 p_Difficulty) const
{
    auto l_DifficultyIterator = m_CreatureGroupSizeStore.find(p_Entry);
    if (l_DifficultyIterator == m_CreatureGroupSizeStore.end())
        return nullptr;

    auto l_StatsIterator = l_DifficultyIterator->second.find(p_Difficulty);
    if (l_StatsIterator == l_DifficultyIterator->second.end())
        return nullptr;

    return &l_StatsIterator->second;
}

float const* ObjectMgr::GetCreatureScalingPct(uint32 p_Size) const
{
    auto l_Iter = m_CreatureScalingPctStore.find(p_Size);
    if (l_Iter == m_CreatureScalingPctStore.end())
        return nullptr;

    return &l_Iter->second;
}

uint64 const* ObjectMgr::GetDiffHealthValue(uint32 p_Entry, uint32 p_DifficultyID) const
{
    auto l_DiffIter = m_CreatureDiffHealthStore.find(p_Entry);
    if (l_DiffIter == m_CreatureDiffHealthStore.end())
        return nullptr;

    auto l_HealthIter = l_DiffIter->second.find(p_DifficultyID);
    if (l_HealthIter == l_DiffIter->second.end())
        return nullptr;

    return &l_HealthIter->second;
}

void ObjectMgr::LoadItemBonusGroup()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query("SELECT id, bonus, flags FROM item_bonus_group");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Item Bonus Group. DB table `item_bonus_group` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields  = l_Result->Fetch();
        uint32 l_GroupID = l_Fields[0].GetUInt32();

        ItemBonusGroup l_ItemBonusGroup;
        Tokenizer l_Tokens(l_Fields[1].GetCString(), ',');

        for (uint32 l_I = 0; l_I < l_Tokens.size(); ++l_I)
            l_ItemBonusGroup.Bonuses.push_back((uint32)atoi(l_Tokens[l_I]));

        l_ItemBonusGroup.Flags = l_Fields[2].GetUInt32();

        m_ItemBonusGroupStore.insert(std::make_pair(l_GroupID, l_ItemBonusGroup));

        l_Count++;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u item bonus group in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadItemBonusGroupLinked()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.Query("SELECT itemEntry, itemBonusGroup FROM item_bonus_group_linked");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Item Bonus Group Linked. DB table `item_bonus_group_linked` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();
        uint32 l_ItemEntry      = l_Fields[0].GetUInt32();
        uint32 l_ItemBonusGroup = l_Fields[1].GetUInt32();

        if (_itemTemplateStore.find(l_ItemEntry) == _itemTemplateStore.end())
            continue;

        ItemTemplate& l_ItemTemplate = _itemTemplateStore[l_ItemEntry];
        l_ItemTemplate.m_ItemBonusGroups.push_back(l_ItemBonusGroup);

        l_Count++;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u item bonus group linked in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadSpellInvalid()
{
    uint32 l_OldMSTime = getMSTime();
    m_SpellInvalid.clear();

    QueryResult l_Result = WorldDatabase.Query("SELECT spellid FROM spell_invalid");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Spell Invalid. DB table `spell_invalid` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields    = l_Result->Fetch();
        uint32 l_SpellID   = l_Fields[0].GetUInt32();

        m_SpellInvalid.push_back(l_SpellID);

        l_Count++;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Spell Invalid in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadSpellStolen()
{
    uint32 l_OldMSTime = getMSTime();
    m_SpellStolen.clear();

    QueryResult l_Result = WorldDatabase.Query("SELECT spell_id FROM spell_stolen");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Spell Stolen. DB table `spell_stolen` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields = l_Result->Fetch();
        uint32 l_SpellID = l_Fields[0].GetUInt32();

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(l_SpellID);
        if (!spellInfo)
        {
            sLog->outError(LOG_FILTER_SQL, "SpellStolen: spell (spell_id:%d) does not exist in `Spell.db2`.", l_SpellID);
            continue;
        }

        m_SpellStolen.push_back(l_SpellID);

        l_Count++;
    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Spell Stolen in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadDisabledEncounters()
{
    uint32 l_OldMSTime = getMSTime();

    m_DisabledEncounters.clear();

    QueryResult l_Result = WorldDatabase.Query("SELECT EncounterID, DifficultyID FROM instance_disabled_rankings");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 disabled ranking. DB table `instance_disabled_rankings` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields         = l_Result->Fetch();
        uint32 l_EncounterID    = l_Fields[0].GetUInt32();
        uint32 l_DifficultyID   = l_Fields[1].GetUInt32();

        m_DisabledEncounters.insert(std::make_pair(l_EncounterID, l_DifficultyID));

        l_Count++;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u disabled ranking in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadInstanceWhitelists()
{
    uint32 l_OldMSTime = getMSTime();

    m_InstanceWhitelist.clear();

    QueryResult l_Result = CharacterDatabase.Query("SELECT mapId, guildId FROM instance_whitelist");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 instance whitelists. DB table `instance_whitelist` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        Field* l_Fields     = l_Result->Fetch();
        uint32 l_MapID      = l_Fields[0].GetUInt32();
        uint64 l_GuildId    = l_Fields[1].GetUInt64();

        auto& l_IDSet = m_InstanceWhitelist[l_MapID];
        l_IDSet.insert(l_GuildId);

        l_Count++;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u instance whitelists in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadDisabledMaps()
{
    uint32 l_OldMSTime = getMSTime();

    m_DisabledMaps.clear();
    m_DisabledMapsItemContextOverride.clear();

    QueryResult l_Result = WorldDatabase.Query("SELECT MapID, DifficultyID, ItemContext FROM disabled_maps");

    if (!l_Result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 disabled maps. DB table `disabled_maps` is empty.");
        return;
    }

    uint32 l_Count = 0;
    do
    {
        uint8 l_Index = 0;

        Field* l_Fields         = l_Result->Fetch();
        uint32 l_MapID          = l_Fields[l_Index++].GetUInt32();
        uint32 l_DifficultyID   = l_Fields[l_Index++].GetUInt32();
        uint8 l_ItemContext     = l_Fields[l_Index++].GetUInt8();

        if (m_DisabledMaps.find(l_MapID) != m_DisabledMaps.end())
            m_DisabledMaps[l_MapID] |= (1 << l_DifficultyID);
        else
            m_DisabledMaps[l_MapID] = (1 << l_DifficultyID);

        auto& l_DifficultyContexts = m_DisabledMapsItemContextOverride[l_MapID];

        l_DifficultyContexts[l_DifficultyID] = l_ItemContext;

        l_Count++;
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u disabled maps in %u ms.", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadWorldVisibleDistance()
{
    uint32 oldMSTime = getMSTime();

    for (uint8 i = 0; i < TYPE_VISIBLE_MAX; ++i)
        sVisibleDistance[i].clear();

    QueryResult result = WorldDatabase.Query("SELECT `type`, `id`, `distance` FROM `world_visible_distance`");

    if (!result)
        return;

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 type = fields[0].GetUInt32();
        uint32 id = fields[1].GetUInt32();
        float distance = fields[2].GetFloat();
        if (type > TYPE_VISIBLE_MAX)
        {
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded world visible distance type %u error", type);
            continue;
        }

        sVisibleDistance[type][id] = distance;

        count++;
    }
    while (result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u world visible distance in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

std::vector<TempSummonData> const* ObjectMgr::GetSummonGroup(uint32 summonerId, SummonerType summonerType, uint8 group) const
{
    TempSummonGroupKey groupKey = TempSummonGroupKey(summonerId, summonerType, group);

    TempSummonDataContainer::const_iterator itr = _tempSummonDataStore.find(groupKey);
    if (itr != _tempSummonDataStore.end())
        return &itr->second;

    return NULL;
}

void ObjectMgr::LoadCustomInstanceZones()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.PQuery("SELECT zone, customZone, maxPlayerCount, radius, x, y, z FROM custom_instance_zone");
    if (!l_Result)
        return;

    uint32 l_Count = 0;

    do
    {
        Field* l_Fields = l_Result->Fetch();
        l_Count++;

        CustomInstanceZone l_CustomInstanceZone;
        l_CustomInstanceZone.RealZoneID     = l_Fields[0].GetUInt32();
        l_CustomInstanceZone.CustomZoneID   = l_Fields[1].GetUInt32();
        l_CustomInstanceZone.MaxPlayerCount = l_Fields[2].GetUInt32();
        l_CustomInstanceZone.Radius         = l_Fields[3].GetFloat();
        l_CustomInstanceZone.Position       = Position(l_Fields[4].GetFloat(), l_Fields[5].GetFloat(), l_Fields[6].GetFloat());

        m_CustomInstanceZones[l_CustomInstanceZone.RealZoneID].push_back(l_CustomInstanceZone);
    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u custom instance zone in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadForbiddenRegex()
{
    for (uint32 l_I = 0; l_I < eChannelType::ChannelTypeMax; l_I++)
        m_ForbiddenRegex[l_I].clear();

    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.PQuery("SELECT regex, channel FROM regex_forbidden");
    if (!l_Result)
        return;

    uint32 l_Count = 0;

    do
    {
        uint8 l_I = 0;
        Field* l_Fields = l_Result->Fetch();
        l_Count++;

        std::regex l_Regex(l_Fields[l_I++].GetString(), std::regex_constants::icase);
        uint32 l_Channel = l_Fields[l_I++].GetUInt32();

        if (l_Channel < eChannelType::ChannelTypeMax)
            m_ForbiddenRegex[l_Channel].push_back(l_Regex);
        else
            sLog->outError(LOG_FILTER_SQL, "ChannelType %u defined in `regex_forbidden` does not exist, ignoring.", l_Channel);

    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Forbidden Regex in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadCosmetics()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = LoginDatabase.PQuery("SELECT type, cosmetic_id, price, expansion, active, locale_enUS, locale_frFR, locale_esES, locale_ruRU, locale_ptPT, locale_itIT FROM cosmetics");
    if (!l_Result)
        return;

    uint32 l_Count = 0;

    do
    {
        uint8 l_I = 0;
        Field* l_Fields = l_Result->Fetch();
        l_Count++;

        CosmeticEntry l_Entry;

        l_Entry.Type        = static_cast<eCosmetics>(l_Fields[l_I++].GetUInt8());
        l_Entry.ID          = l_Fields[l_I++].GetUInt32();
        l_Entry.Cost        = l_Fields[l_I++].GetUInt32();
        l_Entry.Expansion   = l_Fields[l_I++].GetUInt32();
        l_Entry.Active      = l_Fields[l_I++].GetUInt8();
        l_Entry.Locale_enUS = l_Fields[l_I++].GetString();
        l_Entry.Locale_frFR = l_Fields[l_I++].GetString();
        l_Entry.Locale_esES = l_Fields[l_I++].GetString();
        l_Entry.Locale_ruRU = l_Fields[l_I++].GetString();
        l_Entry.Locale_ptPT = l_Fields[l_I++].GetString();
        l_Entry.Locale_itIT = l_Fields[l_I++].GetString();

        if (l_Entry.Type > eCosmetics::MaxCosmetic)
            continue;

        if (l_Entry.Expansion > Expansion::EXPANSION_LEGION)
            continue;

        m_Cosmetics[l_Entry.Type].push_back(l_Entry);
        m_CosmeticsID[l_Entry.Type].push_back(l_Entry.ID);
        m_CosmeticsByEntry[MAKE_PAIR64(l_Entry.Type, l_Entry.ID)] = l_Entry;

    } while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u cosmetics in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

std::string ObjectMgr::GetCosmeticName(Player* p_Player, eCosmetics p_Cosmetic, uint32 p_Id)
{
    LocaleConstant l_Locale = p_Player->GetSession()->GetSessionDbLocaleIndex();
    for (CosmeticEntry l_Entry : m_Cosmetics[p_Cosmetic])
    {
        if (l_Entry.ID == p_Id)
        {
            switch (l_Locale)
            {
                case LocaleConstant::LOCALE_frFR:
                    return l_Entry.Locale_frFR;
                case LocaleConstant::LOCALE_esES:
                    return l_Entry.Locale_esES;
                case LocaleConstant::LOCALE_ruRU:
                    return l_Entry.Locale_ruRU;
                case LocaleConstant::LOCALE_ptPT:
                    return l_Entry.Locale_ptPT;
                case LocaleConstant::LOCALE_itIT:
                    return l_Entry.Locale_itIT;
                case LocaleConstant::LOCALE_enUS:
                default:
                    return l_Entry.Locale_enUS;
            }
        }
    }

    return "Str Not Found";
}

static ScenarioData* g_DefaultScenarioData = nullptr;

ScenarioData const* ObjectMgr::GetScenarioOnMap(uint32 mapId, uint32 difficultyID /*= 0*/, uint32 team /*= 0*/, uint32 pl_class /*= 0*/, uint32 p_DungeonDataID /*= 0*/, uint32 p_ZoneID) const
{
    if (!g_DefaultScenarioData)
    {
        g_DefaultScenarioData = new ScenarioData();
        g_DefaultScenarioData->Class = 0;
        g_DefaultScenarioData->DifficultyID = 0;
        g_DefaultScenarioData->MapID = 0;
        g_DefaultScenarioData->ScenarioID = 0;
        g_DefaultScenarioData->Team = 0;
        g_DefaultScenarioData->ZoneID = 0;
    }

    auto const& itr = _scenarioDataList.find(mapId);
    if (itr == _scenarioDataList.end())
        return nullptr;

    bool l_HaveZoneFilter = false;

    bool find = false;
    ScenarioData const* data = nullptr;
    for (auto const& v : itr->second)
    {
        if (v->ZoneID != 0 && p_ZoneID != v->ZoneID)
        {
            if (v->MapID == mapId)
                l_HaveZoneFilter = true;

            continue;
        }

        if (!find)
        {
            /// Always try to find the lowest DifficultyID
            if (!data || data->DifficultyID < v->DifficultyID)
                data = v;
        }

        if (v->DifficultyID && v->DifficultyID == difficultyID)
        {
            if (v->Team)
            {
                if (v->Team == team)
                {
                    if (v->Class)
                    {
                        if (v->Class == pl_class)
                        {
                            data = v;
                            break;
                        }
                        else
                            continue;
                    }
                    else
                    {
                        data = v;
                        break;
                    }
                }
                else
                    continue;
            }
            else
            {
                if (v->Class)
                {
                    if (v->Class == pl_class)
                    {
                        data = v;
                        break;
                    }
                }
                else
                {
                    /// Ugly but needed for Upper Return to Karazhan
                    if (p_DungeonDataID == 1475 && v->ScenarioID != 1308 && v->ScenarioID != 1289)
                        continue;

                    data = v;
                    find = true;
                }
            }
        }

        if (!v->DifficultyID)
        {
            if (v->Team && v->Team == team)
            {
                if (v->Class)
                {
                    if (v->Class == pl_class)
                    {
                        data = v;
                        break;
                    }
                }
                else
                {
                    data = v;
                    find = true;
                }
            }
            if (!v->Team)
            {
                if (v->Class)
                {
                    if (v->Class == pl_class)
                    {
                        data = v;
                        break;
                    }
                }
                else
                {
                    data = v;
                    find = true;
                }
            }
        }
    }

    /// Prevent use one default entry of LFGDungeons.db2 if there is multiple scenario id linked to the same map.
    if (l_HaveZoneFilter && !data)
        return g_DefaultScenarioData;

    return data;
}

void ObjectMgr::LoadScenarioData()
{
    uint32 l_OldMSTime = getMSTime();

    _scenarioData.clear();
    _scenarioDataList.clear();

    //                                                0            1           2              3         4        5
    QueryResult result = WorldDatabase.Query("SELECT `ScenarioID`, `MapID`, `ZoneID`,  `DifficultyID`, `Team`, `Class` FROM `scenario_data`");
    if (result)
    {
        uint32 counter = 0;
        do
        {
            Field* fields = result->Fetch();

            uint8 i = 0;
            uint32 ScenarioID = fields[i++].GetUInt32();
            ScenarioData& data = _scenarioData[ScenarioID];
            data.MapID        = fields[i++].GetUInt32();
            data.ZoneID       = fields[i++].GetUInt32();
            data.DifficultyID = fields[i++].GetUInt32();
            data.Team         = fields[i++].GetUInt32();
            data.Class        = fields[i++].GetUInt32();
            data.ScenarioID   = ScenarioID;
            _scenarioDataList[data.MapID].push_back(&data);
            ++counter;
        }
        while (result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Scenario data in %u ms.", counter, GetMSTimeDiffToNow(l_OldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Scenario data. DB table `scenario_data` is empty.");
}

void ObjectMgr::LoadScenarioSpellData()
{
    uint32 l_OldMSTime = getMSTime();

    _scenarioDataSpellStep.clear();

    //                                                  0            1        2
    QueryResult result = WorldDatabase.Query("SELECT `scenarioId`, `step`, `spell` from `scenario_step_spells`;");
    if (result)
    {
        uint32 counter = 0;
        do
        {
            Field* fields = result->Fetch();

            uint8 i = 0;
            uint32 ScenarioID = fields[i++].GetUInt32();
            ScenarioSpellData data;
            data.StepId = fields[i++].GetUInt32();
            data.Spells = fields[i++].GetUInt32();
            _scenarioDataSpellStep[ScenarioID].push_back(data);
            ++counter;
        }
        while (result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Scenario Step Spell Data in %u ms.", counter, GetMSTimeDiffToNow(l_OldMSTime));
    }
    else
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Scenario Step Spell data. DB table `scenario_step_spells` is empty.");
}

void ObjectMgr::LoadPlayerChoices()
{
    uint32 l_OldMSTime = getMSTime();
    m_PlayerChoices.clear();

    QueryResult l_Choices = WorldDatabase.Query("SELECT ChoiceID, Question, UiTextureKitId FROM playerchoice");

    if (!l_Choices)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 player choices. DB table `playerchoice` is empty.");
        return;
    }

    uint32 l_ResponseCount = 0;
    uint32 l_RewardCount = 0;
    uint32 l_ItemRewardCount = 0;
    uint32 l_CurrencyRewardCount = 0;
    uint32 l_FactionRewardCount = 0;

    do
    {
        Field* l_Fields = l_Choices->Fetch();

        int32 l_ChoiceID = l_Fields[0].GetInt32();

        PlayerChoice& l_Choice = m_PlayerChoices[l_ChoiceID];
        l_Choice.ChoiceId = l_ChoiceID;
        l_Choice.Question = l_Fields[1].GetString();
        l_Choice.UiTextureKitID = l_Fields[2].GetUInt32();

    } while (l_Choices->NextRow());

    if (QueryResult l_Responses = WorldDatabase.Query("SELECT ChoiceId, ResponseId, ChoiceArtFileId, Header, Answer, Description, Confirmation FROM playerchoice_response ORDER BY `Index` ASC"))
    {
        do
        {
            Field* l_Fields = l_Responses->Fetch();

            int32 l_ChoiceID      = l_Fields[0].GetInt32();
            int32 l_ResponseID    = l_Fields[1].GetInt32();

            auto l_ChoiceItr = m_PlayerChoices.find(l_ChoiceID);
            if (l_ChoiceItr == m_PlayerChoices.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            PlayerChoice* l_Choice = &(l_ChoiceItr->second);
            if (!l_Choice)
            {
                continue;
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
            }

            l_Choice->Responses.emplace_back();

            PlayerChoiceResponse& l_Response = l_Choice->Responses.back();
            l_Response.ResponseID         = l_ResponseID;
            l_Response.ChoiceArtFileID    = l_Fields[2].GetInt32();
            l_Response.Header             = l_Fields[3].GetString();
            l_Response.Answer             = l_Fields[4].GetString();
            l_Response.Description        = l_Fields[5].GetString();
            l_Response.Confirmation       = l_Fields[6].GetString();
            ++l_ResponseCount;
        } while (l_Responses->NextRow());
    }

    if (QueryResult l_Rewards = WorldDatabase.Query("SELECT ChoiceId, ResponseId, TitleId, PackageId, SkillLineId, SkillPointCount, ArenaPointCount, HonorPointCount, Money, Xp, `Unique` FROM playerchoice_response_reward"))
    {
        do
        {
            Field* l_Fields = l_Rewards->Fetch();

            int32 l_ChoiceID      = l_Fields[0].GetInt32();
            int32 l_ResponseID    = l_Fields[1].GetInt32();

            auto l_ChoiceItr = m_PlayerChoices.find(l_ChoiceID);
            if (l_ChoiceItr == m_PlayerChoices.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            PlayerChoice* l_Choice = &(l_ChoiceItr->second);
            if (!l_Choice)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            auto l_responseItr = std::find_if(l_Choice->Responses.begin(), l_Choice->Responses.end(), [l_ResponseID](PlayerChoiceResponse const& p_PlayerChoiceResponse) -> bool
            {
                return p_PlayerChoiceResponse.ResponseID == l_ResponseID;
            });

            if (l_responseItr == l_Choice->Responses.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward` references non-existing ResponseId: %d for ChoiceId %d, skipped", l_ResponseID, l_ChoiceID);
                continue;
            }

            l_responseItr->Reward.emplace();

            PlayerChoiceResponseReward* l_Reward = l_responseItr->Reward.get_ptr();
            l_Reward->TitleID          = l_Fields[2].GetInt32();
            l_Reward->PackageID        = l_Fields[3].GetInt32();
            l_Reward->SkillLineID      = l_Fields[4].GetInt32();
            l_Reward->SkillPointCount  = l_Fields[5].GetUInt32();
            l_Reward->ArenaPointCount  = l_Fields[6].GetUInt32();
            l_Reward->HonorPointCount  = l_Fields[7].GetUInt32();
            l_Reward->Money            = l_Fields[8].GetUInt64();
            l_Reward->Xp               = l_Fields[9].GetUInt32();
            l_Reward->Unique           = l_Fields[10].GetBool();
            ++l_RewardCount;

            if (l_Reward->TitleID && !sCharTitlesStore.LookupEntry(l_Reward->TitleID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward` references non-existing Title %d for ChoiceId %d, ResponseId: %d, set to 0",
                    l_Reward->TitleID, l_ChoiceID, l_ResponseID);
                l_Reward->TitleID = 0;
            }

            if (l_Reward->PackageID && !GetQuestPackageItems(l_Reward->PackageID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward` references non-existing QuestPackage %d for ChoiceId %d, ResponseId: %d, set to 0",
                    l_Reward->PackageID, l_ChoiceID, l_ResponseID);
                l_Reward->PackageID = 0;
            }

            if (l_Reward->SkillLineID && !sSkillLineStore.LookupEntry(l_Reward->SkillLineID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward` references non-existing SkillLine %d for ChoiceId %d, ResponseId: %d, set to 0",
                    l_Reward->SkillLineID, l_ChoiceID, l_ResponseID);
                l_Reward->SkillLineID = 0;
                l_Reward->SkillPointCount = 0;
            }

        } while (l_Rewards->NextRow());
    }

    if (QueryResult l_Rewards = WorldDatabase.Query("SELECT ChoiceId, ResponseId, ItemId, BonusListIDs, Quantity FROM playerchoice_response_reward_item ORDER BY `Index` ASC"))
    {
        do
        {
            Field* l_Fields = l_Rewards->Fetch();

            int32 l_ChoiceID = l_Fields[0].GetInt32();
            int32 l_ResponseID = l_Fields[1].GetInt32();
            uint32 l_ItemID = l_Fields[2].GetUInt32();
            Tokenizer l_BonusListIDsTok(l_Fields[3].GetString(), ' ');
            std::vector<int32> l_BonusListIds;
            for (char const* l_Token : l_BonusListIDsTok)
                l_BonusListIds.push_back(int32(atol(l_Token)));
            int32 l_Quantity = l_Fields[4].GetInt32();

            auto l_ChoiceItr = m_PlayerChoices.find(l_ChoiceID);
            if (l_ChoiceItr == m_PlayerChoices.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_item` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            PlayerChoice* l_Choice = &(l_ChoiceItr->second);
            if (!l_Choice)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_item` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            auto l_ResponseItr = std::find_if(l_Choice->Responses.begin(), l_Choice->Responses.end(), [l_ResponseID](PlayerChoiceResponse const& playerChoiceResponse) -> bool
            {
                return playerChoiceResponse.ResponseID == l_ResponseID;
            });

            if (l_ResponseItr == l_Choice->Responses.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_item` references non-existing ResponseId: %d for ChoiceId %d, skipped", l_ResponseID, l_ChoiceID);
                continue;
            }

            if (!l_ResponseItr->Reward)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_item` references non-existing player choice reward for ChoiceId %d, ResponseId: %d, skipped",
                    l_ChoiceID, l_ResponseID);
                continue;
            }

            if (!GetItemTemplate(l_ItemID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_item` references non-existing item %u for ChoiceId %d, ResponseId: %d, skipped", l_ItemID, l_ChoiceID, l_ResponseID);
                continue;
            }

            l_ResponseItr->Reward->Items.emplace_back(l_ItemID, std::move(l_BonusListIds), l_Quantity);

        } while (l_Rewards->NextRow());
    }

    if (QueryResult l_Rewards = WorldDatabase.Query("SELECT ChoiceId, ResponseId, CurrencyId, Quantity FROM playerchoice_response_reward_currency ORDER BY `Index` ASC"))
    {
        do
        {
            Field* l_Fields = l_Rewards->Fetch();

            int32 l_ChoiceID = l_Fields[0].GetInt32();
            int32 l_ResponseID = l_Fields[1].GetInt32();
            uint32 l_CurrencyID = l_Fields[2].GetUInt32();
            int32 l_Quantity = l_Fields[3].GetInt32();

            auto l_ChoiceItr = m_PlayerChoices.find(l_ChoiceID);
            if (l_ChoiceItr == m_PlayerChoices.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_currency` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            PlayerChoice* l_Choice = &(l_ChoiceItr->second);
            if (!l_Choice)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_currency` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            auto l_ResponseItr = std::find_if(l_Choice->Responses.begin(), l_Choice->Responses.end(), [l_ResponseID](PlayerChoiceResponse const& playerChoiceResponse) -> bool
            {
                return playerChoiceResponse.ResponseID == l_ResponseID;
            });

            if (l_ResponseItr == l_Choice->Responses.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_currency` references non-existing ResponseId: %d for ChoiceId %d, skipped", l_ResponseID, l_ChoiceID);
                continue;
            }

            if (!l_ResponseItr->Reward)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_currency` references non-existing player choice reward for ChoiceId %d, ResponseId: %d, skipped",
                    l_ChoiceID, l_ResponseID);
                continue;
            }

            if (!sCurrencyTypesStore.LookupEntry(l_CurrencyID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_currency` references non-existing currency %u for ChoiceId %d, ResponseId: %d, skipped",
                    l_CurrencyID, l_ChoiceID, l_ResponseID);
                continue;
            }

            l_ResponseItr->Reward->Currency.emplace_back(l_CurrencyID, l_Quantity);

        } while (l_Rewards->NextRow());
    }

    if (QueryResult l_Rewards = WorldDatabase.Query("SELECT ChoiceId, ResponseId, FactionId, Quantity FROM playerchoice_response_reward_faction ORDER BY `Index` ASC"))
    {
        do
        {
            Field* l_Fields = l_Rewards->Fetch();

            int32 l_ChoiceID = l_Fields[0].GetInt32();
            int32 l_ResponseID = l_Fields[1].GetInt32();
            uint32 l_FactionID = l_Fields[2].GetUInt32();
            int32 l_Quantity = l_Fields[3].GetInt32();

            auto l_ChoiceItr = m_PlayerChoices.find(l_ChoiceID);
            if (l_ChoiceItr == m_PlayerChoices.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_faction` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            PlayerChoice* l_Choice = &(l_ChoiceItr->second);
            if (!l_Choice)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_faction` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            auto l_ResponseItr = std::find_if(l_Choice->Responses.begin(), l_Choice->Responses.end(), [l_ResponseID](PlayerChoiceResponse const& playerChoiceResponse) -> bool
            {
                return playerChoiceResponse.ResponseID == l_ResponseID;
            });

            if (l_ResponseItr == l_Choice->Responses.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_faction` references non-existing ResponseId: %d for ChoiceId %d, skipped", l_ResponseID, l_ChoiceID);
                continue;
            }

            if (!l_ResponseItr->Reward)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_faction` references non-existing player choice reward for ChoiceId %d, ResponseId: %d, skipped",
                    l_ChoiceID, l_ResponseID);
                continue;
            }

            if (!sFactionStore.LookupEntry(l_FactionID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_faction` references non-existing faction %u for ChoiceId %d, ResponseId: %d, skipped",
                    l_FactionID, l_ChoiceID, l_ResponseID);
                continue;
            }

            l_ResponseItr->Reward->Faction.emplace_back(l_FactionID, l_Quantity);

        } while (l_Rewards->NextRow());
    }

    if (QueryResult l_Rewards = WorldDatabase.Query("SELECT ChoiceId, ResponseId, SpellId FROM playerchoice_response_reward_spellcast ORDER BY `Index` ASC"))
    {
        do
        {
            Field* l_Fields     = l_Rewards->Fetch();

            int32 l_ChoiceID    = l_Fields[0].GetInt32();
            int32 l_ResponseID  = l_Fields[1].GetInt32();
            uint32 l_SpellID    = l_Fields[2].GetUInt32();

            auto l_ChoiceItr = m_PlayerChoices.find(l_ChoiceID);
            if (l_ChoiceItr == m_PlayerChoices.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_spellcast` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            PlayerChoice* l_Choice = &(l_ChoiceItr->second);
            if (!l_Choice)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_spellcast` references non-existing ChoiceId: %d (ResponseId: %d), skipped", l_ChoiceID, l_ResponseID);
                continue;
            }

            auto l_ResponseItr = std::find_if(l_Choice->Responses.begin(), l_Choice->Responses.end(), [l_ResponseID](PlayerChoiceResponse const& playerChoiceResponse) -> bool
            {
                return playerChoiceResponse.ResponseID == l_ResponseID;
            });

            if (l_ResponseItr == l_Choice->Responses.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_spellcast` references non-existing ResponseId: %d for ChoiceId %d, skipped", l_ResponseID, l_ChoiceID);
                continue;
            }

            if (!l_ResponseItr->Reward)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_spellcast` references non-existing player choice reward for ChoiceId %d, ResponseId: %d, skipped",
                    l_ChoiceID, l_ResponseID);
                continue;
            }

            if (!sSpellMgr->GetSpellInfo(l_SpellID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_response_reward_spellcast` references non-existing l_SpellID %u for ChoiceId %d, ResponseId: %d, skipped",
                    l_SpellID, l_ChoiceID, l_ResponseID);
                continue;
            }

            l_ResponseItr->Reward->Spells.emplace_back(l_SpellID);

        } while (l_Rewards->NextRow());
    }


    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded " SZFMTD " player choices, %u responses, %u rewards, %u item rewards, %u currency rewards and %u faction rewards in %u ms.",
    m_PlayerChoices.size(), l_ResponseCount, l_RewardCount, l_ItemRewardCount, l_CurrencyRewardCount, l_FactionRewardCount, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadPlayerChoicesLocale()
{
    uint32 l_OldMSTime = getMSTime();

    // need for reload case
    m_PlayerChoiceLocales.clear();

    //                                               0         1       2
    if (QueryResult l_Result = WorldDatabase.Query("SELECT ChoiceId, locale, Question FROM playerchoice_locale"))
    {
        do
        {
            Field* l_Fields = l_Result->Fetch();

            uint32 l_ChoiceID         = l_Fields[0].GetUInt32();
            std::string l_LocaleName  = l_Fields[1].GetString();
            std::string l_Question    = l_Fields[2].GetString();

            if (!GetPlayerChoice(l_ChoiceID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_locale` references non-existing ChoiceId: %d for locale %s, skipped", l_ChoiceID, l_LocaleName.c_str());
                continue;
            }

            LocaleConstant l_Locale = GetLocaleByName(l_LocaleName);
            if (l_Locale == LOCALE_enUS)
                continue;

            PlayerChoiceLocale& l_Data = m_PlayerChoiceLocales[l_ChoiceID];
            AddLocaleString(l_Fields[2].GetString(), l_Locale, l_Data.Question);
        } while (l_Result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded " SZFMTD " Player Choice locale strings in %u ms", m_PlayerChoiceLocales.size(), GetMSTimeDiffToNow(l_OldMSTime));
    }

    l_OldMSTime = getMSTime();

    //                                   0         1           2       3       4       5            6
    if (QueryResult l_Result = WorldDatabase.Query("SELECT ChoiceID, ResponseID, locale, Header, Answer, Description, Confirmation FROM playerchoice_response_locale"))
    {
        std::size_t l_Count = 0;
        do
        {
            Field* l_Fields = l_Result->Fetch();

            int32 l_ChoiceID          = l_Fields[0].GetInt32();
            int32 l_ResponseID        = l_Fields[1].GetInt32();
            std::string l_LocaleName  = l_Fields[2].GetString();

            auto l_Itr = m_PlayerChoiceLocales.find(l_ChoiceID);
            if (l_Itr == m_PlayerChoiceLocales.end())
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_locale` references non-existing ChoiceId: %d for ResponseId %d locale %s, skipped",
                    l_ChoiceID, l_ResponseID, l_LocaleName.c_str());
                continue;
            }

            PlayerChoice const* l_PlayerChoice = GetPlayerChoice(l_ResponseID);
            if (!l_PlayerChoice->GetResponse(l_ResponseID))
            {
                sLog->outError(LOG_FILTER_SQL, "Table `playerchoice_locale` references non-existing ResponseId: %d for ChoiceId %d locale %s, skipped",
                    l_ResponseID, l_ChoiceID, l_LocaleName.c_str());
                continue;
            }

            LocaleConstant l_Locale = GetLocaleByName(l_LocaleName);
            if (l_Locale == LOCALE_enUS)
                continue;

            PlayerChoiceResponseLocale& l_Data = l_Itr->second.Responses[l_ResponseID];
            AddLocaleString(l_Fields[3].GetString(), l_Locale, l_Data.Header);
            AddLocaleString(l_Fields[4].GetString(), l_Locale, l_Data.Answer);
            AddLocaleString(l_Fields[5].GetString(), l_Locale, l_Data.Description);
            AddLocaleString(l_Fields[6].GetString(), l_Locale, l_Data.Confirmation);
            ++l_Count;

        } while (l_Result->NextRow());

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded " SZFMTD " Player Choice Response locale strings in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
    }
}

PlayerChoice const* ObjectMgr::GetPlayerChoice(int32 p_ChoiceID) const
{
    auto l_Itr = m_PlayerChoices.find(p_ChoiceID);
    if (l_Itr != m_PlayerChoices.end())
        return &l_Itr->second;

    return nullptr;
}

void ObjectMgr::LoadAddons()
{
    m_AddonFunctions.clear();
    m_LocaleAddonFunctions.clear();

    std::string l_AddonPath = ConfigMgr::GetStringDefault("FirestormAddon.path", "firestorm_addon.lua");

    FILE* l_File = std::fopen(l_AddonPath.c_str(), "r");
    if (!l_File)
        return;

    std::vector<char> l_Buffer(1024);
    while (fread(&l_Buffer[l_Buffer.size() - 1024], sizeof l_Buffer[0], 1024, l_File) != 0)
        l_Buffer.resize(l_Buffer.size() + 1024);

    fclose(l_File);

    std::string l_FileString(l_Buffer.begin(), l_Buffer.end());

    std::string l_Delimiter = "--[FUNCTION]";
    size_t l_Pos = 0;
    while ((l_Pos = l_FileString.find(l_Delimiter)) != std::string::npos)
    {
        m_AddonFunctions.push_back(l_FileString.substr(0, l_Pos));
        l_FileString.erase(0, l_Pos + l_Delimiter.length());
    }

    for (std::string& l_Function : m_AddonFunctions)
    {
        /// Locales
        if (l_Function.find("InitLocales = function()") != std::string::npos)
        {
            size_t l_StartPos;
            size_t l_EndPos;
            for (uint8 l_I = LocaleConstant::LOCALE_enUS; l_I <= LocaleConstant::LOCALE_itIT; ++l_I)
            {
                LocaleConstant l_Locale = static_cast<LocaleConstant>(l_I);
                std::string l_LocalizedFunction = "";
                std::string l_FunctionCopy = l_Function;
                std::string l_Delimiter = "= ";

                while ((l_StartPos = l_FunctionCopy.find(l_Delimiter)) != std::string::npos)
                {
                    l_LocalizedFunction += l_FunctionCopy.substr(0, l_StartPos + l_Delimiter.length());
                    l_FunctionCopy.erase(0, l_StartPos + l_Delimiter.length());

                    if ((l_EndPos = l_FunctionCopy.find("\n")) != std::string::npos)
                    {
                        uint32 l_TrinityStringID = std::atoi(l_FunctionCopy.substr(0, l_EndPos).c_str());
                        if (l_TrinityStringID)
                        {
                            l_LocalizedFunction += "\"";
                            l_LocalizedFunction += sObjectMgr->GetTrinityString(l_TrinityStringID, l_Locale);
                            l_LocalizedFunction += "\" ";
                        }
                        else
                        {
                            l_LocalizedFunction += l_FunctionCopy.substr(0, l_EndPos);
                            l_LocalizedFunction += " ";
                        }
                        l_FunctionCopy.erase(0, l_EndPos + 1);
                    }
                }

                l_LocalizedFunction += l_FunctionCopy.substr(0, l_FunctionCopy.size() - 1);

                std::replace(l_LocalizedFunction.begin(), l_LocalizedFunction.end(), '\n', ' ');
                std::replace(l_LocalizedFunction.begin(), l_LocalizedFunction.end(), '\t', ' ');

                m_LocaleAddonFunctions[l_Locale].push_back(l_LocalizedFunction);
            }

            l_Function = "";
            continue;
        }

        std::replace(l_Function.begin(), l_Function.end(), '\n', ' ');
        std::replace(l_Function.begin(), l_Function.end(), '\t', ' ');
    }
}

std::vector<std::string> const& ObjectMgr::GetAddonFunctions()
{
    return m_AddonFunctions;
}

std::vector<std::string> const& ObjectMgr::GetLocalizedAddonFunctions(LocaleConstant p_Locale)
{
    if (m_LocaleAddonFunctions.find(p_Locale) != m_LocaleAddonFunctions.end())
        return m_LocaleAddonFunctions[p_Locale];

    return m_LocaleAddonFunctions[LocaleConstant::LOCALE_enUS];
}

void ObjectMgr::AddLocalizedAddonFunction(LocaleConstant p_Locale, std::string p_Function)
{
    m_LocaleAddonFunctions[p_Locale].push_back(p_Function);
}

std::string LuaEscape(const char* l_Str)
{
    std::string l_Ret;

    for (uint32 l_I = 0; l_I < strlen(l_Str); ++l_I)
    {
        switch (l_Str[l_I])
        {
            case '\n':
                l_Ret += "\\\n";
                continue;
            case '\"':
                l_Ret += "\\\"";
                continue;
            case '\t':
                l_Ret += "\\\t";
                continue;
            case '\\':
                l_Ret += "\\\\";
                continue;
            case '|':
                l_Ret += "\\124";
                continue;
            default:
                l_Ret += l_Str[l_I];
                continue;
        }
    }

    uint32 l_Size = strlen(l_Str);

    while (l_Size > 2 && l_Str[l_Size - 1] == '\n')
    {
        l_Ret.pop_back();
        l_Ret.pop_back();
        l_Size -= 2;
    }

    return l_Ret;
}

void ObjectMgr::LoadLiveChangelog()
{
    if (!sWorld->getBoolConfig(CONFIG_WEB_DATABASE_ENABLE))
        return;

    m_ChangelogAddonFunctions.clear();

    uint32 l_OldMSTime = getMSTime();

    if (QueryResult l_Result = WebDatabase.Query("SELECT CreationDate, Title, Description FROM live_changelog WHERE expansion = 6"))
    {
        uint32 l_LastDate = 0;

        std::vector<ChangelogItem> l_ChangelogItems;

        do
        {
            Field* l_Fields = l_Result->Fetch();

            ChangelogItem l_Item;

            uint32 l_Timestamp        = l_Fields[0].GetUInt32();

            l_Item.Date         = l_Timestamp / TimeConstants::DAY;
            l_Item.Title        = l_Fields[1].GetString();
            l_Item.Description  = l_Fields[2].GetString();

            l_LastDate = std::max(l_LastDate, l_Item.Date);
            l_ChangelogItems.push_back(l_Item);

        } while (l_Result->NextRow());

        m_LastChangelogDate = l_LastDate;

        if (!l_ChangelogItems.empty())
        {
            std::vector<std::string> l_ChangelogAddonFunctionsTemp;

            for (ChangelogItem const& l_Item : l_ChangelogItems)
            {
                if (l_Item.Date != l_LastDate)
                    continue;

                std::string l_Function = "AddChangelogItem(\"";
                l_Function += LuaEscape(l_Item.Title.c_str());
                l_Function += "\", \"";
                l_Function += LuaEscape(l_Item.Description.c_str());
                l_Function += "\")";

                l_ChangelogAddonFunctionsTemp.push_back(l_Function);
            }

            std::sort(l_ChangelogAddonFunctionsTemp.begin(), l_ChangelogAddonFunctionsTemp.end());
            for (int32 l_I = l_ChangelogAddonFunctionsTemp.size() - 1; l_I >= 0; --l_I)
                m_ChangelogAddonFunctions.push_back(l_ChangelogAddonFunctionsTemp[l_I]);

            time_t l_Date(l_LastDate * TimeConstants::DAY);

            std::tm * ptm = std::localtime(&l_Date);
            char l_Time[32];
            std::strftime(l_Time, 32, "%d/%b/%Y", ptm);
            std::string l_StrTime(l_Time);
            std::transform(l_StrTime.begin(), l_StrTime.end(), l_StrTime.begin(), ::toupper);

            std::string l_Function("GenerateSplashFrame(\"");
            l_Function += l_StrTime;
            l_Function += "\")";

            m_ChangelogAddonFunctions.push_back(l_Function);
        }

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded " SZFMTD " changelog items in %u ms", m_ChangelogAddonFunctions.size(), GetMSTimeDiffToNow(l_OldMSTime));
    }
}

void ObjectMgr::LoadNewsFeed()
{
    if (!sWorld->getBoolConfig(CONFIG_WEB_DATABASE_ENABLE))
        return;

    m_NewsFeedAddonFunctions.clear();

    uint32 l_OldMSTime = getMSTime();

    if (QueryResult l_Result = WebDatabase.Query("SELECT created_time, message, locale FROM facebook_feed ORDER BY created_time DESC"))
    {
        std::vector<NewsFeedItem> l_NewsFeedItems;

        do
        {
            Field* l_Fields = l_Result->Fetch();

            NewsFeedItem l_Item;

            l_Item.Timestamp    = l_Fields[0].GetUInt32();
            l_Item.Description  = l_Fields[1].GetString();
            l_Item.Locale       = static_cast<LocaleConstant>(l_Fields[2].GetUInt32());

            l_NewsFeedItems.push_back(l_Item);

        } while (l_Result->NextRow());

        uint32 l_LastTimestamp = 0;

        for (NewsFeedItem const& l_Item : l_NewsFeedItems)
        {
            if (!l_LastTimestamp)
                l_LastTimestamp = l_Item.Timestamp;

            if (l_Item.Timestamp < (l_LastTimestamp - 2 * TimeConstants::WEEK))
                continue;

            time_t l_Date(l_Item.Timestamp);

            std::tm * ptm = std::localtime(&l_Date);
            char l_Time[45];
            std::strftime(l_Time, 45, "%A, %B %d, %Y %H:%M", ptm);

            std::string l_StrDate = "|cff5b84d6";
            l_StrDate += l_Time;
            l_StrDate += "|r";

            std::string l_Function = "AddNewsFeedItem(\"";
            l_Function += LuaEscape(l_StrDate.c_str());
            l_Function += "\", \"";
            l_Function += LuaEscape(l_Item.Description.c_str());
            l_Function += "\")";

            m_NewsFeedAddonFunctions[l_Item.Locale].push_back(l_Function);
        }

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded " SZFMTD " news feed items in %u ms", m_NewsFeedAddonFunctions.size(), GetMSTimeDiffToNow(l_OldMSTime));
    }
}

void ObjectMgr::LoadIngameAlerts()
{
    m_AlertButtonFunctions.clear();

    uint32 l_OldMSTime = getMSTime();

    if (QueryResult l_Result = WorldDatabase.Query("SELECT Locale, Text FROM ingame_alert"))
    {
        std::vector<std::pair<uint32, std::string>> l_Alerts;

        do
        {
            std::pair<uint32, std::string> l_Alert;

            Field* l_Fields = l_Result->Fetch();

            l_Alert.first   = l_Fields[0].GetUInt32();
            l_Alert.second  = l_Fields[1].GetString();

            l_Alerts.push_back(l_Alert);

        } while (l_Result->NextRow());

        for (auto const& l_Alert : l_Alerts)
        {
            std::string l_Function = "AddAlert(\"";
            l_Function += LuaEscape(l_Alert.second.c_str());
            l_Function += "\")";

            m_AlertButtonFunctions[static_cast<LocaleConstant>(l_Alert.first)].push_back(l_Function);
        }

        for (uint32 l_I = 0; l_I <= 10; ++l_I)
        {
            if (m_AlertButtonFunctions[static_cast<LocaleConstant>(l_I)].empty())
                m_AlertButtonFunctions[static_cast<LocaleConstant>(l_I)] = m_AlertButtonFunctions[LocaleConstant::LOCALE_enUS];
        }

        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded " SZFMTD " alerts in %u ms", m_AlertButtonFunctions.size(), GetMSTimeDiffToNow(l_OldMSTime));
    }
}

std::vector<std::string> const& ObjectMgr::GetLiveChangelogFunctions()
{
    return m_ChangelogAddonFunctions;
}

std::vector<std::string> const& ObjectMgr::GetNewsFeedFunctions(LocaleConstant p_Locale)
{
    if (m_NewsFeedAddonFunctions.find(p_Locale) != m_NewsFeedAddonFunctions.end())
        return m_NewsFeedAddonFunctions[p_Locale];

    return m_NewsFeedAddonFunctions[LocaleConstant::LOCALE_enUS];
}

std::vector<std::string> const& ObjectMgr::GetAlertButtonFunctions(LocaleConstant p_Locale)
{
    if (m_AlertButtonFunctions.find(p_Locale) != m_AlertButtonFunctions.end())
        return m_AlertButtonFunctions[p_Locale];

    return m_AlertButtonFunctions[LocaleConstant::LOCALE_enUS];
}

void ObjectMgr::LoadPvPSeasonReward()
{
    m_PvPSeasonRewards.clear();

    QueryResult l_Result = WorldDatabase.PQuery("SELECT Bracket, MinRating, AchievementId, TitleId, ItemId FROM pvp_season_previous_reward");
    if (!l_Result)
        return;

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint8 l_Bracket    = l_Fields[0].GetUInt8();
        uint32 l_MinRating = l_Fields[1].GetUInt32();

        PvpSeasonReward& l_Reward = m_PvPSeasonRewards[l_Bracket][l_MinRating];
        l_Reward.AchievementId    = l_Fields[2].GetUInt32();
        l_Reward.TitleId          = l_Fields[3].GetUInt32();
        l_Reward.ItemId           = l_Fields[4].GetUInt32();

    } while (l_Result->NextRow());
}

PvpSeasonReward const* ObjectMgr::GetPvpReward(uint8 p_Bracket, uint32 p_Rating)
{
    auto l_BracketRewardsItr = m_PvPSeasonRewards.find(p_Bracket);
    if (l_BracketRewardsItr == m_PvPSeasonRewards.end())
        return nullptr;

    /// Using reverse iterator to loop from the higher MinRating to the lower
    auto& l_BracketRewards = l_BracketRewardsItr->second;
    for (auto l_RewardItr = l_BracketRewards.rbegin(); l_RewardItr != l_BracketRewards.rend(); l_RewardItr++)
    {
        if (p_Rating >= l_RewardItr->first)
            return &l_RewardItr->second;
    }

    return nullptr;
}

void ObjectMgr::LoadShortcutAreas()
{
    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.PQuery("SELECT `mapId`, `polygonIndex`, `x`, `y` FROM `shortcut_areas` ORDER BY `mapId` ASC, `polygonIndex` ASC, `pointId` ASC");
    if (!l_Result)
        return;

    uint32 l_PolygonCount = 0;
    uint32 l_PointCount = 0;

    uint32 l_CurrentMapId           = 0;
    uint32 l_CurrentPolygonIndex    = 0;
    std::vector<G3D::Vector2> l_CurrentPolygon;

    std::function<void(uint32, std::vector<G3D::Vector2> const&, uint32) > l_AddPolygon = [&l_PolygonCount, &l_PointCount](uint32 p_MapId, std::vector<G3D::Vector2> const& p_Polygon, uint32 p_CurrentPolygonIndex)
    {
        if (p_Polygon.empty())
            return;

        if (p_Polygon.size() < 3)
        {
            sLog->outError(LOG_FILTER_SQL, "Trying to load polygon from less than 3 points, mapId : %u, polygonIndex : %u", p_MapId, p_CurrentPolygonIndex);
            return;
        }

        PathGenerator::AddShortcutArea(p_MapId, p_Polygon);
        l_PolygonCount++;
        l_PointCount += p_Polygon.size();
    };

    do
    {
        Field* l_Fields = l_Result->Fetch();

        uint32 l_MapId          = l_Fields[0].GetUInt32();
        uint32 l_PolygonIndex   = l_Fields[1].GetUInt32();

        if (l_MapId != l_CurrentMapId || l_PolygonIndex != l_CurrentPolygonIndex)
        {
            l_AddPolygon(l_CurrentMapId, l_CurrentPolygon, l_CurrentPolygonIndex);

            l_CurrentPolygon.clear();

            l_CurrentMapId = l_MapId;
            l_CurrentPolygonIndex = l_PolygonIndex;
        }

        float l_X               = l_Fields[2].GetFloat();
        float l_Y               = l_Fields[3].GetFloat();

        l_CurrentPolygon.push_back(G3D::Vector2(l_X, l_Y));

    } while (l_Result->NextRow());

    l_AddPolygon(l_CurrentMapId, l_CurrentPolygon, l_CurrentPolygonIndex);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u shortcuts areas (%u points) in %u ms", l_PolygonCount, l_PointCount, GetMSTimeDiffToNow(l_OldMSTime));
}

void ObjectMgr::LoadCosmeticEvents()
{
    m_CosmeticEvents.clear();

    uint32 l_OldMSTime = getMSTime();

    QueryResult l_Result = WorldDatabase.PQuery("SELECT `EventId`, `Index`, `Delay`, `NpcEntry`, `PathId`, `MovementType`, `Speed`, `Flags`, `DespawnTime`, `MoveTime`, `RandomDistMin`, `RandomDistMax`, `DifficultyMask` FROM cosmetic_events");
    if (!l_Result)
        return;

    uint32 l_Count = 0;

    do
    {
        uint8 l_I = 0;
        Field* l_Fields = l_Result->Fetch();
        l_Count++;

        CosmeticEventEntry l_Entry;

        l_Entry.EventId         = l_Fields[l_I++].GetUInt32();
        l_Entry.Index           = l_Fields[l_I++].GetUInt32();
        l_Entry.Delay           = l_Fields[l_I++].GetUInt32();
        l_Entry.NpcEntry        = l_Fields[l_I++].GetUInt32();
        l_Entry.PathId          = l_Fields[l_I++].GetUInt32();
        l_Entry.MovementType    = l_Fields[l_I++].GetUInt32();
        l_Entry.Speed           = l_Fields[l_I++].GetFloat();
        l_Entry.Flags           = l_Fields[l_I++].GetUInt32();
        l_Entry.DespawnTime     = l_Fields[l_I++].GetInt32();
        l_Entry.MoveTime        = l_Fields[l_I++].GetUInt32();
        l_Entry.RandomDistMin   = l_Fields[l_I++].GetUInt32();
        l_Entry.RandomDistMax   = l_Fields[l_I++].GetUInt32();
        l_Entry.DifficultyMask  = l_Fields[l_I++].GetUInt32();

        m_CosmeticEvents[MAKE_PAIR64(l_Entry.EventId, l_Entry.Index)].push_back(l_Entry);

    }
    while (l_Result->NextRow());

    l_Result = WorldDatabase.PQuery("SELECT `EventId`, `Index`, `FollowingNpc`, `Distance`, `Angle` FROM cosmetic_events_addons");
    if (!l_Result)
        return;

    l_OldMSTime = getMSTime();
    l_Count = 0;

    do
    {
        uint8 l_I = 0;
        Field* l_Fields = l_Result->Fetch();
        l_Count++;

        CosmeticEventAddonEntry l_Addon;

        uint32 l_EventId        = l_Fields[l_I++].GetUInt32();
        uint32 l_Index          = l_Fields[l_I++].GetUInt32();
        l_Addon.FollowingNpc    = l_Fields[l_I++].GetUInt32();
        l_Addon.Distance        = l_Fields[l_I++].GetFloat();
        l_Addon.Angle           = l_Fields[l_I++].GetFloat();

        auto l_Itr = m_CosmeticEvents.find(MAKE_PAIR64(l_EventId, l_Index));
        if (l_Itr == m_CosmeticEvents.end())
            continue;

        for (CosmeticEventEntry& l_EventEntry : l_Itr->second)
            l_EventEntry.Addons.push_back(l_Addon);

    }
    while (l_Result->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u cosmetics events addons in %u ms", l_Count, GetMSTimeDiffToNow(l_OldMSTime));
}

bool ObjectMgr::CheckFakeColorProtection(std::string const &p_Msg, ColorFakeProtectionTypes colorProtection) const
{
    if (p_Msg.empty()) ///< Doesn't check empty message
        return true;

    if (sWorld->getIntConfig(CONFIG_COLOR_FAKE_PROTECTION_ENABLED) == 0) ///< Disable all color protection
        return true;

    if ((sWorld->getIntConfig(CONFIG_COLOR_FAKE_PROTECTION_ENABLED) & colorProtection) == 0) ///< Disable color type
        return true;

    std::string l_StrName = p_Msg;
    stripLineInvisibleChars(l_StrName);

    if (strchr(l_StrName.c_str(), '|')) ///< Found "|" for fake color. Example "|cff"
        return false;

    if (!utf8IsValid(p_Msg))
        return false;

    return true;
}

std::set<Quest const*> const* ObjectMgr::GetQuestTask(uint32 areaId) const
{
    return JadeCore::Containers::MapGetValuePtr(_questAreaTaskStore, areaId);
}

bool ObjectMgr::IsAreaTaskQuest(uint32 questId) const
{
    return _questAreaTaskQuests.find(questId) != _questAreaTaskQuests.end();
}