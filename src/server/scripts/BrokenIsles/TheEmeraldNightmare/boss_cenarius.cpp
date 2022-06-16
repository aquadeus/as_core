////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_emerald_nightmare.hpp"

enum Says
{
    SAY_AGGRO                     = 0,
    SAY_FORCES_OF_NIGHTMARE_EMOTE = 1,
    SAY_FORCES_OF_NIGHTMARE       = 2,
    SAY_NIGHTMARE_BRAMBLES        = 3,
    SAY_DEATH                     = 6
};

enum Spells
{
    /// Phase 1
    SPELL_CREEPING_NIGHTMARES           = 210280,
    SPELL_FORCES_OF_NIGHTMARE           = 212726,
    SPELL_NIGHTMARE_BRAMBLES_SUM        = 210290,
    SPELL_AURA_OF_DREAD_THORNS          = 210346,
    SPELL_SUM_ENTANGLING_NIGHTMARES     = 214454,

    /// Phase 2
    SPELL_OVERWHELMING_NIGHTMARE        = 217368,
    SPELL_NIGHTMARE_BLAST_SAVER         = 214706,
    SPELL_SPEAR_OF_NIGHTMARES           = 214529,
    SPELL_NIGHTMARE_BLAST_AT            = 214711,

    /// Mythic
    SPELL_NIGHTMARE_BLAST               = 213162,
    SPELL_BEASTS_OF_NIGHTMARE           = 214876,

    /// Trash
    /// Malfurion Stormrage - 106482
    SPELL_CLEANSING_GROUND_FILTER       = 212630,
    SPELL_CLEANSING_GROUND_AT           = 212639,
    SPELL_CLEANSING_GROUND_STAGE_2      = 214249,
    SPELL_STORMS_RAGE                   = 214713,
    SPELL_CLEANSING_OUTRO               = 225790,

    /// Nightmare Sapling - 106427
    SPELL_GERMINATING                   = 210861,
    SPELL_NIGHTMARE_GERMINATE           = 210974,
    SPELL_GERMINATE_SUMMON              = 210866,
    SPELL_ALLIES_GERMINATE              = 212175,

    /// Tormented Souls - 106895
    SPELL_TORMENTED                     = 212178,
    SPELL_WISPS_VISUAL_CORRUPTED        = 212466,
    SPELL_WISPS_VISUAL_ALLIES           = 212467,
    SPELL_SUM_NIGHTMARE_WISPS           = 212188,
    SPELL_NIGHTMARE_WISPS_SUMMON        = 212187,
    SPELL_SUM_ALLIES_WISPS              = 212192,

    /// Corrupted Emerald Egg - 106898
    SPELL_INCUBATING                    = 212231,
    SPELL_SUM_NIGHTMARE_DRAKE           = 212233,
    SPELL_SUM_ALLIES_DRAKE              = 212235,

    /// Corrupted Nature - 106899
    SPELL_DRYAD_DUMMY                   = 212241,
    SPELL_DRYAD_VISUAL                  = 212424,
    SPELL_SUM_NIGHTMARE_DRYAD           = 212242,
    SPELL_SUM_ALLIES_DRYAD              = 212243,

    /// ALLIES_WISP - 106659
    SPELL_CLEANSING_DETONATE_VIS        = 213403,
    SPELL_CLEANSING_DETONATE_AT         = 211878,

    /// CLEANSED_ANCIENT - 106667
    SPELL_NATURES_FURY                  = 215099,
    SPELL_REPLENISHING_ROOTS            = 211619,

    /// EMERALD_DRAKE - 106809
    SPELL_ANCIENT_DREAM_FILTER          = 211935,
    SPELL_ANCIENT_DREAM_AURA            = 211939,
    SPELL_POISON_BLAST                  = 216510,

    /// REDEEMED_SISTER - 106831
    SPELL_UNBOUND_TOUCH_FILTER          = 211976,
    SPELL_UNBOUND_TOUCH                 = 211989,
    SPELL_JAVELIN_TOSS                  = 215106,

    /// CORRUPTED_WISP - 106304
    SPELL_DESTRUCTIVE_NIGHTMARES        = 210617,
    SPELL_DESTRUCTIVE_NIGHTMARES_DMG    = 210619,
    SPELL_CORRUPTED_DETONATE            = 226713,
    SPELL_FIXATE                        = 216115,

    /// NIGHTMARE_ANCIENT - 105468
    SPELL_DESICCATING_STOMP             = 211073,
    SPELL_MYTHIC_DESICCATING_STOMP      = 226821,
    SPELL_DESICCATION                   = 211156,

    /// ROTTEN_DRAKE - 105494
    SPELL_ROTTEN_BREATH_FILTER          = 211189,
    SPELL_ROTTEN_BREATH_SUM_TRIGGER     = 211188,
    SPELL_ROTTEN_BREATH_DMG             = 211192,
    SPELL_NIGHTMARE_BUFFET              = 211180,

    /// NPC_TWISTED_SISTER - 105495
    SPELL_NIGHTMARE_JAVELIN             = 211498,
    SPELL_TWISTED_TOUCH_OF_LIFE_FILTER  = 211462,
    SPELL_SCORNED_TOUCH_FILTER          = 211487,
    SPELL_SCORNED_TOUCH_TRIGGER         = 211471,

    /// NIGHTMARE_BRAMBLES - 106167
    SPELL_NIGHTMARE_BRAMBLES_AT         = 210331,
    SPELL_NIGHTMARE_BRAMBLES_AT_PER     = 210312,
    SPELL_NIGHTMARE_BRAMBLES_UNKNOWN    = 210316,

    /// ENTANGLING_ROOTS - 108040
    SPELL_ENTANGLING_NIGHTMARES_VIS     = 214460,
    SPELL_ENTANGLING_NIGHTMARES_AURA    = 214463,
    SPELL_ENTANGLING_NIGHTMARES_VEH     = 214462,
    SPELL_ENTANGLING_NIGHTMARES_STUN    = 214505,

    /// BEAST_OF_NIGHTMARE - 108208
    SPELL_GRIPPING_FOG_AT               = 214878,

    /// Other
    SPELL_NIGHTMARE_DISSOLVE_IN         = 212461,
    SPELL_NIGHTMARE_DISSOLVE_OUT        = 212563,
    SPELL_DUDE_TRACKER_AT               = 212622,
    SPELL_SUMMON_DRAKES_DUMMY           = 212604,
    SPELL_CLEANSED_GROUND               = 212170,
    SPELL_CAST_HELPFUL_SPELL            = 211639,
    SPELL_CLEANSED_DISSOLVE_OUT         = 212562,
    SPELL_DISSOLVE_CLEANSED_IN          = 210988,
    SPELL_NIGHTMARE_ENERGY              = 224200,
    SPELL_COSMETIC_CHANNEL              = 225782
};

enum eEvents
{
    EVENT_FORCES_OF_NIGHTMARE = 1,
    EVENT_NIGHTMARE_BRAMBLES,
    EVENT_OVERWHELMING_NIGHTMARE,
    EVENT_NIGHTMARE_BLAST_SAVER,
    EVENT_SPEAR_OF_NIGHTMARES,
    EVENT_NIGHTMARE_BLAST_OR_BEAST, ///< Mythic

    EVENT_SCENE
};

enum eVisualKit
{
    BRAMBLES_KIT_1 = 67102 ///< SMSG_PLAY_SPELL_VISUAL_KIT
};

Position const g_ForcesPos[4] =
{
    { 11392.7f, -12723.7f, 487.00f }, ///< NPC_TORMENTED_SOULS
    { 11339.5f, -12719.2f, 487.33f }, ///< NPC_NIGHTMARE_SAPLING
    { 11340.0f, -12667.8f, 487.45f }, ///< NPC_CORRUPTED_NATURE
    { 11391.2f, -12670.0f, 487.18f }  ///< NPC_CORRUPTED_EMERALD_EGG
};

Position const g_MalfurionPos   = { 11359.50f, -12665.90f, 487.30f, 4.79f };
Position const g_CenterPos      = { 11366.2f, -12695.1f, 486.91f, 0.0f };

