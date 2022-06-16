////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "AreaTrigger.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Containers.h"
#include "tomb_of_sargeras.h"
#include "InstanceScript.h"
#include "Unit.h"

enum eSpells
{
    ///<< Boss Spell >>
    HydraShot                   = 232753, ///< General
    HydraShotAura               = 230139,
    HydraShotDamage             = 230143,
    BurdenofPain                = 230201, ///< General
    BurdenofPainDamage          = 230214,
    FromtheAbyss                = 230227, ///< General
    SlicingTornado              = 232722,
    SlicingTornadoAreaTrigger   = 234809,
    SlicingTornadoDamage        = 232732,
    ConsumingHunger             = 230384,
    ConsumingHungerSummon       = 230855,
    ThunderingShock             = 230358,
    ThunderingShockSummon       = 230360,
    BeckonSarukel               = 232746,
    DevouringMawAreaTrigger     = 232745,
    DevouringMawAreaTriggerLine = 232886,
    DevouringMawAreaTriggerWhole = 232884,
    DevouringMaw                = 234621,
    DevouringMawVisualFirst     = 247117,
    DevouringMawVisualSecond    = 247116,
    DevourWhole                 = 232885,
    CallVellius                 = 232757,
    CrashingWave                = 232827,
    CrashingWaveAreaTrigger     = 240759,
    SummonOssunet               = 232756,
    BefoulingInkRandom          = 232902,
    BefoulingInkCaster          = 233302,
    BefoulingInkAreaTrigger     = 232905,
    OsunnetVisual               = 234178,
    BefoulingInk                = 232913,
    Berserk                     = 64238,
    ConcealingMurkDamage        = 230959,
    HydraAcidAura               = 232754,
    HydraAcidStun               = 234332, ///< Proc with 2 stack and more

    ///< Mythic
    DeliciousBufferfishManaReg  = 239362, ///< Only for Heal
    DeliciousBufferfish         = 239369, ///< Main Aura, when apply for triggers 10 stack
    DeliciousBufferfishDmgDone  = 239375, ///< Not Heal
    DeliciousBufferfishAT       = 239388, ///< Only Visual for fish
    DeliciousBufferfishSummon   = 239403, ///< Used when player taken dmg
    DreadShark                  = 239423, ///< Aura For Dread Shark
    DreadSharkTargetPlayer      = 239434, ///< after remove 239423
    DreadSharkTargetNPC         = 239435, ///< after remove 239423
    DreadSharkPlayerAura        = 239424, ///< Aura for player with Fish
    DreadSharkNPCAura           = 239430, ///< Aura for Npc with Fish
    DreadSharkDamage            = 239436, ///< Use after remove 239434 or 239435
    Sliperry                    = 239420, ///< debuff for fish

    ///<< Npc Spell >>
    ThunderingShockDamage       = 230362,
    ConsumingHungerDamage       = 230920,
    ConcealingMurkAreaTrigger   = 230950,
    DarkDepths                  = 230273,
    JawsfromtheDeep             = 230276,
    WaterBlast                  = 241509,
    MurlocMash                  = 242945
};

enum ePhases
{
    PHASE_TEN_THOUSAND_FANGS = 1, ///< Phase 1 - start battle
    PHASE_NIGHTMARES_FROM_THE_DEEP, ///< Phase 2 - below 70%
    PHASE_VAST_OCEANS_OF_WRATH ///< Phase 3 - below 40%
};

enum eEvents
{
    EventThunderingShock = 1, ///< Phase 1
    EventBurdenofPain, ///< All Phase
    EventFromtheAbyss, ///< All Phase
    EventConsumingHunger, ///< Phase 1 and 3
    EventHydraShot, ///< All Phase
    EventSlicingTornado, ///< Phase 1 and 3
    EventBeckonSarukel, ///< Phase 2
    EventCallVellius, ///< Phase 2 and 3
    EventSummonOssunet, ///< Phase 2 and 3
    EventBerserk, ///< All Phase
    EventBuffSpawn ///< Only Mythic
};

enum eMisc
{
    NpcElectrifyingJellyfish            = 115896,
    NpcPiranhadoEventSlicingTornado     = 118286,
    NpcPiranhadoEventCrashingWave       = 116841,
    NpcOssunet                          = 116881,
    NpcSarukel                          = 116843,
    NpcAbyssStalker                     = 115795,
    NpcRazorjawWaverunner               = 115902,
    NpcDeliciousBufferfish              = 119791
};

enum eTalks
{
    TALK_AGGRO = 0,
    TALK_AGGRO2,
    TALK_AGGRO3,
    TALK_THUNDERINGSHOCK,
    TALK_THUNDERINGSHOCK2,
    TALK_THUNDERINGSHOCK3,
    TALK_SLICINGTORNADO,
    TALK_SLICINGTORNADO2,
    TALK_STARTPHASE2,
    TALK_CALLSFORVELIUS,
    TALK_CALLSFORVELIUS2,
    TALK_CALLSFORVELIUS3,
    TALK_BECKONSARUKEL,
    TALK_BECKONSARUKEL2,
    TALK_BECKONSARUKEL3,
    TALK_STARTPHASE3,
    TALK_BERSERK,
    TALK_SLAY,
    TALK_SLAY2,
    TALK_SLAY3,
    TALK_SLAY4,
    TALK_WIPE,
    TALK_WIPE2,
    TALK_WIPE3,
    TALK_DIED,
    TALK_DIED2,
    TALK_DIED3,
    EMOTE_HYDRASHOT,
    EMOTE_SLICINGTORNADO,
    EMOTE_BECKONSARUKEL
};

enum eAchievSasszine
{
    AchievementFiveCourseSeafoodBuffet = 11676,
    AchievementMythicSassZine          = 11776
};

const Position PositionSummonDeliciousBufferfish[] =
{
    {5776.45f, -1250.17f, 2786.14f, 0.115367f},
    {5797.49f, -1224.20f, 2786.14f, 4.796361f},
    {5823.04f, -1246.80f, 2786.14f, 3.253613f},
    {5801.21f, -1271.54f, 2786.14f, 1.674966f},
    {5782.89f, -1268.27f, 2786.14f, 0.856808f}
};

Player* SelectRandomPlayerOssunet(Unit* l_Sasszine, Creature* l_Ossunet, float l_Range)
{
    Map* map = l_Ossunet->GetMap();
    if (!map->IsDungeon())
        return nullptr;

    Map::PlayerList const &PlayerList = map->GetPlayers();
    if (PlayerList.isEmpty())
        return nullptr;

    std::list<Player*> temp;
    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        if (l_Sasszine->getVictim() != i->getSource() && ///< check main tank
            l_Ossunet->IsWithinDistInMap(i->getSource(), l_Range) && i->getSource()->isAlive() && !i->getSource()->isGameMaster() && !i->getSource()->HasAura(eSpells::BurdenofPain)) ///< check off tank
            temp.push_back(i->getSource());

    if (!temp.empty())
    {
        std::list<Player*>::const_iterator j = temp.begin();
        advance(j, rand()%temp.size());
        return (*j);
    }
    return nullptr;
}

class EventDevouringMaw: public BasicEvent
{
    public:

    explicit EventDevouringMaw(Creature* p_Creature) : pSarukel(p_Creature)
    {
    }

    bool Execute(uint64 /*p_CurrTime*/, uint32 /*p_Diff*/)
    {
        if (pSarukel)
        {
            pSarukel->CastSpell(pSarukel, eSpells::DevouringMawAreaTrigger, true);
            pSarukel->CastSpell(pSarukel, eSpells::DevouringMawAreaTriggerLine, true);
            pSarukel->CastSpell(pSarukel, eSpells::DevouringMawAreaTriggerWhole, true);
        }

        return true;
    }

private:
    Creature* pSarukel;
};

class boss_mistress_sasszine : public CreatureScript
{
    public:
        boss_mistress_sasszine() : CreatureScript("boss_mistress_sasszine") { }

        struct boss_mistress_sasszineAI : public BossAI
        {
            boss_mistress_sasszineAI(Creature* creature) : BossAI(creature, eData::DataMistressSasszine)
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            }

            uint8 uPhase = ePhases::PHASE_TEN_THOUSAND_FANGS;
            const uint32 m_DreadSharkPct[6] = { 85, 70, 55, 40, 25, 10 };
            uint8 m_DreadSharkState = 0;

