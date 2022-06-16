#include "tomb_of_sargeras.h"

enum eSpells
{
    /// Shared Spells
    SpellLunarGhost         = 235268,
    SpellSoulBlueAura       = 240526,
    SpellLunarGhostCat      = 235270,
    SpellTeleport           = 224720,
    SpellSharedHealth       = 232515,
    SpellLeap               = 215231,

    /// Lunar Areatrigger
    SpellFontOfElune        = 236357,
    SpellPreCombatArea      = 235401,
    SpellLightSideAT        = 234565,
    SpellLightSideAT2       = 234587,
    SpellDarkSideAT         = 234657,
    SpellDarkSideAT2        = 234659,
    SpellLunarSuffusion     = 234995,
    SpellUmbralSuffusion    = 234996,
    SpellAstralPurge        = 234998,
    SpellAstralVulnerability= 236330,

    /// Huntress Kasparian
    SpellTwilightGlaiveAOE  = 237561,
    SpellTwilightGlaiveAT   = 236529,
    SpellMoonGlaive         = 236547,
    SpellDiscorporate       = 236550,
    SpellSpectralGlaive     = 237633,
    SpellGlaiveStormBig     = 239379,
    SpellGlaiveStormMid     = 239383,
    SpellGlaiveStormSmall   = 239386,

    /// Captain Yathae Moonstrike
    SpellTwilightVolley     = 236442,
    SpellShadowShot         = 237630,
    SpellIncorporealShotAOE = 236304,
    SpellIncorporealShotDmg = 236306,
    SpellIncorporealShotPlus= 248911,
    SpellRapidShotAOE       = 236672,
    SpellCallMoontalon      = 236694,

    /// Priestess Lunaspyre
    SpellMoonBurnAOE        = 236518,
    SpellMoonBurnDot        = 236519,
    SpellLunarStrike        = 237632,
    SpellLunarFire          = 239264,
    SpellLunarBeacon        = 236712,
    SpellLunarBarrage       = 236717,
    SpellLunarBarrageAT     = 236726,
    SpellEmbraceOfTheEclipse    = 233263,
    SpellLunarDetonation    = 237782,
    SpellUmbraDetonation    = 233284,
};

enum eEvents
{
    /// Huntress Kasparian
    EventTwilightGlaive = 1,
    EventMoonGlaive,
    EventSpectralGlaive,
    EventGlaiveStorm,

    /// Captain Yathae Moonstrike
    EventShadowShot,
    EventTwilightVolley,
    EventRapidShot,
    EventCallMoonTalon,
    EventIncorporealShot,

    /// Priestess Lunaspyre
    EventLunarStrike,
    EventMoonBurn,
    EventEmbraceTheEclipse,
    EventLunarFire,
    EventLunarBeacon,

    /// Global Teleport
    EventRandomTeleport,
};

enum ePhases
{
    PHASE_HUNTRESS,
    PHASE_CAPTAIN,
    PHASE_PRIESTESS,
};

enum eSistersData
{
    DataTwilightGlaiveTarget    = 1,
    DataPhase,
};

enum eSistersActions
{
    ActionTargetReached = 1,
    ActionSisterChange,
    ActionMoonComplete,
    ActionPhaseChange,
};

enum eTalks
{
    TalkIntro,
    TalkAggro,
    TalkKill,
    TalkWipe,
    TalkDeath,
    TalkSisterChange,
    TalkPhaseThree,
    TalkSpellOne,
    TalkSpellTwo,
    TalkFullMoonLight,
};

static const Position g_CenterRoom =    { 6190.061f, -1020.028f, 2899.448f };

static const Position g_WallPositions [28] =
{
    { 6166.42f, -1018.15f,  2899.55f },
    { 6166.48f, -1024.57f,  2899.55f },
    { 6167.46f, -1011.89f,  2899.55f },
    { 6168.47f, -1030.31f,  2899.55f },
    { 6170.44f, -1006.31f,  2899.55f },
    { 6172.09f, -1036.00f,  2899.55f },
    { 6174.64f, -1001.65f,  2899.55f },
    { 6176.28f, -1040.18f,  2899.55f },
    { 6180.16f, -998.116f,  2899.55f },
    { 6181.78f, -1043.45f,  2899.55f },
    { 6186.34f, -996.326f,  2899.55f },
    { 6188.23f, -1044.75f,  2899.55f },
    { 6192.61f, -995.500f,  2899.55f },
    { 6194.88f, -1043.62f,  2899.55f },
    { 6198.99f, -996.755f,  2899.55f },
    { 6200.76f, -1041.81f,  2899.55f },
    { 6204.58f, -999.858f,  2899.55f },
    { 6206.02f, -1038.8f,  2899.55f },
    { 6206.02f, -1038.8f,  2899.55f },
    { 6206.02f, -1038.8f,  2899.55f },
    { 6208.69f, -1004.26f,  2899.55f },
    { 6210.23f, -1034.2f,  2899.55f },
    { 6210.23f, -1034.2f,  2899.55f },
    { 6210.23f, -1034.2f,  2899.55f },
    { 6212.21f, -1009.98f,  2899.55f },
    { 6212.95f, -1028.51f,  2899.55f },
    { 6214.44f, -1015.4f,  2899.55f },
    { 6214.53f, -1022.04f,  2899.55f }

};

/// Huntress Kasparian -
class boss_huntress_kasparian : public CreatureScript
{
    public:
        boss_huntress_kasparian() : CreatureScript("boss_huntress_kasparian")
        {}

        struct boss_huntress_kasparian_AI : public BossAI
        {
            explicit boss_huntress_kasparian_AI(Creature* p_Me) : BossAI(p_Me, eData::DataSistersOfTheMoon)
            {}

            void SetData(uint32 p_Id, uint32 p_Data) override
            {
                if (p_Id == eSistersData::DataPhase)
                    m_Phase = ePhases(p_Data);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSistersActions::ActionMoonComplete)
                {
                    if (m_Phase == PHASE_HUNTRESS)
                        return;

                    if (IsHeroicOrMythic())
                        events.ScheduleEvent(eEvents::EventGlaiveStorm, IN_MILLISECONDS);
                    else if (m_Phase == PHASE_PRIESTESS)
                        events.ScheduleEvent(eEvents::EventGlaiveStorm, IN_MILLISECONDS);
                }
            }

            void Reset() override
            {
                me->DespawnCreaturesInArea(eCreatures::NpcMoonStalker);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                _Reset();
            }

            void HandleMoonArea()
            {
                Creature* l_Summon = DoSummon(eCreatures::NpcMoonStalker, g_CenterRoom, TEMPSUMMON_MANUAL_DESPAWN);

                if (l_Summon == nullptr)
                    return;

                l_Summon->SetReactState(REACT_PASSIVE);
                l_Summon->AddUnitState(UNIT_STATE_CANNOT_TURN | UNIT_STATE_ROOT);
                l_Summon->SetOrientation(0.f);
                l_Summon->CastSpell(l_Summon, eSpells::SpellFontOfElune, true);
            }

