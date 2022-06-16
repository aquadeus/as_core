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

enum eSpells
{
    ///<< Boss Spell >>
    CommandingRoar  = 232192, ///< Summon Npcs
    UncheckedRage   = 231854, ///< Front damage mechanic
    UncheckedRageTarget = 247403, ///< Casted only in Tank
    Enraged         = 247781, ///< used when < 5 targets UncheckedRage
    DrawIn          = 232061, ///< Move AreaTriggers
    Berserk         = 64238,
    AbrasiveArmor   = 233071, ///< Aura! Proc for JaggedAbrasion (creature_template_addon)
    JaggedAbrasion  = 231998, ///<  Triggered by AbrasiveArmor 233071
    FrigidBlows     = 233429, ///< Use After DrawIn
    FrostyDischarge = 232174, ///< remove stack Drenched 231770. Used after remove all stacks FrigidBlows
    DrenchingSlough = 233526,
    DrenchingSloughAreaTrigger = 233530,
    DrenchingWaters = 231768,
    CancelDrenchedAura = 239926, ///< Special for Remove Drenched Aura
    DrenchedAura = 231770,
    OverrideEnergyBarColor = 241486, ///< Runic power for Harjatan from sniff
    GatherEnergy   = 232379,
    ///< Mythic
    Hatching       = 240319,
    WarningIncubatedEgg = 240347,
    HardenedShell  = 240315,
    HardenedShellVisualRed   = 246491,
    HardenedShellVisualBlue  = 246492,
    HardenedShellVisualGreen = 246493,
    HatchingSummonRed   = 240360,
    HatchingSummonBlue  = 241562,
    HatchingSummonGreen = 241563,

    ///<< Npc Spell >>
    ///< RazorjawWavemender
    AqueousBurst   = 231729,
    WaterySplash   = 241514,
    AqueousBurstAreaTrigger = 231754,
    ///< RazorjawGladiator
    DrivenAssault  = 234016,
    DrivenAssaultAura = 234128,
    SplashyCleave   = 234129,
    DrippingBlade   = 239907, ///< Aura proc (creature_template_addon)
    ///<< Mythic >>
    EnragedAfterKill = 247782,
    ///< Colicky Tadpole
    Tantrum      = 241590,
    TantrumJump  = 241591,
    TantrumJumpDamage = 241592,
    GettingAngry = 241594,
    ///< Sickly Tadpole
    SicklyFixate = 241600,
    RottenEmission = 241603,
    RottenEmissionDeathAura = 243115,
    ///< Drippy Tadpole
    DrippingWet  = 241573,
    DrippingWetAreaTrigger = 241574
};

enum eEvents
{
    EventCommandingRoar = 1,
    EventDrawIn,
    EventBerserk,
    EventIncubatedEgg
};

enum eMisc
{
    NpcRazorjawWavemender             = 116569,
    NpcRazorjawGladiator              = 117596,
    NpcYellMistressSasszine           = 121190,
    ///< Mythic
    NpcColickyTadpole                 = 121156,
    NpcSicklyTadpole                  = 121155,
    NpcDrippyTadpole                  = 120574
};

enum eAction
{
    ActionSummonNpc = 1,
    ActionAfterCastFrostyDischarge,
    ActionCountStackDrenchingWaters,
    ActionCastFrigidBlows,
    ActionCastDrawIn
};

const Position RazorjawSpawnPosition[] =
{
    {5885.45f, -898.54f, 2919.86f, 5.53f}, ///< NpcRazorjawGladiator
    {5913.46f, -870.28f, 2919.90f, 5.19f}, ///< NpcRazorjawWavemender
    {5947.40f, -932.73f, 2919.87f, 2.93f}, ///< NpcRazorjawWavemender
    {5948.37f, -899.69f, 2919.86f, 3.12f} ///< NpcRazorjawGladiator
};

enum eTalks
{
    TALK_YELL_SASSZINE  = 0,
    TALK_AGGRO,
    TALK_COMMANDINGROAR,
    TALK_COMMANDINGROAR2,
    TALK_COMMANDINGROAR3,
    TALK_SLAY,
    TALK_DIED,
    TALK_DIED2
};

enum eAchievHarjatan
{
    AchievementGrandFin_Ale    = 11699, ///< TODO: Need fix http://www.wowhead.com/item=143660 and summon npc http://www.wowhead.com/npc=121071
    AchievementMythicHarjatan  = 11775
};

enum eColorIncubatedEggTactics
{
    IncubatedEggTacticRed = 1,
    IncubatedEggTacticBlue,
    IncubatedEggTacticGreen
};

class EventYellSassZine: public BasicEvent
{
    public:

    explicit EventYellSassZine(Creature* p_Creature) : m_Creature(p_Creature)
    {
    }

    bool Execute(uint64 /*p_CurrTime*/, uint32 /*p_Diff*/)
    {
        if (m_Creature)
        {
            if (m_Creature->AI())
                m_Creature->AI()->Talk(eTalks::TALK_YELL_SASSZINE);

            if (InstanceScript* p_instance = m_Creature->GetInstanceScript())
                p_instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::DrenchedAura);

            m_Creature->SetVisible(false);
        }

        return true;
    }

private:
    Creature* m_Creature;
};

class boss_harjatan : public CreatureScript
{
    public:
        boss_harjatan() : CreatureScript("boss_harjatan") { }

        struct boss_harjatanAI : public BossAI
        {
            boss_harjatanAI(Creature* creature) : BossAI(creature, eData::DataHarjatan)
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            }

            uint32 AbrasiveArmor_CDTimer;
            uint8 _stackDrenchingWaters;

