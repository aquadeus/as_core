////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_nighthold.hpp"

enum eSpells
{
    SpellAstromancerVisual      = 232550,
    SpellEtraeusSpaceNeck       = 220629,
    SpellEtraeusSpaceNeck2      = 220630,
    SpellCoronalEjection        = 205554,
    SpellNetherTraversal        = 221875,
    SpellStarBurst              = 205486,
    SpellCoronalEjectionDmg     = 206464,
    SpellBigBang                = 222761,
    SpellGrandConjuction        = 205408,

    /// Absolute Zero
    SpellIceSkyBox              = 208952,
    SpellGravitationalPull      = 205984,
    SpellCometImpact            = 206628,
    SpellChilled                = 206589,
    SpellAbsoluteZero           = 206585,
    SpellShatter                = 206938,
    SpellFrozenSolid            = 206603,
    SpellIceBurst               = 206921,
    SpellFrigidNova             = 206949,
    SpellFrigidNovaDmg          = 206954,
    SpellIcyEjection            = 206936,
    SpellIceCrystal             = 216696,

    /// Shattered World
    SpellFelSkyBox              = 208955,
    SpellGravitationalPullFel   = 214167,
    SpellFelBurst               = 206388,
    SpellFelEmber               = 216768,
    SpellFelEmberArea           = 216794,
    SpellFelNova                = 206517,
    SpellFelImpact              = 208426,
    SpellFelFlameArea           = 206399,
    SpellFelFlameDot            = 206398,
    SpellFelEjection            = 205649,

    /// Inevitable Fate
    SpellGravitationalPullVoid  = 214335,
    SpellVoidSkyBox             = 208957,
    SpellVoidImpact             = 207768,
    SpellVoidNova               = 207439,
    SpellVoidBurst              = 214486,
    SpellVoidBurstDot           = 206965,
    SpellVoidEruption           = 207145,
    SpellVoidEmpowerement       = 214606,
    SpellVoidEjection           = 207143,
    SpellWorldDevouringForce    = 216909,
    SpellDevouringRemmant       = 217046,
    SpellDevouringRemmantDmg    = 217054,

    /// Star Signs Mythic
    SpellStarSignWolf           = 205445,
    SpellStarSignCrab           = 205429,
    SpellStarSignHunter         = 216345,
    SpellStarSignDragon         = 216344,
    SpellGrandTrine             = 207831,

};

enum ePhases
{
    StartPhase = 1,
    IcePhase,
    FelPhase,
    VoidPhase
};

enum eDataEtraeus
{
    DataPhase           = 1,
    DataConjuctionCast,
    DataGravitationalPull
};

enum eTalks
{
    TalkAggro = 0,
    TalkGrandConjuction,
    TalkIcePhase,
    TalkFelPhase,
    TalkVoidPhase,
    TalkThingThatNotShouldBe,
    TalkKill,
    TalkWipe,
    TalkDeath,
    TalkIcePhaseEnter,
    TalkFelPhaseEnter,
    TalkVoidPhaseEnter,
    TalkVoidPhaseEnter2,
    TalkVoidPhaseEnter3,
};

static const std::array<uint32, 4> g_Signs =
{
    {
        eSpells::SpellStarSignCrab,
        eSpells::SpellStarSignDragon,
        eSpells::SpellStarSignWolf,
        eSpells::SpellStarSignHunter
    }
};

class OrientationCheck : public std::unary_function<Unit*, bool>
{
    public:
        explicit OrientationCheck(Unit* p_Caster) : m_Caster(p_Caster) { }
        bool operator()(WorldObject* p_Object)
        {
            return p_Object->isInBack(m_Caster);
        }

    private:
        Unit* m_Caster;
};

const std::array<Position, 3> g_EyeOfVoidSummonPos =
{
    {
        { 626.786f, 3206.619f, 195.959f },
        { 608.031f, 3214.166f, 195.959f },
        { 636.663f, 3192.314f, 195.959f },
    }
};

class boss_star_augur_etraeus : public CreatureScript
{
    public:
        boss_star_augur_etraeus() : CreatureScript("boss_star_augur_etraeus") { }

        enum eEvents
        {
            EventCoronalEjection    = 1,
            EventStarBurst,
            EventNetherTrasversal,

            EventGravitationalPull,
            EventIceBurst,
            EventIcyEjection,
            EventFrigidNova,

            EventFelBurst,
            EventFelEjection,
            EventFelNova,
            EventFelEmber,

            EventVoidBurst,
            EventVoidEjection,
            EventVoidNova,

            EventGrandConjuction,
            EventVoidlingSpawnPoint,
            EventWorldDevouringForce,
            EventBigBang,
        };


        struct boss_star_augur_etraeusAI : public BossAI
        {
            boss_star_augur_etraeusAI(Creature* p_Creature) : BossAI(p_Creature, eData::DataStarAugurEtraeus)
            {
                DoCast(me, eSpells::SpellAstromancerVisual, true);
                DoCast(me, eSpells::SpellEtraeusSpaceNeck2, true);
                DoCast(me, eSpells::SpellEtraeusSpaceNeck, true);
                me->SetReactState(REACT_DEFENSIVE);
                m_WorldTimer = 0;
            }

            void Reset() override
            {
                me->RemoveAllAreasTrigger();
                me->SetReactState(REACT_DEFENSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                _Reset();
                me->SetHover(true);

                CleanUp();

                m_Phase = ePhases::StartPhase;
                m_Sets = 0;
                m_ConjuctionCount = 0;
                m_WorldTimer = 0;
                m_GravitationalCount = 0;

                JustReachedHome();

                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void EnterEvadeMode() override
            {
                Talk(eTalks::TalkWipe);

                _EnterEvadeMode();
                Reset();

                me->InterruptNonMeleeSpells(true);
                me->Respawn(true, true, 30 * IN_MILLISECONDS);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                Talk(eTalks::TalkAggro);
                _EnterCombat();
                m_ThingTalk = false;
                m_Phase = ePhases::StartPhase;
                m_PreviousPhase = ePhases::StartPhase;

                FillEjectionTimers();

                events.ScheduleEvent(eEvents::EventCoronalEjection, 14 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventStarBurst, IN_MILLISECONDS);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventGrandConjuction, 15 * IN_MILLISECONDS);

                me->RemoveAurasDueToSpell(eSpells::SpellAstromancerVisual);
            }

            void CleanUp()
            {
                if (GameObject* l_Dome = me->FindNearestGameObject(eGameObjects::GoAstromancerDome, 250.f))
                    l_Dome->SetGoState(GO_STATE_READY);

                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellIceSkyBox);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelSkyBox);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellVoidSkyBox);

