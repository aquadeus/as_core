////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef QUEST_H
#define QUEST_H

#include "DB2Structure.h"
#include "Define.h"
#include "DatabaseEnv.h"
#include "SharedDefines.h"
#include "WorldPacket.h"
#include "Common.h"

class Player;

class ObjectMgr;

#define MAX_QUEST_LOG_SIZE 50           ///< Last update 6.0.3 19116, idk if blizzard has unlock to 50 quest on live servers, but somes quest packet client-side & updatefield are update to 50

#define QUEST_SOURCE_ITEM_IDS_COUNT 4
#define QUEST_REWARD_CHOICES_COUNT 6
#define QUEST_REWARDS_COUNT 4
#define QUEST_DEPLINK_COUNT 10
#define QUEST_REPUTATIONS_COUNT 5
#define QUEST_EMOTE_COUNT 4
#define QUEST_PVP_KILL_SLOT 0
#define QUEST_REWARD_CURRENCY_COUNT 4
#define QUEST_SOURCE_ITEM_COUNT 1
#define QUEST_REWARD_SPELL_COUNT 3

enum QuestFailedReasons
{
    QUEST_ERR_NONE                    = 0,
    QUEST_ERR_FAILED_LOW_LEVEL        = 1,  ///< "You are not high enough level for that quest.""
    QUEST_ERR_FAILED_WRONG_RACE       = 6,  ///< "That quest is not available to your race."
    QUEST_ERR_ALREADY_DONE            = 7,  ///< "You have completed that daily quest today."
    QUEST_ERR_ONLY_ONE_TIMED          = 12, ///< "You can only be on one timed quest at a time"
    QUEST_ERR_ALREADY_ON1             = 13, ///< "You are already on that quest"
    QUEST_ERR_FAILED_EXPANSION        = 16, ///< "This quest requires an expansion enabled account."
    QUEST_ERR_ALREADY_ON2             = 18, ///< "You are already on that quest"
    QUEST_ERR_FAILED_MISSING_ITEMS    = 21, ///< "You don't have the required items with you.  Check storage."
    QUEST_ERR_FAILED_NOT_ENOUGH_MONEY = 23, ///< "You don't have enough money for that quest"
    QUEST_ERR_FAILED_CAIS             = 24, ///< "You cannot complete quests once you have reached tired time"
    QUEST_ERR_ALREADY_DONE_DAILY      = 26, ///< "You have completed that daily quest today."
    QUEST_ERR_FAILED_SPELL            = 28, ///< "You haven't learned the required spell."
    QUEST_ERR_HAS_IN_PROGRESS         = 30  ///< "Progress Bar objective not completed"
};

enum QuestShareMessages
{
    QUEST_PARTY_MSG_SHARING_QUEST           = 0,
    QUEST_PARTY_MSG_CANT_TAKE_QUEST         = 1,
    QUEST_PARTY_MSG_ACCEPT_QUEST            = 2,
    QUEST_PARTY_MSG_DECLINE_QUEST           = 3,
    QUEST_PARTY_MSG_BUSY                    = 4,
    QUEST_PARTY_MSG_DEAD                    = 5,
    QUEST_PARTY_MSG_LOG_FULL                = 6,
    QUEST_PARTY_MSG_HAVE_QUEST              = 7,
    QUEST_PARTY_MSG_FINISH_QUEST            = 8,
    QUEST_PARTY_MSG_NOT_DAILY               = 9,
    QUEST_PARTY_MSG_SHARING_TIMER_EXPIRED   = 10,
    QUEST_PARTY_MSG_NOT_IN_PARTY            = 11,
    QUEST_PARTY_MSG_DIFFERENT_SERVER_DAILY  = 12,
    QUEST_PARTY_MSG_PUSH_NOT_ALLOWED        = 13,
};

enum QuestStatus
{
    QUEST_STATUS_NONE           = 0,
    QUEST_STATUS_COMPLETE       = 1,
    //QUEST_STATUS_UNAVAILABLE    = 2,
    QUEST_STATUS_INCOMPLETE     = 3,
    //QUEST_STATUS_AVAILABLE      = 4,
    QUEST_STATUS_FAILED         = 5,
    QUEST_STATUS_REWARDED       = 6,        // Not used in DB
    MAX_QUEST_STATUS
};

