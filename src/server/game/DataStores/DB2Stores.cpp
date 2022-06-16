//////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////

#include "AchievementMgr.h"
#include "DB2Stores.h"
#include "Log.h"
#include "SharedDefines.h"
#include "SpellMgr.h"
#include "DB2fmt.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "TransportMgr.h"
#include "Battleground.h"
#include "Player.h"
#include <map>
#include "Common.h"
#include "WowTime.hpp"
#include <ace/OS_NS_time.h>
#include <iostream>
#include <fstream>

/// Lookup table by DB2 hash
std::map<uint32, DB2StorageBase*> sDB2PerHash;
#define DEFAULT_COUNT(a) (sizeof(a) / sizeof(DB2FieldDefault))
#define DEFAULTS(a) a, DEFAULT_COUNT(a)

//////////////////////////////////////////////////////////////////////////
/// Spell DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Spell
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<OverrideSpellDataEntry>               sOverrideSpellDataStore(OverrideSpellDataEntryfmt, false);
    DB2Storage<SpecializationSpellEntry>             sSpecializationSpellStore(SpecializationSpellEntryfmt, false);
    DB2Storage<SpellEntry>                           sSpellStore(SpellEntryfmt, false);
    DB2Storage<SpellAuraOptionsEntry>                sSpellAuraOptionsStore(SpellAuraOptionsEntryfmt, false);
    DB2Storage<SpellAuraRestrictionsEntry>           sSpellAuraRestrictionsStore(SpellAuraRestrictionsEntryfmt, false);
    DB2Storage<SpellCastingRequirementsEntry>        sSpellCastingRequirementsStore(SpellCastingRequirementsEntryfmt, false);
    DB2Storage<SpellCastTimesEntry>                  sSpellCastTimesStore(SpellCastTimeEntryfmt, false);
    DB2Storage<SpellCategoriesEntry>                 sSpellCategoriesStore(SpellCategoriesEntryfmt, false);
    DB2Storage<SpellCategoryEntry>                   sSpellCategoryStore(SpellCategoryEntryfmt, false);
    DB2Storage<SpellClassOptionsEntry>               sSpellClassOptionsStore(SpellClassOptionsEntryfmt, false);
    DB2Storage<SpellCooldownsEntry>                  sSpellCooldownsStore(SpellCooldownsEntryfmt, false);
    DB2Storage<SpellDurationEntry>                   sSpellDurationStore(SpellDurationEntryfmt, false);
    DB2Storage<SpellEffectEntry>                     sSpellEffectStore(SpellEffectEntryfmt, false);
    DB2Storage<SpellEquippedItemsEntry>              sSpellEquippedItemsStore(SpellEquippedItemsEntryfmt, false);
    DB2Storage<SpellFocusObjectEntry>                sSpellFocusObjectStore(SpellFocusObjectEntryfmt, false);
    DB2Storage<SpellInterruptsEntry>                 sSpellInterruptsStore(SpellInterruptsEntryfmt, false);
    DB2Storage<SpellItemEnchantmentConditionEntry>   sSpellItemEnchantmentConditionStore(SpellItemEnchantmentConditionEntryfmt, false);
    DB2Storage<SpellItemEnchantmentEntry>            sSpellItemEnchantmentStore(SpellItemEnchantmentEntryfmt, false);
    DB2Storage<SpellLevelsEntry>                     sSpellLevelsStore(SpellLevelsEntryfmt, false);
    DB2Storage<SpellMiscEntry>                       sSpellMiscStore(SpellMiscEntryfmt, false);
    DB2Storage<SpellPowerEntry>                      sSpellPowerStore(SpellPowerEntryfmt, false);
    DB2Storage<SpellProcsPerMinuteEntry>             sSpellProcsPerMinuteStore(SpellProcsPerMinuteEntryfmt, false);
    DB2Storage<SpellProcsPerMinuteModEntry>          sSpellProcsPerMinuteModStore(SpellProcsPerMinuteModEntryfmt, false);
    DB2Storage<SpellRadiusEntry>                     sSpellRadiusStore(SpellRadiusEntryfmt, false);
    DB2Storage<SpellRangeEntry>                      sSpellRangeStore(SpellRangeEntryfmt, false);
    DB2Storage<SpellReagentsEntry>                   sSpellReagentsStore(SpellReagentsEntryfmt, false);
    DB2Storage<SpellReagentsCurrencyEntry>           sSpellReagentsCurrencyStore(SpellReagentsCurrencyEntryfmt, false);
    DB2Storage<SpellScalingEntry>                    sSpellScalingStore(SpellScalingEntryfmt, false);
    DB2Storage<SpellShapeshiftEntry>                 sSpellShapeshiftStore(SpellShapeshiftEntryfmt, false);
    DB2Storage<SpellShapeshiftFormEntry>             sSpellShapeshiftFormStore(SpellShapeshiftFormEntryfmt, false);
    DB2Storage<SpellTargetRestrictionsEntry>         sSpellTargetRestrictionsStore(SpellTargetRestrictionsEntryfmt, false);
    DB2Storage<SpellTotemsEntry>                     sSpellTotemsStore(SpellTotemsEntryfmt, false);
    //DB2Storage<SpellVisualEntry>                     sSpellVisualStore(SpellVisualEntryfmt, true, DEFAULTS(SpellVisualDefaultFields)); ///  Completely changed, also why was it ever loaded ??? Useless server-side
    DB2Storage<SpellXSpellVisualEntry>               sSpellXSpellVisualStore(SpellXSpellVisualEntryfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    PetFamilySpellsStore         sPetFamilySpellsStore;
    SpellEffectMap               sSpellEffectMap;
    SpellProcsPerMinuteModMap    sSpellProcsPerMinuteMods;
    SpellReagentMap              sSpellReagentMap;
    SpellAuraRestrictionMap      sSpellAuraRestrictionMap;
    SpellCastingRequirementsMap  sSpellCastingRequirementsMap;
    SpellClassOptionsMap         sSpellClassOptionsMap;
    SpellEquippedItemsMap        sSpellEquippedItemsMap;
    SpellReagentsCurrencyMap     sSpellReagentsCurrencyMap;
    SpellScalingMap              sSpellScalingMap;
    SpellShapeshiftMap           sSpellShapeshiftMap;
    SpellTotemsMap               sSpellTotemsMap;
    std::map<uint32, std::set<uint32>> sSpellAuraOptionDifficultyBySpellId;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    /// Find a specific spell effect by index and difficulty
    /// @p_SpellID      : Looking for spell ID
    /// @p_EffectIndex  : Effect index for the spell
    /// @p_Difficulty   : Looking difficulty
    SpellEffectEntry const* GetSpellEffectEntry(uint32 p_SpellID, uint32 p_EffectIndex, uint32 p_Difficulty)
    {
        SpellEffectMap::iterator l_It = sSpellEffectMap.find(p_SpellID);
        if (l_It == sSpellEffectMap.end())
            return nullptr;

        if (p_Difficulty != DifficultyNone)
        {
            /// If spell isn't available in difficulty we want, check fallback difficulty ...
            DifficultyEntry const* l_Difficulty = sDifficultyStore.LookupEntry(p_Difficulty);
            while (l_Difficulty != nullptr)
            {
                SpellEffectEntry const* l_Effect = l_It->second.effects[l_Difficulty->ID][p_EffectIndex];
                if (l_Effect != nullptr)
                    return l_Effect;

                l_Difficulty = sDifficultyStore.LookupEntry(l_Difficulty->FallbackDifficultyID);
            }
        }

        return l_It->second.effects[DifficultyNone][p_EffectIndex];
    }

    /// Get list of Procs Pet Minute mods for a PPM info
    /// @p_SpellPPM_ID : PPM ID
    SpellProcsPerMinuteVector GetSpellProcsPerMinuteMods(uint32 p_SpellPPM_ID)
    {
        auto l_It = sSpellProcsPerMinuteMods.find(p_SpellPPM_ID);
        if (l_It != sSpellProcsPerMinuteMods.end())
            return l_It->second;

        return std::vector<SpellProcsPerMinuteModEntry const*>();
    }

    std::vector<uint32> const* GetGlyphBindableSpells(uint32 p_GlyphPropID)
    {
        auto l_Itr = g_GlyphBindableSpells.find(p_GlyphPropID);
        if (l_Itr != g_GlyphBindableSpells.end())
            return &l_Itr->second;

        return nullptr;
    }

    std::vector<uint32> const* GetGlyphRequiredSpecs(uint32 p_GlyphPropID)
    {
        auto l_Itr = g_GlyphRequiredSpecs.find(p_GlyphPropID);
        if (l_Itr != g_GlyphRequiredSpecs.end())
            return &l_Itr->second;

        return nullptr;
    }

    void LoadSpellAuraOptionBySpellIdDifficulty()
    {
        for (uint32 l_I = 0; l_I < sSpellAuraOptionsStore.GetNumRows(); ++l_I)
        {
            SpellAuraOptionsEntry const* l_SpellAuraOptions = sSpellAuraOptionsStore.LookupEntry(l_I);
            if (!l_SpellAuraOptions)
                continue;

            sSpellAuraOptionDifficultyBySpellId[l_SpellAuraOptions->SpellID].insert(l_SpellAuraOptions->DifficultyID);
        }
    }

    uint32 GetMaxDifficultyOptionBelowCurrentDifficulty(uint32 p_SpellId, uint32 p_Difficulty)
    {
        if (sSpellAuraOptionDifficultyBySpellId.empty())
            LoadSpellAuraOptionBySpellIdDifficulty();

        auto l_SpellAuraOptionByDifficulty = sSpellAuraOptionDifficultyBySpellId.find(p_SpellId);
        if (l_SpellAuraOptionByDifficulty == sSpellAuraOptionDifficultyBySpellId.end())
            return 0;

        uint32 l_MaxDifficultyFoundBelowCurrent = 0;
        for (auto l_Difficulty : sSpellAuraOptionDifficultyBySpellId[p_SpellId])
        {
            if (l_Difficulty > l_MaxDifficultyFoundBelowCurrent && l_Difficulty <= p_Difficulty)
                l_MaxDifficultyFoundBelowCurrent = l_Difficulty;
        }

        return l_MaxDifficultyFoundBelowCurrent;
    }
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Garrison DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Garrison

    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage <GarrSiteLevelEntry>             sGarrSiteLevelStore(GarrSiteLevelEntryfmt, false);
    DB2Storage <GarrSiteLevelPlotInstEntry>     sGarrSiteLevelPlotInstStore(GarrSiteLevelPlotInstEntryfmt, false);
    DB2Storage <GarrPlotInstanceEntry>          sGarrPlotInstanceStore(GarrPlotInstanceEntryfmt, false);
    DB2Storage <GarrPlotEntry>                  sGarrPlotStore(GarrPlotEntryfmt, false);
    DB2Storage <GarrPlotUICategoryEntry>        sGarrPlotUICategoryStore(GarrPlotUICategoryEntryfmt, false);
    DB2Storage <GarrMissionEntry>               sGarrMissionStore(GarrMissionEntryfmt, false);
    DB2Storage <GarrMissionXEncouterEntry>      sGarrMissionXEncouterStore(GarrMissionXEncouterEntryfmt, false);
    DB2Storage <GarrBuildingEntry>              sGarrBuildingStore(GarrBuildingEntryfmt, false);
    DB2Storage <GarrPlotBuildingEntry>          sGarrPlotBuildingStore(GarrPlotBuildingEntryfmt, false);
    DB2Storage <GarrFollowerEntry>              sGarrFollowerStore(GarrFollowerEntryfmt, false);
    DB2Storage <GarrClassSpecEntry>             sGarrClassSpecStore(GarrClassSpecEntryfmt, false);
    DB2Storage <GarrFollowerTypeEntry>          sGarrFollowerTypeStore(GarrFollowerTypefmt, false);
    DB2Storage <GarrFollSupportSpellEntry>      sGarrFollSupportSpellStore(GarrFollSupportSpellfmt, false);
    DB2Storage <GarrAbilityEntry>               sGarrAbilityStore(GarrAbilityEntryfmt, false);
    DB2Storage <GarrAbilityEffectEntry>         sGarrAbilityEffectStore(GarrAbilityEffectEntryfmt, false);
    DB2Storage <GarrFollowerXAbilityEntry>      sGarrFollowerXAbilityStore(GarrFollowerXAbilityEntryfmt, false);
    DB2Storage <GarrBuildingPlotInstEntry>      sGarrBuildingPlotInstStore(GarrBuildingPlotInstEntryfmt, false);
    DB2Storage <GarrMechanicTypeEntry>          sGarrMechanicTypeStore(GarrMechanicTypeEntryfmt, false);
    DB2Storage <GarrMechanicEntry>              sGarrMechanicStore(GarrMechanicEntryfmt, false);
    DB2Storage <GarrEncouterXMechanicEntry>     sGarrEncouterXMechanicStore(GarrEncouterXMechanicEntryfmt, false);
    DB2Storage <GarrEncounterSetXEncounterEntry> sGarrEncounterSetXEncounterStore(GarrEncounterSetXEncounterfmt, false);
    DB2Storage <GarrMechanicSetXMechanicEntry>  sGarrMechanicSetXMechanicStore(GarrMechanicSetXMechanicfmt, false);
    DB2Storage <GarrFollowerLevelXPEntry>       sGarrFollowerLevelXPStore(GarrFollowerLevelXPEntryfmt, false);
    DB2Storage <GarrSpecializationEntry>        sGarrSpecializationStore(GarrSpecializationEntryfmt, false);
    DB2Storage <GarrTalentEntry>                sGarrTalentStore(GarrTalentEntryfmt, false);
    DB2Storage <GarrTalentTreeEntry>            sGarrTalentTreeStore(GarrTalentTreeEntryfmt, false);
    DB2Storage <GarrTypeEntry>                  sGarrTypeStore(GarrTypeEntryfmt, false);
    DB2Storage <CharShipmentEntry>              sCharShipmentStore(CharShipmentEntryfmt, false);
    DB2Storage <CharShipmentContainerEntry>     sCharShipmentContainerStore(CharShipmentContainerEntryfmt, false);
    DB2Storage <GarrItemLevelUpgradeDataEntry>  sGarrItemLevelUpgradeDataStore(GarrItemLevelUpgradeDataEntryfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    GarrTalentTreeIDPerClass                                    s_GarrTalentTreeIDPerClass;
    std::map<uint32, GarrTalentTree>                            s_GarrTalentTreeByTreeID;
    std::map<uint32, std::set<GarrAbilityEffectEntry const*>>   s_GarrAbilityEffectByEffectType;
    std::map<uint32, GarrTalentEntry const*>                    s_GarrTalentByGarrAbilityID;
    std::map<uint32, GarrMechanicList>                          s_GarrMechanicListBySetID;
    std::map<uint32, GarrEncounterList>                         s_GarrEncounterListBySetID;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    const GarrTalentEntry* GetGarrTalentEntryByGarrAbilityID(uint32 p_GarrAbilityId)
    {
        auto l_Itr = s_GarrTalentByGarrAbilityID.find(p_GarrAbilityId);
        if (l_Itr == s_GarrTalentByGarrAbilityID.end())
            return nullptr;

        return (*l_Itr).second;
    }

    const std::set<GarrAbilityEffectEntry const*> GetGarrAbilityEffectEntryByEffectType(uint32 p_EffectType)
    {
        auto l_Itr = s_GarrAbilityEffectByEffectType.find(p_EffectType);
        if (l_Itr == s_GarrAbilityEffectByEffectType.end())
            return {};

        return (*l_Itr).second;
    }

    const GarrTalentTree* GetGarrisonTalentTreeByClassID(uint32 p_GarrisonType, uint32 p_Class)
    {
        if (s_GarrTalentTreeIDPerClass.find(p_GarrisonType) == s_GarrTalentTreeIDPerClass.end())
            return nullptr;

        auto& l_TalentTressPerGarrison = s_GarrTalentTreeIDPerClass[p_GarrisonType];

        if (l_TalentTressPerGarrison.find(p_Class) == l_TalentTressPerGarrison.end())
            return nullptr;

        uint32 l_TalentTreeID = l_TalentTressPerGarrison[p_Class];

        if (s_GarrTalentTreeByTreeID.find(l_TalentTreeID) == s_GarrTalentTreeByTreeID.end())
            return nullptr;

        return &s_GarrTalentTreeByTreeID[l_TalentTreeID];
    }

    std::vector<const GarrTalentEntry*> const* GetGarrisonTalentsByClassAtRank(uint32 p_GarrisonType, uint32 p_Class, uint32 p_Rank)
    {
        auto l_Tree = GetGarrisonTalentTreeByClassID(p_GarrisonType, p_Class);

        if (!l_Tree)
            return nullptr;

        return l_Tree->size() <= p_Rank ? nullptr : &(*l_Tree)[p_Rank];
    }

    GarrMechanicList const* GetGarrMechanicListBySetID(uint32 p_SetID)
    {
        auto l_Itr = s_GarrMechanicListBySetID.find(p_SetID);
        return l_Itr != s_GarrMechanicListBySetID.end() ? &l_Itr->second : nullptr;
    }

    GarrEncounterList const* GetGarrEncounterListBySetID(uint32 p_SetID)
    {
        auto l_Itr = s_GarrEncounterListBySetID.find(p_SetID);
        return l_Itr != s_GarrEncounterListBySetID.end() ? &l_Itr->second : nullptr;
    }
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// BattlePet DB2
//////////////////////////////////////////////////////////////////////////
#pragma region BattlePet
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<BattlePetAbilityEntry>           sBattlePetAbilityStore(BattlePetAbilityfmt, false);
    DB2Storage<BattlePetAbilityEffectEntry>     sBattlePetAbilityEffectStore(BattlePetAbilityEffectfmt, false);
    DB2Storage<BattlePetAbilityTurnEntry>       sBattlePetAbilityTurnStore(BattlePetAbilityTurnfmt, false);
    DB2Storage<BattlePetAbilityStateEntry>      sBattlePetAbilityStateStore(BattlePetAbilityStatefmt, false);
    DB2Storage<BattlePetStateEntry>             sBattlePetStateStore(BattlePetStatefmt, false);
    DB2Storage<BattlePetEffectPropertiesEntry>  sBattlePetEffectPropertiesStore(BattlePetEffectPropertiesfmt, false);
    DB2Storage<BattlePetBreedQualityEntry>      sBattlePetBreedQualityStore(BattlePetBreedQualityfmt, false);
    DB2Storage<BattlePetBreedStateEntry>        sBattlePetBreedStateStore(BattlePetBreedStatefmt, false);
    DB2Storage<BattlePetSpeciesEntry>           sBattlePetSpeciesStore(BattlePetSpeciesfmt, false);
    DB2Storage<BattlePetSpeciesStateEntry>      sBattlePetSpeciesStateStore(BattlePetSpeciesStatefmt, false);
    DB2Storage<BattlePetSpeciesXAbilityEntry>   sBattlePetSpeciesXAbilityStore(BattlePetSpeciesXAbilityfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    BattlePetSpeciesXAbilityBySpecies           sBattlePetAbilitiesBySpecies;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Achievement DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Achievement
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<AchievementEntry>               sAchievementStore(Achievementfmt, false);
    DB2Storage <AchievementCategoryEntry>      sAchievementCategoryStore(AchievementCategoryfmt);
    DB2Storage<CriteriaEntry>                  sCriteriaStore(Criteriafmt, false);
    DB2Storage<CriteriaTreeEntry>              sCriteriaTreeStore(CriteriaTreefmt, false);
    DB2Storage<ModifierTreeEntry>              sModifierTreeStore(ModifierTreefmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Taxi DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Taxi
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<TaxiNodesEntry>                 sTaxiNodesStore(TaxiNodesFormat, false);
    DB2Storage<TaxiPathEntry>                  sTaxiPathStore(TaxiPathFormat, false);
    DB2Storage<TaxiPathNodeEntry>              sTaxiPathNodeStore(TaxiPathNodeFormat, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    TaxiMask                 sTaxiNodesMask;
    TaxiMask                 sOldContinentsNodesMask;
    TaxiMask                 sHordeTaxiNodesMask;
    TaxiMask                 sAllianceTaxiNodesMask;
    TaxiMask                 sArgusTaxiMask;
    TaxiPathSetBySource      sTaxiPathSetBySource;
    TaxiPathNodesByPath      sTaxiPathNodesByPath;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    /// Determinate an alternate map Position
    /// @p_MapID    : Source map ID
    /// @p_X        : Source position x
    /// @p_Y        : Source position y
    /// @p_Z        : Source position z
    /// @p_NewMapID : Output map id
    /// @p_NewPosX  : Output position x
    /// @p_NewPosY  : Output position z
    void DeterminateAlternateMapPosition(uint32 p_MapID, float p_X, float p_Y, float p_Z, uint32* p_NewMapID /*= nullptr*/, float* p_NewPosX /*= nullptr*/, float* p_NewPosY /*= nullptr*/)
    {
        ASSERT(p_NewMapID || (p_NewPosX && p_NewPosY));

        WorldMapTransformsEntry const* l_Transformation = nullptr;

        for (uint32 l_I = 0; l_I < sWorldMapTransformsStore.GetNumRows(); ++l_I)
        {
            WorldMapTransformsEntry const* l_Transform = sWorldMapTransformsStore.LookupEntry(l_I);

            if (!l_Transform)
                continue;

            if (l_Transform->MapID != p_MapID)
                continue;

            if (l_Transform->RegionMinX > p_X || l_Transform->RegionMaxX < p_X)
                continue;
            if (l_Transform->RegionMinY > p_Y || l_Transform->RegionMaxY < p_Y)
                continue;
            if (l_Transform->RegionMinZ > p_Z || l_Transform->RegionMaxZ < p_Z)
                continue;

            l_Transformation = l_Transform;
            break;
        }

        if (!l_Transformation)
        {
            if (p_NewMapID)
                *p_NewMapID = p_MapID;

            if (p_NewPosX && p_NewPosY)
            {
                *p_NewPosX = p_X;
                *p_NewPosY = p_Y;
            }
            return;
        }

        if (p_NewMapID)
            *p_NewMapID = l_Transformation->NewMapID;

        if (!p_NewPosX || !p_NewPosY)
            return;

        if (l_Transformation->RegionScale > 0.0f && l_Transformation->RegionScale < 1.0f)
        {
            p_X = (p_X - l_Transformation->RegionMinX) * l_Transformation->RegionScale + l_Transformation->RegionMinX;
            p_Y = (p_Y - l_Transformation->RegionMinY) * l_Transformation->RegionScale + l_Transformation->RegionMinY;
        }

        *p_NewPosX = p_X + l_Transformation->RegionOffsetX;
        *p_NewPosY = p_Y + l_Transformation->RegionOffsetY;
    }
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Artifact DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Artifact
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<ArtifactEntry>                  sArtifactStore(Aritfactfmt, false);
    DB2Storage<ArtifactAppearanceEntry>        sArtifactAppearanceStore(ArtifactAppearancefmt, false);
    DB2Storage<ArtifactAppearanceSetEntry>     sArtifactAppearanceSetStore(ArtifactAppearanceSetfmt, false);
    DB2Storage<ArtifactPowerEntry>             sArtifactPowerStore(ArtifactPowerfmt, false);
    DB2Storage<ArtifactPowerLinkEntry>         sArtifactPowerLinkStore(ArtifactPowerLinkfmt, false);
    DB2Storage<ArtifactPowerRankEntry>         sArtifactPowerRankStore(ArtifactPowerRankfmt, false);
    DB2Storage<ArtifactCategoryEntry>          sArtifactCategoryStore(ArtifactCategoryfmt, false);
    DB2Storage<ArtifactPowerPickerEntry>       sArtifactPowerPickerStore(ArtifactPowerPickerfmt, false);
    DB2Storage<ArtifactQuestXPEntry>           sArtifactQuestXPStore(ArtifactQuestXPfmt, false);
    DB2Storage<RelicTalentEntry>               sRelicTalentStore(RelicTalentfmt, false);


    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    ArtifactPowerRanksContainer _artifactPowerRanks;
    std::multimap<uint32, ArtifactPowerRankEntry const*> _artifactPowerRanksBySpellId;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    ArtifactPowerRankEntry const* GetArtifactPowerRank(uint32 p_ArtifactPowerId, uint8 p_Rank)
    {
        auto itr = _artifactPowerRanks.find({ p_ArtifactPowerId, p_Rank });
        if (itr != _artifactPowerRanks.end())
            return itr->second;

        return nullptr;
    }

    bool IsArtifactSpell(uint32 p_spellId)
    {
        auto itr = _artifactPowerRanksBySpellId.find(p_spellId);
        if (itr != _artifactPowerRanksBySpellId.end())
            return true;

        return false;
    }

    ArtifactPowerRankEntry const* GetArtifactPowerRankBySpellID(uint32 p_spellId, Player const* p_Player /*= nullptr*/)
    {
        auto l_Range = _artifactPowerRanksBySpellId.equal_range(p_spellId);
        for (auto l_Itr = l_Range.first; l_Itr != l_Range.second; ++l_Itr)
        {
            if (!p_Player)
                return l_Itr->second;

            if (MS::Artifact::Manager const* l_Manager = p_Player->GetCurrentlyEquippedArtifact())
            {
                if (ArtifactPowerEntry const* l_PowerEntry = sArtifactPowerStore.LookupEntry(l_Itr->second->ArtifactPowerID))
                {
                    if (l_PowerEntry->ArtifactID == l_Manager->GetArtifactId())
                        return l_Itr->second;
                }
            }
        }

        return nullptr;
    }

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// SceneScript DB2
//////////////////////////////////////////////////////////////////////////
#pragma region SceneScript
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<SceneScriptPackageEntry>        sSceneScriptPackageStore(SceneScriptPackageEntryfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// WebBrowser DB2
//////////////////////////////////////////////////////////////////////////
#pragma region WebBrowser
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<WbAccessControlListEntry>       sWbAccessControlListStore(WbAccessControlListfmt, false);
    DB2Storage<WbCertWhitelistEntry>           sWbCertWhitelistStore(WbCertWhitelistfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Item DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Item

    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<ItemPriceBaseEntry>             sItemPriceBaseStore(ItemPriceBasefmt, false);
    DB2Storage<ItemClassEntry>                 sItemClassStore(ItemClassfmt, false);
    DB2Storage<ItemDisenchantLootEntry>        sItemDisenchantLootStore(ItemDisenchantLootfmt, false);
    DB2Storage<ItemLimitCategoryEntry>         sItemLimitCategoryStore(ItemLimitCategoryEntryfmt, false);
    DB2Storage<ItemLimitCategoryConditionEntry>sItemLimitCategoryConditionStore(ItemLimitCategoryConditionEntryfmt, false);
    DB2Storage<ItemRandomPropertiesEntry>      sItemRandomPropertiesStore(ItemRandomPropertiesfmt, false);
    DB2Storage<ItemRandomSuffixEntry>          sItemRandomSuffixStore(ItemRandomSuffixfmt, false);
    DB2Storage<ItemSpecEntry>                  sItemSpecStore(ItemSpecEntryfmt, false);
    DB2Storage<ItemSpecOverrideEntry>          sItemSpecOverrideStore(ItemSpecOverrideEntryfmt, false);
    DB2Storage<ItemEntry>                      sItemStore(Itemfmt, false);
    DB2Storage<ItemBonusEntry>                 sItemBonusStore(ItemBonusfmt, false);
    DB2Storage<ItemBonusTreeNodeEntry>         sItemBonusTreeNodeStore(ItemBonusTreeNodefmt, false);
    DB2Storage<ItemBonusListLevelDeltaEntry>   sItemBonusListLevelDeltaStore(ItemBonusListLevelDeltaFmt, false);
    DB2Storage<ItemXBonusTreeEntry>            sItemXBonusTreeStore(ItemXBonusTreefmt, false);
    DB2Storage<ItemCurrencyCostEntry>          sItemCurrencyCostStore(ItemCurrencyCostfmt, false);
    DB2Storage<ItemExtendedCostEntry>          sItemExtendedCostStore(ItemExtendedCostEntryfmt, false);
    DB2Storage<ItemSparseEntry>                sItemSparseStore(ItemSparsefmt, false);
    DB2Storage<ItemEffectEntry>                sItemEffectStore(ItemEffectFmt, false);
    DB2Storage<ItemModifiedAppearanceEntry>    sItemModifiedAppearanceStore(ItemModifiedAppearanceFmt, false);
    DB2Storage<ItemAppearanceEntry>            sItemAppearanceStore(ItemAppearanceFmt, false);
    DB2Storage<TransmogSetItemEntry>           sTransmogSetItemStore(TransmogSetItemFmt, false);
    DB2Storage<TransmogSetEntry>               sTransmogSetStore(TransmogSetFmt, false);
    DB2Storage<ItemUpgradeEntry>               sItemUpgradeStore(ItemUpgradeEntryfmt, false);
    DB2Storage<ItemChildEquipmentEntry>        sItemChildEquipmentStore(ItemChildEquipmentfmt, false);
    DB2Storage<ItemSetSpellEntry>              sItemSetSpellStore(ItemSetSpellFmt, false);
    DB2Storage<ItemArmorQualityEntry>          sItemArmorQualityStore(ItemArmorQualityfmt, false);
    DB2Storage<ItemArmorShieldEntry>           sItemArmorShieldStore(ItemArmorShieldfmt, false);
    DB2Storage<ItemArmorTotalEntry>            sItemArmorTotalStore(ItemArmorTotalfmt, false);
    DB2Storage<ItemBagFamilyEntry>             sItemBagFamilyStore(ItemBagFamilyfmt, false);
    DB2Storage<ItemDamageEntry>                sItemDamageAmmoStore(ItemDamagefmt, false);
    DB2Storage<ItemDamageEntry>                sItemDamageOneHandStore(ItemDamagefmt, false);
    DB2Storage<ItemDamageEntry>                sItemDamageOneHandCasterStore(ItemDamagefmt, false);
    DB2Storage<ItemDamageEntry>                sItemDamageTwoHandStore(ItemDamagefmt, false);
    DB2Storage<ItemDamageEntry>                sItemDamageTwoHandCasterStore(ItemDamagefmt, false);
    DB2Storage<ItemSetEntry>                   sItemSetStore(ItemSetEntryfmt, false);
    DB2Storage<ItemSearchNameEntry>            sItemSearchNameStore(ItemSearchNameEntryfmt, false);
    DB2Storage<RulesetItemUpgradeEntry>        sRulesetItemUpgradeStore(RulesetItemUpgradeEntryfmt, false);
    DB2Storage<PvpItemEntry>                   sPvpItemStore(PvpItemfmt, false);
    DB2Storage<PvpScalingEffectEntry>          sPvpScalingEffectStore(PvpScalingEffectfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    ItemSetSpellsByItemID                                       sItemSetSpellsByItemIDStore;
    std::map<uint32, std::vector<uint32>>                       sItemEffectsByItemID;
    std::map<uint32, std::vector<ItemBonusEntry const*>>        sItemBonusesByID;
    std::map<uint32, std::vector<ItemXBonusTreeEntry const*>>   sItemBonusTreeByID;
    std::map<uint32, uint32>                                    g_PvPItemStoreLevels;
    ItemModifiedAppearanceByItemContainer                       _itemModifiedAppearancesByItem;
    ItemModifiedAppearanceByItemContainer                       _itemDefaultAppearancesByItem;
    TransmogSetItemByTransmogSetContainer                       g_TransmogSetItemByTransmogSet;
    ItemSpecOverridesContainer                                  _itemSpecOverrides;
    ItemBonusListLevelDeltaContainer                            g_ItemLevelDeltaToBonusListContainer;
    ItemChildEquipmentContainer                                 g_itemChildEquipment;
    ItemChildIdSet                                              g_ItemChildIds;
    std::unordered_map<uint32, uint32>                          g_ItemIDByArtifactID;
    std::unordered_map<uint32, uint32>                          g_ItemIDBySpec;
    ItemClassByOldEnumContainer                                 g_itemClassByOldEnum;
    PvpScalingPerSpecs                                          g_PvpScalingPerSpecs;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    /// Get item bonus by item ID
    /// @p_Id : Item id
    std::vector<ItemBonusEntry const*> const* GetItemBonusesByID(uint32 p_Id)
    {
        std::map<uint32, std::vector<ItemBonusEntry const*>>::const_iterator l_It = sItemBonusesByID.find(p_Id);
        return l_It != sItemBonusesByID.end() ? &(l_It->second) : nullptr;
    }

     ItemClassEntry const* GetItemClassByOldEnum(uint32 p_ItemClass)
     {
         return g_itemClassByOldEnum[p_ItemClass];
     }

    /// Get display id by item id and appearance mod ID
    /// @p_ItemID           : Item id
    /// @p_AppearanceModID  : Appearance modifier ID
    uint32 GetItemDisplayID(uint32 p_ItemID, uint32 p_AppearanceModID)
    {
        if (ItemModifiedAppearanceEntry const* l_ModifiedAppearance = GetItemModifiedAppearance(p_ItemID, p_AppearanceModID))
        {
            if (ItemAppearanceEntry const* l_ItemAppearance = sItemAppearanceStore.LookupEntry(l_ModifiedAppearance->AppearanceID))
                return l_ItemAppearance->DisplayID;
        }

        return 0;
    }

    ItemModifiedAppearanceEntry const* GetItemModifiedAppearance(uint32 p_ItemID, uint32 p_AppearanceModId)
    {
        auto l_It = _itemModifiedAppearancesByItem.find(p_ItemID | (p_AppearanceModId << 24));
        if (l_It != _itemModifiedAppearancesByItem.end())
            return l_It->second;

        /// Fall back to unmodified appearance
        l_It = _itemDefaultAppearancesByItem.find(p_ItemID);
        if (l_It != _itemDefaultAppearancesByItem.end())
            return l_It->second;

        return nullptr;
    }

    ItemModifiedAppearanceEntry const* GetDefaultItemModifiedAppearance(uint32 p_ItemID)
    {
        auto l_It = _itemDefaultAppearancesByItem.find(p_ItemID);
        if (l_It != _itemDefaultAppearancesByItem.end())
            return l_It->second;

        return nullptr;
    }

    std::vector<ItemSpecOverrideEntry const*> const* GetItemSpecOverrides(uint32 p_ItemID)
    {
        auto itr = _itemSpecOverrides.find(p_ItemID);
        if (itr != _itemSpecOverrides.end())
            return &itr->second;

        return nullptr;
    }

    /// Get item children
    /// @p_ItemID
    ItemChildEquipmentEntry const* GetItemChildEquipment(uint32 p_ItemID)
    {
        auto itr = g_itemChildEquipment.find(p_ItemID);
        if (itr != g_itemChildEquipment.end())
            return itr->second;

        return nullptr;
    }

    uint32 GetParentItemIDIfExists(uint32 p_ItemID)
    {
        uint32 l_ParentItemId = 0;
        for (ItemChildEquipmentContainer::iterator l_ChildItr = g_itemChildEquipment.begin(); l_ChildItr != g_itemChildEquipment.end(); ++l_ChildItr)
        {
            if (p_ItemID == (*l_ChildItr).second->ChildItemID)
            {
                l_ParentItemId = (*l_ChildItr).first;
                break;
            }
        }

        return l_ParentItemId;
    }

    uint32 GetItemIDByArtifactID(uint32 p_ArtifactID)
    {
        auto l_Itr = g_ItemIDByArtifactID.find(p_ArtifactID);
        return l_Itr != g_ItemIDByArtifactID.end() ? l_Itr->second : 0;
    }

    uint32 GetArtifactItemIDEntryBySpec(uint32 p_SpecID)
    {
        auto l_Itr = g_ItemIDBySpec.find(p_SpecID);
        return l_Itr != g_ItemIDBySpec.end() ? l_Itr->second : 0;
    }

    uint32 GetItemBonusListForItemLevelDelta(int16 p_Delta)
    {
        auto itr = g_ItemLevelDeltaToBonusListContainer.find(p_Delta);
        if (itr != g_ItemLevelDeltaToBonusListContainer.end())
            return itr->second;

        return 0;
    }
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Archaeology DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Archaeology
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<ResearchBranchEntry>             sResearchBranchStore(ResearchBranchfmt, false);
    DB2Storage<ResearchSiteEntry>               sResearchSiteStore(ResearchSitefmt, false);
    DB2Storage<ResearchProjectEntry>            sResearchProjectStore(ResearchProjectfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    std::set<ResearchProjectEntry const*>                         sResearchProjectSet;
    std::unordered_map<uint32, ResearchProjectEntry const*>       sResearchProjectPerSpell;
    std::set<ResearchSiteEntry const*>                            sResearchSiteSet;
    std::map<uint32 /*site_id*/, ResearchZoneEntry>               sResearchSiteDataMap;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Quest DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Quest
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<QuestV2CliTaskEntry>            sQuestV2CliTaskStore(QuestV2CliTaskFmt, false);
    DB2Storage<BountyEntry>                    sBountyStore(Bountyfmt, false);
    DB2Storage<AdventureMapPOIEntry>           sAdventureMapPOIStore(AdventureMapPOIfmt, false);
    DB2Storage<QuestPackageItemEntry>          sQuestPackageItemStore(QuestPackageItemEntryfmt, false);
    DB2Storage<QuestPOIPointEntry>             sQuestPOIPointStore(QuestPOIPointfmt, false);
    DB2Storage<QuestSortEntry>                 sQuestSortStore(QuestSortEntryfmt, false);
    DB2Storage<QuestV2Entry>                   sQuestV2Store(QuestV2fmt, false);
    DB2Storage<QuestXPEntry>                   sQuestXPStore(QuestXPfmt, false);
    DB2Storage<QuestFactionRewEntry>           sQuestFactionRewardStore(QuestFactionRewardfmt, false);
    DB2Storage<QuestMoneyRewardEntry>          sQuestMoneyRewardStore(QuestMoneyRewardfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    std::map<uint32, std::vector<QuestPackageItemEntry const*>> sQuestPackageItemsByGroup;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    /// Get quest bit flag
    /// @p_QuestID : Entry of the quest
    uint32 GetQuestUniqueBitFlag(uint32 p_QuestID)
    {
        QuestV2Entry const* l_QuestV2Entry = sQuestV2Store.LookupEntry(p_QuestID);
        if (!l_QuestV2Entry)
            return 0;

        return l_QuestV2Entry->UniqueBitFlag;
    }

    std::vector<QuestPackageItemEntry const*> const* GetQuestPackageItems(uint32 questPackageID)
    {
        auto itr = sQuestPackageItemsByGroup.find(questPackageID);
        if (itr != sQuestPackageItemsByGroup.end())
            return &itr->second;

        return nullptr;
    }
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Area and maps DB2
//////////////////////////////////////////////////////////////////////////
#pragma region AreaAndMaps

    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<AreaTableEntry>                 sAreaTableStore(AreaTablefmt, false);
    DB2Storage<AreaGroupMemberEntry>           sAreaGroupMemberStore(AreaGroupMemberEntryfmt, false);
    DB2Storage<AreaTriggerEntry>               sAreaTriggerStore(AreaTriggerEntryfmt, false);
    DB2Storage<DifficultyEntry>                sDifficultyStore(DifficultyEntryfmt, false);
    DB2Storage<LiquidTypeEntry>                sLiquidTypeStore(LiquidTypefmt, false);
    DB2Storage<MapEntry>                       sMapStore(MapEntryfmt, false);
    DB2Storage<MapDifficultyEntry>             sMapDifficultyStore(MapDifficultyEntryfmt, false);
    DB2Storage<PhaseEntry>                     sPhaseStore(PhaseEntryfmt, false);
    DB2Storage<WorldMapOverlayEntry>           sWorldMapOverlayStore(WorldMapOverlayEntryfmt, false);
    DB2Storage<WMOAreaTableEntry>              sWMOAreaTableStore(WMOAreaTableEntryfmt, false);
    DB2Storage<WorldMapAreaEntry>              sWorldMapAreaStore(WorldMapAreaEntryfmt, false);
    DB2Storage<WorldMapTransformsEntry>        sWorldMapTransformsStore(WorldMapTransformsfmt, false);
    DB2Storage<World_PVP_AreaEntry>            sWorld_PVP_AreaStore(World_PVP_AreaEntryfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////
    struct WMOAreaTableTripple
    {
        WMOAreaTableTripple(int32 r, int32 a, int32 g) : groupId(g), rootId(r), adtId(a)
        {

        }

        bool operator <(const WMOAreaTableTripple& b) const
        {
            return memcmp(this, &b, sizeof(WMOAreaTableTripple)) < 0;
        }

        // Ordered by entropy; that way memcmp will have a minimal medium runtime
        int32 groupId;
        int32 rootId;
        int32 adtId;
    };

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using WMOAreaInfoByTripple  = std::map<WMOAreaTableTripple, WMOAreaTableEntry const*>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    MapDifficultyContainer                  _mapDifficulties;
    AreaGroupsByAreaID                      s_AreaGroupsByAreaID;
    WMOAreaInfoByTripple                    sWMOAreaInfoByTripple;
    AreaGroupMembersByID                    sAreaGroupMembersByIDStore;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    MapDifficultyEntry const* GetDefaultMapDifficulty(uint32 p_MapID, Difficulty* p_Difficulty /*= nullptr*/)
    {
        auto l_It = _mapDifficulties.find(p_MapID);
        if (l_It == _mapDifficulties.end())
            return nullptr;

        if (l_It->second.empty())
            return nullptr;

        for (auto& l_Current : l_It->second)
        {
            DifficultyEntry const* l_DifficultyEntry = sDifficultyStore.LookupEntry(l_Current.first);
            if (!l_DifficultyEntry)
                continue;

            if (l_DifficultyEntry->Flags & DIFFICULTY_FLAG_DEFAULT)
            {
                if (p_Difficulty)
                    *p_Difficulty = Difficulty(l_Current.first);

                return l_Current.second;
            }
        }

        if (p_Difficulty)
            *p_Difficulty = Difficulty(l_It->second.begin()->first);

        return l_It->second.begin()->second;
    }

    MapDifficultyEntry const* GetMapDifficultyData(uint32 p_MapID, Difficulty difficulty)
    {
        auto l_It = _mapDifficulties.find(p_MapID);
        if (l_It == _mapDifficulties.end())
            return nullptr;

        auto l_DiffIt = l_It->second.find(difficulty);
        if (l_DiffIt == l_It->second.end())
            return nullptr;

        return l_DiffIt->second;
    }

    MapDifficultyEntry const* GetDownscaledMapDifficultyData(uint32 p_MapID, Difficulty& p_Difficulty)
    {
        DifficultyEntry const* l_DiffEntry = sDifficultyStore.LookupEntry(p_Difficulty);
        if (!l_DiffEntry)
            return GetDefaultMapDifficulty(p_MapID, &p_Difficulty);

        uint32 l_TempDiff = p_Difficulty;
        MapDifficultyEntry const* l_MapDiff = GetMapDifficultyData(p_MapID, Difficulty(l_TempDiff));
        while (!l_MapDiff)
        {
            l_TempDiff = l_DiffEntry->FallbackDifficultyID;
            l_DiffEntry = sDifficultyStore.LookupEntry(l_TempDiff);
            if (!l_DiffEntry)
                return GetDefaultMapDifficulty(p_MapID, &p_Difficulty);

            // pull new data
            l_MapDiff = GetMapDifficultyData(p_MapID, Difficulty(l_TempDiff)); // we are 10 normal or 25 normal
        }

        p_Difficulty = Difficulty(l_TempDiff);
        return l_MapDiff;
    }
    uint32 GetMaxLevelForExpansion(uint32 p_Expansion)
    {
        switch (p_Expansion)
        {
        case EXPANSION_VANILLA:
            return 60;
        case EXPANSION_THE_BURNING_CRUSADE:
            return 70;
        case EXPANSION_WRATH_OF_THE_LICH_KING:
            return 80;
        case EXPANSION_CATACLYSM:
            return 85;
        case EXPANSION_MISTS_OF_PANDARIA:
            return 90;
        case EXPANSION_WARLORDS_OF_DRAENOR:
            return 100;
        case EXPANSION_LEGION:
            return 110;
        default:
            break;

        }
        return 0;
    }
    uint32 GetVirtualMapForMapAndZone(uint32 mapid, uint32 zoneId)
    {
        if (mapid != 530 && mapid != 571 && mapid != 732)   // Speed for most cases
            return mapid;

        if (WorldMapAreaEntry const* wma = sWorldMapAreaStore.LookupEntry(zoneId))
            return wma->DisplayMapID >= 0 ? wma->DisplayMapID : wma->MapID;

        return mapid;
    }
    WMOAreaTableEntry const* GetWMOAreaTableEntryByTripple(int32 rootid, int32 adtid, int32 groupid)
    {
        WMOAreaInfoByTripple::iterator i = sWMOAreaInfoByTripple.find(WMOAreaTableTripple(rootid, adtid, groupid));
        if (i == sWMOAreaInfoByTripple.end())
            return NULL;
        return i->second;
    }
    void Zone2MapCoordinates(float& x, float& y, uint32 zone)
    {
        WorldMapAreaEntry const* maEntry = sWorldMapAreaStore.LookupEntry(zone);

        // If not listed then map coordinates (instance)
        if (!maEntry)
            return;

        std::swap(x, y);                                         // At client map coords swapped
        x = x*((maEntry->LocBottom - maEntry->LocTop) / 100) + maEntry->LocTop;
        y = y*((maEntry->LocRight - maEntry->LocLeft) / 100) + maEntry->LocLeft;      // Client y coord from top to down
    }
    void Map2ZoneCoordinates(float& x, float& y, uint32 zone)
    {
        WorldMapAreaEntry const* maEntry = sWorldMapAreaStore.LookupEntry(zone);

        // If not listed then map coordinates (instance)
        if (!maEntry)
            return;

        x = (x - maEntry->LocTop) / ((maEntry->LocBottom - maEntry->LocTop) / 100);
        y = (y - maEntry->LocLeft) / ((maEntry->LocRight - maEntry->LocLeft) / 100);    // Client y coord from top to down
        std::swap(x, y);                                        // Client have map coords swapped
    }
    uint32 GetLiquidFlags(uint32 liquidType)
    {
        if (LiquidTypeEntry const* liq = sLiquidTypeStore.LookupEntry(liquidType))
            return 1 << liq->Type;

        return 0;
    }
    std::vector<uint32> GetAreasForGroup(uint32 areaGroupId)
    {
        auto itr = sAreaGroupMembersByIDStore.find(areaGroupId);
        if (itr != sAreaGroupMembersByIDStore.end())
            return itr->second;

        return std::vector<uint32>();
    }

bool IsWithinAreaGroup(uint32 p_AreaGroup, uint32 p_AreaID)
{
    auto l_Ids = GetAreasForGroup(p_AreaGroup);

    for (auto l_AreaID : l_Ids)
    {
        if (l_AreaID == p_AreaID)
            return true;
    }

    return false;
}

void DeterminaAlternateMapPosition(uint32 p_MapID, float p_X, float p_Y, float p_Z, uint32* p_NewMapID /*= nullptr*/, float* p_NewPosX /*= nullptr*/, float* p_NewPosY /*= nullptr*/)
    {
        ASSERT(p_NewMapID || (p_NewPosX && p_NewPosY));

        WorldMapTransformsEntry const* l_Transformation = nullptr;

        for (uint32 l_I = 0; l_I < sWorldMapTransformsStore.GetNumRows(); ++l_I)
        {
            WorldMapTransformsEntry const* l_Transform = sWorldMapTransformsStore.LookupEntry(l_I);

            if (!l_Transform)
                continue;

            if (l_Transform->MapID != p_MapID)
                continue;

            if (l_Transform->RegionMinX > p_X || l_Transform->RegionMaxX < p_X)
                continue;
            if (l_Transform->RegionMinY > p_Y || l_Transform->RegionMaxY < p_Y)
                continue;
            if (l_Transform->RegionMinZ > p_Z || l_Transform->RegionMaxZ < p_Z)
                continue;

            l_Transformation = l_Transform;
            break;
        }

        if (!l_Transformation)
        {
            if (p_NewMapID)
                *p_NewMapID = p_MapID;

            if (p_NewPosX && p_NewPosY)
            {
                *p_NewPosX = p_X;
                *p_NewPosY = p_Y;
            }
            return;
        }

        if (p_NewMapID)
            *p_NewMapID = l_Transformation->NewMapID;

        if (!p_NewPosX || !p_NewPosY)
            return;

        if (l_Transformation->RegionScale > 0.0f && l_Transformation->RegionScale < 1.0f)
        {
            p_X = (p_X - l_Transformation->RegionMinX) * l_Transformation->RegionScale + l_Transformation->RegionMinX;
            p_Y = (p_Y - l_Transformation->RegionMinY) * l_Transformation->RegionScale + l_Transformation->RegionMinY;
        }

        *p_NewPosX = p_X + l_Transformation->RegionOffsetX;
        *p_NewPosY = p_Y + l_Transformation->RegionOffsetY;
    }
    bool MountTypeXCapabilityEntryComparator::Compare(MountTypeXCapabilityEntry const* left, MountTypeXCapabilityEntry const* right)
    {
        if (left->MountTypeID == right->MountTypeID)
            return left->Index < right->Index;
        return left->MountTypeID < right->MountTypeID;
    }
#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Import DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Import
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<ImportPriceArmorEntry>          sImportPriceArmorStore(ImportPriceArmorfmt, false);
    DB2Storage<ImportPriceQualityEntry>        sImportPriceQualityStore(ImportPriceQualityfmt, false);
    DB2Storage<ImportPriceShieldEntry>         sImportPriceShieldStore(ImportPriceShieldfmt, false);
    DB2Storage<ImportPriceWeaponEntry>         sImportPriceWeaponStore(ImportPriceWeaponfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Char DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Char
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<CharStartOutfitEntry>           sCharStartOutfitStore(CharStartOutfitEntryfmt, false);
    DB2Storage<CharTitlesEntry>                sCharTitlesStore(CharTitlesEntryfmt, false);
    DB2Storage<ChrClassesEntry>                sChrClassesStore(ChrClassesEntryfmt, false);
    DB2Storage<ChrClassXPowerTypesEntry>       sChrClassXPowerTypesStore(ChrClassesXPowerTypesfmt, false);
    DB2Storage<ChrRacesEntry>                  sChrRacesStore(ChrRacesfmt, false);
    DB2Storage<ChrSpecializationsEntry>        sChrSpecializationsStore(ChrSpecializationsfmt, false);
    DB2Storage<CharacterLoadoutItemEntry>      sCharLoadoutItemStore(CharacterLoadoutItemEntryfmt, false);
    DB2Storage<CharacterLoadoutEntry>          sCharLoadoutStore(CharacterLoadoutEntryfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    ChrSpecializationsEntry const* GetChrSpecializationByIndex(uint32 class_, uint32 index)
    {
        return _chrSpecializationsByIndex[class_][index];
    }

    ChrSpecializationsEntry const* GetDefaultChrSpecializationForClass(uint32 class_)
    {
        auto l_Itr = _defaultChrSpecializationsByClass.find(class_);
        if (l_Itr != _defaultChrSpecializationsByClass.end())
            return l_Itr->second;

        return nullptr;
    }

    std::vector<ChrSpecializationsEntry const*> const& GetChrSpecializationsForClass(uint32 p_Class)
    {
        return _chrSpecializationsByClass[p_Class];
    }

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Creature DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Creature
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<CreatureDisplayInfoEntry>       sCreatureDisplayInfoStore(CreatureDisplayInfofmt, false);
    DB2Storage<CreatureDisplayInfoExtraEntry>  sCreatureDisplayInfoExtraStore(CreatureDisplayInfoExtrafmt, false);
    DB2Storage<CreatureFamilyEntry>            sCreatureFamilyStore(CreatureFamilyfmt, false);
    DB2Storage<CreatureModelDataEntry>         sCreatureModelDataStore(CreatureModelDatafmt, false);
    DB2Storage<CreatureTypeEntry>              sCreatureTypeStore(CreatureTypefmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// Misc DB2
//////////////////////////////////////////////////////////////////////////
#pragma region Misc

    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<AnimKitEntry>                   sAnimKitStore(AnimKitfmt, false);
    DB2Storage<ArmorLocationEntry>             sArmorLocationStore(ArmorLocationfmt, false);
    DB2Storage<AuctionHouseEntry>              sAuctionHouseStore(AuctionHouseEntryfmt, false);
    DB2Storage<BankBagSlotPricesEntry>         sBankBagSlotPricesStore(BankBagSlotPricesEntryfmt, false);
    DB2Storage<BarberShopStyleEntry>           sBarberShopStyleStore(BarberShopStyleEntryfmt, false);
    DB2Storage<BattlemasterListEntry>          sBattlemasterListStore(BattlemasterListEntryfmt, false);
    DB2Storage<BroadcastTextEntry>             sBroadcastTextStore(BroadcastTextEntryfmt, false);
    DB2Storage<CharSectionsEntry>              sCharSectionsStore(CharSectionsEntryfmt, false);
    DB2Storage<CinematicCameraEntry>           sCinematicCameraStore(CinematicCameraEntryfmt, false);
    DB2Storage<CinematicSequencesEntry>        sCinematicSequencesStore(CinematicSequencesEntryfmt, false);
    DB2Storage<ChatChannelsEntry>              sChatChannelsStore(ChatChannelsEntryfmt, false);
    DB2Storage<CurrencyTypesEntry>             sCurrencyTypesStore(CurrencyTypesfmt, false);
    DB2Storage<CurveEntry>                     sCurveStore(CurveEntryfmt, false);
    DB2Storage<CurvePointEntry>                sCurvePointStore(CurvePointEntryfmt, false);
    DB2Storage<DestructibleModelDataEntry>     sDestructibleModelDataStore(DestructibleModelDatafmt, false);
    DB2Storage<DungeonEncounterEntry>          sDungeonEncounterStore(DungeonEncounterfmt, false);
    DB2Storage<DurabilityCostsEntry>           sDurabilityCostsStore(DurabilityCostsfmt, false);
    DB2Storage<DurabilityQualityEntry>         sDurabilityQualityStore(DurabilityQualityfmt, false);
    DB2Storage<EmotesEntry>                    sEmotesStore(EmotesEntryfmt, false);
    DB2Storage<EmotesTextEntry>                sEmotesTextStore(EmotesTextEntryfmt, false);
    DB2Storage<EmotesTextSoundEntry>           sEmotesTextSoundStore(EmotesTextSoundEntryfmt, false);
    DB2Storage<FactionEntry>                   sFactionStore(FactionEntryfmt, false);
    DB2Storage<FactionTemplateEntry>           sFactionTemplateStore(FactionTemplateEntryfmt, false);
    DB2Storage<GameObjectDisplayInfoEntry>     sGameObjectDisplayInfoStore(GameObjectDisplayInfofmt, false);
    DB2Storage<GemPropertiesEntry>             sGemPropertiesStore(GemPropertiesEntryfmt, false);
    DB2Storage<GlyphBindableSpellEntry>        sGlyphBindableSpellStore(GlyphBindableSpellfmt, false);
    DB2Storage<GlyphPropertiesEntry>           sGlyphPropertiesStore(GlyphPropertiesfmt, false);
    DB2Storage<GlyphRequiredSpecEntry>         sGlyphRequiredSpecStore(GlyphRequiredSpecfmt, false);
    DB2Storage<GroupFinderActivityEntry>       sGroupFinderActivityStore(GroupFinderActivityfmt, false);
    DB2Storage<GroupFinderCategoryEntry>       sGroupFinderCategoryStore(GroupFinderCategoryfmt, false);
    DB2Storage<GuildPerkSpellsEntry>           sGuildPerkSpellsStore(GuildPerkSpellsfmt, false);
    DB2Storage<GlobalStringsEntry>             sGlobalStringsStore(GlobalStringsfmt, false);
    DB2Storage<HeirloomEntry>                  sHeirloomStore(HeirloomFmt, false);
    DB2Storage<HolidaysEntry>                  sHolidaysStore(Holidaysfmt, false);
    DB2Storage<JournalInstanceEntry>           sJournalInstanceStore(JournalInstancefmt, false);
    DB2Storage<JournalEncounterEntry>          sJournalEncounterStore(JournalEncounterfmt, false);
    DB2Storage<JournalEncounterItemEntry>      sJournalEncounterItemStore(JournalEncounterItemfmt, false);
    DB2Storage<LFGDungeonEntry>                sLFGDungeonStore(LFGDungeonEntryfmt, false);
    DB2Storage<LocationEntry>                  sLocationStore(LocationEntryfmt, false);
    DB2Storage<LockEntry>                      sLockStore(LockEntryfmt, false);
    DB2Storage<MailTemplateEntry>              sMailTemplateStore(MailTemplateEntryfmt, false);
    DB2Storage<MapChallengeModeEntry>          sMapChallengeModeStore(MapChallengeModeEntryfmt, false);
    DB2Storage<MinorTalentEntry>               sMinorTalentStore(MinorTalentfmt, false);
    DB2Storage<MountCapabilityEntry>           sMountCapabilityStore(MountCapabilityfmt, false);
    DB2Storage<MountEntry>                     sMountStore(MountEntryfmt, false);
    DB2Storage<MountTypeXCapabilityEntry>      sMountTypeXCapabilityStore(MountTypeXCapabilityfmt, false);
    DB2Storage<MountXDisplayEntry>             sMountXDisplayStore(MountXDisplayfmt, false);
    DB2Storage<MovieEntry>                     sMovieStore(MovieEntryfmt, false);
    DB2Storage<NameGenEntry>                   sNameGenStore(NameGenfmt, false);
    DB2Storage<ParagonReputationEntry>         sParagonReputationStore(ParagonReputationfmt, false);
    DB2Storage<PathNodeEntry>                  sPathNodeStore(PathNodeEntryfmt, false);
    DB2Storage<PlayerConditionEntry>           sPlayerConditionStore(PlayerConditionEntryfmt, false);
    DB2Storage<PowerDisplayEntry>              sPowerDisplayStore(PowerDisplayfmt, false);
    DB2Storage<PowerTypeEntry>                 sPowerTypeStore(PowerTypefmt, false);
    DB2Storage<PrestigeLevelInfoEntry>         sPrestigeLevelInfoStore(PrestigeLevelInfofmt, false);
    DB2Storage<PvPDifficultyEntry>             sPvPDifficultyStore(PvPDifficultyfmt, false);
    DB2Storage<PvpTalentEntry>                 sPvpTalentStore(PvpTalentfmt, false);
    DB2Storage<PvpTalentUnlockEntry>           sPvpTalentUnlockStore(PvpTalentUnlockfmt, false);
    DB2Storage<PvpRewardEntry>                 sPvpRewardStore(PvpRewardfmt, false);
    DB2Storage<RandomPropertiesPointsEntry>    sRandomPropertiesPointsStore(RandomPropertiesPointsfmt, false);
    DB2Storage<RewardPackEntry>                sRewardPackStore(RewardPackfmt, false);
    DB2Storage<RewardPackXCurrencyTypeEntry>   sRewardPackXCurrencyTypeStore(RewardPackXCurrencyfmt, false);
    DB2Storage<RewardPackXItemEntry>           sRewardPackXItemStore(RewardPackXItemfmt, false);
    DB2Storage<ScalingStatDistributionEntry>   sScalingStatDistributionStore(ScalingStatDistributionfmt, false);
    DB2Storage<SandboxScalingEntry>            sSandboxScalingStore(SandboxScalingfmt, false);
    DB2Storage<ScenarioEntry>                  sScenarioStore(ScenarioEntryfmt, false);
    DB2Storage<ScenarioStepEntry>              sScenarioStepStore(ScenarioStepEntryfmt, false);
    DB2Storage<SkillLineAbilityEntry>          sSkillLineAbilityStore(SkillLineAbilityfmt, false);
    DB2Storage<SkillRaceClassInfoEntry>        sSkillRaceClassInfoStore(SkillRaceClassInfofmt, false);
    DB2Storage<SkillLineEntry>                 sSkillLineStore(SkillLinefmt, false);
    DB2Storage<SoundKitEntry>                  sSoundKitStore(SoundKitEntryFmt, false);
    DB2Storage<SummonPropertiesEntry>          sSummonPropertiesStore(SummonPropertiesfmt, false);
    DB2Storage<TactKeyEntry>                   sTactKeyStore(TactKeyfmt, false);
    DB2Storage<TalentEntry>                    sTalentStore(TalentEntryfmt, false);
    DB2Storage<TotemCategoryEntry>             sTotemCategoryStore(TotemCategoryEntryfmt, false);
    DB2Storage<TransportAnimationEntry>        sTransportAnimationStore(TransportAnimationfmt, false);
    DB2Storage<TransportRotationEntry>         sTransportRotationStore(TransportRotationfmt, false);
    DB2Storage<ToyEntry>                       sToyStore(ToyEntryfmt, false);
    DB2Storage<VehicleEntry>                   sVehicleStore(VehicleEntryfmt, false);
    DB2Storage<VehicleSeatEntry>               sVehicleSeatStore(VehicleSeatEntryfmt, false);
    DB2Storage<VignetteEntry>                  sVignetteStore(VignetteEntryfmt, false);
    DB2Storage<WorldSafeLocsEntry>             sWorldSafeLocsStore(WorldSafeLocsEntryfmt, false);
    DB2Storage<AreaPOIEntry>                   sAreaPOIStore(AreaPOIEntryfmt, false);
    DB2Storage<ContributionEntry>              sContributionStore(ContributionEntryfmt, false);
    DB2Storage<ManagedWorldStateEntry>         sManagedWorldStateStore(ManagedWorldStateEntryfmt, false);
    DB2Storage<ManagedWorldStateBuffEntry>     sManagedWorldStateBuffStore(ManagedWorldStateBuffEntryfmt, false);
    DB2Storage<ManagedWorldStateInputEntry>    sManagedWorldStateInputStore(ManagedWorldStateInputEntryfmt, false);

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    std::map<uint32 /*curveID*/, std::map<uint32/*index*/, CurvePointEntry const*, std::greater<uint32>>>   HeirloomCurvePoints;
    std::unordered_map<uint32 /*ItemID*/, HeirloomEntry const*>                                             HeirloomEntryByItemID;
    std::vector<std::vector<uint8>>                                                                         sPowersByClassStore;
    NameGenVectorArraysMap                                                                                  sGenNameVectoArraysMap;
    uint32                                                                                                  g_MaxPrestigeLevel;
    CurvePointsContainer                                                                                    g_CurvePoints;
    CharSectionsContainer                                                                                   g_CharSections;
    SkillLineAbilityPerSkill                                                                                g_SkillLineAbilityPerSkill;
    SkillLineAbilityReqSpells                                                                               g_SkillLineAbilityReqSpells;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    uint32 GetSkillLineAbilityReqSpell(uint32 spellId)
    {
        auto iter = g_SkillLineAbilityReqSpells.find(spellId);
        if (iter != g_SkillLineAbilityReqSpells.end())
            return iter->second;
        return 0;
    }
    uint8 GetPowerIndexByClass(uint8 p_Class, uint8 p_Power)
    {
        if (p_Class >= MAX_CLASSES)
            return Powers::MAX_POWERS;

        auto const& l_List = sPowersByClassStore[p_Class];
        for (int l_I = 0; l_I < (int)l_List.size(); ++l_I)
        {
            if (l_List[l_I] == p_Power)
                return l_I;
        }

        return Powers::MAX_POWERS;
    }
    uint32 GetHeirloomItemLevel(uint32 curveId, uint32 level)
    {
        /// Assuming linear item level scaling for heirlooms
        auto itr = HeirloomCurvePoints.find(curveId);
        if (itr == HeirloomCurvePoints.end())
            return 0;

        auto it2 = itr->second.begin(); ///< Highest scaling point
        if (level >= it2->second->X)
            return it2->second->Y;

        auto previousItr = it2++;
        for (; it2 != itr->second.end(); ++it2, ++previousItr)
            if (level >= it2->second->X)
                return uint32((previousItr->second->Y - it2->second->Y) / (previousItr->second->X - it2->second->X) * (float(level) - it2->second->X) + it2->second->Y);

        return uint32(previousItr->second->Y);  ///< Lowest scaling point
    }
    HeirloomEntry const* GetHeirloomEntryByItemID(uint32 p_ItemID)
    {
        std::unordered_map<uint32, HeirloomEntry const*>::const_iterator l_Iter = HeirloomEntryByItemID.find(p_ItemID);
        return l_Iter != HeirloomEntryByItemID.end() ? l_Iter->second : nullptr;
    }
    uint32 GetMaxPrestigeLevel()
    {
        return g_MaxPrestigeLevel;
    }
    SimpleFactionsList const* GetFactionTeamList(uint32 faction)
    {
        FactionTeamMap::const_iterator itr = sFactionTeamMap.find(faction);
        if (itr != sFactionTeamMap.end())
            return &itr->second;

        return NULL;
    }
    EmotesTextSoundEntry const* FindTextSoundEmoteFor(uint32 emote, uint32 race, uint32 gender)
    {
        auto itr = sEmotesTextSoundMap.find(EmotesTextSoundKey(emote, race, gender));
        return itr != sEmotesTextSoundMap.end() ? itr->second : nullptr;
    }
    char const* GetPetName(uint32 petfamily, uint32 /*dbclang*/)
    {
        if (!petfamily)
            return NULL;
        CreatureFamilyEntry const* pet_family = sCreatureFamilyStore.LookupEntry(petfamily);
        if (!pet_family)
            return NULL;
        return  pet_family->Name->Get(sWorld->GetDefaultDb2Locale());
    }
    const std::string* GetRandomCharacterName(uint8 race, uint8 gender)
    {
        if (gender > 1)
            return nullptr;

        if (sGenNameVectoArraysMap.find(race) == sGenNameVectoArraysMap.end())
            return nullptr;

        uint32 size = sGenNameVectoArraysMap[race].stringVectorArray[gender].size();
        uint32 randPos = urand(0, size - 1);

        return &sGenNameVectoArraysMap[race].stringVectorArray[gender][randPos];
    }
    void HotfixLfgDungeonsData()
    {
        for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
        {
            if (auto l_Entry = const_cast<LFGDungeonEntry*>(sLFGDungeonStore.LookupEntry(i)))
            {
                if (l_Entry->isScenario())
                    g_SingleDungeonsPerMap[l_Entry->map] = l_Entry;

                if (l_Entry->isScenarioSingle())
                {
                    /// Fix access to some scenarios
                    if (l_Entry->maxlevel == 0)
                        l_Entry->maxlevel = MAX_LEVEL;

                    /// fix when single scenarios are included in random
                    l_Entry->grouptype = 0;
                }
            }
        }
    }
    MountTypeXCapabilitySet const* GetMountCapabilities(uint32 mountType)
    {
        auto itr = _mountCapabilitiesByType.find(mountType);
        if (itr != _mountCapabilitiesByType.end())
            return &itr->second;

        return nullptr;
    }
    MountEntry const* GetMountBySpell(uint32 p_SpellId)
    {
        auto l_Itr = g_MountBySpell.find(p_SpellId);
        if (l_Itr != g_MountBySpell.end())
            return l_Itr->second;

        return nullptr;
    }
    uint32 GetPvPTalentLevelRequirement(uint32 p_Collumn, uint32 p_Tier)
    {
        if (p_Collumn > g_PvpTalentLevelRequirement.size())
            return 0;

        if (p_Tier > g_PvpTalentLevelRequirement[p_Collumn].size())
            return 0;

        return g_PvpTalentLevelRequirement[p_Collumn][p_Tier];
    }
    SkillRaceClassInfoEntry const* GetSkillRaceClassInfo(uint32 skill, uint8 race, uint8 class_)
    {
        auto bounds = _skillRaceClassInfoBySkill.equal_range(skill);
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
        {
            if (itr->second->RaceMask && !(itr->second->RaceMask & (UI64LIT(1) << (race - 1))))
                continue;
            if (itr->second->ClassMask && !(itr->second->ClassMask & (1 << (class_ - 1))))
                continue;

            return itr->second;
        }

        return nullptr;
    }

    std::vector<SkillLineAbilityEntry const*> const* GetSkillLineAbilitiesBySkill(uint32 skillId)
    {
        auto l_Iter = _skillLineAbilitiesBySkillupSkill.find(skillId);
        if (l_Iter != _skillLineAbilitiesBySkillupSkill.end())
            return &l_Iter->second;

        return nullptr;
    }

    std::vector<SpecializationSpellEntry const*> const* GetSpecializationSpells(uint32 specId)
    {
        auto itr = _specializationSpellsBySpec.find(specId);
        if (itr != _specializationSpellsBySpec.end())
            return &itr->second;

        return nullptr;
    }
    std::vector<TalentEntry const*> const& GetTalentsByPosition(uint32 class_, uint32 p_SpecializationId, uint32 tier, uint32 column)
    {
        ASSERT(class_   < MAX_CLASSES);
        ASSERT(tier     < MAX_TALENT_TIERS);        ///< "MAX_TALENT_TIERS must be at least %u", talentInfo->TierID);
        ASSERT(column   < MAX_TALENT_COLUMNS);      ///< "MAX_TALENT_COLUMNS must be at least %u", talentInfo->ColumnIndex);

        uint32 l_Spec = 0;

        if (const ChrSpecializationsEntry* l_Entry = sChrSpecializationsStore.LookupEntry(p_SpecializationId))
            l_Spec = l_Entry->OrderIndex;

        ASSERT(l_Spec  < MAX_SPECIALIZATIONS);

        return _talentsByPosition[class_][l_Spec][tier][column];
    }
    LFGDungeonEntry const* GetSingleDungonByMap(uint32 p_MapID)
    {
        auto l_Itr = g_SingleDungeonsPerMap.find(p_MapID);
        if (l_Itr != g_SingleDungeonsPerMap.end())
            return l_Itr->second;

        return nullptr;
    }

    enum class CurveInterpolationMode : uint8
    {
        Linear = 0,
        Cosine = 1,
        CatmullRom = 2,
        Bezier3 = 3,
        Bezier4 = 4,
        Bezier = 5,
        Constant = 6,
    };

    static CurveInterpolationMode DetermineCurveType(CurveEntry const* curve, std::vector<CurvePointEntry const*> const& points)
    {
        switch (curve->Type)
        {
            case 1:
                return points.size() < 4 ? CurveInterpolationMode::Cosine : CurveInterpolationMode::CatmullRom;
            case 2:
            {
                switch (points.size())
                {
                    case 1:
                        return CurveInterpolationMode::Constant;
                    case 2:
                        return CurveInterpolationMode::Linear;
                    case 3:
                        return CurveInterpolationMode::Bezier3;
                    case 4:
                        return CurveInterpolationMode::Bezier4;
                    default:
                        break;
                }
                return CurveInterpolationMode::Bezier;
            }
            case 3:
                return CurveInterpolationMode::Cosine;
            default:
                break;
        }

        return points.size() != 1 ? CurveInterpolationMode::Linear : CurveInterpolationMode::Constant;
    }

    float GetCurveValueAt(uint32 curveId, float x)
    {
        auto itr = g_CurvePoints.find(curveId);
        if (itr == g_CurvePoints.end())
            return 0.0f;

        CurveEntry const* curve = sCurveStore.LookupEntry(curveId);
        if (!curve)
            return 0.0f;

        std::vector<CurvePointEntry const*> const& points = itr->second;
        if (points.empty())
            return 0.0f;

        switch (DetermineCurveType(curve, points))
        {
            case CurveInterpolationMode::Linear:
            {
                std::size_t pointIndex = 0;
                while (pointIndex < points.size() && points[pointIndex]->X <= x)
                    ++pointIndex;

                if (!pointIndex)
                    return points[0]->Y;

                if (pointIndex >= points.size())
                    return points.back()->Y;

                float xDiff = points[pointIndex]->X - points[pointIndex - 1]->X;
                if (xDiff == 0.0)
                    return points[pointIndex]->Y;

                return (((x - points[pointIndex - 1]->X) / xDiff) * (points[pointIndex]->Y - points[pointIndex - 1]->Y)) + points[pointIndex - 1]->Y;
            }
            case CurveInterpolationMode::Cosine:
            {
                std::size_t pointIndex = 0;
                while (pointIndex < points.size() && points[pointIndex]->X <= x)
                    ++pointIndex;

                if (!pointIndex)
                    return points[0]->Y;

                if (pointIndex >= points.size())
                    return points.back()->Y;

                float xDiff = points[pointIndex]->X - points[pointIndex - 1]->X;
                if (xDiff == 0.0)
                    return points[pointIndex]->Y;

                return ((points[pointIndex]->Y - points[pointIndex - 1]->Y) * (1.0f - std::cos((x - points[pointIndex - 1]->X) / xDiff * float(M_PI))) * 0.5f) + points[pointIndex - 1]->Y;
            }
            case CurveInterpolationMode::CatmullRom:
            {
                std::size_t pointIndex = 1;
                while (pointIndex < points.size() && points[pointIndex]->X <= x)
                    ++pointIndex;

                if (pointIndex == 1)
                    return points[1]->Y;

                if (pointIndex >= points.size() - 1)
                    return points[points.size() - 2]->Y;

                float xDiff = points[pointIndex]->X - points[pointIndex - 1]->X;
                if (xDiff == 0.0)
                    return points[pointIndex]->Y;

                float mu = (x - points[pointIndex - 1]->X) / xDiff;
                float a0 = -0.5f * points[pointIndex - 2]->Y + 1.5f * points[pointIndex - 1]->Y - 1.5f * points[pointIndex]->Y + 0.5f * points[pointIndex + 1]->Y;
                float a1 = points[pointIndex - 2]->Y - 2.5f * points[pointIndex - 1]->Y + 2.0f * points[pointIndex]->Y - 0.5f * points[pointIndex + 1]->Y;
                float a2 = -0.5f * points[pointIndex - 2]->Y + 0.5f * points[pointIndex]->Y;
                float a3 = points[pointIndex - 1]->Y;

                return a0 * mu * mu * mu + a1 * mu * mu + a2 * mu + a3;
            }
            case CurveInterpolationMode::Bezier3:
            {
                float xDiff = points[2]->X - points[0]->X;
                if (xDiff == 0.0)
                    return points[1]->Y;

                float mu = (x - points[0]->X) / xDiff;
                return ((1.0f - mu) * (1.0f - mu) * points[0]->Y) + (1.0f - mu) * 2.0f * mu * points[1]->Y + mu * mu * points[2]->Y;
            }
            case CurveInterpolationMode::Bezier4:
            {
                float xDiff = points[3]->X - points[0]->X;
                if (xDiff == 0.0)
                    return points[1]->Y;

                float mu = (x - points[0]->X) / xDiff;
                return (1.0f - mu) * (1.0f - mu) * (1.0f - mu) * points[0]->Y
                    + 3.0f * mu * (1.0f - mu) * (1.0f - mu) * points[1]->Y
                    + 3.0f * mu * mu * (1.0f - mu) * points[2]->Y
                    + mu * mu * mu * points[3]->Y;
            }
            case CurveInterpolationMode::Bezier:
            {
                float xDiff = points.back()->X - points[0]->X;
                if (xDiff == 0.0f)
                    return points.back()->Y;

                std::vector<float> tmp(points.size());
                for (std::size_t i = 0; i < points.size(); ++i)
                    tmp[i] = points[i]->Y;

                float mu = (x - points[0]->X) / xDiff;
                int32 i = int32(points.size()) - 1;
                while (i > 0)
                {
                    for (int32 k = 0; k < i; ++k)
                        tmp[k] = tmp[k] + mu * (tmp[k + 1] - tmp[k]);

                    --i;
                }
                return tmp[0];
            }
            case CurveInterpolationMode::Constant:
                return points[0]->Y;
            default:
                break;
        }

        return 0.0f;
    }

    char const* GetBroadcastTextValue(BroadcastTextEntry const* p_BroadcastTextEntry, LocaleConstant p_Locale /*= DEFAULT_LOCALE*/, uint8 p_Gender /*= Gender::GENDER_MALE*/, bool p_ForceGender /*= false*/)
    {
        if (p_Gender == Gender::GENDER_FEMALE && (p_ForceGender || p_BroadcastTextEntry->FemaleText->Str[DEFAULT_LOCALE][0] != '\0'))
        {
            if (p_BroadcastTextEntry->FemaleText->Str[p_Locale][0] != '\0')
                return p_BroadcastTextEntry->FemaleText->Str[p_Locale];

            return p_BroadcastTextEntry->FemaleText->Str[DEFAULT_LOCALE];
        }

        if (p_BroadcastTextEntry->MaleText->Str[p_Locale][0] != '\0')
            return p_BroadcastTextEntry->MaleText->Str[p_Locale];

        return p_BroadcastTextEntry->MaleText->Str[DEFAULT_LOCALE];
    }

    CharSectionsEntry const* GetCharSectionEntry(uint8 p_Race, CharSectionType p_GenType, uint8 p_Gender, uint8 p_Type, uint8 p_Color)
    {
        auto l_Eqr = g_CharSections.equal_range(uint32(p_GenType) | uint32(p_Gender << 8) | uint32(p_Race << 16));
        for (auto l_Itr = l_Eqr.first; l_Itr != l_Eqr.second; ++l_Itr)
        {
            if (l_Itr->second->Type == p_Type && l_Itr->second->Color == p_Color)
                return l_Itr->second;
        }

        return nullptr;
    }

    MapChallengeModeEntryContainer const* GetLegionChallengeModeEntries()
    {
        return &g_LegionMapChallengeModeEntrybyMap;
    }

    std::vector<uint32>* GetChallengeMaps()
    {
        return &_challengeModeMaps;
    }

    std::vector<uint32> GetItemLootPerMap(uint32 p_MapID)
    {
        return g_ItemLootPerMap[p_MapID];
    }

    uint32 GetMountDisplayID(Player const* p_Player, uint32 p_MountID)
    {
        auto l_Iter = g_MountVisualsByID.find(p_MountID);

        if (l_Iter != g_MountVisualsByID.end())
        {
            for (auto const& l_Entry : l_Iter->second)
            {
                if (!p_Player || p_Player->EvalPlayerCondition(l_Entry->PlayerConditionID, false).first)
                    return l_Entry->DisplayID;
            }
        }

        return 0;
    }

    ToyEntry const* GetToyEntryFromItemId(uint32 p_ItemID)
    {
        auto l_Itr = g_ToyItems.find(p_ItemID);
        if (l_Itr == g_ToyItems.end())
            return nullptr;

        return l_Itr->second;
    }

#pragma endregion

//////////////////////////////////////////////////////////////////////////
/// WorldState DB2
//////////////////////////////////////////////////////////////////////////
#pragma region WorldState
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    DB2Storage<WorldStateExpressionEntry>      sWorldStateExpressionStore(WorldStateExpressionEntryfmt);

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    static uint8 const g_WorldState_UnpackUnkTable[] = {
        0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00,
        0x20, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x28, 0x00, 0x20, 0x00, 0x20, 0x00,
        0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00,
        0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00,
        0x48, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
        0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
        0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00,
        0x84, 0x00, 0x84, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
        0x10, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
        0x10, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x02, 0x00,
        0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00,
        0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00,
        0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    namespace WorldStateExpressionMathOpcode
    {
        enum
        {
            Add         = 1,
            Substract   = 2,
            Multiply    = 3,
            Divide      = 4,
            Modulo      = 5
        };
    }

    namespace WorldStateExpressionCompareOpcode
    {
        enum
        {
            Equal       = 1,
            Different   = 2,
            Less        = 3,
            LessOrEqual = 4,
            More        = 5,
            MoreOrEqual = 6
        };
    }

    namespace WorldStateExpressionCustomOpType
    {
        enum
        {
            PushUInt32          = 1,
            PushWorldStateValue = 2,
            CallFunction        = 3
        };
    }

    namespace WorldStateExpressionEvalResultLogic
    {
        enum
        {
            FirstAndSecond      = 1,
            FirstOrSecond       = 2,
            NotFirstOrNotSecond = 3
        };
    }

    namespace WorldStateExpressionFunctions
    {
        enum
        {
            None,
            Random,
            Month,
            Day,
            TimeOfDay,
            Region,
            ClockHour,
            DifficultyID,
            HolidayStart,
            HolidayLeft,
            HolidayActive,
            TimerCurrentTime,
            WeekNumber,
            None2,
            None3,
            None4,
            None5,
            None6,
            None7,
            None8,
            None9,
            None10,
            EvalWorldStateExpression
        };

        const char * Names[] =
        {
            "None",
            "Random",
            "Month",
            "Day",
            "TimeOfDay",
            "Region",
            "ClockHour",
            "DifficultyID",
            "HolidayStart",
            "HolidayLeft",
            "HolidayActive",
            "TimerCurrentTime",
            "WeekNumber",
            "None2",
            "None3",
            "None4",
            "None5",
            "None6",
            "None7",
            "None8",
            "None9",
            "None10",
            "EvalWorldStateExpression"
        };
    }

    static std::function<int32(Player*, int32, int32)> g_WorldStateExpressionFunction[] =
    {
        /// WorldStateExpressionFunctions::None
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::Random
        [](Player* /*p_Player*/, int32 p_Min, int32 p_Max) -> int32
        {
            return urand(p_Min, p_Max);
        },
        /// WorldStateExpressionFunctions::Month
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            MS::Utilities::WowTime l_Time;
            l_Time.SetUTCTimeFromPosixTime(sWorld->GetGameTime());

            return l_Time.Month + 1;
        },
        /// WorldStateExpressionFunctions::Day
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            MS::Utilities::WowTime l_Time;
            l_Time.SetUTCTimeFromPosixTime(sWorld->GetGameTime());

            return l_Time.MonthDay + 1;
        },
        /// WorldStateExpressionFunctions::TimeOfDay
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            MS::Utilities::WowTime l_Time;
            l_Time.SetUTCTimeFromPosixTime(sWorld->GetGameTime());

            return l_Time.GetHourAndMinutes();
        },
        /// WorldStateExpressionFunctions::Region
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return sWorld->GetServerRegionID();
        },
        /// WorldStateExpressionFunctions::ClockHour
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            MS::Utilities::WowTime l_Time;
            l_Time.SetUTCTimeFromPosixTime(sWorld->GetGameTime());

            if (l_Time.Hour <= 12)
                return l_Time.Hour;

            if (l_Time.Hour - 12)
                return l_Time.Hour - 12;

            return 12;
        },
        /// WorldStateExpressionFunctions::DifficultyID
        [](Player* p_Player, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            if (!p_Player)
                return 0;

            return p_Player->GetMap()->GetDifficultyID();
        },
        /// WorldStateExpressionFunctions::HolidayStart
        [](Player* /*p_Player*/, int32 p_HolidayID, int32 p_Arg2) -> int32
        {
            HolidaysEntry const* l_Entry = sHolidaysStore.LookupEntry(p_HolidayID);

            if (!l_Entry || (p_Arg2 > 0 && !l_Entry->Duration[p_Arg2]))
                return 0;

            int l_ChoosedDuration = l_Entry->Duration[p_Arg2];
            if (!l_ChoosedDuration)
                l_ChoosedDuration = 24;

            time_t l_CurrentTime = sWorld->GetGameTime();
            struct tm l_LocalTime;
            l_LocalTime.tm_isdst = -1;

            ACE_OS::localtime_r(&l_CurrentTime, &l_LocalTime);

            MS::Utilities::WowTime l_GameTime;
            l_GameTime.SetUTCTimeFromPosixTime(l_CurrentTime);
            l_GameTime.YearDay = l_LocalTime.tm_yday;

            for (uint32 l_I = 0; l_I < MAX_HOLIDAY_DATES; ++l_I)
            {
                MS::Utilities::WowTime l_Time;
                l_Time.Decode(l_Entry->Date[l_I]);

                if (l_Entry->flags & 1)
                {
                    l_Time.YearDay = l_GameTime.YearDay;
                    l_Time.ComputeRegionTime(l_Time);
                }

                if (l_Time.Minute   < 0) l_Time.Minute      = l_GameTime.Minute;
                if (l_Time.Hour     < 0) l_Time.Hour        = l_GameTime.Hour;
                if (l_Time.MonthDay < 0) l_Time.MonthDay    = l_GameTime.Minute;
                if (l_Time.Month    < 0) l_Time.Month       = l_GameTime.Month;
                if (l_Time.Year     < 0)
                {
                    l_Time.Year = l_GameTime.Year;

                    if (l_GameTime < l_Time)
                        --l_Time.Year;
                }

                if (l_Entry->Looping)
                {
                    int32 l_I = 0;
                    int32 v7 = 0;
                    int32 v8 = 0;
                    do
                    {
                        if (l_I >= p_Arg2)
                        {
                            if (l_I > p_Arg2)
                                v8 += l_Entry->Duration[l_I];
                        }
                        else
                        {
                            v7 += l_Entry->Duration[l_I];
                            if (!l_I && !l_Entry->Duration[0])
                                v7 += 24;
                        }
                        ++l_I;
                    } while (l_I < MAX_HOLIDAY_DURATIONS);

                    int v10 = MS::Utilities::Globals::InMinutes::Hour * v7;
                    int v11 = MS::Utilities::Globals::InMinutes::Hour * v8;
                    while (!(l_GameTime <= l_Time))
                    {
                        l_Time.AddHolidayDuration(v10);
                        l_Time.AddHolidayDuration(l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour);
                        if (!(l_GameTime >= l_Time))
                        {
                            int l_Result = (MS::Utilities::Globals::InMinutes::Day * (l_Time.GetDaysSinceEpoch() - l_GameTime.GetDaysSinceEpoch())) - l_GameTime.GetHourAndMinutes();
                            return l_Result + l_Time.GetHourAndMinutes();
                        }
                        l_Time.AddHolidayDuration(v11);
                    }
                    return l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour;
                }

                if (l_GameTime <= l_Time)
                {
                    for (int32 l_Y = 0; l_Y < p_Arg2 && l_Y < MAX_HOLIDAY_DURATIONS; ++l_Y)
                    {
                        uint32 l_Value = l_Entry->Duration[l_Y];
                        if (!l_Value)
                            l_Value = 24;

                        l_Time.AddHolidayDuration(l_Value * MS::Utilities::Globals::InMinutes::Hour);
                    }

                    l_Time.AddHolidayDuration(l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour);

                    if (!(l_GameTime >= l_Time))
                        return l_Time.DiffTime(l_GameTime) / MS::Utilities::Globals::InMinutes::Hour;
                }
            }

            return l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour;
        },
        /// WorldStateExpressionFunctions::HolidayLeft
        [](Player* /*p_Player*/, int32 p_HolidayID, int32 p_Arg2) -> int32
        {
            HolidaysEntry const* l_Entry = sHolidaysStore.LookupEntry(p_HolidayID);

            if (!l_Entry || (p_Arg2 <= 0 && l_Entry->Duration[p_Arg2]))
                return 0;

            int l_ChoosedDuration = l_Entry->Duration[p_Arg2];
            if (!l_ChoosedDuration)
                l_ChoosedDuration = 24;

            time_t l_CurrentTime = sWorld->GetGameTime();
            struct tm l_LocalTime;
            l_LocalTime.tm_isdst = -1;

            ACE_OS::localtime_r(&l_CurrentTime, &l_LocalTime);

            MS::Utilities::WowTime l_GameTime;
            l_GameTime.SetUTCTimeFromPosixTime(l_CurrentTime);
            l_GameTime.YearDay = l_LocalTime.tm_yday;

            for (uint32 l_I = 0; l_I < MAX_HOLIDAY_DATES; ++l_I)
            {
                MS::Utilities::WowTime l_Time;
                l_Time.Decode(l_Entry->Date[l_I]);

                if (l_Entry->flags & 1)
                {
                    l_Time.YearDay = l_GameTime.YearDay;
                    l_Time.ComputeRegionTime(l_Time);
                }

                if (l_Time.Minute   < 0) l_Time.Minute      = l_GameTime.Minute;
                if (l_Time.Hour     < 0) l_Time.Hour        = l_GameTime.Hour;
                if (l_Time.MonthDay < 0) l_Time.MonthDay    = l_GameTime.Minute;
                if (l_Time.Month    < 0) l_Time.Month       = l_GameTime.Month;
                if (l_Time.Year     < 0)
                {
                    l_Time.Year = l_GameTime.Year;

                    if (l_GameTime < l_Time)
                        --l_Time.Year;
                }

                if (l_Entry->Looping)
                {
                    int32 l_I = 0;
                    int32 v7 = 0;
                    int32 v8 = 0;
                    do
                    {
                        if (l_I >= p_Arg2)
                        {
                            if (l_I > p_Arg2)
                                v8 += l_Entry->Duration[l_I];
                        }
                        else
                        {
                            v7 += l_Entry->Duration[l_I];
                            if (!l_I && !l_Entry->Duration[0])
                                v7 += 24;
                        }
                        ++l_I;
                    } while (l_I < MAX_HOLIDAY_DURATIONS);

                    int v10 = MS::Utilities::Globals::InMinutes::Hour * v7;
                    int v11 = MS::Utilities::Globals::InMinutes::Hour * v8;
                    while (!(l_GameTime <= l_Time))
                    {
                        l_Time.AddHolidayDuration(v10);
                        l_Time.AddHolidayDuration(l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour);
                        if (!(l_GameTime >= l_Time))
                        {
                            int l_Result = (MS::Utilities::Globals::InMinutes::Day * (l_Time.GetDaysSinceEpoch() - l_GameTime.GetDaysSinceEpoch())) - l_GameTime.GetHourAndMinutes();
                            return l_Result + l_Time.GetHourAndMinutes();
                        }
                        l_Time.AddHolidayDuration(v11);
                    }
                    return l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour;
                }

                if (l_GameTime <= l_Time)
                {
                    if (p_Arg2 > 0)
                    {
                        for (int32 l_Y = 0; l_Y < p_Arg2 && l_Y < MAX_HOLIDAY_DURATIONS; ++l_Y)
                        {
                            uint32 l_Value = l_Entry->Duration[l_Y];
                            if (!l_Value)
                                l_Value = 24;

                            l_Time.AddHolidayDuration(l_Value * MS::Utilities::Globals::InMinutes::Hour);
                        }
                    }

                    l_Time.AddHolidayDuration(l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour);

                    if (!(l_GameTime >= l_Time))
                        return l_Time.DiffTime(l_GameTime) / MS::Utilities::Globals::InMinutes::Hour;
                }
            }

            return l_ChoosedDuration * MS::Utilities::Globals::InMinutes::Hour;
        },
        /// WorldStateExpressionFunctions::HolidayActive
        [](Player* p_Player, int32 p_HolidayID, int32 /*p_Arg2*/) -> int32
        {
            HolidaysEntry const* l_Entry = sHolidaysStore.LookupEntry(p_HolidayID);

            if (l_Entry)
            {
                int l_I = 0;
                uint32 const* l_Durations = (uint32 const*)l_Entry->Duration;
                while (l_I <= 0 || *l_Durations)
                {
                    signed int l_CurrentDuration = *l_Durations;

                    if (!l_CurrentDuration)
                        l_CurrentDuration = 24;

                    if (g_WorldStateExpressionFunction[WorldStateExpressionFunctions::HolidayStart](p_Player, p_HolidayID, l_I) < (MS::Utilities::Globals::InMinutes::Hour * l_CurrentDuration))
                        return 1;

                    ++l_I;
                    l_Durations += 1;

                    if (l_I >= MAX_HOLIDAY_DURATIONS)
                        return 0;
                }
            }

            return 0;
        },
        /// WorldStateExpressionFunctions::TimerCurrentTime
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return time(nullptr);
        },
        /// WorldStateExpressionFunctions::WeekNumber
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            time_t l_Time = sWorld->GetGameTime();
            return (l_Time - sWorld->GetServerRaidOrigin()) / WEEK;
        },
        /// WorldStateExpressionFunctions::None2
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::None3
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::None4
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::None5
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::None6
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::None7
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::None8
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::None9
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::None10
        [](Player* /*p_Player*/, int32 /*p_Arg1*/, int32 /*p_Arg2*/) -> int32
        {
            return 0;
        },
        /// WorldStateExpressionFunctions::EvalWorldStateExpression
        [](Player* p_Player, int32 p_Arg1, int32 /*p_Arg2*/) -> int32
        {
            WorldStateExpressionEntry const* l_WorldStateExpressionEntry = sWorldStateExpressionStore.LookupEntry(p_Arg1);
            if (!l_WorldStateExpressionEntry)
                return 0;

            return l_WorldStateExpressionEntry->Eval(p_Player) ? 1 : 0;
        }
    };

    /// 19865 - sub_A671A3
    bool UnkSubFunction(char p_Char)
    {
        /// @TODO NEED MORE WORK
        ///if (dword_1368528)
        ///{
        ///    return !!sub_A67152(p_Char, 0);
        ///}

        uint16_t l_Value = *(uint16_t*)(&g_WorldState_UnpackUnkTable[p_Char * 2]);
        return !!(l_Value & 4);
    }

    std::string UnpackWorldStateExpression(char const* p_Input)
    {
        char const* l_ExpressionStr = p_Input;
        std::string l_Unpacked;

        while (*l_ExpressionStr)
        {
            char l_Out = 0;
            char l_Temp = 0;
            if (!UnkSubFunction(l_ExpressionStr[0]))
                l_Temp = l_ExpressionStr[0] - '7';
            else
                l_Temp = l_ExpressionStr[0] - '0';

            l_Out |= l_Temp;
            l_Out *= 16;

            if (!UnkSubFunction(l_ExpressionStr[1]))
                l_Temp = l_ExpressionStr[1] - '7';
            else
                l_Temp = l_ExpressionStr[1] - '0';

            l_Out |= l_Temp;
            l_Unpacked += l_Out;

            l_ExpressionStr += 2;
        }

        return l_Unpacked;
    }

#ifdef _MSC_VER
    struct SetParenthesis
    {
        SetParenthesis(std::ostringstream& p_ReadableExpression) : l_ReadableExpression(p_ReadableExpression)
        {
            l_ReadableExpression << "(";
        }

        ~SetParenthesis()
        {
            l_ReadableExpression << ")";
        }

        std::ostringstream& l_ReadableExpression;
    };
#endif

    int32 WorldStateExpression_EvalPush(Player* p_Player, char const** p_UnpackedExpression, std::vector<std::string>& p_Instructions, std::ostringstream& p_ReadableExpression)
    {
    #define UNPACK_UINT8(x) { x = *((uint8*)((char const*)*p_UnpackedExpression)); *p_UnpackedExpression += sizeof(uint8);}
    #define UNPACK_INT32(x) { x = *((int32*)((char const*)*p_UnpackedExpression)); *p_UnpackedExpression += sizeof(int32);}
        uint8 l_OpType;
        UNPACK_UINT8(l_OpType);

        if (l_OpType == WorldStateExpressionCustomOpType::PushUInt32)
        {
            int32 l_Value;
            UNPACK_INT32(l_Value);

    #ifdef _MSC_VER
            p_Instructions.push_back("mov eax, " + std::to_string(l_Value));
    #endif
            p_ReadableExpression << l_Value;
            return l_Value;
        }
        else if (l_OpType == WorldStateExpressionCustomOpType::PushWorldStateValue)
        {
            int32 l_WorldStateID;
            UNPACK_INT32(l_WorldStateID);

    #ifdef _MSC_VER
            p_Instructions.push_back("push " + std::to_string(l_WorldStateID));
            p_Instructions.push_back("call GetWorldStateValue");
    #endif
            p_ReadableExpression << "GetWorldStateValue("<< l_WorldStateID <<")";

            if (p_Player)
                return p_Player->GetWorldState(l_WorldStateID);
        }
        else if (l_OpType == WorldStateExpressionCustomOpType::CallFunction)
        {
            int32 l_FunctionID;
            UNPACK_INT32(l_FunctionID);

            p_ReadableExpression << std::string(WorldStateExpressionFunctions::Names[l_FunctionID]) << "(";
            int l_Arg1 = WorldStateExpression_EvalPush(p_Player, p_UnpackedExpression, p_Instructions, p_ReadableExpression);
            p_ReadableExpression << ", ";;
            int l_Arg2 = WorldStateExpression_EvalPush(p_Player, p_UnpackedExpression, p_Instructions, p_ReadableExpression);
            p_ReadableExpression << ")";

            if (l_FunctionID > (sizeof(g_WorldStateExpressionFunction) / sizeof(g_WorldStateExpressionFunction[0])))
                return (uint32)-1;

    #ifdef _MSC_VER
            p_Instructions.push_back("push " + std::to_string(l_Arg1));
            p_Instructions.push_back("push " + std::to_string(l_Arg2));
            p_Instructions.push_back("call " + std::string(WorldStateExpressionFunctions::Names[l_FunctionID]));
    #endif
            return g_WorldStateExpressionFunction[l_FunctionID](p_Player, l_Arg1, l_Arg2);
        }

        return (uint32)-1;
    #undef UNPACK_INT32
    #undef UNPACK_UINT8
    }

    int32 WorldStateExpression_EvalArithmetic(Player* p_Player, char const** p_UnpackedExpression, std::vector<std::string>& p_Instructions, bool p_ForA, std::ostringstream& p_ReadableExpression)
    {
        UNUSED(p_ForA);
    #ifdef _MSC_VER
        SetParenthesis l_Parenthesis(p_ReadableExpression);
    #endif
    #define UNPACK_UINT8(x) { x = *(uint8_t*)(*p_UnpackedExpression); *p_UnpackedExpression += sizeof(uint8_t);}
        int l_LeftValue = WorldStateExpression_EvalPush(p_Player, p_UnpackedExpression, p_Instructions, p_ReadableExpression);
        char l_Opperand;
        UNPACK_UINT8(l_Opperand);

        if (!l_Opperand)
        {
    #ifdef _MSC_VER
            p_Instructions.push_back(p_ForA ? "mov pA, eax" : "mov pB, eax");
    #endif
            return l_LeftValue;
        }

    #ifdef _MSC_VER
        p_Instructions.push_back("mov pC, eax");


        switch (l_Opperand)
        {
            case WorldStateExpressionMathOpcode::Add:
                p_ReadableExpression << " + ";
                break;
            case WorldStateExpressionMathOpcode::Substract:
                p_ReadableExpression << " - ";
                break;
            case WorldStateExpressionMathOpcode::Multiply:
                p_ReadableExpression << " * ";
                break;
            case WorldStateExpressionMathOpcode::Divide:
                p_ReadableExpression << " / ";
                break;
            case WorldStateExpressionMathOpcode::Modulo:
                p_ReadableExpression << " % ";
                break;
            default:
                break;
        }
    #endif

        int l_RightValue = WorldStateExpression_EvalPush(p_Player, p_UnpackedExpression, p_Instructions, p_ReadableExpression);

        switch (l_Opperand)
        {
            case WorldStateExpressionMathOpcode::Add:
    #ifdef _MSC_VER
                p_Instructions.push_back(p_ForA ? "mov pA, [pC + eax]" : "mov pB, [pC + eax]");
    #endif
                return l_RightValue + l_LeftValue;
                break;
            case WorldStateExpressionMathOpcode::Substract:
    #ifdef _MSC_VER
                p_Instructions.push_back(p_ForA ? "mov pA, [pC - eax]" : "mov pB, [pC - eax]");
    #endif
                return l_LeftValue - l_RightValue;
                break;
            case WorldStateExpressionMathOpcode::Multiply:
    #ifdef _MSC_VER
                p_Instructions.push_back(p_ForA ? "mov pA, [pC * eax]" : "mov pB, [pC * eax]");
    #endif
                return l_LeftValue * l_RightValue;
                break;
            case WorldStateExpressionMathOpcode::Divide:
    #ifdef _MSC_VER
                p_Instructions.push_back(p_ForA ? "mov pA, [pC / eax]" : "mov pB, [pC / eax]");
    #endif
                if (!l_RightValue)
                    return 0;

                return l_LeftValue / l_RightValue;
                break;
            case WorldStateExpressionMathOpcode::Modulo:
    #ifdef _MSC_VER
                p_Instructions.push_back(p_ForA ? "mov pA, [pC % eax]" : "mov pB, [pC % eax]");
    #endif
                if (!l_RightValue)
                    return 0;

                return l_LeftValue % l_RightValue;
                break;
            default:
                break;
        }

        return 0;
    #undef UNPACK_UINT8
    }

    bool WorldStateExpression_EvalCompare(Player* p_Player, char const** p_UnpackedExpression, std::vector<std::string>& p_Instructions, std::ostringstream& p_ReadableExpression)
    {
    #define UNPACK_UINT8(x) { x = *(uint8*)(*p_UnpackedExpression); *p_UnpackedExpression += sizeof(uint8);}
#ifdef _MSC_VER
        SetParenthesis l_Parenthesis(p_ReadableExpression);
#endif
        int l_LeftValue = WorldStateExpression_EvalArithmetic(p_Player, p_UnpackedExpression, p_Instructions, true, p_ReadableExpression);
        char l_Opperand;
        UNPACK_UINT8(l_Opperand);

        if (!l_Opperand)
        {
    #ifdef _MSC_VER
            p_Instructions.push_back("mov eax, pA");
    #endif
            return !!l_LeftValue;
        }

#ifdef _MSC_VER
        switch (l_Opperand)
        {
            case WorldStateExpressionCompareOpcode::Equal:
                p_ReadableExpression << " == ";
                break;
            case WorldStateExpressionCompareOpcode::Different:
                p_ReadableExpression << " != ";
                break;
            case WorldStateExpressionCompareOpcode::Less:
                p_ReadableExpression << " < ";
                break;
            case WorldStateExpressionCompareOpcode::LessOrEqual:
                p_ReadableExpression << " <= ";
                break;
            case WorldStateExpressionCompareOpcode::More:
                p_ReadableExpression << " > ";
                break;
            case WorldStateExpressionCompareOpcode::MoreOrEqual:
                p_ReadableExpression << " >= ";
                break;
            default:
                break;
        }
#endif

        int l_RightValue = WorldStateExpression_EvalArithmetic(p_Player, p_UnpackedExpression, p_Instructions, false, p_ReadableExpression);

        switch (l_Opperand)
        {
            case WorldStateExpressionCompareOpcode::Equal:
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [pA == pB]");
    #endif
                return l_LeftValue == l_RightValue;
            case WorldStateExpressionCompareOpcode::Different:
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [pA != pB]");
    #endif
                return l_LeftValue != l_RightValue;
            case WorldStateExpressionCompareOpcode::Less:
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [pA < pB]");
    #endif
                return l_LeftValue < l_RightValue;
            case WorldStateExpressionCompareOpcode::LessOrEqual:
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [pA <= pB]");
    #endif
                return l_LeftValue <= l_RightValue;
            case WorldStateExpressionCompareOpcode::More:
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [pA > pB]");
    #endif
                return l_LeftValue > l_RightValue;
            case WorldStateExpressionCompareOpcode::MoreOrEqual:
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [pA >= pB]");
    #endif
                return l_LeftValue >= l_RightValue;
            default:
                break;
        }

        return false;
    #undef UNPACK_UINT8
    }

    bool WorldStateExpression_EvalResult(char p_LogicResult, uint8_t p_EvalResultRoutineID, bool p_EvalResult, uint8_t p_PrevResultRoutineID, bool p_PrevResult, std::vector<std::string>& p_Instructions)
    {
        bool l_Result = false;

        switch (p_LogicResult)
        {
            case WorldStateExpressionEvalResultLogic::FirstAndSecond:
                l_Result = p_EvalResult != 0 && p_PrevResult != 0;
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [pA && pB]");
    #endif
                break;

            case WorldStateExpressionEvalResultLogic::FirstOrSecond:
                l_Result = (p_PrevResult || p_EvalResult) != 0;
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [pA || pB]");
    #endif
                break;

            case WorldStateExpressionEvalResultLogic::NotFirstOrNotSecond:
    #ifdef _MSC_VER
                p_Instructions.push_back("mov eax, [(pA || pB) ? (!pA || !pB) : 0]");
    #endif
                if (p_PrevResult || p_EvalResult)
                    l_Result = p_PrevResult == 0;
                break;
        }

        return l_Result;
    }

    uint32 WorldStateExpressionEntry::GetRequiredWorldStatesIfExists(std::vector<std::pair<uint32, uint32>>* p_WorldStates /*= nullptr*/) const
    {
        std::vector<std::string> p_Instructions;
        std::vector<std::string> p_InstructionsSecond;

        if (!Expression || !strlen(Expression->Get(sWorld->GetDefaultDb2Locale())))
            return 0;

        std::string l_UnpackedExpressionString = UnpackWorldStateExpression(Expression->Get(sWorld->GetDefaultDb2Locale()));
        char const* l_UnpackedExpression = l_UnpackedExpressionString.c_str();

        do
        {
            std::pair<uint32, uint32> l_WorldState;
            char l_Value = *l_UnpackedExpression;
            l_UnpackedExpression += sizeof(uint8);

            if (l_Value != 1)
                break;

            char const** p_UnpackedExpression = &l_UnpackedExpression;

            uint8 l_OpType = *((uint8*)((char const*)*p_UnpackedExpression));
            *p_UnpackedExpression += sizeof(uint8);

            if (l_OpType != WorldStateExpressionCustomOpType::PushWorldStateValue)
                break;

            l_WorldState.first = *((int32*)((char const*)*p_UnpackedExpression));
            *p_UnpackedExpression += sizeof(int32);

            uint8 l_Zero = *((uint8*)((char const*)*p_UnpackedExpression));
            *p_UnpackedExpression += sizeof(uint8);

            if (l_Zero != 0)
                break;

            l_Value = **p_UnpackedExpression;
            *p_UnpackedExpression += sizeof(uint8);

            if (l_Value != 1)
                break;

            l_OpType = *((uint8*)((char const*)*p_UnpackedExpression));
            *p_UnpackedExpression += sizeof(uint8);

            if (l_OpType != WorldStateExpressionCustomOpType::PushUInt32)
                break;

            l_WorldState.second = *((int32*)((char const*)*p_UnpackedExpression));
            *p_UnpackedExpression += sizeof(int32);

            l_Zero = *((uint8*)((char const*)*p_UnpackedExpression));
            *p_UnpackedExpression += sizeof(uint8);

            p_WorldStates->push_back(l_WorldState);

            if (l_Zero != 0)
                break;

        } while (true);

        return p_WorldStates->size() ? p_WorldStates->front().first : 0;
    }

    /// Eval a worldstate expression
    bool WorldStateExpressionEntry::Eval(Player* p_Player, std::vector<std::string> * p_OutStrResult) const
    {
    #define UNPACK_UINT8(x) { x = *l_UnpackedExpression; l_UnpackedExpression += sizeof(uint8);}
        std::vector<std::string> p_Instructions;
        std::vector<std::string> p_InstructionsSecond;

        if (this->Expression && strlen(this->Expression->Get(sWorld->GetDefaultDb2Locale())))
        {
            std::ostringstream l_ReadableExpression;
            std::ostringstream l_RoutinesResults;
            std::string l_UnpackedExpressionString = UnpackWorldStateExpression(this->Expression->Get(sWorld->GetDefaultDb2Locale()));
            char const* l_UnpackedExpression = l_UnpackedExpressionString.c_str();

            char l_Value;
            UNPACK_UINT8(l_Value);

            if (l_Value == 1)
            {
    #ifdef _MSC_VER
                p_Instructions.push_back("========= routine 1 =========");
    #endif
                bool l_Result = WorldStateExpression_EvalCompare(p_Player, &l_UnpackedExpression, p_Instructions, l_ReadableExpression);
    #ifdef _MSC_VER
                l_RoutinesResults << l_Result ? "true" : "false";
                p_Instructions.push_back("=============================");
    #endif
                char l_ResultLogic;
                UNPACK_UINT8(l_ResultLogic);

                if (l_ResultLogic)
                {
                    switch (l_ResultLogic)
                    {
                        case WorldStateExpressionEvalResultLogic::FirstAndSecond:
                            l_RoutinesResults << " && ";
                            l_ReadableExpression << " && ";
                            break;
                        case WorldStateExpressionEvalResultLogic::FirstOrSecond:
                            l_RoutinesResults << " || ";
                            l_ReadableExpression << " || ";
                            break;
                        case WorldStateExpressionEvalResultLogic::NotFirstOrNotSecond:
                            l_RoutinesResults << " ^ ";
                            l_ReadableExpression << " ^ ";
                            break;
                        default:
                            break;
                    }
    #ifdef _MSC_VER
                    p_Instructions.push_back("========= routine 2 =========");
    #endif
                    bool l_Routine2 = WorldStateExpression_EvalCompare(p_Player, &l_UnpackedExpression, p_Instructions, l_ReadableExpression);
    #ifdef _MSC_VER
                    l_RoutinesResults << l_Routine2 ? "true" : "false";
                    p_Instructions.push_back("=============================");

                    p_InstructionsSecond.push_back("call Routine1");
                    p_InstructionsSecond.push_back("mov pA, eax");
                    p_InstructionsSecond.push_back("call Routine2");
                    p_InstructionsSecond.push_back("mov pB, eax");
    #endif
                    l_Result = WorldStateExpression_EvalResult(l_ResultLogic, 2, l_Routine2, 1, l_Result, p_InstructionsSecond);
                    UNPACK_UINT8(l_ResultLogic);

                    if (l_ResultLogic)
                    {
                        switch (l_ResultLogic)
                        {
                            case WorldStateExpressionEvalResultLogic::FirstAndSecond:
                                l_RoutinesResults << " && ";
                                l_ReadableExpression << " && ";
                                break;
                            case WorldStateExpressionEvalResultLogic::FirstOrSecond:
                                l_RoutinesResults << " || ";
                                l_ReadableExpression << " || ";
                                break;
                            case WorldStateExpressionEvalResultLogic::NotFirstOrNotSecond:
                                l_RoutinesResults << " ^ ";
                                l_ReadableExpression << " ^ ";
                                break;
                            default:
                                break;
                        }
    #ifdef _MSC_VER
                        p_Instructions.push_back("========= routine 3 =========");
    #endif
                        bool l_Routine3 = WorldStateExpression_EvalCompare(p_Player, &l_UnpackedExpression, p_Instructions, l_ReadableExpression);
    #ifdef _MSC_VER
                        l_RoutinesResults << l_Routine3 ? "true" : "false";
                        p_Instructions.push_back("=============================");

                        p_InstructionsSecond.push_back("mov pA, eax");
                        p_InstructionsSecond.push_back("call Routine3");
                        p_InstructionsSecond.push_back("mov pB, eax");
    #endif
                        l_Result = WorldStateExpression_EvalResult(l_ResultLogic, 3, l_Routine3, 2, l_Result, p_InstructionsSecond);
                        UNPACK_UINT8(l_ResultLogic);
                    }
                }

                for (auto l_Line : p_InstructionsSecond)
                    p_Instructions.push_back("$> " + l_Line);

    #ifdef _MSC_VER
                if (p_OutStrResult)
                {
                 //   *p_OutStrResult = p_Instructions;
                    p_Player->SendSysMessage(l_ReadableExpression.str().c_str());
                    p_Player->SendSysMessage(l_RoutinesResults.str().c_str());
                }
    #endif

                return l_Result;
            }
        }

        return false;
    #undef UNPACK_UINT8
    }

#pragma  endregion

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

using StoreProblemList = std::list<std::string>;
uint32 DB2FilesCount = 0;

static bool LoadDB2_assert_print(uint32 p_FileSize, uint32 p_ReadSize, const std::string& p_FileName)
{
    sLog->outError(LOG_FILTER_GENERAL, "Size of '%s' set by format string (%u) not equal size of C++ structure (%u).", p_FileName.c_str(), p_FileSize, p_ReadSize);

    /// ASSERT must fail after function call
    return false;
}

/// Load a DB2 store
/// @p_ErrorList        : Error list storage
/// @p_Storage          : Destination storage
/// @p_FileName         : File name to load
/// @p_CustomTableName  : Hotfix SQL table name
/// @p_CustomIndexName  : Hotfix SQL table index name
template<class T>
inline void LoadDB2(StoreProblemList& p_ErrorList, DB2Storage<T>& p_Storage, const std::string& p_DB2Path, const std::string& p_FileName)
{
    std::string p_CustomTableName = "retail_";
    std::string p_CustomIndexName = "ROW_ID";

    std::string l_TempName = p_FileName.substr(0, p_FileName.find_last_of('.'));

    for (int l_I = 0; l_I < l_TempName.size(); ++l_I)
    {
        if (l_I > 0 && isupper(l_TempName[l_I]))
            p_CustomTableName += "_";

        char l_Char = ((char)tolower(l_TempName[l_I]));
        if (l_Char == '-')
            l_Char = '_';

        p_CustomTableName += l_Char;
    }
    /// Compatibility format and C++ structure sizes
    ASSERT(DB2FileLoader::GetFormatRecordSizeForDB2Loader(p_Storage.GetFormat()) == sizeof(T) || LoadDB2_assert_print(DB2FileLoader::GetFormatRecordSizeForDB2Loader(p_Storage.GetFormat()), sizeof(T), p_FileName));

    ++DB2FilesCount;

    SqlDb2 * l_SQLDB2 = NULL;
    if (!p_CustomTableName.empty())
        l_SQLDB2 = new SqlDb2(p_CustomTableName, p_CustomIndexName, p_Storage.GetFormat());

    std::string l_DB2FileName = p_DB2Path + (p_Storage.LoadNew() ? "new/" : "") + p_FileName;

    if (!p_Storage.Load(l_DB2FileName.c_str(), l_SQLDB2, sWorld->GetDefaultDb2Locale()))
    {
        /// Sort problematic db2 to (1) non compatible and (2) nonexistent
        if (FILE* l_File = fopen(l_DB2FileName.c_str(), "rb"))
        {
            char l_Buffer[100];

            std::string l_Format = p_Storage.GetFormat();
            snprintf(l_Buffer, 100, "(exist, but have %u fields instead " UI64FMTD " ) Wrong client version DB2 file?", p_Storage.GetFieldCount(), std::count_if(l_Format.begin(), l_Format.end(), [](char p_Char) -> bool
            {
                return !isdigit(p_Char);
            }));

            p_ErrorList.push_back(l_DB2FileName + l_Buffer);
            fclose(l_File);
        }
        else
            p_ErrorList.push_back(l_DB2FileName);
    }

    for (int i = LOCALE_koKR; i < TOTAL_LOCALES; i++)
    {
        std::string l_DB2FileNameLocale = p_DB2Path + localeNames[i] + "/" + p_FileName;

        /// Check if the locale DB2 file exist
        FILE* l_File = fopen(l_DB2FileNameLocale.c_str(), "rb");
        if (!l_File)
            continue;
        fclose(l_File);

        /// Load localized strings
        p_Storage.LoadStringsFrom(l_DB2FileNameLocale.c_str(), i);
    }

    //p_Storage.GenerateSQLOuput(l_DB2FileName.c_str());

    if (sDB2PerHash.find(p_Storage.GetHash()) == sDB2PerHash.end())
        sDB2PerHash[p_Storage.GetHash()] = &p_Storage;
}

void LoadDB2Stores(const std::string& p_DataPath)
{
    std::string l_DB2Path = p_DataPath + "dbc/";
    StoreProblemList l_BadDB2Files;

    uint32 oldMSTime = getMSTime();

    auto threadPool = new ThreadPoolMap();
    threadPool->start(std::max(std::thread::hardware_concurrency(), 8u));

#define LOAD_DB2(store, str) threadPool->schedule([=]() mutable { LoadDB2(l_BadDB2Files, store, l_DB2Path, str); });

    LOAD_DB2(sOverrideSpellDataStore, "OverrideSpellData.db2");
    LOAD_DB2(sSpecializationSpellStore, "SpecializationSpells.db2");
    LOAD_DB2(sSpellStore, "Spell.db2");
    LOAD_DB2(sSpellAuraOptionsStore, "SpellAuraOptions.db2");
    LOAD_DB2(sSpellAuraRestrictionsStore, "SpellAuraRestrictions.db2");
    LOAD_DB2(sSpellCastingRequirementsStore, "SpellCastingRequirements.db2");
    LOAD_DB2(sSpellCastTimesStore, "SpellCastTimes.db2");
    LOAD_DB2(sSpellCategoriesStore, "SpellCategories.db2");
    LOAD_DB2(sSpellCategoryStore, "SpellCategory.db2");
    LOAD_DB2(sSpellClassOptionsStore, "SpellClassOptions.db2");
    LOAD_DB2(sSpellCooldownsStore, "SpellCooldowns.db2");
    LOAD_DB2(sSpellDurationStore, "SpellDuration.db2");
    LOAD_DB2(sSpellEffectStore, "SpellEffect.db2");
    LOAD_DB2(sSpellEquippedItemsStore, "SpellEquippedItems.db2");
    LOAD_DB2(sSpellFocusObjectStore, "SpellFocusObject.db2");
    LOAD_DB2(sSpellInterruptsStore, "SpellInterrupts.db2");
    LOAD_DB2(sSpellItemEnchantmentStore, "SpellItemEnchantment.db2");
    LOAD_DB2(sSpellLevelsStore, "SpellLevels.db2");
    LOAD_DB2(sSpellMiscStore, "SpellMisc.db2");
    LOAD_DB2(sSpellPowerStore, "SpellPower.db2");
    LOAD_DB2(sSpellProcsPerMinuteStore, "SpellProcsPerMinute.db2");
    LOAD_DB2(sSpellProcsPerMinuteModStore, "SpellProcsPerMinuteMod.db2");
    LOAD_DB2(sSpellRadiusStore, "SpellRadius.db2");
    LOAD_DB2(sSpellRangeStore, "SpellRange.db2");
    LOAD_DB2(sSpellReagentsStore, "SpellReagents.db2");
    LOAD_DB2(sSpellReagentsCurrencyStore, "SpellReagentsCurrency.db2");
    LOAD_DB2(sSpellScalingStore, "SpellScaling.db2");
    LOAD_DB2(sSpellShapeshiftStore, "SpellShapeshift.db2");
    LOAD_DB2(sSpellShapeshiftFormStore, "SpellShapeshiftForm.db2");
    LOAD_DB2(sSpellTargetRestrictionsStore, "SpellTargetRestrictions.db2");
    LOAD_DB2(sSpellXSpellVisualStore, "SpellXSpellVisual.db2");
    LOAD_DB2(sSpellTotemsStore, "SpellTotems.db2");
    LOAD_DB2(sGarrSiteLevelStore, "GarrSiteLevel.db2");
    LOAD_DB2(sGarrSiteLevelPlotInstStore, "GarrSiteLevelPlotInst.db2");
    LOAD_DB2(sGarrPlotInstanceStore, "GarrPlotInstance.db2");
    LOAD_DB2(sGarrPlotStore, "GarrPlot.db2");
    LOAD_DB2(sGarrPlotUICategoryStore, "GarrPlotUICategory.db2");
    LOAD_DB2(sGarrMissionStore, "GarrMission.db2");
    LOAD_DB2(sGarrMissionXEncouterStore, "GarrMissionXEncounter.db2");
    LOAD_DB2(sGarrBuildingStore, "GarrBuilding.db2");
    LOAD_DB2(sGarrPlotBuildingStore, "GarrPlotBuilding.db2");
    LOAD_DB2(sGarrFollowerStore, "GarrFollower.db2");
    LOAD_DB2(sGarrClassSpecStore, "GarrClassSpec.db2");
    LOAD_DB2(sGarrFollowerTypeStore, "GarrFollowerType.db2");
    LOAD_DB2(sGarrFollSupportSpellStore, "GarrFollSupportSpell.db2");
    LOAD_DB2(sGarrAbilityStore, "GarrAbility.db2");
    LOAD_DB2(sGarrAbilityEffectStore, "GarrAbilityEffect.db2");
    LOAD_DB2(sGarrFollowerXAbilityStore, "GarrFollowerXAbility.db2");
    LOAD_DB2(sGarrBuildingPlotInstStore, "GarrBuildingPlotInst.db2");
    LOAD_DB2(sGarrMechanicTypeStore, "GarrMechanicType.db2");
    LOAD_DB2(sGarrMechanicStore, "GarrMechanic.db2");
    LOAD_DB2(sGarrEncouterXMechanicStore, "GarrEncounterXMechanic.db2");
    LOAD_DB2(sGarrEncounterSetXEncounterStore, "GarrEncounterSetXEncounter.db2");
    LOAD_DB2(sGarrMechanicSetXMechanicStore, "GarrMechanicSetXMechanic.db2");
    LOAD_DB2(sGarrFollowerLevelXPStore, "GarrFollowerLevelXP.db2");
    LOAD_DB2(sGarrSpecializationStore, "GarrSpecialization.db2");
    LOAD_DB2(sGarrTalentStore, "GarrTalent.db2");
    LOAD_DB2(sGarrTalentTreeStore, "GarrTalentTree.db2");
    LOAD_DB2(sGarrTypeStore, "GarrType.db2");
    LOAD_DB2(sCharShipmentStore, "CharShipment.db2");
    LOAD_DB2(sCharShipmentContainerStore, "CharShipmentContainer.db2");
    LOAD_DB2(sGarrItemLevelUpgradeDataStore, "GarrItemLevelUpgradeData.db2");
    LOAD_DB2(sBattlePetAbilityStore, "BattlePetAbility.db2");
    LOAD_DB2(sBattlePetAbilityEffectStore, "BattlePetAbilityEffect.db2");
    LOAD_DB2(sBattlePetAbilityTurnStore, "BattlePetAbilityTurn.db2");
    LOAD_DB2(sBattlePetAbilityStateStore, "BattlePetAbilityState.db2");
    LOAD_DB2(sBattlePetStateStore, "BattlePetState.db2");
    LOAD_DB2(sBattlePetEffectPropertiesStore, "BattlePetEffectProperties.db2");
    LOAD_DB2(sBattlePetBreedQualityStore, "BattlePetBreedQuality.db2");
    LOAD_DB2(sBattlePetBreedStateStore, "BattlePetBreedState.db2");
    LOAD_DB2(sBattlePetSpeciesStore, "BattlePetSpecies.db2");
    LOAD_DB2(sBattlePetSpeciesStateStore, "BattlePetSpeciesState.db2");
    LOAD_DB2(sBattlePetSpeciesXAbilityStore, "BattlePetSpeciesXAbility.db2");
    LOAD_DB2(sAchievementStore, "Achievement.db2");
    LOAD_DB2(sAchievementCategoryStore, "Achievement_Category.db2");
    LOAD_DB2(sModifierTreeStore, "ModifierTree.db2");
    LOAD_DB2(sCriteriaStore, "Criteria.db2");
    LOAD_DB2(sCriteriaTreeStore, "CriteriaTree.db2");
    LOAD_DB2(sTaxiNodesStore, "TaxiNodes.db2");
    LOAD_DB2(sTaxiPathStore, "TaxiPath.db2");
    LOAD_DB2(sTaxiPathNodeStore, "TaxiPathNode.db2");
    LOAD_DB2(sArtifactStore, "Artifact.db2");
    LOAD_DB2(sArtifactAppearanceStore, "ArtifactAppearance.db2");
    LOAD_DB2(sArtifactAppearanceSetStore, "ArtifactAppearanceSet.db2");
    LOAD_DB2(sArtifactCategoryStore, "ArtifactCategory.db2");
    LOAD_DB2(sArtifactPowerStore, "ArtifactPower.db2");
    LOAD_DB2(sArtifactPowerPickerStore, "ArtifactPowerPicker.db2");
    LOAD_DB2(sArtifactPowerLinkStore, "ArtifactPowerLink.db2");
    LOAD_DB2(sArtifactPowerRankStore, "ArtifactPowerRank.db2");
    LOAD_DB2(sArtifactQuestXPStore, "ArtifactQuestXP.db2");
    LOAD_DB2(sRelicTalentStore, "RelicTalent.db2");
    LOAD_DB2(sSceneScriptPackageStore, "SceneScriptPackage.db2");
    LOAD_DB2(sWbAccessControlListStore, "WbAccessControlList.db2");
    LOAD_DB2(sWbCertWhitelistStore, "WbCertWhitelist.db2");
    LOAD_DB2(sItemPriceBaseStore, "ItemPriceBase.db2");
    LOAD_DB2(sItemClassStore, "ItemClass.db2");
    LOAD_DB2(sItemDisenchantLootStore, "ItemDisenchantLoot.db2");
    LOAD_DB2(sItemLimitCategoryStore, "ItemLimitCategory.db2");
    LOAD_DB2(sItemLimitCategoryConditionStore, "ItemLimitCategoryCondition.db2");
    LOAD_DB2(sItemRandomPropertiesStore, "ItemRandomProperties.db2");
    LOAD_DB2(sItemRandomSuffixStore, "ItemRandomSuffix.db2");
    LOAD_DB2(sItemSpecStore, "ItemSpec.db2");
    LOAD_DB2(sItemSpecOverrideStore, "ItemSpecOverride.db2");
    LOAD_DB2(sItemStore, "Item.db2");
    LOAD_DB2(sItemBonusStore, "ItemBonus.db2");
    LOAD_DB2(sItemBonusTreeNodeStore, "ItemBonusTreeNode.db2");
    LOAD_DB2(sItemXBonusTreeStore, "ItemXBonusTree.db2");
    LOAD_DB2(sItemCurrencyCostStore, "ItemCurrencyCost.db2");
    LOAD_DB2(sItemExtendedCostStore, "ItemExtendedCost.db2");
    LOAD_DB2(sItemSparseStore, "ItemSparse.db2");
    LOAD_DB2(sItemEffectStore, "ItemEffect.db2");
    LOAD_DB2(sItemModifiedAppearanceStore, "ItemModifiedAppearance.db2");
    LOAD_DB2(sItemAppearanceStore, "ItemAppearance.db2");
    LOAD_DB2(sTransmogSetItemStore, "TransmogSetItem.db2");
    LOAD_DB2(sTransmogSetStore, "TransmogSet.db2");
    LOAD_DB2(sItemUpgradeStore, "ItemUpgrade.db2");
    LOAD_DB2(sItemChildEquipmentStore, "ItemChildEquipment.db2");
    LOAD_DB2(sItemSetSpellStore, "ItemSetSpell.db2");
    LOAD_DB2(sItemArmorQualityStore, "ItemArmorQuality.db2");
    LOAD_DB2(sItemArmorShieldStore, "ItemArmorShield.db2");
    LOAD_DB2(sItemArmorTotalStore, "ItemArmorTotal.db2");
    LOAD_DB2(sItemBagFamilyStore, "ItemBagFamily.db2");
    LOAD_DB2(sItemDamageAmmoStore, "ItemDamageAmmo.db2");
    LOAD_DB2(sItemDamageOneHandStore, "ItemDamageOneHand.db2");
    LOAD_DB2(sItemDamageOneHandCasterStore, "ItemDamageOneHandCaster.db2");
    LOAD_DB2(sItemDamageTwoHandStore, "ItemDamageTwoHand.db2");
    LOAD_DB2(sItemDamageTwoHandCasterStore, "ItemDamageTwoHandCaster.db2");
    LOAD_DB2(sItemSetStore, "ItemSet.db2");
    LOAD_DB2(sItemSearchNameStore, "ItemSearchName.db2");
    LOAD_DB2(sRulesetItemUpgradeStore, "RulesetItemUpgrade.db2");
    LOAD_DB2(sPvpItemStore, "PVPItem.db2");
    LOAD_DB2(sPvpScalingEffectStore, "PvpScalingEffect.db2");
    LOAD_DB2(sItemBonusListLevelDeltaStore, "ItemBonusListLevelDelta.db2");
    LOAD_DB2(sResearchBranchStore, "ResearchBranch.db2");
    LOAD_DB2(sResearchProjectStore, "ResearchProject.db2");
    LOAD_DB2(sResearchSiteStore, "ResearchSite.db2");
    LOAD_DB2(sQuestV2CliTaskStore, "QuestV2CliTask.db2");
    LOAD_DB2(sBountyStore, "Bounty.db2");
    LOAD_DB2(sAdventureMapPOIStore, "AdventureMapPOI.db2");
    LOAD_DB2(sQuestPackageItemStore, "QuestPackageItem.db2");
    LOAD_DB2(sQuestPOIPointStore, "QuestPOIPoint.db2");
    LOAD_DB2(sQuestSortStore, "QuestSort.db2");
    LOAD_DB2(sQuestV2Store, "QuestV2.db2");
    LOAD_DB2(sQuestXPStore, "QuestXP.db2");
    LOAD_DB2(sQuestFactionRewardStore, "QuestFactionReward.db2");
    LOAD_DB2(sQuestMoneyRewardStore, "QuestMoneyReward.db2");
    LOAD_DB2(sAreaTableStore, "AreaTable.db2");
    LOAD_DB2(sAreaGroupMemberStore, "AreaGroupMember.db2");
    LOAD_DB2(sAreaTriggerStore, "AreaTrigger.db2");
    LOAD_DB2(sDifficultyStore, "Difficulty.db2");
    LOAD_DB2(sLiquidTypeStore, "LiquidType.db2");
    LOAD_DB2(sMapStore, "Map.db2");
    LOAD_DB2(sMapDifficultyStore, "MapDifficulty.db2");
    LOAD_DB2(sPhaseStore, "Phase.db2");
    LOAD_DB2(sWorldMapOverlayStore, "WorldMapOverlay.db2");
    LOAD_DB2(sWMOAreaTableStore, "WMOAreaTable.db2");
    LOAD_DB2(sWorldMapAreaStore, "WorldMapArea.db2");
    LOAD_DB2(sWorldMapTransformsStore, "WorldMapTransforms.db2");
    LOAD_DB2(sWorld_PVP_AreaStore, "World_PVP_Area.db2");
    LOAD_DB2(sImportPriceArmorStore, "ImportPriceArmor.db2");
    LOAD_DB2(sImportPriceQualityStore, "ImportPriceQuality.db2");
    LOAD_DB2(sImportPriceShieldStore, "ImportPriceShield.db2");
    LOAD_DB2(sImportPriceWeaponStore, "ImportPriceWeapon.db2");
    LOAD_DB2(sCharStartOutfitStore, "CharStartOutfit.db2");
    LOAD_DB2(sCharTitlesStore, "CharTitles.db2");
    LOAD_DB2(sChrClassesStore, "ChrClasses.db2");
    LOAD_DB2(sChrClassXPowerTypesStore, "ChrClassesXPowerTypes.db2");
    LOAD_DB2(sChrRacesStore, "ChrRaces.db2");
    LOAD_DB2(sChrSpecializationsStore, "ChrSpecialization.db2");
    LOAD_DB2(sCharLoadoutItemStore, "CharacterLoadoutItem.db2");
    LOAD_DB2(sCharLoadoutStore, "CharacterLoadout.db2");
    LOAD_DB2(sCreatureDisplayInfoStore, "CreatureDisplayInfo.db2");
    LOAD_DB2(sCreatureDisplayInfoExtraStore, "CreatureDisplayInfoExtra.db2");
    LOAD_DB2(sCreatureFamilyStore, "CreatureFamily.db2");
    LOAD_DB2(sCreatureModelDataStore, "CreatureModelData.db2");
    LOAD_DB2(sCreatureTypeStore, "CreatureType.db2");
    LOAD_DB2(sAnimKitStore, "AnimKit.db2");
    LOAD_DB2(sArmorLocationStore, "ArmorLocation.db2");
    LOAD_DB2(sAuctionHouseStore, "AuctionHouse.db2");
    LOAD_DB2(sBankBagSlotPricesStore, "BankBagSlotPrices.db2");
    LOAD_DB2(sBarberShopStyleStore, "BarberShopStyle.db2");
    LOAD_DB2(sBattlemasterListStore, "BattlemasterList.db2");
    LOAD_DB2(sBroadcastTextStore, "BroadcastText.db2");
    LOAD_DB2(sCharSectionsStore, "CharSections.db2");
    LOAD_DB2(sCinematicCameraStore, "CinematicCamera.db2");
    LOAD_DB2(sCinematicSequencesStore, "CinematicSequences.db2");
    LOAD_DB2(sChatChannelsStore, "ChatChannels.db2");
    LOAD_DB2(sCurrencyTypesStore, "CurrencyTypes.db2");
    LOAD_DB2(sCurveStore, "Curve.db2");
    LOAD_DB2(sCurvePointStore, "CurvePoint.db2");
    LOAD_DB2(sDestructibleModelDataStore, "DestructibleModelData.db2");
    LOAD_DB2(sDungeonEncounterStore, "DungeonEncounter.db2");
    LOAD_DB2(sDurabilityCostsStore, "DurabilityCosts.db2");
    LOAD_DB2(sDurabilityQualityStore, "DurabilityQuality.db2");
    LOAD_DB2(sEmotesStore, "Emotes.db2");
    LOAD_DB2(sEmotesTextStore, "EmotesText.db2");
    LOAD_DB2(sEmotesTextSoundStore, "EmotesTextSound.db2");
    LOAD_DB2(sFactionStore, "Faction.db2");
    LOAD_DB2(sFactionTemplateStore, "FactionTemplate.db2");
    LOAD_DB2(sGameObjectDisplayInfoStore, "GameObjectDisplayInfo.db2");
    LOAD_DB2(sGemPropertiesStore, "GemProperties.db2");
    LOAD_DB2(sGlyphBindableSpellStore, "GlyphBindableSpell.db2");
    LOAD_DB2(sGlyphPropertiesStore, "GlyphProperties.db2");
    LOAD_DB2(sGlyphRequiredSpecStore, "GlyphRequiredSpec.db2");
    LOAD_DB2(sGroupFinderActivityStore, "GroupFinderActivity.db2");
    LOAD_DB2(sGroupFinderCategoryStore, "GroupFinderCategory.db2");
    LOAD_DB2(sGuildPerkSpellsStore, "GuildPerkSpells.db2");
    LOAD_DB2(sGlobalStringsStore, "GlobalStrings.db2");
    LOAD_DB2(sHeirloomStore, "Heirloom.db2");
    LOAD_DB2(sHolidaysStore, "Holidays.db2");
    LOAD_DB2(sJournalInstanceStore, "JournalInstance.db2");
    LOAD_DB2(sJournalEncounterStore, "JournalEncounter.db2");
    LOAD_DB2(sJournalEncounterItemStore, "JournalEncounterItem.db2");
    LOAD_DB2(sLFGDungeonStore, "LFGDungeons.db2");
    LOAD_DB2(sLocationStore, "Location.db2");
    LOAD_DB2(sLockStore, "Lock.db2");
    LOAD_DB2(sMailTemplateStore, "MailTemplate.db2");
    LOAD_DB2(sMapChallengeModeStore, "MapChallengeMode.db2");
    LOAD_DB2(sMinorTalentStore, "MinorTalent.db2");
    LOAD_DB2(sMountCapabilityStore, "MountCapability.db2");
    LOAD_DB2(sMountStore, "Mount.db2");
    LOAD_DB2(sMountTypeXCapabilityStore, "MountTypeXCapability.db2");
    LOAD_DB2(sMountXDisplayStore, "MountXDisplay.db2");
    LOAD_DB2(sMovieStore, "Movie.db2");
    LOAD_DB2(sNameGenStore, "NameGen.db2");
    LOAD_DB2(sParagonReputationStore, "ParagonReputation.db2");
    LOAD_DB2(sPathNodeStore, "PathNode.db2");
    LOAD_DB2(sPlayerConditionStore, "PlayerCondition.db2");
    LOAD_DB2(sPowerDisplayStore, "PowerDisplay.db2");
    LOAD_DB2(sPowerTypeStore, "PowerType.db2");
    LOAD_DB2(sPrestigeLevelInfoStore, "PrestigeLevelInfo.db2");
    LOAD_DB2(sPvPDifficultyStore, "PVPDifficulty.db2");
    LOAD_DB2(sPvpTalentStore, "PvpTalent.db2");
    LOAD_DB2(sPvpTalentUnlockStore, "PvpTalentUnlock.db2");
    LOAD_DB2(sPvpRewardStore, "PvpReward.db2");
    LOAD_DB2(sRandomPropertiesPointsStore, "RandPropPoints.db2");
    LOAD_DB2(sRewardPackStore, "RewardPack.db2");
    LOAD_DB2(sRewardPackXCurrencyTypeStore, "RewardPackXCurrencyType.db2");
    LOAD_DB2(sRewardPackXItemStore, "RewardPackXItem.db2");
    LOAD_DB2(sScalingStatDistributionStore, "ScalingStatDistribution.db2");
    LOAD_DB2(sSandboxScalingStore, "SandboxScaling.db2");
    LOAD_DB2(sScenarioStore, "Scenario.db2");
    LOAD_DB2(sScenarioStepStore, "ScenarioStep.db2");
    LOAD_DB2(sSkillLineAbilityStore, "SkillLineAbility.db2");
    LOAD_DB2(sSkillRaceClassInfoStore, "SkillRaceClassInfo.db2");
    LOAD_DB2(sSkillLineStore, "SkillLine.db2");
    LOAD_DB2(sSoundKitStore, "SoundKit.db2");
    LOAD_DB2(sSummonPropertiesStore, "SummonProperties.db2");
    LOAD_DB2(sTactKeyStore, "TactKey.db2");
    LOAD_DB2(sTalentStore, "Talent.db2");
    LOAD_DB2(sTotemCategoryStore, "TotemCategory.db2");
    LOAD_DB2(sTransportAnimationStore, "TransportAnimation.db2");
    LOAD_DB2(sTransportRotationStore, "TransportRotation.db2");
    LOAD_DB2(sToyStore, "Toy.db2");
    LOAD_DB2(sVehicleStore, "Vehicle.db2");
    LOAD_DB2(sVehicleSeatStore, "VehicleSeat.db2");
    LOAD_DB2(sVignetteStore, "Vignette.db2");
    LOAD_DB2(sWorldSafeLocsStore, "WorldSafeLocs.db2");
    LOAD_DB2(sAreaPOIStore, "AreaPOI.db2");
    LOAD_DB2(sContributionStore, "Contribution.db2");
    LOAD_DB2(sManagedWorldStateStore, "ManagedWorldState.db2");
    LOAD_DB2(sManagedWorldStateBuffStore, "ManagedWorldStateBuff.db2");
    LOAD_DB2(sManagedWorldStateInputStore, "ManagedWorldStateInput.db2");
    LOAD_DB2(sWorldStateExpressionStore, "WorldStateExpression.db2");

    if (threadPool)
        threadPool->wait();

    if (threadPool)
    {
        threadPool->stop();
        delete threadPool;
    }

    uint32 _oldMSTime = getMSTime();

    for (uint32 l_I = 0; l_I < sSpellProcsPerMinuteModStore.GetNumRows(); ++l_I)
    {
        if (SpellProcsPerMinuteModEntry const* l_PPMEntry = sSpellProcsPerMinuteModStore.LookupEntry(l_I))
            sSpellProcsPerMinuteMods[l_PPMEntry->SpellProcsPerMinuteID].push_back(l_PPMEntry);
    }

    for (uint32 l_I = 1; l_I < sSpellEffectStore.GetNumRows(); ++l_I)
    {
        if (SpellEffectEntry const *spellEffect = sSpellEffectStore.LookupEntry(l_I))
            sSpellEffectMap[spellEffect->EffectSpellId].effects[spellEffect->EffectDifficulty][spellEffect->EffectIndex] = spellEffect;
    }

    for (uint32 l_I = 0; l_I < sSpellReagentsStore.GetNumRows(); ++l_I)
    {
        if (SpellReagentsEntry const* l_SpellReagent = sSpellReagentsStore.LookupEntry(l_I))
            sSpellReagentMap[l_SpellReagent->SpellID] = l_SpellReagent;
    }

    for (uint32 l_I = 0; l_I < sSpellAuraRestrictionsStore.GetNumRows(); ++l_I)
    {
        if (SpellAuraRestrictionsEntry const* l_SpellAuraRestriction = sSpellAuraRestrictionsStore.LookupEntry(l_I))
            sSpellAuraRestrictionMap[l_SpellAuraRestriction->SpellID] = l_SpellAuraRestriction;
    }

    for (uint32 l_I = 0; l_I < sSpellCastingRequirementsStore.GetNumRows(); ++l_I)
    {
        if (SpellCastingRequirementsEntry const* l_SpellCastingRequirements = sSpellCastingRequirementsStore.LookupEntry(l_I))
            sSpellCastingRequirementsMap[l_SpellCastingRequirements->SpellID] = l_SpellCastingRequirements;
    }

    for (uint32 l_I = 0; l_I < sSpellClassOptionsStore.GetNumRows(); ++l_I)
    {
        if (SpellClassOptionsEntry const* l_SpellClassOptionsEntry = sSpellClassOptionsStore.LookupEntry(l_I))
            sSpellClassOptionsMap[l_SpellClassOptionsEntry->SpellID] = l_SpellClassOptionsEntry;
    }

    for (uint32 l_I = 0; l_I < sSpellEquippedItemsStore.GetNumRows(); ++l_I)
    {
        if (SpellEquippedItemsEntry const* l_SpellEquippedItemsEntry = sSpellEquippedItemsStore.LookupEntry(l_I))
            sSpellEquippedItemsMap[l_SpellEquippedItemsEntry->SpellID] = l_SpellEquippedItemsEntry;
    }

    for (uint32 l_I = 0; l_I < sSpellReagentsCurrencyStore.GetNumRows(); ++l_I)
    {
        if (SpellReagentsCurrencyEntry const* l_SpellReagentsCurrency = sSpellReagentsCurrencyStore.LookupEntry(l_I))
            sSpellReagentsCurrencyMap[l_SpellReagentsCurrency->SpellID] = l_SpellReagentsCurrency;
    }

    for (uint32 l_I = 0; l_I < sSpellScalingStore.GetNumRows(); ++l_I)
    {
        if (SpellScalingEntry const* l_SpellScalingEntry = sSpellScalingStore.LookupEntry(l_I))
            sSpellScalingMap[l_SpellScalingEntry->SpellID] = l_SpellScalingEntry;
    }

    for (uint32 l_I = 0; l_I < sSpellShapeshiftStore.GetNumRows(); ++l_I)
    {
        if (SpellShapeshiftEntry const* l_SpellShapeshift = sSpellShapeshiftStore.LookupEntry(l_I))
            sSpellShapeshiftMap[l_SpellShapeshift->SpellId] = l_SpellShapeshift;
    }

    for (uint32 l_I = 0; l_I < sSpellTotemsStore.GetNumRows(); ++l_I)
    {
        if (SpellTotemsEntry const* l_SpellTotems = sSpellTotemsStore.LookupEntry(l_I))
            sSpellTotemsMap[l_SpellTotems->SpellID] = l_SpellTotems;
    }

    for (uint32 l_I = 0; l_I < sGarrTalentStore.GetNumRows(); ++l_I)
    {
        GarrTalentEntry const* l_GarrTalentEntry = sGarrTalentStore.LookupEntry(l_I);
        if (!l_GarrTalentEntry || !l_GarrTalentEntry->GarrAbilityID)
            continue;

        s_GarrTalentByGarrAbilityID[l_GarrTalentEntry->GarrAbilityID] = l_GarrTalentEntry;
    }

    for (uint32 l_I = 0; l_I < sGarrAbilityEffectStore.GetNumRows(); ++l_I)
    {
        GarrAbilityEffectEntry const* l_GarrAbilityEffectEntry = sGarrAbilityEffectStore.LookupEntry(l_I);
        if (!l_GarrAbilityEffectEntry)
            continue;

        s_GarrAbilityEffectByEffectType[l_GarrAbilityEffectEntry->EffectType].insert(l_GarrAbilityEffectEntry);
    }

    for (uint32 l_I = 0; l_I < sGarrTalentTreeStore.GetNumRows(); ++l_I)
    {
        const GarrTalentTreeEntry* l_TalentTreeEntry = sGarrTalentTreeStore.LookupEntry(l_I);

        if (!l_TalentTreeEntry)
            continue;

        s_GarrTalentTreeIDPerClass[l_TalentTreeEntry->GarrisonType][l_TalentTreeEntry->ClassID] = l_TalentTreeEntry->ID;
        s_GarrTalentTreeByTreeID[l_TalentTreeEntry->ID].resize(l_TalentTreeEntry->NumRank);
    }

    for (uint32 l_I = 0; l_I < sGarrTalentStore.GetNumRows(); ++l_I)
    {
        const GarrTalentEntry* l_TalentEntry = sGarrTalentStore.LookupEntry(l_I);

        if (!l_TalentEntry)
            continue;

        if (s_GarrTalentTreeByTreeID.find(l_TalentEntry->TalentTreeID) == s_GarrTalentTreeByTreeID.end())
            continue;

        auto& l_TreePart = s_GarrTalentTreeByTreeID[l_TalentEntry->TalentTreeID][l_TalentEntry->Rank];

        if (l_TreePart.size() < (l_TalentEntry->Column + 1))
            l_TreePart.resize(l_TalentEntry->Column + 1);

        l_TreePart[l_TalentEntry->Column] = l_TalentEntry;
    }

    for (auto l_Entry : sGarrMechanicSetXMechanicStore)
    {
        if (auto l_MechanicEntry = sGarrMechanicStore[l_Entry->GarrMechanicID])
            s_GarrMechanicListBySetID[l_Entry->GarrMechanicSetID].push_back(l_MechanicEntry);
    }

    for (auto l_Entry : sGarrEncounterSetXEncounterStore)
    {
        if (auto l_EncounterID = l_Entry->GarrEncounterID)
            s_GarrEncounterListBySetID[l_Entry->GarrEncounterSetID].push_back(l_EncounterID);
    }

    std::map<uint32 /*entry*/, uint32 /*spellId*/> l_BattlePetsToCorrect =
    {
        { 83562, 167336 }, ///< Zomstrok
        { 73534, 148047 }, ///< Azure Crane Chick
        { 73357, 148062 }, ///< Ominous Flame
        { 73356, 148050 }, ///< Ruby Droplet
        { 85710, 171118 }, ///< Lovebird Hatchling
        { 68666, 135261 }  ///< Ashstone Core
    };

    for (uint32 l_I = 0; l_I < sBattlePetSpeciesStore.GetNumRows(); ++l_I)
    {
        BattlePetSpeciesEntry const* l_SpeciesEntry = sBattlePetSpeciesStore.LookupEntry(l_I);
        if (l_SpeciesEntry == nullptr)
            continue;

        auto l_Itr = l_BattlePetsToCorrect.find(l_SpeciesEntry->CreatureID);
        if (l_Itr != l_BattlePetsToCorrect.end())
            const_cast<BattlePetSpeciesEntry*>(l_SpeciesEntry)->SummonSpellID = l_Itr->second;
    }

    for (uint32 l_I = 0; l_I < sBattlePetSpeciesXAbilityStore.GetNumRows(); ++l_I)
    {
        BattlePetSpeciesXAbilityEntry const* l_SpeciesXAbilityInfo = sBattlePetSpeciesXAbilityStore.LookupEntry(l_I);
        if (l_SpeciesXAbilityInfo == nullptr)
            continue;

        sBattlePetAbilitiesBySpecies[l_SpeciesXAbilityInfo->speciesId].insert(l_SpeciesXAbilityInfo);
    }

    /// Ko'ragh Achievement - Pair Annihilation
    if (CriteriaEntry const* l_Criteria = sCriteriaStore.LookupEntry(24693))
        ((CriteriaEntry*)l_Criteria)->Type = CRITERIA_DATA_TYPE_SCRIPT;

    /// Insane in the Membrane
    /// All those criterias use 'CompleteAchievement' types with nonexistent achievements.
    /// Change it to 'GainReputation' types.
    {
        /// Change amounts for CriteriaTrees
        std::map<uint32, uint32> l_CriteriaTreesToModify =
        {
            { 10688,  42000 },   ///< Exalted with Overlook
            { 10689,  42000 },   ///< Exalted with Booty Bay
            { 10692,  42000 },   ///< Exalted with Gadgetzan
            { 10694,  42000 },   ///< Exalted with Ratchet
            { 10693,  21000 }    ///< Honored with Bloodsail Buccaneers
        };

        for (auto l_CriteriaTreeMod : l_CriteriaTreesToModify)
            if (CriteriaTreeEntry const* l_CriteriaTree = sCriteriaTreeStore.LookupEntry(l_CriteriaTreeMod.first))
                ((CriteriaTreeEntry*)l_CriteriaTree)->Amount = l_CriteriaTreeMod.second;

        /// Change types of Criterias
        std::map<uint32, std::pair<uint32, uint32> > l_CriteriasToModify =
        {
            { 24536, { 577, CriteriaTypes::CRITERIA_TYPE_GAIN_REPUTATION } }, ///< Exalted with Overlook
            { 24535, { 21, CriteriaTypes::CRITERIA_TYPE_GAIN_REPUTATION } }, ///< Exalted with Booty Bay
            { 24537, { 369, CriteriaTypes::CRITERIA_TYPE_GAIN_REPUTATION } }, ///< Exalted with Gadgetzan
            { 24538, { 470, CriteriaTypes::CRITERIA_TYPE_GAIN_REPUTATION } }, ///< Exalted with Ratchet
            { 24534, { 87, CriteriaTypes::CRITERIA_TYPE_GAIN_REPUTATION } }  ///< Honored with Bloodsail Buccaneers
        };

        for (auto l_CriteriaMod : l_CriteriasToModify)
        {
            if (CriteriaEntry const* l_Criteria = sCriteriaStore.LookupEntry(l_CriteriaMod.first))
            {
                ((CriteriaEntry*)l_Criteria)->Asset.FactionID = l_CriteriaMod.second.first;
                ((CriteriaEntry*)l_Criteria)->Type = l_CriteriaMod.second.second;
            }
        }
    }

    {
        /// Init sTaxiPathSetBySource
        for (uint32 l_I = 0; l_I < sTaxiPathStore.GetNumRows(); l_I++)
        {
            TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(l_I);

            if (!entry)
                continue;

            sTaxiPathSetBySource[entry->From][entry->To] = TaxiPathBySourceAndDestination(entry->ID, entry->Cost);
        }

        /// Calculate path nodes count
        std::vector<uint32> l_PathLength;
        l_PathLength.resize(sTaxiPathStore.GetNumRows());

        for (uint32 l_I = 0; l_I < sTaxiPathNodeStore.GetNumRows(); l_I++)
        {
            TaxiPathNodeEntry const* l_PathNodeEntry = sTaxiPathNodeStore.LookupEntry(l_I);

            if (!l_PathNodeEntry)
                continue;

            if (l_PathLength[l_PathNodeEntry->PathID] < l_PathNodeEntry->NodeIndex + 1)
                l_PathLength[l_PathNodeEntry->PathID] = l_PathNodeEntry->NodeIndex + 1;
        }

        /// Set path length
        sTaxiPathNodesByPath.resize(sTaxiPathStore.GetNumRows());
        for (uint32 l_I = 0; l_I < sTaxiPathNodesByPath.size(); ++l_I)
            sTaxiPathNodesByPath[l_I].resize(l_PathLength[l_I]);

        /// fill data
        for (uint32 l_I = 0; l_I < sTaxiPathNodeStore.GetNumRows(); l_I++)
        {
            TaxiPathNodeEntry const* l_PathNodeEntry = sTaxiPathNodeStore.LookupEntry(l_I);

            if (!l_PathNodeEntry)
                continue;

            sTaxiPathNodesByPath[l_PathNodeEntry->PathID][l_PathNodeEntry->NodeIndex] = l_PathNodeEntry;
        }

        /// Initialize global taxi nodes mask
        /// include existed nodes that have at least single not spell base (scripted) path
        {
            if (sTaxiNodesStore.GetNumRows())
            {
                WPError(TaxiMaskSize >= ((sTaxiNodesStore.GetNumRows() - 1) / 8) + 1,
                    "TaxiMaskSize is not large enough to contain all taxi nodes!");
            }

            sTaxiNodesMask.fill(0);
            sOldContinentsNodesMask.fill(0);
            sHordeTaxiNodesMask.fill(0);
            sAllianceTaxiNodesMask.fill(0);

            for (uint32 l_I = 0; l_I < sTaxiNodesStore.GetNumRows(); l_I++)
            {
                TaxiNodesEntry const* l_NodeEntry = sTaxiNodesStore.LookupEntry(l_I);

                if (!l_NodeEntry)
                    continue;

                if (!(l_NodeEntry->Flags & (TAXI_NODE_FLAG_ALLIANCE | TAXI_NODE_FLAG_HORDE)))
                    continue;

                /// Valid taxi network node
                uint8  l_Field      = (uint8)((l_NodeEntry->ID - 1) / 8);
                uint32 l_SubMask    =   1 << ((l_NodeEntry->ID - 1) % 8);

                sTaxiNodesMask[l_Field] |= l_SubMask;

                if (l_NodeEntry->Flags & TAXI_NODE_FLAG_HORDE)
                    sHordeTaxiNodesMask[l_Field] |= l_SubMask;
                if (l_NodeEntry->Flags & TAXI_NODE_FLAG_ALLIANCE)
                    sAllianceTaxiNodesMask[l_Field] |= l_SubMask;

                if (l_NodeEntry->Flags & TaxiNodeFlags::TAXI_NODE_FLAG_ARGUS)
                    sArgusTaxiMask[l_Field] |= l_SubMask;

                uint32 l_NodeMap;
                DeterminateAlternateMapPosition(l_NodeEntry->MapID, l_NodeEntry->x, l_NodeEntry->y, l_NodeEntry->z, &l_NodeMap);

                if (l_NodeMap < 2)
                    sOldContinentsNodesMask[l_Field] |= l_SubMask;
            }
        }
    }

    for (uint32 l_ID = 0; l_ID < sArtifactPowerRankStore.GetNumRows(); ++l_ID)
    {
        if (ArtifactPowerRankEntry const* l_ArtifactRank = sArtifactPowerRankStore.LookupEntry(l_ID))
        {
            _artifactPowerRanks[std::pair<uint32, uint8>{ l_ArtifactRank->ArtifactPowerID, l_ArtifactRank->Rank }] = l_ArtifactRank;

            _artifactPowerRanksBySpellId.insert(std::make_pair(l_ArtifactRank->SpellID, l_ArtifactRank));
        }
    }

    for (PvpScalingEffectEntry const* l_PvpScalingEffect : sPvpScalingEffectStore)
        g_PvpScalingPerSpecs[l_PvpScalingEffect->SpecializationId][l_PvpScalingEffect->PvpScalingEffectTypeId] = l_PvpScalingEffect->Value;

    std::thread itemsparseTh([]() -> void
    {
    for (uint32 l_ID = 0; l_ID < sItemSparseStore.GetNumRows(); ++l_ID)
    {
        /// Why do those still exist ????
        if (l_ID == 139275 || l_ID == 139891 || l_ID == 129738 || l_ID == 134562
            || l_ID == 137581 || l_ID == 127260 || l_ID == 137582                   ///< Pala dps
            || l_ID == 137660                                                       ///< Pala heal
            || l_ID == 137661                                                       ///< Pala tank
            )
            continue;

        ItemSparseEntry const* l_Entry = sItemSparseStore.LookupEntry(l_ID);

        if (!l_Entry)
            continue;

        /// WoD PvP Items don't have anymore reputation requirements
        if ((l_Entry->RequiredReputationFaction == 1681 || l_Entry->RequiredReputationFaction == 1682) && l_Entry->ItemLevel >= 600)
        {
            const_cast<ItemSparseEntry*>(l_Entry)->RequiredReputationFaction = 0;
            const_cast<ItemSparseEntry*>(l_Entry)->RequiredReputationRank = 0;
        }

        if (uint32 l_ArtifactID = l_Entry->ArtifactID)
        {
            g_ItemIDByArtifactID[l_ArtifactID] = l_Entry->ID;

            ArtifactEntry const* l_ArtifactEntry = sArtifactStore.LookupEntry(l_ArtifactID);

            if (uint32 l_Spec = l_ArtifactEntry->SpecializationID)
            {
                g_ItemIDBySpec[l_Spec] = l_Entry->ID;
            }
        }
    }
    });

    for (uint32 l_ID = 0; l_ID < sItemClassStore.GetNumRows(); ++l_ID)
    {
        ItemClassEntry const* l_ItemClass = sItemClassStore.LookupEntry(l_ID);
        if (!l_ItemClass)
            continue;

        ASSERT(l_ItemClass->ClassId < g_itemClassByOldEnum.size());
        ASSERT(!g_itemClassByOldEnum[l_ItemClass->ClassId]);
        g_itemClassByOldEnum[l_ItemClass->ClassId] = l_ItemClass;
    }

    for (uint32 l_ID = 0; l_ID < sItemChildEquipmentStore.GetNumRows(); ++l_ID)
    {
        ItemChildEquipmentEntry const* l_ItemChildEquipment = sItemChildEquipmentStore.LookupEntry(l_ID);

        if (!l_ItemChildEquipment)
            continue;

        /// Don't load invalid child items
        ItemSparseEntry const* l_SparseItem = sItemSparseStore.LookupEntry(l_ItemChildEquipment->ItemID);
        if (!l_SparseItem)
            continue;

        g_ItemChildIds.insert(l_ItemChildEquipment->ChildItemID);

        ASSERT(g_itemChildEquipment.find(l_ItemChildEquipment->ItemID) == g_itemChildEquipment.end() && "Item must have max 1 child item.");
        g_itemChildEquipment[l_ItemChildEquipment->ItemID] = l_ItemChildEquipment;
    }

    for (uint32 l_I = 0; l_I < sItemBonusListLevelDeltaStore.GetNumRows(); l_I++)
    {
        auto l_ItemBonusListLevelDelta = sItemBonusListLevelDeltaStore.LookupEntry(l_I);
        if (l_ItemBonusListLevelDelta == nullptr)
            continue;

        g_ItemLevelDeltaToBonusListContainer[l_ItemBonusListLevelDelta->ItemLevelDelta] = l_ItemBonusListLevelDelta->ID;
    }

    for (uint32 l_I = 0; l_I < sPvpItemStore.GetNumRows(); ++l_I)
    {
        if (PvpItemEntry const* l_Entry = sPvpItemStore.LookupEntry(l_I))
            g_PvPItemStoreLevels[l_Entry->itemId] = l_Entry->ilvl;
    }

    std::thread itemModAppearanceTh([]() -> void
        {
            for (uint32 l_I = 0; l_I < sItemModifiedAppearanceStore.GetNumRows(); l_I++)
            {
                ItemModifiedAppearanceEntry const* l_AppearanceMod = sItemModifiedAppearanceStore.LookupEntry(l_I);
                if (l_AppearanceMod == nullptr)
                    continue;

                ASSERT(l_AppearanceMod->ItemID <= 0xFFFFFF);

                _itemModifiedAppearancesByItem[l_AppearanceMod->ItemID | (l_AppearanceMod->AppearanceModID << 24)] = l_AppearanceMod;

                auto l_DefaultAppearance = _itemDefaultAppearancesByItem.find(l_AppearanceMod->ItemID);
                if (l_DefaultAppearance == _itemDefaultAppearancesByItem.end() || l_DefaultAppearance->second->Index > l_AppearanceMod->Index)
                    _itemDefaultAppearancesByItem[l_AppearanceMod->ItemID] = l_AppearanceMod;
            }
        });
    for (uint32 l_I = 0; l_I < sTransmogSetItemStore.GetNumRows(); ++l_I)
    {
        TransmogSetItemEntry const* l_TransmogSetItemEntry = sTransmogSetItemStore.LookupEntry(l_I);
        if (!l_TransmogSetItemEntry)
            continue;

        g_TransmogSetItemByTransmogSet.insert({ l_TransmogSetItemEntry->TransmogSetGroupID, l_TransmogSetItemEntry });
    }

    for (uint32 l_I = 1; l_I < sItemEffectStore.GetNumRows(); ++l_I)
    {
        if (ItemEffectEntry const* l_Entry = sItemEffectStore.LookupEntry(l_I))
            sItemEffectsByItemID[l_Entry->ItemID].push_back(l_I);
    }

    for (uint32 l_I = 0; l_I < sItemSpecOverrideStore.GetNumRows(); l_I++)
    {
        auto itemSpecOverride = sItemSpecOverrideStore.LookupEntry(l_I);
        if (itemSpecOverride == nullptr)
            continue;

        _itemSpecOverrides[itemSpecOverride->itemEntry].push_back(itemSpecOverride);
    }

    /// Load Item Bonus Tree
    for (uint32 l_I = 0; l_I < sItemXBonusTreeStore.GetNumRows(); l_I++)
    {
        auto l_ItemXBonusTree = sItemXBonusTreeStore.LookupEntry(l_I);
        if (l_ItemXBonusTree == nullptr)
            continue;

        sItemBonusTreeByID[l_ItemXBonusTree->ItemId].push_back(l_ItemXBonusTree);
    }

    for (uint32 l_I = 0; l_I < sItemSetSpellStore.GetNumRows(); l_I++)
    {
        ItemSetSpellEntry const* l_SetSpellEntry = sItemSetSpellStore.LookupEntry(l_I);

        if (!l_SetSpellEntry)
            continue;

        sItemSetSpellsByItemIDStore[l_SetSpellEntry->ItemSetID].push_back(l_SetSpellEntry);
    }

    for (uint32 l_I = 0; l_I < sItemBonusStore.GetNumRows(); l_I++)
    {
        ItemBonusEntry const* l_Entry = sItemBonusStore.LookupEntry(l_I);
        if (!l_Entry)
            continue;

        if (!sItemBonusesByID[l_Entry->ParentItemBonusListId].size())
        {
            sItemBonusesByID[l_Entry->ParentItemBonusListId].resize(MAX_ITEM_BONUS);
            for (int l_J = 0; l_J < MAX_ITEM_BONUS; l_J++)
                sItemBonusesByID[l_Entry->ParentItemBonusListId][l_J] = nullptr;
        }

        sItemBonusesByID[l_Entry->ParentItemBonusListId][l_Entry->Index] = l_Entry;
    }

    for (uint32 l_I = 0; l_I < sResearchProjectStore.GetNumRows(); ++l_I)
    {
        ResearchProjectEntry const* rp = sResearchProjectStore.LookupEntry(l_I);
        if (!rp)
            continue;

        sResearchProjectPerSpell[rp->SpellID] = rp;

        // Branch 0 is for Blizzard tests
        if (rp->ResearchBranchID == 0)
            continue;

        sResearchProjectSet.insert(rp);
    }
    //sResearchProjectStore.Clear();

    
    //sResearchSiteStore.Clear();

    /// Load quest package items
    for (uint32 l_I = 0; l_I < sQuestPackageItemStore.GetNumRows(); l_I++)
    {
        QuestPackageItemEntry const* l_QuestPackageItem = sQuestPackageItemStore.LookupEntry(l_I);
        if (l_QuestPackageItem == nullptr)
            continue;

        sQuestPackageItemsByGroup[l_QuestPackageItem->QuestPackageID].push_back(l_QuestPackageItem);
    }

    for (uint32 i = 0; i < sMapDifficultyStore.GetNumRows(); ++i)
    {
        MapDifficultyEntry const* l_Entry = sMapDifficultyStore.LookupEntry(i);

        if (!l_Entry)
            continue;

        switch (l_Entry->ID)
        {
        case 2952: ///< Highmaul - Heroic
            const_cast<MapDifficultyEntry*>(l_Entry)->Context = 5;
            break;
        case 2953: ///< Highmaul - Mythic
            const_cast<MapDifficultyEntry*>(l_Entry)->Context = 6;
            break;
        case 3731: ///< Return to Karazhan - Heroic
        case 3746: ///< The Arcway - Heroic
            const_cast<MapDifficultyEntry*>(l_Entry)->RaidDurationType = 1;
            break;
        default:
            break;
        }

        _mapDifficulties[l_Entry->MapID][l_Entry->DifficultyID] = l_Entry;
    }

        ///< Make shipyards instances
    if (MapEntry* l_MapEntry = const_cast<MapEntry*>(sMapStore.LookupEntry(1473)))
        l_MapEntry->instanceType = MAP_INSTANCE;

        if (MapEntry* l_MapEntry = const_cast<MapEntry*>(sMapStore.LookupEntry(1474)))
            l_MapEntry->instanceType = MAP_INSTANCE;

        for (uint32 l_I = 0; l_I < sWMOAreaTableStore.GetNumRows(); ++l_I)
        {
             if (WMOAreaTableEntry const* l_WMOAreaTableEntry = sWMOAreaTableStore.LookupEntry(l_I))
                 sWMOAreaInfoByTripple.insert(WMOAreaInfoByTripple::value_type(WMOAreaTableTripple(l_WMOAreaTableEntry->WMOID, l_WMOAreaTableEntry->NameSet, l_WMOAreaTableEntry->WMOGroupID), l_WMOAreaTableEntry));
        }

        for (uint32 l_ID = 0; l_ID < sAreaGroupMemberStore.GetNumRows(); l_ID++)
        {
            if (AreaGroupMemberEntry const* l_Entry = sAreaGroupMemberStore.LookupEntry(l_ID))
            {
                sAreaGroupMembersByIDStore[l_Entry->AreaGroupID].push_back(l_Entry->AreaID);
                s_AreaGroupsByAreaID[l_Entry->AreaID].emplace(l_Entry->AreaGroupID);
            }
        }

        if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
        {
            std::array<uint32, 4> l_SanctuaryZones = {{1637, 1519, 1741, 8336}};
            for (uint32 l_Zone : l_SanctuaryZones)
            {
                AreaTableEntry const* l_AreaTable = sAreaTableStore.LookupEntry(l_Zone);
                if (!l_AreaTable)
                    continue;

                const_cast<AreaTableEntry*>(l_AreaTable)->Flags |= AreaFlags::AREA_FLAG_SANCTUARY;

                if (l_Zone == 8336)
                    const_cast<AreaTableEntry*>(l_AreaTable)->Flags |= AreaFlags::AREA_FLAG_ALLOW_DUELS;
            }
        }

        // need to load after Maps & QuestPOIPoint
        for (auto rs : sResearchSiteStore)
        {
            if (rs->ID == 140           // template
                || rs->ID == 142        // template
                || rs->ID == 161        // template
                || rs->ID == 471        // vashj'ir
                || rs->ID == 473        // vashj'ir
                || rs->ID == 475)       // vashj'ir
                continue;
            sResearchSiteSet.insert(rs);

            auto& data = sResearchSiteDataMap[rs->ID];
            data.map = rs->mapId;
            for (QuestPOIPointEntry const* poi : sQuestPOIPointStore)
                if (poi->QuestPOIBlobId == rs->POIid)
                    data.coords.push_back(ResearchPOIPoint(poi->x, poi->y));

            data.level = 0;
        }

        sPowersByClassStore.resize(MAX_CLASSES);

        for (uint32 l_I = 0; l_I <= sChrClassXPowerTypesStore.GetNumRows(); ++l_I)
        {
            ChrClassXPowerTypesEntry const* l_PowerEntry = sChrClassXPowerTypesStore.LookupEntry(l_I);
            if (!l_PowerEntry)
                continue;

            sPowersByClassStore[l_PowerEntry->classId].push_back(l_PowerEntry->power);
        }

        for (int l_Class = 0; l_Class < MAX_CLASSES; ++l_Class)
            std::sort(sPowersByClassStore[l_Class].begin(), sPowersByClassStore[l_Class].end());

        memset(_chrSpecializationsByIndex, 0, sizeof(_chrSpecializationsByIndex));
        for (uint32 l_I = 0; l_I <= sChrSpecializationsStore.GetNumRows(); ++l_I)
        {
            ChrSpecializationsEntry const* chrSpec = sChrSpecializationsStore.LookupEntry(l_I);
            if (!chrSpec)
                continue;

            ASSERT(chrSpec->ClassID < MAX_CLASSES);
            ASSERT(chrSpec->OrderIndex < MAX_SPECIALIZATIONS);

            uint32 storageIndex = chrSpec->ClassID;
            if (chrSpec->Flags & CHR_SPECIALIZATION_FLAG_PET_OVERRIDE_SPEC)
            {
                ASSERT(!chrSpec->ClassID);
                storageIndex = PET_SPEC_OVERRIDE_CLASS_INDEX;
            }

            _chrSpecializationsByClass[storageIndex].push_back(chrSpec);

            _chrSpecializationsByIndex[storageIndex][chrSpec->OrderIndex] = chrSpec;
            if (chrSpec->Flags & CHR_SPECIALIZATION_FLAG_RECOMMENDED)
                _defaultChrSpecializationsByClass[chrSpec->ClassID] = chrSpec;
        }

        std::unordered_map<uint32, std::set<std::pair<uint8, uint8>>> l_AddedSections;
        for (uint32 l_I = 0; l_I < sCharSectionsStore.GetNumRows(); ++l_I)
        {
            if (CharSectionsEntry const* l_CharSection = sCharSectionsStore.LookupEntry(l_I))
            {
                if (!l_CharSection->Race || !((1 << (l_CharSection->Race - 1)) & RACEMASK_ALL_PLAYABLE)) ///< Ignore Nonplayable races
                    continue;

                /// Not all sections are used for low-res models but we need to get all sections for validation since its viewer dependent
                uint8 l_BaseSection = l_CharSection->GenType;
                switch (l_BaseSection)
                {
                    case SECTION_TYPE_SKIN_LOW_RES:
                    case SECTION_TYPE_FACE_LOW_RES:
                    case SECTION_TYPE_FACIAL_HAIR_LOW_RES:
                    case SECTION_TYPE_HAIR_LOW_RES:
                    case SECTION_TYPE_UNDERWEAR_LOW_RES:
                        l_BaseSection = l_BaseSection + SECTION_TYPE_SKIN;
                        break;
                    case SECTION_TYPE_SKIN:
                    case SECTION_TYPE_FACE:
                    case SECTION_TYPE_FACIAL_HAIR:
                    case SECTION_TYPE_HAIR:
                    case SECTION_TYPE_UNDERWEAR:
                        break;
                    case SECTION_TYPE_CUSTOM_DISPLAY_1_LOW_RES:
                    case SECTION_TYPE_CUSTOM_DISPLAY_2_LOW_RES:
                    case SECTION_TYPE_CUSTOM_DISPLAY_3_LOW_RES:
                        ++l_BaseSection;
                        break;
                    case SECTION_TYPE_CUSTOM_DISPLAY_1:
                    case SECTION_TYPE_CUSTOM_DISPLAY_2:
                    case SECTION_TYPE_CUSTOM_DISPLAY_3:
                        break;
                    default:
                        break;
                }

                uint32 l_SectionKey = l_BaseSection | (l_CharSection->Gender << 8) | (l_CharSection->Race << 16);
                std::pair<uint8, uint8> l_SectionCombination { l_CharSection->Type, l_CharSection->Color };
                if (l_AddedSections[l_SectionKey].count(l_SectionCombination))
                    continue;

                l_AddedSections[l_SectionKey].insert(l_SectionCombination);
                g_CharSections.insert({ l_SectionKey, l_CharSection });
            }
        }

        HotfixLfgDungeonsData();

        for (ToyEntry const* l_Toy : sToyStore)
            g_ToyItems[l_Toy->ItemId] = l_Toy;

        /// Demonic Essence - The Nighthold
        if (VehicleSeatEntry const* l_Seat = sVehicleSeatStore.LookupEntry(16913))
            const_cast<VehicleSeatEntry*>(l_Seat)->AttachmentOffsetZ = 4.0f;

        for (uint32 l_I = 0; l_I < sMountTypeXCapabilityStore.GetNumRows(); ++l_I)
            if (MountTypeXCapabilityEntry const* mountTypeCapability = sMountTypeXCapabilityStore.LookupEntry(l_I))
                _mountCapabilitiesByType[mountTypeCapability->MountTypeID].insert(mountTypeCapability);

        for (uint32 l_I = 0; l_I < sMountStore.GetNumRows(); ++l_I)
            if (MountEntry const* l_MountEntry = sMountStore.LookupEntry(l_I))
                g_MountBySpell[l_MountEntry->SpellID] = l_MountEntry;

        for (auto l_VisualEntry : sMountXDisplayStore)
        {
            if (auto l_MountEntry = sMountStore[l_VisualEntry->MountID])
                g_MountVisualsByID[l_MountEntry->Id].push_back(l_VisualEntry);
        }

        std::set<uint32> scalingCurves;
        for (uint32 l_I = 0; l_I < sScalingStatDistributionStore.GetNumRows(); ++l_I)
            if (ScalingStatDistributionEntry const* ssd = sScalingStatDistributionStore.LookupEntry(l_I))
                scalingCurves.insert(ssd->CurveProperties);

        for (auto itr = g_CurvePoints.begin(); itr != g_CurvePoints.end(); ++itr)
            std::sort(itr->second.begin(), itr->second.end(), [](CurvePointEntry const* point1, CurvePointEntry const* point2) { return point1->Index < point2->Index; });

        uint32 l_MaxPvpTier = 0;
        uint32 l_MaxPvpCollumn = 0;

        for (uint32 l_ID = 0; l_ID < sPvpTalentUnlockStore.GetNumRows(); ++l_ID)
        {
            PvpTalentUnlockEntry const* l_Entry = sPvpTalentUnlockStore.LookupEntry(l_ID);
            if (!l_Entry)
                continue;

            if (l_Entry->m_CollumnIndex > l_MaxPvpCollumn)
                l_MaxPvpCollumn = l_Entry->m_CollumnIndex;

            if (l_Entry->m_TierIndex > l_MaxPvpTier)
                l_MaxPvpTier = l_Entry->m_TierIndex;
        }

        ++l_MaxPvpCollumn;
        ++l_MaxPvpTier;

        g_PvpTalentLevelRequirement.resize(l_MaxPvpCollumn);

        for (uint32 l_I = 0; l_I < l_MaxPvpCollumn; ++l_I)
        {
            g_PvpTalentLevelRequirement[l_I].resize(l_MaxPvpTier);
        }

        for (uint32 l_ID = 0; l_ID < sPvpTalentUnlockStore.GetNumRows(); ++l_ID)
        {
            PvpTalentUnlockEntry const* l_Entry = sPvpTalentUnlockStore.LookupEntry(l_ID);
            if (!l_Entry)
                continue;

            g_PvpTalentLevelRequirement[l_Entry->m_CollumnIndex][l_Entry->m_TierIndex] = l_Entry->m_LevelRequirement;
        }

        std::set<ResearchSiteEntry const*> sResearchSiteSet;
        std::set<ResearchProjectEntry const*> sResearchProjectSet;

        for (uint32 l_I = 0; l_I < sMountTypeXCapabilityStore.GetNumRows(); ++l_I)
            if (MountTypeXCapabilityEntry const* mountTypeCapability = sMountTypeXCapabilityStore.LookupEntry(l_I))
                _mountCapabilitiesByType[mountTypeCapability->MountTypeID].insert(mountTypeCapability);

        for (uint32 l_I = 0; l_I < sCurvePointStore.GetNumRows(); ++l_I)
        {
            if (CurvePointEntry const* curvePoint = sCurvePointStore.LookupEntry(l_I))
            {
                if (sCurveStore.LookupEntry(curvePoint->CurveID))
                    g_CurvePoints[curvePoint->CurveID].push_back(curvePoint);

                if (scalingCurves.count(curvePoint->CurveID))
                    HeirloomCurvePoints[curvePoint->CurveID][curvePoint->Index] = curvePoint;
            }
        }

        for (uint32 l_ID = 0; l_ID < sHeirloomStore.GetNumRows(); ++l_ID)
        {
            HeirloomEntry const* l_Heirloom = sHeirloomStore.LookupEntry(l_ID);

            if (!l_Heirloom)
                continue;

            HeirloomEntryByItemID.insert({ l_Heirloom->ItemID, l_Heirloom });

            for (uint32 l_X = 0; l_X < 2; l_X++)
                if (uint32 l_OlderItemID = l_Heirloom->OldHeirloomID[l_X])
                    HeirloomEntryByItemID.insert({ l_OlderItemID, l_Heirloom });

            if (uint32 l_HeroicID = l_Heirloom->HeroicVersion)
                HeirloomEntryByItemID.insert({ l_HeroicID, l_Heirloom });
        }

        for (uint32 l_ID = 0; l_ID < sSpecializationSpellStore.GetNumRows(); ++l_ID)
        {
            SpecializationSpellEntry const* specSpells = sSpecializationSpellStore.LookupEntry(l_ID);

            if (!specSpells)
                continue;

            _specializationSpellsBySpec[specSpells->SpecializationEntry].push_back(specSpells);
        }

        for (uint32 l_ID = 0; l_ID <  sMapChallengeModeStore.GetNumRows(); l_ID++)
        {
            MapChallengeModeEntry const* entry = sMapChallengeModeStore.LookupEntry(l_ID);
            if (!entry)
                continue;

            _mapChallengeModeEntrybyMap.insert(MapChallengeModeEntryContainer::value_type(entry->MapID, entry));
            switch (entry->MapID)
            {
                case 1501: ///< Black Rook Hold
                case 1516: ///< The Arcway
                case 1571: ///< Court of Stars
                case 1493: ///< Vault of the Wardens
                case 1544: ///< Violet hold
                case 1492: ///< Maw of souls
                case 1466: ///< Darkheart Thicket
                case 1458: ///< Neltharions Lair
                case 1456: ///< Eye of Azhara
                case 1477: ///< Halls of valor
                case 1651: ///< Return to Karazhan
                case 1677: ///< Cathedral of Eternal Night
                case 1753: ///< Seat of Triumvirate
                    _challengeModeMaps.emplace_back(entry->MapID);
                    g_LegionMapChallengeModeEntrybyMap.insert(MapChallengeModeEntryContainer::value_type(entry->MapID, entry));
                    break;
                default:
                    break;
            }
        }


        for (uint32 l_ID = 0; l_ID < sTalentStore.GetNumRows(); ++l_ID)
        {
            TalentEntry const* talentInfo = sTalentStore.LookupEntry(l_ID);

            if (!talentInfo)
                continue;

            ASSERT(talentInfo->ClassID < MAX_CLASSES);
            ASSERT(talentInfo->TierID < MAX_TALENT_TIERS);          ///< "MAX_TALENT_TIERS must be at least %u", talentInfo->TierID);
            ASSERT(talentInfo->ColumnIndex < MAX_TALENT_COLUMNS);   ///< "MAX_TALENT_COLUMNS must be at least %u", talentInfo->ColumnIndex);

            if (talentInfo->SpecID != 0)
                _talentsByPosition[talentInfo->ClassID][sChrSpecializationsStore.LookupEntry(talentInfo->SpecID)->OrderIndex][talentInfo->TierID][talentInfo->ColumnIndex].push_back(talentInfo);
            else
            {
                for (int l_Y = 0; l_Y < MAX_SPECIALIZATIONS; ++l_Y)
                    _talentsByPosition[talentInfo->ClassID][l_Y][talentInfo->TierID][talentInfo->ColumnIndex].push_back(talentInfo);
            }
        }

        for (uint32 l_I = 0; l_I < sNameGenStore.GetNumRows(); ++l_I)
            if (NameGenEntry const* entry = sNameGenStore.LookupEntry(l_I))
                sGenNameVectoArraysMap[entry->race].stringVectorArray[entry->gender].push_back(std::string(entry->name->Get(sWorld->GetDefaultDb2Locale())));

        sNameGenStore.Clear();

        for (uint32 l_I = 0; l_I < sSkillLineAbilityStore.GetNumRows(); ++l_I)
        {
            SkillLineAbilityEntry const *skillLine = sSkillLineAbilityStore.LookupEntry(l_I);

            if (!skillLine)
                continue;

            g_SkillLineAbilityPerSkill[skillLine->SkillLine].push_back(skillLine);

            if (skillLine->SupercedesSpell)
                g_SkillLineAbilityReqSpells[skillLine->spellId] = skillLine->SupercedesSpell;

            SpellMiscEntry const* spellMisc = sSpellMiscStore.LookupByRelationshipID(skillLine->spellId);
            if (spellMisc && spellMisc->Attributes & SPELL_ATTR0_PASSIVE)
            {
                for (uint32 i = 1; i < sCreatureFamilyStore.GetNumRows(); ++i)
                {
                    SpellLevelsEntry const* levels = sSpellLevelsStore.LookupEntry(i);
                    if (!levels)
                        continue;

                    CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(i);
                    if (!cFamily)
                        continue;

                    if (skillLine->SkillLine != cFamily->skillLine[0] && skillLine->SkillLine != cFamily->skillLine[1])
                        continue;

                    if (levels->spellLevel)
                        continue;

                    if ((skillLine->Flags & SKILL_LINE_ABILITY_LEARNED_ON_SKILL_LEARN) == 0)
                        continue;

                    sPetFamilySpellsStore[i].insert(skillLine->spellId);
                }
            }
        }

        for (SkillLineAbilityEntry const* skillLineAbility : sSkillLineAbilityStore)
            _skillLineAbilitiesBySkillupSkill[skillLineAbility->SkillupSkillLineId ? skillLineAbility->SkillupSkillLineId : skillLineAbility->SkillLine].push_back(skillLineAbility);

        for (uint32 l_I = 0; l_I < sSkillRaceClassInfoStore.GetNumRows(); ++l_I)
        {
            SkillRaceClassInfoEntry const* l_Entry = sSkillRaceClassInfoStore.LookupEntry(l_I);

            if (!l_Entry)
                continue;

            if (sSkillLineStore.LookupEntry(l_Entry->SkillID))
                _skillRaceClassInfoBySkill.insert(SkillRaceClassInfoContainer::value_type(l_Entry->SkillID, l_Entry));
        }

        /// Legion Season 5 : With Legion Season 5, the maximum number of times you can earn Prestige has increased from 18 to 25.
        g_MaxPrestigeLevel = 25;

        for (uint32 l_I = 0; l_I < sEmotesTextSoundStore.GetNumRows(); ++l_I)
        {
            if (EmotesTextSoundEntry const* l_Entry = sEmotesTextSoundStore.LookupEntry(l_I))
                sEmotesTextSoundMap[EmotesTextSoundKey(l_Entry->EmotesTextId, l_Entry->RaceId, l_Entry->Gender)] = l_Entry;
        }

        for (uint32 i = 0; i < sFactionStore.GetNumRows(); ++i)
        {
            FactionEntry const* faction = sFactionStore.LookupEntry(i);
            if (faction && faction->ParentFactionID)
            {
                SimpleFactionsList &flist = sFactionTeamMap[faction->ParentFactionID];
                flist.push_back(i);
            }
        }

        /// Remove PvP flags from Argus Invasion faction
        if (FactionTemplateEntry const* l_FactionTemplate = sFactionTemplateStore.LookupEntry(2780))
            const_cast<FactionTemplateEntry*>(l_FactionTemplate)->Flags &= ~FACTION_TEMPLATE_FLAG_PVP;

        for (uint32 i = 0; i < sGameObjectDisplayInfoStore.GetNumRows(); ++i)
        {
            if (GameObjectDisplayInfoEntry const* info = sGameObjectDisplayInfoStore.LookupEntry(i))
            {
                if (info->maxX < info->minX)
                    std::swap(*(float*)(&info->maxX), *(float*)(&info->minX));
                if (info->maxY < info->minY)
                    std::swap(*(float*)(&info->maxY), *(float*)(&info->minY));
                if (info->maxZ < info->minZ)
                    std::swap(*(float*)(&info->maxZ), *(float*)(&info->minZ));
            }
        }

        /// Since mop, we count 7 entries with slot = -1, we must set them at 0, if not, crash !
        for (uint32 i = 0; i < sSummonPropertiesStore.GetNumRows(); ++i)
        {
            if (SummonPropertiesEntry const* prop = sSummonPropertiesStore.LookupEntry(i))
            {
                if (prop->Slot >= MAX_SUMMON_SLOT || prop->Slot < 0)
                    ((SummonPropertiesEntry*)prop)->Slot = 0;
            }
        }

        for (uint32 i = 0; i < sTransportAnimationStore.GetNumRows(); ++i)
        {
            TransportAnimationEntry const* anim = sTransportAnimationStore.LookupEntry(i);
            if (!anim)
                continue;

            sTransportMgr->AddPathNodeToTransport(anim->TransportID, anim->TimeIndex, anim);
        }

        for (uint32 i = 0; i < sTransportRotationStore.GetNumRows(); ++i)
        {
            TransportRotationEntry const* rot = sTransportRotationStore.LookupEntry(i);
            if (!rot)
                continue;

            sTransportMgr->AddPathRotationToTransport(rot->TransportEntry, rot->TimeSeg, rot);
        }

        for (uint32 i = 0; i < sPvPDifficultyStore.GetNumRows(); ++i)
        {
            if (PvPDifficultyEntry const* entry = sPvPDifficultyStore.LookupEntry(i))
            {
                if (entry->bracketId > MS::Battlegrounds::Brackets::Count)
                    ASSERT(false && "Need update Brackets::Count by DB2 data");
            }
        }

        // @TODO: Move this hack to vehicle_seat_dbc table
        if (VehicleEntry * vehicle = (VehicleEntry*)sVehicleStore.LookupEntry(584))
        {
            vehicle->SeatID[0] = 20000;
            vehicle->SeatID[1] = 20001;
            vehicle->SeatID[2] = 20002;
            vehicle->SeatID[3] = 20003;
        }

        for (uint32 l_I = 0; l_I < sWorldSafeLocsStore.GetNumRows(); ++l_I)
        {
            WorldSafeLocsEntry* l_WorldSafeLocs = (WorldSafeLocsEntry*)sWorldSafeLocsStore.LookupEntry(l_I);
            if (l_WorldSafeLocs == nullptr)
                continue;

            // Facing is in degrees
            l_WorldSafeLocs->o *= M_PI / 180;
        }

        for (uint32 l_I = 0; l_I < sGlyphBindableSpellStore.GetNumRows(); ++l_I)
        {
            if (GlyphBindableSpellEntry const* l_GlyphBindableSpell = sGlyphBindableSpellStore.LookupEntry(l_I))
                g_GlyphBindableSpells[l_GlyphBindableSpell->GlyphPropertiesID].push_back(l_GlyphBindableSpell->SpellID);
        }

        for (uint32 l_I = 0; l_I < sGlyphRequiredSpecStore.GetNumRows(); ++l_I)
        {
            if (GlyphRequiredSpecEntry const* l_GlyphRequiredSpec = sGlyphRequiredSpecStore.LookupEntry(l_I))
                g_GlyphRequiredSpecs[l_GlyphRequiredSpec->GlyphID].push_back(l_GlyphRequiredSpec->SpecID);
        }

        for (uint32 l_I = 0; l_I < sJournalInstanceStore.GetNumRows(); ++l_I)
        {
            JournalInstanceEntry const* l_JournalInstance = sJournalInstanceStore.LookupEntry(l_I);
            if (!l_JournalInstance)
                continue;

            for (uint32 l_EncounterI = 0; l_EncounterI < sJournalEncounterStore.GetNumRows(); ++l_EncounterI)
            {
                JournalEncounterEntry const* l_JournalEncounter = sJournalEncounterStore.LookupEntry(l_EncounterI);
                if (!l_JournalEncounter)
                    continue;

                if (l_JournalEncounter->JournalInstanceID != l_JournalInstance->ID)
                    continue;

                for (uint32 l_ItemI = 0; l_ItemI < sJournalEncounterItemStore.GetNumRows(); l_ItemI++)
                {
                    JournalEncounterItemEntry const* l_JournalItem = sJournalEncounterItemStore.LookupEntry(l_ItemI);
                    if (!l_JournalItem)
                        continue;

                    if (l_JournalItem->JournalEncounterID != l_JournalEncounter->ID)
                        continue;

                    g_ItemLootPerMap[l_JournalInstance->MapID].push_back(l_JournalItem->ItemID);
                }
            }
        }

    itemsparseTh.join();
    itemModAppearanceTh.join();

    /// error checks
    if (l_BadDB2Files.size() >= DB2FilesCount)
    {
        sLog->outError(LOG_FILTER_GENERAL, "\nIncorrect DataDir value in worldserver.conf or ALL required *.db2 files (%d) not found by path: %sdb2", DB2FilesCount, p_DataPath.c_str());
        exit(1);
    }
    else if (!l_BadDB2Files.empty())
    {
        std::string l_Str;
        for (std::list<std::string>::iterator l_It = l_BadDB2Files.begin(); l_It != l_BadDB2Files.end(); ++l_It)
            l_Str += *l_It + "\n";

        sLog->outError(LOG_FILTER_GENERAL, "\nSome required *.db2 files (%u from %d) not found or not compatible:\n%s", (uint32)l_BadDB2Files.size(), DB2FilesCount, l_Str.c_str());
        exit(1);
    }

    uint32 l_MaxEffect = 0, l_MaxAuraEffect = 0;
    int32 l_MaxSpellMod = 0;
    bool l_ShutDown = false;

    for (uint32 l_ID = 0; l_ID < sSpellEffectStore.GetNumRows(); ++l_ID)
    {
        SpellEffectEntry const* l_Entry = sSpellEffectStore.LookupEntry(l_ID);

        if (!l_Entry)
            continue;

        l_MaxEffect = std::max(l_MaxEffect, l_Entry->Effect);
        l_MaxAuraEffect = std::max(l_MaxAuraEffect, l_Entry->EffectApplyAuraName);

        if (l_Entry->EffectApplyAuraName == SPELL_AURA_ADD_PCT_MODIFIER || l_Entry->EffectApplyAuraName == SPELL_AURA_ADD_FLAT_MODIFIER)
            l_MaxSpellMod = std::max(l_MaxSpellMod, l_Entry->EffectMiscValue);

        if (TOTAL_SPELL_EFFECTS <= l_MaxEffect)
        {
            sLog->outError(LOG_FILTER_GENERAL, "Fatal error: TOTAL_SPELL_EFFECTS needs to be set to %i\n", l_MaxEffect + 1);
            l_ShutDown = true;
        }

        if (TOTAL_AURAS <= l_MaxAuraEffect)
        {
            sLog->outError(LOG_FILTER_GENERAL, "Fatal error: TOTAL_AURAS needs to be set to %i\n", l_MaxAuraEffect + 1);
            l_ShutDown = true;
        }

        if (MAX_SPELLMOD <= l_MaxSpellMod)
        {
            sLog->outError(LOG_FILTER_GENERAL, "Fatal error: MAX_SPELLMOD needs to be set to %i\n", l_MaxSpellMod + 1);
            l_ShutDown = true;
        }

        if (l_ShutDown)
            exit(1);
    }

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Initialized %d DB2 data stores in %u ms", DB2FilesCount, GetMSTimeDiffToNow(oldMSTime));
}

uint8 ItemLimitCategoryEntry::GetMaxCount(Player const* p_Player) const
{
    uint8 l_Count = maxCount;

    for (uint32 l_Index = 0; l_Index < sItemLimitCategoryConditionStore.GetNumRows(); ++l_Index)
    {
        auto l_Entry = sItemLimitCategoryConditionStore.LookupEntry(l_Index);

        if (!l_Entry || l_Entry->CategoryID != ID)
            continue;

        if (p_Player->EvalPlayerCondition(l_Entry->PlayerConditionID).first)
            l_Count += l_Entry->Count;
    }

    return l_Count;
}