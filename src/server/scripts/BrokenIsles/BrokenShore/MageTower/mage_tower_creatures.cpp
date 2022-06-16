////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-Studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "ScriptedGossip.h"
#include "artifact_challenges.hpp"

/// War Councilor Victoria <Kirin Tor> - 117871
class npc_war_councilor_victoria : public CreatureScript
{
    public:
        npc_war_councilor_victoria() : CreatureScript("npc_war_councilor_victoria") { }

        enum eQuests
        {
            QuestAnImpossibleFoe        = 46065,
            QuestClosingTheEye          = 44925,
            QuestTheGodQueensFury       = 45526,
            QuestFeltotemsFall          = 45627,
            QuestTheHighlordsReturn     = 45416,
            QuestEndOfTheRisenThreat    = 46035,
            QuestThwartingTheTwins      = 46127
        };

        enum eGossipData
        {
            DataMenuID  = 20778,
            DataTextID  = 31209,
            /// Actions
            DataReturn  = 1000
        };

        enum eRespecSpells
        {
            /// Death Knight
            SpellBlood              = 227534,
            SpellFrost              = 228033,
            SpellUnholy             = 228034,
            /// Demon Hunter
            SpellHavoc              = 228035,
            SpellVengeance          = 228036,
            /// Druid
            SpellBalance            = 228037,
            SpellFeral              = 228038,
            SpellGuardian           = 228039,
            SpellRestoration        = 228040,
            /// Hunter
            SpellBeastMastery       = 228041,
            SpellMarksmanship       = 228042,
            SpellSurvival           = 228043,
            /// Mage
            SpellArcane             = 228044,
            SpellFire               = 228045,
            SpellFrostMage          = 228046,
            /// Monk
            SpellBrewmaster         = 228047,
            SpellMistweaver         = 228081,
            SpellWindwalker         = 228082,
            /// Paladin
            SpellHoly               = 228083,
            SpellProtection         = 228084,
            SpellRetribution        = 228085,
            /// Priest
            SpellDiscipline         = 228087,
            SpellHolyPriest         = 228089,
            SpellShadow             = 228090,
            /// Rogue
            SpellAssassination      = 228091,
            SpellOutlaw             = 228092,
            SpellSubtlety           = 228093,
            /// Shaman
            SpellElemental          = 228094,
            SpellEnhancement        = 228095,
            SpellRestorationShaman  = 228103,
            /// Warlock
            SpellAffliction         = 228096,
            SpellDemonology         = 228097,
            SpellDestruction        = 228098,
            /// Warrior
            SpellArms               = 228099,
            SpellFury               = 228100,
            SpellProtectionWarrior  = 228101
        };

        ///                                  Quest             Free   Nethershard
        using GossipOptionByQuest = std::map<uint32, std::pair<uint8, uint8>>;

        GossipOptionByQuest m_GossipsByQuests =
        {
            { eQuests::QuestAnImpossibleFoe,        { 5, 12 } },
            { eQuests::QuestClosingTheEye,          { 4, 11 } },
            { eQuests::QuestTheGodQueensFury,       { 0, 7  } },
            { eQuests::QuestFeltotemsFall,          { 3, 10 } },
            { eQuests::QuestTheHighlordsReturn,     { 1, 8  } },
            { eQuests::QuestEndOfTheRisenThreat,    { 6, 13 } },
            { eQuests::QuestThwartingTheTwins,      { 2, 9  } }
        };

        uint32 m_NetherShardsCost = 100;

        std::map<uint32, uint32> m_ConversationsByQuests =
        {
            { eQuests::QuestAnImpossibleFoe,        4431 },
            { eQuests::QuestFeltotemsFall,          4665 },
            { eQuests::QuestClosingTheEye,          4659 },
            { eQuests::QuestThwartingTheTwins,      5009 },
            { eQuests::QuestTheGodQueensFury,       4877 },
            { eQuests::QuestEndOfTheRisenThreat,    4664 },
            { eQuests::QuestTheHighlordsReturn,     4666 }
        };