                me->DespawnCreaturesInArea(eCreatures::NpcThingThatNotShouldBe, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcVoidling, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcIceCrystal, 250.f);
                me->DespawnCreaturesInArea(eCreatures::NpcRemmantOfTheVoid, 250.f);
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 7.2311f;
                p_Modifier /= 1.55f;
            }

            uint32 GetData(uint32 p_Id) override
            {
                if (p_Id == eDataEtraeus::DataPhase)
                    return m_Phase;
                else if (p_Id == eDataEtraeus::DataConjuctionCast)
                    return m_ConjuctionCount;

                return 0;
            }

            void KilledUnit(Unit* p_Victim) override
            {
                if (p_Victim && p_Victim->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                DoCast(me, eSpells::SpellAstromancerVisual, true);
                DoCast(me, eSpells::SpellEtraeusSpaceNeck2, true);
                DoCast(me, eSpells::SpellEtraeusSpaceNeck, true);
                me->SetUInt32Value(UNIT_FIELD_EMOTE_STATE, EMOTE_STATE_CUSTOMSPELL01);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::TalkDeath);
                me->RemoveAllAreasTrigger();
                _JustDied();

                CleanUp();
            }

            void CleanGravitationalPull()
            {
                if (!instance || !instance->instance)
                    return;

                Map::PlayerList const& l_PlayerList = instance->instance->GetPlayers();
                for (Map::PlayerList::const_iterator l_Itr = l_PlayerList.begin(); l_Itr != l_PlayerList.end(); ++l_Itr)
                {
                    Player* l_Target = l_Itr->getSource();

                    if (l_Target == nullptr)
                        continue;

                    l_Target->RemoveAurasDueToSpell(eSpells::SpellGravitationalPull, 0, 0, AuraRemoveMode::AURA_REMOVE_NONE);
                    l_Target->RemoveAurasDueToSpell(eSpells::SpellGravitationalPullFel, 0, 0, AuraRemoveMode::AURA_REMOVE_NONE);
                }
            }

            void HandleAstroDome(bool p_Open)
            {
                if (GameObject* l_Dome = me->FindNearestGameObject(eGameObjects::GoAstromancerDome, 250.f))
                    l_Dome->SetGoState(p_Open ? GO_STATE_ACTIVE : GO_STATE_READY);
            }

            void OnSpellCasted(SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellNetherTraversal)
                {
                    m_ConjuctionCount = 0;
                    HandleAstroDome(true);

                    switch (m_PreviousPhase)
                    {
                        case ePhases::IcePhase:
                        {
                            m_Phase = ePhases::IcePhase;
                            Talk(TalkIcePhaseEnter);
                            instance->DoCastSpellOnPlayers(eSpells::SpellIceSkyBox);

                            events.ScheduleEvent(eEvents::EventGravitationalPull, 30 * IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventCoronalEjection, GetTimerForEjection() * IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventIceBurst, IN_MILLISECONDS);

                            if (IsHeroicOrMythic())
                                events.ScheduleEvent(eEvents::EventFrigidNova, 48 * IN_MILLISECONDS);

                            if (IsMythic())
                                PrepareNextConjuction();

                            break;
                        }

                        case ePhases::FelPhase:
                        {
                            m_Phase = ePhases::FelPhase;
                            Talk(eTalks::TalkFelPhaseEnter);

                            CleanGravitationalPull();
                            me->DespawnCreaturesInArea(eCreatures::NpcIceCrystal, 250.f);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellAbsoluteZero);
                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellIceSkyBox);
                            instance->DoCastSpellOnPlayers(eSpells::SpellFelSkyBox);

                            events.ScheduleEvent(eEvents::EventGravitationalPull, 30 * IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventCoronalEjection, GetTimerForEjection() * IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventFelBurst, IN_MILLISECONDS);


                            if (IsHeroicOrMythic())
                                events.ScheduleEvent(eEvents::EventFelNova, 58 * IN_MILLISECONDS);

                            if (IsMythic())
                            {
                                events.ScheduleEvent(eEvents::EventFelEmber, 5 * IN_MILLISECONDS);
                                PrepareNextConjuction();
                            }

                            break;
                        }

                        case ePhases::VoidPhase:
                        {
                            m_Phase = ePhases::VoidPhase;

                            Talk(eTalks::TalkVoidPhaseEnter);
                            DelayTalk(8, eTalks::TalkVoidPhaseEnter2);
                            DelayTalk(16, eTalks::TalkVoidPhaseEnter3);

                            CleanGravitationalPull();
                            me->RemoveAreaTrigger(eSpells::SpellFelFlameArea);

                            instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SpellFelSkyBox);
                            instance->DoCastSpellOnPlayers(eSpells::SpellVoidSkyBox);

                            events.ScheduleEvent(eEvents::EventCoronalEjection, 15 * IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventVoidBurst, IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventGravitationalPull, 20 * IN_MILLISECONDS);
                            events.ScheduleEvent(eEvents::EventBigBang, 195 * IN_MILLISECONDS);

                            if (IsHeroicOrMythic())
                                events.ScheduleEvent(eEvents::EventVoidNova, 40 * IN_MILLISECONDS);

                            if (IsMythic())
                            {
                                events.ScheduleEvent(eEvents::EventWorldDevouringForce, 16 * IN_MILLISECONDS);
                                PrepareNextConjuction();
                                events.ScheduleEvent(eEvents::EventVoidlingSpawnPoint, 8 * IN_MILLISECONDS);
                            }

                            break;
                        }

                        default : break;
                    }
                }
            }

            void SpellHit(Unit* p_Target, SpellInfo const* p_Spell) override
            {
                if (p_Spell == nullptr || p_Target == nullptr)
                    return;

                if (p_Spell->Id == eSpells::SpellCometImpact)
                {
                    m_GravitationalCount++;

                    if (m_GravitationalCount < 2)
                        DoCast(p_Target, eSpells::SpellGravitationalPull, true);
                    else
                        m_GravitationalCount = 0;
                }
            }

            void DamageTaken(Unit* /**/, uint32 & /**/, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(90) && m_PreviousPhase == ePhases::StartPhase)
                {
                    m_PreviousPhase = ePhases::IcePhase;

                    events.CancelEvent(eEvents::EventCoronalEjection);
                    events.CancelEvent(eEvents::EventStarBurst);
                    Talk(eTalks::TalkIcePhase);
                    events.ScheduleEvent(eEvents::EventNetherTrasversal, IN_MILLISECONDS);
                }
                else if (HealthBelowPct(60) && m_PreviousPhase == ePhases::IcePhase)
                {
                    m_PreviousPhase = ePhases::FelPhase;

                    Talk(eTalks::TalkFelPhase);
                    events.CancelEvent(eEvents::EventIceBurst);
                    events.CancelEvent(eEvents::EventFrigidNova);
                    events.CancelEvent(eEvents::EventCoronalEjection);
                    events.CancelEvent(eEvents::EventGravitationalPull);
                    events.ScheduleEvent(eEvents::EventNetherTrasversal, IN_MILLISECONDS);
                }
                else if (HealthBelowPct(30) && m_PreviousPhase == ePhases::FelPhase)
                {
                    m_PreviousPhase = ePhases::VoidPhase;

                    Talk(eTalks::TalkVoidPhase);
                    events.CancelEvent(eEvents::EventFelBurst);
                    events.CancelEvent(eEvents::EventFelNova);
                    events.CancelEvent(eEvents::EventCoronalEjection);
                    events.CancelEvent(eEvents::EventGravitationalPull);
                    events.CancelEvent(eEvents::EventFelEmber);

                    events.ScheduleEvent(eEvents::EventNetherTrasversal, IN_MILLISECONDS);
                }
            }

            void JustSummoned(Creature* p_Summon) override
            {
                if (p_Summon->GetEntry() == eCreatures::NpcCoronalEjection)
                {
                    switch (m_Phase)
                    {
                    case ePhases::StartPhase:
                    {
                        p_Summon->CastSpell(p_Summon, eSpells::SpellCoronalEjectionDmg, false);
                        break;
                    }

                    case ePhases::IcePhase:
                    {
                        p_Summon->CastSpell(p_Summon, eSpells::SpellIcyEjection, false);
                        break;
                    }

                    case ePhases::FelPhase:
                    {
                        p_Summon->CastSpell(p_Summon, eSpells::SpellFelEjection, false);
                        break;
                    }

                    case ePhases::VoidPhase:
                    {
                        p_Summon->CastSpell(p_Summon, eSpells::SpellVoidEjection, false);
                        break;
                    }

                    default: break;
                    }
                }
                else if (p_Summon->GetEntry() == eCreatures::NpcThingThatNotShouldBe && !m_ThingTalk)
                {
                    Talk(eTalks::TalkThingThatNotShouldBe);
                    m_ThingTalk = true;
                }
            }

            void PrepareNextConjuction()
            {
                uint32 l_Timer;

                switch (m_Phase)
                {
                    case ePhases::StartPhase:
                    {
                        events.ScheduleEvent(eEvents::EventGrandConjuction, 15 * IN_MILLISECONDS);
                        break;
                    }

                    case ePhases::IcePhase:
                    {
                        events.ScheduleEvent(eEvents::EventGrandConjuction, (30 + (15 * m_ConjuctionCount)) * IN_MILLISECONDS);
                        break;
                    }

                    case ePhases::FelPhase:
                    {
                        l_Timer = 60;

                        if (m_ConjuctionCount == 1)
                            l_Timer = 45;
                        else if (m_ConjuctionCount >= 2)
                            l_Timer = 40;
                        
                        events.ScheduleEvent(eEvents::EventGrandConjuction, l_Timer * IN_MILLISECONDS);
                        break;
                    }

                    case ePhases::VoidPhase:
                    {
                        l_Timer = 50;

                        if (m_ConjuctionCount >= 1)
                            l_Timer = 60;

                        events.ScheduleEvent(eEvents::EventGrandConjuction, l_Timer * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void FillEjectionTimers()
            {
                while (!m_IcyEjectionTimers.empty())
                    m_IcyEjectionTimers.pop();

                while (!m_FelEjectionTimers.empty())
                    m_FelEjectionTimers.pop();

                for (uint8 l_Itr : {23, 35, 7, 6, 50, 2, 2, 23, 35, 7, 6, 50, 2, 2} )
                    m_IcyEjectionTimers.push(l_Itr);

                for (uint8 l_Itr : {18, 4, 2, 13, 4, 4, 32, 5, 3, 3, 22, 5, 16, 3, 3})
                    m_FelEjectionTimers.push(l_Itr);
            }

            uint8 GetTimerForEjection()
            {
                uint8 l_Timer = 0;

                if (m_Phase == ePhases::FelPhase)
                {
                    l_Timer = m_FelEjectionTimers.front();
                    m_FelEjectionTimers.pop();
                    m_FelEjectionTimers.push(l_Timer);
                }
                else if (m_Phase == ePhases::IcePhase)
                {
                    l_Timer = m_IcyEjectionTimers.front();
                    m_IcyEjectionTimers.pop();
                    m_IcyEjectionTimers.push(l_Timer);
                }
                else
                    l_Timer = 30;

                return l_Timer;
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventCoronalEjection:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                        {
                            DoCast(l_Target, eSpells::SpellCoronalEjection);
                            m_Sets++;
                        }

                        if (m_Sets >= 2)
                        {
                            m_Sets = 0;
                            events.ScheduleEvent(eEvents::EventCoronalEjection, GetTimerForEjection() * IN_MILLISECONDS);
                        }
                        else
                            events.ScheduleEvent(eEvents::EventCoronalEjection, 2 * IN_MILLISECONDS);

                        break;
                    }

                    case eEvents::EventStarBurst:
                    {
                        DoCastVictim(eSpells::SpellStarBurst);
                        events.ScheduleEvent(eEvents::EventStarBurst, 2 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventNetherTrasversal:
                    {
                        HandleAstroDome(false);
                        DoCast(me, eSpells::SpellNetherTraversal);
                        break;
                    }

                    case eEvents::EventIceBurst:
                    {
                        DoCastVictim(eSpells::SpellIceBurst);
                        events.ScheduleEvent(eEvents::EventIceBurst, 2 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventGravitationalPull:
                    {
                        if (m_Phase == ePhases::FelPhase && me->getVictim() != nullptr)
                            me->CastCustomSpell(eSpells::SpellGravitationalPullFel, SPELLVALUE_AURA_STACK, 5, me->getVictim(), true);
                        else if (m_Phase == ePhases::IcePhase && me->getVictim() != nullptr)
                            me->CastCustomSpell(eSpells::SpellGravitationalPull, SPELLVALUE_AURA_STACK, 3, me->getVictim(), true);
                        else
                            DoCastVictim(eSpells::SpellGravitationalPullVoid);

                        events.ScheduleEvent(eEvents::EventGravitationalPull, (m_Phase == ePhases::VoidPhase ? 63 : 29) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFrigidNova:
                    {
                        DoCastAOE(eSpells::SpellFrigidNova);
                        events.ScheduleEvent(eEvents::EventFrigidNova, 61 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelBurst:
                    {
                        DoCastVictim(eSpells::SpellFelBurst);
                        events.ScheduleEvent(eEvents::EventFelBurst, 2 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelNova:
                    {
                        DoCastAOE(eSpells::SpellFelNova);
                        events.ScheduleEvent(eEvents::EventFelNova, 45 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventFelEmber:
                    {
                        DoCast(me, eSpells::SpellFelEmber, true);
                        events.ScheduleEvent(eEvents::EventFelEmber, urand(8, 12) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventVoidBurst:
                    {
                        DoCastVictim(eSpells::SpellVoidBurst);
                        events.ScheduleEvent(eEvents::EventVoidBurst, 1500);
                        break;
                    }

                    case eEvents::EventVoidNova:
                    {
                        DoCastAOE(eSpells::SpellVoidNova);
                        events.ScheduleEvent(eEvents::EventVoidNova, 74 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventGrandConjuction:
                    {
                        if (m_ConjuctionCount >= 3)
                            return;

                        Talk(eTalks::TalkGrandConjuction);
                        DoCastAOE(eSpells::SpellGrandConjuction);
                        m_ConjuctionCount++;
                        PrepareNextConjuction();
                        break;
                    }

                    case eEvents::EventWorldDevouringForce:
                    {
                        me->SummonCreature(eCreatures::NpcEyeOfTheVoid, g_EyeOfVoidSummonPos[urand(0, 2)], TEMPSUMMON_TIMED_DESPAWN, 5000);
                        events.ScheduleEvent(eEvents::EventWorldDevouringForce, (42 + m_WorldTimer) * IN_MILLISECONDS);

                        if (m_WorldTimer < 14)
                            m_WorldTimer += 15;

                        break;
                    }

                    case eEvents::EventBigBang:
                    {
                        DoCastAOE(eSpells::SpellBigBang);
                        break;
                    }

                    default : break;
                }
            }

            private:
                uint8 m_Sets, m_GravitationalCount, m_ConjuctionCount, m_WorldTimer;
                ePhases m_Phase, m_PreviousPhase;
                bool m_ThingTalk;
                std::queue<uint8> m_IcyEjectionTimers, m_FelEjectionTimers;
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_star_augur_etraeusAI(p_Creature);
        }
};

/// Ice Crystal - 109003
class npc_nh_etraeus_ice_crystal : public CreatureScript
{
    public:
        npc_nh_etraeus_ice_crystal() : CreatureScript("npc_nh_etraeus_ice_crystal")
        {}

        struct npc_nh_etraeus_ice_crystal_AI : public ScriptedAI
        {
            explicit npc_nh_etraeus_ice_crystal_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nh_etraeus_ice_crystal_AI(p_Me);
        }
};

/// Eye of the Void - 109088
class npc_nh_etraeus_eye_of_the_void : public CreatureScript
{
    public:
        npc_nh_etraeus_eye_of_the_void() : CreatureScript("npc_nh_etraeus_eye_of_the_void")
        {}

        struct npc_nh_etraeus_eye_of_the_void_AI : public ScriptedAI
        {
            explicit npc_nh_etraeus_eye_of_the_void_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                if (p_Summoner == nullptr)
                    return;

                me->SetFacingToObject(p_Summoner);
                DoCast(me, eSpells::SpellWorldDevouringForce);
            }

            void EnterEvadeMode() override
            {
                /*Empty*/
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nh_etraeus_eye_of_the_void_AI(p_Me);
        }
};

/// Voidling -  104688
class npc_nh_etraeus_voidling : public CreatureScript
{
    public:
        npc_nh_etraeus_voidling() : CreatureScript("npc_nh_etraeus_voidling")
        {}

        struct npc_nh_etraeus_voidling_AI : public ScriptedAI
        {
            explicit npc_nh_etraeus_voidling_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {}

            void Reset() override
            {
                if (IsMythic())
                    me->UpdateStatsForLevel();
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 1.3325f;
                p_Modifier /= 1.55f;
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nh_etraeus_voidling_AI(p_Me);
        }
};

/// Thing that not should be - 104880
class npc_nh_etraeus_thing_that_not_should_be : public CreatureScript
{
    public:
        npc_nh_etraeus_thing_that_not_should_be() : CreatureScript("npc_nh_etraeus_thing_that_not_should_be")
        {}

        enum eEvents
        {
            EventWitnessOfVoid  = 1,
            EventCheckBoss,
        };

        enum eSpells
        {
            SpellWitnessTheVoid = 207720,
            SpellVoidShiftAura  = 207714,
        };

        struct npc_nh_etraeus_thing_that_not_should_be_AI : public ScriptedAI
        {
            explicit npc_nh_etraeus_thing_that_not_should_be_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
            }

            void GetMythicHealthMod(float& p_Modifier) override
            {
                p_Modifier = 6.0846f;
                p_Modifier /= 1.55f;
            }

            void EnterCombat(Unit* /**/) override
            {
                DoZoneInCombat();
                events.ScheduleEvent(eEvents::EventWitnessOfVoid, 6 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventCheckBoss, 2 * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();

                if (IsMythic())
                    me->UpdateStatsForLevel();

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                if (p_EventId == eEvents::EventWitnessOfVoid)
                {
                    DoCast(me, eSpells::SpellWitnessTheVoid);
                    events.ScheduleEvent(eEvents::EventWitnessOfVoid, 15 * IN_MILLISECONDS);
                }
                else if (p_EventId == eEvents::EventCheckBoss)
                {
                    InstanceScript* l_Instance = me->GetInstanceScript();
                    if (!l_Instance)
                        return;

                    if (Unit* l_Boss = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::BossStarAugurEtraeus)))
                    {
                        if (me->GetDistance2d(l_Boss) < 9.5f)
                        {
                            DoCast(me, eSpells::SpellVoidShiftAura, true);
                            DoCast(l_Boss, eSpells::SpellVoidShiftAura, true);
                        }
                        else
                        {
                            me->RemoveAurasDueToSpell(eSpells::SpellVoidShiftAura);
                            l_Boss->RemoveAurasDueToSpell(eSpells::SpellVoidShiftAura, me->GetGUID());
                        }
                    }

                    events.ScheduleEvent(eEvents::EventCheckBoss, 500);
                }
            }

            void JustDied(Unit* /**/) override
            {
                Creature* l_Boss = me->FindNearestCreature(eCreatures::BossStarAugurEtraeus, 250.f, true);

                if (l_Boss == nullptr)
                    return;

                l_Boss->RemoveAurasDueToSpell(eSpells::SpellVoidShiftAura, me->GetGUID());
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_nh_etraeus_thing_that_not_should_be_AI(p_Me);
        }
};

/// Remmant of the Void - 109151
class npc_nh_etraeus_remmant_of_the_void : public CreatureScript
{
  public:
    npc_nh_etraeus_remmant_of_the_void() : CreatureScript("npc_nh_etraeus_remmant_of_the_void")
    {}

    enum eEvents
    {
        EventDevouringRemmant = 1,
        EventRestoreMovement,
    };

    enum ePoints
    {
        PointRandomNearPos
    };

    struct npc_nh_etraeus_remmant_of_the_void_AI : public ScriptedAI
    {
        explicit npc_nh_etraeus_remmant_of_the_void_AI(Creature* p_Me) : ScriptedAI(p_Me)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
        }

        void IsSummonedBy(Unit* /**/) override
        {
            events.ScheduleEvent(eEvents::EventRestoreMovement, 250);
            events.ScheduleEvent(eEvents::EventDevouringRemmant, 6 * IN_MILLISECONDS);
        }

        void OnSpellFinished(SpellInfo const* p_Spell) override
        {
            if (p_Spell == nullptr)
                return;

            if (p_Spell->Id == eSpells::SpellDevouringRemmantDmg)
                events.RescheduleEvent(eEvents::EventRestoreMovement, 4 * IN_MILLISECONDS);
        }

        void MovementInform(uint32 p_Type, uint32 p_Id) override
        {
            if (p_Type != POINT_MOTION_TYPE)
                return;

            if (p_Id == ePoints::PointRandomNearPos)
                events.ScheduleEvent(eEvents::EventRestoreMovement, 100);
        }

        void JustDied(Unit* /**/) override
        {
            events.Reset();
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            UpdateOperations(p_Diff);

            events.Update(p_Diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 l_EventId = events.ExecuteEvent())
            {
                if (l_EventId == eEvents::EventDevouringRemmant)
                {
                    events.CancelEvent(eEvents::EventRestoreMovement);
                    me->GetMotionMaster()->Clear();
                    me->StopMoving();

                    InstanceScript* l_Instance = me->GetInstanceScript();

                    if (l_Instance != nullptr)
                    {
                        Unit* l_Boss = sObjectAccessor->GetUnit(*me, l_Instance->GetData64(eCreatures::BossStarAugurEtraeus));

                        if (!l_Boss || !l_Boss->IsAIEnabled)
                            return;

                        if (Unit* l_Target = l_Boss->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                        {
                            me->StopMoving();
                            me->AddUnitState(UNIT_STATE_ROOT);
                            me->SetFacingToObject(l_Target);
                            DoCast(l_Target, eSpells::SpellDevouringRemmant);
                        }

                        events.ScheduleEvent(eEvents::EventDevouringRemmant, 14 * IN_MILLISECONDS);
                    }

                }
                else if (l_EventId == eEvents::EventRestoreMovement)
                {
                    me->ClearUnitState(UNIT_STATE_ROOT);
                    Position l_Pos;
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetRandomNearPosition(l_Pos, frand(5.f, 12.5f));
                    me->GetMotionMaster()->MovePoint(ePoints::PointRandomNearPos, l_Pos);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* p_Me) const override
    {
        return new npc_nh_etraeus_remmant_of_the_void_AI(p_Me);
    }
};

/// Gravitational Pull - 205984, 214167
class spell_nh_etraeus_gravitational_pull : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_gravitational_pull() : SpellScriptLoader("spell_nh_etraeus_gravitational_pull")
        {}

        class spell_gravitational_pull_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gravitational_pull_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (!GetCaster() || !GetUnitOwner())
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();
                UnitAI* l_AI = GetCaster()->GetAI();

                if (l_AI == nullptr)
                    return;

                switch (GetId())
                {
                    case eSpells::SpellGravitationalPull:
                    {
                        if (l_AI->GetData(eDataEtraeus::DataPhase) != ePhases::IcePhase)
                            return;

                        l_Caster->CastSpell(l_Owner, eSpells::SpellCometImpact, true);
                        break;
                    }

                    case eSpells::SpellGravitationalPullFel:
                    {
                        if (l_AI->GetData(eDataEtraeus::DataPhase) != ePhases::FelPhase)
                            return;

                        l_Caster->CastSpell(l_Owner, eSpells::SpellFelImpact, true);
                        break;
                    }

                    case eSpells::SpellGravitationalPullVoid:
                    {
                        if (l_AI->GetData(eDataEtraeus::DataPhase) != ePhases::VoidPhase)
                            return;

                        l_Caster->CastSpell(l_Owner, eSpells::SpellVoidImpact, true);
                        break;
                    }

                    default: break;
                }
            }

            void HandleOnRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (!GetCaster() || !GetTargetApplication())
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AuraRemoveMode::AURA_REMOVE_NONE)
                    return;

                if (GetStackAmount() > 1)
                    GetCaster()->CastCustomSpell(GetId(), SPELLVALUE_AURA_STACK, GetStackAmount() - 1,GetUnitOwner(), true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gravitational_pull_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gravitational_pull_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gravitational_pull_AuraScript();
        }
};

/// Icy Ejection - 206936
class spell_nh_etraeus_icy_ejection : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_icy_ejection() : SpellScriptLoader("spell_nh_etraeus_icy_ejection")
        {}

        class spell_icy_ejection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icy_ejection_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject*& p_Itr)
                {
                    return !p_Itr->IsPlayer() || !p_Itr->ToUnit() || p_Itr->ToUnit()->HasAura(eSpells::SpellIcyEjection);
                });

                TankFilter Cmp;

                p_Targets.sort([&Cmp](WorldObject*& p_Itr, WorldObject*& p_Itr2)
                {
                    return !Cmp(p_Itr->ToPlayer()) && Cmp(p_Itr2->ToPlayer());
                });

                if (p_Targets.size() > 3)
                    p_Targets.resize(3);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_icy_ejection_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_icy_ejection_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_icy_ejection_SpellScript::HandleTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_icy_ejection_SpellScript::HandleTargets, EFFECT_3, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        class spell_icy_ejection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_icy_ejection_AuraScript);

            void HandleOnRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (!GetUnitOwner())
                    return;

                GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellShatter, true);
            }

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (!GetAura())
                    return;

                Aura* l_Aura = GetAura();

                if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_2))
                    l_Effect->SetAmount(l_Effect->GetAmount() - 20);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_icy_ejection_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectRemove += AuraEffectRemoveFn(spell_icy_ejection_AuraScript::HandleOnRemove, EFFECT_3, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_icy_ejection_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_icy_ejection_AuraScript();
        }
};

/// Shatter - 206938
class spell_nh_etraeus_shatter : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_shatter() : SpellScriptLoader("spell_nh_etraeus_shatter")
        {}

        class spell_shatter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shatter_SpellScript);

            void HandleOnCast()
            {
                if (!GetCaster())
                    return;

                Unit* l_Caster = GetCaster();

                if (l_Caster->GetMap() && l_Caster->GetMap()->IsMythic())
                {
                    InstanceScript* l_Instance = l_Caster->GetInstanceScript();

                    if (l_Instance == nullptr)
                        return;

                    uint64 l_Guid = l_Instance->GetData64(eCreatures::BossStarAugurEtraeus);

                    if (Unit* l_Boss = sObjectAccessor->GetUnit(*l_Caster, l_Guid))
                    {
                        auto* l_Ptr = l_Boss->SummonCreature(eCreatures::NpcIceCrystal, l_Caster->GetPosition(), TEMPSUMMON_TIMED_DESPAWN, 90 * IN_MILLISECONDS);

                        if (l_Ptr != nullptr)
                            l_Ptr->GetMotionMaster()->MoveRandom(frand(5, 15));
                    }
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_shatter_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_shatter_SpellScript();
        }
};

/// Fel Ejection - 205649
class spell_nh_etraeus_fel_ejection : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_fel_ejection() : SpellScriptLoader("spell_nh_etraeus_fel_ejection")
        {}

        class spell_fel_ejection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fel_ejection_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject*& p_Itr)
                {
                    return !p_Itr->IsPlayer() || !p_Itr->ToUnit() || p_Itr->ToUnit()->HasAura(eSpells::SpellFelEjection);
                });

                TankFilter Cmp;

                if (p_Targets.empty())
                    return;

                p_Targets.sort([&Cmp](WorldObject*& p_Itr, WorldObject*& p_Itr2)
                 {
                    return !Cmp(p_Itr->ToPlayer()) && Cmp(p_Itr2->ToPlayer());
                });

                if (p_Targets.size() > 3)
                    p_Targets.resize(3);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fel_ejection_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fel_ejection_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        class spell_fel_ejection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_fel_ejection_AuraScript);

            void HandleOnPeriodic(AuraEffect const* /**/)
            {
                if (!GetUnitOwner())
                    return;

                Unit* l_Owner = GetUnitOwner();

                InstanceScript* l_Instance = l_Owner->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                if (Unit* l_Etraeus = sObjectAccessor->GetUnit(*l_Owner, l_Instance->GetData64(eCreatures::BossStarAugurEtraeus)))
                {
                    if (l_Etraeus->IsAIEnabled)
                    {
                        if (l_Etraeus->GetAI()->GetData(eDataEtraeus::DataPhase) == ePhases::FelPhase)
                            l_Etraeus->CastSpell(l_Owner, eSpells::SpellFelFlameArea, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_fel_ejection_AuraScript::HandleOnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fel_ejection_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_fel_ejection_AuraScript();
        }
};

/// Void Ejection - 207143
class spell_nh_etraeus_void_ejection : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_void_ejection() : SpellScriptLoader("spell_nh_etraeus_void_ejection")
        {}

        class spell_void_ejection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_void_ejection_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject*& p_Itr)
                {
                    return !p_Itr->IsPlayer();
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_void_ejection_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_void_ejection_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        class spell_void_ejection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_void_ejection_AuraScript);

            void HandleOnRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (!GetUnitOwner())
                    return;

                InstanceScript* l_Instance = GetUnitOwner()->GetInstanceScript();

                if (l_Instance == nullptr)
                    return;

                Unit* l_Boss = sObjectAccessor->GetUnit(*GetUnitOwner(), l_Instance->GetData64(eCreatures::BossStarAugurEtraeus));

                if (l_Boss == nullptr)
                    return;

                if (!l_Boss->isInCombat())
                    return;

                l_Boss->CastSpell(GetUnitOwner(), eSpells::SpellVoidEruption, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_void_ejection_AuraScript::HandleOnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_void_ejection_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_void_ejection_AuraScript();
        }
};

/// Void Burst - 206965
class spell_nh_etraeus_void_burst : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_void_burst() : SpellScriptLoader("spell_nh_etraeus_void_burst")
        {}

        class spell_void_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_void_burst_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                Unit* l_Owner = GetUnitOwner();
                Unit* l_Caster = GetCaster();

                if (!l_Owner || !l_Caster)
                    return;

                if (InstanceScript* l_Instance = l_Owner->GetInstanceScript())
                {
                    if (l_Instance->GetBossState(eData::DataStarAugurEtraeus) != IN_PROGRESS)
                        return;
                }

                std::list<Player*> l_Targets;

                l_Owner->GetPlayerListInGrid(l_Targets, 100.f);

                if (l_Targets.empty())
                    return;

                l_Targets.remove_if([](Player*& p_Itr)
                {
                    return p_Itr == nullptr || p_Itr->isDead();
                });

                if (l_Targets.empty())
                    return;

                if (l_Targets.size() > 2)
                    JadeCore::Containers::RandomResizeList(l_Targets, 2);

                uint8 l_Stacks = GetStackAmount() >= 2 ? GetStackAmount() / 2 : 0;

                if (l_Stacks == 0)
                    return;

                for (Player* l_Itr : l_Targets)
                {
                    if (l_Itr == nullptr || l_Itr->isDead())
                        continue;

                    if (!l_Caster->isDead())
                        l_Caster->CastCustomSpell(eSpells::SpellVoidBurstDot, SPELLVALUE_AURA_STACK, l_Stacks, l_Itr, true);
                    else
                        return;
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_void_burst_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_void_burst_AuraScript();
        }
};

/// Fel Impact - 206433
class spell_nh_etraeus_fel_impact : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_fel_impact() : SpellScriptLoader("spell_nh_etraeus_fel_impact")
        {}

        class spell_fel_impact_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fel_impact_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetExplTargetUnit())
                    return;

                Unit* l_Target = GetExplTargetUnit();

                std::list<AreaTrigger*> l_Patches;

                l_Target->GetAreaTriggerListWithSpellIDInRange(l_Patches, eSpells::SpellFelFlameArea, 5.f);

                for (auto & it : l_Patches)
                {
                    if (it != nullptr)
                        it->SetDuration(1);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_fel_impact_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fel_impact_SpellScript();
        }
};

/// Witness the Void - 207720
class spell_nh_witness_the_void : public SpellScriptLoader
{
    public:
        spell_nh_witness_the_void() : SpellScriptLoader("spell_nh_witness_the_void")
        {}

        class spell_witness_the_void_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_witness_the_void_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr || GetCaster() == nullptr)
                    return;

                OrientationCheck Cmp(GetCaster());

                if (Cmp(GetHitUnit()))
                    PreventHitAura();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_witness_the_void_SpellScript::HandleOnHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_witness_the_void_SpellScript();
        }
};

