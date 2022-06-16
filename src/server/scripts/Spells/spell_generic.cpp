////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*
 * Scripts for spells with SPELLFAMILY_GENERIC which cannot be included in AI script file
 * of creature using it or can't be bound to any player class.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_gen_"
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SkillDiscovery.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "InstanceScript.h"
#include "Group.h"
#include "LFGMgr.h"
#include "Player.h"
#include "GameEventMgr.h"
#include "DB2Stores.h"
#include "Formulas.h"
#include "spell_generic.hpp"
#include "HelperDefines.h"
#include "Chat.h"

class spell_gen_mercenary_mode : public SpellScriptLoader
{
    public:
        spell_gen_mercenary_mode() : SpellScriptLoader("spell_gen_mercenary_mode") { }

        class spell_gen_mercenary_mode_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_mercenary_mode_AuraScript);

            uint32 GetRaceForClass(TeamId p_TeamID, uint8 p_Class)
            {
                if (p_TeamID == TeamId::TEAM_ALLIANCE)
                {
                    switch (p_Class)
                    {
                        case CLASS_WARRIOR:
                        case CLASS_SHAMAN:
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                        case CLASS_MONK:
                        case CLASS_DEATH_KNIGHT:
                            return RACE_ORC;
                        case CLASS_PALADIN:
                        case CLASS_DRUID:
                            return RACE_TAUREN;
                        case CLASS_PRIEST:
                            return RACE_UNDEAD_PLAYER;
                        case CLASS_DEMON_HUNTER:
                            return RACE_BLOODELF;
                        default:
                            break;
                    }
                }
                else if (p_TeamID == TeamId::TEAM_HORDE)
                {
                    switch (p_Class)
                    {
                        case CLASS_WARRIOR:
                        case CLASS_PALADIN:
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                        case CLASS_PRIEST:
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                        case CLASS_MONK:
                        case CLASS_DEATH_KNIGHT:
                            return RACE_HUMAN;
                        case CLASS_SHAMAN:
                            return RACE_DRAENEI;
                        case CLASS_DRUID:
                        case CLASS_DEMON_HUNTER:
                            return RACE_NIGHTELF;
                        default:
                            break;
                    }
                }

                return CLASS_NONE;
            }

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                uint32 l_NewRace = GetRaceForClass(l_Player->GetTeamId(), l_Player->getClass());
                l_Player->ActivateMercenary(l_NewRace);
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (l_Player == nullptr)
                    return;

                l_Player->DeactivateMercenary();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_mercenary_mode_AuraScript::HandleAfterEffectApply, EFFECT_1, SPELL_AURA_COMPREHEND_LANGUAGE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_mercenary_mode_AuraScript::HandleAfterEffectRemove, EFFECT_1, SPELL_AURA_COMPREHEND_LANGUAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_mercenary_mode_AuraScript();
        }
};

class playerscript_mercenary_mode : public PlayerScript
{
    public:
        playerscript_mercenary_mode() : PlayerScript("playerscript_mercenary_mode") { }

        void OnLogin(Player* p_Player) override
        {
            if (!p_Player)
                return;

            if (p_Player->IsInBattleground())
            {
                if (p_Player->HasAura(MercenarySpells::MercenaryAlliance1))
                {
                    /// Remove Mercenary Mode
                    p_Player->RemoveAura(MercenarySpells::MercenaryAlliance1);
                    p_Player->RemoveAura(MercenarySpells::MercenaryAlliance2);

                    /// Add Back as the Player Loading overrides the Faction of the Mercenary
                    p_Player->AddAura(MercenarySpells::MercenaryAlliance1, p_Player);
                    p_Player->AddAura(MercenarySpells::MercenaryAlliance2, p_Player);
                    return;
                }

                if (p_Player->HasAura(MercenarySpells::MercenaryHorde1))
                {
                    /// Remove Mercenary Mode
                    p_Player->RemoveAura(MercenarySpells::MercenaryHorde1);
                    p_Player->RemoveAura(MercenarySpells::MercenaryHorde2);

                    /// Add Back as the Player Loading overrides the Faction of the Mercenary
                    p_Player->AddAura(MercenarySpells::MercenaryHorde1, p_Player);
                    p_Player->AddAura(MercenarySpells::MercenaryHorde2, p_Player);
                    return;
                }
            }
        }
};

class spell_gen_class_hall_restriction : public SpellScriptLoader
{
    public:
        spell_gen_class_hall_restriction() : SpellScriptLoader("spell_gen_class_hall_reestriction") { }

        class spell_gen_class_hall_restriction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_class_hall_restriction_AuraScript);

            enum eSpells
            {
                SpellPaladin     = 195558,
                SpellDeathKnight = 232719,
                SpellHunter      = 208643,
                SpellDruid       = 203810
            };

            enum eAreas
            {
                Druid           = 7846,
                Druid2          = 8076,
                Hunter          = 7877,
                DeathKnight     = 7679,
                Paladin         = 7638
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                // HACK: only paladin's aura stuns the target
                // Don't let the target to move away
                if (m_scriptSpellId != eSpells::SpellPaladin)
                {
                    GetAura()->SetMaxDuration(1000);
                    GetAura()->SetDuration(1000);
                }
            }

            void HandleAfterEffectRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (l_Target == nullptr)
                    return;

                uint32 l_AreaID = l_Target->GetAreaId();
                WorldLocation l_TeleportLoc;

                if (Player* l_Player = l_Target->ToPlayer())
                {
                    if (!l_AreaID)
                        return;

                    switch (l_AreaID)
                    {
                        case eAreas::Paladin:
                            l_TeleportLoc.m_mapId = 0;
                            l_TeleportLoc.Relocate(2280.233f, -5318.091f, 88.539f, 5.523f);
                            break;
                        case eAreas::Druid:
                        case eAreas::Druid2:
                            l_TeleportLoc.m_mapId = 1220;
                            l_TeleportLoc.Relocate(3659.868f, 7070.324f, 28.591f, 1.234f);
                            break;
                        case eAreas::Hunter:
                            l_TeleportLoc.m_mapId = 1220;
                            l_TeleportLoc.Relocate(4953.824f, 5081.077f, 784.922f, 4.441f);
                            break;
                        case eAreas::DeathKnight:
                            l_TeleportLoc.m_mapId = 1220;
                            l_TeleportLoc.Relocate(-836.156f, 4278.500f, 746.254f, 4.373f);
                            break;
                        default:
                            l_TeleportLoc.m_mapId = l_Player->m_homebindMapId;
                            l_TeleportLoc.Relocate(l_Player->m_homebindX, l_Player->m_homebindY, l_Player->m_homebindZ, 0.0f);
                            break;
                    }

                    l_Player->AddDelayedEvent([l_Player, l_TeleportLoc]() -> void
                    {
                        l_Player->TeleportTo(l_TeleportLoc.GetMapId(), l_TeleportLoc.GetPositionX(), l_TeleportLoc.GetPositionY(), l_TeleportLoc.GetPositionZ(), l_TeleportLoc.GetOrientation());
                    }, 1);
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::SpellDeathKnight)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_class_hall_restriction_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_gen_class_hall_restriction_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                }
                else if (m_scriptSpellId == eSpells::SpellPaladin)
                {
                    AfterEffectRemove += AuraEffectRemoveFn(spell_gen_class_hall_restriction_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
                }
                else if (m_scriptSpellId == eSpells::SpellHunter)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_class_hall_restriction_AuraScript::HandleAfterEffectApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_gen_class_hall_restriction_AuraScript::HandleAfterEffectRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                }
                else if (m_scriptSpellId == eSpells::SpellDruid)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_class_hall_restriction_AuraScript::HandleAfterEffectApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_gen_class_hall_restriction_AuraScript::HandleAfterEffectRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_class_hall_restriction_AuraScript();
        }
};

class spell_gen_absorb0_hitlimit1: public SpellScriptLoader
{
    public:
        spell_gen_absorb0_hitlimit1() : SpellScriptLoader("spell_gen_absorb0_hitlimit1") { }

        class spell_gen_absorb0_hitlimit1_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_absorb0_hitlimit1_AuraScript);

            uint32 limit;

            bool Load()
            {
                // Max absorb stored in 1 dummy effect
                limit = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                return true;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
            {
                absorbAmount = std::min(limit, absorbAmount);
            }

            void Register()
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_absorb0_hitlimit1_AuraScript::Absorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_absorb0_hitlimit1_AuraScript();
        }
};

/// Called by 256896 Light's Reckoning
/// Not in Unit::GetDiminishingPVPDamage because this spell have SPELL_ATTR6_NO_DONE_PCT_DAMAGE_MODS attr. PvP mod is exeption
class spell_gen_light_reckoning : public SpellScriptLoader
{
    public:
        spell_gen_light_reckoning() : SpellScriptLoader("spell_gen_light_reckoning") { }

        class spell_gen_light_reckoning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_light_reckoning_SpellScript);

            void HandleDamage(SpellEffIndex p_EffIndex)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                // Light's Reckoning deals 40% damage in PvP
                if (l_Caster->ToPlayer() && l_Target->ToPlayer())
                {
                    uint32 l_damage = GetHitDamage();
                    AddPct(l_damage, -60.0f);
                    SetHitDamage(l_damage);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_light_reckoning_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_light_reckoning_SpellScript();
        }
};

/// should not break CC on damage or proc on CC targets
/// Called by
/// 242525 Terror From Below (trinket),
/// 255629 Prototype Personnel Decimator (trinket),
/// 215407 Dark Blast (trinket),
/// 213786 Nightfall (trinket),
/// 214052 Fel Meteor (trinket),
/// 230261 Flame Wreath (trinket),
/// 250834 Void Tendril (trinket),
/// 215404 Rancid Maw (trinket),
/// 230242 VolatileEnergy (trinket),
/// 253263 Shadow Blades (trinket)
class spell_crowd_control_ignore : public SpellScriptLoader
{
    public:
        spell_crowd_control_ignore() : SpellScriptLoader("spell_crowd_control_ignore") { }

        enum eSpells
        {
            TerrorFromBelow = 242525,
            PrototypePersonnelDecimator = 255629,
            DarkBlast = 215407,
            Nightfall = 213786,
            FelMeteor = 214052,
            FlameWreath = 230261,
            VoidTendril = 250834,
            VolatileEnergy = 230242,
            RancidMaw = 215404,
            ShadowBlades = 253263
        };

        class spell_crowd_control_ignore_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_crowd_control_ignore_SpellScript);

            SpellCastResult HandleOnCheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!l_Target->HasBreakableByDamageCrowdControlAura())
                    return SPELL_CAST_OK;

                return SPELL_FAILED_BAD_TARGETS;
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                p_Targets.remove_if([this, l_Caster](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->ToUnit() == nullptr)
                        return true;

                    if (p_Object->ToUnit()->HasBreakableByDamageCrowdControlAura())
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::FelMeteor:
                    case eSpells::VoidTendril:
                    case eSpells::Nightfall:
                    case eSpells::FlameWreath:
                    case eSpells::TerrorFromBelow:
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_crowd_control_ignore_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                        break;
                    case eSpells::PrototypePersonnelDecimator:
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_crowd_control_ignore_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                        break;
                    case eSpells::DarkBlast:
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_crowd_control_ignore_SpellScript::FilterTargets, EFFECT_0, TARGET_ENNEMIES_IN_CYLINDER);
                        break;
                    case eSpells::VolatileEnergy:
                        OnCheckCast += SpellCheckCastFn(spell_crowd_control_ignore_SpellScript::HandleOnCheckCast);
                        break;
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_crowd_control_ignore_SpellScript();
        }

        class spell_crowd_control_ignore_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_crowd_control_ignore_AuraScript);

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_ProcEventInfo.GetActionTarget();
                if (!l_Caster || !l_Target)
                    return false;

                if (l_Target->HasBreakableByDamageCrowdControlAura())
                    return false;

                return true;
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::RancidMaw:
                    case eSpells::ShadowBlades:
                        DoCheckProc += AuraCheckProcFn(spell_crowd_control_ignore_AuraScript::HandleCheckProc);
                        break;
                    default:
                        break;
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_crowd_control_ignore_AuraScript();
        }
};


// 41337 Aura of Anger
class spell_gen_aura_of_anger: public SpellScriptLoader
{
    public:
        spell_gen_aura_of_anger() : SpellScriptLoader("spell_gen_aura_of_anger") { }

        class spell_gen_aura_of_anger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_aura_of_anger_AuraScript);

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (AuraEffect* aurEff1 = aurEff->GetBase()->GetEffect(EFFECT_1))
                    aurEff1->ChangeAmount(aurEff1->GetAmount() + 5);
                aurEff->SetAmount(100 * aurEff->GetTickNumber());
            }

            void Register()
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_aura_of_anger_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_aura_of_anger_AuraScript();
        }
};

class spell_gen_av_drekthar_presence: public SpellScriptLoader
{
    public:
        spell_gen_av_drekthar_presence() : SpellScriptLoader("spell_gen_av_drekthar_presence") { }

        class spell_gen_av_drekthar_presence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_av_drekthar_presence_AuraScript);

            bool CheckAreaTarget(Unit* target)
            {
                switch (target->GetEntry())
                {
                    // alliance
                    case 14762: // Dun Baldar North Marshal
                    case 14763: // Dun Baldar South Marshal
                    case 14764: // Icewing Marshal
                    case 14765: // Stonehearth Marshal
                    case 11948: // Vandar Stormspike
                    // horde
                    case 14772: // East Frostwolf Warmaster
                    case 14776: // Tower Point Warmaster
                    case 14773: // Iceblood Warmaster
                    case 14777: // West Frostwolf Warmaster
                    case 11946: // Drek'thar
                        return true;
                    default:
                        return false;
                        break;
                }
            }
            void Register()
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_gen_av_drekthar_presence_AuraScript::CheckAreaTarget);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_av_drekthar_presence_AuraScript();
        }
};

// 46394 Brutallus Burn
class spell_gen_burn_brutallus: public SpellScriptLoader
{
    public:
        spell_gen_burn_brutallus() : SpellScriptLoader("spell_gen_burn_brutallus") { }

        class spell_gen_burn_brutallus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_burn_brutallus_AuraScript);

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (aurEff->GetTickNumber() % 11 == 0)
                    aurEff->SetAmount(aurEff->GetAmount() * 2);
            }

            void Register()
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_burn_brutallus_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_burn_brutallus_AuraScript();
        }
};

enum CannibalizeSpells
{
    SPELL_CANNIBALIZE_TRIGGERED = 20578
};

class spell_gen_cannibalize: public SpellScriptLoader
{
    public:
        spell_gen_cannibalize() : SpellScriptLoader("spell_gen_cannibalize") { }

        class spell_gen_cannibalize_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cannibalize_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CANNIBALIZE_TRIGGERED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfCorpseNear()
            {
                Unit* caster = GetCaster();
                float max_range = GetSpellInfo()->GetMaxRange(false);
                WorldObject* result = NULL;
                // search for nearby enemy corpse in range
                JadeCore::AnyDeadUnitSpellTargetInRangeCheck check(caster, max_range, GetSpellInfo(), TARGET_CHECK_ENEMY);
                JadeCore::WorldObjectSearcher<JadeCore::AnyDeadUnitSpellTargetInRangeCheck> searcher(caster, result, check);
                caster->GetMap()->VisitFirstFound(caster->m_positionX, caster->m_positionY, max_range, searcher);
                if (!result)
                    return SPELL_FAILED_NO_EDIBLE_CORPSES;
                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, SPELL_CANNIBALIZE_TRIGGERED, false);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cannibalize_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_gen_cannibalize_SpellScript::CheckIfCorpseNear);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cannibalize_SpellScript();
        }
};

// 45472 Parachute
enum ParachuteSpells
{
    SPELL_PARACHUTE         = 45472,
    SPELL_PARACHUTE_BUFF    = 44795
};

class spell_gen_parachute: public SpellScriptLoader
{
    public:
        spell_gen_parachute() : SpellScriptLoader("spell_gen_parachute") { }

        class spell_gen_parachute_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PARACHUTE) || !sSpellMgr->GetSpellInfo(SPELL_PARACHUTE_BUFF))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Player* target = GetTarget()->ToPlayer())
                    if (target->IsFalling())
                    {
                        target->RemoveAurasDueToSpell(SPELL_PARACHUTE);
                        target->CastSpell(target, SPELL_PARACHUTE_BUFF, true);
                    }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_parachute_AuraScript();
        }
};

class spell_gen_pet_summoned: public SpellScriptLoader
{
    public:
        spell_gen_pet_summoned() : SpellScriptLoader("spell_gen_pet_summoned") { }

        class spell_gen_pet_summoned_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pet_summoned_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* player = GetCaster()->ToPlayer();
                if (player->GetLastPetNumber())
                {
                    PetType newPetType = (player->getClass() == CLASS_HUNTER) ? HUNTER_PET : SUMMON_PET;

                    if (Pet* l_NewPet = new Pet(player, newPetType))
                    {
                        uint64 l_PlayerGUID = player->GetGUID();
                        uint32 l_PetNumber  = player->GetLastPetNumber();
#ifdef CROSS
                        uint32 l_RealmID    = player->GetSession()->GetInterRealmNumber();
#else
                        uint32 l_RealmID    = g_RealmID;
#endif

                        PreparedStatement* l_PetStatement = PetQueryHolder::GenerateFirstLoadStatement(0, player->GetLastPetNumber(), player->GetRealGUIDLow(), true, PET_SLOT_UNK_SLOT, l_RealmID);
                        CharacterDatabase.AsyncQuery(l_PetStatement, player->GetQueryCallbackMgr(), [l_NewPet, l_PlayerGUID, l_PetNumber, l_RealmID](PreparedQueryResult p_Result) -> void
                        {
                            Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGUID);

                            if (!l_Player || !p_Result)
                            {
                                delete l_NewPet;
                                return;
                            }

                            PetQueryHolder* l_PetHolder = new PetQueryHolder(p_Result->Fetch()[0].GetUInt32(), l_RealmID, p_Result);
                            l_PetHolder->Initialize();

                            auto l_QueryHolderResultFuture = CharacterDatabase.DelayQueryHolder(l_PetHolder);

                            l_Player->GetQueryCallbackMgr()->AddQueryHolderCallback(QueryHolderCallback(l_QueryHolderResultFuture, [l_NewPet, l_PlayerGUID, l_PetNumber](SQLQueryHolder* p_QueryHolder) -> void
                            {
                                Player* l_Player = sObjectAccessor->FindPlayer(l_PlayerGUID);
                                if (!l_Player || !p_QueryHolder)
                                {
                                    delete l_NewPet;
                                    return;
                                }

                                l_NewPet->LoadPetFromDB(l_Player, 0, l_PetNumber, true, PET_SLOT_UNK_SLOT, false, (PetQueryHolder*)p_QueryHolder, [](Pet* p_Pet, bool p_Result) -> void
                                {
                                    if (!p_Result)
                                    {
                                        delete p_Pet;
                                        return;
                                    }

                                    // revive the pet if it is dead
                                    if (p_Pet->getDeathState() == DEAD || p_Pet->getDeathState() == CORPSE)
                                        p_Pet->setDeathState(ALIVE);

                                    p_Pet->ClearUnitState(uint32(UNIT_STATE_ALL_STATE));
                                    p_Pet->SetFullHealth();
                                    p_Pet->SetPower(p_Pet->getPowerType(), p_Pet->GetMaxPower(p_Pet->getPowerType()));

                                    switch (p_Pet->GetEntry())
                                    {
                                        case WarlockPet::DoomGuard:
                                        case WarlockPet::Infernal:
                                        case WarlockPet::Abyssal:
                                            p_Pet->SetEntry(WarlockPet::Imp);
                                            break;
                                        default:
                                            break;
                                    }
                                });
                            }));
                        });
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_pet_summoned_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_pet_summoned_SpellScript();
        }
};

class spell_gen_remove_flight_auras: public SpellScriptLoader
{
    public:
        spell_gen_remove_flight_auras() : SpellScriptLoader("spell_gen_remove_flight_auras") {}

        class spell_gen_remove_flight_auras_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_remove_flight_auras_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->RemoveAurasByType(SPELL_AURA_FLY);
                    target->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_remove_flight_auras_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_remove_flight_auras_SpellScript();
        }
};

// 66118 Leeching Swarm
enum LeechingSwarmSpells
{
    SPELL_LEECHING_SWARM_DMG    = 66240,
    SPELL_LEECHING_SWARM_HEAL   = 66125
};

class spell_gen_leeching_swarm: public SpellScriptLoader
{
    public:
        spell_gen_leeching_swarm() : SpellScriptLoader("spell_gen_leeching_swarm") { }

        class spell_gen_leeching_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_leeching_swarm_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LEECHING_SWARM_DMG) || !sSpellMgr->GetSpellInfo(SPELL_LEECHING_SWARM_HEAL))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                bool l_IsValid = true;
                if (caster->GetMapId() != 649)
                {
                    caster->RemoveAura(GetSpellInfo()->Id);
                    caster->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                    l_IsValid = false;
                    Remove();
                    SetDuration(0);
                }

                if (Unit* target = GetTarget())
                {
                    if (!l_IsValid)
                    {
                        target->RemoveAura(GetSpellInfo()->Id);
                        target->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                        return;
                    }

                    int32 lifeLeeched = target->CountPctFromCurHealth(aurEff->GetAmount());
                    if (lifeLeeched < 250)
                        lifeLeeched = 250;
                    // Damage
                    caster->CastCustomSpell(target, SPELL_LEECHING_SWARM_DMG, &lifeLeeched, 0, 0, false);
                    // Heal
                    caster->CastCustomSpell(caster, SPELL_LEECHING_SWARM_HEAL, &lifeLeeched, 0, 0, false);
                }
            }

            void CheckSpell(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& /*amplitude*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Owner = GetUnitOwner();

                Unit* l_TabUnit[2] = { l_Caster, l_Owner };

                for (uint8 l_Idx = 0; l_Idx < 2; ++l_Idx)
                {
                    if (l_TabUnit[l_Idx])
                    {
                        if (l_TabUnit[l_Idx]->GetMapId() != 649)
                        {
                            isPeriodic = false;
                            Remove();
                            break;
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_leeching_swarm_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_gen_leeching_swarm_AuraScript::CheckSpell, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_leeching_swarm_AuraScript();
        }

        class spell_gen_leeching_swarm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_leeching_swarm_SpellScript);

            SpellCastResult CheckMap()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMapId() != 649)
                        return SPELL_FAILED_INCORRECT_AREA;
                    else
                        return SPELL_CAST_OK;
                }
                else
                    return SPELL_FAILED_CASTER_DEAD;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_leeching_swarm_SpellScript::CheckMap);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_leeching_swarm_SpellScript();
        }
};

enum EluneCandle
{
    NPC_OMEN = 15467,

    SPELL_ELUNE_CANDLE_OMEN_HEAD   = 26622,
    SPELL_ELUNE_CANDLE_OMEN_CHEST  = 26624,
    SPELL_ELUNE_CANDLE_OMEN_HAND_R = 26625,
    SPELL_ELUNE_CANDLE_OMEN_HAND_L = 26649,
    SPELL_ELUNE_CANDLE_NORMAL      = 26636
};

class spell_gen_elune_candle: public SpellScriptLoader
{
    public:
        spell_gen_elune_candle() : SpellScriptLoader("spell_gen_elune_candle") {}

        class spell_gen_elune_candle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_elune_candle_SpellScript);
            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HEAD))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_CHEST))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_R))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_L))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_NORMAL))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId = 0;

                if (GetHitUnit()->GetEntry() == NPC_OMEN)
                {
                    switch (urand(0, 3))
                    {
                        case 0: spellId = SPELL_ELUNE_CANDLE_OMEN_HEAD; break;
                        case 1: spellId = SPELL_ELUNE_CANDLE_OMEN_CHEST; break;
                        case 2: spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_R; break;
                        case 3: spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_L; break;
                    }
                }
                else
                    spellId = SPELL_ELUNE_CANDLE_NORMAL;

                GetCaster()->CastSpell(GetHitUnit(), spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_elune_candle_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_elune_candle_SpellScript();
        }
};

class spell_creature_permanent_feign_death: public SpellScriptLoader
{
    public:
        spell_creature_permanent_feign_death() : SpellScriptLoader("spell_creature_permanent_feign_death") { }

        class spell_creature_permanent_feign_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_creature_permanent_feign_death_AuraScript);

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                if (target->GetTypeId() == TYPEID_UNIT)
                    target->ToCreature()->SetReactState(REACT_PASSIVE);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_creature_permanent_feign_death_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_creature_permanent_feign_death_AuraScript();
        }
};

enum PvPTrinketTriggeredSpells
{
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER         = 72752,
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF    = 72757
};

class spell_pvp_trinket_wotf_shared_cd: public SpellScriptLoader
{
    public:
        spell_pvp_trinket_wotf_shared_cd() : SpellScriptLoader("spell_pvp_trinket_wotf_shared_cd") {}

        class spell_pvp_trinket_wotf_shared_cd_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pvp_trinket_wotf_shared_cd_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER) || !sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF))
                    return false;
                return true;
            }

            void HandleScript()
            {
                // This is only needed because spells cast from spell_linked_spell are triggered by default
                // Spell::SendSpellCooldown() skips all spells with TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD
                GetCaster()->ToPlayer()->AddSpellAndCategoryCooldowns(GetSpellInfo(), GetCastItem() ? GetCastItem()->GetEntry() : 0, GetSpell());
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_pvp_trinket_wotf_shared_cd_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pvp_trinket_wotf_shared_cd_SpellScript();
        }
};

enum AnimalBloodPoolSpell
{
    SPELL_ANIMAL_BLOOD      = 46221,
    SPELL_SPAWN_BLOOD_POOL  = 63471
};

class spell_gen_animal_blood: public SpellScriptLoader
{
    public:
        spell_gen_animal_blood() : SpellScriptLoader("spell_gen_animal_blood") { }

        class spell_gen_animal_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_animal_blood_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SPAWN_BLOOD_POOL))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // Remove all auras with spell id 46221, except the one currently being applied
                while (Aura* aur = GetUnitOwner()->GetOwnedAura(SPELL_ANIMAL_BLOOD, 0, 0, 0, GetAura()))
                    GetUnitOwner()->RemoveOwnedAura(aur);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetUnitOwner())
                    if (owner->IsInWater())
                        owner->CastSpell(owner, SPELL_SPAWN_BLOOD_POOL, true);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_animal_blood_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_animal_blood_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_animal_blood_AuraScript();
        }
};

enum DivineStormSpell
{
    SPELL_DIVINE_STORM  = 53385
};

// 70769 Divine Storm!
class spell_gen_divine_storm_cd_reset: public SpellScriptLoader
{
    public:
        spell_gen_divine_storm_cd_reset() : SpellScriptLoader("spell_gen_divine_storm_cd_reset") {}

        class spell_gen_divine_storm_cd_reset_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_divine_storm_cd_reset_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DIVINE_STORM))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (caster->HasSpellCooldown(SPELL_DIVINE_STORM))
                    caster->RemoveSpellCooldown(SPELL_DIVINE_STORM, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_divine_storm_cd_reset_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_divine_storm_cd_reset_SpellScript();
        }
};

class spell_gen_gunship_portal: public SpellScriptLoader
{
    public:
        spell_gen_gunship_portal() : SpellScriptLoader("spell_gen_gunship_portal") { }

        class spell_gen_gunship_portal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gunship_portal_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Battleground* bg = caster->GetBattleground())
                    if (bg->GetTypeID(true) == BATTLEGROUND_IC)
                        bg->DoAction(1, caster->GetGUID());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gunship_portal_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gunship_portal_SpellScript();
        }
};

enum parachuteIC
{
    SPELL_PARACHUTE_IC = 66657
};

class spell_gen_parachute_ic: public SpellScriptLoader
{
    public:
        spell_gen_parachute_ic() : SpellScriptLoader("spell_gen_parachute_ic") { }

        class spell_gen_parachute_ic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_ic_AuraScript);

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (Player* target = GetTarget()->ToPlayer())
                    if (target->m_movementInfo.fallTime > 2000)
                        target->CastSpell(target, SPELL_PARACHUTE_IC, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_ic_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_parachute_ic_AuraScript();
        }
};

class spell_gen_dungeon_credit: public SpellScriptLoader
{
    public:
        spell_gen_dungeon_credit() : SpellScriptLoader("spell_gen_dungeon_credit") { }

        class spell_gen_dungeon_credit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dungeon_credit_SpellScript);

            bool Load()
            {
                _handled = false;
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void CreditEncounter()
            {
                // This hook is executed for every target, make sure we only credit instance once
                if (_handled)
                    return;

                _handled = true;
                Unit* caster = GetCaster();
                if (InstanceScript* instance = caster->GetInstanceScript())
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, GetSpellInfo()->Id, caster);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_gen_dungeon_credit_SpellScript::CreditEncounter);
            }

            bool _handled;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dungeon_credit_SpellScript();
        }
};

class spell_gen_profession_research: public SpellScriptLoader
{
    public:
        spell_gen_profession_research() : SpellScriptLoader("spell_gen_profession_research") {}

        class spell_gen_profession_research_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_profession_research_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            SpellCastResult CheckRequirement()
            {
                if (HasDiscoveredAllSpells(GetSpellInfo()->Id, GetCaster()->ToPlayer()))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NOTHING_TO_DISCOVER);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_profession_research_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_profession_research_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by 'Throw Back' 221474: Generic Fishing spell that gives AP to the Fishing Rod Artifact 'Underlight Angler' (item ID: 133755)
class spell_gen_fishing_throw_back: public SpellScriptLoader
{
    public:
        spell_gen_fishing_throw_back() : SpellScriptLoader("spell_gen_fishing_throw_back") {}

        class spell_gen_fishing_throw_back_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_fishing_throw_back_SpellScript);

            SpellCastResult CheckArtifactEquipped()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!l_Player->GetCurrentlyEquippedArtifact() || (l_Player->GetCurrentlyEquippedArtifact()->GetItemTemplate() && l_Player->GetCurrentlyEquippedArtifact()->GetItemTemplate()->ItemId != 133755))
                    return SPELL_FAILED_NO_ARTIFACT_EQUIPPED;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_fishing_throw_back_SpellScript::CheckArtifactEquipped);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_fishing_throw_back_SpellScript();
        }
};

class spell_generic_clone: public SpellScriptLoader
{
    public:
        spell_generic_clone() : SpellScriptLoader("spell_generic_clone") { }

        class spell_generic_clone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_generic_clone_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* l_Caster = GetCaster();

                if (Unit* l_Target = GetHitUnit())
                    l_Caster->CastSpell(l_Target,  uint32(GetEffectValue()), true);
            }

            void Register()
            {
                switch (m_scriptSpellId)
                {
                case 106935: /// Figment of Doubt
                case 136757:
                    OnEffectHitTarget += SpellEffectFn(spell_generic_clone_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
                    break;
                case 126240:
                    OnEffectHitTarget += SpellEffectFn(spell_generic_clone_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
                    break;
                case 57528:
                    OnEffectHitTarget += SpellEffectFn(spell_generic_clone_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
                    OnEffectHitTarget += SpellEffectFn(spell_generic_clone_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_DUMMY);
                    break;
                default:
                    OnEffectHitTarget += SpellEffectFn(spell_generic_clone_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
                    OnEffectHitTarget += SpellEffectFn(spell_generic_clone_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
                    break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_generic_clone_SpellScript();
        }
};

enum CloneWeaponSpells
{
    SPELL_COPY_WEAPON_AURA       = 41055,
    SPELL_COPY_WEAPON_2_AURA     = 63416,
    SPELL_COPY_WEAPON_3_AURA     = 69891,

    SPELL_COPY_OFFHAND_AURA      = 45206,
    SPELL_COPY_OFFHAND_2_AURA    = 69892,

    SPELL_COPY_RANGED_AURA       = 57594
};

class spell_generic_clone_weapon: public SpellScriptLoader
{
    public:
        spell_generic_clone_weapon() : SpellScriptLoader("spell_generic_clone_weapon") { }

        class spell_generic_clone_weapon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_generic_clone_weapon_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* l_Caster = GetCaster();

                if (Unit* l_Target = GetHitUnit())
                    l_Caster->CastSpell(l_Target,  uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_generic_clone_weapon_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_generic_clone_weapon_SpellScript();
        }
};

class spell_gen_clone_weapon_aura: public SpellScriptLoader
{
    public:
        spell_gen_clone_weapon_aura() : SpellScriptLoader("spell_gen_clone_weapon_aura") { }

            class spell_gen_clone_weapon_auraScript : public AuraScript
            {
                PrepareAuraScript(spell_gen_clone_weapon_auraScript);

                uint32 prevItem;

                bool Validate(SpellInfo const* /*spellEntry*/)
                {
                    if (!sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_AURA) ||
                        !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_2_AURA) ||
                        !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_3_AURA) ||
                        !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_AURA) ||
                        !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_2_AURA) ||
                        !sSpellMgr->GetSpellInfo(SPELL_COPY_RANGED_AURA))
                        return false;
                    return true;
                }

                void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
                {
                    Unit* caster = GetCaster();
                    Unit* target = GetTarget();

                    if (!caster)
                        return;

                    switch (GetSpellInfo()->Id)
                    {
                        case SPELL_COPY_WEAPON_AURA:
                        case SPELL_COPY_WEAPON_2_AURA:
                        case SPELL_COPY_WEAPON_3_AURA:
                        {
                            prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS);

                            if (Player* player = caster->ToPlayer())
                            {
                                if (Item* mainItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                                    target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS, mainItem->GetEntry());
                            }
                            else
                                target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS));
                            break;
                        case SPELL_COPY_OFFHAND_AURA:
                        case SPELL_COPY_OFFHAND_2_AURA:
                        {
                            prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2);

                            if (Player* player = caster->ToPlayer())
                            {
                                if (Item* offItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                                    target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2, offItem->GetEntry());
                            }
                            else
                                target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2));
                            break;
                        }
                        case SPELL_COPY_RANGED_AURA:
                        {
                            prevItem = target->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 4);

                            if (Player* player = caster->ToPlayer())
                            {
                                if (Item* rangedItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
                                    target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 4, rangedItem->GetEntry());
                            }
                            else
                                target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 4, caster->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 4));
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_COPY_WEAPON_AURA:
                    case SPELL_COPY_WEAPON_2_AURA:
                    case SPELL_COPY_WEAPON_3_AURA:
                       target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS, prevItem);
                       break;
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                        target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 2, prevItem);
                        break;
                    case SPELL_COPY_RANGED_AURA:
                        target->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS + 4, prevItem);
                        break;
                    default:
                        break;
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_clone_weapon_auraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_clone_weapon_auraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_clone_weapon_auraScript();
        }
};

enum FriendOrFowl
{
    SPELL_TURKEY_VENGEANCE  = 25285
};

class spell_gen_turkey_marker : public SpellScriptLoader
{
public:
    spell_gen_turkey_marker() : SpellScriptLoader("spell_gen_turkey_marker") { }

    class spell_gen_turkey_marker_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_turkey_marker_AuraScript);

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            // store stack apply times, so we can pop them while they expire
            _applyTimes.push_back(getMSTime());
        }

        void OnStackChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();

            if (!target)
                return;

            // on stack 15 cast the achievement crediting spell
            if (GetStackAmount() == 15)
                target->CastSpell(target, SPELL_TURKEY_VENGEANCE, true);
        }

        void OnPeriodic(AuraEffect const* /*aurEff*/)
        {
            if (_applyTimes.empty())
                return;

            // pop stack if it expired for us
            if (_applyTimes.front() + GetMaxDuration() < getMSTime())
                ModStackAmount(-1, AURA_REMOVE_BY_EXPIRE);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_gen_turkey_marker_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectApply += AuraEffectApplyFn(spell_gen_turkey_marker_AuraScript::OnStackChange, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_turkey_marker_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }

        std::list<uint32> _applyTimes;
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_turkey_marker_AuraScript();
    }
};

enum MagicRoosterSpells
{
    SPELL_MAGIC_ROOSTER_NORMAL          = 66122,
    SPELL_MAGIC_ROOSTER_DRAENEI_MALE    = 66123,
    SPELL_MAGIC_ROOSTER_TAUREN_MALE     = 66124
};

class spell_gen_magic_rooster: public SpellScriptLoader
{
    public:
        spell_gen_magic_rooster() : SpellScriptLoader("spell_gen_magic_rooster") { }

        class spell_gen_magic_rooster_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_magic_rooster_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Player* target = GetHitPlayer())
                {
                    // prevent client crashes from stacking mounts
                    target->RemoveAurasByType(SPELL_AURA_MOUNTED);

                    uint32 spellId = SPELL_MAGIC_ROOSTER_NORMAL;
                    switch (target->getRace())
                    {
                        case RACE_DRAENEI:
                            if (target->getGender() == GENDER_MALE)
                                spellId = SPELL_MAGIC_ROOSTER_DRAENEI_MALE;
                            break;
                        case RACE_TAUREN:
                            if (target->getGender() == GENDER_MALE)
                                spellId = SPELL_MAGIC_ROOSTER_TAUREN_MALE;
                            break;
                        default:
                            break;
                    }

                    target->CastSpell(target, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_magic_rooster_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_magic_rooster_SpellScript();
        }
};

