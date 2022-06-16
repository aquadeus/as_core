////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GameTables.h"

/*
 * You need to specify index field name of GameTable which you choose in the ctor
 * The key will be read as an uint32, the other fields will be read as floats
 * sizeof(structs) / 4 must be equal to number of the rows
 */

GameTable<BaseManaTableEntry>               g_BaseManaTable("Level");
GameTable<SpellScalingTableEntry>           g_SpellScalingTable("Level");
GameTable<XPTableEntry>                     g_XPTable("Level");
GameTable<HonorLevelTableEntry>             g_HonorLevelTable("Level");
GameTable<HpPerStaTableEntry>               g_HpPerStaTable("Level");
GameTable<ArmorMitigationByLvlTableEntry>   g_ArmorMitigationByLvlTable("Level");
GameTable<CombatRatingsMultByILvl>          g_CombatRatingMultByIlvl("Item Level");
GameTable<CombatRatingsTableEntry>          g_CombatRatingsTable("Level");
GameTable<ArtifactLevelXPTableEntry>        g_ArtifactLevelXP("Level");
GameTable<ArtifactKnowledgeMultiplierEntry> g_ArtifactKnowledgeMultiplier("Level");
GameTable<ItemSocketCostPerLevelEntry>      g_ItemSocketCostPerLevel("Level");
GameTable<BarberShopCostBaseEntry>          g_BarberShopCostBaseEntry("Level");
GameTable<BattlePetTypeDamageModEntry>      g_BattlePetTypeDamageModEntry("PetType");
GameTable<BattlePetXPEntry>                 g_BattlePetXPStore("LEVEL");
GameTable<NpcDamageByClassEntry>            g_NpcDamageByClassGameTable[MAX_EXPANSION] = {  GameTable<NpcDamageByClassEntry>("Level"),
                                                                                            GameTable<NpcDamageByClassEntry>("Level"),
                                                                                            GameTable<NpcDamageByClassEntry>("Level"),
                                                                                            GameTable<NpcDamageByClassEntry>("Level"),
                                                                                            GameTable<NpcDamageByClassEntry>("Level"),
                                                                                            GameTable<NpcDamageByClassEntry>("Level"),
                                                                                            GameTable<NpcDamageByClassEntry>("Level"),
                                                                                            };
GameTable<NpcTotalHpEntry>                  g_NpcTotalHpGameTable[MAX_EXPANSION]       = {  GameTable<NpcTotalHpEntry>("Level"),
                                                                                            GameTable<NpcTotalHpEntry>("Level"),
                                                                                            GameTable<NpcTotalHpEntry>("Level"),
                                                                                            GameTable<NpcTotalHpEntry>("Level"),
                                                                                            GameTable<NpcTotalHpEntry>("Level"),
                                                                                            GameTable<NpcTotalHpEntry>("Level"),
                                                                                            GameTable<NpcTotalHpEntry>("Level"),
                                                                                            };
GameTable<NpcManaCostScalerEntry>           g_NpcManaCostScalerGameTable("Level");

GameTable<ChallengeModeDamageEntry>         g_ChallengeModeDamageTable("ChallengeLevel");
GameTable<ChallengeModeHealthEntry>         g_ChallengeModeHealthTable("ChallengeLevel");

template <class T>
void LoadGameTable(std::list<std::string>& p_NotLoadableTables, GameTable<T>& p_Table, std::string const& p_GameTableFolder, std::string p_FileName)
{
    std::string l_FinalPath = p_GameTableFolder;
    l_FinalPath += p_FileName;


    if (!p_Table.Load(l_FinalPath))
        p_NotLoadableTables.push_back(p_FileName);
}

