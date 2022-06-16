//////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_DB2STRUCTURE_H
#define TRINITY_DB2STRUCTURE_H

#include "Common.h"
#include "DBCEnums.h"
#include "Define.h"
#include "Path.h"
#include "Util.h"
#include "SharedDefines.h"

class Player;

/// GCC has alternative #pragma pack(N) syntax and old gcc version does not support pack(push, N), also any gcc version does not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

/// Always order this file.

//////////////////////////////////////////////////////////////////////////
/// Spell DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Spell
    #define MAX_EFFECT_MASK         4294967295
    #define MAX_SHAPESHIFT_SPELLS   8
    #define MAX_ENCHANTMENT_SPELLS  3
    #define MAX_SPELL_REAGENTS      8
    #define MAX_OVERRIDE_SPELL      10
    #define MAX_SPELL_TOTEMS    2

    struct OverrideSpellDataEntry
    {
        uint32              id;                                 ///< 0
        uint32              spellId[MAX_OVERRIDE_SPELL];        ///< 1
        uint32              PlayerActionBarFileDataId;          ///< 2
        uint8               Flags;                              ///< 3
    };

    struct SpecializationSpellEntry
    {
        LocalizedString*    Description;                        ///< 0      m_DescriptionLang
        uint32              SpellID;                            ///< 1      m_SpellID
        uint32              OverridesSpellID;                   ///< 2      m_OverridesSpellID
        uint16              SpecializationEntry;                ///< 3      m_SpecID
        uint8               OrderIndex;                         ///< 4      No Data
        uint32              ID;                                 ///< 5      m_ID
    };

    struct SpellAuraOptionsEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint32              ProcCharges;                        ///< 1      m_procCharges
        uint32              ProcTypeMask;                       ///< 2      m_procTypeMask
        uint32              InternalCooldown;                   ///< 3      m_internalCooldown
        uint16              StackAmount;                        ///< 4      m_cumulativeAura
        uint16              ProcsPerMinuteEntry;                ///< 5      m_procsPerMinuteEntry
        uint8               DifficultyID;                       ///< 6      m_DifficultyID
        uint8               procChance;                         ///< 7      m_procChance
        uint32              SpellID;                            ///< 8      m_SpellID
    };

    struct SpellAuraRestrictionsEntry
    {
        uint32              Id;                                 ///< 0      ID
        uint32              casterAuraSpell;                    ///< 1      casterAuraSpell
        uint32              targetAuraSpell;                    ///< 2      targetAuraSpell
        uint32              excludeCasterAuraSpell;             ///< 3      excludeCasterAuraSpell
        uint32              excludeTargetAuraSpell;             ///< 4      excludeTargetAuraSpell
        uint8               DifficultyID;                       ///< 5      DifficultyID
        uint8               CasterAuraState;                    ///< 6      CasterAuraState
        uint8               TargetAuraState;                    ///< 7      TargetAuraState
        uint8               ExcludeCasterAuraState;             ///< 8      ExcludeCasterAuraState
        uint8               ExcludeTargetAuraState;             ///< 9     ExcludeTargetAuraState
        uint32              SpellID;                            ///< 10      SpellID
    };

    struct SpellCastingRequirementsEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint32              SpellID;                            ///< 1      m_SpellID
        uint16              MinFactionId;                       ///< 2      m_minFactionID not used
        int16               AreaGroupId;                        ///< 3      m_requiredAreaGroupId
        uint16              RequiresSpellFocus;                 ///< 4      m_requiresSpellFocus
        uint8               FacingCasterFlags;                  ///< 5      m_facingCasterFlags
        uint8               MinReputation;                      ///< 6      m_minReputation not used
        uint8               RequiredAuraVision;                 ///< 7      m_requiredAuraVision not used
    };

    struct SpellCastTimesEntry
    {
        uint32              ID;                                 ///< 0      m_ID
        int32               CastTime;                           ///< 1      m_Base
        int32               MinCastTime;                        ///< 1      m_MinCastTime
        int16               CastTimePerLevel;                   ///< 3      m_CastTimePerLevel
    };

    struct SpellCategoriesEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint16              Category;                           ///< 1      m_category
        uint16              StartRecoveryCategory;              ///< 2      m_startRecoveryCategory
        uint16              ChargeCategory;                     ///< 3      m_chargesCategory
        uint8               DifficultyID;                       ///< 4      m_DifficultyID
        uint8               DmgClass;                           ///< 5      m_defenseType
        uint8               Dispel;                             ///< 6      m_dispelType
        uint8               Mechanic;                           ///< 7      m_mechanic
        uint8               PreventionType;                     ///< 8      m_preventionType
        uint32              SpellID;                            ///< 9      m_spellId
    };

    struct SpellCategoryEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        LocalizedString*    Description;                        ///< 1      m_NameLang
        uint32              ChargeRecoveryTime;                 ///< 2      m_ChargeRecoveryTime
        uint8               Flags;                              ///< 3      m_Flags
        uint8               m_UsesPerWeek;                      ///< 4      m_UsesPerWeek
        uint8               MaxCharges;                         ///< 5      m_MaxCharges
        uint32              ChargeCategoryType;                 ///< 6      m_ChargeCategoryType
    };

    struct SpellClassOptionsEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint32              SpellID;                            ///< 1      m_spellId
        flag128             SpellFamilyFlags;                   ///< 2      m_spellFamilyName
        uint8               SpellFamilyName;                    ///< 5      m_spellClassSet
        uint32              modalNextSpell;                     ///< 6      m_modalNextSpell not used
    };

    struct SpellCooldownsEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint32              CategoryRecoveryTime;               ///< 1      m_categoryRecoveryTime
        uint32              RecoveryTime;                       ///< 2      m_recoveryTime
        uint32              StartRecoveryTime;                  ///< 3      m_startRecoveryTime
        uint8               DifficultyID;                       ///< 4      m_DifficultyID
        uint32              SpellID;                            ///< 5      m_SpellID
    };

    struct SpellDurationEntry
    {
        uint32              ID;                                 ///< 0      m_ID
        int32               Duration;                           ///< 1      m_Duration
        int32               MaxDuration;                        ///< 2      m_MaxDuration
        int32               DurationPerLevel;                   ///< 3      m_DurationPerLevel
    };

    struct SpellEffectEntry
    {
        uint32              Id;                                 ///< 1      m_ID
        uint32              Effect;                             ///< 2      m_Effect
        int32               EffectBasePoints;                   ///< 3      m_EffectBasePoints     Don't must be used in spell/auras explicitly, must be used cached Spell::m_currentBasePoints
        uint32              EffectIndex;                        ///< 4      m_EffectIndex
        uint32              EffectApplyAuraName;                ///< 5      m_EffectAura
        uint32              EffectDifficulty;                   ///< 6      m_EffectDifficulty
        float               EffectValueMultiplier;              ///< 7      m_EffectAmplitude
        uint32              EffectAmplitude;                    ///< 8      m_EffectAuraPeriod
        float               EffectBonusMultiplier;              ///< 9      m_EffectBonusCoefficient
        float               EffectDamageMultiplier;             ///< 10     m_EffectChainAmplitude
        uint32              EffectChainTarget;                  ///< 11     m_EffectChainTargets
        int32               EffectDieSides;                     ///< 12     m_EffectDieSides
        uint32              EffectItemType;                     ///< 13     m_EffectItemType
        uint32              EffectMechanic;                     ///< 14     m_EffectMechanic
        float               EffectPointsPerComboPoint;          ///< 15     m_EffectPointsPerResource
        float               EffectRealPointsPerLevel;           ///< 16     m_EffectRealPointsPerLevel
        uint32              EffectTriggerSpell;                 ///< 17     m_effectTriggerSpell
        float               m_EffectPosFacing;                  ///< 18     m_EffectPosFacing
        uint32              m_EffectAttributes;                 ///< 19     m_EffectAttributes
        float               BonusCoefficientFromAP;             ///< 20     m_BonusCoefficientFromAP
        float               PvpMultiplier;                      ///< 21     m_PvpMultiplier
        float               Coefficient;                        ///< 22
        float               Variance;                           ///< 23
        float               ResourceCoefficient;                ///< 24
        float               GroupSizeCoefficient;               ///< 25
        flag128             EffectSpellClassMask;               ///< 26     m_effectSpellClassMask1 (2/3), effect 0
        int32               EffectMiscValue;                    ///< 27-0   m_EffectMiscValue
        int32               EffectMiscValueB;                   ///< 27-1   m_EffectMiscValueB
        uint32              EffectRadiusIndex;                  ///< 28-0   m_EffectRadiusIndex     Spellradius.db2
        uint32              EffectRadiusMaxIndex;               ///< 28-1   m_EffectRadiusMaxIndex
        uint32              EffectImplicitTargetA;              ///< 29-0   m_implicitTargetA
        uint32              EffectImplicitTargetB;              ///< 29-1   m_implicitTargetB
        uint32              EffectSpellId;                      ///< 30     m_SpellID
    };

    struct SpellEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        LocalizedString*    SpellName;                          ///< 1      m_name_lang
        LocalizedString*    Rank;                               ///< 2      m_nameSubtext_lang
        LocalizedString*    Description;                        ///< 3      m_description_lang
        LocalizedString*    ToolTip;                            ///< 4      m_auraDescription_lang

        SpellEffectEntry const* GetSpellEffect(uint32 eff, uint32 difficulty) const;
    };

    struct SpellEquippedItemsEntry
    {
        uint32              Id;                                 ///< 0      ID
        uint32              SpellID;                            ///< 1      SpellID
        int32               EquippedItemInventoryTypeMask;      ///< 4      equippedItemInvTypes (mask)
        int32               EquippedItemSubClassMask;           ///< 5      equippedItemSubclass (mask)
        int8                EquippedItemClass;                  ///< 3      equippedItemClass (value)
    };

    struct SpellFocusObjectEntry
    {
        uint32              ID;                                 ///< 0      m_ID
        LocalizedString*    Name;                               ///< 1      m_NameLang
    };

    struct SpellInterruptsEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint8               DifficultyID;                       ///< 1      m_DifficultyID
        int16               InterruptFlags;                     ///< 2      m_interruptFlags
        int32               AuraInterruptFlags[2];              ///< 3      m_auraInterruptFlags
        int32               ChannelInterruptFlags[2];           ///< 4      m_channelInterruptFlags
        int32               SpellID;                            ///< 5      m_SpellID
    };

    struct SpellItemEnchantmentConditionEntry
    {
        uint32 ID;                                              ///< 0      m_ID
        uint32 LTOperand[5];                                    ///< 1      m_rt_operand[5]
        uint8  LTOperandType[5];                                ///< 2      m_lt_operandType[5
        uint8  Operator[5];                                     ///< 3      m_lt_operand[5]
        uint8  RTOperandType[5];                                ///< 4      m_operator[5]
        uint8  RTOperand[5];                                    ///< 5      m_rt_operandType[5
        uint8  Logic[5];                                        ///< 6      m_logic[5]
    };

    struct SpellItemEnchantmentEntry
    {
        uint32              ID;                                             ///< 0      m_ID
        LocalizedString*    description;                                    ///< 1      Name
        uint32              spellid[MAX_ENCHANTMENT_SPELLS];                ///< 2      EffectSpellID[3]
        float               m_EffectScalingPoints[MAX_ENCHANTMENT_SPELLS];  ///< 3      EffectScalingPoints
        uint32              TransmogCost;                                   ///< 4      TransmogCost
        uint32              TextureFileDataID;                              ///< 5      TextureFileDataID
        uint16              amount[MAX_ENCHANTMENT_SPELLS];                 ///< 6      EffectPointsMin[3]
        uint16              ItemVisual;                                     ///< 7      ItemVisual
        uint16              Flags;                                          ///< 8      Flags
        uint16              requiredSkill;                                  ///< 9      RequiredSkillID
        uint16              requiredSkillValue;                             ///< 10     RequiredSkillRank
        uint16              m_ItemLevel;                                    ///< 11     ItemLevel
        uint8               charges;                                        ///< 12     Charges
        uint8               type[MAX_ENCHANTMENT_SPELLS];                   ///< 13     Effect[3]
        uint8               ConditionID;                                    ///< 14     ConditionID
        uint8               requiredLevel;                                  ///< 15     MinLevel
        uint8               m_MaxLevel;                                     ///< 16     MaxLevel
        int8                m_ScalingClass;                                 ///< 17     ScalingClass
        int8                m_ScalingClassRestricted;                       ///< 18     ScalingClassRestricted
        uint32              PlayerConditionID;                              ///< 19     PlayerConditionID
    };

    struct SpellLevelsEntry
    {
        uint32              Id;                                 ///< 0        m_ID
        uint16              baseLevel;                          ///< 1        m_baseLevel
        uint16              maxLevel;                           ///< 2        m_maxLevel
        uint16              spellLevel;                         ///< 3        m_spellLevel
        uint8               DifficultyID;                       ///< 4        m_DifficultyID
        uint8               MaxUsableLevel;                     ///< 5        MaxUsableLevel
        uint32              SpellID;                            ///< 6        m_spellId
    };

    struct SpellMiscEntry
    {
        uint32              ID;                                 ///< 0      m_ID
        uint16              CastingTimeIndex;                   ///< 1      m_castingTimeIndex
        uint16              DurationIndex;                      ///< 2      m_durationIndex
        uint16              RangeIndex;                         ///< 3      m_rangeIndex
        uint8               SchoolMask;                         ///< 4      m_SchoolMask
        uint32              SpellIconFileDataId;                ///< 5      m_IconFileDataID
        float               speed;                              ///< 6      m_speed
        uint32              ActiveIconFileDataID;               ///< 7      m_ActiveIconFileDataID
        float               LaunchDelay;                        ///< 8      @TODO implement me
        uint8               DifficultyId;                       ///< 9      @TODO implement me
        uint32              Attributes;                         ///< 10-0    m_attribute
        uint32              AttributesEx;                       ///< 10-1    m_attributesEx
        uint32              AttributesEx2;                      ///< 10-2    m_attributesExB
        uint32              AttributesEx3;                      ///< 10-3    m_attributesExC
        uint32              AttributesEx4;                      ///< 10-4    m_attributesExD
        uint32              AttributesEx5;                      ///< 10-5    m_attributesExE
        uint32              AttributesEx6;                      ///< 10-6    m_attributesExF
        uint32              AttributesEx7;                      ///< 10-7    m_attributesExG
        uint32              AttributesEx8;                      ///< 10-8    m_attributesExH
        uint32              AttributesEx9;                      ///< 10-9    m_attributesExI
        uint32              AttributesEx10;                     ///< 10-10   m_attributesExJ
        uint32              AttributesEx11;                     ///< 10-11   m_attributesExK
        uint32              AttributesEx12;                     ///< 10-12   m_attributesExL
        uint32              AttributesEx13;                     ///< 10-13   m_attributesExM
        uint32              SpellID;                            ///< 11
    };

    struct SpellPowerEntry
    {
        int32               Cost;                               ///< 0      ManaCost
        float               CostBasePercentage;                 ///< 1      ManaCostPercentage
        float               CostPerSecondPercentage;            ///< 2      ManaCostPercentagePerSecond
        int32               RequiredAuraSpellId;                ///< 3      RequiredAura
        float               CostMaxPercentage;                  ///< 4      HealthCostPercentage
        uint8               PowerIndex;                         ///< 5
        int8                PowerType;                          ///< 6
        int32               Id;                                 ///< 7
        uint32              CostPerlevel;                       ///< 8      ManaCostPerLevel
        uint32              CostPerSecond;                      ///< 9     ManaCostPerSecond
        int32               CostAdditional;                     ///< 10     Spell uses [Cost, Cost+CostAdditional] power - affects tooltip parsing as multiplier on SpellEffectEntry::EffectPointsPerResource only SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE, SPELL_EFFECT_WEAPON_DAMAGE, SPELL_EFFECT_NORMALIZED_WEAPON_DMG
        int32               PowerDisplayID;                     ///< 11
        uint32              UnitPowerBarID;                     ///< 12
        int32               SpellId;                            ///< 13
    };

    struct SpellProcsPerMinuteEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        float               BaseProcRate;                       ///< 1      m_procsPerMinute
        uint8               Flags;                              ///< 2
    };

    struct SpellProcsPerMinuteModEntry
    {
        uint32              ID;                                 ///< 0
        float               Coeff;                              ///< 1
        uint16              Param;                              ///< 2
        uint8               Type;                               ///< 3
        uint16              SpellProcsPerMinuteID;              ///< 4
    };

    struct SpellRadiusEntry
    {
        uint32              ID;                                 ///< 0      m_ID
        float               radiusHostile;                      ///< 1      m_Radius
        float               m_RadiusPerLevel;                   ///< 2      m_RadiusPerLevel
        float               radiusFriend;                       ///< 3      m_RadiusMin
        float               RadiusMax;                          ///< 4      m_RadiusMax
    };

    struct SpellRangeEntry
    {
        uint32              ID;                                 ///< 0      m_ID
        LocalizedString*    Name;                               ///< 3      m_displayName_lang
        LocalizedString*    ShortName;                          ///< 4      m_displayNameShort_lang
        float               minRangeHostile;                    ///< 1-0    m_RangeMin
        float               minRangeFriend;                     ///< 1-1    m_RangeMin
        float               maxRangeHostile;                    ///< 2-0    m_RangeMax
        float               maxRangeFriend;                     ///< 2-1    m_RangeMax
        uint8               Flags;                              ///< 5      m_Flags
    };

    struct SpellReagentsEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint32              SpellID;                            ///< 1      m_ID
        int32               Reagent[MAX_SPELL_REAGENTS];        ///< 2      m_reagent
        uint16              ReagentCount[MAX_SPELL_REAGENTS];   ///< 3      m_reagentCount
    };

    struct SpellReagentsCurrencyEntry
    {
        uint32              ID;                                 ///< 0
        uint32              SpellID;                            ///< 1
        uint16              CurrencyID;                         ///< 2
        uint16              CurrencyCount;                      ///< 3
    };

    struct SpellScalingEntry
    {
        uint32              Id;                                 ///< 0      ID
        uint32              SpellID;                            ///< 1      spellID
        uint16              ScalesFromItemLevel;                ///< 2      ScalesFromItemLevel
        uint32              ScalingClass;                       ///< 3      Class
        int32               MinScalingLevel;                    ///< 4      MinScalingLevel
        uint32              MaxScalingLevel;                    ///< 5      MaxScalingLevel
    };

    struct SpellShapeshiftEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint32              SpellId;                            ///< 1
        uint32              ShapeshiftExclude[2];               ///< 2      m_shapeshiftExclude
        uint32              ShapeshiftMask[2];                  ///< 3      m_shapeshiftMask
        int8                StanceBarOrder;                     ///< 4      m_stanceBarOrder
    };

    struct SpellShapeshiftFormEntry
    {
        uint32              ID;                                 ///< 0      m_ID
        LocalizedString*    Name;                               ///< 1      m_NameLang
        float               WeaponDamageVariance;               ///< 2      m_WeaponDamageVariance
        uint32              Flags;                              ///< 3      m_Flags
        uint16              CombatRoundTime;                    ///< 4      m_CombatRoundTime
        uint16              MountTypeID;                        ///< 5      m_MountTypeID
        int8                CreatureType;                       ///< 6      m_CreatureType
        uint8               BonusActionBar;                     ///< 7      m_BonusActionBar
        uint32              AttackIconFileDataID;               ///< 8      m_AttackIconID
        uint32              CreatureDisplayID[4];               ///< 9-4    m_CreatureDisplayID (0 - Alliance, 1 - Horde)
        uint32              PresetSpellID[MAX_SHAPESHIFT_SPELLS]; ///< 13-8   m_PresetSpellID
    };

    struct SpellTargetRestrictionsEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        float               ConeAngle;                          ///< 1      m_ConeAngle
        float               Width;                              ///< 2      m_Width
        uint32              Targets;                            ///< 3      m_targets
        uint16              TargetCreatureType;                 ///< 4      m_targetCreatureType
        uint8               DifficultyID;                       ///< 5      m_DifficultyID
        uint8               MaxAffectedTargets;                 ///< 6      m_maxTargets
        uint32              MaxTargetLevel;                     ///< 7      m_maxTargetLevel
        uint32              SpellId;                            ///< 8      m_spellId
    };

    struct SpellTotemsEntry
    {
        uint32              Id;                                  ///< 0     m_ID
        uint32              SpellID;                             ///< 2
        uint32              Totem[MAX_SPELL_TOTEMS];             ///< 3-4   m_totem
        uint16              TotemCategory[MAX_SPELL_TOTEMS];     ///< 5-6   m_requiredTotemCategoryID
    };

    struct SpellVisualEntry
    {
        uint32 m_precastKit;                        ///< 0
        uint32 m_castKit;                           ///< 1
        uint32 m_impactKit;                         ///< 2
        uint32 m_stateKit;                          ///< 3
        uint32 m_stateDoneKit;                      ///< 4
        uint32 m_channelKit;                        ///< 5
        uint32 m_hasMissile;                        ///< 6
        uint32 m_missileDestinationAttachment;      ///< 7
        uint32 m_missileSound;                      ///< 8
        uint32 m_animEventSoundID;                  ///< 9
        uint32 m_casterImpactKit;                   ///< 10
        uint32 m_targetImpactKit;                   ///< 11
        uint32 m_missileAttachment;                 ///< 12
        uint32 m_missileFollowGroundHeight;         ///< 13
        uint32 m_missileFollowGroundDropSpeed;      ///< 14
        float m_missileCastOffsetX;                 ///< 15
        float m_missileCastOffsetY;                 ///< 16
        float m_missileCastOffsetZ;                 ///< 17
        float m_missileImpactOffsetX;               ///< 18
        float m_missileImpactOffsetY;               ///< 19
        float m_missileImpactOffsetZ;               ///< 20
        uint32 field27;                             ///< 21
        uint32 field28;                             ///< 22
        uint16 m_persistentAreaKit;                 ///< 23
        uint16 field26;                             ///< 24
        uint16 field29;                             ///< 25
        uint8 m_flags;                              ///< 26
        uint8 m_missileModel;                       ///< 27
        uint32 ID;                                  ///< 28
        uint32 m_missilePathType;                   ///< 29
        uint32 unk30;                               ///< 30
        uint32 unk31;                               ///< 31 Type uint32
    };

    struct SpellXSpellVisualEntry
    {
        uint32              VisualID;                           ///< 0
        uint32              Id;                                 ///< 1
        float               Chance;                             ///< 2
        uint16              CasterPlayerConditionID;            ///< 3
        uint16              CasterUnitConditionID;              ///< 4
        uint16              PlayerConditionID;                  ///< 5
        uint16              UnitConditionID;                    ///< 6
        uint32              IconFileDataID;                     ///< 7
        uint32              ActiveIconFileDataID;               ///< 8
        uint8               Flags;                              ///< 9
        uint8               DifficultyID;                       ///< 10
        uint8               Priority;                           ///< 11
        uint32              SpellId;                            ///< 12
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Garrison DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Garrison
    struct GarrSiteLevelEntry
    {
        uint32              SiteLevelID;                        ///< 0
        float               TownHallX;                          ///< 1-0
        float               TownHallY;                          ///< 1-1
        uint16              MapID;                              ///< 2
        uint16              UITextureKitID;                     ///< 3
        uint16              MovieID;                            ///< 4
        uint16              UpgradeRessourceCost;               ///< 5
        uint16              UpgradeMoneyCost;                   ///< 6
        uint8               Level;                              ///< 7
        uint8               SiteID;                             ///< 8
        uint8               Level2;                             ///< 9
    };

    struct GarrSiteLevelPlotInstEntry
    {
        uint32              ID;                                 ///< 0
        float               MinimapX;                           ///< 1-0
        float               MinimapY;                           ///< 1-1
        uint16              GarrSiteLevelID;                    ///< 2
        uint8               PlotInstanceID;                     ///< 3
        uint8               UiMarkerSize;                       ///< 4
    };

    struct GarrPlotInstanceEntry
    {
        uint32              InstanceID;                         ///< 0
        LocalizedString const* Name;                            ///< 1
        uint8               PlotID;                             ///< 2
    };

    struct GarrPlotEntry
    {
        uint32              PlotID;                             ///< 0
        LocalizedString const* Name;                            ///< 1
        uint32              AllianceConstructionGameObjectID;   ///< 2
        uint32              HordeConstructionGameObjectID;      ///< 3
        uint8               GarrPlotUICategoryID;               ///< 4
        uint8               PlotType;                           ///< 5
        uint8               Flags;                              ///< 6
        uint32              MinCount;                           ///< 7-0
        uint32              MaxCount;                           ///< 7-1
    };

    struct GarrPlotUICategoryEntry
    {
        uint32              PlotUICategoryID;                   ///< 0
        LocalizedString const* Name;                            ///< 1
        uint8               Type;                               ///< 2
    };

    struct GarrMissionEntry
    {
        LocalizedString const* Name;                            ///< 0
        LocalizedString const* Description;                     ///< 1
        LocalizedString const* Location;                        ///< 2
        uint32              Duration;                           ///< 3
        uint32              OfferTime;                          ///< 4
        float               MapX;                               ///< 5 X for the ship mission map
        float               MapY;                               ///< 6 Y for the ship mission map
        float               WorldX;                             ///< 7-0 X
        float               WorldY;                             ///< 8-1 Y
        int16               RequiredItemLevel;                  ///< 9
        uint16              LocPrefixID;                        ///< 10
        uint16              CurrencyID;                         ///< 11
        uint8               RequiredLevel;                      ///< 12
        uint8               GarrMechanicTypeRecID;              ///< 13
        uint8               RequiredFollowersCount;             ///< 14
        uint8               Category;                           ///< 15
        uint8               MissionType;                        ///< 16
        uint8               FollowerType;                       ///< 17
        uint8               BaseBonusChance;                    ///< 18
        uint8               LostChance;                         ///< 19
        uint8               GarrisonType;                       ///< 20
        uint32              MissionRecID;                       ///< 21 ID
        uint32              TravelTime;                         ///< 22
        uint32              SubCategory2;                       ///< 23 -- MIGHT BE WRONG
        uint32              CurrencyCost;                       ///< 24
        uint32              Flags;                              ///< 25
        uint32              RewardFollowerExperience;           ///< 26
        uint32              ZoneID;                             ///< 27
        uint32              SubCategory1;                       ///< 28 -- MIGHT BE WRONG
        uint32              OvermaxRewardPackId;                ///< 29
        uint32              EnvGarrMechanicId;                  ///< 30
    };

    struct GarrMissionXEncouterEntry
    {
        uint8               OrderIndex;                         ///< 0
        uint32              ID;                                 ///< 1
        uint32              EncounterID;                        ///< 2
        uint32              GarrEncounterSetID;                 ///< 3
        uint32              MissionID;                          ///< 4
    };

    struct GarrBuildingEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* NameAlliance;                    ///< 1
        LocalizedString const* NameHorde;                       ///< 2
        LocalizedString const* Description;                     ///< 3
        LocalizedString const* Tooltip;                         ///< 4
        uint32              GameObjects[2];                     ///< 5 - 6
        uint32              IconFileDataID;                     ///< 7
        uint16              CostCurrencyID;                     ///< 8
        uint16              AllianceTexPrefixKitID;             ///< 9
        uint16              HordeTexPrefixKitID;                ///< 10
        uint16              AllianceActivationScenePackageID;   ///< 11
        uint16              HordeActivationScenePackageID;      ///< 12
        uint16              FollowerRequiredGarrAbilityID;      ///< 13     GarrAbilityID http://www.wowhead.com/building=27/barracks#rel-garrisonabilities
        uint16              FollowerGarrAbilityEffectID;        ///< 14
        uint16              CostMoney;                          ///< 15
        uint8               GarrSiteId;                         ///< 16
        uint8               Type;                               ///< 17
        uint8               Level;                              ///< 18
        uint8               Flags;                              ///< 19
        uint8               MaxShipments;                       ///< 20
        uint8               GarrisonType;                       ///< 21
        uint32              BuildDuration;                      ///< 22
        uint32              CostCurrencyAmount;                 ///< 23
        uint32              BonusAmount;                        ///< 24     slot number ex : Menagerie Attracts elite pets you can battle for rewards. Also lets 5 of your battle pets hang out in your garrison.
    };

    struct GarrPlotBuildingEntry
    {
        uint32              ID;                                 ///< 0
        uint8               PlotID;                             ///< 1
        uint8               BuildingID;                         ///< 2
    };

    struct GarrFollowerEntry
    {
        LocalizedString const* HordeSourceText;                 ///< 0     Descr1 : zoneH (where you find this follower)
        LocalizedString const* AllinaceSourceText;              ///< 1     Descr2 : zoneA (where you find this follower)
        LocalizedString const* Name;                            ///< 2
        uint32              CreatureID[2];                      ///< 3-1
        int32               HordePortraitIconID;                ///< 4
        int32               AlliancePortraitIconID;             ///< 5
        int32               HordeAddedBroadcastTextID;          ///< 6
        int32               AllianceAddedBroadcastTextID;       ///< 7
        uint16              HordeGarrFollItemSetID;             ///< 9
        uint16              AllianceGarrFollItemSetID;          ///< 10
        int16               ItemLevelWeapon;                    ///< 11
        int16               ItemLevelArmor;                     ///< 12
        uint16              HordeListPortraitTextureKitID;      ///< 13
        uint16              AllianceListPortraitTextureKitID;   ///< 14
        uint8               Type;                               ///< 15
        uint8               HordeUiAnimRaceInfoID;              ///< 16
        uint8               AllianceUiAnimRaceInfoID;           ///< 17
        uint8               Quality;                            ///< 18
        uint8               HordeGarrClassSecID;                ///< 19     GarrClassSpec.db2
        uint8               AllianceGarrClassSecID;             ///< 20
        uint8               Level;                              ///< 21
        int8                Gender;                             ///< 22
        uint8               Flags;                              ///< 23
        int8                HordeSourceTypeEnum;                ///< 24
        int8                AllianceSourceTypeEnum;             ///< 25
        int8                GarrisonType;                       ///< 26
        int8                MaxDurability;                      ///< 27
        int8                Class;                              ///< 28
        int8                HordeFlavorTextGarrStringID;        ///< 29
        int8                AllianceFlavorTextGarrStringID;     ///< 30
        uint32              ID;                                 ///< 31
    };

    struct GarrClassSpecEntry
    {
        LocalizedString const* NameMale;                        ///< 0
        LocalizedString const* NameFemale;                      ///< 1
        LocalizedString const* NameGenderless;                  ///< 2
        uint16              ClassAtlasID;                       ///< 3
        uint16              SpecializationID;                   ///< 4
        uint8               MaxTroopAmount;                     ///< 5
        uint8               Flags;                              ///< 6
        uint32              ID;                                 ///< 7
    };

    struct GarrFollowerTypeEntry
    {
        uint32              ID;                                 ///< 0
        uint16              MaxItemLevel;                       ///< 1
        uint8               SoftCap;                            ///< 2
        uint8               SoftCapBuildingIncreaseID;          ///< 3
        int8                GarrisonType;                       ///< 4
        int8                LevelBiasDividor;                   ///< 5
        int8                ItemLevelBiasDividor;               ///< 6
        uint8               Flags;                              ///< 7
    };

    struct GarrFollSupportSpellEntry
    {
        uint32              ID;                                 ///< 0
        uint32              Spells[2];                          ///< 1 - 2
        uint8               OrderIndex;                         ///< 3
        uint32              FollowerID;                         ///< 4
    };

    struct GarrAbilityEntry
    {
        LocalizedString const* Name;                            ///< 0
        LocalizedString const* Description;                     ///< 1
        uint32              IconID;                             ///< 2
        uint16              Flags;                              ///< 3
        uint16              OtherfactionGarrAbilityID;          ///< 4
        uint8               Category;                           ///< 5
        uint8               FollowerType;                       ///< 6
        uint32              ID;                                 ///< 7
    };

    struct GarrAbilityEffectEntry
    {
        float               CombatWeightBase;                   ///< 0
        float               CombatWeightMax;                    ///< 1
        float               ActionValueFlat;                    ///< 2
        uint32              GarrClassSpecID;                    ///< 3
        uint16              AbilityID;                          ///< 4
        uint8               EffectType;                         ///< 5
        uint8               TargetMask;                         ///< 6
        uint8               CounterMechanicTypeID;              ///< 7
        uint8               Flags;                              ///< 8
        uint8               MiscValueA;                         ///< 9
        uint8               MiscValueB;                         ///< 10
        uint32              ID;                                 ///< 11
    };

    struct GarrFollowerXAbilityEntry
    {
        uint32              ID;                                 ///< 0
        uint16              AbilityID;                          ///< 2
        uint8               FactionIndex;                       ///< 3
        uint16              FollowerID;                         ///< 4
    };

    struct GarrBuildingPlotInstEntry
    {
        float               MinimapX;                           ///< 0-0
        float               MinimapY;                           ///< 0-1
        uint16              UIAtlasTextureMemberID;             ///< 1
        uint16              SiteLevelPlotInstID;                ///< 2
        uint8               BuildingID;                         ///< 3
        uint32              ID;                                 ///< 4
    };

    struct GarrMechanicEntry
    {
        uint32              ID;                                 ///< 0
        float               Factor;                             ///< 1
        uint8               MechanicTypeID;                     ///< 2
        uint32              AbilityID;                          ///< 3
    };

    struct GarrMechanicTypeEntry
    {
        LocalizedString const* Environment;                     ///< 0
        LocalizedString const* EnvironmentDesc;                 ///< 1
        uint32              EnvironmentTextureID;               ///< 2
        uint8               Type;                               ///< 3
        uint32              ID;                                 ///< 4
    };

    struct GarrMechanicSetXMechanicEntry
    {
        uint8               GarrMechanicID;                     ///< 0
        uint32              Index;                              ///< 1
        uint32              GarrMechanicSetID;                  ///< 2
    };

    struct GarrEncouterXMechanicEntry
    {
        uint32              ID;                                 ///< 0
        uint8               MechanicID;                         ///< 1
        uint8               GarrMechanicSetID;                  ///< 2
        uint16              EncounterID;                        ///< 3
    };

    struct GarrEncounterSetXEncounterEntry
    {
        uint32              Index;                              ///< 0
        uint32              GarrEncounterID;                    ///< 1
        uint32              GarrEncounterSetID;                 ///< 2
    };

    struct GarrFollowerLevelXPEntry
    {
        uint32              ID;                                 ///< 0
        uint16              RequiredExperience;                 ///< 1
        uint16              ShipmentXP;                         ///< 2
        uint8               Level;                              ///< 3
        uint8               FollowerType;                       ///< 4
    };

    struct GarrSpecializationEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* Name;                            ///< 1
        LocalizedString const* Description;                     ///< 2
        uint32              IconID;                             ///< 3      IconID
        float               BasePoint[2];                       ///< 4
        uint8               BuildingType;                       ///< 5
        uint8               SpecType;                           ///< 6
        uint8               RequiredUpgradeLevel;               ///< 7
    };

    struct CharShipmentEntry
    {
        uint32              ID;                                 ///< 0
        uint32              TreasureId;                         ///< 1
        uint32              Duration;                           ///< 2
        uint32              SpellID;                            ///< 3
        uint32              ResultItemID;                       ///< 4
        uint32              OnCompleteSpellId;                  ///< 5
        uint16              ShipmentContainerID;                ///< 6
        uint16              FollowerID;                         ///< 7
        uint8               Flags;                              ///< 8
        uint8               MaxShipments;                       ///< 9
    };

    struct GarrTalentEntry
    {
        LocalizedString const* Name;                            ///< 0
        LocalizedString const* Description;                     ///< 1
        uint32              IconID;                             ///< 2
        uint32              ResearchTime;                       ///< 3
        uint8               Rank;                               ///< 4
        uint8               Column;                             ///< 5
        uint8               Flags;                              ///< 6
        uint32              ID;                                 ///< 7
        uint32              TalentTreeID;                       ///< 8
        uint32              GarrAbilityID;                      ///< 9
        uint32              ResearchPlayerConditionID;          ///< 10
        uint32              ResearchCurrencyCost;               ///< 11
        uint32              ResearchCurrencyID;                 ///< 12
        uint32              ResearchGoldCost;                   ///< 13
        uint32              SpellID;                            ///< 14
        uint32              ReplacePlayerConditionID;           ///< 15
        uint32              ReplaceCurrencyCost;                ///< 16
        uint32              ReplaceCurrencyID;                  ///< 17
        uint32              RespecDurationSecs;                 ///< 19
        uint32              ReplaceGoldCost;                    ///< 18
    };

    struct GarrTalentTreeEntry
    {
        uint32              ID;                                 ///< 0
        uint16              UiTextureKitId;                     ///< 1
        uint8               NumRank;                            ///< 2
        uint8               UiOrder;                            ///< 3
        uint32              ClassID;                            ///< 4
        uint32              GarrisonType;                       ///< 5
    };

    struct GarrTypeEntry
    {
        uint32              ID;                                 ///< 0
        uint32              Flags;                              ///< 1
        uint32              CurrencyType;                       ///< 2
        uint32              SecondCurrencyType;                 ///< 3
        uint32              ExpansionRequired;                  ///< 4
        uint32              BaseMap[2];                         ///< 5-6
    };

    struct CharShipmentContainerEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* Name;                            ///< 1
        LocalizedString const* Description;                     ///< 2
        uint32              WorkingSpellVisualId;               ///< 3
        uint16              TextureKitID;                       ///< 4
        uint16              OverrideDisplayIfNotNull;           ///< 5 - shipyard override displayID
        uint16              OverrideDisplayID1;                 ///< 6
        uint16              OverrideIfAmountMet[2];             ///< 7-8
        int16               CrossFactionId;                     ///< 9
        uint8               WorkorderLimit;                     ///< 10
        uint8               BuildingType;                       ///< 11
        uint8               GarrisonType;                       ///< 12
        uint8               ShipmentAmountNeeded[2];            ///< 13-14
        uint8               Faction;                            ///< 15
        uint32              CompleteSpellVisualId;              ///< 16
    };

    struct GarrItemLevelUpgradeDataEntry
    {
        int32               ID;                                 ///< 0
        int32               Operation;                          ///< 1
        int32               MinItemLevel;                       ///< 2
        int32               MaxItemLevel;                       ///< 3
        int32               FollowerTypeID;                         ///< 4
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// BattlePet DB2
//////////////////////////////////////////////////////////////////////////
#pragma region BattlePet
    #define MAX_BATTLEPET_PROPERTIES 6

    struct BattlePetAbilityEntry
    {
        uint32              id;                                 ///< 0
        LocalizedString const* name;                            ///< 0
        LocalizedString const* description;                     ///< 1
        uint32              family;                             ///< 2
        uint16              visualId;                           ///< 3      BattlePetVisual.db2
        int8                petType;                            ///< 4
        uint8               flags;                              ///< 5
        uint32              cooldown;                           ///< 6
    };

    struct BattlePetAbilityEffectEntry
    {
        uint16              abilityTurnId;                      ///< 0      BattlePetAbilityTurn.db2
        uint16              visualId;                           ///< 1      BattlePetVisual.db2
        uint16              triggerAbility;                     ///< 2
        uint16              effect;                             ///< 3
        int16               prop[MAX_BATTLEPET_PROPERTIES];     ///< 4   See BattlePetEffectProperties.db2 with effect
        uint8               effectIndex;                        ///< 5
        uint32              id;                                 ///< 6
    };

    struct BattlePetAbilityTurnEntry
    {
        uint16              abilityId;                          ///< 0      BattlePetAbility.db2
        uint16              visualId;                           ///< 1      BattlePetVisual.db2
        uint8               turn;                               ///< 2
        uint8               hasProcType;                        ///< 3      if 1 then value <> -1
        int8                procType;                           ///< 4
        uint32              id;                                 ///< 5
    };

    struct BattlePetAbilityStateEntry
    {
        uint32              id;                                 ///< 0
        int32               value;                              ///< 1
        uint8               stateId;                            ///< 2      BattlePetState.db2
        uint16              abilityId;                          ///< 3      BattlePetAbility.db2
    };

    struct BattlePetStateEntry
    {
        uint32              id;                                 ///< 0
        LocalizedString const* name;                            ///< 1
        uint16              parent;                             ///< 2      BattlePetState.db2
        uint16              flags;                              ///< 3
    };

    struct BattlePetEffectPropertiesEntry
    {
        uint32              effect;                                 ///< 0
        LocalizedString const* propName[MAX_BATTLEPET_PROPERTIES];  ///< 1
        uint16              flags;                                  ///< 2
        uint8               propIsId[MAX_BATTLEPET_PROPERTIES];     ///< 3   Only set to 1 for AuraID
    };

    struct BattlePetBreedQualityEntry
    {
        uint32              id;                                 ///< 0
        float               factor;                             ///< 2
        uint8               quality;                            ///< 1
    };

    struct BattlePetBreedStateEntry
    {
        uint32              id;                                 ///< 0
        int16               value;                              ///< 1
        uint8               stateId;                            ///< 2
        uint8               breed;                              ///< 3
    };

    struct BattlePetSpeciesEntry
    {
        LocalizedString const* SourceText;                      ///< 3
        LocalizedString const* Description;                     ///< 4
        uint32              CreatureID;                         ///< 0
        uint32              IconFileID;                         ///< 1
        uint32              SummonSpellID;                      ///< 2
        uint16              Flags;                              ///< 5
        uint8               Source;                             ///< 6
        int8                PetType;                            ///< 7
        uint32              ID;                                 ///< 8
        uint32              CardModelSceneID;                   ///< 9
        uint32              LoadoutModelSceneID;                ///< 10
    };

    struct BattlePetSpeciesStateEntry
    {
        uint32              id;                                 ///< 0
        int32               value;                              ///< 1
        uint8               stateId;                            ///< 2
        uint16              speciesId;                          ///< 3
    };

    struct BattlePetSpeciesXAbilityEntry
    {
        uint32              id;                                 ///< 0
        uint16              abilityId;                          ///< 1
        uint8               level;                              ///< 2
        uint8               tier;                               ///< 3
        uint16              speciesId;                          ///< 4
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Achievement DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Achievement
    struct AchievementEntry
    {
        LocalizedString const* TitleLang;                       ///< 0
        LocalizedString const* DescriptionLang;                 ///< 1
        LocalizedString const* RewardLang;                      ///< 2
        uint32              Flags;                              ///< 3
        int16               MapID;                              ///< 4 -1=none
        uint16              Supercedes;                         ///< 5 its Achievement parent (can`t start while parent uncomplete, use its Criteria if don`t have own, use its progress on begin)
        uint16              Category;                           ///< 6
        uint16              UiOrder;                            ///< 7
        uint16              SharesCriteria;                     ///< 8 - referenced achievement (counting of all completed criterias)
        int8                Faction;                            ///< 9  -1=all, 0=horde, 1=alliance
        uint8               Points;                             ///< 10 reward points
        uint8               MinimumCriteria;                    ///< 11 - need this count of completed criterias (own or referenced achievement criterias)
        uint32              ID;                                 ///< 12
        uint32              IconFileDataID;                     ///< 13 icon (from SpellIcon.db2)
        uint32              CriteriaTree;                       ///< 14
    };

    struct AchievementCategoryEntry
    {
        LocalizedString const* Name;
        uint16                 ParentID;
        uint8                  UIOrder;
        uint32                 ID;
    };

    struct CriteriaTreeEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* DescriptionLang;                 ///< 2
        uint32              Amount;                             ///< 1
        uint16              Flags;                              ///< 3
        uint8               Operator;                           ///< 4
        uint32              CriteriaID;                         ///< 5
        uint32              Parent;                             ///< 6
        uint32              OrderIndex;                         ///< 7
    };

    struct CriteriaEntry
    {
        uint32              ID;                                 ///< 0
        union
        {
            uint32 ID;
            // CRITERIA_TYPE_KILL_CREATURE          = 0
            // CRITERIA_TYPE_KILLED_BY_CREATURE     = 20
            uint32 CreatureID;

            // CRITERIA_TYPE_WIN_BG                 = 1
            // CRITERIA_TYPE_COMPLETE_BATTLEGROUND  = 15
            // CRITERIA_TYPE_DEATH_AT_MAP           = 16
            // CRITERIA_TYPE_WIN_ARENA              = 32
            // CRITERIA_TYPE_PLAY_ARENA             = 33
            uint32 MapID;

            // CRITERIA_TYPE_REACH_SKILL_LEVEL      = 7
            // CRITERIA_TYPE_LEARN_SKILL_LEVEL      = 40
            // CRITERIA_TYPE_LEARN_SKILLLINE_SPELLS = 75
            // CRITERIA_TYPE_LEARN_SKILL_LINE       = 112
            uint32 SkillID;

            // CRITERIA_TYPE_COMPLETE_ACHIEVEMENT   = 8
            uint32 AchievementID;

            // CRITERIA_TYPE_COMPLETE_QUESTS_IN_ZONE = 11
            uint32 ZoneID;

            // CRITERIA_TYPE_CURRENCY = 12
            uint32 CurrencyID;

            // CRITERIA_TYPE_DEATH_IN_DUNGEON       = 18
            // CRITERIA_TYPE_COMPLETE_RAID          = 19
            uint32 GroupSize;

            // CRITERIA_TYPE_DEATHS_FROM            = 26
            uint32 DamageType;

            // CRITERIA_TYPE_COMPLETE_QUEST         = 27
            uint32 QuestID;

            // CRITERIA_TYPE_BE_SPELL_TARGET        = 28
            // CRITERIA_TYPE_BE_SPELL_TARGET2       = 69
            // CRITERIA_TYPE_CAST_SPELL             = 29
            // CRITERIA_TYPE_CAST_SPELL2            = 110
            // CRITERIA_TYPE_LEARN_SPELL            = 34
            uint32 SpellID;

            // CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE
            uint32 ObjectiveId;

            // CRITERIA_TYPE_HONORABLE_KILL_AT_AREA = 31
            // CRITERIA_TYPE_ENTER_AREA             = 163
            // CRITERIA_TYPE_LEAVE_AREA             = 164
            uint32 AreaID;

            // CRITERIA_TYPE_OWN_ITEM               = 36
            // CRITERIA_TYPE_USE_ITEM               = 41
            // CRITERIA_TYPE_LOOT_ITEM              = 42
            // CRITERIA_TYPE_EQUIP_ITEM             = 57
            // CRITERIA_TYPE_OWN_TOY                = 185
            uint32 ItemID;

            // CRITERIA_TYPE_HIGHEST_TEAM_RATING    = 38
            // CRITERIA_TYPE_REACH_TEAM_RATING      = 39
            // CRITERIA_TYPE_HIGHEST_PERSONAL_RATING = 39
            uint32 TeamType;

            // CRITERIA_TYPE_EXPLORE_AREA           = 43
            uint32 WorldMapOverlayID;

            // CRITERIA_TYPE_GAIN_REPUTATION        = 46
            // CRITERIA_TYPE_GAIN_PARAGON_REPUTATION = 206
            uint32 FactionID;

            // CRITERIA_TYPE_EQUIP_EPIC_ITEM        = 49
            uint32 ItemSlot;

            // CRITERIA_TYPE_ROLL_NEED_ON_LOOT      = 50
            // CRITERIA_TYPE_ROLL_GREED_ON_LOOT      = 51
            uint32 RollValue;

            // CRITERIA_TYPE_HK_CLASS               = 52
            uint32 ClassID;

            // CRITERIA_TYPE_HK_RACE                = 53
            uint32 RaceID;

            // CRITERIA_TYPE_DO_EMOTE               = 54
            uint32 EmoteID;

            // CRITERIA_TYPE_USE_GAMEOBJECT         = 68
            // CRITERIA_TYPE_FISH_IN_GAMEOBJECT     = 72
            uint32 GameObjectID;

            // CRITERIA_TYPE_HIGHEST_POWER          = 96
            uint32 PowerType;

            // CRITERIA_TYPE_OWN_PET                = 96
            uint32 PetEntry;

            // CRITERIA_TYPE_HIGHEST_STAT           = 97
            uint32 StatType;

            // CRITERIA_TYPE_HIGHEST_SPELLPOWER     = 98
            uint32 SpellSchool;

            // CRITERIA_TYPE_LOOT_TYPE              = 109
            uint32 LootType;

            // CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE = 138
            uint32 GuildChallengeType;

            // CRITERIA_TYPE_LEVEL_BATTLE_PET       = 160
            uint32 PetLevel;

            // CRITERIA_TYPE_COMPLETE_DUNGEON_ENCOUNTER = 165
            uint32 DungeonEncounterID;

            // CRITERIA_TYPE_CONSTRUCT_GARRISON_BUILDING = 169
            uint32 GarrBuildingID;

            // CRITERIA_TYPE_UPGRADE_GARRISON       = 170
            uint32 GarrisonLevel;

            // CRITERIA_TYPE_COMPLETE_GARRISON_MISSION = 174
            uint32 GarrMissionID;

            // CRITERIA_TYPE_LEARN_GARRISON_BLUEPRINT_COUNT = 178
            uint32 GarrBlueprintCount;

            // CRITERIA_TYPE_COMPLETE_GARRISON_SHIPMENT = 182
            uint32 CharShipmentContainerID;

            // CRITERIA_TYPE_APPEARANCE_UNLOCKED_BY_SLOT = 199
            uint32 AppearanceSlot;

            // CRITERIA_TYPE_TRANSMOG_SET_UNLOCKED = 205
            uint32 TransmogSetGroupID;

            // CRITERIA_TYPE_RELIC_TALENT_UNLOCKED = 211
            uint32 ArtifactPowerID;
        } Asset;
        uint32              StartAsset;
        uint32              FailAsset;
        uint32              ModifierTreeId;
        uint16              StartTimer;
        uint16              EligibilityWorldStateID;
        uint8               Type;
        uint8               StartEvent;
        uint8               FailEvent;
        uint8               Flags;
        uint8               EligibilityWorldStateValue;
    };

    struct ModifierTreeEntry
    {
        uint32              ID;                                 ///< 0
        uint32              Asset;                              ///< 1
        uint32              SecondaryAsset;                     ///< 2
        uint32              Parent;                             ///< 3
        uint8               Type;                               ///< 4
        uint8               TertiaryAsset;                      ///< 5
        uint8               Operator;                           ///< 6
        uint8               Amount;                             ///< 7
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Taxi DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Taxi
    struct TaxiPathEntry
    {
        uint16              From;                               ///< 0
        uint16              To;                                 ///< 1
        uint32              ID;                                 ///< 2
        uint32              Cost;                               ///< 3
    };

    struct TaxiNodesEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* Name;                            ///< 3
        float               x;                                  ///< 1-0
        float               y;                                  ///< 1-1
        float               z;                                  ///< 1-2
        uint32              MountCreatureID[2];                 ///< 2
        float               m_MapOffsetX;                       ///< 3-0
        float               m_MapOffsetY;                       ///< 3-1
        float               Facing;                             ///< 4
        float               m_FlightMapOffsetX;                 ///< 5-0
        float               m_FlightMapOffsetY;                 ///< 6-1
        uint16              MapID;                              ///< 7
        uint16              ConditionID;                        ///< 8
        uint16              LearnableIndex;                     ///< 9 - some kind of index only for learnable nodes
        uint8               Flags;                              ///< 10
        int32               UiTextureKitPrefixID;               ///< 11
        uint32              SpecialAtlasIconPlayerConditionID;  ///< 12
    };

    struct TaxiPathNodeEntry
    {
        float               x;                                  ///< 0-0      m_LocX
        float               y;                                  ///< 0-1      m_LocY
        float               z;                                  ///< 0-2      m_LocZ
        uint16              PathID;                             ///< 1
        uint16              MapID;                              ///< 2
        uint8               NodeIndex;                          ///< 3
        uint32              ID;                                 ///< 4
        uint8               Flags;                              ///< 5
        uint32              Delay;                              ///< 1
        uint16              ArrivalEventID;                     ///< 4
        uint16              DepartureEventID;                   ///< 5
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Artifact DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Artifact
    struct ArtifactEntry
    {
        uint32              m_ID;                               /// 0
        LocalizedString const* m_Name;                          /// 1
        uint32              BarConnectRBG;                      /// 2 - for UI only
        uint32              BarDisconnectRBG;                   /// 3 - for UI only
        uint32              TitleRBG;                           /// 4 - for UI onyl
        uint16              UITextureKitID;                     /// 5
        uint16              SpecializationID;                   /// 6
        uint8               ArtifactCategoryID;                 /// 7
        uint8               Flags;                              /// 8
        uint32              UiModelSceneID;                     /// 9
        uint32              SpellVisualKitID;                   /// 10
    };

    struct ArtifactAppearanceEntry
    {
        LocalizedString const* m_Name;                          /// 0
        int32               UiSwatchColor;                      /// 1
        float               UiModelSaturation;                  /// 2
        float               UiModelOpacity;                     /// 3
        uint32              OverrideShapeshiftDisplayId;        /// 4
        uint16              ArtifactAppearanceSetId;            /// 5
        uint16              UiCameraId;                         /// 6
        uint8               DisplayIndex;                       /// 7
        uint8               ItemAppearanceModifierId;           /// 8
        uint8               Flags;                              /// 9
        uint8               OverrideShapeshiftFormId;           /// 10
        uint32              ID;                                 /// 11
        uint32              UnlockPlayerConditionId;            /// 12
        uint32              UiItemAppearanceId;                 /// 13
        uint32              UiAltItemAppearanceId;              /// 14
    };

    struct ArtifactAppearanceSetEntry
    {
        LocalizedString const* Name;                        /// 0
        LocalizedString const* Description;                 /// 1
        uint16                 UiCameraID;                  /// 2
        uint16                 AltHandUICameraID;           /// 3
        uint8                  DisplayIndex;                /// 4
        uint8                  ForgeAttachmentOverride;     /// 5
        uint8                  Flags;                       /// 6
        uint32                 ID;                          /// 7
        uint32                 ArtifactID;                  /// 8
    };

    struct ArtifactPowerEntry
    {
        float               X;                                  /// 0-0 - for UI only
        float               Y;                                  /// 0-1 - for UI only
        uint8               ArtifactID;                         /// 1
        uint8               Flags;                              /// 2
        uint8               MaxPurchasableRank;                 /// 3
        uint8               ArtifactTier;                       /// 4
        uint32              ID;                                 /// 5
        uint32              RelicType;                          /// 6
    };

    struct ArtifactPowerLinkEntry
    {
        uint32              Index;                              /// 0
        uint16              ID;                                 /// 1
        uint16              UnlockedID;                         /// 2
    };

    struct ArtifactPowerRankEntry
    {
        uint32              Index;                              /// 0
        uint32              SpellID;                            /// 1
        float               AuraPointsOverride;                 /// 2 - 0 for 1st rank
        uint16              ItemBonusListId;                    /// 3
        uint8               Rank;                               /// 4
        uint16              ArtifactPowerID;                    /// 5
    };

    struct ArtifactCategoryEntry
    {
        uint32      ID;                                         /// 0
        uint16      XpMultCurrencyId;                           /// 1
        uint16      XpMultCurveId;                              /// 2
    };

    struct ArtifactPowerPickerEntry
    {
        uint32 ID;                                              /// 0
        uint32 PlayerConditionID;                               /// 1
    };

    struct ArtifactQuestXPEntry
    {
        uint32 ID;                                              ///< 0
        uint32 Exp[10];                                         ///< 1
    };

    struct RelicTalentEntry
    {
        uint32 ID;
        uint16 ArtifactPowerID;
        uint8  ArtifactPowerLabel;
        uint32 Type;
        uint32 PVal;
        uint32 Flags;
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// SceneScript DB2
//////////////////////////////////////////////////////////////////////////
#pragma region SceneScript
    struct SceneScriptPackageEntry
    {
        uint32              Entry;                              ///< 0
        LocalizedString const* Name;                            ///< 1
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// WebBrowser DB2
//////////////////////////////////////////////////////////////////////////
#pragma region WebBrowser
    struct WbAccessControlListEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* Url;                             ///< 1
        uint16              GrantFlags;                         ///< 2
        uint8               RevokeFlags;                        ///< 3
        uint8               WowEditInternal;                    ///< 4
        uint8               RegionId;                           ///< 5
    };

    struct WbCertWhitelistEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* Domain;                          ///< 1
        uint8               GrantAccess;                        ///< 2
        uint8               RevokeAccess;                       ///< 3
        uint8               WowEditInternal;                    ///< 4
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Item DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Item
    #define MAX_ITEM_ENCHANTMENT_EFFECTS    5
    #define MAX_ITEM_EXT_COST_ITEMS         5
    #define MAX_ITEM_EXT_COST_CURRENCIES    5
    #define MAX_ITEM_SET_ITEMS             17
    #define MAX_ITEM_SET_SPELLS             8

    struct ItemPriceBaseEntry
    {
        uint32              ID;                                 ///< 0        m_ID
        float               ArmorFactor;                        ///< 1        m_Armor
        float               WeaponFactor;                       ///< 2        m_Weapon
        uint16              ItemLevel;                          ///< 3        m_ItemLevel
    };

    struct ItemClassEntry
    {
        uint32              Class;                        ///< 0      m_ClassID
        LocalizedString const* ClassName;                 ///< 1      m_ClassNameLang
        float               PriceModifier;                ///< 2      m_PriceModifier
        uint8               ClassId;                      ///< 3
        uint8               Flags;                        ///< 4      m_Flags
    };

    struct ItemDisenchantLootEntry
    {
        uint32              Id;                                 ///< 0      m_ID
        uint16              MinItemLevel;                       ///< 1      m_MinLevel
        uint16              MaxItemLevel;                       ///< 2      m_MaxLevel
        uint16              RequiredDisenchantSkill;            ///< 3      m_SkillRequired
        int8                ItemSubClass;                       ///< 4      m_Subclass
        uint8               ItemQuality;                        ///< 5      m_Quality
        int8                Expansion;                          ///< 6
        uint8               ItemClass;                          ///< 7
    };

    struct ItemLimitCategoryEntry
    {
        uint32              ID;                                 ///< 0      Id
        LocalizedString const* name;                            ///< 1      m_name_lang
        uint8               maxCount;                           ///< 2,     m_quantity max allowed equipped as item or in gem slot
        uint8               mode;                               ///< 3,     m_flags 0 = have, 1 = equip (enum ItemLimitCategoryMode)

        uint8 GetMaxCount(Player const* p_Player) const;
    };

    struct ItemLimitCategoryConditionEntry
    {
        uint32              Index;                              ///< 0
        uint8               Count;                              ///< 1
        uint32              PlayerConditionID;                  ///< 2
        uint32              CategoryID;                         ///< 3
    };

    struct ItemRandomPropertiesEntry
    {
        uint32              ID;                                         ///< 0      m_ID
        LocalizedString const* nameSuffix;                              ///< 1      m_name_lang
        uint16              enchant_id[MAX_ITEM_ENCHANTMENT_EFFECTS];   ///< 3      m_Enchantment
    };

    struct ItemRandomSuffixEntry
    {
        uint32              ID;                                 ///< 0        m_ID
        LocalizedString const* Name;                            ///< 1        m_name_lang
        uint16              Enchantment[5];                     ///< 2        m_enchantment
        uint16              AllocationPct[5];                   ///< 3        m_allocationPct
    };

    struct ItemSpecEntry
    {
        uint32              ID;                                 ///< 0
        uint16              SpecializationID;                   ///< 1
        uint8               MinLevel;                           ///< 2
        uint8               MaxLevel;                           ///< 3
        uint8               ItemType;                           ///< 4
        uint8               PrimaryStat;                        ///< 5
        uint8               SecondaryStat;                      ///< 6
    };

    struct ItemSpecOverrideEntry
    {
        uint32              ID;                                 ///< 0      m_ID
        uint16              specID;                             ///< 1      m_SpecID
        uint32              itemEntry;                          ///< 2      m_IemID
    };

    struct ItemEntry
    {
        uint32              ID;                                 ///< 0
        uint32              IconFileDataId;                     ///< 1
        uint8               ClassId;                            ///< 2
        uint8               SubclassId;                         ///< 3
        int8                SoundOverride_subclassId;           ///< 4
        int8                Material;                           ///< 5
        int8                InventoryType;                      ///< 6
        uint8               SheatheType;                        ///< 7
        uint8               ItemGroupSoundsId;                  ///< 8
    };

    struct ItemBonusEntry
    {
        uint32              ID;                                 ///< 0
        int32               Value[3];                           ///< 1
        uint16              ParentItemBonusListId;              ///< 2
        uint8               Type;                               ///< 3
        uint8               Index;                              ///< 4
    };

    struct ItemBonusTreeNodeEntry
    {
        uint32              ID;                                 ///< 0
        uint16              ChildItemBonusTreeId;               ///< 1
        uint16              ChildItemBonusListId;               ///< 2
        uint16              ChildItemLevelSelectorId;           ///< 3
        uint8               ItemContext;                        ///< 4
        uint16              Category;                           ///< 5
    };

    struct ItemBonusListLevelDeltaEntry
    {
        int16  ItemLevelDelta;                                   ///< 0
        uint32 ID;                                               ///< 1
    };

    struct ItemXBonusTreeEntry
    {
        uint32              ID;                                  ///< 0
        uint16              ItemBonusTreeCategory;               ///< 1
        uint32              ItemId;                              ///< 2
    };

    struct ItemCurrencyCostEntry
    {
        uint32              Id;                                  ///< 0
        uint32              ItemId;                              ///< 1
    };

    struct ItemExtendedCostEntry
    {
        uint32              ID;                                                      ///< 0 extended-cost entry id
        uint32              RequiredItem[MAX_ITEM_EXT_COST_ITEMS];                   ///< 1 required item id
        uint32              RequiredCurrencyCount[MAX_ITEM_EXT_COST_CURRENCIES];     ///< 2 required currency count
        uint16              RequiredItemCount[MAX_ITEM_EXT_COST_ITEMS];              ///< 3 required count of 1st item
        uint16              RequiredPersonalArenaRating;                             ///< 4 required personal arena rating
        uint16              RequiredCurrency[MAX_ITEM_EXT_COST_CURRENCIES];          ///< 5 required currency id
        uint8               RequiredArenaSlot;                                       ///< 6 arena slot restrictions (min slot value)
        uint8               RequiredFactionId;                                       ///< 7
        uint8               RequiredFactionStanding;                                 ///< 8
        uint8               RequirementFlags;                                        ///< 9
        uint8               RequiredAchievement;                                     ///< 10
    };

    struct ItemSparseEntry
    {
        uint32              ID;                                  ///< 0
        int64               AllowableRace;                       ///< 1
        LocalizedString const* Name;                             ///< 2
        LocalizedString const* Name2;                            ///< 3
        LocalizedString const* Name3;                            ///< 4
        LocalizedString const* Name4;                            ///< 5
        LocalizedString const* Description;                      ///< 6
        uint32              Flags;                               ///< 7-0
        uint32              Flags2;                              ///< 7-1
        uint32              Flags3;                              ///< 7-2
        uint32              Flags4;                              ///< 7-3
        float               PriceRandomValue;                    ///< 8
        float               PriceVariance;                       ///< 9
        uint32              BuyCount;                            ///< 10
        uint32              BuyPrice;                            ///< 11
        uint32              SellPrice;                           ///< 12
        uint32              RequiredSpell;                       ///< 13
        uint32              MaxCount;                            ///< 14
        uint32              Stackable;                           ///< 15
        int32               ItemStatAllocation[MAX_ITEM_PROTO_STATS];  ///< 16
        float               ItemStatSocketCostMultiplier[MAX_ITEM_PROTO_STATS];///< 17
        float               RangedModRange;                      ///< 18
        uint32              BagFamily;                           ///< 19
        float               ArmorDamageModifier;                 ///< 20
        uint32              Duration;                            ///< 21
        float               StatScalingFactor;                   ///< 22
        int16               AllowableClass;                      ///< 23
        uint16              ItemLevel;                           ///< 23
        uint16              RequiredSkill;                       ///< 24
        uint16              RequiredSkillRank;                   ///< 25
        uint16              RequiredReputationFaction;           ///< 26
        int16               ItemStatValue[MAX_ITEM_PROTO_STATS]; ///< 27
        uint16              ScalingStatDistribution;             ///< 28
        uint16              Delay;                               ///< 29
        uint16              PageText;                            ///< 30
        uint16              StartQuest;                          ///< 31
        uint16              LockID;                              ///< 32
        uint16              RandomProperty;                      ///< 33
        uint16              RandomSuffix;                        ///< 34
        uint16              ItemSet;                             ///< 35
        uint16              Area;                                ///< 36
        uint16              Map;                                 ///< 37
        uint16              TotemCategory;                       ///< 58
        int16               SocketBonus;                         ///< 38
        uint16              GemProperties;                       ///< 39
        uint16              ItemLimitCategory;                   ///< 40
        uint16              HolidayId;                           ///< 41
        uint16              RequiredTransmogHolidayID;           ///< 50
        int16               ItemNameDescriptionID;               ///< 42
        uint8               Quality;                             ///< 43
        uint8               InventoryType;                       ///< 45
        int8                RequiredLevel;                       ///< 46
        uint8               RequiredHonorRank;                   ///< 47
        uint8               RequiredCityRank;                    ///< 48
        uint8               RequiredReputationRank;              ///< 49
        uint8               ContainerSlots;                      ///< 50
        int8                ItemStatType[MAX_ITEM_PROTO_STATS];  ///< 51
        uint8               DamageType;                          ///< 52
        uint8               Bonding;                             ///< 53
        uint8               LanguageID;                          ///< 54
        uint8               PageMaterial;                        ///< 55
        int8                Material;                            ///< 56
        uint8               Sheath;                              ///< 57
        uint8               SocketColor[MAX_ITEM_PROTO_SOCKETS]; ///< 59
        int8                CurrencySubstitutionId;              ///< 60
        int8                CurrencySubstitutionCount;           ///< 61
        int8                ArtifactID;                          ///< 62
        int8                RequiredExpansion;                   ///< 63
    };

    struct ItemEffectEntry
    {
        uint32              ID;                                  ///< 0
        uint32              SpellID;                             ///< 1
        uint32              SpellCooldown;                       ///< 2
        uint32              SpellCategoryCooldown;               ///< 3
        int16               SpellCharge;                         ///< 4
        uint16              SpellCategory;                       ///< 5
        uint16              ChrSpecializationID;                 ///< 6
        uint8               EffectIndex;                         ///< 7
        uint8               SpellTrigger;                        ///< 8
        uint32              ItemID;                              ///< 9
    };

    struct ItemModifiedAppearanceEntry
    {
        uint32              ItemID;                              ///< 0
        uint32              ID;                                  ///< 5
        uint8               AppearanceModID;                     ///< 2
        uint16              AppearanceID;                        ///< 1
        uint8               Index;                               ///< 3
        uint8               SourceType;                          ///< 4
    };

    struct ItemAppearanceEntry
    {
        uint32              ID;                                  ///< 0
        uint32              DisplayID;                           ///< 1
        uint32              IconFileDataID;                      ///< 2
        uint32              UIOrder;                             ///< 3
        uint8               ObjectComponentSlot;                 ///< 4
    };

    struct TransmogSetItemEntry
    {
        uint32              BaseSetID;                           ///< 0
        uint32              TransmogSetGroupID;                  ///< 1
        uint32              AppearanceId;                        ///< 2
        uint32              Flags;                               ///< 3
    };

    struct TransmogSetEntry
    {
        LocalizedString*    Name;
        uint16              BaseSetID;
        uint16              UIOrder;
        uint8               ExpansionID;
        uint32              ID;
        int32               Flags;
        int32               QuestID;
        int32               ClassMask;
        int32               ItemNameDescriptionID;
        uint32              TransmogSetGroupID;
    };

    struct ItemUpgradeEntry
    {
        uint32              Id;                                  ///< 0
        uint32              currencyCost;                        ///< 1
        uint16              precItemUpgradeId;                   ///< 2
        uint16              currencyId;                          ///< 3
        uint8               itemUpgradePath;                     ///< 4
        uint8               itemLevelUpgrade;                    ///< 5
    };

    struct ItemChildEquipmentEntry
    {
        uint32              ID;                                  ///< 0
        uint32              ChildItemID;                         ///< 1
        uint8               TargetSlot;                          ///< 2
        uint32              ItemID;                              ///< 3
    };

    struct ItemSetSpellEntry
    {
        uint32              ID;                                  ///< 0
        uint32              SpellID;                             ///< 1
        uint16              SpecializationID;                    ///< 2
        uint8               Threshold;                           ///< 3
        uint16              ItemSetID;                           ///< 4
    };

    struct ItemArmorQualityEntry
    {
        uint32              Id;                                  ///< 0        m_ID
        float               Value[7];                            ///< 1        m_Qualitymod
        uint16              Id2;                                 ///< 2        m_ItemLevel
    };

    struct ItemArmorShieldEntry
    {
        uint32              Id;                                 ///< 0        m_ID
        float               Value[7];                           ///< 1        m_Qualitymod
        uint16              Id2;                                ///< 2        m_ItemLevel
    };

    struct ItemArmorTotalEntry
    {
        uint32              Id;                                 ///< 0        m_ID
        float               Value[4];                           ///< 1        m_Qualitymod
        uint16              Id2;                                ///< 2        m_ItemLevel
    };

    struct ItemBagFamilyEntry
    {
        uint32              ID;                                ///< 0        m_ID
        LocalizedString const* m_NameLang;                     ///< 1        m_NameLang
    };

    /// common struct for:
    /// ItemDamageAmmo.db2
    /// ItemDamageOneHand.db2
    /// ItemDamageOneHandCaster.db2
    /// ItemDamageTwoHand.db2
    /// ItemDamageTwoHandCaster.db2
    struct ItemDamageEntry
    {
        uint32              Id;                                ///< 0        m_ID
        float               DPS[7];                            ///< 1        m_Qualitymod
        uint16              Id2;                               ///< 2        m_ItemLevel
    };

    struct ItemSetEntry
    {
        uint32              ID;                                ///< 0        m_ID
        LocalizedString const* Name;                           ///< 1        m_name_lang
        uint32              ItemID[MAX_ITEM_SET_ITEMS];        ///< 2        m_itemID
        uint16              RequiredSkillRank;                 ///< 3        m_requiredSkillRank
        uint32              RequiredSkill;                     ///< 4        m_requiredSkill
        uint32              Flags;                             ///< 5
    };

    struct ItemSearchNameEntry
    {
        uint64              AllowableRace;                     ///< 0
        LocalizedString*    Name;                              ///< 1
        uint32              ID;                                ///< 2
        uint32              Flags[3];                          ///< 3
        uint16              ItemLevel;                         ///< 4
        uint8               Quality;                           ///< 5
        uint8               RequiredExpansion;                 ///< 6
        uint8               RequiredLevel;                     ///< 7
        uint16              RequiredReputationFaction;         ///< 8
        uint8               RequiredReputationRank;            ///< 8
        uint32              AllowableClass;                    ///< 9
        uint16              RequiredSkill;                     ///< 10
        uint16              RequiredSkillRank;                 ///< 11
        uint32              RequiredSpell;                     ///< 12
    };

    struct RulesetItemUpgradeEntry
    {
        uint32              Id;                                ///< 0
        uint32              itemid;                            ///< 1
        uint16              itemUpgradeId;                     ///< 2
    };

    struct PvpItemEntry
    {
        uint32              RecordID;                          ///< 0
        uint32              itemId;                            ///< 1
        uint8               ilvl;                              ///< 2
    };

    struct PvpScalingEffectEntry
    {
        uint32              Id;                                ///< 0
        float               Value;                             ///< 1
        uint32              PvpScalingEffectTypeId;            ///< 2
        uint32              SpecializationId;                  ///< 3
    };

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Archaeology DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Archaeology
    struct ResearchBranchEntry
    {
        uint32              ID;                                 ///< 0        m_ID
        LocalizedString const* m_Name;                          ///< 1        m_NameLang
        uint32              ItemID;                             ///< 2        m_ItemID
        uint16              CurrencyID;                         ///< 3        m_CurrencyID
        uint8               m_ResearchFieldID;                  ///< 4        m_ResearchFieldID
        uint32              m_TextureFileID1;                   ///< 5        m_Texture
        uint32              m_TextureFileID2;                   ///< 6        m_Texture
    };

    struct ResearchProjectEntry
    {
        LocalizedString const* m_NameLang;                      ///< 0        m_NameLang
        LocalizedString const* m_DescriptionLang;               ///< 1        m_DescriptionLang
        uint32              SpellID;                            ///< 2        m_SpellID
        uint16              ResearchBranchID;                   ///< 3        m_ResearchBranchID
        uint8               Rarity;                             ///< 4        m_Rarity
        uint8               NumSockets;                         ///< 5        m_NumSockets
        uint32              ID;                                 ///< 6        m_ID
        uint32              m_TextureFileID;                    ///< 7        m_Texture
        uint32              RequiredWeight;                     ///< 8        m_RequiredWeight

    };

    struct ResearchSiteEntry
    {
        uint32              ID;                                 ///< 0        m_ID
        LocalizedString const* m_NameLang;                      ///< 1        m_NameLang
        uint32              POIid;                              ///< 2        m_QuestPOIBlobID
        uint16              mapId;                              ///< 3        m_MapID
        uint32              AreaPOIIconEnum;                    ///< 4        m_AreaPOIIconEnum
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Quest DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Quest
    struct QuestV2CliTaskEntry
    {
        uint64              FiltRaces;                          ///< 0
        LocalizedString const* QuestTitle;                      ///< 1
        LocalizedString const* BulletText;                      ///< 2
        uint32              StartItem;                          ///< 3
        uint16              UniqueBitFlag;                      ///< 4
        uint16              PlayerConditionID;                  ///< 5
        uint16              FiltClasses;                        ///< 6
        uint16              FiltCompletedQuest[3];              ///< 7
        uint16              FiltMinSkillId;                     ///< 8
        uint16              WorldStateExpressionID;             ///< 9
        uint8               FiltActiveQuest;                    ///< 10
        uint8               FiltCompletedQuestLogic;            ///< 11
        uint8               FiltMaxFactionId;                   ///< 12
        uint8               FiltMaxFactionValue;                ///< 13
        uint8               FiltMaxLevel;                       ///< 14
        uint8               FiltMinFactionId;                   ///< 15
        uint8               FiltMinFactionValue;                ///< 16
        uint8               FiltMinLevel;                       ///< 17
        uint8               FiltMinSkillValue;                  ///< 18
        uint8               FiltNonActiveQuest;                 ///< 19
        uint32              QuestID;                            ///< 20
        uint32              BreadCrumbId;                       ///< 21
        uint32              QuestInfoId;                        ///< 22
        uint32              SandboxScalingId;                   ///< 23
    };

    struct QuestMoneyRewardEntry
    {
        uint32 Level;
        uint32 Money[10];
    };

    struct BountyEntry
    {
        uint32 m_Index;                                         ///< 0
        uint32 IconFileDataId;                                  ///< 1
        uint16 m_QuestID;                                       ///< 2
        uint16 m_FactionID;                                     ///< 3
        uint32 TurninPlayerConditionId;                         ///< 4
        uint8 m_RequiredCount;                                  ///< 5
    };

    struct AdventureMapPOIEntry
    {
        uint32                    m_ID;                   ///< 0
        LocalizedString const*    m_Name;                 ///< 0
        LocalizedString const*    m_Description;          ///< 1
        float                     m_Position[2];          ///< 2 - 2
        uint32                    RewardItemId;           ///< 3
        uint8                     Type;                   ///< 4 category 0 internal 1 zone intro 2 launch by item 3 class hall
        uint32                    PlayerConditionId;      ///< 7
        uint32                    m_QuestID;              ///< 8
        uint32                    LfgDungeonId;           ///< 9
        uint32                    UiTextureAtlasMemberId; ///< 10
        uint32                    UiTextureKitId;         ///< 11
        uint32                    WorldMapAreaId;         ///< 12
        uint32                    DungeonMapId;           ///< 13
        uint32                    AreaTableId;            ///< 14
    };

    struct QuestPackageItemEntry
    {
        uint32              ID;                                 ///< 0
        uint32              ItemID;                             ///< 1
        uint16              QuestPackageID;                     ///< 2
        uint8               FilterType;                         ///< 3
        uint32              ItemCount;                          ///< 4
    };

    struct QuestPOIPointEntry
    {
        uint32              ID;                                 ///< 0        m_ID
        int16               x;                                  ///< 1        m_X
        int16               y;                                  ///< 2        m_Y
        uint32              QuestPOIBlobId;                     ///< 3        m_QuestPOIBlobID
    };

    struct QuestSortEntry
    {
        uint32              id;                                 ///< 0      m_ID
        LocalizedString const* m_SortName_lang;                 ///< 1      m_SortName_lang
        uint8               SortOrder;                          ///< 2      m_SortOrder
    };

    struct QuestV2Entry
    {
        uint32              ID;                                 ///< 0
        uint16              UniqueBitFlag;                      ///< 1
    };

    struct QuestXPEntry
    {
        uint32              id;                                 ///< 0      m_ID
        uint16              Exp[10];                            ///< 1      m_Difficulty
    };

    struct QuestFactionRewEntry
    {
        uint32              id;                                 ///< 0        m_ID
        int16               QuestRewFactionValue[10];           ///< 1        m_Difficulty
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Area and maps DB2
//////////////////////////////////////////////////////////////////////////
#pragma region AreaAndMaps
    #define MAX_WORLD_MAP_OVERLAY_AREA_IDX 4

    struct AreaTableEntry
    {
        uint32              ID;                                 ///< 0        m_ID
        LocalizedString const* m_ZoneName;                      ///< 1        m_ZoneName
        LocalizedString const* AreaNameLang;                    ///< 2        m_AreaNameLang
        uint32              Flags;                              ///< 3-0      m_Flags
        uint32              m_Flags2;                           ///< 3-1      m_Flags2
        float               m_AmbientMultiplier;                ///< 4        m_AmbientMultiplier
        uint16              ContinentID;                        ///< 5        m_ContinentID
        uint16              ParentAreaID;                       ///< 6        m_ParentAreaID          If 0 then it's zone, else it's zone id of this area
        uint16              AreaBit;                            ///< 7        m_AreaBit               Main index
        uint16              m_AmbienceID;                       ///< 8        m_AmbienceID
        uint16              m_ZoneMusic;                        ///< 9        m_ZoneMusic
        uint16              m_IntroSound;                       ///< 10       m_IntroSound
        uint16              LiquidTypeID[4];                    ///< 11       m_LiquidTypeID          Liquid override by type
        uint16              m_UwZoneMusic;                      ///< 12       m_UwIntroSound          All zeros
        uint16              m_UwAmbience;                       ///< 13       m_UwAmbience
        int16               m_PvpCombatWorldStateID;            ///< 14       m_PvpCombatWorldStateID
        uint8               m_SoundProviderPref;                ///< 15       m_SoundProviderPref
        uint8               m_SoundProviderPrefUnderwater;      ///< 16       m_SoundProviderPrefUnderwater
        int8                ExplorationLevel;                   ///< 17       m_ExplorationLevel
        uint8               FactionGroupMask;                   ///< 18       m_FactionGroupMask
        uint8               m_MountFlags;                       ///< 19       m_MountFlags
        uint8               m_WildBattlePetLevelMin;            ///< 20       m_WildBattlePetLevelMin
        uint8               m_WildBattlePetLevelMax;            ///< 21       m_WildBattlePetLevelMax
        uint8               m_WindSettingsID;                   ///< 22       m_WindSettingsID
        uint32              m_UwIntroSound;                     ///< 23       m_UwIntroSound          All zeros

        // /DeathKnightStart map, maybe AreaFlags ?
        bool IsSanctuary() const
        {
            if (ID == 7594) ///< Dalaran Underbelly
                return false;

            if (ContinentID == 609)
                return true;

            return (Flags & AREA_FLAG_SANCTUARY) != 0;
        }
    };

    struct AreaGroupMemberEntry
    {
        uint32              ID;                                 ///< 0
        uint16              AreaID;                             ///< 1
        uint16              AreaGroupID;                        ///< 2
    };

    struct AreaTriggerEntry
    {
        float               Pos[3];                             ///< 0  - 4m_x
        float               Radius;                             ///< 1    m_radius
        float               BoxLength;                          ///< 2    m_box_length
        float               BoxWidth;                           ///< 3    m_box_width
        float               BoxHeight;                          ///< 4    m_box_heigh
        float               BoxYaw;                             ///< 5    m_box_yaw
        uint16              ContinentID;                        ///< 6    m_ContinentID
        uint16              PhaseID;                            ///< 7
        uint16              PhaseGroupID;                       ///< 8
        uint16              ShapeID;                            ///< 9
        uint16              AreaTriggerActionSetID;             ///< 10
        uint8               PhaseUseFlags;                      ///< 11
        uint8               ShapeType;                          ///< 12
        uint8               Flag;                               ///< 13
        uint32              ID;                                 ///< 14
    };

    struct DifficultyEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* Name;                            ///< 1
        uint16              GroupSizeHealthCurveID;             ///< 2
        uint16              GroupSizeDmgCurveID;                ///< 3
        uint16              GroupSizeSpellPointsCurveID;        ///< 4
        uint8               FallbackDifficultyID;               ///< 5
        uint8               InstanceType;                       ///< 6
        uint8               MinPlayers;                         ///< 7
        uint8               MaxPlayers;                         ///< 8
        int8                OldEnumValue;                       ///< 9
        uint8               Flags;                              ///< 10
        uint8               ToggleDifficultyID;                 ///< 11
        uint8               ItemBonusTreeModID;                 ///< 12
        uint8               OrderIndex;                         ///< 13
    };

    struct LiquidTypeEntry
    {
        uint32              Id;                                 ///< 0
        LocalizedString const* Name;                            ///< 1
        LocalizedString const* Texture[6];                      ///< 2
        uint32              SpellID;                            ///< 3
        float               MaxDarkenDepth;                     ///< 4
        float               FogDarkenIntensity;                 ///< 5
        float               AmbDarkenIntensity;                 ///< 6
        float               DirDarkenIntensity;                 ///< 7
        float               ParticleScale;                      ///< 8
        uint32              Color[2];                           ///< 9
        float               Float[18];                          ///< 10
        uint32              Int[4];                             ///< 11
        uint16              Flags;                              ///< 12
        uint16              LightID;                            ///< 13
        uint8               Type;                               ///< 14
        uint8               ParticleMovement;                   ///< 15
        uint8               ParticleTexSlots;                   ///< 16
        uint8               MaterialID;                         ///< 17
        uint8               DepthTexCount[6];                   ///< 18
        uint32              SoundID;                            ///< 19
    };

    struct MapEntry
    {
        uint32              MapID;                              ///< 0
        LocalizedString const* Directory;                       ///< 1
        LocalizedString const* MapNameLang;                     ///< 2
        LocalizedString const* MapDescription0;                 ///< 3
        LocalizedString const* MapDescription1;                 ///< 4
        LocalizedString const* ShortDescription;                ///< 5
        LocalizedString const* LongDescription;                 ///< 6
        uint32              Flags;                              ///< 7-0
        uint32              Flags2;                             ///< 7-1
        float               MinimapIconScale;                   ///< 8
        float               CorpseX;                            ///< 9-0       m_corpseX               entrance x coordinate in ghost mode  (in most cases = normal entrance)
        float               CorpseY;                            ///< 9-1       m_corpseY               entrance y coordinate in ghost mode  (in most cases = normal entrance)
        uint16              AreaTableID;                        ///< 10
        uint16              LoadingScreenID;                    ///< 11
        int16               CorpseMapID;                        ///< 12
        uint16              TimeOfDayOverride;                  ///< 13
        int16               ParentMapID;                        ///< 14
        int16               CosmeticParentMapID;                ///< 15
        uint16              WindSettingsID;                     ///< 16
        uint8               instanceType;                       ///< 17
        uint8               MapType;                            ///< 18
        uint8               ExpansionID;                        ///< 19
        uint8               MaxPlayers;                         ///< 20
        uint8               TimeOffset;                         ///< 21

        // Helpers
        uint32 Expansion() const { return ExpansionID; }

        bool IsDungeon()                const { return instanceType == MAP_INSTANCE || instanceType == MAP_RAID || instanceType == MAP_SCENARIO; }
        bool IsNonRaidDungeon()         const { return instanceType == MAP_INSTANCE; }
        bool Instanceable()             const { return instanceType == MAP_INSTANCE || instanceType == MAP_RAID || instanceType == MAP_BATTLEGROUND || instanceType == MAP_ARENA || instanceType == MAP_SCENARIO; }
        bool IsRaid()                   const { return instanceType == MAP_RAID; }
        bool IsBattleground()           const { return instanceType == MAP_BATTLEGROUND; }
        bool IsBattleArena()            const { return instanceType == MAP_ARENA; }
        bool IsScenario()               const { return instanceType == MAP_SCENARIO; }
        bool IsBattlegroundOrArena()    const { return instanceType == MAP_BATTLEGROUND || instanceType == MAP_ARENA; }
        bool IsWorldMap()               const { return instanceType == MAP_COMMON; }

        bool GetEntrancePos(int32 &mapid, float &x, float &y) const
        {
            if (CorpseMapID < 0)
                return false;
            mapid = CorpseMapID;
            x = CorpseX;
            y = CorpseY;
            return true;
        }

        // @todo : use m_ContinentID from AreTable.db2 used for SPELL_ATTR4_CAST_ONLY_IN_OUTLAND issue #281
        bool IsContinent() const
        {
            return MapID == 0 || MapID == 1 || MapID == 530 || MapID == 571 || MapID == 860 || MapID == 870 || MapID == 1116 || MapID == 1220;
        }

        bool IsDynamicDifficultyMap() const { return (Flags & MAP_FLAG_CAN_TOGGLE_DIFFICULTY) != 0; }
    };

    struct MapDifficultyEntry
    {
        uint32              ID;                                 ///< 0
        LocalizedString const* Message_lang;                    ///< 1  m_message_lang (text showed when transfer to map failed)
        uint8               DifficultyID;                       ///< 2
        uint8               RaidDurationType;                   ///< 3  1 means daily reset, 2 means weekly
        uint8               MaxPlayers;                         ///< 4  m_maxPlayers some heroic versions have 0 when expected same amount as in normal version
        uint8               LockID;                             ///< 5
        uint8               Flags;                              ///< 6
        uint8               ItemBonusTreeModID;                 ///< 7
        uint32              Context;                            ///< 8
        uint16              MapID;                              ///< 9

        uint32 GetRaidDuration() const
        {
            if (RaidDurationType == 1)
                return 86400;
            if (RaidDurationType == 2)
                return 604800;
            return 0;
        }
    };

    struct PhaseEntry
    {
        uint32              ID;                                 ///< 0        m_ID
        uint16              flag;                               ///< 1        m_Flags
    };

    struct WorldMapOverlayEntry
    {
        LocalizedString const* TextureName;                             ///< 1
        uint32              ID;                                         ///< 0
        uint16              TextureWidth;                               ///< 2
        uint16              TextureHeight;                              ///< 3
        uint32              MapAreaID;                                  ///< 4            // idx in WorldMapArea.db2
        uint32              OffsetX;                                    ///< 6
        uint32              OffsetY;                                    ///< 7
        uint32              HitRectTop;                                 ///< 8
        uint32              HitRectLeft;                                ///< 9
        uint32              HitRectBottom;                              ///< 10
        uint32              HitRectRight;                               ///< 11
        uint32              PlayerConditionID;                          ///< 12
        uint32              Flags;                                      ///< 13
        uint32              areatableID[MAX_WORLD_MAP_OVERLAY_AREA_IDX];///< 5
    };

    struct WMOAreaTableEntry
    {
        LocalizedString const* AreaName;                                ///< 0
        int32               WMOGroupID;                                 ///< 1  used in group WMO
        uint16              AmbienceID;                                 ///< 2
        uint16              ZoneMusic;                                  ///< 3
        uint16              IntroSound;                                 ///< 4
        uint16              AreaTableID;                                ///< 5
        uint16              UWIntroSound;                               ///< 6
        uint16              UWAmbience;                                 ///< 7
        int8                NameSet;                                    ///< 8  used in adt file
        uint8               SoundProviderPref;                          ///< 9
        uint8               SoundProviderPrefUnderwater;                ///< 10
        uint8               Flags;                                      ///< 11
        uint32              ID;                                         ///< 12
        uint32              UWZoneMusic;                                ///< 13
        int16               WMOID;                                      ///< 14  used in root WMO
    };

    struct WorldMapAreaEntry
    {
        LocalizedString const* AreaName;                                ///< 0
        float               LocLeft;                                    ///< 1
        float               LocRight;                                   ///< 2
        float               LocTop;                                     ///< 3
        float               LocBottom;                                  ///< 4
        uint32              Flags;                                      ///< 5
        uint16              MapID;                                      ///< 6
        uint16              AreaID;                                     ///< 7
        int16               DisplayMapID;                               ///< 8
        int16               DefaultDungeonFloor;                        ///< 9
        uint16              ParentWorldMapID;                           ///< 10
        uint8               LevelRangeMin;                              ///< 11
        uint8               LevelRangeMax;                              ///< 12
        uint8               BountySetID;                                ///< 13
        uint8               BountyBoardLocation;                        ///< 14
        uint32              ID;                                         ///< 15
        uint32              PlayerConditionID;                          ///< 16
    };

    struct WorldMapTransformsEntry
    {
        uint32              ID;                                         ///< 0
        float               RegionMinX;                                 ///< 1-0
        float               RegionMinY;                                 ///< 1-1
        float               RegionMinZ;                                 ///< 1-2
        float               RegionMaxX;                                 ///< 1-3
        float               RegionMaxY;                                 ///< 1-4
        float               RegionMaxZ;                                 ///< 1-5
        float               RegionOffsetX;                              ///< 2-0
        float               RegionOffsetY;                              ///< 2-1
        float               RegionScale;                                ///< 3
        uint16              MapID;                                      ///< 4
        uint16              AreaID;                                     ///< 5
        uint16              NewMapID;                                   ///< 6
        uint16              NewDungeonMapID;                            ///< 7
        uint16              NewAreaID;                                  ///< 8
        uint8               Flags;                                      ///< 9
        int32               Priority;                                   ///< 10
    };

    struct World_PVP_AreaEntry
    {
        uint32              ID;                                         ///< 0
        uint16              AreaID;                                     ///< 1
        uint16              NextTimeWorldState;                         ///< 2
        uint16              GameTimeWorldState;                         ///< 3
        uint16              BattlePopulateTime;                         ///< 4
        int16               MapID;                                      ///< 5
        uint8               MinLevel;                                   ///< 6
        uint8               MaxLevel;                                   ///< 7
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Import DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Import
    struct ImportPriceArmorEntry
    {
        uint32              ID;                                         ///< 0
        float               ClothFactor;                                ///< 1
        float               LeatherFactor;                              ///< 2
        float               MailFactor;                                 ///< 3
        float               PlateFactor;                                ///< 4
    };

    struct ImportPriceQualityEntry
    {
        uint32              QualityId;                                  ///< 0      m_ID
        float               Factor;                                     ///< 1      m_data
    };

    struct ImportPriceShieldEntry
    {
        uint32              Id;                                         ///< 0      m_ID
        float               Factor;                                     ///< 1      m_data
    };

    struct ImportPriceWeaponEntry
    {
        uint32              Id;                                         ///< 0      m_ID
        float               Factor;                                     ///< 1      m_data
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Char DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Char
    #define MAX_OUTFIT_ITEMS 24

    struct CharStartOutfitEntry
    {
        uint32              m_ID;                                       ///< 0      m_ID
        int32               ItemId[MAX_OUTFIT_ITEMS];                   ///< 1      m_itemID
        uint32              m_petDisplayID;                             ///< 2      m_petDisplayID
        uint8               ClassID;                                    ///< 3
        uint8               SexID;                                      ///< 4
        uint8               OutfitID;                                   ///< 5
        uint8               m_petFamilyId;                              ///< 6     m_petFamilyId
        uint8               RaceID;                                     ///< 7
    };

    struct CharacterLoadoutItemEntry
    {
        uint32              ID;                                         ///< 0
        uint32              ItemID;                                     ///< 1
        uint16              LoadoutID;                                  ///< 2
    };

    struct CharacterLoadoutEntry
    {
        uint32              ID;                                         ///< 1
        uint64              Flags;                                      ///< 2
        uint8               ClassID;                                    ///< 3
        uint8               Expansion;                                  ///< 4
    };

    struct CharTitlesEntry
    {
        uint32              ID;                                         ///< 0
        LocalizedString const* NameMale;                                ///< 1
        LocalizedString const* NameFemale;                              ///< 2
        uint16              MaskID;                                     ///< 3
        uint8               Flags;                                      ///< 4
    };

    struct ChrClassesEntry
    {
        LocalizedString const* m_petNameToken;                          ///< 0        m_petNameToken
        LocalizedString const* NameLang;                                ///< 1        m_name_lang
        LocalizedString const* m_name_female_lang;                      ///< 2        m_name_female_lang
        LocalizedString const* m_name_male_lang;                        ///< 3        m_name_male_lang
        LocalizedString const* m_filename;                              ///< 4        m_filename
        uint32              m_CreateScreenFileDataID;                   ///< 5        m_CreateScreenFileDataID
        uint32              m_SelectScreenFileDataID;                   ///< 6        m_SelectScreenFileDataID
        uint32              m_IconFileDataID;                           ///< 7        m_IconFileDataID
        uint32              m_LowResScreenFileDataID;                   ///< 8        m_LowResScreenFileDataID
        uint32              m_RequiredPlayerLevel;                      ///< 9        m_RequiredPlayerLevel - 7.3.5
        uint16              flags;                                      ///< 10       m_flags                 (0x08 HasRelicSlot)
        uint16              CinematicSequenceID;                        ///< 11       m_cinematicSequenceID
        uint16              m_DefaultSpec;                              ///< 12       m_DefaultSpec
        uint8               DisplayPower;                               ///< 13       m_DisplayPower
        uint8               SpellClassSet;                              ///< 14       m_spellClassSet
        uint8               AttackPowerPerStrength;                     ///< 15       m_AttackPowerPerStrength        Attack Power bonus per point of strength
        uint8               AttackPowerPerAgility;                      ///< 16       m_AttackPowerPerAgility         Attack Power bonus per point of agility
        uint8               RangedAttackPowerPerAgility;                ///< 17       m_RangedAttackPowerPerAgility   Ranged Attack Power bonus per point of agility
        uint8               MainStat;                                   ///< 18       m_MainStat
        uint32              ClassID;                                    ///< 19       m_ID
    };

    struct ChrClassXPowerTypesEntry
    {
        uint32              entry;                                      ///< 0
        uint8               power;                                      ///< 1
        uint8               classId;                                    ///< 2
    };

    struct ChrRacesEntry
    {
        LocalizedString const* m_ClientPrefix;                          ///< 0        m_ClientPrefix
        LocalizedString const* m_ClientFileString;                      ///< 1        m_ClientFileString
        LocalizedString const* name;                                    ///< 2        m_NameLang used for DB2 language detection/selection
        LocalizedString const* m_NameFemaleLang;                        ///< 3        m_NameFemaleLang
        LocalizedString const* m_NameMaleLang;                          ///< 4        m_NameMaleLang
        LocalizedString const* m_HairCustomization;                     ///< 5        m_HairCustomization
        uint32              Flags;                                      ///< 6        m_Flags
        uint32              MaleDisplayID;                              ///< 7        m_MaleDisplayId
        uint32              FemaleDisplayID;                            ///< 8        m_FemaleDisplayId
        uint32              m_CreateScreenFileDataID;                   ///< 9        m_CreateScreenFileDataID
        uint32              m_SelectScreenFileDataID;                   ///< 10       m_SelectScreenFileDataID
        float               m_MaleCustomizeOffset[3];                   ///< 11       m_MaleCustomizeOffset
        float               m_FemaleCustomizeOffset[3];                 ///< 12       m_FemaleCustomizeOffset
        uint32              m_LowResScreenFileDataID;                   ///< 13       m_LowResScreenFileDataID
        uint32              StartingLevel;                              ///< 14       735 Allied races start at lvl 20
        uint32              UIDisplayOrder;                             ///< 15
        uint16              FactionID;                                  ///< 16       m_FactionID
        uint16              m_ResSicknessSpellID;                       ///< 17       m_ResSicknessSpellID
        uint16              m_SplashSoundID;                            ///< 18       m_SplashSoundID
        uint16              CinematicSequence;                          ///< 19       m_CinematicSequenceID
        uint8               BaseLanguage;                               ///< 20       m_BaseLanguage          (7 Alliance, 1 Horde, 42 Neutral)
        uint8               m_CreatureType;                             ///< 21       m_CreatureType
        uint8               m_alliance;                                 ///< 22       m_alliance              (0 alliance, 1 horde, 2 disabled races)
        uint8               m_RaceRelated;                              ///< 23       m_RaceRelated
        uint8               m_UnalteredVisualRaceID;                    ///< 24       m_UnalteredVisualRaceID
        uint8               m_CharComponentTextureLayoutID;             ///< 25       m_CharComponentTextureLayoutID
        uint8               m_DefaultClassID;                           ///< 26       m_DefaultClassID
        uint8               m_NeutralRaceID;                            ///< 27       m_NeutralRaceID
        uint8               ItemAppearanceFrameRaceID;                  ///< 28       ItemAppearanceFrameRaceID
        uint8               m_CharComponentTexLayoutHiResID;            ///< 29       m_CharComponentTexLayoutHiResID
        uint32              RaceID;                                     ///< 30       m_ID
        uint32              m_HighResMaleDisplayId;                     ///< 31       m_HighResMaleDisplayId
        uint32              m_HighResFemaleDisplayId;                   ///< 32       m_HighResFemaleDisplayId
        uint32              HeritageArmorAchievementID;                 ///< 33       7.3.5 Allied Races heritage armor
        uint32              MaleCorpseBonesModelFileDataID;             ///< 34
        uint32              FemaleCorpseBonesModelFileDataID;           ///< 35
        uint32              AlteredFormTransitionSpellVisualID[3];      ///< 36
        uint32              AlteredFormTransitionSpellVisualKitID[3];   ///< 37


    };

    struct ChrSpecializationsEntry
    {
        LocalizedString const* SpecializationName;                      ///< 2        m_NameLang
        LocalizedString const* NameLang;                                ///< 3        m_NameLang1
        LocalizedString const* Description;                             ///< 4        m_DescriptionLang
        uint32              MasterySpellID;                             ///< 0        m_MasterySpellID
        uint32              MasterySpellID2;                            ///< 1        m_MasterySpellID
        uint8               ClassID;                                    ///< 5        m_ClassID
        uint8               OrderIndex;                                 ///< 6        m_OrderIndex
        uint8               PetTalentType;                              ///< 7        m_PetTalentType
        uint8               Role;                                       ///< 8        m_Role
        uint8               MainStat;                                   ///< 9        m_MainStat
        uint32              ID;                                         ///< 10       m_ID
        uint32              IconID;                                     ///< 11
        uint32              Flags;                                      ///< 12       m_Flags
        uint32              AnimReplacementSetID;                       ///< 13       m_AnimReplacementSetID
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Creature DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Creature
    struct CreatureDisplayInfoEntry
    {
        uint32              ID;                                         ///< 0
        float               CreatureModelScale;                         ///< 1  CreatureModelScale
        uint16              ModelID;                                    ///< 2
        uint16              NPCSoundID;                                 ///< 3
        uint8               SizeClass;                                  ///< 4
        uint8               Flags;                                      ///< 5
        int8                Gender;                                     ///< 6
        int32               ExtendedDisplayInfoID;                      ///< 7
        uint32              PortraitTextureFileID;                      ///< 8
        uint8               CreatureModelAlpha;                         ///< 9
        uint16              SoundID;                                    ///< 10
        float               PlayerModelScale;                           ///< 11
        uint32              PortraitCreatureDisplayInfoID;              ///< 12
        uint8               BloodID;                                    ///< 13
        uint16              ParticleColorID;                            ///< 14
        uint32              CreatureGeosetData;                         ///< 15
        uint16              ObjectEffectPackageID;                      ///< 16
        uint16              AnimReplacementSetID;                       ///< 17
        int8                UnarmedWeaponSubclass;                      ///< 18
        uint32              StateSpellVisualKitID;                      ///< 19
        float               InstanceOtherPlayerPetScale;                ///< 20
        uint32              MountSpellVisualKitID;                      ///< 21
        int32               TextureVariation[3];                        ///< 22
    };

    struct CreatureDisplayInfoExtraEntry
    {
        uint32              ID;                                         ///< 0
        uint32              FileDataID;                                 ///< 1
        uint32              HDFileDataID;                               ///< 2
        uint8               DisplayRaceID;                              ///< 3
        uint8               DisplaySexID;                               ///< 4
        uint8               DisplayClassID;                             ///< 5
        uint8               SkinID;                                     ///< 6
        uint8               FaceID;                                     ///< 7
        uint8               HairStyleID;                                ///< 8
        uint8               HairColorID;                                ///< 9
        uint8               FacialHairID;                               ///< 10
        uint8               CustomDisplayOption[3];                     ///< 11
        uint8               Flags;                                      ///< 12
    };

    struct CreatureFamilyEntry
    {
        uint32              ID;                                         ///< 0
        LocalizedString const* Name;                                    ///< 1
        float               minScale;                                   ///< 2
        float               maxScale;                                   ///< 3
        uint32              IconFileDataID;                             ///< 4
        uint16              skillLine[2];                               ///< 5
        uint16              petFoodMask;                                ///< 6
        uint8               minScaleLevel;                              ///< 7
        uint8               maxScaleLevel;                              ///< 8
        uint8               petTalentType;                              ///< 9
    };

    struct CreatureModelDataEntry
    {
        uint32              Id;                                         ///< 0
        float               ModelScale;                                 ///< 1
        float               FootprintTextureLength;                     ///< 2
        float               FootprintTextureWidth;                      ///< 3
        float               FootprintParticleScale;                     ///< 4
        float               CollisionWidth;                             ///< 5
        float               CollisionHeight;                            ///< 6
        float               MountHeight;                                ///< 7
        float               GeoBoxMin[3];                               ///< 8-0
        float               GeoBoxMax[3];                               ///< 8-3
        float               WorldEffectScale;                           ///< 9
        float               AttachedEffectScale;                        ///< 10
        float               MissileCollisionRadius;                     ///< 11
        float               MissileCollisionPush;                       ///< 12
        float               MissileCollisionRaise;                      ///< 13
        float               OverrideLootEffectScale;                    ///< 14
        float               OverrideNameScale;                          ///< 15
        float               OverrideSelectionRadius;                    ///< 16
        float               TamedPetBaseScale;                          ///< 17
        float               HoverHeight;                                ///< 18
        uint32              Flags;                                      ///< 19
        uint32              FileDataID;                                 ///< 20
        uint32              SizeClass;                                  ///< 21
        uint32              BloodID;                                    ///< 22
        uint32              FootprintTextureID;                         ///< 23
        uint32              FoleyMaterialID;                            ///< 24
        uint32              FootstepEffectID;                           ///< 25
        uint32              DeathThudEffectID;                          ///< 26
        uint32              SoundID;                                    ///< 27
        uint32              CreatureGeosetDataID;                       ///< 38
    };

    struct CreatureTypeEntry
    {
        uint32              ID;                                         ///< 0      m_ID
        LocalizedString const* nameLang;                                ///< 1      m_name_lang
        uint8               flags;                                      ///< 2      m_flags       1 = no_xp
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Misc DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Misc
    #define MAX_FACTION_RELATIONS 4
    #define MAX_HOLIDAY_DURATIONS 10
    #define MAX_BROADCAST_TEXT_EMOTES 3
    #define MAX_HOLIDAY_DATES 16
    #define MAX_HOLIDAY_FLAGS 10
    #define MAX_HEIRLOOM_UPGRADE_LEVEL 3
    #define MAX_MOUNT_CAPABILITIES 26
    #define MAX_LOCK_CASE 8
    #define MAX_VEHICLE_SEATS 8

    struct AnimKitEntry
    {
        uint32              ID;                                         ///< 0
        uint32              OneShotDuration;                            ///< 1
        uint16              OneShotStopAnimKitID;                       ///< 2
        uint16              LowDefAnimKitID;                            ///< 3
    };

    struct ArmorLocationEntry
    {
        uint32              InventoryType;                              ///< 0        m_ID
        float               Value[5];                                   ///< 1        m_clothmodifier / m_leathermodifier / m_chainmodifier / m_platemodifier / m_modifier  //multiplier for armor types
    };

    struct AuctionHouseEntry
    {
        uint32              ID;                                         ///< 0        m_ID
        LocalizedString const* Name;                                    ///< 1
        uint16              FactionID;                                  ///< 2        m_factionID             m_ID of faction.db2 for player factions associated with city
        uint8               DepositRate;                                ///< 3        m_depositRate           1/3 from real
        uint8               ConsignmentRate;                            ///< 4        m_consignementRate
    };

    struct BankBagSlotPricesEntry
    {
        uint32              ID;                                         ///< 0        m_ID
        uint32              price;                                      ///< 1        m_cost
    };

    struct BarberShopStyleEntry
    {
        LocalizedString const* m_DisplayName_lang;                      ///< 0        m_DisplayName_lang
        LocalizedString const* m_Description_lang;                      ///< 1        m_Description_lang
        float               m_CostMultiplier;                           ///< 2        m_Cost_Modifier
        uint8               Type;                                       ///< 3        m_type                  Value 0 -> hair, value 2 -> facial hair
        uint8               Race;                                       ///< 4        m_race
        uint8               Sex;                                        ///< 5        m_sex
        uint8               Data;                                       ///< 6        m_data                  Real ID to hair/facial hair
        uint32              ID;                                         ///< 7        m_ID
    };

    struct BattlemasterListEntry
    {
        uint32              ID;                                         ///< 0
        LocalizedString const* NameLang;                                ///< 1
        LocalizedString const* GameType;                                ///< 3
        LocalizedString const* ShortDescription;                        ///< 4
        LocalizedString const* LongDescription;                         ///< 5
        uint32              IconFileDataID;                             ///< 2
        int16               MapID[16];                                  ///< 6
        uint16              HolidayWorldState;                          ///< 7
        uint16              PlayerConditionID;                          ///< 8
        uint8               InstanceType;                               ///< 9
        uint8               GroupsAllowed;                              ///< 10
        uint8               MaxGroupSize;                               ///< 11
        uint8               MinLevel;                                   ///< 12
        uint8               MaxLevel;                                   ///< 13
        uint8               RatedPlayers;                               ///< 14
        uint8               MinPlayers;                                 ///< 15
        uint8               MaxPlayers;                                 ///< 16
        uint8               Flags;                                      ///< 17
    };

    struct BroadcastTextEntry
    {
        uint32                  ID;                                     ///< 0
        LocalizedString const*  MaleText;                               ///< 1
        LocalizedString const*  FemaleText;                             ///< 2
        uint16                  EmoteID[MAX_BROADCAST_TEXT_EMOTES];     ///< 3 - 5
        uint16                  EmoteDelay[MAX_BROADCAST_TEXT_EMOTES];  ///< 6 - 8
        uint16                  EmotesId;                               ///< 9
        uint8                   Language;                               ///< 10
        uint8                   Type;                                   ///< 11
        uint32                  PlayerConditionID;                      ///< 12
        uint32                  SoundID[2];                             ///< 13 - 14
    };

    struct CharSectionsEntry
    {
        uint32 ID;                                                      ///< 0
        uint32 TextureFileDataID[3];                                    ///< 1 - 2 - 3
        uint16 Flags;                                                   ///< 4
        uint8 Race;                                                     ///< 5
        uint8 Gender;                                                   ///< 6
        uint8 GenType;                                                  ///< 7
        uint8 Type;                                                     ///< 8
        uint8 Color;                                                    ///< 9
    };

    struct CinematicCameraEntry
    {
        uint32              id;                                         ///< 0
        uint32              soundid;                                    ///< 1
        float               EndPosX;                                    ///< 2-0
        float               EndPosY;                                    ///< 2-1
        float               EndPosZ;                                    ///< 2-2
        float               GlobalRotation;                             ///< 3
        uint32              ModelFileDataID;                            ///< 4
    };

    struct CinematicSequencesEntry
    {
        uint32              Id;                                         ///< 0
        uint32              SoundID;                                    ///< 1
        uint16              cinematicCamera[8];                         ///< 2
    };

    struct ChatChannelsEntry
    {
        uint32              ID;                                         ///< 0
        LocalizedString const* NameLang;                                ///< 2
        LocalizedString const* Shortcut;                                ///< 3
        uint32              Flags;                                      ///< 1
        uint8               FactionGroup;                               ///< 4
    };

    struct CurrencyTypesEntry
    {
        uint32              ID;                                         ///< 0
        LocalizedString const* m_NameLang;                              ///< 1
        LocalizedString const* Description;                             ///< 2
        uint32              TotalCap;                                   ///< 3
        uint32              WeekCap;                                    ///< 4
        uint32              Flags;                                      ///< 5
        uint8               CategoryID;                                 ///< 6
        uint8               m_SpellCategory;                            ///< 7
        uint8               m_Quality;                                  ///< 8
        uint32 InventoryIconFileDataID;                                 ///< 9
        uint32              m_SpellWeight;                              ///< 10

        bool HasPrecision() const { return Flags & CURRENCY_FLAG_HIGH_PRECISION; }
        bool HasSeasonCount() const { return Flags & CURRENCY_FLAG_HAS_SEASON_COUNT; }
        float GetPrecision() const { return HasPrecision() ? 100.0f : 1.0f; }
    };

    struct CurveEntry
    {
        uint32 ID;        ///< 0
        uint8  Type;      ///< 1
        uint8  Flags;    ///< 2
    };

    struct CurvePointEntry
    {
        uint32              ID;                                         ///< 0
        float               X;                                          ///< 1-0
        float               Y;                                          ///< 1-1
        uint16              CurveID;                                    ///< 2
        uint8               Index;                                      ///< 3
    };

    struct DestructibleModelDataEntry
    {
        uint32              Id;                                         ///< 0
        uint16              DamagedDisplayId;                           ///< 1
        uint16              DestroyedDisplayId;                         ///< 2
        uint16              RebuildingDisplayId;                        ///< 3
        uint16              SmokeDisplayId;                             ///< 4
        uint16              HealEffectSpeed;                            ///< 5
        uint8               StateDamagedImpactEffectDoodadSet;          ///< 6
        uint8               StateDamagedAmbientDoodadSet;               ///< 7
        uint8               StateDamagedNameSet;                        ///< 8
        uint8               StateDestroyedDestructionDoodadSet;         ///< 9
        uint8               StateDestroyedImpactEffectDoodadSet;        ///< 10
        uint8               StateDestroyedAmbientDoodadSet;             ///< 11
        uint8               StateDestroyedNameSet;                      ///< 12
        uint8               StateRebuildingDestructionDoodadSet;        ///< 13
        uint8               StateRebuildingImpactEffectDoodadSet;       ///< 14
        uint8               StateRebuildingAmbientDoodadSet;            ///< 15
        uint8               StateRebuildingNameSet;                     ///< 16
        uint8               StateSmokeInitDoodadSet;                    ///< 17
        uint8               StateSmokeAmbientDoodadSet;                 ///< 18
        uint8               StateSmokeNameSet;                          ///< 19
        uint8               EjectDirection;                             ///< 20
        uint8               DoNotHighlight;                             ///< 21
        uint8               HealEffect;                                 ///< 22
    };

    struct DungeonEncounterEntry
    {
        LocalizedString const* NameLang;                                ///< 0
        uint32              CreatureDisplayID;                          ///< 1
        uint16              MapID;                                      ///< 2
        uint8               DifficultyID;                               ///< 3
        uint8               Bit;                                        ///< 4
        uint8               Flags;                                      ///< 5
        uint32              ID;                                         ///< 6
        int32               OrderIndex;                                 ///< 7
        uint32              TextureFileDataID;                          ///< 8
    };

    struct DurabilityCostsEntry
    {
        uint32 ID;
        uint16 WeaponSubClassCost[21];
        uint16 ArmorSubClassCost[8];
    };

    struct DurabilityQualityEntry
    {
        uint32              Id;                                         ///< 0        m_ID
        float               quality_mod;                                ///< 1        m_data
    };

    struct EmotesEntry
    {
        uint32              Id;                                         ///< 0
        uint64              RaceMask;                                   ///< 1
        LocalizedString const* EmoteSlashCommand;                       ///< 2
        uint32              SpellVisualKitID;                           ///< 3
        uint32              EmoteFlags;                                 ///< 4
        uint16              AnimID;                                     ///< 5
        uint8               EmoteSpecProc;                              ///< 6
        uint32              EmoteSpecProcParam;                         ///< 7
        uint32              EmoteSoundID;                               ///< 8
        uint32              ClassMask;                                  ///< 9
    };

    struct EmotesTextEntry
    {
        uint32              Id;                                         ///< 0        m_ID
        LocalizedString const* Name;                                    ///< 1        m_Name
        uint16              EmoteID;                                    ///< 2        m_EmoteID
    };

    struct EmotesTextSoundEntry
    {
        uint32              ID;                                         ///< 0
        uint8               RaceId;                                     ///< 1
        uint8               Gender;                                     ///< 2
        uint8               ClassId;                                    ///< 3
        uint32              SoundID;                                    ///< 4
        uint16              EmotesTextId;                               ///< 5
    };

    struct FactionEntry
    {
        uint64              ReputationRaceMask[4];                      ///< 0
        LocalizedString const* NameLang;                                ///< 1
        LocalizedString const* DescriptionLang;                         ///< 2
        uint32              ID;                                         ///< 3
        int32               ReputationBase[4];                          ///< 4
        float               ParentFactionMod[2];                        ///< 5
        uint32              ReputationMax[4];                           ///< 6
        int16               ReputationIndex;                            ///< 7
        uint16              ReputationClassMask[4];                     ///< 8
        uint16              ReputationFlags[4];                         ///< 9
        uint16              ParentFactionID;                            ///< 10
        uint16              ParagonFactionID;                           ///< 11
        uint8               ParentFactionCap[2];                        ///< 12
        uint8               Expansion;                                  ///< 13
        uint8               Flags;                                      ///< 14
        uint8               FriendshipRepID;                            ///< 15

        // helpers
        bool CanHaveReputation() const
        {
            return ReputationIndex >= 0;
        }
    };

    struct FactionTemplateEntry
    {
        uint32              ID;                                         ///< 0        m_ID
        uint16              Faction;                                    ///< 1        m_faction
        uint16              Flags;                                      ///< 2        m_flags
        uint16              Enemies[MAX_FACTION_RELATIONS];             ///< 3        m_enemies[MAX_FACTION_RELATIONS]
        uint16              Friend[MAX_FACTION_RELATIONS];              ///< 4        m_friend[MAX_FACTION_RELATIONS]
        uint8               FactionGroup;                               ///< 5        m_factionGroup
        uint8               FriendGroup;                                ///< 6        m_friendGroup
        uint8               EnemyGroup;                                 ///< 7        m_enemyGroup

        bool IsFriendlyTo(FactionTemplateEntry const& p_Entry) const
        {
            if (ID == p_Entry.ID)
                return true;

            if (p_Entry.Faction)
            {
                for (uint8 i = 0; i < MAX_FACTION_RELATIONS; ++i)
                {
                    if (Enemies[i] == p_Entry.Faction)
                        return false;
                }

                for (uint8 i = 0; i < MAX_FACTION_RELATIONS; ++i)
                {
                    if (Friend[i] == p_Entry.Faction)
                        return true;
                }
            }

            return (FriendGroup & p_Entry.FactionGroup) || (FactionGroup & p_Entry.FriendGroup);
        }

        bool IsHostileTo(FactionTemplateEntry const& p_Entry) const
        {
            /// Hackfix for Mage Tower challenge: End of the Risen Threat
            if (ID != 2897 && ID == p_Entry.ID)
                return false;

            if (p_Entry.Faction)
            {
                for (uint8 i = 0; i < MAX_FACTION_RELATIONS; ++i)
                {
                    if (Enemies[i] == p_Entry.Faction)
                        return true;
                }

                for (uint8 i = 0; i < MAX_FACTION_RELATIONS; ++i)
                {
                    if (Friend[i] == p_Entry.Faction)
                        return false;
                }
            }

            return (EnemyGroup & p_Entry.FactionGroup) != 0;
        }

        bool IsHostileToPlayers() const { return (EnemyGroup & FACTION_MASK_PLAYER); }

        bool IsNeutralToAll() const
        {
            for (uint8 i = 0; i < MAX_FACTION_RELATIONS; ++i)
            {
                if (Enemies[i] != 0)
                    return false;
            }

            return EnemyGroup == 0 && FriendGroup == 0;
        }

        bool IsContestedGuardFaction() const { return (Flags & FACTION_TEMPLATE_FLAG_CONTESTED_GUARD); }
    };

    struct GameObjectDisplayInfoEntry
    {
        uint32              Displayid;                                  ///< 0      m_ID
        uint32              m_FileDataID;                               ///< 1      m_FileDataID
        float               minX;                                       ///< 2-0    m_GeoBoxMin
        float               minY;                                       ///< 2-1    m_GeoBoxMin
        float               minZ;                                       ///< 2-2    m_GeoBoxMin
        float               maxX;                                       ///< 2-3    m_GeoBoxMax
        float               maxY;                                       ///< 2-4    m_GeoBoxMax
        float               maxZ;                                       ///< 2-5    m_GeoBoxMax
        float               m_OverrideLootEffectScale;                  ///< 3      m_OverrideLootEffectScale
        float               m_OverrideNameScale;                        ///< 4      m_OverrideNameScale
        uint16              m_ObjectEffectPackageID;                    ///< 5      m_ObjectEffectPackageID
    };

    struct GemPropertiesEntry
    {
        uint32              ID;                                         ///< 0        m_id
        uint32              Type;                                       ///< 1        m_type
        uint16              EnchantID;                                  ///< 2        m_EnchantId
        uint16              MinItemLevel;                               ///< 3        m_requiredItemLevel
    };

    struct GlobalStringsEntry
    {
        uint32                 ID;
        LocalizedString const* BaseTag;
        LocalizedString const* TagText;
        uint8                  Flags;
    };

    struct GlyphBindableSpellEntry
    {
        uint32 ID;                                                      ///< 0      ID
        uint32 SpellID;                                                 ///< 1      SpellID
        uint16 GlyphPropertiesID;                                       ///< 2      GlyphPropID
    };

    struct GlyphPropertiesEntry
    {
        uint32              Id;                                         ///< 0        m_ID
        uint32              SpellID;                                    ///< 1        m_SpellID
        uint16              m_SpellIconID;                              ///< 3        m_SpellIconID
        uint8               TypeFlags;                                  ///< 2        m_GlyphType
        uint8               GlyphExclusiveCategoryID;                   ///< 4        m_GlyphExclusiveCategoryID
    };

    struct GlyphRequiredSpecEntry
    {
        uint32              ID;                                         ///< 0
        uint16              SpecID;                                     ///< 1
        uint16              GlyphID;                                    ///< 2
    };

    struct GroupFinderActivityEntry
    {
        uint32              ID;                                         ///< 0
        LocalizedString const* Name;                                    ///< 1
        LocalizedString const* DifficultyString;                        ///< 2
        int16               RequiredILvl;                               ///< 3
        int16               MapID;                                      ///< 4
        int16               ZoneID;                                     ///< 5
        int8                CategoryID;                                 ///< 6
        uint8               ActivityGroupID;                            ///< 7
        int8                OrderIndex;                                 ///< 8
        int8                MinLevel;                                   ///< 9
        int8                MaxLevel;                                   ///< 10
        int8                Difficulty;                                 ///< 11
        int8                Type;                                       ///< 12
        int8                Flags;                                      ///< 13
        int8                MaxPlayers;                                 ///< 14
    };

    struct GroupFinderCategoryEntry
    {
        uint32              ID;                                         ///< 0
        LocalizedString const* Name;                                    ///< 1
        int8                Flags;                                      ///< 2
        int8                OrderIndex;                                 ///< 3
    };

    struct GuildPerkSpellsEntry
    {
        uint32              Id;                                         ///< 0      m_ID
        uint32              SpellId;                                    ///< 1      m_SpellID
    };

    struct HeirloomEntry
    {
        LocalizedString const* Description;                                 ///< 0
        uint32              ItemID;                                         ///< 1
        uint32              OldHeirloomID[2];                               ///< 2
        uint32              HeroicVersion;                                  ///< 3
        uint32              UpgradableByItemID[MAX_HEIRLOOM_UPGRADE_LEVEL]; ///< 4
        uint16              UpgradeIemBonusID[MAX_HEIRLOOM_UPGRADE_LEVEL];  ///< 5
        uint8               Flags;                                          ///< 6
        uint8               Source;                                         ///< 7
        uint32              ID;                                             ///< 8
    };

    struct HolidaysEntry
    {
        uint32              Id;                                         ///< 0
        uint32              Date[MAX_HOLIDAY_DATES];                    ///< 1 dates in unix time starting at January, 1, 2000
        uint16              Duration[MAX_HOLIDAY_DURATIONS];            ///< 2
        uint16              Region;                                     ///< 3
        uint8               Looping;                                    ///< 4
        uint8               CalendarFlags[MAX_HOLIDAY_FLAGS];           ///< 5
        uint8               Priority;                                   ///< 6
        int8                CalendarFilterType;                         ///< 7
        uint8               flags;                                      ///< 8
        uint32              holidayNameId;                              ///< 9
        uint32              holidayDescriptionId;                       ///< 10
        int32               TextureFileDataID[3];                       ///< 11
    };

    struct JournalInstanceEntry
    {
        LocalizedString const* Name;                    ///< 4
        LocalizedString const* Description;             ///< 5
        uint32                 ButtonFileDataId;        ///< 0
        uint32                 ButtonSmallFileDataId;   ///< 1
        uint32                 BackgroundFileDataId;    ///< 2
        uint32                 LoreFileDataId;          ///< 3
        uint16                 MapID;                   ///< 6
        uint16                 AreaId;                  ///< 7
        uint8                  OrderIndex;              ///< 8
        uint8                  Flags;                   ///< 9
        uint32                 ID;                      ///< 10
    };

    struct JournalEncounterEntry
    {
        uint32                 ID;                      ///< 0
        LocalizedString const* Name;                    ///< 3
        LocalizedString const* Description;             ///< 4
        float                  Map[2];                  ///< 1-2
        uint16                 DungeonMapId;            ///< 5
        uint16                 WorldMapAreaId;          ///< 6
        uint16                 FirstSectionId;          ///< 7
        uint16                 JournalInstanceID;       ///< 8
        uint8                  DifficultyMask;          ///< 9
        uint8                  Flags;                   ///< 10
        uint32                 OrderIndex;              ///< 11
        uint32                 MapDisplayConditionId;   ///< 12
    };

    struct JournalEncounterItemEntry
    {
        uint32 ItemID;                                  ///< 0
        uint16 JournalEncounterID;                      ///< 1
        uint8  DifficultyMask;                          ///< 2
        uint8  FactionMask;                             ///< 3
        uint8  Flags;                                   ///< 4
        uint32 ID;                                      ///< 5
    };

    struct LFGDungeonEntry
    {
        uint32              ID;                                         ///< 0
        LocalizedString const* Name;                                    ///< 1
        LocalizedString const* Description;                             ///< 3
        uint32              Flags;                                      ///< 2
        float               MinItemLevel;                               ///< 4
        uint16              maxlevel;                                   ///< 5
        uint16              recmaxlevel;                                ///< 6
        int16               map;                                        ///< 7
        uint16              RandomID;                                   ///< 8
        uint16              ScenarioID;                                 ///< 9
        uint16              LastBossJournalEncounterID;                 ///< 10
        uint16              BonusReputationAmount;                      ///< 11
        uint16              MentorItemLevel;                            ///< 12
        uint16              PlayerConditionID;                          ///< 13
        uint8               minlevel;                                   ///< 14
        uint8               reclevel;                                   ///< 15
        uint8               recminlevel;                                ///< 16
        uint8               difficulty;                                 ///< 17
        uint8               type;                                       ///< 18
        int8                Faction;                                    ///< 19
        uint8               expansion;                                  ///< 20
        uint8               OrderIndex;                                 ///< 21
        uint8               grouptype;                                  ///< 22
        uint8               tankNeeded;                                 ///< 23
        uint8               healerNeeded;                               ///< 24
        uint8               dpsNeeded;                                  ///< 25
        uint8               MinCountTank;                               ///< 26
        uint8               MinCountHealer;                             ///< 27
        uint8               MinCountDamage;                             ///< 28
        uint8               category;                                   ///< 29
        uint8               MentorCharLevel;                            ///< 30
        int32               TextureFileDataID;                          ///< 31
        int32               RewardIconFileDataID;                       ///< 32
        int32               ProposalTextureFileDataID;                  ///< 33

        // Helpers
        uint32 Entry() const { return ID + (type << 24); }
        // 1 = LFG_TYPE_DUNGEON
        bool isScenario() const { return type == 1 && tankNeeded == 0 && healerNeeded == 0 && dpsNeeded == 3; }
        bool isScenarioSingle() const { return type == 1 && tankNeeded == 0 && healerNeeded == 0 && dpsNeeded == 1; }

        bool FitsTeam(uint32 team) const
        {
            if (Faction == -1)
                return true;
            else if (Faction == 0)
                return team == HORDE;
            else
                return team == ALLIANCE;
        }
    };

    struct LocationEntry
    {
        uint32              ID;                                                     ///< 0
        float               X;                                                      ///< 1-0
        float               Y;                                                      ///< 1-1
        float               Z;                                                      ///< 1-2
        float               Unk1;                                                   ///< 2-0
        float               Unk2;                                                   ///< 2-1
        float               Unk3;                                                   ///< 2-2
    };

    struct LockEntry
    {
        uint32              ID;                                                     ///< 0    m_ID
        uint32              Index[MAX_LOCK_CASE];                                   ///< 1    m_Index
        uint16              Skill[MAX_LOCK_CASE];                                   ///< 2    m_Skill
        uint8               Type[MAX_LOCK_CASE];                                    ///< 3    m_Type
        uint8               Action[MAX_LOCK_CASE];                                  ///< 4    m_Action
    };

    struct MailTemplateEntry
    {
        uint32              ID;                                                     ///< 0      m_ID
        LocalizedString const* content;                                             ///< 1      m_body_lang
    };

    struct MapChallengeModeEntry
    {
        LocalizedString const*  Name;                                               ///< 0
        uint32              ID;                                                     ///< 1
        uint16              MapID;                                                  ///< 2
        uint16              ChestTimer[3];                                          ///< 3
        uint8               Flags;                                                  ///< 4
    };

    struct MinorTalentEntry
    {
        uint32              ID;                                                     ///< 0
        uint32              spellID;                                                ///< 1
        uint32              orderIndex;                                             ///< 2
        uint32              specializationID;                                       ///< 3
    };

    struct MountCapabilityEntry
    {
        uint32              RequiredSpell;                                          ///< 0      m_ReqSpellKnownID
        uint32              SpeedModSpell;                                          ///< 1      m_ModSpellAuraID
        uint16              RequiredRidingSkill;                                    ///< 2      m_ReqRidingSkill
        uint16              RequiredArea;                                           ///< 3      m_ReqAreaID
        int16               RequiredMap;                                            ///< 4      m_ReqMapID
        uint8               Flags;                                                  ///< 5      m_Flags
        uint32              Id;                                                     ///< 6      m_ID
        uint32              RequiredAura;                                           ///< 7      m_ReqSpellAuraID
    };

    struct MountEntry
    {
        LocalizedString const* Name;                                                ///< 0
        LocalizedString const* Description;                                         ///< 1
        LocalizedString const* SourceDescription;                                   ///< 2
        uint32              SpellID;                                                ///< 3
        float               CameraPivotMultiplier;                                  ///< 4
        uint16              MountType;                                              ///< 5
        uint16              Flags;                                                  ///< 6
        uint8               FilterCategory;                                         ///< 7
        uint32              Id;                                                     ///< 8
        uint32              PlayerConditionID;                                      ///< 9
        uint32              UiModelSceneID;                                         ///< 10
    };

    struct MountTypeXCapabilityEntry
    {
        uint32              ID;                                                     ///< 0
        uint16              MountTypeID;                                            ///< 1
        uint16              CapabilityID;                                           ///< 2
        uint8               Index;                                                  ///< 3
    };

    struct MountXDisplayEntry
    {
        uint32 ID;                                                                  ///< 0
        uint32 DisplayID;                                                           ///< 2
        uint32 PlayerConditionID;                                                   ///< 3
        uint32 MountID;                                                             ///< 1
    };

    struct MovieEntry
    {
        uint32              Id;                                                     ///< 0        m_ID
        uint32              AudioFileDataID;                                        ///< 1        m_AudioFileDataID
        uint32              m_SubtitleFileDataID;                                   ///< 2        m_SubtitleFileDataID
        uint8               Volume;                                                 ///< 3        m_Volume
        uint8               KeyID;                                                  ///< 4        m_KeyID
    };

    struct NameGenEntry
    {
        uint32              id;                                                     ///< 0      m_ID
        LocalizedString const* name;                                                ///< 1      m_Name
        uint8               race;                                                   ///< 2      m_RaceID
        uint8               gender;                                                 ///< 3      m_Sex
    };

    struct ParagonReputationEntry
    {
        uint32              ID;                                                     ///< 0 m_ID
        uint32              LevelThreshold;                                         ///< 1 m_NeededRep
        uint32              QuestID;                                                ///< 2 m_QuestID
        uint32              FactionID;                                              ///< 3 m_FactionID
        uint32              ParagonID;                                              ///< 4 m_ParagonID
    };

    struct PathNodeEntry
    {
        uint32              ID;                                                     ///< 0
        uint32              LocationID;                                             ///< 1
        uint16              PathID;                                                 ///< 2
        uint16              Order;                                                  ///< 3
    };

    struct PlayerConditionEntry
    {
        uint64              RaceMask;                                               ///< 0
        LocalizedString const* FailureDescription;                                  ///< 1
        uint32              ID;                                                     ///< 2
        uint8               Flags;                                                  ///< 3
        uint16              MinLevel;                                               ///< 4
        uint16              MaxLevel;                                               ///< 5
        uint32              ClassMask;                                              ///< 6
        int8                Gender;                                                 ///< 7
        int8                NativeGender;                                           ///< 8
        uint32              SkillLogic;                                             ///< 9
        uint8               LanguageID;                                             ///< 10
        uint8               MinLanguage;                                            ///< 11
        uint32              MaxLanguage;                                            ///< 12
        uint16              MaxFactionID;                                           ///< 13
        uint8               MaxReputation;                                          ///< 14
        uint32              ReputationLogic;                                        ///< 15
        uint8               CurrentPvpFaction;                                      ///< 16
        uint8               MinPVPRank;                                             ///< 17
        uint8               MaxPVPRank;                                             ///< 18
        uint8               PvpMedal;                                               ///< 19
        uint32              PrevQuestLogic;                                         ///< 20
        uint32              CurrQuestLogic;                                         ///< 21
        uint32              CurrentCompletedQuestLogic;                             ///< 22
        uint32              SpellLogic;                                             ///< 23
        uint32              ItemLogic;                                              ///< 24
        uint8               ItemFlags;                                              ///< 25
        uint32              AuraSpellLogic;                                         ///< 26
        uint16              WorldStateExpressionID;                                 ///< 27
        uint8               WeatherID;                                              ///< 28
        uint8               PartyStatus;                                            ///< 29
        uint8               LifetimeMaxPVPRank;                                     ///< 30
        uint32              AchievementLogic;                                       ///< 31
        uint32              LfgLogic;                                               ///< 32
        uint32              AreaLogic;                                              ///< 33
        uint32              CurrencyLogic;                                          ///< 34
        uint16              QuestKillID;                                            ///< 35
        uint32              QuestKillLogic;                                         ///< 36
        int8                MinExpansionLevel;                                      ///< 37
        int8                MaxExpansionLevel;                                      ///< 38
        int8                MinExpansionTier;                                       ///< 39
        int8                MaxExpansionTier;                                       ///< 40
        uint8               MinGuildLevel;                                          ///< 41
        uint8               MaxGuildLevel;                                          ///< 42
        uint8               PhaseUseFlags;                                          ///< 43
        uint16              PhaseID;                                                ///< 44
        uint32              PhaseGroupID;                                           ///< 45
        uint32              MinAvgItemLevel;                                        ///< 46
        uint32              MaxAvgItemLevel;                                        ///< 47
        uint16              MinAvgEquippedItemLevel;                                ///< 48
        uint16              MaxAvgEquippedItemLevel;                                ///< 49
        int8                ChrSpecializationIndex;                                 ///< 50
        int8                ChrSpecializationRole;                                  ///< 51
        int8                PowerType;                                              ///< 52
        int8                PowerTypeComp;                                          ///< 53
        int8                PowerTypeValue;                                         ///< 54
        uint32              ModifierTreeID;                                         ///< 55
        uint32              MainHandItemSubclassMask;                               ///< 56
        uint16              SkillID[4];                                             ///< 57 - 60
        uint16              MinSkill[4];                                            ///< 61 - 64
        uint16              MaxSkill[4];                                            ///< 65 - 68
        uint32              MinFactionID[3];                                        ///< 69 - 71
        uint8               MinReputation[3];                                       ///< 72 - 74
        uint16              PrevQuestID[4];                                         ///< 75 - 78
        uint16              CurrQuestID[4];                                         ///< 79 - 82
        uint16              CurrentCompletedQuestID[4];                             ///< 83 - 86
        uint32              SpellID[4];                                             ///< 87 - 90
        uint32              ItemID[4];                                              ///< 91 - 94
        uint32              ItemCount[4];                                           ///< 95 - 98
        uint16              Explored[2];                                            ///< 99 - 100
        uint32              Time[2];                                                ///< 101 - 102
        uint32              AuraSpellID[4];                                         ///< 103 - 106
        uint8               AuraCount[4];                                           ///< 107 - 110
        uint16              Achievement[4];                                         ///< 111 - 114
        uint8               LfgStatus[4];                                           ///< 115 - 118
        uint8               LfgCompare[4];                                          ///< 119 - 122
        uint32              LfgValue[4];                                            ///< 123 - 126
        uint16              AreaID[4];                                              ///< 127 - 130
        uint32              CurrencyID[4];                                          ///< 131 - 134
        uint32              CurrencyCount[4];                                       ///< 135 - 138
        uint32              QuestKillMonster[6];                                    ///< 139 - 145
        uint32              MovementFlags[2];                                       ///< 146 - 147
    };

    struct PowerDisplayEntry
    {
        uint32              ID;                                                     ///< 0
        LocalizedString const*  GlobalStringBaseTag;                                ///< 1
        uint8               PowerType;                                              ///< 2
        uint8               Red;                                                    ///< 3
        uint8               Green;                                                  ///< 4
        uint8               Blue;                                                   ///< 5
    };

    struct PowerTypeEntry
    {
        uint32              m_DBIndex;                                              ///< 0 PowerTypeToken
        LocalizedString const* m_Name;                                              ///< 1
        LocalizedString const* PowerCostToken;                                      ///< 2
        float               RegenerationPeace;                                      ///< 3
        float               RegenerationCombat;                                     ///< 4  Only some fields, duplicates of RegenValue
        uint16              MaxPower;                                               ///< 5
        uint16              RegenerationDelay;                                      ///< 6 RegenerationDelay
        uint16              m_Flags;                                                ///< 7
        uint8               m_ID;                                                   ///< 8 PowerTypeEnum
        uint8               RegenerationMin;                                        ///< 9
        uint8               RegenerationCenter;                                     ///< 10
        uint8               RegenerationMax;                                        ///< 11
        uint8               UIModifier;                                             ///< 12
    };

    struct PrestigeLevelInfoEntry
    {
        uint32              ID;                                                     ///< 0
        LocalizedString const* Name;                                                ///< 1
        uint32              TextureID;                                              ///< 2
        uint8               PrestigeLevel;                                          ///< 3
        uint8               Flags;                                                  ///< 4
    };

    struct PvPDifficultyEntry
    {
        uint32              id;                                                     ///< 0        m_ID
        uint8               bracketId;                                              ///< 2        m_RangeIndex
        uint8               minLevel;                                               ///< 3        m_MinLevel
        uint8               maxLevel;                                               ///< 4        m_MaxLevel
        uint16              mapId;                                                  ///< 1        m_MapID

        uint32 GetBracketId() const { return bracketId; }
    };

    struct PvpTalentEntry
    {
        uint32              m_ID;                                                   ///< 0
        LocalizedString const*  l_Description;                                      ///< 1
        uint32              m_SpellID;                                              ///< 2
        uint32              m_OverridesSpellID;                                     ///< 3
        uint32              m_ExtraSpellID;                                         ///< 4
        uint32              m_TierIndex;                                            ///< 5
        uint32              m_CollumnIndex;                                         ///< 6
        uint32              m_Flags;                                                ///< 7
        uint32              m_Class;                                                ///< 8
        uint32              m_SpecID;                                               ///< 9
        uint32              m_Role;                                                 ///< 10
    };

    struct PvpTalentUnlockEntry
    {
        uint32              m_ID;                                                   ///< 0
        uint8               m_TierIndex;                                            ///< 1
        uint8               m_CollumnIndex;                                         ///< 2
        uint8               m_LevelRequirement;                                     ///< 3
    };

    struct PvpRewardEntry
    {
        uint32              ID;                                                   ///< 0
        uint32              HonorLevel;                                           ///< 1
        uint32              PrestigeLevel;                                        ///< 2
        uint32              RewardPackID;                                         ///< 3
    };

    struct RandomPropertiesPointsEntry
    {
        uint32              Id;                                                     ///< 0
        uint32              EpicPropertiesPoints[5];                                ///< 1
        uint32              RarePropertiesPoints[5];                                ///< 2
        uint32              UncommonPropertiesPoints[5];                            ///< 3
    };

    struct RewardPackEntry
    {
        uint32              ID;                                                     ///< 0
        uint32              MoneyReward;                                            ///< 1
        float               ArtifactXPMultiplier;                                   ///< 2
        uint8               ArtifactXPDifficulty;                                   ///< 3
        uint8               ArtifactCategoryID;                                     ///< 4
        uint32              TitleID;                                                ///< 5
        uint32              TreasurePickerId;                                       ///< 6
    };

    struct RewardPackXCurrencyTypeEntry
    {
        uint32              m_ID;                                                   ///< 0
        uint32              m_CurrencyID;                                           ///< 1
        uint32              m_Count;                                                ///< 2
        uint32              m_RewardPackID;                                         ///< 3
    };

    struct RewardPackXItemEntry
    {
        uint32              ID;                                                     ///< 0
        uint32              ItemID;                                                 ///< 1
        uint32               Amount;                                                 ///< 2
        uint32              RewardPackID;                                           ///< 3
    };

    struct ScalingStatDistributionEntry
    {
        uint32              Id;                                                     ///< 0      m_ID
        uint16              CurveProperties;                                        ///< 1      m_CurveProperties
        uint32              MinLevel;                                               ///< 2      m_Minlevel
        uint32              MaxLevel;                                               ///< 3      m_Maxlevel
    };

    struct SandboxScalingEntry
    {
        uint32              ID;
        uint32              MinLevel;
        uint32              MaxLevel;
        uint32              Flags;
    };

    struct ScenarioEntry
    {
        uint32              ID;                                                     ///< 0
        LocalizedString const* NameLang;                                            ///< 1
        uint16              Data;                                                   ///< 2
        uint8               Flags;                                                  ///< 3
        uint8               Type;                                                   ///< 4
    };

    struct ScenarioStepEntry
    {
        uint32              ID;                                                     ///< 0
        LocalizedString const* DescriptionLang;                                     ///< 1
        LocalizedString const* TitleLang;                                           ///< 2
        uint16              ScenarioID;                                             ///< 3
        uint16              PreviousStepID;                                         ///< 4
        uint16              RewardQuestID;                                          ///< 5
        uint8               Step;                                                   ///< 6
        uint8               Flags;                                                  ///< 7
        uint32              CriteriaTreeID;                                         ///< 8
        uint32              BonusRequiredStepID;                                    ///< 9

        bool IsBonusObjective() const { return Flags & SCENARIO_STEP_FLAG_BONUS_OBJECTIVE; }
    };

    struct SkillLineAbilityEntry
    {
        uint64              RaceMask;                                               ///< 0        m_raceMask
        uint32              id;                                                     ///< 1        m_ID
        uint32              spellId;                                                ///< 2        m_spell
        uint32              SupercedesSpell;                                        ///< 3        m_supercededBySpell
        uint16              SkillLine;                                              ///< 4        m_skillLine
        uint16              TrivialSkillLineRankHigh;                               ///< 5        m_trivialSkillLineRankHigh
        uint16              TrivialSkillLineRankLow;                                ///< 6        m_trivialSkillLineRankLow
        uint16              UniqueBit;                                              ///< 7        m_UniqueBit
        uint16              TradeSkillCategoryID;                                   ///< 8        m_TradeSkillCategoryI
        uint8               NumSkillUps;                                            ///< 9        m_NumSkillUps
        uint32              ClassMask;                                              ///< 10       m_classMask
        uint16              MinSkillLineRank;                                       ///< 11       m_minSkillLineRank
        uint8               Flags;                                                  ///< 12       21531
        uint8               SkillupSkillLineId;                                     ///< 13       21531
    };

    struct SkillRaceClassInfoEntry
    {
        uint32              ID;                                                     ///< 0
        int64               RaceMask;                                               ///< 1
        uint16              SkillID;                                                ///< 2
        uint16              Flags;                                                  ///< 3
        uint16              SkillTierID;                                            ///< 4
        uint8               Availability;                                           ///< 5
        uint8               MinLevel;                                               ///< 6
        int32               ClassMask;                                              ///< 7
    };

    struct SkillLineEntry
    {
        uint32              id;                                                     ///< 0        m_ID
        LocalizedString const* DisplayName;                                         ///< 1        m_displayName_lang
        LocalizedString const* Description;                                         ///< 2        m_description_lang
        LocalizedString const* AlternateVerb;                                       ///< 3        m_alternateVerb_lang
        uint16              Flags;                                                  ///< 4        m_Flags
        int8                CategoryID;                                             ///< 5        m_categoryID
        uint8               CanLink;                                                ///< 6        m_canLink (prof. with recipes)
        uint32              IconFileDataID;                                         ///< 7        m_spellIconID
        uint32              ParentSkillLineID;                                      ///< 8        m_parentSkillLineID
    };

    struct SoundKitEntry
    {
        uint32              ID;                                                     ///< 0
        float               VolumeFloat;                                            ///< 1
        float               MinDistance;                                            ///< 2
        float               DistanceCutoff;                                         ///< 3
        uint16              Flags;                                                  ///< 4
        uint16              SoundEntriesAdvancedID;                                 ///< 5
        uint8               SoundType;                                              ///< 6
        uint8               DialogType;                                             ///< 7
        uint8               EAXDef;                                                 ///< 8
        float               VolumeVariationPlus;                                    ///< 9
        float               VolumeVariationMinus;                                   ///< 10
        float               PitchVariationPlus;                                     ///< 11
        float               PitchVariationMinus;                                    ///< 12
        float               PitchAdjust;                                            ///< 13
        uint16              BusOverwriteID;                                         ///< 14
        uint8               MaxInstances;                                           ///< 15
    };

    struct SummonPropertiesEntry
    {
        uint32              Id;                                                     ///< 0        m_ID
        uint32              Flags;                                                  ///< 1        m_Flags
        uint32              Category;                                               ///< 2        m_Control
        uint32              Faction;                                                ///< 3        m_Faction
        uint32              Type;                                                   ///< 4        m_Title
        int32               Slot;                                                   ///< 5        m_Slot
    };

    struct TactKeyEntry
    {
        uint32              ID;                                                     ///< 0
        uint8               Key[16];                                                ///< 1
    };

    struct TalentEntry
    {
        uint32              Id;                                                     ///< 0
        LocalizedString const* DescriptionLang;                                     ///< 1
        uint32              SpellID;                                                ///< 2
        uint32              OverridesSpellID;                                       ///< 3
        uint16              SpecID;                                                 ///< 4
        uint8               TierID;                                                 ///< 5
        uint8               ColumnIndex;                                            ///< 6
        uint8               Flags;                                                  ///< 7
        uint8               CategoryMask[2];                                        ///< 8
        uint8               ClassID;                                                ///< 9
    };

    struct TotemCategoryEntry
    {
        uint32              ID;                                                     ///< 0      m_ID
        LocalizedString const* name;                                                ///< 1      m_name_lang
        uint32              categoryMask;                                           ///< 3      m_totemCategoryMask     (compatibility mask for same type: different for totems, compatible from high to low for rods)
        uint8               categoryType;                                           ///< 2      m_totemCategoryType     (one for specialization)
    };

    struct TransportAnimationEntry
    {
        uint32              Id;                                                     ///< 0
        uint32              TimeIndex;                                              ///< 1
        float               X;                                                      ///< 2
        float               Y;                                                      ///< 3
        float               Z;                                                      ///< 4
        uint8               SequenceID;                                             ///< 5
        uint32              TransportID;                                            ///< 6
    };

    struct TransportRotationEntry
    {
        uint32              Id;                                                     ///< 0
        uint32              TimeSeg;                                                ///< 1
        float               X;                                                      ///< 2
        float               Y;                                                      ///< 3
        float               Z;                                                      ///< 4
        float               W;                                                      ///< 5
        uint32              TransportEntry;                                         ///< 6
    };

    struct ToyEntry
    {
        LocalizedString const*      SourceText;
        int32                       ItemId;
        uint8                       Flags;
        uint8                       SourceTypeEnum;
        uint32                      ID;
    };

    struct VehicleEntry
    {
        uint32              ID;                                                     ///< 0
        uint32              Flags;                                                  ///< 1
        float               TurnSpeed;                                              ///< 2
        float               PitchSpeed;                                             ///< 3
        float               PitchMin;                                               ///< 4
        float               PitchMax;                                               ///< 5
        float               MouseLookOffsetPitch;                                   ///< 6
        float               CameraFadeDistScalarMin;                                ///< 7
        float               CameraFadeDistScalarMax;                                ///< 8
        float               CameraPitchOffset;                                      ///< 9
        float               FacingLimitRight;                                       ///< 10
        float               FacingLimitLeft;                                        ///< 11
        float               CameraYawOffset;                                        ///< 12
        uint16              SeatID[MAX_VEHICLE_SEATS];                              ///< 13
        uint16              VehicleUIIndicatorID;                                   ///< 14
        uint16              PowerDisplayID[3];                                      ///< 15
        uint8               FlagsB;                                                 ///< 16
        uint8               UILocomotionType;                                       ///< 17
        int32               MissileTargetingID;                                     ///< 18
    };

    struct VehicleSeatEntry
    {
        uint32              ID;                                                     ///< 0
        uint32              m_flags;                                                ///< 1
        uint32              m_flagsB;                                               ///< 2
        uint32              m_flagsC;                                               ///< 3
        float               AttachmentOffsetX;                                      ///< 4
        float               AttachmentOffsetY;                                      ///< 5
        float               AttachmentOffsetZ;                                      ///< 6
        float               EnterPreDelay;                                          ///< 7
        float               EnterSpeed;                                             ///< 8
        float               EnterGravity;                                           ///< 9
        float               EnterMinDuration;                                       ///< 10
        float               EnterMaxDuration;                                       ///< 11
        float               EnterMinArcHeight;                                      ///< 12
        float               EnterMaxArcHeight;                                      ///< 13
        float               ExitPreDelay;                                           ///< 14
        float               ExitSpeed;                                              ///< 15
        float               ExitGravity;                                            ///< 16
        float               ExitMinDuration;                                        ///< 17
        float               ExitMaxDuration;                                        ///< 18
        float               ExitMinArcHeight;                                       ///< 19
        float               ExitMaxArcHeight;                                       ///< 20
        float               PassengerYaw;                                           ///< 21
        float               PassengerPitch;                                         ///< 22
        float               PassengerRoll;                                          ///< 23
        float               VehicleEnterAnimDelay;                                  ///< 24
        float               VehicleExitAnimDelay;                                   ///< 25
        float               CameraEnteringDelay;                                    ///< 26
        float               CameraEnteringDuration;                                 ///< 27
        float               CameraExitingDelay;                                     ///< 28
        float               CameraExitingDuration;                                  ///< 29
        float               CameraOffsetX;                                          ///< 30
        float               CameraOffsetY;                                          ///< 31
        float               CameraOffsetZ;                                          ///< 32
        float               CameraPosChaseRate;                                     ///< 33
        float               CameraFacingChaseRate;                                  ///< 34
        float               CameraEnteringZoom;                                     ///< 35
        float               CameraSeatZoomMin;                                      ///< 36
        float               CameraSeatZoomMax;                                      ///< 37
        uint32              UISkinFileDataID;                                       ///< 38
        int16               EnterAnimStart;                                         ///< 39
        int16               EnterAnimLoop;                                          ///< 40
        int16               RideAnimStart;                                          ///< 41
        int16               RideAnimLoop;                                           ///< 42
        int16               RideUpperAnimStart;                                     ///< 43
        int16               RideUpperAnimLoop;                                      ///< 44
        int16               ExitAnimStart;                                          ///< 45
        int16               ExitAnimLoop;                                           ///< 46
        int16               ExitAnimEnd;                                            ///< 47
        int16               VehicleEnterAnim;                                       ///< 48
        int16               VehicleExitAnim;                                        ///< 49
        int16               VehicleRideAnimLoop;                                    ///< 50
        uint16              EnterAnimKitID;                                         ///< 51
        uint16              RideAnimKitID;                                          ///< 52
        uint16              ExitAnimKitID;                                          ///< 53
        uint16              VehicleEnterAnimKitID;                                  ///< 54
        uint16              VehicleRideAnimKitID;                                   ///< 55
        uint16              VehicleExitAnimKitID;                                   ///< 56
        uint16              CameraModeID;                                           ///< 57
        int8                AttachmentID;                                           ///< 58
        int8                PassengerAttachmentID;                                  ///< 59
        int8                VehicleEnterAnimBone;                                   ///< 60
        int8                VehicleExitAnimBone;                                    ///< 61
        int8                VehicleRideAnimLoopBone;                                ///< 62
        uint8               VehicleAbilityDisplay;                                  ///< 63
        uint32              EnterUISoundID;                                         ///< 64
        uint32              ExitUISoundID;                                          ///< 65

        bool CanEnterOrExit() const { return m_flags & VEHICLE_SEAT_FLAG_CAN_ENTER_OR_EXIT; }
        bool CanSwitchFromSeat() const { return m_flags & VEHICLE_SEAT_FLAG_CAN_SWITCH; }
        bool IsUsableByOverride() const {
            return (m_flags & VEHICLE_SEAT_FLAG_UNCONTROLLED)
                || (m_flagsB & (VEHICLE_SEAT_FLAG_B_USABLE_FORCED
                    | VEHICLE_SEAT_FLAG_B_USABLE_FORCED_2
                    | VEHICLE_SEAT_FLAG_B_USABLE_FORCED_3 | VEHICLE_SEAT_FLAG_B_USABLE_FORCED_4));
        }
        bool IsEjectable() const { return m_flagsB & VEHICLE_SEAT_FLAG_B_EJECTABLE; }
    };

    struct VignetteEntry
    {
        uint32              Id;                                                      ///< 0
        LocalizedString const* Name;                                                 ///< 1
        float               X;                                                       ///< 2
        float               Y;                                                       ///< 3
        uint32              Flags;                                                   ///< 5
        uint32              QuestFeedbackEffectId;                                   ///< 4
        uint32              PlayerConditionID;                                       ///< 6
        uint32              QuestID;                                                 ///< 7
    };

    struct WorldSafeLocsEntry
    {
        WorldSafeLocsEntry()
            : ID(0),
            x(0),
            y(0),
            z(0),
            o(0),
            MapID(0)
        {

        }

        uint32              ID;                                                     ///< 0
        LocalizedString const* name;                                                ///< 1 m_AreaName_lang
        float               x;                                                      ///< 2
        float               y;                                                      ///< 3
        float               z;                                                      ///< 4
        float               o;                                                      ///< 5
        uint16              MapID;                                                  ///< 6
    };

    struct AreaPOIEntry
    {
        int32                   ID;
        LocalizedString const*  Name;
        LocalizedString const*  Description;
        uint32                  Flags;
        float                   PositionX;
        float                   PositionY;
        float                   PositionZ;
        uint32                  PoiDataType;
        uint32                  PoiData;
        uint16                  MapID;
        uint16                  AreaId;
        uint16                  WorldStateId;
        uint16                  WmoGroupId;
        int8                    Importance;
        int8                    Icon;
        uint32                  PlayerConditionID;
        uint32                  PortLocId;
        int32                   UiTextureAtlasMemberId;
        int32                   MapFloor;
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// Contributions DB2
    ////////////////////////////////////////////////////////////////////////////////

    struct ContributionEntry
    {
        LocalizedString const*  Description;
        LocalizedString const*  Name;
        int32                   ID;
        int32                   ManagedWorldStateInputID;
        int32                   UiTextureAtlasMemberID[4];
        int32                   OrderIndex;
    };

    struct ManagedWorldStateEntry
    {
        int32                   CurrentStageWorldStateID;
        int32                   ProgressWorldStateID;
        int32                   UpTimeSecs;
        int32                   DownTimeSecs;
        int32                   OccurencesWorldStateID;
        int32                   AccumulationStateTargetValue;
        int32                   DepletionStateTargetValue;
        int32                   AccumulationAmountPerMinute;
        int32                   DepletionAmountPerMinute;
        int32                   ID;
    };

    struct ManagedWorldStateBuffEntry
    {
        int32                   ID;
        int32                   OccurenceValue;
        int32                   BuffSpellID;
        int32                   PlayerConditionID;
        int32                   ManagedWorldStateID;
    };

    struct ManagedWorldStateInputEntry
    {
        int32                   ID;
        int32                   ManagedWorldStateID;
        int32                   QuestID;
        int32                   ValidInputConditionID;
    };
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// WorldState DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region WorldState
    struct WorldStateExpressionEntry
    {
        uint32              ID;                                                     ///< 0
        LocalizedString const*  Expression;                                         ///< 1

                                                                /// Eval a worldstate expression
        bool Eval(Player* p_Player, std::vector<std::string> * p_OutStrResult = nullptr) const;
        uint32 GetRequiredWorldStatesIfExists(std::vector<std::pair<uint32, uint32>>* p_WorldStates = nullptr) const;
    };
#pragma endregion

/// GCC has alternative #pragma pack(N) syntax and old gcc version does not support pack(push, N), also any gcc version does not support it at some platform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

#endif