enum QuestGiverStatus
{
    DIALOG_STATUS_NONE                     = 0x0000,
    DIALOG_STATUS_UNK                      = 0x0001,
    DIALOG_STATUS_UNAVAILABLE              = 0x0002,
    DIALOG_STATUS_LOW_LEVEL_AVAILABLE      = 0x0004,
    DIALOG_STATUS_LOW_LEVEL_REWARD_REP     = 0x0008,
    DIALOG_STATUS_LOW_LEVEL_AVAILABLE_REP  = 0x0010,
    DIALOG_STATUS_INCOMPLETE               = 0x0020,
    DIALOG_STATUS_REWARD_REP               = 0x0040,
    DIALOG_STATUS_AVAILABLE_REP            = 0x0080,
    DIALOG_STATUS_AVAILABLE                = 0x0100,
    DIALOG_STATUS_REWARD2                  = 0x0200, // no yellow dot on minimap
    DIALOG_STATUS_REWARD                   = 0x0400, // yellow dot on minimap
    DIALOG_STATUS_LEGENDARY_OFFER          = 0x0800, ///< Orange "!"
    DIALOG_STATUS_LEGENDARY_REWARD         = 0x1000, ///< Orange "?"

     /// Custom value meaning that script call did not return any valid quest status
    DIALOG_STATUS_SCRIPTED_NO_STATUS        = 0x8000
};

enum QuestFlags : uint32
{
    // Flags used at server and sent to client
    QUEST_FLAGS_NONE                         = 0x00000000,
    QUEST_FLAGS_STAY_ALIVE                   = 0x00000001,                // Not used currently
    QUEST_FLAGS_PARTY_ACCEPT                 = 0x00000002,                // Not used currently. If player in party, all players that can accept this quest will receive confirmation box to accept quest CMSG_QUEST_CONFIRM_ACCEPT/SMSG_QUEST_CONFIRM_ACCEPT
    QUEST_FLAGS_EXPLORATION                  = 0x00000004,                // Not used currently
    QUEST_FLAGS_SHARABLE                     = 0x00000008,                // Can be shared: Player::CanShareQuest()
    QUEST_FLAG_HAS_CONDITION                 = 0x00000010,                // Not used currently
    QUEST_FLAGS_HIDE_REWARD_POI              = 0x00000020,                // Not used currently
    QUEST_FLAGS_RAID                         = 0x00000040,                // Not used currently
    QUEST_FLAGS_EXPANSION_ONLY               = 0x00000080,                // Not used currently: Deprecated
    QUEST_FLAGS_NO_MONEY_FROM_EXPERIENCE     = 0x00000100,                // Not used currently: @TODO
    QUEST_FLAGS_HIDDEN_REWARDS               = 0x00000200,                // Items and money rewarded only sent in SMSG_QUESTGIVER_OFFER_REWARD (not in SMSG_QUESTGIVER_QUEST_DETAILS or in client quest log(SMSG_QUEST_QUERY_RESPONSE))
    QUEST_FLAGS_AUTO_REWARDED                = 0x00000400,                // These quests are automatically rewarded on quest complete and they will never appear in quest log client side. @TODO: Check it ! (Tracking on WPP)
    QUEST_FLAGS_DEPRICATE_REPUTATION         = 0x00000800,                // Not used currently: Deprecated
    QUEST_FLAGS_DAILY                        = 0x00001000,                // Used to know quest is Daily one
    QUEST_FLAGS_FLAG_FOR_PVP                 = 0x00002000,                // Not used currently
    QUEST_FLAGS_UNAVAILABLE                  = 0x00004000,                // Used on quests that are not generically available
    QUEST_FLAGS_WEEKLY                       = 0x00008000,
    QUEST_FLAGS_AUTO_SUBMIT                  = 0x00010000,                // Quests with this flag player submit automatically by special button in player gui, might not work properly atm
    QUEST_FLAGS_DISPLAY_ITEM_IN_TRACKER      = 0x00020000,
    QUEST_FLAGS_DISABLE_COMPLETION_TEXT      = 0x00040000,                // use Objective text as Complete text
    QUEST_FLAGS_AUTO_ACCEPT                  = 0x00080000,                // The client recognizes this flag as auto-accept. Quest is automatically added to questlog, without offering gossip item menu to accept it manually.
    QUEST_FLAGS_AUTOCOMPLETE                 = 0x00100000,                // auto complete if the quest has at least 1 objective. Otherwise show autosubmit UI.
    QUEST_FLAGS_AUTO_TAKE                    = 0x00200000,                // Automatically suggestion of accepting quest. Not from npc.
    QUEST_FLAGS_UPDATE_PHASE_SHIFT           = 0x00400000,                // Not used currently
    QUEST_FLAGS_SOR_WHITE_LIST               = 0x00800000,                // Not used currently
    QUEST_FLAGS_LAUNCH_GOSSIP_COMPLETE       = 0x01000000,                // Not used currently @TODO
    QUEST_FLAGS_REMOVE_EXTRA_GET_ITEMS       = 0x02000000,                // Not used currently
    QUEST_FLAGS_HIDE_UNTIL_DISCOVERED        = 0x04000000,                // Not used currently
    QUEST_FLAGS_PORTRAIT_IN_QUEST_LOG        = 0x08000000,                // Not used currently
    QUEST_FLAGS_SHOW_ITEM_WHEN_COMPLETED     = 0x10000000,                // Not used currently
    QUEST_FLAGS_LAUNCH_GOSSIP_ACCEPT         = 0x20000000,                // Not used currently @TODO
    QUEST_FLAGS_ITEMS_GLOW_WHEN_DONE         = 0x40000000,                // Not used currently @TODO
    QUEST_FLAGS_FAIL_ON_LOGOUT               = 0x80000000                 // Not used currenlty @TODO
};

