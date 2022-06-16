////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "SharedDefines.h"

namespace MS { namespace Garrison { namespace GBrokenIsles
{
    namespace Globals
    {
        enum
        {
            DefaultSiteLevelID          = 560,
            DefaultFollowerItemLevel    = 760,
            ClassHallAreaGroup          = 4903,
            CommandBarSpell             = 213170,
            FollowerSupportMapID        = 1220
        };
    }

    namespace FollowerConstants
    {
        enum
        {
            SlotA_AbilityID = 414,
            SlotB_AbilityID = 415,
            SlotC_AbilityID = 855
        };
    }

    namespace WorkOrderType
    {
        enum Type
        {
            ArtifactResearch,
            TroopsRecruitment,
            SealOfBrokenFate,
            WorldQuestNuke,
            Armament,
            Cooking,

            Max
        };
    }

    namespace WorkOrderStates
    {
        enum Type
        {
            InProgress,
            AtLeastOneCompleted,
            AllCompleted_LimitNotReached,
            AllCompleted_LimitReached
        };
    }

    struct WorkOrderInfo
    {
        uint32 WorkOrderID;
        WorkOrderType::Type Type;
        uint8 Class;
        uint32 RequiredGarrTalent;
        uint32 GameObjectEntry;
    };

    /// 9000000 + (WorkOrderID << 1) + State
    #define SHIPMENT_PHASE_ID(WorkOrderID, State)        (9000000 + (WorkOrderID << 8) + State)
    #define SHIPMENT_PHASE_ID_GET_WORK_ORDER_ID(PhaseID) ((PhaseID - 9000000) >> 8)
    #define SHIPMENT_PHASE_ID_GET_STATE(PhaseID)         ((PhaseID - 9000000) & 0xF)

    #define WORKORDER_PHASE_ID(TalentId)                 (9000000 + TalentId)

    enum eWorkOrderNPC
    {
        /// Artifact Knowledge NPCs
        NPCWarriorAK            = 111741,
        NPCPaladinAK            = 91190,
        NPCHunterAK             = 98737,
        NPCRogueAK              = 102641,
        NPCPriestAK             = 111738,
        NPCDeathKnightAK        = 97111,
        NPCShamanAK             = 111739,
        NPCMageAK               = 110624,
        NPCWarlockAK            = 111740,
        NPCMonkAK               = 106942,
        NPCDruidAK              = 111737,
        NPCDemonHunterAK        = 111736,

        /// Troops NPCs
        NPCWarriorSavyn         = 106460,
        NPCWarriorHjalmar       = 106459,

        NPCPaladinAnsela        = 106447,
        NPCPaladinBorn          = 106448,

        NPCHunterLenara         = 106444,
        NPCHunterSampson        = 106446,

        NPCRogueLonika          = 105979,
        NPCRogueYancey          = 106083,

        NPCPriestGesslar        = 106450,
        NPCPriestEliza          = 106451,

        NPCDeathKnightMarogh    = 106435,
        NPCDeathKnightKorgaz    = 106436,

        NPCShamanMorn           = 106457,
        NPCShamanFelinda        = 112208,

        NPCMageOmniara          = 106377,
        NPCMageMimic            = 106433,

        NPCWarlockJared         = 106217,
        NPCWarlockDyala         = 106216,

        NPCMonkTianji           = 105015,
        NPCMonkGinLai           = 105019,

        NPCDruidLilith          = 108393,
        NPCDruidYaris           = 106442,

        NPCDemonHunterGaardoun  = 103025,
        NPCDemonHunterAriana    = 103760,

        /// NPCs Seal Of Broken Fate
        NPCPaladinSoBF          = 112986,
        NPCHunterSoBF           = 110816,
        NPCRogueSoBF            = 110820,
        NPCPriestSoBF           = 110819,
        NPCShamanSoBF           = 112299,
        NPCMageSoBF             = 112982,
        NPCMonkSoBF             = 110817,
        NPCDruidSoBF            = 110810,
        NPCDemonHunterSoBF      = 112992,

        /// NPCs Nuke World Quest
        NPCDeathKnightWQNuke    = 111634,
        NPCWarlockWQNuke        = 113371,
        NPCMageWQNuke           = 111734,
        NPCPaladinWQNuke        = 111772,
        NPCWarriorWQNuke        = 111774,
        NPCDemonHunterWQNuke    = 111775,

        /// NPCs Armor
        NPCWarriorArmament      = 110437,
        NPCPaladinArmament      = 110434,
        NPCHunterArmament       = 110412,
        NPCRogueArmament        = 110348,
        NPCPriestArmament       = 110595,
        NPCDeathKnightArmament  = 110410,
        NPCMageArmament         = 110427,
        NPCWarlockArmament      = 110408,
        NPCDemonHunterArmament  = 110433,

