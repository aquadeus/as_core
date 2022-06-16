#include "throne_of_thunder.hpp"

// TODO: correct power display
// There are two power displays in dbc (215 anima and 216 vita)
// I think that those are correct, but there is no vehicle id for those powers
// So I use 2888 vehicle id (power display 219 anima) until I find the correct way

enum ScriptedTexts
{
    SAY_AGGRO           = 0,
    SAY_BERSERK         = 1,
    SAY_INTRO_1         = 2,
    SAY_INTRO_2         = 3,
    SAY_INTRO_3         = 4,
    SAY_KILL            = 5,
    SAY_PHASE_2         = 6,
    SAY_MURDEROUS       = 7,
    SAY_CREATION        = 8,
    SAY_VITA            = 9,
    SAY_ANIMA           = 10,
    SAY_END             = 11,
};

enum Spells
{
    SPELL_ZERO_MANA                     = 96301, 

    SPELL_MATERIALS_OF_CREATION         = 138321,

    SPELL_SUMMON_ESSENCE_OF_VITA        = 138324,
    SPELL_DRAW_VITA                     = 138328,
    SPELL_SUMMON_CRACKLING_STALKER      = 138339,
    SPELL_IMBUED_WITH_VITA              = 138332,
    SPELL_UNLEASHED_VITA                = 138330,
    SPELL_FATAL_STRIKE                  = 138334,
    SPELL_UNSTABLE_VITA                 = 138297,
    SPELL_UNSTABLE_VITA_ALLY            = 138308,
    SPELL_UNSTABLE_VITA_DMG             = 138370,
    SPELL_UNSTABLE_VITA_DUMMY           = 138371,
    SPELL_VITA_SENSITIVITY              = 138372,

    SPELL_SUMMON_ESSENCE_OF_ANIMA       = 138323,
    SPELL_DRAW_ANIMA                    = 138327,
    SPELL_SUMMON_SANGUINE_HORROR        = 138338,
    SPELL_IMBUED_WITH_ANIMA             = 138331,
    SPELL_MURDEROUS_STRIKE              = 138333,
    SPELL_UNSTABLE_ANIMA                = 138288,
    SPELL_UNSTABLE_ANIMA_DUMMY          = 138294,
    SPELL_UNSTABLE_ANIMA_DMG            = 138295,
    SPELL_ANIMA_SENSITIVITY             = 139318,

    SPELL_LINGERING_ENERGIES            = 138450,

    SPELL_CALL_ESSENCE                  = 139040,

    SPELL_RUIN_BOLT                     = 139087,
    SPELL_RUIN                          = 139073,
    SPELL_RUIN_DMG                      = 139074,

    // Crackling Stalker
    SPELL_CAUTERIZING_FLARE             = 138337,
    SPELL_CRACKLE                       = 138340,

    // Sanguine Horror
    SPELL_SANGUINE_VOLLEY               = 138341,

    // Corrupted Vita
    SPELL_CORRUPTED_VITA                = 139072,
    SPELL_CORRUPTED_VITA_DMG            = 139078,

    // Corrupted Anima
    SPELL_TWISTED_ANIMA                 = 139075,
    SPELL_CORRUPTED_ANIMA               = 139071,

    SPELL_RA_DEN_BONUS                  = 139692
};

enum eRadenClassSpells
{
    SPELL_PALADIN_PROTECTION = 132403,
    SPELL_WARRIOR_PROTECTION = 132404,
    SPELL_DRUID_BEAR         = 132402,
    SPELL_DK_BLOOD           = 77535,
    SPELL_MONK_BREWMASTER    = 115307
};

enum Adds
{
    NPC_ESSENCE_OF_VITA     = 69870,
    NPC_CORRUPTED_VITA      = 69958,
    NPC_CRACKLING_STALKER   = 69872,
    NPC_ESSENCE_OF_ANIMA    = 69869,
    NPC_CORRUPTED_ANIMA     = 69957,
    NPC_SANGUINE_HORROR     = 69871,
};

enum Events
{
    EVENT_CREATION  = 1,
    EVENT_ESSENCE_MOVE,
    EVENT_ESSENCE_UPDATE,
    EVENT_ACTIVATE_VITA,
    EVENT_ACTIVATE_ANIMA,
    EVENT_UNSTABLE_VITA,
    EVENT_CRACKLING_STALKER,
    EVENT_UNSTABLE_ANIMA,
    EVENT_SANGUINE_HORROR,
    EVENT_MOVE,
    EVENT_CRACKLE,
    EVENT_SANGUINE_VOLLEY,
    EVENT_LAST_PHASE,
    EVENT_RUIN_BOLT,
    EVENT_CALL_ESSENCE,
};

enum Actions
{
    ACTION_CREATION = 1,
    ACTION_VITA,
    ACTION_ANIMA,
    ACTION_CORRUPTED_VITA,
    ACTION_CORRUPTED_ANIMA,
    ACTION_CORRUPTED_ESSENCE_MOVE,
    ACTION_DEBUG_RESET_RADEN
};

enum Datas
{
    DATA_CORRUPTED_ESSENCE_ACTIVE   = 1,
    DATA_ESSENCE_MOVE,
    DATA_RADEN_PHASE
};

enum Phases
{
    PHASE_NONE,
    PHASE_VITA,
    PHASE_ANIMA,
    PHASE_LAST,
};

const Position spherePos[2] = 
{
    { 5448.32f, 4593.14f, -2.46f, 1.56f },
    { 5447.50f, 4722.57f, -2.46f, 4.71f }
};

const Position centerPos = { 5448.50f, 4655.93f, -2.46f, 0.0f };

