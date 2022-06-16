////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CREATURE_H
#define CREATURE_H

#include "Common.h"
#include "Unit.h"
#include "UpdateMask.h"
#include "ItemPrototype.h"
#include "LootMgr.h"
#include "DatabaseEnv.h"
#include "Cell.h"

class SpellInfo;
class GarrisonNPCAI;
class CreatureAI;
class Quest;
class Player;
class WorldSession;
class CreatureGroup;
class CreatureScript;
struct CellArea;

enum CreatureDifficultyFlags
{
    CREATURE_DIFFICULTYFLAGS_UNK1                                  = 0x00000001, // Related to mounts
    CREATURE_DIFFICULTYFLAGS_NO_EXPERIENCE                         = 0x00000002,
    CREATURE_DIFFICULTYFLAGS_NO_LOOT                               = 0x00000004,
    CREATURE_DIFFICULTYFLAGS_UNKILLABLE                            = 0x00000008,
    CREATURE_DIFFICULTYFLAGS_TAMEABLE                              = 0x00000010, // CREATURE_TYPEFLAGS_TAMEABLE
    CREATURE_DIFFICULTYFLAGS_IMMUNE_TO_PC                          = 0x00000020, // UNIT_FLAG_IMMUNE_TO_PC
    CREATURE_DIFFICULTYFLAGS_IMMUNE_TO_NPC                         = 0x00000040, // UNIT_FLAG_IMMUNE_TO_NPC
    CREATURE_DIFFICULTYFLAGS_UNK2                                  = 0x00000080,
    CREATURE_DIFFICULTYFLAGS_SESSILE                               = 0x00000100, // Creature is rooted
    CREATURE_DIFFICULTYFLAGS_NOT_SELECTABLE                        = 0x00000200, // UNIT_FLAG_NOT_SELECTABLE
    CREATURE_DIFFICULTYFLAGS_UNK3                                  = 0x00000400, // Related to health - it seems similar to CREATURE_DIFFICULTYFLAGS_2_KEEP_HEALTH_POINTS_AT_RESET
    CREATURE_DIFFICULTYFLAGS_NO_CORPSE_UPON_DEATH                  = 0x00000800, // Creature instantly disappear when killed
    CREATURE_DIFFICULTYFLAGS_UNK5                                  = 0x00001000,
    CREATURE_DIFFICULTYFLAGS_UNK6                                  = 0x00002000,
    CREATURE_DIFFICULTYFLAGS_UNK7                                  = 0x00004000,
    CREATURE_DIFFICULTYFLAGS_UNK8                                  = 0x00008000,
    CREATURE_DIFFICULTYFLAGS_BOSS                                  = 0x00010000, // CREATURE_TYPEFLAGS_BOSS
    CREATURE_DIFFICULTYFLAGS_UNK9                                  = 0x00020000,
    CREATURE_DIFFICULTYFLAGS_WATER_BOUND                           = 0x00040000,
    CREATURE_DIFFICULTYFLAGS_CAN_PENETRATE_WATER                   = 0x00080000,
    CREATURE_DIFFICULTYFLAGS_UNK10                                 = 0x00100000,
    CREATURE_DIFFICULTYFLAGS_GHOST                                 = 0x00200000, // CREATURE_TYPEFLAGS_GHOST
    CREATURE_DIFFICULTYFLAGS_UNK11                                 = 0x00400000,
    CREATURE_DIFFICULTYFLAGS_DO_NOT_PLAY_WOUND_PARRY_ANIMATION     = 0x00800000, // CREATURE_TYPEFLAGS_DO_NOT_PLAY_WOUND_PARRY_ANIMATION
    CREATURE_DIFFICULTYFLAGS_HIDE_FACTION_TOOLTIP                  = 0x01000000, // CREATURE_TYPEFLAGS_HIDE_FACTION_TOOLTIP
    CREATURE_DIFFICULTYFLAGS_IGNORE_COMBAT                         = 0x02000000,
    CREATURE_DIFFICULTYFLAGS_UNK12                                 = 0x04000000,
    CREATURE_DIFFICULTYFLAGS_SUMMON_GUARD_IF_IN_AGGRO_RANGE        = 0x08000000, // Creature will summon a guard if player is within its aggro range (even if creature doesn't attack per se)
    CREATURE_DIFFICULTYFLAGS_ONLY_SWIM                             = 0x10000000, // UNIT_FLAG_UNK_15
    CREATURE_DIFFICULTYFLAGS_UNK13                                 = 0x20000000, // Related to gravity
    CREATURE_DIFFICULTYFLAGS_TFLAG_UNK5                            = 0x40000000, // CREATURE_TYPEFLAGS_UNK5
    CREATURE_DIFFICULTYFLAGS_LARGE_AOI                             = 0x80000000  // UnitFlags2 0x200000
};

enum CreatureDifficultyFlags2
{
    CREATURE_DIFFICULTYFLAGS_2_UNK1                                = 0x00000001,
    CREATURE_DIFFICULTYFLAGS_2_FORCE_PARTY_MEMBERS_INTO_COMBAT     = 0x00000002,
    CREATURE_DIFFICULTYFLAGS_2_UNK2                                = 0x00000004,
    CREATURE_DIFFICULTYFLAGS_2_SPELL_ATTACKABLE                    = 0x00000008, // CREATURE_TYPEFLAGS_SPELL_ATTACKABLE
    CREATURE_DIFFICULTYFLAGS_2_UNK3                                = 0x00000010,
    CREATURE_DIFFICULTYFLAGS_2_UNK4                                = 0x00000020,
    CREATURE_DIFFICULTYFLAGS_2_UNK5                                = 0x00000040,
    CREATURE_DIFFICULTYFLAGS_2_UNK6                                = 0x00000080,
    CREATURE_DIFFICULTYFLAGS_2_UNK7                                = 0x00000100,
    CREATURE_DIFFICULTYFLAGS_2_UNK8                                = 0x00000200,
    CREATURE_DIFFICULTYFLAGS_2_UNK9                                = 0x00000400,
    CREATURE_DIFFICULTYFLAGS_2_DEAD_INTERACT                       = 0x00000800, // CREATURE_TYPEFLAGS_DEAD_INTERACT
    CREATURE_DIFFICULTYFLAGS_2_UNK10                               = 0x00001000,
    CREATURE_DIFFICULTYFLAGS_2_UNK11                               = 0x00002000,
    CREATURE_DIFFICULTYFLAGS_2_HERBLOOT                            = 0x00004000, // CREATURE_TYPEFLAGS_HERBLOOT
    CREATURE_DIFFICULTYFLAGS_2_MININGLOOT                          = 0x00008000, // CREATURE_TYPEFLAGS_MININGLOOT
    CREATURE_DIFFICULTYFLAGS_2_DONT_LOG_DEATH                      = 0x00010000, // CREATURE_TYPEFLAGS_DONT_LOG_DEATH
    CREATURE_DIFFICULTYFLAGS_2_UNK12                               = 0x00020000,
    CREATURE_DIFFICULTYFLAGS_2_MOUNTED_COMBAT                      = 0x00040000, // CREATURE_TYPEFLAGS_MOUNTED_COMBAT
    CREATURE_DIFFICULTYFLAGS_2_UNK13                               = 0x00080000,
    CREATURE_DIFFICULTYFLAGS_2_UNK14                               = 0x00100000, // This flag seems similar to CREATURE_DIFFICULTYFLAGS_IGNORE_COMBAT
    CREATURE_DIFFICULTYFLAGS_2_UNK15                               = 0x00200000,
    CREATURE_DIFFICULTYFLAGS_2_UNK16                               = 0x00400000,
    CREATURE_DIFFICULTYFLAGS_2_UNK17                               = 0x00800000,
    CREATURE_DIFFICULTYFLAGS_2_UNK18                               = 0x01000000,
    CREATURE_DIFFICULTYFLAGS_2_HIDE_BODY                           = 0x02000000, // UNIT_FLAG2_UNK1
    CREATURE_DIFFICULTYFLAGS_2_UNK19                               = 0x04000000,
    CREATURE_DIFFICULTYFLAGS_2_SERVER_ONLY                         = 0x08000000,
    CREATURE_DIFFICULTYFLAGS_2_CAN_SAFE_FALL                       = 0x10000000,
    CREATURE_DIFFICULTYFLAGS_2_CAN_ASSIST                          = 0x20000000, // CREATURE_TYPEFLAGS_CAN_ASSIST
    CREATURE_DIFFICULTYFLAGS_2_KEEP_HEALTH_POINTS_AT_RESET         = 0x40000000,
    CREATURE_DIFFICULTYFLAGS_2_IS_PET_BAR_USED                     = 0x80000000  // CREATURE_TYPEFLAGS_IS_PET_BAR_USED
};