        std::map<uint32, uint32> m_DungeonsByQuests =
        {
            { eQuests::QuestAnImpossibleFoe,        eDungeons::DungeonTheImpMothersDen      },
            { eQuests::QuestFeltotemsFall,          eDungeons::DungeonFeltotemsFall         },
            { eQuests::QuestClosingTheEye,          eDungeons::DungeonTheArchmagesReckoning },
            { eQuests::QuestThwartingTheTwins,      eDungeons::DungeonThwartingTheTwins     },
            { eQuests::QuestTheGodQueensFury,       eDungeons::DungeonTheGodQueensFury      },
            { eQuests::QuestEndOfTheRisenThreat,    eDungeons::DungeonEndOfTheRisenThreat   },
            { eQuests::QuestTheHighlordsReturn,     eDungeons::DungeonTheHighlordsReturn    }
        };

        std::map<uint32, std::pair<uint32, uint32>> m_ClassSpecRequirements =
        {
            { SpecIndex::SPEC_WARRIOR_ARMS,             { eQuests::QuestClosingTheEye,          eRespecSpells::SpellArms                } },
            { SpecIndex::SPEC_WARRIOR_FURY,             { eQuests::QuestAnImpossibleFoe,        eRespecSpells::SpellFury                } },
            { SpecIndex::SPEC_WARRIOR_PROTECTION,       { eQuests::QuestTheHighlordsReturn,     eRespecSpells::SpellProtectionWarrior   } },
            { SpecIndex::SPEC_PALADIN_HOLY,             { eQuests::QuestEndOfTheRisenThreat,    eRespecSpells::SpellHoly                } },
            { SpecIndex::SPEC_PALADIN_PROTECTION,       { eQuests::QuestTheHighlordsReturn,     eRespecSpells::SpellProtection          } },
            { SpecIndex::SPEC_PALADIN_RETRIBUTION,      { eQuests::QuestTheGodQueensFury,       eRespecSpells::SpellRetribution         } },
            { SpecIndex::SPEC_HUNTER_BEASTMASTERY,      { eQuests::QuestFeltotemsFall,          eRespecSpells::SpellBeastMastery        } },
            { SpecIndex::SPEC_HUNTER_MARKSMANSHIP,      { eQuests::QuestThwartingTheTwins,      eRespecSpells::SpellMarksmanship        } },
            { SpecIndex::SPEC_HUNTER_SURVIVAL,          { eQuests::QuestClosingTheEye,          eRespecSpells::SpellSurvival            } },
            { SpecIndex::SPEC_ROGUE_ASSASSINATION,      { eQuests::QuestTheGodQueensFury,       eRespecSpells::SpellAssassination       } },
            { SpecIndex::SPEC_ROGUE_OUTLAW,             { eQuests::QuestAnImpossibleFoe,        eRespecSpells::SpellOutlaw              } },
            { SpecIndex::SPEC_ROGUE_SUBTLETY,           { eQuests::QuestClosingTheEye,          eRespecSpells::SpellSubtlety            } },
            { SpecIndex::SPEC_PRIEST_DISCIPLINE,        { eQuests::QuestFeltotemsFall,          eRespecSpells::SpellDiscipline          } },
            { SpecIndex::SPEC_PRIEST_HOLY,              { eQuests::QuestEndOfTheRisenThreat,    eRespecSpells::SpellHolyPriest          } },
            { SpecIndex::SPEC_PRIEST_SHADOW,            { eQuests::QuestThwartingTheTwins,      eRespecSpells::SpellShadow              } },
            { SpecIndex::SPEC_DK_BLOOD,                 { eQuests::QuestTheHighlordsReturn,     eRespecSpells::SpellBlood               } },
            { SpecIndex::SPEC_DK_FROST,                 { eQuests::QuestClosingTheEye,          eRespecSpells::SpellFrost               } },
            { SpecIndex::SPEC_DK_UNHOLY,                { eQuests::QuestAnImpossibleFoe,        eRespecSpells::SpellUnholy              } },
            { SpecIndex::SPEC_SHAMAN_ELEMENTAL,         { eQuests::QuestAnImpossibleFoe,        eRespecSpells::SpellElemental           } },
            { SpecIndex::SPEC_SHAMAN_ENHANCEMENT,       { eQuests::QuestTheGodQueensFury,       eRespecSpells::SpellEnhancement         } },
            { SpecIndex::SPEC_SHAMAN_RESTORATION,       { eQuests::QuestEndOfTheRisenThreat,    eRespecSpells::SpellRestorationShaman   } },
            { SpecIndex::SPEC_MAGE_ARCANE,              { eQuests::QuestTheGodQueensFury,       eRespecSpells::SpellArcane              } },
            { SpecIndex::SPEC_MAGE_FIRE,                { eQuests::QuestAnImpossibleFoe,        eRespecSpells::SpellFire                } },
            { SpecIndex::SPEC_MAGE_FROST,               { eQuests::QuestThwartingTheTwins,      eRespecSpells::SpellFrostMage           } },
            { SpecIndex::SPEC_WARLOCK_AFFLICTION,       { eQuests::QuestThwartingTheTwins,      eRespecSpells::SpellAffliction          } },
            { SpecIndex::SPEC_WARLOCK_DEMONOLOGY,       { eQuests::QuestTheGodQueensFury,       eRespecSpells::SpellDemonology          } },
            { SpecIndex::SPEC_WARLOCK_DESTRUCTION,      { eQuests::QuestFeltotemsFall,          eRespecSpells::SpellDestruction         } },
            { SpecIndex::SPEC_MONK_BREWMASTER,          { eQuests::QuestTheHighlordsReturn,     eRespecSpells::SpellBrewmaster          } },
            { SpecIndex::SPEC_MONK_WINDWALKER,          { eQuests::QuestFeltotemsFall,          eRespecSpells::SpellWindwalker          } },
            { SpecIndex::SPEC_MONK_MISTWEAVER,          { eQuests::QuestEndOfTheRisenThreat,    eRespecSpells::SpellMistweaver          } },
            { SpecIndex::SPEC_DRUID_BALANCE,            { eQuests::QuestThwartingTheTwins,      eRespecSpells::SpellBalance             } },
            { SpecIndex::SPEC_DRUID_FERAL,              { eQuests::QuestAnImpossibleFoe,        eRespecSpells::SpellFeral               } },
            { SpecIndex::SPEC_DRUID_GUARDIAN,           { eQuests::QuestTheHighlordsReturn,     eRespecSpells::SpellGuardian            } },
            { SpecIndex::SPEC_DRUID_RESTORATION,        { eQuests::QuestEndOfTheRisenThreat,    eRespecSpells::SpellRestoration         } },
            { SpecIndex::SPEC_DEMON_HUNTER_HAVOC,       { eQuests::QuestClosingTheEye,          eRespecSpells::SpellHavoc               } },
            { SpecIndex::SPEC_DEMON_HUNTER_VENGEANCE,   { eQuests::QuestTheHighlordsReturn,     eRespecSpells::SpellVengeance           } }
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player)
                return true;

