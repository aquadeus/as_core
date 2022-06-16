////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once


#include "Common.h"
#include "SharedDefines.h"
#include "GarrisonMgrStructures.hpp"

#define GARRISON_MISSION_DISTRIB_FOLLOWER_COEFF (1.35f)

namespace MS { namespace Garrison
{
    namespace Globals
    {
        enum
        {
            MaxFollowerPerMission           = 3,
            FollowerActivationCost          = (250 * GOLD),
            FollowerActivationMaxStack      = 1,
            MissionDistributionInterval     = (25 * MINUTE),
        };
    }

    namespace FactionIndex
    {
        enum Type : uint8
        {
            Horde       = 0,
            Alliance    = 1,
            Max         = 2
        };
    }

    namespace Mission
    {
        enum State : uint8
        {
            Offered                 = 0,
            InProgress              = 1,
            Completed               = 2,
            OvermaxRewardPending    = 3,
            Reward1Claimed          = 4,
            Reward2Claimed          = 5,
            Reward3Claimed          = 6
        };

        enum Flags : uint8
        {
            IsRare         = 0x01,
            IsElite        = 0x02,
            AppliesFatigue = 0x04,
            AlwaysFail     = 0x08,
            IsZoneSupport  = 0x10
        };

        enum BonusRollResults : uint8
        {
            Ok    = 0,
            Error = 1
        };

        enum MissionType
        {
            QuestMission = 70
        };

        enum SupportMission : uint32
        {
            /// Alliance
            ZoneSupportAszunaA       = 778,
            ZoneSupportSuramarA      = 779,
            ZoneSupportValSharahA    = 780,
            ZoneSupportHighmountainA = 781,
            ZoneSupportStormheimA    = 782,
            ZoneSupportBrokenShoreA  = 783,
            ZoneSupportThalDranathA  = 784,

            /// Horde
            ZoneSupportAszunaH       = 879,
            ZoneSupportHighmountainH = 880,
            ZoneSupportStormheimH    = 881,
            ZoneSupportSuramarH      = 882,
            ZoneSupportValSharahH    = 883,

            CombatAllyMission        = 989
        };
    }

    namespace GarrisonType
    {
        enum Type : uint32
        {
            GarrisonDraenor     = 2,
            GarrisonBrokenIsles = 3,
            Max
        };
    }

    namespace PlotTypes
    {
        enum
        {
            Small           = 0,
            Medium          = 1,
            Large           = 2,
            Farm            = 3,
            Mine            = 4,
            FishingHut      = 5,
            PetMenagerie    = 6,
            Shipyard        = 7,
            Max             = 8
        };
    }