/// Grand Conjuction - 205408
class spell_nh_etraeus_grand_conjunction : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_grand_conjunction() : SpellScriptLoader("spell_nh_etraeus_grand_conjunction")
        {}

        class spell_grand_conjuction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_grand_conjuction_SpellScript);

            bool Load() override
            {
                m_StarSigns.fill(0);
                return true;
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || !GetCaster())
                    return;

                TankFilter Cmp;

                p_Targets.sort([&Cmp] (WorldObject*& p_It, WorldObject*& p_It2)
                {
                    return !Cmp(p_It->ToPlayer()) && Cmp(p_It2->ToPlayer());
                });

                if (p_Targets.size() % 2 != 0 && !p_Targets.empty())
                    p_Targets.pop_back();
            }

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit() || !GetCaster())
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();

                switch (l_Caster->GetAI()->GetData(eDataEtraeus::DataConjuctionCast))
                {
                    case 1:
                    {
                        uint8 & l_WolfCount = m_StarSigns.at(0);
                        uint8 & l_CrabCount = m_StarSigns.at(1);

                        if (l_WolfCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignWolf, true);
                            l_WolfCount++;
                        }
                        else if (l_CrabCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignCrab, true);
                            l_CrabCount++;
                        }

                        break;
                    }

                    case 2:
                    {
                        uint8 & l_WolfCount = m_StarSigns.at(0);
                        uint8 & l_CrabCount = m_StarSigns.at(1);
                        uint8 & l_HunterCount = m_StarSigns.at(2);

                        if (l_WolfCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignWolf, true);
                            l_WolfCount++;
                        }
                        else if (l_CrabCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignCrab, true);
                            l_CrabCount++;
                        }
                        else if (l_HunterCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignHunter, true);
                            l_HunterCount++;
                        }

                        break;
                    }

                    case 3:
                    {
                        uint8 & l_WolfCount     = m_StarSigns.at(0);
                        uint8 & l_CrabCount     = m_StarSigns.at(1);
                        uint8 & l_HunterCount   = m_StarSigns.at(2);
                        uint8 & l_DragonCount   = m_StarSigns.at(3);

                        if (l_WolfCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignWolf, true);
                            l_WolfCount++;
                        }
                        else if (l_CrabCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignCrab, true);
                            l_CrabCount++;
                        }
                        else if (l_HunterCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignHunter, true);
                            l_HunterCount++;
                        }
                        else if (l_DragonCount < 4)
                        {
                            l_Caster->CastSpell(l_Target, eSpells::SpellStarSignDragon, true);
                            l_DragonCount++;
                        }

                        break;
                    }

                    default : break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_grand_conjuction_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grand_conjuction_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }

            std::array<uint8, 4> m_StarSigns;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_grand_conjuction_SpellScript();
        }
};