// last checked in 19802
enum QuestFlagsEx
{
    QUEST_FLAGS_EX_NONE                                                 = 0x0000000,
    QUEST_FLAGS_EX_KEEP_ADDITIONAL_ITEMS                                = 0x0000001,
    QUEST_FLAGS_EX_SUPPRESS_GOSSIP_COMPLETE                             = 0x0000002,
    QUEST_FLAGS_EX_SUPPRESS_GOSSIP_ACCEPT                               = 0x0000004,
    QUEST_FLAGS_EX_DISALLOW_PLAYER_AS_QUESTGIVER                        = 0x0000008,
    QUEST_FLAGS_EX_DISPLAY_CLASS_CHOICE_REWARDS                         = 0x0000010,
    QUEST_FLAGS_EX_DISPLAY_SPEC_CHOICE_REWARDS                          = 0x0000020,
    QUEST_FLAGS_EX_REMOVE_FROM_LOG_ON_PERIDOIC_RESET                    = 0x0000040,
    QUEST_FLAGS_EX_ACCOUNT_LEVEL_QUEST                                  = 0x0000080,
    QUEST_FLAGS_EX_LEGENDARY_QUEST                                      = 0x0000100,
    QUEST_FLAGS_EX_NO_GUILD_XP                                          = 0x0000200,
    QUEST_FLAGS_EX_RESET_CACHE_ON_ACCEPT                                = 0x0000400,
    QUEST_FLAGS_EX_NO_ABANDON_ONCE_ANY_OBJECTIVE_COMPLETE               = 0x0000800,
    QUEST_FLAGS_EX_RECAST_ACCEPT_SPELL_ON_LOGIN                         = 0x0001000,
    QUEST_FLAGS_EX_UPDATE_ZONE_AURAS                                    = 0x0002000,
    QUEST_FLAGS_EX_NO_CREDIT_FOR_PROXY                                  = 0x0004000,
    QUEST_FLAGS_EX_DISPLAY_AS_DAILY_QUEST                               = 0x0008000,
    QUEST_FLAGS_EX_PART_OF_QUEST_LINE                                   = 0x0010000,
    QUEST_FLAGS_EX_QUEST_FOR_INTERNAL_BUILDS_ONLY                       = 0x0020000,
    QUEST_FLAGS_EX_SUPPRESS_SPELL_LEARN_TEXT_LINE                       = 0x0040000,
    QUEST_FLAGS_EX_DISPLAY_HEADER_AS_OBJECTIVE_FOR_TASKS                = 0x0080000,
    QUEST_FLAGS_EX_GARRISON_NON_OWNERS_ALLOWED                          = 0x0100000,
    QUEST_FLAGS_EX_REMOVE_QUEST_ON_WEEKLY_RESET                         = 0x0200000,
    QUEST_FLAGS_EX_SUPPRESS_FAREWELL_AUDIO_AFTER_QUEST_ACCEPT           = 0x0400000,
    QUEST_FLAGS_EX_REWARDS_BYPASS_WEEKLY_CAPS_AND_SEASON_TOTAL          = 0x0800000,
    QUEST_FLAGS_EX_CLEAR_PROGRESS_OF_CRITERIA_TREE_OBJECTIVES_ON_ACCEPT = 0x1000000
};

