////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "firelands.h"
#include "boss_ragnaros_firelands.h"

enum ScriptTextsRagnaros
{
    SAY_INTRO       = 1,
    SAY_AGGRO       = 12,
    SAY_EVENT       = 11,
    SAY_WRATH       = 10,
    SAY_ADDS        = 0,
    SAY_PICKUP      = 5,
    SAY_DROP        = 4,
    SAY_KILL        = 9,
    SAY_DEATH_1     = 3,
    SAY_DEATH_2     = 2,
    SAY_HEROIC_1    = 8,
    SAY_HEROIC_2    = 7,
    SAY_HEROIC_3    = 6,
};

enum ScriptTextsMalfurion
{ 
    SAY_MALFURION_EVENT_1 = 0,
    SAY_MALFURION_EVENT_2 = 1,
    SAY_MALFURION_EVENT_3 = 2,
};

enum ScriptTextsCenarius
{
    SAY_CENARIUS_EVENT_1 = 1,
    SAY_CENARIUS_EVENT_2 = 2,
    SAY_CENARIUS_EVENT_3 = 0,
    SAY_CENARIUS_EVENT_4 = 3,
};

enum ScriptTextsHamuul
{
    SAY_HAMUUL_EVENT_1 = 0,
};

enum Spells
{
    // Ragnaros
    SPELL_BERSERK                       = 47008, // ?
    SPELL_BURNING_WOUND_AURA            = 99401, // need cd for creatures?
    SPELL_BURNING_WOUND                 = 99399,
    SPELL_BURNING_BLAST                 = 99400,
    SPELL_SULFURAS_SMASH_AOE            = 98703, // select player
    SPELL_SULFURAS_SMASH_SUMMON_1       = 98713, // player summon stalker
    SPELL_SULFURAS_SMASH_SUMMON_2       = 98706, // main sulfuras stalker ?
    SPELL_SULFURAS_SMASH                = 98710, // main spell
    SPELL_SULFURAS_SMASH_DMG            = 98708, // damage
    SPELL_SULFURAS_SMASH_DMG_25         = 100256,
    SPELL_SULFURAS_SMASH_DMG_10H        = 100257,
    SPELL_SULFURAS_SMASH_DMG_25H        = 100258,
    SPELL_LAVA_POOL                     = 98712,
    SPELL_SULFURAS_AURA                 = 100456, // aura on stalker ?
    SPELL_LAVA_WAVE_AURA                = 98873,
    SPELL_LAVALOGGED_10                 = 101088,
    SPELL_LAVALOGGED_25                 = 101102,
    SPELL_SCORCHED_GROUND               = 98871,
    SPELL_WRATH_OF_RAGNAROS_AOE         = 98259,
    SPELL_WRATH_OF_RAGNAROS_DMG         = 98263,
    SPELL_HAND_OF_RAGNAROS              = 98237,
    SPELL_MAGMA_TRAP_AOE                = 98159,
    SPELL_MAGMA_TRAP                    = 98164,
    SPELL_MAGMA_TRAP_DMG                = 98170,
    SPELL_MAGMA_TRAP_AURA               = 98172,
    SPELL_MAGMA_TRAP_AURA_AOE           = 98171,
    SPELL_MAGMA_TRAP_DUMMY              = 98179,
    SPELL_MAGMA_TRAP_ERUPTION           = 98175,
    SPELL_MAGMA_TRAP_VULNERABILITY      = 100238,
    SPELL_MAGMA_BLAST                   = 98313,
    SPELL_SPLITTING_BLOW_1              = 98951,
    SPELL_SPLITTING_BLOW_2              = 98952,
    SPELL_SPLITTING_BLOW_3              = 98953,
    SPELL_SPLITTING_BLOW_SCRIPT         = 99012,
    SPELL_INVOKE_SONS_AURA              = 99056,
    SPELL_INVOKE_SONS_SUMMON_1          = 99014,
    SPELL_INVOKE_SONS_AOE_1             = 99054,
    SPELL_INVOKE_SONS_MISSILE           = 99050,
    SPELL_INVOKE_SONS_AOE_2             = 99051,
    SPELL_HIT_ME                        = 100446, // from sniffs
    SPELL_INVIS_PRE_VISUAL              = 98983,
    SPELL_INVIS_PRE_VISUAL_25           = 100133,
    SPELL_INVIS_PRE_VISUAL_10H          = 100134,
    SPELL_INVIS_PRE_VISUAL_25H          = 100135,
    SPELL_SUBMERGE                      = 100051,
    SPELL_SUBMERGE_AURA                 = 98982,
    SPELL_SUBMERGE_AURA_25              = 100295,
    SPELL_SUBMERGE_AURA_10H             = 100296,
    SPELL_SUBMERGE_AURA_25H             = 100297,
    SPELL_SUPERNOVA                     = 99112,
    SPELL_BLAZING_SPEED_AURA            = 98473,
    SPELL_BLAZING_SPEED                 = 99414,
    SPELL_BLAZING_SPEED_25              = 100306,
    SPELL_BLAZING_SPEED_10H             = 100307,
    SPELL_BLAZING_SPEED_25H             = 100308,
    SPELL_MOLTEN_SEED_AOE_1             = 98333, 
    SPELL_MOLTEN_SEED_AOE_2             = 98497, // hit stalkers
    SPELL_MOLTEN_SEED_MISSILE           = 98495,
    SPELL_MOLTEN_SEED_DMG               = 98498,
    SPELL_MOLTEN_SEED_DMG_25            = 100579,
    SPELL_MOLTEN_SEED_DMG_10H           = 100580,
    SPELL_MOLTEN_SEED_DMG_25H           = 100581,
    SPELL_MOLTEN_SEED_VISUAL            = 98520,
    SPELL_MOLTEN_INFERNO                = 98518,
    SPELL_MOLTEN_POWER                  = 100157,
    SPELL_ENGULFING_FLAMES_NEAR_VISUAL  = 99216,
    SPELL_ENGULFING_FLAMES_MID_VISUAL   = 99217,
    SPELL_ENGULFING_FLAMES_FAR_VISUAL   = 99218,
    SPELL_ENGULFING_FLAMES_NEAR         = 99172,
    SPELL_ENGULFING_FLAMES_MID          = 99235,
    SPELL_ENGULFING_FLAMES_FAR          = 99236,
    SPELL_WORLD_IN_FLAME                = 100171,
    SPELL_ENGULFING_FLAME_SCRIPT        = 99171,
    SPELL_BLAZING_HEAT                  = 100460,
    SPELL_BLAZING_HEAT_AURA_1           = 99126, // on players
    SPELL_BLAZING_HEAT_AURA_1_25        = 100984,
    SPELL_BLAZING_HEAT_AURA_1_10H       = 100985,
    SPELL_BLAZING_HEAT_AURA_1_25H       = 100986,
    SPELL_BLAZING_HEAT_DUMMY            = 99125, // spawn pools
    SPELL_BLAZING_HEAT_SUMMON           = 99129,
    SPELL_BLAZING_HEAT_AURA_2           = 99128,
    SPELL_LIVING_METEOR_TRANSFORM       = 99301, // visual for meteor (what is for?)
    SPELL_LIVING_METEOR_AOE_1           = 99267, // to select players
    SPELL_LIVING_METEOR_MISSILE         = 99268,
    SPELL_LIVING_METEOR_AURA_1          = 99269,
    SPELL_LIVING_METEOR_AURA_1_25       = 100279,
    SPELL_LIVING_METEOR_AURA_1_10H      = 100280,
    SPELL_LIVING_METEOR_AURA_1_25H      = 100281,
    SPELL_LIVING_METEOR_AOE_2           = 99279, // select target to explode
    SPELL_LIVING_METEOR_AURA_2          = 100278, // stacking speed
    SPELL_LIVING_METEOR_AURA_2_25       = 100285,
    SPELL_LIVING_METEOR_SPEED           = 100277,
    SPELL_LIVING_METEOR_SPEED_25        = 100286,
    SPELL_LIVING_METEOR_DMG_REDUCTION   = 100904,
    SPELL_LIVING_METEOR_AURA_3          = 100910, // near meteor debuf
    SPELL_LIVING_METEOR_AURA_4          = 100911, // near knockbacked meteor debuff
    SPELL_METEOR_IMPACT                 = 99287,
    SPELL_COMBUSTIBLE                   = 99296,
    SPELL_COMBUSTIBLE_25                = 100282,
    SPELL_COMBUSTIBLE_10H               = 100283,
    SPELL_COMBUSTIBLE_25H               = 100284,
    SPELL_COMBUSTION                    = 99303,
    SPELL_FIXATE                        = 99849,
    SPELL_LEGS_HEAL                     = 100346,
    SPELL_LEGS_SUBMERGE                 = 100312,
    SPELL_TRANSFORM                     = 100420,
    SPELL_SUPERHEATED                   = 100593,
    SPELL_SUPERHEATED_DMG_10H           = 100594,
    SPELL_SUPERHEATED_DMG_25H           = 100915,
    SPELL_DREADFLAME_MISSILE            = 100675,
    SPELL_DREADFLAME_SUMMON             = 100679,
    SPELL_DREADFLAME_DUMMY              = 100691,
    SPELL_DREADFLAME_AURA               = 100692,
    SPELL_DREADFLAME_DMG                = 100941,
    SPELL_EMPOWER_SULFURAS              = 100604,
    SPELL_RAGE_OF_RAGNAROS_AOE          = 101107,
    SPELL_RAGE_OF_RAGNAROS              = 101110,

    SPELL_ENCOUNTER_COMPLETE            = 102237,
    SPELL_ACHIEVEMENT                   = 101091,

    // Malfurion & Cloudburst
    SPELL_DRAW_OUT_FIRELORD_1           = 100342, // Cosmetic beam at beginning
    SPELL_TRANSFORM_MALFURION           = 100310,
    SPELL_CLOUDBURST_AOE                = 100751,
    SPELL_CLOUDBURST_MISSILE            = 100714,
    SPELL_CLOUDBURST_DUMMY              = 100758,
    SPELL_CLOUDBURST_SCRIPT             = 100759,
    SPELL_DELUGE_AURA_10H               = 100713,
    SPELL_DELUGE_AURA_25H               = 101015,

    // Hamuul & Entrapping Roots
    SPELL_DRAW_OUT_FIRELORD_2           = 100344, // Cosmetic beam at beginning
    SPELL_SUMMON_ENTRAPPING_ROOTS       = 100644,
    SPELL_ENTRAPPING_ROOTS_SCRIPT       = 100645,
    SPELL_ENTRAPPING_ROOTS_MISSILE      = 100646,
    SPELL_ENTRAPPING_ROOTS_AURA         = 100647,
    SPELL_ENTRAPPING_ROOTS_DMG_10H      = 100653,
    SPELL_ENTRAPPING_ROOTS_DMG_25H      = 101237,

    // Cenarius & Breadth of Frost
    SPELL_DRAW_OUT_FIRELORD_3           = 100345, // Cosmetic beam at beginning
    SPELL_FREEZE                        = 100907,
    SPELL_TRANSFORM_HAMUUL              = 100311,
    SPELL_SUMMON_BREADTH_OF_FROST       = 100476,
    SPELL_BREADTH_OF_FROST              = 100479,
    SPELL_BREADTH_OF_FROST_AURA         = 100478,
    SPELL_BREADTH_OF_FROST_ALLY         = 100503,
    SPELL_BREADTH_OF_FROST_DMG          = 100567,
};

