////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "the_arcway.hpp"
#include "HelperDefines.h"

enum eTalks
{
    Aggro       = 2,
    Death       = 3
};

enum eSpells
{
    SpawnVisual                 = 202679, ///< Hit npc 98734
    ArcaneBlast                 = 196357,
    VolatileMagic               = 196562,
    VolatileMagicDmg            = 196570,
    OverchargeMana              = 196392,
    Overcharge                  = 196396,
    NetherLink                  = 196804,
    NetherLinkAT                = 196806,
    WitheringConsumption        = 196549,
    ConsumeEssence              = 196877,
    ChargedBolt                 = 220581,
    ChargedBoltAreatrigger      = 220569,
    TeleportVisual              = 41236
};

enum eEvents
{
    EventVolatileMagic          = 1,
    EventOverchargeMana         = 2,
    EventNetherLink             = 3,
    EventWitheringConsumption   = 4,
    EventConsumeEssence         = 5,

    DataOvercharge
};

enum eActions
{
    ActionCastOverchargeMana    = 0,
    ActionReactAggressive       = 1,
    ActionActivateVandros       = 2,
    ActionCastNetherAT          = 3
};

enum eAchievements
{
    ArcanicCling    = 10773
};

/// Ivanyr - 98203
class boss_ivanyr : public CreatureScript
{
    public:
        boss_ivanyr() : CreatureScript("boss_ivanyr") { }

        struct boss_ivanyrAI : public BossAI
        {
            boss_ivanyrAI(Creature* p_Creature) : BossAI(p_Creature, DATA_IVANYR)
            {
                DoCast(me, eSpells::SpawnVisual, true);
                m_IntroDone = false;
                m_EncounterStarted = false;
            }

            bool m_IntroDone;
            bool m_EncounterStarted;
            uint8 m_OverChargeCount;
            std::set<uint32> m_ChargedBoltHits;
            std::vector<uint64> m_TargetsGuids;

            void Reset() override
            {
                _Reset();
                m_OverChargeCount = 0;
                m_EncounterStarted = false;
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAurasDueToSpell(eSpells::WitheringConsumption);
                me->RemoveAurasDueToSpell(eSpells::Overcharge);
                me->SummonCreature(NPC_NIGHT_CRYSTAL, 3137.53f, 5135.01f, 623.28f, 5.2f);
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /*= 0*/) override
            {
                switch (p_ID)
                {
                    case eActions::ActionCastNetherAT:
                    {
                        if (m_TargetsGuids.empty() || p_Guid != m_TargetsGuids[0])
                            break;

                        me->m_SpellHelper.GetUint64Set()[eSpellHelpers::NetherLinks].clear();
                        for (uint64 l_GUID : m_TargetsGuids)
                            me->m_SpellHelper.GetUint64Set()[eSpellHelpers::NetherLinks].insert(l_GUID);

                        Player* l_Player = sObjectAccessor->GetPlayer(*me, m_TargetsGuids.front());
                        if (!l_Player)
                            break;

                        Position l_Dest = *l_Player;
                        l_Dest.m_orientation = 0.0;

                        me->CastSpell(l_Dest, eSpells::NetherLinkAT, true);

                        m_TargetsGuids.clear();
                        break;
                    }
                    default:
                        m_TargetsGuids.push_back(p_Guid);
                        break;
                }
            }

