////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Item_Scripts
SD%Complete: 100
SDComment: Items for a range of different items. See content below (in script)
SDCategory: Items
EndScriptData */

/* ContentData
item_nether_wraith_beacon(i31742)   Summons creatures for quest Becoming a Spellfire Tailor (q10832)
item_flying_machine(i34060, i34061)  Engineering crafted flying machines
item_gor_dreks_ointment(i30175)     Protecting Our Own(q10488)
item_only_for_flight                Items which should only useable while flying
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Spell.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "Chat.h"
#include "ChallengeMgr.h"
#include "ScriptedGossip.h"
#include "HelperDefines.h"

/*#####
# item_only_for_flight
#####*/

enum eOnlyForFlight
{
    SPELL_ARCANE_CHARGES    = 45072
};

class item_only_for_flight : public ItemScript
{
public:
    item_only_for_flight() : ItemScript("item_only_for_flight") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
    {
        uint32 itemId = item->GetEntry();
        bool disabled = false;

        //for special scripts
        switch (itemId)
        {
           case 24538:
                if (player->GetAreaId() != 3628)
                    disabled = true;
                    break;
           case 34489:
                if (player->GetZoneId() != 4080)
                    disabled = true;
                    break;
           case 34475:
                if (const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(SPELL_ARCANE_CHARGES))
                    Spell::SendCastResult(player, spellInfo, Guid128(GUID_TYPE_CAST, 0, spellInfo->Id, 0), SPELL_FAILED_NOT_ON_GROUND);
                    break;
        }

        // allow use in flight only
        if (player->isInFlight() && !disabled)
            return false;

        // error
        player->SendEquipError(EQUIP_ERR_CLIENT_LOCKED_OUT, item, NULL);
        return true;
    }
};

/*#####
# item_nether_wraith_beacon
#####*/

class item_nether_wraith_beacon : public ItemScript
{
public:
    item_nether_wraith_beacon() : ItemScript("item_nether_wraith_beacon") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
    {
        if (player->GetQuestStatus(10832) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* nether = player->SummonCreature(22408, player->GetPositionX(), player->GetPositionY()+20, player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 180000))
                nether->AI()->AttackStart(player);

            if (Creature* nether = player->SummonCreature(22408, player->GetPositionX(), player->GetPositionY()-20, player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 180000))
                nether->AI()->AttackStart(player);
        }
        return false;
    }
};

/*#####
# item_gor_dreks_ointment
#####*/

class item_gor_dreks_ointment : public ItemScript
{
public:
    item_gor_dreks_ointment() : ItemScript("item_gor_dreks_ointment") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
    {
        if (targets.GetUnitTarget() && targets.GetUnitTarget()->GetTypeId() == TYPEID_UNIT &&
            targets.GetUnitTarget()->GetEntry() == 20748 && !targets.GetUnitTarget()->HasAura(32578))
            return false;

        player->SendEquipError(EQUIP_ERR_CLIENT_LOCKED_OUT, item, NULL);
        return true;
    }
};

/*#####
# item_incendiary_explosives
#####*/

class item_incendiary_explosives : public ItemScript
{
public:
    item_incendiary_explosives() : ItemScript("item_incendiary_explosives") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const & /*targets*/)
    {
        if (player->FindNearestCreature(26248, 15) || player->FindNearestCreature(26249, 15))
            return false;
        else
        {
            player->SendEquipError(EQUIP_ERR_OUT_OF_RANGE, item, NULL);
            return true;
        }
    }
};

/*#####
# item_mysterious_egg
#####*/

class item_mysterious_egg : public ItemScript
{
public:
    item_mysterious_egg() : ItemScript("item_mysterious_egg") { }

    bool OnExpire(Player* player, ItemTemplate const* /*pItemProto*/)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 39883, 1); // Cracked Egg
        if (msg == EQUIP_ERR_OK)
            player->StoreNewItem(dest, 39883, true, Item::GenerateItemRandomPropertyId(39883));

        return true;
    }
};

/*#####
# item_disgusting_jar
#####*/

class item_disgusting_jar : public ItemScript
{
public:
    item_disgusting_jar() : ItemScript("item_disgusting_jar") {}

    bool OnExpire(Player* player, ItemTemplate const* /*pItemProto*/)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 44718, 1); // Ripe Disgusting Jar
        if (msg == EQUIP_ERR_OK)
            player->StoreNewItem(dest, 44718, true, Item::GenerateItemRandomPropertyId(44718));

        return true;
    }
};

/*#####
# item_pile_fake_furs
#####*/

enum ePileFakeFur
{
    GO_CARIBOU_TRAP_1                                      = 187982,
    GO_CARIBOU_TRAP_2                                      = 187995,
    GO_CARIBOU_TRAP_3                                      = 187996,
    GO_CARIBOU_TRAP_4                                      = 187997,
    GO_CARIBOU_TRAP_5                                      = 187998,
    GO_CARIBOU_TRAP_6                                      = 187999,
    GO_CARIBOU_TRAP_7                                      = 188000,
    GO_CARIBOU_TRAP_8                                      = 188001,
    GO_CARIBOU_TRAP_9                                      = 188002,
    GO_CARIBOU_TRAP_10                                     = 188003,
    GO_CARIBOU_TRAP_11                                     = 188004,
    GO_CARIBOU_TRAP_12                                     = 188005,
    GO_CARIBOU_TRAP_13                                     = 188006,
    GO_CARIBOU_TRAP_14                                     = 188007,
    GO_CARIBOU_TRAP_15                                     = 188008,
    GO_HIGH_QUALITY_FUR                                    = 187983,
    NPC_NESINGWARY_TRAPPER                                 = 25835
};

#define CaribouTrapsNum 15
const uint32 CaribouTraps[CaribouTrapsNum] =
{
    GO_CARIBOU_TRAP_1, GO_CARIBOU_TRAP_2, GO_CARIBOU_TRAP_3, GO_CARIBOU_TRAP_4, GO_CARIBOU_TRAP_5,
    GO_CARIBOU_TRAP_6, GO_CARIBOU_TRAP_7, GO_CARIBOU_TRAP_8, GO_CARIBOU_TRAP_9, GO_CARIBOU_TRAP_10,
    GO_CARIBOU_TRAP_11, GO_CARIBOU_TRAP_12, GO_CARIBOU_TRAP_13, GO_CARIBOU_TRAP_14, GO_CARIBOU_TRAP_15
};

