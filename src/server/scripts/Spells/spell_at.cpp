////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Containers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "HelperDefines.h"
#include "ScriptedCreature.h"

/// Fungal Growth - 164717
class spell_at_druid_fungal_growth : public AreaTriggerEntityScript
{
    public:
        spell_at_druid_fungal_growth() : AreaTriggerEntityScript("at_fungal_growth") { }

        enum eWildMushroomSpells
        {
            FungalCloud         = 81281,
            WildMushroomBalance = 88747,
        };

        std::list<uint64> m_Targets;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            auto l_SpellInfo = sSpellMgr->GetSpellInfo(eWildMushroomSpells::WildMushroomBalance);
            auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();

            if (l_AreaTriggerCaster == nullptr || l_SpellInfo == nullptr)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = (float)l_SpellInfo->Effects[EFFECT_0].BasePoints;

            p_AreaTrigger->GetAttackableUnitListInPositionRange(l_TargetList, *p_AreaTrigger, l_Radius, true);

            for (Unit* l_Target : l_TargetList)
            {
                if (l_Target == nullptr)
                    return;

                if (!l_Target->HasAura(eWildMushroomSpells::FungalCloud) && l_AreaTriggerCaster->_IsValidAttackTarget(l_Target, nullptr, nullptr, true))
                {
                    l_AreaTriggerCaster->CastSpell(l_Target, eWildMushroomSpells::FungalCloud, true);
                    m_Targets.push_back(l_Target->GetGUID());
                }
            }

            std::list<uint64> l_Targets(m_Targets);
            for (uint64 l_TargetGuid : l_Targets)
            {
                Unit* l_Target = ObjectAccessor::GetUnit(*l_AreaTriggerCaster, l_TargetGuid);

                if (l_Target == nullptr)
                    return;

                if (l_Target->HasAura(eWildMushroomSpells::FungalCloud, l_AreaTriggerCaster->GetGUID()) && l_Target->GetDistance(l_AreaTriggerCaster) <= l_Radius)
                    return;

                if (l_Target->HasAura(eWildMushroomSpells::FungalCloud, l_AreaTriggerCaster->GetGUID()))
                    l_Target->RemoveAurasDueToSpell(eWildMushroomSpells::FungalCloud, l_AreaTriggerCaster->GetGUID());

                m_Targets.remove(l_TargetGuid);
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            auto l_AreaTriggerCaster = p_AreaTrigger->GetCaster();

            if (l_AreaTriggerCaster == nullptr)
                return;

            std::list<uint64> l_Targets = m_Targets;
            for (uint64 l_TargetGuid : l_Targets)
            {
                Unit* l_Target = ObjectAccessor::GetUnit(*l_AreaTriggerCaster, l_TargetGuid);

                if (l_Target == nullptr)
                    return;

                if (l_Target->HasAura(eWildMushroomSpells::FungalCloud, l_AreaTriggerCaster->GetGUID()))
                    l_Target->RemoveAurasDueToSpell(eWildMushroomSpells::FungalCloud, l_AreaTriggerCaster->GetGUID());

                m_Targets.remove(l_TargetGuid);
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_druid_fungal_growth();
        }
};

/// last update : 7.3.5 25996
/// Ursol Vortex - 102793
class spell_at_druid_ursol_vortex : public AreaTriggerEntityScript
{
    public:
        spell_at_druid_ursol_vortex(): AreaTriggerEntityScript("at_ursol_vortex") { }

        enum eSpells
        {
            VortexJump = 118283,
            VortexSnare = 127797
        };

        std::list<uint64> m_TargetList;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            std::list<Unit*> l_NewTargetList;
            float l_Radius = 3.5f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            JadeCore::NearestAttackableUnitInPositionRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> searcher(l_Caster, l_NewTargetList, u_check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

            /// NearestAttackableUnitInObjectRangeCheck doesn't remove all incorrect targets
            /// TODO: Maybe it needs to correct the function in that class
            l_NewTargetList.remove_if([&](Unit* p_Unit) -> bool
            {
                return !l_Caster->IsValidAttackTarget(p_Unit);
            });


            for (auto l_It = m_TargetList.begin(); l_It != m_TargetList.end();)
            {
                Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster, *l_It);

                if (l_Target == nullptr)
                {
                    l_It = m_TargetList.erase(l_It);
                    continue;
                }

                if (std::find(l_NewTargetList.begin(), l_NewTargetList.end(), l_Target) == l_NewTargetList.end())
                {
                    const SpellInfo* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::VortexJump);

                    if (!l_Target->HasAura(eSpells::VortexJump, l_Target->GetGUID()) && !l_Target->IsImmunedToSpellEffect(l_SpellInfo, EFFECT_0))
                        l_Target->CastSpell(p_AreaTrigger, eSpells::VortexJump, true);
                    else
                    {
                        l_Target->RemoveAura(eSpells::VortexSnare);
                        l_It = m_TargetList.erase(l_It);
                        continue;
                    }
                }
                ++l_It;
            }

            if (!l_NewTargetList.empty())
            {
                for (auto itr : l_NewTargetList)
                {
                    if (!itr->HasAura(eSpells::VortexSnare))
                        l_Caster->CastSpell(itr, eSpells::VortexSnare, true);
                    if (std::find(m_TargetList.begin(), m_TargetList.end(), itr->GetGUID()) == m_TargetList.end())
                        m_TargetList.push_back(itr->GetGUID());
                }
            }
        }

        void OnRemove(AreaTrigger* /*p_AreaTrigger*/, uint32 /*p_Time*/)
        {
            for (auto l_It : m_TargetList)
            {
                Unit* l_Target = ObjectAccessor::FindUnit(l_It);
                if (l_Target)
                {
                    l_Target->RemoveAura(eSpells::VortexSnare);
                    l_Target->RemoveAura(eSpells::VortexJump);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_druid_ursol_vortex();
        }
};

/// last update : 6.1.2 19802
/// Solar Beam - 78675
class spell_at_druid_solar_beam : public AreaTriggerEntityScript
{
    public:
        spell_at_druid_solar_beam() : AreaTriggerEntityScript("spell_at_druid_solar_beam") { }

        std::list<uint64> m_TargetList;

        enum eSpells
        {
            solarBeamSilence = 81261
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            float l_Radius = 5.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            std::list<Unit*> l_NewTargetList;
            p_AreaTrigger->GetAttackableUnitListInPositionRange(l_NewTargetList, *p_AreaTrigger, l_Radius, true);

            for (Unit* l_Target : l_NewTargetList)
            {
                if (l_Caster->_IsValidAttackTarget(l_Target, nullptr, nullptr, true) && std::find(m_TargetList.begin(), m_TargetList.end(), l_Target->GetGUID()) == m_TargetList.end())
                {
                    m_TargetList.push_back(l_Target->GetGUID());
                    l_Caster->CastSpell(l_Target, eSpells::solarBeamSilence, true);
                }
            }

            for (auto l_It = m_TargetList.begin(); l_It != m_TargetList.end(); ++l_It)
            {
                Unit* l_Target = ObjectAccessor::FindUnit(*l_It);
                if (!l_Target || (std::find(l_NewTargetList.begin(), l_NewTargetList.end(), l_Target) == l_NewTargetList.end()))
                {
                    if (l_Target)
                        l_Target->RemoveAura(eSpells::solarBeamSilence);
                }
            }
        }

        void OnRemove(AreaTrigger* /*p_AreaTrigger*/, uint32 /*p_Time*/)
        {
            for (uint64 l_TargetGUID : m_TargetList)
            {
                Unit* l_Target = ObjectAccessor::FindUnit(l_TargetGUID);
                if (l_Target)
                    l_Target->RemoveAura(eSpells::solarBeamSilence);
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_druid_solar_beam();
        }
};

/// Last Update 7.3.5 build 26365
/// Flare - 132950
class spell_at_hun_flare : public AreaTriggerEntityScript
{
    public:
        spell_at_hun_flare() : AreaTriggerEntityScript("spell_at_flare") { }

        enum eSpells
        {
            RogueStealth    = 1784,
            Subterfuge      = 115191,
            PvPRulesAura    = 134735
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            float l_Radius = 7.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            std::list<Unit*> l_NewTargetList;
            p_AreaTrigger->GetAttackableUnitListInPositionRange(l_NewTargetList, *p_AreaTrigger, p_AreaTrigger->GetRadius(), true);

            for (Unit* l_Target : l_NewTargetList)
            {
                if (!l_Target->IsValidAttackTarget(l_Caster))
                    continue;

                l_Target->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                l_Target->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
                if (l_Target->getClass() == CLASS_ROGUE)
                {
                    l_Target->RemoveAura(eSpells::Subterfuge);
                    l_Target->RemoveAura(eSpells::RogueStealth);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_hun_flare();
        }
};

/// Binding Shot - 109248
class spell_at_hun_binding_shot : public AreaTriggerEntityScript
{
    public:
        spell_at_hun_binding_shot() : AreaTriggerEntityScript("spell_hun_binding_shot_areatrigger") { }

        enum eSpells
        {
            BindingShotLink         = 117405,
            BindingShotImmune       = 117553,
            BindingShotVisualLink   = 117614
        };

        uint32 m_LinkVisualTimer = 1000;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId())->Effects[EFFECT_1].CalcRadius(l_Caster);

                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_TargetList, *p_AreaTrigger, l_Radius, true);

                if (l_TargetList.empty())
                    return;

                l_TargetList.remove_if([this, l_Caster](Unit* p_Unit) -> bool
                                       {
                                           if (p_Unit == nullptr || !l_Caster->_IsValidAttackTarget(p_Unit, nullptr, nullptr, true))
                                               return true;

                                           if (p_Unit->HasAura(eSpells::BindingShotImmune))
                                               return true;

                                           return false;
                                       });

                for (Unit* l_Target : l_TargetList)
                {
                    if (!l_Target->HasAura(eSpells::BindingShotLink))
                        l_Caster->CastSpell(l_Target, eSpells::BindingShotLink, true);
                }

                if (m_LinkVisualTimer <= p_Time)
                {
                    m_LinkVisualTimer = 1000;

                    for (Unit* l_Target : l_TargetList)
                        l_Target->CastSpell(p_AreaTrigger->m_positionX, p_AreaTrigger->m_positionY, p_AreaTrigger->m_positionZ, eSpells::BindingShotVisualLink, true);
                }
                else
                    m_LinkVisualTimer -= p_Time;
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_hun_binding_shot();
        }
};

/// Last Update 6.2.3
/// Ice Trap - 13809
/// Ice Trap (Frost - Trap Launcher) - 82940 ///< 7.0.3 deprecated
class spell_at_hun_ice_trap : public AreaTriggerEntityScript
{
    public:
        spell_at_hun_ice_trap() : AreaTriggerEntityScript("at_ice_trap") { }

        enum eSpells
        {
            SpellIceTrapEffect  = 13810,
            SpellEntrapment     = 19387,
            SpellEntrapmentRoot = 64803
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            SpellInfo const* l_CreateSpell = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId());
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster && l_CreateSpell)
            {
                float l_Radius = MELEE_RANGE * 2;

                std::list<Unit*> l_NewTargetList;
                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_NewTargetList, *p_AreaTrigger, l_Radius, true);

                l_NewTargetList.sort(JadeCore::DistanceCompareOrderPred(p_AreaTrigger));
                for (Unit* l_Target : l_NewTargetList)
                {
                    if (l_Target != nullptr && l_Caster->_IsValidAttackTarget(l_Target, nullptr, nullptr, true) && !l_Target->isTotem() && !l_Target->isStatue() && p_AreaTrigger->IsInWorld())
                    {
                        l_Target->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                        l_Caster->CastSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::SpellIceTrapEffect, true);

                        if (l_Caster->HasAura(eSpells::SpellEntrapment)) ///< Entrapment
                            l_Caster->CastSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::SpellEntrapmentRoot, true);

                        p_AreaTrigger->Remove(0);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_hun_ice_trap();
        }
};

/// Last Update 6.2.3
/// Snake Trap - 34600
/// Snake Trap (Fire - Trap Launcher) - 82949
class spell_at_hun_snake_trap : public AreaTriggerEntityScript
{
    public:
        spell_at_hun_snake_trap() : AreaTriggerEntityScript("at_snake_trap") { }

