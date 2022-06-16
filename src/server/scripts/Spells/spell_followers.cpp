////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
* Scripts for spells of support followers.
* Ordered alphabetically using scriptname.
* Scriptnames of files in this file should be prefixed with "spell_mastery_".
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ScriptedCosmeticAI.hpp"
#include "Player.h"
#include "WorldPacket.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Language.h"
#include "ScriptedCosmeticAI.hpp"
#include "CreatureTextMgr.h"
#include "ScriptPCH.h"
#include "HelperDefines.h"


/// Last Update 7.0.3 Build 22522
/// Lulubell - 109951
/// For Six Soul Bag - 218837
class npc_lulubell : public CreatureScript
{
    public:
        npc_lulubell() : CreatureScript("npc_lulubell") { }

        struct npc_lulubellAI : public ScriptedAI
        {
            npc_lulubellAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                SixSoulBagHaste         = 215858,
                SixSoulBagMastery       = 215865,
                SixSoulBagCritical      = 215866,
                SixSoulBagVersatility   = 215867,
                SixSoulBagSoulShard     = 215868
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                std::array<uint32, 5> m_SixSoulBagBuffs =
                {
                    {
                        eSpells::SixSoulBagHaste,
                        eSpells::SixSoulBagMastery,
                        eSpells::SixSoulBagCritical,
                        eSpells::SixSoulBagVersatility,
                        eSpells::SixSoulBagSoulShard
                    }
                };