class item_pile_fake_furs : public ItemScript
{
public:
    item_pile_fake_furs() : ItemScript("item_pile_fake_furs") { }

    bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const & /*targets*/)
    {
        GameObject* go = NULL;
        for (uint8 i = 0; i < CaribouTrapsNum; ++i)
        {
            go = player->FindNearestGameObject(CaribouTraps[i], 5.0f);
            if (go)
                break;
        }

        if (!go)
            return false;

        if (go->FindNearestCreature(NPC_NESINGWARY_TRAPPER, 10.0f, true) || go->FindNearestCreature(NPC_NESINGWARY_TRAPPER, 10.0f, false) || go->FindNearestGameObject(GO_HIGH_QUALITY_FUR, 2.0f))
            return true;

        float x, y, z;
        go->GetClosePoint(x, y, z, go->GetObjectSize() / 3, 7.0f);
        go->SummonGameObject(GO_HIGH_QUALITY_FUR, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, 0, 0, 0, 0, 1000);
        if (TempSummon* summon = player->SummonCreature(NPC_NESINGWARY_TRAPPER, x, y, z, go->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 1000))
        {
            summon->SetVisible(false);
            summon->SetReactState(REACT_PASSIVE);
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        }
        return false;
    }
};

/*#####
# item_petrov_cluster_bombs
#####*/

enum ePetrovClusterBombs
{
    SPELL_PETROV_BOMB           = 42406,
    AREA_ID_SHATTERED_STRAITS   = 4064,
    ZONE_ID_HOWLING             = 495
};

class item_petrov_cluster_bombs : public ItemScript
{
public:
    item_petrov_cluster_bombs() : ItemScript("item_petrov_cluster_bombs") { }

    bool OnUse(Player* player, Item* /*item*/, const SpellCastTargets & /*targets*/)
    {
        if (player->GetZoneId() != ZONE_ID_HOWLING)
            return false;

        if (!player->GetTransport() || player->GetAreaId() != AREA_ID_SHATTERED_STRAITS)
        {
            if (const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(SPELL_PETROV_BOMB))
                Spell::SendCastResult(player, spellInfo, Guid128(GUID_TYPE_CAST, 0, spellInfo->Id, 0), SPELL_FAILED_NOT_HERE);

            return true;
        }

        return false;
    }
};

/*######
# item_dehta_trap_smasher
# For quest 11876, Help Those That Cannot Help Themselves
######*/
enum eHelpThemselves
{
    QUEST_CANNOT_HELP_THEMSELVES                  =  11876,
    NPC_TRAPPED_MAMMOTH_CALF                      =  25850,
    GO_MAMMOTH_TRAP_1                             = 188022,
    GO_MAMMOTH_TRAP_2                             = 188024,
    GO_MAMMOTH_TRAP_3                             = 188025,
    GO_MAMMOTH_TRAP_4                             = 188026,
    GO_MAMMOTH_TRAP_5                             = 188027,
    GO_MAMMOTH_TRAP_6                             = 188028,
    GO_MAMMOTH_TRAP_7                             = 188029,
    GO_MAMMOTH_TRAP_8                             = 188030,
    GO_MAMMOTH_TRAP_9                             = 188031,
    GO_MAMMOTH_TRAP_10                            = 188032,
    GO_MAMMOTH_TRAP_11                            = 188033,
    GO_MAMMOTH_TRAP_12                            = 188034,
    GO_MAMMOTH_TRAP_13                            = 188035,
    GO_MAMMOTH_TRAP_14                            = 188036,
    GO_MAMMOTH_TRAP_15                            = 188037,
    GO_MAMMOTH_TRAP_16                            = 188038,
    GO_MAMMOTH_TRAP_17                            = 188039,
    GO_MAMMOTH_TRAP_18                            = 188040,
    GO_MAMMOTH_TRAP_19                            = 188041,
    GO_MAMMOTH_TRAP_20                            = 188042,
    GO_MAMMOTH_TRAP_21                            = 188043,
    GO_MAMMOTH_TRAP_22                            = 188044
};

#define MammothTrapsNum 22
const uint32 MammothTraps[MammothTrapsNum] =
{
    GO_MAMMOTH_TRAP_1, GO_MAMMOTH_TRAP_2, GO_MAMMOTH_TRAP_3, GO_MAMMOTH_TRAP_4, GO_MAMMOTH_TRAP_5,
    GO_MAMMOTH_TRAP_6, GO_MAMMOTH_TRAP_7, GO_MAMMOTH_TRAP_8, GO_MAMMOTH_TRAP_9, GO_MAMMOTH_TRAP_10,
    GO_MAMMOTH_TRAP_11, GO_MAMMOTH_TRAP_12, GO_MAMMOTH_TRAP_13, GO_MAMMOTH_TRAP_14, GO_MAMMOTH_TRAP_15,
    GO_MAMMOTH_TRAP_16, GO_MAMMOTH_TRAP_17, GO_MAMMOTH_TRAP_18, GO_MAMMOTH_TRAP_19, GO_MAMMOTH_TRAP_20,
    GO_MAMMOTH_TRAP_21, GO_MAMMOTH_TRAP_22
};

class item_dehta_trap_smasher : public ItemScript
{
public:
    item_dehta_trap_smasher() : ItemScript("item_dehta_trap_smasher") { }

    bool OnUse(Player* player, Item* /*item*/, const SpellCastTargets & /*targets*/)
    {
        if (player->GetQuestStatus(QUEST_CANNOT_HELP_THEMSELVES) != QUEST_STATUS_INCOMPLETE)
            return false;

        Creature* pMammoth = player->FindNearestCreature(NPC_TRAPPED_MAMMOTH_CALF, 5.0f);
        if (!pMammoth)
            return false;

        GameObject* pTrap = NULL;
        for (uint8 i = 0; i < MammothTrapsNum; ++i)
        {
            pTrap = player->FindNearestGameObject(MammothTraps[i], 11.0f);
            if (pTrap)
            {
                pMammoth->AI()->DoAction(1);
                pTrap->SetGoState(GO_STATE_READY);
                player->KilledMonsterCredit(NPC_TRAPPED_MAMMOTH_CALF, 0);
                return true;
            }
        }
        return false;
    }
};