    namespace ResultEnum
    {
        enum Type
        {
            SUCCESS                                            = 0,
            ERROR_NO_GARRISON                                  = 1,
            ERROR_GARRISON_EXISTS                              = 2,
            ERROR_GARRISON_SAME_TYPE_EXISTS                    = 3,
            ERROR_INVALID_GARRISON                             = 4,
            ERROR_INVALID_GARRISON_LEVEL                       = 5,
            ERROR_GARRISON_LEVEL_UNCHANGED                     = 6,
            ERROR_NOT_IN_GARRISON                              = 7,
            ERROR_NO_BUILDING                                  = 8,
            ERROR_BUILDING_EXISTS                              = 9,
            ERROR_INVALID_PLOT_INSTANCEID                      = 10,
            ERROR_INVALID_BUILDINGID                           = 11,
            ERROR_INVALID_UPGRADE_LEVEL                        = 12,
            ERROR_UPGRADE_LEVEL_EXCEEDS_GARRISON_LEVEL         = 13,
            ERROR_PLOTS_NOT_FULL                               = 14,
            ERROR_INVALID_SITE_ID                              = 15,
            ERROR_INVALID_PLOT_BUILDING                        = 16,
            ERROR_INVALID_FACTION                              = 17,
            ERROR_INVALID_SPECIALIZATION                       = 18,
            ERROR_SPECIALIZATION_EXISTS                        = 19,
            ERROR_SPECIALIZATION_ON_COOLDOWN                   = 20,
            ERROR_BLUEPRINT_EXISTS                             = 21,
            ERROR_REQUIRES_BLUEPRINT                           = 22,
            ERROR_INVALID_DOODAD_SET_ID                        = 23,
            ERROR_BUILDING_TYPE_EXISTS                         = 24,
            ERROR_BUILDING_NOT_ACTIVE                          = 25,
            ERROR_CONSTRUCTION_COMPLETE                        = 26,
            ERROR_FOLLOWER_EXISTS                              = 27,
            ERROR_INVALID_FOLLOWER                             = 28,
            ERROR_FOLLOWER_ALREADY_ON_MISSION                  = 29,
            ERROR_FOLLOWER_IN_BUILDING                         = 30,
            ERROR_FOLLOWER_INVALID_FOR_BUILDING                = 31,
            ERROR_INVALID_FOLLOWER_LEVEL                       = 32,
            ERROR_MISSION_EXISTS                               = 33,
            ERROR_INVALID_MISSION                              = 34,
            ERROR_INVALID_MISSION_TIME                         = 35,
            ERROR_INVALID_MISSION_REWARD_INDEX                 = 36,
            ERROR_MISSION_NOT_OFFERED                          = 37,
            ERROR_ALREADY_ON_MISSION                           = 38,
            ERROR_MISSION_SIZE_INVALID                         = 39,
            ERROR_FOLLOWER_SOFT_CAP_EXCEEDED                   = 40,
            ERROR_NOT_ON_MISSION                               = 41,
            ERROR_ALREADY_COMPLETED_MISSION                    = 42,
            ERROR_MISSION_NOT_COMPLETE                         = 43,
            ERROR_MISSION_REWARDS_PENDING                      = 44,
            ERROR_MISSION_EXPIRED                              = 45,
            ERROR_NOT_ENOUGH_CURRENCY                          = 46,
            ERROR_NOT_ENOUGH_GOLD                              = 47,
            ERROR_BUILDING_MISSING                             = 48,
            ERROR_NO_ARCHITECT                                 = 49,
            ERROR_ARCHITECT_NOT_AVAILABLE                      = 50,
            ERROR_NO_MISSION_NPC                               = 51,
            ERROR_MISSION_NPC_NOT_AVAILABLE                    = 52,
            ERROR_INTERNAL__ERROR                              = 53,
            ERROR_INVALID_STATIC_TABLE_VALUE                   = 54,
            ERROR_INVALID_ITEM_LEVEL                           = 55,
            ERROR_INVALID_AVAILABLE_RECRUIT                    = 56,
            ERROR_FOLLOWER_ALREADY_RECRUITED                   = 57,
            ERROR_RECRUITMENT_GENERATION_IN_PROGRESS           = 58,
            ERROR_RECRUITMENT_ON_COOLDOWN                      = 59,
            ERROR_RECRUIT_BLOCKED_BY_GENERATION                = 60,
            ERROR_RECRUITMENT_NPC_NOT_AVAILABLE                = 61,
            ERROR_INVALID_FOLLOWER_QUALITY                     = 62,
            ERROR_PROXY_NOT_OK                                 = 63,
            ERROR_RECALL_PORTAL_USED_LESS_THAN_24_HOURS_AGO    = 64,
            ERROR_ON_REMOVE_BUILDING_SPELL_FAILED              = 65,
            ERROR_OPERATION_NOT_SUPPORTED                      = 66,
            ERROR_FOLLOWER_FATIGUED                            = 67,
            ERROR_UPGRADE_CONDITION_FAILED                     = 68,
            ERROR_FOLLOWER_INACTIVE                            = 69,
            ERROR_FOLLOWER_ACTIVE                              = 70,
            ERROR_FOLLOWER_ACTIVATION_UNAVAILABLE              = 71,
            ERROR_FOLLOWER_TYPE_MISMATCH                       = 72,
            ERROR_INVALID_GARRISON_TYPE                        = 73,
            ERROR_MISSION_START_CONDITION_FAILED               = 74,
            ERROR_INVALID_FOLLOWER_ABILITY                     = 75,
            ERROR_INVALID_MISSION_BONUS_ABILITY                = 76,
            ERROR_HIGHER_BUILDING_TYPE_EXISTS                  = 77,
            ERROR_AT_FOLLOWER_HARD_CAP                         = 78,
            ERROR_FOLLOWER_CANNOT_GAIN_XP                      = 79,
            ERROR_NO_OP                                        = 80,
            ERROR_AT_CLASS_SPEC_CAP                            = 81,
            ERROR_MISSION_REQUIRES_100_TO_START                = 82,
            ERROR_MISSION_MISSING_REQUIRED_FOLLOWER            = 83,
            ERROR_INVALID_TALENT                               = 84,
            ERROR_ALREADY_RESEARCHING_TALENT                   = 85,
            ERROR_FAILED_CONDITION                             = 86,
            ERROR_INVALID_TIER                                 = 87,
            ERROR_INVALID_CLASS                                = 88
        };
    }