        enum eSpells
        {
            SummonSnakes        = 57879,
            SpellEntrapment     = 19387,
            SpellEntrapmentRoot = 64803
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            SpellInfo const* l_CreateSpell = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId());
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster && l_CreateSpell)
            {
                float l_Radius = MELEE_RANGE * 2;

                std::list<Unit*> l_NewTargetList;
                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_NewTargetList, *p_AreaTrigger, l_Radius, true);

                l_NewTargetList.sort(JadeCore::DistanceCompareOrderPred(p_AreaTrigger));
                for (Unit* l_Target : l_NewTargetList)
                {
                    if (l_Target != nullptr && l_Caster->_IsValidAttackTarget(l_Target, nullptr, nullptr, true) && !l_Target->isTotem() && !l_Target->isStatue() && p_AreaTrigger->IsInWorld())
                    {
                        l_Target->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                        l_Caster->CastSpell(l_Target, eSpells::SummonSnakes, true);

                        if (l_Caster->HasAura(eSpells::SpellEntrapment)) ///< Entrapment
                            l_Caster->CastSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::SpellEntrapmentRoot, true);

                        p_AreaTrigger->Remove(0);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_hun_snake_trap();
        }
};

/// Last Update 6.2.3
/// Ice trap effect - 13810
class spell_at_hun_ice_trap_effect : public AreaTriggerEntityScript
{
    public:
        spell_at_hun_ice_trap_effect() : AreaTriggerEntityScript("at_ice_trap_effect") { }

        enum eSpells
        {
            GlyphOfBlackIce = 109263,
            BlackIceEffect = 83559,
            IceTrapEffect = 135299
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            std::list<Unit*> targetList;
            float l_Radius = 10.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            JadeCore::NearestAttackableUnitInPositionRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> searcher(l_Caster, targetList, u_check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

            for (auto itr : targetList)
            {
                if (itr != nullptr && l_Caster->_IsValidAttackTarget(itr, nullptr, nullptr, true) && !itr->isTotem() && !itr->HasAura(eSpells::IceTrapEffect) && !itr->isStatue())
                    itr->CastSpell(itr, IceTrapEffect, true);
            }

            // Glyph of Black Ice
            if (l_Caster->GetDistance(p_AreaTrigger) <= l_Radius && l_Caster->HasAura(GlyphOfBlackIce) && !l_Caster->HasAura(BlackIceEffect))
                l_Caster->CastSpell(l_Caster, BlackIceEffect, true);
            if (l_Caster->GetDistance(p_AreaTrigger) > l_Radius || !l_Caster->HasAura(GlyphOfBlackIce))
                l_Caster->RemoveAura(BlackIceEffect);
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            if (l_Caster->HasAura(BlackIceEffect))
                l_Caster->RemoveAura(BlackIceEffect);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_hun_ice_trap_effect();
        }
};

/// Item - Mage WoD PvP Frost 2P Bonus - 180723
class spell_at_mage_wod_frost_2p_bonus : public AreaTriggerEntityScript
{
    public:
        spell_at_mage_wod_frost_2p_bonus() : AreaTriggerEntityScript("at_mage_wod_frost_2p_bonus") { }

        enum eSpells
        {
            SlickIce = 180724
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            std::list<Unit*> targetList;
            float l_Radius = 20.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            JadeCore::NearestAttackableUnitInPositionRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> searcher(l_Caster, targetList, u_check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

            for (auto itr : targetList)
            {
                if (itr->GetDistance(p_AreaTrigger) <= 6.0f)
                    l_Caster->CastSpell(itr, eSpells::SlickIce, true);
                else
                    itr->RemoveAura(eSpells::SlickIce, l_Caster->GetGUID());
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            std::list<Unit*> targetList;
            float l_Radius = 10.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            JadeCore::NearestAttackableUnitInPositionRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> searcher(l_Caster, targetList, u_check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

            for (auto itr : targetList)
            {
                if (itr->HasAura(eSpells::SlickIce, l_Caster->GetGUID()))
                    itr->RemoveAura(eSpells::SlickIce, l_Caster->GetGUID());
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_mage_wod_frost_2p_bonus();
        }
};

/// Last Update 6.2.3
/// Arcane Orb - 153626
class spell_at_mage_arcane_orb : public AreaTriggerEntityScript
{
    public:
        spell_at_mage_arcane_orb() : AreaTriggerEntityScript("spell_areatrigger_arcane_orb") { }

        enum eArcaneOrbSpell
        {
            ArcaneOrbDamage = 153640
        };

        enum eDatas
        {
            DamageDelay = 1 * IN_MILLISECONDS ///< Delay between damage cast (and self-snare check)
        };

        int32 m_Delay = 700;
        Position m_LastPosition;
        std::unordered_set<uint64> m_DamagedTargets;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
        {
            m_Delay += p_Time;
            if (eDatas::DamageDelay < m_Delay)
            {
                if (p_AreaTrigger->GetPosition() == m_LastPosition)     ///< This will prevent multiple hits to the same target if it get stuck on a wall
                {
                    p_AreaTrigger->SetDuration(0);
                    return;
                }

                m_LastPosition = p_AreaTrigger->GetPosition();

                if (Unit* l_Caster = p_AreaTrigger->GetCaster())
                {
                    std::list<Unit*> l_TargetList;
                    float l_Radius = 7.0f;

                    p_AreaTrigger->GetAttackableUnitListInPositionRange(l_TargetList, *p_AreaTrigger, l_Radius, true);

                    for (Unit* l_Unit : l_TargetList)
                    {
                        if (l_Caster->_IsValidAttackTarget(l_Unit, nullptr, nullptr, true) && l_Caster->IsWithinLOSInMap(l_Unit))
                        {
                            auto l_It = m_DamagedTargets.find(l_Unit->GetGUID());

                            if (l_It == m_DamagedTargets.end())
                            {
                                m_DamagedTargets.insert(l_Unit->GetGUID());

                                l_Caster->CastSpell(l_Unit, eArcaneOrbSpell::ArcaneOrbDamage, true);
                            }
                        }
                    }
                }
                m_Delay -= eDatas::DamageDelay;
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_mage_arcane_orb();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Frozen Orb - 84714
class spell_at_mage_frozen_orb : public AreaTriggerEntityScript
{
public:
    spell_at_mage_frozen_orb() : AreaTriggerEntityScript("spell_at_mage_frozen_orb") { }

    enum eSpells
    {
        FrozenOrbDamage         = 84721,
        IceTime                 = 235227,
        FrostNova               = 235235,
        FingersOfFrost          = 112965,
        FingersOfFrostTrigger   = 44544,
        DeepShatter             = 198123
    };

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        if (m_Timer <= p_Time)
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                uint32 l_Radius = 9.0f; ///< For us, visual matches damage, but the frozen orb has particles that fck up this. so visual radius is set to less than 9 to keep it 'clean'.
                std::list<Unit*> l_TargetList;
                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_TargetList, *p_AreaTrigger, l_Radius, true);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (!p_AreaTrigger->IsInHeight(l_Unit))
                        continue;

                    if (l_Caster->_IsValidAttackTarget(l_Unit, p_AreaTrigger->GetSpellInfo(), nullptr, true))
                    {
                        auto l_UnitData = m_GuidListData.find(l_Unit->GetGUID());
                        if (l_UnitData != m_GuidListData.end())
                            l_UnitData->second++;
                        else
                            m_GuidListData[l_Unit->GetGUID()] = 1;
                        l_Caster->CastSpell(l_Unit, eSpells::FrozenOrbDamage, true);
                        if (!m_FOF && l_Caster->HasAura(eSpells::FingersOfFrost) && !l_Caster->HasAura(eSpells::DeepShatter))
                        {
                            m_FOF = true;
                            l_Caster->CastSpell(l_Caster, eSpells::FingersOfFrostTrigger, true);
                        }
                    }
                }

                for (auto l_GuidList : m_GuidListData)
                    if (l_GuidList.second == 20)
                    {
                        p_AreaTrigger->Remove(0);
                        return;
                    }
            }
            m_Timer = 0.5 * IN_MILLISECONDS;
        }
        else
            m_Timer -= p_Time;
    }

    void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
    {
        if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            if (l_Caster->HasAura(eSpells::IceTime))
                l_Caster->CastSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::FrostNova, true);
    }

private:
    int32 m_Timer = 0; ///< Delay between damage cast (and self-snare check)
    std::unordered_map<uint64, uint32> m_GuidListData; ///< uint64 - Unit Guid | uint32 - hit Max Count
    bool m_FOF = false;

    AreaTriggerEntityScript* GetAI() const
    {
        return new spell_at_mage_frozen_orb();
    }
};

/// Rune of Power - 116011
class spell_at_mage_rune_of_power : public AreaTriggerEntityScript
{
    public:
        spell_at_mage_rune_of_power() : AreaTriggerEntityScript("at_rune_of_power") { }

        enum eSpells
        {
            RuneOfPowerBuff = 116014
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster->GetDistance2d(p_AreaTrigger) < 8.0f)
                l_Caster->CastSpell(l_Caster, eSpells::RuneOfPowerBuff, true);
            else
                l_Caster->RemoveAura(eSpells::RuneOfPowerBuff);
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            if (p_AreaTrigger->GetCaster())
                p_AreaTrigger->GetCaster()->RemoveAura(eSpells::RuneOfPowerBuff);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_mage_rune_of_power();
        }
};

/// Last Update
/// Meteor - 177345
class spell_at_mage_meteor_timestamp : public AreaTriggerEntityScript
{
    public:
        spell_at_mage_meteor_timestamp() : AreaTriggerEntityScript("spell_areatrigger_meteor_timestamp") {}

        enum eSpells
        {
            MeteorDamage = 153564,
            MeteorVisualEffect = 174556
        };

        void OnCreate(AreaTrigger* p_AreaTrigger)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            l_Caster->CastSpell(p_AreaTrigger->m_positionX, p_AreaTrigger->m_positionY, p_AreaTrigger->m_positionZ, eSpells::MeteorVisualEffect, true);
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            l_Caster->CastSpell(p_AreaTrigger->m_positionX, p_AreaTrigger->m_positionY, p_AreaTrigger->m_positionZ, eSpells::MeteorDamage, true);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_mage_meteor_timestamp();
        }
};

/// Afterlife (healing sphere) - 117032
class spell_at_monk_afterlife_healing_sphere : public AreaTriggerEntityScript
{
    public:
        spell_at_monk_afterlife_healing_sphere() : AreaTriggerEntityScript("at_afterlife_healing_sphere") { }

        enum eAfterlife
        {
            SpellHealingSphere = 125355
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            SpellInfo const* l_CreateSpell = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId());
            Unit* l_AreaTriggerCaster = p_AreaTrigger->GetCaster();

            if (l_AreaTriggerCaster && l_CreateSpell)
            {
                float l_Radius = 1.0f;
                Unit* l_Target = nullptr;

                JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Checker(p_AreaTrigger, l_AreaTriggerCaster, l_Radius);
                JadeCore::UnitSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_Target, l_Checker);
                p_AreaTrigger->VisitNearbyGridObject(l_Radius, l_Searcher);
                if (!l_Target)
                    p_AreaTrigger->VisitNearbyWorldObject(l_Radius, l_Searcher);

                if (l_Target != nullptr && l_Target->GetGUID() == l_AreaTriggerCaster->GetGUID())
                {
                    l_AreaTriggerCaster->CastSpell(l_Target, eAfterlife::SpellHealingSphere, true);
                    p_AreaTrigger->Remove(0);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_monk_afterlife_healing_sphere();
        }
};

/// Gift of the Serpent (healing sphere) - 119031
class spell_at_monk_healing_sphere : public AreaTriggerEntityScript
{
    public:
        spell_at_monk_healing_sphere() : AreaTriggerEntityScript("at_healing_sphere") { }

        enum eGiftOfTheSerpent
        {
            SpellHealingSphere = 124041
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            SpellInfo const* l_CreateSpell = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId());
            Unit* l_AreaTriggerCaster = p_AreaTrigger->GetCaster();

