////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "violet_hold_legion.hpp"

#define GOSSIP_I_WANT_IN    "I'm not fighting, so send me in now!"
#define SPAWN_TIME          20000

enum Sinclari
{
    SAY_SINCLARI_1              = 0,
    SAY_SINCLARI_2              = 1,
    SAY_KEEPER                  = 2,
    SAY_ELITE                   = 3,
    SAY_GUARDIAN                = 4,
};

Position const exitPos = {4556.62f, 4015.16f, 83.67f};
Position const plrTeleportPos = {4577.63f, 4015.38f, 83.60f, 6.27f};
Position const movDoorPos = {4585.82f, 4015.32f, 83.47f, 3.14f};

//102278
class npc_sinclari_vh_leg : public CreatureScript
{
public:
    npc_sinclari_vh_leg() : CreatureScript("npc_sinclari_vh_leg") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (InstanceScript* instance = creature->GetInstanceScript())
        {
            switch (instance->GetData(DATA_MAIN_EVENT_PHASE))
            {
                case NOT_STARTED:
                case FAIL: // Allow to start event if not started or wiped
                    player->ADD_GOSSIP_ITEM_DB(19229, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                    player->SEND_GOSSIP_MENU(28225, creature->GetGUID());
                    break;
                case IN_PROGRESS: // Allow to teleport inside if event is in progress
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_I_WANT_IN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                    player->SEND_GOSSIP_MENU(28225, creature->GetGUID());
                    break;
                default:
                    player->SEND_GOSSIP_MENU(13910, creature->GetGUID());
                    break;
            }
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM_DB(19230, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(28226, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->PlayerTalkClass->SendCloseGossip();
                CAST_AI(npc_sinclari_vh_leg::npc_sinclariAI, (creature->AI()))->uiPhase = 1;
                if (InstanceScript* instance = creature->GetInstanceScript())
                    instance->SetData(DATA_MAIN_EVENT_PHASE, SPECIAL);
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->NearTeleportTo(plrTeleportPos.GetPositionX(), plrTeleportPos.GetPositionY(), plrTeleportPos.GetPositionZ(), plrTeleportPos.GetOrientation(), true);
                player->PlayerTalkClass->SendCloseGossip();
                break;
        }
        return true;
    }

    struct npc_sinclariAI : public ScriptedAI
    {
        npc_sinclariAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
           instance = creature->GetInstanceScript();
           me->SetReactState(REACT_PASSIVE);
        }

        InstanceScript* instance;
        SummonList summons;

        uint8  uiPhase;
        uint32 uiTimer;

        void Reset() override
        {
            uiPhase = 0;
            uiTimer = 0;
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            std::list<Creature*> GuardList;
            me->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
            if (!GuardList.empty())
            {
                for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                {
                    if (Creature* pGuard = *itr)
                    {
                        pGuard->DisappearAndDie();
                        pGuard->Respawn();
                        pGuard->SetVisible(true);
                    }
                }
            }
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
        }

        void DoAction(int32 const action) override
        {
            summons.DespawnAll();
        }

        void UpdateAI(uint32 const uiDiff) override
        {
            ScriptedAI::UpdateAI(uiDiff);

            if (uiPhase)
            {
                if (uiTimer <= uiDiff)
                {
                    switch (uiPhase)
                    {
                        case 1:
                            Talk(SAY_SINCLARI_1);
                            uiTimer = 4000;
                            uiPhase = 2;
                            break;
                        case 2:
                        {
                            std::list<Creature*> GuardList;
                            me->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
                            if (!GuardList.empty())
                                for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                                {
                                    if (Creature* pGuard = *itr)
                                    {
                                        pGuard->SetReactState(REACT_PASSIVE);
                                        pGuard->SetWalk(false);
                                        pGuard->GetMotionMaster()->MovePoint(0, exitPos);
                                    }
                                }
                            uiTimer = 6000;
                            uiPhase = 3;
                            break;
                        }
                        case 3:
                        {
                            std::list<Creature*> GuardList;
                            me->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
                            if (!GuardList.empty())
                                for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                                {
                                    if (Creature* pGuard = *itr)
                                    {
                                        pGuard->SetVisible(false);
                                        pGuard->SetReactState(REACT_PASSIVE);
                                    }
                                }
                            uiTimer = 2000;
                            uiPhase = 4;
                            break;
                        }
                        case 4:
                            Talk(SAY_SINCLARI_2);
                            me->GetMotionMaster()->MovePoint(0, exitPos);
                            uiTimer = 4000;
                            uiPhase = 5;
                            break;
                        case 5:
                            if (instance)
                                instance->SetData(DATA_MAIN_EVENT_PHASE, IN_PROGRESS);
                            uiTimer = 0;
                            uiPhase = 0;
                            break;
                    }
                }
                else uiTimer -= uiDiff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_sinclariAI(creature);
    }
};

//102267
class npc_teleportation_portal_vh_leg : public CreatureScript
{
public:
    npc_teleportation_portal_vh_leg() : CreatureScript("npc_teleportation_portal_vh_leg") { }

    struct npc_teleportation_portalAI : public ScriptedAI
    {
        npc_teleportation_portalAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
            instance = creature->GetInstanceScript();
            bPortalGuardianOrKeeperOrEliteSpawn = false;
        }

        uint32 uiSpawnTimer;
        bool bPortalGuardianOrKeeperOrEliteSpawn;

        SummonList summons;

        InstanceScript* instance;

        void Reset() override
        {
            uiSpawnTimer = 10000;
            bPortalGuardianOrKeeperOrEliteSpawn = false;
            DoCast(me, SPELL_PORTAL_PERIODIC);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!instance) //Massive usage of instance, global check
                return;
            
            if (instance->GetData(DATA_REMOVE_NPC) == 1)
            {
                me->DespawnOrUnsummon();
                instance->SetData(DATA_REMOVE_NPC, 0);
            }

            uint8 uiWaveCount = instance->GetData(DATA_WAVE_COUNT);
            if ((uiWaveCount == 6) || (uiWaveCount == 12)) //Don't spawn mobs on boss encounters
                return;

            /// Guardian or Keeper
            if (instance->GetData(DATA_PORTAL_LOCATION) < 4)
            {
                if (uiSpawnTimer <= diff)
                {
                    if (bPortalGuardianOrKeeperOrEliteSpawn)
                    {
                        uint8 k = instance->GetData(DATA_WAVE_COUNT) < 12 ? 3 : 4;
                        for (uint8 i = 0; i < k; ++i)
                        {
                            uint32 entry = RAND(NPC_EREDAR_INVADER_1, NPC_EREDAR_INVADER_2, NPC_WRATHLORD_BULWARK, NPC_FELSTALKER_RAVENER_1, NPC_FELSTALKER_RAVENER_2, NPC_INFILTRATOR_ASSASSIN);
                            DoSummon(entry, me, 2.0f, 20000, TEMPSUMMON_CORPSE_TIMED_DESPAWN);
                        }
                    }
                    else
                    {
                        bPortalGuardianOrKeeperOrEliteSpawn = true;
                        uint32 entry = RAND(NPC_PORTAL_GUARDIAN_1, NPC_PORTAL_GUARDIAN_2, NPC_PORTAL_KEEPER_1, NPC_PORTAL_KEEPER_2);
                        if (Creature* pPortalKeeper = DoSummon(entry, me, 2.0f, 20000, TEMPSUMMON_CORPSE_TIMED_DESPAWN))
                        {
                            me->CastSpell(pPortalKeeper, SPELL_PORTAL_CHANNEL, true);
                            pPortalKeeper->AI()->Talk(0);
                            if (Creature* announcer = me->FindNearestCreature(102278, 500.0f))
                            {
                                if (entry == NPC_PORTAL_GUARDIAN_1 || entry == NPC_PORTAL_GUARDIAN_2)
                                    announcer->AI()->ZoneTalk(SAY_GUARDIAN);
                                else
                                    announcer->AI()->ZoneTalk(SAY_KEEPER);
                            }
                        }
                    }

                    uiSpawnTimer = SPAWN_TIME;
                }
                else
                    uiSpawnTimer -= diff;

                if (bPortalGuardianOrKeeperOrEliteSpawn && !me->IsNonMeleeSpellCasted(false))
                {
                    me->Kill(me, false);
                    me->RemoveCorpse();
                }
            }
            /// Elite Squad
            else
            {
                if (!bPortalGuardianOrKeeperOrEliteSpawn)
                {
                    if (uiSpawnTimer <= diff)
                    {
                        if (Creature* announcer = me->FindNearestCreature(102278, 500.0f))
                            announcer->AI()->ZoneTalk(SAY_ELITE);

                        bPortalGuardianOrKeeperOrEliteSpawn = true;

                        /// The elite Legion squads consist of three to four enemies who emerge from the portal and begin to run toward the prison seal.
                        /// The composition of these elite Legion squads varies based on how far players have progressed through the dungeon.
                        bool l_SpiderType   = uiWaveCount >= 6 && instance->GetData(DATA_THIRD_BOSS) == DATA_SAELORN;
                        uint8 l_Specials    = uiWaveCount < 12 ? 1 : 2;
                        uint8 l_Counter     = uiWaveCount < 12 ? 3 : 4;

                        for (uint8 l_I = 0; l_I < l_Counter; ++l_I)
                        {
                            uint32 l_Entry = 0;

                            /// Until players have defeated the first boss, the elite Legion squads will contain mobs drawn from the following list:
                            if (uiWaveCount < 6 || !l_Specials)
                                l_Entry = RAND(NPC_BLAZING_INFERNAL, NPC_EREDAR_SHADOW_MENDER, NPC_INFILTRATOR_ASSASSIN, NPC_WRATHLORD_BULWARK);
                            /// After players have defeated the first boss, subsequent elite Legion squads will contain new enemy types.
                            /// These enemies are determined by the identity of the Invasion Commander that players will fight at the end of the instance.
                            /// If the third boss will be Sael'orn, these trash mobs are spider-themed...
                            else if (l_SpiderType && l_Specials > 0)
                            {
                                /// Sael'orn's spider-themed trash deals largely poison damage and threatens the party with DoT effects.
                                /// The elite Legion squads that spawn between the First and Second Escaped Prisoner fights will contain one of the two following mobs:
                                if (uiWaveCount < 12)
                                    l_Entry = RAND(NPC_ACOLYTE_OF_SAELORN, NPC_VENOMHIDE_SHADOWSPINNER);
                                else
                                    l_Entry = RAND(NPC_ACOLYTE_OF_SAELORN, NPC_VENOMHIDE_SHADOWSPINNER, NPC_BROOD_OF_SAELORN);

                                --l_Specials;
                            }
                            ///... and if the third boss will be Fel Lord Betrug, these trash mobs are demon-themed.
                            else if (l_Specials > 0)
                            {
                                l_Entry = RAND(NPC_WRATHGUARD_DECIMATOR, NPC_SHADOWY_OVERFIEND);

                                --l_Specials;
                            }

                            DoSummon(l_Entry, me, 2.0f, 20000, TEMPSUMMON_CORPSE_TIMED_DESPAWN);
                        }

                        me->SetVisible(false);
                    }
                    else
                        uiSpawnTimer -= diff;
                }
                else
                {
                    /// If all spawned elites have died kill portal
                    if (summons.empty())
                    {
                        me->Kill(me, false);
                        me->RemoveCorpse();
                    }
                }
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (instance)
                instance->SetData(DATA_WAVE_COUNT, instance->GetData(DATA_WAVE_COUNT)+1);
        }

        void JustSummoned(Creature* summoned) override
        {
            summons.Summon(summoned);
            if (summoned)
                instance->SetData64(DATA_ADD_TRASH_MOB, summoned->GetGUID());
        }

        void SummonedCreatureDies(Creature* summoned, Unit* /*killer*/) override
        {
            summons.Despawn(summoned);
            if (summoned)
                instance->SetData64(DATA_DEL_TRASH_MOB, summoned->GetGUID());
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_teleportation_portalAI(creature);
    }
};

//102282
class npc_lord_malgath : public CreatureScript
{
public:
    npc_lord_malgath() : CreatureScript("npc_lord_malgath") { }

    struct npc_lord_malgathAI : public ScriptedAI
    {
        npc_lord_malgathAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            uiBoss = 0;
            eFight = false;
        }

        InstanceScript* instance;
        EventMap events;

        bool eFight;
        uint8 waveCount;
        uint8 uiBoss;

        void Reset() override {}

        void EnterCombat(Unit* who) override
        {
            events.ScheduleEvent(EVENT_6, urand(12000, 17000)); //205046
            events.ScheduleEvent(EVENT_7, 30000); //204962
            events.ScheduleEvent(EVENT_8, 18000); //204963 
            events.ScheduleEvent(EVENT_9, 19000); //204966
        }
        
        void IsSummonedBy(Unit* summoner) override
        {
            if (instance && !uiBoss)
            {
                waveCount = instance->GetData(DATA_WAVE_COUNT);
                uiBoss = waveCount == 6 ? instance->GetData(DATA_FIRST_BOSS) : instance->GetData(DATA_SECOND_BOSS);
            }

            if (waveCount == 6 || waveCount == 12)
                events.ScheduleEvent(EVENT_1, 2000);
            else
                events.ScheduleEvent(EVENT_4, 2000);
        }
        
        void JustDied(Unit* /*killer*/) override
        {
            if (!instance)
                return;

            if (Creature* l_Boss = instance->instance->GetCreature(instance->GetData64(instance->GetData(DATA_THIRD_BOSS))))
                l_Boss->AI()->DoAction(true);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (waveCount == 6 || waveCount == 12)
                events.ScheduleEvent(EVENT_2, 1000);
            else
                events.ScheduleEvent(EVENT_5, 1000);
        }
        
        void JustReachedHome() override
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            me->RemoveAurasDueToSpell(SPELL_FEL_SHIELD);
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!UpdateVictim() && eFight)
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        me->GetMotionMaster()->MovePoint(uiBoss, saboMovePos[uiBoss].GetPositionX(), 
                                                                 saboMovePos[uiBoss].GetPositionY(), 
                                                                 saboMovePos[uiBoss].GetPositionZ(), false);
                        break;
                    case EVENT_2:
                        DoCast(me, SPELL_SHIELD_DESTRUCTION, true);
                        events.ScheduleEvent(EVENT_3, 7000);
                        break;
                    case EVENT_3:
                        if (instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                            instance->SetData(DATA_START_BOSS_ENCOUNTER, 1);
                        me->DespawnOrUnsummon(1000);
                        break;
                    case EVENT_4:
                        me->GetMotionMaster()->MovePoint(1, saboFightPos);
                        break;
                    case EVENT_5:
                        eFight = true;
                        me->SetOrientation(3.13f);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveAurasDueToSpell(SPELL_FEL_SHIELD);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoZoneInCombat(me, 100.0f);
                        me->SetHomePosition(*me);
                        SetFlyMode(false);
                        break;
                    case EVENT_6:
                        DoCast(205046);
                        events.ScheduleEvent(EVENT_6, urand(12000, 17000)); //205046
                        break;
                    case EVENT_7:
                        DoCast(204962);
                        events.ScheduleEvent(EVENT_7, 30000); //204962
                        break;
                    case EVENT_8:
                        DoCast(204963);
                        events.ScheduleEvent(EVENT_8, 18000); //204963 
                        break;
                    case EVENT_9:
                        DoCast(204966);
                        events.ScheduleEvent(EVENT_9, 19000); //204966
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lord_malgathAI (creature);
    }
};

//102272,102368,102380,102395,102400,102397,102269,102369,102270,102370, 102336, 102302, 102337, 102335, 102398, 112738, 112733, 112732, 112741, 112739
class npc_violet_hold_trash : public CreatureScript
{
public:
    npc_violet_hold_trash() : CreatureScript("npc_violet_hold_trash") {}

    struct npc_violet_hold_trashAI : public ScriptedAI
    {
        npc_violet_hold_trashAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_DEFENSIVE);
            attackDoorTimer = 0;

            m_MovementWasStopped = true;
            m_MovePos = Position();
        }

        InstanceScript* instance;
        EventMap events;

        uint32 attackDoorTimer;

        Position m_MovePos;
        bool m_MovementWasStopped;

        void Reset() override
        {
            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_STUNNED);
            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_UNK_29);
            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_FEIGN_DEATH);
        }

        void EnterCombat(Unit* who) override
        {
            if (who->GetTypeId() != TYPEID_PLAYER)
                return;

            attackDoorTimer = 0;
            me->SetReactState(REACT_AGGRESSIVE);
            if (me->GetEntry() == NPC_PORTAL_GUARDIAN_2)
            {
                events.ScheduleEvent(EVENT_1, 4000); // 204028
                events.ScheduleEvent(EVENT_2, 25000); // 204032
                events.ScheduleEvent(EVENT_3, 20000);// while under 204140 and get damage - cast 204208
            }
            if (me->GetEntry() == NPC_PORTAL_GUARDIAN_1)
            {
                Talk(1);
                events.ScheduleEvent(EVENT_4, 16000); // 204517
                events.ScheduleEvent(EVENT_5, 18000); // 204493   
            }
            if (me->GetEntry() == NPC_PORTAL_KEEPER_1)
            {
                events.ScheduleEvent(EVENT_6, 31000); // 204722  33
                events.ScheduleEvent(EVENT_7, 33000); // 204876
                events.ScheduleEvent(EVENT_8, 23000); // 204895
            }
            if (me->GetEntry() == NPC_PORTAL_KEEPER_2)
            {
                events.ScheduleEvent(EVENT_9, 9000); // 204951
                events.ScheduleEvent(EVENT_10, 27000); // 204901
                events.ScheduleEvent(EVENT_11, 14000); // 204948
            }
            if (me->GetEntry() == NPC_FELGUARD_DESTROYER_2 || me->GetEntry() == NPC_SHADOW_COUNCIL_WARLOCK || me->GetEntry() == NPC_FELGUARD_DESTROYER_1 || me->GetEntry() == NPC_EREDAR_INVADER_2 || me->GetEntry() == NPC_EREDAR_INVADER_1)
                events.ScheduleEvent(EVENT_12, 15000); // 205115 205123
            if (me->GetEntry() == NPC_INFILTRATOR_ASSASSIN)
                events.ScheduleEvent(EVENT_13, 27000); // 205097 and cast 205093
            if (me->GetEntry() == NPC_EREDAR_SHADOW_MENDER)
            {
                DoCast(205099);
                events.ScheduleEvent(EVENT_14, 20000); //205103
                events.ScheduleEvent(EVENT_15, 13000); //205102
            }
            if (me->GetEntry() == NPC_WRATHLORD_BULWARK)
            {
                DoCast(182405);
                events.ScheduleEvent(EVENT_16, 20000); // 205082
                events.ScheduleEvent(EVENT_17, 21000); // 205080
            }
            if (me->GetEntry() == NPC_FELSTALKER_RAVENER_2 || me->GetEntry() == NPC_FELSTALKER_RAVENER_1)
                events.ScheduleEvent(EVENT_18, 22000); // 205108
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (me->GetEntry() == NPC_PORTAL_GUARDIAN_2)
                Talk(urand(1, 5));
            if (me->GetEntry() == NPC_PORTAL_GUARDIAN_1)
                Talk(urand(2, 8));
            if (me->GetEntry() == NPC_PORTAL_KEEPER_1)
                Talk(2);
            if (me->GetEntry() == NPC_PORTAL_KEEPER_2)
                Talk(urand(1,2));
          
            me->RemoveAllAreasTrigger();
        }
        
        void IsSummonedBy(Unit* summoner) override
        {
           if (me->GetEntry() != NPC_PORTAL_KEEPER_2 && me->GetEntry() != NPC_PORTAL_KEEPER_1 && me->GetEntry() != NPC_PORTAL_GUARDIAN_2 && me->GetEntry() != NPC_PORTAL_GUARDIAN_1)
           {
               Position l_Pos = { movDoorPos.GetPositionX() + irand(-5,5), movDoorPos.GetPositionY() + irand(-5,5), movDoorPos.GetPositionZ() };

               m_MovePos = l_Pos;

               me->GetMotionMaster()->MovePoint(1, l_Pos);
               me->SetHomePosition(movDoorPos);
           }
        }

        void JustReachedHome() override
        {
           if (me->GetEntry() != NPC_PORTAL_KEEPER_2 && me->GetEntry() != NPC_PORTAL_KEEPER_1 && me->GetEntry() != NPC_PORTAL_GUARDIAN_2 && me->GetEntry() != NPC_PORTAL_GUARDIAN_1)
               CreatureStartAttackDoor();
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                if (!me->IsNearPosition(&m_MovePos, 1.0f))
                {
                    events.ScheduleEvent(EVENT_19, 200);
                    m_MovementWasStopped = true;
                    return;
                }

                CreatureStartAttackDoor();
            }
        }

        void CreatureStartAttackDoor()
        {
            DoCast(me, SPELL_DESTROY_DOOR_SEAL, true);
            attackDoorTimer = 2000;
        }

        void UpdateAI(uint32 const diff) override
        {
            if (attackDoorTimer)
            {
                if (attackDoorTimer <= diff)
                {
                    if (instance)
                        if (instance->GetData(DATA_DOOR_INTEGRITY) >= 1)
                            instance->SetData(DATA_DOOR_INTEGRITY, instance->GetData(DATA_DOOR_INTEGRITY) -1);
                    attackDoorTimer = 2000;
                }
                else
                    attackDoorTimer -= diff;
            }
            
            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING) || !UpdateVictim())
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                    {
                        DoCast(204028);
                        events.ScheduleEvent(EVENT_1, 4000); //204028
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(204032);
                        events.ScheduleEvent(EVENT_2, 25000); // 204032
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(204140);
                        events.ScheduleEvent(EVENT_3, 20000);// while under 204140 and get damage - cast 204208
                        break;
                    }
                   case EVENT_4:
                   {
                       DoCast(204517);
                       events.ScheduleEvent(EVENT_4, 16000); // 204517
                       break;
                   }
                    case EVENT_5:
                    {
                        DoCast(204493);
                        events.ScheduleEvent(EVENT_5, 18000); // 204493         
                        break;
                    }
                    case EVENT_6:
                    {
                        DoCast(204722);
                        events.ScheduleEvent(EVENT_6, 33000); // 204722  33
                        break;
                    }
                    case EVENT_7:
                    {
                        DoCast(204876);
                        events.ScheduleEvent(EVENT_7, 33000); // 204876
                        break;
                    }
                    case EVENT_8:
                    {
                        DoCast(204895);
                        events.ScheduleEvent(EVENT_8, 23000); // 204895    
                        break;
                    }
                    case EVENT_9:
                    {
                        DoCast(204951);
                        events.ScheduleEvent(EVENT_9, 9000); // 204951
                        break;
                    }
                    case EVENT_10:
                    {
                        DoCast(204901);
                        events.ScheduleEvent(EVENT_10, 27000); // 204901
                        break;
                    }
                    case EVENT_11:
                    {
                        DoCast(204948);
                        events.ScheduleEvent(EVENT_11, 14000); // 204948 
                        break;
                    }
                    case EVENT_12:
                    {
                        if (me->GetEntry() == 102368 || me->GetEntry() == 102272)
                            DoCast(205115);
                        if (me->GetEntry() == 102380)
                            DoCast(205123);
                        if (me->GetEntry() == 102370 || me->GetEntry() == 102270)
                            DoCast(205513);
                        events.ScheduleEvent(EVENT_12, 15000); // 205115 205123
                        break;
                    }
                    case EVENT_13:
                    {
                        DoCast(205097);
                        DoCast(205093);
                        events.ScheduleEvent(EVENT_13, 27000); // 205097 and cast 205093
                        break;
                    }
                    case EVENT_14:
                    {
                        DoCast(205103);
                        events.ScheduleEvent(EVENT_14, 20000); //205103
                        break;
                    }
                    case EVENT_15:
                    {
                        DoCast(205102);
                        events.ScheduleEvent(EVENT_15, 13000); //205102    
                        break;
                    }
                    case EVENT_16:
                    {
                        DoCast(205082);
                        events.ScheduleEvent(EVENT_16, 20000); // 205082
                        break;
                    }
                    case EVENT_17:
                    {
                        DoCast(205080);
                        events.ScheduleEvent(EVENT_17, 21000); // 205080        
                        break;
                    }
                    case EVENT_18:
                    {
                        DoCast(205108);
                        events.ScheduleEvent(EVENT_18, 22000); // 205108
                        break;
                    }
                    case EVENT_19:
                    {
                        if (!me->HasBreakableByDamageCrowdControlAura() && m_MovementWasStopped)
                        {
                            m_MovementWasStopped = false;
                            me->GetMotionMaster()->MovePoint(1, m_MovePos);
                            break;
                        }

                        events.ScheduleEvent(EVENT_19, 200);
                        break;
                    }
                    default:
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_violet_hold_trashAI(creature);
    }
};

