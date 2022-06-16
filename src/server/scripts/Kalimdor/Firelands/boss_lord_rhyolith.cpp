////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "firelands.h"
#include "boss_lord_rhyolith.h"

enum ScriptedTexts
{
    SAY_AGGRO   = 0,
    SAY_ARMOR   = 1,
    SAY_DEATH   = 2,
    SAY_KILL    = 3,
    SAY_LAVA    = 4,
    SAY_STOMP   = 5,
    SAY_TRANS   = 6,
};

enum Spells
{
    // Lord Rhyolith
    SPELL_OBSIDIAN_ARMOR                    = 98632,
    SPELL_CONCLUSIVE_STOMP                  = 97282,
    SPELL_VOLCANIC_BIRTH                    = 98010,
    SPELL_FLAME_OF_LIFE                     = 98045,
    SPELL_HEATED_VOLCANO                    = 98493,
    SPELL_LAVA_LINE                         = 100650,
    SPELL_DRINK_MAGMA                       = 98034,
    SPELL_SUPERHEATED                       = 101304,
    SPELL_MOLTEN_SPEW                       = 98043,
    SPELL_SUMMON_FRAGMENT_OF_RHYOLITH       = 98135,
    SPELL_SUMMON_FRAGMENT_OF_RHYOLITH_SUM   = 98136,
    SPELL_SUMMON_FRAGMENT_OF_RHYOLITH_SUM_H = 100392,
    SPELL_SUMMON_SPARK_OF_RHYOLITH          = 98553,
    SPELL_SUMMON_SPARK_OF_RHYOLITH_SUM      = 98552,
    SPELL_IMMOLATION                        = 99846,
    SPELL_BURNING_FEET                      = 98837,
    SPELL_RIDE_VEHICLE                      = 98843,
    SPELL_BALANCE_BAR                       = 98226,
    SPELL_MOLTEN_ARMOR                      = 98255,

    // Volcano
    SPELL_EXPLODE                           = 97719, // 46419 ?
    SPELL_VOLCANO_SMOKE                     = 97699,
    SPELL_ERUPTION                          = 98264,
    SPELL_LAVA_STRIKE                       = 98491,
    SPELL_ERUPTION_DMG                      = 98492,

    // Crater
    SPELL_MAGMA_FLOW                        = 97225, // aura
    SPELL_MAGMA_FLOW_AREA                   = 97230,
    SPELL_MAGMA_FLOW_DMG                    = 97234,
    SPELL_MAGMA                             = 98472,
    SPELL_LAVA_TUBE                         = 98265,

    // Spark of Rhyolith
    SPELL_IMMOLATION_SPARK                  = 98597,
    SPELL_INFERNAL_RAGE                     = 98596,

    // Fragment of Rhyolith
    SPELL_MELTDOWN                          = 98646,

    // Liquid Obsidian
    SPELL_FUSE                              = 99875,
};

enum Adds
{
    NPC_SPARK_OF_RHYOLITH       = 53211,
    NPC_FRAGMENT_OF_RHYOLITH    = 52620,
    NPC_CRATER                  = 52866,
    NPC_LIQUID_OBSIDIAN         = 52619,
    NPC_VOLCANO                 = 52582,
    NPC_LEFT_FOOT               = 52577, 
    NPC_RIGHT_FOOT              = 53087, 
    NPC_RHYOLITH_2              = 53772,
    NPC_MOVEMENT_CONTROLLER     = 52659,
};

enum Events
{
    EVENT_CHECK_MOVE        = 1,
    EVENT_CONCLUSIVE_STOMP  = 2,
    EVENT_ACTIVATE_VOLCANO  = 3,
    EVENT_CHECK_RHYOLITH    = 4, // check if the boss is near for adds
    EVENT_MAGMA_FLOW        = 5, // tube
    EVENT_MAGMA_FLOW_1      = 6, // spawn lines
    EVENT_MAGMA_FLOW_2      = 7, // despawn lines
    EVENT_START_MOVE        = 8, // start moving for adds
    EVENT_INFERNAL_RAGE     = 9,
    EVENT_SUPERHEATED       = 10,
    EVENT_FRAGMENT          = 11,
    EVENT_SPARK             = 12,
    EVENT_ERUPTION          = 13,
};

enum Others
{
    DATA_PHASE                      = 1,
    DATA_HITS                       = 2,
    ACTION_ADD_MOLTEN_ARMOR         = 3,
    ACTION_REMOVE_MOLTEN_ARMOR      = 4,
    ACTION_ADD_OBSIDIAN_ARMOR       = 5,
    ACTION_REMOVE_OBSIDIAN_ARMOR    = 6,
    DATA_ACHIEVE                    = 7,
};

class boss_lord_rhyolith : public CreatureScript
{
    public:
        boss_lord_rhyolith() : CreatureScript("boss_lord_rhyolith") { }

        CreatureAI* GetAI(Creature* pCreature) const override
        {
            return new boss_lord_rhyolithAI(pCreature);
        }

