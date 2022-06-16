////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _SPELLMGR_H
#define _SPELLMGR_H

// For static or at-server-startup loaded spell data

#include "Common.h"
#include "SharedDefines.h"
#include "Unit.h"
#include "SpellLogMgr.h"

class SpellInfo;
class Player;
class Unit;
class ProcEventInfo;
struct SkillLineAbilityEntry;

// only used in code
enum SpellCategories
{
    SPELLCATEGORY_HEALTH_MANA_POTIONS = 4,
    SPELLCATEGORY_DEVOUR_MAGIC        = 12,
    SPELLCATEGORY_JUDGEMENT           = 1210,               // Judgement (seal trigger)
    SPELLCATEGORY_FOOD                = 11,
    SPELLCATEGORY_DRINK               = 59
};

/// SpellFamilyFlags
enum SpellFamilyFlag
{
    /// Rogue
    SPELLFAMILYFLAG_ROGUE_VANISH            = 0x00000800,
    SPELLFAMILYFLAG_ROGUE_VAN_EVAS_SPRINT   = 0x00000860,   ///< Vanish, Evasion, Sprint
    SPELLFAMILYFLAG1_ROGUE_COLDB_SHADOWSTEP = 0x00000240,   ///< Cold Blood, Shadowstep
    SPELLFAMILYFLAG_ROGUE_KICK              = 0x00000010,   ///< Kick
    SPELLFAMILYFLAG_ROGUE_BLADE_FLURRY      = 0x40000000,   ///< Blade Flurry
    SPELLFAMILYFLAG1_ROGUE_BLADE_FLURRY     = 0x00000800,   ///< Blade Flurry

    /// Warrior
    SPELLFAMILYFLAG_WARRIOR_CHARGE          = 0x00000001,
    SPELLFAMILYFLAG_WARRIOR_SLAM            = 0x00200000,
    SPELLFAMILYFLAG_WARRIOR_EXECUTE         = 0x20000000,
    SPELLFAMILYFLAG_WARRIOR_CONCUSSION_BLOW = 0x04000000,

    /// Warlock
    SPELLFAMILYFLAG_WARLOCK_LIFETAP         = 0x00040000,

    /// Druid
    SPELLFAMILYFLAG2_DRUID_STARFALL         = 0x00000100,

    /// Paladin
    SPELLFAMILYFLAG1_PALADIN_DIVINESTORM    = 0x00020000,

    /// Shaman
    SPELLFAMILYFLAG_SHAMAN_FROST_SHOCK      = 0x80000000,
    SPELLFAMILYFLAG_SHAMAN_HEALING_STREAM   = 0x00002000,
    SPELLFAMILYFLAG_SHAMAN_MANA_SPRING      = 0x00004000,
    SPELLFAMILYFLAG2_SHAMAN_LAVA_LASH       = 0x00000004,
    SPELLFAMILYFLAG_SHAMAN_FIRE_NOVA        = 0x28000000,

    /// Deathknight
    SPELLFAMILYFLAG_DK_DEATH_STRIKE         = 0x00000010,
    SPELLFAMILYFLAG_DK_DEATH_COIL           = 0x00002000,

    /// TODO: Figure out a more accurate name for the following familyflag(s)
    SPELLFAMILYFLAG_SHAMAN_TOTEM_EFFECTS    = 0x04000000  ///< Seems to be linked to most totems and some totem effects
};

// Spell proc event related declarations (accessed using SpellMgr functions)
enum ProcFlags
{
    PROC_FLAG_NONE                            = 0x00000000,

    PROC_FLAG_KILLED                          = 0x00000001,    // 00 Killed by agressor - not sure about this flag
    PROC_FLAG_KILL                            = 0x00000002,    // 01 Kill target (in most cases need XP/Honor reward)

    PROC_FLAG_DONE_MELEE_AUTO_ATTACK          = 0x00000004,    // 02 Done melee auto attack
    PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK         = 0x00000008,    // 03 Taken melee auto attack

    PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS      = 0x00000010,    // 04 Done attack by Spell that has dmg class melee
    PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS     = 0x00000020,    // 05 Taken attack by Spell that has dmg class melee

    PROC_FLAG_DONE_RANGED_AUTO_ATTACK         = 0x00000040,    // 06 Done ranged auto attack
    PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK        = 0x00000080,    // 07 Taken ranged auto attack

    PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS     = 0x00000100,    // 08 Done attack by Spell that has dmg class ranged
    PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS    = 0x00000200,    // 09 Taken attack by Spell that has dmg class ranged

    PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS   = 0x00000400,    // 10 Done positive spell that has dmg class none
    PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS  = 0x00000800,    // 11 Taken positive spell that has dmg class none

    PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG   = 0x00001000,    // 12 Done negative spell that has dmg class none
    PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG  = 0x00002000,    // 13 Taken negative spell that has dmg class none

    PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS  = 0x00004000,    // 14 Done positive spell that has dmg class magic
    PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS = 0x00008000,    // 15 Taken positive spell that has dmg class magic

    PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG  = 0x00010000,    // 16 Done negative spell that has dmg class magic
    PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG = 0x00020000,    // 17 Taken negative spell that has dmg class magic

    PROC_FLAG_DONE_PERIODIC                   = 0x00040000,    // 18 Successful do periodic (damage / healing)
    PROC_FLAG_TAKEN_PERIODIC                  = 0x00080000,    // 19 Taken spell periodic (damage / healing)

    PROC_FLAG_TAKEN_DAMAGE                    = 0x00100000,    // 20 Taken any damage
    PROC_FLAG_DONE_TRAP_ACTIVATION            = 0x00200000,    // 21 On trap activation (possibly needs name change to ON_GAMEOBJECT_CAST or USE)

    PROC_FLAG_DONE_MAINHAND_ATTACK            = 0x00400000,    // 22 Done main-hand melee attacks (spell and autoattack)
    PROC_FLAG_DONE_OFFHAND_ATTACK             = 0x00800000,    // 23 Done off-hand melee attacks (spell and autoattack)

    PROC_FLAG_DEATH                           = 0x01000000,    // 24 Died in any way
    PROC_FLAG_JUMP                            = 0x02000000,    // 25 Jumped

    PROC_FLAG_ENTER_COMBAT                    = 0x08000000,    // 27 Entered combat
    PROC_FLAG_ENCOUNTER_START                 = 0x10000000,    // 28 Encounter started

    PROC_FLAG_LOOT                            = 0x40000000,    // 30 On loot

    // flag masks
    AUTO_ATTACK_PROC_FLAG_MASK                = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK
                                                | PROC_FLAG_DONE_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK,

    MELEE_PROC_FLAG_MASK                      = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK
                                                | PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS
                                                | PROC_FLAG_DONE_MAINHAND_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK,

    RANGED_PROC_FLAG_MASK                     = PROC_FLAG_DONE_RANGED_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK
                                                | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS,

    SPELL_PROC_FLAG_MASK                      = PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS
                                                | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS
                                                | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS
                                                | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG
                                                | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS
                                                | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG,

    SPELL_CAST_PROC_FLAG_MASK                  = SPELL_PROC_FLAG_MASK | PROC_FLAG_DONE_TRAP_ACTIVATION | RANGED_PROC_FLAG_MASK,

    PERIODIC_PROC_FLAG_MASK                    = PROC_FLAG_DONE_PERIODIC | PROC_FLAG_TAKEN_PERIODIC,

    DONE_HIT_PROC_FLAG_MASK                    = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_RANGED_AUTO_ATTACK
                                                 | PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS
                                                 | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG
                                                 | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG
                                                 | PROC_FLAG_DONE_PERIODIC | PROC_FLAG_DONE_MAINHAND_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK,

    TAKEN_HIT_PROC_FLAG_MASK                   = PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK | PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK
                                                 | PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS | PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS
                                                 | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_NEG
                                                 | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_NEG
                                                 | PROC_FLAG_TAKEN_PERIODIC | PROC_FLAG_TAKEN_DAMAGE,

    REQ_SPELL_PHASE_PROC_FLAG_MASK             = SPELL_PROC_FLAG_MASK & DONE_HIT_PROC_FLAG_MASK,

    MULTISTRIKE_DONE_HIT_PROC_FLAG_MASK        = PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_RANGED_AUTO_ATTACK
                                                 | PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS | PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS
                                                 | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG
                                                 | PROC_FLAG_DONE_PERIODIC | PROC_FLAG_DONE_MAINHAND_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK,

    POSITIVE_PROC_FLAG_MASK                    = PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_NONE_DMG_CLASS_POS
                                                 | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_TAKEN_SPELL_MAGIC_DMG_CLASS_POS,

    DONE_NEGATIVE_PROC_FLAG_MASK               = PROC_FLAG_KILL | PROC_FLAG_DONE_MELEE_AUTO_ATTACK | PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS
                                                 | PROC_FLAG_DONE_RANGED_AUTO_ATTACK | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_NEG
                                                 | PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_NEG | PROC_FLAG_DONE_PERIODIC
                                                 | PROC_FLAG_DONE_MAINHAND_ATTACK | PROC_FLAG_DONE_OFFHAND_ATTACK
};

#define MELEE_BASED_TRIGGER_MASK (PROC_FLAG_DONE_MELEE_AUTO_ATTACK      | \
                                  PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK     | \
                                  PROC_FLAG_DONE_SPELL_MELEE_DMG_CLASS  | \
                                  PROC_FLAG_TAKEN_SPELL_MELEE_DMG_CLASS | \
                                  PROC_FLAG_DONE_RANGED_AUTO_ATTACK     | \
                                  PROC_FLAG_TAKEN_RANGED_AUTO_ATTACK    | \
                                  PROC_FLAG_DONE_SPELL_RANGED_DMG_CLASS | \
                                  PROC_FLAG_TAKEN_SPELL_RANGED_DMG_CLASS)