class spell_gen_allow_cast_from_item_only: public SpellScriptLoader
{
    public:
        spell_gen_allow_cast_from_item_only() : SpellScriptLoader("spell_gen_allow_cast_from_item_only") { }

        class spell_gen_allow_cast_from_item_only_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_allow_cast_from_item_only_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (!GetCastItem())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_allow_cast_from_item_only_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_allow_cast_from_item_only_SpellScript();
        }
};

enum Launch
{
    SPELL_LAUNCH_NO_FALLING_DAMAGE = 66251
};

class spell_gen_launch: public SpellScriptLoader
{
    public:
        spell_gen_launch() : SpellScriptLoader("spell_gen_launch") {}

        class spell_gen_launch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_launch_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                    player->AddAura(SPELL_LAUNCH_NO_FALLING_DAMAGE, player); // prevents falling damage
            }

            void Launch()
            {
                WorldLocation const* const position = GetExplTargetDest();

                if (Player* player = GetHitPlayer())
                {
                    player->ExitVehicle();

                    // A better research is needed
                    // There is no spell for this, the following calculation was based on void Spell::CalculateJumpSpeeds

                    float speedZ = 10.0f;
                    float dist = position->GetExactDist2d(player->GetPositionX(), player->GetPositionY());
                    float speedXY = dist;

                    player->GetMotionMaster()->MoveJump(position->GetPositionX(), position->GetPositionY(), position->GetPositionZ(), speedXY, speedZ);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_launch_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_FORCE_CAST);
                AfterHit += SpellHitFn(spell_gen_launch_SpellScript::Launch);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_launch_SpellScript();
        }
};

enum VehicleScaling
{
    SPELL_GEAR_SCALING      = 66668
};

class spell_gen_vehicle_scaling: public SpellScriptLoader
{
    public:
        spell_gen_vehicle_scaling() : SpellScriptLoader("spell_gen_vehicle_scaling") { }

        class spell_gen_vehicle_scaling_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_vehicle_scaling_SpellScript);

            SpellCastResult CheckCast()
            {
                if (Unit* target = GetExplTargetUnit())
                {
                    if (target->IsPlayer())
                        return SPELL_FAILED_DONT_REPORT;
                    else if (target->getLevel() >= MAX_LEVEL)
                        return SPELL_FAILED_DONT_REPORT;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_vehicle_scaling_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_vehicle_scaling_SpellScript();
        }

        class spell_gen_vehicle_scaling_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_vehicle_scaling_AuraScript);

            bool Load() override
            {
                return GetCaster() && GetCaster()->IsPlayer();
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                Unit* caster = GetCaster();
                float factor;
                uint16 baseItemLevel;

                // TODO: Reserach coeffs for different vehicles
                switch (GetId())
                {
                    case SPELL_GEAR_SCALING:
                        factor = 1.0f;
                        baseItemLevel = 405;
                        break;
                    default:
                        factor = 1.0f;
                        baseItemLevel = 405;
                        break;
                }

                uint16 avgILvl = caster->ToPlayer()->GetAverageItemLevelEquipped();
                if (avgILvl < baseItemLevel)
                    return;                     // TODO: Research possibility of scaling down

                amount = uint16((avgILvl - baseItemLevel) * factor);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_vehicle_scaling_AuraScript();
        }
};

class spell_gen_oracle_wolvar_reputation: public SpellScriptLoader
{
    public:
        spell_gen_oracle_wolvar_reputation() : SpellScriptLoader("spell_gen_oracle_wolvar_reputation") { }

        class spell_gen_oracle_wolvar_reputation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_oracle_wolvar_reputation_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* player = GetCaster()->ToPlayer();
                uint32 factionId = GetSpellInfo()->Effects[effIndex].CalcValue();
                int32  repChange =  GetSpellInfo()->Effects[EFFECT_1].CalcValue();

                FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

                if (!factionEntry)
                    return;

                // Set rep to baserep + basepoints (expecting spillover for oposite faction -> become hated)
                // Not when player already has equal or higher rep with this faction
                if (player->GetReputationMgr().GetBaseReputation(factionEntry) < repChange)
                    player->GetReputationMgr().SetReputation(factionEntry, repChange);

                // EFFECT_INDEX_2 most likely update at war state, we already handle this in SetReputation
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_oracle_wolvar_reputation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_oracle_wolvar_reputation_SpellScript();
        }
};

class spell_gen_luck_of_the_draw: public SpellScriptLoader
{
    public:
        spell_gen_luck_of_the_draw() : SpellScriptLoader("spell_gen_luck_of_the_draw") { }

        class spell_gen_luck_of_the_draw_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_luck_of_the_draw_AuraScript);

            bool Load()
            {
                return GetUnitOwner()->IsPlayer();
            }

            // cheap hax to make it have update calls
            void CalcPeriodic(AuraEffect const* /*effect*/, bool& isPeriodic, int32& amplitude)
            {
                isPeriodic = true;
                amplitude = 5 * IN_MILLISECONDS;
            }

            void Update(AuraEffect* /*effect*/)
            {
                if (Player* owner = GetUnitOwner()->ToPlayer())
                {
                    const LfgDungeonSet dungeons = sLFGMgr->GetSelectedDungeons(owner->GetGUID());
                    LfgDungeonSet::const_iterator itr = dungeons.begin();

                    if (itr == dungeons.end())
                    {
                        Remove(AURA_REMOVE_BY_DEFAULT);
                        return;
                    }


                    LFGDungeonEntry const* randomDungeon = sLFGDungeonStore.LookupEntry(*itr);
                    if (GroupPtr group = owner->GetGroup())
                        if (Map const* map = owner->GetMap())
                            if (group->isLFGGroup())
                                if (uint32 dungeonId = sLFGMgr->GetDungeon(group->GetGUID(), true))
                                    if (LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(dungeonId))
                                        if (uint32(dungeon->map) == map->GetId() && dungeon->difficulty == uint32(map->GetDifficultyID()))
                                            if (randomDungeon && randomDungeon->type == TYPEID_RANDOM_DUNGEON)
                                                return; // in correct dungeon

                    Remove(AURA_REMOVE_BY_DEFAULT);
                }
            }

            void Register()
            {
                DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_gen_luck_of_the_draw_AuraScript::CalcPeriodic, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_luck_of_the_draw_AuraScript::Update, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_luck_of_the_draw_AuraScript();
        }
};

enum DummyTrigger
{
    SPELL_PERSISTANT_SHIELD_TRIGGERED       = 26470,
    SPELL_PERSISTANT_SHIELD                 = 26467
};

class spell_gen_dummy_trigger: public SpellScriptLoader
{
    public:
        spell_gen_dummy_trigger() : SpellScriptLoader("spell_gen_dummy_trigger") { }

        class spell_gen_dummy_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dummy_trigger_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                int32 damage = GetEffectValue();
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                    if (SpellInfo const* triggeredByAuraSpell = GetTriggeringSpell())
                        if (triggeredByAuraSpell->Id == SPELL_PERSISTANT_SHIELD_TRIGGERED)
                            caster->CastCustomSpell(target, SPELL_PERSISTANT_SHIELD_TRIGGERED, &damage, NULL, NULL, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dummy_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dummy_trigger_SpellScript();
        }

};

class spell_gen_spirit_healer_res: public SpellScriptLoader
{
    public:
        spell_gen_spirit_healer_res(): SpellScriptLoader("spell_gen_spirit_healer_res") { }

        class spell_gen_spirit_healer_res_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spirit_healer_res_SpellScript);

            bool Load()
            {
                return GetOriginalCaster() && GetOriginalCaster()->IsPlayer();
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* l_OriginalCaster = GetOriginalCaster()->ToPlayer();

                if (Unit * l_Target = GetHitUnit())
                {
                    WorldPacket l_Data(SMSG_SPIRIT_HEALER_CONFIRM, 16 + 2);
                    l_Data.appendPackGUID(l_Target->GetGUID());

                    l_OriginalCaster->GetSession()->SendPacket(&l_Data);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_spirit_healer_res_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_spirit_healer_res_SpellScript();
        }
};

enum TransporterBackfires
{
    SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH     = 23444,
    SPELL_TRANSPORTER_EVIL_TWIN                 = 23445,
    SPELL_TRANSPORTER_MALFUNCTION_MISS          = 36902
};

class spell_gen_gadgetzan_transporter_backfire: public SpellScriptLoader
{
    public:
        spell_gen_gadgetzan_transporter_backfire() : SpellScriptLoader("spell_gen_gadgetzan_transporter_backfire") { }

        class spell_gen_gadgetzan_transporter_backfire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gadgetzan_transporter_backfire_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH) || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_EVIL_TWIN)
                    || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_MISS))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                int32 r = irand(0, 119);
                if (r < 20)                           // Transporter Malfunction - 1/6 polymorph
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH, true);
                else if (r < 100)                     // Evil Twin               - 4/6 evil twin
                    caster->CastSpell(caster, SPELL_TRANSPORTER_EVIL_TWIN, true);
                else                                    // Transporter Malfunction - 1/6 miss the target
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_MISS, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gadgetzan_transporter_backfire_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gadgetzan_transporter_backfire_SpellScript();
        }
};

enum GnomishTransporter
{
    SPELL_TRANSPORTER_SUCCESS                   = 23441,
    SPELL_TRANSPORTER_FAILURE                   = 23446
};

class spell_gen_gnomish_transporter: public SpellScriptLoader
{
    public:
        spell_gen_gnomish_transporter() : SpellScriptLoader("spell_gen_gnomish_transporter") { }

        class spell_gen_gnomish_transporter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gnomish_transporter_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_SUCCESS) || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_FAILURE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, roll_chance_i(50) ? SPELL_TRANSPORTER_SUCCESS : SPELL_TRANSPORTER_FAILURE , true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gnomish_transporter_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gnomish_transporter_SpellScript();
        }
};

enum DalaranDisguiseSpells
{
    SPELL_SUNREAVER_DISGUISE_TRIGGER       = 69672,
    SPELL_SUNREAVER_DISGUISE_FEMALE        = 70973,
    SPELL_SUNREAVER_DISGUISE_MALE          = 70974,

    SPELL_SILVER_COVENANT_DISGUISE_TRIGGER = 69673,
    SPELL_SILVER_COVENANT_DISGUISE_FEMALE  = 70971,
    SPELL_SILVER_COVENANT_DISGUISE_MALE    = 70972
};

class spell_gen_dalaran_disguise: public SpellScriptLoader
{
    public:
        spell_gen_dalaran_disguise(const char* name) : SpellScriptLoader(name) {}

        class spell_gen_dalaran_disguise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dalaran_disguise_SpellScript);
            bool Validate(SpellInfo const* spellEntry)
            {
                switch (spellEntry->Id)
                {
                    case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_MALE))
                            return false;
                        break;
                    case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_MALE))
                            return false;
                        break;
                }
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                {
                    uint8 gender = player->getGender();

                    uint32 spellId = GetSpellInfo()->Id;

                    switch (spellId)
                    {
                        case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SUNREAVER_DISGUISE_FEMALE : SPELL_SUNREAVER_DISGUISE_MALE;
                            break;
                        case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SILVER_COVENANT_DISGUISE_FEMALE : SPELL_SILVER_COVENANT_DISGUISE_MALE;
                            break;
                        default:
                            break;
                    }
                    GetCaster()->CastSpell(player, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dalaran_disguise_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dalaran_disguise_SpellScript();
        }
};

/* DOCUMENTATION: Break-Shield spells
    Break-Shield spells can be classified in three groups:

        - Spells on vehicle bar used by players:
            + EFFECT_0: SCRIPT_EFFECT
            + EFFECT_1: NONE
            + EFFECT_2: NONE
        - Spells casted by players triggered by script:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: FORCE_CAST
        - Spells casted by NPCs on players:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: NONE

    In the following script we handle the SCRIPT_EFFECT for effIndex EFFECT_0 and EFFECT_1.
        - When handling EFFECT_0 we're in the "Spells on vehicle bar used by players" case
          and we'll trigger "Spells casted by players triggered by script"
        - When handling EFFECT_1 we're in the "Spells casted by players triggered by script"
          or "Spells casted by NPCs on players" so we'll search for the first defend layer and drop it.
*/

enum BreakShieldSpells
{
    SPELL_BREAK_SHIELD_DAMAGE_2K                 = 62626,
    SPELL_BREAK_SHIELD_DAMAGE_10K                = 64590,

    SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS    = 62575, // Also on ToC5 mounts
    SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE = 64595,
    SPELL_BREAK_SHIELD_TRIGGER_UNK               = 66480
};

class spell_gen_break_shield: public SpellScriptLoader
{
    public:
        spell_gen_break_shield(const char* name) : SpellScriptLoader(name) {}

        class spell_gen_break_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_break_shield_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_BREAK_SHIELD_TRIGGER_UNK:
                            case SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_10K;
                                break;
                            case SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_2K;
                                break;
                            default:
                                return;
                        }

                        if (Unit* rider = GetCaster()->GetCharmer())
                            rider->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                        {
                            Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                            for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                            {
                                if (Aura* aura = itr->second->GetBase())
                                {
                                    SpellInfo const* auraInfo = aura->GetSpellInfo();
                                    if (aura->GetId() == 62552 || aura->GetId() == 62719 || aura->GetId() == 64100 || aura->GetId() == 66482)
                                    {
                                        aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                        // Remove dummys from rider (Necessary for updating visual shields)
                                        if (Unit* rider = target->GetCharmer())
                                            if (Aura* defend = rider->GetAura(aura->GetId()))
                                                defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                    default:
                        break;
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_break_shield_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_break_shield_SpellScript();
        }
};

/* DOCUMENTATION: Charge spells
    Charge spells can be classified in four groups:

        - Spells on vehicle bar used by players:
            + EFFECT_0: SCRIPT_EFFECT
            + EFFECT_1: TRIGGER_SPELL
            + EFFECT_2: NONE
        - Spells casted by player's mounts triggered by script:
            + EFFECT_0: CHARGE
            + EFFECT_1: TRIGGER_SPELL
            + EFFECT_2: APPLY_AURA
        - Spells casted by players on the target triggered by script:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: NONE
        - Spells casted by NPCs on players:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: CHARGE
            + EFFECT_2: SCRIPT_EFFECT

    In the following script we handle the SCRIPT_EFFECT and CHARGE
        - When handling SCRIPT_EFFECT:
            + EFFECT_0: Corresponds to "Spells on vehicle bar used by players" and we make player's mount cast
              the charge effect on the current target ("Spells casted by player's mounts triggered by script").
            + EFFECT_1 and EFFECT_2: Triggered when "Spells casted by player's mounts triggered by script" hits target,
              corresponding to "Spells casted by players on the target triggered by script" and "Spells casted by
              NPCs on players" and we check Defend layers and drop a charge of the first found.
        - When handling CHARGE:
            + Only launched for "Spells casted by player's mounts triggered by script", makes the player cast the
              damaging spell on target with a small chance of failing it.
*/

enum ChargeSpells
{
    SPELL_CHARGE_DAMAGE_8K5             = 62874,
    SPELL_CHARGE_DAMAGE_20K             = 68498,
    SPELL_CHARGE_DAMAGE_45K             = 64591,

    SPELL_CHARGE_CHARGING_EFFECT_8K5    = 63661,
    SPELL_CHARGE_CHARGING_EFFECT_20K_1  = 68284,
    SPELL_CHARGE_CHARGING_EFFECT_20K_2  = 68501,
    SPELL_CHARGE_CHARGING_EFFECT_45K_1  = 62563,
    SPELL_CHARGE_CHARGING_EFFECT_45K_2  = 66481,

    SPELL_CHARGE_TRIGGER_FACTION_MOUNTS = 62960,
    SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION = 68282,

    SPELL_CHARGE_MISS_EFFECT            = 62977
};

class spell_gen_mounted_charge: public SpellScriptLoader
{
    public:
        spell_gen_mounted_charge() : SpellScriptLoader("spell_gen_mounted_charge") { }

        class spell_gen_mounted_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mounted_charge_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_20K_1;
                                break;
                            case SPELL_CHARGE_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_8K5;
                                break;
                            default:
                                return;
                        }

                        // If target isn't a training dummy there's a chance of failing the charge
                        if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL) && roll_chance_f(12.5f))
                            spellId = SPELL_CHARGE_MISS_EFFECT;

                        if (Unit* vehicle = GetCaster()->GetVehicleBase())
                            vehicle->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                    case EFFECT_2:
                        {
                            Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                            for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                            {
                                if (Aura* aura = itr->second->GetBase())
                                {
                                    SpellInfo const* auraInfo = aura->GetSpellInfo();
                                    if (aura->GetId() == 62552 || aura->GetId() == 62719 || aura->GetId() == 64100 || aura->GetId() == 66482)
                                    {
                                        aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                        // Remove dummys from rider (Necessary for updating visual shields)
                                        if (Unit* rider = target->GetCharmer())
                                            if (Aura* defend = rider->GetAura(aura->GetId()))
                                                defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                    default:
                        break;
                }
            }

            void HandleChargeEffect(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_CHARGE_CHARGING_EFFECT_8K5:
                        spellId = SPELL_CHARGE_DAMAGE_8K5;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_2:
                        spellId = SPELL_CHARGE_DAMAGE_20K;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_2:
                        spellId = SPELL_CHARGE_DAMAGE_45K;
                        break;
                    default:
                        return;
                }

                if (Unit* rider = GetCaster()->GetCharmer())
                    rider->CastSpell(GetHitUnit(), spellId, false);
                else
                    GetCaster()->CastSpell(GetHitUnit(), spellId, false);
            }

            void Register()
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                if (spell->HasEffect(SPELL_EFFECT_SCRIPT_EFFECT))
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);

                if (spell->Effects[EFFECT_0].Effect == SPELL_EFFECT_CHARGE)
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleChargeEffect, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_mounted_charge_SpellScript();
        }
};

enum DefendVisuals
{
    SPELL_VISUAL_SHIELD_1 = 63130,
    SPELL_VISUAL_SHIELD_2 = 63131,
    SPELL_VISUAL_SHIELD_3 = 63132
};

class spell_gen_defend: public SpellScriptLoader
{
    public:
        spell_gen_defend() : SpellScriptLoader("spell_gen_defend") { }

        class spell_gen_defend_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_defend_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_2))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_3))
                    return false;
                return true;
            }

            void RefreshVisualShields(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                {
                    Unit* target = GetTarget();

                    for (uint8 i = 0; i < GetSpellInfo()->StackAmount; ++i)
                        target->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);

                    target->CastSpell(target, SPELL_VISUAL_SHIELD_1 + GetAura()->GetStackAmount() - 1, true, NULL, aurEff);
                }
                else
                    GetTarget()->RemoveAurasDueToSpell(GetId());
            }

            void RemoveVisualShields(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                for (uint8 i = 0; i < GetSpellInfo()->StackAmount; ++i)
                    GetTarget()->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);
            }

            void RemoveDummyFromDriver(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (TempSummon* vehicle = caster->ToTempSummon())
                        if (Unit* rider = vehicle->GetSummoner())
                            rider->RemoveAurasDueToSpell(GetId());
            }

            void Register()
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                // Defend spells casted by NPCs (add visuals)
                if (spell->Effects[EFFECT_0].ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_defend_AuraScript::RefreshVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                }

                // Remove Defend spell from player when he dismounts
                if (spell->Effects[EFFECT_2].ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
                    AfterEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveDummyFromDriver, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);

                // Defend spells casted by players (add/remove visuals)
                if (spell->Effects[EFFECT_1].ApplyAuraName == SPELL_AURA_DUMMY)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_defend_AuraScript::RefreshVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                }
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_defend_AuraScript();
        }
};

enum MountedDuelSpells
{
    SPELL_ON_TOURNAMENT_MOUNT = 63034,
    SPELL_MOUNTED_DUEL        = 62875
};

class spell_gen_tournament_duel: public SpellScriptLoader
{
    public:
        spell_gen_tournament_duel() : SpellScriptLoader("spell_gen_tournament_duel") { }

        class spell_gen_tournament_duel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_tournament_duel_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ON_TOURNAMENT_MOUNT))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_MOUNTED_DUEL))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* rider = GetCaster()->GetCharmer())
                {
                    if (Player* plrTarget = GetHitPlayer())
                    {
                        if (plrTarget->HasAura(SPELL_ON_TOURNAMENT_MOUNT) && plrTarget->GetVehicleBase())
                            rider->CastSpell(plrTarget, SPELL_MOUNTED_DUEL, true);
                    }
                    else if (Unit* unitTarget = GetHitUnit())
                    {
                        if (unitTarget->GetCharmer() && unitTarget->GetCharmer()->IsPlayer() && unitTarget->GetCharmer()->HasAura(SPELL_ON_TOURNAMENT_MOUNT))
                            rider->CastSpell(unitTarget->GetCharmer(), SPELL_MOUNTED_DUEL, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_tournament_duel_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_tournament_duel_SpellScript();
        }
};

enum TournamentMountsSpells
{
    SPELL_LANCE_EQUIPPED = 62853
};

class spell_gen_summon_tournament_mount: public SpellScriptLoader
{
    public:
        spell_gen_summon_tournament_mount() : SpellScriptLoader("spell_gen_summon_tournament_mount") { }

        class spell_gen_summon_tournament_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_summon_tournament_mount_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LANCE_EQUIPPED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfLanceEquiped()
            {
                if (GetCaster()->IsInDisallowedMountForm())
                    GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

                if (!GetCaster()->HasAura(SPELL_LANCE_EQUIPPED))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_LANCE_EQUIPPED);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_summon_tournament_mount_SpellScript::CheckIfLanceEquiped);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_summon_tournament_mount_SpellScript();
        }
};

enum TournamentPennantSpells
{
    SPELL_PENNANT_STORMWIND_ASPIRANT      = 62595,
    SPELL_PENNANT_STORMWIND_VALIANT       = 62596,
    SPELL_PENNANT_STORMWIND_CHAMPION      = 62594,
    SPELL_PENNANT_GNOMEREGAN_ASPIRANT     = 63394,
    SPELL_PENNANT_GNOMEREGAN_VALIANT      = 63395,
    SPELL_PENNANT_GNOMEREGAN_CHAMPION     = 63396,
    SPELL_PENNANT_SEN_JIN_ASPIRANT        = 63397,
    SPELL_PENNANT_SEN_JIN_VALIANT         = 63398,
    SPELL_PENNANT_SEN_JIN_CHAMPION        = 63399,
    SPELL_PENNANT_SILVERMOON_ASPIRANT     = 63401,
    SPELL_PENNANT_SILVERMOON_VALIANT      = 63402,
    SPELL_PENNANT_SILVERMOON_CHAMPION     = 63403,
    SPELL_PENNANT_DARNASSUS_ASPIRANT      = 63404,
    SPELL_PENNANT_DARNASSUS_VALIANT       = 63405,
    SPELL_PENNANT_DARNASSUS_CHAMPION      = 63406,
    SPELL_PENNANT_EXODAR_ASPIRANT         = 63421,
    SPELL_PENNANT_EXODAR_VALIANT          = 63422,
    SPELL_PENNANT_EXODAR_CHAMPION         = 63423,
    SPELL_PENNANT_IRONFORGE_ASPIRANT      = 63425,
    SPELL_PENNANT_IRONFORGE_VALIANT       = 63426,
    SPELL_PENNANT_IRONFORGE_CHAMPION      = 63427,
    SPELL_PENNANT_UNDERCITY_ASPIRANT      = 63428,
    SPELL_PENNANT_UNDERCITY_VALIANT       = 63429,
    SPELL_PENNANT_UNDERCITY_CHAMPION      = 63430,
    SPELL_PENNANT_ORGRIMMAR_ASPIRANT      = 63431,
    SPELL_PENNANT_ORGRIMMAR_VALIANT       = 63432,
    SPELL_PENNANT_ORGRIMMAR_CHAMPION      = 63433,
    SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT  = 63434,
    SPELL_PENNANT_THUNDER_BLUFF_VALIANT   = 63435,
    SPELL_PENNANT_THUNDER_BLUFF_CHAMPION  = 63436,
    SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT = 63606,
    SPELL_PENNANT_ARGENT_CRUSADE_VALIANT  = 63500,
    SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION = 63501,
    SPELL_PENNANT_EBON_BLADE_ASPIRANT     = 63607,
    SPELL_PENNANT_EBON_BLADE_VALIANT      = 63608,
    SPELL_PENNANT_EBON_BLADE_CHAMPION     = 63609
};

enum TournamentMounts
{
    NPC_STORMWIND_STEED             = 33217,
    NPC_IRONFORGE_RAM               = 33316,
    NPC_GNOMEREGAN_MECHANOSTRIDER   = 33317,
    NPC_EXODAR_ELEKK                = 33318,
    NPC_DARNASSIAN_NIGHTSABER       = 33319,
    NPC_ORGRIMMAR_WOLF              = 33320,
    NPC_DARK_SPEAR_RAPTOR           = 33321,
    NPC_THUNDER_BLUFF_KODO          = 33322,
    NPC_SILVERMOON_HAWKSTRIDER      = 33323,
    NPC_FORSAKEN_WARHORSE           = 33324,
    NPC_ARGENT_WARHORSE             = 33782,
    NPC_ARGENT_STEED_ASPIRANT       = 33845,
    NPC_ARGENT_HAWKSTRIDER_ASPIRANT = 33844
};

enum TournamentQuestsAchievements
{
    ACHIEVEMENT_CHAMPION_STORMWIND     = 2781,
    ACHIEVEMENT_CHAMPION_DARNASSUS     = 2777,
    ACHIEVEMENT_CHAMPION_IRONFORGE     = 2780,
    ACHIEVEMENT_CHAMPION_GNOMEREGAN    = 2779,
    ACHIEVEMENT_CHAMPION_THE_EXODAR    = 2778,
    ACHIEVEMENT_CHAMPION_ORGRIMMAR     = 2783,
    ACHIEVEMENT_CHAMPION_SEN_JIN       = 2784,
    ACHIEVEMENT_CHAMPION_THUNDER_BLUFF = 2786,
    ACHIEVEMENT_CHAMPION_UNDERCITY     = 2787,
    ACHIEVEMENT_CHAMPION_SILVERMOON    = 2785,
    ACHIEVEMENT_ARGENT_VALOR           = 2758,
    ACHIEVEMENT_CHAMPION_ALLIANCE      = 2782,
    ACHIEVEMENT_CHAMPION_HORDE         = 2788,

    QUEST_VALIANT_OF_STORMWIND         = 13593,
    QUEST_A_VALIANT_OF_STORMWIND       = 13684,
    QUEST_VALIANT_OF_DARNASSUS         = 13706,
    QUEST_A_VALIANT_OF_DARNASSUS       = 13689,
    QUEST_VALIANT_OF_IRONFORGE         = 13703,
    QUEST_A_VALIANT_OF_IRONFORGE       = 13685,
    QUEST_VALIANT_OF_GNOMEREGAN        = 13704,
    QUEST_A_VALIANT_OF_GNOMEREGAN      = 13688,
    QUEST_VALIANT_OF_THE_EXODAR        = 13705,
    QUEST_A_VALIANT_OF_THE_EXODAR      = 13690,
    QUEST_VALIANT_OF_ORGRIMMAR         = 13707,
    QUEST_A_VALIANT_OF_ORGRIMMAR       = 13691,
    QUEST_VALIANT_OF_SEN_JIN           = 13708,
    QUEST_A_VALIANT_OF_SEN_JIN         = 13693,
    QUEST_VALIANT_OF_THUNDER_BLUFF     = 13709,
    QUEST_A_VALIANT_OF_THUNDER_BLUFF   = 13694,
    QUEST_VALIANT_OF_UNDERCITY         = 13710,
    QUEST_A_VALIANT_OF_UNDERCITY       = 13695,
    QUEST_VALIANT_OF_SILVERMOON        = 13711,
    QUEST_A_VALIANT_OF_SILVERMOON      = 13696
};

class spell_gen_on_tournament_mount: public SpellScriptLoader
{
    public:
        spell_gen_on_tournament_mount() : SpellScriptLoader("spell_gen_on_tournament_mount") { }

        class spell_gen_on_tournament_mount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_on_tournament_mount_AuraScript);

            uint32 _pennantSpellId;

            bool Load()
            {
                _pennantSpellId = 0;
                return GetCaster() && GetCaster()->IsPlayer();
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* vehicle = caster->GetVehicleBase())
                    {
                        _pennantSpellId = GetPennatSpellId(caster->ToPlayer(), vehicle);
                        caster->CastSpell(caster, _pennantSpellId, true);
                    }
                }
            }

            void HandleRemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->RemoveAurasDueToSpell(_pennantSpellId);
            }

            uint32 GetPennatSpellId(Player* player, Unit* mount)
            {
                switch (mount->GetEntry())
                {
                    case NPC_ARGENT_STEED_ASPIRANT:
                    case NPC_STORMWIND_STEED:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_STORMWIND) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_VALIANT;
                        else
                            return SPELL_PENNANT_STORMWIND_ASPIRANT;
                    }
                    case NPC_GNOMEREGAN_MECHANOSTRIDER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_GNOMEREGAN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_VALIANT;
                        else
                            return SPELL_PENNANT_GNOMEREGAN_ASPIRANT;
                    }
                    case NPC_DARK_SPEAR_RAPTOR:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SEN_JIN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_VALIANT;
                        else
                            return SPELL_PENNANT_SEN_JIN_ASPIRANT;
                    }
                    case NPC_ARGENT_HAWKSTRIDER_ASPIRANT:
                    case NPC_SILVERMOON_HAWKSTRIDER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SILVERMOON) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_VALIANT;
                        else
                            return SPELL_PENNANT_SILVERMOON_ASPIRANT;
                    }
                    case NPC_DARNASSIAN_NIGHTSABER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_DARNASSUS) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_VALIANT;
                        else
                            return SPELL_PENNANT_DARNASSUS_ASPIRANT;
                    }
                    case NPC_EXODAR_ELEKK:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THE_EXODAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_VALIANT;
                        else
                            return SPELL_PENNANT_EXODAR_ASPIRANT;
                    }
                    case NPC_IRONFORGE_RAM:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_IRONFORGE) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_VALIANT;
                        else
                            return SPELL_PENNANT_IRONFORGE_ASPIRANT;
                    }
                    case NPC_FORSAKEN_WARHORSE:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_UNDERCITY) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_VALIANT;
                        else
                            return SPELL_PENNANT_UNDERCITY_ASPIRANT;
                    }
                    case NPC_ORGRIMMAR_WOLF:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_ORGRIMMAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_VALIANT;
                        else
                            return SPELL_PENNANT_ORGRIMMAR_ASPIRANT;
                    }
                    case NPC_THUNDER_BLUFF_KODO:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THUNDER_BLUFF) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_VALIANT;
                        else
                            return SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT;
                    }
                    case NPC_ARGENT_WARHORSE:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_ALLIANCE) || player->HasAchieved(ACHIEVEMENT_CHAMPION_HORDE))
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_CHAMPION : SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION;
                        else if (player->HasAchieved(ACHIEVEMENT_ARGENT_VALOR))
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_VALIANT : SPELL_PENNANT_ARGENT_CRUSADE_VALIANT;
                        else
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_ASPIRANT : SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT;
                    }
                    default:
                        return 0;
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_on_tournament_mount_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_on_tournament_mount_AuraScript::HandleRemoveEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_on_tournament_mount_AuraScript();
        }
};

class spell_gen_tournament_pennant: public SpellScriptLoader
{
    public:
        spell_gen_tournament_pennant() : SpellScriptLoader("spell_gen_tournament_pennant") { }

        class spell_gen_tournament_pennant_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_tournament_pennant_AuraScript);

            bool Load()
            {
                return GetCaster() && GetCaster()->IsPlayer();
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (!caster->GetVehicleBase())
                        caster->RemoveAurasDueToSpell(GetId());
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_tournament_pennant_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_tournament_pennant_AuraScript();
        }
};

enum ChaosBlast
{
    SPELL_CHAOS_BLAST   = 37675
};

class spell_gen_chaos_blast: public SpellScriptLoader
{
    public:
        spell_gen_chaos_blast() : SpellScriptLoader("spell_gen_chaos_blast") { }

        class spell_gen_chaos_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_chaos_blast_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CHAOS_BLAST))
                    return false;
                return true;
            }
            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                int32 basepoints0 = 100;
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                    caster->CastCustomSpell(target, SPELL_CHAOS_BLAST, &basepoints0, NULL, NULL, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_chaos_blast_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_chaos_blast_SpellScript();
        }

};

class spell_gen_wg_water: public SpellScriptLoader
{
    public:
        spell_gen_wg_water() : SpellScriptLoader("spell_gen_wg_water") {}

        class spell_gen_wg_water_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_wg_water_SpellScript);

            SpellCastResult CheckCast()
            {
                if (!GetSpellInfo()->CheckTargetCreatureType(GetCaster()))
                    return SPELL_FAILED_DONT_REPORT;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_wg_water_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_wg_water_SpellScript();
        }
};

class spell_gen_count_pct_from_max_hp: public SpellScriptLoader
{
    public:
        spell_gen_count_pct_from_max_hp(char const* name, int32 damagePct = 0) : SpellScriptLoader(name), _damagePct(damagePct) { }

        class spell_gen_count_pct_from_max_hp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_count_pct_from_max_hp_SpellScript);

        public:
            spell_gen_count_pct_from_max_hp_SpellScript(int32 damagePct) : SpellScript(), _damagePct(damagePct) { }

            void RecalculateDamage()
            {
                if (!_damagePct)
                    _damagePct = GetHitDamage();

                SetHitDamage(GetHitUnit()->CountPctFromMaxHealth(_damagePct));
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_count_pct_from_max_hp_SpellScript::RecalculateDamage);
            }

        private:
            int32 _damagePct;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_count_pct_from_max_hp_SpellScript(_damagePct);
        }

    private:
        int32 _damagePct;
};

class spell_gen_despawn_self: public SpellScriptLoader
{
public:
    spell_gen_despawn_self() : SpellScriptLoader("spell_gen_despawn_self") { }

    class spell_gen_despawn_self_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_despawn_self_SpellScript);

        bool Load()
        {
            return GetCaster()->GetTypeId() == TYPEID_UNIT;
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            if (GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_DUMMY || GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_SCRIPT_EFFECT)
                GetCaster()->ToCreature()->DespawnOrUnsummon();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_despawn_self_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_ANY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_despawn_self_SpellScript();
    }
};

class spell_gen_touch_the_nightmare: public SpellScriptLoader
{
public:
    spell_gen_touch_the_nightmare() : SpellScriptLoader("spell_gen_touch_the_nightmare") { }

    class spell_gen_touch_the_nightmare_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_touch_the_nightmare_SpellScript);

        void HandleDamageCalc(SpellEffIndex /*effIndex*/)
        {
            uint32 bp = GetCaster()->GetMaxHealth() * 0.3f;
            SetHitDamage(bp);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_touch_the_nightmare_SpellScript::HandleDamageCalc, EFFECT_2, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_touch_the_nightmare_SpellScript();
    }
};

class spell_gen_dream_funnel: public SpellScriptLoader
{
public:
    spell_gen_dream_funnel() : SpellScriptLoader("spell_gen_dream_funnel") { }

    class spell_gen_dream_funnel_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_dream_funnel_AuraScript);

        void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
        {
            if (GetCaster())
                amount = GetCaster()->GetMaxHealth() * 0.05f;

            canBeRecalculated = false;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_dream_funnel_AuraScript();
    }
};

enum GenericBandage
{
    SPELL_RECENTLY_BANDAGED = 11196
};

class spell_gen_bandage: public SpellScriptLoader
{
    public:
        spell_gen_bandage() : SpellScriptLoader("spell_gen_bandage") { }

        class spell_gen_bandage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bandage_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RECENTLY_BANDAGED))
                    return false;
                return true;
            }

            SpellCastResult CheckCast()
            {
                if (Unit* target = GetExplTargetUnit())
                {
                    if (target->HasAura(SPELL_RECENTLY_BANDAGED))
                        return SPELL_FAILED_TARGET_AURASTATE;
                }
                return SPELL_CAST_OK;
            }

            void HandleScript()
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, SPELL_RECENTLY_BANDAGED, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_bandage_SpellScript::CheckCast);
                AfterHit += SpellHitFn(spell_gen_bandage_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bandage_SpellScript();
        }
};

enum GenericLifebloom
{
    SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL        = 43422,
    SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL              = 52552,
    SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL           = 53692,
    SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL           = 57763,
    SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL    = 66094
};

