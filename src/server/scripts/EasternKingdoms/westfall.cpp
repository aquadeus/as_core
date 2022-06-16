////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Westfall
SD%Complete: 90
SDComment: Quest support: 155, 1651
SDCategory: Westfall
EndScriptData */

/* ContentData
npc_daphne_stilwell
npc_defias_traitor
EndContentData */

#include "EasternKingdomsPCH.h"

/*######
## npc_daphne_stilwell
######*/

enum eEnums
{
    SAY_DS_START        = -1000293,
    SAY_DS_DOWN_1       = -1000294,
    SAY_DS_DOWN_2       = -1000295,
    SAY_DS_DOWN_3       = -1000296,
    SAY_DS_PROLOGUE     = -1000297,

    SPELL_SHOOT         = 6660,
    QUEST_TOME_VALOR    = 1651,
    NPC_DEFIAS_RAIDER   = 6180,
    EQUIP_ID_RIFLE      = 2511
};

class npc_daphne_stilwell : public CreatureScript
{
public:
    npc_daphne_stilwell() : CreatureScript("npc_daphne_stilwell") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_TOME_VALOR)
        {
            DoScriptText(SAY_DS_START, creature);

            if (npc_escortAI* pEscortAI = CAST_AI(npc_daphne_stilwell::npc_daphne_stilwellAI, creature->AI()))
                pEscortAI->Start(true, true, player->GetGUID());
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daphne_stilwellAI(creature);
    }

    struct npc_daphne_stilwellAI : public npc_escortAI
    {
        npc_daphne_stilwellAI(Creature* creature) : npc_escortAI(creature) {}

        uint32 uiWPHolder;
        uint32 uiShootTimer;

        void Reset()
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                switch (uiWPHolder)
                {
                    case 7:
                        DoScriptText(SAY_DS_DOWN_1, me);
                        break;
                    case 8:
                        DoScriptText(SAY_DS_DOWN_2, me);
                        break;
                    case 9:
                        DoScriptText(SAY_DS_DOWN_3, me);
                        break;
                }
            }
            else
                uiWPHolder = 0;

            uiShootTimer = 0;
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            uiWPHolder = waypointId;

            switch (waypointId)
            {
                case 4:
                    SetEquipmentSlots(false, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE, EQUIP_ID_RIFLE);
                    me->SetSheath(SHEATH_STATE_RANGED);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING_NO_SHEATHE);
                    break;
                case 7:
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11450.836f, 1569.755f, 54.267f, 4.230f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11449.697f, 1569.124f, 54.421f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.237f, 1568.307f, 54.620f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 8:
                    me->SetSheath(SHEATH_STATE_RANGED);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11450.836f, 1569.755f, 54.267f, 4.230f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11449.697f, 1569.124f, 54.421f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.237f, 1568.307f, 54.620f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.037f, 1570.213f, 54.961f, 4.283f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 9:
                    me->SetSheath(SHEATH_STATE_RANGED);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11450.836f, 1569.755f, 54.267f, 4.230f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11449.697f, 1569.124f, 54.421f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.237f, 1568.307f, 54.620f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.037f, 1570.213f, 54.961f, 4.283f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11449.018f, 1570.738f, 54.828f, 4.220f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 10:
                    SetRun(false);
                    break;
                case 11:
                    DoScriptText(SAY_DS_PROLOGUE, me);
                    break;
                case 13:
                    SetEquipmentSlots(true);
                    me->SetSheath(SHEATH_STATE_UNARMED);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING_NO_SHEATHE);
                    break;
                case 17:
                    player->GroupEventHappens(QUEST_TOME_VALOR, me);
                    break;
            }
        }

        void AttackStart(Unit* who, bool /*p_Melee*/ =  true)
        {
            if (!who)
                return;

            if (me->Attack(who, false))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);

                me->GetMotionMaster()->MoveChase(who, 30.0f);
            }
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }

        void Update(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (uiShootTimer <= diff)
            {
                uiShootTimer = 1500;

                if (!me->IsWithinDist(me->getVictim(), ATTACK_DISTANCE))
                    DoCast(me->getVictim(), SPELL_SHOOT);
            }
            else
                uiShootTimer -= diff;
        }
    };
};

/*######
## npc_defias_traitor
######*/

#define SAY_START                   -1000101
#define SAY_PROGRESS                -1000102
#define SAY_END                     -1000103
#define SAY_AGGRO_1                 -1000104
#define SAY_AGGRO_2                 -1000105

#define QUEST_DEFIAS_BROTHERHOOD    155