enum ProcFlagsExLegacy
{
    PROC_EX_NONE                    = 0x00000000,                 // If none can tigger on Hit/Crit only (passive spells MUST defined by SpellFamily flag)
    PROC_EX_NORMAL_HIT              = 0x00000001,                 // If set only from normal hit (only damage spells)
    PROC_EX_CRITICAL_HIT            = 0x00000002,
    PROC_EX_MISS                    = 0x00000004,
    PROC_EX_RESIST                  = 0x00000008,
    PROC_EX_DODGE                   = 0x00000010,
    PROC_EX_PARRY                   = 0x00000020,
    PROC_EX_BLOCK                   = 0x00000040,
    PROC_EX_EVADE                   = 0x00000080,
    PROC_EX_IMMUNE                  = 0x00000100,
    PROC_EX_DEFLECT                 = 0x00000200,
    PROC_EX_ABSORB                  = 0x00000400,
    PROC_EX_REFLECT                 = 0x00000800,
    PROC_EX_INTERRUPT               = 0x00001000,                 // Melee hit result can be Interrupt (not used)
    PROC_EX_FULL_BLOCK              = 0x00002000,                 // block al attack damage
    PROC_EX_RESERVED2               = 0x00004000,
    PROC_EX_NOT_ACTIVE_SPELL        = 0x00008000,                 // Spell mustn't do damage/heal to proc
    PROC_EX_EX_TRIGGER_ALWAYS       = 0x00010000,                 // If set trigger always no matter of hit result
    PROC_EX_EX_ONE_TIME_TRIGGER     = 0x00020000,                 // If set trigger always but only one time (not implemented yet)
    PROC_EX_ONLY_ACTIVE_SPELL       = 0x00040000,                 // Spell has to do damage/heal to proc

    // Flags for internal use - do not use these in db!
    PROC_EX_INTERNAL_CANT_PROC      = 0x00800000,
    PROC_EX_INTERNAL_DOT            = 0x01000000,
    PROC_EX_INTERNAL_HOT            = 0x02000000,
    PROC_EX_INTERNAL_TRIGGERED      = 0x04000000,
    PROC_EX_INTERNAL_REQ_FAMILY     = 0x08000000,
    PROC_EX_INTERNAL_MULTISTRIKE    = 0x10000000,
};

#define AURA_SPELL_PROC_EX_MASK \
   (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT | PROC_EX_MISS | \
    PROC_EX_RESIST | PROC_EX_DODGE | PROC_EX_PARRY | PROC_EX_BLOCK | \
    PROC_EX_EVADE | PROC_EX_IMMUNE | PROC_EX_DEFLECT | \
    PROC_EX_ABSORB | PROC_EX_REFLECT | PROC_EX_INTERRUPT)

enum ProcFlagsSpellType
{
    PROC_SPELL_TYPE_NONE              = 0x0000000,
    PROC_SPELL_TYPE_DAMAGE            = 0x0000001, // damage type of spell
    PROC_SPELL_TYPE_HEAL              = 0x0000002, // heal type of spell
    PROC_SPELL_TYPE_NO_DMG_HEAL       = 0x0000004, // other spells
    PROC_SPELL_TYPE_MASK_ALL          = PROC_SPELL_TYPE_DAMAGE | PROC_SPELL_TYPE_HEAL | PROC_SPELL_TYPE_NO_DMG_HEAL
};

enum ProcFlagsSpellPhase
{
    PROC_SPELL_PHASE_NONE             = 0x0000000,
    PROC_SPELL_PHASE_CAST             = 0x0000001,
    PROC_SPELL_PHASE_HIT              = 0x0000002,
    PROC_SPELL_PHASE_FINISH           = 0x0000004,
    PROC_SPELL_PHASE_MASK_ALL         = PROC_SPELL_PHASE_CAST | PROC_SPELL_PHASE_HIT | PROC_SPELL_PHASE_FINISH
};

enum ProcFlagsHit
{
    PROC_HIT_NONE                = 0x0000000, // no value - PROC_HIT_NORMAL | PROC_HIT_CRITICAL for TAKEN proc type, PROC_HIT_NORMAL | PROC_HIT_CRITICAL | PROC_HIT_ABSORB for DONE
    PROC_HIT_NORMAL              = 0x0000001, // non-critical hits
    PROC_HIT_CRITICAL            = 0x0000002,
    PROC_HIT_MISS                = 0x0000004,
    PROC_HIT_FULL_RESIST         = 0x0000008,
    PROC_HIT_DODGE               = 0x0000010,
    PROC_HIT_PARRY               = 0x0000020,
    PROC_HIT_BLOCK               = 0x0000040, // partial or full block
    PROC_HIT_EVADE               = 0x0000080,
    PROC_HIT_IMMUNE              = 0x0000100,
    PROC_HIT_DEFLECT             = 0x0000200,
    PROC_HIT_ABSORB              = 0x0000400, // partial or full absorb
    PROC_HIT_REFLECT             = 0x0000800,
    PROC_HIT_INTERRUPT           = 0x0001000, // (not used atm)
    PROC_HIT_FULL_BLOCK          = 0x0002000,
    PROC_HIT_MASK_ALL            = 0x2FFF,
};

enum ProcAttributes
{
    PROC_ATTR_REQ_EXP_OR_HONOR   = 0x0000010,
};

enum eOverrideValues
{
    SpellDurationId             = 0,
    SpellOverrideSpellList      = 1,
    SpellRangeId                = 2,
    SpellStackAmount            = 3,
    SpellAttribute0             = 4,
    SpellAttribute1             = 5,
    SpellAttribute2             = 6,
    SpellAttribute3             = 7,
    SpellAttribute4             = 8,
    SpellAttribute5             = 9,
    SpellAttribute6             = 10,
    SpellAttribute7             = 11,
    SpellAttribute8             = 12,
    SpellAttribute9             = 13,
    SpellAttribute10            = 14,
    SpellAttribute11            = 15,
    SpellAttribute12            = 16,
    SpellAttribute13            = 17,

    EffectName                  = 18,
    EffectApplyAuraName         = 19,
    EffectBasePoints            = 20,
    EffectTargetA               = 21,
    EffectTargetB               = 22,
    EffectMiscA                 = 23,
    EffectMiscB                 = 24,
    EffectRadiusId              = 25,
    EffectTriggerSpell          = 26,
    EffectAmplitude             = 27,
    EffectSpellClassMask0       = 28,
    EffectSpellClassMask1       = 29,
    EffectSpellClassMask2       = 30,
    EffectSpellClassMask3       = 31,

    ProcCharges                 = 32,

    StartRecoveryTime           = 33,

    ProcFlags                   = 34,
    ProcChance                  = 35,
    InterruptFlags              = 36,
    AuraInterruptFlags          = 37,

    SpellCustomAttributes       = 38,
    ChannelInterruptFlags       = 39,

    OverrideRecoveryTime        = 40,
    OverrideDispelType          = 41,
    OverrideTargetAuraState     = 42,
    OverrideSpellSchoolMask     = 43,
    OverrideSpellFamily         = 44
};

struct SpellInfoOverrideEntry
{
    uint32      spellId;                                    ///< SpellID needing data override
    int8        effectID;                                   ///< Index of the effect to override (-1 if not an effect value)
    uint32      overrideValue;                              ///< Data to override (see enum : eOverrideValues)
    int64       value;                                      ///< New Value
};

using SpellInfoOverrideMap = std::unordered_multimap<uint32, SpellInfoOverrideEntry>;

enum eAuraHooks
{
    OnDispel            = 0,
    AfterDispel         = 1,
    AfterEffectApply    = 2,
    AfterEffectRemove   = 3,
    OnEffectPeriodic    = 4,
    AfterEffectPeriodic = 5,
    OnAbsorb            = 6,
    AfterAbsorb         = 7,
    CheckProc           = 8,
    PrepareProc         = 9,
    OnProc              = 10,
    AfterProc           = 11,
    OnEffectProc        = 12,
    AfterEffectProc     = 13,
    OnSplitDamage       = 14
};

enum eAuraScriptActions
{
    CastSpell           = 0,
    CheckingProc        = 1,
    RemoveAura          = 2,
    ModAmount           = 3,
    SetAmount           = 4,
    ModAbsorb           = 5,
    SetDuration         = 6,
    ModDuration         = 7,
    ReduceCooldown      = 8,
    RemoveCooldown      = 9,
    TriggerSAI          = 10,
};

enum eScriptActors
{
    Caster              = 0,
    Target              = 1,
    HitUnit             = 2,
    ProcActor           = 3,
    ProcTarget          = 4
};

enum eCalculationType
{
    TypeCalcPct         = 0,
    TypeAddPct          = 1,
    TypeRemovePct       = 2,
    TypeAddFlat         = 3,
    TypeRemoveFlat      = 4
};

enum eDataSources
{
    SourceSpellInfo     = 0,
    SourceAura          = 1
};

struct AuraScriptEntry
{
    uint32              SpellId;                                    ///< SpellID to which bind the script
    uint8               Hook;                                       ///< Hook used to do stuffs (see eAuraHooks)
    int8                EffectId;                                   ///< Index of the effect to use (-1 if not an effect hook)
    uint8               Action;                                     ///< Action to do (see eAuraScriptActions)
    uint32              ActionSpellId;                              ///< SpellId (use depends of the action)
    uint32              TargetSpellId;                              ///< can override the aura to modify for Set/Mod amount and Set/Mod Duration
    int8                TargetEffectId;                             ///< can override the effectID to modify for Set/Mod amount and Set/Mod Duration
    uint32              TargetAuraType;                             ///< Can replace targetSpellId if wanted.
    uint32              ActionCaster;                               ///< Actor of the action (see eAuraScriptActors)
    uint32              ActionOriginalCaster;                       ///< Original caster if needed (see eAuraScriptActors)
    uint32              ActionTarget;                               ///< Target of the action (see eAuraScriptActors)
    bool                Triggered;                                  ///< for cast spell action
    std::set<uint32>    AuraState;                                  ///< Required auras
    std::set<uint32>    ExcludeAuraState;                           ///< Forbiden auras
    std::set<uint32>    TargetAuraState;                            ///< Target Required auras
    std::set<uint32>    TargetExcludeAuraState;                     ///< Target Forbiden auras
    uint8               CalculationType;                            ///< see eCalculationType
    uint8               DataSource;                                 ///< see eDataSources
    int8                DataEffectId;                               ///< if value is based on a spell data, id of the effect to pick basepoints
    uint32              ArtifactTraitId;                            ///< if needed, trait id to get the rank of the player
    int8                DataStat;                                   ///< if needed, stat to use as data (like health or power)
    std::set<uint32>    ActionSpellList;                            ///< in case of CheckProc, only allow the spellId in this list
};

using AuraScriptMap = std::unordered_multimap<uint32, AuraScriptEntry>;
using AuraScriptMapBounds = std::pair<AuraScriptMap::const_iterator, AuraScriptMap::const_iterator>;

