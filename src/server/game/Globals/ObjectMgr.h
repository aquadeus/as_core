////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H

#include <regex>

#include "Log.h"
#include "Object.h"
#include "Bag.h"
#include "Creature.h"
#include "Player.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "TemporarySummon.h"
#include "Corpse.h"
#include "QuestDef.h"
#include "ItemPrototype.h"
#include "NPCHandler.h"
#include "DatabaseEnv.h"
#include "Mail.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include "VehicleDefines.h"
#include "Common.h"
#include "ConditionMgr.h"
#include <functional>
#include "PhaseMgr.h"
#include <ace/Thread_Mutex.h>
#include <Battleground.h>
#include "Interfaces/Interface_GarrisonSite.hpp"

class Item;
class PhaseMgr;

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push, N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

enum eChannelType
{
    ChannelWorld,
    ChannelWhisper,

    ChannelTypeMax,
    ChannelTypeSize
};

struct PageText
{
    std::string Text;
    uint16 NextPageID;
    uint32 m_PlayerConditionID;
    uint8 m_Flags;
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

using MultimapBounds_uint32 = std::pair<std::multimap<uint32, uint32>::const_iterator, std::multimap<uint32, uint32>::const_iterator>;

typedef std::map<uint32, PageText> PageTextContainer;

typedef std::unordered_map<uint16, InstanceTemplate> InstanceTemplateContainer;

struct GameTele
{
    float  position_x;
    float  position_y;
    float  position_z;
    float  orientation;
    uint32 mapId;
    std::string name;
    std::wstring wnameLow;
};

typedef std::unordered_map<uint32, GameTele > GameTeleContainer;

enum ScriptsType
{
    SCRIPTS_FIRST = 1,

    SCRIPTS_QUEST_END = SCRIPTS_FIRST,
    SCRIPTS_QUEST_START,
    SCRIPTS_SPELL,
    SCRIPTS_GAMEOBJECT,
    SCRIPTS_EVENT,
    SCRIPTS_WAYPOINT,

    SCRIPTS_LAST
};

enum eScriptFlags
{
    // Talk Flags
    SF_TALK_USE_PLAYER          = 0x1,

    // Emote flags
    SF_EMOTE_USE_STATE          = 0x1,

    // TeleportTo flags
    SF_TELEPORT_USE_CREATURE    = 0x1,

    // KillCredit flags
    SF_KILLCREDIT_REWARD_GROUP  = 0x1,

    // RemoveAura flags
    SF_REMOVEAURA_REVERSE       = 0x1,

    // CastSpell flags
    SF_CASTSPELL_SOURCE_TO_TARGET = 0,
    SF_CASTSPELL_SOURCE_TO_SOURCE = 1,
    SF_CASTSPELL_TARGET_TO_TARGET = 2,
    SF_CASTSPELL_TARGET_TO_SOURCE = 3,
    SF_CASTSPELL_SEARCH_CREATURE  = 4,
    SF_CASTSPELL_TRIGGERED      = 0x1,

    // PlaySound flags
    SF_PLAYSOUND_TARGET_PLAYER  = 0x1,
    SF_PLAYSOUND_DISTANCE_SOUND = 0x2,

    // Orientation flags
    SF_ORIENTATION_FACE_TARGET  = 0x1,
};

struct ScriptInfo
{
    ScriptsType type;
    uint32 id;
    uint32 delay;
    ScriptCommands command;

    union
    {
        struct
        {
            uint32 nData[3];
            float  fData[4];
        } Raw;

        struct                      // SCRIPT_COMMAND_TALK (0)
        {
            uint32 ChatType;        // datalong
            uint32 Flags;           // datalong2
            int32  TextID;          // dataint
        } Talk;

        struct                      // SCRIPT_COMMAND_EMOTE (1)
        {
            uint32 EmoteID;         // datalong
            uint32 Flags;           // datalong2
        } Emote;

        struct                      // SCRIPT_COMMAND_FIELD_SET (2)
        {
            uint32 FieldID;         // datalong
            uint32 FieldValue;      // datalong2
        } FieldSet;

        struct                      // SCRIPT_COMMAND_MOVE_TO (3)
        {
            uint32 Unused1;         // datalong
            uint32 TravelTime;      // datalong2
            int32  Unused2;         // dataint

            float DestX;
            float DestY;
            float DestZ;
        } MoveTo;

        struct                      // SCRIPT_COMMAND_FLAG_SET (4)
                                    // SCRIPT_COMMAND_FLAG_REMOVE (5)
        {
            uint32 FieldID;         // datalong
            uint32 FieldValue;      // datalong2
        } FlagToggle;

        struct                      // SCRIPT_COMMAND_TELEPORT_TO (6)
        {
            uint32 MapID;           // datalong
            uint32 Flags;           // datalong2
            int32  Unused1;         // dataint

            float DestX;
            float DestY;
            float DestZ;
            float Orientation;
        } TeleportTo;

        struct                      // SCRIPT_COMMAND_QUEST_EXPLORED (7)
        {
            uint32 QuestID;         // datalong
            uint32 Distance;        // datalong2
        } QuestExplored;

        struct                      // SCRIPT_COMMAND_KILL_CREDIT (8)
        {
            uint32 CreatureEntry;   // datalong
            uint32 Flags;           // datalong2
        } KillCredit;

        struct                      // SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (9)
        {
            uint32 GOGuid;          // datalong
            uint32 DespawnDelay;    // datalong2
        } RespawnGameobject;

        struct                      // SCRIPT_COMMAND_TEMP_SUMMON_CREATURE (10)
        {
            uint32 CreatureEntry;   // datalong
            uint32 DespawnDelay;    // datalong2
            int32  Unused1;         // dataint

            float PosX;
            float PosY;
            float PosZ;
            float Orientation;
        } TempSummonCreature;

        struct                      // SCRIPT_COMMAND_CLOSE_DOOR (12)
                                    // SCRIPT_COMMAND_OPEN_DOOR (11)
        {
            uint32 GOGuid;          // datalong
            uint32 ResetDelay;      // datalong2
        } ToggleDoor;

                                    // SCRIPT_COMMAND_ACTIVATE_OBJECT (13)

        struct                      // SCRIPT_COMMAND_REMOVE_AURA (14)
        {
            uint32 SpellID;         // datalong
            uint32 Flags;           // datalong2
        } RemoveAura;

        struct                      // SCRIPT_COMMAND_CAST_SPELL (15)
        {
            uint32 SpellID;         // datalong
            uint32 Flags;           // datalong2
            int32  CreatureEntry;   // dataint

            float SearchRadius;
        } CastSpell;

        struct                      // SCRIPT_COMMAND_PLAY_SOUND (16)
        {
            uint32 SoundID;         // datalong
            uint32 Flags;           // datalong2
        } PlaySound;

        struct                      // SCRIPT_COMMAND_CREATE_ITEM (17)
        {
            uint32 ItemEntry;       // datalong
            uint32 Amount;          // datalong2
        } CreateItem;

        struct                      // SCRIPT_COMMAND_DESPAWN_SELF (18)
        {
            uint32 DespawnDelay;    // datalong
        } DespawnSelf;

        struct                      // SCRIPT_COMMAND_LOAD_PATH (20)
        {
            uint32 PathID;          // datalong
            uint32 IsRepeatable;    // datalong2
        } LoadPath;

        struct                      // SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT (21)
        {
            uint32 CreatureEntry;   // datalong
            uint32 ScriptID;        // datalong2
            uint32 ScriptType;      // dataint
        } CallScript;

        struct                      // SCRIPT_COMMAND_KILL (22)
        {
            uint32 Unused1;         // datalong
            uint32 Unused2;         // datalong2
            int32  RemoveCorpse;    // dataint
        } Kill;

        struct                      // SCRIPT_COMMAND_ORIENTATION (30)
        {
            uint32 Flags;           // datalong
            uint32 Unused1;         // datalong2
            int32  Unused2;         // dataint

            float Unused3;
            float Unused4;
            float Unused5;
            float Orientation;
        } Orientation;

        struct                      // SCRIPT_COMMAND_EQUIP (31)
        {
            uint32 EquipmentID;     // datalong
        } Equip;

        struct                      // SCRIPT_COMMAND_MODEL (32)
        {
            uint32 ModelID;         // datalong
        } Model;

                                    // SCRIPT_COMMAND_CLOSE_GOSSIP (33)

        struct                      // SCRIPT_COMMAND_PLAYMOVIE (34)
        {
            uint32 MovieID;         // datalong
        } PlayMovie;

        struct                      // SCRIPT_COMMAND_SET_ANIMKIT (100)
        {
            uint32 AnimKitID;      // datalong
        } SetAnimKit;
    };

    ScriptInfo()
    {
        memset(this, 0, sizeof(ScriptInfo));
    }

    std::string GetDebugInfo() const;
};


struct CharacterTemplate
{
    struct TemplateItem
    {
        int32 m_ItemID;
        int32 m_Faction;
        int32 m_Count;
        int8  m_Type;
    };

    struct TemplateFaction
    {
        uint32 m_FactionID;
        uint32 m_Reputaion;
    };

    uint32 m_ID;
    uint8 m_PlayerClass;
    std::string m_Name;
    std::string m_Description;

    Position m_AliancePos;
    Position m_HordePos;
    int16 m_AlianceMapID;
    int16 m_HordeMapID;
    uint8 m_Level;
    uint64 m_Money;
    uint8 m_HordeDefaultRace;
    uint8 m_AllianceDefaultRace;

    std::list<TemplateItem> m_TemplateItems;
    std::list<TemplateFaction> m_TemplateFactions;
    std::list<uint32> m_SpellIDs;
};

typedef std::unordered_map<uint32, CharacterTemplate*> CharacterTemplates;

typedef std::multimap<uint32, ScriptInfo> ScriptMap;
typedef std::map<uint32, ScriptMap > ScriptMapMap;
typedef std::multimap<uint32, uint32> SpellScriptsContainer;
typedef std::pair<SpellScriptsContainer::iterator, SpellScriptsContainer::iterator> SpellScriptsBounds;
extern ScriptMapMap sQuestEndScripts;
extern ScriptMapMap sQuestStartScripts;
extern ScriptMapMap sSpellScripts;
extern ScriptMapMap sGameObjectScripts;
extern ScriptMapMap sEventScripts;
extern ScriptMapMap sWaypointScripts;

std::string GetScriptsTableNameByType(ScriptsType type);
ScriptMapMap* GetScriptsMapByType(ScriptsType type);
std::string GetScriptCommandName(ScriptCommands command);

struct SpellClickInfo
{
    uint32 spellId;
    uint8 castFlags;
    SpellClickUserTypes userType;

    // helpers
    bool IsFitToRequirements(Unit const* clicker, Unit const* clickee) const;
};

typedef std::multimap<uint32, SpellClickInfo> SpellClickInfoContainer;
typedef std::pair<SpellClickInfoContainer::const_iterator, SpellClickInfoContainer::const_iterator> SpellClickInfoMapBounds;

struct AreaTriggerStruct
{
    uint32 target_mapId;
    uint32 source_zoneId;
    uint32 source_mapId;
    float  target_X;
    float  target_Y;
    float  target_Z;
    float  target_Orientation;
    float  radius;
};

typedef std::set<uint32> CellGuidSet;
typedef std::map<uint32/*player guid*/, uint32/*instance*/> CellCorpseSet;
struct CellObjectGuids
{
    CellGuidSet eventobject;
    CellGuidSet conversation;
    CellGuidSet creatures;
    CellGuidSet gameobjects;
    CellGuidSet areatriggers;
    CellCorpseSet corpses;
};
typedef ACE_Based::LockedMap<uint32/*cell_id*/, CellObjectGuids> CellObjectGuidsMap;
typedef ACE_Based::LockedMap<uint32/*(mapid, spawnMode) pair*/, CellObjectGuidsMap> MapObjectGuids;

// Trinity string ranges
#define MIN_TRINITY_STRING_ID           1                    // 'trinity_string'
#define MAX_TRINITY_STRING_ID           2000000000
#define MIN_DB_SCRIPT_STRING_ID        MAX_TRINITY_STRING_ID // 'db_script_string'
#define MAX_DB_SCRIPT_STRING_ID        2000010000
#define MIN_CREATURE_AI_TEXT_STRING_ID (-1)                 // 'creature_ai_texts'
#define MAX_CREATURE_AI_TEXT_STRING_ID (-1000000)

// Trinity Trainer Reference start range
#define TRINITY_TRAINER_START_REF      200000

struct TrinityStringLocale
{
    StringVector Content;
};

enum VisibleDistanceType
{
    TYPE_VISIBLE_MAP,
    TYPE_VISIBLE_ZONE,
    TYPE_VISIBLE_AREA,
    TYPE_VISIBLE_MAX
};

typedef std::unordered_map<uint32 /*id*/, float /*distance*/> VisibleDistanceMap;
extern VisibleDistanceMap sVisibleDistance[TYPE_VISIBLE_MAX];
float GetVisibleDistance(uint32 type, uint32 id);

typedef std::map<uint64, uint64> LinkedRespawnContainer;
typedef std::unordered_map<uint32, ConversationSpawnData> ConversationDataContainer;
typedef std::unordered_map<uint32, EventObjectData> EventObjectDataContainer;
typedef std::unordered_map<uint32, AreaTriggerData> AreaTriggerDataContainer;
typedef std::unordered_map<uint32, CreatureData> CreatureDataContainer;
typedef std::unordered_map<uint32, GameObjectData> GameObjectDataContainer;
typedef std::unordered_map<uint32, CreatureLocale> CreatureLocaleContainer;
typedef std::unordered_map<uint32, DisplayChoiceLocale> DisplayChoiceLocaleContainer;
typedef std::unordered_map<uint32, GameObjectLocale> GameObjectLocaleContainer;
typedef std::unordered_map<uint32, ItemLocale> ItemLocaleContainer;
typedef std::unordered_map<uint32, QuestLocale> QuestLocaleContainer;
typedef std::unordered_map<uint32, NpcTextLocale> NpcTextLocaleContainer;
typedef std::unordered_map<uint32, PageTextLocale> PageTextLocaleContainer;
typedef std::unordered_map<int32, TrinityStringLocale> TrinityStringLocaleContainer;
typedef std::unordered_map<uint64, GossipMenuItemsLocale> GossipMenuItemsLocaleContainer;
typedef std::unordered_map<uint32, PointOfInterestLocale> PointOfInterestLocaleContainer;
typedef std::unordered_map<uint32, QuestObjectiveLocale> QuestObjectiveLocaleContainer;
typedef std::map<TempSummonGroupKey, std::vector<TempSummonData>> TempSummonDataContainer;
typedef std::unordered_map<uint32, std::set<Quest const*>> QuestAreaTaskMap;
typedef std::unordered_set<uint32> QuestAreaTaskQuests;

typedef std::multimap<uint32, uint32> QuestRelations;
typedef std::pair<QuestRelations::const_iterator, QuestRelations::const_iterator> QuestRelationBounds;

struct PetStatInfo
{
    enum PowerStatBase
    {
        AttackPower = 0,
        SpellPower  = 1
    };