enum Adds
{
    NPC_SULFURAS_SMASH_1            = 53266,
    NPC_SULFURAS_SMASH_2            = 53268,
    NPC_LAVA_SCION                  = 53231,
    NPC_SULFURAS_HAND_OF_RAGNAROS_1 = 53420,
    NPC_SULFURAS_HAND_OF_RAGNAROS_2 = 53419,
    NPC_PLATFORM_STALKER            = 53952,
    NPC_SPLITTING_BLOW              = 53393,
    NPC_MAGMA_TRAP                  = 53086,
    NPC_SON_OF_FLAME                = 53140,
    NPC_LAVA_WAVE                   = 53363,
    NPC_MOLTEN_SEED_CASTER          = 53186,
    NPC_ENGULFING_FLAMES            = 53485,
    NPC_MOLTEN_ELEMENTAL            = 53189,
    NPC_MAGMA                       = 53729,
    NPC_BLAZING_HEAT                = 53473,
    NPC_LIVING_METEOR               = 53500,
    NPC_DREADFLAME_SPAWN            = 54203,
    NPC_DREADFLAME                  = 54127,
    NPC_MAGMA_GEYSER                = 54184, // noot need in 4.3

    NPC_MALFURION_STORMRAGE         = 53875,
    NPC_CLOUDBURST                  = 54147,
    NPC_HAMUUL_RUNETOTEM            = 53876,
    NPC_ENTRAPPING_ROOTS            = 54074,
    NPC_CENARIUS                    = 53872,
    NPC_BREADTH_OF_FROST            = 53953,
};

enum Events
{
    EVENT_SULFURAS_SMASH    = 1,
    EVENT_WRATH_OF_RAGNAROS = 2,
    EVENT_HAND_OF_RAGNAROS  = 3,
    EVENT_MAGMA_TRAP        = 4,
    EVENT_LAVA_WAVE         = 5,
    EVENT_CONTINUE          = 6,
    EVENT_CHECK_PLAYER      = 7,
    EVENT_CHECK_SONS        = 8,
    EVENT_CONTINUE_PHASE_2  = 9,
    EVENT_ENGULFING_FLAMES  = 10,
    EVENT_MOLTEN_SEED       = 11,
    EVENT_SUBMERGE          = 12,
    EVENT_CONTINUE_PHASE_3  = 13,
    EVENT_BLAZING_HEAT      = 14,
    EVENT_BERSERK           = 15,
    EVENT_CHECK_TARGET      = 16,
    EVENT_DESPAWN           = 17,
    EVENT_FIXATE            = 18,
    EVENT_COMBUSTIBLE       = 19,
    EVENT_LIVING_METEOR     = 20,
    EVENT_EVENT_1           = 21,
    EVENT_EVENT_2           = 22,
    EVENT_EVENT_3           = 23,
    EVENT_EVENT_4           = 24,
    EVENT_EVENT_5           = 25,
    EVENT_DRAW_OUT          = 26,
    EVENT_BREADTH_OF_FROST  = 27,
    EVENT_DREADFLAME        = 28,
    EVENT_DREADFLAME_UPDATE = 29,
    EVENT_CHECK_CLOUDBURST  = 30,
    EVENT_CLOUDBURST        = 31,
    EVENT_ENTRAPPING_ROOTS  = 32,
    EVENT_EMPOWER_SULFURAS  = 33,
    EVENT_CONTINUE_METEOR   = 34,
    EVENT_RAGE_OF_RAGNAROS  = 35,
    EVENT_BURNING_WOUND     = 36,
    EVENT_EVENT_WIN_1       = 37,
    EVENT_EVENT_WIN_2       = 38,
    EVENT_EVENT_WIN_3       = 39,
    EVENT_RAGNAROS_UP       = 40,
};

enum Actions
{
    ACTION_LAVA_WAVE            = 1,
    ACTION_SPLITTING_BLOW       = 2,
    ACTION_WORLD_IN_FLAME_OFF   = 3,
    ACTION_COMBUSTION           = 4,
    ACTION_EXPLODE              = 5,
    ACTION_EMPOWER_SULFURAS     = 6,
    ACTION_WIN                  = 7,
    ACTION_LAVALOGGED           = 8,
    ACTION_RAGNAROS_UP          = 9,
};

enum DisplayIds
{
    MODEL_INVIS     = 15435,
    MODEL_SON       = 1070,
    MODEL_ELEM      = 38520,
};

enum Points
{
    POINT_SULFURAS   = 1,
};

enum OtherData
{
    DATA_DREADFLAME_ID  = 1,
    DATA_CLICKS         = 2,
};

class CloudburstCheck
{
        public:
            CloudburstCheck(WorldObject const* obj) : i_obj(obj) {}
            bool operator()(Unit* u)
            {
                if (u->isAlive() && i_obj->GetDistance(u) <= 3.0f && u->HasAura(u->GetMap()->Is25ManRaid() ? SPELL_DELUGE_AURA_25H : SPELL_DELUGE_AURA_10H))
                    return true;

                return false;
            }
        private:
            WorldObject const* i_obj;
};