class spell_gen_lifebloom: public SpellScriptLoader
{
    public:
        spell_gen_lifebloom(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_lifebloom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lifebloom_AuraScript);

        public:
            spell_gen_lifebloom_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // Final heal only on duration end
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                // final heal
                GetTarget()->CastSpell(GetTarget(), _spellId, true, NULL, aurEff, GetCasterGUID());
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_lifebloom_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_lifebloom_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

/// The following code for mounts might surely be deprecated
enum Mounts
{
    SPELL_COLD_WEATHER_FLYING           = 54197,

    // Magic Broom
    SPELL_MAGIC_BROOM_150               = 42667,
    SPELL_MAGIC_BROOM_280               = 42668,

    // Winged Steed of the Ebon Blade
    SPELL_WINGED_STEED_150              = 54726,
    SPELL_WINGED_STEED_280              = 54727,

    // Blazing Hippogryph
    SPELL_BLAZING_HIPPOGRYPH_150        = 74854,
    SPELL_BLAZING_HIPPOGRYPH_280        = 74855
};

class spell_gen_mount: public SpellScriptLoader
{
    public:
        spell_gen_mount(const char* name, uint32 mount0 = 0, uint32 mount60 = 0, uint32 mount100 = 0, uint32 mount150 = 0, uint32 mount280 = 0, uint32 mount310 = 0) : SpellScriptLoader(name),
            _mount0(mount0), _mount60(mount60), _mount100(mount100), _mount150(mount150), _mount280(mount280), _mount310(mount310) { }

        class spell_gen_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mount_SpellScript);

        public:
            spell_gen_mount_SpellScript(uint32 mount0, uint32 mount60, uint32 mount100, uint32 mount150, uint32 mount280, uint32 mount310) : SpellScript(),
                _mount0(mount0), _mount60(mount60), _mount100(mount100), _mount150(mount150), _mount280(mount280), _mount310(mount310) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (_mount0 && !sSpellMgr->GetSpellInfo(_mount0))
                    return false;
                if (_mount60 && !sSpellMgr->GetSpellInfo(_mount60))
                    return false;
                if (_mount100 && !sSpellMgr->GetSpellInfo(_mount100))
                    return false;
                if (_mount150 && !sSpellMgr->GetSpellInfo(_mount150))
                    return false;
                if (_mount280 && !sSpellMgr->GetSpellInfo(_mount280))
                    return false;
                if (_mount310 && !sSpellMgr->GetSpellInfo(_mount310))
                    return false;
                return true;
            }

            void HandleMount(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Player* target = GetHitPlayer())
                {
                    // Prevent stacking of mounts and client crashes upon dismounting
                    target->RemoveAurasByType(SPELL_AURA_MOUNTED, 0, GetHitAura());

                    // Triggered spell id dependent on riding skill and zone
                    bool canFly = false;
                    uint32 map = GetVirtualMapForMapAndZone(target->GetMapId(), target->GetZoneId());
                    if (map == 530 || (map == 571 && target->HasSpell(SPELL_COLD_WEATHER_FLYING)))
                        canFly = true;

                    float x, y, z;
                    target->GetPosition(x, y, z);
                    uint32 l_AreaID = target->GetBaseMap()->GetAreaId(x, y, z);
                    AreaTableEntry const* area = sAreaTableStore.LookupEntry(l_AreaID);
                    if (!area || (canFly && (area->Flags & AREA_FLAG_NO_FLY_ZONE)))
                        canFly = false;

                    uint32 mount = 0;
                    switch (target->GetBaseSkillValue(SKILL_RIDING))
                    {
                        case 0:
                            mount = _mount0;
                            break;
                        case 75:
                            mount = _mount60;
                            break;
                        case 150:
                            mount = _mount100;
                            break;
                        case 225:
                            if (canFly)
                                mount = _mount150;
                            else
                                mount = _mount100;
                            break;
                        case 300:
                            if (canFly)
                                mount = _mount280;
                            else
                                mount = _mount100;
                            break;
                        case 375:
                            if (canFly)
                                mount = _mount310;
                            else
                                mount = _mount100;
                            break;
                        default:
                            break;
                    }

                    if (mount)
                    {
                        PreventHitAura();
                        target->CastSpell(target, mount, true);
                    }
                }
            }

            void Register()
            {
                 OnEffectHitTarget += SpellEffectFn(spell_gen_mount_SpellScript::HandleMount, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }

        private:
            uint32 _mount0;
            uint32 _mount60;
            uint32 _mount100;
            uint32 _mount150;
            uint32 _mount280;
            uint32 _mount310;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_mount_SpellScript(_mount0, _mount60, _mount100, _mount150, _mount280, _mount310);
        }

    private:
        uint32 _mount0;
        uint32 _mount60;
        uint32 _mount100;
        uint32 _mount150;
        uint32 _mount280;
        uint32 _mount310;
};

enum FoamSword
{
    ITEM_FOAM_SWORD_GREEN = 45061,
    ITEM_FOAM_SWORD_PINK = 45176,
    ITEM_FOAM_SWORD_BLUE = 45177,
    ITEM_FOAM_SWORD_RED = 45178,
    ITEM_FOAM_SWORD_YELLOW = 45179,

    SPELL_BONKED = 62991,
    SPELL_FOAM_SWORD_DEFEAT = 62994,
    SPELL_ON_GUARD = 62972
};

class spell_gen_upper_deck_create_foam_sword: public SpellScriptLoader
{
public:
    spell_gen_upper_deck_create_foam_sword() : SpellScriptLoader("spell_gen_upper_deck_create_foam_sword") { }

    class spell_gen_upper_deck_create_foam_sword_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_upper_deck_create_foam_sword_SpellScript);

        void HandleScript(SpellEffIndex effIndex)
        {
            if (Player* player = GetHitPlayer())
            {
                static uint32 const itemId[5] = { ITEM_FOAM_SWORD_GREEN, ITEM_FOAM_SWORD_PINK, ITEM_FOAM_SWORD_BLUE, ITEM_FOAM_SWORD_RED, ITEM_FOAM_SWORD_YELLOW };
                // player can only have one of these items
                for (uint8 i = 0; i < 5; ++i)
                {
                    if (player->HasItemCount(itemId[i], 1, true))
                        return;
                }

                CreateItem(effIndex, itemId[urand(0, 4)]);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_upper_deck_create_foam_sword_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_upper_deck_create_foam_sword_SpellScript();
    }
};

class spell_gen_bonked: public SpellScriptLoader
{
public:
    spell_gen_bonked() : SpellScriptLoader("spell_gen_bonked") { }

    class spell_gen_bonked_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_bonked_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (Player* target = GetHitPlayer())
            {
                Aura const* aura = GetHitAura();
                if (!(aura && aura->GetStackAmount() == 3))
                    return;

                target->CastSpell(target, SPELL_FOAM_SWORD_DEFEAT, true);
                target->RemoveAurasDueToSpell(SPELL_BONKED);

                if (Aura const* aura = target->GetAura(SPELL_ON_GUARD))
                {
                    if (Item* item = target->GetItemByGuid(aura->GetCastItemGUID()))
                        target->DestroyItemCount(item->GetEntry(), 1, true);
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_bonked_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_bonked_SpellScript();
    }
};

// Gift of the Naaru - 59548 or 59547 or 59545 or 59544 or 59543 or 59542 or 121093 or 28880
class spell_gen_gift_of_the_naaru: public SpellScriptLoader
{
    public:
        spell_gen_gift_of_the_naaru() : SpellScriptLoader("spell_gen_gift_of_the_naaru") { }

        class spell_gen_gift_of_the_naaru_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_gift_of_the_naaru_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (!GetCaster())
                    return;

                amount = GetCaster()->CountPctFromMaxHealth(4);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_gift_of_the_naaru_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_gift_of_the_naaru_AuraScript();
        }
};

enum eRunningWild
{
    MountDisplayId   = 73200,
    SpellAlteredForm = 97709
};

class spell_gen_running_wild: public SpellScriptLoader
{
    public:
        spell_gen_running_wild() : SpellScriptLoader("spell_gen_running_wild") { }

        class spell_gen_running_wild_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_running_wild_AuraScript);

            void HandleMount(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                target->Mount(eRunningWild::MountDisplayId, 0, 0);

                // cast speed aura
                if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(aurEff->GetAmount()))
                    target->CastSpell(target, mountCapability->SpeedModSpell, TRIGGERED_FULL_MASK);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_running_wild_AuraScript::HandleMount, EFFECT_1, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_gen_running_wild_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_running_wild_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(eRunningWild::SpellAlteredForm))
                    return false;
                return true;
            }

            bool Load()
            {
                // Definitely not a good thing, but currently the only way to do something at cast start
                // Should be replaced as soon as possible with a new hook: BeforeCastStart
                GetCaster()->CastSpell(GetCaster(), eRunningWild::SpellAlteredForm, TRIGGERED_FULL_MASK);
                return false;
            }

            void Register()
            {
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_running_wild_AuraScript();
        }

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_running_wild_SpellScript();
        }
};

class spell_gen_two_forms: public SpellScriptLoader
{
    public:
        spell_gen_two_forms() : SpellScriptLoader("spell_gen_two_forms") { }

        class spell_gen_two_forms_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_two_forms_SpellScript);

            SpellCastResult CheckCast()
            {
                if (GetCaster()->isInCombat())
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                // Player cannot transform to human form if he is forced to be worgen for some reason (Darkflight)
                if (GetCaster()->GetAuraEffectsByType(SPELL_AURA_WORGEN_ALTERED_FORM).size() > 1)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleTransform(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();
                PreventHitDefaultEffect(effIndex);
                if (target->HasAuraType(SPELL_AURA_WORGEN_ALTERED_FORM))
                    target->RemoveAurasByType(SPELL_AURA_WORGEN_ALTERED_FORM);
                else    // Basepoints 1 for this aura control whether to trigger transform transition animation or not.
                    target->CastCustomSpell(eRunningWild::SpellAlteredForm, SPELLVALUE_BASE_POINT0, 1, target, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_two_forms_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_gen_two_forms_SpellScript::HandleTransform, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_two_forms_SpellScript();
        }
};

class spell_gen_darkflight: public SpellScriptLoader
{
    public:
        spell_gen_darkflight() : SpellScriptLoader("spell_gen_darkflight") { }

        class spell_gen_darkflight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_darkflight_SpellScript);

            void TriggerTransform()
            {
                GetCaster()->CastSpell(GetCaster(), eRunningWild::SpellAlteredForm, TRIGGERED_FULL_MASK);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_gen_darkflight_SpellScript::TriggerTransform);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_darkflight_SpellScript();
        }
};

// Blood Fury - 20572 or Blood Fury - 33702
class spell_gen_blood_fury: public SpellScriptLoader
{
    public:
        spell_gen_blood_fury() : SpellScriptLoader("spell_gen_blood_fury") { }

        class spell_gen_blood_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_blood_fury_SpellScript);

            void HandleOnHit()
            {
                if (GetCaster()->HasAura(GetSpellInfo()->Id) && GetCaster()->getClass() != CLASS_HUNTER)
                {
                    GetCaster()->RemoveAura(GetSpellInfo()->Id);
                    GetCaster()->CastSpell(GetCaster(), 33697, true);
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_blood_fury_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_blood_fury_SpellScript();
        }
};

#define GOBELING_GUMBO_BURP 42755

// Gobelin Gumbo - 42760
class spell_gen_gobelin_gumbo: public SpellScriptLoader
{
    public:
        spell_gen_gobelin_gumbo() : SpellScriptLoader("spell_gen_gobelin_gumbo") { }

        class spell_gen_gobelin_gumbo_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_gobelin_gumbo_AuraScript);

            uint32 burpTimer;

            bool Load()
            {
                burpTimer = 10000;
                return true;
            }

            void OnUpdate(uint32 diff, AuraEffect* /*aurEff*/)
            {
                if (GetCaster())
                {
                    if (burpTimer <= diff)
                    {
                        burpTimer = 10000;
                        if (roll_chance_i(30))
                            GetCaster()->CastSpell(GetCaster(), GOBELING_GUMBO_BURP, true);
                    }
                    else
                        burpTimer -= diff;
                }
            }

            void Register()
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_gen_gobelin_gumbo_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_gobelin_gumbo_AuraScript();
        }
};

enum SilvermoonPolymorph
{
    NPC_AUROSALIA   = 18744
};

enum magePolymorphForms
{
    SPELL_MAGE_SQUIRREL_FORM                     = 32813,
    SPELL_MAGE_GIRAFFE_FORM                      = 32816,
    SPELL_MAGE_SERPENT_FORM                      = 32817,
    SPELL_MAGE_DRAGONHAWK_FORM                   = 32818,
    SPELL_MAGE_WORGEN_FORM                       = 32819,
    SPELL_MAGE_SHEEP_FORM                        = 32820
};

// TODO: move out of here and rename - not a mage spell
class spell_mage_polymorph_cast_visual: public SpellScriptLoader
{
    public:
        spell_mage_polymorph_cast_visual() : SpellScriptLoader("spell_mage_polymorph_visual") { }

        class spell_mage_polymorph_cast_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mage_polymorph_cast_visual_SpellScript);

            static const uint32 PolymorhForms[6];

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                // check if spell ids exist in db2
                for (uint32 i = 0; i < 6; i++)
                    if (!sSpellMgr->GetSpellInfo(PolymorhForms[i]))
                        return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetCaster()->FindNearestCreature(NPC_AUROSALIA, 30.0f))
                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->CastSpell(target, PolymorhForms[urand(0, 5)], true);
            }

            void Register()
            {
                // add dummy effect spell handler to Polymorph visual
                OnEffectHitTarget += SpellEffectFn(spell_mage_polymorph_cast_visual_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mage_polymorph_cast_visual_SpellScript();
        }
};

const uint32 spell_mage_polymorph_cast_visual::spell_mage_polymorph_cast_visual_SpellScript::PolymorhForms[6] =
{
    SPELL_MAGE_SQUIRREL_FORM,
    SPELL_MAGE_GIRAFFE_FORM,
    SPELL_MAGE_SERPENT_FORM,
    SPELL_MAGE_DRAGONHAWK_FORM,
    SPELL_MAGE_WORGEN_FORM,
    SPELL_MAGE_SHEEP_FORM
};

/// Last Update 6.2.3
/// Hardened Shell - 129787
class spell_gen_hardened_shell: public SpellScriptLoader
{
    public:
        spell_gen_hardened_shell() : SpellScriptLoader("spell_gen_hardened_shell") { }

        class spell_gen_hardened_shell_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_hardened_shell_AuraScript);

            void HandleAfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Target = GetTarget();

                if (l_Target->IsMounted())
                {
                    l_Target->RemoveAurasByType(SPELL_AURA_MOUNTED);
                    l_Target->Dismount();
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_hardened_shell_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_gen_hardened_shell_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_hardened_shell_SpellScript);

            SpellCastResult CheckLevel()
            {
                Unit* l_Target = GetExplTargetUnit();

                if (l_Target == nullptr)
                    return SpellCastResult::SPELL_FAILED_SUCCESS;

                if (l_Target->getLevel() > 94)
                    return SpellCastResult::SPELL_FAILED_HIGHLEVEL;


                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_hardened_shell_SpellScript::CheckLevel);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_hardened_shell_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_hardened_shell_SpellScript();
        }
};

class spell_gen_ds_flush_knockback: public SpellScriptLoader
{
    public:
        spell_gen_ds_flush_knockback() : SpellScriptLoader("spell_gen_ds_flush_knockback") {}

        class spell_gen_ds_flush_knockback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_ds_flush_knockback_SpellScript);

            void HandleOnHit()
            {
                // Here the target is the water spout and determines the position where the player is knocked from
                if (Unit* caster = GetCaster())
                {
                    if (!GetHitUnit())
                        return;

                    if (Player* player = GetHitUnit()->ToPlayer())
                    {
                        float horizontalSpeed = 20.0f + (40.0f - GetCaster()->GetDistance(caster));
                        float verticalSpeed = 8.0f;
                        // This method relies on the Dalaran Sewer map disposition and Water Spout position
                        // What we do is knock the player from a position exactly behind him and at the end of the pipe
                        player->KnockbackFrom(caster->GetPositionX(), player->GetPositionY(), horizontalSpeed, verticalSpeed);
                    }
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_ds_flush_knockback_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_ds_flush_knockback_SpellScript();
        }
};

/// Last Update 6.2.3
/// Orb of Power - 121164 / 121175 / 121176 / 121177
class spell_gen_orb_of_power: public SpellScriptLoader
{
    public:
        spell_gen_orb_of_power() : SpellScriptLoader("spell_gen_orb_of_power") { }

        enum eSpells
        {
            PowerOrb    = 116524
        };

        class spell_gen_orb_of_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_orb_of_power_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                Unit* l_Target = GetTarget();

                if (AuraEffect* l_DamageDone = l_Target->GetAuraEffect(GetSpellInfo()->Id, EFFECT_2))
                {
                    // Max +200% damage done
                    if (l_DamageDone->GetAmount() + 10 >= 100)
                        l_DamageDone->ChangeAmount(100);
                    else
                        l_DamageDone->ChangeAmount(l_DamageDone->GetAmount() + 10);
                }
                if (AuraEffect* l_Healing = l_Target->GetAuraEffect(GetSpellInfo()->Id, EFFECT_0))
                {
                    // Max -90% heal taken
                    if (l_Healing->GetAmount() - 5 <= -90)
                        l_Healing->ChangeAmount(-90);
                    else
                        l_Healing->ChangeAmount(l_Healing->GetAmount() - 5);
                }
                if (AuraEffect* l_DamageTaken = l_Target->GetAuraEffect(GetSpellInfo()->Id, EFFECT_1))
                {
                    // Max +500% damage taken
                    if (l_DamageTaken->GetAmount() + 30 >= 500)
                        l_DamageTaken->ChangeAmount(500);
                    else
                        l_DamageTaken->ChangeAmount(l_DamageTaken->GetAmount() + 30);
                }
            }

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                // Use CastSpell for achievements
                l_Target->CastSpell(l_Target, eSpells::PowerOrb, true);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();

                l_Target->RemoveFlagsAuras();

                l_Target->RemoveAura(eSpells::PowerOrb);
            }


            void OnUpdate(uint32 /*diff*/, AuraEffect* /*aurEff*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                Player* l_Player = l_Caster->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (!l_Player->GetMap()->IsBattlegroundOrArena())
                    l_Player->RemoveAura(GetSpellInfo()->Id);
            }

            void Register()
            {
                OnEffectUpdate += AuraEffectUpdateFn(spell_gen_orb_of_power_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_orb_of_power_AuraScript::OnTick, EFFECT_3, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectApply += AuraEffectApplyFn(spell_gen_orb_of_power_AuraScript::OnApply, EFFECT_3, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_orb_of_power_AuraScript::OnRemove, EFFECT_3, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_orb_of_power_AuraScript();
        }
};

// Touch of Elune - 45529
class PlayerScript_gen_touch_of_elune: public PlayerScript
{
    public:
        PlayerScript_gen_touch_of_elune() :PlayerScript("PlayerScript_Touch_Of_Elune") {}

        enum TouchOfELuneSpell
        {
            Day         = 154796,
            Night       = 154797,
            Racial      = 154748,
            EventNight  = 25 // Id on game_event
        };

        void OnUpdate(Player* p_Player, uint32 /*p_Diff*/)
        {
            if (p_Player->getRace() != Races::RACE_NIGHTELF || !p_Player->HasSpell(TouchOfELuneSpell::Racial))
                return;

            if (sGameEventMgr->IsActiveEvent(TouchOfELuneSpell::EventNight) && !p_Player->HasAura(TouchOfELuneSpell::Night))
            {
                p_Player->RemoveAura(TouchOfELuneSpell::Day);
                p_Player->AddAura(TouchOfELuneSpell::Night, p_Player);
            }
            else if (!p_Player->HasAura(TouchOfELuneSpell::Day) && !sGameEventMgr->IsActiveEvent(TouchOfELuneSpell::EventNight))
            {
                p_Player->RemoveAura(TouchOfELuneSpell::Night);
                p_Player->AddAura(TouchOfELuneSpell::Day, p_Player);
            }
        }
};

/// Greymane Gurubashi gates phasing, called only if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE)) (see the return new PlayerScript_gen_greymane_gurubashi_arena() at the end)
class PlayerScript_gen_greymane_gurubashi_arena : public PlayerScript
{
    public:
        PlayerScript_gen_greymane_gurubashi_arena() :PlayerScript("PlayerScript_gen_greymane_gurubashi_arena") {}

        enum eAreaID
        {
            GurubashiRingBattle     = 2177
        };

        enum eEntry
        {
            TranscendenceSpirit     = 54569
        };

        enum eSpells
        {
            DoorClosedAura          = 134735,
            TranscendenceBuff       = 101643,
            DisplacementBeacon      = 212799,
            FeignDeath              = 5384,
            Shadowmeld              = 58984,
            CloackOfShadow          = 31224,
            Vanish                  = 131361
        };

        void OnEnterInCombat(Player* p_Player) override
        {
            if (p_Player->GetAreaId() != eAreaID::GurubashiRingBattle)
                return;

            /// Phasing to see the gates around the arena
            p_Player->SetInPhase(eSpells::DoorClosedAura, true, true);

            /// Prevent Monk's Transcendence abuse (casting it outside of Battle Ring, go in, and when low life Transfer out)
            if (p_Player->getClass() == Classes::CLASS_MONK)
            {
                for (Unit* l_Summon : p_Player->m_Controlled)
                {
                    if (!l_Summon || l_Summon->GetEntry() != eEntry::TranscendenceSpirit)
                        break;

                    Creature* l_Creature = l_Summon->ToCreature();
                    if (!l_Creature)
                        break;

                    if (l_Creature->GetAreaId() != eAreaID::GurubashiRingBattle)
                    {
                        l_Creature->DespawnOrUnsummon();
                        p_Player->RemoveAura(eSpells::TranscendenceBuff);
                        return;
                    }
                }
            }

            /// Removes Mage's Displacement AT so he can't Blink back IF it was outside of the battle ring
            if (p_Player->getClass() == Classes::CLASS_MAGE)
            {
                AreaTrigger* l_DisplacementBeacon = p_Player->GetLastAreaTrigger(eSpells::DisplacementBeacon);
                if (!l_DisplacementBeacon)
                    return;

                if (l_DisplacementBeacon->GetAreaId() != eAreaID::GurubashiRingBattle)
                    p_Player->RemoveAura(eSpells::DisplacementBeacon);
            }
        }

        void OnLeaveCombat(Player* p_Player) override
        {
            /// Player shouldn't have Gurubashi gates aura removed when they use spells like FeignDeath/Shadowmeld/Shadow step, etc...
            if (p_Player->HasAura(eSpells::FeignDeath) || p_Player->HasAura(eSpells::Shadowmeld) || p_Player->HasAura(eSpells::CloackOfShadow) || p_Player->HasAura(eSpells::Vanish))
                return;

            p_Player->AddDelayedEvent([p_Player]()->void
            {
                p_Player->SetInPhase(eSpells::DoorClosedAura, true, false);
            }, 10000);
        }

        void OnLogin(Player* p_Player) override
        {
            p_Player->SetInPhase(134735, true, false);
        }
};

/// last update : 6.1.2 19802
/// Drums of Fury - 178207, Drums of Rage - 146555, Drums of the Mountain - 230935
class spell_gen_drums_of_fury : public SpellScriptLoader
{
    public:
        spell_gen_drums_of_fury() : SpellScriptLoader("spell_gen_drums_of_fury") { }

        class spell_gen_drums_of_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_drums_of_fury_SpellScript);

            enum eSpells
            {
                Exhausted = 57723,
                Insanity = 95809,
                Sated = 57724,
                TemporalDisplacement = 80354,
                Fatigued = 160455
            };

            void HandleImmunity(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (l_Target->HasAura(eSpells::Exhausted) || l_Target->HasAura(eSpells::Insanity) ||
                    l_Target->HasAura(eSpells::Sated) || l_Target->HasAura(eSpells::TemporalDisplacement) ||
                    l_Target->HasAura(eSpells::Fatigued))
                {
                    PreventHitAura();
                    return;
                }

                if (!l_Target->IsPlayer())
                    return;

                if (InstanceScript* l_Instance = l_Target->GetInstanceScript())
                    l_Instance->HandleBloodlustApplied(l_Target->GetGUID());
            }

            void HandleAfterHit()
            {
                Unit* l_Target = GetHitUnit();

                if (l_Target == nullptr)
                    return;

                if (!l_Target->HasAura(eSpells::Exhausted))
                    l_Target->CastSpell(l_Target, eSpells::Exhausted, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_drums_of_fury_SpellScript::HandleImmunity, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
                OnEffectHitTarget += SpellEffectFn(spell_gen_drums_of_fury_SpellScript::HandleImmunity, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
                AfterHit += SpellHitFn(spell_gen_drums_of_fury_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_drums_of_fury_SpellScript();
        }
};

class spell_gen_selfie_camera : public SpellScriptLoader
{
    public:
        spell_gen_selfie_camera() : SpellScriptLoader("spell_gen_selfie_camera") { }


        class spell_gen_selfie_camera_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_selfie_camera_AuraScript);

            uint64 m_PhotoBinberGUID = 0;

            enum eData
            {
                PhotoBomberNPC  = 91977,
                VisualKit       = 54168
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /* p_Mode */)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (l_Caster->GetTypeId() != TypeID::TYPEID_PLAYER)
                    return;

                l_Caster->ToPlayer()->SendPlaySpellVisualKit(eData::VisualKit, 2, 0);

                std::list<Creature*> l_ListPhotoBomber;

                l_Caster->GetCreatureListWithEntryInGrid(l_ListPhotoBomber, eData::PhotoBomberNPC, 100.0f);

                /// Remove other players Master PhotoBomber
                for (std::list<Creature*>::iterator l_Itr = l_ListPhotoBomber.begin(); l_Itr != l_ListPhotoBomber.end(); l_Itr++)
                {
                    Unit* l_Owner = (*l_Itr)->GetOwner();

                    if (l_Owner != nullptr && l_Owner->GetGUID() == l_Caster->GetGUID() && (*l_Itr)->isSummon())
                    {
                        m_PhotoBinberGUID = (*l_Itr)->GetGUID();
                        break;
                    }
                }
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /* p_Mode */)
            {
                Creature* l_PhotoBomber = ObjectAccessor::FindCreature(m_PhotoBinberGUID);

                if (l_PhotoBomber != nullptr)
                    l_PhotoBomber->DespawnOrUnsummon();

                Unit* l_Caster = GetCaster();

                if (l_Caster == nullptr)
                    return;

                if (l_Caster->GetTypeId() != TypeID::TYPEID_PLAYER)
                    return;

                l_Caster->ToPlayer()->CancelSpellVisualKit(eData::VisualKit);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_selfie_camera_AuraScript::OnApply, EFFECT_0, SPELL_AURA_482, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_selfie_camera_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_482, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_selfie_camera_AuraScript();
        }
};

/// 181883 - S.E.L.F.I.E. Lens Upgrade Kit
class spell_gen_selfie_lens_upgrade_kit : public SpellScriptLoader
{
    enum ItemIDs
    {
        SELFIECameraMkII = 122674
    };

    public:
        /// Constructor
        spell_gen_selfie_lens_upgrade_kit()
            : SpellScriptLoader("spell_gen_selfie_lens_upgrade_kit")
        {

        }

        /// Spell script
        class spell_gen_selfie_lens_upgrade_kit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_selfie_lens_upgrade_kit_SpellScript);

            /// After the spell is caster
            void OnAfterCast()
            {
                if (GetCaster()->ToPlayer())
                    GetCaster()->ToPlayer()->AddItem(ItemIDs::SELFIECameraMkII, 1);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_selfie_lens_upgrade_kit_SpellScript::OnAfterCast);
            }
        };

        /// Get a fresh spell script instance
        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_selfie_lens_upgrade_kit_SpellScript();
        }

};

/// Carrying Seaforium - 52410
class spell_gen_carrying_seaforium : public SpellScriptLoader
{
    public:
        spell_gen_carrying_seaforium() : SpellScriptLoader("spell_gen_carrying_seaforium") { }

        class spell_gen_carrying_seaforium_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_carrying_seaforium_SpellScript);

            enum eBombEntry
            {
                SeaforiumCharge = 190752
            };

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                Position l_Pos = *l_Caster;
                l_Caster->GetRandomNearPosition(l_Pos, 3.0f);

                GameObject* l_GameObject = new GameObject;

                if (!l_GameObject->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), eBombEntry::SeaforiumCharge, l_Caster->GetMap(),
                    l_Caster->GetPhaseMask(), l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ, l_Caster->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 100, GO_STATE_READY))
                {
                    delete l_GameObject;
                    return;
                }

                l_GameObject->SetRespawnTime(35000);
                l_GameObject->SetOwnerGUID(l_Caster->GetGUID());

                l_Caster->GetMap()->AddToMap(l_GameObject);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_carrying_seaforium_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_carrying_seaforium_SpellScript();
        }
};

/// Inherit Master Threat List - 58838 - last update: 5.4.2 (17688)
class spell_gen_inherit_master_threat_list : public SpellScriptLoader
{
    public:
        spell_gen_inherit_master_threat_list() : SpellScriptLoader("spell_inherit_master_threat_list") {}

        class spell_inherit_master_threat_list_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_inherit_master_threat_list_SpellScript);

            void HandleCopy(SpellEffIndex)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (!GetHitUnit())
                     return;

                    if (Creature* l_Target = GetHitUnit()->ToCreature())
                    {
                        l_Target->getThreatManager().clearReferences();
                        std::list<HostileReference*>& l_PlayerThreatManager = l_Caster->getThreatManager().getThreatList();
                        for (HostileReference* l_Threat : l_PlayerThreatManager)
                        {
                            if (Unit* l_Obj = Unit::GetUnit(*l_Target, l_Threat->getUnitGuid()))
                                l_Target->getThreatManager().addThreat(l_Target, l_Threat->getThreat());
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_inherit_master_threat_list_SpellScript::HandleCopy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

    SpellScript* GetSpellScript() const
    {
        return new spell_inherit_master_threat_list_SpellScript;
    }
};

/// Taunt Flag Targeting - 51640 - last update: 6.1.2 19865
class spell_gen_taunt_flag_targeting : public SpellScriptLoader
{
    public:
        spell_gen_taunt_flag_targeting() : SpellScriptLoader("spell_taunt_flag_targeting") {}

        class spell_taunt_flag_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_taunt_flag_targeting_SpellScript);

            Position m_BestTargetPos;
            char const* m_BestTargetName = nullptr;
            std::size_t m_BestTargetNameLength = 0;

            SpellCastResult CheckCast()
            {
                /// Used to improve error message
                bool l_FoundCorpse = false;
                bool l_InFrontOfMe = false;
                if (Player* l_Caster = GetCaster()->ToPlayer())
                {
                    float l_SearchDist = GetSpellInfo()->Effects[SpellEffIndex::EFFECT_0].CalcRadius(l_Caster);

                    std::list<WorldObject*> l_Targets;
                    JadeCore::AllWorldObjectsInRange l_Check(l_Caster, l_SearchDist);
                    JadeCore::WorldObjectListSearcher<JadeCore::AllWorldObjectsInRange> l_Searcher(l_Caster, l_Targets, l_Check);
                    l_Caster->VisitNearbyObject(l_SearchDist, l_Searcher);

                    Position l_CasterPos;
                    l_Caster->GetPosition(&l_CasterPos);

                    for (auto l_It = l_Targets.begin(); l_It != l_Targets.end(); ++l_It)
                    {
                        /// Either we have a corpse, either we have a player
                        if (Corpse* l_Corpse = (*l_It)->ToCorpse())
                        {
                            if (Player* l_Owner = ObjectAccessor::FindPlayer(l_Corpse->GetOwnerGUID()))
                            {
                                if (l_Owner->GetGUID() == l_Caster->GetGUID())
                                    continue;

                                l_FoundCorpse = true;

                                if (!l_Caster->isInFront(l_Corpse)) ///< Only corpses in front of us
                                    continue;

                                l_InFrontOfMe = true;

                                if (m_BestTargetName && m_BestTargetPos.GetExactDistSq(&l_CasterPos) < l_Corpse->GetExactDistSq(&l_CasterPos))
                                    continue;

                                if (l_Caster->GetReactionTo(l_Owner) > REP_NEUTRAL || l_Owner->GetReactionTo(l_Caster) > REP_NEUTRAL) ///< Only enemies corpses
                                    continue;

                                l_Corpse->GetPosition(&m_BestTargetPos);

                                m_BestTargetName = l_Owner->GetName();
                                m_BestTargetNameLength = l_Owner->GetNameLength();
                            }
                        }
                        else if (Player* l_Player = (*l_It)->ToPlayer())
                        {
                            if (l_Player->GetGUID() == l_Caster->GetGUID())
                                continue;

                            if (l_Player->isAlive()) ///< Only corpses
                                continue;

                            l_FoundCorpse = true;

                            if (!l_Caster->isInFront(l_Player))
                                continue;

                            l_InFrontOfMe = true;

                            /// Skips ghosts
                            if (l_Player->HasAuraType(SPELL_AURA_GHOST))
                                continue;

                            /// Is this corpse closer?
                            if (m_BestTargetName && m_BestTargetPos.GetExactDistSq(&l_CasterPos) < l_Player->GetExactDistSq(&l_CasterPos))
                                continue;

                            if (l_Caster->GetReactionTo(l_Player) > REP_NEUTRAL || l_Player->GetReactionTo(l_Caster) > REP_NEUTRAL) ///< Only enemies corpses
                                continue;

                            l_Player->GetPosition(&m_BestTargetPos);

                            m_BestTargetName = l_Player->GetName();
                            m_BestTargetNameLength = l_Player->GetNameLength();
                        }
                    }
                }

                if (!m_BestTargetName)
                {
                    if (l_FoundCorpse)
                        return (l_InFrontOfMe) ? SpellCastResult::SPELL_FAILED_BAD_TARGETS : SpellCastResult::SPELL_FAILED_NOT_INFRONT;
                    else
                    {
                        SetCustomCastResultMessage(SpellCustomErrors::SPELL_CUSTOM_ERROR_NO_NEARBY_CORPSES);
                        return SpellCastResult::SPELL_FAILED_CUSTOM_ERROR;
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCast(SpellEffIndex /*p_Index*/)
            {
                if (Player* l_Caster = GetCaster()->ToPlayer())
                {
                    if (m_BestTargetName)
                    {
                        l_Caster->CastSpell(m_BestTargetPos, 51657, true);
                        l_Caster->AddSpellCooldown(GetSpellInfo()->Id, 0, 60 * IN_MILLISECONDS, true);

                        if (WorldSession* l_Session = l_Caster->GetSession())
                        {
                            char const* l_TauntText = l_Session->GetTrinityString(LangTauntFlag);
                            std::size_t l_TauntTextLength = std::strlen(l_TauntText);

                            std::string l_Text;
                            l_Text.reserve(l_TauntTextLength + m_BestTargetNameLength);
                            l_Text.append(l_TauntText, l_TauntTextLength);
                            l_Text.append(m_BestTargetName);

                            WorldPacket l_Data;
                            /// No specific target needed
                            l_Caster->BuildPlayerChat(&l_Data, 0, CHAT_MSG_EMOTE, l_Text, LANG_UNIVERSAL);
                            l_Session->SendPacket(&l_Data);
                        }
                    }
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_taunt_flag_targeting_SpellScript::CheckCast);
                OnEffectLaunch += SpellEffectFn(spell_taunt_flag_targeting_SpellScript::HandleCast, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_taunt_flag_targeting_SpellScript;
        }
};

/// last update : 6.1.2 19802
/// Sword Technique - 177189
class spell_gen_sword_technique: public SpellScriptLoader
{
    public:
        spell_gen_sword_technique() : SpellScriptLoader("spell_gen_sword_technique") { }

        class spell_gen_sword_technique_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_sword_technique_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                if (GetCaster() == nullptr)
                    return;

                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if ((l_Player->GetMap() && l_Player->GetMap()->IsBattlegroundOrArena()) || l_Player->IsInPvPCombat())
                    p_Amount -= CalculatePct(p_Amount, 40);
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_sword_technique_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_STAT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_sword_technique_AuraScript();
        }
};

/// Faction check for spells (seems to cause problems when applied to every spells)
class spell_gen_check_faction : public SpellScriptLoader
{
    public:
        spell_gen_check_faction() : SpellScriptLoader("spell_gen_check_faction") {}

        class spell_gen_check_faction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_check_faction_SpellScript);

            SpellCastResult CheckFaction()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    uint64 l_RaceMask = l_Caster->getRaceMask();
                    uint32 l_AttributeEx7 = GetSpellInfo()->AttributesEx7;

                    if ((l_AttributeEx7 & SpellAttr7::SPELL_ATTR7_ALLIANCE_ONLY && ((l_RaceMask & (uint64)RACEMASK_ALLIANCE) == 0)) ||
                        (l_AttributeEx7 & SpellAttr7::SPELL_ATTR7_HORDE_ONLY && ((l_RaceMask & (uint64)RACEMASK_HORDE) == 0)))
                        return SpellCastResult::SPELL_FAILED_SPELL_UNAVAILABLE;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_check_faction_SpellScript::CheckFaction);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_check_faction_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
/// Stoneform - 20594
class spell_gen_stoneform_dwarf_racial : public SpellScriptLoader
{
    public:
        spell_gen_stoneform_dwarf_racial() : SpellScriptLoader("spell_gen_stoneform_dwarf_racial") {}

        class spell_gen_stoneform_dwarf_racial_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_stoneform_dwarf_racial_SpellScript);

            enum eSpell
            {
                PvPTrinket          = 42292,
                HonorableMedallion  = 195710,
                GladiatorsMedallion = 208683
            };

            void HandleAfterCast()
            {
                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    /// Patch note 6.1 : "Stoneform when used, now triggers a 30-second shared cooldown with other PvP trinkets that breaks crowd-control effects."
                    l_Player->AddSpellCooldown(eSpell::PvPTrinket, 0, 30 * TimeConstants::IN_MILLISECONDS);

                    if (Item* l_FirstTrinket = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_TRINKET1))
                    {
                        for (uint8 l_I = 0; l_I < MAX_ITEM_PROTO_SPELLS; ++l_I)
                        {
                            _Spell const& l_SpellData = l_FirstTrinket->GetTemplate()->Spells[l_I];

                            /// No spell or not which one we search
                            if (!l_SpellData.SpellId || l_SpellData.SpellId != eSpell::PvPTrinket)
                                continue;

                            l_Player->ApplyEquipCooldown(l_FirstTrinket);
                            break;
                        }
                    }

                    if (Item* l_SecondTrinket = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_TRINKET2))
                    {
                        for (uint8 l_I = 0; l_I < MAX_ITEM_PROTO_SPELLS; ++l_I)
                        {
                            _Spell const& l_SpellData = l_SecondTrinket->GetTemplate()->Spells[l_I];

                            /// No spell or not which one we search
                            if (!l_SpellData.SpellId || l_SpellData.SpellId != eSpell::PvPTrinket)
                                continue;

                            l_Player->ApplyEquipCooldown(l_SecondTrinket);
                            break;
                        }
                    }

                    if (l_Player->HasSpell(eSpell::HonorableMedallion))
                        l_Player->AddSpellCooldown(eSpell::HonorableMedallion, 0, GetSpellInfo()->CategoryRecoveryTime, true);

                    if (l_Player->HasSpell(eSpell::GladiatorsMedallion))
                        l_Player->AddSpellCooldown(eSpell::GladiatorsMedallion, 0, GetSpellInfo()->CategoryRecoveryTime, true);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_stoneform_dwarf_racial_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_stoneform_dwarf_racial_SpellScript();
        }
};