/// Fel Ember - 216781
class spell_nh_etraeus_fel_ember : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_fel_ember() : SpellScriptLoader("spell_nh_etraeus_fel_ember")
        {}

        class spell_fel_ember_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fel_ember_SpellScript);

            void HandleOnCast()
            {
                if (GetExplTargetDest() == nullptr || GetCaster() == nullptr)
                    return;

                if (GetCaster()->GetEntry() == eCreatures::BossStarAugurEtraeus)
                {
                    UnitAI* AI = GetCaster()->GetAI();

                    if (AI != nullptr && AI->GetData(eDataEtraeus::DataPhase) != ePhases::FelPhase)
                        return;
                }

                WorldLocation  const* l_Dest = GetExplTargetDest();

                Unit* l_Caster = GetCaster();

                uint32 l_Patches = urand(1,3);

                for (uint8 l_Itr = 0; l_Itr < l_Patches; ++l_Itr)
                {
                    Position l_Pos;
                    l_Caster->GetRandomPoint(*l_Dest, 5.f, l_Pos);
                    GetCaster()->CastSpell(l_Pos, eSpells::SpellFelEmberArea, true);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_fel_ember_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fel_ember_SpellScript();
        }
};

/// Frost Nova - 206953
class spell_nh_etraeus_frigid_nova : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_frigid_nova() : SpellScriptLoader("spell_nh_etraeus_frigid_nova")
        {}

        class spell_frigid_nova_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frigid_nova_dmg_SpellScript);

            bool Load() override
            {
                m_Count = 0;
                return true;
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || !GetCaster())
                    return;

                p_Targets.remove_if([this](WorldObject*& p_Itr)
                {
                    return p_Itr->GetGUID() == GetCaster()->GetGUID();
                });

                if (p_Targets.size() >= 4)
                    m_Count = 4;
                else
                    m_Count = p_Targets.size();

                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    Unit* l_Boss = sObjectAccessor->GetUnit(*GetCaster(), l_Instance->GetData64(eCreatures::BossStarAugurEtraeus));

                    int32 l_Bp = GetSpellInfo()->Effects[EFFECT_0].BasePoints;

                    l_Bp = l_Bp * (1.f - (0.2f * m_Count));

                    if (l_Boss)
                        l_Boss->CastCustomSpell(eSpells::SpellFrigidNovaDmg, SPELLVALUE_BASE_POINT0, l_Bp, GetCaster(), true);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_frigid_nova_dmg_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }

            size_t m_Count;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_frigid_nova_dmg_SpellScript();
        }
};