enum CreatureDifficultyFlags3
{
    CREATURE_DIFFICULTYFLAGS_3_UNK1                                = 0x00000001,
    CREATURE_DIFFICULTYFLAGS_3_UNK2                                = 0x00000002,
    CREATURE_DIFFICULTYFLAGS_3_INSTANTLY_APPEAR_MODEL              = 0x00000004, // UNIT_FLAG2_INSTANTLY_APPEAR_MODEL
    CREATURE_DIFFICULTYFLAGS_3_MASK_UID                            = 0x00000008, // CREATURE_TYPEFLAG_MASK_UID
    CREATURE_DIFFICULTYFLAGS_3_ENGINEERLOOT                        = 0x00000010, // CREATURE_TYPEFLAGS_ENGINEERLOOT
    CREATURE_DIFFICULTYFLAGS_3_UNK3                                = 0x00000020,
    CREATURE_DIFFICULTYFLAGS_3_UNK4                                = 0x00000040,
    CREATURE_DIFFICULTYFLAGS_3_UNK5                                = 0x00000080,
    CREATURE_DIFFICULTYFLAGS_3_CANNOT_SWIM                         = 0x00000100, // UNIT_FLAG_UNK_14
    CREATURE_DIFFICULTYFLAGS_3_EXOTIC                              = 0x00000200, // CREATURE_TYPEFLAGS_EXOTIC
    CREATURE_DIFFICULTYFLAGS_3_GIGANTIC_AOI                        = 0x00000400, // Since MoP, creatures with that flag have UnitFlags2 0x400000
    CREATURE_DIFFICULTYFLAGS_3_INFINITE_AOI                        = 0x00000800, // Since MoP, creatures with that flag have UnitFlags2 0x40000000
    CREATURE_DIFFICULTYFLAGS_3_WATERWALKING                        = 0x00001000,
    CREATURE_DIFFICULTYFLAGS_3_HIDE_NAMEPLATE                      = 0x00002000, // CREATURE_TYPEFLAGS_HIDE_NAMEPLATE
    CREATURE_DIFFICULTYFLAGS_3_UNK6                                = 0x00004000,
    CREATURE_DIFFICULTYFLAGS_3_UNK7                                = 0x00008000,
    CREATURE_DIFFICULTYFLAGS_3_USE_DEFAULT_COLLISION_BOX           = 0x00010000, // CREATURE_TYPEFLAGS_USE_DEFAULT_COLLISION_BOX
    CREATURE_DIFFICULTYFLAGS_3_UNK8                                = 0x00020000,
    CREATURE_DIFFICULTYFLAGS_3_IS_SIEGE_WEAPON                     = 0x00040000, // CREATURE_TYPEFLAGS_IS_SIEGE_WEAPON
    CREATURE_DIFFICULTYFLAGS_3_UNK9                                = 0x00080000,
    CREATURE_DIFFICULTYFLAGS_3_UNK10                               = 0x00100000,
    CREATURE_DIFFICULTYFLAGS_3_UNK11                               = 0x00200000,
    CREATURE_DIFFICULTYFLAGS_3_PROJECTILE_COLLISION                = 0x00400000, // CREATURE_TYPEFLAGS_PROJECTILE_COLLISION
    CREATURE_DIFFICULTYFLAGS_3_CAN_BE_MULTITAPPED                  = 0x00800000,
    CREATURE_DIFFICULTYFLAGS_3_DO_NOT_PLAY_MOUNTED_ANIMATIONS      = 0x01000000, // CREATURE_TYPEFLAGS_DO_NOT_PLAY_MOUNTED_ANIMATIONS
    CREATURE_DIFFICULTYFLAGS_3_DISABLE_TURN                        = 0x02000000, // UNIT_FLAG2_DISABLE_TURN
    CREATURE_DIFFICULTYFLAGS_3_UNK12                               = 0x04000000,
    CREATURE_DIFFICULTYFLAGS_3_UNK13                               = 0x08000000,
    CREATURE_DIFFICULTYFLAGS_3_UNK14                               = 0x10000000,
    CREATURE_DIFFICULTYFLAGS_3_IS_LINK_ALL                         = 0x20000000, // CREATURE_TYPEFLAGS_IS_LINK_ALL
    CREATURE_DIFFICULTYFLAGS_3_UNK15                               = 0x40000000,
    CREATURE_DIFFICULTYFLAGS_3_UNK16                               = 0x80000000
};

enum CreatureDifficultyFlags4
{
    CREATURE_DIFFICULTYFLAGS_4_HAS_NO_BIRTH_ANIMATION              = 0x00000001, // SMSG_UPDATE_OBJECT's "NoBirthAnim"
    CREATURE_DIFFICULTYFLAGS_4_UNK1                                = 0x00000002,
    CREATURE_DIFFICULTYFLAGS_4_UNK2                                = 0x00000004,
    CREATURE_DIFFICULTYFLAGS_4_INTERACT_ONLY_WITH_CREATOR          = 0x00000008, // CREATURE_TYPEFLAGS_INTERACT_ONLY_WITH_CREATOR
    CREATURE_DIFFICULTYFLAGS_4_DO_NOT_PLAY_UNIT_EVENT_SOUNDS       = 0x00000010, // CREATURE_TYPEFLAGS_DO_NOT_PLAY_UNIT_EVENT_SOUNDS
    CREATURE_DIFFICULTYFLAGS_4_HAS_NO_SHADOW_BLOB                  = 0x00000020, // CREATURE_TYPEFLAGS_HAS_NO_SHADOW_BLOB
    CREATURE_DIFFICULTYFLAGS_4_UNK3                                = 0x00000040,
    CREATURE_DIFFICULTYFLAGS_4_UNK4                                = 0x00000080,
    CREATURE_DIFFICULTYFLAGS_4_UNK5                                = 0x00000100,
    CREATURE_DIFFICULTYFLAGS_4_UNK6                                = 0x00000200,
    CREATURE_DIFFICULTYFLAGS_4_UNK7                                = 0x00000400,
    CREATURE_DIFFICULTYFLAGS_4_UNK8                                = 0x00000800,
    CREATURE_DIFFICULTYFLAGS_4_UNK9                                = 0x00001000,
    CREATURE_DIFFICULTYFLAGS_4_UNK10                               = 0x00002000,
    CREATURE_DIFFICULTYFLAGS_4_UNK11                               = 0x00004000,
    CREATURE_DIFFICULTYFLAGS_4_UFLAG2_UNK20                        = 0x00008000, // UnitFlags2 0x100000
    CREATURE_DIFFICULTYFLAGS_4_UNK12                               = 0x00010000,
    CREATURE_DIFFICULTYFLAGS_4_UNK13                               = 0x00020000,
    CREATURE_DIFFICULTYFLAGS_4_UNK14                               = 0x00040000,
    CREATURE_DIFFICULTYFLAGS_4_FORCE_GOSSIP                        = 0x00080000, // CREATURE_TYPEFLAGS_FORCE_GOSSIP
    CREATURE_DIFFICULTYFLAGS_4_UNK15                               = 0x00100000,
    CREATURE_DIFFICULTYFLAGS_4_DO_NOT_SHEATHE                      = 0x00200000, // CREATURE_TYPEFLAGS_DO_NOT_SHEATHE
    CREATURE_DIFFICULTYFLAGS_4_IGNORE_SPELL_MIN_RANGE_RESTRICTIONS = 0x00400000, // UnitFlags2 0x8000000
    CREATURE_DIFFICULTYFLAGS_4_UNK16                               = 0x00800000,
    CREATURE_DIFFICULTYFLAGS_4_PREVENT_SWIM                        = 0x01000000, // UnitFlags2 0x1000000
    CREATURE_DIFFICULTYFLAGS_4_HIDE_IN_COMBAT_LOG                  = 0x02000000, // UnitFlags2 0x2000000
    CREATURE_DIFFICULTYFLAGS_4_UNK17                               = 0x04000000,
    CREATURE_DIFFICULTYFLAGS_4_UNK18                               = 0x08000000,
    CREATURE_DIFFICULTYFLAGS_4_UNK19                               = 0x10000000,
    CREATURE_DIFFICULTYFLAGS_4_DO_NOT_TARGET_ON_INTERACTION        = 0x20000000, // CREATURE_TYPEFLAGS_DO_NOT_TARGET_ON_INTERACTION
    CREATURE_DIFFICULTYFLAGS_4_DO_NOT_RENDER_OBJECT_NAME           = 0x40000000, // CREATURE_TYPEFLAGS_DO_NOT_RENDER_OBJECT_NAME
    CREATURE_DIFFICULTYFLAGS_4_UNIT_IS_QUEST_BOSS                  = 0x80000000  // CREATURE_TYPEFLAGS_UNIT_IS_QUEST_BOSS
};

