////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "eye_of_azshara.hpp"

enum Says
{
    SAY_CALL,
    SAY_FRENZY
};

enum Spells
{
    SPELL_GROUND_SLAM               = 193093,
    SPELL_GROUND_SLAM_DUMMY         = 193056,
    SPELL_GASEOUS_BUBBLES           = 193018,
    SPELL_GASEOUS_BUBBLES_EXPLOSE   = 193047,
    SPELL_QUAKE                     = 193152,
    SPELL_CALL_THE_SEAS             = 193051,
    SPELL_FRENZY                    = 197550,

    SPELL_QUAKE_VISAL               = 193175,
    SPELL_QUAKE_DMG                 = 193159,
    SPELL_AFTERSHOCK_DMG            = 193171,

    SPELL_BONUS_ROLL                = 226622
};

enum eEvents
{
    EVENT_GROUND_SLAM       = 1,
    EVENT_GASEOUS_BUBBLES,
    EVENT_QUAKE,
    EVENT_CALL_THE_SEAS
};

/// King Deepbeard - 91797
class boss_king_deepbeard : public CreatureScript
{
    public:
        boss_king_deepbeard() : CreatureScript("boss_king_deepbeard") { }

        struct boss_king_deepbeardAI : public BossAI
        {
            boss_king_deepbeardAI(Creature* p_Creature) : BossAI(p_Creature, DATA_DEEPBEARD) { }

            bool m_LowHP;
            bool m_ActiveEnergy;
            uint16 m_ChangeEnergyTimer;

            void Reset() override
            {
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, SpellImmunity::IMMUNITY_EFFECT, SpellEffects::SPELL_EFFECT_KNOCK_BACK_DEST, true);

                _Reset();

                me->setPowerType(Powers::POWER_ENERGY);
                me->SetMaxPower(Powers::POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 30);

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS_2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);

                m_ChangeEnergyTimer = 1000;
                m_ActiveEnergy = true;
                m_LowHP = false;
            }

            void EnterCombat(Unit* /*p_Who*/) override
            {
                _EnterCombat();

                events.ScheduleEvent(EVENT_GROUND_SLAM, 7000);
                events.ScheduleEvent(EVENT_GASEOUS_BUBBLES, 11000);
                events.ScheduleEvent(EVENT_CALL_THE_SEAS, 20000);
            }

            void JustDied(Unit* /*p_Killer*/) override
            {
                _JustDied();

                CastSpellToPlayers(me, SPELL_BONUS_ROLL, true);
                for (Map::PlayerList::const_iterator l_Itr = me->GetMap()->GetPlayers().begin(); l_Itr != me->GetMap()->GetPlayers().end(); ++l_Itr)
                {
                    Player* l_Player = l_Itr->getSource();
                    if (!l_Player)
                        continue;

                    l_Player->AddDelayedEvent([l_Player]() -> void
                    {
                        l_Player->ClearInCombat();
                    }, 100);
                }
            }

            void DamageTaken(Unit* /*p_Attacker*/, uint32& p_Damage, SpellInfo const* /*p_SpellInfo*/) override
            {
                if (me->HealthBelowPctDamaged(30, p_Damage) && !m_LowHP)
                {
                    m_LowHP = true;
                    DoCast(me, SPELL_FRENZY, true);
                    Talk(SAY_FRENZY);
                }
            }

            void RegeneratePower(Powers /*p_Power*/, int32& p_Value) override
            {
                /// Regens by script
                p_Value = 0;
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == SPELL_QUAKE)
                    m_ActiveEnergy = true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                if (m_ActiveEnergy)
                {
                    if (m_ChangeEnergyTimer <= p_Diff)
                    {
                        if (me->GetPower(POWER_ENERGY) >= 100)
                        {
                            events.ScheduleEvent(EVENT_QUAKE, 0);
                            m_ActiveEnergy = false;
                        }
                        else
                            me->ModifyPower(POWER_ENERGY, 5);

                        m_ChangeEnergyTimer = 1000;
                    }
                    else
                        m_ChangeEnergyTimer -= p_Diff;
                }

                events.Update(p_Diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetDistance(me->GetHomePosition()) >= 70.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                switch (events.ExecuteEvent())
                {
                    case EVENT_GROUND_SLAM:
                    {
                        if (me->getVictim())
                        {
                            me->SetFacingTo(me->GetAngle(me->getVictim()));
                            DoCast(me->getVictim(), SPELL_GROUND_SLAM);
                            DoCast(me, SPELL_GROUND_SLAM_DUMMY, true);
                        }
                        events.ScheduleEvent(EVENT_GROUND_SLAM, 18000);
                        break;
                    }
                    case EVENT_GASEOUS_BUBBLES:
                    {
                        DoCast(SPELL_GASEOUS_BUBBLES);
                        events.ScheduleEvent(EVENT_GASEOUS_BUBBLES, 33000);
                        break;
                    }
                    case EVENT_QUAKE:
                    {
                        DoCast(SPELL_QUAKE);

                        AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            std::list<Creature*> l_Quakes;

                            me->GetCreatureListWithEntryInGrid(l_Quakes, NPC_QUAKE, 100.0f);

                            for (Creature* l_Summon : l_Quakes)
                            {
                                l_Summon->CastSpell(l_Summon, SPELL_AFTERSHOCK_DMG, true);

                                if (!IsMythic())
                                    l_Summon->DespawnOrUnsummon(1 * TimeConstants::IN_MILLISECONDS);
                            }
                        });

                        break;
                    }
                    case EVENT_CALL_THE_SEAS:
                    {
                        DoCast(SPELL_CALL_THE_SEAS);
                        Talk(SAY_CALL);
                        events.ScheduleEvent(EVENT_CALL_THE_SEAS, 30000);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_king_deepbeardAI(p_Creature);
        }
};

