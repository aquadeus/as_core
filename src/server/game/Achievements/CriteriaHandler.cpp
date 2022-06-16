////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "CriteriaHandler.h"
#include "Battleground.h"
#include "DB2Stores.h"
#include "DisableMgr.h"
#include "GameEventMgr.h"
#include "GarrisonMgr.hpp"
#include "Group.h"
#include "InstanceScript.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Formulas.h"
#include "ScenarioMgr.h"
#include "PetBattle.h"
#include "CollectionMgr.hpp"
#include "BattlegroundSM.h"

bool CriteriaData::IsValid(Criteria const* criteria)
{
    if (DataType >= MAX_CRITERIA_DATA_TYPE)
    {
        sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` for criteria (Entry: %u) contains a wrong data type (%u), ignored.", criteria->ID, DataType);
        return false;
    }

    switch (criteria->Entry->Type)
    {
        case CRITERIA_TYPE_KILL_CREATURE:
        case CRITERIA_TYPE_KILL_CREATURE_TYPE:
        case CRITERIA_TYPE_WIN_BG:
        case CRITERIA_TYPE_FALL_WITHOUT_DYING:
        case CRITERIA_TYPE_COMPLETE_QUEST:          // only hardcoded list
        case CRITERIA_TYPE_CAST_SPELL:
        case CRITERIA_TYPE_WIN_RATED_ARENA:
        case CRITERIA_TYPE_DO_EMOTE:
        case CRITERIA_TYPE_SPECIAL_PVP_KILL:
        case CRITERIA_TYPE_WIN_DUEL:
        case CRITERIA_TYPE_LOOT_TYPE:
        case CRITERIA_TYPE_CAST_SPELL2:
        case CRITERIA_TYPE_BE_SPELL_TARGET:
        case CRITERIA_TYPE_BE_SPELL_TARGET2:
        case CRITERIA_TYPE_EQUIP_EPIC_ITEM:
        case CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
        case CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
        case CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
        case CRITERIA_TYPE_HONORABLE_KILL:
        case CRITERIA_TYPE_COMPLETE_DAILY_QUEST:    // only Children's Week achievements
        case CRITERIA_TYPE_USE_ITEM:                // only Children's Week achievements
        case CRITERIA_TYPE_GET_KILLING_BLOWS:
        case CRITERIA_TYPE_REACH_LEVEL:
        case CRITERIA_TYPE_ON_LOGIN:
        case CRITERIA_TYPE_LOOT_EPIC_ITEM:
        case CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
            break;
        default:
            if (DataType != CRITERIA_DATA_TYPE_SCRIPT)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` contains data for a non-supported criteria type (Entry: %u Type: %u), ignored.", criteria->ID, criteria->Entry->Type);
                return false;
            }
            break;
    }

    switch (DataType)
    {
        case CRITERIA_DATA_TYPE_NONE:
        case CRITERIA_DATA_TYPE_INSTANCE_SCRIPT:
            return true;
        case CRITERIA_DATA_TYPE_T_CREATURE:
            if (!Creature.Id || !sObjectMgr->GetCreatureTemplate(Creature.Id))
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_CREATURE (%u) contains a non-existing creature id in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Creature.Id);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE:
            if (!ClassRace.Class && !ClassRace.Race)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE (%u) must not have 0 in either value field, ignored.",
                    criteria->ID, criteria->Entry->Type, DataType);
                return false;
            }
            if (ClassRace.Class && ((1 << (ClassRace.Class-1)) & CLASSMASK_ALL_PLAYABLE) == 0)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE (%u) contains a non-existing class in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, ClassRace.Class);
                return false;
            }
            if (ClassRace.Race && ((1 << (ClassRace.Race-1)) & RACEMASK_ALL_PLAYABLE) == 0)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE (%u) contains a non-existing race in value2 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, ClassRace.Race);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_T_PLAYER_LESS_HEALTH:
            if (Health.Percent < 1 || Health.Percent > 100)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_PLAYER_LESS_HEALTH (%u) contains a wrong percent value in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Health.Percent);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_S_AURA:
        case CRITERIA_DATA_TYPE_T_AURA:
        {
            SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(Aura.SpellId);
            if (!spellEntry)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type %s (%u) contains a wrong spell id in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, (DataType == CRITERIA_DATA_TYPE_S_AURA ? "CRITERIA_DATA_TYPE_S_AURA" : "CRITERIA_DATA_TYPE_T_AURA"), DataType, Aura.SpellId);
                return false;
            }
            SpellEffectInfo const* effect = &spellEntry->Effects[Aura.EffectIndex];
            if (!effect)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type %s (%u) contains a wrong spell effect index in value2 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, (DataType == CRITERIA_DATA_TYPE_S_AURA ? "CRITERIA_DATA_TYPE_S_AURA" : "CRITERIA_DATA_TYPE_T_AURA"), DataType, Aura.EffectIndex);
                return false;
            }
            if (!effect->ApplyAuraName)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type %s (%u) contains a non-aura spell effect (ID: %u Effect: %u), ignored.",
                    criteria->ID, criteria->Entry->Type, (DataType == CRITERIA_DATA_TYPE_S_AURA ? "CRITERIA_DATA_TYPE_S_AURA" : "CRITERIA_DATA_TYPE_T_AURA"), DataType, Aura.SpellId, Aura.EffectIndex);
                return false;
            }
            return true;
        }
        case CRITERIA_DATA_TYPE_VALUE:
            if (Value.ComparisonType >= COMP_TYPE_MAX)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_VALUE (%u) contains a wrong ComparisionType in value2 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Value.ComparisonType);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_T_LEVEL:
            if (Level.Min > STRONG_MAX_LEVEL)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `CRITERIA_DATA` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_T_LEVEL (%u) contains a wrong minlevel in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Level.Min);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_T_GENDER:
            if (Gender.Gender > GENDER_NONE)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_T_GENDER (%u) contains a wrong gender value in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Gender.Gender);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_SCRIPT:
            if (!ScriptId)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_SCRIPT (%u) does not have a ScriptName set, ignored.",
                    criteria->ID, criteria->Entry->Type, DataType);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT:
            if (MapPlayers.MaxCount <= 0)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT (%u) contains a wrong max players count in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, MapPlayers.MaxCount);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_T_TEAM:
            if (Team.Team != ALLIANCE && Team.Team != HORDE)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_T_TEAM (%u) contains an unknown team value in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Team.Team);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_S_DRUNK:
            if (Drunk.State >= MAX_DRUNKEN)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_S_DRUNK (%u) contains an unknown drunken state value in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Drunk.State);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_HOLIDAY:
            if (!sHolidaysStore.LookupEntry(Holiday.Id))
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_HOLIDAY (%u) contains an unknown holiday entry in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Holiday.Id);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_GAME_EVENT:
        {
            GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
            if (GameEvent.Id < 1 || GameEvent.Id >= events.size())
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_GAME_EVENT (%u) has unknown game_event in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, GameEvent.Id);
                return false;
            }
            return true;
        }
        case CRITERIA_DATA_TYPE_BG_LOSS_TEAM_SCORE:
            return true;                                    // not check correctness node indexes
        case CRITERIA_DATA_TYPE_S_EQUIPPED_ITEM:
            if (EquippedItem.Quality >= MAX_ITEM_QUALITY)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_S_EQUIPPED_ITEM (%u) contains an unknown quality state value in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, EquippedItem.Quality);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_MAP_ID:
            if (!sMapStore.LookupEntry(Map.Id))
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_MAP_ID (%u) contains an unknown map entry in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, Map.Id);
            }
            return true;
        case CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE:
            if (!ClassRace.Class && !ClassRace.Race)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE (%u) should not have 0 in either value field. Ignored.",
                    criteria->ID, criteria->Entry->Type, DataType);
                return false;
            }
            if (ClassRace.Class && ((1 << (ClassRace.Class-1)) & CLASSMASK_ALL_PLAYABLE) == 0)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE (%u) contains a non-existing class entry in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, ClassRace.Class);
                return false;
            }
            if (ClassRace.Race && ((1 << (ClassRace.Race-1)) & RACEMASK_ALL_PLAYABLE) == 0)
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE (%u) contains a non-existing race entry in value2 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, ClassRace.Race);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_S_KNOWN_TITLE:
            if (!sCharTitlesStore.LookupEntry(KnownTitle.Id))
            {
                sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_S_KNOWN_TITLE (%u) contains an unknown title_id in value1 (%u), ignore.",
                    criteria->ID, criteria->Entry->Type, DataType, KnownTitle.Id);
                return false;
            }
            return true;
        case CRITERIA_DATA_TYPE_S_ITEM_QUALITY:
            if (ItemQuality.Quality >= MAX_ITEM_QUALITY)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `criteria_data` (Entry: %u Type: %u) for data type CRITERIA_DATA_TYPE_S_ITEM_QUALITY (%u) contains an unknown quality state value in value1 (%u), ignored.",
                    criteria->ID, criteria->Entry->Type, DataType, ItemQuality.Quality);
                return false;
            }
            return true;
        default:
            sLog->outError(LOG_FILTER_SQL,  "Table `criteria_data` (Entry: %u Type: %u) contains data of a non-supported data type (%u), ignored.", criteria->ID, criteria->Entry->Type, DataType);
            return false;
    }
}

bool CriteriaData::Meets(uint32 criteriaId, Player const* source, Unit const* target, uint32 miscValue1 /*= 0*/) const
{
    switch (DataType)
    {
        case CRITERIA_DATA_TYPE_NONE:
            return true;
        case CRITERIA_DATA_TYPE_T_CREATURE:
            if (!target || target->GetTypeId() != TYPEID_UNIT)
                return false;
            return target->GetEntry() == Creature.Id;
        case CRITERIA_DATA_TYPE_T_PLAYER_CLASS_RACE:
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return false;
            if (ClassRace.Class && ClassRace.Class != target->ToPlayer()->getClass())
                return false;
            if (ClassRace.Race && ClassRace.Race != target->ToPlayer()->getRace())
                return false;
            return true;
        case CRITERIA_DATA_TYPE_S_PLAYER_CLASS_RACE:
            if (source->GetTypeId() != TYPEID_PLAYER)
                return false;
            if (ClassRace.Class && ClassRace.Class != source->ToPlayer()->getClass())
                return false;
            if (ClassRace.Race && ClassRace.Race != source->ToPlayer()->getRace())
                return false;
            return true;
        case CRITERIA_DATA_TYPE_T_PLAYER_LESS_HEALTH:
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return false;
            return !target->HealthAbovePct(Health.Percent);
        case CRITERIA_DATA_TYPE_S_AURA:
            return source->HasAuraEffect(Aura.SpellId, uint8(Aura.EffectIndex));
        case CRITERIA_DATA_TYPE_T_AURA:
            return target && target->HasAuraEffect(Aura.SpellId, uint8(Aura.EffectIndex));
        case CRITERIA_DATA_TYPE_VALUE:
            return CompareValues(ComparisionType(Value.ComparisonType), miscValue1, Value.Value);
        case CRITERIA_DATA_TYPE_T_LEVEL:
            if (!target)
                return false;
            return target->getLevel() >= Level.Min;
        case CRITERIA_DATA_TYPE_T_GENDER:
            if (!target)
                return false;
            return target->getGender() == Gender.Gender;
        case CRITERIA_DATA_TYPE_SCRIPT:
            return sScriptMgr->OnCriteriaCheck(ScriptId, const_cast<Player*>(source), const_cast<Unit*>(target));
        case CRITERIA_DATA_TYPE_MAP_PLAYER_COUNT:
            return source->GetMap()->GetPlayersCountExceptGMs() <= MapPlayers.MaxCount;
        case CRITERIA_DATA_TYPE_T_TEAM:
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return false;
            return target->ToPlayer()->GetTeam() == Team.Team;
        case CRITERIA_DATA_TYPE_S_DRUNK:
            return Player::GetDrunkenstateByValue(source->GetDrunkValue()) >= DrunkenState(Drunk.State);
        case CRITERIA_DATA_TYPE_HOLIDAY:
            return IsHolidayActive(HolidayIds(Holiday.Id));
        case CRITERIA_DATA_TYPE_GAME_EVENT:
            return IsEventActive(uint16(GameEvent.Id));
        case CRITERIA_DATA_TYPE_BG_LOSS_TEAM_SCORE:
        {
            Battleground* bg = source->GetBattleground();
            if (!bg)
                return false;

            uint32 score = bg->GetTeamScore(source->GetTeamId() == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE);
            return score >= BattlegroundScore.Min && score <= BattlegroundScore.Max;
        }
        case CRITERIA_DATA_TYPE_INSTANCE_SCRIPT:
        {
            if (!source->IsInWorld())
                return false;
            class Map* map = source->GetMap();
            if (!map->IsDungeon())
            {
                sLog->outError(LOG_FILTER_ACHIEVEMENTSYS, "Criteria system call CRITERIA_DATA_TYPE_INSTANCE_SCRIPT (%u) for criteria %u in a non-dungeon/non-raid map %u",
                    DataType, criteriaId, map->GetId());
                return false;
            }
            InstanceScript* instance = map->ToInstanceMap()->GetInstanceScript();
            if (!instance)
            {
                sLog->outError(LOG_FILTER_ACHIEVEMENTSYS, "Criteria system call CRITERIA_DATA_TYPE_INSTANCE_SCRIPT (%u) for criteria %u in map %u, but the map does not have an instance script.",
                    DataType, criteriaId, map->GetId());
                return false;
            }
            return instance->CheckAchievementCriteriaMeet(criteriaId, source, target, miscValue1);
        }
        case CRITERIA_DATA_TYPE_S_EQUIPPED_ITEM:
        {
            ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(miscValue1);
            if (!pProto)
                return false;
            return pProto->ItemLevel >= EquippedItem.ItemLevel && pProto->Quality >= EquippedItem.Quality;
        }
        case CRITERIA_DATA_TYPE_MAP_ID:
            return source->GetMapId() == Map.Id;
        case CRITERIA_DATA_TYPE_S_KNOWN_TITLE:
        {
            if (CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(KnownTitle.Id))
                return source && const_cast<Player*>(source)->HasTitle(titleInfo->MaskID);

            return false;
        }
        case CRITERIA_DATA_TYPE_S_ITEM_QUALITY:
        {
            ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(miscValue1);
            if (!pProto)
                return false;
            return pProto->Quality == ItemQuality.Quality;
        }
        default:
            break;
    }
    return false;
}

bool CriteriaDataSet::Meets(Player const* source, Unit const* target, uint32 miscValue /*= 0*/) const
{
    for (CriteriaData const& data : _storage)
        if (!data.Meets(_criteriaId, source, target, miscValue))
            return false;

    return true;
}

CriteriaHandler::CriteriaHandler() { }

CriteriaHandler::~CriteriaHandler() { }

void CriteriaHandler::Reset()
{
    for (auto iter = _criteriaProgress.begin(); iter != _criteriaProgress.end(); ++iter)
        SendCriteriaProgressRemoved(iter->first);

    _criteriaProgress.clear();
}

/**
 * this function will be called whenever the user might have done a criteria relevant action
 */