/// last update : 6.1.2 19802
/// Touch of the Grave - 5227
class spell_dru_touch_of_the_grave : public SpellScriptLoader
{
    public:
        spell_dru_touch_of_the_grave() : SpellScriptLoader("spell_dru_touch_of_the_grave") { }

        class spell_dru_touch_of_the_grave_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dru_touch_of_the_grave_AuraScript);

            enum eSpells
            {
                TouchoftheGraveEffect = 127802
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Attacker = p_EventInfo.GetDamageInfo()->GetActor();
                Unit* l_Victim = p_EventInfo.GetDamageInfo()->GetTarget();
                if (l_Attacker == nullptr || l_Victim == nullptr)
                    return;

                if (l_Attacker->GetGUID() == l_Victim->GetGUID() || !l_Attacker->ToPlayer())
                    return;

                if (l_Attacker->ToPlayer()->HasSpellCooldown(eSpells::TouchoftheGraveEffect))
                    return;

                /// Can proc just on damage spell, also check for absorbed damage, because all damage can be absorbed but it's still damage spell
                if (p_EventInfo.GetDamageInfo() && p_EventInfo.GetDamageInfo()->GetAmount() == 0 && p_EventInfo.GetDamageInfo()->GetAbsorb() == 0)
                    return;

                int32 l_Spell = l_Attacker->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
                int32 l_ApMelee = l_Attacker->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
                int32 l_ApRange = l_Attacker->GetTotalAttackPowerValue(WeaponAttackType::RangedAttack);

                int32 l_Damage = std::max(std::max(l_ApMelee, l_ApRange), l_Spell);

                if (Player* l_Player = l_Attacker->ToPlayer())
                {
                    float l_VersaMod = 1.0f;
                    AddPct(l_VersaMod, l_Player->GetRatingBonusValue(CR_VERSATILITY_DAMAGE_DONE) + l_Player->GetTotalAuraModifier(SPELL_AURA_MOD_VERSATILITY_PCT));
                    l_Damage *= l_VersaMod;
                }

                l_Attacker->CastCustomSpell(eSpells::TouchoftheGraveEffect, SpellValueMod::SPELLVALUE_BASE_POINT0, l_Damage, l_Victim, true);
                l_Attacker->ToPlayer()->AddSpellCooldown(eSpells::TouchoftheGraveEffect, 0, 15 * IN_MILLISECONDS, true);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_dru_touch_of_the_grave_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dru_touch_of_the_grave_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Remove Rigor Mortis - 73523 on Fresh Out Of The Grave quest acceptation (24959)
class PlayerScript_gen_remove_rigor_mortis : public PlayerScript
{
    public:
        PlayerScript_gen_remove_rigor_mortis() :PlayerScript("PlayerScript_gen_remove_rigor_mortis") {}

        enum Constants
        {
            QUEST_FRESH_OUT_OF_THE_GRAVE = 24959,
            SPELL_RIGOR_MORTIS = 73523
        };

        void OnQuestAccept(Player* p_Player, Quest const* p_Quest) override
        {
            if (p_Quest->GetQuestId() == Constants::QUEST_FRESH_OUT_OF_THE_GRAVE)
                p_Player->RemoveAura(Constants::SPELL_RIGOR_MORTIS);
        }
};

/// last update : 6.1.2 19802
/// Savage Fortitude - 181706
class spell_gen_savage_fortitude : public SpellScriptLoader
{
    public:
        spell_gen_savage_fortitude() : SpellScriptLoader("spell_gen_savage_fortitude") { }

        class spell_gen_savage_fortitude_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_savage_fortitude_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32& p_Amount, bool& /*canBeRecalculated*/)
            {
                p_Amount *= 10;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_savage_fortitude_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH_2);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_savage_fortitude_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Mark of Warsong - 159675
class spell_gen_mark_of_warsong : public SpellScriptLoader
{
    public:
        spell_gen_mark_of_warsong() : SpellScriptLoader("spell_gen_mark_of_warsong") { }

        class spell_gen_mark_of_warsong_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_mark_of_warsong_AuraScript);

            void OnApply(AuraEffect const* p_AurEff, AuraEffectHandleModes /*mode*/)
            {
                p_AurEff->GetBase()->ModStackAmount(9);
            }

            void OnPeriodic(AuraEffect const* p_AurEff)
            {
                if (p_AurEff->GetBase()->GetStackAmount() > 1)
                    p_AurEff->GetBase()->SetStackAmount(p_AurEff->GetBase()->GetStackAmount() - 1);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_mark_of_warsong_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_RATING, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_mark_of_warsong_AuraScript::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_mark_of_warsong_AuraScript();
        }
};

/// last update : 6.1.2 19802
/// Elixir of Wandering Spirits - 147412
class spell_gen_elixir_of_wandering_spirits : public SpellScriptLoader
{
public:
    spell_gen_elixir_of_wandering_spirits() : SpellScriptLoader("spell_gen_elixir_of_wandering_spirits") { }

    class spell_gen_elixir_of_wandering_spirits_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_elixir_of_wandering_spirits_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {

            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            const uint8 l_AmountOfModels = 8;
            /// Array of all spells that change model for this item
            static uint32 const WanderingSpiritsMorphs[l_AmountOfModels] = { 147402, 147403, 147405, 147406, 147407, 147409, 147410, 147411 };

            /// Remove previus auras if have, to prevent usebug with many auras
            for (uint8 l_I = 0; l_I < l_AmountOfModels; l_I++)
                l_Caster->RemoveAura(WanderingSpiritsMorphs[l_I]);

            l_Caster->CastSpell(l_Caster, WanderingSpiritsMorphs[urand(0, 7)], true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_elixir_of_wandering_spirits_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_elixir_of_wandering_spirits_SpellScript();
    }
};

/// Last Update 6.2.3
/// Shadowmeld - 58984
class spell_gen_shadowmeld : public SpellScriptLoader
{
    public:
        spell_gen_shadowmeld() : SpellScriptLoader("spell_gen_shadowmeld") { }

        class spell_gen_shadowmeld_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_shadowmeld_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                /// 6.1 Hotfixes: March 23 - Should drop the character from pvp combat
                Player* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (l_Player->IsInPvPCombat())
                    l_Player->SetInPvPCombat(false);

                /// Shadowmeld must remove combat
                l_Player->CombatStop();
                l_Player->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);
                l_Player->InterruptNonMeleeSpells(true);

                auto l_IsFightingWithBoss = l_Player->IsFightingWithDungeonBoss();

                if (l_IsFightingWithBoss.first)
                {
                    if (l_IsFightingWithBoss.second)
                        l_Player->getHostileRefManager().deleteReferences();
                }
                else
                    l_Player->getHostileRefManager().deleteReferences();
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (InstanceScript* l_InstanceScript = l_Caster->GetInstanceScript())
                    {
                        if (l_InstanceScript->IsEncounterInProgress())
                            l_Caster->SetInCombatState(false);
                    }
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_shadowmeld_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_TOTAL_THREAT, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_shadowmeld_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_MOD_TOTAL_THREAT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_shadowmeld_AuraScript();
        }

        class spell_gen_shadowmeld_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_shadowmeld_SpellScript);

            void HandleBeforeCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_DAMAGE, SpellSchoolMask::SPELL_SCHOOL_MASK_ALL, true);

                for (uint32 l_Itr = 0; l_Itr < Mechanics::MAX_MECHANIC; l_Itr++)
                    l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, l_Itr, true);

            }


            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->AddDelayedEvent([l_Caster]() -> void
                {
                    l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_DAMAGE, SpellSchoolMask::SPELL_SCHOOL_MASK_ALL, false);

                    for (uint32 l_Itr = 0; l_Itr < Mechanics::MAX_MECHANIC; l_Itr++)
                        l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, l_Itr, false);

                }, 400);
            }

            void Register()
            {
                OnPrepare += SpellOnPrepareFn(spell_gen_shadowmeld_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_gen_shadowmeld_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_shadowmeld_SpellScript();
        }
};

enum DraenicPhilosophers
{
    ForceAura       = 60229,
    AgilityAura     = 60233,
    IntellectAura   = 60234
};

static uint32 const g_DraenicAuras[3] =
{
    DraenicPhilosophers::ForceAura,
    DraenicPhilosophers::AgilityAura,
    DraenicPhilosophers::IntellectAura
};

static Stats const g_DraenicStats[3] =
{
    STAT_STRENGTH,
    STAT_AGILITY,
    STAT_INTELLECT
};

/// last update : 6.1.2 19802
/// Draenic Philosopher's Stone - 157136
class spell_gen_draenic_philosophers : public SpellScriptLoader
{
    public:
        spell_gen_draenic_philosophers() : SpellScriptLoader("spell_gen_draenic_philosophers") { }

        class spell_gen_draenic_philosophers_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_draenic_philosophers_AuraScript);

            int32 m_Value = 1414;

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Player* l_Player = GetTarget()->ToPlayer();
                Stats l_MaxStatValue = STAT_STRENGTH;

                if (l_Player->HasSpellCooldown(GetSpellInfo()->Id))
                    return;

                for (int8 l_I = 0; l_I < 3; l_I++)
                {
                    if (l_Player->GetStat(g_DraenicStats[l_I]) >= l_Player->GetStat(g_DraenicStats[l_MaxStatValue]))
                        l_MaxStatValue = (Stats)l_I;
                }

                /// Can proc just on damage spell, also check for absorbed damage, because all damage can be absorbed but it's still damage spell
                if (p_EventInfo.GetDamageInfo() && p_EventInfo.GetDamageInfo()->GetAmount() == 0 && p_EventInfo.GetDamageInfo()->GetAbsorb() == 0)
                    return;

                l_Player->AddSpellCooldown(GetSpellInfo()->Id, 0, 55 * IN_MILLISECONDS, true);

                l_Player->CastSpell(l_Player, g_DraenicAuras[l_MaxStatValue], true);
                if (AuraEffect* l_DreanicAura = l_Player->GetAuraEffect(g_DraenicAuras[l_MaxStatValue], EFFECT_0))
                    l_DreanicAura->ChangeAmount(m_Value);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_gen_draenic_philosophers_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_draenic_philosophers_AuraScript();
        }
};

/// last update : 6.1.2
/// Jard's Peculiar Energy Source - 143743
class spell_gen_jards_peculiar_energy_source : public SpellScriptLoader
{
    public:
        spell_gen_jards_peculiar_energy_source() : SpellScriptLoader("spell_gen_jards_peculiar_energy_source") { }

        class spell_gen_jards_peculiar_energy_source_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_jards_peculiar_energy_source_SpellScript);

            enum eSpells
            {
                SkyGolem                    = 139192,
                RascalBot                   = 143714,
                Pierre                      = 139196,
                AdvancedRefrigerationUnit   = 139197,
                JardPeculiarenergySource    = 139176
            };

            void HandleOnHit()
            {
                Player* l_Player = GetCaster()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                l_Player->learnSpell(eSpells::SkyGolem, false);
                l_Player->learnSpell(eSpells::RascalBot, false);
                l_Player->learnSpell(eSpells::Pierre, false);
                l_Player->learnSpell(eSpells::AdvancedRefrigerationUnit, false);
                l_Player->learnSpell(eSpells::JardPeculiarenergySource, false);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_jards_peculiar_energy_source_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_jards_peculiar_energy_source_SpellScript();
        }
};

/// last update : 6.1.2
/// Celestial Cloth and Its Uses - 143626
class spell_gen_celestial_cloth_and_its_uses : public SpellScriptLoader
{
public:
    spell_gen_celestial_cloth_and_its_uses() : SpellScriptLoader("spell_gen_celestial_cloth_and_its_uses") { }

    class spell_gen_celestial_cloth_and_its_uses_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_celestial_cloth_and_its_uses_SpellScript);

        enum eSpells
        {
            CelestialCloth              = 143011,
            AcceleratedCelestialCloth   = 146925,
            BeltOfTheNightSky           = 142964,
            LeggingsOfTheNightSky       = 142955,
            WhiteCloudBelt              = 142960,
            WhiteCloudLeggings          = 142951
        };

        void HandleOnHit()
        {
            Player* l_Player = GetCaster()->ToPlayer();

            if (l_Player == nullptr)
                return;

            l_Player->learnSpell(eSpells::CelestialCloth, false);
            l_Player->learnSpell(eSpells::AcceleratedCelestialCloth, false);
            l_Player->learnSpell(eSpells::BeltOfTheNightSky, false);
            l_Player->learnSpell(eSpells::LeggingsOfTheNightSky, false);
            l_Player->learnSpell(eSpells::WhiteCloudBelt, false);
            l_Player->learnSpell(eSpells::WhiteCloudLeggings, false);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_gen_celestial_cloth_and_its_uses_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_celestial_cloth_and_its_uses_SpellScript();
    }
};

/// last update : 6.1.2
/// Hardened Magnificent Hide and Its Uses - 143644
class spell_gen_hardened_magnificient_hide_and_its_uses : public SpellScriptLoader
{
public:
    spell_gen_hardened_magnificient_hide_and_its_uses() : SpellScriptLoader("spell_gen_hardened_magnificient_hide_and_its_uses") { }

    class spell_gen_hardened_magnificient_hide_and_its_uses_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_hardened_magnificient_hide_and_its_uses_SpellScript);

        enum eSpells
        {
            HardenMagnificentHide               = 142976,
            AcceleratedHardenMagnificentHide    = 146923,
            GorgeStalkerBelt                    = 142966,
            GorgeStalkerLegplates               = 142957,
            KrasariProwlerBelt                  = 142962,
            KrasariProwlerBritches              = 142953,
            PennyroyalBelt                      = 142961,
            PennyroyalLeggings                  = 142952,
            SnowLilyBelt                        = 142965,
            SnowLilyBritches                    = 142956
        };

        void HandleOnHit()
        {
            Player* l_Player = GetCaster()->ToPlayer();

            if (l_Player == nullptr)
                return;

            l_Player->learnSpell(eSpells::HardenMagnificentHide, false);
            l_Player->learnSpell(eSpells::AcceleratedHardenMagnificentHide, false);
            l_Player->learnSpell(eSpells::GorgeStalkerBelt, false);
            l_Player->learnSpell(eSpells::GorgeStalkerLegplates, false);
            l_Player->learnSpell(eSpells::KrasariProwlerBelt, false);
            l_Player->learnSpell(eSpells::KrasariProwlerBritches, false);
            l_Player->learnSpell(eSpells::PennyroyalBelt, false);
            l_Player->learnSpell(eSpells::PennyroyalLeggings, false);
            l_Player->learnSpell(eSpells::SnowLilyBelt, false);
            l_Player->learnSpell(eSpells::SnowLilyBritches, false);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_gen_hardened_magnificient_hide_and_its_uses_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_hardened_magnificient_hide_and_its_uses_SpellScript();
    }
};

/// Power handler
/// Reset timer to correctly start decreasing power at 10 sec
class spell_gen_power_handler : public PlayerScript
{
    public:
        spell_gen_power_handler() : PlayerScript("spell_gen_power_handler") {}

        void OnEnterInCombat(Player* p_Player)
        {
            p_Player->ResetAllPowerCombatTimers();
        }

        void OnLeaveCombat(Player* p_Player)
        {
            p_Player->ResetAllPowerCombatTimers();
        }
};

/// Drop Fish - 202103
/// Update 7.1.5 Build 23420
class spell_gen_drop_fish : public SpellScriptLoader
{
    public:

        spell_gen_drop_fish() : SpellScriptLoader("spell_gen_drop_fish") { }

        class spell_gen_drop_fish_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_drop_fish_SpellScript);

            enum eItems
            {
                SeerspinePuffer = 133738
            };

            void HandleEffectHit(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (l_Player->AddItem(eItems::SeerspinePuffer, 1) != nullptr)
                    {
                        l_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_LOOT_ITEM, eItems::SeerspinePuffer, 1);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_drop_fish_SpellScript::HandleEffectHit, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_CREATE_LOOT);
            }

        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_drop_fish_SpellScript();
        }
};

/// In Ship - 206024
/// Update 7.1.5 Build 23420
class spell_gen_in_ship : public SpellScriptLoader
{
    public:
        spell_gen_in_ship() : SpellScriptLoader("spell_gen_in_ship") { }

        class spell_gen_in_ship_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_in_ship_AuraScript);

            enum eNpcs
            {
                AllianceShip = 103950,
                HordeShip    = 104055,
                Ghostship    = 105711
            };

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                if (Creature* l_Creature = l_Caster->GetVehicleCreatureBase())
                {
                    if (l_Creature->GetEntry() == eNpcs::AllianceShip ||
                        l_Creature->GetEntry() == eNpcs::HordeShip ||
                        l_Creature->GetEntry() == eNpcs::Ghostship)
                    {
                        l_Caster->ExitVehicle();
                        l_Creature->DespawnOrUnsummon(1 * IN_MILLISECONDS);
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_in_ship_AuraScript::OnRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_in_ship_AuraScript();
        }
};

/// The script is used to prevent using spells (with 2+ charges) twice in a row.
/// Called by Shadow Dance - 185313, Charge - 100, Intercept - 198304
/// Called by Fel Rush - 195072
/// Update 7.3.5 Build 26365
class spell_gen_spells_with_charges_checks : public SpellScriptLoader
{
    public:
        spell_gen_spells_with_charges_checks() : SpellScriptLoader("spell_gen_spells_with_charges_checks") { }

        class spell_gen_spells_with_charges_checks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spells_with_charges_checks_SpellScript);

            enum eSpells
            {
                ShadowDance = 185313,
                Charge      = 100,
                Intercept   = 198304,
                FelRush     = 195072,
                FelRushAura = 197922
            };

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                if (GetSpellInfo()->Id == eSpells::ShadowDance)
                {
                    /// Prevent using Shadow Dance if Rogue already has the aura.
                    if (l_Caster->HasAura(eSpells::ShadowDance))
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }
                else if (GetSpellInfo()->Id == eSpells::Charge || GetSpellInfo()->Id == eSpells::Intercept)
                {
                    /// Prevent using Charge if Warrior is charging.
                    auto l_MotionType = l_Caster->GetMotionMaster()->GetMotionSlotType(MovementSlot::MOTION_SLOT_CONTROLLED);
                    if (l_MotionType != MovementGeneratorType::NULL_MOTION_TYPE &&
                        l_MotionType != MovementGeneratorType::DISTRACT_MOTION_TYPE) ///< The check is taken from MotionMaster::MoveCharge
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }
                else if (GetSpellInfo()->Id == eSpells::FelRush)
                {
                    /// Prevent using Fel Rush if DH already has the aura.
                    if (l_Caster->HasAura(eSpells::FelRushAura))
                        return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_spells_with_charges_checks_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_spells_with_charges_checks_SpellScript();
        }
};

/// Alchemist's Flask - 105617
enum alchemists_effects
{
    ALCHEMISTS_STRENGTH = 79638,
    ALCHEMISTS_AGILITY = 79639,
    ALCHEMISTS_INTELLECT = 79640
};

class spell_gen_alchemists_flask : public SpellScriptLoader
{
    public:
        spell_gen_alchemists_flask() : SpellScriptLoader("spell_gen_alchemists_flask") { }

        class spell_gen_alchemists_flask_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_alchemists_flask_SpellScript);

            void HandleOnHit()
            {
                if (!GetCaster())
                    return;

                if (!GetHitUnit())
                    return;

                if (Player* _player = GetCaster()->ToPlayer())
                {
                    int32 agility = _player->GetTotalStatValue(STAT_AGILITY);
                    int32 strength = _player->GetTotalStatValue(STAT_STRENGTH);
                    int32 intellect = _player->GetTotalStatValue(STAT_INTELLECT);

                    if (intellect > agility && intellect > strength)
                        _player->CastSpell(_player, ALCHEMISTS_INTELLECT, true);
                    else if (agility > intellect && agility > strength)
                        _player->CastSpell(_player, ALCHEMISTS_AGILITY, true);
                    else
                        _player->CastSpell(_player, ALCHEMISTS_STRENGTH, true);
                }
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_alchemists_flask_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_alchemists_flask_SpellScript();
        }
};

/// Potion of Illusion - 80265
class spell_gen_potion_of_illusion : public SpellScriptLoader
{
    public:
        spell_gen_potion_of_illusion() : SpellScriptLoader("spell_gen_potion_of_illusion") { }

        class spell_gen_potion_of_illusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_potion_of_illusion_SpellScript);

            enum eSpells
            {
                Illusion = 80396
            };

            void HandleOnHit()
            {
                Unit* l_Caster = GetCaster();

                l_Caster->CastSpell(l_Caster, eSpells::Illusion, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_potion_of_illusion_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_potion_of_illusion_SpellScript();
        }
};

/// last update : 6.1.2
/// Hemet's Heartseeker - 173286, Megawatt Filament - 156059, Oglethorpe's Missile Splitter - 156052
class spell_gen_inge_trigger_enchant : public SpellScriptLoader
{
    public:
        spell_gen_inge_trigger_enchant() : SpellScriptLoader("spell_gen_inge_trigger_enchant") { }

        class spell_gen_inge_trigger_enchant_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_inge_trigger_enchant_AuraScript);

            enum eSpells
            {
                HemetsHeartseekerAura   = 173286,
                HemetsHeartseeker       = 173288,
                MegawattFilamentAura    = 156059,
                MegawattFilament        = 156060,
                OglethorpesMissileAura  = 156052,
                OglethorpesMissile      = 156055
            };

            void OnProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                DamageInfo* l_DamageInfo = p_EventInfo.GetDamageInfo();

                if (l_DamageInfo == nullptr)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Unit* l_Caster = l_DamageInfo->GetActor();

                if (l_Target == nullptr || l_Caster == nullptr)
                    return;

                uint32 l_WeaponSpeed = l_Caster->GetAttackTime(WeaponAttackType::RangedAttack);
                float l_Chance = l_Caster->GetPPMProcChance(l_WeaponSpeed, 1.55f, GetSpellInfo());

                if (!roll_chance_f(l_Chance))
                    return;

                switch (GetSpellInfo()->Id)
                {
                case eSpells::HemetsHeartseekerAura:
                    l_Caster->CastSpell(l_Caster, eSpells::HemetsHeartseeker, true);
                    break;
                case eSpells::MegawattFilamentAura:
                    l_Caster->CastSpell(l_Caster, eSpells::MegawattFilament, true);
                    break;
                case eSpells::OglethorpesMissileAura:
                    l_Caster->CastSpell(l_Caster, eSpells::OglethorpesMissile, true);
                    break;
                default:
                    break;
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_gen_inge_trigger_enchant_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_inge_trigger_enchant_AuraScript();
        }
};

/// last update : 6.1.2
/// Mark of the Thunderlord - 159234
class spell_gen_mark_of_thunderlord : public SpellScriptLoader
{
    public:
        spell_gen_mark_of_thunderlord() : SpellScriptLoader("spell_gen_mark_of_thunderlord") { }

        class spell_gen_mark_of_thunderlord_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_mark_of_thunderlord_AuraScript);

            void OnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
            {
                AuraEffect* p_NotConstAurEff = p_AurEff->GetBase()->GetEffect(EFFECT_1);
                if (!p_NotConstAurEff)
                    return;

                uint8 l_TimesIncreasedTime = p_AurEff->GetAmount();

                if (l_TimesIncreasedTime > 2)
                    return;

                PreventDefaultAction();

                if (!(p_EventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT))
                    return;

                if (!p_AurEff->GetBase()->GetDuration())
                    return;

                p_AurEff->GetBase()->SetDuration(p_AurEff->GetBase()->GetDuration() + 2 * IN_MILLISECONDS);
                l_TimesIncreasedTime++;
                p_NotConstAurEff->SetAmount(l_TimesIncreasedTime);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_gen_mark_of_thunderlord_AuraScript::OnProc, EFFECT_1, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_mark_of_thunderlord_AuraScript();
        }
};

/// Last Update 6.2.3
/// Iron Horde Pirate Costume - 173956
class spell_generic_iron_horde_pirate_costume : public SpellScriptLoader
{
    public:
        spell_generic_iron_horde_pirate_costume() : SpellScriptLoader("spell_generic_iron_horde_pirate_costume") { }

        class spell_generic_iron_horde_pirate_costume_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_generic_iron_horde_pirate_costume_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();

                if (l_Caster->getGender() == GENDER_MALE)
                    l_Caster->CastSpell(l_Caster, 173958, true);
                else
                    l_Caster->CastSpell(l_Caster, 173959, true);

            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_generic_iron_horde_pirate_costume_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_generic_iron_horde_pirate_costume_SpellScript();
        }
};

/// last update : 6.2.3
/// Kilrogg's Dead Eye - 184762
class spell_gen_kilroggs_dead_eye : public SpellScriptLoader
{
    public:
        spell_gen_kilroggs_dead_eye() : SpellScriptLoader("spell_gen_kilroggs_dead_eye") { }

        class spell_gen_kilroggs_dead_eye_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_kilroggs_dead_eye_AuraScript);

            void OnAbsorb(AuraEffect* p_AurEff, DamageInfo& p_DmgInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Target = GetCaster();
                if (l_Target == nullptr)
                    return;

                float l_Multiplier = (p_AurEff->GetBase()->GetEffect(EFFECT_1)->GetAmount() / 100) * ((100.0f - l_Target->GetHealthPct()) / 100.0f);
                p_AbsorbAmount = CalculatePct(p_DmgInfo.GetAmount(), l_Multiplier);
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_kilroggs_dead_eye_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_kilroggs_dead_eye_AuraScript::OnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_kilroggs_dead_eye_AuraScript();
        }
};

/// Last Update 6.2.3
/// Blood Elf Illusion - 160331
class spell_gen_blood_elfe_illusion : public SpellScriptLoader
{
    public:
        spell_gen_blood_elfe_illusion() : SpellScriptLoader("spell_gen_blood_elfe_illusion") { }

        class spell_gen_blood_elfe_illusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_blood_elfe_illusion_AuraScript);

            enum eSpells
            {
                BloodElfMale = 20578,
                BloodElfFemale = 20579,
                SkyMirrorImage  = 127315
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                l_Caster->RemoveAura(eSpells::SkyMirrorImage);

                if (l_Caster->getGender() == GENDER_MALE)
                    l_Caster->SetDisplayId(eSpells::BloodElfMale);
                else if (l_Caster->getGender() == GENDER_FEMALE)
                    l_Caster->SetDisplayId(eSpells::BloodElfFemale);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_blood_elfe_illusion_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_blood_elfe_illusion_AuraScript();
        }
};

/// Last Update 6.2.3
/// Wyrmhunter Hooks - 88914
class spell_gen_wyrmhunter_hooks : public SpellScriptLoader
{
    public:
        spell_gen_wyrmhunter_hooks() : SpellScriptLoader("spell_gen_wyrmhunter_hooks") {}

        class spell_gen_wyrmhunter_hooks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_wyrmhunter_hooks_SpellScript);

            enum eData
            {
                NpcObsidianPyrewing = 46141
            };

            SpellCastResult CheckTarget()
            {
                if (Unit* l_Target = GetExplTargetUnit())
                {
                    if (l_Target->IsPlayer())
                        return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                    else if (l_Target->ToCreature() && l_Target->ToCreature()->GetEntry() != eData::NpcObsidianPyrewing)
                        return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_wyrmhunter_hooks_SpellScript::CheckTarget);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_wyrmhunter_hooks_SpellScript();
        }
};

/// Last Update 6.2.3
/// Demon Hunter's Aspect - 113095
uint32 MaleModels[] = { 20122, 20124, 20130, 20131, 20132, 20133, 20134, 20756, 20757, 20758, 20759 };
class spell_gen_demon_hunters_aspect : public SpellScriptLoader
{
    public:
        spell_gen_demon_hunters_aspect() : SpellScriptLoader("spell_gen_demon_hunters_aspect") { }

        class  spell_gen_demon_hunters_aspect_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_demon_hunters_aspect_AuraScript);

            enum eDatas
            {
                FemaleModel = 20126
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                /// Random model
                uint8 l_RandomModel = urand(0, 10);

                if (l_Player->getGender() == GENDER_MALE)
                    l_Player->SetDisplayId(MaleModels[l_RandomModel]);
                else ///< TODO : Fine display ID female
                    l_Player->SetDisplayId(eDatas::FemaleModel);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                l_Player->SetDisplayId(l_Player->GetNativeDisplayId());
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_demon_hunters_aspect_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_demon_hunters_aspect_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_demon_hunters_aspect_AuraScript();
        }
};

/// Last Update 6.2.3
/// Jewel of Hellfire - 187150
class spell_gen_jewel_of_hellfire_trigger : public SpellScriptLoader
{
    public:
        spell_gen_jewel_of_hellfire_trigger() : SpellScriptLoader("spell_gen_jewel_of_hellfire_trigger") { }

        class spell_gen_jewel_of_hellfire_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_jewel_of_hellfire_trigger_SpellScript);

            enum eSpells
            {
                JewelAura = 187174
            };

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* l_Caster = GetCaster();
                l_Caster->CastSpell(l_Caster, eSpells::JewelAura, false);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_jewel_of_hellfire_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_jewel_of_hellfire_trigger_SpellScript();
        }
};

/// Last Update 6.2.3
/// Jewel of Hellfire - 187174
class spell_gen_jewel_of_hellfire : public SpellScriptLoader
{
public:
    spell_gen_jewel_of_hellfire() : SpellScriptLoader("spell_gen_jewel_of_hellfire") { }

    class  spell_gen_jewel_of_hellfire_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_jewel_of_hellfire_AuraScript);

        enum eDatas
        {
            MorphMale = 63130,
            MorphFemale = 63138
        };

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Player = GetTarget()->ToPlayer();

            if (l_Player == nullptr)
                return;

            if (l_Player->getGender() == GENDER_MALE)
                l_Player->SetDisplayId(eDatas::MorphMale);
            else
                l_Player->SetDisplayId(eDatas::MorphFemale);
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* l_Player = GetTarget()->ToPlayer();

            if (l_Player == nullptr)
                return;

            l_Player->SetDisplayId(l_Player->GetNativeDisplayId());
        }

        void Register()
        {
            AfterEffectApply += AuraEffectRemoveFn(spell_gen_jewel_of_hellfire_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_gen_jewel_of_hellfire_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_jewel_of_hellfire_AuraScript();
    }
};

/// Last Update 6.2.3
/// Wyrmhunter Hooks - 88914
class spell_reconfigured_remote_shock : public SpellScriptLoader
{
    public:
        spell_reconfigured_remote_shock() : SpellScriptLoader("spell_reconfigured_remote_shock") {}

        class spell_reconfigured_remote_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_reconfigured_remote_shock_SpellScript);

            enum eData
            {
                NpcJungleShredder = 67285
            };

            SpellCastResult CheckTarget()
            {
                if (Unit* l_Target = GetExplTargetUnit())
                {
                    if (l_Target->IsPlayer())
                        return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                    else if (l_Target->ToCreature() && l_Target->ToCreature()->GetEntry() != eData::NpcJungleShredder)
                        return SpellCastResult::SPELL_FAILED_BAD_TARGETS;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_reconfigured_remote_shock_SpellScript::CheckTarget);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_reconfigured_remote_shock_SpellScript();
        }
};

/// Last Update 6.2.3
/// Ironbeard's Hat - 188228
class spell_gen_ironbeards_hat : public SpellScriptLoader
{
    public:
        spell_gen_ironbeards_hat() : SpellScriptLoader("spell_gen_ironbeards_hat") { }

        class  spell_gen_ironbeards_hat_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_ironbeards_hat_AuraScript);

            enum eDatas
            {
                Morph = 63424
            };

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                l_Player->SetDisplayId(eDatas::Morph);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                l_Player->RestoreDisplayId();
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_ironbeards_hat_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_ironbeards_hat_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_ironbeards_hat_AuraScript();
        }
};

/// Support for Pilfering Perfume quest(A:24656 H:24541)
enum ServiceUniform
{
    SPELL_SERVICE_UNIFORM       = 71450
};

class spell_gen_service_uniform : public SpellScriptLoader
{
    public:
        spell_gen_service_uniform() : SpellScriptLoader("spell_gen_service_uniform") { }

        class spell_gen_service_uniform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_service_uniform_AuraScript);

            enum eDatas
            {
                MorphMale = 31002,
                MorphFemale = 31003
            };

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SERVICE_UNIFORM))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                if (l_Player->getGender() == GENDER_MALE)
                    l_Player->SetDisplayId(eDatas::MorphMale);
                else
                    l_Player->SetDisplayId(eDatas::MorphFemale);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                l_Player->RestoreDisplayId();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_service_uniform_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_service_uniform_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_service_uniform_AuraScript();
    }
};

/// Last Update 6.2.3
/// Coin of Many Faces - 192225
class spell_gen_coin_of_many_faces : public SpellScriptLoader
{
    public:
        spell_gen_coin_of_many_faces() : SpellScriptLoader("spell_gen_coin_of_many_faces") { }

        class  spell_gen_coin_of_many_faces_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_coin_of_many_faces_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                CreatureTemplate const* l_CreatureTemplate = sObjectMgr->GetRandomTemplate(CreatureType::CREATURE_TYPE_HUMANOID);

                if (l_CreatureTemplate != nullptr)
                    l_Player->SetDisplayId(l_CreatureTemplate->Modelid[0]);
           }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* l_Player = GetTarget()->ToPlayer();

                if (l_Player == nullptr)
                    return;

                l_Player->RestoreDisplayId();
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_coin_of_many_faces_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_coin_of_many_faces_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_coin_of_many_faces_AuraScript();
        }
};

/// Last Update 6.2.3
/// Nullification Barrier - 115817
class spell_nullification_barrier : public SpellScriptLoader
{
    public:
        spell_nullification_barrier() : SpellScriptLoader("spell_nullification_barrier") { }

        class spell_nullification_barrier_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_nullification_barrier_SpellScript);

            SpellCastResult CheckMap()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetMapId() != 1008)
                        return SPELL_FAILED_INCORRECT_AREA;
                    else
                        return SPELL_CAST_OK;
                }
                else
                    return SPELL_FAILED_CASTER_DEAD;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_nullification_barrier_SpellScript::CheckMap);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_nullification_barrier_SpellScript();
        }
};


