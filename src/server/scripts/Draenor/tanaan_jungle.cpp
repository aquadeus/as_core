////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM - STUDIO
//  Copyright 2016 Millenium - studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////

#include "tanaan_jungle.h"

// @Creature: Archmage Khadgar 78558 - Dark Portal area
class npc_archmage_khadgar_78558 : public CreatureScript
{
    public:
        npc_archmage_khadgar_78558() : CreatureScript("npc_archmage_khadgar_78558") {}
    
        enum eTalk
        {
            SAY_QA_AZEROTHS_LAST_STAND = 0,
            SAY_QA_THE_PORTALS_POWER = 1,
        };
    
        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, Quest const* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case TanaanIntroQuests::QUEST_AZEROTHS_LAST_STAND:
                    if (p_Creature->IsAIEnabled)
                        p_Creature->AI()->PersonalTalk(eTalk::SAY_QA_AZEROTHS_LAST_STAND, p_Player->GetGUID());
                    break;
    
                case TanaanIntroQuests::QUEST_THE_PORTALS_POWER:
                    if (p_Creature->IsAIEnabled)
                        p_Creature->AI()->PersonalTalk(eTalk::SAY_QA_THE_PORTALS_POWER, p_Player->GetGUID());
                    break;
                case TanaanIntroQuests::QUEST_THE_COST_OF_WAR:
                    p_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_RUNNING_AWAY, 16, *p_Player);
                    break;
                default:
                    break;
            }
            return true;
        }
    
        struct npc_archmage_khadgar_78558AI : public ScriptedAI
        {
            npc_archmage_khadgar_78558AI(Creature* p_Creature) : ScriptedAI(p_Creature) {}
    
            uint32 BarrierTimer = 0;
            Creature* l_PrevIter = nullptr;
            std::list<Creature*> l_DummyNpcList;
    
            void UpdateAI(uint32 const diff) override
            {
                if (BarrierTimer <= diff)
                {
                    if (l_DummyNpcList.empty())
                    {
                        me->GetCreatureListWithEntryInGrid(l_DummyNpcList, TanaanIntroCreatures::NPC_KHADGAR_SHIELD_TARGET_DUMMY, 80.0f);
                        BarrierTimer = 10 * IN_MILLISECONDS;
                    }
                    else
                    {
                        for (Creature* l_Iter : l_DummyNpcList)
                        {
                            if (l_Iter->GetDistance2d(me) <= 20.0f)
                            {
                                me->CastSpell(l_Iter, TanaanIntroSpells::SPELL_ARCANE_AEGIS, true);
                                l_PrevIter = l_Iter;
                            }
                            else
                            {
                                if (l_PrevIter)
                                    l_Iter->CastSpell(l_PrevIter, TanaanIntroSpells::SPELL_INCOMING_CAPSULE, true);
                            }
                        }
                        BarrierTimer = 60 * IN_MILLISECONDS;
                    }
                }
                else BarrierTimer -= diff;
            }
        };
    
        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_archmage_khadgar_78558AI(p_Creature);
        }
};

// @Creature: Gul'dan 78333 - Dark Portal area
class npc_gul_dan_78333 : public CreatureScript
{
    public:
        npc_gul_dan_78333() : CreatureScript("npc_gul_dan_78333") {}

        struct npc_gul_dan_78333AI : public ScriptedAI
        {
            npc_gul_dan_78333AI(Creature* creature) : ScriptedAI(creature) {}
       
            void MoveInLineOfSight(Unit* p_Unit) override
            {
                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;
    
                if (l_Player->GetDistance2d(me) >= 40.0f)
                    return;
    
                if (l_Player->GetPositionZ() - 5.0f > me->GetPositionZ())
                    return;
    
                if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_PORTALS_POWER) == QUEST_STATUS_INCOMPLETE &&
                    l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_ENTER_GULDAN_PRISON) < 1)
                {
                    l_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_ENTER_GULDAN_PRISON);
                    l_Player->PlayScene(TanaanIntroScenes::SCENE_GULDAN_REVEAL, l_Player);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_gul_dan_78333AI(creature);
        }
};

// @Creature: Thaelin (78568) and Hansel (78569) - Dark Portal area
class npc_detonation_dwarves : public CreatureScript
{
    public:
        npc_detonation_dwarves() : CreatureScript("npc_detonation_dwarves") {}
    
        struct npc_detonation_dwarvesAI : public ScriptedAI
        {
            npc_detonation_dwarvesAI(Creature* p_Creature) : ScriptedAI(p_Creature) {}
    
            void MoveInLineOfSight(Unit* p_Unit) override
            {
                if (Player* l_Player = p_Unit->ToPlayer())
                {
                    if (!me->IsWithinDistInMap(l_Player, 15.0f))
                        return;
    
                    if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_ONSLAUGHTS_END) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (me->GetEntry() == TanaanIntroCreatures::NPC_THAELIN_DARKANVIL)
                        {
                            if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_FLAG_THAELIN_EVENT_COMPLETE) == QUEST_STATUS_NONE)
                                l_Player->AddAura(TanaanIntroSpells::SPELL_THAELIN_EVENT_AURA, l_Player);
                        }
                        else if (me->GetEntry() == TanaanIntroCreatures::NPC_HANSEL_HEAVYHANDS)
                        {
                            if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_FLAG_HANSEL_EVENT_COMPLETE) == QUEST_STATUS_NONE)
                                l_Player->AddAura(TanaanIntroSpells::SPELL_HANSEL_EVENT_AURA, l_Player);
                        }
                    }
                }
            }
        };
    
        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_detonation_dwarvesAI(p_Creature);
        }
};

// @Creature: Teron'gor (81696) and Cho'gall (81695) - Dark Portal area
class npc_spire_prisoners : public CreatureScript
{
    public:
        npc_spire_prisoners() : CreatureScript("npc_spire_prisoners") {}
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spire_prisonersAI(creature);
        }
    
        struct npc_spire_prisonersAI : public ScriptedAI
        {
            npc_spire_prisonersAI(Creature* creature) : ScriptedAI(creature) {}
    
            void MoveInLineOfSight(Unit* p_Unit) override
            {
                if (!p_Unit)
                    return;
    
                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;
    
                if (me->GetExactDist2d(l_Player) >= 40.0f)
                    return;
    
                if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_ONSLAUGHTS_END) == QUEST_STATUS_INCOMPLETE)
                {
                    if (me->GetEntry() == TanaanIntroCreatures::NPC_TERONGOR && l_Player->HasAura(TanaanIntroSpells::SPELL_THAELIN_EVENT_AURA))
                    {
                        l_Player->RemoveAura(TanaanIntroSpells::SPELL_THAELIN_EVENT_AURA);
                        if (me->IsAIEnabled)
                        {
                            uint64 l_PlayerGuid = l_Player->GetGUID();
                            me->AI()->PersonalTalk(0, l_Player->GetGUID());
    
                            me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                            {
                                Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid);
                                if (!l_Player)
                                    return;
    
                                if (l_Player->GetQuestObjectiveCounter(273946) < 1)
                                    PersonalTalk(1, l_PlayerGuid);
                            }, 4000);
    
                            me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                            {
                                Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid);
                                if (!l_Player)
                                    return;
    
                                if (l_Player->GetQuestObjectiveCounter(273946) < 1)
                                    PersonalTalk(2, l_PlayerGuid);
                            }, 9000);
                        }
                    }
                    else if (me->GetEntry() == TanaanIntroCreatures::NPC_CHOGALL && l_Player->HasAura(TanaanIntroSpells::SPELL_HANSEL_EVENT_AURA))
                    {
                        l_Player->RemoveAura(TanaanIntroSpells::SPELL_HANSEL_EVENT_AURA);
                        if (me->IsAIEnabled)
                            me->AI()->PersonalTalk(0, l_Player->GetGUID());
                    }
                }
            }
        };
};

// @Creture: Korag (78573) and Taag (81762) - Heartblood area
class npc_bled_dry_questgivers : public CreatureScript
{
    public:
        npc_bled_dry_questgivers() : CreatureScript("npc_bled_dry_questgivers") {}
    
        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, Quest const* p_Quest) override
        {
            if (p_Creature->GetEntry() == TanaanIntroCreatures::NPC_KORAG && p_Quest->GetQuestId() == TanaanIntroQuests::QUEST_BLED_DRY_H)
            {
                p_Creature->PlayDirectSound(1322, p_Player);
                if (Creature* l_Ashka = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_ASHKA, 20.0f))
                    l_Ashka->HandleEmoteCommand(EMOTE_ONESHOT_CRY, p_Player->GetGUID());
            }
            else if (p_Creature->GetEntry() == TanaanIntroCreatures::NPC_TAAG && p_Quest->GetQuestId() == TanaanIntroQuests::QUEST_BLED_DRY_A)
            {
                p_Creature->PlayDirectSound(8987, p_Player);
                if (Creature* l_Rephuura = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_REPHUURA, 20.0f))
                    l_Rephuura->HandleEmoteCommand(EMOTE_ONESHOT_CRY, p_Player->GetGUID());
            }
    
            return true;
        }
};

// @Creature: Ariok (78556) - Spawned creature
class npc_ariok_78556 : public CreatureScript
{
    public:
        npc_ariok_78556() : CreatureScript("npc_ariok_78556") {}
    
        struct npc_ariok_78556AI : public ScriptedAI
        {
            npc_ariok_78556AI(Creature* p_Creature) : ScriptedAI(p_Creature) {}
    
            enum eGossipData
            {
                GOSSIP_MENU_ID = 16641,
                GOSSIP_ID = 0,
            };
    
            void sGossipSelect(Player* p_Player, uint32 p_Sender, uint32 p_Action) override
            {
                p_Player->PlayerTalkClass->SendCloseGossip();
                if (p_Sender == eGossipData::GOSSIP_MENU_ID && p_Action == eGossipData::GOSSIP_ID)
                {
                    p_Player->CastSpell(me, TanaanIntroSpells::SPELL_SUMMON_ARIOK_GUARDIAN, true);
                    p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_SPEAK_WITH_ARIOK);
                    me->DestroyForPlayer(p_Player);
                }
            }
        };
    
        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ariok_78556AI(p_Creature);
        }
};

// @Creature: Ariok (80087) - Summoned creature for quest event
class npc_ariok_80087 : public CreatureScript
{
    public:
        npc_ariok_80087() : CreatureScript("npc_ariok_80087") {}
    
        struct npc_ariok_80087AI : public ScriptedAI
        {
            npc_ariok_80087AI(Creature* p_Creature) : ScriptedAI(p_Creature) {}
    
            enum eEvents
            {
                SAY_ONE = 1,
                SAY_TWO,
                SAY_THREE,
                SAY_FOUR,
                SAY_FIVE,
            };
    
            EventMap m_Events;
            ObjectGuid m_PlayerGUID;
            bool m_AltarFound = false;
            bool m_ArmyRun = false;
    
            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                {
                    me->DespawnOrUnsummon();
                    return;
                }
    
                m_PlayerGUID = l_Player->GetObjectGuid();
                m_Events.ScheduleEvent(eEvents::SAY_ONE, 1000);
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_PlayerGUID)
                    return;
    
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        l_Player->RemoveAura(TanaanIntroSpells::SPELL_SUMMON_ARIOK_GUARDIAN);
                        return;
                    }
    
                    if (!l_Player->HasQuest(TanaanIntroQuests::QUEST_ALTAR_ALTERCATION) || l_Player->isDead())
                    {
                        me->DespawnOrUnsummon();
                        l_Player->RemoveAura(TanaanIntroSpells::SPELL_SUMMON_ARIOK_GUARDIAN);
                        return;
                    }
    
                    if (l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_BLOOD_ORB_DESTROYED) >= 3)
                    {
                        me->AI()->PersonalTalk(7, m_PlayerGUID);
                        me->DespawnOrUnsummon();
                        l_Player->RemoveAura(TanaanIntroSpells::SPELL_SUMMON_ARIOK_GUARDIAN);
                        return;
                    }
    
                    if (l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_ESCORT_ARIOK_TO_ALTAR) > 0 && !m_AltarFound)
                    {
                        m_AltarFound = true;
                        m_Events.CancelEvent(eEvents::SAY_THREE);
                        m_Events.ScheduleEvent(eEvents::SAY_FOUR, 100);
                    }
    
                    if (m_AltarFound && !m_ArmyRun && l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_BLOOD_ORB_DESTROYED) > 0)
                    {
                        m_ArmyRun = true;
                        me->AI()->PersonalTalk(6, m_PlayerGUID);
                    }
                }
    
                m_Events.Update(p_Diff);
                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::SAY_ONE:
                    {
                        me->AI()->PersonalTalk(1, m_PlayerGUID);
                        m_Events.ScheduleEvent(eEvents::SAY_TWO, 6000);
                        break;
                    }
                    case eEvents::SAY_TWO:
                    {
                        me->AI()->PersonalTalk(2, m_PlayerGUID);
                        m_Events.ScheduleEvent(eEvents::SAY_THREE, 7000);
                        break;
                    }
                    case eEvents::SAY_THREE:
                    {
                        me->AI()->PersonalTalk(3, m_PlayerGUID);
                        break;
                    }
                    case eEvents::SAY_FOUR:
                    {
                        me->AI()->PersonalTalk(4, m_PlayerGUID);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION, m_PlayerGUID);
                        me->AddUnitState(UNIT_STATE_ROOT);
                        m_Events.ScheduleEvent(eEvents::SAY_FIVE, 7000);
                        break;
                    }
                    case eEvents::SAY_FIVE:
                    {
                        me->AI()->PersonalTalk(5, m_PlayerGUID);
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        break;
                    }
                    default:
                        break;
                }
    
                if (!UpdateVictim())
                    return;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ariok_80087AI(p_Creature);
        }
};

// @Creature: Kilrogg Deadeye (81926) - Hidden trigger
class npc_kilrogg_deadeye_81926 : public CreatureScript
{
    public:
        npc_kilrogg_deadeye_81926() : CreatureScript("npc_kilrogg_deadeye_81926") {}
    