        /// NPCs Cooking
        NPCNomi                 = 101846
    };

    enum eWorkorderComponenent
    {
        LeanShank           = 124117,
        WildfowlEgg         = 124121,
        BigGamyRibs         = 124119,
        FattyBearsteak      = 124118,
        Leyblood            = 124120,
        CursedQueenfish     = 124107,
        MossgillPerch       = 124108,
        HighmountainSalmon  = 124109,
        Stormray            = 124110,
        RunescaleKoi        = 124111,
        BlackBarracuda      = 124112,
        Bacon               = 133680,
        SilverMackerelQuest = 133912,
        SilverMackerel      = 133607,

        Count               = 14
    };

    enum eWorkorderMetaRecipe
    {
        RecipesLeanShank           = 133877,
        RecipesWildfowlEgg         = 133886,
        RecipesBigGamyRibs         = 133915,
        RecipesFattyBearsteak      = 133914,
        RecipesLeyblood            = 133916,
        RecipesCursedQueenfish     = 133917,
        RecipesMossgillPerch       = 133918,
        RecipesHighmountainSalmon  = 133919,
        RecipesStormray            = 133920,
        RecipesRunescaleKoi        = 133921,
        RecipesBlackBarracuda      = 133922,
        RecipesBacon               = 133923,
        RecipesSilverMackerel      = 141700,
    };

    enum eWorkorderRecipe
    {
        AzshariSaladR1              = 133822,
        AzshariSaladR2              = 133842,
        AzshariSaladR3              = 133862,
        SuramarSurfAndTurfR2        = 133837,
        SuramarSurfAndTurfR3        = 133857,
        SaltAndPepperShankR2        = 133831,
        SaltAndPepperShankR3        = 133851,
        HeartyFeast                 = 133869,
        NightborneDelicacyPlatterR1 = 133823,
        NightborneDelicacyPlatterR2 = 133843,
        NightborneDelicacyPlatterR3 = 133863,
        FaronaarFizzR2              = 133834,
        FaronaarFizzR3              = 133854,
        TheHungryMagisterR1         = 133821,
        TheHungryMagisterR2         = 133841,
        TheHungryMagisterR3         = 133861,
        BearTartareR2               = 133847,
        BearTartareR3               = 133867,
        LeybequeRibsR2              = 133836,
        LeybequeRibsR3              = 133856,
        BarracudaMrglgaghR2         = 133838,
        BarracudaMrglgaghR3         = 133858,
        LavishSuramarFeast          = 133850,
        FishbrulSpecialR1           = 133825,
        FishbrulSpecialR2           = 133845,
        FishbrulSpecialR3           = 133865,
        FighterChowR2               = 133848,
        FighterChowR3               = 133868,
        DeepFriedMossgillR2         = 133832,
        DeepFriedMossgillR3         = 133852,
        DrogbarStyleSalmonR2        = 133840,
        DrogbarStyleSalmonR3        = 133860,
        SeedBatteredFishPlateR1     = 133824,
        SeedBatteredFishPlateR2     = 133844,
        SeedBatteredFishPlateR3     = 133864,
        KoiScentedStormrayR2        = 133839,
        KoiScentedStormrayR3        = 133859,
        SpicedRibRoastR2            = 133835,
        SpicedRibRoastR3            = 133855,
        CrispyBaconR1               = 133871,
        CrispyBaconR2               = 133872,
        CrispyBaconR3               = 133873,
        SilverMackrelR1             = 133826,
        SilverMackrelR2             = 133846,
        SilverMackrelR3             = 133866
    };

    enum eCookingConstant
    {
        MaxWorkOrder        = 24,
        CookPlotInstanceID  = 99999999
    };

    static const std::array<std::pair<uint32, uint32>, eWorkorderComponenent::Count> s_CookingWorkorderComponenents =
    {{
        { eWorkorderComponenent::LeanShank          , 104684 },
        { eWorkorderComponenent::WildfowlEgg        , 104708 },
        { eWorkorderComponenent::BigGamyRibs        , 104709 },
        { eWorkorderComponenent::FattyBearsteak     , 104710 },
        { eWorkorderComponenent::Leyblood           , 104711 },
        { eWorkorderComponenent::CursedQueenfish    , 104712 },
        { eWorkorderComponenent::MossgillPerch      , 104713 },
        { eWorkorderComponenent::HighmountainSalmon , 104714 },
        { eWorkorderComponenent::Stormray           , 104715 },
        { eWorkorderComponenent::RunescaleKoi       , 104716 },
        { eWorkorderComponenent::BlackBarracuda     , 104717 },
        { eWorkorderComponenent::SilverMackerel     , 104858 },
        { eWorkorderComponenent::SilverMackerelQuest, 104858 },
        { eWorkorderComponenent::Bacon              , 104718 }
    }};