            void KilledUnit(Unit* p_Target) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_Target->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void PrepareNextSister(uint32 const p_Entry)
            {
                Creature* l_Sister = sObjectAccessor->GetCreature(*me, instance->GetData64(p_Entry));

                if (l_Sister == nullptr)
                    return;

                l_Sister->NearTeleportTo(g_CenterRoom, true);
                l_Sister->RemoveAllAuras();

                l_Sister->SetHealth(me->GetHealth());

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (l_Sister->IsAIEnabled)
                    l_Sister->GetAI()->DoAction(eSistersActions::ActionSisterChange);
            }

            void EnableCombatInSisters()
            {
                if (instance == nullptr)
                    return;

                Creature* l_SisterOne = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossPriestessLunaspyre));
                Creature* l_SisterTwo = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossCaptainYathaeMoonStrike));

                if (l_SisterOne != nullptr)
                    DoZoneInCombat(l_SisterOne, 150.f);

                if (l_SisterTwo != nullptr)
                    DoZoneInCombat(l_SisterTwo, 150.f);
            }

            void EnterCombat(Unit* /**/) override
            {
                Talk(eTalks::TalkAggro);

                _EnterCombat();

                DoCast(me, eSpells::SpellSharedHealth, true);

                m_Phase = ePhases::PHASE_HUNTRESS;

                EnableCombatInSisters();

                HandleMoonArea();

                PrepareTimers();

                events.ScheduleEvent(eEvents::EventTwilightGlaive, GetTimeForTwilightGlaive() * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventMoonGlaive, 15 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /**/, uint32 & p_Dmg, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(70) && m_Phase == ePhases::PHASE_HUNTRESS)
                {
                    PrepareNextSister(eCreatures::BossCaptainYathaeMoonStrike);

                    m_Phase = ePhases::PHASE_CAPTAIN;

                    events.Reset();

                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    DoCast(me, eSpells::SpellLunarGhost, true);
                    DoCast(me, eSpells::SpellLunarGhostCat, true);

                    me->AddUnitState(UNIT_STATE_ROOT);

                    DoCast(me, eSpells::SpellTeleport);

                    events.ScheduleEvent(eEvents::EventRandomTeleport, urand(10, 12) * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventTwilightGlaive, 6 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventSpectralGlaive, 2 * IN_MILLISECONDS);
                }
            }

            void EnterEvadeMode() override
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                BossAI::EnterEvadeMode();

                if (instance == nullptr)
                    return;

                Creature* l_SisterOne    = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossPriestessLunaspyre));
                Creature* l_SisterTwo      = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossCaptainYathaeMoonStrike));

                if (l_SisterOne)
                    l_SisterOne->getThreatManager().clearReferences();

                if (l_SisterTwo)
                    l_SisterTwo->getThreatManager().clearReferences();
            }

            void JustDied(Unit* /**/) override
            {
                ClearDelayedOperations();
                DelayTalk(10, eTalks::TalkDeath);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                _JustDied();
            }

            void PrepareTimers()
            {
                while (!m_TwilightGlaiveTimers.empty())
                    m_TwilightGlaiveTimers.pop();

                for (const uint32 & l_Itr : { 18, 20, 30 })
                    m_TwilightGlaiveTimers.push(l_Itr);
            }

            uint32 GetTimeForTwilightGlaive()
            {
                if (m_Phase != ePhases::PHASE_HUNTRESS)
                    return 20;

                uint32 l_Ret = m_TwilightGlaiveTimers.back();

                m_TwilightGlaiveTimers.pop();
                m_TwilightGlaiveTimers.push(l_Ret);

                return l_Ret;
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventTwilightGlaive:
                    {
                        Talk(eTalks::TalkSpellOne);
                        DoCastAOE(eSpells::SpellTwilightGlaiveAOE);
                        events.ScheduleEvent(eEvents::EventTwilightGlaive, GetTimeForTwilightGlaive() * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventMoonGlaive:
                    {
                        DoCastVictim(eSpells::SpellMoonGlaive);
                        events.ScheduleEvent(eEvents::EventMoonGlaive, 15 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventGlaiveStorm:
                    {
                        if (m_Phase != ePhases::PHASE_HUNTRESS)
                        {
                            Creature* l_Stalker = me->FindNearestCreature(eCreatures::NpcMoonStalker, 125.f);

                            if (l_Stalker == nullptr)
                                return;

                            me->SetFacingToObject(l_Stalker);

                            DoCast(me, eSpells::SpellGlaiveStormBig);
                        }

                        break;
                    }

                    case eEvents::EventSpectralGlaive:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                        {
                            me->SetFacingToObject(l_Target);
                            DoCast(l_Target, eSpells::SpellSpectralGlaive);
                        }

                        events.ScheduleEvent(eEvents::EventSpectralGlaive, 6 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventRandomTeleport:
                    {
                        Position l_Pos = g_WallPositions[urand(0, 27)];
                        me->CastSpell(l_Pos, eSpells::SpellLeap, true);
                        events.ScheduleEvent(eEvents::EventRandomTeleport, urand(8, 14) * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                if (m_Phase == ePhases::PHASE_HUNTRESS)
                    DoMeleeAttackIfReady();
            }

            private:
                ePhases m_Phase;
                std::queue<uint32> m_TwilightGlaiveTimers;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_huntress_kasparian_AI(p_Me);
        }
};

/// Captain Yathae Moonstrike -
class boss_captain_yathae_moonstrike : public CreatureScript
{
    public:
        boss_captain_yathae_moonstrike() : CreatureScript("boss_captain_yathae_moonstrike")
        {}

        enum eYathaeTalks
        {
            TalkMoonTalon = 6
        };

        struct boss_captain_yathae_moonstrike_AI : public BossAI
        {
            explicit boss_captain_yathae_moonstrike_AI(Creature* p_Me) : BossAI(p_Me, eData::DataSistersOfTheMoon)
            {}

            void SetData(uint32 p_Id, uint32 p_Data) override
            {
                if (p_Id == eSistersData::DataPhase)
                    m_Phase = ePhases(p_Data);
            }

            void Reset() override
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                _Reset();
            }

            void KilledUnit(Unit* p_Target) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_Target->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void EnterEvadeMode() override
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                BossAI::EnterEvadeMode();

                if (instance == nullptr)
                    return;

                Creature* l_SisterOne = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossPriestessLunaspyre));
                Creature* l_SisterTwo = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossHuntressKasparian));

                if (l_SisterOne)
                    l_SisterOne->getThreatManager().clearReferences();

                if (l_SisterTwo)
                    l_SisterTwo->getThreatManager().clearReferences();
            }

            void SummonedCreatureDies(Creature* p_Summon, Unit* /**/) override
            {
                if (p_Summon == nullptr)
                    return;

                if (p_Summon->GetEntry() == eCreatures::NpcMoontalon && IsMythic())
                    events.RescheduleEvent(eEvents::EventCallMoonTalon, 20 * IN_MILLISECONDS);
            }

            void EnterCombat(Unit* /**/) override
            {
                DelayTalk(4, eTalks::TalkAggro);

                me->AddUnitState(UNIT_STATE_ROOT);

                _EnterCombat();

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                DoCast(me, eSpells::SpellSharedHealth, true);
                me->SetReactState(REACT_PASSIVE);

                m_Phase = ePhases::PHASE_HUNTRESS;

                events.ScheduleEvent(eEvents::EventRandomTeleport, urand(8, 12) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventTwilightVolley, 18 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventShadowShot, 3 * IN_MILLISECONDS);
            }

            void PrepareNextSister(uint32 const p_Entry)
            {
                Creature* l_Sister = sObjectAccessor->GetCreature(*me, instance->GetData64(p_Entry));

                if (l_Sister == nullptr)
                    return;

                l_Sister->NearTeleportTo(g_CenterRoom, true);
                l_Sister->SetHealth(me->GetHealth());
                l_Sister->RemoveAllAuras();

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (l_Sister->IsAIEnabled)
                    l_Sister->GetAI()->DoAction(eSistersActions::ActionSisterChange);
            }

            void JustDied(Unit* /**/) override
            {
                DelayTalk(5, eTalks::TalkDeath);
                _JustDied();
                ClearDelayedOperations();
            }

            void ChangePhasesOnSisters()
            {
                Creature* l_SisterOne = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossPriestessLunaspyre));
                Creature* l_SisterTwo = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossHuntressKasparian));

                if (l_SisterOne && l_SisterOne->IsAIEnabled)
                    l_SisterOne->GetAI()->SetData(eSistersData::DataPhase, m_Phase);

                if (l_SisterTwo && l_SisterTwo->IsAIEnabled)
                    l_SisterTwo->GetAI()->SetData(eSistersData::DataPhase, m_Phase);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSistersActions::ActionSisterChange)
                {
                    m_Phase = ePhases::PHASE_CAPTAIN;
                    ChangePhasesOnSisters();
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->NearTeleportTo(g_CenterRoom, true);
                    me->ClearUnitState(UNIT_STATE_ROOT);

                    DelayTalk(2, eTalks::TalkSisterChange);

                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    events.Reset();

                    me->RemoveAllAuras();

                    events.ScheduleEvent(eEvents::EventRapidShot, 16 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventCallMoonTalon, 7 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventTwilightVolley, 10 * IN_MILLISECONDS);
                }
                else if (p_Action == eSistersActions::ActionMoonComplete)
                {
                    if (m_Phase == ePhases::PHASE_CAPTAIN)
                        return;

                    if (IsHeroicOrMythic())
                        events.ScheduleEvent(eEvents::EventIncorporealShot, IN_MILLISECONDS);
                    else if (m_Phase == ePhases::PHASE_HUNTRESS)
                        events.ScheduleEvent(eEvents::EventIncorporealShot, IN_MILLISECONDS);
                }
            }

            void DamageTaken(Unit* /**/, uint32 & p_Dmg, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(40) && m_Phase == ePhases::PHASE_CAPTAIN)
                {
                    PrepareNextSister(eCreatures::BossPriestessLunaspyre);

                    m_Phase = ePhases::PHASE_PRIESTESS;

                    events.Reset();

                    me->AddUnitState(UNIT_STATE_ROOT);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    DoCast(me, eSpells::SpellLunarGhost, true);
                    DoCast(me, eSpells::SpellTeleport, true);

                    events.ScheduleEvent(eEvents::EventRandomTeleport, urand(10, 12) * IN_MILLISECONDS);
                }
            }

            uint32 GetSpecialAbilityTimer()
            {
                return 0;
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventTwilightVolley:
                    {
                        Unit* l_Target = SelectRangedTarget(true);

                        if (l_Target == nullptr)
                            l_Target = SelectMeleeTarget(false);

                        if (l_Target == nullptr)
                            l_Target = me->getVictim();

                        if (l_Target != nullptr)
                            DoCast(l_Target, eSpells::SpellTwilightVolley);

                        events.ScheduleEvent(eEvents::EventTwilightVolley, 20 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventShadowShot:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            DoCast(l_Target, eSpells::SpellShadowShot);

                        events.ScheduleEvent(eEvents::EventShadowShot, urand(4, 6) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventIncorporealShot:
                    {
                        events.DelayEvent(eEvents::EventRandomTeleport, 8 * IN_MILLISECONDS);
                        DoCastAOE(eSpells::SpellIncorporealShotAOE);
                        break;
                    }

                    case eEvents::EventRapidShot:
                    {
                        DoCastAOE(eSpells::SpellRapidShotAOE);
                        events.ScheduleEvent(eEvents::EventRapidShot, 20 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventCallMoonTalon:
                    {
                        if (summons.HasEntry(eCreatures::NpcMoontalon))
                            summons.DespawnEntry(eCreatures::NpcMoontalon);

                        Talk(eYathaeTalks::TalkMoonTalon);
                        DoCast(me, eSpells::SpellCallMoontalon);
                        events.ScheduleEvent(eEvents::EventCallMoonTalon, 146 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventRandomTeleport:
                    {
                        DoCast(me, eSpells::SpellTeleport);
                        events.ScheduleEvent(eEvents::EventRandomTeleport, urand(10, 15) * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                if (m_Phase == ePhases::PHASE_CAPTAIN)
                    DoMeleeAttackIfReady();
            }

            private:
                ePhases m_Phase;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_captain_yathae_moonstrike_AI(p_Me);
        }
};

/// Priestess Lunaspyre -118518
class boss_priestess_lunaspyre : public CreatureScript
{
    public:
        boss_priestess_lunaspyre() : CreatureScript("boss_priestess_lunaspyre")
        {}

        struct boss_priestess_lunaspyre_AI : public BossAI
        {
            explicit boss_priestess_lunaspyre_AI(Creature* p_Me) : BossAI(p_Me, eData::DataSistersOfTheMoon)
            {}

            void Reset() override
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                CleanUp();
                _Reset();

                m_Achievement = false;
                m_GhostFounded = false;
            }

            uint32 GetData(uint32 p_Id) override
            {
                if (p_Id == eSistersData::DataPhase)
                    return m_Phase;

                return 0;
            }

            void EnterEvadeMode() override
            {
                me->DespawnAreaTriggersInArea(eSpells::SpellLunarBarrageAT, 150.f);

                me->ClearUnitState(UNIT_STATE_ROOT);
                BossAI::EnterEvadeMode();

                if (instance == nullptr)
                    return;

                Creature* l_SisterOne = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossCaptainYathaeMoonStrike));
                Creature* l_SisterTwo = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossHuntressKasparian));

                if (l_SisterOne)
                    l_SisterOne->getThreatManager().clearReferences();

                if (l_SisterTwo)
                    l_SisterTwo->getThreatManager().clearReferences();
            }

            void CleanUp()
            {
                me->DespawnAreaTriggersInArea(eSpells::SpellLunarBarrageAT, 150.f);
            }

            void KilledUnit(Unit* p_Target) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_Target->IsPlayer())
                    Talk(eTalks::TalkKill);
            }

            void ChangePhasesOnSisters()
            {
                Creature* l_SisterOne = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossPriestessLunaspyre));
                Creature* l_SisterTwo = sObjectAccessor->GetCreature(*me, instance->GetData64(eCreatures::BossHuntressKasparian));

                if (l_SisterOne && l_SisterOne->IsAIEnabled)
                    l_SisterOne->GetAI()->SetData(eSistersData::DataPhase, m_Phase);

                if (l_SisterTwo && l_SisterTwo->IsAIEnabled)
                    l_SisterTwo->GetAI()->SetData(eSistersData::DataPhase, m_Phase);
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(eTalks::TalkDeath);
                CleanUp();

                ClearDelayedOperations();

                _JustDied();

                if (instance == nullptr)
                    return;

                Unit* l_Captain = sObjectAccessor->GetUnit(*me, instance->GetData64(eCreatures::BossCaptainYathaeMoonStrike));
                Unit* l_Huntress = sObjectAccessor->GetUnit(*me, instance->GetData64(eCreatures::BossHuntressKasparian));

                if (l_Captain && p_Killer)
                    p_Killer->Kill(l_Captain);

                if (l_Huntress && p_Killer)
                    p_Killer->Kill(l_Huntress);

                if (!IsLFR() && m_Achievement)
                {
                    Creature* l_Ghost = me->FindNearestCreature(eCreatures::NpcTwilightSoul, 30.f, false);

                    if (l_Ghost != nullptr && instance != nullptr)
                        instance->DoCompleteAchievement(eAchievements::WaxOnWax);
                }
            }

            void EnterCombat(Unit* /**/) override
            {
                DelayTalk(8, eTalks::TalkAggro);

                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_PASSIVE);

                _EnterCombat();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                m_Phase = ePhases::PHASE_HUNTRESS;
                events.ScheduleEvent(eEvents::EventMoonBurn, 9 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventLunarStrike, urand(3, 5) * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventRandomTeleport, urand(8, 12) * IN_MILLISECONDS);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSistersActions::ActionSisterChange)
                {
                    me->SetReactState(REACT_AGGRESSIVE);

                    me->NearTeleportTo(g_CenterRoom, true);

                    Talk(eTalks::TalkSisterChange);

                    me->ClearUnitState(UNIT_STATE_ROOT);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    m_Phase = ePhases::PHASE_PRIESTESS;

                    ChangePhasesOnSisters();

                    me->RemoveAllAuras();
                    events.Reset();

                    events.ScheduleEvent(eEvents::EventMoonBurn, 10 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventLunarFire, 20 * IN_MILLISECONDS);
                    events.ScheduleEvent(eEvents::EventLunarBeacon, 18 * IN_MILLISECONDS);
                }
                else if (p_Action == eSistersActions::ActionMoonComplete)
                {
                    if (m_Phase == PHASE_PRIESTESS)
                        return;

                    if (IsHeroicOrMythic())
                        events.ScheduleEvent(eEvents::EventEmbraceTheEclipse, IN_MILLISECONDS);
                    else if (m_Phase == PHASE_CAPTAIN)
                        events.ScheduleEvent(eEvents::EventEmbraceTheEclipse, IN_MILLISECONDS);
                }
            }

            void DamageTaken(Unit* /**/, uint32 & /**/, SpellInfo const* /**/) override
            {
                if (HealthBelowPct(20) && !m_GhostFounded)
                {
                    Creature* l_Ghost = me->FindNearestCreature(eCreatures::NpcTwilightSoul, 50.f, true);
                    m_GhostFounded = true;

                    if (l_Ghost != nullptr)
                    {
                        m_Achievement = l_Ghost->GetDistance2d(g_CenterRoom.GetPositionX(), g_CenterRoom.GetPositionY()) <= 30.f;

                        if (m_Achievement)
                            l_Ghost->CastSpell(l_Ghost, eSpells::SpellSoulBlueAura, true);
                    }
                }
            }

            void SetData(uint32 p_Id, uint32 p_Data) override
            {
                if (p_Id == eSistersData::DataPhase)
                    m_Phase = ePhases(p_Data);
            }

            void ExecuteEvent(uint32 const p_EventId) override
            {
                switch (p_EventId)
                {
                    case eEvents::EventEmbraceTheEclipse:
                    {
                        Talk(eTalks::TalkFullMoonLight);
                        DoCastAOE(eSpells::SpellEmbraceOfTheEclipse);
                        break;
                    }

                    case eEvents::EventMoonBurn:
                    {
                        Talk(eTalks::TalkSpellOne);
                        DoCastAOE(eSpells::SpellMoonBurnAOE);
                        events.ScheduleEvent(eEvents::EventMoonBurn, (m_Phase == ePhases::PHASE_PRIESTESS ? 18 : 25) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventLunarStrike:
                    {
                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.f, true))
                            DoCast(l_Target, eSpells::SpellLunarStrike);

                        events.ScheduleEvent(eEvents::EventLunarStrike, urand(4, 6) * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventLunarFire:
                    {
                        DoCastVictim(eSpells::SpellLunarFire);
                        events.ScheduleEvent(eEvents::EventLunarFire, 10 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventLunarBeacon:
                    {
                        Talk(eTalks::TalkSpellTwo);

                        if (Unit* l_Target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            DoCast(l_Target, eSpells::SpellLunarBeacon);

                        events.ScheduleEvent(eEvents::EventLunarBeacon, 22 * IN_MILLISECONDS);
                        break;
                    }

                    case eEvents::EventRandomTeleport:
                    {
                        DoCast(me, eSpells::SpellTeleport);
                        events.ScheduleEvent(eEvents::EventRandomTeleport, urand(10, 12) * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                if (m_Phase == ePhases::PHASE_PRIESTESS)
                    DoMeleeAttackIfReady();
            }

            private:
                bool m_GhostFounded;
                bool m_Achievement;
                ePhases m_Phase;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new boss_priestess_lunaspyre_AI(p_Me);
        }
};

/// Moontalon - 119205
class npc_sisters_of_the_moon_moontalon : public CreatureScript
{
    public:
        npc_sisters_of_the_moon_moontalon() : CreatureScript("npc_sisters_of_the_moon_moontalon")
        {}

        enum eEvents
        {
            EventDeathlyScreech = 1,
        };

        enum eSpells
        {
            SpellDeathlyScreech = 236697,
        };

        struct npc_sisters_of_the_moon_moontalon_AI : public ScriptedAI
        {
            explicit npc_sisters_of_the_moon_moontalon_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                m_CanCast = false;
            }

            void Reset() override
            {
                m_CanCast = false;
                events.Reset();
            }

            void DamageTaken(Unit*  /**/, uint32 & p_Dmg, SpellInfo const*) override
            {
                if (HealthBelowPct(25) && !m_CanCast)
                {
                    m_CanCast = true;
                    events.ScheduleEvent(eEvents::EventDeathlyScreech, urand(5, 10) * IN_MILLISECONDS);
                }

            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                if (p_EventId == eEvents::EventDeathlyScreech)
                {
                    DoCastAOE(eSpells::SpellDeathlyScreech);
                    events.ScheduleEvent(eEvents::EventDeathlyScreech, urand(10, 15) * IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 const l_EventId = events.ExecuteEvent())
                    ExecuteEvent(l_EventId);

                DoMeleeAttackIfReady();
            }

            private:
                bool m_CanCast;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sisters_of_the_moon_moontalon_AI(p_Me);
        }
};

/// Glaive Target - 119054
class npc_sisters_of_the_moon_glaive_target : public CreatureScript
{
    public:
        npc_sisters_of_the_moon_glaive_target() : CreatureScript("npc_sisters_of_the_moon_glaive_target")
        {}

        enum eEvents
        {
            EventFollowTarget    = 1,
            EventBackCaster,
        };

        struct npc_sisters_of_the_moon_glaive_target_AI : public ScriptedAI
        {
            explicit npc_sisters_of_the_moon_glaive_target_AI(Creature* p_Me) : ScriptedAI(p_Me)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(UnitMoveType::MOVE_RUN, 3.f, true);
                me->SetSpeed(UnitMoveType::MOVE_WALK, 3.f, true);
                me->SetWalk(false);
                m_IsReturning = false;
                m_TargetGUID = 0;
            }

            void SetGUID(uint64 p_Guid, int32 p_Id) override
            {
                m_TargetGUID = p_Guid;

                events.ScheduleEvent(eEvents::EventFollowTarget, 100);
            }

            uint64 GetGUID(int32 p_Id) override
            {
                return (p_Id == eSistersData::DataTwilightGlaiveTarget ? m_TargetGUID : 0);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eSistersActions::ActionTargetReached)
                {
                    events.Reset();
                    me->GetMotionMaster()->Clear();
                    me->StopMoving();

                    AddTimedDelayedOperation(500, [this]() -> void
                    {
                        if (TempSummon* l_TempSum = me->ToTempSummon())
                        {
                            if (Unit* l_Summoner = l_TempSum->GetSummoner())
                            {
                                m_IsReturning = true;
                                me->GetMotionMaster()->MoveFollow(l_Summoner, 0.f, 0.f);
                            }
                        }
                    });
                }
            }

            void CheckDistanceFromSummoner()
            {
                if (TempSummon* l_TempSum = me->ToTempSummon())
                {
                    if (Unit* l_Summoner = l_TempSum->GetSummoner())
                    {
                        if (me->GetDistance2d(l_Summoner) <= 5.f)
                        {
                            events.Reset();
                            me->DespawnOrUnsummon(100);
                        }
                    }
                }
            }

            void ExecuteEvent(uint32 const p_EventId)
            {
                switch (p_EventId)
                {
                    case eEvents::EventFollowTarget:
                    {
                        Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);

                        if (l_Target != nullptr)
                            me->GetMotionMaster()->MoveFollow(l_Target, 0.f, 0.f);

                        events.ScheduleEvent(eEvents::EventFollowTarget, IN_MILLISECONDS);
                        break;
                    }
                    default: break;
                }
            }


            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (m_IsReturning)
                    CheckDistanceFromSummoner();

                events.Update(p_Diff);

                ExecuteEvent(events.ExecuteEvent());
            }

            private:
                bool m_IsReturning;
                uint64 m_TargetGUID;
        };

        CreatureAI* GetAI(Creature* p_Me) const override
        {
            return new npc_sisters_of_the_moon_glaive_target_AI(p_Me);
        }
};

/// Twilight Glaive - 237561
class spell_sisters_of_the_moon_twilight_glaive : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_twilight_glaive() : SpellScriptLoader("spell_sisters_of_the_moon_twilight_glaive")
        {}

        class spell_sotm_twilight_glaive_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sotm_twilight_glaive_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.sort(SearcherCmp());

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sotm_twilight_glaive_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        class spell_sotm_twilight_glaive_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sotm_twilight_glaive_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetCaster() == nullptr || GetUnitOwner() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                Position l_Pos;

                l_Caster->GetNearPosition(l_Pos, 5.f, 0);

                Creature* l_Summon = l_Caster->SummonCreature(eCreatures::NpcGlaiveTarget, l_Pos, TEMPSUMMON_MANUAL_DESPAWN);

                if (l_Summon != nullptr)
                {
                    l_Summon->CastSpell(l_Summon, eSpells::SpellTwilightGlaiveAT, true);

                    if (l_Summon->IsAIEnabled)
                        l_Summon->GetAI()->SetGUID(GetUnitOwner()->GetGUID(), eSistersData::DataTwilightGlaiveTarget);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sotm_twilight_glaive_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sotm_twilight_glaive_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_sotm_twilight_glaive_AuraScript();
        }
};

/// Moon Burn - 236518
class spell_sisters_of_the_moon_moon_burn : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_moon_burn() : SpellScriptLoader("spell_sisters_of_the_moon_moon_burn")
        {}

        class spell_sisters_of_the_moon_moon_burn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sisters_of_the_moon_moon_burn_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.sort(SearcherCmp());

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sisters_of_the_moon_moon_burn_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sisters_of_the_moon_moon_burn_SpellScript();
        }
};