    PetStatInfo()
    {
        memset(this, 0, sizeof(PetStatInfo));
    }

    float m_Speed;
    float m_ArmorCoef;
    float m_APSPCoef;
    float m_HealthCoef;
    float m_DamageCoef;
    float m_AttackSpeed;
    float m_CreatePower;
    float m_SecondaryStatCoef;

    Powers        m_Power;
    PowerStatBase m_PowerStat;
};

struct MailLevelReward
{
    MailLevelReward(uint32 _entry, uint64 _raceMask, uint32 _classMask, std::string& _subject, std::string& _body, uint64 _gold, std::vector<uint32> _items = std::vector<uint32>())
    : entry(_entry)
    , raceMask(_raceMask)
    , classMask(_classMask)
    , subject(_subject)
    , body(_body)
    , gold(_gold)
    , items(_items) {}

    uint32 entry;
    uint64 raceMask;
    uint32 classMask;
    std::string subject;
    std::string body;
    std::vector<uint32> items;
    uint64 gold;

    const static uint32 Sender = 147999;
};

struct MailRewardLocale
{
    StringVector subject;
    StringVector body;
};

using MailRewardСontainer = std::vector<MailLevelReward>;
using MailRewardByLevelContainer = std::unordered_map<uint8, MailRewardСontainer>;
using MailRewardLocaleContainer = std::unordered_map<uint32, MailRewardLocale>;

using CreatureLootBonusMap = std::map<uint32, uint32>;

// We assume the rate is in general the same for all three types below, but chose to keep three for scalability and customization
struct RepRewardRate
{
    float quest_rate;                                       // We allow rate = 0.0 in database. For this case, it means that
    float creature_rate;                                    // no reputation are given at all for this faction/rate type.
    float spell_rate;
};

struct ReputationOnKillEntry
{
    uint32 RepFaction1;
    uint32 RepFaction2;
    uint32 ReputationMaxCap1;
    int32 RepValue1;
    uint32 ReputationMaxCap2;
    int32 RepValue2;
    bool IsTeamAward1;
    bool IsTeamAward2;
    bool TeamDependent;
};

struct RepSpilloverTemplate
{
    uint32 faction[MAX_SPILLOVER_FACTIONS];
    float faction_rate[MAX_SPILLOVER_FACTIONS];
    uint32 faction_rank[MAX_SPILLOVER_FACTIONS];
};

struct PointOfInterest
{
    uint32 entry;
    float x;
    float y;
    uint32 icon;
    uint32 flags;
    uint32 data;
    std::string icon_name;
};

struct GossipMenuItems
{
    uint32             MenuId;
    uint32             OptionIndex;
    uint32             OptionBroadcastTextID;
    uint32             BoxBroadcastTextID;
    uint8              OptionIcon;
    std::string        OptionText;
    uint32             OptionType;
    uint32             OptionNpcflag;
    uint32             ActionMenuId;
    uint32             ActionPoiId;
    bool               BoxCoded;
    uint32             BoxMoney;
    std::string        BoxText;
    ConditionContainer Conditions;
};

struct GossipMenus
{
    uint32             entry;
    uint32             text_id;
    ConditionContainer conditions;
};

typedef std::multimap<uint32, GossipMenus> GossipMenusContainer;
typedef std::pair<GossipMenusContainer::const_iterator, GossipMenusContainer::const_iterator> GossipMenusMapBounds;
typedef std::pair<GossipMenusContainer::iterator, GossipMenusContainer::iterator> GossipMenusMapBoundsNonConst;
typedef std::multimap<uint32, GossipMenuItems> GossipMenuItemsContainer;
typedef std::pair<GossipMenuItemsContainer::const_iterator, GossipMenuItemsContainer::const_iterator> GossipMenuItemsMapBounds;
typedef std::pair<GossipMenuItemsContainer::iterator, GossipMenuItemsContainer::iterator> GossipMenuItemsMapBoundsNonConst;
typedef std::map<uint32, int32> CurrencyOnKillEntry;

struct QuestPOIPoint
{
    int32 X;
    int32 Y;

    QuestPOIPoint() : X(0), Y(0) { }
    QuestPOIPoint(int32 _X, int32 _Y) : X(_X), Y(_Y) { }
};

struct QuestPOI
{
    int32 BlobIndex;
    int32 ObjectiveIndex;
    int32 QuestObjectiveID;
    int32 QuestObjectID;
    int32 MapID;
    int32 WorldMapAreaID;
    int32 Floor;
    int32 Priority;
    int32 Flags;
    int32 WorldEffectID;
    int32 PlayerConditionID;
    int32 UnkWoD1;
    bool AlwaysAllowMergingBlobs;
    std::vector<QuestPOIPoint> points;

    QuestPOI() : BlobIndex(0), ObjectiveIndex(0), QuestObjectiveID(0), QuestObjectID(0), MapID(0), WorldMapAreaID(0), Floor(0), Priority(0), Flags(0), WorldEffectID(0), PlayerConditionID(0), UnkWoD1(0) { }
    QuestPOI(int32 _BlobIndex, int32 _ObjectiveIndex, int32 _QuestObjectiveID, int32 _QuestObjectID, int32 _MapID, int32 _WorldMapAreaID, int32 _Foor, int32 _Priority, int32 _Flags, int32 _WorldEffectID, int32 _PlayerConditionID, int32 _UnkWoD1, bool _AlwaysAllowMergingBlobs) :
        BlobIndex(_BlobIndex), ObjectiveIndex(_ObjectiveIndex), QuestObjectiveID(_QuestObjectiveID), QuestObjectID(_QuestObjectID), MapID(_MapID), WorldMapAreaID(_WorldMapAreaID),
        Floor(_Foor), Priority(_Priority), Flags(_Flags), WorldEffectID(_WorldEffectID), PlayerConditionID(_PlayerConditionID), UnkWoD1(_UnkWoD1), AlwaysAllowMergingBlobs(_AlwaysAllowMergingBlobs) { }
};

enum ChoiceItemType
{
    CHOICE_TYPE_ITEM,
    CHOICE_TYPE_CURRENCY,
    CHOICE_TYPE_FACTION,
    CHOICE_TYPE_ITEMCHOICE,
    CHOICE_TYPE_MAX,
};

struct DisplayChoiceItemData
{
    int32 ResponseID;
    int32 Type;
    int32 Id;
    int32 DisplayID;
    int32 Unk;
    int32 Quantity;
};

struct DisplayChoiceData
{
    int32 ChoiceID;
    int32 ResponseID;
    int32 ChoiceArtFileID;
    bool HasReward;
    std::string Answer;
    std::string Title;
    std::string Description;
    std::string Confirm;
    int32 TitleID;
    int32 PackageID;
    int32 SkillLineID;
    int32 SkillPointCount;
    int32 ArenaPointCount;
    int32 HonorPointCount;
    int64 Money;
    int32 Xp;
    int32 ItemsCount;
    int32 CurrenciesCount;
    int32 FactionsCount;
    int32 ItemChoicesCount;
    std::string Question;
    int32 SpellID;

    std::vector<DisplayChoiceItemData> DisplayChoiceItems[CHOICE_TYPE_MAX];
};

typedef std::unordered_map<uint32/*entry*/, std::vector<DisplayChoiceData> > DisplayChoiceMap;

typedef std::vector<QuestPOI> QuestPOIVector;
typedef std::unordered_map<uint32, QuestPOIVector> QuestPOIContainer;

struct ScenarioPOIPoint
{
    int32 x;
    int32 y;

    ScenarioPOIPoint() : x(0), y(0) {}
    ScenarioPOIPoint(int32 _x, int32 _y) : x(_x), y(_y) {}
};

struct ScenarioPOI
{
    uint32 BlobID;
    uint32 MapID;
    uint32 WorldMapAreaID;
    uint32 Floor;
    uint32 Priority;
    uint32 Flags;
    uint32 WorldEffectID;
    uint32 PlayerConditionID;
    std::vector<ScenarioPOIPoint> points;

    ScenarioPOI() : BlobID(0), MapID(0), WorldMapAreaID(0), Floor(0), Priority(0), Flags(0), WorldEffectID(0), PlayerConditionID(0) {}
    ScenarioPOI(uint32 _BlobID, uint32 _MapID, uint32 _WorldMapAreaID, uint32 _Floor, uint32 _Priority, uint32 _Flags, uint32 _WorldEffectID, uint32 _PlayerConditionID) :
        BlobID(_BlobID), MapID(_MapID), WorldMapAreaID(_WorldMapAreaID), Floor(_Floor), Priority(_Priority), Flags(_Flags), WorldEffectID(_WorldEffectID), PlayerConditionID(_PlayerConditionID) { }
};

typedef std::vector<ScenarioPOI> ScenarioPOIVector;
typedef std::unordered_map<uint32, ScenarioPOIVector> ScenarioPOIContainer;

struct GraveYardData
{
    uint32 safeLocId;
    uint32 team;
};

typedef std::multimap<uint32, GraveYardData> GraveYardContainer;

typedef std::unordered_map<uint32, VendorItemData> CacheVendorItemContainer;
typedef std::unordered_map<uint32, TrainerSpellData> CacheTrainerSpellContainer;

enum SkillRangeType
{
    SKILL_RANGE_LANGUAGE,                                   // 300..300
    SKILL_RANGE_LEVEL,                                      // 1..max skill for level
    SKILL_RANGE_MONO,                                       // 1..1, grey monolite bar
    SKILL_RANGE_RANK,                                       // 1..skill for known rank
    SKILL_RANGE_NONE,                                       // 0..0 always
};

#define MAX_SKILL_STEP 16

struct SkillTiersEntry
{
    uint32      ID;                                         // 0
    uint32      Value[MAX_SKILL_STEP];                      // 1-16
};

SkillRangeType GetSkillRangeType(SkillRaceClassInfoEntry const* rcEntry);

#define MAX_PLAYER_NAME          12                         // max allowed by client name length
#define MAX_INTERNAL_PLAYER_NAME 15                         // max server internal player name length (> MAX_PLAYER_NAME for support declined names)
#define MAX_PET_NAME             12                         // max allowed by client name length
#define MAX_CHARTER_NAME         24                         // max allowed by client name length

bool normalizePlayerName(std::string& name);

struct LanguageDesc
{
    Language lang_id;
    uint32   spell_id;
    uint32   skill_id;
};

extern LanguageDesc lang_description[LANGUAGES_COUNT];
LanguageDesc const* GetLanguageDescByID(uint32 lang);

enum EncounterCreditType
{
    ENCOUNTER_CREDIT_KILL_CREATURE  = 0,
    ENCOUNTER_CREDIT_CAST_SPELL     = 1,
};

struct DungeonEncounter
{
    DungeonEncounter(DungeonEncounterEntry const* _dbcEntry, EncounterCreditType _creditType, uint32 _creditEntry, uint32 _lastEncounterDungeon)
        : dbcEntry(_dbcEntry), creditType(_creditType), creditEntry(_creditEntry), lastEncounterDungeon(_lastEncounterDungeon) { }