/// Devouring Remmant - 217046
class spell_nh_etraeus_devouring_remmant : public SpellScriptLoader
{
  public:
    spell_nh_etraeus_devouring_remmant() : SpellScriptLoader("spell_nh_etraeus_devouring_remmant")
    {}

    class spell_devouring_remmant_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_devouring_remmant_AuraScript);

        void HandleAfterApply(AuraEffect const* /**/, AuraEffectHandleModes /**/)
        {
            if (GetCaster() == nullptr || GetUnitOwner() == nullptr)
                return;

            GetCaster()->AddUnitState(UNIT_STATE_ROOT);
        }
        
        void HandleAfterEffectRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)       
        {
            if (!GetCaster() || !GetUnitOwner())
                return;
            
            GetCaster()->StopMoving();
            GetCaster()->SetFacingToObject(GetUnitOwner());
            GetCaster()->CastSpell(GetUnitOwner(), eSpells::SpellDevouringRemmantDmg, true);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_devouring_remmant_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_devouring_remmant_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_devouring_remmant_AuraScript();
    }
};

/// Grand Trine - 207831
class spell_nh_etraeus_grand_trine : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_grand_trine() : SpellScriptLoader("spell_nh_etraeus_grand_trine")
        {}

        class spell_grand_trine_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_grand_trine_SpellScript);

            void HandleOnHit(SpellEffIndex /**/)
            {
                if (!GetHitUnit())
                    return;

                if (GetHitDamage() > 0)
                {
                    Unit* l_Target = GetHitUnit();

                    uint32 l_Count = l_Target->GetAuraCount(eSpells::SpellGrandTrine);

                    SetHitDamage(GetHitDamage() * (2.f * l_Count));
                }
            }

            void Register() override
            {
                OnEffectLaunchTarget += SpellEffectFn(spell_grand_trine_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_grand_trine_SpellScript();
        }
};