/// Teleport - 224720
class spell_sisters_of_the_moon_teleport : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_teleport() : SpellScriptLoader("spell_sisters_of_the_moon_teleport")
        {}

        class spell_sisters_of_the_moon_teleport_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sisters_of_the_moon_teleport_SpellScript);

            void HandleBeforeCast()
            {
                if (GetCaster() == nullptr)
                    return;

                WorldLocation l_Dest;

                l_Dest.Relocate(g_WallPositions[urand(0, 27)]);

                SetExplTargetDest(l_Dest);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_sisters_of_the_moon_teleport_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sisters_of_the_moon_teleport_SpellScript();
        }
};

/// Lunar Beacon - 236712
class spell_sisters_of_the_moon_lunar_beacon : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_lunar_beacon() : SpellScriptLoader("spell_sisters_of_the_moon_lunar_beacon")
        {}

        class spell_sisters_of_the_moon_lunar_beacon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sisters_of_the_moon_lunar_beacon_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                Unit* l_Owner = GetUnitOwner();

                GetCaster()->CastSpell(l_Owner, eSpells::SpellLunarBarrage, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sisters_of_the_moon_lunar_beacon_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sisters_of_the_moon_lunar_beacon_AuraScript();
        }
};

/// Lunar Barrage - 236717
class spell_sisters_of_the_moon_lunar_barrage : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_lunar_barrage() : SpellScriptLoader("spell_sisters_of_the_moon_lunar_barrage")
        {}

        class spell_sisters_of_the_moon_lunar_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sisters_of_the_moon_lunar_barrage_AuraScript);

            void HandlePeriodic(AuraEffect const* /**/)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                GetUnitOwner()->CastSpell(GetUnitOwner(), eSpells::SpellLunarBarrageAT, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sisters_of_the_moon_lunar_barrage_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sisters_of_the_moon_lunar_barrage_AuraScript();
        }
};