    DungeonEncounterEntry const* dbcEntry;
    EncounterCreditType creditType;
    uint32 creditEntry;
    uint32 lastEncounterDungeon;
};

typedef std::list<DungeonEncounter const*> DungeonEncounterList;
typedef std::unordered_map<uint32, DungeonEncounterList> DungeonEncounterContainer;

struct GuildChallengeReward
{
    uint32 Experience;
    uint32 Gold;
    uint32 ChallengeCount;
    uint32 Gold2;
};

struct HotfixInfo
{
    uint32 Type;
    uint32 Timestamp;
    int32  Entry;
};

struct BattlePetTemplate
{
    uint32 Species;
    uint32 Breed;
    uint32 Quality;
    uint32 Level;
};
typedef std::map<uint32, BattlePetTemplate> BattlePetTemplateContainer;

struct BattlePetNpcTeamMember
{
    uint32 Specie;
    uint32 Level;
    uint32 Ability[3];
};
typedef std::map<uint32, std::vector<BattlePetNpcTeamMember>> BattlePetNpcTeamMembers;

struct ResearchLootEntry
{
    uint16 id;
    float x;
    float y;
    float z;
    uint32 ResearchBranchID;
};

struct GarrisonPlotBuildingContent
{
    GarrisonPlotBuildingContent() {}
    GarrisonPlotBuildingContent(const GarrisonPlotBuildingContent& p_Other)
    {
        DB_ID               = p_Other.DB_ID;
        PlotTypeOrBuilding  = p_Other.PlotTypeOrBuilding;
        FactionIndex        = p_Other.FactionIndex;
        CreatureOrGob       = p_Other.CreatureOrGob;
        X                   = p_Other.X;
        Y                   = p_Other.Y;
        Z                   = p_Other.Z;
        O                   = p_Other.O;
    }

    uint32 DB_ID;
    int32 PlotTypeOrBuilding;
    uint32 FactionIndex;
    int32 CreatureOrGob;
    float X, Y, Z, O;
};

struct GarrisonMissionReward
{
    uint32 FollowerXP;
    uint32 ItemID;
    uint32 ItemQuantity;
    uint32 CurrencyType;
    uint32 CurrencyQuantity;
    uint32 TreasureChance;
    uint32 TreasureXP;
    uint32 TreasureQuality;
    uint32 GarrMssnBonusAbilityID;

    uint32 BonusGold;
    uint32 BonusItemID;
};

/// Tradeskill NPC Recipes
struct RecipesConditions
{
    uint32 RecipeID;
    uint32 PlayerConditionID;
};

namespace ItemBonusGroupFlags
{
    enum
    {
        QuestRewardQualityUpgrade   = 0x01,   ///< Used for items which are quest reward and have a chance get a quality upgrade
        BonusForLFRDifficulty       = 0x02,
        BonusForNormalDifficulty    = 0x04,
        BonusForHeroicDifficulty    = 0x08,
        BonusForMythicDifficulty    = 0x10
    };
};

struct ItemBonusGroup
{
    std::vector<uint32> Bonuses;
    uint32              Flags;
};

using ItemBonusGroupMap = std::map<uint32, ItemBonusGroup>;

struct ConversationActor
{
    uint32 entry;
    uint32 ID;
    uint32 ActorID;
    uint32 CreatureID;
    uint32 DisplayID;
    uint32 Unk1;
    uint32 Unk2;
    uint32 Unk3;
    uint32 Duration;
};

struct ConversationCreature
{
    uint32 Entry;
    uint32 ID;
    uint32 CreatureID;
    uint32 CreatureGuid;
    uint32 Unk1;
    uint32 Unk2;
    uint32 Duration;
};

struct ConversationData
{
    uint32 Entry;
    uint32 ID;
    uint32 TextID;
    uint32 Unk1;
    uint32 Unk2;
};

struct CreatureActionData
{
    uint32 entry;
    uint32 target;
    uint8 type;
    uint32 spellId;
    uint8 action;
};

struct ItemExtendedCostAddon
{
    uint32 Entry;
    uint64 OverrideBuyPrice;
};

using ConversationDataMap       = std::unordered_map<uint32/*entry*/, std::vector<ConversationData>>;
using ConversationCreatureMap   = std::unordered_map<uint32/*entry*/, std::vector<ConversationCreature>>;
using ConversationActorMap      = std::unordered_map<uint32/*entry*/, std::vector<ConversationActor>>;
using CreatureActionDataMap     = std::unordered_map<uint32/*entry*/, std::vector<CreatureActionData>>;
using ItemExtendedCostAddonMap  = std::unordered_map<uint32, ItemExtendedCostAddon>;

typedef std::vector<HotfixInfo> HotfixData;
typedef std::map<uint32, uint32> QuestObjectiveLookupMap;
typedef std::vector<GuildChallengeReward> GuildChallengeRewardData;
typedef std::map<uint32, bool> UpdateSkipData;

typedef std::vector<ResearchLootEntry> ResearchLootVector;
typedef std::vector<ResearchPOIPoint> ResearchPOIPoints;
typedef std::map<uint32 /*site id*/, ResearchZoneEntry> ResearchZoneMap;
typedef std::map<uint32, std::vector<RecipesConditions>> NpcRecipesContainer;

struct AreaTriggerForce
{
    uint32 AuraID;
    uint32 CustomEntry;
    G3D::Vector3 wind;
    G3D::Vector3 center;
    float windSpeed;
    uint8 windType;
};

typedef std::unordered_map<uint32/*entry*/, std::vector<AreaTriggerForce> > AreaTriggerForceMap;

struct ScenarioData
{
    uint32 ScenarioID;
    uint32 MapID;
    uint32 ZoneID;
    uint32 DifficultyID;
    uint32 Team;
    uint32 Class;
};

struct ScenarioSpellData
{
    uint32 StepId;
    uint32 Spells;
};

typedef std::unordered_map<uint32/*ScenarioID*/, ScenarioData> ScenarioDataMap;
typedef std::unordered_map<uint32 /*MapID*/, std::list<ScenarioData* >> ScenarioDataListMap;
typedef std::unordered_map<uint32 /*ScenarioID*/, std::vector<ScenarioSpellData>> ScenarioDataSpellsStep;

struct CustomInstanceZone
{
    uint32   RealZoneID;
    uint32   CustomZoneID;
    uint32   MaxPlayerCount;
    float    Radius;
    Position Position;
};

typedef std::vector<CustomInstanceZone> CustomInstanceZones;
typedef std::unordered_map<uint32/*zoneId*/, CustomInstanceZones> CustomInstanceZonesMap;

struct CombatAIEventData
{
    uint32 StartMin;
    uint32 StartMax;
    uint32 RepeatMin;
    uint32 RepeatMax;
    uint32 RepeatFail;

    uint32 SpellID;
    float AttackDist;

    uint32 EventCheck;
    uint32 EventFlags;

    uint32 DifficultyMask;
};

struct ChangelogItem
{
    uint32 Date;
    std::string Title;
    std::string Description;
};

struct NewsFeedItem
{
    uint32 Timestamp;
    std::string Description;
    LocaleConstant Locale;
};

struct PlayerChoiceResponseLocale
{
    std::vector<std::string> Answer;
    std::vector<std::string> Header;
    std::vector<std::string> Description;
    std::vector<std::string> Confirmation;
};

struct PlayerChoiceLocale
{
    std::vector<std::string> Question;
    std::unordered_map<int32 /*ResponseId*/, PlayerChoiceResponseLocale> Responses;
};

struct PlayerChoiceResponseRewardItem
{
    PlayerChoiceResponseRewardItem() : Id(0), Quantity(0) { }
    PlayerChoiceResponseRewardItem(uint32 id, std::vector<int32> bonusListIDs, int32 quantity) : Id(id), BonusListIDs(std::move(bonusListIDs)), Quantity(quantity) { }

    uint32 Id;
    std::vector<int32> BonusListIDs;
    int32 Quantity;
};

struct PlayerChoiceResponseRewardEntry
{
    PlayerChoiceResponseRewardEntry() : Id(0), Quantity(0) { }
    PlayerChoiceResponseRewardEntry(uint32 id, int32 quantity) : Id(id), Quantity(quantity) { }

    uint32 Id;
    int32 Quantity;
};

struct PlayerChoiceResponseReward
{
    int32 TitleID;
    int32 PackageID;
    int32 SkillLineID;
    uint32 SkillPointCount;
    uint32 ArenaPointCount;
    uint32 HonorPointCount;
    uint64 Money;
    uint32 Xp;
    bool Unique;

    std::vector<PlayerChoiceResponseRewardItem> Items;
    std::vector<PlayerChoiceResponseRewardEntry> Currency;
    std::vector<PlayerChoiceResponseRewardEntry> Faction;
    std::vector<uint32> Spells;
};

struct PlayerChoiceResponse
{
    int32 ResponseID;
    int32 ChoiceArtFileID;

    std::string Header;
    std::string Answer;
    std::string Description;
    std::string Confirmation;
    Optional<PlayerChoiceResponseReward> Reward;
};

struct PlayerChoice
{
    int32 ChoiceId;
    std::string Question;
    int32 UiTextureKitID;
    std::vector<PlayerChoiceResponse> Responses;

    PlayerChoiceResponse const* GetResponse(int32 p_ResponseID) const
    {
        auto l_Itr = std::find_if(Responses.begin(), Responses.end(),
            [p_ResponseID](PlayerChoiceResponse const& playerChoiceResponse) { return playerChoiceResponse.ResponseID == p_ResponseID; });
        return l_Itr != Responses.end() ? &(*l_Itr) : nullptr;
    }
};

struct PvpSeasonReward
{
    uint32 AchievementId;
    uint32 TitleId;
    uint32 ItemId;
};

struct CosmeticEventAddonEntry
{
    uint32  FollowingNpc;
    float   Distance;
    float   Angle;
};

struct CosmeticEventEntry
{
    uint32  EventId;
    uint32  Index;
    uint32  Delay;
    uint32  NpcEntry;
    uint32  PathId;
    uint32  MovementType;
    float   Speed;
    uint32  Flags;
    int32   DespawnTime;
    uint32  MoveTime;
    uint32  RandomDistMin;
    uint32  RandomDistMax;
    uint32  DifficultyMask;

    std::vector<CosmeticEventAddonEntry> Addons;
};

enum CosmeticEventFlags
{
    CosmeticSmooth          = 0x01,
    CosmeticCyclic          = 0x02,
    CosmeticSummonByPlayer  = 0x04,
    CosmeticGroundSmooth    = 0x08,
    CosmeticActsAsWaypoints = 0x10
};

enum PvpSeasons
{
    LegionSeason1,
    LegionSeason2,
    LegionSeason3,
    LegionSeason4,
    LegionSeason5,
    LegionSeason6,
    LegionSeason7,
    MaxPvpSeason,
    CurrentSeason = LegionSeason7
};

struct ItemSeasonDescriptions
{
    ItemSeasonDescriptions(uint32 p_RegularGearDescriptionID, uint32 p_EliteGearDescriptionID)
    {
        RegularGearDescriptionID = p_RegularGearDescriptionID;
        EliteGearDescriptionID   = p_EliteGearDescriptionID;
    }

    uint32 RegularGearDescriptionID;
    uint32 EliteGearDescriptionID;
};

typedef std::map<uint32 /*MinRating*/, PvpSeasonReward> PvPSeasonRewards;
typedef std::map<uint8 /*Bracket*/, PvPSeasonRewards> PvpSeasonRewardsPerBracket;

enum ColorFakeProtectionTypes
{
    COLOR_FAKE_PROTECTION_GUILD     = 1,
    COLOR_FAKE_PROTECTION_CALENDAR  = 2,
    COLOR_FAKE_PROTECTION_CHANNEL   = 4,
    COLOR_FAKE_PROTECTION_MAIL      = 8,
    COLOR_FAKE_PROTECTION_AFK       = 16,
    COLOR_FAKE_PROTECTION_DND       = 32
};

#ifndef CROSS
class PlayerDumpReader;
#endif /* not CROSS */

class ObjectMgr
{
#ifndef CROSS
    friend class PlayerDumpReader;
#endif /* not CROSS */
    friend class ACE_Singleton<ObjectMgr, ACE_Null_Mutex>;

    private:
        ObjectMgr();
        ~ObjectMgr();

    public:
        typedef std::unordered_map<uint32, Item*> ItemMap;

        typedef std::unordered_map<uint32, Quest*> QuestMap;

        typedef std::unordered_map<uint32, AreaTriggerStruct> AreaTriggerContainer;