class boss_ragnaros_firelands : public CreatureScript
{
    public:
        boss_ragnaros_firelands() : CreatureScript("boss_ragnaros_firelands") { }

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_ragnaros_firelandsAI(pCreature);
        }

        struct boss_ragnaros_firelandsAI : public BossAI
        {
            boss_ragnaros_firelandsAI(Creature* pCreature) : BossAI(pCreature, DATA_RAGNAROS)
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
                me->SetDisableGravity(true);
                me->SetCanFly(true);
            }

            void InitializeAI() override
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(FLScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            bool AllowAchieve()
            {
                return false;
            }

            void Reset() override
            {
                _Reset();
                DespawnEncounterCreatures();
                DespawnEventCreatures();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELUGE_AURA_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELUGE_AURA_25H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED_DMG_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED_DMG_25H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAGE_OF_RAGNAROS);

                phase = 0;
                Lavalogged = 0;
                bFlames = false;
                bRage = false;
                memset(bFloor, false, sizeof(bFloor));
                uiDreadFlameTimer = 40000;

                me->SetUInt32Value(UNIT_FIELD_ANIM_TIER, 50331648);
                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 50);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 50);
                me->SetReactState(REACT_DEFENSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                if (GameObject* pGo = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_RAGNAROS_FLOOR)))
                    pGo->SetDestructibleState(GO_DESTRUCTIBLE_INTACT);
            }

            void AttackStart(Unit* who, bool /*p_Melee*/ =  true) override
            {
                if (!who)
                    return;
                if (me->HasReactState(REACT_PASSIVE))
                    return;

                if (phase != 5)
                    me->Attack(who, false);
                else
                {
                    if (me->Attack(who, true))
                        me->GetMotionMaster()->MoveChase(who);
                }
            }

            void EnterCombat(Unit* attacker) override
            {
                if (!instance->CheckRequiredBosses(DATA_RAGNAROS, attacker->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoNearTeleportPlayers(FLEntrancePos);
                    return;
                }

                Talk(SAY_AGGRO);

                DespawnEncounterCreatures();
                DespawnEventCreatures();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELUGE_AURA_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELUGE_AURA_25H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED_DMG_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED_DMG_25H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAGE_OF_RAGNAROS);

                phase = 0;
                Lavalogged = 0;
                bFlames = false;
                bRage = false;
                memset(bFloor, false, sizeof(bFloor));
                uiDreadFlameTimer = 40000;

                events.ScheduleEvent(EVENT_HAND_OF_RAGNAROS, 25000);
                events.ScheduleEvent(EVENT_MAGMA_TRAP, 15000);
                events.ScheduleEvent(EVENT_SULFURAS_SMASH, 30000);
                events.ScheduleEvent(EVENT_BERSERK, 18 * MINUTE * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CHECK_TARGET, 4000);
                events.ScheduleEvent(EVENT_BURNING_WOUND, urand(5000, 10000));

                // check for Tarecgosa
                if (!IsHeroic())
                {
                    Map::PlayerList const &PlayerList = instance->instance->GetPlayers();
                    if (!PlayerList.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            if (Player* plr = i->getSource())
                            {
                                uint32 l_ObjectiveID = 0;
                                if (plr->GetQuestStatus(QUEST_HEART_OF_FLAME_HORDE) == QUEST_STATUS_INCOMPLETE)
                                    l_ObjectiveID = QUEST_OBJECTIVE_HEART_OF_FLAME_HORDE;
                                else if (plr->GetQuestStatus(QUEST_HEART_OF_FLAME_ALLIANCE) == QUEST_STATUS_INCOMPLETE)
                                    l_ObjectiveID = QUEST_OBJECTIVE_HEART_OF_FLAME_ALLIANCE;

                                if (l_ObjectiveID)
                                {
                                    if (plr->GetQuestObjectiveCounter(l_ObjectiveID) >= 250)
                                    {
                                        bRage = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if (bRage)
                    events.ScheduleEvent(EVENT_RAGE_OF_RAGNAROS, urand(5000, 10000));

                // Summon magma near Ragnaros
                if (Creature* pMagma = me->SummonCreature(NPC_MAGMA, magmaPos))
                    pMagma->SetDisableGravity(true);

                // Summon molten seed casters in lava under platform
                for (uint8 i = 0; i < 18; ++i)
                    if (Creature* pCaster = me->SummonCreature(NPC_MOLTEN_SEED_CASTER, moltenseedcasterPos[i]))
                        pCaster->SetDisableGravity(true);

                // Summon stalkers for flames
                for (uint8 i = 0; i < MAX_ENGULFING_FLAMES; ++i)
                    if (Creature* pFlame = me->SummonCreature(NPC_ENGULFING_FLAMES, engulfingflamesPos[i]))
                        pFlame->SetDisableGravity(true);

                DoZoneInCombat();
                instance->SetBossState(DATA_RAGNAROS, IN_PROGRESS);
            }

            void EnterEvadeMode() override
            {
                if (instance)
                    instance->SetBossState(DATA_RAGNAROS, FAIL);

                CreatureAI::EnterEvadeMode();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!IsHeroic())
                {
                    /// In Case players oneshot him before his normal mechanic would allow him to be dead
                    if (GetDifficulty() == Difficulty::Difficulty10N)
                        instance->DoRespawnGameObject(instance->GetData64(DATA_RAGNAROS_CACHE_10), DAY);
                    else if (GetDifficulty() == Difficulty::Difficulty25N)
                        instance->DoRespawnGameObject(instance->GetData64(DATA_RAGNAROS_CACHE_25), DAY);

                    me->DespawnOrUnsummon();
                    return;
                }

                instance->SetBossState(DATA_RAGNAROS, DONE);

                _JustDied();
                DespawnEncounterCreatures();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELUGE_AURA_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELUGE_AURA_25H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED_DMG_10H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED_DMG_25H);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAGE_OF_RAGNAROS);

                Talk(SAY_DEATH_2);

                AddSmoulderingAura(me);

                // Achievement Ragnar'os
                if (Lavalogged >= 3)
                    instance->DoUpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT, 0, me); 
                
                if (Creature* pMalfurion = me->FindNearestCreature(NPC_MALFURION_STORMRAGE, 300.0f))
                    pMalfurion->AI()->DoAction(ACTION_WIN);
                if (Creature* pCenarius = me->FindNearestCreature(NPC_CENARIUS, 300.0f))
                    pCenarius->AI()->DoAction(ACTION_WIN);
                if (Creature* pHamuul = me->FindNearestCreature(NPC_HAMUUL_RUNETOTEM, 300.0f))
                    pHamuul->AI()->DoAction(ACTION_WIN);
                
                Map::PlayerList const &plrList = instance->instance->GetPlayers();
                if (!plrList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = plrList.begin(); i != plrList.end(); ++i)
                    {
                        if (Player* plr = i->getSource())
                        {
                            uint32 l_ObjectiveID = 0;
                            if (plr->GetQuestStatus(QUEST_HEART_OF_FLAME_HORDE) == QUEST_STATUS_INCOMPLETE)
                                l_ObjectiveID = QUEST_OBJECTIVE_HEART_OF_FLAME_HORDE;
                            else if (plr->GetQuestStatus(QUEST_HEART_OF_FLAME_ALLIANCE) == QUEST_STATUS_INCOMPLETE)
                                l_ObjectiveID = QUEST_OBJECTIVE_HEART_OF_FLAME_ALLIANCE;

                            if (l_ObjectiveID)
                            {
                                if (plr->GetQuestObjectiveCounter(l_ObjectiveID) >= 250)
                                {
                                    plr->CastSpell(plr, SPELL_HEART_OF_RAGNAROS_CREATE, true);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            
            void KilledUnit(Unit* who) override
            {
                if (who && who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void SpellHit(Unit* who, const SpellInfo* spellInfo) override
            {
                if (spellInfo->Id == SPELL_DRAW_OUT_FIRELORD_1)
                {
                    if (GameObject* pGo = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_RAGNAROS_FLOOR)))
                        pGo->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);

                    summons.DespawnEntry(NPC_MAGMA);
                    summons.DespawnEntry(NPC_MOLTEN_SEED_CASTER);
                    summons.DespawnEntry(NPC_ENGULFING_FLAMES);

                    events.ScheduleEvent(EVENT_EVENT_2, 10000);
                }
                else if (spellInfo->Id == RAID_MODE(SPELL_ENTRAPPING_ROOTS_DMG_10H, SPELL_ENTRAPPING_ROOTS_DMG_25H, SPELL_ENTRAPPING_ROOTS_DMG_10H, SPELL_ENTRAPPING_ROOTS_DMG_25H))
                {
                    if (who->GetEntry() == NPC_ENTRAPPING_ROOTS)
                        who->ToCreature()->DespawnOrUnsummon();

                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() != NPC_CENARIUS &&
                    summon->GetEntry() != NPC_MALFURION_STORMRAGE &&
                    summon->GetEntry() != NPC_HAMUUL_RUNETOTEM)
                    BossAI::JustSummoned(summon);
            }

            void DoAction(const int32 action) override
            {
                switch(action)
                {
                    case ACTION_LAVA_WAVE:
                        events.ScheduleEvent(EVENT_LAVA_WAVE, 500);
                        break;
                    case ACTION_SPLITTING_BLOW:
                    {
                        Talk(SAY_ADDS);
                        EntryCheckPredicate pred1(NPC_SPLITTING_BLOW);
                        summons.DoAction(ACTION_SPLITTING_BLOW, pred1);
                        EntryCheckPredicate pred2(NPC_SON_OF_FLAME);
                        summons.DoAction(ACTION_SPLITTING_BLOW, pred2);
                        break;
                    }
                    case ACTION_WORLD_IN_FLAME_OFF:
                        bFlames = false;
                        break;
                    case ACTION_EMPOWER_SULFURAS:
                        events.ScheduleEvent(EVENT_EMPOWER_SULFURAS, urand(7000, 15000));
                        break;
                    case ACTION_LAVALOGGED:
                        Lavalogged++;
                        break;
                    case ACTION_RAGNAROS_UP:
                        events.ScheduleEvent(EVENT_SULFURAS_SMASH, 15500);
                        events.ScheduleEvent(EVENT_MOLTEN_SEED, 15000);
                        events.ScheduleEvent(EVENT_ENGULFING_FLAMES, 40000);
                        events.ScheduleEvent(EVENT_CHECK_TARGET, 5000);
                        events.ScheduleEvent(EVENT_BURNING_WOUND, urand(5000, 10000));
                        break;
                }
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || bFlames)
                    return;

                if (me->HealthBelowPct(70) && phase == 0)
                {
                    phase = 1;
                    
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();

                    events.CancelEvent(EVENT_WRATH_OF_RAGNAROS);
                    events.CancelEvent(EVENT_HAND_OF_RAGNAROS);
                    events.CancelEvent(EVENT_MAGMA_TRAP);
                    events.CancelEvent(EVENT_SULFURAS_SMASH);
                    events.CancelEvent(EVENT_CONTINUE);
                    events.CancelEvent(EVENT_CHECK_TARGET);
                    events.CancelEvent(EVENT_BURNING_WOUND);
                    events.CancelEvent(EVENT_RAGE_OF_RAGNAROS);
                    DespawnEventCreatures();

                    Talk(SAY_DROP);
                    DoCast(me, SPELL_SUBMERGE, true);

                    uint8 side = urand(0, 1);

                    if (Creature* pStalker = me->SummonCreature(NPC_SPLITTING_BLOW, sulfurasPos[side], TEMPSUMMON_TIMED_DESPAWN, 20000))
                    {
                        me->SetFacingToObject(pStalker);
                        DoCast(pStalker, (side == 1 ? SPELL_SPLITTING_BLOW_1 : SPELL_SPLITTING_BLOW_3));
                    }
                    for (uint8 i = 0; i < 8; ++i)
                    {
                        if (Creature* pSon = me->SummonCreature(NPC_SON_OF_FLAME, sonsPos[side][i]))
                        {
                            pSon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            pSon->SetDisplayId(MODEL_INVIS);
                            pSon->CastSpell(pSon, SPELL_HIT_ME, true);
                            pSon->CastSpell(pSon, SPELL_INVIS_PRE_VISUAL, true);
                        }
                    }
                    events.ScheduleEvent(EVENT_CHECK_SONS, 10000);
                    events.ScheduleEvent(EVENT_SUBMERGE, 10000);
                    events.ScheduleEvent(EVENT_CONTINUE_PHASE_2, 55000);
                    return;
                }
                else if (me->HealthBelowPct(40) && phase == 2)
                {
                    phase = 3;
                    
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();

                    events.CancelEvent(EVENT_ENGULFING_FLAMES);
                    events.CancelEvent(EVENT_SULFURAS_SMASH);
                    events.CancelEvent(EVENT_CONTINUE);
                    events.CancelEvent(EVENT_MOLTEN_SEED);
                    events.CancelEvent(EVENT_CHECK_TARGET);
                    events.CancelEvent(EVENT_BURNING_WOUND);
                    events.CancelEvent(EVENT_RAGE_OF_RAGNAROS);
                    DespawnEventCreatures();

                    Talk(SAY_DROP);
                    DoCast(me, SPELL_SUBMERGE, true);

                    uint8 side = urand(0, 1);

                    if (Creature* pStalker = me->SummonCreature(NPC_SPLITTING_BLOW, sulfurasPos[side], TEMPSUMMON_TIMED_DESPAWN, 20000))
                    {
                        me->SetFacingToObject(pStalker);
                        DoCast(pStalker, (side == 1 ? SPELL_SPLITTING_BLOW_1 : SPELL_SPLITTING_BLOW_3));
                    }
                    for (uint8 i = 0; i < 8; ++i)
                    {
                        if (Creature* pSon = me->SummonCreature(NPC_SON_OF_FLAME, sonsPos[side][i]))
                        {
                            pSon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            pSon->SetDisplayId(MODEL_INVIS);
                            pSon->CastSpell(pSon, SPELL_HIT_ME, true);
                            pSon->CastSpell(pSon, SPELL_INVIS_PRE_VISUAL, true);
                        }
                    }

                    for (uint8 i = 0; i < 2; ++i)
                        me->SummonCreature(NPC_LAVA_SCION, lavascionPos[i]);

                    events.ScheduleEvent(EVENT_CHECK_SONS, 10000);
                    events.ScheduleEvent(EVENT_SUBMERGE, 10000);
                    events.ScheduleEvent(EVENT_CONTINUE_PHASE_3, 55000);
                    return;
                }
                if (me->HealthBelowPct(10) && phase == 4)
                {
                    phase = 5;
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    if (IsHeroic())
                    {
                        events.CancelEvent(EVENT_ENGULFING_FLAMES);
                        events.CancelEvent(EVENT_LIVING_METEOR);
                        events.CancelEvent(EVENT_SULFURAS_SMASH);
                        events.CancelEvent(EVENT_CHECK_TARGET);
                        events.CancelEvent(EVENT_BURNING_WOUND);
                        events.CancelEvent(EVENT_RAGE_OF_RAGNAROS);

                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveAllAuras();
                        DoCast(me, SPELL_LEGS_SUBMERGE, true);
                        Talk(SAY_HEROIC_1);
                        events.ScheduleEvent(EVENT_EVENT_1, 5000);
                    }
                    else
                    {
                        CompleteNormalEncounter();
                        return;
                    }
                }

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_RAGE_OF_RAGNAROS:
                            Talk(SAY_EVENT);
                            DoCastAOE(SPELL_RAGE_OF_RAGNAROS_AOE);
                            break;
                        case EVENT_BURNING_WOUND:
                            DoCastVictim(SPELL_BURNING_WOUND);
                            events.ScheduleEvent(EVENT_BURNING_WOUND, urand(12000, 15000));
                            break;
                        case EVENT_CHECK_TARGET:
                            if (!me->IsWithinMeleeRange(me->getVictim()))
                                DoCastVictim(SPELL_MAGMA_BLAST);
                            events.ScheduleEvent(EVENT_CHECK_TARGET, 2000);
                            break;
                        case EVENT_WRATH_OF_RAGNAROS:
                            Talk(SAY_WRATH);
                            DoCastAOE(SPELL_WRATH_OF_RAGNAROS_AOE);
                            break;
                        case EVENT_HAND_OF_RAGNAROS:
                            DoCastAOE(SPELL_HAND_OF_RAGNAROS);
                            events.ScheduleEvent(EVENT_HAND_OF_RAGNAROS, urand(25000, 30000));
                            break;
                        case EVENT_MAGMA_TRAP:
                            DoCastAOE(SPELL_MAGMA_TRAP_AOE);
                            events.ScheduleEvent(EVENT_MAGMA_TRAP, urand(25000, 30000));
                            break;
                        case EVENT_SULFURAS_SMASH:
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            DoCastAOE(SPELL_SULFURAS_SMASH_AOE);
                            events.ScheduleEvent(EVENT_SULFURAS_SMASH, ((phase == 2) ? 40000 : 30000));
                            events.ScheduleEvent(EVENT_CONTINUE, 5000);
                            if (phase == 0)
                                events.ScheduleEvent(EVENT_WRATH_OF_RAGNAROS, 12000);
                            break;
                        case EVENT_LAVA_WAVE:
                        {
                            EntryCheckPredicate pred(NPC_SULFURAS_SMASH_2);
                            summons.DoAction(ACTION_LAVA_WAVE, pred);
                            break;
                        }
                        case EVENT_CONTINUE:
                            if (phase == 0 || phase == 2 || phase == 4)
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->Attack(me->getVictim(), false);
                            }
                            break;
                        case EVENT_SUBMERGE:
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            break;
                        case EVENT_CHECK_SONS:
                            if (!me->FindNearestCreature(NPC_SON_OF_FLAME, 300.0f))
                            {
                                if (phase == 1)
                                    events.RescheduleEvent(EVENT_CONTINUE_PHASE_2, 2000);
                                else if (phase == 3)
                                    events.RescheduleEvent(EVENT_CONTINUE_PHASE_3, 2000);
                            }
                            else
                                events.ScheduleEvent(EVENT_CHECK_SONS, 2000);
                            break;
                        case EVENT_CONTINUE_PHASE_2:
                            events.CancelEvent(EVENT_CHECK_SONS);
                            summons.DespawnEntry(NPC_SON_OF_FLAME);
                            phase = 2;
                            Talk(SAY_PICKUP);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            summons.DespawnEntry(NPC_SULFURAS_HAND_OF_RAGNAROS_1);
                            summons.DespawnEntry(NPC_SULFURAS_HAND_OF_RAGNAROS_2);
                            DespawnCreatures(NPC_SULFURAS_HAND_OF_RAGNAROS_1);
                            DespawnCreatures(NPC_SULFURAS_HAND_OF_RAGNAROS_2);
                            me->RemoveAura(RAID_MODE(SPELL_SUBMERGE_AURA, SPELL_SUBMERGE_AURA_25, SPELL_SUBMERGE_AURA_10H, SPELL_SUBMERGE_AURA_25H));
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->Attack(me->getVictim(), false);
                            events.ScheduleEvent(EVENT_SULFURAS_SMASH, (IsHeroic() ? 6000 : 15500));
                            events.ScheduleEvent(EVENT_MOLTEN_SEED, (IsHeroic() ? 15000 : 21500));
                            events.ScheduleEvent(EVENT_ENGULFING_FLAMES, 40000);
                            events.ScheduleEvent(EVENT_CHECK_TARGET, 5000);
                            events.ScheduleEvent(EVENT_BURNING_WOUND, urand(5000, 10000));
                            break;
                        case EVENT_ENGULFING_FLAMES:
                            if (!IsHeroic())
                            {
                                switch (urand(0, 2))
                                {
                                    case 0:
                                        DoCastAOE(SPELL_ENGULFING_FLAMES_NEAR_VISUAL, true);
                                        DoCastAOE(SPELL_ENGULFING_FLAMES_NEAR);
                                        break;
                                    case 1:
                                        DoCastAOE(SPELL_ENGULFING_FLAMES_MID_VISUAL, true);
                                        DoCastAOE(SPELL_ENGULFING_FLAMES_MID);
                                        break;
                                    case 2:
                                        DoCastAOE(SPELL_ENGULFING_FLAMES_FAR_VISUAL, true);
                                        DoCastAOE(SPELL_ENGULFING_FLAMES_FAR);
                                        break;
                                }
                            }
                            else
                            {
                                bFlames = true;
                                //DoCast(me, SPELL_WORLD_IN_FLAME); Don't work. Why?
                                me->AddAura(SPELL_WORLD_IN_FLAME, me);
                            }
                            events.RescheduleEvent(EVENT_CHECK_TARGET, 12000);
                            events.ScheduleEvent(EVENT_ENGULFING_FLAMES, ((phase == 2) ? 40000 : 30000));
                            break;
                        case EVENT_MOLTEN_SEED:
                        {
                            me->CastCustomSpell(SPELL_MOLTEN_SEED_AOE_1, SPELLVALUE_MAX_TARGETS, (Is25ManRaid() ? 20 : 10), me, true); 
                            events.ScheduleEvent(EVENT_MOLTEN_SEED, 60000);
                            break;
                        }
                        case EVENT_CONTINUE_PHASE_3:
                            events.CancelEvent(EVENT_CHECK_SONS);
                            summons.DespawnEntry(NPC_SON_OF_FLAME);
                            phase = 4;
                            Talk(SAY_PICKUP);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            summons.DespawnEntry(NPC_SULFURAS_HAND_OF_RAGNAROS_1);
                            summons.DespawnEntry(NPC_SULFURAS_HAND_OF_RAGNAROS_2);
                            DespawnCreatures(NPC_SULFURAS_HAND_OF_RAGNAROS_1);
                            DespawnCreatures(NPC_SULFURAS_HAND_OF_RAGNAROS_2);
                            me->RemoveAura(RAID_MODE(SPELL_SUBMERGE_AURA, SPELL_SUBMERGE_AURA_25, SPELL_SUBMERGE_AURA_10H, SPELL_SUBMERGE_AURA_25H));
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->Attack(me->getVictim(), false);
                            events.ScheduleEvent(EVENT_SULFURAS_SMASH, 15500);
                            events.ScheduleEvent(EVENT_LIVING_METEOR, 45000);
                            events.ScheduleEvent(EVENT_ENGULFING_FLAMES, 30000);
                            events.ScheduleEvent(EVENT_CHECK_TARGET, 5000);
                            events.ScheduleEvent(EVENT_BURNING_WOUND, urand(5000, 10000));
                            break;
                        case EVENT_LIVING_METEOR:
                            DoCastAOE(SPELL_LIVING_METEOR_AOE_1);
                            events.ScheduleEvent(EVENT_LIVING_METEOR, 45000);
                            break;
                        case EVENT_DESPAWN:
                            summons.DespawnAll();
                            DespawnEncounterCreatures();
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELUGE_AURA_10H);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DELUGE_AURA_25H);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED_DMG_10H);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED_DMG_25H);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAGE_OF_RAGNAROS);

                            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                me->SetVisible(false);
                                pTarget->Kill(me);
                            }
                            else
                                me->DespawnOrUnsummon(2000);
                            break;                 
                        case EVENT_EVENT_1:
                            me->SummonCreature(NPC_MALFURION_STORMRAGE, npcPos[0]);
                            me->SummonCreature(NPC_CENARIUS, npcPos[1]);
                            me->SummonCreature(NPC_HAMUUL_RUNETOTEM, npcPos[2]);
                            break; 
                        case EVENT_EVENT_2:
                            me->RemoveAura(SPELL_DRAW_OUT_FIRELORD_1);
                            me->RemoveAura(SPELL_DRAW_OUT_FIRELORD_2);
                            me->RemoveAura(SPELL_DRAW_OUT_FIRELORD_3);
                            me->RemoveAura(SPELL_LEGS_SUBMERGE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            
                            DoCast(me, SPELL_TRANSFORM, true);
                            // DoCast(me, SPELL_LEGS_HEAL, true);
                            me->SetHealth(me->CountPctFromMaxHealth(50));

                            me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 56.0f, me->GetOrientation(), true);
                            Talk(SAY_HEROIC_2);

                            me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 8);
                            me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 8);
                            
                            events.ScheduleEvent(EVENT_EVENT_3, 9000);
                            break;
                        case EVENT_EVENT_3:
                            
                            //me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_INSTANTLY_APPEAR_MODEL);
                            if (Creature* pMalfurion = me->FindNearestCreature(NPC_MALFURION_STORMRAGE, 100.0f))
                                pMalfurion->AI()->Talk(SAY_MALFURION_EVENT_1);

                            events.ScheduleEvent(EVENT_EVENT_4, 7000);
                            break;
                        case EVENT_EVENT_4:
                            if (GameObject* pGo = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_RAGNAROS_FLOOR)))
                                pGo->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                            me->SetDisableGravity(false);
                            me->SetCanFly(false);
                            me->SetUInt32Value(UNIT_FIELD_ANIM_TIER, 0);
                            me->RemoveUnitMovementFlag(MOVEMENTFLAG_SWIMMING);
                            Talk(SAY_HEROIC_3);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->Attack(me->getVictim(), true);
                            me->GetMotionMaster()->MoveChase(me->getVictim());
                            events.ScheduleEvent(EVENT_EVENT_5, 2000);
                            break;
                        case EVENT_EVENT_5:
                            DoCast(me, SPELL_SUPERHEATED, true);
                            events.ScheduleEvent(EVENT_DREADFLAME, 15000);
                            if (GameObject* pGo = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_RAGNAROS_FLOOR)))
                                pGo->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
                            break;
                        case EVENT_DREADFLAME:
                        {
                            uint8 max = (Is25ManRaid() ? 6 : 2);
                            std::list<Position> t_pos;
                            for (uint32 i = 0; i < MAX_FLOOR_POS; ++i)
                                t_pos.push_back(floorPos[i]);

                            if (t_pos.size() > max)
                                JadeCore::Containers::RandomResizeList(t_pos, max);

                            if (!t_pos.empty())
                                for (std::list<Position>::const_iterator itr = t_pos.begin(); itr != t_pos.end(); ++itr)
                                    me->CastSpell((*itr).GetPositionX(), (*itr).GetPositionY(), (*itr).GetPositionZ(), SPELL_DREADFLAME_MISSILE, true);

                            if (uiDreadFlameTimer > 15000)
                                uiDreadFlameTimer -= 5000;
                            events.ScheduleEvent(EVENT_DREADFLAME, uiDreadFlameTimer);
                            break;
                        }
                        case EVENT_EMPOWER_SULFURAS:
                            DoCast(me, SPELL_EMPOWER_SULFURAS);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            uint8 phase;
            bool bFlames;
            bool bRage;
            uint8 Lavalogged;
            uint32 uiDreadFlameTimer;

            void DespawnEncounterCreatures()
            {
                DespawnCreatures(NPC_SULFURAS_HAND_OF_RAGNAROS_1);
                DespawnCreatures(NPC_SULFURAS_HAND_OF_RAGNAROS_2);
                DespawnCreatures(NPC_MOLTEN_ELEMENTAL);
                DespawnCreatures(NPC_LAVA_WAVE);
                DespawnCreatures(NPC_BLAZING_HEAT);
                DespawnCreatures(NPC_DREADFLAME);
                DespawnCreatures(NPC_BREADTH_OF_FROST);
                DespawnCreatures(NPC_CLOUDBURST);
                DespawnCreatures(NPC_ENTRAPPING_ROOTS); 
            }

            void DespawnEventCreatures()
            {
                DespawnCreatures(NPC_CENARIUS);
                DespawnCreatures(NPC_HAMUUL_RUNETOTEM);
                DespawnCreatures(NPC_MALFURION_STORMRAGE);
            }

            void DespawnCreatures(uint32 entry)
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);

                if (creatures.empty())
                   return;

                for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                     (*iter)->DespawnOrUnsummon();
            }

            void CompleteNormalEncounter()
            {
                events.Reset();
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->RemoveAllAuras();
                DoCast(me, SPELL_LEGS_SUBMERGE);
                Talk(SAY_DEATH_1);
                
                // Achievement Ragnar'os
                if (Lavalogged >= 3)
                    instance->DoUpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT, 0, me); 

                // Achievement
                instance->DoUpdateCriteria(CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ENCOUNTER_COMPLETE, 0, me); 

                instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_ENCOUNTER_COMPLETE, me); 
                instance->SetBossState(DATA_RAGNAROS, DONE);

                if (GetDifficulty() == Difficulty::Difficulty10N)
                    instance->DoRespawnGameObject(instance->GetData64(DATA_RAGNAROS_CACHE_10), DAY);
                else if (GetDifficulty() == Difficulty::Difficulty25N)
                    instance->DoRespawnGameObject(instance->GetData64(DATA_RAGNAROS_CACHE_25), DAY);

                events.ScheduleEvent(EVENT_DESPAWN, 3000);
            }
        };
};