enum CreatureDifficultyFlags5
{
    CREATURE_DIFFICULTYFLAGS_5_CANNOT_SWITCH_TARGETS               = 0x00000001, // UnitFlags2 0x4000000
    CREATURE_DIFFICULTYFLAGS_5_UNK1                                = 0x00000002,
    CREATURE_DIFFICULTYFLAGS_5_UFLAG2_UNK30                        = 0x00000004, // UnitFlags2 0x10000000
    CREATURE_DIFFICULTYFLAGS_5_UNK2                                = 0x00000008,
    CREATURE_DIFFICULTYFLAGS_5_UNK3                                = 0x00000010,
    CREATURE_DIFFICULTYFLAGS_5_UNK4                                = 0x00000020,
    CREATURE_DIFFICULTYFLAGS_5_UNK5                                = 0x00000040,
    CREATURE_DIFFICULTYFLAGS_5_UNK6                                = 0x00000080,
    CREATURE_DIFFICULTYFLAGS_5_CAN_INTERACT_EVEN_IF_HOSTILE        = 0x00000100, // UNIT_FLAG2_ALLOW_ENEMY_INTERACT
    CREATURE_DIFFICULTYFLAGS_5_UNK7                                = 0x00000200,
    CREATURE_DIFFICULTYFLAGS_5_TFLAG2_UNK1                         = 0x00000400, // CREATURE_TYPEFLAGS_2_UNK1
    CREATURE_DIFFICULTYFLAGS_5_TFLAG2_UNK2                         = 0x00000800, // CREATURE_TYPEFLAGS_2_UNK2
    CREATURE_DIFFICULTYFLAGS_5_TFLAG2_UNK3                         = 0x00001000, // CREATURE_TYPEFLAGS_2_UNK3
    CREATURE_DIFFICULTYFLAGS_5_UFLAG2_UNK19                        = 0x00002000, // UnitFlags2 0x80000
    CREATURE_DIFFICULTYFLAGS_5_UNK8                                = 0x00004000,
    CREATURE_DIFFICULTYFLAGS_5_UNK9                                = 0x00008000,
    CREATURE_DIFFICULTYFLAGS_5_UNK10                               = 0x00010000,
    CREATURE_DIFFICULTYFLAGS_5_UNK11                               = 0x00020000,
    CREATURE_DIFFICULTYFLAGS_5_UNK12                               = 0x00040000,
    CREATURE_DIFFICULTYFLAGS_5_UNK13                               = 0x00080000,
    CREATURE_DIFFICULTYFLAGS_5_TFLAG2_UNK4                         = 0x00100000, // CREATURE_TYPEFLAGS_2_UNK4
    CREATURE_DIFFICULTYFLAGS_5_UNK14                               = 0x00200000,
    CREATURE_DIFFICULTYFLAGS_5_UNK15                               = 0x00400000,
    CREATURE_DIFFICULTYFLAGS_5_TFLAG2_UNK5                         = 0x00800000, // CREATURE_TYPEFLAGS_2_UNK5
    CREATURE_DIFFICULTYFLAGS_5_UNK16                               = 0x01000000,
    CREATURE_DIFFICULTYFLAGS_5_UNK17                               = 0x02000000,
    CREATURE_DIFFICULTYFLAGS_5_UNK18                               = 0x04000000,
    CREATURE_DIFFICULTYFLAGS_5_UNK19                               = 0x08000000,
    CREATURE_DIFFICULTYFLAGS_5_UNK20                               = 0x10000000,
    CREATURE_DIFFICULTYFLAGS_5_UNK21                               = 0x20000000,
    CREATURE_DIFFICULTYFLAGS_5_TFLAG2_UNK6                         = 0x40000000, // CREATURE_TYPEFLAGS_2_UNK6
    CREATURE_DIFFICULTYFLAGS_5_UNK22                               = 0x80000000
};

enum CreatureDifficultyFlags6
{
    CREATURE_DIFFICULTYFLAGS_6_UNK1                                = 0x00000001,
    CREATURE_DIFFICULTYFLAGS_6_UNK2                                = 0x00000002,
    CREATURE_DIFFICULTYFLAGS_6_UNK3                                = 0x00000004,
    CREATURE_DIFFICULTYFLAGS_6_UNK4                                = 0x00000008,
    CREATURE_DIFFICULTYFLAGS_6_UNK5                                = 0x00000010,
    CREATURE_DIFFICULTYFLAGS_6_UNK6                                = 0x00000020,
    CREATURE_DIFFICULTYFLAGS_6_UNK7                                = 0x00000040,
    CREATURE_DIFFICULTYFLAGS_6_TFLAG2_UNK7                         = 0x00000080,
    CREATURE_DIFFICULTYFLAGS_6_UNK8                                = 0x00000100,
    CREATURE_DIFFICULTYFLAGS_6_UNK9                                = 0x00000200,
    CREATURE_DIFFICULTYFLAGS_6_UNK10                               = 0x00000400,
    CREATURE_DIFFICULTYFLAGS_6_UNK11                               = 0x00000800,
    CREATURE_DIFFICULTYFLAGS_6_UNK12                               = 0x00001000,
    CREATURE_DIFFICULTYFLAGS_6_UNK13                               = 0x00002000,
    CREATURE_DIFFICULTYFLAGS_6_UNK14                               = 0x00004000,
    CREATURE_DIFFICULTYFLAGS_6_UNK15                               = 0x00008000,
    CREATURE_DIFFICULTYFLAGS_6_UNK16                               = 0x00010000,
    CREATURE_DIFFICULTYFLAGS_6_UNK17                               = 0x00020000,
    CREATURE_DIFFICULTYFLAGS_6_UNK18                               = 0x00040000,
    CREATURE_DIFFICULTYFLAGS_6_UNK19                               = 0x00080000,
    CREATURE_DIFFICULTYFLAGS_6_UNK20                               = 0x00100000,
    CREATURE_DIFFICULTYFLAGS_6_UNK21                               = 0x00200000,
    CREATURE_DIFFICULTYFLAGS_6_UNK22                               = 0x00400000,
    CREATURE_DIFFICULTYFLAGS_6_UNK23                               = 0x00800000,
    CREATURE_DIFFICULTYFLAGS_6_UNK24                               = 0x01000000,
    CREATURE_DIFFICULTYFLAGS_6_UNK25                               = 0x02000000,
    CREATURE_DIFFICULTYFLAGS_6_UNK26                               = 0x04000000,
    CREATURE_DIFFICULTYFLAGS_6_UNK27                               = 0x08000000,
    CREATURE_DIFFICULTYFLAGS_6_UNK28                               = 0x10000000,
    CREATURE_DIFFICULTYFLAGS_6_UNK29                               = 0x20000000,
    CREATURE_DIFFICULTYFLAGS_6_UNK30                               = 0x40000000,
    CREATURE_DIFFICULTYFLAGS_6_TFLAG2_UNK14                        = 0x80000000
};

enum CreatureDifficultyFlags7
{
    CREATURE_DIFFICULTYFLAGS_7_TFLAG2_UNK15                        = 0x00000001,
    CREATURE_DIFFICULTYFLAGS_7_TFLAG2_UNK16                        = 0x00000002,
    CREATURE_DIFFICULTYFLAGS_7_TFLAG2_UNK17                        = 0x00000004,
    CREATURE_DIFFICULTYFLAGS_7_UNK1                                = 0x00000008
};

enum CreatureFlagsExtra
{
    CREATURE_FLAG_EXTRA_INSTANCE_BIND       = 0x00000001,       ///< creature kill bind instance with killer and killer's group
    CREATURE_FLAG_EXTRA_CIVILIAN            = 0x00000002,       ///< not aggro (ignore faction/reputation hostility)
    CREATURE_FLAG_EXTRA_NO_PARRY            = 0x00000004,       ///< creature can't parry
    CREATURE_FLAG_EXTRA_NO_PARRY_HASTEN     = 0x00000008,       ///< creature can't counter-attack at parry
    CREATURE_FLAG_EXTRA_NO_BLOCK            = 0x00000010,       ///< creature can't block
    CREATURE_FLAG_EXTRA_NO_CRUSH            = 0x00000020,       ///< creature can't do crush attacks
    CREATURE_FLAG_EXTRA_NO_XP_AT_KILL       = 0x00000040,       ///< creature kill not provide XP
    CREATURE_FLAG_EXTRA_TRIGGER             = 0x00000080,       ///< trigger creature
    CREATURE_FLAG_EXTRA_NO_TAUNT            = 0x00000100,       ///< creature is immune to taunt auras and effect attack me
    CREATURE_FLAG_EXTRA_PERSONAL_LOOT       = 0x00000200,       ///< Personal loot mobs and increment healths by player
    CREATURE_FLAG_EXTRA_WORLDEVENT          = 0x00004000,       ///< custom flag for world event creatures (left room for merging)
    CREATURE_FLAG_EXTRA_GUARD               = 0x00008000,       ///< Creature is guard
    CREATURE_FLAG_EXTRA_NO_CRIT             = 0x00020000,       ///< creature can't do critical strikes
    CREATURE_FLAG_EXTRA_NO_SKILLGAIN        = 0x00040000,       ///< creature won't increase weapon skills
    CREATURE_FLAG_EXTRA_TAUNT_DIMINISH      = 0x00080000,       ///< Taunt is a subject to diminishing returns on this creautre·
    CREATURE_FLAG_EXTRA_ALL_DIMINISH        = 0x00100000,       ///< Creature is subject to all diminishing returns as player are
    CREATURE_FLAG_EXTRA_LOG_GROUP_DMG       = 0x00200000,       ///< All damage done to the create will be logged into database, help to spot cheaters/exploit/usebug
    CREATURE_FLAG_EXTRA_IMMUNITY_KNOCKBACK  = 0x00400000,       ///< Creature is immune to knockback effects
    CREATURE_FLAG_EXTRA_DUNGEON_BOSS        = 0x10000000,       ///< creature is a dungeon boss
    CREATURE_FLAG_EXTRA_IGNORE_PATHFINDING  = 0x20000000,       ///< creature ignore pathfinding (NYI)
    CREATURE_FLAG_EXTRA_DUNGEON_END_BOSS    = 0x40000000        ///< Creature is the last boss of the dungeon where he is
};