        typedef std::unordered_map<std::pair<uint32, std::pair<uint32, uint32>>, AreaTriggerStruct> GameobjectAreaTriggerContainer;

        typedef std::unordered_map<uint32, uint32> AreaTriggerScriptContainer;

        typedef std::unordered_map<uint32, AccessRequirement> AccessRequirementContainer;
        typedef std::unordered_map<uint32, LFRAccessRequirement> LFRAccessRequirements;
        typedef std::unordered_map<uint32, RepRewardRate > RepRewardRateContainer;
        typedef std::unordered_map<uint32, ReputationOnKillEntry> RepOnKillContainer;
        typedef std::unordered_map<uint32, RepSpilloverTemplate> RepSpilloverTemplateContainer;
        typedef std::unordered_map<uint32, CurrencyOnKillEntry> CurOnKillContainer;

        typedef std::unordered_map<uint32, PointOfInterest> PointOfInterestContainer;

        typedef std::vector<std::string> ScriptNameContainer;

        typedef std::map<uint32, uint32> CharacterConversionMap;

        Player* GetPlayerByLowGUID(uint32 lowguid) const;

        GameObjectTemplate const* GetGameObjectTemplate(uint32 entry);
        GameObjectTemplateContainer const* GetGameObjectTemplates() const { return &_gameObjectTemplateStore; }
        int LoadReferenceVendor(int32 vendor, int32 item, uint8 type, std::set<uint32>* skip_vendors);

        void LoadGameObjectTemplate();
        void LoadGameObjectAreatriggerTeleport();

        void LoadGarrisonPlotBuildingContent();
        void AddGarrisonPlotBuildingContent(GarrisonPlotBuildingContent& p_Data);
        void DeleteGarrisonPlotBuildingContent(GarrisonPlotBuildingContent& p_Data);
        std::vector<GarrisonPlotBuildingContent> GetGarrisonPlotBuildingContent(int32 p_PlotTypeOrBuilding, uint32 p_FactionIndex);

        void LoadGarrisonMissionReward();
        std::vector<GarrisonMissionReward> const* GetMissionRewards(uint32 p_MissionID);

        void LoadNpcRecipesConditions();
        std::vector<RecipesConditions> GetNpcRecipesConditions(uint32 p_NpcID) { return _NpcRecipesConditions[p_NpcID]; }

        void LoadEventObjectTemplates();
        void LoadEventObjects();

        void LoadAreaTriggers();

        EventObjectTemplate const* GetEventObjectTemplate(uint32 p_Entry);
        EventObjectTemplate const* AddEventObjectTemplate(uint32 p_Entry, float p_Radius, uint32 p_Spell, uint32 p_WorldSafe);
        EventObjectData& NewOrExistEventObjectData(uint32 const& p_Guid) { return m_EventObjectData[p_Guid]; }

        EventObjectData const* GetEventObjectData(uint32 const& p_Guid) const
        {
            EventObjectDataContainer::const_iterator l_Iter = m_EventObjectData.find(p_Guid);
            if (l_Iter == m_EventObjectData.end())
                return nullptr;

            return &l_Iter->second;
        }

        AreaTriggerData const* GetAreaTriggerData(uint32 const& p_Guid) const
        {
            AreaTriggerDataContainer::const_iterator l_Iter = m_AreaTriggerData.find(p_Guid);
            if (l_Iter == m_AreaTriggerData.end())
                return nullptr;

            return &l_Iter->second;
        }

        void AddEventObjectToGrid(uint32 const& p_Guid, EventObjectData const* p_Data);

        void AddAreaTriggerToGrid(uint32 const& p_Guid, AreaTriggerData const* p_Data);

        CreatureTemplate const* GetCreatureTemplate(uint32 entry);
        CreatureTemplate const* GetRandomTemplate(CreatureType p_Type);
        CreatureTemplateContainer const* GetCreatureTemplates() const { return &m_CreatureTemplateStore; }

        CreatureModelInfo const* GetCreatureModelInfo(uint32 modelId);
        CreatureModelInfo const* GetCreatureModelRandomGender(uint32* displayID);
        static uint32 ChooseDisplayId(uint32 team, const CreatureTemplate* cinfo, const CreatureData* data = NULL);
        static void ChooseCreatureFlags(const CreatureTemplate* p_CreatureTemplate, uint32& p_NpcFlags1, uint32& p_NpcFlags2, uint32& p_UnitFlags1, uint32& p_UnitFlags2, uint32& p_UnitFlags3, uint32& p_Dynamicflags, const CreatureData* p_Data = nullptr);
        EquipmentInfo const* GetEquipmentInfo(uint32 p_Entry, int8& p_ID);
        CreatureAddon const* GetCreatureAddon(uint32 lowguid);
        CreatureAddon const* GetCreatureTemplateAddon(uint32 entry);
        ItemTemplate const* GetItemTemplate(uint32 entry);
        ItemTemplate const* GetItemRewardForPvp(uint16 p_Spec, MS::Battlegrounds::RewardGearType p_Type, Player* p_Source, InventoryType p_InventoryType = InventoryType::INVTYPE_NON_EQUIP, PvpSeasons p_Season = PvpSeasons::CurrentSeason);
        ItemTemplate const* GetItemLegendary(uint16 p_Spec, Player* p_Source, bool p_Relinquished = false, InventoryType p_InventoryType = InventoryType::INVTYPE_NON_EQUIP);
        ItemTemplate const* GetItemLegendaryForClass(uint32 p_ClassID, Player* p_Source);
        bool IsLegionLegendaryItem(uint32 p_ItemId, bool p_Relinquished = false) const;

        ItemTemplateContainer const* GetItemTemplateStore() const { return &_itemTemplateStore; }

        InstanceTemplate const* GetInstanceTemplate(uint32 mapId);

        PetStatInfo const* GetPetStatInfo(uint32 p_Entry) const;

        void GetPlayerClassLevelInfo(uint32 class_, uint8 level, uint32& baseHP, uint32& baseMana) const;

        PlayerInfo const* GetPlayerInfo(uint32 race, uint32 class_) const
        {
            if (race >= MAX_RACES)
                return NULL;
            if (class_ >= MAX_CLASSES)
                return NULL;
            PlayerInfo const* info = &_playerInfo[race][class_];
            if (info->displayId_m == 0 || info->displayId_f == 0)
                return NULL;
            return info;
        }

        void GetPlayerLevelInfo(uint32 race, uint32 class_, uint8 level, PlayerLevelInfo* info) const;

        uint64 GetPlayerGUIDByName(std::string name) const;
        bool GetPlayerNameByGUID(uint64 guid, std::string& name) const;
        uint32 GetPlayerTeamByGUID(uint64 guid) const;
#ifdef CROSS

#endif /* CROSS */
        uint32 GetPlayerAccountIdByGUID(uint64 guid) const;
#ifndef CROSS
        uint32 GetPlayerAccountIdByPlayerName(const std::string& name) const;
#endif /* not CROSS */

        uint32 GetNearestTaxiNode(float x, float y, float z, uint32 mapid, uint32 team);
        void GetTaxiPath(uint32 source, uint32 destination, uint32& path, uint32& cost);
        uint32 GetTaxiMountDisplayId(uint32 id, uint32 team, bool allowed_alt_team = false, uint32 p_PreferedMountDisplay = 0);

        Quest const* GetQuestTemplate(uint32 quest_id) const
        {
            QuestMap::const_iterator itr = _questTemplates.find(quest_id);
            return itr != _questTemplates.end() ? itr->second : NULL;
        }

        QuestMap const& GetQuestTemplates() const { return _questTemplates; }

        std::vector<uint32>* GetScriptedQuests() { return &m_ScriptedQuests; }

        uint32 GetQuestForAreaTrigger(uint32 Trigger_ID) const
        {
            QuestAreaTriggerContainer::const_iterator itr = _questAreaTriggerStore.find(Trigger_ID);
            if (itr != _questAreaTriggerStore.end())
                return itr->second;
            return 0;
        }

        bool IsTavernAreaTrigger(uint32 Trigger_ID) const
        {
            return _tavernAreaTriggerStore.find(Trigger_ID) != _tavernAreaTriggerStore.end();
        }

        bool IsGameObjectForQuests(uint32 entry) const
        {
            return _gameObjectForQuestStore.find(entry) != _gameObjectForQuestStore.end();
        }

        GossipText const* GetGossipText(uint32 Text_ID) const;