        struct npc_kilrogg_deadeye_81926AI : public ScriptedAI
        {
            npc_kilrogg_deadeye_81926AI(Creature* creature) : ScriptedAI(creature) {}
    
            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetReactState(REACT_PASSIVE);
            }
    
            void MoveInLineOfSight(Unit* p_Unit) override
            {
                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;
    
                if (me->GetExactDist2d(l_Player) >= 80.0f)
                    return;
    
                if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_ALTAR_ALTERCATION) == QUEST_STATUS_INCOMPLETE &&
                    l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_ESCORT_ARIOK_TO_ALTAR) < 1)
                {
                    l_Player->CastSpell(l_Player, TanaanIntroSpells::SPELL_ON_ALTAR, true);
    
                    if (me->IsAIEnabled)
                    {
                        me->AI()->PersonalTalk(0, l_Player->GetGUID(), 15000);
                        me->AI()->PersonalTalk(1, l_Player->GetGUID(), 35000);
                    }
                }
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kilrogg_deadeye_81926AI(creature);
        }
};

// @Creature: Ritual Orb (83670) - Spawns for quest event
class npc_blood_ritual_orb_83670 : public CreatureScript
{
    public:
        npc_blood_ritual_orb_83670() : CreatureScript("npc_blood_ritual_orb_83670") {}
    
        struct npc_blood_ritual_orb_83670AI : public ScriptedAI
        {
            npc_blood_ritual_orb_83670AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_BLOOD_RITUAL_ORB_BEAM = 170044,
                SPELL_DESTROYING_BLOOD_ROB = 167955,
                SPELL_ARIOK_SUMMON_AURA = 159404
            };
    
            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                Player* l_Player = p_Caster->ToPlayer();
                if (!l_Player)
                    return;
    
                if (!p_Spell || p_Spell->Id != eSpells::SPELL_DESTROYING_BLOOD_ROB)
                    return;
    
                if (l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_BLOOD_ORB_DESTROYED) == 1)
                {
                    l_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_SNEAKY_ARMY, 16, l_Player->GetPosition());

                    if (Creature* ariok = me->FindNearestCreature(78950, 20.0f))
                        ariok->AI()->Talk(1);
                }
                else if (l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_BLOOD_ORB_DESTROYED) == 2)
                {
                    if (Creature* ariok = me->FindNearestCreature(78950, 20.0f))
                        ariok->AI()->Talk(2);
                }
                else if (l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_BLOOD_ORB_DESTROYED) == 3)
                {
                    if (me->IsAIEnabled)
                    {
                        uint64 l_PlayerGuid = l_Player->GetGUID();
                        me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                            {
                                l_Player->RemoveAurasDueToSpell(SPELL_ARIOK_SUMMON_AURA);
                                l_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_KILROGG_REVEAL, 16, *l_Player);
                                l_Player->CancelStandaloneScene(l_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_EYE_OF_KILROGG));
                            }
                        }, 2000);
                    }
                }
                me->DestroyForPlayer(l_Player);
            }
    
            bool m_Spawn = false;
            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_Spawn)
                    return;
    
                if (Creature* l_Bloodchosen = me->FindNearestCreature(TanaanIntroCreatures::NPC_BLEEDING_HOLLOW_BLOODCHOSEN, 50.0f))
                {
                    m_Spawn = true;
                    me->CastSpell(l_Bloodchosen, eSpells::SPELL_BLOOD_RITUAL_ORB_BEAM, true);
                }
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_blood_ritual_orb_83670AI(creature);
        }
};

// @Creature: Archmage Khadgar (78560) - After bridge destroyed event spawn
class npc_archmage_khadgar_78560 : public CreatureScript
{
    public:
        npc_archmage_khadgar_78560() : CreatureScript("npc_archmage_khadgar_78560") {}
    
        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, const Quest* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case TanaanIntroQuests::QUEST_KARGATHAR_PROVING_GROUNDS:
                {
                    p_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_DESTROY_BRIDGE, 16, *p_Player);
                    p_Player->CastSpell(p_Creature, TanaanIntroSpells::SPELL_SUMMON_KHADGAR_BRIDGE, true);
                    break;
                }
                case TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_A:
                case TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_H:
                {
                    if (Creature* l_Thrall = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_THRALL, 15.0f))
                        l_Thrall->AI()->PersonalTalk(0, p_Player->GetGUID());
    
                    break;
                }
                default:
                    break;
            }
    
            return true;
        }
    
        struct npc_archmage_khadgar_78560AI : public ScriptedAI
        {
            npc_archmage_khadgar_78560AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_FROST_CHANNELING = 176071
            };
    
            void InitializeAI() override
            {
                if (me->GetPhaseMask() != 4)
                    return;
    
                me->CastSpell(me, eSpells::SPELL_FROST_CHANNELING, true);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_CANNOT_TURN);
            }
    
            void MoveInLineOfSight(Unit* p_Unit) override
            {
                if (me->GetPhaseMask() == 4)
                    return;
    
                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;
    
                if (me->GetDistance2d(l_Player) >= 30.0f)
                    return;
    
                if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_ALTAR_ALTERCATION) == QUEST_STATUS_INCOMPLETE &&
                    l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_FIND_KHADGAR_BRIDGE) < 1 &&
                    l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_BLOOD_ORB_DESTROYED) == 3)
                    l_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_FIND_KHADGAR_BRIDGE);
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_archmage_khadgar_78560AI(creature);
        }
};

// @Creature: Archmage Khadgar (80244) - Summoned creature for bridge destroy event
class npc_archmage_khadgar_80244 : public CreatureScript
{
    public:
        npc_archmage_khadgar_80244() : CreatureScript("npc_archmage_khadgar_80244") {}
    
        struct npc_archmage_khadgar_80244AI : public ScriptedAI
        {
            npc_archmage_khadgar_80244AI(Creature* p_Creature) : ScriptedAI(p_Creature) {}
    
            enum eEvents
            {
                SPAWN_SETUP = 1,
                DESTROY_BRIDGE,
                RUN_EVENT,
            };
    
            enum eSpells
            {
                SPELL_METEOR_SHOWER = 165864,
            };
    
            EventMap m_Events;
            ObjectGuid m_PlayerGUID;
    
            G3D::Vector3 const RunPath[4] =
            {
                {4212.088f, -2783.934f, 23.66859f},
                {4221.65f, -2792.031f, 21.28993f},
                {4228.165f, -2808.644f, 17.32043f},
                {4229.694f, -2812.865f, 17.2561f}
            };
    
            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                    return;
    
                me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_CANNOT_TURN);
                m_PlayerGUID = l_Player->GetObjectGuid();
                m_Events.ScheduleEvent(eEvents::SPAWN_SETUP, 500);
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_PlayerGUID)
                {
                    me->DespawnOrUnsummon();
                    return;
                }
    
                m_Events.Update(p_Diff);
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
    
                    if (!l_Player->HasQuest(TanaanIntroQuests::QUEST_KARGATHAR_PROVING_GROUNDS))
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
                }
    
                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::SPAWN_SETUP:
                    {
                        //me->SetOrientation(3.246312f);
                        me->SetFacingTo(3.246312f);
                        me->SetFlag(UNIT_FIELD_FLAGS, 0);
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    
                        m_Events.ScheduleEvent(eEvents::DESTROY_BRIDGE, 1000);
                        break;
                    }
                    case eEvents::DESTROY_BRIDGE:
                    {
                        me->CastSpell(me, eSpells::SPELL_METEOR_SHOWER, true);
                        me->AI()->PersonalTalk(0, m_PlayerGUID);
    
                        m_Events.ScheduleEvent(eEvents::RUN_EVENT, 8500);
                        break;
                    }
                    case eEvents::RUN_EVENT:
                    {
                        me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_CANNOT_TURN);
                        //me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                        me->AI()->PersonalTalk(1, m_PlayerGUID);
                        me->GetMotionMaster()->MoveSmoothPath(0, RunPath, 4, false);
                        break;
                    }
                }
            }
    
            void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
            {
                if (p_ID == 0)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player)
                        return;
    
                    l_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_FOLLOW_KHADGAR);
                    me->SetFacingTo(6.242f);
                    me->DespawnOrUnsummon();
                }
            }
        };
    
        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_archmage_khadgar_80244AI(p_Creature);
        }
};

class npc_archmage_khadgar_78561 : public CreatureScript
{
    public:
        npc_archmage_khadgar_78561() : CreatureScript("npc_archmage_khadgar_78561") {}
    
        bool OnQuestReward(Player* p_Player, Creature* p_Creature, const Quest* p_Quest, uint32 /*p_Option*/) override
        {
            switch (p_Quest->GetQuestId())
            {
                case TanaanIntroQuests::QUEST_KILL_YOUR_HUNDRED:
                {
                    if (p_Player->GetTeamId() == TEAM_HORDE)
                    {
                        p_Creature->AI()->PersonalTalk(0, p_Player->GetGUID());
                        p_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_LIADRIN_AND_OLIN, 16, p_Player->GetPosition());
                    }
                    else if (p_Player->GetTeamId() == TEAM_ALLIANCE)
                    {
                        p_Creature->AI()->PersonalTalk(1, p_Player->GetGUID());
                        p_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_MALADAAR_AND_QIANA, 16, p_Player->GetPosition());
                    }
                    break;
                }
                default:
                    break;
            }
    
            return true;
        }
};

class npc_kargath_bladefist_79097 : public CreatureScript
{
    private:
        struct SpawnJumpPos
        {
            Position SpawnPos;
            Position JumpPos;
        };
    
    public:
        npc_kargath_bladefist_79097() : CreatureScript("npc_kargath_bladefist_79097") {}
    
        enum eActions
        {
            ActionSummonHandBrawler = 0,
            ActionGiveQuestCredit,
        };
    
        struct npc_kargath_bladefist_79097AI : public ScriptedAI
        {
            npc_kargath_bladefist_79097AI(Creature* creature) : ScriptedAI(creature) {}
    
            std::vector<SpawnJumpPos> l_PositionList =
            {
                {{4395.952f, -2786.451f, 13.049f, 4.582f},  {4393.971f, -2792.850f, 4.651f, 4.582f}},
                {{4419.850f, -2783.709f, 15.932f, 4.840f},  {4419.225f, -2793.640f, 4.773f, 4.840f}},
                {{4408.649f, -2786.520f, 13.052f, 4.910f},  {4410.132f, -2793.898f, 4.722f, 4.910f}},
                {{4384.628f, -2782.742f, 15.752f, 4.658f},  {4384.151f, -2791.617f, 4.595f, 4.658f}},
                {{4354.430f, -2844.290f, 17.664f, 0.321f},  {4362.545f, -2841.840f, 5.507f, 0.321f}},
                {{4371.080f, -2867.260f, 22.906f, 1.282f},  {4376.954f, -2857.616f, 4.873f, 1.282f}},
                {{4397.440f, -2869.620f, 23.687f, 1.520f},  {4397.410f, -2858.444f, 4.983f, 1.520f}},
                {{4410.280f, -2870.773f, 19.829f, 1.538f},  {4410.625f, -2860.225f, 5.092f, 1.538f}},
                {{4434.270f, -2862.230f, 18.895f, 2.328f},  {4423.780f, -2852.214f, 4.665f, 2.328f}},
                {{4445.680f, -2841.460f, 22.139f, 2.719f},  {4433.338f, -2840.700f, 4.949f, 2.719f}},
                {{4446.960f, -2814.430f, 22.227f, 3.304f},  {4435.016f, -2814.458f, 4.930f, 3.304f}},
                {{4354.580f, -2811.680f, 21.845f, 6.007f},  {4365.790f, -2812.246f, 4.780f, 6.007f}}
            };
    
            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            }
    
            void DoAction(int32 const p_Action) override
            {
                std::list<Player*> l_PlayerList;
                GetPlayerListInGrid(l_PlayerList, me, 100.0f);
    
                if (l_PlayerList.empty())
                    return;
    
                if (p_Action == eActions::ActionSummonHandBrawler)
                {
                    for (Player* l_Player : l_PlayerList)
                    {
                        if (l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_KILL_99_ENEMIES) < 99)
                        {
                            uint32 l_Random = urand(0, l_PositionList.size() - 1);
                            if (Creature* l_Creature = me->SummonCreature(TanaanIntroCreatures::NPC_SHATTERED_HAND_BRAWLER, l_PositionList[l_Random].SpawnPos, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                            {
                                l_Creature->SetPhaseMask(me->GetPhaseMask(), true);
                                l_Creature->SetFlagExtra(CREATURE_FLAG_EXTRA_IGNORE_PATHFINDING);
                                l_Creature->GetMotionMaster()->MoveJump(l_PositionList[l_Random].JumpPos, 10.0f, 10.0f, 1);
                            }
                        }
                    }
                }
                else if (p_Action == eActions::ActionGiveQuestCredit)
                {
                    for (Player* l_Player : l_PlayerList)
                    {
                        if (l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_KILL_99_ENEMIES) < 99)
                        {
                            l_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_99_KILLS);
                            l_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_100_KILLS);
                        }
                    }
                }
            }
        };
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kargath_bladefist_79097AI(creature);
        }
};

class npc_shattered_hand_brawler_82057 : public CreatureScript
{
    public:
        npc_shattered_hand_brawler_82057() : CreatureScript("npc_shattered_hand_brawler_82057") {}
    
        struct npc_shattered_hand_brawler_82057AI : public ScriptedAI
        {
            npc_shattered_hand_brawler_82057AI(Creature* creature) : ScriptedAI(creature) {}
    
            void InitializeAI() override
            {
                if (me->isSummon())
                    return;
    
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetInCombatWithZone();
            }
    
            void JustDied(Unit* p_Killer) override
            {
                if (!p_Killer)
                    return;
    
                if (Creature* l_Kargath = me->FindNearestCreature(TanaanIntroCreatures::NPC_KARGATH_BLADEFIST, 100.0f))
                {
                    if (l_Kargath->GetAI())
                    {
                        l_Kargath->AI()->DoAction(1);
    
                        if (!me->isSummon())
                            l_Kargath->AI()->DoAction(0);
                    }
                }
            }
    