            if (l_AreaTriggerCaster && l_CreateSpell)
            {
                float l_Radius = 1.0f;
                Unit* l_Target = nullptr;
                std::list<Unit*> l_TargetList;

                p_AreaTrigger->GetAttackableUnitListInPositionRange(l_TargetList, *p_AreaTrigger, l_Radius, true);

                l_TargetList.remove_if([this, l_AreaTriggerCaster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    if (!l_AreaTriggerCaster->IsValidAssistTarget(p_Object->ToUnit()))
                        return true;

                    return false;
                });

                if (l_TargetList.size() > 1)
                {
                    l_TargetList.sort(JadeCore::ObjectDistanceOrderPred(p_AreaTrigger));
                    l_TargetList.resize(1);
                }

                if (l_TargetList.size() == 1)
                l_Target = l_TargetList.front();

                if (l_Target != nullptr)
                {
                    l_AreaTriggerCaster->CastSpell(l_Target, eGiftOfTheSerpent::SpellHealingSphere, true);
                    p_AreaTrigger->Remove(0);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_monk_healing_sphere();
        }
};

class spell_at_monk_chi_sphere_afterlife : public AreaTriggerEntityScript
{
    public:
        spell_at_monk_chi_sphere_afterlife() : AreaTriggerEntityScript("at_chi_sphere_afterlife") { }

        enum eSpells
        {
            SpellChiSphere = 121283
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            SpellInfo const* l_CreateSpell = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId());
            Unit* l_AreaTriggerCaster = p_AreaTrigger->GetCaster();

            if (l_AreaTriggerCaster && l_CreateSpell)
            {
                float l_Radius = 1.0f;
                Unit* l_Target = nullptr;

                JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Checker(p_AreaTrigger, l_AreaTriggerCaster, l_Radius);
                JadeCore::UnitSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_Target, l_Checker);
                p_AreaTrigger->VisitNearbyGridObject(l_Radius, l_Searcher);
                if (!l_Target)
                    p_AreaTrigger->VisitNearbyWorldObject(l_Radius, l_Searcher);

                if (l_Target != nullptr && l_Target->GetGUID() == l_AreaTriggerCaster->GetGUID())
                {
                    l_AreaTriggerCaster->CastSpell(l_Target, eSpells::SpellChiSphere, true);
                    p_AreaTrigger->Remove(0);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_monk_chi_sphere_afterlife();
        }
};

// Gift of the Ox 124503 124506
class spell_at_monk_gift_of_the_ox : public AreaTriggerEntityScript
{
    public:
        spell_at_monk_gift_of_the_ox()  : AreaTriggerEntityScript("at_gift_of_the_ox") { }

        enum eSpells
        {
            GiftOfTheOxHeal     = 124507,
            GiftOfTheOxHealBig  = 214416,
            HealingSphereActive = 224863,
            Overflow            = 213180
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            GuidList* l_TargetList = p_AreaTrigger->GetAffectedPlayers();
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_TargetList->find(l_Caster->GetGUID()) == l_TargetList->end())
                return;

            if (AuraEffect* l_AuraEff = l_Caster->GetAuraEffect(eSpells::Overflow, EFFECT_0))
            {
                if (roll_chance_i(l_AuraEff->GetAmount()))
                {
                    l_Caster->CastSpell(l_Caster, eSpells::GiftOfTheOxHealBig, true);
                    p_AreaTrigger->SetDuration(0);
                    return;
                }
            }

            l_Caster->CastSpell(l_Caster, eSpells::GiftOfTheOxHeal, true);
            p_AreaTrigger->SetDuration(0);
            return;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Aura* l_Aura = l_Caster->GetAura(eSpells::HealingSphereActive);
            if (!l_Aura)
                return;

            l_Aura->DropStack();
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_monk_gift_of_the_ox();
        }
};

/// Power Word: Barrier - 62618
class spell_at_pri_power_word_barrier : public AreaTriggerEntityScript
{
    public:
        spell_at_pri_power_word_barrier() : AreaTriggerEntityScript("spell_areatrigger_power_word_barrier") { }

        enum eSpells
        {
            PowerWordBarrierAura    = 81782,
            RenewedHopePassive      = 197469,
            RenewedHopeHeal         = 197470
        };

        void OnCreate(AreaTrigger* p_AreaTrigger)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            if (!l_Caster->HasAura(eSpells::RenewedHopePassive))
                return;

            std::list<Unit*> l_FriendListInRadius;
            float l_Radius = 6.5f;

            JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_FriendListInRadius, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

            if (!l_FriendListInRadius.empty())
            {
                for (Unit* l_Unit : l_FriendListInRadius)
                {
                    if (l_Caster->IsValidAssistTarget(l_Unit))
                        l_Caster->CastSpell(l_Unit, eSpells::RenewedHopeHeal, true);
                }
            }
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_FriendListInRadius;
                float l_Radius = 6.5f;

                JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_FriendListInRadius, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                if (!l_FriendListInRadius.empty())
                {
                    for (Unit* l_Unit : l_FriendListInRadius)
                    {
                        if (l_Caster->IsValidAssistTarget(l_Unit))
                            l_Caster->CastSpell(l_Unit, eSpells::PowerWordBarrierAura, true);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_pri_power_word_barrier();
        }
};

/// Angelic Feather - 121536
class spell_at_pri_angelic_feather : public AreaTriggerEntityScript
{
    public:
        spell_at_pri_angelic_feather() : AreaTriggerEntityScript("at_angelic_feather") { }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            std::list<Unit*> l_TargetList;
            std::list<Unit*> l_PlayerTargetList;
            std::list<Unit*> l_UnitTargetList;
            float l_Radius = 1.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, l_TargetList, u_check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

            for (auto l_Target : l_TargetList)
            {
                if (l_Target->IsPlayer())
                    l_PlayerTargetList.push_back(l_Target);
            }

            l_PlayerTargetList.sort(JadeCore::DistanceOrderPred(p_AreaTrigger));

            for (auto l_Target : l_TargetList)
            {
                if (!l_Target->IsPlayer())
                    l_UnitTargetList.push_back(l_Target);
            }

            l_UnitTargetList.sort(JadeCore::DistanceOrderPred(p_AreaTrigger));

            for (auto l_Target : l_TargetList)
            {
                if (l_Target == l_Caster)
                    l_PlayerTargetList.push_front(l_Target);
            }

            l_TargetList.clear();

            for (auto l_Target : l_PlayerTargetList)
                l_TargetList.push_back(l_Target);

            for (auto l_Target : l_UnitTargetList)
                l_TargetList.push_back(l_Target);

            if (!l_TargetList.empty())
            {
                for (auto itr : l_TargetList)
                {
                    l_Caster->CastSpell(itr, 121557, true); // Angelic Feather increase speed
                    p_AreaTrigger->SetDuration(0);
                    return;
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_pri_angelic_feather();
        }
};

/// Last Update 7.1.5 Build 23420
/// Halo - 120517
class spell_at_pri_halo : public AreaTriggerEntityScript
{
    public:
        spell_at_pri_halo() : AreaTriggerEntityScript("spell_at_pri_halo") { }

        enum eSpells
        {
            HaloHeal    = 120692,
            HaloDamage  = 120696
        };

        void OnCreate(AreaTrigger* p_AreaTrigger)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            l_Caster->m_SpellHelper.GetBool(eSpells::HaloDamage) = true;
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            const float maxRange = 30.f;
            int32 maxDuration = p_AreaTrigger->GetUInt32Value(AREATRIGGER_FIELD_DURATION);
            int32 currentDuration = p_AreaTrigger->GetDuration();
            int32 lastDuration = maxDuration >= currentDuration ? maxDuration - currentDuration : 0;

            float currentDistance = (maxRange * lastDuration) / maxDuration;

            std::list<Unit*> l_Targets;

            HaloCheck l_Check(p_AreaTrigger, currentDistance, &m_ProcessedGuids);
            JadeCore::UnitListSearcher<HaloCheck> l_Searcher(p_AreaTrigger, l_Targets, l_Check);
            p_AreaTrigger->VisitNearbyObject(currentDistance, l_Searcher);

            std::for_each(l_Targets.begin(), l_Targets.end(), [&](Unit* p_Unit)
            {
                if (p_Unit->IsWithinLOSInMap(l_Caster))
                {
                    if (p_Unit->IsFriendlyTo(l_Caster) && l_Caster->IsValidAssistTarget(p_Unit))
                    {
                        l_Caster->CastSpell(p_Unit, eSpells::HaloHeal, true);
                    }
                    else if (l_Caster->_IsValidAttackTarget(p_Unit, nullptr, nullptr, true))
                    {
                        l_Caster->CastSpell(p_Unit, eSpells::HaloDamage, true);
                    }
                }

                m_ProcessedGuids.insert(p_Unit->GetGUID());
            });
        }

    private:

        std::set<uint64> m_ProcessedGuids;

    private:

        class HaloCheck
        {
            public:
                HaloCheck(WorldObject const* obj, float range, std::set<uint64> const* processedGuids) :
                    i_obj(obj), i_range(range), i_processedGuids(processedGuids) {}
                bool operator()(Unit* u)
                {
                    if (i_processedGuids->find(u->GetGUID()) == i_processedGuids->end() &&
                        u->isAlive() && i_obj->IsWithinDistInMap(u, i_range))
                        return true;
                    else
                        return false;
                }
            private:
                WorldObject const* i_obj;
                float i_range;
                std::set<uint64> const* i_processedGuids;
        };


        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_pri_halo();
        }
};

/// Last Update 7.1.5 Build 23420
/// Divine Star - 110744
class spell_at_pri_divine_star : public AreaTriggerEntityScript
{
public:
    spell_at_pri_divine_star() : AreaTriggerEntityScript("at_pri_divine_star") { }

    enum eSpells
    {
        DivineStarDamage = 122128
    };

    void OnCreate(AreaTrigger* p_AreaTrigger)
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        m_Reached = false;

        l_Caster->m_SpellHelper.GetBool(eSpells::DivineStarDamage) = true;
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        if (!m_Reached && p_AreaTrigger->GetAreaTriggerInfo().moveType == AT_MOVE_TYPE_TO_TARGET)
        {
            m_Reached = true;
            l_Caster->m_SpellHelper.GetBool(eSpells::DivineStarDamage) = true;
        }
    }

private:
    bool m_Reached;

    AreaTriggerEntityScript* GetAI() const
    {
        return new spell_at_pri_divine_star();
    }
};

/// Last Update 6.2.3
/// Smoke Bomb - 76577
class spell_at_rogue_smoke_bomb : public AreaTriggerEntityScript
{
    public:
        spell_at_rogue_smoke_bomb() : AreaTriggerEntityScript("spell_areatrigger_smoke_bomb") { }

        std::list<uint64> m_TargetList;

