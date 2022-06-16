////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "ChallengeMgr.h"
#include "ScenarioMgr.h"
#include "GameTables.h"
#include "Challenge.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"

/// 105877 - Volcanic Plume
class npc_challenger_volcanic_plume : public CreatureScript
{
    public:
        npc_challenger_volcanic_plume() : CreatureScript("npc_challenger_volcanic_plume") { }

        struct npc_challenger_volcanic_plumeAI : public Scripted_NoMovementAI
        {
            npc_challenger_volcanic_plumeAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            EventMap m_Events;

            void Reset()
            {
                /// Make sure the faction isn't the summoner faction
                me->setFaction(16);

                /// Clear the summoner guid, a summoned creature can't hit his owner
                me->SetGuidValue(UNIT_FIELD_SUMMONED_BY,   0);
                me->SetGuidValue(UNIT_FIELD_DEMON_CREATOR, 0);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 0.0f);

                m_Events.Reset();
                m_Events.ScheduleEvent(EVENT_1, 250);
            }

            void UpdateAI(uint32 const p_Diff)
            {
                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 l_EventId = m_Events.ExecuteEvent())
                {
                    switch (l_EventId)
                    {
                        case EVENT_1:
                            me->CastSpell(me, ChallengeSpells::ChallengerVolcanicPlume, false);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new npc_challenger_volcanic_plumeAI(p_Creature);
        }
};

/// 206150 - Challenger's Might
class spell_challengers_might : public SpellScriptLoader
{
    public:
        spell_challengers_might() : SpellScriptLoader("spell_challengers_might") { }

        enum eSpells
        {
            SummonFelExplosive = 243110
        };