        struct boss_lord_rhyolithAI : public BossAI
        {
            boss_lord_rhyolithAI(Creature* pCreature) : BossAI(pCreature, DATA_RHYOLITH)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                me->SetSpeed(MOVE_RUN, 0.3f, true);
                me->SetSpeed(MOVE_WALK, 0.3f, true);
                m_Controller = 0;
                m_RightFoot = 0;
                m_LeftFoot = 0;
                curMove = 0;
                bAchieve = true;
                players_count = 0;
            }

            void InitializeAI() override
            {
                if (!me->isDead())
                    Reset();
            }

            void Reset() override
            {
                _Reset();

                me->SetHealth(me->GetMaxHealth());
                me->SetReactState(REACT_PASSIVE);
                summons.DespawnEntry(NPC_VOLCANO);

                if (instance && instance->GetBossState(DATA_RHYOLITH) != DONE)
                    me->SetVisible(true);
                else
                    me->DespawnOrUnsummon();

                curMove = 0;
                bAchieve = true;
                players_count = 0;
                phase = 0;

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
                    instance->SetData(DATA_RHYOLITH_HEALTH_SHARED, me->GetMaxHealth() / 2);
                }
            }

            void EnterEvadeMode() override
            {
                if (instance)
                {
                    instance->SetBossState(DATA_RHYOLITH, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
                }

                BossAI::EnterEvadeMode();
            }

            void DamageTaken(Unit* /*who*/, uint32 &damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                damage = 0;
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() != NPC_RHYOLITH_2)
                    BossAI::JustSummoned(summon);
            }

            void EnterCombat(Unit* /*attacker*/) override
            {
                Talk(SAY_AGGRO);
                
                curMove = 0;
                bAchieve = true;
                phase = 0;
                players_count = instance ? instance->instance->GetPlayers().getSize() : 0;

                if (Creature* l_Controller = me->SummonCreature(NPC_MOVEMENT_CONTROLLER, movePos[curMove]))
                    m_Controller = l_Controller->GetGUID();

                if (Creature* l_RightFoot = me->FindNearestCreature(NPC_RIGHT_FOOT, 100.0f))
                    m_RightFoot = l_RightFoot->GetGUID();

                if (Creature* l_LeftFoot = me->FindNearestCreature(NPC_LEFT_FOOT, 100.0f))
                    m_LeftFoot = l_LeftFoot->GetGUID();

                if (Creature* l_Controller = Creature::GetCreature(*me, m_Controller))
                {
                    me->SetSpeed(MOVE_RUN, 0.3f, true);
                    me->SetSpeed(MOVE_WALK, 0.3f, true);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MoveFollow(l_Controller, 0.0f, 0.0f);
                }

                if (instance)
                    instance->SetData(DATA_RHYOLITH_HEALTH_SHARED, me->GetMaxHealth() / 2);

                events.ScheduleEvent(EVENT_CHECK_MOVE, 1000);
                events.ScheduleEvent(EVENT_CONCLUSIVE_STOMP, 10000);
                events.ScheduleEvent(EVENT_ACTIVATE_VOLCANO, urand(25000, 30000));
                events.ScheduleEvent(EVENT_FRAGMENT, urand(25000, 30000));
                events.ScheduleEvent(EVENT_SUPERHEATED, (IsHeroic() ? 5 * MINUTE * IN_MILLISECONDS : 6 * MINUTE * IN_MILLISECONDS));

                DoCastAOE(SPELL_BALANCE_BAR, true);

                DoZoneInCombat();

                if (instance)
                    instance->SetBossState(DATA_RHYOLITH, IN_PROGRESS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (instance)
                    instance->SetBossState(DATA_RHYOLITH, DONE);

                _JustDied();
            }

            void JustReachedHome() override
            {
                me->GetVehicleKit()->InstallAllAccessories(false);

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
            }

            void DoAction(const int32 action) override
            {
                switch(action)
                {
                    case ACTION_ADD_MOLTEN_ARMOR:
                        me->AddAura(SPELL_MOLTEN_ARMOR, me);
                        if (Creature* l_LeftFoot = Creature::GetCreature(*me, m_LeftFoot))
                            l_LeftFoot->AddAura(SPELL_MOLTEN_ARMOR, l_LeftFoot);
                        if (Creature* l_RightFoot = Creature::GetCreature(*me, m_RightFoot))
                            l_RightFoot->AddAura(SPELL_MOLTEN_ARMOR, l_RightFoot);
                        break;
                    case ACTION_REMOVE_MOLTEN_ARMOR:
                        me->RemoveAuraFromStack(SPELL_MOLTEN_ARMOR);
                        if (Creature* l_LeftFoot = Creature::GetCreature(*me, m_LeftFoot))
                            l_LeftFoot->RemoveAuraFromStack(SPELL_MOLTEN_ARMOR);
                        if (Creature* l_RightFoot = Creature::GetCreature(*me, m_RightFoot))
                            l_RightFoot->RemoveAuraFromStack(SPELL_MOLTEN_ARMOR);
                        break;
                    case ACTION_ADD_OBSIDIAN_ARMOR:
                        if (Creature* l_LeftFoot = Creature::GetCreature(*me, m_LeftFoot))
                            l_LeftFoot->CastSpell(l_LeftFoot, SPELL_OBSIDIAN_ARMOR, true);
                        if (Creature* l_RightFoot = Creature::GetCreature(*me, m_RightFoot))
                            l_RightFoot->CastSpell(l_RightFoot, SPELL_OBSIDIAN_ARMOR, true);
                        break;
                    case ACTION_REMOVE_OBSIDIAN_ARMOR:
                        if (Creature* l_LeftFoot = Creature::GetCreature(*me, m_LeftFoot))
                            l_LeftFoot->RemoveAuraFromStack(SPELL_OBSIDIAN_ARMOR, 0, AURA_REMOVE_BY_DEFAULT);
                        if (Creature* l_RightFoot = Creature::GetCreature(*me, m_RightFoot))
                            l_RightFoot->RemoveAuraFromStack(SPELL_OBSIDIAN_ARMOR, 0, AURA_REMOVE_BY_DEFAULT);
                        break;
                }
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (instance && (instance->GetData(DATA_RHYOLITH_HEALTH_SHARED) != 0))
                    me->SetHealth(instance->GetData(DATA_RHYOLITH_HEALTH_SHARED) * 2);

                if (me->HealthBelowPct(25) && phase == 0)
                {
                    phase = 1;
                    me->StopMoving();
                    events.Reset();

                    uint32 _health = me->GetHealth();
                    
                    if (Creature* pRhyolith = me->SummonCreature(NPC_RHYOLITH_2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                    {
                        pRhyolith->RemoveAllAuras();
                        pRhyolith->SetHealth(_health);
                        pRhyolith->CastSpell(pRhyolith, SPELL_IMMOLATION, true);
                        pRhyolith->AI()->SetData(DATA_ACHIEVE, uint32(bAchieve));
                    }

                    summons.DespawnEntry(NPC_VOLCANO);
                    summons.DespawnEntry(NPC_LIQUID_OBSIDIAN);
                    me->AttackStop();
                    me->SetVisible(false);
                    events.Reset();

                    Talk(SAY_TRANS);

                    if (instance)
                    {
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ERUPTION_DMG);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
                    }
                    
                    if (Creature* l_Controller = Creature::GetCreature(*me, m_Controller))
                    {
                        l_Controller->DespawnOrUnsummon();
                        m_Controller = 0;
                    }

                    if (Creature* l_LeftFoot = Creature::GetCreature(*me, m_LeftFoot))
                    {
                        l_LeftFoot->DespawnOrUnsummon();
                        m_LeftFoot = 0;
                    }

                    if (Creature* l_RightFoot = Creature::GetCreature(*me, m_RightFoot))
                    {
                        l_RightFoot->DespawnOrUnsummon();
                        m_RightFoot = 0;
                    }     

                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_MOVE:
                        {
                            if (Creature* l_Controller = Creature::GetCreature(*me, m_Controller))
                            {
                                if (me->GetDistance2d(l_Controller) <= 0.5f)
                                {
                                    me->StopMoving();
                                    me->CastSpell(me, SPELL_DRINK_MAGMA);
                                    events.Reset();
                                    events.ScheduleEvent(EVENT_CHECK_MOVE, 8000);
                                    return;
                                }
                            }

                            Creature* l_Controller = Creature::GetCreature(*me, m_Controller);
                            Creature* l_RightFoot = Creature::GetCreature(*me, m_RightFoot);
                            Creature* l_LeftFoot = Creature::GetCreature(*me, m_LeftFoot);

                            if (l_Controller && l_LeftFoot && l_LeftFoot->IsAIEnabled && l_RightFoot && l_RightFoot->IsAIEnabled)
                            {
                                int32 l_dmg = l_LeftFoot->AI()->GetData(DATA_HITS);
                                int32 r_dmg = l_RightFoot->AI()->GetData(DATA_HITS);

                                if (!l_dmg && !r_dmg && instance)
                                {
                                    //me->RemoveAura(SPELL_BURNING_FEET);
                                    l_LeftFoot->RemoveAura(SPELL_BURNING_FEET);
                                    l_RightFoot->RemoveAura(SPELL_BURNING_FEET);
                                    instance->DoSetAlternatePowerOnPlayers(25);
                                    events.ScheduleEvent(EVENT_CHECK_MOVE, 1000);
                                    return;
                                }

                                int32 i = NormalizeMove(curMove, CalculateNextMove(curMove, l_dmg, r_dmg));
                                if (i < 0)
                                    bAchieve = false;

                                if (i != curMove)
                                {
                                    //me->RemoveAura(SPELL_BURNING_FEET);
                                    l_LeftFoot->RemoveAura(SPELL_BURNING_FEET);
                                    l_RightFoot->RemoveAura(SPELL_BURNING_FEET);
                                    curMove = i;
                                    l_Controller->NearTeleportTo(movePos[curMove].GetPositionX(), movePos[curMove].GetPositionY(), movePos[curMove].GetPositionZ(), 0.0f);
                                }
                                else
                                {
                                    //DoCast(me, SPELL_BURNING_FEET, true);
                                    l_LeftFoot->CastSpell(l_LeftFoot, SPELL_BURNING_FEET, true);
                                    l_RightFoot->CastSpell(l_RightFoot, SPELL_BURNING_FEET, true);
                                }
                            }
                            events.ScheduleEvent(EVENT_CHECK_MOVE, 1000);
                            break;
                        }
                        case EVENT_CONCLUSIVE_STOMP:
                            Talk(SAY_STOMP);
                            DoCastAOE(SPELL_CONCLUSIVE_STOMP);
                            events.ScheduleEvent(EVENT_CONCLUSIVE_STOMP, urand(35000, 40000));
                            break;
                        case EVENT_ACTIVATE_VOLCANO:
                        {
                            std::list<Creature*> volcanos;
                            Creature* pTarget = NULL;
                            me->GetCreatureListWithEntryInGrid(volcanos, NPC_VOLCANO, 300.0f);
                            
                            if (!volcanos.empty())
                                volcanos.remove_if(AuraCheck(SPELL_ERUPTION));

                            if (!volcanos.empty())
                            {
                                std::list<Creature*> volcanos_1;
                                for (std::list<Creature*>::const_iterator itr = volcanos.begin(); itr != volcanos.end(); ++itr)
                                {
                                    if (me->HasInArc(M_PI / 2, (*itr)))
                                        volcanos_1.push_back((*itr));
                                }
                                pTarget = JadeCore::Containers::SelectRandomContainerElement((volcanos_1.empty() ? volcanos : volcanos_1));
                            }
                            if (pTarget)
                            {
                                Talk(SAY_LAVA);
                                DoCast(pTarget, SPELL_HEATED_VOLCANO, true);
                            }
                            events.ScheduleEvent(EVENT_ACTIVATE_VOLCANO, urand(45000, 50000));
                            break;
                        }
                        case EVENT_SPARK:
                        {
                            uint32 i = urand(0, _MAX_VOLCANO - 1);
                            me->CastSpell(volcanoPos[i].GetPositionX(), volcanoPos[i].GetPositionY(), volcanoPos[i].GetPositionZ(), SPELL_SUMMON_SPARK_OF_RHYOLITH, true);
                            events.ScheduleEvent(EVENT_FRAGMENT, 30000);
                            break;
                        }
                        case EVENT_FRAGMENT:
                        {
                            std::set<uint8> posList;
                            uint8 max_size = 5;

                            while (posList.size() < max_size)
                            {
                                uint8 i = urand(0, _MAX_VOLCANO - 1);
                                
                                if (posList.find(i) == posList.end())
                                    posList.insert(i);
                            }

                            for (std::set<uint8>::const_iterator itr = posList.begin(); itr != posList.end(); ++itr)
                                me->CastSpell(volcanoPos[(*itr)].GetPositionX(), volcanoPos[(*itr)].GetPositionY(), volcanoPos[(*itr)].GetPositionZ(), SPELL_SUMMON_FRAGMENT_OF_RHYOLITH, true);
                            
                            events.ScheduleEvent(EVENT_SPARK, 30000);
                            break;
                        }
                        case EVENT_SUPERHEATED:
                            DoCast(me, SPELL_SUPERHEATED, true);
                            events.ScheduleEvent(EVENT_SUPERHEATED, 10000);
                            break;
                        case EVENT_START_MOVE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            uint64 m_Controller;
            uint64 m_RightFoot;
            uint64 m_LeftFoot;
            int32 curMove;
            bool bAchieve;
            uint8 players_count;
            uint8 phase;

            int32 CalculateNextMove(int32 /*cur*/, int32 left, int32 right)
            {
                int32 diff = right - left;
                
                if (diff == 0)
                    return 0;
                else if (diff > 0 && diff < 3)
                    return 0;
                else if (diff < 0 && diff > -3)
                    return 0;

                diff /= 2;
                if (diff > 6)
                    diff = 6;
                else if (diff < -6)
                    diff = -6;

                float step = 4.1666666f;
                int32 energy = int32((diff + 6) * step);
                if (energy < 0)
                    energy = 0;
                if (energy > 50)
                    energy = 50;

                if (instance)
                    instance->DoSetAlternatePowerOnPlayers(energy);

                return int32(diff); 
            }

            int32 NormalizeMove(int32 cur, int32 diff)
            {
                int32 new_move = cur + diff;
                if (new_move >= 0 && new_move < 192)
                    return new_move;
                else if (new_move < 0)
                    return (191 + new_move);
                else if (new_move >= 192)
                    return (new_move - 191);
                return cur;
            }

            class AuraCheck
            {
                public:
                    AuraCheck(uint32 spellId) : _spellId(spellId) {}
        
                    bool operator()(Creature* volcano)
                    {
                       return (volcano->HasAura(_spellId));
                    }
                private:
                    uint32 _spellId;
            };
        };
};

class npc_lord_rhyolith_rhyolith : public CreatureScript
{
    public:
        npc_lord_rhyolith_rhyolith() : CreatureScript("npc_lord_rhyolith_rhyolith") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_rhyolithAI(pCreature);
        }

        struct npc_lord_rhyolith_rhyolithAI : public ScriptedAI
        {
            npc_lord_rhyolith_rhyolithAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                pInstance = me->GetInstanceScript();
            }

            void InitializeAI() override
            {
                if (!pInstance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(FLScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void Reset() override
            {
                events.Reset();
            }

            void EnterEvadeMode() override
            {
                me->DespawnOrUnsummon();
            }

            void EnterCombat(Unit* /*attacker*/) override
            {
                events.ScheduleEvent(EVENT_CONCLUSIVE_STOMP, 10000);

                DoZoneInCombat();
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_ACHIEVE)
                    bAchieve = data;
            }

            bool AllowAchieve()
            {
                return bAchieve;
            }

            void JustDied(Unit* killer) override
            {
                if (pInstance)
                {
                    if (Creature* pRhyolith = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_RHYOLITH)))
                        killer->Kill(pRhyolith);
                    pInstance->SetBossState(DATA_RHYOLITH, DONE);
                    pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
                }
                Talk(SAY_DEATH);

                AddSmoulderingAura(me);
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CONCLUSIVE_STOMP:
                            Talk(SAY_STOMP);
                            DoCastAOE(SPELL_CONCLUSIVE_STOMP);
                            events.ScheduleEvent(EVENT_CONCLUSIVE_STOMP, urand(35000, 40000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            InstanceScript* pInstance;
            EventMap events;
            bool bAchieve;
        };
};

class npc_lord_rhyolith_right_foot : public CreatureScript
{
    public:
        npc_lord_rhyolith_right_foot() : CreatureScript("npc_lord_rhyolith_right_foot") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_right_footAI(pCreature);
        }

        struct npc_lord_rhyolith_right_footAI : public Scripted_NoMovementAI
        {
            npc_lord_rhyolith_right_footAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                pInstance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                me->CastCustomSpell(SPELL_OBSIDIAN_ARMOR, SPELLVALUE_AURA_STACK, 80, me, true);
                memset(m_hits, 0, sizeof(m_hits));
                hitsTimer = 1000;
            }

            void Reset() override
            {
                memset(m_hits, 0, sizeof(m_hits));
                hitsTimer = 1000;
            }

            void EnterCombat(Unit* /*who*/) override
            {
                if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 300.0f))
                    DoZoneInCombat(pRhyolith);
                if (Creature* pLeftFoot = me->FindNearestCreature(NPC_LEFT_FOOT, 300.0f))
                    DoZoneInCombat(pLeftFoot);
            }

            uint32 GetData(uint32 type) override
            {
                if (type == DATA_HITS)
                    return GetHits();
                return 0;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 300.0f))
                    DoZoneInCombat(pRhyolith);
                if (Creature* pRightFoot = me->FindNearestCreature(NPC_RIGHT_FOOT, 300.0f))
                    DoZoneInCombat(pRightFoot);

                if (pInstance)
                    pInstance->SetData(DATA_RHYOLITH_HEALTH_SHARED, 0);
            }

            void DamageTaken(Unit* who, uint32 &damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!me || !me->isAlive())
                    return;

                if (who->GetGUID() == me->GetGUID())
                    return;

                m_hits[0]++;

                if (pInstance)
                    pInstance->SetData(DATA_RHYOLITH_HEALTH_SHARED, me->GetHealth() > damage ? me->GetHealth() - damage : 0);
            }

            void UpdateAI(const uint32 diff) override
            {
                me->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);

                if (!UpdateVictim())
                    return;

                hitsTimer  -= diff;

                if (hitsTimer <= 0)
                {
                     ResetHits();
                     hitsTimer = 1000;
                }

                if (pInstance)
                    if (pInstance->GetData(DATA_RHYOLITH_HEALTH_SHARED) != 0)
                        me->SetHealth(pInstance->GetData(DATA_RHYOLITH_HEALTH_SHARED));
            }
        private:
            InstanceScript* pInstance;
            uint32 m_hits[3];
            int32 hitsTimer;

            void ResetHits()
            {
                for (uint8 i = 3 - 1;  i > 0; i--)
                    m_hits[i] = m_hits[i - 1];

                m_hits[0] = 0;
            }

            uint32 GetHits()
            {
                uint32 value = 0;

                for (uint8 i = 0; i < 3; ++i)
                     value += m_hits[i];

                return value;
            }
        };
};