        enum eSmokeSpells
        {
            SmokeBombAura = 88611
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            float l_Radius = 4.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            std::list<Unit*> l_NewTargetList;
            JadeCore::AnyUnitInPositionRangeCheck u_check(*p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyUnitInPositionRangeCheck> searcher(p_AreaTrigger, l_NewTargetList, u_check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

            for (Unit* l_Target : l_NewTargetList)
            {
                if (std::find(m_TargetList.begin(), m_TargetList.end(), l_Target->GetGUID()) == m_TargetList.end())
                {
                    m_TargetList.push_back(l_Target->GetGUID());
                    l_Caster->CastSpell(l_Target, eSmokeSpells::SmokeBombAura, true);
                }
            }

            for (auto l_It = m_TargetList.begin(); l_It != m_TargetList.end();)
            {
                Unit* l_Target = ObjectAccessor::FindUnit(*l_It);
                if (!l_Target || (std::find(l_NewTargetList.begin(), l_NewTargetList.end(), l_Target) == l_NewTargetList.end()))
                {
                    if (l_Target)
                        l_Target->RemoveAura(eSmokeSpells::SmokeBombAura);

                    l_It = m_TargetList.erase(l_It);
                }
                else
                    ++l_It;
            }
        }

        void OnRemove(AreaTrigger* /*p_AreaTrigger*/, uint32 /*p_Time*/)
        {
            for (uint64 l_TargetGUID : m_TargetList)
            {
                Unit* l_Target = ObjectAccessor::FindUnit(l_TargetGUID);
                if (l_Target)
                    l_Target->RemoveAura(eSmokeSpells::SmokeBombAura);
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_rogue_smoke_bomb();
        }
};

/// Last Update 7.1.5 Build 22522
/// Cut to the Chase - 197020
class spell_at_rogue_cut_to_the_chase : public AreaTriggerEntityScript
{
    public:
        spell_at_rogue_cut_to_the_chase() : AreaTriggerEntityScript("spell_areatrigger_cut_to_the_chase") { }

        std::list<uint64> m_TargetList;

        enum eSpells
        {
            CuttingToTheChase = 197023
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CuttingToTheChase);
            if (!l_Caster || !l_SpellInfo)
                return;

            GuidList* l_AffectedList = p_AreaTrigger->GetAffectedPlayers();
            float l_MaxSpeed = 0;
            int32 l_BaseValue = l_SpellInfo->Effects[0].BasePoints;
            Player* l_FastestPlayer = nullptr;
            for (std::set<uint64>::iterator l_Itr = l_AffectedList->begin(); l_Itr != l_AffectedList->end(); ++l_Itr)
            {
                Player* l_Player = sObjectAccessor->GetPlayer(*p_AreaTrigger, (*l_Itr));
                if (!l_Player)
                    continue;

                if (l_Player->IsFriendlyTo(l_Caster))
                    continue;

                if (l_Player->IsMounted())
                    continue;

                /// using 17.5f to disable Fel Rush, Roll and Chi torpedo. (they modify movespeed, not like 'Charge')
                if (l_Player->GetSpeed(UnitMoveType::MOVE_RUN) > l_MaxSpeed && l_Player->GetSpeed(UnitMoveType::MOVE_RUN) < 17.5f)
                {
                    l_MaxSpeed = l_Player->GetSpeed(UnitMoveType::MOVE_RUN);
                    l_FastestPlayer = l_Player;
                }
            }

            if (l_MaxSpeed < l_Caster->GetSpeed(UnitMoveType::MOVE_RUN))
                return;

            int32 l_Amount = (((l_FastestPlayer->GetSpeed(UnitMoveType::MOVE_RUN) * 100) / 7) - 100);

            if (Aura* l_Aura = l_FastestPlayer->GetAura(eSpells::CuttingToTheChase))
            {
                l_Caster->CastCustomSpell(l_Caster, eSpells::CuttingToTheChase, &l_BaseValue, NULL, NULL, true);
                return;
            }

            l_Amount += l_BaseValue;

            l_Caster->CastCustomSpell(l_Caster, eSpells::CuttingToTheChase, &l_Amount, NULL, NULL, true);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_rogue_cut_to_the_chase();
        }
};

/// Last Update 7.3.5 build 26365
/// Chi burst - 123986
class spell_at_monk_chi_burst : public AreaTriggerEntityScript
{
    public:
        spell_at_monk_chi_burst() : AreaTriggerEntityScript("spell_at_monk_chi_burst") { }

        enum eSpells
        {
            FierceTiger     = 103985,
            ChiBurstDamage  = 148135,
            ChiBurstHeal    = 130654
        };

        enum eEntry
        {
            ChiBurstDamageEntry = 1315,
            ChiBurstHealEntry = 1316
        };

        std::list<uint64> m_UnitGUIDList;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            Player* l_ModOwner = l_Caster->GetSpellModOwner();

            if (l_ModOwner == nullptr)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = 3.0f;

            JadeCore::AnyUnitInPositionRangeCheck l_Check(*p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyUnitInPositionRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

            float l_DmgMult = l_ModOwner->HasSpell(eSpells::FierceTiger) ? 1.2f : 1.0f;

            int32 l_Healing = sSpellMgr->GetSpellInfo(eSpells::ChiBurstHeal)->Effects[EFFECT_0].BasePoints + l_ModOwner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack) * sSpellMgr->GetSpellInfo(eSpells::ChiBurstHeal)->Effects[EFFECT_0].AttackPowerMultiplier;

            std::list<uint64> l_UnitGUIDList = m_UnitGUIDList;
            l_TargetList.remove_if([this, l_Caster, l_UnitGUIDList](Unit* p_Unit) -> bool
            {
                if (p_Unit == nullptr)
                    return true;

                if (!(std::find(l_UnitGUIDList.begin(), l_UnitGUIDList.end(), p_Unit->GetGUID()) == l_UnitGUIDList.end()))
                    return true;

                return false;
            });

            for (Unit* l_Target : l_TargetList)
            {
                if (l_Target->IsFriendlyTo(l_Caster) && l_Caster->IsValidAssistTarget(l_Target) && p_AreaTrigger->GetRealEntry() == eEntry::ChiBurstHealEntry)
                    l_Caster->CastCustomSpell(l_Target, eSpells::ChiBurstHeal, &l_Healing, NULL, NULL, true);
                else if (l_Caster->_IsValidAttackTarget(l_Target, nullptr, nullptr, true) && p_AreaTrigger->GetRealEntry() == eEntry::ChiBurstDamageEntry)
                    l_Caster->CastSpell(l_Target, eSpells::ChiBurstDamage, true);

                m_UnitGUIDList.push_back(l_Target->GetGUID());
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_monk_chi_burst();
        }
};

/// last update : 6.2.3
/// Charging Ox Wave - 119392
class spell_at_monk_charging_ox_wave : public AreaTriggerEntityScript
{
    public:
        spell_at_monk_charging_ox_wave() : AreaTriggerEntityScript("spell_at_monk_charging_ox_wave") { }

        enum eSpells
        {
            Stun = 123687
        };

        void OnSetCreatePosition(AreaTrigger* /*p_AreaTrigger*/, Unit* p_Caster, Position& p_SourcePosition, Position& p_DestinationPosition, std::vector<G3D::Vector3>& p_PathToLinearDestination)
        {
            Position l_Position;
            float l_Dist = 30.0f;

            l_Position.m_positionX = p_SourcePosition.m_positionX + (l_Dist * cos(p_Caster->GetOrientation()));
            l_Position.m_positionY = p_SourcePosition.m_positionY + (l_Dist * sin(p_Caster->GetOrientation()));
            l_Position.m_positionZ = p_SourcePosition.m_positionZ;

            p_PathToLinearDestination.push_back(l_Position.AsVector3());
            p_DestinationPosition = l_Position;
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_AreaTriggerCaster = p_AreaTrigger->GetCaster();

            if (l_AreaTriggerCaster == nullptr)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = 2.0f;

            p_AreaTrigger->GetAttackableUnitListInPositionRange(l_TargetList, *p_AreaTrigger, l_Radius, true);

            for (Unit* l_Target : l_TargetList)
            {
                if (l_Target == nullptr)
                    return;

                if (!l_Target->HasAura(eSpells::Stun) && l_AreaTriggerCaster->_IsValidAttackTarget(l_Target, nullptr, nullptr, true))
                    l_AreaTriggerCaster->CastSpell(l_Target, eSpells::Stun, true);
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_monk_charging_ox_wave();
        }
};

/// Last Update 7.0.3 - 22293
/// Sigil of Chains - 202138
class spell_at_dh_sigil_of_chains: public AreaTriggerEntityScript
{
    public:
        spell_at_dh_sigil_of_chains() : AreaTriggerEntityScript("spell_at_dh_sigil_of_chains") { }

        enum eChains
        {
            Effect          = 204834,
            VisualEffect    = 208673
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            float l_Radius = 8.0f;

            if (l_Caster == nullptr)
                return;

            Position l_Pos;
            p_AreaTrigger->GetPosition(&l_Pos);
            l_Caster->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), eChains::VisualEffect, true);
            l_Caster->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), eChains::Effect, true);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_dh_sigil_of_chains();
        }
};

/// Last Update 7.0.3 - 22293
/// Sigil of Misery - 207684
class spell_at_dh_sigil_of_misery : public AreaTriggerEntityScript
{
    public:
        spell_at_dh_sigil_of_misery() : AreaTriggerEntityScript("spell_at_dh_sigil_of_misery") { }

        enum eMisery
        {
            FearEffect      = 207685,
            VisualEffect    = 208714

        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            float l_Radius = 8.0f;

            if (l_Caster == nullptr)
                return;

            Position l_Pos;
            p_AreaTrigger->GetPosition(&l_Pos);
            l_Caster->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), eMisery::VisualEffect, true);
            l_Caster->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), eMisery::FearEffect, true);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_dh_sigil_of_misery();
        }
};

/// Last Update 7.0.3 - 22293
/// Sigil of Silence - 202137
class spell_at_dh_sigil_of_silence : public AreaTriggerEntityScript
{
    public:
        spell_at_dh_sigil_of_silence() : AreaTriggerEntityScript("spell_at_dh_sigil_of_silence") { }

        enum eSilence
        {
            SilenceEffect   = 204490,
            VisualEffect    = 208709
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            float l_Radius = 8.0f;

            if (l_Caster == nullptr)
                return;

            Position l_Pos;
            p_AreaTrigger->GetPosition(&l_Pos);
            l_Caster->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), eSilence::VisualEffect, true);
            l_Caster->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), eSilence::SilenceEffect, true);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_dh_sigil_of_silence();
        }
};

/// Last Update 7.0.3 - 22293
/// Sigil of Flame - 204596
class spell_at_dh_sigil_of_flame : public AreaTriggerEntityScript
{
    public:
        spell_at_dh_sigil_of_flame() : AreaTriggerEntityScript("spell_at_dh_sigil_of_flame") { }

        enum eFlame
        {
            FlameEffect     = 204598,
            VisualEffect    = 208710
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            float l_Radius = 8.0f;

            if (l_Caster == nullptr)
                return;

            Position l_Pos;
            p_AreaTrigger->GetPosition(&l_Pos);
            l_Caster->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), eFlame::VisualEffect, true);
            l_Caster->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), eFlame::FlameEffect, true);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_dh_sigil_of_flame();
        }
};

/// Last Update 7.1.5 - 23420
/// Fury of the Illidari - 201467
class spell_at_dh_flury_of_the_illidari : public AreaTriggerEntityScript
{
    public:
        spell_at_dh_flury_of_the_illidari() : AreaTriggerEntityScript("spell_at_flury_of_the_illidari") { }

    enum eSpells
    {
        FuryOfTheIllidariDamageRight    = 201628,
        FuryOfTheIllidariDamageLeft     = 201789,
        RageOfTheIllidari               = 201472,
        RageOfTheIllidariDamage         = 217070,
        RageOfTheIllidariDamageVisual   = 226948
    };

    bool m_Animation = false;
    uint32 m_LastTime = 0;
    uint32 m_TicksCounter = 0;

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
    {
        float l_Radius = 7.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        if (!l_Caster)
            return;

        uint32 l_Time = getMSTime();

        if (l_Time >= m_LastTime + (3000.0f / 14.0f) && m_TicksCounter < 7)
        {
            ++m_TicksCounter;
            m_LastTime = l_Time;

            l_Caster->CastSpell(p_AreaTrigger, eSpells::FuryOfTheIllidariDamageRight, true);
            l_Caster->CastSpell(p_AreaTrigger, eSpells::FuryOfTheIllidariDamageLeft, true);
        }

        if (!m_Animation && l_Caster->HasAura(eSpells::RageOfTheIllidari) && (p_AreaTrigger->GetDuration() - p_Time) <= 300)
        {
            l_Caster->CastSpell(p_AreaTrigger->GetPosition(), eSpells::RageOfTheIllidariDamageVisual, true);
            m_Animation = true;
        }
    }

    void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster || !l_Caster->HasAura(eSpells::RageOfTheIllidari))
            return;

        int32 l_Damage = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::RageOfTheIllidari);
        l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::RageOfTheIllidari) = 0;

        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::RageOfTheIllidari);
        if (!l_SpellInfo)
            return;

        l_Damage = CalculatePct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
        l_Caster->CastCustomSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::RageOfTheIllidariDamage, &l_Damage, nullptr, nullptr, true);
    }

    AreaTriggerEntityScript* GetAI() const override
    {
        return new spell_at_dh_flury_of_the_illidari();
    }
};