#define CREATURE_FLAG_EXTRA_DB_ALLOWED (CREATURE_FLAG_EXTRA_INSTANCE_BIND | CREATURE_FLAG_EXTRA_CIVILIAN | \
    CREATURE_FLAG_EXTRA_NO_PARRY | CREATURE_FLAG_EXTRA_NO_PARRY_HASTEN | CREATURE_FLAG_EXTRA_NO_BLOCK | \
    CREATURE_FLAG_EXTRA_NO_CRUSH | CREATURE_FLAG_EXTRA_NO_XP_AT_KILL | CREATURE_FLAG_EXTRA_TRIGGER | \
    CREATURE_FLAG_EXTRA_NO_TAUNT | CREATURE_FLAG_EXTRA_WORLDEVENT | CREATURE_FLAG_EXTRA_NO_CRIT | CREATURE_FLAG_EXTRA_IGNORE_PATHFINDING | \
    CREATURE_FLAG_EXTRA_NO_SKILLGAIN | CREATURE_FLAG_EXTRA_TAUNT_DIMINISH | CREATURE_FLAG_EXTRA_ALL_DIMINISH | \
    CREATURE_FLAG_EXTRA_GUARD | CREATURE_FLAG_EXTRA_DUNGEON_END_BOSS | CREATURE_FLAG_EXTRA_DUNGEON_BOSS | CREATURE_FLAG_EXTRA_LOG_GROUP_DMG | \
    CREATURE_FLAG_EXTRA_PERSONAL_LOOT | CREATURE_FLAG_EXTRA_IMMUNITY_KNOCKBACK)

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push, N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

// Creature Pet entries
enum WarlockPet
{
    Infernal            = 89,
    Imp                 = 416,
    VoidWalker          = 1860,
    Succubus            = 1863,
    FelHunter           = 417,
    FelGuard            = 17252,
    FellImp             = 58959,
    VoidLord            = 58960,
    Shivarra            = 58963,
    Observer            = 58964,
    WrathGuard          = 58965,
    DoomGuard           = 11859,
    Abyssal             = 58997,
    TerrorGuard         = 59000,
    TerrorGuardPet      = 78215,
    InfernalPet         = 78217,
    DoomGuardPet        = 78158,
    AbyssalPet          = 78216,
    AbyssalPEtGreenFire = 78218
};

// Mage
enum MagePet
{
    FrozenOrb      = 45322
};

// Druid
enum DruidPet
{
    TreantGuardian    = 54985,
    TreantFeral       = 54984,
    TreantRestoration = 54983,
    TreantBalance     = 1964
};

enum ShamanPet
{
    FireElemental = 15438
};

enum DeathKnightPet
{
    Ghoul               = 26125,
    Skulker             = 99541,
    BloodWorm           = 28017,
    Gargoyle            = 27829,
    DarkArbiter         = 100876,
    Abomination         = 106848,
    DancingRuneWeapon   = 27893,
    ShamblingHorror     = 97055,
    ArmyOfTheDead       = 24207
};

enum eMonkPets
{
    Fire    = 69791,
    Earth   = 69792,
    Niuzao  = 73967
};

/// - Pet & creatures regen life every 5 secs (WoD 6.x)
#define CREATURE_REGEN_HEALTH_INTERVAL 5 * IN_MILLISECONDS

#define CREATURE_REGEN_INTERVAL 2 * IN_MILLISECONDS

const uint8 MAX_KILL_CREDIT = 2;
const uint32 MAX_CREATURE_MODELS = 4;
const uint32 MAX_CREATURE_NAMES = 4;
const uint32 MAX_CREATURE_SPELLS = 8;
const uint32 MAX_CREATURE_QUEST_ITEMS = 10;


// from `creature_template` table
struct CreatureTemplate
{
    uint32  Entry;
    uint32  DifficultyEntry[Difficulty::MaxDifficulties];
    uint32  KillCredit[MAX_KILL_CREDIT];
    uint32  Modelid[MAX_CREATURE_MODELS];
    std::string Name[MAX_CREATURE_NAMES];
    std::string NameAlt[MAX_CREATURE_NAMES];
    std::string Title;
    std::string TitleAlt;
    std::string CursorName;
    uint32  GossipMenuId;
    uint8   minlevel;
    uint8   maxlevel;
    int32  expansion;
    uint32  RequiredExpansion;
    uint32  faction;
    uint32  NpcFlags1;
    uint32  NpcFlags2;
    uint32  QuestLootID = 0;
    float   speed_walk;
    float   speed_run;
    float   speed_fly;
    float   scale;
    uint32  rank;
    uint32  dmgschool;
    float   dmg_multiplier;
    uint32  baseattacktime;
    uint32  rangeattacktime;
    float   baseVariance;
    float   rangeVariance;
    uint32  unit_class;                                     // enum Classes. Note only 4 classes are known for creatures.
    uint32  UnitFlags1;                                     // enum UnitFlags mask values
    uint32  UnitFlags2;                                     // enum UnitFlags2 mask values
    uint32  UnitFlags3;                                     // enum UnitFlags3 mask values
    uint32  dynamicflags;
    uint32  WorldEffectID;
    CreatureFamily  family;                                 // enum CreatureFamily values (optional)
    uint32  trainer_type;
    uint32  trainer_spell;
    uint32  trainer_class;
    uint32  trainer_race;
    uint32  type;                                           // enum CreatureType values
    uint32  type_flags;                                     // enum CreatureTypeFlags mask values
    uint32  type_flags2;                                    // unknown enum, only set for 4 creatures (with value 1)
    uint32  lootid;
    uint32  pickpocketLootId;
    uint32  SkinLootId;
    int32   resistance[MAX_SPELL_SCHOOL];
    uint32  spells[MAX_CREATURE_SPELLS];
    uint32  VehicleId;
    uint32  mingold;
    uint32  maxgold;
    std::string AIName;
    uint32  MovementType;
    uint32  InhabitType;
    float   HoverHeight;
    float   ModHealth;
    float   ModMana;
    float   ModManaExtra;                                   // Added in 4.x, this value is usually 2 for a small group of creatures with double mana
    float   ModArmor;
    bool    RacialLeader;
    uint32  questItems[MAX_CREATURE_QUEST_ITEMS];
    uint32  movementId;
    uint32  VignetteID;
    uint32  TrackingQuestID;
    bool    RegenHealth;
    bool    QuestPersonalLoot = false;
    uint32  MechanicImmuneMask;
    uint32  flags_extra;
    uint32  ScriptID;
    uint8 ScaleLevelMin = 0;
    uint8 ScaleLevelMax = 0;
    int8  ScaleLevelDelta = 0;

    uint32  GetRandomValidModelId() const;
    uint32  GetFirstValidModelId() const;

    // helpers
    SkillType GetRequiredLootSkill() const
    {
        if (type_flags & CREATURE_TYPE_FLAG_HERB_SKINNING_SKILL)
            return SKILL_HERBALISM;
        else if (type_flags & CREATURE_TYPE_FLAG_MINING_SKINNING_SKILL)
            return SKILL_MINING;
        else if (type_flags & CREATURE_TYPE_FLAG_ENGINEERING_SKINNING_SKILL)
            return SKILL_ENGINEERING;
        else
            return SKILL_SKINNING;                          // normal case
    }

    bool IsExotic() const
    {
        return (type_flags & CREATURE_TYPE_FLAG_EXOTIC_PET) != 0;
    }