                me->CastSpell(p_Summoner, m_SixSoulBagBuffs[urand(1, m_SixSoulBagBuffs.size()) - 1], false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lulubellAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Summon Eredar Twin - 216154
class spell_summon_eredar_twin : public SpellScriptLoader
{
    public:
        spell_summon_eredar_twin() : SpellScriptLoader("spell_summon_eredar_twin") { }

        class spell_summon_eredar_twin_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_eredar_twin_SpellScript);

            enum eSpells
            {
                SummonSacrolash = 216157,
                SummonAlythess  = 216160
            };

            void HandleAfterCast()
            {
                std::array<uint32, 2> l_Summons =
                {
                    {
                        eSpells::SummonAlythess,
                        eSpells::SummonSacrolash
                    }
                };

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, l_Summons[urand(0, l_Summons.size() - 1)], true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_summon_eredar_twin_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_eredar_twin_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Lady Sacrolash - 108838
/// For Summon Eredar Twin - 216154
class npc_lady_sacrolash : public CreatureScript
{
    public:
        npc_lady_sacrolash() : CreatureScript("npc_lady_sacrolash") { }

        struct npc_lady_sacrolashAI : public ScriptedAI
        {
            npc_lady_sacrolashAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                ConfoundingBlowCharge   = 216270,
                ConfoundingBlow         = 216246
            };

            bool m_BlowCasted = false;

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                Unit* l_Summoner = me->GetOwner();
                if (!l_Summoner)
                    return;

                Player* l_Player = l_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = l_Summoner->ToPlayer()->GetSelectedUnit();
                if (!l_Target)
                    return;

                if (!m_BlowCasted)
                {
                    me->CastSpell(l_Target, eSpells::ConfoundingBlowCharge, false);
                    me->CastSpell(l_Target, eSpells::ConfoundingBlow, true);
                    m_BlowCasted = true;
                    return;
                }

                me->Attack(l_Target, true);
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lady_sacrolashAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Grand Warlock Alythess - 108839
/// For Summon Eredar Twin - 216154
class npc_grand_warlock_alythess : public CreatureScript
{
    public:
        npc_grand_warlock_alythess() : CreatureScript("npc_grand_warlock_alythess") { }

        struct npc_grand_warlock_alythessAI : public ScriptedAI
        {
            npc_grand_warlock_alythessAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                Pyrogenics  = 216259,
                Blaze       = 216262
            };

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                Unit* l_Summoner = me->GetOwner();
                if (!l_Summoner)
                    return;

                Player* l_Player = l_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                if (!l_Summoner->HasAura(eSpells::Pyrogenics))
                {
                    me->CastSpell(l_Summoner, eSpells::Pyrogenics, true);
                    return;
                }

                Unit* l_Target = l_Player->GetSelectedUnit();
                if (!l_Target)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                me->CastSpell(l_Target, eSpells::Blaze, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_grand_warlock_alythessAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Emerald Charge - 219209
class spell_emerald_charge : public SpellScriptLoader
{
    public:
        spell_emerald_charge() : SpellScriptLoader("spell_emerald_charge") { }

        class spell_emerald_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_emerald_charge_SpellScript);

            enum eSpells
            {
                SummonKeeperRemulos = 219211
            };

            void HandeAfterCast()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Dest || !l_Caster)
                    return;

                l_Caster->CastSpell(l_Dest, eSpells::SummonKeeperRemulos, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_emerald_charge_SpellScript::HandeAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_emerald_charge_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Keeper Remulos - 110096
/// For Summon Keeper Remulos - 219211
class npc_keeper_remulos : public CreatureScript
{
    public:
        npc_keeper_remulos() : CreatureScript("npc_keeper_remulos") { }

        enum eSpells
        {
            EmeraldChargeCharge     = 219261,
            EmeraldChargeDamage     = 219372,
            EmeralChargeFlameAT     = 219373,
            EmeraldChargeHeal       = 219408
        };

        struct npc_keeper_remulosAI : public ScriptedAI
        {
            npc_keeper_remulosAI(Creature* creature) : ScriptedAI(creature)
            {
                m_Dest = nullptr;
            }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                if (!m_Dest)
                    return;

                /*MotionMaster* l_MotionMaster = me->GetMotionMaster();
                if (!l_MotionMaster)
                    return;*/

                //l_MotionMaster->MoveCharge(m_Dest->m_positionX, m_Dest->m_positionY, m_Dest->m_positionZ, 42.0f);
                me->CastSpell(m_Dest, eSpells::EmeraldChargeCharge, true);
            }

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                Unit* l_Summoner = me->GetOwner();
                if (!l_Summoner)
                    return;

                Player* l_Player = l_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                me->CastSpell(me, eSpells::EmeralChargeFlameAT, true);
            }

            public:
                WorldLocation const* m_Dest;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_keeper_remulosAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Called By Summon Keeper Remulos - 219211
class spell_summon_keeper_remulos : public SpellScriptLoader
{
    public:
        spell_summon_keeper_remulos() : SpellScriptLoader("spell_summon_keeper_remulos") { }

        class spell_summon_keeper_remulos_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_keeper_remulos_SpellScript);

            enum eDatas
            {
                KeeperRemulos = 110096
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                WorldLocation const* l_Dest = GetExplTargetDest();
                if (!l_Dest || !l_Caster)
                    return;

                Creature* l_Remulos = nullptr;
                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon->GetEntry() == eDatas::KeeperRemulos)
                        l_Remulos = l_Summon->ToCreature();
                }
                if (!l_Remulos)
                    return;

                npc_keeper_remulos::npc_keeper_remulosAI* l_RemulosAI = CAST_AI(npc_keeper_remulos::npc_keeper_remulosAI, l_Remulos->GetAI());
                if (!l_RemulosAI)
                    return;

                l_RemulosAI->m_Dest = l_Dest;
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_summon_keeper_remulos_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_keeper_remulos_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Calydus - 108741
/// For Summon Calydus - 215979
class npc_calydus : public CreatureScript
{
    public:
        npc_calydus() : CreatureScript("npc_calydus") { }

        struct npc_calydusAI : public ScriptedAI
        {
            npc_calydusAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                EternalBanishment = 216007
            };

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                Unit* l_Summoner = me->GetOwner();
                if (!l_Summoner)
                    return;

                Player* l_Player = l_Summoner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = l_Player->GetSelectedUnit();
                if (!l_Target)
                    return;

                me->CastSpell(l_Target, eSpells::EternalBanishment, false);
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_calydusAI(creature);
        }
};


/// Last Update 7.0.3 Build 22522
/// Called By Nightmarish Visions - 218543
class spell_nightmarish_visions : public SpellScriptLoader
{
    public:
        spell_nightmarish_visions() : SpellScriptLoader("spell_nightmarish_visions") { }

        class spell_nightmarish_visions_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nightmarish_visions_SpellScript);

            enum eSpells
            {
                SummonNaralex = 218533
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonNaralex, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_nightmarish_visions_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_nightmarish_visions_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Alonsus Faol  875
class npc_alonsus_faol : public CreatureScript
{
    public:
        npc_alonsus_faol() : CreatureScript("npc_alonsus_faol") { }

        struct npc_alonsus_faol_ScriptedAI : public ScriptedAI
        {
            npc_alonsus_faol_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                SearingLight = 222098
            };

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                Unit* l_Summoner = me->GetOwner();
                if (!l_Summoner)
                    return;

                me->CastSpell(l_Summoner, eSpells::SearingLight, false);
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_alonsus_faol_ScriptedAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Hodir - 111941
class npc_hodir : public CreatureScript
{
    public:
        npc_hodir() : CreatureScript("npc_hodir") { }

        struct npc_hodir_ScriptedAI : public ScriptedAI
        {
            npc_hodir_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                WindsOfTheNorth = 222935
            };

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                Unit* l_Summoner = me->GetOwner();
                if (!l_Summoner)
                    return;

                me->CastSpell(l_Summoner, eSpells::WindsOfTheNorth, false);
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hodir_ScriptedAI(creature);
        }
};

/// Called by Finna, Personal Phalanx - 223580
/// Called for Finna Bjornsdottir (npc) - 109508
class spell_finna_personal_phalanx : public SpellScriptLoader
{
    public:
        spell_finna_personal_phalanx() : SpellScriptLoader("spell_finna_personal_phalanx") { }

        class spell_finna_personal_phalanx_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_finna_personal_phalanx_SpellScript);

            enum eNPC
            {
                Finna       = 112301
            };

            enum eSpells
            {
                SummonFinna = 223584,
                FinnaTimer  = 223583
            };

            void HandleOnObjectAreaTargetSelect(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Creature* l_Finna = nullptr;
                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon && l_Summon->GetEntry() == eNPC::Finna)
                    {
                        l_Finna = l_Summon->ToCreature();
                        break;
                    }
                }

                uint32 l_InCombatWithCaster = 0;
                for (WorldObject* l_Target : p_Targets)
                {
                    if (!l_Target)
                        continue;

                    Unit* l_TargetUnit = l_Target->ToUnit();
                    if (!l_TargetUnit)
                        continue;

                    Unit* l_TargetVictim = l_TargetUnit->getVictim();
                    if (!l_TargetVictim)
                        continue;

                    if (l_TargetVictim->GetGUID() == l_Caster->GetGUID())
                        l_InCombatWithCaster++;

                    if (l_Finna && l_TargetVictim->GetGUID() == l_Finna->GetGUID())
                        l_InCombatWithCaster++;
                }

                if (l_InCombatWithCaster <= 1)
                {
                    if (l_Finna)
                        l_Finna->DespawnOrUnsummon(1);

                    return;
                }

                if (l_Caster->HasAura(eSpells::FinnaTimer))
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonFinna, true);
                l_Caster->CastSpell(l_Caster, eSpells::FinnaTimer, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_finna_personal_phalanx_SpellScript::HandleOnObjectAreaTargetSelect, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_finna_personal_phalanx_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Finna Bjornsdottir - 112301 (Personal Phalanx - 223584)
class npc_finna_bjornsdottir : public CreatureScript
{
    public:
        npc_finna_bjornsdottir() : CreatureScript("npc_finna_bjornsdottir") { }

        struct npc_finna_bjornsdottir_ScriptedAI : public ScriptedAI
        {
            npc_finna_bjornsdottir_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                ChallengingShout    = 223591,
                RetaliationAura     = 223592
            };

            bool m_Casted = false;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (m_Casted)
                    return;

                me->CastSpell(me, eSpells::RetaliationAura, false);
                me->CastSpell(me, eSpells::ChallengingShout, false);

                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_finna_bjornsdottir_ScriptedAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Retaliation - 223582
/// Called for Finna Bjornsdottir - 112301 (Personal Phalanx - 223584)
class spell_retaliation_finna_bjornsdottir: public SpellScriptLoader
{
    public:
        spell_retaliation_finna_bjornsdottir() : SpellScriptLoader("spell_retaliation_finna_bjornsdottir") { }

        class spell_retaliation_finna_bjornsdottir_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_retaliation_finna_bjornsdottir_AuraScript);

            enum eSpells
            {
                RetaliationDmg = 223593
            };

            void HandleOnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_ProcInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcInfo.GetActor();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::RetaliationDmg, false);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_retaliation_finna_bjornsdottir_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_retaliation_finna_bjornsdottir_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Mathias Shaw - 112117 (SI:7 Intel - 223310)
class npc_mathias_shaw : public CreatureScript
{
    public:
        npc_mathias_shaw() : CreatureScript("npc_mathias_shaw") { }

        struct npc_mathias_shaw_ScriptedAI : public ScriptedAI
        {
            npc_mathias_shaw_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                SI7Aura = 223360,
                SI7Buff = 222129
            };

            void Reset() override
            {
                me->SetOrientation(0.785f);
                TempSummon* l_Summon = me->ToTempSummon();
                if (!l_Summon)
                    return;

                Unit* l_Summoner = l_Summon->GetSummoner();
                if (!l_Summoner)
                    return;

                uint64 l_SummonerGUID = l_Summoner->GetGUID();

                l_Summon->AddDelayedEvent([l_Summon, l_SummonerGUID]() -> void
                {
                    Unit* l_Summoner = sObjectAccessor->GetUnit(*l_Summon, l_SummonerGUID);
                    if (!l_Summoner)
                        return;

                    l_Summoner->RemoveAura(eSpells::SI7Buff);
                    l_Summon->CastSpell(l_Summoner, eSpells::SI7Aura, false);
                }, 1000);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mathias_shaw_ScriptedAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by SI:7 Intel - 222129
class spell_si7_intel: public SpellScriptLoader
{
    public:
        spell_si7_intel() : SpellScriptLoader("spell_si7_intel") { }

        class spell_si7_intel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_si7_intel_AuraScript);

            enum eSpells
            {
                SI7Buff     = 222132,
                SI7Timer    = 222147
            };

            bool m_Stealthed = false;

            void OnTick(AuraEffect const* p_AurEff)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                if (l_Owner->HasStealthAura())
                {
                    l_Owner->CastSpell(l_Owner, eSpells::SI7Buff, true);
                    m_Stealthed = true;
                }
                else if (m_Stealthed)
                {
                    l_Owner->CastSpell(l_Owner, eSpells::SI7Timer, true);
                    m_Stealthed = false;
                }
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                if (!l_Owner)
                    return;

                l_Owner->RemoveAura(eSpells::SI7Buff);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_si7_intel_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_si7_intel_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_si7_intel_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by SI:7 Intel - 222147
class spell_si7_intel_remove: public SpellScriptLoader
{
    public:
        spell_si7_intel_remove() : SpellScriptLoader("spell_si7_intel_remove") { }

        class spell_si7_intel_remove_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_si7_intel_remove_AuraScript);

            enum eSpells
            {
                SI7Buff = 222132
            };

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (!l_Caster->HasStealthAura())
                 l_Caster->RemoveAura(eSpells::SI7Buff);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_si7_intel_remove_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_si7_intel_remove_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// High Priestess Ishanah - 111536 (Light of the Naaru - 222112)
class npc_high_priestess_ishanah : public CreatureScript
{
    public:
        npc_high_priestess_ishanah() : CreatureScript("npc_high_priestess_ishanah") { }

        struct npc_high_priestess_ishanah_ScriptedAI : public ScriptedAI
        {
            npc_high_priestess_ishanah_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                LOTNHealAndDamage = 222122
            };

            bool m_Casted = false;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING) || m_Casted)
                    return;

                me->CastSpell(me, eSpells::LOTNHealAndDamage, false);
                me->DespawnOrUnsummon(2000);
                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_high_priestess_ishanah_ScriptedAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Seal of Ravenholdt - 222717
class spell_seal_of_ravenholdt : public SpellScriptLoader
{
    public:
        spell_seal_of_ravenholdt() : SpellScriptLoader("spell_seal_of_ravenholdt") { }

        class spell_seal_of_ravenholdt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_seal_of_ravenholdt_SpellScript);

            enum eCreatures
            {
                LordJorachRavenholdt = 111867
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                for (Unit* l_Controlled : l_Caster->m_Controlled)
                {
                    if (l_Controlled && l_Controlled->GetEntry() == eCreatures::LordJorachRavenholdt)
                    {
                        l_Controlled->SetTarget(l_Target->GetGUID());
                        break;
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_seal_of_ravenholdt_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_seal_of_ravenholdt_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Lord Jorach Ravenholdt - 111867 (Seal of Ravenholdt - 222717)
class npc_lord_jorach_ravenholdt : public CreatureScript
{
    public:
        npc_lord_jorach_ravenholdt() : CreatureScript("npc_lord_jorach_ravenholdt") { }

        struct npc_lord_jorach_ravenholdt_ScriptedAI : public ScriptedAI
        {
            npc_lord_jorach_ravenholdt_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                Dispatch = 222359
            };

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                uint64 l_TargetGUID = me->GetTargetGUID();
                if (me->HasUnitState(UNIT_STATE_CASTING) || l_TargetGUID == 0)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Dispatch);
                if (!l_Target || !l_SpellInfo)
                    return;

                int32 l_Amount = CalculatePct(l_Target->GetMaxHealth(), 100 - l_SpellInfo->Effects[EFFECT_1].BasePoints);
                me->CastCustomSpell(l_Target, eSpells::Dispatch, &l_Amount, nullptr, nullptr, false);

                me->DespawnOrUnsummon(1000);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lord_jorach_ravenholdt_ScriptedAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Thorim - 112046 (Titan's Wrath - 223132)
class npc_thorim : public CreatureScript
{
    public:
        npc_thorim() : CreatureScript("npc_thorim") { }

        struct npc_thorim_ScriptedAI : public ScriptedAI
        {
            npc_thorim_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                TitansWrathStun = 223911,
                TitansWrathBuff = 223135
            };

            bool m_Casted = false;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING) || m_Casted)
                    return;

                Unit* l_Summoner = me->GetOwner();
                if (!l_Summoner)
                    return;

                l_Summoner->RemoveAura(eSpells::TitansWrathBuff);

                me->CastSpell(me, eSpells::TitansWrathStun, false);
                me->CastSpell(l_Summoner, eSpells::TitansWrathBuff, false);
                me->DespawnOrUnsummon(4000);
                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_thorim_ScriptedAI(creature);
        }
};

/// Last Update 7.3.5 - Build 26365
/// Thorim Colossal Might - 223135
class spell_thorim_colossal_might : public SpellScriptLoader
{
    public:
        spell_thorim_colossal_might() : SpellScriptLoader("spell_thorim_colossal_might") { }

        class spell_thorim_colossal_might_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thorim_colossal_might_SpellScript);

            enum eSpells
            {
                TitanWrathBuff = 223135
            };

            enum eWorldLoc
            {
                BrokenIsle = 1220
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                if (l_Caster->GetMapId() != eWorldLoc::BrokenIsle) ///< Can be used only in Broken Isle
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                if (l_Target->getClass() != Classes::CLASS_WARRIOR) ///< Can target only warrior
                    return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_thorim_colossal_might_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_thorim_colossal_might_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Meat Hook - 221226
class spell_npc_meat_hook_dmg : public SpellScriptLoader
{
    public:
        spell_npc_meat_hook_dmg() : SpellScriptLoader("spell_npc_meat_hook_dmg") { }

        class spell_npc_meat_hook_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_npc_meat_hook_dmg_SpellScript);

            enum eSpells
            {
                MeatHookGrab = 221232
            };

            void HandleOnHit()
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_Caster = GetCaster();

                if (!l_Caster || !l_Target)
                    return;

                l_Target->CastSpell(l_Caster, eSpells::MeatHookGrab, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_npc_meat_hook_dmg_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_npc_meat_hook_dmg_SpellScript();
        }
};

/// Called by Blood Vanguard - 222621
class spell_blood_vanguard : public SpellScriptLoader
{
    public:
        spell_blood_vanguard() : SpellScriptLoader("spell_blood_vanguard") { }

        class spell_blood_vanguard_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_vanguard_SpellScript);

            enum eSpells
            {
                SummonLadyLiadrin   = 222622,

                BloodKnightLeft1    = 222624,
                BloodKnightLeft2    = 222626,
                BloodKnightRight1   = 222627,
                BloodKnightRight2   = 222628
            };

            std::array<uint32, 4> const m_Vanguard =
            {
                {
                    eSpells::BloodKnightLeft1,
                    eSpells::BloodKnightLeft2,
                    eSpells::BloodKnightRight1,
                    eSpells::BloodKnightRight2
                }
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonLadyLiadrin, false);

                for (uint32 l_SpellId : m_Vanguard)
                    l_Caster->CastSpell(l_Caster, l_SpellId, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_blood_vanguard_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_blood_vanguard_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Meat Hook - 221462
/// Rottgut the Abomination - 111237
class spell_npc_rottgut_the_abomination : public CreatureScript
{
    public:
        spell_npc_rottgut_the_abomination() : CreatureScript("spell_npc_rottgut_the_abomination") { }

        struct spell_npc_rottgut_the_abomination_AI : public ScriptedAI
        {
            spell_npc_rottgut_the_abomination_AI(Creature *creature) : ScriptedAI(creature) { }

            uint64 m_TargetGUID;
            bool m_TargetHooked;

            enum eSpells
            {
                MeatHookGrab = 221226
            };

            void Reset() override
            {
                float l_AP = me->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

                m_TargetGUID = 0;
                m_TargetHooked = false;

                me->SetBaseWeaponDamage(WeaponAttackType::BaseAttack, MAXDAMAGE, l_AP * 7);
                me->SetBaseWeaponDamage(WeaponAttackType::BaseAttack, MINDAMAGE, l_AP * 7);

                me->UpdateAttackPowerAndDamage();
            }


            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Owner = me->GetOwner();
                Unit* l_Target = GetTarget();

                if (!l_Owner || !l_Target)
                {
                    ScriptedAI::UpdateAI(p_Diff);
                    return;
                }

                if (!m_TargetHooked)
                {
                    me->CastSpell(l_Target, eSpells::MeatHookGrab, true);
                    AttackStart(l_Target);
                    m_TargetHooked = true;
                }
                else
                    ScriptedAI::UpdateAI(p_Diff);
            }

            Unit* GetTarget()
            {
                Unit* l_Target = (m_TargetGUID != 0) ? sObjectAccessor->GetUnit(*me, m_TargetGUID) : nullptr;
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MeatHookGrab);

                if (l_Target)
                    return l_Target;

                m_TargetHooked = false;

                if (!l_SpellInfo || !l_SpellInfo->RangeEntry)
                    return nullptr;

                int32 l_Radius = l_SpellInfo->RangeEntry->maxRangeHostile;

                std::list<Unit*> l_TargetList;
                JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck l_Check(me, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyAttackableVisibleUnitInObjectRangeCheck> l_Searcher(me, l_TargetList, l_Check);
                me->VisitNearbyObject(l_Radius, l_Searcher);

                l_TargetList.sort(JadeCore::DistanceOrderPred(me));

                if (l_TargetList.empty())
                    return nullptr;

                m_TargetGUID = l_TargetList.front()->GetGUID();
                return l_TargetList.front();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_rottgut_the_abomination_AI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Lady Liadrin - 111794 (Blood Vanguard - 222619)
class npc_lady_liadrin : public CreatureScript
{
    public:
        npc_lady_liadrin() : CreatureScript("npc_lady_liadrin") { }

        struct npc_lady_liadrin_ScriptedAI : public ScriptedAI
        {
            npc_lady_liadrin_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                ArgentWarhorse  = 67466,
                Trample         = 222171
            };

            void Reset() override
            {
                me->CastSpell(me, eSpells::ArgentWarhorse, true);

                Unit* l_Summoner = me->GetOwner();
                MotionMaster* l_MotionMaster = me->GetMotionMaster();
                if (!l_Summoner || !l_MotionMaster)
                return;

                me->CastSpell(me, eSpells::Trample, true, nullptr, nullptr, l_Summoner->GetGUID());

                Position l_Dest = me->GetPosition() + (l_Summoner->GetPosition() - me->GetPosition()) * 10.0f;

                l_MotionMaster->MoveCharge(&l_Dest, 9.7f, EVENT_CHARGE, true);

                me->DespawnOrUnsummon(1500);
            }

            void UpdateAI(uint32 const p_Time) override
            {
                me->UpdateGroundPositionZ(me->m_positionX, me->m_positionY, me->m_positionZ);
                me->UpdatePosition(me->GetPosition());
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lady_liadrin_ScriptedAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Garona Halforcen - 111351
class spell_npc_garona_halforcen : public CreatureScript
{
    public:
        spell_npc_garona_halforcen() : CreatureScript("spell_npc_garona_halforcen") { }

        struct spell_npc_garona_halforcen_AI : public ScriptedAI
        {
            spell_npc_garona_halforcen_AI(Creature *creature) : ScriptedAI(creature) { }

            int32 m_Cooldown;
            int32 m_Chance;

            enum eSpells
            {
                VilePoison = 173965
            };

            void Reset() override
            {
                m_Cooldown = 500;
                m_Chance = 10;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Owner = me->GetOwner();
                Unit* l_Target = me->getVictim();

                if (!l_Owner || !l_Target || m_Cooldown || m_Chance == 0)
                {
                    m_Cooldown = (m_Cooldown > p_Diff) ? m_Cooldown - p_Diff : 0;
                    ScriptedAI::UpdateAI(p_Diff);
                    return;
                }

                m_Cooldown = 500;

                if (roll_chance_i(m_Chance))
                {
                    me->CastSpell(l_Target, eSpells::VilePoison);
                    m_Chance = 0;
                }
                else
                    m_Chance += 10;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_garona_halforcen_AI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Archmage Vargoth - 111188
class npc_archmage_vargoth : public CreatureScript
{
    public:
        npc_archmage_vargoth() : CreatureScript("npc_archmage_vargoth") { }

        struct npc_archmage_vargoth_AI : public ScriptedAI
        {
            npc_archmage_vargoth_AI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                Buff = 221335
            };

            bool m_Casted = false;
            int32 m_Cooldown;

            void Reset() override
            {
                m_Cooldown = 1000;
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING) || m_Casted)
                    return;

                m_Cooldown = (m_Cooldown > p_Diff) ? m_Cooldown - p_Diff : 0;
                if (m_Cooldown)
                    return;

                std::list<Unit*> l_PlayerList;
                Unit* l_Owner = nullptr;
                if (!me->ToTempSummon() || !(l_Owner = me->ToTempSummon()->GetSummoner()))
                    return;

                l_Owner->GetPartyMembers(l_PlayerList);

                for (Unit* l_Target : l_PlayerList)
                {
                    me->CastSpell(l_Target, eSpells::Buff, false);
                    me->AddAura(eSpells::Buff, l_Target);
                }
                me->DespawnOrUnsummon(2000);
                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_archmage_vargoth_AI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Blood Knight - 111797 (Blood Vanguard - 222619)
class npc_blood_knight : public CreatureScript
{
    public:
        npc_blood_knight() : CreatureScript("npc_blood_knight") { }

        struct npc_blood_knight_ScriptedAI : public ScriptedAI
        {
            npc_blood_knight_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eCreatures
            {
                LadyLiadrin         = 111794
            };

            enum eSpells
            {
                ThalassianWarhorse  = 34769,
                Trample             = 222171
            };

            void Reset() override
            {
                me->CastSpell(me, eSpells::ThalassianWarhorse, true);

                Unit* l_Summoner = me->GetOwner();
                MotionMaster* l_MotionMaster = me->GetMotionMaster();
                if (!l_Summoner || !l_MotionMaster)
                    return;

                me->CastSpell(me, eSpells::Trample, true, nullptr, nullptr, l_Summoner->GetGUID());

                Position l_Dest;
                for (Unit* l_Controlled : l_Summoner->m_Controlled)
                {
                    if (l_Controlled->GetEntry() == eCreatures::LadyLiadrin)
                    {
                        l_Dest = me->GetPosition() + (l_Summoner->GetPosition() - l_Controlled->GetPosition()) * 10.0f;
                        break;
                    }
                }

                l_MotionMaster->MoveCharge(&l_Dest, 9.7f, EVENT_CHARGE, true);

                me->DespawnOrUnsummon(1500);
            }

            void UpdateAI(uint32 const p_Time) override
            {
                me->UpdateGroundPositionZ(me->m_positionX, me->m_positionY, me->m_positionZ);
                me->UpdatePosition(me->GetPosition());
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_blood_knight_ScriptedAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Hemet Nesingwary - 110428
class npc_hemet_nesingwary : public CreatureScript
{
    public:
        npc_hemet_nesingwary() : CreatureScript("npc_hemet_nesingwary") { }

        struct npc_hemet_nesingwary_AI : public ScriptedAI
        {
            npc_hemet_nesingwary_AI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                Attack  = 219652,
                PvPAura = 134735
            };

            bool m_Casted = false;

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING) || m_Casted)
                    return;

                Unit* l_Owner = nullptr;
                if (!me->ToTempSummon() || !(l_Owner = me->ToTempSummon()->GetSummoner()))
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Owner, l_Owner->GetTargetGUID());
                if (!l_Target)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Attack);
                if (!l_SpellInfo)
                    return;

                if (l_Owner->HasAura(eSpells::PvPAura) || l_Target->HasAura(eSpells::PvPAura))
                    return;

                int32 l_Damage = CalculatePct(l_Target->GetMaxHealth(), l_SpellInfo->Effects[EFFECT_0].BasePoints);

                me->CastCustomSpell(l_Target, eSpells::Attack, &l_Damage, nullptr, nullptr, true, nullptr, nullptr, l_Owner->GetGUID());
                me->DespawnOrUnsummon(2000);
                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hemet_nesingwary_AI(creature);
        }
};

/// Called by Trample - 222172
class spell_trample : public SpellScriptLoader
{
    public:
        spell_trample() : SpellScriptLoader("spell_trample") { }

        class spell_trample_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_trample_SpellScript);

            enum eSpells
            {
                TrampleTrigger = 222175
            };

            void HandleOnObjectAreaTargetSelect(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (WorldObject* l_Object : p_Targets)
                {
                    if (!l_Object)
                        continue;

                    Unit* l_Target = l_Object->ToUnit();
                    if (!l_Target)
                        continue;

                    l_Caster->CastSpell(l_Target, eSpells::TrampleTrigger, false);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_trample_SpellScript::HandleOnObjectAreaTargetSelect, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_trample_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by That one's mine! - 219578
class spell_npc_that_ones_mine : public SpellScriptLoader
{
    public:
        spell_npc_that_ones_mine() : SpellScriptLoader("spell_npc_that_ones_mine") { }

        class spell_npc_that_ones_mine_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_npc_that_ones_mine_AuraScript);

            enum eSpells
            {
                SummonHemetNesingwary = 219590,
                PvpAura               = 134735
            };

            bool HandleCheckProc(ProcEventInfo&)
            {
                Unit* l_Caster = GetUnitOwner();

                if (!l_Caster || !l_Caster->HasAura(eSpells::PvpAura))
                    return false;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Caster, l_Caster->GetTargetGUID());

                if (l_Target && l_Target->ToCreature())
                {
                    Creature* l_Creature = l_Target->ToCreature();
                    CreatureEliteType l_Rank = static_cast<CreatureEliteType>(l_Creature->GetCreatureTemplate()->rank);

                    if (l_Creature->isWorldBoss())
                        return false;

                    if (l_Rank != CreatureEliteType::CREATURE_ELITE_NORMAL && l_Rank != CreatureEliteType::CREATURE_WEAK)
                        return false;
                }

                return true;
            }

            void HandleProc(ProcEventInfo& eventInfo)
            {
                Unit* l_Caster = GetUnitOwner();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonHemetNesingwary, true);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_npc_that_ones_mine_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_npc_that_ones_mine_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_npc_that_ones_mine_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Judgment's Gaze - 221745
class spell_judgments_gaze : public SpellScriptLoader
{
    public:
        spell_judgments_gaze() : SpellScriptLoader("spell_judgments_gaze") { }

        class spell_judgments_gaze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_judgments_gaze_SpellScript);

            enum eCreatures
            {
                Lothraxion          = 111330
            };

            enum eSpells
            {
                SummonLothraxion    = 221743
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();;
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonLothraxion, true);

                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon->GetEntry() == eCreatures::Lothraxion)
                    {
                        l_Summon->SetTarget(l_Target->GetGUID());
                        break;
                    }
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_judgments_gaze_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_judgments_gaze_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Lothraxion - 111330
class npc_lothraxion : public CreatureScript
{
    public:
        npc_lothraxion() : CreatureScript("npc_lothraxion") { }

        struct npc_lothraxion_AI : public ScriptedAI
        {
            npc_lothraxion_AI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                JudgmentsGaze = 226912
            };

            bool m_Casted = false;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                uint64 l_TargetGUID = me->GetTargetGUID();
                if (l_TargetGUID == 0 || me->HasUnitState(UNIT_STATE_CASTING) || m_Casted)
                    return;

                Unit* l_Victim = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                if (!l_Victim)
                    return;

                me->CastSpell(l_Victim, eSpells::JudgmentsGaze, false);

                me->DespawnOrUnsummon(4000);

                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lothraxion_AI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Stealth Attack - 210929
class spell_stealth_attack : public SpellScriptLoader
{
    public:
        spell_stealth_attack() : SpellScriptLoader("spell_stealth_attack") { }

        class spell_stealth_attack_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stealth_attack_SpellScript);

            enum eSpells
            {
                TaranZhuTimer       = 219311,
                SummonTaranZhu      = 212428
            };

            bool m_ShouldProc = false;

            void HandleOnObjectAreaTargetSelect(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();;
                if (!l_Caster)
                    return;

                for (WorldObject* l_Object : p_Targets)
                {
                    Unit* l_Target = l_Object->ToUnit();
                    if (!l_Target)
                        return;

                    Unit* l_Victim = l_Target->getVictim();
                    if (!l_Victim)
                        return;

                    if (l_Victim->GetGUID() == l_Caster->GetGUID())
                    {
                        m_ShouldProc = true;
                        return;
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();;
                if (!l_Caster || l_Caster->HasAura(eSpells::TaranZhuTimer) || !m_ShouldProc)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonTaranZhu, true);
                l_Caster->CastSpell(l_Caster, eSpells::TaranZhuTimer, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_stealth_attack_SpellScript::HandleOnObjectAreaTargetSelect, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_stealth_attack_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_stealth_attack_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Taran Zhu - 107022
class npc_taran_zhu : public CreatureScript
{
    public:
        npc_taran_zhu() : CreatureScript("npc_taran_zhu") { }

        struct npc_taran_zhu_AI : public ScriptedAI
        {
            npc_taran_zhu_AI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                ShadoStrike = 212435
            };

            bool m_Casted = false;

            void Reset() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                me->SetOrientation(l_Owner->GetOrientation() + 2.355f);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (m_Casted)
                    return;

                me->CastSpell(me, eSpells::ShadoStrike, false);

                me->DespawnOrUnsummon(1500);
                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_taran_zhu_AI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Farseer Nobundo- 109455
class npc_farseer_nobundo : public CreatureScript
{
    public:
        npc_farseer_nobundo() : CreatureScript("npc_farseer_nobundo") { }

        struct npc_farseer_nobundo_AI : public ScriptedAI
        {
            npc_farseer_nobundo_AI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                HealingSurge    = 217938,
                Riptide         = 217924,
                LightningBolt   = 218013
            };

            EventMap    m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void OwnerDamagedBy(Unit* /* p_Attacker */) override
            {
                m_Events.ScheduleEvent(EVENT_2, 1);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);
                Unit* l_Caster = me->GetOwner();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                case EVENT_1:
                    DoSpellAttackIfReady(eSpells::LightningBolt);
                    m_Events.ScheduleEvent(EVENT_1, 1);
                    break;
                case EVENT_3:
                    me->CastSpell(l_Player, eSpells::HealingSurge);
                    me->CastSpell(l_Player, eSpells::Riptide);
                    break;
                default:
                    break;
                }

                if (me->getClass() != UnitClass::UNIT_CLASS_MAGE)
                    m_Events.ScheduleEvent(EVENT_1, 1);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_farseer_nobundo_AI(creature);
        }
};

/// Mylune - 110056
class npc_mylune : public CreatureScript
{
    public:
        npc_mylune() : CreatureScript("npc_mylune") { }

        struct npc_mylune_AI : public ScriptedAI
        {
            npc_mylune_AI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                SummonFaerieDragons = 219201,
                MylunesMelody       = 219192
            };

            bool m_Casted = false;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void UpdateAI(uint32 const p_Time) override
            {
                UpdateOperations(p_Time);

                if (m_Casted || p_Time == 0)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SummonFaerieDragons);
                if (!l_SpellInfo)
                    return;

                me->CastSpell(me, eSpells::MylunesMelody, false);
                AddTimedDelayedOperation(750, [this]() -> void ///< this is required accessing the enumerator and me
                {
                    SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SummonFaerieDragons);
                    if (!l_SpellInfo)
                        return;

                    for (int32 l_Amount = 0; l_Amount < l_SpellInfo->Effects[EFFECT_0].BasePoints; l_Amount++)
                        me->CastSpell(me, eSpells::SummonFaerieDragons, true);
                });

                me->DespawnOrUnsummon(l_SpellInfo->GetDuration());
                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mylune_AI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Mylune's Melody - 218965
class spell_npc_mylunes_melody : public SpellScriptLoader
{
    public:
        spell_npc_mylunes_melody() : SpellScriptLoader("spell_npc_mylunes_melody") { }

        class spell_npc_mylunes_melody_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_npc_mylunes_melody_AuraScript);

            enum eCreatureModelIds
            {
                SummonedCritterModelId      = 14659
            };

            enum eSpells
            {
                MylunesMelodyCritterSummon  = 218976
            };

            uint8 m_NbTicks = 0;
            std::array<uint64, 7> m_SummonsGUIDs = { {0, 0, 0, 0, 0, 0, 0} };

            std::array<int32, 55> const m_Models =
            { {
                328, 51292, 64651, 51292, 71224, 4626, 6302, 134, 28397, 2177, 27883, 31794, 36644, 36944, 51301,
                54854, 62547, 2957, 8971, 4343, 7511, 36583, 42509, 32789, 16633, 14952, 47992, 47989, 2176, 2177,
                1987, 36584, 901, 38512, 43948, 23311, 1418, 32663, 36591, 42381, 36544, 387, 45902, 40093,
                44537, 43875, 44445, 43876, 34160, 45880, 40089, 42757, 44602, 45999, 46953
            } };

            int32 GetRandomModelId()
            {
                return m_Models[urand(0, m_Models.size() - 1)];
            }

            void ModifyDisplayId(Unit* p_Caster, Unit* p_Summon)
            {
                p_Summon->SetDisplayId(GetRandomModelId());

                uint32& l_OlderSummon = p_Caster->m_SpellHelper.GetUint32(eSpellHelpers::MylunesLastCritter);
                l_OlderSummon++;
                if (l_OlderSummon > 6)
                    l_OlderSummon = 0;

                return;
            }

            void HandleOnEffectPeriodic(AuraEffect const* p_AurEff)
            {
                Unit* l_Caster = GetUnitOwner();
                if (!l_Caster)
                    return;

                uint32& l_OlderSummon = l_Caster->m_SpellHelper.GetUint32(eSpellHelpers::MylunesLastCritter);
                bool l_HasToSummon = false;

                for (uint32 l_CheckSummon = 0; l_CheckSummon <= 6; l_CheckSummon++)
                {
                    Unit* l_Unit = sObjectAccessor->GetUnit(*l_Caster, m_SummonsGUIDs[l_CheckSummon]);
                    if (!l_Unit)
                    {
                        l_OlderSummon = l_CheckSummon;
                        l_HasToSummon = true;
                        break;
                    }
                }

                m_NbTicks++;

                if (m_NbTicks != p_AurEff->GetAmount() * l_OlderSummon + 1)
                    return;
                else
                    m_NbTicks = 0;

                if (!l_HasToSummon)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::MylunesMelodyCritterSummon, true);
                for (Unit* l_Summon : l_Caster->m_Controlled)
                {
                    if (l_Summon && l_Summon->GetDisplayId() == eCreatureModelIds::SummonedCritterModelId)
                    {
                        m_SummonsGUIDs[l_OlderSummon] = l_Summon->GetGUID();
                        ModifyDisplayId(l_Caster, l_Summon);
                    }
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_npc_mylunes_melody_AuraScript::HandleOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_npc_mylunes_melody_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Mylune's Melody Critter - 110002
class npc_mylunes_melody_critter : public CreatureScript
{
    public:
        npc_mylunes_melody_critter() : CreatureScript("npc_mylunes_melody_critter") { }

        struct npc_mylunes_melody_critter_AI : public ScriptedAI
        {
            npc_mylunes_melody_critter_AI(Creature* creature) : ScriptedAI(creature) {}

            std::array<float, 7> const m_Positions = { { 1.57f, 4.71f, 3.14f, 2.355f, 3.925f, 0.785f, 5.495f } };

            enum eSpells
            {
                MylunesMelodyCritterSummon  = 218976,
                MylunesMelodyAura           = 218965
            };

            void Reset() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                uint32& l_OlderSummon = l_Owner->m_SpellHelper.GetUint32(eSpellHelpers::MylunesLastCritter);

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::MylunesMelodyCritterSummon);
                if (!l_SpellInfo)
                    return;

                SpellRadiusEntry const* l_RadiusEntry = l_SpellInfo->Effects[EFFECT_0].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetOrientation(l_Owner->GetOrientation());
                me->m_positionX = l_Owner->m_positionX + l_RadiusEntry->RadiusMax * cosf(m_Positions[l_OlderSummon]);
                me->m_positionY = l_Owner->m_positionY + l_RadiusEntry->RadiusMax * sinf(m_Positions[l_OlderSummon]);
                me->UpdateGroundPositionZ(me->m_positionX, me->m_positionY, me->m_positionZ);
                me->UpdatePosition(me->GetPosition());
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (!l_Owner->HasAura(eSpells::MylunesMelodyAura))
                    me->DespawnOrUnsummon(1);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_mylunes_melody_critter_AI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Mark of Thassarian - 222205
/// Called for Thassarian (npc) - 111591
class spell_thassarian : public SpellScriptLoader
{
    public:
        spell_thassarian() : SpellScriptLoader("spell_thassarian") { }

        class spell_thassarian_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thassarian_SpellScript);

            enum eSpells
            {
                SummonThassarian    = 222219,
                ThassarianTimer     = 222226
            };

            bool m_ShouldProc = false;

            void HandleOnObjectAreaTargetSelect(std::list<WorldObject*>& p_Targets)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (WorldObject* l_Object : p_Targets)
                {
                    Unit* l_Target = l_Object->ToUnit();
                    if (!l_Target)
                        return;

                    Unit* l_Victim = l_Target->getVictim();
                    if (!l_Victim)
                        return;

                    if (l_Victim->GetGUID() == l_Caster->GetGUID())
                    {
                        m_ShouldProc = true;
                        return;
                    }
                }
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();;
                if (!l_Caster || l_Caster->HasAura(eSpells::ThassarianTimer) || !m_ShouldProc)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::SummonThassarian, true);
                l_Caster->CastSpell(l_Caster, eSpells::ThassarianTimer, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_thassarian_SpellScript::HandleOnObjectAreaTargetSelect, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_thassarian_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_thassarian_SpellScript();
        }
};


/// Last Update 7.1.5 Build 23420
/// Thassarian - 111591
class npc_thassarian : public CreatureScript
{
    public:
        npc_thassarian() : CreatureScript("npc_thassarian") { }

        struct npc_thassarian_AI : public ScriptedAI
        {
            npc_thassarian_AI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                ThassarianStun      = 222317,
                MarkOfThassarian    = 222308
            };

            bool m_Casted = false;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void UpdateAI(uint32 const p_Time) override
            {
                if (m_Casted || p_Time == 0)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ThassarianStun);
                if (!l_SpellInfo)
                    return;

                me->CastSpell(me, eSpells::ThassarianStun, false);

                me->DespawnOrUnsummon(l_SpellInfo->GetDuration());
                m_Casted = true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_thassarian_AI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Mark of Thassarian - 222317
class spell_npc_mark_of_thassarian : public SpellScriptLoader
{
    public:
        spell_npc_mark_of_thassarian() : SpellScriptLoader("spell_npc_mark_of_thassarian") { }

        enum eSpells
        {
            MarkOfThassarian = 222308
        };

        class spell_npc_mark_of_thassarian_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_npc_mark_of_thassarian_SpellScript);


            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, eSpells::MarkOfThassarian, true, nullptr, nullptr);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_npc_mark_of_thassarian_SpellScript::HandleOnHit);
            }
        };

        class spell_npc_mark_of_thassarian_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_npc_mark_of_thassarian_AuraScript);

            void HandleOnEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                l_Target->RemoveAura(eSpells::MarkOfThassarian);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_npc_mark_of_thassarian_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_npc_mark_of_thassarian_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_npc_mark_of_thassarian_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Mark of Thassarian (heal) - 222308
class spell_npc_mark_of_thassarian_heal : public SpellScriptLoader
{
    public:
        spell_npc_mark_of_thassarian_heal() : SpellScriptLoader("spell_npc_mark_of_thassarian_heal") { }

        class spell_npc_mark_of_thassarian_heal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_npc_mark_of_thassarian_heal_AuraScript);

            enum eSpells
            {
                MarkOfThassarianHeal = 222313
            };

            void HandleProc(ProcEventInfo& p_EventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return;

                TempSummon* l_Summon = l_Caster->ToTempSummon();
                if (!l_Summon)
                    return;

                Unit* l_Owner = l_Summon->GetOwner();
                if (!l_Owner)
                    return;

                int32 l_Amount = CalculatePct(l_DamageInfo->GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);

                l_Caster->CastCustomSpell(l_Owner, eSpells::MarkOfThassarianHeal, &l_Amount, nullptr, nullptr, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_npc_mark_of_thassarian_heal_AuraScript::HandleProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_npc_mark_of_thassarian_heal_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Dark Stampede 221761
class spell_dark_stampede : public SpellScriptLoader
{
    public:
        spell_dark_stampede() : SpellScriptLoader("spell_dark_stampede") { }

        class spell_dark_stampede_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dark_stampede_SpellScript);

            enum eSpells
            {
                SummonWhitemane         = 221756,
                SummonTrollbane         = 221762,
                SummonDarionMograine    = 221764,
                SummonNazgrim           = 221766
            };

            std::array<uint32, 4> const m_Horsemen =
            {
                {
                    eSpells::SummonWhitemane,
                    eSpells::SummonTrollbane,
                    eSpells::SummonDarionMograine,
                    eSpells::SummonNazgrim
                }
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                for (uint32 l_SpellId : m_Horsemen)
                    l_Caster->CastSpell(l_Caster, l_SpellId, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_dark_stampede_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dark_stampede_SpellScript();
        }
};

/// Four Horsemen - 111337, 111339, 111340, 111341 (Dark Stampede - 221761)
class npc_four_horsemen : public CreatureScript
{
    public:
        npc_four_horsemen() : CreatureScript("npc_four_horsemen") { }

        struct npc_four_horsemen_ScriptedAI : public ScriptedAI
        {
            npc_four_horsemen_ScriptedAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                AcherusDeathcharger = 48778,
                Trample             = 222171
            };

            void Reset() override
            {
                me->CastSpell(me, eSpells::AcherusDeathcharger, true);

                Unit* l_Summoner = me->GetOwner();
                MotionMaster* l_MotionMaster = me->GetMotionMaster();
                if (!l_Summoner || !l_MotionMaster)
                    return;

                me->CastSpell(me, eSpells::Trample, true, nullptr, nullptr, l_Summoner->GetGUID());

                Position l_Dest = *me;

                l_Dest.m_positionX += 25.0f * std::cos(me->GetOrientation());
                l_Dest.m_positionY += 25.0f * std::sin(me->GetOrientation());

                l_MotionMaster->MoveCharge(&l_Dest, 9.7f, EVENT_CHARGE, true);
            }

            void UpdateAI(uint32 const p_Time) override
            {
                me->UpdateGroundPositionZ(me->m_positionX, me->m_positionY, me->m_positionZ);
                me->UpdatePosition(me->GetPosition());
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_four_horsemen_ScriptedAI(creature);
        }
};

#ifndef __clang_analyzer__
void AddSC_follower_spell_scripts()
{
    new spell_npc_that_ones_mine();
    new npc_hemet_nesingwary();
    new npc_archmage_vargoth();
    new spell_npc_garona_halforcen();
    new spell_npc_rottgut_the_abomination();
    new spell_npc_meat_hook_dmg();

    ///Warlock
    new npc_lulubell();
    new npc_calydus();

    new spell_summon_eredar_twin();
    new npc_lady_sacrolash();
    new npc_grand_warlock_alythess();

    ///Druid
    new spell_emerald_charge();
    new npc_keeper_remulos();
    new spell_summon_keeper_remulos();
    new npc_mylune();
    new spell_npc_mylunes_melody();
    new npc_mylunes_melody_critter();

    new spell_nightmarish_visions();

    ///Priest
    new npc_alonsus_faol();
    new npc_high_priestess_ishanah();

    ///Warrior
    new npc_hodir();
    new spell_finna_personal_phalanx();
    new npc_finna_bjornsdottir();
    new spell_retaliation_finna_bjornsdottir();
    new npc_thorim();
    new spell_thorim_colossal_might();

    /// Rogue
    new npc_mathias_shaw();
    new spell_si7_intel();
    new spell_si7_intel_remove();
    new spell_seal_of_ravenholdt();
    new npc_lord_jorach_ravenholdt();

    /// Paladin
    new spell_blood_vanguard();
    new npc_lady_liadrin();
    new npc_blood_knight();
    new spell_trample();
    new spell_judgments_gaze();
    new npc_lothraxion();

    /// Monk
    new spell_stealth_attack();
    new npc_taran_zhu();

    /// Shaman
    new npc_farseer_nobundo();

    ///DK
    new spell_thassarian();
    new npc_thassarian();
    new spell_npc_mark_of_thassarian();
    new spell_npc_mark_of_thassarian_heal();
    new spell_dark_stampede();
    new npc_four_horsemen();
}
#endif