            p_Player->PlayerTalkClass->ClearGossipMenu();

            bool l_HasQuest = false;

            for (auto const& l_Iter : m_GossipsByQuests)
            {
                if (p_Player->HasQuest(l_Iter.first))
                {
                    l_HasQuest = true;

                    /// Other attempts costs 100 Nethershard
                    if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::BrokenShoreMageTowerAttempts) > 0)
                    {
                        if (p_Player->GetCurrency(CurrencyTypes::CURRENCY_TYPE_NETHERSHARD, false) >= m_NetherShardsCost)
                            p_Player->ADD_GOSSIP_ITEM_DB(eGossipData::DataMenuID, l_Iter.second.second + 7, eTradeskill::GOSSIP_SENDER_MAIN, l_Iter.second.second);
                        else
                            p_Player->ADD_GOSSIP_ITEM_DB(eGossipData::DataMenuID, l_Iter.second.second, eTradeskill::GOSSIP_SENDER_MAIN, eGossipData::DataReturn);
                    }
                    /// Free attempt
                    else
                        p_Player->ADD_GOSSIP_ITEM_DB(eGossipData::DataMenuID, l_Iter.second.first, eTradeskill::GOSSIP_SENDER_MAIN, l_Iter.second.first);
                }
            }

            p_Player->PrepareQuestMenu(p_Creature->GetGUID());

            if (l_HasQuest)
                p_Player->SEND_GOSSIP_MENU(eGossipData::DataTextID, p_Creature->GetGUID());
            else
                p_Player->SendPreparedGossip(p_Creature);

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            if (p_Action == eGossipData::DataReturn)
                return true;

            uint32 l_AttemptCount = p_Player->GetCharacterWorldStateValue(CharacterWorldStates::BrokenShoreMageTowerAttempts);
            if (l_AttemptCount > 0)
            {
                if (p_Player->GetCurrency(CurrencyTypes::CURRENCY_TYPE_NETHERSHARD, false) < m_NetherShardsCost)
                    return true;
            }

            uint32 l_QuestID = 0;

            /// Find the quest related to the selected option
            for (auto const& l_Iter : m_GossipsByQuests)
            {
                if (p_Player->HasQuest(l_Iter.first))
                {
                    if (p_Action != l_Iter.second.first && p_Action != l_Iter.second.second)
                        continue;

                    l_QuestID = l_Iter.first;
                }
            }

            /// Quest not found for dungeon list
            auto const& l_Iter = m_DungeonsByQuests.find(l_QuestID);
            if (l_Iter == m_DungeonsByQuests.end())
                return true;

            /// Spec not found
            auto const& l_SpecIter = m_ClassSpecRequirements.find(p_Player->GetActiveSpecializationID());
            if (l_SpecIter == m_ClassSpecRequirements.end())
                return true;

            /// Spec does not fit selected gossip, ask player if he wants to change spec
            if (l_QuestID != l_SpecIter->second.first)
            {
                /// Get the spec needed for asked quest
                for (auto const& l_Itr : m_ClassSpecRequirements)
                {
                    /// Not for this class
                    ChrSpecializationsEntry const* l_Entry = sChrSpecializationsStore.LookupEntry(l_Itr.first);
                    if (!l_Entry || l_Entry->ClassID != p_Player->getClass())
                        continue;

                    if (l_Itr.second.first == l_QuestID)
                    {
                        if (uint32 l_SpellID = l_Itr.second.second)
                        {
                            p_Player->CastSpell(p_Player, l_SpellID, true);
                            break;
                        }
                    }
                }

                p_Player->PlayerTalkClass->SendCloseGossip();
                return false;
            }

            if (l_AttemptCount > 0)
                p_Player->ModifyCurrency(CurrencyTypes::CURRENCY_TYPE_NETHERSHARD, -int32(m_NetherShardsCost));

            p_Player->SetCharacterWorldState(CharacterWorldStates::BrokenShoreMageTowerAttempts, l_AttemptCount + 1);

            std::set<uint32> l_DungeonSet;

            uint8 l_Roles = LfgRoles::LFG_ROLEMASK_DAMAGE;

            l_DungeonSet.insert(l_Iter->second);

            sLFGMgr->Join(p_Player, l_Roles, l_DungeonSet, "");

            p_Player->PlayerTalkClass->SendCloseGossip();
            return false;
        }

        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, Quest const* p_Quest) override
        {
            auto const& l_Iter = m_ConversationsByQuests.find(p_Quest->GetQuestId());
            if (l_Iter == m_ConversationsByQuests.end())
                return false;

            Conversation* l_Conversation = new Conversation;
            if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), l_Iter->second, p_Player, nullptr, *p_Player))
                delete l_Conversation;

            return true;
        }
};