            void InitializeAI() override
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(TOSScriptName))
                    me->IsAIEnabled = false;
                else if (!me->isDead())
                    Reset();
            }

            void RespawnMythicEggs()
            {
                std::list<Creature*> l_Creatures;
                me->GetCreatureListWithEntryInGrid(l_Creatures, eCreatures::NpcIncubatedEgg, 200.f);
                for (Creature* l_Eggs : l_Creatures)
                {
                    l_Eggs->RemoveAura(eSpells::HardenedShellVisualRed);
                    l_Eggs->RemoveAura(eSpells::HardenedShellVisualBlue);
                    l_Eggs->RemoveAura(eSpells::HardenedShellVisualGreen);
                    l_Eggs->RemoveAura(eSpells::HardenedShell);
                    l_Eggs->CastStop();
                    if (l_Eggs->AI())
                        l_Eggs->AI()->EnterEvadeMode();

                    l_Eggs->NearTeleportTo(l_Eggs->GetHomePosition());
                    l_Eggs->Respawn(true, true);
                    l_Eggs->UpdateStatsForLevel();
                    if (!l_Eggs->HasFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE))
                        l_Eggs->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                }
            }

            void ClearOther()
            {
                me->RemoveAurasDueToSpell(eSpells::AbrasiveArmor);
                me->RemoveAurasDueToSpell(eSpells::GatherEnergy);
                me->RemoveAurasDueToSpell(eSpells::OverrideEnergyBarColor);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::AqueousBurst);
                    instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::JaggedAbrasion);
                }

                me->DespawnAreaTriggersInArea(eSpells::DrenchingSloughAreaTrigger, 200.f);
                me->DespawnAreaTriggersInArea(eSpells::AqueousBurstAreaTrigger, 200.f);
                me->DespawnAreaTriggersInArea(eSpells::DrippingWetAreaTrigger, 200.f);
                me->SetPower(POWER_ENERGY, 0);
                if (IsMythic())
                {
                    RespawnMythicEggs();
                    std::vector<uint32> p_TadpoleEntrys = { eMisc::NpcColickyTadpole, eMisc::NpcSicklyTadpole, eMisc::NpcDrippyTadpole };
                    me->DespawnCreaturesInArea(p_TadpoleEntrys, 200.f);
                }
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                ClearOther();
                AbrasiveArmor_CDTimer = 0;
                _stackDrenchingWaters = 0;
                if (instance->GetData(eData::DataPreEventHarjatan) == DONE)
                    me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                me->RemoveAurasDueToSpell(eSpells::AbrasiveArmor);
                me->SetPower(POWER_ENERGY, 0);
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

            void SummonRazorjaw()
            {
                if (!IsLFR()) ///< in LFR not mechanic with NpcRazorjawGladiator
                {
                    if (Creature* razorjawGlad = me->SummonCreature(eMisc::NpcRazorjawGladiator, RazorjawSpawnPosition[0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000)) ///< top right
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, razorjawGlad);
                    me->SummonCreature(eMisc::NpcRazorjawGladiator, RazorjawSpawnPosition[3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000); ///< bottom left
                }
                if (Creature* razorjawWav = me->SummonCreature(eMisc::NpcRazorjawWavemender, RazorjawSpawnPosition[1], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000)) ///< bottom right
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, razorjawWav);
                ///< In LFR 2 Frame for NpcRazorjawWavemender because without NpcRazorjawGladiator
                if (IsLFR())
                {
                    if (Creature* razorjawWav2 = me->SummonCreature(eMisc::NpcRazorjawWavemender, RazorjawSpawnPosition[2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000)) ///< bottom right
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, razorjawWav2);
                }
                else
                    me->SummonCreature(eMisc::NpcRazorjawWavemender, RazorjawSpawnPosition[2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1800000); ///< top left

            }

            void RescheduleEventsAfterFrostyDischarge()
            {
                me->SetPower(POWER_ENERGY, 0);
                events.RescheduleEvent(eEvents::EventCommandingRoar, 17 * IN_MILLISECONDS);
                events.RescheduleEvent(eEvents::EventDrawIn, 59 * IN_MILLISECONDS);
                if (IsMythic())
                {
                    events.RescheduleEvent(eEvents::EventIncubatedEgg, 30 * IN_MILLISECONDS);
                    RespawnMythicEggs();
                }
            }

            void CancelEventsAfterCastDrawIn()
            {
                events.CancelEvent(eEvents::EventCommandingRoar);
                events.CancelEvent(eEvents::EventDrawIn);
                if (IsMythic())
                    events.CancelEvent(eEvents::EventIncubatedEgg);
            }

            void DoAction(int32 const p_Action) override
            {
                switch(p_Action)
                {
                    case eAction::ActionSummonNpc:
                         SummonRazorjaw();
                         break;
                    case eAction::ActionAfterCastFrostyDischarge:
                         RescheduleEventsAfterFrostyDischarge();
                         break;
                    case eAction::ActionCountStackDrenchingWaters:
                         _stackDrenchingWaters++;
                         break;
                    case eAction::ActionCastFrigidBlows:
                    {
                         if (_stackDrenchingWaters)
                         {
                             me->SetAuraStack(eSpells::FrigidBlows, me, _stackDrenchingWaters);
                             me->AddAura(eSpells::OverrideEnergyBarColor, me);
                             CancelEventsAfterCastDrawIn();
                         }
                         else ///< not stacks re-apply events
                         {
                             RescheduleEventsAfterFrostyDischarge();
                             me->RemoveAurasDueToSpell(eSpells::OverrideEnergyBarColor);
                         }
                         _stackDrenchingWaters = 0;
                         break;
                    }
                    case eAction::ActionCastDrawIn:
                         CancelEventsAfterCastDrawIn();
                         break;
                    default:
                         break;
                }
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                _stackDrenchingWaters = 0;
                me->SetPower(POWER_ENERGY, 0);
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
                    Talk(eTalks::TALK_SLAY);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                if (IsMythic())
                    instance->DoCompleteAchievement(eAchievHarjatan::AchievementMythicHarjatan);

                Talk(urand(eTalks::TALK_DIED, eTalks::TALK_DIED2));
                if (Creature* l_Sasszine = me->FindNearestCreature(eMisc::NpcYellMistressSasszine, 500.0f))
                    l_Sasszine->m_Events.AddEvent(new EventYellSassZine(l_Sasszine), l_Sasszine->m_Events.CalculateTime(3 * TimeConstants::IN_MILLISECONDS));

                ClearOther();
            }

            void EnterCombat(Unit* /*attacker*/) override
            {
                 Talk(eTalks::TALK_AGGRO);
                _EnterCombat();
                me->RemoveAurasDueToSpell(eSpells::AbrasiveArmor);

                events.ScheduleEvent(eEvents::EventCommandingRoar, 17500);
                events.ScheduleEvent(eEvents::EventDrawIn, 58 * IN_MILLISECONDS);
                if (!IsLFR())
                    events.ScheduleEvent(eEvents::EventBerserk, (IsMythic() ? 360 : 480) * IN_MILLISECONDS);

                me->AddAura(eSpells::GatherEnergy, me);
                if (IsMythic())
                {
                    events.ScheduleEvent(eEvents::EventIncubatedEgg, 30500);
                    RespawnMythicEggs();
                }
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
                    me->AttackerStateUpdate(l_Victim);
                    me->resetAttackTimer();

                    if (AbrasiveArmor_CDTimer == 0)
                    {
                        AbrasiveArmor_CDTimer = 5000;
                        DoCastVictim(eSpells::JaggedAbrasion);
                    }
                }

                if (me->haveOffhandWeapon() && me->isAttackReady(WeaponAttackType::OffAttack))
                {
                    me->AttackerStateUpdate(l_Victim, WeaponAttackType::OffAttack);
                    me->resetAttackTimer(WeaponAttackType::OffAttack);
                }
            }

            bool CheckRoom()
            {
                if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 55)
                {
                    EnterEvadeMode();
                    return false;
                }

                return true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (AbrasiveArmor_CDTimer > 0)
                {
                    if (AbrasiveArmor_CDTimer <= p_Diff)
                    {
                        AbrasiveArmor_CDTimer = 0;
                    } else AbrasiveArmor_CDTimer -= p_Diff;
                }

                if (me->GetPower(POWER_ENERGY) >= 100 && !me->HasUnitState(UnitState::UNIT_STATE_CASTING) && !me->HasAura(eSpells::OverrideEnergyBarColor) && !me->HasAura(eSpells::DrawIn))
                    DoCastAOE(eSpells::UncheckedRage);

                if (!UpdateVictim() || !CheckRoom())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case eEvents::EventCommandingRoar:
                        {
                            Talk(urand(eTalks::TALK_COMMANDINGROAR, eTalks::TALK_COMMANDINGROAR3));
                            DoCast(eSpells::CommandingRoar);
                            events.ScheduleEvent(eEvents::EventCommandingRoar, 32800);
                            break;
                        }
                        case eEvents::EventDrawIn:
                        {
                            DoCast(eSpells::DrawIn);
                            events.ScheduleEvent(eEvents::EventDrawIn, 59 * IN_MILLISECONDS);
                            break;
                        }
                        case eEvents::EventBerserk:
                        {
                            DoCast(eSpells::Berserk);
                            break;
                        }
                        case eEvents::EventIncubatedEgg:
                        {
                            std::list<Creature*> l_Creatures;
                            me->GetCreatureListWithEntryInGrid(l_Creatures, eCreatures::NpcMythicEggController, 200.f);
                            JadeCore::RandomResizeList(l_Creatures, 2);
                            uint8 l_RandTactic = urand(eColorIncubatedEggTactics::IncubatedEggTacticRed, eColorIncubatedEggTactics::IncubatedEggTacticGreen);
                            for (Creature* l_EggController : l_Creatures)
                            {
                                if (l_EggController->AI())
                                    l_EggController->AI()->DoAction(l_RandTactic);

                                ///< Generate new tactic for second target
                                if (l_RandTactic == eColorIncubatedEggTactics::IncubatedEggTacticRed)
                                    l_RandTactic = urand(eColorIncubatedEggTactics::IncubatedEggTacticBlue, eColorIncubatedEggTactics::IncubatedEggTacticGreen);
                                else if (l_RandTactic == eColorIncubatedEggTactics::IncubatedEggTacticGreen)
                                    l_RandTactic = urand(eColorIncubatedEggTactics::IncubatedEggTacticRed, eColorIncubatedEggTactics::IncubatedEggTacticBlue);
                                else if (l_RandTactic == eColorIncubatedEggTactics::IncubatedEggTacticBlue)
                                {
                                    if (roll_chance_i(60))
                                        l_RandTactic = eColorIncubatedEggTactics::IncubatedEggTacticRed;
                                    else
                                        l_RandTactic = eColorIncubatedEggTactics::IncubatedEggTacticGreen;
                                }
                            }
                            events.ScheduleEvent(eEvents::EventIncubatedEgg, 40600);
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

                if (summon->GetEntry() == eMisc::NpcRazorjawWavemender || summon->GetEntry() == eMisc::NpcRazorjawGladiator)
                    summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_harjatanAI(p_Creature);
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Commanding Roar - 232192
class spell_harjatan_commanding_roar : public SpellScriptLoader
{
    public:
        spell_harjatan_commanding_roar() : SpellScriptLoader("spell_harjatan_commanding_roar") { }

        class spell_harjatan_commanding_roar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_harjatan_commanding_roar_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* I_Caster = GetCaster())
                   if (Creature* I_Harjatan= I_Caster->ToCreature())
                       if (I_Harjatan->AI())
                           I_Harjatan->AI()->DoAction(eAction::ActionSummonNpc);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_harjatan_commanding_roar_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_harjatan_commanding_roar_SpellScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Unchecked Rage - 231854
class spell_harjatan_unchecked_rage : public SpellScriptLoader
{
    public:
        spell_harjatan_unchecked_rage() : SpellScriptLoader("spell_harjatan_unchecked_rage") { }

        class spell_harjatan_unchecked_rage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_harjatan_unchecked_rage_SpellScript);

            void HandleTargetSelect(std::list<WorldObject*>& p_Targets)
            {
                m_TargetCount = int32(p_Targets.size());
            }

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = l_Caster->getVictim())
                        l_Caster->CastSpell(l_Target, eSpells::UncheckedRageTarget, true);

                    if (m_TargetCount < 5)
                        l_Caster->CastSpell(l_Caster, eSpells::Enraged, true);

                    l_Caster->SetPower(POWER_ENERGY, 0);

                    if (l_Caster->GetMap() && l_Caster->GetMap()->IsMythic())
                    {
                        std::list<Creature*> l_Creatures;
                        l_Caster->GetCreatureListWithEntryInGrid(l_Creatures, eCreatures::NpcIncubatedEgg, 20.f);
                        for (Creature* l_Eggs : l_Creatures)
                        {
                            if (!l_Eggs->HasAura(eSpells::HardenedShell) || !l_Caster->isInFront(l_Eggs, M_PI / 1.64f))
                                continue;

                            l_Eggs->RemoveAura(eSpells::HardenedShell);
                            l_Eggs->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        }
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_harjatan_unchecked_rage_SpellScript::HandleTargetSelect, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
                AfterCast += SpellCastFn(spell_harjatan_unchecked_rage_SpellScript::HandleAfterCast);
            }

        private:
              int32 m_TargetCount = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_harjatan_unchecked_rage_SpellScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called by DrawIn - 232061