void RemoveRadenEncounterAuras(InstanceScript* p_Instance)
{
    if (!p_Instance)
        return;

    std::vector<uint32> const l_AurasToRemove =
    {
        SPELL_UNLEASHED_ANIMA,
        SPELL_UNSTABLE_ANIMA,
        SPELL_UNSTABLE_VITA_ALLY,
        SPELL_UNSTABLE_VITA,
        SPELL_VITA_SENSITIVITY,
        SPELL_ANIMA_SENSITIVITY
    };

    for (uint32 l_Aura : l_AurasToRemove)
        p_Instance->DoRemoveAurasDueToSpellOnPlayers(l_Aura);
}

Creature* GetRaden(Unit* me)
{
    if (!me)
        return nullptr;

    InstanceScript* l_Instance = me->GetInstanceScript();
    if (!l_Instance)
        return nullptr;

    if (Creature* l_RaDen = Creature::GetCreature(*me, l_Instance->GetData64(NPC_RA_DEN)))
        return l_RaDen;

    return nullptr;
}

class boss_ra_den : public CreatureScript
{
    public:
        boss_ra_den() : CreatureScript("boss_ra_den") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_ra_denAI(creature);
        }

        struct boss_ra_denAI : public BossAI
        {
            boss_ra_denAI(Creature* creature) : BossAI(creature, DATA_RA_DEN)
            {
                ApplyAllImmunities(true);

                me->setActive(true);

                isIntroDone = false;
                introStep = 0;
                introTimer = 1000;

                talkCreationDone = false;
                talkVitaDone = false;
                talkAnimaDone = false;
                talkMurderousStrike = false;

                phase = PHASE_NONE;

                isCompleted = false;

                m_Instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                summons.DespawnAll();
                m_Events.Reset();

                InitPowers();

                me->SetReactState(REACT_DEFENSIVE);
                me->ClearUnitState(UNIT_STATE_ROOT);

                talkCreationDone    = false;
                talkVitaDone        = false;
                talkAnimaDone       = false;
                talkMurderousStrike = false;
                corruptedSummonDone = false;

                phase = PHASE_NONE;

                isCompleted = false;

                if (m_Instance)
                {
                    if (m_Instance->GetBossState(DATA_RA_DEN) != DONE)
                        m_Instance->SetBossState(DATA_RA_DEN, NOT_STARTED);
                    else
                        EndEncounter();
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    RemoveRadenEncounterAuras(m_Instance);
                }

                me->SetVisible(IsHeroic());
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!isIntroDone)
                {
                    if (who->GetTypeId() == TYPEID_PLAYER && me->GetDistance(who) <= 100.0f)
                    {
                        isIntroDone = true;
                        introStep = 1;
                        introTimer = 1000;
                    }
                }
            }

            void EnterCombat(Unit* attacker) override
            {
                if (!m_Instance)
                    return;

                if (m_Instance->GetBossState(DATA_RA_DEN) == IN_PROGRESS)
                    return;

                m_Instance->SetBossState(DATA_RA_DEN, IN_PROGRESS);

                if (!m_Instance->CheckRequiredBosses(DATA_RA_DEN))
                {
                    EnterEvadeMode();
                    return;
                }

                if (!m_Instance->GetData(DATA_TEST_REALM))
                {
                    uint32 triesCount = m_Instance->GetData(DATA_RADEN_TRIES_COUNT);
                    if (triesCount >= RADEN_MAX_TRIES)
                    {
                        EnterEvadeMode();
                        return;
                    }
                }

                Talk(SAY_AGGRO);

                m_Events.ScheduleEvent(EVENT_CREATION, 11000);

                DoZoneInCombat();
            }
            
            void EnterEvadeMode() override
            {
                if (!m_Instance || m_Instance->GetBossState(DATA_RA_DEN) != IN_PROGRESS)
                    return;

                m_Events.Reset();
                summons.DespawnAll();
                me->ClearUnitState(UNIT_STATE_ROOT);

                m_Instance->SetBossState(DATA_RA_DEN, FAIL);

                if (m_Instance->CheckRequiredBosses(DATA_RA_DEN))
                {
                    uint32 radenCount = m_Instance->GetData(DATA_RADEN_TRIES_COUNT);
                    m_Instance->SetData(DATA_RADEN_TRIES_COUNT, radenCount + 1);
                }

                me->RemoveAllAuras();
                me->RemoveAllDynObjects();
                me->RemoveAllAreasTrigger();
                me->SetFullHealth();

                me->GetMotionMaster()->MoveTargetedHome();

                RemoveRadenEncounterAuras(m_Instance);

                _EnterEvadeMode();
            }

            void JustReachedHome() override
            {
                Reset();
            }

            void KilledUnit(Unit* who) override
            {
                if (who && who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void SummonedCreatureDies(Creature* summon, Unit* killer) override
            {
                summons.DespawnEntry(me->GetEntry());
            }

            void DoAction(const int32 action) override
            {
                switch (action)
                {
                    case ACTION_CREATION:
                        SummonEssences();
                        break;
                    case ACTION_VITA:
                        if (phase == PHASE_VITA)
                            return;

                        phase = PHASE_VITA;

                        CancelAnima();

                        m_Events.CancelEvent(EVENT_ACTIVATE_ANIMA);
                        m_Events.ScheduleEvent(EVENT_ACTIVATE_VITA, 1500);
                        break;
                    case ACTION_ANIMA:
                        if (phase == PHASE_ANIMA)
                            return;

                        phase = PHASE_ANIMA;

                        CancelVita();

                        m_Events.CancelEvent(EVENT_ACTIVATE_VITA);
                        m_Events.ScheduleEvent(EVENT_ACTIVATE_ANIMA, 1500);
                        break;
                    case ACTION_CORRUPTED_VITA:
                    case ACTION_CORRUPTED_ANIMA:
                        DoCast(me, action == ACTION_CORRUPTED_VITA ? SPELL_CORRUPTED_VITA : SPELL_CORRUPTED_ANIMA, true);
                        me->ClearUnitState(UNIT_STATE_CASTING);             ///< Prevent next spell to be cancelled because of UNIT_STATE_CASTING
                        DoCastAOE(SPELL_CALL_ESSENCE);                      ///< Call a new orb
                        break;
                    case ACTION_DEBUG_RESET_RADEN:
                    {
                        if (!m_Instance)
                            return;

                        m_Instance->SetBossState(DATA_RA_DEN, NOT_STARTED);
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        me->setFaction(FACTION_MONSTER);
                        talkCreationDone = false;
                        talkVitaDone = false;
                        talkAnimaDone = false;
                        talkMurderousStrike = false;
                        corruptedSummonDone = false;

                        phase = PHASE_NONE;

                        isCompleted = false;
                        m_Events.Reset();
                        InitPowers();
                        me->SetReactState(REACT_DEFENSIVE);

                        if (GameObject* l_Chest = GetClosestGameObjectWithEntry(me, Is25ManRaid() ? DATA_CACHE_OF_STORMS_25_HEROIC : DATA_CACHE_OF_STORMS_10_HEROIC, 100.f))
                            l_Chest->RemoveFromWorld();
                        break;
                    }
                    default:
                        break;
                }
            }

            void AttackStart(Unit* target, bool /*p_Melee*/) override
            {
                if (!target)
                    return;

                if (phase == PHASE_LAST)
                {
                    if (me->Attack(target, true))
                        DoStartNoMovement(target);
                }
                else
                    BossAI::AttackStart(target);
            }

            void DamageTaken(Unit* attacker, uint32& damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Instance)
                {
                    if (m_Instance->GetBossState(DATA_RA_DEN) != IN_PROGRESS && me->getFaction() == FACTION_MONSTER)
                        EnterCombat(attacker);
                }

                if (phase != PHASE_LAST)
                {
                    if (me->HealthBelowPctDamaged(40, damage))
                    {
                        phase = PHASE_LAST;

                        Talk(SAY_PHASE_2);

                        if (m_Instance)
                        {
                            m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA);
                            m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA_ALLY);
                            m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VITA_SENSITIVITY);
                            m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_ANIMA);
                        }

                        me->SetPower(POWER_MANA, 0);
                        m_Events.Reset();
                        CancelVita();
                        CancelAnima();
                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();

                        me->GetMotionMaster()->MovePoint(1, centerPos);
                    }
                }
                else
                {
                    if (me->HealthBelowPctDamaged(10, damage))
                    {
                        damage = 0;

                        if (!isCompleted)
                            EndEncounter();
                    }
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type != POINT_MOTION_TYPE || p_Id != 1)
                    return;

                me->SetReactState(REACT_AGGRESSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                m_Events.ScheduleEvent(EVENT_LAST_PHASE, 1000);
            }

            uint32 GetData(uint32 p_Data) override
            {
                if (p_Data == DATA_RADEN_PHASE)
                    return phase;

                return 0;
            }

            void SetGUID(uint64 p_Guid, int32 p_Data) override
            {
                if (p_Data == DATA_ESSENCE_MOVE)
                {
                    if (Creature* l_Essence = Creature::GetCreature(*me, p_Guid))
                        l_Essence->AI()->DoAction(ACTION_CORRUPTED_ESSENCE_MOVE);
                }
            }

            void UpdateAI(const uint32 diff) override
            {
                UpdateIntro(diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (CheckFatalStrike() || CheckMurderousStrike())
                    return;

                // Trying to draw the boss out of his room
                if (me->GetDistance(me->GetHomePosition()) > 95.0f)
                    EnterEvadeMode();

                if (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CREATION:
                            if (!talkCreationDone)
                            {
                                talkCreationDone = true;
                                Talk(SAY_CREATION);
                            }

                            DoCast(SPELL_MATERIALS_OF_CREATION);
                            m_Events.ScheduleEvent(EVENT_CREATION, urand(32500, 35000));
                            break;
                        case EVENT_ACTIVATE_VITA:
                            ApplyVita();
                            break;
                        case EVENT_ACTIVATE_ANIMA:
                            ApplyAnima();
                            break;
                        case EVENT_UNSTABLE_VITA:
                            DoCastAOE(SPELL_UNSTABLE_VITA);
                            break;
                        case EVENT_CRACKLING_STALKER:
                            DoCast(me, SPELL_SUMMON_CRACKLING_STALKER);
                            m_Events.ScheduleEvent(EVENT_CRACKLING_STALKER, 40000);
                            break;
                        case EVENT_UNSTABLE_ANIMA:
                            break;
                        case EVENT_SANGUINE_HORROR:
                            DoCast(me, SPELL_SUMMON_SANGUINE_HORROR);
                            m_Events.ScheduleEvent(EVENT_SANGUINE_HORROR, 40000);
                            break;
                        case EVENT_LAST_PHASE:
                            SummonCorruptedEssences();
                            me->AddAura(SPELL_RUIN, me);

                            m_Events.ScheduleEvent(EVENT_CALL_ESSENCE, 15000);
                            m_Events.ScheduleEvent(EVENT_RUIN_BOLT, 5000);
                            break;
                        case EVENT_CALL_ESSENCE:
                            DoCastAOE(SPELL_CALL_ESSENCE);
                            break;
                        case EVENT_RUIN_BOLT:
                            if (Unit* victim = me->getVictim())
                            {
                                if (!me->IsWithinMeleeRange(victim))
                                    DoCastVictim(SPELL_RUIN_BOLT);
                            }
                            m_Events.ScheduleEvent(EVENT_RUIN_BOLT, 3000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:

            bool isIntroDone;
            uint8 introStep;
            uint32 introTimer;
            InstanceScript* m_Instance;
            EventMap m_Events;

            bool talkCreationDone;
            bool talkVitaDone;
            bool talkAnimaDone;
            bool talkMurderousStrike;
            bool isCompleted;
            bool corruptedSummonDone;

            Phases phase;

        private:

            void InitPowers()
            {
                me->AddAura(SPELL_ZERO_MANA, me);

                me->setPowerType(POWER_MANA);
                me->SetMaxPower(POWER_MANA, 100);
                me->SetPower(POWER_MANA, 0);
            }

            void UpdateIntro(const uint32 diff)
            {
                if (introStep == 0)
                    return;

                if (introTimer <= diff)
                {
                    switch (introStep)
                    {
                        case 1:
                            Talk(SAY_INTRO_1);
                            introStep++;
                            introTimer = 13000;
                            break;
                        case 2:
                            Talk(SAY_INTRO_2);
                            introStep++;
                            introTimer = 22000;
                            break;
                        case 3:
                            Talk(SAY_INTRO_3);
                            introStep = 0;
                            break;
                        default:
                            introStep = 0;
                            break;
                    }
                }
                else
                    introTimer -= diff;
            }

            void SummonEssences()
            {
                uint8 first  = urand(0, 1);
                uint8 second = 1 - first;

                me->CastSpell(spherePos[first].GetPositionX(), spherePos[first].GetPositionY(), spherePos[first].GetPositionZ() + 1.0f, SPELL_SUMMON_ESSENCE_OF_VITA, true);
                me->CastSpell(spherePos[second].GetPositionX(), spherePos[second].GetPositionY(), spherePos[second].GetPositionZ(), SPELL_SUMMON_ESSENCE_OF_ANIMA, true);
            }

            void SummonCorruptedEssences()
            {
                if (corruptedSummonDone)
                    return;

                std::vector<Position> positions;
                BuildCorruptedEssencesPositions(positions, centerPos, 50.0f, me->GetPositionZ());

                uint8 i = 0;
                for (std::vector<Position>::const_iterator itr = positions.begin(); itr != positions.end(); ++itr)
                {
                    me->SummonCreature((i % 2) ? NPC_CORRUPTED_ANIMA : NPC_CORRUPTED_VITA, *itr);
                    i = 1 - i;
                }

                corruptedSummonDone = true;
            }

            void BuildCorruptedEssencesPositions(std::vector<Position> &positions, Position const& center, float radius, float z)
            {
                float step = M_PI / 20.0f;
                float angle = 0.0f;

                for (uint8 i = 0; i < 40; angle += step, ++i)
                {
                    Position pos;
                    pos.m_positionX = center.GetPositionX() + radius * cosf(angle);
                    pos.m_positionY = center.GetPositionY() + radius * sinf(angle);
                    pos.m_positionZ = me->GetMap()->GetHeight(pos.m_positionX, pos.m_positionY, z);
                    positions.push_back(pos);
                }
            }

            bool CheckFatalStrike()
            {
                if (phase != PHASE_VITA)
                    return false;

                if (me->GetPower(POWER_MANA) < 100)
                    return false;

                if (me->GetCurrentSpell(CURRENT_MELEE_SPELL))
                    return false;

                if (!talkMurderousStrike)
                {
                    talkMurderousStrike = true;
                    Talk(SAY_MURDEROUS);
                }

                DoCastVictim(SPELL_FATAL_STRIKE);

                return true;
            }

            bool CheckMurderousStrike()
            {
                if (phase != PHASE_ANIMA)
                    return false;

                if (me->GetPower(POWER_MANA) < 100)
                    return false;

                DoCastVictim(SPELL_MURDEROUS_STRIKE);

                return true;
            }

            void ApplyVita()
            {
                if (!talkVitaDone)
                {
                    talkVitaDone = true;
                    Talk(SAY_VITA);
                }

                m_Events.ScheduleEvent(EVENT_UNSTABLE_VITA, 2000);
                m_Events.ScheduleEvent(EVENT_CRACKLING_STALKER, 10000);

                DoCast(SPELL_UNLEASHED_VITA);
                
                me->AddAura(SPELL_IMBUED_WITH_VITA, me);
                me->AddAura(SPELL_LINGERING_ENERGIES, me);
            }

            void CancelVita()
            {
                if (m_Instance)
                {
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_VITA_SENSITIVITY);
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA);
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_VITA_ALLY);
                }
                me->RemoveAura(SPELL_IMBUED_WITH_VITA);
                m_Events.CancelEvent(EVENT_UNSTABLE_VITA);
                m_Events.CancelEvent(EVENT_CRACKLING_STALKER);
            }

            void ApplyAnima()
            {
                if (!talkAnimaDone)
                {
                    talkAnimaDone = true;
                    Talk(SAY_ANIMA);
                }

                m_Events.ScheduleEvent(EVENT_UNSTABLE_ANIMA, 2000);
                m_Events.ScheduleEvent(EVENT_SANGUINE_HORROR, 10000);

                DoCast(SPELL_UNLEASHED_ANIMA);
                me->AddAura(SPELL_IMBUED_WITH_ANIMA, me);
                me->AddAura(SPELL_LINGERING_ENERGIES, me);
            }

            void CancelAnima()
            {
                if (m_Instance)
                {
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANIMA_SENSITIVITY);
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNLEASHED_ANIMA);
                }

                me->RemoveAura(SPELL_IMBUED_WITH_ANIMA);
                m_Events.CancelEvent(EVENT_UNSTABLE_ANIMA);
                m_Events.CancelEvent(EVENT_SANGUINE_HORROR);
            }

            void EndEncounter()
            {
                if (isCompleted)
                    return;

                isCompleted = true;

                m_Events.Reset();
                summons.DespawnAll();

                me->setFaction(FACTION_FRIENDLY);
                me->RemoveAllAuras();
                me->RemoveAllAreasTrigger();
                me->RemoveAllDynObjects();

                me->AttackStop();
                me->InterruptNonMeleeSpells(true);
                me->CombatStop(true);

                if (m_Instance)
                {
                    /// True if Boss State isn't already on DONE
                    if (m_Instance->SetBossState(DATA_RA_DEN, DONE))
                    {
                        m_Instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_RA_DEN, me);
                        m_Instance->DoModifyPlayerCurrencies(396, 4000);
                        m_Instance->DoRespawnGameObject(m_Instance->GetData64(Is25ManRaid() ? DATA_CACHE_OF_STORMS_25_HEROIC : DATA_CACHE_OF_STORMS_10_HEROIC), DAY);
                        RemoveRadenEncounterAuras(m_Instance);

                        me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                        Talk(SAY_END);

                        m_Instance->DoCompleteAchievement(eAchievements::ACHIEVEMENT_HEROIC_RADEN);
                        if (AchievementEntry const* l_Achievement = sAchievementStore.LookupEntry(eAchievements::ACHIEVEMENT_RADEN_REALM_FIRST))
                        {
                            // Someone on this realm has already completed that achievement
                            if (!sAchievementMgr->IsRealmCompleted(l_Achievement))
                                m_Instance->DoCompleteAchievement(eAchievements::ACHIEVEMENT_RADEN_REALM_FIRST);
                        }
                    }

                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    Map::PlayerList const& l_PlrList = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Itr = l_PlrList.begin(); l_Itr != l_PlrList.end(); ++l_Itr)
                    {
                        if (Player* l_Player = l_Itr->getSource())
                            me->CastSpell(l_Player, SPELL_RA_DEN_BONUS, true);
                    }

                    // LFR Loot
                    if (me->GetMap()->IsLFR())
                    {
                        Player* l_Player = me->GetMap()->GetPlayers().begin()->getSource();
                        if (l_Player && l_Player->GetGroup())
                            sLFGMgr->AutomaticLootAssignation(me, l_Player->GetGroup());
                    }
                }
            }
        };
};