enum QuestSpecialFlags
{
    QUEST_SPECIAL_FLAGS_NONE                 = 0x000,
    // Trinity flags for set SpecialFlags in DB if required but used only at server
    QUEST_SPECIAL_FLAGS_REPEATABLE           = 0x001,
    QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT = 0x002, // if required area explore, spell SPELL_EFFECT_QUEST_COMPLETE casting, table `*_script` command SCRIPT_COMMAND_QUEST_EXPLORED use, set from script)
    QUEST_SPECIAL_FLAGS_AUTO_ACCEPT          = 0x004, // quest is to be auto-accepted.
    QUEST_SPECIAL_FLAGS_DF_QUEST             = 0x008, // quest is used by Dungeon Finder.
    QUEST_SPECIAL_FLAGS_MONTHLY              = 0x010, // Set by 16 in SpecialFlags in DB if the quest is reset at the begining of the month
    QUEST_SPECIAL_FLAGS_CAST                 = 0x020, // Set by 32 in SpecialFlags in DB if the quest requires RequiredOrNpcGo killcredit but NOT kill (a spell cast)
    QUEST_SPECIAL_FLAGS_DYNAMIC_ITEM_REWARD  = 0x040, // Set by 64 in SpecialFlags in DB if the quest has dynamic item reward

    // room for more custom flags

    QUEST_SPECIAL_FLAGS_DB_ALLOWED = QUEST_SPECIAL_FLAGS_REPEATABLE | QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT | QUEST_SPECIAL_FLAGS_AUTO_ACCEPT | QUEST_SPECIAL_FLAGS_DF_QUEST | QUEST_SPECIAL_FLAGS_MONTHLY | QUEST_SPECIAL_FLAGS_DYNAMIC_ITEM_REWARD,

    //QUEST_SPECIAL_FLAGS_DELIVER              = 0x0080,   // Internal flag computed only
    //QUEST_SPECIAL_FLAGS_SPEAKTO              = 0x0100,   // Internal flag computed only
    QUEST_SPECIAL_FLAGS_KILL                 = 0x0200,   // Internal flag computed only
    QUEST_SPECIAL_FLAGS_TIMED                = 0x0400,   // Internal flag computed only
    //QUEST_SPECIAL_FLAGS_PLAYER_KILL          = 0x0800,   // Internal flag computed only
};

enum QuestObjectiveType
{
    QUEST_OBJECTIVE_TYPE_NPC                    = 0,
    QUEST_OBJECTIVE_TYPE_ITEM                   = 1,
    QUEST_OBJECTIVE_TYPE_GO                     = 2,
    QUEST_OBJECTIVE_TYPE_NPC_INTERACT           = 3,
    QUEST_OBJECTIVE_TYPE_CURRENCY               = 4,
    QUEST_OBJECTIVE_TYPE_SPELL                  = 5,
    QUEST_OBJECTIVE_TYPE_FACTION_MIN            = 6,
    QUEST_OBJECTIVE_TYPE_FACTION_MAX            = 7,
    QUEST_OBJECTIVE_TYPE_MONEY                  = 8,
    QUEST_OBJECTIVE_TYPE_PLAYER                 = 9,
    QUEST_OBJECTIVE_TYPE_AREATRIGGER            = 10,
    QUEST_OBJECTIVE_TYPE_PET_TRAINER_DEFEAT     = 11,
    QUEST_OBJECTIVE_TYPE_PET_BATTLE_ELITE       = 12,
    QUEST_OBJECTIVE_TYPE_PET_BATTLE_PVP         = 13,
    QUEST_OBJECTIVE_TYPE_CRITERIA_TREE          = 14,
    QUEST_OBJECTIVE_PROGRESS_BAR                = 15,
    QUEST_OBJECTIVE_TYPE_HAVE_CURRENCY          = 16,
    QUEST_OBJECTIVE_TYPE_OBTAIN_CURRENCY        = 17,
    QUEST_OBJECTIVE_TYPE_END
};