class spell_harjatan_drawn_in : public SpellScriptLoader
{
    public:
        spell_harjatan_drawn_in() : SpellScriptLoader("spell_harjatan_drawn_in") { }

        class spell_harjatan_drawn_in_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_harjatan_drawn_in_AuraScript);

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (Creature* I_Harjatan = l_Caster->ToCreature())
                    if (I_Harjatan->AI())
                        I_Harjatan->AI()->DoAction(eAction::ActionCastDrawIn);

                std::list<AreaTrigger*> l_DrenchingSloughAreaTriggerList;
                std::list<AreaTrigger*> l_AqueousBurstAreaTriggerList;
                std::list<AreaTrigger*> l_DrippingWetAreaTriggerList;
                l_Caster->GetAreaTriggerListWithSpellIDInRange(l_DrenchingSloughAreaTriggerList, eSpells::DrenchingSloughAreaTrigger, 200.f, true);
                l_Caster->GetAreaTriggerListWithSpellIDInRange(l_AqueousBurstAreaTriggerList, eSpells::AqueousBurstAreaTrigger, 200.f, true);
                l_Caster->GetAreaTriggerListWithSpellIDInRange(l_DrippingWetAreaTriggerList, eSpells::DrippingWetAreaTrigger, 200.f, true);

                for (AreaTrigger* l_Areatrigger : l_DrenchingSloughAreaTriggerList)
                {
                    l_Areatrigger->MoveAreaTrigger(l_Caster->GetPosition(), 8 * IN_MILLISECONDS);
                }

                for (AreaTrigger* l_At : l_AqueousBurstAreaTriggerList)
                {
                    l_At->MoveAreaTrigger(l_Caster->GetPosition(), 8 * IN_MILLISECONDS);
                }

                for (AreaTrigger* l_AtWet : l_DrippingWetAreaTriggerList)
                {
                    l_AtWet->MoveAreaTrigger(l_Caster->GetPosition(), 8 * IN_MILLISECONDS);
                }
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Creature* I_Harjatan = l_Caster->ToCreature();
                if (!I_Harjatan)
                    return;

                if (I_Harjatan->AI())
                    I_Harjatan->AI()->DoAction(eAction::ActionCastFrigidBlows);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_harjatan_drawn_in_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_harjatan_drawn_in_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_harjatan_drawn_in_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by DrenchingSlough - 233530, AqueousBurst - 231754