    bool isTameable(bool canTameExotic) const
    {
        if ((type != CREATURE_TYPE_BEAST && type != CREATURE_TYPE_MECHANICAL) || family == CREATURE_FAMILY_NONE || (type_flags & CREATURE_TYPE_FLAG_TAMEABLE_PET) == 0)
            return false;

        // if can tame exotic then can tame any temable
        return canTameExotic || !IsExotic();
    }
};

struct CreatureLevelStat
{
    uint32   baseHP;
    uint32   baseMP;
    uint32   healthMax;
    float    baseMinDamage;
    float    baseMaxDamage;
    uint32   AttackPower;
    uint32   RangedAttackPower;
    uint32   BaseArmor;
};

using CreatureLevelStatContainer = std::unordered_map<uint8, CreatureLevelStat>;

// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint32, CreatureTemplate> CreatureTemplateContainer;

// Defines base stats for creatures (used to calculate HP/mana/armor/attackpower/rangedattackpower/base_damage)
struct CreatureBaseStats
{
    uint32 BaseHealth[MAX_EXPANSION];
    uint32 BaseMana;
    uint32 BaseArmor;
    uint32 AttackPower;
    uint32 RangedAttackPower;
    float BaseDamage[MAX_EXPANSION];

    // Helpers

    uint32 GenerateHealth(CreatureTemplate const* info) const
    {
        return uint32((BaseHealth[info->expansion] * info->ModHealth) + 0.5f);
    }

    uint32 GenerateMana(CreatureTemplate const* info) const
    {
        // Mana can be 0.
        if (!BaseMana)
            return 0;

        return uint32((BaseMana * info->ModMana * info->ModManaExtra) + 0.5f);
    }

    uint32 GenerateArmor(CreatureTemplate const* info) const
    {
        return uint32((BaseArmor * info->ModArmor) + 0.5f);
    }

    float GenerateBaseDamage(CreatureTemplate const* info) const
    {
        return BaseDamage[info->expansion];
    }

    static CreatureBaseStats const* GetBaseStats(uint8 level, uint8 unitClass);
};

typedef std::unordered_map<uint16, CreatureBaseStats> CreatureBaseStatsContainer;

#define MAX_GROUP_SCALING 31

struct CreatureGroupSizeStat
{
    std::vector<uint32> Healths;
    std::vector<float>  MinDamage;
    std::vector<float>  MaxDamage;

    CreatureGroupSizeStat()
    {
        Healths.resize(MAX_GROUP_SCALING);
        MinDamage.resize(MAX_GROUP_SCALING);
        MaxDamage.resize(MAX_GROUP_SCALING);
    }

    uint32 GetHealthFor(uint32 p_GroupSize) const
    {
        p_GroupSize = std::min(std::max((uint32)10, p_GroupSize), (uint32)MAX_GROUP_SCALING - 1);
        return Healths[p_GroupSize];
    }
};

typedef std::map<uint32/*CreatureEntry*/, std::map<uint32/*Difficulty*/, CreatureGroupSizeStat>> CreatureGroupSizeStatsContainer;
typedef std::map<uint32/*CreatureEntry*/, std::map<uint32/*DifficultyID*/, uint64>> CreatureDiffHealthContainer;

struct CreatureDamageLog
{
    uint32 Spell;
    uint32 Damage;
    uint32 AttackerGuid;
    uint32 Time;
};

typedef std::list<CreatureDamageLog> CreatureDamageLogList;

struct GroupDump
{
    std::string Dump;
    time_t      Time;
};

typedef std::list<GroupDump> GroupDumpList;

#define GROUP_DUMP_TIMER 60000

struct CreatureLocale
{
    StringVector Name[MAX_CREATURE_NAMES];
    StringVector NameAlt[MAX_CREATURE_NAMES];
    StringVector Title;
    StringVector TitleAlt;
};

struct DisplayChoiceLocale
{
    StringVector Description;
    StringVector Question;
    StringVector Answer;
};

struct GossipMenuItemsLocale
{
    StringVector OptionText;
    StringVector BoxText;
};

struct PointOfInterestLocale
{
    StringVector IconName;
};

struct EquipmentInfo
{
    uint32  ItemEntry[MAX_EQUIPMENT_ITEMS];
};

// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint8, EquipmentInfo> EquipmentInfoContainerInternal;
typedef std::unordered_map<uint32, EquipmentInfoContainerInternal> EquipmentInfoContainer;

// from `creature` table
struct CreatureData
{
    explicit CreatureData() : dbData(true) {}
    uint32 id;                                              // entry in creature_template
    std::string linkedId;
    uint16 mapid;
    uint16 zoneId;
    uint16 areaId;
    uint32 phaseMask;
    uint32 phaseID;
    uint32 displayid;
    int8 equipmentId;
    float posX;
    float posY;
    float posZ;
    float orientation;
    uint32 spawntimesecs;
    float spawndist;
    uint32 currentwaypoint;
    uint64 curhealth;
    uint32 curmana;
    uint8 movementType;
    uint32 spawnMask;
    uint32 NpcFlags1;
    uint32 NpcFlags2;
    uint32 UnitFlags1;                                     // enum UnitFlags mask values
    uint32 UnitFlags2;                                     // enum UnitFlags2 mask values
    uint32 UnitFlags3;                                     // enum UnitFlags3 mask values
    uint32 dynamicflags;
    uint32 WorldEffectID;
    bool isActive;
    std::string NameTag;
    uint32 ScriptId;
    bool dbData;
};

/// `creature_addon` table
struct CreatureAddon
{
    uint32 PathID;
    uint32 Mount;
    uint32 Bytes1;
    uint32 Bytes2;
    uint32 Emote;
    uint16 aiAnimKit;
    uint16 movementAnimKit;
    uint16 meleeAnimKit;
    std::vector<uint32> Auras;
};

typedef std::unordered_map<uint32, CreatureAddon> CreatureAddonContainer;

struct CreatureModelInfo
{
    float bounding_radius;
    float combat_reach;
    int8 gender;
    uint32 displayId_other_gender;
};

// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint32, CreatureModelInfo> CreatureModelContainer;

enum InhabitTypeValues
{
    INHABIT_GROUND = 1,
    INHABIT_WATER  = 2,
    INHABIT_AIR    = 4,
    INHABIT_ROOT   = 8,
    INHABIT_ANYWHERE = INHABIT_GROUND | INHABIT_WATER | INHABIT_AIR | INHABIT_ROOT
};

// Enums used by StringTextData::Type (CreatureEventAI)
enum ChatType
{
    CHAT_TYPE_SAY               = 0,
    CHAT_TYPE_YELL              = 1,
    CHAT_TYPE_TEXT_EMOTE        = 2,
    CHAT_TYPE_BOSS_EMOTE        = 3,
    CHAT_TYPE_WHISPER           = 4,
    CHAT_TYPE_BOSS_WHISPER      = 5,
    CHAT_TYPE_ZONE_YELL         = 6,
    CHAT_TYPE_END               = 255
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

// Vendors
struct VendorItem
{
    VendorItem(uint32 _item, int32 _maxcount, uint32 _incrtime, uint32 _ExtendedCost, uint8 _Type,  uint32 p_PlayerConditionID, std::vector<uint32> p_BonusList)
        : item(_item), maxcount(_maxcount), incrtime(_incrtime), ExtendedCost(_ExtendedCost), Type(_Type), PlayerConditionID(p_PlayerConditionID), BonusList(p_BonusList) {}

    uint32 item;
    uint32 maxcount;                                        // 0 for infinity item amount
    uint32 incrtime;                                        // time for restore items amount if maxcount != 0
    uint32 ExtendedCost;
    uint8  Type;
    uint32 PlayerConditionID;                               ///< PlayerCondition
    std::vector<uint32> BonusList;
    //helpers

    bool IsGoldRequired(ItemTemplate const* pProto) const { return pProto->Flags2 & int32(ItemFlagsEX::DONT_IGNORE_BUY_PRICE) || !ExtendedCost; }
};
typedef std::vector<VendorItem*> VendorItemList;

struct VendorItemData
{
    VendorItemList m_items;

    VendorItem* GetItem(uint32 slot) const
    {
        if (slot >= m_items.size())
            return NULL;

        return m_items[slot];
    }
    bool Empty() const { return m_items.empty(); }
    uint32 GetItemCount() const { return m_items.size(); }
    void AddItem(uint32 item, int32 maxcount, uint32 ptime, uint32 ExtendedCost, uint8 type, uint32 p_PlayerConditionID, std::vector<uint32> p_BonusList)
    {
        m_items.push_back(new VendorItem(item, maxcount, ptime, ExtendedCost, type, p_PlayerConditionID, p_BonusList));
    }
    bool RemoveItem(uint32 item_id, uint8 type);
    VendorItem const* FindItemCostPair(uint32 item_id, uint32 extendedCost, uint8 type) const;
    void Clear()
    {
        for (VendorItemList::const_iterator itr = m_items.begin(); itr != m_items.end(); ++itr)
            delete (*itr);
        m_items.clear();
    }
};

struct VendorItemCount
{
    explicit VendorItemCount(uint32 _item, uint32 _count)
        : itemId(_item), count(_count), lastIncrementTime(time(NULL)) {}