class npc_lord_rhyolith_left_foot : public CreatureScript
{
    public:
        npc_lord_rhyolith_left_foot() : CreatureScript("npc_lord_rhyolith_left_foot") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_left_footAI(pCreature);
        }

        struct npc_lord_rhyolith_left_footAI : public Scripted_NoMovementAI
        {
            npc_lord_rhyolith_left_footAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                pInstance = me->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
                me->CastCustomSpell(SPELL_OBSIDIAN_ARMOR, SPELLVALUE_AURA_STACK, 80, me, true);
                memset(m_hits, 0, sizeof(m_hits));
                hitsTimer = 1000;
            }

            void Reset() override
            {
                memset(m_hits, 0, sizeof(m_hits));
                hitsTimer = 1000;
            }

            void EnterCombat(Unit* /*who*/) override
            {
                if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 300.0f))
                    DoZoneInCombat(pRhyolith);
                if (Creature* pRightFoot = me->FindNearestCreature(NPC_RIGHT_FOOT, 300.0f))
                    DoZoneInCombat(pRightFoot);
            }

            uint32 GetData(uint32 type) override
            {
                if (type == DATA_HITS)
                    return GetHits();
                return 0;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 300.0f))
                    DoZoneInCombat(pRhyolith);
                if (Creature* pRightFoot = me->FindNearestCreature(NPC_RIGHT_FOOT, 300.0f))
                    DoZoneInCombat(pRightFoot);

                if (pInstance)
                    pInstance->SetData(DATA_RHYOLITH_HEALTH_SHARED, 0);
            }

            void DamageTaken(Unit* who, uint32 &damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (!me || !me->isAlive())
                    return;

                if (who->GetGUID() == me->GetGUID())
                    return;

                m_hits[0]++;

                if (pInstance)
                    pInstance->SetData(DATA_RHYOLITH_HEALTH_SHARED, me->GetHealth() > damage ? me->GetHealth() - damage : 0);
            }

            void UpdateAI(const uint32 diff) override
            {
                me->ClearUnitState(UnitState::UNIT_STATE_UNATTACKABLE);

                if (!UpdateVictim())
                    return;

                hitsTimer  -= diff;

                if (hitsTimer <= 0)
                {
                     ResetHits();
                     hitsTimer = 1000;
                }

                if (pInstance)
                    if (pInstance->GetData(DATA_RHYOLITH_HEALTH_SHARED) != 0)
                        me->SetHealth(pInstance->GetData(DATA_RHYOLITH_HEALTH_SHARED));
            }
        private:
            InstanceScript* pInstance;
            uint32 m_hits[3];
            int32 hitsTimer;

            void ResetHits()
            {
                for (uint8 i = 3 - 1;  i > 0; i--)
                    m_hits[i] = m_hits[i - 1];

                m_hits[0] = 0;
            }

            uint32 GetHits()
            {
                uint32 value = 0;

                for (uint8 i = 0; i < 3; ++i)
                     value += m_hits[i];

                return value;
            }
        };
};