            void MovementInform(uint32 p_Type, uint32 /*p_Id*/) override
            {
                if (p_Type != EFFECT_MOTION_TYPE)
                    return;
    
                std::list<Player*> l_PlayerList;
                me->GetPlayerListInGrid(l_PlayerList, 50.0f);
    
                if (l_PlayerList.empty())
                {
                    std::list<Creature*> l_CreatureList;
                    me->GetCreatureListInGrid(l_CreatureList, 50.0f);
    
                    if (!l_CreatureList.empty())
                    {
                        for (Creature* l_Creature : l_CreatureList)
                        {
                            if (l_Creature->IsHostileTo(me))
                            {
                                AttackStart(l_Creature);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    for (Player* l_Player : l_PlayerList)
                    {
                        if (l_Player->HasQuest(TanaanIntroQuests::QUEST_KILL_YOUR_HUNDRED) && l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_KILL_99_ENEMIES) < 99)
                        {
                            uint64 l_PlayerGuid = l_Player->GetGUID();
                            me->AddDelayedEvent([this, l_PlayerGuid]() -> void
                            {
                                if (Player* l_Target = ObjectAccessor::GetPlayer(*me, l_PlayerGuid))
                                    AttackStart(l_Target);
    
                            }, 1000);
                            break;
                        }
                    }
                }
            }
        };
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shattered_hand_brawler_82057AI(creature);
        }
};

class npc_shadowmoon_ritualist_79590 : public CreatureScript
{
    public:
        npc_shadowmoon_ritualist_79590() : CreatureScript("npc_shadowmoon_ritualist_79590") {}
    
        struct npc_shadowmoon_ritualist_79590AI : public ScriptedAI
        {
            npc_shadowmoon_ritualist_79590AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_VOID_CHANNELING = 160677,
                SPELL_COSMETIC_VOID_MISSILE = 165877,
                SPELL_VOID_BOLT = 157355,
                SPELL_AURA_FEIGN_DEATH = 29266,
            };
    
            uint32 m_VoidBoltTimer = 0;
    
            void InitializeAI() override
            {
                if (me->HasAura(eSpells::SPELL_AURA_FEIGN_DEATH))
                    return;
    
                me->AddAura(eSpells::SPELL_VOID_CHANNELING, me);
                me->AddAura(eSpells::SPELL_COSMETIC_VOID_MISSILE, me);
            }
    
            void Reset() override
            {
                m_VoidBoltTimer = 0;
            }
    
            void JustRespawned() override
            {
                InitializeAI();
                Reset();
            }
    
            void JustReachedHome() override
            {
                InitializeAI();
            }
    
            void EnterCombat(Unit* p_Unit) override
            {
                // 30% chance to say random text
                if (urand(0, 100) > 30)
                    return;
    
                me->AI()->Talk(0);
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                if (m_VoidBoltTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SPELL_VOID_BOLT);
    
                    m_VoidBoltTimer = urand(5000, 8000);
                }
                else m_VoidBoltTimer -= p_Diff;
    
                DoMeleeAttackIfReady();
            }
        };
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadowmoon_ritualist_79590AI(creature);
        }
};

class npc_shadowmoon_voidaxe_79589 : public CreatureScript
{
    public:
        npc_shadowmoon_voidaxe_79589() : CreatureScript("npc_shadowmoon_voidaxe_79589") {}
    
        struct npc_shadowmoon_voidaxe_79589AI : public ScriptedAI
        {
            npc_shadowmoon_voidaxe_79589AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_ENTROPIC_WHIRL = 170480,
                SPELL_CLUMSY_SWIPE = 170456,
                SPELL_AURA_VOID_STRIKES = 157589,
                SPELL_AURA_FEIGN_DEATH = 29266,
            };
    
            enum eEvents
            {
                EVENT_ENTROPIC_WHIRL = 1,
                EVENT_CLUMSY_SWIPE,
            };
    
            EventMap m_Events;
    
            void InitializeAI() override
            {
                if (me->HasAura(eSpells::SPELL_AURA_FEIGN_DEATH))
                    return;
    
                if (!me->HasAura(eSpells::SPELL_AURA_VOID_STRIKES))
                    me->AddAura(eSpells::SPELL_AURA_VOID_STRIKES, me);
            }
    
            void Reset() override
            {
                m_Events.Reset();
            }
    
            void EnterCombat(Unit* /*who*/) override
            {
                m_Events.ScheduleEvent(EVENT_ENTROPIC_WHIRL, urand(10000, 15000));
                m_Events.ScheduleEvent(EVENT_CLUMSY_SWIPE, urand(0, 3000));
    
                // 30% chance to say random text
                if (urand(0, 100) > 30)
                    return;
    
                me->AI()->Talk(0);
            }
    
            void JustReachedHome() override
            {
                InitializeAI();
                Reset();
            }
    
            void JustRespawned() override
            {
                InitializeAI();
                Reset();
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                m_Events.Update(p_Diff);
                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EVENT_ENTROPIC_WHIRL:
                    {
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, eSpells::SPELL_ENTROPIC_WHIRL);
    
                        m_Events.ScheduleEvent(eEvents::EVENT_ENTROPIC_WHIRL, urand(10000, 15000));
                        break;
                    }
                    case eEvents::EVENT_CLUMSY_SWIPE:
                    {
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, eSpells::SPELL_CLUMSY_SWIPE);
    
                        m_Events.ScheduleEvent(eEvents::EVENT_CLUMSY_SWIPE, urand(5000, 8000));
                        break;
                    }
                    default:
                        break;
                }
    
                DoMeleeAttackIfReady();
            }
        };
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadowmoon_voidaxe_79589AI(creature);
        }
};

class npc_yrel_78994 : public CreatureScript
    {
    public:
        npc_yrel_78994() : CreatureScript("npc_yrel_78994") {}
    
        enum eSpells
        {
            SPELL_SUMMON_YREL_GUARDIAN = 161108,
        };
    
        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, const Quest* p_Quest) override
        {
            switch (p_Quest->GetQuestId())
            {
                case TanaanIntroQuests::QUEST_YREL_A:
                case TanaanIntroQuests::QUEST_YREL_H:
                {
                    if (CreatureTemplate const* l_Template = sObjectMgr->GetCreatureTemplate(79795))
                    {
                        p_Player->SummonCreature(l_Template->Entry, p_Creature->GetPosition());
                    }
    
                    //p_Player->CastSpell(p_Creature, eSpells::SPELL_SUMMON_YREL_GUARDIAN);
                    break;
                }
                default:
                    break;
            }
    
            return true;
        }
    
        struct npc_yrel_78994AI : public ScriptedAI
        {
            npc_yrel_78994AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eTexts
            {
                SAY_CREDIT = 0,
            };
    
            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                //me->AddUnitState(UNIT_STATE_CANNOT_TURN);
            }
    
            void MoveInLineOfSight(Unit* p_Unit) override
            {
                Player* l_Player = p_Unit->ToPlayer();
                if (!l_Player)
                    return;
    
                if (!me->HasInPhaseList(TanaanIntroPhases::PHASE_YREL_NOT_FREED))
                    return;
    
                if (l_Player->GetExactDist2d(me) >= 20.0f)
                    return;
    
                if ((l_Player->HasQuest(TanaanIntroQuests::QUEST_MASTERS_OF_SHADOW_A) && l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_FIND_ANKOVA_A) < 1) ||
                    (l_Player->HasQuest(TanaanIntroQuests::QUEST_MASTERS_OF_SHADOW_H) && l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_FIND_ANKOVA_H) < 1))
                {
                    l_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_FIND_ANKOVA);
                    me->AI()->PersonalTalk(0, l_Player->GetGUID());
                }
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_yrel_78994AI(creature);
        }
};

class npc_yrel_guardian_79795 : public CreatureScript
{
    public:
        npc_yrel_guardian_79795() : CreatureScript("npc_yrel_guardian_79795") {}
    
        enum eSpells
        {
            SPELL_SUMMON_YREL_GUARDIAN = 161108,
        };
    
        struct npc_yrel_guardian_79795AI : public ScriptedAI
        {
            npc_yrel_guardian_79795AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eTexts
            {
                SAY_ONE = 1,
                SAY_TWO = 2,
                SAY_THREE = 3
            };
    
            enum eEvents
            {
                EVENT_SAY_ONE = 1,
                EVENT_SAY_TWO = 2,
                EVENT_RUN = 3,
                EVENT_UNROOT = 4
            };
    
            G3D::Vector3 const RunPath[10] =
            {
                {4545.055f, -2528.351f, 13.994f},
                {4539.695f, -2516.788f, 14.630f},
                {4540.138f, -2503.493f, 20.047f},
                {4540.296f, -2499.260f, 20.305f},
                {4540.869f, -2486.340f, 25.651f},
                {4540.108f, -2477.052f, 25.445f},
                {4531.276f, -2474.630f, 25.300f},
                {4518.173f, -2474.941f, 25.307f},
                {4513.635f, -2494.780f, 25.723f},
                {4516.580f, -2495.620f, 25.872f}
            };
    
            ObjectGuid m_PlayerGUID;
            EventMap m_Events;
            bool m_AlliesFound = false;
    
            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                {
                    me->DespawnOrUnsummon();
                    return;
                }
    
                m_PlayerGUID = l_Player->GetObjectGuid();
                me->SetOwnerGUID(m_PlayerGUID);
                me->SetEntryForOwner(79794);
    
                me->SetFlag(UNIT_FIELD_FLAGS, 131968);
                me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN | UNIT_STATE_ROOT);
                me->SetFacingTo(3.896f);
    
                me->GetMotionMaster()->MoveFollow(l_Player, 0.0f, 0.0f);
                m_Events.ScheduleEvent(eEvents::EVENT_SAY_ONE, 1000);
            }
    
            void MovementInform(uint32 p_Type, uint32 /*p_ID*/) override
            {
                if (p_Type != EFFECT_MOTION_TYPE)
                    return;
    
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                if (!l_Player)
                    return;
    
                if (me->GetDistance(4516.580f, -2495.620f, 25.872f) >= 5.0f)
                    return;
    
                l_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_YREL);
                me->SetFacingTo(1.665f);
                me->DespawnOrUnsummon();
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_PlayerGUID)
                    return;
    
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
    
                    if ((!l_Player->HasQuest(TanaanIntroQuests::QUEST_YREL_A) && l_Player->GetTeamId() == TEAM_ALLIANCE) ||
                        (!l_Player->HasQuest(TanaanIntroQuests::QUEST_YREL_H) && l_Player->GetTeamId() == TEAM_HORDE))
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }
    
                    if (me->GetExactDist2d(4538.877f, -2529.961f) <= 25.0f && !m_AlliesFound)
                    {
                        m_AlliesFound = true;
                        me->AI()->PersonalTalk(eTexts::SAY_THREE, m_PlayerGUID);
                        me->AddUnitState(UNIT_STATE_ROOT);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT | UNIT_FLAG_SERVER_CONTROLLED | UNIT_FLAG_PET_IN_COMBAT);
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFacingTo(2.288f);
    
                        m_Events.ScheduleEvent(EVENT_RUN, 5000);
                    }
                }
    
                m_Events.Update(p_Diff);
                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EVENT_SAY_ONE:
                    {
                        me->AI()->PersonalTalk(eTexts::SAY_ONE, m_PlayerGUID);
    
                        m_Events.ScheduleEvent(eEvents::EVENT_UNROOT, 2000);
                        m_Events.ScheduleEvent(eEvents::EVENT_SAY_TWO, 8000);
                        break;
                    }
                    case eEvents::EVENT_SAY_TWO:
                    {
                        me->AI()->PersonalTalk(eTexts::SAY_TWO, m_PlayerGUID);
                        break;
                    }
                    case eEvents::EVENT_RUN:
                    {
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        me->GetMotionMaster()->MoveSmoothPath(1, RunPath, 10, false);
                        break;
                    }
                    case eEvents::EVENT_UNROOT:
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                        me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        break;
                    }
                    default:
                        break;
                }
    
                if (!UpdateVictim())
                    return;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_yrel_guardian_79795AI(creature);
        }
};

class npc_taskmaster_gurran_79585 : public CreatureScript
{
    public:
        npc_taskmaster_gurran_79585() : CreatureScript("npc_taskmaster_gurran_79585") {}
    
        struct npc_taskmaster_gurran_79585AI : public ScriptedAI
        {
            npc_taskmaster_gurran_79585AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_VOID_SLICE = 165674,
                SPELL_LASHING_VOID = 165685,
            };
    
            enum eTexts
            {
                SAY_AGGRO = 0,
            };
    
            enum eTimers
            {
                TIMER_VOID_SLICE_MIN = 8000,
                TIMER_VOID_SLICE_MAX = 9000,
                TIMER_LASHING_VOID_MIN = 5000,
                TIMER_LASHING_VOID_MAX = 6000
            };
    
            uint32 m_VoidSliceTimer;
            uint32 m_LashingVoidTimer;
    
            void EnterCombat(Unit* /*p_Unit*/) override
            {
                me->AI()->Talk(SAY_AGGRO);
                m_VoidSliceTimer = urand(eTimers::TIMER_VOID_SLICE_MIN, eTimers::TIMER_VOID_SLICE_MAX);
                m_LashingVoidTimer = urand(eTimers::TIMER_LASHING_VOID_MIN, eTimers::TIMER_LASHING_VOID_MAX);
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                if (m_LashingVoidTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SPELL_LASHING_VOID);
    
                    m_LashingVoidTimer = urand(eTimers::TIMER_LASHING_VOID_MIN, eTimers::TIMER_LASHING_VOID_MAX) + 10 * IN_MILLISECONDS;
                }
                else m_LashingVoidTimer -= p_Diff;
    
                if (m_VoidSliceTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SPELL_VOID_SLICE);
    
                    m_VoidSliceTimer = urand(eTimers::TIMER_VOID_SLICE_MIN, eTimers::TIMER_VOID_SLICE_MAX) + 10 * IN_MILLISECONDS;
                }
                else m_VoidSliceTimer -= p_Diff;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_taskmaster_gurran_79585AI(creature);
        }
};