class npc_ra_den_essence_of_vita_anima : public CreatureScript
{
    public:
        npc_ra_den_essence_of_vita_anima() : CreatureScript("npc_ra_den_essence_of_vita_anima") { }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ra_den_essence_of_vita_animaAI(p_Creature);
        }

        struct npc_ra_den_essence_of_vita_animaAI : public ScriptedAI
        {
            npc_ra_den_essence_of_vita_animaAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.5f);

                if (me->GetEntry() == NPC_ESSENCE_OF_VITA)
                {
                    me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    me->SetHover(true);
                }
            }

            void IsSummonedBy(Unit* owner) override
            {
                events.ScheduleEvent(EVENT_ESSENCE_MOVE, 1000);
                events.ScheduleEvent(EVENT_ESSENCE_UPDATE, 500);
            }

            void JustDied(Unit* who) override
            {
                events.CancelEvent(EVENT_ESSENCE_UPDATE);
                me->DespawnOrUnsummon();
            }

            void UpdateAI(const uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ESSENCE_MOVE:
                            if (Creature* pRaden = GetRaden(me))
                            {
                                uint32 spellId = me->GetEntry() == NPC_ESSENCE_OF_VITA ? SPELL_DRAW_VITA : SPELL_DRAW_ANIMA;
                                DoCast(pRaden, spellId);
                                me->ClearUnitState(UNIT_STATE_CASTING);

                                me->GetMotionMaster()->MoveFollow(pRaden, 0.1f, 0.1f);
                            }
                            break;
                        case EVENT_ESSENCE_UPDATE:
                            if (Creature* pRaden = GetClosestCreatureWithEntry(me, NPC_RA_DEN, 1.0f, true))
                            {
                                if (pRaden->AI()->GetData(DATA_RADEN_PHASE) != PHASE_LAST)
                                    pRaden->AI()->DoAction(me->GetEntry() == NPC_ESSENCE_OF_VITA ? ACTION_VITA : ACTION_ANIMA);
                                me->DespawnOrUnsummon();
                            }
                            else
                                events.ScheduleEvent(EVENT_ESSENCE_UPDATE, 500);
                            break;
                        default:
                            break;
                    }
                }
            }
        };
};