enum TheEmissary
{
    QUEST_THE_EMISSARY      =   11626,
    NPC_LEVIROTH            =   26452
};

class item_trident_of_nazjan : public ItemScript
{
public:
    item_trident_of_nazjan() : ItemScript("item_Trident_of_Nazjan") { }

    bool OnUse(Player* player, Item* item, const SpellCastTargets & /*targets*/)
    {
        if (player->GetQuestStatus(QUEST_THE_EMISSARY) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* pLeviroth = player->FindNearestCreature(NPC_LEVIROTH, 10.0f)) // spell range
            {
                pLeviroth->AI()->AttackStart(player);
                return false;
            } else
                player->SendEquipError(EQUIP_ERR_OUT_OF_RANGE, item, NULL);
        } else
            player->SendEquipError(EQUIP_ERR_CLIENT_LOCKED_OUT, item, NULL);
        return true;
    }
};

enum eCapturedFrog
{
    QUEST_THE_PERFECT_SPIES      = 25444,
    NPC_VANIRAS_SENTRY_TOTEM     = 40187
};

class item_captured_frog : public ItemScript
{
public:
    item_captured_frog() : ItemScript("item_captured_frog") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
    {
        if (player->GetQuestStatus(QUEST_THE_PERFECT_SPIES) == QUEST_STATUS_INCOMPLETE)
        {
            if (player->FindNearestCreature(NPC_VANIRAS_SENTRY_TOTEM, 10.0f))
                return false;
            else
                player->SendEquipError(EQUIP_ERR_OUT_OF_RANGE, item, NULL);
        }
        else
            player->SendEquipError(EQUIP_ERR_CLIENT_LOCKED_OUT, item, NULL);
        return true;
    }
};

class item_sylvanas_music_box : public ItemScript
{
    public:
        item_sylvanas_music_box() : ItemScript("item_sylvanas_music_box") { }

        bool OnUse(Player* player, Item* /*item*/, SpellCastTargets const& /*targets*/)
        {
            Map* map = player->GetMap();
            if (!map)
                return false;

            Map::PlayerList const &_list = map->GetPlayers();
            if (_list.isEmpty())
                return false;

            for (Map::PlayerList::const_iterator i = _list.begin(); i != _list.end(); ++i)
            {
                if (Player* pPlayer = i->getSource())
                    if (pPlayer->IsWithinDistInMap(player, 50.0f))
                        pPlayer->SendPlaySound(10896, player->GetGUID());
            }

            return false;
        }
};

namespace ProfessionBookSpells
{
    enum
    {
        Alchemy         = 156614,
        Blacksmithing   = 169923,
        Enchanting      = 161788,
        Engineering     = 161787,
        Inscription     = 161789,
        JewelCrafting   = 169926,
        LeatherWorking  = 169925,
        Tailoring       = 169924,
        FirstAid        = 160329,
        Cooking         = 160360
    };
}

namespace ProfessionBookSpellLearnSpells
{
    uint32 AlchemyLearnedRecipes[] =
    {
        156587, ///< Alchemical Catalyst
        156585, ///< Crescent Oil
        175865, ///< Draenic Invisibility Potion
        175867, ///< Draenic Living Action Potion
        156582, ///< Draenic Mana Potion
        175853, ///< Draenic Swiftness Potion
        175866, ///< Draenic Water Breathing Elixir
        175869, ///< Draenic Water Walking Elixir
        175868, ///< Pure Rage Potion
        175880, ///< Secrets of Draenor Alchemy
    };

    uint32 BlacksmithingLearnedRecipes[] =
    {
        171692, ///< Smoldering Breastplate
        171693, ///< Smoldering Greaves
        171691, ///< Smoldering Helm
        171690, ///< Truesteel Ingot
        176090  ///< Secrets of Draenor Blacksmithing
    };

    uint32 EnchantingLearnedRecipes[] =
    {
        158907, ///< Breath of Critical Strike
        158908, ///< Breath of Haste
        158909, ///< Breath of Mastery
        158910, ///< Breath of Multistrike
        158911, ///< Breath of Versatility
        158896, ///< Breath of Versatility
        159236, ///< Mark of the Shattered Hand
        169092, ///< Temporal Crystal
        169091, ///< Luminous Shard
        162948, ///< Enchanted Dust
        177043  ///< Secrets of Draenor Enchanting
    };

    uint32 EngineeringLearnedRecipes[] =
    {
        169080, ///< Gearspring Parts
        162208, ///< Ultimate Gnomish Army Knife
        162204, ///< Goblin Glider Kit
        173308, ///< Mecha-Blast Rocket
        177054, ///< Secrets of Draenor Engineering
        173309, ///< Shieldtronic Shield
        162207  ///< Stealthman 54
    };

    uint32 InscriptionLearnedRecipes[] =
    {
        175390, ///< Laughing Tarot
        175389, ///< Ocean Tarot
        175392, ///< Savage Tarot
        169081, ///< War Paints
        166669, ///< Card of Omens
        167950, ///< Research: Warbinder's Ink
        177045, ///< Secrets of Draenor Inscription
        178497  ///< Warbinder's Ink
    };

    uint32 JewelCraftingLearnedRecipes[] =
    {
        170710, ///< Glowing Blackrock Band
        170704, ///< Glowing Iron Band
        170707, ///< Glowing Iron Choker
        170711, ///< Shifting Blackrock Band
        170705, ///< Shifting Iron Band
        170708, ///< Shifting Iron Choker
        170712, ///< Whispering Blackrock Band
        170706, ///< Whispering Iron Band
        170709, ///< Whispering Iron Choker
        170700, ///< Taladite Crystal
        176087  ///< Secrets of Draenor Jewelcrafting
    };

    uint32 LeatherWorkingLearnedRecipes[] =
    {
        171260, ///< Journeying Helm
        171261, ///< Journeying Robes
        171262, ///< Journeying Slacks
        171263, ///< Traveling Helm
        171265, ///< Traveling Leggings
        171264, ///< Traveling Tunic
        171391, ///< Burnished Leather
        176089  ///< Secrets of Draenor Leatherworking
    };

    uint32 TailoringLearnedRecipes[] =
    {
        168852, ///< Sumptuous Cowl
        168854, ///< Sumptuous Leggings
        168853, ///< Sumptuous Robes
        168835, ///< Hexweave Cloth
        176058  ///< Secrets of Draenor Tailoring
    };