enum eSpellHooks
{
    BeforeCast          = 0,
    OnCast              = 1,
    AfterCast           = 2,
    OnPrepare           = 3,
    CheckCast           = 4,
    TakePowers          = 5,
    OnEffectHit         = 6,
    OnEffectHitTarget   = 7,
    BeforeHit           = 8,
    OnHit               = 9,
    AfterHit            = 10
};

enum eSpellScriptActions
{
    SpellCast           = 0,
    CheckingCast        = 1,
    AuraRemove          = 2,
    ModDamage           = 3,
    SetDamage           = 4,
    ModAuraAmount       = 5,
    SetAuraAmount       = 6,
    ModAuraDuration     = 7,
    SetAuraDuration     = 8,
    CooldownReduce      = 9,
    CooldownRemove      = 10
};

struct SpellScriptEntry
{
    uint32              SpellId;                                    ///< SpellID to which bind the script
    uint8               Hook;                                       ///< Hook used to do stuffs (see eAuraHooks)
    int8                EffectId;                                   ///< Index of the effect to use (-1 if not an effect hook)
    uint8               Action;                                     ///< Action to do (see eAuraScriptActions)
    uint32              ActionSpellId;                              ///< SpellId (use depends of the action)
    uint32              TargetSpellId;                              ///< can override the aura to modify for Set/Mod amount and Set/Mod Duration
    int8                TargetEffectId;                             ///< can override the effectID to modify for Set/Mod amount and Set/Mod Duration
    uint32              TargetAuraType;                             ///< Can replace targetSpellId if wanted.
    uint32              ActionCaster;                               ///< Actor of the action (see eAuraScriptActors)
    uint32              ActionOriginalCaster;                       ///< Original caster if needed (see eAuraScriptActors)
    uint32              ActionTarget;                               ///< Target of the action (see eAuraScriptActors)
    bool                Triggered;                                  ///< for cast spell action
    std::set<uint32>    AuraState;                                  ///< Required auras
    std::set<uint32>    ExcludeAuraState;                           ///< Forbiden auras
    std::set<uint32>    TargetAuraState;                            ///< Target Required auras
    std::set<uint32>    TargetExcludeAuraState;                     ///< Target Forbiden auras
    uint8               CalculationType;                            ///< see eCalculationType
    uint8               DataSource;                                 ///< see eDataSources
    int8                DataEffectId;                               ///< if value is based on a spell data, id of the effect to pick basepoints
    uint32              ArtifactTraitId;                            ///< if needed, trait id to get the rank of the player
    int8                DataStat;                                   ///< if needed, stat to use as data (like health or power)
    std::set<uint32>    ActionSpellList;                            ///< in case of CheckProc, only allow the spellId in this list
};

using SpellScriptMap = std::unordered_multimap<uint32, SpellScriptEntry>;
using SpellScriptMapBounds = std::pair<SpellScriptMap::const_iterator, SpellScriptMap::const_iterator>;

struct SpellProcEventEntry
{
    uint32      schoolMask;                                 // if nonzero - bit mask for matching proc condition based on spell candidate's school: Fire=2, Mask=1<<(2-1)=2
    uint32      spellFamilyName;                            // if nonzero - for matching proc condition based on candidate spell's SpellFamilyNamer value
    flag128     spellFamilyMask;                            // if nonzero - for matching proc condition based on candidate spell's SpellFamilyFlags  (like auras 107 and 108 do)
    uint32      procFlags;                                  // bitmask for matching proc event
    uint32      procEx;                                     // proc Extend info (see ProcFlagsEx)
    float       ppmRate;                                    // for melee (ranged?) damage spells - proc rate per minute. if zero, falls back to flat chance from Spell.db2
    float       customChance;                               // Owerride chance (in most cases for debug only)
    uint32      cooldown;                                   // hidden cooldown used for some spell proc events, applied to _triggered_spell_
    bool        Active;                                     // if set to 1, force l_Active to be true in SpellMgr::IsSpellProcEventCanTriggeredBy();
};

typedef std::vector<SpellProcEventEntry*> SpellProcEventMap;

struct SpellProcEntry
{
    uint32      schoolMask;                                 // if nonzero - bitmask for matching proc condition based on spell's school
    uint32      spellFamilyName;                            // if nonzero - for matching proc condition based on candidate spell's SpellFamilyName
    flag128     spellFamilyMask;                            // if nonzero - bitmask for matching proc condition based on candidate spell's SpellFamilyFlags
    uint32      typeMask;                                   // if nonzero - owerwrite procFlags field for given Spell.db2 entry, bitmask for matching proc condition, see enum ProcFlags
    uint32      spellTypeMask;                              // if nonzero - bitmask for matching proc condition based on candidate spell's damage/heal effects, see enum ProcFlagsSpellType
    uint32      spellPhaseMask;                             // if nonzero - bitmask for matching phase of a spellcast on which proc occurs, see enum ProcFlagsSpellPhase
    uint32      hitMask;                                    // if nonzero - bitmask for matching proc condition based on hit result, see enum ProcFlagsHit
    uint32      attributesMask;                             // bitmask, see ProcAttributes
    float       ratePerMinute;                              // if nonzero - chance to proc is equal to value * aura caster's weapon speed / 60
    float       chance;                                     // if nonzero - owerwrite procChance field for given Spell.db2 entry, defines chance of proc to occur, not used if perMinuteRate set
    uint32      cooldown;                                   // if nonzero - cooldown in secs for aura proc, applied to aura
    uint32      charges;                                    // if nonzero - owerwrite procCharges field for given Spell.db2 entry, defines how many times proc can occur before aura remove, 0 - infinite
};

typedef std::unordered_map<uint32, SpellProcEntry> SpellProcMap;

struct SpellEnchantProcEntry
{
    uint32      customChance;
    float       PPMChance;
    uint32      procEx;
};

typedef std::unordered_map<uint32, SpellEnchantProcEntry> SpellEnchantProcEventMap;

struct SpellBonusEntry
{
    float  direct_damage;
    float  dot_damage;
    float  ap_bonus;
    float  ap_dot_bonus;
};

typedef std::unordered_map<uint32, SpellBonusEntry>     SpellBonusMap;

enum SpellGroup
{
    SPELL_GROUP_NONE = 0,
    SPELL_GROUP_ELIXIR_BATTLE = 1,
    SPELL_GROUP_ELIXIR_GUARDIAN = 2,
    SPELL_GROUP_ELIXIR_UNSTABLE = 3,
    SPELL_GROUP_ELIXIR_SHATTRATH = 4,
    SPELL_GROUP_CORE_RANGE_MAX = 5,
};

#define SPELL_GROUP_DB_RANGE_MIN 1000

//                  spell_id, group_id
typedef std::multimap<uint32, SpellGroup > SpellSpellGroupMap;
typedef std::pair<SpellSpellGroupMap::const_iterator, SpellSpellGroupMap::const_iterator> SpellSpellGroupMapBounds;

//                      group_id, spell_id
typedef std::multimap<SpellGroup, int32> SpellGroupSpellMap;
typedef std::pair<SpellGroupSpellMap::const_iterator, SpellGroupSpellMap::const_iterator> SpellGroupSpellMapBounds;

enum SpellGroupStackRule
{
    SPELL_GROUP_STACK_RULE_DEFAULT                    = 0,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE                  = 1,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE_FROM_SAME_CASTER = 2,
    SPELL_GROUP_STACK_RULE_EXCLUSIVE_SAME_EFFECT      = 3,

    SPELL_GROUP_STACK_RULE_MAX
};

typedef std::map<SpellGroup, SpellGroupStackRule> SpellGroupStackMap;

struct SpellThreatEntry
{
    int32       flatMod;                                    // flat threat-value for this Spell  - default: 0
    float       pctMod;                                     // threat-multiplier for this Spell  - default: 1.0f
    float       apPctMod;                                   // Pct of AP that is added as Threat - default: 0.0f
};

typedef std::map<uint32, SpellThreatEntry> SpellThreatMap;

// coordinates for spells (accessed using SpellMgr functions)
struct SpellTargetPosition
{
    uint32 target_mapId;
    float  target_X;
    float  target_Y;
    float  target_Z;
    float  target_Orientation;
};

typedef std::map<std::pair<uint32 /*spell_id*/, SpellEffIndex /*effIndex*/>, SpellTargetPosition> SpellTargetPositionMap;

struct SpellScene
{
    int32 SceneScriptPackageID;
    int32 MiscValue;
    int32 PlaybackFlags;

    // trigger system
    uint32 TrigerSpell;
    uint32 MonsterCredit;

    /// Internal
    uint32 scriptID;
};

// Enum with EffectRadiusIndex and their actual radius
enum EffectRadiusIndex
{
    EFFECT_RADIUS_2_YARDS       = 7,
    EFFECT_RADIUS_5_YARDS       = 8,
    EFFECT_RADIUS_20_YARDS      = 9,
    EFFECT_RADIUS_30_YARDS      = 10,
    EFFECT_RADIUS_45_YARDS      = 11,
    EFFECT_RADIUS_100_YARDS     = 12,
    EFFECT_RADIUS_10_YARDS      = 13,
    EFFECT_RADIUS_8_YARDS       = 14,
    EFFECT_RADIUS_3_YARDS       = 15,
    EFFECT_RADIUS_1_YARD        = 16,
    EFFECT_RADIUS_13_YARDS      = 17,
    EFFECT_RADIUS_15_YARDS      = 18,
    EFFECT_RADIUS_18_YARDS      = 19,
    EFFECT_RADIUS_25_YARDS      = 20,
    EFFECT_RADIUS_35_YARDS      = 21,
    EFFECT_RADIUS_200_YARDS     = 22,
    EFFECT_RADIUS_40_YARDS      = 23,
    EFFECT_RADIUS_65_YARDS      = 24,
    EFFECT_RADIUS_70_YARDS      = 25,
    EFFECT_RADIUS_4_YARDS       = 26,
    EFFECT_RADIUS_50_YARDS      = 27,
    EFFECT_RADIUS_50000_YARDS   = 28,
    EFFECT_RADIUS_6_YARDS       = 29,
    EFFECT_RADIUS_500_YARDS     = 30,
    EFFECT_RADIUS_80_YARDS      = 31,
    EFFECT_RADIUS_12_YARDS      = 32,
    EFFECT_RADIUS_99_YARDS      = 33,
    EFFECT_RADIUS_55_YARDS      = 35,
    EFFECT_RADIUS_0_YARDS       = 36,
    EFFECT_RADIUS_7_YARDS       = 37,
    EFFECT_RADIUS_21_YARDS      = 38,
    EFFECT_RADIUS_34_YARDS      = 39,
    EFFECT_RADIUS_9_YARDS       = 40,
    EFFECT_RADIUS_150_YARDS     = 41,
    EFFECT_RADIUS_11_YARDS      = 42,
    EFFECT_RADIUS_16_YARDS      = 43,
    EFFECT_RADIUS_0_5_YARDS     = 44,   // 0.5 yards
    EFFECT_RADIUS_10_YARDS_2    = 45,
    EFFECT_RADIUS_5_YARDS_2     = 46,
    EFFECT_RADIUS_15_YARDS_2    = 47,
    EFFECT_RADIUS_60_YARDS      = 48,
    EFFECT_RADIUS_90_YARDS      = 49,
    EFFECT_RADIUS_15_YARDS_3    = 50,
    EFFECT_RADIUS_60_YARDS_2    = 51,
    EFFECT_RADIUS_5_YARDS_3     = 52,
    EFFECT_RADIUS_60_YARDS_3    = 53,
    EFFECT_RADIUS_50000_YARDS_2 = 54,
    EFFECT_RADIUS_130_YARDS     = 55,
    EFFECT_RADIUS_38_YARDS      = 56,
    EFFECT_RADIUS_45_YARDS_2    = 57,
    EFFECT_RADIUS_32_YARDS      = 59,
    EFFECT_RADIUS_44_YARDS      = 60,
    EFFECT_RADIUS_14_YARDS      = 61,
    EFFECT_RADIUS_47_YARDS      = 62,
    EFFECT_RADIUS_23_YARDS      = 63,
    EFFECT_RADIUS_3_5_YARDS     = 64,   // 3.5 yards
    EFFECT_RADIUS_80_YARDS_2    = 65
};