    uint32 itemId;
    uint32 count;
    time_t lastIncrementTime;
};

typedef std::list<VendorItemCount> VendorItemCounts;

struct TrainerSpell
{
    TrainerSpell() : spell(0), spellCost(0), reqSkill(0), reqSkillValue(0), reqLevel(0)
    {
        for (uint8 i = 0; i < SpellEffIndex::MAX_EFFECTS; ++i)
            learnedSpell[i] = 0;
    }

    uint32 spell;
    uint32 spellCost;
    uint32 reqSkill;
    uint32 reqSkillValue;
    uint32 reqLevel;
    uint32 learnedSpell[32];

    // helpers
    bool IsCastable() const { return learnedSpell[0] != spell; }
};

typedef std::unordered_map<uint32 /*spellid*/, TrainerSpell> TrainerSpellMap;

struct TrainerSpellData
{
    TrainerSpellData() : trainerType(0) {}
    ~TrainerSpellData() { spellList.clear(); }

    TrainerSpellMap spellList;
    uint32 trainerType;                                     // trainer type based at trainer spells, can be different from creature_template value.
    TrainerSpell const* Find(uint32 spell_id) const;
};

typedef std::map<uint32, time_t> CreatureSpellCooldowns;

// max different by z coordinate for creature aggro reaction
#define CREATURE_Z_ATTACK_RANGE 3

class Creature : public Unit, public GridObject<Creature>, public MapObject
{
    public:
        explicit Creature(bool isWorldObject = false);
        virtual ~Creature();

        void AddToWorld() override;
        void RemoveFromWorld() override;

        void AddToGrid(GridRefManager<Creature>& m) override;
        void AddToNearbyGrids();
        void UpdatePresenceForNearbyGrids();

        void DisappearAndDie();

        bool Create(uint32 guidlow, Map* map, uint32 phaseMask, uint32 Entry, uint32 vehId, uint32 team, float x, float y, float z, float ang, const CreatureData* data = NULL);
        bool LoadCreaturesAddon();
        void SelectLevel(const CreatureTemplate* cinfo);
        void UpdateStatsForLevel();
        void UpdateStat();
        void UpdateGroupSizeStats();
        void LoadEquipment(int8 p_ID = 1, bool p_Force = false);
        void LoadSpecialEquipment(uint32 p_First, uint32 p_Second = 0, uint32 p_Third = 0);

        CreatureLevelStatContainer m_levelStat;
        void GenerateScaleLevelStat(const CreatureTemplate* p_Template);
        CreatureLevelStat const* GetScaleLevelStat(uint8 p_Level);

        uint32 GetDBTableGUIDLow() const { return m_DBTableGuid; }

        void Update(uint32 time) override;                         // overwrited Unit::Update
        void GetRespawnPosition(float &x, float &y, float &z, float* ori = nullptr, float* dist = nullptr) const;

        void SetCorpseDelay(uint32 delay) { m_corpseDelay = delay; }
        uint32 GetCorpseDelay() const { return m_corpseDelay; }
        bool isRacialLeader() const { return GetCreatureTemplate()->RacialLeader; }
        bool isCivilian() const { return GetFlagsExtra() & CREATURE_FLAG_EXTRA_CIVILIAN; }
        bool isTrigger() const { return GetFlagsExtra() & CREATURE_FLAG_EXTRA_TRIGGER; }
        bool isGuard() const { return GetFlagsExtra() & CREATURE_FLAG_EXTRA_GUARD; }
        bool canWalk() const { return GetCreatureTemplate()->InhabitType & INHABIT_GROUND || (GetCreatureTemplate()->InhabitType & INHABIT_AIR && !CanFly()); }
        bool CanSwim() const override { return GetCreatureTemplate()->InhabitType & INHABIT_WATER || isPet(); }
        bool CanFly() const override;
        bool CanShared()  const { return GetCreatureTemplate()->QuestPersonalLoot; }

        void SetReactState(ReactStates st) { m_reactState = st; }
        ReactStates GetReactState() { return m_reactState; }
        bool HasReactState(ReactStates state) const { return (m_reactState == state); }
        void InitializeReactState()
        {
            if (isTotem() || isTrigger() || GetCreatureType() == CREATURE_TYPE_CRITTER || isSpiritService())
                SetReactState(REACT_PASSIVE);
            else if (isPet())
                SetReactState(REACT_HELPER);
            else
                SetReactState(REACT_AGGRESSIVE);
            /*else if (isCivilian())
            SetReactState(REACT_DEFENSIVE);*/;
        }

        ///// TODO RENAME THIS!!!!!
        bool isCanTrainingOf(Player* player, bool msg) const;
        bool isCanInteractWithBattleMaster(Player* player, bool msg) const;
        bool isCanTrainingAndResetTalentsOf(Player* player) const;
        bool canCreatureAttack(Unit const* victim, bool force = true) const;
        bool IsInCombatWithPlayer();

        bool IsImmunedToSpell(SpellInfo const* spellInfo) override;
                                                            // redefine Unit::IsImmunedToSpell
        bool IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const override;
                                                            // redefine Unit::IsImmunedToSpellEffect
        bool isElite() const
        {
            if (isPet())
                return false;

            uint32 rank = GetCreatureTemplate()->rank;
            return rank != CREATURE_ELITE_NORMAL && rank != CREATURE_ELITE_RARE;
        }

        bool isWorldBoss() const
        {
            if (isPet())
                return false;

            return GetCreatureTemplate()->type_flags & CREATURE_TYPE_FLAG_BOSS_MOB;
        }

        bool IsDungeonBoss() const;
        bool IsPersonal() const;
        bool IsPersonalForQuest(Player const* player) const;

        uint32 GetVignetteId() const { return m_creatureInfo ? m_creatureInfo->VignetteID : 0; }
        uint32 GetQuestLootId() const { return m_creatureInfo ? m_creatureInfo->QuestLootID : 0; }

        uint8 getLevelForTarget(WorldObject const* target) const override; // overwrite Unit::getLevelForTarget for boss level support

        bool IsInEvadeMode() const { return HasUnitState(UNIT_STATE_EVADE); }

        bool AIM_Initialize(CreatureAI* ai = nullptr);
        void Motion_Initialize();

        bool isCanGiveSpell(Unit* /*caster*/, SpellInfo const* p_ProcSpell)
        {
            if (IsPetGuardianStuff())
                return true;

            if (p_ProcSpell->AttributesEx & SPELL_ATTR1_CANT_BE_REFLECTED || p_ProcSpell->AttributesEx & SPELL_ATTR1_CANT_BE_REDIRECTED)
                return false;
            return true;
        }

        void AI_SendMoveToPacket(float x, float y, float z, uint32 time, uint32 MovementFlags, uint8 type);
        inline CreatureAI* AI() const { return (CreatureAI*)i_AI; }

        GarrisonNPCAI* ToGarrisonNPCAI() const;

        SpellSchoolMask GetMeleeDamageSchoolMask() const override { return m_meleeDamageSchoolMask; }
        void SetMeleeDamageSchool(SpellSchools school) { m_meleeDamageSchoolMask = SpellSchoolMask(1 << school); }

        void _AddCreatureSpellCooldown(uint32 spell_id, time_t end_time);
        void _AddCreatureCategoryCooldown(uint32 category, time_t apply_time);
        void AddCreatureSpellCooldown(uint32 spellid);
        bool HasSpellCooldown(uint32 spell_id) const;
        bool HasCategoryCooldown(uint32 spell_id) const;
        uint32 GetCreatureSpellCooldownDelay(uint32 spellId) const
        {
            CreatureSpellCooldowns::const_iterator itr = m_CreatureSpellCooldowns.find(spellId);
            time_t t = time(NULL);
            return uint32(itr != m_CreatureSpellCooldowns.end() && itr->second > t ? itr->second - t : 0);
        }
        virtual void ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs, uint32 p_SpellID = 0) override;

        bool HasSpell(uint32 spellID) const override;

        bool UpdateEntry(uint32 entry, uint32 team = ALLIANCE, const CreatureData* data = nullptr) ;