    uint32 FirstAidLearnedRecipes[] =
    {
        172541, ///< Blackwater Anti-Venom
        172539, ///< Antiseptic Bandage
        172542, ///< Fire Ammonite Oil
        172540  ///< Healing Tonic
    };

    uint32 CookingLearnedRecipes[] =
    {
        161002, ///< Grilled Saberfish
        161001  ///< Saberfish Broth
    };
}

class player_draenor_profession : public PlayerScript
{
    public:
        player_draenor_profession()
            : PlayerScript("player_draenor_profession")
        {
#define MakeVector(a) std::vector<uint32>(a, a + (sizeof(a) / sizeof(a[0])))
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(156606, MakeVector(ProfessionBookSpellLearnSpells::AlchemyLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158737, MakeVector(ProfessionBookSpellLearnSpells::BlacksmithingLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158716, MakeVector(ProfessionBookSpellLearnSpells::EnchantingLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158739, MakeVector(ProfessionBookSpellLearnSpells::EngineeringLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158748, MakeVector(ProfessionBookSpellLearnSpells::InscriptionLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158750, MakeVector(ProfessionBookSpellLearnSpells::JewelCraftingLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158752, MakeVector(ProfessionBookSpellLearnSpells::LeatherWorkingLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158758, MakeVector(ProfessionBookSpellLearnSpells::TailoringLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158741, MakeVector(ProfessionBookSpellLearnSpells::FirstAidLearnedRecipes)));
            m_SpellPerProfessions.push_back(std::pair<uint32, std::vector<uint32>>(158765, MakeVector(ProfessionBookSpellLearnSpells::CookingLearnedRecipes)));
#undef MakeVector
        }

        /// Called when a player logs in.
        /// @p_Player : Player instance
        void OnLogin(Player* p_Player) override
        {
            for (auto l_Pair : m_SpellPerProfessions)
            {
                if (p_Player->HasSpell(l_Pair.first))
                {
                    for (auto l_SpellID : l_Pair.second)
                    {
                        if (!p_Player->HasSpell(l_SpellID))
                            p_Player->learnSpell(l_SpellID, false);
                    }
                }
            }
        }

    private:
        std::vector<std::pair<uint32, std::vector<uint32>>> m_SpellPerProfessions;

};

/// A Treatise on the Alchemy of Draenor - 156614
/// First Aid in Draenor - 160329
/// Introduction to Cooking in Draenor - 160360
/// Learning (Engineering) - 161787
/// Learning (Enchanting) - 161788
/// A Treatise on the Inscription of Draenor - 161789
/// Draenor Blacksmithing - 169923
/// Draenor Tailoring - 169924
/// Draenor Leatherworking - 169925
/// Draenor Jewelcrafting - 169926
class spell_draenor_profession : public SpellScriptLoader
{
    public:
        spell_draenor_profession()
            : SpellScriptLoader("spell_draenor_profession")
        {
        }

        class spell_draenor_profession_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_draenor_profession_SpellScript);

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr || l_Caster->GetTypeId() != TypeID::TYPEID_PLAYER)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                std::vector<uint32> l_LinkedSpells;

                if (l_Player)
                {
#define MakeVector(a) std::vector<uint32>(a, a + (sizeof (a) / sizeof (a[0])))
                    switch (GetSpellInfo()->Id)
                    {
                        case ProfessionBookSpells::Alchemy:        l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::AlchemyLearnedRecipes);        break;
                        case ProfessionBookSpells::Blacksmithing:  l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::BlacksmithingLearnedRecipes);  break;
                        case ProfessionBookSpells::Enchanting:     l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::EnchantingLearnedRecipes);     break;
                        case ProfessionBookSpells::Engineering:    l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::EngineeringLearnedRecipes);    break;
                        case ProfessionBookSpells::Inscription:    l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::InscriptionLearnedRecipes);    break;
                        case ProfessionBookSpells::JewelCrafting:  l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::JewelCraftingLearnedRecipes);  break;
                        case ProfessionBookSpells::LeatherWorking: l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::LeatherWorkingLearnedRecipes); break;
                        case ProfessionBookSpells::Tailoring:      l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::TailoringLearnedRecipes);      break;
                        case ProfessionBookSpells::FirstAid:       l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::FirstAidLearnedRecipes);       break;
                        case ProfessionBookSpells::Cooking:        l_LinkedSpells = MakeVector(ProfessionBookSpellLearnSpells::CookingLearnedRecipes); l_Player->CastSpell(l_Player, 158766, true); break;

                        default:
                            break;
                    }
#undef MakeVector
                }

                for (size_t l_I = 0; l_I < l_LinkedSpells.size(); ++l_I)
                    l_Player->learnSpell(l_LinkedSpells[l_I], false);
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_draenor_profession_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_draenor_profession_SpellScript();
        }
};

/// Eye of the Black Prince - 93403
class item_eye_of_the_black_prince : public ItemScript
{
    public:
        item_eye_of_the_black_prince() : ItemScript("item_eye_of_the_black_prince") { }

        bool OnUse(Player* p_Player, Item* p_Item, SpellCastTargets const& p_Targets)
        {
            Item* l_ItemTarget = p_Targets.GetItemTarget();
            if (l_ItemTarget == nullptr)
            {
                p_Player->SendEquipError(EQUIP_ERR_CLIENT_LOCKED_OUT, p_Item, NULL);
                return true;
            }

            /// Cannot be used on WoD weapons
            if (l_ItemTarget->GetTemplate() && l_ItemTarget->GetTemplate()->ItemLevel > 582)
            {
                p_Player->SendEquipError(EQUIP_ERR_CLIENT_LOCKED_OUT, p_Item, NULL);
                return true;
            }

            return false;
        }
};

/// Clinking Present - 122718
class item_script_clinking_present : public ItemScript
{
    public:
        item_script_clinking_present() : ItemScript("item_script_clinking_present") { }

        enum eItemIDs
        {
            HordeChauffeuredChopper     = 120968,
            AllianceChauffeuredChopper  = 122703
        };

        bool OnOpen(Player* p_Player, Item* /*p_Item*/) override
        {
            if (p_Player->GetTeamId() == TeamId::TEAM_ALLIANCE)
                p_Player->AddItem(eItemIDs::AllianceChauffeuredChopper, 1);
            else
                p_Player->AddItem(eItemIDs::HordeChauffeuredChopper, 1);

            return false;
        }
};