            void InitializeAI() override
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(TOSScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void ClearOther()
            {
                std::list<Creature*> l_Creatures;
                me->GetCreatureListWithEntryInGrid(l_Creatures, eMisc::NpcPiranhadoEventSlicingTornado, 300.f);
                me->GetCreatureListWithEntryInGrid(l_Creatures, eMisc::NpcPiranhadoEventCrashingWave, 200.f);
                me->GetCreatureListWithEntryInGrid(l_Creatures, eMisc::NpcOssunet, 200.f);
                me->GetCreatureListWithEntryInGrid(l_Creatures, eMisc::NpcSarukel, 200.f);

                if (!l_Creatures.empty())
                {
                    for (Creature* l_Creature : l_Creatures)
                    {
                        l_Creature->CastStop();
                        l_Creature->m_Events.KillAllEvents(true);
                        l_Creature->RemoveAurasDueToSpell(eSpells::SlicingTornadoAreaTrigger);
                        l_Creature->RemoveAurasDueToSpell(eSpells::CrashingWaveAreaTrigger);
                        l_Creature->RemoveAurasDueToSpell(eSpells::BefoulingInkAreaTrigger);
                        l_Creature->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTrigger);
                        l_Creature->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTriggerLine);
                        l_Creature->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTriggerWhole);
                    }
                }

                me->DespawnAreaTriggersInArea(eSpells::SlicingTornadoAreaTrigger, 200.f);
                me->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTrigger, 200.f);
                me->DespawnAreaTriggersInArea(eSpells::CrashingWaveAreaTrigger, 200.f);
                me->DespawnAreaTriggersInArea(eSpells::BefoulingInkAreaTrigger, 200.f);
                me->DespawnAreaTriggersInArea(eSpells::ConcealingMurkAreaTrigger, 200.f);
                me->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTriggerLine, 200.f);
                me->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTriggerWhole, 200.f);

                me->RemoveAurasDueToSpell(eSpells::Berserk);
                me->RemoveAurasDueToSpell(eSpells::DreadShark);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::HydraShotAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BurdenofPain);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ConsumingHunger);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::SlicingTornadoDamage);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::JawsfromtheDeep);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DevouringMaw);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BefoulingInk);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ConcealingMurkDamage);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::HydraAcidAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::HydraAcidStun);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DeliciousBufferfish);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DreadShark);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ThunderingShockDamage);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::ConsumingHungerDamage);

                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DeliciousBufferfishManaReg);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DeliciousBufferfish);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DeliciousBufferfishDmgDone);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DeliciousBufferfishSummon);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DreadSharkPlayerAura);
                instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Sliperry);

                me->DespawnCreaturesInArea(eMisc::NpcAbyssStalker, 200.f);
                me->DespawnCreaturesInArea(eMisc::NpcRazorjawWaverunner, 200.f);
                me->DespawnCreaturesInArea(eMisc::NpcElectrifyingJellyfish, 200.f);
                me->DespawnCreaturesInArea(eMisc::NpcDeliciousBufferfish, 200.f);
                instance->DoRemoveForcedMovementsOnPlayers();
            }

            void SummonDeliciousBufferfish()
            {
                for (uint8 i = 0; i < 5; ++i)
                     me->SummonCreature(eMisc::NpcDeliciousBufferfish, PositionSummonDeliciousBufferfish[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000);
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                uPhase = ePhases::PHASE_TEN_THOUSAND_FANGS;
                ClearOther();
                m_DreadSharkState = 0;
                me->SetCanFly(false);
                me->SetDisableGravity(false);
            }

            bool IgnoreFlyingMovementFlagsUpdate() override
            {
                return true;
            }

            bool CanFly() override
            {
                return false;
            }

            void JustReachedHome() override
            {
                Talk(urand(eTalks::TALK_WIPE, eTalks::TALK_WIPE3));
                _JustReachedHome();
                uPhase = ePhases::PHASE_TEN_THOUSAND_FANGS;
                m_DreadSharkState = 0;
            }

            void EnterEvadeMode() override
            {
                ClearOther();
                BossAI::EnterEvadeMode();
                me->NearTeleportTo(me->GetHomePosition(), false, TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_UNSUMMON_PET);
            }

            void KilledUnit(Unit* p_Who) override
            {
                if (p_Who->IsPlayer())
                    Talk(urand(eTalks::TALK_SLAY, eTalks::TALK_SLAY4));
            }

            bool IsEasyDifficulty()
            {
                return (IsNormal() || IsLFR());
            }

            uint32 GetData(uint32 p_ID /* = 0 */) override
            {
                return uint32(uPhase);
            }

            void StartPhaseNightmaresFromTheDeep()
            {
                events.CancelEvent(eEvents::EventThunderingShock);
                events.CancelEvent(eEvents::EventSlicingTornado);
                events.CancelEvent(eEvents::EventConsumingHunger);
                events.RescheduleEvent(eEvents::EventBurdenofPain, 23500);
                events.RescheduleEvent(eEvents::EventFromtheAbyss, 28 * IN_MILLISECONDS);
                if (!IsLFR())
                    events.RescheduleEvent(eEvents::EventHydraShot, 15800);

                events.RescheduleEvent(eEvents::EventSummonOssunet, 11600);
                events.RescheduleEvent(eEvents::EventCallVellius, 30 * IN_MILLISECONDS);
                events.RescheduleEvent(eEvents::EventBeckonSarukel, 40 * IN_MILLISECONDS);
            }

            void StartPhaseVastOceansOfWrath()
            {
                events.RescheduleEvent(eEvents::EventBurdenofPain, 23500);
                events.RescheduleEvent(eEvents::EventFromtheAbyss, 26 * IN_MILLISECONDS);
                events.RescheduleEvent(eEvents::EventConsumingHunger, 39 * IN_MILLISECONDS);
                events.RescheduleEvent(eEvents::EventSlicingTornado, (IsMythic() ? 35 * IN_MILLISECONDS : 45 * IN_MILLISECONDS));
                if (!IsLFR())
                    events.RescheduleEvent(eEvents::EventHydraShot, 15800);

                events.CancelEvent(eEvents::EventBeckonSarukel);
                events.RescheduleEvent(eEvents::EventSummonOssunet, 11600);
                events.RescheduleEvent(eEvents::EventCallVellius, 30 * IN_MILLISECONDS);
            }

            void EnterCombat(Unit* /*attacker*/) override
            {
                ClearOther();
                Talk(urand(eTalks::TALK_AGGRO, eTalks::TALK_AGGRO3));
                _EnterCombat();
                uPhase = ePhases::PHASE_TEN_THOUSAND_FANGS;
                m_DreadSharkState = 0;

                events.ScheduleEvent(eEvents::EventThunderingShock, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBurdenofPain, (IsEasyDifficulty() ? 17900 : 15400));
                events.ScheduleEvent(eEvents::EventFromtheAbyss, 18 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventConsumingHunger, 20 * IN_MILLISECONDS);
                if (!IsLFR())
                    events.ScheduleEvent(eEvents::EventHydraShot, 25200);

                events.ScheduleEvent(eEvents::EventSlicingTornado, 30 * IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventBerserk, (IsLFR() ? 540 : 480) * IN_MILLISECONDS);

                if (IsMythic())
                    events.ScheduleEvent(eEvents::EventBuffSpawn, 12500);
            }

            bool CheckRoom()
            {
                if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 36)
                {
                    EnterEvadeMode();
                    return false;
                }

                return true;
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (IsMythic())
                    instance->DoCompleteAchievement(eAchievSasszine::AchievementMythicSassZine);

                ClearOther();
                Talk(urand(eTalks::TALK_DIED, eTalks::TALK_DIED3));
                instance->AddTimedDelayedOperation(2500, [this]() -> void
                {
                    ClearOther();
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() || !CheckRoom())
                    return;

                if (uPhase == ePhases::PHASE_TEN_THOUSAND_FANGS && me->HealthBelowPct(70))
                {
                    uPhase = ePhases::PHASE_NIGHTMARES_FROM_THE_DEEP;
                    StartPhaseNightmaresFromTheDeep();
                    Talk(eTalks::TALK_STARTPHASE2);
                }

                if (uPhase == PHASE_NIGHTMARES_FROM_THE_DEEP && me->HealthBelowPct(40) && !IsLFR())
                {
                    uPhase = ePhases::PHASE_VAST_OCEANS_OF_WRATH;
                    StartPhaseVastOceansOfWrath();
                    Talk(eTalks::TALK_STARTPHASE3);
                }

                if (IsMythic() && m_DreadSharkState <= 5 && me->HealthBelowPct(m_DreadSharkPct[m_DreadSharkState]))
                {
                    ++m_DreadSharkState;
                    DoCast(me, eSpells::DreadShark);
                    events.RescheduleEvent(eEvents::EventBuffSpawn, 21 * IN_MILLISECONDS);
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventThunderingShock:
                        {
                            Talk(urand(eTalks::TALK_THUNDERINGSHOCK, eTalks::TALK_THUNDERINGSHOCK3));
                            DoCast(eSpells::ThunderingShock);
                            events.ScheduleEvent(eEvents::EventThunderingShock, 32200);
                            break;
                        }
                        case eEvents::EventBurdenofPain:
                        {
                            DoCastVictim(eSpells::BurdenofPain);
                            events.ScheduleEvent(eEvents::EventBurdenofPain, 25100);
                            break;
                        }
                        case eEvents::EventFromtheAbyss:
                        {
                            for (uint8 i = 0; i < 3; ++i)
                                DoCast(eSpells::FromtheAbyss);

                            events.ScheduleEvent(eEvents::EventFromtheAbyss, 27 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventConsumingHunger:
                        {
                            DoCastAOE(eSpells::ConsumingHunger);
                            events.ScheduleEvent(eEvents::EventConsumingHunger, 31600);
                            break;
                        }
                        case eEvents::EventHydraShot:
                        {
                            Talk(eTalks::EMOTE_HYDRASHOT);
                            DoCast(eSpells::HydraShot);
                            uint32 i_timer = 40 * IN_MILLISECONDS;
                            if (uPhase == PHASE_NIGHTMARES_FROM_THE_DEEP || IsMythic())
                                i_timer = 30 * IN_MILLISECONDS;

                            events.ScheduleEvent(eEvents::EventHydraShot, i_timer);
                            break;
                        }
                        case eEvents::EventSlicingTornado:
                        {
                            Talk(eTalks::EMOTE_SLICINGTORNADO);
                            DoCast(eSpells::SlicingTornado);
                            Talk(urand(eTalks::TALK_SLICINGTORNADO, eTalks::TALK_SLICINGTORNADO2));

                            events.ScheduleEvent(eEvents::EventSlicingTornado, (IsMythic() ? 35 * IN_MILLISECONDS : 45 * IN_MILLISECONDS));
                            break;
                        }
                        case eEvents::EventBerserk:
                        {
                            Talk(eTalks::TALK_BERSERK);
                            DoCast(eSpells::Berserk);
                            break;
                        }
                        case eEvents::EventCallVellius:
                        {
                            Talk(urand(eTalks::TALK_CALLSFORVELIUS, eTalks::TALK_CALLSFORVELIUS3));
                            DoCastVictim(eSpells::CallVellius);
                            events.ScheduleEvent(eEvents::EventCallVellius, 42 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventSummonOssunet:
                        {
                            DoCastVictim(eSpells::SummonOssunet);
                            uint32 i_timer = 31 * IN_MILLISECONDS;
                            if (uPhase < PHASE_VAST_OCEANS_OF_WRATH)
                                i_timer = 42 * IN_MILLISECONDS;
                            events.ScheduleEvent(eEvents::EventSummonOssunet, i_timer);
                            break;
                        }
                        case eEvents::EventBeckonSarukel:
                        {
                            Talk(eTalks::EMOTE_BECKONSARUKEL);
                            DoCastVictim(eSpells::BeckonSarukel);
                            Talk(urand(eTalks::TALK_BECKONSARUKEL, eTalks::TALK_BECKONSARUKEL3));
                            events.ScheduleEvent(eEvents::EventBeckonSarukel, 42 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventBuffSpawn:
                        {
                            SummonDeliciousBufferfish();
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void JustSummoned(Creature* summon) override
            {
                BossAI::JustSummoned(summon);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new boss_mistress_sasszineAI(p_Creature);
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Thundering Shock - 230358
class spell_sasszine_thundering_shock : public SpellScriptLoader
{
    public:
        spell_sasszine_thundering_shock() : SpellScriptLoader("spell_sasszine_thundering_shock") { }

        class spell_sasszine_thundering_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_thundering_shock_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (InstanceScript* p_instance = l_Caster->GetInstanceScript())
                    if (Unit* l_Sasszine = ObjectAccessor::GetUnit(*l_Caster, p_instance->GetData64(eData::DataMistressSasszine)))
                        if (Unit* l_Victim = l_Sasszine->getVictim())
                            p_Targets.remove(l_Victim);

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::BurdenofPain));
                p_Targets.remove_if(CheckDeadTarget());

                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, 5);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::ThunderingShockSummon, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_thundering_shock_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_sasszine_thundering_shock_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sasszine_thundering_shock_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Consuming Hunger - 230384
class spell_sasszine_consuming_hunger : public SpellScriptLoader
{
    public:
        spell_sasszine_consuming_hunger() : SpellScriptLoader("spell_sasszine_consuming_hunger") { }

        class spell_sasszine_consuming_hunger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_consuming_hunger_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (InstanceScript* p_instance = l_Caster->GetInstanceScript())
                    if (Unit* l_Sasszine = ObjectAccessor::GetUnit(*l_Caster, p_instance->GetData64(eData::DataMistressSasszine)))
                        if (Unit* l_Victim = l_Sasszine->getVictim())
                            p_Targets.remove(l_Victim);

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::BurdenofPain));
                p_Targets.remove_if(CheckDeadTarget());
                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::ConsumingHungerDamage));

                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, 2);
            }

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::ConsumingHungerSummon, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_consuming_hunger_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_consuming_hunger_SpellScript::SelectTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_sasszine_consuming_hunger_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sasszine_consuming_hunger_SpellScript();
        }
};

/// NPC 115902 - Razorjaw Waverunner
class npc_sasszine_razorjaw_waverunner : public CreatureScript
{
    public:
        npc_sasszine_razorjaw_waverunner() : CreatureScript("npc_sasszine_razorjaw_waverunner") { }

        struct npc_sasszine_razorjaw_waverunnerAI : public ScriptedAI
        {
            npc_sasszine_razorjaw_waverunnerAI(Creature* creature) : ScriptedAI(creature) {}

            enum eNpcEvents
            {
                EventWaterBlast = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void DoAction(int32 const p_Action) override
            {
                m_Events.Reset();
                m_Events.ScheduleEvent(eNpcEvents::EventWaterBlast, 500);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                me->CastSpell(p_Summoner, eSpells::ConsumingHungerDamage, true);
                ///< Hack, idk why aura a have Duration 8 sec
                if (Aura* l_Aura = me->GetAura(eSpells::ConsumingHungerDamage))
                {
                    l_Aura->SetDuration(120000);
                    l_Aura->SetMaxDuration(120000);
                }

                if (Aura* l_AuraSummon = p_Summoner->GetAura(eSpells::ConsumingHungerDamage))
                {
                    l_AuraSummon->SetDuration(120000);
                    l_AuraSummon->SetMaxDuration(120000);
                }

                DoZoneInCombat(me, 250.0f);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->DespawnOrUnsummon(5000);
            }

            Player* SelectRandomPlayer(Unit* l_Sasszine, float l_Range)
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return nullptr;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return nullptr;

                std::list<Player*> temp;
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (l_Sasszine->getVictim() != i->getSource() && ///< check main tank
                        me->IsWithinDistInMap(i->getSource(), l_Range) && i->getSource()->isAlive() && !i->getSource()->isGameMaster() && !i->getSource()->HasAura(eSpells::BurdenofPain)) ///< check off tank
                        temp.push_back(i->getSource());

                if (!temp.empty())
                {
                    std::list<Player*>::const_iterator j = temp.begin();
                    advance(j, rand() % temp.size());
                    return (*j);
                }
                return nullptr;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case eNpcEvents::EventWaterBlast:
                    {
                        if (InstanceScript* p_instance = me->GetInstanceScript())
                            if (Unit* l_Sasszine = ObjectAccessor::GetUnit(*me, p_instance->GetData64(eData::DataMistressSasszine)))
                                if (Player* l_Player = SelectRandomPlayer(l_Sasszine, 15.f))
                                    DoCast(l_Player, eSpells::WaterBlast);

                        m_Events.ScheduleEvent(eNpcEvents::EventWaterBlast, 3 * IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sasszine_razorjaw_waverunnerAI(creature);
        }
};

/// NPC 115795 - Abyss Stalker
class npc_sasszine_abyss_stalker : public CreatureScript
{
    public:
        npc_sasszine_abyss_stalker() : CreatureScript("npc_sasszine_abyss_stalker") { }

        struct npc_sasszine_abyss_stalkerAI : public ScriptedAI
        {
            npc_sasszine_abyss_stalkerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eNpcEvents
            {
                EventDarkDepths = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (IsHeroic() || IsMythic())
                    m_Events.ScheduleEvent(eNpcEvents::EventDarkDepths, urand(1 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 250.0f);
                if (IsHeroic() || IsMythic())
                    m_Events.ScheduleEvent(eNpcEvents::EventDarkDepths, urand(1 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (!IsLFR())
                {
                    me->CastSpell(me, eSpells::ConcealingMurkAreaTrigger, true);
                    me->DespawnOrUnsummon(25000);
                }
                else
                    me->DespawnOrUnsummon(5000);
            }

            void DoMeleeAttackIfReady()
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                Unit* l_Victim = me->getVictim();

                if (l_Victim != nullptr && !l_Victim->isAlive())
                    return;

                if (!me->IsWithinMeleeRange(l_Victim))
                    return;

                if (me->isAttackReady())
                {
                    me->AttackerStateUpdateWithSchoolMask(l_Victim, WeaponAttackType::BaseAttack, false, SPELL_SCHOOL_MASK_SHADOW); ///< From sniff mask shadow
                    me->resetAttackTimer();
                }

                if (me->haveOffhandWeapon() && me->isAttackReady(WeaponAttackType::OffAttack))
                {
                    me->AttackerStateUpdateWithSchoolMask(l_Victim, WeaponAttackType::OffAttack, false, SPELL_SCHOOL_MASK_SHADOW); ///< From sniff mask shadow
                    me->resetAttackTimer(WeaponAttackType::OffAttack);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eNpcEvents::EventDarkDepths:
                            DoCast(eSpells::DarkDepths);
                            m_Events.ScheduleEvent(eNpcEvents::EventDarkDepths, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_sasszine_abyss_stalkerAI(p_Creature);
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Concealing Murk - 230950
class spell_at_sasszine_concealing_murk : public AreaTriggerEntityScript
{
    public:
        spell_at_sasszine_concealing_murk() : AreaTriggerEntityScript("spell_at_sasszine_concealing_murk") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;
        uint32 l_MaxDuration = 20 * IN_MILLISECONDS;
        std::list<AreaTrigger*> l_TornadoAreaTriggerList;

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster || !l_Caster->GetMap())
                return;

            l_MaxDuration = (l_Caster->GetMap()->IsMythic() ? 10 * IN_MILLISECONDS : 20 * IN_MILLISECONDS);
            p_AreaTrigger->SetDuration(l_MaxDuration);
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 500;
                if (!p_AreaTrigger)
                    return;

                auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
                if (!l_AreaTriggerCaster || !l_AreaTriggerCaster->GetMap())
                    return;

                uint32 I_ReduceDuration = (l_AreaTriggerCaster->GetMap()->IsMythic() ? 4 * IN_MILLISECONDS : (l_AreaTriggerCaster->GetMap()->IsHeroic() ? 10 * IN_MILLISECONDS : 2 * IN_MILLISECONDS));
                if (p_AreaTrigger->GetDuration() <= I_ReduceDuration && p_AreaTrigger->GetDuration() != 0)
                    p_AreaTrigger->SetRadius(p_AreaTrigger->GetRadius() - (l_AreaTriggerCaster->GetMap()->IsMythic() ? 1.5f : 0.7f));

                std::list<AreaTrigger*> l_AreatriggerList;
                l_AreaTriggerCaster->GetAreaTriggerListWithSpellIDInRange(l_AreatriggerList, eSpells::SlicingTornadoAreaTrigger, 50.f);
                if (!l_AreatriggerList.empty())
                {
                    for (AreaTrigger* l_Areatrigger : l_AreatriggerList)
                    {
                        if (p_AreaTrigger->GetDistance2d(l_Areatrigger) <= 8.f)
                        {
                            if (l_AreaTriggerCaster->GetMap()->IsMythic())
                            {
                                bool l_TornadoAlreadyUse = false;
                                for (auto I_TornadoAT : l_TornadoAreaTriggerList)
                                {
                                    if (!I_TornadoAT || !I_TornadoAT->IsInWorld())
                                        continue;

                                    if (l_Areatrigger->GetGUID() == I_TornadoAT->GetGUID())
                                    {
                                        l_TornadoAlreadyUse = true;
                                        break;
                                    }
                                }

                                if (l_TornadoAlreadyUse)
                                    continue;

                                l_Areatrigger->MoveAreaTrigger(l_Areatrigger->GetDestination(), 1, true);
                                l_TornadoAreaTriggerList.push_back(l_Areatrigger);
                            }
                            else
                                l_Areatrigger->Remove();
                        }
                    }
                }

                float l_Radius = p_AreaTrigger->GetRadius();
                std::list<Player*> l_PlayerList;
                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::ConcealingMurkDamage) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::ConcealingMurkDamage))
                        l_Player->RemoveAurasDueToSpell(eSpells::ConcealingMurkDamage);

                    m_Targets.remove(l_TargetGuid);
                }

                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);
                l_PlayerList.remove_if(CheckDeadTarget());

                if (!l_PlayerList.empty())
                {
                    SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ConcealingMurkDamage);
                    for (Player* l_Player : l_PlayerList)
                    {
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::ConcealingMurkDamage))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::ConcealingMurkDamage, true);
                            m_Targets.push_back(l_Player->GetGUID());
                        }
                    }
                }
            }
            else
                m_CheckTimer -= p_Time;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (!p_AreaTrigger)
                return;

            auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
            if (!l_AreaTriggerCaster)
                return;

            std::list<uint64> l_Targets = m_Targets;
            for (uint64 l_TargetGuid : l_Targets)
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                if (!l_Player)
                    continue;

                if (l_Player->HasAura(eSpells::ConcealingMurkDamage))
                    l_Player->RemoveAurasDueToSpell(eSpells::ConcealingMurkDamage);

                m_Targets.remove(l_TargetGuid);
            }

            for (auto l_TornadoAreaTrigger : l_TornadoAreaTriggerList)
            {
                if (!l_TornadoAreaTrigger || !l_TornadoAreaTrigger->IsInWorld())
                    continue;

                l_TornadoAreaTrigger->MoveAreaTrigger(l_TornadoAreaTrigger->GetDestination(), 16 * IN_MILLISECONDS);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_sasszine_concealing_murk();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Hydra Shot - 232753
class spell_sasszine_hydra_shot : public SpellScriptLoader
{
    public:
        spell_sasszine_hydra_shot() : SpellScriptLoader("spell_sasszine_hydra_shot") { }

        class spell_sasszine_hydra_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_hydra_shot_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (InstanceScript* p_instance = l_Caster->GetInstanceScript())
                    if (Unit* l_Sasszine = ObjectAccessor::GetUnit(*l_Caster, p_instance->GetData64(eData::DataMistressSasszine)))
                        if (Unit* l_Victim = l_Sasszine->getVictim())
                            p_Targets.remove(l_Victim);

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::BurdenofPain));
                p_Targets.remove_if(CheckDeadTarget());

                p_Targets.remove_if([](WorldObject* p_Target) -> bool
                {
                    if (!p_Target->IsPlayer() || p_Target->ToPlayer()->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (p_Targets.empty())
                    return;

                uint8 t_counts = 1; ///< Normal
                Map* map = l_Caster->GetMap();
                if (!map)
                    return;

                if (map->IsMythic())
                    t_counts = 4;
                else if (map->IsHeroic())
                    t_counts = 3;

                JadeCore::RandomResizeList(p_Targets, t_counts);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::HydraShotAura, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_hydra_shot_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_sasszine_hydra_shot_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sasszine_hydra_shot_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Hydra Shot Aura - 230139
class spell_sasszine_hydra_shot_aura : public SpellScriptLoader
{
    public:
        spell_sasszine_hydra_shot_aura() : SpellScriptLoader("spell_sasszine_hydra_shot_aura") { }

        class spell_sasszine_hydra_shot_aura_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_sasszine_hydra_shot_aura_Aurascript);

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::HydraShotDamage, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sasszine_hydra_shot_aura_Aurascript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sasszine_hydra_shot_aura_Aurascript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Hydra Shot Aura - 230143
class spell_sasszine_hydra_shot_damage : public SpellScriptLoader
{
    public:
        spell_sasszine_hydra_shot_damage() : SpellScriptLoader("spell_sasszine_hydra_shot_damage") { }

        class spell_sasszine_hydra_shot_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_hydra_shot_damage_SpellScript);

            uint32 m_TargetCount;

            bool Load() override
            {
                m_TargetCount = 0;
                return true;
            }

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                m_TargetCount = uint32(p_Targets.size());
            }

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                if (!m_TargetCount)
                    return;

                SetHitDamage(GetHitDamage() / m_TargetCount);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::HydraAcidAura, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_hydra_shot_damage_SpellScript::CorrectTargets, EFFECT_0, TARGET_ENNEMIES_IN_CYLINDER);
                OnEffectHitTarget += SpellEffectFn(spell_sasszine_hydra_shot_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                AfterHit += SpellHitFn(spell_sasszine_hydra_shot_damage_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sasszine_hydra_shot_damage_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Hydra Acid Aura - 232754
class spell_sasszine_hydra_acid_aura : public SpellScriptLoader
{
    public:
        spell_sasszine_hydra_acid_aura() : SpellScriptLoader("spell_sasszine_hydra_acid_aura") { }

        class spell_sasszine_hydra_acid_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sasszine_hydra_acid_aura_AuraScript);

            void AfterStackApply(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (GetStackAmount() >= 2)
                    l_Target->CastSpell(l_Target, eSpells::HydraAcidStun, true);
            }


            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sasszine_hydra_acid_aura_AuraScript::AfterStackApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sasszine_hydra_acid_aura_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Slicing Tornado - 232722
class spell_sasszine_slicing_tornado : public SpellScriptLoader
{
    public:
        spell_sasszine_slicing_tornado() : SpellScriptLoader("spell_sasszine_slicing_tornado") { }

        class spell_sasszine_slicing_tornado_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_slicing_tornado_SpellScript);

            enum eTactic
            {
                TacticCenter = 0,
                TacticLeft = 1,
                TacticRight = 2,
                TacticHeroicLeft = 3,
                TacticHeroicRight = 4
            };

            void HandleAfterCast()
            {
                Unit* I_Caster = GetCaster();
                if (!I_Caster)
                    return;

                std::list<Creature*> l_Tornados;
                I_Caster->GetCreatureListWithEntryInGrid(l_Tornados, eMisc::NpcPiranhadoEventSlicingTornado, 300.f);
                if (l_Tornados.empty())
                    return;

                uint8 minTactic = (I_Caster->GetMap()->IsHeroicOrMythic() ? eTactic::TacticHeroicLeft : eTactic::TacticCenter);
                uint8 maxTactic = (I_Caster->GetMap()->IsHeroicOrMythic() ? eTactic::TacticHeroicRight : eTactic::TacticRight);
                m_tactics = urand(minTactic, maxTactic);

                for (Creature* l_tornado : l_Tornados)
                {
                    if ((m_tactics == eTactic::TacticCenter && l_tornado->GetOrientation() == 5.16207f) ||
                        (m_tactics == eTactic::TacticLeft && l_tornado->GetOrientation() == 1.20782f) ||
                        (m_tactics == eTactic::TacticRight && l_tornado->GetOrientation() == 0.524855f) ||
                        (m_tactics == eTactic::TacticHeroicLeft && l_tornado->GetOrientation() == 2.81938f) ||
                        (m_tactics == eTactic::TacticHeroicRight && l_tornado->GetOrientation() == 5.16208f))
                    {
                        l_tornado->CastSpell(l_tornado, eSpells::SlicingTornadoAreaTrigger, true);
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_sasszine_slicing_tornado_SpellScript::HandleAfterCast);
            }

        private:
            uint8 m_tactics = 0;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sasszine_slicing_tornado_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Slicing Tornado AreaTrigger - 234809
class spell_at_sasszine_slicing_tornado : public AreaTriggerEntityScript
{
    public:
        spell_at_sasszine_slicing_tornado() : AreaTriggerEntityScript("spell_at_sasszine_slicing_tornado") {}

        uint32 m_FindConcealingMurkTimer = 500;
        float l_Radius = 4.f;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                if (m_FindConcealingMurkTimer <= p_Time)
                {
                    m_FindConcealingMurkTimer = 500;

                    std::list<Player*> l_PlayerList;
                    JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                    JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                    p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);
                    l_PlayerList.remove_if(CheckDeadTarget());

                    if (!l_PlayerList.empty())
                    {
                        SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SlicingTornadoDamage);
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player || !p_SpellInfo || l_Player->IsImmunedToSpell(p_SpellInfo))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::SlicingTornadoDamage, true);
                        }
                    }
                }
                else
                    m_FindConcealingMurkTimer -= p_Time;
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_sasszine_slicing_tornado();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Call Vellius - 232757
class spell_sasszine_call_vellius : public SpellScriptLoader
{
    public:
        spell_sasszine_call_vellius() : SpellScriptLoader("spell_sasszine_call_vellius") { }