/// Last Update 6.2.3
/// Transmorphose - 162313
class spell_gen_transmorphose : public SpellScriptLoader
{
    public:
        spell_gen_transmorphose() : SpellScriptLoader("spell_gen_transmorphose") { }

        class  spell_gen_transmorphose_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_transmorphose_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Player* l_Player = GetTarget()->ToPlayer();
                if (l_Player == nullptr)
                    return;

                AuraEffect* l_AuraEffect = GetEffect(0);
                if (l_AuraEffect == nullptr)
                    return;

                Gender l_NewGender = (Gender)l_Player->getGender();

                /// This need to understand if player already have this buff (for example after logout), to prevent change of the gender one more time
                uint32 l_NativeGender = l_AuraEffect->GetAmount();
                if (l_NativeGender == 0)
                    l_AuraEffect->SetAmount(l_NewGender + 1);

                uint32 l_OtherGender = l_AuraEffect->GetAmount() - 1;
                if (l_OtherGender == GENDER_MALE)            ///< MALE
                    l_NewGender = GENDER_FEMALE;
                else if (l_OtherGender == GENDER_FEMALE)     ///< FEMALE
                    l_NewGender = GENDER_MALE;
                else
                    return;

                /// Set new gender
                l_Player->SetGender(l_NewGender);
                l_Player->SetByteValue(PLAYER_FIELD_INEBRIATION, PLAYER_BYTES_3_OFFSET_GENDER, l_NewGender);

                /// Set new gender
                l_Player->SetGender(l_NewGender);
                l_Player->SetByteValue(PLAYER_FIELD_ARENA_FACTION, 0, l_NewGender);

                PlayerInfo const* l_Info = sObjectMgr->GetPlayerInfo(l_Player->getRace(), l_Player->getClass());
                if (!l_Info)
                    return;
                uint16 l_NewDisplayId = 0;

                if (l_NewGender == GENDER_MALE)
                    l_NewDisplayId = l_Info->displayId_m;
                else if (l_NewGender == GENDER_FEMALE)
                    l_NewDisplayId = l_Info->displayId_f;

                /// Check if this is first time, our player already has this aura
                /// If aura has applied less then 3 seconds ago - just casted
                bool l_FirstTime = false;
                if (Aura* l_Transmorphed = l_Player->GetAura(GetSpellInfo()->Id))
                    if (l_Transmorphed->GetDuration() > (l_Transmorphed->GetMaxDuration() - 500))
                        l_FirstTime = true;

                /// Check if at the moment player has the same model as his native
                bool l_CurrentModelEqualNative = l_Player->GetNativeDisplayId() == l_Player->GetDisplayId();

                /// If not in original form (for example cat,bear,metamorphosis), we don't need to change displayId now
                if ((!l_FirstTime && !l_CurrentModelEqualNative) || l_CurrentModelEqualNative)
                    l_Player->InitDisplayIds();
                else
                    l_Player->SetNativeDisplayId(l_NewDisplayId);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Player* l_Player = GetTarget()->ToPlayer();
                if (l_Player == nullptr)
                    return;

                AuraEffect* l_AuraEffect = GetEffect(0);
                if (l_AuraEffect == nullptr)
                    return;

                uint32 l_NativeGender = l_AuraEffect->GetAmount() - 1;

                l_Player->SetByteValue(UNIT_FIELD_SEX, UNIT_BYTES_0_OFFSET_GENDER, l_NativeGender);
                l_Player->SetByteValue(PLAYER_FIELD_ARENA_FACTION, 0, l_NativeGender);

                PlayerInfo const* l_Info = sObjectMgr->GetPlayerInfo(l_Player->getRace(), l_Player->getClass());
                if (!l_Info)
                    return;
                uint16 l_NativeDisplayId = 0;

                if (l_NativeGender == GENDER_MALE)
                    l_NativeDisplayId = l_Info->displayId_m;
                else if (l_NativeGender == GENDER_FEMALE)
                    l_NativeDisplayId = l_Info->displayId_f;

                /// If not in original form (for example cat,bear,metamorphosis), we don't need to change displayId now
                if (l_Player->GetNativeDisplayId() == l_Player->GetDisplayId())
                    l_Player->InitDisplayIds();
                else
                    l_Player->SetNativeDisplayId(l_NativeDisplayId);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_transmorphose_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AuraEffectHandleModes(AURA_EFFECT_HANDLE_REAL | AURA_EFFECT_HANDLE_SEND_FOR_CLIENT));
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_transmorphose_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_transmorphose_AuraScript();
        }
};

/// Last Update 6.2.3
/// Pvp Trinket - 42292
class spell_gen_pvp_trinket : public SpellScriptLoader
{
    public:
        spell_gen_pvp_trinket() : SpellScriptLoader("spell_gen_pvp_trinket") { }

        class spell_gen_pvp_trinket_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pvp_trinket_SpellScript);

            enum eSpells
            {
                AllianceTinketVisual    = 97403,
                HordeTinketVisual       = 97404
            };

            void TriggerAnimation()
            {
                Player* caster = GetCaster()->ToPlayer();

                switch (caster->GetTeam())
                {
                case ALLIANCE:
                    caster->CastSpell(caster, eSpells::AllianceTinketVisual, TRIGGERED_FULL_MASK);
                    break;
                case HORDE:
                    caster->CastSpell(caster, eSpells::HordeTinketVisual, TRIGGERED_FULL_MASK);
                    break;
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_gen_pvp_trinket_SpellScript::TriggerAnimation);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_pvp_trinket_SpellScript();
        }
};

/// Called By Appraisal - 134280
class spell_gen_appraisal : public SpellScriptLoader
{
public:
    spell_gen_appraisal() : SpellScriptLoader("spell_gen_appraisal") { }

    class spell_gen_appraisal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_appraisal_SpellScript);

        void HandleAfterCast()
        {
            Player* l_Player = GetCaster()->ToPlayer();
            if (!l_Player)
                return;

            int32 reward = 0;

            static std::map<int32, int32> l_Loots = {
                {92616, 92617}, ///< Golden Fruit Bowl
                {92602, 92603}, ///< Large Pile of Gold Coins
                {92580, 92581}, ///< Fragrant Perfume
                {92586, 92587}, ///< Sparkling Sapphire
                {92598, 92599}, ///< Gold Ring
                {92584, 92585}, ///< Expensive Ruby
                {92594, 92595}, ///< Diamond Ring
                {92600, 92601}, ///< Small Pile of Gold Coins
                {92590, 92591}, ///< Ruby Necklace
                {92610, 92611}, ///< Golden Platter
                {92588, 92589}, ///< Jade Kitten Figurine
                {92604, 92605}, ///< Golden Goblet
                {92614, 92615}, ///< Taric's Family Jewels
                {92592, 92593}, ///< Spellstone Necklace
                {92582, 92583}, ///< Cheap Cologne
                {92620, 92621}, ///< Elysia's Bindings
                {92624, 92624}, ///< Theldren's rusted runeblade
                {92606, 92607}, ///< Golden High Elf Statuette
                {92612, 92613}, ///< Zena's Ridiculously Rich Yarnball
                {92622, 92623}, ///< Ancient Orcish Shield
            };

            int32 count;
            for (std::map<int32, int32>::const_iterator l_Itr = l_Loots.begin(); l_Itr != l_Loots.end(); ++l_Itr)
            {
                count = l_Player->GetItemCount(l_Itr->first);
                while (count > 0)
                {
                    Item* l_Item = l_Player->GetItemByEntry(l_Itr->first);
                    uint32 l_ItemCount = l_Item->GetCount();

                    uint8 l_BagSlot = l_Item->GetBagSlot();
                    uint8 l_Slot = l_Item->GetSlot();
                    l_Player->DestroyItem(l_BagSlot, l_Slot, false);

                    ItemPosCountVec l_Dest;
                    InventoryResult l_Result = l_Player->CanStoreNewItem(l_BagSlot, l_Slot, l_Dest, l_Itr->second, l_ItemCount);
                    if (l_Result != EQUIP_ERR_OK || l_Dest.empty())
                    {
                        ChatHandler(l_Player).PSendSysMessage("An error occurred, returning your item...");
                        l_Player->AddItem(l_Itr->first, l_ItemCount);
                        return;
                    }

                    l_Item = l_Player->StoreNewItem(l_Dest, l_Itr->second, true, Item::GenerateItemRandomPropertyId(l_Itr->second));
                    if (!l_Item)
                    {
                        ChatHandler(l_Player).PSendSysMessage("An error occurred, returning your item...");
                        l_Player->AddItem(l_Itr->first, l_ItemCount);
                        return;
                    }

                    count -= l_ItemCount;
                }
            }
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_gen_appraisal_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_appraisal_SpellScript();
    }
};

/// Last Update 6.2.3
/// Goblin Glider - 126389
class spell_gen_golbin_glider : public SpellScriptLoader
{
    public:
        spell_gen_golbin_glider() : SpellScriptLoader("spell_gen_golbin_glider") { }

        class  spell_gen_golbin_glider_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_golbin_glider_AuraScript);

            void OnUpdate(const uint32 /*diff*/)
            {
                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player || l_Player->IsFalling())
                    return;

                if (Aura* l_Aura = l_Player->GetAura(GetSpellInfo()->Id))
                    l_Player->RemoveAura(l_Aura);
            }

            void Register()
            {
                OnAuraUpdate += AuraUpdateFn(spell_gen_golbin_glider_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_golbin_glider_AuraScript();
        }
};


bool SummonHati::Execute(uint64 /*p_EndTime*/, uint32 /*p_Time*/)
{
    Player* l_Player = sObjectAccessor->FindPlayer(m_GUID);
    if (!l_Player || !l_Player->HasSpell(eSpells::TitanStrike))
        return true;

    Pet* l_Pet = l_Player->GetPet();
    if (!l_Pet)
        return true;

    if (Creature* l_Hati = l_Player->GetHati())
        return true;

    uint32 l_Cooldown = l_Player->GetSpellCooldownDelay(eSpells::Stormbound);
    if (l_Cooldown)
        l_Player->m_Events.AddEvent(new SummonHati(l_Player->GetGUID()), l_Player->m_Events.CalculateTime(l_Cooldown));
    else
        l_Player->CastSpell(l_Player, eSpells::Stormbound, true);

    return true;
}

/// Last Update 7.1.5 - 23420
/// Called by Ritual Of Summoning - 7720
class spell_gen_ritual_of_summoning : public SpellScriptLoader
{
    public:
        spell_gen_ritual_of_summoning() : SpellScriptLoader("spell_gen_ritual_of_summoning") { }

        class spell_gen_ritual_of_summoning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_ritual_of_summoning_SpellScript);

            void HandleHitTarget(SpellEffIndex p_EffIndex)
            {
                if (Unit* l_Target = GetHitUnit())
                {
                    if (Unit* l_Caster = GetCaster())
                    {
                        if (l_Caster->GetMapId() == 1220 && l_Target->getLevel() < 98)
                            PreventHitEffect(p_EffIndex);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_ritual_of_summoning_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON_PLAYER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_ritual_of_summoning_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by PvP Rule Enabled - 134735
class spell_gen_pvp_rule_enabled : public SpellScriptLoader
{
    public:
        spell_gen_pvp_rule_enabled() : SpellScriptLoader("spell_gen_pvp_rule_enabled") { }

        class  spell_gen_pvp_rule_enabled_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_pvp_rule_enabled_AuraScript);

            void OnApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Player* l_Player = l_Target->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->ActivatePvPTalents();
            }

            void OnRemove(AuraEffect const* p_AuraEffect, AuraEffectHandleModes p_Modes)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                Player* l_Player = l_Target->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->DeactivatePvPTalents();
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_pvp_rule_enabled_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_pvp_rule_enabled_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_pvp_rule_enabled_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by PvP Stats - Set Stats - Script Effect - 198147
class spell_gen_pvp_stat_set_stat : public SpellScriptLoader
{
    public:
        spell_gen_pvp_stat_set_stat() : SpellScriptLoader("spell_gen_pvp_stat_set_stat") { }

        class  spell_gen_pvp_stat_set_stat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pvp_stat_set_stat_SpellScript);

            enum eSpells
            {
                PrincplesOfWar = 197912
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasAura(eSpells::PrincplesOfWar))
                    l_Player->RemoveAura(eSpells::PrincplesOfWar);
                else
                    l_Player->CastSpell(l_Player, eSpells::PrincplesOfWar, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_pvp_stat_set_stat_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_pvp_stat_set_stat_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by all PvP Stat Spell
class spell_gen_pvp_stat_spells : public SpellScriptLoader
{
    public:
        spell_gen_pvp_stat_spells() : SpellScriptLoader("spell_gen_pvp_stat_spells") { }

        class spell_gen_pvp_stat_spells_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_pvp_stat_spells_AuraScript);

            enum ePvPScalingTypes
            {
                AttackSpeed      = 1,   ///< Attack Speed (Multiplicative) always 1
                WeaponIlvlOffset = 2,   ///< Weapon ilevel offset (Additive, max level only) always 0
                ModDamageDone    = 3,   ///< Mod Damage Done (Override)
                ModPowerRegen    = 4,   ///< Mod Power Regen (Override)
                Strength         = 5,   ///< Strength (Multiplicative)
                Agility          = 6,   ///< Agility (Multiplicative)
                Intellect        = 7,   ///< Intellect (Multiplicative)
                Stamina          = 8,   ///< Stamina (Multiplicative)
                Armor            = 9,   ///< Armor (Multiplicative)
                Mastery          = 10,  ///< Mastery (Multiplicative)
                Haste            = 11,  ///< Haste (Multiplicative)
                Versatility1     = 12,  ///< Versatility 1/2 (Multiplicative)
                Versatility2     = 13,  ///< Versatility 2/2 (Multiplicative)
                CriticalStrike   = 14   ///< Critical Strike (Multiplicative)
            };

            void CalculateAmount(AuraEffect const* p_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_ILvl = l_Player->GetAverageItemLevelEquipped();
                int32 l_ILvlBonus = l_ILvl - 800;
                if (l_ILvlBonus <= 0)
                    l_ILvlBonus = 0;

                if (sSpellMgr->IsInPvPTemplateDebug())
                    return;

                uint32 l_PlayerSpec = l_Player->GetActiveSpecializationID();

                auto l_SpecTemplateItr = g_PvpScalingPerSpecs.find(l_PlayerSpec);
                if (l_SpecTemplateItr == g_PvpScalingPerSpecs.end())
                    return;

                auto l_SpecTemplate = l_SpecTemplateItr->second;

                int32 l_BaseAmount = 0;
                float l_TemplateModifier = 1.f;

                switch (p_AuraEffect->GetAuraType())
                {
                    case SPELL_AURA_MOD_STAT:
                    {
                        uint32 l_MiscA = p_AuraEffect->GetMiscValue();

                        /// Main stats template values
                        switch (l_MiscA)
                        {
                            case UNIT_MOD_STAT_STAMINA:
                                p_Amount = 38032;
                                break;
                            case UNIT_MOD_STAT_AGILITY:
                            case UNIT_MOD_STAT_INTELLECT:
                            case UNIT_MOD_STAT_STRENGTH:
                                p_Amount = 13626;
                                break;
                            default:
                                break;
                        }

                        /// Main stats multipliers
                        switch (l_MiscA)
                        {
                            case UNIT_MOD_STAT_AGILITY:
                                l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Agility];
                                break;
                            case UNIT_MOD_STAT_INTELLECT:
                                l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Intellect];
                                break;
                            case UNIT_MOD_STAT_STRENGTH:
                                l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Strength];
                                break;
                            case UNIT_MOD_STAT_STAMINA:
                                l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Stamina];
                                break;
                            default:
                                break;
                        }

                        l_BaseAmount = l_Player->GetCreateStat(Stats(l_MiscA));
                        break;
                    }
                    case SPELL_AURA_MOD_RATING:
                    {
                        uint32 l_MiscA = p_AuraEffect->GetMiscValue();
                        for (uint32 l_Itr = 0; l_Itr <= MAX_COMBAT_RATING; ++l_Itr)
                        {
                            if (l_MiscA & (1 << l_Itr))
                            {
                                switch (l_Itr)
                                {
                                    case CombatRating::CR_MASTERY:
                                        l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Mastery];
                                        break;
                                    case CombatRating::CR_HASTE_MELEE:
                                    case CombatRating::CR_HASTE_RANGED:
                                    case CombatRating::CR_HASTE_SPELL:
                                        l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Haste];
                                        break;
                                    case CombatRating::CR_VERSATILITY_DAMAGE_DONE:
                                    case CombatRating::CR_VERSATUKUTY_HEALING_DONE:
                                        l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Versatility1];
                                        break;
                                    case CombatRating::CR_VERSATILITY_DAMAGE_TAKEN:
                                        l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Versatility2];
                                        break;
                                    case CombatRating::CR_CRIT_MELEE:
                                    case CombatRating::CR_CRIT_RANGED:
                                    case CombatRating::CR_CRIT_SPELL:
                                        l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::CriticalStrike];
                                        break;
                                    default:
                                        break;
                                }

                                p_Amount = 6692;

                                break;
                            }
                        }
                        break;
                    }
                    case SPELL_AURA_MOD_BASE_RESISTANCE:
                    {
                        switch (l_PlayerSpec)
                        {
                            case SPEC_DEMON_HUNTER_HAVOC:       p_Amount = 2429; break;
                            case SPEC_DEMON_HUNTER_VENGEANCE:   p_Amount = 5060; break;
                            case SPEC_DK_BLOOD:                 p_Amount = 6252; break;
                            case SPEC_DK_FROST:                 p_Amount = 5384; break;
                            case SPEC_DK_UNHOLY:                p_Amount = 4765; break;
                            case SPEC_DRUID_BALANCE:            p_Amount = 2186; break;
                            case SPEC_DRUID_FERAL:              p_Amount = 2793; break;
                            case SPEC_DRUID_GUARDIAN:           p_Amount = 2664; break;
                            case SPEC_DRUID_RESTORATION:        p_Amount = 2429; break;
                            case SPEC_MONK_BREWMASTER:          p_Amount = 4797; break;
                            case SPEC_MONK_MISTWEAVER:          p_Amount = 2671; break;
                            case SPEC_MONK_WINDWALKER:          p_Amount = 2428; break;
                            case SPEC_SHAMAN_ENHANCEMENT:       p_Amount = 3776; break;
                            case SPEC_SHAMAN_ELEMENTAL:         p_Amount = 4911; break;
                            case SPEC_SHAMAN_RESTORATION:       p_Amount = 3778; break;
                            case SPEC_PRIEST_DISCIPLINE:        p_Amount = 1634; break;
                            case SPEC_PRIEST_HOLY:              p_Amount = 1606; break;
                            case SPEC_PRIEST_SHADOW:            p_Amount = 1646; break;
                            case SPEC_PALADIN_HOLY:             p_Amount = 4766; break;
                            case SPEC_PALADIN_PROTECTION:       p_Amount = 6813; break;
                            case SPEC_PALADIN_RETRIBUTION:      p_Amount = 4765; break;
                            case SPEC_WARLOCK_AFFLICTION:       p_Amount = 1945; break;
                            case SPEC_WARLOCK_DEMONOLOGY:       p_Amount = 1945; break;
                            case SPEC_WARLOCK_DESTRUCTION:      p_Amount = 2527; break;
                            case SPEC_HUNTER_BEASTMASTERY:      p_Amount = 3022; break;
                            case SPEC_HUNTER_MARKSMANSHIP:      p_Amount = 3627; break;
                            case SPEC_HUNTER_SURVIVAL:          p_Amount = 3778; break;
                            case SPEC_WARRIOR_ARMS:             p_Amount = 5242; break;
                            case SPEC_WARRIOR_FURY:             p_Amount = 4764; break;
                            case SPEC_WARRIOR_PROTECTION:       p_Amount = 7097; break;
                            case SPEC_MAGE_ARCANE:              p_Amount = 1945; break;
                            case SPEC_MAGE_FIRE:                p_Amount = 1945; break;
                            case SPEC_MAGE_FROST:               p_Amount = 1945; break;
                            case SPEC_ROGUE_ASSASSINATION:      p_Amount = 2429; break;
                            case SPEC_ROGUE_OUTLAW:             p_Amount = 2429; break;
                            case SPEC_ROGUE_SUBTLETY:           p_Amount = 2429; break;
                            default:
                                break;
                        }
                        l_TemplateModifier = l_SpecTemplate[ePvPScalingTypes::Armor];
                        break;
                    }
                    case SPELL_AURA_MOD_DAMAGE_PERCENT_DONE:
                    {
                        p_Amount = l_SpecTemplate[ePvPScalingTypes::ModDamageDone];
                        break;
                    }
                    case SPELL_AURA_MOD_POWER_REGEN:
                    {
                        p_Amount = l_SpecTemplate[ePvPScalingTypes::ModPowerRegen];
                        break;
                    }
                    default:
                        break;
                }

                /// (tempate primary characteristic+ basic primary characteristic)* item lvl*modifier specialization
                float l_ILvlBonusMultiplier = 1.0f + (float(l_ILvlBonus) / 1000.0f);
                p_Amount += l_BaseAmount;
                p_Amount *= l_TemplateModifier;
                if (l_SpellInfo->Id != 198439 || p_AuraEffect->GetEffIndex() != 3) ///< Shouldn't be affected by iLVL, as the bonus from aura 198439 is negative for the regen (-12320 for Rdruid for example) so increased iLVL induced less mana regen 7.3.5 - Build 26365
                    p_Amount = int32(p_Amount * l_ILvlBonusMultiplier);

                p_Amount -= l_BaseAmount;
            }

            void HandleAfterEffectApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = GetCaster()->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->AddDelayedEvent([l_Player]()->void
                {
                    /// Apply weapon damage template
                    if (Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                    {
                        l_Player->UpdateWeaponDamage(l_Item->GetTemplate(), EQUIPMENT_SLOT_MAINHAND, false, l_Item);
                        l_Player->UpdateWeaponDamage(l_Item->GetTemplate(), EQUIPMENT_SLOT_MAINHAND, true, l_Item);
                    }

                    if (Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                    {
                        l_Player->UpdateWeaponDamage(l_Item->GetTemplate(), EQUIPMENT_SLOT_OFFHAND, false, l_Item);
                        l_Player->UpdateWeaponDamage(l_Item->GetTemplate(), EQUIPMENT_SLOT_OFFHAND, true, l_Item);
                    }
                }, 2 * IN_MILLISECONDS);
            }

            void Register() override
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(m_scriptSpellId);
                if (!l_SpellInfo)
                    return;

                AfterEffectApply += AuraEffectApplyFn(spell_gen_pvp_stat_spells_AuraScript::HandleAfterEffectApply, EFFECT_13, SPELL_AURA_MOD_RATING, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);

                for (uint8 l_Itr = 0; l_Itr <= EFFECT_31; ++l_Itr)
                {
                    if (l_SpellInfo->Effects[l_Itr].ApplyAuraName)
                        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_pvp_stat_spells_AuraScript::CalculateAmount, l_Itr, l_SpellInfo->Effects[l_Itr].ApplyAuraName);
                }
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_pvp_stat_spells_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Principles Of War - 197912
class spell_gen_principles_of_war : public SpellScriptLoader
{
    public:
        spell_gen_principles_of_war() : SpellScriptLoader("spell_gen_principles_of_war") { }

        class  spell_gen_principles_of_war_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_principles_of_war_AuraScript);

            void CalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                uint32 l_ILvl = l_Player->GetAverageItemLevelEquipped();
                int32 l_ILvlBonus = l_ILvl - 800;
                if (l_ILvlBonus >= 10)
                    p_Amount = l_ILvlBonus / 10;
                else
                    p_Amount = 0;
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_principles_of_war_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_90);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_principles_of_war_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called for PvP Rules Enabled - 134735
/// Called for Honorable Medallion - 195710
class PlayerScript_gen_pvp_rules_enabled : public PlayerScript
{
    public:
        PlayerScript_gen_pvp_rules_enabled() :PlayerScript("PlayerScript_gen_pvp_rules_enabled") {}

        enum eSpells
        {
            HonorableMedallion  = 195710,
            PvPRulesEnabled     = 134735,
            PrincipleOfWar      = 197912
        };

        void EnablePvPTemplate(Player* p_Player)
        {
            p_Player->SetInBattlegroundProcess(true);

            for (uint8 l_I = 0; l_I < INVENTORY_SLOT_BAG_END; ++l_I)
            {
                if (Item* l_Item = p_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_I))
                {
                    p_Player->_ApplyItemMods(l_Item, l_I, false, l_Item->GetTemplate()->Class == ItemClass::ITEM_CLASS_WEAPON ? false : true);

                    if (ItemTemplate const* l_ItemTemplate = l_Item->GetTemplate())
                        RemoveItemsSetItem(p_Player, l_ItemTemplate);

                    /// HACK: restore relics
                    /// It needs after 'l_Item->IsArtifactOrArtifactChild(p_Player) && !p_Player->GetSession()->PlayerLoading()' change
                    /// Or the artifact will be downgraded (bug).
                    if (l_Item->IsArtifactOrArtifactChild(p_Player))
                    {
                        p_Player->ApplyEnchantment(l_Item, EnchantmentSlot::SOCK_ENCHANTMENT_SLOT, true);
                        p_Player->ApplyEnchantment(l_Item, EnchantmentSlot::SOCK_ENCHANTMENT_SLOT_2, true);
                        p_Player->ApplyEnchantment(l_Item, EnchantmentSlot::SOCK_ENCHANTMENT_SLOT_3, true);
                    }
                }
            }

            if (!p_Player->HasSpell(eSpells::HonorableMedallion))
                p_Player->learnSpell(eSpells::HonorableMedallion, false, false, false, 0, true);

            if (!p_Player->HasAura(eSpells::PvPRulesEnabled))
                p_Player->CastSpell(p_Player, eSpells::PvPRulesEnabled, true);

            if (!p_Player->HasAura(eSpells::PrincipleOfWar))
            {
                p_Player->CastSpell(p_Player, uint32(PvPStatsSpells::SET_STATS_SCRIPT_EFFECT), true);
                p_Player->CastSpell(p_Player, uint32(p_Player->SelectPvpStatsSpell()), true);
            }

            p_Player->UpdateStatBuffFields(STAT_AGILITY);
            p_Player->UpdateStatBuffFields(STAT_STRENGTH);
            p_Player->UpdateStatBuffFields(STAT_INTELLECT);

            p_Player->SetInBattleground(true);

            /// Remove all auras with attribute SPELL_ATTR8_NOT_IN_BG_OR_ARENA when entering pvp instance.
            std::set<uint32> l_AuraToRemove;
            std::multimap<uint32, AuraApplication*> l_AppliedAuras = p_Player->GetAppliedAuras();
            for (auto l_Itr = l_AppliedAuras.begin(); l_Itr != l_AppliedAuras.end(); ++l_Itr)
            {
                if ((*l_Itr).second->GetBase()->GetSpellInfo()->AttributesEx8 & SPELL_ATTR8_NOT_IN_BG_OR_ARENA)
                    l_AuraToRemove.insert((*l_Itr).second->GetBase()->GetSpellInfo()->Id);
            }

            for (uint32 l_SpellId : l_AuraToRemove)
                p_Player->RemoveAurasDueToSpell(l_SpellId);

            p_Player->RecalculateAllAurasAmount();

            p_Player->SetHealth(p_Player->GetMaxHealth());
        }

        /// Custom for tournament realm, pvp template are enabled everywhere
        void OnTeleport(Player* p_Player, SpellInfo const* p_Spell, bool p_WorldPort) override
        {
            if (!sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE) || !p_WorldPort)
                return;

            DisablePvPTemplate(p_Player);
            EnablePvPTemplate(p_Player);
        }

        void OnEnterBG(Player* p_Player, uint32 /*p_MapID*/) override
        {
            if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return;

            EnablePvPTemplate(p_Player);
        }

        void OnLeaveBG(Player* p_Player, uint32 /*p_MapID*/) override
        {
            if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return;

            DisablePvPTemplate(p_Player);
        }

        void OnDuelStart(Player* p_Player1, Player* p_Player2) override
        {
            if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return;

            p_Player1->SetInDuel(true);
            p_Player2->SetInDuel(true);
            p_Player1->SetInPvPCombat(true);
            p_Player2->SetInPvPCombat(true);

            p_Player1->RecalculateAllAurasAmount();
            p_Player2->RecalculateAllAurasAmount();

            if (p_Player1->GetMap()->GetEntry()->IsWorldMap() && p_Player2->GetMap()->GetEntry()->IsWorldMap())
            {
                if (!p_Player1->HasAura(eSpells::PvPRulesEnabled))
                    p_Player1->CastSpell(p_Player1, eSpells::PvPRulesEnabled, true);
                if (!p_Player2->HasAura(eSpells::PvPRulesEnabled))
                    p_Player2->CastSpell(p_Player2, eSpells::PvPRulesEnabled, true);
            }
        }

        void OnDuelEnd(Player* p_Player1, Player* p_Player2, DuelCompleteType /*p_CompletionType*/) override
        {
            if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return;

            p_Player1->RemoveAurasDueToSpell(eSpells::PvPRulesEnabled);
            p_Player2->RemoveAurasDueToSpell(eSpells::PvPRulesEnabled);

            p_Player1->SetInDuel(false);
            p_Player2->SetInDuel(false);

            p_Player1->RecalculateAllAurasAmount(true);
            p_Player2->RecalculateAllAurasAmount(true);
        }

        void OnModifySpec(Player* p_Player, int32 p_OldSpec, int32 p_NewSpec) override
        {
            if (!p_Player->IsInBattleground())
                return;

            if (p_Player->HasAura(uint32(p_Player->SelectPvpStatsSpell(p_OldSpec))))
                p_Player->RemoveAura(uint32(p_Player->SelectPvpStatsSpell(p_OldSpec)));

            p_Player->CastSpell(p_Player, uint32(p_Player->SelectPvpStatsSpell(p_NewSpec)), true);
        }

        void OnLogin(Player* p_Player) override
        {
            if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
            {
                EnablePvPTemplate(p_Player);
                return;
            }

            if (p_Player->IsInBattleground())
            {
                p_Player->CastSpell(p_Player, uint32(p_Player->SelectPvpStatsSpell()), true);
                p_Player->CastSpell(p_Player, eSpells::PvPRulesEnabled, true);
                p_Player->CastSpell(p_Player, eSpells::PrincipleOfWar, true);
            }
            /// Make sure pvp template stats aura are removed outside battleground (can happen in case of crash)
            else if (p_Player->HasAura(eSpells::PrincipleOfWar))
            {
                sLog->outAshran("PlayerScript_gen_pvp_rules_enabled::OnLogin: player %u isn't in battleground but have principle of war aura !", p_Player->GetGUIDLow());

                p_Player->RemoveAurasDueToSpell(uint32(p_Player->SelectPvpStatsSpell()));
                p_Player->RemoveAurasDueToSpell(uint32(PvPStatsSpells::SET_STATS_SCRIPT_EFFECT));
                p_Player->RemoveAurasDueToSpell(eSpells::PrincipleOfWar);
            }
        }

        void OnLogout(Player* p_Player) override
        {
            if (p_Player->IsInBattleground() || sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                DisablePvPTemplate(p_Player);
        }

        void DisablePvPTemplate(Player* p_Player)
        {
            if (!p_Player->HasAura(eSpells::PrincipleOfWar) && sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
                return;

            p_Player->SetInBattleground(false);

            p_Player->SetInBattlegroundProcess(false);

            p_Player->SetInBattlegroundLeaving(true);

            PlayerSpellMap const& l_PlayerSpells = p_Player->GetSpellMap();
            for (auto l_Itr = l_PlayerSpells.begin(); l_Itr != l_PlayerSpells.end(); ++l_Itr)
            {
                if (!(*l_Itr).second->active)
                    continue;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo((*l_Itr).first);
                if (!l_SpellInfo)
                    continue;

                if (l_SpellInfo->IsPassive() && p_Player->IsNeedCastPassiveSpellAtLearn(l_SpellInfo) && (l_SpellInfo->AttributesEx8 & SPELL_ATTR8_NOT_IN_BG_OR_ARENA))
                    p_Player->CastSpell(p_Player, (*l_Itr).first, true);
            }

            p_Player->RecalculateAllAurasAmount();

            for (uint8 l_I = 0; l_I < INVENTORY_SLOT_BAG_END; ++l_I)
            {
                if (Item* l_Item = p_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_I))
                {
                    p_Player->_ApplyItemMods(l_Item, l_I, true, l_Item->GetTemplate()->Class == ItemClass::ITEM_CLASS_WEAPON ? false : true);
                    AddItemsSetItem(p_Player, l_Item);
                }
            }

            p_Player->RemoveAurasDueToSpell(uint32(p_Player->SelectPvpStatsSpell()));
            p_Player->RemoveAurasDueToSpell(eSpells::PvPRulesEnabled);
            p_Player->RemoveAurasDueToSpell(eSpells::PrincipleOfWar);

            p_Player->SetInBattlegroundLeaving(false);
        }
};

class PlayerScript_gen_goblin_create : public PlayerScript
{
    public:
        PlayerScript_gen_goblin_create() :PlayerScript("PlayerScript_gen_goblin_create") {}

        enum eSpells
        {
            PackHobgoblin = 69046
        };

        void OnLogin(Player* p_Player) override
        {
            if (!(p_Player->getRace() == RACE_GOBLIN))
                return;

            /// Should be learn on quest line but actually no working
            if (!p_Player->HasSpell(eSpells::PackHobgoblin))
                p_Player->learnSpell(eSpells::PackHobgoblin, false, false, false, 0, true);
        }
};

class PlayerScript_gen_liquid_fire_of_elune : public PlayerScript
{
    public:
        PlayerScript_gen_liquid_fire_of_elune() :PlayerScript("PlayerScript_gen_liquid_fire_of_elune") {}

        enum eSpells
        {
            TheLiquidFireofElune = 47972,
            Exhilaration = 194291
        };

        void OnSpellLearned(Player* p_Player, uint32 p_SpellID) override
        {
            if (p_SpellID == eSpells::Exhilaration)
                p_Player->removeSpell(eSpells::Exhilaration);
        }

        void OnLogout(Player* p_Player) override
        {
            p_Player->RemoveAura(eSpells::TheLiquidFireofElune);
        }
};

/// Used to test Virtual Map Serving System
/// NEVER use it on live realms
class spell_crash_test : public SpellScriptLoader
{
    public:
        spell_crash_test() : SpellScriptLoader("spell_crash_test") { }

        class spell_crash_test_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_crash_test_SpellScript);

            void HandleAfterCast()
            {
                /// Make the thread crash on purpose (crashtest)
                Player* l_Player = nullptr;
                l_Player->TeleportTo(0, 0, 0, 0, 0);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_crash_test_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_crash_test_SpellScript();
        }
};

/// Skinning - 8613
/// Add Stonehide Leather Sample item at first legion skinning
class spell_gen_learn_legion_skinning : public SpellScriptLoader
{
    public:
        spell_gen_learn_legion_skinning() : SpellScriptLoader("spell_gen_learn_legion_skinning") { }

        enum eSpells
        {
            LegionSkinning      = 195125,
            LearnLegionSkinning = 201707
        };

        enum eQuests
        {
            StonehideLeatherSample = 40131,
            StormscaleSample       = 40141
        };

        enum eItems
        {
            StartStonehideLeatherSample = 129860,
            StartStormscaleSample       = 129862
        };

        class spell_gen_learn_legion_skinning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_learn_legion_skinning_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Player* l_Caster = GetCaster()->ToPlayer();
                if (!l_Caster)
                    return;

                if (Creature* l_Creature = GetHitUnit()->ToCreature())
                {
                    if (l_Creature->GetCreatureTemplate()->expansion == EXPANSION_LEGION)
                    {
                        if (LootTemplates_Skinning.HaveItemInLootTableFor(l_Creature->GetCreatureTemplate()->SkinLootId, eItems::StartStonehideLeatherSample))
                        {
                            if (l_Caster->GetQuestStatus(eQuests::StonehideLeatherSample) == QuestStatus::QUEST_STATUS_NONE && !l_Caster->HasItemCount(eItems::StartStonehideLeatherSample))
                                l_Caster->AddItem(eItems::StartStonehideLeatherSample, 1);
                        }

                        if (LootTemplates_Skinning.HaveItemInLootTableFor(l_Creature->GetCreatureTemplate()->SkinLootId, eItems::StartStormscaleSample))
                        {
                            if (l_Caster->GetQuestStatus(eQuests::StormscaleSample) == QuestStatus::QUEST_STATUS_NONE && !l_Caster->HasItemCount(eItems::StartStormscaleSample))
                                l_Caster->AddItem(eItems::StartStormscaleSample, 1);
                        }
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_learn_legion_skinning_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SKINNING);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_learn_legion_skinning_SpellScript();
        }
};

