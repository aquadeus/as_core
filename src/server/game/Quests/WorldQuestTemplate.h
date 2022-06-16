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
#include <unordered_map>

namespace MS { namespace WorldQuest
{
    enum WorldQuestType : uint32
    {
        Blacksmithing           = 116,
        Leatherworking          = 117,
        Alchemy                 = 118,
        Herbalism               = 119,
        Mining                  = 120,
        Tailoring               = 121,
        Engineering             = 122,
        Enchanting              = 123,
        Skinning                = 124,
        Jewelcrafting           = 125,
        Inscription             = 126,

        Fishing                 = 130,
        Cooking                 = 131,
        BattlePet               = 115,

        RegularWorldQuest       = 109,
        RareWorldQuest          = 135,
        RareEliteWorldQuest     = 136,
        EpicEliteWorldQuest     = 112, /// World Bosses & Nighthold quests (atm)
        EliteWorldQuest         = 111, /// Only 1 ????
        DailyPvP                = 113,
        Dungeon                 = 137,

        Bounty                  = 128, ///< Unused in the client, used internally

        LegionInvasionWQ        = 139,
        LegionInvasionEliteWQ   = 142,
        LegionDungeonWQ         = 145,
        LegionInvasionWrapper   = 146
    };

    namespace Factions
    {
        enum : uint16
        {
            KirimTor            = 1090,
            HighmountainTribes  = 1828,
            TheNighfallen       = 1859,
            Dreamwalkers        = 1883,
            Wardens             = 1894,
            CourtOfFadoris      = 1900,
            Valarjar            = 1948,
            ArmiesOfLegionfall  = 2045,
            ArmyOfTheLight      = 2165,
            ArgussianReach      = 2170
        };
    }

    namespace Globals
    {
        enum class LoadType
        {
            Money       = 0,
            Item        = 1,
            Currency    = 2
        };
    }

    namespace WorldQuestLootFlags
    {
        enum : uint32
        {
            NoBonuses   = 1,
        };
    }

    constexpr uint32 g_WorldQuestFactions[] { Factions::KirimTor, Factions::HighmountainTribes, Factions::TheNighfallen, Factions::Dreamwalkers, Factions::Wardens, Factions::CourtOfFadoris, Factions::Valarjar};
    constexpr uint32 g_WorldQuestPrimaryMapFactions[] { Factions::HighmountainTribes, Factions::TheNighfallen, Factions::Dreamwalkers, Factions::CourtOfFadoris, Factions::Valarjar };

    class Template;

    using WorldQuestMap = std::unordered_map<uint32, Template const*>;
    using WorldQuestList = std::vector<Template const*>;
    using WorldQuestsByTypeList = std::unordered_map<uint32, WorldQuestList>;
    using WorldQuestsByFactionList = std::unordered_map<uint32, WorldQuestList>;

    struct WorldQuestLootGroupEntry
    {
        Globals::LoadType m_Type;
        uint32 m_ItemID;
        uint32 m_CountMin;
        uint32 m_CountMax;
        uint32 m_Flags;
    };

    struct WorldQuestLoot
    {
        uint32 l_GroupID;
        uint32 l_Chance;
    };

    using WorldQuestLootListByClass = std::vector<WorldQuestLoot>;
    using WorldQuestLootMap = std::unordered_map<uint32, WorldQuestLootListByClass>;
    using WorldQuestLootGroupList = std::array<std::vector<WorldQuestLootGroupEntry>, Classes::MAX_CLASSES + 1>;
    using WorldQuestLootGroupMap = std::unordered_map<uint32, WorldQuestLootGroupList>;

    using WorldQuestAreaGroupMap = std::map<uint32, uint32>;

    class Template
    {
    public:
        explicit Template(uint32 p_ID);
        Template(Template const&) = delete;
        void operator=(Template const&) = delete;

        bool Load(Field* p_Fields);

        static Template const* GetTemplate(uint32 p_ID);
        static WorldQuestMap const& GetTemplates();
        static WorldQuestList const* GetTemplatesByWorldQuestType(WorldQuestType p_Type);
        static WorldQuestAreaGroupMap const& GetAreaGroupMap();
        static WorldQuestList const& GetTemplatesBySecondaryFaction(uint32 p_Faction);

        static void LoadAllWorldQuests();
        static void LoadLootTemplates();

        WorldQuestLootGroupEntry const* GenerateLoot(uint8 p_PlayerClass) const;
        uint32 GenerateDuration() const;

        uint32 GetID() const { return m_ID; }
        uint32 GetWorldStateID() const { return m_WorldStateID; }
        uint32 GetWorldStateValue() const { return m_WorldStateValue; }
        uint32 GetPlayerConditionID() const { return m_PlayerConditionID; }
        uint32 GetSkillIDRequired() const { return m_SkillIDRequired; }
        uint32 GetRequiredLevel() const { return m_RequiredLevel; }
        WorldQuestType GetWorldQuestType() const { return m_WorldQuestType; }
        uint32 GetPrimaryFaction() const { return m_FactionIDs[0]; }
        uint32 GetSecondaryFaction() const { return m_FactionIDs[1]; }
        Quest const* GetQuestTemplate() const { return m_QuestTemplate; }
        int32 GetAreaID() const { return m_AreaID; }
        bool IsEnabled() const { return m_Enabled; }

        void OverrideQuestTemplate(Quest const* p_Quest) { m_QuestTemplate = p_Quest; }

        void GenerateAvailableApTokenIDs();
        void GetAvailableApTokenIDs(std::vector<uint32>& p_TokenIds) const;

    private:
        uint32 m_ID;
        uint32 m_WorldStateID;
        uint32 m_WorldStateValue;
        uint32 m_PlayerConditionID;
        uint32 m_SkillIDRequired;
        uint32 m_RequiredLevel;
        WorldQuestType m_WorldQuestType;
        uint32 m_FactionIDs[2];
        uint32 m_AreaID;
        Quest const* m_QuestTemplate;

        std::vector<uint32> m_AvailableApTokens;

        bool m_Enabled;
    };

    namespace Internal
    {
        struct Holder
        {
            WorldQuestMap m_WorldQuestMap;
            WorldQuestsByTypeList m_WorldQuestsByTypeList;
            WorldQuestsByFactionList m_WorldQuestsByFactionList;
            WorldQuestsByFactionList m_WorldQuestsBySecondaryFactionList;
            WorldQuestLootMap m_WorldQuestLootMap;
            WorldQuestLootGroupMap m_WorldQuestLootGroupMap;
            WorldQuestAreaGroupMap m_WorldQuestAreaGroupMap;
            ~Holder();
        };
    }
}}