class npc_ra_den_corrupted_vita_anima : public CreatureScript
{
    public:
        npc_ra_den_corrupted_vita_anima() : CreatureScript("npc_ra_den_corrupted_vita_anima") { }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ra_den_corrupted_vita_animaAI(p_Creature);
        }

        struct npc_ra_den_corrupted_vita_animaAI : public ScriptedAI
        {
            npc_ra_den_corrupted_vita_animaAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);

                isActive = false;
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 0.5f);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

                if (me->GetEntry() == NPC_CORRUPTED_VITA)
                {
                    //me->SetCanFly(true);
                    //me->SetDisableGravity(true);
                    me->SetHover(true);
                }
            }

            void IsSummonedBy(Unit* summoner) override
            {
                // spinning around Ra-den
                if (summoner->GetEntry() == NPC_RA_DEN)
                {
                    Movement::MoveSplineInit l_Init(me);
                    FillCirclePath(me, *summoner, me->GetDistance(summoner), l_Init.Path());
                    l_Init.SetWalk(true);
                    l_Init.SetCyclic();
                    l_Init.Launch();
                }
            }

            uint32 GetData(uint32 type) override
            {
                if (type == DATA_CORRUPTED_ESSENCE_ACTIVE)
                    return isActive ? 1 : 0;

                return 0;
            }

            void MovementInform(uint32 p_Type, uint32 p_Id) override
            {
                if (p_Type != POINT_MOTION_TYPE || p_Id != 1)
                    return;

                if (Creature* l_RaDen = GetRaden(me))
                {
                    l_RaDen->AI()->DoAction(me->GetEntry() == NPC_CORRUPTED_ANIMA ? ACTION_CORRUPTED_ANIMA : ACTION_CORRUPTED_VITA);
                    me->DespawnOrUnsummon();
                }
            }

            void DoAction(const int32 action) override
            {
                if (action == ACTION_CORRUPTED_ESSENCE_MOVE)
                {
                    isActive = true;

                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);

                    Movement::MoveSplineInit l_Init(me);
                    l_Init.Stop();

                    me->SetSpeed(MOVE_RUN, 0.3f);

                    if (Creature* pRaden = GetRaden(me))
                        me->GetMotionMaster()->MovePoint(1, *pRaden, false);
                }
            }

            void JustDied(Unit* who) override
            {
                me->StopMoving();
                me->GetMotionMaster()->MovementExpired(false);

                switch (me->GetEntry())
                {
                    case NPC_CORRUPTED_VITA:
                    {
                        if (Player* target = me->FindNearestPlayer(100.0f))
                            DoCast(target, SPELL_CORRUPTED_VITA_DMG, true);
                        break;
                    }
                    case NPC_CORRUPTED_ANIMA:
                        DoCastAOE(SPELL_TWISTED_ANIMA, true);
                        break;
                    default:
                        break;
                }

                me->DespawnOrUnsummon(500);
            }

            private:
                bool isActive;

                void FillCirclePath(Creature* me, Position const& p_CenterPos, float p_Radius, Movement::PointsArray& p_Path)
                {
                    float l_Step = M_PI / 6.0f;
                    float l_Angle = p_CenterPos.GetAngle(me->GetPositionX(), me->GetPositionY());

                    for (uint8 l_Iter = 0; l_Iter < 12; l_Angle += l_Step, l_Iter++)
                    {
                        G3D::Vector3 l_Point;
                        l_Point.x = p_CenterPos.GetPositionX() + p_Radius * cosf(l_Angle);
                        l_Point.y = p_CenterPos.GetPositionY() + p_Radius * sinf(l_Angle);
                        l_Point.z = me->GetPositionZ();
                        p_Path.push_back(l_Point);
                    }
                }
        };
};