class npc_ragnaros_firelands_sulfuras_smash : public CreatureScript
{
    public:
        npc_ragnaros_firelands_sulfuras_smash() : CreatureScript("npc_ragnaros_firelands_sulfuras_smash") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_sulfuras_smashAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_sulfuras_smashAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_sulfuras_smashAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                pInstance = me->GetInstanceScript();
            }

            void Reset()
            {
                if (!pInstance)
                    return;

                if (Creature* pRagnaros = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_RAGNAROS)))
                {    
                    if (me->GetEntry() == NPC_SULFURAS_SMASH_1)
                    {
                        pRagnaros->SetFacingToObject(me);
                        pRagnaros->CastSpell(me, SPELL_SULFURAS_SMASH_SUMMON_2, true);
                    }
                    else
                    {
                        DoCast(me, SPELL_LAVA_POOL, true);
                        pRagnaros->CastSpell(me, SPELL_SULFURAS_SMASH);
                    }
                }
            }

            void DoAction(const int32 action)
            {
                if (!pInstance)
                    return;

                if (action == ACTION_LAVA_WAVE)
                {
                    Position startPos[3];
                    Position endPos[3];
                    me->GetNearPosition(startPos[0], 1.0f, -(M_PI / 2.0f));
                    me->GetNearPosition(startPos[1], 1.0f, -(M_PI / 2.0f));
                    me->GetNearPosition(startPos[2], 1.0f, 0.0f);
                    me->GetNearPosition(endPos[0], 100.0f, -(M_PI / 2.0f));
                    me->GetNearPosition(endPos[1], 100.0f, 0.0f);
                    me->GetNearPosition(endPos[2], 100.0f, (M_PI / 2.0f));

                    for (uint8 i = 0; i < 3; ++i)
                        SendLavaWave(startPos[i], endPos[i]);

                    me->RemoveAura(SPELL_LAVA_POOL);
                    DoCast(me, SPELL_SULFURAS_AURA, true);
                }
            }

        private:
            InstanceScript* pInstance;

            void SendLavaWave(Position startpos, Position endpos)
            {
                if (Creature* pRagnaros = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_RAGNAROS)))
                    if (Creature* pLavaWave = pRagnaros->SummonCreature(NPC_LAVA_WAVE, startpos, TEMPSUMMON_TIMED_DESPAWN, 15000))
                    {
                        pLavaWave->CastSpell(pLavaWave, SPELL_LAVA_WAVE_AURA, true);
                        if (Creature* pStalker = me->SummonCreature(NPC_PLATFORM_STALKER, endpos, TEMPSUMMON_TIMED_DESPAWN, 30000))
                        {
                            pStalker->SetDisableGravity(true);
                            pStalker->SetCanFly(true);
                            pLavaWave->AI()->SetGUID(pStalker->GetGUID());
                        }
                    }
            }
        };
};

