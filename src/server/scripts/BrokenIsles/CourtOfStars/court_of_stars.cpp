////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2019 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "court_of_stars.hpp"

enum eSpells
{
    WildDetonation = 209477
};

enum eEvents
{

};

class npc_cos_withered_manawyrm : public CreatureScript
{
    public:
        npc_cos_withered_manawyrm() : CreatureScript("npc_cos_withered_manawyrm"){ }

        struct npc_cos_withered_manawyrmAI : public ScriptedAI
        {
            npc_cos_withered_manawyrmAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Killed = false;
            }

            bool m_Killed;

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                uint64 l_Health = me->GetHealth();

                if (p_Damage > l_Health)
                {
                    if (m_Killed)
                    {
                        p_Damage = 0;
                    }
                    else
                    {
                        m_Killed = true;
                        p_Damage = 0;
                        me->SetHealth(1);
                        me->AttackStop();
                        me->SetReactState(REACT_PASSIVE);
                        DoCast(eSpells::WildDetonation);
                        me->ForcedDespawn(2600);
                    }
                }
            }

            void Reset() override
            {
                m_Killed = false;
                me->SetReactState(REACT_AGGRESSIVE);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!m_Killed)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cos_withered_manawyrmAI(p_Creature);
        }
};

void AddSC_court_of_stars()
{
    new npc_cos_withered_manawyrm();
}