/// Incorporeal Shot Searcher - 236304
class spell_sisters_of_the_moon_incorporeal_shot_searcher : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_incorporeal_shot_searcher() : SpellScriptLoader("spell_sisters_of_the_moon_incorporeal_shot_searcher")
        {}

        class spell_sisters_of_the_moon_incorporeal_shot_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sisters_of_the_moon_incorporeal_shot_searcher_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.sort(SearcherCmp());

                if (p_Targets.size() > 1)
                    p_Targets.resize(1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sisters_of_the_moon_incorporeal_shot_searcher_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sisters_of_the_moon_incorporeal_shot_searcher_SpellScript();
        }
};

/// Incorporeal Shot - 236305
class spell_sisters_of_the_moon_incorporeal_shot : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_incorporeal_shot() : SpellScriptLoader("spell_sisters_of_the_moon_incorporeal_shot")
        {}

        enum eSpells
        {
            SpellIncorporealShotDmg    = 236306,
        };

        class spell_sisters_of_the_moon_incorporeal_shot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sisters_of_the_moon_incorporeal_shot_AuraScript);

            void HandleAfterRemove(AuraEffect const* /**/, AuraEffectHandleModes /**/)
            {
                if (GetUnitOwner() == nullptr || GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();

                l_Caster->CastSpell(l_Owner, eSpells::SpellIncorporealShotDmg, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sisters_of_the_moon_incorporeal_shot_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript()  const override
        {
            return new spell_sisters_of_the_moon_incorporeal_shot_AuraScript();
        }
};

/// Incorporeal Shot Dmg - 236306
class spell_sisters_of_the_moon_incorporeal_shot_dmg : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_incorporeal_shot_dmg() : SpellScriptLoader("spell_sisters_of_the_moon_incorporeal_shot_dmg")
        {}

        class spell_sisters_of_the_moon_incorporeal_shot_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sisters_of_the_moon_incorporeal_shot_dmg_SpellScript);

            bool Load() override
            {
                m_TargetCount = 0;
                return true;
            }

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                m_TargetCount = p_Targets.size();
            }

            void HandleAfterCast()
            {
                if (GetCaster() == nullptr)
                    return;

                if (m_TargetCount < 6)
                    GetCaster()->CastSpell(GetCaster(), eSpells::SpellIncorporealShotPlus, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sisters_of_the_moon_incorporeal_shot_dmg_SpellScript::HandleTargets, EFFECT_0, TARGET_ENNEMIES_IN_CYLINDER);
                AfterCast += SpellCastFn(spell_sisters_of_the_moon_incorporeal_shot_dmg_SpellScript::HandleAfterCast);
            }

            size_t m_TargetCount;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sisters_of_the_moon_incorporeal_shot_dmg_SpellScript();
        }
};