        WorldSafeLocsEntry const* GetDefaultGraveYard(uint32 team);
        WorldSafeLocsEntry const* GetClosestGraveYard(float x, float y, float z, uint32 MapId, uint32 team);
        bool AddGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool persist = true);
        void RemoveGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool persist = false);
        void LoadGraveyardZones();
        GraveYardData const* FindGraveYardData(uint32 id, uint32 zone);

        AreaTriggerStruct const* GetAreaTrigger(uint32 trigger) const
        {
            AreaTriggerContainer::const_iterator itr = _areaTriggerStore.find(trigger);
            if (itr != _areaTriggerStore.end())
                return &itr->second;
            return NULL;
        }

        AccessRequirement const* GetAccessRequirement(uint32 mapid, Difficulty difficulty) const
        {
            AccessRequirementContainer::const_iterator itr = _accessRequirementStore.find(MAKE_PAIR32(mapid, difficulty));
            if (itr != _accessRequirementStore.end())
                return &itr->second;
            return NULL;
        }

        LFRAccessRequirement const* GetLFRAccessRequirement(uint32 p_DungeonID) const
        {
            LFRAccessRequirements::const_iterator l_Iter = m_LFRAccessRequirements.find(p_DungeonID);
            if (l_Iter != m_LFRAccessRequirements.end())
                return &l_Iter->second;

            return nullptr;
        }

        AreaTriggerStruct const* GetGoBackTrigger(uint32 Map) const;
        AreaTriggerStruct const* GetMapEntranceTrigger(uint32 Map) const;

        uint32 GetAreaTriggerScriptId(uint32 trigger_id);
        SpellScriptsBounds GetSpellScriptsBounds(uint32 spell_id);

        RepRewardRate const* GetRepRewardRate(uint32 factionId) const
        {
            RepRewardRateContainer::const_iterator itr = _repRewardRateStore.find(factionId);
            if (itr != _repRewardRateStore.end())
                return &itr->second;

            return NULL;
        }

        ReputationOnKillEntry const* GetReputationOnKilEntry(uint32 id) const
        {
            RepOnKillContainer::const_iterator itr = _repOnKillStore.find(id);
            if (itr != _repOnKillStore.end())
                return &itr->second;
            return NULL;
        }

        RepSpilloverTemplate const* GetRepSpilloverTemplate(uint32 factionId) const
        {
            RepSpilloverTemplateContainer::const_iterator itr = _repSpilloverTemplateStore.find(factionId);
            if (itr != _repSpilloverTemplateStore.end())
                return &itr->second;

            return NULL;
        }

        CurrencyOnKillEntry const* GetCurrencyOnKillEntry(uint32 id) const
        {
            CurOnKillContainer::const_iterator itr = _curOnKillStore.find(id);
            if (itr != _curOnKillStore.end())
                return &itr->second;
            return NULL;
        }

        CurrencyOnKillEntry const* GetPersonnalCurrencyOnKillEntry(uint32 p_ID) const
        {
            CurOnKillContainer::const_iterator l_Iter = m_PersonnalCurrOnKillStore.find(p_ID);
            if (l_Iter != m_PersonnalCurrOnKillStore.end())
                return &l_Iter->second;
            return NULL;
        }

        PointOfInterest const* GetPointOfInterest(uint32 id) const
        {
            PointOfInterestContainer::const_iterator itr = _pointsOfInterestStore.find(id);
            if (itr != _pointsOfInterestStore.end())
                return &itr->second;
            return NULL;
        }

        QuestPOIVector const* GetQuestPOIVector(uint32 questId)
        {
            QuestPOIContainer::const_iterator itr = _questPOIStore.find(questId);
            if (itr != _questPOIStore.end())
                return &itr->second;
            return NULL;
        }

        ScenarioPOIVector const* GetScenarioPOIVector(uint32 criteriaTreeId)
        {
            ScenarioPOIContainer::const_iterator itr = _scenarioPOIStore.find(criteriaTreeId);
            if (itr != _scenarioPOIStore.end())
                return &itr->second;
            return NULL;
        }

        VehicleAccessoryList const* GetVehicleAccessoryList(Vehicle* veh) const;

        DungeonEncounterList const* GetDungeonEncounterList(uint32 mapId, Difficulty difficulty)
        {
            std::unordered_map<uint32, DungeonEncounterList>::const_iterator itr = _dungeonEncounterStore.find(MAKE_PAIR32(mapId, difficulty));

            if (itr != _dungeonEncounterStore.end())
                return &itr->second;
            else
            {
                itr = _dungeonEncounterStore.find(MAKE_PAIR32(mapId, DifficultyNone));
                if (itr != _dungeonEncounterStore.end())
                    return &itr->second;
            }

            return NULL;
        }

        void LoadQuests();
        void LoadQuestObjectives();
        void LoadQuestObjectiveLocales();

        void LoadQuestRelations()
        {
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading GO Start Quest Data...");
            LoadGameobjectQuestStarters();
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading GO End Quest Data...");
            LoadGameobjectQuestEnders();
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Creature Start Quest Data...");
            LoadCreatureQuestStarters();
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, "Loading Creature End Quest Data...");
            LoadCreatureQuestEnders();
        }
        void LoadFollowerQuests();
        std::vector<uint32> FollowerQuests;

        void LoadWorldQuests();

        void LoadQuestForItem();
        std::map<uint32, std::vector<std::pair<uint32, uint8>>> QuestForItem;    ///< <ItemID, [<QuestID, ObjectiveIndex>]>

        void LoadGameobjectQuestStarters();
        void LoadGameobjectQuestEnders();
        void LoadCreatureQuestStarters();
        void LoadCreatureQuestEnders();

        QuestRelations* GetGOQuestRelationMap()
        {
            return &_goQuestRelations;
        }

        QuestRelationBounds GetGOQuestRelationBounds(uint32 go_entry)
        {
            return _goQuestRelations.equal_range(go_entry);
        }

        QuestRelationBounds GetGOQuestInvolvedRelationBounds(uint32 go_entry)
        {
            return _goQuestInvolvedRelations.equal_range(go_entry);
        }

        QuestRelations* GetCreatureQuestRelationMap()
        {
            return &_creatureQuestRelations;
        }

        QuestRelationBounds GetCreatureQuestRelationBounds(uint32 creature_entry)
        {
            return _creatureQuestRelations.equal_range(creature_entry);
        }

        void AddCreatureQuestRelationBounds(uint32 p_Creature_entry, uint32 p_QuestID)
        {
            if (_creatureQuestRelations.find(p_Creature_entry) == _creatureQuestRelations.end())
                _creatureQuestRelations.insert(std::make_pair(p_Creature_entry, p_QuestID));
        }

        QuestRelationBounds GetCreatureQuestInvolvedRelationBounds(uint32 creature_entry)
        {
            return _creatureQuestInvolvedRelations.equal_range(creature_entry);
        }

        void AddCreatureQuestInvolvedRelationBounds(uint32 p_Creature_entry, uint32 p_QuestID)
        {
            if (_creatureQuestInvolvedRelations.find(p_Creature_entry) == _creatureQuestInvolvedRelations.end())
                _creatureQuestInvolvedRelations.insert(std::make_pair(p_Creature_entry, p_QuestID));
        }

        void LoadGameObjectScripts();
        void LoadQuestEndScripts();
        void LoadQuestStartScripts();
        void LoadEventScripts();
        void LoadSpellScripts();
        void LoadWaypointScripts();
        void LoadSpellScriptNames();
        void ValidateSpellScripts();
        bool LoadTrinityStrings(char const* table, int32 min_value, int32 max_value);
        bool LoadTrinityStrings() { return LoadTrinityStrings("trinity_string", MIN_TRINITY_STRING_ID, MAX_TRINITY_STRING_ID); }
        void LoadDbScriptStrings();
        void LoadCreatureClassLevelStats();
        void LoadCreatureGroupSizeStats();
        void LoadCreatureDiffHealth();
        void LoadCreatureWDBLocales();
        void LoadWDBCreatureTemplates();
        void LoadCreatureTemplates();
        void LoadCombatAIEvents();
        void LoadCreatureTemplatesDifficulties();
        void LoadCreatureTemplateAddons();
        void CheckCreatureTemplate(CreatureTemplate const* cInfo);
        void CheckCreatureTemplateWDB(CreatureTemplate* p_Template);
        void RestructCreatureGUID(uint32 nbLigneToRestruct);
        void RestructGameObjectGUID(uint32 nbLigneToRestruct);
        void LoadTempSummons();
        void LoadConversations();
        void LoadCreatures();
        void LoadLinkedRespawn();
        bool SetCreatureLinkedRespawn(uint32 guid, uint32 linkedGuid);
        void LoadCreatureAddons();
        void LoadCreatureModelInfo();
        void LoadEquipmentTemplates();
        void LoadGameObjectLocales();
        void LoadGameobjects();
        void LoadItemTemplates();
        void LoadPvpItemRewards();
        void LoadAvailableLegendaryItems();
        void LoadItemTemplateCorrections();
        void LoadItemTemplateAddon();
        void LoadItemScriptNames();
        void LoadItemSpecs();
        void LoadItemBonusGroup();
        void LoadItemBonusGroupLinked();
        void LoadItemExtendedCostAddon();
        void LoadQuestLocales();
        void LoadNpcTextLocales();
        void LoadPageTextLocales();
        void LoadGossipMenuItemsLocales();
        void LoadPointOfInterestLocales();
        void LoadInstanceTemplate();
        void LoadInstanceEncounters();
        void LoadMailLevelRewards();
        void LoadMailRewardsLocale();
        void LoadCreatureBonusLoots();
        void LoadVehicleTemplateAccessories();
        void LoadGossipText();

        void LoadAreaTriggerTeleports();
        void LoadAccessRequirements();
        void LoadLFRAccessRequirements();
        void LoadQuestAreaTriggers();
        void LoadAreaTriggerScripts();
        void LoadTavernAreaTriggers();
        void LoadGameObjectForQuests();

        void LoadPageTexts();
        PageText const* GetPageText(uint32 pageEntry);

        void LoadSpellAuraNotSaved();

        void LoadPlayerInfo();
        void LoadPetStatInfo();
        void LoadExplorationBaseXP();
        void LoadPetNames();
        void LoadPetNumber();
        void LoadCorpses();
        void LoadFishingBaseSkillLevel();

        void LoadReputationRewardRate();
        void LoadReputationOnKill();
        void LoadReputationSpilloverTemplate();
        void LoadCurrencyOnKill();
        void LoadPersonnalCurrencyOnKill();

        void LoadPointsOfInterest();
        void LoadQuestPOI();
        void LoadScenarioPOI();

        void LoadNPCSpellClickSpells();

        void LoadGameTele();

        void LoadGossipMenu();
        void LoadGossipMenuItems();

        void LoadVendors();
        void LoadTrainerSpell();
        void AddSpellToTrainer(uint32 entry, uint32 spell, uint32 spellCost, uint32 reqSkill, uint32 reqSkillValue, uint32 reqLevel);

        void LoadPhaseDefinitions();
        void LoadSpellPhaseInfo();
        void LoadConversationData();
        void LoadSpellInvalid();
        void LoadSpellStolen();
        void LoadDisabledEncounters();
        void LoadDisabledMaps();
        void LoadInstanceWhitelists();
        void LoadWorldVisibleDistance();
        void LoadBattlePetTemplate();
        void LoadBattlePetNpcTeamMember();
        void ComputeBattlePetSpawns();

        void LoadForbiddenRegex();

        void LoadCosmetics();

        void LoadAddons();
        void LoadRAFRewards();
        void LoadPvPSeasonReward();

        void LoadLiveChangelog();
        void LoadNewsFeed();
        void LoadIngameAlerts();

        void LoadShortcutAreas();

        void LoadCosmeticEvents();

        PvpSeasonReward const* GetPvpReward(uint8 p_Bracket, uint32 p_Rating);

#ifndef CROSS
        void LoadGuildChallengeRewardInfo();

#endif /* not CROSS */
        void LoadResearchSiteZones();
        void LoadResearchSiteLoot();

        void LoadCharacterTemplateData();

        void LoadPlayerChoices();
        void LoadPlayerChoicesLocale();

        std::vector<BattlePetNpcTeamMember> GetPetBattleTrainerTeam(uint32 p_NpcID)
        {
            return m_BattlePetNpcTeamMembers[p_NpcID];
        }

        BattlePetTemplate const* GetBattlePetTemplate(uint32 species) const
        {
            BattlePetTemplateContainer::const_iterator itr = _battlePetTemplateStore.find(species);
            return itr != _battlePetTemplateStore.end() ? &itr->second : NULL;
        }

        ResearchZoneMap const& GetResearchZoneMap() const { return sResearchSiteDataMap; }
        ResearchLootVector const& GetResearchLoot() const { return _researchLoot; }

        PhaseDefinitionStore const* GetPhaseDefinitionStore() { return &_PhaseDefinitionStore; }
        SpellPhaseStore const* GetSpellPhaseStore() { return &_SpellPhaseStore; }

        std::string GeneratePetName(uint32 entry);
        uint32 GetBaseXP(uint8 level);
        uint32 GetXPForLevel(uint8 level) const;

        int32 GetFishingBaseSkillLevel(uint32 entry) const
        {
            FishingBaseSkillContainer::const_iterator itr = _fishingBaseForAreaStore.find(entry);
            return itr != _fishingBaseForAreaStore.end() ? itr->second : 0;
        }

        void LoadSkillTiers();
        SkillTiersEntry const* GetSkillTier(uint32 skillTierId) const
        {
            auto itr = _skillTiers.find(skillTierId);
            return itr != _skillTiers.end() ? &itr->second : nullptr;
        }

#ifndef CROSS
        void ReturnOrDeleteOldMails(bool serverUp);

#endif /* not CROSS */
        CreatureBaseStats const* GetCreatureBaseStats(uint8 level, uint8 unitClass);
        CreatureGroupSizeStat const* GetCreatureGroupSizeStat(uint32 p_Entry, uint32 p_Difficulty) const;
        float const* GetCreatureScalingPct(uint32 p_Size) const;
        uint64 const* GetDiffHealthValue(uint32 p_Entry, uint32 p_DifficultyID) const;

        void SetHighestGuids();
#ifdef CROSS

#endif /* CROSS */
        uint32 GenerateLowGuid(HighGuid guidhigh);
#ifndef CROSS
        uint32 GenerateLowGuid(HighGuid p_GuidHigh, uint32 p_Range);
#endif /* not CROSS */
        uint32 GenerateAuctionID();
#ifndef CROSS
        uint64 GenerateEquipmentSetGuid(uint32 p_Range = 1);
        uint32 GenerateMailID(uint32 p_Range = 1);
        uint32 GeneratePetNumber(uint32 p_Range = 1);
#else /* CROSS */
        uint64 GenerateEquipmentSetGuid();
        uint32 GenerateMailID();
        uint32 GeneratePetNumber();
#endif /* CROSS */
        uint64 GenerateVoidStorageItemId();

#ifdef CROSS
        uint32 GenerateLocalRealmLowGuid(HighGuid p_GuidHigh, uint32 p_RealmID);