// Spell pet auras
class PetAura
{
    private:
        typedef std::unordered_map<uint32, uint32> PetAuraMap;

    public:
        PetAura() : removeOnChangePet(false), damage(0)
        {
            auras.clear();
        }

        PetAura(uint32 petEntry, uint32 aura, bool _removeOnChangePet, int _damage) :
        removeOnChangePet(_removeOnChangePet), damage(_damage)
        {
            auras[petEntry] = aura;
        }

        uint32 GetAura(uint32 petEntry) const
        {
            PetAuraMap::const_iterator itr = auras.find(petEntry);
            if (itr != auras.end())
                return itr->second;
            PetAuraMap::const_iterator itr2 = auras.find(0);
            if (itr2 != auras.end())
                return itr2->second;
            return 0;
        }

        void AddAura(uint32 petEntry, uint32 aura)
        {
            auras[petEntry] = aura;
        }

        bool IsRemovedOnChangePet() const
        {
            return removeOnChangePet;
        }

        int32 GetDamage() const
        {
            return damage;
        }

    private:
        PetAuraMap auras;
        bool removeOnChangePet;
        int32 damage;
};
typedef std::map<uint32, PetAura> SpellPetAuraMap;

struct SpellArea
{
    uint32 spellId;
    uint32 areaId;                                          // zone/subzone/or 0 is not limited to zone
    uint32 questStart;                                      // quest start (quest must be active or rewarded for spell apply)
    uint32 questEnd;                                        // quest end (quest must not be rewarded for spell apply)
    int32  auraSpell;                                       // spell aura must be applied for spell apply)if possitive) and it must not be applied in other case
    uint64 raceMask;                                        // can be applied only to races
    Gender gender;                                          // can be applied only to gender
    uint32 questStartStatus;                                // QuestStatus that quest_start must have in order to keep the spell
    uint32 questEndStatus;                                  // QuestStatus that the quest_end must have in order to keep the spell (if the quest_end's status is different than this, the spell will be dropped)
    bool autocast;                                          // if true then auto applied at area enter, in other case just allowed to cast

    // helpers
    bool IsFitToRequirements(Player const* player, uint32 newZone, uint32 newArea) const;
};

typedef std::multimap<uint32, SpellArea> SpellAreaMap;
typedef std::multimap<uint32, SpellArea const*> SpellAreaForQuestMap;
typedef std::multimap<uint32, SpellArea const*> SpellAreaForAuraMap;
typedef std::multimap<uint32, SpellArea const*> SpellAreaForAreaMap;
typedef std::pair<SpellAreaMap::const_iterator, SpellAreaMap::const_iterator> SpellAreaMapBounds;
typedef std::pair<SpellAreaForQuestMap::const_iterator, SpellAreaForQuestMap::const_iterator> SpellAreaForQuestMapBounds;
typedef std::pair<SpellAreaForAuraMap::const_iterator, SpellAreaForAuraMap::const_iterator>  SpellAreaForAuraMapBounds;
typedef std::pair<SpellAreaForAreaMap::const_iterator, SpellAreaForAreaMap::const_iterator>  SpellAreaForAreaMapBounds;

// Spell rank chain  (accessed using SpellMgr functions)
struct SpellChainNode
{
    SpellInfo const* prev;
    SpellInfo const* next;
    SpellInfo const* first;
    SpellInfo const* last;
    uint8  rank;
};

typedef std::unordered_map<uint32, SpellChainNode> SpellChainMap;

//                   spell_id  req_spell
typedef std::multimap<uint32, uint32> SpellRequiredMap;
typedef std::pair<SpellRequiredMap::const_iterator, SpellRequiredMap::const_iterator> SpellRequiredMapBounds;

//                   req_spell spell_id
typedef std::multimap<uint32, uint32> SpellsRequiringSpellMap;
typedef std::pair<SpellsRequiringSpellMap::const_iterator, SpellsRequiringSpellMap::const_iterator> SpellsRequiringSpellMapBounds;

// Spell learning properties (accessed using SpellMgr functions)
struct SpellLearnSkillNode
{
    uint16 skill;
    uint16 step;
    uint16 value;                                           // 0  - max skill value for player level
    uint16 maxvalue;                                        // 0  - max skill value for player level
};

typedef std::map<uint32, SpellLearnSkillNode> SpellLearnSkillMap;

struct SpellLearnSpellNode
{
    uint32 spell;
    bool active;                                            // show in spellbook or not
    bool autoLearned;
};

typedef std::multimap<uint32, SpellLearnSpellNode> SpellLearnSpellMap;
typedef std::pair<SpellLearnSpellMap::const_iterator, SpellLearnSpellMap::const_iterator> SpellLearnSpellMapBounds;

typedef std::multimap<uint32, SkillLineAbilityEntry const*> SkillLineAbilityMap;
typedef std::pair<SkillLineAbilityMap::const_iterator, SkillLineAbilityMap::const_iterator> SkillLineAbilityMapBounds;

typedef std::multimap<uint32, uint32> PetLevelupSpellSet;
typedef std::map<uint32, PetLevelupSpellSet> PetLevelupSpellMap;

typedef std::map<uint32, uint32> SpellDifficultySearcherMap;

typedef std::map<uint32, float>  AreaTriggerVisuals;
typedef std::map<uint16, uint16> ItemUpgradeDatas;

#define MAX_CREATURE_SPELL_DATA_SLOT 4

struct PetDefaultSpellsEntry
{
    uint32 spellid[MAX_CREATURE_SPELL_DATA_SLOT];
};

// < 0 for petspelldata id, > 0 for creature_id
typedef std::map<int32, PetDefaultSpellsEntry> PetDefaultSpellsMap;

typedef std::vector<uint32> SpellCustomAttribute;
typedef std::vector<bool> EnchantCustomAttribute;

typedef std::vector<SpellInfo*> SpellInfoMap;

bool IsPrimaryProfessionSkill(uint32 skill);

bool IsWeaponSkill(uint32 skill);

inline bool IsProfessionSkill(uint32 skill)
{
    return  IsPrimaryProfessionSkill(skill) || skill == SKILL_FISHING || skill == SKILL_COOKING || skill == SKILL_FIRST_AID || skill == SKILL_ARCHAEOLOGY || skill == SKILL_WAY_OF_THE_GRILL || skill == SKILL_WAY_OF_THE_WOK || skill == SKILL_WAY_OF_THE_POT || skill == SKILL_WAY_OF_THE_STEAMER || skill == SKILL_WAY_OF_THE_OVEN || skill == SKILL_WAY_OF_THE_BREW;
}

inline bool IsProfessionOrRidingSkill(uint32 skill)
{
    return  IsProfessionSkill(skill) || skill == SKILL_RIDING;
}

bool IsPartOfSkillLine(uint32 skillId, uint32 spellId);

// spell diminishing returns
DiminishingGroup GetDiminishingReturnsGroupForSpell(SpellInfo const* spellproto, Unit* p_Caster);
DiminishingReturnsType GetDiminishingReturnsGroupType(DiminishingGroup group);
DiminishingLevels GetDiminishingReturnsMaxLevel(DiminishingGroup group);
int32 GetDiminishingReturnsLimitDuration(SpellInfo const* spellproto, Unit* p_OriginalCaster);

#define SPELL_LINKED_MAX_SPELLS  300000

enum SpellLinkedType
{
    SPELL_LINK_CAST             = 0,            // +: cast; -: remove
    SPELL_LINK_ON_HIT           = 1 * 300000,
    SPELL_LINK_AURA             = 2 * 300000,   // +: aura; -: immune
    SPELL_LINK_BEFORE_HIT       = 3 * 300000,
    SPELL_LINK_AURA_HIT         = 4 * 300000,
    SPELL_LINK_BEFORE_CAST      = 5 * 300000,
    SPELL_LINK_PREPARE_CAST     = 6 * 300000,
    SPELL_LINK_BEFORE_CHECK     = 7 * 300000,
    SPELL_LINK_FINISH_CAST      = 8 * 300000,
    SPELL_LINK_ON_ADD_TARGET    = 9 * 300000,
    SPELL_LINK_REMOVE           = 0
};

enum SpellLinkedUnitType
{
    LINK_UNIT_TYPE_DEFAULT         = 0,
    LINK_UNIT_TYPE_PET             = 1,
    LINK_UNIT_TYPE_OWNER           = 2,
    LINK_UNIT_TYPE_CASTER          = 3,
    LINK_UNIT_TYPE_SELECTED        = 4,
    LINK_UNIT_TYPE_TARGET          = 5,
    LINK_UNIT_TYPE_VICTIM          = 6,
    LINK_UNIT_TYPE_ATTACKER        = 7,
    LINK_UNIT_TYPE_NEARBY          = 8,
    LINK_UNIT_TYPE_NEARBY_ALLY     = 9,
    LINK_UNIT_TYPE_ORIGINALCASTER  = 10
};