class npc_ragnaros_firelands_lava_wave : public CreatureScript
{
    public:
        npc_ragnaros_firelands_lava_wave() : CreatureScript("npc_ragnaros_firelands_lava_wave") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_lava_waveAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_lava_waveAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_lava_waveAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                bDest = false;
                bDespawn = false;
                destGUID = 0;
            }

            void SetGUID(uint64 guid, int32 /*type*/)
            {
                bDest = true;
                destGUID = guid;
                events.ScheduleEvent(EVENT_CONTINUE, 400);
            }

            void UpdateAI(const uint32 diff)
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    if (bDest)
                        if (Creature* pStalker = ObjectAccessor::GetCreature(*me, destGUID))
                            me->GetMotionMaster()->MoveFollow(pStalker, 0, 0.0f);
                }

                if (!bDespawn && bDest)
                {
                    if (Creature* pStalker = ObjectAccessor::GetCreature(*me, destGUID))
                        if (me->GetDistance(pStalker) <= 1.0f)
                        {
                            bDespawn = true;
                            me->DespawnOrUnsummon();
                        }
                }
            }
        private:
            EventMap events;
            bool bDest;
            bool bDespawn;
            uint64 destGUID;
        };
};

class npc_ragnaros_firelands_magma_trap : public CreatureScript
{
    public:
        npc_ragnaros_firelands_magma_trap() : CreatureScript("npc_ragnaros_firelands_magma_trap") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_magma_trapAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_magma_trapAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_magma_trapAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                DoCast(me, SPELL_MAGMA_TRAP_DUMMY);
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_CHECK_PLAYER, 500);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    if (me->FindNearestPlayer(5.0f))
                    {
                        DoCastAOE(SPELL_MAGMA_TRAP_ERUPTION);
                        me->DespawnOrUnsummon(500);
                    }
                    else
                        events.ScheduleEvent(EVENT_CHECK_PLAYER, 500);
                }
            }
        private:
            EventMap events;
        };
};

class npc_ragnaros_firelands_splitting_blow : public CreatureScript
{
    public:
        npc_ragnaros_firelands_splitting_blow() : CreatureScript("npc_ragnaros_firelands_splitting_blow") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_splitting_blowAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_splitting_blowAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_splitting_blowAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void DoAction(const int32 action)
            {
                if (action == ACTION_SPLITTING_BLOW)
                {
                    DoCast(me, SPELL_INVOKE_SONS_AURA, true);
                }
            }
        };
};

class npc_ragnaros_firelands_sulfuras_hand_of_ragnaros : public CreatureScript
{
    public:
        npc_ragnaros_firelands_sulfuras_hand_of_ragnaros() : CreatureScript("npc_ragnaros_firelands_sulfuras_hand_of_ragnaros") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_sulfuras_hand_of_ragnarosAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_sulfuras_hand_of_ragnarosAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_sulfuras_hand_of_ragnarosAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                DoZoneInCombat();
                DoCastAOE(SPELL_INVOKE_SONS_AOE_1, true);
                DoCast(me, SPELL_SULFURAS_AURA, true);
            }
        };
};

class npc_ragnaros_firelands_son_of_flame : public CreatureScript
{
    public:
        npc_ragnaros_firelands_son_of_flame() : CreatureScript("npc_ragnaros_firelands_son_of_flame") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_son_of_flameAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_son_of_flameAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_son_of_flameAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset()
            {
                events.Reset();
                bDespawn = false;
            }

            void SpellHit(Unit* /*who*/, const SpellInfo* spellInfo)
            {
                if (spellInfo->Id == SPELL_INVOKE_SONS_AOE_2)
                {
                    me->RemoveAura(RAID_MODE(SPELL_INVIS_PRE_VISUAL, SPELL_INVIS_PRE_VISUAL_25, SPELL_INVIS_PRE_VISUAL_10H, SPELL_INVIS_PRE_VISUAL_25H));
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                    me->SetDisplayId(me->GetNativeDisplayId());
                    DoCast(me, SPELL_BLAZING_SPEED_AURA, true);
                    events.ScheduleEvent(EVENT_CONTINUE, 2000);
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->FindNearestCreature(NPC_SULFURAS_HAND_OF_RAGNAROS_1, 1.0f))
                    if (!bDespawn)
                    {
                        bDespawn = true;
                        DoCastAOE(SPELL_SUPERNOVA);
                        me->DespawnOrUnsummon(500);
                    }

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())   
                    if (Creature* pSulfuras = me->FindNearestCreature(NPC_SULFURAS_HAND_OF_RAGNAROS_1, 300.0f))
                        me->GetMotionMaster()->MoveFollow(pSulfuras, 0.0f, 0.0f);
            }
        private:
            EventMap events;
            bool bDespawn;
        };
};

class npc_ragnaros_firelands_molten_seed_caster : public CreatureScript
{
    public:
        npc_ragnaros_firelands_molten_seed_caster() : CreatureScript("npc_ragnaros_firelands_molten_seed_caster") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_molten_seed_casterAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_molten_seed_casterAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_molten_seed_casterAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void SpellHit(Unit* who, const SpellInfo* spellInfo)
            {
                if (who && (spellInfo->Id == SPELL_MOLTEN_SEED_AOE_2))
                    who->CastSpell(who, SPELL_MOLTEN_SEED_MISSILE, true);
            }
        };
};

class npc_ragnaros_firelands_molten_elemental : public CreatureScript
{
    public:
        npc_ragnaros_firelands_molten_elemental() : CreatureScript("npc_ragnaros_firelands_molten_elemental") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_molten_elementalAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_molten_elementalAI : public ScriptedAI
        {
            npc_ragnaros_firelands_molten_elementalAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                bHit = false;
                bStart = false;
                pInstance = me->GetInstanceScript();
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                if (Creature* pCaster = me->FindNearestCreature(NPC_MOLTEN_SEED_CASTER, 500.0f))
                    me->CastSpell(pCaster, SPELL_MOLTEN_SEED_AOE_2, true);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->SetDisplayId(MODEL_INVIS);
                me->CastSpell(me, SPELL_HIT_ME, true);
                DoZoneInCombat();
            }

            void SpellHit(Unit* /*who*/, const SpellInfo* spellInfo)
            {
                if (bHit)
                    return;

                if (spellInfo->Id == RAID_MODE(SPELL_MOLTEN_SEED_DMG, SPELL_MOLTEN_SEED_DMG_25, SPELL_MOLTEN_SEED_DMG_10H, SPELL_MOLTEN_SEED_DMG_25H))
                {
                    bHit = true;
                    DoCast(me, SPELL_MOLTEN_SEED_VISUAL, true);
                    events.ScheduleEvent(EVENT_CONTINUE, 10000);
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(const uint32 diff)
            {
                if (bStart)
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent()) 
                {
                    DoCastAOE(SPELL_MOLTEN_INFERNO);
                    me->RemoveAura(SPELL_MOLTEN_SEED_VISUAL);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetDisplayId(me->GetNativeDisplayId());
                    me->SetReactState(REACT_AGGRESSIVE);
                    
                    if (pInstance)
                        if (Creature* pRagnaros = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_RAGNAROS)))
                            if (Unit* pTarget = pRagnaros->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                AttackStart(pTarget);
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            bool bHit;
            bool bStart;
            InstanceScript* pInstance;
        };
};

class npc_ragnaros_firelands_lava_scion : public CreatureScript
{
    public:
        npc_ragnaros_firelands_lava_scion() : CreatureScript("npc_ragnaros_firelands_lava_scion") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_lava_scionAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_lava_scionAI : public ScriptedAI
        {
            npc_ragnaros_firelands_lava_scionAI(Creature* pCreature) : ScriptedAI(pCreature)
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
            }

            void Reset()
            {
                events.Reset();
                me->SetReactState(REACT_PASSIVE);
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_CONTINUE, 2000);
                events.ScheduleEvent(EVENT_BLAZING_HEAT, urand(7000, 13000));
                if (IsHeroic())
                    DoCast(me, SPELL_MOLTEN_POWER, true);
            }

            void JustDied(Unit* /*killer*/)
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent()) 
                {
                    switch (eventId)
                    {
                        case EVENT_CONTINUE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            break;
                        case EVENT_BLAZING_HEAT:
                        {
                            std::list<Creature*> lavascions;
                            GetCreatureListWithEntryInGrid(lavascions, me, NPC_LAVA_SCION, 500.0f);

                            std::vector<uint64> guids;

                            if (!lavascions.empty())
                            {
                                for (std::list<Creature*>::const_iterator itr = lavascions.begin(); itr != lavascions.end(); ++itr)
                                    if (Creature* scion = (*itr)->ToCreature())
                                        if (scion->getVictim())
                                            guids.push_back(scion->getVictim()->GetGUID());
                            }
                            
                            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, BlazingHeatSelector(guids)))
                                DoCast(pTarget, SPELL_BLAZING_HEAT);
                            events.ScheduleEvent(EVENT_BLAZING_HEAT, urand(18000, 22000));
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;

            struct BlazingHeatSelector : public std::unary_function<Unit*, bool>
            {
                public:
                    
                    BlazingHeatSelector(std::vector<uint64> _guids) : tankGUIDs(_guids) {}

                    bool operator()(Unit const* target) const
                    {
                        if (target->HasAura(SPELL_BLAZING_HEAT_AURA_1) ||
                            target->HasAura(SPELL_BLAZING_HEAT_AURA_1_25) ||
                            target->HasAura(SPELL_BLAZING_HEAT_AURA_1_10H) ||
                            target->HasAura(SPELL_BLAZING_HEAT_AURA_1_25H))
                            return false;

                        if (!tankGUIDs.empty())
                            for (std::vector<uint64>::const_iterator itr = tankGUIDs.begin(); itr != tankGUIDs.end(); ++itr)
                                if (target->GetGUID() == (*itr))
                                    return false;
                               

                        return true;
                    }
            private:
                std::vector<uint64> tankGUIDs;
            };
        };
};

class npc_ragnaros_firelands_blazing_heat : public CreatureScript
{
    public:
        npc_ragnaros_firelands_blazing_heat() : CreatureScript("npc_ragnaros_firelands_blazing_heat") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_blazing_heatAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_blazing_heatAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_blazing_heatAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                DoZoneInCombat();
            }
        };
};