class spell_at_harjatan_drenching_waters : public AreaTriggerEntityScript
{
    public:
        spell_at_harjatan_drenching_waters() : AreaTriggerEntityScript("spell_at_harjatan_drenching_waters") {}

        std::list<uint64> m_Targets;
        uint32 m_CheckTimer = 500;
        float l_Radius = 5.0f;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_CheckTimer <= p_Time)
            {
                m_CheckTimer = 500;
                if (!p_AreaTrigger)
                     return;

                auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();
                if (!l_AreaTriggerCaster)
                    return;

                std::list<Player*> l_PlayerList;

                std::list<uint64> l_Targets(m_Targets);
                for (uint64 l_TargetGuid : l_Targets)
                {
                    Player* l_Player = ObjectAccessor::GetPlayer(*l_AreaTriggerCaster, l_TargetGuid);
                    if (!l_Player)
                        continue;

                    if (l_Player->HasAura(eSpells::DrenchingWaters) && l_Player->GetDistance(p_AreaTrigger) <= l_Radius)
                        continue;

                    if (l_Player->HasAura(eSpells::DrenchingWaters))
                        l_Player->RemoveAurasDueToSpell(eSpells::DrenchingWaters);

                    m_Targets.remove(l_TargetGuid);
                }

                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_PlayerList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                l_PlayerList.remove_if(CheckDeadTarget());
                if (!l_PlayerList.empty())
                {
                    for (Player* l_Player : l_PlayerList)
                    {
                        if (!l_Player)
                            continue;

                        if (!l_Player->HasAura(eSpells::DrenchingWaters))
                        {
                            l_Player->CastSpell(l_Player, eSpells::DrenchingWaters, true);
                            m_Targets.push_back(l_Player->GetGUID());
                        }
                    }
                }

                std::list<Creature*> l_Harjatan;
                JadeCore::AllCreaturesOfEntryInRange checker(p_AreaTrigger, eCreatures::NpcHarjatan, 1.f);
                JadeCore::CreatureListSearcher<JadeCore::AllCreaturesOfEntryInRange> searcher(p_AreaTrigger, l_Harjatan, checker);
                p_AreaTrigger->VisitNearbyObject(1.f, searcher);
                if (l_Harjatan.empty())
                    return;

                for (Creature* harjatan : l_Harjatan)
                {
                    if (!harjatan->HasAura(eSpells::DrawIn))
                        continue;

                    if (harjatan->AI())
                        harjatan->AI()->DoAction(eAction::ActionCountStackDrenchingWaters);
                    p_AreaTrigger->Remove();
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

                if (l_Player->HasAura(eSpells::DrenchingWaters))
                    l_Player->RemoveAurasDueToSpell(eSpells::DrenchingWaters);

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_harjatan_drenching_waters();
        }
};