#endif /* CROSS */

        uint64 GenerateEncounterAttemptID();

        typedef std::multimap<int32, uint32> ExclusiveQuestGroups;
        ExclusiveQuestGroups mExclusiveQuestGroups;

        MailLevelReward const* GetMailLevelReward(uint8 level, uint64 raceMask, uint32 classMask)
        {
            auto itr = _mailLevelReward.find(level);
            if (itr != _mailLevelReward.end())
            {
                for (MailLevelReward const& reward : itr->second)
                    if ((!reward.raceMask || reward.raceMask & raceMask) && (!reward.classMask || reward.classMask & classMask))
                        return &reward;
            }

            return nullptr;
        }

        MailRewardLocale const* GetMailRewardLocale(uint32 entry)
        {
            auto itr = _mailRewardLocale.find(entry);
            if (itr != _mailRewardLocale.end())
                return &itr->second;

            return nullptr;
        }

        uint32 GetCreatureLootBonus(uint32 p_Entry) const
        {
            auto l_Iter = m_CreatureBonusLootData.find(p_Entry);
            if (l_Iter != m_CreatureBonusLootData.end())
                return l_Iter->second;

            return 0;
        }

        CellObjectGuids const& GetCellObjectGuids(uint16 mapid, uint8 spawnMode, uint32 cell_id)
        {
            return _mapObjectGuidsStore[MAKE_PAIR32(mapid, spawnMode)][cell_id];
        }

        CellObjectGuidsMap const& GetMapObjectGuids(uint16 mapid, uint8 spawnMode)
        {
            return _mapObjectGuidsStore[MAKE_PAIR32(mapid, spawnMode)];
        }

       /**
        * Gets temp summon data for all creatures of specified group.
        *
        * @param summonerId   Summoner's entry.
        * @param summonerType Summoner's type, see SummonerType for available types.
        * @param group        Id of required group.
        *
        * @return null if group was not found, otherwise reference to the creature group data
        */
        std::vector<TempSummonData> const* GetSummonGroup(uint32 summonerId, SummonerType summonerType, uint8 group) const;

        ConversationSpawnData const* GetConversationSpawnData(uint32 p_Guid) const
        {
            ConversationDataContainer::const_iterator l_It = m_ConversationSpawnDataStore.find(p_Guid);

            if (l_It == m_ConversationSpawnDataStore.end())
                return nullptr;

            return &l_It->second;
        }
        ConversationSpawnData& NewOrExistConversationSpawnData(uint32 p_Guid) { return m_ConversationSpawnDataStore[p_Guid]; }

        CreatureData const* GetCreatureData(uint32 guid) const
        {
            CreatureDataContainer::const_iterator itr = _creatureDataStore.find(guid);
            if (itr == _creatureDataStore.end()) return NULL;
            return &itr->second;
        }
        CreatureData& NewOrExistCreatureData(uint32 guid) { return _creatureDataStore[guid]; }
        void DeleteCreatureData(uint32 guid);
        uint64 GetLinkedRespawnGuid(uint64 guid) const
        {
            LinkedRespawnContainer::const_iterator itr = _linkedRespawnStore.find(guid);
            if (itr == _linkedRespawnStore.end()) return 0;
            return itr->second;
        }
        CreatureLocale const* GetCreatureLocale(uint32 entry) const
        {
            CreatureLocaleContainer::const_iterator itr = _creatureLocaleStore.find(entry);
            if (itr == _creatureLocaleStore.end()) return NULL;
            return &itr->second;
        }
        GameObjectLocale const* GetGameObjectLocale(uint32 entry) const
        {
            GameObjectLocaleContainer::const_iterator itr = _gameObjectLocaleStore.find(entry);
            if (itr == _gameObjectLocaleStore.end()) return NULL;
            return &itr->second;
        }
        QuestLocale const* GetQuestLocale(uint32 entry) const
        {
            QuestLocaleContainer::const_iterator itr = _questLocaleStore.find(entry);
            if (itr == _questLocaleStore.end()) return NULL;
            return &itr->second;
        }
        NpcTextLocale const* GetNpcTextLocale(uint32 entry) const
        {
            NpcTextLocaleContainer::const_iterator itr = _npcTextLocaleStore.find(entry);
            if (itr == _npcTextLocaleStore.end()) return NULL;
            return &itr->second;
        }
        PageTextLocale const* GetPageTextLocale(uint32 entry) const
        {
            PageTextLocaleContainer::const_iterator itr = _pageTextLocaleStore.find(entry);
            if (itr == _pageTextLocaleStore.end()) return NULL;
            return &itr->second;
        }
        GossipMenuItemsLocale const* GetGossipMenuItemsLocale(uint64 entry) const
        {
            GossipMenuItemsLocaleContainer::const_iterator itr = _gossipMenuItemsLocaleStore.find(entry);
            if (itr == _gossipMenuItemsLocaleStore.end()) return NULL;
            return &itr->second;
        }
        PointOfInterestLocale const* GetPointOfInterestLocale(uint32 poi_id) const
        {
            PointOfInterestLocaleContainer::const_iterator itr = _pointOfInterestLocaleStore.find(poi_id);
            if (itr == _pointOfInterestLocaleStore.end()) return NULL;
            return &itr->second;
        }
        DisplayChoiceLocale const* GetDisplayChoiceLocale(uint32 p_ResponseID) const
        {
            DisplayChoiceLocaleContainer::const_iterator l_Itr = _displayChoiceLocaleStore.find(p_ResponseID);

            if (l_Itr == _displayChoiceLocaleStore.end())
                return nullptr;

            return &l_Itr->second;
        }
        QuestObjectiveLocale const* GetQuestObjectiveLocale(uint32 objectiveId) const;

        GameObjectData const* GetGOData(uint32 guid) const
        {
            GameObjectDataContainer::const_iterator itr = _gameObjectDataStore.find(guid);
            if (itr == _gameObjectDataStore.end()) return NULL;
            return &itr->second;
        }
        GameObjectData& NewGOData(uint32 guid) { return _gameObjectDataStore[guid]; }
        void DeleteGOData(uint32 guid);

        TrinityStringLocale const* GetTrinityStringLocale(int32 entry) const
        {
            TrinityStringLocaleContainer::const_iterator itr = _trinityStringLocaleStore.find(entry);
            if (itr == _trinityStringLocaleStore.end()) return NULL;
            return &itr->second;
        }

        std::string GetFormatedTrinityString(uint32 p_Entry, int32 p_Locale, ...);

        const char* GetTrinityString(int32 entry, LocaleConstant locale_idx) const;
        const char* GetTrinityStringForDB2Locale(int32 entry) const { return GetTrinityString(entry, DBCLocaleIndex); }
        LocaleConstant GetDBCLocaleIndex() const { return DBCLocaleIndex; }
        void SetDBCLocaleIndex(LocaleConstant locale) { DBCLocaleIndex = locale; }
        std::vector<DisplayChoiceData> const* GetDisplayChoiceData(uint32 ChoiceID) const;

        const std::vector<ConversationData>*        GetConversationData(uint32 entry) const;
        const std::vector<ConversationCreature>*    GetConversationCreature(uint32 entry) const;
        const std::vector<ConversationActor>*       GetConversationActor(uint32 entry) const;

        void AddCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid, uint32 instance);
        void DeleteCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid);

        // grid objects
        void AddConversationToGrid(uint32 guid, ConversationSpawnData const* data);
        void AddCreatureToGrid(uint32 guid, CreatureData const* data);
        void RemoveCreatureFromGrid(uint32 guid, CreatureData const* data);
        void AddGameobjectToGrid(uint32 guid, GameObjectData const* data);
        void RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data);
        bool AddGOData(uint32 p_LowGuid, uint32 entry, uint32 map, float x, float y, float z, float o, uint32 spawntimedelay = 0, float rotation0 = 0, float rotation1 = 0, float rotation2 = 0, float rotation3 = 0);
        uint32 AddCreData(uint32 entry, uint32 team, uint32 map, float x, float y, float z, float o, uint32 spawntimedelay = 0);

        // reserved names
#ifndef CROSS
        void LoadReservedPlayersNames();
