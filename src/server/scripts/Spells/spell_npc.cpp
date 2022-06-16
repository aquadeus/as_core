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
#include "ScriptedCreature.h"
#include "Containers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Vehicle.h"
#include "VMapFactory.h"
#include "TotemAI.h"
#include "spell_generic.hpp"
#include "HelperDefines.h"
#include "PassiveAI.h"
#include "Group.h"
#include "Totem.h"

class GuardianPetTarget
{
    public:
        void UpdateTarget(Unit* l_UnitPet, Player* l_Owner, Unit*& l_Target, bool onlyInCombatWithOwner)
        {
            if (!l_UnitPet || !l_Owner || !l_UnitPet->ToCreature())
                return;

            Unit* l_OwnerTarget = l_Owner->GetSelectedUnit();
            Unit* l_NewTarget = nullptr;

            l_NewTarget = (l_OwnerTarget && onlyInCombatWithOwner && !l_OwnerTarget->GetThreatTarget(l_Owner->GetGUID()))
                ? nullptr : l_OwnerTarget;

            if (!l_NewTarget || (l_NewTarget && (l_NewTarget->isDead() || !l_UnitPet->IsValidAttackTarget(l_NewTarget))))
                l_NewTarget = l_Owner->getAttackerForHelper();

            if (onlyInCombatWithOwner && l_NewTarget && !l_NewTarget->GetThreatTarget(l_Owner->GetGUID()))
                l_NewTarget = nullptr;

            l_Target = l_NewTarget ? l_NewTarget : l_Target;
        }
};

/// Prismatic Crystal - 76933
class spell_npc_mage_prismatic_crystal : public CreatureScript
{
    public:
        spell_npc_mage_prismatic_crystal() : CreatureScript("npc_mage_prismatic_crystal") { }

        enum eSpells
        {
            PrismaticCrystalAura    = 155153,
            PrismaticCrystalDamage  = 155152
        };

        struct spell_npc_mage_prismatic_crystalAI : public ScriptedAI
        {
            spell_npc_mage_prismatic_crystalAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eDatas
            {
                FactionHostile = 14,
                FactionFriend  = 35
            };

            uint64 m_Owner = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_Owner = p_Summoner->GetGUID();

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                if (Player* l_Player = p_Summoner->ToPlayer())
                {
                    if (Aura* l_Aura = me->AddAura(eSpells::PrismaticCrystalAura, me))
                    {
                        if (AuraEffect* l_DamageTaken = l_Aura->GetEffect(SpellEffIndex::EFFECT_0))
                        {
                            if (l_Player->GetActiveSpecializationID() == SpecIndex::SPEC_MAGE_FROST)
                                l_DamageTaken->ChangeAmount(10);    ///< BasePoint = 30, but only for Arcane and Fire spec
                        }
                    }
                }

                me->setFaction(eDatas::FactionHostile);
                me->ForceValuesUpdateAtIndex(EUnitFields::UNIT_FIELD_FACTION_TEMPLATE);
            }

            void EnterEvadeMode() override
            {
                ///< No evade mode for Prismatic Crystal
            }
            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (p_Attacker->GetGUID() != m_Owner)
                {
                    p_Damage = 0;
                    return;
                }

                if (p_SpellInfo && p_SpellInfo->Id != eSpells::PrismaticCrystalDamage)
                {
                    if (Unit* l_Owner = sObjectAccessor->FindUnit(m_Owner))
                    {
                        int32 l_BasePoint = p_Damage;
                        l_Owner->CastCustomSpell(me, eSpells::PrismaticCrystalDamage, nullptr, &l_BasePoint, nullptr, true);
                    }
                }
            }


            void OnSendFactionTemplate(uint32& p_FactionID, Player* p_Target) override
            {
                if (m_Owner == p_Target->GetGUID())
                    p_FactionID = eDatas::FactionHostile;
                else
                    p_FactionID = eDatas::FactionFriend;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_mage_prismatic_crystalAI(p_Creature);
        }
};

/// Arcane Familiar - 103636
class spell_npc_mage_arcane_familiar : public CreatureScript
{
    public:
        spell_npc_mage_arcane_familiar() : CreatureScript("npc_mage_arcane_familiar") { }

        struct spell_npc_mage_arcane_familiarAI : public ScriptedAI
        {
            spell_npc_mage_arcane_familiarAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                ArcaneAssaultVisual     = 205235,
                ArcaneAssaultDamage     = 225119,
                ArcaneFamiliarManaBuff  = 210126
            };

            void Reset() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (!l_Owner->HasAura(eSpells::ArcaneFamiliarManaBuff))
                    l_Owner->CastSpell(l_Owner, eSpells::ArcaneFamiliarManaBuff, true);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (!l_Owner->isInCombat())
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                if (l_Player->HasSpellCooldown(eSpells::ArcaneAssaultDamage))
                    return;

                Unit* l_Target = l_Owner->getVictim();
                if (!l_Target || !l_Target->isInCombat())
                    return;

                me->CastSpell(l_Target, eSpells::ArcaneAssaultVisual, false);
                me->CastSpell(l_Target, eSpells::ArcaneAssaultDamage, false, nullptr, nullptr, l_Player->GetGUID());
                l_Player->AddSpellCooldown(eSpells::ArcaneAssaultDamage, 0, 2 * IN_MILLISECONDS, false);
            }

            void JustDespawned() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                l_Owner->RemoveAurasDueToSpell(eSpells::ArcaneFamiliarManaBuff);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                me->DespawnOrUnsummon(1);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_mage_arcane_familiarAI(p_Creature);
        }
};

/// Shadow Reflection - 77726
class spell_npc_rogue_shadow_reflection : public CreatureScript
{
    public:
        spell_npc_rogue_shadow_reflection() : CreatureScript("npc_rogue_shadow_reflection") { }

        struct spell_npc_rogue_shadow_reflectionAI : public ScriptedAI
        {
            spell_npc_rogue_shadow_reflectionAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_Queuing(true) { }

            enum eSpells
            {
                SpellShadowReflectionAura = 156744,
                ShadowReflectionClone     = 168691
            };

            enum eDatas
            {
                AddSpellToQueue = 0,
                FinishFirstPhase
            };

            struct SpellData
            {
                SpellData(uint32 p_ID, uint32 p_Time, uint32 p_ComboPoints)
                {
                    ID = p_ID;
                    Time = p_Time;
                    ComboPoints = p_ComboPoints;
                }

                uint32 ID;
                uint32 Time;
                uint32 ComboPoints;
            };

            uint64 m_TargetGUID = 0;
            bool m_Queuing;

            /// Automatically stored in good time order
            std::queue<SpellData> m_SpellQueue;

            void Reset()
            {
                me->SetCanDualWield(true);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->CastSpell(me, eSpells::SpellShadowReflectionAura, true);

                if (Unit* l_Owner = me->ToTempSummon()->GetOwner())
                {
                    if (Player* l_Player = l_Owner->ToPlayer())
                    {
                        if (l_Player->GetSelectedUnit())
                            m_TargetGUID = l_Player->GetSelectedUnit()->GetGUID();
                        else if (l_Owner->getVictim())
                            m_TargetGUID = l_Owner->getVictim()->GetGUID();
                    }
                }

                Unit* l_Target = ObjectAccessor::FindUnit(m_TargetGUID);
                if (l_Target != nullptr)
                    me->GetMotionMaster()->MoveFollow(l_Target, 2.0f, 0.0f);
            }

            void AddHitQueue(uint32 *p_Data, int32 p_ID)
            {
                switch (p_ID)
                {
                    case eDatas::AddSpellToQueue:
                    {
                        if (!m_Queuing)
                            break;

                        uint32 l_SpellID = p_Data[0];
                        uint32 l_Time = p_Data[1];
                        uint32 l_ComboPoints = p_Data[2];
                        m_SpellQueue.push(SpellData(l_SpellID, l_Time, l_ComboPoints));
                        break;
                    }
                    case eDatas::FinishFirstPhase:
                    {
                        if (!m_Queuing || me->ToTempSummon() == nullptr)
                            break;

                        m_Queuing = false;
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        if (m_TargetGUID)
                        {
                            Unit* l_Target = ObjectAccessor::FindUnit(m_TargetGUID);
                            if (l_Target != nullptr)
                                AttackStart(l_Target);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo)
            {
                if (p_SpellInfo->Id == eSpells::ShadowReflectionClone && p_Caster != nullptr && p_Caster->GetTypeId() == TypeID::TYPEID_PLAYER)
                {
                    me->SetMaxPower(Powers::POWER_COMBO_POINT, 5);
                    me->SetPower(Powers::POWER_COMBO_POINT, p_Caster->GetPower(Powers::POWER_COMBO_POINT));

                    for (uint8 l_AttType = 0; l_AttType < WeaponAttackType::MaxAttack; ++l_AttType)
                    {
                        me->SetBaseWeaponDamage((WeaponAttackType)l_AttType, MAXDAMAGE, p_Caster->GetBaseWeaponDamageRange((WeaponAttackType)l_AttType, MAXDAMAGE));
                        me->SetBaseWeaponDamage((WeaponAttackType)l_AttType, MINDAMAGE, p_Caster->GetBaseWeaponDamageRange((WeaponAttackType)l_AttType, MINDAMAGE));
                    }

                    me->UpdateAttackPowerAndDamage();
                }
            }

            void UpdateAI(uint32 const p_Diff)
            {
                if (m_Queuing)
                    return;

                if (!UpdateVictim())
                    return;

                if (SpellData* l_SpellData = GetNextSpellData())
                {
                    if (l_SpellData->Time <= p_Diff)
                    {
                        me->SetPower(Powers::POWER_COMBO_POINT, l_SpellData->ComboPoints);
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, l_SpellData->ID, true);

                        m_SpellQueue.pop();
                    }
                    else
                        l_SpellData->Time -= p_Diff;
                }

                DoMeleeAttackIfReady();
            }

            SpellData* GetNextSpellData()
            {
                if (m_SpellQueue.empty())
                    return nullptr;

                return &m_SpellQueue.front();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_rogue_shadow_reflectionAI(p_Creature);
        }
};

/// Lightning Surge Totem - 61245
class spell_npc_sha_lightning_surge_totem : public CreatureScript
{
    public:
        spell_npc_sha_lightning_surge_totem() : CreatureScript("npc_lightning_surge_totem") { }

        enum eSpells
        {
            StaticCharge = 118905
        };

        struct spell_npc_sha_lightning_surge_totemAI : public TotemAI
        {
            bool m_HasBeenCasted;

            spell_npc_sha_lightning_surge_totemAI(Creature* p_Creature) : TotemAI(p_Creature)
            {
                m_HasBeenCasted = false;
            }

            void Reset()
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            }

            void UpdateAI(uint32 const /*p_Diff*/)
            {
                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!m_HasBeenCasted)
                {
                    me->CastSpell(me, eSpells::StaticCharge, false);
                    m_HasBeenCasted = true;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_sha_lightning_surge_totemAI(p_Creature);
        }
};

/// Spirit Link Totem - 53006
class spell_npc_sha_spirit_link_totem : public CreatureScript
{
    public:
        spell_npc_sha_spirit_link_totem() : CreatureScript("npc_spirit_link_totem") { }

        struct spell_npc_sha_spirit_link_totemAI : public TotemAI
        {
            uint32 CastTimer;

            spell_npc_sha_spirit_link_totemAI(Creature* creature) : TotemAI(creature)
            {
                CastTimer = 1000;
            }

            void Reset()
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                if (me->GetOwner() && me->GetOwner()->IsPlayer())
                {
                    me->CastSpell(me, 98007, false);
                    me->CastSpell(me, 98017, true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (CastTimer >= diff)
                {
                    if (me->GetOwner() && me->GetOwner()->IsPlayer())
                    {
                        if (me->GetEntry() == 53006)
                        {
                            me->CastSpell(me, 98007, false);
                            me->CastSpell(me, 98017, true);
                        }
                    }
                }

                CastTimer = 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_sha_spirit_link_totemAI(creature);
        }
};

/// Earthen Shield Totem - 100943 (spell: 198838)
class spell_npc_sha_earthen_shield_totem : public CreatureScript
{
    public:
        spell_npc_sha_earthen_shield_totem() : CreatureScript("npc_earthen_shield_totem") { }

        struct spell_npc_sha_earthen_shield_totem_AI : public TotemAI
        {
            spell_npc_sha_earthen_shield_totem_AI(Creature* creature) : TotemAI(creature) { }

            void IsSummonedBy(Unit* p_Owner) override
            {
                me->SetMaxHealth(p_Owner->GetMaxHealth());
                me->SetHealth(me->GetMaxHealth());
            }

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
            }

            void UpdateAI(uint32 const diff) override
            {
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_sha_earthen_shield_totem_AI(creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called for Cloudburst Totem - 78001
/// Called by Cloudburst Totem - 157153
class spell_npc_sha_cloudburst_totem : public CreatureScript
{
    public:
        spell_npc_sha_cloudburst_totem() : CreatureScript("spell_npc_sha_cloudburst_totem") { }

        enum eSpells
        {
            CloudburstTotem = 157153,
            CloudburstAura  = 157504
        };

        struct spell_npc_sha_cloudburst_totem_AI : public TotemAI
        {
            spell_npc_sha_cloudburst_totem_AI(Creature* p_Creature) : TotemAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Owner) override
            {
                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CloudburstTotem))
                {
                    if (Totem* l_Totem = me->ToTotem())
                    {
                        m_Duration = l_Totem->GetTotemDuration();
                        l_Totem->SetTotemDuration(m_Duration + 5 * TimeConstants::IN_MILLISECONDS);
                    }
                }
                TotemAI::IsSummonedBy(p_Owner);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_IsDespawned)
                    return;

                if (m_Duration <= p_Diff)
                {
                    m_IsDespawned = true;
                    me->RemoveAurasDueToSpell(eSpells::CloudburstAura);
                    me->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
                    return;
                }
                else
                {
                    m_Duration -= p_Diff;
                }

                TotemAI::UpdateAI(p_Diff);
            }

        private:

            bool m_IsDespawned = false;
            uint32 m_Duration = 1 * IN_MILLISECONDS;
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_sha_cloudburst_totem_AI(p_Creature);
        }
};

/// Greater Storm Elemental - 77936
/// Primal Storm Elemental - 77942
class spell_npc_sha_storm_elemental : public CreatureScript
{
    public:
        spell_npc_sha_storm_elemental() : CreatureScript("npc_storm_elemental") { }

        enum eSpells
        {
            SpellWindGust       = 157331,
            SpellCallLightning  = 157348
        };

        struct spell_npc_sha_storm_elementalAI : public ScriptedAI
        {
            spell_npc_sha_storm_elementalAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                EventWindGust = 1,
                EventCallLightning
            };

            EventMap m_Events;

            void Reset()
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* /*p_Attacker*/)
            {
                m_Events.ScheduleEvent(eEvents::EventWindGust, 500);
                m_Events.ScheduleEvent(eEvents::EventCallLightning, 8000);
            }

            void UpdateAI(uint32 const p_Diff)
            {
                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Owner = me->GetOwner();

                if (l_Owner == nullptr)
                    return;

                Player* l_Player = l_Owner->ToPlayer();

                if (!l_Player->isInCombat())
                {
                    me->CombatStop();
                    return;
                }

                if (!UpdateVictim() || (l_Player->GetSelectedUnit() && me->getVictim() && l_Player->GetSelectedUnit() != me->getVictim()))
                {
                    Unit* l_OwnerTarget = NULL;
                    if (Player* l_Plr = l_Owner->ToPlayer())
                        l_OwnerTarget = l_Plr->GetSelectedUnit();
                    else
                        l_OwnerTarget = l_Owner->getVictim();

                    if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && !l_Owner->IsFriendlyTo(l_OwnerTarget) && me->IsValidAttackTarget(l_OwnerTarget))
                        AttackStart(l_OwnerTarget);
                    return;
                }

                if (me->getVictim() && !me->IsValidAttackTarget(me->getVictim()))
                    return;

                if (Unit* l_Target = me->getVictim())
                {
                    if (!l_Target->HasAura(eSpells::SpellCallLightning, me->GetGUID()))
                        me->CastSpell(l_Target, eSpells::SpellCallLightning, false);
                }

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventWindGust:
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, eSpells::SpellWindGust, false);
                        m_Events.ScheduleEvent(eEvents::EventWindGust, 500);
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_sha_storm_elementalAI(p_Creature);
        }
};

/// Greater Fire Elemental - 15438 - 95061
/// Primal Fire Elemental - 61029
class spell_npc_sha_fire_elemental : public CreatureScript
{
    public:
        spell_npc_sha_fire_elemental() : CreatureScript("npc_fire_elemental") { }

        struct spell_npc_sha_fire_elementalAI : public ScriptedAI
        {
            spell_npc_sha_fire_elementalAI(Creature* p_Creature) : ScriptedAI(p_Creature) {}

            enum fireEvents
            {
                EVENT_FIRE_NOVA     = 1,
                EVENT_FIRE_BLAST    = 2,
                EVENT_FIRE_SHIELD   = 3
            };

            enum fireSpells
            {
                SPELL_SHAMAN_FIRE_BLAST     = 57984,
                SPELL_SHAMAN_FIRE_NOVA      = 117588,
                SPELL_SHAMAN_FIRE_SHIELD    = 13376
            };

            enum eNpc
            {
                FireElemen      = 95061
            };

            EventMap events;

            void Reset()
            {
                events.Reset();
                if (me->GetEntry() == 95061)
                {
                    events.ScheduleEvent(EVENT_FIRE_BLAST, 0);
                    return;
                }

                events.ScheduleEvent(EVENT_FIRE_NOVA, urand(5000, 20000));
                events.ScheduleEvent(EVENT_FIRE_BLAST, urand(5000, 20000));
                events.ScheduleEvent(EVENT_FIRE_SHIELD, 0);
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
            }

            void UpdateAI(const uint32 p_Diff)
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    Unit* l_OwnerTarget = NULL;
                    if (Player* l_Player = l_Owner->ToPlayer())
                    {
                        if (!l_Player->isInCombat())
                        {
                            me->CombatStop();
                            return;
                        }
                        l_OwnerTarget = l_Player->GetSelectedUnit();
                    }
                    else
                        l_OwnerTarget = l_Owner->getVictim();

                    if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && !l_Owner->IsFriendlyTo(l_OwnerTarget) && me->IsValidAttackTarget(l_OwnerTarget))
                        AttackStart(l_OwnerTarget);
                }

                if (!UpdateVictim())
                    return;

                if (me->getVictim() && !me->IsValidAttackTarget(me->getVictim()))
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(p_Diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_FIRE_NOVA:
                        DoCastVictim(SPELL_SHAMAN_FIRE_NOVA);
                        events.ScheduleEvent(EVENT_FIRE_NOVA, urand(5000, 20000));
                        break;
                    case EVENT_FIRE_BLAST:
                        DoCastVictim(SPELL_SHAMAN_FIRE_BLAST);
                        events.ScheduleEvent(EVENT_FIRE_BLAST, 0);
                        break;
                    case EVENT_FIRE_SHIELD:
                        DoCastVictim(SPELL_SHAMAN_FIRE_SHIELD);
                        events.ScheduleEvent(EVENT_FIRE_SHIELD, 4000);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI *GetAI(Creature* p_Creature) const
        {
            return new spell_npc_sha_fire_elementalAI(p_Creature);
        }
};

/// Greater Earth Elemental - 15352 - 95072
/// Primal Earth Elemental - 61056
class spell_npc_sha_earth_elemental : public CreatureScript
{
    public:
        spell_npc_sha_earth_elemental() : CreatureScript("npc_earth_elemental") { }

        enum eSpells
        {
            AngeredEarth = 36213
        };

        struct spell_npc_sha_earth_elementalAI : public ScriptedAI
        {
            uint32 AngeredEarth_Timer;

            spell_npc_sha_earth_elementalAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                AngeredEarth_Timer = 0;
            }

            void Reset()
            {
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
            }

            void UpdateAI(const uint32 diff)
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    Unit* l_OwnerTarget = NULL;
                    if (Player* l_Plr = l_Owner->ToPlayer())
                        l_OwnerTarget = l_Plr->GetSelectedUnit();
                    else
                        l_OwnerTarget = l_Owner->getVictim();

                    if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && !l_Owner->IsFriendlyTo(l_OwnerTarget) && me->IsValidAttackTarget(l_OwnerTarget))
                        AttackStart(l_OwnerTarget);
                }

                if (!UpdateVictim())
                    return;

                if (me->getVictim() && !me->IsValidAttackTarget(me->getVictim()))
                    return;

                if (AngeredEarth_Timer <= diff)
                {
                    DoCast(me->getVictim(), eSpells::AngeredEarth);
                    AngeredEarth_Timer = 5000 + rand() % 15000; // 5-20 sec cd
                }
                else
                    AngeredEarth_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_sha_earth_elementalAI(p_Creature);
        }
};

/// Spirit Wolf - 29264, 100820
class spell_npc_sha_feral_spirit : public CreatureScript
{
    public:
        spell_npc_sha_feral_spirit() : CreatureScript("npc_feral_spirit") { }

        enum eSpells
        {
            SpiritLeap                  = 58867,
            SpiritWalk                  = 58875,
            SpiritHunt                  = 58877,
            GlyphOfSpiritRaptors        = 147783,
            RaptorTranform              = 147908,
            FeralSpiritWindfuryDriver   = 170523,
            T17Enhancement4P            = 165610,
            WindfuryAttack              = 170512,
            FeralSpiritMasteryEffect    = 131073,

            FieryJaws                   = 224125,
            FrozenBite                  = 224126,
            CracklingSurge              = 224127,
        };

        enum eNPC
        {
            DoomWolves = 100820
        };

        enum eDisplayId
        {
            DoomWolvesFire = 66843,
            DoomWolvesFrost = 66844,
            DoomWolvesNature = 66845
        };

        enum eAction
        {
            ActionWindfury
        };

        struct spell_npc_sha_feral_spiritAI : public ScriptedAI
        {
            spell_npc_sha_feral_spiritAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_WindFuryCooldown(0) { }

            uint32 m_WindFuryCooldown;
            uint32 m_DoomWolvesAttackCD;

            void Reset() override
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    if (l_Owner->HasAura(eSpells::GlyphOfSpiritRaptors))
                        me->CastSpell(me, eSpells::RaptorTranform, true);

                    /// While Feral Spirits is active, [...] your wolves can proc Windfury.
                    if (l_Owner->HasAura(eSpells::T17Enhancement4P))
                        me->CastSpell(me, eSpells::FeralSpiritWindfuryDriver, true);

                    if (l_Owner->ToPlayer() && l_Owner->ToPlayer()->GetActiveSpecializationID() == SPEC_SHAMAN_ENHANCEMENT  && l_Owner->getLevel() >= 56)
                        me->CastSpell(me, eSpells::FeralSpiritMasteryEffect, true);

                    /// Set wolf's AP to 50% of caster AP
                    float l_AP = l_Owner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
                    for (uint8 l_AttType = 0; l_AttType < WeaponAttackType::MaxAttack; ++l_AttType)
                    {
                        me->SetBaseWeaponDamage((WeaponAttackType)l_AttType, MAXDAMAGE, l_AP * 0.5f);
                        me->SetBaseWeaponDamage((WeaponAttackType)l_AttType, MINDAMAGE, l_AP * 0.5f);
                    }
                    me->UpdateAttackPowerAndDamage();
                }

                me->SetSpeed(MOVE_RUN, 1.5f, true);

                m_DoomWolvesAttackCD = 0;
                m_WindFuryCooldown = 0;
                me->CastSpell(me, eSpells::SpiritHunt, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
            }

            void DoAction(int32 const p_Action) override
            {
                if (p_Action == eAction::ActionWindfury && !m_WindFuryCooldown)
                {
                    if (Unit* l_Target = me->getVictim())
                    {
                        m_WindFuryCooldown = 5 * TimeConstants::IN_MILLISECONDS;

                        uint8 l_Count = 3;

                        for (uint8 l_I = 0; l_I < l_Count; l_I++)
                            me->CastSpell(l_Target, eSpells::WindfuryAttack, true);
                    }
                }
            }