        class spell_challengers_might_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_challengers_might_AuraScript);

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                m_VolcanicTimer  = 0;
                m_ExplosiveTimer = 0;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Map* l_Map = l_Caster->GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Progress = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Progress)
                    return;

                auto const& l_Challenge = l_Progress->GetChallenge();
                if (!l_Challenge)
                    return;

                ChallengeModeHealthEntry const* l_ChallHealth = g_ChallengeModeHealthTable.LookupEntry(l_Challenge->GetChallengeLevel());
                ChallengeModeDamageEntry const* l_ChallDamage = g_ChallengeModeDamageTable.LookupEntry(l_Challenge->GetChallengeLevel());
                if (!l_ChallHealth || !l_ChallDamage)
                    return;

                float l_ModHealth = l_ChallHealth->Scalar;
                float l_ModDamage = l_ChallDamage->Scalar;

                bool l_IsDungeonBoss = false;

                InstanceScript* l_Instance = l_Challenge->GetInstanceScript();

                if (Creature* l_Creature = l_Caster->ToCreature())
                {
                    if (l_Creature->IsDungeonBoss() || (l_Instance && l_Instance->GetEncounterIDForBoss(l_Creature)))
                        l_IsDungeonBoss = true;

                    /// Custom case for Fenryr in Hall of Valor
                    /// There are two of them, one is the real boss, the other one is a "fake" one
                    if (l_Creature->GetEntry() == 95674)
                        l_IsDungeonBoss = true;
                }

                if (l_IsDungeonBoss)
                {
                    if (l_Challenge->HasAffix(Affixes::Tyrannical))
                    {
                        l_ModHealth *= 1.4f;
                        l_ModDamage *= 1.15f;
                    }
                }
                ///< 7.3.5: After checking retail vids of the same key, on the same patch, one with tyranical, one with fortified, we found out tempsummons shouldn't be affected by either
                else if (l_Challenge->HasAffix(Affixes::Fortified) && !l_Caster->ToTempSummon())
                {
                    if (l_Instance && l_Instance->IsValidTargetForAffix(l_Caster, Affixes::Fortified))
                    {
                        l_ModHealth *= 1.2f;
                        l_ModDamage *= 1.4f;
                    }
                }

                l_ModHealth = (l_ModHealth - 1.0f) * 100.0f;
                l_ModDamage = (l_ModDamage - 1.0f) * 100.0f;

                switch (p_AurEff->GetEffIndex())
                {
                    case EFFECT_0:
                        p_Amount = l_ModHealth;
                        break;
                    case EFFECT_1:
                        p_Amount = l_ModDamage;
                        break;
                    case EFFECT_2:
                        p_Amount = (l_Challenge->HasAffix(Affixes::Raging) && !l_IsDungeonBoss) ? 1 : 0;
                        break;
                    case EFFECT_3:
                        p_Amount = l_Challenge->HasAffix(Affixes::Bolstering) ? 1 : 0;
                        break;
                    case EFFECT_4:
                        p_Amount = l_Challenge->HasAffix(Affixes::Tyrannical) && l_IsDungeonBoss ? 1 : 0;
                        break;
                    case EFFECT_7:
                        p_Amount = l_Challenge->HasAffix(Affixes::Volcanic) ? 1 : 0;
                        break;
                    case EFFECT_8:
                        p_Amount = l_Challenge->HasAffix(Affixes::Necrotic) ? 1 : 0;
                        break;
                    case EFFECT_9:
                        p_Amount = l_Challenge->HasAffix(Affixes::Fortified) && !l_IsDungeonBoss ? 1 : 0;
                        break;
                    case EFFECT_10:
                        p_Amount = l_Challenge->HasAffix(Affixes::Sanguine) ? 1 : 0;
                        break;
                    case EFFECT_11:
                        p_Amount = l_Challenge->HasAffix(Affixes::Quaking) ? 1 : 0;
                        break;
                    case EFFECT_12:
                        p_Amount = l_Challenge->HasAffix(Affixes::Explosive) ? 1 : 0;
                        break;
                    case EFFECT_13:
                        p_Amount = l_Challenge->HasAffix(Affixes::Bursting) ? 1 : 0;
                        break;
                    default:
                        break;
                }
            }

            /// Volcanic
            uint32 m_VolcanicTimer;
            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!p_AurEff->GetAmount())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster || !l_Caster->isInCombat() || l_Caster->isDead() || l_Caster->GetEntry() == 105877)
                    return;

                if (l_Caster->ToCreature() && !l_Caster->ToCreature()->IsInCombatWithPlayer())
                    return;

                Map* l_Map = l_Caster->GetMap();
                if (!l_Map)
                    return;

                if (m_VolcanicTimer <= 1000)
                    m_VolcanicTimer = 0;
                else
                    m_VolcanicTimer -= 1000;

                if (roll_chance_i(95.0f))
                    return;

                std::list<Player*> l_Targets;

                Map::PlayerList const& l_Players = l_Map->GetPlayers();
                for (Map::PlayerList::const_iterator l_Itr = l_Players.begin(); l_Itr != l_Players.end(); ++l_Itr)
                {
                    if (Player* l_Player = l_Itr->getSource())
                    {
                        if (!l_Player->isDead() && !l_Player->isInCombat() && l_Caster->GetDistance(l_Player) > 15.0f)
                            l_Targets.push_back(l_Player);
                    }
                }

                if (l_Targets.empty())
                    return;

                if (!m_VolcanicTimer)
                {
                    m_VolcanicTimer = urand(5, 15) * IN_MILLISECONDS;
                    JadeCore::RandomResizeList(l_Targets, 1);

                    Player*& l_Target = l_Targets.front();

                    Creature* l_Trigger = l_Caster->SummonTrigger(l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ(), 0, 12000);

                    if (l_Trigger != nullptr)
                        l_Trigger->CastSpell(l_Trigger, ChallengeSpells::ChallengerSummonVolcanicPlume, true);
                }
            }

            void OnUpdate(uint32 const p_Diff)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Map* l_Map = l_Caster->GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Progress = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Progress)
                    return;

                auto const& l_Challenge = l_Progress->GetChallenge();

                if (!l_Challenge)
                    return;

                if (l_Challenge->HasAffix(Affixes::Raging))
                    _AddRaging(l_Challenge);

                if (l_Challenge->HasAffix(Affixes::Explosive))
                    _SpawnExplosive(p_Diff, l_Challenge);
            }

            void _AddRaging(Challenge* p_Challenge)
            {
                Unit* l_Caster = GetCaster();

                InstanceScript* l_Instance = p_Challenge->GetInstanceScript();

                if (!l_Instance)
                    return;

                if (!l_Instance->IsValidTargetForAffix(l_Caster, Affixes::Raging))
                    return;

                if (!l_Caster->isInCombat() || l_Caster->ToCreature()->IsDungeonBoss())
                    return;

                bool & l_RagingCasted = l_Caster->m_SpellHelper.GetBool(ChallengeSpells::ChallengerRaging);

                if (!l_Caster->HasAura(ChallengeSpells::ChallengerRaging) &&
                    l_Caster->HealthBelowPct(30) &&
                    l_Caster->IsHostileToPlayers() &&
                    !l_RagingCasted)
                {
                    l_RagingCasted = true;
                    l_Caster->DelayedCastSpell(l_Caster, ChallengeSpells::ChallengerRaging, true, 1);
                }
            }

            void _SpawnExplosive(uint32 p_Diff, Challenge* p_Challenge)
            {
                if (!GetCaster())
                    return;

                Unit* l_Caster = GetCaster();

                if (!l_Caster->GetMap())
                    return;

                Map::PlayerList const & l_Players = l_Caster->GetMap()->GetPlayers();

                if (l_Players.isEmpty())
                    return;

                if (l_Caster->isInCombat() && !l_Caster->isDead() &&
                    l_Caster->GetEntry() != 105877 && l_Caster->ToCreature() &&
                    l_Caster->GetCreatureType() != CREATURE_TYPE_CRITTER &&
                    !l_Caster->IsPetGuardianStuff())
                {
                    uint32 l_Pos = urand(0, l_Players.getSize() - 1);

                    auto l_Itr = l_Players.begin();

                    for (uint32 l_Idx = 0; (l_Idx < l_Pos) && (l_Itr != l_Players.end()); ++l_Idx)
                        ++l_Itr;

                    Player* l_RandomPlayer = l_Itr->getSource();

                    if (!l_RandomPlayer || !l_Caster->IsHostileTo(l_RandomPlayer) || !l_Caster->InSamePhase(l_RandomPlayer))
                        return;

                    m_ExplosiveTimer -= p_Diff;

                    if (m_ExplosiveTimer <= 0)
                    {
                        Creature* l_Creature = l_Caster->ToCreature();

                        m_ExplosiveTimer = 1000;

                        if (roll_chance_i(3) && l_Creature && l_Creature->IsInCombatWithPlayer())
                            l_Caster->CastSpell(l_Caster, eSpells::SummonFelExplosive, true);
                    }
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_3, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_4, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_7, SPELL_AURA_PERIODIC_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_8, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_9, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_10, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_11, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_12, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_might_AuraScript::CalculateAmount, EFFECT_13, SPELL_AURA_DUMMY);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_challengers_might_AuraScript::OnTick, EFFECT_7, SPELL_AURA_PERIODIC_DUMMY);
                OnAuraUpdate += AuraUpdateFn(spell_challengers_might_AuraScript::OnUpdate);
            }

            int32 m_ExplosiveTimer;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_challengers_might_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Raging Affix buff: 'Enrage' 228318