void LoadGameTables(const std::string& p_DataPath)
{
    std::string l_GameTablePath = p_DataPath + "dbc/gametables/";
    std::list<std::string> l_NotLoadableTables;

    LoadGameTable(l_NotLoadableTables, g_BaseManaTable,                 l_GameTablePath, "BaseMp.txt");
    LoadGameTable(l_NotLoadableTables, g_SpellScalingTable,             l_GameTablePath, "SpellScaling.txt");
    LoadGameTable(l_NotLoadableTables, g_XPTable,                       l_GameTablePath, "xp.txt");
    LoadGameTable(l_NotLoadableTables, g_HonorLevelTable,               l_GameTablePath, "HonorLevel.txt");
    LoadGameTable(l_NotLoadableTables, g_HpPerStaTable,                 l_GameTablePath, "HpPerSta.txt");
    LoadGameTable(l_NotLoadableTables, g_ArmorMitigationByLvlTable,     l_GameTablePath, "ArmorMitigationByLvl.txt");
    LoadGameTable(l_NotLoadableTables, g_CombatRatingsTable,            l_GameTablePath, "CombatRatings.txt");
    LoadGameTable(l_NotLoadableTables, g_CombatRatingMultByIlvl,        l_GameTablePath, "CombatRatingsMultByILvl.txt");
    LoadGameTable(l_NotLoadableTables, g_ArtifactLevelXP,               l_GameTablePath, "ArtifactLevelXP.txt");
    LoadGameTable(l_NotLoadableTables, g_ArtifactKnowledgeMultiplier,   l_GameTablePath, "ArtifactKnowledgeMultiplier.txt");
    LoadGameTable(l_NotLoadableTables, g_ItemSocketCostPerLevel,        l_GameTablePath, "ItemSocketCostPerLevel.txt");
    LoadGameTable(l_NotLoadableTables, g_BarberShopCostBaseEntry,       l_GameTablePath, "BarberShopCostBase.txt");
    LoadGameTable(l_NotLoadableTables, g_BattlePetTypeDamageModEntry,   l_GameTablePath, "BattlePetTypeDamageMod.txt");
    LoadGameTable(l_NotLoadableTables, g_BattlePetXPStore,              l_GameTablePath, "BattlePetXP.txt");

    LoadGameTable(l_NotLoadableTables, g_NpcDamageByClassGameTable[0],  l_GameTablePath, "NpcDamageByClass.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcDamageByClassGameTable[1],  l_GameTablePath, "NpcDamageByClassExp1.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcDamageByClassGameTable[2],  l_GameTablePath, "NpcDamageByClassExp2.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcDamageByClassGameTable[3],  l_GameTablePath, "NpcDamageByClassExp3.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcDamageByClassGameTable[4],  l_GameTablePath, "NpcDamageByClassExp4.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcDamageByClassGameTable[5],  l_GameTablePath, "NpcDamageByClassExp5.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcDamageByClassGameTable[6],  l_GameTablePath, "NpcDamageByClassExp6.txt");

    LoadGameTable(l_NotLoadableTables, g_NpcTotalHpGameTable[0],        l_GameTablePath, "NpcTotalHp.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcTotalHpGameTable[1],        l_GameTablePath, "NpcTotalHpExp1.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcTotalHpGameTable[2],        l_GameTablePath, "NpcTotalHpExp2.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcTotalHpGameTable[3],        l_GameTablePath, "NpcTotalHpExp3.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcTotalHpGameTable[4],        l_GameTablePath, "NpcTotalHpExp4.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcTotalHpGameTable[5],        l_GameTablePath, "NpcTotalHpExp5.txt");
    LoadGameTable(l_NotLoadableTables, g_NpcTotalHpGameTable[6],        l_GameTablePath, "NpcTotalHpExp6.txt");

    LoadGameTable(l_NotLoadableTables, g_NpcManaCostScalerGameTable,    l_GameTablePath, "NPCManaCostScaler.txt");

    LoadGameTable(l_NotLoadableTables, g_ChallengeModeDamageTable,      l_GameTablePath, "ChallengeModeDamage.txt");
    LoadGameTable(l_NotLoadableTables, g_ChallengeModeHealthTable,      l_GameTablePath, "ChallengeModeHealth.txt");


    if (!l_NotLoadableTables.empty())
    {
        std::string l_Str;
        for (std::list<std::string>::iterator l_I = l_NotLoadableTables.begin(); l_I != l_NotLoadableTables.end(); ++l_I)
            l_Str += *l_I + "\n";

        sLog->outError(LOG_FILTER_GENERAL, "\nSome required gametables could not be loaded:\n%s", l_Str.c_str());
        exit(1);
    }
}