class npc_ungra_79583 : public CreatureScript
{
    public:
        npc_ungra_79583() : CreatureScript("npc_ungra_79583") {}
    
        struct npc_ungra_79583AI : public ScriptedAI
        {
            npc_ungra_79583AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_VOID_CHANNELING = 160677,
                SPELL_COSMETIC_VOID_MISSILE = 165877,
                SPELL_VOID_SHOCK = 165702,
                SPELL_VOID_MONSTROSITY_TOTEM = 165689,
            };
    
            enum eTexts
            {
                SAY_AGGRO = 0,
            };
    
            enum eTimers
            {
                TIMER_VOID_SHOCK_MIN = 3000,
                TIMER_VOID_SHOCK_MAX = 5000,
                TIMER_VOID_MONSTROSITY_TOTEM_MIN = 6000,
                TIMER_VOID_MONSTROSITY_TOTEM_MAX = 8000
            };
    
            uint32 m_VoidShockTimer;
            uint32 m_MonstrosityTotemTimer;
    
            void InitializeAI() override
            {
                me->AddAura(eSpells::SPELL_VOID_CHANNELING, me);
                me->AddAura(eSpells::SPELL_COSMETIC_VOID_MISSILE, me);
            }
    
            void JustRespawned() override
            {
                InitializeAI();
            }
    
            void JustReachedHome() override
            {
                InitializeAI();
            }
    
            void EnterCombat(Unit* /*p_Unit*/) override
            {
                me->AI()->Talk(SAY_AGGRO);
                m_VoidShockTimer = urand(eTimers::TIMER_VOID_SHOCK_MIN, eTimers::TIMER_VOID_SHOCK_MAX);
                m_MonstrosityTotemTimer = urand(eTimers::TIMER_VOID_MONSTROSITY_TOTEM_MIN, eTimers::TIMER_VOID_MONSTROSITY_TOTEM_MAX);
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                if (m_VoidShockTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SPELL_VOID_SHOCK);
    
                    m_VoidShockTimer = urand(8000, 9000);
                }
                else m_VoidShockTimer -= p_Diff;
    
                if (m_MonstrosityTotemTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SPELL_VOID_MONSTROSITY_TOTEM);
    
                    m_MonstrosityTotemTimer = 35000;
                }
                else m_MonstrosityTotemTimer -= p_Diff;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ungra_79583AI(creature);
        }
};

class npc_olin_umberhide_guardian_79822 : public CreatureScript
{
    public:
        npc_olin_umberhide_guardian_79822() : CreatureScript("npc_olin_umberhide_guardian_79822") {}
    
        struct npc_olin_umberhide_guardian_79822AI : public ScriptedAI
        {
            npc_olin_umberhide_guardian_79822AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_WAR_STOMP = 166969,
            };
    
            uint32 m_WarStompTimer;
    
            void EnterCombat(Unit* /*p_Unit*/) override
            {
                m_WarStompTimer = urand(3000, 5000);
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                if (m_WarStompTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        if (me->IsWithinMeleeRange(l_Target))
                            me->CastSpell(l_Target, eSpells::SPELL_WAR_STOMP);
    
                    m_WarStompTimer = urand(10000, 13000);
                }
                else m_WarStompTimer -= p_Diff;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_olin_umberhide_guardian_79822AI(creature);
        }
};

class npc_lady_liadrin_guardian_79823 : public CreatureScript
{
    public:
        npc_lady_liadrin_guardian_79823() : CreatureScript("npc_lady_liadrin_guardian_79823") {}
    
        struct npc_lady_liadrin_guardian_79823AI : public ScriptedAI
        {
            npc_lady_liadrin_guardian_79823AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_HOLY_LIGHT = 167420,
                SPELL_LIGHTS_REUNION = 166933,
            };
    
            ObjectGuid m_PlayerGUID;
            uint32 m_LolyLightTimer;
            uint32 m_LightsReunionTimer;
    
            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                {
                    me->DespawnOrUnsummon();
                    return;
                }
    
                m_PlayerGUID = l_Player->GetObjectGuid();
            }
    
            void EnterCombat(Unit* /*p_Unit*/) override
            {
                m_LightsReunionTimer = urand(3000, 8000);
                m_LolyLightTimer = 0;
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (l_Player->GetHealthPct() <= 50.0f)
                    {
                        me->CastSpell(l_Player, eSpells::SPELL_HOLY_LIGHT);
                        return;
                    }
                }
    
                if (m_LightsReunionTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SPELL_LIGHTS_REUNION);
    
                    m_LightsReunionTimer = urand(8000, 13000);
                }
                else m_LightsReunionTimer -= p_Diff;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lady_liadrin_guardian_79823AI(creature);
        }
};

class npc_cordana_felsong_guardian_79783 : public CreatureScript
{
    public:
        npc_cordana_felsong_guardian_79783() : CreatureScript("npc_cordana_felsong_guardian_79783") {}
    
        struct npc_cordana_felsong_guardian_79783AI : public ScriptedAI
        {
            npc_cordana_felsong_guardian_79783AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_SHADOW_STRIKES = 166911,
                SPELL_SHADOWSTEP = 167422,
            };
    
            uint32 m_ShadowStrikesTimer;
            uint32 m_ShadowstepTimer;
    
            void EnterCombat(Unit* /*p_Unit*/) override
            {
                m_ShadowstepTimer = urand(3000, 5000);
                m_ShadowStrikesTimer = 5000;
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                if (m_ShadowstepTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SPELL_SHADOWSTEP);
    
                    m_ShadowstepTimer = urand(5000, 8000);
                }
                else m_ShadowstepTimer -= p_Diff;
    
                if (m_ShadowStrikesTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SPELL_SHADOW_STRIKES);
    
                    m_ShadowStrikesTimer = urand(12000, 15000);
                }
                else m_ShadowStrikesTimer -= p_Diff;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cordana_felsong_guardian_79783AI(creature);
        }
};

class npc_qiana_moonshadow_guardian_79775 : public CreatureScript
{
    public:
        npc_qiana_moonshadow_guardian_79775() : CreatureScript("npc_qiana_moonshadow_guardian_79775") {}
    
        struct npc_qiana_moonshadow_guardian_79775AI : public ScriptedAI
        {
            npc_qiana_moonshadow_guardian_79775AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_RIP = 79829,
                SPELL_CAT_FORM = 32356,
            };
    
            uint32 m_RipTimer;
    
            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetShapeshiftForm(ShapeshiftForm::FORM_CREATURE_CAT);
                //me->CastSpell(me, eSpells::SPELL_CAT_FORM, true);
                me->setPowerType(Powers::POWER_ENERGY);
                me->SetMaxPower(Powers::POWER_ENERGY, 100);
                me->SetPower(Powers::POWER_ENERGY, 100, true);
            }
    
            void EnterCombat(Unit* /*p_Unit*/) override
            {
                m_RipTimer = urand(3000, 5000);
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                if (m_RipTimer <= p_Diff)
                {
                    if (Unit* l_Target = me->getVictim())
                        if (me->IsWithinMeleeRange(l_Target))
                            me->CastSpell(l_Target, eSpells::SPELL_RIP);
    
                    m_RipTimer = urand(5000, 10000);
                }
                else m_RipTimer -= p_Diff;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_qiana_moonshadow_guardian_79775AI(creature);
        }
};

class npc_kelidan_the_breaker_79702 : public CreatureScript
{
    public:
        npc_kelidan_the_breaker_79702() : CreatureScript("npc_kelidan_the_breaker_79702") {}
    
        struct npc_kelidan_the_breaker_79702AI : public ScriptedAI
        {
            npc_kelidan_the_breaker_79702AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eEvents
            {
                EVENT_VOID_BOLT_VOLLEY = 1,
                EVENT_VOID_VACUUM,
                EVENT_VOID_SHELL,
            };
    
            enum eSpells
            {
                SPELL_VOID_CHANNELING = 160677,
                SPELL_VOID_BOLT_VOLLEY = 165875,
                SPELL_VOID_VACUUM = 165871,
                SPELL_VOID_SHELL = 165880,
            };
    
            enum eTexts
            {
                SAY_AGGRO = 0,
                SAY_VOID_VACUUM,
                SAY_VOID_SHELL,
                SAY_DEFEAT,
            };
    
            EventMap m_Events;
            bool m_Defeated = false;
    
            void Reset() override
            {
                m_Events.Reset();
                m_Defeated = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, eUnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC));
            }
    
            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const*  /*p_SpellInfo*/) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;
                    me->SetHealth(1);
                }
    
                if (me->GetHealthPct() <= 2.0f && !m_Defeated)
                {
                    m_Defeated = true;
                    me->CastStop();
                    me->CombatStop();
                    me->SetReactState(REACT_PASSIVE);
                    me->AddUnitState(UNIT_STATE_CANNOT_TURN | UNIT_STATE_CANNOT_TURN);
                    me->SetFlag(UNIT_FIELD_FLAGS, eUnitFlags(UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC));
    
                    me->AI()->Talk(eTexts::SAY_DEFEAT);
                    me->DespawnOrUnsummon(3000);
    
                    std::list<HostileReference*> l_ThreatList = me->getThreatManager().getThreatList();
                    for (HostileReference* l_Ref : l_ThreatList)
                    {
                        if (Unit* l_Unit = l_Ref->getTarget())
                        {
                            if (l_Unit == nullptr || !l_Unit->IsPlayer())
                                continue;
    
                            l_Unit->ToPlayer()->KilledMonsterCredit(me->GetEntry());
                            l_Unit->ToPlayer()->CancelStandaloneScene(l_Unit->ToPlayer()->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_KELIDAN_PARTICLES_DAMAGE));
                        }
                    }
                }
            }
    
            void EnterCombat(Unit* /*p_Unit*/) override
            {
                me->AI()->Talk(eTexts::SAY_AGGRO);
                m_Defeated = false;
    
                m_Events.ScheduleEvent(eEvents::EVENT_VOID_BOLT_VOLLEY, 2000);
                m_Events.ScheduleEvent(eEvents::EVENT_VOID_VACUUM, 13000);
                m_Events.ScheduleEvent(eEvents::EVENT_VOID_SHELL, 23000);
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                m_Events.Update(p_Diff);
                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EVENT_VOID_BOLT_VOLLEY:
                    {
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, eSpells::SPELL_VOID_BOLT_VOLLEY);
    
                        m_Events.ScheduleEvent(eEvents::EVENT_VOID_BOLT_VOLLEY, urand(3000, 4000));
                        break;
                    }
                    case eEvents::EVENT_VOID_VACUUM:
                    {
                        if (Unit* l_Target = me->getVictim())
                        {
                            me->AI()->Talk(eTexts::SAY_VOID_VACUUM);
                            me->CastSpell(me, eSpells::SPELL_VOID_VACUUM);
                        }
    
                        m_Events.ScheduleEvent(eEvents::EVENT_VOID_VACUUM, 28000);
                        break;
                    }
                    case eEvents::EVENT_VOID_SHELL:
                    {
                        if (Unit* l_Target = me->getVictim())
                        {
                            me->AI()->Talk(eTexts::SAY_VOID_SHELL);
                            me->CastSpell(me, eSpells::SPELL_VOID_SHELL);
                        }
    
                        m_Events.ScheduleEvent(eEvents::EVENT_VOID_SHELL, 32000);
                        break;
                    }
                    default:
                        break;
                }
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kelidan_the_breaker_79702AI(creature);
        }
};

class npc_ganar_prisoner_79917 : public CreatureScript
{
    public:
        npc_ganar_prisoner_79917() : CreatureScript("npc_ganar_prisoner_79917") {}
    
        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, const Quest* p_Quest) override
        {
            if (p_Creature->GetEntry() == TanaanIntroCreatures::NPC_GANAR)
            {
                if (p_Quest->GetQuestId() == TanaanIntroQuests::QUEST_POLISHING_THE_IRON_THRONE)
                    p_Creature->AI()->PersonalTalk(0, p_Player->GetGUID());
                else if (p_Quest->GetQuestId() == TanaanIntroQuests::QUEST_THE_PRODIGAL_FROSTWOLF)
                    p_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_GANAR_FREED, 16, *p_Player);
            }
    
            return true;
        }
    
        struct npc_ganar_prisoner_79917AI : public ScriptedAI
        {
            npc_ganar_prisoner_79917AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_CHAIN = 163701,
                SPELL_THROW_SHET = 171739,
                SPELL_SHET_AURA = 163367,
            };
    
            enum eAnims
            {
                ANIM_THROW_SHET = 7109,
                ANIM_UNK = 7107
            };
    
            void InitializeAI() override
            {
                me->SetWalk(true);
                me->SetReactState(REACT_PASSIVE);
            }
    
            void MovementInform(uint32 /*type*/, uint32 p_Id) override
            {
                if (!me->HasAura(eSpells::SPELL_CHAIN))
                {
                    if (Creature* l_RopeBunny = me->FindNearestCreature(79403, 10.0f))
                        l_RopeBunny->CastSpell(me, eSpells::SPELL_CHAIN, true);
                }
    
                if (p_Id == 0)
                {
                    me->SetFacingTo(4.834f);
                    me->AddDelayedEvent([this]() -> void
                    {
                        me->AddAura(eSpells::SPELL_SHET_AURA, me);
                    }, 3000);
                }
                else if (p_Id == 1)
                {
                    me->PlayOneShotAnimKitId(eAnims::ANIM_THROW_SHET);
                    me->CastSpell(4181.102f, -2059.151f, 0.304f, eSpells::SPELL_THROW_SHET, true);
                    me->RemoveAura(eSpells::SPELL_SHET_AURA);
                }
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ganar_prisoner_79917AI(creature);
        }
};

class npc_overseer_gotrigg_80574 : public CreatureScript
{
    public:
        npc_overseer_gotrigg_80574() : CreatureScript("npc_overseer_gotrigg_80574") {}
    