class player_script_challenge_completion : public PlayerScript
{
    public:
        player_script_challenge_completion() : PlayerScript("player_script_challenge_completion") { }

        enum eQuests
        {
            QuestTheImpMothersGift  = 45902,
            QuestXylemsGift         = 45908,
            QuestTheGodQueensGift   = 45904,
            QuestNavaroggsGift      = 45909,
            QuestKruulsGift         = 45905,
            QuestJarodsGift         = 45906,
            QuestRaestsGift         = 45910
        };

        enum eMounts
        {
            MountDeathKnight    = 229387,
            MountDemonHunter    = 229417,
            MountWarlock        = 232412,
            MountWarrior        = 229388,
            MountShaman         = 231442,
            MountPriest         = 229377,
            MountMonk           = 229385,
            MountMage           = 229376,
            MountRogue          = 231434,
            MountPaladin        = 231435,
            MountHunter         = 229386,
            MountDruid          = 231437
        };

        std::map<uint32, std::map<uint8, uint32>> m_QuestMounts =
        {
            {
                eQuests::QuestTheImpMothersGift,
                {
                    { Classes::CLASS_DEATH_KNIGHT,  eMounts::MountDeathKnight },
                    { Classes::CLASS_DRUID,         eMounts::MountDruid },
                    { Classes::CLASS_MAGE,          eMounts::MountMage },
                    { Classes::CLASS_ROGUE,         eMounts::MountRogue },
                    { Classes::CLASS_WARRIOR,       eMounts::MountWarrior },
                    { Classes::CLASS_SHAMAN,        eMounts::MountShaman }
                }
            },
            {
                eQuests::QuestXylemsGift,
                {
                    { Classes::CLASS_DEATH_KNIGHT,  eMounts::MountDeathKnight },
                    { Classes::CLASS_DEMON_HUNTER,  eMounts::MountDemonHunter },
                    { Classes::CLASS_HUNTER,        eMounts::MountHunter },
                    { Classes::CLASS_ROGUE,         eMounts::MountRogue },
                    { Classes::CLASS_WARRIOR,       eMounts::MountWarrior }
                }
            },
            {
                eQuests::QuestTheGodQueensGift,
                {
                    { Classes::CLASS_MAGE,      eMounts::MountMage },
                    { Classes::CLASS_PALADIN,   eMounts::MountPaladin },
                    { Classes::CLASS_ROGUE,     eMounts::MountRogue },
                    { Classes::CLASS_SHAMAN,    eMounts::MountShaman },
                    { Classes::CLASS_WARLOCK,   eMounts::MountWarlock }
                }
            },
            {
                eQuests::QuestNavaroggsGift,
                {
                    { Classes::CLASS_HUNTER,    eMounts::MountHunter },
                    { Classes::CLASS_MONK,      eMounts::MountMonk },
                    { Classes::CLASS_PRIEST,    eMounts::MountPriest },
                    { Classes::CLASS_WARLOCK,   eMounts::MountWarlock }
                }
            },
            {
                eQuests::QuestKruulsGift,
                {
                    { Classes::CLASS_DEATH_KNIGHT,  eMounts::MountDeathKnight },
                    { Classes::CLASS_DEMON_HUNTER,  eMounts::MountDemonHunter },
                    { Classes::CLASS_DRUID,         eMounts::MountDruid },
                    { Classes::CLASS_MONK,          eMounts::MountMonk },
                    { Classes::CLASS_PALADIN,       eMounts::MountPaladin },
                    { Classes::CLASS_WARRIOR,       eMounts::MountWarrior }
                }
            },
            {
                eQuests::QuestJarodsGift,
                {
                    { Classes::CLASS_DRUID,     eMounts::MountDruid },
                    { Classes::CLASS_PALADIN,   eMounts::MountPaladin },
                    { Classes::CLASS_PRIEST,    eMounts::MountPriest },
                    { Classes::CLASS_SHAMAN,    eMounts::MountShaman },
                    { Classes::CLASS_MONK,      eMounts::MountMonk }
                }
            },
            {
                eQuests::QuestRaestsGift,
                {
                    { Classes::CLASS_DRUID,     eMounts::MountDruid },
                    { Classes::CLASS_HUNTER,    eMounts::MountHunter },
                    { Classes::CLASS_MAGE,      eMounts::MountMage },
                    { Classes::CLASS_PRIEST,    eMounts::MountPriest },
                    { Classes::CLASS_WARLOCK,   eMounts::MountWarlock }
                }
            }
        };