/// 195114, 2366, 158745, 2368, 3570, 11993, 28695, 50300, 74519, 110413
/// Herb gathering
class spell_herb_gathering : public SpellScriptLoader
{
    public:
        spell_herb_gathering() : SpellScriptLoader("spell_herb_gathering"){}

        class spell_herb_gathering_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_herb_gathering_SpellScript);

            void HandleBeforeCast()
            {
                Player* caster = GetCaster()->ToPlayer();
                if (caster->IsMounted() && !(caster->HasAura(134359) || caster->HasAura(223814) || caster->GetSession()->IsPremium())) ///< Skygollem & Mechanized Lumber Extractor allow you to gather herbs while mounted, and so does the VIP.
                {
                    caster->RemoveAurasByType(SPELL_AURA_MOUNTED);
                }
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_herb_gathering_SpellScript::HandleBeforeCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_herb_gathering_SpellScript();
        }
};


/// 193378, 193379, 193380, 193381, 193382, 193383, 193384, 205139, 205695, 205712, 205713, 205714
/// Broken Isles - Herbalism nodes
class spell_gen_herbalism_trap : public SpellScriptLoader
{
    public:
        spell_gen_herbalism_trap() : SpellScriptLoader("spell_gen_herbalism_trap") { }

        enum eSpells
        {
            /// Spells used to learn the legion rank of the profession the first time a player gather a node in Broken Isles
            LegionHerbalistLearning = 201702,
            LegionHerbalistSkill    = 195114,

            /// Ranks require special scripts
            AethrilRank3            = 193294,
            FjarnskagglRank3        = 193303,

            /// Gathering node have a chance to proc thoses summons
            SummonNightfallen       = 193560,
            SummonDreamleaf         = 193810,
            SummonNightmareCreeper  = 193562,
            SummonFrenziedFox       = 193573,
            SummonNightfallen2      = 193561,

            /// Grow spells, used with bud / seed
            SummonStarlightRoseGrow = 193813,
            SummonFelwortGrow       = 193814,
            SummonFoxflowerGrow     = 193811,
            SummonFjarnskagglGrow   = 193812,
            SummonAethrilGrow       = 193809,

            /// You get this perk from Herbalism by obtaining rank 3 Fjarnskagg
            QuickMount              = 227252
        };

        class spell_gen_herbalism_trap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_herbalism_trap_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* l_Caster = GetCaster()->ToPlayer();
                if (!l_Caster)
                    return;

                switch ( GetSpellInfo()->Effects[effIndex].BasePoints)
                {
                    case eSpells::LegionHerbalistLearning:
                    {
                        if (!l_Caster->HasSpell(eSpells::LegionHerbalistSkill))
                            l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                        break;
                    }
                    case eSpells::SummonNightfallen:
                    case eSpells::SummonDreamleaf:
                    case eSpells::SummonNightmareCreeper:
                    case eSpells::SummonFrenziedFox:
                    case eSpells::SummonNightfallen2:
                    {
                        if (roll_chance_i(15))
                            l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                        break;
                    }
                    case eSpells::QuickMount:
                    {
                        if (!l_Caster->HasSpell(eSpells::FjarnskagglRank3))
                            break;

                        l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                        break;
                    }
                    case eSpells::SummonAethrilGrow:
                    {
                        if (!l_Caster->HasSpell(eSpells::AethrilRank3))
                            break;

                        if (roll_chance_i(20))
                            l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_herbalism_trap_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_herbalism_trap_SpellScript();
        }
};

/// 185496, 185528, 185529, 185768, 185769, 185770, 204918, 204981, 205009, 205060, 205802, 205804, 205805
/// Broken Isles - mining nodes
class spell_gen_mining_trap : public SpellScriptLoader
{
    public:
        spell_gen_mining_trap() : SpellScriptLoader("spell_gen_mining_trap") { }

        enum eSpells
        {
            LegionMiningLearning = 201706,
            LegionMiningSkill    = 195122,

            SummonOreProtector   = 185498,
            SummonOreProtector2  = 185776,

            SummonRethuA         = 185505,  ///< Related to Rethu's Lesson quest, unlock rank 3 of Leystone Deposit
            SummonRethuB         = 185530,  ///< Related to Rethu's Experience quest, unlock rank 3 of Leystone Seam
            SummonRethuC         = 185531,  ///< Related to Rethu's Sacrifice quest, unlock rank 3 of Living Leystone
            SummonRinThissa      = 185772,  ///< Related to Rin'thissa quest, unlock rank 3 of Felslate Deposit
            SummonLyrelle        = 185774,  ///< Related to Lyrelle quest, unlock rank 3 of Felslate Seam
            SummonOndri          = 185775,  ///< Related to Ondri quest, unlock rank 3 of Living Felslate

            LeystoneDepositRank2 = 184456,
            LeystoneSeamRank2    = 184485,
            LivingLeystoneRank2  = 184489,
            FelslateDepositRank2 = 184493,
            FelslateSeamRank2    = 184497,
            LivingFelslateRank2  = 184501

        };

        class spell_gen_mining_trap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mining_trap_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* l_Caster = GetCaster()->ToPlayer();
                if (!l_Caster)
                    return;

                switch (GetSpellInfo()->Effects[effIndex].BasePoints)
                {
                    case eSpells::LegionMiningLearning:
                    {
                        if (!l_Caster->HasSpell(eSpells::LegionMiningSkill))
                            l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                        break;
                    }
                    case eSpells::SummonOreProtector:
                    case eSpells::SummonOreProtector2:
                    {
                        if (roll_chance_i(15))
                            l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                        break;
                    }
                    case eSpells::SummonRethuA:
                    {
                        if (l_Caster->HasSpell(eSpells::LeystoneDepositRank2))
                        {
                            if (roll_chance_f(0.5f))
                                l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                        }
                        break;
                    }
                    case eSpells::SummonRethuB:
                    {
                        if (l_Caster->HasSpell(eSpells::LeystoneSeamRank2))
                        {
                            if (roll_chance_f(0.5f))
                                l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                        }
                        break;
                    }
                    case eSpells::SummonRethuC:
                    {
                        if (l_Caster->HasSpell(eSpells::LivingLeystoneRank2))
                        {
                            if (roll_chance_f(0.5f))
                                l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_mining_trap_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_mining_trap_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Archaeology Trap - 203082, 203098 , 203100
/// Broken Isles - Archaeology traps
class spell_gen_archaeology_trap : public SpellScriptLoader
{
    public:
        spell_gen_archaeology_trap() : SpellScriptLoader("spell_gen_archaeology_trap") { }

        class spell_gen_archaeology_trap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_archaeology_trap_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* l_Caster = GetCaster()->ToPlayer();
                if (!l_Caster)
                    return;

                if (!GetSpellInfo()->Effects[effIndex].BasePoints)
                    return;

                if (roll_chance_f(5.0f))
                    l_Caster->CastSpell(l_Caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_archaeology_trap_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_archaeology_trap_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Fishing - 131474
class spell_gen_fishing : public SpellScriptLoader
{
    public:
        spell_gen_fishing() : SpellScriptLoader("spell_gen_fishing") { }

        class spell_gen_fishing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_fishing_SpellScript);

            enum eSpells
            {
                Fishing = 131476
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::Fishing, true);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_fishing_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_fishing_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Adaptation - 214027
class spell_gen_adapation : public SpellScriptLoader
{
    public:
        spell_gen_adapation() : SpellScriptLoader("spell_gen_adapation") { }

        class  spell_gen_adapation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_adapation_AuraScript);

            enum eSpells
            {
                MedallionOfImmunity = 46227,
                HonorableMedallion  = 195710,
                Adapted             = 195901,
                SolarBeamInterrupt  = 81261,
                KidneyShot          = 408,
                AdaptationID        = 214027
            };

            bool HandleCheckProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return false;

                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                Spell const* l_ProcSpell = l_DamageInfo->GetSpell();
                Unit* l_Attacker = l_DamageInfo->GetActor();
                if (!l_ProcSpellInfo || !l_ProcSpell || !l_Attacker || !l_ProcSpellInfo->_IsLossOfControl() || l_ProcSpellInfo->Id == eSpells::SolarBeamInterrupt)
                    return false;

                if (l_Caster->HasAura(eSpells::Adapted) || (l_Caster->IsPlayer() && l_Caster->ToPlayer()->HasSpellCooldown(m_scriptSpellId)))
                    return false;

                int32 l_CPUsed = 0;
                if (l_ProcSpellInfo->Id == eSpells::KidneyShot)
                    l_CPUsed = l_Attacker->GetPower(Powers::POWER_COMBO_POINT);

                DiminishingGroup l_DiminishingGroup = GetDiminishingReturnsGroupForSpell(l_ProcSpellInfo, l_Attacker);
                DiminishingLevels l_DiminishingLevel = l_Caster->GetDiminishing(l_DiminishingGroup);
                l_DiminishingLevel = l_DiminishingLevel ? DiminishingLevels(l_DiminishingLevel - 1) : l_DiminishingLevel;
                int32 l_Duration = l_CPUsed ? (l_CPUsed + 1) * IN_MILLISECONDS : l_ProcSpellInfo->GetMaxDuration();
                int32 l_LimitDuration = GetDiminishingReturnsLimitDuration(l_ProcSpellInfo, l_ProcSpell->GetOriginalCaster());
                l_Caster->ApplyDiminishingToDuration(l_DiminishingGroup, l_Duration, l_ProcSpell->GetOriginalCaster() ? l_ProcSpell->GetOriginalCaster() : l_Caster, l_DiminishingLevel, l_LimitDuration);

                if (l_Duration < l_SpellInfo->Effects[EFFECT_0].BasePoints * IN_MILLISECONDS)
                    return false;

                return true;
            }

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo * l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_DamageInfo || !l_SpellInfo)
                    return;

                Unit* l_Attacker = l_DamageInfo->GetActor();
                Player* l_Player = l_Caster->ToPlayer();
                SpellInfo const* l_ProcSpellInfo = l_DamageInfo->GetSpellInfo();
                if (!l_Attacker || !l_Player || !l_ProcSpellInfo)
                    return;

                l_Player->AddDelayedEvent([l_Player]()
                {
                    if (l_Player)
                    {
                        l_Player->CastSpell(l_Player, eSpells::HonorableMedallion, true);
                        l_Player->CastSpell(l_Player, eSpells::MedallionOfImmunity, true);
                        l_Player->CastSpell(l_Player, eSpells::Adapted, true);

                        l_Player->SendArenaCooldown(eSpells::AdaptationID, 60 * TimeConstants::IN_MILLISECONDS, true);
                    }
                }, 750);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_gen_adapation_AuraScript::HandleCheckProc);
                OnProc += AuraProcFn(spell_gen_adapation_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_adapation_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Vim and Vigor - 195483
class spell_gen_vim_and_vigor : public SpellScriptLoader
{
    public:
        spell_gen_vim_and_vigor() : SpellScriptLoader("spell_gen_vim_and_vigor") { }

        class  spell_gen_vim_and_vigor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_vim_and_vigor_AuraScript);

            enum eSpells
            {
                VimAndVigor = 195488
            };

            void HandleOnPeriodic(AuraEffect const* /*p_AuraEffect*/)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                    return;

                if (l_Caster->GetHealthPct() < l_SpellInfo->Effects[EFFECT_1].BasePoints)
                    return;

                Aura* l_Aura = l_Caster->GetAura(eSpells::VimAndVigor);
                if (l_Aura && l_Aura->GetDuration() > l_SpellInfo->Effects[EFFECT_0].Amplitude)
                    return;

                l_Caster->CastSpell(l_Caster, eSpells::VimAndVigor, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_vim_and_vigor_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_vim_and_vigor_AuraScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Hardiness - 195416
class spell_gen_hardiness : public SpellScriptLoader
{
    public:
        spell_gen_hardiness() : SpellScriptLoader("spell_gen_hardiness") { }

        class  spell_gen_hardiness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_hardiness_AuraScript);

            void HandleOnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Caster || !l_SpellInfo)
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                if (l_Caster->GetHealthPct() < l_SpellInfo->Effects[EFFECT_1].BasePoints)
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                p_AbsorbAmount = CalculatePct(p_DamageInfo.GetAmount(), l_SpellInfo->Effects[EFFECT_0].BasePoints);
                p_AuraEffect->ChangeAmount(p_AbsorbAmount + 1);
            }

            void HandleAfterEffectApply(AuraEffect const* p_AuraEffect, AuraEffectHandleModes /*p_Modes*/)
            {
                const_cast<AuraEffect*>(p_AuraEffect)->ChangeAmount(1);
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_hardiness_AuraScript::HandleOnAbsorb, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectApply += AuraEffectApplyFn(spell_gen_hardiness_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_hardiness_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Every Man For Himself - 59752 - Honorable Medallion - 195710 - Gladiator's Medallion - 208683
class spell_gen_every_man_for_himself : public SpellScriptLoader
{
    public:
        spell_gen_every_man_for_himself() : SpellScriptLoader("spell_gen_every_man_for_himself") { }

        class spell_gen_every_man_for_himself_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_every_man_for_himself_SpellScript);

            enum eSpells
            {
                EveryManForHimself  = 59752,
                HonorableMedallion  = 195710,
                GladiatorsMedallion = 208683,
                Stoneform           = 20594,
                Adaptation          = 214027
            };

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                switch (m_scriptSpellId)
                {
                    case eSpells::EveryManForHimself:
                        l_Player->AddSpellCooldown(eSpells::HonorableMedallion, 0, 30 * IN_MILLISECONDS, true);
                        l_Player->AddSpellCooldown(eSpells::GladiatorsMedallion, 0, 30 * IN_MILLISECONDS, true);
                        l_Player->AddSpellCooldown(eSpells::Adaptation, 0, 30 * IN_MILLISECONDS, true);
                        break;
                    case eSpells::HonorableMedallion:
                    case eSpells::GladiatorsMedallion:
                        l_Player->AddSpellCooldown(eSpells::EveryManForHimself, 0, 30 * IN_MILLISECONDS, true);
                        l_Player->AddSpellCooldown(eSpells::Stoneform, 0, 30 * IN_MILLISECONDS, true);
                        break;
                    default:
                        break;
                }

                if (m_scriptSpellId == eSpells::GladiatorsMedallion)
                    l_Player->SendArenaCooldown(m_scriptSpellId, 120 * TimeConstants::IN_MILLISECONDS);
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_every_man_for_himself_SpellScript::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_every_man_for_himself_SpellScript();
        }
};

/// Last Update 7.0.3 Build 22522
/// Called by Sparring - 195425
class spell_gen_sparring : public SpellScriptLoader
{
    public:
        spell_gen_sparring() : SpellScriptLoader("spell_gen_sparring") { }

        class  spell_gen_sparring_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_sparring_AuraScript);

            void HandleOnAbsorb(AuraEffect* p_AuraEffect, DamageInfo& p_DamageInfo, uint32& p_AbsorbAmount)
            {
                SpellSchoolMask l_SpellSchoolMask = p_DamageInfo.GetSchoolMask();
                if (l_SpellSchoolMask > SpellSchoolMask::SPELL_SCHOOL_MASK_NORMAL)
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_SpellInfo)
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                if (!roll_chance_i(l_SpellInfo->Effects[EFFECT_0].BasePoints))
                {
                    p_AbsorbAmount = 0;
                    return;
                }

                p_AbsorbAmount = CalculatePct(p_DamageInfo.GetAmount(), l_SpellInfo->Effects[EFFECT_1].BasePoints);
                p_AuraEffect->SetAmount(p_AbsorbAmount + 1);
                return;
            }

            void Register() override
            {
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_sparring_AuraScript::HandleOnAbsorb, EFFECT_0 ,SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_sparring_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Intimidated - 206891
class spell_gen_intimidated : public SpellScriptLoader
{
    public:
        spell_gen_intimidated() : SpellScriptLoader("spell_gen_intimidated") { }

        class spell_gen_intimidated_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_intimidated_AuraScript);

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                DamageInfo* l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                Aura* l_Aura = GetAura();
                if (!l_Target || !l_DamageInfo || !l_Caster || !l_Aura)
                    return;

                Unit* l_Attacker = l_DamageInfo->GetActor();
                if (!l_Attacker || l_Attacker == l_Target)
                    return;

                if (l_Attacker->IsPetGuardianStuff())
                    return;

                std::set<uint64>& l_Attackers = l_Target->m_SpellHelper.GetUint64Set()[eSpellHelpers::Intimidated];

                if (l_Attackers.find(l_Attacker->GetGUID()) != l_Attackers.end())
                    l_Aura->RefreshDuration();
                else
                {
                    l_Caster->CastSpell(l_Target, m_scriptSpellId, true);
                    l_Attackers.insert(l_Attacker->GetGUID());
                }
            }

            void HandleAfterApply(AuraEffect const* /*p_AuraEffect*/, AuraEffectHandleModes /*p_Modes*/)
            {
                Unit* l_Target = GetTarget();
                Unit* l_Caster = GetCaster();
                if (!l_Target)
                    return;

                l_Target->m_SpellHelper.GetUint64Set()[eSpellHelpers::Intimidated].clear();
                l_Target->m_SpellHelper.GetUint64Set()[eSpellHelpers::Intimidated].insert(l_Caster->GetGUID());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_intimidated_AuraScript::HandleAfterApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                OnProc += AuraProcFn(spell_gen_intimidated_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_intimidated_AuraScript();
        }
};

/// Last Update 7.1.5
/// Called by All Spells listed Below
class spell_gen_armor_specialization : public SpellScriptLoader
{
    public:
        spell_gen_armor_specialization() : SpellScriptLoader("spell_gen_armor_specialization") { }

        class  spell_gen_armor_specialization_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_armor_specialization_AuraScript);

            enum eSpells
            {
                LeatherSpecializationFirst      = 86092,
                LeatherSpecializationSecond     = 86093,
                LeatherSpecializationThird      = 86096,
                LeatherSpecializationFourth     = 86097,
                LeatherSpecializationFifth      = 86104,
                LeatherSpecializationSixth      = 120224,
                LeatherSpecializationSeventh    = 120225,
                LeatherSpecializationEighth     = 120227,
                LeatherSpecializationNinth      = 178976,
                LeatherSpecializationTenth      = 226359,

                MailSpecializationFirst         = 86099,
                MailSpecializationSecond        = 86100,
                MailSpecializationThird         = 86108,
                MailSpecializationFourth        = 86538,

                PlateSpecializationFirst        = 86101,
                PlateSpecializationSecond       = 86102,
                PlateSpecializationThird        = 86103,
                PlateSpecializationFourth       = 86110,
                PlateSpecializationFifth        = 86113,
                PlateSpecializationSixth        = 86535,
                PlateSpecializationSeventh      = 86536,
                PlateSpecializationEighth       = 86537,
                PlateSpecializationNinth        = 86539,

                SpellNethermancy                = 86091,
                SpellWizardry                   = 89744,
                SpellMysticism                  = 89745
            };

            std::vector<uint32> m_LeatherSpecializationSpells =
            {
                eSpells::LeatherSpecializationFirst,
                eSpells::LeatherSpecializationSecond,
                eSpells::LeatherSpecializationThird,
                eSpells::LeatherSpecializationFourth,
                eSpells::LeatherSpecializationFifth,
                eSpells::LeatherSpecializationSixth,
                eSpells::LeatherSpecializationSeventh,
                eSpells::LeatherSpecializationEighth,
                eSpells::LeatherSpecializationNinth,
                eSpells::LeatherSpecializationTenth
            };

            std::vector<uint32> m_MailSpecializationSpells =
            {
                eSpells::MailSpecializationFirst,
                eSpells::MailSpecializationSecond,
                eSpells::MailSpecializationThird,
                eSpells::MailSpecializationFourth
            };

            std::vector<uint32> m_PlateSpecializationSpells =
            {
                eSpells::PlateSpecializationFirst,
                eSpells::PlateSpecializationSecond,
                eSpells::PlateSpecializationThird,
                eSpells::PlateSpecializationFourth,
                eSpells::PlateSpecializationFifth,
                eSpells::PlateSpecializationSixth,
                eSpells::PlateSpecializationSeventh,
                eSpells::PlateSpecializationEighth,
                eSpells::PlateSpecializationNinth
            };

            std::vector<uint32> m_ClothSpecializationSpells =
            {
                eSpells::SpellNethermancy,
                eSpells::SpellWizardry,
                eSpells::SpellMysticism
            };

            std::vector<uint32> m_SlotsToCheck =
            {
                EquipmentSlots::EQUIPMENT_SLOT_HEAD,
                EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS,
                EquipmentSlots::EQUIPMENT_SLOT_CHEST,
                EquipmentSlots::EQUIPMENT_SLOT_WRISTS,
                EquipmentSlots::EQUIPMENT_SLOT_HANDS,
                EquipmentSlots::EQUIPMENT_SLOT_WAIST,
                EquipmentSlots::EQUIPMENT_SLOT_LEGS,
                EquipmentSlots::EQUIPMENT_SLOT_FEET
            };

            void HandleCalculateAmount(AuraEffect const* /*p_AuraEffect*/, int32& p_Amount, bool& /*p_CanBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                {
                    p_Amount = 0;
                    return;
                }

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                {
                    p_Amount = 0;
                    return;
                }

                uint8 l_ItemSubClass = 0;
                if (std::find(m_LeatherSpecializationSpells.begin(), m_LeatherSpecializationSpells.end(), m_scriptSpellId) != m_LeatherSpecializationSpells.end())
                    l_ItemSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
                else if (std::find(m_MailSpecializationSpells.begin(), m_MailSpecializationSpells.end(), m_scriptSpellId) != m_MailSpecializationSpells.end())
                    l_ItemSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
                else if (std::find(m_PlateSpecializationSpells.begin(), m_PlateSpecializationSpells.end(), m_scriptSpellId) != m_PlateSpecializationSpells.end())
                    l_ItemSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_PLATE;
                else if (std::find(m_ClothSpecializationSpells.begin(), m_ClothSpecializationSpells.end(), m_scriptSpellId) != m_ClothSpecializationSpells.end())
                    l_ItemSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;

                if (!l_ItemSubClass)
                {
                    p_Amount = 0;
                    return;
                }

                for (uint32 l_Slot : m_SlotsToCheck)
                {
                    Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Slot);
                    if (!l_Item)
                    {
                        p_Amount = 0;
                        return;
                    }

                    ItemTemplate const* l_ItemTemplate = l_Item->GetTemplate();
                    if (!l_ItemTemplate || l_ItemTemplate->GetSubClass() != l_ItemSubClass)
                    {
                        p_Amount = 0;
                        return;
                    }
                }

                p_Amount = 5;
            }

            void Register() override
            {
                uint32 l_Effect = SpellEffIndex::EFFECT_0;
                switch (m_scriptSpellId)
                {
                    case eSpells::LeatherSpecializationSecond:
                        l_Effect = SpellEffIndex::EFFECT_1;
                        break;
                    case eSpells::LeatherSpecializationThird:
                        l_Effect = SpellEffIndex::EFFECT_2;
                        break;
                    case eSpells::LeatherSpecializationFourth:
                        l_Effect = SpellEffIndex::EFFECT_0;
                        break;
                    case eSpells::LeatherSpecializationFifth:
                        l_Effect = SpellEffIndex::EFFECT_1;
                        break;
                }

                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_armor_specialization_AuraScript::HandleCalculateAmount, l_Effect, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_armor_specialization_AuraScript();
        }
};

///< Disable cosmetic in instanciated zones.
class PlayerScript_gen_disable_cosmetics: public PlayerScript
{
    public:
        PlayerScript_gen_disable_cosmetics() :PlayerScript("PlayerScript_Disable_Cosmetics") {}

        void OnTeleport(Player* p_Player, SpellInfo const* /*p_Spell*/, bool p_WorldPort) override
        {
            if (!p_WorldPort)
                return;

            p_Player->EnableCosmetic(p_Player->GetMap()->GetEntry()->IsWorldMap());
        }
};

///< Resummon Bodyguard if needed at at login
class PlayerScript_Summon_Bodyguard : public PlayerScript
{
    public:
        PlayerScript_Summon_Bodyguard() :PlayerScript("PlayerScript_Summon_Bodyguard") {}

        void OnLogin(Player* p_Player) override
        {
            p_Player->RecastCombatAllySpellIfNeeded();
        }

        void OnTeleport(Player* p_Player, SpellInfo const* /*p_Spell*/, bool) override
        {
            p_Player->RecastCombatAllySpellIfNeeded();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Gunpowder Charges - 202039
class spell_gen_gunpowder_charges : public SpellScriptLoader
{
    public:
        spell_gen_gunpowder_charges() : SpellScriptLoader("spell_gen_gunpowder_charges") { }

        class spell_gen_gunpowder_charges_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gunpowder_charges_SpellScript);

            std::array<uint32, 3> const m_CreaturesEntry = { {92326, 100561, 114216} };

            SpellCastResult HandleOnCheckCast()
            {
                Unit* l_Target = GetExplTargetUnit();
                if (!l_Target)
                    return SPELL_FAILED_BAD_TARGETS;

                uint32 l_TargetEntry = l_Target->GetEntry();

                for (uint32 l_Entry : m_CreaturesEntry)
                    if (l_Entry == l_TargetEntry)
                        return SPELL_CAST_OK;

                return SPELL_FAILED_BAD_TARGETS;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_gunpowder_charges_SpellScript::HandleOnCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_gunpowder_charges_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Resurrecting - 160029
class spell_gen_resurrecting : public SpellScriptLoader
{
    public:
        spell_gen_resurrecting() : SpellScriptLoader("spell_gen_resurrecting") { }

        class spell_gen_resurrecting_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_resurrecting_AuraScript);

            enum eSpells
            {
                Resurrecting = 160029
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target || l_Target->isDead())
                    return;

                l_Target->RemoveAura(eSpells::Resurrecting);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_resurrecting_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_resurrecting_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Toss Explosive Barrel - 123199
class spell_gen_toss_explosive_barrel : public SpellScriptLoader
{
    public:
        spell_gen_toss_explosive_barrel() : SpellScriptLoader("spell_gen_toss_explosive_barrel") {}

        class spell_gen_toss_explosive_barrel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_toss_explosive_barrel_SpellScript);

            enum eSpells
            {
                FireintheYaungHole = 129358
            };

            void HandleAfterHit()
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    ProcessAchievement(l_Player, l_Target);
                }
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_gen_toss_explosive_barrel_SpellScript::HandleAfterHit);
            }

        private:

            void ProcessAchievement(Player* p_Player, Unit* p_Target)
            {
                std::set<uint32> m_Creatures =
                {
                    62568, ///< Gai - Cho Gatewatcher
                    62577, ///< Gai - Cho Cauterizer
                    62602, ///< Kri'thik Screecher
                    62572, ///< Kri'thik Aggressor
                    62589  ///< Gai - Cho Boltshooter
                };

                if (m_Creatures.find(p_Target->GetEntry()) == m_Creatures.end())
                    return;

                if (p_Target->isAlive())
                    return;

                p_Player->StartCriteriaTimer(CriteriaTimedTypes::CRITERIA_TIMED_TYPE_SPELL_TARGET2, eSpells::FireintheYaungHole);

                /// It should be 'CastSpell' but the spell has wrong TargetA.
                p_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_BE_SPELL_TARGET, eSpells::FireintheYaungHole);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_toss_explosive_barrel_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Obliterum - 183484
class spell_gen_obliterum : public SpellScriptLoader
{
    public:
        spell_gen_obliterum() : SpellScriptLoader("spell_gen_obliterum") { }

        class spell_gen_obliterum_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_obliterum_SpellScript);

            std::array<uint32, 11> m_ObliterumItemBonuses =
            {
                { 669, 670, 671, 672, 673, 674, 675, 676, 677, 678, 679 }
            };

            SpellCastResult HandleOnCheckCast()
            {
                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                if (!l_Item->IsSoulBound())
                    return SPELL_FAILED_NOT_SOULBOUND;

                std::vector<uint32> const& l_ItemBonuses = l_Item->GetAllItemBonuses();

                bool l_Found = false;
                for (uint32 l_ItemBonus : l_ItemBonuses)
                {
                    auto l_Itr = std::find(m_ObliterumItemBonuses.begin(), m_ObliterumItemBonuses.end(), l_ItemBonus);
                    if (l_Itr != m_ObliterumItemBonuses.end() && ++l_Itr != m_ObliterumItemBonuses.end())
                        l_Found = true;
                }

                if (!l_Found)
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_obliterum_SpellScript::HandleOnCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_obliterum_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Called By Obliterum - 249954
class spell_gen_primal_obliterum : public SpellScriptLoader
{
    public:
        spell_gen_primal_obliterum() : SpellScriptLoader("spell_gen_primal_obliterum") { }

        class spell_gen_primal_obliterum_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_primal_obliterum_SpellScript);

            std::array<uint32, 11> m_PrimalObliterumItemBonuses =
            {
                { 3598, 3599, 3600, 3601, 3602, 3603, 3604, 3605, 3606, 3607, 3608 }
            };

            SpellCastResult HandleOnCheckCast()
            {
                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                if (!l_Item->IsSoulBound())
                    return SPELL_FAILED_NOT_SOULBOUND;

                std::vector<uint32> const& l_ItemBonuses = l_Item->GetAllItemBonuses();

                bool l_Found = false;
                for (uint32 l_ItemBonus : l_ItemBonuses)
                {
                    auto l_Itr = std::find(m_PrimalObliterumItemBonuses.begin(), m_PrimalObliterumItemBonuses.end(), l_ItemBonus);
                    if (l_Itr != m_PrimalObliterumItemBonuses.end() && ++l_Itr != m_PrimalObliterumItemBonuses.end())
                        l_Found = true;
                }

                if (!l_Found)
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_primal_obliterum_SpellScript::HandleOnCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_primal_obliterum_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Obliterate - 193024
class spell_gen_obliterate : public SpellScriptLoader
{
    public:
        spell_gen_obliterate() : SpellScriptLoader("spell_gen_obliterate") { }

        class spell_gen_obliterate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_obliterate_SpellScript);

            SpellCastResult HandleOnCheckCast()
            {
                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                ItemTemplate const* l_ItemTemplate = l_Item->GetTemplate();
                if (!l_ItemTemplate)
                    return SPELL_FAILED_NO_VALID_TARGETS;

                if (!(l_ItemTemplate->Flags3 & static_cast<uint32>(ItemFlagsEX2::OBLITERATABLE)))
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_obliterate_SpellScript::HandleOnCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_obliterate_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Learning - 483
class spell_gen_learning : public SpellScriptLoader
{
    public:
        spell_gen_learning() : SpellScriptLoader("spell_gen_learning") { }

        class spell_gen_learning_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_learning_SpellScript);

            SpellCastResult HandleOnCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                uint32 l_LearnedSpell = GetSpellValue()->EffectBasePoints[SpellValueMod::SPELLVALUE_BASE_POINT0];
                if (!l_LearnedSpell)
                    return SPELL_FAILED_DONT_REPORT;

                uint32 l_PrevSpellInChain = sSpellMgr->GetPrevSpellInChain(l_LearnedSpell);
                if (l_PrevSpellInChain && !l_Caster->HasSpell(l_PrevSpellInChain))
                    return SPELL_FAILED_MUST_KNOW_SUPERCEDING_SPELL;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_learning_SpellScript::HandleOnCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_learning_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by all the lust effects from the enum below
class spell_gen_lust : public SpellScriptLoader
{
    public:
        spell_gen_lust() : SpellScriptLoader("spell_gen_lust") { }

        class spell_gen_lust_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lust_AuraScript);

            enum eSpells
            {
                BloodLust           = 2825,
                Heroism             = 32182,
                TimeWarp            = 80353,
                Netherwinds         = 160452,
                AncientHysteria     = 90355,
                DrumsOfFury         = 178207,
                DrumsOfTheMountain  = 230935,
                DrumsOfRage         = 146555
            };

            void HandleAfterEffectApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Owner = GetUnitOwner();
                static const int32 m_Spells[8] =
                {
                    BloodLust,
                    Heroism,
                    TimeWarp,
                    Netherwinds,
                    AncientHysteria,
                    DrumsOfFury,
                    DrumsOfTheMountain,
                    DrumsOfRage
                };

                if (!l_Owner)
                    return;

                for (int32 l_Spell : m_Spells)
                {
                    if (l_Spell == m_scriptSpellId)
                        continue;

                    l_Owner->RemoveAurasDueToSpell(l_Spell);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_lust_AuraScript::HandleAfterEffectApply, EFFECT_0, SPELL_AURA_MELEE_SLOW, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_lust_AuraScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Arcane Torrent - 28730
class spell_gen_arcane_torrent : public SpellScriptLoader
{
    public:
        spell_gen_arcane_torrent() : SpellScriptLoader("spell_gen_arcane_torrent") { }

        class spell_gen_arcane_torrent_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_arcane_torrent_SpellScript);

            void HandleHit()
            {
                PreventHitDefaultEffect(SpellEffIndex::EFFECT_1);
                PreventHitDefaultEffect(SpellEffIndex::EFFECT_2);
                PreventHitDefaultEffect(SpellEffIndex::EFFECT_3);
            }

            void HandleAfterCast()
            {
                Unit* l_Caster = GetCaster();
                SpellInfo const* l_SpellInfo = GetSpellInfo();

                if (!l_Caster || !l_SpellInfo)
                    return;

                Energize(l_Caster, l_SpellInfo, SpellEffIndex::EFFECT_1);
                Energize(l_Caster, l_SpellInfo, SpellEffIndex::EFFECT_2);
                Energize(l_Caster, l_SpellInfo, SpellEffIndex::EFFECT_3);
            }

            void Energize(Unit* p_Caster, SpellInfo const* p_SpellInfo, int32 p_EffIndex)
            {
                int32 l_PowerType = (p_SpellInfo->EffectCount >= (p_EffIndex + 1)) ? p_SpellInfo->Effects[p_EffIndex].MiscValue : -1;

                if (l_PowerType >= 0)
                {
                    uint32 l_Value = p_SpellInfo->Effects[p_EffIndex].BasePoints;

                    if (p_SpellInfo->Effects[p_EffIndex].Effect == SPELL_EFFECT_ENERGIZE_PCT)
                        l_Value = CalculatePct(p_Caster->GetMaxPower(static_cast<Powers>(l_PowerType)), l_Value);

                    p_Caster->EnergizeBySpell(p_Caster, m_scriptSpellId, l_Value, static_cast<Powers>(l_PowerType));
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_gen_arcane_torrent_SpellScript::HandleAfterCast);
                BeforeHit += SpellHitFn(spell_gen_arcane_torrent_SpellScript::HandleHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_arcane_torrent_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Capturing - 97388
class spell_gen_capturing : public SpellScriptLoader
{
    public:
        spell_gen_capturing() : SpellScriptLoader("spell_gen_capturing") { }

        class spell_gen_capturing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_capturing_SpellScript);

            std::array<uint32, 3> const m_CreaturesEntry = { {92326, 100561, 114216} };

            SpellCastResult HandleOnCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_ERROR;

                if (l_Caster->HasAura(213602))
                    return SPELL_FAILED_TRY_AGAIN;

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_capturing_SpellScript::HandleOnCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_capturing_SpellScript();
        }
};

/// Rocket Blast - 96211
class spell_gen_sa_rocket_blast : public SpellScriptLoader
{
    public:
        spell_gen_sa_rocket_blast() : SpellScriptLoader("spell_gen_sa_rocket_blast") { }

        class spell_gen_sa_rocket_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_sa_rocket_blast_SpellScript);

            void HandleDamage(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Target = GetHitUnit();
                if (l_Target == nullptr)
                    return;

                uint32 l_Level = GetCaster()->getLevelForTarget(l_Target);
                int32 l_Damage = 500 * l_Level;

                /// This is pretty hacky, but I have no clue how damage should be calculated
                SetHitDamage(irand(l_Damage, l_Damage * 4));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_sa_rocket_blast_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_sa_rocket_blast_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Stonehide Leather Barding - 208705
class PlayerScript_gen_stonehide_leather_barding : public PlayerScript
{
    public:
        PlayerScript_gen_stonehide_leather_barding() :PlayerScript("PlayerScript_stonehide_leather_barding") {}

        enum eSpells
        {
            StonehideLeatherBarding = 208705
        };

        enum eMaps
        {
            BrokenIsles = 1220
        };

        enum eMapAreas
        {
            BrokenShore     = 7543,
            Azsuna          = 7334,
            Valsharah       = 7558,
            Stormheim       = 7541,
            Dalaran         = 7502,
            Highmountain    = 7503,
            Suramar         = 7637,
            DalaranSewers   = 8392,
            TheEyeOfAzshara = 7578
        };

        void OnMount(Player* p_Player, uint32 /*p_CreatureID*/)
        {
            if (!p_Player->HasAura(eSpells::StonehideLeatherBarding))
                return;

            if (p_Player->GetMapId() != eMaps::BrokenIsles)
                return;

            switch (p_Player->GetZoneId(true))
            {
                case eMapAreas::BrokenShore:
                case eMapAreas::Azsuna:
                case eMapAreas::Valsharah:
                case eMapAreas::Stormheim:
                case eMapAreas::Dalaran:
                case eMapAreas::Highmountain:
                case eMapAreas::Suramar:
                case eMapAreas::DalaranSewers:
                case eMapAreas::TheEyeOfAzshara:
                    p_Player->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, true);
                    break;
                default:
                    break;
            }
        }

        void OnDismount(Player* p_Player)
        {
            if (p_Player->HasAura(eSpells::StonehideLeatherBarding))
                p_Player->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, false);
        }

        void OnUpdateZone(Player* p_Player, uint32 p_NewZoneID, uint32 p_OldZoneID, uint32 /*p_NewAreaID*/)
        {
            if (p_NewZoneID == p_OldZoneID)
                return;

            if (!p_Player->HasAura(eSpells::StonehideLeatherBarding))
                return;

            if (p_Player->GetMapId() != eMaps::BrokenIsles)
            {
                p_Player->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, false);
                return;
            }

            switch (p_NewZoneID)
            {
                case eMapAreas::BrokenShore:
                case eMapAreas::Azsuna:
                case eMapAreas::Valsharah:
                case eMapAreas::Stormheim:
                case eMapAreas::Dalaran:
                case eMapAreas::Highmountain:
                case eMapAreas::Suramar:
                case eMapAreas::DalaranSewers:
                case eMapAreas::TheEyeOfAzshara:
                    p_Player->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, true);
                    break;
                default:
                    p_Player->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, false);
                    break;
            }
        }
};

/// Last Update 7.1.5 Build 23420
/// Called By Stonehide Leather Barding - 208705
class spell_gen_stonehide_leather_barding : public SpellScriptLoader
{
    public:
        spell_gen_stonehide_leather_barding() : SpellScriptLoader("spell_gen_stonehide_leather_barding") { }