/// last update : 7.0.3
/// Song of Chi-Ji - 198898
class spell_at_song_of_chi_ji : public AreaTriggerEntityScript
{
    public:
        spell_at_song_of_chi_ji() : AreaTriggerEntityScript("spell_at_song_of_chi_ji") { }

        std::list<uint64> m_TargetList;

        enum eSpells
        {
            SongOfChiJiStun = 198909
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            float l_Radius = 5.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            std::list<Unit*> l_NewTargetList;
            p_AreaTrigger->GetAttackableUnitListInPositionRange(l_NewTargetList, *p_AreaTrigger, l_Radius, true);

            for (Unit* l_Target : l_NewTargetList)
            {
                if (l_Caster->_IsValidAttackTarget(l_Target, nullptr, nullptr, true) && std::find(m_TargetList.begin(), m_TargetList.end(), l_Target->GetGUID()) == m_TargetList.end())
                {
                    m_TargetList.push_back(l_Target->GetGUID());
                    l_Caster->CastSpell(l_Target, eSpells::SongOfChiJiStun, true);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_song_of_chi_ji();
        }
};

/// last update :  7.3.5 - Build 26365
/// Lunar Beam - 204066
class spell_at_druid_lunar_beam : public AreaTriggerEntityScript
{
    public:
        spell_at_druid_lunar_beam() : AreaTriggerEntityScript("spell_at_druid_lunar_beam") { }

        enum eSpells
        {
            LunarBeamEffect = 204069
        };

        uint32 m_UpdateTime = 0;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            float l_Radius = 5.0f;
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (l_Caster == nullptr)
                return;

            m_UpdateTime += p_Time;

            if (m_UpdateTime >= 1 * IN_MILLISECONDS)
            {
                l_Caster->CastSpell(p_AreaTrigger->GetPosition(), eSpells::LunarBeamEffect, true);
                m_UpdateTime -= 1 * IN_MILLISECONDS;
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_druid_lunar_beam();
        }
};

/// Last Update 7.3.5 - 25996
/// Inner Demons - 202387
class spell_at_dh_inner_demon : public AreaTriggerEntityScript
{
    public:
        spell_at_dh_inner_demon() : AreaTriggerEntityScript("spell_at_dh_inner_demon") { }

        enum eSpells
        {
            InnerDemonsDamage = 202388
        };

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            WorldLocation l_AreatriggerLocation = *p_AreaTrigger;

            l_Caster->AddDelayedEvent([l_Caster, l_AreatriggerLocation]() -> void
            {
                l_Caster->CastCustomSpell(l_AreatriggerLocation.GetPositionX(), l_AreatriggerLocation.GetPositionY(), l_AreatriggerLocation.GetPositionZ(), eSpells::InnerDemonsDamage, nullptr, nullptr, nullptr, true);
            }, 1300);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_dh_inner_demon();
        }
};

/// Last Update 7.3.5 build 26365
/// Defile - 152280 (1713)
class spell_at_dk_defile : public AreaTriggerEntityScript
{
    public:
        spell_at_dk_defile() : AreaTriggerEntityScript("spell_at_dk_defile") { }

        enum eSpells
        {
            DefileDebuff    = 156004,
            DefileDamage    = 156000,
            DefileBuff      = 218100
        };

        enum eNpcs
        {
            Defile  = 82521
        };

        enum eEvents
        {
            BasicEvent   = 1,
            ScaleEvent
        };

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            m_Events.ScheduleEvent(eEvents::BasicEvent, 1);

            p_AreaTrigger->SetDuration(12 * IN_MILLISECONDS);
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (l_Caster == nullptr)
                return;

            m_Events.Update(p_Time);
            switch (m_Events.ExecuteEvent())
            {
                case eEvents::BasicEvent:
                {
                    DealDamage(p_AreaTrigger, l_Caster);
                    break;
                }
                case eEvents::ScaleEvent:
                {
                    if (Unit* l_Defile = l_Caster->GetFirstMinionByEntry(eNpcs::Defile))
                    {
                        if (m_InternalScale > l_Defile->GetObjectScale())
                        {
                            l_Defile->SetObjectScale(l_Defile->GetObjectScale() + float(m_RadIncrement / 100.0f));
                            m_Events.ScheduleEvent(eEvents::ScaleEvent, 100);
                        }
                    }
                    break;
                }
            }
        }

        void DealDamage(AreaTrigger* p_AreaTrigger, Unit* p_Caster)
        {
            /*
                Description: Defiles the ground targeted by the Death Knight for $d. Every $t3 sec, if there are any enemies standing in the Defile,
                it deals $156000s1 Shadowfrost damage to them and grows in radius, and increases your Mastery by $218100s1, stacking up to $218100u times.
                While you remain within Defile, your $?s207311[Clawing Shadows][Scourge Strike] will hit all enemies near the target.
            */
            if (Unit* l_Defile = p_Caster->GetFirstMinionByEntry(eNpcs::Defile))
            {
                std::list<Unit*> l_Targets;
                l_Defile->GetAttackableUnitListInPositionRange(l_Targets, *p_AreaTrigger, p_AreaTrigger->GetRadius(), false);
                l_Targets.remove_if(JadeCore::UnitAttackableCheck(false, p_Caster));

                bool l_Affected = false;
                for (auto l_Itr : l_Targets)
                {
                    l_Affected = true;
                    if (l_Itr->HasAura(eSpells::DefileDebuff, p_Caster->GetGUID()))
                        p_AreaTrigger->CastSpellByOwnerFromATLocation(l_Itr, eSpells::DefileDamage);
                }

                /// increase radius
                if (l_Affected)
                {
                    p_Caster->CastSpell(p_Caster, eSpells::DefileBuff, true);
                    p_AreaTrigger->SetRadius(p_AreaTrigger->GetRadius() + m_RadIncrement);

                    m_InternalScale = l_Defile->GetObjectScale() + float(m_RadIncrement / 10.0f);
                    l_Defile->SetObjectScale(l_Defile->GetObjectScale() + float(m_RadIncrement / 100.0f));
                    m_Events.ScheduleEvent(eEvents::ScaleEvent, 100);
                }
            }

            m_Events.ScheduleEvent(eEvents::BasicEvent, 1 * IN_MILLISECONDS);
        }

        private:
            EventMap m_Events;
            float m_InternalScale;
            float m_RadIncrement = 1.0f;

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_dk_defile();
        }
};

/// Last Update 7.1.5 Build 22522
/// Wild Protector - 204358 (6237)
class spell_at_hun_wild_protector : public AreaTriggerEntityScript
{
    public:
        spell_at_hun_wild_protector() : AreaTriggerEntityScript("spell_at_hun_wild_protector") { }

        std::list<uint64> m_TargetList;

        enum eSpells
        {
            WildProtectorAura = 204190,
            WildProtectorProc = 204205
        };

        const int m_Delay = 500;
        int m_time = m_Delay;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
        {
            if ((m_time += p_Time) < m_Delay)
                return;
            m_time = 0;

            if (p_AreaTrigger->GetDuration() == 0)
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WildProtectorAura);
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (!l_SpellInfo || !l_Caster || !l_Caster->GetOwner() || !l_Caster->GetOwner()->HasAura(eSpells::WildProtectorAura))
            {
                p_AreaTrigger->SetDuration(0);
                return;
            }

            int32 l_Radius = l_SpellInfo->Effects[0].BasePoints;
            std::list<Unit*> l_TargetList;
            JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);
            std::list<uint64> l_NewTargetList;

            for (auto l_Target : l_TargetList)
            {
                Aura* l_Aura = l_Target->GetAura(eSpells::WildProtectorProc);

                l_NewTargetList.push_back(l_Target->GetGUID());
                m_TargetList.remove(l_Target->GetGUID());

                if (!l_Aura)
                    p_AreaTrigger->CastSpell(l_Target, eSpells::WildProtectorProc);
                else
                    l_Aura->SetDuration(l_Aura->GetMaxDuration());
            }

            for (auto l_TargetGUID : m_TargetList)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetGUID);

                if (l_Target)
                    l_Target->RemoveAurasDueToSpell(eSpells::WildProtectorProc);
            }

            m_TargetList = l_NewTargetList;
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();

            if (!l_Caster)
                return;

            for (auto l_TargetId : m_TargetList)
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_TargetId);

                if (l_Target)
                    l_Target->RemoveAurasDueToSpell(eSpells::WildProtectorProc);
            }
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_hun_wild_protector();
        }
};

/// Last Update 7.1.5 Build 23420
/// Into the Fray - 202603
class spell_at_war_into_the_fray : public AreaTriggerEntityScript
{
    public:
        spell_at_war_into_the_fray() : AreaTriggerEntityScript("spell_at_war_into_the_fray") { }

        enum eSpells
        {
            IntoTheFrayBuff = 202602
        };

        enum eTalents
        {
            IntoTheFrayTalent = 22398
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer > p_Time)
            {
                m_UpdateTimer -= p_Time;
                return;
            }

            m_UpdateTimer = 1 * IN_MILLISECONDS;

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();
            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            JadeCore::AnyUnfriendlyNoTotemUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyNoTotemUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

            uint32 l_TargetsCount = l_TargetList.size();
            if (l_TargetsCount > 5)
                l_TargetsCount = 5;

            Aura* l_Aura = l_Caster->GetAura(eSpells::IntoTheFrayBuff);
            if (l_TargetsCount > 0)
            {
                if (l_Aura)
                {
                    l_Aura->SetStackAmount(l_TargetsCount);
                }
                else
                {
                    if (l_Player->HasTalent(eTalents::IntoTheFrayTalent, l_Player->GetActiveTalentGroup()))
                        l_Caster->CastCustomSpell(eSpells::IntoTheFrayBuff, SpellValueMod::SPELLVALUE_AURA_STACK, l_TargetsCount, l_Caster, true);
                }
            }
            else
            {
                if (l_Aura)
                    l_Aura->Remove();
            }
        }

    private:

        uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_war_into_the_fray();
        }
};

/// Last Update 7.1.5 Build 23420
/// Volatile Ichor - 8210 (222194)
class spell_at_item_volatile_ichor : public AreaTriggerEntityScript
{
    public:
        spell_at_item_volatile_ichor() : AreaTriggerEntityScript("spell_at_volatile_ichor") { }

        enum eSpells
        {
            VolatileIchor = 222187,
            VolatileIchorDmg = 222197
        };

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            uint64& l_TargetGUID = l_Caster->m_SpellHelper.GetUint64(eSpellHelpers::VolatileIchor);

            p_AreaTrigger->SetTargetGuid(l_TargetGUID);
            l_TargetGUID = 0;
        }

        void OnDestinationReached(AreaTrigger* p_AreaTrigger) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(p_AreaTrigger->GetCastItemGuid());
            if (!l_Trinket)
                return;

            float l_Multiplier = 0.0f;
            int32 l_TrinketItemLevel = l_Trinket->GetItemLevel(l_Player);
            RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
            if (l_RandomPropertiesPoints)
                l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

            SpellInfo const* l_VolatileIchorInfo = sSpellMgr->GetSpellInfo(eSpells::VolatileIchor);
            if (!l_VolatileIchorInfo)
                return;

            float l_ScalingMultiplier = l_VolatileIchorInfo->Effects[EFFECT_0].ScalingMultiplier;
            float l_Amount = l_ScalingMultiplier * l_Multiplier;
            float l_DeltaScalingMultiplier = l_VolatileIchorInfo->Effects[EFFECT_0].DeltaScalingMultiplier;
            if (l_DeltaScalingMultiplier)
            {
                float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                l_Amount += frand(-l_Delta, l_Delta);
            }

            int32 l_Damage = (int32)l_Amount;

            l_Caster->CastCustomSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::VolatileIchorDmg, &l_Damage, nullptr, nullptr, true, l_Trinket);
            p_AreaTrigger->Remove();
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_item_volatile_ichor();
        }
};

/// Last Update 7.3.5 Build 26365
/// Nightmarish Ichor - 8167 (222023)
class spell_at_item_nightmarish_ichor : public AreaTriggerEntityScript
{
    public:
        spell_at_item_nightmarish_ichor() : AreaTriggerEntityScript("spell_at_nightmarish_ichor") { }