Position const g_BeastPos[9] =
{
    { 11408.86f, -12738.13f, 488.3f, 2.42f },
    { 11405.04f, -12698.67f, 488.0f, 3.06f },
    { 11409.42f, -12661.88f, 488.0f, 3.74f },
    { 11375.43f, -12660.12f, 487.0f, 4.64f },
    { 11354.59f, -12653.14f, 488.3f, 4.86f },
    { 11331.45f, -12661.74f, 488.4f, 5.38f },
    { 11328.11f, -12700.74f, 487.4f, 0.06f },
    { 11335.28f, -12734.77f, 488.0f, 0.97f },
    { 11366.40f, -12742.10f, 488.2f, 1.64f }
};

/// Cenarius - 104636
class boss_cenarius : public CreatureScript
{
    public:
        boss_cenarius() : CreatureScript("boss_cenarius") { }

        struct boss_cenariusAI : public BossAI
        {
            boss_cenariusAI(Creature* p_Creature) : BossAI(p_Creature, DATA_CENARIUS)
            {
                DoCast(SPELL_COSMETIC_CHANNEL); ///< Visual
            }

            bool m_FirstConv;
            bool m_SecondPhase;

            float m_SpearRadius;

            uint8 m_PowerCount; ///< Hack
            uint8 m_PowerTick;
            uint8 m_WaveCount;

            uint16 m_PowerTimer;

            uint32 m_LastCleansedAlly;

            void Reset() override
            {
                _Reset();
                m_SecondPhase = false;
                m_PowerTimer = 0;
                m_PowerTick = 1;
                m_WaveCount = 0;
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 90);
                DoCast(me, SPELL_NIGHTMARE_ENERGY, true);

                AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->SummonCreature(NPC_MALFURION_STORMRAGE, g_MalfurionPos);
                });

                me->SetReactState(REACT_AGGRESSIVE);
                m_FirstConv = false;

                m_SpearRadius = 1.0f;

                m_LastCleansedAlly = 0;

                me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                /// Remove cosmetic channel
                me->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL, true, true);

                _EnterCombat();
                DoCast(me, SPELL_CREEPING_NIGHTMARES, true);
                m_PowerTimer = 1000;

                EntryCheckPredicate l_Pred(NPC_MALFURION_STORMRAGE);
                summons.DoAction(ACTION_1, l_Pred);

                events.ScheduleEvent(EVENT_NIGHTMARE_BRAMBLES, 25000);

                if (IsMythic())
                    events.ScheduleEvent(EVENT_NIGHTMARE_BLAST_OR_BEAST, 30000);

                Talk(SAY_AGGRO);
            }

            void EnterEvadeMode() override
            {
                CreatureAI::EnterEvadeMode();

                RemoveEncounterAuras();

                summons.DespawnAll();

                ClearDelayedOperations();

                me->NearTeleportTo(me->GetHomePosition());
                me->Respawn(true, true, 30 * TimeConstants::IN_MILLISECONDS);

                if (InstanceScript* l_Instance = me->GetInstanceScript())
                    l_Instance->SetBossState(DATA_CENARIUS, EncounterState::FAIL);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->ClearLootContainers();

                RemoveEncounterAuras();

                _JustDied();
                Talk(SAY_DEATH);

                if (Creature* l_Malfurion = me->SummonCreature(NPC_MALFURION_STORMRAGE, me->GetPositionX() + 5.0f, me->GetPositionY() + 5.0f, me->GetPositionZ())) ///< Outro
                {
                    l_Malfurion->SetFacingToObject(me);

                    if (l_Malfurion->IsAIEnabled)
                        l_Malfurion->AI()->DoAction(ACTION_4);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                BossAI::JustSummoned(p_Summon);

                /// Register last cleansed ally for next Forces of Nightmare
                if (IsMythic())
                {
                    switch (p_Summon->GetEntry())
                    {
                        case NPC_ALLIES_WISP:
                        case NPC_CLEANSED_ANCIENT:
                        case NPC_EMERALD_DRAKE:
                        case NPC_REDEEMED_SISTER:
                        {
                            m_LastCleansedAlly = p_Summon->GetEntry();
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void SetData(uint32 /*p_ID*/, uint32 p_Value) override
            {
                if (!IsMythic())
                    return;

                m_LastCleansedAlly = p_Value;
            }

            void SpellHit(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == SPELL_FORCES_OF_NIGHTMARE)
                {
                    switch (m_WaveCount)
                    {
                        case 1:
                        {
                            me->SummonCreature(NPC_TORMENTED_SOULS, g_ForcesPos[0], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            me->SummonCreature(NPC_NIGHTMARE_SAPLING, g_ForcesPos[1], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            me->SummonCreature(NPC_CORRUPTED_EMERALD_EGG, g_ForcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            break;
                        }
                        case 2:
                        {
                            me->SummonCreature(NPC_TORMENTED_SOULS, g_ForcesPos[0], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            me->SummonCreature(NPC_CORRUPTED_NATURE, g_ForcesPos[2], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            me->SummonCreature(NPC_CORRUPTED_EMERALD_EGG, g_ForcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            break;
                        }
                        case 3:
                        {
                            me->SummonCreature(NPC_NIGHTMARE_SAPLING, g_ForcesPos[1], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            me->SummonCreature(NPC_CORRUPTED_NATURE, g_ForcesPos[2], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            break;
                        }
                        case 4:
                        {
                            me->SummonCreature(NPC_TORMENTED_SOULS, g_ForcesPos[0], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            me->SummonCreature(NPC_CORRUPTED_EMERALD_EGG, g_ForcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            break;
                        }
                        case 5:
                        {
                            me->SummonCreature(NPC_NIGHTMARE_SAPLING, g_ForcesPos[1], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            me->SummonCreature(NPC_CORRUPTED_NATURE, g_ForcesPos[3], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            break;
                        }
                        case 6:
                        {
                            me->SummonCreature(NPC_TORMENTED_SOULS, g_ForcesPos[0], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            me->SummonCreature(NPC_CORRUPTED_NATURE, g_ForcesPos[2], TEMPSUMMON_TIMED_DESPAWN, 13000);
                            break;
                        }
                        default:
                            break;
                    }

                    DoCast(me, SPELL_AURA_OF_DREAD_THORNS, true);

                    if (IsMythic() && m_WaveCount >= 2 && m_LastCleansedAlly)
                    {
                        Position l_Pos;
                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 10.0f, frand(0.0f, 6.0f));

                        switch (m_LastCleansedAlly)
                        {
                            case NPC_ALLIES_WISP:
                            {
                                for (uint8 l_I = 0; l_I < 5; ++l_I)
                                {
                                    me->CastSpell(l_Pos, SPELL_NIGHTMARE_WISPS_SUMMON, true);
                                    g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 10.0f, frand(0.0f, 6.0f));
                                }

                                break;
                            }
                            case NPC_CLEANSED_ANCIENT:
                            {
                                me->CastSpell(l_Pos, SPELL_NIGHTMARE_GERMINATE, true);
                                break;
                            }
                            case NPC_EMERALD_DRAKE:
                            {
                                me->CastSpell(l_Pos, SPELL_SUM_NIGHTMARE_DRAKE, true);
                                break;
                            }
                            case NPC_REDEEMED_SISTER:
                            {
                                me->CastSpell(l_Pos, SPELL_SUM_NIGHTMARE_DRYAD, true);
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                else if (p_SpellInfo->Id == SPELL_BEASTS_OF_NIGHTMARE)
                {
                    Position l_PosLeft;
                    Position l_PosRight;

                    std::list<uint8> l_RandList;

                    for (uint8 l_I = 0; l_I < 9; ++l_I)
                        l_RandList.push_back(l_I);

                    JadeCore::Containers::RandomResizeList(l_RandList, 2);

                    for (std::list<uint8>::iterator l_Iter = l_RandList.begin(); l_Iter != l_RandList.end(); ++l_Iter)
                    {
                        switch (*l_Iter)
                        {
                            case 0:
                            case 2:
                            case 5:
                            case 8:
                            {
                                float l_Dist = 1.5f;

                                for (uint8 l_I = 0; l_I < 3; l_I++)
                                {
                                    g_BeastPos[*l_Iter].SimplePosXYRelocationByAngle(l_PosLeft, l_Dist, M_PI / 3.0f); ///< Left
                                    g_BeastPos[*l_Iter].SimplePosXYRelocationByAngle(l_PosRight, l_Dist, -M_PI / 3.0f); ///< Right

                                    me->SummonCreature(NPC_BEAST_OF_NIGHTMARE, l_PosLeft.GetPositionX(), l_PosLeft.GetPositionY(), l_PosLeft.GetPositionZ(), g_BeastPos[*l_Iter].GetOrientation());
                                    me->SummonCreature(NPC_BEAST_OF_NIGHTMARE, l_PosRight.GetPositionX(), l_PosRight.GetPositionY(), l_PosRight.GetPositionZ(), g_BeastPos[*l_Iter].GetOrientation());

                                    l_Dist += 4;
                                }

                                break;
                            }
                            default:
                            {
                                float l_Dist = 1.5f;

                                for (uint8 l_I = 0; l_I < 3; l_I++)
                                {
                                    g_BeastPos[*l_Iter].SimplePosXYRelocationByAngle(l_PosLeft, l_Dist, M_PI / 2.0f); ///< Left
                                    g_BeastPos[*l_Iter].SimplePosXYRelocationByAngle(l_PosRight, l_Dist, -M_PI / 2.0f); ///< Right

                                    me->SummonCreature(NPC_BEAST_OF_NIGHTMARE, l_PosLeft);
                                    me->SummonCreature(NPC_BEAST_OF_NIGHTMARE, l_PosRight);

                                    l_Dist += 4;
                                }

                                break;
                            }

                            break;
                        }
                    }
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case SPELL_SPEAR_OF_NIGHTMARES:
                    {
                        DoCast(p_Target, SPELL_NIGHTMARE_BLAST_AT, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(35, p_Damage) && !m_SecondPhase)
                {
                    m_SecondPhase = true;

                    DoStopAttack();

                    me->GetMotionMaster()->Clear();

                    events.ScheduleEvent(EVENT_OVERWHELMING_NIGHTMARE, 100);
                    events.ScheduleEvent(EVENT_SPEAR_OF_NIGHTMARES, 21000);

                    EntryCheckPredicate l_Pred(NPC_MALFURION_STORMRAGE);
                    summons.DoAction(ACTION_3, l_Pred);

                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3417, p_Attacker, nullptr, *p_Attacker))
                        delete l_Conversation;
                }

                if (me->HealthBelowPct(65) && !m_FirstConv)
                {
                    m_FirstConv = true;

                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3416, p_Attacker, nullptr, *p_Attacker))
                        delete l_Conversation;

                    events.ScheduleEvent(EVENT_SCENE, 20000);
                }
            }

            void DamageDealt(Unit* /*p_Victim*/, uint32& p_Damage, DamageEffectType /*p_DamageType*/, SpellInfo const* p_SpellInfo) override
            {
                if (!p_SpellInfo)
                    return;

                if (p_SpellInfo->Id == SPELL_SPEAR_OF_NIGHTMARES)
                {
                    float l_BaseDamage = float(p_SpellInfo->Effects[EFFECT_0].CalcValue(me));
                    if (l_BaseDamage == 0)
                        return;

                    float l_DamagePct = float(p_Damage) / float(p_SpellInfo->Effects[EFFECT_0].CalcValue(me));

                    /// 35 yards radius if hit at 100% damage
                    m_SpearRadius = l_DamagePct * 35.0f;
                }
            }

            float GetFData(uint32 p_ID /*= 0*/) const override
            {
                return m_SpearRadius;
            }

            uint32 GetData(uint32 p_ID /*= 0*/) override
            {
                return m_WaveCount;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetDistance(g_CenterPos) > 60.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                if (m_PowerTimer)
                {
                    if (m_PowerTimer <= p_Diff)
                    {
                        m_PowerTimer = 1000;
                        m_PowerCount = me->GetPower(POWER_ENERGY);

                        if (m_PowerCount < 100)
                        {
                            if (m_PowerTick < 2)
                            {
                                m_PowerTick++;
                                me->SetPower(POWER_ENERGY, m_PowerCount + 1);
                            }
                            else
                            {
                                m_PowerTick = 0;
                                me->SetPower(POWER_ENERGY, m_PowerCount + 2);
                            }
                        }
                        else
                        {
                            if (!me->HasUnitState(UNIT_STATE_CASTING))
                                events.ScheduleEvent(EVENT_FORCES_OF_NIGHTMARE, 100);
                        }
                    }
                    else
                        m_PowerTimer -= p_Diff;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_FORCES_OF_NIGHTMARE:
                    {
                        if (!m_SecondPhase && m_WaveCount < 6)
                        {
                            m_WaveCount++;
                            DoCast(SPELL_FORCES_OF_NIGHTMARE);

                            EntryCheckPredicate l_Pred(NPC_MALFURION_STORMRAGE);
                            summons.DoAction(ACTION_2, l_Pred);

                            Talk(SAY_FORCES_OF_NIGHTMARE_EMOTE);
                            Talk(SAY_FORCES_OF_NIGHTMARE);
                        }
                        else
                        {
                            if (Creature* l_Malfurion = me->FindNearestCreature(NPC_MALFURION_STORMRAGE, 100.0f))
                                DoCast(l_Malfurion, SPELL_SUM_ENTANGLING_NIGHTMARES);
                        }

                        break;
                    }
                    case EVENT_NIGHTMARE_BRAMBLES:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, -15.0f, true))
                            DoCast(l_Target, SPELL_NIGHTMARE_BRAMBLES_SUM);
                        Talk(SAY_NIGHTMARE_BRAMBLES);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BRAMBLES, 30000);
                        break;
                    }
                    case EVENT_OVERWHELMING_NIGHTMARE:
                    {
                        DoCast(me, SPELL_OVERWHELMING_NIGHTMARE, true);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BLAST_SAVER, 4000);
                        break;
                    }
                    case EVENT_NIGHTMARE_BLAST_SAVER:
                    {
                        me->RemoveAurasDueToSpell(SPELL_NIGHTMARE_ENERGY);
                        DoCast(me, SPELL_NIGHTMARE_BLAST_SAVER, true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    }
                    case EVENT_SPEAR_OF_NIGHTMARES:
                    {
                        DoCastVictim(SPELL_SPEAR_OF_NIGHTMARES);
                        events.ScheduleEvent(EVENT_SPEAR_OF_NIGHTMARES, 18000);
                        break;
                    }
                    case EVENT_NIGHTMARE_BLAST_OR_BEAST:
                    {
                        if (!m_SecondPhase && m_WaveCount < 6)
                            DoCastVictim(SPELL_NIGHTMARE_BLAST);
                        else
                            DoCast(SPELL_BEASTS_OF_NIGHTMARE);
                        events.ScheduleEvent(EVENT_NIGHTMARE_BLAST_OR_BEAST, 30000);
                        break;
                    }
                    case EVENT_SCENE:
                    {
                        Conversation* l_Conversation = new Conversation;
                        if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3406, me, nullptr, *me))
                            delete l_Conversation;

                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void RemoveEncounterAuras()
            {
                if (InstanceScript* l_Instance = me->GetInstanceScript())
                {
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE_BLAST);
                    l_Instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ANCIENT_DREAM_AURA);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_cenariusAI (p_Creature);
        }
};

/// Nightmare Sapling - 106427
/// Tormented Souls - 106895
/// Corrupted Emerald Egg - 106898
/// Corrupted Nature - 106899
class npc_cenarius_force_summoner : public CreatureScript
{
    public:
        npc_cenarius_force_summoner() : CreatureScript("npc_cenarius_force_summoner") { }

        struct npc_cenarius_force_summonerAI : public ScriptedAI
        {
            npc_cenarius_force_summonerAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            uint64 m_OwnerGuid = 0;
            bool m_Allies;

            uint32 m_WaveCount = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_Allies = false;

                m_OwnerGuid = p_Summoner->GetGUID();

                switch (me->GetEntry())
                {
                    case NPC_NIGHTMARE_SAPLING:
                        DoCast(me, SPELL_GERMINATING, true);
                        events.ScheduleEvent(EVENT_1, 10000);
                        break;
                    case NPC_TORMENTED_SOULS:
                        DoCast(me, SPELL_TORMENTED, true);
                        DoCast(me, SPELL_WISPS_VISUAL_CORRUPTED, true);
                        events.ScheduleEvent(EVENT_2, 10000);
                        break;
                    case NPC_CORRUPTED_EMERALD_EGG:
                        DoCast(me, SPELL_INCUBATING, true);
                        events.ScheduleEvent(EVENT_3, 10000);
                        break;
                    case NPC_CORRUPTED_NATURE:
                        DoCast(me, SPELL_DRYAD_DUMMY, true);
                        DoCast(me, SPELL_DRYAD_VISUAL, true);
                        events.ScheduleEvent(EVENT_4, 10000);
                        break;
                }

                DoCast(me, SPELL_NIGHTMARE_DISSOLVE_IN, true);
                DoCast(me, SPELL_DUDE_TRACKER_AT, true);
                DoCast(me, SPELL_SUMMON_DRAKES_DUMMY, true);
                events.ScheduleEvent(EVENT_5, 10000);

                if (Creature* l_Cenarius = p_Summoner->ToCreature())
                {
                    if (l_Cenarius->IsAIEnabled)
                        m_WaveCount = l_Cenarius->AI()->GetData();
                }
            }

            void SpellHit(Unit* /*p_Target*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_CLEANSED_GROUND)
                {
                    m_Allies = true;

                    if (me->GetEntry() == NPC_TORMENTED_SOULS)
                    {
                        me->RemoveAurasDueToSpell(SPELL_WISPS_VISUAL_CORRUPTED);
                        DoCast(me, SPELL_WISPS_VISUAL_ALLIES, true);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (m_WaveCount == 5)
                        {
                            Position l_Pos = g_ForcesPos[1];

                            for (uint8 l_I = 0; l_I < 2; ++l_I)
                            {
                                g_ForcesPos[1].SimplePosXYRelocationByAngle(l_Pos, frand(3.0f, 10.0f), frand(0.0f, 2.0f * M_PI), true);

                                me->CastSpell(l_Pos, m_Allies ? SPELL_ALLIES_GERMINATE : SPELL_NIGHTMARE_GERMINATE, true, 0, 0, m_OwnerGuid);

                                m_Allies = false;
                            }
                        }
                        else
                            me->CastSpell(me, m_Allies ? SPELL_ALLIES_GERMINATE : SPELL_NIGHTMARE_GERMINATE, true, 0, 0, m_OwnerGuid);

                        break;
                    }
                    case EVENT_2:
                    {
                        Position l_Pos;
                        g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 30.0f, frand(0.0f, 6.0f), true);

                        for (uint8 l_I = 0; l_I < (m_WaveCount == 5 ? 10 : 5); ++l_I)
                        {
                            me->CastSpell(l_Pos, m_Allies ? SPELL_SUM_ALLIES_WISPS : SPELL_SUM_NIGHTMARE_WISPS, true, 0, 0, m_OwnerGuid);

                            if (m_WaveCount == 5 && l_I >= 4 && m_Allies)
                                m_Allies = false;
                        }

                        break;
                    }
                    case EVENT_3:
                    {
                        if (m_WaveCount == 4)
                        {
                            Position l_Pos = g_ForcesPos[3];

                            for (uint8 l_I = 0; l_I < 2; ++l_I)
                            {
                                g_ForcesPos[3].SimplePosXYRelocationByAngle(l_Pos, frand(3.0f, 10.0f), frand(0.0f, 2.0f * M_PI), true);

                                me->CastSpell(l_Pos, m_Allies ? SPELL_SUM_ALLIES_DRAKE : SPELL_SUM_NIGHTMARE_DRAKE, true, 0, 0, m_OwnerGuid);

                                m_Allies = false;
                            }
                        }
                        else
                            me->CastSpell(me, m_Allies ? SPELL_SUM_ALLIES_DRAKE : SPELL_SUM_NIGHTMARE_DRAKE, true, 0, 0, m_OwnerGuid);

                        break;
                    }
                    case EVENT_4:
                    {
                        if (m_WaveCount == 3)
                        {
                            Position l_Pos = g_ForcesPos[2];

                            for (uint8 l_I = 0; l_I < 2; ++l_I)
                            {
                                g_ForcesPos[2].SimplePosXYRelocationByAngle(l_Pos, frand(3.0f, 10.0f), frand(0.0f, 2.0f * M_PI), true);

                                me->CastSpell(l_Pos, m_Allies ? SPELL_SUM_ALLIES_DRYAD : SPELL_SUM_NIGHTMARE_DRYAD, true, 0, 0, m_OwnerGuid);

                                m_Allies = false;
                            }
                        }
                        else
                            me->CastSpell(me, m_Allies ? SPELL_SUM_ALLIES_DRYAD : SPELL_SUM_NIGHTMARE_DRYAD, true, 0, 0, m_OwnerGuid);

                        break;
                    }
                    case EVENT_5:
                    {
                        DoCast(me, SPELL_NIGHTMARE_DISSOLVE_OUT, true);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cenarius_force_summonerAI(p_Creature);
        }
};

/// Nightmare Ancient - 105468
/// Corrupted Wisp - 106304
/// Rotten Drake - 105494
/// Twisted Sister - 105495
class npc_cenarius_forces_of_nightmare : public CreatureScript
{
    public:
        npc_cenarius_forces_of_nightmare() : CreatureScript("npc_cenarius_forces_of_nightmare") { }

        struct npc_cenarius_forces_of_nightmareAI : public ScriptedAI
        {
            npc_cenarius_forces_of_nightmareAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);

                m_FixateTarget = 0;
            }

            InstanceScript* m_Instance;

            bool m_Buffet;          ///< Drake

            uint64 m_FixateTarget;

            void Reset() override { }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (me->GetEntry() != NPC_CORRUPTED_WISP && m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                else if (me->GetEntry() == NPC_CORRUPTED_WISP && IsMythic())
                    DoCast(me, SPELL_CORRUPTED_DETONATE, true);
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat();

                switch (me->GetEntry())
                {
                    case NPC_CORRUPTED_WISP:
                    {
                        DoCast(me, SPELL_FIXATE, true);
                        DoCast(me, SPELL_DESTRUCTIVE_NIGHTMARES, true);
                        me->SetSpeed(MOVE_RUN, 1.0f);
                        break;
                    }
                    case NPC_NIGHTMARE_ANCIENT:
                    {
                        DefaultEvents(50);
                        break;
                    }
                    case NPC_ROTTEN_DRAKE:
                    {
                        m_Buffet = false;
                        DefaultEvents(30);
                        break;
                    }
                    case NPC_TWISTED_SISTER:
                    {
                        DefaultEvents(80);
                        events.ScheduleEvent(EVENT_1, 3000);
                        events.ScheduleEvent(EVENT_2, 11000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DefaultEvents(uint8 p_PowerCount)
            {
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, int32(p_PowerCount));
                DoCast(me, SPELL_DISSOLVE_CLEANSED_IN, true);
                DoCast(me, SPELL_NIGHTMARE_ENERGY, true);
                DoCast(me, SPELL_CAST_HELPFUL_SPELL, true);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case SPELL_ROTTEN_BREATH_FILTER:
                    {
                        if (Creature* l_Rotten = me->FindNearestCreature(NPC_ROTTEN_BREATH_TRIG, 100.0f))
                            DoCast(l_Rotten, SPELL_ROTTEN_BREATH_DMG);

                        break;
                    }
                    case SPELL_FIXATE:
                    {
                        m_FixateTarget = p_Target->GetGUID();
                        events.ScheduleEvent(EVENT_3, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& /*p_Damage*/, SpellInfo const* p_SpellInfo) override
            {
                if (me->GetEntry() == NPC_ROTTEN_DRAKE)
                {
                    if (me->HealthBelowPct(46) && !m_Buffet)
                    {
                        m_Buffet = true;
                        DoCast(me, SPELL_NIGHTMARE_BUFFET, true);
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (!IsMythic())
                    return;

                if (p_SpellInfo->Id == SPELL_MYTHIC_DESICCATING_STOMP)
                    me->CastSpell(me, SPELL_DESICCATING_STOMP);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->GetEntry() != NPC_CORRUPTED_WISP && me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
                            DoCast(l_Target, SPELL_NIGHTMARE_JAVELIN);
                        events.ScheduleEvent(EVENT_2, urand(5, 8) * IN_MILLISECONDS);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(me, SPELL_TWISTED_TOUCH_OF_LIFE_FILTER, true);
                        events.ScheduleEvent(EVENT_3, 12000);
                        break;
                    }
                    case EVENT_3:
                    {
                        if (m_FixateTarget)
                        {
                            if (Unit* l_Target = Unit::GetUnit(*me, m_FixateTarget))
                            {
                                if (l_Target->IsWithinMeleeRange(me))
                                {
                                    DoCast(me, SPELL_DESTRUCTIVE_NIGHTMARES_DMG, true);
                                    m_FixateTarget = 0;
                                    DoCast(me, SPELL_FIXATE, true);
                                }
                            }
                        }

                        events.ScheduleEvent(EVENT_3, 1000);
                        break;
                    }
                    default:
                        break;
                }

                if (me->GetEntry() != NPC_CORRUPTED_WISP)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cenarius_forces_of_nightmareAI(p_Creature);
        }
};

/// Cleansed Ancient - 106667
/// Wisp - 106659
/// Emerald Drake - 106809
/// Redeemed Sister - 106831
class npc_cenarius_allies_of_nature : public CreatureScript
{
    public:
        npc_cenarius_allies_of_nature() : CreatureScript("npc_cenarius_allies_of_nature") { }

        struct npc_cenarius_allies_of_natureAI : public ScriptedAI
        {
            npc_cenarius_allies_of_natureAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
                me->SetReactState(REACT_PASSIVE);
            }

            InstanceScript* m_Instance;
            uint64 m_OwnerGuid = 0;
            uint32 m_Spell = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_OwnerGuid = p_Summoner->GetGUID();

                switch (me->GetEntry())
                {
                    case NPC_ALLIES_WISP:
                    {
                        DoCast(me, SPELL_CLEANSING_DETONATE_VIS, true);
                        events.ScheduleEvent(EVENT_1, 1000);
                        break;
                    }
                    case NPC_CLEANSED_ANCIENT:
                    {
                        m_Spell = SPELL_NATURES_FURY;
                        me->SetMaxPower(POWER_ENERGY, 100);
                        me->SetPower(POWER_ENERGY, 80);
                        DefaultEvents();
                        break;
                    }
                    case NPC_EMERALD_DRAKE:
                    {
                        m_Spell = SPELL_POISON_BLAST;
                        me->SetMaxPower(POWER_ENERGY, 100);
                        me->SetPower(POWER_ENERGY, 100);
                        DefaultEvents();
                        break;
                    }
                    case NPC_REDEEMED_SISTER:
                    {
                        m_Spell = SPELL_JAVELIN_TOSS;
                        me->SetMaxPower(POWER_ENERGY, 100);
                        me->SetPower(POWER_ENERGY, 56);
                        DefaultEvents();
                        break;
                    }
                    default:
                        break;
                }

                if (m_Instance && IsMythic())
                {
                    if (Creature* l_Cenarius = Creature::GetCreature(*me, m_Instance->GetData64(NPC_CENARIUS)))
                    {
                        if (l_Cenarius->IsAIEnabled)
                            l_Cenarius->AI()->SetData(0, me->GetEntry());
                    }
                }
            }

            void DefaultEvents()
            {
                DoCast(me, SPELL_CAST_HELPFUL_SPELL, true);
                DoCast(me, SPELL_DISSOLVE_CLEANSED_IN, true);
                events.ScheduleEvent(EVENT_2, 2000);
                events.ScheduleEvent(EVENT_3, 58000);

                if (m_Instance)
                    m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_CLEANSING_DETONATE_AT)
                    me->DespawnOrUnsummon(500);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_UNBOUND_TOUCH_FILTER)
                    DoCast(p_Target, SPELL_UNBOUND_TOUCH);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || !m_OwnerGuid)
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        me->CastSpell(me, SPELL_CLEANSING_DETONATE_AT, false, 0, 0, m_OwnerGuid);
                        break;
                    }
                    case EVENT_2:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            if (me->GetDistance(l_Owner) > 20.0f)
                            {
                                Position l_Pos;
                                me->GetNearPosition(l_Pos, me->GetDistance(l_Owner) / 2, me->GetRelativeAngle(l_Owner));
                                me->GetMotionMaster()->MovePoint(1, l_Pos);
                            }
                            else
                            {
                                me->SetFacingToObject(l_Owner);
                                DoCast(m_Spell);
                            }
                        }
                        events.ScheduleEvent(EVENT_2, 3600);
                        break;
                    }
                    case EVENT_3:
                    {
                        events.Reset();
                        me->CastStop();
                        DoCast(me, SPELL_CLEANSED_DISSOLVE_OUT, true);

                        if (m_Instance)
                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cenarius_allies_of_natureAI(p_Creature);
        }
};

/// Malfurion Stormrage - 106482
class npc_cenarius_malfurion_stormrage : public CreatureScript
{
    public:
        npc_cenarius_malfurion_stormrage() : CreatureScript("npc_cenarius_malfurion_stormrage") { }

        void DefaultGossip(Player* p_Player, Creature* p_Creature, bool p_WithQuests = false)
        {
            if (p_WithQuests)
            {
                p_Player->PrepareGossipMenu(p_Creature, p_Creature->GetCreatureTemplate()->GossipMenuId, true);
                p_Player->PlayerTalkClass->ClearGossipMenu();
            }
            else
                p_Player->PlayerTalkClass->ClearMenus();

            if (p_Player->GetCharacterWorldStateValue(CharacterWorldStates::EmeraldNightmareCenariusAccessUnlocked) & (1LL << p_Player->GetMap()->GetSpawnMode()))
                p_Player->ADD_GOSSIP_ITEM_DB(MALFURION_MENU_ID, 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());
        }

        bool OnGossipHello(Player* p_Player, Creature* p_Creature) override
        {
            InstanceScript* l_Instance = p_Creature->GetInstanceScript();

            /// No gossip
            if (l_Instance == nullptr)
            {
                DefaultGossip(p_Player, p_Creature, true);
                return true;
            }

            /// To Xavius gossip - Cenarius must be dead, and no encounter in progress
            if (l_Instance->GetBossState(DATA_CENARIUS) == DONE && !l_Instance->IsEncounterInProgress())
            {
                p_Player->PlayerTalkClass->ClearMenus();
                p_Player->ADD_GOSSIP_ITEM_DB(MALFURION_MENU_ID, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                p_Player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, p_Creature->GetGUID());
                return false;
            }

            DefaultGossip(p_Player, p_Creature, true);
            return true;
        }

        bool OnGossipSelect(Player* p_Player, Creature* /*p_Creature*/, uint32 /*p_Sender*/, uint32 p_Action) override
        {
            if (p_Action == GOSSIP_ACTION_INFO_DEF + 2)
                p_Player->NearTeleportTo({ 11370.2627f, -12667.8965f, 486.999f, 4.6431f });
            else
                p_Player->NearTeleportTo({ -2990.38f, -5026.19f, 147.706f, 3.94f });

            return true;
        }

        bool OnQuestComplete(Player* p_Player, Creature* /*p_Creature*/, Quest const* p_Quest) override
        {
            if (p_Quest->GetQuestId() < MALFURION_ACCESS_NM || p_Quest->GetQuestId() > MALFURION_ACCESS_MM)
                return true;

            uint64 l_Value = p_Player->GetCharacterWorldStateValue(CharacterWorldStates::EmeraldNightmareCenariusAccessUnlocked);

            l_Value |= (1LL << p_Player->GetMap()->GetSpawnMode());

            p_Player->SetCharacterWorldState(CharacterWorldStates::EmeraldNightmareCenariusAccessUnlocked, l_Value);
            return true;
        }

        struct npc_cenarius_malfurion_stormrageAI : public ScriptedAI
        {
            npc_cenarius_malfurion_stormrageAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
                m_IntroDone = m_Instance != nullptr ? m_Instance->GetBossState(DATA_NYTHENDRA) == DONE : false;
                m_IntroStart = false;
                m_FirstSay = false;
            }

            InstanceScript* m_Instance;
            uint8 m_PowerTick;
            uint16 m_PowerTimer;
            uint32 m_PowerCount;
            uint32 m_MaxPower;
            bool m_IntroDone;
            bool m_IntroStart;
            bool m_FirstSay;
            bool m_Defeated;
            uint32 m_IntroTimer;

            EventMap m_CosmeticEvents;

            void Reset() override
            {
                me->SetMaxPower(POWER_MANA, 295104);
                m_MaxPower = me->GetMaxPower(POWER_MANA);
                m_PowerCount = 0;
                m_PowerTick = 0;
                m_PowerTimer = 0;
                m_Defeated = false;
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (me->GetPositionZ() <= 590.0f)
                    return;

                if (!p_Who->IsPlayer())
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 70.0f))
                {
                    m_IntroDone = true;
                    Conversation* l_Conversation = new Conversation;
                    if (!l_Conversation->CreateConversation(sObjectMgr->GenerateLowGuid(HighGuid::HIGHGUID_CONVERSATION), 3602, p_Who, nullptr, *p_Who))
                        delete l_Conversation;
                    DoCast(SPELL_PORTAL_IRIS_COSMETIC);
                    m_IntroTimer = 5000;
                    m_IntroStart = true;
                    m_FirstSay = false;
                }
            }

            uint32 GetData(uint32 p_ID /*= 0*/) override
            {
                return m_IntroDone;
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner->GetEntry() == NPC_CENARIUS)
                {
                    me->setRegeneratingHealth(false);
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case ACTION_1:
                        events.ScheduleEvent(EVENT_1, 500);
                        break;
                    case ACTION_2:
                        events.ScheduleEvent(EVENT_2, 3000);
                        break;
                    case ACTION_3:
                        me->SetPower(POWER_MANA, CalculatePct(m_MaxPower, 50));
                        if (m_Instance)
                            m_Instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                        m_PowerTimer = 1000;
                        break;
                    case ACTION_4:
                        events.ScheduleEvent(EVENT_5, 4000); ///< Outro
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case SPELL_CLEANSING_GROUND_FILTER:
                    {
                        DoCast(p_Target, SPELL_CLEANSING_GROUND_AT, true);
                        break;
                    }
                    case SPELL_CLEANSING_OUTRO:
                    {
                        Talk(4);

                        if (Creature* l_Cenarius = me->GetAnyOwner()->ToCreature())
                        {
                            me->AddAura(SPELL_CLEANSING_OUTRO, l_Cenarius);
                            me->InterruptNonMeleeSpells(true);
                        }

                        m_CosmeticEvents.ScheduleEvent(EVENT_7, 10000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (m_Defeated)
                {
                    p_Damage = 0;
                    return;
                }

                /// Malfurion can't die, but he is "disabled" if he receives too much damages
                if (p_Damage >= me->GetHealth() && !m_Defeated)
                {
                    m_Defeated = true;

                    me->InterruptNonMeleeSpells(true);

                    me->SetHealth(1);

                    p_Damage = 0;

                    me->SetUInt32Value(UNIT_FIELD_STATE_ANIM_ID, ANIM_KNEEL_LOOP);

                    m_PowerCount = 0;
                    m_PowerTick = 0;
                    m_PowerTimer = 0;

                    events.Reset();
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_Defeated)
                    return;

                m_CosmeticEvents.Update(p_Diff);

                switch (m_CosmeticEvents.ExecuteEvent())
                {
                    case EVENT_7:
                    {
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                        Talk(5);
                        m_CosmeticEvents.ScheduleEvent(EVENT_8, 10000);
                        break;
                    }
                    case EVENT_8:
                    {
                        Talk(6);
                        m_CosmeticEvents.ScheduleEvent(EVENT_9, 10000);
                        break;
                    }
                    case EVENT_9:
                    {
                        Talk(7);
                        break;
                    }
                    default:
                        break;
                }

                events.Update(p_Diff);

                if (m_PowerTimer)
                {
                    if (m_PowerTimer <= p_Diff)
                    {
                        m_PowerTimer = 1000;
                        m_PowerCount = me->GetPower(POWER_MANA);

                        if (m_PowerCount < m_MaxPower)
                        {
                            if (m_PowerTick < 2)
                            {
                                m_PowerTick++;
                                me->SetPower(POWER_MANA, m_PowerCount + CalculatePct(m_MaxPower, 2));
                            }
                            else
                            {
                                m_PowerTick = 0;
                                me->SetPower(POWER_MANA, m_PowerCount + CalculatePct(m_MaxPower, 1));
                            }
                        }
                        else if (!me->HasUnitState(UNIT_STATE_STUNNED))
                        {
                            events.ScheduleEvent(EVENT_3, 100);
                            me->SetPower(POWER_MANA, 0);
                        }
                    }
                    else
                        m_PowerTimer -= p_Diff;
                }

                if (m_IntroStart) ///< Intro on the start
                {
                    if (m_IntroTimer <= p_Diff)
                    {
                        if (!m_FirstSay)
                        {
                            Talk(0);
                            m_FirstSay = true;
                        }
                        else
                        {
                            Talk(1);
                            m_IntroStart = false;
                        }
                         m_IntroTimer = 16000;
                    }
                    else
                        m_IntroTimer -= p_Diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        if (Unit* l_Owner = me->GetAnyOwner())
                        {
                            if (me->GetDistance(l_Owner) > 20.0f)
                            {
                                Position l_Pos;
                                me->GetNearPosition(l_Pos, me->GetDistance(l_Owner) / 2, me->GetRelativeAngle(l_Owner));
                                me->GetMotionMaster()->MovePoint(1, l_Pos);
                            }
                            else
                            {
                                me->SetFacingToObject(l_Owner);
                                DoCast(SPELL_STORMS_RAGE);
                            }
                        }
                        events.ScheduleEvent(EVENT_1, 2000);
                        break;
                    }
                    case EVENT_2:
                    {
                        DoCast(SPELL_CLEANSING_GROUND_FILTER);
                        Talk(2);
                        break;
                    }
                    case EVENT_3:
                    {
                        DoCast(SPELL_CLEANSING_GROUND_STAGE_2);
                        events.DelayEvents(3000);
                        events.ScheduleEvent(EVENT_4, 100);
                        break;
                    }
                    case EVENT_4:
                    {
                        if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Summoner->IsAIEnabled)
                            {
                                if (Unit* l_Target = l_Summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                                    DoCast(l_Target, SPELL_CLEANSING_GROUND_AT, true);
                            }
                        }

                        break;
                    }
                    case EVENT_5:  ///< Outro
                    {
                        Talk(3);
                        if (Creature* l_Cenarius = me->GetAnyOwner()->ToCreature())
                            me->CastSpell(l_Cenarius, SPELL_CLEANSING_OUTRO);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cenarius_malfurion_stormrageAI(p_Creature);
        }
};

/// Nightmare Brambles - 106167
class npc_cenarius_nightmare_brambles : public CreatureScript
{
    public:
        npc_cenarius_nightmare_brambles() : CreatureScript("npc_cenarius_nightmare_brambles") { }

        struct npc_cenarius_nightmare_bramblesAI : public ScriptedAI
        {
            npc_cenarius_nightmare_bramblesAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetSpeed(MOVE_RUN, 0.5f);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            uint64 m_TargetGuid = 0;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                events.ScheduleEvent(EVENT_1, 1000);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                p_Damage = 0;
            }

            void UpdateAI(uint32 const p_Diff)  override
            {
                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_1:
                    {
                        DoCast(me, SPELL_NIGHTMARE_BRAMBLES_AT, true);
                        DoCast(me, SPELL_NIGHTMARE_BRAMBLES_AT_PER, true);
                        DoCast(me, SPELL_NIGHTMARE_BRAMBLES_UNKNOWN, true);
                        events.ScheduleEvent(EVENT_2, 1000);
                        events.ScheduleEvent(EVENT_3, 2000);
                        break;
                    }
                    case EVENT_2:
                    {
                        events.ScheduleEvent(EVENT_2, 2000);
                        if (m_TargetGuid)
                        {
                            if (Player* l_Player = Player::GetPlayer(*me, m_TargetGuid))
                            {
                                if (l_Player->IsInWorld() && l_Player->isAlive() && l_Player->GetDistance(me) < 100.0f)
                                    break;
                            }
                        }

                        if (Creature* l_Summoner = me->GetAnyOwner()->ToCreature())
                        {
                            if (l_Summoner->IsAIEnabled)
                            {
                                if (Unit* l_Target = l_Summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 1, 80.0f, true))
                                    m_TargetGuid = l_Target->GetGUID();
                            }
                        }

                        break;
                    }
                    case EVENT_3:
                    {
                        me->SendPlaySpellVisualKit(0, BRAMBLES_KIT_1);
                        if (Player* l_Player = Player::GetPlayer(*me, m_TargetGuid))
                        {
                            if (l_Player->IsInWorld())
                                me->GetMotionMaster()->MovePoint(1, *l_Player);
                        }

                        events.ScheduleEvent(EVENT_3, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cenarius_nightmare_bramblesAI(p_Creature);
        }
};

/// Entangling Roots - 108040
class npc_cenarius_entangling_roots : public CreatureScript
{
    public:
        npc_cenarius_entangling_roots() : CreatureScript("npc_cenarius_entangling_roots") { }

        struct npc_cenarius_entangling_rootsAI : public ScriptedAI
        {
            npc_cenarius_entangling_rootsAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISTRACT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_ENTANGLING_NIGHTMARES_VIS, true);
                DoCast(me, SPELL_NIGHTMARE_DISSOLVE_IN, true);
                DoCast(me, SPELL_ENTANGLING_NIGHTMARES_AURA, true);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_ENTANGLING_NIGHTMARES_VEH)
                {
                    p_Target->CastSpell(me, 46598, true); ///< Ride vehicle
                    DoCast(me, SPELL_ENTANGLING_NIGHTMARES_STUN, true);
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Creature* l_Malfurion = me->FindNearestCreature(NPC_MALFURION_STORMRAGE, 30.0f))
                    l_Malfurion->RemoveAurasDueToSpell(SPELL_ENTANGLING_NIGHTMARES_STUN);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cenarius_entangling_rootsAI(p_Creature);
        }
};

/// Beast of Nightmare - 108208
class npc_cenarius_beast_of_nightmare : public CreatureScript
{
    public:
        npc_cenarius_beast_of_nightmare() : CreatureScript("npc_cenarius_beast_of_nightmare") { }

        struct npc_cenarius_beast_of_nightmareAI : public ScriptedAI
        {
            npc_cenarius_beast_of_nightmareAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
            }

            uint16 m_MoveTimer;

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                m_MoveTimer = 2000;
            }

            void SpellHit(Unit* /*p_Caster*/, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_GRIPPING_FOG_AT)
                {
                    Position l_Pos;
                    me->GetNearPosition(l_Pos, 100.0f, 0.0f);
                    me->GetMotionMaster()->MovePoint(1, l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), false);
                }
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_ID) override
            {
                if (p_ID == 1)
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_MoveTimer)
                {
                    if (m_MoveTimer <= p_Diff)
                    {
                        m_MoveTimer = 0;
                        DoCast(me, SPELL_GRIPPING_FOG_AT, true);
                    }
                    else
                        m_MoveTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_cenarius_beast_of_nightmareAI(p_Creature);
        }
};

/// Cleansing Ground - 212630
class spell_cenarius_cleansing_ground : public SpellScriptLoader
{
    public:
        spell_cenarius_cleansing_ground() : SpellScriptLoader("spell_cenarius_cleansing_ground") { }

        class spell_cenarius_cleansing_ground_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_cleansing_ground_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                std::vector<WorldObject*> l_TempCreatures;
                for (WorldObject* l_Creature : p_Targets)
                {
                    if (l_Creature && l_Creature->IsInWorld())
                    {
                        if (l_Creature->GetEntry() == NPC_NIGHTMARE_SAPLING || l_Creature->GetEntry() == NPC_TORMENTED_SOULS ||
                            l_Creature->GetEntry() == NPC_CORRUPTED_EMERALD_EGG || l_Creature->GetEntry() == NPC_CORRUPTED_NATURE)
                            l_TempCreatures.push_back(l_Creature);
                    }
                }

                uint8 l_Counter[4] = { 0, 0, 0, 0 };

                for (WorldObject* l_Player : p_Targets)
                {
                    if (!l_Player->IsPlayer())
                        continue;

                    for (uint8 l_I = 0; l_I < l_TempCreatures.size(); ++l_I)
                    {
                        if (l_TempCreatures[l_I]->GetDistance(l_Player) < 33.0f)
                        {
                            if (l_I < 4)
                                l_Counter[l_I]++;
                        }
                    }
                }

                uint8 l_SaveCount = l_Counter[0];
                uint8 l_SelectID = 0;

                for (uint8 l_I = 1; l_I < 4; ++l_I)
                {
                    if (l_SaveCount < l_Counter[l_I])
                    {
                        l_SaveCount = l_Counter[l_I];
                        l_SelectID = l_I;
                    }
                }

                p_Targets.clear();

                if (l_TempCreatures.empty() || l_TempCreatures.size() <= l_SelectID)
                    return;

                p_Targets.push_back(l_TempCreatures[l_SelectID]);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cenarius_cleansing_ground_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_cleansing_ground_SpellScript();
        }
};

/// Summon Wisps - 212188, 212192
class spell_cenarius_sum_wisp : public SpellScriptLoader
{
    public:
        spell_cenarius_sum_wisp() : SpellScriptLoader("spell_cenarius_sum_wisp") { }

        class spell_cenarius_sum_wisp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_sum_wisp_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                PreventHitDefaultEffect(EFFECT_0);

                Position l_Pos;
                g_CenterPos.SimplePosXYRelocationByAngle(l_Pos, 30.0f, frand(0.0f, 6.0f));

                if (WorldLocation* l_Dest = const_cast<WorldLocation*>(GetExplTargetDest()))
                    l_Dest->Relocate(l_Pos);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_cenarius_sum_wisp_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_sum_wisp_SpellScript();
        }
};