class npc_ra_den_crackling_stalker : public CreatureScript
{
    public:
        npc_ra_den_crackling_stalker() : CreatureScript("npc_ra_den_crackling_stalker") { }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ra_den_crackling_stalkerAI(p_Creature);
        }

        struct npc_ra_den_crackling_stalkerAI : public ScriptedAI
        {
            npc_ra_den_crackling_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                ApplyAllImmunities(true);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                events.Reset();
            }

            void IsSummonedBy(Unit* owner) override
            {
                events.ScheduleEvent(EVENT_MOVE, 1500);
            }

            void JustDied(Unit* who) override
            {
                DoCastAOE(SPELL_CAUTERIZING_FLARE);

                me->DespawnOrUnsummon(1000);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.ScheduleEvent(EVENT_CRACKLE, urand(8000, 10000));
                            break;
                        case EVENT_CRACKLE:
                            DoCastVictim(SPELL_CRACKLE);
                            events.ScheduleEvent(EVENT_CRACKLE, urand(15000, 20000));
                            break;
                        default:
                            break;
                    }
                }
            }

        private:

            void ApplyLingeringEnergiesFromRaden()
            {
                if (Creature* pRaden = GetRaden(me))
                {
                    if (Aura* aur = pRaden->GetAura(SPELL_LINGERING_ENERGIES))
                    {
                        uint8 stacks = aur->GetStackAmount();

                        if (Aura* aur = me->AddAura(SPELL_LINGERING_ENERGIES, me))
                            aur->SetStackAmount(stacks);
                    }
                }
            }
        };
};