/// Fel Nova - 206517
class spell_nh_etraeus_fel_nova : public SpellScriptLoader
{
  public:
    spell_nh_etraeus_fel_nova() : SpellScriptLoader("spell_nh_etraeus_fel_nova")
    {}

    class spell_fel_nova_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_fel_nova_SpellScript);

        void HandleOnHit(SpellEffIndex /**/)
        {
            if (GetHitUnit() == nullptr || GetCaster() == nullptr)
                return;

            if (GetHitDamage() > 0)
            {
                int l_Dist = GetCaster()->GetExactDist2d(GetHitUnit());
                float l_Pct = 0.f;

                if (l_Dist >= 25)
                    l_Pct = 0.90f;
                else if (l_Dist >= 9)
                {
                    l_Pct = 0.10f;

                    for (size_t l_Itr = 0; l_Itr < l_Dist - 9; ++l_Itr)
                        l_Pct += 0.05f;
                }

                SetHitDamage(GetHitDamage() * (1.f - l_Pct));
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_fel_nova_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_fel_nova_SpellScript();
    }
};

/// World Devouring Force   - 216909
/// Devouring Remmant       - 217054
class spell_nh_etraeus_world_devouring_force : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_world_devouring_force() : SpellScriptLoader("spell_nh_etraeus_world_devouring_force")
        {}

        enum eSpellsRestrictions
        {
            SpellWorldDevouringForceRestriction = 31666,
            SpellDevouringRemmantRestriction    = 31707,
        };

        class spell_world_devouring_force_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_world_devouring_force_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty() || GetCaster() == nullptr || GetSpellInfo() == nullptr)
                    return;
                
                uint32 l_SpellId = GetSpellInfo()->Id;
                uint32 l_SpellRestrictionId = l_SpellId == eSpells::SpellWorldDevouringForce ? SpellWorldDevouringForceRestriction : SpellDevouringRemmantRestriction;
                
                SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(l_SpellRestrictionId);
                
                float l_Range = l_SpellId == eSpells::SpellWorldDevouringForce ? 200.f : 300.f;

                Unit* l_Caster = GetCaster();

                if (l_Restriction == nullptr)
                    return;

                p_Targets.remove_if([&l_Caster, &l_Restriction, &l_Range] (WorldObject*& p_Itr)
                {
                    if(p_Itr == nullptr)
                        return true;

                    if (!p_Itr->IsInAxe(l_Caster, l_Restriction->Width, l_Range))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_world_devouring_force_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_world_devouring_force_SpellScript();
        }
};