class npc_lord_rhyolith_volcano : public CreatureScript
{
    public:
        npc_lord_rhyolith_volcano() : CreatureScript("npc_lord_rhyolith_volcano") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_volcanoAI(pCreature);
        }

        struct npc_lord_rhyolith_volcanoAI : public Scripted_NoMovementAI
        {
            npc_lord_rhyolith_volcanoAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                DoCast(me, SPELL_VOLCANO_SMOKE, true);
                events.ScheduleEvent(EVENT_CHECK_RHYOLITH, 3000);
            }

            void SpellHit(Unit* /*who*/, const SpellInfo* spellInfo) override
            {
                if (spellInfo->Id == SPELL_HEATED_VOLCANO)
                {
                    me->RemoveAura(SPELL_VOLCANO_SMOKE);
                    events.ScheduleEvent(EVENT_ERUPTION, 3000);
                }
            }

            void UpdateAI(const uint32 diff)  override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ERUPTION:
                            DoCast(me, SPELL_ERUPTION, true);
                            break;
                        case EVENT_CHECK_RHYOLITH:
                            if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 5.0f, true))
                            {
                                if (pRhyolith->IsInEvadeMode() || (pRhyolith->AI()->GetData(DATA_PHASE) != 0))
                                    return;

                                uint32 k = urand(0, _MAX_VOLCANO - 1);
                                if (Unit* pTarget = pRhyolith->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 300.0f, true))
                                    if (Player* pPlayer = pTarget->ToPlayer())
                                        pPlayer->SummonGameObject(GO_RHYOLITH_FRAGMENT, volcanoPos[k].GetPositionX(), volcanoPos[k].GetPositionY(), volcanoPos[k].GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, MINUTE * IN_MILLISECONDS);  

                                pRhyolith->AI()->DoAction(ACTION_ADD_MOLTEN_ARMOR);
                                pRhyolith->AI()->DoAction(ACTION_REMOVE_OBSIDIAN_ARMOR);
                                pRhyolith->SummonCreature(NPC_CRATER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
                                if (IsHeroic())
                                {
                                    std::set<uint8> posList;
                                    uint8 max_size = 5;

                                    while (posList.size() < max_size)
                                    {
                                        uint8 i = urand(0, 191);
                                        if (pRhyolith->GetDistance2d(movePos[i].GetPositionX(), movePos[i].GetPositionY()) <= 10.0f)
                                            continue;

                                        if (posList.find(i) == posList.end())
                                            posList.insert(i);
                                    }

                                    for (std::set<uint8>::const_iterator itr = posList.begin(); itr != posList.end(); ++itr)
                                        pRhyolith->SummonCreature(NPC_LIQUID_OBSIDIAN, movePos[(*itr)]);
                                }
                                
                                me->DespawnOrUnsummon(500);
                            }
                            else
                                events.ScheduleEvent(EVENT_CHECK_RHYOLITH, 1000);
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class npc_lord_rhyolith_crater : public CreatureScript
{
    public:
        npc_lord_rhyolith_crater() : CreatureScript("npc_lord_rhyolith_crater") { }

        CreatureAI* GetAI(Creature* pCreature) const override
        {
            return new npc_lord_rhyolith_craterAI(pCreature);
        }

        struct npc_lord_rhyolith_craterAI : public Scripted_NoMovementAI
        {
            npc_lord_rhyolith_craterAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                DoCast(me, SPELL_EXPLODE, true);
                DoCast(me, SPELL_MAGMA, true);
                events.ScheduleEvent(EVENT_MAGMA_FLOW, 20000);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MAGMA_FLOW:
                            DoCast(me, SPELL_LAVA_TUBE, true);
                            events.ScheduleEvent(EVENT_MAGMA_FLOW_1, 3000);
                            break;
                        case EVENT_MAGMA_FLOW_1:
                            DoCast(me, SPELL_MAGMA_FLOW, true);
                            events.ScheduleEvent(EVENT_MAGMA_FLOW_2, 15000);
                            break;
                        case EVENT_MAGMA_FLOW_2:
                            me->RemoveAura(SPELL_LAVA_TUBE);
                            me->RemoveAllDynObjects();
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class npc_lord_rhyolith_liquid_obsidian : public CreatureScript
{
    public:
        npc_lord_rhyolith_liquid_obsidian() : CreatureScript("npc_lord_rhyolith_liquid_obsidian") { }

        CreatureAI* GetAI(Creature* pCreature) const override
        {
            return new npc_lord_rhyolith_liquid_obsidianAI(pCreature);
        }

        struct npc_lord_rhyolith_liquid_obsidianAI : public ScriptedAI
        {
            npc_lord_rhyolith_liquid_obsidianAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.3f, true);
                me->SetSpeed(MOVE_WALK, 0.3f, true);
            }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_START_MOVE, 2000);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_MOVE:
                            if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 300.0f))
                            {
                                me->GetMotionMaster()->MoveFollow(pRhyolith, 0.0f, 0.0f);
                                events.ScheduleEvent(EVENT_CHECK_RHYOLITH, 1000);
                            }
                            break;
                        case EVENT_CHECK_RHYOLITH:
                            if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 0.5f, true))
                                me->CastSpell(pRhyolith, SPELL_FUSE, true);
                            else
                                events.ScheduleEvent(EVENT_CHECK_RHYOLITH, 1000);
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class npc_lord_rhyolith_spark_of_rhyolith : public CreatureScript
{
    public:
        npc_lord_rhyolith_spark_of_rhyolith() : CreatureScript("npc_lord_rhyolith_spark_of_rhyolith") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_lord_rhyolith_spark_of_rhyolithAI(pCreature);
        }

        struct npc_lord_rhyolith_spark_of_rhyolithAI : public ScriptedAI
        {
            npc_lord_rhyolith_spark_of_rhyolithAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_START_MOVE, 2000);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_MOVE:
                            DoCast(me, SPELL_IMMOLATION_SPARK, true);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                            events.ScheduleEvent(EVENT_INFERNAL_RAGE, 5000);
                            break;
                        case EVENT_INFERNAL_RAGE:
                            DoCast(me, SPELL_INFERNAL_RAGE, true);
                            events.ScheduleEvent(EVENT_INFERNAL_RAGE, 5000);
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class npc_lord_rhyolith_fragment_of_rhyolith : public CreatureScript
{
    public:
        npc_lord_rhyolith_fragment_of_rhyolith() : CreatureScript("npc_lord_rhyolith_fragment_of_rhyolith") { }

        CreatureAI* GetAI(Creature* pCreature) const override
        {
            return new npc_lord_rhyolith_fragment_of_rhyolithAI(pCreature);
        }

        struct npc_lord_rhyolith_fragment_of_rhyolithAI : public ScriptedAI
        {
            npc_lord_rhyolith_fragment_of_rhyolithAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustDied(Unit* /*who*/) override
            {
                if (Creature* pRhyolith = me->FindNearestCreature(NPC_RHYOLITH, 5.0f))
                    pRhyolith->AI()->DoAction(ACTION_REMOVE_MOLTEN_ARMOR);
            }

            void EnterCombat(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_START_MOVE, 2000);
            }

            void MovementInform(uint32 type, uint32 data) override
            {
                if (type == POINT_MOTION_TYPE)
                    if (data == EVENT_CHARGE)
                        me->Kill(me);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
                
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_MOVE:
                            DoCast(me, SPELL_MELTDOWN, true);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                            break;
                    }
                }
            }
        private:
            EventMap events;
        };
};