enum SpellLinkedHasType
{
    LINK_HAS_AURA_ON_CASTER       = 0,
    LINK_HAS_AURA_ON_TARGET       = 1,
    LINK_HAS_SPELL_ON_CASTER      = 2,
    LINK_HAS_AURA_ON_OWNER        = 3,
    LINK_HAS_AURATYPE             = 4,
    LINK_HAS_MY_AURA_ON_CASTER    = 5,
    LINK_HAS_MY_AURA_ON_TARGET    = 6,
    LINK_HAS_AURA_STATE           = 7,
    LINK_HAS_SPECID               = 8,
    LINK_HAS_OBJECT_TYPE          = 9,
    LINK_HAS_FRIEND               = 10,
    LINK_HAS_ATTACKABLE           = 11,
    LINK_HAS_DISTANCE             = 12,
    LINK_HAS_AURA_STACK_ON_CASTER = 13,
    LINK_HAS_AURA_STACK_ON_TARGET = 14,
    LINK_HAS_CASTER_FULL_HP       = 15,
    LINK_HAS_TARGET_FULL_HP       = 16
};

enum SpellLinkedActionType
{
    LINK_ACTION_DEFAULT             = 0,
    LINK_ACTION_LEARN               = 1,
    LINK_ACTION_AURATYPE            = 2,
    LINK_ACTION_SEND_COOLDOWN       = 3,
    LINK_ACTION_CAST_NO_TRIGGER     = 4,
    LINK_ACTION_ADD_AURA            = 5,
    LINK_ACTION_CASTINAURA          = 6,
    LINK_ACTION_CHANGE_STACK        = 7,
    LINK_ACTION_REMOVE_COOLDOWN     = 8,
    LINK_ACTION_REMOVE_MOVEMENT     = 9, // RemoveMovementImpairingAuras
    LINK_ACTION_CHANGE_DURATION     = 10, // Mod Duration
    LINK_ACTION_CAST_DEST           = 11,
    LINK_ACTION_CHANGE_CHARGES      = 12,
    LINK_ACTION_MODIFY_COOLDOWN     = 13,
    LINK_ACTION_CATEGORY_COOLDOWN   = 14,
    LINK_ACTION_CATEGORY_CHARGES    = 15,
    LINK_ACTION_RECALCULATE_AMOUNT  = 16,
    LINK_ACTION_CAST_COUNT          = 17,
    LINK_ACTION_FULL_TRIGGER        = 18
};

enum SpellLinkedTargetType
{
    LINK_TARGET_DEFAULT          = 0,
    LINK_TARGET_FROM_EFFECT      = 1
};

enum SpellTriggeredType
{
    SPELL_TRIGGER_BP                            = 0,            // set basepoint to spell from amount
    SPELL_TRIGGER_BP_CUSTOM                     = 1,            // set basepoint to spell custom from BD
    SPELL_TRIGGER_MANA_COST                     = 2,            // set basepoint to spell mana cost
    SPELL_TRIGGER_DAM_HEALTH                    = 3,            // set basepoint to spell damage or heal percent
    SPELL_TRIGGER_COOLDOWN                      = 4,            // Set cooldown for trigger spell
    SPELL_TRIGGER_UPDATE_DUR                    = 5,            // Update duration for select spell
    SPELL_TRIGGER_GET_DUR_AURA                  = 6,            // Get duration from select aura to cast bp
    SPELL_TRIGGER_UPDATE_DUR_TO_MAX             = 8,            // Update duration for select spell to max duration
    SPELL_TRIGGER_PERC_FROM_DAMGE               = 9,            // Percent from damage
    SPELL_TRIGGER_PERC_MAX_MANA                 = 10,           // Percent from max mana
    SPELL_TRIGGER_PERC_BASE_MANA                = 11,           // Percent from base mana
    SPELL_TRIGGER_PERC_CUR_MANA                 = 12,           // Percent from curent mana
    SPELL_TRIGGER_CHECK_PROCK                   = 13,           // Check proc from spell to trigger
    SPELL_TRIGGER_DUMMY                         = 14,           // spell to trigger without option for bp
    SPELL_TRIGGER_CAST_DEST                     = 15,           // spell to trigger without option for bp
    SPELL_TRIGGER_CHECK_DAMAGE                  = 16,           // spell to trigger if damage > amount
    SPELL_TRIGGER_ADD_STACK                     = 17,           // add spell stack
    SPELL_TRIGGER_ADD_CHARGES                   = 18,           // add spell charges
    SPELL_TRIGGER_ADD_CHARGES_STACK             = 19,           // add spell charges and stack
    SPELL_TRIGGER_CAST_OR_REMOVE                = 20,           // cast spell without option
    SPELL_TRIGGER_UPDATE_DUR_TO_IGNORE_MAX      = 21,           // Update duration for select spell to ignore max duration
    SPELL_TRIGGER_ADD_DURATION                  = 22,           // Add duration for select spell
    SPELL_TRIGGER_MODIFY_COOLDOWN               = 23,           // Modify cooldown for trigger spell
    SPELL_TRIGGER_VENGEANCE                     = 24,
    SPELL_TRIGGER_ADD_DURATION_OR_CAST          = 25,           // Add duration for select spell or cast his
    SPELL_TRIGGER_REMOVE_CD_RUNE                = 26,           // Add duration for select spell or cast his
    SPELL_TRIGGER_BP_SPELLID                    = 27,           // set basepoint to spellId from proc
    SPELL_TRIGGER_BP_SPD_AP                     = 28,           // set basepoint to spellId from SPD or AP
    SPELL_TRIGGER_COMBOPOINT_BP                 = 29,           // set basepoint to bp * combopoints
    SPELL_TRIGGER_DAM_PERC_FROM_MAX_HP          = 30,           // set basepoint to (damage / max hp) * 100
    SPELL_TRIGGER_SUMM_DAMAGE_PROC              = 31,           // summ damage in amount, proc if damage > bp0(1,2) * SPD(SPDH,AP)
    SPELL_TRIGGER_ADDPOWER_PCT                  = 32,           // set basepoint to spell add power percent from aura amount
    SPELL_TRIGGER_ADD_ABSORB_PCT                = 33,           // set basepoint from absorb percent
    SPELL_TRIGGER_ADD_BLOCK_PCT                 = 34,           // set basepoint from block percent
    SPELL_TRIGGER_NEED_COMBOPOINTS              = 35,           // Proc from spell that need compopoiunts
    SPELL_TRIGGER_HOLYPOWER_BONUS               = 36,           // Holypower bonus
    SPELL_TRIGGER_CAST_AFTER_MAX_STACK          = 37,           // Cast after max stack
    SPELL_TRIGGER_DAM_MAXHEALTH                 = 38,           // set basepoint to spell damage or max heal percent
    SPELL_TRIGGER_STACK_AMOUNT                  = 39,           // stack damage in amount
    SPELL_TRIGGER_BP_DURATION                   = 40,           // damage is duration
    SPELL_TRIGGER_ADD_STACK_AND_CAST            = 41,           // change stack and set bp = stack
    SPELL_TRIGGER_ADD_SPELL_CHARGES             = 42,           // add spell charges
};

enum DummyTriggerType
{
    DUMMY_TRIGGER_BP                            = 0,            // set basepoint to spell from amount
    DUMMY_TRIGGER_BP_CUSTOM                     = 1,            // set basepoint to spell custom from BD
    DUMMY_TRIGGER_COOLDOWN                      = 2,            // Set cooldown for trigger spell
    DUMMY_TRIGGER_CHECK_PROCK                   = 3,            // Check proc from spell to trigger
    DUMMY_TRIGGER_DUMMY                         = 4,            // spell to trigger without option for bp
    DUMMY_TRIGGER_CAST_DEST                     = 5,            // spell to trigger without option for bp
    DUMMY_TRIGGER_CAST_OR_REMOVE                = 6,            // cast spell without option
    DUMMY_TRIGGER_DAM_MAXHEALTH                 = 7,            // set basepoint to spell damage or max heal percent
    DUMMY_TRIGGER_COPY_AURA                     = 8,            // Copy aura
    DUMMY_TRIGGER_ADD_POWER_COST                = 9,            // Add power cost to spell
    DUMMY_TRIGGER_CAST_DEST2                    = 10,           // Cast spell on dest
    DUMMY_TRIGGER_CAST_IGNORE_GCD               = 11,           // cast ignore GCD
};

enum AuraTriggerType
{
    AURA_TRIGGER                                = 0,            // cast spell
    AURA_TRIGGER_BP                             = 1,            // set basepoint to spell custom from amount
    AURA_TRIGGER_BP_CUSTOM                      = 2,            // set basepoint to spell custom from BD
    AURA_TRIGGER_CHECK_COMBAT                   = 3,            // cast spell in check combat
    AURA_TRIGGER_DEST                           = 4,            // cast spell on dest
    AURA_TRIGGER_DYNOBJECT                      = 5,            // cast spell on dest DynObject
    AURA_TRIGGER_FROM_SUMMON_SLOT               = 6,            // cast spell from summon slot(totem or any)
    AURA_TRIGGER_AREATRIGGER                    = 7,            // cast spell on dest AreaTrigger
    AAURA_TRIGGER_FROM_SUMMON_SLOT_DEST         = 8,            // cast spell from summon slot(totem or any) to dest loc
    AURA_TRIGGER_FROM_SUMMON_DEST               = 9,            // cast spell to summon dest loc
    AURA_TRIGGER_AREATRIGGER_CAST               = 10,           // cast AreaTrigger
    AURA_TRIGGER_TARGETCASTER                   = 11,           // cast spell from the target
};