            void UpdateCooldown(uint32& p_Cooldown, uint32 const p_Diff)
            {
                if (p_Cooldown > 0 && p_Cooldown > p_Diff)
                    p_Cooldown -= p_Diff;
                else if (p_Cooldown > 0 && p_Diff > p_Cooldown)
                    p_Cooldown = 0;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateCooldown(m_WindFuryCooldown, p_Diff);
                UpdateCooldown(m_DoomWolvesAttackCD, p_Diff);

                Unit* l_Owner = me->GetOwner();
                if (l_Owner == nullptr)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (l_Player == nullptr)
                    return;

                if (!l_Player->isInCombat())
                {
                    me->CombatStop();
                    return;
                }

                if (!UpdateVictim() || (l_Player->GetSelectedUnit() && me->getVictim() && l_Player->GetSelectedUnit() != me->getVictim()))
                {
                    Unit* l_OwnerTarget = nullptr;
                    l_OwnerTarget = l_Player->getVictim();

                    if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && !l_Owner->IsFriendlyTo(l_OwnerTarget) && me->IsValidAttackTarget(l_OwnerTarget))
                        AttackStart(l_OwnerTarget);
                }

                Unit* l_Target = me->getVictim();

                if (l_Target)
                {
                    if (!me->IsWithinMeleeRange(l_Target) && l_Target->IsInRange(me, 5.0f, 30.0f))
                    {
                        if (!me->HasSpellCooldown(eSpells::SpiritLeap))
                            me->CastSpell(l_Target, eSpells::SpiritLeap, true);
                    }
                }

                DoMeleeAttackIfReady();
                if (me->GetEntry() == eNPC::DoomWolves)
                {
                    if (!m_DoomWolvesAttackCD)
                        DoomWolvesAttack();
                }
            }

            void DoomWolvesAttack()
            {
                m_DoomWolvesAttackCD = 5 * TimeConstants::IN_MILLISECONDS;

                Unit* l_Target = me->getVictim();

                if (!l_Target)
                    return;

                switch (me->GetDisplayId())
                {
                    case eDisplayId::DoomWolvesFire:
                        me->CastSpell(l_Target, eSpells::FieryJaws, true, (Item*)nullptr, (AuraEffect*)nullptr, me->GetOwnerGUID());
                        break;
                    case eDisplayId::DoomWolvesFrost:
                        me->CastSpell(l_Target, eSpells::FrozenBite, true, (Item*)nullptr, (AuraEffect*)nullptr, me->GetOwnerGUID());
                        break;
                    case eDisplayId::DoomWolvesNature:
                        me->CastSpell(l_Target, eSpells::CracklingSurge, true, (Item*)nullptr, (AuraEffect*)nullptr, me->GetOwnerGUID());
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_sha_feral_spiritAI(p_Creature);
        }
};

/// Ravager - 76168
class spell_npc_warr_ravager : public CreatureScript
{
    public:
        spell_npc_warr_ravager() : CreatureScript("npc_warrior_ravager") { }

        enum eDatas
        {
            DisplayID = 55644,
            RavagerAura = 153709
        };

        struct spell_npc_warr_ravagerAI : public ScriptedAI
        {
            spell_npc_warr_ravagerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void IsSummonedBy(Unit* p_Summoner)
            {
                me->SetDisplayId(eDatas::DisplayID);
                me->CastSpell(me, eDatas::RavagerAura, true);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NOT_SELECTABLE |
                                                           eUnitFlags::UNIT_FLAG_UNK_15 |
                                                           eUnitFlags::UNIT_FLAG_PVP_ATTACKABLE);

                if (p_Summoner == nullptr || p_Summoner->GetTypeId() != TypeID::TYPEID_PLAYER)
                    return;

                if (Player* l_Player = p_Summoner->ToPlayer())
                {
                    if (Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_MAINHAND))
                    {
                        /// Display Transmogrifications on player's clone
                        if (ItemTemplate const* l_Proto = sObjectMgr->GetItemTemplate(l_Item->GetDynamicValue(EItemDynamicFields::ITEM_DYNAMIC_FIELD_MODIFIERS, 0)))
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS, l_Proto->ItemId);
                        else
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS, l_Item->GetTemplate()->ItemId);
                    }

                    if (Item* l_Item = l_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, EquipmentSlots::EQUIPMENT_SLOT_OFFHAND))
                    {
                        /// Display Transmogrifications on player's clone
                        if (ItemTemplate const* l_Proto = sObjectMgr->GetItemTemplate(l_Item->GetDynamicValue(EItemDynamicFields::ITEM_DYNAMIC_FIELD_MODIFIERS, 0)))
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 2, l_Proto->ItemId);
                        else
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 2, l_Item->GetTemplate()->ItemId);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_warr_ravagerAI(p_Creature);
        }
};

/// Mocking Banner - 59390
class spell_npc_warr_mocking_banner : public CreatureScript
{
    public:
        spell_npc_warr_mocking_banner() : CreatureScript("spell_npc_warr_mocking_banner") { }

        struct spell_npc_warr_mocking_bannerAI : public ScriptedAI
        {
            spell_npc_warr_mocking_bannerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset()
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_warr_mocking_bannerAI(p_Creature);
        }
};

/// War Banner - 119052
class spell_npc_warr_war_banner : public CreatureScript
{
    public:
        spell_npc_warr_war_banner() : CreatureScript("spell_npc_warr_war_banner") { }

        struct spell_npc_warr_war_bannerAI : public ScriptedAI
        {
            spell_npc_warr_war_bannerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                WarBanner       = 236321,
                WarBannerTalent = 236320
            };

            std::list<uint64> m_AffectedUnits;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WarBannerTalent);
                if (!l_SpellInfo)
                    return;

                ///  1- Get The Units in range
                float l_Radius = l_SpellInfo->Effects[EFFECT_1].CalcRadius();
                std::list<Unit*> l_TargetList;
                JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(me, l_Owner, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(me, l_TargetList, l_Check);
                me->VisitNearbyObject(l_Radius, l_Searcher);

                /// Remove attackable units (duel opponent shares the same faction and therefore is considered as friendly, but shouldn't benefit from the buff)
                l_TargetList.remove_if([l_Owner](Unit* l_Target) -> bool
                {
                    if (l_Owner->IsValidAttackTarget(l_Target))
                        return true;

                    return false;
                });

                std::list<uint64> l_TargetGuids;
                for (Unit* l_Target : l_TargetList)
                    l_TargetGuids.push_back(l_Target->GetGUID());

                /// 2 - Get the already affected units (m_AffectedUnits)
                /// Nothing to do here

                ///3 Remove from the affected target list (and remove the aura on those removed units) the units that are no longer in range,

                for (auto l_Itr = m_AffectedUnits.begin(); l_Itr != m_AffectedUnits.end();)
                {
                    if (std::find(l_TargetGuids.begin(), l_TargetGuids.end(), (*l_Itr)) == l_TargetGuids.end())
                    {
                        if (Unit* l_Target = sObjectAccessor->GetUnit(*me, *l_Itr))
                            l_Target->RemoveAurasDueToSpell(eSpells::WarBanner, me->GetGUID());

                        l_Itr = m_AffectedUnits.erase(l_Itr);
                    }
                    else
                        ++l_Itr;
                }

                /// Add in the affected target list (and cast the spell on those added units) the units that just came in range.

                for (uint64 l_TargetGuid : l_TargetGuids)
                {
                    if (std::find(m_AffectedUnits.begin(), m_AffectedUnits.end(), l_TargetGuid) == m_AffectedUnits.end())
                    {
                        if (Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGuid))
                            me->AddAura(eSpells::WarBanner, l_Target);

                        m_AffectedUnits.push_back(l_TargetGuid);
                    }
                }

                /// Affected list is now updated and every aura has been added or removed according to what's needed
            }

            void JustDespawned() override
            {
                for (uint64 l_TargetGuid : m_AffectedUnits)
                {
                    if (Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGuid))
                        l_Target->RemoveAurasDueToSpell(eSpells::WarBanner, me->GetGUID());
                }

                m_AffectedUnits.clear();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_warr_war_bannerAI(p_Creature);
        }
};

/// Wild Imp - 55659
class spell_npc_warl_wild_imp : public CreatureScript
{
    public:
        spell_npc_warl_wild_imp() : CreatureScript("npc_wild_imp") { }

        enum eSpells
        {
            Firebolt        = 104318,
            MoltenCore      = 122351,
            Implosion       = 196278,
            ImplosionJump   = 205205
        };

        struct spell_npc_warl_wild_impAI : public ScriptedAI
        {
            bool m_Implose = false;

            spell_npc_warl_wild_impAI(Creature *creature) : ScriptedAI(creature) { }

            void Reset()
            {
                me->SetReactState(REACT_HELPER);
                if (me->GetOwner())
                {
                    if (Guardian* l_OwnerPet = me->GetOwner()->GetGuardianPet())
                    {
                        if (l_OwnerPet->GetEntry() != 55659)
                        {
                            l_OwnerPet->UpdateAllStats();
                        }
                    }
                }
            }

            void SetData(uint32 p_Type, uint32 p_Value)
            {
                if (p_Type == 0 && p_Value == 1)
                    m_Implose = true;
            }

            void UpdateAI(const uint32 /*p_Diff*/)
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    if (m_Implose)
                    {
                        if (Unit* l_Victim = me->getVictim())
                        {
                            if (me->GetDistance(l_Victim) <= 4.0f)
                                l_Owner->CastSpell(me, eSpells::Implosion, true);
                            else if (l_Victim->isDead())
                                return;
                            else
                                me->CastSpell(l_Victim, eSpells::ImplosionJump, false);
                        }
                    }

                    if (!l_Owner->ToPlayer())
                        return;

                    uint64 l_Guid = 0;

                    std::map<uint64, uint64>& l_ImpendinDoom = l_Owner->m_SpellHelper.GetUint64Map()[eSpellHelpers::ImpendinDoom];
                    if (l_ImpendinDoom.find(me->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL)) != l_ImpendinDoom.end())
                        l_Guid = l_ImpendinDoom[me->GetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL)];
                    else
                        l_Guid = l_Owner->ToPlayer()->m_SpellHelper.GetUint64(eSpellHelpers::WildImpsTarget);

                    if (l_Guid == 0)
                        return;

                    Unit* l_ImpsTarget = sObjectAccessor->GetUnit(*me, l_Guid);

                    if (!l_ImpsTarget || !l_Owner->isInCombat())
                    {
                        me->GetMotionMaster()->MoveFollow(l_Owner, 1.0f, 0.0f);
                        return;
                    }

                    if (me->isTargetableForAttack(l_ImpsTarget) && (!me->getVictim() || me->getVictim() != l_ImpsTarget))
                        AttackStart(l_ImpsTarget, false);
                }

                if (!me->getVictim())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!me->IsValidAttackTarget(me->getVictim()))
                    return;

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Firebolt);
                if (!l_SpellInfo)
                    return;

                DoCastSpell(me->getVictim(), l_SpellInfo, false, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_warl_wild_impAI(creature);
        }
};

/// Imp - 416 - Fel Imp - 58959
class spell_npc_warl_imp : public CreatureScript
{
    public:
        spell_npc_warl_imp() : CreatureScript("npc_imp") { }

        enum eSpells
        {
            Firebolt = 3110,
            Felbolt  = 115746
        };

        struct spell_npc_warl_impAI : public ScriptedAI
        {
            spell_npc_warl_impAI(Creature *creature) : ScriptedAI(creature)
            {
                m_TargetGUID = 0;
            }

            uint64 m_TargetGUID;
            bool m_IsFromGrimoireOfService = false;

            void SetGUID(uint64 p_Guid, int32 /*p_Index*/) override
            {
                m_IsFromGrimoireOfService = true;
                m_TargetGUID = p_Guid;
            }