class npc_ra_den_sanguine_horror : public CreatureScript
{
    public:
        npc_ra_den_sanguine_horror() : CreatureScript("npc_ra_den_sanguine_horror") { }

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_ra_den_sanguine_horrorAI(p_Creature);
        }

        struct npc_ra_den_sanguine_horrorAI : public Scripted_NoMovementAI
        {
            npc_ra_den_sanguine_horrorAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature)
            {
                ApplyAllImmunities(true);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                events.Reset();
            }

            void IsSummonedBy(Unit* owner) override
            {
                events.ScheduleEvent(EVENT_MOVE, 1500);

                ApplyLingeringEnergiesFromRaden();
            }

            void JustDied(Unit* who) override
            {
                me->DespawnOrUnsummon(1000);
            }

            void UpdateAI(const uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.ScheduleEvent(EVENT_SANGUINE_VOLLEY, 4000);
                            break;
                        case EVENT_SANGUINE_VOLLEY:
                            if (!me->IsWithinMeleeRange(me->getVictim()))
                                DoCastAOE(SPELL_SANGUINE_VOLLEY);

                            events.ScheduleEvent(EVENT_SANGUINE_VOLLEY, 3000);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:

            void ApplyLingeringEnergiesFromRaden()
            {
                if (Creature* pRaden = GetRaden(me))
                {
                    if (Aura* aur = pRaden->GetAura(SPELL_LINGERING_ENERGIES))
                    {
                        uint8 stacks = aur->GetStackAmount();

                        if (Aura* aur = me->AddAura(SPELL_LINGERING_ENERGIES, me))
                            aur->SetStackAmount(stacks);
                    }
                }
            }
        };
};

#define GOSSIP_ACTIVATE_RADEN_IRIS "Send us to Ra-den, please."

class npc_ra_den_test : public CreatureScript
{
    public:
        npc_ra_den_test() : CreatureScript("npc_ra_den_test") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ACTIVATE_RADEN_IRIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->SEND_GOSSIP_MENU(1, creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            InstanceScript* l_Instance = player->GetInstanceScript();

            if (l_Instance)
            {
                if (GameObject* l_Iris = GameObject::GetGameObject(*player, l_Instance->GetData64(GOB_RADEN_ENTRANCE)))
                    l_Iris->SetGoState(l_Iris->GetGoState() == GO_STATE_ACTIVE ? GO_STATE_READY : GO_STATE_ACTIVE);

                Map::PlayerList const &l_PlayerList = player->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
                    l_Itr->getSource()->NearTeleportTo(5593.81f, 4655.74f, -2.48f, M_PI);
            }

            return true;
        }