        struct npc_overseer_gotrigg_80574AI : public ScriptedAI
        {
            npc_overseer_gotrigg_80574AI(Creature* creature) : ScriptedAI(creature) {}
    
            enum eSpells
            {
                SPELL_BURNING_BODY = 166401,
                SPELL_BURNING_RADIANCE = 166403
            };
    
            uint32 m_BurningBodyTimer;
            uint32 m_BurningRadianceTimer;
    
            void JustReachedHome() override
            {
                me->PlayOneShotAnimKitId(6340);
            }
    
            void EnterCombat(Unit* /*p_Who*/) override
            {
                //Yell Aggro
                me->AI()->Talk(0);
                m_BurningRadianceTimer = 5000;
                m_BurningBodyTimer = 11000;
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                if (m_BurningRadianceTimer <= p_Diff)
                {
                    me->CastSpell(me, eSpells::SPELL_BURNING_RADIANCE);
                    m_BurningRadianceTimer = 20000;
                }
                else m_BurningRadianceTimer -= p_Diff;
    
                if (m_BurningBodyTimer <= p_Diff)
                {
                    if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                        me->CastSpell(l_Target, eSpells::SPELL_BURNING_BODY);
    
                    m_BurningBodyTimer = 18000;
                }
                else m_BurningBodyTimer -= p_Diff;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_overseer_gotrigg_80574AI(creature);
        }
};

class npc_thaelin_darkanvil_78568 : public CreatureScript
{
    public:
        npc_thaelin_darkanvil_78568() : CreatureScript("npc_thaelin_darkanvil_78568") {}
    
        struct npc_thaelin_darkanvil_78568AI : public ScriptedAI
        {
            npc_thaelin_darkanvil_78568AI(Creature* p_Creature) : ScriptedAI(p_Creature) {}
    
            enum eGossipData
            {
                GOSSIP_MENU_ID = 16518,
                GOSSIP_ID = 0,
            };
    
            void sGossipSelect(Player* p_Player, uint32 p_Sender, uint32 p_Action) override
            {
                if (p_Sender == eGossipData::GOSSIP_MENU_ID && p_Action == eGossipData::GOSSIP_ID)
                {
                    p_Player->CastSpell(me, TanaanIntroSpells::SPELL_SUMMON_THAELIN_GUARDIAN, true);
                    p_Player->KilledMonsterCredit(80880); //Speak with Thaelin credit
                    p_Player->PlayerTalkClass->SendCloseGossip();
                    me->DestroyForPlayer(p_Player);
                }
            }
        };
    
        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_thaelin_darkanvil_78568AI(p_Creature);
        }
};

class npc_thaelin_darkanvil_guardian_80886 : public CreatureScript
{
    public:
        npc_thaelin_darkanvil_guardian_80886() : CreatureScript("npc_thaelin_darkanvil_guardian_80886") {}
    
        struct npc_thaelin_darkanvil_guardian_80886AI : public ScriptedAI
        {
            npc_thaelin_darkanvil_guardian_80886AI(Creature* p_Creature) : ScriptedAI(p_Creature) {}
    
            enum eTalk
            {
                SAY_SPAWN = 0,
                SAY_NEAR_GOGLUK,
                SAY_NEAR_CONSOLE
            };
    
            ObjectGuid m_PlayerGUID;
            bool m_GoglukFound = false;
            bool m_ConsoleFound = false;
    
            void IsSummonedBy(Unit* p_Summoner) override
            {
                Player* l_Player = p_Summoner->ToPlayer();
                if (!l_Player)
                {
                    me->DespawnOrUnsummon();
                    return;
                }
    
                m_PlayerGUID = l_Player->GetObjectGuid();
                me->AI()->PersonalTalk(eTalk::SAY_SPAWN, m_PlayerGUID);
            }
    
            void MovementInform(uint32 type, uint32 p_Id) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (p_Id == 1)
                {
                    if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                    {
                        l_Player->KilledMonsterCredit(80887); //Escort Thaelin Credit
                        l_Player->RemoveAura(TanaanIntroSpells::SPELL_SUMMON_THAELIN_GUARDIAN);
                        me->DespawnOrUnsummon();
                    }
                }
            }
    
            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->isSummon())
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID);
                    if (!l_Player || !l_Player->IsInWorld())
                    {
                        me->DespawnOrUnsummon();
                        l_Player->RemoveAura(TanaanIntroSpells::SPELL_SUMMON_THAELIN_GUARDIAN);
                        return;
                    }
    
                    if (!l_Player->HasQuest(TanaanIntroQuests::QUEST_TAKING_A_TRIP_TO_THE_TOP_OF_THE_TANK))
                    {
                        me->DespawnOrUnsummon();
                        l_Player->RemoveAura(TanaanIntroSpells::SPELL_SUMMON_THAELIN_GUARDIAN);
                        return;
                    }
    
                    if (me->GetExactDist(4065.759f, -1955.649f, 27.860f) <= 40.0f && !m_GoglukFound)
                    {
                        m_GoglukFound = true;
                        me->AI()->PersonalTalk(eTalk::SAY_NEAR_GOGLUK, m_PlayerGUID);
                        me->SetCanFly(true);
                    }
    
                    if (l_Player->GetExactDist(4063.770f, -2020.122f, 75.473f) <= 15.0f && !m_ConsoleFound)
                    {
                        m_ConsoleFound = true;
                        me->AI()->PersonalTalk(eTalk::SAY_NEAR_CONSOLE, m_PlayerGUID);
                        me->SetReactState(REACT_PASSIVE);
                        me->GetMotionMaster()->MovePoint(1, 4063.770f, -2020.120f, 75.473f);
    
                        //Add delayed event in case the waypoint fails
                        me->AddDelayedEvent([this]() -> void
                        {
                            if (Player* l_Player = ObjectAccessor::GetPlayer(*me, m_PlayerGUID))
                            {
                                l_Player->KilledMonsterCredit(80887); //Escort Thaelin Credit
                                l_Player->RemoveAura(TanaanIntroSpells::SPELL_SUMMON_THAELIN_GUARDIAN);
                                me->DespawnOrUnsummon();
                            }
                        }, 10000);
                    }
                }
    
                if (!UpdateVictim())
                    return;
    
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
    
                DoMeleeAttackIfReady();
            }
        };
    
        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_thaelin_darkanvil_guardian_80886AI(p_Creature);
        }
};

class npc_thaelin_darkanvil_80521 : public CreatureScript
{
    public:
        npc_thaelin_darkanvil_80521() : CreatureScript("npc_thaelin_darkanvil_80521") {}
    
        enum eTalks
        {
            YELL_QUEST_ACCEPT = 0,
    
            //Handled in OnSceneTriggerEvent actually
            SAY_INSTRUCTIONS
        };
    
        bool OnQuestAccept(Player* p_Player, Creature* p_Creature, const Quest* p_Quest) override
        {
            if (p_Creature->GetEntry() == TanaanIntroCreatures::NPC_THAELIN_FINALE)
            {
                if (p_Quest->GetQuestId() == TanaanIntroQuests::QUEST_A_TASTE_OF_IRON)
                    p_Creature->AI()->PersonalTalk(eTalks::YELL_QUEST_ACCEPT, p_Player->GetGUID());
            }
            return true;
        }
};

// @Gameobject: Mark of _* (233056, 233057, 233058, 233059, 229600) - Dark Portal area
class gob_mark_of_tanaan : public GameObjectScript
{
    public:
        gob_mark_of_tanaan() : GameObjectScript("gob_mark_of_tanaan") {}
    
        bool OnGossipHello(Player* p_Player, GameObject* p_Gameobject) override
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_ONSLAUGHTS_END) == QUEST_STATUS_INCOMPLETE)
            {
                if (p_Gameobject->GetEntry() == TanaanIntroGameObjects::GOB_MARK_OF_THE_BLEEDING_HOLLOW)
                {
                    p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_TERONGOR_FREED);
                    p_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_TERONGOR_FREED, 16, *p_Player);
                }
                else if (p_Gameobject->GetEntry() == TanaanIntroGameObjects::GOB_MARK_OF_THE_SHADOWMOON)
                {
                    p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_CHOGALL_FREED);
                    p_Player->PlayStandaloneScene(TanaanIntroScenes::SCENE_CHOGALL_FREED, 16, *p_Player);
                }
            }
            else if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_PORTALS_POWER) == QUEST_STATUS_INCOMPLETE)
            {
                uint8 l_TalkId = 0;
                if (p_Gameobject->GetEntry() == TanaanIntroGameObjects::GOB_MARK_OF_THE_BLACKROCK)
                {
                    p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_MARK_OF_BLACKROCK_DESTROYED);
                    l_TalkId = 1;
                }
                else if (p_Gameobject->GetEntry() == TanaanIntroGameObjects::GOB_MARK_OF_THE_SHATTERED_HAND)
                {
                    p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_MARK_OF_SHATTERED_HAND_DESTROYED);
                    l_TalkId = 2;
                }
                else if (p_Gameobject->GetEntry() == TanaanIntroGameObjects::GOB_MARK_OF_THE_BURNING_BLADE)
                {
                    p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_MARK_OF_THE_BURNING_HAND_DESTROYED);
                    l_TalkId = 3;
                }
    
                if (Creature* l_GulDan = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_GULDAN, 50.0f))
                    l_GulDan->AI()->PersonalTalk(l_TalkId, p_Player->GetGUID());
            }
            return true;
        }
};

// @Gameobject: Stasis rune (233104) - Dark Portal area
class gob_stasis_rune_233104 : public GameObjectScript
{
    public:
        gob_stasis_rune_233104() : GameObjectScript("gob_stasis_rune_233104") {}
    
        bool OnGossipHello(Player* p_Player, GameObject* p_Gameobject) override
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_PORTALS_POWER) == QUEST_STATUS_INCOMPLETE)
            {
                p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_STASIS_RUNE_DESTROYED);
                p_Player->PlayScene(TanaanIntroScenes::SCENE_GULDAN_FREED, p_Player);
            }
    
            return true;
        }
};

// @Gameobject: Bleeding Hollow Cage (229353 and 229352) - Heartblood area
class gob_bleeding_hollow_cages : public GameObjectScript
{
    public:
        gob_bleeding_hollow_cages() : GameObjectScript("gob_bleeding_hollow_cages") {}
    
        bool OnGossipHello(Player* p_Player, GameObject* p_Gameobject) override
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_BLED_DRY_A) == QUEST_STATUS_INCOMPLETE ||
                p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_BLED_DRY_H) == QUEST_STATUS_INCOMPLETE)
            {
                if (p_Gameobject->GetEntry() == TanaanIntroGameObjects::GOB_BLEEDING_HOLLOW_CAGE_EAST)
                {
                    if (p_Player->GetTeamId() == TEAM_HORDE)
                        p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_EASTERN_CAGE_OPENED_H);
                    else if (p_Player->GetTeamId() == TEAM_ALLIANCE)
                        p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_EASTERN_CAGE_OPENED_A);
    
                    p_Player->PlayScene(TanaanIntroScenes::SCENE_RELEASE_EASTERN_CAGES, p_Player);
                }
                else if (p_Gameobject->GetEntry() == TanaanIntroGameObjects::GOB_BLEEDING_HOLLOW_CAGE_SOUTH)
                {
                    if (p_Player->GetTeamId() == TEAM_HORDE)
                        p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_SOUTHERN_CAGE_OPENED_H);
                    else if (p_Player->GetTeamId() == TEAM_ALLIANCE)
                        p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_SOUTHERN_CAGE_OPENED_A);
    
                    p_Player->PlayScene(TanaanIntroScenes::SCENE_RELEASE_SOUTHERN_CAGES, p_Player);
                }
            }
            return true;
        }
};

class gob_makeshift_plunger_231066 : public GameObjectScript
{
    public:
        gob_makeshift_plunger_231066() : GameObjectScript("gob_makeshift_plunger_231066") {}
    
        bool OnGossipHello(Player* p_Player, GameObject* p_Gameobject) override
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_GUNPOWDER_PLOT) == QUEST_STATUS_INCOMPLETE &&
                p_Player->HasItemCount(112323, 1)) //Item 112323 - Blackrock Powder Keg
            {
                //Spell 162575 - Assist Dwarves (Credit for quest)
                p_Player->CastSpell(p_Player, 162575, true);
            }
            return true;
        }
};

class gob_siege_tank_turrets : public GameObjectScript
{
    public:
        gob_siege_tank_turrets() : GameObjectScript("gob_siege_tank_turrets") {}
    
        bool OnGossipHello(Player* p_Player, GameObject* p_Gameobject) override
        {
            if (p_Gameobject->GetEntry() == TanaanIntroGameObjects::GOB_WORLDBREAKER_SIDE_TURRET)
            {
                if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_A_TASTE_OF_IRON) == QUEST_STATUS_INCOMPLETE &&
                    p_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_KILL_200_ORCS) != 200)
                {
                    p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_TASTE_OF_IRON_PERIODIC_CHECK, true);
                    p_Gameobject->DestroyForPlayer(p_Player);
                }
            }
    
            return true;
        }
    
        struct gob_siege_tank_turretsAI : public GameObjectAI
        {
            gob_siege_tank_turretsAI(GameObject* go) : GameObjectAI(go)
            {
                go->SetAIAnimKitId(5460);
            }
        };
    
        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new gob_siege_tank_turretsAI(go);
        }
};

class gob_main_cannon_trigger_232538 : public GameObjectScript
{
    public:
        gob_main_cannon_trigger_232538() : GameObjectScript("gob_main_cannon_trigger_232538") {}
    
        bool OnGossipHello(Player* p_Player, GameObject* p_Gameobject) override
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_A_TASTE_OF_IRON) == QUEST_STATUS_INCOMPLETE &&
                p_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_KILL_200_ORCS) == 200)
            {
                //Warlords reveal movie + portal destroyed
                p_Player->SendMovieStart(187);
                p_Player->QuestObjectiveSatisfy(p_Gameobject->GetEntry(), 1, QuestObjectiveType::QUEST_OBJECTIVE_TYPE_GO, p_Gameobject->GetGUID());
            }
    
            return true;
        }
};