    static const std::set<uint32> s_CookingWorkordersLoots =
    {
        eWorkorderMetaRecipe::RecipesLeanShank,
        eWorkorderMetaRecipe::RecipesWildfowlEgg,
        eWorkorderMetaRecipe::RecipesBigGamyRibs,
        eWorkorderMetaRecipe::RecipesFattyBearsteak,
        eWorkorderMetaRecipe::RecipesLeyblood,
        eWorkorderMetaRecipe::RecipesCursedQueenfish,
        eWorkorderMetaRecipe::RecipesMossgillPerch,
        eWorkorderMetaRecipe::RecipesHighmountainSalmon,
        eWorkorderMetaRecipe::RecipesStormray,
        eWorkorderMetaRecipe::RecipesRunescaleKoi,
        eWorkorderMetaRecipe::RecipesBlackBarracuda,
        eWorkorderMetaRecipe::RecipesBacon,
        eWorkorderMetaRecipe::RecipesSilverMackerel,
    };

    static const std::set<uint32> s_CookingWorkorderRecipes =
    {
            eWorkorderRecipe::CrispyBaconR1,                eWorkorderRecipe::CrispyBaconR2,                eWorkorderRecipe::CrispyBaconR3,
            eWorkorderRecipe::AzshariSaladR1,               eWorkorderRecipe::AzshariSaladR2,               eWorkorderRecipe::AzshariSaladR3,
            eWorkorderRecipe::NightborneDelicacyPlatterR1,  eWorkorderRecipe::NightborneDelicacyPlatterR2,  eWorkorderRecipe::NightborneDelicacyPlatterR3,
            eWorkorderRecipe::TheHungryMagisterR1,          eWorkorderRecipe::TheHungryMagisterR2,          eWorkorderRecipe::TheHungryMagisterR3,
            eWorkorderRecipe::FishbrulSpecialR1,            eWorkorderRecipe::FishbrulSpecialR2,            eWorkorderRecipe::FishbrulSpecialR3,
            eWorkorderRecipe::SeedBatteredFishPlateR1,      eWorkorderRecipe::SeedBatteredFishPlateR2,      eWorkorderRecipe::SeedBatteredFishPlateR3,
            eWorkorderRecipe::SilverMackrelR1,              eWorkorderRecipe::SilverMackrelR2,              eWorkorderRecipe::SilverMackrelR3,
            eWorkorderRecipe::SuramarSurfAndTurfR2,         eWorkorderRecipe::SuramarSurfAndTurfR3,
            eWorkorderRecipe::SaltAndPepperShankR2,         eWorkorderRecipe::SaltAndPepperShankR3,
            eWorkorderRecipe::FaronaarFizzR2,               eWorkorderRecipe::FaronaarFizzR3,
            eWorkorderRecipe::BearTartareR2,                eWorkorderRecipe::BearTartareR3,
            eWorkorderRecipe::LeybequeRibsR2,               eWorkorderRecipe::LeybequeRibsR3,
            eWorkorderRecipe::BarracudaMrglgaghR2,          eWorkorderRecipe::BarracudaMrglgaghR3,
            eWorkorderRecipe::FighterChowR2,                eWorkorderRecipe::FighterChowR3,
            eWorkorderRecipe::DeepFriedMossgillR2,          eWorkorderRecipe::DeepFriedMossgillR3,
            eWorkorderRecipe::DrogbarStyleSalmonR2,         eWorkorderRecipe::DrogbarStyleSalmonR3,
            eWorkorderRecipe::KoiScentedStormrayR2,         eWorkorderRecipe::KoiScentedStormrayR3,
            eWorkorderRecipe::SpicedRibRoastR2,             eWorkorderRecipe::SpicedRibRoastR3,
            eWorkorderRecipe::LavishSuramarFeast,
            eWorkorderRecipe::HeartyFeast
    };