class npc_ragnaros_firelands_living_meteor : public CreatureScript
{
    public:
        npc_ragnaros_firelands_living_meteor() : CreatureScript("npc_ragnaros_firelands_living_meteor") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_living_meteorAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_living_meteorAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_living_meteorAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
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
                me->SetReactState(REACT_PASSIVE);
                pInstance = me->GetInstanceScript();
                me->SetSpeed(MOVE_RUN, 0.6f, true);
            }

            void Reset()
            {
                bExplode = false;
            }

            void SpellHit(Unit* who, const SpellInfo* spellInfo)
            {
                if (who->GetEntry() == NPC_BREADTH_OF_FROST && spellInfo->Id == SPELL_BREADTH_OF_FROST_DMG)
                {
                    who->ToCreature()->DespawnOrUnsummon();
                    me->StopMoving();
                    me->GetMotionMaster()->MovementExpired(false);
                    me->RemoveAura(SPELL_LIVING_METEOR_DMG_REDUCTION);
                    me->RemoveAura(RAID_MODE(SPELL_COMBUSTIBLE, SPELL_COMBUSTIBLE_25, SPELL_COMBUSTIBLE_10H, SPELL_COMBUSTIBLE_25H));
                    me->RemoveAura(RAID_MODE(SPELL_LIVING_METEOR_AURA_2, SPELL_LIVING_METEOR_AURA_2_25));
                    me->RemoveAura(RAID_MODE(SPELL_LIVING_METEOR_SPEED, SPELL_LIVING_METEOR_SPEED_25));
                    me->RemoveAura(RAID_MODE(SPELL_LIVING_METEOR_AURA_1, SPELL_LIVING_METEOR_AURA_1_25, SPELL_LIVING_METEOR_AURA_1_10H, SPELL_LIVING_METEOR_AURA_1_25H));
                    events.ScheduleEvent(EVENT_CONTINUE_METEOR, 11000);
                }
                else if (who->GetEntry() == NPC_LAVA_WAVE && (spellInfo->Id == SPELL_LAVALOGGED_10 || spellInfo->Id == SPELL_LAVALOGGED_25))
                {
                    if (pInstance)
                        if (Creature* pRagnaros = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_RAGNAROS)))
                            pRagnaros->AI()->DoAction(ACTION_LAVALOGGED);
                }
            }

            void EnterCombat(Unit* /*who*/)
            {
                DoCast(me, SPELL_LIVING_METEOR_DMG_REDUCTION, true);
                DoCast(me, SPELL_LIVING_METEOR_AURA_1, true);
                DoCast(me, SPELL_COMBUSTIBLE, true);
                events.ScheduleEvent(EVENT_FIXATE, 2000);
            }

            void DoAction(const int32 action)
            {
                if (action == ACTION_COMBUSTION)
                {
                    me->RemoveAura(RAID_MODE(SPELL_COMBUSTIBLE, SPELL_COMBUSTIBLE_25, SPELL_COMBUSTIBLE_10H, SPELL_COMBUSTIBLE_25H));
                    me->RemoveAura(RAID_MODE(SPELL_LIVING_METEOR_AURA_2, SPELL_LIVING_METEOR_AURA_2_25));
                    me->RemoveAura(RAID_MODE(SPELL_LIVING_METEOR_SPEED, SPELL_LIVING_METEOR_SPEED_25));
                    events.ScheduleEvent(EVENT_FIXATE, 2000);
                    events.ScheduleEvent(EVENT_COMBUSTIBLE, 5000);
                }
                else if (action == ACTION_EXPLODE)
                {
                    if (!bExplode)
                    {
                        bExplode = true;
                        me->GetMotionMaster()->MovementExpired(false);
                        me->StopMoving();
                        DoCastAOE(SPELL_METEOR_IMPACT);
                        events.ScheduleEvent(EVENT_FIXATE, 2000);
                    }
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIXATE:
                            if (pInstance)
                                if (Creature* pRagnaros = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_RAGNAROS)))
                                {
                                    Unit* pTarget = NULL;
                                    pTarget = pRagnaros->AI()->SelectTarget(SELECT_TARGET_RANDOM, 2, 0.0f, true);
                                    if (!pTarget)
                                        pTarget = pRagnaros->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
                                    if (pTarget)
                                    {
                                        bExplode = false;
                                        DoCast(me, SPELL_LIVING_METEOR_AURA_2, true);
                                        DoCast(pTarget, SPELL_FIXATE);
                                        me->ClearUnitState(UNIT_STATE_CASTING);
                                        me->SetSpeed(MOVE_RUN, 0.6f, true);
                                        me->GetMotionMaster()->MoveFollow(pTarget, 0.0f, 0.0f);
                                    }
                                }
                            break;
                        case EVENT_COMBUSTIBLE:
                            DoCast(me, SPELL_COMBUSTIBLE, true);
                            break;
                        case EVENT_CONTINUE_METEOR:
                            DoCast(me, SPELL_LIVING_METEOR_DMG_REDUCTION, true);
                            DoCast(me, SPELL_LIVING_METEOR_AURA_1, true);
                            DoCast(me, SPELL_COMBUSTIBLE, true);
                            events.ScheduleEvent(EVENT_FIXATE, 2000);
                            break;
                    }
                }

            }
        private:
            EventMap events;
            bool bExplode;
            InstanceScript* pInstance;
        };
};

class npc_ragnaros_firelands_cenarius : public CreatureScript
{
    public:
        npc_ragnaros_firelands_cenarius() : CreatureScript("npc_ragnaros_firelands_cenarius") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_cenariusAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_cenariusAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_cenariusAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                pInstance = me->GetInstanceScript();
            }

            void Reset()
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_EVENT_1, 1000);
                events.ScheduleEvent(EVENT_BREADTH_OF_FROST, 31000);
            }

            void DoAction(const int32 action)
            {
                if (action == ACTION_WIN)
                {
                    me->InterruptNonMeleeSpells(false);
                    events.Reset();
                    events.ScheduleEvent(EVENT_EVENT_WIN_1, 15000);
                }
            }

            void JustSummoned(Creature* summon)
            {
                if (summon->GetEntry() == NPC_BREADTH_OF_FROST)
                    DoCast(summon, SPELL_BREADTH_OF_FROST);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!pInstance)
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent()) 
                {
                    switch (eventId)
                    {
                        case EVENT_EVENT_1:
                            Talk(SAY_CENARIUS_EVENT_1);
                            events.ScheduleEvent(EVENT_DRAW_OUT, 2000);
                            break;
                        case EVENT_DRAW_OUT:
                            DoCast(me, SPELL_DRAW_OUT_FIRELORD_3);
                            if (Creature* pMalfurion = me->FindNearestCreature(NPC_MALFURION_STORMRAGE, 100.0f))
                                pMalfurion->CastSpell(pMalfurion, SPELL_DRAW_OUT_FIRELORD_1);
                            if (Creature* pHamuul = me->FindNearestCreature(NPC_HAMUUL_RUNETOTEM, 100.0f))
                                pHamuul->CastSpell(pHamuul, SPELL_DRAW_OUT_FIRELORD_2);
                            events.ScheduleEvent(EVENT_RAGNAROS_UP, 15000);
                            break;
                        case EVENT_RAGNAROS_UP:
                            if (Creature* p_Ragnaros = me->FindNearestCreature(NPC_RAGNAROS, 200.0f))
                            {
                                if (p_Ragnaros->IsAIEnabled)
                                {
                                    p_Ragnaros->AI()->DoAction(ACTION_RAGNAROS_UP);
                                    p_Ragnaros->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                                    p_Ragnaros->RemoveAura(SPELL_SUBMERGE_AURA_25H);
                                    p_Ragnaros->SetReactState(REACT_AGGRESSIVE);
                                    p_Ragnaros->Attack(p_Ragnaros->getVictim(), false);
                                }
                            }
                            break;
                        case EVENT_BREADTH_OF_FROST:
                        {
                            uint32 i = urand(0, 275);
                            me->CastSpell(floorPos[i].GetPositionX(), floorPos[i].GetPositionY(), floorPos[i].GetPositionZ(), SPELL_SUMMON_BREADTH_OF_FROST, true);
                            events.ScheduleEvent(EVENT_BREADTH_OF_FROST, 45000);
                            break;
                        }
                        case EVENT_EVENT_WIN_1:
                            Talk(SAY_CENARIUS_EVENT_2);
                            events.ScheduleEvent(EVENT_EVENT_WIN_2, 15000);
                            break;
                        case EVENT_EVENT_WIN_2:
                            Talk(SAY_CENARIUS_EVENT_3);
                            events.ScheduleEvent(EVENT_EVENT_WIN_3, 20000);
                            break;
                        case EVENT_EVENT_WIN_3:
                            Talk(SAY_CENARIUS_EVENT_4);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap events;
            InstanceScript* pInstance;
        };
};

class npc_ragnaros_firelands_malfurion_stormrage : public CreatureScript
{
    public:
        npc_ragnaros_firelands_malfurion_stormrage() : CreatureScript("npc_ragnaros_firelands_malfurion_stormrage") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_malfurion_stormrageAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_malfurion_stormrageAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_malfurion_stormrageAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                pInstance = me->GetInstanceScript();
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                DoCast(me, SPELL_TRANSFORM_MALFURION, true);
                events.ScheduleEvent(EVENT_CLOUDBURST, 48000);
            }

            void DoAction(const int32 action)
            {
                if (action == ACTION_WIN)
                {
                    me->InterruptNonMeleeSpells(false);
                    events.Reset();
                    events.ScheduleEvent(EVENT_EVENT_WIN_1, 10000);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!pInstance)
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent()) 
                {
                    switch (eventId)
                    {
                        case EVENT_CLOUDBURST:
                        {
                            uint32 i = urand(0, MAX_FLOOR_POS - 1);
                            me->CastSpell(floorPos[i].GetPositionX(), floorPos[i].GetPositionY(), floorPos[i].GetPositionZ(), SPELL_CLOUDBURST_MISSILE, true);
                            break;
                        }
                        case EVENT_EVENT_WIN_1:
                            Talk(SAY_MALFURION_EVENT_2);
                            events.ScheduleEvent(EVENT_EVENT_WIN_2, 10000);
                            break;
                        case EVENT_EVENT_WIN_2:
                            Talk(SAY_MALFURION_EVENT_3);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap events;
            InstanceScript* pInstance;
        };
};

class npc_ragnaros_firelands_hamuul_runetotem : public CreatureScript
{
    public:
        npc_ragnaros_firelands_hamuul_runetotem() : CreatureScript("npc_ragnaros_firelands_hamuul_runetotem") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_hamuul_runetotemAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_hamuul_runetotemAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_hamuul_runetotemAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                pInstance = me->GetInstanceScript();
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                DoCast(me, SPELL_TRANSFORM_HAMUUL, true);
                events.ScheduleEvent(EVENT_ENTRAPPING_ROOTS, 55000);
            }

            void JustSummoned(Creature* summon)
            {
                if (summon->GetEntry() == NPC_ENTRAPPING_ROOTS)
                    DoCast(summon, SPELL_ENTRAPPING_ROOTS_MISSILE);
            }

            void DoAction(const int32 action)
            {
                if (action == ACTION_WIN)
                {
                    me->InterruptNonMeleeSpells(false);
                    events.Reset();
                    events.ScheduleEvent(EVENT_EVENT_WIN_1, 35000);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!pInstance)
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent()) 
                {
                    switch (eventId)
                    { 
                        case EVENT_ENTRAPPING_ROOTS:
                        {
                            uint32 i = urand(0, 275);
                            me->CastSpell(floorPos[i].GetPositionX(), floorPos[i].GetPositionY(), floorPos[i].GetPositionZ(), SPELL_SUMMON_ENTRAPPING_ROOTS, true);
                            if (Creature* pRagnaros = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_RAGNAROS)))
                                pRagnaros->AI()->DoAction(ACTION_EMPOWER_SULFURAS);
                            events.ScheduleEvent(EVENT_ENTRAPPING_ROOTS, 55000);
                            break;
                        }
                        case EVENT_EVENT_WIN_1:
                            Talk(SAY_HAMUUL_EVENT_1);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap events;
            InstanceScript* pInstance;
        };
};