enum QuestObjectiveFlags
{
    QUEST_OBJECTIVE_FLAG_TRACKED_ON_MINIMAP                 = 0x01, ///< Client displays large yellow blob on minimap for creature/gameobject
    QUEST_OBJECTIVE_FLAG_SEQUENCED                          = 0x02, ///< Client will not see the objective displayed until all previous objectives are completed
    QUEST_OBJECTIVE_FLAG_OPTIONAL                           = 0x04, ///< Not required to complete the quest
    QUEST_OBJECTIVE_FLAG_HIDDEN                             = 0x08, ///< Never displayed in quest log
    QUEST_OBJECTIVE_FLAG_HIDE_ITEM_GAINS                    = 0x10, ///< Skip showing item objective progress
    QUEST_OBJECTIVE_FLAG_PROGRESS_COUNTS_ITEMS_IN_INVENTORY = 0x20, ///< Item objective progress counts items in inventory instead of reading it from updatefields
    QUEST_OBJECTIVE_FLAG_PART_OF_PROGRESS_BAR               = 0x40, ///< Hidden objective used to calculate progress bar percent (quests are limited to a single progress bar objective)
};

enum QuestMethod
{
    QUEST_METHOD_AUTO_COMPLETE              = 0,
    QUEST_METHOD_DISABLED                   = 1,
    QUEST_METHOD_ENABLED_NO_AUTO_COMPLETE   = 2,
    QUEST_METHOD_AUTO_SUBMITED              = 3
};

enum class QuestRewardItemBonus : uint32
{
    UncommunToRare  = 171,
    UncommunToEpic  = 15,
    RareToEpic      = 545
};

const float gQuestRewardBonusRareChanceRange[2] = { 0.0f,    10.0f };    ///< Range 0-10
const float gQuestRewardBonusEpicChanceRange[2] = { 10.01f, 20.0f};    ///< Range 10.01-20

struct QuestLocale
{
    QuestLocale() { }

    StringVector Title;
    StringVector Details;
    StringVector Objectives;
    StringVector OfferRewardText;
    StringVector RequestItemsText;
    StringVector EndText;
    StringVector CompletedText;
    std::vector< StringVector > ObjectiveText;
    // new on 4.x
    StringVector QuestGiverTextWindow;
    StringVector QuestGiverTargetName;
    StringVector QuestTurnTextWindow;
    StringVector QuestTurnTargetName;
};

struct QuestObjective
{
    uint32 ID;
    uint32 QuestID;
    uint8 Type;
    int8 Index;
    int32 ObjectID;
    int32 Amount;
    uint32 Flags;
    uint32 Flags2;
    float ProgressBarWeight;
    std::string Description;
    std::vector<int32> VisualEffects;
};

struct QuestObjectiveLocale
{
    StringVector Description;
};

// This Quest class provides a convenient way to access a few pretotaled (cached) quest details,
// all base quest information, and any utility functions such as generating the amount of
// xp to give
class Quest
{
    friend class ObjectMgr;
    public:
        Quest(Field* questRecord);
        uint32 XPValue(Player* player) const;
        uint32 MoneyValue(uint8 playerLevel) const;

        bool HasFlag(uint32 flag) const { return (Flags & flag) != 0; }
        bool HasFlag2(uint32 flag) const { return (Flags2 & flag) != 0; }
        void SetFlag(uint32 flag) { Flags |= flag; }

        bool HasSpecialFlag(uint32 flag) const { return (SpecialFlags & flag) != 0; }
        void SetSpecialFlag(uint32 flag) { SpecialFlags |= flag; }

