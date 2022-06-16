//////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//////////////////////////////////////////////////////////////////////////
/// Spell DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Spell
    const char OverrideSpellDataEntryfmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_ARR_10, FT_INT,                              ///< spellId[MAX_OVERRIDE_SPELL]
        FT_INT,                                         ///< Flags
        FT_BYTE,                                        ///< PlayerActionbarFileDataID
        FT_END
    };

    const char SpecializationSpellEntryfmt[] =
    {
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< LearnSpell
        FT_INT,                                         ///< OverrideSpell
        FT_SHORT,                                       ///< SpecializationEntry
        FT_BYTE,                                        ///< DifficultyID
        FT_INDEX,                                       ///< Id
        FT_END
    };

    const char SpellEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< SpellName
        FT_STRING,                                      ///< Rank
        FT_STRING,                                      ///< Description
        FT_STRING,                                      ///< ToolTip
        FT_END
    };

    const char SpellAuraOptionsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< procCharges
        FT_INT,                                         ///< procFlags
        FT_INT,                                         ///< InternalCooldown
        FT_SHORT,                                       ///< StackAmount
        FT_SHORT,                                       ///< ProcsPerMinuteEntry
        FT_BYTE,                                        ///< DifficultyID
        FT_BYTE,                                        ///< procChance
        FT_INT_RELATIONSHIP,                            ///< SpellID
        FT_END
    };

    const char SpellAuraRestrictionsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< casterAuraSpell
        FT_INT,                                         ///< targetAuraSpell
        FT_INT,                                         ///< excludeCasterAuraSpell
        FT_INT,                                         ///< excludeTargetAuraSpell
        FT_BYTE,                                        ///< DifficultyID
        FT_BYTE,                                        ///< CasterAuraState
        FT_BYTE,                                        ///< TargetAuraState
        FT_BYTE,                                        ///< ExcludeCasterAuraState
        FT_BYTE,                                        ///< ExcludeTargetAuraState
        FT_INT_RELATIONSHIP,                            ///< SpellID
        FT_END
    };

    const char SpellCastingRequirementsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SpellID
        FT_SHORT,                                       ///< MinFactionId
        FT_SHORT,                                       ///< AreaGroupId
        FT_SHORT,                                       ///< RequiresSpellFocus
        FT_BYTE,                                        ///< FacingCasterFlags
        FT_BYTE,                                        ///< MinReputation
        FT_BYTE,                                        ///< RequiredAuraVision
        FT_END
    };

    const char SpellCastTimeEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< CastTime
        FT_INT,                                         ///< MinCastTime
        FT_SHORT,                                       ///< CastTimePerLevel
        FT_END
    };

    const char SpellCategoriesEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_SHORT,                                       ///< Category
        FT_SHORT,                                       ///< StartRecoveryCategory
        FT_SHORT,                                       ///< ChargeCategory
        FT_BYTE,                                        ///< DifficultyID
        FT_BYTE,                                        ///< DmgClass
        FT_BYTE,                                        ///< Dispel
        FT_BYTE,                                        ///< Mechanic
        FT_BYTE,                                        ///< PreventionType
        FT_INT_RELATIONSHIP,                            ///< SpellID
        FT_END
    };

    const char SpellCategoryEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< ChargeRecoveryTime
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< UsesPerWeek
        FT_BYTE,                                        ///< MaxCharges
        FT_INT,                                         ///< ChargeCategoryType
        FT_END
    };

    const char SpellClassOptionsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SpellID
        FT_ARR_4, FT_INT,                               ///< SpellFamilyFlags
        FT_BYTE,                                        ///< SpellFamilyName
        FT_INT,                                         ///< modalNextSpell
        FT_END
    };

    const char SpellCooldownsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< CategoryRecoveryTime;
        FT_INT,                                         ///< RecoveryTime
        FT_INT,                                         ///< StartRecoveryTime
        FT_BYTE,                                        ///< DifficultyID
        FT_INT_RELATIONSHIP,                            ///< SpellID
        FT_END
    };

    const char SpellDurationEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< Duration
        FT_INT,                                         ///< MaxDuration
        FT_INT,                                         ///< DurationPerLevel
        FT_END
    };

    const char SpellEffectEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< Effect
        FT_INT,                                         ///< EffectBasePoints
        FT_INT,                                         ///< EffectIndex
        FT_INT,                                         ///< EffectApplyAuraName
        FT_INT,                                         ///< EffectDifficulty
        FT_FLOAT,                                       ///< EffectValueMultiplier
        FT_INT,                                         ///< EffectAmplitude
        FT_FLOAT,                                       ///< EffectBonusMultiplier
        FT_FLOAT,                                       ///< EffectDamageMultiplier
        FT_INT,                                         ///< EffectChainTarget
        FT_INT,                                         ///< EffectDieSides
        FT_INT,                                         ///< EffectItemType
        FT_INT,                                         ///< EffectMechanic
        FT_FLOAT,                                       ///< EffectPointsPerComboPoint
        FT_FLOAT,                                       ///< EffectRealPointsPerLevel
        FT_INT,                                         ///< EffectTriggerSpell
        FT_FLOAT,                                       ///< m_EffectPosFacing
        FT_INT,                                         ///< m_EffectAttributes
        FT_FLOAT,                                       ///< BonusCoefficientFromAP
        FT_FLOAT,                                       ///< m_PvpMultiplier
        FT_FLOAT,                                       ///< Coefficient
        FT_FLOAT,                                       ///< Variance
        FT_FLOAT,                                       ///< ResourceCoefficient
        FT_FLOAT,                                       ///< GroupSizeCoefficient
        FT_ARR_4, FT_INT,                               ///< EffectSpellClassMask
        FT_ARR_2, FT_INT,                               ///< EffectMiscValue & EffectMiscValueB
        FT_ARR_2, FT_INT,                               ///< EffectRadiusIndex & EffectRadiusMaxIndex
        FT_ARR_2, FT_INT,                               ///< EffectImplicitTargetA & EffectImplicitTargetB
        FT_INT_RELATIONSHIP,                            ///< EffectSpellId
        FT_END
    };

    const char SpellEquippedItemsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SpellID
        FT_INT,                                         ///< EquippedItemInventoryTypeMask
        FT_INT,                                         ///< EquippedItemSubClassMask
        FT_BYTE,                                        ///< EquippedItemClass
        FT_END
    };

    const char SpellFocusObjectEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_END
    };

    const char SpellInterruptsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_BYTE,                                        ///< DifficultyID
        FT_SHORT,                                       ///< InterruptFlags
        FT_ARR_2, FT_INT,                               ///< AuraInterruptFlags
        FT_ARR_2, FT_INT,                               ///< ChannelInterruptFlags
        FT_INT_RELATIONSHIP,                            ///< SpellID
        FT_END
    };

    const char SpellItemEnchantmentConditionEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_5, FT_INT,                               ///< Value[5]
        FT_ARR_5, FT_BYTE,                              ///< Color[5]
        FT_ARR_5, FT_BYTE,                              ///< LT_Operand[5]
        FT_ARR_5, FT_BYTE,                              ///< Comparator[5]
        FT_ARR_5, FT_BYTE,                              ///< CompareColor[5]
        FT_ARR_5, FT_BYTE,                              ///< Logic[5]
        FT_END
    };

    const char SpellItemEnchantmentEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< description
        FT_ARR_3, FT_INT,                               ///< spellid[MAX_ENCHANTMENT_SPELLS]
        FT_ARR_3, FT_FLOAT,                             ///< m_EffectScalingPoints[MAX_ENCHANTMENT_SPELLS];
        FT_INT,                                         ///< TransmogCost
        FT_INT,                                         ///< TextureFileDataID
        FT_ARR_3, FT_SHORT,                             ///< amount[MAX_ENCHANTMENT_SPELLS]
        FT_SHORT,                                       ///< itemVisualID
        FT_SHORT,                                       ///< slot
        FT_SHORT,                                       ///< requiredSkill
        FT_SHORT,                                       ///< requiredSkillValue
        FT_SHORT,                                       ///< m_ItemLevel
        FT_BYTE,                                        ///< charges
        FT_ARR_3, FT_BYTE,                              ///< type[MAX_ENCHANTMENT_SPELLS];
        FT_BYTE,                                        ///< ConditionID
        FT_BYTE,                                        ///< requiredLevel
        FT_BYTE,                                        ///< m_MaxLevel
        FT_BYTE,                                        ///< m_ScalingClass
        FT_BYTE,                                        ///< m_ScalingClassRestricted
        FT_INT,                                         ///< PlayerConditionID
        FT_END
    };

    const char SpellLevelsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_SHORT,                                       ///< baseLevel
        FT_SHORT,                                       ///< maxLevel
        FT_SHORT,                                       ///< spellLevel
        FT_BYTE,                                        ///< DifficultyID
        FT_BYTE,                                        ///< MaxUsableLevel
        FT_INT_RELATIONSHIP,                            ///< SpellID
        FT_END
    };

    const char SpellMiscEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_SHORT,                                       ///< CastingTimeIndex
        FT_SHORT,                                       ///< DurationIndex
        FT_SHORT,                                       ///< rangeIndex
        FT_BYTE,                                        ///< SchoolMask
        FT_INT,                                         ///< IconFileDataID
        FT_FLOAT,                                       ///< speed
        FT_INT,                                         ///< ActiveIconFileDataID
        FT_FLOAT,                                       ///< MultistrikeSpeedMod
        FT_BYTE,                                        ///< SchoolMask
        FT_ARR_14, FT_INT,                              ///< Attributes
        FT_INT_RELATIONSHIP,                            ///< SpellID
        FT_END
    };

    const char SpellPowerEntryfmt[] =
    {
        FT_INT,                                         ///< Cost
        FT_FLOAT,                                       ///< CostBasePercentage
        FT_FLOAT,                                       ///< CostPerSecondPercentage
        FT_INT,                                         ///< RequiredAuraSpellId
        FT_FLOAT,                                       ///< CostMaxPercentage
        FT_BYTE,                                        ///< PowerIndex
        FT_BYTE,                                        ///< PowerType
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< CostPerlevel
        FT_INT,                                         ///< CostPerSecond
        FT_INT,                                         ///< CostAdditional
        FT_INT,                                         ///< PowerDisplayID
        FT_INT,                                         ///< UnitPowerBarID
        FT_INT_RELATIONSHIP,                            ///< SpellId
        FT_END
    };

    const char SpellProcsPerMinuteEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_FLOAT,                                       ///< BaseProcRate
        FT_BYTE,                                        ///< Flags
        FT_END
    };

    char const SpellProcsPerMinuteModEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_FLOAT,                                       ///< Coeff
        FT_SHORT,                                       ///< Param
        FT_BYTE,                                        ///< Type
        FT_WORD_RELATIONSHIP,                           ///< SpellProcsPerMinuteID
        FT_END
    };

    const char SpellRadiusEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_FLOAT,                                       ///< radiusHostile
        FT_FLOAT,                                       ///< m_RadiusPerLevel
        FT_FLOAT,                                       ///< radiusFriend
        FT_FLOAT,                                       ///< RadiusMax
        FT_END
    };

    const char SpellRangeEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name;
        FT_STRING,                                      ///< ShortName
        FT_ARR_2, FT_FLOAT,                             ///< minRangeHostile & minRangeFriend
        FT_ARR_2, FT_FLOAT,                             ///< maxRangeHostile & maxRangeFriend
        FT_BYTE,                                        ///< type;
        FT_END
    };

    const char SpellReagentsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SpellID
        FT_ARR_8, FT_INT,                               ///< Reagent[MAX_SPELL_REAGENTS]
        FT_ARR_8, FT_SHORT,                             ///< ReagentCount[MAX_SPELL_REAGENTS]
        FT_END
    };

    const char SpellReagentsCurrencyEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< SpellID
        FT_SHORT,                                       ///< CurrencyID
        FT_SHORT,                                       ///< CurrencyCount
        FT_END
    };

    const char SpellScalingEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SpellID
        FT_SHORT,                                       ///< ScalesFromItemLevel
        FT_INT,                                         ///< ScalingClass
        FT_INT,                                         ///< MinScalingLevel
        FT_INT,                                         ///< MaxScalingLevel
        FT_END
    };

    const char SpellShapeshiftEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SpellID
        FT_ARR_2, FT_INT,                               ///< ShapeshiftExclude[2]
        FT_ARR_2, FT_INT,                               ///< ShapeshiftMask[2]
        FT_BYTE,                                        ///< StanceBarOrder
        FT_END
    };

    const char SpellShapeshiftFormEntryfmt[] =
    {
        FT_INDEX,                                       ///<  ID
        FT_STRING,                                      ///<  m_NameLang
        FT_FLOAT,                                       ///<  UnkLegion
        FT_INT,                                         ///<  m_Flags
        FT_SHORT,                                       ///<  attackSpeed
        FT_SHORT,                                       ///<  m_MountTypeID
        FT_BYTE,                                        ///<  creatureType
        FT_BYTE,                                        ///<  m_BonusActionBar
        FT_INT,                                         ///<  m_AttackIconID
        FT_ARR_4, FT_INT,                               ///<  m_CreatureDisplayID[4]
        FT_ARR_8, FT_INT,                               ///<  stanceSpell[MAX_SHAPESHIFT_SPELLS]
        FT_END
    };

    const char SpellTargetRestrictionsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_FLOAT,                                       ///< ConeAngle
        FT_FLOAT,                                       ///< Width
        FT_INT,                                         ///< Targets
        FT_SHORT,                                       ///< TargetCreatureType
        FT_BYTE,                                        ///< DifficultyID
        FT_BYTE,                                        ///< MaxAffectedTargets
        FT_INT,                                         ///< MaxTargetLevel
        FT_INT_RELATIONSHIP,                            ///< SpellId
        FT_END
    };

    const char SpellTotemsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SpellID
        FT_ARR_2, FT_INT,                               ///< Totem[MAX_SPELL_TOTEMS]
        FT_ARR_2, FT_SHORT,                             ///< TotemCategory[MAX_SPELL_TOTEMS]
        FT_END
    };

    const char SpellVisualEntryfmt[] =
    {
        FT_INT,                                         ///< m_precastKit
        FT_INT,                                         ///< m_castKit
        FT_INT,                                         ///< m_impactKit
        FT_INT,                                         ///< m_stateKit
        FT_INT,                                         ///< m_stateDoneKit
        FT_INT,                                         ///< m_channelKit
        FT_INT,                                         ///< m_hasMissile
        FT_INT,                                         ///< m_missileDestinationAttachment
        FT_INT,                                         ///< m_missileSound
        FT_INT,                                         ///< m_animEventSoundID
        FT_INT,                                         ///< m_casterImpactKit
        FT_INT,                                         ///< m_targetImpactKit
        FT_INT,                                         ///< m_missileAttachment
        FT_INT,                                         ///< m_missileFollowGroundHeight
        FT_INT,                                         ///< m_missileFollowGroundDropSpeed
        FT_ARR_3, FT_FLOAT,                             ///< m_missileCastOffsets
        FT_ARR_3, FT_FLOAT,                             ///< m_missileImpactOffsets
        FT_INT,                                         ///< field27
        FT_INT,                                         ///< field28
        FT_SHORT,                                       ///< m_persistentAreaKit
        FT_SHORT,                                       ///< field26
        FT_SHORT,                                       ///< field29
        FT_BYTE,                                        ///< m_flags
        FT_BYTE,                                        ///< m_missileModel
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< m_missilePathType
        FT_INT,                                         ///< unk30
        FT_INT,                                         ///< unk31
        FT_END
    };

    const char SpellXSpellVisualEntryfmt[] =
    {
        FT_INT,                                         ///< VisualID
        FT_INDEX,                                       ///< Id
        FT_FLOAT,                                       ///< Chance
        FT_SHORT,                                       ///< CasterPlayerConditionID
        FT_SHORT,                                       ///< CasterUnitConditionID
        FT_SHORT,                                       ///< PlayerConditionID
        FT_SHORT,                                       ///< UnitConditionID
        FT_INT,                                         ///< IconFileDataID
        FT_INT,                                         ///< ActiveIconFileDataID
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< DifficultyID
        FT_BYTE,                                        ///< Priority
        FT_INT_RELATIONSHIP,                            ///< SpellId
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Garrison DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Garrison
    const char GarrSiteLevelEntryfmt[] =
    {
        FT_INDEX,                                       ///< SiteLevelID
        FT_ARR_2, FT_FLOAT,                             ///< TownHallX & TownHallY
        FT_SHORT,                                       ///< MapID
        FT_SHORT,                                       ///< SiteID
        FT_SHORT,                                       ///< UITextureKitID
        FT_SHORT,                                       ///< UpgradeCost
        FT_SHORT,                                       ///< UpgradeMoneyCost
        FT_BYTE,                                        ///< Level
        FT_BYTE,                                        ///< MovieID
        FT_BYTE,                                        ///< Level2
        FT_END
    };

    const char GarrSiteLevelPlotInstEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_2, FT_FLOAT,                             ///< MinimapX & MinimapY
        FT_SHORT,                                       ///< SiteLevelID
        FT_BYTE,                                        ///< PlotInstanceID
        FT_BYTE,                                        ///< Unk1
        FT_END
    };

    const char GarrPlotInstanceEntryfmt[] =
    {
        FT_INDEX,                                       ///< InstanceID
        FT_STRING,                                      ///< Name
        FT_BYTE,                                        ///< PlotID
        FT_END
    };

    const char GarrPlotEntryfmt[] =
    {
        FT_INDEX,                                       ///< PlotID
        FT_STRING,                                      ///< Name
        FT_INT,                                         ///< AllianceConstructionGameObjectID
        FT_INT,                                         ///< HordeConstructionGameObjectID
        FT_BYTE,                                        ///< GarrPlotUICategoryID
        FT_BYTE,                                        ///< PlotType
        FT_BYTE,                                        ///< Flags
        FT_ARR_2, FT_INT,                               ///< MinCount & MaxCount
        FT_END
    };

    const char GarrPlotUICategoryEntryfmt[] =
    {
        FT_INDEX,                                       ///< PlotUICategoryID
        FT_STRING,                                      ///< Name
        FT_BYTE,                                        ///< Type
        FT_END
    };

    const char GarrMissionEntryfmt[] =
    {
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_STRING,                                      ///< Location
        FT_INT,                                         ///< Duration
        FT_INT,                                         ///< OfferTime
        FT_ARR_2, FT_FLOAT,                             ///< MapX & MapY
        FT_ARR_2, FT_FLOAT,                             ///< WorldX & WorldY
        FT_SHORT,                                       ///< RequiredItemLevel
        FT_SHORT,                                       ///< LocPrefixID
        FT_SHORT,                                       ///< CurrencyID
        FT_BYTE,                                        ///< RequiredLevel
        FT_BYTE,                                        ///< GarrMechanicTypeRecID
        FT_BYTE,                                        ///< RequiredFollowersCount
        FT_BYTE,                                        ///< Category
        FT_BYTE,                                        ///< MissionType
        FT_BYTE,                                        ///< FollowerType
        FT_BYTE,                                        ///< BaseBonusChance
        FT_BYTE,                                        ///< LostChance
        FT_BYTE,                                        ///< GarrisonType
        FT_INDEX,                                       ///< MissionRecID
        FT_INT,                                         ///< TravelTime
        FT_INT,                                         ///< SubCategory2
        FT_INT,                                         ///< CurrencyCost
        FT_INT,                                         ///< Flags
        FT_INT,                                         ///< RewardFollowerExperience
        FT_INT,                                         ///< ZoneID
        FT_INT,                                         ///< SubCategory1
        FT_INT,                                         ///< UnkLegionB
        FT_INT_RELATIONSHIP,                            ///< UnkLegionA
        FT_END
    };

    const char GarrMissionXEncouterEntryfmt[] =
    {
        FT_BYTE,                                        ///< UnkLegionA
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< EncounterID
        FT_INT,                                         ///< UnkLegionB
        FT_INT_RELATIONSHIP,                            ///< MissionID
        FT_END
    };

    const char GarrBuildingEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< NameAlliance
        FT_STRING,                                      ///< NameHorde
        FT_STRING,                                      ///< Description
        FT_STRING,                                      ///< Tooltip
        FT_INT,                                         ///< GameObjects[0]
        FT_INT,                                         ///< GameObjects[1]
        FT_INT,                                         ///< IconFileDataID
        FT_SHORT,                                       ///< CostCurrencyID
        FT_SHORT,                                       ///< AllianceTexPrefixKitID
        FT_SHORT,                                       ///< HordeTexPrefixKitID
        FT_SHORT,                                       ///< AllianceActivationScenePackageID
        FT_SHORT,                                       ///< HordeActivationScenePackageID
        FT_SHORT,                                       ///< FollowerRequiredGarrAbilityID
        FT_SHORT,                                       ///< FollowerGarrAbilityEffectID
        FT_SHORT,                                       ///< CostMoney
        FT_BYTE,                                        ///< Unk
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< Level
        FT_BYTE,                                        ///< BuildingCategory
        FT_BYTE,                                        ///< MaxShipments
        FT_BYTE,                                        ///< UnkLegion
        FT_INT,                                         ///< BuildDuration
        FT_INT,                                         ///< CostCurrencyAmount
        FT_INT,                                         ///< BonusAmount
        FT_END
    };

    const char GarrPlotBuildingEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_BYTE,                                        ///< PlotID
        FT_BYTE,                                        ///< BuildingID
        FT_END
    };

    const char GarrFollowerEntryfmt[] =
    {
        FT_STRING,                                      ///< HordeSourceText
        FT_STRING,                                      ///< AllinaceSourceText
        FT_STRING,                                      ///< Name
        FT_INT,                                         ///< CreatureID[0]
        FT_INT,                                         ///< CreatureID[1]
        FT_INT,                                         ///< HordePortraitIconID
        FT_INT,                                         ///< AlliancePortraitIconID
        FT_INT,                                         ///< UnkA
        FT_INT,                                         ///< UnkB
        FT_SHORT,                                       ///< HordeGarrFollItemSetID
        FT_SHORT,                                       ///< AllianceGarrFollItemSetID
        FT_SHORT,                                       ///< ItemLevelWeapon
        FT_SHORT,                                       ///< ItemLevelArmor
        FT_SHORT,                                       ///< HordeListPortraitTextureKitID
        FT_SHORT,                                       ///< AllianceListPortraitTextureKitID
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< HordeUiAnimRaceInfoID
        FT_BYTE,                                        ///< AllianceUiAnimRaceInfoID
        FT_BYTE,                                        ///< Quality
        FT_BYTE,                                        ///< HordeGarrClassSecID
        FT_BYTE,                                        ///< AllianceGarrClassSecID
        FT_BYTE,                                        ///< Level
        FT_BYTE,                                        ///< UnkC
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< UnkD
        FT_BYTE,                                        ///< UnkE
        FT_BYTE,                                        ///< UnkF
        FT_BYTE,                                        ///< UnkG
        FT_BYTE,                                        ///< UnkH
        FT_BYTE,                                        ///< UnkI
        FT_BYTE,                                        ///< UnkJ
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char GarrClassSpecEntryfmt[] =
    {
        FT_STRING,                                       ///< HordeName
        FT_STRING,                                       ///< AllianceName
        FT_STRING,                                       ///< Unk1
        FT_SHORT,                                        ///< GarrisonClassID
        FT_SHORT,                                        ///< SpecializationID
        FT_BYTE,                                         ///< MaxTroopAmount
        FT_BYTE,                                         ///< Unk2
        FT_INDEX,                                        ///< ID
        FT_END
    };

    const char GarrFollowerTypefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< MaxItemLevel
        FT_BYTE,                                        ///< SoftCap
        FT_BYTE,                                        ///< SoftCapBuildingIncreaseID
        FT_BYTE,                                        ///< UnkA
        FT_BYTE,                                        ///< UnkB
        FT_BYTE,                                        ///< UnkC
        FT_BYTE,                                        ///< Flags
        FT_END
    };

    const char GarrFollSupportSpellfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< Spell0
        FT_INT,                                         ///< Spell1
        FT_BYTE,                                        ///< Unk
        FT_INT_RELATIONSHIP,                            ///< Follower ID
        FT_END
    };

    const char GarrAbilityEntryfmt[] =
    {
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< IconID
        FT_SHORT,                                       ///< AbilityType
        FT_SHORT,                                       ///< OtherfactionGarrAbilityID
        FT_BYTE,                                        ///< Category
        FT_BYTE,                                        ///< FollowerType
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char GarrAbilityEffectEntryfmt[] =
    {
        FT_FLOAT,                                       ///< ModMin
        FT_FLOAT,                                       ///< ModMax
        FT_FLOAT,                                       ///< Amount
        FT_INT,                                         ///< Unk62
        FT_SHORT,                                       ///< AbilityID
        FT_BYTE,                                        ///< EffectType
        FT_BYTE,                                        ///< TargetMask
        FT_BYTE,                                        ///< CounterMechanicTypeID
        FT_BYTE,                                        ///< Unk3
        FT_BYTE,                                        ///< MiscValueA
        FT_BYTE,                                        ///< MiscValueB
        FT_INDEX,                                       ///< EffectID
        FT_END
    };

    const char GarrFollowerXAbilityEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< AbilityID
        FT_BYTE,                                        ///< FactionIndex
        FT_WORD_RELATIONSHIP,                           ///< FollowerID
        FT_END
    };

    const char GarrBuildingPlotInstEntryfmt[] =
    {
        FT_ARR_2, FT_FLOAT,                             ///< MinimapX & MinimapY
        FT_SHORT,                                       ///< UIAtlasTextureMemberID
        FT_SHORT,                                       ///< SiteLevelPlotInstID
        FT_BYTE,                                        ///< BuildingID
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char GarrMechanicEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_FLOAT,                                       ///< Unk2
        FT_BYTE,                                        ///< Unk1
        FT_INT,                                         ///< MechanicTypeID
        FT_END
    };

    const char GarrEncounterSetXEncounterfmt[] =
    {
        FT_INDEX,                                       ///< Index
        FT_INT,                                         ///< GarrEncounterID
        FT_INT_RELATIONSHIP,                            ///< GarrEncounterSetID
        FT_END
    };

    const char GarrMechanicTypeEntryfmt[] =
    {
        FT_STRING,                                      ///< Environment
        FT_STRING,                                      ///< EnvironmentDesc
        FT_INT,                                         ///< EnvironmentTextureID
        FT_BYTE,                                        ///< Type
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char GarrEncouterXMechanicEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_BYTE,                                        ///< MechanicID
        FT_BYTE,                                        ///< Unk
        FT_WORD_RELATIONSHIP,                           ///< EncounterID
        FT_END
    };

    const char GarrMechanicSetXMechanicfmt[] =
    {
        FT_BYTE,                                        ///< GarrMechanicID
        FT_INDEX,                                       ///< Index
        FT_INT_RELATIONSHIP,                            ///< SetID
        FT_END
    };

    const char GarrFollowerLevelXPEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< RequiredExperience
        FT_SHORT,                                       ///< Unk
        FT_BYTE,                                        ///< Level
        FT_BYTE,                                        ///< FollowerType
        FT_END
    };

    const char GarrSpecializationEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< IconID
        FT_ARR_2, FT_FLOAT,                             ///< BasePoint
        FT_BYTE,                                        ///< Unk2
        FT_BYTE,                                        ///< Unk3
        FT_BYTE,                                        ///< Unk4
        FT_END
    };

    const char CharShipmentEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< Unk1
        FT_INT,                                         ///< Duration
        FT_INT,                                         ///< SpellID
        FT_INT,                                         ///< ResultItemID
        FT_INT,                                         ///< UnkSpellID
        FT_SHORT,                                       ///< ShipmentContainerID
        FT_SHORT,                                       ///< FollowerID
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< Unk1A
        FT_END
    };

    const char GarrTalentEntryfmt[] =
    {
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< IconID
        FT_INT,                                         ///< ResearchTime
        FT_BYTE,                                        ///< Rank
        FT_BYTE,                                        ///< Column
        FT_BYTE,                                        ///< Unk2
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< TalentTreeID
        FT_INT,                                         ///< Unk3
        FT_INT,                                         ///< ResearchPlayerConditionID
        FT_INT,                                         ///< ResearchCurrencyCost
        FT_INT,                                         ///< ResearchCurrencyID
        FT_INT,                                         ///< ResearchGoldCost
        FT_INT,                                         ///< SpellID
        FT_INT,                                         ///< Unk4
        FT_INT,                                         ///< ReplacePlayerConditionID
        FT_INT,                                         ///< ReplaceCurrencyCost
        FT_INT,                                         ///< ReplaceCurrencyID
        FT_INT,                                         ///< ReplaceGoldCost
        FT_END
    };

    const char GarrTalentTreeEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< Unk730
        FT_BYTE,                                        ///< NumRank
        FT_BYTE,                                        ///< Unk
        FT_INT,                                         ///< ClassID
        FT_INT,                                         ///< GarrisonType
        FT_END
    };

    const char GarrTypeEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< Flags
        FT_INT,                                         ///< CurrencyType
        FT_INT,                                         ///< SecondCurrencyType
        FT_INT,                                         ///< ExpansionRequired
        FT_ARR_2, FT_INT,                               ///< BaseMap
        FT_END
    };

    const char CharShipmentContainerEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< Unk10
        FT_SHORT,                                       ///< TextureKitID
        FT_SHORT,                                       ///< OverrideDisplayIfNotNull
        FT_SHORT,                                       ///< OverrideDisplayID1
        FT_SHORT,                                       ///< OverrideIfAmountMet[0]
        FT_SHORT,                                       ///< OverrideIfAmountMet[1]
        FT_SHORT,                                       ///< Unk8
        FT_BYTE,                                        ///< WorkorderLimit
        FT_BYTE,                                        ///< BuildingType
        FT_BYTE,                                        ///< GarrisonType
        FT_BYTE,                                        ///< ShipmentAmountNeeded[0]
        FT_BYTE,                                        ///< ShipmentAmountNeeded[1]
        FT_BYTE,                                        ///< Unk9
        FT_INT,                                         ///< Unk1E
        FT_END
    };

    const char GarrItemLevelUpgradeDataEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< Operation
        FT_INT,                                         ///< MinItemLevel
        FT_INT,                                         ///< MaxItemLevel
        FT_INT,                                         ///< FollowerTypeID
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// BattlePet DB2
//////////////////////////////////////////////////////////////////////////
#pragma region BattlePet
    const char BattlePetAbilityfmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_STRING,                                      ///< name
        FT_STRING,                                      ///< description
        FT_INT,                                         ///< family
        FT_SHORT,                                       ///< visualId
        FT_BYTE,                                        ///< petType
        FT_BYTE,                                        ///< flags
        FT_INT,                                         ///< cooldown
        FT_END
    };

    const char BattlePetAbilityEffectfmt[] =
    {
        FT_SHORT,                                       ///< abilityTurnId
        FT_SHORT,                                       ///< visualId
        FT_SHORT,                                       ///< triggerAbility
        FT_SHORT,                                       ///< effect
        FT_ARR_6, FT_SHORT,                             ///< prop[MAX_BATTLEPET_PROPERTIES]
        FT_BYTE,                                        ///< effectIndex
        FT_INDEX,                                       ///< id
        FT_END
    };

    const char BattlePetAbilityTurnfmt[] =
    {
        FT_SHORT,                                       ///< id
        FT_SHORT,                                       ///< abilityId
        FT_BYTE,                                        ///< visualId
        FT_BYTE,                                        ///< turn
        FT_BYTE,                                        ///< hasProcType
        FT_INDEX,                                       ///< procType
        FT_END
    };

    const char BattlePetAbilityStatefmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_INT,                                         ///< value
        FT_BYTE,                                        ///< hasProcType
        FT_WORD_RELATIONSHIP,                           ///< abilityId
        FT_END
    };

    const char BattlePetStatefmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_STRING,                                      ///< name
        FT_SHORT,                                       ///< parent
        FT_SHORT,                                       ///< flags
        FT_END
    };

    const char BattlePetEffectPropertiesfmt[] =
    {
        FT_INDEX,                                       ///< effect
        FT_ARR_6, FT_STRING,                            ///< propName[MAX_BATTLEPET_PROPERTIES]
        FT_SHORT,                                       ///< flags
        FT_ARR_6, FT_BYTE,                              ///< propIsId[MAX_BATTLEPET_PROPERTIES]
        FT_END
    };

    const char BattlePetBreedQualityfmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_FLOAT,                                       ///< factor
        FT_BYTE,                                        ///< quality
        FT_END
    };

    const char BattlePetBreedStatefmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_SHORT,                                       ///< value
        FT_BYTE,                                        ///< stateId
        FT_BYTE_RELATIONSHIP,                           ///< breed
        FT_END
    };

    const char BattlePetSpeciesfmt[] =
    {
        FT_STRING,                                      ///< source
        FT_STRING,                                      ///< description
        FT_INT,                                         ///< entry
        FT_INT,                                         ///< iconId
        FT_INT,                                         ///< spellId
        FT_SHORT,                                       ///< flags
        FT_BYTE,                                        ///< obtainmentCategoryDescription
        FT_BYTE,                                        ///< type
        FT_INDEX,                                       ///< id
        FT_INT,                                         ///< Unk730_0
        FT_INT,                                         ///< Unk730_1
        FT_END
    };

    const char BattlePetSpeciesStatefmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_INT,                                         ///< value
        FT_BYTE,                                        ///< stateId
        FT_WORD_RELATIONSHIP,                           ///< speciesId
        FT_END
    };

    const char BattlePetSpeciesXAbilityfmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_SHORT,                                       ///< abilityId
        FT_BYTE,                                        ///< level
        FT_BYTE,                                        ///< tier
        FT_WORD_RELATIONSHIP,                           ///< speciesId
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Achievement DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Achievement
    const char Achievementfmt[] =
    {
        FT_STRING,                                      ///< TitleLang
        FT_STRING,                                      ///< DescriptionLang
        FT_STRING,                                      ///< RewardLang
        FT_INT,                                         ///< Flags
        FT_SHORT,                                       ///< InstanceId
        FT_SHORT,                                       ///< Supercedes
        FT_SHORT,                                       ///< Category
        FT_SHORT,                                       ///< UiOrder
        FT_SHORT,                                       ///< SharesCriteria
        FT_BYTE,                                        ///< Faction
        FT_BYTE,                                        ///< Points
        FT_BYTE,                                        ///< MinimumCriteria
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< IconFileDataID
        FT_INT,                                         ///< CriteriaTree
        FT_END
    };

    const char AchievementCategoryfmt[] =
    {
        FT_STRING,
        FT_SHORT,
        FT_BYTE,
        FT_INDEX,
        FT_END
    };

    const char CriteriaTreefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< DescriptionLang
        FT_INT,                                         ///< Amount
        FT_SHORT,                                       ///< Flags
        FT_BYTE,                                        ///< Operator
        FT_INT,                                         ///< CriteriaID
        FT_INT,                                         ///< Parent
        FT_INT,                                         ///< OrderIndex
        FT_END
    };

    const char Criteriafmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< criteriaArg1
        FT_INT,                                         ///< StartAsset
        FT_INT,                                         ///< FailAsset
        FT_INT,                                         ///< ModifierTreeId
        FT_SHORT,                                       ///< StartTimer
        FT_SHORT,                                       ///< EligibilityWorldStateID
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< StartEvent
        FT_BYTE,                                        ///< FailEvent
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< EligibilityWorldStateValue
        FT_END
    };

    const char ModifierTreefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< Asset
        FT_INT,                                         ///< SecondaryAsset
        FT_INT,                                         ///< Parent
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< Unk
        FT_BYTE,                                        ///< Operator
        FT_BYTE,                                        ///< Amount
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Taxi DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Taxi
    const char TaxiPathFormat[] =
    {
        FT_SHORT,                                       ///< From
        FT_SHORT,                                       ///< To
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< Cost
        FT_END
    };

    const char TaxiNodesFormat[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_ARR_3, FT_FLOAT,                             ///< x & y & z
        FT_ARR_2, FT_INT,                               ///< MountCreatureID[2]
        FT_ARR_2, FT_FLOAT,                             ///< m_MapOffsetX & m_MapOffsetY
        FT_FLOAT,                                       ///< Unk730
        FT_ARR_2, FT_FLOAT,                             ///< m_FlightMapOffsetX & m_FlightMapOffsetY
        FT_SHORT,                                       ///< MapID
        FT_SHORT,                                       ///< ConditionID
        FT_SHORT,                                       ///< LearnableIndex
        FT_BYTE,                                        ///< Flags
        FT_INT,                                         ///< UiTextureKitPrefixID
        FT_INT,                                         ///< SpecialAtlasIconPlayerConditionID
        FT_END
    };

    const char TaxiPathNodeFormat[] =
    {
        FT_ARR_3, FT_FLOAT,                             ///< x & y & z
        FT_SHORT,                                       ///< PathID
        FT_SHORT,                                       ///< MapID
        FT_BYTE,                                        ///< NodeIndex
        FT_INDEX,                                       ///< ID
        FT_BYTE,                                        ///< Flags
        FT_INT,                                         ///< Delay
        FT_SHORT,                                       ///< ArrivalEventID
        FT_SHORT,                                       ///< DepartureEventID
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Artifact DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Artifact
    const char Aritfactfmt[] =
    {
        FT_INDEX,                                       ///< m_ID
        FT_STRING,                                      ///< m_Name
        FT_INT,                                         ///< BarConnectRBG
        FT_INT,                                         ///< BarDisconnectRBG
        FT_INT,                                         ///< TitleRBG
        FT_SHORT,                                       ///< UITextureKitID
        FT_SHORT,                                       ///< SpecializationID
        FT_BYTE,                                        ///< Unk
        FT_BYTE,                                        ///< Flags
        FT_INT,                                         ///< UiModelSceneID
        FT_INT,                                         ///< SpellVisualKitID
        FT_END
    };

    const char ArtifactAppearancefmt[] =
    {
        FT_STRING,                                      ///< Name
        FT_INT,                                         ///< Unk0
        FT_FLOAT,                                       ///< UnkScale0
        FT_FLOAT,                                       ///< UnkScale1
        FT_INT,                                         ///< DruidFormRelated
        FT_SHORT,                                       ///< ArtifactApperaranceSetID
        FT_SHORT,                                       ///< Unk1
        FT_BYTE,                                        ///< Collumn
        FT_BYTE,                                        ///< AppearanceModID
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< Unk2
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< ConditionID
        FT_INT,                                         ///< Unk3
        FT_INT,                                         ///< Unk4
        FT_END
    };

    const char ArtifactAppearanceSetfmt[] =
    {
        FT_STRING,                                      ///< m_Name
        FT_STRING,                                      ///< m_Name2
        FT_SHORT,                                       ///< m_UiCameraID
        FT_SHORT,                                       ///< m_AltHandUICameraID
        FT_BYTE,                                        ///< m_DisplayIndex
        FT_BYTE,                                        ///< m_AttachmentPoint
        FT_BYTE,                                        ///< unk
        FT_INDEX,                                       ///< m_ID
        FT_INT_RELATIONSHIP,                            ///< m_ArtifactID
        FT_END
    };

    const char ArtifactPowerfmt[] =
    {
        FT_ARR_2, FT_FLOAT,                             ///< MapOffsetX
        FT_BYTE,                                        ///< ArtifactID
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< MaxRank
        FT_BYTE,                                        ///< ArtifactTier
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< RelicType
        FT_END
    };

    const char ArtifactPowerLinkfmt[] =
    {
        FT_INDEX,                                       ///< Index
        FT_SHORT,                                       ///< ID
        FT_SHORT,                                       ///< UnlockedID
        FT_END
    };

    const char ArtifactPowerPickerfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< PlayerConditionID
        FT_END
    };

    const char ArtifactPowerRankfmt[] =
    {
        FT_INDEX,                                       ///< Index
        FT_INT,                                         ///< SpellID
        FT_FLOAT,                                       ///< RankBasePoints
        FT_SHORT,                                       ///< Unk2
        FT_BYTE,                                        ///< Rank
        FT_WORD_RELATIONSHIP,                           ///< ArtifactPowerID
        FT_END
    };

    const char ArtifactQuestXPfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_10, FT_INT,                              ///< Exp
        FT_END
    };

    const char ArtifactCategoryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< ArtifactKnowledgeCurrencyID
        FT_SHORT,                                       ///< ArtifactKnowledgeMultiplierCurveID
        FT_END
    };

    const char RelicTalentfmt[] =
    {
        FT_INDEX,
        FT_SHORT,
        FT_BYTE,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_END
    };

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// SceneScript DB2
//////////////////////////////////////////////////////////////////////////
#pragma region SceneScript
    const char SceneScriptPackageEntryfmt[] =
    {
        FT_INDEX,                                       ///< Entry
        FT_STRING,                                      ///< Name
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// WebBrowser DB2
//////////////////////////////////////////////////////////////////////////
#pragma region WebBrowser
    const char WbAccessControlListfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Url
        FT_SHORT,                                       ///< AccessFlags
        FT_BYTE,                                        ///< Unk1
        FT_BYTE,                                        ///< Restricted
        FT_BYTE,                                        ///< Region
        FT_END
    };

    const char WbCertWhitelistfmt[] =
    {
        FT_INDEX,
        FT_STRING,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Item DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Item
    const char ItemPriceBasefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_FLOAT,                                       ///< ArmorFactor
        FT_FLOAT,                                       ///< WeaponFactor
        FT_SHORT,                                       ///< SpellID
        FT_END
    };

    const char ItemClassfmt[] =
    {
        FT_INDEX,                                       ///< Class
        FT_STRING,                                      ///< m_ClassNameLang
        FT_FLOAT,                                       ///< PriceFactor
        FT_BYTE,                                        ///< OldEnumValue
        FT_BYTE,                                        ///< m_Flags
        FT_END
    };

    const char ItemDisenchantLootfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_SHORT,                                       ///< MinItemLevel
        FT_SHORT,                                       ///< MaxItemLevel
        FT_SHORT,                                       ///< RequiredDisenchantSkill
        FT_BYTE,                                        ///< ItemSubClass
        FT_BYTE,                                        ///< ItemQuality
        FT_BYTE,                                        ///< Expansion
        FT_BYTE_RELATIONSHIP,                           ///< ItemClass
        FT_END
    };

    const char ItemLimitCategoryEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< name
        FT_BYTE,                                        ///< maxCount
        FT_BYTE,                                        ///< mode
        FT_END
    };

    const char ItemLimitCategoryConditionEntryfmt[] =
    {
        FT_INDEX,                                       ///< Index
        FT_BYTE,                                        ///< Count
        FT_INT,                                         ///< PlayerConditionID
        FT_INT_RELATIONSHIP,                            ///< CategoryID
        FT_END
    };

    const char ItemRandomPropertiesfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< nameSuffix
        FT_ARR_5, FT_SHORT,                             ///< enchant_id
        FT_END
    };

    const char ItemRandomSuffixfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< nameSuffix
        FT_ARR_5, FT_SHORT,                             ///< enchant_id
        FT_ARR_5, FT_SHORT,                             ///< prefix
        FT_END
    };

    const char ItemSpecEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< SpecializationID
        FT_BYTE,                                        ///< MinLevel
        FT_BYTE,                                        ///< MaxLevel
        FT_BYTE,                                        ///< ItemType
        FT_BYTE,                                        ///< PrimaryStat
        FT_BYTE,                                        ///< SecondaryStat
        FT_END
    };

    const char ItemSpecOverrideEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< specID
        FT_INT_RELATIONSHIP,                            ///< itemEntry
        FT_END
    };

    const char Itemfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< DisplayID
        FT_BYTE,                                        ///< Class
        FT_BYTE,                                        ///< SubClass
        FT_BYTE,                                        ///< SoundOverrideSubclass
        FT_BYTE,                                        ///< Material
        FT_BYTE,                                        ///< InventoryType
        FT_BYTE,                                        ///< Sheath
        FT_BYTE,                                        ///< GroupSoundsID
        FT_END
    };

    const char ItemBonusfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_3, FT_INT,                               ///< Value[3]
        FT_SHORT,                                       ///< Entry
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< Index
        FT_END
    };

    const char ItemBonusTreeNodefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< Context
        FT_SHORT,                                       ///< ItemBonusEntry
        FT_SHORT,                                       ///< ItemLevelSelectorID
        FT_BYTE,                                        ///< LinkedCategory
        FT_WORD_RELATIONSHIP,                           ///< Category
        FT_END
    };

    const char ItemBonusListLevelDeltaFmt[] =
    {
        FT_SHORT,                                        ///< Delta
        FT_INDEX,                                        ///< Id
        FT_END
    };

    const char ItemXBonusTreefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< ItemBonusTreeCategory
        FT_INT_RELATIONSHIP,                            ///< ItemId
        FT_END
    };

    const char ItemCurrencyCostfmt[] =
    {
        FT_INT,                                         ///< Id
        FT_INDEX,                                       ///< ItemId
        FT_END
    };

    const char ItemExtendedCostEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_5, FT_INT,                               ///< RequiredItem[MAX_ITEM_EXT_COST_ITEMS]
        FT_ARR_5, FT_INT,                               ///< RequiredCurrencyCount[MAX_ITEM_EXT_COST_CURRENCIES]
        FT_ARR_5, FT_SHORT,                             ///< RequiredItemCount[MAX_ITEM_EXT_COST_ITEMS]
        FT_SHORT,                                       ///< RequiredPersonalArenaRating
        FT_ARR_5, FT_SHORT,                             ///< RequiredCurrency[MAX_ITEM_EXT_COST_CURRENCIES]
        FT_BYTE,                                        ///< RequiredArenaSlot
        FT_BYTE,                                        ///< RequiredFactionId
        FT_BYTE,                                        ///< RequiredFactionStanding
        FT_BYTE,                                        ///< RequirementFlags
        FT_BYTE,                                        ///< RequiredAchievement
        FT_END
    };

    const char ItemSparsefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_QWORD,                                       ///< AllowableRace
        FT_NSTRING,                                     ///< Name
        FT_NSTRING,                                     ///< Name2
        FT_NSTRING,                                     ///< Name3
        FT_NSTRING,                                     ///< Name4
        FT_NSTRING,                                     ///< Description
        FT_ARR_4, FT_INT,                               ///< Flags & Flags2 & Flags3 & Flags4
        FT_FLOAT,                                       ///< Unk430_1
        FT_FLOAT,                                       ///< Unk430_2
        FT_INT,                                         ///< BuyCount
        FT_INT,                                         ///< BuyPrice
        FT_INT,                                         ///< SellPrice
        FT_INT,                                         ///< RequiredSpell
        FT_INT,                                         ///< MaxCount
        FT_INT,                                         ///< Stackable
        FT_ARR_10, FT_INT,                              ///< ScalingValue
        FT_ARR_10, FT_FLOAT,                            ///< SocketCostRate
        FT_FLOAT,                                       ///< RangedModRange
        FT_INT,                                         ///< BagFamily
        FT_FLOAT,                                       ///< ArmorDamageModifier
        FT_INT,                                         ///< Duration
        FT_FLOAT,                                       ///< StatScalingFactor
        FT_SHORT,                                       ///< AllowableClass
        FT_SHORT,                                       ///< ItemLevel
        FT_SHORT,                                       ///< RequiredSkill
        FT_SHORT,                                       ///< RequiredSkillRank
        FT_SHORT,                                       ///< RequiredReputationFaction
        FT_ARR_10, FT_SHORT,                            ///< ItemStatValue
        FT_SHORT,                                       ///< ScalingStatDistribution
        FT_SHORT,                                       ///< Delay
        FT_SHORT,                                       ///< PageText
        FT_SHORT,                                       ///< StartQuest
        FT_SHORT,                                       ///< LockID
        FT_SHORT,                                       ///< RandomProperty
        FT_SHORT,                                       ///< RandomSuffix
        FT_SHORT,                                       ///< ItemSet
        FT_SHORT,                                       ///< Area
        FT_SHORT,                                       ///< Map
        FT_SHORT,                                       ///< TotemCategory
        FT_SHORT,                                       ///< SocketBonus
        FT_SHORT,                                       ///< GemProperties
        FT_SHORT,                                       ///< ItemLimitCategory
        FT_SHORT,                                       ///< HolidayId
        FT_SHORT,                                       ///< RequiredTransmogHolidayID
        FT_SHORT,                                       ///< ItemNameDescriptionID
        FT_BYTE,                                        ///< Quality
        FT_BYTE,                                        ///< InventoryType
        FT_BYTE,                                        ///< RequiredLevel
        FT_BYTE,                                        ///< RequiredHonorRank
        FT_BYTE,                                        ///< RequiredCityRank
        FT_BYTE,                                        ///< RequiredReputationRank
        FT_BYTE,                                        ///< ContainerSlots
        FT_ARR_10, FT_BYTE,                             ///< ItemStatType
        FT_BYTE,                                        ///< DamageType
        FT_BYTE,                                        ///< Bonding
        FT_BYTE,                                        ///< LanguageID
        FT_BYTE,                                        ///< PageMaterial
        FT_BYTE,                                        ///< Material
        FT_BYTE,                                        ///< Sheath
        FT_ARR_3, FT_BYTE,                              ///< Color
        FT_BYTE,                                        ///< CurrencySubstitutionId
        FT_BYTE,                                        ///< CurrencySubstitutionCount
        FT_BYTE,                                        ///< ArtifactID
        FT_BYTE,                                        ///< RequiredExpansion
        FT_END
    };

    const char ItemEffectFmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< SpellID
        FT_INT,                                         ///< SpellCooldown
        FT_INT,                                         ///< SpellCategoryCooldown
        FT_SHORT,                                       ///< SpellCharge
        FT_SHORT,                                       ///< SpellCategory
        FT_SHORT,                                       ///< ChrSpecializationID
        FT_BYTE,                                        ///< EffectIndex
        FT_BYTE,                                        ///< SpellTrigger
        FT_INT_RELATIONSHIP,                            ///< ItemID
        FT_END
    };

    const char ItemModifiedAppearanceFmt[] =
    {
        FT_INT,                                         ///< ItemID
        FT_INDEX,                                       ///< ID
        FT_BYTE,                                        ///< AppearanceModID
        FT_SHORT,                                       ///< AppearanceID
        FT_BYTE,                                        ///< Index
        FT_BYTE,                                        ///< SourceType
        FT_END
    };

    const char ItemAppearanceFmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< DisplayID
        FT_INT,                                         ///< IconFileDataID
        FT_INT,                                         ///< UIOrder
        FT_BYTE,                                        ///< ObjectComponentSlot
        FT_END
    };

    const char TransmogSetItemFmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< TransmogSetId
        FT_INT,                                         ///< AppearanceId
        FT_INT,                                         ///< Unk
        FT_END
    };

    const char TransmogSetFmt[] =
    {
        FT_STRING,                                      ///< Name
        FT_SHORT,                                       ///< BaseSetID;
        FT_SHORT,                                       ///< UIOrder
        FT_BYTE,                                        ///< ExpansionID
        FT_INDEX,                                       ///< ID;
        FT_INT,                                         ///< Flags
        FT_INT,                                         ///< QuestID
        FT_INT,                                         ///< ClassMask
        FT_INT,                                         ///< ItemNameDescriptionID
        FT_INT,                                         ///< TransmogSetGroupID
        FT_END
    };

    const char ItemUpgradeEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< currencyCost
        FT_SHORT,                                       ///< precItemUpgradeId
        FT_SHORT,                                       ///< currencyId
        FT_BYTE,                                        ///< itemUpgradePath
        FT_BYTE,                                        ///< itemLevelUpgrade
        FT_END
    };

    const char ItemChildEquipmentfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< ChildItemID
        FT_BYTE,                                        ///< TargetSlot
        FT_INT_RELATIONSHIP,                            ///< ItemID
        FT_END
    };

    const char ItemSetSpellFmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< SpellID
        FT_SHORT,                                       ///< SpecializationID
        FT_BYTE,                                        ///< PieceRequirement
        FT_WORD_RELATIONSHIP,                           ///< ItemSetID
        FT_END
    };

    const char ItemArmorQualityfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_ARR_7, FT_FLOAT,                             ///< Value[7]
        FT_SHORT,                                       ///< Id2
        FT_END
    };

    const char ItemArmorShieldfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_ARR_7, FT_FLOAT,                             ///< Value[7]
        FT_SHORT,                                       ///< Id2
        FT_END
    };

    const char ItemArmorTotalfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_FLOAT,                                       ///< Value
        FT_FLOAT,                                       ///< Value
        FT_FLOAT,                                       ///< Value
        FT_FLOAT,                                       ///< Value
        FT_SHORT,                                       ///< Id2
        FT_END
    };

    const char ItemBagFamilyfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< m_NameLang
        FT_END
    };

    /// common struct for:
    /// ItemDamageAmmo.db2
    /// ItemDamageOneHand.db2
    /// ItemDamageOneHandCaster.db2
    /// ItemDamageTwoHand.db2
    /// ItemDamageTwoHandCaster.db2
    const char ItemDamagefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_7, FT_FLOAT,                             ///< DPS
        FT_SHORT,                                       ///< Id2
        FT_END
    };

    const char ItemSetEntryfmt[] =
    {
        FT_INDEX,                                       ///< m_ID
        FT_STRING,                                      ///< m_name_lang
        FT_ARR_17, FT_INT,                              ///< m_itemID
        FT_SHORT,                                       ///< m_requiredSkillRank
        FT_INT,                                         ///< m_requiredSkill
        FT_INT,                                         ///<
        FT_END
    };

    const char ItemSearchNameEntryfmt[] =
    {
        FT_QWORD,                                       ///< AllowableRace
        FT_STRING,                                      ///< Name
        FT_INDEX,                                       ///< m_ID
        FT_ARR_3, FT_INT,                               ///< Flags[3]
        FT_SHORT,                                       ///< ItemLevel
        FT_BYTE,                                        ///< Quality
        FT_BYTE,                                        ///< RequiredExpansion
        FT_BYTE,                                        ///< RequiredLevel
        FT_SHORT,                                       ///< RequiredReputationFaction
        FT_BYTE,                                        ///< RequiredReputationRank
        FT_INT,                                         ///< AllowableClass
        FT_SHORT,                                       ///< RequiredSkill
        FT_SHORT,                                       ///< RequiredSkillRank
        FT_INT,                                         ///< RequiredSpell
        FT_END
    };

    const char RulesetItemUpgradeEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< itemid
        FT_SHORT,                                       ///< itemUpgradeId
        FT_END
    };

    const char PvpItemfmt[] =
    {
        FT_INDEX,                                       ///< RecordID
        FT_INT,                                         ///< itemId
        FT_BYTE,                                        ///< ilvl
        FT_END
    };

    const char PvpScalingEffectfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_FLOAT,                                       ///< Value
        FT_INT,                                         ///< PvPScalingEffectTypeId
        FT_INT,                                         ///< SpecializationId
        FT_END
    };

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Archaeology DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Archaeology
    const char ResearchBranchfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< m_Name
        FT_INT,                                         ///< ItemID
        FT_SHORT,                                       ///< CurrencyID
        FT_BYTE,                                        ///< m_ResearchFieldID
        FT_INT,                                         ///< m_TextureFileID1
        FT_INT,                                         ///< m_TextureFileID2
        FT_END
    };

    const char ResearchProjectfmt[] =
    {
        FT_STRING,                                      ///< m_NameLang
        FT_STRING,                                      ///< m_DescriptionLang
        FT_INT,                                         ///< spellId
        FT_SHORT,                                       ///< branchId
        FT_BYTE,                                        ///< rare
        FT_BYTE,                                        ///< m_NumSockets
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< m_TextureFileID
        FT_INT,                                         ///< req_currency
        FT_END
    };

    const char ResearchSitefmt[] =
    {
        FT_INT,                                         ///< ID
        FT_STRING,                                      ///< m_NameLang
        FT_INT,                                         ///< POIid
        FT_SHORT,                                       ///< mapId
        FT_INT,                                         ///< AreaPOIIconEnum
        FT_END
    };

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Quest DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Quest
    const char QuestV2CliTaskFmt[] =
    {
        FT_QWORD,                                       ///< RaceMask
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Unk2
        FT_INT,                                         ///< Unk1
        FT_SHORT,                                       ///< Unk4
        FT_SHORT,                                       ///< Unk5
        FT_SHORT,                                       ///< Unk6
        FT_ARR_3, FT_SHORT,                             ///< Unk7[3]
        FT_SHORT,                                       ///< Unk8
        FT_SHORT,                                       ///< Unk9
        FT_BYTE,                                        ///< Unk10
        FT_BYTE,                                        ///< Unk11
        FT_BYTE,                                        ///< Unk12
        FT_BYTE,                                        ///< Unk13
        FT_BYTE,                                        ///< Unk14
        FT_BYTE,                                        ///< Unk15
        FT_BYTE,                                        ///< Unk16
        FT_BYTE,                                        ///< Unk17
        FT_BYTE,                                        ///< Unk18
        FT_BYTE,                                        ///< Unk19
        FT_INDEX,                                       ///< QuestID
        FT_INT,                                         ///< Unk20
        FT_INT,                                         ///< Unk21
        FT_INT,                                         ///< Unk21
        FT_END
    };

    const char Bountyfmt[] =
    {
        FT_INDEX,                                       ///< Index
        FT_INT,                                         ///< Unk
        FT_SHORT,                                       ///< QuestID
        FT_SHORT,                                       ///< FactionID
        FT_INT,                                         ///< Unk730
        FT_BYTE_RELATIONSHIP,                           ///< RequiredCount
        FT_END
    };

    const char QuestPOIPointCliTaskfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< X
        FT_SHORT,                                       ///< Y
        FT_SHORT,                                       ///< QuestID
        FT_SHORT,                                       ///< MapID
        FT_SHORT,                                       ///< AreaID
        FT_BYTE,                                        ///< Floor
        FT_INT,                                         ///< Unk
        FT_END
    };

    const char AdventureMapPOIfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Unk1
        FT_STRING,                                      ///< Unk2
        FT_ARR_2, FT_FLOAT,                             ///< Position
        FT_INT,                                         ///< Unk0
        FT_BYTE,                                        ///< Unk3
        FT_INT,                                         ///< Unk4
        FT_INT,                                         ///< QuestID
        FT_INT,                                         ///< Unk5
        FT_INT,                                         ///< Unk6
        FT_INT,                                         ///< Unk7
        FT_INT,                                         ///< Unk8
        FT_INT,                                         ///< Unk9
        FT_INT,                                         ///< Unk10
        FT_END
    };

    const char QuestPackageItemEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< ItemId
        FT_SHORT,                                       ///< PackageID
        FT_BYTE,                                        ///< Type
        FT_INT,                                         ///< Count
        FT_END
    };

    const char QuestPOIPointfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< x
        FT_SHORT,                                       ///< y
        FT_INT_RELATIONSHIP,                            ///< unk
        FT_END
    };

    const char QuestSortEntryfmt[] =
    {
        FT_INDEX,                                       ///< m_ID
        FT_STRING,                                      ///< m_SortName_lang
        FT_BYTE,                                        ///< m_Unk
        FT_END
    };

    const char QuestV2fmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< UniqueBitFlag
        FT_END
    };

    const char QuestXPfmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_ARR_10, FT_SHORT,                            ///< Exp
        FT_END
    };

    const char QuestFactionRewardfmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_ARR_10, FT_SHORT,                            ///< Exp
        FT_END
    };

    const char QuestMoneyRewardfmt[] =
    {
        FT_INDEX,
        FT_ARR_10, FT_INT,
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Area and maps DB2
//////////////////////////////////////////////////////////////////////////
#pragma region AreaAndMaps
    const char AreaTablefmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_STRING,                                      ///< SortName_lang
        FT_STRING,                                      ///< AreaNameLang
        FT_ARR_2, FT_INT,                               ///< Flags & Flags2
        FT_FLOAT,                                       ///< AmbientMultiplier
        FT_SHORT,                                       ///< ContinentID
        FT_SHORT,                                       ///< ParentAreaID
        FT_SHORT,                                       ///< AreaBit
        FT_SHORT,                                       ///< AmbienceID
        FT_SHORT,                                       ///< ZoneMusic
        FT_SHORT,                                       ///< IntroSound
        FT_ARR_4, FT_SHORT,                             ///< LiquidTypeID[4]
        FT_SHORT,                                       ///< UwZoneMusic
        FT_SHORT,                                       ///< UwAmbience
        FT_SHORT,                                       ///< PvpCombatWorldStateID
        FT_BYTE,                                        ///< SoundProviderPref
        FT_BYTE,                                        ///< SoundProviderPrefUnderwater
        FT_BYTE,                                        ///< ExplorationLevel
        FT_BYTE,                                        ///< FactionGroupMask
        FT_BYTE,                                        ///< MountFlags
        FT_BYTE,                                        ///< WildBattlePetLevelMin
        FT_BYTE,                                        ///< WildBattlePetLevelMax
        FT_BYTE,                                        ///< WindSettingsID
        FT_INT,                                         ///< UwIntroSound
        FT_END
    };

    const char AreaGroupMemberEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< AreaID
        FT_WORD_RELATIONSHIP,                           ///< AreaGroupID
        FT_END
    };

    const char AreaTriggerEntryfmt[] =
    {
        FT_ARR_3, FT_FLOAT,                             ///< Pos[3]
        FT_FLOAT,                                       ///< Radius
        FT_FLOAT,                                       ///< BoxLength
        FT_FLOAT,                                       ///< BoxWidth
        FT_FLOAT,                                       ///< BoxHeight
        FT_FLOAT,                                       ///< BoxYaw
        FT_SHORT,                                       ///< ContinentID
        FT_SHORT,                                       ///< PhaseID
        FT_SHORT,                                       ///< PhaseGroupID
        FT_SHORT,                                       ///< ShapeID
        FT_SHORT,                                       ///< AreaTriggerActionSetID
        FT_BYTE,                                        ///< PhaseUseFlags
        FT_BYTE,                                        ///< ShapeType
        FT_BYTE,                                        ///< Flag
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char DifficultyEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_SHORT,                                       ///< GroupSizeHealthCurveID
        FT_SHORT,                                       ///< GroupSizeDmgCurveID
        FT_SHORT,                                       ///< GroupSizeSpellPointsCurveID
        FT_BYTE,                                        ///< FallbackDifficultyID
        FT_BYTE,                                        ///< InstanceType
        FT_BYTE,                                        ///< MinPlayers
        FT_BYTE,                                        ///< MaxPlayers
        FT_BYTE,                                        ///< OldEnumValue
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< ToggleDifficultyID
        FT_BYTE,                                        ///< ItemBonusTreeModID
        FT_BYTE,                                        ///< OrderIndex
        FT_END
    };

    const char LiquidTypefmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< Name
        FT_ARR_6, FT_STRING,                            ///< Texture[6]
        FT_INT,                                         ///< SpellID
        FT_FLOAT,                                       ///< MaxDarkenDepth
        FT_FLOAT,                                       ///< FogDarkenIntensity
        FT_FLOAT,                                       ///< AmbDarkenIntensity
        FT_FLOAT,                                       ///< DirDarkenIntensity
        FT_FLOAT,                                       ///< ParticleScale
        FT_ARR_2, FT_INT,                               ///< Color[2]
        FT_ARR_18, FT_FLOAT,                            ///< Float[18]
        FT_ARR_4, FT_INT,                               ///< Int[4]
        FT_SHORT,                                       ///< Flags
        FT_SHORT,                                       ///< LightID
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< ParticleMovement
        FT_BYTE,                                        ///< ParticleTexSlots
        FT_BYTE,                                        ///< MaterialID
        FT_ARR_6, FT_BYTE,                              ///< DepthTexCount[6]
        FT_INT,                                         ///< SoundID
        FT_END
    };

    const char MapEntryfmt[] =
    {
        FT_INDEX,                                       ///< MapID
        FT_STRING,                                      ///< Directory
        FT_STRING,                                      ///< MapNameLang
        FT_STRING,                                      ///< MapDescription0
        FT_STRING,                                      ///< MapDescription1
        FT_STRING,                                      ///< ShortDescription
        FT_STRING,                                      ///< LongDescription
        FT_ARR_2, FT_INT,                               ///< Flags & Flags2
        FT_FLOAT,                                       ///< MinimapIconScale
        FT_ARR_2, FT_FLOAT,                             ///< CorpseX & CorpseY
        FT_SHORT,                                       ///< AreaTableID
        FT_SHORT,                                       ///< LoadingScreenID
        FT_SHORT,                                       ///< CorpseMapID
        FT_SHORT,                                       ///< TimeOfDayOverride
        FT_SHORT,                                       ///< ParentMapID
        FT_SHORT,                                       ///< CosmeticParentMapID
        FT_SHORT,                                       ///< WindSettingsID
        FT_BYTE,                                        ///< instanceType
        FT_BYTE,                                        ///< unk5
        FT_BYTE,                                        ///< ExpansionID
        FT_BYTE,                                        ///< MaxPlayers
        FT_BYTE,                                        ///< TimeOffset
        FT_END
    };

    const char MapDifficultyEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< AreaTriggerText
        FT_BYTE,                                        ///< DifficultyID
        FT_BYTE,                                        ///< RaidDurationType
        FT_BYTE,                                        ///< MaxPlayers
        FT_BYTE,                                        ///< LockID
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< ItemBonusTreeDifficulty
        FT_INT,                                         ///< Context
        FT_WORD_RELATIONSHIP,                           ///< MapId
        FT_END
    };

    const char PhaseEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< flag
        FT_END
    };

    const char WorldMapOverlayEntryfmt[] =
    {
        FT_STRING,                                      ///< TextureName
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< TextureWidth
        FT_SHORT,                                       ///< TextureHeight
        FT_INT,                                         ///< MapAreaID
        FT_INT,                                         ///< OffsetX
        FT_INT,                                         ///< OffsetY
        FT_INT,                                         ///< HitRectTop
        FT_INT,                                         ///< HitRectLeft
        FT_INT,                                         ///< HitRectBottom
        FT_INT,                                         ///< HitRectRight
        FT_INT,                                         ///< PlayerConditionID
        FT_INT,                                         ///< Flags
        FT_ARR_4, FT_INT,                               ///< areatableID
        FT_END
    };

    const char WMOAreaTableEntryfmt[] =
    {
        FT_STRING,                                      ///< AreaName
        FT_INT,                                         ///< WMOGroupID
        FT_SHORT,                                       ///< AmbienceID
        FT_SHORT,                                       ///< ZoneMusic
        FT_SHORT,                                       ///< IntroSound
        FT_SHORT,                                       ///< AreaTableID
        FT_SHORT,                                       ///< UWIntroSound
        FT_SHORT,                                       ///< UWAmbience
        FT_BYTE,                                        ///< NameSet
        FT_BYTE,                                        ///< SoundProviderPref
        FT_BYTE,                                        ///< SoundProviderPrefUnderwater
        FT_BYTE,                                        ///< Flags
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< UWZoneMusic
        FT_WORD_RELATIONSHIP,                           ///< WMOID
        FT_END
    };

    const char WorldMapAreaEntryfmt[] =
    {
        FT_STRING,                                      ///< AreaName
        FT_FLOAT,                                       ///< y1
        FT_FLOAT,                                       ///< y2
        FT_FLOAT,                                       ///< x1
        FT_FLOAT,                                       ///< x2
        FT_INT,                                         ///< Flags
        FT_SHORT,                                       ///< map_id
        FT_SHORT,                                       ///< area_id
        FT_SHORT,                                       ///< virtual_map_id
        FT_SHORT,                                       ///< DefaultDungeonFloor
        FT_SHORT,                                       ///< ParentWorldMapID
        FT_BYTE,                                        ///< minRecommendedLevel
        FT_BYTE,                                        ///< maxRecommendedLevel
        FT_BYTE,                                        ///< BountySetID
        FT_BYTE,                                        ///< BountyBoardLocation
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< PlayerConditionID
        FT_END
    };

    const char WorldMapTransformsfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_6, FT_FLOAT,                             ///< RegionMinX to RegionMaxZ
        FT_ARR_2, FT_FLOAT,                             ///< RegionOffsetX & RegionOffsetY
        FT_FLOAT,                                       ///< RegionScale
        FT_SHORT,                                       ///< MapID
        FT_SHORT,                                       ///< AreaID
        FT_SHORT,                                       ///< NewMapID
        FT_SHORT,                                       ///< NewDungeonMapID
        FT_SHORT,                                       ///< NewAreaID
        FT_BYTE,                                        ///< Flags
        FT_INT,                                         ///< Priority
        FT_END
    };

    const char World_PVP_AreaEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< AreaID
        FT_SHORT,                                       ///< NextTimeWorldState
        FT_SHORT,                                       ///< GameTimeWorldState
        FT_SHORT,                                       ///< BattlePopulateTime
        FT_SHORT,                                       ///< MapID
        FT_BYTE,                                        ///< MinLevel
        FT_BYTE,                                        ///< MaxLevel
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Import DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Import
    const char ImportPriceArmorfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_FLOAT,                                       ///< ClothFactor
        FT_FLOAT,                                       ///< LeatherFactor
        FT_FLOAT,                                       ///< MailFactor
        FT_FLOAT,                                       ///< PlateFactor
        FT_END
    };

    const char ImportPriceQualityfmt[] =
    {
        FT_INDEX,                                       ///< QualityId
        FT_FLOAT,                                       ///< Factor
        FT_END
    };

    const char ImportPriceShieldfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_FLOAT,                                       ///< Factor
        FT_END
    };

    const char ImportPriceWeaponfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_FLOAT,                                       ///< Factor
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Char DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Char
    const char CharStartOutfitEntryfmt[] =
    {
        FT_INDEX,                                       ///< m_ID
        FT_ARR_24, FT_INT,                              ///< ItemId[MAX_OUTFIT_ITEMS]
        FT_INT,                                         ///< m_petDisplayID
        FT_BYTE,                                        ///< ClassID
        FT_BYTE,                                        ///< SexID
        FT_BYTE,                                        ///< OutfitID
        FT_BYTE,                                        ///< m_petFamilyId
        FT_BYTE_RELATIONSHIP,                           ///< RaceID
        FT_END
    };

    const char CharTitlesEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< NameMale
        FT_STRING,                                      ///< NameFemale
        FT_SHORT,                                       ///< MaskID
        FT_BYTE,                                        ///< Flags
        FT_END
    };

    const char CharacterLoadoutItemEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< ItemID
        FT_WORD_RELATIONSHIP,                           ///< LoadoutID
        FT_END
    };

    const char CharacterLoadoutEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_QWORD,                                       ///< Flags
        FT_BYTE,                                        ///< ClassId
        FT_BYTE,                                        ///< Expansion
        FT_END
    };

    const char ChrClassesEntryfmt[] =
    {
        FT_STRING,                                      ///< m_petNameToken
        FT_STRING,                                      ///< NameLang
        FT_STRING,                                      ///< m_name_female_lang
        FT_STRING,                                      ///< m_name_male_lang
        FT_STRING,                                      ///< m_filename
        FT_INT,                                         ///< m_CreateScreenFileDataID
        FT_INT,                                         ///< m_SelectScreenFileDataID
        FT_INT,                                         ///< m_IconFileDataID
        FT_INT,                                         ///< m_LowResScreenFileDataID
        FT_INT,                                         ///< m_RequiredPlayerLevel;
        FT_SHORT,                                       ///< flags
        FT_SHORT,                                       ///< CinematicSequenceID
        FT_SHORT,                                       ///< m_DefaultSpec
        FT_BYTE,                                        ///< DisplayPower
        FT_BYTE,                                        ///< SpellClassSet
        FT_BYTE,                                        ///< AttackPowerPerStrength
        FT_BYTE,                                        ///< AttackPowerPerAgility
        FT_BYTE,                                        ///< RangedAttackPowerPerAgility
        FT_BYTE,                                        ///< MainStat
        FT_INDEX,                                       ///< ClassID
        FT_END
    };

    const char ChrClassesXPowerTypesfmt[] =
    {
        FT_INDEX,                                       ///< entry
        FT_BYTE,                                        ///< power
        FT_BYTE_RELATIONSHIP,                           ///< classId
        FT_END
    };

    const char ChrRacesfmt[] =
    {
        FT_STRING,                                      ///< m_ClientPrefix
        FT_STRING,                                      ///< m_ClientFileString
        FT_STRING,                                      ///< name
        FT_STRING,                                      ///< m_NameFemaleLang
        FT_STRING,                                      ///< m_NameMaleLang
        FT_STRING,                                      ///< m_HairCustomization
        FT_INT,                                         ///< Flags
        FT_INT,                                         ///< MaleDisplayID
        FT_INT,                                         ///< FemaleDisplayID
        FT_INT,                                         ///< m_CreateScreenFileDataID
        FT_INT,                                         ///< m_SelectScreenFileDataID
        FT_ARR_3, FT_FLOAT,                             ///< m_MaleCustomizeOffset[3]
        FT_ARR_3, FT_FLOAT,                             ///< m_FemaleCustomizeOffset[3]
        FT_INT,                                         ///< m_LowResScreenFileDataID
        FT_INT,                                         ///< StartingLevel
        FT_INT,                                         ///< UIDisplayOrder
        FT_SHORT,                                       ///< FactionID
        FT_SHORT,                                       ///< m_ResSicknessSpellID
        FT_SHORT,                                       ///< m_SplashSoundID
        FT_SHORT,                                       ///< CinematicSequence
        FT_BYTE,                                        ///< TeamID
        FT_BYTE,                                        ///< m_CreatureType
        FT_BYTE,                                        ///< m_alliance
        FT_BYTE,                                        ///< m_RaceRelated
        FT_BYTE,                                        ///< m_UnalteredVisualRaceID
        FT_BYTE,                                        ///< m_CharComponentTextureLayoutID
        FT_BYTE,                                        ///< m_DefaultClassID
        FT_BYTE,                                        ///< m_NeutralRaceID
        FT_BYTE,                                        ///< ItemAppearanceFrameRaceID
        FT_BYTE,                                        ///< m_CharComponentTexLayoutHiResID
        FT_INDEX,                                       ///< entry
        FT_INT,                                         ///< m_HighResMaleDisplayId
        FT_INT,                                         ///< m_HighResFemaleDisplayId
        FT_INT,                                         ///< HeritageArmorAchievementID
        FT_INT,                                         ///< MaleCorpseBonesModelFileDataID
        FT_INT,                                         ///< FemaleCorpseBonesModelFileDataID
        FT_ARR_3, FT_INT,                               ///< AlteredFormTransitionSpellVisualID[3]
        FT_ARR_3, FT_INT,                               ///< AlteredFormTransitionSpellVisualKitID[3]
        FT_END
    };

    const char ChrSpecializationsfmt[] =
    {
        FT_STRING,                                      ///< SpecializationName
        FT_STRING,                                      ///< NameLang
        FT_STRING,                                      ///< Description
        FT_ARR_2, FT_INT,                               ///< MasterySpellID & MasterySpellID2
        FT_BYTE,                                        ///< ClassID
        FT_BYTE,                                        ///< OrderIndex
        FT_BYTE,                                        ///< PetTalentType
        FT_BYTE,                                        ///< Role
        FT_BYTE,                                        ///< MainStat
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< IconID
        FT_INT,                                         ///< Flags
        FT_INT,                                         ///< AnimReplacementSetID
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Creature DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Creature
    const char CreatureDisplayInfofmt[] =
    {
        FT_INDEX,                                       ///< Displayid
        FT_FLOAT,                                       ///< scale
        FT_SHORT,                                       ///< ModelId
        FT_SHORT,                                       ///< NPCSoundID
        FT_BYTE,                                        ///< SizeClass
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< Gender
        FT_INT,                                         ///< ExtendedDisplayInfoID
        FT_INT,                                         ///< PortraitTextureFileID
        FT_BYTE,                                        ///< CreatureModelAlpha
        FT_SHORT,                                       ///< SoundID
        FT_FLOAT,                                       ///< PlayerModelScale
        FT_INT,                                         ///< PortraitCreatureDisplayInfoID
        FT_BYTE,                                        ///< BloodID
        FT_SHORT,                                       ///< ParticleColorID
        FT_INT,                                         ///< CreatureGeosetData
        FT_SHORT,                                       ///< ObjectEffectPackageID
        FT_SHORT,                                       ///< AnimReplacementSetID
        FT_BYTE,                                        ///< UnarmedWeaponSubclass
        FT_INT,                                         ///< StateSpellVisualKitID
        FT_FLOAT,                                       ///< InstanceOtherPlayerPetScale
        FT_INT,                                         ///< MountSpellVisualKitID
        FT_ARR_3, FT_INT,                               ///< TextureVariation[3]
        FT_END
    };

    char const CreatureDisplayInfoExtrafmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< FileDataID
        FT_INT,                                         ///< HDFileDataID
        FT_BYTE,                                        ///< DisplayRaceID
        FT_BYTE,                                        ///< DisplaySexID
        FT_BYTE,                                        ///< DisplayClassID
        FT_BYTE,                                        ///< SkinID;
        FT_BYTE,                                        ///< FaceID
        FT_BYTE,                                        ///< HairStyleID
        FT_BYTE,                                        ///< HairColorID
        FT_BYTE,                                        ///< FacialHairID
        FT_ARR_3, FT_BYTE,                              ///< CustomDisplayOption[3]
        FT_BYTE,                                        ///< Flags
        FT_END
    };

    const char CreatureFamilyfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_FLOAT,                                       ///< minScale
        FT_FLOAT,                                       ///< maxScale
        FT_INT,                                         ///< IconFileDataID
        FT_ARR_2, FT_SHORT,                             ///< skillLine
        FT_SHORT,                                       ///< petFoodMask
        FT_BYTE,                                        ///< minScaleLevel
        FT_BYTE,                                        ///< maxScaleLevel
        FT_BYTE,                                        ///< petTalentType
        FT_END
    };

    const char CreatureModelDatafmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_FLOAT,                                       ///< ModelScale
        FT_FLOAT,                                       ///< FootprintTextureLength
        FT_FLOAT,                                       ///< FootprintTextureWidth
        FT_FLOAT,                                       ///< FootprintParticleScale
        FT_FLOAT,                                       ///< CollisionWidth
        FT_FLOAT,                                       ///< CollisionHeight
        FT_FLOAT,                                       ///< MountHeight
        FT_ARR_6, FT_FLOAT,                             ///< GeoBoxMin & GeoBoxMax
        FT_FLOAT,                                       ///< WorldEffectScale
        FT_FLOAT,                                       ///< AttachedEffectScale
        FT_FLOAT,                                       ///< MissileCollisionRadius
        FT_FLOAT,                                       ///< MissileCollisionPush
        FT_FLOAT,                                       ///< MissileCollisionRaise
        FT_FLOAT,                                       ///< OverrideLootEffectScale
        FT_FLOAT,                                       ///< OverrideNameScale
        FT_FLOAT,                                       ///< OverrideSelectionRadius
        FT_FLOAT,                                       ///< TamedPetBaseScale
        FT_FLOAT,                                       ///< HoverHeight
        FT_INT,                                         ///< Flags
        FT_INT,                                         ///< FileDataID
        FT_INT,                                         ///< SizeClass
        FT_INT,                                         ///< BloodID
        FT_INT,                                         ///< FootprintTextureID
        FT_INT,                                         ///< FoleyMaterialID
        FT_INT,                                         ///< FootstepEffectID
        FT_INT,                                         ///< DeathThudEffectID
        FT_INT,                                         ///< SoundID
        FT_INT,                                         ///< CreatureGeosetDataID
        FT_END
    };

    const char CreatureTypefmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< nameLang
        FT_BYTE,                                        ///< flags
        FT_END
    };
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Misc DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Misc
    const char AnimKitfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< OneShotDuration
        FT_SHORT,                                       ///< OneShotStopAnimKitID
        FT_SHORT,                                       ///< LowDefAnimKitID
        FT_END
    };

    const char ArmorLocationfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_FLOAT,                                       ///< Value
        FT_FLOAT,                                       ///< Value
        FT_FLOAT,                                       ///< Value
        FT_FLOAT,                                       ///< Value
        FT_FLOAT,                                       ///< Value
        FT_END
    };

    const char AuctionHouseEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_SHORT,                                       ///< FactionID
        FT_BYTE,                                        ///< DepositRate
        FT_BYTE,                                        ///< ConsignmentRate
        FT_END
    };

    const char BankBagSlotPricesEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< price
        FT_END
    };

    const char BarberShopStyleEntryfmt[] =
    {
        FT_STRING,                                      ///< m_DisplayName_lang
        FT_STRING,                                      ///< m_Description_lang
        FT_FLOAT,                                       ///< m_CostMultiplier
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< Race
        FT_BYTE,                                        ///< Sex
        FT_BYTE,                                        ///< Data
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char BattlemasterListEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< GameLang
        FT_STRING,                                      ///< GameType
        FT_STRING,                                      ///< ShortDescription
        FT_STRING,                                      ///< LongDescription
        FT_INT,                                         ///< IconFileDataID
        FT_ARR_16, FT_SHORT,                            ///< MapID[16]
        FT_SHORT,                                       ///< HolidayWorldState
        FT_SHORT,                                       ///< PlayerConditionID
        FT_BYTE,                                        ///< InstanceType
        FT_BYTE,                                        ///< GroupsAllowed
        FT_BYTE,                                        ///< MaxGroupSize
        FT_BYTE,                                        ///< MinLevel
        FT_BYTE,                                        ///< MaxLevel
        FT_BYTE,                                        ///< RatedPlayers
        FT_BYTE,                                        ///< MinPlayers
        FT_BYTE,                                        ///< MaxPlayers
        FT_BYTE,                                        ///< Flags
        FT_END
    };

    char const BroadcastTextEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< MaleText
        FT_STRING,                                      ///< FemaleText
        FT_ARR_3, FT_SHORT,                             ///< EmoteID
        FT_ARR_3, FT_SHORT,                             ///< EmoteDelay
        FT_SHORT,                                       ///< UnkEmoteID
        FT_BYTE,                                        ///< Language
        FT_BYTE,                                        ///< Type
        FT_INT,                                         ///< PlayerConditionID
        FT_ARR_2, FT_INT,                               ///< SoundID
        FT_END
    };

    char const CharSectionsEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_3, FT_INT,                               ///< TextureFileDataID
        FT_SHORT,                                       ///< Flags
        FT_BYTE,                                        ///< Race
        FT_BYTE,                                        ///< Gender
        FT_BYTE,                                        ///< GenType
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< Color
        FT_END
    };

    const char CinematicCameraEntryfmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_INT,                                       ///< soundid
        FT_ARR_3, FT_FLOAT,                             ///< EndPosX & EndPosY & EndPosZ
        FT_FLOAT,                                       ///< GlobalRotation
        FT_INT,                                         ///< filename
        FT_END
    };

    const char CinematicSequencesEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SoundID
        FT_ARR_8, FT_SHORT,                             ///< cinematicCamera
        FT_END
    };

    const char ChatChannelsEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< NameLang
        FT_STRING,                                      ///< Shortcut
        FT_INT,                                         ///< Flags
        FT_BYTE,                                        ///< FactionGroup
        FT_END
    };

    const char CurrencyTypesfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< NameLang
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< TotalCap
        FT_INT,                                         ///< WeekCap
        FT_INT,                                         ///< Flags
        FT_BYTE,                                        ///< CategoryID
        FT_BYTE,                                        ///< SpellCategory
        FT_BYTE,                                        ///< Quality
        FT_INT,                                         ///< InventoryIconFileDataID
        FT_INT,                                         ///< SpellWeight
        FT_END
    };

    const char CurveEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< Unused
        FT_END
    };

    const char CurvePointEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_2, FT_FLOAT,                             ///< X & Y
        FT_SHORT,                                       ///< CurveID
        FT_BYTE,                                        ///< Index
        FT_END
    };

    const char DestructibleModelDatafmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_SHORT,                                       ///< DamagedDisplayId
        FT_SHORT,                                       ///< DestroyedDisplayId
        FT_SHORT,                                       ///< RebuildingDisplayId
        FT_SHORT,                                       ///< SmokeDisplayId
        FT_SHORT,                                       ///< HealEffectSpeed
        FT_BYTE,                                        ///< StateDamagedImpactEffectDoodadSet
        FT_BYTE,                                        ///< StateDamagedAmbientDoodadSet
        FT_BYTE,                                        ///< StateDamagedNameSet
        FT_BYTE,                                        ///< StateDestroyedDestructionDoodadSet
        FT_BYTE,                                        ///< StateDestroyedImpactEffectDoodadSet
        FT_BYTE,                                        ///< StateDestroyedAmbientDoodadSet
        FT_BYTE,                                        ///< StateDestroyedNameSet
        FT_BYTE,                                        ///< StateRebuildingDestructionDoodadSet
        FT_BYTE,                                        ///< StateRebuildingImpactEffectDoodadSet
        FT_BYTE,                                        ///< StateRebuildingAmbientDoodadSet
        FT_BYTE,                                        ///< StateRebuildingNameSet
        FT_BYTE,                                        ///< StateSmokeInitDoodadSet
        FT_BYTE,                                        ///< StateSmokeAmbientDoodadSet
        FT_BYTE,                                        ///< StateSmokeNameSet
        FT_BYTE,                                        ///< EjectDirection
        FT_BYTE,                                        ///< DoNotHighlight
        FT_BYTE,                                        ///< HealEffect
        FT_END
    };

    const char DungeonEncounterfmt[] =
    {
        FT_STRING,                                      ///< NameLang
        FT_INT,                                         ///< CreatureDisplayID
        FT_SHORT,                                       ///< MapID
        FT_BYTE,                                        ///< DifficultyID
        FT_BYTE,                                        ///< Bit
        FT_BYTE,                                        ///< Flags
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< OrderIndex
        FT_INT,                                         ///< TextureFileDataID
        FT_END
    };

    const char DurabilityCostsfmt[] =
    {
        FT_INDEX,                                       ///< Itemlvl
        FT_ARR_21, FT_SHORT,                            ///< multiplier
        FT_ARR_8, FT_SHORT,                             ///< multiplier
        FT_END
    };

    const char DurabilityQualityfmt[] =
    {
        FT_INDEX,                                       ///< Itemlvl
        FT_FLOAT,                                       ///< quality_mod
        FT_END
    };

    const char EmotesEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_QWORD,                                       ///< RaceMask
        FT_STRING,                                      ///< EmoteSlashCommand
        FT_INT,                                         ///< SpellVisualKitID
        FT_INT,                                         ///< Flags
        FT_SHORT,                                       ///< m_AnimID
        FT_BYTE,                                        ///< EmoteType
        FT_INT,                                         ///< UnitStandState
        FT_INT,                                         ///< m_EventSoundID
        FT_INT,                                         ///< ClassMask
        FT_END
    };

    const char EmotesTextEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< m_Name
        FT_SHORT,                                       ///< textid
        FT_END
    };

    const char EmotesTextSoundEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_BYTE,                                        ///< RaceId
        FT_BYTE,                                        ///< Gender
        FT_BYTE,                                        ///< ClassId
        FT_INT,                                         ///< SoundID
        FT_WORD_RELATIONSHIP,                           ///< TextId
        FT_END
    };

    const char FactionEntryfmt[] =
    {
        FT_ARR_4, FT_QWORD,                             ///< ReputationRaceMask
        FT_STRING,                                      ///< NameLang
        FT_STRING,                                      ///< DescriptionLang
        FT_INDEX,                                       ///< ID
        FT_ARR_4, FT_INT,                               ///< ReputationBase
        FT_ARR_2, FT_FLOAT,                             ///< ParentFactionModIn
        FT_ARR_4, FT_INT,                               ///< ReputationMax
        FT_SHORT,                                       ///< ReputationIndex
        FT_ARR_4, FT_SHORT,                             ///< ReputationClassMask
        FT_ARR_4, FT_SHORT,                             ///< ReputationFlags
        FT_SHORT,                                       ///< ParentFactionID
        FT_SHORT,                                       ///< ParagonFactionID
        FT_ARR_2, FT_BYTE,                              ///< ParentFactionCapIn
        FT_BYTE,                                        ///< Expansion
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< FriendshipRepID
        FT_END
    };

    const char FactionTemplateEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< Faction
        FT_SHORT,                                       ///< Flags
        FT_ARR_4, FT_SHORT,                             ///< Enemies[MAX_FACTION_RELATIONS]
        FT_ARR_4, FT_SHORT,                             ///< Friend[MAX_FACTION_RELATIONS]
        FT_BYTE,                                        ///< FactionGroup
        FT_BYTE,                                        ///< FriendGroup
        FT_BYTE,                                        ///< EnemyGroup
        FT_END
    };

    const char GameObjectDisplayInfofmt[] =
    {
        FT_INDEX,                                       ///< Displayid
        FT_INT,                                         ///< m_FileDataID
        FT_ARR_6, FT_FLOAT,                             ///<
        FT_FLOAT,                                       ///< m_OverrideLootEffectScale
        FT_FLOAT,                                       ///< m_OverrideNameScale
        FT_SHORT,                                       ///< m_ObjectEffectPackageID
        FT_END
    };

    const char GemPropertiesEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< color
        FT_SHORT,                                       ///< spellitemenchantement
        FT_SHORT,                                       ///< requiredILvl
        FT_END
    };

    char const GlyphBindableSpellfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< SpellID
        FT_WORD_RELATIONSHIP,                           ///< GlyphPropID
        FT_END
    };

    const char GlobalStringsfmt[] =
    {
        FT_INDEX,
        FT_STRING,
        FT_STRING,
        FT_BYTE,
        FT_END
    };

    const char GlyphPropertiesfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< SpellId
        FT_SHORT,                                       ///< m_SpellIconID
        FT_BYTE,                                        ///< TypeFlags
        FT_BYTE,                                        ///< GlyphExclusiveCategoryID
        FT_END
    };

    const char GlyphRequiredSpecfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< SpecID
        FT_WORD_RELATIONSHIP,                           ///< GlyphID
        FT_END
    };

    const char GroupFinderActivityfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< DifficultyString
        FT_SHORT,                                       ///< RequiredILvl
        FT_SHORT,                                       ///< MapID
        FT_SHORT,                                       ///< ZoneID
        FT_BYTE,                                        ///< CategoryID
        FT_BYTE,                                        ///< ActivityGroupID
        FT_BYTE,                                        ///< Unk1
        FT_BYTE,                                        ///< MinLevel
        FT_BYTE,                                        ///< MaxLevel
        FT_BYTE,                                        ///< Difficulty
        FT_BYTE,                                        ///< Type
        FT_BYTE,                                        ///< Unk10
        FT_BYTE,                                        ///< MaxPlayers
        FT_END
    };

    const char GroupFinderCategoryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_BYTE,                                        ///< Unk1
        FT_BYTE,                                        ///< Unk2
        FT_END
    };

    const char GuildPerkSpellsfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< SpellId
        FT_END
    };

    const char HeirloomFmt[] =
    {
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< ItemID
        FT_INT, FT_INT,                                 ///< OldHeirloomID[2]
        FT_INT,                                         ///< HeroicVersion
        FT_ARR_3, FT_INT,                               ///< UpgradableByItemID[MAX_HEIRLOOM_UPGRADE_LEVEL]
        FT_ARR_3, FT_SHORT,                             ///< UpgradeIemBonusID[MAX_HEIRLOOM_UPGRADE_LEVEL]
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< Source
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char Holidaysfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_ARR_16, FT_INT,                              ///< Date[MAX_HOLIDAY_DATES]
        FT_ARR_10, FT_SHORT,                            ///< Duration[MAX_HOLIDAY_DURATIONS]
        FT_SHORT,                                       ///< Region
        FT_BYTE,                                        ///< Looping
        FT_ARR_10, FT_BYTE,                             ///< CalendarFlags[MAX_HOLIDAY_FLAGS]
        FT_BYTE,                                        ///< Priority
        FT_BYTE,                                        ///< CalendarFilterType
        FT_BYTE,                                        ///< flags
        FT_INT,                                         ///< holidayNameId
        FT_INT,                                         ///< holidayDescriptionId
        FT_ARR_3, FT_INT,                               ///< TextureFileDataID[MAX_HOLIDAY_DATES]
        FT_END
    };

    const char JournalInstancefmt[]=
    {
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< Unk1
        FT_INT,                                         ///< Unk2
        FT_INT,                                         ///< Unk3
        FT_INT,                                         ///< Unk4
        FT_SHORT,                                       ///< MapID
        FT_SHORT,                                       ///< Unk5
        FT_BYTE,                                        ///< Unk6
        FT_BYTE,                                        ///< Unk7
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char JournalEncounterfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_ARR_2, FT_FLOAT,                             ///< Unk1
        FT_SHORT,                                       ///< Unk2
        FT_SHORT,                                       ///< Unk3
        FT_SHORT,                                       ///< Unk4
        FT_SHORT,                                       ///< JournalInstanceID
        FT_BYTE,                                        ///< Unk5
        FT_BYTE,                                        ///< Unk6
        FT_INT,                                         ///< Unk7
        FT_INT,                                         ///< Unk8
        FT_END
    };

    const char JournalEncounterItemfmt[] =
    {
        FT_INT,                                         ///< ItemID
        FT_SHORT,                                       ///< JournalEncounterID
        FT_BYTE,                                        ///< Unk1
        FT_BYTE,                                        ///< Unk2
        FT_BYTE,                                        ///< Unk3
        FT_INDEX,                                       ///< Id
        FT_END
    };

    const char LFGDungeonEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description;
        FT_INT,                                         ///< Flags
        FT_FLOAT,                                       ///< MinItemLevel
        FT_SHORT,                                       ///< maxlevel
        FT_SHORT,                                       ///< recmaxlevel
        FT_SHORT,                                       ///< map
        FT_SHORT,                                       ///< flags2
        FT_SHORT,                                       ///< ScenarioID
        FT_SHORT,                                       ///< LastBossJournalEncounterID
        FT_SHORT,                                       ///< BonusReputationAmount
        FT_SHORT,                                       ///< MentorItemLevel
        FT_SHORT,                                       ///< PlayerConditionID
        FT_BYTE,                                        ///< minlevel
        FT_BYTE,                                        ///< reclevel
        FT_BYTE,                                        ///< recminlevel
        FT_BYTE,                                        ///< difficulty
        FT_BYTE,                                        ///< type
        FT_BYTE,                                        ///< Faction
        FT_BYTE,                                        ///< expansion
        FT_BYTE,                                        ///< OrderIndex
        FT_BYTE,                                        ///< grouptype
        FT_BYTE,                                        ///< tankNeeded
        FT_BYTE,                                        ///< healerNeeded
        FT_BYTE,                                        ///< dpsNeeded
        FT_BYTE,                                        ///< MinCountTank
        FT_BYTE,                                        ///< MinCountHealer
        FT_BYTE,                                        ///< MinCountDamage
        FT_BYTE,                                        ///< category
        FT_BYTE,                                        ///< MentorCharLevel
        FT_INT,                                         ///< TextureFileDataID
        FT_INT,                                         ///< RewardIconFileDataID
        FT_INT,                                         ///< ProposalTextureFileDataID
        FT_END
    };

    const char LocationEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_3, FT_FLOAT,                             ///< X & Y & Z
        FT_ARR_3, FT_FLOAT,                             ///< Unk1 & Unk2 & Unk3
        FT_END
    };

    const char LockEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_8, FT_INT,                               ///< Index
        FT_ARR_8, FT_SHORT,                             ///< Skill
        FT_ARR_8, FT_BYTE,                              ///< Type
        FT_ARR_8, FT_BYTE,                              ///< Action
        FT_END
    };

    const char MailTemplateEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< content
        FT_END
    };

    const char MapChallengeModeEntryfmt[] =
    {
        FT_STRING,                                      ///< Name
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< MapID
        FT_ARR_3, FT_SHORT,                             ///< BronzeTime & SilverTime & GoldTime
        FT_BYTE,                                        ///< Field3
        FT_END
    };

    const char MinorTalentfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< spellID
        FT_INT,                                         ///< orderIndex
        FT_INT_RELATIONSHIP,                            ///< specializationID
        FT_END
    };

    const char MountCapabilityfmt[] =
    {
        FT_INT,                                         ///< RequiredSpell
        FT_INT,                                         ///< SpeedModSpell
        FT_SHORT,                                       ///< RequiredRidingSkill
        FT_SHORT,                                       ///< RequiredArea
        FT_SHORT,                                       ///< RequiredMap
        FT_BYTE,                                        ///< Flags
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< RequiredAura
        FT_END
    };

    const char MountEntryfmt[] =
    {
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_STRING,                                      ///< MountEntry
        FT_INT,                                         ///< SpellID
        FT_FLOAT,                                       ///< CameraPivotMultiplier
        FT_SHORT,                                       ///< MountType
        FT_SHORT,                                       ///< Flags
        FT_BYTE,                                        ///< FilterCategory
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< PlayerConditionID
        FT_INT,                                         ///< UiModelSceneID
        FT_END
    };

    const char MountTypeXCapabilityfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< MountTypeID
        FT_SHORT,                                       ///< CapabilityID
        FT_BYTE,                                        ///< Index
        FT_END
    };

    const char MountXDisplayfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< DisplayID
        FT_INT,                                         ///< PlayerConditionID
        FT_INT_RELATIONSHIP,                            ///< MountID
        FT_END
    };

    const char MovieEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< AudioFileDataID
        FT_INT,                                         ///< m_SubtitleFileDataID
        FT_BYTE,                                        ///< Volume
        FT_BYTE,                                        ///< KeyID
        FT_END
    };

    const char NameGenfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< name
        FT_BYTE,                                        ///< race
        FT_BYTE,                                        ///< gender
        FT_END
    };

    const char ParagonReputationfmt[] =
    {
        FT_INDEX,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_INT_RELATIONSHIP,
        FT_END
    };

    char const PathNodeEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< LocationID
        FT_SHORT,                                       ///< PathID
        FT_SHORT,                                       ///< Order
        FT_END
    };

    const char PlayerConditionEntryfmt[] =
    {
        FT_QWORD,
        FT_STRING,
        FT_INDEX,
        FT_BYTE,
        FT_SHORT,
        FT_SHORT,
        FT_INT,
        FT_BYTE,
        FT_BYTE,
        FT_INT,
        FT_BYTE,
        FT_BYTE,
        FT_INT,
        FT_SHORT,
        FT_BYTE,
        FT_INT,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_BYTE,
        FT_INT,
        FT_SHORT,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_SHORT,
        FT_INT,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_SHORT,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_SHORT,
        FT_SHORT,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_BYTE,
        FT_INT,
        FT_INT,
        FT_ARR_4, FT_SHORT,
        FT_ARR_4, FT_SHORT,
        FT_ARR_4, FT_SHORT,
        FT_ARR_3, FT_INT,
        FT_ARR_3, FT_BYTE,
        FT_ARR_4, FT_SHORT,
        FT_ARR_4, FT_SHORT,
        FT_ARR_4, FT_SHORT,
        FT_ARR_4, FT_INT,
        FT_ARR_4, FT_INT,
        FT_ARR_4, FT_INT,
        FT_ARR_2, FT_SHORT,
        FT_ARR_2, FT_INT,
        FT_ARR_4, FT_INT,
        FT_ARR_4, FT_BYTE,
        FT_ARR_4, FT_SHORT,
        FT_ARR_4, FT_BYTE,
        FT_ARR_4, FT_BYTE,
        FT_ARR_4, FT_INT,
        FT_ARR_4, FT_SHORT,
        FT_ARR_4, FT_INT,
        FT_ARR_4, FT_INT,
        FT_ARR_6, FT_INT,
        FT_ARR_2, FT_INT,
        FT_END
    };

    const char PowerDisplayfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< GlobalStringBaseTag
        FT_BYTE,                                        ///< ActualType
        FT_BYTE,                                        ///< Red
        FT_BYTE,                                        ///< Green
        FT_BYTE,                                        ///< Blue
        FT_END
    };

    const char PowerTypefmt[] =
    {
        FT_INT,                                         ///< m_DBIndex
        FT_STRING,                                      ///< m_Name
        FT_STRING,                                      ///< PowerCostToken
        FT_FLOAT,                                       ///< RegenerationPeace
        FT_FLOAT,                                       ///< RegenerationCombat
        FT_SHORT,                                       ///< MaxPower
        FT_SHORT,                                       ///< RegenerationDelay
        FT_SHORT,                                       ///< Flags
        FT_BYTE_INDEX,                                  ///< PowerTypeEnum
        FT_BYTE,                                        ///< RegenerationMin
        FT_BYTE,                                        ///< RegenerationCenter
        FT_BYTE,                                        ///< RegenerationMax
        FT_BYTE,                                        ///< UIModifier
        FT_END,
    };

    const char PrestigeLevelInfofmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< m_Name
        FT_INT,                                         ///< m_TextureID
        FT_BYTE,                                        ///< m_Level
        FT_BYTE,                                        ///< m_Unk
        FT_END
    };

    const char PvPDifficultyfmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_BYTE,                                        ///< bracketId
        FT_BYTE,                                        ///< minLevel
        FT_BYTE,                                        ///< maxLevel
        FT_WORD_RELATIONSHIP,                           ///< mapId
        FT_END
    };

    const char PvpTalentfmt[] =
    {
        FT_INDEX,                                      ///< ID
        FT_STRING,                                     ///< Name
        FT_INT,                                        ///< SpellID
        FT_INT,                                        ///< OverrideSpellID
        FT_INT,                                        ///< ExtraSpellID
        FT_INT,                                        ///< TierIndex
        FT_INT,                                        ///< CollumnIndex
        FT_INT,                                        ///< Flags
        FT_INT,                                        ///< Class
        FT_INT,                                        ///< SpecExclusive
        FT_INT,                                        ///< Role
        FT_END
    };

    const char PvpTalentUnlockfmt[] =
    {
        FT_INDEX,                                       ///< m_ID
        FT_BYTE,                                        ///< m_TierIndex
        FT_BYTE,                                        ///< m_CollumnIndex
        FT_BYTE,                                        ///< m_LevelRequirement
        FT_END
    };

    const char PvpRewardfmt[] =
    {
        FT_INDEX,                                       ///< m_ID
        FT_INT,                                         ///< m_Level
        FT_INT,                                         ///< m_Prestigie
        FT_INT,                                         ///< m_RewardPackID
        FT_END
    };

    const char RandomPropertiesPointsfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_ARR_5, FT_INT,                               ///< EpicPropertiesPoints[5];
        FT_ARR_5, FT_INT,                               ///< RarePropertiesPoints[5];
        FT_ARR_5, FT_INT,                               ///< UncommonPropertiesPoints[5]
        FT_END
    };

    const char RewardPackfmt[] =
    {
        FT_INDEX,
        FT_INT,
        FT_FLOAT,
        FT_BYTE,
        FT_BYTE,
        FT_INT,
        FT_INT,
        FT_END
    };

    const char RewardPackXCurrencyfmt[] =
    {
        FT_INDEX,                                       ///< m_ID
        FT_INT,                                         ///< m_CurrencyID
        FT_INT,                                         ///< m_Count
        FT_INT_RELATIONSHIP,                            ///< m_RewardPackID
        FT_END
    };

    const char RewardPackXItemfmt[] =
    {
        FT_INDEX,                                       ///< m_ID
        FT_INT,                                         ///< m_RewardPackID
        FT_INT,                                         ///< m_Count
        FT_INT_RELATIONSHIP,                            ///< m_ItemID
        FT_END
    };

    const char ScalingStatDistributionfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_SHORT,                                       ///< m_Flags
        FT_INT,                                         ///< UncommonPropertiesPoints[5]
        FT_INT,                                         ///< UncommonPropertiesPoints[5]
        FT_END
    };

    const char SandboxScalingfmt[] =
    {
        FT_INDEX,
        FT_INT,
        FT_INT,
        FT_INT,
        FT_END
    };

    const char ScenarioEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< LocalizedString
        FT_SHORT,                                       ///< Data
        FT_BYTE,                                        ///< Flags
        FT_BYTE,                                        ///< Type
        FT_END
    };

    const char ScenarioStepEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< DescriptionLang
        FT_STRING,                                      ///< TitleLang
        FT_SHORT,                                       ///< ScenarioID
        FT_SHORT,                                       ///< RelatedStep
        FT_SHORT,                                       ///< RewardQuestID
        FT_BYTE,                                        ///< OrderIndex
        FT_BYTE,                                        ///< Flags
        FT_INT,                                         ///< CriteriaTreeID
        FT_INT,                                         ///< Supersedes
        FT_END
    };

    const char SkillLineAbilityfmt[] =
    {
        FT_QWORD,                                       ///< racemask
        FT_INDEX,                                       ///< id
        FT_INT,                                         ///< spellId
        FT_INT,                                         ///< forward_spellid
        FT_SHORT,                                       ///< skillId
        FT_SHORT,                                       ///< max_value
        FT_SHORT,                                       ///< min_value
        FT_SHORT,                                       ///< m_UniqueBit
        FT_SHORT,                                       ///< m_TradeSkillCategoryID
        FT_BYTE,                                        ///< skill_gain
        FT_INT,                                         ///< classmask
        FT_SHORT,                                       ///< m_minSkillLineRank
        FT_BYTE,                                        ///< Unk
        FT_BYTE,                                        ///< Unk
        FT_END
    };

    const char SkillRaceClassInfofmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_QWORD,                                       ///< RaceMask
        FT_SHORT,                                       ///< SkillID
        FT_SHORT,                                       ///< Flags
        FT_SHORT,                                       ///< SkillTierID
        FT_BYTE,                                        ///< Availability
        FT_BYTE,                                        ///< MinLevel
        FT_INT,                                         ///< ClassMask
        FT_END
    };

    const char SkillLinefmt[] =
    {
        FT_INDEX,                                       ///< id
        FT_STRING,                                      ///< name
        FT_STRING,                                      ///< description
        FT_STRING,                                      ///< alternateVerb
        FT_SHORT,                                       ///< Flags
        FT_BYTE,                                        ///< categoryId
        FT_BYTE,                                        ///< canLink
        FT_INT,                                         ///< IconFileDataID
        FT_INT,                                         ///< parentSkillLineID
        FT_END
    };

    const char SoundKitEntryFmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_FLOAT,                                       ///< VolumeFloat
        FT_FLOAT,                                       ///< MinDistance
        FT_FLOAT,                                       ///< DistanceCutoff
        FT_SHORT,                                       ///< Flags
        FT_SHORT,                                       ///< SoundEntriesAdvancedID
        FT_BYTE,                                        ///< SoundType
        FT_BYTE,                                        ///< DialogType
        FT_BYTE,                                        ///< EAXDef
        FT_FLOAT,                                       ///< VolumeVariationPlus
        FT_FLOAT,                                       ///< VolumeVariationMinus
        FT_FLOAT,                                       ///< PitchVariationPlus
        FT_FLOAT,                                       ///< PitchVariationMinus
        FT_FLOAT,                                       ///< PitchAdjust
        FT_SHORT,                                       ///< BusOverwriteID
        FT_BYTE,                                        ///< Unk700
        FT_END
    };

    const char SummonPropertiesfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< Flags
        FT_INT,                                         ///< Category
        FT_INT,                                         ///< Faction
        FT_INT,                                         ///< Type
        FT_INT,                                         ///< Slot
        FT_END
    };

    const char TactKeyfmt[] =
    {
        FT_INDEX,                                       ///< Index
        FT_ARR_16, FT_BYTE,                             ///< Key
        FT_END
    };

    const char TalentEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< tDescriptionLang
        FT_INT,                                         ///< SpellID
        FT_INT,                                         ///< OverridesSpellID
        FT_SHORT,                                       ///< SpecID
        FT_BYTE,                                        ///< TierID
        FT_BYTE,                                        ///< ColumnIndex
        FT_BYTE,                                        ///< Flags
        FT_ARR_2, FT_BYTE,                              ///< CategoryMask[2]
        FT_BYTE,                                        ///< ClassID
        FT_END
    };

    const char TotemCategoryEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< tDescriptionLang
        FT_INT,                                         ///< OverridesSpellID
        FT_BYTE,
        FT_END
    };

    const char TransportAnimationfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< TimeSeg
        FT_ARR_3, FT_FLOAT,                             ///< X & Y & Z
        FT_BYTE,                                        ///< MovementId
        FT_INT_RELATIONSHIP,                            ///< TransportEntry
        FT_END
    };

    const char TransportRotationfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_INT,                                         ///< TimeSeg
        FT_ARR_4, FT_FLOAT,                             ///< X & Y & Z & W
        FT_INT_RELATIONSHIP,                            ///< TransportEntry
        FT_END
    };

    const char ToyEntryfmt[] =
    {
        FT_STRING,
        FT_INT,
        FT_BYTE,
        FT_BYTE,
        FT_INDEX,
        FT_END
    };

    const char VehicleEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< Flags
        FT_FLOAT,                                       ///< TurnSpeed
        FT_FLOAT,                                       ///< PitchSpeed
        FT_FLOAT,                                       ///< PitchMin
        FT_FLOAT,                                       ///< PitchMax
        FT_FLOAT,                                       ///< MouseLookOffsetPitch
        FT_FLOAT,                                       ///< CameraFadeDistScalarMin
        FT_FLOAT,                                       ///< CameraFadeDistScalarMax
        FT_FLOAT,                                       ///< CameraPitchOffset
        FT_FLOAT,                                       ///< FacingLimitRight
        FT_FLOAT,                                       ///< FacingLimitLeft
        FT_FLOAT,                                       ///< CameraYawOffset
        FT_ARR_8, FT_SHORT,                             ///< SeatID[MAX_VEHICLE_SEATS]
        FT_SHORT,                                       ///< VehicleUIIndicatorID
        FT_ARR_3, FT_SHORT,                             ///< PowerDisplayID[3]
        FT_BYTE,                                        ///< FlagsB
        FT_BYTE,                                        ///< UILocomotionType
        FT_INT,                                         ///< MissileTargetingID
        FT_END
    };

    const char VehicleSeatEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< m_flags;
        FT_INT,                                         ///< m_flagsB;
        FT_INT,                                         ///< m_flagsC;
        FT_ARR_3, FT_FLOAT,                             ///< AttachmentOffsetX & AttachmentOffsetY & AttachmentOffsetZ
        FT_FLOAT,                                       ///< EnterPreDelay;
        FT_FLOAT,                                       ///< EnterSpeed;
        FT_FLOAT,                                       ///< EnterGravity;
        FT_FLOAT,                                       ///< EnterMinDuration;
        FT_FLOAT,                                       ///< EnterMaxDuration;
        FT_FLOAT,                                       ///< EnterMinArcHeight;
        FT_FLOAT,                                       ///< EnterMaxArcHeight;
        FT_FLOAT,                                       ///< ExitPreDelay;
        FT_FLOAT,                                       ///< ExitSpeed;
        FT_FLOAT,                                       ///< ExitGravity;
        FT_FLOAT,                                       ///< ExitMinDuration;
        FT_FLOAT,                                       ///< ExitMaxDuration;
        FT_FLOAT,                                       ///< ExitMinArcHeight;
        FT_FLOAT,                                       ///< ExitMaxArcHeight;
        FT_FLOAT,                                       ///< PassengerYaw;
        FT_FLOAT,                                       ///< PassengerPitch;
        FT_FLOAT,                                       ///< PassengerRoll;
        FT_FLOAT,                                       ///< VehicleEnterAnimDelay;
        FT_FLOAT,                                       ///< VehicleExitAnimDelay;
        FT_FLOAT,                                       ///< CameraEnteringDelay;
        FT_FLOAT,                                       ///< CameraEnteringDuration
        FT_FLOAT,                                       ///< CameraExitingDelay;
        FT_FLOAT,                                       ///< CameraExitingDuration;
        FT_ARR_3, FT_FLOAT,                             ///< CameraOffsetX & CameraOffsetY & CameraOffsetZ
        FT_FLOAT,                                       ///< CameraPosChaseRate;
        FT_FLOAT,                                       ///< CameraFacingChaseRate;
        FT_FLOAT,                                       ///< CameraEnteringZoom;
        FT_FLOAT,                                       ///< CameraSeatZoomMin;
        FT_FLOAT,                                       ///< CameraSeatZoomMax;
        FT_INT,                                         ///< UISkinFileDataID;
        FT_SHORT,                                       ///< EnterAnimStart;
        FT_SHORT,                                       ///< EnterAnimLoop;
        FT_SHORT,                                       ///< RideAnimStart;
        FT_SHORT,                                       ///< RideAnimLoop;
        FT_SHORT,                                       ///< RideUpperAnimStart;
        FT_SHORT,                                       ///< RideUpperAnimLoop;
        FT_SHORT,                                       ///< ExitAnimStart;
        FT_SHORT,                                       ///< ExitAnimLoop;
        FT_SHORT,                                       ///< ExitAnimEnd;
        FT_SHORT,                                       ///< VehicleEnterAnim;
        FT_SHORT,                                       ///< VehicleExitAnim;
        FT_SHORT,                                       ///< VehicleRideAnimLoop;
        FT_SHORT,                                       ///< EnterAnimKitID;
        FT_SHORT,                                       ///< RideAnimKitID;
        FT_SHORT,                                       ///< ExitAnimKitID;
        FT_SHORT,                                       ///< VehicleEnterAnimKitID;
        FT_SHORT,                                       ///< VehicleRideAnimKitID;
        FT_SHORT,                                       ///< VehicleExitAnimKitID;
        FT_SHORT,                                       ///< CameraModeID;
        FT_BYTE,                                        ///< AttachmentID;
        FT_BYTE,                                        ///< PassengerAttachmentID;
        FT_BYTE,                                        ///< VehicleEnterAnimBone;
        FT_BYTE,                                        ///< VehicleExitAnimBone;
        FT_BYTE,                                        ///< VehicleRideAnimLoopBon
        FT_BYTE,                                        ///< VehicleAbilityDisplay;
        FT_INT,                                         ///< EnterUISoundID;
        FT_INT,                                         ///< ExitUISoundID;
        FT_END
    };

    const char VignetteEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< Name
        FT_FLOAT,                                       ///< X
        FT_FLOAT,                                       ///< Y
        FT_INT,                                         ///< Flags
        FT_INT,                                         ///< QuestFeedbackEffectId
        FT_INT,                                         ///< Unk
        FT_INT,                                         ///< Unk2
        FT_END
    };

    const char WorldSafeLocsEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< Name
        FT_ARR_3, FT_FLOAT,                             ///< Pos
        FT_FLOAT,                                       ///< facing
        FT_SHORT,                                       ///< MapID
        FT_END
    };

    const char AreaPOIEntryfmt[] =
    {
        FT_INDEX,                                       ///< Id
        FT_STRING,                                      ///< Name
        FT_STRING,                                      ///< Description
        FT_INT,                                         ///< Flag ?
        FT_ARR_3, FT_FLOAT,                             ///< Position
        FT_INT,                                         ///< Field1C
        FT_INT,                                         ///< Field20
        FT_SHORT,                                       ///< Map ID
        FT_SHORT,                                       ///< Field24
        FT_SHORT,                                       ///< Field28
        FT_SHORT,                                       ///< Field2A
        FT_BYTE,                                        ///< Field2C
        FT_BYTE,                                        ///< Field2D
        FT_INT,                                         ///< Player Condition ID
        FT_INT,                                         ///< Field30
        FT_INT,                                         ///< Field32
        FT_INT,                                         ///< Field33
        FT_END
    };

    const char ContributionEntryfmt[] =
    {
        FT_STRING,                                      ///< Description
        FT_STRING,                                      ///< Name
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< ManagedWorldStateInputID
        FT_ARR_4, FT_INT,                               ///< UiTextureAtlasMemberID
        FT_INT,                                         ///< OrderIndex
        FT_END
    };

    const char ManagedWorldStateEntryfmt[] =
    {
        FT_INT,                                         ///< CurrentStageWorldStateID
        FT_INT,                                         ///< ProgressWorldStateID
        FT_INT,                                         ///< UpTimeSecs
        FT_INT,                                         ///< DownTimeSecs
        FT_INT,                                         ///< OccurencesWorldStateID
        FT_INT,                                         ///< AccumulationStateTargetValue
        FT_INT,                                         ///< DepletionStateTargetValue
        FT_INT,                                         ///< AccumulationAmountPerMinute
        FT_INT,                                         ///< DepletionAmountPerMinute
        FT_INDEX,                                       ///< ID
        FT_END
    };

    const char ManagedWorldStateBuffEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< OccurenceValue
        FT_INT,                                         ///< BuffSpellID;
        FT_INT,                                         ///< PlayerConditionID;
        FT_INT_RELATIONSHIP,                            ///< RelationshipData;
        FT_END
    };

    const char ManagedWorldStateInputEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_INT,                                         ///< ManageWorldStateID
        FT_INT,                                         ///< QuestID
        FT_INT,                                         ///< ValidInputConditionID
        FT_END
    };
#pragma  endregion

////////////////////////////////////////////////////////////////////////////////
/// WorldState DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region WorldState
    const char WorldStateExpressionEntryfmt[] =
    {
        FT_INDEX,                                       ///< ID
        FT_STRING,                                      ///< Expression
        FT_END
    };
#pragma endregion