        void OnQuestComplete(Player* p_Player, Quest const* p_Quest) override
        {
            if (!p_Player || !p_Quest)
                return;

            /// Wrong quest
            auto const& l_Iter = m_QuestMounts.find(p_Quest->GetQuestId());
            if (l_Iter == m_QuestMounts.end())
                return;

            /// Wrong class
            auto const& l_ClassIter = l_Iter->second.find(p_Player->getClass());
            if (l_ClassIter == l_Iter->second.end())
                return;

            if (!p_Player->HasSpell(l_ClassIter->second))
                p_Player->learnSpell(l_ClassIter->second, false);
        }

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player)
                return;

            for (auto const& l_Iter : m_QuestMounts)
            {
                auto const& l_ClassIter = l_Iter.second.find(p_Player->getClass());
                if (l_ClassIter == l_Iter.second.end())
                    continue;

                if (p_Player->GetQuestStatus(l_Iter.first) == QuestStatus::QUEST_STATUS_REWARDED)
                {
                    if (!p_Player->HasSpell(l_ClassIter->second))
                        p_Player->learnSpell(l_ClassIter->second, false);

                    break;
                }
            }
        }
};

#ifndef __clang_analyzer__
void AddSC_mage_tower_creatures()
{
    /// NPC
    new npc_war_councilor_victoria();

    /// PlayerScript
    new player_script_challenge_completion();
}
#endif
