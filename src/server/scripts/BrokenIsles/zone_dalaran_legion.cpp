////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "zone_dalaran_legion.h"

/// Forsaken Guard 103626 & Gilnean Guard 108323
class npc_guard_dalaran : public CreatureScript
{
    public:
        npc_guard_dalaran() : CreatureScript("npc_guard_dalaran") { }

        struct npc_guard_dalaranAI : public Scripted_NoMovementAI
        {
            npc_guard_dalaranAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                p_Creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                p_Creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_NORMAL, true);
                p_Creature->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
            }

            void Reset() override { }
            void EnterCombat(Unit* /*p_Who*/) override { }
            void AttackStart(Unit* /*p_Who*/, bool /*p_Melee*/) override { }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who || !p_Who->IsInWorld() || p_Who->GetZoneId() != 7502) ///< Dalaran city zoneID in Broken isle
                    return;

                if (!me->IsWithinDist(p_Who, 65.0f, false) || !me->IsWithinLOSInMap(p_Who))
                    return;

                Player* l_Player = p_Who->GetCharmerOrOwnerPlayerOrPlayerItself();

                /// Game Master aren't trepasser's
                if (!l_Player || l_Player->isGameMaster() || l_Player->IsBeingTeleported())
                    return;

                switch (me->GetEntry())
                {
                    case NPC_GILNEAN_GUARD:
                    {
                        if (l_Player->GetTeam() == HORDE)             ///< Horde unit found in Alliance area
                        {
                            if (me->isInBackInMap(p_Who, 12.0f))  ///< In my line of sight, "outdoors", and behind me
                                p_Who->CastSpell(p_Who, TRESPASSER_H, true);      ///< Teleport the Horde unit out
                        }
                        break;
                    }
                    case NPC_FORSAKEN_GUARD:
                    {
                        if (l_Player->GetTeam() == ALLIANCE)          ///< Alliance unit found in Horde area
                        {
                            if (me->isInBackInMap(p_Who, 12.0f))  ///< In my line of sight, "outdoors", and behind me
                                p_Who->CastSpell(p_Who, TRESPASSER_A, true);      ///< Teleport the Alliance unit out
                        }
                        break;
                    }
                    default:
                        break;
                }

                me->SetOrientation(me->GetHomePosition().GetOrientation());
                return;
            }

            void UpdateAI(const uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_guard_dalaranAI(p_Creature);
        }
};

class npc_legion_artifact_lord : public CreatureScript
{
    public:
        npc_legion_artifact_lord() : CreatureScript("npc_legion_artifact_lord") { }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (!p_Player || p_Player->getLevel() < 98)
                return false;

            static std::map<uint8 /*Class*/, uint32 /*SpellId*/> k_ArtifactChoiceSpells =
            {
                { Classes::CLASS_WARRIOR,         197999 },
                { Classes::CLASS_PALADIN,         197124 },
                { Classes::CLASS_HUNTER,          198430 },
                { Classes::CLASS_ROGUE,           201343 },
                { Classes::CLASS_PRIEST,          199700 },
                { Classes::CLASS_DEATH_KNIGHT,    199985 },
                { Classes::CLASS_SHAMAN,          205790 },
                { Classes::CLASS_MAGE,            203654 },
                { Classes::CLASS_WARLOCK,         199411 },
                { Classes::CLASS_MONK,            198902 },
                { Classes::CLASS_DRUID,           199653 },
                { Classes::CLASS_DEMON_HUNTER,    201092 }
            };

            p_Player->CastSpell(p_Player, k_ArtifactChoiceSpells[p_Player->getClass()], true);

            return true;
        }
};

/// Archmage Lan'dalock - 111243
class npc_dalaran_archmage_landalock : public CreatureScript
{
    public:
        npc_dalaran_archmage_landalock() : CreatureScript("npc_dalaran_archmage_landalock") { }