/// Astral Purge - 234998
class spell_sisters_of_the_moon_astral_purge : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_astral_purge() : SpellScriptLoader("spell_sisters_of_the_moon_astral_purge")
        {}

        class spell_sisters_of_the_moon_astral_purge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sisters_of_the_moon_astral_purge_SpellScript);

            void HandleAfterCast()
            {
                if (GetCaster() == nullptr)
                    return;

                Unit* l_Caster = GetCaster();

                std::vector<uint32> l_AurasToRemove =
                {
                    eSpells::SpellDiscorporate,
                    eSpells::SpellMoonBurnDot,
                };

                for (const uint32 l_Itr : l_AurasToRemove)
                    l_Caster->RemoveAurasDueToSpell(l_Itr);

                if (InstanceScript* l_Instance = GetCaster()->GetInstanceScript())
                {
                    if (l_Instance->instance && l_Instance->instance->IsMythic())
                        l_Instance->DoCastSpellOnPlayers(eSpells::SpellAstralVulnerability);
                }
            }

            void HandleOnEffectHit(SpellEffIndex /***/)
            {
                if (GetHitUnit() == nullptr)
                    return;

                Unit* l_Target = GetHitUnit();

                if (l_Target->GetMap() && l_Target->GetMap()->IsMythic())
                {
                    if (Aura* l_Aura = l_Target->GetAura(eSpells::SpellAstralVulnerability))
                    {
                        uint8 l_Stacks = l_Aura->GetStackAmount();

                        float l_Pct = (l_Aura->GetEffect(EFFECT_0)->GetBaseAmount() * l_Stacks) / 100.f;

                        SetHitDamage(GetHitDamage() * (1.0f + l_Pct));
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_sisters_of_the_moon_astral_purge_SpellScript::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterCast += SpellCastFn(spell_sisters_of_the_moon_astral_purge_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript()  const override
        {
            return new spell_sisters_of_the_moon_astral_purge_SpellScript();
        }
};

/// Embrace of the Eclipse - 233264
class spell_sisters_of_the_moon_embrace_of_the_eclipse : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_embrace_of_the_eclipse() : SpellScriptLoader("spell_sisters_of_the_moon_embrace_of_the_eclipse")
        {}

        class spell_sisters_of_the_moon_embrace_of_the_eclipse_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sisters_of_the_moon_embrace_of_the_eclipse_SpellScript);

            void HandleTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                uint32 l_Entry = 0;

                if (l_Caster->IsAIEnabled)
                {
                    uint32 l_Phase = l_Caster->GetAI()->GetData(eSistersData::DataPhase);

                    if (l_Phase == ePhases::PHASE_HUNTRESS)
                        l_Entry = eCreatures::BossHuntressKasparian;
                    else if (l_Phase == ePhases::PHASE_CAPTAIN)
                        l_Entry = eCreatures::BossCaptainYathaeMoonStrike;
                }

                p_Targets.remove_if([&l_Entry] (WorldObject*& p_Itr)
                {
                    return  p_Itr->GetEntry() != l_Entry;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sisters_of_the_moon_embrace_of_the_eclipse_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        class spell_sisters_of_the_moon_embrace_of_the_eclipse_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sisters_of_the_moon_embrace_of_the_eclipse_AuraScript);

            bool Load() override
            {
                m_Absorbed = 0;
                return true;
            }

            void HandleOnAbsorb(AuraEffect* /**/, DamageInfo& /**/, uint32& absorbAmount)
            {
                m_Absorbed += absorbAmount;
            }

            void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /**/)
            {
                if (GetSpellInfo() == nullptr || GetUnitOwner() == nullptr)
                    return;

                uint32 m_MaxAbsorb = GetSpellInfo()->Effects[EFFECT_0].BasePoints;

                if (m_Absorbed < m_MaxAbsorb)
                    GetUnitOwner()->CastCustomSpell(eSpells::SpellLunarDetonation, SPELLVALUE_BASE_POINT0, m_MaxAbsorb - m_Absorbed, GetUnitOwner());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sisters_of_the_moon_embrace_of_the_eclipse_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_sisters_of_the_moon_embrace_of_the_eclipse_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }

            uint32 m_Absorbed;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sisters_of_the_moon_embrace_of_the_eclipse_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_sisters_of_the_moon_embrace_of_the_eclipse_AuraScript();
        }
};

