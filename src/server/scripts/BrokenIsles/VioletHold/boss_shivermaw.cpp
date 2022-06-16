////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2016 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "violet_hold_legion.hpp"

enum Says
{
    SAY_WARNING             = 0,
};

enum Spells
{
    SPELL_FROST_BREATH              = 201379,
    SPELL_RELENTLESS_STORM          = 201672,
    SPELL_RELENTLESS_STORM_TRIG     = 201848, //26 cast count
    SPELL_RELENTLESS_STORM_SEARCH   = 201865, //Target search
    SPELL_RELENTLESS_STORM_DMG      = 201852,
    SPELL_RELENTLESS_STORM_AT_1     = 201643,
    SPELL_RELENTLESS_STORM_AT_2     = 201652,
    SPELL_TAIL_SWEEP                = 201354,
    SPELL_WING_BUFFET               = 201355,
    SPELL_ICE_BOMB                  = 201960,
    SPELL_ICE_BOMB_DMG_AOE          = 201961,
    SPELL_FROZEN                    = 202037,
    SPELL_FRIGID_WINDS              = 202062,

    SPELL_BONUS_LOOT                = 226663
};

enum eEvents
{
    EVENT_FROST_BREATH              = 1,
    EVENT_RELENTLESS_STORM          = 2,
    EVENT_TAIL_SWEEP                = 3,
    EVENT_WING_BUFFET               = 4,
    EVENT_FLY                       = 5,
    EVENT_ICE_BOMB                  = 6,
    EVENT_GROUND                    = 7,
    EVENT_FRIGID_WINDS              = 8
};

/// Shivermaw - 101951
class boss_shivermaw : public CreatureScript
{
public:
    boss_shivermaw() : CreatureScript("boss_shivermaw") {}

    struct boss_shivermawAI : public BossAI
    {
        boss_shivermawAI(Creature* creature) : BossAI(creature, DATA_SHIVERMAW)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            removeloot = false;
        }

        bool removeloot;

        void Reset() override
        {
            _Reset();
            SetFlyMode(false);

            if (instance->GetData(DATA_MAIN_EVENT_PHASE) == IN_PROGRESS)
                me->SetReactState(REACT_DEFENSIVE);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();

            events.ScheduleEvent(EVENT_FROST_BREATH, 5000); //49:27, 49:53, 50:28
            events.ScheduleEvent(EVENT_RELENTLESS_STORM, 9000); //49:31, 49:45, 50:32, 50:46, 51:33, 51:47
            events.ScheduleEvent(EVENT_TAIL_SWEEP, 17000); //49:39, 49:57

            if (IsHeroicOrMythic())
                events.ScheduleEvent(EVENT_FRIGID_WINDS, 33000);

            events.ScheduleEvent(EVENT_FLY, 40000);
        }

        void JustDied(Unit* /*killer*/) override
        {
            SetFlyMode(false);

            me->GetMotionMaster()->Clear();

            Position l_Pos = *me;
            l_Pos.m_positionZ = me->GetMap()->GetHeight(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);

            me->GetMotionMaster()->MoveLand(0, l_Pos);

            _JustDied();

            if (removeloot)
            {
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                me->ClearLootContainers();
            }

            CastSpellToPlayers(me, SPELL_BONUS_LOOT, true);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_RELENTLESS_STORM)
            {
                DoCast(me, SPELL_RELENTLESS_STORM_AT_1, true);
                DoCast(me, SPELL_RELENTLESS_STORM_AT_2, true);

                uint32 l_Time = 0;
                for (uint8 i = 0; i < 26; i++)
                {
                    auto l_Missile = [this]() -> void
                    {
                        Position l_Pos;
                        me->GetRandomNearPosition(l_Pos, 50.0f);

                        G3D::Vector3 l_Src(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ);
                        G3D::Vector3 l_Dst(l_Src);

                        l_Src.z -= 0.01f;

                        me->PlayOrphanSpellVisual(l_Src, G3D::Vector3(), l_Dst, 52628, 3.0f);

                        AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this, l_Src, l_Dst]() -> void
                        {
                            me->PlayOrphanSpellVisual(l_Src, G3D::Vector3(0.0f, 0.0f, 344.0f), l_Dst, 52630);
                        });

                        me->CastSpell(l_Pos.GetPositionX(), l_Pos.GetPositionY(), l_Pos.GetPositionZ(), SPELL_RELENTLESS_STORM_TRIG, true);
                    };

                    if (l_Time)
                    {
                        AddTimedDelayedOperation(l_Time, [l_Missile]() -> void
                        {
                            l_Missile();
                        });
                    }
                    else
                        l_Missile();