            void Reset() override
            {
                if (!m_IsFromGrimoireOfService)
                {
                    ScriptedAI::Reset();
                    return;
                }
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                if (!m_IsFromGrimoireOfService)
                {
                    ScriptedAI::UpdateAI(p_Diff);
                    return;
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_OwnerTarget = sObjectAccessor->GetUnit(*me, m_TargetGUID);
                if (!l_OwnerTarget)
                    return;

                AttackStart(l_OwnerTarget);

                if (!me->IsValidAttackTarget(l_OwnerTarget) || l_OwnerTarget->HasStealthAura())
                    return;

                if (me->GetEntry() == WarlockPet::Imp)
                    me->CastSpell(l_OwnerTarget, eSpells::Firebolt, false);

                if (me->GetEntry() == WarlockPet::FellImp)
                    me->CastSpell(l_OwnerTarget, eSpells::Felbolt, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_warl_impAI(creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Shadowy Tear - 99887
/// For Dimensionnal Rift - 196586
class spell_npc_shadowy_tear : public CreatureScript
{
    public:
        spell_npc_shadowy_tear() : CreatureScript("npc_shadowy_tear") { }

        struct spell_npc_shadowy_tearAI : public ScriptedAI
        {
            spell_npc_shadowy_tearAI(Creature* creature) : ScriptedAI(creature)
            {
                m_TargetGUID = 0;
            }

            uint64 m_TargetGUID;

            enum eSpells
            {
                ShadowyTearVisualAura   = 219107,
                ShadowBarrage           = 196659
            };

            void Reset() override
            {
                if (!me->HasAura(eSpells::ShadowyTearVisualAura))
                    me->CastSpell(me, eSpells::ShadowyTearVisualAura, true);

                if (m_TargetGUID)
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = l_Player->GetSelectedUnit();
                if (!l_Target || l_Player->IsFriendlyTo(l_Target))
                    return;

                m_TargetGUID = l_Target->GetGUID();
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                Unit* l_Owner = me->GetOwner();
                if (me->HasAura(eSpells::ShadowBarrage) || !l_Owner)
                    return;

                me->CastSpell(me, eSpells::ShadowBarrage, false, nullptr, nullptr, l_Owner->GetGUID());
                Aura* l_ShadowBarrage = me->GetAura(eSpells::ShadowBarrage);
                if (!l_ShadowBarrage)
                    return;

                if (Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID))
                {
                    if (l_Target->HasStealthAura() || l_Target->HasInvisibilityAura())
                    {
                        m_TargetGUID = 0;
                        me->RemoveAura(eSpells::ShadowBarrage);
                    }
                }

                l_ShadowBarrage->SetScriptGuid(1, m_TargetGUID);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_shadowy_tearAI(creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Chaos Tear - 108493
/// For Dimensionnal Rift - 196586
class spell_npc_chaos_tear : public CreatureScript
{
    public:
        spell_npc_chaos_tear() : CreatureScript("npc_chaos_tear") { }

        struct spell_npc_chaos_tearAI : public ScriptedAI
        {
            spell_npc_chaos_tearAI(Creature* creature) : ScriptedAI(creature)
            {
                m_TargetGUID = 0;
                m_Casted = false;
            }

            uint64 m_TargetGUID;
            bool m_Casted;
            uint32 m_ChaosBoltTimer;

            enum eSpells
            {
                ChaosTearVisualAura = 219117,
                ChaosBolt           = 215279
            };

            void Reset() override
            {
                if (!me->HasAura(eSpells::ChaosTearVisualAura))
                    me->CastSpell(me, eSpells::ChaosTearVisualAura, true);

                if (m_TargetGUID)
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = l_Player->GetSelectedUnit();
                if (!l_Target || l_Player->IsFriendlyTo(l_Target))
                    return;

                m_TargetGUID = l_Target->GetGUID();
                m_ChaosBoltTimer = 2 * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!m_TargetGUID || m_Casted || !l_Owner)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);

                if (!l_Target)
                    return;

                if (l_Target->HasStealthAura() || l_Target->HasInvisibilityAura())
                {
                    m_TargetGUID = 0;
                    return;
                }

                if (m_ChaosBoltTimer > p_Diff)
                {
                    m_ChaosBoltTimer -= p_Diff;
                    return;
                }

                m_Casted = true;

                me->CastSpell(l_Target, eSpells::ChaosBolt, false, nullptr, nullptr, l_Owner->GetGUID());
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_chaos_tearAI(creature);
        }
};

/// Last Update 7.3.5 Build 25996
/// Unstable Tear - 94584
/// For Dimensionnal Rift - 196586
class spell_npc_unstable_tear : public CreatureScript
{
    public:
        spell_npc_unstable_tear() : CreatureScript("npc_unstable_tear") { }

        struct spell_npc_unstable_tearAI : public ScriptedAI
        {
            spell_npc_unstable_tearAI(Creature* creature) : ScriptedAI(creature)
            {
                m_TargetGUID = 0;
                m_BarrageTimer = 0;
            }

            uint64 m_TargetGUID;
            uint32 m_BarrageTimer;

            enum eSpells
            {
                UnstableTearVisualAura  = 219290,
                ChaosBarrage            = 187385
            };

            void Reset() override
            {
                m_BarrageTimer = 1500;

                if (!me->HasAura(eSpells::UnstableTearVisualAura))
                    me->CastSpell(me, eSpells::UnstableTearVisualAura, true);

                if (m_TargetGUID)
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = l_Player->GetSelectedUnit();
                if (!l_Target || l_Player->IsFriendlyTo(l_Target))
                    return;

                m_TargetGUID = l_Target->GetGUID();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner || me->HasAura(eSpells::ChaosBarrage))
                    return;

                if (m_BarrageTimer > p_Diff)
                {
                    m_BarrageTimer -= p_Diff;
                    return;
                }

                me->CastSpell(me, eSpells::ChaosBarrage, false, nullptr, nullptr, l_Owner->GetGUID());
                Aura* l_ShadowBarrage = me->GetAura(eSpells::ChaosBarrage);
                if (!l_ShadowBarrage)
                    return;

                if (Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID))
                {
                    if (l_Target->HasStealthAura() || l_Target->HasInvisibilityAura())
                    {
                        m_TargetGUID = 0;
                        me->RemoveAura(eSpells::ChaosBarrage);
                    }
                }


                l_ShadowBarrage->SetScriptGuid(1, m_TargetGUID);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_unstable_tearAI(creature);
        }
};


/// Last Update 6.2.3
/// Doomguard - 78158
class spell_npc_warl_doomguard: public CreatureScript
{
    public:
        spell_npc_warl_doomguard() : CreatureScript("npc_doomguard") { }

        enum eSpells
        {
            DoomBolt = 85692
        };

        struct spell_npc_warl_doomguardAI : public ScriptedAI
        {
            spell_npc_warl_doomguardAI(Creature *creature) : ScriptedAI(creature) { }

            void Reset()
            {
                me->SetPower(Powers::POWER_ENERGY, me->GetMaxPower(Powers::POWER_ENERGY));
                me->SetReactState(REACT_HELPER);
                if (!me->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER))
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
            }

            void UpdateAI(const uint32 /*p_Diff*/)
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    Unit* l_OwnerTarget = l_Owner->getVictim();

                    if (l_OwnerTarget == nullptr)
                        if (Player* l_Player = l_Owner->ToPlayer())
                            l_OwnerTarget = l_Player->GetSelectedUnit();

                    if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && (!me->getVictim() || me->getVictim() != l_OwnerTarget))
                        AttackStart(l_OwnerTarget);
                }

                if (!me->getVictim())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                me->CastSpell(me->getVictim(), eSpells::DoomBolt, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_warl_doomguardAI(creature);
        }
};

enum eGatewaySpells
{
    PortalVisual                = 113900,
    GatewayInteract             = 113902,
    CooldownMarker              = 113942,
    PillarsOfTheDarkPortal      = 217519,
    PillarsOfTheDarkPortalCD    = 217551,
    Planeswalker                = 196675,
    PlaneswalkerBuff            = 196674,
    GatewayMastery              = 248855,
    TravellingVisual            = 113896
};

enum eGatewayNpc
{
    GreenGate = 59262,
    PurpleGate = 59271
};

/// npc_demonic_gateway_purple - 59271
class spell_npc_warl_demonic_gateway_purple : public CreatureScript
{
    public:
        spell_npc_warl_demonic_gateway_purple() : CreatureScript("npc_demonic_gateway_purple") { }

        struct spell_npc_warl_demonic_gateway_purpleAI : public ScriptedAI
        {
            spell_npc_warl_demonic_gateway_purpleAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eAreaID
            {
                GurubashiRingBattle = 2177
            };

            void Reset()
            {
                me->CastSpell(me, eGatewaySpells::PortalVisual, true);

                me->SetFlag(UNIT_FIELD_INTERACT_SPELL_ID, eGatewaySpells::GatewayInteract);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                if (me->GetMap()->IsDungeon() || me->GetMap()->IsRaid())
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_ALLOW_ENEMY_INTERACT);

                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void OnSpellClick(Unit* p_Clicker)
            {
                if (p_Clicker->GetTypeId() != TYPEID_PLAYER)
                    return;

                // Demonic Gateway cooldown marker
                if (p_Clicker->HasAura(eGatewaySpells::CooldownMarker))
                    return;

                /// Greymane Gurubashi arena Usebug fix (clicking on gate while being in combat and in the battle ring)
                if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE) && p_Clicker->GetAreaId() == eAreaID::GurubashiRingBattle)
                {
                    Map* l_Map = p_Clicker->GetMap();
                    if (!l_Map)
                        return;

                    uint32 l_PortalAreaID = l_Map->GetAreaId(me->m_positionX, me->m_positionY, me->m_positionZ);
                    if (l_PortalAreaID != eAreaID::GurubashiRingBattle)
                        return;
                }

                /// Can't use gates in control
                if (p_Clicker->isFeared() || p_Clicker->isInStun() || p_Clicker->isConfused())
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner || !l_Owner->ToPlayer())
                    return;

                if (GroupPtr l_Group = p_Clicker->ToPlayer()->GetGroup())
                {
                    if (l_Owner->ToPlayer()->GetGroup() != l_Group)
                        return;
                }
                else if (l_Owner != p_Clicker)
                    return;

                if (p_Clicker->HasAura(eGatewaySpells::Planeswalker))
                    p_Clicker->CastSpell(p_Clicker, eGatewaySpells::PlaneswalkerBuff, true);

                std::list<Creature*> l_GreenGates;
                me->GetCreatureListWithEntryInGrid(l_GreenGates, eGatewayNpc::GreenGate, 75.0f);

                if (l_GreenGates.empty())
                    return;

                l_GreenGates.sort(JadeCore::DistanceCompareOrderPred(me));
                for (auto itr : l_GreenGates)
                {
                    if (p_Clicker->HasAura(eGatewaySpells::PillarsOfTheDarkPortal) && !p_Clicker->HasAura(eGatewaySpells::PillarsOfTheDarkPortalCD))
                    {
                        p_Clicker->CastSpell(p_Clicker, eGatewaySpells::PillarsOfTheDarkPortalCD, true);
                    }
                    else
                    {
                        p_Clicker->CastSpell(p_Clicker, eGatewaySpells::CooldownMarker, true);
                        int32 l_Reduce = 0;
                        if (SpellInfo const* l_MasterySpellInfo = sSpellMgr->GetSpellInfo(eGatewaySpells::GatewayMastery))
                            if (l_Owner->HasAura(eGatewaySpells::GatewayMastery))
                                l_Reduce = l_MasterySpellInfo->Effects[EFFECT_2].BasePoints;

                        if (Aura* l_Aura = p_Clicker->GetAura(eGatewaySpells::CooldownMarker))
                        {
                            if (itr->GetOwnerGUID() == p_Clicker->GetGUID())
                                l_Aura->SetDuration(MINUTE * IN_MILLISECONDS + (l_Reduce * IN_MILLISECONDS));
                            else
                                l_Aura->SetDuration(l_Aura->GetDuration() + (l_Reduce * IN_MILLISECONDS));
                        }
                    }

                    Unit* l_CurrentGateOwner = itr->GetOwner();

                    /// Can't teleport to other players Gates
                    if (l_CurrentGateOwner && l_Owner != l_CurrentGateOwner)
                        continue;

                    p_Clicker->CastSpell(p_Clicker, eGatewaySpells::TravellingVisual, true);

                    // Init dest coordinates
                    float x, y, z;
                    itr->GetPosition(x, y, z);

                    float speedXY;
                    float speedZ = 5;

                    speedXY = p_Clicker->GetExactDist2d(x, y) * 10.0f / speedZ;
                    p_Clicker->GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ);
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_warl_demonic_gateway_purpleAI(p_Creature);
        }
};

/// npc_demonic_gateway_green - 59262
class spell_npc_warl_demonic_gateway_green : public CreatureScript
{
    public:
        spell_npc_warl_demonic_gateway_green() : CreatureScript("npc_demonic_gateway_green") { }

        struct spell_npc_warl_demonic_gateway_greenAI : public ScriptedAI
        {
            spell_npc_warl_demonic_gateway_greenAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eAreaID
            {
                GurubashiRingBattle = 2177
            };

            void Reset()
            {
                me->CastSpell(me, eGatewaySpells::PortalVisual, true);

                me->SetFlag(UNIT_FIELD_INTERACT_SPELL_ID, eGatewaySpells::GatewayInteract);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                if (me->GetMap()->IsDungeon() || me->GetMap()->IsRaid())
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_ALLOW_ENEMY_INTERACT);

                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void OnSpellClick(Unit* p_Clicker)
            {
                if (p_Clicker->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (p_Clicker->HasAura(eGatewaySpells::CooldownMarker))
                    return;

                /// Greymane Gurubashi arena Usebug fix (clicking on gate while being in combat and in the battle ring)
                if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE) && p_Clicker->GetAreaId() == eAreaID::GurubashiRingBattle)
                {
                    Map* l_Map = p_Clicker->GetMap();
                    if (!l_Map)
                        return;

                    uint32 l_PortalAreaID = l_Map->GetAreaId(me->m_positionX, me->m_positionY, me->m_positionZ);
                    if (l_PortalAreaID != eAreaID::GurubashiRingBattle)
                        return;
                }

                /// Can't use gates in control
                if (p_Clicker->isFeared() || p_Clicker->isInStun() || p_Clicker->isConfused())
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner || !l_Owner->ToPlayer())
                    return;

                if (GroupPtr l_Group = p_Clicker->ToPlayer()->GetGroup())
                {
                    if (l_Owner->ToPlayer()->GetGroup() != l_Group)
                        return;
                }
                else if (l_Owner != p_Clicker)
                    return;

                if (p_Clicker->HasAura(eGatewaySpells::Planeswalker))
                    p_Clicker->CastSpell(p_Clicker, eGatewaySpells::PlaneswalkerBuff, true);

                std::list<Creature*> l_PurpleGates;
                me->GetCreatureListWithEntryInGrid(l_PurpleGates, eGatewayNpc::PurpleGate, 75.0f);

                if (l_PurpleGates.empty())
                    return;

                l_PurpleGates.sort(JadeCore::DistanceCompareOrderPred(me));
                for (auto itr : l_PurpleGates)
                {
                    Unit* l_CurrentGateOwner = itr->GetOwner();

                    /// Can't teleport to other players Gates
                    if (l_CurrentGateOwner && l_Owner != l_CurrentGateOwner)
                        continue;

                    if (p_Clicker->HasAura(eGatewaySpells::PillarsOfTheDarkPortal) && !p_Clicker->HasAura(eGatewaySpells::PillarsOfTheDarkPortalCD))
                    {
                        p_Clicker->CastSpell(p_Clicker, eGatewaySpells::PillarsOfTheDarkPortalCD, true);
                    }
                    else
                    {
                        p_Clicker->CastSpell(p_Clicker, eGatewaySpells::CooldownMarker, true);
                        int32 l_Reduce = 0;
                        if (SpellInfo const* l_MasterySpellInfo = sSpellMgr->GetSpellInfo(eGatewaySpells::GatewayMastery))
                            if (l_Owner->HasAura(eGatewaySpells::GatewayMastery))
                                l_Reduce = l_MasterySpellInfo->Effects[EFFECT_2].BasePoints;

                        if (Aura* l_Aura = p_Clicker->GetAura(eGatewaySpells::CooldownMarker))
                        {
                            if (itr->GetOwnerGUID() == p_Clicker->GetGUID())
                                l_Aura->SetDuration(MINUTE * IN_MILLISECONDS + (l_Reduce * IN_MILLISECONDS));
                            else
                                l_Aura->SetDuration(l_Aura->GetDuration() + (l_Reduce * IN_MILLISECONDS));
                        }
                    }

                    p_Clicker->CastSpell(p_Clicker, eGatewaySpells::TravellingVisual, true);

                    // Init dest coordinates
                    float x, y, z;
                    itr->GetPosition(x, y, z);

                    float speedXY;
                    float speedZ = 5;

                    speedXY = p_Clicker->GetExactDist2d(x, y) * 10.0f / speedZ;
                    p_Clicker->GetMotionMaster()->MoveJump(x, y, z, speedXY, speedZ);
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_warl_demonic_gateway_greenAI(p_Creature);
        }
};

/// Inner Demon - 82927
/// Spawned by Inner Demon - 166862
class spell_npc_warl_inner_demon : public CreatureScript
{
    public:
        spell_npc_warl_inner_demon() : CreatureScript("spell_npc_warl_inner_demon") { }

        struct spell_npc_warl_inner_demonAI : public ScriptedAI
        {
            spell_npc_warl_inner_demonAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                InnerDemonAura  = 181608,
                SoulFire        = 166864
            };

            enum eEvent
            {
                EventSoulFire = 1
            };

            EventMap m_Events;

            void Reset() override
            {
                if (Unit* l_Owner = me->GetOwner())
                    l_Owner->CastSpell(me, eSpells::InnerDemonAura, true);
            }

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                m_Events.Reset();

                m_Events.ScheduleEvent(eEvent::EventSoulFire, 100);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                {
                    if (Unit* l_Owner = me->GetOwner())
                    {
                        Unit* l_OwnerTarget = nullptr;
                        if (Player* l_Player = l_Owner->ToPlayer())
                            l_OwnerTarget = l_Player->GetSelectedUnit();
                        else
                            l_OwnerTarget = l_Owner->getVictim();

                        if (l_OwnerTarget && !l_OwnerTarget->HasStealthAura())
                            AttackStart(l_OwnerTarget);
                    }

                    return;
                }

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (m_Events.ExecuteEvent() == eEvent::EventSoulFire)
                {
                    if (Unit* l_Target = me->getVictim())
                        me->CastSpell(l_Target, eSpells::SoulFire, false);

                    m_Events.ScheduleEvent(eEvent::EventSoulFire, 50);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_warl_inner_demonAI(p_Creature);
        }
};

/// last update : 6.1.2 19802
/// Force of Nature (treant resto) - 54983
class spell_npc_dru_force_of_nature_resto : public CreatureScript
{
    public:
        spell_npc_dru_force_of_nature_resto() : CreatureScript("npc_dru_force_of_nature_resto") { }

        enum eSpells
        {
            Swiftmed        = 142421,
            HealingTouch    = 113828
        };

        struct spell_npc_dru_force_of_nature_resto_impAI : public ScriptedAI
        {
            spell_npc_dru_force_of_nature_resto_impAI(Creature *creature) : ScriptedAI(creature) { }

            bool m_FirstTarget = true;

            void Reset()
            {
                me->SetReactState(REACT_HELPER);

                Unit* l_Owner = me->ToTempSummon() ? me->ToTempSummon()->GetSummoner() : NULL;
                Unit* l_Target = l_Owner ? (l_Owner->getVictim() ? l_Owner->getVictim() : (l_Owner->ToPlayer() ? l_Owner->ToPlayer()->GetSelectedUnit() : NULL)) : NULL;

                if (!l_Owner || !l_Target)
                    return;

                if (me->IsValidAssistTarget(l_Target))
                    me->CastSpell(l_Target, eSpells::Swiftmed, false); /// Heal
            }

            void UpdateAI(const uint32 /*p_Diff*/)
            {
                Unit* l_Target = nullptr;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (m_FirstTarget)
                {
                    if (Unit* l_Owner = me->GetOwner())
                    {
                        l_Target = l_Owner->getVictim();

                        if (l_Target == nullptr)
                        {
                            if (Player* l_Player = l_Owner->ToPlayer())
                                l_Target = l_Player->GetSelectedUnit();
                        }
                        if (l_Target == nullptr)
                            return;

                        if (!me->IsValidAssistTarget(l_Target))
                            l_Target = l_Owner;
                    }
                    m_FirstTarget = false;
                }

                if (l_Target == nullptr)
                {
                    std::list<Unit*> l_FriendlyUnitList;
                    JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(me, me, 40.0f);
                    JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(me, l_FriendlyUnitList, l_Check);
                    me->VisitNearbyObject(40.0f, l_Searcher);

                    if (l_FriendlyUnitList.size() > 1)
                    {
                        l_FriendlyUnitList.sort(JadeCore::HealthPctOrderPred());
                        l_FriendlyUnitList.resize(1);
                    }

                    l_Target = l_FriendlyUnitList.empty() ? nullptr : l_FriendlyUnitList.front();
                }

                if (l_Target == nullptr)
                    return;

                if (!me->IsValidAssistTarget(l_Target))
                    return;

                me->CastSpell(l_Target, eSpells::HealingTouch, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_dru_force_of_nature_resto_impAI(creature);
        }
};

/// Last Update 6.2.3
/// Totem of Harmony - 65387
class spell_npc_totem_of_harmony : public CreatureScript
{
public:
    spell_npc_totem_of_harmony() : CreatureScript("spell_npc_totem_of_harmony") { }

    enum eSpells
    {
        TotemofHarmonyAura = 128182
    };

    struct spell_npc_totem_of_harmony_bannerAI : public ScriptedAI
    {
        spell_npc_totem_of_harmony_bannerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        void Reset()
        {
            me->CastSpell(me, eSpells::TotemofHarmonyAura, true);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const
    {
        return new spell_npc_totem_of_harmony_bannerAI(p_Creature);
    }
};

/// Black Ox Statue - 61146
class spell_npc_black_ox_statue : public CreatureScript
{
    public:
        spell_npc_black_ox_statue() : CreatureScript("spell_npc_black_ox_statue") { }

        enum eSpells
        {
            ThreatEvent = 163178
        };

        struct spell_npc_black_ox_statueAI : public ScriptedAI
        {
            spell_npc_black_ox_statueAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eEvents
            {
                ThreatEvent = 1
            };

            EventMap m_Events;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->setFaction(p_Summoner->getFaction());
            }

            void Reset() override
            {
                m_Events.Reset();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                me->CastSpell(me, eSpells::ThreatEvent, true);
                m_Events.ScheduleEvent(eEvents::ThreatEvent, 1000);
            }

            void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex) override
            {
                if (p_Spell->GetSpellInfo()->Id == eSpells::ThreatEvent && !p_Targets.empty())
                {
                    p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                    {
                        if (!p_Object || !p_Object->ToUnit())
                            return true;

                        /// Don't taunt ennemies that are too much lower or higher than statue on a Z axis
                        if (std::abs(p_Object->GetPositionZ() - me->GetPositionZ()) > 10.0f)
                            return true;

                        return false;
                    });
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::ThreatEvent:
                        me->CastSpell(me, eSpells::ThreatEvent, true);
                        m_Events.ScheduleEvent(eEvents::ThreatEvent, 1000);
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_black_ox_statueAI(p_Creature);
        }
};

/// Last Update 6.2.3
/// Explosive Sheep- 2675
class spell_npc_inge_exeplosive_sheep : public CreatureScript
{
    public:
        spell_npc_inge_exeplosive_sheep() : CreatureScript("npc_inge_exeplosive_sheep") { }

        enum eSpells
        {
            Explosion = 4050
        };

        struct spell_npc_inge_exeplosive_sheep_AI : public ScriptedAI
        {
            spell_npc_inge_exeplosive_sheep_AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset()
            {
                me->SetSpeed(MOVE_WALK, 0.4f);
                me->SetSpeed(MOVE_RUN, 0.4f);
            }

            void UpdateAI(uint32 const /*p_Diff*/)
            {
                const float l_MaxRadius = 15.0f; ///< Spell radius

                Unit* l_Target = me->getVictim();
                if (l_Target == nullptr)
                {
                    std::list<Unit*> l_Targets;

                    JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(me, me, l_MaxRadius);
                    JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(me, l_Targets, l_Check);
                    me->VisitNearbyObject(l_MaxRadius, l_Searcher);

                    for (Unit* l_Target : l_Targets)
                    {
                        if (l_Target->isAlive() && me->GetExactDistSq(l_Target) < l_MaxRadius * l_MaxRadius && me->IsWithinLOSInMap(l_Target) && me->IsValidAttackTarget(l_Target))
                        {
                            AttackStart(l_Target);
                            return;
                        }
                    }
                }
                else if (l_Target->GetDistance(me) < 3.0f)
                {
                    me->CastSpell(me, eSpells::Explosion, true);
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_inge_exeplosive_sheep_AI(p_Creature);
        }
};

/// Last Update 7.3.2 Build 25549
/// Dreadstalkers - 98035 - for Call Dreadstalkers - 104316
class spell_npc_warl_dreadstalkers : public CreatureScript
{
    public:
        spell_npc_warl_dreadstalkers() : CreatureScript("spell_npc_warl_dreadstalkers") { }

        struct spell_npc_warl_dreadstalkersAI : public ScriptedAI
        {
            spell_npc_warl_dreadstalkersAI(Creature *creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                Dreadbite               = 205196,
                SharpenedDreadfangsAura = 211123,
                SharpenedDreadfangs     = 215111,
                JawsOfShadow            = 238109,
                JawsOfShadowAura        = 242922,
                RageOfGuldan            = 257926,
                RageOfGuldanIncr        = 253014,
                T21Demonology4PBonus    = 251852
            };

            uint64 m_TargetGUID;
            bool m_DreadBiteCasted = false;
            bool m_SecondDreadBiteCasted = true;
            bool m_HasJump = false;
            bool m_DataUsed = false;

            void Reset() override
            {
                m_HasJump = false;
                me->SetReactState(REACT_HELPER);
                m_TargetGUID = 0;
                m_DreadBiteCasted = false;
                m_HasJump = false;
            }

            void SetData(uint32 p_Type, uint32 p_Value) override
            {
                if (m_DataUsed)
                    return;

                if (p_Type == 0 && p_Value == 1)
                    m_SecondDreadBiteCasted = false;

                m_DataUsed = true;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (AuraEffect* l_AuraEffect = summoner->GetAuraEffect(eSpells::RageOfGuldan, EFFECT_0))
                {
                    int32 l_Bp = l_AuraEffect->GetAmount();
                    me->CastCustomSpell(me, eSpells::RageOfGuldanIncr, &l_Bp, NULL, NULL, true);
                    summoner->DelayedRemoveAura(eSpells::RageOfGuldan, 1);
                }

                AuraEffect* l_AuraEffect = summoner->GetAuraEffect(eSpells::SharpenedDreadfangsAura, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                int32 l_Bp = l_AuraEffect->GetAmount();
                me->CastCustomSpell(me, eSpells::SharpenedDreadfangs, &l_Bp, nullptr, nullptr, true);
            }

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                Player* l_Player = me->GetSpellModOwner();
                if (!l_Player)
                    return;

                Unit* l_Target = nullptr;
                m_TargetGUID = l_Player->m_SpellHelper.GetUint64(eSpellHelpers::Dreadstalkers);
                if (m_TargetGUID)
                    l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);

                if (m_DreadBiteCasted && m_SecondDreadBiteCasted)
                    l_Target = l_Player->getVictim();

                if (!UpdateVictim() || (l_Player->getVictim() && me->getVictim() && l_Player->getVictim() != me->getVictim() && m_DreadBiteCasted))
                {
                    if (l_Target && me->isTargetableForAttack(l_Target) && !l_Player->IsFriendlyTo(l_Target) && me->IsValidAttackTarget(l_Target))
                    {
                        AttackStart(l_Target);
                    }
                    return;
                }

                if (!m_DreadBiteCasted && l_Target)
                {
                    if (me->IsWithinMeleeRange(l_Target))
                    {
                        m_DreadBiteCasted = true;
                        me->CastSpell(l_Target, eSpells::Dreadbite, false);
                        if (l_Player->HasAura(eSpells::JawsOfShadow))
                        {
                            if (!l_Target->HasAura(eSpells::JawsOfShadowAura))
                                me->CastSpell(l_Target, eSpells::JawsOfShadowAura, true);
                            else if (Aura* l_Aur = l_Target->GetAura(eSpells::JawsOfShadowAura))
                                l_Aur->SetDuration(l_Aur->GetMaxDuration());
                        }


                        return;
                    }
                    else if (!m_HasJump)
                    {
                        me->GetMotionMaster()->MoveJump(l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ(), 60, 0, 0, true, 0, l_Target->GetGUID());
                        m_HasJump = true;
                    }
                }

                if (!m_SecondDreadBiteCasted && l_Target)
                {
                    if (me->IsWithinMeleeRange(l_Target))
                    {
                        if (AuraEffect* l_AuraEffect = l_Player->GetAuraEffect(eSpells::T21Demonology4PBonus, EFFECT_0))
                        {
                            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Dreadbite))
                            {
                                m_SecondDreadBiteCasted = true;
                                me->CastSpell(l_Target, eSpells::Dreadbite, true); ///< Cast is triggered here, and we use that to script T21Demonology4PBonus directly in Dreadbite's spellscript
                                return;
                            }
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }

            void OnCalculateMeleeDamage(Unit* p_Victim, uint32* p_Damage, DamageInfo* p_DamageInfo) override
            {
                if (AuraEffect* l_RageOfGuldan = me->GetAuraEffect(eSpells::RageOfGuldanIncr, EFFECT_0))
                    AddPct(*p_Damage, l_RageOfGuldan->GetAmount());
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_warl_dreadstalkersAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Wild Imp - 99737 - for Improved Dreadstalkers - 196272
class spell_npc_warl_dreadstalkers_imps : public CreatureScript
{
    public:
        spell_npc_warl_dreadstalkers_imps() : CreatureScript("spell_npc_warl_dreadstalkers_imps") { }

        struct spell_npc_warl_dreadstalkers_impsAI : public ScriptedAI
        {
            spell_npc_warl_dreadstalkers_impsAI(Creature *creature) : ScriptedAI(creature) {}

            enum eEntry
            {
                Dreadstalkers = 98035
            };

            enum eSpells
            {
                Firebolt                = 104318
            };

            void Reset() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_DreadStalker = nullptr;
                for (Unit* l_Summon : l_Owner->m_Controlled)
                {
                    if (l_Summon->GetEntry() == eEntry::Dreadstalkers)
                    {
                        if (Vehicle* l_Vehicle = l_Summon->GetVehicleKit())
                        {
                            if (!l_Vehicle->GetPassenger(0))
                            {
                                l_DreadStalker = l_Summon;
                                break;
                            }
                        }
                    }
                }

                me->EnterVehicle(l_DreadStalker, 0, true);
            }

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    Unit* l_Dreadstalker = me->GetVehicleBase();
                    if (!l_Dreadstalker)
                        return;

                    Unit* l_DreadstalkerTarget = l_Dreadstalker->getVictim();

                    if (l_DreadstalkerTarget && me->isTargetableForAttack(l_DreadstalkerTarget) && (!me->getVictim() || me->getVictim() != l_DreadstalkerTarget))
                        AttackStart(l_DreadstalkerTarget);
                }

                if (!me->getVictim())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!me->IsValidAttackTarget(me->getVictim()))
                    return;

                me->CastSpell(me->getVictim(), eSpells::Firebolt, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_warl_dreadstalkers_impsAI(creature);
        }
};

/// Last Update 7.0.3 Build 22293
/// Niuzao - 73967
class spell_npc_monk_niuzao : public CreatureScript
{
    public:
        spell_npc_monk_niuzao() : CreatureScript("spell_npc_monk_niuzao") { }

        struct spell_npc_monk_niuzao_elementalAI : public ScriptedAI
        {
            spell_npc_monk_niuzao_elementalAI(Creature* p_Creature) : ScriptedAI(p_Creature) {}

            enum eEvents
            {
                EOxCharge   = 1,
                EProvoke    = 2,
                EStomp      = 3
            };

            enum eSpells
            {
                OxCharge    = 196728,
                Provoke     = 196727,
                Stomp       = 227291
            };

            EventMap events;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(eEvents::EStomp, urand(5000, 10000));
                events.ScheduleEvent(eEvents::EProvoke, urand(1000, 2000));
                events.ScheduleEvent(eEvents::EOxCharge, 0);
            }

            void UpdateAI(const uint32 diff)
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (l_Owner->GetMaxHealth() > me->GetMaxHealth())
                    me->SetMaxHealth(l_Owner->GetMaxHealth());

                if (!UpdateVictim())
                {
                    Unit* l_OwnerTarget = NULL;
                    if (Player* l_Plr = l_Owner->ToPlayer())
                        l_OwnerTarget = l_Plr->GetSelectedUnit();
                    else
                        l_OwnerTarget = l_Owner->getVictim();

                    if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && !l_Owner->IsFriendlyTo(l_OwnerTarget) && me->IsValidAttackTarget(l_OwnerTarget))
                            AttackStart(l_OwnerTarget);

                    return;
                }

                if (me->getVictim() && !me->IsValidAttackTarget(me->getVictim()))
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                case eEvents::EOxCharge:
                    DoCastVictim(eSpells::OxCharge);
                    break;
                case eEvents::EProvoke:
                    DoCastVictim(eSpells::Provoke);
                    events.ScheduleEvent(eEvents::EProvoke, urand(5000, 70000));
                    break;
                case eEvents::EStomp:
                    DoCastVictim(eSpells::Stomp);
                    events.ScheduleEvent(eEvents::EStomp, urand(5000, 10000));
                    break;
                default:
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI *GetAI(Creature* p_Creature) const
        {
            return new spell_npc_monk_niuzao_elementalAI(p_Creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Darkglare - 103673 for Summon Darkglare - 205180
class spell_npc_warl_darkglare : public CreatureScript
{
    public:
        spell_npc_warl_darkglare() : CreatureScript("spell_npc_warl_darkglare") { }

        struct spell_npc_warl_darkglareAI : public ScriptedAI
        {
            spell_npc_warl_darkglareAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                EyeLaser        = 205231,
                Doom            = 603,
                GrimoireOfSynergy = 171975
            };

            uint64 m_TargetGUID;

            void Reset() override
            {
                m_TargetGUID = 0;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (l_Owner->HasAura(eSpells::GrimoireOfSynergy))
                    me->AddAura(eSpells::GrimoireOfSynergy, me);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (!m_TargetGUID)
                {
                    SpellInfo const* l_EyeLaser = sSpellMgr->GetSpellInfo(eSpells::EyeLaser);
                    if (!l_EyeLaser)
                        return;

                    SpellRangeEntry const* l_Range = l_EyeLaser->RangeEntry;
                    if (!l_Range)
                        return;

                    Unit* l_Owner = me->GetOwner();
                    if (!l_Owner)
                        return;

                    std::list<Unit*> l_TargetList;
                    JadeCore::AnyUnitHavingBuffInObjectRangeCheck l_Ucheck(l_Owner, me, l_Range->maxRangeHostile, eSpells::Doom, false);
                    JadeCore::UnitListSearcher<JadeCore::AnyUnitHavingBuffInObjectRangeCheck> l_Searcher(me, l_TargetList, l_Ucheck);
                    me->VisitNearbyObject(l_Range->maxRangeHostile, l_Searcher);

                    if (l_TargetList.empty())
                        return;

                    m_TargetGUID = l_TargetList.front()->GetGUID();
                }

                if (Unit* l_Target = Unit::GetUnit(*me, m_TargetGUID))
                {
                    if (l_Target->HasAura(eSpells::Doom, me->GetOwnerGUID()))
                        me->CastSpell(l_Target, eSpells::EyeLaser, false);
                    else
                        m_TargetGUID = 0;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_warl_darkglareAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Fel Lord - 107024 for Call Fel Lord - 212459
class spell_npc_warl_fel_lord : public CreatureScript
{
    public:
        spell_npc_warl_fel_lord() : CreatureScript("spell_npc_warl_fel_lord") { }

        struct spell_npc_warl_fel_lordAI : public ScriptedAI
        {
            spell_npc_warl_fel_lordAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                FelCleave       = 213688
            };

            void UpdateAI(uint32 const p_Diff) override
            {
                SpellInfo const* l_FelCleave = sSpellMgr->GetSpellInfo(eSpells::FelCleave);
                if (!l_FelCleave)
                    return;

                SpellRadiusEntry const* l_RadiusEntry = l_FelCleave->Effects[EFFECT_0].RadiusEntry;
                if (!l_RadiusEntry)
                    return;

                uint32 l_Radius = l_RadiusEntry->RadiusMax;

                std::list<Unit*> l_TargetList;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(me, me, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(me, l_TargetList, l_Check);
                me->VisitNearbyObject(l_Radius, l_Searcher);

                if (!l_TargetList.size())
                    return;

                l_TargetList.sort(JadeCore::ObjectDistanceOrderPred(me));

                Unit* l_Unit = *l_TargetList.begin();
                if ((*l_Unit).GetDistance(*me) > l_Radius)
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                uint32 l_Intellect = l_Owner->GetUInt32Value(EUnitFields::UNIT_FIELD_STATS + Stats::STAT_INTELLECT);
                int32 l_Damage = l_FelCleave->Effects[EFFECT_0].BasePoints * l_Intellect;
                me->CastCustomSpell(me, eSpells::FelCleave, &l_Damage, NULL, NULL, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_warl_fel_lordAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Observer - 107100 for Call Observer - 201996
class spell_npc_warl_observer : public CreatureScript
{
    public:
        spell_npc_warl_observer() : CreatureScript("spell_npc_warl_observer") { }

        struct spell_npc_warl_observerAI : public ScriptedAI
        {
            spell_npc_warl_observerAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                CallObserver        = 201996,
                EyeOfTheObserver    = 212580
            };

            std::set<uint64> m_AffectedTarget;

            void AfterSummon(Unit* p_Summoner, Unit* p_Target, uint32 p_SpellId) override   ///< 7.1.5 - Build 23420: Corrected Observer HP to be equal to 10% of caster Health
            {
                uint64 l_WarlockHealth = p_Summoner->GetMaxHealth();
                me->SetMaxHealth(l_WarlockHealth / 10);
                me->SetFullHealth();
            }

            void Reset() override
            {
                AddTimedDelayedOperation(10, [=]() -> void
                {
                    SetFlyMode(true);

                    Position l_Pos      = me->GetPosition();
                    l_Pos.m_positionZ   += 5.0f;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveTakeoff(0, l_Pos);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                SpellInfo const* l_Observer = sSpellMgr->GetSpellInfo(eSpells::CallObserver);
                if (!l_Observer)
                    return;

                uint32 l_Radius = l_Observer->Effects[EFFECT_1].BasePoints;

                std::list<Unit*> l_TargetList;
                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(me, me, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(me, l_TargetList, l_Check);
                me->VisitNearbyObject(l_Radius, l_Searcher);

                std::set<uint64> l_TargetsToRemove;
                for (uint64 l_GUID : m_AffectedTarget)
                {
                    bool l_Found = false;
                    for (Unit* l_Target : l_TargetList)
                    {
                        if (l_Target->GetGUID() == l_GUID)
                        {
                            l_Found = true;
                            break;
                        }
                    }
                    if (!l_Found)
                        l_TargetsToRemove.insert(l_GUID);
                }

                std::set<uint64> l_NewTargets;
                for (Unit* l_Target : l_TargetList)
                {
                    if (m_AffectedTarget.find(l_Target->GetGUID()) == m_AffectedTarget.end())
                        l_NewTargets.insert(l_Target->GetGUID());
                }

                for (uint64 l_TargetGUID : l_TargetsToRemove)
                {
                    m_AffectedTarget.erase(l_TargetGUID);
                    Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                    if (!l_Target)
                        continue;

                    l_Target->RemoveAurasDueToSpell(eSpells::EyeOfTheObserver);
                }

                for (uint64 l_TargetGUID : l_NewTargets)
                {
                    m_AffectedTarget.insert(l_TargetGUID);
                    Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                    if (!l_Target)
                        continue;

                    me->CastSpell(l_Target, eSpells::EyeOfTheObserver, true);
                }
            }

            void JustDespawned() override
            {
                for (uint64 l_TargetGUID : m_AffectedTarget)
                {
                    Unit* l_Target = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                    if (!l_Target)
                        continue;

                    l_Target->RemoveAurasDueToSpell(eSpells::EyeOfTheObserver);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_warl_observerAI(creature);
        }
};

/// Last update : 7.3.5 build 26124
/// Demonic Empowerment - 193396
class spell_npc_demonic_emprowement_attackspeed : public SpellScriptLoader
{
    public:
        spell_npc_demonic_emprowement_attackspeed() : SpellScriptLoader("spell_npc_demonic_emprowement_attackspeed") { }

        class spell_npc_demonic_emprowement_attackspeed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_npc_demonic_emprowement_attackspeed_AuraScript);

            void AttackSpeedCalc(Unit* p_Target, bool remove)
            {
                uint32 l_AttackTime = 2 * IN_MILLISECONDS;
                Unit* l_Owner = p_Target->GetOwner();
                if (!l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                {
                    l_AttackTime *= l_Player->GetFloatValue(UNIT_FIELD_MOD_HASTE);

                    if (!remove)
                        l_AttackTime /= 1.5f;
                }

                p_Target->SetAttackTime(WeaponAttackType::BaseAttack, l_AttackTime);
                p_Target->SetAttackTime(WeaponAttackType::OffAttack, l_AttackTime);
                p_Target->SetAttackTime(WeaponAttackType::RangedAttack, l_AttackTime);
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();

                if (!l_Target)
                    return;

                AttackSpeedCalc(l_Target, false);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Target = GetTarget();

                if (!l_Target)
                    return;

                AttackSpeedCalc(l_Target, true);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_npc_demonic_emprowement_attackspeed_AuraScript::OnApply, EFFECT_2, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_npc_demonic_emprowement_attackspeed_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_npc_demonic_emprowement_attackspeed_AuraScript();
        }
};

/// Last Update 7.3.5 Build 26124
/// Fel Lord - 17252 for Grimoire : Felguard - 111898
class spell_npc_warl_grimoire_felguard : public CreatureScript
{
    public:
        spell_npc_warl_grimoire_felguard() : CreatureScript("spell_npc_warl_grimoire_felguard") { }

        struct spell_npc_warl_grimoire_felguardAI : public ScriptedAI
        {
            spell_npc_warl_grimoire_felguardAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                Felstorm                = 89751,
                LegionStrike            = 30213,
                DemonicEmpowerment      = 193396
            };

            enum eStates
            {
                JustSpawned,
                HasCharged,
                LegionStrikeCasted,
                FelstormCasted
            };

            uint8 m_State = eStates::JustSpawned;
            bool m_IsFromGrimoireOfService = false;
            uint32 m_TimeSinceLastCast = 0;

            void SetGUID(uint64 /*p_Guid*/, int32 /*p_Index*/) override
            {
                m_IsFromGrimoireOfService = true;
            }

            void OnMeleeAttackDone(Unit* /*p_Target*/, DamageInfo* /*p_DamageInfo*/, WeaponAttackType /*p_WeaponAttackType*/) override
            {
                me->ModifyPower(Powers::POWER_ENERGY, 5);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!m_IsFromGrimoireOfService)
                {
                    ScriptedAI::UpdateAI(p_Diff);
                    return;
                }

                if (me->HasAura(eSpells::Felstorm))
                    m_TimeSinceLastCast = 0;
                else
                {
                    m_TimeSinceLastCast += p_Diff;
                    ScriptedAI::UpdateAI(p_Diff);
                }

                Unit* l_Target = me->getVictim();

                if (!l_Target && me->ToUnit())
                    if (Unit* l_Owner = me->ToUnit()->GetOwner())
                        if (l_Owner->ToPlayer())
                            l_Target = l_Owner->ToPlayer()->GetSelectedUnit();

                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LegionStrike);
                if (!l_Target || !l_SpellInfo)
                    return;

                if (me->getVictim() != l_Target)
                    AttackStart(l_Target);

                if (!me->IsWithinMeleeRange(l_Target))
                {
                    if (m_State == eStates::JustSpawned)
                    {
                        me->GetMotionMaster()->MoveCharge(l_Target);
                        m_State = eStates::HasCharged;
                    }
                    else if (m_State > eStates::HasCharged)
                        me->GetMotionMaster()->MoveChase(l_Target);

                    return;
                }

                if (m_TimeSinceLastCast < 2 * IN_MILLISECONDS)
                    return;

                if (me->GetPower(POWER_ENERGY) < 100)
                    return;

                switch (m_State)
                {
                    case eStates::HasCharged:
                    {
                        m_State = eStates::LegionStrikeCasted;
                        me->CastSpell(l_Target, eSpells::LegionStrike, false);
                        m_TimeSinceLastCast = 0;
                        return;
                    }
                    case eStates::LegionStrikeCasted:
                    {
                        me->AttackStop();
                        me->CastSpell(me, eSpells::Felstorm, false);
                        m_TimeSinceLastCast = 0;
                        m_State = eStates::FelstormCasted;
                        return;
                    }
                    case eStates::FelstormCasted:
                    default:
                        break;
                }

                me->CastSpell(l_Target, eSpells::LegionStrike, false);
                m_TimeSinceLastCast = 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_warl_grimoire_felguardAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Felhunter - 417 for Grimoire
class spell_npc_warl_grimoire_felhunter : public CreatureScript
{
public:
    spell_npc_warl_grimoire_felhunter() : CreatureScript("spell_npc_warl_grimoire_felhunter") { }

    struct spell_npc_warl_grimoire_felhunterAI : public ScriptedAI
    {
        spell_npc_warl_grimoire_felhunterAI(Creature *creature) : ScriptedAI(creature) { }

        enum eSpells
        {
            ShadowBite = 54049
        };

        bool m_IsFromGrimoireOfService = false;
        uint32 m_TimeSinceLastCast = 0;

        void SetGUID(uint64 /*p_Guid*/, int32 /*p_Index*/) override
        {
            m_IsFromGrimoireOfService = true;
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!m_IsFromGrimoireOfService)
            {
                ScriptedAI::UpdateAI(p_Diff);
                return;
            }

            Unit* l_Target = me->getVictim();

            if (!l_Target && me->ToUnit())
                if (Unit* l_Owner = me->ToUnit()->GetOwner())
                    if (l_Owner->ToPlayer())
                        l_Target = l_Owner->ToPlayer()->GetSelectedUnit();

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ShadowBite);
            if (!l_Target || !l_SpellInfo || me->GetPower(POWER_ENERGY) < 100)
                return;

            if (me->getVictim() != l_Target)
                AttackStart(l_Target);

            if (m_TimeSinceLastCast > 2 * IN_MILLISECONDS)
            {
                me->CastSpell(l_Target, l_SpellInfo->Id, false);
                m_TimeSinceLastCast = 0;
            }
            else
                m_TimeSinceLastCast += p_Diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new spell_npc_warl_grimoire_felhunterAI(creature);
    }
};

/// Last Update 7.1.5 Build 23420
/// Succubus  - 1863 for Grimoire, 120526,
class spell_npc_warl_grimoire_succubus : public CreatureScript
{
public:
    spell_npc_warl_grimoire_succubus() : CreatureScript("spell_npc_warl_grimoire_succubus") { }

    struct spell_npc_warl_grimoire_succubusAI : public ScriptedAI
    {
        spell_npc_warl_grimoire_succubusAI(Creature *creature) : ScriptedAI(creature) { }

        enum eSpells
        {
            LashOfPain = 7814,
            Seduction = 6358
        };

        void Reset() override
        {
            me->SetMaxPower(POWER_ENERGY, 200);
            me->SetPower(POWER_ENERGY, 200);
        }

        void SetGUID(uint64 /*p_Guid*/, int32 /*p_Index*/) override
        {
            m_IsFromGrimoireOfService = true;
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (!m_IsFromGrimoireOfService)
            {
                ScriptedAI::UpdateAI(p_Diff);
                return;
            }

            Unit* l_Target = me->getVictim();

            if (!l_Target && me->ToUnit())
                if (Unit* l_Owner = me->ToUnit()->GetOwner())
                    if (l_Owner->ToPlayer())
                        l_Target = l_Owner->ToPlayer()->GetSelectedUnit();

            if (!l_Target)
                return;

            if (me->getVictim() != l_Target)
                AttackStart(l_Target);

            if (!m_FirstAttack)
            {
                m_FirstAttack = true;
                me->CastSpell(l_Target, eSpells::Seduction, false);
                return;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::LashOfPain);
            if (!l_SpellInfo || me->GetPower(POWER_ENERGY) < 100)
                return;

            if (m_TimeSinceLastCast > 2 * IN_MILLISECONDS)
            {
                me->CastSpell(l_Target, l_SpellInfo->Id, false);
                m_TimeSinceLastCast = 0;
            }
            else
                m_TimeSinceLastCast += p_Diff;

            DoMeleeAttackIfReady();
        }

    private:
        bool m_IsFromGrimoireOfService = false;
        bool m_FirstAttack = false;
        uint32 m_TimeSinceLastCast = 0;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new spell_npc_warl_grimoire_succubusAI(creature);
    }
};

/// Last Update 7.1.5 Build 23420
/// Infernal - 89 - Infernal (Lord of the flames) - 108452 - Abyssal - 58997
class spell_npc_infernal : public CreatureScript
{
    public:
        spell_npc_infernal() : CreatureScript("spell_npc_infernal") { }

        struct spell_npc_infernalAI : public ScriptedAI
        {
            spell_npc_infernalAI(Creature *creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_HELPER);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!me->isInCombat() && (!me->GetOwner() || !me->GetOwner()->isInCombat()))
                    return;

                if (!me->HasReactState(REACT_PASSIVE))
                {
                    if (Unit* victim = me->SelectVictim())
                        AttackStart(victim);

                    if (!me->getVictim())
                        return;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_infernalAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Psyfiend - 101398 for Psyfiend - 211522
class spell_npc_priest_psyfiend : public CreatureScript
{
    public:
        spell_npc_priest_psyfiend() : CreatureScript("spell_npc_priest_psyfiend") { }

        struct spell_npc_priest_psyfiendAI : public ScriptedAI
        {
            spell_npc_priest_psyfiendAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                Psyflay     = 199845,
                Psufiend    = 199824,
                MindFlay    = 237388
            };

            enum eNpcs
            {
                JadeSerpentStatue = 60849
            };

            uint64 m_VictimGUID = 0;

            void Reset() override
            {
                Unit* l_Caster = me->GetOwner();
                if (!l_Caster)
                    return;

                ApplyAllImmunities(true);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Psufiend))
                    me->SetMaxHealth(l_SpellInfo->Effects[EFFECT_0].BasePoints);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Victim = me->getVictim();
                Unit* l_Caster = me->GetOwner();
                if (!l_Caster)
                    return;

                Player* l_Player = l_Caster->ToPlayer();
                if (!l_Player)
                    return;

                if (m_VictimGUID == 0 || !sObjectAccessor->GetUnit(*l_Caster, m_VictimGUID) || (sObjectAccessor->GetUnit(*l_Caster, m_VictimGUID) && !sObjectAccessor->GetUnit(*l_Caster, m_VictimGUID)->isAlive()))
                {
                    if (!l_Victim || l_Victim->GetGUID() != l_Caster->GetTargetGUID())
                        l_Victim = sObjectAccessor->GetUnit(*l_Caster, l_Caster->GetTargetGUID());

                    if (l_Victim)
                    {
                        if (l_Victim->GetEntry() == eNpcs::JadeSerpentStatue || l_Victim->isTotem())
                            l_Victim = l_Victim->GetOwner();
                    }

                    if (!l_Victim && l_Player->GetSelectedUnit() && l_Caster->IsValidAttackTarget(l_Player->GetSelectedUnit()))
                        l_Victim = l_Player->GetSelectedUnit();

                    /// Psyfiend doesn't attack Jade Serpent Statue
                    if (!l_Victim)
                        return;

                    uint64 l_NewGUID = l_Victim->GetGUID();
                    if (l_NewGUID == m_VictimGUID)
                        return;

                    if (l_NewGUID == me->GetGUID())
                        return;

                    m_VictimGUID = l_NewGUID;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*l_Player, m_VictimGUID);
                if (!l_Target)
                    return;

                if (l_Target == me->ToUnit())
                    return;

                me->CastSpell(l_Target, eSpells::Psyflay, false);
                me->SetTarget(m_VictimGUID);
            }

            void HealReceived(Unit* /* p_DoneBy */, uint32& p_AddHealth) override
            {
                p_AddHealth = 0;
            }

            void DamageTaken(Unit* /* p_Attacker */, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo && (p_SpellInfo->IsTargetingArea() || p_SpellInfo->IsAffectingArea() ||
                    p_SpellInfo->Id == eSpells::MindFlay))
                    p_Damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_priest_psyfiendAI(creature);
        }
};

class HatiAppearanceHelper
{
    enum eTimers
    {
        UpdateAppearance = 2 * IN_MILLISECONDS
    };

    enum eModels
    {
        Default         = 68716,
        Eaglewatch      = 68899,
        HiddenBow       = 68900,
        ElekksThunder   = 68901,
        BoarshotCannon  = 68898
    };

    enum eArtAppearances
    {
        Default1 = 288,
        Default2 = 452,
        Default3 = 453,
        Default4 = 454,

        EagleWatch1 = 448,
        EagleWatch2 = 456,
        EagleWatch3 = 457,
        EagleWatch4 = 455,

        HiddenBow1 = 918,
        HiddenBow2 = 917,
        HiddenBow3 = 919,
        HiddenBow4 = 920,

        ElekksThunder1 = 462,
        ElekksThunder2 = 461,
        ElekksThunder3 = 450,
        ElekksThunder4 = 463,

        BoarshotCannon1 = 460,
        BoarshotCannon2 = 458,
        BoarshotCannon3 = 459,
        BoarshotCannon4 = 449
    };

    public:

        HatiAppearanceHelper(Creature* p_Hati) :
            m_Hati(p_Hati),
            m_UpdateAppearanceTimer(eTimers::UpdateAppearance), m_LastArtAppearance(0)
        {
        }

        void Update(uint32 p_Diff)
        {
            if (m_Hati == nullptr)
                return;

            if (m_UpdateAppearanceTimer > p_Diff)
            {
                m_UpdateAppearanceTimer -= p_Diff;
                return;
            }

            m_UpdateAppearanceTimer = eTimers::UpdateAppearance;

            if (Player* l_Owner = GetOwner())
            {
                if (auto l_SwapperState = l_Owner->GetCharacterWorldStateValue(CharacterWorldStates::EssenceSwapperState))
                {
                    if (m_Hati->GetDisplayId() != l_SwapperState)
                    {

                        CreatureTemplate const* l_CreatureTemplate = sObjectMgr->GetCreatureTemplate(l_Owner->GetCharacterWorldStateValue(CharacterWorldStates::EssenceSwapperEntry));
                        if (l_CreatureTemplate == nullptr)
                        {
                            ClearEssenceSwapper(l_Owner);
                            return;
                        }

                        CreatureDisplayInfoEntry const* l_DisplayInfo = sCreatureDisplayInfoStore.LookupEntry(l_SwapperState);
                        CreatureFamilyEntry const* l_PetFamily = sCreatureFamilyStore.LookupEntry(l_CreatureTemplate->family);
                        if (l_DisplayInfo == nullptr || l_PetFamily == nullptr)
                        {
                            ClearEssenceSwapper(l_Owner);
                            return;
                        }

                        auto l_DisplayScale = std::max(l_DisplayInfo->CreatureModelScale, 0.1f);
                        auto l_FamilyScale = std::max(l_PetFamily->maxScale, 0.1f);

                        auto l_Scale = l_FamilyScale / l_DisplayScale;
                        l_Scale = RoundToInterval(l_Scale, 0.1f, 10.f);

                        m_Hati->SetDisplayId(l_SwapperState);
                        m_Hati->SetNativeDisplayId(l_SwapperState);
                        m_Hati->SetObjectScale(l_Scale);
                    }
                }
                else
                {
                    const float l_DefaultObjectScale = 1.0f;

                    if (!G3D::fuzzyEq(m_Hati->GetObjectScale(), l_DefaultObjectScale))
                        m_Hati->SetObjectScale(l_DefaultObjectScale);

                    if (MS::Artifact::Manager* l_Artifact = l_Owner->GetCurrentlyEquippedArtifact())
                    {
                        m_Hati->SetObjectScale(1.0f);

                        uint32 l_CurrentArtAppearance = l_Artifact->GetAppearanceID();

                        if (m_LastArtAppearance != l_CurrentArtAppearance)
                        {
                            m_LastArtAppearance = l_CurrentArtAppearance;

                            m_Hati->SetDisplayId(GetModelForArtAppearance(m_LastArtAppearance));
                        }
                    }
                }
            }
        }

    private:

        Player* GetOwner()
        {
            if (!m_Hati)
                return nullptr;

            if (Unit* l_Owner = m_Hati->GetOwner())
                if (Player* l_Player = l_Owner->ToPlayer())
                    return l_Player;

            return nullptr;
        }

        uint32 GetModelForArtAppearance(uint32 p_ArtAppearance)
        {
            switch (p_ArtAppearance)
            {
                case eArtAppearances::EagleWatch1:
                case eArtAppearances::EagleWatch2:
                case eArtAppearances::EagleWatch3:
                case eArtAppearances::EagleWatch4:
                    return eModels::Eaglewatch;
                case eArtAppearances::HiddenBow1:
                case eArtAppearances::HiddenBow2:
                case eArtAppearances::HiddenBow3:
                case eArtAppearances::HiddenBow4:
                    return eModels::HiddenBow;
                case eArtAppearances::ElekksThunder1:
                case eArtAppearances::ElekksThunder2:
                case eArtAppearances::ElekksThunder3:
                case eArtAppearances::ElekksThunder4:
                    return eModels::ElekksThunder;
                case eArtAppearances::BoarshotCannon1:
                case eArtAppearances::BoarshotCannon2:
                case eArtAppearances::BoarshotCannon3:
                case eArtAppearances::BoarshotCannon4:
                    return eModels::BoarshotCannon;
                case eArtAppearances::Default1:
                case eArtAppearances::Default2:
                case eArtAppearances::Default3:
                case eArtAppearances::Default4:
                default:
                    return eModels::Default;
            }

            return eModels::Default;
        }

        void ClearEssenceSwapper(Player* p_Player)
        {
            p_Player->SetCharacterWorldState(CharacterWorldStates::EssenceSwapperState, 0);
            p_Player->SetCharacterWorldState(CharacterWorldStates::EssenceSwapperEntry, 0);
        }

    private:

        Creature* m_Hati;
        uint32 m_UpdateAppearanceTimer;
        uint32 m_LastArtAppearance;
};

/// Last Update 7.0.3 Build 22522
/// Hati - 100324 for Titanstrike - 197344
class spell_npc_hun_hati : public CreatureScript
{
    public:
        spell_npc_hun_hati() : CreatureScript("spell_npc_hun_hati") { }

        struct spell_npc_hun_hatiAI : public ScriptedAI
        {
            spell_npc_hun_hatiAI(Creature *creature) : ScriptedAI(creature), m_AppearanceHelper(creature) { }

            enum eSpells
            {
                Titanstrike = 197344,
                Stormbound  = 197388
            };

            void Reset() override
            {
                if (!me->GetOwner())
                    me->DespawnOrUnsummon();

                me->SetReactState(REACT_PASSIVE);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                l_Player->m_Events.AddEvent(new SummonHati(l_Player->GetGUID()), l_Player->m_Events.CalculateTime(30 * IN_MILLISECONDS));
                l_Player->RemoveSpellCooldown(eSpells::Stormbound);
                l_Player->AddSpellCooldown(eSpells::Stormbound, 0, 29 * IN_MILLISECONDS);
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_AppearanceHelper.Update(p_Diff);

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (me->GetDistance(*l_Player) > 90)
                {
                    me->DespawnOrUnsummon();
                    l_Player->m_Events.AddEvent(new SummonHati(l_Player->GetGUID()), l_Player->m_Events.CalculateTime(1));
                }

                Pet* l_Pet = l_Player->GetPet();
                if (!l_Pet)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (!l_Pet->isInCombat())
                {
                    Position l_Position = l_Owner->GetPosition();
                    me->GetMotionMaster()->MoveFollow(l_Owner, PET_FOLLOW_DIST, me->GetFollowAngle());
                    return;
                }

                if (Unit* l_Victim = l_Pet->getVictim())
                {
                    AttackStart(l_Victim);
                }
                else if (l_Pet->getThreatManager().isThreatListEmpty() && me->getThreatManager().isThreatListEmpty())
                {
                    EnterEvadeMode();
                }

                DoMeleeAttackIfReady();
            }

        private:

            HatiAppearanceHelper m_AppearanceHelper;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_hun_hatiAI(creature);
        }
};

/// Last Update 7.0.3
/// Bloodworm - 99773
class spell_npc_dk_bloodworm : public CreatureScript
{
    public:
        spell_npc_dk_bloodworm() : CreatureScript("spell_npc_dk_bloodworm") { }

        enum eSpells
        {
            BloodWormsSpell = 195679
        };

        struct spell_npc_dk_bloodworm_impAI : public ScriptedAI
        {
            spell_npc_dk_bloodworm_impAI(Creature *creature) : ScriptedAI(creature) { }

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                Unit* l_Owner = me->GetOwner();
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BloodWormsSpell);
                if (l_SpellInfo == nullptr)
                    return;

                if (l_Owner == nullptr)
                    return;

                Unit* l_OwnerTarget = l_Owner->getVictim();

                if (l_OwnerTarget == nullptr)
                    if (Player* l_Player = l_Owner->ToPlayer())
                        l_OwnerTarget = l_Player->GetSelectedUnit();

                if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && (!me->getVictim() || me->getVictim() != l_OwnerTarget))
                    AttackStart(l_OwnerTarget);

                if (l_Owner->GetHealthPct() < l_SpellInfo->Effects[EFFECT_1].BasePoints)
                {
                    Burst();
                    me->DespawnOrUnsummon();
                }

                DoMeleeAttackIfReady();
            }

            void JustDespawned() override
            {
                Burst();
            }

            void Burst()
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::BloodWormsSpell);
                if (l_SpellInfo == nullptr)
                    return;

                if (Unit* l_Owner = me->GetOwner())
                {
                    uint32 l_Health = ((l_Owner->GetMaxHealth() - l_Owner->GetHealth()) / 100.0f) * l_SpellInfo->Effects[EFFECT_2].BasePoints;
                    me->HealBySpell(l_Owner, l_SpellInfo, l_Health, false);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_dk_bloodworm_impAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Gargoyle - 27829
class spell_npc_dk_gargoyle : public CreatureScript
{
    public:
        spell_npc_dk_gargoyle() : CreatureScript("spell_npc_dk_gargoyle") { }

        enum eSpells
        {
            GargoyleStrike = 51963
        };

        struct spell_npc_dk_gargoyleAI : public ScriptedAI
        {
            spell_npc_dk_gargoyleAI(Creature *creature) : ScriptedAI(creature) { }

            uint64 m_TargetGUID = 0;

            void Reset() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = l_Player->GetSelectedUnit();
                if (!l_Target)
                    return;

                m_TargetGUID = l_Target->GetGUID();
            }

            void UpdateAI(const uint32 /*p_Diff*/) override
            {

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_Target = me->getVictim() ? me->getVictim() : sObjectAccessor->GetUnit(*l_Owner, m_TargetGUID);
                if (!l_Target || (l_Target && !me->IsValidAttackTarget(l_Target)))
                {
                    Player* l_Player = l_Owner->ToPlayer();
                    if (!l_Player)
                        return;

                    GuardianPetTarget l_GuardianPetTarget;
                    l_GuardianPetTarget.UpdateTarget(me, l_Player, l_Target, false);
                }

                if (!l_Target)
                    return;

                m_TargetGUID = l_Target->GetGUID();

                me->CastSpell(l_Target, eSpells::GargoyleStrike, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_dk_gargoyleAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Val'kyr Battlemaiden - 100876
class spell_npc_dk_valkyr : public CreatureScript
{
    public:
        spell_npc_dk_valkyr() : CreatureScript("spell_npc_dk_valkyr") { }

        enum eSpells
        {
            ValkyrStrike    = 198715,
            DarkArbiter     = 207349
        };

        struct spell_npc_dk_valkyrAI : public PassiveAI
        {
            spell_npc_dk_valkyrAI(Creature *creature) : PassiveAI(creature) { }

            uint32 m_PointID = 1;

            void Reset() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                me->setActive(true);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetByteFlag(UNIT_FIELD_BYTES_1, UNIT_BYTES_1_OFFSET_ANIM_TIER, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                me->AddUnitMovementFlag(MOVEMENTFLAG_HOVER);

                Position l_Pos = l_Owner->GetPosition();
                l_Pos.m_positionZ += 2.0f;
                me->GetMotionMaster()->MoveSmoothFlyPath(m_PointID, l_Pos);
                ++m_PointID;

                if (!m_TimerInitialized)
                {
                    m_TimerInitialized = true;
                    m_ConfusionTimer = urand(1250, 2250);
                    int32 l_TotalDuration = sSpellMgr->GetSpellInfo(eSpells::DarkArbiter)->GetMaxDuration();
                    l_TotalDuration += m_ConfusionTimer + urand(1900, 2100) + 1000; // allow a second grace timer for last missile to hit
                    if (TempSummon* l_TempSumm = me->ToTempSummon())
                        l_TempSumm->SetDuration(l_TotalDuration);

                    if (me->GetOwner() && me->GetOwner()->ToPlayer())
                        me->GetOwner()->ToPlayer()->SendTempSummonUITimer(me, l_TotalDuration);
                }
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                if (m_ConfusionTimer > 0)
                {
                    m_ConfusionTimer -= p_Diff;
                    return;
                }

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = me->getVictim();
                GuardianPetTarget l_GuardianPetTarget;
                l_GuardianPetTarget.UpdateTarget(me, l_Player, l_Target, false);

                if (!l_Target)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                me->SetFacingToObject(l_Target);

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::ValkyrStrike))
                {
                    if (me->IsWithinCombatRange(l_Target, l_SpellInfo->GetMaxRange(false))
                        && me->IsWithinLOS(l_Target->GetPositionX(), l_Target->GetPositionY(), l_Target->GetPositionZ()))
                    {
                        me->GetMotionMaster()->Clear();
                        me->CastSpell(l_Target, l_SpellInfo, TRIGGERED_NONE);
                    }
                    else
                    {
                        Position l_Pos = l_Target->GetPosition();
                        l_Pos.m_positionX = l_Target->GetPositionX() - (5.0f * cos(l_Target->GetOrientation()));
                        l_Pos.m_positionY = l_Target->GetPositionY() - (5.0f * sin(l_Target->GetOrientation()));
                        l_Pos.m_positionZ = l_Target->GetPositionZ() + 3.0f;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveSmoothFlyPath(m_PointID, l_Pos);
                        ++m_PointID;
                    }
                }
            }

    private:
        int32 m_ConfusionTimer = 0;
        bool m_TimerInitialized = false;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_dk_valkyrAI(creature);
        }
};

/// Last Update 7.0.3 Build 22522
/// Treant - 103822
/// For Force Of Nature - 205636
class npc_force_of_nature_balance : public CreatureScript
{
    public:
        npc_force_of_nature_balance() : CreatureScript("npc_force_of_nature_balance") { }

        enum eSpells
        {
            TreantTaunt = 205644,
            ForceOfNature = 205636
        };

        struct npc_force_of_nature_balanceAI : public ScriptedAI
        {
            npc_force_of_nature_balanceAI(Creature* creature) : ScriptedAI(creature) {}

            uint64 m_TargetGUID = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                SpellInfo const* l_ForceOfNature = sSpellMgr->GetSpellInfo(eSpells::ForceOfNature);
                if (!l_ForceOfNature)
                    return;

                std::list<Unit*> l_TargetList;
                float l_Radius = l_ForceOfNature->Effects[EFFECT_0].RadiusEntry->RadiusMax;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(me, me, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(me, l_TargetList, l_Check);
                me->VisitNearbyObject(l_Radius, l_Searcher);

                Unit* l_FinalTarget = nullptr;

                for (Unit* l_Target : l_TargetList)
                {
                    uint64 l_TargetGUID = l_Target->GetTargetGUID();
                    if (l_TargetGUID)
                    {
                        Unit* l_TargetsTarget = sObjectAccessor->GetUnit(*me, l_TargetGUID);
                        if (l_TargetsTarget && l_TargetsTarget->ToCreature() && l_TargetsTarget->ToCreature()->GetEntry() != me->GetEntry())
                            continue;
                    }

                    if (!p_Summoner->IsValidAttackTarget(l_Target))
                        continue;

                    me->CastSpell(l_Target, eSpells::TreantTaunt, false);
                    l_FinalTarget = l_Target;
                    break;
                }

                if (!l_FinalTarget)
                {
                    if (!l_TargetList.size())
                    {
                        Unit* l_Caster = me->GetOwner();
                        if (!l_Caster)
                            return;

                        l_FinalTarget = l_Caster->getVictim();
                    }
                }

                if (!l_FinalTarget)
                    return;

                m_TargetGUID = l_FinalTarget->GetGUID();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Target = nullptr;
                l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);
                if (!l_Target)
                    return;

                AttackStart(l_Target);
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_force_of_nature_balanceAI(creature);
        }
};

/// Resonance Totem - 102392
class spell_npc_resonance_totem : public CreatureScript
{
    public:
        spell_npc_resonance_totem() : CreatureScript("spell_npc_resonance_totem") { }

        struct sspell_npc_resonance_totemAI : public TotemAI
        {
            sspell_npc_resonance_totemAI(Creature* p_Creature) : TotemAI(p_Creature) { }

            enum eSpells
            {
                ResonanceTotemAura = 202192
            };

            void Reset() override
            {
                me->CastSpell(me, eSpells::ResonanceTotemAura, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new sspell_npc_resonance_totemAI(p_Creature);
        }
};

/// Storm Totem - 106317
class spell_npc_storm_totem : public CreatureScript
{
    public:
        spell_npc_storm_totem() : CreatureScript("spell_npc_storm_totem") { }

        struct spell_npc_storm_totem_totemAI : public TotemAI
        {
            spell_npc_storm_totem_totemAI(Creature* p_Creature) : TotemAI(p_Creature) { }

            enum eSpells
            {
                StormTotemAura = 210652
            };

            void Reset() override
            {
                me->CastSpell(me, eSpells::StormTotemAura, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_storm_totem_totemAI(p_Creature);
        }
};

/// Ember Totem - 106319
class spell_npc_ember_totem : public CreatureScript
{
    public:
        spell_npc_ember_totem() : CreatureScript("spell_npc_ember_totem") { }

        struct spell_npc_ember_totem_totemAI : public TotemAI
        {
            spell_npc_ember_totem_totemAI(Creature* p_Creature) : TotemAI(p_Creature) { }

            enum eSpells
            {
                EmberTotemAura = 210658
            };

            void Reset() override
            {
                me->CastSpell(me, eSpells::EmberTotemAura, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_ember_totem_totemAI(p_Creature);
        }
};

/// Skyfury Totem - 105427
class spell_npc_skyfury_totem : public CreatureScript
{
    public:
        spell_npc_skyfury_totem() : CreatureScript("spell_npc_skyfury_totem") { }

        struct spell_npc_skyfury_totemAI : public TotemAI
        {
            spell_npc_skyfury_totemAI(Creature* p_Creature) : TotemAI(p_Creature) { }

            enum eSpells
            {
                SkyfuryTotemAreatrigger   = 208964
            };

            void Reset() override
            {
                Unit* l_Caster = me->GetOwner();
                if (!l_Caster)
                    return;

                me->CastSpell(me, eSpells::SkyfuryTotemAreatrigger, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_skyfury_totemAI(p_Creature);
        }
};

/// Tailwind Totem - 106321
class spell_npc_tailwind_totem : public CreatureScript
{
    public:
        spell_npc_tailwind_totem() : CreatureScript("spell_npc_tailwind_totem") { }

        struct spell_npc_tailwind_totem_totemAI : public TotemAI
        {
            spell_npc_tailwind_totem_totemAI(Creature* p_Creature) : TotemAI(p_Creature) { }

            enum eSpells
            {
                TailwindTotemAura = 210659
            };

            void Reset() override
            {
                me->CastSpell(me, eSpells::TailwindTotemAura, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_tailwind_totem_totemAI(p_Creature);
        }
};

/// Counterstrike Totem - 106319
class spell_npc_counterstrike_totem : public CreatureScript
{
    public:
        spell_npc_counterstrike_totem() : CreatureScript("spell_npc_counterstrike_totem") { }

        struct spell_npc_counterstrike_totemAI : public TotemAI
        {
            spell_npc_counterstrike_totemAI(Creature* p_Creature) : TotemAI(p_Creature) { }

            enum eSpells
            {
                CounterstrikeTotemAreatrigger   = 208990,
                CounterstrikeTotemDamage        = 209031
            };

            void Reset() override
            {
                Unit* l_Caster = me->GetOwner();
                if (!l_Caster)
                    return;

                me->SetMaxHealth((l_Caster->GetMaxHealth(l_Caster) * 2) / 100);
                me->CastSpell(me, eSpells::CounterstrikeTotemAreatrigger, true);
            }

            void UpdateAI(uint32 const p_Diff) override { }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_counterstrike_totemAI(p_Creature);
        }
};

/// Earth And Fire Spirits
/// For Earth, Storm, and Fire - 138123
class spell_npc_earth_and_fire_spirit : public CreatureScript
{
    public:
        spell_npc_earth_and_fire_spirit() : CreatureScript("npc_earth_and_fire_spirit") { }

        struct spell_npc_earth_and_fire_spiritAI : public ScriptedAI
        {
            spell_npc_earth_and_fire_spiritAI(Creature* creature) : ScriptedAI(creature)
            {
                m_TargetGUID = 0;
            }

            uint64 m_TargetGUID;

            enum eSpells
            {
                EarthSpiritVisual       = 138083,
                FireSpiritVisual        = 138081,
                SetSpiritStats          = 138130,
                CloneCaster             = 60352,
                StormEarthAndFire       = 137639,
                FistsOfFury             = 113656,
                CinidariaTheSymbiote    = 207692
            };

            void EnterCombat(Unit* /*p_Attacker*/) override
            {
                if (me->GetOwner() && me->GetOwner()->HasAura(eSpells::CinidariaTheSymbiote))
                    me->CastSpell(me, eSpells::CinidariaTheSymbiote, true);
            }

            void Reset() override
            {
                if (!me->HasAura(eSpells::SetSpiritStats))
                    me->CastSpell(me, eSpells::SetSpiritStats, true);

                Unit* l_Target = nullptr;
                if (m_TargetGUID)
                    l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);

                TempSummon* l_TempSummon = me->ToTempSummon();
                if (!l_TempSummon)
                    return;

                Unit* l_Owner = l_TempSummon->GetSummoner();
                if (!l_Owner)
                    return;

                l_Owner->CastSpell(me, eSpells::CloneCaster, true);
                std::set<uint64>& l_EarthAndFireSpirts = l_Owner->m_SpellHelper.GetUint64Set()[eSpellHelpers::EarthAndFireSpirit];
                if (l_EarthAndFireSpirts.find(me->GetGUID()) == l_EarthAndFireSpirts.end())
                    l_EarthAndFireSpirts.insert(me->GetGUID());

                if (!l_Target)
                {
                    Player* l_Player = l_Owner->ToPlayer();
                    if (!l_Player)
                        return;

                    l_Target = l_Player->GetSelectedUnit();
                    if (!l_Target)
                        return;
                }

                m_TargetGUID = l_Target->GetGUID();

                if (Aura* l_Aura = l_Owner->GetAura(eSpells::StormEarthAndFire))
                    l_Aura->SetScriptGuid(0, m_TargetGUID);

                if (l_Owner->IsValidAttackTarget(l_Target))
                    AttackStart(l_Target);
            }

            void OwnerInterrupted(SpellInfo const* p_SpellInfo) override
            {
                me->InterruptNonMeleeSpells(true);
            }

            void JustDespawned() override
            {
                TempSummon* l_TempSummon = me->ToTempSummon();
                if (!l_TempSummon)
                    return;

                Unit* l_Owner = l_TempSummon->GetSummoner();
                if (!l_Owner)
                    return;

                l_Owner->m_SpellHelper.GetUint64Set()[eSpellHelpers::EarthAndFireSpirit].erase(l_TempSummon->GetGUID());
            }


            void UpdateAI(uint32 const p_Diff) override
            {
                TempSummon* l_TempSummon = me->ToTempSummon();
                if (!l_TempSummon)
                    return;

                Unit* l_Owner = l_TempSummon->GetSummoner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);
                if (!l_Target)
                    return;

                if (l_Player->HasAura(eSpells::FistsOfFury))
                    return;

                if (me->getVictim() && me->getVictim() != l_Target && !UpdateVictim())
                {
                    AttackStart(l_Target);
                    return;
                }

                DoMeleeAttackIfReady();
            }

            void SetGUID(uint64 p_Guid, int32 p_ID /* = 0 */) override
            {
                if (!p_Guid)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*me, p_Guid);
                if (!l_Target)
                    return;

                m_TargetGUID = p_Guid;

                AttackStart(l_Target);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (me->GetSpellModOwner())
                    me->GetSpellModOwner()->RemoveAura(eSpells::StormEarthAndFire); /// https://cdn.discordapp.com/attachments/453573411588014080/493782469594841088/asdasdas.png When one of the spirits dies, SeF is cancelled.

                return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_earth_and_fire_spiritAI(creature);
        }
};

/// 7.1.5 - Build 23420
/// Called by Shadow Bulwark - 17767
class spell_warl_shadow_bulwark : public SpellScriptLoader
{
public:
    spell_warl_shadow_bulwark() : SpellScriptLoader("spell_warl_shadow_bulwark") { }

    class spell_warl_shadow_bulwark_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_shadow_bulwark_SpellScript);

        SpellCastResult CheckConditions()
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return SPELL_FAILED_DONT_REPORT;

            Pet* l_Pet = l_Caster->ToPet();
            if (!l_Pet)
                return SPELL_CAST_OK;

            PetSpellMap::iterator l_Itr = l_Pet->m_spells.find((GetSpellInfo()->Id));

            if (l_Itr->second.active == ActiveStates::ACT_ENABLED && l_Caster->GetHealthPct() > 20.0f)
                return SPELL_FAILED_DONT_REPORT;

            return SPELL_CAST_OK;
        }

        void Register()
        {

            OnCheckCast += SpellCheckCastFn(spell_warl_shadow_bulwark_SpellScript::CheckConditions);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_shadow_bulwark_SpellScript();
    }
};

/// Last Update: 7.0.3: 22522
/// Dark Minion - 94072
/// For Black Arrow - 194599
class npc_dark_minion : public CreatureScript
{
    public:
        npc_dark_minion() : CreatureScript("npc_dark_minion") { }

        enum eSpells
        {
            Provoke          = 105509,
            BlackArrow       = 194599,
            BlackArrowSummon = 186070
        };

        struct npc_dark_minionAI : public ScriptedAI
        {
            npc_dark_minionAI(Creature* creature) : ScriptedAI(creature) {}

            uint64 m_TargetGUID = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                SpellInfo const* l_BlackArrowSummon = sSpellMgr->GetSpellInfo(eSpells::BlackArrowSummon);
                if (!l_BlackArrowSummon || !p_Summoner)
                    return;

                std::list<Unit*> l_TargetList;
                float l_Radius = l_BlackArrowSummon->Effects[0].RadiusEntry->RadiusMax;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(me, me, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(me, l_TargetList, l_Check);
                me->VisitNearbyObject(l_Radius, l_Searcher);

                Unit* l_FinalTarget = nullptr;

                for (Unit* l_Target : l_TargetList)
                {
                    if (!l_Target->HasAura(eSpells::BlackArrow, p_Summoner->GetGUID()))
                        continue;

                    l_FinalTarget = l_Target;
                    break;
                }

                if (!l_FinalTarget)
                {
                    if (!l_TargetList.size())
                        l_FinalTarget = p_Summoner->getVictim();
                    else
                        l_FinalTarget = l_TargetList.front();
                }

                if (!l_FinalTarget)
                    return;

                m_TargetGUID = l_FinalTarget->GetGUID();
                l_FinalTarget->AddThreat(me, 1000000.0f);  ///< Fixate target on Dark Minion
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Target = nullptr;
                l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);
                if (!l_Target)
                    return;

                /// Even if target was changed, fixate it one more time
                if (l_Target->GetTargetGUID() != me->GetGUID())
                    l_Target->AddThreat(me, 1000000.0f);

                AttackStart(l_Target);
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dark_minionAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Stampede - 102199
/// For Stampede - 201430
class spell_npc_stampede : public CreatureScript
{
    public:
        spell_npc_stampede() : CreatureScript("spell_npc_stampede") { }

        struct spell_npc_stampedeAI : public ScriptedAI
        {
            spell_npc_stampedeAI(Creature* creature) : ScriptedAI(creature) {}

            enum eSpells
            {
                StampedePeriodic = 201631
            };

            enum eDatas
            {
                InvisibleDisplay = 11686
            };

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetDisplayId(eDatas::InvisibleDisplay);
                me->CastSpell(me, eSpells::StampedePeriodic, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_stampedeAI(creature);
        }
};

/// Last Update 7.0.3
/// 102401 - Unholy Mutation (Summoned by Unholy mutation - 201934)
class spell_npc_unhloy_mutation : public CreatureScript
{
    public:
        spell_npc_unhloy_mutation() : CreatureScript("spell_npc_unhloy_mutation") { }

        enum eSpells
        {
            UnholyMutation      = 201934,
            UnholyMutationSlime = 200646
        };

        enum eEvents
        {
            CastMutationSlime = 1
        };

        struct spell_npc_unhloy_mutation_AI : public ScriptedAI
        {
            spell_npc_unhloy_mutation_AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            int32  m_RadiusSearch = 0;
            uint64 m_Owner = 0;

            EventMap events;

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(eEvents::CastMutationSlime, 1000);
                me->SetHealth(200000);
                me->SetMaxHealth(200000);
            }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::UnholyMutation);

                if (l_SpellInfo != nullptr)
                    m_RadiusSearch = l_SpellInfo->Effects[EFFECT_2].BasePoints;

                m_Owner = p_Summoner->GetGUID();

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

                if (events.ExecuteEvent() == eEvents::CastMutationSlime)
                {
                    if (Unit* l_Owner = sObjectAccessor->FindUnit(m_Owner))
                    {
                        std::list<Unit*> l_Targets;

                        JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(me, me, m_RadiusSearch);
                        JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(me, l_Targets, l_Check);
                        me->VisitNearbyObject(m_RadiusSearch, l_Searcher);

                        for (Unit* l_Target : l_Targets)
                        {
                            if (l_Target->isAlive() && me->IsWithinLOSInMap(l_Target) && l_Owner->IsValidAttackTarget(l_Target))
                            {
                                me->CastSpell(l_Target, eSpells::UnholyMutationSlime, true);
                            }
                        }
                        events.ScheduleEvent(eEvents::CastMutationSlime, 1000);
                    }
                    else
                        me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_unhloy_mutation_AI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// 97055 - Shambling Horror (Summoned by Summon Shambling Horror - 191759)
class spell_npc_shambling_horror : public CreatureScript
{
    public:
        spell_npc_shambling_horror() : CreatureScript("spell_npc_shambling_horror") { }

        struct spell_npc_shambling_horror_AI : public ScriptedAI
        {
            spell_npc_shambling_horror_AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Necrobomb = 191758
            };

            bool m_NecrobombCaster = false;

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                if (m_NecrobombCaster)
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_Victim = l_Owner->getVictim();
                if (!l_Victim || me->GetDistance(l_Victim) <= 1.f)
                {
                    me->CastSpell(me, eSpells::Necrobomb, false, 0, nullptr, l_Owner->GetGUID());
                    m_NecrobombCaster = true;
                    me->DespawnOrUnsummon(1);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_shambling_horror_AI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Skulker - 99541
class spell_npc_dk_skulker : public CreatureScript
{
    public:
        spell_npc_dk_skulker() : CreatureScript("spell_npc_dk_skulker") { }

        enum eSpells
        {
            SkulkerShot = 212423,
            ArrowSpray  = 207260
        };

        struct spell_npc_dk_skulkerAI : public ScriptedAI
        {
            spell_npc_dk_skulkerAI(Creature *creature) : ScriptedAI(creature) { }

            void Reset() override
            {

            }

            void UpdateAI(const uint32 p_Diff) override
            {
                UpdateAction(p_Diff);
            }

        private:

            Pet* GetGhoul()
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return nullptr;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return nullptr;

                if (Pet* l_Pet = l_Player->GetPet())
                    if (l_Pet->GetEntry() == DeathKnightPet::Ghoul || l_Pet->GetEntry() == DeathKnightPet::Abomination)
                        return l_Pet;

                return nullptr;
            }

            void UpdateAction(uint32 p_Diff)
            {
                if (m_UpdateTimer > p_Diff)
                {
                    m_UpdateTimer -= p_Diff;
                    return;
                }

                m_UpdateTimer = 1 * IN_MILLISECONDS;

                if (Pet* l_Ghoul = GetGhoul())
                {
                    if (CharmInfo* l_CharmInfo = l_Ghoul->GetCharmInfo())
                    {
                        if (l_CharmInfo->IsCommandAttack() || (l_Ghoul->ToUnit() && l_Ghoul->ToUnit()->isInCombat() && l_Ghoul->getVictim()))
                            AttackTarget(l_Ghoul->getVictim());
                        else
                        {
                            me->InterruptNonMeleeSpells(false);
                            me->AttackStop();

                            if (!me->IsMoving())
                            {
                                if (me->GetDistance(l_Ghoul) >= 5.0f)
                                {
                                    me->GetMotionMaster()->MoveFollow(l_Ghoul, 1.0f, 0.0f);
                                }
                            }
                        }
                    }
                }
            }

            void AttackTarget(Unit* p_Target)
            {
                if (!p_Target)
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (AttackTargetAOE(p_Target))
                    return;

                me->CastSpell(p_Target, eSpells::SkulkerShot, false);

                ++m_SkulkerShotCounter;
            }

            bool AttackTargetAOE(Unit* p_Target)
            {
                if (p_Target == nullptr)
                    return false;

                if (m_SkulkerShotCounter >= 2)
                {
                    m_SkulkerShotCounter = 0;

                    const float l_Dist = 8.0f;
                    const int32 l_AoeCount = 2;

                    std::list<Unit*> l_Targets;
                    JadeCore::AnyUnfriendlyUnitInObjectRangeCheck u_check(p_Target, me, l_Dist);
                    JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(p_Target, l_Targets, u_check);
                    p_Target->VisitNearbyObject(l_Dist, searcher);

                    if (l_Targets.size() > l_AoeCount)
                    {
                        me->CastSpell(p_Target, eSpells::ArrowSpray, false);
                        return true;
                    }
                }

                return false;
            }


        private:

            uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;
            uint32 m_SkulkerShotCounter = 0;

        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_dk_skulkerAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Skulker Shot - 212423
class spell_npc_skulker_shot : public SpellScriptLoader
{
    public:
        spell_npc_skulker_shot() : SpellScriptLoader("spell_npc_skulker_shot") { }

        class spell_npc_skulker_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_npc_skulker_shot_SpellScript);

            void HandleHitTarget(SpellEffIndex p_EffIndex)
            {
                Unit* l_Target = GetHitUnit();
                Unit* l_MainTarget = GetExplTargetUnit();
                if (!l_Target || !l_MainTarget)
                    return;

                if (l_Target == l_MainTarget)
                {
                    SetHitDamage(GetHitDamage() * 2);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_npc_skulker_shot_SpellScript::HandleHitTarget, SpellEffIndex::EFFECT_1, SpellEffects::SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_npc_skulker_shot_SpellScript();
        }
};

/// Last Update 7.1.5 Build 23420
/// Wind Spirit
/// 195650 - Crosswinds (Summoned by Crosswinds - 195653)
class spell_npc_wind_spirit : public CreatureScript
{
    public:
        spell_npc_wind_spirit() : CreatureScript("spell_npc_wind_spirit") { }

        struct spell_npc_wind_spirit_AI : public ScriptedAI
        {
            spell_npc_wind_spirit_AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                CloneCaster         = 60352,
                FlyingKick          = 196052,
                DamagingSpell       = 196061,
                WindSpiritVisual    = 196051
            };

            uint64 m_TargetGUID = 0;
            bool m_DespawnScheduled = false;

            void Reset() override
            {
                TempSummon* l_TempSummon = me->ToTempSummon();
                if (!l_TempSummon)
                    return;

                Unit* l_Owner = l_TempSummon->GetSummoner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = l_Player->GetSelectedUnit();

                std::set<uint64>& l_AvailableTargets = l_Player->m_SpellHelper.GetUint64Set()[eSpellHelpers::CrosswindsAvailableTargets];

                if (!l_Target || (l_AvailableTargets.find(l_Target->GetGUID()) == l_AvailableTargets.end()) || !l_Player->IsValidAttackTarget(l_Target))
                {
                    uint64 l_TargetGUID = l_Player->m_SpellHelper.GetUint64(eSpellHelpers::Crosswinds);
                    l_Target = sObjectAccessor->GetUnit(*l_Player, l_TargetGUID);

                    if (!l_Target || !me->isInFront(l_Target))
                        return;
                }

                l_Player->m_SpellHelper.GetUint64(eSpellHelpers::Crosswinds) = l_Target->GetGUID();

                m_TargetGUID = l_Target->GetGUID();
                l_Owner->CastSpell(me, eSpells::CloneCaster, true);
                me->CastSpell(me, eSpells::WindSpiritVisual, true);
                me->CastSpell(l_Target, eSpells::FlyingKick, false);
            }

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);
                if (!l_Target)
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (me->GetDistance(l_Target) < 1.0f && !m_DespawnScheduled)
                {
                    m_DespawnScheduled = true;

                    me->CastSpell(l_Target, eSpells::DamagingSpell, true);
                    AddTimedDelayedOperation(600, [this]() -> void
                    {
                        me->RemoveAura(eSpells::CloneCaster);
                        me->RemoveAura(eSpells::WindSpiritVisual);
                        me->DespawnOrUnsummon(100);
                    });
                }
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_wind_spirit_AI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Chi-Ji, the Red Crane - 100868
class spell_npc_chi_ji : public CreatureScript
{
    public:
        spell_npc_chi_ji() : CreatureScript("spell_npc_chi_ji") { }

        struct spell_npc_chi_ji_AI : public ScriptedAI
        {
            spell_npc_chi_ji_AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                CraneHeal = 198764,
                CraneHealTarget = 198766
            };

#define     CHI_JI_NB_TARGETS   3

            uint32 m_TargetCooldown;
            uint32 m_HealCooldown;
            uint64 m_SelectedTargets[CHI_JI_NB_TARGETS] = { 0 };
            int32 m_CurrentTarget;
            int32 m_NbTargets;
            float m_TargetRadius;

            void UpdateCooldown(uint32& p_Cooldown, uint32 const p_Diff)
            {
                if (p_Cooldown > 0 && p_Cooldown > p_Diff)
                    p_Cooldown -= p_Diff;
                else if (p_Cooldown > 0 && p_Diff > p_Cooldown)
                    p_Cooldown = 0;
            }

            void Reset() override
            {
                if (auto owner = me->GetAnyOwner())
                {
                    if (auto plr = owner->ToPlayer())
                    {
                        uint8 slot = 0;
                        for (int32 i = SUMMON_SLOT_TOTEM; i <= MAX_TOTEM_SLOT; ++i)
                        {
                            if (!owner->m_SummonSlot[i])
                            {
                                slot = i;
                                break;
                            }
                        }
                        if (slot)
                        {
                            if (plr->m_SummonSlot[SUMMON_SLOT_PET] == me->GetGUID())
                                plr->m_SummonSlot[SUMMON_SLOT_PET] = 0;
                            plr->m_SummonSlot[slot] = me->GetGUID();
                        }
                    }
                }
                m_TargetCooldown = 0;
                m_HealCooldown = 0;
                m_CurrentTarget = 0;
                m_NbTargets = 0;
                me->m_SpellHelper.GetUint64(eSpellHelpers::CraneHealLastTarget) = 0;

                SpellInfo const* l_CraneHealInfo = sSpellMgr->GetSpellInfo(eSpells::CraneHealTarget);

                m_TargetRadius = (l_CraneHealInfo && l_CraneHealInfo->Effects[0].RadiusEntry) ? l_CraneHealInfo->Effects[0].RadiusEntry->RadiusMax : 0;
                me->SetReactState(ReactStates::REACT_PASSIVE);
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                UpdateCooldown(m_TargetCooldown, p_Diff);
                UpdateCooldown(m_HealCooldown, p_Diff);

                if (!m_TargetCooldown && m_CurrentTarget < m_NbTargets)
                {
                    if (!me->GetOwner())
                        return;

                    me->m_SpellHelper.GetUint64(eSpellHelpers::CraneHealLastTarget) = m_SelectedTargets[m_CurrentTarget];
                    me->m_SpellHelper.GetBool(eSpellHelpers::CraneHealLastTarget) = true;
                    me->CastSpell(me, eSpells::CraneHeal, false);
                    me->m_SpellHelper.GetBool(eSpellHelpers::CraneHealLastTarget) = false;
                    m_TargetCooldown = static_cast<uint32>(0.5 * TimeConstants::IN_MILLISECONDS);
                    m_HealCooldown = static_cast<uint32>((2 * std::max<float>(me->GetOwner()->GetFloatValue(UNIT_FIELD_MOD_CASTING_SPEED), 0.5f)) * TimeConstants::IN_MILLISECONDS);
                    m_CurrentTarget++;
                }
                else if (m_CurrentTarget >= m_NbTargets && !m_HealCooldown)
                {
                    m_CurrentTarget = 0;
                    SelectTargets();
                }
            }

            void SelectTargets()
            {
                std::list<Unit*> l_Targets;
                JadeCore::AnyGroupedUnitInObjectRangeCheck l_Check(me, me, 40.0f, true);
                JadeCore::UnitListSearcher<JadeCore::AnyGroupedUnitInObjectRangeCheck> l_Searcher(me, l_Targets, l_Check);
                me->VisitNearbyObject(m_TargetRadius, l_Searcher);
                uint64 l_PrevTargets[CHI_JI_NB_TARGETS] = { 0 };

                l_Targets.sort(JadeCore::HealthPctOrderPredPlayer());
                memcpy(l_PrevTargets, m_SelectedTargets, m_NbTargets * sizeof(*l_PrevTargets));
                m_NbTargets = 0;
                for (auto l_Target : l_Targets)     ///< Select 3 new targets
                {
                    if (l_Target != me)
                    {
                        if (!TargetAlreadySelected(l_Target->GetGUID(), l_PrevTargets))
                        {
                            m_SelectedTargets[m_NbTargets] = l_Target->GetGUID();
                            ++m_NbTargets;
                        }
                    }
                    if (m_NbTargets == 3)
                        break;
                }

                for (int i = 0; i < CHI_JI_NB_TARGETS && m_NbTargets < CHI_JI_NB_TARGETS; ++i)      ///< If didn't find enough new targets then select previously selected targets
                {
                    Unit* l_Target = Unit::GetUnit(*me, l_PrevTargets[i]);

                    if (!me->GetOwner())
                        continue;

                    if (!l_Target || !l_Target->IsInRange(me->GetOwner(), 0, m_TargetRadius))
                        continue;

                    m_SelectedTargets[m_NbTargets] = l_PrevTargets[i];
                    ++m_NbTargets;
                }
            }

            bool TargetAlreadySelected(uint64 p_TargetGUID, uint64 p_Targets[CHI_JI_NB_TARGETS])
            {
                for (int i = 0; i < CHI_JI_NB_TARGETS; ++i)
                {
                    if (p_Targets[i] == p_TargetGUID)
                        return true;
                }
                return false;
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_chi_ji_AI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Sphere of Insanity - 98680
class spell_npc_sphere_of_insanity : public CreatureScript
{
public:
    spell_npc_sphere_of_insanity() : CreatureScript("spell_npc_sphere_of_insanity") { }

    struct spell_npc_sphere_of_insanity_AI : public ScriptedAI
    {
        spell_npc_sphere_of_insanity_AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

        void Reset() override
        {
            me->SetReactState(ReactStates::REACT_PASSIVE);
        }
    };

    CreatureAI* GetAI(Creature* p_Creature) const override
    {
        return new spell_npc_sphere_of_insanity_AI(p_Creature);
    }
};

/// Last Update 7.3.5 Build 26365
/// Tidal Totem - 105422
class spell_npc_tidal_totem : public CreatureScript
{
    public:
        spell_npc_tidal_totem() : CreatureScript("spell_npc_tidal_totem") { }

        struct spell_npc_tidal_totemAI : public TotemAI
        {
            spell_npc_tidal_totemAI(Creature* p_Creature) : TotemAI(p_Creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            enum eSpell
            {
                SpellTidalTotemHeal = 209069
            };

            EventMap m_Events;
            uint8 m_CastCount = 7;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                p_Summoner->CastSpell(me, eSpell::SpellTidalTotemHeal, true);

                --m_CastCount;

                m_Events.ScheduleEvent(EVENT_1, 1000);
            }

            void JustDespawned() override
            {
                ///< There is a problem with a creature model after despawning, so make the totem invisible
                me->SetDisplayId(11686);
                me->ForceSendUpdate();

                /// Last tick before despawn if needed.
                if (!m_CastCount)
                    return;

                if (Unit* l_Owner = me->GetAnyOwner())
                    l_Owner->CastSpell(me, eSpell::SpellTidalTotemHeal, true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_Events.Update(p_Diff);

                if (m_Events.ExecuteEvent() == EVENT_1 && m_CastCount > 0)
                {
                    if (Unit* l_Owner = me->GetAnyOwner())
                        l_Owner->CastSpell(me, eSpell::SpellTidalTotemHeal, true);

                    --m_CastCount;

                    if (m_CastCount > 0)
                        m_Events.ScheduleEvent(EVENT_1, 1000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_tidal_totemAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Akaari's Soul - 105850 - 105849
class spell_npc_akaari_soul : public CreatureScript
{
    public:
        spell_npc_akaari_soul() : CreatureScript("spell_npc_akaari_soul") { }

        struct spell_npc_akaari_soulAI : public ScriptedAI
        {
            spell_npc_akaari_soulAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                SoulRip = 220893
            };

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                Unit* l_Caster = me->GetOwner();
                if (!l_Caster)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                std::map<uint64, uint64>& l_Helper = l_Caster->m_SpellHelper.GetUint64Map()[eSpellHelpers::AkaariSoul];

                uint64 l_OldestID = static_cast<uint64>(-1);
                for (std::map<uint64, uint64>::const_iterator l_Itr = l_Helper.begin(); l_Itr != l_Helper.end(); ++l_Itr)
                {
                    if (l_Itr->first < l_OldestID)
                        l_OldestID = l_Itr->first;
                }

                if (l_OldestID == static_cast<uint64>(-1))
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                Unit* l_Victim = Unit::GetUnit(*l_Caster, l_Helper[l_OldestID]);
                l_Helper.erase(l_OldestID);
                if (!l_Victim)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (l_Caster->IsValidAttackTarget(l_Victim))
                {
                    l_Caster->CastSpell(l_Victim, eSpells::SoulRip, true);
                    me->CastSpell(l_Victim, eSpells::SoulRip, true, nullptr, nullptr, l_Caster->GetGUID());
                }

                me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_akaari_soulAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Void Tentacle - 98167
class spell_npc_void_tentacle : public CreatureScript
{
    public:
        spell_npc_void_tentacle() : CreatureScript("spell_npc_void_tentacle") { }

        struct spell_npc_void_tentacleAI : public ScriptedAI
        {
            spell_npc_void_tentacleAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                MindFlay = 193473
            };

            void Reset() override
            {
                me->AddUnitState(UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (!l_Owner->ToPlayer())
                    return;

                uint64 l_TargetGUID = me->GetTargetGUID();
                Unit* l_Target = sObjectAccessor->FindUnit(l_TargetGUID);
                if (!l_Target || !l_Target->isInCombat() || !l_Target->isAlive())
                    l_Target = l_Owner->getVictim();

                if (!l_Target || !l_Target->isInCombat() || !l_Target->isAlive())
                    if (l_Owner->ToPlayer()->GetSelectedUnit() && l_Owner->IsValidAttackTarget(l_Owner->ToPlayer()->GetSelectedUnit()))
                        l_Target = l_Owner->ToPlayer()->GetSelectedUnit();

                if (!l_Target || !l_Target->isInCombat() || !l_Target->isAlive())
                    return;

                me->CastSpell(l_Target, eSpells::MindFlay, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_void_tentacleAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Mindbender - 62982 for Minbender - 200174
class spel_npc_pri_mindbender : public CreatureScript
{
    public:
        spel_npc_pri_mindbender() : CreatureScript("spel_npc_pri_mindbender") { }

        struct spel_npc_pri_mindbenderAI : public ScriptedAI
        {
            spel_npc_pri_mindbenderAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                AtonementPassive    = 81749,
                AtonementHeal       = 81751,
                AtonementHealCrit   = 94472,
                PowerWordBarrier    = 81782,
                BarrierOfTheDevoted = 197815,
                ArtificialDamage    = 219655,
                Mindbender          = 200174,
                ShadowCrawl         = 63619
            };

            enum eEvents
            {
                EventShadowCrawl = 1
            };

            uint64 m_TargetGUID = 0;

            void Reset() override
            {
                events.ScheduleEvent(eEvents::EventShadowCrawl, 3000);
                uint32 l_AttackTime = 1500;
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (Player* l_Player = l_Owner->ToPlayer())
                    l_AttackTime *= l_Player->GetFloatValue(UNIT_FIELD_MOD_HASTE);

                me->SetAttackTime(WeaponAttackType::BaseAttack, l_AttackTime);
                me->SetAttackTime(WeaponAttackType::OffAttack, l_AttackTime);
            }

            void EnterEvadeMode() override { }

            void OnMeleeAttackDone(Unit* p_Target, DamageInfo* p_DamageInfo, WeaponAttackType p_AttackType) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                m_TargetGUID = p_Target->GetGUID();

                if (l_Owner->ToPlayer() && (l_Owner->ToPlayer()->GetActiveSpecializationID() == SPEC_PRIEST_SHADOW) && me->GetEntry() != 19668 && me->GetEntry() != 67235)
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::Mindbender))
                        l_Owner->ModifyPower(Powers::POWER_INSANITY, l_SpellInfo->Effects[EFFECT_2].BasePoints * l_Owner->GetPowerCoeff(Powers::POWER_INSANITY));
                }

                AuraEffect const* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::AtonementPassive, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                float l_Pct = l_AuraEffect->GetAmount();
                int32 l_Damage = p_DamageInfo->GetAmount();
                if (l_Damage == 0)
                    l_Damage = p_DamageInfo->GetAbsorb();

                double l_Resist = 0;
                auto const& l_ResistanceEffect = p_Target->GetAuraEffectsByType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
                for (AuraEffect* l_Resistance : l_ResistanceEffect)
                {
                    l_Resist = -l_Resistance->GetAmount();
                    if (l_Resist != 0)
                    {
                        double l_Percentage = (1 - (l_Resist / 100));
                        l_Percentage *= 100;
                        l_Damage = l_Damage * (100 / l_Percentage);
                    }
                }

                l_Damage = CalculatePct(l_Damage, l_Pct);

                std::set<uint64> l_TargetGUIDs = l_Owner->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
                {
                    Unit* l_Target = ObjectAccessor::GetUnit(*l_Owner, *l_Itr);

                    if (!l_Target)
                        continue;

                    l_Owner->CastCustomSpell(l_Target, eSpells::AtonementHeal, &l_Damage, NULL, NULL, true);
                }
            }

            void OnCalculateMeleeDamage(Unit* p_Victim, uint32* p_Damage, DamageInfo* p_DamageInfo) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player || me->GetEntry() == 19668)
                    return;

                switch (l_Player->GetActiveSpecializationID())
                {
                    case SPEC_PRIEST_DISCIPLINE:
                    {
                        if (!p_DamageInfo || !p_DamageInfo->GetSpell())
                            break;

                        l_Owner->ModifyPower(Powers::POWER_MANA, CalculatePct(l_Owner->GetMaxPower(POWER_MANA), 0.5f));

                        if (AuraEffect* l_ArtificialDamage = l_Owner->GetAuraEffect(eSpells::ArtificialDamage, EFFECT_0))
                            AddPct(*p_Damage, l_ArtificialDamage->GetAmount());

                        AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::AtonementPassive, EFFECT_0);
                        if (!l_AuraEffect)
                            return;

                        float l_Pct = l_AuraEffect->GetAmount();
                        int32 l_Damage = static_cast<int32>(*p_Damage);
                        l_Damage = CalculatePct(l_Damage, l_Pct);

                        if (AuraEffect *l_BarrierOfTheDevoted = l_Owner->GetAuraEffect(eSpells::BarrierOfTheDevoted, EFFECT_0))
                        {
                            if (l_Owner->HasAura(eSpells::PowerWordBarrier))
                                l_Damage = AddPct(l_Damage, l_BarrierOfTheDevoted->GetAmount());
                        }

                        std::set<uint64> l_TargetGUIDs = l_Owner->m_SpellHelper.GetUint64Set()[eSpellHelpers::AtonementTargets];
                        for (auto l_Itr = l_TargetGUIDs.begin(); l_Itr != l_TargetGUIDs.end(); ++l_Itr)
                        {
                            Unit* l_Target = ObjectAccessor::GetUnit(*l_Owner, *l_Itr);

                            if (!l_Target)
                                continue;

                            l_Owner->CastCustomSpell(l_Target, p_DamageInfo->GetSpell()->IsCritForTarget(p_Victim) ? eSpells::AtonementHealCrit : eSpells::AtonementHeal, &l_Damage, NULL, NULL, true);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    if (Player* l_Player = l_Owner->ToPlayer())
                    {
                        uint32 l_AttackTime = 1500 * l_Player->GetFloatValue(UNIT_FIELD_MOD_HASTE);

                        me->SetAttackTime(WeaponAttackType::BaseAttack, l_AttackTime);
                        me->SetAttackTime(WeaponAttackType::OffAttack, l_AttackTime);
                    }
                }

                ScriptedAI::UpdateAI(p_Diff);

                events.Update(p_Diff);
                if (events.ExecuteEvent() == eEvents::EventShadowCrawl)
                {
                    if (Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID))
                        me->CastSpell(l_Target, eSpells::ShadowCrawl, false);

                    events.ScheduleEvent(eEvents::EventShadowCrawl, 6000);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spel_npc_pri_mindbenderAI(creature);
        }
};


/// Last Update 7.1.5 Build 23420
/// Guardian of the Forgotten Queen - 114565
class spell_npc_guardian_of_the_forgotten_queen : public CreatureScript
{
    public:
        spell_npc_guardian_of_the_forgotten_queen() : CreatureScript("spell_npc_guardian_of_the_forgotten_queen") { }

        struct spell_npc_guardian_of_the_forgotten_queen_AI : public ScriptedAI
        {
            spell_npc_guardian_of_the_forgotten_queen_AI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                DivineShield = 228050
            };

            uint64 m_TargetGUID = 0;

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                AddTimedDelayedOperation(10, [=]() -> void
                {
                    SetFlyMode(true);

                    Position l_Pos = me->GetPosition();
                    l_Pos.m_positionZ += 5.0f;

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveTakeoff(0, l_Pos);
                });
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner || !m_TargetGUID)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID);
                if (!l_Target)
                    return;

                if (!l_Target->HasAura(eSpells::DivineShield))
                    me->CastSpell(l_Target, eSpells::DivineShield, false);
            }

            void SetGUID(uint64 p_Guid, int32 /*p_Index*/) override
            {
                m_TargetGUID = p_Guid;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_guardian_of_the_forgotten_queen_AI(p_Creature);
        }
};

/// Last Update 7.3.5 build 26365
/// Lightning Elemental - 97022
class spell_npc_lightning_elemental : public CreatureScript
{
    public:
        spell_npc_lightning_elemental() : CreatureScript("spell_npc_lightning_elemental") { }

        struct spell_npc_lightning_elemental_ScriptedAI : public ScriptedAI
        {
            spell_npc_lightning_elemental_ScriptedAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_Elemental = me->ToUnit();
                if (!l_Elemental)
                    return;

                if (l_Elemental->m_SpellHelper.GetUint32(eSpellHelpers::LightningElementalSpell) == 0)
                    return;

                if (l_Elemental->m_SpellHelper.GetUint64(eSpellHelpers::LightningElementalTarget) == 0)
                    return;

                Unit* l_Target = sObjectAccessor->GetUnit(*me, l_Elemental->m_SpellHelper.GetUint64(eSpellHelpers::LightningElementalTarget));

                if (!l_Target)
                    l_Target = l_Owner->getVictim();

                if (!l_Target)
                    return;

                me->CastSpell(l_Target, l_Elemental->m_SpellHelper.GetUint32(eSpellHelpers::LightningElementalSpell), false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_lightning_elemental_ScriptedAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Mirror image - 31216
/// Don't know why don't we use npc_mirror_image
class spell_npc_mirror_image : public CreatureScript
{
    public:
        spell_npc_mirror_image() : CreatureScript("spell_npc_mirror_image") { }

        struct spell_npc_mirror_image_AI : public ScriptedAI
        {
            spell_npc_mirror_image_AI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                MirrorImage     = 60352,
                FrostBolt       = 59638,
                Fireball        = 88082,
                ArcaneBlast     = 88084
            };

            void Reset() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                l_Owner->CastSpell(me, eSpells::MirrorImage, true);

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                switch (l_Player->GetActiveSpecializationID())
                {
                    case SPEC_MAGE_FROST:
                        m_Spell = eSpells::FrostBolt;
                        break;
                    case SPEC_MAGE_FIRE:
                        m_Spell = eSpells::Fireball;
                        break;
                    case SPEC_MAGE_ARCANE:
                        m_Spell = eSpells::ArcaneBlast;
                        break;
                }
            }

            bool CanAIAttack(Unit const* l_Target) const override
            {
                return l_Target && !l_Target->HasBreakableByDamageCrowdControlAura();
            }

            void UpdateAI(uint32 const /*p_Diff*/) override
            {
                me->AddUnitState(UNIT_STATE_FOLLOW);
                me->SetReactState(REACT_HELPER);
                me->UpdateAllStats();

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                {
                    if (Spell const* l_Spell = me->GetCurrentSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL))
                    {
                        if (Unit* l_Target = l_Spell->GetUnitTarget())
                            if (!CanAIAttack(l_Target))
                                me->InterruptNonMeleeSpells(false);
                    }
                    return;
                }

                Unit* l_Victim = l_Owner->getVictim() ? l_Owner->getVictim() : me->getAttackerForHelper();

                if (!l_Victim)
                    if (Player* l_Player = l_Owner->ToPlayer())
                        l_Victim = l_Player->GetSelectedUnit();

                if (l_Victim && me->isTargetableForAttack(l_Victim) && (!me->getVictim() || me->getVictim() != l_Victim))
                    me->CastSpell(l_Victim, m_Spell, false, nullptr, nullptr, l_Owner->GetGUID());
            }

        private:

            uint32 m_Spell = eSpells::FrostBolt;

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_mirror_image_AI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Dire Basilisk - 105419
class spell_npc_hun_dire_basilisk : public CreatureScript
{
    public:
        spell_npc_hun_dire_basilisk() : CreatureScript("spell_npc_hun_dire_basilisk") { }

        struct spell_npc_hun_dire_basiliskAI : public ScriptedAI
        {
            spell_npc_hun_dire_basiliskAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                BasiliskDebuff        = 209967,
            };

            void Reset() override
            {
                me->SetSpeed(UnitMoveType::MOVE_RUN, 0.2f);

                for (uint8 l_AttType = 0; l_AttType < WeaponAttackType::MaxAttack; ++l_AttType)
                {
                    me->SetBaseWeaponDamage((WeaponAttackType)l_AttType, MAXDAMAGE, me->GetBaseWeaponDamageRange((WeaponAttackType)l_AttType, MAXDAMAGE) * 6.f);
                    me->SetBaseWeaponDamage((WeaponAttackType)l_AttType, MINDAMAGE, me->GetBaseWeaponDamageRange((WeaponAttackType)l_AttType, MINDAMAGE) * 6.f);
                }

                me->UpdateAttackPowerAndDamage();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Target = me->getVictim();
                if (!l_Target)
                    return;

                if (!l_Target->HasAura(eSpells::BasiliskDebuff))
                    me->CastSpell(l_Target, eSpells::BasiliskDebuff, false);

                ScriptedAI::UpdateAI(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_hun_dire_basiliskAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Soul Effigy 103679
class spell_npc_warl_soul_effigy : public CreatureScript
{
    public:
        spell_npc_warl_soul_effigy() : CreatureScript("spell_npc_warl_soul_effigy") { }

        struct spell_npc_warl_soul_effigy_AI : public ScriptedAI
        {
            spell_npc_warl_soul_effigy_AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SoulEffigyDamage    = 205260,
                SoulEffigyAura      = 205247,
                SoulEffigyEffect    = 205178,
                SoulEffigyVisual    = 205277
            };

            enum eDatas
            {
                NEUTRAL_FACTION     = 7,
                HOSTIL_FACTION      = 14,
            };

            uint64 m_CasterGUID = 0;
            uint64 m_VictimGUID = 0;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_CasterGUID = p_Summoner->GetGUID();
                Unit* l_Caster = sObjectAccessor->GetUnit(*me, m_CasterGUID);
                if (!l_Caster)
                    return;

                m_VictimGUID = l_Caster->GetTargetGUID();
                ApplyAllImmunities(true);
                me->setFaction(eDatas::HOSTIL_FACTION);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const /* p_Diff */) override
            {
                Unit* l_Target = sObjectAccessor->GetUnit(*me, m_VictimGUID);
                Unit* l_Caster = sObjectAccessor->GetUnit(*me, m_CasterGUID);
                if (!l_Target || !l_Caster || !l_Target->HasAura(eSpells::SoulEffigyEffect))
                {
                    me->DisappearAndDie();
                    return;
                }

                me->SetHealthScal(l_Target->GetHealth(l_Caster));
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::SoulEffigyAura);
                Unit* l_Caster = sObjectAccessor->GetUnit(*me, m_CasterGUID);
                Unit* l_Target = sObjectAccessor->GetUnit(*me, m_VictimGUID);
                if (!l_SpellInfo || !l_Caster || !l_Target || p_Attacker != l_Caster)
                {
                    p_Damage = 0;
                    return;
                }

                int32 l_BasePoint = CalculatePct(p_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                p_Damage = l_BasePoint;
                me->CastSpell(l_Target, eSpells::SoulEffigyVisual, true);
                me->CastCustomSpell(l_Target, eSpells::SoulEffigyDamage, &l_BasePoint, nullptr, nullptr, true, nullptr, nullptr, m_CasterGUID);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const
        {
            return new spell_npc_warl_soul_effigy_AI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Shadowy Reflection - 111748 (Summoned by Shadowy Reflection - 222481)
/// Summoned For Item : Phantasmal Echo - 138225
class spell_npc_shadowy_reflection : public CreatureScript
{
    public:
        spell_npc_shadowy_reflection() : CreatureScript("spell_npc_shadowy_reflection") { }

        struct spell_npc_shadowy_reflectionAI : public ScriptedAI
        {
            spell_npc_shadowy_reflectionAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                CloneCaster         = 222485,
                ShadowyReflection   = 222478
            };

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);

                TempSummon* l_TempSummon = me->ToTempSummon();
                if (!l_TempSummon)
                    return;

                Unit* l_Owner = l_TempSummon->GetSummoner();
                if (!l_Owner)
                    return;

                l_Owner->CastSpell(me, eSpells::CloneCaster, true);
                me->GetMotionMaster()->MoveFollow(l_Owner, 1.0f, 2.0f);

                AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::ShadowyReflection, EFFECT_2);
                if (!l_AuraEffect)
                    return;

                me->SetMaxHealth(l_AuraEffect->GetAmount());
                me->SetHealth(l_AuraEffect->GetAmount());
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                ScriptedAI::UpdateAI(p_Diff);
            }

            void SetData(uint32 /*p_ID*/, uint32 p_Value) override
            {
                me->ModifyHealth(-static_cast<int64>(p_Value));
            }

        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_shadowy_reflectionAI(p_Creature);
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
            MeatHookGrab = 221599
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
                me->SetTarget(l_Target->GetGUID());
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
/// Summon li Li  Stormstout (106770)
class spell_npc_li_li_stormstout : public CreatureScript
{
    public:
        spell_npc_li_li_stormstout() : CreatureScript("spell_npc_li_li_stormstout") { }

        struct spell_npc_li_li_stormstoutAI : public ScriptedAI
        {
            spell_npc_li_li_stormstoutAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SummonCloudSerpent  = 197353,
                SerpentBreath       = 211879
            };

            void InitializeAI() override
            {
                me->CastSpell(me, eSpells::SummonCloudSerpent, true);
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                for (Unit* l_Pet : me->m_Controlled)
                    l_Pet->CastSpell(l_Owner, eSpells::SerpentBreath, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_li_li_stormstoutAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Altruis the Sufferer (111938)
class spell_npc_altruis_the_sufferer : public CreatureScript
{
    public:
        spell_npc_altruis_the_sufferer() : CreatureScript("spell_npc_altruis_the_sufferer") { }

        struct spell_npc_altruis_the_suffererAI : public ScriptedAI
        {
            spell_npc_altruis_the_suffererAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                BladeDance  = 222938,
            };

            void InitializeAI() override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_Target = sObjectAccessor->GetCreature(*l_Owner, l_Owner->GetTargetGUID());
                if (!l_Target)
                    return;

                me->CastSpell(l_Target, eSpells::BladeDance, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_altruis_the_suffererAI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Windfury Totem - 105690
class spell_npc_windfury_totem : public CreatureScript
{
    public:
        spell_npc_windfury_totem() : CreatureScript("spell_npc_windfury_totem") { }

        struct spell_npc_windfury_totemAI : public ScriptedAI
        {
            spell_npc_windfury_totemAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Areatrigger         = 209386,
                WindfuryTotem       = 204332
            };

            void InitializeAI() override
            {
                Unit* l_Caster = me->GetOwner();
                if (!l_Caster)
                    return;

                me->SetMaxHealth((l_Caster->GetMaxHealth(l_Caster) * 2) / 100);
                me->IsAIEnabled = false;
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_REMOVE_CLIENT_CONTROL);
                me->CastSpell(me, eSpells::Areatrigger, true);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    if (Player* l_Player = l_Owner->ToPlayer())
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WindfuryTotem))
                        {
                            l_Player->RemoveSpellCooldown(eSpells::WindfuryTotem, true);
                            l_Player->AddSpellCooldown(eSpells::WindfuryTotem, 0, l_SpellInfo->RecoveryTime, true);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_windfury_totemAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Felhunter - 86723 (Summoned by Summon Felhunter - 173552, Summoned for Call Felhunter - 212619)
class spell_npc_felhunter : public CreatureScript
{
    public:
        spell_npc_felhunter() : CreatureScript("spell_npc_felhunter") { }

        struct spell_npc_felhunter_AI : public ScriptedAI
        {
            spell_npc_felhunter_AI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                SpellLock = 19647
            };

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_ROOT);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                uint64 l_TargetGUID = me->GetTargetGUID();

                Unit* l_Target = sObjectAccessor->FindUnit(l_TargetGUID);
                if (!l_Target)
                {
                    Unit* l_Owner = me->GetOwner();
                    if (!l_Owner)
                        return;

                    l_Target = l_Owner->getVictim();
                    if (!l_Target)
                    {
                        me->DespawnOrUnsummon(500);
                        return;
                    }
                }

                me->CastSpell(l_Target, eSpells::SpellLock, false);
                me->DespawnOrUnsummon(500);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_felhunter_AI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Matron Mother Malevolence - 112056
class spell_npc_matron_mother_malevolence : public CreatureScript
{
    public:
        spell_npc_matron_mother_malevolence() : CreatureScript("spell_npc_matron_mother_malevolence") { }

        struct spell_npc_matron_mother_malevolenceAI : public ScriptedAI
        {
            spell_npc_matron_mother_malevolenceAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            enum eSpells
            {
                Whirlwind = 223152
            };

            void UpdateAI(uint32 const p_Diff) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                me->SetUInt32Value(UNIT_FIELD_ATTACK_POWER, l_Owner->GetInt32Value(UNIT_FIELD_ATTACK_POWER));
                me->SetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, l_Owner->GetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER));
                me->CastSpell(me, eSpells::Whirlwind, false);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_matron_mother_malevolenceAI(p_Creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Spitting Cobra - 194407
/// Spitting Cobra - 104493
class spell_npc_spitting_cobra : public CreatureScript
{
    public:
        spell_npc_spitting_cobra() : CreatureScript("spell_npc_spitting_cobra") { }

        struct spell_npc_spitting_cobra_AI : public ScriptedAI
        {
            spell_npc_spitting_cobra_AI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                CobraSpit = 206685
            };

            void Reset() override
            {
                me->SetReactState(REACT_HELPER);
            }

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                Unit* l_Target = me->getVictim();

                if (Unit* l_Owner = me->GetOwner())
                {
                    if (l_Owner->ToPlayer())
                    {
                        Unit* l_NewTarget = nullptr;
                        GuardianPetTarget l_GuardianPetTarget;
                        l_GuardianPetTarget.UpdateTarget(me, l_Owner->ToPlayer(), l_NewTarget, true);

                        if (l_NewTarget && l_NewTarget != l_Target)
                            AttackStart(l_NewTarget);

                        l_Target = l_NewTarget;
                    }
                }

                if (!l_Target)
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::CobraSpit))
                {
                    if (me->IsWithinCombatRange(l_Target, l_SpellInfo->GetMaxRange(false)))
                        me->CastSpell(l_Target, l_SpellInfo, TRIGGERED_NONE);
                    else
                        DoMeleeAttackIfReady();
                }
                else
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_spitting_cobra_AI(p_Creature);
        }
};

/// Last Update 7.3.2 Build 25549
/// Cobra Commander - 121661
class spell_npc_hun_cobra_commander : public CreatureScript
{
    public:
        spell_npc_hun_cobra_commander() : CreatureScript("spell_npc_hun_cobra_commander") { }

        struct spell_npc_hun_cobra_commanderAI : public ScriptedAI
        {
            spell_npc_hun_cobra_commanderAI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                DeathstrikeVenom = 243120
            };

            void IsSummonedBy(Unit* p_Summoner) override
            {
                me->SetReactState(REACT_DEFENSIVE);

                if (!p_Summoner && !p_Summoner->IsPlayer())
                    return;

                float l_AP = p_Summoner->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
                /// Based on SimCraft
                float l_Damage = 0.4f * 1.25f * l_AP;
                for (uint8 l_AttType = 0; l_AttType < WeaponAttackType::MaxAttack; ++l_AttType)
                {
                    me->SetBaseWeaponDamage((WeaponAttackType)l_AttType, MINDAMAGE, l_Damage);
                    me->SetBaseWeaponDamage((WeaponAttackType)l_AttType, MAXDAMAGE, l_Damage);
                }

                DoCast(eSpells::DeathstrikeVenom);
            }

            void UpdateAI(const uint32 /*p_Diff*/) override
            {
                if (me->GetReactState() != REACT_DEFENSIVE)
                    me->SetReactState(REACT_DEFENSIVE);

                if (Unit* l_Owner = me->GetOwner())
                {
                    Unit* l_OwnerTarget = NULL;
                    if (Player* l_Player = l_Owner->ToPlayer())
                    {
                        if (!l_Player->isInCombat())
                        {
                            me->CombatStop();
                            return;
                        }
                        l_OwnerTarget = l_Player->GetSelectedUnit();
                    }
                    else
                        l_OwnerTarget = l_Owner->getVictim();

                    if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && !l_Owner->IsFriendlyTo(l_OwnerTarget) && me->IsValidAttackTarget(l_OwnerTarget))
                        AttackStart(l_OwnerTarget);
                }

                if (!UpdateVictim())
                    return;

                if (me->getVictim() && !me->IsValidAttackTarget(me->getVictim()))
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_hun_cobra_commanderAI(p_Creature);
        }
};

/// Last Update 7.3.2 Build 25549
/// Deathstrike Venom - 243121
class spell_npc_hun_deathstrikevenom : public SpellScriptLoader
{
public:
    spell_npc_hun_deathstrikevenom() : SpellScriptLoader("spell_npc_hun_deathstrikevenom") { }

    class spell_npc_hun_deathstrikevenom_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_npc_hun_deathstrikevenom_SpellScript);

        void ChangeDamage(SpellEffIndex/* p_SpellEffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            Player* l_Player = l_Owner->ToPlayer();
            if (!l_Player)
                return;

            int32 l_Damage = GetHitDamage();
            float l_AP = l_Player->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
            float l_Mastery = (l_Player->GetFloatValue(PLAYER_FIELD_MASTERY) * 2.25f / 100) + 1;
            l_Damage = 2 * 0.25f * 5.32 * l_AP * l_Mastery;
            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_npc_hun_deathstrikevenom_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_npc_hun_deathstrikevenom_SpellScript();
    }
};

/// Last Update 7.3.2 Build 25549
/// Called by Legion Strike - 30213 , ShadowBite - 54049 , Firebolt - 3110, Felbolt - 115746, Tongue Lash - 115778, Anguish - 170176, ConsumingShadows - 3716, Mortal Cleave - 115625
class spell_warl_pets_autocast_mechanic : public SpellScriptLoader
{
    public:
        spell_warl_pets_autocast_mechanic() : SpellScriptLoader("spell_warl_pets_autocast_mechanic") { }

        class spell_warl_pets_autocast_mechanic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_pets_autocast_mechanic_SpellScript);

            enum eSpells
            {
                LegionStrike     = 30213,
                MortalCleave     = 115625,

                ShadowBite       = 54049,
                TongueLash       = 115778,

                Firebolt         = 3110,
                Felbolt          = 115746,

                Anguish          = 170176,
                ConsumingShadows = 3716
            };

            SpellCastResult CheckConditions()
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return SPELL_FAILED_DONT_REPORT;

                Pet* l_Pet = l_Caster->ToPet();
                if (!l_Pet)
                    return SPELL_CAST_OK;

                uint32 l_PowerRequire = 0;
                switch (GetSpellInfo()->Id)
                {
                    case eSpells::LegionStrike:
                    case eSpells::MortalCleave:
                        l_PowerRequire = 100;
                        break;
                    case eSpells::ShadowBite:
                    case eSpells::TongueLash:
                        l_PowerRequire = 110;
                        break;
                    case eSpells::Firebolt:
                    case eSpells::Felbolt:
                        l_PowerRequire = 160;
                        break;
                    case eSpells::ConsumingShadows:
                        l_PowerRequire = 130;
                        break;
                    case eSpells::Anguish:
                        l_PowerRequire = 150;
                        break;
                }

                PetSpellMap::iterator l_Itr = l_Pet->m_spells.find(GetSpellInfo()->Id);
                if (l_Itr->second.active == ActiveStates::ACT_ENABLED && l_PowerRequire > 0)
                    if (l_Caster->GetPower(l_Caster->getPowerType()) < l_PowerRequire)
                        return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_warl_pets_autocast_mechanic_SpellScript::CheckConditions);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_pets_autocast_mechanic_SpellScript();
        }
};

/// Last Update 7.3.5 build 26365
class spell_npc_flame_rift : public CreatureScript
{
    public:
        spell_npc_flame_rift() : CreatureScript("npc_flame_rift") { }

        struct spell_npc_flame_riftAI : public ScriptedAI
        {
            uint64 m_TargetGUID;
            uint32 m_BoltTimer;

            spell_npc_flame_riftAI(Creature* creature) : ScriptedAI(creature)
            {
                m_TargetGUID = 0;
                m_BoltTimer = 0;
            }

            enum eSpells
            {
                SearingBolt             = 243050,
                FlameRiftDisplay        = 243045
            };

            void Reset() override
            {
                if (!me->HasAura(eSpells::FlameRiftDisplay))
                    me->CastSpell(me, eSpells::FlameRiftDisplay, true);

                if (m_TargetGUID)
                    return;

                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Player* l_Player = l_Owner->ToPlayer();
                if (!l_Player)
                    return;

                Unit* l_Target = l_Player->GetSelectedUnit();
                if (!l_Target || l_Player->IsFriendlyTo(l_Target))
                    return;

                m_TargetGUID = l_Target->GetGUID();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (Unit* l_Target = sObjectAccessor->GetUnit(*me, m_TargetGUID))
                {
                    if (l_Target->HasStealthAura() || l_Target->HasInvisibilityAura())
                    {
                        m_TargetGUID = 0;
                        return;
                    }

                    if (me->HasAura(eSpells::FlameRiftDisplay))
                    {
                        uint8 l_Stacks = 0;
                        Aura* l_BoltAura = l_Target->GetAura(eSpells::SearingBolt, me->GetGUID());

                        if (l_BoltAura)
                            l_Stacks = l_BoltAura->GetStackAmount();

                        if (l_Stacks < 20)
                        {
                            if (m_BoltTimer <= p_Diff)
                            {
                                if (l_Stacks % 2 == 0)
                                    if (l_BoltAura)
                                        if (AuraEffect* l_BoltEffect = l_BoltAura->GetEffect(EFFECT_1))
                                            l_BoltEffect->HandlePeriodicDamageAurasTick(l_Target, me);

                                me->CastSpell(l_Target, eSpells::SearingBolt, true);
                                m_BoltTimer = 500;
                            }
                            else
                                m_BoltTimer -= p_Diff;
                        }
                        else
                            me->RemoveAura(eSpells::FlameRiftDisplay);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new spell_npc_flame_riftAI(creature);
        }
};

/// Last Update 7.3.5 build 26365
/// Called Chaos Bolt 215279, Shadow Bolt 196657, Chaos Barrage 187394
class spell_npc_tears_destruction_modifier : public SpellScriptLoader
{
public:
    spell_npc_tears_destruction_modifier() : SpellScriptLoader("spell_npc_tears_destruction_modifier") { }

    enum eSpells
    {
        DestructionWarlockAura      = 137046,
        ChaosBolt                   = 215279
    };

    class spell_npc_tears_destruction_modifier_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_npc_tears_destruction_modifier_SpellScript);

        void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Target = GetHitUnit();
            if (!l_Target)
                return;

            if (l_Target->HasStealthAura() || l_Target->HasInvisibilityAura())
            {
                SetHitDamage(0);
                return;
            }

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            if (Player* l_OwnerPlayer = l_Owner->ToPlayer())
            {
                int32 l_Damage = GetHitDamage();

                if (l_Owner->HasAura(eSpells::DestructionWarlockAura))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DestructionWarlockAura))
                        AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                }

                ///< Chaos Bolt is reduced by 43.75% in PvP
                if (m_scriptSpellId == eSpells::ChaosBolt && l_Owner->CanApplyPvPSpellModifiers())
                    l_Damage *= 0.5625f;

                SetHitDamage(l_Damage);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_npc_tears_destruction_modifier_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_npc_tears_destruction_modifier_SpellScript();
    }
};

/// Last Update 7.3.5 build 26365
/// Called Searing Bolt 243050
class spell_npc_searing_bolt : public SpellScriptLoader
{
public:
    spell_npc_searing_bolt() : SpellScriptLoader("spell_npc_searing_bolt") { }

    enum eSpells
    {
        EmpoweredLifeTapBuff        = 235156,
        LessonsOfSpaceTime          = 236176,
        DestructionWarlockAura      = 137046
    };

    class spell_npc_searing_bolt_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_npc_searing_bolt_SpellScript);

        void HandleDamage(SpellEffIndex /*p_EffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            if (Player* l_OwnerPlayer = l_Owner->ToPlayer())
            {
                int32 l_Damage = l_OwnerPlayer->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL);

                if (SpellInfo const* l_EmpoweredLifeTapInfo = sSpellMgr->GetSpellInfo(eSpells::EmpoweredLifeTapBuff))
                    if (l_OwnerPlayer->HasAura(eSpells::EmpoweredLifeTapBuff))
                        AddPct(l_Damage, l_EmpoweredLifeTapInfo->Effects[EFFECT_0].BasePoints);

                if (SpellInfo const* l_LessonsOfSpaceTimeInfo = sSpellMgr->GetSpellInfo(eSpells::LessonsOfSpaceTime))
                    if (l_OwnerPlayer->HasAura(eSpells::LessonsOfSpaceTime))
                        AddPct(l_Damage, l_LessonsOfSpaceTimeInfo->Effects[EFFECT_0].BasePoints);

                if (l_Owner->HasAura(eSpells::DestructionWarlockAura))
                {
                    if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::DestructionWarlockAura))
                        AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);
                }

                if (l_OwnerPlayer->CanApplyPvPSpellModifiers())
                    l_Damage *= 0.79f;

                SetHitDamage(l_Damage);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_npc_searing_bolt_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    class spell_npc_searing_bolt_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_npc_searing_bolt_AuraScript);

        void CalculateAmount(AuraEffect const* p_AurEff, int32& p_Amount, bool& /*canBeRecalculated*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            if (Player* l_OwnerPlayer = l_Owner->ToPlayer())
            {
                AuraEffect* l_Effect = const_cast<AuraEffect*>(p_AurEff);
                Aura* l_BoltDebuff = p_AurEff->GetBase();

                if (!l_Effect || !l_BoltDebuff)
                    return;

                int32 l_Amount = CalculatePct(l_OwnerPlayer->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_ALL), 7);

                if (SpellInfo const* l_EmpoweredLifeTapInfo = sSpellMgr->GetSpellInfo(eSpells::EmpoweredLifeTapBuff))
                    if (l_OwnerPlayer->HasAura(eSpells::EmpoweredLifeTapBuff))
                        AddPct(l_Amount, l_EmpoweredLifeTapInfo->Effects[EFFECT_0].BasePoints);

                if (SpellInfo const* l_EmpoweredLifeTapInfo = sSpellMgr->GetSpellInfo(eSpells::LessonsOfSpaceTime))
                    if (l_OwnerPlayer->HasAura(eSpells::LessonsOfSpaceTime))
                        AddPct(l_Amount, l_EmpoweredLifeTapInfo->Effects[EFFECT_0].BasePoints);

                if (l_OwnerPlayer->CanApplyPvPSpellModifiers())
                    l_Amount *= 0.33f;

                p_Amount = l_Amount;
            }
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_npc_searing_bolt_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_npc_searing_bolt_SpellScript();
    }

    AuraScript* GetAuraScript() const
    {
        return new spell_npc_searing_bolt_AuraScript();
    }
};

/// Last Update 7.3.2
class spell_npc_echo_of_the_highlord : public CreatureScript
{
public:
    spell_npc_echo_of_the_highlord() : CreatureScript("npc_echo_of_the_highlord") { }

    struct spell_npc_echo_of_the_highlordAI : public ScriptedAI
    {
        spell_npc_echo_of_the_highlordAI(Creature* creature) : ScriptedAI(creature) { }

        enum eSpells
        {
            SearingBolt = 243050,
            FlameRiftDisplay = 243045
        };

        void InitializeAI() override
        {
            Unit* l_Owner = me->GetOwner();
            if (!l_Owner)
                return;

            Player* l_Player = l_Owner->ToPlayer();
            if (!l_Player)
                return;

            me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 0, l_Player->GetUInt32Value(PLAYER_FIELD_VISIBLE_ITEMS + (EQUIPMENT_SLOT_MAINHAND * 2) + 0));
            me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 2, l_Player->GetUInt16Value(PLAYER_FIELD_VISIBLE_ITEMS + (EQUIPMENT_SLOT_MAINHAND * 2) + 1, 0));
            me->SetUInt32Value(EUnitFields::UNIT_FIELD_VIRTUAL_ITEMS + 4, l_Player->GetUInt16Value(PLAYER_FIELD_VISIBLE_ITEMS + (EQUIPMENT_SLOT_MAINHAND * 2) + 1, 1));
        }

        void UpdateAI(uint32 const p_Diff) override { }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new spell_npc_echo_of_the_highlordAI(creature);
    }
};

/// Last update : 7.3.5 build 25996
/// Wrathstorm - 115831, Felstorm - 89751
class spell_npc_wrathguard_wrathstorm : public SpellScriptLoader
{
    public:
        spell_npc_wrathguard_wrathstorm() : SpellScriptLoader("spell_npc_wrathguard_wrathstorm") { }

        class spell_npc_wrathguard_wrathstorm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_npc_wrathguard_wrathstorm_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();

                if (!l_Caster)
                    return;

                l_Caster->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
            }

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                l_Caster->ClearUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_npc_wrathguard_wrathstorm_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_npc_wrathguard_wrathstorm_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_npc_wrathguard_wrathstorm_AuraScript();
        }
};

/// Last Update 7.3.5 Build 25996
class spell_npc_eldritch_grimoire : public CreatureScript
{
public:
    spell_npc_eldritch_grimoire() : CreatureScript("npc_eldritch_grimoire") { }

    struct spell_npc_eldritch_grimoireAI : public ScriptedAI
    {
        spell_npc_eldritch_grimoireAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            Position l_Pos = me->GetPosition();
            l_Pos.m_positionZ += 1.f;
            me->NearTeleportTo(l_Pos);
        }

        void sGossipHello(Player* p_Player)
        {
            WorldPacket l_Data(SMSG_SHOW_MAILBOX, 16);
            l_Data.appendPackGUID(me->GetGUID());
            p_Player->SendMessageToSet(&l_Data, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new spell_npc_eldritch_grimoireAI(creature);
    }
};

/// Last Update 7.3.5 Build 26365
/// Called By Tiger Lightning - 123996
class spell_npc_xuen_tiger_lightning_damage : public SpellScriptLoader
{
public:
    spell_npc_xuen_tiger_lightning_damage() : SpellScriptLoader("spell_npc_xuen_tiger_lightning_damage") { }

    class spell_npc_xuen_tiger_lightning_damage_SpellScript : public SpellScript
    {
        enum eSpells
        {
            WindwalkerSpec              = 137025,
            FerocityOfTheBrokenTemple   = 241136
        };

        enum eArtifactPowers
        {
            WindborneBlowsArtifact  = 1376
        };

        PrepareSpellScript(spell_npc_xuen_tiger_lightning_damage_SpellScript);

        void ChangeDamage(SpellEffIndex/* p_SpellEffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            Player* l_Player = l_Owner->ToPlayer();
            if (!l_Player)
                return;

            float l_AP = l_Player->GetTotalAttackPowerValue(WeaponAttackType::BaseAttack);
            int32 l_Damage = 4.509f * l_AP * 0.3f;

            uint8 l_Rank = l_Player->GetRankOfArtifactPowerId(eArtifactPowers::WindborneBlowsArtifact);
            float l_Amount = 5 * (1 + (l_Rank-1) / 10);
            AddPct(l_Damage, l_Amount);

            ///< Ferocity of the Broken
            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::FerocityOfTheBrokenTemple))
                AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

            ///< Versatility
            l_Damage *= (1.0f + (l_Player->GetFloatValue(PLAYER_FIELD_VERSATILITY) / 100.0f));

            ///< Windwalker spec modifier
            if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(eSpells::WindwalkerSpec))
                AddPct(l_Damage, l_SpellInfo->Effects[EFFECT_0].BasePoints);

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_npc_xuen_tiger_lightning_damage_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_npc_xuen_tiger_lightning_damage_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Xuen The White Tiger 63508
class spell_npc_xuen_the_white_tiger : public CreatureScript
{
    public:
        spell_npc_xuen_the_white_tiger() : CreatureScript("spell_npc_xuen_the_white_tiger") { }

		enum eSpells
		{
			TigerLightning = 123996
		};

        struct spell_npc_xuen_the_white_tigerAI : public ScriptedAI
        {
            spell_npc_xuen_the_white_tigerAI(Creature *creature) : ScriptedAI(creature) { }

            uint32 m_UpdateTimer = 1 * IN_MILLISECONDS;

            void UpdateAI(const uint32 p_Diff) override
            {
                if (m_UpdateTimer > p_Diff)
                {
                    m_UpdateTimer -= p_Diff;
                    return;
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();

                m_UpdateTimer = 1 * IN_MILLISECONDS;

                if (me->isInCombat() && me->getVictim())
                    me->CastSpell(me, eSpells::TigerLightning, true);

                if (Unit* l_Owner = me->GetOwner())
                {
                    Unit* l_OwnerTarget = nullptr;
                    if (Player* l_Plr = l_Owner->ToPlayer())
                        l_OwnerTarget = l_Plr->GetSelectedUnit();
                    else
                        l_OwnerTarget = l_Owner->getVictim();

                    if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && !l_Owner->IsFriendlyTo(l_OwnerTarget) && me->IsValidAttackTarget(l_OwnerTarget))
                        AttackStart(l_OwnerTarget);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_xuen_the_white_tigerAI(creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Dancing Rune Weapon - 27893
class npc_dk_dancing_rune_weapon : public CreatureScript
{
    public:
        npc_dk_dancing_rune_weapon() : CreatureScript("npc_dk_dancing_rune_weapon") { }

        enum eSpells
        {
            BloodPlague = 55078
        };

        struct npc_dk_dancing_rune_weapon_AI : public ScriptedAI
        {
            npc_dk_dancing_rune_weapon_AI(Creature* p_Creature) : ScriptedAI(p_Creature) {}

            uint32 m_RemainingTime = 0;

            void Reset() override
            {
                if (Unit* l_Owner = me->GetOwner())
                {
                    Player* l_Player = l_Owner->ToPlayer();
                    if (l_Player == nullptr)
                        return;

                    float l_MinDamage = 0.f;
                    float l_MaxDamage = 0.f;

                    l_Player->CalculateMinMaxDamage(WeaponAttackType::BaseAttack, false, true, l_MinDamage, l_MaxDamage);
                    me->SetBaseWeaponDamage(WeaponAttackType::BaseAttack, MINDAMAGE, l_MinDamage);
                    me->SetBaseWeaponDamage(WeaponAttackType::BaseAttack, MAXDAMAGE, l_MaxDamage);
                    me->setAttackTimer(WeaponAttackType::BaseAttack, l_Owner->getAttackTimer(WeaponAttackType::BaseAttack));
                    me->SetAttackTime(WeaponAttackType::BaseAttack, l_Owner->GetAttackTime(WeaponAttackType::BaseAttack));
                    me->UpdateDamagePhysical(WeaponAttackType::BaseAttack);

                    if (TempSummon* l_TempSummon = me->ToTempSummon())
                        m_RemainingTime = l_TempSummon->GetTimer();
                }
            }

            void DamageDealt(Unit* p_Victim, uint32& p_Damage, DamageEffectType p_Type, SpellInfo const* p_SpellInfo) override
            {
                p_Damage /= 3;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_Victim = me->getVictim() ? me->getVictim() : l_Owner->getVictim();
                if (!l_Victim)
                    return;

                TempSummon* l_TempSummon = me->ToTempSummon();
                if (!l_TempSummon)
                    return;

                if (m_RemainingTime <= p_Diff)
                {
                    if (me->GetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS) != 0)
                    {
                        me->SetDisplayId(11686);
                        me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEMS, 0);
                    }
                }
                else
                    m_RemainingTime -= p_Diff;

                if (Aura* l_Aura = l_Victim->GetAura(eSpells::BloodPlague, me->GetGUID()))
                {
                    if (l_TempSummon->GetTimer() < l_Aura->GetDuration() + IN_MILLISECONDS)
                        l_TempSummon->SetDuration(l_Aura->GetDuration() + IN_MILLISECONDS);
                }
                else if (!m_RemainingTime)
                    l_TempSummon->SetDuration(IN_MILLISECONDS);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_dk_dancing_rune_weapon_AI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Cobra Spite - 206685
class spell_npc_hun_cobra_spit : public SpellScriptLoader
{
public:
    spell_npc_hun_cobra_spit() : SpellScriptLoader("spell_npc_hun_cobra_spit") { }

    class spell_npc_hun_cobra_spit_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_npc_hun_cobra_spit_SpellScript);

        void ChangeDamage(SpellEffIndex/* p_SpellEffectIndex*/)
        {
            Unit* l_Caster = GetCaster();
            if (!l_Caster)
                return;

            Unit* l_Owner = l_Caster->GetOwner();
            if (!l_Owner)
                return;

            Unit* l_Target = GetHitUnit();
            if (!l_Target)
                return;

            int32 l_Damage = GetHitDamage();
            l_Damage = l_Owner->SpellDamageBonusDone(l_Target, GetSpellInfo(), l_Damage, EFFECT_0, SPELL_DIRECT_DAMAGE);
            l_Damage = l_Target->SpellDamageBonusTaken(l_Caster, GetSpellInfo(), l_Damage, SPELL_DIRECT_DAMAGE, EFFECT_0);

            SetHitDamage(l_Damage);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_npc_hun_cobra_spit_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_npc_hun_cobra_spit_SpellScript();
    }
};

/// Last Update 7.3.5 Build 26365
/// Flight Master's Whistle - Item: 141605 - Summon: 114281 - Spell: 253937 - Linked Spell: 227334
class spell_npc_flight_master_mount : public CreatureScript
{
    public:
        spell_npc_flight_master_mount() : CreatureScript("spell_npc_flight_master_mount") { }

        enum eSpells
        {
            Ride = 232721,
            CrossOver = 255442
        };

        enum ePoints
        {
            Point_PlayerPosition = 1,
            Point_RandomPosition = 2
        };

        enum eEvents
        {
            Event_GoToPlayer = 1,
            Event_Fly = 2,
            Event_Teleport = 3
        };

        struct spell_npc_flight_master_mountAI : public ScriptedAI
        {
            spell_npc_flight_master_mountAI(Creature *creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGUID = p_Summoner->GetGUID();

                events.ScheduleEvent(eEvents::Event_GoToPlayer, 500);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                events.Update(p_Diff);

		Player* l_Summoner = sObjectAccessor->FindPlayer(m_SummonerGUID);
		if (!l_Summoner)
		    return;

                switch (events.ExecuteEvent())
                {
                case eEvents::Event_GoToPlayer:
                {
                    Position const l_Bird_To_Player_Point = { l_Summoner->m_positionX, l_Summoner->m_positionY, l_Summoner->m_positionZ };
                    me->SetCanFly(true);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(ePoints::Point_PlayerPosition, l_Bird_To_Player_Point, false);
                    break;
                }
                case eEvents::Event_Teleport:
                {
                    uint32 l_taxiId = sObjectMgr->GetNearestTaxiNode(l_Summoner->GetPositionX(), l_Summoner->GetPositionY(), l_Summoner->GetPositionZ(), l_Summoner->GetMapId(), l_Summoner->GetTeam());
                    l_Summoner->TeleportTo(l_Summoner->GetMapId(), sTaxiNodesStore.LookupEntry(l_taxiId)->x, sTaxiNodesStore.LookupEntry(l_taxiId)->y, sTaxiNodesStore.LookupEntry(l_taxiId)->z, l_Summoner->GetOrientation());
                }
                default:
                    break;
                }
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
		Player* l_Summoner = sObjectAccessor->FindPlayer(m_SummonerGUID);
		if (!l_Summoner)
		    return;

                switch (p_ID)
                {
                case ePoints::Point_PlayerPosition:
                {
                    float l_x, l_y, l_z;
                    me->GetRandomContactPoint(me, l_y, l_x, l_z, 15.0f, 20.0f);
                    Position const l_Bird_random_Point = { l_x, l_y, l_z };

                    l_Summoner->CastSpell(me, eSpells::Ride, true);
                    me->SetCanFly(true);
                    me->GetMotionMaster()->MovePoint(ePoints::Point_RandomPosition, l_Bird_random_Point, false);
                    events.ScheduleEvent(eEvents::Event_Teleport, 700);
                    l_Summoner->CastSpell(l_Summoner, eSpells::CrossOver, true);
                    break;
                }
                default:
                    break;
                }
            }

        private:
            uint64 m_SummonerGUID = 0;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new spell_npc_flight_master_mountAI(creature);
        }
};

/// Last Update 7.1.5 Build 23420
/// Called by Spawn of Serpentrix - 215750
/// Spawn of Serpentrix - 108655
class spell_npc_spawn_of_serpentrix : public CreatureScript
{
    public:
        spell_npc_spawn_of_serpentrix() : CreatureScript("spell_npc_spawn_of_serpentrix") { }

        struct spell_npc_spawn_of_serpentrix_AI : public ScriptedAI
        {
            spell_npc_spawn_of_serpentrix_AI(Creature *creature) : ScriptedAI(creature) { }

            enum eSpells
            {
                Spell_MagmaSpit = 215754
            };

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                me->SetReactState(REACT_HELPER);
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                Unit* l_Owner = me->GetOwner();
                if (!l_Owner)
                    return;

                Unit* l_OwnerTarget = l_Owner->getVictim();
                if (!l_OwnerTarget)
                {
                    if (Player* l_Player = l_Owner->ToPlayer())
                    {
                        l_OwnerTarget = l_Player->GetSelectedUnit();
                    }
                }

                if (l_OwnerTarget && me->isTargetableForAttack(l_OwnerTarget) && !l_OwnerTarget->HasStealthAura() && (!me->getVictim() || me->getVictim() != l_OwnerTarget))
                    AttackStart(l_OwnerTarget);

                if (!me->getVictim())
                    return;

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                DoSpellAttackIfReady(eSpells::Spell_MagmaSpit);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new spell_npc_spawn_of_serpentrix_AI(p_Creature);
        }
};

/// Last Update 7.3.5 Build 26365
/// Called by Magma Spit - 215754
class spell_npc_magma_spit : public SpellScriptLoader
{
    public:
        spell_npc_magma_spit() : SpellScriptLoader("spell_npc_magma_spit") { }

        class spell_npc_magma_spit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_npc_magma_spit_SpellScript);

            enum eSpells
            {
                Spell_SpawnOfSerpentrix = 215745
            };

            void HandleChangeDamage(SpellEffIndex/* p_SpellEffectIndex*/)
            {
                Unit* l_Caster = GetCaster();
                if (!l_Caster)
                    return;

                Unit* l_Owner = l_Caster->GetOwner();
                if (!l_Owner)
                    return;

                AuraEffect* l_AuraEffect = l_Owner->GetAuraEffect(eSpells::Spell_SpawnOfSerpentrix, EFFECT_0);
                if (!l_AuraEffect)
                    return;

                SetHitDamage(l_AuraEffect->GetAmount());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_npc_magma_spit_SpellScript::HandleChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_npc_magma_spit_SpellScript();
        }
};

/// Last Update 7.3.5 Build 26365
/// Legion Bombardment - 129063
class spell_npc_item_legion_bombardment : public CreatureScript
{
public:
    spell_npc_item_legion_bombardment() : CreatureScript("spell_npc_item_legion_bombardment") { }

    struct spell_npc_item_legion_bombardmentAI : public ScriptedAI
    {
        spell_npc_item_legion_bombardmentAI(Creature *creature) : ScriptedAI(creature) { }

        enum eSpells
        {
            sVisual = 256135,
            sAreaTrigger = 253248,
            sSelfDamage = 256327
        };

        uint32 m_UpdateTimer = 1.3 * IN_MILLISECONDS;
        uint64 m_OwnerGuid = 0;

        void IsSummonedBy(Unit* p_Summoner) override
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            me->DisableHealthRegen();

            m_OwnerGuid = p_Summoner->GetGUID();
        }

        void UpdateAI(uint32 const p_Diff) override
        {
            if (m_UpdateTimer > p_Diff)
            {
                m_UpdateTimer -= p_Diff;
                return;
            }

            m_UpdateTimer = 1.3 * IN_MILLISECONDS;

            Player* l_Player = sObjectAccessor->GetPlayer(*me, m_OwnerGuid);
            if (!l_Player)
                return;

            AreaTrigger* l_AreaTrigger = l_Player->GetAreaTrigger(eSpells::sAreaTrigger);
            if (!l_AreaTrigger)
                return;

            Item* l_Trinket = l_Player->GetItemByGuid(l_AreaTrigger->GetCastItemGuid());
            if (!l_Trinket)
                return;

            Position l_AtPos = l_AreaTrigger->GetPosition();

            me->SetFacingTo(me->GetAngle(l_AtPos.m_positionX, l_AtPos.m_positionY));

            /// If you stand too close to the target, the blasts will hit the ship and it will "crash" and be destroyed before firing off its full volley.
            /// Also destroys ship if trinket has been unequipped
            if (l_Trinket->IsEquipped() && me->GetDistance2d(l_AtPos.m_positionX, l_AtPos.m_positionY) > 10.0f)
                me->CastSpell(l_AtPos, eSpells::sVisual, true, l_Trinket, nullptr, l_Player->GetGUID());
            else
                me->CastSpell(me, eSpells::sSelfDamage, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new spell_npc_item_legion_bombardmentAI(creature);
    }
};

#ifndef __clang_analyzer__
void AddSC_npc_spell_scripts()
{
    new spell_npc_spitting_cobra();
    new spell_npc_windfury_totem();
    new spell_npc_rottgut_the_abomination();
    new spell_npc_meat_hook_dmg();

    /// Mage NPC
    new spell_npc_mage_prismatic_crystal();
    new spell_npc_mage_arcane_familiar();
    new spell_npc_mirror_image();

    /// Monk NPC
    new spell_npc_black_ox_statue();
    new spell_npc_monk_niuzao();
    new spell_npc_earth_and_fire_spirit();
    new spell_npc_wind_spirit();
    new spell_npc_chi_ji();
    new spell_npc_li_li_stormstout();
    new spell_npc_xuen_the_white_tiger();
    new spell_npc_xuen_tiger_lightning_damage();

    /// Rogue NPC
    new spell_npc_rogue_shadow_reflection();
    new spell_npc_akaari_soul();

    /// Shaman NPC
    new spell_npc_sha_lightning_surge_totem();
    new spell_npc_sha_spirit_link_totem();
    new spell_npc_sha_earthen_shield_totem();
    new spell_npc_sha_cloudburst_totem();
    new spell_npc_sha_storm_elemental();
    new spell_npc_sha_fire_elemental();
    new spell_npc_sha_earth_elemental();
    new spell_npc_sha_feral_spirit();
    new spell_npc_resonance_totem();
    new spell_npc_storm_totem();
    new spell_npc_tailwind_totem();
    new spell_npc_ember_totem();
    new spell_npc_skyfury_totem();
    new spell_npc_counterstrike_totem();
    new spell_npc_tidal_totem();
    new spell_npc_lightning_elemental();

    /// Warrior NPC
    new spell_npc_warr_ravager();
    new spell_npc_warr_mocking_banner();
    new spell_npc_warr_war_banner();

    /// Warlock NPC
    new spell_npc_warl_wild_imp();
    new spell_npc_warl_imp();
    new spell_npc_shadowy_tear();
    new spell_npc_chaos_tear();
    new spell_npc_unstable_tear();
    new spell_npc_warl_doomguard();
    new spell_npc_warl_demonic_gateway_purple();
    new spell_npc_warl_demonic_gateway_green();
    new spell_npc_warl_inner_demon();
    new spell_npc_warl_dreadstalkers();
    new spell_npc_warl_dreadstalkers_imps();
    new spell_npc_warl_darkglare();
    new spell_npc_warl_fel_lord();
    new spell_npc_warl_observer();
    new spell_npc_warl_grimoire_felguard();
    new spell_npc_warl_soul_effigy();
    new spell_npc_infernal();
    new spell_npc_felhunter();
    new spell_warl_shadow_bulwark();
    new spell_npc_warl_grimoire_felhunter();
    new spell_npc_warl_grimoire_succubus();
    new spell_warl_pets_autocast_mechanic();
    new spell_npc_flame_rift();
    new spell_npc_tears_destruction_modifier();
    new spell_npc_searing_bolt();
    new spell_npc_wrathguard_wrathstorm();
    ///new spell_npc_demonic_emprowement_attackspeed(); ///< removed as it was breaking AS on pets, but need to monitor for now, so i'm not deleting the script yet

    /// Priest NPC
    new spell_npc_priest_psyfiend();
    new spell_npc_sphere_of_insanity();
    new spell_npc_void_tentacle();
    new spel_npc_pri_mindbender();

    /// Druid NPC
    new spell_npc_dru_force_of_nature_resto();
    new npc_force_of_nature_balance();

    /// Hunt NPC
    new spell_npc_hun_hati();
    new npc_dark_minion();
    new spell_npc_stampede();
    new spell_npc_hun_dire_basilisk();
    new spell_npc_hun_cobra_commander();
    new spell_npc_hun_deathstrikevenom();
    new spell_npc_hun_cobra_spit();

    /// Generic NPC
    new spell_npc_totem_of_harmony();
    new spell_npc_inge_exeplosive_sheep();
    new spell_npc_flight_master_mount();
    new spell_npc_spawn_of_serpentrix();
    new spell_npc_magma_spit();

    /// Death Knight NPC
    new spell_npc_dk_bloodworm();
    new spell_npc_dk_gargoyle();
    new spell_npc_dk_valkyr();
    new spell_npc_unhloy_mutation();
    new spell_npc_shambling_horror();
    new spell_npc_dk_skulker();
    new spell_npc_skulker_shot();
    new npc_dk_dancing_rune_weapon();

    /// Paladin NPC
    new spell_npc_guardian_of_the_forgotten_queen();
    new spell_npc_echo_of_the_highlord();

    /// Demon Hunter
    new spell_npc_altruis_the_sufferer();
    new spell_npc_matron_mother_malevolence();

    /// Item NPC
    new spell_npc_shadowy_reflection();
    new spell_npc_item_legion_bombardment();

    /// Nightborne
    new spell_npc_eldritch_grimoire();
}
#endif