        enum eSpells
        {
            Spell_NightmarishIchorMainBuff = 222015,
            Spell_NightmarishIchorVersatilityBuff = 222027
        };

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
            {
                p_AreaTrigger->Remove();
                return;
            }

            p_AreaTrigger->SetTargetGuid(l_Caster->GetGUID());
        }

        bool OnAddTarget(AreaTrigger* p_Areatrigger, Unit* p_Target) override
        {
            Unit* l_Caster = p_Areatrigger->GetCaster();
            if (!l_Caster || p_Target != l_Caster)
                return false;

            AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(eSpells::Spell_NightmarishIchorMainBuff, EFFECT_1);
            if (!l_AuraEffect)
                return false;

            int32 l_Amount = l_AuraEffect->GetAmount();

            l_Caster->CastCustomSpell(l_Caster, eSpells::Spell_NightmarishIchorVersatilityBuff, &l_Amount, nullptr, nullptr, true);
            p_Areatrigger->Remove();
            return false;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_item_nightmarish_ichor();
        }
};


/// Last Update 7.1.5 Build 23420
/// Tormenting Cyclone - 221857
class spell_at_item_tormenting_cyclone : public AreaTriggerEntityScript
{
    public:
        spell_at_item_tormenting_cyclone() : AreaTriggerEntityScript("spell_at_tormenting_cyclone") { }

        enum eSpells
        {
            TormentingCyclone       = 221845,
            TormentingCycloneDmg    = 221865
        };

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_item_tormenting_cyclone();
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer > p_Time)
            {
                m_UpdateTimer -= p_Time;
                return;
            }

            m_UpdateTimer = 1 * IN_MILLISECONDS;

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(p_AreaTrigger->GetCastItemGuid());
            if (!l_Trinket)
                return;

            float l_Multiplier = 0.0f;
            int32 l_TrinketItemLevel = l_Trinket->GetItemLevel(l_Player);
            RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
            if (l_RandomPropertiesPoints)
                l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

            SpellInfo const* l_TormentingCycloneInfo = sSpellMgr->GetSpellInfo(eSpells::TormentingCyclone);
            if (!l_TormentingCycloneInfo)
                return;

            float l_ScalingMultiplier = l_TormentingCycloneInfo->Effects[EFFECT_0].ScalingMultiplier;
            float l_Amount = l_ScalingMultiplier * l_Multiplier;
            float l_DeltaScalingMultiplier = l_TormentingCycloneInfo->Effects[EFFECT_0].DeltaScalingMultiplier;
            if (l_DeltaScalingMultiplier)
            {
                float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                l_Amount += frand(-l_Delta, l_Delta);
            }

            int32 l_Damage = (int32)l_Amount;

            l_Caster->CastCustomSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::TormentingCycloneDmg, &l_Damage, nullptr, nullptr, true, l_Trinket);
        }

    private:

        uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
};


/// Last Update 7.1.5 Build 23420
/// Lesser Soul Fragment - 7371 (215393)
class spell_at_dh_lesser_soul_fragment : public AreaTriggerEntityScript
{
    public:
        spell_at_dh_lesser_soul_fragment() : AreaTriggerEntityScript("spell_at_lesser_soul_fragment") { }

        enum eSpells
        {
            ShatteredSoul1 = 204255, ///< Spawned by Shear and Fracture
            ConsumeSoul    = 208014
        };

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            if (p_AreaTrigger->GetSpellId() == eSpells::ShatteredSoul1)
            {
                std::list<AreaTrigger*> l_AreaTriggers;
                l_Caster->GetAreaTriggerList(l_AreaTriggers, eSpells::ShatteredSoul1);
                const uint8 l_MaxTriggersCount = 5;
                if (l_AreaTriggers.size() > l_MaxTriggersCount)
                {
                    l_AreaTriggers.sort(JadeCore::AreaTriggerDurationPctOrderPred());

                    while (l_AreaTriggers.size() > l_MaxTriggersCount)
                    {
                        AreaTrigger* l_AreaTrigger = l_AreaTriggers.front();

                        l_Caster->CastSpell(l_AreaTrigger->GetPositionX(), l_AreaTrigger->GetPositionY(), l_AreaTrigger->GetPositionZH(), eSpells::ConsumeSoul, true);

                        l_AreaTrigger->Remove();
                        l_AreaTriggers.pop_front();
                    }
                }
            }

            p_AreaTrigger->ClearVisibleOnlyForSomePlayers();
            p_AreaTrigger->AddPlayerInPersonnalVisibilityList(l_Caster->GetGUID());
            p_AreaTrigger->UpdateObjectVisibility();
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_dh_lesser_soul_fragment();
        }
};

/// Last Update 7.3.5 Build 26365
/// Windfury Totem - 204332
class spell_at_shaman_windfury_totem : public AreaTriggerEntityScript
{
    public:
        spell_at_shaman_windfury_totem() : AreaTriggerEntityScript("spell_at_shaman_windfury_totem") { }

        enum eSpells
        {
            WindfuryTotem       = 204332,
            WindfuryTotemBuff   = 209385,
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (p_AreaTrigger->GetCaster() == nullptr)
                return;

            Unit* l_TotemOwner = p_AreaTrigger->GetCaster()->GetOwner();
            if (!l_TotemOwner)
                return;

            Player* l_Caster = l_TotemOwner->ToPlayer();
            if (!l_Caster)
                return;

            float l_Radius = 10.0f;
            std::list<Unit*> l_FriendList;

            JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_FriendList, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

            if (!l_FriendList.empty())
                for (Unit* l_Unit : l_FriendList)
                    if (l_Caster->IsFriendlyTo(l_Unit) || l_Unit == p_AreaTrigger->GetCaster())
                        if (!l_Unit->HasAura(eSpells::WindfuryTotemBuff))
                            l_Caster->CastSpell(l_Unit, eSpells::WindfuryTotemBuff, true);
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            Unit* l_Owner = l_Caster ? l_Caster->GetOwner() : nullptr;
            Player* l_Player = nullptr;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WindfuryTotem);

            if (!l_Owner || !(l_Player = l_Owner->ToPlayer()) || !l_SpellInfo)
                return;

            l_Player->RemoveSpellCooldown(eSpells::WindfuryTotem, true);
            l_Player->AddSpellCooldown(eSpells::WindfuryTotem, 0, l_SpellInfo->RecoveryTime, true);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_shaman_windfury_totem();
        }
};

/// Rain of Fire - 5740
class spell_at_warl_rain_of_fire : public AreaTriggerEntityScript
{
    public:
        spell_at_warl_rain_of_fire() : AreaTriggerEntityScript("spell_at_warl_rain_of_fire") { }

        void OnCreate(AreaTrigger* p_AreaTrigger) override
        {
            p_AreaTrigger->m_stealthDetect.AddFlag(StealthType::STEALTH_GENERAL);
            p_AreaTrigger->m_stealthDetect.SetValue(StealthType::STEALTH_GENERAL, 1000);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_warl_rain_of_fire();
        }
};

/// Last Update 7.1.5 Build 22522
/// Consecration - 26573
class spell_at_pal_consecration : public AreaTriggerEntityScript
{
public:
    spell_at_pal_consecration() : AreaTriggerEntityScript("spell_at_consecration") { }

    enum eSpells
    {
        ConsecrationBuff = 188370,
        ConsecrationDmg = 204242,
        Heathcliffs = 207603,
        ConsecrationGround = 204054,
        ConsecrationHeal = 204241,
        ConsecrationPeriodic = 205228,
        ConcecrationDamage = 81297
    };

    void OnCreate(AreaTrigger* p_AreaTrigger) override
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        if (l_Caster)
            l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ConsecrationCount)++;

        if (p_AreaTrigger->GetSpellInfo()->AttributesEx5 & SPELL_ATTR5_HASTE_AFFECT_DURATION)
        {
            if (Player* l_Player = l_Caster->GetSpellModOwner())
            {
                int32 l_Duration = int32(p_AreaTrigger->GetDuration() * std::max<float>(l_Player->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED), 0.4f));
                p_AreaTrigger->SetDuration(l_Duration);
            }
        }
    }

    void OnRemove(AreaTrigger* p_AreaTrigger, uint32) override
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        if (!l_Caster)
            return;

        if (--l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::ConsecrationCount) == 0)
        {
            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();

            JadeCore::AnyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

            for (Unit* l_Unit : l_TargetList)
            {
                if (!l_Unit)
                    continue;

                l_Unit->RemoveAurasDueToSpell(eSpells::ConsecrationBuff, l_Caster->GetGUID());
                l_Unit->RemoveAurasDueToSpell(eSpells::ConsecrationDmg, l_Caster->GetGUID());
                l_Unit->RemoveAurasDueToSpell(eSpells::Heathcliffs, l_Caster->GetGUID());
            }

            l_Caster->RemoveAurasDueToSpell(eSpells::ConsecrationBuff, l_Caster->GetGUID());
            l_Caster->RemoveAurasDueToSpell(eSpells::ConsecrationDmg, l_Caster->GetGUID());
            l_Caster->RemoveAurasDueToSpell(eSpells::Heathcliffs, l_Caster->GetGUID());
        }
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
    {
        if (m_UpdateTimer == -1)
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(p_AreaTrigger->GetSpellId(), EFFECT_0))
                {
                    m_UpdateTimer = l_AuraEffect->GetAmplitude();
                    m_UpdateTimerFromEffect = l_AuraEffect->GetAmplitude();
                }
            }
            return;
        }

        if (m_UpdateTimer > p_Time)
        {
            m_UpdateTimer -= p_Time;
            return;
        }

        m_UpdateTimer = m_UpdateTimerFromEffect;

        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        l_Caster->CastSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::ConcecrationDamage, true);

        if (l_Caster->HasAura(eSpells::ConsecrationGround))
        {
            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();

            JadeCore::AnyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

            l_TargetList.remove_if([this, l_Caster](Unit* p_Unit) -> bool
            {
                if (p_Unit == nullptr || !p_Unit->IsPlayer() || !l_Caster->IsValidAssistTarget(p_Unit))
                    return true;

                return false;
            });

            if (l_TargetList.empty())
                return;

            l_TargetList.sort(JadeCore::HealthPctOrderPred());

            if (l_TargetList.size() > 6)
                l_TargetList.resize(6);

            l_Caster->CastSpell(l_Caster, eSpells::ConsecrationHeal, true);
        }
    }

    uint32 m_UpdateTimer = -1;
    uint32 m_UpdateTimerFromEffect = 0;

    AreaTriggerEntityScript* GetAI() const override
    {
        return new spell_at_pal_consecration();
    }
};

/// Chi Orbit - 5280
class spell_at_monk_chi_orbit : public AreaTriggerEntityScript
{
    public:
        spell_at_monk_chi_orbit() : AreaTriggerEntityScript("spell_at_monk_chi_orbit") { }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            l_Caster->RemoveAura(p_AreaTrigger->GetSpellId());
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_monk_chi_orbit();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Perfectly Preserved Cake - 140793
/// Called By Delicious Cake! - 225126
class spell_at_item_delicious_cake : public AreaTriggerEntityScript
{
    public:
        spell_at_item_delicious_cake() : AreaTriggerEntityScript("spell_at_delicious_cake")
        {
            m_TargetsCount = 0;
        }

        enum eSpells
        {
            SpellDeliciousCakeShield    = 225723,
        };

        bool OnAddTarget(AreaTrigger* p_At, Unit* p_Target) override
        {
            if (p_Target == nullptr || p_At->GetCaster() == nullptr)
                return false;

            if (!p_Target->IsPlayer())
                return false;

            if (m_TargetsCount >= 4)
                return false;

            if (p_Target->HasAura(eSpells::SpellDeliciousCakeShield))
                return false;

            Player* l_Caster = p_At->GetCaster()->ToPlayer();

            if (l_Caster == nullptr)
                return false;

            Item* l_Item = l_Caster->GetItemByGuid(p_At->GetCastItemGuid());

            m_TargetsCount++;
            l_Caster->CastSpell(p_Target, eSpells::SpellDeliciousCakeShield, true, l_Item);

            return true;
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_item_delicious_cake();
        }