        class spell_gen_stonehide_leather_barding_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_stonehide_leather_barding_AuraScript);

            enum eMaps
            {
                BrokenIsles = 1220
            };

            enum eMapAreas
            {
                BrokenShore     = 7543,
                Azsuna          = 7334,
                Valsharah       = 7558,
                Stormheim       = 7541,
                Dalaran         = 7502,
                Highmountain    = 7503,
                Suramar         = 7637,
                DalaranSewers   = 8392,
                TheEyeOfAzshara = 7578
            };

            std::vector<eMapAreas> m_Areas =
            {
                eMapAreas::BrokenShore,
                eMapAreas::Azsuna,
                eMapAreas::Valsharah,
                eMapAreas::Stormheim,
                eMapAreas::Dalaran,
                eMapAreas::Highmountain,
                eMapAreas::Suramar,
                eMapAreas::DalaranSewers,
                eMapAreas::TheEyeOfAzshara
            };

            void OnApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Aura* l_Aura = GetAura();
                if (!l_Caster || !l_Aura)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasSkill(SKILL_LEATHERWORKING))
                    l_Aura->SetDuration(GetMaxDuration() * 4);

                if (l_Player->GetMapId() != eMaps::BrokenIsles || std::find(m_Areas.begin(), m_Areas.end(), l_Player->GetZoneId()) == m_Areas.end())
                    return;

                if (!l_Player->IsMounted())
                    return;

                l_Player->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, true);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ApplySpellImmune(0, SpellImmunity::IMMUNITY_MECHANIC, Mechanics::MECHANIC_DAZE, false);
            }

            bool HandleCanRefreshProc(bool p_Decrease)
            {
                return false;
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_stonehide_leather_barding_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_stonehide_leather_barding_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                CanRefreshProc += AuraCanRefreshProcFn(spell_gen_stonehide_leather_barding_AuraScript::HandleCanRefreshProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_stonehide_leather_barding_AuraScript();
        }
};

/// Last Update 7.3.5 Build 23420
/// Recently Dropped Flag - 50326
/// Teleport - 66550
/// Teleport - 66551
/// Carrying Huge Seaforium - 68377
/// Rocket Boots Engaged - 30452
/// Goblin Glider - 126389
/// Alliance Glider - 294383
/// Horde Glider - 294384
/// Horde Flag Carrier Faction Reaction - 141204
/// Glider Test - 143951
/// Glider Test - 160277
/// Wings of the Outcasts - 175771
/// Pterrordax Swoop - 281954
/// Goblin Glider - 292427
/// Shadowy Duel - 207756
class spell_gen_check_flag_picker : public SpellScriptLoader
{
    public:
        spell_gen_check_flag_picker() : SpellScriptLoader("spell_gen_check_flag_picker") { }

        class spell_gen_check_flag_picker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_check_flag_picker_SpellScript);


            SpellCastResult HandleCheckCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Player* l_Player = l_Caster->ToPlayer())
                    {
                        Map* l_map = l_Player->GetMap();
                        if (l_map && (l_map->IsRaid() || l_map->IsDungeon()))
                            return SpellCastResult::SPELL_FAILED_NOT_HERE;

                        if (Battleground* l_Bg = l_Player->GetBattleground())
                        {
                            const GuidList l_FlagPickers1 = l_Bg->GetFlagPickersGUID(TeamId::TEAM_ALLIANCE);
                            const GuidList l_FlagPickers2 = l_Bg->GetFlagPickersGUID(TeamId::TEAM_HORDE);

                            if (l_FlagPickers1.find(l_Player->GetGUID()) != l_FlagPickers1.end() ||
                                l_FlagPickers2.find(l_Player->GetGUID()) != l_FlagPickers2.end())
                            {
                                SetCustomCastResultMessage(SpellCustomErrors::SPELL_CUSTOM_ERROR_HOLDING_FLAG);
                                return SpellCastResult::SPELL_FAILED_CUSTOM_ERROR;
                            }
                        }
                    }
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_check_flag_picker_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_check_flag_picker_SpellScript();
        }
};

class spell_gen_check_flag : public SpellScriptLoader
{
public:
    spell_gen_check_flag() : SpellScriptLoader("spell_gen_check_flag") { }

    class spell_gen_check_flag_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_check_flag_SpellScript);


        SpellCastResult HandleCheckCast()
        {
            if (Unit* l_Caster = GetCaster())
            {
                if (Player* l_Player = l_Caster->ToPlayer())
                {
                    if (Battleground* l_Bg = l_Player->GetBattleground())
                    {
                        const GuidList l_FlagPickers1 = l_Bg->GetFlagPickersGUID(TeamId::TEAM_ALLIANCE);
                        const GuidList l_FlagPickers2 = l_Bg->GetFlagPickersGUID(TeamId::TEAM_HORDE);

                        if (l_FlagPickers1.find(l_Player->GetGUID()) != l_FlagPickers1.end() ||
                            l_FlagPickers2.find(l_Player->GetGUID()) != l_FlagPickers2.end())
                        {
                            SetCustomCastResultMessage(SpellCustomErrors::SPELL_CUSTOM_ERROR_HOLDING_FLAG);
                            return SpellCastResult::SPELL_FAILED_CUSTOM_ERROR;
                        }
                    }
                }
            }

            return SpellCastResult::SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_gen_check_flag_SpellScript::HandleCheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_check_flag_SpellScript();
    }
};

/// Update to Legion 7.1.5 build: 23420
/// Dampening - 110310
class spell_gen_dampening : public SpellScriptLoader
{
    public:
        spell_gen_dampening() : SpellScriptLoader("spell_gen_dampening") { }

        class spell_gen_dampening_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_dampening_AuraScript);

            void OnTick(AuraEffect const* p_AurEff)
            {
                if (AuraEffect* l_FirstEffect = p_AurEff->GetBase()->GetEffect(EFFECT_0))
                {
                    if (l_FirstEffect->GetAmount() < 100)
                        l_FirstEffect->SetAmount(l_FirstEffect->GetAmount() + 1);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_dampening_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_dampening_AuraScript();
        }
};

enum Spells
{
    SPELL_FLURRY_OF_XUEN                = 146195, // item aura
    SPELL_FLURRY_OF_XUEN_VISUAL         = 148957,

    SPELL_SPIRIT_OF_CHI_JI              = 146199, // item aura
    SPELL_SPIRIT_OF_CHI_JI_VISUAL       = 148956,

    SPELL_ENDURANCE_OF_NIUZAO           = 146193, // item aura
    SPELL_ENDURANCE_OF_NIUZAO_VISUAL    = 148958,

    SPELL_ESSENSE_OF_YULON              = 146197, // item aura
    SPELL_ESSENSE_OF_YULON_VISUAL       = 148954,

    DK_SPELL_SHROUD_OF_PURGATORY        = 116888,
};

class spell_item_legendary_cloak_visual : public SpellScriptLoader
{
    public:
        spell_item_legendary_cloak_visual() : SpellScriptLoader("spell_legendary_cloak_visual")
        {
        }

        class spell_legendary_cloak_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_legendary_cloak_visual_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (!GetUnitOwner())
                    return;

                if (GetUnitOwner()->isInCombat())
                    return;

                if (!roll_chance_f(20.f))
                    return;

                uint32 visualSpellId = 0;

                switch (GetId())
                {
                    case SPELL_FLURRY_OF_XUEN: visualSpellId = SPELL_FLURRY_OF_XUEN_VISUAL; break;
                    case SPELL_SPIRIT_OF_CHI_JI: visualSpellId = SPELL_SPIRIT_OF_CHI_JI_VISUAL; break;
                    case SPELL_ENDURANCE_OF_NIUZAO: visualSpellId = SPELL_ENDURANCE_OF_NIUZAO_VISUAL; break;
                    case SPELL_ESSENSE_OF_YULON: visualSpellId = SPELL_ESSENSE_OF_YULON_VISUAL; break;
                }

                if (visualSpellId)
                {
                    GetUnitOwner()->CastSpell(GetUnitOwner(), visualSpellId, true);
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_legendary_cloak_visual_AuraScript::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_legendary_cloak_visual_AuraScript();
        }
};

class PlayerScript_gen_shadowmeld : public PlayerScript
{
    public:
        PlayerScript_gen_shadowmeld() :PlayerScript("PlayerScript_gen_shadowmeld") {}

        enum eSpells
        {
            Shadowmeld = 58984
        };

        void OnUpdate(Player* p_Player, uint32 /*p_Diff*/)
        {
            if (p_Player->getRace() != Races::RACE_NIGHTELF || !p_Player->IsMoving() || !p_Player->HasAura(eSpells::Shadowmeld))
                return;

            p_Player->RemoveAura(eSpells::Shadowmeld);
        }
};

/// Quick Punch - 172057
/// Strong Sweep - 172602
/// Jump Attack - 172604
class spell_gen_luchador_attacks : public SpellScriptLoader
{
    public:
        spell_gen_luchador_attacks() : SpellScriptLoader("spell_gen_luchador_attacks") { }

        enum eSpells
        {
            SpellQuickPunchDamage       = 172057,
            SpellQuickPunchOverride     = 172614,

            SpellStrongSweepDamage      = 172602,
            SpellStrongSweepOverride    = 172615,

            SpellJumpAttackDamage       = 172604,
            SpellJumpAttackOverride     = 172616
        };

        class spell_gen_luchador_attacks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_luchador_attacks_SpellScript);

            void FilterTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject const* p_Target) -> bool
                {
                    if (Unit const* l_Target = p_Target->ToUnit())
                    {
                        /// If target has none of the possible costumes, don't affect it
                        if (!l_Target->HasAura(eSpells::SpellQuickPunchOverride) &&
                            !l_Target->HasAura(eSpells::SpellStrongSweepOverride) &&
                            !l_Target->HasAura(eSpells::SpellJumpAttackOverride))
                            return true;

                        return false;
                    }

                    return false;
                });
            }

            void Register() override
            {
                switch (m_scriptSpellId)
                {
                    case eSpells::SpellQuickPunchDamage:
                    {
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_luchador_attacks_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_ENEMY_BETWEEN_DEST_2);
                        break;
                    }
                    case eSpells::SpellStrongSweepDamage:
                    {
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_luchador_attacks_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_110);
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_luchador_attacks_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_110);
                        break;
                    }
                    case eSpells::SpellJumpAttackDamage:
                    {
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_luchador_attacks_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_luchador_attacks_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_luchador_attacks_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Horde Flag - 23333 - Alliance Flag - 23335
class spell_gen_warsong_gulch_flags : public SpellScriptLoader
{
    public:
        spell_gen_warsong_gulch_flags() : SpellScriptLoader("spell_warsong_gulch_flags") { }

        class spell_warsong_gulch_flags_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warsong_gulch_flags_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                Unit* l_Target = GetTarget();
                if (!l_Target)
                    return;

                uint32 l_SpellId = m_scriptSpellId;

                if (l_Target->IsMounted())
                    l_Target->AddDelayedEvent([l_Target, l_SpellId]()-> void
                    {
                        l_Target->RemoveAurasDueToSpell(l_SpellId);
                    }, 0);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warsong_gulch_flags_AuraScript::HandleEffectPeriodic, EFFECT_4, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_warsong_gulch_flags_AuraScript();
        }
};

/// Shadow Sight - 34709
class spell_gen_shadow_sight : public SpellScriptLoader
{
    public:
        spell_gen_shadow_sight() : SpellScriptLoader("spell_gen_shadow_sight") { }

        class spell_gen_shadow_sight_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_shadow_sight_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    l_Target->RemoveAurasByType(AuraType::SPELL_AURA_MOD_INVISIBILITY);
                    l_Target->RemoveAurasByType(AuraType::SPELL_AURA_MOD_STEALTH);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_shadow_sight_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DETECT_STEALTH, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_shadow_sight_AuraScript();
        }
};

/// Last Update - 7.3.5 Build 26365
/// Called by Stealth - 1784, Prowl - 5215, 102547
class spell_gen_shadow_sight_check : public SpellScriptLoader
{
    public:
        spell_gen_shadow_sight_check() : SpellScriptLoader("spell_gen_shadow_sight_check") { }

        class spell_gen_shadow_sight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_shadow_sight_SpellScript);

            enum eSpells
            {
                ShadowSight = 34709
            };

            SpellCastResult CheckCast()
            {
                Unit* l_Caster = GetCaster();
                Spell const* l_Spell = GetSpell();
                if (l_Caster != nullptr && l_Spell != nullptr && l_Caster->HasAura(eSpells::ShadowSight) && l_Spell->m_FromClient)
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_shadow_sight_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_shadow_sight_SpellScript();
        }
};

/// Shaman Events Debug - 193450
/// Renamed "PvE Events Debug"
class spell_gen_shaman_events_debug : public SpellScriptLoader
{
    public:
        spell_gen_shaman_events_debug() : SpellScriptLoader("spell_gen_shaman_events_debug") { }

        class spell_gen_shaman_events_debug_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_shaman_events_debug_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                    l_Player->SetCommandStatusOn(PlayerCommandStates::CHEAT_GOD);
            }

            void AfterRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster())
                    return;

                if (Player* l_Player = GetCaster()->ToPlayer())
                {
                    l_Player->SetCommandStatusOff(PlayerCommandStates::CHEAT_GOD);

                    /// Reapply the aura 30 seconds later if necessary
                    AuraRemoveMode l_RemoveMode = GetTargetApplication()->GetRemoveMode();
                    if (l_RemoveMode != AuraRemoveMode::AURA_REMOVE_BY_CANCEL)
                        return;

                    /*l_Player->AddDelayedEvent([l_Player]() -> void
                    {
                        if (InstanceScript* l_Instance = l_Player->GetInstanceScript())
                        {
                            /// If still inside a test raid, reapply the aura
                            if (sObjectMgr->IsDisabledMap(l_Instance->instance->GetId(), l_Instance->instance->GetDifficultyID()))
                            {
                                if (sObjectMgr->GetItemContextOverride(l_Instance->instance->GetId(), l_Instance->instance->GetDifficultyID()) == ItemContext::None)
                                    l_Player->CastSpell(l_Player, eInstanceSpells::SpellPvEEventsDebug, true);
                            }
                        }
                    }, 30 * TimeConstants::IN_MILLISECONDS);*/
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_shaman_events_debug_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_shaman_events_debug_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_shaman_events_debug_AuraScript();
        }
};

/// Last Update 7.3.2 Build 25549
/// Concordance of the Legionfall - 239042
class spell_gen_concordance_of_the_legionfall : public SpellScriptLoader
{
public:
    spell_gen_concordance_of_the_legionfall() : SpellScriptLoader("spell_gen_concordance_of_the_legionfall") { }

    class spell_gen_concordance_of_the_legionfall_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_concordance_of_the_legionfall_AuraScript);

        enum eSpells
        {
            AgilityDPS          = 162697,
            StrengthDPS         = 162698,
            IntellectDPS        = 162699,
            AgilityTank         = 162700,
            IntellectHealer     = 162701,
            StrengthTank        = 162702,

            StatStrength        = 242583,
            StatAgility         = 242584,
            StatIntellect       = 242586,
            StatVersatility     = 243096,

            MurderousIntent     = 252191,
            MurderousIntentBuff = 252202,
            Shocklight          = 252799,
            ShocklightBuff      = 252801,
            InsigniaOfTheGrandArmy = 251977
        };

        enum eArtifactPowers
        {
            MurderousIntentArtifact = 1774,
            ShocklightArtifact = 1777
        };

        std::map<uint32, uint32> m_Spells
        {
            { AgilityDPS, StatAgility },
            { StrengthDPS, StatStrength },
            { IntellectDPS, StatIntellect },
            { AgilityTank, StatVersatility },
            { IntellectHealer, StatIntellect },
            { StrengthTank, StatVersatility }
        };

        void HandleOnProc(AuraEffect const* p_AurEff, ProcEventInfo& p_EventInfo)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            for (auto l_Spells : m_Spells)
            {
                if (l_Caster->HasAura(l_Spells.first))
                {
                    int32 l_Bp = p_AurEff->GetAmount();
                    l_Caster->CastCustomSpell(l_Caster, l_Spells.second, &l_Bp, NULL, NULL, true);
                    break;
                }
            }

            if (l_Caster->HasAura(eSpells::MurderousIntent))
            {
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::MurderousIntentArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::MurderousIntentArtifact) -1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                l_Caster->CastCustomSpell(l_Caster, eSpells::MurderousIntentBuff, &l_Amount, NULL, NULL, true);
            }

            if (l_Caster->HasAura(eSpells::Shocklight))
            {
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::ShocklightArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::ShocklightArtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                l_Caster->CastCustomSpell(l_Caster, eSpells::ShocklightBuff, &l_Amount, NULL, NULL, true);
            }
        }

        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_gen_concordance_of_the_legionfall_AuraScript::HandleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_concordance_of_the_legionfall_AuraScript();
    }
};

/// Last Update 7.3.2 build 25549
/// Called for Concordance of the Legionfall - 239042
/// Called by Concordance of the Legionfall - 242583, 242584, 242586, 243096
class spell_gen_concordance_of_the_legionfall_stat : public SpellScriptLoader
{
    public:
        spell_gen_concordance_of_the_legionfall_stat() : SpellScriptLoader("spell_gen_concordance_of_the_legionfall_stat") { }

        class spell_gen_concordance_of_the_legionfall_stat_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_concordance_of_the_legionfall_stat_AuraScript);

            enum eSpells
            {
                StatVersatility = 243096
            };

            void HandlePvPModifier(AuraEffect const* l_AuraEffect, int32& p_Amount, bool& p_CanBeRecalculated)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Player->CanApplyPvPSpellModifiers())
                    return;

                p_Amount *= 0.35f;   ///< Concordance of the Legionfall is reduced by 65% in PvP
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::StatVersatility)
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_concordance_of_the_legionfall_stat_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_RATING);
                else
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_concordance_of_the_legionfall_stat_AuraScript::HandlePvPModifier, EFFECT_0, SPELL_AURA_MOD_STAT);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_concordance_of_the_legionfall_stat_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
/// Arcane Pulse - 260364
class spell_gen_arcane_pulse : public SpellScriptLoader
{
public:
    spell_gen_arcane_pulse() : SpellScriptLoader("spell_gen_arcane_pulse") { }