/// Chauffeured Chopper (Horde) - 120968
/// Chauffeured Chopper (Alliance) - 122703
class item_script_chauffeured_chopper : public ItemScript
{
    public:
        item_script_chauffeured_chopper() : ItemScript("item_script_chauffeured_chopper") { }

        enum eItemIDs
        {
            HordeChauffeuredChopper     = 120968,
            AllianceChauffeuredChopper  = 122703
        };

        enum eSpellIDs
        {
            SummonChauffeurHorde    = 179244,
            SummonChauffeurAlliance = 179245
        };

        bool OnUse(Player* p_Player, Item* p_Item, SpellCastTargets const& /*p_Targets*/) override
        {
            switch (p_Item->GetEntry())
            {
                case eItemIDs::HordeChauffeuredChopper:
                {
                    p_Player->learnSpell(eSpellIDs::SummonChauffeurHorde, false);
                    break;
                }
                case eItemIDs::AllianceChauffeuredChopper:
                {
                    p_Player->learnSpell(eSpellIDs::SummonChauffeurAlliance, false);
                    break;
                }
                default:
                    break;
            }

            return false;
        }
};

/// Challenger's Strongbox - 127831
class item_script_challengers_strongbox : public ItemScript
{
    public:
        item_script_challengers_strongbox() : ItemScript("item_script_challengers_strongbox") { }

        bool OnOpen(Player* p_Player, Item* p_Item) override
        {
            ItemTemplate const* l_Proto = p_Item->GetTemplate();
            LootTemplate const* l_LootTemplate = LootTemplates_Item.GetLootFor(l_Proto->ItemId);
            if (!l_LootTemplate)
                return false;

            uint32 l_ItemID = 0;
            std::list<ItemTemplate const*> l_LootTable;
            std::vector<uint32> l_Items;
            l_LootTemplate->FillAutoAssignationLoot(l_LootTable);
            uint32 l_SpecID = p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID();

            for (ItemTemplate const* l_Template : l_LootTable)
            {
                if ((l_Template->AllowableClass && !(l_Template->AllowableClass & p_Player->getClassMask())) ||
                    (l_Template->AllowableRace && !(l_Template->AllowableRace & p_Player->getRaceMask())))
                    continue;

                if (l_Template->HasSpec((SpecIndex)l_SpecID, p_Player->getLevel()))
                    l_Items.push_back(l_Template->ItemId);
            }

            if (l_Items.empty())
                return false;

            l_ItemID = l_Items[urand(0, l_Items.size() - 1)];

            if (!l_ItemID)
                return false;

            if (!p_Player->GetBagsFreeSlots())
                return false;

            if (Item* l_Item = p_Player->AddItem(l_ItemID, 1))
            {
                p_Player->SendDisplayToast(l_ItemID, 1, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false, l_Item->GetAllItemBonuses());
                p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);
            }

            return true;
        }
};

/*#####
# item_mysterious_egg 94295
#####*/

class item_primal_egg : public ItemScript
{
    public:
        item_primal_egg() : ItemScript("item_primal_egg") { }

        enum eItems
        {
            CrackedPrimalEgg    = 94296
        };

        bool OnExpire(Player* player, ItemTemplate const* /*pItemProto*/) override
        {
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, eItems::CrackedPrimalEgg, 1);
            if (msg == EQUIP_ERR_OK)
                player->StoreNewItem(dest, eItems::CrackedPrimalEgg, true, Item::GenerateItemRandomPropertyId(eItems::CrackedPrimalEgg));

            return true;
        }
};

/*#####
# Mysterious Egg 112107
#####*/

class item_mysterious_egg_112107 : public ItemScript
{
    public:
        item_mysterious_egg_112107() : ItemScript("item_mysterious_egg_112107") { }

        enum eItems
        {
            FruitHunter = 117564
        };

        bool OnExpire(Player* player, ItemTemplate const* /*pItemProto*/) override
        {
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, eItems::FruitHunter, 1);
            if (msg == EQUIP_ERR_OK)
                player->StoreNewItem(dest, eItems::FruitHunter, true, Item::GenerateItemRandomPropertyId(eItems::FruitHunter));

            return true;
        }
};

/*#####
# Fel-Spotted Egg 153190
#####*/

class item_fel_spotted_egg_153190 : public ItemScript
{
public:
    item_fel_spotted_egg_153190() : ItemScript("item_fel_spotted_egg_153190") { }

    enum eItems
    {
        Item_CrackedFelSpottedEgg = 153191
    };

    bool OnExpire(Player* p_Player, ItemTemplate const* /*p_ItemProto*/) override
    {
        ItemPosCountVec l_Dest;
        uint8 l_Msg = p_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Dest, eItems::Item_CrackedFelSpottedEgg, 1);
        if (l_Msg == EQUIP_ERR_OK)
            p_Player->StoreNewItem(l_Dest, eItems::Item_CrackedFelSpottedEgg, true, Item::GenerateItemRandomPropertyId(eItems::Item_CrackedFelSpottedEgg));

        return true;
    }
};

/// Regular check about passengers (allows checks for crossfaction mount)
class PlayerScript_VehicleCheck : public PlayerScript
{
    public:
        PlayerScript_VehicleCheck() : PlayerScript("PlayerScript_VehicleCheck") {}

        void OnUpdateZone(Player* p_Player, uint32 /*p_NewZoneID*/, uint32 /*p_OldZoneID*/, uint32 /*p_NewAreaID*/) override
        {
            if (Vehicle* l_Vehicle = p_Player->GetVehicle())
            {
                if (Unit* l_Driver = l_Vehicle->GetBase())
                {
                    if (Player* l_DriverPlayer = l_Driver->ToPlayer())
                    {
                        if (!p_Player->CanMountAsPassenger(l_DriverPlayer))
                            p_Player->ExitVehicle();
                    }
                }
            }
        }
};

/// Time-Lost Proto-Drake hacklootfix
class PlayerScript_ProtoDrakeLootHackfix : public PlayerScript
{
public:
    PlayerScript_ProtoDrakeLootHackfix() : PlayerScript("PlayerScript_ProtoDrakeLootHackfix") {}