class spell_affix_raging : public SpellScriptLoader
{
    public:
        spell_affix_raging() : SpellScriptLoader("spell_affix_raging") { }

        class spell_affix_raging_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_affix_raging_SpellScript);

            SpellCastResult CheckTarget()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_BAD_TARGETS;

                Creature* l_Creature = l_Caster->ToCreature();
                if (!l_Creature || l_Creature->IsDungeonBoss())
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_affix_raging_SpellScript::CheckTarget);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_affix_raging_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Sanguine Affix Player Debuff - 226512 AND Sanguine Affix Mobs Buff - 226510
class spell_affix_sanguine_debuff : public SpellScriptLoader
{
    public:
        spell_affix_sanguine_debuff() : SpellScriptLoader("spell_affix_sanguine_debuff") { }

        class spell_affix_sanguine_debuff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_affix_sanguine_debuff_AuraScript);

            enum eSpells
            {
                SanguineIchor   = 226489
            };

            void OnUpdate(uint32 /*p_Diff*/)
            {
                WorldObject* l_Victim = GetOwner();
                if (!l_Victim)
                    return;

                Unit* l_UnitVictim = l_Victim->ToUnit();
                if (!l_UnitVictim)
                    return;

                std::list<AreaTrigger*> l_ATList;
                l_UnitVictim->GetAreaTriggerListWithSpellIDInRange(l_ATList, eSpells::SanguineIchor, 7);
                if (l_ATList.empty())
                    l_UnitVictim->RemoveAura(GetAura());
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_affix_sanguine_debuff_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_affix_sanguine_debuff_AuraScript();
        }
};