        struct npc_ra_den_testAI : public ScriptedAI
        {
            npc_ra_den_testAI(Creature* creature) : ScriptedAI(creature)
            {
                m_Instance = creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset()
            {
                if (!IsHeroic())
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetVisible(false);
                }

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);
                me->SetObjectScale(0.2f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ra_den_testAI(creature);
        }
};

class spell_ra_den_materials_of_creation: public SpellScriptLoader
{
    public:
        spell_ra_den_materials_of_creation() : SpellScriptLoader("spell_ra_den_materials_of_creation") { }

        class spell_ra_den_materials_of_creation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ra_den_materials_of_creation_SpellScript);

            void HandleAfterCast()
            {
                if (!GetCaster())
                    return;

                if (Creature* pRaden = GetCaster()->ToCreature())
                    pRaden->AI()->DoAction(ACTION_CREATION);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_ra_den_materials_of_creation_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ra_den_materials_of_creation_SpellScript();
        }
};

class spell_ra_den_fatal_strike: public SpellScriptLoader
{
    public:
        spell_ra_den_fatal_strike() : SpellScriptLoader("spell_ra_den_fatal_strike") { }

        class spell_ra_den_fatal_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ra_den_fatal_strike_SpellScript);

            void HandleDamage(SpellEffIndex effIndex)
            {
                if (!GetHitUnit())
                    return;

                if (Player* player = GetHitUnit()->ToPlayer())
                {
                    uint32 l_Aura = 0;
                    switch (player->GetActiveSpecializationID())
                    {
                        case SPEC_PALADIN_PROTECTION:
                            l_Aura = SPELL_PALADIN_PROTECTION;
                            break;
                        case SPEC_WARRIOR_PROTECTION:
                            l_Aura = SPELL_WARRIOR_PROTECTION;
                            break;
                        case SPEC_DRUID_GUARDIAN:
                            l_Aura = SPELL_DRUID_BEAR;
                            break;
                        case SPEC_DK_BLOOD:
                            l_Aura = SPELL_DK_BLOOD;
                            break;
                        case SPEC_MONK_BREWMASTER:
                            l_Aura = SPELL_MONK_BREWMASTER;
                            break;
                        default:
                            break;
                    }

                    if (player->HasAura(l_Aura))
                        PreventHitEffect(effIndex);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_ra_den_fatal_strike_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_INSTAKILL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ra_den_fatal_strike_SpellScript();
        }
};

class spell_ra_den_unstable_vita_aura : public SpellScriptLoader
{
    public:
        spell_ra_den_unstable_vita_aura() : SpellScriptLoader("spell_ra_den_unstable_vita_aura") { }
        
        class spell_ra_den_unstable_vita_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ra_den_unstable_vita_aura_SpellScript);

            void FilterTargets(std::list<WorldObject*> &targets)
            {
                if (!GetCaster())
                    return;

                targets.clear();

                Creature* pRaden = GetCaster()->ToCreature();
                if (!pRaden || !pRaden->IsAIEnabled || !pRaden->GetAI())
                    return;

                Unit* target = pRaden->AI()->SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true);
                if (!target)
                    Unit* target = pRaden->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
                
                if (target)
                    targets.push_back(target);
            }

            void Register() override
            {
                if (m_scriptSpellId == SPELL_UNSTABLE_VITA)
                {
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ra_den_unstable_vita_aura_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                }
            }
        };

        class spell_ra_den_unstable_vita_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ra_den_unstable_vita_aura_AuraScript);

            void HandleTick(AuraEffect const* aurEff)
            {
                if (!GetUnitOwner())
                    return;

                Player* player = GetUnitOwner()->ToPlayer();
                if (!player)
                    return;

                InstanceScript* pInstance = player->GetInstanceScript();
                if (!pInstance)
                    return;

                Creature* pRaden = GetRaden(player);
                if (!pRaden || !pRaden->IsAIEnabled || !pRaden->GetAI())
                    return;

                if (pRaden->AI()->GetData(DATA_RADEN_PHASE) != PHASE_VITA)
                    return;

                std::list<Player*> players;

                pRaden->GetPlayerListInGrid(players, 180.0f);

                players.remove(player);
                if (players.size() < 1)
                    return;

                players.sort(JadeCore::DistanceOrderPred(player));

                std::list<Player*>::reverse_iterator ritr = players.rbegin();

                Player* target = *ritr;

                if (target->GetGUID() == player->GetGUID())
                    return;

                player->AddAura(SPELL_UNSTABLE_VITA_ALLY, target);
                player->CastSpell(target, SPELL_UNSTABLE_VITA_DUMMY, true);
                player->CastSpell(target, SPELL_UNSTABLE_VITA_DMG, true, NULL, NULL, pRaden->GetGUID());
            }

            void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Creature* l_Raden = GetRaden(l_Owner))
                {
                    if (l_Raden->IsAIEnabled && l_Raden->GetAI())
                    {
                        if (l_Raden->AI()->GetData(DATA_RADEN_PHASE) == PHASE_VITA)
                            l_Owner->AddAura(SPELL_VITA_SENSITIVITY, l_Owner);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ra_den_unstable_vita_aura_AuraScript::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ra_den_unstable_vita_aura_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ra_den_unstable_vita_aura_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_ra_den_unstable_vita_aura_AuraScript();
        }
};

class spell_ra_den_unstable_vita_dmg: public SpellScriptLoader
{
    public:
        spell_ra_den_unstable_vita_dmg() : SpellScriptLoader("spell_ra_den_unstable_vita_dmg") { }

        class spell_ra_den_unstable_vita_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ra_den_unstable_vita_dmg_SpellScript);

            void HandleKill(SpellEffIndex effIndex)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (!l_Target->HasAura(SPELL_VITA_SENSITIVITY))
                    PreventHitDefaultEffect(effIndex);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_ra_den_unstable_vita_dmg_SpellScript::HandleKill, EFFECT_1, SPELL_EFFECT_INSTAKILL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ra_den_unstable_vita_dmg_SpellScript();
        }
};