        // table data accessors:
        uint32 GetQuestId() const { return Id; }
        uint32 GetQuestMethod() const { return Method; }
        uint32 GetQuestPackageID() const { return PackageID; }
        int32  GetZoneOrSort() const { return ZoneOrSort; }
        uint32 GetMinLevel() const { return MinLevel; }
        uint32 GetMaxLevel() const { return MaxLevel; }
        int32  GetQuestLevel() const { return Level; }
        int32  GetQuestMaxScalingLevel() const { return MaxScalingLevel; }
        uint32 GetType() const { return Type; }
        int8   GetRequiredTeam() const { return RequiredTeam; }
        int32  GetRequiredClasses() const { return RequiredClasses; }
        int64  GetRequiredRaces() const { return RequiredRaces; }
        uint32 GetRequiredSkill() const { return RequiredSkillId; }
        uint32 GetRequiredSkillValue() const { return RequiredSkillPoints; }
        uint32 GetRequiredMinRepFaction() const { return RequiredMinRepFaction; }
        int32  GetRequiredMinRepValue() const { return RequiredMinRepValue; }
        uint32 GetRequiredMaxRepFaction() const { return RequiredMaxRepFaction; }
        int32  GetRequiredMaxRepValue() const { return RequiredMaxRepValue; }
        uint32 GetSuggestedPlayers() const { return SuggestedPlayers; }
        uint32 GetLimitTime() const { return LimitTime; }
        uint32 GetAreaGroupId() const { return AreaGroupId; }
        uint32 QuestRewardID() const { return RewardID; }
        int32 GetExpansion() const { return Expansion; }
        int32  GetPrevQuestId() const { return PrevQuestId; }
        int32  GetNextQuestId() const { return NextQuestId; }
        int32  GetExclusiveGroup() const { return ExclusiveGroup; }
        uint32 GetNextQuestInChain() const { return NextQuestIdChain; }
        uint32 GetCharTitleId() const { return RewardTitleId; }
        uint32 GetBonusTalents() const { return RewardTalents; }
        uint32 GetXPId() const { return RewardXPId; }
        float GetXPMultiplier() const { return RewardXPMultiplier; }
        uint32 GetSrcItemId() const { return SourceItemId; }
        uint32 GetSrcSpell() const { return SourceSpellid; }
        std::string GetTitle() const { return Title; }
        std::string GetDetails() const { return Details; }
        std::string GetObjectives() const { return Objectives; }
        std::string GetOfferRewardText() const { return OfferRewardText; }
        std::string GetRequestItemsText() const { return RequestItemsText; }
        std::string GetEndText() const { return EndText; }
        std::string GetCompletedText() const { return CompletedText; }
        std::string GetQuestGiverTextWindow() const { return QuestGiverTextWindow; }
        std::string GetQuestGiverTargetName() const { return QuestGiverTargetName; }
        std::string GetQuestTurnTextWindow() const { return QuestTurnTextWindow; }
        std::string GetQuestTurnTargetName() const { return QuestTurnTargetName; }
        float  GetMoneyMultiplier() const { return RewardMoneyMultiplier; }
        uint32 GetMoneyDifficulty() const { return RewardMoneyDifficulty; }
        uint32 GetRewHonorAddition() const { return RewardHonor; }
        float  GetRewHonorMultiplier() const { return RewardHonorMultiplier; }
        uint32 GetRewMoneyMaxLevel() const;
        uint32 GetRewArtifactXP() const { return RewardArtifactXP; }
        float GetRewArtifactXPMultilier() const { return RewardArtifactXPMultiplier; }
        uint32 GetRewArtifactXPDifficulty() const { return RewardArtifactXPDifficulty; }
        int32  GetRewSpellCast() const { return RewardSpellCast; }
        uint32 GetRewMailTemplateId() const { return RewardMailTemplateId; }
        uint32 GetRewMailDelaySecs() const { return RewardMailDelay; }
        uint32 GetPointMapId() const { return PointMapId; }
        float  GetPointX() const { return PointX; }
        float  GetPointY() const { return PointY; }
        uint32 GetPointOpt() const { return PointOption; }
        uint32 GetSoundAccept() const { return SoundAccept; }
        uint32 GetSoundTurnIn() const { return SoundTurnIn; }
        uint32 GetIncompleteEmote() const { return EmoteOnIncomplete; }
        uint32 GetCompleteEmote() const { return EmoteOnComplete; }
        uint32 GetQuestStartScript() const { return StartScript; }
        uint32 GetQuestCompleteScript() const { return CompleteScript; }
        bool   IsLegendary() const { return Flags2 & QUEST_FLAGS_EX_LEGENDARY_QUEST; }
        bool   IsRepeatable() const { return SpecialFlags & QUEST_SPECIAL_FLAGS_REPEATABLE; }
        bool   IsAutoAccept() const;
        bool   IsAutoTake() const;
        bool   IsAutoComplete() const;
        uint32 GetFlags() const { return Flags; }
        uint32 GetFlags2() const { return Flags2; }
        uint32 GetSpecialFlags() const { return SpecialFlags; }
        uint32 GetMinimapTargetMark() const { return MinimapTargetMark; }
        uint32 GetRewardSkillId() const { return RewardSkillId; }
        uint32 GetRewardSkillPoints() const { return RewardSkillPoints; }
        uint32 GetRewardReputationMask() const { return RewardReputationMask; }
        uint32 GetQuestGiverPortrait() const { return QuestGiverPortrait; }
        uint32 GetQuestTurnInPortrait() const { return QuestTurnInPortrait; }
        bool   IsDaily() const { return Flags & QUEST_FLAGS_DAILY; }
        bool   IsWeekly() const { return Flags & QUEST_FLAGS_WEEKLY; }
        bool   IsMonthly() const { return Flags & QUEST_SPECIAL_FLAGS_MONTHLY; }
        bool   IsSeasonal() const { return (ZoneOrSort == -QUEST_SORT_SEASONAL || ZoneOrSort == -QUEST_SORT_SPECIAL || ZoneOrSort == -QUEST_SORT_LUNAR_FESTIVAL || ZoneOrSort == -QUEST_SORT_MIDSUMMER || ZoneOrSort == -QUEST_SORT_BREWFEST || ZoneOrSort == -QUEST_SORT_LOVE_IS_IN_THE_AIR || ZoneOrSort == -QUEST_SORT_NOBLEGARDEN) && !IsRepeatable(); }
        bool   IsDailyOrWeekly() const { return Flags & (QUEST_FLAGS_DAILY | QUEST_FLAGS_WEEKLY); }
        bool   IsRaidQuest() const { return Type == QUEST_INFO_RAID || Type == QUEST_INFO_RAID_10 || Type == QUEST_INFO_RAID_25; }
        bool   IsAllowedInRaid() const;
        bool   IsDFQuest() const { return SpecialFlags & QUEST_SPECIAL_FLAGS_DF_QUEST; }
        bool   IsAccountQuest() const { return Type == QUEST_INFO_ACCOUNT; }
        uint32 CalculateHonorGain() const;