/// Cast Helpful Spell - 211639
class spell_cenarius_allies_periodic_energize : public SpellScriptLoader
{
    public:
        spell_cenarius_allies_periodic_energize() : SpellScriptLoader("spell_cenarius_allies_periodic_energize") { }

        class spell_cenarius_allies_periodic_energize_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_cenarius_allies_periodic_energize_AuraScript);

            uint8 m_PowerCount;
            uint8 m_PowerTick = 0;

            void OnTick(AuraEffect const* aurEff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                m_PowerCount = l_Caster->GetPower(POWER_ENERGY);

                if (m_PowerCount < 100)
                {
                    switch (l_Caster->GetEntry())
                    {
                        case NPC_CLEANSED_ANCIENT:
                        {
                            if (m_PowerTick < 7)
                            {
                                m_PowerTick++;
                                l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 3);
                            }
                            else
                            {
                                m_PowerTick = 0;
                                l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 2);
                            }
                            break;
                        }
                        case NPC_EMERALD_DRAKE:
                        {
                            if (m_PowerTick < 2)
                            {
                                m_PowerTick++;
                                l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 3);
                            }
                            else
                            {
                                m_PowerTick = 0;
                                l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 2);
                            }
                            break;
                        }
                        case NPC_REDEEMED_SISTER:
                        case NPC_NIGHTMARE_ANCIENT:
                        {
                            if (m_PowerTick < 2)
                            {
                                m_PowerTick++;
                                l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 3);
                            }
                            else
                            {
                                m_PowerTick = 0;
                                l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 4);
                            }
                            break;
                        }
                        case NPC_ROTTEN_DRAKE:
                            l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 4);
                            break;
                        case NPC_TWISTED_SISTER:
                            l_Caster->SetPower(POWER_ENERGY, m_PowerCount + 5);
                            break;
                    }
                }
                else if (!l_Caster->HasUnitState(UNIT_STATE_CASTING))
                {
                    switch (l_Caster->GetEntry())
                    {
                        case NPC_CLEANSED_ANCIENT:
                        {
                            l_Caster->CastSpell(l_Caster, SPELL_REPLENISHING_ROOTS);
                            break;
                        }
                        case NPC_EMERALD_DRAKE:
                        {
                            l_Caster->CastSpell(l_Caster, SPELL_ANCIENT_DREAM_FILTER);
                            break;
                        }
                        case NPC_REDEEMED_SISTER:
                        {
                            l_Caster->CastSpell(l_Caster, SPELL_UNBOUND_TOUCH_FILTER, true);
                            break;
                        }
                        case NPC_NIGHTMARE_ANCIENT:
                        {
                            if (l_Caster->GetMap()->IsMythic())
                                l_Caster->CastSpell(l_Caster, SPELL_MYTHIC_DESICCATING_STOMP);
                            else
                                l_Caster->CastSpell(l_Caster, SPELL_DESICCATING_STOMP);

                            break;
                        }
                        case NPC_ROTTEN_DRAKE:
                        {
                            l_Caster->CastSpell(l_Caster, SPELL_ROTTEN_BREATH_FILTER);
                            break;
                        }
                        case NPC_TWISTED_SISTER:
                        {
                            l_Caster->CastSpell(l_Caster, SPELL_SCORNED_TOUCH_FILTER);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_cenarius_allies_periodic_energize_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_cenarius_allies_periodic_energize_AuraScript();
        }
};

/// Ancient Dream - 211935
class spell_cenarius_ancient_dream_filter : public SpellScriptLoader
{
    public:
        spell_cenarius_ancient_dream_filter() : SpellScriptLoader("spell_cenarius_ancient_dream_filter") { }

        class spell_cenarius_ancient_dream_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_ancient_dream_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.clear();

                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Summoner = l_Caster->GetAnyOwner())
                    {
                        if (l_Summoner && l_Summoner->getVictim())
                            p_Targets.push_back(l_Summoner->getVictim());
                    }
                }
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                    GetHitUnit()->CastSpell(GetHitUnit(), SPELL_ANCIENT_DREAM_AURA, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cenarius_ancient_dream_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnHit += SpellHitFn(spell_cenarius_ancient_dream_filter_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_ancient_dream_filter_SpellScript();
        }
};