class npc_defias_traitor : public CreatureScript
{
public:
    npc_defias_traitor() : CreatureScript("npc_defias_traitor") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_DEFIAS_BROTHERHOOD)
        {
            if (npc_escortAI* pEscortAI = CAST_AI(npc_defias_traitor::npc_defias_traitorAI, creature->AI()))
                pEscortAI->Start(true, true, player->GetGUID());

            DoScriptText(SAY_START, creature, player);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defias_traitorAI(creature);
    }

    struct npc_defias_traitorAI : public npc_escortAI
    {
        npc_defias_traitorAI(Creature* creature) : npc_escortAI(creature) { Reset(); }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
                case 35:
                    SetRun(false);
                    break;
                case 36:
                    DoScriptText(SAY_PROGRESS, me, player);
                    break;
                case 44:
                    DoScriptText(SAY_END, me, player);
                    player->GroupEventHappens(QUEST_DEFIAS_BROTHERHOOD, me);
                    break;
            }
        }

        void EnterCombat(Unit* who)
        {
            DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2), me, who);
        }

        void Reset() {}
    };
};

enum clodoEnum
{
    NPC_HOMELESS_STORMWIND_CITIZEN      = 42384,
    NPC_HOMELESS_STORMWIND_CITIZEN2     = 42386,
    NPC_WEST_PLAINS_DRIFTER             = 42391,
    NPC_WEST_PLAINS_DRIFTER2            = 42400,
    NPC_ORPHAN                          = 42385,
    NPC_TRANSIENT                       = 42383,
    KILL_CREDIT_WESTFALL_STEW           = 42617
};

enum eQuests
{
    QuestMurderWasTheCaseThatTheyGaveMe = 26209,
    QuestMurderKillCreditClue1          = 42414,
    QuestMurderKillCreditClue2          = 42415,
    QuestMurderKillCreditClue3          = 42416,
    QuestMurderKillCreditClue4          = 42417,
    QuestMurderObjectIDClue1            = 265754,
    QuestMurderObjectIDClue2            = 265755,
    QuestMurderObjectIDClue3            = 265756,
    QuestMurderObjectIDClue4            = 265757
};