class spell_ra_den_murderous_strike: public SpellScriptLoader
{
    public:
        spell_ra_den_murderous_strike() : SpellScriptLoader("spell_ra_den_murderous_strike") { }

        class spell_ra_den_murderous_strike_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ra_den_murderous_strike_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool& /*canBeRecalculated*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                amount = l_Owner->CountPctFromCurHealth(20);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_ra_den_murderous_strike_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ra_den_murderous_strike_AuraScript();
        }
};

class spell_ra_den_unstable_anima_aura : public SpellScriptLoader
{
    public:
        spell_ra_den_unstable_anima_aura() : SpellScriptLoader("spell_ra_den_unstable_anima_aura") { }

        class spell_ra_den_unstable_anima_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ra_den_unstable_anima_aura_SpellScript);

            void FilterTargets(std::list<WorldObject*> &targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster || l_Caster->GetEntry() != NPC_RA_DEN)
                    return;

                targets.clear();

                Creature* pRaden = l_Caster->ToCreature();
                if (!pRaden)
                    return;

                Unit* target = pRaden->AI()->SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true);
                if (!target)
                    Unit* target = pRaden->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true);
                
                if (target)
                    targets.push_back(target);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ra_den_unstable_anima_aura_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_ra_den_unstable_anima_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ra_den_unstable_anima_aura_AuraScript);

            void HandleTick(AuraEffect const* aurEff)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (Creature* l_Raden = GetRaden(l_Owner))
                    l_Owner->CastSpell(l_Owner, SPELL_UNSTABLE_ANIMA_DMG, true, nullptr, nullptr, l_Raden->GetGUID());
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ra_den_unstable_anima_aura_AuraScript::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ra_den_unstable_anima_aura_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_ra_den_unstable_anima_aura_AuraScript();
        }
};


class spell_ra_den_unstable_anima_dmg: public SpellScriptLoader
{
    public:
        spell_ra_den_unstable_anima_dmg() : SpellScriptLoader("spell_ra_den_unstable_anima_dmg") { }

        class spell_ra_den_unstable_anima_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ra_den_unstable_anima_dmg_SpellScript);

            void HandleKill(SpellEffIndex effIndex)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (!l_Target->HasAura(SPELL_ANIMA_SENSITIVITY))
                {
                    PreventHitDefaultEffect(effIndex);
                    l_Target->AddAura(SPELL_ANIMA_SENSITIVITY, l_Target);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_ra_den_unstable_anima_dmg_SpellScript::HandleKill, EFFECT_1, SPELL_EFFECT_INSTAKILL);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ra_den_unstable_anima_dmg_SpellScript();
        }
};

class spell_ra_den_call_essence : public SpellScriptLoader
{
    public:
        spell_ra_den_call_essence() : SpellScriptLoader("spell_ra_den_call_essence") { }
        
        class spell_ra_den_call_essence_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ra_den_call_essence_SpellScript);

            void FilterTargets(std::list<WorldObject*> &targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                targets.clear();

                std::list<Creature*> creatures;
                l_Caster->GetCreatureListWithEntryInGrid(creatures, NPC_CORRUPTED_VITA, 90.0f); // TODO: adjust search range
                l_Caster->GetCreatureListWithEntryInGridAppend(creatures, NPC_CORRUPTED_ANIMA, 90.0f); // TODO: adjust search range

                if (creatures.empty())
                    return;

                creatures.remove_if([](Creature* l_Creature) -> bool
                {
                    if (l_Creature->IsAIEnabled && l_Creature->AI())
                    {
                        if (l_Creature->AI()->GetData(DATA_CORRUPTED_ESSENCE_ACTIVE))
                            return true;
                    }

                    return false;
                });

                if (!creatures.empty())
                {
                    if (Creature* target = JadeCore::Containers::SelectRandomContainerElement(creatures))
                    {
                        targets.push_back(target);
                        if (l_Caster->GetEntry() == NPC_RA_DEN)
                            l_Caster->GetAI()->SetGUID(target->GetGUID(), DATA_ESSENCE_MOVE);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ra_den_call_essence_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ra_den_call_essence_SpellScript();
        }
};

// 139071 - Corrupted Anima
class spell_ra_den_corrupted_anima : public SpellScriptLoader
{
    public:
        spell_ra_den_corrupted_anima() : SpellScriptLoader("spell_ra_den_corrupted_anima") { }

        class spell_ra_den_corrupted_anima_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ra_den_corrupted_anima_SpellScript);

            float healthLevel;

            void SaveHealthLevel()
            {
                if (Unit* owner = GetCaster())
                    healthLevel = owner->GetHealthPct() / 100.0f;
            }

            void AdjustLife()
            {
                if (Unit* owner = GetCaster())
                    owner->SetHealth(owner->GetMaxHealth() * healthLevel);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_ra_den_corrupted_anima_SpellScript::SaveHealthLevel);
                AfterCast  += SpellCastFn(spell_ra_den_corrupted_anima_SpellScript::AdjustLife);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ra_den_corrupted_anima_SpellScript;
        }
};


void AddSC_boss_ra_den()
{
    new boss_ra_den();                          // 69473
    new npc_ra_den_essence_of_vita_anima();     // 69870 69869
    new npc_ra_den_corrupted_vita_anima();      // 69958 69957
    new npc_ra_den_crackling_stalker();         // 69872
    new npc_ra_den_sanguine_horror();           // 69871

    new npc_ra_den_test();                      // 200000

    new spell_ra_den_materials_of_creation();   // 138321
    new spell_ra_den_fatal_strike();            // 138334
    new spell_ra_den_unstable_vita_aura();      // 138297 138308
    new spell_ra_den_unstable_vita_dmg();       // 138370
    new spell_ra_den_murderous_strike();        // 138333
    new spell_ra_den_unstable_anima_aura();     // 138288
    new spell_ra_den_unstable_anima_dmg();      // 138295
    new spell_ra_den_call_essence();            // 139040
    new spell_ra_den_corrupted_anima();         // 139071
}