enum SpellAuraDummyOption
{
    SPELL_DUMMY_ENABLE                          = 0,            // enable or disable aura(set amount to 0)
    SPELL_DUMMY_ADD_PERC                        = 1,            // add percent to amount
    SPELL_DUMMY_ADD_VALUE                       = 2,            // add value to amount
    SPELL_DUMMY_ADD_ATTRIBUTE                   = 3,            // add attribute to spell value
    SPELL_DUMMY_MOD_EFFECT_MASK                 = 4,            // Modify effect mask for add aura
    SPELL_DUMMY_CRIT_RESET                      = 5,            // reset or not crit chance
    SPELL_DUMMY_CRIT_ADD_PERC                   = 6,            // add percent to crit
    SPELL_DUMMY_CRIT_ADD_VALUE                  = 7,            // add value to crit
    SPELL_DUMMY_ADD_PERC_BP                     = 8,            // add percent(bp / 100) to amount
    SPELL_DUMMY_DAMAGE_ADD_PERC                 = 9,            // add percent to damage
    SPELL_DUMMY_DAMAGE_ADD_VALUE                = 10,           // add value to damage
    SPELL_DUMMY_DURATION_ADD_PERC               = 11,           // add percent to duration
    SPELL_DUMMY_DURATION_ADD_VALUE              = 12,           // add value to duration
    SPELL_DUMMY_CASTTIME_ADD_PERC               = 13,           // add percent to castTime
    SPELL_DUMMY_CASTTIME_ADD_VALUE              = 14,           // add value to castTime
};

enum SpellTargetFilterType
{
    SPELL_FILTER_SORT_BY_HEALT                  = 0,            // Sort target by healh
    SPELL_FILTER_BY_AURA                        = 1,            // Remove target by aura
    SPELL_FILTER_BY_DISTANCE                    = 2,            // Check distance
    SPELL_FILTER_TARGET_TYPE                    = 3,            // Check target rype
    SPELL_FILTER_SORT_BY_DISTANCE               = 4,            // Sort by distance
    SPELL_FILTER_TARGET_FRIENDLY                = 5,            // Check Friendly
    SPELL_FILTER_TARGET_IN_RAID                 = 6,            // Check Raid
    SPELL_FILTER_TARGET_IN_PARTY                = 7,            // Check Party
    SPELL_FILTER_TARGET_EXPL_TARGET             = 8,            // Select explicit target
    SPELL_FILTER_TARGET_EXPL_TARGET_REMOVE      = 9,            // Select explicit target remove
    SPELL_FILTER_TARGET_IN_LOS                  = 10,           // Select target in los
    SPELL_FILTER_TARGET_IS_IN_BETWEEN           = 11,           // Select target is in between
    SPELL_FILTER_TARGET_IS_IN_BETWEEN_SHIFT     = 12,           // Select target is in between and shift
    SPELL_FILTER_BY_AURA_OR                     = 13,           // Remove target by any aura
    SPELL_FILTER_BY_ENTRY                       = 14,           // Remove target by any entry
    SPELL_FILTER_TARGET_ATTACKABLE              = 15,           // Check Attackable
    SPELL_FILTER_BY_DISTANCE_TARGET             = 16,           // Sort by distance target
    SPELL_FILTER_BY_PLAYER_ROLE                 = 17,           ///< Check player role
    SPELL_FILTER_BY_PLAYER_TYPE                 = 18,            ///< Check if player is ranged or melee
    SPELL_FILTER_BY_DISTANCE_DEST               = 19,           // Filter by distance dest
    SPELL_FILTER_BY_DISTANCE_PET                = 20,           // Filter by distance pet
    SPELL_FILTER_BY_OWNER                       = 21,           // Filter by owner
    SPELL_FILTER_ONLY_RANGED_SPEC               = 22,           // Select Ranged damager and Healer
    SPELL_FILTER_ONLY_MELEE_SPEC                = 23,           // Select Melee damager and Tank
    SPELL_FILTER_ONLY_TANK_SPEC_OR_NOT          = 24,           // Tank selection or exception
    SPELL_FILTER_BY_AURA_CASTER                 = 25,           // Remove target by aura caster
    SPELL_FILTER_PLAYER_IS_HEALER_SPEC          = 26,           // Select Healer
    SPELL_FILTER_RANGED_SPEC_PRIORITY           = 27,           // Select Ranged damager and Healer Priority
    SPELL_FILTER_MELEE_SPEC_PRIORITY            = 28,           // Select Melee damager and Tank Priority
    SPELL_FILTER_TANK_SPEC_PRIORITY             = 29,           // Select Tank damager and Tank Priority
    SPELL_FILTER_ONLY_PLAYER_TARGET             = 30            // Exclude non-player targets
};

enum eFilterResizeType
{
    FilterResizeTypeFlat        = 1,
    FilterResizeTypeRandom      = 2,
    FilterResizeTypeRaidSizePct = 3
};

enum SpellCheckCastType
{
    SPELL_CHECK_CAST_DEFAULT                    = 0,            // Not check type, check only dataType and dataType2
    SPELL_CHECK_CAST_HEALTH                     = 1,            // Check healh percent
    SPELL_CHECK_CAST_AURA                       = 2,            ///< Check caster's auras
    SPELL_CHECK_CAST_HAS_SPELL                  = 3
};

enum SpellVisualType
{
    SPELL_VISUAL_TYPE_ON_CAST = 0,
    SPELL_VISUAL_TYPE_CUSTOM  = 1,
};

struct SpellTriggered
{
    int32 SpellID;
    int32 SpellTrigger;
    int32 SpellCooldown;
    uint8 Target;
    uint8 Caster;
    uint8 TargetAura;
    uint8 TargetAura2;
    uint8 Options;
    float Bp0;
    float Bp1;
    float Bp2;
    uint8 EffectMask;
    int32 Aura;
    int32 Aura2;
    int32 Chance;
    uint8 Group;
    int64 ProcFlags;
    int32 ProcEx;
    int32 CheckSpellID;
    int32 AddPType;
    int32 SchoolMask;
    int32 DummyID;
    int32 DummyEffect;
    int32 CreatureType;
};

struct SpellDummyTrigger
{
    int32 SpellID;
    int32 SpellTrigger;
    int32 Target;
    int32 Caster;
    int32 TargetAura;
    int32 Options;
    int32 Bp0;
    int32 Bp1;
    int32 Bp2;
    int32 EffectMask;
    int32 Aura;
    int32 Chance;
    int32 Slot;
};

struct SpellAuraDummy
{
    int32 SpellID;
    int32 SpellDummyID;
    int32 Type;
    int32 Target;
    int32 Caster;
    int32 TargetAura;
    int32 Options;
    int32 EffectMask;
    int32 EffectDummy;
    int32 Aura;
    int32 RemoveAura;
    int32 Chance;
    int32 Attr;
    int32 AttrValue;
    int32 CustomBP;
    int32 Charge;
};

struct SpellTargetFilter
{
    int32 SpellID;
    int32 TargetID;
    int32 Options;
    float Param1;
    float Param2;
    float Param3;
    int32 Aura;
    int32 Chance;
    int32 EffectMask;
    int32 ResizeType;
    int32 Count;
    int32 MaxCount;
    int32 AddCount;
    int32 AddCaster;
};

struct SpellCheckCast
{
    int32 SpellID;
    int32 Type;
    int32 ErrorID;
    int32 CustomErrorID;
    int32 Caster;
    int32 Target;
    int32 CheckType;
    int32 DataType;
    int32 CheckType2;
    int32 DataType2;
    int32 Param1;
    int32 Param2;
    int32 Param3;
};

struct SpellVisual
{
    int32 SpellID;
    int32 SpellVisualID;
    int16 MissReason;
    int16 ReflectStatus;
    float TravelSpeed;
    bool  SpeedAsTime;
    bool  HasPosition;
    int16 Type;
};

struct SpellVisualPlayOrphan
{
    int32    SpellID;
    int32    SpellVisualID;
    float    TravelSpeed;
    bool     SpeedAsTime;
    float    UnkFloat;
    Position SourceOrientation;
    int8     Type;
};

struct SpellPendingCast
{
    int32 SpellID;
    int32 PendingID;
    int8  Options;
    int32 Check;
};

struct SpellLinked
{
    int32 effect;
    int32 hastalent;
    uint8 hastype;
    int32 hasparam;
    int32 hastalent2;
    uint8 hastype2;
    int32 hasparam2;
    int32 chance;
    int32 caster;
    int32 target;
    int32 cooldown;
    uint32 hitmask;
    uint32 effectMask;
    int32 removeMask;
    uint8 actiontype;
    int8 targetCount;
    int8 targetCountType;
    int8 group;
    int32 duration;
    float param;
};

typedef std::vector<std::set<uint32> > SpellClassList;
typedef std::map<uint32, std::set<MinorTalentEntry const*> > SpecializatioPerkMap;
typedef std::map<uint32, std::list<uint32> > SpellOverrideInfo;
typedef std::set<uint32> TalentsPlaceHoldersSpell;
typedef std::set<uint32> TalentSpellSet;
typedef std::vector<std::list<uint32> > SpellPowerVector;
typedef std::map<uint32, std::set<uint32>> AvaiableDifficultySpell;
typedef std::map<std::pair<uint32, uint32>, uint32> DataStoreMapPair;
typedef std::map<std::string, DataStoreMapPair> DatastoreDifficultyKey;
typedef std::map<std::string, DataStoreMapPair> SpellBaseDifficultyDBCs;
typedef std::unordered_map<int32, std::vector<SpellTriggered>> SpellTriggeredMap;
typedef std::unordered_map<int32, std::vector<SpellDummyTrigger>> SpellDummyTriggerMap;
typedef std::unordered_map<int32, std::vector<SpellDummyTrigger>> SpellAuraTriggerMap;
typedef std::unordered_map<int32, std::vector<SpellAuraDummy>> SpellAuraDummyMap;
typedef std::unordered_map<int32, std::vector<SpellTargetFilter>> SpellTargetFilterMap;
typedef std::unordered_map<int32, std::vector<SpellCheckCast>> SpellCheckCastMap;
typedef std::unordered_map<int32, std::vector<SpellVisual>> PlaySpellVisualMap;
typedef std::unordered_map<int32, std::vector<SpellVisualPlayOrphan>> SpellVisualPlayOrphanMap;
typedef std::unordered_map<int32, std::vector<SpellPendingCast>> SpellPendingCastMap;
typedef std::unordered_map<int32, std::vector<SpellLinked>> SpellLinkedMap;
typedef std::map<uint32, uint32> RespecializationSpellsMap;

using ItemSourceSkills = std::map<uint32, std::vector<uint32>>;
using TradeSpellSkills = std::map<uint32, std::list<SkillLineAbilityEntry const*>>;

struct SpellUpgradeItemStage
{
    int32 ItemClass;
    int32 ItemSubclassMask;
    int32 InventoryTypeMask;
    int32 MaxIlevel;
};