class npc_westfall_stew : public CreatureScript
{
    public:
        npc_westfall_stew() : CreatureScript("npc_westfall_stew") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_westfall_stewAI (creature);
        }

        struct npc_westfall_stewAI : public ScriptedAI
        {
            npc_westfall_stewAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 time;
            bool booltest;

            void Reset()
            {
                time = 6000;
                booltest = true;

                if (!me->ToTempSummon() || !me->ToTempSummon()->GetSummoner())
                    return;

                if (Player* invocer = me->ToTempSummon()->GetSummoner()->ToPlayer())
                {
                    std::list<Creature*> clodoList;
                    GetCreatureListWithEntryInGrid(clodoList, me, NPC_HOMELESS_STORMWIND_CITIZEN, 15.0f);
                    GetCreatureListWithEntryInGrid(clodoList, me, NPC_HOMELESS_STORMWIND_CITIZEN2, 15.0f);
                    GetCreatureListWithEntryInGrid(clodoList, me, NPC_WEST_PLAINS_DRIFTER, 15.0f);
                    GetCreatureListWithEntryInGrid(clodoList, me, NPC_WEST_PLAINS_DRIFTER2, 15.0f);
                    GetCreatureListWithEntryInGrid(clodoList, me, NPC_TRANSIENT, 15.0f);
                    GetCreatureListWithEntryInGrid(clodoList, me, NPC_ORPHAN, 15.0f);
                    for (Creature* clodo : clodoList)
                    {
                        if (clodo->getStandState() != UNIT_STAND_STATE_SIT)
                        {
                            clodo->GetMotionMaster()->MoveFollow(me, 1, me->GetAngle(clodo));
                            clodo->SetStandState(UNIT_STAND_STATE_SIT);
                            invocer->KilledMonsterCredit(KILL_CREDIT_WESTFALL_STEW, 0);
                        }
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (booltest)
                {
                    if (time < diff)
                    {
                        std::list<Creature*> clodoList;
                        GetCreatureListWithEntryInGrid(clodoList, me, NPC_HOMELESS_STORMWIND_CITIZEN, 15.0f);
                        GetCreatureListWithEntryInGrid(clodoList, me, NPC_HOMELESS_STORMWIND_CITIZEN2, 15.0f);
                        GetCreatureListWithEntryInGrid(clodoList, me, NPC_WEST_PLAINS_DRIFTER, 15.0f);
                        GetCreatureListWithEntryInGrid(clodoList, me, NPC_WEST_PLAINS_DRIFTER2, 15.0f);
                        GetCreatureListWithEntryInGrid(clodoList, me, NPC_TRANSIENT, 15.0f);
                        GetCreatureListWithEntryInGrid(clodoList, me, NPC_ORPHAN, 15.0f);
                        for (Creature* clodo : clodoList)
                        {
                            clodo->SetStandState(UNIT_STAND_STATE_STAND);
                            clodo->SetDefaultMovementType(RANDOM_MOTION_TYPE);
                        }

                        me->DespawnOrUnsummon();
                        booltest = false;
                    }
                    else
                        time -= diff;
                }
            }
        };
};

class npc_westfall_homeless : public CreatureScript
{
    public:
        npc_westfall_homeless() : CreatureScript("npc_westfall_homeless") { }

        bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*sender*/, uint32 p_Action) override
        {
            p_Player->PlayerTalkClass->ClearMenus();
            p_Player->PlayerTalkClass->SendCloseGossip();
            p_Creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            p_Creature->DespawnOrUnsummon(30 * IN_MILLISECONDS);

            switch (p_Action)
            {
            case GOSSIP_ACTION_INFO_DEF + 1:
            {
                if (urand(0, 1))
                {
                    if (p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue1) == 0)
                    {
                        p_Player->KilledMonsterCredit(QuestMurderKillCreditClue1);
                        p_Creature->AI()->Talk(1, p_Player->GetGUID());
                        return true;
                    }
                    else if (p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue1) == 1 && p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue2) == 0)
                    {
                        p_Player->KilledMonsterCredit(QuestMurderKillCreditClue2);
                        p_Creature->AI()->Talk(2, p_Player->GetGUID());
                        return true;
                    }
                    else if (p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue2) == 1 && p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue3) == 0)
                    {
                        p_Player->KilledMonsterCredit(QuestMurderKillCreditClue3);
                        p_Creature->AI()->Talk(3, p_Player->GetGUID());
                        return true;
                    }
                    else if (p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue3) == 1 && p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue4) == 0)
                    {
                        p_Player->KilledMonsterCredit(QuestMurderKillCreditClue4);
                        p_Creature->AI()->Talk(4, p_Player->GetGUID());
                        return true;
                    }
                }
                else
                {
                    p_Creature->setFaction(14);
                    p_Creature->AI()->AttackStart(p_Player);
                    p_Creature->AI()->Talk(0);
                }
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 2:
            {
                if (p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue1) == 0)
                {
                    p_Player->KilledMonsterCredit(QuestMurderKillCreditClue1);
                    p_Creature->AI()->Talk(1, p_Player->GetGUID());
                    return true;
                }
                else if (p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue1) == 1 && p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue2) == 0)
                {
                    p_Player->KilledMonsterCredit(QuestMurderKillCreditClue2);
                    p_Creature->AI()->Talk(2, p_Player->GetGUID());
                    return true;
                }
                else if (p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue2) == 1 && p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue3) == 0)
                {
                    p_Player->KilledMonsterCredit(QuestMurderKillCreditClue3);
                    p_Creature->AI()->Talk(3, p_Player->GetGUID());
                    return true;
                }
                else if (p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue3) == 1 && p_Player->GetQuestObjectiveCounter(eQuests::QuestMurderObjectIDClue4) == 0)
                {
                    p_Player->KilledMonsterCredit(QuestMurderKillCreditClue2);
                    p_Creature->AI()->Talk(4, p_Player->GetGUID());
                    return true;
                }
            }
            }
            return true;
        }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            if (p_Player->GetQuestStatus(QuestMurderWasTheCaseThatTheyGaveMe) == QUEST_STATUS_INCOMPLETE)
            {
                p_Player->ADD_GOSSIP_ITEM_DB(11635, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                p_Player->ADD_GOSSIP_ITEM_DB(11635, 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            }
            p_Player->SEND_GOSSIP_MENU(16250, p_Creature->GetGUID());
            return true;
        }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_westfall_homelessAI(creature);
        }

        struct npc_westfall_homelessAI : public ScriptedAI
        {
            npc_westfall_homelessAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
                me->setFaction(7);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        };
};

#ifndef __clang_analyzer__
void AddSC_westfall()
{
    new npc_daphne_stilwell();
    new npc_defias_traitor();
    new npc_westfall_stew();
    new npc_westfall_homeless();
}
#endif
