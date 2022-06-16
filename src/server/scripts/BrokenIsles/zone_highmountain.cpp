////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "zone_highmountain.h"

enum eObjectiveIds
{
    StormyGland             = 285238,
    PristineHarpyFeather    = 285300,
    ReflectiveScale         = 285301,
    EssenceOfPureSpirit     = 285299,
    PartsDelivered          = 285239,
    StareInAwe              = 285241
};

enum eActions
{
    ActionQuestPrepared = 1,
    ActionPartDelivered
};

/// Image of Mimiron - 110424
class npc_highmountain_image_of_mimiron : public CreatureScript
{
    public:
        npc_highmountain_image_of_mimiron() : CreatureScript("npc_highmountain_image_of_mimiron") { }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_highmountain_image_of_mimironAI(p_Creature);
        }

        enum eScriptedTexts
        {
            SayInventions   = 0,
            SayGiveParts    = 1,
            SayBringParts   = 2
        };

        enum eQuests
        {
            ANewInvention   = 43370
        };

        enum eCredits
        {
            CreditPartsDelivered    = 110447,
            CreditStareInAwe        = 110424
        };

        enum eGossips
        {
            GossipMain  = 110424,
            GossipAdd1  = 110425,
            GossipAdd2  = 110426
        };

        enum eMenuItemIds
        {
            MenuItemGeneral   = 0,
            MenuItemHatiBack  = 1
        };

        enum eMenuItemActions
        {
            MenuItemActionGeneral   = 1,
            MenuItemActionHati      = 2
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (p_Player->GetActiveSpecializationID() != SpecIndex::SPEC_HUNTER_BEASTMASTERY)
                return true;

            p_Player->PrepareQuestMenu(p_Creature->GetGUID());

            p_Player->PlayerTalkClass->GetGossipMenu().AddMenuItem(eGossips::GossipMain, eMenuItemIds::MenuItemGeneral, GOSSIP_SENDER_MAIN, eMenuItemActions::MenuItemActionGeneral);
            p_Player->PlayerTalkClass->GetGossipMenu().AddGossipMenuItemData(eMenuItemIds::MenuItemGeneral, eGossips::GossipAdd1, 0);

            if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::EssenceSwapperState) != 0)
            {
                p_Player->PlayerTalkClass->GetGossipMenu().AddMenuItem(eGossips::GossipMain, eMenuItemIds::MenuItemHatiBack, GOSSIP_SENDER_MAIN, eMenuItemActions::MenuItemActionHati);
            }

            p_Player->PlayerTalkClass->SendGossipMenu(eGossips::GossipMain, p_Creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 p_Sender, uint32 p_Action) override
        {
            if (p_Sender == GOSSIP_SENDER_MAIN && p_Action == eMenuItemActions::MenuItemActionHati)
            {
                p_Player->SetCharacterWorldState(CharacterWorldStates::EssenceSwapperState, 0);
                p_Player->SetCharacterWorldState(CharacterWorldStates::EssenceSwapperEntry, 0);
                p_Player->PlayerTalkClass->SendCloseGossip();
                return true;
            }

            return false;
        }

        struct npc_highmountain_image_of_mimironAI : public ScriptedAI
        {
            npc_highmountain_image_of_mimironAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_QUESTGIVER);
                me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_GOSSIP);
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who)
                    return;

                Player* l_Player = p_Who->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->GetQuestStatus(eQuests::ANewInvention) != QuestStatus::QUEST_STATUS_INCOMPLETE)
                    return;

                if (m_DeliveredPartsPlayers.find(l_Player->GetGUID()) != m_DeliveredPartsPlayers.end())
                    return;

                if (l_Player->GetQuestObjectiveCounter(eObjectiveIds::StormyGland) > 0 &&
                    l_Player->GetQuestObjectiveCounter(eObjectiveIds::PristineHarpyFeather) > 0 &&
                    l_Player->GetQuestObjectiveCounter(eObjectiveIds::ReflectiveScale) > 0 &&
                    l_Player->GetQuestObjectiveCounter(eObjectiveIds::EssenceOfPureSpirit) > 0 &&
                    l_Player->GetQuestObjectiveCounter(eObjectiveIds::PartsDelivered) == 0)
                {
                    m_DeliveredPartsPlayers[l_Player->GetGUID()] = true;

                    Talk(eScriptedTexts::SayGiveParts);
                }
            }

            void DoAction(const int32 p_Action) override
            {
                if (p_Action == eActions::ActionPartDelivered)
                {
                    Talk(eScriptedTexts::SayBringParts);
                }
            }

        private:

            std::map<uint64, bool> m_DeliveredPartsPlayers;
        };
};

/// XR9-47 - 110447
class npc_highmountain_XR9_47 : public CreatureScript
{
    public:
        npc_highmountain_XR9_47() : CreatureScript("npc_highmountain_XR9_47") { }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_highmountain_XR9_47AI(p_Creature);
        }

        enum eSpells
        {
            GivingParts = 219662
        };

        enum eNpcs
        {
            ImageOfMimiron  = 110424
        };

        enum eCredits
        {
            CreditStareInAwe = 110424
        };

        struct npc_highmountain_XR9_47AI : public ScriptedAI
        {
            npc_highmountain_XR9_47AI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                InitFlags();
            }

            void Reset() override
            {
                InitFlags();
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (Player* l_Player = p_Clicker->ToPlayer())
                {
                    if (l_Player->GetQuestObjectiveCounter(eObjectiveIds::StormyGland) > 0 &&
                        l_Player->GetQuestObjectiveCounter(eObjectiveIds::PristineHarpyFeather) > 0 &&
                        l_Player->GetQuestObjectiveCounter(eObjectiveIds::ReflectiveScale) > 0 &&
                        l_Player->GetQuestObjectiveCounter(eObjectiveIds::EssenceOfPureSpirit) > 0 &&
                        l_Player->GetQuestObjectiveCounter(eObjectiveIds::PartsDelivered) == 0)
                    {
                        l_Player->CastSpell(l_Player, eSpells::GivingParts, true);

                        l_Player->KilledMonsterCredit(eCredits::CreditStareInAwe);

                        if (Creature* l_Mimiron = GetMimiron())
                            l_Mimiron->AI()->DoAction(eActions::ActionPartDelivered);
                    }
                }
            }

        private:

            Creature* GetMimiron()
            {
                return me->FindNearestCreature(eNpcs::ImageOfMimiron, 10.0f);
            }

            void InitFlags()
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_QUESTGIVER);
                me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);
            }
        };
};

#ifndef __clang_analyzer__
void AddSC_highmountain()
{
    /// Npc
    new npc_highmountain_image_of_mimiron();
    new npc_highmountain_XR9_47();
}
#endif