    class spell_gen_arcane_pulse_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_arcane_pulse_SpellScript);

        void HandleDamage(SpellEffIndex /*p_EffIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            int32 l_SpellPower = l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
            int32 l_AttackPower = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

            int32 l_Damage = l_AttackPower * 2.0f;
            if (l_SpellPower > l_AttackPower)
                l_Damage = l_SpellPower * 0.75f;
            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_arcane_pulse_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_arcane_pulse_SpellScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Light's Judgment - 256893
class spell_gen_lights_judgment : public SpellScriptLoader
{
public:
    spell_gen_lights_judgment() : SpellScriptLoader("spell_gen_lights_judgment") { }

    class spell_gen_lights_judgment_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_lights_judgment_SpellScript);

        void HandleDamage(SpellEffIndex p_EffIndex)
        {
            Unit* l_Caster = GetCaster();
            Unit* l_Target = GetHitUnit();
            if (l_Caster == nullptr || l_Target == nullptr)
                return;

            int32 l_SpellPower = l_Caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);
            int32 l_AttackPower = l_Caster->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);

            int32 l_Damage = l_AttackPower * 6.25f;
            if (l_SpellPower > l_AttackPower)
                l_Damage = l_SpellPower * 5;

            l_Damage = l_Caster->SpellDamageBonusDone(l_Target, GetSpellInfo(), (uint32)l_Damage, p_EffIndex, DamageEffectType::SPELL_DIRECT_DAMAGE);
            l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), (uint32)l_Damage, DamageEffectType::SPELL_DIRECT_DAMAGE, p_EffIndex);

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectLaunchTarget += SpellEffectFn(spell_gen_lights_judgment_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_lights_judgment_SpellScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Rugged Tenacity - 255659
class spell_gen_rugged_tenacity : public SpellScriptLoader
{
public:
    spell_gen_rugged_tenacity() : SpellScriptLoader("spell_gen_rugged_tenacity") { }

    class spell_gen_rugged_tenacity_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_rugged_tenacity_AuraScript);

        void OnUpdate(uint32 p_Diff)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            if (m_UpdateTimer <= p_Diff)
            {
                if (AuraEffect* l_AuraEffect = GetEffect(EFFECT_0))
                {
                    int32 l_Amount = int32(l_Caster->GetStat(STAT_STAMINA) * 0.0003f * 60.0f) + 1;
                    l_AuraEffect->SetAmount(-l_Amount);
                }
                m_UpdateTimer = 1 * IN_MILLISECONDS;
            }
            else
            {
                m_UpdateTimer -= p_Diff;
            }
        }

        void Register() override
        {
            OnAuraUpdate += AuraUpdateFn(spell_gen_rugged_tenacity_AuraScript::OnUpdate);
        }

    private:
        uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_rugged_tenacity_AuraScript();
    }
};

/// Last Update 7.3.5 Build 25996
/// Spatial Rift - 257040
class spell_gen_spatial_rift : public SpellScriptLoader
{
public:
    spell_gen_spatial_rift() : SpellScriptLoader("spell_gen_spatial_rift") { }

    class spell_gen_spatial_rift_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_spatial_rift_SpellScript);

        enum eSpells
        {
            SpatialRiftAreaTrigger = 256948,
            SpatialRiftTeleport = 257034
        };

        SpellCastResult CheckCast()
        {
            Unit* l_Caster = GetCaster();
            float l_MaxRadius = GetSpellInfo()->GetMaxRange(false);

            AreaTrigger* l_AreaTrigger = l_Caster->GetLastAreaTrigger(eSpells::SpatialRiftAreaTrigger);
            if (!l_AreaTrigger || !l_Caster->IsPlayer())
                return SPELL_FAILED_ERROR;

            if (l_Caster->GetDistance2d(l_AreaTrigger) > l_MaxRadius)
                return SPELL_FAILED_OUT_OF_RANGE;

            if (Battleground* l_BG = l_Caster->ToPlayer()->GetBattleground())
                if (l_BG->GetStatus() != STATUS_IN_PROGRESS)
                    return SPELL_FAILED_DONT_REPORT;

            return SPELL_CAST_OK;
        }

        void HandleDummy(SpellEffIndex p_EffIndex)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            AreaTrigger* l_AreaTrigger = l_Caster->GetLastAreaTrigger(eSpells::SpatialRiftAreaTrigger);
            if (!l_AreaTrigger)
                return;

            l_Caster->SetFacingToObject(l_AreaTrigger);
            float l_X = l_AreaTrigger->GetPositionX() - 1.8f * (std::cos(l_Caster->GetOrientation()));
            float l_Y = l_AreaTrigger->GetPositionY() - 1.8f * (std::sin(l_Caster->GetOrientation()));
            float l_Z = l_AreaTrigger->GetPositionZ();
            l_Caster->UpdateAllowedPositionZ(l_X, l_Y, l_Z);
            l_Caster->CastSpell(l_X, l_Y, l_Z, eSpells::SpatialRiftTeleport, true);
            l_AreaTrigger->Remove();
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_gen_spatial_rift_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_gen_spatial_rift_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_spatial_rift_SpellScript();
    }
};

/// Last Update 7.3.5 build 26124
/// Called By Mining - 195122 For Pride Of Ironhorn 255655
class spell_gen_pride_of_ironhorn : public SpellScriptLoader
{
    public:
        spell_gen_pride_of_ironhorn() : SpellScriptLoader("spell_gen_pride_of_ironhorn") { }

        class spell_gen_pride_of_ironhorn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pride_of_ironhorn_SpellScript);

            enum eSpells
            {
                PrideOfIronhorn = 255655,
                Mining          = 195122
            };

            void HandleOnPrepare()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Spell* l_Spell = GetSpell();
                SpellInfo const* l_SpellInfo = GetSpellInfo();
                if (!l_Spell || !l_SpellInfo)
                    return;

                if (l_Caster->HasAura(eSpells::PrideOfIronhorn) && l_SpellInfo->Id == eSpells::Mining)
                    if (l_Spell->GetCastTime() > 1700)
                        l_Spell->SetCastTime(1700);
            }

            void Register() override
            {
                OnPrepare += SpellOnPrepareFn(spell_gen_pride_of_ironhorn_SpellScript::HandleOnPrepare);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_pride_of_ironhorn_SpellScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called By Warboard - Alliance - 261654
class spell_gen_warboard_alliance : public SpellScriptLoader
{
    public:
        spell_gen_warboard_alliance() : SpellScriptLoader("spell_gen_warboard_alliance") { }

        class spell_gen_warboard_alliance_SpellScript : public SpellScript
        {
            enum eChoiceId
            {
                AllianceWarboard = 352
            };

            PrepareSpellScript(spell_gen_warboard_alliance_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return SPELL_FAILED_DONT_REPORT;

                if (!l_Player->SendPlayerChoice(l_Player->GetGUID(), eChoiceId::AllianceWarboard))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NOTHING_INTERESTING_POSTED_HERE_RIGHT_NOW);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_warboard_alliance_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_warboard_alliance_SpellScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called By Warboard - Horde - 261655
class spell_gen_warboard_horde : public SpellScriptLoader
{
    public:
        spell_gen_warboard_horde() : SpellScriptLoader("spell_gen_warboard_horde") { }

        class spell_gen_warboard_horde_SpellScript : public SpellScript
        {
            enum eChoiceId
            {
                HordeWarboard = 342
            };

            PrepareSpellScript(spell_gen_warboard_horde_SpellScript);

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return SPELL_FAILED_DONT_REPORT;

                if (!l_Player->SendPlayerChoice(l_Player->GetGUID(), eChoiceId::HordeWarboard))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NOTHING_INTERESTING_POSTED_HERE_RIGHT_NOW);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_warboard_horde_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_warboard_horde_SpellScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called By Warboard - Neutral - 256856
class spell_gen_warboard_neutral : public SpellScriptLoader
{
public:
    spell_gen_warboard_neutral() : SpellScriptLoader("spell_gen_warboard_neutral") { }

    class spell_gen_warboard_neutral_SpellScript : public SpellScript
    {
        enum eChoiceId
        {
            NeutralWarboard = 0 ///< not found
        };

        PrepareSpellScript(spell_gen_warboard_neutral_SpellScript);

        SpellCastResult HandleCheckCast()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return SPELL_FAILED_DONT_REPORT;

            Player* l_Player = l_Caster->ToPlayer();
            if (!l_Player)
                return SPELL_FAILED_DONT_REPORT;

            if (!l_Player->SendPlayerChoice(l_Player->GetGUID(), eChoiceId::NeutralWarboard))
            {
                SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NOTHING_INTERESTING_POSTED_HERE_RIGHT_NOW);
                return SPELL_FAILED_CUSTOM_ERROR;
            }

            return SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_gen_warboard_neutral_SpellScript::HandleCheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_warboard_neutral_SpellScript();
    }
};

class spell_gen_token_loot : public SpellScriptLoader
{
    public:
        spell_gen_token_loot() : SpellScriptLoader("spell_gen_token_loot") { }

        class spell_gen_token_loot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_token_loot_SpellScript);

            uint32 m_ItemID = 0;

            SpellCastResult HandleCheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                LootStore& l_LootStore = LootTemplates_Spell;
                LootTemplate const* l_LootTemplate = l_LootStore.GetLootFor(GetSpellInfo()->Id);
                if (l_LootTemplate == nullptr)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                std::list<ItemTemplate const*> l_LootTable;
                l_LootTemplate->FillAutoAssignationLoot(l_LootTable, l_Player, true);

                if (l_LootTable.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                uint32 l_SpecID = l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID();

                std::set<uint32> l_Items;
                for (ItemTemplate const* l_Template : l_LootTable)
                {
                    if (!l_Template->HasSpec((SpecIndex)l_SpecID, l_Player->getLevel()))
                        continue;

                    l_Items.insert(l_Template->ItemId);
                }

                if (l_Items.empty())
                    return SpellCastResult::SPELL_FAILED_ERROR;

                auto l_Itr = l_Items.begin();
                std::advance(l_Itr, urand(0, l_Items.size() - 1));

                m_ItemID = *l_Itr;
                if (!m_ItemID)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                {
                    l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffectIndex)
            {
                PreventHitDefaultEffect(p_EffectIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player || !m_ItemID)
                    return;

                l_Player->AddItem(m_ItemID, 1, {}, false, 0, false, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_token_loot_SpellScript::HandleCheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_gen_token_loot_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_CREATE_LOOT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_token_loot_SpellScript();
        }
};

/// Synthesize Legendary - 262946
class spell_gen_synthesize_legendary : public SpellScriptLoader
{
    public:
        spell_gen_synthesize_legendary() : SpellScriptLoader("spell_gen_synthesize_legendary") { }

        enum eTokenIDs
        {
            TokenWarrior        = 147305,
            TokenPaladin        = 147300,
            TokenHunter         = 147297,
            TokenRogue          = 147302,
            TokenPriest         = 147301,
            TokenDeathKnight    = 147294,
            TokenShaman         = 147303,
            TokenMage           = 147298,
            TokenWarlock        = 147304,
            TokenMonk           = 147299,
            TokenDruid          = 147296,
            TokenDemonHunter    = 147295
        };

        class spell_gen_synthesize_legendary_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_synthesize_legendary_SpellScript);

            uint32 m_ItemID = 0;

            std::map<uint32, uint32> m_TokensByClass =
            {
                { Classes::CLASS_WARRIOR,       eTokenIDs::TokenWarrior     },
                { Classes::CLASS_PALADIN,       eTokenIDs::TokenPaladin     },
                { Classes::CLASS_HUNTER,        eTokenIDs::TokenHunter      },
                { Classes::CLASS_ROGUE,         eTokenIDs::TokenRogue       },
                { Classes::CLASS_PRIEST,        eTokenIDs::TokenPriest      },
                { Classes::CLASS_DEATH_KNIGHT,  eTokenIDs::TokenDeathKnight },
                { Classes::CLASS_SHAMAN,        eTokenIDs::TokenShaman      },
                { Classes::CLASS_MAGE,          eTokenIDs::TokenMage        },
                { Classes::CLASS_WARLOCK,       eTokenIDs::TokenWarlock     },
                { Classes::CLASS_MONK,          eTokenIDs::TokenMonk        },
                { Classes::CLASS_DRUID,         eTokenIDs::TokenDruid       },
                { Classes::CLASS_DEMON_HUNTER,  eTokenIDs::TokenDemonHunter }
            };

            SpellCastResult HandleCheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                /// Source: http://www.wowhead.com/news=280377/legendary-changes-in-7-3-5
                /// If you are still missing legendaries for your spec, you will obtain one of the missing ones
                ItemTemplate const* l_LegendaryItem = sObjectMgr->GetItemLegendary(l_Player->GetLootSpecId() ? l_Player->GetLootSpecId() : l_Player->GetActiveSpecializationID(), l_Player);
                if (!l_LegendaryItem)
                {
                    /// If you have obtained all legendaries for your spec, but are still missing other legendaries for your class, you will get one of them at random
                    l_LegendaryItem = sObjectMgr->GetItemLegendaryForClass(l_Player->getClass(), l_Player);
                    if (!l_LegendaryItem)
                    {
                        /// If you have obtained all legendaries for your class, you will receive a Bind-on-Account token for a different class.
                        do
                        {
                            auto const& l_Iter = m_TokensByClass.find(urand(Classes::CLASS_WARRIOR, Classes::CLASS_DEMON_HUNTER));
                            if (l_Iter != m_TokensByClass.end() && l_Iter->first != l_Player->getClass())
                                m_ItemID = l_Iter->second;
                        }
                        while (m_ItemID == 0);
                    }
                    else
                        m_ItemID = l_LegendaryItem->ItemId;
                }
                else
                    m_ItemID = l_LegendaryItem->ItemId;

                if (!m_ItemID)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                {
                    l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffectIndex)
            {
                PreventHitDefaultEffect(p_EffectIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player || !m_ItemID)
                    return;

                l_Player->AddItem(m_ItemID, 1, {}, false, 0, false, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_synthesize_legendary_SpellScript::HandleCheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_gen_synthesize_legendary_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_LOOT_BONUS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_synthesize_legendary_SpellScript();
        }
};

/// Identify Legendary - 240485 / 240487 / 240508 / 240509 / 240510 / 240511 / 240512 / 240513 / 240514 / 240515 / 240516 / 240518
class spell_gen_identify_legendary : public SpellScriptLoader
{
    public:
        spell_gen_identify_legendary() : SpellScriptLoader("spell_gen_identify_legendary") { }

        enum eTokenIDs
        {
            TokenWarrior        = 147305,
            TokenPaladin        = 147300,
            TokenHunter         = 147297,
            TokenRogue          = 147302,
            TokenPriest         = 147301,
            TokenDeathKnight    = 147294,
            TokenShaman         = 147303,
            TokenMage           = 147298,
            TokenWarlock        = 147304,
            TokenMonk           = 147299,
            TokenDruid          = 147296,
            TokenDemonHunter    = 147295
        };

        class spell_gen_identify_legendary_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_identify_legendary_SpellScript);

            uint32 m_ItemID = 0;

            std::map<uint32, uint32> m_TokensByClass =
            {
                { Classes::CLASS_WARRIOR,       eTokenIDs::TokenWarrior     },
                { Classes::CLASS_PALADIN,       eTokenIDs::TokenPaladin     },
                { Classes::CLASS_HUNTER,        eTokenIDs::TokenHunter      },
                { Classes::CLASS_ROGUE,         eTokenIDs::TokenRogue       },
                { Classes::CLASS_PRIEST,        eTokenIDs::TokenPriest      },
                { Classes::CLASS_DEATH_KNIGHT,  eTokenIDs::TokenDeathKnight },
                { Classes::CLASS_SHAMAN,        eTokenIDs::TokenShaman      },
                { Classes::CLASS_MAGE,          eTokenIDs::TokenMage        },
                { Classes::CLASS_WARLOCK,       eTokenIDs::TokenWarlock     },
                { Classes::CLASS_MONK,          eTokenIDs::TokenMonk        },
                { Classes::CLASS_DRUID,         eTokenIDs::TokenDruid       },
                { Classes::CLASS_DEMON_HUNTER,  eTokenIDs::TokenDemonHunter }
            };

            SpellCastResult HandleCheckCast()
            {
                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                /// Source: http://www.wowhead.com/news=280377/legendary-changes-in-7-3-5
                /// If you are still missing legendaries for your class, you will obtain one of the missing ones
                ItemTemplate const* l_LegendaryItem = sObjectMgr->GetItemLegendaryForClass(l_Player->getClass(), l_Player);
                if (!l_LegendaryItem)
                {
                    /// If you have obtained all legendaries for your class, you will receive a Bind-on-Account token for a different class.
                    do
                    {
                        auto const& l_Iter = m_TokensByClass.find(urand(Classes::CLASS_WARRIOR, Classes::CLASS_DEMON_HUNTER));
                        if (l_Iter != m_TokensByClass.end() && l_Iter->first != l_Player->getClass())
                            m_ItemID = l_Iter->second;
                    }
                    while (m_ItemID == 0);
                }
                else
                    m_ItemID = l_LegendaryItem->ItemId;

                if (!m_ItemID)
                    return SpellCastResult::SPELL_FAILED_ERROR;

                ItemPosCountVec l_Destination;
                InventoryResult l_Message = l_Player->CanStoreNewItem(InventorySlot::NULL_BAG, InventorySlot::NULL_SLOT, l_Destination, m_ItemID, 1);

                if (l_Message != InventoryResult::EQUIP_ERR_OK)
                {
                    l_Player->SendEquipError(InventoryResult::EQUIP_ERR_INV_FULL, nullptr);
                    return SpellCastResult::SPELL_FAILED_DONT_REPORT;
                }

                return SpellCastResult::SPELL_CAST_OK;
            }

            void HandleCreateItem(SpellEffIndex p_EffectIndex)
            {
                PreventHitDefaultEffect(p_EffectIndex);

                Player* l_Player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
                if (!l_Player || !m_ItemID)
                    return;

                l_Player->AddItem(m_ItemID, 1, {}, false, 0, false, true);
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_identify_legendary_SpellScript::HandleCheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_gen_identify_legendary_SpellScript::HandleCreateItem, EFFECT_0, SPELL_EFFECT_LOOT_BONUS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_identify_legendary_SpellScript();
        }
};

/// Argus Filter - 258803
class spell_gen_argus_filter : public SpellScriptLoader
{
    public:
        spell_gen_argus_filter() : SpellScriptLoader("spell_gen_argus_filter") { }

        enum eBody
        {
            ArgusSkybox = 9
        };

        class spell_gen_argus_filter_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_argus_filter_AuraScript);

            void AfterApply(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (Unit* l_Target = GetTarget())
                {
                    if (!l_Target->IsPlayer())
                        return;

                    l_Target->AddDelayedEvent([l_Target]() -> void
                    {
                        l_Target->ToPlayer()->SendUpdateCelestialBody(eBody::ArgusSkybox);
                    }, 1 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_argus_filter_AuraScript::AfterApply, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_argus_filter_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Well Fed - 201336, 225601, 225606
/// Pepper Breath - 201580, 225621, 225622
class spell_gen_pepper_breath : public SpellScriptLoader
{
    public:
        spell_gen_pepper_breath() : SpellScriptLoader("spell_gen_pepper_breath") { }

        enum eSpells
        {
            WellFed1 = 201336,
            WellFed2 = 225601,
            WellFed3 = 225606,

            PepperBreath1 = 201580,
            PepperBreath2 = 225621,
            PepperBreath3 = 225622,

            PepperBreathDamage1 = 201573,
            PepperBreathDamage2 = 225623,
            PepperBreathDamage3 = 225624
        };

        class spell_gen_pepper_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_pepper_breath_AuraScript);

            void HandleOnEffectProc(AuraEffect const* /*p_AurEff*/, ProcEventInfo& p_EventInfo)
            {
                PreventDefaultAction();

                Unit* l_Caster = GetCaster();
                Unit* l_Target = p_EventInfo.GetActionTarget();
                if (!l_Caster || !l_Target)
                    return;

                l_Caster->CastSpell(l_Target, GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true);
            }

            void Register()
            {
                if (m_scriptSpellId == eSpells::WellFed1 ||
                    m_scriptSpellId == eSpells::WellFed2 ||
                    m_scriptSpellId == eSpells::WellFed3)
                OnEffectProc += AuraEffectProcFn(spell_gen_pepper_breath_AuraScript::HandleOnEffectProc, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_pepper_breath_AuraScript();
        }

        class spell_gen_pepper_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pepper_breath_SpellScript);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetHitUnit();
                if (!l_Caster || !l_Target)
                    return;

                uint32 l_SpellId = 0;
                switch (GetSpellInfo()->Id)
                {
                    case eSpells::PepperBreath1: l_SpellId = eSpells::PepperBreathDamage1; break;
                    case eSpells::PepperBreath2: l_SpellId = eSpells::PepperBreathDamage2; break;
                    case eSpells::PepperBreath3: l_SpellId = eSpells::PepperBreathDamage3; break;
                }

                if (!l_SpellId)
                    return;

                uint16 l_Delay = 0;

                for (uint8 i = 0; i < urand(4, 6); i++)
                {
                    l_Caster->DelayedCastSpell(l_Target, l_SpellId, true, l_Delay);
                    l_Delay += 200;
                }
            }

            void Register() override
            {
                if (m_scriptSpellId == eSpells::PepperBreath1 ||
                    m_scriptSpellId == eSpells::PepperBreath2 ||
                    m_scriptSpellId == eSpells::PepperBreath3)
                OnEffectHitTarget += SpellEffectFn(spell_gen_pepper_breath_SpellScript::HandleDummy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_pepper_breath_SpellScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Shadowbind - 252875
class spell_gen_shadowbind : public SpellScriptLoader
{
    public:
        spell_gen_shadowbind() : SpellScriptLoader("spell_gen_shadowbind") { }

        class  spell_gen_shadowbind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_shadowbind_AuraScript);

            enum eSpells
            {
                ShadowbindDamage = 252879,
                InsigniaOfTheGrandArmy = 251977
            };

            enum eArtifactPowers
            {
                ShadowbindDamageAtifact = 1778
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo * l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Target)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::ShadowbindDamageAtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::ShadowbindDamageAtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                l_Caster->CastCustomSpell(l_Target, eSpells::ShadowbindDamage, &l_Amount, NULL, NULL, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_shadowbind_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_shadowbind_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Chaotic Darkness - 252888
class spell_gen_chaotic_darkness : public SpellScriptLoader
{
    public:
        spell_gen_chaotic_darkness() : SpellScriptLoader("spell_gen_chaotic_darkness") { }

        class  spell_gen_chaotic_darkness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_chaotic_darkness_AuraScript);

            enum eSpells
            {
                ChaoticDarknessDamage = 252896,
                ChaoticDarknessHeal = 252897,
                InsigniaOfTheGrandArmy = 251977
            };

            enum eArtifactPowerIds
            {
                ChaoticDraknessArtifact = 1779
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo * l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Target)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowerIds::ChaoticDraknessArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowerIds::ChaoticDraknessArtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = urand(l_RankEntry->AuraPointsOverride, l_RankEntry->AuraPointsOverride * 5);
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                l_Caster->CastCustomSpell(l_Target, eSpells::ChaoticDarknessDamage, &l_Amount, NULL, NULL, true);
                l_Caster->CastCustomSpell(l_Target, eSpells::ChaoticDarknessHeal, &l_Amount, NULL, NULL, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_chaotic_darkness_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_chaotic_darkness_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Torment the Weak - 252906
class spell_gen_torment_the_weak : public SpellScriptLoader
{
    public:
        spell_gen_torment_the_weak() : SpellScriptLoader("spell_gen_torment_the_weak") { }

        class  spell_gen_torment_the_weak_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_torment_the_weak_AuraScript);

            enum eSpells
            {
                TormentOfTheWeakDamage = 252907,
                TormentOfTheWeak = 252906,
                InsigniaOfTheGrandArmy = 251977
            };

            enum eArtifactPowers
            {
                TormentOfTheWeakArt = 1780
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo * l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Target)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::TormentOfTheWeakArt, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::TormentOfTheWeakArt) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                l_Caster->CastCustomSpell(l_Target, eSpells::TormentOfTheWeakDamage, &l_Amount, NULL, NULL, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_torment_the_weak_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_torment_the_weak_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Dark Sorrows - 252922
class spell_gen_dark_sorrows : public SpellScriptLoader
{
    public:
        spell_gen_dark_sorrows() : SpellScriptLoader("spell_gen_dark_sorrows") { }

        class  spell_gen_dark_sorrows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_dark_sorrows_AuraScript);

            enum eSpells
            {
                Sorrow = 252921,
                InsigniaOfTheGrandArmy = 251977
            };

            enum eArtifactPowers
            {
                DarkSorrowArtifact = 1781
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo * l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Target)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::DarkSorrowArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::DarkSorrowArtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                l_Caster->CastCustomSpell(l_Target, eSpells::Sorrow, &l_Amount, NULL, NULL, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_dark_sorrows_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_dark_sorrows_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Sorrow - 252921
class spell_gen_sorrow : public SpellScriptLoader
{
    public:
        spell_gen_sorrow() : SpellScriptLoader("spell_gen_sorrow") { }

        class  spell_gen_sorrow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_sorrow_AuraScript);

            enum eSpells
            {
                SorrowAOEDamage = 253022
            };

            enum eArtifactPowers
            {
               DarkSorrowArtifact = 1781
            };

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                Unit* l_Target = GetTarget();
                if (!l_Caster || !l_Target)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::DarkSorrowArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::DarkSorrowArtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                l_Caster->CastCustomSpell(l_Target, eSpells::SorrowAOEDamage, &l_Amount, NULL, NULL, true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_sorrow_AuraScript::OnRemove, SpellEffIndex::EFFECT_0, AuraType::SPELL_AURA_DUMMY, AuraEffectHandleModes::AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_sorrow_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Master of Shadows - 252091
class spell_gen_master_of_shadows : public SpellScriptLoader
{
    public:
        spell_gen_master_of_shadows() : SpellScriptLoader("spell_gen_master_of_shadows") { }

        class spell_gen_master_of_shadows_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_master_of_shadows_AuraScript);

            enum eArtifactPowers
            {
                MasterOfShadowsArtifact = 1771
            };

            enum eSpells
            {
                InsigniaOfTheGrandArmy = 251977
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32 & p_Amount, bool & /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::MasterOfShadowsArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::MasterOfShadowsArtifact) - 1);
                if (!l_RankEntry)
                    return;

                p_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        p_Amount += CalculatePct(p_Amount, l_Effect->GetAmount());
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_master_of_shadows_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_master_of_shadows_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_master_of_shadows_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Light Speed - 252088
class spell_gen_light_speed : public SpellScriptLoader
{
    public:
        spell_gen_light_speed() : SpellScriptLoader("spell_gen_light_speed") { }

        class spell_gen_light_speed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_light_speed_AuraScript);

            enum eArtifactPowers
            {
                LightSpeedArtifact = 1770
            };

            enum eSpells
            {
                InsigniaOfTheGrandArmy = 251977
            };

            void CalculateAmount(AuraEffect const* /*p_AurEff*/, int32 & p_Amount, bool & /*canBeRecalculated*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::LightSpeedArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::LightSpeedArtifact) - 1);
                if (!l_RankEntry)
                    return;

                p_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        p_Amount += CalculatePct(p_Amount, l_Effect->GetAmount());
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_light_speed_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_light_speed_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_RATING);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_light_speed_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Refractive Shell - 252207
class spell_gen_refractive_shell : public SpellScriptLoader
{
    public:
        spell_gen_refractive_shell() : SpellScriptLoader("spell_gen_refractive_shell") { }

        class  spell_gen_refractive_shell_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_refractive_shell_AuraScript);

            enum eSpells
            {
                RefractiveShell = 252208,
                InsigniaOfTheGrandArmy = 251977
            };

            enum eArtifactPowers
            {
                RefractiveShellArtifact = 1775
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::RefractiveShellArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::RefractiveShellArtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                l_Caster->CastCustomSpell(l_Caster, eSpells::RefractiveShell, &l_Amount, NULL, NULL, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_refractive_shell_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_refractive_shell_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Secure in the Light - 253070
class spell_gen_secure_in_the_light : public SpellScriptLoader
{
    public:
        spell_gen_secure_in_the_light() : SpellScriptLoader("spell_gen_secure_in_the_light") { }

        class  spell_gen_secure_in_the_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_secure_in_the_light_AuraScript);

            enum eSpells
            {
                SecureInTheLightDamage = 253073,
                HolyBulwark = 253072,
                InsigniaOfTheGrandArmy = 251977
            };

            enum eArtifactPowers
            {
                SecureInTheLightArtifact = 1782
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo * l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Target)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::SecureInTheLightArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::SecureInTheLightArtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                if (l_DamageInfo->GetSpellInfo())
                {
                    if (l_DamageInfo->GetSpellInfo()->IsPositive())
                        l_Caster->CastCustomSpell(l_Caster, eSpells::HolyBulwark, &l_Amount, NULL, NULL, true);
                    else
                        l_Caster->CastCustomSpell(l_Target, eSpells::SecureInTheLightDamage, &l_Amount, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_secure_in_the_light_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_secure_in_the_light_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Infusion of Light - 253093
class spell_gen_infusion_of_light : public SpellScriptLoader
{
    public:
        spell_gen_infusion_of_light() : SpellScriptLoader("spell_gen_infusion_of_light") { }

        class  spell_gen_infusion_of_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_infusion_of_light_AuraScript);

            enum eSpells
            {
                InfusionOfLightDamage = 253098,
                InfusionOfLightHeal = 253099,
                InsigniaOfTheGrandArmy = 251977
            };

            enum eArtifactPowers
            {
                InfusionOfLightArtifact = 1783
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                DamageInfo * l_DamageInfo = p_ProcEventInfo.GetDamageInfo();
                if (!l_Caster || !l_DamageInfo)
                    return;

                Unit* l_Target = l_DamageInfo->GetTarget();
                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player || !l_Target)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::InfusionOfLightArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::InfusionOfLightArtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());

                if (l_DamageInfo->GetSpellInfo())
                {
                    if (l_DamageInfo->GetSpellInfo()->IsPositive())
                        l_Caster->CastCustomSpell(l_Caster, eSpells::InfusionOfLightHeal, &l_Amount, NULL, NULL, true);
                    else
                        l_Caster->CastCustomSpell(l_Target, eSpells::InfusionOfLightDamage, &l_Amount, NULL, NULL, true);
                }
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_infusion_of_light_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_infusion_of_light_AuraScript();
        }
};

/// Last Update 7.3.5 build 25996
/// Called by Light's Embrace - 253111
class spell_gen_lights_embrace : public SpellScriptLoader
{
    public:
        spell_gen_lights_embrace() : SpellScriptLoader("spell_gen_lights_embrace") { }

        class  spell_gen_lights_embrace_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lights_embrace_AuraScript);

            enum eSpells
            {
                LightsEmbraceHeal = 253216,
                InsigniaOfTheGrandArmy = 251977
            };

            enum eArtifactPowers
            {
                LightsEmbraceArtifact = 1784
            };

            void HandleOnProc(ProcEventInfo& p_ProcEventInfo)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                MS::Artifact::Manager* l_Manager = l_Player->GetCurrentlyEquippedArtifact();
                if (!l_Manager)
                    return;

                ArtifactPowerRankEntry const* l_RankEntry = GetArtifactPowerRank(eArtifactPowers::LightsEmbraceArtifact, l_Manager->GetCurrentRankWithBonus(eArtifactPowers::LightsEmbraceArtifact) - 1);
                if (!l_RankEntry)
                    return;

                int32 l_Amount = l_RankEntry->AuraPointsOverride;
                if (Aura* l_Aura = l_Player->GetAura(eSpells::InsigniaOfTheGrandArmy))
                    if (AuraEffect* l_Effect = l_Aura->GetEffect(EFFECT_0))
                        l_Amount += CalculatePct(l_Amount, l_Effect->GetAmount());


                l_Amount /= 3; ///< Ticks 3 times
                l_Caster->CastCustomSpell(l_Caster, eSpells::LightsEmbraceHeal, &l_Amount, NULL, NULL, true);
            }

            void Register() override
            {
                OnProc += AuraProcFn(spell_gen_lights_embrace_AuraScript::HandleOnProc);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_lights_embrace_AuraScript();
        }
};

/// Called by Titan Forging - 258408
class spell_gen_titan_forging : public SpellScriptLoader
{
    public:
        spell_gen_titan_forging() : SpellScriptLoader("spell_gen_titan_forging") { }

        class spell_gen_titan_forging_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_titan_forging_SpellScript);

            std::array<uint32, 13> m_TitanforgingBonuses =
            {
                { 3985, 3986, 3987, 3988, 3989, 3990, 3991, 3992, 3993, 3994, 3995, 3996, 3997 }
            };

            SpellCastResult HandleOnCheckCast()
            {
                Item* l_Item = GetExplTargetItem();
                if (!l_Item)
                    return SpellCastResult::SPELL_FAILED_NO_VALID_TARGETS;

                if (!l_Item->IsSoulBound())
                    return SpellCastResult::SPELL_FAILED_NOT_SOULBOUND;

                std::vector<uint32> const& l_ItemBonuses = l_Item->GetAllItemBonuses();

                bool l_Found = false;
                for (uint32 l_ItemBonus : l_ItemBonuses)
                {
                    auto l_Itr = std::find(m_TitanforgingBonuses.begin(), m_TitanforgingBonuses.end(), l_ItemBonus);
                    if (l_Itr != m_TitanforgingBonuses.end() && ++l_Itr != m_TitanforgingBonuses.end())
                        l_Found = true;
                }

                if (!l_Found)
                    return SpellCastResult::SPELL_FAILED_BAD_TARGETS;

                return SpellCastResult::SPELL_CAST_OK;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_titan_forging_SpellScript::HandleOnCheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_titan_forging_SpellScript();
        }
};

class spell_gen_magic_broom : public SpellScriptLoader
{
    public:
        spell_gen_magic_broom() : SpellScriptLoader("spell_gen_magic_broom") {}

        class spell_gen_magic_broom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_magic_broom_SpellScript);

            bool Load()
            {
                return GetCaster()->IsPlayer();
            }

            SpellCastResult HandleCheckCast()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                if (l_Caster->CanApplyPvPSpellModifiers())
                    return SPELL_FAILED_NOT_HERE;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_magic_broom_SpellScript::HandleCheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_magic_broom_SpellScript();
        }
};

class spell_emerland_ring : public SpellScriptLoader
{
public:
    spell_emerland_ring() : SpellScriptLoader("spell_emerland_ring") { }

    class spell_emerland_ring_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_emerland_ring_AuraScript);

        void OnStackChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (GetStackAmount() == 10)                
                target->CastSpell(target, 221583);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_emerland_ring_AuraScript::OnStackChange, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_emerland_ring_AuraScript();
    }
};

class spell_gen_flag_capture : public SpellScriptLoader
{
public:
    spell_gen_flag_capture() : SpellScriptLoader("spell_gen_flag_capture") { }

    class spell_gen_flag_capture_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_flag_capture_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (Player* target = GetHitUnit()->ToPlayer())
            {
                Unit* caster = GetCaster();

                switch (GetSpellInfo()->Id)
                {
                    case 70263:
                        if (target->GetTeam() == HORDE)
                            caster->CastSpell(caster, 70261);
                        else
                            caster->CastSpell(caster, 73301);
                        break;
                    case 70365:
                        if (target->GetTeam() == HORDE)
                            caster->CastSpell(caster, 70364);
                        else
                            caster->CastSpell(caster, 73328);
                        break;
                    case 70506:
                        if (target->GetTeam() == HORDE)
                            caster->CastSpell(caster, 70505);
                        else
                            caster->CastSpell(caster, 73329);
                        break;
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_flag_capture_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_flag_capture_SpellScript();
    }
};

class spell_gen_cinematic_timer : public SpellScriptLoader
{
public:
    spell_gen_cinematic_timer() : SpellScriptLoader("spell_gen_cinematic_timer") { }

    class spell_gen_cinematic_timer_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_cinematic_timer_AuraScript);

        enum eSpells
        {
            HordeTeleport = 94724,
            AlianceTeleport = 94723
        };

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Player* target = GetTarget()->ToPlayer())
            {
                if (target->GetTeam() == HORDE)
                    target->CastSpell(target, HordeTeleport, true);
                else
                    target->CastSpell(target, AlianceTeleport, true);
            }
        }

        void Register()
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_gen_cinematic_timer_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_cinematic_timer_AuraScript();
    }
};

static const std::unordered_map<uint32, uint32> miningLegionRelearnQuests =
{
    {38785, 191945 },
    {38784, 191944 },
    {38777, 191943 },
    {38790, 191951 },
    {38789, 191947 },
    {38791, 191953 },
    {38792, 191950 },
    {38793, 191952 },
    {38794, 191954 },
    {38795, 191956 },
    {38796, 191959 },
    {38797, 191964 },
    {38802, 191965 },
    {38801, 191961 },
    {38800, 191957 },
    {38805, 191966 },
    {38804, 191963 },
    {38803, 191958 },
    {38806, 191968 },
    {38807, 191969 },
    {39830, 191971 },
};

//201696
class spell_gen_relearn_mining_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_mining_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : miningLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_mining_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> enchantingLegionRelearnQuests =
{
    {39874, 201698 },
    {39875, 211528 },
    {39905, 208718 },
    {39883, 208686 },
    {39882, 190954 },
    {39904, 212398 },
    {39918, 208898 },
    {39910, 208896 },
    {39914, 208897 },
    {39923, 209015 },
};

//205504
class spell_gen_relearn_enchanting_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_enchanting_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : enchantingLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_enchanting_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> inscriptionLegionRelearnQuests =
{
    {39847, 201703 },
    {39931, 192962 },
    {39933, 193005 },
    {39950, 193386 },
    {39957, 193479 },
    {40057, 193050 },
    {39940, 210344 },
    {40060, 193049 },
    {39943, 193748 },
    {40052, 193363 },
    {39954, 210810 },
    {39960, 193542 },
    {39956, 193542 },
};

//205507
class spell_gen_relearn_inscription_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_inscription_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : inscriptionLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_inscription_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> tailoringLegionRelearnQuests =
{
    {38944, 201708 },
    {38945, 186017 },
    {38949, 186018 },
    {38953, 186019 },
    {38955, 186390 },
    {38957, 186020 },
    {38958, 186021 },
    {38963, 186023 },
    {38961, 186022 },
    {38966, 186025 },
    {38970, 186028 },
    {38974, 186611 },
    {38971, 186603 },
    {38975, 186623 },
    {48074, 248752 },
};

// 205510
class spell_gen_relearn_tailoring_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_tailoring_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : tailoringLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_tailoring_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> leatherworkingLegionRelearnQuests =
{
    {39958, 201705 },
    {40183, 194898 },
    {41889, 196574 },
    {40201, 196443 },
    {40179, 194862 },
    {40178, 194830 },
    {40181, 194891 },
    {40182, 194892 },
    {40186, 194952 },
    {40185, 194948 },
    {40192, 194997 },
    {40191, 194976 },
    {40198, 195098 },
    {40205, 216233 },
    {40203, 216231 },
    {40204, 216232 },
    {40189, 194965 },
    {40327, 196065 },
    {40194, 195003 },
    {40188, 194959 },
    {40199, 195101 },
    {40209, 216236 },
    {40207, 216234 },
    {40208, 216235 },
    {40215, 195314 },
    {40214, 195326 },
    {48078, 248798 },
};

//205509
class spell_gen_relearn_leatherworking_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_leatherworking_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : leatherworkingLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_leatherworking_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> herbalismLegionRelearnQuests =
{
    {40014, 193415 },
    {40016, 193416 },
    {40017, 193417 },
    {40019, 193418 },
    {40021, 193419 },
    {40023, 193420 },
    {40040, 193430 },
    {40041, 193431 },
    {40042, 193432 },
    {40029, 193424 },
    {40031, 193425 },
    {40033, 193426 },
    {40024, 193421 },
    {40026, 193422 },
    {40028, 193423 },
    {40035, 193427 },
    {40037, 193428 },
    {40039, 193429 },
};

//205501 
class spell_gen_relearn_herbalism_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_herbalism_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : herbalismLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_herbalism_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> skinningLegionRelearnQuests =
{
    {40142, 194820 },
    {40145, 194821 },
    {40155, 194822 },
    {40132, 194817 },
    {40135, 194818 },
    {40140, 194819 },
    {40156, 194823 },
    {40158, 194824 },
    {40159, 194825 },
    {40131, 201707 },
};

//205500
class spell_gen_relearn_skinning_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_skinning_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : skinningLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_skinning_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> blacksmithingLegionRelearnQuests =
{
    {39681, 191237 },
    {38501, 191243 },
    {38499, 201699 },
    {38500, 191338 },
    {39699, 191462 },
    {48055, 248589 },
};

//205503
class spell_gen_relearn_blacksmithing_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_blacksmithing_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
        {
            for (auto i : blacksmithingLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_blacksmithing_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> alchemyLegionRelearnQuests =
{
    {39325, 201697 },
    {48016, 248403 },
    {48013, 248402 },
    {48002, 248381 },
};

//205502
class spell_gen_relearn_alchemy_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_alchemy_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : alchemyLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_alchemy_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

static const std::unordered_map<uint32, uint32> jewelcraftingLegionRelearnQuests =
{
    {40523, 201704 },
    {40529, 197931 },
    {40538, 198797 },
    {40539, 200151 },
    {40546, 199526 },
    {40542, 199417 },
    {40561, 199785 },
    {40560, 199784 },
    {40559, 199783 },
    {48075, 248792 },
    {48076, 248797 },
    {40558, 200410 },
};

// 205508
class spell_gen_relearn_jewelcrafting_quests : public SpellScript
{
    PrepareSpellScript(spell_gen_relearn_jewelcrafting_quests);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (auto player = GetCaster()->ToPlayer())
            for (auto i : jewelcraftingLegionRelearnQuests)
                if (player->IsQuestRewarded(i.first))
                    player->DelayedCastSpell(player, i.second, true, 100);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_gen_relearn_jewelcrafting_quests::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_gen_call_to_xera : public SpellScriptLoader
{
public:
    spell_gen_call_to_xera() : SpellScriptLoader("spell_gen_call_to_xera") { }

    class spell_gen_call_to_xera_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_call_to_xera_SpellScript);

        enum eCredits
        {
            LightsHeart = 113857
        };

        void HandleAfterCast()
        {
            Unit* l_Caster = GetCaster();
            if (l_Caster == nullptr)
                return;

            Player* l_Player = l_Caster->ToPlayer();
            if (l_Player == nullptr)
                return;

            l_Player->KilledMonsterCredit(eCredits::LightsHeart);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_gen_call_to_xera_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_call_to_xera_SpellScript();
    }
};

// 167424 167457
class spell_gen_explore_quest_assassins_mark : public SpellScript
{
    PrepareSpellScript(spell_gen_explore_quest_assassins_mark);

    void HandleOnHit()
    {
        if (auto caster = GetCaster())
        {
            uint32 spellid = 0;
            switch (GetSpellInfo()->Id)
            {
            case 167424:
                if (roll_chance_i(30)) spellid = 167430; else spellid = 167446;
                break;
            case 167457:
                if (roll_chance_i(30)) spellid = 167456; else spellid = 167455;
                break;
            default:
                break;
            }
            if (spellid)
                caster->CastSpell(caster, spellid, true);
        }
    }

    void Register()
    {
        OnHit += SpellHitFn(spell_gen_explore_quest_assassins_mark::HandleOnHit);
    }
};


#ifndef __clang_analyzer__
void AddSC_generic_spell_scripts()
{
    RegisterSpellScript(spell_gen_relearn_mining_quests);
    RegisterSpellScript(spell_gen_explore_quest_assassins_mark);
    RegisterSpellScript(spell_gen_relearn_enchanting_quests);
    RegisterSpellScript(spell_gen_relearn_inscription_quests);
    RegisterSpellScript(spell_gen_relearn_tailoring_quests);
    RegisterSpellScript(spell_gen_relearn_leatherworking_quests);
    RegisterSpellScript(spell_gen_relearn_herbalism_quests);
    RegisterSpellScript(spell_gen_relearn_skinning_quests);
    RegisterSpellScript(spell_gen_relearn_blacksmithing_quests);
    RegisterSpellScript(spell_gen_relearn_alchemy_quests);
    RegisterSpellScript(spell_gen_relearn_jewelcrafting_quests);
    new spell_gen_flag_capture();
    new spell_emerland_ring();
    new spell_gen_mercenary_mode();
    new playerscript_mercenary_mode();
    new spell_gen_class_hall_restriction();
    new spell_gen_capturing();
    new spell_gen_arcane_torrent();
    new spell_gen_lust();
    new spell_gen_armor_specialization();
    new spell_gen_principles_of_war();
    new spell_gen_pvp_stat_spells();
    new spell_gen_pvp_rule_enabled();
    new spell_gen_pvp_stat_set_stat();
    new spell_gen_ritual_of_summoning();
    new spell_gen_appraisal();
    new spell_gen_pvp_trinket();
    new spell_gen_ironbeards_hat();
    new spell_gen_coin_of_many_faces();
    new spell_gen_jewel_of_hellfire();
    new spell_gen_jewel_of_hellfire_trigger();
    new spell_reconfigured_remote_shock();
    new spell_gen_demon_hunters_aspect();
    new spell_gen_wyrmhunter_hooks();
    new spell_gen_blood_elfe_illusion();
    new spell_gen_kilroggs_dead_eye();
    new spell_generic_iron_horde_pirate_costume();
    new spell_gen_mark_of_thunderlord();
    new spell_gen_inge_trigger_enchant();
    new spell_gen_potion_of_illusion();
    new spell_gen_alchemists_flask();
    new spell_gen_jards_peculiar_energy_source();
    new spell_gen_celestial_cloth_and_its_uses();
    new spell_gen_hardened_magnificient_hide_and_its_uses();
    new spell_gen_draenic_philosophers();
    new spell_gen_shadowmeld();
    new spell_gen_mark_of_warsong();
    new spell_gen_savage_fortitude();
    new spell_dru_touch_of_the_grave();
    new spell_gen_drums_of_fury();
    new spell_gen_absorb0_hitlimit1();
    new spell_gen_light_reckoning();
    new spell_crowd_control_ignore();
    new spell_gen_aura_of_anger();
    new spell_gen_av_drekthar_presence();
    new spell_gen_burn_brutallus();
    new spell_gen_cannibalize();
    new spell_gen_leeching_swarm();
    new spell_gen_parachute();
    new spell_gen_pet_summoned();
    new spell_gen_remove_flight_auras();
    new spell_creature_permanent_feign_death();
    new spell_pvp_trinket_wotf_shared_cd();
    new spell_gen_animal_blood();
    new spell_gen_divine_storm_cd_reset();
    new spell_gen_parachute_ic();
    new spell_gen_gunship_portal();
    new spell_gen_dungeon_credit();
    new spell_gen_profession_research();
    new spell_generic_clone();
    new spell_generic_clone_weapon();
    new spell_gen_clone_weapon_aura();
    new spell_gen_turkey_marker();
    new spell_gen_magic_rooster();
    new spell_gen_allow_cast_from_item_only();
    new spell_gen_launch();
    new spell_gen_vehicle_scaling();
    new spell_gen_oracle_wolvar_reputation();
    new spell_gen_luck_of_the_draw();
    new spell_gen_dummy_trigger();
    new spell_gen_spirit_healer_res();
    new spell_gen_gadgetzan_transporter_backfire();
    new spell_gen_gnomish_transporter();
    new spell_gen_dalaran_disguise("spell_gen_sunreaver_disguise");
    new spell_gen_dalaran_disguise("spell_gen_silver_covenant_disguise");
    new spell_gen_elune_candle();
    new spell_gen_break_shield("spell_gen_break_shield");
    new spell_gen_break_shield("spell_gen_tournament_counterattack");
    new spell_gen_mounted_charge();
    new spell_gen_defend();
    new spell_gen_tournament_duel();
    new spell_gen_summon_tournament_mount();
    new spell_gen_on_tournament_mount();
    new spell_gen_tournament_pennant();
    new spell_gen_chaos_blast();
    new spell_gen_wg_water();
    new spell_gen_despawn_self();
    new spell_gen_touch_the_nightmare();
    new spell_gen_dream_funnel();
    new spell_gen_bandage();
    new spell_gen_lifebloom("spell_hexlord_lifebloom", SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_tur_ragepaw_lifebloom", SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_cenarion_scout_lifebloom", SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_twisted_visage_lifebloom", SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_faction_champion_dru_lifebloom", SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_mount("spell_magic_broom", 0, 0, 0, SPELL_MAGIC_BROOM_150, SPELL_MAGIC_BROOM_280);
    new spell_gen_mount("spell_winged_steed_of_the_ebon_blade", 0, 0, 0, SPELL_WINGED_STEED_150, SPELL_WINGED_STEED_280);
    new spell_gen_mount("spell_blazing_hippogryph", 0, 0, 0, SPELL_BLAZING_HIPPOGRYPH_150, SPELL_BLAZING_HIPPOGRYPH_280);
    new spell_gen_upper_deck_create_foam_sword();
    new spell_gen_bonked();
    new spell_gen_gift_of_the_naaru();
    new spell_gen_running_wild();
    new spell_gen_two_forms();
    new spell_gen_darkflight();
    new spell_gen_blood_fury();
    new spell_gen_gobelin_gumbo();
    new spell_mage_polymorph_cast_visual();
    new spell_gen_hardened_shell();
    new spell_gen_ds_flush_knockback();
    new spell_gen_orb_of_power();
    new spell_gen_dampening();
    new spell_gen_selfie_camera();
    new spell_gen_selfie_lens_upgrade_kit();
    new spell_gen_carrying_seaforium();
    new spell_gen_inherit_master_threat_list();
    new spell_gen_taunt_flag_targeting();
    new spell_gen_sword_technique();
    //new spell_gen_check_faction(); -- temp disable
    new spell_gen_stoneform_dwarf_racial();
    new spell_gen_elixir_of_wandering_spirits();
    new spell_gen_service_uniform();
    new spell_nullification_barrier();
    new spell_gen_gunpowder_charges();
    new spell_gen_resurrecting();
    new spell_gen_toss_explosive_barrel();
    new spell_gen_stonehide_leather_barding();
    new spell_gen_check_flag_picker();
    new spell_gen_check_flag;
    new spell_gen_power_handler();
    new spell_gen_drop_fish();
    new spell_gen_in_ship();
    new spell_gen_spells_with_charges_checks();
    /// Honor Talents
    new spell_gen_adapation();
    new spell_gen_vim_and_vigor();
    new spell_gen_hardiness();
    new spell_gen_sparring();
    new spell_gen_intimidated();
    new spell_gen_magic_broom();
    new spell_gen_call_to_xera();

    /// Racial
    new spell_gen_every_man_for_himself();

    /// PlayerScript
    new PlayerScript_gen_touch_of_elune();
    new PlayerScript_gen_remove_rigor_mortis();
    new PlayerScript_gen_pvp_rules_enabled();
    new PlayerScript_gen_goblin_create();
    new PlayerScript_gen_liquid_fire_of_elune();
    new PlayerScript_gen_stonehide_leather_barding();
    new PlayerScript_gen_shadowmeld();

    if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
        new PlayerScript_gen_greymane_gurubashi_arena();

    /// Professions
    new spell_gen_learn_legion_skinning();
    new spell_herb_gathering();
    new spell_gen_herbalism_trap();
    new spell_gen_mining_trap();
    new spell_gen_archaeology_trap();
    new spell_gen_fishing();
    new spell_gen_fishing_throw_back();

    new spell_gen_golbin_glider();
    ///new spell_crash_test();

    new PlayerScript_gen_disable_cosmetics();
    new spell_gen_obliterum();
    new spell_gen_obliterate();
    new spell_gen_primal_obliterum();

    new PlayerScript_Summon_Bodyguard();
    new spell_gen_learning();
    new spell_gen_sa_rocket_blast();

    /// Pvp
    new spell_gen_dampening();

    /// Legendary cloak
    new spell_item_legendary_cloak_visual();     ///< 146195 146199 146193 146197

    new spell_gen_luchador_attacks();

    new spell_gen_warsong_gulch_flags();
    new spell_gen_shadow_sight();
    new spell_gen_shadow_sight_check();
    new spell_gen_shaman_events_debug();

    /// Artifacts
    new spell_gen_concordance_of_the_legionfall();

    /// PvP Modifiers
    new spell_gen_concordance_of_the_legionfall_stat();

    /// Allied Races
    new spell_gen_arcane_pulse();
    new spell_gen_lights_judgment();
    new spell_gen_rugged_tenacity();
    new spell_gen_spatial_rift();
    new spell_gen_pride_of_ironhorn();

    /// Warboards
    new spell_gen_warboard_alliance();
    new spell_gen_warboard_horde();
    new spell_gen_warboard_neutral();

    new spell_gen_token_loot();
    new spell_gen_synthesize_legendary();
    new spell_gen_identify_legendary();
    new spell_gen_argus_filter();
    new spell_gen_pepper_breath();

    /// Netherlight Crucible
    new spell_gen_shadowbind();
    new spell_gen_chaotic_darkness();
    new spell_gen_torment_the_weak();
    new spell_gen_dark_sorrows();
    new spell_gen_sorrow();
    new spell_gen_master_of_shadows();
    new spell_gen_light_speed();
    new spell_gen_refractive_shell();
    new spell_gen_secure_in_the_light();
    new spell_gen_infusion_of_light();
    new spell_gen_lights_embrace();

    new spell_gen_titan_forging();
}
#endif