        std::vector<QuestObjective> QuestObjectives;

        // multiple values
        uint32 RequiredSourceItemId[QUEST_SOURCE_ITEM_IDS_COUNT];
        uint32 RequiredSourceItemCount[QUEST_SOURCE_ITEM_IDS_COUNT];
        uint32 RewardChoiceItemId[QUEST_REWARD_CHOICES_COUNT];
        uint32 RewardChoiceItemCount[QUEST_REWARD_CHOICES_COUNT];
        uint32 RewardItemId[QUEST_REWARDS_COUNT];
        uint32 RewardItemIdCount[QUEST_REWARDS_COUNT];
        uint32 RewardFactionId[QUEST_REPUTATIONS_COUNT];
        int32  RewardFactionValueId[QUEST_REPUTATIONS_COUNT];
        int32  RewardFactionValueIdOverride[QUEST_REPUTATIONS_COUNT];
        int32  RewardFactionCapIn[QUEST_REPUTATIONS_COUNT];
        uint32 DetailsEmote[QUEST_EMOTE_COUNT];
        uint32 DetailsEmoteDelay[QUEST_EMOTE_COUNT];
        uint32 OfferRewardEmote[QUEST_EMOTE_COUNT];
        uint32 OfferRewardEmoteDelay[QUEST_EMOTE_COUNT];
        uint32 RewardSpells[QUEST_REWARD_SPELL_COUNT];

        // 4.x
        uint32 RewardCurrencyId[QUEST_REWARD_CURRENCY_COUNT];
        uint32 RewardCurrencyCount[QUEST_REWARD_CURRENCY_COUNT];

        int32  RewardMoney;

        uint32 InProgressPhaseID;
        uint32 CompletedPhaseID;

        uint32 GetRewChoiceItemsCount() const { return m_rewChoiceItemsCount; }
        uint32 GetRewItemsCount() const { return m_rewItemsCount; }
        uint32 GetRewCurrencyCount() const { return m_rewCurrencyCount; }