/// 206151 - Challenger's Burden
class spell_challengers_burden : public SpellScriptLoader
{
    public:
        spell_challengers_burden() : SpellScriptLoader("spell_challengers_burden") { }

        class spell_challengers_burden_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_challengers_burden_AuraScript);

            void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                Map* l_Map = l_Caster->GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Progress = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Progress)
                    return;

                auto const& l_Challenge = l_Progress->GetChallenge();
                if (!l_Challenge)
                    return;

                switch (p_AurEff->GetEffIndex())
                {
                    case EFFECT_1:
                        if (!l_Challenge->HasAffix(Affixes::Grievous))
                            p_Amount = 0;
                        break;
                    case EFFECT_2:
                        if (!l_Challenge->HasAffix(Affixes::Skittish) || !l_Player->IsActiveSpecTankSpec())
                            p_Amount = 0;
                        break;
                    default:
                        break;
                }
            }

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (!GetCaster())
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                Map* l_Map = l_Player->GetMap();
                if (!l_Map)
                    return;

                Scenario* l_Progress = sScenarioMgr->GetScenario(l_Map->GetScenarioGuid());
                if (!l_Progress)
                    return;

                auto const& l_Challenge = l_Progress->GetChallenge();
                if (!l_Challenge || !l_Challenge->HasAffix(Affixes::Grievous))
                    return;

                float l_HealthPct = GetTarget()->GetHealthPct();

                if (l_HealthPct < 90.0f && !GetTarget()->HasAura(ChallengeSpells::ChallengerGrievousWound) && GetTarget()->isInCombat())
                    GetTarget()->CastSpell(GetTarget(), ChallengeSpells::ChallengerGrievousWound, true);

                if (l_HealthPct >= 90.0f && GetTarget()->HasAura(ChallengeSpells::ChallengerGrievousWound))
                    GetTarget()->RemoveAurasDueToSpell(ChallengeSpells::ChallengerGrievousWound);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_burden_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_DUMMY);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_challengers_burden_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_THREAT);
                OnEffectPeriodic   += AuraEffectPeriodicFn(spell_challengers_burden_AuraScript::OnTick, EFFECT_3, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_challengers_burden_AuraScript();
        }
};

/// 240559 - Affix grievous wound
class spell_affix_grievous_wound : public SpellScriptLoader
{
    public:
        spell_affix_grievous_wound() : SpellScriptLoader("spell_affix_grievous_wound") { }

        class spell_affix_grievous_wound_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_affix_grievous_wound_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (GetTarget()->isInCombat())
                    ModStackAmount(1);
            }

            void OnUpdate(const uint32 p_Diff)
            {
                if (GetUnitOwner() == nullptr)
                    return;

                Unit*&& l_Owner = GetUnitOwner();

                if (l_Owner->GetHealthPct() >= 90.f)
                    Remove();
            }

            void Register()
            {
                OnAuraUpdate += AuraUpdateFn(spell_affix_grievous_wound_AuraScript::OnUpdate);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_affix_grievous_wound_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_affix_grievous_wound_AuraScript();
        }
};

/// 209859 - Bolster
class spell_challenger_bolster : public SpellScriptLoader
{
    public:
        spell_challenger_bolster() : SpellScriptLoader("spell_challenger_bolster") { }

        class spell_challenger_bolster_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_challenger_bolster_SpellScript);

            bool CanApplyBolsterOn(Creature* p_Creature) const
            {
                if (p_Creature->IsPetGuardianStuff())
                    return false;

                if (!p_Creature->GetDBTableGUIDLow())
                    return false;

                if (!p_Creature->isInCombat())
                    return false;

                /// Eggs in Dearkheart Thicket
                if (p_Creature->GetEntry() == 101072 ||
                    p_Creature->GetEntry() == 100533)
                    return false;

                return true;
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                for (std::list<WorldObject*>::iterator l_Itr = p_Targets.begin(); l_Itr != p_Targets.end();)
                {
                    if (Creature* l_Creature = (*l_Itr)->ToCreature())
                    {
                        if (!CanApplyBolsterOn(l_Creature))
                            l_Itr = p_Targets.erase(l_Itr);
                        else
                            ++l_Itr;
                    }
                    else
                        l_Itr = p_Targets.erase(l_Itr);
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challenger_bolster_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challenger_bolster_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challenger_bolster_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challenger_bolster_SpellScript::FilterTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_challenger_bolster_SpellScript();
        }
};

