////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* ScriptData
SDName: Eversong_Woods
SD%Complete: 100
SDComment: Quest support: 8483, 8488, 8490, 9686
SDCategory: Eversong Woods
EndScriptData */

/* ContentData
npc_prospector_anvilward
npc_apprentice_mirveda
npc_infused_crystal
npc_kelerun_bloodmourn
go_harbinger_second_trial
EndContentData */

#include "EasternKingdomsPCH.h"

/*######
## npc_apprentice_mirveda
######*/

#define QUEST_UNEXPECTED_RESULT 8488
#define MOB_GHARZUL     15958
#define MOB_ANGERSHADE  15656
#define TALK_CREDIT  15402

class npc_apprentice_mirveda : public CreatureScript
{
public:
    npc_apprentice_mirveda() : CreatureScript("npc_apprentice_mirveda") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_UNEXPECTED_RESULT)
        {
            creature->AI()->DoAction(1);
            player->KilledMonsterCredit(TALK_CREDIT);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_apprentice_mirvedaAI(creature);
    }

    struct npc_apprentice_mirvedaAI : public ScriptedAI
    {
        npc_apprentice_mirvedaAI(Creature* creature) : ScriptedAI(creature), Summons(me) {}

        SummonList Summons;

        void Reset() override
        {
            Summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/) override {}

        void JustSummoned(Creature* summoned) override
        {
            Summons.Summon(summoned);
            summoned->AI()->AttackStart(me);
        }

        void JustDied(Unit* /*killer*/) override
        {
            std::list<Player*> P_List;
            me->GetPlayerListInGrid(P_List, 30.0f);
            if (!P_List.empty())
                for (std::list<Player*>::const_iterator itr = P_List.begin(); itr != P_List.end(); ++itr)
                    (*itr)->FailQuest(QUEST_UNEXPECTED_RESULT);
        }

        void DoAction(int32 const /*p_Action*/) override
        {
            me->SummonCreature(MOB_GHARZUL, 8745, -7134.32f, 35.22f, 0, TEMPSUMMON_CORPSE_DESPAWN, 120000);
            me->SummonCreature(MOB_ANGERSHADE, 8745.41f, -7128.39f, 35.70f, 0, TEMPSUMMON_CORPSE_DESPAWN, 120000);
            me->SummonCreature(MOB_ANGERSHADE, 8751.18f, -7130.57f, 35.26f, 0, TEMPSUMMON_CORPSE_DESPAWN, 120000);
            me->SummonCreature(MOB_ANGERSHADE, 8750.06f, -7136.17f, 35.22f, 0, TEMPSUMMON_CORPSE_DESPAWN, 120000);
        }
    };
};

/*######
## npc_infused_crystal
######*/

#define MOB_ENRAGED_WRAITH  17086
#define EMOTE   -1000283
#define QUEST_POWERING_OUR_DEFENSES 8490

struct Location
{
    float x, y, z;
};

static Location SpawnLocations[] =
{
    {8270.68f, -7188.53f, 139.619f},
    {8284.27f, -7187.78f, 139.603f},
    {8297.43f, -7193.53f, 139.603f},
    {8303.5f, -7201.96f, 139.577f},
    {8273.22f, -7241.82f, 139.382f},
    {8254.89f, -7222.12f, 139.603f},
    {8278.51f, -7242.13f, 139.162f},
    {8267.97f, -7239.17f, 139.517f}
};

class npc_infused_crystal : public CreatureScript
{
public:
    npc_infused_crystal() : CreatureScript("npc_infused_crystal") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infused_crystalAI (creature);
    }

    struct npc_infused_crystalAI : public Scripted_NoMovementAI
    {
        npc_infused_crystalAI(Creature* creature) : Scripted_NoMovementAI(creature) {}

        uint32 EndTimer;
        uint32 WaveTimer;
        bool Completed;
        bool Progress;
        uint64 PlayerGUID;

        void Reset()
        {
            EndTimer = 0;
            Completed = false;
            Progress = false;
            PlayerGUID = 0;
            WaveTimer = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!Progress && who->IsPlayer() && me->IsWithinDistInMap(who, 10.0f))
            {
                if (CAST_PLR(who)->GetQuestStatus(QUEST_POWERING_OUR_DEFENSES) == QUEST_STATUS_INCOMPLETE)
                {
                    PlayerGUID = who->GetGUID();
                    WaveTimer = 1000;
                    EndTimer = 60000;
                    Progress = true;
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (PlayerGUID && !Completed)
                if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                    CAST_PLR(player)->FailQuest(QUEST_POWERING_OUR_DEFENSES);
        }

        void UpdateAI(const uint32 diff)
        {
            if (EndTimer < diff && Progress)
            {
                DoScriptText(EMOTE, me);
                Completed = true;
                if (PlayerGUID)
                    if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                        CAST_PLR(player)->CompleteQuest(QUEST_POWERING_OUR_DEFENSES);

                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
            } else EndTimer -= diff;

            if (WaveTimer < diff && !Completed && Progress)
            {
                uint32 ran1 = rand()%8;
                uint32 ran2 = rand()%8;
                uint32 ran3 = rand()%8;
                me->SummonCreature(MOB_ENRAGED_WRAITH, SpawnLocations[ran1].x, SpawnLocations[ran1].y, SpawnLocations[ran1].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                me->SummonCreature(MOB_ENRAGED_WRAITH, SpawnLocations[ran2].x, SpawnLocations[ran2].y, SpawnLocations[ran2].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                me->SummonCreature(MOB_ENRAGED_WRAITH, SpawnLocations[ran3].x, SpawnLocations[ran3].y, SpawnLocations[ran3].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000);
                WaveTimer = 30000;
            } else WaveTimer -= diff;
        }
    };
};

#ifndef __clang_analyzer__
void AddSC_eversong_woods()
{
    new npc_apprentice_mirveda();
    new npc_infused_crystal();
}
#endif