void CriteriaHandler::UpdateCriteria(CriteriaTypes type, uint64 miscValue1 /*= 0*/, uint64 miscValue2 /*= 0*/, uint64 miscValue3 /*= 0*/, Unit const* unit /*= nullptr*/, Player* referencePlayer /*= nullptr*/)
{
    if (type >= CRITERIA_TYPE_TOTAL)
        return;

    if (!referencePlayer)
        return;

    // disable for gamemasters with GM-mode enabled
    if (referencePlayer->isGameMaster())
        return;

    CriteriaList const& criteriaList = GetCriteriaByType(type);
    for (Criteria const* criteria : criteriaList)
    {
        CriteriaTreeList const* trees = sCriteriaMgr->GetCriteriaTreesByCriteria(criteria->ID);
        if (!CanUpdateCriteria(criteria, trees, miscValue1, miscValue2, miscValue3, unit, referencePlayer))
            continue;

        // requirements not found in the db2
        if (CriteriaDataSet const* data = sCriteriaMgr->GetCriteriaDataSet(criteria))
            if (!data->Meets(referencePlayer, unit, uint32(miscValue1)))
                continue;

        switch (type)
        {
            // std. case: increment at 1
            case CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS:
            case CRITERIA_TYPE_LOSE_DUEL:
            case CRITERIA_TYPE_CREATE_AUCTION:
            case CRITERIA_TYPE_WON_AUCTIONS:    /* FIXME: for online player only currently */
            case CRITERIA_TYPE_ROLL_NEED:
            case CRITERIA_TYPE_ROLL_GREED:
            case CRITERIA_TYPE_QUEST_ABANDONED:
            case CRITERIA_TYPE_FLIGHT_PATHS_TAKEN:
            case CRITERIA_TYPE_ACCEPTED_SUMMONINGS:
            case CRITERIA_TYPE_LOOT_EPIC_ITEM:
            case CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
            case CRITERIA_TYPE_DEATH:
            case CRITERIA_TYPE_COMPLETE_DAILY_QUEST:
            case CRITERIA_TYPE_DEATH_AT_MAP:
            case CRITERIA_TYPE_DEATH_IN_DUNGEON:
            case CRITERIA_TYPE_KILLED_BY_CREATURE:
            case CRITERIA_TYPE_KILLED_BY_PLAYER:
            case CRITERIA_TYPE_DEATHS_FROM:
            case CRITERIA_TYPE_BE_SPELL_TARGET:
            case CRITERIA_TYPE_BE_SPELL_TARGET2:
            case CRITERIA_TYPE_CAST_SPELL:
            case CRITERIA_TYPE_CAST_SPELL2:
            case CRITERIA_TYPE_WIN_RATED_ARENA:
            case CRITERIA_TYPE_USE_ITEM:
            case CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
            case CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
            case CRITERIA_TYPE_DO_EMOTE:
            case CRITERIA_TYPE_USE_GAMEOBJECT:
            case CRITERIA_TYPE_FISH_IN_GAMEOBJECT:
            case CRITERIA_TYPE_WIN_DUEL:
            case CRITERIA_TYPE_HK_CLASS:
            case CRITERIA_TYPE_HK_RACE:
            case CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
            case CRITERIA_TYPE_HONORABLE_KILL:
            case CRITERIA_TYPE_SPECIAL_PVP_KILL:
            case CRITERIA_TYPE_GET_KILLING_BLOWS:
            case CRITERIA_TYPE_HONORABLE_KILL_AT_AREA:
            case CRITERIA_TYPE_WIN_ARENA: // This also behaves like CRITERIA_TYPE_WIN_RATED_ARENA
            case CRITERIA_TYPE_ON_LOGIN:
            case CRITERIA_TYPE_OWN_PET:
            case CRITERIA_TYPE_PLACE_GARRISON_BUILDING:
            case CRITERIA_TYPE_OWN_BATTLE_PET_COUNT:
            case CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE:
            case CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE:
            case CRITERIA_TYPE_KILL_CREATURE_TYPE_GUILD:
            case CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS:
            case CRITERIA_TYPE_ENTER_AREA:
            case CRITERIA_TYPE_LEAVE_AREA:
            case CRITERIA_TYPE_OWN_TOY_COUNT:
            case CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER:
            case CRITERIA_TYPE_COMPLETE_SCENARIO_COUNT:
            case CRITERIA_TYPE_REACH_SCENARIO_BOSS:
            case CRITERIA_TYPE_COMPLETE_CHALLENGE:
            case CRITERIA_TYPE_COMPLETE_GARRISON_MISSION:
            case CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_2:
            case CRITERIA_TYPE_WIN_PET_BATTLE:
            case CRITERIA_TYPE_COMPLETE_WORLD_QUEST:
            case CRITERIA_TYPE_WIN_RATED_BATTLEGROUND:
            case CRITERIA_TYPE_SEND_EVENT_SCENARIO:
            case CRITERIA_TYPE_SEND_EVENT:
            case CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE:
            case CRITERIA_TYPE_COMPLETE_GARRISON_SHIPMENT:
            case CRITERIA_TYPE_CLEAR_DIGSITE:
            case CRITERIA_TYPE_SURVEY_GAMEOBJECT:
                SetCriteriaProgress(criteria, 1, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            // std case: increment at miscValue1
            case CRITERIA_TYPE_MONEY_FROM_VENDORS:
            case CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS:
            case CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD:
            case CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING:
            case CRITERIA_TYPE_GOLD_SPENT_AT_BARBER:
            case CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL:
            case CRITERIA_TYPE_LOOT_MONEY:
            case CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS:/* FIXME: for online player only currently */
            case CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED:
            case CRITERIA_TYPE_TOTAL_HEALING_RECEIVED:
            case CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS:
            case CRITERIA_TYPE_WIN_BG:
            case CRITERIA_TYPE_COMPLETE_BATTLEGROUND:
            case CRITERIA_TYPE_DAMAGE_DONE:
            case CRITERIA_TYPE_HEALING_DONE:
            case CRITERIA_TYPE_BUY_GUILD_TABARD:
            case CRITERIA_TYPE_CATCH_FROM_POOL:
            case CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS:
            case CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS:
            case CRITERIA_TYPE_COMPLETE_QUESTS_GUILD:
            case CRITERIA_TYPE_HONORABLE_KILLS_GUILD:
            case CRITERIA_TYPE_PRESTIGE_REACHED:
            case CRITERIA_TYPE_HONOR_LEVEL_REACHED:
            case CRITERIA_TYPE_REACH_LEVEL_2:
            case CRITERIA_TYPE_CONSTRUCT_GARRISON_BUILDING:
                SetCriteriaProgress(criteria, miscValue1, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            case CRITERIA_TYPE_KILL_CREATURE:
            case CRITERIA_TYPE_KILL_CREATURE_TYPE:
            case CRITERIA_TYPE_LOOT_TYPE:
            case CRITERIA_TYPE_OWN_ITEM:
            case CRITERIA_TYPE_LOOT_ITEM:
            case CRITERIA_TYPE_CURRENCY:
                SetCriteriaProgress(criteria, miscValue2, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            // std case: high value at miscValue1
            case CRITERIA_TYPE_HIGHEST_AUCTION_BID:
            case CRITERIA_TYPE_HIGHEST_AUCTION_SOLD: /* FIXME: for online player only currently */
            case CRITERIA_TYPE_HIGHEST_HIT_DEALT:
            case CRITERIA_TYPE_HIGHEST_HIT_RECEIVED:
            case CRITERIA_TYPE_HIGHEST_HEAL_CAST:
            case CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED:
            case CRITERIA_TYPE_LEVEL_BATTLE_PET:
            case CRITERIA_TYPE_OWN_HEIRLOOMS:
            case CRITERIA_TYPE_ARTIFACT_TRAITS_UNLOCKED:
                SetCriteriaProgress(criteria, miscValue1, referencePlayer, PROGRESS_HIGHEST);
                break;
            case CRITERIA_TYPE_REACH_LEVEL:
                SetCriteriaProgress(criteria, referencePlayer->getLevel(), referencePlayer);
                break;
            case CRITERIA_TYPE_REACH_SKILL_LEVEL:
                if (uint32 skillvalue = referencePlayer->GetBaseSkillValue(criteria->Entry->Asset.SkillID))
                    SetCriteriaProgress(criteria, skillvalue, referencePlayer);
                break;
            case CRITERIA_TYPE_LEARN_SKILL_LEVEL:
                if (uint32 maxSkillvalue = referencePlayer->GetPureMaxSkillValue(criteria->Entry->Asset.SkillID))
                    SetCriteriaProgress(criteria, maxSkillvalue, referencePlayer);
                break;
            case CRITERIA_TYPE_COMPLETE_QUEST_COUNT:
                SetCriteriaProgress(criteria, referencePlayer->GetRewardedQuestCount(), referencePlayer);
                break;
            case CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY:
            {
                time_t nextDailyResetTime = sWorld->GetNextDailyQuestsResetTime();
                CriteriaProgress *progress = GetCriteriaProgress(criteria);

                if (!miscValue1) // Login case.
                {
                    // reset if player missed one day.
                    if (progress && progress->Date < (nextDailyResetTime - 2 * DAY))
                        SetCriteriaProgress(criteria, 0, referencePlayer, PROGRESS_SET);
                    continue;
                }

                ProgressType progressType;
                if (!progress)
                    // 1st time. Start count.
                    progressType = PROGRESS_SET;
                else if (progress->Date < (nextDailyResetTime - 2 * DAY))
                    // last progress is older than 2 days. Player missed 1 day => Restart count.
                    progressType = PROGRESS_SET;
                else if (progress->Date < (nextDailyResetTime - DAY))
                    // last progress is between 1 and 2 days. => 1st time of the day.
                    progressType = PROGRESS_ACCUMULATE;
                else
                    // last progress is within the day before the reset => Already counted today.
                    continue;

                SetCriteriaProgress(criteria, 1, referencePlayer, progressType);
                break;
            }
            case CRITERIA_TYPE_FALL_WITHOUT_DYING:
                // miscValue1 is the ingame fallheight*100 as stored in db2
                SetCriteriaProgress(criteria, miscValue1, referencePlayer);
                break;
            case CRITERIA_TYPE_COMPLETE_QUEST:
            case CRITERIA_TYPE_LEARN_SPELL:
            case CRITERIA_TYPE_EXPLORE_AREA:
            case CRITERIA_TYPE_VISIT_BARBER_SHOP:
            case CRITERIA_TYPE_EQUIP_EPIC_ITEM:
            case CRITERIA_TYPE_EQUIP_ITEM:
            case CRITERIA_TYPE_COMPLETE_ACHIEVEMENT:
            case CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER:
            case CRITERIA_TYPE_OWN_BATTLE_PET:
                SetCriteriaProgress(criteria, 1, referencePlayer);
                break;
            case CRITERIA_TYPE_BUY_BANK_SLOT:
                SetCriteriaProgress(criteria, referencePlayer->GetBankBagSlotCount(), referencePlayer);
                break;
            case CRITERIA_TYPE_GAIN_REPUTATION:
            {
                int32 reputation = referencePlayer->GetReputationMgr().GetReputation(criteria->Entry->Asset.FactionID);
                if (reputation > 0)
                    SetCriteriaProgress(criteria, reputation, referencePlayer);
                break;
            }
            case CRITERIA_TYPE_GAIN_EXALTED_REPUTATION:
                SetCriteriaProgress(criteria, referencePlayer->GetReputationMgr().GetExaltedFactionCount(), referencePlayer);
                break;
            case CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS:
            case CRITERIA_TYPE_LEARN_SKILL_LINE:
            {
                uint32 spellCount = 0;
                for (PlayerSpellMap::const_iterator spellIter = referencePlayer->GetSpellMap().begin();
                    spellIter != referencePlayer->GetSpellMap().end();
                    ++spellIter)
                {
                    SkillLineAbilityMapBounds bounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellIter->first);
                    for (SkillLineAbilityMap::const_iterator skillIter = bounds.first; skillIter != bounds.second; ++skillIter)
                    {
                        if (skillIter->second->SkillLine == criteria->Entry->Asset.SkillID)
                            spellCount++;
                    }
                }
                SetCriteriaProgress(criteria, spellCount, referencePlayer);
                break;
            }
            case CRITERIA_TYPE_GAIN_REVERED_REPUTATION:
                SetCriteriaProgress(criteria, referencePlayer->GetReputationMgr().GetReveredFactionCount(), referencePlayer);
                break;
            case CRITERIA_TYPE_GAIN_HONORED_REPUTATION:
                SetCriteriaProgress(criteria, referencePlayer->GetReputationMgr().GetHonoredFactionCount(), referencePlayer);
                break;
            case CRITERIA_TYPE_KNOWN_FACTIONS:
                SetCriteriaProgress(criteria, referencePlayer->GetReputationMgr().GetVisibleFactionCount(), referencePlayer);
                break;
            case CRITERIA_TYPE_EARN_HONORABLE_KILL:
                if (!miscValue1)
                    SetCriteriaProgress(criteria, referencePlayer->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS), referencePlayer, PROGRESS_HIGHEST);
                else
                    SetCriteriaProgress(criteria, miscValue1, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            case CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED:
                SetCriteriaProgress(criteria, referencePlayer->GetMoney(), referencePlayer, PROGRESS_HIGHEST);
                break;
            case CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS:
                if (!miscValue1)
                    continue;
                SetCriteriaProgress(criteria, miscValue1, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            case CRITERIA_TYPE_HIGHEST_PERSONAL_RATING:
            {
                uint32 reqTeamType = criteria->Entry->Asset.TeamType;

                if (miscValue1)
                {
                    if (miscValue2 != reqTeamType)
                        continue;

                    SetCriteriaProgress(criteria, miscValue1, referencePlayer, PROGRESS_HIGHEST);
                }

                break;
            }
            case CRITERIA_TYPE_SPENT_GOLD_GUILD_REPAIRS:
            case CRITERIA_TYPE_GAIN_PARAGON_REPUTATION:
            {
                if (!miscValue1)
                    continue;

                SetCriteriaProgress(criteria, miscValue1, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            }
            case CRITERIA_TYPE_REACH_GUILD_LEVEL:
            case CRITERIA_TYPE_REACH_RATED_BATTLEGROUND_RATING:
                SetCriteriaProgress(criteria, miscValue1, referencePlayer);
                break;
            case CRITERIA_TYPE_CRAFT_ITEMS_GUILD:
            case CRITERIA_TYPE_COMPLETE_SCENARIO:
            {
                if (!miscValue1 || !miscValue2)
                    continue;

                SetCriteriaProgress(criteria, miscValue2, referencePlayer, PROGRESS_ACCUMULATE);
                break;
            }
            case CRITERIA_TYPE_APPEARANCE_UNLOCKED_BY_SLOT:
            {
                uint32 l_Count = referencePlayer->GetSession()->GetCollectionMgr()->GetCountOfAppearancesBySlot(miscValue1);
                if (!l_Count)
                    continue;

                SetCriteriaProgress(criteria, l_Count, referencePlayer, PROGRESS_HIGHEST);
                break;
            }

            case CRITERIA_TYPE_LEARN_GARRISON_BLUEPRINT_COUNT:
            {
                if (referencePlayer == nullptr)
                    continue;

                if (auto l_Garrison = referencePlayer->GetDraenorGarrison())
                {
                    SetCriteriaProgress(criteria, l_Garrison->GetKnownBlueprints().size(), referencePlayer, PROGRESS_HIGHEST);
                }

                break;
            }
            case CRITERIA_TYPE_UPGRADE_GARRISON:
            case CRITERIA_TYPE_UPGRADE_GARRISON_BUILDING:
                SetCriteriaProgress(criteria, miscValue1, referencePlayer, PROGRESS_SET);
                break;
            /// no information or non existant criteria in DB2
            case CRITERIA_TYPE_UNK_2:
            case CRITERIA_TYPE_UNK_25:
            case CRITERIA_TYPE_UNK_58:
            case CRITERIA_TYPE_UNK_98:
            case CRITERIA_TYPE_UNK_99:
            case CRITERIA_TYPE_UNK_100:
            case CRITERIA_TYPE_UNK_111:
            case CRITERIA_TYPE_UNK_116:
            case CRITERIA_TYPE_UNK_117:
            case CRITERIA_TYPE_UNK_131:
            case CRITERIA_TYPE_UNK_140:
            case CRITERIA_TYPE_UNK_141:
            case CRITERIA_TYPE_UNK_142:
            case CRITERIA_TYPE_UNK_143:
            case CRITERIA_TYPE_UNK_144:
            case CRITERIA_TYPE_UNK_154:
            case CRITERIA_TYPE_UNK_159:
            case CRITERIA_TYPE_UNK_166:
            case CRITERIA_TYPE_UNK_177:
            case CRITERIA_TYPE_UNK_179:
            case CRITERIA_TYPE_UNK_180:
            case CRITERIA_TYPE_UNK_181:
            case CRITERIA_TYPE_UNK_188:
            case CRITERIA_TYPE_UNK_192:
            case CRITERIA_TYPE_UNK_193:
            case CRITERIA_TYPE_UNK_201:
            case CRITERIA_TYPE_UNK_202:
            case CRITERIA_TYPE_UNK_208:
            case CRITERIA_TYPE_UNK_209:
            case CRITERIA_TYPE_UNK_210:
            case CRITERIA_TYPE_UNK_212:

            /// FIXME: not triggered in code as result, need to implement
            case CRITERIA_TYPE_TRANSMOG_SET_UNLOCKED:   ///< 730
            case CRITERIA_TYPE_EARN_HONOR_XP:           ///< 730
            case CRITERIA_TYPE_RELIC_TALENT_UNLOCKED:   ///< 730
            case CRITERIA_TYPE_LEARN_ALL_CLASS_HALL_TALENT:
            case CRITERIA_TYPE_ESPORT_AWARD:
            case CRITERIA_TYPE_HIGHEST_TEAM_RATING:
            case CRITERIA_TYPE_OWN_RANK:
            case CRITERIA_TYPE_LFR_DUNGEONS_COMPLETED:
            case CRITERIA_TYPE_LFR_LEAVES:
            case CRITERIA_TYPE_LFR_VOTE_KICKS_INITIATED_BY_PLAYER:
            case CRITERIA_TYPE_LFR_VOTE_KICKS_NOT_INIT_BY_PLAYER:
            case CRITERIA_TYPE_BE_KICKED_FROM_LFR:
            case CRITERIA_TYPE_COUNT_OF_LFR_QUEUE_BOOSTS_BY_TANK:
            case CRITERIA_TYPE_CAPTURE_BATTLE_PET_CREDIT:
            case CRITERIA_TYPE_LEVEL_BATTLE_PET_CREDIT:
            case CRITERIA_TYPE_COMPLETE_RAID:
            case CRITERIA_TYPE_START_GARRISON_MISSION:
            case CRITERIA_TYPE_COMPLETE_GARRISON_MISSION_COUNT:
            case CRITERIA_TYPE_RAISE_GARRISON_FOLLOWER_ITEM_LEVEL:
            case CRITERIA_TYPE_RAISE_GARRISON_FOLLOWER_LEVEL:
            case CRITERIA_TYPE_OWN_TOY:
            case CRITERIA_TYPE_PLAY_ARENA:
            case CRITERIA_TYPE_CAPTURE_BATTLE_PET:
            case CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE:
            case CRITERIA_TYPE_MANUAL_COMPLETE_CRITERIA:
            case CRITERIA_TYPE_DEFEAT_CREATURE_GROUP:
            case CRITERIA_TYPE_COOK_RECIPES_GUILD:
            case CRITERIA_TYPE_EARN_PET_BATTLE_ACHIEVEMENT_POINTS:
            case CRITERIA_TYPE_RELEASE_SPIRIT:
            case CRITERIA_TYPE_GARRISON_COMPLETE_DUNGEON_ENCOUNTER:
            case CRITERIA_TYPE_COMPLETE_LFG_DUNGEON:
            case CRITERIA_TYPE_LFG_VOTE_KICKS_INITIATED_BY_PLAYER:
            case CRITERIA_TYPE_LFG_VOTE_KICKS_NOT_INIT_BY_PLAYER:
            case CRITERIA_TYPE_BE_KICKED_FROM_LFG:
            case CRITERIA_TYPE_LFG_LEAVES:
            case CRITERIA_TYPE_COUNT_OF_LFG_QUEUE_BOOSTS_BY_TANK:
            case CRITERIA_TYPE_START_ORDER_HALL_MISSION:
            case CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_WITH_QUALITY:
            case CRITERIA_TYPE_ARTIFACT_POWER_EARNED:
            case CRITERIA_TYPE_ORDER_HALL_TALENT_LEARNED:
            case CRITERIA_TYPE_ORDER_HALL_RECRUIT_TROOP:
                break;
        }

        for (CriteriaTree const* tree : *trees)
        {
            if (IsCompletedCriteriaTree(tree))
                CompletedCriteriaTree(tree, referencePlayer);

            AfterCriteriaTreeUpdate(tree, referencePlayer);
        }
    }
}

void CriteriaHandler::UpdateTimedCriteria(uint32 timeDiff)
{
    if (!_timeCriteriaTrees.empty())
    {
        for (auto itr = _timeCriteriaTrees.begin(); itr != _timeCriteriaTrees.end();)
        {
            // Time is up, remove timer and reset progress
            if (itr->second <= timeDiff)
            {
                CriteriaTree const* criteriaTree = sCriteriaMgr->GetCriteriaTree(itr->first);
                if (criteriaTree->Criteria)
                    RemoveCriteriaProgress(criteriaTree->Criteria);

                itr = _timeCriteriaTrees.erase(itr);
            }
            else
            {
                itr->second -= timeDiff;
                ++itr;
            }
        }
    }
}

void CriteriaHandler::StartCriteriaTimer(CriteriaTimedTypes type, uint32 entry, uint32 timeLost /* = 0 */)
{
    CriteriaList const& criteriaList = sCriteriaMgr->GetTimedCriteriaByType(type);
    for (Criteria const* criteria : criteriaList)
    {
        if (criteria->Entry->StartAsset != entry)
            continue;

        CriteriaTreeList const* trees = sCriteriaMgr->GetCriteriaTreesByCriteria(criteria->ID);
        bool canStart = false;
        for (CriteriaTree const* tree : *trees)
        {
            if (_timeCriteriaTrees.find(tree->ID) == _timeCriteriaTrees.end() && !IsCompletedCriteriaTree(tree))
            {
                // Start the timer
                if (criteria->Entry->StartTimer * uint32(IN_MILLISECONDS) > timeLost)
                {
                    _timeCriteriaTrees[tree->ID] = criteria->Entry->StartTimer * IN_MILLISECONDS - timeLost;
                    canStart = true;
                }
            }
        }

        if (!canStart)
            continue;

        // and at client too
        SetCriteriaProgress(criteria, 0, nullptr, PROGRESS_SET);
    }
}

void CriteriaHandler::RemoveCriteriaTimer(CriteriaTimedTypes type, uint32 entry)
{
    CriteriaList const& criteriaList = sCriteriaMgr->GetTimedCriteriaByType(type);
    for (Criteria const* criteria : criteriaList)
    {
        if (criteria->Entry->StartAsset != entry)
            continue;

        CriteriaTreeList const* trees = sCriteriaMgr->GetCriteriaTreesByCriteria(criteria->ID);
        // Remove the timer from all trees
        for (CriteriaTree const* tree : *trees)
            _timeCriteriaTrees.erase(tree->ID);

        // remove progress
        RemoveCriteriaProgress(criteria, true);
    }
}

CriteriaProgress* CriteriaHandler::GetCriteriaProgress(Criteria const* entry)
{
    auto iter = _criteriaProgress.find(entry->ID);
    if (iter == _criteriaProgress.end())
        return nullptr;

    return &iter->second;
}

void CriteriaHandler::SetCriteriaProgress(Criteria const* criteria, uint64 changeValue, Player* referencePlayer, ProgressType progressType)
{
    // Don't allow to cheat - doing timed criteria without timer active
    CriteriaTreeList const* trees = nullptr;
    if (criteria->Entry->StartTimer)
    {
        trees = sCriteriaMgr->GetCriteriaTreesByCriteria(criteria->ID);
        if (!trees)
            return;

        bool hasTreeForTimed = false;
        for (CriteriaTree const* tree : *trees)
        {
            auto timedIter = _timeCriteriaTrees.find(tree->ID);
            if (timedIter != _timeCriteriaTrees.end())
            {
                hasTreeForTimed = true;
                break;
            }
        }

        if (!hasTreeForTimed)
            return;
    }

    CriteriaProgress* progress = GetCriteriaProgress(criteria);
    if (!progress)
    {
        // not create record for 0 counter but allow it for timed criteria
        // we will need to send 0 progress to client to start the timer
        if (changeValue == 0 && !criteria->Entry->StartTimer)
            return;

        progress = &_criteriaProgress[criteria->ID];
        progress->Counter = changeValue;
    }
    else
    {
        uint64 newValue = 0;
        switch (progressType)
        {
            case PROGRESS_SET:
                newValue = changeValue;
                break;
            case PROGRESS_ACCUMULATE:
            {
                // avoid overflow
                uint64 max_value = std::numeric_limits<uint64>::max();
                newValue = max_value - progress->Counter > changeValue ? progress->Counter + changeValue : max_value;
                break;
            }
            case PROGRESS_HIGHEST:
                newValue = progress->Counter < changeValue ? changeValue : progress->Counter;
                break;
        }

        // not update (not mark as changed) if counter will have same value
        if (progress->Counter == newValue && !criteria->Entry->StartTimer)
            return;

        progress->Counter = newValue;
    }

    progress->Changed = true;
    progress->Date = time(NULL); // set the date to the latest update.
    progress->PlayerGUID = referencePlayer ? referencePlayer->GetGUID() : 0;

    uint32 timeElapsed = 0;

    if (criteria->Entry->StartTimer)
    {
        ASSERT(trees);

        for (CriteriaTree const* tree : *trees)
        {
            auto timedIter = _timeCriteriaTrees.find(tree->ID);
            if (timedIter != _timeCriteriaTrees.end())
            {
                // Client expects this in packet
                timeElapsed = criteria->Entry->StartTimer - (timedIter->second / IN_MILLISECONDS);

                // Remove the timer, we wont need it anymore
                if (IsCompletedCriteriaTree(tree))
                    _timeCriteriaTrees.erase(timedIter);
            }
        }
    }

    SendCriteriaUpdate(criteria, progress, timeElapsed, true);

    if (referencePlayer)
        if (auto inst = referencePlayer->GetMap()->ToInstanceMap())
            if (uint32 instanceId = inst ? inst->GetInstanceId() : 0)
                if (Scenario* scenario = sScenarioMgr->GetScenario(instanceId))
                    scenario->SendCriteriaUpdate(criteria, progress, timeElapsed);
}

void CriteriaHandler::RemoveCriteriaProgress(Criteria const* criteria, bool p_Timed /*= false*/)
{
    if (!criteria)
        return;

    auto criteriaProgress = _criteriaProgress.find(criteria->ID);
    if (criteriaProgress == _criteriaProgress.end())
        return;

    if (p_Timed)
    {
        std::time_t l_Date = time(nullptr);

        criteriaProgress->second.Counter = 1;
        criteriaProgress->second.Date = l_Date;
    }

    SendCriteriaProgressRemoved(criteria->ID, criteria, &criteriaProgress->second);

    _criteriaProgress.erase(criteriaProgress);
}

bool CriteriaHandler::IsCompletedCriteriaTree(CriteriaTree const* tree)
{
    if (!tree)
        return false;

    if (!CanCompleteCriteriaTree(tree))
        return false;

    uint64 requiredCount = tree->Entry->Amount;
    switch (tree->Entry->Operator)
    {
        case CRITERIA_TREE_OPERATOR_SINGLE:
            return tree->Criteria && IsCompletedCriteria(tree->Criteria, requiredCount);
        case CRITERIA_TREE_OPERATOR_SINGLE_NOT_COMPLETED:
            return !tree->Criteria || !IsCompletedCriteria(tree->Criteria, requiredCount);
        case CRITERIA_TREE_OPERATOR_ALL:
            for (CriteriaTree const* node : tree->Children)
                if (!IsCompletedCriteriaTree(node))
                    return false;
            return true;
        case CRITERIA_TREE_OPERAROR_SUM_CHILDREN:
        {
            uint64 progress = 0;
            CriteriaMgr::WalkCriteriaTree(tree, [this, &progress](CriteriaTree const* criteriaTree)
            {
                if (criteriaTree->Criteria)
                    if (CriteriaProgress const* criteriaProgress = GetCriteriaProgress(criteriaTree->Criteria))
                        progress += criteriaProgress->Counter;
            });
            return progress >= requiredCount;
        }
        case CRITERIA_TREE_OPERATOR_MAX_CHILD:
        {
            uint64 progress = 0;
            CriteriaMgr::WalkCriteriaTree(tree, [this, &progress](CriteriaTree const* criteriaTree)
            {
                if (criteriaTree->Criteria)
                    if (CriteriaProgress const* criteriaProgress = GetCriteriaProgress(criteriaTree->Criteria))
                        if (criteriaProgress->Counter > progress)
                            progress = criteriaProgress->Counter;
            });
            return progress >= requiredCount;
        }
        case CRITERIA_TREE_OPERATOR_COUNT_DIRECT_CHILDREN:
        {
            uint64 progress = 0;
            for (CriteriaTree const* node : tree->Children)
                if (node->Criteria)
                    if (CriteriaProgress const* criteriaProgress = GetCriteriaProgress(node->Criteria))
                        if (criteriaProgress->Counter >= 1)
                            if (++progress >= requiredCount)
                                return true;

            return false;
        }
        case CRITERIA_TREE_OPERATOR_ANY:
        {
            uint64 progress = 0;
            for (CriteriaTree const* node : tree->Children)
                if (IsCompletedCriteriaTree(node))
                    if (++progress >= requiredCount)
                        return true;

            return false;
        }
        case CRITERIA_TREE_OPERATOR_SUM_CHILDREN_WEIGHT:
        {
            uint64 progress = 0;
            CriteriaMgr::WalkCriteriaTree(tree, [this, &progress](CriteriaTree const* criteriaTree)
            {
                if (criteriaTree->Criteria)
                    if (CriteriaProgress const* criteriaProgress = GetCriteriaProgress(criteriaTree->Criteria))
                        progress += criteriaProgress->Counter * criteriaTree->Entry->Amount;
             });
             return progress >= requiredCount;
          }
        default:
            break;
    }

    return false;
}

bool CriteriaHandler::CanUpdateCriteriaTree(Criteria const* criteria, CriteriaTree const* tree, Player* referencePlayer) const
{
    if ((tree->Entry->Flags & CRITERIA_TREE_FLAG_HORDE_ONLY && referencePlayer->GetTeam() != HORDE) ||
        (tree->Entry->Flags & CRITERIA_TREE_FLAG_ALLIANCE_ONLY && referencePlayer->GetTeam() != ALLIANCE))
        return false;

    return true;
}

bool CriteriaHandler::CanCompleteCriteriaTree(CriteriaTree const* /*tree*/)
{
    return true;
}

bool CriteriaHandler::IsCompletedCriteria(Criteria const* criteria, uint64 requiredAmount)
{
    CriteriaProgress const* progress = GetCriteriaProgress(criteria);
    if (!progress)
        return false;

    switch (CriteriaTypes(criteria->Entry->Type))
    {
        case CRITERIA_TYPE_WIN_BG:
        case CRITERIA_TYPE_KILL_CREATURE:
        case CRITERIA_TYPE_REACH_LEVEL:
        case CRITERIA_TYPE_REACH_GUILD_LEVEL:
        case CRITERIA_TYPE_REACH_SKILL_LEVEL:
        case CRITERIA_TYPE_COMPLETE_QUEST_COUNT:
        case CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY:
        case CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE:
        case CRITERIA_TYPE_DAMAGE_DONE:
        case CRITERIA_TYPE_HEALING_DONE:
        case CRITERIA_TYPE_COMPLETE_DAILY_QUEST:
        case CRITERIA_TYPE_FALL_WITHOUT_DYING:
        case CRITERIA_TYPE_BE_SPELL_TARGET:
        case CRITERIA_TYPE_BE_SPELL_TARGET2:
        case CRITERIA_TYPE_CAST_SPELL:
        case CRITERIA_TYPE_CAST_SPELL2:
        case CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
        case CRITERIA_TYPE_HONORABLE_KILL_AT_AREA:
        case CRITERIA_TYPE_HONORABLE_KILL:
        case CRITERIA_TYPE_EARN_HONORABLE_KILL:
        case CRITERIA_TYPE_OWN_ITEM:
        case CRITERIA_TYPE_WIN_RATED_ARENA:
        case CRITERIA_TYPE_HIGHEST_PERSONAL_RATING:
        case CRITERIA_TYPE_USE_ITEM:
        case CRITERIA_TYPE_LOOT_ITEM:
        case CRITERIA_TYPE_BUY_BANK_SLOT:
        case CRITERIA_TYPE_GAIN_REPUTATION:
        case CRITERIA_TYPE_GAIN_EXALTED_REPUTATION:
        case CRITERIA_TYPE_VISIT_BARBER_SHOP:
        case CRITERIA_TYPE_EQUIP_EPIC_ITEM:
        case CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
        case CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
        case CRITERIA_TYPE_HK_CLASS:
        case CRITERIA_TYPE_HK_RACE:
        case CRITERIA_TYPE_DO_EMOTE:
        case CRITERIA_TYPE_EQUIP_ITEM:
        case CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD:
        case CRITERIA_TYPE_LOOT_MONEY:
        case CRITERIA_TYPE_USE_GAMEOBJECT:
        case CRITERIA_TYPE_SPECIAL_PVP_KILL:
        case CRITERIA_TYPE_FISH_IN_GAMEOBJECT:
        case CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS:
        case CRITERIA_TYPE_WIN_DUEL:
        case CRITERIA_TYPE_LOOT_TYPE:
        case CRITERIA_TYPE_LEARN_SKILL_LINE:
        case CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS:
        case CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS:
        case CRITERIA_TYPE_GET_KILLING_BLOWS:
        case CRITERIA_TYPE_CURRENCY:
        case CRITERIA_TYPE_PLACE_GARRISON_BUILDING:
        case CRITERIA_TYPE_OWN_BATTLE_PET_COUNT:
        case CRITERIA_TYPE_BUY_GUILD_TABARD:
        case CRITERIA_TYPE_ENTER_AREA:
        case CRITERIA_TYPE_LEAVE_AREA:
        case CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS:
        case CRITERIA_TYPE_CATCH_FROM_POOL:
        case CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS:
        case CRITERIA_TYPE_COMPLETE_QUESTS_GUILD:
        case CRITERIA_TYPE_HONORABLE_KILLS_GUILD:
        case CRITERIA_TYPE_KILL_CREATURE_TYPE_GUILD:
        case CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER:
        case CRITERIA_TYPE_COMPLETE_SCENARIO:
        case CRITERIA_TYPE_COMPLETE_SCENARIO_COUNT:
        case CRITERIA_TYPE_REACH_SCENARIO_BOSS:
        case CRITERIA_TYPE_COMPLETE_CHALLENGE:
        case CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS:
        case CRITERIA_TYPE_OWN_PET:
        case CRITERIA_TYPE_COMPLETE_GARRISON_MISSION:
        case CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE:
        case CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE:
        case CRITERIA_TYPE_WIN_PET_BATTLE:
        case CRITERIA_TYPE_LEVEL_BATTLE_PET:
        case CRITERIA_TYPE_LEARN_GARRISON_BLUEPRINT_COUNT:
        case CRITERIA_TYPE_OWN_HEIRLOOMS:
        case CRITERIA_TYPE_ARTIFACT_TRAITS_UNLOCKED:
        case CRITERIA_TYPE_APPEARANCE_UNLOCKED_BY_SLOT:
        case CRITERIA_TYPE_COMPLETE_WORLD_QUEST:
        case CRITERIA_TYPE_OWN_TOY_COUNT:
        case CRITERIA_TYPE_GAIN_PARAGON_REPUTATION:
        case CRITERIA_TYPE_EARN_HONOR_XP:
        case CRITERIA_TYPE_RELIC_TALENT_UNLOCKED:
        case CRITERIA_TYPE_SEND_EVENT_SCENARIO:
        case CRITERIA_TYPE_SEND_EVENT:
        case CRITERIA_TYPE_HONOR_LEVEL_REACHED:
        case CRITERIA_TYPE_PRESTIGE_REACHED:
        case CRITERIA_TYPE_REACH_LEVEL_2:
        case CRITERIA_TYPE_COMPLETE_GARRISON_SHIPMENT:
        case CRITERIA_TYPE_KILL_CREATURE_TYPE:
        case CRITERIA_TYPE_CLEAR_DIGSITE:
        case CRITERIA_TYPE_SURVEY_GAMEOBJECT:
        case CRITERIA_TYPE_UPGRADE_GARRISON:
        case CRITERIA_TYPE_UPGRADE_GARRISON_BUILDING:
        case CRITERIA_TYPE_CONSTRUCT_GARRISON_BUILDING:
            return progress->Counter >= requiredAmount;
        case CRITERIA_TYPE_COMPLETE_ACHIEVEMENT:
        case CRITERIA_TYPE_COMPLETE_QUEST:
        case CRITERIA_TYPE_LEARN_SPELL:
        case CRITERIA_TYPE_EXPLORE_AREA:
        case CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER:
        case CRITERIA_TYPE_OWN_BATTLE_PET:
            return progress->Counter >= 1;
        case CRITERIA_TYPE_LEARN_SKILL_LEVEL:
            return progress->Counter >= (requiredAmount * 75);
        case CRITERIA_TYPE_WIN_ARENA:
        case CRITERIA_TYPE_WIN_RATED_BATTLEGROUND:
        case CRITERIA_TYPE_REACH_RATED_BATTLEGROUND_RATING:
            return requiredAmount && progress->Counter >= requiredAmount;
        case CRITERIA_TYPE_ON_LOGIN:
            return true;
        // handle all statistic-only criteria here
        case CRITERIA_TYPE_COMPLETE_BATTLEGROUND:
        case CRITERIA_TYPE_DEATH_AT_MAP:
        case CRITERIA_TYPE_DEATH:
        case CRITERIA_TYPE_DEATH_IN_DUNGEON:
        case CRITERIA_TYPE_KILLED_BY_CREATURE:
        case CRITERIA_TYPE_KILLED_BY_PLAYER:
        case CRITERIA_TYPE_DEATHS_FROM:
        case CRITERIA_TYPE_HIGHEST_TEAM_RATING:
        case CRITERIA_TYPE_MONEY_FROM_VENDORS:
        case CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS:
        case CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS:
        case CRITERIA_TYPE_GOLD_SPENT_AT_BARBER:
        case CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL:
        case CRITERIA_TYPE_LOSE_DUEL:
        case CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS:
        case CRITERIA_TYPE_CREATE_AUCTION:
        case CRITERIA_TYPE_HIGHEST_AUCTION_BID:
        case CRITERIA_TYPE_HIGHEST_AUCTION_SOLD:
        case CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED:
        case CRITERIA_TYPE_WON_AUCTIONS:
        case CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE:
        case CRITERIA_TYPE_GAIN_REVERED_REPUTATION:
        case CRITERIA_TYPE_GAIN_HONORED_REPUTATION:
        case CRITERIA_TYPE_KNOWN_FACTIONS:
        case CRITERIA_TYPE_LOOT_EPIC_ITEM:
        case CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
        case CRITERIA_TYPE_ROLL_NEED:
        case CRITERIA_TYPE_ROLL_GREED:
        case CRITERIA_TYPE_QUEST_ABANDONED:
        case CRITERIA_TYPE_FLIGHT_PATHS_TAKEN:
        case CRITERIA_TYPE_ACCEPTED_SUMMONINGS:
        default:
            break;
    }

    return false;
}

bool CriteriaHandler::CanUpdateCriteria(Criteria const* criteria, CriteriaTreeList const* trees, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, Unit const* unit, Player* referencePlayer)
{
    /// @TODO implement the disable type
    ///if (DisableMgr::IsDisabledFor(DISABLE_TYPE_CRITERIA, criteria->ID, nullptr))
    ///{
    ///    sLog->outTrace(LOG_FILTER_ACHIEVEMENTSYS, "CriteriaHandler::CanUpdateCriteria: (Id: %u Type %s) Disabled", criteria->ID, CriteriaMgr::GetCriteriaTypeString(criteria->Entry->Type));
    ///    return false;
    ///}

    bool treeRequirementPassed = false;

    for (CriteriaTree const* tree : *trees)
    {
        if (!CanUpdateCriteriaTree(criteria, tree, referencePlayer))
            continue;

        treeRequirementPassed = true;
        break;
    }

    if (!treeRequirementPassed)
        return false;

    if (!RequirementsSatisfied(criteria, miscValue1, miscValue2, miscValue3, unit, referencePlayer))
        return false;

    if (criteria->Modifier && !AdditionalRequirementsSatisfied(criteria->Modifier, miscValue1, miscValue2, unit, referencePlayer))
        return false;

    if (!ConditionsSatisfied(criteria, referencePlayer))
        return false;

    return true;
}

bool CriteriaHandler::ConditionsSatisfied(Criteria const* criteria, Player* referencePlayer) const
{
    if (!criteria->Entry->FailEvent)
        return true;

    switch (criteria->Entry->FailEvent)
    {
        case CRITERIA_CONDITION_BG_MAP:
            if (!referencePlayer->InBattleground())
                return false;
            break;
        case CRITERIA_CONDITION_NOT_IN_GROUP:
            if (referencePlayer->GetGroup())
                return false;
            break;
        default:
            break;
    }

    return true;
}

bool CriteriaHandler::RequirementsSatisfied(Criteria const* criteria, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, Unit const* unit, Player* referencePlayer) const
{
    switch (CriteriaTypes(criteria->Entry->Type))
    {
        case CRITERIA_TYPE_ACCEPTED_SUMMONINGS:
        case CRITERIA_TYPE_COMPLETE_DAILY_QUEST:
        case CRITERIA_TYPE_CREATE_AUCTION:
        case CRITERIA_TYPE_FALL_WITHOUT_DYING:
        case CRITERIA_TYPE_FLIGHT_PATHS_TAKEN:
        case CRITERIA_TYPE_GET_KILLING_BLOWS:
        case CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS:
        case CRITERIA_TYPE_GOLD_SPENT_AT_BARBER:
        case CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL:
        case CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS:
        case CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING:
        case CRITERIA_TYPE_HIGHEST_AUCTION_BID:
        case CRITERIA_TYPE_HIGHEST_AUCTION_SOLD:
        case CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED:
        case CRITERIA_TYPE_HIGHEST_HEAL_CAST:
        case CRITERIA_TYPE_HIGHEST_HIT_DEALT:
        case CRITERIA_TYPE_HIGHEST_HIT_RECEIVED:
        case CRITERIA_TYPE_HONORABLE_KILL:
        case CRITERIA_TYPE_LOOT_MONEY:
        case CRITERIA_TYPE_LOSE_DUEL:
        case CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD:
        case CRITERIA_TYPE_MONEY_FROM_VENDORS:
        case CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS:
        case CRITERIA_TYPE_QUEST_ABANDONED:
        case CRITERIA_TYPE_REACH_GUILD_LEVEL:
        case CRITERIA_TYPE_ROLL_GREED:
        case CRITERIA_TYPE_ROLL_NEED:
        case CRITERIA_TYPE_SPECIAL_PVP_KILL:
        case CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED:
        case CRITERIA_TYPE_TOTAL_HEALING_RECEIVED:
        case CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS:
        case CRITERIA_TYPE_VISIT_BARBER_SHOP:
        case CRITERIA_TYPE_WIN_DUEL:
        case CRITERIA_TYPE_WIN_RATED_ARENA:
        case CRITERIA_TYPE_WON_AUCTIONS:
        case CRITERIA_TYPE_PRESTIGE_REACHED:
        case CRITERIA_TYPE_REACH_LEVEL_2:
            if (!miscValue1)
                return false;
            break;
        case CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.MapID)
                return false;
            break;
        case CRITERIA_TYPE_BUY_BANK_SLOT:
        case CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY:
        case CRITERIA_TYPE_COMPLETE_QUEST_COUNT:
        case CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS:
        case CRITERIA_TYPE_GAIN_EXALTED_REPUTATION:
        case CRITERIA_TYPE_GAIN_HONORED_REPUTATION:
        case CRITERIA_TYPE_GAIN_REVERED_REPUTATION:
        case CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED:
        case CRITERIA_TYPE_HIGHEST_PERSONAL_RATING:
        case CRITERIA_TYPE_KNOWN_FACTIONS:
        case CRITERIA_TYPE_REACH_LEVEL:
        case CRITERIA_TYPE_ON_LOGIN:
        case CRITERIA_TYPE_COMPLETE_CHALLENGE:
        case CRITERIA_TYPE_KILL_CREATURE_TYPE:
            break;
        case CRITERIA_TYPE_COMPLETE_ACHIEVEMENT:
            if (!RequiredAchievementSatisfied(criteria->Entry->Asset.AchievementID))
                return false;
            break;
        case CRITERIA_TYPE_WIN_BG:
        case CRITERIA_TYPE_COMPLETE_BATTLEGROUND:
        case CRITERIA_TYPE_DEATH_AT_MAP:
            if (!miscValue1 || criteria->Entry->Asset.MapID != referencePlayer->GetMapId())
                return false;
            break;
        case CRITERIA_TYPE_KILL_CREATURE:
        case CRITERIA_TYPE_KILLED_BY_CREATURE:
        case CRITERIA_TYPE_REACH_SCENARIO_BOSS:
        case CRITERIA_TYPE_COMPLETE_SCENARIO:
            if (!miscValue1 || criteria->Entry->Asset.CreatureID != miscValue1)
                return false;
            break;
        case CRITERIA_TYPE_REACH_SKILL_LEVEL:
        case CRITERIA_TYPE_LEARN_SKILL_LEVEL:
            // update at loading or specific skill update
            if (miscValue1 && miscValue1 != criteria->Entry->Asset.SkillID)
                return false;
            break;
        case CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE:
            if (miscValue1 && miscValue1 != criteria->Entry->Asset.ZoneID)
                return false;
            break;
        case CRITERIA_TYPE_DEATH:
        {
            if (!miscValue1)
                return false;
            break;
        }
        case CRITERIA_TYPE_DEATH_IN_DUNGEON:
        {
            if (!miscValue1)
                return false;

            Map const* map = referencePlayer->IsInWorld() ? referencePlayer->GetMap() : sMapMgr->FindMap(referencePlayer->GetMapId(), referencePlayer->GetInstanceId());
            if (!map || !map->IsDungeon())
                return false;

            //FIXME: work only for instances where max == min for players
            if (map->ToInstanceMap()->GetMaxPlayers() != criteria->Entry->Asset.GroupSize)
                return false;
            break;
        }
        case CRITERIA_TYPE_KILLED_BY_PLAYER:
            if (!miscValue1 || !unit || unit->GetTypeId() != TYPEID_PLAYER)
                return false;
            break;
        case CRITERIA_TYPE_DEATHS_FROM:
            if (!miscValue1 || miscValue2 != criteria->Entry->Asset.DamageType)
                return false;
            break;
        case CRITERIA_TYPE_COMPLETE_QUEST:
        {
            // if miscValues != 0, it contains the questID.
            if (miscValue1)
            {
                if (miscValue1 != criteria->Entry->Asset.QuestID)
                    return false;
            }
            else
            {
                // login case.
                if (!referencePlayer->GetQuestRewardStatus(criteria->Entry->Asset.QuestID))
                    return false;
            }

            if (CriteriaDataSet const* data = sCriteriaMgr->GetCriteriaDataSet(criteria))
                if (!data->Meets(referencePlayer, unit))
                    return false;
            break;
        }
        case CRITERIA_TYPE_SEND_EVENT_SCENARIO:
        case CRITERIA_TYPE_SEND_EVENT:
        case CRITERIA_TYPE_CONSTRUCT_GARRISON_BUILDING:
        case CRITERIA_TYPE_UPGRADE_GARRISON:
        case CRITERIA_TYPE_UPGRADE_GARRISON_BUILDING:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.ID)
                return false;
            break;
        case CRITERIA_TYPE_BE_SPELL_TARGET:
        case CRITERIA_TYPE_BE_SPELL_TARGET2:
        case CRITERIA_TYPE_CAST_SPELL:
        case CRITERIA_TYPE_CAST_SPELL2:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.SpellID)
                return false;
            break;
        case CRITERIA_TYPE_LEARN_SPELL:
            if (miscValue1 && miscValue1 != criteria->Entry->Asset.SpellID)
                return false;

            if (!referencePlayer->HasSpell(criteria->Entry->Asset.SpellID))
                return false;
            break;
        case CRITERIA_TYPE_LOOT_TYPE:
            // miscValue1 = itemId - miscValue2 = count of item loot
            // miscValue3 = loot_type (note: 0 = LOOT_CORPSE and then it ignored)
            if (!miscValue1 || !miscValue2 || !miscValue3 || miscValue3 != criteria->Entry->Asset.LootType)
                return false;
            break;
        case CRITERIA_TYPE_OWN_ITEM:
            if (miscValue1 && criteria->Entry->Asset.ItemID != miscValue1)
                return false;
            break;
        case CRITERIA_TYPE_USE_ITEM:
        case CRITERIA_TYPE_LOOT_ITEM:
        case CRITERIA_TYPE_EQUIP_ITEM:
            if (!miscValue1 || criteria->Entry->Asset.ItemID != miscValue1)
                return false;
            break;
        case CRITERIA_TYPE_EXPLORE_AREA:
        {
            WorldMapOverlayEntry const* worldOverlayEntry = sWorldMapOverlayStore.LookupEntry(criteria->Entry->Asset.WorldMapOverlayID);
            if (!worldOverlayEntry)
                break;

            bool matchFound = false;
            for (int j = 0; j < MAX_WORLD_MAP_OVERLAY_AREA_IDX; ++j)
            {
                AreaTableEntry const* area = sAreaTableStore.LookupEntry(worldOverlayEntry->areatableID[j]);
                if (!area)
                    break;

                if (area->AreaBit < 0)
                    continue;

                int exploreFlag = area->AreaBit;

                // Hack: Explore Southern Barrens
                if (criteria->Entry->Asset.WorldMapOverlayID == 3009)
                {
                    exploreFlag = 2936;
                }

                uint16 playerIndexOffset = uint16(uint32(exploreFlag) / 32);
                if (playerIndexOffset >= PLAYER_EXPLORED_ZONES_SIZE)
                    continue;

                uint32 mask = 1 << (uint32(exploreFlag) % 32);
                if (referencePlayer->GetUInt32Value(PLAYER_FIELD_EXPLORED_ZONES + playerIndexOffset) & mask)
                {
                    matchFound = true;
                    break;
                }
            }

            if (!matchFound)
                return false;
            break;
        }
        case CRITERIA_TYPE_GAIN_REPUTATION:
            if (miscValue1 && miscValue1 != criteria->Entry->Asset.FactionID)
                return false;
            break;
        case CRITERIA_TYPE_EQUIP_EPIC_ITEM:
            // miscValue1 = itemid miscValue2 = itemSlot
            if (!miscValue1 || miscValue2 != criteria->Entry->Asset.ItemSlot)
                return false;
            break;
        case CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
        case CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
        {
            // miscValue1 = itemid miscValue2 = diced value
            if (!miscValue1 || miscValue2 != criteria->Entry->Asset.RollValue)
                return false;

            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(uint32(miscValue1));
            if (!proto)
                return false;
            break;
        }
        case CRITERIA_TYPE_DO_EMOTE:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.EmoteID)
                return false;
            break;
        case CRITERIA_TYPE_DAMAGE_DONE:
        case CRITERIA_TYPE_HEALING_DONE:
            if (!miscValue1)
                return false;

            if (criteria->Entry->FailEvent == CRITERIA_CONDITION_BG_MAP)
            {
                if (!referencePlayer->InBattleground())
                    return false;

                // map specific case (BG in fact) expected player targeted damage/heal
                if (!unit || unit->GetTypeId() != TYPEID_PLAYER)
                    return false;
            }
            break;
        case CRITERIA_TYPE_USE_GAMEOBJECT:
        case CRITERIA_TYPE_FISH_IN_GAMEOBJECT:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.GameObjectID)
                return false;
            break;
        case CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS:
        case CRITERIA_TYPE_LEARN_SKILL_LINE:
            if (miscValue1 && miscValue1 != criteria->Entry->Asset.SkillID)
                return false;
            break;
        case CRITERIA_TYPE_LOOT_EPIC_ITEM:
        case CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
        {
            if (!miscValue1)
                return false;
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(uint32(miscValue1));
            if (!proto || proto->Quality < ITEM_QUALITY_EPIC)
                return false;
            break;
        }
        case CRITERIA_TYPE_HK_CLASS:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.ClassID)
                return false;
            break;
        case CRITERIA_TYPE_HK_RACE:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.RaceID)
                return false;
            break;
        case CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.ObjectiveId)
                return false;
            break;
        case CRITERIA_TYPE_HONORABLE_KILL_AT_AREA:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.AreaID)
                return false;
            break;
        case CRITERIA_TYPE_CURRENCY:
            if (!miscValue1 || !miscValue2 || int64(miscValue2) < 0
                || miscValue1 != criteria->Entry->Asset.CurrencyID)
                return false;
            break;
        case CRITERIA_TYPE_WIN_ARENA:
            if (miscValue1 != criteria->Entry->Asset.MapID)
                return false;
            break;
        case CRITERIA_TYPE_HIGHEST_TEAM_RATING:
            return false;
        case CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE:
            if (miscValue1 != criteria->Entry->Asset.GuildChallengeType)
                return false;
            break;
        case CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.DungeonEncounterID)
                return false;
            break;
        case CRITERIA_TYPE_PLACE_GARRISON_BUILDING:
            if (miscValue1 != criteria->Entry->Asset.GarrBuildingID)
                return false;
            break;
        case CRITERIA_TYPE_COMPLETE_GARRISON_MISSION:
            if (miscValue1 != criteria->Entry->Asset.GarrMissionID)
                return false;
            break;
        case CRITERIA_TYPE_OWN_PET:
            if (miscValue1)
            {
                if (miscValue1 != criteria->Entry->Asset.PetEntry)
                    return false;
            }
            // Login case ?
            else
            {
                if (!referencePlayer)
                    return false;

                bool petFound = false;
                auto battlePets = referencePlayer->GetBattlePets();
                for (std::vector<BattlePet::Ptr>::iterator l_It = battlePets.begin(); l_It != battlePets.end(); ++l_It)
                {
                    BattlePet::Ptr l_Pet = (*l_It);
                    BattlePetSpeciesEntry const* l_SpeciesInfo = sBattlePetSpeciesStore.LookupEntry(l_Pet->Species);
                    if (l_SpeciesInfo && l_SpeciesInfo->CreatureID == criteria->Entry->Asset.PetEntry)
                    {
                        petFound = true;
                        break;
                    }
                }

                if (!petFound)
                    return false;
            break;
        }
        case CRITERIA_TYPE_LEVEL_BATTLE_PET:
            if (miscValue1 < criteria->Entry->Asset.PetLevel)
                return false;
            break;
        case CRITERIA_TYPE_APPEARANCE_UNLOCKED_BY_SLOT:
            if (miscValue1 != criteria->Entry->Asset.AppearanceSlot)
                return false;
            break;
        case CRITERIA_TYPE_COMPLETE_GARRISON_SHIPMENT:
            if (!miscValue1 || miscValue1 != criteria->Entry->Asset.ID)
                return false;
            break;
        default:
            break;
    }
    return true;
}