        bool HasDynamicReward() const { return !DynamicRewards.empty(); }
        std::list<QuestPackageItemEntry const*> DynamicRewards;

        typedef std::vector<int32> PrevQuests;
        PrevQuests prevQuests;
        typedef std::vector<uint32> PrevChainQuests;
        PrevChainQuests prevChainQuests;

        typedef std::vector<uint32> CompletionsNpcs;
        CompletionsNpcs completionsNpcs;

        // objective stuff
        QuestObjective const* GetQuestObjective(uint32 objectiveId) const;
        QuestObjective const* GetQuestObjectiveXIndex(uint8 index) const;
        QuestObjective const* GetQuestObjectiveXObjectId(uint32 objectId) const;

        uint8 GetQuestObjectiveCount() const { return QuestObjectives.size(); }
        uint8 GetQuestObjectiveCountType(uint8 type) const;
        uint32 GetQuestObjectiveId(uint32 p_QuestId, uint8 p_ObjIndex) const;

        // cached data
    private:
        uint32 m_rewChoiceItemsCount;
        uint32 m_rewItemsCount;
        uint32 m_rewCurrencyCount;
        uint8 QuestObjecitveTypeCount[QUEST_OBJECTIVE_TYPE_END];

        // table data
    protected:
        uint32 Id;
        uint32 Method;
        int32  ZoneOrSort;
        uint32 MinLevel;
        uint32 MaxLevel;
        uint32 MaxScalingLevel;
        int32  Level;
        uint32 Type;
        int8   RequiredTeam;
        int32  RequiredClasses;
        int64  RequiredRaces;
        uint32 RequiredSkillId;
        uint32 RequiredSkillPoints;
        uint32 RequiredMinRepFaction;
        int32  RequiredMinRepValue;
        uint32 RequiredMaxRepFaction;
        int32  RequiredMaxRepValue;
        uint32 SuggestedPlayers;
        uint32 LimitTime;
        uint32 Flags;
        uint32 Flags2;
        uint32 RewardTitleId;
        uint32 RewardTalents;
        int32  PrevQuestId;
        int32  NextQuestId;
        uint32 PackageID;
        uint32 NextQuestIdChain;
        uint32 RewardXPId;
        float  RewardXPMultiplier;
        uint32 SourceItemId;
        uint32 SourceSpellid;
        std::string Title;
        std::string Details;
        std::string Objectives;
        std::string OfferRewardText;
        std::string RequestItemsText;
        std::string EndText;
        std::string CompletedText;
        uint32 RewardHonor;
        float  RewardHonorMultiplier;
        uint32 RewardArtifactXP;
        float RewardArtifactXPMultiplier;
        uint32 RewardArtifactXPDifficulty;
        uint32 RewardMoneyDifficulty;
        float  RewardMoneyMultiplier;
        uint32 RewardMoneyMaxLevel;
        int32  RewardSpellCast;
        uint32 RewardMailTemplateId;
        uint32 RewardMailDelay;
        uint32 PointMapId;
        float  PointX;
        float  PointY;
        uint32 PointOption;
        uint32 EmoteOnIncomplete;
        uint32 EmoteOnComplete;
        uint32 StartScript;
        uint32 CompleteScript;
        uint32 MinimapTargetMark;
        uint32 RewardSkillId;
        uint32 RewardSkillPoints;
        uint32 RewardReputationMask;
        uint32 QuestGiverPortrait;
        uint32 QuestTurnInPortrait;
        std::string QuestGiverTextWindow;
        std::string QuestGiverTargetName;
        std::string QuestTurnTextWindow;
        std::string QuestTurnTargetName;
        uint32 SoundAccept;
        uint32 SoundTurnIn;
        uint32 AreaGroupId;
        uint32 RewardID;
        int32 Expansion;
        uint32 SpecialFlags;        ///< custom flags, not sniffed/WDB
        int32  ExclusiveGroup;      ///< custom value, not sniffed/WDB
};

struct QuestStatusData
{
    QuestStatusData(): Status(QUEST_STATUS_NONE), Timer(0), Explored(false)
    {

    }

    QuestStatus Status;
    uint32 Timer;
    bool Explored;
};

#endif