class npc_ragnaros_firelands_dreadflame_spawn : public CreatureScript
{
    public:
        npc_ragnaros_firelands_dreadflame_spawn() : CreatureScript("npc_ragnaros_firelands_dreadflame_spawn") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_dreadflame_spawnAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_dreadflame_spawnAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_dreadflame_spawnAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_DREADFLAME_UPDATE, 1000);
                events.ScheduleEvent(EVENT_CHECK_CLOUDBURST, 1000);
            }

            void JustSummoned(Creature* summon)
            {
                if (me->isInCombat())
                    DoZoneInCombat(summon);
            }

            void UpdateAI(const uint32 diff)
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DREADFLAME_UPDATE:
                        {
                            uint32 i = GetNextDreadflame();
                            bFloor[i] = true;
                            //me->CastSpell(floorPos[i].GetPositionX(), floorPos[i].GetPositionY(), floorPos[i].GetPositionZ(), SPELL_DREADFLAME_SUMMON, true);
                            if (Creature* pDreadflame = me->SummonCreature(NPC_DREADFLAME, floorPos[i], TEMPSUMMON_TIMED_DESPAWN, 600000))
                                pDreadflame->AI()->SetData(DATA_DREADFLAME_ID, i + 1);    
                            events.ScheduleEvent(EVENT_DREADFLAME_UPDATE, 5000);
                            break;
                        }
                        case EVENT_CHECK_CLOUDBURST:
                        {
                            UnitList targets;
                            CloudburstCheck check(me);
                            JadeCore::UnitListSearcher<CloudburstCheck> searcher(me, targets, check);
                            me->VisitNearbyObject(3.0f, searcher);
                            if (!targets.empty())
                            {
                                events.Reset();
                                me->DespawnOrUnsummon();
                            }
                            else
                                events.ScheduleEvent(EVENT_CHECK_CLOUDBURST, 1000);
                            break;
                        }
                    }
                }
            }

        private:
            EventMap events;

            uint32 GetNextDreadflame()
            {
                Position myPos;
                me->GetPosition(&myPos);
                float f_dist = myPos.GetExactDist(&floorPos[0]);
                uint32 iFound = 0;
                for (uint32 i = 0; i < MAX_FLOOR_POS; ++i)
                {
                    float t_dist = myPos.GetExactDist(&floorPos[i]);
                    if (t_dist < f_dist && !bFloor[i])
                    {
                        f_dist = t_dist;
                        iFound = i;
                    }
                }
                return iFound;
            }
        };
};

class npc_ragnaros_firelands_dreadflame : public CreatureScript
{
    public:
        npc_ragnaros_firelands_dreadflame() : CreatureScript("npc_ragnaros_firelands_dreadflame") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_dreadflameAI(pCreature);
        }
            
        struct npc_ragnaros_firelands_dreadflameAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_dreadflameAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                _id = 0;
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                DoCast(me, SPELL_DREADFLAME_AURA, true);
                events.ScheduleEvent(EVENT_CHECK_CLOUDBURST, 1000);
                events.ScheduleEvent(EVENT_CHECK_TARGET, 1000);
            }

            void SetData(uint32 type, uint32 data)
            {
                if (type == DATA_DREADFLAME_ID)
                    _id = data;
            }

            void UpdateAI(const uint32 diff)
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_TARGET:
                        {
                            std::list<Player*> targets;
                            JadeCore::AnyPlayerInObjectRangeCheck check(me, 3.0f, true);
                            JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> searcher(me, targets, check);
                            me->VisitNearbyObject(3.0f, searcher);
                            if (!targets.empty())
                            {
                                for (std::list<Player*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                    if (Player* plr = (*itr))
                                        if (!plr->HasAura(Is25ManRaid() ? SPELL_DELUGE_AURA_25H : SPELL_DELUGE_AURA_10H))
                                            DoCast(plr, SPELL_DREADFLAME_DMG, true);
                            }
                            
                            events.ScheduleEvent(EVENT_CHECK_TARGET, 1000);
                            break;
                        }
                        case EVENT_CHECK_CLOUDBURST:
                        {
                            UnitList targets;
                            CloudburstCheck check(me);
                            JadeCore::UnitListSearcher<CloudburstCheck> searcher(me, targets, check);
                            me->VisitNearbyObject(3.0f, searcher);
                            if (!targets.empty())
                            {
                                events.Reset();
                                if (_id)
                                    bFloor[_id - 1] = false;
                                me->RemoveAllDynObjects();
                                me->DespawnOrUnsummon();
                            }
                            else
                                events.ScheduleEvent(EVENT_CHECK_CLOUDBURST, 1000);
                            break;
                        }
                    }
                }
            }
        private:
            EventMap events;
            uint32 _id;
        };
};

class npc_ragnaros_firelands_cloudburst : public CreatureScript
{
    public:
        npc_ragnaros_firelands_cloudburst() : CreatureScript("npc_ragnaros_firelands_cloudburst") {}

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_ragnaros_firelands_cloudburstAI(pCreature);
        }

        bool OnGossipHello(Player* pPlayer, Creature* pCreature)
        {
            uint32 clicks = pCreature->AI()->GetData(DATA_CLICKS);
            if (!clicks)
                return true;

            uint32 spellId = (pCreature->GetMap()->Is25ManRaid() ? SPELL_DELUGE_AURA_25H : SPELL_DELUGE_AURA_10H); 
            if (pPlayer->HasAura(spellId))
                return true;

            pPlayer->CastSpell(pPlayer, spellId, true);

            clicks--;

            if (!clicks)
                pCreature->DespawnOrUnsummon();
            else
                pCreature->AI()->SetData(DATA_CLICKS, clicks);

            return true;
        }
    
        struct npc_ragnaros_firelands_cloudburstAI : public Scripted_NoMovementAI
        {
            npc_ragnaros_firelands_cloudburstAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
            {
                me->SetReactState(REACT_PASSIVE);
                _clicks = (pCreature->GetMap()->Is25ManRaid() ? 3 : 1);
            }

            void SetData(uint32 type, uint32 data)
            {
                if (type == DATA_CLICKS)
                    _clicks = data;
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_CLICKS)
                    return _clicks;
                return 0;
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                DoCast(me, SPELL_CLOUDBURST_DUMMY, true);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

        private:
            uint32 _clicks;
        };
};

class spell_ragnaros_firelands_wrath_of_ragnaros_aoe : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_wrath_of_ragnaros_aoe() : SpellScriptLoader("spell_ragnaros_firelands_wrath_of_ragnaros_aoe") { }

        class spell_ragnaros_firelands_wrath_of_ragnaros_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_wrath_of_ragnaros_aoe_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                targets.remove_if(DistanceCheck(GetCaster()));

                if (!targets.empty())
                {
                    uint32 max_size = (GetCaster()->GetMap()->Is25ManRaid() ? 3 : 1);
                    JadeCore::Containers::RandomResizeList(targets, max_size);
                }
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_WRATH_OF_RAGNAROS_DMG, true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ragnaros_firelands_wrath_of_ragnaros_aoe_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_ragnaros_firelands_wrath_of_ragnaros_aoe_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        private:
            class DistanceCheck
            {
                public:
                    DistanceCheck(Unit* searcher) : _searcher(searcher) {}
            
                    bool operator()(WorldObject* unit)
                    {
                        return (!unit->ToUnit() || _searcher->GetDistance(unit) < 18.0f);
                    }

                private:
                    Unit* _searcher;
            };
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_wrath_of_ragnaros_aoe_SpellScript();
        }
};

class spell_ragnaros_firelands_magma_trap_aoe : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_magma_trap_aoe() : SpellScriptLoader("spell_ragnaros_firelands_magma_trap_aoe") { }

        class spell_ragnaros_firelands_magma_trap_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_magma_trap_aoe_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                std::list<WorldObject*> tempList;

                for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if (GetCaster()->GetDistance2d((*itr)) >= 25.0f)
                        tempList.push_back((*itr));

                if (!tempList.empty())
                {
                    targets.clear();
                    targets.push_back(JadeCore::Containers::SelectRandomContainerElement(tempList));
                }
                else
                    JadeCore::Containers::RandomResizeList(targets, 1);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGMA_TRAP, true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ragnaros_firelands_magma_trap_aoe_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_ragnaros_firelands_magma_trap_aoe_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_magma_trap_aoe_SpellScript();
        }
};

class spell_ragnaros_firelands_sulfuras_smash_aoe : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_sulfuras_smash_aoe() : SpellScriptLoader("spell_ragnaros_firelands_sulfuras_smash_aoe") { }

        class spell_ragnaros_firelands_sulfuras_smash_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_sulfuras_smash_aoe_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                std::list<WorldObject*> tempList;

                for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if (GetCaster()->GetDistance2d((*itr)) >= 25.0f)
                        tempList.push_back((*itr));

                if (!tempList.empty())
                {
                    targets.clear();
                    targets.push_back(JadeCore::Containers::SelectRandomContainerElement(tempList));
                }
                else
                    JadeCore::Containers::RandomResizeList(targets, 1);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ragnaros_firelands_sulfuras_smash_aoe_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_sulfuras_smash_aoe_SpellScript();
        }
};

class spell_ragnaros_firelands_sulfuras_smash : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_sulfuras_smash() : SpellScriptLoader("spell_ragnaros_firelands_sulfuras_smash") { }


        class spell_ragnaros_firelands_sulfuras_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_sulfuras_smash_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                if (GetCaster()->GetAI())
                    GetCaster()->GetAI()->DoAction(ACTION_LAVA_WAVE);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_ragnaros_firelands_sulfuras_smash_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_sulfuras_smash_SpellScript();
        }
};

class spell_ragnaros_firelands_splitting_blow : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_splitting_blow() : SpellScriptLoader("spell_ragnaros_firelands_splitting_blow") { }


        class spell_ragnaros_firelands_splitting_blow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_splitting_blow_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                if (GetCaster()->GetAI())
                    GetCaster()->GetAI()->DoAction(ACTION_SPLITTING_BLOW);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_ragnaros_firelands_splitting_blow_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_splitting_blow_SpellScript();
        }
};

class spell_ragnaros_firelands_invoke_sons_aoe_1 : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_invoke_sons_aoe_1() : SpellScriptLoader("spell_ragnaros_firelands_invoke_sons_aoe_1") { }


        class spell_ragnaros_firelands_invoke_sons_aoe_1_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_invoke_sons_aoe_1_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_INVOKE_SONS_MISSILE, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_ragnaros_firelands_invoke_sons_aoe_1_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_invoke_sons_aoe_1_SpellScript();
        }
};

class spell_ragnaros_firelands_blazing_speed : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_blazing_speed() : SpellScriptLoader("spell_ragnaros_firelands_blazing_speed") { }

        class spell_ragnaros_firelands_blazing_speed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ragnaros_firelands_blazing_speed_AuraScript);

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (!GetCaster())
                    return;

                uint32 spell_id = 0;
                switch (GetCaster()->GetMap()->GetDifficultyID())
                {
                    case Difficulty::Difficulty10N:
                        spell_id = SPELL_BLAZING_SPEED;
                        break;
                    case Difficulty::Difficulty10HC :
                        spell_id = SPELL_BLAZING_SPEED_25;
                        break;
                    case Difficulty::Difficulty25N:
                        spell_id = SPELL_BLAZING_SPEED_10H;
                        break;
                    case Difficulty::Difficulty25HC:
                        spell_id = SPELL_BLAZING_SPEED_25H;
                        break;
                    default:
                        break;
                }

                if (!spell_id)
                    return;

                float health_pct = GetCaster()->GetHealthPct();

                if (health_pct <= 50.0f)
                {
                    if (GetCaster()->HasAura(spell_id))
                        GetCaster()->RemoveAura(spell_id);
                }
                else
                {
                    uint8 stacks = uint8(0.2f * (health_pct - 50.0f));
                    if (Aura* aur = GetCaster()->GetAura(spell_id))
                        aur->SetStackAmount(stacks);
                    else
                        GetCaster()->CastCustomSpell(spell_id, SPELLVALUE_AURA_STACK, int32(stacks), GetCaster(), true);
                    GetCaster()->RemoveMovementImpairingAuras();
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ragnaros_firelands_blazing_speed_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ragnaros_firelands_blazing_speed_AuraScript();
        }
};