        enum eData
        {
            MaxWeeklyTokens = 3
        };

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::BrokenIslesWeeklyBonusRollTokenCount) >= 3)
            {
                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());
                return true;
            }

            return false;
        }

        bool OnQuestComplete(Player* p_Player, Creature* /*p_Creature*/, Quest const* /*p_Quest*/) override
        {
            p_Player->PlayerTalkClass->SendCloseGossip();
            p_Player->ModifyCharacterWorldState(CharacterWorldStates::BrokenIslesWeeklyBonusRollTokenCount, 1);
            return true;
        }
};

/// Professor Pallin - 92195
class npc_dalaran_professor_pallin : public CreatureScript
{
    public:
        npc_dalaran_professor_pallin() : CreatureScript("npc_dalaran_professor_pallin") { }

        enum eTexts
        {
            FishPigment = 0
        };

        enum eQuests
        {
            FishInk = 39932
        };

        bool OnQuestComplete(Player* p_Player, Creature* p_Creature, Quest const* p_Quest) override
        {
            p_Player->PlayerTalkClass->SendCloseGossip();

            if (p_Quest->GetQuestId() == eQuests::FishInk)
            {
                p_Creature->AI()->Talk(eTexts::FishPigment);
            }

            return true;
        }
};

class player_script_hidden_appearance_unlock : public PlayerScript
{
    public:
        player_script_hidden_appearance_unlock() : PlayerScript("player_script_hidden_appearance_unlock") { }

        void UnlockAppearance(Player* p_Player, uint32 p_SceneInstanceID)
        {
            if (uint32 l_MiscID = p_Player->GetMiscIDBySceneInstanceID(p_SceneInstanceID))
            {
                if (uint32 const* l_SpellID = sSpellMgr->GetSpellSceneTrigger(l_MiscID))
                    p_Player->CastSpell(p_Player, *l_SpellID, true);
            }
        }

        void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceID) override
        {
            UnlockAppearance(p_Player, p_SceneInstanceID);
        }

        void OnSceneComplete(Player* p_Player, uint32 p_SceneInstanceID) override
        {
            UnlockAppearance(p_Player, p_SceneInstanceID);
        }
};

class player_script_a_challenging_look_unlock : public PlayerScript
{
    public:
        player_script_a_challenging_look_unlock() : PlayerScript("player_script_a_challenging_look_unlock") { }

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

        enum eSpells
        {
            SpellChallengersTriumph     = 234764,
            SpellAcquiringTheApperance  = 229414
        };

        enum eData
        {
            DataSceneMiscID = 1484
        };

        Position const m_JumpPos = { -851.7915f, 4622.6953f, 749.36975f, 1.494558f };

        std::set<uint32> m_Quests =
        {
            eQuests::QuestTheImpMothersGift,
            eQuests::QuestXylemsGift,
            eQuests::QuestTheGodQueensGift,
            eQuests::QuestNavaroggsGift,
            eQuests::QuestKruulsGift,
            eQuests::QuestJarodsGift,
            eQuests::QuestRaestsGift
        };

        void OnQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            if (m_Quests.find(p_Quest->GetQuestId()) == m_Quests.end())
                return;

            p_Player->GetMotionMaster()->Clear();

            p_Player->CastSpell(m_JumpPos, eSpells::SpellChallengersTriumph, true);
        }

        void CompleteKillCredit(Player* p_Player, uint32 p_SceneInstanceID)
        {
            if (uint32 l_MiscID = p_Player->GetMiscIDBySceneInstanceID(p_SceneInstanceID))
            {
                if (l_MiscID == eData::DataSceneMiscID)
                    p_Player->CastSpell(p_Player, eSpells::SpellAcquiringTheApperance, true);
            }
        }

        void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceID) override
        {
            CompleteKillCredit(p_Player, p_SceneInstanceID);
        }

        void OnSceneComplete(Player* p_Player, uint32 p_SceneInstanceID) override
        {
            CompleteKillCredit(p_Player, p_SceneInstanceID);
        }
};

#ifndef __clang_analyzer__
void AddSC_dalaran_legion()
{
    new npc_guard_dalaran();
    new npc_legion_artifact_lord();
    new npc_dalaran_archmage_landalock();
    new npc_dalaran_professor_pallin();

    new player_script_hidden_appearance_unlock();
    new player_script_a_challenging_look_unlock();
}
#endif