/// Embrace of the Eclipse - 233263
class spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg : public SpellScriptLoader
{
    public:
        spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg() : SpellScriptLoader("spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg")
        {}

        class spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg_AuraScript);

            bool Load() override
            {
                m_Absorbed = 0;
                return true;
            }

            void HandleOnAbsorb(AuraEffect* /**/, DamageInfo& /**/, uint32& absorbAmount)
            {
                m_Absorbed += absorbAmount;
            }

            void HandleAfterRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes /**/)
            {
                if (GetSpellInfo() == nullptr || GetUnitOwner() == nullptr)
                    return;

                uint32 m_MaxAbsorb = GetSpellInfo()->Effects[EFFECT_0].BasePoints;

                if (m_Absorbed < m_MaxAbsorb)
                    GetUnitOwner()->CastCustomSpell(eSpells::SpellUmbraDetonation, SPELLVALUE_BASE_POINT0, m_MaxAbsorb - m_Absorbed, GetUnitOwner());
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB, AURA_EFFECT_HANDLE_REAL);
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_HEAL_ABSORB);
            }

            uint32 m_Absorbed;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg_AuraScript();
        }
};

/// Lunar Suffusion - 234995
/// Umbra Suffusion - 234996
class spell_sister_of_the_moon_suffusion : public SpellScriptLoader
{
    public:
        spell_sister_of_the_moon_suffusion() : SpellScriptLoader("spell_sister_of_the_moon_suffusion")
        {}