#endif /* not CROSS */
        bool IsReservedName(const std::string& name) const;

        // name with valid structure and symbols
        static uint8 CheckPlayerName(const std::string& name, bool create = false);
        static PetNameInvalidReason CheckPetName(const std::string& name);
        static bool IsValidCharterName(const std::string& name);

        static bool CheckDeclinedNames(std::wstring w_ownname, DeclinedName const& names);

        GameTele const* GetGameTele(uint32 id) const
        {
            GameTeleContainer::const_iterator itr = _gameTeleStore.find(id);
            if (itr == _gameTeleStore.end()) return NULL;
            return &itr->second;
        }
        GameTele const* GetGameTele(const std::string& name) const;
        GameTeleContainer const& GetGameTeleMap() const { return _gameTeleStore; }
        bool AddGameTele(GameTele& data);
        bool DeleteGameTele(const std::string& name);

        TrainerSpellData const* GetNpcTrainerSpells(uint32 entry) const
        {
            CacheTrainerSpellContainer::const_iterator  iter = _cacheTrainerSpellStore.find(entry);
            if (iter == _cacheTrainerSpellStore.end())
                return NULL;

            return &iter->second;
        }

        VendorItemData const* GetNpcVendorItemList(uint32 entry) const
        {
            CacheVendorItemContainer::const_iterator iter = _cacheVendorItemStore.find(entry);
            if (iter == _cacheVendorItemStore.end())
                return NULL;

            return &iter->second;
        }
        void AddVendorItem(uint32 entry, uint32 item, int32 maxcount, uint32 incrtime, uint32 extendedCost, uint8 type, bool persist = true, uint32 p_PlayerConditionID = 0); // for event
        bool RemoveVendorItem(uint32 entry, uint32 item, uint8 type, bool persist = true); // for event
        bool IsVendorItemValid(uint32 vendor_entry, uint32 id, int32 maxcount, uint32 ptime, uint32 ExtendedCost, uint8 type, Player* player = NULL, std::set<uint32>* skip_vendors = NULL, uint32 ORnpcflag = 0) const;

        void LoadScriptNames();
        ScriptNameContainer& GetScriptNames() { return _scriptNamesStore; }
        const char* GetScriptName(uint32 id) const { return id < _scriptNamesStore.size() ? _scriptNamesStore[id].c_str() : ""; }
        uint32 GetScriptId(const char* name);

        SpellClickInfoMapBounds GetSpellClickInfoMapBounds(uint32 creature_id) const
        {
            return SpellClickInfoMapBounds(_spellClickInfoStore.lower_bound(creature_id), _spellClickInfoStore.upper_bound(creature_id));
        }

        GossipMenusMapBounds GetGossipMenusMapBounds(uint32 uiMenuId) const
        {
            return GossipMenusMapBounds(_gossipMenusStore.lower_bound(uiMenuId), _gossipMenusStore.upper_bound(uiMenuId));
        }

        GossipMenusMapBoundsNonConst GetGossipMenusMapBoundsNonConst(uint32 uiMenuId)
        {
            return GossipMenusMapBoundsNonConst(_gossipMenusStore.lower_bound(uiMenuId), _gossipMenusStore.upper_bound(uiMenuId));
        }

        GossipMenuItemsMapBounds GetGossipMenuItemsMapBounds(uint32 uiMenuId) const
        {
            return GossipMenuItemsMapBounds(_gossipMenuItemsStore.lower_bound(uiMenuId), _gossipMenuItemsStore.upper_bound(uiMenuId));
        }
        GossipMenuItemsMapBoundsNonConst GetGossipMenuItemsMapBoundsNonConst(uint32 uiMenuId)
        {
            return GossipMenuItemsMapBoundsNonConst(_gossipMenuItemsStore.lower_bound(uiMenuId), _gossipMenuItemsStore.upper_bound(uiMenuId));
        }

        // for wintergrasp only
        GraveYardContainer GraveYardStore;

        static void AddLocaleString(const std::string& s, LocaleConstant locale, StringVector& data);
        static inline void GetLocaleString(const StringVector& data, int loc_idx, std::string& value)
        {
            if (data.size() > size_t(loc_idx) && !data[loc_idx].empty())
                value = data[loc_idx];
        }

        CharacterConversionMap FactionChange_Achievements;
        CharacterConversionMap FactionChange_Items;
        CharacterConversionMap FactionChange_Spells;
        CharacterConversionMap FactionChange_Reputation;
        CharacterConversionMap FactionChange_Titles;

        void LoadFactionChangeAchievements();
        void LoadFactionChangeItems();
        void LoadFactionChangeSpells();
        void LoadFactionChangeReputations();
        void LoadFactionChangeTitles();

        void LoadHotfixData(bool p_Reload = false);
        void LoadHotfixTableHashs();
        std::map<std::string, uint32> HotfixTableID;
        std::map<uint32, bool> HotfixTableIndexInfos;

        HotfixData const& GetHotfixData() const { return _hotfixData; }
        time_t GetHotfixDate(uint32 entry, uint32 type) const
        {
            time_t ret = 0;
            for (HotfixData::const_iterator itr = _hotfixData.begin(); itr != _hotfixData.end(); ++itr)
                if (itr->Entry == entry && itr->Type == type)
                    if (itr->Timestamp > ret)
                        ret = itr->Timestamp;

            return ret ? ret : time(NULL);
        }

        GuildChallengeRewardData const& GetGuildChallengeRewardData() const { return _challengeRewardData; }

        ///Temporaire pour la creation des Z, a remettre en private apres
        GameObjectDataContainer _gameObjectDataStore;

        uint64 GetCreatureGUIDByLootViewGUID(uint64 lootview)
        {
            return _lootViewGUID.find(lootview) != _lootViewGUID.end() ? _lootViewGUID[lootview] : 0;
        }

        void setLootViewGUID(uint64 lootview, uint64 creature)
        {
            _lootViewGUID[lootview] = creature;
        }

        void LoadAreaTriggerActionsAndData();
        void LoadAreaTriggerForces();

        AreaTriggerInfo const* GetAreaTriggerInfo(uint32 entry, uint32 p_SpellID = 0);
        AreaTriggerInfo const* GetAreaTriggerInfoByEntry(uint32 entry, uint32 p_SpellID = 0);
        AreaTriggerCircle const* GetAreaTriggerCircle(uint32 p_Entry) const;
        std::vector<AreaTriggerForce> const* GetAreaTriggerForce(uint32 AuraID) const;

        void LoadScenarioData();
        void LoadScenarioSpellData();

        AreaTriggerMoveSplines GetAreaTriggerMoveSplines(uint32 p_move_curve_id, uint32 p_path_id);
        uint32 GetAreaTriggerPathSize(uint32 p_move_curve_id);
        uint32 GetAreatriggerMoveDuration(SpellInfo const* p_SpellInfo);

        bool HasScenarioInMap(uint32 mapId) const
        {
            return _scenarioDataList.find(mapId) != _scenarioDataList.end();
        }

        ScenarioData const* GetScenarioOnMap(uint32 mapId, uint32 difficultyID , uint32 factionID , uint32 pl_class, uint32 p_DungeonDataID, uint32 p_ZoneID) const;

        std::vector<ScenarioSpellData> const* GetScenarioSpells(int32 ScenarioId) const
        {
            ScenarioDataSpellsStep::const_iterator itr = _scenarioDataSpellStep.find(ScenarioId);
            if (itr == _scenarioDataSpellStep.end())
                return NULL;

            return &itr->second;
        }

        CharacterTemplates const& GetCharacterTemplates() const { return m_CharacterTemplatesStore; }
        CharacterTemplate const* GetCharacterTemplate(uint32 p_ID) const;

        bool QuestObjectiveExists(uint32 objectiveId) const;
        uint32 GetQuestObjectiveQuestId(uint32 objectiveId) const;
        std::vector<QuestObjective> GetQuestObjectivesByType(uint8 p_Type)
        {
            return m_QuestObjectiveByType[p_Type];
        }

        uint32 GetNewGarrisonID()
        {
            return m_GarrisonID++;
        }

        uint32 GetNewGarrisonBuildingID()
        {
            return m_GarrisonBuildingID++;
        }

        uint32 GetNewGarrisonFollowerID()
        {
            return m_GarrisonFollowerID++;
        }

        uint32 GetNewGarrisonMissionID()
        {
            return m_GarrisonMissionID++;
        }

        uint32 GenerateNewVignetteGUID()
        {
            return m_HiVignetteGuid++;
        }

        uint32 GetNewGarrisonWorkOrderID()
        {
            return m_GarrisonWorkOrderID++;
        }

        uint32 GetNewStandaloneSceneInstanceID()
        {
            return m_StandaloneSceneInstanceID++;
        }

        uint32 GetNewGarrisonSiteBaseID()
        {
            return m_GarrisonSiteBaseID++;
        }

        uint32 GetNewScenarioID()
        {
            return m_ScenarioID++;
        }

        PlayerChoice const* GetPlayerChoice(int32 p_ChoiceID) const;

        PlayerChoiceLocale const* GetPlayerChoiceLocale(int32 p_ChoiceID) const
        {
            auto l_Itr = m_PlayerChoiceLocales.find(p_ChoiceID);
            if (l_Itr == m_PlayerChoiceLocales.end())
                return nullptr;

            return &l_Itr->second;
        }

        MS::Garrison::Interfaces::GarrisonSite* GetGarrisonSiteBase(uint32 p_ID) const
        {
            auto l_Itr = m_GarrisonSiteBaseContainer.find(p_ID);
            if (l_Itr == m_GarrisonSiteBaseContainer.end())
                return nullptr;

            return l_Itr->second;
        }

        void RegisterGarrisonSiteBase(uint32 p_Id, MS::Garrison::Interfaces::GarrisonSite* p_GarrisonSiteBase)
        {
            m_GarrisonSiteBaseContainer[p_Id] = p_GarrisonSiteBase;
        }

        ItemBonusGroup const* GetItemBonusGroup(uint32 p_GroupID) const
        {
            auto l_Find = m_ItemBonusGroupStore.find(p_GroupID);
            if (l_Find == m_ItemBonusGroupStore.end())
                return nullptr;

            return &l_Find->second;
        }

        std::vector<std::regex> const& GetForbiddenRegex(eChannelType p_ChannelType) const
        {
            return p_ChannelType < eChannelType::ChannelTypeMax ? m_ForbiddenRegex[p_ChannelType] : m_ForbiddenRegex[eChannelType::ChannelTypeMax];
        }

        bool IsInvalidSpell(uint32 p_SpellId)
        {
            if (std::find(m_SpellInvalid.begin(), m_SpellInvalid.end(), p_SpellId) != m_SpellInvalid.end())
                return true;

            return false;
        }

        std::multimap<uint32, CombatAIEventData> const& GetCombatAIEvents()
        {
            return m_CombatAIEvents;
        }

        uint32 GetLastCombatAIUpdateTime()
        {
            return m_LastCombatAIUpdate;
        }

        bool IsStolenSpell(uint32 p_SpellId)
        {
            if (std::find(m_SpellStolen.begin(), m_SpellStolen.end(), p_SpellId) != m_SpellStolen.end())
                return true;

            return false;
        }

        bool IsDisabledEncounter(uint32 p_EncounterID, uint32 p_DifficultyID) const
        {
            auto l_Iter = m_DisabledEncounters.find(std::make_pair(p_EncounterID, p_DifficultyID));
            if (l_Iter == m_DisabledEncounters.end())
                return false;

            return true;
        }

        bool IsGuildWhitelisted(uint32 p_MapID, uint64 p_GuildID) const
        {
            auto l_Iter = m_InstanceWhitelist.find(p_MapID);
            if (l_Iter == m_InstanceWhitelist.end())
                return false;

            if ((*l_Iter).second.find(p_GuildID) == (*l_Iter).second.end())
                return false;

            return true;
        }

        bool IsDisabledMap(uint32 p_MapID, uint32 p_DifficultyID) const
        {
            /// Map not disabled
            auto l_Iter = m_DisabledMaps.find(p_MapID);
            if (l_Iter == m_DisabledMaps.end())
                return false;

            /// Difficulty not disabled
            if (!((1 << p_DifficultyID) & (*l_Iter).second))
                return false;

            return true;
        }

        ItemContext GetItemContextOverride(uint32 p_MapID, uint32 p_DifficultyID) const
        {
            /// Map not disabled
            auto const& l_Iter = m_DisabledMapsItemContextOverride.find(p_MapID);
            if (l_Iter == m_DisabledMapsItemContextOverride.end())
                return ItemContext::None;

            /// Difficulty not disabled
            auto const& l_DiffIter = l_Iter->second.find(p_DifficultyID);
            if (l_DiffIter == l_Iter->second.end())
                return ItemContext::None;

            return ItemContext(l_DiffIter->second);
        }

        ItemExtendedCostAddon const* GetItemExtendedCostAddon(uint32 p_Entry)
        {
            auto l_Itr = m_ItemExtendedCostAddon.find(p_Entry);
            if (l_Itr == m_ItemExtendedCostAddon.end())
                return nullptr;

            return &l_Itr->second;
        }

        AreaTriggerStruct const* GetGameObjectAreaTriggerTeleport(uint32 p_Entry, uint32 p_MapID, uint32 p_ZoneID = 0) const
        {
            auto l_Itr = m_GameobjectAreaTriggerStore.find(std::make_pair(p_Entry, std::make_pair(p_MapID, p_ZoneID)));
            if (l_Itr == m_GameobjectAreaTriggerStore.end())
                return nullptr;

            return &l_Itr->second;
        }

        void LoadCustomInstanceZones();
        CustomInstanceZones const* GetCustomInstanceZones(uint32 p_Zone) const
        {
            auto l_Itr = m_CustomInstanceZones.find(p_Zone);
            if (l_Itr == m_CustomInstanceZones.end())
                return nullptr;

            return &l_Itr->second;
        }

        std::vector<CosmeticEntry> const GetActiveCosmetics(eCosmetics p_type) const
        {
            std::vector<CosmeticEntry> l_ActiveCosmetics;
            for (CosmeticEntry l_Cosmetic : m_Cosmetics[p_type])
            {
                if (l_Cosmetic.Active == 1)
                    l_ActiveCosmetics.push_back(l_Cosmetic);
            }

            return l_ActiveCosmetics;
        }

        std::vector<uint32> const GetAllCosmeticsID(eCosmetics p_type) const
        {
            return m_CosmeticsID[p_type];
        }

        CosmeticEntry const* GetCosmeticByEntry(eCosmetics p_Type, uint32 p_ID)
        {
            if (m_CosmeticsByEntry.find(MAKE_PAIR64(p_Type, p_ID)) != m_CosmeticsByEntry.end())
                return &m_CosmeticsByEntry[MAKE_PAIR64(p_Type, p_ID)];

            return nullptr;
        }
        std::string GetCosmeticName(Player* p_Player, eCosmetics p_Cosmetic, uint32 p_Id);

        bool ShouldntBeSaved(uint32 p_SpellId)
        {
            return m_spellAuraNotSavedStore.find(p_SpellId) != m_spellAuraNotSavedStore.end();
        }

        uint32 GetGUIDByLinkedId(std::string p_LinkedId) const
        {
            auto l_Itr = m_LinkedIdXGuid.find(p_LinkedId);
            if (l_Itr == m_LinkedIdXGuid.end())
                return 0;

            return l_Itr->second;
        }

        MultimapBounds_uint32 const GetAreasForCreature(uint32 p_CreatureID)
        {
            return m_CreatureIdXAreaId.equal_range(p_CreatureID);
        }

        MultimapBounds_uint32 const GetAreasForGameObject(uint32 p_GameObjectID)
        {
            return m_GameObjectIdXAreaId.equal_range(p_GameObjectID);
        }

        void ClearInvasionsQuestsAreaContainers()
        {
            m_CreatureIdXAreaId.clear();
            m_GameObjectIdXAreaId.clear();
        }

        std::vector<CosmeticEntry> const& GetLootBoxCosmeticPool()
        {
            if (m_LootBoxCosmeticPool.size())
                return m_LootBoxCosmeticPool;

            for (uint32 l_Type = eCosmetics::CosmeticBegin; l_Type < eCosmetics::MaxCosmetic; ++l_Type)
            {
                for (CosmeticEntry l_Cosmetic : m_Cosmetics[l_Type])
                {
                    if (l_Cosmetic.Active < 2)
                        continue;

                    for (uint8 l_I = 0; l_I < l_Cosmetic.Active - 1; ++l_I)
                        m_LootBoxCosmeticPool.push_back(l_Cosmetic);
                }
            }

            return m_LootBoxCosmeticPool;
        }

        bool IsInWPDebug() const { return m_WPDebug; }
        void SetWPDebug(bool p_Apply) { m_WPDebug = p_Apply; }

        static uint32 ChangeTitleIdForGender(uint32 p_Id, uint8 p_Gender)
        {
            if (p_Id == 415 && p_Gender == 1) return 416; ///< Talon King
            if (p_Id == 416 && p_Gender == 0) return 415; ///< Talon Queen
            if (p_Id == 138 && p_Gender == 1) return 137; ///< Patron
            if (p_Id == 137 && p_Gender == 0) return 138; ///< Matron

            return p_Id;
        }

        std::vector<std::string> const& GetAddonFunctions();
        std::vector<std::string> const& GetLocalizedAddonFunctions(LocaleConstant p_Locale);
        void AddLocalizedAddonFunction(LocaleConstant p_Locale, std::string p_Function);

        uint32 GetLastChangelogDate() { return m_LastChangelogDate; }
        std::vector<std::string> const& GetLiveChangelogFunctions();
        std::vector<std::string> const& GetNewsFeedFunctions(LocaleConstant p_Locale);
        std::vector<std::string> const& GetAlertButtonFunctions(LocaleConstant p_Locale);

        std::unordered_map<uint64, std::vector<CosmeticEventEntry>> const& GetCosmeticEventDatas() const
        {
            return m_CosmeticEvents;
        }

        bool CheckFakeColorProtection(std::string const &p_Msg, ColorFakeProtectionTypes colorProtection) const;

        std::set<Quest const*> const* GetQuestTask(uint32 areaId) const;
        bool IsAreaTaskQuest(uint32 questId) const;

    private:

#ifndef CROSS
        std::atomic<uint32> m_HighItemGuid;
        std::atomic<uint32> m_MailId;
        std::atomic<uint32> m_PetNumber;
        std::atomic<uint64> m_EquipmentSetGuid;
#endif

        // first free id for selected id type
        std::atomic<unsigned int>  _auctionId;
        std::atomic<unsigned long> _equipmentSetGuid;
        std::atomic<unsigned int>  _itemTextId;
        std::atomic<unsigned int>  _mailId;
        std::atomic<unsigned int>  _hiPetNumber;
        std::atomic<unsigned long> _voidItemId;

        // first free low guid for selected guid type
        std::atomic<unsigned int> _hiCharGuid;
        std::atomic<unsigned int> _hiCreatureGuid;
        std::atomic<unsigned int> _hiPetGuid;
        std::atomic<unsigned int> _hiVehicleGuid;
        std::atomic<unsigned int> _hiGoGuid;
        std::atomic<unsigned int> _hiDoGuid;
        std::atomic<unsigned int> _hiCorpseGuid;
        std::atomic<unsigned int> _hiAreaTriggerGuid;
        std::atomic<unsigned int> _hiMoTransGuid;
        std::atomic<unsigned int> m_GarrisonID;
        std::atomic<unsigned int> m_GarrisonBuildingID;
        std::atomic<unsigned int> m_GarrisonFollowerID;
        std::atomic<unsigned int> m_GarrisonMissionID;
        std::atomic<unsigned int> m_GarrisonWorkOrderID;
        std::atomic<unsigned int> m_HiVignetteGuid;
        std::atomic<unsigned int> m_StandaloneSceneInstanceID;
        std::atomic<unsigned int> m_HighConversationGuid;
        std::atomic<unsigned int> m_HighEventObjectGuid;
        std::atomic<unsigned int> m_GarrisonSiteBaseID;
        std::atomic<unsigned int> m_ScenarioID;
        std::atomic<unsigned long> m_EncounterAttemptID;