//102671
class npc_vh_prison_cell_mover : public CreatureScript
{
public:
    npc_vh_prison_cell_mover() : CreatureScript("npc_vh_prison_cell_mover") {}

    struct npc_vh_prison_cell_moverAI : public ScriptedAI
    {
        npc_vh_prison_cell_moverAI(Creature* creature) : ScriptedAI(creature) 
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override
        {
            for (int8 i = 0; i < 6; i++)
                if (me->GetDistance(bossStartMove[i]) < 14.0f)
                {
                    me->GetMotionMaster()->MoveIdle();
                    me->LoadPath(me->GetEntry() * 100 + i);
                    me->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
                    me->GetMotionMaster()->Initialize();
                }
        }

        void UpdateAI(uint32 const diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_vh_prison_cell_moverAI(creature);
    }
};

// 103312
class npc_soul_vortex : public CreatureScript
{
public:
    npc_soul_vortex() : CreatureScript("npc_soul_vortex") {}

    struct npc_soul_vortexAI : public ScriptedAI
    {
        npc_soul_vortexAI(Creature* creature) : ScriptedAI(creature) 
        {
           DoCast(204465);
           SetCombatMovement(false);
        }
        
          void UpdateAI(uint32 const diff) override
          {
            if (Unit* target = me->FindNearestPlayer(100.0f))
            {
               if (me->IsWithinMeleeRange(target))
               {
                  DoCast(204498);
                  me->RemoveAreaTrigger(204465);
                  me->DespawnOrUnsummon(1);
               }
            }
          }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_soul_vortexAI(creature);
    }
};

class spell_shadow_bomb : public SpellScriptLoader
{
    public:
        spell_shadow_bomb() : SpellScriptLoader("spell_shadow_bomb") { }

        class spell_shadow_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shadow_bomb_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* target = GetTarget();
                if (!target)
                    return;

                if (Creature* malgath = target->FindNearestCreature(102282, 500, true))
                    malgath->CastSpell(target, 204961, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_shadow_bomb_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shadow_bomb_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_violet_hold_legion()
{
    new npc_sinclari_vh_leg();
    new npc_teleportation_portal_vh_leg();
    new npc_lord_malgath();
    new npc_violet_hold_trash();
    new npc_vh_prison_cell_mover();
    new npc_soul_vortex();
    
    new spell_shadow_bomb();
}
#endif