        class spell_sister_of_the_moon_suffusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sister_of_the_moon_suffusion_AuraScript);

            bool Load() override
            {
                m_Timer = 0;

                return true;
            }

            void HandleOnUpdate(uint32 const p_Diff)
            {
                m_Timer += p_Diff;

                if (m_Timer >= 3 * IN_MILLISECONDS)
                {
                    m_Timer = 0;
                    ModStackAmount(1);
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_sister_of_the_moon_suffusion_AuraScript::HandleOnUpdate);
            }

            uint32 m_Timer;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sister_of_the_moon_suffusion_AuraScript();
        }
};

/// Twilight Glaive - 236529
class at_sisters_of_the_moon_twilight_glaive : public AreaTriggerEntityScript
{
    public:
        at_sisters_of_the_moon_twilight_glaive() : AreaTriggerEntityScript("at_sisters_of_the_moon_twilight_glaive")
        {
            m_IsMarkedReached = false;
        }

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_At == nullptr || p_Target == nullptr)
                return true;

            if (!p_Target->IsPlayer())
                return true;

            Unit* l_Caster = p_At->GetCaster();

            if (l_Caster->IsAIEnabled)
            {
                if (l_Caster->GetAI()->GetGUID(eSistersData::DataTwilightGlaiveTarget) == p_Target->GetGUID())
                {
                    if (m_IsMarkedReached)
                        return true;
                    else
                    {
                        m_IsMarkedReached = true;
                        l_Caster->GetAI()->DoAction(eSistersActions::ActionTargetReached);
                    }
                }
            }

            return false;
        }

        bool m_IsMarkedReached;

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_sisters_of_the_moon_twilight_glaive();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Font of Elune - 11284 (SpellId : 236357)
class at_sisters_of_the_moon_font_of_elune : public AreaTriggerEntityScript
{
    public:
        at_sisters_of_the_moon_font_of_elune() : AreaTriggerEntityScript("at_sisters_of_the_moon_font_of_elune") {}

        enum eShapes
        {
            WaxingMoon  = 153,
            WaningMoon  = 155
        };

        enum eSpells
        {
            WaxingMoonLightSide = 234565,
            WaxingMoonDarkSide  = 234657,

            WaningMoonLightSide = 234587,
            WaningMoonDarkSide  = 234659
        };

        int32 m_MoonPhaseTimer = 0;
        uint32 const m_MoonCycle = 54 * IN_MILLISECONDS;
        bool m_SpecialSpells = false;

        uint32 m_Shape = 0;

        void PrepareSpecialSpells(AreaTrigger*& p_At)
        {
            InstanceScript* l_Instance = p_At->GetInstanceScript();

            if (l_Instance == nullptr)
                return;

            Unit* l_Huntress    = sObjectAccessor->GetUnit(*p_At, l_Instance->GetData64(eCreatures::BossHuntressKasparian));
            Unit* l_Captain     = sObjectAccessor->GetUnit(*p_At, l_Instance->GetData64(eCreatures::BossCaptainYathaeMoonStrike));
            Unit* l_Priestess   = sObjectAccessor->GetUnit(*p_At, l_Instance->GetData64(eCreatures::BossPriestessLunaspyre));

            if (l_Huntress != nullptr && l_Huntress->IsAIEnabled)
                l_Huntress->GetAI()->DoAction(eSistersActions::ActionMoonComplete);

            if (l_Captain != nullptr && l_Captain->IsAIEnabled)
                l_Captain->GetAI()->DoAction(eSistersActions::ActionMoonComplete);

            if (l_Priestess != nullptr && l_Priestess->IsAIEnabled)
                l_Priestess->GetAI()->DoAction(eSistersActions::ActionMoonComplete);
        }