using SpellUpgradeItemStages = std::map<uint32, std::vector<SpellUpgradeItemStage>>;
using SpellSceneMap = std::map<int32, SpellScene>;
using SpellSceneTriggerMap = std::map<uint32, uint32>;

class SpellMgr
{
    friend class ACE_Singleton<SpellMgr, ACE_Null_Mutex>;
    // Constructors
    private:
        SpellMgr();
        ~SpellMgr();

    // Accessors (const or static functions)
    public:
        // Spell correctness for client using
        static bool IsSpellValid(SpellInfo const* spellInfo, Player* player = NULL, bool msg = true);
        bool IsSpellForbidden(uint32 spellid);

        // Spell difficulty
        uint32 GetSpellDifficultyId(uint32 spellId) const;
        void SetSpellDifficultyId(uint32 spellId, uint32 id);
        SpellInfo const* GetSpellForDifficulty(uint32 p_SpellId, Difficulty p_Difficulty) const;
        SpellInfo const* GetSpellForDifficultyFromSpell(SpellInfo const* spell, Unit const* caster) const;

        // Spell Ranks table
        SpellChainNode const* GetSpellChainNode(uint32 spell_id) const;
        uint32 GetFirstSpellInChain(uint32 spell_id) const;
        uint32 GetLastSpellInChain(uint32 spell_id) const;
        uint32 GetNextSpellInChain(uint32 spell_id) const;
        uint32 GetPrevSpellInChain(uint32 spell_id) const;
        uint8 GetSpellRank(uint32 spell_id) const;
        // not strict check returns provided spell if rank not avalible
        uint32 GetSpellWithRank(uint32 spell_id, uint32 rank, bool strict = false) const;

        // Spell Required table
        SpellRequiredMapBounds GetSpellsRequiredForSpellBounds(uint32 spell_id) const;
        SpellsRequiringSpellMapBounds GetSpellsRequiringSpellBounds(uint32 spell_id) const;
        bool IsSpellRequiringSpell(uint32 spellid, uint32 req_spellid) const;
        const SpellsRequiringSpellMap GetSpellsRequiringSpell();
        uint32 GetSpellRequired(uint32 spell_id) const;

        // Spell learning
        SpellLearnSkillNode const* GetSpellLearnSkill(uint32 spell_id) const;
        SpellLearnSpellMapBounds GetSpellLearnSpellMapBounds(uint32 spell_id) const;
        bool IsSpellLearnSpell(uint32 spell_id) const;
        bool IsSpellLearnToSpell(uint32 spell_id1, uint32 spell_id2) const;

        // Spell target coordinates
        SpellTargetPosition const* GetSpellTargetPosition(uint32 spell_id, SpellEffIndex effIndex) const;

        // Spell Groups table
        SpellSpellGroupMapBounds GetSpellSpellGroupMapBounds(uint32 spell_id) const;
        uint32 IsSpellMemberOfSpellGroup(uint32 spellid, SpellGroup groupid) const;