bool CriteriaHandler::AdditionalRequirementsSatisfied(ModifierTreeNode const* tree, uint64 miscValue1, uint64 miscValue2, Unit const* unit, Player* referencePlayer) const
{
    uint8 l_ReqAmount = tree->Entry->Amount;
    uint8 l_OkayCount = 0;
    for (ModifierTreeNode const* node : tree->Children)
    {
        if (AdditionalRequirementsSatisfied(node, miscValue1, miscValue2, unit, referencePlayer))
            ++l_OkayCount;
    }

    if (!tree->Children.empty())
    {
        switch (tree->Entry->Operator)
        {
            case ModifierTreeOperator::OperatorSupOrEqual:
            {
                if (l_OkayCount < l_ReqAmount)
                    return false;
                break;
            }
            default:
            {
                if (l_OkayCount < uint8(tree->Children.size()))
                    return false;
                break;
            }
        }
    }

    uint32 reqType = tree->Entry->Type;
    if (!reqType)
        return true;

    uint32 reqValue = tree->Entry->Asset;

    GroupPtr l_Group = referencePlayer ? referencePlayer->GetGroup() : nullptr;

    switch (CriteriaAdditionalCondition(reqType))
    {
        case CRITERIA_ADDITIONAL_CONDITION_SOURCE_DRUNK_VALUE: ///< 1
            if (referencePlayer->GetDrunkValue() < reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_ITEM_LEVEL: // 3
        {
            // miscValue1 is itemid
            ItemTemplate const* const item = sObjectMgr->GetItemTemplate(uint32(miscValue1));
            if (!item || item->ItemLevel < reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_CREATURE_ENTRY: // 4
        {
            if (!unit)
                return false;

            switch (tree->Entry->Operator)
            {
                case ModifierTreeOperator::OperatorEqual:
                {
                    if (unit->GetEntry() != reqValue)
                        return false;
                    break;
                }
                case ModifierTreeOperator::OperatorDifferent:
                {
                    if (unit->GetEntry() == reqValue)
                        return false;
                    break;
                }
            }
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_PLAYER: // 5
            if (!unit || unit->GetTypeId() != TYPEID_PLAYER)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_DEAD: // 6
            if (!unit || unit->isAlive())
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_MUST_BE_ENEMY: // 7
            if (!unit || !referencePlayer->IsHostileTo(unit))
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_SOURCE_HAS_AURA: // 8
            /// HACK: there is no spell 128648 (7.1.5)
            if (reqValue == 128648)
            {
                /// Criteria for 'Mine Mine Mine!' achievement (Silvershart Mines)
                Battleground* p_Bg = referencePlayer->GetBattleground();
                if (p_Bg == nullptr || p_Bg->GetTypeID(true) != BattlegroundTypeId::BATTLEGROUND_SM || !((BattlegroundSM*)p_Bg)->IsPlayerAnyCartKeeper(referencePlayer))
                    return false;
            }
            else
            {
                bool l_HasAura = referencePlayer->HasAura(reqValue);

                switch (tree->Entry->Operator)
                {
                    case ModifierTreeOperator::OperatorEqual:
                    {
                        /// Must have the aura
                        if (l_ReqAmount > 0 && !l_HasAura)
                            return false;

                        break;
                    }
                    case ModifierTreeOperator::OperatorDifferent:
                    {
                        /// Must not have the aura
                        if (l_ReqAmount > 0 && l_HasAura)
                            return false;

                        break;
                    }
                    default:
                        break;
                }

                return l_HasAura;
            }
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_HAS_AURA: // 10
            if (!unit || !unit->HasAura(reqValue))
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_HAS_AURA_TYPE: // 11
            if (!unit || !unit->HasAuraType(AuraType(reqValue)))
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_ITEM_QUALITY_MIN: // 14
        {
            // miscValue1 is itemid
            ItemTemplate const* const item = sObjectMgr->GetItemTemplate(uint32(miscValue1));
            if (!item || item->Quality < reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_ITEM_QUALITY_EQUALS: // 15
        {
            // miscValue1 is itemid
            ItemTemplate const* const item = sObjectMgr->GetItemTemplate(uint32(miscValue1));
            if (!item || item->Quality != reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_SOURCE_AREA_OR_ZONE: // 17
        {
            uint32 zoneId, areaId;
            referencePlayer->GetZoneAndAreaId(zoneId, areaId);
            if (zoneId != reqValue && areaId != reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_AREA_OR_ZONE: // 18
        {
            if (!unit)
                return false;
            uint32 zoneId, areaId;
            unit->GetZoneAndAreaId(zoneId, areaId);
            if (zoneId != reqValue && areaId != reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_MAP_DIFFICULTY_OLD: ///< 20
        {
            DifficultyEntry const* difficulty = sDifficultyStore.LookupEntry(referencePlayer->GetMap()->GetDifficultyID());
            if (!difficulty || difficulty->OldEnumValue == -1 || uint32(difficulty->OldEnumValue) != reqValue)
                return false;
        }
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_CREATURE_YIELDS_XP:   ///< 21
        {
            if (!unit)
                return false;
            else
            {
                uint32 l_XP = JadeCore::XP::Gain(referencePlayer, const_cast<Unit*>(unit));
                if (!l_XP)
                    return false;
            }
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_ARENA_TYPE: ///< 24
        {
            Battleground* bg = referencePlayer->GetBattleground();
            if (!bg || !bg->isArena() || bg->GetArenaType() != reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_SOURCE_RACE: // 25
            if (referencePlayer->getRace() != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_SOURCE_CLASS: // 26
            if (referencePlayer->getClass() != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_RACE: // 27
            if (!unit || unit->GetTypeId() != TYPEID_PLAYER || unit->getRace() != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_CLASS: // 28
            if (!unit || unit->GetTypeId() != TYPEID_PLAYER || unit->getClass() != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_MAX_GROUP_MEMBERS: // 29
            if (l_Group && l_Group->GetMembersCount() >= reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_CREATURE_TYPE: // 30
        {
            if (!unit)
                return false;
            Creature const* const creature = unit->ToCreature();
            if (!creature || creature->GetCreatureType() != reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_SOURCE_MAP: // 32
            if (referencePlayer->GetMapId() != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_COMPLETE_QUEST_NOT_IN_GROUP: ///< 35
            if (l_Group)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_MIN_PERSONAL_RATING: ///< 36
        {
            Battleground* l_Battleground = referencePlayer->GetBattleground();
            if (!l_Battleground || !l_Battleground->isArena() || !l_Battleground->IsRatedBG())
                return false;
            else
            {
                if (referencePlayer->GetArenaMatchMakerRating((uint8)l_Battleground->GetBattlegroundBracketType()) < reqValue)
                    return false;
            }

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_TITLE_BIT_INDEX: // 38
            // miscValue1 is title's bit index
            if (miscValue1 != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_SOURCE_LEVEL: // 39
            if (referencePlayer->getLevel() != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_LEVEL: // 40
            if (!unit || unit->getLevel() != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_ZONE: // 41
            if (!referencePlayer || referencePlayer->GetZoneId() != reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_HEALTH_PERCENT_BELOW: // 46
            if (!unit || unit->GetHealthPct() >= reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_MIN_ACHIEVEMENT_POINTS:  ///< 56
            if (referencePlayer->GetAchievementPoints() < reqValue)
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_REQUIRES_LFG_GROUP:  ///< 58
            if (!referencePlayer->GetGroup() || !referencePlayer->GetGroup()->isLFGGroup())
                return false;
            break;
        case CRITERIA_ADDITIONAL_CONDITION_REQUIRES_GUILD_GROUP:    ///< 61
        {
            Map* l_Map = referencePlayer->GetMap();
            if (!referencePlayer->GetGroup() || !referencePlayer->GetGuildId() || !l_Map)
                return false;
            else
            {
                uint32 l_GuildID = referencePlayer->GetGuildId();
                GroupPtr l_Group = referencePlayer->GetGroup();
                if (!l_Group->IsGuildGroup() || l_Group->GetGroupGuildID() != l_GuildID)
                    return false;
            }

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_GUILD_REPUTATION:    ///< 62
        {
            if (referencePlayer->GetReputationMgr().GetReputation(REP_GUILD) < int32(reqValue))
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_RATED_BATTLEGROUND:  ///< 63
        {
            /// No check needed, rated BG criterias are only updated at the end of a rated BG
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_PROJECT_RARITY:  ///< 65
        {
            if (!miscValue1)
                return false;

            ResearchProjectEntry const* l_Project = sResearchProjectStore.LookupEntry(miscValue1);
            if (!l_Project)
                return false;
            else
            {
                if (l_Project->Rarity != reqValue)
                    return false;

                if (referencePlayer->GetArchaeologyMgr().IsCompletedProject(l_Project->ID))
                    return false;
            }

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_PROJECT_RACE:    ///< 66
        {
            if (!miscValue1)
                return false;

            ResearchProjectEntry const* l_Project = sResearchProjectStore.LookupEntry(miscValue1);
            if (!l_Project)
                return false;
            else if (l_Project->ResearchBranchID != reqValue)
                return false;

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_WORLD_STATE: ///< 67
        {
            switch (reqValue)
            {
                case InstanceWorldStates::MythicLevel2:
                case InstanceWorldStates::MythicLevel5:
                case InstanceWorldStates::MythicLevel10:
                case InstanceWorldStates::MythicLevel15:
                case eRTKWorldStates::OperaWikket:
                case eRTKWorldStates::OperaWestfall:
                case eRTKWorldStates::OperaBeautifulBeast:
                case eOSWorldStates::TwilightAssist:
                case eOSWorldStates::TwilightDuo:
                case eOSWorldStates::TwilightZone:
                    if (referencePlayer->GetWorldState(reqValue) == 0)
                        return false;
                    break;
                case 1368:  ///< Winter Veil event
                    if (sGameEventMgr->IsActiveEvent(2))
                        return true;
                    break;
                case 7523:  ///< Love is in the air
                    if (sGameEventMgr->IsActiveEvent(8))
                        return true;
                    break;
                case 4546:  ///< Brewfest
                    if (sGameEventMgr->IsActiveEvent(24))
                        return true;
                    break;
                case InstanceWorldStates::NightholdNotReleased:     ///< Cutting Edge: Xavius & Ahead of the Curve: Xavius are now disabled
                case InstanceWorldStates::ToSNotReleased:           ///< Ahead of the Curve: Gul'dan/Helya - Now disabled
                case InstanceWorldStates::KilJaedensAchievement:    ///< Tomb of Sargeras: Kil'jaeden's Dark Souls achievement - Handled in the script
                case InstanceWorldStates::ArgusAchievement:         ///< Antorus the Burning Throne: Stardust Crusaders - Handled in script
                    return false;
                default:
                    WorldStateExpressionEntry const* l_Expression = sWorldStateExpressionStore.LookupEntry(reqValue);
                    if (!l_Expression || !l_Expression->Eval(referencePlayer))
                        return false;
                    break;
            }

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_MAP_DIFFICULTY:  ///< 68
        {
            if (!unit || !unit->GetMap())
                return false;

            switch (tree->Entry->Operator)
            {
                case ModifierTreeOperator::OperatorEqual:
                {
                    if (unit->GetMap()->GetDifficultyID() != reqValue)
                        return false;
                    break;
                }
                case ModifierTreeOperator::OperatorDifferent:
                {
                    if (unit->GetMap()->GetDifficultyID() == reqValue)
                        return false;
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_MIN_LEVEL:    ///< 70
        {
            if (!unit || unit->getLevelForTarget(referencePlayer) < reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_TARGET_MAX_LEVEL:    ///< 71
        {
            if (!unit || unit->getLevelForTarget(referencePlayer) > reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_ACTIVE_SCENARIO: // 74
        {
            if (!referencePlayer)
                return false;

            if (Scenario* progress = sScenarioMgr->GetScenario(referencePlayer->GetMap()->GetScenarioGuid()))
                if (progress->GetScenarioId() != reqValue)
                    return true;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_RATED_BATTLEGROUND_RATING: ///< 64
        {
            if (referencePlayer->GetArenaPersonalRating(uint8(BattlegroundBracketType::Battleground10v10)) < reqValue)
                return false;

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_BATTLEPET_FAMILY: ///< 78
        {
            // Login case ?
            if (!miscValue1)
            {
                if (!referencePlayer)
                    return false;

                bool found = false;
                auto battlePets = referencePlayer->GetBattlePets();
                for (std::vector<BattlePet::Ptr>::iterator l_It = battlePets.begin(); l_It != battlePets.end(); ++l_It)
                {
                    BattlePet::Ptr l_Pet = (*l_It);
                    BattlePetSpeciesEntry const* l_SpeciesInfo = sBattlePetSpeciesStore.LookupEntry(l_Pet->Species);
                    if (l_SpeciesInfo && l_SpeciesInfo->Source == reqValue)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    return false;
            }
            else
            {
                if (miscValue1 != reqValue)
                    return false;
            }
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_HAS_ACHIEVEMENT: ///< 86
            return referencePlayer->HasAchieved(miscValue1);
        case CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_SPECIES: // 91
        {
            // Login case ?
            if (!miscValue1)
            {
                if (!referencePlayer)
                    return false;

                bool found = false;
                auto battlePets = referencePlayer->GetBattlePets();
                for (std::vector<BattlePet::Ptr>::iterator l_It = battlePets.begin(); l_It != battlePets.end(); ++l_It)
                {
                    BattlePet::Ptr l_Pet = (*l_It);
                    if (l_Pet->Species == reqValue)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    return false;
            }
            else
            {
                if (miscValue1 != reqValue)
                    return false;
            }
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_HAS_QUEST_REWARDED: ///< 110
            return referencePlayer->IsQuestRewarded(miscValue1);
        case CRITERIA_ADDITIONAL_CONDITION_ARENA_SEASON:    ///< 125
        {
            if (sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID) != reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_ENTRY:   ///< 144
        case CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_ENTRY_2: ///< 157
        {
            if (!referencePlayer)
                return false;
            auto* garrison = referencePlayer->GetGarrison();
            if (!garrison)
                return false;
            auto* follower = garrison->GetFollower(reqValue);
            if (!follower)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_QUALITY: // 145
        {
            if (!referencePlayer)
                return false;
            auto* garrison = referencePlayer->GetGarrison();
            if (!garrison)
                return false;
            auto* follower = garrison->GetFollower(miscValue1);
            if (!follower || follower->Quality != reqValue)
                return false;

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_LEVEL: // 146
        {
            if (!referencePlayer)
                return false;
            auto* garrison = referencePlayer->GetGarrison();
            if (!garrison)
                return false;
            auto* follower = garrison->GetFollower(miscValue1);
            if (!follower || follower->Level < reqValue)
                return false;

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_ILVL: // 184
        {
            if (!referencePlayer)
                return false;
            auto* garrison = referencePlayer->GetGarrison();
            if (!garrison)
                return false;
            auto* follower = garrison->GetFollower(miscValue1);
            if (!follower || ((follower->ItemLevelArmor + follower->ItemLevelWeapon) / 2) < reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_GARRISON_FOLLOWER_TYPE: // 187
        {
            if (!referencePlayer)
                return false;

            auto* l_Garrison = referencePlayer->GetGarrison();
            if (!l_Garrison)
                return false;

            auto* l_Follower = l_Garrison->GetFollower(miscValue1);
            if (!l_Follower || l_Follower->GetEntry() == nullptr || l_Follower->GetEntry()->Type != reqValue)
                return false;

            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_HONOR_LEVEL: ///< 193
        {
            if (!referencePlayer || referencePlayer->GetHonorLevel() != reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_PRESTIGE_LEVEL: ///< 194
        {
            if (!referencePlayer || referencePlayer->GetPrestige() < reqValue)
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_HAS_GARRISON_TALENT: ///< 202
        {
            if (!referencePlayer || !referencePlayer->GetGarrison() || !referencePlayer->GetGarrison()->HasResearchedTalent(reqValue))
                return false;
            break;
        }
        case CRITERIA_ADDITIONAL_CONDITION_WORLD_QUEST_TYPE: ///< 206
        {
            auto l_Quest = MS::WorldQuest::Template::GetTemplate(miscValue2);

            if (!l_Quest || l_Quest->GetWorldQuestType() != reqValue)
                return false;

            break;
        }
        /// NYI, don't allow to earn achievements
        case CRITERIA_ADDITIONAL_CONDITION_BATTLEPET_MASTER_PET_TAMER:
        case CRITERIA_ADDITIONAL_CONDITION_BATTLE_PET_ID:
        case CRITERIA_ADDITIONAL_CONDITION_BATTLEPET_WIN_IN_PVP:
        case CRITERIA_ADDITIONAL_CONDITION_UNK_151:
        case CRITERIA_ADDITIONAL_CONDITION_UNK_152:
        case CRITERIA_ADDITIONAL_CONDITION_UNK_153:
        case CRITERIA_ADDITIONAL_CONDITION_UNK_154:
        case CRITERIA_ADDITIONAL_CONDITION_UNK_155:
            return false;
        default:
            break;
    }

    return true;
}

char const* CriteriaMgr::GetCriteriaTypeString(uint32 type)
{
    return GetCriteriaTypeString(CriteriaTypes(type));
}

char const* CriteriaMgr::GetCriteriaTypeString(CriteriaTypes type)
{
    switch (type)
    {
        case CRITERIA_TYPE_KILL_CREATURE:
            return "KILL_CREATURE";
        case CRITERIA_TYPE_WIN_BG:
            return "TYPE_WIN_BG";
        case CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS:
            return "COMPLETE_RESEARCH";
        case CRITERIA_TYPE_SURVEY_GAMEOBJECT:
            return "SURVEY_GAMEOBJECT";
        case CRITERIA_TYPE_REACH_LEVEL:
            return "REACH_LEVEL";
        case CRITERIA_TYPE_CLEAR_DIGSITE:
            return "CLEAR_DIGSITE";
        case CRITERIA_TYPE_REACH_SKILL_LEVEL:
            return "REACH_SKILL_LEVEL";
        case CRITERIA_TYPE_COMPLETE_ACHIEVEMENT:
            return "COMPLETE_ACHIEVEMENT";
        case CRITERIA_TYPE_COMPLETE_QUEST_COUNT:
            return "COMPLETE_QUEST_COUNT";
        case CRITERIA_TYPE_COMPLETE_DAILY_QUEST_DAILY:
            return "COMPLETE_DAILY_QUEST_DAILY";
        case CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE:
            return "COMPLETE_QUESTS_IN_ZONE";
        case CRITERIA_TYPE_CURRENCY:
            return "CURRENCY";
        case CRITERIA_TYPE_DAMAGE_DONE:
            return "DAMAGE_DONE";
        case CRITERIA_TYPE_COMPLETE_DAILY_QUEST:
            return "COMPLETE_DAILY_QUEST";
        case CRITERIA_TYPE_COMPLETE_BATTLEGROUND:
            return "COMPLETE_BATTLEGROUND";
        case CRITERIA_TYPE_DEATH_AT_MAP:
            return "DEATH_AT_MAP";
        case CRITERIA_TYPE_DEATH:
            return "DEATH";
        case CRITERIA_TYPE_DEATH_IN_DUNGEON:
            return "DEATH_IN_DUNGEON";
        case CRITERIA_TYPE_COMPLETE_RAID:
            return "COMPLETE_RAID";
        case CRITERIA_TYPE_KILLED_BY_CREATURE:
            return "KILLED_BY_CREATURE";
        case CRITERIA_TYPE_MANUAL_COMPLETE_CRITERIA:
            return "MANUAL_COMPLETE_CRITERIA";
        case CRITERIA_TYPE_COMPLETE_CHALLENGE:
            return "COMPLETE_CHALLENGE";
        case CRITERIA_TYPE_KILLED_BY_PLAYER:
            return "KILLED_BY_PLAYER";
        case CRITERIA_TYPE_FALL_WITHOUT_DYING:
            return "FALL_WITHOUT_DYING";
        case CRITERIA_TYPE_DEATHS_FROM:
            return "DEATHS_FROM";
        case CRITERIA_TYPE_COMPLETE_QUEST:
            return "COMPLETE_QUEST";
        case CRITERIA_TYPE_BE_SPELL_TARGET:
            return "BE_SPELL_TARGET";
        case CRITERIA_TYPE_CAST_SPELL:
            return "CAST_SPELL";
        case CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE:
            return "BG_OBJECTIVE_CAPTURE";
        case CRITERIA_TYPE_HONORABLE_KILL_AT_AREA:
            return "HONORABLE_KILL_AT_AREA";
        case CRITERIA_TYPE_WIN_ARENA:
            return "WIN_ARENA";
        case CRITERIA_TYPE_PLAY_ARENA:
            return "PLAY_ARENA";
        case CRITERIA_TYPE_LEARN_SPELL:
            return "LEARN_SPELL";
        case CRITERIA_TYPE_HONORABLE_KILL:
            return "HONORABLE_KILL";
        case CRITERIA_TYPE_OWN_ITEM:
            return "OWN_ITEM";
        case CRITERIA_TYPE_WIN_RATED_ARENA:
            return "WIN_RATED_ARENA";
        case CRITERIA_TYPE_HIGHEST_TEAM_RATING:
            return "HIGHEST_TEAM_RATING";
        case CRITERIA_TYPE_HIGHEST_PERSONAL_RATING:
            return "HIGHEST_PERSONAL_RATING";
        case CRITERIA_TYPE_LEARN_SKILL_LEVEL:
            return "LEARN_SKILL_LEVEL";
        case CRITERIA_TYPE_USE_ITEM:
            return "USE_ITEM";
        case CRITERIA_TYPE_LOOT_ITEM:
            return "LOOT_ITEM";
        case CRITERIA_TYPE_EXPLORE_AREA:
            return "EXPLORE_AREA";
        case CRITERIA_TYPE_OWN_RANK:
            return "OWN_RANK";
        case CRITERIA_TYPE_BUY_BANK_SLOT:
            return "BUY_BANK_SLOT";
        case CRITERIA_TYPE_GAIN_REPUTATION:
            return "GAIN_REPUTATION";
        case CRITERIA_TYPE_GAIN_EXALTED_REPUTATION:
            return "GAIN_EXALTED_REPUTATION";
        case CRITERIA_TYPE_VISIT_BARBER_SHOP:
            return "VISIT_BARBER_SHOP";
        case CRITERIA_TYPE_EQUIP_EPIC_ITEM:
            return "EQUIP_EPIC_ITEM";
        case CRITERIA_TYPE_ROLL_NEED_ON_LOOT:
            return "ROLL_NEED_ON_LOOT";
        case CRITERIA_TYPE_ROLL_GREED_ON_LOOT:
            return "GREED_ON_LOOT";
        case CRITERIA_TYPE_HK_CLASS:
            return "HK_CLASS";
        case CRITERIA_TYPE_HK_RACE:
            return "HK_RACE";
        case CRITERIA_TYPE_DO_EMOTE:
            return "DO_EMOTE";
        case CRITERIA_TYPE_HEALING_DONE:
            return "HEALING_DONE";
        case CRITERIA_TYPE_GET_KILLING_BLOWS:
            return "GET_KILLING_BLOWS";
        case CRITERIA_TYPE_EQUIP_ITEM:
            return "EQUIP_ITEM";
        case CRITERIA_TYPE_MONEY_FROM_VENDORS:
            return "MONEY_FROM_VENDORS";
        case CRITERIA_TYPE_GOLD_SPENT_FOR_TALENTS:
            return "GOLD_SPENT_FOR_TALENTS";
        case CRITERIA_TYPE_NUMBER_OF_TALENT_RESETS:
            return "NUMBER_OF_TALENT_RESETS";
        case CRITERIA_TYPE_MONEY_FROM_QUEST_REWARD:
            return "MONEY_FROM_QUEST_REWARD";
        case CRITERIA_TYPE_GOLD_SPENT_FOR_TRAVELLING:
            return "GOLD_SPENT_FOR_TRAVELLING";
        case CRITERIA_TYPE_DEFEAT_CREATURE_GROUP :
            return "DEFEAT_CREATURE_GROUP";
        case CRITERIA_TYPE_GOLD_SPENT_AT_BARBER:
            return "GOLD_SPENT_AT_BARBER";
        case CRITERIA_TYPE_GOLD_SPENT_FOR_MAIL:
            return "GOLD_SPENT_FOR_MAIL";
        case CRITERIA_TYPE_LOOT_MONEY:
            return "LOOT_MONEY";
        case CRITERIA_TYPE_USE_GAMEOBJECT:
            return "USE_GAMEOBJECT";
        case CRITERIA_TYPE_BE_SPELL_TARGET2:
            return "BE_SPELL_TARGET2";
        case CRITERIA_TYPE_SPECIAL_PVP_KILL:
            return "SPECIAL_PVP_KILL";
        case CRITERIA_TYPE_COMPLETE_CHALLENGE_MODE:
            return "COMPLETE_CHALLENGE_MODE";
        case CRITERIA_TYPE_FISH_IN_GAMEOBJECT:
            return "FISH_IN_GAMEOBJECT";
        case CRITERIA_TYPE_SEND_EVENT:
            return "SEND_EVENT";
        case CRITERIA_TYPE_ON_LOGIN:
            return "ON_LOGIN";
        case CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS:
            return "LEARN_SKILLLINE_SPELLS";
        case CRITERIA_TYPE_WIN_DUEL:
            return "WIN_DUEL";
        case CRITERIA_TYPE_LOSE_DUEL:
            return "LOSE_DUEL";
        case CRITERIA_TYPE_KILL_CREATURE_TYPE:
            return "KILL_CREATURE_TYPE";
        case CRITERIA_TYPE_COOK_RECIPES_GUILD:
            return "COOK_RECIPE_GUILD";
        case CRITERIA_TYPE_GOLD_EARNED_BY_AUCTIONS:
            return "GOLD_EARNED_BY_AUCTIONS";
        case CRITERIA_TYPE_EARN_PET_BATTLE_ACHIEVEMENT_POINTS:
            return "EARN_PET_BATTLE_ACHIEVEMENT_POINTS";
        case CRITERIA_TYPE_CREATE_AUCTION:
            return "CREATE_AUCTION";
        case CRITERIA_TYPE_HIGHEST_AUCTION_BID:
            return "HIGHEST_AUCTION_BID";
        case CRITERIA_TYPE_WON_AUCTIONS:
            return "WON_AUCTIONS";
        case CRITERIA_TYPE_HIGHEST_AUCTION_SOLD:
            return "HIGHEST_AUCTION_SOLD";
        case CRITERIA_TYPE_HIGHEST_GOLD_VALUE_OWNED:
            return "HIGHEST_GOLD_VALUE_OWNED";
        case CRITERIA_TYPE_GAIN_REVERED_REPUTATION:
            return "GAIN_REVERED_REPUTATION";
        case CRITERIA_TYPE_GAIN_HONORED_REPUTATION:
            return "GAIN_HONORED_REPUTATION";
        case CRITERIA_TYPE_KNOWN_FACTIONS:
            return "KNOWN_FACTIONS";
        case CRITERIA_TYPE_LOOT_EPIC_ITEM:
            return "LOOT_EPIC_ITEM";
        case CRITERIA_TYPE_RECEIVE_EPIC_ITEM:
            return "RECEIVE_EPIC_ITEM";
        case CRITERIA_TYPE_SEND_EVENT_SCENARIO:
            return "SEND_EVENT_SCENARIO";
        case CRITERIA_TYPE_ROLL_NEED:
            return "ROLL_NEED";
        case CRITERIA_TYPE_ROLL_GREED:
            return "ROLL_GREED";
        case CRITERIA_TYPE_RELEASE_SPIRIT:
            return "RELEASE_SPIRIT";
        case CRITERIA_TYPE_OWN_PET:
            return "OWN_PET";
        case CRITERIA_TYPE_GARRISON_COMPLETE_DUNGEON_ENCOUNTER:
           return "GARRISON_COMPLETE_DUNGEON_ENCOUNTER";
        case CRITERIA_TYPE_HIGHEST_HIT_DEALT:
            return "HIT_DEALT";
        case CRITERIA_TYPE_HIGHEST_HIT_RECEIVED:
            return "HIT_RECEIVED";
        case CRITERIA_TYPE_TOTAL_DAMAGE_RECEIVED:
            return "TOTAL_DAMAGE_RECEIVED";
        case CRITERIA_TYPE_HIGHEST_HEAL_CAST:
            return "HIGHEST_HEAL_CAST";
        case CRITERIA_TYPE_TOTAL_HEALING_RECEIVED:
            return "TOTAL_HEALING_RECEIVED";
        case CRITERIA_TYPE_HIGHEST_HEALING_RECEIVED:
            return "HIGHEST_HEALING_RECEIVED";
        case CRITERIA_TYPE_QUEST_ABANDONED:
            return "QUEST_ABANDONED";
        case CRITERIA_TYPE_FLIGHT_PATHS_TAKEN:
            return "FLIGHT_PATHS_TAKEN";
        case CRITERIA_TYPE_LOOT_TYPE:
            return "LOOT_TYPE";
        case CRITERIA_TYPE_CAST_SPELL2:
            return "CAST_SPELL2";
        case CRITERIA_TYPE_LEARN_SKILL_LINE:
            return "LEARN_SKILL_LINE";
        case CRITERIA_TYPE_EARN_HONORABLE_KILL:
            return "EARN_HONORABLE_KILL";
        case CRITERIA_TYPE_ACCEPTED_SUMMONINGS:
            return "ACCEPTED_SUMMONINGS";
        case CRITERIA_TYPE_EARN_ACHIEVEMENT_POINTS:
            return "EARN_ACHIEVEMENT_POINTS";
        case CRITERIA_TYPE_COMPLETE_LFG_DUNGEON:
            return "COMPLETE_LFG_DUNGEON";
        case CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS:
            return "USE_LFD_TO_GROUP_WITH_PLAYERS";
        case CRITERIA_TYPE_LFG_VOTE_KICKS_INITIATED_BY_PLAYER:
            return "LFG_VOTE_KICKS_INITIATED_BY_PLAYER";
        case CRITERIA_TYPE_LFG_VOTE_KICKS_NOT_INIT_BY_PLAYER:
            return "LFG_VOTE_KICKS_NOT_INIT_BY_PLAYER";
        case CRITERIA_TYPE_BE_KICKED_FROM_LFG:
            return "BE_KICKED_FROM_LFG";
        case CRITERIA_TYPE_LFG_LEAVES:
            return "LFG_LEAVES";
        case CRITERIA_TYPE_SPENT_GOLD_GUILD_REPAIRS:
            return "SPENT_GOLD_GUILD_REPAIRS";
        case CRITERIA_TYPE_REACH_GUILD_LEVEL:
            return "REACH_GUILD_LEVEL";
        case CRITERIA_TYPE_CRAFT_ITEMS_GUILD:
            return "CRAFT_ITEMS_GUILD";
        case CRITERIA_TYPE_CATCH_FROM_POOL:
            return "CATCH_FROM_POOL";
        case CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS:
            return "BUY_GUILD_BANK_SLOTS";
        case CRITERIA_TYPE_EARN_GUILD_ACHIEVEMENT_POINTS:
            return "EARN_GUILD_ACHIEVEMENT_POINTS";
        case CRITERIA_TYPE_WIN_RATED_BATTLEGROUND:
            return "WIN_RATED_BATTLEGROUND";
        case CRITERIA_TYPE_REACH_RATED_BATTLEGROUND_RATING:
            return "REACH_RATED_BATTLEGROUND_RATING";
        case CRITERIA_TYPE_BUY_GUILD_TABARD:
            return "BUY_GUILD_TABARD";
        case CRITERIA_TYPE_COMPLETE_QUESTS_GUILD:
            return "COMPLETE_QUESTS_GUILD";
        case CRITERIA_TYPE_HONORABLE_KILLS_GUILD:
            return "HONORABLE_KILLS_GUILD";
        case CRITERIA_TYPE_KILL_CREATURE_TYPE_GUILD:
            return "KILL_CREATURE_TYPE_GUILD";
        case CRITERIA_TYPE_COUNT_OF_LFG_QUEUE_BOOSTS_BY_TANK:
            return "COUNT_OF_LFG_QUEUE_BOOSTS_BY_TANK";
        case CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE:
            return "GUILD_CHALLENGE_TYPE";
        case CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE:
            return "GUILD_CHALLENGE";
        case CRITERIA_TYPE_LFR_DUNGEONS_COMPLETED:
            return "LFR_DUNGEONS_COMPLETED";
        case CRITERIA_TYPE_LFR_LEAVES:
            return "LFR_LEAVES";
        case CRITERIA_TYPE_LFR_VOTE_KICKS_INITIATED_BY_PLAYER:
            return "LFR_VOTE_KICKS_INITIATED_BY_PLAYER";
        case CRITERIA_TYPE_LFR_VOTE_KICKS_NOT_INIT_BY_PLAYER:
            return "LFR_VOTE_KICKS_NOT_INIT_BY_PLAYER";
        case CRITERIA_TYPE_BE_KICKED_FROM_LFR:
            return "BE_KICKED_FROM_LFR";
        case CRITERIA_TYPE_COUNT_OF_LFR_QUEUE_BOOSTS_BY_TANK:
            return "COUNT_OF_LFR_QUEUE_BOOSTS_BY_TANK";
        case CRITERIA_TYPE_COMPLETE_SCENARIO_COUNT:
            return "COMPLETE_SCENARIO_COUNT";
        case CRITERIA_TYPE_COMPLETE_SCENARIO:
            return "COMPLETE_SCENARIO";
        case CRITERIA_TYPE_REACH_SCENARIO_BOSS:
            return "CRITERIA_TYPE_REACH_SCENARIO_BOSS";
        case CRITERIA_TYPE_OWN_BATTLE_PET:
            return "OWN_BATTLE_PET";
        case CRITERIA_TYPE_OWN_BATTLE_PET_COUNT:
            return "OWN_BATTLE_PET_COUNT";
        case CRITERIA_TYPE_CAPTURE_BATTLE_PET:
            return "CAPTURE_BATTLE_PET";
        case CRITERIA_TYPE_WIN_PET_BATTLE:
            return "WIN_PET_BATTLE";
        case CRITERIA_TYPE_LEVEL_BATTLE_PET:
            return "LEVEL_BATTLE_PET";
        case CRITERIA_TYPE_CAPTURE_BATTLE_PET_CREDIT:
            return "CAPTURE_BATTLE_PET_CREDIT";
        case CRITERIA_TYPE_LEVEL_BATTLE_PET_CREDIT:
            return "LEVEL_BATTLE_PET_CREDIT";
        case CRITERIA_TYPE_ENTER_AREA:
            return "ENTER_AREA";
        case CRITERIA_TYPE_LEAVE_AREA:
            return "LEAVE_AREA";
        case CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER:
            return "COMPLETE_DUNGEON_ENCOUNTER";
        case CRITERIA_TYPE_PLACE_GARRISON_BUILDING:
            return "PLACE_GARRISON_BUILDING";
        case CRITERIA_TYPE_UPGRADE_GARRISON_BUILDING:
            return "UPGRADE_GARRISON_BUILDING";
        case CRITERIA_TYPE_CONSTRUCT_GARRISON_BUILDING:
            return "CONSTRUCT_GARRISON_BUILDING";
        case CRITERIA_TYPE_UPGRADE_GARRISON:
            return "UPGRADE_GARRISON";
        case CRITERIA_TYPE_START_GARRISON_MISSION:
            return "START_GARRISON_MISSION";
        case CRITERIA_TYPE_START_ORDER_HALL_MISSION:
            return "START_ORDER_HALL_MISSION";
        case CRITERIA_TYPE_COMPLETE_GARRISON_MISSION_COUNT:
            return "COMPLETE_GARRISON_MISSION_COUNT";
        case CRITERIA_TYPE_COMPLETE_GARRISON_MISSION:
            return "COMPLETE_GARRISON_MISSION";
        case CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_2:
            return "CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_2";
        case CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER:
            return "RECRUIT_GARRISON_FOLLOWER";
        case CRITERIA_TYPE_LEARN_GARRISON_BLUEPRINT_COUNT:
            return "LEARN_GARRISON_BLUEPRINT_COUNT";
        case CRITERIA_TYPE_COMPLETE_GARRISON_SHIPMENT:
            return "COMPLETE_GARRISON_SHIPMENT";
        case CRITERIA_TYPE_RAISE_GARRISON_FOLLOWER_ITEM_LEVEL:
            return "RAISE_GARRISON_FOLLOWER_ITEM_LEVEL";
        case CRITERIA_TYPE_RAISE_GARRISON_FOLLOWER_LEVEL:
            return "RAISE_GARRISON_FOLLOWER_LEVEL";
        case CRITERIA_TYPE_OWN_TOY:
            return "OWN_TOY";
        case CRITERIA_TYPE_OWN_TOY_COUNT:
            return "OWN_TOY_COUNT";
        case CRITERIA_TYPE_RECRUIT_GARRISON_FOLLOWER_WITH_QUALITY:
            return "RECRUIT_GARRISON_FOLLOWER_WITH_QUALITY";
        case CRITERIA_TYPE_OWN_HEIRLOOMS:
            return "OWN_HEIRLOOMS";
        case CRITERIA_TYPE_ARTIFACT_POWER_EARNED:
            return "ARTIFACT_POWER_EARNED";
        case CRITERIA_TYPE_ARTIFACT_TRAITS_UNLOCKED:
            return "ARTIFACT_TRAITS_UNLOCKED";
        case CRITERIA_TYPE_HONOR_LEVEL_REACHED:
            return "HONOR_LEVEL_REACHED";
        case CRITERIA_TYPE_PRESTIGE_REACHED:
            return "PRESTIGE_REACHED";
        case CRITERIA_TYPE_ORDER_HALL_TALENT_LEARNED:
            return "ORDER_HALL_TALENT_LEARNED";
        case CRITERIA_TYPE_APPEARANCE_UNLOCKED_BY_SLOT:
            return "APPEARANCE_UNLOCKED_BY_SLOT";
        case CRITERIA_TYPE_ORDER_HALL_RECRUIT_TROOP:
            return "ORDER_HALL_RECRUIT_TROOP";
        case CRITERIA_TYPE_COMPLETE_WORLD_QUEST:
            return "COMPLETE_WORLD_QUEST";
        case CRITERIA_TYPE_REACH_LEVEL_2:
            return "REACH_LEVEL_2";
        case CRITERIA_TYPE_LEARN_ALL_CLASS_HALL_TALENT:
            return "LEARN_ALL_CLASS_HALL_TALENT";
        case CRITERIA_TYPE_ESPORT_AWARD:
            return "ESPORT_AWARD";
        case CRITERIA_TYPE_TRANSMOG_SET_UNLOCKED:
            return "TRANSMOG_SET_UNLOCKED";
        case CRITERIA_TYPE_GAIN_PARAGON_REPUTATION:
            return "GAIN_PARAGON_REPUTATION";
        case CRITERIA_TYPE_EARN_HONOR_XP:
            return "EARN_HONOR_XP";
        case CRITERIA_TYPE_RELIC_TALENT_UNLOCKED:
            return "RELIC_TALENT_UNLOCKED";
        case CRITERIA_TYPE_UNK_2:
        case CRITERIA_TYPE_UNK_25:
        case CRITERIA_TYPE_UNK_58:
        case CRITERIA_TYPE_UNK_98:
        case CRITERIA_TYPE_UNK_99:
        case CRITERIA_TYPE_UNK_100:
        case CRITERIA_TYPE_UNK_111:
        case CRITERIA_TYPE_UNK_116:
        case CRITERIA_TYPE_UNK_117:
        case CRITERIA_TYPE_UNK_131:
        case CRITERIA_TYPE_UNK_140:
        case CRITERIA_TYPE_UNK_141:
        case CRITERIA_TYPE_UNK_142:
        case CRITERIA_TYPE_UNK_143:
        case CRITERIA_TYPE_UNK_144:
        case CRITERIA_TYPE_UNK_154:
        case CRITERIA_TYPE_UNK_159:
        case CRITERIA_TYPE_UNK_166:
        case CRITERIA_TYPE_UNK_177:
        case CRITERIA_TYPE_UNK_179:
        case CRITERIA_TYPE_UNK_180:
        case CRITERIA_TYPE_UNK_181:
        case CRITERIA_TYPE_UNK_188:
        case CRITERIA_TYPE_UNK_192:
        case CRITERIA_TYPE_UNK_193:
        case CRITERIA_TYPE_UNK_201:
        case CRITERIA_TYPE_UNK_202:
        case CRITERIA_TYPE_UNK_208:
        case CRITERIA_TYPE_UNK_209:
        case CRITERIA_TYPE_UNK_210:
        case CRITERIA_TYPE_UNK_212:
            return "MISSING_TYPE";
    }
    return "MISSING_TYPE";
}

CriteriaMgr* CriteriaMgr::Instance()
{
    static CriteriaMgr instance;
    return &instance;
}

//==========================================================
CriteriaMgr::~CriteriaMgr()
{
    for (auto itr = _criteriaTrees.begin(); itr != _criteriaTrees.end(); ++itr)
        delete itr->second;

    for (auto itr = _criteria.begin(); itr != _criteria.end(); ++itr)
        delete itr->second;

    for (auto itr = _criteriaModifiers.begin(); itr != _criteriaModifiers.end(); ++itr)
        delete itr->second;
}

void CriteriaMgr::LoadCriteriaModifiersTree()
{
    uint32 oldMSTime = getMSTime();

    if (sModifierTreeStore.GetNumRows() == 0)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING,  ">> Loaded 0 criteria modifiers.");
        return;
    }

    // Load modifier tree nodes
    for (uint32 i = 0; i < sModifierTreeStore.GetNumRows(); ++i)
    {
        ModifierTreeEntry const* tree = sModifierTreeStore.LookupEntry(i);
        if (!tree)
            continue;

        ModifierTreeNode* node = new ModifierTreeNode();
        node->Entry = tree;
        _criteriaModifiers[node->Entry->ID] = node;
    }

    // Build tree
    for (auto itr = _criteriaModifiers.begin(); itr != _criteriaModifiers.end(); ++itr)
    {
        if (!itr->second->Entry->Parent)
            continue;

        auto parent = _criteriaModifiers.find(itr->second->Entry->Parent);
        if (parent != _criteriaModifiers.end())
            parent->second->Children.push_back(itr->second);
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING,  ">> Loaded %u criteria modifiers in %u ms", uint32(_criteriaModifiers.size()), GetMSTimeDiffToNow(oldMSTime));
}

template<typename T>
T GetEntry(std::unordered_map<uint32, T> const& map, CriteriaTreeEntry const* tree)
{
    CriteriaTreeEntry const* cur = tree;
    auto itr = map.find(tree->ID);
    while (itr == map.end())
    {
        if (!cur->Parent)
            break;

        cur = sCriteriaTreeStore.LookupEntry(cur->Parent);
        if (!cur)
            break;

        itr = map.find(cur->ID);
    }

    if (itr == map.end())
        return nullptr;

    return itr->second;
};

std::vector<CriteriaTreeEntry const*> const* CriteriaMgr::GetCriteriaTreeList(uint32 parent)
{
    CriteriaTreeContainer::const_iterator itr = _criteriaTree.find(parent);
    if (itr != _criteriaTree.end())
        return &itr->second;

    return nullptr;
}

void CriteriaMgr::LoadCriteriaList()
{
    uint32 oldMSTime = getMSTime();

    if (sCriteriaTreeStore.GetNumRows() == 0)
    {
        sLog->outError(LOG_FILTER_SERVER_LOADING,  ">> Loaded 0 criteria.");
        return;
    }

    std::unordered_map<uint32 /*criteriaTreeID*/, AchievementEntry const*> achievementCriteriaTreeIds;

    for (int l_I = 0; l_I < sAchievementStore.GetNumRows(); ++l_I)
    {
        AchievementEntry const* achievement = sAchievementStore.LookupEntry(l_I);

        if (!achievement)
            continue;

        achievementCriteriaTreeIds[achievement->CriteriaTree] = achievement;
    }

    std::unordered_map<uint32 /*criteriaTreeID*/, ScenarioStepEntry const*> scenarioCriteriaTreeIds;
    for (uint32 i = 0; i < sScenarioStepStore.GetNumRows(); i++)
    {
        ScenarioStepEntry const* scenarioStep = sScenarioStepStore.LookupEntry(i);
        if (!scenarioStep)
            continue;

        if (scenarioStep->CriteriaTreeID)
            scenarioCriteriaTreeIds[scenarioStep->CriteriaTreeID] = scenarioStep;
    }

    // Load criteria tree nodes

    for (int l_I = 0; l_I < sCriteriaTreeStore.GetNumRows(); ++l_I)
    {
        CriteriaTreeEntry const* tree = sCriteriaTreeStore.LookupEntry(l_I);

        if (!tree)
            continue;

        if (tree->Parent)
            _criteriaTree[tree->Parent].push_back(tree);

        // Find linked achievement
        AchievementEntry const* achievement = GetEntry(achievementCriteriaTreeIds, tree);
        ScenarioStepEntry const* scenarioStep = GetEntry(scenarioCriteriaTreeIds, tree);

        CriteriaTree* criteriaTree = new CriteriaTree();
        criteriaTree->ID = tree->ID;
        criteriaTree->Achievement = achievement;
        criteriaTree->ScenarioStep = scenarioStep;
        criteriaTree->Entry = tree;

        _criteriaTrees[criteriaTree->Entry->ID] = criteriaTree;
    }

    // Build tree
    for (auto itr = _criteriaTrees.begin(); itr != _criteriaTrees.end(); ++itr)
    {
        if (!itr->second->Entry->Parent)
            continue;

        auto parent = _criteriaTrees.find(itr->second->Entry->Parent);
        if (parent != _criteriaTrees.end())
        {
            parent->second->Children.push_back(itr->second);
            while (parent != _criteriaTrees.end())
            {
                auto cur = parent;
                parent = _criteriaTrees.find(parent->second->Entry->Parent);
                if (parent == _criteriaTrees.end())
                {
                    if (sCriteriaStore.LookupEntry(itr->second->Entry->CriteriaID))
                        _criteriaTreeByCriteria[itr->second->Entry->CriteriaID].push_back(cur->second);
                }
            }
        }
        else if (sCriteriaStore.LookupEntry(itr->second->Entry->CriteriaID))
            _criteriaTreeByCriteria[itr->second->Entry->CriteriaID].push_back(itr->second);
    }

    // Load criteria
    uint32 criterias = 0;
    uint32 guildCriterias = 0;
    uint32 scenarioCriterias = 0;

    for (int l_I = 0; l_I < sCriteriaStore.GetNumRows(); ++l_I)
    {
        CriteriaEntry const* criteriaEntry = sCriteriaStore.LookupEntry(l_I);

        if (!criteriaEntry)
            continue;

        auto treeItr = _criteriaTreeByCriteria.find(criteriaEntry->ID);
        if (treeItr == _criteriaTreeByCriteria.end())
            continue;

        Criteria* criteria = new Criteria();
        criteria->ID = criteriaEntry->ID;
        criteria->Entry = criteriaEntry;
        auto mod = _criteriaModifiers.find(criteriaEntry->ModifierTreeId);
        if (mod != _criteriaModifiers.end())
            criteria->Modifier = mod->second;

        _criteria[criteria->ID] = criteria;

        for (CriteriaTree const* tree : treeItr->second)
        {
            if (AchievementEntry const* achievement = tree->Achievement)
            {
                if (achievement->Flags & ACHIEVEMENT_FLAG_GUILD)
                    criteria->FlagsCu |= CRITERIA_FLAG_CU_GUILD;
                else if (achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT)
                    criteria->FlagsCu |= CRITERIA_FLAG_CU_ACCOUNT;
                else
                    criteria->FlagsCu |= CRITERIA_FLAG_CU_PLAYER;
            }
            else if (tree->ScenarioStep)
                criteria->FlagsCu |= CRITERIA_FLAG_CU_SCENARIO;
            else
                criteria->FlagsCu |= CRITERIA_FLAG_CU_PLAYER;
        }

        if (criteria->FlagsCu & (CRITERIA_FLAG_CU_PLAYER | CRITERIA_FLAG_CU_ACCOUNT))
        {
            ++criterias;
            _criteriasByType[criteriaEntry->Type].push_back(criteria);
        }

        if (criteria->FlagsCu & CRITERIA_FLAG_CU_GUILD)
        {
            ++guildCriterias;
            _guildCriteriasByType[criteriaEntry->Type].push_back(criteria);
        }

        if (criteria->FlagsCu & CRITERIA_FLAG_CU_SCENARIO)
        {
            ++scenarioCriterias;
            _scenarioCriteriasByType[criteriaEntry->Type].push_back(criteria);
        }

        if (criteriaEntry->StartTimer)
            _criteriasByTimedType[criteriaEntry->StartEvent].push_back(criteria);
    }

    for (auto& p : _criteriaTrees)
        const_cast<CriteriaTree*>(p.second)->Criteria = GetCriteria(p.second->Entry->CriteriaID);

    sLog->outInfo(LOG_FILTER_SERVER_LOADING,  ">> Loaded %u criteria, %u guild criteria and %u scenario criteria in %u ms.", criterias, guildCriterias, scenarioCriterias, GetMSTimeDiffToNow(oldMSTime));
}

void CriteriaMgr::LoadCriteriaData()
{
    uint32 oldMSTime = getMSTime();

    _criteriaDataMap.clear();                              // need for reload case


    uint32 count = 0;

    for (uint8 l_I = 0; l_I < 2; ++l_I)
    {
        QueryResult result;
        if (l_I == 0)
            result = WorldDatabase.Query("SELECT criteria_id, type, value1, value2, ScriptName FROM criteria_data");
        else
            result = WorldDatabase.Query("SELECT criteria_id, type, value1, value2, ScriptName FROM achievement_criteria_data");

        if (!result)
        {
            sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 additional criteria data. DB table `criteria_data` is empty.");
            return;
        }

        do
        {
            Field* fields = result->Fetch();
            uint32 criteria_id = fields[0].GetUInt32();

            Criteria const* criteria = GetCriteria(criteria_id);

            if (!criteria)
            {
                sLog->outError(LOG_FILTER_SQL, "Table `criteria_data` contains data for non-existing criteria (Entry: %u). Ignored.", criteria_id);
                continue;
            }

            uint32 dataType = fields[1].GetUInt8();
            std::string scriptName = fields[4].GetString();
            uint32 scriptId = 0;
            if (!scriptName.empty())
            {
                if (dataType != CRITERIA_DATA_TYPE_SCRIPT)
                    sLog->outError(LOG_FILTER_SQL, "Table `criteria_data` contains a ScriptName for non-scripted data type (Entry: %u, type %u), useless data.", criteria_id, dataType);
                else
                    scriptId = sObjectMgr->GetScriptId(scriptName.c_str());
            }

            CriteriaData data(dataType, fields[2].GetUInt32(), fields[3].GetUInt32(), scriptId);

            if (!data.IsValid(criteria))
                continue;

            // this will allocate empty data set storage
            CriteriaDataSet& dataSet = _criteriaDataMap[criteria_id];
            dataSet.SetCriteriaId(criteria_id);

            // add real data only for not NONE data types
            if (data.DataType != CRITERIA_DATA_TYPE_NONE)
                dataSet.Add(data);

            // counting data by and data types
            ++count;
        } while (result->NextRow());

    }

    LoadAccountCriteriaList();

    sLog->outInfo(LOG_FILTER_SERVER_LOADING,  ">> Loaded %u additional criteria data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void CriteriaMgr::LoadAccountCriteriaList()
{
    for (int l_I = 0; l_I < sAchievementStore.GetNumRows(); ++l_I)
    {
        AchievementEntry const* l_Achievement = sAchievementStore.LookupEntry(l_I);
        LoadAccountCriteriaList(l_Achievement);
    }
}

void CriteriaMgr::LoadAccountCriteriaList(AchievementEntry const* p_Achievement)
{
    /// ACHIEVEMENT_FLAG_ACCOUNT check is disabled because root Achievement flag all criterias, and
    /// sub achievement's criterias as account bind
    if (!p_Achievement || (p_Achievement->Flags & ACHIEVEMENT_FLAG_ACCOUNT) == 0)
        return;

    CriteriaTree const* l_Tree = GetCriteriaTree(p_Achievement->CriteriaTree);

    if (l_Tree)
        LoadAccountCriteriaList(l_Tree);
}

void CriteriaMgr::LoadAccountCriteriaList(CriteriaTree const* p_Tree)
{
    if (!p_Tree)
        return;

    if (p_Tree->Criteria)
    {
        const_cast<Criteria*>(p_Tree->Criteria)->AccountBind = true;

        if (CriteriaEntry const* l_Entry = p_Tree->Criteria->Entry)
        {
            /// Account bind achievement bind all sub achievements
            //if (l_Entry->Type == CRITERIA_TYPE_COMPLETE_ACHIEVEMENT)
                //LoadAccountCriteriaList(sAchievementStore.LookupEntry(l_Entry->Asset.AchievementID));
        }
    }

    for (auto l_Current : p_Tree->Children)
        LoadAccountCriteriaList(l_Current);
}

CriteriaTree const* CriteriaMgr::GetCriteriaTree(uint32 criteriaTreeId) const
{
    auto itr = _criteriaTrees.find(criteriaTreeId);
    if (itr == _criteriaTrees.end())
        return nullptr;

    return itr->second;
}

Criteria const* CriteriaMgr::GetCriteria(uint32 criteriaId) const
{
    auto itr = _criteria.find(criteriaId);
    if (itr == _criteria.end())
        return nullptr;

    return itr->second;
}

ModifierTreeNode const* CriteriaMgr::GetModifierTree(uint32 modifierTreeId) const
{
    auto itr = _criteriaModifiers.find(modifierTreeId);
    if (itr != _criteriaModifiers.end())
        return itr->second;

    return nullptr;
}