/// Ancient Dream - 211939
class spell_cenarius_ancient_dream : public SpellScriptLoader
{
    public:
        spell_cenarius_ancient_dream() : SpellScriptLoader("spell_cenarius_ancient_dream") { }

        class spell_cenarius_ancient_dream_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_cenarius_ancient_dream_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                p_Amount = -1;
            }

            void Absorb(AuraEffect* p_AurEff, DamageInfo& p_DamageInfo, uint32& /*p_AbsorbAmount*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (p_DamageInfo.GetAmount() < l_Target->GetHealth())
                        return;

                    l_Target->SetHealth(l_Target->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_1].BasePoints));
                    p_AurEff->GetBase()->Remove();
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_cenarius_ancient_dream_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_cenarius_ancient_dream_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_cenarius_ancient_dream_AuraScript();
        }
};

/// Desiccating Stomp - 211073
class spell_cenarius_desiccating_stomp_filter : public SpellScriptLoader
{
    public:
        spell_cenarius_desiccating_stomp_filter() : SpellScriptLoader("spell_cenarius_desiccating_stomp_filter") { }

        class spell_cenarius_desiccating_stomp_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_desiccating_stomp_filter_SpellScript);

            bool m_TargetsEmpty = false;

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    m_TargetsEmpty = true;
            }

            void OnAfterCast()
            {
                if (GetCaster() && m_TargetsEmpty)
                    GetCaster()->CastSpell(GetCaster(), SPELL_DESICCATION, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cenarius_desiccating_stomp_filter_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_cenarius_desiccating_stomp_filter_SpellScript::OnAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_desiccating_stomp_filter_SpellScript();
        }
};

/// Twisted Touch of Life - 211462
class spell_cenarius_twisted_touch_of_life_filter : public SpellScriptLoader
{
    public:
        spell_cenarius_twisted_touch_of_life_filter() : SpellScriptLoader("spell_cenarius_twisted_touch_of_life_filter") { }

        class spell_cenarius_twisted_touch_of_life_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_twisted_touch_of_life_filter_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                std::list<WorldObject*> l_TempCreatures;

                for (WorldObject* l_Creature : p_Targets)
                {
                    switch (l_Creature->GetEntry())
                    {
                        case NPC_NIGHTMARE_ANCIENT:
                        case NPC_ROTTEN_DRAKE:
                        case NPC_CENARIUS:
                            l_TempCreatures.push_back(l_Creature);
                            break;
                        default:
                            break;
                    }
                }

                l_TempCreatures.sort(JadeCore::UnitHealthState(true));

                if (l_TempCreatures.size() > 1)
                    l_TempCreatures.resize(1);

                if (!l_TempCreatures.empty())
                    p_Targets = l_TempCreatures;
            }

            void HandleOnHit()
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), GetSpellInfo()->Effects[EFFECT_0].BasePoints);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cenarius_twisted_touch_of_life_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnHit += SpellHitFn(spell_cenarius_twisted_touch_of_life_filter_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_twisted_touch_of_life_filter_SpellScript();
        }
};

/// Dread Thorns - 210340
class spell_cenarius_dread_thorns_reflects : public SpellScriptLoader
{
    public:
        spell_cenarius_dread_thorns_reflects() : SpellScriptLoader("spell_cenarius_dread_thorns_reflects") { }

        class spell_cenarius_dread_thorns_reflects_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_cenarius_dread_thorns_reflects_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                if (!GetCaster())
                    return;

                Unit* l_Target = p_EventInfo.GetProcTarget();
                if (!l_Target || !p_EventInfo.GetDamageInfo())
                    return;

                int32 l_Damage = CalculatePct(p_EventInfo.GetDamageInfo()->GetAmount(), p_AurEff->GetAmount());
                if (!l_Damage)
                    return;

                GetCaster()->CastCustomSpell(l_Target, 210342, &l_Damage, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_cenarius_dread_thorns_reflects_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_cenarius_dread_thorns_reflects_AuraScript();
        }
};

/// Scorned Touch - 211472
class spell_cenarius_scorned_touch : public SpellScriptLoader
{
    public:
        spell_cenarius_scorned_touch() : SpellScriptLoader("spell_cenarius_scorned_touch") { }

        class spell_cenarius_scorned_touch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cenarius_scorned_touch_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetHitUnit())
                    {
                        Unit* l_FirstTarget = GetExplTargetUnit();
                        if (!l_FirstTarget || l_FirstTarget->GetGUID() == l_Target->GetGUID())
                            return;

                        l_Caster->CastSpell(l_Target, SPELL_SCORNED_TOUCH_TRIGGER, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_cenarius_scorned_touch_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_cenarius_scorned_touch_SpellScript();
        }
};

/// Nightmares - 214711
class areatrigger_cenarius_nightmares : public AreaTriggerEntityScript
{
    public:
        areatrigger_cenarius_nightmares() : AreaTriggerEntityScript("areatrigger_cenarius_nightmares") { }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                if (!l_Caster->ToCreature() || !l_Caster->ToCreature()->IsAIEnabled)
                    return;

                float l_Radius = l_Caster->ToCreature()->AI()->GetFData();

                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 10000);

                /// Sniffed Values
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.0f);
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.0f);

                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, p_AreaTrigger->GetAreaTriggerInfo().ElapsedTime);
                p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_Radius);

                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
                p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1);
            }
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            float l_MaxRadius  = p_AreaTrigger->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4);
            float l_PctPerMSec = ((l_MaxRadius - 1.0f) * 10.0f) / 1000.0f;
            float l_BaseRadius = p_AreaTrigger->GetAreaTriggerInfo().Radius;
            float l_CurrRadius = p_AreaTrigger->GetRadius();

            l_CurrRadius += CalculatePct(l_BaseRadius, l_PctPerMSec * p_Time);

            p_AreaTrigger->SetRadius(std::min(l_CurrRadius, p_AreaTrigger->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4)));
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_cenarius_nightmares();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_cenarius()
{
    /// Boss
    new boss_cenarius();

    /// Creatures
    new npc_cenarius_force_summoner();
    new npc_cenarius_forces_of_nightmare();
    new npc_cenarius_allies_of_nature();
    new npc_cenarius_malfurion_stormrage();
    new npc_cenarius_nightmare_brambles();
    new npc_cenarius_entangling_roots();
    new npc_cenarius_beast_of_nightmare();

    /// Spells
    new spell_cenarius_cleansing_ground();
    new spell_cenarius_sum_wisp();
    new spell_cenarius_allies_periodic_energize();
    new spell_cenarius_ancient_dream_filter();
    new spell_cenarius_ancient_dream();
    new spell_cenarius_desiccating_stomp_filter();
    new spell_cenarius_twisted_touch_of_life_filter();
    new spell_cenarius_dread_thorns_reflects();
    new spell_cenarius_scorned_touch();

    /// AreaTrigger
    new areatrigger_cenarius_nightmares();
}
#endif
