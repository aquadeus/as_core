////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "KalimdorPCH.h"

/// Archmage Xylem - 117252
class npc_azshara_archmage_xylem : public CreatureScript
{
    public:
        npc_azshara_archmage_xylem() : CreatureScript("npc_azshara_archmage_xylem") { }

        enum eQuest
        {
            QuestClosingTheEye = 44925
        };

        enum eCreature
        {
            CreatureJoanna = 116048
        };

        enum eAction
        {
            ActionXylemEnd
        };

        enum eTalk
        {
            TalkEnd2
        };

        struct npc_azshara_archmage_xylemAI : public ScriptedAI
        {
            npc_azshara_archmage_xylemAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            std::set<uint64> m_TriggeredPlayers;

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer() || !p_Who->ToPlayer()->HasQuest(eQuest::QuestClosingTheEye))
                    return;

                uint64 l_Guid = p_Who->GetGUID();

                if (m_TriggeredPlayers.find(l_Guid) != m_TriggeredPlayers.end())
                    return;

                m_TriggeredPlayers.insert(l_Guid);

                if (Creature* l_Joanna = me->FindNearestCreature(eCreature::CreatureJoanna, 20.0f))
                {
                    if (l_Joanna->IsAIEnabled)
                    {
                        l_Joanna->AI()->DoAction(eAction::ActionXylemEnd);

                        AddTimedDelayedOperation(7 * TimeConstants::IN_MILLISECONDS, [this, l_Guid]() -> void
                        {
                            if (Player* l_Player = Player::GetPlayer(*me, l_Guid))
                            {
                                Talk(eTalk::TalkEnd2, l_Player->GetGUID());
                            }
                        });
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_azshara_archmage_xylemAI(p_Creature);
        }
};

/// Joanna <Xylem's Apprentice> - 116048
class npc_azshara_joanna : public CreatureScript
{
    public:
        npc_azshara_joanna() : CreatureScript("npc_azshara_joanna") { }

        enum eAction
        {
            ActionXylemEnd
        };

        enum eTalks
        {
            TalkEnd0,
            TalkEnd1
        };

        struct npc_azshara_joannaAI : public ScriptedAI
        {
            npc_azshara_joannaAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionXylemEnd)
                {
                    Talk(eTalks::TalkEnd0);

                    AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS + 500, [this]() -> void
                    {
                        Talk(eTalks::TalkEnd1);
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_azshara_joannaAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_azshara()
{
    new npc_azshara_archmage_xylem();
    new npc_azshara_joanna();
}
#endif