    private:
        uint8 m_TargetsCount;
};

/// Last update 7.3.5 Build 26365
/// Item Ampoule of Pure Void - 151312
/// Called By Leeching Void - 250765
class spell_at_item_leeching_void : public AreaTriggerEntityScript
{
    public:

        spell_at_item_leeching_void() : AreaTriggerEntityScript("spell_at_item_leeching_void") { }

        enum eItems
        {
            AmpouleOfPureVoid = 151312
        };

        enum eSpells
        {
            PoolOfPureVoid = 250766
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
        {
            if (m_TicksCount > 0)
                DoCast(p_AreaTrigger);
        }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_Timer > p_Time)
            {
                m_Timer -= p_Time;
                return;
            }

            m_Timer = 1 * TimeConstants::IN_MILLISECONDS;

            --m_TicksCount;

            DoCast(p_AreaTrigger);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_item_leeching_void();
        }

    private:

        void DoCast(AreaTrigger* p_AreaTrigger)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (l_Caster == nullptr || !l_Caster->IsPlayer())
                return;

            Item* l_Trinket = l_Caster->ToPlayer()->GetItemByEntry(eItems::AmpouleOfPureVoid);
            if (l_Trinket == nullptr)
                return;

            l_Caster->CastSpell(*p_AreaTrigger, eSpells::PoolOfPureVoid, true, l_Trinket);
        }

        int32 m_Timer = 1 * TimeConstants::IN_MILLISECONDS;
        int32 m_TicksCount = 10;
};

/// Last update 7.3.2 Build 25549
/// Seraphim's Blessing - 204927
class spell_at_pal_seraphims_blessing : public AreaTriggerEntityScript
{
public:
    spell_at_pal_seraphims_blessing() : AreaTriggerEntityScript("spell_at_pal_seraphims_blessing") { }

    enum eSpells
    {
        BlockProc = 205069,
        Trigger = 248878
    };

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
    {
        if (m_UpdateTimer > p_Time)
        {
            m_UpdateTimer -= p_Time;
            return;
        }

        m_UpdateTimer = 1 * IN_MILLISECONDS;

        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        if (l_Caster->HasAura(eSpells::BlockProc))
            return;

        if (AuraEffect* l_AuraEffect = l_Caster->GetAuraEffect(p_AreaTrigger->GetSpellId(), EFFECT_0))
        {
            int32 l_HealthPct = l_AuraEffect->GetAmount();
            if (l_Caster->HealthBelowPct(l_HealthPct))
            {
                l_Caster->CastSpell(l_Caster, eSpells::BlockProc, true);
                l_Caster->CastSpell(l_Caster, eSpells::Trigger, true);

                if (Aura* l_Aura = l_Caster->GetAura(eSpells::Trigger))
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId()))
                        l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);
                return;
            }

            float l_Radius = (float)l_AuraEffect->GetAmount();
            std::list<Unit*> l_FriendList;

            JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_FriendList, l_Check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

            if (!l_FriendList.empty())
            {
                for (Unit* l_Unit : l_FriendList)
                {
                    if (l_Caster->IsValidAssistTarget(l_Unit) && l_Unit->IsInRaidWith(l_Caster) && l_Unit->HealthBelowPct(l_HealthPct))
                    {
                        l_Caster->CastSpell(l_Caster, eSpells::BlockProc, true);
                        l_Caster->CastSpell(l_Caster, eSpells::Trigger, true);
                        if (Aura* l_Aura = l_Caster->GetAura(eSpells::Trigger))
                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId()))
                                l_Aura->SetDuration(l_SpellInfo->Effects[EFFECT_2].BasePoints * IN_MILLISECONDS);
                        return;
                    }
                }
            }
        }
    }

    AreaTriggerEntityScript* GetAI() const override
    {
        return new spell_at_pal_seraphims_blessing();
    }

private:
    uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
};

/// Last Update 7.3.5 Build 26365
/// Called by Sindragosa's fury - 4714
class spell_at_dk_sindragosas_fury : public AreaTriggerEntityScript
{
    public:
        spell_at_dk_sindragosas_fury() : AreaTriggerEntityScript("spell_at_dk_sindragosas_fury") { }

        enum eSpells
        {
            VisualSpell     = 224399,
            DamageSpell     = 190780
        };

        void OnCreate(AreaTrigger* p_AreaTrigger)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            float l_Radius = p_AreaTrigger->GetRadius();
            float l_Distance = 40.0f;
            uint8 l_MaxSteps = (uint8)(l_Distance / l_Radius);

            for (uint8 l_i = 1; l_i < l_MaxSteps; ++l_i)
            {
                float l_PositionX = p_AreaTrigger->GetPositionX() + (l_i * l_Radius * cos(l_Caster->GetOrientation()));
                float l_PositionY = p_AreaTrigger->GetPositionY() + (l_i * l_Radius * sin(l_Caster->GetOrientation()));
                float l_PositionZ = p_AreaTrigger->GetPositionZ();
                l_Caster->UpdateAllowedPositionZ(l_PositionX, l_PositionY, l_PositionZ);
                l_Caster->CastCustomSpell(l_PositionX, l_PositionY, l_PositionZ, eSpells::VisualSpell, nullptr, nullptr, nullptr, true);
            }

            std::list<uint64>& l_SindragosasFuryTargets = l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::SindragosasFuryTargets];
            if (!l_SindragosasFuryTargets.empty())
            {
                l_SindragosasFuryTargets.sort();
                l_SindragosasFuryTargets.unique();
                for (auto l_TargetGUID : l_SindragosasFuryTargets)
                {
                    Unit* l_Target = ObjectAccessor::GetUnit(*l_Caster, l_TargetGUID);
                    if (l_Target)
                        l_Caster->CastSpell(l_Target, eSpells::DamageSpell, true);
                }
            }

            l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::SindragosasFuryTargets].clear();
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_dk_sindragosas_fury();
        }
};

/// Last Update 7.3.5 Build 26124
/// Called by Sindragosa's fury - 8430
class spell_at_dk_sindragosas_fury_targets : public AreaTriggerEntityScript
{
    public:
        spell_at_dk_sindragosas_fury_targets() : AreaTriggerEntityScript("spell_at_dk_sindragosas_fury_targets") { }

        void OnCreate(AreaTrigger* p_AreaTrigger)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();

            JadeCore::NearestAttackableUnitInPositionRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> searcher(l_Caster, l_TargetList, u_check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

            for (auto l_Itr : l_TargetList)
                if (l_Itr && l_Caster->IsValidAttackTarget(l_Itr))
                    l_Caster->m_SpellHelper.GetUint64List()[eSpellHelpers::SindragosasFuryTargets].push_back(l_Itr->GetGUID());
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_dk_sindragosas_fury_targets();
        }
};

/// Last Update 7.3.5 Build 26124
/// Called by Thal'kiel's Discord - 6913
class spell_at_dk_thalkiels_discord : public AreaTriggerEntityScript
{
    public:
        spell_at_dk_thalkiels_discord() : AreaTriggerEntityScript("spell_at_dk_thalkiels_discord") { }

        enum eSpells
        {
            ThalkielsDiscordDamage = 211727
        };

        int32 m_Timer = 1500;
        uint8 m_Hits = 0;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
        {
            if (m_Hits >= 3)
                return;

            if (m_Timer <= p_Time)
            {
                if (Unit* l_Caster = p_AreaTrigger->GetCaster())
                {
                    l_Caster->CastCustomSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::ThalkielsDiscordDamage, nullptr, nullptr, nullptr, true);
                    ++m_Hits;
                }
                m_Timer = 1500;
            }
            else
                m_Timer -= p_Time;
        }


        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            l_Caster->CastCustomSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::ThalkielsDiscordDamage, nullptr, nullptr, nullptr, true);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_dk_thalkiels_discord();
        }
};

/// Last update 7.1.5 Build 23420
/// Item Bottled Hurricane - 137378
/// Called by Raging Storm - 215296
class spell_at_raging_storm : public AreaTriggerEntityScript
{
    public:
        spell_at_raging_storm() : AreaTriggerEntityScript("spell_at_raging_storm") { }

        enum eSpells
        {
            RagingStormHeal = 215314
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_UpdateTimer >= p_Time)
            {
                m_UpdateTimer -= p_Time;
                return;
            }

            m_UpdateTimer = 1 * IN_MILLISECONDS;

            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                l_Caster->CastSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::RagingStormHeal, true);
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_raging_storm();
        }

    private:
        uint32 m_UpdateTimer = 800;
};

/// Last update 7.3.5 Build 26365
/// Called by Areatrigger Diamond Ice 4424
class spell_at_diamond_ice : public AreaTriggerEntityScript
{
    public:
        spell_at_diamond_ice() : AreaTriggerEntityScript("spell_at_diamond_ice")
        {
            m_Catched = false;
        }

        enum eSpells
        {
            DiamondIce                  = 203340,
            DiamondIceAura              = 203337,
            PvPRulesAura                = 134735,
            FreezingTrapAura            = 3355
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (!p_AreaTrigger)
                return;

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            if (m_Catched)
                return;

            Player* l_Player = p_AreaTrigger->GetCaster()->ToPlayer();
            if (!l_Player)
                return;

            Unit* l_Target = nullptr;
            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();

            JadeCore::NearestAttackableUnitInPositionRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInPositionRangeCheck> searcher(l_Caster, l_TargetList, u_check);
            p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

            for (auto l_Itr : l_TargetList)
            {
                if (l_Itr && l_Caster->IsValidAttackTarget(l_Itr))
                {
                    l_Target = l_Itr;
                    break;
                }
            }

            if (!l_Target)
                return;

            uint32 l_SpellId = eSpells::FreezingTrapAura;
            if (l_Player->HasAura(eSpells::DiamondIce) && (l_Player->CanApplyPvPSpellModifiers() || l_Player->HasAura(eSpells::PvPRulesAura)))
                l_SpellId = eSpells::DiamondIceAura;

            l_Player->m_SpellHelper.GetUint64Map()[eSpellHelpers::TriggeredByAreaTrigger][l_SpellId] = p_AreaTrigger->GetGUID();
            l_Player->CastSpell(l_Target, l_SpellId, true);

            m_Catched = true;
            p_AreaTrigger->Remove(0);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_diamond_ice();
        }

    private:
        uint8 m_Catched;
};

/// Last Update 7.3.5 build 26365
/// Called by Terror From Below - 251526
/// Called for item Terror From Below - 147016
class spell_at_terror_from_below : public AreaTriggerEntityScript
{
    enum eSpells
    {
        TerrorFromBelowDamage   = 242525
    };

    uint32 m_Timer = 0;

    public:
        spell_at_terror_from_below() : AreaTriggerEntityScript("spell_at_terror_from_below") { }

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (m_Timer < 1000)
            {
                m_Timer += p_Time;
                return;
            }

            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(p_AreaTrigger->GetCastItemGuid());
            if (!l_Trinket)
                return;

            float l_Multiplier = 0.0f;
            int32 l_TrinketItemLevel = l_Trinket->GetItemLevel(l_Player);
            RandomPropertiesPointsEntry const* l_RandomPropertiesPoints = sRandomPropertiesPointsStore.LookupEntry(l_TrinketItemLevel);
            if (l_RandomPropertiesPoints)
                l_Multiplier = l_RandomPropertiesPoints->RarePropertiesPoints[0];

            SpellInfo const* l_TerrorFromBelowInfo = sSpellMgr->GetSpellInfo(TerrorFromBelowDamage);
            if (!l_TerrorFromBelowInfo)
                return;

            float l_ScalingMultiplier = l_TerrorFromBelowInfo->Effects[EFFECT_0].ScalingMultiplier;
            float l_Amount = l_ScalingMultiplier * l_Multiplier;
            float l_DeltaScalingMultiplier = l_TerrorFromBelowInfo->Effects[EFFECT_0].DeltaScalingMultiplier;
            if (l_DeltaScalingMultiplier)
            {
                float l_Delta = l_DeltaScalingMultiplier * l_ScalingMultiplier * l_Multiplier * 0.5f;
                l_Amount += frand(-l_Delta, l_Delta);
            }

            int32 l_Damage = (int32)l_Amount;