    static const std::multimap<uint32, WorkOrderInfo> s_WorkOrderInfos[MAX_CLASSES] =
    {
        /// CLASS_NONE
        {                                                                                                        /// These Ids are USELESS NOT EVEN USED PUT WHATEVER YOU WANT
            { eWorkorderComponenent::LeanShank,             { 235, WorkOrderType::Cooking,          CLASS_NONE,            0, 202349 }},    ///< Lean Shank Recipes
            { eWorkorderComponenent::WildfowlEgg,           { 236, WorkOrderType::Cooking,          CLASS_NONE,            0, 202395 }},    ///< Wildfowl Egg Recipes
            { eWorkorderComponenent::BigGamyRibs,           { 237, WorkOrderType::Cooking,          CLASS_NONE,            0, 202396 }},    ///< Big Gamy Ribs Recipes
            { eWorkorderComponenent::FattyBearsteak,        { 238, WorkOrderType::Cooking,          CLASS_NONE,            0, 202397 }},    ///< Fatty Bearsteak Recipes
            { eWorkorderComponenent::Leyblood,              { 239, WorkOrderType::Cooking,          CLASS_NONE,            0, 202398 }},    ///< Leyblood Recipes
            { eWorkorderComponenent::CursedQueenfish,       { 240, WorkOrderType::Cooking,          CLASS_NONE,            0, 202399 }},    ///< Cursed Queenfish Recipes
            { eWorkorderComponenent::MossgillPerch,         { 241, WorkOrderType::Cooking,          CLASS_NONE,            0, 202400 }},    ///< Mossgill Perch Recipes
            { eWorkorderComponenent::HighmountainSalmon,    { 242, WorkOrderType::Cooking,          CLASS_NONE,            0, 202401 }},    ///< Highmountain Salmon Recipes
            { eWorkorderComponenent::Stormray,              { 243, WorkOrderType::Cooking,          CLASS_NONE,            0, 202402 }},    ///< Stormray Recipes
            { eWorkorderComponenent::RunescaleKoi,          { 244, WorkOrderType::Cooking,          CLASS_NONE,            0, 202403 }},    ///< Runescale Koi Recipes
            { eWorkorderComponenent::BlackBarracuda,        { 245, WorkOrderType::Cooking,          CLASS_NONE,            0, 202404 }},    ///< Black Barracuda Recipes
            { eWorkorderComponenent::Bacon,                 { 246, WorkOrderType::Cooking,          CLASS_NONE,            0, 202405 }},    ///< Bacon Recipes
            { eWorkorderComponenent::SilverMackerelQuest,   { 247, WorkOrderType::Cooking,          CLASS_NONE,            0, 202406 }},    ///< Silver Mackerel Quest
            { eWorkorderComponenent::SilverMackerel,        { 401, WorkOrderType::Cooking,          CLASS_NONE,            0, 202406 }},    ///< Silver Mackerel
        },
        /// CLASS_WARRIOR
        {
            { eWorkOrderNPC::NPCWarriorAK,              { 353, WorkOrderType::ArtifactResearch, CLASS_WARRIOR,         0, 252801 }},    ///< Artifact research - Warrior

            { eWorkOrderNPC::NPCWarriorSavyn,           { 285, WorkOrderType::TroopsRecruitment, CLASS_WARRIOR,        0, 250879 }},    ///< Shieldmaiden Warband
            { eWorkOrderNPC::NPCWarriorSavyn,           { 324, WorkOrderType::TroopsRecruitment, CLASS_WARRIOR,        0, 250879 }},    ///< Shieldmaiden Warband - Quest
            { eWorkOrderNPC::NPCWarriorSavyn,           { 310, WorkOrderType::TroopsRecruitment, CLASS_WARRIOR,      407, 250879 }},    ///< Valkyra Shieldmaidens
            { eWorkOrderNPC::NPCWarriorHjalmar,         { 284, WorkOrderType::TroopsRecruitment, CLASS_WARRIOR,        0, 250878 }},    ///< Valarjar Aspirants
            { eWorkOrderNPC::NPCWarriorHjalmar,         { 309, WorkOrderType::TroopsRecruitment, CLASS_WARRIOR,      406, 250878 }},    ///< Stormforged Valarjar

            { eWorkOrderNPC::NPCWarriorWQNuke,          { 356, WorkOrderType::WorldQuestNuke,  CLASS_WARRIOR,          0, 252887 }},    ///< World Quest Nuke - Warrior

            { eWorkOrderNPC::NPCWarriorArmament,        { 365, WorkOrderType::Armament,  CLASS_WARRIOR,                0, 252016 }}     ///< Armament - Warrior
        },
        /// CLASS_PALADIN
        {
            { eWorkOrderNPC::NPCPaladinAK,              { 348, WorkOrderType::ArtifactResearch, CLASS_PALADIN,         0, 252796 }},    ///< Artifact research - Paladin

            { eWorkOrderNPC::NPCPaladinAnsela,          { 272, WorkOrderType::TroopsRecruitment, CLASS_PALADIN,        0, 250906 }},    ///< Squad of Squires
            { eWorkOrderNPC::NPCPaladinAnsela,          { 326, WorkOrderType::TroopsRecruitment, CLASS_PALADIN,        0, 250906 }},    ///< Squad of Squires - Quest
            { eWorkOrderNPC::NPCPaladinAnsela,          { 307, WorkOrderType::TroopsRecruitment, CLASS_PALADIN,      395, 250906 }},    ///< Shieldbearer Phalanx
            { eWorkOrderNPC::NPCPaladinBorn,            { 273, WorkOrderType::TroopsRecruitment, CLASS_PALADIN,        0, 250907 }},    ///< Silver Hand Knights
            { eWorkOrderNPC::NPCPaladinBorn,            { 308, WorkOrderType::TroopsRecruitment, CLASS_PALADIN,      396, 250907 }},    ///< Silver Hand Templar

            { eWorkOrderNPC::NPCPaladinSoBF,            { 381, WorkOrderType::SealOfBrokenFate, CLASS_PALADIN,         0, 254233 }},    ///< Seal of Broken Fate - Paladin

            { eWorkOrderNPC::NPCPaladinWQNuke,          { 354, WorkOrderType::WorldQuestNuke,  CLASS_PALADIN,          0, 252885 }},    ///< World Quest Nuke - Paladin

            { eWorkOrderNPC::NPCPaladinArmament,        { 369, WorkOrderType::Armament,  CLASS_PALADIN,                0, 252015 }}     ///< Armament - Paladin
        },
        /// CLASS_HUNTER
        {
            { eWorkOrderNPC::NPCHunterAK,               { 346, WorkOrderType::ArtifactResearch, CLASS_HUNTER,          0, 252794 }},    ///< Artifact research - Hunter

            { eWorkOrderNPC::NPCHunterLenara,           { 269, WorkOrderType::TroopsRecruitment, CLASS_HUNTER,         0, 250894 }},    ///< Squad of Archers
            { eWorkOrderNPC::NPCHunterLenara,           { 328, WorkOrderType::TroopsRecruitment, CLASS_HUNTER,         0, 250894 }},    ///< Squad of Archers - Quest
            { eWorkOrderNPC::NPCHunterLenara,           { 301, WorkOrderType::TroopsRecruitment, CLASS_HUNTER,       373, 250894 }},    ///< Unseen Marksmen
            { eWorkOrderNPC::NPCHunterSampson,          { 270, WorkOrderType::TroopsRecruitment, CLASS_HUNTER,         0, 250895 }},    ///< Band of Trackers
            { eWorkOrderNPC::NPCHunterSampson,          { 302, WorkOrderType::TroopsRecruitment, CLASS_HUNTER,       374, 250895 }},    ///< Pathfinders

            { eWorkOrderNPC::NPCHunterSoBF,             { 376, WorkOrderType::SealOfBrokenFate, CLASS_HUNTER,          0, 252185 }},    ///< Seal of Broken Fate - Hunter

            { eWorkOrderNPC::NPCHunterArmament,         { 364, WorkOrderType::Armament,  CLASS_HUNTER,                 0, 252005 }}     ///< Armament - Hunter
        },
        /// CLASS_ROGUE
        {
            { eWorkOrderNPC::NPCRogueAK,                { 350, WorkOrderType::ArtifactResearch, CLASS_ROGUE,           0, 252798 }},    ///< Artifact research - Rogue

            { eWorkOrderNPC::NPCRogueLonika,            { 278, WorkOrderType::TroopsRecruitment, CLASS_ROGUE,          0, 250897 }},    ///< Gang of Bandits
            { eWorkOrderNPC::NPCRogueLonika,            { 325, WorkOrderType::TroopsRecruitment, CLASS_ROGUE,          0, 250897 }},    ///< Gang of Bandits - Quest
            { eWorkOrderNPC::NPCRogueLonika,            { 317, WorkOrderType::TroopsRecruitment, CLASS_ROGUE,        439, 250897 }},    ///< Defias Thieves
            { eWorkOrderNPC::NPCRogueYancey,            { 279, WorkOrderType::TroopsRecruitment, CLASS_ROGUE,          0, 250898 }},    ///< Crew of Pirates
            { eWorkOrderNPC::NPCRogueYancey,            { 318, WorkOrderType::TroopsRecruitment, CLASS_ROGUE,        440, 250898 }},    ///< Uncrowned Duelists

            { eWorkOrderNPC::NPCRogueSoBF,              { 372, WorkOrderType::SealOfBrokenFate, CLASS_ROGUE,           0, 252188 }},    ///< Seal of Broken Fate - Rogue

            { eWorkOrderNPC::NPCRogueArmament,          { 360, WorkOrderType::Armament,  CLASS_ROGUE,                  0, 251966 }}     ///< Armament - Rogue
        },
        /// CLASS_PRIEST
        {
            { eWorkOrderNPC::NPCPriestAK,               { 349, WorkOrderType::ArtifactResearch, CLASS_PRIEST,          0, 252797 }},    ///< Artifact research - Priest

            { eWorkOrderNPC::NPCPriestGesslar,          { 275, WorkOrderType::TroopsRecruitment, CLASS_PRIEST,         0, 250909 }},    ///< Group of Acolytes
            { eWorkOrderNPC::NPCPriestGesslar,          { 333, WorkOrderType::TroopsRecruitment, CLASS_PRIEST,         0, 250909 }},    ///< Group of Acolytes - Quest
            { eWorkOrderNPC::NPCPriestGesslar,          { 332, WorkOrderType::TroopsRecruitment, CLASS_PRIEST,       450, 250909 }},    ///< Netherlight Paragons
            { eWorkOrderNPC::NPCPriestEliza,            { 276, WorkOrderType::TroopsRecruitment, CLASS_PRIEST,         0, 250910 }},    ///< Band of Zealots
            { eWorkOrderNPC::NPCPriestEliza,            { 334, WorkOrderType::TroopsRecruitment, CLASS_PRIEST,       451, 250910 }},    ///< Dark Zealots

            { eWorkOrderNPC::NPCPriestSoBF,             { 382, WorkOrderType::SealOfBrokenFate, CLASS_PRIEST,          0, 252187 }},    ///< Seal of Broken Fate - Priest

            { eWorkOrderNPC::NPCPriestArmament,         { 370, WorkOrderType::Armament,  CLASS_PRIEST,                 0, 252058 }}     ///< Armament - Priest
        },
        /// CLASS_DEATH_KNIGHT
        {
            { eWorkOrderNPC::NPCDeathKnightAK,          { 343, WorkOrderType::ArtifactResearch, CLASS_DEATH_KNIGHT,    0, 252791 }},    ///< Artifact research - Death Knight

            { eWorkOrderNPC::NPCDeathKnightMarogh,      { 260, WorkOrderType::TroopsRecruitment, CLASS_DEATH_KNIGHT,   0, 250888 }},    ///< Pack of Ghouls
            { eWorkOrderNPC::NPCDeathKnightMarogh,      { 331, WorkOrderType::TroopsRecruitment, CLASS_DEATH_KNIGHT,   0, 250888 }},    ///< Pack of Ghouls - Quest
            { eWorkOrderNPC::NPCDeathKnightMarogh,      { 315, WorkOrderType::TroopsRecruitment, CLASS_DEATH_KNIGHT, 428, 250888 }},    ///< Geist Swarm
            { eWorkOrderNPC::NPCDeathKnightKorgaz,      { 261, WorkOrderType::TroopsRecruitment, CLASS_DEATH_KNIGHT,   0, 250889 }},    ///< Ebon Knights
            { eWorkOrderNPC::NPCDeathKnightKorgaz,      { 316, WorkOrderType::TroopsRecruitment, CLASS_DEATH_KNIGHT, 429, 250889 }},    ///< Ebon Ravagers

            { eWorkOrderNPC::NPCDeathKnightWQNuke,      { 340, WorkOrderType::WorldQuestNuke,  CLASS_DEATH_KNIGHT,     0, 252771 }},    ///< World Quest Nuke - Death Knight

            { eWorkOrderNPC::NPCDeathKnightArmament,    { 371, WorkOrderType::Armament,  CLASS_DEATH_KNIGHT,           0, 251996 }}     ///< Armament - Death Knight
        },
        /// CLASS_SHAMAN
        {
            { eWorkOrderNPC::NPCShamanAK,               { 351, WorkOrderType::ArtifactResearch, CLASS_SHAMAN,          0, 252799 }},    ///< Artifact research - Shaman

            { eWorkOrderNPC::NPCShamanMorn,             { 282, WorkOrderType::TroopsRecruitment, CLASS_SHAMAN,         0, 250281 }},    ///< Lesser Elementals
            { eWorkOrderNPC::NPCShamanMorn,             { 322, WorkOrderType::TroopsRecruitment, CLASS_SHAMAN,         0, 250281 }},    ///< Lesser Elementals - Quest
            { eWorkOrderNPC::NPCShamanMorn,             { 297, WorkOrderType::TroopsRecruitment, CLASS_SHAMAN,       44, 250281 }},    ///< Greater Elementals
            { eWorkOrderNPC::NPCShamanFelinda,          { 281, WorkOrderType::TroopsRecruitment, CLASS_SHAMAN,         0, 250280 }},    ///< Circle of Earthcallers
            { eWorkOrderNPC::NPCShamanFelinda,          { 298, WorkOrderType::TroopsRecruitment, CLASS_SHAMAN,       44, 250280 }},    ///< Earthen Ring Geomancers

            { eWorkOrderNPC::NPCShamanSoBF,             { 374, WorkOrderType::SealOfBrokenFate, CLASS_SHAMAN,          0, 252189 }}     ///< Seal of Broken Fate - Shaman
        },
        /// CLASS_MAGE
        {
            { eWorkOrderNPC::NPCMageAK,                 { 335, WorkOrderType::ArtifactResearch, CLASS_MAGE,            0, 252068 }},    ///< Artifact research - Mage

            { eWorkOrderNPC::NPCMageOmniara,            { 257, WorkOrderType::TroopsRecruitment, CLASS_MAGE,           0, 250900 }},    ///< Water Elemental
            { eWorkOrderNPC::NPCMageOmniara,            { 327, WorkOrderType::TroopsRecruitment, CLASS_MAGE,           0, 250900 }},    ///< Water Elemental - Quest
            { eWorkOrderNPC::NPCMageOmniara,            { 303, WorkOrderType::TroopsRecruitment, CLASS_MAGE,         384, 250900 }},    ///< Arcane Golems
            { eWorkOrderNPC::NPCMageMimic,              { 258, WorkOrderType::TroopsRecruitment, CLASS_MAGE,           0, 250901 }},    ///< Tirisgarde Apprentices
            { eWorkOrderNPC::NPCMageMimic,              { 304, WorkOrderType::TroopsRecruitment, CLASS_MAGE,         385, 250901 }},    ///< Kirin Tor Invokers

            { eWorkOrderNPC::NPCMageSoBF,               { 375, WorkOrderType::SealOfBrokenFate, CLASS_MAGE,            0, 254232 }},    ///< Seal of Broken Fate - Mage

            { eWorkOrderNPC::NPCMageWQNuke,             { 342, WorkOrderType::WorldQuestNuke,  CLASS_MAGE,             0, 252790 }},    ///< World Quest Nuke - Mage

            { eWorkOrderNPC::NPCMageArmament,           { 363, WorkOrderType::Armament,  CLASS_MAGE,                   0, 252013 }}     ///< Armament - Mage
        },
        /// CLASS_WARLOCK
        {
            { eWorkOrderNPC::NPCWarlockAK,              { 352, WorkOrderType::ArtifactResearch, CLASS_WARLOCK,         0, 252800 }},    ///< Artifact research - Warlock

            { eWorkOrderNPC::NPCWarlockJared,           { 254, WorkOrderType::TroopsRecruitment, CLASS_WARLOCK,        0, 250913 }},    ///< Black Harvest Acolytes
            { eWorkOrderNPC::NPCWarlockJared,           { 321, WorkOrderType::TroopsRecruitment, CLASS_WARLOCK,        0, 250913 }},    ///< Black Harvest Acolytes Quest
            { eWorkOrderNPC::NPCWarlockJared,           { 299, WorkOrderType::TroopsRecruitment, CLASS_WARLOCK,      362, 250913 }},    ///< Black Harvest Invokers
            { eWorkOrderNPC::NPCWarlockDyala,           { 255, WorkOrderType::TroopsRecruitment, CLASS_WARLOCK,        0, 250912 }},    ///< Pack of Imps
            { eWorkOrderNPC::NPCWarlockDyala,           { 300, WorkOrderType::TroopsRecruitment, CLASS_WARLOCK,      365, 250912 }},    ///< Swarm of Wild Imps

            { eWorkOrderNPC::NPCWarlockWQNuke,          { 341, WorkOrderType::WorldQuestNuke,  CLASS_WARLOCK,          0, 252778 }},    ///< World Quest Nuke - Warlock

            { eWorkOrderNPC::NPCWarlockArmament,        { 366, WorkOrderType::Armament,  CLASS_WARLOCK,               0, 251993 }}     ///< Armament - Warlock
        },
        /// CLASS_MONK
        {
            { eWorkOrderNPC::NPCMonkAK,                 { 347, WorkOrderType::ArtifactResearch, CLASS_MONK,            0, 252795 }},    ///< Artifact research - Monk

            { eWorkOrderNPC::NPCMonkTianji,             { 249, WorkOrderType::TroopsRecruitment, CLASS_MONK,           0, 248576 }},    ///< Ox Initiates
            { eWorkOrderNPC::NPCMonkTianji,             { 323, WorkOrderType::TroopsRecruitment, CLASS_MONK,           0, 248576 }},    ///< Ox Initiates - Quest
            { eWorkOrderNPC::NPCMonkTianji,             { 296, WorkOrderType::TroopsRecruitment, CLASS_MONK,         250, 248576 }},    ///< Ox Adepts
            { eWorkOrderNPC::NPCMonkGinLai,             { 250, WorkOrderType::TroopsRecruitment, CLASS_MONK,           0, 248556 }},    ///< Tiger Initates
            { eWorkOrderNPC::NPCMonkGinLai,             { 295, WorkOrderType::TroopsRecruitment, CLASS_MONK,         251, 248556 }},    ///< Tiger Adepts

            { eWorkOrderNPC::NPCMonkSoBF,               { 379, WorkOrderType::SealOfBrokenFate, CLASS_MONK,            0, 252186 }}     ///< Seal of Broken Fate - Monk
        },
        /// CLASS_DRUID
        {
            { eWorkOrderNPC::NPCDruidAK,                { 345, WorkOrderType::ArtifactResearch, CLASS_DRUID,           0, 252793 }},    ///< Artifact research - Druid

            { eWorkOrderNPC::NPCDruidLilith,            { 266, WorkOrderType::TroopsRecruitment, CLASS_DRUID,          0, 250872 }},    ///< Dreamgrove Treants
            { eWorkOrderNPC::NPCDruidLilith,            { 329, WorkOrderType::TroopsRecruitment, CLASS_DRUID,          0, 250872 }},    ///< Dreamgrove Treants - Quest
            { eWorkOrderNPC::NPCDruidLilith,            { 306, WorkOrderType::TroopsRecruitment, CLASS_DRUID,        351, 250872 }},    ///< Daughters of Cenarius
            { eWorkOrderNPC::NPCDruidYaris,             { 267, WorkOrderType::TroopsRecruitment, CLASS_DRUID,          0, 250886 }},    ///< Druids of the Claw
            { eWorkOrderNPC::NPCDruidYaris,             { 305, WorkOrderType::TroopsRecruitment, CLASS_DRUID,        352, 250886 }},    ///< Keepers of the Grove

            { eWorkOrderNPC::NPCDruidSoBF,              { 380, WorkOrderType::SealOfBrokenFate, CLASS_DRUID,           0, 252184 }}     ///< Seal of Broken Fate - Druid
        },
        /// CLASS_DEMON_HUNTER
        {
            { eWorkOrderNPC::NPCDemonHunterAK,          { 344, WorkOrderType::ArtifactResearch, CLASS_DEMON_HUNTER,    0, 252792 }},    ///< Artifact research - Demon hunter

            { eWorkOrderNPC::NPCDemonHunterGaardoun,    { 263, WorkOrderType::TroopsRecruitment, CLASS_DEMON_HUNTER,   0, 250891 }},    ///< Ashtongue Warriors
            { eWorkOrderNPC::NPCDemonHunterGaardoun,    { 330, WorkOrderType::TroopsRecruitment, CLASS_DEMON_HUNTER,   0, 250891 }},    ///< Ashtongue Warriors - Quest
            { eWorkOrderNPC::NPCDemonHunterGaardoun,    { 314, WorkOrderType::TroopsRecruitment, CLASS_DEMON_HUNTER, 417, 250891 }},    ///< Naga Myrmidons
            { eWorkOrderNPC::NPCDemonHunterAriana,      { 264, WorkOrderType::TroopsRecruitment, CLASS_DEMON_HUNTER,   0, 250892 }},    ///< Illidari Adepts
            { eWorkOrderNPC::NPCDemonHunterAriana,      { 313, WorkOrderType::TroopsRecruitment, CLASS_DEMON_HUNTER, 418, 250892 }},    ///< Transformed Illidari Adepts

            { eWorkOrderNPC::NPCDemonHunterSoBF,        { 373, WorkOrderType::SealOfBrokenFate, CLASS_DEMON_HUNTER,    0, 254235 }},    ///< Seal of Broken Fate - Demon hunter

            { eWorkOrderNPC::NPCDemonHunterWQNuke,      { 357, WorkOrderType::WorldQuestNuke,  CLASS_DEMON_HUNTER,     0, 252888 }},    ///< World Quest Nuke - Demon hunter

            { eWorkOrderNPC::NPCDemonHunterArmament,    { 361, WorkOrderType::Armament,  CLASS_DEMON_HUNTER,           0, 252014 }}     ///< Armament - Demon hunter
        },
    };

    static const std::array<uint32, MAX_CLASSES> s_ArmamentLootTemplates =
    {
       {
            0,       ///< CLASS_NONE
            123457,  ///< CLASS_WARRIOR
            123458,  ///< CLASS_PALADIN
            123459,  ///< CLASS_HUNTER
            123460,  ///< CLASS_ROGUE
            123461,  ///< CLASS_PRIEST
            123462,  ///< CLASS_DEATH_KNIGHT
            123463,  ///< CLASS_SHAMAN
            123464,  ///< CLASS_MAGE
            123465,  ///< CLASS_WARLOCK
            123466,  ///< CLASS_MONK
            123467,  ///< CLASS_DRUID
            123468   ///< CLASS_DEMON_HUNTER
        }
    };

    static const std::map<uint32, uint32> m_questStartShipments
    {  // quest  orderID
        { 41797, 321 },
        { 41946, 323 },
        { 42127, 327 },
        { 42142, 322 },
        { 42524, 328 },
        { 42585, 329 },
        { 42609, 324 },
        { 42679, 330 },
        { 42848, 326 },
        { 43013, 325 },
        { 43266, 331 },
        { 43275, 333 },
    };

}   ///< namespace BrokenIsles
}   ///< namespace Garrison
}   ///< namespace MS