class spell_lord_rhyolith_conclusive_stomp : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_conclusive_stomp() : SpellScriptLoader("spell_lord_rhyolith_conclusive_stomp") { }


        class spell_lord_rhyolith_conclusive_stomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lord_rhyolith_conclusive_stomp_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* pCreature = GetCaster()->ToCreature())
                    if (pCreature->GetEntry() != NPC_RHYOLITH_2)
                    {
                        std::set<uint8> posList;
                        uint8 max_size = urand(2, 3);

                        while (posList.size() < max_size)
                        {
                            uint8 i = urand(0, 31);
                            if (GetCaster()->GetDistance2d(volcanoPos[i].GetPositionX(), volcanoPos[i].GetPositionY()) <= 5.0f)
                                continue;

                            if (posList.find(i) == posList.end())
                                posList.insert(i);
                        }

                        for (std::set<uint8>::const_iterator itr = posList.begin(); itr != posList.end(); ++itr)
                            GetCaster()->CastSpell(volcanoPos[(*itr)].GetPositionX(), volcanoPos[(*itr)].GetPositionY(), volcanoPos[(*itr)].GetPositionZ(), SPELL_VOLCANIC_BIRTH, true);
                    }
            }

            void HandleAfterCast()
            {
                if (!GetCaster())
                    return;

                std::list<Creature*> creatureList;
                GetCaster()->GetCreatureListWithEntryInGrid(creatureList, NPC_DULL_RHYOLITH_FOCUS, 100.0f);
                if (!creatureList.empty())
                {
                    for (std::list<Creature*>::const_iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
                    {
                        if (Creature* pFocus = (*itr)->ToCreature())
                        {
                            pFocus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            pFocus->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            pFocus->CastSpell(pFocus, SPELL_TRANSFORM_CHARGED_RHYOLITH_FOCUS, true);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_lord_rhyolith_conclusive_stomp_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                AfterCast += SpellCastFn(spell_lord_rhyolith_conclusive_stomp_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lord_rhyolith_conclusive_stomp_SpellScript();
        }
};

class spell_lord_rhyolith_magma_flow : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_magma_flow() : SpellScriptLoader("spell_lord_rhyolith_magma_flow") { }

        class spell_lord_rhyolith_magma_flow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_lord_rhyolith_magma_flow_AuraScript);
            
            bool Load() override
            {
                count = 0;
                add = true;
                return true;
            }

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (!GetCaster())
                    return;

                count++;

                for (float a = 0; a <= 2 * M_PI; a += M_PI / 2)
                {
                    Position pos;
                    GetCaster()->GetNearPosition(pos, 1.0f * count, a + frand(-0.05f, 0.05f));
                    GetCaster()->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_MAGMA_FLOW_AREA, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_lord_rhyolith_magma_flow_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

        private:
            uint8 count;
            bool add;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_lord_rhyolith_magma_flow_AuraScript();
        }
};

class spell_lord_rhyolith_fuse : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_fuse() : SpellScriptLoader("spell_lord_rhyolith_fuse") { }


        class spell_lord_rhyolith_fuse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lord_rhyolith_fuse_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (Creature* pRhyolith = GetHitUnit()->ToCreature())
                        pRhyolith->AI()->DoAction(ACTION_ADD_OBSIDIAN_ARMOR);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_lord_rhyolith_fuse_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lord_rhyolith_fuse_SpellScript();
        }
};