            void OnTaunt(Unit* p_Taunter) override
            {
                if (!p_Taunter->IsPlayer())
                    return;

                EnterCombat(p_Taunter);
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                if (m_EncounterStarted)
                    return;

                m_EncounterStarted = true;
                Talk(eTalks::Aggro);
                _EnterCombat();

                events.ScheduleEvent(eEvents::EventVolatileMagic, 8 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eEvents::EventOverchargeMana, 30 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Talk(eTalks::Death);

                if (m_ChargedBoltHits.empty())
                {
                    AchievementEntry const* l_AchievementEntry = sAchievementStore.LookupEntry(eAchievements::ArcanicCling);
                    if (!l_AchievementEntry)
                        return;

                    Map::PlayerList const& l_Players = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator l_Iter = l_Players.begin(); l_Iter != l_Players.end(); ++l_Iter)
                    {
                        if (Player* l_Player = l_Iter->getSource())
                        {
                            l_Player->CompletedAchievement(l_AchievementEntry);
                        }
                    }
                }

                _JustDied();
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (!p_Who->IsPlayer())
                    return;

                if (!m_IntroDone && me->IsWithinDistInMap(p_Who, 130.0f))
                {
                    m_IntroDone = true;
                    Talk(0);
                }
            }

            void JustReachedHome() override
            {
                if (!me->isInCombat())
                    DoCast(me, eSpells::SpawnVisual, true);
            }

            void OnSpellInterrupted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id != eSpells::OverchargeMana)
                    return;

                if (p_SpellInfo->Id == eSpells::OverchargeMana)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoZoneInCombat();
                }
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionCastOverchargeMana:
                    {
                        m_OverChargeCount++;
                        DoCast(me, eSpells::OverchargeMana, true);
                        uint64 l_GUID = me->GetGUID();
                        AddTimedDelayedOperation(10 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            DoZoneInCombat();
                        });
                        break;
                    }
                    case eActions::ActionReactAggressive:
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 p_Type) override
            {
                switch (p_Type)
                {
                    case eEvents::DataOvercharge:
                        return m_OverChargeCount;
                }

                return 0;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case DATA_ACH_ARCANIC_CLING:
                        if (m_ChargedBoltHits.find(p_Value) != m_ChargedBoltHits.end())
                            break;

                        m_ChargedBoltHits.insert(p_Value);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 60.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case eEvents::EventVolatileMagic:
                    {
                        me->CastSpell(me, eSpells::VolatileMagic);

                        events.ScheduleEvent(eEvents::EventNetherLink, 10 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eEvents::EventVolatileMagic, 32 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventOverchargeMana:
                    {
                        DoStopAttack();
                        me->SetReactState(REACT_PASSIVE);
                        DoCast(eSpells::TeleportVisual);
                        me->NearTeleportTo(me->GetHomePosition());

                        AddTimedDelayedOperation(500, [this]() -> void
                        {
                            DoAction(eActions::ActionCastOverchargeMana);
                        });

                        events.ScheduleEvent(eEvents::EventOverchargeMana, 50000);
                        break;
                    }
                    case eEvents::EventNetherLink:
                    {
                        m_TargetsGuids.clear();
                        DoCast(eSpells::NetherLink);
                        break;
                    }
                    case eEvents::EventConsumeEssence:
                    {
                        DoCast(eSpells::ConsumeEssence);
                        events.ScheduleEvent(eEvents::EventConsumeEssence, 16000);
                        break;
                    }
                    default:
                        break;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HasAura(eSpells::WitheringConsumption))
                    DoMeleeAttackIfReady();
                else
                    DoSpellAttackIfReady(eSpells::ArcaneBlast);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_ivanyrAI(p_Creature);
        }
};

/// Overcharge Mana - 196392
class spell_ivanyr_overcharge_mana : public SpellScriptLoader
{
    public:
        spell_ivanyr_overcharge_mana() : SpellScriptLoader("spell_ivanyr_overcharge_mana") { }

        class spell_ivanyr_overcharge_mana_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ivanyr_overcharge_mana_AuraScript);

            uint16 m_TickTimer = 1000;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                l_Target->SendSpellCreateVisual(l_SpellInfo, nullptr, l_Caster);

                if (m_TickTimer <= 0)
                {
                    l_Caster->CastSpell(l_Caster, eSpells::Overcharge, true);
                    m_TickTimer = 1000;
                }
                else
                    m_TickTimer -= 100;
            }

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (!l_Target->HasAura(eSpells::ChargedBolt))
                    l_Target->CastSpell(l_Target, eSpells::ChargedBolt, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ivanyr_overcharge_mana_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectApply += AuraEffectApplyFn(spell_ivanyr_overcharge_mana_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ivanyr_overcharge_mana_AuraScript();
        }
};