        SpellGroupSpellMapBounds GetSpellGroupSpellMapBounds(SpellGroup group_id) const;
        void GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells) const;
        void GetSetOfSpellsInSpellGroup(SpellGroup group_id, std::set<uint32>& foundSpells, std::set<SpellGroup>& usedGroups) const;

        // Spell Group Stack Rules table
        bool AddSameEffectStackRuleSpellGroups(SpellInfo const* spellInfo, int32 amount, std::map<SpellGroup, int32>& groups) const;
        SpellGroupStackRule CheckSpellGroupStackRules(SpellInfo const* spellInfo1, SpellInfo const* spellInfo2) const;

        // Spell proc event table
        SpellProcEventEntry const* GetSpellProcEvent(uint32 spellId) const;
        bool IsSpellProcEventCanTriggeredBy(SpellProcEventEntry const* spellProcEvent, uint32 EventProcFlag, SpellInfo const* procSpell, uint32 procFlags, uint32 procExtra, bool active, SpellInfo const* AuraInfo);

        // Spell proc table
        SpellProcEntry const* GetSpellProcEntry(uint32 spellId) const;
        bool CanSpellTriggerProcOnEvent(SpellProcEntry const& procEntry, ProcEventInfo& eventInfo);

        // Spell bonus data table
        SpellBonusEntry const* GetSpellBonusData(uint32 spellId) const;

        // Spell threat table
        SpellThreatEntry const* GetSpellThreatEntry(uint32 spellID) const;

        SkillLineAbilityMapBounds GetSkillLineAbilityMapBounds(uint32 spell_id) const;

        PetAura const* GetPetAura(uint32 spell_id, uint8 eff);

        SpellEnchantProcEntry const* GetSpellEnchantProcEvent(uint32 enchId) const;
        bool IsArenaAllowedEnchancment(uint32 ench_id) const;

        std::vector<SpellLinked>const* GetSpellLinked(int32 spell_id) const;
        SpellScene const* GetSpellScene(int32 miscValue) const;
        uint32 const* GetSpellSceneTrigger(uint32 p_MiscID) const;
        uint32 const* GetTriggeredSpellScene(uint32 p_SpellID) const;

        std::vector<SpellTriggered> const* GetSpellTriggered(int32 p_SpellID) const;
        std::vector<SpellDummyTrigger> const* GetSpellDummyTrigger(int32 p_SpellID) const;
        std::vector<SpellDummyTrigger> const* GetSpellAuraTrigger(int32 p_SpellID) const;
        std::vector<SpellAuraDummy> const* GetSpellAuraDummy(int32 p_SpellID) const;
        std::vector<SpellTargetFilter> const* GetSpellTargetFilter(int32 p_SpellID) const;
        std::vector<SpellCheckCast> const* GetSpellCheckCast(int32 p_SpellID) const;
        std::vector<SpellVisual> const* GetPlaySpellVisualData(int32 p_SpellID) const;
        std::vector<SpellVisualPlayOrphan> const* GetSpellVisualPlayOrphan(int32 p_SpellID) const;
        std::vector<SpellPendingCast> const* GetSpellPendingCast(int32 p_SpellID) const;
        uint32 const* GetRespecializationIDBySpell(uint32 p_SpellID) const;

        PetLevelupSpellSet const* GetPetLevelupSpellList(uint32 petFamily) const;
        PetDefaultSpellsEntry const* GetPetDefaultSpellsEntry(int32 id) const;
        SpellPowerEntry const* GetSpellPowerEntryByIdAndPower(uint32 id, Powers power) const;

        // Spell area
        SpellAreaMapBounds GetSpellAreaMapBounds(uint32 spell_id) const;
        SpellAreaForQuestMapBounds GetSpellAreaForQuestMapBounds(uint32 quest_id) const;
        SpellAreaForQuestMapBounds GetSpellAreaForQuestEndMapBounds(uint32 quest_id) const;
        SpellAreaForAuraMapBounds GetSpellAreaForAuraMapBounds(uint32 spell_id) const;
        SpellAreaForAreaMapBounds GetSpellAreaForAreaMapBounds(uint32 area_id) const;

        // SpellInfo object management
        SpellInfo const* GetSpellInfo(uint32 spellId, Difficulty difficulty = DifficultyNone) const;
        uint32 GetSpellInfoStoreSize() const { return mSpellInfoMap[DifficultyNone].size(); }
        std::set<uint32> GetSpellClassList(uint8 ClassID) const { return mSpellClassInfo[ClassID]; }
        std::list<uint32> GetSpellPowerList(uint32 spellId) const { return mSpellPowerInfo[spellId]; }
        TalentsPlaceHoldersSpell GetTalentPlaceHoldersSpell() const { return mPlaceHolderSpells; }
        const std::set<MinorTalentEntry const*>* GetSpecializationPerks(uint32 specializationId) const;

        uint32 GetSummoningSpellForEntry(uint32 p_Entry) { return (m_SummoningSpellForEntry.find(p_Entry) != m_SummoningSpellForEntry.end()) ? m_SummoningSpellForEntry[p_Entry] : 0; }
        static std::vector<uint32> const GetDireBeastSpells() {
            return { 121118, 122802, 122804, 122806, 122807, 122809, 122811, 126213, 126214, 126215, 126216, 132764, 138574, 138580, 139226, 139227,
                149365, 170357, 170358, 170359, 170360, 170361, 170362, 170363, 170364, 204397, 204409, 204410, 204411, 204412, 204413,
                204415, 204416, 204417, 204418, 204419, 204420, 204422, 204423, 204424, 204425, 204426, 204427, 204428, 204429, 204430, 204431, 204432,
                204433, 204434, 204480, 204494, 204507, 204512, 204514, 204516, 204518, 204520, 204521, 204522, 204523, 204524, 204525, 204526, 204527,
                204528, 204530, 204532, 204533, 204534, 204546, 204548, 204550, 204552, 204554, 204555, 204556, 204557, 204558, 204560, 204561, 204565,
                204568, 204569, 204571, 204572, 204583, 204584, 204585, 204614, 204619, 204621, 204622, 204623, 204627, 204629, 204630, 204632, 204633,
                204636, 212381, 212382, 212386, 212388, 212389, 212396, 212397, 219199, 224573, 224574, 224575, 224576, 224577, 224578, };} ///< Honor Talents Dire Beast Spells
        static std::unordered_set<uint32> const GetDireBeastEntries() {
            return { 62865,62855,62856,62857,62858,62860,62210,64617,64618,64619,64620,62005,69989,69990,69990,69989,74139,86187,86186,86653,86183,
                86181,86180,62858,86177,103244,103248,103249,103250,103251,103252,103253,103254,103255,103256,103257,103258,103259,103260,103261,103262,
                103263,103264,103265,103266,103267,103309,103268,103269,103270,103301,103311,103313,103314,103315,103316,103317,103318,103319,103320,
                103321,103323,103324,103327,103329,103331,103332,103333,103335,103336,103351,103352,103353,103354,103356,103357,103358,103359,103361,
                103364,103365,103366,103367,103368,103369,103370,103374,103375,103376,103377,103379,103382,103383,103385,103387,103390,103391,103392,
                103393,103394,106971,106972,106975,106977,106978,106988,106989,110063,128751,113342,113347,113343,113344,113346 };}

        bool IsTalent(uint32 spellId) { return mTalentSpellInfo.find(spellId) != mTalentSpellInfo.end() ?  true :  false; }

        // Item Upgrade datas
        uint16 GetDatasForILevel(uint16 iLevel) { return mItemUpgradeDatas.find(iLevel) != mItemUpgradeDatas.end() ? mItemUpgradeDatas[iLevel] : 0; }

        // Spell Difficulty
        uint32 GetDifficultyEntryForDataStore(std::string p_DbcName, uint32 p_SpellID, uint32 p_DifficultyID) const
        {
            DatastoreDifficultyKey::const_iterator l_Itr = mDatastoreSpellDifficultyKey.find(p_DbcName);
            if (l_Itr == mDatastoreSpellDifficultyKey.end())
                return 0;

            DataStoreMapPair::const_iterator l_Itr2 = l_Itr->second.find(std::make_pair(p_SpellID, p_DifficultyID));
            if (l_Itr2 == l_Itr->second.end())
                return 0;

            return l_Itr2->second;
        }

        uint32 GetBaseSpellDBCID(std::string p_DbcName, uint32 p_SpellID) const
        {
            SpellBaseDifficultyDBCs::const_iterator l_Itr = mSpellBaseDifficultyDBCs.find(p_DbcName);
            if (l_Itr == mSpellBaseDifficultyDBCs.end())
                return 0;

            DataStoreMapPair::const_iterator l_Itr2 = l_Itr->second.find(std::make_pair(p_SpellID, Difficulty::DifficultyNone));
            if (l_Itr2 == l_Itr->second.end())
                return 0;

            return l_Itr2->second;
        }

        std::vector<uint32> const* GetItemSourceSkills(uint32 p_ItemEntry) const
        {
            auto l_Itr = m_ItemSourceSkills.find(p_ItemEntry);
            if (l_Itr == m_ItemSourceSkills.end())
                return nullptr;

            return &l_Itr->second;
        }

        std::list<SkillLineAbilityEntry const*> const& GetTradeSpellFromSkill(uint32 p_Skill)
        {
            if (m_SkillTradeSpells.find(p_Skill) == m_SkillTradeSpells.end())
                m_SkillTradeSpells[p_Skill] = {};

            return m_SkillTradeSpells[p_Skill];
        }

        std::vector<SpellUpgradeItemStage> const& GetSpellUpgradeItemStage(uint32 p_ItemBonusTreeCategory)
        {
            return m_SpellUpgradeItemStages[p_ItemBonusTreeCategory];
        }

        std::unordered_map<uint32, SpellVisualMap> VisualsBySpellMap;

        uint32 GetSpellXVisualForSpellID(uint32 p_SpellId, uint32 p_Difficulty, uint32 p_Fallback)
        {
            auto l_It = VisualsBySpellMap.find(p_SpellId);
            if (l_It != VisualsBySpellMap.end())
            {
                auto l_SecondIt = l_It->second.find(p_Difficulty);

                if (l_SecondIt != l_It->second.end())
                    return l_SecondIt->second.at(0)->Id;
                else
                {
                    p_Difficulty = 0;
                    l_SecondIt = l_It->second.find(p_Difficulty);
                    if (l_SecondIt != l_It->second.end())
                        return l_SecondIt->second.at(0)->Id;
                }
            }

            return p_Fallback;
        }

    // Modifiers
    public:

        // Loading data at server startup
        void LoadSpellRanks();
        void LoadSpellScene();
        void LoadSpellRequired();
        void LoadSpellLearnSkills();
        void LoadSpellLearnSpells();
        void LoadSpellTargetPositions();
        void LoadSpellGroups();
        void LoadSpellGroupStackRules();
        void LoadSpellInfoOverride();
        void LoadAuraScripts(bool p_Reload);
        void LoadSpellScripts(bool p_Reload);
        void LoadSpellProcEvents();
        void LoadSpellProcs();
        void LoadSpellBonusess();
        void LoadSpellThreats();
        void LoadSkillLineAbilityMap();
        void LoadSpellPetAuras();
        void LoadEnchantCustomAttr();
        void LoadSpellEnchantProcData();
        void LoadSpellLinked();
        void LoadPetLevelupSpellMap();
        void LoadPetDefaultSpells();
        void LoadSpellAreas();
        void InitializeSpellDifficulty();
        void LoadSpellInfoStore();
        void LoadSpellClassInfo();
        void UnloadSpellInfoStore();
        void UnloadSpellInfoImplicitTargetConditionLists();
        void LoadSpellCustomAttr();
        void LoadTalentSpellInfo();
        void LoadSpellPowerInfo();
        void LoadForbiddenSpells();
        void InitializeItemUpgradeDatas();
        void LoadSpellPlaceHolder();
        void LoadSpellUpgradeItemStage();
        void LoadSpellTriggered();
        void LoadSpellVisual();
        void LoadSpellPendingCast();
        void LoadRespecializationSpells();
        bool HaveSameItemSourceSkill(Item* p_Item1, Item* p_Item2) const;

        void LoadChangeRecoveryRateSpells();
        std::set<int32> const& GetChangeRecoveryRateSpells(uint32 spellId) const;

        void LoadSpellDamageLog();

        bool IsInATDebug() const { return m_ATDebug; }
        void SetATDebug(bool p_Apply) { m_ATDebug = p_Apply; }

        bool IsInATPolygonDebug() const { return m_ATPolygonDebug; }
        void SetATPolygonDebug(bool p_Apply) { m_ATPolygonDebug = p_Apply; }

        bool IsInDRDebug() const { return m_DRDebug; }
        void SetDRDebug(bool p_Apply) { m_DRDebug = p_Apply; }

        bool IsInPvPTemplateDebug() const { return m_PvPTemplateDebug; }
        void SetPvPTemplateDebug(bool p_Apply) { m_PvPTemplateDebug = p_Apply; }

        bool GetSpellDamageLog(uint32 p_SpellId, uint32& p_MinDamageForLogging) const
        {
            auto l_Itr = m_SpellDamageLogMap.find(p_SpellId);
            if (l_Itr == m_SpellDamageLogMap.end())
                return false;

            p_MinDamageForLogging = l_Itr->second;
            return true;
        }

        bool IsFollowerAbilityFromSpell(uint32 p_AbilityID)
        {
            return (m_FollowerAbilitiesFromSpell.find(p_AbilityID) != m_FollowerAbilitiesFromSpell.end());
        }

        std::vector<uint32>        mSpellCreateItemList;
        AuraScriptMap              mAuraScriptMap;
        SpellScriptMap             mSpellScriptMap;

    private:
        SpellDifficultySearcherMap mSpellDifficultySearcherMap;
        SpellChainMap              mSpellChains;
        SpellsRequiringSpellMap    mSpellsReqSpell;
        SpellRequiredMap           mSpellReq;
        SpellLearnSkillMap         mSpellLearnSkills;
        SpellLearnSpellMap         mSpellLearnSpells;
        SpellTargetPositionMap     mSpellTargetPositions;
        SpellSpellGroupMap         mSpellSpellGroup;
        SpellGroupSpellMap         mSpellGroupSpell;
        SpellGroupStackMap         mSpellGroupStack;
        SpellInfoOverrideMap       mSpellInfoOverrideMap;
        SpellProcEventMap          mSpellProcEventMap;
        SpellProcMap               mSpellProcMap;
        SpellBonusMap              mSpellBonusMap;
        SpellThreatMap             mSpellThreatMap;
        SpellPetAuraMap            mSpellPetAuraMap;
        SpellLinkedMap             mSpellLinkedMap;
        SpellEnchantProcEventMap   mSpellEnchantProcEventMap;
        EnchantCustomAttribute     mEnchantCustomAttr;
        SpellAreaMap               mSpellAreaMap;
        SpellAreaForQuestMap       mSpellAreaForQuestMap;
        SpellAreaForQuestMap       mSpellAreaForActiveQuestMap;
        SpellAreaForQuestMap       mSpellAreaForQuestEndMap;
        SpellAreaForAuraMap        mSpellAreaForAuraMap;
        SpellAreaForAreaMap        mSpellAreaForAreaMap;
        SkillLineAbilityMap        mSkillLineAbilityMap;
        PetLevelupSpellMap         mPetLevelupSpellMap;
        PetDefaultSpellsMap        mPetDefaultSpellsMap;           // only spells not listed in related mPetLevelupSpellMap entry
        SpellInfoMap               mSpellInfoMap[Difficulty::MaxDifficulties];
        SpellClassList             mSpellClassInfo;
        SpecializatioPerkMap       mSpecializationPerks;
        TalentSpellSet             mTalentSpellInfo;
        SpellPowerVector           mSpellPowerInfo;
        std::list<uint32>          mForbiddenSpells;
        SpellSceneMap              mSpellSceneMap;
        SpellSceneTriggerMap       m_SpellSceneTriggerMap;
        TalentsPlaceHoldersSpell   mPlaceHolderSpells;
        ItemUpgradeDatas           mItemUpgradeDatas;
        AvaiableDifficultySpell    mAvaiableDifficultyBySpell;
        SpellBaseDifficultyDBCs    mSpellBaseDifficultyDBCs;
        DatastoreDifficultyKey     mDatastoreSpellDifficultyKey;
        ItemSourceSkills           m_ItemSourceSkills;
        TradeSpellSkills           m_SkillTradeSpells;
        SpellUpgradeItemStages     m_SpellUpgradeItemStages;
        SpellTriggeredMap          m_SpellTriggeredMap;
        SpellDummyTriggerMap       m_SpellDummyTriggerMap;
        SpellAuraTriggerMap        m_SpellAuraTriggerMap;
        SpellAuraDummyMap          m_SpellAuraDummyMap;
        SpellTargetFilterMap       m_SpellTargetFilterMap;
        SpellCheckCastMap          m_SpellCheckCastMap;
        PlaySpellVisualMap         m_SpellVisualMap;
        SpellVisualPlayOrphanMap   m_SpellVisualPlayOrphanMap;
        SpellPendingCastMap        m_SpellPendingCastMap;
        RespecializationSpellsMap  m_RespecializationSpellsMap;
        std::set<uint32>           m_FollowerAbilitiesFromSpell;

        std::map<uint32, uint32>   m_SummoningSpellForEntry;
        std::unordered_map<uint32, uint32> m_SpellDamageLogMap;

        std::map<uint32, std::set<int32> > m_ChangeRecoveryRateSpells;

        bool m_ATDebug = false;
        bool m_ATPolygonDebug = false;
        bool m_DRDebug = false;
        bool m_PvPTemplateDebug = false;
};

#define sSpellMgr ACE_Singleton<SpellMgr, ACE_Null_Mutex>::instance()

bool IsCCSpell(SpellInfo const* p_SpellProto);

#endif