    void OnCreatureKill(Player* killer, Creature* killed)
    {
        if (killed->GetEntry() == 32491) ///< Time-Lost Proto-Drake
        {
            if (killer->AddItem(44168, 1))  ///< Reins of the Time-Lost Proto-Drake
                ChatHandler(killer).PSendSysMessage("Reins of the Time-Lost Proto-Drake have been added to your inventory due to loot bugs.");
            else
            {
                // Send by mail
                SQLTransaction trans = CharacterDatabase.BeginTransaction();

                MailDraft draft("Time-Lost Proto-Drake loot", "Due to bugs with the Time-Lost Proto-Drake, here are the Reins of the Time-Lost Proto-Drake as a loot.\n\n-Firestorm Team");

                if (Item* item = Item::CreateItem(44168, 1, killer))
                {
                    item->SaveToDB(trans);                               // save for prevent lost at next mail load, if send fail then item will deleted
                    draft.AddItem(item);
                }

#ifndef CROSS
                draft.SendMailTo(trans, MailReceiver(killer, killer->GetGUIDLow()), MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM));
#else /* CROSS */
                draft.SendMailTo(trans, MailReceiver(killer, killer->GetRealGUIDLow()), MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM));
#endif /* CROSS */
                CharacterDatabase.CommitTransaction(trans);

                ChatHandler(killer).PSendSysMessage("Your inventory seems to be full, we sent you the Reins of the Time-Lost Proto-Drake by mail.");
            }
        }
    }
};

/// Flask of Appearances - 500000
class item_script_morpher : public ItemScript
{
    public:
        item_script_morpher() : ItemScript("item_script_morpher") { }

        static ACE_Based::LockedMap<uint64, std::vector<uint32>> m_Auras;
        static ACE_Based::LockedMap<uint64, std::vector<uint32>> m_Morphs;

        enum eMenus
        {
            MenuAura = GOSSIP_ACTION_INFO_DEF + 1,
            MenuMorph = GOSSIP_ACTION_INFO_DEF + 2,
            MenuDisable = GOSSIP_ACTION_INFO_DEF + 3,
            ListAuraBegin = GOSSIP_ACTION_INFO_DEF + 4
        };

        enum eGossipMenus
        {
            MenuBase        = 300100,
            AuraList        = 300101,
            MorphList       = 300102
        };

        enum eGossipMenuOptions
        {
            OptionAura      = 0,
            OptionMorph     = 1,
            OptionDisable   = 2
        };

        bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
        {
            if (!player->GetMap()->GetEntry()->IsWorldMap())
                return true;

            m_Auras[player->GetGUID()] = player->GetCosmetics(eCosmetics::CosmeticAura);
            m_Morphs[player->GetGUID()] = player->GetCosmetics(eCosmetics::CosmeticMorph);

            player->PlayerTalkClass->ClearMenus();
            player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionAura, GOSSIP_SENDER_MAIN, eMenus::MenuAura);
            player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionMorph, GOSSIP_SENDER_MAIN, eMenus::MenuMorph);
            player->ADD_GOSSIP_ITEM_DB(eGossipMenus::MenuBase, eGossipMenuOptions::OptionDisable, GOSSIP_SENDER_MAIN, eMenus::MenuDisable);
            player->SEND_GOSSIP_MENU(eGossipMenus::MenuBase, item->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Item* p_Item, uint32 p_Sender, uint32 p_Action) override
        {
            if (!p_Player->GetMap()->GetEntry()->IsWorldMap())
                return true;

            switch (p_Action)
            {
                case eMenus::MenuAura:
                {
                    GenerateAuraList(p_Player, p_Item);
                    break;
                }
                case eMenus::MenuMorph:
                {
                    GenerateMorphList(p_Player, p_Item);
                    break;
                }
                case eMenus::MenuDisable:
                {
                    p_Player->UpdateActiveCosmetic(eCosmetics::MaxCosmetic, 0);
                    p_Player->PlayerTalkClass->SendCloseGossip();
                    break;
                }
                default:
                {
                    uint32 l_ListMorphBegin = (m_Auras[p_Player->GetGUID()].size() + eMenus::ListAuraBegin);

                    if (p_Action >= eMenus::ListAuraBegin && p_Action < l_ListMorphBegin)
                        p_Player->UpdateActiveCosmetic(eCosmetics::CosmeticAura, m_Auras[p_Player->GetGUID()][p_Action - eMenus::ListAuraBegin]);

                    if (p_Action >= l_ListMorphBegin)
                        p_Player->UpdateActiveCosmetic(eCosmetics::CosmeticMorph, m_Morphs[p_Player->GetGUID()][p_Action - l_ListMorphBegin]);

                    p_Player->EnableCosmetic(p_Player->GetMap()->GetEntry()->IsWorldMap());
                    p_Player->PlayerTalkClass->SendCloseGossip();
                    break;
                }
            }

            return true;
        }

        void GenerateAuraList(Player* p_Player, Item* p_Item)
        {
            uint8 l_Itr = 0;
            p_Player->PlayerTalkClass->ClearMenus();

            for (auto l_Aura : m_Auras[p_Player->GetGUID()])
            {
                p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetCosmeticName(p_Player, eCosmetics::CosmeticAura, l_Aura), GOSSIP_SENDER_MAIN, eMenus::ListAuraBegin + l_Itr);
                ++l_Itr;
            }

            p_Player->SEND_GOSSIP_MENU(eGossipMenus::AuraList, p_Item->GetGUID());
        }

        void GenerateMorphList(Player* p_Player, Item* p_Item)
        {
            uint8 l_Itr = 0;
            p_Player->PlayerTalkClass->ClearMenus();

            uint32 l_ListMorphBegin = (m_Auras[p_Player->GetGUID()].size() + eMenus::ListAuraBegin);

            for (auto l_Morph : m_Morphs[p_Player->GetGUID()])
            {
                p_Player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetCosmeticName(p_Player, eCosmetics::CosmeticMorph, l_Morph), GOSSIP_SENDER_MAIN, l_ListMorphBegin + l_Itr);
                ++l_Itr;
            }

            p_Player->SEND_GOSSIP_MENU(eGossipMenus::MorphList, p_Item->GetGUID());
        }
};

/// Flask of appearances - 500000
class PlayerScript_flask_of_appearances : public PlayerScript
{
    public:
        PlayerScript_flask_of_appearances() : PlayerScript("PlayerScript_flask_of_appearances") { }