/// Volcanic Plume - 209862
class spell_challenge_volcanic_plume : public SpellScriptLoader
{
    public:
        spell_challenge_volcanic_plume() : SpellScriptLoader("spell_challenge_volcanic_plume") { }

        class spell_challenge_volcanic_plume_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_challenge_volcanic_plume_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                p_Targets.remove_if([l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr)
                        return true;

                    if (p_Object->GetDistance(*l_Caster) > 2.0f)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_challenge_volcanic_plume_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_challenge_volcanic_plume_SpellScript();
        }
};

/// 240443 - Burst
class spell_challengers_burst : public SpellScriptLoader
{
    public:
        spell_challengers_burst() : SpellScriptLoader("spell_challengers_burst") { }

        enum eSpells
        {
            BurstDamage = 243237
        };

        class spell_challengers_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_challengers_burst_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                SpellInfo const* l_BurstDamage = sSpellMgr->GetSpellInfo(eSpells::BurstDamage);
                if (!l_BurstDamage)
                    return;

                int32 l_DamagePct = l_BurstDamage->Effects[0].BasePoints * GetStackAmount();
                GetTarget()->CastCustomSpell(GetTarget(), eSpells::BurstDamage, &l_DamagePct, nullptr, nullptr, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_challengers_burst_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_challengers_burst_AuraScript();
        }
};

/// 120651 - Fel Explosive
class npc_fel_explosive : public CreatureScript
{
    public:
        npc_fel_explosive() : CreatureScript("npc_fel_explosive") { }

        enum eSpells
        {
            ExplosiveOrbAT = 240445,
            FelExplosion   = 240446
        };

        struct npc_fel_explosiveAI : public Scripted_NoMovementAI
        {
            npc_fel_explosiveAI(Creature* p_Creature) : Scripted_NoMovementAI(p_Creature) { }

            EventMap m_Events;

            bool IgnoreAoESpells() const override
            {
                return true;
            }

            void OnBuildDynFlags(uint32& p_DynFlags, Player* p_Target) override
            {
                // Zuraal Void Realm
                if (me->GetMapId() == 1753 && p_Target && p_Target->HasAura(244061))
                    p_DynFlags &= ~(UNIT_DYNFLAG_HIDE_MODEL | UNIT_DYNFLAG_NO_INTERACT);
            }

            void Reset() override
            {
                me->AddDelayedEvent([this]()->void
                {
                    me->CastSpell(me, eSpells::ExplosiveOrbAT, true);
                    me->CastSpell(me, eSpells::FelExplosion, false);
                }, 500);

                me->AddDelayedEvent([this]()->void
                {
                    /// Hackfix to clear the visual client side
                    if (AreaTrigger* l_AT = me->GetAreaTrigger(eSpells::ExplosiveOrbAT))
                    {
                        l_AT->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                        l_AT->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                        l_AT->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                    }

                    me->AddDelayedEvent([this]()->void
                    {
                        me->RemoveAllAreasTrigger();
                        me->ForcedDespawn(0);
                    }, 500);

                }, 6500);
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    /// Hackfix to clear the visual client side
                    if (AreaTrigger* l_AT = me->GetAreaTrigger(eSpells::ExplosiveOrbAT))
                    {
                        l_AT->SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, 0);
                        l_AT->SetUInt32Value(AREATRIGGER_FIELD_SPELL_FOR_VISUALS, 0);
                        l_AT->SetUInt32Value(AREATRIGGER_FIELD_SPELL_XSPELL_VISUAL_ID, 0);
                    }

                    me->AddDelayedEvent([this]()->void
                    {
                        me->RemoveAllAreasTrigger();
                        me->ForcedDespawn(0);
                    }, 500);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_fel_explosiveAI(p_Creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_challenge_scripts()
{
    /// Creature
    new npc_challenger_volcanic_plume();
    new npc_fel_explosive();

    /// Spells
    new spell_challengers_might();
    new spell_challengers_burden();
    new spell_challenger_bolster();
    new spell_challenge_volcanic_plume();
    new spell_challengers_burst();
    new spell_affix_grievous_wound();
    new spell_affix_sanguine_debuff();
    new spell_affix_raging();

}
#endif