        class spell_sasszine_call_vellius_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_call_vellius_SpellScript);

            void HandleAfterCast()
            {
                Unit* I_Caster = GetCaster();
                if (!I_Caster)
                    return;

                std::list<Creature*> l_PiranhadoList;
                I_Caster->GetCreatureListWithEntryInGrid(l_PiranhadoList, eMisc::NpcPiranhadoEventCrashingWave, 200.f);
                if (l_PiranhadoList.empty())
                    return;

                JadeCore::RandomResizeList(l_PiranhadoList, 1);
                for (Creature* l_Piranhado : l_PiranhadoList)
                {
                    l_Piranhado->CastSpell(l_Piranhado, eSpells::CrashingWaveAreaTrigger, true); ///< Visual
                    l_Piranhado->CastSpell(l_Piranhado, eSpells::CrashingWave, false);
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_sasszine_call_vellius_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sasszine_call_vellius_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Summon Ossunet - 232756
class spell_sasszine_summon_ossunet : public SpellScriptLoader
{
    public:
        spell_sasszine_summon_ossunet() : SpellScriptLoader("spell_sasszine_summon_ossunet") { }

        class spell_sasszine_summon_ossunet_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_summon_ossunet_SpellScript);

            void HandleAfterCast()
            {
                Unit* I_Caster = GetCaster();
                if (!I_Caster)
                    return;

                std::list<Creature*> l_OssunetList;
                I_Caster->GetCreatureListWithEntryInGrid(l_OssunetList, eMisc::NpcOssunet, 200.f);
                if (l_OssunetList.empty())
                    return;

                JadeCore::RandomResizeList(l_OssunetList, 1);
                uint8 countBefouling = (I_Caster->GetMap()->IsMythic() ? 6 : 4);
                for (Creature* l_Ossunet : l_OssunetList)
                {
                    l_Ossunet->CastSpell(l_Ossunet, eSpells::OsunnetVisual, true);
                    for (uint8 i = 0; i < countBefouling; ++i)
                    {
                        if (Player* l_Player = SelectRandomPlayerOssunet(I_Caster, l_Ossunet, 100.f))
                            l_Ossunet->CastSpell(l_Player, eSpells::BefoulingInkRandom, true);
                    }
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_sasszine_summon_ossunet_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sasszine_summon_ossunet_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Befouling Ink - 232905
class spell_at_sasszine_befouling_ink : public AreaTriggerEntityScript
{
    public:
        spell_at_sasszine_befouling_ink() : AreaTriggerEntityScript("spell_at_sasszine_befouling_ink") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;
        float l_Radius = 5.f; /// From sniff
        bool l_Removed = false;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 500;
                if (!p_AreaTrigger)
                    return;

                SpellInfo const* p_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BefoulingInk);
                if (!p_SpellInfo)
                {
                    p_AreaTrigger->Remove();
                    return;
                }

                if (l_Removed)
                {
                    p_AreaTrigger->Remove();
                    return;
                }

                std::list<Player*> l_PlayerList;
                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);
                l_PlayerList.remove_if(CheckDeadTarget());

                if (!l_PlayerList.empty())
                {
                    for (Player* l_Player : l_PlayerList)
                    {
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (!l_Player || l_Player->IsImmunedToSpell(p_SpellInfo) || l_Player->HasAura(eSpells::BefoulingInk))
                                continue;

                            if (l_Removed)
                                break;

                            p_AreaTrigger->Remove();
                            l_Removed = true;
                            l_Player->CastSpell(l_Player, eSpells::BefoulingInk, true);
                            break;
                        }
                    }
                }
            }
            else
                m_CheckTimer -= p_Time;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_sasszine_befouling_ink();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Beckon Sarukel - 232746
class spell_sasszine_beckon_sarukel : public SpellScriptLoader
{
    public:
        spell_sasszine_beckon_sarukel() : SpellScriptLoader("spell_sasszine_beckon_sarukel") { }

        class spell_sasszine_beckon_sarukel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_beckon_sarukel_SpellScript);

            void HandleSendEvent(SpellEffIndex /*effIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    std::list<Creature*> l_SarukelList;
                    l_Caster->GetCreatureListWithEntryInGrid(l_SarukelList, eMisc::NpcSarukel, 200.0f);
                    if (l_SarukelList.empty())
                        return;
                    ///< Remove Aura and Events from old Sarukel
                    for (Creature* l_OldSarukel : l_SarukelList)
                    {
                        l_OldSarukel->CastStop();
                        l_OldSarukel->m_Events.KillAllEvents(true);
                        l_OldSarukel->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTrigger);
                        l_OldSarukel->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTriggerLine);
                        l_OldSarukel->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTriggerWhole);
                    }
                    ///< Remove AreaTriggers from old Sarukel
                    if (InstanceScript* p_instance = l_Caster->GetInstanceScript())
                    {
                        if (Creature* l_Sasszine = ObjectAccessor::GetCreature(*l_Caster, p_instance->GetData64(eData::DataMistressSasszine)))
                        {
                            l_Sasszine->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTrigger, 200.f);
                            l_Sasszine->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTriggerLine, 200.f);
                            l_Sasszine->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTriggerWhole, 200.f);
                        }
                    }

                    ///< Generate new Sarukel
                    JadeCore::RandomResizeList(l_SarukelList, 1);
                    for (Creature* l_Sarukel : l_SarukelList)
                    {
                        l_Sarukel->CastSpell(l_Sarukel, eSpells::DevouringMawVisualFirst, true);
                        l_Sarukel->CastSpell(l_Sarukel, eSpells::DevouringMawVisualSecond, true);
                        l_Sarukel->m_Events.AddEvent(new EventDevouringMaw(l_Sarukel), l_Sarukel->m_Events.CalculateTime(2000));
                    }
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_sasszine_beckon_sarukel_SpellScript::HandleSendEvent, EFFECT_1, SPELL_EFFECT_SEND_EVENT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sasszine_beckon_sarukel_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Devouring Maw - 232745
class spell_at_sasszine_devouring_maw : public AreaTriggerEntityScript
{
    public:
        spell_at_sasszine_devouring_maw() : AreaTriggerEntityScript("spell_at_sasszine_devouring_maw") {}

        uint32 m_CheckTimer = 500;
        float l_Radius = 200.f; /// From sniff
        std::list<uint64> m_Targets;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
             if (!p_AreaTrigger || !p_AreaTrigger->IsInWorld())
                return;

            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 1000;
                if (!p_AreaTrigger)
                    return;

                auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
                if (!l_AreaTriggerCaster)
                    return;

                std::list<Player*> l_PlayerList;
                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                if (!l_PlayerList.empty())
                {
                    l_PlayerList.remove_if(CheckDeadTarget());
                    for (Player* l_Player : l_PlayerList)
                    {
                        if (!l_Player || !l_Player->IsInWorld())
                            continue;

                        if (!l_Player->HasAura(eSpells::DevouringMaw))
                            l_Player->CastSpell(l_Player, eSpells::DevouringMaw, true);

                        if (std::find(m_Targets.begin(), m_Targets.end(), l_Player->GetGUID()) != m_Targets.end())
                            continue;

                        l_Player->SendApplyMovementForce(p_AreaTrigger->GetGUID(), true, *p_AreaTrigger, 4.f, 1); ///< From sniff
                        m_Targets.push_back(l_Player->GetGUID());
                    }
                }
            }
            else
                m_CheckTimer -= p_Time;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (!p_AreaTrigger)
                return;

            auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
            if (!l_AreaTriggerCaster)
                return;

            InstanceScript* p_instance = l_AreaTriggerCaster->GetInstanceScript();
            if (!p_instance)
                return;

            p_instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DevouringMaw);

            Map::PlayerList const& l_Players = p_instance->instance->GetPlayers();
            for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                if (Player* l_Player = l_Iter->getSource())
                    if (l_Player->IsInWorld() && !l_Player->isGameMaster() && l_Player->HasMovementForce(p_AreaTrigger->GetGUID()))
                        l_Player->SendApplyMovementForce(p_AreaTrigger->GetGUID(), false, Position());
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_sasszine_devouring_maw();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Dark Depths - 230273
class spell_sasszine_dark_depths : public SpellScriptLoader
{
    public:
        spell_sasszine_dark_depths() : SpellScriptLoader("spell_sasszine_dark_depths") { }

        class spell_sasszine_dark_depths_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_dark_depths_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (InstanceScript* p_instance = l_Caster->GetInstanceScript())
                    if (Unit* l_Sasszine = ObjectAccessor::GetUnit(*l_Caster, p_instance->GetData64(eData::DataMistressSasszine)))
                        if (Unit* l_Victim = l_Sasszine->getVictim())
                            p_Targets.remove(l_Victim);

                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::BurdenofPain));
                p_Targets.remove_if(JadeCore::UnitAuraCheck(true, eSpells::JawsfromtheDeep));
                p_Targets.remove_if(CheckDeadTarget());

                if (p_Targets.empty())
                    return;

                JadeCore::RandomResizeList(p_Targets, 1);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (l_Target->HasAura(eSpells::JawsfromtheDeep))
                    return;

                l_Caster->CastSpell(l_Target, eSpells::JawsfromtheDeep, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_dark_depths_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_sasszine_dark_depths_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sasszine_dark_depths_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Consuming Hunger - 230920
class spell_sasszine_consuming_hunger_triggered : public SpellScriptLoader
{
    public:
        spell_sasszine_consuming_hunger_triggered() : SpellScriptLoader("spell_sasszine_consuming_hunger_triggered") { }

        class spell_sasszine_consuming_hunger_triggered_Aurascript : public AuraScript
        {
            PrepareAuraScript(spell_sasszine_consuming_hunger_triggered_Aurascript);

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* I_Caster = GetCaster())
                {
                    if (Creature* pCreature = I_Caster->ToCreature())
                    {
                        InstanceScript* p_instance = pCreature->GetInstanceScript();
                        if (!p_instance)
                            return;

                        pCreature->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        pCreature->setFaction(14);
                        if (pCreature->AI())
                        {
                            pCreature->AI()->DoZoneInCombat();
                            pCreature->AI()->DoAction(0);
                            Creature* l_Sasszine = ObjectAccessor::GetCreature(*I_Caster, p_instance->GetData64(eData::DataMistressSasszine));
                            if (!l_Sasszine || !l_Sasszine->AI() || l_Sasszine->AI()->GetData(0) != ePhases::PHASE_TEN_THOUSAND_FANGS || !l_Sasszine->GetMap() || !l_Sasszine->GetMap()->IsMythic())
                                return;

                            std::list<Creature*> l_RazorjawWaverunnerList;
                            pCreature->GetCreatureListWithEntryInGrid(l_RazorjawWaverunnerList, eMisc::NpcRazorjawWaverunner, 15.0f);
                            if (l_RazorjawWaverunnerList.empty() || l_RazorjawWaverunnerList.size() < 2)
                                return;

                            for (Creature* murlocs : l_RazorjawWaverunnerList)
                                 murlocs->CastSpell(murlocs, eSpells::MurlocMash, true);
                        }
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sasszine_consuming_hunger_triggered_Aurascript::HandleRemove, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sasszine_consuming_hunger_triggered_Aurascript();
        }
};

/// NPC 115896 - Electrifying Jellyfish
class npcl_sasszine_electrifying_jellyfish : public CreatureScript
{
    public:
        npcl_sasszine_electrifying_jellyfish() : CreatureScript("npcl_sasszine_electrifying_jellyfish") { }

        struct npcl_sasszine_electrifying_jellyfishAI : public ScriptedAI
        {
            npcl_sasszine_electrifying_jellyfishAI(Creature* p_Creature) : ScriptedAI(p_Creature) {}

            enum eNpcEvents
            {
                EventCastThunderingShock = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
                m_Events.Reset();
                m_Events.ScheduleEvent(eNpcEvents::EventCastThunderingShock, 500);
                me->DespawnOrUnsummon(4500); ///< Despawn AfterCast
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case eNpcEvents::EventCastThunderingShock:
                    {
                        me->CastSpell(me, eSpells::ThunderingShockDamage, false);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npcl_sasszine_electrifying_jellyfishAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Devouring Maw - 232886
class spell_at_sasszine_devouring_maw_line : public AreaTriggerEntityScript
{
    public:
        spell_at_sasszine_devouring_maw_line() : AreaTriggerEntityScript("spell_at_sasszine_devouring_maw_line") {}

        uint32 m_CheckTimer = 500;
        std::list<uint64> m_Targets;

        void FinishInk(Unit* l_AreaTriggerCaster)
        {
            if (!l_AreaTriggerCaster)
                return;

            l_AreaTriggerCaster->CastStop();
            l_AreaTriggerCaster->m_Events.KillAllEvents(true);
            l_AreaTriggerCaster->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTrigger);
            l_AreaTriggerCaster->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTriggerLine);
            l_AreaTriggerCaster->RemoveAurasDueToSpell(eSpells::DevouringMawAreaTriggerWhole);
            if (InstanceScript* p_instance = l_AreaTriggerCaster->GetInstanceScript())
            {
                if (Creature* l_Sasszine = ObjectAccessor::GetCreature(*l_AreaTriggerCaster, p_instance->GetData64(eData::DataMistressSasszine)))
                {
                    l_Sasszine->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTrigger, 200.f);
                    l_Sasszine->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTriggerLine, 200.f);
                    l_Sasszine->DespawnAreaTriggersInArea(eSpells::DevouringMawAreaTriggerWhole, 200.f);
                }
            }
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (!p_AreaTrigger || !p_AreaTrigger->IsInWorld())
                return;

            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 500;

                auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
                if (!l_AreaTriggerCaster)
                    return;

                float l_Radius = (p_AreaTrigger->GetRadius() - 1.f);
                std::list<Player*> l_PlayerList;
                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                if (!l_PlayerList.empty())
                {
                    l_PlayerList.remove_if(CheckDeadTarget());
                    for (Player* l_Player : l_PlayerList)
                    {
                        if (!l_Player || !l_Player->IsInWorld())
                            continue;

                        if (l_Player->HasAura(eSpells::BefoulingInk))
                        {
                            l_Player->RemoveAurasDueToSpell(eSpells::BefoulingInk);
                            m_Targets.push_back(l_Player->GetGUID());
                            continue;
                        }
                    }
                }

                if (m_Targets.size() >= (l_AreaTriggerCaster->GetMap()->IsMythic() ? 5 : 3))
                {
                    FinishInk(l_AreaTriggerCaster);
                    m_Targets.clear();
                }
            }
            else
                m_CheckTimer -= p_Time;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_sasszine_devouring_maw_line();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Devouring Maw - 232884
class spell_at_sasszine_devouring_maw_whole : public AreaTriggerEntityScript
{
    public:
        spell_at_sasszine_devouring_maw_whole() : AreaTriggerEntityScript("spell_at_sasszine_devouring_maw_whole") {}

        uint32 m_CheckTimer = 500;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (!p_AreaTrigger || !p_AreaTrigger->IsInWorld())
                return;

            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 500;

                auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
                if (!l_AreaTriggerCaster)
                    return;

                float l_Radius = (p_AreaTrigger->GetRadius() - 1.f);
                std::list<Player*> l_PlayerList;
                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                if (!l_PlayerList.empty())
                {
                    l_PlayerList.remove_if(CheckDeadTarget());
                    for (Player* l_Player : l_PlayerList)
                    {
                        if (!l_Player || !l_Player->IsInWorld())
                            continue;

                        l_AreaTriggerCaster->CastSpell(l_Player, eSpells::DevourWhole, true);
                    }
                }
            }
            else
               m_CheckTimer -= p_Time;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_sasszine_devouring_maw_whole();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Crashing Wave - 232827
class spell_sasszine_crashing_wave : public SpellScriptLoader
{
    public:
        spell_sasszine_crashing_wave() : SpellScriptLoader("spell_sasszine_crashing_wave") { }

        class spell_sasszine_crashing_wave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_crashing_wave_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                float l_Radius = GetSpellInfo()->Effects[0].CalcRadius(l_Caster);
                float l_Width = 16.f;
                p_Targets.remove_if([l_Radius, l_Caster, l_Width](WorldObject* p_Object) -> bool
                {
                    if (!p_Object || !p_Object->IsInAxe(l_Caster, l_Width, l_Radius))
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_crashing_wave_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_crashing_wave_SpellScript::CorrectTargets, EFFECT_1, TARGET_UNIT_ENEMY_BETWEEN_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sasszine_crashing_wave_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Burden of Pain - 230201
class spell_sasszine_burden_of_pain: public SpellScriptLoader
{
    public:
        spell_sasszine_burden_of_pain() : SpellScriptLoader("spell_sasszine_burden_of_pain") { }

        class spell_sasszine_burden_of_pain_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sasszine_burden_of_pain_AuraScript);

            enum MonkSpells
            {
                LightStagger        = 124275,
                ModerateStagger     = 124274,
                HeavyStagger        = 124273
            };

            void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*auraEffect*/, DamageInfo& dmgInfo, uint32& /*absorbAmount*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (dmgInfo.GetSpellInfo() && (dmgInfo.GetSpellInfo()->Id == eSpells::SlicingTornadoDamage || dmgInfo.GetSpellInfo()->Id == MonkSpells::LightStagger || dmgInfo.GetSpellInfo()->Id == MonkSpells::ModerateStagger || dmgInfo.GetSpellInfo()->Id == MonkSpells::HeavyStagger))
                    return;

                bool l_CheckSchool = (dmgInfo.GetSchoolMask() == SpellSchoolMask::SPELL_SCHOOL_MASK_NORMAL);
                if (!l_CheckSchool) ///< Check on school
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BurdenofPain, l_Target->GetMap()->GetDifficultyID());
                if (!l_SpellInfo)
                    return;

                int32 l_Damage = (dmgInfo.GetAmount() + dmgInfo.GetAbsorb());
                if (!l_Damage)
                    return;

                l_Damage = CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                l_Target->CastCustomSpell(l_Target, eSpells::BurdenofPainDamage, &l_Damage, NULL, NULL, true);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_sasszine_burden_of_pain_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_sasszine_burden_of_pain_AuraScript::Absorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sasszine_burden_of_pain_AuraScript();
        }
};

/// Last Update: 7.3.5 build 26365
/// Called by Slicing Tornado Damage 232732
class spell_sasszine_slicing_tornado_damage : public SpellScriptLoader
{
public:
    spell_sasszine_slicing_tornado_damage() : SpellScriptLoader("spell_sasszine_slicing_tornado_damage") { }

    class spell_sasszine_slicing_tornado_damage_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sasszine_slicing_tornado_damage_AuraScript);

        void OnPeriodic(AuraEffect const* p_AuraEffect)
        {
            Unit* l_Target = GetTarget();
            if (!l_Target || !p_AuraEffect)
                return;

            if (!l_Target->HasAura(eSpells::BurdenofPain))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BurdenofPain, l_Target->GetMap()->GetDifficultyID());
            if (!l_SpellInfo)
                return;

            int32 l_Damage = p_AuraEffect->GetAmount();
            if (!l_Damage)
                return;

            l_Damage = CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
            l_Target->CastCustomSpell(l_Target, eSpells::BurdenofPainDamage, &l_Damage, NULL, NULL, true);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_sasszine_slicing_tornado_damage_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_sasszine_slicing_tornado_damage_AuraScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Called by Delicious Bufferfish - 239369
class spell_sasszine_delicious_bufferfish : public SpellScriptLoader
{
    public:
        spell_sasszine_delicious_bufferfish() : SpellScriptLoader("spell_sasszine_delicious_bufferfish") { }

        class spell_sasszine_delicious_bufferfish_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sasszine_delicious_bufferfish_AuraScript);

            bool IsHealer(SpecIndex p_Spec)
            {
                switch (p_Spec)
                {
                    case SPEC_DRUID_RESTORATION:
                    case SPEC_PRIEST_HOLY:
                    case SPEC_PRIEST_DISCIPLINE:
                    case SPEC_PALADIN_HOLY:
                    case SPEC_SHAMAN_RESTORATION:
                    case SPEC_MONK_MISTWEAVER:
                        return true;
                        break;
                    default:
                        break;
                }
                return false;
            }

            void AfterStackApply(AuraEffect const* /*p_Auraeffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Player* l_PlayerTarget = l_Target->ToPlayer();
                if (!l_PlayerTarget)
                    return;

                if (IsHealer((SpecIndex)l_PlayerTarget->GetActiveSpecializationID()))
                {
                    l_PlayerTarget->CastSpell(l_PlayerTarget, eSpells::DeliciousBufferfishManaReg, true);
                    if (Aura* l_Aura = l_PlayerTarget->GetAura(eSpells::DeliciousBufferfishManaReg))
                        l_Aura->SetStackAmount(10);
                }
                else
                {
                    l_PlayerTarget->CastSpell(l_PlayerTarget, eSpells::DeliciousBufferfishDmgDone, true);
                    if (Aura* l_Aura = l_PlayerTarget->GetAura(eSpells::DeliciousBufferfishDmgDone))
                        l_Aura->SetStackAmount(10);
                }
            }

            void Absorb(AuraEffect* /*auraEffect*/, DamageInfo& dmgInfo, uint32& /*absorbAmount*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->RemoveAurasDueToSpell(eSpells::DeliciousBufferfishManaReg);
                l_Target->RemoveAurasDueToSpell(eSpells::DeliciousBufferfishDmgDone);
                l_Target->RemoveAurasDueToSpell(eSpells::DreadSharkPlayerAura);
                l_Target->CastSpell(l_Target, eSpells::Sliperry, true);
                l_Target->CastSpell(l_Target, eSpells::DeliciousBufferfishSummon, true);
            }

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->RemoveAurasDueToSpell(eSpells::DeliciousBufferfishManaReg);
                l_Target->RemoveAurasDueToSpell(eSpells::DeliciousBufferfishDmgDone);
                l_Target->RemoveAurasDueToSpell(eSpells::DreadSharkPlayerAura);
            }

            void OnTick(const AuraEffect* /*p_AurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (Aura* l_AuraBufferfishManaReg = l_Target->GetAura(eSpells::DeliciousBufferfishManaReg))
                    if (l_AuraBufferfishManaReg->GetStackAmount() < 50)
                        l_AuraBufferfishManaReg->SetStackAmount(l_AuraBufferfishManaReg->GetStackAmount() + 1);

                if (Aura* l_AuraBufferfishDmgDone = l_Target->GetAura(eSpells::DeliciousBufferfishDmgDone))
                    if (l_AuraBufferfishDmgDone->GetStackAmount() < 50)
                        l_AuraBufferfishDmgDone->SetStackAmount(l_AuraBufferfishDmgDone->GetStackAmount() + 1);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_sasszine_delicious_bufferfish_AuraScript::AfterStackApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_sasszine_delicious_bufferfish_AuraScript::Absorb, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sasszine_delicious_bufferfish_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sasszine_delicious_bufferfish_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sasszine_delicious_bufferfish_AuraScript();
        }
};

/// NPC 119791 - Delicious Bufferfish
class npc_sasszine_delicious_bufferfish : public CreatureScript
{
    public:
        npc_sasszine_delicious_bufferfish() : CreatureScript("npc_sasszine_delicious_bufferfish") { }

        struct npc_sasszine_delicious_bufferfishAI : public ScriptedAI
        {
            npc_sasszine_delicious_bufferfishAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint32 l_CDTimer = 500;
            float l_Radius = 2.f;
            bool UseSpell;

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->CastSpell(me, eSpells::DeliciousBufferfishAT, true);
                UseSpell = false;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (l_CDTimer <= p_Diff)
                {
                    l_CDTimer = 500;
                    if (UseSpell)
                        return;

                    std::list<Player*> l_PlayerList;
                    JadeCore::AnyPlayerInObjectRangeCheck l_Check(me, l_Radius);
                    JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(me, l_PlayerList, l_Check);
                    me->VisitNearbyObject(l_Radius, l_Searcher);

                    if (!l_PlayerList.empty())
                    {
                        l_PlayerList.remove_if(CheckDeadTarget());
                        for (Player* l_Player : l_PlayerList)
                        {
                            if (UseSpell)
                                break;

                            if (!l_Player || l_Player->HasAura(eSpells::DeliciousBufferfish) || l_Player->HasAura(eSpells::Sliperry))
                                continue;

                            l_Player->CastSpell(l_Player, eSpells::DeliciousBufferfish, true);
                            me->DespawnOrUnsummon();
                            UseSpell = true;
                            break;
                        }
                    }
                }
                else
                   l_CDTimer -= p_Diff;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_sasszine_delicious_bufferfishAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Dread Shark - 239423
class spell_sasszine_dread_shark : public SpellScriptLoader
{
    public:
        spell_sasszine_dread_shark() : SpellScriptLoader("spell_sasszine_dread_shark") { }

        class spell_sasszine_dread_shark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sasszine_dread_shark_AuraScript);

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::DreadSharkTargetPlayer, true);
                l_Target->CastSpell(l_Target, eSpells::DreadSharkTargetNPC, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sasszine_dread_shark_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sasszine_dread_shark_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Dread Shark - 239430
class spell_sasszine_dread_shark_npc_visual : public SpellScriptLoader
{
    public:
        spell_sasszine_dread_shark_npc_visual() : SpellScriptLoader("spell_sasszine_dread_shark_npc_visual") { }

        class spell_sasszine_dread_shark_npc_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_dread_shark_npc_visual_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (p_Object->GetEntry() != eMisc::NpcDeliciousBufferfish)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_dread_shark_npc_visual_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sasszine_dread_shark_npc_visual_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Dread Shark - 239434
class spell_sasszine_dread_shark_target_player : public SpellScriptLoader
{
    public:
        spell_sasszine_dread_shark_target_player() : SpellScriptLoader("spell_sasszine_dread_shark_target_player") { }

        class spell_sasszine_dread_shark_target_player_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_dread_shark_target_player_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->IsPlayer() || !p_Object->ToPlayer()->HasAura(eSpells::DreadSharkPlayerAura))
                        return true;

                    return false;
                });
            }

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::DreadSharkDamage, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_dread_shark_target_player_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                AfterHit += SpellHitFn(spell_sasszine_dread_shark_target_player_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sasszine_dread_shark_target_player_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Dread Shark - 239435
class spell_sasszine_dread_shark_target_npc : public SpellScriptLoader
{
    public:
        spell_sasszine_dread_shark_target_npc() : SpellScriptLoader("spell_sasszine_dread_shark_target_npc") { }

        class spell_sasszine_dread_shark_target_npc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sasszine_dread_shark_target_npc_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || !p_Object->ToUnit() || !p_Object->ToUnit()->HasAura(eSpells::DreadSharkNPCAura))
                        return true;

                    return false;
                });
            }

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::DreadSharkDamage, true);
                if (Creature* l_Creature = l_Target->ToCreature())
                    l_Creature->DespawnOrUnsummon();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sasszine_dread_shark_target_npc_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                AfterHit += SpellHitFn(spell_sasszine_dread_shark_target_npc_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_sasszine_dread_shark_target_npc_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Befouling Ink - 232913
class spell_sasszine_befouling_ink : public SpellScriptLoader
{
    public:
        spell_sasszine_befouling_ink() : SpellScriptLoader("spell_sasszine_befouling_ink") { }

        class spell_sasszine_befouling_ink_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sasszine_befouling_ink_AuraScript);

            void HandleRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                InstanceScript* p_instance = l_Target->GetInstanceScript();
                if (!p_instance || p_instance->GetBossState(eData::DataMistressSasszine) != IN_PROGRESS)
                    return;

                l_Target->CastSpell(l_Target, eSpells::BefoulingInkCaster, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sasszine_befouling_ink_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sasszine_befouling_ink_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_mistress_sasszine()
{
    ///< Boss >>
    new boss_mistress_sasszine();
    new spell_sasszine_thundering_shock();
    new spell_sasszine_consuming_hunger();
    new spell_at_sasszine_concealing_murk();
    new spell_sasszine_hydra_shot();
    new spell_sasszine_hydra_shot_aura();
    new spell_sasszine_hydra_shot_damage();
    new spell_sasszine_hydra_acid_aura();
    new spell_sasszine_slicing_tornado();
    new spell_at_sasszine_slicing_tornado();
    new spell_sasszine_call_vellius();
    new spell_sasszine_summon_ossunet();
    new spell_at_sasszine_befouling_ink();
    new spell_sasszine_beckon_sarukel();
    new spell_at_sasszine_devouring_maw();
    new spell_sasszine_dark_depths();
    new spell_sasszine_consuming_hunger_triggered();
    new spell_at_sasszine_devouring_maw_line();
    new spell_at_sasszine_devouring_maw_whole();
    new spell_sasszine_crashing_wave();
    new spell_sasszine_burden_of_pain();
    new spell_sasszine_slicing_tornado_damage();
    new spell_sasszine_befouling_ink();

    ///<NPC >>
    new npc_sasszine_razorjaw_waverunner();
    new npc_sasszine_abyss_stalker();
    new npcl_sasszine_electrifying_jellyfish();

    ///< Mythic >>
    new spell_sasszine_delicious_bufferfish();
    new npc_sasszine_delicious_bufferfish();
    new spell_sasszine_dread_shark();
    new spell_sasszine_dread_shark_npc_visual();
    new spell_sasszine_dread_shark_target_player();
    new spell_sasszine_dread_shark_target_npc();
}
#endif