class spell_taste_of_iron_game_aura_164042 : public SpellScriptLoader
{
    public:
        spell_taste_of_iron_game_aura_164042() : SpellScriptLoader("spell_taste_of_iron_game_aura_164042") {}
    
        class spell_taste_of_iron_game_aura_164042_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_taste_of_iron_game_aura_164042_AuraScript);
    
            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;
    
                if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_A_TASTE_OF_IRON) == QUEST_STATUS_NONE ||
                    l_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_KILL_200_ORCS) == 200)
                {
                    l_Player->RemoveAura(TanaanIntroSpells::SPELL_TASTE_OF_IRON_PERIODIC_CHECK);
                    //l_Player->CastSpell(l_Player, TanaanIntroSpells::SPELL_TASTE_OF_IRON_TELEPORT, true);
                }
            }
    
            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_taste_of_iron_game_aura_164042_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };
    
        AuraScript* GetAuraScript() const override
        {
            return new spell_taste_of_iron_game_aura_164042_AuraScript();
        }
};

class spell_khadgars_watch_absorb_dmg_167410 : public SpellScriptLoader
{
    public:
        spell_khadgars_watch_absorb_dmg_167410() : SpellScriptLoader("spell_khadgars_watch_absorb_dmg_167410") {}
    
        class spell_khadgars_watch_absorb_dmg_167410_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_khadgars_watch_absorb_dmg_167410_AuraScript);
    
            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }
    
            void Absorb(AuraEffect* /*p_AurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;
    
                if (dmgInfo.GetAmount() >= l_Player->GetHealth())
                {
                    if (!l_Player->HasAura(TanaanIntroSpells::SPELL_KHADGARS_WATCH_PACIFY))
                        l_Player->AddAura(TanaanIntroSpells::SPELL_KHADGARS_WATCH_PACIFY, l_Player);
    
                    absorbAmount = dmgInfo.GetAmount();
                }
            }
    
            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_khadgars_watch_absorb_dmg_167410_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_khadgars_watch_absorb_dmg_167410_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };
    
        AuraScript* GetAuraScript() const
        {
            return new spell_khadgars_watch_absorb_dmg_167410_AuraScript();
        }
};

class spell_khadgars_watch_periodic_check_167421 : public SpellScriptLoader
{
    public:
        spell_khadgars_watch_periodic_check_167421() : SpellScriptLoader("spell_khadgars_watch_periodic_check_167421") {}
    
        class spell_khadgars_watch_periodic_check_167421_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_khadgars_watch_periodic_check_167421_AuraScript);
    
            void HandleOnPeriodic(AuraEffect const* p_AuraEffect)
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;
    
                if (!l_Player->HasAura(TanaanIntroSpells::SPELL_KHADGARS_WATCH_ABSORB_DMG) &&
                    !l_Player->HasAura(TanaanIntroSpells::SPELL_TIME_LOCKED))
                    l_Player->AddAura(TanaanIntroSpells::SPELL_KHADGARS_WATCH_ABSORB_DMG, l_Player);
    
                if (l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_HOME_STRETCH_H) == QUEST_STATUS_INCOMPLETE ||
                    l_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_HOME_STRETCH_A) == QUEST_STATUS_INCOMPLETE)
                {
                    if (l_Player->GetDistance(4017.221f, -2004.734f, 30.482f) <= 40.0f && !l_Player->HasAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_A))
                        l_Player->AddAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_A, l_Player);
    
                    if (l_Player->GetDistance(3915.951f, -2021.819f, 18.915f) <= 40.0f && !l_Player->HasAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_B))
                        l_Player->AddAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_B, l_Player);
    
                    if (l_Player->GetDistance(3817.744f, -2071.290f, 14.870f) <= 40.0f && !l_Player->HasAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_C))
                        l_Player->AddAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_C, l_Player);
    
                    if (l_Player->GetDistance(3745.331f, -2042.948f, 13.079f) <= 40.0f && !l_Player->HasAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_D))
                        l_Player->AddAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_D, l_Player);
    
                    if (l_Player->GetDistance(3709.763f, -2055.590f, 9.608f) <= 40.0f && !l_Player->HasAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_E))
                        l_Player->AddAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_E, l_Player);
    
                    if (l_Player->GetDistance(3625.976f, -2101.866f, 7.386f) <= 90.0f && !l_Player->HasAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_F))
                        l_Player->AddAura(TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_F, l_Player);
                }
            }
    
            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_khadgars_watch_periodic_check_167421_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };
    
        AuraScript* GetAuraScript() const override
        {
            return new spell_khadgars_watch_periodic_check_167421_AuraScript();
        }
};

// @General: Handle player scripts
class playerScript_tanaan_intro : public PlayerScript
{
private:
    struct RemoveList
    {
        uint32 QuestId;
        uint32 AuraId;
    };

    struct QuestCheckPoint
    {
        uint32 QuestId;
        Position Pos;
    };

    enum eAreaIndex
    {
        AREA_THE_DARK_PORTAL = 0,
        AREA_HEARTBLOOD,
        AREA_THE_BLEEDING_ALTAR,
        AREA_TARTHOG_BRIDGE,
        AREA_UMBRAL_HALLS,
        AREA_KARGATHAR_PROVING_GROUNS,
        AREA_BLACKROCK_QUARRY,
        AREA_BLACKROCK_QUARRY_FIGHT,
    };

public:
    playerScript_tanaan_intro() : PlayerScript("playerScript_tanaan_intro") {}

    std::vector<QuestCheckPoint> l_CheckpointList =
    {
        {TanaanIntroQuests::QUEST_THE_COST_OF_WAR,                          {3940.366f, -2508.106f, 69.394f, 2.478f}},
        {TanaanIntroQuests::QUEST_BLAZE_OF_GLORY,                           {3839.209f, -2782.117f, 93.842f, 1.245f}},
        {TanaanIntroQuests::QUEST_ALTAR_ALTERCATION,                        {4194.588f, -2785.064f, 26.459f, 3.701f}},
        {TanaanIntroQuests::QUEST_KARGATHAR_PROVING_GROUNDS,                {4233.516f, -2812.768f, 17.141f, 3.266f}},
        {TanaanIntroQuests::QUEST_KILL_YOUR_HUNDRED,                        {4502.938f, -2637.876f, 1.459f, 0.145f}},
        {TanaanIntroQuests::QUEST_YREL_A,                                   {4511.928f, -2496.728f, 25.711f, 0.578f}},
        {TanaanIntroQuests::QUEST_YREL_H,                                   {4511.928f, -2496.728f, 25.711f, 0.578f}},
        {TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_A,                    {4608.470f, -2242.531f, 14.624f, 5.568f}},
        {TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_H,                    {4608.470f, -2242.531f, 14.624f, 5.568f}},
        {TanaanIntroQuests::QUEST_THE_BATTLE_OF_THE_FORGE,                  {4403.725f, -2089.388f, 2.056f, 0.081f}},
        {TanaanIntroQuests::QUEST_TAKING_A_TRIP_TO_THE_TOP_OF_THE_TANK,     {4065.743f, -2017.411f, 75.242f, 4.709f}},
        {TanaanIntroQuests::QUEST_THE_HOME_STRETCH_H,                       {5535.01f, 5019.88f, 12.637f, 5.174f}},
        {TanaanIntroQuests::QUEST_THE_HOME_STRETCH_A,                       {2332.718f, 432.501f, 1.010f, 2.557f}}, //Location not from sniffs
    };

    // Used to apply/remove phases on SceneTrigger/QuestAbandon
    void HandlePhases(Player* p_Player, uint8 p_Area, bool p_Apply = true)
    {
        switch (p_Area)
        {
            case eAreaIndex::AREA_HEARTBLOOD:
            {
                p_Player->SetInPhase(TanaanIntroPhases::PHASE_HEARTBLOOD_HUT, true, p_Apply);
                p_Player->SetInPhase(TanaanIntroPhases::PHASE_HEARTBLOOD_HUT_DEAD_ORCS, true, p_Apply);
                break;
            }
            case eAreaIndex::AREA_UMBRAL_HALLS:
            {
                p_Player->SetInPhase(TanaanIntroPhases::PHASE_SEE_BLOCKING_BOULDER_UMBRAL_HALLS, true, p_Apply);
                break;
            }
            case eAreaIndex::AREA_BLACKROCK_QUARRY:
            {
                if (p_Player->GetTeamId() == TEAM_HORDE)
                    p_Player->SetInPhase(TanaanIntroPhases::PHASE_BLACKROCK_QUARRY_FREED_ORCS, true, p_Apply);
                else
                    p_Player->SetInPhase(TanaanIntroPhases::PHASE_BLACKROCK_QUARRY_FREED_DRAENEI, true, p_Apply);

                p_Player->SetInPhase(TanaanIntroPhases::PHASE_BLACKROCK_QUARRY_ARMY, true, p_Apply);
                break;
            }
            case eAreaIndex::AREA_BLACKROCK_QUARRY_FIGHT:
            {
                p_Player->SetInPhase(TanaanIntroPhases::PHASE_BLACKROCK_QUARRY_KHADGAR_AT_DAM, true, p_Apply);
                break;
            }
            default:
                break;
        }
    }

    void BlackrockTerrainSwap(Player* p_Player)
    {
        std::set<uint32> l_PhaseId, l_Terrainswap, l_InactiveTerrainSwap;
        l_Terrainswap.insert((uint32)TanaanIntroZones::TERRAIN_SWAPID);
        l_InactiveTerrainSwap.insert((uint32)TanaanIntroZones::TERRAIN_SWAPID);

        p_Player->GetSession()->SendSetPhaseShift(l_PhaseId, l_Terrainswap, l_InactiveTerrainSwap);
    }

    void PlayScene(Player* p_Player, uint32 p_PackageId, uint32 p_PlaybackFlags = 0)
    {
        if (p_PlaybackFlags == 0)
            p_Player->PlayScene(p_PackageId, p_Player);
        else
            p_Player->PlayStandaloneScene(p_PackageId, p_PlaybackFlags, *p_Player);
    };

    void CancelScene(Player* p_Player, uint32 p_PackageId)
    {
        p_Player->CancelStandaloneScene(p_Player->GetSceneInstanceIDByPackage(p_PackageId));
    }

    void ClearEndSceneAuras(Player* p_Player)
    {
        for (uint32 l_SpellId = TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_A; l_SpellId < TanaanIntroSpells::SPELL_IRON_BASTION_PROGRESS_G; l_SpellId++)
        {
            if (p_Player->HasAura(l_SpellId))
                p_Player->RemoveAura(l_SpellId);
        }
    }

    void TeleportFinale(Player* p_Player)
    {
        if (p_Player->GetTeamId() == TEAM_HORDE)
        {
            p_Player->RemoveAura(TanaanIntroSpells::SPELL_AURA_FINALE_HORDE_BOAT);
            p_Player->TeleportTo(1116, l_CheckpointList[11].Pos);
        }
        else
        {
            p_Player->RemoveAura(TanaanIntroSpells::SPELL_AURA_FINALE_ALLIANCE_BOAT);
            p_Player->TeleportTo(1116, l_CheckpointList[12].Pos);
        }
    }

    void OnQuestAccept(Player* p_Player, const Quest* p_Quest) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        switch (p_Quest->GetQuestId())
        {
            case TanaanIntroQuests::QUEST_ONSLAUGHTS_END:
            {
                PlayScene(p_Player, TanaanIntroScenes::SCENE_DETONATION_DWARVES, 16);
                break;
            }
            case TanaanIntroQuests::QUEST_BLAZE_OF_GLORY:
            {
                PlayScene(p_Player, TanaanIntroScenes::SCENE_TRAIL_OF_FLAME, 16);

                // Security check because of phase change
                if (p_Player->IsQuestRewarded(TanaanIntroQuests::QUEST_FLAG_ARMY_PUSHED))
                    break;
                else
                    PlayScene(p_Player, TanaanIntroScenes::SCENE_HOLDOUT, 16);

                break;
            }
            case TanaanIntroQuests::QUEST_ALTAR_ALTERCATION:
            {
                PlayScene(p_Player, TanaanIntroScenes::SCENE_EYE_OF_KILROGG, 16);
                break;
            }
            case TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_A:
            case TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_H:
            {
                PlayScene(p_Player, TanaanIntroScenes::SCENE_RING_OF_FIRE, 16);
                break;
            }
            case TanaanIntroQuests::QUEST_KILL_YOUR_HUNDRED:
            {
                PlayScene(p_Player, TanaanIntroScenes::SCENE_ENTER_KARGATH_ARENA, 16);
                break;
            }
            case TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_A:
            case TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_H:
            {
                if (p_Player->GetTeamId() == TEAM_ALLIANCE)
                    PlayScene(p_Player, TanaanIntroScenes::SCENE_MALADAAR_AND_YREL_LEAVE, 16);
                else
                    PlayScene(p_Player, TanaanIntroScenes::SCENE_LIADRIN_AND_YREL_LEAVE, 16);

                PlayScene(p_Player, TanaanIntroScenes::SCENE_KELIDAN_PARTICLES_DAMAGE, 16);
                break;
            }
            case TanaanIntroQuests::QUEST_THE_BATTLE_OF_THE_FORGE:
            {
                if (p_Player->GetTeamId() == TEAM_HORDE)
                {
                    CancelScene(p_Player, TanaanIntroScenes::SCENE_ARMING_PRISONERS_H);
                    p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_HORDE_SUMMON_GUARDIANS_AURA, true);

                }
                else
                {
                    CancelScene(p_Player, TanaanIntroScenes::SCENE_ARMING_PRISONERS_A);
                    p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_ALLIANCE_SUMMON_GUARDIANS_AURA, true);
                }

                PlayScene(p_Player, TanaanIntroScenes::SCENE_BLACKHAND_REVEAL);
                break;
            }
            case TanaanIntroQuests::QUEST_A_TASTE_OF_IRON:
            {
                PlayScene(p_Player, TanaanIntroScenes::SCENE_SHOOTING_IRON_HORDE, 20);
                break;
            }
            case TanaanIntroQuests::QUEST_THE_HOME_STRETCH_H:
            case TanaanIntroQuests::QUEST_THE_HOME_STRETCH_A:
            {
                if (p_Player->IsMounted())
                    p_Player->Dismount();

                PlayScene(p_Player, TanaanIntroScenes::SCENE_IRON_BASTION_RUN_FINALE, 16);
                break;
            }
            default:
                break;
        }
    }

    void OnQuestComplete(Player* p_Player, const Quest* p_Quest) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        switch (p_Quest->GetQuestId())
        {
            case TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_A:
            case TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_H:
            {
                if (p_Player->GetAreaId() == TanaanIntroZones::AREA_UMBRAL_HALLS)
                {
                    p_Player->AddDelayedEvent([this, p_Player]() -> void
                    {
                        PlayScene(p_Player, TanaanIntroScenes::SCENE_NERZHUL_REVEAL);
                    }, 3000);
                }

                break;
            }
            case TanaanIntroQuests::QUEST_PREPARE_FOR_BATTLE_A:
            case TanaanIntroQuests::QUEST_PREPARE_FOR_BATTLE_H:
            {
                if (Creature* l_Blackhand = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_BLACKHAND, 300.0f))
                    l_Blackhand->AI()->PersonalTalk(0, p_Player->GetGUID(), 0, TEXT_RANGE_ZONE);

                break;
            }
            case TanaanIntroQuests::QUEST_THE_BATTLE_OF_THE_FORGE:
            {
                if (Creature* l_Khadgar = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_BLACKROCK_KHADGAR, 300.0f))
                    l_Khadgar->AI()->PersonalTalk(0, p_Player->GetGUID(), 0, TEXT_RANGE_ZONE);

                break;
            }
            case TanaanIntroQuests::QUEST_A_TASTE_OF_IRON:
            {
                CancelScene(p_Player, TanaanIntroScenes::SCENE_SHOOTING_IRON_HORDE);
                break;
            }
            default:
                break;
        }
    }

    void OnQuestAbandon(Player* p_Player, const Quest* p_Quest) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        std::vector<RemoveList> l_RemoveList =
        {
            {TanaanIntroQuests::QUEST_FLAG_HANSEL_EVENT_COMPLETE, TanaanIntroSpells::SPELL_HANSEL_EVENT_AURA},
            {TanaanIntroQuests::QUEST_FLAG_THAELIN_EVENT_COMPLETE, TanaanIntroSpells::SPELL_THAELIN_EVENT_AURA},
        };

        switch (p_Quest->GetQuestId())
        {
            case TanaanIntroQuests::QUEST_ONSLAUGHTS_END:
            {
                for (auto const& l_Data : l_RemoveList)
                {
                    if (Quest const* l_Quest = sObjectMgr->GetQuestTemplate(l_Data.QuestId))
                        p_Player->RemoveRewardedQuest(l_Quest->GetQuestId());

                    if (SpellInfo const* l_Spell = sSpellMgr->GetSpellInfo(l_Data.AuraId))
                        p_Player->RemoveAura(l_Spell->Id);
                }

                CancelScene(p_Player, TanaanIntroScenes::SCENE_DETONATION_DWARVES);
                break;
            }
            case TanaanIntroQuests::QUEST_THE_COST_OF_WAR:
            {
                HandlePhases(p_Player, eAreaIndex::AREA_HEARTBLOOD, false);
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_AURA_RUN_AWAY_SCENE);
                break;
            }
            case TanaanIntroQuests::QUEST_BLAZE_OF_GLORY:
            {
                CancelScene(p_Player, TanaanIntroScenes::SCENE_TRAIL_OF_FLAME);
                CancelScene(p_Player, TanaanIntroScenes::SCENE_HOLDOUT);
                break;
            }
            case TanaanIntroQuests::QUEST_ALTAR_ALTERCATION:
            {
                CancelScene(p_Player, TanaanIntroScenes::SCENE_EYE_OF_KILROGG);
                CancelScene(p_Player, TanaanIntroScenes::SCENE_KILROGG_REVEAL);
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_ON_ALTAR);
                break;
            }
            case TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_A:
            case TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_H:
            {
                CancelScene(p_Player, TanaanIntroScenes::SCENE_RING_OF_FIRE);
                p_Player->SetInPhase(TanaanIntroPhases::PHASE_SEE_MALADAAR_AND_DREKTHAR, true, false);
                break;
            }
            case TanaanIntroQuests::QUEST_KILL_YOUR_HUNDRED:
            {
                CancelScene(p_Player, TanaanIntroScenes::SCENE_ENTER_KARGATH_ARENA);
                CancelScene(p_Player, TanaanIntroScenes::SCENE_LEAVE_KARGATH_ARENA);
                p_Player->SetInPhase(TanaanIntroPhases::PHASE_SEE_BLOCKING_BOULDER_UMBRAL_HALLS, true, false);

                // Tp back to the questgiver
                p_Player->TeleportTo(TanaanIntroZones::MAP_TANAAN_INTRO, l_CheckpointList[3].Pos);
                break;
            }
            case TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_A:
            case TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_H:
            {
                HandlePhases(p_Player, eAreaIndex::AREA_BLACKROCK_QUARRY, false);
                CancelScene(p_Player, TanaanIntroScenes::SCENE_KELIDAN_PARTICLES_DAMAGE);
                p_Player->TeleportTo(TanaanIntroZones::MAP_TANAAN_INTRO, l_CheckpointList[5].Pos);
                break;
            }
            case TanaanIntroQuests::QUEST_THE_BATTLE_OF_THE_FORGE:
            {
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_HORDE_SUMMON_GUARDIANS_AURA);
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_ALLIANCE_SUMMON_GUARDIANS_AURA);
                CancelScene(p_Player, TanaanIntroScenes::SCENE_KHADGAR_RUN_TO_DAM);
                HandlePhases(p_Player, eAreaIndex::AREA_BLACKROCK_QUARRY_FIGHT, false);
                break;
            }
            case TanaanIntroQuests::QUEST_THE_GUNPOWDER_PLOT:
            {
                if (p_Player->HasAura(TanaanIntroSpells::SPELL_LEAKY_GUNPOWDER_BARREL))
                    p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_REMOVE_GUNPOWDER_BARREL, true);
                break;
            }
            case TanaanIntroQuests::QUEST_A_TASTE_OF_IRON:
            {
                CancelScene(p_Player, TanaanIntroScenes::SCENE_SHOOTING_IRON_HORDE);
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_BLOODLUST_FROM_SCENE);
                break;
            }
            case TanaanIntroQuests::QUEST_THE_HOME_STRETCH_H:
            case TanaanIntroQuests::QUEST_THE_HOME_STRETCH_A:
            {
                ClearEndSceneAuras(p_Player);
                CancelScene(p_Player, TanaanIntroScenes::SCENE_IRON_BASTION_RUN_FINALE);
                break;
            }
            default:
                break;
        }

    }

    void OnQuestReward(Player* p_Player, const Quest* p_Quest) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        switch (p_Quest->GetQuestId())
        {
            case TanaanIntroQuests::QUEST_ALTAR_ALTERCATION:
            {
                CancelScene(p_Player, TanaanIntroScenes::SCENE_KILROGG_REVEAL);
                CancelScene(p_Player, TanaanIntroScenes::SCENE_EYE_OF_KILROGG);
                break;
            }
            case TanaanIntroQuests::QUEST_PREPARE_FOR_BATTLE_A:
            case TanaanIntroQuests::QUEST_PREPARE_FOR_BATTLE_H:
            {
                if (p_Player->GetTeamId() == TEAM_HORDE)
                {
                    if (Creature* l_Drekthar = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_DREKTHAR, 20.0f))
                        l_Drekthar->AI()->PersonalTalk(0, p_Player->GetGUID());

                    PlayScene(p_Player, TanaanIntroScenes::SCENE_ARMING_PRISONERS_H, 16);
                }
                else
                {
                    if (Creature* l_Exarch = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_EXARCH_MALADAAR, 20.0f))
                        l_Exarch->AI()->PersonalTalk(0, p_Player->GetGUID());

                    PlayScene(p_Player, TanaanIntroScenes::SCENE_ARMING_PRISONERS_A, 16);
                }
                break;
            }
            case TanaanIntroQuests::QUEST_THE_BATTLE_OF_THE_FORGE:
            {
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_HORDE_SUMMON_GUARDIANS_AURA);
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_ALLIANCE_SUMMON_GUARDIANS_AURA);
                CancelScene(p_Player, TanaanIntroScenes::SCENE_KHADGAR_RUN_TO_DAM);
                PlayScene(p_Player, TanaanIntroScenes::SCENE_DAM_EXPLOSION);
                HandlePhases(p_Player, eAreaIndex::AREA_BLACKROCK_QUARRY_FIGHT, false);
                break;
            }
            default:
                break;
        }
    }

    void OnObjectiveValidate(Player* p_Player, uint32 p_QuestID, uint32 p_ObjectiveID) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        switch (p_QuestID)
        {
            case TanaanIntroQuests::QUEST_KILL_YOUR_HUNDRED:
            {
                if (p_ObjectiveID == TanaanIntroQuestObjectives::OBJ_KILL_99_ENEMIES)
                    PlayScene(p_Player, TanaanIntroScenes::SCENE_LEAVE_KARGATH_ARENA, 16);
                //else if (p_ObjectiveID == TanaanIntroQuestObjectives::OBJ_ESCAPE_KARGATH_ARENA)
                //    PlayScene(p_Player, TanaanIntroScenes::SCENE_CAVE_IN);

                break;
            }
            default:
                break;
        }
    }

    // Handle scene end
    void OnSceneCancel(Player* p_Player, uint32 p_SceneInstanceId) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_TERONGOR_FREED))
        {
            if (Creature* l_Grommash = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_GROMMASH_HELLSCREAM, 250.0f))
            {
                l_Grommash->AI()->PersonalTalk(0, p_Player->GetGUID(), 0, TEXT_RANGE_ZONE);
                l_Grommash->AI()->PersonalTalk(1, p_Player->GetGUID(), 5000, TEXT_RANGE_ZONE);
            }
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_CHOGALL_FREED))
        {
            if (Creature* l_Khadgar = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_ARCHMAGE_KHADGAR_1, 200.0f))
            {
                l_Khadgar->AI()->PersonalTalk(2, p_Player->GetGUID(), 0, TEXT_RANGE_ZONE);
                l_Khadgar->AI()->PersonalTalk(3, p_Player->GetGUID(), 5000, TEXT_RANGE_ZONE);
            }
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_GULDAN_REVEAL))
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_PORTALS_POWER) == QUEST_STATUS_INCOMPLETE)
                if (Creature* l_GulDan = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_GULDAN, 50.0f))
                    l_GulDan->AI()->PersonalTalk(0, p_Player->GetGUID());
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_RUNNING_AWAY))
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_COST_OF_WAR) == QUEST_STATUS_COMPLETE)
                HandlePhases(p_Player, eAreaIndex::AREA_HEARTBLOOD);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_RING_OF_FIRE))
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_H) == QUEST_STATUS_COMPLETE ||
                p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_A) == QUEST_STATUS_COMPLETE)
                p_Player->SetInPhase(TanaanIntroPhases::PHASE_SEE_MALADAAR_AND_DREKTHAR, true, true);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_ENTER_KARGATH_ARENA))
        {
            p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_ENTER_KARGATH_ARENA);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_LEAVE_KARGATH_ARENA))
        {
            //Prevent fail in case player cancels the scene
            if (p_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_ESCAPE_KARGATH_ARENA) < 1)
            {
                p_Player->CompleteQuest(TanaanIntroQuests::QUEST_KILL_YOUR_HUNDRED);
                p_Player->TeleportTo(TanaanIntroZones::MAP_TANAAN_INTRO, l_CheckpointList[4].Pos);
            }

            PlayScene(p_Player, TanaanIntroScenes::SCENE_CAVE_IN);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_CAVE_IN))
        {
            HandlePhases(p_Player, eAreaIndex::AREA_UMBRAL_HALLS);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_NERZHUL_REVEAL))
        {
            if (p_Player->GetAreaId() == TanaanIntroZones::AREA_UMBRAL_HALLS)
                p_Player->TeleportTo(TanaanIntroZones::MAP_TANAAN_INTRO, 4519.265f, -2295.608f, 33.832f, 1.314f);

            PlayScene(p_Player, TanaanIntroScenes::SCENE_FROM_CAVE_TO_RIDGE, 16);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_FROM_CAVE_TO_RIDGE))
        {
            HandlePhases(p_Player, eAreaIndex::AREA_BLACKROCK_QUARRY);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_BLACKHAND_REVEAL))
        {
            PlayScene(p_Player, TanaanIntroScenes::SCENE_KHADGAR_RUN_TO_DAM, 16);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_KHADGAR_RUN_TO_DAM))
        {
            GuidSet* l_SummonList = p_Player->GetSummonList(TanaanIntroCreatures::NPC_CORDANA_GUARDIAN);
            for (auto l_Entry : *l_SummonList)
            {
                if (Creature* l_Cordana = ObjectAccessor::GetCreature(*p_Player, l_Entry))
                {
                    if (l_Cordana->GetEntry() == 79783)
                    {
                        l_Cordana->AI()->PersonalTalk(0, p_Player->GetGUID());
                        break;
                    }
                }

            }

            HandlePhases(p_Player, eAreaIndex::AREA_BLACKROCK_QUARRY_FIGHT);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_DAM_EXPLOSION))
        {
            BlackrockTerrainSwap(p_Player);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_BUILDING_EXPLOSION_KEG_LEAKING))
        {
            p_Player->RemoveAura(TanaanIntroSpells::SPELL_LEAKY_GUNPOWDER_BARREL);
        }
        else if (p_SceneInstanceId == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_IRON_BASTION_RUN_FINALE))
        {
            ClearEndSceneAuras(p_Player);
            p_Player->KilledMonsterCredit(81024);
        }
        else
        {
            if (p_Player->GetAuraIDByScenceInstance(p_SceneInstanceId) == TanaanIntroSpells::SPELL_AURA_FINALE_HORDE_BOAT ||
                p_Player->GetAuraIDByScenceInstance(p_SceneInstanceId) == TanaanIntroSpells::SPELL_AURA_FINALE_ALLIANCE_BOAT)
                TeleportFinale(p_Player);
        }
    }

    // Reminder: Scenes triggered by spells have GetSceneInstanceIDByPackage = 0
    void OnSceneTriggerEvent(Player* p_Player, uint32 p_SceneInstanceID, std::string p_Event) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_DETONATION_DWARVES))
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_ONSLAUGHTS_END) == QUEST_STATUS_INCOMPLETE)
            {
                if (p_Event == "Thaelin")
                {
                    if (Creature* l_Thaelin = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_THAELIN_DARKANVIL, 30.0f))
                        if (l_Thaelin->IsAIEnabled)
                            l_Thaelin->AI()->PersonalTalk(0, p_Player->GetGUID());

                    p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_THAELIN_EVENT_FLAG, true);
                }
                else if (p_Event == "Hansel")
                {
                    if (Creature* l_Hansel = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_HANSEL_HEAVYHANDS, 30.0f))
                        if (l_Hansel->IsAIEnabled)
                            l_Hansel->AI()->PersonalTalk(0, p_Player->GetGUID());

                    p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_HANSEL_EVENT_FLAG, true);
                }
            }
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_TRAIL_OF_FLAME))
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_BLAZE_OF_GLORY) == QUEST_STATUS_INCOMPLETE)
            {
                if (p_Event == "Visual")
                {
                    if (!p_Player->HasAura(TanaanIntroSpells::SPELL_AURA_TRAIL_OF_FLAME_BTN))
                        p_Player->AddAura(TanaanIntroSpells::SPELL_AURA_TRAIL_OF_FLAME_BTN, p_Player);
                }
                else if (p_Event == "Credit")
                    p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_HUTS_BURNED);
                else if (p_Event == "Clear")
                {
                    if (p_Player->HasAura(TanaanIntroSpells::SPELL_AURA_TRAIL_OF_FLAME_BTN))
                        p_Player->RemoveAura(TanaanIntroSpells::SPELL_AURA_TRAIL_OF_FLAME_BTN);
                }
            }
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_HOLDOUT))
        {
            if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_FLAG_ARMY_PUSHED) == QUEST_STATUS_NONE)
            {
                if (p_Event == "Push")
                    p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_ARMY_PUSHED_FLAG, true);
            }
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_DESTROY_BRIDGE))
        {
            if (p_Player->HasQuest(TanaanIntroQuests::QUEST_KARGATHAR_PROVING_GROUNDS))
            {
                if (p_Event == "Bridge")
                {
                    p_Player->SetInPhase(TanaanIntroPhases::PHASE_BRIDGE_NOT_DESTROYED, true, false);
                    p_Player->SetInPhase(TanaanIntroPhases::PHASE_BRIDGE_DESTROYED, true, true);
                }
            }
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_RING_OF_FIRE))
        {
            if (p_Event == "Credit")
                p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_RING_OF_FIRE_CREDIT, true);
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_ENTER_KARGATH_ARENA))
        {
            if (p_Event == "Phase")
                p_Player->SetInPhase(TanaanIntroPhases::PHASE_ENTER_KARGATH_ARENA, true, true);
            else if (p_Event == "Credit")
            {
                CancelScene(p_Player, TanaanIntroScenes::SCENE_ENTER_KARGATH_ARENA);
                p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_ENTER_KARGATH_ARENA);
            }
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_LEAVE_KARGATH_ARENA))
        {
            if (p_Event == "Hundred")
            {
                if (p_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_KILL_100_ENEMIES))
                    p_Player->QuestObjectiveSatisfy(TanaanIntroKillCredits::CREDIT_100_KILLS, 100, QUEST_OBJECTIVE_TYPE_NPC);
            }
            else if (p_Event == "Credit")
            {
                CancelScene(p_Player, p_SceneInstanceID);
                p_Player->KilledMonsterCredit(TanaanIntroKillCredits::CREDIT_ESCAPE_KARGATH_ARENA);
            }
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_KELIDAN_PARTICLES_DAMAGE))
        {
            // Spell 165871 - Void Vacuum
            if (p_Event == "Damage" && p_Player->HasAura(165871))
                p_Player->CastSpell(p_Player, 165847, true);
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_NERZHUL_REVEAL))
        {
            if (p_Event == "Teleport")
                p_Player->TeleportTo(TanaanIntroZones::MAP_TANAAN_INTRO, 4519.265f, -2295.608f, 33.832f, 1.314f);
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_DAM_EXPLOSION))
        {
            if (p_Event == "Force Phase")
                BlackrockTerrainSwap(p_Player);
        }
        else if (p_SceneInstanceID == p_Player->GetSceneInstanceIDByPackage(TanaanIntroScenes::SCENE_SHOOTING_IRON_HORDE))
        {
            if (p_Event == "Credit")
                p_Player->KilledMonsterCredit(80016);
            else if (p_Event == "Bloodlust")
                p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_BLOODLUST_FROM_SCENE, true);
            else if (p_Event == "CancelGame")
            {
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_BLOODLUST_FROM_SCENE);
                p_Player->RemoveAura(TanaanIntroSpells::SPELL_TASTE_OF_IRON_PERIODIC_CHECK);
            }
            else if (p_Event == "Instructions")
            {
                if (Creature* l_Thaelin = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_THAELIN_FINALE, 50.0f))
                    l_Thaelin->AI()->PersonalTalk(1, p_Player->GetGUID());
            }
        }
        else
        {
            // Handle Server Triggers by spell scene
            if (p_Player->HasQuest(TanaanIntroQuests::QUEST_THE_GUNPOWDER_PLOT))
            {
                if (Creature* l_Hansel = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_HANSEL_HEAVYHANDS, 50.0f))
                {
                    if (p_Event == "TalkA")
                        l_Hansel->AI()->PersonalTalk(1, p_Player->GetGUID());
                    else if (p_Event == "TalkB")
                        l_Hansel->AI()->PersonalTalk(2, p_Player->GetGUID());
                    else if (p_Event == "TalkC")
                        l_Hansel->AI()->PersonalTalk(3, p_Player->GetGUID());
                    else if (p_Event == "TalkD")
                        l_Hansel->AI()->PersonalTalk(4, p_Player->GetGUID());
                }
            }
            // Teleport out of tanaan zone when you turn in the last quest
            else if (p_Event == "Teleport" && p_Player->GetAreaId() == TanaanIntroZones::AREA_IRON_BASTION)
                TeleportFinale(p_Player);
        }
    }

    void OnLogin(Player* p_Player) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_COST_OF_WAR) == QUEST_STATUS_COMPLETE)
            HandlePhases(p_Player, eAreaIndex::AREA_HEARTBLOOD);

        if (p_Player->IsQuestRewarded(TanaanIntroQuests::QUEST_BLAZE_OF_GLORY) && p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_FLAG_ARMY_PUSHED) == QUEST_STATUS_NONE)
            if (Quest const* l_Quest = sObjectMgr->GetQuestTemplate(TanaanIntroQuests::QUEST_FLAG_ARMY_PUSHED))
                p_Player->RewardQuest(l_Quest, 0, nullptr, false);

        if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_ALTAR_ALTERCATION) == QUEST_STATUS_INCOMPLETE)
            PlayScene(p_Player, TanaanIntroScenes::SCENE_EYE_OF_KILROGG, 16);

        if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_H) == QUEST_STATUS_COMPLETE ||
            p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_A_POTENTIAL_ALLY_A) == QUEST_STATUS_COMPLETE)
            p_Player->SetInPhase(TanaanIntroPhases::PHASE_SEE_MALADAAR_AND_DREKTHAR, true, true);

        if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_KILL_YOUR_HUNDRED) == QUEST_STATUS_COMPLETE)
            HandlePhases(p_Player, eAreaIndex::AREA_UMBRAL_HALLS);

        if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_A) == QUEST_STATUS_COMPLETE ||
            p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_KELIDAN_THE_BREAKER_H) == QUEST_STATUS_COMPLETE)
            HandlePhases(p_Player, eAreaIndex::AREA_BLACKROCK_QUARRY);

        if (p_Player->HasQuest(TanaanIntroQuests::QUEST_THE_BATTLE_OF_THE_FORGE))
            HandlePhases(p_Player, eAreaIndex::AREA_BLACKROCK_QUARRY_FIGHT);

        if (p_Player->IsQuestRewarded(TanaanIntroQuests::QUEST_THE_BATTLE_OF_THE_FORGE))
            BlackrockTerrainSwap(p_Player);

        if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_GUNPOWDER_PLOT) == QUEST_STATUS_INCOMPLETE &&
            p_Player->HasAura(TanaanIntroSpells::SPELL_LEAKY_GUNPOWDER_BARREL))
            PlayScene(p_Player, TanaanIntroScenes::SCENE_BUILDING_EXPLOSION_KEG_LEAKING, 16);

        if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_A_TASTE_OF_IRON) == QUEST_STATUS_INCOMPLETE &&
            p_Player->GetQuestObjectiveCounter(TanaanIntroQuestObjectives::OBJ_KILL_200_ORCS) != 200)
            PlayScene(p_Player, TanaanIntroScenes::SCENE_SHOOTING_IRON_HORDE, 20);
    }

    void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 p_OldZoneID, uint32 p_NewAreaID) override
    {
        if (p_Player->GetMapId() != TanaanIntroZones::MAP_TANAAN_INTRO)
            return;

        if (p_NewAreaID == TanaanIntroZones::AREA_BLACKROCK_QUARRY && p_NewZoneID == TanaanIntroZones::ZONE_TANAAN_JUNGLE_INTRO)
        {
            if (p_Player->IsQuestRewarded(TanaanIntroQuests::QUEST_THE_BATTLE_OF_THE_FORGE))
                BlackrockTerrainSwap(p_Player);
        }

        if (p_Player->GetQuestStatus(TanaanIntroQuests::QUEST_THE_COST_OF_WAR) == QUEST_STATUS_NONE && p_NewAreaID != TanaanIntroZones::AREA_THE_DARK_PORTAL)
        {
            p_Player->TeleportTo(TanaanIntroZones::MAP_TANAAN_INTRO, 4066.520f, -2380.036f, 94.806f, 1.511f);
            p_Player->AddDelayedEvent([p_Player]() -> void
            {
                if (Creature* l_Khadgar = p_Player->FindNearestCreature(TanaanIntroCreatures::NPC_ARCHMAGE_KHADGAR_1, 30.0f))
                    l_Khadgar->AI()->PersonalTalk(4, p_Player->GetGUID());
            }, 3000);
        }

        if (p_Player->isGameMaster())
            return;

        if (p_NewZoneID == p_OldZoneID)
            return;

        if (p_NewZoneID != TanaanIntroZones::ZONE_TANAAN_JUNGLE_INTRO)
            return;

        // When the player login in the same zone
        if (p_OldZoneID == 0)
            return;

        Position l_LastPos;
        for (auto const& l_Point : l_CheckpointList)
        {
            if (!p_Player->IsQuestRewarded(l_Point.QuestId))
                continue;

            l_LastPos = l_Point.Pos;
        }

        if (!l_LastPos)
            return;

        if (l_LastPos == l_CheckpointList[11].Pos || l_LastPos == l_CheckpointList[12].Pos)
            p_Player->TeleportTo(TanaanIntroZones::MAP_DRAENOR, l_LastPos);
        else
            p_Player->TeleportTo(TanaanIntroZones::MAP_TANAAN_INTRO, l_LastPos);
    }

    void OnItemLooted(Player* p_Player, Item* p_Item) override
    {
        if (!p_Player || !p_Item)
            return;

        // Item 112323 - Blackrock Powder Keg
        if (p_Item->GetEntry() != 112323)
            return;

        p_Player->CastSpell(p_Player, TanaanIntroSpells::SPELL_LEAKY_GUNPOWDER_BARREL, true);
    }
};

#ifndef __clang_analyzer__
void AddSC_tanaan_jungle()
{
    // creatures
    new npc_archmage_khadgar_78558();
    new npc_detonation_dwarves();
    new npc_spire_prisoners();
    new npc_gul_dan_78333();
    new npc_bled_dry_questgivers();
    new npc_ariok_78556();
    new npc_kilrogg_deadeye_81926();
    new npc_ariok_80087();
    new npc_blood_ritual_orb_83670();
    new npc_archmage_khadgar_78560();
    new npc_archmage_khadgar_78561();
    new npc_archmage_khadgar_80244();
    new npc_kargath_bladefist_79097();
    new npc_shattered_hand_brawler_82057();
    new npc_shadowmoon_ritualist_79590();
    new npc_shadowmoon_voidaxe_79589();
    new npc_taskmaster_gurran_79585();
    new npc_ungra_79583();
    new npc_yrel_78994();
    new npc_yrel_guardian_79795();
    new npc_kelidan_the_breaker_79702();
    new npc_olin_umberhide_guardian_79822();
    new npc_lady_liadrin_guardian_79823();
    new npc_cordana_felsong_guardian_79783();
    new npc_qiana_moonshadow_guardian_79775();
    new npc_ganar_prisoner_79917();
    new npc_overseer_gotrigg_80574();
    new npc_thaelin_darkanvil_78568();
    new npc_thaelin_darkanvil_guardian_80886();
    new npc_thaelin_darkanvil_80521();

    // gobjects
    new gob_mark_of_tanaan();
    new gob_stasis_rune_233104();
    new gob_bleeding_hollow_cages();
    new gob_makeshift_plunger_231066();
    new gob_siege_tank_turrets();
    new gob_main_cannon_trigger_232538();

    // spells
    new spell_taste_of_iron_game_aura_164042();
    new spell_khadgars_watch_absorb_dmg_167410();
    new spell_khadgars_watch_periodic_check_167421();

    // player script
    new playerScript_tanaan_intro();
}
#endif