class spell_lord_rhyolith_drink_magma : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_drink_magma() : SpellScriptLoader("spell_lord_rhyolith_drink_magma") { }


        class spell_lord_rhyolith_drink_magma_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lord_rhyolith_drink_magma_SpellScript);

            void HandleDummy()
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), SPELL_MOLTEN_SPEW, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_lord_rhyolith_drink_magma_SpellScript::HandleDummy);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lord_rhyolith_drink_magma_SpellScript();
        }
};

class spell_lord_rhyolith_lava_strike : public SpellScriptLoader
{
    public:
        spell_lord_rhyolith_lava_strike() : SpellScriptLoader("spell_lord_rhyolith_lava_strike") { }


        class spell_lord_rhyolith_lava_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lord_rhyolith_lava_strike_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                uint32 max_size = (GetCaster()->GetMap()->Is25ManRaid() ? 6 : 3);
                if (!targets.empty())
                    JadeCore::Containers::RandomResizeList(targets, max_size);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return; 

                GetCaster()->CastSpell(GetHitUnit(), SPELL_LAVA_STRIKE, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lord_rhyolith_lava_strike_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_lord_rhyolith_lava_strike_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_lord_rhyolith_lava_strike_SpellScript();
        }
};

class achievement_not_an_ambi_turner : public AchievementCriteriaScript
{
    public:
        achievement_not_an_ambi_turner() : AchievementCriteriaScript("achievement_not_an_ambi_turner") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target)
                return false;

            if (npc_lord_rhyolith_rhyolith::npc_lord_rhyolith_rhyolithAI* lord_rhyolithAI = CAST_AI(npc_lord_rhyolith_rhyolith::npc_lord_rhyolith_rhyolithAI, target->GetAI()))
                return lord_rhyolithAI->AllowAchieve();

            return false;
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_lord_rhyolith()
{
    new boss_lord_rhyolith();
    new npc_lord_rhyolith_rhyolith();
    new npc_lord_rhyolith_left_foot();
    new npc_lord_rhyolith_right_foot();
    new npc_lord_rhyolith_volcano();
    new npc_lord_rhyolith_crater();
    new npc_lord_rhyolith_spark_of_rhyolith();
    new npc_lord_rhyolith_fragment_of_rhyolith();
    new npc_lord_rhyolith_liquid_obsidian();
    new spell_lord_rhyolith_conclusive_stomp();
    new spell_lord_rhyolith_magma_flow();
    new spell_lord_rhyolith_fuse();
    new spell_lord_rhyolith_drink_magma();
    new spell_lord_rhyolith_lava_strike();
    new achievement_not_an_ambi_turner();
}
#endif
