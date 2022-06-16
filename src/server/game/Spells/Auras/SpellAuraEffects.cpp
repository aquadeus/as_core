////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Unit.h"
#include "ObjectAccessor.h"
#include "Util.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "Battleground.h"
#include "OutdoorPvPMgr.h"
#include "Formulas.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "ScriptMgr.h"
#include "Vehicle.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "WeatherMgr.h"
#include "HelperDefines.h"
#include "RecruitAFriendMgr.hpp"

class Aura;
//
// EFFECT HANDLER NOTES
//
// in aura handler there should be check for modes:
// AURA_EFFECT_HANDLE_REAL set
// AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK set
// AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK set - aura is recalculated or is just applied/removed - need to redo all things related to m_amount
// AURA_EFFECT_HANDLE_CHANGE_AMOUNT_SEND_FOR_CLIENT_MASK - logical or of above conditions
// AURA_EFFECT_HANDLE_STAT - set when stats are reapplied
// such checks will speedup trinity change amount/send for client operations
// because for change amount operation packets will not be send
// aura effect handlers shouldn't contain any AuraEffect or Aura object modifications

pAuraEffectHandler AuraEffectHandler[TOTAL_AURAS]=
{
    &AuraEffect::HandleNULL,                                      //  0 SPELL_AURA_NONE
    &AuraEffect::HandleBindSight,                                 //  1 SPELL_AURA_BIND_SIGHT
    &AuraEffect::HandleModPossess,                                //  2 SPELL_AURA_MOD_POSSESS
    &AuraEffect::HandleNoImmediateEffect,                         //  3 SPELL_AURA_PERIODIC_DAMAGE implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleAuraDummy,                                 //  4 SPELL_AURA_DUMMY
    &AuraEffect::HandleModConfuse,                                //  5 SPELL_AURA_MOD_CONFUSE
    &AuraEffect::HandleModCharm,                                  //  6 SPELL_AURA_MOD_CHARM
    &AuraEffect::HandleModFear,                                   //  7 SPELL_AURA_MOD_FEAR
    &AuraEffect::HandleNoImmediateEffect,                         //  8 SPELL_AURA_PERIODIC_HEAL implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleModAttackSpeed,                            //  9 SPELL_AURA_MOD_ATTACKSPEED
    &AuraEffect::HandleModThreat,                                 // 10 SPELL_AURA_MOD_THREAT
    &AuraEffect::HandleModTaunt,                                  // 11 SPELL_AURA_MOD_TAUNT
    &AuraEffect::HandleAuraModStun,                               // 12 SPELL_AURA_MOD_STUN
    &AuraEffect::HandleModDamageDone,                             // 13 SPELL_AURA_MOD_DAMAGE_DONE
    &AuraEffect::HandleNoImmediateEffect,                         // 14 SPELL_AURA_MOD_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonus and Unit::SpellDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         // 15 SPELL_AURA_DAMAGE_SHIELD    implemented in Unit::DoAttackDamage
    &AuraEffect::HandleModStealth,                                // 16 SPELL_AURA_MOD_STEALTH
    &AuraEffect::HandleModStealthDetect,                          // 17 SPELL_AURA_MOD_DETECT
    &AuraEffect::HandleModInvisibility,                           // 18 SPELL_AURA_MOD_INVISIBILITY
    &AuraEffect::HandleModInvisibilityDetect,                     // 19 SPELL_AURA_MOD_INVISIBILITY_DETECTION
    &AuraEffect::HandleNoImmediateEffect,                         // 20 SPELL_AURA_OBS_MOD_HEALTH implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleNoImmediateEffect,                         // 21 SPELL_AURA_OBS_MOD_POWER implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleAuraModResistance,                         // 22 SPELL_AURA_MOD_RESISTANCE
    &AuraEffect::HandleNoImmediateEffect,                         // 23 SPELL_AURA_PERIODIC_TRIGGER_SPELL implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleNoImmediateEffect,                         // 24 SPELL_AURA_PERIODIC_ENERGIZE implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleAuraModPacify,                             // 25 SPELL_AURA_MOD_PACIFY
    &AuraEffect::HandleAuraModRoot,                               // 26 SPELL_AURA_MOD_ROOT
    &AuraEffect::HandleAuraModSilence,                            // 27 SPELL_AURA_MOD_SILENCE
    &AuraEffect::HandleNoImmediateEffect,                         // 28 SPELL_AURA_REFLECT_SPELLS        implement in Unit::SpellHitResult
    &AuraEffect::HandleAuraModStat,                               // 29 SPELL_AURA_MOD_STAT
    &AuraEffect::HandleAuraModSkill,                              // 30 SPELL_AURA_MOD_SKILL
    &AuraEffect::HandleAuraModIncreaseSpeed,                      // 31 SPELL_AURA_MOD_INCREASE_SPEED
    &AuraEffect::HandleAuraModIncreaseMountedSpeed,               // 32 SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED
    &AuraEffect::HandleAuraModDecreaseSpeed,                      // 33 SPELL_AURA_MOD_DECREASE_SPEED
    &AuraEffect::HandleAuraModIncreaseHealth,                     // 34 SPELL_AURA_MOD_INCREASE_HEALTH
    &AuraEffect::HandleAuraModIncreaseEnergy,                     // 35 SPELL_AURA_MOD_INCREASE_ENERGY
    &AuraEffect::HandleAuraModShapeshift,                         // 36 SPELL_AURA_MOD_SHAPESHIFT
    &AuraEffect::HandleAuraModEffectImmunity,                     // 37 SPELL_AURA_EFFECT_IMMUNITY
    &AuraEffect::HandleAuraModStateImmunity,                      // 38 SPELL_AURA_STATE_IMMUNITY
    &AuraEffect::HandleAuraModSchoolImmunity,                     // 39 SPELL_AURA_SCHOOL_IMMUNITY
    &AuraEffect::HandleAuraModDmgImmunity,                        // 40 SPELL_AURA_DAMAGE_IMMUNITY
    &AuraEffect::HandleAuraModDispelImmunity,                     // 41 SPELL_AURA_DISPEL_IMMUNITY
    &AuraEffect::HandleNoImmediateEffect,                         // 42 SPELL_AURA_PROC_TRIGGER_SPELL  implemented in Unit::ProcDamageAndSpellFor and Unit::HandleProcTriggerSpell
    &AuraEffect::HandleNoImmediateEffect,                         // 43 SPELL_AURA_PROC_TRIGGER_DAMAGE implemented in Unit::ProcDamageAndSpellFor
    &AuraEffect::HandleAuraTrackCreatures,                        // 44 SPELL_AURA_TRACK_CREATURES
    &AuraEffect::HandleAuraTrackResources,                        // 45 SPELL_AURA_TRACK_RESOURCES
    &AuraEffect::HandleNULL,                                      // 46 SPELL_AURA_46
    &AuraEffect::HandleAuraModParryPercent,                       // 47 SPELL_AURA_MOD_PARRY_PERCENT
    &AuraEffect::HandleNULL,                                      // 48 SPELL_AURA_48 spell Napalm (area damage spell with additional delayed damage effect)
    &AuraEffect::HandleAuraModDodgePercent,                       // 49 SPELL_AURA_MOD_DODGE_PERCENT
    &AuraEffect::HandleNoImmediateEffect,                         // 50 SPELL_AURA_MOD_CRITICAL_HEALING_AMOUNT implemented in Unit::SpellCriticalHealingBonus
    &AuraEffect::HandleAuraModBlockPercent,                       // 51 SPELL_AURA_MOD_BLOCK_PERCENT
    &AuraEffect::HandleAuraModWeaponCritPercent,                  // 52 SPELL_AURA_MOD_WEAPON_CRIT_PERCENT
    &AuraEffect::HandleNoImmediateEffect,                         // 53 SPELL_AURA_PERIODIC_LEECH implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleModHitChance,                              // 54 SPELL_AURA_MOD_HIT_CHANCE
    &AuraEffect::HandleModSpellHitChance,                         // 55 SPELL_AURA_MOD_SPELL_HIT_CHANCE
    &AuraEffect::HandleAuraTransform,                             // 56 SPELL_AURA_TRANSFORM
    &AuraEffect::HandleModSpellCritChance,                        // 57 SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    &AuraEffect::HandleAuraModIncreaseSwimSpeed,                  // 58 SPELL_AURA_MOD_INCREASE_SWIM_SPEED
    &AuraEffect::HandleNoImmediateEffect,                         // 59 SPELL_AURA_MOD_DAMAGE_DONE_CREATURE implemented in Unit::MeleeDamageBonus and Unit::SpellDamageBonus
    &AuraEffect::HandleAuraModPacifyAndSilence,                   // 60 SPELL_AURA_MOD_PACIFY_SILENCE
    &AuraEffect::HandleAuraModScale,                              // 61 SPELL_AURA_MOD_SCALE
    &AuraEffect::HandleNoImmediateEffect,                         // 62 SPELL_AURA_PERIODIC_HEALTH_FUNNEL implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleUnused,                                    // 63 unused (4.3.4) old SPELL_AURA_PERIODIC_MANA_FUNNEL
    &AuraEffect::HandleNoImmediateEffect,                         // 64 SPELL_AURA_PERIODIC_MANA_LEECH implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleModCastingSpeed,                           // 65 SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK
    &AuraEffect::HandleFeignDeath,                                // 66 SPELL_AURA_FEIGN_DEATH
    &AuraEffect::HandleAuraModDisarm,                             // 67 SPELL_AURA_MOD_DISARM
    &AuraEffect::HandleAuraModStalked,                            // 68 SPELL_AURA_MOD_STALKED
    &AuraEffect::HandleNoImmediateEffect,                         // 69 SPELL_AURA_SCHOOL_ABSORB implemented in Unit::CalcAbsorbResist
    &AuraEffect::HandleNoImmediateEffect,                         // 70 SPELL_AURA_PERIODIC_SCHOOL_DAMAGE
    &AuraEffect::HandleModSpellCritChanceSchool,                  // 71 SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    &AuraEffect::HandleModPowerCostPCT,                           // 72 SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT
    &AuraEffect::HandleModPowerCost,                              // 73 SPELL_AURA_MOD_POWER_COST_SCHOOL
    &AuraEffect::HandleNoImmediateEffect,                         // 74 SPELL_AURA_REFLECT_SPELLS_SCHOOL  implemented in Unit::SpellHitResult
    &AuraEffect::HandleNoImmediateEffect,                         // 75 SPELL_AURA_MOD_LANGUAGE
    &AuraEffect::HandleNoImmediateEffect,                         // 76 SPELL_AURA_FAR_SIGHT
    &AuraEffect::HandleModMechanicImmunity,                       // 77 SPELL_AURA_MECHANIC_IMMUNITY
    &AuraEffect::HandleAuraMounted,                               // 78 SPELL_AURA_MOUNTED
    &AuraEffect::HandleModDamagePercentDone,                      // 79 SPELL_AURA_MOD_DAMAGE_PERCENT_DONE
    &AuraEffect::HandleModPercentStat,                            // 80 SPELL_AURA_MOD_PERCENT_STAT
    &AuraEffect::HandleNoImmediateEffect,                         // 81 SPELL_AURA_SPLIT_DAMAGE_PCT implemented in Unit::CalcAbsorbResist
    &AuraEffect::HandleWaterBreathing,                            // 82 SPELL_AURA_WATER_BREATHING
    &AuraEffect::HandleModBaseResistance,                         // 83 SPELL_AURA_MOD_BASE_RESISTANCE
    &AuraEffect::HandleNoImmediateEffect,                         // 84 SPELL_AURA_MOD_REGEN implemented in Player::RegenerateHealth
    &AuraEffect::HandleModPowerRegen,                             // 85 SPELL_AURA_MOD_POWER_REGEN implemented in Player::Regenerate
    &AuraEffect::HandleChannelDeathItem,                          // 86 SPELL_AURA_CHANNEL_DEATH_ITEM
    &AuraEffect::HandleNoImmediateEffect,                         // 87 SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN implemented in Unit::MeleeDamageBonus and Unit::SpellDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         // 88 SPELL_AURA_MOD_HEALTH_REGEN_PERCENT implemented in Player::RegenerateHealth
    &AuraEffect::HandleNoImmediateEffect,                         // 89 SPELL_AURA_PERIODIC_DAMAGE_PERCENT
    &AuraEffect::HandleUnused,                                    // 90 unused (4.3.4) old SPELL_AURA_MOD_RESIST_CHANCE
    &AuraEffect::HandleNoImmediateEffect,                         // 91 SPELL_AURA_MOD_DETECT_RANGE implemented in Creature::GetAttackDistance
    &AuraEffect::HandlePreventFleeing,                            // 92 SPELL_AURA_PREVENTS_FLEEING
    &AuraEffect::HandleModUnattackable,                           // 93 SPELL_AURA_MOD_UNATTACKABLE
    &AuraEffect::HandleNoImmediateEffect,                         // 94 SPELL_AURA_INTERRUPT_REGEN implemented in Player::Regenerate
    &AuraEffect::HandleAuraGhost,                                 // 95 SPELL_AURA_GHOST
    &AuraEffect::HandleNoImmediateEffect,                         // 96 SPELL_AURA_SPELL_MAGNET implemented in Unit::SelectMagnetTarget
    &AuraEffect::HandleNoImmediateEffect,                         // 97 SPELL_AURA_MANA_SHIELD implemented in Unit::CalcAbsorbResist
    &AuraEffect::HandleAuraModSkill,                              // 98 SPELL_AURA_MOD_SKILL_TALENT
    &AuraEffect::HandleAuraModAttackPower,                        // 99 SPELL_AURA_MOD_ATTACK_POWER
    &AuraEffect::HandleUnused,                                    //100 SPELL_AURA_AURAS_VISIBLE obsolete? all player can see all auras now, but still have spells including GM-spell
    &AuraEffect::HandleModResistancePercent,                      //101 SPELL_AURA_MOD_RESISTANCE_PCT
    &AuraEffect::HandleNoImmediateEffect,                         //102 SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS implemented in Unit::MeleeDamageBonus
    &AuraEffect::HandleAuraModTotalThreat,                        //103 SPELL_AURA_MOD_TOTAL_THREAT
    &AuraEffect::HandleAuraWaterWalk,                             //104 SPELL_AURA_WATER_WALK
    &AuraEffect::HandleAuraFeatherFall,                           //105 SPELL_AURA_FEATHER_FALL
    &AuraEffect::HandleAuraHover,                                 //106 SPELL_AURA_HOVER
    &AuraEffect::HandleNoImmediateEffect,                         //107 SPELL_AURA_ADD_FLAT_MODIFIER implemented in AuraEffect::CalculateSpellMod()
    &AuraEffect::HandleNoImmediateEffect,                         //108 SPELL_AURA_ADD_PCT_MODIFIER implemented in AuraEffect::CalculateSpellMod()
    &AuraEffect::HandleNoImmediateEffect,                         //109 SPELL_AURA_ADD_TARGET_TRIGGER
    &AuraEffect::HandleModPowerRegenPCT,                          //110 SPELL_AURA_MOD_POWER_REGEN_PERCENT implemented in Player::Regenerate, Creature::Regenerate
    &AuraEffect::HandleNoImmediateEffect,                         //111 SPELL_AURA_ADD_CASTER_HIT_TRIGGER implemented in Unit::SelectMagnetTarget
    &AuraEffect::HandleNoImmediateEffect,                         //112 SPELL_AURA_OVERRIDE_CLASS_SCRIPTS
    &AuraEffect::HandleNoImmediateEffect,                         //113 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //114 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT implemented in Unit::MeleeDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //115 SPELL_AURA_MOD_HEALING                 implemented in Unit::SpellBaseHealingBonusForVictim
    &AuraEffect::HandleNoImmediateEffect,                         //116 SPELL_AURA_MOD_REGEN_DURING_COMBAT
    &AuraEffect::HandleNoImmediateEffect,                         //117 SPELL_AURA_MOD_MECHANIC_RESISTANCE     implemented in Unit::MagicSpellHitResult
    &AuraEffect::HandleNoImmediateEffect,                         //118 SPELL_AURA_MOD_HEALING_PCT             implemented in Unit::SpellHealingBonus
    &AuraEffect::HandleUnused,                                    //119 SPELL_AURA_PVP_TALENTS NYI
    &AuraEffect::HandleAuraUntrackable,                           //120 SPELL_AURA_UNTRACKABLE
    &AuraEffect::HandleAuraEmpathy,                               //121 SPELL_AURA_EMPATHY
    &AuraEffect::HandleModOffhandDamagePercent,                   //122 SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT
    &AuraEffect::HandleModTargetResistance,                       //123 SPELL_AURA_MOD_TARGET_RESISTANCE
    &AuraEffect::HandleAuraModRangedAttackPower,                  //124 SPELL_AURA_MOD_RANGED_ATTACK_POWER
    &AuraEffect::HandleNoImmediateEffect,                         //125 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //126 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT implemented in Unit::MeleeDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //127 SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS implemented in Unit::MeleeDamageBonus
    &AuraEffect::HandleFixate,                                    //128 SPELL_AURA_FIXATE
    &AuraEffect::HandleAuraModIncreaseSpeed,                      //129 SPELL_AURA_MOD_SPEED_ALWAYS
    &AuraEffect::HandleAuraModIncreaseMountedSpeed,               //130 SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS
    &AuraEffect::HandleNoImmediateEffect,                         //131 SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS implemented in Unit::MeleeDamageBonus
    &AuraEffect::HandleAuraModIncreaseEnergyPercent,              //132 SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT
    &AuraEffect::HandleAuraModIncreaseHealthPercent,              //133 SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT
    &AuraEffect::HandleAuraModRegenInterrupt,                     //134 SPELL_AURA_MOD_MANA_REGEN_INTERRUPT
    &AuraEffect::HandleModHealingDone,                            //135 SPELL_AURA_MOD_HEALING_DONE
    &AuraEffect::HandleNoImmediateEffect,                         //136 SPELL_AURA_MOD_HEALING_DONE_PERCENT   implemented in Unit::SpellHealingBonus
    &AuraEffect::HandleModTotalPercentStat,                       //137 SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE
    &AuraEffect::HandleModMeleeSpeedPct,                          //138 SPELL_AURA_MOD_MELEE_HASTE
    &AuraEffect::HandleForceReaction,                             //139 SPELL_AURA_FORCE_REACTION
    &AuraEffect::HandleAuraModRangedHaste,                        //140 SPELL_AURA_MOD_RANGED_HASTE
    &AuraEffect::HandleUnused,                                    //141 SPELL_AURA_141
    &AuraEffect::HandleAuraModBaseResistancePCT,                  //142 SPELL_AURA_MOD_BASE_RESISTANCE_PCT
    &AuraEffect::HandleModCooldownRecoveryRate,                   //143 SPELL_AURA_MOD_COOLDOWN_RECOVERY_RATE       implemented in Unit::AddSpellAndCategoryCooldown
    &AuraEffect::HandleNoImmediateEffect,                         //144 SPELL_AURA_SAFE_FALL                         implemented in WorldSession::HandleMovementOpcodes
    &AuraEffect::HandleAuraModPetTalentsPoints,                   //145 SPELL_AURA_MOD_PET_TALENT_POINTS
    &AuraEffect::HandleNoImmediateEffect,                         //146 SPELL_AURA_ALLOW_TAME_PET_TYPE
    &AuraEffect::HandleModStateImmunityMask,                      //147 SPELL_AURA_MECHANIC_IMMUNITY_MASK
    &AuraEffect::HandleAuraRetainComboPoints,                     //148 SPELL_AURA_RETAIN_COMBO_POINTS
    &AuraEffect::HandleNoImmediateEffect,                         //149 SPELL_AURA_REDUCE_PUSHBACK
    &AuraEffect::HandleShieldBlockValue,                          //150 SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT
    &AuraEffect::HandleAuraTrackStealthed,                        //151 SPELL_AURA_TRACK_STEALTHED
    &AuraEffect::HandleNoImmediateEffect,                         //152 SPELL_AURA_MOD_DETECTED_RANGE implemented in Creature::GetAttackDistance
    &AuraEffect::HandleUnused,                                    //153 Unused (4.3.4) old SPELL_AURA_SPLIT_DAMAGE_FLAT
    &AuraEffect::HandleModStealthLevel,                           //154 SPELL_AURA_MOD_STEALTH_LEVEL
    &AuraEffect::HandleNoImmediateEffect,                         //155 SPELL_AURA_MOD_WATER_BREATHING
    &AuraEffect::HandleNoImmediateEffect,                         //156 SPELL_AURA_MOD_REPUTATION_GAIN
    &AuraEffect::HandleNULL,                                      //157 unused 7.0.3
    &AuraEffect::HandleNoImmediateEffect,                         //158 SPELL_AURA_CAN_CHANGE_TALENT
    &AuraEffect::HandleNoImmediateEffect,                         //159 SPELL_AURA_NO_PVP_CREDIT      only for Honorless Target spell
    &AuraEffect::HandleUnused,                                    //160 Unused (4.3.4) old SPELL_AURA_MOD_AOE_AVOIDANCE
    &AuraEffect::HandleNoImmediateEffect,                         //161 SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT
    &AuraEffect::HandleNoImmediateEffect,                         //162 SPELL_AURA_POWER_BURN implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleNoImmediateEffect,                         //163 SPELL_AURA_MOD_CRIT_DAMAGE_BONUS implemented in Unit::MeleeCriticalDamageBonus and Unit and Unit::SpellCriticalDamageBonus
    &AuraEffect::HandleAuraDrowning,                              //164 SPELL_AURA_DROWNING
    &AuraEffect::HandleNoImmediateEffect,                         //165 SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS implemented in Unit::MeleeDamageBonus
    &AuraEffect::HandleAuraModAttackPowerPercent,                 //166 SPELL_AURA_MOD_ATTACK_POWER_PCT
    &AuraEffect::HandleAuraModRangedAttackPowerPercent,           //167 SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT
    &AuraEffect::HandleNoImmediateEffect,                         //168 SPELL_AURA_MOD_DAMAGE_DONE_VERSUS            implemented in Unit::SpellDamageBonus, Unit::MeleeDamageBonus
    &AuraEffect::HandleUnused,                                    //169 Unused (4.3.4) old SPELL_AURA_MOD_CRIT_PERCENT_VERSUS
    &AuraEffect::HandleNULL,                                      //170 SPELL_AURA_DETECT_AMORE       various spells that change visual of units for aura target (clientside?)
    &AuraEffect::HandleAuraModIncreaseSpeed,                      //171 SPELL_AURA_MOD_SPEED_NOT_STACK
    &AuraEffect::HandleAuraModIncreaseMountedSpeed,               //172 SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK
    &AuraEffect::HandleUnused,                                    //173 unused (4.3.4) no spells, old SPELL_AURA_ALLOW_CHAMPION_SPELLS  only for Proclaim Champion spell
    &AuraEffect::HandleModSpellDamagePercentFromStat,             //174 SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT  implemented in Unit::SpellBaseDamageBonus
    &AuraEffect::HandleModSpellHealingPercentFromStat,            //175 SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT implemented in Unit::SpellBaseHealingBonus
    &AuraEffect::HandleSpiritOfRedemption,                        //176 SPELL_AURA_SPIRIT_OF_REDEMPTION   only for Spirit of Redemption spell, die at aura end
    &AuraEffect::HandleCharmConvert,                              //177 SPELL_AURA_AOE_CHARM
    &AuraEffect::HandleAuraModMaxPower,                           //178 SPELL_AURA_MOD_MAX_POWER_PCT
    &AuraEffect::HandleNoImmediateEffect,                         //179 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE implemented in Unit::SpellCriticalBonus
    &AuraEffect::HandleNoImmediateEffect,                         //180 SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS   implemented in Unit::SpellDamageBonus
    &AuraEffect::HandleUnused,                                    //181 unused (4.3.4) old SPELL_AURA_MOD_FLAT_SPELL_CRIT_DAMAGE_VERSUS
    &AuraEffect::HandleAuraModResistenceOfStatPercent,            //182 SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT
    &AuraEffect::HandleNoImmediateEffect,                         //183 SPELL_AURA_MOD_SPELL_CRITICAL_CHANCE_TARGET_HEALTH_PCT implemented in Unit::GetUnitSpellCriticalChance
    &AuraEffect::HandleNoImmediateEffect,                         //184 SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE  implemented in Unit::RollMeleeOutcomeAgainst
    &AuraEffect::HandleNoImmediateEffect,                         //185 SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE implemented in Unit::RollMeleeOutcomeAgainst
    &AuraEffect::HandleNoImmediateEffect,                         //186 SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE  implemented in Unit::MagicSpellHitResult
    &AuraEffect::HandleNoImmediateEffect,                         //187 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE  implemented in Unit::GetUnitCriticalChance
    &AuraEffect::HandleNoImmediateEffect,                         //188 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE implemented in Unit::GetUnitCriticalChance
    &AuraEffect::HandleModRating,                                 //189 SPELL_AURA_MOD_RATING
    &AuraEffect::HandleNoImmediateEffect,                         //190 SPELL_AURA_MOD_FACTION_REPUTATION_GAIN     implemented in Player::CalculateReputationGain
    &AuraEffect::HandleAuraModUseNormalSpeed,                     //191 SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED
    &AuraEffect::HandleModMeleeRangedSpeedPct,                    //192 SPELL_AURA_MOD_MELEE_RANGED_HASTE
    &AuraEffect::HandleModCombatSpeedPct,                         //193 SPELL_AURA_MELEE_SLOW (in fact combat (any type attack) speed pct)
    &AuraEffect::HandleNoImmediateEffect,                         //194 SPELL_AURA_MOD_TARGET_ABSORB_SCHOOL implemented in Unit::CalcAbsorbResist
    &AuraEffect::HandleNoImmediateEffect,                         //195 SPELL_AURA_MOD_TARGET_ABILITY_ABSORB_SCHOOL implemented in Unit::CalcAbsorbResist
    &AuraEffect::HandleNoImmediateEffect,                         //196 SPELL_AURA_MOD_COOLDOWN - flat mod of spell cooldowns
    &AuraEffect::HandleNoImmediateEffect,                         //197 SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE implemented in Unit::SpellCriticalBonus Unit::GetUnitCriticalChance
    &AuraEffect::HandleUnused,                                    //198 unused (4.3.4) old SPELL_AURA_MOD_ALL_WEAPON_SKILLS
    &AuraEffect::HandleUnused,                                    //199 unused (4.3.4) old SPELL_AURA_MOD_INCREASES_SPELL_PCT_TO_HIT
    &AuraEffect::HandleNoImmediateEffect,                         //200 SPELL_AURA_MOD_XP_PCT implemented in Player::RewardPlayerAndGroupAtKill
    &AuraEffect::HandleAuraAllowFlight,                           //201 SPELL_AURA_FLY                             this aura enable flight mode...
    &AuraEffect::HandleNoImmediateEffect,                         //202 SPELL_AURA_CANNOT_BE_DODGED                implemented in Unit::RollPhysicalOutcomeAgainst
    &AuraEffect::HandleNoImmediateEffect,                         //203 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE  implemented in Unit::MeleeCriticalDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //204 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE implemented in Unit::MeleeCriticalDamageBonus
    &AuraEffect::HandleNULL,                                      //205 SPELL_AURA_MOD_SCHOOL_CRIT_DMG_TAKEN
    &AuraEffect::HandleAuraModIncreaseFlightSpeed,                //206 SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED
    &AuraEffect::HandleAuraModIncreaseFlightSpeed,                //207 SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED
    &AuraEffect::HandleAuraModIncreaseFlightSpeed,                //208 SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED
    &AuraEffect::HandleAuraModIncreaseFlightSpeed,                //209 SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS
    &AuraEffect::HandleAuraModIncreaseFlightSpeed,                //210 SPELL_AURA_MOD_VEHICLE_SPEED_ALWAYS
    &AuraEffect::HandleAuraModIncreaseFlightSpeed,                //211 SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK
    &AuraEffect::HandleNoImmediateEffect,                         //212 SPELL_AURA_MOD_REWARDED_HONOR_PCT
    &AuraEffect::HandleNoImmediateEffect,                         //213 SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT implemented in Player::RewardRage
    &AuraEffect::HandleNULL,                                      //214 Tamed Pet Passive
    &AuraEffect::HandleArenaPreparation,                          //215 SPELL_AURA_ARENA_PREPARATION
    &AuraEffect::HandleModCastingSpeed,                           //216 SPELL_AURA_HASTE_SPELLS
    &AuraEffect::HandleModMeleeSpeedPct,                          //217 SPELL_AURA_MOD_MELEE_HASTE_2
    &AuraEffect::HandleNULL,                                      //218 Affects item stats from equipment
    &AuraEffect::HandleModManaRegen,                              //219 SPELL_AURA_MOD_MANA_REGEN_FROM_STAT
    &AuraEffect::HandleNoImmediateEffect,                         //220 SPELL_AURA_CHANGE_DAMAGE_SCHOOL_MASK
    &AuraEffect::HandleNULL,                                      //221 SPELL_AURA_MOD_DETAUNT
    &AuraEffect::HandleUnused,                                    //222 unused (3.2.0)
    &AuraEffect::HandleNoImmediateEffect,                         //223 SPELL_AURA_RAID_PROC_FROM_CHARGE
    &AuraEffect::HandleAuraGainTalent,                            //224 SPELL_AURA_GAIN_TALENT (7.2.x ? )
    &AuraEffect::HandleAuraFogOfWar,                              //225 SPELL_AURA_FOG_OF_WAR implemented in WorldObject::canSeeOrDetect
    &AuraEffect::HandleNoImmediateEffect,                         //226 SPELL_AURA_PERIODIC_DUMMY implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleNoImmediateEffect,                         //227 SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleNoImmediateEffect,                         //228 SPELL_AURA_DETECT_STEALTH stealth detection
    &AuraEffect::HandleNoImmediateEffect,                         //229 SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE
    &AuraEffect::HandleAuraModIncreaseHealth,                     //230 SPELL_AURA_MOD_INCREASE_HEALTH_2
    &AuraEffect::HandleNoImmediateEffect,                         //231 SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE
    &AuraEffect::HandleNoImmediateEffect,                         //232 SPELL_AURA_MECHANIC_DURATION_MOD           implement in Unit::CalculateSpellDuration
    &AuraEffect::HandleUnused,                                    //233 set model id to the one of the creature with id GetMiscValue() - clientside
    &AuraEffect::HandleNoImmediateEffect,                         //234 SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK implement in Unit::CalculateSpellDuration
    &AuraEffect::HandleNoImmediateEffect,                         //235 SPELL_AURA_MOD_DISPEL_RESIST               implement in Unit::MagicSpellHitResult
    &AuraEffect::HandleAuraControlVehicle,                        //236 SPELL_AURA_CONTROL_VEHICLE
    &AuraEffect::HandleModSpellDamagePercentFromAttackPower,      //237 SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER  implemented in Unit::SpellBaseDamageBonus
    &AuraEffect::HandleModSpellHealingPercentFromAttackPower,     //238 SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER implemented in Unit::SpellBaseHealingBonus
    &AuraEffect::HandleAuraModScale,                              //239 SPELL_AURA_MOD_SCALE_2 only in Noggenfogger Elixir (16595) before 2.3.0 aura 61
    &AuraEffect::HandleAuraModExpertise,                          //240 SPELL_AURA_MOD_EXPERTISE
    &AuraEffect::HandleForceMoveForward,                          //241 SPELL_AURA_FORCE_MOVE_FORWARD Forces the caster to move forward
    &AuraEffect::HandleNULL,                                      //242 SPELL_AURA_MOD_SPELL_DAMAGE_FROM_HEALING - 2 test spells: 44183 and 44182
    &AuraEffect::HandleAuraModFaction,                            //243 SPELL_AURA_MOD_FACTION
    &AuraEffect::HandleComprehendLanguage,                        //244 SPELL_AURA_COMPREHEND_LANGUAGE
    &AuraEffect::HandleNoImmediateEffect,                         //245 SPELL_AURA_MOD_AURA_DURATION_BY_DISPEL
    &AuraEffect::HandleNoImmediateEffect,                         //246 SPELL_AURA_MOD_AURA_DURATION_BY_DISPEL_NOT_STACK implemented in Spell::EffectApplyAura
    &AuraEffect::HandleAuraCloneCaster,                           //247 SPELL_AURA_CLONE_CASTER
    &AuraEffect::HandleNoImmediateEffect,                         //248 SPELL_AURA_MOD_COMBAT_RESULT_CHANCE         implemented in Unit::RollMeleeOutcomeAgainst
    &AuraEffect::HandleAuraConvertRune,                           //249 SPELL_AURA_CONVERT_RUNE
    &AuraEffect::HandleAuraModIncreaseHealth,                     //250 SPELL_AURA_MOD_INCREASE_HEALTH_2
    &AuraEffect::HandleNoImmediateEffect,                         //251 SPELL_AURA_MOD_ENEMY_DODGE
    &AuraEffect::HandleModCombatSpeedPct,                         //252 SPELL_AURA_252 Is there any difference between this and SPELL_AURA_MELEE_SLOW ? maybe not stacking mod?
    &AuraEffect::HandleNoImmediateEffect,                         //253 SPELL_AURA_MOD_BLOCK_CRIT_CHANCE  implemented in Unit::isBlockCritical
    &AuraEffect::HandleAuraModDisarm,                             //254 SPELL_AURA_MOD_DISARM_OFFHAND
    &AuraEffect::HandleNoImmediateEffect,                         //255 SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT    implemented in Unit::SpellDamageBonus
    &AuraEffect::HandleNoReagentUseAura,                          //256 SPELL_AURA_NO_REAGENT_USE Use SpellClassMask for spell select
    &AuraEffect::HandleNULL,                                      //257 SPELL_AURA_MOD_TARGET_RESIST_BY_SPELL_CLASS Use SpellClassMask for spell select
    &AuraEffect::HandleNoImmediateEffect,                         //258 SPELL_AURA_OVERRIDE_SUMMONED_OBJECT implemented in Spell::EffectTransmitted
    &AuraEffect::HandleNoImmediateEffect,                         //259 SPELL_AURA_MOD_HOT_PCT implemented in Unit::SpellHealingBonusTaken
    &AuraEffect::HandleNoImmediateEffect,                         //260 SPELL_AURA_SCREEN_EFFECT (miscvalue = id in ScreenEffect.db2) not required any code
    &AuraEffect::HandlePhase,                                     //261 SPELL_AURA_PHASE
    &AuraEffect::HandleNoImmediateEffect,                         //262 SPELL_AURA_ABILITY_IGNORE_AURASTATE implemented in spell::cancast
    &AuraEffect::HandleAuraAllowOnlyAbility,                      //263 SPELL_AURA_ALLOW_ONLY_ABILITY player can use only abilities set in SpellClassMask
    &AuraEffect::HandleUnused,                                    //264 unused (3.2.0)
    &AuraEffect::HandleUnused,                                    //265 unused (4.3.4)
    &AuraEffect::HandleUnused,                                    //266 unused (4.3.4)
    &AuraEffect::HandleNoImmediateEffect,                         //267 SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL         implemented in Unit::IsImmunedToSpellEffect
    &AuraEffect::HandleUnused,                                    //268 unused (4.3.4) old SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT.
    &AuraEffect::HandleNoImmediateEffect,                         //269 SPELL_AURA_MOD_DAMAGE_TO_CASTER         implemented in Unit::SpellDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //270 SPELL_AURA_MOD_DAMAGE_FROM_CASTER_BY_SCHOOLMASK     implemented in Unit::SpellDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //271 SPELL_AURA_MOD_DAMAGE_FROM_CASTER       implemented in Unit::SpellDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //272 SPELL_AURA_IGNORE_MELEE_RESET
    &AuraEffect::HandleUnused,                                    //273 clientside
    &AuraEffect::HandleUnused,                                    //274 unused (4.3.4)
    &AuraEffect::HandleNoImmediateEffect,                         //275 SPELL_AURA_MOD_IGNORE_SHAPESHIFT Use SpellClassMask for spell select
    &AuraEffect::HandleNULL,                                      //276 mod damage % mechanic?
    &AuraEffect::HandleUnused,                                    //277 unused (4.3.4) old SPELL_AURA_MOD_MAX_AFFECTED_TARGETS
    &AuraEffect::HandleAuraModDisarm,                             //278 SPELL_AURA_MOD_DISARM_RANGED disarm ranged weapon
    &AuraEffect::HandleAuraInitializeImages,                      //279 SPELL_AURA_INITIALIZE_IMAGES
    &AuraEffect::HandleNoImmediateEffect,                         //280 SPELL_AURA_MOD_ARMOR_PENETRATION_PCT implemented in Unit::CalcArmorReducedDamage
    &AuraEffect::HandleNoImmediateEffect,                         //281 SPELL_AURA_MOD_GUID_REP_GAIN_PCT implemented in Player::RewardGuildReputation
    &AuraEffect::HandleAuraIncreaseBaseHealthPercent,             //282 SPELL_AURA_INCREASE_BASE_HEALTH_PERCENT
    &AuraEffect::HandleNoImmediateEffect,                         //283 SPELL_AURA_MOD_HEALING_RECEIVED       implemented in Unit::SpellHealingBonus
    &AuraEffect::HandleAuraLinked,                                //284 SPELL_AURA_LINKED
    &AuraEffect::HandleAuraModAttackPowerOfArmor,                 //285 SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR  implemented in Player::UpdateAttackPowerAndDamage
    &AuraEffect::HandleNoImmediateEffect,                         //286 SPELL_AURA_ABILITY_PERIODIC_CRIT implemented in AuraEffect::PeriodicTick
    &AuraEffect::HandleNoImmediateEffect,                         //287 SPELL_AURA_DEFLECT_SPELLS             implemented in Unit::MagicSpellHitResult and Unit::MeleeSpellHitResult
    &AuraEffect::HandleUnused,                                    //288 unused (7.x) was SPELL_AURA_IGNORE_HIT_DIRECTION
    &AuraEffect::HandleNoImmediateEffect,                         //289 SPELL_AURA_PREVENT_DURABILITY_LOSS
    &AuraEffect::HandleAuraModCritPct,                            //290 SPELL_AURA_MOD_CRIT_PCT
    &AuraEffect::HandleNoImmediateEffect,                         //291 SPELL_AURA_MOD_XP_QUEST_PCT  implemented in Player::RewardQuest
    &AuraEffect::HandleAuraOpenStable,                            //292 SPELL_AURA_OPEN_STABLE
    &AuraEffect::HandleAuraOverrideSpells,                        //293 SPELL_AURA_OVERRIDE_SPELLS auras which probably add set of abilities to their target based on it's miscvalue
    &AuraEffect::HandleNoImmediateEffect,                         //294 SPELL_AURA_PREVENT_REGENERATE_POWER implemented in Player::Regenerate(Powers power)
    &AuraEffect::HandleUnused,                                    //295 unused (4.3.4)
    &AuraEffect::HandleAuraSetVehicle,                            //296 SPELL_AURA_SET_VEHICLE_ID sets vehicle on target
    &AuraEffect::HandleNULL,                                      //297 Spirit Burst spells
    &AuraEffect::HandleAuraStrangulate,                           //298 70569 - Strangulating, maybe prevents talk or cast
    &AuraEffect::HandleUnused,                                    //299 unused (4.3.4)
    &AuraEffect::HandleNoImmediateEffect,                         //300 SPELL_AURA_SHARE_DAMAGE_PCT implemented in Unit::DealDamage
    &AuraEffect::HandleNoImmediateEffect,                         //301 SPELL_AURA_SCHOOL_HEAL_ABSORB implemented in Unit::CalcHealAbsorb
    &AuraEffect::HandleUnused,                                    //302 unused (4.3.4)
    &AuraEffect::HandleNoImmediateEffect,                         //303 SPELL_AURA_MOD_DAMAGE_DONE_VERSUS_AURASTATE implemented in Unit::SpellDamageBonus, Unit::MeleeDamageBonus
    &AuraEffect::HandleAuraModFakeInebriation,                    //304 SPELL_AURA_MOD_DRUNK
    &AuraEffect::HandleAuraModIncreaseSpeed,                      //305 SPELL_AURA_MOD_MINIMUM_SPEED
    &AuraEffect::HandleUnused,                                    //306 unused (4.3.4)
    &AuraEffect::HandleUnused,                                    //307 unused (4.3.4)
    &AuraEffect::HandleNULL,                                      //308 new aura for hunter traps
    &AuraEffect::HandleAuraModResiliencePct,                      //309 SPELL_AURA_MOD_RESILIENCE_PCT
    &AuraEffect::HandleNoImmediateEffect,                         //310 SPELL_AURA_MOD_CREATURE_AOE_DAMAGE_AVOIDANCE implemented in Spell::CalculateDamageDone
    &AuraEffect::HandleNULL,                                      //311 0 spells in 3.3.5
    &AuraEffect::HandleNULL,                                      //312 0 spells in 3.3.5
    &AuraEffect::HandleUnused,                                    //313 unused (4.3.4)
    &AuraEffect::HandlePreventResurrection,                       //314 SPELL_AURA_PREVENT_RESURRECTION todo
    &AuraEffect::HandleNoImmediateEffect,                         //315 SPELL_AURA_UNDERWATER_WALKING todo
    &AuraEffect::HandleNoImmediateEffect,                         //316 unused (4.3.4) old SPELL_AURA_PERIODIC_HASTE
    &AuraEffect::HandleAuraModSpellPowerPercent,                  //317 SPELL_AURA_MOD_SPELL_POWER_PCT
    &AuraEffect::HandleAuraMastery,                               //318 SPELL_AURA_MASTERY
    &AuraEffect::HandleModMeleeSpeedPct,                          //319 SPELL_AURA_MOD_MELEE_HASTE_3
    &AuraEffect::HandleAuraModRangedHaste,                        //320 SPELL_AURA_MOD_RANGED_HASTE_2
    &AuraEffect::HandleAuraModNoActions,                          //321 SPELL_AURA_MOD_NO_ACTIONS
    &AuraEffect::HandleNoImmediateEffect,                         //322 SPELL_AURA_INTERFERE_TARGETTING implemented in Spell::CheckCast
    &AuraEffect::HandleUnused,                                    //323 unused (4.3.4)
    &AuraEffect::HandleNULL,                                      //324 SPELL_AURA_324
    &AuraEffect::HandleUnused,                                    //325 unused (4.3.4)
    &AuraEffect::HandleNULL,                                      //326 SPELL_AURA_326
    &AuraEffect::HandleUnused,                                    //327 unused (4.3.4)
    &AuraEffect::HandleNoImmediateEffect,                         //328 SPELL_AURA_PROC_ON_POWER_AMOUNT implemented in Unit::HandleAuraProcOnPowerAmount
    &AuraEffect::HandleNoImmediateEffect,                         //329 SPELL_AURA_MOD_RUNE_REGEN_SPEED implemented in Spell::TakeRunePower
    &AuraEffect::HandleNoImmediateEffect,                         //330 SPELL_AURA_CAST_WHILE_WALKING
    &AuraEffect::HandleAuraForceWeather,                          //331 SPELL_AURA_FORCE_WEATHER
    &AuraEffect::HandleNoImmediateEffect,                         //332 SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS implemented in Unit::GetCastSpellInfo
    &AuraEffect::HandleNoImmediateEffect,                         //333 SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2 implemented in Unit::GetCastSpellInfo
    &AuraEffect::HandleNULL,                                      //334
    &AuraEffect::HandleNULL,                                      //335 SPELL_AURA_SEE_WHILE_INVISIBLE
    &AuraEffect::HandleNULL,                                      //336 SPELL_AURA_MOD_FLYING_RESTRICTIONS
    &AuraEffect::HandleNoImmediateEffect,                         //337 SPELL_AURA_MOD_VENDOR_ITEMS_PRICES
    &AuraEffect::HandleNoImmediateEffect,                         //338 SPELL_AURA_MOD_DURABILITY_LOSS
    &AuraEffect::HandleNULL,                                      //339
    &AuraEffect::HandleNULL,                                      //340 SPELL_AURA_SPELL_AURA_MOD_RESURRECTED_HEALTH_BY_GUILD_MEMBER
    &AuraEffect::HandleModCategoryCooldown,                       //341 SPELL_AURA_MOD_SPELL_CATEGORY_COOLDOWN
    &AuraEffect::HandleModMeleeRangedSpeedPct,                    //342 SPELL_AURA_MOD_MELEE_RANGED_HASTE_2
    &AuraEffect::HandleNoImmediateEffect,                         //343 SPELL_AURA_MOD_AUTOATTACK_DAMAGE_TARGET implemented in Unit::MeleeDamageBonusTaken
    &AuraEffect::HandleModAutoAttackDamage,                       //344 SPELL_AURA_MOD_AUTOATTACK_DAMAGE implemented in Unit::MeleeDamageBonusTaken
    &AuraEffect::HandleNoImmediateEffect,                         //345 SPELL_AURA_BYPASS_ARMOR_FOR_CASTER
    &AuraEffect::HandleEnableAltPower,                            //346 SPELL_AURA_ENABLE_ALT_POWER
    &AuraEffect::HandleUnused,                                    //347 Unused (7.0.3)
    &AuraEffect::HandleNoImmediateEffect,                         //348 SPELL_AURA_AURA_DEPOSIT_BONUS_MONEY_IN_GUILD_BANK_ON_LOOT implemented in WorldSession::HandleLootMoneyOpcode
    &AuraEffect::HandleNoImmediateEffect,                         //349 SPELL_AURA_MOD_CURRENCY_GAIN implemented in Player::ModifyCurrency (TODO?)
    &AuraEffect::HandleNULL,                                      //350
    &AuraEffect::HandleNULL,                                      //351 SPELL_AURA_351
    &AuraEffect::HandleNULL,                                      //352 SPELL_AURA_352
    &AuraEffect::HandleModCamouflage,                             //353 SPELL_AURA_MOD_CAMOUFLAGE
    &AuraEffect::HandleNULL,                                      //354 SPELL_AURA_354
    &AuraEffect::HandleNoImmediateEffect,                         //355 SPELL_AURA_MOD_CAST_SPEED           implemented in Unit::ModSpellCastTime
    &AuraEffect::HandleNULL,                                      //356 SPELL_AURA_356
    &AuraEffect::HandleAuraEnableBossUnitFrame,                   //357 SPELL_AURA_ENABLE_BOSS1_UNIT_FRAME
    &AuraEffect::HandleNoImmediateEffect,                         //358 SPELL_AURA_WORGEN_ALTERED_FORM clientside
    &AuraEffect::HandleNULL,                                      //359 SPELL_AURA_359
    &AuraEffect::HandleNULL,                                      //360 SPELL_AURA_360
    &AuraEffect::HandleNoImmediateEffect,                         //361 SPELL_AURA_PROC_MELEE_TRIGGER_SPELL
    &AuraEffect::HandleUnused,                                    //362 unused (4.3.4)
    &AuraEffect::HandleNULL,                                      //363 SPELL_AURA_MOD_NEXT_SPELL
    &AuraEffect::HandleUnused,                                    //364 unused (4.3.4)
    &AuraEffect::HandleNULL,                                      //365 SPELL_AURA_365
    &AuraEffect::HandleOverrideSpellPowerByAttackPower,           //366 SPELL_AURA_OVERRIDE_SPELL_POWER_BY_AP_PCT  implemented in Unit::SpellBaseDamageBonus
    &AuraEffect::HandleNoImmediateEffect,                         //367 SPELL_AURA_OVERRIDE_AUTO_ATTACKS_BY_SPELL implemented in Player::Update
    &AuraEffect::HandleUnused,                                    //368 unused (4.3.4)
    &AuraEffect::HandleNULL,                                      //369 SPELL_SPELL_AURA_ENABLE_POWER_BAR_TIMER
    &AuraEffect::HandleNULL,                                      //370 SPELL_AURA_SET_FAIR_FAR_CLIP
    &AuraEffect::HandleNULL,                                      //371 SPELL_AURA_372
    &AuraEffect::HandleAuraMountedVisual,                         //372 SPELL_AURA_MOUNTED_VISUAL
    &AuraEffect::HandleAuraModIncreaseSpeed,                      //373 SPELL_AURA_INCREASE_MIN_SWIM_SPEED
    &AuraEffect::HandleNoImmediateEffect,                         //374 SPELL_AURA_REDUCE_FALL_DAMAGE_PERCENT implemente in Player::EnvironmentalDamage
    &AuraEffect::HandleNULL,                                      //375 SPELL_AURA_375
    &AuraEffect::HandleNoImmediateEffect,                         //376 SPELL_AURA_MOD_CURRENCY_GAIN_2  implemented in Player::ModifyCurrency
    &AuraEffect::HandleNoImmediateEffect,                         //377 SPELL_AURA_ALLOW_ALL_CASTS_WHILE_WALKING
    &AuraEffect::HandleNULL,                                      //378 SPELL_AURA_378
    &AuraEffect::HandleAuraModifyManaRegenFromManaPct,            //379 SPELL_AURA_MODIFY_MANA_REGEN_FROM_MANA_PCT
    &AuraEffect::HandleNULL,                                      //380 SPELL_AURA_380
    &AuraEffect::HandleNoImmediateEffect,                         //381 SPELL_AURA_INCREASE_HEALTH_FROM_OWNER
    &AuraEffect::HandleAuraModPetStats,                           //382 SPELL_AURA_MOD_PET_STATS
    &AuraEffect::HandleNoImmediateEffect,                         //383 SPELL_AURA_ALLOW_CAST_WHILE_IN_COOLDOWN
    &AuraEffect::HandleNULL,                                      //384 SPELL_AURA_384
    &AuraEffect::HandleNoImmediateEffect,                         //385 SPELL_AURA_STRIKE_SELF in Unit::AttackerStateUpdate
    &AuraEffect::HandleNoImmediateEffect,                         //386 SPELL_AURA_INCREASE_REST_BONUS_PERCENT implemented in Rest gain calculation
    &AuraEffect::HandleNoImmediateEffect,                         //387 SPELL_AURA_REDUCE_ITEM_MODIFY_COST implemented in Transmogrification and Void Chamber handlers
    &AuraEffect::HandleNoImmediateEffect,                         //388 SPELL_AURA_MOD_TAXI_FLIGHT_SPEED in FlightPathMovementGenerator::DoReset
    &AuraEffect::HandleNoImmediateEffect,                         //389 SPELL_AURA_MOD_CAST_TIME_WHILE_MOVING in SpellInfo::AttackerStateUpdate
    &AuraEffect::HandleNULL,                                      //390 SPELL_AURA_390
    &AuraEffect::HandleNULL,                                      //391 SPELL_AURA_391
    &AuraEffect::HandleNULL,                                      //392 SPELL_AURA_392
    &AuraEffect::HandleNoImmediateEffect,                         //393 SPELL_AURA_DEFLECT_FRONT_SPELLS
    &AuraEffect::HandleBonusLoot,                                 //394 SPELL_AURA_TRIGGER_BONUS_LOOT (NYI)
    &AuraEffect::HandleAreaTrigger,                               //395 SPELL_AURA_AREATRIGGER
    &AuraEffect::HandleNoImmediateEffect,                         //396 SPELL_AURA_TRIGGER_ON_MODIFY_POWER implemented in Unit::SetPower
    &AuraEffect::HandleBattlegroundFlag,                          //397 SPELL_AURA_BATTLEGROUND_FLAG
    &AuraEffect::HandleBattlegroundFlag,                          //398 SPELL_AURA_BATTLEGROUND_FLAG_2
    &AuraEffect::HandleModTimeRate,                               //399 SPELL_AURA_MOD_TIME_RATE
    &AuraEffect::HandleAuraModSkillValue,                         //400 SPELL_AURA_MOD_SKILL_VALUE
    &AuraEffect::HandleNULL,                                      //401 SPELL_AURA_401
    &AuraEffect::HandleAuraSetPowerType,                          //402 SPELL_AURA_SET_POWER_TYPE
    &AuraEffect::HandleChangeSpellVisualEffect,                   //403 SPELL_AURA_CHANGE_VISUAL_EFFECT
    &AuraEffect::HandleOverrideAttackPowerBySpellPower,           //404 SPELL_AURA_OVERRIDE_AP_BY_SPELL_POWER_PCT
    &AuraEffect::HandleIncreaseRatingPct,                         //405 SPELL_AURA_INCREASE_RATING_PCT
    &AuraEffect::HandleNULL,                                      //406 SPELL_AURA_406
    &AuraEffect::HandleModFear,                                   //407 SPELL_AURA_MOD_FEAR_2      TODO : Find the difference between 7 & 407
    &AuraEffect::HandleNoImmediateEffect,                         //408 SPELL_AURA_ADD_VIRTUAL_CHARGE partially implemented in Player::RemoveSpellMods (needs a part of scripting)
    &AuraEffect::HandleAllowMoveWhileFalling,                     //409 SPELL_AURA_ALLOW_MOVE_WHILE_FALLING
    &AuraEffect::HandleNoImmediateEffect,                         //410 SPELL_AURA_STAMPEDE_ONLY_CURRENT_PET implemented in EffectSummonMultipleHunterPets
    &AuraEffect::HandleNoImmediateEffect,                         //411 SPELL_AURA_MOD_MAX_CHARGES implemented in Player::CalcMaxCharges
    &AuraEffect::HandleModManaRegenByHaste,                       //412 SPELL_AURA_MOD_MANA_REGEN_BY_HASTE
    &AuraEffect::HandleAuraModParryPercent,                       //413 SPELL_AURA_MOD_PARRY_PERCENT
    &AuraEffect::HandleNULL,                                      //414 SPELL_AURA_MOD_DEFLECT_RANGED_ATTACKS
    &AuraEffect::HandleNULL,                                      //415 SPELL_AURA_415
    &AuraEffect::HandleNoImmediateEffect,                         //416 SPELL_AURA_MOD_COOLDOWN_BY_HASTE
    &AuraEffect::HandleNoImmediateEffect,                         //417 SPELL_AURA_MOD_GLOBAL_COOLDOWN_BY_HASTE
    &AuraEffect::HandleAuraModMaxPower,                           //418 SPELL_AURA_MOD_MAX_POWER
    &AuraEffect::HandleAuraModifyManaPoolPct,                     //419 SPELL_AURA_MODIFY_MANA_POOL_PCT
    &AuraEffect::HandleNULL,                                      //420 SPELL_AURA_420
    &AuraEffect::HandleNoImmediateEffect,                         //421 SPELL_AURA_MOD_ABSORPTION_PCT implemented in Unit
    &AuraEffect::HandleNULL,                                      //422 SPELL_AURA_422
    &AuraEffect::HandleNULL,                                      //423 SPELL_AURA_423
    &AuraEffect::HandleNULL,                                      //424 SPELL_AURA_424
    &AuraEffect::HandleNULL,                                      //425 SPELL_AURA_425
    &AuraEffect::HandleNULL,                                      //426 SPELL_AURA_426
    &AuraEffect::HandleNULL,                                      //427 SPELL_AURA_427
    &AuraEffect::HandleAuraLinked,                                //428 SPELL_AURA_LINKED_2
    &AuraEffect::HandleNoImmediateEffect,                         //429 SPELL_AURA_MOD_PET_DAMAGE_DONE
    &AuraEffect::HandleAuraActivateSpellScene,                    //430 SPELL_AURA_ACTIVATE_SCENE
    &AuraEffect::HandleAuraContestedPvp,                          //431 SPELL_AURA_CONTESTED_PVP
    &AuraEffect::HandleNULL,                                      //432 SPELL_AURA_432
    &AuraEffect::HandleNULL,                                      //433 SPELL_AURA_433
    &AuraEffect::HandleNULL,                                      //434 SPELL_AURA_434
    &AuraEffect::HandleNULL,                                      //435 SPELL_AURA_435
    &AuraEffect::HandleNULL,                                      //436 SPELL_AURA_436
    &AuraEffect::HandleAuraAllowFlight,                           //437 SPELL_AURA_USE_FLIGHT_MODE
    &AuraEffect::HandleNULL,                                      //438 SPELL_AURA_438
    &AuraEffect::HandleNULL,                                      //439 SPELL_AURA_439
    &AuraEffect::HandleNULL,                                      //440 SPELL_AURA_440 - Old SPELL_AURA_MOD_MULTISTRIKE_EFFECT_PCT (6.2.3)
    &AuraEffect::HandleNULL,                                      //441 SPELL_AURA_441 - Old SPELL_AURA_MOD_MULTISTRIKE_PCT (6.2.3)
    &AuraEffect::HandleNULL,                                      //442 SPELL_AURA_442
    &AuraEffect::HandleAuraLeech,                                 //443 SPELL_AURA_MOD_LEECH_PCT
    &AuraEffect::HandleNULL,                                      //444 SPELL_AURA_444
    &AuraEffect::HandleNULL,                                      //445 SPELL_AURA_445
    &AuraEffect::HandleNULL,                                      //446 SPELL_AURA_446
    &AuraEffect::HandleNoImmediateEffect,                         //447 SPELL_AURA_MOD_XP_FROM_CREATURE_TYPE
    &AuraEffect::HandleNULL,                                      //448 SPELL_AURA_MOD_CRITICAL_HEAl_GAIN
    &AuraEffect::HandleNULL,                                      //449 SPELL_AURA_UPGRADE_CHARACTER_SPELL_TIER
    &AuraEffect::HandleNULL,                                      //450 SPELL_AURA_450
    &AuraEffect::HandleAuraAdaptation,                            //451 SPELL_AURA_OVERRIDE_PET_SPECS
    &AuraEffect::HandleNULL,                                      //452 SPELL_AURA_452
    &AuraEffect::HandleNoImmediateEffect,                         //453 SPELL_AURA_CHARGE_RECOVERY_MOD implemented in Player::GetChargeRecoveryTime
    &AuraEffect::HandleNoImmediateEffect,                         //454 SPELL_AURA_CHARGE_RECOVERY_MULTIPLIER implemented in Player::GetChargeRecoveryTime
    &AuraEffect::HandleAuraModRoot,                               //455 SPELL_AURA_MOD_ROOT_2
    &AuraEffect::HandleNoImmediateEffect,                         //456 SPELL_AURA_CHARGE_RECOVERY_AFFECTED_BY_HASTE implemented in Player::GetChargeRecoveryTime
    &AuraEffect::HandleNoImmediateEffect,                         //457 SPELL_AURA_CHARGE_RECOVERY_AFFECTED_BY_HASTE_REGEN implemented in Player::GetChargeRecoveryTime
    &AuraEffect::HandleAuraIncreaseDualWieldDamage,               //458 SPELL_AURA_INCREASE_DUAL_WIELD_DAMAGE
    &AuraEffect::HandleDisableMovementForce,                      //459 SPELL_AURA_DISABLE_MOVEMENT_FORCE
    &AuraEffect::HandleNoImmediateEffect,                         //460 SPELL_AURA_RESET_COOLDOWNS_BEFORE_DUEL
    &AuraEffect::HandleNULL,                                      //461 SPELL_AURA_461
    &AuraEffect::HandleNULL,                                      //462 SPELL_AURA_MOD_HEALING_AND_ABSORB_FROM_CASTER
    &AuraEffect::HandleAuraAddParryPCTOfCSFromGear,               //463 SPELL_AURA_CONVERT_CRIT_RATING_PCT_TO_PARRY_RATING
    &AuraEffect::HandleNoImmediateEffect,                         //464 SPELL_AURA_MOD_AP_FROM_BONUS_ARMOR_PCT
    &AuraEffect::HandleAuraBonusArmor,                            //465 SPELL_AURA_MOD_BONUS_ARMOR
    &AuraEffect::HandleAuraBonusArmor,                            //466 SPELL_AURA_MOD_BONUS_ARMOR_PCT
    &AuraEffect::HandleModStatBonusPercent,                       //467 SPELL_AURA_MOD_STAT_BONUS_PCT
    &AuraEffect::HandleNoImmediateEffect,                         //468 SPELL_AURA_TRIGGER_AT_HEALTH_PCT
    &AuraEffect::HandleNoImmediateEffect,                         //469 SPELL_AURA_TRIGGER_BONUS_LOOT_2
    &AuraEffect::HandleModDebuffSpeed,                            //470 SPELL_AURA_MOD_DEBUFF_SPEED
    &AuraEffect::HandleAuraVersatility,                           //471 SPELL_AURA_MOD_VERSATILITY_PCT
    &AuraEffect::HandleFixate,                                    //472 SPELL_AURA_FIXATE2
    &AuraEffect::HandleNoImmediateEffect,                         //473 SPELL_AURA_PREVENT_DURABILITY_LOSS_FROM_COMBAT
    &AuraEffect::HandleNULL,                                      //474 SPELL_AURA_EFFECT_INCREASE_WQ_REWARD_PROC
    &AuraEffect::HandleAllowUsingGameobjectsWhileMounted,         //475 SPELL_AURA_ALLOW_USING_GAMEOBJECTS_WHILE_MOUNTED
    &AuraEffect::HandleNoImmediateEffect,                         //476 SPELL_AURA_MOD_CURRENCY_GAIN_PCT
    &AuraEffect::HandleNULL,                                      //477 SPELL_AURA_477
    &AuraEffect::HandleNULL,                                      //478 SPELL_AURA_478
    &AuraEffect::HandleNULL,                                      //479 SPELL_AURA_479
    &AuraEffect::HandleNULL,                                      //480 SPELL_AURA_480
    &AuraEffect::HandleNoImmediateEffect,                         //481 SPELL_AURA_CONVERT_CONSUMED_RUNE implemented in Spell::TakeRunePower
    &AuraEffect::HandleNULL,                                      //482 SPELL_AURA_482
    &AuraEffect::HandleNULL,                                      //483 SPELL_AURA_483
    &AuraEffect::HandleNULL,                                      //484 SPELL_AURA_484
    &AuraEffect::HandleAuraModMovementForceSpeed,                 //485 SPELL_AURA_MOD_MOVEMENT_FORCE_SPEED
    &AuraEffect::HandleNULL,                                      //486 SPELL_AURA_486
    &AuraEffect::HandleNULL,                                      //487 SPELL_AURA_487
    &AuraEffect::HandleNULL,                                      //488 SPELL_AURA_488
    &AuraEffect::HandleNULL,                                      //489 SPELL_AURA_489
    &AuraEffect::HandleSwitchTeam,                                //490 SPELL_AURA_SWITCH_TEAM used for mercenary
    &AuraEffect::HandleNoImmediateEffect,                         //491 SPELL_AURA_MOD_REWARDED_HONOR_PCT_2
};

AuraEffect::AuraEffect(Aura* base, uint8 effIndex, int32 *baseAmount, Unit* /*caster*/):
m_base(base), m_spellInfo(base->GetSpellInfo()), m_baseAmount(baseAmount ? *baseAmount : m_spellInfo->Effects[effIndex].BasePoints),
m_amount(0), m_floatAmount(0.0f), m_donePct(1.0f), m_CrowdControlDamage(0), m_spellmod(NULL),
m_periodicTimer(0), m_tickNumber(0), m_AmplitudeForced(false), m_effIndex(effIndex), m_canBeRecalculated(true), m_isPeriodic(false), m_PeriodicHitDamage(0), m_PeriodicAmount(0)
{
}

AuraEffect::~AuraEffect()
{
    delete m_spellmod;
}

float AuraEffect::GetCalculatedAmountForClient_First()
{
    return GetAmount();
}

float AuraEffect::GetCalculatedAmountForClient_Second()
{
    switch (GetAuraType())
    {
        case SPELL_AURA_MOD_HEALING_PCT:
        case SPELL_AURA_SCHOOL_ABSORB:
        case SPELL_AURA_ADD_FLAT_MODIFIER:
        case SPELL_AURA_ADD_PCT_MODIFIER:
        case SPELL_AURA_BYPASS_ARMOR_FOR_CASTER:
        case SPELL_AURA_PERIODIC_HEAL:
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_SCHOOL_DAMAGE:
        case SPELL_AURA_MOD_INCREASE_SPEED:
        case SPELL_AURA_FOG_OF_WAR:
        case SPELL_AURA_MOD_DAMAGE_PERCENT_DONE:
        case SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN:
        case SPELL_AURA_MOD_SPELL_CRIT_CHANCE:
        case SPELL_AURA_MOD_AURA_DURATION_BY_DISPEL:
        case SPELL_AURA_MOD_RATING:
        case SPELL_AURA_OBS_MOD_POWER:
        case SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK:
        case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2:
        case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS:
        case SPELL_AURA_MOUNTED:
            return GetAmount();
        case SPELL_AURA_DUMMY:
        {
            switch (GetBase()->GetId())
            {
                case 132639:
                    return GetAmount();
                default:
                    break;
            }

            break;
        }
        default:
            break;
    }

    return 0.0f;
}

void AuraEffect::GetTargetList(std::list<Unit*> & targetList) const
{
    Aura::ApplicationMap const & targetMap = GetBase()->GetApplicationMap();
    // remove all targets which were not added to new list - they no longer deserve area aura
    for (Aura::ApplicationMap::const_iterator appIter = targetMap.begin(); appIter != targetMap.end(); ++appIter)
    {
        if (appIter->second->HasEffect(GetEffIndex()))
            targetList.push_back(appIter->second->GetTarget());
    }
}

void AuraEffect::GetApplicationList(std::list<AuraApplication*> & applicationList) const
{
    Aura::ApplicationMap const & targetMap = GetBase()->GetApplicationMap();
    for (Aura::ApplicationMap::const_iterator appIter = targetMap.begin(); appIter != targetMap.end(); ++appIter)
    {
        if (appIter->second->HasEffect(GetEffIndex()))
            applicationList.push_back(appIter->second);
    }
}

int32 AuraEffect::CalculateAmount(Unit* caster)
{
    int32 amount;

    Item* l_Item = nullptr;
    if (uint64 itemGUID = GetBase()->GetCastItemGUID())
    {
        if (caster)
        {
            if (Player* l_PlayerCaster = caster->ToPlayer())
                l_Item = l_PlayerCaster->GetItemByGuid(itemGUID);
        }
    }

    // default amount calculation
    amount = m_spellInfo->Effects[m_effIndex].CalcValue(caster, &m_baseAmount, GetBase()->GetOwner()->ToUnit(), l_Item);

    // check item enchant aura cast
    if (!amount && caster)
    {
        if (l_Item != nullptr)
        {
            if (l_Item->GetItemSuffixFactor())
            {
                ItemRandomSuffixEntry const* item_rand_suffix = sItemRandomSuffixStore.LookupEntry(abs(l_Item->GetItemRandomPropertyId()));
                if (item_rand_suffix)
                {
                    for (int k = 0; k < MAX_ENCHANTMENT_SPELLS; k++)
                    {
                        SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(item_rand_suffix->Enchantment[k]);
                        if (pEnchant)
                        {
                            for (int t = 0; t < MAX_ENCHANTMENT_SPELLS; t++)
                            {
                                if (pEnchant->spellid[t] == m_spellInfo->Id)
                                {
                                    amount = uint32((item_rand_suffix->AllocationPct[k] * l_Item->GetItemSuffixFactor()) / 10000);
                                    break;
                                }
                            }
                        }

                        if (amount)
                            break;
                    }
                }
            }
        }
    }

    float DoneActualBenefit = 0.0f;

    bool l_IsCrowControlAura = false;

    switch (GetAuraType())
    {
        case SPELL_AURA_MOD_RATING:
        {
            // Heart's Judgment, Heart of Ignacious trinket (Heroic)
            if (m_spellInfo->Id == 92328 && caster)
            {
                if (Aura* pAura = caster->GetAura(92325))
                    amount *= pAura->GetStackAmount();
            }
            break;
        }

        /// crowd control auras
        case SPELL_AURA_MOD_FEAR:
        case SPELL_AURA_MOD_STUN:
        case SPELL_AURA_MOD_ROOT:
        case SPELL_AURA_MOD_ROOT_2:
        case SPELL_AURA_TRANSFORM:
        case SPELL_AURA_MOD_FEAR_2:
            l_IsCrowControlAura = true;
            break;
        default:
            break;
    }

    if (GetSpellInfo()->AuraInterruptFlags & SpellAuraInterruptFlags::AURA_INTERRUPT_FLAG_TAKE_DAMAGE_AMOUNT ||
        GetSpellInfo()->Attributes & SpellAttr0::SPELL_ATTR0_BREAKABLE_BY_DAMAGE)
        l_IsCrowControlAura = true;

    if (l_IsCrowControlAura)
    {
        m_canBeRecalculated = false;

        if (m_spellInfo->ProcFlags)
        {
            if (GetAuraType() == SPELL_AURA_MOD_ROOT || GetAuraType() == SPELL_AURA_MOD_ROOT_2)
                m_CrowdControlDamage = int32(GetBase()->GetUnitOwner()->CountPctFromMaxHealth(10));
            else
                m_CrowdControlDamage = int32(GetBase()->GetUnitOwner()->CountPctFromMaxHealth(2));
        }
    }

    // custom amount calculations go here
    switch (GetAuraType())
    {
        case SPELL_AURA_MOD_RATING:
        {
            // Heart's Judgment, Heart of Ignacious trinket (Heroic)
            if (m_spellInfo->Id == 92328 && caster)
            {
                if (Aura* pAura = caster->GetAura(92325))
                    amount *= pAura->GetStackAmount();
            }
            break;
        }
        // Any damage breaks confusion
        case SPELL_AURA_MOD_CONFUSE:
            amount = 1;
            break;
        case SPELL_AURA_SCHOOL_ABSORB:
        {
            m_canBeRecalculated = false;
            if (!caster)
                break;
            switch (GetSpellInfo()->SpellFamilyName)
            {
                case SPELLFAMILY_MONK:
                {
                    switch (GetSpellInfo()->Id)
                    {
                        case 116849: ///< Life Cocoon
                        {
                            DoneActualBenefit += caster->SpellBaseDamageBonusDone(m_spellInfo->GetSchoolMask()) * 42.0f;
                            break;
                        }
                        default:
                            break;
                    }

                    break;
                }
                case SPELLFAMILY_MAGE:
                {
                    // Mage Ward
                    if (GetSpellInfo()->SpellFamilyFlags[0] & 0x8 && GetSpellInfo()->SpellFamilyFlags[2] & 0x8)
                    {
                        // +80.68% from sp bonus
                        DoneActualBenefit += caster->SpellBaseDamageBonusDone(m_spellInfo->GetSchoolMask()) * 0.8068f;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case SPELL_AURA_MANA_SHIELD:
            m_canBeRecalculated = false;
            if (!caster)
                break;
            // Mana Shield
            if (GetSpellInfo()->SpellFamilyName == SPELLFAMILY_MAGE && GetSpellInfo()->SpellFamilyFlags[0] & 0x8000 && m_spellInfo->SpellFamilyFlags[2] & 0x8)
            {
                // +80.7% from +spd bonus
                DoneActualBenefit += caster->SpellBaseDamageBonusDone(m_spellInfo->GetSchoolMask()) * 0.807f;
            }
            break;
        case SPELL_AURA_DUMMY:
            if (!caster)
                break;
            break;
        case SPELL_AURA_PERIODIC_DAMAGE:
        {
            if (!caster)
                break;
            break;
        }
        case SPELL_AURA_PERIODIC_ENERGIZE:
        {
            switch (m_spellInfo->Id)
            {
                case 57669: // Replenishment (0.2% from max)
                    amount = CalculatePct(GetBase()->GetUnitOwner()->GetMaxPower(POWER_MANA), amount);
                    break;
                default:
                    break;
            }

            break;
        }
        case SPELL_AURA_PERIODIC_HEAL:
        {
            if (!caster)
                break;

            switch (GetId())
            {
                case 746:    ///< First Aid from Linen Bandage
                case 1159:   ///< First Aid from Heavy Linen Bandage
                case 3267:   ///< First Aid from Wool Bandage
                case 3268:   ///< First Aid from Heavy Wool Bandage
                case 7926:   ///< First Aid from Silk Bandage
                case 7927:   ///< First Aid from Heavy Silk Bandage
                case 10838:  ///< First Aid from Mageweave Bandage
                case 10839:  ///< First Aid from Heavy Mageweave Bandage
                case 18608:  ///< First Aid from Runecloth Bandage
                case 18610:  ///< First Aid from Heavy Runecloth Bandage
                case 23567:  ///< First Aid from Warsong Gulch Runecloth Bandage
                case 23568:  ///< First Aid from Warsong Gulch Mageweave Bandage
                case 23569:  ///< First Aid from Warsong Gulch Silk Bandage
                case 23696:  ///< First Aid from Alterac Heavy Runecloth Bandage
                case 24412:  ///< First Aid from Highlander's Silk Bandage
                case 24413:  ///< First Aid from Highlander's Mageweave Bandage
                case 24414:  ///< First Aid from Arathi Basin Runecloth Bandage
                case 27030:  ///< First Aid from Netherweave Bandage
                case 27031:  ///< First Aid from Heavy Netherweave Bandage
                case 45543:  ///< First Aid from Frostweave Bandage
                case 45544:  ///< First Aid from Heavy Frostweave Bandage
                case 51803:  ///< First Aid from ?
                case 51827:  ///< First Aid from ?
                case 72996:  ///< First Aid from ?
                case 74553:  ///< First Aid from Embersilk Bandage
                case 74554:  ///< First Aid from Heavy Embersilk Bandage
                case 74555:  ///< First Aid from Dense Embersilk Bandage
                case 102694: ///< First Aid from Windwool Bandage
                case 102695: ///< First Aid from Heavy Windwool Bandage
                case 129096: ///< First Aid from Battle-Mender's Dressing
                case 133940: ///< First aid from Silkweave Bandage
                case 161255: ///< First Aid from Antiseptic Bandage
                case 170401: ///< First Aid from Ashran Bandage
                case 202850: ///< First Aid from Silkweave Bandage
                case 202852: ///< First Aid from Silkweave Splint
                case 221409: ///< First Aid from Silvery Salve
                case 230048: ///< First Aid from Feathered Luffa (7.1.0)
                case 232495: ///< First Aid from Arcane Splint (7.2.2)
                {
                    /// The Doctor Is In (Guild perk)
                    if (Aura* doctorIsIn = caster->GetAura(118076))
                    {
                        if (caster->GetTypeId() != TYPEID_PLAYER)
                            break;

                        Player* _plr = caster->ToPlayer();
                        if (_plr->GetMap()->IsBattlegroundOrArena())
                            break;

                        AddPct(amount, doctorIsIn->GetEffect(0)->GetAmount());
                    }

                    break;
                }
                default:
                    break;
            }

            break;
        }
        case SPELL_AURA_MOD_THREAT:
        {
            uint8 level_diff = 0;
            float multiplier = 0.0f;
            switch (GetId())
            {
                // Arcane Shroud
                case 26400:
                    level_diff = GetBase()->GetUnitOwner()->getLevelForTarget(caster) - 60;
                    multiplier = 2;
                    break;
            }
            if (level_diff > 0)
                amount += int32(multiplier * level_diff);
            break;
        }
        case SPELL_AURA_MOD_INCREASE_HEALTH:
            // Vampiric Blood
            if (GetId() == 55233)
                amount = GetBase()->GetUnitOwner()->CountPctFromMaxHealth(amount);
            break;
        case SPELL_AURA_MOD_INCREASE_SPEED:
           //Displacer Beast - Keep the sprint if not in cat form
            if (GetId() == 137452)
                break;
            // Dash - do not set speed if not in cat form
            if (GetSpellInfo()->SpellFamilyName == SPELLFAMILY_DRUID && GetSpellInfo()->SpellFamilyFlags[2] & 0x00000008)
                amount = GetBase()->GetUnitOwner()->GetShapeshiftForm() == FORM_CAT_FORM ? amount : 0;
            break;
        case SPELL_AURA_MOUNTED:
        {
            uint32 l_MountType = GetMiscValueB();

            if (MountEntry const* l_MountEntry = GetMountBySpell(GetSpellInfo()->Id))
                l_MountType = l_MountEntry->MountType;

            if (MountCapabilityEntry const* mountCapability = GetBase()->GetUnitOwner()->GetMountCapability(l_MountType))
            {
                amount = mountCapability->Id;
                m_canBeRecalculated = false;
            }
            break;
        }
        case SPELL_AURA_BYPASS_ARMOR_FOR_CASTER:
        {
            switch (GetId())
            {
                case 91021: // Find Weakness
                    if (GetBase()->GetUnitOwner()->IsPlayer())
                        amount /= 2;
                    break;
                default:
                    break;
            }

            break;
        }
        case SPELL_AURA_MOD_SPEED_ALWAYS:
        {
            // Stealth / Prowl
            if (m_spellInfo->HasAura(SPELL_AURA_MOD_STEALTH))
            {
                Unit * target = GetBase()->GetUnitOwner();
                if (!target)
                    break;

                // Elusiveness (Racial Passive)
                if (AuraEffect* eff = target->GetAuraEffect(21009, 0))
                    amount += eff->GetAmount();
            }
            break;
        }
        case SPELL_AURA_MOD_INCREASE_SWIM_SPEED:
        {
            switch (GetId())
            {
                case 86496: // Mount Speed Mod: Walking Speed Ground Mount
                {
                    if (!caster)
                        break;

                    if (uint32 mapId = caster->GetZoneId())
                    {
                        // Vashj'Ir
                        if (mapId == 4815 || mapId == 5144 ||mapId == 5145 || mapId == 5146)
                            amount = 300;
                        else
                            break;
                    }

                    break;
                }
                default:
                    break;
            }

            break;
        }
        case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
        {
        }
        case SPELL_AURA_MOD_DAMAGE_PERCENT_DONE:
        {
            switch (GetId())
            {
                case 211048: ///< Chaos Blades
                {
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        float l_Mastery = caster->GetFloatValue(PLAYER_FIELD_MASTERY) / 2.0f;
                        amount = l_Mastery;
                    }
                    break;
                }
            }
            break;
        }
        default:
            break;
    }

    if (m_spellInfo->HasAttribute(SPELL_ATTR10_STACK_DAMAGE_OR_HEAL) && GetBase()->GetMaxDuration() != -1 && caster && GetTotalTicks())
    {
        switch (GetAuraType())
        {
            case SPELL_AURA_OBS_MOD_HEALTH:
            case SPELL_AURA_PERIODIC_HEAL:
            case SPELL_AURA_PERIODIC_LEECH:
            case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
            case SPELL_AURA_PERIODIC_SCHOOL_DAMAGE:
            case SPELL_AURA_PERIODIC_DAMAGE:
            {
                Unit* l_Target = GetBase()->GetUnitOwner();

                if (l_Target != nullptr)
                {
                    uint32 l_OldBp = l_Target->GetRemainingPeriodicAmount(caster->GetGUID(), m_spellInfo->Id, GetAuraType());
                    uint32 getTicks = GetTotalTicks();

                    amount *= getTicks;

                    if (l_OldBp)
                        getTicks++;

                    if (amount)
                        amount /= getTicks;

                    amount += l_OldBp;
                }
                break;
            }
            default:
                break;
        }
    }

    if (DoneActualBenefit != 0.0f && caster)
        amount += (int32)DoneActualBenefit;

    GetBase()->CallScriptEffectCalcAmountHandlers(this, amount, m_canBeRecalculated);

    if (caster && caster->GetTypeId() == TypeID::TYPEID_PLAYER)
    {
        if (GetAuraType() == AuraType::SPELL_AURA_SCHOOL_ABSORB || GetAuraType() == AuraType::SPELL_AURA_SCHOOL_HEAL_ABSORB)
        {
            amount = AbsorbBonusDone(caster, amount);
            amount = AbsorbBonusTaken(caster, GetBase()->GetUnitOwner(), amount);

            bool l_IsCrit = false;

            /// Check if is crit
            if (caster->IsAuraAbsorbCrit(m_spellInfo, m_spellInfo->GetSchoolMask()) && GetBase()->GetCastItemLevel() <= 0)
            {
                l_IsCrit = true; ///< l_IsCrit is never read 01/18/16
                if (amount != -1) ///< Fix Absord infinite
                    amount = caster->SpellCriticalAuraAbsorbBonus(m_spellInfo, amount);
            }
        }
    }

    CalculateFromDummyAmount(caster, GetBase()->GetUnitOwner(), amount);

    LOG_SPELL(caster, GetId(), "CalculateAmount(): Aura %s: EffIndex %i: : Amount %i (DoneActualBenefit %i) * %i (stacks)", GetSpellInfo()->GetNameForLogging().c_str(), GetEffIndex(), amount, DoneActualBenefit, GetBase()->GetStackAmount());

    if (CanAuraEffectBeStacked())
        amount *= GetBase()->GetStackAmount();

    if (AmountIsNotDependantOfStackModification())
        amount = m_amount;

    return amount;
}

uint32 AuraEffect::AbsorbBonusDone(Unit* p_Caster, int32 p_Amount)
{
    if (m_spellInfo->HasAttribute(SPELL_ATTR3_NO_DONE_BONUS) || m_spellInfo->HasAttribute(SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS))
        return p_Amount;

    /// Apply bonus absorption
    float l_TotalMod = 1.0f;

    Unit::AuraEffectList const l_ModAbsorbEffectList = p_Caster->GetAuraEffectsByType(SPELL_AURA_MOD_ABSORPTION_PCT);
    for (Unit::AuraEffectList::const_iterator l_i = l_ModAbsorbEffectList.begin(); l_i != l_ModAbsorbEffectList.end(); ++l_i)
    {
        /// HackFix for Bulwark Of Order Absorb - Artifact trait
        if (m_spellInfo->Id == 209388 /*Bulwark Of Order Absorb*/ && (*l_i)->GetId() == 209858/*Necrotic Rot*/)
            continue;

        AddPct(l_TotalMod, (*l_i)->GetAmount());
    }

    /// HackFix for Shield of Faith -Artifact trait
    if (m_spellInfo->Id == 17)
    {
        if (Aura* l_Aura = p_Caster->GetAura(197729))
        {
            if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(EFFECT_0))
            {
                AddPct(l_TotalMod, (float)l_AuraEffect->GetAmount());
            }
        }
    }

    float l_VersaMod = 1.0f;
    /// Apply Versatility Absorb Bonus Done
    if (p_Caster->GetSpellModOwner())
        AddPct(l_VersaMod, p_Caster->GetSpellModOwner()->GetRatingBonusValue(CR_VERSATILITY_DAMAGE_DONE) + p_Caster->GetSpellModOwner()->GetTotalAuraModifier(SPELL_AURA_MOD_VERSATILITY_PCT));

    p_Amount *= l_TotalMod;
    p_Amount *= l_VersaMod;

    return p_Amount;
}

uint32 AuraEffect::AbsorbBonusTaken(Unit* p_Caster, Unit* p_Target, int32 p_Amount)
{
    if (!p_Target)
        return p_Amount;

    float l_TotalMod = 1.0f;

    Unit::AuraEffectList const& mHealingGet = p_Target->GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_RECEIVED);
    for (Unit::AuraEffectList::const_iterator i = mHealingGet.begin(); i != mHealingGet.end(); ++i)
    {
        if (p_Caster->GetGUID() == (*i)->GetCasterGUID() && (*i)->IsAffectingSpell(GetSpellInfo()))
            AddPct(l_TotalMod, (*i)->GetAmount());
    }

    if (p_Caster->GetSpellModOwner() && p_Target->GetSpellModOwner())
        AddPct(l_TotalMod, p_Caster->GetSpellModOwner()->GetDiminishingPVPDamage(GetSpellInfo()));

    /// Celestial Fortune (Brewmaster Monk) - Has SPELL_AURA_MOD_ABSORPTION which is only handled in AbsorbBonusDone
    if (p_Target->IsPlayer() && p_Target->HasAura(216519))
    {
        if (AuraEffect const* l_AurEff = p_Target->GetAuraEffect(216519, SpellEffIndex::EFFECT_0))
        {
            float l_Crit_chance = p_Target->GetFloatValue(EPlayerFields::PLAYER_FIELD_SPELL_CRIT_PERCENTAGE);
            if (roll_chance_f(l_Crit_chance))
                AddPct(l_TotalMod, l_AurEff->GetAmount());
        }
    }

    if (GetAuraType() == SPELL_AURA_SCHOOL_ABSORB)
    {
        switch (GetSpellInfo()->Id)
        {
            case 122470: ///< Touch of Karma
            case 48707: ///< Anti-magic Shell
            case 145051: ///< Protection of Niuzao
                break;
            default:
                if (auto auraEffect = p_Target->GetAuraEffect(110310, EFFECT_0)) ///< Dampening
                    AddPct(l_TotalMod, -auraEffect->GetAmount());
                break;
        }
    }

    p_Amount *= l_TotalMod;

    return p_Amount;
}

void AuraEffect::CalculateFromDummyAmount(Unit* caster, Unit* target, int32 &amount)
{
    if (!caster)
        return;

    if (std::vector<SpellAuraDummy> const* spellAuraDummy = sSpellMgr->GetSpellAuraDummy(GetId()))
    {
        for (std::vector<SpellAuraDummy>::const_iterator itr = spellAuraDummy->begin(); itr != spellAuraDummy->end(); ++itr)
        {
            sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AuraEffect::CalculateFromDummyAmount start GetId %i, amount %i, mask %i type %u option %u caster %u GetCaster %u",
            GetId(), amount, itr->EffectMask, itr->Type, itr->Options, caster->GetGUIDLow(), GetCaster()->GetGUIDLow());

            if (itr->Type != SPELL_DUMMY_DEFAULT)
                continue;

            if (!(itr->EffectMask & (1<<m_effIndex)))
                continue;

            Unit* _caster = caster;
            Unit* _targetAura = caster;
            bool check = false;

            if (itr->Caster == 1 && target) //get caster as target
                _caster = target;
            if (itr->Caster == 2 && _caster->ToPlayer()) //get target pet
            {
                if (Pet* pet = _caster->ToPlayer()->GetPet())
                    _caster = (Unit*)pet;
            }
            if (itr->Caster == 3) //get target owner
            {
                if (Unit* owner = _caster->GetOwner())
                    _caster = owner;
            }

            if (itr->TargetAura == 1) //get caster aura
                _targetAura = GetCaster();
            if (itr->TargetAura == 2 && target) //get target aura
                _targetAura = target;

            if (!_targetAura)
                _targetAura = caster;

            Aura* _aura = _caster->GetAura(abs(itr->SpellDummyID), caster->GetGUID());

            switch (itr->Options)
            {
                case SPELL_DUMMY_ENABLE: //0
                {
                    if (itr->Aura > 0 && !_targetAura->HasAura(itr->Aura))
                        continue;
                    if (itr->Aura < 0 && _targetAura->HasAura(abs(itr->Aura)))
                        continue;

                    if (itr->SpellDummyID > 0 && !_aura)
                    {
                        amount = 0;
                        check = true;
                    }
                    if (itr->SpellDummyID < 0 && _aura)
                    {
                        amount = 0;
                        check = true;
                    }
                    break;
                }
                case SPELL_DUMMY_ADD_PERC: //1
                {
                    if (itr->Aura > 0 && !_targetAura->HasAura(itr->Aura))
                        continue;
                    if (itr->Aura < 0 && _targetAura->HasAura(abs(itr->Aura)))
                        continue;

                    if (itr->SpellDummyID > 0 && _aura)
                    {
                        if (AuraEffect const* dummyEff = _aura->GetEffect(itr->EffectDummy))
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyEff->GetAmount();
                            amount += CalculatePct(amount, bp);
                            check = true;
                        }
                        else if (SpellInfo const* dummyInfo = sSpellMgr->GetSpellInfo(itr->SpellDummyID)) // for spell without aura for this affect
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyInfo->Effects[itr->EffectDummy].BasePoints;
                            amount += CalculatePct(amount, bp);
                            check = true;
                        }
                    }
                    if (itr->SpellDummyID < 0 && _aura)
                    {
                        if (AuraEffect const* dummyEff = _aura->GetEffect(itr->EffectDummy))
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyEff->GetAmount();
                            amount -= CalculatePct(amount, bp);
                            check = true;
                        }
                        else if (SpellInfo const* dummyInfo = sSpellMgr->GetSpellInfo(abs(itr->SpellDummyID))) // for spell without aura for this affect
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyInfo->Effects[itr->EffectDummy].BasePoints;
                            amount -= CalculatePct(amount, bp);
                            check = true;
                        }
                    }
                    break;
                }
                case SPELL_DUMMY_ADD_VALUE: //2
                {
                    if (itr->Aura > 0 && !_targetAura->HasAura(itr->Aura))
                        continue;
                    if (itr->Aura < 0 && _targetAura->HasAura(abs(itr->Aura)))
                        continue;

                    if (itr->SpellDummyID > 0 && _aura)
                    {
                        if (AuraEffect const* dummyEff = _aura->GetEffect(itr->EffectDummy))
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyEff->GetAmount();
                            amount += bp;
                            check = true;
                        }
                        else if (SpellInfo const* dummyInfo = sSpellMgr->GetSpellInfo(itr->SpellDummyID)) // for spell without aura for this affect
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyInfo->Effects[itr->EffectDummy].BasePoints;
                            amount += bp;
                            check = true;
                        }
                    }
                    if (itr->SpellDummyID < 0 && _aura)
                    {
                        if (AuraEffect const* dummyEff = _aura->GetEffect(itr->EffectDummy))
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyEff->GetAmount();
                            amount -= bp;
                            check = true;
                        }
                        else if (SpellInfo const* dummyInfo = sSpellMgr->GetSpellInfo(abs(itr->SpellDummyID))) // for spell without aura for this affect
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyInfo->Effects[itr->EffectDummy].BasePoints;
                            amount -= bp;
                            check = true;
                        }
                    }
                    break;
                }
                case SPELL_DUMMY_ADD_PERC_BP: //8
                {
                    if (itr->Aura > 0 && !_targetAura->HasAura(itr->Aura))
                        continue;
                    if (itr->Aura < 0 && _targetAura->HasAura(abs(itr->Aura)))
                        continue;

                    if (itr->SpellDummyID > 0 && _aura)
                    {
                        if (AuraEffect const* dummyEff = _aura->GetEffect(itr->EffectDummy))
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyEff->GetAmount();

                            bp /= 100.0f;
                            amount += CalculatePct(amount, bp);
                            check = true;
                        }
                        else if (SpellInfo const* dummyInfo = sSpellMgr->GetSpellInfo(itr->SpellDummyID)) // for spell without aura for this affect
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyInfo->Effects[itr->EffectDummy].BasePoints;

                            bp /= 100.0f;
                            amount += CalculatePct(amount, bp);
                            check = true;
                        }
                    }
                    if (itr->SpellDummyID < 0 && _aura)
                    {
                        if (AuraEffect const* dummyEff = _aura->GetEffect(itr->EffectDummy))
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyEff->GetAmount();

                            bp /= 100.0f;
                            amount -= CalculatePct(amount, bp);
                            check = true;
                        }
                        else if (SpellInfo const* dummyInfo = sSpellMgr->GetSpellInfo(abs(itr->SpellDummyID))) // for spell without aura for this affect
                        {
                            float bp = itr->CustomBP;
                            if (!bp)
                                bp = dummyInfo->Effects[itr->EffectDummy].BasePoints;

                            bp /= 100.0f;
                            amount -= CalculatePct(amount, bp);
                            check = true;
                        }
                    }
                    break;
                }
            }

            if (check && itr->RemoveAura)
                _caster->RemoveAurasDueToSpell(itr->RemoveAura);
        }

        sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AuraEffect::CalculateFromDummyAmount end GetId %i, amount %i m_effIndex %u", GetId(), amount, m_effIndex);
    }
}

void AuraEffect::CalculatePeriodic(Unit* caster, bool resetPeriodicTimer /*= true*/, bool load /*= false*/, bool p_Recalculation /* = false*/, bool p_IgnorePreviousModOnReapply /* = false*/)
{
    if (m_AmplitudeForced && !p_IgnorePreviousModOnReapply)
        return;

    m_amplitude = m_spellInfo->Effects[m_effIndex].Amplitude;

    if (caster && caster->IsPlayer() && m_spellInfo->Id == 163317) ///< Rushing Jade Wind
        if (SpellInfo const* l_RushingJadeInfo = sSpellMgr->GetSpellInfo(116847))
            m_amplitude = l_RushingJadeInfo->Effects[EFFECT_0].Amplitude;

    // prepare periodics
    switch (GetAuraType())
    {
        case SPELL_AURA_OBS_MOD_POWER:
            // 3 spells have no amplitude set
            if (!m_amplitude)
                m_amplitude = 1 * IN_MILLISECONDS;
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_SCHOOL_DAMAGE:
        case SPELL_AURA_PERIODIC_HEAL:
        case SPELL_AURA_OBS_MOD_HEALTH:
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
        case SPELL_AURA_PERIODIC_ENERGIZE:
        case SPELL_AURA_PERIODIC_LEECH:
        case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
        case SPELL_AURA_PERIODIC_MANA_LEECH:
        case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
        case SPELL_AURA_POWER_BURN:
        case SPELL_AURA_PERIODIC_DUMMY:
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
            m_isPeriodic = true;
            break;
        default:
            break;
    }

    GetBase()->CallScriptEffectCalcPeriodicHandlers(this, m_isPeriodic, m_amplitude);

    if (!m_isPeriodic)
        return;

    Player* modOwner = caster ? caster->GetSpellModOwner() : NULL;

    // Apply casting time mods
    if (m_amplitude)
    {
        // Apply periodic time mod
        if (modOwner)
            modOwner->ApplySpellMod(GetId(), SPELLMOD_ACTIVATION_TIME, m_amplitude);

        if (caster)
        {
            // Haste modifies periodic time of channeled spells
            if (m_spellInfo->AttributesEx5 & SPELL_ATTR5_HASTE_AFFECT_DURATION || m_spellInfo->HasAttribute(SpellAttr8::SPELL_ATTR8_HASTE_AFFECT_DURATION))
                m_amplitude = int32(m_amplitude * std::max<float>(caster->GetOwnerOrUnitItself()->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED), 0.5f));

            m_amplitude *= caster->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE);

            if (!m_spellInfo->IsPositive())
            {
                std::list<Unit*> l_TargetList;
                GetTargetList(l_TargetList);

                if (l_TargetList.size() == 1)
                {
                    std::list<AuraEffect*> l_AuraEffectList = l_TargetList.front()->GetAuraEffectsByType(AuraType::SPELL_AURA_MOD_DEBUFF_SPEED);
                    float l_Speed = 1.0f;
                    for (AuraEffect const* l_AuraEffect : l_AuraEffectList)
                        AddPct(l_Speed, l_AuraEffect->GetAmount());

                    m_amplitude /= l_Speed;
                }
            }
        }
    }

    if (p_Recalculation) ///< Case of Update Amplitude, we don't need to recalculate TickNumber
        return;

    if (load) // aura loaded from db
    {
        m_tickNumber = m_amplitude ? GetBase()->GetDuration() / m_amplitude : 0;
        m_periodicTimer = m_amplitude ? GetBase()->GetDuration() % m_amplitude : 0;
        if (m_spellInfo->AttributesEx5 & SPELL_ATTR5_START_PERIODIC_AT_APPLY)
            ++m_tickNumber;
    }
    else // aura just created or reapplied
    {
        m_tickNumber = 0;
        // reset periodic timer on aura create or on reapply when aura isn't dot
        // possibly we should not reset periodic timers only when aura is triggered by proc
        // or maybe there's a spell attribute somewhere
        if (resetPeriodicTimer)
        {
            m_periodicTimer = 0;
            // Start periodic on next tick or at aura apply
            if (m_amplitude && !(m_spellInfo->AttributesEx5 & SPELL_ATTR5_START_PERIODIC_AT_APPLY))
                m_periodicTimer += m_amplitude;
        }
    }
}

void AuraEffect::CalculateSpellMod()
{
    switch (GetAuraType())
    {
        case SPELL_AURA_ADD_FLAT_MODIFIER:
        case SPELL_AURA_ADD_PCT_MODIFIER:
        {
            if (!m_spellmod)
            {
                m_spellmod = new SpellModifier(GetBase());
                m_spellmod->op = SpellModOp(GetMiscValue());

                m_spellmod->type = SpellModType(uint32(GetAuraType())); // SpellModType value == spell aura types
                m_spellmod->spellId = GetId();
                m_spellmod->mask = GetSpellEffectInfo()->SpellClassMask;
                m_spellmod->charges = GetBase()->GetCharges();
            }

            m_spellmod->value = GetAmount();
            break;
        }
        default:
            break;
    }

    GetBase()->CallScriptEffectCalcSpellModHandlers(this, m_spellmod);
}

void AuraEffect::ChangeAmount(int32 newAmount, bool mark, bool onStackOrReapply, bool forceChangeAmount/* = false*/, float newFloatAmount/* = 0.0f*/)
{
    // Reapply if amount change
    uint8 handleMask = 0;
    if (newAmount != GetAmount() || forceChangeAmount)
        handleMask |= AURA_EFFECT_HANDLE_CHANGE_AMOUNT;
    if (onStackOrReapply)
        handleMask |= AURA_EFFECT_HANDLE_REAPPLY;

    if (!handleMask)
        return;

    std::list<AuraApplication*> effectApplications;
    GetApplicationList(effectApplications);

    for (std::list<AuraApplication*>::const_iterator apptItr = effectApplications.begin(); apptItr != effectApplications.end(); ++apptItr)
        if ((*apptItr)->HasEffect(GetEffIndex()))
            HandleEffect(*apptItr, handleMask, false);

    if (handleMask & AURA_EFFECT_HANDLE_CHANGE_AMOUNT)
    {
        if (!mark)
            m_amount = newAmount;
        else
            SetAmount(newAmount);

        SetFloatAmount(G3D::fuzzyEq(newFloatAmount, 0.0f) ? newAmount : newFloatAmount);
    }

    if (handleMask & AURA_EFFECT_HANDLE_REAPPLY)
        CalculateSpellMod();

    for (std::list<AuraApplication*>::const_iterator apptItr = effectApplications.begin(); apptItr != effectApplications.end(); ++apptItr)
        if ((*apptItr)->HasEffect(GetEffIndex()))
            HandleEffect(*apptItr, handleMask, true);

    if (GetSpellInfo()->HasAttribute(SPELL_ATTR8_AURA_SEND_AMOUNT))
        GetBase()->SetNeedClientUpdateForTargets();
}

void AuraEffect::HandleEffect(AuraApplication * aurApp, uint8 mode, bool apply)
{
    // check if call is correct, we really don't want using bitmasks here (with 1 exception)
    ASSERT(mode == AURA_EFFECT_HANDLE_REAL
        || mode == AURA_EFFECT_HANDLE_SEND_FOR_CLIENT
        || mode == AURA_EFFECT_HANDLE_CHANGE_AMOUNT
        || mode == AURA_EFFECT_HANDLE_STAT
        || mode == AURA_EFFECT_HANDLE_SKILL
        || mode == AURA_EFFECT_HANDLE_REAPPLY
        || mode == (AURA_EFFECT_HANDLE_CHANGE_AMOUNT | AURA_EFFECT_HANDLE_REAPPLY));

    // register/unregister effect in lists in case of real AuraEffect apply/remove
    // registration/unregistration is done always before real effect handling (some effect handlers code is depending on this)
    if (mode & AURA_EFFECT_HANDLE_REAL)
        aurApp->GetTarget()->_RegisterAuraEffect(this, apply);

    // real aura apply/remove, handle modifier
    if (mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_REAPPLY))
        ApplySpellMod(aurApp->GetTarget(), apply);

    // call scripts helping/replacing effect handlers
    Aura* auraBase = GetBase();
    bool prevented = false;

    if (auraBase)
    {
        if (apply)
            prevented = auraBase->CallScriptEffectApplyHandlers(this, const_cast<AuraApplication const*>(aurApp), (AuraEffectHandleModes)mode);
        else
            prevented = auraBase->CallScriptEffectRemoveHandlers(this, const_cast<AuraApplication const*>(aurApp), (AuraEffectHandleModes)mode);
    }

    // check if script events have removed the aura or if default effect prevention was requested
    if ((apply && aurApp->GetRemoveMode()) || prevented)
        return;

    if (GetAuraType() >= TOTAL_AURAS)
        return;

    (*this.*AuraEffectHandler [GetAuraType()])(const_cast<AuraApplication const*>(aurApp), mode, apply);

    // check if script events have removed the aura or if default effect prevention was requested
    if (apply && aurApp->GetRemoveMode())
        return;

    if (!apply && aurApp->GetBase()->GetId() == 113656)
        if (Unit* l_Caster = aurApp->GetBase()->GetCaster())
            if (l_Caster->IsPlayer())
                l_Caster->ToPlayer()->SetLastCastedSpell(113656);

    // call scripts triggering additional events after apply/remove
    if (auraBase)
    {
        if (apply)
            auraBase->CallScriptAfterEffectApplyHandlers(this, const_cast<AuraApplication const*>(aurApp), (AuraEffectHandleModes)mode);
        else
            auraBase->CallScriptAfterEffectRemoveHandlers(this, const_cast<AuraApplication const*>(aurApp), (AuraEffectHandleModes)mode);
    }
}

void AuraEffect::HandleEffect(Unit* target, uint8 mode, bool apply)
{
    AuraApplication* aurApp = GetBase()->GetApplicationOfTarget(target->GetGUID());
    ASSERT(aurApp);
    HandleEffect(aurApp, mode, apply);
}

void AuraEffect::ApplySpellMod(Unit* target, bool apply)
{
    if (!m_spellmod || target->GetTypeId() != TYPEID_PLAYER)
        return;

    target->ToPlayer()->AddSpellMod(m_spellmod, apply);

    Aura* checkBase = GetBase();
    // Auras with charges do not mod amount of passive auras
    if (checkBase && checkBase->IsUsingCharges())
        return;

    // reapply some passive spells after add/remove related spellmods
    // Warning: it is a dead loop if 2 auras each other amount-shouldn't happen
    switch (GetMiscValue())
    {
        case SPELLMOD_ALL_EFFECTS:
        case SPELLMOD_EFFECT1:
        case SPELLMOD_EFFECT2:
        case SPELLMOD_EFFECT3:
        case SPELLMOD_EFFECT4:
        case SPELLMOD_EFFECT5:
        {
            uint64 guid = target->GetGUID();
            Unit::AuraApplicationMap & auras = target->GetAppliedAuras();
            for (Unit::AuraApplicationMap::iterator iter = auras.begin(); iter != auras.end(); ++iter)
            {
                Aura* aura = iter->second->GetBase();
                // only passive and permanent auras-active auras should have amount set on spellcast and not be affected
                // if aura is casted by others, it will not be affected
                if ((aura->IsPassive() || aura->IsPermanent()) && aura->GetSpellInfo()->IsAffectedBySpellMod(m_spellmod))
                {
                    if (aura->GetCasterGUID() != guid)
                    {
                        Unit* l_Caster = aura->GetCaster();
                        if (!l_Caster || l_Caster->GetOwnerGUID() != guid)
                            continue;
                    }

                    if (GetMiscValue() == SPELLMOD_ALL_EFFECTS)
                    {
                        for (uint8 i = 0; i < aura->GetEffectCount(); ++i)
                        {
                            if (AuraEffect* aurEff = aura->GetEffect(i))
                                aurEff->RecalculateAmount(true);
                        }
                    }
                    else
                    {
                        uint8 effIndex = 0;
                        for (SpellModOp mod : { SPELLMOD_EFFECT1, SPELLMOD_EFFECT2, SPELLMOD_EFFECT3, SPELLMOD_EFFECT4, SPELLMOD_EFFECT5 })
                        {
                            if (GetMiscValue() == mod)
                            {
                                if (AuraEffect* aurEff = aura->GetEffect(effIndex))
                                    aurEff->RecalculateAmount(true);
                            }
                            ++effIndex;
                        }
                    }
                }
            }
        }
        default:
            break;
    }
}

void AuraEffect::Update(uint32 diff, Unit* caster)
{
    ((Aura*)m_base)->CallScriptEffectUpdateHandlers(diff, this);

    if (m_isPeriodic && (m_base->GetDuration() >= 0 || m_base->IsPassive() || m_base->IsPermanent()))
    {
        if (m_periodicTimer > int32(diff))
            m_periodicTimer -= diff;
        else // tick also at m_periodicTimer == 0 to prevent lost last tick in case max m_duration == (max m_periodicTimer)*N
        {
            ++m_tickNumber;

            // update before tick (aura can be removed in TriggerSpell or PeriodicTick calls)
            m_periodicTimer += m_amplitude - diff;
            UpdatePeriodic(caster);

            std::list<AuraApplication*> effectApplications;
            GetApplicationList(effectApplications);
            // tick on targets of effects
            for (std::list<AuraApplication*>::const_iterator apptItr = effectApplications.begin(); apptItr != effectApplications.end(); ++apptItr)
                if ((*apptItr)->HasEffect(GetEffIndex()))
                    PeriodicTick(*apptItr, caster, SpellEffIndex(GetEffIndex()));

            // TEMPORARY HACKS FOR PERIODIC HANDLERS OF DYNAMIC OBJECT AURAS
            if (m_base->GetType() == DYNOBJ_AURA_TYPE)
            {
                if (DynamicObject const* d_owner = m_base->GetDynobjOwner())
                {
                    switch (GetSpellInfo()->Id)
                    {
                        // Smoke Bomb
                        case 76577:
                            GetCaster()->CastSpell(d_owner->GetPositionX(), d_owner->GetPositionY(), d_owner->GetPositionZ(), 88611, true);
                            break;
                        case 103558: // Choking Smoke Bomb, Asira Dawnslayer, Hour of Twilight
                            GetCaster()->CastSpell(d_owner->GetPositionX(), d_owner->GetPositionY(), d_owner->GetPositionZ(), 103790, true);
                            break;
                    }
                }
            }
        }
    }
}

void AuraEffect::UpdatePeriodic(Unit* caster)
{
    switch (GetAuraType())
    {
        case SPELL_AURA_PERIODIC_DUMMY:
            switch (GetSpellInfo()->SpellFamilyName)
            {
                case SPELLFAMILY_GENERIC:
                    switch (GetId())
                    {
                        /// Drink
                        case 430:
                        case 431:
                        case 432:
                        case 1133:
                        case 1135:
                        case 1137:
                        case 10250:
                        case 22734:
                        case 27089:
                        case 34291:
                        case 43182:
                        case 43183:
                        case 46755:
                        case 49472:
                        case 57073:
                        case 61830:
                        case 72623:
                        case 80166:
                        case 80167:
                        case 87958:
                        case 87959:
                        case 92736:
                        case 92797:
                        case 92800:
                        case 92803:
                        case 104262:
                        case 104270:
                        case 105230:
                        case 105232:
                        case 118358:
                        case 130336:
                        case 172786:
                        case 225738:
                        case 227227:
                        {
                            if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                                return;
                            /// Get SPELL_AURA_MOD_POWER_REGEN aura from spell
                            if (AuraEffect* aurEff = GetBase()->GetEffect(0))
                            {
                                if (aurEff->GetAuraType() != SPELL_AURA_MOD_POWER_REGEN)
                                    m_isPeriodic = false;
                                else
                                {
                                    // default case - not in arena
                                    if (!caster->ToPlayer()->InArena())
                                    {
                                        aurEff->ChangeAmount(GetAmount());
                                        m_isPeriodic = false;
                                    }
                                    else
                                    {
                                        // **********************************************
                                        // This feature uses only in arenas
                                        // **********************************************
                                        // Here need increase mana regen per tick (6 second rule)
                                        // on 0 tick -   0  (handled in 2 second)
                                        // on 1 tick - 166% (handled in 4 second)
                                        // on 2 tick - 133% (handled in 6 second)

                                        // Apply bonus for 1 - 4 tick
                                        switch (m_tickNumber)
                                        {
                                            case 1:   // 0%
                                                aurEff->ChangeAmount(0);
                                                break;
                                            case 2:   // 166%
                                                aurEff->ChangeAmount(GetAmount() * 5 / 3);
                                                break;
                                            case 3:   // 133%
                                                aurEff->ChangeAmount(GetAmount() * 4 / 3);
                                                break;
                                            default:  // 100% - normal regen
                                                aurEff->ChangeAmount(GetAmount());
                                                // No need to update after 4th tick
                                                m_isPeriodic = false;
                                                break;
                                        }
                                    }
                                }
                            }

                            break;
                        }
                        case 58549: ///< Tenacity
                        case 59911: ///< Tenacity (vehicle)
                           GetBase()->RefreshDuration();
                           break;
                        case 66823: ///< Paralytic Toxin
                            // Get 0 effect aura
                            if (AuraEffect* slow = GetBase()->GetEffect(0))
                            {
                                int32 newAmount = slow->GetAmount() - 10;
                                if (newAmount < -100)
                                    newAmount = -100;
                                slow->ChangeAmount(newAmount);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case SPELLFAMILY_MAGE:
                    if (GetId() == 55342)// Mirror Image
                        m_isPeriodic = false;
                    break;
                case SPELLFAMILY_DEATHKNIGHT:
                    // Chains of Ice
                    if (GetSpellInfo()->SpellFamilyFlags[1] & 0x00004000)
                    {
                        // Get 0 effect aura
                        if (AuraEffect* slow = GetBase()->GetEffect(0))
                        {
                            int32 newAmount = slow->GetAmount() + GetAmount();
                            if (newAmount > 0)
                                newAmount = 0;
                            slow->ChangeAmount(newAmount);
                        }
                        return;
                    }
                    break;
                default:
                    break;
           }
       default:
           break;
    }
    /// Update Amplitude since WoD, should be recalculated dynamically
    CalculatePeriodic(caster, false, true, true);
    GetBase()->CallScriptEffectUpdatePeriodicHandlers(this);
}

bool AuraEffect::CanPeriodicTickCrit(Unit* target, Unit const* caster) const
{
    ASSERT(caster);

    if (caster->HasAuraTypeWithAffectMask(SPELL_AURA_ABILITY_PERIODIC_CRIT, m_spellInfo))
        return true;

    return caster->IsSpellCrit(target, m_spellInfo, m_spellInfo->GetSchoolMask(), WeaponAttackType::BaseAttack, DamageEffectType::DOT);
}

bool AuraEffect::IsAffectingSpell(SpellInfo const* spell) const
{
    if (!spell)
        return false;

    /// In case of SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS, we need to check MiscValue as spell id too
    if (m_spellInfo->Effects[m_effIndex].ApplyAuraName == SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS ||
        m_spellInfo->Effects[m_effIndex].ApplyAuraName == SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2)
    {
        if (m_spellInfo->Effects[m_effIndex].MiscValue == spell->Id)
            return true;
    }

    /// Check family name
    if (spell->SpellFamilyName != m_spellInfo->SpellFamilyName)
        return false;

    /// Check EffectClassMask
    if (m_spellInfo->Effects[m_effIndex].SpellClassMask & spell->SpellFamilyFlags)
        return true;

    return false;
}

void AuraEffect::SendTickImmune(Unit* target, Unit* caster) const
{
    if (caster)
        caster->SendSpellDamageImmune(target, m_spellInfo->Id);
}

void AuraEffect::PeriodicTick(AuraApplication * aurApp, Unit* caster, SpellEffIndex effIndex) const
{
    /// Only call the OnEffectPeriodic Hooks for NON dynamic spells, as dynamic ones will have their calculation redone later (which means the call is useless here)
    if (!IsPeriodicDynamicAmount(m_spellInfo->Id))
    {
        bool prevented = GetBase()->CallScriptEffectPeriodicHandlers(this, aurApp);
        if (prevented)
            return;
    }

    Unit* target = aurApp->GetTarget();

    AuraSpellTrigger(target, caster, effIndex);

    switch (GetAuraType())
    {
        case SPELL_AURA_PERIODIC_DUMMY:
            HandlePeriodicDummyAuraTick(target, caster, effIndex);
            break;
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
            HandlePeriodicTriggerSpellAuraTick(target, caster);
            break;
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
            HandlePeriodicTriggerSpellWithValueAuraTick(target, caster);
            break;
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
        case SPELL_AURA_PERIODIC_SCHOOL_DAMAGE:
            HandlePeriodicDamageAurasTick(target, caster);
            break;
        case SPELL_AURA_PERIODIC_LEECH:
            HandlePeriodicHealthLeechAuraTick(target, caster);
            break;
        case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
            HandlePeriodicHealthFunnelAuraTick(target, caster);
            break;
        case SPELL_AURA_PERIODIC_HEAL:
        case SPELL_AURA_OBS_MOD_HEALTH:
            HandlePeriodicHealAurasTick(target, caster);
            break;
        case SPELL_AURA_PERIODIC_MANA_LEECH:
            HandlePeriodicManaLeechAuraTick(target, caster);
            break;
        case SPELL_AURA_OBS_MOD_POWER:
            HandleObsModPowerAuraTick(target, caster);
            break;
        case SPELL_AURA_PERIODIC_ENERGIZE:
            HandlePeriodicEnergizeAuraTick(target, caster);
            break;
        case SPELL_AURA_POWER_BURN:
            HandlePeriodicPowerBurnAuraTick(target, caster);
            break;
        default:
            break;
    }

    GetBase()->CallScriptAfterEffectPeriodicHandlers(this, aurApp);
    GetBase()->SetPeriodicHealDone(0);
}

void AuraEffect::HandleProc(AuraApplication* aurApp, ProcEventInfo& eventInfo)
{
    bool prevented = GetBase()->CallScriptEffectProcHandlers(this, const_cast<AuraApplication const*>(aurApp), eventInfo);
    if (prevented)
        return;

    switch (GetAuraType())
    {
        case SPELL_AURA_PROC_TRIGGER_SPELL:
            HandleProcTriggerSpellAuraProc(aurApp, eventInfo);
            break;
        case SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE:
            HandleProcTriggerSpellWithValueAuraProc(aurApp, eventInfo);
            break;
        case SPELL_AURA_PROC_TRIGGER_DAMAGE:
            HandleProcTriggerDamageAuraProc(aurApp, eventInfo);
            break;
        case SPELL_AURA_RAID_PROC_FROM_CHARGE:
            HandleRaidProcFromChargeAuraProc(aurApp, eventInfo);
            break;
        default:
            break;
    }

    GetBase()->CallScriptAfterEffectProcHandlers(this, const_cast<AuraApplication const*>(aurApp), eventInfo);
}

void AuraEffect::CleanupTriggeredSpells(Unit* target)
{
    uint32 tSpellId = m_spellInfo->Effects[GetEffIndex()].TriggerSpell;
    if (!tSpellId || tSpellId == 225259 || tSpellId == 223126)
        return;

    ///< Scarlet Inquisitor's Expurgation
    if (tSpellId == 248289)
        return;

    SpellInfo const* tProto = sSpellMgr->GetSpellInfo(tSpellId);
    if (!tProto)
        return;

    if (tProto->GetDuration() != -1)
        return;

    // needed for spell 43680, maybe others
    // TODO: is there a spell flag, which can solve this in a more sophisticated way?
    if (m_spellInfo->Effects[GetEffIndex()].ApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL &&
            uint32(m_spellInfo->GetDuration()) == m_spellInfo->Effects[GetEffIndex()].Amplitude)
        return;

    target->RemoveAurasDueToSpell(tSpellId, GetCasterGUID());
}

void AuraEffect::HandleShapeshiftBoosts(Unit* target, bool apply) const
{
    std::list<uint32> l_CastList;
    std::list<uint32> l_RemoveList;

    switch (GetMiscValue())
    {
        case FORM_CAT_FORM:
        {
            l_CastList.push_back(3025);
            l_CastList.push_back(48629);
            l_CastList.push_back(106840);
            l_CastList.push_back(113636);
            break;
        }
        case FORM_TRAVEL_FORM:
        {
            l_CastList.push_back(5419);
            l_CastList.push_back(115034);
            break;
        }
        case FORM_AQUATIC_FORM:
        {
            l_CastList.push_back(5421);
            break;
        }
        case FORM_BEAR_FORM:
        {
            l_CastList.push_back(1178);
            l_CastList.push_back(21178);
            l_CastList.push_back(106829);
            l_CastList.push_back(106899);
            break;
        }
        case FORM_FLIGHT_FORM:
        {
            l_CastList.push_back(33948);
            l_CastList.push_back(34764);

            /// Archdruid's Lunarwing Form
            if (target->HasSpell(231437))
                l_CastList.push_back(243719);

            break;
        }
        case FORM_FLIGHT_FORM_EPIC:
        {
            l_CastList.push_back(40122);
            l_CastList.push_back(40121);

            /// Archdruid's Lunarwing Form
            if (target->HasSpell(231437))
                l_CastList.push_back(243719);

            break;
        }
        case FORM_SHADOWFORM:
        {
            /// Glyph of Shadow
            if (target->HasAura(107906))
            {
                l_CastList.push_back(107904);
                l_RemoveList.push_back(107903);
            }
            else
            {
                l_CastList.push_back(107903);
                l_RemoveList.push_back(107904);
            }

            /// Glyph of Shadowy Friends
            if (target->HasAura(126745))
                l_CastList.push_back(142024);

            break;
        }
        case FORM_GHOST_WOLF:
        {
            /// Glyph of the Ghost Wolf
            if (target->HasAura(58135) || !apply)
                l_CastList.push_back(160942);

            break;
        }
        case FORM_GHOUL:
        case FORM_AMBIENT:
        case FORM_STEALTH:
        case FORM_CREATURE_CAT:
        case FORM_CREATURE_BEAR:
        case FORM_BATTLE_STANCE:
        case FORM_DEFENSIVE_STANCE:
        case FORM_BERSERKER_STANCE:
        case FORM_GLADIATOR_STANCE:
        default:
            break;
    }

    if (apply)
    {
        /// Remove cooldown of spells triggered on stance change - they may share cooldown with stance spell
        for (uint32 const& l_Spell : l_CastList)
        {
            if (target->IsPlayer())
                target->ToPlayer()->RemoveSpellCooldown(l_Spell);

            target->CastSpell(target, l_Spell, true, NULL, this);
        }

        for (uint32 const& l_Spell : l_RemoveList)
            target->RemoveOwnedAura(l_Spell, target->GetGUID());

        if (target->IsPlayer())
        {
            Player* plrTarget = target->ToPlayer();

            PlayerSpellMap const& sp_list = plrTarget->GetSpellMap();
            for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
            {
                if (itr->second->state == PLAYERSPELL_REMOVED || itr->second->disabled)
                    continue;

                if (std::find(l_CastList.begin(), l_CastList.end(), itr->first) != l_CastList.end())
                    continue;

                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);
                if (!spellInfo || !(spellInfo->Attributes & (SPELL_ATTR0_PASSIVE | SPELL_ATTR0_HIDDEN_CLIENTSIDE)))
                    continue;

                if (spellInfo->Stances & (UI64LIT(1) << (GetMiscValue() - 1)))
                    target->CastSpell(target, itr->first, true, NULL, this);
            }
        }
    }
    else
    {
        for (uint32 const& l_Spell : l_CastList)
            target->RemoveOwnedAura(l_Spell, target->GetGUID());

        for (uint32 const& l_Spell : l_RemoveList)
            target->RemoveOwnedAura(l_Spell, target->GetGUID());

        const Unit::AuraEffectList& shapeshifts = target->GetAuraEffectsByType(SPELL_AURA_MOD_SHAPESHIFT);
        AuraEffect* newAura = nullptr;
        // Iterate through all the shapeshift auras that the target has, if there is another aura with SPELL_AURA_MOD_SHAPESHIFT, then this aura is being removed due to that one being applied
        for (Unit::AuraEffectList::const_iterator itr = shapeshifts.begin(); itr != shapeshifts.end(); ++itr)
        {
            if ((*itr) != this)
            {
                newAura = *itr;
                break;
            }
        }
        Unit::AuraApplicationMap& tAuras = target->GetAppliedAuras();
        for (Unit::AuraApplicationMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
        {
            // Use the new aura to see on what stance the target will be
            uint64 newStance = newAura ? (UI64LIT(1) << (newAura->GetMiscValue() - 1)) : 0;

            /// If the stances are not compatible with the spell, remove it, but not on periodic aura apply on you (aura should continue to tick)
            if (itr->second->GetBase()->IsRemovedOnShapeLost(target) && !(itr->second->GetBase()->GetSpellInfo()->Stances & newStance) && !itr->second->GetBase()->GetSpellInfo()->IsPeriodic() && !itr->second->GetBase()->GetSpellInfo()->GetDuration())
                target->RemoveAura(itr);
            else
                ++itr;
        }
    }
}

/*********************************************************/
/***               AURA EFFECT HANDLERS                ***/
/*********************************************************/

/**************************************/
/***       VISIBILITY & PHASES      ***/
/**************************************/

void AuraEffect::HandleModInvisibilityDetect(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();
    InvisibilityType type = InvisibilityType(GetMiscValue());

    if (apply)
    {
        target->m_invisibilityDetect.AddFlag(type);
        target->m_invisibilityDetect.AddValue(type, GetAmount());
    }
    else
    {
        if (!target->HasAuraType(SPELL_AURA_MOD_INVISIBILITY_DETECT))
            target->m_invisibilityDetect.DelFlag(type);

        target->m_invisibilityDetect.AddValue(type, -GetAmount());
    }

    // call functions which may have additional effects after chainging state of unit
    target->UpdateObjectVisibility();
}

void AuraEffect::HandleModInvisibility(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();
    InvisibilityType type = InvisibilityType(GetMiscValue());

    if (apply)
    {
        /// Apply glow vision
        /// YES it's weird to use "PLAYER_FIELD_NUM_BACKPACK_SLOTS" but this field content backpack slots, aura vision and IGNORE_POWER_REGEN_PREDICTION_MASK ...
        /// Blizzard name the fields not us :D
        if (target->IsPlayer() && GetSpellInfo()->GetMaxDuration() != -1)
            target->SetByteFlag(PLAYER_FIELD_NUM_BACKPACK_SLOTS, PLAYER_FIELD_NUM_BACKPACK_SLOTS_OFFSET_AURA_VISION, AURA_VISION_INVISIBILITY_GLOW);

        if (GetBase()->GetId() == 32612) // invisible mage pet
            if (Unit* pet = target->GetGuardianPet())
                target->AddAura(32612,pet);

        target->m_invisibility.AddFlag(type);
        target->m_invisibility.AddValue(type, GetAmount());
    }
    else
    {
        if (!target->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
        {
            /// If not have different invisibility auras.
            /// Remove glow vision
            /// YES it's weird to use "PLAYER_FIELD_NUM_BACKPACK_SLOTS" but this field content backpack slots, aura vision and IGNORE_POWER_REGEN_PREDICTION_MASK ...
            /// Blizzard name the fields not us :D
            if (target->IsPlayer())
                target->RemoveByteFlag(PLAYER_FIELD_NUM_BACKPACK_SLOTS, PLAYER_FIELD_NUM_BACKPACK_SLOTS_OFFSET_AURA_VISION, AURA_VISION_INVISIBILITY_GLOW);

            target->m_invisibility.DelFlag(type);
        }
        else
        {
            bool found = false;
            Unit::AuraEffectList const& invisAuras = target->GetAuraEffectsByType(SPELL_AURA_MOD_INVISIBILITY);
            for (Unit::AuraEffectList::const_iterator i = invisAuras.begin(); i != invisAuras.end(); ++i)
            {
                if (GetMiscValue() == (*i)->GetMiscValue())
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                target->m_invisibility.DelFlag(type);
        }

        target->m_invisibility.AddValue(type, -GetAmount());
    }

    // call functions which may have additional effects after chainging state of unit
    if (apply && (mode & AURA_EFFECT_HANDLE_REAL))
    {
        // drop flag at invisibiliy in bg
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);
    }

    target->UpdateObjectVisibility();
}

//TODO: Finish this aura ///< thanks you dev !
void AuraEffect::HandleModCamouflage(AuraApplication const *aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
    {
        //
    }
    else if (!(target->isCamouflaged()))
    {
        if (!GetCaster())
            return;

        if (Unit* pet = GetCaster()->GetGuardianPet())
            pet->RemoveAurasByType(SPELL_AURA_MOD_CAMOUFLAGE);
    }
}

void AuraEffect::HandleModStealthDetect(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();
    StealthType type = StealthType(GetMiscValue());

    if (apply)
    {
        target->m_stealthDetect.AddFlag(type);
        target->m_stealthDetect.AddValue(type, GetAmount());
    }
    else
    {
        if (!target->HasAuraType(SPELL_AURA_MOD_STEALTH_DETECT))
            target->m_stealthDetect.DelFlag(type);

        target->m_stealthDetect.AddValue(type, -GetAmount());
    }

    // call functions which may have additional effects after chainging state of unit
    target->UpdateObjectVisibility();
}

void AuraEffect::HandleModStealth(const AuraApplication * p_AuraApplication, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_SEND_FOR_CLIENT_MASK))
        return;

    Unit* l_Target = p_AuraApplication->GetTarget();
    StealthType l_Type = StealthType(GetMiscValue());

    if (p_Apply)
    {
        l_Target->m_stealth.AddFlag(l_Type);
        l_Target->m_stealth.AddValue(l_Type, GetAmount());

        l_Target->SetStandFlags(UNIT_STAND_FLAGS_CREEP);

        Unit::AttackerSet const& l_Attackers = l_Target->getAttackers();
        for (Unit::AttackerSet::const_iterator l_Itr = l_Attackers.begin(); l_Itr != l_Attackers.end();)
        {
            if (!(*l_Itr)->canSeeOrDetect(l_Target))
                (*(l_Itr++))->AttackStop();
            else
                ++l_Itr;
        }
    }
    else
    {
        l_Target->m_stealth.AddValue(l_Type, -GetAmount());

        if (!l_Target->HasAuraType(SPELL_AURA_MOD_STEALTH)) // if last SPELL_AURA_MOD_STEALTH
        {
            l_Target->m_stealth.DelFlag(l_Type);
            l_Target->RemoveStandFlags(UNIT_STAND_FLAGS_CREEP);
        }
    }

    // call functions which may have additional effects after chainging state of unit
    if (p_Apply && (p_Mode & AURA_EFFECT_HANDLE_REAL))
    {
        // drop flag at stealth in bg
        l_Target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);
        l_Target->RemoveFlagsAuras();
    }

    l_Target->UpdateObjectVisibility();
}

void AuraEffect::HandleModStealthLevel(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();
    StealthType type = StealthType(GetMiscValue());

    if (apply)
        target->m_stealth.AddValue(type, GetAmount());
    else
        target->m_stealth.AddValue(type, -GetAmount());

    // call functions which may have additional effects after chainging state of unit
    target->UpdateObjectVisibility();
}

void AuraEffect::HandleSpiritOfRedemption(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();

    if (l_Target->GetTypeId() != TYPEID_PLAYER)
        return;

    // prepare spirit state
    if (p_Apply)
    {
        if (l_Target->IsPlayer())
        {
            // disable breath/etc timers
            l_Target->ToPlayer()->StopMirrorTimers();

            // set stand state (expected in this form)
            if (!l_Target->IsStandState())
                l_Target->SetStandState(UNIT_STAND_STATE_STAND);
        }

        if (GetId() != 215769)
            l_Target->SetFullHealth();
    }
    // die at aura end
    else if (l_Target->isAlive() && (!l_Target->HasAura(211317) || l_Target->HasAura(211319)) && GetId() != 215769)  ///< Archbishop Benedictus Restitution
    {
        // call functions which may have additional effects after chainging state of unit
        l_Target->setDeathState(JUST_DIED);
    }

    l_Target->SetControlled(p_Apply, UNIT_STATE_ROOT);

    if (p_Apply)
        l_Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
    else
        l_Target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
}

void AuraEffect::HandleAuraGhost(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
    {
        target->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
        target->m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
        target->m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_GHOST);
    }
    else
    {
        if (target->HasAuraType(SPELL_AURA_GHOST))
            return;

        target->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
        target->m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);
        target->m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);
    }
}

void AuraEffect::HandlePhase(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();


    if (Player* player = target->ToPlayer())
    {
        if (apply)
            player->GetPhaseMgr().RegisterPhasingAuraEffect(this);
        else
            player->GetPhaseMgr().UnRegisterPhasingAuraEffect(this);
    }
    else
    {
        uint32 newPhase = 0;
        Unit::AuraEffectList const& phases = target->GetAuraEffectsByType(SPELL_AURA_PHASE);
        if (!phases.empty())
            for (Unit::AuraEffectList::const_iterator itr = phases.begin(); itr != phases.end(); ++itr)
                newPhase |= (*itr)->GetMiscValue();

        if (!newPhase)
        {
            newPhase = PHASEMASK_NORMAL;
            if (Creature* creature = target->ToCreature())
                if (CreatureData const* data = sObjectMgr->GetCreatureData(creature->GetDBTableGUIDLow()))
                    newPhase = data->phaseMask;
        }

        target->SetPhaseMask(newPhase, false);
    }

    // call functions which may have additional effects after chainging state of unit
    // phase auras normally not expected at BG but anyway better check
    if (apply && (mode & AURA_EFFECT_HANDLE_REAL))
    {
        // drop flag at invisibiliy in bg
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);
    }

    // need triggering visibility update base at phase update of not GM invisible (other GMs anyway see in any phases)
    if (target->IsVisible())
    {
        if (target->IsPlayer() && target->ToPlayer()->IsBeingTeleportedFar())
            return;

        target->UpdateObjectVisibility();
    }
}

/**********************/
/***   UNIT MODEL   ***/
/**********************/

void AuraEffect::HandleAuraModShapeshift(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    uint32 modelid = 0;
    Powers PowerType = POWER_MANA;
    ShapeshiftForm form = ShapeshiftForm(GetMiscValue());

    if (apply)
        target->m_SpellHelper.GetBool(eSpellHelpers::IsShapeshifting) = true;

    if (target->IsPlayer())
    {
        sScriptMgr->OnPlayerChangeShapeshift(target->ToPlayer(), form);
        target->ToPlayer()->EnableCosmetic(!apply);
    }

    if (!(mode & AURA_EFFECT_HANDLE_REAL))
    {
        if (apply)
            target->m_SpellHelper.GetBool(eSpellHelpers::IsShapeshifting) = false;
        return;
    }

    switch (form)
    {
        case FORM_TIGER_STANCE:
        case FORM_OX_STANCE:
        case FORM_CAT_FORM:                                 // 0x01
        case FORM_GHOUL:                                    // 0x07
            PowerType = POWER_ENERGY;
            break;
        case FORM_BEAR_FORM:                                // 0x05
        case FORM_BATTLE_STANCE:                            // 0x11
        case FORM_DEFENSIVE_STANCE:                         // 0x12
        case FORM_BERSERKER_STANCE:                         // 0x13
        case FORM_GLADIATOR_STANCE:                         // 0x21
            PowerType = POWER_RAGE;
            break;
        case FORM_TREE_OF_LIFE:                             // 0x02
        case FORM_TRAVEL_FORM:                              // 0x03
        case FORM_AQUATIC_FORM:                             // 0x04
        case FORM_AMBIENT:                                  // 0x06
        case FORM_THARONJA_SKELETON:                        // 0x0A
        case FORM_DARKMOON_TEST_OF_STRENGTH:                // 0x0B
        case FORM_BLB_PLAYER:                               // 0x0C
        case FORM_SHADOW_DANCE:                             // 0x0D
        case FORM_CREATURE_BEAR:                            // 0x0E
        case FORM_CREATURE_CAT:                             // 0x0F
        case FORM_GHOST_WOLF:                               // 0x10
            break;
        case FORM_CRANE_STANCE:                             // 0x09
        case FORM_SERPENT_STANCE:                           // 0x14
            PowerType = POWER_MANA;
            break;
        case FORM_ZOMBIE:                                   // 0x15
        case FORM_METAMORPHOSIS:                            // 0x16
        case FORM_UNDEAD:                                   // 0x19
        case FORM_FRENZY:                                   // 0x1A
        case FORM_FLIGHT_FORM_EPIC:                         // 0x1B
        case FORM_SHADOWFORM:                               // 0x1C
        case FORM_FLIGHT_FORM:                                   // 0x1D
        case FORM_STEALTH:                                  // 0x1E
        case FORM_MOONKIN_FORM:                                  // 0x1F
        case FORM_SPIRIT_OF_REDEMPTION:                       // 0x20
            break;
        default:
            break;
    }

    modelid = target->GetModelForForm(form);

    if (apply)
    {
        // remove polymorph before changing display id to keep new display id
        switch (form)
        {
            case FORM_CAT_FORM:
            case FORM_TREE_OF_LIFE:
            case FORM_TRAVEL_FORM:
            case FORM_AQUATIC_FORM:
            case FORM_BEAR_FORM:
            case FORM_FLIGHT_FORM_EPIC:
            case FORM_FLIGHT_FORM:
            case FORM_MOONKIN_FORM:
            case FORM_MOONKIN_FORM_AFFINITY:
            {
                if (form == FORM_TREE_OF_LIFE)
                    target->RemoveAurasDueToSpell(114282);

                // remove movement affects
                uint64 mechanicMask = (1LL << MECHANIC_SNARE) | (1LL << MECHANIC_ROOT);
                target->RemoveAurasWithMechanic(mechanicMask);

                // and polymorphic affects
                if (target->IsPolymorphed())
                    target->RemoveAurasDueToSpell(target->getTransForm());

                break;
            }
            case FORM_SPIRIT_OF_REDEMPTION:
                if (target->HasAura(126094)) /// Glyph of the Val'kyr
                    modelid = 24991; /// Need sniff
                break;
            case FORM_GLADIATOR_STANCE:
            case FORM_BATTLE_STANCE:
            case FORM_DEFENSIVE_STANCE:
            case FORM_BERSERKER_STANCE:
                modelid = 0;
            default:
               break;
        }

        // remove other shapeshift before applying a new one
        target->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT, 0, GetBase());
        target->m_SpellHelper.GetBool(eSpellHelpers::IsShapeshifting) = false;

        // stop handling the effect if it was removed by linked event
        if (aurApp->GetRemoveMode())
            return;

        if (modelid > 0)
            target->SetDisplayId(modelid);

        if (PowerType != POWER_MANA)
        {
            int32 oldPower = target->GetPower(PowerType); ///< oldPower is never read 01/18/16
            // reset power to default values only at power change
            if (target->getPowerType() != PowerType)
                target->setPowerType(PowerType);
        }

        // stop handling the effect if it was removed by linked event
        if (aurApp->GetRemoveMode())
            return;

        target->SetShapeshiftForm(form);
    }
    else
    {
        // reset model id if no other auras present
        // may happen when aura is applied on linked event on aura removal
        if (!target->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
        {
            target->SetShapeshiftForm(FORM_NONE);
            if (target->getClass() == CLASS_DRUID)
            {
                target->setPowerType(POWER_MANA);
                // Remove movement impairing effects also when shifting out
                if (target->getClass() != CLASS_DRUID)
                    target->RemoveMovementImpairingAuras();
            }
        }

        if (modelid > 0)
            target->RestoreDisplayId();

        switch (form)
        {
            case FORM_TREE_OF_LIFE:
            case FORM_TRAVEL_FORM:
            case FORM_AQUATIC_FORM:
            case FORM_FLIGHT_FORM_EPIC:
            case FORM_FLIGHT_FORM:
            case FORM_MOONKIN_FORM_AFFINITY:
            {
                // remove movement affects
                uint64 mechanicMask = (1LL << MECHANIC_SNARE) | (1LL << MECHANIC_ROOT);
                target->RemoveAurasWithMechanic(mechanicMask);
                break;
            }
            // Nordrassil Harness - bonus
            case FORM_BEAR_FORM:
            case FORM_CAT_FORM:
            {
                if (AuraEffect* dummy = target->GetAuraEffect(37315, 0))
                    target->CastSpell(target, 37316, true, NULL, dummy);

                // remove movement affects
                uint64 mechanicMask = (1LL << MECHANIC_SNARE) | (1LL << MECHANIC_ROOT);
                target->RemoveAurasWithMechanic(mechanicMask);
                break;
            }
            // Nordrassil Regalia - bonus
            case FORM_MOONKIN_FORM:
            {
                if (AuraEffect* dummy = target->GetAuraEffect(37324, 0))
                    target->CastSpell(target, 37325, true, NULL, dummy);

                target->RemoveAurasDueToSpell(114302); // Astral Form

                // remove movement affects
                uint64 mechanicMask = (1LL << MECHANIC_SNARE) | (1LL << MECHANIC_ROOT);
                target->RemoveAurasWithMechanic(mechanicMask);
                break;
            }
            default:
                break;
        }
    }

    // adding/removing linked auras
    // add/remove the shapeshift aura's boosts
    HandleShapeshiftBoosts(target, apply);

    if (target->IsPlayer())
        target->ToPlayer()->InitDataForForm();

    if (target->getClass() == CLASS_DRUID)
    {
        // Disarm handling
        // If druid shifts while being disarmed we need to deal with that since forms aren't affected by disarm
        // and also HandleAuraModDisarm is not triggered
        if (!target->CanUseAttackType(WeaponAttackType::BaseAttack))
        {
            if (Item* pItem = target->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                target->ToPlayer()->_ApplyWeaponDamage(EQUIPMENT_SLOT_MAINHAND, pItem, apply, pItem->GetScaleIlvl());
        }
    }

    // stop handling the effect if it was removed by linked event
    if (apply && aurApp->GetRemoveMode())
        return;

    if (target->IsPlayer())
    {
        SpellShapeshiftFormEntry const* shapeInfo = sSpellShapeshiftFormStore.LookupEntry(form);
        // Learn spells for shapeshift form - no need to send action bars or add spells to spellbook
        for (uint8 i = 0; i < MAX_SHAPESHIFT_SPELLS; ++i)
        {
            if (!shapeInfo || !shapeInfo->PresetSpellID[i])
                continue;

            if (apply)
                target->ToPlayer()->AddTemporarySpell(shapeInfo->PresetSpellID[i]);
            else
                target->ToPlayer()->RemoveTemporarySpell(shapeInfo->PresetSpellID[i]);
        }

        // Update the Mastery percentage for Shapeshift
        target->ToPlayer()->UpdateMasteryPercentage();
    }
}

void AuraEffect::HandleAuraTransform(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    if (p_Apply)
    {
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_POLYMORPH, LossOfControlType::TypePacifySilence);

        // update active transform spell only when transform or shapeshift not set or not overwriting negative by positive case
        if ((l_Target->GetModelForForm(l_Target->GetShapeshiftForm()) == l_Target->GetNativeDisplayId()) || !GetSpellInfo()->IsPositive())
        {
            // special case (spell specific functionality)
            if (GetMiscValue() == 0)
            {
                switch (GetId())
                {
                    // Orb of Deception
                    case 16739:
                    {
                        if (l_Target->GetTypeId() != TYPEID_PLAYER)
                            break;

                        switch (l_Target->getRace())
                        {
                            // Blood Elf
                            case RACE_BLOODELF:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 17829 : 17830);
                                break;
                            // Orc
                            case RACE_ORC:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 10139 : 10140);
                                break;
                            // Troll
                            case RACE_TROLL:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 10135 : 10134);
                                break;
                            // Tauren
                            case RACE_TAUREN:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 10136 : 10147);
                                break;
                            // Undead
                            case RACE_UNDEAD_PLAYER:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 10146 : 10145);
                                break;
                            // Draenei
                            case RACE_DRAENEI:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 17827 : 17828);
                                break;
                            // Dwarf
                            case RACE_DWARF:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 10141 : 10142);
                                break;
                            // Gnome
                            case RACE_GNOME:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 10148 : 10149);
                                break;
                            // Human
                            case RACE_HUMAN:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 10137 : 10138);
                                break;
                            // Night Elf
                            case RACE_NIGHTELF:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 10143 : 10144);
                                break;
                            /// Todo
                            case RACE_NIGHTBORNE:
                            case RACE_HIGHMOUNTAIN_TAUREN:
                            case RACE_VOID_ELF:
                            case RACE_LIGHTFORGED_DRAENEI:
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    // Murloc costume
                    case 42365:
                        l_Target->SetDisplayId(21723);
                        break;
                    // Dread Corsair
                    case 50517:
                    // Corsair Costume
                    case 51926:
                    {
                        if (l_Target->GetTypeId() != TYPEID_PLAYER)
                            break;

                        switch (l_Target->getRace())
                        {
                            // Blood Elf
                            case RACE_BLOODELF:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25032 : 25043);
                                break;
                            // Orc
                            case RACE_ORC:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25039 : 25050);
                                break;
                            // Troll
                            case RACE_TROLL:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25041 : 25052);
                                break;
                            // Tauren
                            case RACE_TAUREN:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25040 : 25051);
                                break;
                            // Undead
                            case RACE_UNDEAD_PLAYER:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25042 : 25053);
                                break;
                            // Draenei
                            case RACE_DRAENEI:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25033 : 25044);
                                break;
                            // Dwarf
                            case RACE_DWARF:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25034 : 25045);
                                break;
                            // Gnome
                            case RACE_GNOME:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25035 : 25046);
                                break;
                            // Human
                            case RACE_HUMAN:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25037 : 25048);
                                break;
                            // Night Elf
                            case RACE_NIGHTELF:
                                l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 25038 : 25049);
                                break;
                            /// Todo
                            case RACE_NIGHTBORNE:
                            case RACE_HIGHMOUNTAIN_TAUREN:
                            case RACE_VOID_ELF:
                            case RACE_LIGHTFORGED_DRAENEI:
                                break;
                            default:
                                break;
                        }
                        break;
                    }
                    // Pygmy Oil
                    case 53806:
                        l_Target->SetDisplayId(22512);
                        break;
                    // Honor the Dead
                    case 65386:
                    case 65495:
                        l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 29203 : 29204);
                        break;
                    // Darkspear Pride
                    case 75532:
                        l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 31737 : 31738);
                        break;
                    // Gnomeregan Pride
                    case 75531:
                        l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 31654 : 31655);
                        break;
                    /// Rain from Above
                    case 93467:
                    {
                        if (l_Target->getRace() == Races::RACE_NIGHTELF)
                            l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 65312 : 63247);
                        else
                            l_Target->SetDisplayId(l_Target->getGender() == GENDER_MALE ? 67675 : 67673);

                        break;
                    }
                    default:
                        break;
                }
            }
            else
            {
                CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(GetMiscValue());
                if (!ci)
                    l_Target->SetDisplayId(16358);
                else
                {
                    uint32 model_id = 0;

                    if (uint32 modelid = ci->GetRandomValidModelId())
                        model_id = modelid;                     // Will use the default model here

                    // Leyara's Locket
                    if (GetSpellInfo()->Id == 101185)
                        if (Unit* caster = GetCaster())
                            if (caster->getGender() == GENDER_FEMALE)
                                model_id = 38744;

                    l_Target->SetDisplayId(model_id);

                    // Dragonmaw Illusion (set mount model also)
                    if (GetId() == 42016 && l_Target->GetMountID() && !l_Target->GetAuraEffectsByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED).empty())
                        l_Target->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 16314);
                }
            }

            /// Must be apply for Plunder armor - 208535
            if (l_Target->HasAuraType(SPELL_AURA_CLONE_CASTER) && GetId() != 208535)
                l_Target->RemoveAurasByType(SPELL_AURA_CLONE_CASTER);
        }

        // update active transform spell only when transform or shapeshift not set or not overwriting negative by positive case
        SpellInfo const* transformSpellInfo = sSpellMgr->GetSpellInfo(l_Target->getTransForm());
        if (!transformSpellInfo || !GetSpellInfo()->IsPositive() || transformSpellInfo->IsPositive())
        {
            l_Target->setTransForm(GetId());
            Unit* l_Caster = GetCaster();
            if (l_Caster)
                l_Target->m_SpellHelper.GetUint64(eSpellHelpers::PlunderArmor) = GetCaster()->GetGUID();
        }

        // polymorph case
        if ((p_Mode & AURA_EFFECT_HANDLE_REAL) && l_Target->IsPlayer() &&
            (l_Target->IsPolymorphed() || l_Target->HasAuraWithMechanic(1LL << MECHANIC_POLYMORPH)))
        {
            // for players, start regeneration after 1s (in polymorph fast regeneration case)
            // only if caster is Player (after patch 2.4.2)
            if (IS_PLAYER_GUID(GetCasterGUID()))
                l_Target->ToPlayer()->setRegenTimerCount(1*IN_MILLISECONDS);

            //dismount polymorphed target (after patch 2.4.2)
            if (l_Target->IsMounted())
                l_Target->RemoveAurasByType(SPELL_AURA_MOUNTED);

            if (l_Target->HasUnitState(UNIT_STATE_CASTING))
            {
                l_Target->InterruptSpell(CURRENT_GENERIC_SPELL);
                l_Target->InterruptSpell(CURRENT_CHANNELED_SPELL);
                l_Target->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
            }
        }
    }
    else
    {
        // HandleEffect(this, AURA_EFFECT_HANDLE_SEND_FOR_CLIENT, true) will reapply it if need
        if (l_Target->getTransForm() == GetId())
            l_Target->setTransForm(0);

        l_Target->RestoreDisplayId();
        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypePacifySilence);

        // Dragonmaw Illusion (restore mount model)
        if (GetId() == 42016 && l_Target->GetMountID() == 16314)
        {
            if (!l_Target->GetAuraEffectsByType(SPELL_AURA_MOUNTED).empty())
            {
                uint32 cr_id = l_Target->GetAuraEffectsByType(SPELL_AURA_MOUNTED).front()->GetMiscValue();
                if (CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(cr_id))
                {
                    uint32 team = 0;
                    if (l_Target->IsPlayer())
                        team = l_Target->ToPlayer()->GetTeam();

                    uint32 displayID = sObjectMgr->ChooseDisplayId(team, ci);
                    sObjectMgr->GetCreatureModelRandomGender(&displayID);

                    l_Target->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, displayID);
                }
            }
        }
    }

    /// Refresh BoundingRadius and CombatReach
    l_Target->SetObjectScale(l_Target->GetObjectScale());
}

void AuraEffect::HandleAuraModScale(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    int32 l_Amount = GetAmount();

    /// Apply the scale mod only once even if you have multiple stack of the same aura
    switch (m_spellInfo->Id)
    {
        case 193497: ///< Rising Tide (Wrath of Azshara)
        case 204975: ///< Chitinous Exoskeleton (Skorpyron)
        case 225202: ///< Swarm - Scale (Elerethe Renferal)
        case 225259: ///< Growing Egg Sac Cosmetic (Elerethe Renferal)
        case 225047: ///< Shrink (Terrrace Grove Tender)
        case 221419: ///< Unrelenting (Karam Magespear)
            break;
        case 118779: ///< Victory Rush
            if (AuraEffect* l_AuraEffect = target->GetAuraEffect(58104, EFFECT_0))
                l_Amount = l_AuraEffect->GetAmount();
            break;
        case 257470: ///< Eonar's Verdant Embrace
        case 257471: ///< Eonar's Verdant Embrace
        case 257472: ///< Eonar's Verdant Embrace
        case 257473: ///< Eonar's Verdant Embrace
        case 257474: ///< Eonar's Verdant Embrace
        case 257475: ///< Eonar's Verdant Embrace
        case 256836: ///< Norgannon's Command
        {
            l_Amount = m_spellInfo->Effects[SpellEffIndex::EFFECT_1].BasePoints;
            break;
        }
        default:
        {
            if ((mode & AURA_EFFECT_HANDLE_REAL) && target->GetAuraCount(m_spellInfo->Id) > (apply ? 1 : 0))
                return;

            break;
        }
    }

    target->ApplyPercentModFloatValue(OBJECT_FIELD_SCALE, (float)l_Amount, apply);

    float l_Scale = target->GetFloatValue(OBJECT_FIELD_SCALE);
    if (l_Scale > 3)
        sLog->outAshran("ScaleLog: Target Entry : %u, Target GUID : %ul, Caster GUID : %ul, SpellID : %u, Scale : %f", target->GetEntry(), target->GetGUID(), GetBase()->GetCaster() ? GetBase()->GetCaster()->GetGUID() : 0, GetSpellInfo()->Id, l_Scale);

    target->SetObjectScale(target->GetFloatValue(OBJECT_FIELD_SCALE));

    if (apply)
    {
        if (uint32 l_ScaleDur = GetMiscValue())
            target->SetUInt32Value(EUnitFields::UNIT_FIELD_SCALE_DURATION, l_ScaleDur);
    }
    else
        target->SetUInt32Value(EUnitFields::UNIT_FIELD_SCALE_DURATION, 500);
}

void AuraEffect::HandleAuraCloneCaster(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
    {
        Unit* caster = GetCaster();
        if (!caster || caster == target)
            return;

        Unit* l_SimulacrumTarget = sObjectAccessor->GetUnit(*caster, caster->m_SpellHelper.GetUint64(eSpellHelpers::SimulacrumTarget));
        Unit* displayOwner = l_SimulacrumTarget ? l_SimulacrumTarget : caster;

        if (displayOwner->m_SpellHelper.GetBool(eSpellHelpers::MirrorImage))
        {
            Unit* l_IllusionTarget = sObjectAccessor->GetUnit(*displayOwner, displayOwner->m_SpellHelper.GetUint64(eSpellHelpers::MirrorImage));
            if (l_IllusionTarget)
                displayOwner = l_IllusionTarget;
        }

        // What must be cloned? at least display and scale
        target->SetDisplayId(displayOwner->GetDisplayId());

        if (displayOwner->IsPlayer() && target->IsPlayer() && GetId() != 208535)
        {
            if (Item* l_Item = displayOwner->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                target->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, l_Item);
            else
                target->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, nullptr);
            if (Item* l_Item = displayOwner->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                target->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, l_Item);
            else
                target->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, nullptr);

            target->m_SpellHelper.GetBool(eSpellHelpers::MirrorImage) = true;
            target->m_SpellHelper.GetUint64(eSpellHelpers::MirrorImage) = displayOwner->GetGUID();
        }
        else if (GetId() != 208535)
        {
            uint32 l_MainHand = displayOwner->IsPlayer() ? displayOwner->GetUInt32Value(PLAYER_FIELD_VISIBLE_ITEMS + (EQUIPMENT_SLOT_MAINHAND * 2)) : displayOwner->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS);
            uint32 l_OffHand = displayOwner->IsPlayer() ? displayOwner->GetUInt32Value(PLAYER_FIELD_VISIBLE_ITEMS + (EQUIPMENT_SLOT_OFFHAND * 2)) : displayOwner->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 1);
            target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS, l_MainHand);
            target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2, l_OffHand);
        }

        if (!displayOwner->HasAuraType(SPELL_AURA_TRANSFORM))
            target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
    }
    else
    {
        if (target->IsPlayer())
        {
            if (Item* l_Item = target->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                target->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, l_Item);
            else
                target->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_MAINHAND, nullptr);
            if (Item* l_Item = target->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                target->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, l_Item);
            else
                target->ToPlayer()->SetVisibleItemSlot(EQUIPMENT_SLOT_OFFHAND, nullptr);
        }
        else
        {
            target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS, 0);
            target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2, 0);
        }
        target->m_SpellHelper.GetBool(eSpellHelpers::MirrorImage) = false;
        target->m_SpellHelper.GetUint64(eSpellHelpers::MirrorImage) = 0;
        target->SetDisplayId(target->GetNativeDisplayId());
        target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
    }
}

void AuraEffect::HandleAuraInitializeImages(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
    {
        Unit* caster = GetCaster();
        if (!caster || caster == target)
            return;

        target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
    }
    else
        target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_MIRROR_IMAGE);
}

void AuraEffect::HandleAuraEnableBossUnitFrame(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr || GetCaster() == nullptr)
        return;

    WorldPacket l_Data;
    GetCaster()->BuildEncounterFrameData(&l_Data, p_Apply);
    l_Target->SendMessageToSet(&l_Data, true);
}

/************************/
/***      FIGHT       ***/
/************************/

void AuraEffect::HandleFeignDeath(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = aurApp->GetTarget();

    if (l_Target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
    {
        std::pair<bool, bool> l_IsFightWithBoss = std::make_pair(false, false);

        if (Player* l_Player = l_Target->ToPlayer())
        {
            l_IsFightWithBoss = l_Player->IsFightingWithDungeonBoss();
            l_Player->CombatStopWithInstances();
        }
        else
            l_Target->CombatStop();

        l_Target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);

        // prevent interrupt message
        if (GetCasterGUID() == l_Target->GetGUID() && l_Target->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            l_Target->FinishSpell(CURRENT_GENERIC_SPELL, false);
        l_Target->InterruptNonMeleeSpells(true);

        if (l_IsFightWithBoss.first)
        {
            if (l_IsFightWithBoss.second)
                l_Target->getHostileRefManager().deleteReferences();
        }
        else
            l_Target->getHostileRefManager().deleteReferences();

            // stop handling the effect if it was removed by linked event
            if (aurApp->GetRemoveMode())
                return;
            // blizz like 2.0.x
            l_Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
            // blizz like 2.0.x
            l_Target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
            // blizz like 2.0.x
            l_Target->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

        if (!l_IsFightWithBoss.first)
            l_Target->AddUnitState(UNIT_STATE_DIED);
    }
    else
    {
        // blizz like 2.0.x
        l_Target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
        // blizz like 2.0.x
        l_Target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
        // blizz like 2.0.x
        l_Target->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

        l_Target->ClearUnitState(UNIT_STATE_DIED);
    }
}

void AuraEffect::HandleModUnattackable(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
    if (!apply && target->HasAuraType(SPELL_AURA_MOD_UNATTACKABLE))
        return;

    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE, apply);

    // call functions which may have additional effects after chainging state of unit
    if (apply && (mode & AURA_EFFECT_HANDLE_REAL))
    {
        target->CombatStop();
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);
    }
}

void AuraEffect::HandleAuraModDisarm(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    AuraType l_AuraType = GetAuraType();

    //Prevent handling aura twice
    if ((p_Apply) ? l_Target->GetAuraEffectsByType(l_AuraType).size() > 1 : l_Target->HasAuraType(l_AuraType))
        return;

    uint32 l_Field, l_Flag, l_Slot;
    WeaponAttackType l_AttType;
    switch (l_AuraType)
    {
        case SPELL_AURA_MOD_DISARM:
            l_Field   = UNIT_FIELD_FLAGS;
            l_Flag    = UNIT_FLAG_DISARMED;
            l_Slot    = EQUIPMENT_SLOT_MAINHAND;
            l_AttType = WeaponAttackType::BaseAttack;
            break;
        case SPELL_AURA_MOD_DISARM_OFFHAND:
            l_Field   = UNIT_FIELD_FLAGS_2;
            l_Flag    = UNIT_FLAG2_DISARM_OFFHAND;
            l_Slot    = EQUIPMENT_SLOT_OFFHAND;
            l_AttType = WeaponAttackType::OffAttack;
            break;
        case SPELL_AURA_MOD_DISARM_RANGED:
        default:
            return;
    }

    // if disarm aura is to be removed, remove the flag first to reapply damage/aura mods
    if (!p_Apply)
        l_Target->RemoveFlag(l_Field, l_Flag);

    // Handle damage modification, shapeshifted druids are not affected
    if (l_Target->IsPlayer() && !l_Target->IsInFeralForm())
    {
        if (Item* pItem = l_Target->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Slot))
        {
            uint8 attacktype = Player::GetAttackBySlot(l_Slot);

            if (attacktype < WeaponAttackType::MaxAttack)
            {
                l_Target->ToPlayer()->_ApplyWeaponDamage(l_Slot, pItem, !p_Apply, pItem->GetScaleIlvl());
                l_Target->ToPlayer()->_ApplyWeaponDependentAuraMods(pItem, WeaponAttackType(attacktype), !p_Apply);
            }
        }
    }

    // if disarm effects should be applied, wait to set flag until damage mods are unapplied
    if (p_Apply)
        l_Target->SetFlag(l_Field, l_Flag);

    if (l_Target->GetTypeId() == TYPEID_UNIT && l_Target->ToCreature()->GetCurrentEquipmentId())
        l_Target->UpdateDamagePhysical(l_AttType);

    if (p_Apply)
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_DISARM, LossOfControlType::TypeDisarm);
    else
        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypeDisarm);
}

void AuraEffect::HandleAuraModSilence(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    if (p_Apply)
    {
        l_Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_SILENCE, LossOfControlType::TypeSilence);

        bool l_Interrupted = false;
        // call functions which may have additional effects after changing state of unit
        // Stop cast only spells vs PreventionType == SPELL_PREVENTION_TYPE_SILENCE

        bool l_IsImmuneToInterrupt = false;
        SpellImmuneContainer const& l_EffectMap = l_Target->m_spellImmune[SpellImmunity::IMMUNITY_EFFECT];
        for (SpellImmuneContainer::const_iterator l_Iter = l_EffectMap.begin(); l_Iter != l_EffectMap.end(); ++l_Iter)
            if (l_Iter->first == SPELL_EFFECT_INTERRUPT_CAST)
                l_IsImmuneToInterrupt = true;

        if (!l_IsImmuneToInterrupt)
        {
            for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
            {
                if (Spell* spell = l_Target->GetCurrentSpell(CurrentSpellTypes(i)))
                {
                    // Stop spells on prepare or casting state
                    if (spell->m_spellInfo->PreventionType & (SpellPreventionMask::Silence | SpellPreventionMask::Pacify))
                    {
                        l_Target->InterruptSpell(CurrentSpellTypes(i), false);
                        l_Interrupted = true;
                    }
                }
            }
        }

        /// Item - Warlock WoD PvP Affliction 2P Bonus
        if (l_Target->ToPlayer())
            l_Target->ToPlayer()->HandleWarlockWodPvpBonus();

        if (GetCaster() && l_Interrupted)
        {
            if (Spell* spell = GetCaster()->FindCurrentSpellBySpellId(GetId()))
                spell->AppendProcExFlag(PROC_EX_INTERRUPT);
        }
    }
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (l_Target->HasAuraType(SPELL_AURA_MOD_SILENCE) || l_Target->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE))
            return;

        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypeSilence);
        l_Target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
    }
}

void AuraEffect::HandleAuraModPacify(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    if (p_Apply)
    {
        l_Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

        if (m_spellInfo->HasAura(SPELL_AURA_MOD_PACIFY))
            l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_SILENCE, LossOfControlType::TypePacify);
        else if (m_spellInfo->HasAura(SPELL_AURA_MOD_PACIFY_SILENCE))
            l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_SILENCE, LossOfControlType::TypePacifySilence);

        l_Target->AttackStop();
    }
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (l_Target->HasAuraType(SPELL_AURA_MOD_PACIFY) || l_Target->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE))
            return;

        l_Target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

        if (m_spellInfo->HasAura(SPELL_AURA_MOD_PACIFY))
            l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_SILENCE, LossOfControlType::TypePacify);
        else if (m_spellInfo->HasAura(SPELL_AURA_MOD_PACIFY_SILENCE))
            l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_SILENCE, LossOfControlType::TypePacifySilence);
    }
}

void AuraEffect::HandleAuraModPacifyAndSilence(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    // Vengeance of the Blue Flight (TODO: REMOVE THIS!)
    if (m_spellInfo->Id == 45839)
    {
        if (apply)
            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        else
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }
    if (!(apply))
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(SPELL_AURA_MOD_PACIFY_SILENCE))
            return;
    }
    HandleAuraModPacify(aurApp, mode, apply);
    HandleAuraModSilence(aurApp, mode, apply);

    ///< For Hexes and polymorphs with NPCs
    if (GetSpellInfo()->Mechanic == MECHANIC_POLYMORPH && !target->IsPlayer())
    {
        target->SetControlled(apply, UNIT_STATE_CONFUSED);

        if (apply)
            target->SendAddLossOfControl(aurApp, Mechanics::MECHANIC_DISORIENTED, LossOfControlType::TypeConfuse);
        else
            target->SendRemoveLossOfControl(aurApp, LossOfControlType::TypeConfuse);
    }
}

void AuraEffect::HandleAuraAllowOnlyAbility(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->IsPlayer())
    {
        if (apply)
            target->SetFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_ALLOW_ONLY_ABILITY);
        else
        {
            // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
            if (target->HasAuraType(SPELL_AURA_ALLOW_ONLY_ABILITY))
                return;
            target->RemoveFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_ALLOW_ONLY_ABILITY);
        }
    }
}

void AuraEffect::HandleAuraModNoActions(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();

    if (p_Apply)
    {
        l_Target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_NO_ACTIONS);

        // call functions which may have additional effects after chainging state of unit
        // Stop cast only spells vs PreventionType & SPELL_PREVENTION_TYPE_SILENCE
        for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
            if (Spell* l_Spell = l_Target->GetCurrentSpell(CurrentSpellTypes(i)))
                if (l_Spell->m_spellInfo->PreventionType & (SpellPreventionMask::NoActions))
                    // Stop spells on prepare or casting state
                    l_Target->InterruptSpell(CurrentSpellTypes(i), false);
    }
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (l_Target->HasAuraType(SPELL_AURA_MOD_NO_ACTIONS))
            return;

        l_Target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_NO_ACTIONS);
    }
}

void AuraEffect::HandleAuraFogOfWar(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL))
        return;

    p_AurApp->GetTarget()->UpdateObjectVisibility();
}

/****************************/
/***      TRACKING        ***/
/****************************/

void AuraEffect::HandleAuraTrackCreatures(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    target->SetUInt32Value(PLAYER_FIELD_TRACK_CREATURE_MASK, (apply) ? ((uint32)1)<<(GetMiscValue()-1) : 0);
}

void AuraEffect::HandleAuraTrackResources(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    target->SetUInt32Value(PLAYER_FIELD_TRACK_RESOURCE_MASK, (apply) ? ((uint32)1)<<(GetMiscValue()-1): 0);
}

void AuraEffect::HandleAuraTrackStealthed(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!(apply))
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(GetAuraType()))
            return;
    }
    target->ApplyModFlag(PLAYER_FIELD_LOCAL_FLAGS, PLAYER_LOCAL_FLAG_TRACK_STEALTHED, apply);
}

void AuraEffect::HandleAuraModStalked(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    // used by spells: Hunter's Mark, Mind Vision, Syndicate Tracker (MURP) DND
    if (apply)
        target->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (!target->HasAuraType(GetAuraType()))
            target->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT);
    }

    // call functions which may have additional effects after chainging state of unit
    target->UpdateObjectVisibility();
}

void AuraEffect::HandleAuraUntrackable(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
        target->SetByteFlag(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_VIS_FLAG, UNIT_STAND_FLAGS_UNTRACKABLE);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(GetAuraType()))
            return;
        target->RemoveByteFlag(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_VIS_FLAG, UNIT_STAND_FLAGS_UNTRACKABLE);
    }
}

/****************************/
/***  SKILLS & TALENTS    ***/
/****************************/

void AuraEffect::HandleAuraModPetTalentsPoints(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;
}

void AuraEffect::HandleAuraModSkill(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_SKILL)))
        return;

    Player* target = aurApp->GetTarget()->ToPlayer();
    if (!target)
        return;

    uint32 prot = GetMiscValue();
    int32 points = GetAmount();

    if (prot == SKILL_DEFENSE)
        return;

    target->ModifySkillBonus(prot, (apply ? points : -points), GetAuraType() == SPELL_AURA_MOD_SKILL_TALENT);
}

void AuraEffect::HandleAuraModSkillValue(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_SKILL)))
        return;

    Player* target = aurApp->GetTarget()->ToPlayer();
    if (!target)
        return;

    uint32 skill = GetMiscValue();
    int32 points = GetAmount();

    target->ModifySkillBonus(skill, (apply ? points : -points), GetAuraType() == SPELL_AURA_MOD_SKILL_VALUE);
}

void AuraEffect::HandleAuraGainTalent(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    Player* player = target->ToPlayer();
    if (!player)
        return;

    uint32 talentId = GetMiscValue();

    TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);
    if (!talentInfo)
        return;

    if (talentInfo->SpecID && talentInfo->SpecID != player->GetActiveSpecializationID())
        return;

    if (talentInfo->ClassID != player->getClass())
        return;

    if (talentInfo->TierID >= player->GetUInt32Value(PLAYER_FIELD_MAX_TALENT_TIERS))
        return;

    uint32 spellid = talentInfo->SpellID;
    if (!spellid)
    {
        sLog->outError(LOG_FILTER_PLAYER, "AuraEffect::HandleAuraGainTalentt: Talent.db2 has no spellInfo for talent: %u (spell id = 0)", talentId);
        return;
    }

    if (apply)
    {
        if (player->HasAura(spellid))
            return;

        player->CastSpell(player, spellid, true);
    }
    else
        player->RemoveAurasDueToSpell(spellid);
}
/****************************/
/***       MOVEMENT       ***/
/****************************/

void AuraEffect::HandleAuraMounted(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* l_Target = p_AurApp->GetTarget();

    if (p_Apply)
    {
        uint32 l_CreatureEntry = GetMiscValue();
        uint32 l_DisplayId = 0;
        uint32 l_VehicleId = 0;

        // Festive Holiday Mount
        if (l_Target->HasAura(62061))
        {
            if (GetBase()->HasEffectType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
                l_CreatureEntry = 24906;
            else
                l_CreatureEntry = 15665;
        }

        if (CreatureTemplate const* l_CreatureTemplate = sObjectMgr->GetCreatureTemplate(l_CreatureEntry))
        {
            uint32 l_Team = 0;
            if (l_Target->IsPlayer())
                l_Team = l_Target->ToPlayer()->GetTeam();

            l_DisplayId = ObjectMgr::ChooseDisplayId(l_Team, l_CreatureTemplate);
            sObjectMgr->GetCreatureModelRandomGender(&l_DisplayId);

            l_VehicleId = l_CreatureTemplate->VehicleId;

            //some spell has one aura of mount and one of vehicle
            SpellInfo const* l_SpellInfo = GetSpellInfo();
            for (uint32 l_I = 0; l_I < l_SpellInfo->EffectCount; ++l_I)
                if (l_SpellInfo->Effects[l_I].Effect == SPELL_EFFECT_SUMMON
                    && l_SpellInfo->Effects[l_I].MiscValue == GetMiscValue())
                    l_DisplayId = 0;
        }

        for (uint32 l_I = 0; l_I < sMountStore.GetNumRows(); ++l_I)
        {
            auto l_MountEntry = sMountStore.LookupEntry(l_I);
            if (!l_MountEntry || l_MountEntry->SpellID != GetId())
                continue;

            l_DisplayId = GetMountDisplayID(l_Target->ToPlayer(), l_I);

            /// Flag 0x02, no UNIT_FIELD_MOUNT_DISPLAY_ID, guessed
            if (l_MountEntry->Flags & 0x02)
                l_DisplayId = 0;            
            break;
        }

         if (l_DisplayId == 77297) ///< Stormwind Skychaser
            l_VehicleId = 5449;
         else if (l_DisplayId == 77298) ///< Orgrimmar Interceptor
             l_VehicleId = 5684;
        
        l_Target->Mount(l_DisplayId, l_VehicleId, GetMiscValue());

        /// Divine steed is not really a mount
        if (GetMiscValue() != 14565)
            l_Target->RemoveFlagsAuras();

        if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(GetAmount()))
            l_Target->CastSpell(l_Target, mountCapability->SpeedModSpell, true);
    }
    else
    {
        l_Target->Dismount();
        //some mounts like Headless Horseman's Mount or broom stick are skill based spell
        // need to remove ALL aura related to mounts, this will stop client crash with broom stick
        // and never endless flying after using Headless Horseman's Mount
        if (p_Mode & AURA_EFFECT_HANDLE_REAL)
        {
            l_Target->RemoveAurasByType(SPELL_AURA_MOUNTED);

            // remove speed aura
            if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(GetAmount()))
                l_Target->RemoveAurasDueToSpell(mountCapability->SpeedModSpell, l_Target->GetGUID());
        }

        /// Plunder Armor Transmo must be reapply after unmount
        if (l_Target->HasAura(198529))
        {
            Player* l_OldTarget = sObjectAccessor->FindPlayer(l_Target->m_SpellHelper.GetUint64(eSpellHelpers::PlunderArmor));
            if (l_OldTarget)
                l_OldTarget->CastSpell(l_Target, 208535, true);
        }
    }

    if (!sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
    {
        if (GetId() == 180545      ///< Mystic Runesaber
            || GetId() == 163024  ///< Warforged Nightmare
            || GetId() == 171847) ///< Cindermane Charger
        {
            if (p_Apply) /// && (!sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE) || (l_Target->GetAreaId() != 1741 && l_Target->GetAreaId() != 2177)))
                l_Target->CastSpell(l_Target, 86461, true);
            else
                l_Target->RemoveAurasDueToSpell(86461, l_Target->GetGUID());
        }
    }

    l_Target->UpdateSpeed(MOVE_RUN, true);
}

void AuraEffect::HandleAuraAllowFlight(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    /// Fel rush
    /// sometime, the gravity isn't re-applied when the aura is removed so the DH can 'fly' need to find a workaround
    /*if (GetId() == 197922)
    {
        target->SetDisableGravity(apply);
        return;
    }*/

    if (GetId() == 245781)
    {
        target->SetDisableGravity(apply);
        target->SendPlayHoverAnim(apply);
        return;
    }

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(SPELL_AURA_FLY) || target->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || target->HasAuraType(SPELL_AURA_USE_FLIGHT_MODE))
            return;
    }

    target->SetCanFly(apply, !apply);

    if (target->IsPlayer())
        target->ToPlayer()->SendMovementSetCanTransitionBetweenSwimAndFly(apply);

    if (!apply && target->GetTypeId() == TYPEID_UNIT && !target->IsLevitating())
        target->GetMotionMaster()->MoveFall();
}

void AuraEffect::HandleAuraWaterWalk(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(GetAuraType()))
            return;
    }

    if (target->IsPlayer())
    {
        Player* l_Player = target->ToPlayer();
        if (l_Player == nullptr)
            return;

        uint32 l_SpellEntry = GetId();
        switch (l_SpellEntry)
        {
            case 118089:
            case 127271:
            case 127272:
            case 127274:
            case 127278:
            {
                if (l_Player->IsInBattleground())
                    target->SetWaterWalking(false);
                break;
            }
            default:
                target->SetWaterWalking(apply);
                break;
        }
        return;
    }

    target->SetWaterWalking(apply);
}

void AuraEffect::HandleAuraFeatherFall(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(GetAuraType()))
            return;
    }

    target->SetFeatherFall(apply);

    // start fall from current height
    if (!apply && target->IsPlayer())
        target->ToPlayer()->SetFallInformation(0, target->GetPositionZ());
}

void AuraEffect::HandleAuraHover(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(GetAuraType()))
            return;
    }

    target->SetHover(apply);    //! Sets movementflags
}

void AuraEffect::HandleWaterBreathing(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    // update timers in client
    if (target->IsPlayer())
        target->ToPlayer()->UpdateMirrorTimers();
}

void AuraEffect::HandleForceMoveForward(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
        target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVEMENT);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(GetAuraType()))
            return;
        target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVEMENT);
    }
}

void AuraEffect::HandleAllowMoveWhileFalling(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    target->SendCanTurnWhileFalling(apply);
}

/****************************/
/***        THREAT        ***/
/****************************/

void AuraEffect::HandleModThreat(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();
    for (int8 i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if (GetMiscValue() & (1 << i))
            ApplyPercentModFloatVar(target->m_threatModifier[i], float(GetAmount()), apply);
}

void AuraEffect::HandleAuraModTotalThreat(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (!target->isAlive() || target->GetTypeId() != TYPEID_PLAYER)
        return;

    Unit* caster = GetCaster();
    if (caster && caster->isAlive())
        target->getHostileRefManager().addTempThreat((float)GetAmount(), apply);
}

void AuraEffect::HandleModTaunt(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    if (!target->isAlive())
        return;

    Unit* caster = GetCaster();
    if (!caster || !caster->isAlive())
        return;

    if (!target->CanHaveThreatList())
    {
        if (target->getVictim() != caster)
        {
            if (target->GetTypeId() == TYPEID_UNIT && target->ToCreature()->IsAIEnabled
                && (!target->ToCreature()->HasReactState(REACT_PASSIVE) || (target->IsPetGuardianStuff() && IS_PLAYER_GUID(target->GetCharmerOrOwnerGUID()))))
            {
                // taken from case COMMAND_ATTACK:                        //spellid=1792  //ATTACK PetHandler.cpp
                if (CharmInfo* charmInfo = target->GetCharmInfo())
                {
                    target->AttackStop();
                    charmInfo->SetIsCommandAttack(true);
                    charmInfo->SetIsAtStay(false);
                    charmInfo->SetIsFollowing(false);
                    charmInfo->SetIsReturning(false);
                }
                target->ToCreature()->AI()->AttackStart(caster);
            }
        }
        return;
    }

    if (apply)
    {
        target->TauntApply(caster);
        target->getThreatManager().modifyThreatPercent(caster, GetAmount());
    }
    else
    {
        // When taunt aura fades out, mob will switch to previous target if current has less than 1.1 * secondthreat
        target->TauntFadeOut(caster);
    }
}

/*****************************/
/***        CONTROL        ***/
/*****************************/
void AuraEffect::HandleModConfuse(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    l_Target->SetControlled(p_Apply, UNIT_STATE_CONFUSED);

    if (p_Apply)
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_DISORIENTED, LossOfControlType::TypeConfuse);
    else
        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypeConfuse);
}

void AuraEffect::HandleModFear(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    /// Item - Warlock WoD PvP Affliction 2P Bonus
    if (l_Target->ToPlayer())
        l_Target->ToPlayer()->HandleWarlockWodPvpBonus();

    l_Target->SetControlled(p_Apply, UNIT_STATE_FLEEING);

    if (p_Apply)
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_FEAR, LossOfControlType::TypeFear);
    else
        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypeFear);
}

void AuraEffect::HandleAuraModStun(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    switch (m_spellInfo->Id)
    {
        case 117436:// Lightning Prison
            l_Target->RemoveAura(111850);
            break;
        case 127266:// Amber Prison
        case 200166:// Metamorphosis
            if (l_Target->IsPlayer())
                return;
            break;
        default:
            break;
    }

    /// Item - Warlock WoD PvP Affliction 2P Bonus
    if (l_Target->ToPlayer())
        l_Target->ToPlayer()->HandleWarlockWodPvpBonus();

    l_Target->SetControlled(p_Apply, UNIT_STATE_STUNNED);

    if (p_Apply)
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_STUN, LossOfControlType::TypeStunMechanic);
    else
        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypeStunMechanic);
}

void AuraEffect::HandleAuraModRoot(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    l_Target->SetControlled(p_Apply, UNIT_STATE_ROOT);

    if (p_Apply)
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_ROOT, LossOfControlType::TypeRoot);
    else
        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypeRoot);
}

void AuraEffect::HandlePreventFleeing(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    if (l_Target->isFeared())
        l_Target->SetControlled(!(p_Apply), UNIT_STATE_FLEEING);
}

/***************************/
/***        CHARM        ***/
/***************************/
void AuraEffect::HandleModPossess(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    Unit* caster = GetCaster();

    // no support for posession AI yet
    if (caster && caster->GetTypeId() == TYPEID_UNIT)
    {
        HandleModCharm(p_AurApp, p_Mode, p_Apply);
        return;
    }

    if (p_Apply)
    {
        l_Target->SetCharmedBy(caster, CHARM_TYPE_POSSESS, p_AurApp);
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_CHARM, LossOfControlType::TypePossess);
    }
    else
    {
        l_Target->RemoveCharmedBy(caster);
        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypePossess);
    }
}

void AuraEffect::HandleModCharm(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    Unit* caster = GetCaster();

    if (p_Apply)
    {
        l_Target->SetCharmedBy(caster, CHARM_TYPE_CHARM, p_AurApp);
        l_Target->SendAddLossOfControl(p_AurApp, Mechanics::MECHANIC_CHARM, LossOfControlType::TypePossess);
    }
    else
    {
        l_Target->RemoveCharmedBy(caster);
        l_Target->SendRemoveLossOfControl(p_AurApp, LossOfControlType::TypePossess);
    }
}

void AuraEffect::HandleCharmConvert(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    Unit* caster = GetCaster();

    if (apply)
        target->SetCharmedBy(caster, CHARM_TYPE_CONVERT, aurApp);
    else
        target->RemoveCharmedBy(caster);
}

/**
 * Such auras are applied from a caster(=player) to a vehicle.
 * This has been verified using spell #49256
 */
void AuraEffect::HandleAuraControlVehicle(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (!target->IsVehicle())
        return;

    Unit* caster = GetCaster();

    if (!caster || caster == target)
        return;

    if (apply)
        caster->_EnterVehicle(target->GetVehicleKit(), m_amount - 1, aurApp);
    else
    {
        if (GetId() == 53111) // Devour Humanoid
        {
            target->Kill(caster);
            if (caster->GetTypeId() == TYPEID_UNIT)
                caster->ToCreature()->RemoveCorpse();
        }
        caster->_ExitVehicle();
        // some SPELL_AURA_CONTROL_VEHICLE auras have a dummy effect on the player - remove them
        caster->RemoveAurasDueToSpell(GetId());
    }
}

/*********************************************************/
/***                  MODIFY SPEED                     ***/
/*********************************************************/
void AuraEffect::HandleAuraModIncreaseSpeed(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    if (GetAuraType() == SPELL_AURA_INCREASE_MIN_SWIM_SPEED)
    {
        target->UpdateSpeed(MOVE_SWIM, false);
        return;
    }

    target->UpdateSpeed(MOVE_RUN, false);
    target->UpdateSpeed(MOVE_WALK, false);

    if (GetAuraType() == SPELL_AURA_MOD_MINIMUM_SPEED)
    {
        target->UpdateSpeed(MOVE_RUN_BACK, false);
        target->UpdateSpeed(MOVE_FLIGHT, false);
    }
}

void AuraEffect::HandleAuraModIncreaseMountedSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    HandleAuraModIncreaseSpeed(aurApp, mode, apply);
}

void AuraEffect::HandleAuraModIncreaseFlightSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    //! Update ability to fly
    if (GetAuraType() == SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED || GetAuraType() == SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK && (apply || (!target->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) && !target->HasAura(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED) && !target->HasAuraType(SPELL_AURA_FLY))))
        {
            target->SetCanFly(apply);

            if (target->IsPlayer())
                target->ToPlayer()->SendMovementSetCanTransitionBetweenSwimAndFly(apply);

            if (!apply && target->GetTypeId() == TYPEID_UNIT && !target->IsLevitating())
                target->GetMotionMaster()->MoveFall();
        }

        //! Someone should clean up these hacks and remove it from this function. It doesn't even belong here.
        if (mode & AURA_EFFECT_HANDLE_REAL)
        {
            /// Players on flying mounts must be immune to polymorph (only above the ground)
            if (target->IsPlayer())
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);

            // Dragonmaw Illusion (overwrite mount model, mounted aura already applied)
            if (apply && target->HasAuraEffect(42016, 0) && target->GetMountID())
                target->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 16314);
        }
    }

    if (mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK)
        target->UpdateSpeed(MOVE_FLIGHT, true);
}

void AuraEffect::HandleAuraModIncreaseSwimSpeed(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    target->UpdateSpeed(MOVE_SWIM, true);
}

void AuraEffect::HandleAuraModDecreaseSpeed(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    target->UpdateSpeed(MOVE_RUN, true);
    target->UpdateSpeed(MOVE_SWIM, true);
    target->UpdateSpeed(MOVE_FLIGHT, true);
    target->UpdateSpeed(MOVE_RUN_BACK, true);
    target->UpdateSpeed(MOVE_SWIM_BACK, true);
    target->UpdateSpeed(MOVE_FLIGHT_BACK, true);
}

void AuraEffect::HandleAuraModUseNormalSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    target->UpdateSpeed(MOVE_RUN,  true);
    target->UpdateSpeed(MOVE_RUN_BACK, true);
    target->UpdateSpeed(MOVE_WALK, true);
    target->UpdateSpeed(MOVE_SWIM, true);
    target->UpdateSpeed(MOVE_FLIGHT,  true);
}

void AuraEffect::HandleAuraModMovementForceSpeed(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr)
        return;

    l_Target->UpdateMovementForceSpeed();
}

/*********************************************************/
/***                     IMMUNITY                      ***/
/*********************************************************/

void AuraEffect::HandleModStateImmunityMask(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    std::list<AuraType> aura_immunity_list;
    uint64 mechanic_immunity_list = 0;
    int32 miscVal = GetMiscValue();

    /// Without disarm/banish/knockback
    auto applyLosOfControlImmunities = [&]() -> void
    {
        mechanic_immunity_list = (1LL << MECHANIC_SNARE) | (1LL << MECHANIC_ROOT)
            | (1LL << MECHANIC_FEAR) | (1LL << MECHANIC_STUN)
            | (1LL << MECHANIC_SLEEP) | (1LL << MECHANIC_CHARM)
            | (1LL << MECHANIC_SAPPED) | (1LL << MECHANIC_HORROR)
            | (1LL << MECHANIC_POLYMORPH) | (1LL << MECHANIC_DISORIENTED)
            | (1LL << MECHANIC_FREEZE) | (1LL << MECHANIC_TURN);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SNARE, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_ROOT, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FEAR, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_STUN, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SLEEP, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_CHARM, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SAPPED, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_HORROR, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FREEZE, apply);
        target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_TURN, apply);
        aura_immunity_list.push_back(SPELL_AURA_MOD_STUN);
        aura_immunity_list.push_back(SPELL_AURA_MOD_DECREASE_SPEED);
        aura_immunity_list.push_back(SPELL_AURA_MOD_ROOT);
        aura_immunity_list.push_back(SPELL_AURA_MOD_ROOT_2);
        aura_immunity_list.push_back(SPELL_AURA_MOD_CONFUSE);
        aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR);
        aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR_2);
    };

    switch (miscVal)
    {
        case 27:
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SILENCE, apply);
            aura_immunity_list.push_back(SPELL_AURA_MOD_SILENCE);
            break;
        case 96:
        case 2129:
        case 1615:
        {
            if (GetAmount())
            {
                applyLosOfControlImmunities();
            }
            break;
        }
        case 679:
        case 1921:
        {
            if (GetId() == 57742 || GetId() == 115018)
            {
                applyLosOfControlImmunities();
            }
            break;
        }
        case 1557:
        case 1984: ///< Purification - Fel-Powered Purifier (Eonar the Life-binder)
        {
            if (GetId() == 64187)
            {
                mechanic_immunity_list = (1LL << MECHANIC_STUN);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_STUN, apply);
                aura_immunity_list.push_back(SPELL_AURA_MOD_STUN);
            }
            else
            {
                applyLosOfControlImmunities();
            }
            break;
        }
        case 1614:
        case 1694:
        {
            target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, apply);
            aura_immunity_list.push_back(SPELL_AURA_MOD_TAUNT);
            break;
        }
        case 1630:
        {
            if (!GetAmount())
            {
                target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, apply);
                aura_immunity_list.push_back(SPELL_AURA_MOD_TAUNT);
            }
            else
            {
                applyLosOfControlImmunities();
            }
            break;
        }
        case 477:
        case 1733:
        {
            if (!GetAmount())
            {
                applyLosOfControlImmunities();

                mechanic_immunity_list |= (1LL << MECHANIC_BANISH) | (1LL << MECHANIC_DISARM);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_BANISH, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_DISARM, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, apply);
                aura_immunity_list.push_back(SPELL_AURA_MOD_DISARM);
                aura_immunity_list.push_back(SPELL_AURA_MOD_DISARM_OFFHAND);
                aura_immunity_list.push_back(SPELL_AURA_MOD_DISARM_RANGED);
            }
            break;
        }
        case 878:
        {
            if (GetAmount() == 1)
            {
                mechanic_immunity_list = (1LL << MECHANIC_SNARE) | (1LL << MECHANIC_STUN)
                    | (1LL << MECHANIC_DISORIENTED) | (1LL << MECHANIC_FREEZE);

                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SNARE, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_STUN, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FREEZE, apply);
                aura_immunity_list.push_back(SPELL_AURA_MOD_STUN);
                aura_immunity_list.push_back(SPELL_AURA_MOD_DECREASE_SPEED);
            }
            break;
        }
        case 1701:
        {
            /// Bolstering (mythic+)
            if (GetId() == 209859)
            {
                mechanic_immunity_list = (1LL << MECHANIC_FEAR) | (1LL << MECHANIC_STUN)
                    | (1LL << MECHANIC_SLEEP) | (1LL << MECHANIC_CHARM)
                    | (1LL << MECHANIC_SAPPED) | (1LL << MECHANIC_HORROR)
                    | (1LL << MECHANIC_POLYMORPH) | (1LL << MECHANIC_DISORIENTED)
                    | (1LL << MECHANIC_FREEZE) | (1LL << MECHANIC_TURN) | (1 << MECHANIC_SILENCE);

                aura_immunity_list.push_back(SPELL_AURA_MOD_SILENCE);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SILENCE, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FEAR, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SLEEP, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_CHARM, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SAPPED, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_HORROR, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FREEZE, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_TURN, apply);
                aura_immunity_list.push_back(SPELL_AURA_TRANSFORM);
                aura_immunity_list.push_back(SPELL_AURA_MOD_CONFUSE);
                aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR);
                aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR_2);
            }
            break;
        }
        case 1887:
        {
            // Frost Pillar
            if (GetId() == 51271)
            {
                target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_JUMP_DEST, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_ID, 49560, apply);   // Glyph of Resilient Grip hack
                miscVal = 0;

                // Glyph of Frost Pillar
                if (target->HasAura(58635))
                {
                    mechanic_immunity_list = (1LL << MECHANIC_FEAR) | (1LL << MECHANIC_STUN)
                    | (1LL << MECHANIC_SLEEP) | (1LL << MECHANIC_CHARM)
                    | (1LL << MECHANIC_SAPPED) | (1LL << MECHANIC_HORROR)
                    | (1LL << MECHANIC_POLYMORPH) | (1LL << MECHANIC_DISORIENTED)
                    | (1LL << MECHANIC_FREEZE) | (1LL << MECHANIC_TURN);

                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FEAR, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_STUN, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SLEEP, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_CHARM, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SAPPED, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_HORROR, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FREEZE, apply);
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_TURN, apply);
                    aura_immunity_list.push_back(SPELL_AURA_MOD_STUN);
                    aura_immunity_list.push_back(SPELL_AURA_TRANSFORM);
                    aura_immunity_list.push_back(SPELL_AURA_MOD_CONFUSE);
                    aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR);
                    aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR_2);

                    // Apply Root !
                    if (apply)
                        target->CastSpell(target, 90259, true);
                    else
                        target->RemoveAura(90259);
                }
            }

            break;
        }
        case 1912: // Catalyzing Presence - Aggramar
        {
            std::vector<uint32> l_Mechanics;

            for (uint32 l_Idx = 1; l_Idx < Mechanics::MAX_MECHANIC; ++l_Idx)
            {
                if (miscVal & (1 << Mechanics(l_Idx - 1)))
                    l_Mechanics.push_back(l_Idx);
            }

            l_Mechanics.insert(l_Mechanics.begin(),
                {
                    MECHANIC_SNARE,
                    MECHANIC_ROOT,
                    MECHANIC_FEAR,
                    MECHANIC_STUN,
                    MECHANIC_SLEEP,
                    MECHANIC_CHARM,
                    MECHANIC_SAPPED,
                    MECHANIC_HORROR,
                    MECHANIC_POLYMORPH,
                    MECHANIC_DISORIENTED,
                    MECHANIC_FREEZE,
                    MECHANIC_TURN,
                    MECHANIC_BANISH,
                    MECHANIC_DISARM,
                    MECHANIC_SHACKLE
                });

            for (const uint32 l_Itr : l_Mechanics)
            {
                mechanic_immunity_list |= (1LL << l_Itr);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, l_Itr, apply);
            }

            aura_immunity_list =
            {
                SPELL_AURA_MOD_STUN,
                SPELL_AURA_MOD_CONFUSE,
                SPELL_AURA_MOD_FEAR,
                SPELL_AURA_MOD_FEAR_2,
                SPELL_AURA_MOD_ROOT,
                SPELL_AURA_MOD_ROOT_2,
                SPELL_AURA_TRANSFORM
            };

            break;
        }
        case 2071:
        {
            /// Rage of the Sleeper
            if (GetId() == 200851 && target->HasAura(200855))
            {
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FEAR, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SLEEP, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_CHARM, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SAPPED, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_HORROR, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FREEZE, apply);
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_TURN, apply);
                aura_immunity_list.push_back(SPELL_AURA_MOD_STUN);
                aura_immunity_list.push_back(SPELL_AURA_TRANSFORM);
                aura_immunity_list.push_back(SPELL_AURA_MOD_CONFUSE);
                aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR);
                aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR_2);
                aura_immunity_list.push_back(SPELL_AURA_MOD_ROOT);
                aura_immunity_list.push_back(SPELL_AURA_MOD_ROOT_2);
            }
            break;
        }
        default:
            break;
    }

    if (aura_immunity_list.empty())
    {
        if (miscVal & (1 << 10))
            aura_immunity_list.push_back(SPELL_AURA_MOD_STUN);

        if (miscVal & (1 << 1))
            aura_immunity_list.push_back(SPELL_AURA_TRANSFORM);

        // These flag can be recognized wrong:
        if (miscVal & (1 << 6))
            aura_immunity_list.push_back(SPELL_AURA_MOD_DECREASE_SPEED);

        if (miscVal & (1 << 0))
        {
            aura_immunity_list.push_back(SPELL_AURA_MOD_ROOT);
            aura_immunity_list.push_back(SPELL_AURA_MOD_ROOT_2);
        }

        if (miscVal & (1 << 2))
            aura_immunity_list.push_back(SPELL_AURA_MOD_CONFUSE);

        if (miscVal & (1 << 9))
        {
            aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR);
            aura_immunity_list.push_back(SPELL_AURA_MOD_FEAR_2);
        }

        if (miscVal & (1 << 7))
            aura_immunity_list.push_back(SPELL_AURA_MOD_DISARM);
    }

    switch (GetId())
    {
        case 198144: ///< Ice Form
            target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, apply);
            target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, apply);
            target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_JUMP_DEST, apply);
            break;
        case 240777: ///< Feet of Wind
            aura_immunity_list.remove(SPELL_AURA_MOD_STUN);
            break;
    }

    // apply immunities
    for (std::list <AuraType>::iterator iter = aura_immunity_list.begin(); iter != aura_immunity_list.end(); ++iter)
        target->ApplySpellImmune(GetId(), IMMUNITY_STATE, *iter, apply);

    if (apply && GetSpellInfo()->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
    {
        target->RemoveAurasWithMechanic(mechanic_immunity_list, AURA_REMOVE_BY_DEFAULT, GetId());
        for (std::list <AuraType>::iterator iter = aura_immunity_list.begin(); iter != aura_immunity_list.end(); ++iter)
            target->RemoveAurasByType(*iter);
    }
}

void AuraEffect::HandleModMechanicImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    uint64 mechanic = 0;

    switch (GetId())
    {
        case 42292: ///< PvP trinket
        case 65547: ///< PvP trinket (Trial of Crusader)
        case 53490: ///< Bullheaded (Cunning pet Ability)
        case 195710: ///< Honorable Medallion (Honor Talent)
        case 208683: ///< Gladiator's Medallion (Honor Talent)
        {
            mechanic = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;
            // Actually we should apply immunities here, too, but the aura has only 100 ms duration, so there is practically no point
            break;
        }
        case 54508: // Demonic Empowerment
        {
            mechanic = (1LL << MECHANIC_SNARE) | (1LL << MECHANIC_ROOT) | (1LL << MECHANIC_FEAR) | (1LL << MECHANIC_BANISH);
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SNARE, apply);
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_ROOT, apply);
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_STUN, apply);
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_FEAR, apply);
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_BANISH, apply);
            break;
        }
        case 129020:// Avatar
        {
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_SNARE, apply);
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_ROOT, apply);
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, MECHANIC_DAZE, apply);
            break;
        }
        default:
        {
            if (GetMiscValue() < 1)
                return;
            mechanic = 1LL << GetMiscValue();
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, GetMiscValue(), apply);
            break;
        }
    }

    // Blizz uses 100 duration for breaking out of mechanic without the attributes
    if (apply && (GetSpellInfo()->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY || aurApp->GetBase()->GetDuration() <= 100))
        target->RemoveAurasWithMechanic(mechanic, AURA_REMOVE_BY_DEFAULT, GetId());
}

void AuraEffect::HandleAuraModEffectImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, GetMiscValue(), apply);

    // when removing flag aura, handle flag drop
    if (!apply && target->IsPlayer()
        && (GetSpellInfo()->AuraInterruptFlags & AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION))
    {
        if (target->IsPlayer())
        {
            if (target->ToPlayer()->InBattleground())
            {
                if (Battleground* bg = target->ToPlayer()->GetBattleground())
                    bg->EventPlayerDroppedFlag(target->ToPlayer());
            }
            else
                sOutdoorPvPMgr->HandleDropFlag((Player*)target, GetSpellInfo()->Id);
        }
    }
}

void AuraEffect::HandleAuraModStateImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    target->ApplySpellImmune(GetId(), IMMUNITY_STATE, GetMiscValue(), apply);

    if (apply && GetSpellInfo()->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY)
        target->RemoveAurasByType(AuraType(GetMiscValue()), 0, GetBase());
}

void AuraEffect::HandleAuraModSchoolImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    target->ApplySpellImmune(GetId(), IMMUNITY_SCHOOL, GetMiscValue(), (apply));

    if (GetSpellInfo()->Mechanic == MECHANIC_BANISH)
    {
        if (apply)
            target->AddUnitState(UNIT_STATE_ISOLATED);
        else
        {
            bool banishFound = false;
            Unit::AuraEffectList const& banishAuras = target->GetAuraEffectsByType(GetAuraType());
            for (Unit::AuraEffectList::const_iterator i = banishAuras.begin(); i != banishAuras.end(); ++i)
            {
                if ((*i)->GetSpellInfo()->Mechanic == MECHANIC_BANISH)
                {
                    banishFound = true;
                    break;
                }
            }

            if (!banishFound)
                target->ClearUnitState(UNIT_STATE_ISOLATED);
        }
    }

    if (apply && GetMiscValue() == SPELL_SCHOOL_MASK_NORMAL)
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);

    // remove all flag auras (they are positive, but they must be removed when you are immune)
    if (GetSpellInfo()->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY
        && GetSpellInfo()->AttributesEx2 & SPELL_ATTR2_DAMAGE_REDUCED_SHIELD)
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);

    // TODO: optimalize this cycle - use RemoveAurasWithInterruptFlags call or something else
    if ((apply)
        && GetSpellInfo()->AttributesEx & SPELL_ATTR1_DISPEL_AURAS_ON_IMMUNITY
        && GetSpellInfo()->IsPositive())                       //Only positive immunity removes auras
    {
        uint32 school_mask = GetMiscValue();
        Unit::AuraApplicationMap& Auras = target->GetAppliedAuras();
        for (Unit::AuraApplicationMap::iterator iter = Auras.begin(); iter != Auras.end();)
        {
            SpellInfo const* spell = iter->second->GetBase()->GetSpellInfo();
            if ((spell->GetSchoolMask() & school_mask)//Check for school mask
                && GetSpellInfo()->CanDispelAura(spell)
                && !iter->second->IsPositive()          //Don't remove positive spells
                && spell->Id != GetId())               //Don't remove self
            {
                target->RemoveAura(iter);
            }
            else
                ++iter;
        }
    }
}

void AuraEffect::HandleAuraModDmgImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    target->ApplySpellImmune(GetId(), IMMUNITY_DAMAGE, GetMiscValue(), apply);
}

void AuraEffect::HandleAuraModDispelImmunity(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    target->ApplySpellDispelImmunity(m_spellInfo, DispelType(GetMiscValue()), (apply));
}

/*********************************************************/
/***                  MODIFY STATS                     ***/
/*********************************************************/

/********************************/
/***        RESISTANCE        ***/
/********************************/

void AuraEffect::HandleAuraModResistance(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    for (int8 x = SPELL_SCHOOL_NORMAL; x < MAX_SPELL_SCHOOL; x++)
    {
        if (GetMiscValue() & int32(1<<x))
        {
            target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + x), TOTAL_VALUE, float(GetAmount()), apply);
            if (target->IsPlayer() || target->ToCreature()->isPet())
                target->ApplyResistanceBuffModsMod(SpellSchools(x), GetAmount() > 0, (float)GetAmount(), apply);
        }
    }
}

void AuraEffect::HandleAuraModBaseResistancePCT(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* l_Target = aurApp->GetTarget();

    // only players have base stats
    if (!l_Target->IsPlayer())
    {
        //pets only have base armor
        if (l_Target->ToCreature()->isPet() && (GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL))
            l_Target->HandleStatModifier(UNIT_MOD_ARMOR, BASE_PCT, static_cast<float>(GetAmount()), apply);
    }
    else
    {
        for (int8 l_I = SPELL_SCHOOL_NORMAL; l_I < MAX_SPELL_SCHOOL; l_I++)
        {
            if (GetMiscValue() & static_cast<int32>(1 << l_I))
                l_Target->HandleStatModifier(static_cast<UnitMods>(UNIT_MOD_RESISTANCE_START + l_I), BASE_PCT, static_cast<float>(GetAmount()), apply);
        }
    }
}

void AuraEffect::HandleModCooldownRecoveryRate(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & (AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL | AuraEffectHandleModes::AURA_EFFECT_HANDLE_CHANGE_AMOUNT)))
        return;

    if (!p_AurApp->GetTarget() || !p_AurApp->GetTarget()->IsPlayer())
        return;

    Player* l_Target = p_AurApp->GetTarget()->ToPlayer();

    ACE_UINT64 l_CurrTime = 0;
    ACE_OS::gettimeofday().msec(l_CurrTime);

    float l_Mod = 1.0f;

    if (p_Apply)
    {
        if (GetAmount() > 0.0f)
            l_Mod *= 100.0f / (100.0f + float(GetAmount()));
        else
            l_Mod *= (100.0f - float(GetAmount())) / 100.0f;
    }
    else
    {
        if (GetAmount() > 0.0f)
            l_Mod /= 100.0f / (100.0f + float(GetAmount()));
        else
            l_Mod /= (100.0f - float(GetAmount())) / 100.0f;
    }

    auto const l_List = sSpellMgr->GetChangeRecoveryRateSpells(m_spellInfo->Id);

    auto const l_Cooldowns = l_Target->GetSpellCooldownMap();
    for (auto l_Iter : l_Cooldowns)
    {
        if (l_Iter.first == m_spellInfo->Id)
            continue;

        /// Spell should not be affected
        if (!l_List.empty() && l_List.find(l_Iter.first) == l_List.end())
            continue;

        if (l_Iter.second.end > l_CurrTime)
        {
            float l_Modifier = l_Target->GetTotalCooldownRecoveryRateForSpell(l_Iter.first, p_Apply ? this : nullptr, p_Apply ? nullptr : this);

            l_Target->SendModifyCooldownRecoveryRate(l_Iter.first, l_Mod, l_Modifier);

            uint32 l_Cooldown = (l_Iter.second.end - l_CurrTime) * l_Mod;

            l_Target->AddSpellCooldown(l_Iter.first, 0, l_Cooldown);
        }
    }

    for (auto& l_Iter : l_Target->GetChargeCooldowns())
    {
        if (l_Iter.second.empty())
            continue;

        /// Spell should not be affected
        if (!l_List.empty())
        {
            bool l_Found = false;

            for (int32 const& l_Value : l_List)
            {
                if (-int32(l_Iter.first) == l_Value)
                {
                    l_Found = true;
                    break;
                }
            }

            if (!l_Found)
                continue;
        }

        float l_Modifier = l_Target->GetTotalChargeRecoveryRateForCategory(l_Iter.first, p_Apply ? this : nullptr, p_Apply ? nullptr : this);

        l_Target->SendModifyChargeRecoveryRate(l_Iter.first, l_Mod, l_Modifier);

        Clock::time_point l_Now = Clock::now();

        std::deque<ChargeEntry>& l_ChargeRefreshTimes = l_Iter.second;

        SpellCategoryEntry const* l_CategoryEntry = sSpellCategoryStore.LookupEntry(l_Iter.first);
        if (!l_CategoryEntry)
            continue;

        Clock::time_point l_LastChargeEnd;
        uint32 l_Index = 0;
        for (ChargeEntry& l_Charge : l_ChargeRefreshTimes)
        {
            std::chrono::milliseconds l_LeftChargeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(l_Charge.RechargeEnd - (l_Index ? l_Charge.RechargeStart : l_Now));
            if (l_Index)
            {
                l_Charge.RechargeStart = l_LastChargeEnd;
                l_Charge.RechargeEnd = l_Charge.RechargeStart;
            }
            else
                l_Charge.RechargeEnd -= l_LeftChargeDuration;

            int32 l_DurationAsInteger = std::max(0, (int32)l_LeftChargeDuration.count());
            l_DurationAsInteger = static_cast<float>(l_DurationAsInteger) * l_Mod;
            l_LeftChargeDuration = std::chrono::milliseconds(l_DurationAsInteger);

            l_Charge.RechargeEnd += l_LeftChargeDuration;
            l_LastChargeEnd = l_Charge.RechargeEnd;

            l_Index++;
        }
    }
}

void AuraEffect::HandleModResistancePercent(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    for (int8 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; i++)
    {
        if (GetMiscValue() & int32(1<<i))
        {
            target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_PCT, float(GetAmount()), apply);
            if (target->IsPlayer() || target->ToCreature()->isPet())
            {
                target->ApplyResistanceBuffModsPercentMod(SpellSchools(i), true, (float)GetAmount(), apply);
                target->ApplyResistanceBuffModsPercentMod(SpellSchools(i), false, (float)GetAmount(), apply);
            }
        }
    }
}

void AuraEffect::HandleModBaseResistance(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    // only players have base stats
    if (target->GetTypeId() != TYPEID_PLAYER)
    {
        //only pets have base stats
        if (target->ToCreature()->isPet() && (GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL))
            target->HandleStatModifier(UNIT_MOD_ARMOR, TOTAL_VALUE, float(GetAmount()), apply);
    }
    else
    {
        for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; i++)
            if (GetMiscValue() & (1<<i))
                target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), GetSpellInfo()->IsPvPStatTemplate() ? BASE_VALUE : TOTAL_VALUE, float(GetAmount()), apply);
    }
}

void AuraEffect::HandleModTargetResistance(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    // applied to damage as HandleNoImmediateEffect in Unit::CalcAbsorbResist and Unit::CalcArmorReducedDamage

    // show armor penetration
    if (target->IsPlayer() && (GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL))
        target->ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE, GetAmount(), apply);

    // show as spell penetration only full spell penetration bonuses (all resistances except armor and holy
    if (target->IsPlayer() && (GetMiscValue() & SPELL_SCHOOL_MASK_SPELL) == SPELL_SCHOOL_MASK_SPELL)
        target->ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_RESISTANCE, GetAmount(), apply);
}

/********************************/
/***           STAT           ***/
/********************************/

void AuraEffect::HandleAuraModStat(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (GetMiscValue() < -2 || GetMiscValue() > 4)
        return;

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; i++)
    {
        // -1 or -2 is all stats (misc < -2 checked in function beginning)
        if (GetMiscValue() < 0 || GetMiscValue() == i)
        {
            //target->ApplyStatMod(Stats(i), m_amount, apply);
            target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, float(GetAmount()), apply);
            if (target->IsPlayer() || target->ToCreature()->isPet())
                target->ApplyStatBuffMod(Stats(i), (float)GetAmount(), apply);
        }
    }
}

void AuraEffect::HandleModPercentStat(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (GetMiscValue() < -1 || GetMiscValue() > 4)
        return;

    // only players have base stats
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        if (GetMiscValue() == i || GetMiscValue() == -1)
            target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), BASE_PCT, float(m_amount), apply);
    }
}

void AuraEffect::HandleModSpellDamagePercentFromStat(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Magic damage modifiers implemented in Unit::SpellDamageBonus
    // This information for client side use only
    // Recalculate bonus
    target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void AuraEffect::HandleModSpellHealingPercentFromStat(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void AuraEffect::HandleAuraModSpellPowerPercent(AuraApplication const * aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit *target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void AuraEffect::HandleAuraMastery(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit *target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    target->ToPlayer()->UpdateMasteryPercentage();
}

void AuraEffect::HandleModSpellDamagePercentFromAttackPower(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Magic damage modifiers implemented in Unit::SpellDamageBonus
    // This information for client side use only
    // Recalculate bonus
    target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void AuraEffect::HandleModSpellHealingPercentFromAttackPower(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void AuraEffect::HandleModHealingDone(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    // implemented in Unit::SpellHealingBonus
    // this information is for client side only
    target->ToPlayer()->UpdateSpellDamageAndHealingBonus();
}

void AuraEffect::HandleModTotalPercentStat(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    // save current health state
    float healthPct = target->GetHealthPct();
    bool alive = target->isAlive();

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; i++)
    {
        if (GetMiscValueB() & 1 << i || !GetMiscValueB()) // 0 is also used for all stats
        {
            if (target->IsPlayer() || target->ToCreature()->isPet())
            {
                float l_CurrentAmount = target->GetTotalAuraMultiplierByMiscBMask(GetAuraType(), 1 << i, apply ? this : nullptr, apply ? nullptr : (AuraEffect*)this);
                float l_NewAmount = target->GetTotalAuraMultiplierByMiscBMask(GetAuraType(), 1 << i, apply ? nullptr : this);

                target->ApplyStatPercentBuffMod(Stats(i), l_CurrentAmount, false);
                target->ApplyStatPercentBuffMod(Stats(i), l_NewAmount, true);
                target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_PCT, l_CurrentAmount, false);
                target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_PCT, l_NewAmount, true);
                target->UpdateStatBuffFields(Stats(i));
            }
            else
                target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_PCT, float(GetAmount()), apply);
        }
    }

    // recalculate current HP/MP after applying aura modifications (only for spells with SPELL_ATTR0_UNK4 0x00000010 flag)
    // this check is total bullshit i think
    if (GetMiscValueB() & 1 << STAT_STAMINA)
        target->SetHealth(std::max<uint32>(uint32(healthPct * target->GetMaxHealth() * 0.01f), (alive ? 1 : 0)));
}

void AuraEffect::HandleAuraModResistenceOfStatPercent(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (GetMiscValue() != SPELL_SCHOOL_MASK_NORMAL)
    {
        // support required adding replace UpdateArmor by loop by UpdateResistence at intellect update
        // and include in UpdateResistence same code as in UpdateArmor for aura mod apply.
        return;
    }

    // Recalculate Armor
    target->UpdateArmor();
}

void AuraEffect::HandleAuraModExpertise(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
}

void AuraEffect::HandleAuraAddModifier(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK)))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (!m_spellmod || !l_Target || !l_Target->IsPlayer())
        return;

    l_Target->ToPlayer()->AddSpellMod(m_spellmod, p_Apply);

    /// Auras with charges do not mod amount of passive auras
    if (GetBase() && GetBase()->IsUsingCharges())
        return;

    int8 l_Effect;
    switch (GetMiscValue())
    {
        case SPELLMOD_ALL_EFFECTS:
            l_Effect = -1;
            break;
        case SPELLMOD_EFFECT1:
            l_Effect = 0;
            break;
        case SPELLMOD_EFFECT2:
            l_Effect = 1;
            break;
        case SPELLMOD_EFFECT3:
            l_Effect = 2;
            break;
        case SPELLMOD_EFFECT4:
            l_Effect = 3;
            break;
        case SPELLMOD_EFFECT5:
            l_Effect = 4;
            break;
        default:
            return;
    }

    /// reapply some passive spells after add/remove related spellmods
    /// Warning: it is a dead loop if 2 auras each other amount-shouldn't happen
    std::vector<Aura*> l_Auras;
    uint64 l_Guid = l_Target->GetGUID();
    Unit::AuraApplicationMap const& l_AppliedAuras = l_Target->GetAppliedAuras();
    for (Unit::AuraApplicationMap::const_iterator l_Iter = l_AppliedAuras.begin(); l_Iter != l_AppliedAuras.end(); ++l_Iter)
    {
        Aura* l_Aura = l_Iter->second->GetBase();
        /// only passive and permament auras-active auras should have amount set on spellcast and not be affected
        /// if aura is casted by others, it will not be affected
        if (!l_Aura || (!l_Aura->IsPassive() && !l_Aura->IsPermanent()) || !l_Aura->GetSpellInfo()->IsAffectedBySpellMod(m_spellmod))
            continue;

        l_Auras.push_back(l_Aura);
    }

    for (Aura* l_Aura : l_Auras)
    {
        if (l_Effect == -1)
        {
            for (uint8 l_I = 0; l_I < 5; l_I++)
            {
                if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(l_I))
                    l_AuraEffect->RecalculateAmount();
            }
        }
        else if (AuraEffect* l_AuraEffect = l_Aura->GetEffect(static_cast<uint8>(l_Effect)))
            l_AuraEffect->RecalculateAmount();
    }
}

/********************************/
/***      HEAL & ENERGIZE     ***/
/********************************/
void AuraEffect::HandleModPowerRegen(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    if (!aurApp->GetTarget() || aurApp->GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* l_Player = aurApp->GetTarget()->ToPlayer();
    switch (GetMiscValue())
    {
        case POWER_MANA:
            l_Player->UpdateManaRegen();
            break;
        case POWER_RUNES:
            l_Player->UpdateAllRunesRegen();
            break;
        case POWER_ENERGY:
            l_Player->UpdateEnergyRegen();
            break;
        case POWER_FOCUS:
            l_Player->UpdateFocusRegen();
            break;
        default:
            // other powers are not immediate effects - implemented in Player::Regenerate, Creature::Regenerate
            break;
    }
}

void AuraEffect::HandleModPowerRegenPCT(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    HandleModPowerRegen(aurApp, mode, apply);
}

void AuraEffect::HandleModManaRegen(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    //Note: an increase in regen does NOT cause threat.
    target->ToPlayer()->UpdateManaRegen();
}

void AuraEffect::HandleAuraModIncreaseHealth(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
    {
        target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(GetAmount()), apply);
        target->ModifyHealth(GetAmount());
    }
    else
    {
        if (int32(target->GetHealth()) > GetAmount())
            target->ModifyHealth(-GetAmount());
        else
            target->SetHealth(1);
        target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(GetAmount()), apply);
    }
}

void AuraEffect::HandleAuraModIncreaseEnergy(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    Powers powerType = Powers(GetMiscValue());
    // do not check power type, we can always modify the maximum
    // as the client will not see any difference
    // also, placing conditions that may change during the aura duration
    // inside effect handlers is not a good idea
    //if (int32(powerType) != GetMiscValue())
    //    return;

    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + powerType);

    target->SetCanModifyStats(true);
    target->HandleStatModifier(unitMod, TOTAL_VALUE, float(GetAmount()), apply);
}

void AuraEffect::HandleAuraModIncreaseEnergyPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    Powers powerType = Powers(GetMiscValue());
    // do not check power type, we can always modify the maximum
    // as the client will not see any difference
    // also, placing conditions that may change during the aura duration
    // inside effect handlers is not a good idea
    //if (int32(powerType) != GetMiscValue())
    //    return;

    target->SetCanModifyStats(true);

    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + powerType);
    float amount = float(GetAmount());

    if (apply)
    {
        target->HandleStatModifier(unitMod, TOTAL_PCT, amount, apply);
        target->ModifyPowerPct(powerType, amount, apply);
    }
    else
    {
        target->ModifyPowerPct(powerType, amount, apply);
        target->HandleStatModifier(unitMod, TOTAL_PCT, amount, apply);
    }
}

void AuraEffect::HandleAuraModIncreaseHealthPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT | AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK)))
        return;

    Unit* target = aurApp->GetTarget();
    Aura* aura = aurApp->GetBase();
    if (!aura)
        return;

    float amount = float(GetAmount());

    /// Unit will keep hp% after MaxHealth being modified if unit is alive.
    float percent = target->GetHealthPct();
    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_PCT, amount, apply);

    if (target->isAlive())
        target->SetHealth(target->CountPctFromMaxHealth(percent));
}

void AuraEffect::HandleAuraIncreaseBaseHealthPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    target->HandleStatModifier(UNIT_MOD_HEALTH, BASE_PCT, float(GetAmount()), apply);
}

/********************************/
/***          FIGHT           ***/
/********************************/

void AuraEffect::HandleAuraModParryPercent(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    target->ToPlayer()->UpdateParryPercentage();
}

void AuraEffect::HandleAuraModDodgePercent(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    target->ToPlayer()->UpdateDodgePercentage();
}

void AuraEffect::HandleAuraModBlockPercent(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    target->ToPlayer()->UpdateBlockPercentage();
}

void AuraEffect::HandleAuraModRegenInterrupt(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    HandleModManaRegen(aurApp, mode, apply);
}

void AuraEffect::HandleAuraModWeaponCritPercent(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    for (int i = 0; i < WeaponAttackType::MaxAttack; ++i)
        if (Item* pItem = target->ToPlayer()->GetWeaponForAttack(WeaponAttackType(i), true))
            target->ToPlayer()->_ApplyWeaponDependentAuraCritMod(pItem, WeaponAttackType(i), this, apply);

    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // GetMiscValue() comparison with item generated damage types

    if (GetSpellInfo()->EquippedItemClass == -1)
    {
        target->ToPlayer()->HandleBaseModValue(CRIT_PERCENTAGE,         FLAT_MOD, float (GetAmount()), apply);
        target->ToPlayer()->HandleBaseModValue(OFFHAND_CRIT_PERCENTAGE, FLAT_MOD, float (GetAmount()), apply);
        target->ToPlayer()->HandleBaseModValue(RANGED_CRIT_PERCENTAGE,  FLAT_MOD, float (GetAmount()), apply);
    }
    else
    {
        // done in Player::_ApplyWeaponDependentAuraMods
    }
}

void AuraEffect::HandleModHitChance(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* l_Target = aurApp->GetTarget();

    if (l_Target->IsPlayer())
    {
        Player* l_Player = l_Target->ToPlayer();
        if (l_Player == nullptr)
            return;

        l_Player->UpdateMeleeHitChances();
        l_Player->UpdateRangedHitChances();
        if (Pet* l_Pet = l_Player->GetPet())
        {
            l_Pet->m_modMeleeHitChance += (apply) ? GetAmount() : (-GetAmount());
            l_Pet->m_modRangedHitChance += (apply) ? GetAmount() : (-GetAmount());
        }
    }
    else
    {
        l_Target->m_modMeleeHitChance += (apply) ? GetAmount() : (-GetAmount());
        l_Target->m_modRangedHitChance += (apply) ? GetAmount() : (-GetAmount());
    }
}

void AuraEffect::HandleModSpellHitChance(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* l_Target = aurApp->GetTarget();

    if (l_Target->IsPlayer())
    {
        Player* l_Player = l_Target->ToPlayer();
        if (l_Player == nullptr)
            return;

        l_Player->UpdateSpellHitChances();
        if (Pet* l_Pet = l_Player->GetPet())
            l_Pet->m_modSpellHitChance += (apply) ? GetAmount() : (-GetAmount());
    }
    else
        l_Target->m_modSpellHitChance += (apply) ? GetAmount() : (-GetAmount());
}

void AuraEffect::HandleModSpellCritChance(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* l_Target = aurApp->GetTarget();

    if (l_Target->GetTypeId() == TYPEID_PLAYER)
        l_Target->ToPlayer()->UpdateAllCritPercentages();
    else
        l_Target->m_baseSpellCritChance += (apply) ? GetAmount() : -GetAmount();
}

void AuraEffect::HandleModSpellCritChanceSchool(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* l_Target = aurApp->GetTarget();

    if (l_Target->GetTypeId() != TYPEID_PLAYER)
        return;

    for (int l_School = SPELL_SCHOOL_NORMAL; l_School < MAX_SPELL_SCHOOL; ++l_School)
        if (GetMiscValue() & (1 << l_School))
            l_Target->ToPlayer()->UpdateSpellCritChance(l_School);
}

void AuraEffect::HandleAuraModCritPct(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();
    Player* l_Player = target->ToPlayer();

    if (!l_Player)
    {
        target->m_baseSpellCritChance += (apply) ? GetAmount():-GetAmount();
        return;
    }

    float l_CurrentAmount = l_Player->GetTotalAuraModifier(GetAuraType(), apply ? this : nullptr, apply ? nullptr : (AuraEffect*)this);
    float l_NewAmount = l_Player->GetTotalAuraModifier(GetAuraType(), apply ? nullptr : this);
    l_Player->HandleBaseModValue(CRIT_PERCENTAGE,         FLAT_MOD, l_CurrentAmount, false);
    l_Player->HandleBaseModValue(OFFHAND_CRIT_PERCENTAGE, FLAT_MOD, l_CurrentAmount, false);
    l_Player->HandleBaseModValue(RANGED_CRIT_PERCENTAGE,  FLAT_MOD, l_CurrentAmount, false);
    l_Player->HandleBaseModValue(CRIT_PERCENTAGE,         FLAT_MOD, l_NewAmount, true);
    l_Player->HandleBaseModValue(OFFHAND_CRIT_PERCENTAGE, FLAT_MOD, l_NewAmount, true);
    l_Player->HandleBaseModValue(RANGED_CRIT_PERCENTAGE,  FLAT_MOD, l_NewAmount, true);

    // included in Player::UpdateSpellCritChance calculation
    l_Player->UpdateAllCritPercentages();
}

void AuraEffect::HandleAuraModResiliencePct(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* _player = target->ToPlayer();

    float baseValue = _player->GetFloatValue(PLAYER_FIELD_MOD_RESILIENCE_PERCENT);

    if (apply)
        baseValue += GetAmount() / 100.0f;
    else
        baseValue -= GetAmount() / 100.0f;

    _player->SetFloatValue(PLAYER_FIELD_MOD_RESILIENCE_PERCENT, baseValue);
}

/********************************/
/***         ATTACK SPEED     ***/
/********************************/

void AuraEffect::HandleModCastingSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    target->ApplyCastTimePercentMod(float(GetAmount()), apply);
}

void AuraEffect::HandleModMeleeRangedSpeedPct(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    //! ToDo: Haste auras with the same handler _CAN'T_ stack together
    Unit* target = aurApp->GetTarget();

    target->ApplyAttackTimePercentMod(WeaponAttackType::BaseAttack, (float)GetAmount(), apply);
    target->ApplyAttackTimePercentMod(WeaponAttackType::OffAttack, (float)GetAmount(), apply);
    target->ApplyAttackTimePercentMod(WeaponAttackType::RangedAttack, (float)GetAmount(), apply);

    if (target->getClass() == CLASS_DEATH_KNIGHT && target->ToPlayer())
        target->ToPlayer()->UpdateAllRunesRegen();
}

void AuraEffect::HandleModCombatSpeedPct(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    float l_Amount = GetSpellInfo()->GetFloatAmount(GetAmount());
    target->ApplyCastTimePercentMod(l_Amount, apply);
    target->ApplyAttackTimePercentMod(WeaponAttackType::BaseAttack, l_Amount, apply);
    target->ApplyAttackTimePercentMod(WeaponAttackType::OffAttack, l_Amount, apply);
    target->ApplyAttackTimePercentMod(WeaponAttackType::RangedAttack, l_Amount, apply);

    if (Player* l_Player = target->ToPlayer())
    {
        if (l_Player->getClass() == CLASS_WARLOCK)
        {
            for (Unit* l_Summon : l_Player->m_Controlled)
            {
                l_Summon->ApplyCastTimePercentMod(l_Amount, apply);
                l_Summon->ApplyAttackTimePercentMod(WeaponAttackType::BaseAttack, l_Amount, apply);
                l_Summon->ApplyAttackTimePercentMod(WeaponAttackType::OffAttack, l_Amount, apply);
                l_Summon->ApplyAttackTimePercentMod(WeaponAttackType::RangedAttack, l_Amount, apply);
            }
        }
    }

    // Unholy Presence
    if (target->IsPlayer())
        target->ToPlayer()->UpdateRating(CR_HASTE_MELEE);
}

void AuraEffect::HandleModAttackSpeed(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    target->ApplyAttackTimePercentMod(WeaponAttackType::BaseAttack, (float)GetAmount(), apply);
    target->UpdateDamagePhysical(WeaponAttackType::BaseAttack);
}

void AuraEffect::HandleModMeleeSpeedPct(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    //! ToDo: Haste auras with the same handler _CAN'T_ stack together
    Unit* target = aurApp->GetTarget();

    int32 value = GetAmount();

    target->ApplyAttackTimePercentMod(WeaponAttackType::BaseAttack,   (float)value, apply);
    target->ApplyAttackTimePercentMod(WeaponAttackType::OffAttack,    (float)value, apply);

    if (GetId() == 120275 && target->IsPlayer())
        target->ToPlayer()->UpdateRating(CR_HASTE_MELEE);
}

void AuraEffect::HandleAuraModRangedHaste(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    //! ToDo: Haste auras with the same handler _CAN'T_ stack together
    Unit* target = aurApp->GetTarget();

    target->ApplyAttackTimePercentMod(WeaponAttackType::RangedAttack, (float)GetAmount(), apply);
}

/********************************/
/***       COMBAT RATING      ***/
/********************************/

void AuraEffect::HandleModRating(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    switch (m_spellInfo->Id)
    {
        // Heart's Judgment, Heart of Ignacious
        case 91041:
            target->RemoveAurasDueToSpell(91027);
            break;
        // Heart's Judgment, Heart of Ignacious (Heroic)
        case 92328:
            target->RemoveAurasDueToSpell(92325);
            break;
    }

    for (uint32 rating = 0; rating < MAX_COMBAT_RATING; ++rating)
    {
        if (GetMiscValue() & (1 << rating))
        {
            float l_CurrentAmount = target->GetTotalAuraModifierByMiscMask(GetAuraType(), 1 << rating, apply ? this : nullptr, apply ? nullptr : (AuraEffect*)this);
            float l_NewAmount = target->GetTotalAuraModifierByMiscMask(GetAuraType(), 1 << rating, apply ? nullptr : this);

            target->ToPlayer()->ApplyRatingMod(CombatRating(rating), l_CurrentAmount, false);
            target->ToPlayer()->ApplyRatingMod(CombatRating(rating), l_NewAmount, true);
        }
    }
}

/********************************/
/***        ATTACK POWER      ***/
/********************************/

void AuraEffect::HandleAuraModAttackPower(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    float l_CurrentAmount = target->GetTotalAuraModifier(GetAuraType(), apply ? this : nullptr, apply ? nullptr : (AuraEffect*)this);
    float l_NewAmount = target->GetTotalAuraModifier(GetAuraType(), apply ? nullptr : this);
    target->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, l_CurrentAmount, false);
    target->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, l_NewAmount, true);
}

void AuraEffect::HandleAuraModRangedAttackPower(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if ((target->getClassMask() & CLASSMASK_WAND_USERS) != 0)
        return;

    float l_CurrentAmount = target->GetTotalAuraModifier(GetAuraType(), apply ? this : nullptr, apply ? nullptr : (AuraEffect*)this);
    float l_NewAmount = target->GetTotalAuraModifier(GetAuraType(), apply ? nullptr : this);
    target->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, l_CurrentAmount, false);
    target->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, l_NewAmount, true);
}

void AuraEffect::HandleAuraModAttackPowerPercent(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    // Don't apply Markmanship aura twice on pet
    if (GetCaster() && GetCaster()->ToPlayer() && aurApp->GetBase()->GetId() == 19506)
        if (Pet* pet = GetCaster()->ToPlayer()->GetPet())
            if (target->GetGUID() == pet->GetGUID())
                return;

    //UNIT_FIELD_ATTACK_POWER_MULTIPLIER = multiplier - 1
    ////target->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_PCT, float(GetAmount()), apply);
    target->UpdateAttackPowerAndDamage();
}

void AuraEffect::HandleAuraModRangedAttackPowerPercent(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    if ((target->getClassMask() & CLASSMASK_WAND_USERS) != 0)
        return;

    //UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = multiplier - 1
    ///target->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_PCT, float(GetAmount()), apply);
    target->UpdateAttackPowerAndDamage(true);
}

void AuraEffect::HandleAuraModAttackPowerOfArmor(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    // Recalculate bonus
    if (target->IsPlayer())
        target->ToPlayer()->UpdateAttackPowerAndDamage(false);
}

void AuraEffect::HandleOverrideAttackPowerBySpellPower(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    // Recalculate bonus
    if (target->IsPlayer())
        target->ToPlayer()->UpdateAttackPowerAndDamage(false);

    // Magic damage modifiers implemented in Unit::MeleeDamageBonusDone
    // This information for client side use only
    // Get attack power bonus for all attack type
    target->SetFloatValue(PLAYER_FIELD_OVERRIDE_APBY_SPELL_POWER_PERCENT, float(GetAmount()));
}

void AuraEffect::HandleOverrideSpellPowerByAttackPower(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Get healing bonus for all schools
    target->SetFloatValue(PLAYER_FIELD_OVERRIDE_SPELL_POWER_BY_APPERCENT, float(GetAmount()));
}

void AuraEffect::HandleIncreaseRatingPct(AuraApplication const* p_AurApp, uint8 p_Mode, bool /*p_Apply*/) const
{
    if (!(p_Mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target->GetTypeId() != TYPEID_PLAYER)
        return;

    for (uint32 l_Rating = 0; l_Rating < MAX_COMBAT_RATING; ++l_Rating)
    {
        if (GetMiscValue() & (1 << l_Rating))
            l_Target->ToPlayer()->UpdateRating(CombatRating(l_Rating));
    }
}

/********************************/
/***        DAMAGE BONUS      ***/
/********************************/
void AuraEffect::HandleModDamageDone(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    // apply item specific bonuses for already equipped weapon
    if (target->IsPlayer())
    {
        for (int i = 0; i < WeaponAttackType::MaxAttack; ++i)
            if (Item* pItem = target->ToPlayer()->GetWeaponForAttack(WeaponAttackType(i), true))
                target->ToPlayer()->_ApplyWeaponDependentAuraDamageMod(pItem, WeaponAttackType(i), this, apply);
    }

    // GetMiscValue() is bitmask of spell schools
    // 1 (0-bit) - normal school damage (SPELL_SCHOOL_MASK_NORMAL)
    // 126 - full bitmask all magic damages (SPELL_SCHOOL_MASK_MAGIC) including wands
    // 127 - full bitmask any damages
    //
    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // GetMiscValue() comparison with item generated damage types

    if ((GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL) != 0)
    {
        // apply generic physical damage bonuses including wand case
        if (GetSpellInfo()->EquippedItemClass == -1 || target->GetTypeId() != TYPEID_PLAYER)
        {
            target->HandleStatModifier(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_VALUE, float(GetAmount()), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_VALUE, float(GetAmount()), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_VALUE, float(GetAmount()), apply);

            if (target->IsPlayer())
            {
                if (GetAmount() > 0)
                    target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, GetAmount(), apply);
                else
                    target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG, GetAmount(), apply);
            }
        }
        else
        {
            // done in Player::_ApplyWeaponDependentAuraMods
        }
    }

    // Skip non magic case for speedup
    if ((GetMiscValue() & SPELL_SCHOOL_MASK_MAGIC) == 0)
        return;

    if (GetSpellInfo()->EquippedItemClass != -1 || (GetSpellInfo()->EquippedItemInventoryTypeMask != 0 && GetSpellInfo()->EquippedItemInventoryTypeMask != -1))
    {
        // wand magic case (skip generic to all item spell bonuses)
        // done in Player::_ApplyWeaponDependentAuraMods

        // Skip item specific requirements for not wand magic damage
        return;
    }

    // Magic damage modifiers implemented in Unit::SpellDamageBonus
    // This information for client side use only
    if (target->IsPlayer())
    {
        target->ToPlayer()->ApplySpellPowerBonus(GetAmount(), apply);

        if (Guardian* pet = target->ToPlayer()->GetGuardianPet())
            pet->UpdateAttackPowerAndDamage();
    }
}

void AuraEffect::HandleModDamagePercentDone(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    if (target->IsPlayer())
    {
        for (int i = 0; i < WeaponAttackType::MaxAttack; ++i)
            if (Item* item = target->ToPlayer()->GetWeaponForAttack(WeaponAttackType(i), false))
                target->ToPlayer()->_ApplyWeaponDependentAuraDamageMod(item, WeaponAttackType(i), this, apply);
    }

    float l_Amount = GetSpellInfo()->GetFloatAmount(GetAmount(), GetFloatAmount());

    if ((GetMiscValue() & SPELL_SCHOOL_MASK_NORMAL) && (GetSpellInfo()->EquippedItemClass == -1 || target->GetTypeId() != TYPEID_PLAYER))
    {
        target->HandleStatModifier(UNIT_MOD_DAMAGE_MAINHAND,         TOTAL_PCT, l_Amount, apply);
        target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND,          TOTAL_PCT, l_Amount, apply);
        target->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED,           TOTAL_PCT, l_Amount, apply);

        if (target->IsPlayer())
            target->ToPlayer()->ApplyPercentModFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PERCENT, l_Amount, apply);
    }
    else
    {
        // done in Player::_ApplyWeaponDependentAuraMods for SPELL_SCHOOL_MASK_NORMAL && EquippedItemClass != -1 and also for wand case
    }
}

void AuraEffect::HandleModOffhandDamagePercent(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, float(GetAmount()), apply);
}

void AuraEffect::HandleShieldBlockValue(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();

    BaseModType modType = FLAT_MOD;
    if (GetAuraType() == SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT)
        modType = PCT_MOD;

    if (target->IsPlayer())
    {
        target->ToPlayer()->HandleBaseModValue(SHIELD_BLOCK_VALUE, modType, float(GetAmount()), apply);
        if (modType == PCT_MOD)
            target->ToPlayer()->ApplyModUInt32Value(PLAYER_FIELD_SHIELD_BLOCK, GetAmount(), apply);
    }
}

void AuraEffect::HandleModAutoAttackDamage(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    target->ToPlayer()->UpdateAttackPowerAndDamage(false);
}

/********************************/
/***        POWER COST        ***/
/********************************/

void AuraEffect::HandleModPowerCostPCT(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    float amount = CalculatePct(1.0f, GetAmount());
    for (int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if (GetMiscValue() & (1 << i))
            target->ApplyModSignedFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + i, amount, apply);
}

void AuraEffect::HandleModPowerCost(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    for (int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if (GetMiscValue() & (1<<i))
            target->ApplyModInt32Value(UNIT_FIELD_POWER_COST_MODIFIER+i, GetAmount(), apply);
}

void AuraEffect::HandleArenaPreparation(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREPARATION);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(GetAuraType()))
            return;
        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREPARATION);
    }
}

void AuraEffect::HandleNoReagentUseAura(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    flag128 mask;
    Unit::AuraEffectList const& noReagent = target->GetAuraEffectsByType(SPELL_AURA_NO_REAGENT_USE);
        for (Unit::AuraEffectList::const_iterator i = noReagent.begin(); i != noReagent.end(); ++i)
            mask |= (*i)->m_spellInfo->Effects[(*i)->m_effIndex].SpellClassMask;

    target->SetUInt32Value(PLAYER_FIELD_NO_REAGENT_COST_MASK  , mask[0]);
    target->SetUInt32Value(PLAYER_FIELD_NO_REAGENT_COST_MASK+1, mask[1]);
    target->SetUInt32Value(PLAYER_FIELD_NO_REAGENT_COST_MASK+2, mask[2]);
    target->SetUInt32Value(PLAYER_FIELD_NO_REAGENT_COST_MASK+3, mask[3]);
}

void AuraEffect::HandleAuraRetainComboPoints(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    // remove only if aura expire by time (in case combo points amount change aura removed without combo points lost)
    if (!(apply) && GetBase()->GetDuration() == 0)
        target->AddComboPoints(-GetAmount());
}

/*********************************************************/
/***                    OTHERS                         ***/
/*********************************************************/

void AuraEffect::HandleModCategoryCooldown(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 categoryId = uint32(GetMiscValue());
    int32 value = apply ? -GetAmount() : 0;

    SpellCategoriesEntry* spellCategory = const_cast<SpellCategoriesEntry*>(sSpellCategoriesStore.LookupEntry(categoryId)); ///< spellCategory is never read 01/18/16
    if (!sSpellCategoriesStore.LookupEntry(categoryId))
        return;

    target->ToPlayer()->SendCategoryCooldown(categoryId, value);
}

void AuraEffect::HandleAuraDummy(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_REAPPLY)))
        return;

    Unit* target = aurApp->GetTarget();

    Unit* caster = GetCaster();

    if (mode & AURA_EFFECT_HANDLE_REAL)
    {
        // pet auras
        if (PetAura const* petSpell = sSpellMgr->GetPetAura(GetId(), m_effIndex))
        {
            if (apply)
                target->AddPetAura(petSpell);
            else
                target->RemovePetAura(petSpell);
        }
    }

    if (mode & (AURA_EFFECT_HANDLE_REAL | AURA_EFFECT_HANDLE_REAPPLY))
    {
        // AT APPLY
        if (apply)
        {
            switch (GetId())
            {
                // Satisfied
                case 87649:
                {
                    if (target->GetTypeId() != TYPEID_PLAYER)
                        break;
                    if (Aura* aur = target->GetAura(87649))
                        if (aur->GetStackAmount() >= 91)
                            target->ToPlayer()->UpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, 99041);

                    break;
                }
                case 1515:                                      // Tame beast
                    // FIX_ME: this is 2.0.12 threat effect replaced in 2.1.x by dummy aura, must be checked for correctness
                    if (caster && target->CanHaveThreatList())
                        target->AddThreat(caster, 10.0f);
                    break;
                case 13139:                                     // net-o-matic
                    // root to self part of (root_target->charge->root_self sequence
                    if (caster)
                        caster->CastSpell(caster, 13138, true, NULL, this);
                    break;
                case 37096:                                     // Blood Elf Illusion
                {
                    if (caster)
                    {
                        switch (caster->getGender())
                        {
                            case GENDER_FEMALE:
                                caster->CastSpell(target, 37095, true, NULL, this); // Blood Elf Disguise
                                break;
                            case GENDER_MALE:
                                caster->CastSpell(target, 37093, true, NULL, this);
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                }
                case 39850:                                     // Rocket Blast
                    if (roll_chance_i(20))                       // backfire stun
                        target->CastSpell(target, 51581, true, NULL, this);
                    break;
                case 43873:                                     // Headless Horseman Laugh
                    target->PlayDistanceSound(target, 11965);
                    break;
                case 46354:                                     // Blood Elf Illusion
                    if (caster)
                    {
                        switch (caster->getGender())
                        {
                            case GENDER_FEMALE:
                                caster->CastSpell(target, 46356, true, NULL, this);
                                break;
                            case GENDER_MALE:
                                caster->CastSpell(target, 46355, true, NULL, this);
                                break;
                        }
                    }
                    break;
                case 46361:                                     // Reinforced Net
                    if (caster)
                        target->GetMotionMaster()->MoveFall();
                    break;
                case 71563: ///< Deadly Precision
                    {
                        Aura* newAura = target->AddAura(71564, target);
                        if (newAura != nullptr)
                            newAura->SetStackAmount(newAura->GetSpellInfo()->StackAmount);
                    }
                    break;
                case 59628: // Tricks of the Trade
                    if (caster && caster->GetMisdirectionTarget())
                        target->SetReducedThreatPercent(100, caster->GetMisdirectionTarget()->GetGUID());
                    break;
            }
        }
        // AT REMOVE
        else
        {
            if ((GetSpellInfo()->IsQuestTame()) && caster && caster->isAlive() && target->isAlive())
            {
                uint32 finalSpelId = 0;
                switch (GetId())
                {
                    case 19548: finalSpelId = 19597; break;
                    case 19674: finalSpelId = 19677; break;
                    case 19687: finalSpelId = 19676; break;
                    case 19688: finalSpelId = 19678; break;
                    case 19689: finalSpelId = 19679; break;
                    case 19692: finalSpelId = 19680; break;
                    case 19693: finalSpelId = 19684; break;
                    case 19694: finalSpelId = 19681; break;
                    case 19696: finalSpelId = 19682; break;
                    case 19697: finalSpelId = 19683; break;
                    case 19699: finalSpelId = 19685; break;
                    case 19700: finalSpelId = 19686; break;
                    case 30646: finalSpelId = 30647; break;
                    case 30653: finalSpelId = 30648; break;
                    case 30654: finalSpelId = 30652; break;
                    case 30099: finalSpelId = 30100; break;
                    case 30102: finalSpelId = 30103; break;
                    case 30105: finalSpelId = 30104; break;
                }

                if (finalSpelId)
                    caster->CastSpell(target, finalSpelId, true, NULL, this);
            }

            switch (m_spellInfo->SpellFamilyName)
            {
                case SPELLFAMILY_GENERIC:
                    switch (GetId())
                    {
                        case 2584: // Waiting to Resurrect
                            // Waiting to resurrect spell cancel, we must remove player from resurrect queue
                            if (!(mode & AURA_EFFECT_HANDLE_REAL))
                                break;

                            if (target->IsPlayer())
                            {
                                if (Battleground* bg = target->ToPlayer()->GetBattleground())
                                    bg->RemovePlayerFromResurrectQueue(target->GetGUID());
                                if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(target->GetZoneId()))
                                    bf->RemovePlayerFromResurrectQueue(target->GetGUID());
                            }
                            break;
                        case 36730:                                     // Flame Strike
                        {
                            target->CastSpell(target, 36731, true, NULL, this);
                            break;
                        }
                        case 44191:                                     // Flame Strike
                        {
                            if (target->GetMap()->IsDungeon())
                            {
                                uint32 spellId = target->GetMap()->IsHeroic() ? 46163 : 44190;

                                target->CastSpell(target, spellId, true, NULL, this);
                            }
                            break;
                        }
                        case 43681: // Inactive
                        {
                            if (target->GetTypeId() != TYPEID_PLAYER || aurApp->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                                return;
#ifndef CROSS

                            if (target->GetMap()->IsBattleground())
                                target->ToPlayer()->LeaveBattleground();
#endif /* not CROSS */
                            break;
                        }
                        case 42783: // Wrath of the Astromancer
                            target->CastSpell(target, GetAmount(), true, NULL, this);
                            break;
                        case 46308: // Burning Winds casted only at creatures at spawn
                            target->CastSpell(target, 47287, true, NULL, this);
                            break;
                        case 52172:  // Don Carlos' Famous Hat
                        case 60244:  // Blood Parrot Despawn Aura
                            target->CastSpell((Unit*)NULL, GetAmount(), true, NULL, this);
                            break;
                        case 58600: // Restricted Flight Area
                        case 83100: // Restricted Flight Area
                        case 91604: // Restricted Flight Area
                            if (aurApp->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                                target->CastSpell(target, 58601, true);
                            break;
                    }
                    break;
                case SPELLFAMILY_DEATHKNIGHT:
                    // Summon Gargoyle (Dismiss Gargoyle at remove)
                    if (GetId() == 61777)
                        target->CastSpell(target, GetAmount(), true);
                    break;
                case SPELLFAMILY_ROGUE:
                    //  Tricks of the trade
                    switch (GetId())
                    {
                        case 59628: //Tricks of the trade buff on rogue (6sec duration)
                            target->SetReducedThreatPercent(0,0);
                            break;
                        case 57934: //Tricks of the trade buff on rogue (30sec duration)
                            if (aurApp->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE || (caster && !caster->GetMisdirectionTarget()))
                                target->SetReducedThreatPercent(0,0);
                            else
                                target->SetReducedThreatPercent(0, (caster ? caster->GetMisdirectionTarget()->GetGUID() : 0));
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // AT APPLY & REMOVE

    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            if (!(mode & AURA_EFFECT_HANDLE_REAL))
                break;
            switch (GetId())
            {
                // Recently Bandaged
                case 11196:
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, GetMiscValue(), apply);
                    break;
                // Unstable Power
                case 24658:
                {
                    uint32 spellId = 24659;
                    if (apply && caster)
                    {
                        SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellId);

                        for (uint32 i = 0; i < spell->StackAmount; ++i)
                            caster->CastSpell(target, spell->Id, true, NULL, nullptr, GetCasterGUID());
                        break;
                    }
                    target->RemoveAurasDueToSpell(spellId);
                    break;
                }
                // Restless Strength
                case 24661:
                {
                    uint32 spellId = 24662;
                    if (apply && caster)
                    {
                        SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellId);
                        for (uint32 i = 0; i < spell->StackAmount; ++i)
                            caster->CastSpell(target, spell->Id, true, NULL, nullptr, GetCasterGUID());
                        break;
                    }
                    target->RemoveAurasDueToSpell(spellId);
                    break;
                }
                // Tag Murloc
                case 30877:
                {
                    // Tag/untag Blacksilt Scout
                    target->SetEntry(apply ? 17654 : 17326);

                    if (apply)
                        if (auto player = caster->ToPlayer())
                            player->KilledMonsterCredit(17654);

                    break;
                }
                case 57819:  // Argent Champion
                case 57820:  // Ebon Champion
                case 57821:  // Champion of the Kirin Tor
                case 57822:  // Wyrmrest Champion
                case 93339:  // Champion of the Earthen Ring
                case 94158:  // Champion of the Dragonmaw Clan
                case 93337:  // Champion of Ramkahen
                case 93341:  // Champion of the Guardians of Hyjal
                case 93368:  // Champion of the Wildhammer Clan
                case 93347:  // Champion of Therazane
                case 93830:  // Bilgewater Champion
                case 93827:  // Darkspear Champion
                case 93806:  // Darnassus Champion
                case 93811:  // Exodar Champion
                case 93816:  // Gilneas Champion
                case 93821:  // Gnomeregan Champion
                case 93805:  // Ironforge Champion
                case 93825:  // Orgrimmar Champion
                case 93795:  // Stormwind Champion
                case 94463:  // Thunder Bluff Champion
                case 94462:  // Undercity Champion
                case 93828:  // Silvermoon Champion
                case 126434: // Tushui
                case 126436: // Huojin
                {
                    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                        break;

                    uint32 FactionID = 0;

                    if (apply)
                    {
                        switch (m_spellInfo->Id)
                        {
                            case 57819: FactionID  = 1106;  break; // Argent Crusade
                            case 57820: FactionID  = 1098;  break; // Knights of the Ebon Blade
                            case 57821: FactionID  = 1090;  break; // Kirin Tor
                            case 57822: FactionID  = 1091;  break; // The Wyrmrest Accord
                            // Alliance factions
                            case 93795: FactionID  = 72;    break; // Stormwind
                            case 93805: FactionID  = 47;    break; // Ironforge
                            case 93806: FactionID  = 69;    break; // Darnassus
                            case 93811: FactionID  = 930;   break; // Exodar
                            case 93816: FactionID  = 1134;  break; // Gilneas
                            case 93821: FactionID  = 54;    break; // Gnomeregan
                            case 126434: FactionID = 1353;  break; // Tushui Pandaren
                            // Horde factions
                            case 93825: FactionID  = 76;    break; // Orgrimmar
                            case 93827: FactionID  = 530;   break; // Darkspear Trolls
                            case 93828: FactionID  = 911;   break; // Silvermoon
                            case 93830: FactionID  = 1133;  break; // Bilgewater Cartel
                            case 94462: FactionID  = 68;    break; // Undercity
                            case 94463: FactionID  = 81;    break; // Thunder Bluff
                            case 126436: FactionID = 1352;  break; // Huojin Pandaren
                            // Cataclysm factions
                            case 93337: FactionID  = 1173;  break; // Ramkahen
                            case 93339: FactionID  = 1135;  break; // The Earthen Ring
                            case 93341: FactionID  = 1158;  break; // Guardians of Hyjal
                            case 93347: FactionID  = 1171;  break; // Therazane
                            case 93368: FactionID  = 1174;  break; // Wildhammer Clan
                            case 94158: FactionID  = 1172;  break; // Dragonmaw Clan
                        }
                    }

                    caster->ToPlayer()->SetChampioningFaction(FactionID);
                    break;
                }
                // LK Intro VO (1)
                case 58204:
                    if (target->IsPlayer())
                    {
                        // Play part 1
                        if (apply)
                            target->PlayDirectSound(14970, target->ToPlayer());
                        // continue in 58205
                        else
                            target->CastSpell(target, 58205, true);
                    }
                    break;
                // LK Intro VO (2)
                case 58205:
                    if (target->IsPlayer())
                    {
                        // Play part 2
                        if (apply)
                            target->PlayDirectSound(14971, target->ToPlayer());
                        // Play part 3
                        else
                            target->PlayDirectSound(14972, target->ToPlayer());
                    }
                    break;
                case 62061: // Festive Holiday Mount
                    if (target->HasAuraType(SPELL_AURA_MOUNTED))
                    {
                        uint32 creatureEntry = 0;
                        if (apply)
                        {
                            if (target->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
                                creatureEntry = 24906;
                            else
                                creatureEntry = 15665;
                        }
                        else
                            creatureEntry = target->GetAuraEffectsByType(SPELL_AURA_MOUNTED).front()->GetMiscValue();

                        if (CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(creatureEntry))
                        {
                            uint32 team = 0;
                            if (target->IsPlayer())
                                team = target->ToPlayer()->GetTeam();

                            uint32 displayID = sObjectMgr->ChooseDisplayId(team, creatureInfo);
                            sObjectMgr->GetCreatureModelRandomGender(&displayID);

                            target->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, displayID);
                        }
                    }
                    break;
            }

            break;
        }
        case SPELLFAMILY_MAGE:
        {
            //if (!(mode & AURA_EFFECT_HANDLE_REAL))
                //break;
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            //if (!(mode & AURA_EFFECT_HANDLE_REAL))
                //break;
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
                break;

            switch (GetId())
            {
                case 61336:                                 // Survival Instincts
                {
                    if (!(mode & AURA_EFFECT_HANDLE_REAL))
                        break;

                    if (apply)
                    {
                        if (!target->IsInFeralForm())
                            break;

                        target->CastSpell(target, 50322, true);
                    }
                    else
                        target->RemoveAurasDueToSpell(50322);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            if (!(mode & AURA_EFFECT_HANDLE_REAL))
                break;
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            if (!(mode & AURA_EFFECT_HANDLE_REAL))
                break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            if (!(mode & AURA_EFFECT_HANDLE_REAL))
                break;

            break;
        }
    }

    switch (m_spellInfo->Id)
    {
        case 196055:    ///< Double Jump, @todo MOVE ME TO SPELLSCRIPT
        {
            if (Player* l_Player = caster->ToPlayer())
            {
                l_Player->SendMovementSetCanDoubleJump(apply);
            }
        }
    }
}

void AuraEffect::HandleSwitchTeam(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (!l_Target)
        return;

    Player* l_Player = l_Target->ToPlayer();
    if (!l_Player)
        return;

    l_Player->SwitchToOppositeTeam(p_Apply);
}

void AuraEffect::HandleChannelDeathItem(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    if (apply || aurApp->GetRemoveMode() != AURA_REMOVE_BY_DEATH)
        return;

    Unit* caster = GetCaster();

    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* plCaster = caster->ToPlayer();
    Unit* target = aurApp->GetTarget(); ///< target is never read 01/18/16

    // Item amount
    if (GetAmount() <= 0)
        return;

    if (GetSpellInfo()->Effects[m_effIndex].ItemType == 0)
        return;

    //Adding items
    uint32 noSpaceForCount = 0;
    uint32 count = m_amount;

    ItemPosCountVec dest;
    InventoryResult msg = plCaster->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, GetSpellInfo()->Effects[m_effIndex].ItemType, count, &noSpaceForCount);
    if (msg != EQUIP_ERR_OK)
    {
        count-=noSpaceForCount;
        plCaster->SendEquipError(msg, NULL, NULL, GetSpellInfo()->Effects[m_effIndex].ItemType);
        if (count == 0)
            return;
    }

    Item* newitem = plCaster->StoreNewItem(dest, GetSpellInfo()->Effects[m_effIndex].ItemType, true);
    if (!newitem)
    {
        plCaster->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }
    plCaster->SendNewItem(newitem, count, true, true);
}

void AuraEffect::HandleBindSight(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    Unit* caster = GetCaster();

    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    caster->ToPlayer()->SetViewpoint(target, apply);

    if (apply)
    {
        caster->AddDelayedEvent([caster]() -> void
        {
            auto l_ViewPoint = caster->ToPlayer()->GetViewpoint();
            auto l_Seer = caster->ToPlayer()->m_seer;
            if (l_ViewPoint != nullptr && l_ViewPoint != l_Seer)
            {
                caster->ToPlayer()->SetSeer(l_ViewPoint);
                caster->ToPlayer()->UpdateVisibilityForPlayer(true);
            }
        }, 500);
    }
}

void AuraEffect::HandleForceReaction(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)target;

    uint32 faction_id = GetMiscValue();
    ReputationRank faction_rank = ReputationRank(m_amount);

    player->GetReputationMgr().ApplyForceReaction(faction_id, faction_rank, apply);
    player->GetReputationMgr().SendForceReactions();

    // stop fighting if at apply forced rank friendly or at remove real rank friendly
    if ((apply && faction_rank >= REP_FRIENDLY) || (!apply && player->GetReputationRank(faction_id) >= REP_FRIENDLY))
        player->StopAttackFaction(faction_id);
}

void AuraEffect::HandleAuraEmpathy(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_UNIT)
        return;

    if (!apply)
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(GetAuraType()))
            return;
    }

    CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(target->GetEntry());
    if (ci && ci->type == CREATURE_TYPE_BEAST)
        target->ApplyModUInt32Value(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_SPECIALINFO, apply);
}

void AuraEffect::HandleAuraModFaction(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
    {
        target->setFaction(GetMiscValue());
        if (target->IsPlayer())
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
    }
    else
    {
        target->RestoreFaction();
        if (target->IsPlayer())
            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
    }
}

void AuraEffect::HandleComprehendLanguage(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
        target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG);
    else
    {
        if (target->HasAuraType(GetAuraType()))
            return;

        target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG);
    }
}

void AuraEffect::HandleAuraConvertRune(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    /*if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)target;

    if (player->getClass() != CLASS_DEATH_KNIGHT)
        return;

    uint32 runes = m_amount;
    RuneType rune = RuneType(GetMiscValueB());
    bool permanently = GetId() == 54637; // Find the better way

    // convert number of runes specified in aura amount of rune type in miscvalue to runetype in miscvalueb
    if (apply)
    {
        for (uint32 i = 0; i < MAX_RUNES && runes; ++i)
        {
            if (GetMiscValue() != player->GetCurrentRune(i))
                continue;

            player->AddRuneBySpell(i, rune, GetId());
            if (permanently)
                player->SetRuneConvertType(i, permanently);

            --runes;
        }
    }
    else
        player->RemoveRunesBySpell(GetId());*/
}

void AuraEffect::HandleAuraLinked(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    Unit* target = aurApp->GetTarget();

    uint32 triggeredSpellId = m_spellInfo->Effects[m_effIndex].TriggerSpell;
    SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(triggeredSpellId);
    if (!triggeredSpellInfo)
        return;

    if ((mode & AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK) && !apply && triggeredSpellInfo->HasEffect(SPELL_EFFECT_SUMMON))
    {
        SpellEffectInfo const* l_Effect = triggeredSpellInfo->GetEffectByType(SPELL_EFFECT_SUMMON);
        for (Unit* l_Summon : target->m_Controlled)
        {
            if (l_Summon->GetEntry() == l_Effect->MiscValue)
            {
                if (Creature* l_Creature = l_Summon->ToCreature())
                {
                    l_Creature->DespawnOrUnsummon();
                    break;
                }
            }
        }
    }

    if (mode & AURA_EFFECT_HANDLE_REAL)
    {
        if (apply)
        {
            Unit* caster = triggeredSpellInfo->NeedsToBeTriggeredByCaster() ? GetCaster() : target;

            if (!caster)
                return;
            // If amount avalible cast with basepoints (Crypt Fever for example)
            if (GetAmount())
                caster->CastCustomSpell(target, triggeredSpellId, &m_amount, NULL, NULL, true, NULL, this);
            else
                caster->CastSpell(target, triggeredSpellId, true, NULL, this);
        }
        else
        {
            uint64 casterGUID = triggeredSpellInfo->NeedsToBeTriggeredByCaster() ? GetCasterGUID() : target->GetGUID();
            target->RemoveAura(triggeredSpellId, casterGUID, 0, aurApp->GetRemoveMode());
        }
    }
    else if (mode & AURA_EFFECT_HANDLE_REAPPLY && apply)
    {
        uint64 casterGUID = triggeredSpellInfo->NeedsToBeTriggeredByCaster() ? GetCasterGUID() : target->GetGUID();
        // change the stack amount to be equal to stack amount of our aura
        Aura* triggeredAura = target->GetAura(triggeredSpellId, casterGUID);
        if (triggeredAura != nullptr)
            triggeredAura->ModStackAmount(GetBase()->GetStackAmount() - triggeredAura->GetStackAmount());
    }
}

void AuraEffect::HandleAuraActivateSpellScene(AuraApplication const* p_AuraApplication, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AuraApplication->GetTarget();
    if (!l_Target || l_Target->GetTypeId() != TYPEID_PLAYER)
        return;

    Position l_Position;
    l_Target->GetPosition(&l_Position);

    l_Target->ToPlayer()->SendSpellScene(GetId(), GetMiscValue(), p_Apply, &l_Position);
}

void AuraEffect::HandleAuraContestedPvp(AuraApplication const* p_AuraApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AuraApp->GetTarget();

    if (l_Target == nullptr || !GetMiscValue())
        return;

    if (Player* l_Player = l_Target->ToPlayer())
    {
        if (p_Apply)
        {
            /// 1 : Friendly - 2 : FFA - 3 : Contested PvP
            l_Player->SetUInt32Value(PLAYER_FIELD_OVERRIDE_ZONE_PVPTYPE, GetMiscValue());

            if (GetMiscValue() == OverridePvpTypes::TypeFFA)
                l_Player->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
        }
        else
        {
            l_Player->SetUInt32Value(PLAYER_FIELD_OVERRIDE_ZONE_PVPTYPE, 0);

            if (GetMiscValue() == OverridePvpTypes::TypeFFA)
                l_Player->RemoveByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_PVP | UNIT_BYTE2_FLAG_FFA_PVP);
        }
    }
}

void AuraEffect::HandleAuraOpenStable(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER || !target->IsInWorld())
        return;

    if (apply)
        target->ToPlayer()->GetSession()->SendStablePet(target->GetGUID());

     // client auto close stable dialog at !apply aura
}

void AuraEffect::HandleAuraModFakeInebriation(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();

    if (apply)
    {
        target->m_invisibilityDetect.AddFlag(INVISIBILITY_DRUNK);
        target->m_invisibilityDetect.AddValue(INVISIBILITY_DRUNK, GetAmount());

        if (target->IsPlayer())
        {
            int32 oldval = target->ToPlayer()->GetInt32Value(PLAYER_FIELD_FAKE_INEBRIATION);
            target->ToPlayer()->SetInt32Value(PLAYER_FIELD_FAKE_INEBRIATION, oldval + GetAmount());
        }
    }
    else
    {
        bool removeDetect = !target->HasAuraType(SPELL_AURA_MOD_FAKE_INEBRIATE);

        target->m_invisibilityDetect.AddValue(INVISIBILITY_DRUNK, -GetAmount());

        if (target->IsPlayer())
        {
            int32 oldval = target->ToPlayer()->GetInt32Value(PLAYER_FIELD_FAKE_INEBRIATION);
            target->ToPlayer()->SetInt32Value(PLAYER_FIELD_FAKE_INEBRIATION, oldval - GetAmount());

            if (removeDetect)
                removeDetect = !target->ToPlayer()->GetDrunkValue();
        }

        if (removeDetect)
            target->m_invisibilityDetect.DelFlag(INVISIBILITY_DRUNK);
    }

    // call functions which may have additional effects after chainging state of unit
    target->UpdateObjectVisibility();
}

void AuraEffect::HandleAuraOverrideSpells(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Player* target = aurApp->GetTarget()->ToPlayer();

    if (!target || !target->IsInWorld())
        return;

    uint32 overrideId = uint32(GetMiscValue());

    if (apply)
    {
        target->SetUInt16Value(PLAYER_FIELD_OVERRIDE_SPELLS_ID, PLAYER_FIELD_OVERRIDE_SPELLS_ID_OFFSET_OVERRIDE_SPELLS_ID, overrideId);
        if (OverrideSpellDataEntry const* overrideSpells = sOverrideSpellDataStore.LookupEntry(overrideId))
            for (uint8 i = 0; i < MAX_OVERRIDE_SPELL; ++i)
                if (uint32 spellId = overrideSpells->spellId[i])
                    target->AddTemporarySpell(spellId);
    }
    else
    {
        target->SetUInt16Value(PLAYER_FIELD_OVERRIDE_SPELLS_ID, PLAYER_FIELD_OVERRIDE_SPELLS_ID_OFFSET_OVERRIDE_SPELLS_ID, 0);
        if (OverrideSpellDataEntry const* overrideSpells = sOverrideSpellDataStore.LookupEntry(overrideId))
            for (uint8 i = 0; i < MAX_OVERRIDE_SPELL; ++i)
                if (uint32 spellId = overrideSpells->spellId[i])
                    target->RemoveTemporarySpell(spellId);
    }
}

void AuraEffect::HandleAuraSetVehicle(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();

    if (!target->IsInWorld())
        return;

    uint32 vehicleId = GetMiscValue();

    if (target->GetVehicleKit())
        target->RemoveVehicleKit();

    if (apply)
    {
        if (!target->CreateVehicleKit(vehicleId, 0))
            return;
    }

    WorldPacket l_Data(SMSG_SET_VEHICLE_REC_ID, target->GetPackGUID().size()+4);
    l_Data.appendPackGUID(target->GetGUID());
    l_Data << uint32(apply ? vehicleId : 0);
    target->SendMessageToSet(&l_Data, true);

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
    {
        l_Data.Initialize(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, 0);
        target->ToPlayer()->GetSession()->SendPacket(&l_Data);
        target->GetVehicleKit()->Reset();
        target->GetVehicleKit()->InstallAllAccessories(false);
    }
    else
        target->RemoveVehicleKit(true);
}

void AuraEffect::HandlePreventResurrection(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    if (aurApp->GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        aurApp->GetTarget()->RemoveFlag(PLAYER_FIELD_LOCAL_FLAGS, PLAYER_LOCAL_FLAG_RELEASE_TIMER);
    else if (!aurApp->GetTarget()->GetBaseMap()->Instanceable())
        aurApp->GetTarget()->SetFlag(PLAYER_FIELD_LOCAL_FLAGS, PLAYER_LOCAL_FLAG_RELEASE_TIMER);
}

bool AuraEffect::AuraSpellTrigger(Unit* target, Unit* caster, SpellEffIndex effIndex) const
{
    if (std::vector<SpellDummyTrigger> const* spellTrigger = sSpellMgr->GetSpellAuraTrigger(m_spellInfo->Id))
    {
        bool check = false;
        Unit* triggerTarget = GetBase()->GetUnitOwner();;
        Unit* triggerCaster = caster;
        int32 basepoints0 = m_amount;

        for (std::vector<SpellDummyTrigger>::const_iterator itr = spellTrigger->begin(); itr != spellTrigger->end(); ++itr)
        {
            if (!(itr->EffectMask & (1 << effIndex)))
                continue;

            //if (target)
            {
                if (itr->Target)
                    triggerTarget = (target ? target : caster)->GetUnitForLinkedSpell(caster, target, itr->Target);

                if (!triggerTarget)
                {
                    //sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AuraEffect::AuraSpellTrigger: not exist triggerTarget");
                    check = true;
                    continue;
                }
                if (itr->TargetAura > 0 && !triggerTarget->HasAura(itr->TargetAura))
                {
                    check = true;
                    continue;
                }
                else if (itr->TargetAura < 0 && triggerTarget->HasAura(abs(itr->TargetAura)))
                {
                    check = true;
                    continue;
                }
            }

            if (caster)
            {
                if (itr->Caster)
                    triggerCaster = caster->GetUnitForLinkedSpell(caster, target, itr->Caster);
                if (itr->Aura > 0 && !caster->HasAura(itr->Aura))
                {
                    check = true;
                    continue;
                }
                else if (itr->Aura < 0 && caster->HasAura(abs(itr->Aura)))
                {
                    check = true;
                    continue;
                }
            }

            if (!triggerCaster)
            {
                //sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AuraEffect::AuraSpellTrigger: not exist triggerCaster");
                check = true;
                continue;
            }

            if (itr->Chance != 0)
            {
                if (!roll_chance_i(itr->Chance))
                    continue;
            }

            int32 bp0 = int32(itr->Bp0);
            int32 bp1 = int32(itr->Bp1);
            int32 bp2 = int32(itr->Bp2);
            int32 spell_trigger = m_amount;

            if (itr->SpellTrigger != 0)
                spell_trigger = abs(itr->SpellTrigger);

            switch (itr->Options)
            {
                case AURA_TRIGGER: //0
                {
                    triggerCaster->CastSpell(triggerTarget, spell_trigger, true);
                    check = true;
                    break;
                }
                case AURA_TRIGGER_BP: //1
                {
                    if (itr->SpellTrigger < 0)
                        basepoints0 *= -1;

                    triggerCaster->CastCustomSpell(triggerTarget, spell_trigger, &basepoints0, &basepoints0, &basepoints0, true, NULL, this);
                    check = true;
                    break;
                }
                case AURA_TRIGGER_BP_CUSTOM: //2
                {
                    triggerCaster->CastCustomSpell(triggerTarget, spell_trigger, &bp0, &bp1, &bp2, true, NULL, this);
                    check = true;
                    break;
                }
                case AURA_TRIGGER_CHECK_COMBAT: //3
                {
                    if (itr->SpellTrigger < 0 && !caster->isInCombat())
                        triggerCaster->CastSpell(triggerTarget, spell_trigger, true);
                    else if (itr->SpellTrigger > 0 && caster->isInCombat())
                        triggerCaster->CastSpell(triggerTarget, spell_trigger, true);
                    check = true;
                    break;
                }
                case AURA_TRIGGER_DEST: //4
                {
                    triggerCaster->CastSpell(triggerTarget->GetPositionX(), triggerTarget->GetPositionY(), triggerTarget->GetPositionZ(), spell_trigger, true, NULL, this);
                    check = true;
                    break;
                }
                case AURA_TRIGGER_DYNOBJECT: //5
                {
                    std::list<DynamicObject*> list;
                    triggerCaster->GetDynObjectList(list, GetId());
                    if (!list.empty())
                    {
                        Unit* owner = triggerCaster->GetOwner();
                        for (std::list<DynamicObject*>::iterator itr = list.begin(); itr != list.end(); ++itr)
                        {
                            if (DynamicObject* dynObj = (*itr))
                                triggerCaster->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), spell_trigger, true, NULL, this, owner ? owner->GetGUID() : 0);
                        }
                    }
                    check = true;
                    break;
                }
                case AURA_TRIGGER_FROM_SUMMON_SLOT: //6
                {
                    if (itr->Slot < MAX_SUMMON_SLOT)
                        if (triggerCaster->m_SummonSlot[itr->Slot])
                            if (Creature* summon = triggerCaster->GetMap()->GetCreature(triggerCaster->m_SummonSlot[itr->Slot]))
                                summon->CastSpell(summon, spell_trigger, true, NULL, this, triggerCaster->GetGUID());
                    check = true;
                    break;
                }
                case AURA_TRIGGER_AREATRIGGER: //7
                {
                    std::list<AreaTrigger*> list;
                    triggerCaster->GetAreaTriggerList(list, GetId());
                    if (!list.empty())
                    {
                        Unit* owner = triggerCaster->GetOwner();
                        for (std::list<AreaTrigger*>::iterator itr = list.begin(); itr != list.end(); ++itr)
                        {
                            if (AreaTrigger* areaObj = (*itr))
                                triggerCaster->CastSpell(areaObj->GetPositionX(), areaObj->GetPositionY(), areaObj->GetPositionZ(), spell_trigger, true, NULL, this, owner ? owner->GetGUID() : 0);
                        }
                    }
                    check = true;
                    break;
                }
                case AAURA_TRIGGER_FROM_SUMMON_SLOT_DEST: //8
                {
                    if (itr->Slot < MAX_SUMMON_SLOT)
                        if (triggerCaster->m_SummonSlot[itr->Slot])
                            if (Creature* summon = triggerCaster->GetMap()->GetCreature(triggerCaster->m_SummonSlot[itr->Slot]))
                                summon->CastSpell(summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ(), spell_trigger, true, NULL, this, triggerCaster->GetGUID());
                    check = true;
                    break;
                }
                case AURA_TRIGGER_FROM_SUMMON_DEST: //9
                {
                    GuidList* summonList = triggerCaster->GetSummonList(itr->Slot);
                    for (GuidList::const_iterator iter = summonList->begin(); iter != summonList->end(); ++iter)
                    {
                        if (Creature* summon = ObjectAccessor::GetCreature(*triggerCaster, (*iter)))
                            summon->CastSpell(summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ(), spell_trigger, true, NULL, this, triggerCaster->GetGUID());
                    }
                    check = true;
                    break;
                }
                case AURA_TRIGGER_AREATRIGGER_CAST: //10
                {
                    std::list<AreaTrigger*> list;
                    triggerCaster->GetAreaTriggerList(list, GetId());
                    if (!list.empty())
                    {
                        for (std::list<AreaTrigger*>::iterator itr = list.begin(); itr != list.end(); ++itr)
                        {
                            if (AreaTrigger* areaObj = (*itr))
                                areaObj->CastAction();
                        }
                    }
                    check = true;
                    break;
                }
                case AURA_TRIGGER_TARGETCASTER: //11
                {
                    triggerTarget->CastSpell(triggerTarget, spell_trigger, true, nullptr, nullptr, triggerCaster->GetGUID());
                    check = true;
                    break;
                }
                default:
                    break;
            }
        }

        if (check)
            return true;
    }

    return false;
}

void AuraEffect::HandlePeriodicDummyAuraTick(Unit* target, Unit* caster, SpellEffIndex p_EffIndex) const
{
    if (!caster || !target)
        return;

    uint32 l_TriggerSpellId = m_spellInfo->Effects[p_EffIndex].TriggerSpell;

    switch (GetSpellInfo()->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (GetId())
            {
                case 199837: // Shade of Xavius - Drain Essence
                {
                    if (caster)
                        caster->CastSpell(caster, 204502, true);
                    break;
                }
                case 200050: // Shade of Xavius - Apocalyptic Nightmare
                {
                    if (caster && caster->ToCreature() && caster->ToCreature()->IsAIEnabled)
                        caster->ToCreature()->AI()->DoAction(0);

                    break;
                }
                case 98971: // Smoldering Rune, Item - Death Knight T12 DPS 2P Bonus
                    caster->CastSpell(caster, 99055, true);
                    break;
                case 91296: // Egg Shell, Corrupted Egg Shell
                    GetCaster()->CastSpell(caster, 91306, true);
                    break;
                case 91308: // Egg Shell, Corrupted Egg Shell (H)
                    caster->CastSpell(caster, 91311, true);
                    break;
                case 66149: // Bullet Controller Periodic - 10 Man
                case 68396: // Bullet Controller Periodic - 25 Man
                {
                    caster->CastCustomSpell(66152, SPELLVALUE_MAX_TARGETS, urand(1, 6), target, true);
                    caster->CastCustomSpell(66153, SPELLVALUE_MAX_TARGETS, urand(1, 6), target, true);
                    break;
                }
                case 62292: // Blaze (Pool of Tar)
                    // should we use custom damage?
                    target->CastSpell((Unit*)NULL, m_spellInfo->Effects[m_effIndex].TriggerSpell, true);
                    break;
                case 62399: // Overload Circuit
                {
                    if (target->GetMap()->IsDungeon() && int(target->GetAppliedAuras().count(62399)) >= (target->GetMap()->IsHeroic() ? 4 : 2))
                    {
                        target->CastSpell(target, 62475, true); // System Shutdown
                        if (Unit* veh = target->GetVehicleBase())
                            veh->CastSpell(target, 62475, true);
                    }
                    break;
                }
                case 64821: // Fuse Armor (Razorscale)
                {
                    if (GetBase()->GetStackAmount() == GetSpellInfo()->StackAmount)
                    {
                        target->CastSpell(target, 64774, true, NULL, nullptr, GetCasterGUID());
                        target->RemoveAura(64821);
                    }
                    break;
                }
                case 102522:// Fan the Flames
                {
                    switch (rand()%4)
                    {
                        case 1:
                            caster->CastSpell(caster, 109090, true);
                            break;
                        case 2:
                            caster->CastSpell(caster, 109105, true);
                            break;
                    }

                    if (GetBase()->GetDuration() < 1000)
                        caster->CastSpell(caster, 109107, true);

                    break;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Mirror Image
            if (GetId() == 55342)
                // Set name of summons to name of caster
                target->CastSpell((Unit*)NULL, m_spellInfo->Effects[m_effIndex].TriggerSpell, true);
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            switch (GetSpellInfo()->Id)
            {
                // Master of Subtlety
                case 31666:
                    if (!target->HasAuraType(SPELL_AURA_MOD_STEALTH))
                        target->RemoveAurasDueToSpell(31665);
                    break;
            }
            break;
        }
        default:
            break;
    }

    if (caster && l_TriggerSpellId == 197752) ///< @TODO: change "l_TriggerSpellId == 197752" by "l_TriggerSpellId" and review every legion dungeons, it's going to break some scripts (cast twice) since we handle it manually in some scripts
    {
        std::list<DynamicObject*> list;
        caster->GetDynObjectList(list, GetId());
        if (!list.empty())
        {
            Unit* owner = caster->GetAnyOwner();
            for (std::list<DynamicObject*>::iterator itr = list.begin(); itr != list.end(); ++itr)
            {
                if (DynamicObject* dynObj = (*itr))
                    caster->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), l_TriggerSpellId, true, NULL, this, owner ? owner->GetGUID() : 0);
            }
        }
        else if (target)
        {
            SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(l_TriggerSpellId);
            if (!triggeredSpellInfo)
                return;

            SpellCastResult checkResult = triggeredSpellInfo->CheckExplicitTarget(caster, target);
            if (checkResult == SPELL_CAST_OK)
                caster->CastSpell(target, l_TriggerSpellId, true, NULL, this);
            else if (Unit* _target = (caster->ToPlayer() ? caster->ToPlayer()->GetSelectedUnit() : caster->getVictim()))
            {
                SpellCastResult checkResult = triggeredSpellInfo->CheckExplicitTarget(caster, _target);
                if (checkResult == SPELL_CAST_OK)
                    caster->CastSpell(_target, l_TriggerSpellId, true, NULL, this);
                else
                    caster->CastSpell(caster, l_TriggerSpellId, true, NULL, this);
            }
        }
    }
}

void AuraEffect::HandlePeriodicTriggerSpellAuraTick(Unit* target, Unit* caster) const
{
    // generic casting code with custom spells and target/caster customs
    uint32 triggerSpellId = GetSpellInfo()->Effects[GetEffIndex()].TriggerSpell;

    SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(triggerSpellId);
    SpellInfo const* auraSpellInfo = GetSpellInfo();
    uint32 auraId = auraSpellInfo->Id;

    // specific code for cases with no trigger spell provided in field
    if (triggeredSpellInfo == NULL)
    {
        switch (auraSpellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                switch (auraId)
                {
                    // Thaumaturgy Channel
                    case 9712:
                        triggerSpellId = 21029;
                        break;
                    // Brood Affliction: Bronze
                    case 23170:
                        triggerSpellId = 23171;
                        break;
                    // Restoration
                    case 24379:
                    case 23493:
                    {
                        if (caster)
                        {
                            int32 heal = caster->CountPctFromMaxHealth(10);
                            caster->HealBySpell(target, auraSpellInfo, heal);

                            if (int32 mana = caster->GetMaxPower(POWER_MANA))
                            {
                                mana /= 10;
                                caster->EnergizeBySpell(caster, 23493, mana, POWER_MANA);
                            }
                        }
                        return;
                    }
                    // Nitrous Boost
                    case 27746:
                        if (caster && target->GetPower(POWER_MANA) >= 10)
                        {
                            target->ModifyPower(POWER_MANA, -10);
                            target->SendEnergizeSpellLog(caster, 27746, 10, 0, POWER_MANA);
                        }
                        else
                            target->RemoveAurasDueToSpell(27746);
                        return;
                    // Frost Blast
                    case 27808:
                        if (caster)
                            caster->CastCustomSpell(29879, SPELLVALUE_BASE_POINT0, int32(target->CountPctFromMaxHealth(21, caster)), target, true, NULL, this);
                        return;
                    // Inoculate Nestlewood Owlkin
                    case 29528:
                        if (target->GetTypeId() != TYPEID_UNIT) // prevent error reports in case ignored player target
                            return;
                        break;
                    // Feed Captured Animal
                    case 29917:
                        triggerSpellId = 29916;
                        break;
                    // Extract Gas
                    case 30427:
                    {
                        // move loot to player inventory and despawn target
                        if (caster && caster->IsPlayer() &&
                                target->GetTypeId() == TYPEID_UNIT &&
                                target->ToCreature()->GetCreatureTemplate()->type == CREATURE_TYPE_GAS_CLOUD)
                        {
                            Player* player = caster->ToPlayer();
                            Creature* creature = target->ToCreature();
                            // missing lootid has been reported on startup - just return
                            if (!creature->GetCreatureTemplate()->SkinLootId)
                                return;

                            player->AutoStoreLoot(creature->GetCreatureTemplate()->SkinLootId, LootTemplates_Skinning, true);

                            creature->DespawnOrUnsummon();
                        }
                        return;
                    }
                    // Quake
                    case 30576:
                        triggerSpellId = 30571;
                        break;
                    // Doom
                    // TODO: effect trigger spell may be independant on spell targets, and executed in spell finish phase
                    // so instakill will be naturally done before trigger spell
                    case 31347:
                    {
                        target->CastSpell(target, 31350, true, NULL, this);
                        target->Kill(target);
                        return;
                    }
                    // Spellcloth
                    case 31373:
                    {
                        // Summon Elemental after create item
                        target->SummonCreature(17870, 0, 0, 0, target->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
                        return;
                    }
                    // Flame Quills
                    case 34229:
                    {
                        // cast 24 spells 34269-34289, 34314-34316
                        for (uint32 spell_id = 34269; spell_id != 34290; ++spell_id)
                            target->CastSpell(target, spell_id, true, NULL, this);
                        for (uint32 spell_id = 34314; spell_id != 34317; ++spell_id)
                            target->CastSpell(target, spell_id, true, NULL, this);
                        return;
                    }
                    // Remote Toy
                    case 37027:
                        triggerSpellId = 37029;
                        break;
                    // Eye of Grillok
                    case 38495:
                        triggerSpellId = 38530;
                        break;
                    // Absorb Eye of Grillok (Zezzak's Shard)
                    case 38554:
                    {
                        if (!caster || target->GetTypeId() != TYPEID_UNIT)
                            return;

                        caster->CastSpell(caster, 38495, true, NULL, this);

                        Creature* creatureTarget = target->ToCreature();

                        creatureTarget->DespawnOrUnsummon();
                        return;
                    }
                    // Tear of Azzinoth Summon Channel - it's not really supposed to do anything, and this only prevents the console spam
                    case 39857:
                        triggerSpellId = 39856;
                        break;
                    // Personalized Weather
                    case 46736:
                        triggerSpellId = 46737;
                        break;
                }
                break;
            }
            case SPELLFAMILY_SHAMAN:
            {
                switch (auraId)
                {
                    // Totemic Mastery (Skyshatter Regalia (Shaman Tier 6) - bonus)
                    case 38443:
                    {
                        bool all = true;
                        for (int i = SUMMON_SLOT_TOTEM; i < MAX_TOTEM_SLOT; ++i)
                        {
                            if (!target->m_SummonSlot[i])
                            {
                                all = false;
                                break;
                            }
                        }

                        if (all)
                            target->CastSpell(target, 38437, true, NULL, this);
                        else
                            target->RemoveAurasDueToSpell(38437);
                        return;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    else
    {
        // Spell exist but require custom code
        switch (auraId)
        {
            case 163317: ///< Rushing Jade Wind
            {
                if (caster && caster->IsPlayer())
                    triggerSpellId = 148187;
                break;
            }

            case 210312: ///< Nightmare Brambles (Cenarius)
            {
                if (caster)
                {
                    if (Unit* owner = caster->GetAnyOwner())
                        caster->CastSpell(caster, triggerSpellId, true, nullptr, nullptr, owner->GetGUID());
                }
                return;
            }
            case 191855: //Serpentrix: Toxic Wound
                target->CastSpell(target, 191856, true, nullptr, nullptr, GetCasterGUID());
                return;
            // Pursuing Spikes (Anub'arak)
            case 65920:
            case 65922:
            case 65923:
            {
                Unit* permafrostCaster = NULL;
                Aura* permafrostAura = target->GetAura(66193);
                if (!permafrostAura)
                    permafrostAura = target->GetAura(67855);
                if (!permafrostAura)
                    permafrostAura = target->GetAura(67856);
                if (!permafrostAura)
                    permafrostAura = target->GetAura(67857);

                if (permafrostAura)
                    permafrostCaster = permafrostAura->GetCaster();

                if (permafrostCaster)
                {
                    if (Creature* permafrostCasterCreature = permafrostCaster->ToCreature())
                        permafrostCasterCreature->DespawnOrUnsummon(3000);

                    target->CastSpell(target, 66181, false);
                    target->RemoveAllAuras();
                    if (Creature* targetCreature = target->ToCreature())
                        targetCreature->DisappearAndDie();
                }
                break;
            }
            // Negative Energy Periodic
            case 46284:
                target->CastCustomSpell(triggerSpellId, SPELLVALUE_MAX_TARGETS, m_tickNumber / 10 + 1, NULL, true, NULL, this);
                return;
            // Poison (Grobbulus)
            case 28158:
            case 54362:
            // Slime Pool (Dreadscale & Acidmaw)
            case 66882:
                target->CastCustomSpell(triggerSpellId, SPELLVALUE_RADIUS_MOD, (int32)((((float)m_tickNumber / 60) * 0.9f + 0.1f) * 10000 * 2 / 3), NULL, true, NULL, this);
                return;
            // Beacon of Light
            case 53563:
            {
                // area aura owner casts the spell
                GetBase()->GetUnitOwner()->CastSpell(target, triggeredSpellInfo, true, 0, this, GetBase()->GetUnitOwner()->GetGUID());
                return;
            }
            // Slime Spray - temporary here until preventing default effect works again
            // added on 9.10.2010
            case 69508:
            {
                if (caster)
                    caster->CastSpell(target, triggerSpellId, true, NULL, nullptr, caster->GetGUID());
                return;
            }
            case 24745: // Summon Templar, Trigger
            case 24747: // Summon Templar Fire, Trigger
            case 24757: // Summon Templar Air, Trigger
            case 24759: // Summon Templar Earth, Trigger
            case 24761: // Summon Templar Water, Trigger
            case 24762: // Summon Duke, Trigger
            case 24766: // Summon Duke Fire, Trigger
            case 24769: // Summon Duke Air, Trigger
            case 24771: // Summon Duke Earth, Trigger
            case 24773: // Summon Duke Water, Trigger
            case 24785: // Summon Royal, Trigger
            case 24787: // Summon Royal Fire, Trigger
            case 24791: // Summon Royal Air, Trigger
            case 24792: // Summon Royal Earth, Trigger
            case 24793: // Summon Royal Water, Trigger
            {
                // All this spells trigger a spell that requires reagents; if the
                // triggered spell is cast as "triggered", reagents are not consumed
                if (caster)
                    caster->CastSpell(target, triggerSpellId, false);
                return;
            }
            case 106768: ///< Brew Barrel Periodic Trigger
                return;
            case 5740:  // Rain of Fire
            {
                if (caster && target)
                {
                    if (caster != target)
                        return;
                }
                break;
            }
            case 254261: ///< Shadow Barrage
            {
                if (caster)
                    if (Unit* l_Victim = caster->getVictim())
                        target = l_Victim;

                break;
            }
            default:
                break;
        }
    }

    /// Infernal should not cast his aoe when the player is not in combat
    if (triggerSpellId == 20153 && caster && caster->isGuardian())
    {
        Unit* l_Owner = caster->GetOwner();

        if (l_Owner && !l_Owner->isInCombat())
            return;
    }

    // Reget trigger spell proto
    triggeredSpellInfo = sSpellMgr->GetSpellInfo(triggerSpellId);

    if (triggeredSpellInfo)
    {
        if (Unit* triggerCaster = triggeredSpellInfo->NeedsToBeTriggeredByCaster() ? caster : target)
        {
            uint64 l_CastItemGUID = GetBase()->GetCastItemGUID();
            if (l_CastItemGUID && caster && caster->IsPlayer())
            {
                Item* l_CastItem = caster->ToPlayer()->GetItemByGuid(l_CastItemGUID);
                triggerCaster->CastSpell(target, triggeredSpellInfo, true, l_CastItem, this);
            }
            else
                triggerCaster->CastSpell(target, triggeredSpellInfo, true, NULL, this);
        }
    }
    else
    {
        Creature* c = target->ToCreature();
        if (!c || !caster || !sScriptMgr->OnDummyEffect(caster, GetId(), SpellEffIndex(GetEffIndex()), target->ToCreature()) ||
            !c->AI()->sOnDummyEffect(caster, GetId(), SpellEffIndex(GetEffIndex())))
            sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AuraEffect::HandlePeriodicTriggerSpellAuraTick: Spell %u has non-existent spell %u in EffectTriggered[%d] and is therefor not triggered.", GetId(), triggerSpellId, GetEffIndex());
    }
}

void AuraEffect::HandlePeriodicTriggerSpellWithValueAuraTick(Unit* target, Unit* caster) const
{
    uint32 triggerSpellId = GetSpellInfo()->Effects[m_effIndex].TriggerSpell;
    if (SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(triggerSpellId))
    {
        if (Unit* triggerCaster = triggeredSpellInfo->NeedsToBeTriggeredByCaster() ? caster : target)
        {
            int32 basepoints0 = GetAmount();

            // Hack fix for Protectors of the Endless - Defiled Ground
            if (triggerSpellId == 117988)
                triggerCaster->CastCustomSpell(target, triggerSpellId, 0, &basepoints0, 0, true, 0, this);
            else
                triggerCaster->CastCustomSpell(target, triggerSpellId, &basepoints0, 0, 0, true, 0, this);
        }
    }
}

void AuraEffect::HandlePeriodicDamageAurasTick(Unit* p_Target, Unit* p_Caster) const
{
    if (!p_Caster || !p_Target || !p_Target->isAlive())
        return;

    if (p_Target->HasUnitState(UNIT_STATE_ISOLATED) || p_Target->IsImmunedToDamage(m_spellInfo))
    {
        SendTickImmune(p_Target, p_Caster);
        return;
    }

    // Consecrate ticks can miss and will not show up in the combat log
    if (m_spellInfo->Effects[GetEffIndex()].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
        p_Caster->SpellHitResult(p_Target, m_spellInfo, false) != SPELL_MISS_NONE)
        return;

    // some auras remove at specific health level or more
    if (GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE)
    {
        switch (m_spellInfo->Id)
        {
            case 43093: case 31956: case 38801:  // Grievous Wound
            case 35321: case 38363: case 39215:  // Gushing Wound
                if (p_Target->IsFullHealth())
                {
                    p_Target->RemoveAurasDueToSpell(m_spellInfo->Id);
                    return;
                }
                break;
            case 38772: // Grievous Wound
            {
                uint32 percent = m_spellInfo->Effects[EFFECT_1].CalcValue(p_Caster);
                if (!p_Target->HealthBelowPct(percent))
                {
                    p_Target->RemoveAurasDueToSpell(m_spellInfo->Id);
                    return;
                }
                break;
            }
            case 250669: // Argus - Soulburst
            {
                if (p_Target->IsImmunedToDamage(GetSpellInfo()->GetSchoolMask()))
                {
                    SendTickImmune(p_Target, p_Caster);
                    return;
                }
                break;
            }
        }
    }

    std::ostringstream l_DamageCalculationLog;
    l_DamageCalculationLog << "Aura " << GetId() << std::endl;

    CleanDamage l_CleanDamage = CleanDamage(0, 0, WeaponAttackType::BaseAttack, MELEE_HIT_NORMAL);

    // AOE spells are not affected by the new periodic system.
    bool l_IsAreaAura = m_spellInfo->Effects[m_effIndex].IsAreaAuraEffect() || m_spellInfo->Effects[m_effIndex].IsEffect(SPELL_EFFECT_PERSISTENT_AREA_AURA);
    // ignore negative values (can be result apply spellmods to aura damage
    float l_Damage = std::max(GetSpellInfo()->GetFloatAmount(GetAmount(), GetFloatAmount()), 0.0f);

    l_DamageCalculationLog << "base damage " << l_Damage << std::endl;

    DamageInfo l_DamageInfo(p_Caster, p_Target, l_Damage, GetSpellInfo(), DamageEffectType::DOT, WeaponAttackType::BaseAttack);
    bool l_DynamicDot = IsPeriodicDynamicAmount(m_spellInfo->Id);

    if (GetAuraType() == SPELL_AURA_PERIODIC_DAMAGE || GetAuraType() == SPELL_AURA_PERIODIC_SCHOOL_DAMAGE)
    {
        l_DamageCalculationLog << "damage bonus done " << l_Damage << std::endl;

        if (l_IsAreaAura) ///< Only Warlock's Unstable Affliction have snapshotting in Legion (7.3.5 Build 26365)
        {
            float l_DonePct = GetDonePct(); // Single target DOTs have their % done bonus applied in Aura::HandleAuraSpecificPeriodics

            ///< For non-snapshotting damage mod auras
            Unit::AuraEffectList const& l_AuraEffects = p_Caster->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            for (AuraEffect* l_AuraEff : l_AuraEffects)
                if (l_AuraEff->GetSpellInfo() && l_AuraEff->GetSpellInfo()->IsNonSnapshottingAura())
                    AddPct(l_DonePct, l_AuraEff->GetSpellInfo()->GetFloatAmount(l_AuraEff->GetAmount(), l_AuraEff->GetFloatAmount()));

            l_Damage = uint32(float(l_Damage) * l_DonePct);
            p_Caster->SpellDamagePctDone(p_Target, m_spellInfo, DOT, m_effIndex, &l_DamageCalculationLog); ///< just fill the log
        }
        else
        {
            if (l_DynamicDot)
            {
                l_Damage = float(const_cast<AuraEffect*>(this)->CalculateAmount(p_Caster)); ///< check if update spellPower/Ap
                l_Damage = p_Caster->SpellDamageBonusDone(p_Target, m_spellInfo, l_Damage, GetEffIndex(), DOT, GetBase()->GetStackAmount());
                l_Damage = uint32(float(l_Damage) * p_Caster->SpellDamagePctDone(p_Target, m_spellInfo, DOT, m_effIndex, &l_DamageCalculationLog));

                l_DamageCalculationLog << "pct damage done " << l_Damage << std::endl;

                l_Damage = p_Target->SpellDamageBonusTaken(p_Caster, m_spellInfo, l_Damage, DOT, GetEffIndex(), GetBase()->GetStackAmount());

                l_DamageCalculationLog << "spell damage bonus taken" << l_Damage << std::endl;

                const_cast<AuraEffect*>(this)->SetAmount(l_Damage); ///< Set the amount back to the "base" calculation to avoid stacking of bonuses each tick

                bool prevented = GetBase()->CallScriptEffectPeriodicHandlers(this, p_Target->GetAuraApplication(m_spellInfo->Id, p_Caster->GetObjectGuid(), GetBase()->GetCastItemGUID(), 1 << GetEffIndex()));
                if (prevented)
                    return;

                /// Needed after the call of the OnEffectPeriodic Hook Call in case the amount is altered.
                l_Damage = GetAmount();

                if (l_Damage != GetPeriodicAmount())
                {
                    m_PeriodicAmount = l_Damage;
                    const_cast<AuraEffect*>(this)->SetAmount(l_Damage); ///< Update tooltip
                    const_cast<AuraEffect*>(this)->SetCanBeRecalculated(true);
                }
            }
            else
            {
                l_Damage = p_Caster->SpellDamageBonusDone(p_Target, m_spellInfo, l_Damage, GetEffIndex(), DOT, GetBase()->GetStackAmount());
                l_Damage = uint32(float(l_Damage) * p_Caster->SpellDamagePctDone(p_Target, m_spellInfo, DOT, m_effIndex, &l_DamageCalculationLog));

                l_DamageCalculationLog << "pct damage done " << l_Damage << std::endl;

                l_Damage = p_Target->SpellDamageBonusTaken(p_Caster, m_spellInfo, l_Damage, DOT, GetEffIndex(), GetBase()->GetStackAmount());

                l_DamageCalculationLog << "spell damage bonus taken" << l_Damage << std::endl;
            }
        }

        switch (m_spellInfo->Id)
        {
            /// Ignite should remove Polymorph
            case 12654:
            {
                /// Polymorph
                if (p_Target->IsPolymorphed())
                    p_Target->RemoveAurasDueToSpell(p_Target->getTransForm());

                break;
            }
            /// Roaring Blaze
            case 157736:
            {
                if (Aura* l_RoaringBlazeAura = p_Target->GetAura(205690, p_Caster->GetGUID()))
                {
                    if (SpellInfo const* l_RoaringBlazeSpellInfo = sSpellMgr->GetSpellInfo(205690))
                    {
                        int32 l_Modifier = l_RoaringBlazeSpellInfo->Effects[EFFECT_0].BasePoints;
                        for (uint8 l_i = 0; l_i < l_RoaringBlazeAura->GetStackAmount(); ++l_i)
                            AddPct(l_Damage, l_Modifier);
                    }
                }

                /// Mastery: Chaotic Energies
                if (Player* l_Player = p_Caster->ToPlayer())
                {
                    float l_Mastery = l_Player->GetFloatValue(PLAYER_FIELD_MASTERY);
                    l_Mastery += frand(0, l_Mastery);

                    AddPct(l_Damage, l_Mastery);
                }

                break;
            }
            /// Poisoned Ammo
            case 162543:
            {
                /// To prevent double multiplication
                l_Damage = std::max(GetAmount(), 0);
                break;
            }
            /// Deep Wounds
            case 115767:
            {
                if (Player* _player = GetCaster()->ToPlayer())
                {
                    if (_player->GetActiveSpecializationID() == SPEC_WARRIOR_ARMS)
                        l_Damage *= 2;
                }

                break;
            }
            /// Nether Tempest and Living Bomb deal 85% of damage if used on player
            case 44457:
            case 114923:
            {
                if (p_Target->IsPlayer())
                    l_Damage *= 0.85f;

                break;
            }
            /// Unbound Plague
            case 70911:
            {
                l_Damage *= uint32(pow(1.25f, int32(m_tickNumber)));
                break;
            }
            /// Poison Essence - Portal Keeper Hasabel
            case 246316:
            {
                l_Damage = p_Target->CountPctFromCurHealth(m_spellInfo->Effects[SpellEffIndex::EFFECT_0].BasePoints);
                break;
            }
            default:
                break;
        }

        if (GetAmplitude() && GetTickNumber() <= 1 && GetTotalTicks() == m_spellInfo->GetDuration() / GetAmplitude()) ///< Some spells should not deal damage at first tick of first apply
        {
            switch (GetId())
            {
                case 118253: ///< Serpent Sting
                    l_Damage = 0;
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        l_Damage = uint32(p_Target->CountPctFromMaxHealth((int32)l_Damage));

        /// Apply versatility rating for players
        /// Don't apply For  SPELL_AURA_PERIODIC_DAMAGE and SPELL_AURA_PERIODIC_SCHOOL_DAMAGE because already calculated in m_DonePct
        if (p_Caster->GetSpellModOwner())
            l_Damage += CalculatePct(l_Damage, p_Caster->GetSpellModOwner()->GetRatingBonusValue(CR_VERSATILITY_DAMAGE_DONE) + p_Caster->GetSpellModOwner()->GetTotalAuraModifier(SPELL_AURA_MOD_VERSATILITY_PCT));

        if (GetId() == 111400) ///< Burning Rush
        {
            if (l_Damage >= p_Target->GetHealth())
            {
                GetBase()->SetDuration(1);
                return;
            }
        }

        l_DamageCalculationLog << "apply versality " << l_Damage << std::endl;
    }

    // Calculate armor mitigation
    if (Unit::IsDamageReducedByArmor(GetSpellInfo()->GetSchoolMask(), GetSpellInfo(), GetEffIndex()))
    {
        l_DamageInfo.SetAmount(l_Damage);
        p_Caster->CalcArmorReducedDamage(l_DamageInfo);
        l_Damage = l_DamageInfo.GetAmount();
    }

    l_DamageCalculationLog << "armor mitigation " << l_Damage << std::endl;

    // WoD: Apply factor on damages depending on creature level and expansion
    if ((p_Caster->IsPlayer() || p_Caster->IsPetGuardianStuff()) && p_Target->GetTypeId() == TYPEID_UNIT)
        l_Damage *= p_Caster->CalculateDamageDealtFactor(p_Caster, p_Target->ToCreature());
    else if (p_Caster->GetTypeId() == TYPEID_UNIT && (p_Target->IsPlayer() || p_Target->IsPetGuardianStuff()))
        l_Damage *= p_Caster->CalculateDamageTakenFactor(p_Target, p_Caster->ToCreature());

    l_DamageCalculationLog << "wod dmg factor " << l_Damage << std::endl;

    if (!(m_spellInfo->AttributesEx4 & SPELL_ATTR4_FIXED_DAMAGE))
        if (m_spellInfo->Effects[m_effIndex].IsTargetingArea() || l_IsAreaAura)
        {
            l_Damage = int32(float(l_Damage) * p_Target->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE, m_spellInfo->SchoolMask));
            if (p_Caster->GetTypeId() != TYPEID_PLAYER)
                l_Damage = int32(float(l_Damage) * p_Target->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CREATURE_AOE_DAMAGE_AVOIDANCE, m_spellInfo->SchoolMask));
        }

    l_DamageCalculationLog << "avoidance " << l_Damage << std::endl;

    if (l_DynamicDot && l_Damage != GetPeriodicAmount())
    {
        m_PeriodicAmount = uint32(l_Damage);
        const_cast<AuraEffect*>(this)->SetAmount(uint32(l_Damage)); ///< Update tooltip
        const_cast<AuraEffect*>(this)->SetCanBeRecalculated(true);
    }

    bool l_Crit = CanPeriodicTickCrit(p_Target, p_Caster);

    if (l_Crit)
        l_Damage = p_Caster->SpellCriticalDamageBonus(m_spellInfo, l_Damage, p_Target);

    l_DamageCalculationLog << "critical damage bonus " << l_Damage << std::endl;


    if (m_spellInfo->IsAffectedByResilience())
    {
        int32 l_Dmg = l_Damage;
        p_Caster->ApplyResilience(p_Target, &l_Dmg);
        l_Damage = l_Dmg;
    }

    l_DamageInfo.SetAmount(l_Damage);

    p_Caster->CalcAbsorbResist(l_DamageInfo);

    l_DamageCalculationLog << "calc absorb resist damage " << l_Damage << std::endl;


    p_Caster->DealDamageMods(l_DamageInfo);

    l_Damage = l_DamageInfo.GetAmount();

    l_DamageCalculationLog << "deal damage mods " << l_Damage << std::endl;
    l_DamageCalculationLog << l_DamageInfo.ToString();

    // Set trigger flag
    uint32 l_ProcAttacker = 0;
    uint32 l_ProcVictim = 0;
    uint32 l_ProcEx = 0;

    /// TODO: refactor Spell::CanSpellProc
    /// Just cannot use SPELL_ATTR3_CANT_TRIGGER_PROC because there are many spells having periodic damage effects
    /// So handle if the caster damage himself now
    bool l_CanTriggerProc = GetCaster() != p_Target || !(GetSpellInfo()->AttributesEx3 & SPELL_ATTR3_CANT_TRIGGER_PROC);
    if (l_CanTriggerProc)
    {
        l_ProcAttacker = PROC_FLAG_DONE_PERIODIC;
        l_ProcVictim   = PROC_FLAG_TAKEN_PERIODIC;
        l_ProcEx = (l_Crit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT) | PROC_EX_INTERNAL_DOT;
        if (l_DamageInfo.GetAbsorb())
            l_ProcEx |= PROC_EX_ABSORB;

        if (l_Damage)
            l_ProcVictim |= PROC_FLAG_TAKEN_DAMAGE;
    }

    int32 l_Overkill = l_Damage - p_Target->GetHealth(GetCaster());
    if (l_Overkill < 0)
        l_Overkill = 0;

    if (p_Target && p_Target->ToPlayer() && p_Target->getClass() == CLASS_MONK && GetSpellInfo())
    {
        switch (GetSpellInfo()->Id)
        {
            case Stagger::HEAVY_STAGGER:
            case Stagger::MODERATE_STAGGER:
            case Stagger::LIGHT_STAGGER:
            {
                if (l_Damage >= p_Target->GetHealth())
                {
                    l_Damage = p_Target->GetHealth() - 2;
                    l_Overkill = 0;
                }
                break;
            }
            default:
                break;
        }
    }

    l_DamageInfo.SetOverHeal(l_Overkill);

    m_PeriodicHitDamage = l_Damage;

    p_Caster->ProcDamageAndSpell(p_Target, l_ProcAttacker, l_ProcVictim, l_ProcEx, l_DamageInfo, nullptr, GetBase());
    l_Damage = p_Caster->DealDamage(p_Target, l_Damage, &l_CleanDamage, DOT, l_DamageInfo.GetSchoolMask(), GetSpellInfo(), true, l_DamageInfo.GetAbsorb());

    SpellPeriodicAuraLogInfo l_PInfo(this, l_Damage, l_Overkill, l_DamageInfo.GetAbsorb(), l_DamageInfo.GetResist(), 0.0f, l_Crit);
    p_Target->SendPeriodicAuraLog(&l_PInfo);
}

void AuraEffect::HandlePeriodicHealthLeechAuraTick(Unit* p_Target, Unit* p_Caster) const
{
    if (!p_Caster || !p_Target->isAlive())
        return;

    if (p_Target->HasUnitState(UNIT_STATE_ISOLATED) || p_Target->IsImmunedToDamage(GetSpellInfo()))
    {
        SendTickImmune(p_Target, p_Caster);
        return;
    }

    if (GetSpellInfo()->Effects[GetEffIndex()].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
        p_Caster->SpellHitResult(p_Target, GetSpellInfo(), false) != SPELL_MISS_NONE)
        return;

    bool l_IsAreaAura = m_spellInfo->Effects[m_effIndex].IsAreaAuraEffect() || m_spellInfo->Effects[m_effIndex].IsEffect(SPELL_EFFECT_PERSISTENT_AREA_AURA);
    // ignore negative values (can be result apply spellmods to aura damage
    uint32 l_Damage = std::max(GetAmount(), 0);

    bool I_DynamicDot = IsPeriodicDynamicAmount(m_spellInfo->Id);
    if (I_DynamicDot)
        l_Damage = uint32(const_cast<AuraEffect*>(this)->CalculateAmount(p_Caster)); ///< check if update spellPower/Ap

    l_Damage = p_Caster->SpellDamageBonusDone(p_Target, m_spellInfo, l_Damage, GetEffIndex(), DOT, GetBase()->GetStackAmount());

    uint32 l_UnmitigatedDamage = l_Damage;

    if (l_IsAreaAura)
        l_Damage = uint32(float(l_Damage) * p_Caster->SpellDamagePctDone(p_Target, m_spellInfo, DOT, m_effIndex));
    else
        l_Damage = uint32(float(l_Damage) * p_Caster->SpellDamagePctDone(p_Target, m_spellInfo, DOT, m_effIndex));

    l_Damage = p_Target->SpellDamageBonusTaken(p_Caster, GetSpellInfo(), l_Damage, DOT, GetEffIndex(), GetBase()->GetStackAmount());

    switch (m_spellInfo->Id)
    {
        /// Leech Essence - Lady Dacidion
        case 244915:
        {
            l_Damage = p_Target->CountPctFromCurHealth(m_spellInfo->Effects[EFFECT_0].BasePoints);
            break;
        }
        default:
            break;
    }

    SpellNonMeleeDamage l_DamageInfo(p_Caster, p_Target, l_Damage, l_UnmitigatedDamage, GetSpellInfo(), DamageEffectType::DOT, WeaponAttackType::BaseAttack);

    // Calculate armor mitigation
    if (Unit::IsDamageReducedByArmor(l_DamageInfo.GetSchoolMask(), l_DamageInfo.GetSpellInfo(), GetEffIndex()))
    {
        p_Caster->CalcArmorReducedDamage(l_DamageInfo);
        l_Damage = l_DamageInfo.GetAmount();
    }

    if (!(m_spellInfo->AttributesEx4 & SPELL_ATTR4_FIXED_DAMAGE))
        if (m_spellInfo->Effects[m_effIndex].IsTargetingArea() || l_IsAreaAura)
        {
            l_Damage = int32(float(l_Damage) * p_Target->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE, m_spellInfo->SchoolMask));
            if (p_Caster->GetTypeId() != TYPEID_PLAYER)
                l_Damage = int32(float(l_Damage) * p_Target->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CREATURE_AOE_DAMAGE_AVOIDANCE, m_spellInfo->SchoolMask));
        }

    if (I_DynamicDot && l_Damage != GetPeriodicAmount())
    {
        m_PeriodicAmount = l_Damage;
        const_cast<AuraEffect*>(this)->SetAmount(l_Damage); ///< Update tooltip
        const_cast<AuraEffect*>(this)->SetCanBeRecalculated(true);
    }

    bool l_Crit = CanPeriodicTickCrit(p_Target, p_Caster);

    if (l_Crit)
        l_Damage = p_Caster->SpellCriticalDamageBonus(m_spellInfo, l_Damage, p_Target);

    int32 l_Dmg = l_Damage;
    p_Caster->ApplyResilience(p_Target, &l_Dmg);
    l_Damage = l_Dmg;

    l_DamageInfo.SetAmount(l_Damage);

    p_Caster->CalcAbsorbResist(l_DamageInfo);

    if (l_Crit)
        l_DamageInfo.HitInfo |= SPELL_HIT_TYPE_CRIT;

    p_Caster->SendSpellNonMeleeDamageLog(l_DamageInfo);

    // Set trigger flag
    uint32 l_ProcAttacker = PROC_FLAG_DONE_PERIODIC;
    uint32 l_ProcVictim   = PROC_FLAG_TAKEN_PERIODIC;
    uint32 l_ProcEx = (l_Crit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT) | PROC_EX_INTERNAL_DOT;

    if (l_Damage)
        l_ProcVictim |= PROC_FLAG_TAKEN_DAMAGE;

    if (p_Caster->isAlive())
        p_Caster->ProcDamageAndSpell(p_Target, l_ProcAttacker, l_ProcVictim, l_ProcEx, l_DamageInfo);

    CleanDamage l_CleanDamage = CleanDamage(l_DamageInfo.GetAmount(), l_DamageInfo.GetAbsorb(), WeaponAttackType::BaseAttack, MELEE_HIT_NORMAL);
    int32 l_NewDamage = p_Caster->DealDamage(p_Target, l_DamageInfo.GetAmount(), &l_CleanDamage, l_DamageInfo.GetDamageType(), l_DamageInfo.GetSchoolMask(), l_DamageInfo.GetSpellInfo(), false, l_DamageInfo.GetAbsorb());

    if (p_Caster->isAlive())
    {
        float l_GainMultiplier = GetSpellInfo()->Effects[GetEffIndex()].CalcValueMultiplier(p_Caster);

        uint32 l_Heal = uint32(p_Caster->SpellHealingBonusDone(p_Caster, GetSpellInfo(), uint32(l_NewDamage * l_GainMultiplier), GetEffIndex(), DOT, GetBase()->GetStackAmount()));
        l_Heal = uint32(p_Caster->SpellHealingBonusTaken(p_Caster, GetSpellInfo(), l_Heal, DOT, GetBase()->GetStackAmount()));

        int32 l_Gain = p_Caster->HealBySpell(p_Caster, GetSpellInfo(), l_Heal);
        p_Caster->getHostileRefManager().threatAssist(p_Caster, l_Gain * 0.5f, GetSpellInfo());
    }
}

void AuraEffect::HandlePeriodicHealthFunnelAuraTick(Unit* p_Target, Unit* p_Caster) const
{
    if (!p_Caster || !p_Caster->isAlive() || !p_Target->isAlive())
        return;

    if (p_Target->HasUnitState(UNIT_STATE_ISOLATED))
    {
        SendTickImmune(p_Target, p_Caster);
        return;
    }

    uint32 l_Damage = std::max(GetAmount(), 0);
    // do not kill health donator
    if (p_Caster->GetHealth() < l_Damage)
        l_Damage = p_Caster->GetHealth() - 1;
    if (!l_Damage)
        return;

    p_Caster->ModifyHealth(-(int32)l_Damage);

    float l_GainMultiplier = GetSpellInfo()->Effects[GetEffIndex()].CalcValueMultiplier(p_Caster);

    l_Damage = int32(l_Damage * l_GainMultiplier);

    p_Caster->HealBySpell(p_Target, GetSpellInfo(), l_Damage);
}

void AuraEffect::HandlePeriodicHealAurasTick(Unit* p_Target, Unit* p_Caster) const
{
    if (!p_Caster || !p_Target->isAlive())
        return;

    if (p_Target->HasUnitState(UNIT_STATE_ISOLATED))
    {
        SendTickImmune(p_Target, p_Caster);
        return;
    }

    // heal for caster damage (must be alive)
    if (p_Target != p_Caster && GetSpellInfo()->AttributesEx2 & SPELL_ATTR2_HEALTH_FUNNEL && !p_Caster->isAlive())
        return;

    // don't regen when permanent aura target has full power
    if (GetBase()->IsPermanent() && p_Target->IsFullHealth() && GetId() != 204262)
        return;

    bool l_IsAreaAura = m_spellInfo->Effects[m_effIndex].IsAreaAuraEffect() || m_spellInfo->Effects[m_effIndex].IsEffect(SPELL_EFFECT_PERSISTENT_AREA_AURA);
    // ignore negative values (can be result apply spellmods to aura damage
    int32 l_Damage = std::max(GetAmount(), 0);

    bool I_DynamicHot = IsPeriodicDynamicAmount(m_spellInfo->Id);
    if (I_DynamicHot)
        l_Damage = int32(const_cast<AuraEffect*>(this)->CalculateAmount(p_Caster)); ///< check if update spellPower/Ap

    if (GetAuraType() == SPELL_AURA_OBS_MOD_HEALTH)
    {
        // Taken mods
        float l_TakenTotalMod = 1.0f;

        // Tenacity increase healing % taken
        if (AuraEffect const* l_Tenacity = p_Target->GetAuraEffect(58549, 0))
            AddPct(l_TakenTotalMod, l_Tenacity->GetAmount());

        l_TakenTotalMod = std::max(l_TakenTotalMod, 0.0f);

        if (p_Target->HasAura(193546) && GetId() == 185311)
            l_Damage = uint32(p_Target->CountPctFromMaxHealth(float(6.5), p_Caster));
        else
            l_Damage = uint32(p_Target->CountPctFromMaxHealth(l_Damage, p_Caster));

        switch (m_spellInfo->Id)
        {
            case 154149: // Energize (Skyreach), hackfix from client point of view. So hack fix on server side.
                l_Damage /= 100;
                break;
            default:
                break;
        }

        l_Damage = uint32(l_Damage * l_TakenTotalMod);

        l_Damage = p_Caster->SpellHealingBonusDone(p_Target, GetSpellInfo(), l_Damage, GetEffIndex(), DOT, GetBase()->GetStackAmount());
        l_Damage = p_Target->SpellHealingBonusTaken(p_Caster, GetSpellInfo(), l_Damage, DOT, GetBase()->GetStackAmount());
    }
    else
    {
        l_Damage = p_Caster->SpellHealingBonusDone(p_Target, GetSpellInfo(), l_Damage, GetEffIndex(), DOT, GetBase()->GetStackAmount());

        if (l_IsAreaAura)
            l_Damage = uint32(float(l_Damage) * p_Caster->SpellHealingPctDone(p_Target, m_spellInfo));
        else if (m_spellInfo->Id == 77489) {}   ///< Fix for Holy Priest mastery: Echo of Light which shouldnt be affected by mods, but still should be affected by dampening (double debuff baby !) PS: attribute no_done_bonus doesnt work here
        else
        {
            float l_DonePct =  GetDonePct(); // Single target HOTs have their % done bonus applied in Aura::HandleAuraSpecificPeriodics

            ///< For non-snapshotting heal mod auras
            auto const l_AuraEffects = p_Caster->GetAuraEffectsByType(SPELL_AURA_MOD_HEALING_DONE_PERCENT);
            for (AuraEffect* l_AuraEff : l_AuraEffects)
                if (l_AuraEff->GetSpellInfo() && l_AuraEff->GetSpellInfo()->IsNonSnapshottingAura())
                    AddPct(l_DonePct, l_AuraEff->GetSpellInfo()->GetFloatAmount(l_AuraEff->GetAmount(), l_AuraEff->GetFloatAmount()));

            l_Damage = uint32(float(l_Damage) * l_DonePct);
        }

        l_Damage = p_Target->SpellHealingBonusTaken(p_Caster, GetSpellInfo(), l_Damage, DOT, GetBase()->GetStackAmount());
    }

    if (I_DynamicHot && l_Damage != GetPeriodicAmount())
    {
        m_PeriodicAmount = l_Damage;
        const_cast<AuraEffect*>(this)->SetAmount(l_Damage); ///< Update tooltip
        const_cast<AuraEffect*>(this)->SetCanBeRecalculated(true);
    }

    bool l_Crit = CanPeriodicTickCrit(p_Target, p_Caster);

    if (l_Crit)
        l_Damage = p_Caster->SpellCriticalHealingBonus(m_spellInfo, l_Damage, p_Target);

    uint32 l_Absorb = 0;
    uint32 l_Heal = uint32(l_Damage);

    /// Crimson Vial heals for 50% in PvP
    if ((GetBase()->GetSpellInfo()->Id == 185311 || GetBase()->GetSpellInfo()->Id == 212198)
        && GetCaster()->CanApplyPvPSpellModifiers())
        l_Heal /= 2;

    /// Greater Blessing of Wisdom heals 0.2% per 2 sec
    if (GetBase()->GetSpellInfo()->Id == 203539)
        l_Heal = CalculatePct(l_Heal, 20);

    GetBase()->SetPeriodicHealDone(l_Heal);
    p_Caster->CalcHealAbsorb(p_Target, GetSpellInfo(), l_Heal, l_Absorb);
    int32 l_Gain = p_Caster->DealHeal(p_Target, l_Heal, GetSpellInfo());

    DamageInfo damageInfo(p_Caster, p_Target, l_Damage, GetSpellInfo(), DamageEffectType::HEAL, WeaponAttackType::BaseAttack);
    damageInfo.Absorb(l_Absorb);

    SpellPeriodicAuraLogInfo pInfo(this, l_Heal, l_Heal - l_Gain, l_Absorb, 0, 0.0f, l_Crit);
    p_Target->SendPeriodicAuraLog(&pInfo);

    damageInfo.SetOverHeal(int32(l_Heal - l_Gain));

    p_Target->getHostileRefManager().threatAssist(p_Caster, float(l_Gain) * 0.5f, GetSpellInfo());

    bool l_HaveCastItem = GetBase()->GetCastItemGUID() != 0;

    uint32 l_ProcAttacker = PROC_FLAG_DONE_PERIODIC;
    uint32 l_ProcVictim   = PROC_FLAG_TAKEN_PERIODIC;
    uint32 l_ProcEx = (l_Crit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT) | PROC_EX_INTERNAL_HOT;
    // ignore item heals
    if (!l_HaveCastItem)
        p_Caster->ProcDamageAndSpell(p_Target, l_ProcAttacker, l_ProcVictim, l_ProcEx, damageInfo);
}

void AuraEffect::HandlePeriodicManaLeechAuraTick(Unit* target, Unit* caster) const
{
    Powers powerType = Powers(GetMiscValue());

    if (!caster || !caster->isAlive() || !target->isAlive() || target->getPowerType() != powerType)
        return;

    if (target->HasUnitState(UNIT_STATE_ISOLATED) || target->IsImmunedToDamage(GetSpellInfo()))
    {
        SendTickImmune(target, caster);
        return;
    }

    if (GetSpellInfo()->Effects[GetEffIndex()].Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
        caster->SpellHitResult(target, GetSpellInfo(), false) != SPELL_MISS_NONE)
        return;

    // ignore negative values (can be result apply spellmods to aura damage
    int32 drainAmount = std::max(m_amount, 0);

    // Special case: draining x% of mana (up to a maximum of 2*x% of the caster's maximum mana)
    // It's mana percent cost spells, m_amount is percent drain from target
    for (auto itr : GetSpellInfo()->SpellPowers)
    {
        if (itr->PowerType == POWER_MANA && itr->CostBasePercentage)
        {
            // max value
            int32 maxmana = CalculatePct(caster->GetMaxPower(powerType), drainAmount * 2.0f);
            ApplyPct(drainAmount, target->GetMaxPower(powerType));
            if (drainAmount > maxmana)
                drainAmount = maxmana;
            break;
        }
    }

    int32 drainedAmount = -target->ModifyPower(powerType, -drainAmount);

    float gainMultiplier = GetSpellInfo()->Effects[GetEffIndex()].CalcValueMultiplier(caster);

    SpellPeriodicAuraLogInfo pInfo(this, drainedAmount, 0, 0, 0, gainMultiplier, false);
    target->SendPeriodicAuraLog(&pInfo);

    int32 gainAmount = int32(drainedAmount * gainMultiplier);
    int32 gainedAmount = 0;
    if (gainAmount)
    {
        gainedAmount = caster->ModifyPower(powerType, gainAmount);
        target->AddThreat(caster, float(gainedAmount) * 0.5f, GetSpellInfo()->GetSchoolMask(), GetSpellInfo());
    }

    // Drain Mana
    if (m_spellInfo->SpellFamilyName == SPELLFAMILY_WARLOCK
        && m_spellInfo->SpellFamilyFlags[0] & 0x00000010)
    {
        int32 manaFeedVal = 0;
        if (AuraEffect const* aurEff = GetBase()->GetEffect(1))
            manaFeedVal = aurEff->GetAmount();
        // Mana Feed - Drain Mana
        if (manaFeedVal > 0)
        {
            int32 feedAmount = CalculatePct(gainedAmount, manaFeedVal);
            caster->CastCustomSpell(caster, 32554, &feedAmount, NULL, NULL, true, NULL, this);
        }
    }
}

void AuraEffect::HandleObsModPowerAuraTick(Unit* p_Target, Unit* p_Caster) const
{
    Powers l_PowerType;
    if (GetMiscValue() == POWER_ALL)
        l_PowerType = p_Target->getPowerType();
    else
        l_PowerType = Powers(GetMiscValue());

    if (!p_Target->isAlive() || !p_Target->GetMaxPower(l_PowerType))
        return;

    if (p_Target->HasUnitState(UNIT_STATE_ISOLATED))
    {
        SendTickImmune(p_Target, p_Caster);
        return;
    }

    // don't regen when permanent aura target has full power
    if (GetBase()->IsPermanent() && p_Target->GetPower(l_PowerType) == p_Target->GetMaxPower(l_PowerType))
        return;

    // ignore negative values (can be result apply spellmods to aura damage
    uint32 l_Amount = std::max(m_amount, 0) * p_Target->GetMaxPower(l_PowerType) / 100;

    /// Greater Blessing of Wisdom gain power 0.2% per 2 sec
    if (GetBase()->GetSpellInfo()->Id == 203539)
        l_Amount = CalculatePct(l_Amount, 20);

    SpellPeriodicAuraLogInfo l_pInfo(this, l_Amount, 0, 0, 0, 0.0f, false);
    p_Target->SendPeriodicAuraLog(&l_pInfo);

    int32 l_Gain = p_Target->ModifyPower(l_PowerType, l_Amount);

    if (p_Caster)
        p_Target->getHostileRefManager().threatAssist(p_Caster, float(l_Gain) * 0.5f, GetSpellInfo());
}

void AuraEffect::HandlePeriodicEnergizeAuraTick(Unit* target, Unit* caster) const
{
    Powers powerType = Powers(GetMiscValue());

    if (target->IsPlayer() && target->getPowerType() != powerType && powerType < POWER_RUNES && powerType != POWER_COMBO_POINT)
        return;

    if (!target->isAlive() || !target->GetMaxPower(powerType))
        return;

    if (target->HasUnitState(UNIT_STATE_ISOLATED))
    {
        SendTickImmune(target, caster);
        return;
    }

    // don't regen when permanent aura target has full power
    if (GetBase()->IsPermanent() && target->GetPower(powerType) == target->GetMaxPower(powerType))
        return;

    // ignore negative values (can be result apply spellmods to aura damage
    int32 amount = std::max(m_amount, 0);

    SpellPeriodicAuraLogInfo pInfo(this, amount, 0, 0, 0, 0.0f, false);
    target->SendPeriodicAuraLog(&pInfo);

    int32 gain = target->ModifyPower(powerType, amount);

    if (powerType == POWER_RUNES)
    {
        Player* l_Player = target->ToPlayer();
        if (l_Player)
            for (int i = 0; i < gain; ++i)
                l_Player->RestoreRune();
    }

    if (caster)
        target->getHostileRefManager().threatAssist(caster, float(gain) * 0.5f, GetSpellInfo());
}

void AuraEffect::HandlePeriodicPowerBurnAuraTick(Unit* p_Target, Unit* p_Caster) const
{
    Powers l_PowerType = Powers(GetMiscValue());

    if (!p_Caster || !p_Target->isAlive() || p_Target->getPowerType() != l_PowerType)
        return;

    if (p_Target->HasUnitState(UNIT_STATE_ISOLATED) || p_Target->IsImmunedToDamage(GetSpellInfo()))
    {
        SendTickImmune(p_Target, p_Caster);
        return;
    }

    // ignore negative values (can be result apply spellmods to aura damage
    int32 l_Damage = std::max(m_amount, 0);

    uint32 l_Gain = uint32(-p_Target->ModifyPower(l_PowerType, -l_Damage));

    float l_DamageMultiplier = GetSpellInfo()->Effects[GetEffIndex()].CalcValueMultiplier(p_Caster);

    SpellInfo const* l_SpellProto = GetSpellInfo();
    // maybe has to be sent different to client, but not by SMSG_PERIODICAURALOG
    SpellNonMeleeDamage l_DamageInfo(p_Caster, p_Target, int32(l_Gain * l_DamageMultiplier), l_SpellProto, DamageEffectType::DOT, GetBase()->GetSpellXSpellVisualID());
    // no SpellDamageBonus for burn mana
    p_Caster->CalculateSpellDamageTaken(l_DamageInfo);
    p_Caster->DealDamageMods(l_DamageInfo);

    p_Caster->SendSpellNonMeleeDamageLog(l_DamageInfo);

    // Set trigger flag
    uint32 l_ProcAttacker = PROC_FLAG_DONE_PERIODIC;
    uint32 l_ProcVictim   = PROC_FLAG_TAKEN_PERIODIC;
    uint32 l_ProcEx       = createProcExtendMask(l_DamageInfo, SPELL_MISS_NONE) | PROC_EX_INTERNAL_DOT;
    if (l_DamageInfo.GetAmount())
        l_ProcVictim |= PROC_FLAG_TAKEN_DAMAGE;

    p_Caster->ProcDamageAndSpell(l_DamageInfo.GetTarget(), l_ProcAttacker, l_ProcVictim, l_ProcEx, l_DamageInfo);

    p_Caster->DealSpellDamage(l_DamageInfo, true);
}

void AuraEffect::HandleProcTriggerSpellAuraProc(AuraApplication* aurApp, ProcEventInfo& eventInfo)
{
    Unit* triggerCaster = aurApp->GetTarget();
    Unit* triggerTarget = eventInfo.GetProcTarget();

    uint32 triggerSpellId = GetSpellInfo()->Effects[GetEffIndex()].TriggerSpell;
    if (SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(triggerSpellId))
    {
        triggerCaster->CastSpell(triggerTarget, triggeredSpellInfo, true, NULL, this);
    }
}

void AuraEffect::HandleProcTriggerSpellWithValueAuraProc(AuraApplication* aurApp, ProcEventInfo& eventInfo)
{
    Unit* triggerCaster = aurApp->GetTarget();
    Unit* triggerTarget = eventInfo.GetProcTarget();

    uint32 triggerSpellId = GetSpellInfo()->Effects[m_effIndex].TriggerSpell;
    if (SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(triggerSpellId))
    {
        int32 basepoints0 = GetAmount();
        triggerCaster->CastCustomSpell(triggerTarget, triggerSpellId, &basepoints0, NULL, NULL, true, NULL, this);
    }
}

void AuraEffect::HandleProcTriggerDamageAuraProc(AuraApplication* p_AuraApplication, ProcEventInfo& p_ProcEventInfo)
{
    Unit* l_Target = p_AuraApplication->GetTarget();
    Unit* l_TriggerTarget = p_ProcEventInfo.GetProcTarget();

    uint32 l_Damage = l_Target->SpellDamageBonusDone(l_TriggerTarget, GetSpellInfo(), GetAmount(), GetEffIndex(), SPELL_DIRECT_DAMAGE);
    uint32 l_UnmitigatedDamage = l_Damage;
    l_Damage = l_TriggerTarget->SpellDamageBonusTaken(l_Target, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, GetEffIndex());

    SpellNonMeleeDamage l_DamageInfo(l_Target, l_TriggerTarget, l_Damage, l_UnmitigatedDamage, GetSpellInfo(), SPELL_DIRECT_DAMAGE, GetBase()->GetSpellXSpellVisualID());
    l_Target->CalculateSpellDamageTaken(l_DamageInfo);
    l_Target->DealDamageMods(l_DamageInfo);

    l_Target->SendSpellNonMeleeDamageLog(l_DamageInfo);
    l_Target->DealSpellDamage(l_DamageInfo, true);
}

void AuraEffect::HandleRaidProcFromChargeAuraProc(AuraApplication* aurApp, ProcEventInfo& /*eventInfo*/)
{
    Unit* target = aurApp->GetTarget();

    uint32 triggerSpellId;
    switch (GetId())
    {
        case 57949:            // Shiver
            triggerSpellId = 57952;
            //animationSpellId = 57951; dummy effects for jump spell have unknown use (see also 41637)
            break;
        case 59978:            // Shiver
            triggerSpellId = 59979;
            break;
        case 43593:            // Cold Stare
            triggerSpellId = 43594;
            break;
        default:
            sLog->outDebug(LOG_FILTER_SPELLS_AURAS, "AuraEffect::HandleRaidProcFromChargeAuraProc: received not handled spell: %u", GetId());
            return;
    }

    int32 jumps = GetBase()->GetCharges();

    // current aura expire on proc finish
    GetBase()->SetCharges(0);
    GetBase()->SetUsingCharges(true);

    // next target selection
    if (jumps > 0)
    {
        if (Unit* caster = GetCaster())
        {
            float radius = GetSpellInfo()->Effects[GetEffIndex()].CalcRadius(caster);

            if (Unit* triggerTarget = target->GetNextRandomRaidMemberOrPet(radius))
            {
                target->CastSpell(triggerTarget, GetSpellInfo(), true, NULL, this, GetCasterGUID());
                Aura* aura = triggerTarget->GetAura(GetId(), GetCasterGUID());
                if (aura != nullptr)
                    aura->SetCharges(jumps);
            }
        }
    }

    target->CastSpell(target, triggerSpellId, true, NULL, this, GetCasterGUID());
}

void AuraEffect::HandleAuraForceWeather(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Player* target = aurApp->GetTarget()->ToPlayer();

    if (!target)
        return;

    if (apply)
    {
        WorldPacket l_Data(SMSG_WEATHER, (4 + 4 + 4));
        l_Data << uint32(GetMiscValue());
        l_Data << float(1.0f);
        l_Data.WriteBit(0);
        l_Data.FlushBits();

        target->GetSession()->SendPacket(&l_Data);
    }
    else
    {
        // send weather for current zone
        if (Weather* weather = WeatherMgr::FindWeather(target->GetZoneId()))
            weather->SendWeatherUpdateToPlayer(target);
        else
        {
            if (!WeatherMgr::AddWeather(target->GetZoneId()))
            {
                // send fine weather packet to remove old weather
                WeatherMgr::SendFineWeatherUpdateToPlayer(target);
            }
        }
    }
}

void AuraEffect::HandleEnableAltPower(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (!l_Target)
        return;

    if (!p_Apply)
    {
        l_Target->SetMaxPower(POWER_ALTERNATE_POWER, 0);
        l_Target->SetPower(POWER_ALTERNATE_POWER, 0);
        return;
    }

    uint32 l_StartPower = 0;
    uint32 l_MaxPower = 0;

    switch (GetMiscValue())
    {
        case 80:
            l_MaxPower = 3;
            break;
        case 32:
        case 89:
            l_MaxPower = 4;
            break;
        case 30:
        case 34:
        case 90:
            l_MaxPower = 5;
            break;
        case 33:
        case 35:
            l_MaxPower = 7;
            break;
        case 88:
            l_MaxPower = 10;
            break;
        case 117:
            l_MaxPower = 25;
            break;
        case 129:
        case 133:
            l_MaxPower = 30;
            break;
        case 29:
            l_MaxPower = 34;
            break;
        case 114:
        case 203:
            l_MaxPower = 40;
            break;
        case 64:
            l_MaxPower = 50;
            break;
        case 84:
            l_MaxPower = 60;
            break;
        case 137:
        case 149:
        case 195:
            l_MaxPower = 90;
            break;
        case 23:
        case 37:
        case 39:
        case 61:
        case 65:
        case 68:
        case 69:
        case 72:
        case 78:
        case 91:
        case 93:
        case 94:
        case 103:
        case 107:
        case 110:
        case 113:
        case 134:
        case 148:
        case 151:
        case 165:
        case 176:
        case 178:
        case 179:
        case 183:
        case 199:
        case 204:
        case 205:
        case 206:
        case 207:
        default:
            l_MaxPower = 100;
            break;
        case 63:
            l_MaxPower = 105;
            break;
        case 87:
            l_MaxPower = 120;
            break;
        case 66:
        case 67:
            l_MaxPower = 180;
            break;
        case 24:
            l_MaxPower = 250;
            break;
        case 26:
            l_MaxPower = 300;
            break;
        case 158:
            l_MaxPower = 700;
            break;
        case 36:
            l_MaxPower = 35000;
            break;
    }

    switch (GetMiscValue())
    {
        case 89:
            l_StartPower = 4;
            break;
        case 34:
        case 90:
        case 204:
        case 205:
        case 207:
            l_StartPower = 5;
            break;
        case 103:
            l_StartPower = 10;
            break;
        case 64:
            l_StartPower = 25;
            break;
        case 87:
        case 93:
        case 148:
        case 151:
        case 176:
        case 183:
            l_StartPower = 50;
            break;
        case 178:
            l_StartPower = 100;
            break;
        default:
            l_StartPower = 0;
            break;
    }

    l_Target->SetMaxPower(POWER_ALTERNATE_POWER, l_MaxPower);
    l_Target->SetPower(POWER_ALTERNATE_POWER, l_StartPower);
}

void AuraEffect::HandleAuraStrangulate(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    target->SetControlled(apply, UNIT_STATE_STUNNED);

    if (apply)
        target->UpdateHeight(1.0f);
    else
        target->UpdateHeight(0.0f);
}

void AuraEffect::HandleModManaRegenByHaste(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    if (target->IsPlayer())
        target->ToPlayer()->UpdateManaRegen();
}

void AuraEffect::HandleAuraModPetStats(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    Player* plrTarget = target->ToPlayer();
    if (!plrTarget)
        return;

    Guardian* pet = plrTarget->GetGuardianPet();
    if (!pet)
        return;

    pet->UpdateAllStats();
}

void AuraEffect::HandleAuraModMaxPower(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    Powers power = Powers(GetMiscValue());

    target->UpdateMaxPower(power);
}

void AuraEffect::HandleChangeSpellVisualEffect(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    Player* player = target->ToPlayer();
    if (player == NULL)
        return;

    uint32 spellToReplace = apply ? GetMiscValue() : 0;
    uint32 replacer = apply ? m_spellInfo->Id : 0;

    player->SetDynamicValue(UNIT_DYNAMIC_FIELD_PASSIVE_SPELLS, 0, spellToReplace);
    player->SetDynamicValue(UNIT_DYNAMIC_FIELD_PASSIVE_SPELLS, 1, replacer);
}

void AuraEffect::HandleAuraModifyManaRegenFromManaPct(AuraApplication const* aurApp, uint8 mode, bool /*apply*/) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* target = aurApp->GetTarget();
    if (!target)
        return;

    Player* player = target->ToPlayer();
    if (!player)
        return;

    player->UpdateManaRegen();
}

void AuraEffect::HandleAuraModifyManaPoolPct(AuraApplication const* p_AurApp, uint8 p_Mode, bool /*p_Apply*/) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (!l_Target)
        return;

    Player* l_Player = l_Target->ToPlayer();
    if (!l_Player)
        return;

    if (l_Player->GetPowerIndex(POWER_MANA, l_Player->getClass()) == MAX_POWERS)
        return;

    float l_Mod   = 1.0f;
    uint32 l_HP;
    uint32 l_Mana = 0;

    AuraType l_AuraType = (AuraType)m_spellInfo->Effects[GetEffIndex()].ApplyAuraName;

    AddPct(l_Mod, l_Player->GetTotalAuraModifier(l_AuraType));

    sObjectMgr->GetPlayerClassLevelInfo(l_Player->getClass(), l_Player->getLevel(), l_HP, l_Mana);

    l_Player->SetMaxPower(POWER_MANA, l_Mod* l_Mana);
}

void AuraEffect::HandleAuraLeech(AuraApplication const* p_AurApp, uint8 p_Mode, bool /*p_Apply*/) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    if (Player* l_Player = p_AurApp->GetTarget()->ToPlayer())
        l_Player->UpdateLeechPercentage();
}

void AuraEffect::HandleAuraVersatility(AuraApplication const* p_AurApp, uint8 p_Mode, bool /*p_Apply*/) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    if (Player* l_Player = p_AurApp->GetTarget()->ToPlayer())
        l_Player->UpdateVersatilityPercentage();
}

void AuraEffect::HandleAuraAddParryPCTOfCSFromGear(AuraApplication const* p_AurApp, uint8 p_Mode, bool /*p_Apply*/) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL) || !p_AurApp->GetTarget())
        return;

    if (Player* l_Player = p_AurApp->GetTarget()->ToPlayer())
        l_Player->UpdateParryPercentage();
}

void AuraEffect::HandleAuraBonusArmor(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK))
        return;

    Player* l_Player = p_AurApp->GetTarget()->ToPlayer();
    if (!l_Player)
        return;

    UnitModifierType l_Type;

    switch (GetAuraType())
    {
        case SPELL_AURA_MOD_BONUS_ARMOR:
            l_Type = TOTAL_VALUE;
            break;
        case SPELL_AURA_MOD_BONUS_ARMOR_PCT:
            l_Type = TOTAL_PCT;
            break;
        default:
            return;
    }

    l_Player->HandleStatModifier(UNIT_MOD_BONUS_ARMOR, l_Type, (float)GetAmount(), p_Apply);
    l_Player->UpdateArmor();
}

void AuraEffect::HandleModStatBonusPercent(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & (AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* l_Target = p_AurApp->GetTarget();

    if (GetMiscValue() < -1 || GetMiscValue() > 4)
    {
        sLog->outWarn(LOG_FILTER_SPELLS_AURAS, "WARNING: Misc Value for SPELL_AURA_MOD_STAT_BONUS_PCT not valid");
        return;
    }

    /// Only players have base stats
    if (l_Target->GetTypeId() != TYPEID_PLAYER)
        return;

    for (int32 l_I = STAT_STRENGTH; l_I < MAX_STATS; ++l_I)
    {
        if (GetMiscValue() == l_I || GetMiscValue() == -1)
        {
            l_Target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + l_I), BASE_PCT_EXCLUDE_CREATE, float(m_amount), p_Apply);

            if (l_Target->IsPlayer() || l_Target->ToCreature()->isPet())
                l_Target->ApplyStatPercentBuffMod(Stats(l_I), float(m_amount), p_Apply);
        }
    }
}

void AuraEffect::HandleAllowUsingGameobjectsWhileMounted(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    if (p_AurApp->GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if (p_Apply)
        p_AurApp->GetTarget()->SetFlag(PLAYER_FIELD_LOCAL_FLAGS, PLAYER_LOCAL_FLAG_CAN_USE_OBJECTS_MOUNTED);
    else if (!p_AurApp->GetTarget()->HasAuraType(SPELL_AURA_ALLOW_USING_GAMEOBJECTS_WHILE_MOUNTED))
        p_AurApp->GetTarget()->RemoveFlag(PLAYER_FIELD_LOCAL_FLAGS, PLAYER_LOCAL_FLAG_CAN_USE_OBJECTS_MOUNTED);
}

void AuraEffect::HandleAreaTrigger(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (!l_Target)
        return;

    // AreaTrigger is removed at the end of his own duration
    if (!p_Apply)
    {
        if (AreaTrigger* l_AreaTrigger = l_Target->GetAreaTrigger(m_spellInfo->Id))
        {
            l_AreaTrigger->UpdateAffectedList(0, AT_ACTION_MOMENT_DESPAWN);//any remove from world
            l_AreaTrigger->Remove();
        }

        return;
    }

    uint32 l_MiscValue = GetMiscValue();
    Position l_Position;
    l_Target->GetPosition(&l_Position);

    Unit* l_Caster = GetCaster();
    /// Not sure if this is a generic thing
    /// Duel's (236273) first areatrigger (9761)(TARGET_UNIT_TARGET_ENEMY) should be casted by this enemy
    if (l_MiscValue == 9761)
        l_Caster = l_Target;

    AreaTrigger* l_AreaTrigger = new AreaTrigger;
    if (!l_AreaTrigger->CreateAreaTrigger(sObjectMgr->GenerateLowGuid(HIGHGUID_AREATRIGGER), l_MiscValue, l_Caster, nullptr, m_spellInfo, l_Position, l_Position, nullptr, l_Target->GetGUID()))
    {
        delete l_AreaTrigger;
        return;
    }

    l_AreaTrigger->SetCreatingEffectGuid(GetBase()->GetCastGUID());
}

void AuraEffect::HandleDisableMovementForce(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Player* player = nullptr;
    if (Unit* target = p_AurApp->GetTarget())
    {
        if (target->ToPlayer())
            player = target->ToPlayer();
        else
            return;
    }

    player->RemoveAllMovementForces();
}

void AuraEffect::HandleAuraSetPowerType(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (l_Target == nullptr || l_Target->GetTypeId() != TYPEID_UNIT)
        return;

    PowerDisplayEntry const* l_PowerDisplay = sPowerDisplayStore.LookupEntry(GetMiscValue());
    if (l_PowerDisplay == nullptr)
        return;

    Powers l_Power = Powers(l_PowerDisplay->PowerType);
    l_Target->setPowerType(l_Power);

    if (p_Apply)
        l_Target->SetUInt32Value(EUnitFields::UNIT_FIELD_OVERRIDE_DISPLAY_POWER_ID, GetMiscValue());
    else
        l_Target->SetUInt32Value(EUnitFields::UNIT_FIELD_OVERRIDE_DISPLAY_POWER_ID, 0);
}

void AuraEffect::HandleAuraAdaptation(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL))
        return;

    Player* l_Player = p_AurApp->GetTarget()->ToPlayer();
    if (!l_Player)
        return;

    if (l_Player->getClass() != CLASS_HUNTER)
        return;

    Pet* l_Pet = l_Player->GetPet();
    if (!l_Pet)
        return;

    ChrSpecializationsEntry const* l_CurrentSpec = sChrSpecializationsStore.LookupEntry(l_Pet->GetSpecialization());
    if (!l_CurrentSpec)
        return;

    l_Pet->SetSpecialization(GetChrSpecializationByIndex(p_Apply ? PET_SPEC_OVERRIDE_CLASS_INDEX : 0, l_CurrentSpec->OrderIndex)->ID);
}

void AuraEffect::HandleAuraIncreaseDualWieldDamage(AuraApplication const* p_AurApp, uint8 p_Mode, bool /*p_Apply*/) const
{
    if (!(p_Mode & AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL))
        return;

    Player* l_Player = p_AurApp->GetBase()->GetUnitOwner()->ToPlayer();
    if (!l_Player)
        return;

    if (l_Player->CanModifyStats())
    {
        l_Player->UpdateDamagePhysical(WeaponAttackType::BaseAttack);

        if (l_Player->CanUseAttackType(WeaponAttackType::OffAttack))
            l_Player->UpdateDamagePhysical(WeaponAttackType::OffAttack);
    }
}

void AuraEffect::HandleBattlegroundFlag(AuraApplication const* p_AurApp, uint8 p_Mode, bool /*p_Apply*/) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Player* l_Target = p_AurApp->GetTarget()->ToPlayer();

    /// When removing flag aura, handle flag drop
    if (!p_AurApp && l_Target)
    {
        if (l_Target->InBattleground())
        {
            if (Battleground* l_Bg = l_Target->GetBattleground())
                l_Bg->EventPlayerDroppedFlag(l_Target);
        }
        else
            sOutdoorPvPMgr->HandleDropFlag(l_Target, GetSpellInfo()->Id);
    }
}

void AuraEffect::HandleAuraDrowning(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Player* l_PlayerTarget = p_AurApp->GetTarget()->ToPlayer();
    if (!l_PlayerTarget)
        return;

    if (p_Apply)
        l_PlayerTarget->SetMirrorTimerFlag(UNDERWATER_INWATER);
    else
        l_PlayerTarget->RemoveMirrorTimerFlag(UNDERWATER_INWATER);
}

void AuraEffect::HandleFixate(AuraApplication const* aurApp, uint8 mode, bool apply) const
{
    if (!(mode & AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* caster = GetCaster();
    if (!caster || !caster->CanHaveThreatList())
        return;

    Unit* target = aurApp->GetTarget();
    if (!target || target == caster)
        return;

    if (apply)
    {
        if (m_spellInfo->Id != 194289 && m_spellInfo->Id != 228221)
            caster->DeleteThreatList();

        caster->AddThreat(target, std::numeric_limits<float>::max());
        caster->TauntApply(target);
    }
    else
        caster->TauntFadeOut(target);
}

void AuraEffect::HandleModTimeRate(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & (AuraEffectHandleModes::AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK | AuraEffectHandleModes::AURA_EFFECT_HANDLE_STAT)))
        return;

    Unit* l_Target = p_AurApp->GetTarget();

    l_Target->ApplyAttackTimePercentMod(WeaponAttackType::BaseAttack, float(GetAmount()), p_Apply);
    l_Target->ApplyAttackTimePercentMod(WeaponAttackType::OffAttack, float(GetAmount()), p_Apply);
    l_Target->ApplyAttackTimePercentMod(WeaponAttackType::RangedAttack, float(GetAmount()), p_Apply);

    float l_OldModTimeRate = l_Target->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE);

    if (GetAmount() > 0)
        l_Target->ApplyPercentModFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE, float(GetAmount()), !p_Apply);
    else
    {
        float l_Mod = float(100.0f / (100.0f - static_cast<float>(-GetAmount())));
        if (!p_Apply)
            l_Mod = 1 / l_Mod;

        l_Target->SetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE, l_Target->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE) * l_Mod);
    }

    l_Target->UpdateSpeed(UnitMoveType::MOVE_RUN, true);
    l_Target->UpdateSpeed(UnitMoveType::MOVE_WALK, true);
    l_Target->UpdateSpeed(UnitMoveType::MOVE_SWIM, true);
    l_Target->UpdateSpeed(UnitMoveType::MOVE_FLIGHT, true);
    l_Target->UpdateSpeed(UnitMoveType::MOVE_RUN_BACK, true);
    l_Target->UpdateSpeed(UnitMoveType::MOVE_SWIM_BACK, true);
    l_Target->UpdateSpeed(UnitMoveType::MOVE_FLIGHT_BACK, true);

    float l_NewModTimeRate = l_Target->GetFloatValue(EUnitFields::UNIT_FIELD_MOD_TIME_RATE);

    /// It's the part of the fix the exploit
    /// Remove the aura immediately on player's logging out
    bool l_Forced = !p_Apply && l_Target->IsPlayer() && l_Target->ToPlayer()->GetSession()->PlayerLogout();

    l_Target->ScheduleUpdateAuraDuration(l_OldModTimeRate, l_NewModTimeRate, !l_Forced);
    if (Player* l_Player = l_Target->ToPlayer())
    {
        l_Player->UpdateCooldowns(l_OldModTimeRate, l_NewModTimeRate);
        l_Player->UpdateManaRegen();
        l_Player->UpdateEnergyRegen();
        l_Player->UpdateAllRunesRegen();
        l_Player->UpdateFocusRegen();
    }
}

void AuraEffect::HandleModDebuffSpeed(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL))
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (!l_Target)
        return;

    float l_Speed = 1.0f;
    AddPct(l_Speed, GetAmount());

    for (auto l_Aura : l_Target->GetAppliedAuras())
    {
        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(l_Aura.first);
        if (!l_SpellInfo || l_SpellInfo->IsPositive())
            continue;

        Aura* l_Base = l_Aura.second->GetBase();
        if (!l_Base)
            continue;

        if (p_Apply)
            l_Base->SetDuration(l_Base->GetDuration() / l_Speed);
        else
            l_Base->SetDuration(l_Base->GetDuration() * l_Speed);

        for (uint8 l_Effect = 0; l_Effect < l_Base->GetEffectCount(); ++l_Effect)
        {
            AuraEffect* l_AuraEffect = l_Base->GetEffect(l_Effect);
            if (!l_AuraEffect || !l_AuraEffect->IsPeriodic())
                continue;

            l_AuraEffect->UpdatePeriodic(l_Base->GetCaster());
        }
    }
}

void AuraEffect::HandleBonusLoot(AuraApplication const* p_AurApp, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL) || !p_Apply)
        return;

    Unit* l_Target = p_AurApp->GetTarget();
    if (!l_Target)
        return;

    Player* l_Player = l_Target->ToPlayer();
    if (!l_Player || !l_Target->HasAura(eRecruitAFriendConstants::TrackerSpell))
        return;

    Unit* l_Caster = GetCaster();
    if (!l_Caster || !l_Caster->ToCreature())
        return;

    LootStore& l_LootStore = LootTemplates_Creature;
    LootTemplate const* l_LootTemplate = l_LootStore.GetLootFor(l_Caster->ToCreature()->GetCreatureTemplate()->lootid);
    if (l_LootTemplate == nullptr)
        return;

    std::list<ItemTemplate const*> l_LootTable;
    std::vector<uint32> l_Items;
    l_LootTemplate->FillAutoAssignationLoot(l_LootTable, l_Player, false);

    /// Let's assume it's 25% base drop chance, increased by 10% for each bonus roll failed
    float l_DropChance = sRecruitAFriendMgr->GetFloatConstant(eRecruitAFriendConstants::BonusLootChance);
    uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

    for (ItemTemplate const* l_Template : l_LootTable)
    {
        if (l_Template->HasSpec((SpecIndex)l_SpecID, l_Player->getLevel()))
            l_Items.push_back(l_Template->ItemId);
    }

    uint32 l_EncounterID = 0;
    if (!l_Caster->IsPlayer())
    {
        if (InstanceScript* l_Instance = l_Caster->GetInstanceScript())
            l_EncounterID = l_Instance->GetEncounterIDForBoss(l_Caster->ToCreature());
    }

    l_Player->RemoveAurasByType(AuraType::SPELL_AURA_TRIGGER_BONUS_LOOT);
    l_Player->RemoveAurasByType(AuraType::SPELL_AURA_TRIGGER_BONUS_LOOT_2);

    if (l_Items.empty() || !roll_chance_i(l_DropChance))
        return;

    std::random_shuffle(l_Items.begin(), l_Items.end());

    uint32 l_ItemID = l_Items[0];

    ItemContext l_Context = ItemContext::None;

    switch (l_Player->GetMap()->GetDifficultyID())
    {
        case Difficulty::DifficultyMythic:
            l_Context = ItemContext::DungeonMythic;
            break;
        case Difficulty::DifficultyRaidNormal:
            l_Context = ItemContext::RaidNormal;
            break;
        case Difficulty::DifficultyRaidHeroic:
            l_Context = ItemContext::RaidHeroic;
            break;
        case Difficulty::DifficultyRaidMythic:
            l_Context = ItemContext::RaidMythic;
            break;
        case Difficulty::DifficultyRaidLFR:
            l_Context = ItemContext::RaidLfr;
            break;
        default:
            break;
    }

    auto l_Item = l_Player->AddItem(l_ItemID, 1, { }, false, l_EncounterID, true, false, l_Context);
    l_Player->SendDisplayToast(l_ItemID, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, true, false, l_Item ? l_Item->GetAllItemBonuses() : std::vector<uint32>());
}

bool AuraEffect::IsPeriodicDynamicAmount(uint32 p_SpellID) const
{
    bool l_IsDynamic = false;

    switch (p_SpellID)
    {
        case 703:    ///< Garotte
        case 1943:   ///< Rupture
        case 589:    ///< Shadow Word: Pain
        case 34914:  ///< Vampiric Touch
        case 157736: ///< Immolate
        case 188389: ///< Flame Shock (Level 110)
        case 204213: ///< Purge the Wicked
        case 139:    ///< Renew
        case 14914:  ///< Holy Fire
        case 205065: ///< Void Torrent
        case 191587: ///< Virulent Plague
        case 774:    ///< Rejuvenation
        case 155777: ///< Rejuvenation Germination talent
        case 164812: ///< Moonfire
        case 164815: ///< Sunfire
        case 202347: ///< Stellar Flare
        case 124280: ///< Touch Of Karma
        case 63106:  ///< Siphon Life
        case 8936:   ///< Regrowth
        case 1079:   ///< Rip
        case 210705: ///< Ashamane's Rip
        case 155722: ///< Rake Bleed
        case 192090: ///< Thrash
        case 106830: ///< Thrash (Cat)
        case 772:    ///< Rend
        case 118253: ///< Serpent Sting
        case 2818:   ///< Deadly Poison
        case 185855: ///< Lacerate
        case 207690: ///< Bloodlet
        case 192759: ///< Kingsbane
        case 154953: ///< Internal Bleeding
        case 162487: ///< Steel trap
        case 55095:  ///< Frost Fever
        case 199969: ///< Wandering Plague
        case 204437: ///< Lightning Lasso
        case 206647: ///< Electrocute
        case 205546: ///< Odyn's Fury
        case 115767: ///< Deep Wounds
        case 205273: ///< Wake of Ashes
        case 204081: ///< On the Trail
        case 208684: ///< Dire Beast: Hawk
        case 194858: ///< Dragonsfire Grenade
        case 191413: ///< Bestial Ferocity
        case 194599: ///< Black Arrow
        case 198097: ///< Creeping Venom
        case 195452: ///< Nightblade
        case 155625: ///< Moonfire (Cat)
        case 146739: ///< Corruption
        case 233490: ///< Unstable Affliction spells
        case 233496:
        case 233497:
        case 233498:
        case 233499:
            l_IsDynamic = true;
            break;
        case 33763:  ///< LifeBloom
            l_IsDynamic = !(GetCaster() && GetCaster()->m_SpellHelper.GetBool(eSpellHelpers::TheDarkTitansAdviceProc));
            break;
        default:
            break;
    }

    return l_IsDynamic;
}

void AuraEffect::HandleAuraMountedVisual(AuraApplication const* p_AuraApplication, uint8 p_Mode, bool p_Apply) const
{
    if (!(p_Mode & AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK))
        return;

    Unit* l_Target = p_AuraApplication->GetTarget();

    if (p_Apply)
    {
        uint32 l_CreatureEntry = GetMiscValue();
        uint32 l_DisplayId = 0;
        uint32 l_VehicleId = 0;

        if (MountEntry const* l_MountEntry = sMountStore.LookupEntry(GetId()))
        {
            auto l_Itr = g_MountVisualsByID.find(l_MountEntry->Id);
            if (l_Itr != g_MountVisualsByID.end())
            {
                std::vector<MountXDisplayEntry const*> l_MountDisplays = l_Itr->second;
                if (l_MountEntry->Flags & MountFlags::MOUNT_FLAG_SELF_MOUNT)
                {
                    l_DisplayId = DISPLAYID_HIDDEN_MOUNT;
                }
                else
                {
                    std::vector<MountXDisplayEntry const*> l_UsableDisplays;
                    std::copy_if(l_MountDisplays.begin(), l_MountDisplays.end(), std::back_inserter(l_UsableDisplays), [l_Target](MountXDisplayEntry const* p_MountDisplay) -> bool
                    {
                        if (Player* l_PlayerTarget = l_Target->ToPlayer())
                            return l_PlayerTarget->EvalPlayerCondition(p_MountDisplay->PlayerConditionID).first;

                        return true;
                    });

                    if (!l_UsableDisplays.empty())
                        l_DisplayId = JadeCore::Containers::SelectRandomContainerElement(l_UsableDisplays)->DisplayID;
                }
            }
        }

        if (CreatureTemplate const* l_CreatureInfo = sObjectMgr->GetCreatureTemplate(l_CreatureEntry))
        {
            l_VehicleId = l_CreatureInfo->VehicleId;

            if (!l_DisplayId)
            {
                l_DisplayId = ObjectMgr::ChooseDisplayId(0, l_CreatureInfo);
                sObjectMgr->GetCreatureModelRandomGender(&l_DisplayId);
            }

            //some spell has one aura of mount and one of vehicle
            for (SpellEffectInfo const& l_Effect : GetBase()->GetSpellInfo()->Effects._effects)
                if (l_Effect.Effect == SPELL_EFFECT_SUMMON && l_Effect.MiscValue == GetMiscValue())
                    l_DisplayId = 0;
        }

        l_Target->Mount(l_DisplayId, l_VehicleId, l_CreatureEntry);

        // cast speed aura
        if (p_Mode & AURA_EFFECT_HANDLE_REAL)
            if (MountCapabilityEntry const* l_MountCapability = sMountCapabilityStore.LookupEntry(GetAmount()))
                l_Target->CastSpell(l_Target, l_MountCapability->SpeedModSpell, true);
    }
    else
    {
        l_Target->Dismount();
        //some mounts like Headless Horseman's Mount or broom stick are skill based spell
        // need to remove ALL arura related to mounts, this will stop client crash with broom stick
        // and never endless flying after using Headless Horseman's Mount
        if (p_Mode & AURA_EFFECT_HANDLE_REAL)
        {
            l_Target->RemoveAurasByType(SPELL_AURA_MOUNTED);

            // remove speed aura
            if (MountCapabilityEntry const* l_MountCapability = sMountCapabilityStore.LookupEntry(GetAmount()))
                l_Target->RemoveAurasDueToSpell(l_MountCapability->SpeedModSpell, l_Target->GetGUID());
        }
    }
}
