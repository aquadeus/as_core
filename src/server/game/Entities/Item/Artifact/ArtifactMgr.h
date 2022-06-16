///////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2014-2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Common.h"
#include "DB2Stores.h"

class Item;
class AuraEffect;

namespace MS { namespace Artifact
{
    namespace ArtifactFlags
    {
        enum
        {
            AltOnTop            = 1, ///< Found in Lua, WTF ?
            HasChildEquipement  = 2, ///< ItemChildEquipement.db2 -> Eg: Enhancement molten off-hand
        };
    }

    namespace ArtifactPowerFlags
    {
        enum
        {
            IsGold                  = 0x01, ///< Golden Dragon around the trait
            IsStart                 = 0x02, ///< First thing, unlocks the tree
            IsFinal                 = 0x04, ///< Whether the current rank is final or not
            ScalesWhitNumPowers     = 0x08, ///<
            DontCountFirstBonusRank = 0x10, ///<
            Tier1Upgrade            = 0x20,
        };
    }

    struct ArtifactPowerRanks
    {
        ArtifactPowerRanks() : PurchasedRank(0), CurrentRankWithBonus(0) {}

        ArtifactPowerRanks(uint8 p_PurchasedRank, uint8 p_CurrentRankWithBonus) : PurchasedRank(p_PurchasedRank), CurrentRankWithBonus(p_CurrentRankWithBonus) {}

        uint32 ToDynamicField()
        {
            return PurchasedRank | CurrentRankWithBonus << 8;
        }

        uint8 PurchasedRank;
        uint8 CurrentRankWithBonus;
    };

    class Manager
    {
    public:
        Manager(uint32 p_ArtifactID);

        static Manager* GenerateNew(uint32 p_ArtifactID);

        void SaveToDB(SQLTransaction& p_Trans);
        void LoadFromDB(uint64 p_ArtifactPower, uint32 p_AppearanceModId, uint8 p_Tier, std::unordered_map<uint32, ArtifactPowerRanks> const* p_Powers);
        void SetOwner(Player* p_Player);
        void SetItemOwner(Item* p_Owner);

        void Init();

        uint32 GetMaxLevel() const;
        uint8 GetPowerCount() const;

        void OnEquip();
        void OnUnequip();

        void UpdateRelicTalents(bool p_Apply);

        std::unordered_map<uint32, ArtifactPowerRanks>::iterator GetPowerReference(uint32 p_Power);
        std::unordered_map<uint32, ArtifactPowerRanks>::const_iterator GetPowerReference(uint32 p_Power) const;
        bool HasPower(uint32 p_ID) const;

        bool AddPower(uint32 p_ID, uint8 p_RankWithoutRelics, uint8 p_BonusRank = 0, bool p_IgnoreChecks = false, bool p_IgnoreCost = false);
        bool UpgradePower(uint32 p_ID, bool p_IgnoreCost = false, bool p_ByBonus = false);
        void LearnOrUpdateSpellSpell(ArtifactPowerRankEntry const* p_RankEntry) const;
        void LearnOrUpdateSpellSpell(uint32 p_ArtifactPowerId);

        uint8 GetPurchasedRank(uint32 p_ID);
        uint8 GetCurrentRankWithBonus(uint32 p_ID);
        void AddOrUpgradeTrait(uint32 p_ID, bool p_IgnoreCost = false, bool p_ByBonus = false);
        void RemoveOrDowngradeTrait(uint32 p_ID, bool p_ForceRemove = false);
        bool ChangeAppearance(uint32 p_ArtifactAppearanceID, bool p_IgnoreConditionCheck = false);

        void UpdatePowerBonusRankByType(uint32 p_RelicType, uint16 p_Bonus, bool p_Apply);
        void UpdatePowerBonusRankById(uint32 p_Id, uint16 p_Bonus, bool p_Apply);

        void ModifyArtifactPower(int64 p_Value, uint32 p_ArtifactCategoryId = 0, uint32 p_ArtifactKnowledgeLevel = 0);
        void SetArtifactPower(uint64 p_Value);
        uint64 GetArtifactPower() const;

        uint32 GetArtifactId() const;

        Item* ToItem() { return m_Item; }
        ItemTemplate const* GetItemTemplate();

        uint8 GetLevel() const;
        uint64 GetCostForNextRank() const;
        uint64 GetCostForRank(uint8 p_Rank, bool p_ForceTier2 = false) const;

        void DumpPowerFields();

        void UpdateAppearance();
        ArtifactAppearanceEntry const* GenerateDefaultAppearance();
        uint32 GetAppearanceID() const { return m_AppearanceID; };
        Player* GetOwner() const { return m_Owner; }
        std::unordered_map<uint32, ArtifactPowerRanks> const& GetPowers() const { return m_Powers; }

        void ResetPowers();
        void _LearnSpells();

        /// Power Id of the basic Spell of the Artifact
        static bool IsArtifactPrimarySpellPowerId(uint32 p_ArtifactPowerId);

        /// Bonus Damage or Stamina per trait purchased (0.75%) Power Id
        static bool IsArtifactPassiveDamageOrStaminaPerTraitPowerId(uint32 p_ArtifactPowerId);

        /// Non purchased trait Power Id (either basic spell of artifact, or passive damage/stamina per trait)
        static bool IsArtifactNonPurshaseableTraitPowerId(uint32 p_ArtifactPowerId);

        void SetArtifactTier(uint8 p_Tier) { m_Tier = p_Tier; }

    private:
        uint32 _CalculateMaxLevel();
        void _CopyPowersIntoDynamicFields();

        Item* m_Item;
        ArtifactEntry const* m_Entry;
        uint32 m_AppearanceID;
        uint64 m_ArtifactPower;
        uint32 m_MaxLevel;
        uint8 m_Tier;
        std::unordered_map<uint32, ArtifactPowerRanks> m_Powers;
        Player* m_Owner;

        void _UnlearnSpells();
    };
}   ///< namespace Artifact
}   ///< namespace MS