/// Charged Bolt - 220581
class spell_ivanyr_charged_bolt : public SpellScriptLoader
{
    public:
        spell_ivanyr_charged_bolt() : SpellScriptLoader("spell_ivanyr_charged_bolt") { }

        class spell_ivanyr_charged_bolt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ivanyr_charged_bolt_AuraScript);

            uint8 m_ChargeCount = 0;

            void OnTick(AuraEffect const* /*p_AurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                if (l_Target->GetAnyOwner())
                {
                    if (Unit* l_Owner = l_Target->GetAnyOwner())
                        m_ChargeCount = l_Owner->GetAI()->GetData(eEvents::DataOvercharge);
                }

                for (uint8 l_I = 0; l_I < m_ChargeCount; l_I++)
                    l_Target->CastSpell(l_Target, eSpells::ChargedBoltAreatrigger, true);
            }

            void HandleOnEffectRemove(AuraEffect const* p_AurEff, AuraEffectHandleModes p_Mode)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Creature* l_Creature = l_Caster->ToCreature();
                if (!l_Creature)
                    return;

                if (l_Creature->IsAIEnabled)
                    l_Creature->AI()->DoAction(eActions::ActionReactAggressive);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ivanyr_charged_bolt_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                OnEffectRemove += AuraEffectRemoveFn(spell_ivanyr_charged_bolt_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ivanyr_charged_bolt_AuraScript();
        }
};

/// Nether Link - 196804
class spell_ivanyr_nether_link : public SpellScriptLoader
{
    public:
        spell_ivanyr_nether_link() : SpellScriptLoader("spell_ivanyr_nether_link") { }

        class spell_ivanyr_nether_link_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ivanyr_nether_link_SpellScript);

            enum eSpells
            {
                NetherLinkChainVisual   = 196835,
                NetherLinkMarkVisual    = 196805
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                std::list<Player*> l_Playerlist;
                std::vector<Player*> l_Players;

                l_Caster->GetPlayerListInGrid(l_Playerlist, 150.0f, false);

                if (l_Playerlist.size() < 3)
                    return;

                JadeCore::RandomResizeList(l_Playerlist, 3);

                /// Ugly to handle like this, I know but there isn't any other method ^.^
                for (Player* l_Player : l_Playerlist)
                    l_Players.push_back(l_Player);

                Creature* l_Ivanyr = l_Caster->ToCreature();
                if (!l_Ivanyr || !l_Ivanyr->IsAIEnabled)
                    return;

                for (uint32 l_I = 0; l_I < l_Players.size(); ++l_I)
                {
                    Player* l_Player = l_Players[l_I];
                    Player* l_OtherPlayer = l_Players[(l_I + 1) % l_Players.size()];
                    Player* l_ThirdPlayer = l_Players[(l_I + 2) % l_Players.size()];

                    l_Ivanyr->AI()->SetGUID(l_Player->GetGUID());

                    l_Player->AddAura(eSpells::NetherLinkMarkVisual, l_Player);
                    l_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::NetherLinks].insert(l_OtherPlayer->GetGUID());
                    l_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::NetherLinks].insert(l_ThirdPlayer->GetGUID());
                    l_Player->CastSpell(l_OtherPlayer, eSpells::NetherLinkChainVisual, false);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_ivanyr_nether_link_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ivanyr_nether_link_SpellScript();
        }
};

/// last update : 7.3.5 - Build 26365
/// Called by Nether Link Tethers - 196835
class spell_ivanyr_nether_link_force : public SpellScriptLoader
{
    public:
        spell_ivanyr_nether_link_force() : SpellScriptLoader("spell_ivanyr_nether_link_force") { }

        class spell_ivanyr_nether_link_force_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ivanyr_nether_link_force_AuraScript);

            int32 m_UpdateDelay = 0;

            enum eEntry
            {
                Ivanyr  = 98203
            };

            void OnUpdate(uint32 p_Diff)
            {
                m_UpdateDelay -= p_Diff;
                if (m_UpdateDelay > 0)
                    return;
                else
                    m_UpdateDelay = 500;

                WorldObject* l_Target = GetOwner();
                if (!l_Target || !l_Target->ToUnit())
                    return;

                Player* l_TargetPlayer = l_Target->ToUnit()->ToPlayer();
                if (!l_TargetPlayer)
                    return;

                std::set<uint64>& l_LinkedTargets = l_TargetPlayer->m_SpellHelper.GetUint64Set()[eSpellHelpers::NetherLinks];
                if (l_LinkedTargets.empty())
                    return;

                l_TargetPlayer->RemoveAllMovementForces();

                for (auto l_Itr : l_LinkedTargets)
                {
                    Player* l_LinkedPlayer = Player::GetPlayer(*l_TargetPlayer, l_Itr);
                    if (!l_LinkedPlayer)
                        continue;

                    float l_Dist = std::min(l_TargetPlayer->GetExactDist2d(l_LinkedPlayer), 20.0f);
                    float l_Force = std::max(10.0f - (float(l_Dist) / 2.0f), 2.0f);

                    l_TargetPlayer->SendApplyMovementForce(l_LinkedPlayer->GetGUID(), true, *l_LinkedPlayer, - l_Force, 1);
                }
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                WorldObject* l_Target = GetOwner();
                if (!l_Caster || !l_Target || !l_Target->IsPlayer())
                    return;

                if (Player* l_TargetPlayer = l_Target->ToPlayer())
                {
                    l_TargetPlayer->m_SpellHelper.GetUint64Set()[eSpellHelpers::NetherLinks].clear();
                    l_TargetPlayer->RemoveAllMovementForces();
                }

                InstanceScript* l_Instance = l_Caster->GetInstanceScript();
                if (!l_Instance)
                    return;

                uint64 l_IvanyrGUID = l_Instance->GetData64(eData::DATA_IVANYR);
                if (l_IvanyrGUID == 0)
                    return;

                Creature* l_Ivanyr = l_Caster->GetMap()->GetCreature(l_IvanyrGUID);
                if (!l_Ivanyr->IsAIEnabled)
                    return;

                l_Ivanyr->AI()->SetGUID(l_Target->GetGUID(), eActions::ActionCastNetherAT);
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_ivanyr_nether_link_force_AuraScript::OnUpdate);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ivanyr_nether_link_force_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ivanyr_nether_link_force_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Nether Link DOT - 196824
class spell_ivanyr_nether_link_dot : public SpellScriptLoader
{
    public:
        spell_ivanyr_nether_link_dot() : SpellScriptLoader("spell_ivanyr_nether_link_dot") { }

        class spell_ivanyr_nether_link_dot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ivanyr_nether_link_dot_AuraScript);

            enum eSpells
            {
                ChallengersMight = 206150
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {

                if (Unit* l_Caster = GetCaster())
                {
                    if (AuraEffect const* l_AurEff = l_Caster->GetAuraEffect(eSpells::ChallengersMight, SpellEffIndex::EFFECT_1))
                        AddPct(p_Amount, l_AurEff->GetAmount());
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_ivanyr_nether_link_dot_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ivanyr_nether_link_dot_AuraScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Volatile Magic - 196562
class spell_ivanyr_volatile_magic : public SpellScriptLoader
{
    public:
        spell_ivanyr_volatile_magic() : SpellScriptLoader("spell_ivanyr_volatile_magic") { }

        class spell_ivanyr_volatile_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ivanyr_volatile_magic_SpellScript);

            enum eSpells
            {
                VolatileMagicAura   = 196562
            };

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                if (p_Targets.size() > 1)
                {
                    p_Targets.remove_if([](WorldObject* p_Itr) -> bool
                    {
                        return p_Itr->IsPlayer() && p_Itr->ToPlayer()->IsActiveSpecTankSpec();
                    });
                }

                if (p_Targets.size() > 3)
                    p_Targets.resize(3);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ivanyr_volatile_magic_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ivanyr_volatile_magic_SpellScript();
        }
};

class spell_ivanyr_charged_bolt_damage : public SpellScriptLoader
{
    public:
        spell_ivanyr_charged_bolt_damage() : SpellScriptLoader("spell_ivanyr_charged_bolt_damage") { }

        class spell_ivanyr_charged_bolt_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ivanyr_charged_bolt_damage_SpellScript);

            void HandleOnHit(SpellEffIndex /*p_EffIndex*/)
            {
                Player* l_Target = GetHitPlayer();
                if (l_Target == nullptr)
                    return;

                InstanceScript* l_Instance = l_Target->GetInstanceScript();
                if (l_Instance == nullptr)
                    return;

                uint64 l_IvanyrGUID = l_Instance->GetData64(DATA_IVANYR);
                if (l_IvanyrGUID == 0)
                    return;

                Creature* l_Ivanyr = ObjectAccessor::FindCreature(l_IvanyrGUID);
                if (l_Ivanyr == nullptr)
                    return;

                if (l_Ivanyr->IsAIEnabled)
                    l_Ivanyr->AI()->SetData(DATA_ACH_ARCANIC_CLING, l_Target->GetGUIDLow());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_ivanyr_charged_bolt_damage_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ivanyr_charged_bolt_damage_SpellScript();
        }
};

