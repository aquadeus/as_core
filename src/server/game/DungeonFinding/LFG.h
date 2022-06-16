////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LFG_H
#define _LFG_H

#include "Common.h"

enum LfgRoles : uint8
{
    LFG_ROLEMASK_NONE                                    = 0x00,
    LFG_ROLEMASK_LEADER                                  = 0x01,
    LFG_ROLEMASK_TANK                                    = 0x02,
    LFG_ROLEMASK_HEALER                                  = 0x04,
    LFG_ROLEMASK_DAMAGE                                  = 0x08,
    LFG_ROLEMASK_FULL                                    = LFG_ROLEMASK_TANK | LFG_ROLEMASK_HEALER | LFG_ROLEMASK_DAMAGE
};

/// Last update: 7.0.3 22522
enum LfgUpdateType : uint8
{
    LFG_UPDATETYPE_DEFAULT                   = 0,       ///< Internal Use
    LFG_UPDATETYPE_LEADER_UNK1               = 1,       ///< FIXME: At group leave
    LFG_UPDATETYPE_ROLECHECK_ABORTED         = 6,       ///< FIXME: name
    LFG_UPDATETYPE_JOIN_QUEUE                = 24,
    LFG_UPDATETYPE_ROLECHECK_FAILED          = 7,

    LFG_UPDATETYPE_REMOVED_FROM_QUEUE        = 25,
    LFG_UPDATETYPE_PROPOSAL_FAILED           = 9,
    LFG_UPDATETYPE_PROPOSAL_DECLINED         = 0,
    LFG_UPDATETYPE_GROUP_FOUND               = 11,
    LFG_UPDATETYPE_ADDED_TO_QUEUE            = 13,
    LFG_UPDATETYPE_PROPOSAL_BEGIN            = 14,      ///< In flex - PVP_SUSPENDED_QUEUE_STATUS
    LFG_UPDATETYPE_UPDATE_STATUS             = 15,
    LFG_UPDATETYPE_GROUP_MEMBER_OFFLINE      = 16,
    LFG_UPDATETYPE_GROUP_DISBAND_UNK16       = 17,      ///< FIXME: Sometimes at group disband
    LFG_UPDATETYPE_PARTY_ROLE_NOT_AVAILABLE  = 44,
    LFG_UPDATETYPE_LFG_OBJECT_FAILED         = 46,
    LFG_UPDATETYPE_REMOVED_LEVELUP           = 47,
    LFG_UPDATETYPE_REMOVED_XP_TOGGLE         = 48,
    LFG_UPDATETYPE_REMOVED_FACTION_CHANGE    = 49,
    LFG_UPDATETYPE_SUSPEND_QUEUE             = 52,
};

enum LfgState : uint8
{
    LFG_STATE_NONE,                                        // Not using LFG / LFR
    LFG_STATE_ROLECHECK,                                   // Rolecheck active
    LFG_STATE_QUEUED,                                      // Queued
    LFG_STATE_PROPOSAL,                                    // Proposal active
    LFG_STATE_BOOT,                                        // Vote kick active
    LFG_STATE_DUNGEON,                                     // In LFG Group, in a Dungeon
    LFG_STATE_FINISHED_DUNGEON,                            // In LFG Group, in a finished Dungeon
    LFG_STATE_RAIDBROWSER                                  // Using Raid finder
};

/// Instance lock types
/// Last update: 7.0.3 22522
enum LfgLockStatusType : uint16
{
    LFG_LOCKSTATUS_OK                        = 0,      ///< Internal use only
    LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION    = 1,
    LFG_LOCKSTATUS_TOO_LOW_LEVEL             = 2,
    LFG_LOCKSTATUS_TOO_HIGH_LEVEL            = 3,
    LFG_LOCKSTATUS_TOO_LOW_GEAR_SCORE        = 4,
    LFG_LOCKSTATUS_TOO_HIGH_GEAR_SCORE       = 5,
    LFG_LOCKSTATUS_RAID_LOCKED               = 6,
    LFG_LOCKSTATUS_TARGET_LEVEL_TOO_HIGH     = 7,      ///< Not used in lockedReason
    LFG_LOCKSTATUS_TARGET_LEVEL_TOO_LOW      = 8,      ///< Not used in lockedReason
    LFG_LOCKSTATUS_AREA_NOT_EXPLORED         = 9,
    LFG_LOCKSTATUS_WRONG_FACTION             = 10,     ///< LFG_INSTANCE_INVALID_WRONG_FACTION - not used in lockedReason
    LFG_LOCKSTATUS_NOT_COMLETE_CHALANGE      = 11,     ///< Not complete challenge (scenario)
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_LOW_LEVEL  = 1001,
    LFG_LOCKSTATUS_ATTUNEMENT_TOO_HIGH_LEVEL = 1002,
    LFG_LOCKSTATUS_QUEST_NOT_COMPLETED       = 1022,
    LFG_LOCKSTATUS_MISSING_ITEM              = 1025,
    LFG_LOCKSTATUS_WRONG_TIME_RANGE          = 1029,    ///< Not used in lockedReason
    LFG_LOCKSTATUS_WRONG_TIME                = 1030,    ///< Not used in lockedReason
    LFG_LOCKSTATUS_NOT_IN_SEASON             = 1031,    ///< Not used in lockedReason
    LFG_LOCKSTATUS_MISSING_ACHIEVEMENT       = 1034,
    LFG_LOCKSTATUS_TEMPORARILY_DISABLED      = 10000
};

/// Dungeon and reason why player can't join
struct LfgLockStatus
{
    LfgLockStatusType lockstatus;                          ///< Lock type
    uint32 SubReason1;
    uint32 SubReason2;

    LfgLockStatus()
    {
        SubReason1 = 0;
        SubReason2 = 0;
        lockstatus = LFG_LOCKSTATUS_OK;
    }
};


typedef std::set<uint32> LfgDungeonSet;
typedef ACE_Based::LockedMap<uint32, LfgLockStatus> LfgLockMap;
typedef ACE_Based::LockedMap<uint64, LfgLockMap> LfgLockPartyMap;

#endif