        void OnLogin(Player* p_Player) override
        {
            std::vector<uint32> l_AurasOrMorphs;
            l_AurasOrMorphs = p_Player->GetCosmetics(eCosmetics::CosmeticAura);
            if (l_AurasOrMorphs.empty())
                l_AurasOrMorphs = p_Player->GetCosmetics(eCosmetics::CosmeticMorph);

            if (l_AurasOrMorphs.empty())
            {
                if (Item * l_Item = p_Player->GetItemByEntry(ITEM_COSMETIC))
                    p_Player->DestroyItem(l_Item->GetBagSlot(), l_Item->GetSlot(), true);

                return;
            }

            if (!p_Player->GetItemByEntry(ITEM_COSMETIC))
                p_Player->AddItem(ITEM_COSMETIC, 1);
        }
};

/// Keystone ID: 138019
class item_challenge_key : public ItemScript
{
    public:
        item_challenge_key() : ItemScript("item_challenge_key") { }

        bool OnCreate(Player* p_Player, Item* p_Item) override
        {
            p_Item->SetUInt32Value(ITEM_FIELD_EXPIRATION, sWorld->getNextChallengeKeyReset() - time(nullptr));

            uint32 l_KeyLevel = 2;
            if (auto l_OploteLoot = sChallengeMgr->GetOploteLoot(p_Player->GetGUID()))
                l_KeyLevel = std::max(2, (int)l_OploteLoot->ChallengeLevel - 1);
            else if (uint32 l_SavedKeystoneLevel = p_Player->m_SpellHelper.GetUint32(eSpellHelpers::KeystoneLevel))
                l_KeyLevel = l_SavedKeystoneLevel;

            l_KeyLevel = std::min(l_KeyLevel, (uint32)ChallengeLevels::Max);

            MapChallengeModeEntry const* l_MapChallenge = nullptr;

            while (!l_MapChallenge)
                l_MapChallenge = JadeCore::Containers::SelectRandomContainerElement(*GetLegionChallengeModeEntries()).second;

            p_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_MAP_CHALLENGE_MODE_ID, l_MapChallenge->ID);
            p_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_LEVEL, l_KeyLevel);
            p_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_IS_CHARGED, 1);

            if (l_KeyLevel > 3)
                p_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_1, sChallengeMgr->GetCurrentAffixes()[0]);
            if (l_KeyLevel > 6)
                p_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_2, sChallengeMgr->GetCurrentAffixes()[1]);
            if (l_KeyLevel > 9)
                p_Item->SetModifier(ITEM_MODIFIER_CHALLENGE_KEYSTONE_AFFIX_ID_3, sChallengeMgr->GetCurrentAffixes()[2]);

            p_Item->SetState(ITEM_CHANGED, p_Player);

            return false;
        }
};

class item_pvp_apparence : public ItemScript
{
    public:
        item_pvp_apparence() : ItemScript("item_pvp_apparence") { }

        bool OnUse(Player* p_Player, Item* p_Item, SpellCastTargets const& p_Targets) override
        {
            p_Player->CompletedAchievement(sAchievementStore.LookupEntry(p_Player->GetTeamId() == TeamId::TEAM_HORDE ? 10745 : 10743));

            if (p_Item->GetEntry() == 140455 || p_Item->GetEntry() == 140452)
                p_Player->CompletedAchievement(sAchievementStore.LookupEntry(p_Player->GetTeamId() == TeamId::TEAM_HORDE ? 11166 : 11165));

            if (p_Item->GetEntry() == 140453 || p_Item->GetEntry() == 140456)
                p_Player->CompletedAchievement(sAchievementStore.LookupEntry(p_Player->GetTeamId() == TeamId::TEAM_HORDE ? 11168 : 11167));

            if (p_Item->GetEntry() == 140454 || p_Item->GetEntry() == 140457)
                p_Player->CompletedAchievement(sAchievementStore.LookupEntry(p_Player->GetTeamId() == TeamId::TEAM_HORDE ? 11170 : 11169));

            return true;
        }
};

/// This shit should be removed after some time
/// Achievement On Metzen - 273
class playerscript_on_metzen_reward : public PlayerScript
{
    public:
        playerscript_on_metzen_reward() : PlayerScript("playerscript_on_metzen_reward") {}

        enum eAchievements
        {
            OnMetzen = 273
        };

        enum eItems
        {
            WinterVeilDisguiseKit = 17712
        };

        void OnLogin(Player* p_Player) override
        {
            if (p_Player->HasAchieved(eAchievements::OnMetzen) && !p_Player->HasToy(eItems::WinterVeilDisguiseKit) &&
                !p_Player->HasItemCount(eItems::WinterVeilDisguiseKit))
            {
                p_Player->AddNewToyToBox(eItems::WinterVeilDisguiseKit);
            }
        }
};

/// Achievement Commander of Argus - 12078
class playerscript_commander_of_argus_reward : public PlayerScript
{
    public:
        playerscript_commander_of_argus_reward() : PlayerScript("playerscript_commander_of_argus_reward") {}

        enum eAchievements
        {
            CommanderOfArgus = 12078
        };

        enum eSpells
        {
            ArsenalLightforgedWeapons = 250032
        };

        enum eQuests
        {
            CustomQuestCommanderOfArgus = 15239600
        };

        void OnAchievementComplete(Player* p_Player, AchievementEntry const* p_Achievement) override
        {
            switch (p_Achievement->ID)
            {
                case eAchievements::CommanderOfArgus:
                {
                    p_Player->CastSpell(p_Player, eSpells::ArsenalLightforgedWeapons, true);

                    if (Quest const* l_Quest = sObjectMgr->GetQuestTemplate(eQuests::CustomQuestCommanderOfArgus))
                    {
                        p_Player->RewardQuest(l_Quest, 0, nullptr, false);
                    }

                    break;
                }
                default:
                    break;
            }
        }

        /// This part of code should be removed after some time
        void OnLogin(Player* p_Player) override
        {
            if (p_Player->HasAchieved(eAchievements::CommanderOfArgus) && !p_Player->IsQuestRewarded(eQuests::CustomQuestCommanderOfArgus))
            {
                p_Player->CastSpell(p_Player, eSpells::ArsenalLightforgedWeapons, true);

                if (Quest const* l_Quest = sObjectMgr->GetQuestTemplate(eQuests::CustomQuestCommanderOfArgus))
                {
                    p_Player->RewardQuest(l_Quest, 0, nullptr, false);
                }
            }
        }
};