            l_Caster->CastCustomSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), eSpells::TerrorFromBelowDamage, &l_Damage, nullptr, nullptr, true, l_Trinket);
            p_AreaTrigger->Remove();
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_terror_from_below();
        }
};

/// Last Update 7.3.5
/// Windburst - 204475  areatrigger - 6026
class spell_at_hunt_windfury : public AreaTriggerEntityScript
{
    public:
        spell_at_hunt_windfury() : AreaTriggerEntityScript("spell_at_hunt_windfury") { }

        enum eSpells
        {
            CyclonicBurst = 238124,
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            l_Caster->CastSpell(p_AreaTrigger, eSpells::CyclonicBurst, true);
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_hunt_windfury();
        }
};

/// Last Update 7.3.5 build 26365
/// Soul Fragment - 6710
class spell_at_dh_soul_fragment_demon_soul: public AreaTriggerEntityScript
{
    public:
        spell_at_dh_soul_fragment_demon_soul() : AreaTriggerEntityScript("spell_at_dh_soul_fragment_demon_soul") { }

        enum eSpells
        {
            DemonSoul   = 163073
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            if (p_AreaTrigger->GetCustomData() == 1)
                l_Caster->CastSpell(l_Caster, eSpells::DemonSoul, true);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_dh_soul_fragment_demon_soul();
        }
};

/// Last Update 7.3.5 build 26365
/// Flame Gale - 230213
class spell_at_flame_gale : public AreaTriggerEntityScript
{
public:
    spell_at_flame_gale() : AreaTriggerEntityScript("spell_at_flame_gale") { }

    enum eSpells
    {
        Spell_ThunderRitual = 230222,
        Spell_FlameGaleDamage = 230215
    };

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
    {
        if (m_UpdateTimer > p_Time)
        {
            m_UpdateTimer -= p_Time;
            return;
        }
        else
        {
            m_UpdateTimer = 1 * IN_MILLISECONDS;
        }

        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        Player* l_Player = l_Caster->ToPlayer();
        if (!l_Player)
            return;

        Item* l_Trinket = l_Player->GetItemByGuid(p_AreaTrigger->GetCastItemGuid());
        SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(p_AreaTrigger->GetSpellId());
        if (!l_Trinket || !l_SpellInfo)
            return;

        std::list<Unit*> l_TargetList;
        float l_Radius = p_AreaTrigger->GetRadius();
        JadeCore::NearestAttackableUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
        l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

        for (Unit* l_Unit : l_TargetList)
        {
            if (l_Unit->HasAura(eSpells::Spell_ThunderRitual) && !m_Modified)
            {
                l_Caster->CastSpell(l_Unit, eSpells::Spell_FlameGaleDamage, true, l_Trinket);
                AddPct(l_Radius, l_SpellInfo->Effects[EFFECT_3].BasePoints);
                p_AreaTrigger->SetRadius(l_Radius);
                m_Modified = true;
                l_Caster->m_SpellHelper.GetBool(eSpellHelpers::FlameGaleBonusDamage) = true;
            }
            else
            {
                l_Caster->CastSpell(l_Unit, eSpells::Spell_FlameGaleDamage, true, l_Trinket);
            }
        }
    }

    void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/) override
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        l_Caster->m_SpellHelper.GetBool(eSpellHelpers::FlameGaleBonusDamage) = false;
    }

    AreaTriggerEntityScript* GetAI() const override
    {
        return new spell_at_flame_gale();
    }

private:
    uint32 m_UpdateTimer = 0;
    bool m_Modified = false;
};

/// Last Update 7.3.5 build 26365
/// Wave of Flame - 251947
class spell_at_wave_of_flame : public AreaTriggerEntityScript
{
    public:
        spell_at_wave_of_flame() : AreaTriggerEntityScript("spell_at_wave_of_flame") { }

        enum eSpells
        {
            WaveOfFlame = 251948
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(p_AreaTrigger->GetCastItemGuid());
            if (!l_Trinket)
                return;

            std::list<Unit*> l_TargetList;
            float l_Radius = p_AreaTrigger->GetRadius();
            JadeCore::NearestAttackableUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
            JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

            std::list<uint64>& l_AffectedList = l_Player->m_SpellHelper.GetUint64List()[eSpellHelpers::SmolderingTitanguard];
            for (Unit* l_Unit : l_TargetList)
            {
                std::list<uint64>::iterator l_Found = std::find(l_AffectedList.begin(), l_AffectedList.end(), l_Unit->GetGUID());
                if (!l_Caster->IsValidAttackTarget(l_Unit) || l_Found != l_AffectedList.end())
                    continue;

                l_Caster->CastSpell(l_Unit, eSpells::WaveOfFlame, true, l_Trinket);
                l_Player->m_SpellHelper.GetUint64List()[eSpellHelpers::SmolderingTitanguard].push_back(l_Unit->GetGUID());
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new spell_at_wave_of_flame();
        }
};

/// Last Update 7.3.5 build 26365
/// Fire Mines - 11883
class spell_at_item_fire_mines : public AreaTriggerEntityScript
{
    public:
        spell_at_item_fire_mines() : AreaTriggerEntityScript("spell_at_item_fire_mines") { }

        enum eSpells
        {
            FireMinesDamage = 253321
        };

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 /*p_Time*/)
        {
            Unit* l_Caster = p_AreaTrigger->GetCaster();
            if (!l_Caster)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return;

            Item* l_Item = l_Player->GetItemByGuid(p_AreaTrigger->GetCastItemGuid());
            l_Caster->CastSpell(p_AreaTrigger, eSpells::FireMinesDamage, true, l_Item);
        }

        AreaTriggerEntityScript* GetAI() const
        {
            return new spell_at_item_fire_mines();
        }
};

/// Last Update 7.3.5
/// Ring of Peace - 116844
class spell_at_monk_ring_of_peace : public AreaTriggerEntityScript
{
public:
    spell_at_monk_ring_of_peace() : AreaTriggerEntityScript("spell_at_monk_ring_of_peace") { }

    enum eSpells
    {
        RingOfPeaceBegin = 237371,
        RingOfPeace = 142895
    };

    void OnCreate(AreaTrigger* p_AreaTrigger) override
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        l_Caster->CastSpell(p_AreaTrigger, eSpells::RingOfPeaceBegin, true);
        m_UpdateTimer = 0.4 * IN_MILLISECONDS;
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
    {
        if (m_UpdateTimer > p_Time)
        {
            m_UpdateTimer -= p_Time;
            return;
        }

        m_UpdateTimer = 0.4 * IN_MILLISECONDS;

        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (!l_Caster)
            return;

        l_Caster->CastSpell(p_AreaTrigger, eSpells::RingOfPeace, true);
    }

    AreaTriggerEntityScript* GetAI() const override
    {
        return new spell_at_monk_ring_of_peace();
    }

private:
    int32 m_UpdateTimer = 400;
};

/// Last Update 7.3.5
/// Trap Rune - 204192
class spell_at_trap_rune : public AreaTriggerEntityScript
{
public:

    spell_at_trap_rune() : AreaTriggerEntityScript("spell_at_trap_rune") { }

    enum eRequiredNPC
    {
        NightborneSiegecaster = 101783,
        ImperialSpellbreaker = 111527,
        NightborneInvader = 111599,
        NightborneInfiltrator = 101784,
        NightborneArcanist = 111530,
    };

    enum eSpells
    {
        Ensnared = 204195,
    };

    uint32 m_Timer = 1 * TimeConstants::IN_MILLISECONDS;
    bool l_Activated = false;

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
    {
        if (m_Timer <= p_Time)
        {
            if (!l_Activated)
            {
                std::list<Creature*> l_TargetList{};
                p_AreaTrigger->GetCreatureListWithEntryInGrid(l_TargetList, eRequiredNPC::NightborneSiegecaster, 2.0f);
                p_AreaTrigger->GetCreatureListWithEntryInGrid(l_TargetList, eRequiredNPC::ImperialSpellbreaker, 2.0f);
                p_AreaTrigger->GetCreatureListWithEntryInGrid(l_TargetList, eRequiredNPC::NightborneInvader, 2.0f);
                p_AreaTrigger->GetCreatureListWithEntryInGrid(l_TargetList, eRequiredNPC::NightborneInfiltrator, 2.0f);
                p_AreaTrigger->GetCreatureListWithEntryInGrid(l_TargetList, eRequiredNPC::NightborneArcanist, 2.0f);
                if (!l_TargetList.empty())
                {
                    l_Activated = true;

                    for (auto l_Itr : l_TargetList)
                        p_AreaTrigger->CastSpellByOwnerFromATLocation(l_Itr, eSpells::Ensnared);
                }
                m_Timer = 1 * TimeConstants::IN_MILLISECONDS;
            }
            else
            {
                p_AreaTrigger->Despawn();
            }
        }
        else
            m_Timer -= p_Time;
    }

    spell_at_trap_rune* GetAI() const override
    {
        return new spell_at_trap_rune();
    }
};

#ifndef __clang_analyzer__
void AddSC_areatrigger_spell_scripts()
{
    /// Paladin Area Trigger
    new spell_at_pal_consecration();
    new spell_at_pal_seraphims_blessing();

    /// Druid Area Trigger
    new spell_at_druid_fungal_growth();
    new spell_at_druid_ursol_vortex();
    new spell_at_druid_solar_beam();
    new spell_at_druid_lunar_beam();

    /// Hunter Area Trigger
    new spell_at_hun_binding_shot();
    new spell_at_hun_ice_trap();
    new spell_at_hun_ice_trap_effect();
    new spell_at_hun_snake_trap();
    new spell_at_hun_flare();
    new spell_at_hun_wild_protector();
    new spell_at_diamond_ice();
    new spell_at_hunt_windfury();

    /// Mage Area Trigger
    new spell_at_mage_wod_frost_2p_bonus();
    new spell_at_mage_arcane_orb();
    new spell_at_mage_meteor_timestamp();
    new spell_at_mage_rune_of_power();
    new spell_at_mage_frozen_orb();

    /// Monk Area Trigger
    new spell_at_monk_healing_sphere();
    new spell_at_monk_afterlife_healing_sphere();
    new spell_at_monk_chi_sphere_afterlife();
    new spell_at_monk_gift_of_the_ox();
    new spell_at_monk_chi_burst();
    new spell_at_monk_charging_ox_wave();
    new spell_at_song_of_chi_ji();
    new spell_at_monk_chi_orbit();
    new spell_at_monk_ring_of_peace();

    /// Priest Area Trigger
    new spell_at_pri_power_word_barrier();
    new spell_at_pri_angelic_feather();
    new spell_at_pri_halo();
    new spell_at_pri_divine_star();

    /// Rogue Area Trigger
    new spell_at_rogue_smoke_bomb();
    new spell_at_rogue_cut_to_the_chase();

    /// Demon Hunter Area Trigger
    new spell_at_dh_inner_demon();
    new spell_at_dh_sigil_of_silence();
    new spell_at_dh_sigil_of_chains();
    new spell_at_dh_sigil_of_misery();
    new spell_at_dh_sigil_of_flame();
    new spell_at_dh_flury_of_the_illidari();
    new spell_at_dh_lesser_soul_fragment();
    new spell_at_dh_soul_fragment_demon_soul();

    /// Death Knight Area Trigger
    new spell_at_dk_defile();
    new spell_at_dk_sindragosas_fury();
    new spell_at_dk_sindragosas_fury_targets();

    /// Warrior Area Trigger
    new spell_at_war_into_the_fray();

    /// Item Areatrigger
    new spell_at_item_volatile_ichor();
    new spell_at_item_nightmarish_ichor();
    new spell_at_item_tormenting_cyclone();
    new spell_at_item_delicious_cake();
    new spell_at_item_leeching_void();
    new spell_at_raging_storm();
    new spell_at_trap_rune();

    /// Shaman Areatrigger
    new spell_at_shaman_windfury_totem();

    /// warlock Areatrigger
    new spell_at_warl_rain_of_fire();
    new spell_at_dk_thalkiels_discord();

    /// Trinkets' Areatriggers
    new spell_at_terror_from_below();
    new spell_at_flame_gale();
    new spell_at_wave_of_flame();
    new spell_at_item_fire_mines();
}
#endif