/// Last Update 7.3.5 - Build 26365
/// Called by Volatile Magic - 196562
class spell_ivanyr_volatile_magic_damage : public SpellScriptLoader
{
    public:
        spell_ivanyr_volatile_magic_damage() : SpellScriptLoader("spell_ivanyr_volatile_magic_damage") { }

        class spell_ivanyr_volatile_magic_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ivanyr_volatile_magic_damage_SpellScript);

            void HandleOnDmg(SpellEffIndex /**/)
            {
                if (GetHitUnit() == nullptr ||
                    GetCaster() == nullptr ||
                    GetHitDamage() <= 0)
                    return;

                int32 l_Dmg = GetHitDamage();

                InstanceScript* l_Instance = GetCaster()->GetInstanceScript();
                if (l_Instance == nullptr)
                    return;

                if (!l_Instance->IsChallenge())
                    return;

                Unit* l_Ivanyr = sObjectAccessor->FindUnit(l_Instance->GetData64(DATA_IVANYR));
                if (l_Ivanyr == nullptr)
                    return;

                if (AuraEffect* l_Might = l_Ivanyr->GetAuraEffect(206150, EFFECT_1))
                    AddPct(l_Dmg, l_Might->GetAmount());

                SetHitDamage(l_Dmg);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                p_Targets.remove_if([&](WorldObject* l_Target) -> bool
                {
                    if (l_Target->ToUnit() == nullptr)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_ivanyr_volatile_magic_damage_SpellScript::HandleOnDmg, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ivanyr_volatile_magic_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ivanyr_volatile_magic_damage_SpellScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_ivanyr()
{
    /// Boss
    new boss_ivanyr();

    /// Spells
    new spell_ivanyr_overcharge_mana();
    new spell_ivanyr_nether_link();
    new spell_ivanyr_nether_link_force();
    new spell_ivanyr_charged_bolt();
    new spell_ivanyr_nether_link_dot();
    new spell_ivanyr_volatile_magic();
    new spell_ivanyr_volatile_magic_damage();
    new spell_ivanyr_charged_bolt_damage();
}
#endif