ACE_Based::LockedMap<uint64, std::vector<uint32>> item_script_morpher::m_Auras;
ACE_Based::LockedMap<uint64, std::vector<uint32>> item_script_morpher::m_Morphs;

/// Artifact booster - 60000 - Almighty artifact booster - 600001
class item_script_artifact_booster : public ItemScript
{
    public:
        item_script_artifact_booster() : ItemScript("item_script_artifact_booster") { }

        enum eData
        {
            ArtifactLevel   = 80,
            OneSpecItem     = 600010,
            AllSpecsItem    = 600011
        };

        bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
        {
            MS::Artifact::Manager* l_Manager = player->GetCurrentlyEquippedArtifact();
            if (!l_Manager)
            {
                player->SendSysMessage("Your don't have equipped Artifact");
                return true;
            }

            int64 l_APToReward = 0;
            for (uint8 l_Level = l_Manager->GetLevel() + 1; l_Level <= eData::ArtifactLevel; ++l_Level)
                l_APToReward += l_Manager->GetCostForRank(l_Level, true);

            l_APToReward -= l_Manager->GetArtifactPower();
            if (l_APToReward <= 0)
            {
                player->SendSysMessage("Your currently equipped artifact is already above level 80");
                return true;
            }

            l_Manager->ModifyArtifactPower(l_APToReward);
            player->DestroyItemCount(eData::OneSpecItem, 1, true);

            return true;
        }
};

/// 139487, 139484, 139486, 139488 Cache of Nightmarish Treasures
class item_cache_of_nightmarish_treasures : public ItemScript
{
public:
    item_cache_of_nightmarish_treasures() : ItemScript("item_cache_of_nightmarish_treasures") { }

    enum eMaps
    {
        TheEmerlandNightmare = 1520
    };

    enum eItems
    {
        CacheLFR = 139484,
        CacheNormal = 139486,
        CacheHeroic = 139487,
        CacheMythic = 139488
    };

    bool OnOpen(Player* p_Player, Item* p_Item)
    {
        std::vector<uint32> l_PossibleLoots = GetItemLootPerMap(eMaps::TheEmerlandNightmare);
        std::random_device l_RandomDevice;
        std::mt19937 l_RandomGenerator(l_RandomDevice());
        std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

        for (uint32 l_ItemId : l_PossibleLoots)
        {
            ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
            if (!l_ItemTpl || !l_ItemTpl->IsStuff() || !p_Item)
                continue;

            if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                continue;

            if (p_Item->GetEntry() == eItems::CacheLFR)
            {
                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidLfr))
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
            }
            else if (p_Item->GetEntry() == eItems::CacheNormal)
            {
                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidNormal))
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
            }
            else if (p_Item->GetEntry() == eItems::CacheHeroic) 
            {
                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidHeroic))
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
            }
            else if (p_Item->GetEntry() == eItems::CacheMythic)
            {
                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidMythic))
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
            }

            break;
        }

        p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);
        return true;
    }
};

/// 140150, 140148, 140152, 140154 Cache of Nightborne Treasures
class item_cache_of_nightborne_treasures : public ItemScript
{
public:
    item_cache_of_nightborne_treasures() : ItemScript("item_cache_of_nightborne_treasures") { }

    enum eMaps
    {
        TheNighthold = 1530
    };

    enum eItems
    {
        CacheLFR = 140148,
        CacheNormal = 140150,
        CacheHeroic = 140152,
        CacheMythic = 140154
    };

    bool OnOpen(Player* p_Player, Item* p_Item)
    {
        std::vector<uint32> l_PossibleLoots = GetItemLootPerMap(eMaps::TheNighthold);
        std::random_device l_RandomDevice;
        std::mt19937 l_RandomGenerator(l_RandomDevice());
        std::shuffle(l_PossibleLoots.begin(), l_PossibleLoots.end(), l_RandomGenerator);

        for (uint32 l_ItemId : l_PossibleLoots)
        {
            ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
            if (!l_ItemTpl || !l_ItemTpl->IsStuff() || !p_Item)
                continue;

            if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                continue;

            if (p_Item->GetEntry() == eItems::CacheLFR)
            {
                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidLfr))
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
            }
            else if (p_Item->GetEntry() == eItems::CacheNormal)
            {
                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidNormal))
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
            }
            else if (p_Item->GetEntry() == eItems::CacheHeroic)
            {
                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidHeroic))
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
            }
            else if (p_Item->GetEntry() == eItems::CacheMythic)
            {
                if (Item* l_Item = p_Player->AddItem(l_ItemTpl->ItemId, 1, {}, false, 0, true, true, ItemContext::RaidMythic))
                    p_Player->SendDisplayToast(l_ItemTpl->ItemId, 0, 0, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, true, l_Item->GetAllItemBonuses());
            }

            break;
        }

        p_Player->DestroyItem(p_Item->GetBagSlot(), p_Item->GetSlot(), true);
        return true;
    }
};

#ifndef __clang_analyzer__
void AddSC_item_scripts()
{
    new item_only_for_flight();
    new item_nether_wraith_beacon();
    new item_gor_dreks_ointment();
    new item_incendiary_explosives();
    new item_mysterious_egg();
    new item_disgusting_jar();
    new item_pile_fake_furs();
    new item_petrov_cluster_bombs();
    new item_dehta_trap_smasher();
    new item_trident_of_nazjan();
    new item_captured_frog();
    new item_sylvanas_music_box();
    new spell_draenor_profession();
    new player_draenor_profession();
    new item_eye_of_the_black_prince();
    new item_script_clinking_present();
    new item_script_chauffeured_chopper();
    new item_script_challengers_strongbox();
    new item_challenge_key();
    new item_pvp_apparence();
    new item_primal_egg();
    new item_mysterious_egg_112107();
    new item_fel_spotted_egg_153190();
    new PlayerScript_VehicleCheck();
    new PlayerScript_ProtoDrakeLootHackfix();
    new playerscript_on_metzen_reward();
    new playerscript_commander_of_argus_reward();
    new item_cache_of_nightmarish_treasures();
    new item_cache_of_nightborne_treasures();

    /// Customs
    new item_script_morpher();
    new item_script_artifact_booster();
    new PlayerScript_flask_of_appearances();
}
#endif