    namespace AbilityEffectTypes
    {
        enum Type
        {
            ModCounterMechanic            = 0,    ///< @TODO
            ModSoloMission                = 1,    ///< Proc if MissionFollowerCount == 1
            ModsuccessChance              = 2,    ///< Proc every time
            ModFollowerSoftCap            = 3,    ///< Updated in 7.2.0 (Old was ModTravelTime)
            ModXp                         = 4,    ///< Mod the XP earn (self, party)
            ModFriendlyRace               = 5,    ///< Proc if Find(MissionFollowers[Class], MiscValueA) != NULL
            ModLongMission                = 6,    ///< Proc if Duration > (3600 * MiscValueB)
            ModShortMission               = 7,    ///< Proc if Duration < (3600 * MiscValueB)
            ModCurrency                   = 8,    ///< Implemented.
            ModLongTravel                 = 9,    ///< Proc if TravelDuration > (3600 * MiscValueB)
            ModShortTravel                = 10,   ///< Proc if TravelDuration < (3600 * MiscValueB)
            ModBias                       = 11,   ///< NYI
            ModProfession                 = 12,   ///< NYI
            ModBronzeTreasureDrop         = 13,   ///< NYI
            ModSilverTreasureDrop         = 14,   ///< NYI
            ModGoldTreasureDrop           = 15,   ///< NYI
            ModChestDropRate              = 16,   ///< NYI
            ModMissionTime                = 17,   ///< Proc every time
            ModMentoring                  = 18,   ///< NYI
            ModGold                       = 19,   ///< NYI
            ModPreventDeath               = 20,   ///< NYI
            ModTreasureOnMission          = 21,   ///< NYI
            ModFriendlyClass              = 22,   ///< Implemented in Mission::InitMission
            ModAdvantageMechanic          = 23,   ///< NYI
            ModSuccessPerDurability       = 24,   ///< Implemented in Mission::InitMission
            ModSuccessDurabilityInRange   = 25,   ///< NYI
            ModFriendlyFollower           = 26,   ///< Implemented in Mission::InitMission
            ModKillTroops                 = 27,   ///< Implemented in Mission::InitMission
            OneTroopRegeneratesHp         = 28,   ///< NYI
            ModBonusChestChanceFlat       = 29,   ///< Implemented in Mission::InitMission
            ModItemLevel                  = 30,   ///< NYI
            ModStartingDurability         = 31,   ///< NYI
            ModUniqueTroop                = 32,   ///< NYI
            ModClassSpecificLimit         = 33,   ///< NYI
            ModMaxTroopAmount             = 34,   ///< Implemented in Follower::GetMaxCount()
            CanRecoverDeadTroops           = 35,  ///< Implemented in Mission::InitMission
            ModRewardOnWorldQuestComplete = 36,   ///< NYI
            ModRewardFromWQWithCombatAlly = 37,   ///< Completing WQ with combat ally gives order ressources
            ModMissionCost                = 39    ///< Implemented in Mission::InitMission
        };
    }

    namespace AbilityEffectTargetMasks
    {
        enum Type
        {
            Unk     = 0 << 0,
            Self    = 1 << 0,
            Party   = 1 << 1
        };
    }

    namespace MechanicTypes
    {
        enum Type
        {
            Environment = 0,
            Racial      = 1,
            Ability     = 2
        };
    }

    /// WorldState : See WorldState.db2
    namespace WorldStates
    {
        enum Type
        {

        };
    }

    namespace BuildingCategory
    {
        enum Type
        {
            Prebuilt    = 0,
            UserBuilt   = 1
        };
    }

    enum eGarrisonFollowerTypeID
    {
        FollowerTypeIDDraenor   = 1,
        FollowerTypeIDShipyard  = 2,
        FollowerTypeIDClassHall = 4
    };

    namespace FollowerFlags
    {
        enum Type : uint32
        {
            Favorite     = 0x01,
            isFatigued   = 0x02,
            isInactive   = 0x04,
            isLimitedUse = 0x08,
            NoXPGain     = 0x10
        };
    }

    namespace FollowerTypeFlags
    {
        enum : uint32
        {
            CanRenameFollower           = 1,    ///< The specified type can be renamed, eg ships - LUA RenameFollower
            CanStartMissionPastSoftCap  = 2,    ///< You can start a mission above follower soft cap - LUA AllowMissionStartAboveSoftCap
            Unk1                        = 4,    ///< Unk, only regular follower has it
            Unk2                        = 8     ///< Unk, only ships have it
        };
    }

    static const uint32 gGarrisonBuildingActivationGameObject[FactionIndex::Max] =
    {
        233248,     ///< Horde
        233250      ///< Alliance
    };

    enum eFollowerSoftCaps
    {
        SoftCapDraenor = 20,
        SoftCapShipyard = 6,
        SoftCapClassHall = 6
    };

    enum eGarrisonAbilityFlags
    {
        CannotBeObtainedRandomly = 0x2,
        HordeOnly = 0x4,
        AllianceOnly = 0x8,
        Permanent = 0x10,
        Exclusive = 0x20
    };
}   ///< namespace Garrison
}   ///< namespace MS