        void UpdateMovementFlags();

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType, bool l_IsNoLongerDualWielding = false) override;

        int8 GetOriginalEquipmentId() const { return m_OriginalEquipmentId; }
        uint8 GetCurrentEquipmentId() { return m_equipmentId; }
        void SetCurrentEquipmentId(uint8 p_ID) { m_equipmentId = p_ID; }

        float GetSpellDamageMod(int32 Rank) const;

        VendorItemData const* GetVendorItems() const;
        uint32 GetVendorItemCurrentCount(VendorItem const* vItem);
        uint32 UpdateVendorItemCurrentCount(VendorItem const* vItem, uint32 used_count);

        TrainerSpellData const* GetTrainerSpells() const;

        CreatureTemplate const* GetCreatureTemplate() const { return m_creatureInfo; }
        CreatureTemplate const* GetNativeTemplate() const { return m_NativeCreatureInfo; }
        CreatureData const* GetCreatureData() const { return m_creatureData; }
        CreatureAddon const* GetCreatureAddon() const;
        CreatureScript* GetCreatureScript();

        std::string GetAIName() const;
        std::string GetScriptName() const;
        uint32 GetScriptId() const;

        void Say(int32 textId, uint32 language, uint64 TargetGuid) { MonsterSay(textId, language, TargetGuid); }
        void Yell(int32 textId, uint32 language, uint64 TargetGuid) { MonsterYell(textId, language, TargetGuid); }
        void TextEmote(int32 textId, uint64 TargetGuid, bool IsBossEmote = false) { MonsterTextEmote(textId, TargetGuid, IsBossEmote); }
        void Whisper(int32 textId, uint64 receiver, bool IsBossWhisper = false) { MonsterWhisper(textId, receiver, IsBossWhisper); }
        void YellToZone(int32 textId, uint32 language, uint64 TargetGuid) { MonsterYellToZone(textId, language, TargetGuid); }

        // override WorldObject function for proper name localization
        const char* GetNameForLocaleIdx(LocaleConstant locale_idx) const override;

        uint32 CalculateDynamicResapawnTime();
        void setDeathState(DeathState s) override;

        bool LoadFromDB(uint32 guid, Map* map) { return LoadCreatureFromDB(guid, map, false); }
        bool LoadCreatureFromDB(uint32 guid, Map* map, bool addToMap = true, bool p_GenerateNewGuid = true);
        void SaveToDB();
                                                            // overriden in Pet
        virtual void SaveToDB(uint32 mapid, uint32 spawnMask, uint32 phaseMask);
        virtual void DeleteFromDB();                        // overriden in Pet

        std::map<uint64, Loot> m_LootContainers;
        bool lootForPickPocketed;
        bool lootForBody;
        bool isTappedBy(Player const* player) const;                          // return true if the creature is tapped by the player or a member of his party.

        void AllLootRemovedFromCorpse();

        uint32 GetLootMode() const { return m_LootMode; }
        bool HasLootMode(uint32 lootMode) const { return m_LootMode & lootMode; }
        void SetLootMode(uint32 lootMode) { m_LootMode = lootMode; }
        void AddLootMode(uint32 lootMode) { m_LootMode |= lootMode; }
        void RemoveLootMode(uint32 lootMode) { m_LootMode &= ~lootMode; }
        void ResetLootMode() { m_LootMode = LOOT_MODE_DEFAULT; }

        SpellInfo const* reachWithSpellAttack(Unit* victim);
        SpellInfo const* reachWithSpellCure(Unit* victim);

        uint32 m_spells[MAX_CREATURE_SPELLS];
        CreatureSpellCooldowns m_CreatureSpellCooldowns;
        CreatureSpellCooldowns m_CreatureCategoryCooldowns;

        bool canStartAttack(Unit const* u, bool force) const;
        float GetAttackDistance(Unit const* player) const;
        float GetAggroRange(Unit const* target) const;

        void SendAIReaction(AiReaction reactionType);

        Unit* SelectNearestTarget(float dist = 0) const;
        Unit* SelectNearestTargetNoCC(float dist = 0) const;
        Unit* SelectNearestTargetInAttackDistance(float dist = 0) const;
        Player* SelectNearestPlayer(float distance = 0) const;
        Player* SelectNearestPlayerNotGM(float distance = 0) const;
        Unit* SelectNearestHostileUnitInAggroRange(bool useLOS = false) const;

        void DoFleeToGetAssistance();
        void CallForHelp(float fRadius);
        void CallAssistance(float p_Radius = 0.0f, bool p_CheckFaction = true);
        void SetNoCallAssistance(bool val) { m_AlreadyCallAssistance = val; }
        void SetNoSearchAssistance(bool val) { m_AlreadySearchedAssistance = val; }
        bool HasSearchedAssistance() { return m_AlreadySearchedAssistance; }
        bool CanAssistTo(const Unit* u, const Unit* enemy, bool checkfaction = true) const;
        bool _IsTargetAcceptable(const Unit* target) const;

        MovementGeneratorType GetDefaultMovementType() const { return m_defaultMovementType; }
        void SetDefaultMovementType(MovementGeneratorType mgt) { m_defaultMovementType = mgt; }

        void RemoveCorpse(bool setSpawnTime = true, bool p_HomePosAsRespawn = false);

        void DespawnOrUnsummon(uint32 msTimeToDespawn = 0);
        void DespawnCreaturesInArea(uint32 p_Entry, float p_Range = 100.0f);
        void DespawnCreaturesInArea(std::vector<uint32> p_Entry, float p_Range = 100.0f);
        void DespawnAreaTriggersInArea(uint32 p_SpellID, float p_Range = 100.0f);
        void DespawnAreaTriggersInArea(std::vector<uint32> p_SpellIDs, float p_Range = 100.0f, bool p_ClearOthers = true);

        time_t const& GetRespawnTime() const { return m_respawnTime; }
        time_t GetRespawnTimeEx() const;
        void SetRespawnTime(uint32 respawn) { m_respawnTime = respawn ? time(NULL) + respawn : 0; }
        void Respawn(bool force = false, bool p_HomePosAsRespawn = false, uint32 p_RespawnTime = 2 * TimeConstants::IN_MILLISECONDS);

        void StopAttack();

        void SaveRespawnTime() override;

        uint32 GetRemoveCorpseDelay() const { return uint32(m_corpseRemoveTime); }
        void SetRemoveCorpseDelay(uint32 delay) { m_corpseRemoveTime = delay; }

        uint32 GetRespawnDelay() const { return m_respawnDelay; }
        void SetRespawnDelay(uint32 delay) { m_respawnDelay = delay; }

        float GetRespawnRadius() const { return m_respawnradius; }
        void SetRespawnRadius(float dist) { m_respawnradius = dist; }

        uint32 m_groupLootTimer;                            // (msecs)timer used for group loot

        void SendZoneUnderAttackMessage(Player* attacker);

        void SetInCombatWithZone();

        bool hasQuest(uint32 quest_id) const override;
        bool hasInvolvedQuest(uint32 quest_id)  const override;

        bool isRegeneratingHealth() { return m_regenHealth; }
        void setRegeneratingHealth(bool regenHealth) { m_regenHealth = regenHealth; }
        virtual uint8 GetPetAutoSpellSize() const { return MAX_SPELL_CHARM; }
        virtual uint32 GetPetAutoSpellOnPos(uint8 pos) const
        {
            if (m_charmInfo == nullptr)
                return 0;

            if (pos >= MAX_SPELL_CHARM || m_charmInfo->GetCharmSpell(pos) == nullptr || m_charmInfo->GetCharmSpell(pos)->GetType() != ACT_ENABLED)
                return 0;
            else
                return m_charmInfo->GetCharmSpell(pos)->GetAction();
        }

        void SetPosition(float x, float y, float z, float o);
        void SetPosition(const Position &pos) { SetPosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()); }

        void SetHomePosition(float x, float y, float z, float o) { m_homePosition.Relocate(x, y, z, o); }
        void SetHomePosition(const Position &pos) { m_homePosition.Relocate(pos); }
        void GetHomePosition(float &x, float &y, float &z, float &ori) const { m_homePosition.GetPosition(x, y, z, ori); }
        Position GetHomePosition() const { return m_homePosition; }

        void SetTransportHomePosition(float x, float y, float z, float o) { m_transportHomePosition.Relocate(x, y, z, o); }
        void SetTransportHomePosition(const Position &pos) { m_transportHomePosition.Relocate(pos); }
        void GetTransportHomePosition(float &x, float &y, float &z, float &ori) { m_transportHomePosition.GetPosition(x, y, z, ori); }
        Position GetTransportHomePosition() { return m_transportHomePosition; }

        uint32 GetWaypointPath(){return m_path_id;}
        void LoadPath(uint32 pathid) { m_path_id = pathid; }

        uint32 GetCurrentWaypointID(){return m_waypointID;}
        void UpdateWaypointID(uint32 wpID){m_waypointID = wpID;}
        uint32 GetPrevWaypointID(){return m_PrevWaypointID;}
        void UpdatePrevWaypointID(uint32 p_WaypointId){m_PrevWaypointID = p_WaypointId;}

        void SearchFormation();
        CreatureGroup* GetFormation() {return m_formation;}
        void SetFormation(CreatureGroup* formation) {m_formation = formation;}

        Unit* SelectVictim();

        void SetDisableReputationGain(bool disable) { DisableReputationGain = disable; }
        bool IsReputationGainDisabled() { return DisableReputationGain; }

        uint32 GetOriginalEntry() const { return m_originalEntry; }
        void SetOriginalEntry(uint32 entry) { m_originalEntry = entry; }

        static float _GetDamageMod(int32 Rank);

        float m_SightDistance, m_CombatDistance;

        void SetGUIDTransport(uint32 guid) { guid_transport=guid; }
        uint32 GetGUIDTransport() { return guid_transport; }

        void FarTeleportTo(Map* map, float X, float Y, float Z, float O);
        uint32 GetPlayerCount() const { return m_playerCount; }

        bool m_isTempWorldObject; //true when possessed

        void SetBossID(uint32 p_ID) { m_BossID = p_ID; }
        uint32 GetBossID() const { return m_BossID; }

        void ForcedDespawn(uint32 timeMSToDespawn = 0);

        void SetLockAI(bool lock) { m_AI_locked = lock; }

        void SendAddFollowerQuery(Player* p_Player, uint32 p_Sender, uint32 p_Action, char const* p_FollowerName);

        uint32 m_LOSCheckTimer;
        bool m_LOSCheck_creature;
        bool m_LOSCheck_player;

        void SetEncounterStartTime(time_t p_Time) { m_StartEncounterTime = p_Time; }
        time_t GetEncounterStartTime() const { return m_StartEncounterTime; }

        CreatureDamageLogList const& GetDamageLogs() { return m_DamageLogs; }
        void AddDamageLog(CreatureDamageLog p_Log) { m_DamageLogs.push_back(p_Log); }
        void ClearDamageLog() { m_DamageLogs.clear(); }

        GroupDumpList const& GetGroupDumps() { return m_GroupDumps; }
        void AddGroupDump(GroupDump p_Dump) { m_GroupDumps.push_back(p_Dump); }
        void ClearGroupDumps() { m_GroupDumps.clear(); }

        void DumpGroup();

        void AddMovementInform(uint32 p_Type, uint32 p_ID)
        {
            m_MovementInform.push_back(std::make_pair(p_Type, p_ID));
        }

        void ClearLootContainers();

        bool IsDireBeast() const;
        bool IsHati() const;

        void SetBonusLootSpell(uint32 p_SpellID) { m_BonusLootSpellID = p_SpellID; }
        uint32 GetBonusLootSpell() const { return m_BonusLootSpellID; }

        bool IsKillable() const { return m_IsKillable; }
        void SetKillable(bool p_Value) { m_IsKillable = p_Value; }

        std::string ComputeLinkedId() const;

        void SetEntryForOwner(uint32 p_Entry) { m_SpecificEntryForOwner = p_Entry; }

        uint32 GetVignetteID() const { return m_VignetteID; }
        void SetVignetteID(uint32 p_VignetteID) { m_VignetteID = p_VignetteID; }

        uint32 GetTrackingQuestID() const { return m_TrackingQuestID; }
        void SetTrackingQuestID(uint32 p_TrackingQuestID) { m_TrackingQuestID = p_TrackingQuestID; }

        void AddAllowerTrackingQuestLooter(uint64 p_Guid) { m_AllowedTrackingQuestLooters.insert(p_Guid); }
        bool IsAllowedTrackingQuestLooter(uint64 p_Guid) const { return m_AllowedTrackingQuestLooters.find(p_Guid) != m_AllowedTrackingQuestLooters.end(); }

        void SetCosmetic(bool p_Apply) { m_IsCosmetic = p_Apply; }
        bool IsCosmetic() const { return m_IsCosmetic; }

        bool IsNeedRespawn() const { return m_NeedRespawn; }

        uint32 GetFlagsExtra() const { return m_FlagExtras; }
        void SetFlagExtra(uint32 p_Flag, bool p_Apply = true);

        void SetNameTag(std::string p_NameTag) { m_NameTag = p_NameTag; }
        std::string GetNameTag() const { return m_NameTag; }

        void InitDireBeastDisplayIdAndScale(uint32& displayId, float& scale);

    private:
        void DoRespawn();

        bool m_NeedRespawn;
        int m_RespawnFrameDelay;

        int32 m_MovingUpdateTimer;
        int32 m_NotMovingUpdateTimer;

        uint32 m_BonusLootSpellID;

        bool m_IsKillable;
        bool m_IsCosmetic;

        uint32 m_FlagExtras;

    protected:
        bool CreateFromProto(uint32 guidlow, uint32 Entry, uint32 vehId, uint32 team, const CreatureData* data = nullptr);
        bool InitEntry(uint32 entry, uint32 team = ALLIANCE, const CreatureData* data = nullptr);

        // vendor items
        VendorItemCounts m_vendorItemCounts;

        static float _GetHealthMod(int32 Rank);
        float _GetHealthModPersonal(uint32 &p_Count);
        float _GetDamageModForDiff();
        float _GetHealthModForDiff();

        /// Timers
        time_t m_corpseRemoveTime;                          ///< (msecs) timer for death or corpse disappearance
        time_t m_respawnTime;                               ///< (secs)  time of next respawn
        time_t m_respawnChallenge;                          ///< (secs) time of next respawn
        uint32 m_respawnDelay;                              ///< (secs)  delay between corpse disappearance and respawning
        uint32 m_corpseDelay;                               ///< (secs)  delay between death and corpse disappearance
        float m_respawnradius;

        ReactStates m_reactState;                           ///< for AI, not charmInfo
        void RegenerateMana();
        void RegenerateHealth();
        void Regenerate(Powers power);
        MovementGeneratorType m_defaultMovementType;
        uint32 m_DBTableGuid;                               ///< For new or temporary creatures is 0 for saved it is lowguid
        uint8 m_equipmentId;
        int8 m_OriginalEquipmentId; // can be -1

        bool m_AlreadyCallAssistance;
        bool m_AlreadySearchedAssistance;
        bool m_regenHealth;
        bool m_AI_locked;

        SpellSchoolMask m_meleeDamageSchoolMask;
        uint32 m_originalEntry;

        Position m_homePosition;
        Position m_transportHomePosition;

        bool DisableReputationGain;

        CreatureTemplate const* m_creatureInfo;             ///  0 can different from sObjectMgr->GetCreatureTemplate(GetEntry())
        CreatureTemplate const* m_NativeCreatureInfo;
        CreatureData const* m_creatureData;
        CreatureScript* m_CreatureScript;

        bool m_HasDifficultyEntry;

        uint32 m_LootMode;                                  ///< bitmask, default LOOT_MODE_DEFAULT, determines what loot will be lootable
        uint32 guid_transport;
        uint32 m_playerCount;
        uint32 m_BossID;
        uint32 m_HealthRegenTimer;

        uint8 m_spawnMode;

        bool IsInvisibleDueToDespawn() const override;
        bool CanAlwaysSee(WorldObject const* obj) const override;

        std::string m_NameTag;
    private:

        CreatureDamageLogList m_DamageLogs;
        time_t m_StartEncounterTime;

        GroupDumpList m_GroupDumps;
        uint32 m_DumpGroupTimer;

        //WaypointMovementGenerator vars
        uint32 m_PrevWaypointID;
        uint32 m_waypointID;
        uint32 m_path_id;

        //Formation var
        CreatureGroup* m_formation;
        bool TriggerJustRespawned;

        std::list<std::pair<uint32, uint32>> m_MovementInform;

        uint32 m_SkipUpdateCounter;
        uint32 m_DelayedUpdateTime;

        uint32 m_VignetteID;
        uint32 m_TrackingQuestID;

        CellArea m_Cells;
        uint32 m_SpecificEntryForOwner;

        std::set<uint64> m_AllowedTrackingQuestLooters;
};

class AssistDelayEvent : public BasicEvent
{
    public:
        AssistDelayEvent(uint64 victim, Unit& owner) : BasicEvent(), m_victim(victim), m_owner(owner) { }

        bool Execute(uint64 e_time, uint32 p_time);
        void AddAssistant(uint64 guid) { m_assistants.push_back(guid); }
    private:
        AssistDelayEvent();

        uint64            m_victim;
        std::list<uint64> m_assistants;
        Unit&             m_owner;
};

class ForcedDespawnDelayEvent : public BasicEvent
{
    public:
        ForcedDespawnDelayEvent(Creature& owner) : BasicEvent(), m_owner(owner) { }
        bool Execute(uint64 e_time, uint32 p_time);

    private:
        Creature& m_owner;
};

class SetPhaseDelayEvent : public BasicEvent
{
    public:
        SetPhaseDelayEvent(Creature& p_Owner, uint32 p_Phase) : BasicEvent(), m_Owner(p_Owner), m_Phase(p_Phase) { }
        bool Execute(uint64 p_ETime, uint32 p_Time);

    private:
        Creature& m_Owner;
        uint32 m_Phase;
};

#endif