/// Coronal Ejection - 206464
class spell_nh_etraeus_coronal_ejection : public SpellScriptLoader
{
    public:
        spell_nh_etraeus_coronal_ejection() : SpellScriptLoader("spell_nh_etraeus_coronal_ejection")
        {}

        class spell_coronal_ejection_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_coronal_ejection_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([](WorldObject*& p_Itr)
                {
                    return !p_Itr->IsPlayer() || !p_Itr->ToUnit() || p_Itr->ToUnit()->HasAura(eSpells::SpellCoronalEjectionDmg);
                });


            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_coronal_ejection_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_coronal_ejection_SpellScript::HandleTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_coronal_ejection_SpellScript();
        }
};

/// Absolute Zero - 206585
class at_nh_etraeus_absolute_zero : public AreaTriggerEntityScript
{
    public:
        at_nh_etraeus_absolute_zero() : AreaTriggerEntityScript("at_nh_etraeus_absolute_zero")
        {
            m_TimerCheck    = 0;
            m_CanTakeTargets = false;
        }

        void OnCreate(AreaTrigger* p_At) override
        {
            Unit* l_Owner = sObjectAccessor->GetUnit(*p_At, p_At->GetTargetGuid());

            if (l_Owner == nullptr)
                return;

            if (Aura* l_Aura = l_Owner->GetAura(eSpells::SpellAbsoluteZero))
            {
                if (l_Aura->GetStackAmount() > 1)
                {
                    p_At->SetDuration(1);
                    return;
                }
            }

            m_CanTakeTargets = true;
        }

        bool OnRemoveTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return true;

            auto l_Itr = m_TargetsGuid.find(p_Target->GetGUID());

            if (l_Itr != m_TargetsGuid.end())
                m_TargetsGuid.erase(l_Itr);