                    l_Time += 100;
                }
            }
        }

        void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode /*p_Mode*/) override
        {
            if (p_Dest == nullptr)
                return;

            if (p_SpellInfo->Id == SPELL_RELENTLESS_STORM_TRIG)
                me->CastSpell(p_Dest, SPELL_RELENTLESS_STORM_DMG, true);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell) override
        {
            if (spell->Id == SPELL_ICE_BOMB_DMG_AOE)
                if (target->GetPositionZ() < 82.0f)
                    DoCast(target, SPELL_FROZEN, true);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 2)
                events.ScheduleEvent(EVENT_ICE_BOMB, 500);

            if (id == 3)
            {
                SetFlyMode(false);
                me->SetReactState(REACT_AGGRESSIVE);
            }
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_REMOVE_LOOT)
                removeloot = true;
        }

        void UpdateAI(uint32 const diff) override
        {
            UpdateOperations(diff);

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_FROST_BREATH:
                        DoCast(SPELL_FROST_BREATH);
                        events.ScheduleEvent(EVENT_FROST_BREATH, 26000);
                        break;
                    case EVENT_RELENTLESS_STORM:
                        DoCast(SPELL_RELENTLESS_STORM);
                        events.ScheduleEvent(EVENT_RELENTLESS_STORM, 14000);
                        break;
                    case EVENT_TAIL_SWEEP:
                        DoCastVictim(SPELL_TAIL_SWEEP);
                        events.ScheduleEvent(EVENT_TAIL_SWEEP, 18000);
                        events.ScheduleEvent(EVENT_WING_BUFFET, 3000); //49:42, 50:00, 50:43
                        break;
                    case EVENT_WING_BUFFET:
                        if (me->getVictim())
                            DoCast(me->getVictim(), SPELL_WING_BUFFET);
                        break;
                    case EVENT_FLY:
                        me->SetReactState(ReactStates::REACT_PASSIVE);
                        DoStopAttack();
                        SetFlyMode(true);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(2, 4632.15f, 4016.52f, 112.51f, false); //Fly point
                        if (IsHeroicOrMythic())
                            events.ScheduleEvent(EVENT_FRIGID_WINDS, 53000);
                        events.ScheduleEvent(EVENT_FLY, 60000);
                        break;
                    case EVENT_ICE_BOMB:
                        DoCast(SPELL_ICE_BOMB);
                        ZoneTalk(SAY_WARNING);
                        events.ScheduleEvent(EVENT_GROUND, 6000);
                        break;
                    case EVENT_GROUND:
                        events.DelayEvents(3000);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(3, me->GetPositionX(), me->GetPositionY(), 78.0f);
                        break;
                    case EVENT_FRIGID_WINDS:
                        DoCast(me, SPELL_FRIGID_WINDS);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_shivermawAI (creature);
    }
};

//102301
class npc_shivermaw_ice_block : public CreatureScript
{
public:
    npc_shivermaw_ice_block() : CreatureScript("npc_shivermaw_ice_block") {}

    struct npc_shivermaw_ice_blockAI : public ScriptedAI
    {
        npc_shivermaw_ice_blockAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL);
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (Player* pl = me->FindNearestPlayer(0.0f))
                pl->RemoveAurasDueToSpell(SPELL_FROZEN);

            me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const diff) override {}
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_shivermaw_ice_blockAI(creature);
    }
};

/// Tail Sweep - 201354
class spell_shivermaw_tail_sweep : public SpellScriptLoader
{
    public:
        spell_shivermaw_tail_sweep() : SpellScriptLoader("spell_shivermaw_tail_sweep") { }

        class spell_shivermaw_tail_sweep_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shivermaw_tail_sweep_SpellScript);

            enum eSpell
            {
                TargetRestrict = 28989
            };

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                p_Targets.clear();

                SpellTargetRestrictionsEntry const* l_Restriction = sSpellTargetRestrictionsStore.LookupEntry(eSpell::TargetRestrict);
                if (l_Restriction == nullptr)
                    return;

                Unit* l_Caster = GetCaster();
                if (l_Caster == nullptr)
                    return;

                std::list<Unit*> l_TargetList;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, 20.0f);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                l_Caster->VisitNearbyObject(20.0f, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (l_Caster->isInBack(l_Unit, l_Restriction->ConeAngle * M_PI / 180.0f))
                        p_Targets.push_back(l_Unit);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shivermaw_tail_sweep_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_shivermaw_tail_sweep_SpellScript();
        }
};

/// Relentless Storm - 201643
class at_shivermaw_relentless_storm : public AreaTriggerEntityScript
{
    public:
        at_shivermaw_relentless_storm() : AreaTriggerEntityScript("at_shivermaw_relentless_storm") { }

        void BeforeCreate(AreaTrigger* p_AreaTrigger) override
        {
            switch (p_AreaTrigger->GetSpellId())
            {
                case SPELL_RELENTLESS_STORM_AT_1:
                    p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, 113.1431f);
                    break;
                case SPELL_RELENTLESS_STORM_AT_2:
                    p_AreaTrigger->SetFloatValue(EAreaTriggerFields::AREATRIGGER_FIELD_BOUNDS_RADIUS_2D, 37.58063f);
                    break;
                default:
                    break;
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new at_shivermaw_relentless_storm();
        }
};

#ifndef __clang_analyzer__
void AddSC_boss_shivermaw()
{
    new boss_shivermaw();
    new npc_shivermaw_ice_block();
    new spell_shivermaw_tail_sweep();
    new at_shivermaw_relentless_storm();
}
#endif