#ifdef CROSS
        std::atomic<unsigned int> m_HighItemGuid;
#endif

        ACE_Based::LockedMap<uint32, MS::Garrison::Interfaces::GarrisonSite*> m_GarrisonSiteBaseContainer;

        QuestMap _questTemplates;
        std::vector<uint32> m_ScriptedQuests;
        QuestObjectiveLookupMap m_questObjectiveLookup;
        std::vector<uint32> m_IgnoredQuestObjectives;

        typedef std::unordered_map<uint32, GossipText> GossipTextContainer;
        typedef std::unordered_map<uint32, uint32> QuestAreaTriggerContainer;
        typedef std::set<uint32> TavernAreaTriggerContainer;
        typedef std::set<uint32> GameObjectForQuestContainer;
        typedef std::multimap<uint32, AreaTriggerInfo> AreaTriggerInfoMap;
        typedef std::map<uint32, AreaTriggerCircle> AreaTriggerCircleMap;

        QuestAreaTriggerContainer _questAreaTriggerStore;
        TavernAreaTriggerContainer _tavernAreaTriggerStore;
        GameObjectForQuestContainer _gameObjectForQuestStore;
        GossipTextContainer _gossipTextStore;
        AreaTriggerContainer _areaTriggerStore;
        AreaTriggerScriptContainer _areaTriggerScriptStore;
        AccessRequirementContainer _accessRequirementStore;
        LFRAccessRequirements m_LFRAccessRequirements;
        DungeonEncounterContainer _dungeonEncounterStore;
        GameobjectAreaTriggerContainer m_GameobjectAreaTriggerStore;

        RepRewardRateContainer _repRewardRateStore;
        RepOnKillContainer _repOnKillStore;
        RepSpilloverTemplateContainer _repSpilloverTemplateStore;
        CurOnKillContainer _curOnKillStore;
        CurOnKillContainer m_PersonnalCurrOnKillStore;

        GossipMenusContainer _gossipMenusStore;
        GossipMenuItemsContainer _gossipMenuItemsStore;
        PointOfInterestContainer _pointsOfInterestStore;

        QuestPOIContainer _questPOIStore;
        ScenarioPOIContainer _scenarioPOIStore;

        QuestRelations _goQuestRelations;
        QuestRelations _goQuestInvolvedRelations;
        QuestRelations _creatureQuestRelations;
        QuestRelations _creatureQuestInvolvedRelations;

        //character reserved names
        typedef std::set<std::wstring> ReservedNamesContainer;
        ReservedNamesContainer _reservedNamesStore;

        GameTeleContainer _gameTeleStore;

        ScriptNameContainer _scriptNamesStore;

        SpellClickInfoContainer _spellClickInfoStore;

        SpellScriptsContainer _spellScriptsStore;

        VehicleAccessoryContainer _vehicleTemplateAccessoryStore;

        LocaleConstant DBCLocaleIndex;

        PageTextContainer _pageTextStore;
        InstanceTemplateContainer _instanceTemplateStore;

        PhaseDefinitionStore _PhaseDefinitionStore;
        SpellPhaseStore _SpellPhaseStore;

        uint32 _skipUpdateCount;
        ACE_Based::LockedMap<uint64, uint64> _lootViewGUID;

        AreaTriggerInfoMap _areaTriggerData;
        AreaTriggerInfoMap _areaTriggerDataByEntry;
        AreaTriggerMoveSplinesContainer m_AreaTriggerMoveSplines;
        AreaTriggerCircleMap m_AreaTriggerCircleData;

        ScenarioDataMap _scenarioData;
        ScenarioDataListMap _scenarioDataList;
        ScenarioDataSpellsStep _scenarioDataSpellStep;

        ResearchZoneMap _researchZoneMap;
        ResearchLootVector _researchLoot;

        std::vector<GarrisonPlotBuildingContent> m_GarrisonPlotBuildingContents;
        std::map<uint32, std::vector<GarrisonMissionReward>> m_GarrisonMissionRewards;
        NpcRecipesContainer _NpcRecipesConditions;

        ItemBonusGroupMap m_ItemBonusGroupStore;

        MailRewardByLevelContainer _mailLevelReward;
        MailRewardLocaleContainer _mailRewardLocale;

        CreatureLootBonusMap m_CreatureBonusLootData;

        CustomInstanceZonesMap m_CustomInstanceZones;

        QuestAreaTaskMap _questAreaTaskStore;
        QuestAreaTaskQuests _questAreaTaskQuests;

    private:
        CharacterTemplates m_CharacterTemplatesStore;
        void LoadScripts(ScriptsType type);
        void CheckScripts(ScriptsType type, std::set<int32>& ids);
        void LoadQuestRelationsHelper(QuestRelations& map, std::string table, bool starter, bool go);
        void PlayerCreateInfoAddItemHelper(uint32 race_, uint32 class_, uint32 itemId, int32 count);

        CreatureBaseStatsContainer _creatureBaseStatsStore;
        CreatureGroupSizeStatsContainer m_CreatureGroupSizeStore;
        CreatureDiffHealthContainer m_CreatureDiffHealthStore;

        typedef std::map<uint32, float> CreatureScalingPctContainer;
        CreatureScalingPctContainer m_CreatureScalingPctStore;

        typedef std::map<uint32, PetStatInfo> PetStatInfoContainer;
        PetStatInfoContainer m_PetInfoStore;

        void BuildPlayerLevelInfo(uint8 race, uint8 class_, uint8 level, PlayerLevelInfo* plinfo) const;

        PlayerInfo _playerInfo[MAX_RACES][MAX_CLASSES];

        typedef std::vector<uint32> PlayerXPperLevel;       // [level]
        PlayerXPperLevel _playerXPperLevel;

        typedef std::map<uint32, uint32> BaseXPContainer;          // [area level][base xp]
        BaseXPContainer _baseXPTable;

        typedef std::map<uint32, int32> FishingBaseSkillContainer; // [areaId][base skill level]
        FishingBaseSkillContainer _fishingBaseForAreaStore;

        std::unordered_map<uint32, SkillTiersEntry> _skillTiers;

        typedef std::map<uint32, StringVector> HalfNameContainer;
        HalfNameContainer _petHalfName0;
        HalfNameContainer _petHalfName1;

        MapObjectGuids _mapObjectGuidsStore;
        CreatureDataContainer _creatureDataStore;
        uint32 m_LastCreatureTemplateEntry;
        CreatureTemplateContainer m_CreatureTemplateStore;
        EventObjectTemplateContainer m_EventObjectTemplateMap;
        EventObjectDataContainer m_EventObjectData;
        AreaTriggerDataContainer m_AreaTriggerData;
        ConversationDataContainer m_ConversationSpawnDataStore;
        CreatureModelContainer _creatureModelStore;
        std::unordered_map<std::string, CreatureAddon> _creatureAddonStoreByLinked;
        std::unordered_map<uint32, CreatureAddon> _creatureAddonStoreByGuid;
        CreatureAddonContainer _creatureTemplateAddonStore;
        EquipmentInfoContainer _equipmentInfoStore;
        LinkedRespawnContainer _linkedRespawnStore;
        CreatureLocaleContainer _creatureLocaleStore;
        DisplayChoiceLocaleContainer _displayChoiceLocaleStore;
        //GameObjectDataContainer _gameObjectDataStore;
        GameObjectLocaleContainer _gameObjectLocaleStore;
        GameObjectTemplateContainer _gameObjectTemplateStore;
        /// Stores temp summon data grouped by summoner's entry, summoner's type and group id
        TempSummonDataContainer _tempSummonDataStore;

        ItemTemplateContainer _itemTemplateStore;
        ItemTemplateListBySpec _regularPvpGearRewards[PvpSeasons::MaxPvpSeason];
        ItemTemplateListBySpec _ratedPvpGearRewards[PvpSeasons::MaxPvpSeason];
        ItemTemplateListBySpec _elitePvpGearRewards[PvpSeasons::MaxPvpSeason];
        ItemTemplateListBySpec g_LegendariesAvailable;
        ItemTemplateListBySpec g_RelinquishedLegendariesAvailable;
        ItemTemplateListBySpec g_ClassesLegendariesAvailable;
        std::set<uint32> _LegionLegendariesSet;
        std::set<uint32> _LegionRelinquishedLegendaries;

        QuestLocaleContainer _questLocaleStore;
        NpcTextLocaleContainer _npcTextLocaleStore;
        PageTextLocaleContainer _pageTextLocaleStore;
        TrinityStringLocaleContainer _trinityStringLocaleStore;
        GossipMenuItemsLocaleContainer _gossipMenuItemsLocaleStore;
        PointOfInterestLocaleContainer _pointOfInterestLocaleStore;
        BattlePetTemplateContainer _battlePetTemplateStore;
        BattlePetNpcTeamMembers m_BattlePetNpcTeamMembers;
        QuestObjectiveLocaleContainer m_questObjectiveLocaleStore;

        std::map<uint8, std::vector<QuestObjective>> m_QuestObjectiveByType;

        CacheVendorItemContainer _cacheVendorItemStore;
        CacheTrainerSpellContainer _cacheTrainerSpellStore;

        std::set<uint32> _difficultyEntries[Difficulty::MaxDifficulties - 1]; // already loaded difficulty 1 value in creatures, used in CheckCreatureTemplate
        std::set<uint32> _hasDifficultyEntries[Difficulty::MaxDifficulties - 1]; // already loaded creatures with difficulty 1 values, used in CheckCreatureTemplate
        std::unordered_set<uint32> _creaturesOrigEntry;
        std::unordered_set<uint32> _creaturesDiffEntry;

        std::list<uint32> m_SpellInvalid;
        std::list<uint32> m_SpellStolen;

        std::set<uint32> m_spellAuraNotSavedStore;

        enum CreatureLinkedRespawnType
        {
            CREATURE_TO_CREATURE,
            CREATURE_TO_GO,         // Creature is dependant on GO
            GO_TO_GO,
            GO_TO_CREATURE,         // GO is dependant on creature
        };
        HotfixData _hotfixData;
        GuildChallengeRewardData _challengeRewardData;

        ConversationDataMap     m_ConversationData;
        ConversationCreatureMap m_ConversationCreature;
        ConversationActorMap    m_ConversationActor;

        DisplayChoiceMap _displayChoiceMap;
        std::set<std::pair<uint32, uint32>> m_DisabledEncounters;
        std::map<uint32, uint32> m_DisabledMaps;
        std::map<uint32, std::map<uint32, uint8>> m_DisabledMapsItemContextOverride;
        std::map<uint32, std::set<uint64>> m_InstanceWhitelist;

        ItemExtendedCostAddonMap m_ItemExtendedCostAddon;

        AreaTriggerForceMap _areaTriggerForceMap;

        std::vector<CosmeticEntry>      m_Cosmetics[eCosmetics::MaxCosmetic];
        std::vector<uint32>             m_CosmeticsID[eCosmetics::MaxCosmetic];
        std::map<uint64, CosmeticEntry> m_CosmeticsByEntry;
        std::vector<CosmeticEntry>      m_LootBoxCosmeticPool;

        std::vector<std::regex>         m_ForbiddenRegex[eChannelType::ChannelTypeSize];

        std::multimap<uint32, CombatAIEventData> m_CombatAIEvents;
        uint32                          m_LastCombatAIUpdate;

        std::map<std::string, uint32>        m_LinkedIdXGuid;

        std::multimap<uint32, uint32>   m_CreatureIdXAreaId;
        std::multimap<uint32, uint32>   m_GameObjectIdXAreaId;

        std::unordered_map<int32 /*choiceId*/, PlayerChoice> m_PlayerChoices;
        std::unordered_map<int32, PlayerChoiceLocale> m_PlayerChoiceLocales;

        bool                            m_WPDebug;

        std::vector<std::string>                            m_AddonFunctions;
        std::map<LocaleConstant, std::vector<std::string>>  m_LocaleAddonFunctions;

        std::vector<std::string>                            m_ChangelogAddonFunctions;
        uint32                                              m_LastChangelogDate;
        std::map<LocaleConstant, std::vector<std::string>>  m_NewsFeedAddonFunctions;
        std::map<LocaleConstant, std::vector<std::string>>  m_AlertButtonFunctions;

        std::unordered_map<uint64, std::vector<CosmeticEventEntry>> m_CosmeticEvents;

        PvpSeasonRewardsPerBracket m_PvPSeasonRewards;
};

#define sObjectMgr ACE_Singleton<ObjectMgr, ACE_Null_Mutex>::instance()

// scripting access functions
bool LoadTrinityStrings(char const* table, int32 start_value = MAX_CREATURE_AI_TEXT_STRING_ID, int32 end_value = std::numeric_limits<int32>::min());

#endif