            return true;
        }

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr || !m_CanTakeTargets)
                return false;

            if (!p_Target->IsPlayer())
                return false;

            if (p_Target->GetGUID() == p_At->GetTargetGuid())
                return false;

            InstanceScript* l_Instance = p_At->GetInstanceScript();

            if (l_Instance != nullptr)
            {
                Unit* l_Boss = sObjectAccessor->GetUnit(*(p_At->GetCaster()), l_Instance->GetData64(eCreatures::BossStarAugurEtraeus));

                if (l_Boss != nullptr)
                {
                    if (p_Target->HasAura(eSpells::SpellChilled))
                        l_Boss->CastSpell(p_Target, eSpells::SpellFrozenSolid, true);
                    else
                        l_Boss->CastSpell(p_Target, eSpells::SpellChilled, true);

                    m_TargetsGuid.insert(p_Target->GetGUID());
                }
            }

            return true;

        }

        void OnUpdate(AreaTrigger* p_At, uint32 p_Diff) override
        {
            m_TimerCheck += p_Diff;

            Unit* l_Owner = sObjectAccessor->GetUnit(*p_At, p_At->GetTargetGuid());

            if (l_Owner != nullptr && !l_Owner->HasAura(eSpells::SpellAbsoluteZero))
            {
                p_At->SetDuration(1);
                return;
            }

            if (m_TimerCheck >= 500)
            {
                m_TimerCheck = 0;

                if (m_TargetsGuid.size() >= 3)
                {
                    p_At->SetDuration(1);
                    
                    if (l_Owner != nullptr)
                        l_Owner->RemoveAurasDueToSpell(eSpells::SpellAbsoluteZero);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_nh_etraeus_absolute_zero();
        }

    private:
        std::set<uint64> m_TargetsGuid;
        uint32 m_TimerCheck;
        bool m_CanTakeTargets;
};

/// Fel Flame - 206399
class at_nh_etraeus_fel_flame : public AreaTriggerEntityScript
{
    public:
        at_nh_etraeus_fel_flame() : AreaTriggerEntityScript("at_nh_etraeus_fel_flame")
        {
            m_TimerCheck = 0;
        }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_TIME_TO_TARGET_SCALE, 3 * IN_MILLISECONDS);

            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE, 1);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 2, 1.f);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 3, 1.f);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, 10.f); ///< (+200% / TimeToTargetScale / 1000) per second
            p_AreaTrigger->SetUInt32Value(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 5, 268435456);
            p_AreaTrigger->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 6, 1.40f);
        }

        void OnUpdate(AreaTrigger* p_At, uint32 p_Diff) override
        {
            float l_VisualRadius = p_At->GetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4);
            float l_CurrRadius = p_At->GetRadius();

            l_VisualRadius += CalculatePct(1.f, 1.8f);
            l_CurrRadius += CalculatePct(1.f, 1.2f);

            if (l_CurrRadius < 1.f)
            {
                p_At->SetRadius(l_CurrRadius);
                p_At->SetFloatValue(AREATRIGGER_FIELD_OVERRIDE_SCALE_CURVE + 4, l_VisualRadius); ///< (+200% / TimeToTargetScale / 1000) per second
            }

            m_TimerCheck += p_Diff;

            if (m_TimerCheck >= 3 * IN_MILLISECONDS)
            {
                m_TimerCheck = 0;

                GuidList* l_Targets = p_At->GetAffectedPlayers();

                if (l_Targets != nullptr)
                {
                    for (auto & l_Itr : *l_Targets)
                    {
                        Player* l_Target = sObjectAccessor->GetPlayer(*p_At, l_Itr);

                        if (l_Target == nullptr)
                            continue;

                        p_At->GetCaster()->CastSpell(l_Target, eSpells::SpellFelFlameDot, true);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_nh_etraeus_fel_flame();
        }

    private:
        uint32 m_TimerCheck;
        uint32 m_TimerAddDot;
};

/// Star Sign - 205429, 205445, 216344, 216345
class at_nh_etraeus_star_sign : public AreaTriggerEntityScript
{
    public:
        at_nh_etraeus_star_sign() : AreaTriggerEntityScript("at_nh_etraeus_star_sign")
        {
        }

        void OnRemove(AreaTrigger* p_At, uint32 p_Diff) override
        {
            Unit* l_Owner = sObjectAccessor->GetUnit(*p_At, p_At->GetTargetGuid());

            if (l_Owner == nullptr)
                return;

            if (Aura* l_Aura = l_Owner->GetAura(p_At->GetSpellId()))
            {
                if (l_Aura->GetCustomData() != 20)
                    l_Owner->CastSpell(l_Owner, eSpells::SpellGrandTrine, true);
            }
        }

        void CheckSign(Unit*& p_Target, AreaTrigger*& p_At)
        {
            Unit* l_Owner = sObjectAccessor->GetUnit(*p_At, p_At->GetTargetGuid());

            if (l_Owner == nullptr)
                return;

            if (p_Target->HasAura(p_At->GetSpellId()))
            {
                if (Aura* l_At = l_Owner->GetAura(p_At->GetSpellId()))
                    l_At->SetCustomData(20);

                if (Aura* l_At = p_Target->GetAura(p_At->GetSpellId()))
                    l_At->SetCustomData(20);

                l_Owner->RemoveAurasDueToSpell(p_At->GetSpellId());
                p_Target->RemoveAurasDueToSpell(p_At->GetSpellId());
            }
            else
            {
                bool l_Result = false;

                for (uint32 const & l_Itr : g_Signs)
                {
                    if (l_Itr != p_At->GetSpellId() && p_Target->HasAura(l_Itr))
                    {
                        l_Result = true;
                        break;
                    }
                }

                if (l_Result)
                    l_Owner->CastSpell(l_Owner, eSpells::SpellGrandTrine, true);
            }
        }

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return false;

            if (p_Target->isDead())
                return false;

            if (!p_Target->IsPlayer())
                return false;

            if (p_Target->GetGUID() == p_At->GetTargetGuid())
                return false;

            CheckSign(p_Target, p_At);

            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_nh_etraeus_star_sign();
        }

};

#ifndef __clang_analyzer__
void AddSC_boss_star_augur_etraeus()
{
    new boss_star_augur_etraeus();
    new npc_nh_etraeus_thing_that_not_should_be();
    new npc_nh_etraeus_remmant_of_the_void();
    new npc_nh_etraeus_voidling();
    new npc_nh_etraeus_eye_of_the_void();

    /// Spells
    new spell_nh_etraeus_grand_trine();
    new spell_nh_etraeus_icy_ejection();
    new spell_nh_etraeus_fel_impact();
    new spell_nh_etraeus_gravitational_pull();
    new spell_nh_etraeus_fel_ejection();
    new spell_nh_etraeus_fel_ember();
    new spell_nh_witness_the_void();
    new spell_nh_etraeus_fel_nova();
    new spell_nh_etraeus_shatter();
    new spell_nh_etraeus_void_ejection();
    new spell_nh_etraeus_void_burst();
    new spell_nh_etraeus_devouring_remmant();
    new spell_nh_etraeus_grand_conjunction();
    new spell_nh_etraeus_frigid_nova();
    new spell_nh_etraeus_world_devouring_force();
    new spell_nh_etraeus_coronal_ejection();

    /// AreaTriggers
    new at_nh_etraeus_absolute_zero();
    new at_nh_etraeus_fel_flame();
    new at_nh_etraeus_star_sign();
}
#endif