class spell_ragnaros_firelands_engulfing_flames : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_engulfing_flames() : SpellScriptLoader("spell_ragnaros_firelands_engulfing_flames") { }

        class spell_ragnaros_firelands_engulfing_flames_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_engulfing_flames_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (targets.empty())
                    return;

                switch (GetSpellInfo()->Id)
                {
                    case 99172:
                    case 100175:
                    case 100176:
                    case 100177:
                    case 99216:
                        targets.remove_if(NearCheck(GetCaster()));
                        break;
                    case 99235:
                    case 100178:
                    case 100179:
                    case 100180:
                    case 99217:
                        targets.remove_if(MidCheck(GetCaster()));
                        break;
                    case 99236:
                    case 100181:
                    case 100182:
                    case 100183:
                    case 99218:
                        targets.remove_if(FarCheck(GetCaster()));
                        break;
                }

            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ragnaros_firelands_engulfing_flames_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_SRC_AREA_ENTRY);
            }
        private:
            class NearCheck
            {
                public:
                    NearCheck(Unit* searcher) : _searcher(searcher) {}
            
                    bool operator()(WorldObject* unit)
                    {
                        return (!unit->ToUnit() || _searcher->GetDistance(unit) >= 1.0f);
                    }

                private:
                    Unit* _searcher;
            };

            class MidCheck
            {
                public:
                    MidCheck(Unit* searcher) : _searcher(searcher) {}
            
                    bool operator()(WorldObject* unit)
                    {
                        return (!unit->ToUnit() || _searcher->GetDistance(unit) < 1.0f || _searcher->GetDistance(unit) > 12.0f);
                    }

                private:
                    Unit* _searcher;
            };

            class FarCheck
            {
                public:
                    FarCheck(Unit* searcher) : _searcher(searcher) {}
            
                    bool operator()(WorldObject* unit)
                    {
                        return (!unit->ToUnit() || _searcher->GetDistance(unit) < 12.0f);
                    }

                private:
                    Unit* _searcher;
            };
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_engulfing_flames_SpellScript();
        }
};

class spell_ragnaros_firelands_blazing_heat_dummy : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_blazing_heat_dummy() : SpellScriptLoader("spell_ragnaros_firelands_blazing_heat_dummy") { }


        class spell_ragnaros_firelands_blazing_heat_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_blazing_heat_dummy_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                if (!GetCaster()->FindNearestCreature(NPC_BLAZING_HEAT, 3.0f))
                    GetCaster()->CastSpell(GetCaster(), SPELL_BLAZING_HEAT_SUMMON, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_ragnaros_firelands_blazing_heat_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_blazing_heat_dummy_SpellScript();
        }
};

class spell_ragnaros_firelands_engulfing_flame_script : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_engulfing_flame_script() : SpellScriptLoader("spell_ragnaros_firelands_engulfing_flame_script") { }


        class spell_ragnaros_firelands_engulfing_flame_script_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_engulfing_flame_script_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                switch (urand(0, 2))
                {
                    case 0:
                        GetCaster()->CastSpell(GetCaster(), SPELL_ENGULFING_FLAMES_NEAR_VISUAL, true);
                        GetCaster()->CastSpell(GetCaster(), SPELL_ENGULFING_FLAMES_NEAR);
                        break;
                    case 1:
                        GetCaster()->CastSpell(GetCaster(), SPELL_ENGULFING_FLAMES_MID_VISUAL, true);
                        GetCaster()->CastSpell(GetCaster(), SPELL_ENGULFING_FLAMES_MID);
                        break;
                    case 2:
                        GetCaster()->CastSpell(GetCaster(), SPELL_ENGULFING_FLAMES_FAR_VISUAL, true);
                        GetCaster()->CastSpell(GetCaster(), SPELL_ENGULFING_FLAMES_FAR);
                        break;
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_ragnaros_firelands_engulfing_flame_script_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_engulfing_flame_script_SpellScript();
        }
};

class spell_ragnaros_firelands_world_in_flame : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_world_in_flame() : SpellScriptLoader("spell_ragnaros_firelands_world_in_flame") { }

        class spell_ragnaros_firelands_world_in_flame_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ragnaros_firelands_world_in_flame_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* pCaster = GetCaster()->ToCreature())
                    pCaster->AI()->DoAction(ACTION_WORLD_IN_FLAME_OFF);
            }
            
            void Register()
            {
                OnEffectRemove += AuraEffectApplyFn(spell_ragnaros_firelands_world_in_flame_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ragnaros_firelands_world_in_flame_AuraScript();
        }
};

class spell_ragnaros_firelands_living_meteor_aoe_1 : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_living_meteor_aoe_1() : SpellScriptLoader("spell_ragnaros_firelands_living_meteor_aoe_1") { }

        class spell_ragnaros_firelands_living_meteor_aoe_1_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_living_meteor_aoe_1_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetCaster()->CastSpell(GetHitUnit(), SPELL_LIVING_METEOR_MISSILE, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_ragnaros_firelands_living_meteor_aoe_1_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_living_meteor_aoe_1_SpellScript();
        }
};

class spell_ragnaros_firelands_living_meteor_aoe_2 : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_living_meteor_aoe_2() : SpellScriptLoader("spell_ragnaros_firelands_living_meteor_aoe_2") { }

        class spell_ragnaros_firelands_living_meteor_aoe_2_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_living_meteor_aoe_2_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (Creature* pMeteor = GetCaster()->ToCreature())
                    pMeteor->AI()->DoAction(ACTION_EXPLODE);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_ragnaros_firelands_living_meteor_aoe_2_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_living_meteor_aoe_2_SpellScript();
        }
};

class spell_ragnaros_firelands_breadth_of_frost_script : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_breadth_of_frost_script() : SpellScriptLoader("spell_ragnaros_firelands_breadth_of_frost_script") { }

        class spell_ragnaros_firelands_breadth_of_frost_script_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_breadth_of_frost_script_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetHitUnit()->RemoveAura(GetHitUnit()->GetMap()->Is25ManRaid() ? SPELL_SUPERHEATED_DMG_25H : SPELL_SUPERHEATED_DMG_10H);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_ragnaros_firelands_breadth_of_frost_script_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_breadth_of_frost_script_SpellScript();
        }
};

class spell_ragnaros_firelands_superheated_dmg : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_superheated_dmg() : SpellScriptLoader("spell_ragnaros_firelands_superheated_dmg") { }

        class spell_ragnaros_firelands_superheated_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_superheated_dmg_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (!targets.empty())
                    targets.remove_if(BreadthCloudburstCheck());
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ragnaros_firelands_superheated_dmg_SpellScript::FilterTargets, EFFECT_0,TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ragnaros_firelands_superheated_dmg_SpellScript::FilterTargets, EFFECT_1,TARGET_UNIT_SRC_AREA_ENEMY);
            }
        private:
            class BreadthCloudburstCheck
            {
                public:
                    BreadthCloudburstCheck() {}
            
                    bool operator()(WorldObject* unit)
                    {
                        return (!unit->ToUnit() || unit->ToUnit()->HasAura(SPELL_BREADTH_OF_FROST_ALLY) || unit->ToUnit()->HasAura(unit->GetMap()->Is25ManRaid() ? SPELL_DELUGE_AURA_25H : SPELL_DELUGE_AURA_10H));
                    }

                private:
                    Unit* _searcher;
            };
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_superheated_dmg_SpellScript();
        }
};

class spell_ragnaros_firelands_rage_of_ragnaros_aoe : public SpellScriptLoader
{
    public:
        spell_ragnaros_firelands_rage_of_ragnaros_aoe() : SpellScriptLoader("spell_ragnaros_firelands_rage_of_ragnaros_aoe") { }

        class spell_ragnaros_firelands_rage_of_ragnaros_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ragnaros_firelands_rage_of_ragnaros_aoe_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (Player* plr = GetHitUnit()->ToPlayer())
                {
                    uint32 l_ObjectiveID = 0;
                    if (plr->GetQuestStatus(QUEST_HEART_OF_FLAME_HORDE) == QUEST_STATUS_INCOMPLETE)
                        l_ObjectiveID = QUEST_OBJECTIVE_HEART_OF_FLAME_HORDE;
                    else if (plr->GetQuestStatus(QUEST_HEART_OF_FLAME_ALLIANCE) == QUEST_STATUS_INCOMPLETE)
                        l_ObjectiveID = QUEST_OBJECTIVE_HEART_OF_FLAME_ALLIANCE;

                    if (l_ObjectiveID)
                    {
                        if (plr->GetQuestObjectiveCounter(l_ObjectiveID) >= 250)
                            GetCaster()->CastSpell(plr, SPELL_RAGE_OF_RAGNAROS, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_ragnaros_firelands_rage_of_ragnaros_aoe_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ragnaros_firelands_rage_of_ragnaros_aoe_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_ragnaros_firelands()
{
    new boss_ragnaros_firelands();
    new npc_ragnaros_firelands_sulfuras_smash();
    new npc_ragnaros_firelands_lava_wave();
    new npc_ragnaros_firelands_magma_trap();
    new npc_ragnaros_firelands_son_of_flame();
    new npc_ragnaros_firelands_splitting_blow();
    new npc_ragnaros_firelands_sulfuras_hand_of_ragnaros();
    new npc_ragnaros_firelands_molten_seed_caster();
    new npc_ragnaros_firelands_molten_elemental();
    new npc_ragnaros_firelands_lava_scion();
    new npc_ragnaros_firelands_blazing_heat();
    new npc_ragnaros_firelands_living_meteor();
    new npc_ragnaros_firelands_cenarius();
    new npc_ragnaros_firelands_malfurion_stormrage();
    new npc_ragnaros_firelands_hamuul_runetotem();
    new npc_ragnaros_firelands_dreadflame_spawn();
    new npc_ragnaros_firelands_dreadflame();
    new npc_ragnaros_firelands_cloudburst();
    new spell_ragnaros_firelands_wrath_of_ragnaros_aoe();
    new spell_ragnaros_firelands_magma_trap_aoe();
    new spell_ragnaros_firelands_sulfuras_smash_aoe();
    new spell_ragnaros_firelands_sulfuras_smash();
    new spell_ragnaros_firelands_splitting_blow();
    new spell_ragnaros_firelands_invoke_sons_aoe_1();
    new spell_ragnaros_firelands_blazing_speed();
    new spell_ragnaros_firelands_engulfing_flames();
    new spell_ragnaros_firelands_blazing_heat_dummy();
    new spell_ragnaros_firelands_engulfing_flame_script();
    new spell_ragnaros_firelands_world_in_flame();
    new spell_ragnaros_firelands_living_meteor_aoe_1();
    new spell_ragnaros_firelands_living_meteor_aoe_2();
    new spell_ragnaros_firelands_breadth_of_frost_script();
    new spell_ragnaros_firelands_superheated_dmg();
    new spell_ragnaros_firelands_rage_of_ragnaros_aoe();
}
#endif
