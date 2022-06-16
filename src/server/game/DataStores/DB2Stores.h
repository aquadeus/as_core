////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"
#include "ByteBuffer.h"
#include "DB2Store.h"
#include "DB2Structure.h"
#include <list>

/// Lookup table by DB2 hash
extern std::map<uint32, DB2StorageBase*> sDB2PerHash;

#define DEFINE_DB2_SET_COMPARATOR(structure) \
    struct structure ## Comparator : public std::binary_function<structure const*, structure const*, bool> \
    { \
        bool operator()(structure const* left, structure const* right) const { return Compare(left, right); } \
        static bool Compare(structure const* left, structure const* right); \
    };

////////////////////////////////////////////////////////////////////////////////
/// Spell DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Spell
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<OverrideSpellDataEntry>               sOverrideSpellDataStore;
    extern DB2Storage<SpecializationSpellEntry>             sSpecializationSpellStore;
    extern DB2Storage<SpellEntry>                           sSpellStore;
    extern DB2Storage<SpellAuraOptionsEntry>                sSpellAuraOptionsStore;
    extern DB2Storage<SpellAuraRestrictionsEntry>           sSpellAuraRestrictionsStore;
    extern DB2Storage<SpellCastingRequirementsEntry>        sSpellCastingRequirementsStore;
    extern DB2Storage<SpellCastTimesEntry>                  sSpellCastTimesStore;
    extern DB2Storage<SpellCategoriesEntry>                 sSpellCategoriesStore;
    extern DB2Storage<SpellCategoryEntry>                   sSpellCategoryStore;
    extern DB2Storage<SpellClassOptionsEntry>               sSpellClassOptionsStore;
    extern DB2Storage<SpellCooldownsEntry>                  sSpellCooldownsStore;
    extern DB2Storage<SpellDurationEntry>                   sSpellDurationStore;
    extern DB2Storage<SpellEffectEntry>                     sSpellEffectStore;
    extern DB2Storage<SpellEquippedItemsEntry>              sSpellEquippedItemsStore;
    extern DB2Storage<SpellFocusObjectEntry>                sSpellFocusObjectStore;
    extern DB2Storage<SpellInterruptsEntry>                 sSpellInterruptsStore;
    extern DB2Storage<SpellItemEnchantmentConditionEntry>   sSpellItemEnchantmentConditionStore;
    extern DB2Storage<SpellItemEnchantmentEntry>            sSpellItemEnchantmentStore;
    extern DB2Storage<SpellLevelsEntry>                     sSpellLevelsStore;
    extern DB2Storage<SpellMiscEntry>                       sSpellMiscStore;
    extern DB2Storage<SpellPowerEntry>                      sSpellPowerStore;
    extern DB2Storage<SpellProcsPerMinuteEntry>             sSpellProcsPerMinuteStore;
    extern DB2Storage<SpellProcsPerMinuteModEntry>          sSpellProcsPerMinuteModStore;
    extern DB2Storage<SpellRadiusEntry>                     sSpellRadiusStore;
    extern DB2Storage<SpellRangeEntry>                      sSpellRangeStore;
    extern DB2Storage<SpellReagentsEntry>                   sSpellReagentsStore;
    extern DB2Storage<SpellReagentsCurrencyEntry>           sSpellReagentsCurrencyStore;
    extern DB2Storage<SpellScalingEntry>                    sSpellScalingStore;
    extern DB2Storage<SpellShapeshiftEntry>                 sSpellShapeshiftStore;
    extern DB2Storage<SpellShapeshiftFormEntry>             sSpellShapeshiftFormStore;
    extern DB2Storage<SpellTargetRestrictionsEntry>         sSpellTargetRestrictionsStore;
    extern DB2Storage<SpellTotemsEntry>                     sSpellTotemsStore;
    //extern DB2Storage<SpellVisualEntry>                     sSpellVisualStore;
    extern DB2Storage<SpellXSpellVisualEntry>               sSpellXSpellVisualStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////
    struct SpellReagent
    {
        SpellReagent()
        {
            memset(reagents, 0, MAX_SPELL_REAGENTS * sizeof(SpellReagentsEntry*));
        }

        SpellReagentsEntry const* reagents[MAX_SPELL_REAGENTS];
    };

    struct SpellTotem
    {
        SpellTotem()
        {
            totems[0] = nullptr;
            totems[1] = nullptr;
        }

        SpellTotemsEntry const* totems[MAX_SPELL_TOTEMS];
    };

    struct SpellEffect
    {
        std::unordered_map<uint32, std::unordered_map<uint32, SpellEffectEntry const*>> effects;
    };

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using PetFamilySpellsSet          = std::set<uint32>;
    using PetFamilySpellsStore        = std::map<uint32, PetFamilySpellsSet>;
    using SpellEffectMap              = std::map<uint32, SpellEffect>;
    using SpellProcsPerMinuteVector   = std::vector<SpellProcsPerMinuteModEntry const*>;
    using SpellProcsPerMinuteModMap   = std::unordered_map<uint32, SpellProcsPerMinuteVector>;
    using SpellReagentMap             = std::unordered_map<uint32, SpellReagentsEntry const*>;
    using SpellAuraRestrictionMap     = std::unordered_map<uint32, SpellAuraRestrictionsEntry const*>;
    using SpellCastingRequirementsMap = std::unordered_map<uint32, SpellCastingRequirementsEntry const*>;
    using SpellClassOptionsMap        = std::unordered_map<uint32, SpellClassOptionsEntry const*>;
    using SpellEquippedItemsMap       = std::unordered_map<uint32, SpellEquippedItemsEntry const*>;
    using SpellReagentsCurrencyMap    = std::unordered_map<uint32, SpellReagentsCurrencyEntry const*>;
    using SpellScalingMap             = std::unordered_map<uint32, SpellScalingEntry const*>;
    using SpellShapeshiftMap          = std::unordered_map<uint32, SpellShapeshiftEntry const*>;
    using SpellTotemsMap              = std::unordered_map<uint32, SpellTotemsEntry const*>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    extern PetFamilySpellsStore         sPetFamilySpellsStore;
    extern SpellEffectMap               sSpellEffectMap;
    extern SpellProcsPerMinuteModMap    sSpellProcsPerMinuteMods;
    extern SpellReagentMap              sSpellReagentMap;
    extern SpellAuraRestrictionMap      sSpellAuraRestrictionMap;
    extern SpellCastingRequirementsMap  sSpellCastingRequirementsMap;
    extern SpellClassOptionsMap         sSpellClassOptionsMap;
    extern SpellEquippedItemsMap        sSpellEquippedItemsMap;
    extern SpellReagentsCurrencyMap     sSpellReagentsCurrencyMap;
    extern SpellScalingMap              sSpellScalingMap;
    extern SpellShapeshiftMap           sSpellShapeshiftMap;
    extern SpellTotemsMap               sSpellTotemsMap;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    /// Find a specific spell effect by index and difficulty
    /// @p_SpellID      : Looking for spell ID
    /// @p_EffectIndex  : Effect index for the spell
    /// @p_Difficulty   : Looking difficulty
    SpellEffectEntry const* GetSpellEffectEntry(uint32 p_SpellID, uint32 p_EffectIndex, uint32 p_Difficulty);
    /// Get list of Procs Pet Minute mods for a PPM info
    /// @p_SpellPPM_ID : PPM ID
    SpellProcsPerMinuteVector GetSpellProcsPerMinuteMods(uint32 spellprocsPerMinuteId);
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Garrison DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Garrison
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage <GarrSiteLevelEntry>              sGarrSiteLevelStore;
    extern DB2Storage <GarrSiteLevelPlotInstEntry>      sGarrSiteLevelPlotInstStore;
    extern DB2Storage <GarrPlotInstanceEntry>           sGarrPlotInstanceStore;
    extern DB2Storage <GarrPlotEntry>                   sGarrPlotStore;
    extern DB2Storage <GarrPlotUICategoryEntry>         sGarrPlotUICategoryStore;
    extern DB2Storage <GarrMissionEntry>                sGarrMissionStore;
    extern DB2Storage <GarrMissionXEncouterEntry>       sGarrMissionXEncouterStore;
    extern DB2Storage <GarrBuildingEntry>               sGarrBuildingStore;
    extern DB2Storage <GarrPlotBuildingEntry>           sGarrPlotBuildingStore;
    extern DB2Storage <GarrFollowerEntry>               sGarrFollowerStore;
    extern DB2Storage <GarrClassSpecEntry>              sGarrClassSpecStore;
    extern DB2Storage <GarrFollowerTypeEntry>           sGarrFollowerTypeStore;
    extern DB2Storage <GarrFollSupportSpellEntry>       sGarrFollSupportSpellStore;
    extern DB2Storage <GarrAbilityEntry>                sGarrAbilityStore;
    extern DB2Storage <GarrAbilityEffectEntry>          sGarrAbilityEffectStore;
    extern DB2Storage <GarrFollowerXAbilityEntry>       sGarrFollowerXAbilityStore;
    extern DB2Storage <GarrBuildingPlotInstEntry>       sGarrBuildingPlotInstStore;
    extern DB2Storage <GarrMechanicTypeEntry>           sGarrMechanicTypeStore;
    extern DB2Storage <GarrMechanicEntry>               sGarrMechanicStore;
    extern DB2Storage <GarrEncouterXMechanicEntry>      sGarrEncouterXMechanicStore;
    extern DB2Storage <GarrEncounterSetXEncounterEntry> sGarrEncounterSetXEncounterStore;
    extern DB2Storage <GarrFollowerLevelXPEntry>        sGarrFollowerLevelXPStore;
    extern DB2Storage <GarrSpecializationEntry>         sGarrSpecializationStore;
    extern DB2Storage <GarrTalentEntry>                 sGarrTalentStore;
    extern DB2Storage <GarrTalentTreeEntry>             sGarrTalentTreeStore;
    extern DB2Storage <GarrTypeEntry>                   sGarrTypeStore;
    extern DB2Storage <CharShipmentEntry>               sCharShipmentStore;
    extern DB2Storage <CharShipmentContainerEntry>      sCharShipmentContainerStore;
    extern DB2Storage <GarrItemLevelUpgradeDataEntry>   sGarrItemLevelUpgradeDataStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using GarrTalentTreeIDPerClass  = std::map<uint32, std::map<uint32, uint32>>;
    using GarrTalentTree            = std::vector<std::vector<const GarrTalentEntry*>>;
    using GarrMechanicList          = std::vector<GarrMechanicEntry const*>;
    using GarrEncounterList         = std::vector<uint32>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    extern GarrTalentTreeIDPerClass             s_GarrTalentTreeIDPerClass;
    extern std::map<uint32, GarrTalentTree>     s_GarrTalentTreeByTreeID;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    const GarrTalentTree* GetGarrisonTalentTreeByClassID(uint32 p_GarrisonType, uint32 p_Class);
    std::vector<const GarrTalentEntry*> const* GetGarrisonTalentsByClassAtRank(uint32 p_GarrisonType, uint32 p_Class, uint32 p_Rank);
    const std::set<GarrAbilityEffectEntry const*> GetGarrAbilityEffectEntryByEffectType(uint32 p_EffectType);
    const GarrTalentEntry* GetGarrTalentEntryByGarrAbilityID(uint32 p_GarrAbilityId);
    GarrMechanicList const* GetGarrMechanicListBySetID(uint32 p_SetID);
    GarrEncounterList const* GetGarrEncounterListBySetID(uint32 p_SetID);
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// BattlePet DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region BattlePet
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<BattlePetAbilityEntry>            sBattlePetAbilityStore;
    extern DB2Storage<BattlePetAbilityEffectEntry>      sBattlePetAbilityEffectStore;
    extern DB2Storage<BattlePetAbilityTurnEntry>        sBattlePetAbilityTurnStore;
    extern DB2Storage<BattlePetAbilityStateEntry>       sBattlePetAbilityStateStore;
    extern DB2Storage<BattlePetStateEntry>              sBattlePetStateStore;
    extern DB2Storage<BattlePetEffectPropertiesEntry>   sBattlePetEffectPropertiesStore;
    extern DB2Storage<BattlePetBreedQualityEntry>       sBattlePetBreedQualityStore;
    extern DB2Storage<BattlePetBreedStateEntry>         sBattlePetBreedStateStore;
    extern DB2Storage<BattlePetSpeciesEntry>            sBattlePetSpeciesStore;
    extern DB2Storage<BattlePetSpeciesStateEntry>       sBattlePetSpeciesStateStore;
    extern DB2Storage<BattlePetSpeciesXAbilityEntry>    sBattlePetSpeciesXAbilityStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using BattlePetSpeciesXAbilityBySpecies = std::map<uint32, std::set<BattlePetSpeciesXAbilityEntry const*>>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    extern BattlePetSpeciesXAbilityBySpecies            sBattlePetAbilitiesBySpecies;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Achievement DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Achievement
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage <AchievementEntry>                sAchievementStore;
    extern DB2Storage <AchievementCategoryEntry>        sAchievementCategoryStore;
    extern DB2Storage <CriteriaEntry>                   sCriteriaStore;
    extern DB2Storage <CriteriaTreeEntry>               sCriteriaTreeStore;
    extern DB2Storage <ModifierTreeEntry>               sModifierTreeStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Taxi DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Taxi
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage <TaxiNodesEntry>                  sTaxiNodesStore;
    extern DB2Storage <TaxiPathEntry>                   sTaxiPathStore;
    extern DB2Storage <TaxiPathNodeEntry>               sTaxiPathNodeStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////
    struct TaxiPathBySourceAndDestination
    {
        TaxiPathBySourceAndDestination() : ID(0), price(0) { }
        TaxiPathBySourceAndDestination(uint32 _id, uint32 _price) : ID(_id), price(_price) { }

        uint32 ID;
        uint32 price;
    };

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    #define TaxiMaskSize 258
    using TaxiPathSetForSource  = std::map<uint32, TaxiPathBySourceAndDestination>;
    using TaxiPathSetBySource   = std::map<uint32, TaxiPathSetForSource>;
    using TaxiPathNodeList      = std::vector<TaxiPathNodeEntry const*>;
    using TaxiPathNodesByPath   = std::vector<TaxiPathNodeList>;
    using TaxiMask              = std::array<uint8, TaxiMaskSize>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    extern TaxiMask                 sTaxiNodesMask;
    extern TaxiMask                 sOldContinentsNodesMask;
    extern TaxiMask                 sHordeTaxiNodesMask;
    extern TaxiMask                 sAllianceTaxiNodesMask;
    extern TaxiMask                 sArgusTaxiMask;
    extern TaxiPathSetBySource      sTaxiPathSetBySource;
    extern TaxiPathNodesByPath      sTaxiPathNodesByPath;

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
    void DeterminateAlternateMapPosition(uint32 p_MapID, float p_X, float p_Y, float p_Z, uint32* p_NewMapID = nullptr, float* p_NewPosX = nullptr, float* p_NewPosY = nullptr);

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Artifact DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Artifact
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage <ArtifactEntry>                sArtifactStore;
    extern DB2Storage <ArtifactAppearanceEntry>      sArtifactAppearanceStore;
    extern DB2Storage <ArtifactAppearanceSetEntry>   sArtifactAppearanceSetStore;
    extern DB2Storage <ArtifactCategoryEntry>        sArtifactCategoryStore;
    extern DB2Storage <ArtifactPowerEntry>           sArtifactPowerStore;
    extern DB2Storage <ArtifactPowerLinkEntry>       sArtifactPowerLinkStore;
    extern DB2Storage <ArtifactPowerPickerEntry>     sArtifactPowerPickerStore;
    extern DB2Storage <ArtifactPowerRankEntry>       sArtifactPowerRankStore;
    extern DB2Storage <ArtifactQuestXPEntry>         sArtifactQuestXPStore;
    extern DB2Storage <RelicTalentEntry>             sRelicTalentStore;


    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using ArtifactPowerRanksContainer = std::unordered_map<std::pair<uint32, uint8>, ArtifactPowerRankEntry const*>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    ArtifactPowerRankEntry const* GetArtifactPowerRank(uint32 p_ArtifactPowerId, uint8 p_Rank);
    bool IsArtifactSpell(uint32 p_spellId);
    ArtifactPowerRankEntry const* GetArtifactPowerRankBySpellID(uint32 p_spellId, Player const* p_Player = nullptr);
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// SceneScript DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region SceneScript
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage <SceneScriptPackageEntry>         sSceneScriptPackageStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// WebBrowser DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region WebBrowser
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage <WbAccessControlListEntry>        sWbAccessControlListStore;      ///< Only for hotifx
    extern DB2Storage <WbCertWhitelistEntry>            sWbCertWhitelistStore;          ///< Only for hotifx

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Item DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Item
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<ItemPriceBaseEntry>               sItemPriceBaseStore;
    extern DB2Storage<ItemClassEntry>                   sItemClassStore;
    extern DB2Storage<ItemDisenchantLootEntry>          sItemDisenchantLootStore;
    extern DB2Storage<ItemLimitCategoryEntry>           sItemLimitCategoryStore;
    extern DB2Storage<ItemLimitCategoryConditionEntry>  sItemLimitCategoryConditionStore;
    extern DB2Storage<ItemRandomPropertiesEntry>        sItemRandomPropertiesStore;
    extern DB2Storage<ItemRandomSuffixEntry>            sItemRandomSuffixStore;
    extern DB2Storage<ItemSpecEntry>                    sItemSpecStore;
    extern DB2Storage<ItemSpecOverrideEntry>            sItemSpecOverrideStore;
    extern DB2Storage<ItemEntry>                        sItemStore;
    extern DB2Storage<ItemBonusEntry>                   sItemBonusStore;
    extern DB2Storage<ItemBonusTreeNodeEntry>           sItemBonusTreeNodeStore;
    extern DB2Storage<ItemBonusListLevelDeltaEntry>     sItemBonusListLevelDeltaStore;
    extern DB2Storage<ItemXBonusTreeEntry>              sItemXBonusTreeStore;
    extern DB2Storage<ItemCurrencyCostEntry>            sItemCurrencyCostStore;
    extern DB2Storage<ItemExtendedCostEntry>            sItemExtendedCostStore;
    extern DB2Storage<ItemSparseEntry>                  sItemSparseStore;
    extern DB2Storage<ItemEffectEntry>                  sItemEffectStore;
    extern DB2Storage<ItemModifiedAppearanceEntry>      sItemModifiedAppearanceStore;
    extern DB2Storage<ItemAppearanceEntry>              sItemAppearanceStore;
    extern DB2Storage<TransmogSetItemEntry>             sTransmogSetItemStore;
    extern DB2Storage<TransmogSetEntry>                 sTransmogSetStore;
    extern DB2Storage<ItemUpgradeEntry>                 sItemUpgradeStore;
    extern DB2Storage<ItemChildEquipmentEntry>          sItemChildEquipmentStore;
    extern DB2Storage<ItemSetSpellEntry>                sItemSetSpellStore;
    extern DB2Storage<ItemArmorQualityEntry>            sItemArmorQualityStore;
    extern DB2Storage<ItemArmorShieldEntry>             sItemArmorShieldStore;
    extern DB2Storage<ItemArmorTotalEntry>              sItemArmorTotalStore;
    extern DB2Storage<ItemBagFamilyEntry>               sItemBagFamilyStore;
    extern DB2Storage<ItemDamageEntry>                  sItemDamageAmmoStore;
    extern DB2Storage<ItemDamageEntry>                  sItemDamageOneHandStore;
    extern DB2Storage<ItemDamageEntry>                  sItemDamageOneHandCasterStore;
    extern DB2Storage<ItemDamageEntry>                  sItemDamageTwoHandStore;
    extern DB2Storage<ItemDamageEntry>                  sItemDamageTwoHandCasterStore;
    extern DB2Storage<ItemSetEntry>                     sItemSetStore;
    extern DB2Storage<ItemSearchNameEntry>              sItemSearchNameStore;
    extern DB2Storage<RulesetItemUpgradeEntry>          sRulesetItemUpgradeStore;
    extern DB2Storage<PvpItemEntry>                     sPvpItemStore;
    extern DB2Storage<PvpScalingEffectEntry>            sPvpScalingEffectStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using ItemSetSpellsByItemID                 = std::unordered_map<uint32, std::list<ItemSetSpellEntry const*>>;
    using ItemModifiedAppearanceByItemContainer = std::unordered_map<uint32 /*itemId | appearanceMod << 24*/, ItemModifiedAppearanceEntry const*>;
    using TransmogSetItemByTransmogSetContainer = std::unordered_multimap<uint32, TransmogSetItemEntry const*>;
    using ItemSpecOverridesContainer            = std::unordered_map<uint32, std::vector<ItemSpecOverrideEntry const*>>;
    using ItemChildEquipmentContainer           = std::unordered_map<uint32 /*itemId*/, ItemChildEquipmentEntry const*>;
    using ItemChildIdSet                        = std::unordered_set<uint32>;
    using ItemBonusListLevelDeltaContainer      = std::unordered_map<int16, uint32>;
    using ItemClassByOldEnumContainer           = std::array<ItemClassEntry const*, 19>;
    using PvpScalingPerSpecs                    = std::unordered_map<uint32, std::unordered_map<uint32, float>>;


    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    extern ItemSetSpellsByItemID                                        sItemSetSpellsByItemIDStore;
    extern std::map<uint32, std::vector<uint32>>                        sItemEffectsByItemID;
    extern std::map<uint32, std::vector<ItemBonusEntry const*>>         sItemBonusesByID;
    extern std::map<uint32, std::vector<ItemXBonusTreeEntry const*>>    sItemBonusTreeByID;
    extern std::map<uint32, uint32>                                     g_PvPItemStoreLevels;
    extern ItemModifiedAppearanceByItemContainer                        _itemModifiedAppearancesByItem;
    extern ItemModifiedAppearanceByItemContainer                        _itemDefaultAppearancesByItem;
    extern TransmogSetItemByTransmogSetContainer                        g_TransmogSetItemByTransmogSet;
    extern ItemSpecOverridesContainer                                   _itemSpecOverrides;
    extern ItemBonusListLevelDeltaContainer                             g_ItemLevelDeltaToBonusListContainer;
    extern ItemClassByOldEnumContainer                                  g_itemClassByOldEnum;
    extern ItemChildIdSet                                               g_ItemChildIds;
    extern PvpScalingPerSpecs                                           g_PvpScalingPerSpecs;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    /// Get item bonus by item ID
    /// @p_Id : Item id
    std::vector<ItemBonusEntry const*> const* GetItemBonusesByID(uint32 p_Id);
    /// Get display id by item id and appearance mod ID
    /// @p_ItemID           : Item id
    /// @p_AppearanceModID  : Appearance modifier ID
    uint32 GetItemDisplayID(uint32 p_ItemID, uint32 p_AppearanceModID);
    ItemModifiedAppearanceEntry const* GetItemModifiedAppearance(uint32 p_ItemID, uint32 p_AppearanceModId);
    ItemModifiedAppearanceEntry const* GetDefaultItemModifiedAppearance(uint32 p_ItemID);
    std::vector<ItemSpecOverrideEntry const*> const* GetItemSpecOverrides(uint32 p_ItemID);
    /// Get display id by item id and appearance mod ID
    /// @p_ItemID           : Item id
    /// @p_AppearanceModID  : Appearance modifier ID
    ItemChildEquipmentEntry const* GetItemChildEquipment(uint32 p_ItemID);
    uint32 GetParentItemIDIfExists(uint32 p_ItemID);
    uint32 GetItemIDByArtifactID(uint32 p_ArtifactID);
    uint32 GetArtifactItemIDEntryBySpec(uint32 p_SpecID);
    uint32 GetItemBonusListForItemLevelDelta(int16 p_Delta);
    ItemClassEntry const* GetItemClassByOldEnum(uint32 p_ItemClass);
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Archaeology DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Archaeology
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<ResearchBranchEntry>          sResearchBranchStore;
    extern DB2Storage<ResearchProjectEntry>         sResearchProjectStore;
    extern DB2Storage<ResearchSiteEntry>            sResearchSiteStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////
    struct ResearchPOIPoint
    {
        ResearchPOIPoint(int32 _x, int32 _y) { x = _x; y = _y; }
        int32 x;
        int32 y;
    };

    struct ResearchZoneEntry
    {
        uint32 POIid;
        std::vector<ResearchPOIPoint> coords;
        uint16 map;
        uint16 zone;
        uint8 level;
    };

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    extern std::set<ResearchProjectEntry const*>                   sResearchProjectSet;
    extern std::set<ResearchSiteEntry const*>                      sResearchSiteSet;
    extern std::unordered_map<uint32, ResearchProjectEntry const*> sResearchProjectPerSpell;
    extern std::map<uint32 /*site_id*/, ResearchZoneEntry>         sResearchSiteDataMap;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Quest DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Quest
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<AdventureMapPOIEntry>         sAdventureMapPOIStore;
    extern DB2Storage<QuestV2CliTaskEntry>          sQuestV2CliTaskStore;
    extern DB2Storage<BountyEntry>                  sBountyStore;
    extern DB2Storage<QuestPackageItemEntry>        sQuestPackageItemStore;
    extern DB2Storage<QuestPOIPointEntry>           sQuestPOIPointStore;
    extern DB2Storage<QuestSortEntry>               sQuestSortStore;
    extern DB2Storage<QuestV2Entry>                 sQuestV2Store;
    extern DB2Storage<QuestXPEntry>                 sQuestXPStore;
    extern DB2Storage<QuestFactionRewEntry>         sQuestFactionRewardStore;
    extern DB2Storage<QuestMoneyRewardEntry>        sQuestMoneyRewardStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    extern std::map<uint32, std::vector<QuestPackageItemEntry const*>>  sQuestPackageItemsByGroup;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    /// Get quest bit flag
    /// @p_QuestID : Entry of the quest
    uint32 GetQuestUniqueBitFlag(uint32 p_QuestID);
    std::vector<QuestPackageItemEntry const*> const* GetQuestPackageItems(uint32 questPackageID);
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Area and maps DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region AreaAndMaps
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<AreaTableEntry>               sAreaTableStore;
    extern DB2Storage<AreaGroupMemberEntry>         sAreaGroupMemberStore;
    extern DB2Storage<AreaTriggerEntry>             sAreaTriggerStore;
    extern DB2Storage<DifficultyEntry>              sDifficultyStore;
    extern DB2Storage<LiquidTypeEntry>              sLiquidTypeStore;
    extern DB2Storage<MapEntry>                     sMapStore;
    extern DB2Storage<MapDifficultyEntry>           sMapDifficultyStore;
    extern DB2Storage<PhaseEntry>                   sPhaseStore;
    extern DB2Storage<WorldMapOverlayEntry>         sWorldMapOverlayStore;
    extern DB2Storage<WMOAreaTableEntry>            sWMOAreaTableStore;
    extern DB2Storage<WorldMapAreaEntry>            sWorldMapAreaStore;
    extern DB2Storage<WorldMapTransformsEntry>      sWorldMapTransformsStore;
    extern DB2Storage<World_PVP_AreaEntry>          sWorld_PVP_AreaStore;

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using MapDifficultyContainer= std::unordered_map<uint32, std::unordered_map<uint32, MapDifficultyEntry const*>> ;
    using AreaGroupMembersByID  = std::unordered_map<uint32, std::vector<uint32>>;
    using AreaGroupsByAreaID    = ACE_Based::LockedMap<uint32, std::set<uint32>>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    extern MapDifficultyContainer _mapDifficulties;
    extern AreaGroupsByAreaID     s_AreaGroupsByAreaID;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    MapDifficultyEntry const* GetDefaultMapDifficulty(uint32 p_MapID, Difficulty* p_Difficulty = nullptr);
    MapDifficultyEntry const* GetMapDifficultyData(uint32 p_MapID, Difficulty difficulty);
    MapDifficultyEntry const* GetDownscaledMapDifficultyData(uint32 p_MapID, Difficulty& p_Difficulty);
    uint32 GetMaxLevelForExpansion(uint32 expansion);
    uint32 GetVirtualMapForMapAndZone(uint32 mapid, uint32 zoneId);
    WMOAreaTableEntry const* GetWMOAreaTableEntryByTripple(int32 rootid, int32 adtid, int32 groupid);
    void Zone2MapCoordinates(float &x, float &y, uint32 zone);
    void Map2ZoneCoordinates(float &x, float &y, uint32 zone);
    uint32 GetLiquidFlags(uint32 liquidType);
    std::vector<uint32> GetAreasForGroup(uint32 areaGroupId);
    bool IsWithinAreaGroup(uint32 p_AreaGroup, uint32 p_AreaID);
    void DeterminaAlternateMapPosition(uint32 p_MapID, float p_X, float p_Y, float p_Z, uint32* p_NewMapID = nullptr, float* p_NewPosX = nullptr, float* p_NewPosY = nullptr);
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Import DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Import
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<ImportPriceArmorEntry>        sImportPriceArmorStore;
    extern DB2Storage<ImportPriceQualityEntry>      sImportPriceQualityStore;
    extern DB2Storage<ImportPriceShieldEntry>       sImportPriceShieldStore;
    extern DB2Storage<ImportPriceWeaponEntry>       sImportPriceWeaponStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Char DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Char
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<CharStartOutfitEntry>         sCharStartOutfitStore;
    extern DB2Storage<CharTitlesEntry>              sCharTitlesStore;
    extern DB2Storage<ChrClassesEntry>              sChrClassesStore;
    extern DB2Storage<ChrClassXPowerTypesEntry>     sChrClassXPowerTypesStore;
    extern DB2Storage<ChrRacesEntry>                sChrRacesStore;
    extern DB2Storage<ChrSpecializationsEntry>      sChrSpecializationsStore;
    extern DB2Storage<CharacterLoadoutItemEntry>    sCharLoadoutItemStore;
    extern DB2Storage<CharacterLoadoutEntry>        sCharLoadoutStore;
    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using ChrSpecializationByIndexContainer = ChrSpecializationsEntry const* [MAX_CLASSES + 1][MAX_SPECIALIZATIONS];
    using ChrSpecialzationByClassContainer  = std::unordered_map<uint32, ChrSpecializationsEntry const*>;
    using ChrSpecializationsByByClass       = std::unordered_map<uint32, std::vector<ChrSpecializationsEntry const*>>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    static ChrSpecializationByIndexContainer _chrSpecializationsByIndex;
    static ChrSpecialzationByClassContainer  _defaultChrSpecializationsByClass;
    static ChrSpecializationsByByClass       _chrSpecializationsByClass;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    ChrSpecializationsEntry const* GetChrSpecializationByIndex(uint32 class_, uint32 index);
    ChrSpecializationsEntry const* GetDefaultChrSpecializationForClass(uint32 class_);
    std::vector<ChrSpecializationsEntry const*> const& GetChrSpecializationsForClass(uint32 p_Class);
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Creature DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Creature
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<CreatureDisplayInfoEntry>     sCreatureDisplayInfoStore;
    extern DB2Storage<CreatureDisplayInfoExtraEntry>sCreatureDisplayInfoExtraStore;
    extern DB2Storage<CreatureFamilyEntry>          sCreatureFamilyStore;
    extern DB2Storage<CreatureModelDataEntry>       sCreatureModelDataStore;
    extern DB2Storage<CreatureTypeEntry>            sCreatureTypeStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// Misc DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region Misc
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<AnimKitEntry>                                  sAnimKitStore;
    extern DB2Storage<ArmorLocationEntry>                            sArmorLocationStore;
    extern DB2Storage<AuctionHouseEntry>                             sAuctionHouseStore;
    extern DB2Storage<BankBagSlotPricesEntry>                        sBankBagSlotPricesStore;
    extern DB2Storage<BarberShopStyleEntry>                          sBarberShopStyleStore;
    extern DB2Storage<BattlemasterListEntry>                         sBattlemasterListStore;
    extern DB2Storage<BroadcastTextEntry>                            sBroadcastTextStore;
    extern DB2Storage<CharSectionsEntry>                             sCharSectionsStore;
    extern DB2Storage<CinematicCameraEntry>                          sCinematicCameraStore;
    extern DB2Storage<CinematicSequencesEntry>                       sCinematicSequencesStore;
    extern DB2Storage<ChatChannelsEntry>                             sChatChannelsStore;
    extern DB2Storage<CurrencyTypesEntry>                            sCurrencyTypesStore;
    extern DB2Storage<CurveEntry>                                    sCurveStore;
    extern DB2Storage<CurvePointEntry>                               sCurvePointStore;
    extern DB2Storage<DestructibleModelDataEntry>                    sDestructibleModelDataStore;
    extern DB2Storage<DungeonEncounterEntry>                         sDungeonEncounterStore;
    extern DB2Storage<DurabilityCostsEntry>                          sDurabilityCostsStore;
    extern DB2Storage<DurabilityQualityEntry>                        sDurabilityQualityStore;
    extern DB2Storage<EmotesEntry>                                   sEmotesStore;
    extern DB2Storage<EmotesTextEntry>                               sEmotesTextStore;
    extern DB2Storage<EmotesTextSoundEntry>                          sEmotesTextSoundStore;
    extern DB2Storage<FactionEntry>                                  sFactionStore;
    extern DB2Storage<FactionTemplateEntry>                          sFactionTemplateStore;
    extern DB2Storage<GameObjectDisplayInfoEntry>                    sGameObjectDisplayInfoStore;
    extern DB2Storage<GemPropertiesEntry>                            sGemPropertiesStore;
    extern DB2Storage<GlyphBindableSpellEntry>                       sGlyphBindableSpellStore;
    extern DB2Storage<GlyphPropertiesEntry>                          sGlyphPropertiesStore;
    extern DB2Storage<GlyphRequiredSpecEntry>                        sGlyphRequiredSpecStore;
    extern DB2Storage<GroupFinderActivityEntry>                      sGroupFinderActivityStore;
    extern DB2Storage<GroupFinderCategoryEntry>                      sGroupFinderCategoryStore;
    extern DB2Storage<GuildPerkSpellsEntry>                          sGuildPerkSpellsStore;
    extern DB2Storage<GlobalStringsEntry>                            sGlobalStringsStore;
    extern DB2Storage<HeirloomEntry>                                 sHeirloomStore;
    extern DB2Storage<HolidaysEntry>                                 sHolidaysStore;
    extern DB2Storage<JournalInstanceEntry>                          sJournalInstanceStore;
    extern DB2Storage<JournalEncounterEntry>                         sJournalEncounterStore;
    extern DB2Storage<JournalEncounterItemEntry>                     sJournalEncounterItemStore;
    extern DB2Storage<LFGDungeonEntry>                               sLFGDungeonStore;
    extern DB2Storage<LocationEntry>                                 sLocationStore;
    extern DB2Storage<LockEntry>                                     sLockStore;
    extern DB2Storage<MailTemplateEntry>                             sMailTemplateStore;
    extern DB2Storage<MapChallengeModeEntry>                         sMapChallengeModeStore;
    extern DB2Storage<MinorTalentEntry>                              sMinorTalentStore;
    extern DB2Storage<MountCapabilityEntry>                          sMountCapabilityStore;
    extern DB2Storage<MountEntry>                                    sMountStore;
    extern DB2Storage<MountTypeXCapabilityEntry>                     sMountTypeXCapabilityStore;
    extern DB2Storage<MountXDisplayEntry>                            sMountXDisplayStore;
    extern DB2Storage<MovieEntry>                                    sMovieStore;
    extern DB2Storage<NameGenEntry>                                  sNameGenStore;
    extern DB2Storage<ParagonReputationEntry>                        sParagonReputationStore;
    extern DB2Storage<PathNodeEntry>                                 sPathNodeStore;
    extern DB2Storage<PlayerConditionEntry>                          sPlayerConditionStore;
    extern DB2Storage<PowerDisplayEntry>                             sPowerDisplayStore;
    extern DB2Storage<PowerTypeEntry>                                sPowerTypeStore;
    extern DB2Storage<PrestigeLevelInfoEntry>                        sPrestigeLevelInfoStore;
    extern DB2Storage<PvPDifficultyEntry>                            sPvPDifficultyStore;
    extern DB2Storage<PvpTalentEntry>                                sPvpTalentStore;
    extern DB2Storage<PvpTalentUnlockEntry>                          sPvpTalentUnlockStore;
    extern DB2Storage<PvpRewardEntry>                                sPvpRewardStore;
    extern DB2Storage<RandomPropertiesPointsEntry>                   sRandomPropertiesPointsStore;
    extern DB2Storage<RewardPackEntry>                               sRewardPackStore;
    extern DB2Storage<RewardPackXCurrencyTypeEntry>                  sRewardPackXCurrencyTypeStore;
    extern DB2Storage<RewardPackXItemEntry>                          sRewardPackXItemStore;
    extern DB2Storage<ScalingStatDistributionEntry>                  sScalingStatDistributionStore;
    extern DB2Storage<SandboxScalingEntry>                           sSandboxScalingStore;
    extern DB2Storage<ScenarioEntry>                                 sScenarioStore;
    extern DB2Storage<ScenarioStepEntry>                             sScenarioStepStore;
    extern DB2Storage<SkillLineAbilityEntry>                         sSkillLineAbilityStore;
    extern DB2Storage<SkillRaceClassInfoEntry>                       sSkillRaceClassInfoStore;
    extern DB2Storage<SkillLineEntry>                                sSkillLineStore;
    extern DB2Storage<SoundKitEntry>                                 sSoundKitStore;
    extern DB2Storage<SummonPropertiesEntry>                         sSummonPropertiesStore;
    extern DB2Storage<TactKeyEntry>                                  sTactKeyStore;
    extern DB2Storage<TalentEntry>                                   sTalentStore;
    extern DB2Storage<TotemCategoryEntry>                            sTotemCategoryStore;
    extern DB2Storage<TransportAnimationEntry>                       sTransportAnimationStore;
    extern DB2Storage<TransportRotationEntry>                        sTransportRotationStore;
    extern DB2Storage<ToyEntry>                                      sToyStore;
    extern DB2Storage<VehicleEntry>                                  sVehicleStore;
    extern DB2Storage<VehicleSeatEntry>                              sVehicleSeatStore;
    extern DB2Storage<VignetteEntry>                                 sVignetteStore;
    extern DB2Storage<WorldSafeLocsEntry>                            sWorldSafeLocsStore;
    extern DB2Storage<AreaPOIEntry>                                  sAreaPOIStore;
    extern DB2Storage<ContributionEntry>                             sContributionStore;
    extern DB2Storage<ManagedWorldStateEntry>                        sManagedWorldStateStore;
    extern DB2Storage<ManagedWorldStateBuffEntry>                    sManagedWorldStateBuffStore;
    extern DB2Storage<ManagedWorldStateInputEntry>                   sManagedWorldStateInputStore;

    //////////////////////////////////////////////////////////////////////////
    /// Structures
    //////////////////////////////////////////////////////////////////////////
    DEFINE_DB2_SET_COMPARATOR(MountTypeXCapabilityEntry);

    struct VectorArray
    {
        std::vector<std::string> stringVectorArray[2];
    };

    struct TalentSpellPos
    {
        TalentSpellPos() : talent_id(0), rank(0) {}
        TalentSpellPos(uint16 _talent_id, uint8 _rank) : talent_id(_talent_id), rank(_rank) {}

        uint16 talent_id;
        uint8  rank;
    };

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using SimpleFactionsList                = std::list<uint32>;
    using EmotesTextSoundKey                = std::tuple<uint32, uint32, uint32>;
    using FactionTeamMap                    = std::map<uint32, SimpleFactionsList>;
    using MountTypeXCapabilitySet           = std::set<MountTypeXCapabilityEntry const*, MountTypeXCapabilityEntryComparator>;
    using MountCapabilitiesByTypeContainer  = std::unordered_map<uint32, MountTypeXCapabilitySet>;
    using MountBySpellContainer             = std::unordered_map<uint32, MountEntry const*>;
    using MountVisualsMap                   = std::unordered_map<uint32, std::vector<MountXDisplayEntry const*>>;
    using NameGenVectorArraysMap            = std::map<uint32, VectorArray>;
    using TalentSpellPosMap                 = std::map<uint32, TalentSpellPos>;
    using SkillRaceClassInfoContainer       = std::unordered_multimap<uint32, SkillRaceClassInfoEntry const*>;
    using SpecializationSpellsContainer     = std::unordered_map<uint32, std::vector<SpecializationSpellEntry const*>>;
    using TalentsByPosition                 = std::vector<TalentEntry const*> [MAX_CLASSES][MAX_SPECIALIZATIONS][MAX_TALENT_TIERS][MAX_TALENT_COLUMNS];
    using GlyphBindableSpellsContainer      = std::unordered_map<uint32, std::vector<uint32>>;
    using GlyphRequiredSpecsContainer       = std::unordered_map<uint32, std::vector<uint32>>;
    using SkillLineAbilityPerSkill          = std::unordered_map<uint32, std::vector<SkillLineAbilityEntry const*>>;
    using SkillLineAbilityReqSpells         = std::unordered_map<uint32, uint32>;
    using CurvePointsContainer              = std::unordered_map<uint32 /*curveID*/, std::vector<CurvePointEntry const*>> ;
    using CharSectionsContainer             = std::unordered_multimap<uint32, CharSectionsEntry const*>;
    using MapChallengeModeEntryContainer    = std::unordered_multimap<uint32, MapChallengeModeEntry const*>;
    using ItemLootPerMapContainer           = std::unordered_map<uint32, std::vector<uint32>>;
    using SingleDungonPerMap                = std::map<uint32, LFGDungeonEntry const*>;
    using ToyItemMap                        = std::unordered_map<uint32, ToyEntry const*>;

    //////////////////////////////////////////////////////////////////////////
    /// Variables
    //////////////////////////////////////////////////////////////////////////
    static std::map<EmotesTextSoundKey, EmotesTextSoundEntry const*> sEmotesTextSoundMap;
    static MountCapabilitiesByTypeContainer     _mountCapabilitiesByType;
    static MountBySpellContainer                g_MountBySpell;
    static MountVisualsMap                      g_MountVisualsByID;
    static FactionTeamMap                       sFactionTeamMap;
    static std::vector<std::vector<uint32>>     g_PvpTalentLevelRequirement;
    static SkillRaceClassInfoContainer          _skillRaceClassInfoBySkill;
    static std::unordered_map<uint32, std::vector<SkillLineAbilityEntry const*>> _skillLineAbilitiesBySkillupSkill;
    static SpecializationSpellsContainer        _specializationSpellsBySpec;
    static TalentsByPosition                    _talentsByPosition;
    static GlyphBindableSpellsContainer         g_GlyphBindableSpells;
    static GlyphRequiredSpecsContainer          g_GlyphRequiredSpecs;
    static std::vector<uint32>                  _challengeModeMaps;
    static MapChallengeModeEntryContainer       _mapChallengeModeEntrybyMap;
    static MapChallengeModeEntryContainer       g_LegionMapChallengeModeEntrybyMap;
    static ItemLootPerMapContainer              g_ItemLootPerMap;
    extern SkillLineAbilityPerSkill             g_SkillLineAbilityPerSkill;
    extern SkillLineAbilityReqSpells            g_SkillLineAbilityReqSpells;
    extern CurvePointsContainer                 g_CurvePoints;
    extern CharSectionsContainer                g_CharSections;
    static SingleDungonPerMap                   g_SingleDungeonsPerMap;
    static ToyItemMap                           g_ToyItems;

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////
    uint32 GetSkillLineAbilityReqSpell(uint32 spellId);
    uint8 GetPowerIndexByClass(uint8 p_Class, uint8 p_Power);
    uint32 GetHeirloomItemLevel(uint32 curveId, uint32 level);
    HeirloomEntry const* GetHeirloomEntryByItemID(uint32 p_ItemID);
    uint32 GetMaxPrestigeLevel();
    SimpleFactionsList const* GetFactionTeamList(uint32 faction);
    EmotesTextSoundEntry const* FindTextSoundEmoteFor(uint32 p_Emote, uint32 p_Race, uint32 p_Gender);
    char const* GetPetName(uint32 petfamily, uint32 dbclang);
    const std::string* GetRandomCharacterName(uint8 race, uint8 gender);
    void HotfixLfgDungeonsData();
    MountTypeXCapabilitySet const* GetMountCapabilities(uint32 mountType);
    MountEntry const* GetMountBySpell(uint32 p_SpellId);
    std::vector<SkillLineAbilityEntry const*> const* GetSkillLineAbilitiesBySkill(uint32 skillId);
    uint32 GetPvPTalentLevelRequirement(uint32 p_Collumn, uint32 p_Tier);
    SkillRaceClassInfoEntry const* GetSkillRaceClassInfo(uint32 skill, uint8 race, uint8 class_);
    std::vector<SpecializationSpellEntry const*> const* GetSpecializationSpells(uint32 specId);
    std::vector<TalentEntry const*> const& GetTalentsByPosition(uint32 class_, uint32 p_SpecializationId, uint32 tier, uint32 column);
    std::vector<uint32> const* GetGlyphBindableSpells(uint32 p_GlyphPropID);
    std::vector<uint32> const* GetGlyphRequiredSpecs(uint32 p_GlyphPropID);
    void LoadSpellAuraOptionBySpellIdDifficulty();
    uint32 GetMaxDifficultyOptionBelowCurrentDifficulty(uint32 p_SpellID, uint32 p_Difficulty);
    float GetCurveValueAt(uint32 curveId, float x);
    char const* GetBroadcastTextValue(BroadcastTextEntry const* p_BroadcastTextEntry, LocaleConstant p_Locale = DEFAULT_LOCALE, uint8 p_Gender = Gender::GENDER_MALE, bool p_ForceGender = false);
    CharSectionsEntry const* GetCharSectionEntry(uint8 p_Race, CharSectionType p_GenType, uint8 p_Gender, uint8 p_Type, uint8 p_Color);
    std::vector<uint32>* GetChallengeMaps();
    MapChallengeModeEntryContainer const* GetLegionChallengeModeEntries();
    std::vector<uint32> GetItemLootPerMap(uint32 p_MapID);
    uint32 GetMountDisplayID(Player const* p_Player, uint32 p_MountID);
    LFGDungeonEntry const* GetSingleDungonByMap(uint32 p_MapID);
    ToyEntry const* GetToyEntryFromItemId(uint32 p_ItemID);
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
/// WorldState DB2
////////////////////////////////////////////////////////////////////////////////
#pragma region WorldState
    //////////////////////////////////////////////////////////////////////////
    /// Store list
    //////////////////////////////////////////////////////////////////////////
    extern DB2Storage<WorldStateExpressionEntry>                     sWorldStateExpressionStore;

    //////////////////////////////////////////////////////////////////////////
    /// Types
    //////////////////////////////////////////////////////////////////////////
    using WorldStateIdByWorldQuest =  std::unordered_map<uint32 /*p_QuestID*/, uint32 /*WorldStateID*/>;

    //////////////////////////////////////////////////////////////////////////
    /// Stores
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Methods
    //////////////////////////////////////////////////////////////////////////

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void LoadDB2Stores(const std::string& p_DataPath);