        bool OnRemoveTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_At == nullptr || p_Target == nullptr)
                return true;

            if (p_Target->GetEntry() == eCreatures::BossCaptainYathaeMoonStrike ||
                p_Target->GetEntry() == eCreatures::BossPriestessLunaspyre ||
                p_Target->GetEntry() == eCreatures::BossHuntressKasparian)
            {
                if (p_Target->isDead())
                    return true;

                if (p_Target->IsAIEnabled && p_Target->isInCombat() &&
                    !p_Target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                {
                    CreatureAI* l_AI = p_Target->ToCreature()->AI();

                    if (l_AI != nullptr)
                        l_AI->AddTimedDelayedOperation(5 * IN_MILLISECONDS, [l_AI] () -> void
                        {
                            l_AI->EnterEvadeMode();
                        });
                }

                return true;
            }

            return false;
        }

        void OnUpdate(AreaTrigger* p_At, uint32 p_Diff) override
        {
            m_MoonPhaseTimer -= p_Diff;

            if (m_MoonPhaseTimer > 0)
                return;

            m_MoonPhaseTimer += m_MoonCycle;

            if (m_SpecialSpells)
                PrepareSpecialSpells(p_At);

            if (m_Shape == eShapes::WaxingMoon)
                m_Shape = eShapes::WaningMoon;
            else
                m_Shape = eShapes::WaxingMoon;

            p_At->ReShape(m_Shape, false);

            Unit* l_Caster = p_At->GetCaster();
            if (!l_Caster)
                return;

            l_Caster->SetOrientation(0.f);

            switch (m_Shape)
            {
                case eShapes::WaningMoon:
                    l_Caster->CastSpell(l_Caster, eSpells::WaningMoonDarkSide, true);
                    l_Caster->CastSpell(l_Caster, eSpells::WaningMoonLightSide, true);
                    break;
                case eShapes::WaxingMoon:
                    m_SpecialSpells = true;
                    l_Caster->CastSpell(l_Caster, eSpells::WaxingMoonDarkSide, true);
                    l_Caster->CastSpell(l_Caster, eSpells::WaxingMoonLightSide, true);
                    break;
                default:
                    break;
            }


        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_sisters_of_the_moon_font_of_elune();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called By Moon Area Trigger -  (234565, 234587, 234657, 234659)
class at_sisters_of_the_moon_moon_side : public AreaTriggerEntityScript
{
    public:
        at_sisters_of_the_moon_moon_side() : AreaTriggerEntityScript("at_sisters_of_the_moon_moon_side")
        {}

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr)
                return true;

            uint32 l_Id = p_At->GetSpellId();

            if (l_Id == eSpells::SpellLightSideAT || l_Id == eSpells::SpellLightSideAT2)
            {
                if (p_Target->HasAura(eSpells::SpellUmbralSuffusion))
                    p_Target->CastSpell(p_Target, eSpells::SpellAstralPurge, true);
            }
            else if (l_Id == eSpells::SpellDarkSideAT || l_Id == eSpells::SpellDarkSideAT2)
            {
                if (p_Target->HasAura(eSpells::SpellLunarSuffusion))
                    p_Target->CastSpell(p_Target, eSpells::SpellAstralPurge, true);
            }

            return false;
        }

        void OnUpdate(AreaTrigger* p_At, uint32 p_Diff) override
        {
            if (p_At == nullptr)
                return;

            GuidList* l_Targets = p_At->GetAffectedPlayers();
            uint32 l_Id = p_At->GetSpellId();

            if (l_Targets == nullptr)
                return;

            for (uint64 const & l_Itr : *l_Targets)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*p_At, l_Itr);

                if (l_Target == nullptr || !l_Target->IsPlayer())
                    continue;

                if (l_Id == eSpells::SpellLightSideAT || l_Id == eSpells::SpellLightSideAT2)
                {
                    SpellInfo const* p_Spell = sSpellMgr->GetSpellInfo(eSpells::SpellLunarSuffusion);

                    if (!l_Target->HasAura(eSpells::SpellLunarSuffusion) &&
                        !l_Target->IsImmunedToSpell(p_Spell))
                        l_Target->CastSpell(l_Target, eSpells::SpellLunarSuffusion, false);

                    if (Aura* l_Aura = l_Target->GetAura(eSpells::SpellUmbralSuffusion))
                        l_Aura->Remove();
                }
                else if (l_Id == eSpells::SpellDarkSideAT || l_Id == eSpells::SpellDarkSideAT2)
                {
                    SpellInfo const* p_Spell = sSpellMgr->GetSpellInfo(eSpells::SpellUmbralSuffusion);

                    if (!l_Target->HasAura(eSpells::SpellUmbralSuffusion) &&
                        !l_Target->IsImmunedToSpell(p_Spell))
                        l_Target->CastSpell(l_Target, eSpells::SpellUmbralSuffusion, false);

                    if (Aura* l_Aura = l_Target->GetAura(eSpells::SpellLunarSuffusion))
                        l_Aura->Remove();
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_sisters_of_the_moon_moon_side();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called By Glaive Storm -  (239379, 239383, 239386)
class at_sisters_of_the_moon_glaive_storm : public AreaTriggerEntityScript
{
    public:
        at_sisters_of_the_moon_glaive_storm() : AreaTriggerEntityScript("at_sisters_of_the_moon_glaive_storm")
        {
        }

        void OnSetCreatePosition(AreaTrigger* p_At, Unit* /**/, Position& p_Src, Position& p_Tgt, std::vector<G3D::Vector3>& /**/) override
        {
            p_At->SetDuration(12000);
            p_At->SetTimeToTarget(12000);
            p_At->SetOrientation(p_At->GetCaster()->GetOrientation());

            if (p_At->GetSpellId() == eSpells::SpellGlaiveStormBig || 
                p_At->GetSpellId() == eSpells::SpellGlaiveStormMid)
                SpawnNextGlaive(p_At);

            p_At->SetMoveType(AreaTriggerMoveType::AT_MOVE_TYPE_THROUGH_WALLS);
        }

        void SpawnNextGlaive(AreaTrigger* p_At)
        {
            Unit* l_Caster = p_At->GetCaster();

            if (l_Caster == nullptr)
                return;

            Position l_Dest;

            p_At->GetFirstCollisionPosition(l_Dest, 48.f, 0);

            float l_Orientations[3] =
            {
                l_Dest.GetOrientation() + float(M_PI),
                l_Dest.GetOrientation() + float(M_PI) + float(M_PI) / 12.f,
                l_Dest.GetOrientation() + float(M_PI) - float(M_PI) / 12.f,
            };

            uint8 l_Glaives = 3;

            if (p_At->GetSpellId() == eSpells::SpellGlaiveStormMid)
                l_Glaives = 2;

            for (uint8 l_Itr = 0; l_Itr < l_Glaives; ++l_Itr)
            {
                l_Dest.SetOrientation(l_Orientations[l_Itr]);

                Creature* l_Stalker = l_Caster->SummonCreature(eCreatures::NpcGlaiveTarget, l_Dest, TEMPSUMMON_TIMED_DESPAWN, 24000);

                if (l_Stalker != nullptr)
                {
                    if (p_At->GetSpellId() == eSpells::SpellGlaiveStormBig)
                        l_Stalker->DelayedCastSpell(l_Stalker, eSpells::SpellGlaiveStormMid, true, 4750);
                    else if (p_At->GetSpellId() == eSpells::SpellGlaiveStormMid)
                        l_Stalker->DelayedCastSpell(l_Stalker, eSpells::SpellGlaiveStormSmall, true, 4750);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_sisters_of_the_moon_glaive_storm();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_sisters_of_the_moon()
{
    /// Creatures
    new boss_huntress_kasparian();
    new boss_captain_yathae_moonstrike();
    new boss_priestess_lunaspyre();

    new npc_sisters_of_the_moon_glaive_target();
    new npc_sisters_of_the_moon_moontalon();

    /// Spells
    new spell_sisters_of_the_moon_twilight_glaive();
    new spell_sisters_of_the_moon_moon_burn();
    new spell_sisters_of_the_moon_teleport();
    new spell_sisters_of_the_moon_lunar_beacon();
    new spell_sisters_of_the_moon_lunar_barrage();
    new spell_sisters_of_the_moon_incorporeal_shot_searcher();
    new spell_sisters_of_the_moon_incorporeal_shot();
    new spell_sisters_of_the_moon_incorporeal_shot_dmg();
    new spell_sisters_of_the_moon_astral_purge();
    new spell_sister_of_the_moon_suffusion();
    new spell_sisters_of_the_moon_embrace_of_the_eclipse();
    new spell_sisters_of_the_moon_embrace_of_the_eclipse_dmg();

    /// Areatriggers
    new at_sisters_of_the_moon_glaive_storm();
    new at_sisters_of_the_moon_twilight_glaive();
    new at_sisters_of_the_moon_font_of_elune();
    new at_sisters_of_the_moon_moon_side();
}
#endif