/// << NPC >>

/// NPC 116569 - Razorjaw Wavemender
class npc_harjatan_razorjaw_wavemender : public CreatureScript
{
    public:
        npc_harjatan_razorjaw_wavemender() : CreatureScript("npc_harjatan_razorjaw_wavemender") { }

        struct npc_harjatan_razorjaw_wavemenderAI : public ScriptedAI
        {
            npc_harjatan_razorjaw_wavemenderAI(Creature* p_Creature) : ScriptedAI(p_Creature)
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
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum mEvents
            {
                EventAqueousBurst = 1,
                EventWaterySplash
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 250.0f);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(mEvents::EventWaterySplash, 3500);
                m_Events.ScheduleEvent(mEvents::EventAqueousBurst, 10 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                    p_instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            Player* SelectRandomPlayer(Creature* l_Harjatan, float l_Radius)
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return nullptr;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return nullptr;

                std::list<Player*> temp;
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (l_Harjatan->getVictim() != i->getSource() && ///< check tank
                        me->IsWithinDistInMap(i->getSource(), l_Radius) && i->getSource()->isAlive() && !i->getSource()->isGameMaster() && !i->getSource()->IsActiveSpecTankSpec())
                        temp.push_back(i->getSource());

                if (!temp.empty())
                {
                    std::list<Player*>::const_iterator j = temp.begin();
                    advance(j, rand()%temp.size());
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
                        case mEvents::EventAqueousBurst:
                        {
                            if (InstanceScript* p_instance = me->GetInstanceScript())
                            {
                                if (Creature* l_Harjatan = Unit::GetCreature(*me, p_instance->GetData64(eData::DataHarjatan)))
                                    if (Player* l_Player = SelectRandomPlayer(l_Harjatan, 40.f))
                                        DoCast(l_Player, eSpells::AqueousBurst);
                            }
                            m_Events.ScheduleEvent(mEvents::EventAqueousBurst, urand(15 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        }
                        case mEvents::EventWaterySplash:
                        {
                            if (InstanceScript* p_instance = me->GetInstanceScript())
                            {
                                if (Creature* l_Harjatan = Unit::GetCreature(*me, p_instance->GetData64(eData::DataHarjatan)))
                                    if (Player* l_Player = SelectRandomPlayer(l_Harjatan, 100.f))
                                        DoCast(l_Player, eSpells::WaterySplash);
                            }
                            m_Events.ScheduleEvent(mEvents::EventWaterySplash, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harjatan_razorjaw_wavemenderAI(p_Creature);
        }
};

/// NPC 117596 - Razorjaw Gladiator
class npc_harjatan_razorjaw_gladiator : public CreatureScript
{
    public:
        npc_harjatan_razorjaw_gladiator() : CreatureScript("npc_harjatan_razorjaw_gladiator") { }

        struct npc_harjatan_razorjaw_gladiatorAI : public ScriptedAI
        {
            npc_harjatan_razorjaw_gladiatorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
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
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT_2, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            }

            enum mEvents
            {
                EventSplashyCleave = 1,
                EventDrivenAssault
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            Player* SelectRandomPlayer(Unit* l_Harjatan)
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return nullptr;

                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (PlayerList.isEmpty())
                    return nullptr;

                std::list<Player*> temp;
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (l_Harjatan->getVictim() != i->getSource() && ///< check tank
                        me->IsWithinDistInMap(i->getSource(), 250.f) && i->getSource()->isAlive() && !i->getSource()->isGameMaster() && !i->getSource()->IsActiveSpecTankSpec())
                        temp.push_back(i->getSource());

                if (!temp.empty())
                {
                    std::list<Player*>::const_iterator j = temp.begin();
                    advance(j, rand()%temp.size());
                    return (*j);
                }
                return nullptr;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 250.0f);
                me->RemoveAurasDueToSpell(eSpells::DrippingBlade);
                DoCast(eSpells::DrippingBlade);

                if (InstanceScript* p_instance = me->GetInstanceScript())
                {
                    if (Unit* l_Harjatan = ObjectAccessor::GetUnit(*me, p_instance->GetData64(eData::DataHarjatan)))
                    {
                        if (Player* l_Player = SelectRandomPlayer(l_Harjatan))
                        {
                            DoCast(l_Player, eSpells::DrivenAssault);
                            DoCast(eSpells::DrivenAssaultAura);
                            m_Events.ScheduleEvent(mEvents::EventSplashyCleave, 1 * IN_MILLISECONDS);
                            m_Events.ScheduleEvent(mEvents::EventDrivenAssault, 1 * IN_MILLISECONDS);
                        }
                    }
                }
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                    p_instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);
                while (uint32 eventId = m_Events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventSplashyCleave:
                        {
                            if (me->getVictim() && me->IsWithinMeleeRange(me->getVictim()))
                            {
                                DoCast(me->getVictim(), eSpells::SplashyCleave);
                                m_Events.ScheduleEvent(mEvents::EventSplashyCleave, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            }
                            else
                                m_Events.ScheduleEvent(mEvents::EventSplashyCleave, 1 * IN_MILLISECONDS);
                            break;
                        }
                        case mEvents::EventDrivenAssault:
                        {
                            if (me->getVictim() && !me->getVictim()->HasAura(eSpells::DrivenAssault, me->GetGUID()))
                            {
                                if (InstanceScript* p_instance = me->GetInstanceScript())
                                {
                                    if (Unit* l_Harjatan = ObjectAccessor::GetUnit(*me, p_instance->GetData64(eData::DataHarjatan)))
                                    {
                                        if (Player* l_Player = SelectRandomPlayer(l_Harjatan))
                                        {
                                            DoCast(l_Player, eSpells::DrivenAssault);
                                            DoCast(eSpells::DrivenAssaultAura);
                                        }
                                    }
                                }
                            }
                            m_Events.ScheduleEvent(mEvents::EventDrivenAssault, 1 * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harjatan_razorjaw_gladiatorAI(p_Creature);
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by AqueousBurst - 231729
class spell_harjatan_aqueous_burst: public SpellScriptLoader
{
    public:
        spell_harjatan_aqueous_burst() : SpellScriptLoader("spell_harjatan_aqueous_burst") { }

        class spell_harjatan_aqueous_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_harjatan_aqueous_burst_AuraScript);

            void HandleDispel(DispelInfo* dispelInfo)
            {
                Unit* l_Target = GetUnitOwner();
                if (!l_Target)
                    return;

                if (InstanceScript* p_instance = l_Target->GetInstanceScript())
                    if (Unit* l_Harjatan = ObjectAccessor::GetUnit(*l_Target, p_instance->GetData64(eData::DataHarjatan)))
                        l_Harjatan->CastSpell(l_Target, eSpells::AqueousBurstAreaTrigger, true);
            }

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Target = GetUnitOwner();
                if (!l_Target)
                    return;

                if (InstanceScript* p_instance = l_Target->GetInstanceScript())
                    if (Unit* l_Harjatan = ObjectAccessor::GetUnit(*l_Target, p_instance->GetData64(eData::DataHarjatan)))
                        l_Harjatan->CastSpell(l_Target, eSpells::AqueousBurstAreaTrigger, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_harjatan_aqueous_burst_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterDispel += AuraDispelFn(spell_harjatan_aqueous_burst_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_harjatan_aqueous_burst_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Frigid Blows - 233548
class spell_harjatan_frigid_blows_triggered : public SpellScriptLoader
{
    public:
        spell_harjatan_frigid_blows_triggered() : SpellScriptLoader("spell_harjatan_frigid_blows_triggered") { }

        class spell_harjatan_frigid_blows_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_harjatan_frigid_blows_triggered_SpellScript);

            void SelectTarget(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* I_Victim = l_Caster->getVictim();
                if (!I_Victim)
                    return;

                if (p_Targets.empty())
                    return;

                p_Targets.remove(I_Victim);
                p_Targets.remove_if(CheckDeadTarget());
                if (p_Targets.empty())
                    return;

                auto l_RandomItr = p_Targets.begin();
                std::advance(l_RandomItr, urand(0, uint32(p_Targets.size() - 1)));
                WorldObject* l_Target = *l_RandomItr;
                p_Targets.clear();
                p_Targets.push_back(l_Target);
            }

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::DrenchingSlough, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_harjatan_frigid_blows_triggered_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_harjatan_frigid_blows_triggered_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_harjatan_frigid_blows_triggered_SpellScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Called by Frigid Blows - 233429
class spell_harjatan_frigid_blows: public SpellScriptLoader
{
    public:
        spell_harjatan_frigid_blows() : SpellScriptLoader("spell_harjatan_frigid_blows") { }

        class spell_harjatan_frigid_blows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_harjatan_frigid_blows_AuraScript);

            void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell((Unit*)nullptr, eSpells::FrostyDischarge, false);
                l_Caster->RemoveAurasDueToSpell(eSpells::OverrideEnergyBarColor);

                if (Creature* I_Harjatan = l_Caster->ToCreature())
                    if (I_Harjatan->AI())
                        I_Harjatan->AI()->DoAction(eAction::ActionAfterCastFrostyDischarge);
            }

            void HandleOnProc(ProcEventInfo& /*p_ProcEventInfo*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                if (l_Aura->GetStackAmount() >= 2)
                    l_Aura->SetStackAmount(l_Aura->GetStackAmount() - 1);
                else
                    l_Caster->RemoveAuraFromStack(l_Aura->GetId());
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_harjatan_frigid_blows_AuraScript::HandleOnProc);
                AfterEffectRemove += AuraEffectRemoveFn(spell_harjatan_frigid_blows_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_harjatan_frigid_blows_AuraScript();
        }
};

/// Last Update 7.3.2 - 25549
/// Called by Frosty Discharge - 232174
class spell_harjatan_frosty_discharge : public SpellScriptLoader
{
    public:
        spell_harjatan_frosty_discharge() : SpellScriptLoader("spell_harjatan_frosty_discharge") { }

        class spell_harjatan_frosty_discharge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_harjatan_frosty_discharge_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* I_Caster = GetCaster())
                    I_Caster->CastSpell((Unit*)nullptr, eSpells::CancelDrenchedAura, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_harjatan_frosty_discharge_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_harjatan_frosty_discharge_SpellScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Called by Drenching Waters - 231768
class spell_harjatan_drenching_waters : public SpellScriptLoader
{
    public:
        spell_harjatan_drenching_waters() : SpellScriptLoader("spell_harjatan_drenching_waters") { }

        class spell_harjatan_drenching_waters_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_harjatan_drenching_waters_AuraScript);

            void OnTick(const AuraEffect* /*p_AurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->CastSpell(l_Target, eSpells::DrenchedAura, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_harjatan_drenching_waters_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_harjatan_drenching_waters_AuraScript();
        }
};

/// Last Update 7.3.5 - 26365
/// Called by Drenched - 241511
class spell_harjatan_drenching_drenched_damage : public SpellScriptLoader
{
    public:
        spell_harjatan_drenching_drenched_damage() : SpellScriptLoader("spell_harjatan_drenching_drenched_damage") { }

        class spell_harjatan_drenching_drenched_damaget_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_harjatan_drenching_drenched_damaget_SpellScript);

            void HandleEffectHitTarget(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (!l_Target)
                    return;

                if (AuraEffect* l_AuraEffect = l_Target->GetAuraEffect(eSpells::DrenchedAura, EFFECT_0))
                {
                    int32 l_Damage = GetHitDamage();
                    AddPct(l_Damage, l_AuraEffect->GetAmount());
                    SetHitDamage(l_Damage);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_harjatan_drenching_drenched_damaget_SpellScript::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_harjatan_drenching_drenched_damaget_SpellScript;
        }
};

/// NPC 120557 - Mythic Egg Controller
class npc_harjatan_mythic_controller_egg : public CreatureScript
{
    public:
        npc_harjatan_mythic_controller_egg() : CreatureScript("npc_harjatan_mythic_controller_egg") { }

        struct npc_harjatan_mythic_controller_eggAI : public ScriptedAI
        {
            npc_harjatan_mythic_controller_eggAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void DoAction(int32 const p_Action) override
            {
                if (!IsMythic())
                    return;

                uint32 I_AuraId = 0;
                switch (p_Action)
                {
                    case eColorIncubatedEggTactics::IncubatedEggTacticRed:
                        I_AuraId = eSpells::HardenedShellVisualRed;
                        break;
                    case eColorIncubatedEggTactics::IncubatedEggTacticBlue:
                        I_AuraId = eSpells::HardenedShellVisualBlue;
                        break;
                    case eColorIncubatedEggTactics::IncubatedEggTacticGreen:
                        I_AuraId = eSpells::HardenedShellVisualGreen;
                        break;
                    default:
                        break;
                }

                if (!I_AuraId)
                    return;

                std::list<Creature*> l_Creatures;
                me->GetCreatureListWithEntryInGrid(l_Creatures, eCreatures::NpcIncubatedEgg, 20.f);
                for (Creature* l_Eggs : l_Creatures)
                    l_Eggs->AddAura(I_AuraId, l_Eggs);

                if (I_AuraId != eSpells::HardenedShellVisualGreen)
                    JadeCore::RandomResizeList(l_Creatures, 1);

                for (Creature* l_Egg : l_Creatures)
                {
                    if (I_AuraId == eSpells::HardenedShellVisualGreen) ///< For Green MaxHealth / 2
                    {
                        uint64 I_Health = l_Egg->GetMaxHealth() / 2;
                        l_Egg->SetMaxHealth(I_Health);
                        l_Egg->SetHealth(I_Health);
                    }
                    l_Egg->RemoveAura(eSpells::HardenedShell);
                    l_Egg->AddAura(eSpells::HardenedShell, l_Egg);
                    l_Egg->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                    l_Egg->CastSpell(l_Egg, eSpells::Hatching, false);
                    DoZoneInCombat(l_Egg, 250.0f);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harjatan_mythic_controller_eggAI(p_Creature);
        }
};

/// Last Update 7.3.5 - 26365
/// Called by Hatching - 240319
class spell_harjatan_hatching : public SpellScriptLoader
{
    public:
        spell_harjatan_hatching() : SpellScriptLoader("spell_harjatan_hatching") { }

        class spell_harjatan_hatching_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_harjatan_hatching_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* I_Caster = GetCaster())
                {
                   if (Creature* I_Egg = I_Caster->ToCreature())
                   {
                      if (I_Egg->AI())
                          I_Egg->AI()->DoAction(0);
                   }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_harjatan_hatching_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_harjatan_hatching_SpellScript();
        }
};

/// NPC 120545 - Incubated Egg
class npc_harjatan_incubated_egg : public CreatureScript
{
    public:
        npc_harjatan_incubated_egg() : CreatureScript("npc_harjatan_incubated_egg") { }

        struct npc_harjatan_incubated_eggAI : public ScriptedAI
        {
            npc_harjatan_incubated_eggAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
            }

            void DoAction(int32 const p_Action) override
            {
                if (!IsMythic())
                    return;

                uint32 l_SpellId = 0;
                if (me->HasAura(eSpells::HardenedShellVisualRed))
                    l_SpellId = eSpells::HatchingSummonRed;
                else if (me->HasAura(eSpells::HardenedShellVisualBlue))
                    l_SpellId = eSpells::HatchingSummonBlue;
                else if (me->HasAura(eSpells::HardenedShellVisualGreen))
                    l_SpellId = eSpells::HatchingSummonGreen;

                if (!l_SpellId)
                    return;

                me->CastSpell(me, l_SpellId, true);
                me->Kill(me);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harjatan_incubated_eggAI(p_Creature);
        }
};

/// NPC 121156 - Colicky Tadpole
class npc_harjatan_colicky_tadpole : public CreatureScript
{
    public:
        npc_harjatan_colicky_tadpole() : CreatureScript("npc_harjatan_colicky_tadpole") { }

        struct npc_harjatan_colicky_tadpoleAI : public ScriptedAI
        {
            npc_harjatan_colicky_tadpoleAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum mEvents
            {
                EventTantrum = 1,
                EventGettingAngry
            };

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 250.0f);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.ScheduleEvent(mEvents::EventGettingAngry, 5 * IN_MILLISECONDS);
                m_Events.ScheduleEvent(mEvents::EventTantrum, 500);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(eSpells::EnragedAfterKill, true);
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
                        case mEvents::EventGettingAngry:
                        {
                            DoCast(me, eSpells::GettingAngry);
                            m_Events.ScheduleEvent(mEvents::EventGettingAngry, urand(10 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            break;
                        }
                        case mEvents::EventTantrum:
                        {
                            DoCast(me, eSpells::Tantrum);
                            m_Events.ScheduleEvent(mEvents::EventTantrum, urand(15 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harjatan_colicky_tadpoleAI(p_Creature);
        }
};

/// NPC 121155 - Sickyly Tadpole
class npc_harjatan_sickly_tadpole : public CreatureScript
{
    public:
        npc_harjatan_sickly_tadpole() : CreatureScript("npc_harjatan_sickly_tadpole") { }

        struct npc_harjatan_sickly_tadpoleAI : public ScriptedAI
        {
            npc_harjatan_sickly_tadpoleAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum mEvents
            {
                EventCheckFixate = 1
            };

            void Reset() override
            {
                events.Reset();
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetSpeed(MOVE_WALK, 0.5f);
                me->SetSpeed(MOVE_RUN, 0.5f);
                me->SetSpeed(MOVE_RUN_BACK, 0.5f);
                me->SetWalk(true);
                DoZoneInCombat(me, 250.0f);
            }

            Player* SelectRandomPlayer(Creature* l_Harjatan, Unit* I_PrevTarget, float l_Radius)
            {
                std::list<Player*> temp;
                me->GetPlayerListInGrid(temp, l_Radius);

                if (I_PrevTarget)
                    if (Player* I_PrevPlayer = I_PrevTarget->ToPlayer())
                        temp.remove(I_PrevPlayer);

                if (Unit* I_Victim = l_Harjatan->getVictim())
                    if (Player* I_Player = I_Victim->ToPlayer())
                        temp.remove(I_Player);

                temp.remove_if(CheckDeadTarget());
                temp.remove_if([](Player* p_Player) -> bool
                {
                    if (!p_Player || p_Player->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (temp.empty())
                    return nullptr;

                temp.sort(JadeCore::DistanceCompareOrderPred(me, false)); ///< first check of top range players
                if (!temp.empty())
                    if (Player* l_Player = temp.front())
                        return l_Player;

                return nullptr;
            }

            void AddedTargetFixate(Unit* I_PrevTarget)
            {
                if (InstanceScript* p_instance = me->GetInstanceScript())
                {
                    if (Creature* l_Harjatan = Unit::GetCreature(*me, p_instance->GetData64(eData::DataHarjatan)))
                    {
                        if (Player* l_Player = SelectRandomPlayer(l_Harjatan, I_PrevTarget, 70.f))
                        {
                            me->RemoveAurasDueToSpell(eSpells::RottenEmissionDeathAura);
                            DoCast(l_Player, eSpells::SicklyFixate);
                        }
                    }
                }
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                AddedTargetFixate(nullptr);
                events.ScheduleEvent(mEvents::EventCheckFixate, 1 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(eSpells::EnragedAfterKill, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case mEvents::EventCheckFixate:
                        {
                            if (Unit* l_Target = me->getVictim())
                            {
                                if (!l_Target->HasAura(eSpells::SicklyFixate, me->GetGUID()))
                                    AddedTargetFixate(l_Target);

                                if (l_Target->HasAura(eSpells::SicklyFixate, me->GetGUID()) && !me->HasAura(eSpells::RottenEmissionDeathAura) && me->GetDistance(l_Target) <= 5.f)
                                {
                                    me->AddAura(eSpells::RottenEmissionDeathAura, me);
                                    me->CastSpell(me, eSpells::RottenEmission, true);
                                    l_Target->RemoveAurasDueToSpell(eSpells::SicklyFixate);
                                    me->CastStop();
                                    AddedTargetFixate(l_Target);
                                }
                            }

                            events.ScheduleEvent(mEvents::EventCheckFixate, 1 * IN_MILLISECONDS);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harjatan_sickly_tadpoleAI(p_Creature);
        }
};

/// NPC 120574 - Drippy Tadpole
class npc_harjatan_drippy_tadpole : public CreatureScript
{
    public:
        npc_harjatan_drippy_tadpole() : CreatureScript("npc_harjatan_drippy_tadpole") { }

        struct npc_harjatan_drippy_tadpoleAI : public ScriptedAI
        {
            npc_harjatan_drippy_tadpoleAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoZoneInCombat(me, 250.0f);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                DoCast(me, eSpells::DrippingWet);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                DoCast(eSpells::EnragedAfterKill, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_harjatan_drippy_tadpoleAI(p_Creature);
        }
};

/// Last Update: 7.3.5 26365
/// Called by Tantrum - 241590
class spell_harjatan_tantum : public SpellScriptLoader
{
    public:
        spell_harjatan_tantum() : SpellScriptLoader("spell_harjatan_tantum") { }

        class spell_harjatan_tantum_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_harjatan_tantum_AuraScript);

            std::list<uint64> m_Targets;
            void OnTick(const AuraEffect* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                InstanceScript* p_instance = l_Caster->GetInstanceScript();
                if (!p_instance)
                    return;

                std::list<Player*> temp;
                l_Caster->GetPlayerListInGrid(temp, 100.f);
                if (temp.empty())
                    return;

                if (Creature* l_Harjatan = Unit::GetCreature(*l_Caster, p_instance->GetData64(eData::DataHarjatan)))
                    if (Unit* I_Victim = l_Harjatan->getVictim())
                        if (Player* I_Player = I_Victim->ToPlayer())
                            temp.remove(I_Player);

                temp.remove_if(CheckDeadTarget());
                temp.remove_if([](Player* p_Player) -> bool
                {
                    if (!p_Player || p_Player->IsActiveSpecTankSpec())
                        return true;

                    return false;
                });

                if (!m_Targets.empty())
                {
                    for (uint64 l_TargetGuid : m_Targets)
                    {
                        Player* l_Player = ObjectAccessor::GetPlayer(*l_Caster, l_TargetGuid);
                        if (!l_Player)
                            continue;

                        temp.remove(l_Player);
                    }
                }

                if (temp.empty())
                    return;

                temp.sort(JadeCore::DistanceCompareOrderPred(l_Caster));
                Player* l_Target = (!temp.empty() ? temp.front() : nullptr);
                if (!l_Target)
                    return;

                m_Targets.push_back(l_Target->GetGUID());
                l_Caster->GetMotionMaster()->MoveJump(l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ(), 30.f, 15.f, 10.f, eSpells::Tantrum, eSpells::TantrumJumpDamage);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_harjatan_tantum_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_harjatan_tantum_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_harjatan()
{
    ///< Boss >>
    new boss_harjatan();
    new spell_harjatan_commanding_roar();
    new spell_harjatan_unchecked_rage();
    new spell_harjatan_drawn_in();
    new spell_at_harjatan_drenching_waters();
    new spell_harjatan_frigid_blows_triggered();
    new spell_harjatan_frigid_blows();
    new spell_harjatan_frosty_discharge();
    new spell_harjatan_drenching_waters();
    new spell_harjatan_drenching_drenched_damage();
    ///< Npcs Razorjaws >>
    new npc_harjatan_razorjaw_wavemender();
    new npc_harjatan_razorjaw_gladiator();
    new spell_harjatan_aqueous_burst();
    ///< Mythic
    new npc_harjatan_mythic_controller_egg();
    new spell_harjatan_hatching();
    new npc_harjatan_incubated_egg();
    new npc_harjatan_colicky_tadpole();
    new npc_harjatan_sickly_tadpole();
    new npc_harjatan_drippy_tadpole();
    new spell_harjatan_tantum();
}
#endif