/// Quake - 97916
class npc_deepbeard_quake : public CreatureScript
{
    public:
        npc_deepbeard_quake() : CreatureScript("npc_deepbeard_quake") { }

        struct npc_deepbeard_quakeAI : public ScriptedAI
        {
            npc_deepbeard_quakeAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override { }

            void IsSummonedBy(Unit* /*p_Summoner*/) override
            {
                DoCast(me, SPELL_QUAKE_VISAL, true);
                DoCast(SPELL_QUAKE_DMG);

                /// Set to infinite duration for mythic and mythic+ modes
                if (IsMythic() && me->ToTempSummon())
                    me->ToTempSummon()->SetDuration(-1);
            }

            void UpdateAI(uint32 const /*p_Diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_deepbeard_quakeAI(creature);
        }
};

/// Call the Seas - 97844
class npc_deepbeard_call_the_seas : public CreatureScript
{
    public:
        npc_deepbeard_call_the_seas() : CreatureScript("npc_deepbeard_call_the_seas") { }

        struct npc_deepbeard_call_the_seasAI : public ScriptedAI
        {
            npc_deepbeard_call_the_seasAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            bool m_MoveActive;
            uint16 m_RandomMoveTimer;

            void Reset() override
            {
                m_MoveActive = true;
                m_RandomMoveTimer = 1000;
            }

            void MovementInform(uint32 p_Type, uint32 /*p_ID*/) override
            {
                if (p_Type != POINT_MOTION_TYPE)
                    return;

                m_MoveActive = true;
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (m_MoveActive)
                {
                    if (m_RandomMoveTimer <= p_Diff)
                    {
                        m_MoveActive = false;
                        m_RandomMoveTimer = 500;

                        float l_Angle = (float)rand_norm() * static_cast<float>(2 * M_PI);
                        float l_Dist = 20 * (float)rand_norm();
                        float l_X = me->GetHomePosition().GetPositionX() + l_Dist * std::cos(l_Angle);
                        float l_Y = me->GetHomePosition().GetPositionY() + l_Dist * std::sin(l_Angle);
                        float l_Z = me->GetHomePosition().GetPositionZ();
                        JadeCore::NormalizeMapCoord(l_X);
                        JadeCore::NormalizeMapCoord(l_Y);

                        me->GetMotionMaster()->MovePoint(1, l_X, l_Y, l_Z);
                    }
                    else
                        m_RandomMoveTimer -= p_Diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_deepbeard_call_the_seasAI(p_Creature);
        }
};

/// Gaseous Bubbles - 193018
class spell_deepbeard_gaseous_explosion : public SpellScriptLoader
{
    public:
        spell_deepbeard_gaseous_explosion() : SpellScriptLoader("spell_deepbeard_gaseous_explosion") { }

        class spell_deepbeard_gaseous_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deepbeard_gaseous_explosion_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (p_Targets.empty())
                    return;

                p_Targets.remove_if([this](WorldObject* p_Object) -> bool
                {
                    if (p_Object == nullptr || p_Object->GetTypeId() != TypeID::TYPEID_PLAYER)
                        return true;

                    Player* l_Player = p_Object->ToPlayer();
                    if (l_Player == nullptr)
                        return true;

                    if (l_Player->GetRoleForGroup() == Roles::ROLE_TANK)
                        return true;

                    return false;
                });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_deepbeard_gaseous_explosion_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deepbeard_gaseous_explosion_SpellScript();
        }

        class spell_deepbeard_gaseous_explosion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deepbeard_gaseous_explosion_AuraScript);

            int32 m_Damage = 0;

            void OnRemove(AuraEffect const* /*p_AurEff*/, AuraEffectHandleModes /*p_Mode*/)
            {
                if (!GetCaster() || !GetTarget())
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                {
                    if (GetTarget()->GetMap()->GetDifficultyID() != Difficulty::DifficultyNormal)
                    {
                        if (SpellInfo const* l_SpellInfo = sSpellMgr->GetSpellInfo(SPELL_GASEOUS_BUBBLES_EXPLOSE))
                            m_Damage = l_SpellInfo->Effects[EFFECT_0].BasePoints;

                        std::list<Player*> l_Players;
                        GetPlayerListInGrid(l_Players, GetCaster(), 100.0f);

                        for (Player* const& player : l_Players)
                        {
                            if (AuraEffect* l_AurEff = player->GetAuraEffect(GetId(), EFFECT_0))
                            {
                                m_Damage += l_AurEff->GetAmount();
                                l_AurEff->GetBase()->Remove();
                            }
                        }

                        GetTarget()->CastCustomSpell(GetTarget(), SPELL_GASEOUS_BUBBLES_EXPLOSE, &m_Damage, 0, 0, true);
                    }
                    else
                        GetTarget()->CastSpell(GetTarget(), SPELL_GASEOUS_BUBBLES_EXPLOSE, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_deepbeard_gaseous_explosion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_deepbeard_gaseous_explosion_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_king_deepbeard()
{
    /// Boss
    new boss_king_deepbeard();

    /// Creatures
    new npc_deepbeard_quake();
    new npc_deepbeard_call_the_seas();

    /// Spell
    new spell_deepbeard_gaseous_explosion();
}
#endif
