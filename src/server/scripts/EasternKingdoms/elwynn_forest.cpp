////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/*######
## npc_stormwind_infantry
######*/

#include "EasternKingdomsPCH.h"

#define NPC_WOLF    49871

class npc_stormwind_infantry : public CreatureScript
{
public:
    npc_stormwind_infantry() : CreatureScript("npc_stormwind_infantry") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormwind_infantryAI (creature);
    }

    struct npc_stormwind_infantryAI : public ScriptedAI
    {
        npc_stormwind_infantryAI(Creature* creature) : ScriptedAI(creature) {}

        bool HasATarget;

        void Reset()
        {
            HasATarget = false;
        }

        void DamageTaken(Unit* doneBy, uint32& damage, SpellInfo const*  /*p_SpellInfo*/)
        {
            if (doneBy->ToCreature())
                if (me->GetHealth() <= damage || me->GetHealthPct() <= 80.0f)
                    damage = 0;
        }

        void DamageDealt(Unit* target, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*p_SpellInfo*/)
        {
            if (target->ToCreature())
                if (target->GetHealth() <= damage || target->GetHealthPct() <= 70.0f)
                    damage = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who && !HasATarget)
                if (me->GetDistance(who) < 5.0f)
                    if (Creature* creature = who->ToCreature())
                        if (creature->GetEntry() == NPC_WOLF)
                            AttackStart(who);
        }

        void EnterEvadeMode()
        {
            HasATarget = false;
        }
    };
};

/*######
## npc_stormwind_injured_soldier
######*/

#define SPELL_HEAL          93072

class npc_stormwind_injured_soldier : public CreatureScript
{
public:
    npc_stormwind_injured_soldier() : CreatureScript("npc_stormwind_injured_soldier") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormwind_injured_soldierAI (creature);
    }

    struct npc_stormwind_injured_soldierAI : public ScriptedAI
    {
        npc_stormwind_injured_soldierAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {}

        void OnSpellClick(Unit* /*Clicker*/)
        {
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->RemoveByteFlag(UNIT_FIELD_ANIM_TIER, UNIT_BYTES_1_OFFSET_STAND_STATE, UNIT_STAND_STATE_DEAD);
            me->ForcedDespawn(2000);
            me->SetRespawnDelay(10);
        }

        void EnterCombat(Unit* /*who*/)
        {
            return;
        }
    };
};

/// this quest is deprecated since 7.0.3
/*######
## npc_training_dummy_elwynn
######*/

enum eTrainingDummySpells
{
    SPELL_CHARGE        = 100,
    SPELL_AUTORITE      = 105361, // OnDamage
    SPELL_RUN_TROUGH    = 2098,
    SPELL_MOT_DOULEUR_1 = 589,
    SPELL_MOT_DOULEUR_2 = 124464, // Je ne sais pas si un des deux est le bon
    SPELL_NOVA          = 122,
    SPELL_CORRUPTION_1  = 172,
    SPELL_CORRUPTION_2  = 87389,
    SPELL_CORRUPTION_3  = 131740,
    SPELL_PAUME_TIGRE   = 100787
};

class npc_training_dummy_start_zones : public CreatureScript
{
public:
    npc_training_dummy_start_zones() : CreatureScript("npc_training_dummy_start_zones") { }

    struct npc_training_dummy_start_zonesAI : Scripted_NoMovementAI
    {
        npc_training_dummy_start_zonesAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {}

        uint32 resetTimer;

        void Reset()
        {
            me->SetControlled(true, UNIT_STATE_STUNNED);//disable rotate
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);//imune to knock aways like blast wave

            resetTimer = 5000;
        }

        void EnterEvadeMode()
        {
            if (!_EnterEvadeMode())
                return;

            Reset();
        }

        void MoveInLineOfSight(Unit* p_Who)
        {
            if (!me->IsWithinDistInMap(p_Who, 25.0f) && p_Who->isInCombat())
            {
                me->RemoveAllAurasByCaster(p_Who->GetGUID());
                me->getHostileRefManager().deleteReference(p_Who);
            }
        }

        void DamageTaken(Unit* doneBy, uint32& damage, SpellInfo const*  /*p_SpellInfo*/)
        {
            resetTimer = 5000;
            damage = 0;

            if (doneBy->HasAura(SPELL_AUTORITE))
                if (doneBy->ToPlayer())
                    doneBy->ToPlayer()->KilledMonsterCredit(44175, 0);

            me->SetInCombatWith(doneBy);
            doneBy->SetInCombatWith(me);
        }

        void EnterCombat(Unit* /*who*/)
        {
            return;
        }

        void SpellHit(Unit* Caster, const SpellInfo* Spell)
        {
            switch (Spell->Id)
            {
                case SPELL_CHARGE:
                case SPELL_RUN_TROUGH:
                case SPELL_MOT_DOULEUR_1:
                case SPELL_MOT_DOULEUR_2:
                case SPELL_NOVA:
                case SPELL_CORRUPTION_1:
                case SPELL_CORRUPTION_2:
                case SPELL_CORRUPTION_3:
                case SPELL_PAUME_TIGRE:
                    if (Caster->ToPlayer())
                        Caster->ToPlayer()->KilledMonsterCredit(44175, 0);
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (!me->HasUnitState(UNIT_STATE_STUNNED))
                me->SetControlled(true, UNIT_STATE_STUNNED);//disable rotate

            if (resetTimer <= diff)
            {
                EnterEvadeMode();
                resetTimer = 5000;
            }
            else
                resetTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_training_dummy_start_zonesAI(creature);
    }
};

/// Hag's Broom - 51710
class npc_hags_broom_51710 : public CreatureScript
{
    public:
        npc_hags_broom_51710() : CreatureScript("npc_hags_broom_51710") { }

        struct npc_hags_broom_51710AI : public VehicleAI
        {
            npc_hags_broom_51710AI(Creature* p_Creature) : VehicleAI(p_Creature) { }

            G3D::Vector3 const Path_HagsBroomCircle[40] =
            {
                { 1675.743f, 248.1068f, -48.69805f },
                { 1663.993f, 235.1736f, -56.44384f },
                { 1660.054f, 218.0972f, -56.44384f },
                { 1647.606f, 195.8403f, -56.44384f },
                { 1618.122f, 175.2569f, -56.44384f },
                { 1591.681f, 172.4983f, -56.44384f },
                { 1562.734f, 176.6528f, -56.44384f },
                { 1533.047f, 206.7066f, -56.44384f },
                { 1524.212f, 233.3333f, -56.44384f },
                { 1529.76f, 261.5816f, -56.44384f },
                { 1546.182f, 286.5035f, -56.44384f },
                { 1576.585f, 307.6285f, -56.44384f },
                { 1615.399f, 306.1354f, -56.44384f },
                { 1641.585f, 291.1267f, -56.44384f },
                { 1662.649f, 263.1996f, -56.44384f },
                { 1664.774f, 231.9115f, -56.44384f },
                { 1657.422f, 206.026f, -56.44384f },
                { 1638.375f, 201.25f, -56.44384f },
                { 1627.198f, 208.2517f, -56.44384f },
                { 1621.915f, 213.9583f, -56.75116f },
                { 1616.583f, 217.9462f, -55.60745f },
                { 1599.887f, 218.6094f, -50.92685f },
                { 1579.885f, 220.2483f, -46.27073f },
                { 1571.024f, 236.6684f, -52.70364f },
                { 1579.03f, 264.2621f, -43.70478f },
                { 1611.262f, 256.967f, -41.95214f },
                { 1617.38f, 240.3542f, -41.95214f },
                { 1613.135f, 223.0816f, -41.95214f },
                { 1593.88f, 213.6042f, -41.95214f },
                { 1578.345f, 221.6771f, -41.95214f },
                { 1569.002f, 239.7674f, -41.95214f },
                { 1577.858f, 259.4375f, -39.65142f },
                { 1595.896f, 261.2222f, -39.65142f },
                { 1613.816f, 257.1996f, -39.65142f },
                { 1627.106f, 240.4635f, -32.8395f },
                { 1637.733f, 239.8663f, -30.1907f },
                { 1679.2772f, 239.3166f, -31.6678f },
                { 1703.7586f, 239.6562f, -36.3491f },
                { 1721.8422f, 239.4136f, -55.1420f },
                { 1682.1398f, 239.7995f, -57.4484f }
            };

            enum eSpells
            {
                Spell_RideVehicle = 46598,
                Spell_TeleportToUndercity = 96005,
                Spell_ReturnHome = 96017,
                Spell_TeleportToStormwind = 96006
            };

            enum ePoints
            {
                Point_PathBeforeTeleportEnd = 1,
                Point_PreCirclePathEnd = 2,
                Point_CirclePathEnd = 3,
                Point_ReturnToLandPathEnd = 4
            };

            enum eNpcs
            {
                Npc_Hag = 51836
            };

            ObjectGuid m_SummonerGuid;
            bool m_CanStartCirclePath = true;
            bool m_FlyghtEnd = false;
            bool m_CanReturnToLand = true;

            void IsSummonedBy(Unit* p_Summoner) override
            {
                m_SummonerGuid = p_Summoner->GetGUID();
                me->SetReactState(ReactStates::REACT_PASSIVE);
                p_Summoner->EnterVehicle(me, 1, true);
                me->SetDisableGravity(true);

                if (Creature* l_Creature = p_Summoner->SummonCreature(eNpcs::Npc_Hag, me->GetPosition(), TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                {
                    l_Creature->EnterVehicle(me, 0, true);
                }

                me->AddDelayedEvent([this]() -> void
                {
                    G3D::Vector3 const Path_HagsBroomBeforeTeleport[6] =
                    {
                        { -9113.018f, 415.5278f, 96.10319f },
                        { -9106.776f, 416.059f, 99.06326f },
                        { -9094.982f, 420.6267f, 103.8405f },
                        { -9081.766f, 429.8889f, 108.6826f },
                        { -9066.531f, 441.7587f, 112.8316f },
                        { -9051.566f, 455.0469f, 112.8316f }
                    };

                    me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PathBeforeTeleportEnd, Path_HagsBroomBeforeTeleport, 6);
                }, 1000);
            }

            void MovementInform(uint32 /*p_Type*/, uint32 p_PointId) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player)
                    return;

                switch (p_PointId)
                {
                case ePoints::Point_PathBeforeTeleportEnd:
                {
                    DoCast(eSpells::Spell_TeleportToUndercity, true);
                    break;
                }
                case ePoints::Point_PreCirclePathEnd:
                {
                    if (!m_FlyghtEnd)
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_HagsBroomCircle, 40);
                    }

                    break;
                }
                case ePoints::Point_CirclePathEnd:
                {
                    if (!m_FlyghtEnd)
                    {
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_CirclePathEnd, Path_HagsBroomCircle, 40);
                    }

                    break;
                }
                case ePoints::Point_ReturnToLandPathEnd:
                {
                    if (Vehicle* l_Vehicle = me->GetVehicleKit())
                    {
                        if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                        {
                            if (l_Unit->ToCreature())
                            {
                                l_Unit->ExitVehicle();
                                l_Unit->ToCreature()->DespawnOrUnsummon();
                            }
                        }
                    }

                    l_Player->ExitVehicle();
                    me->DespawnOrUnsummon();
                    break;
                }
                default:
                    break;
                }
            }

            void SpellHit(Unit* p_Caster, SpellInfo const* p_Spell) override
            {
                if (p_Spell->Id == eSpells::Spell_ReturnHome)
                {
                    m_FlyghtEnd = true;
                    me->GetMotionMaster()->Clear();
                    DoCast(eSpells::Spell_TeleportToStormwind, true);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                Player* l_Player = ObjectAccessor::GetPlayer(*me, m_SummonerGuid);
                if (!l_Player || !l_Player->IsInWorld())
                {
                    if (Vehicle* l_Vehicle = me->GetVehicleKit())
                    {
                        if (Unit* l_Unit = l_Vehicle->GetPassenger(0))
                        {
                            if (l_Unit->ToCreature())
                            {
                                l_Unit->ExitVehicle();
                                l_Unit->ToCreature()->DespawnOrUnsummon();
                            }
                        }
                    }

                    me->DespawnOrUnsummon();
                    return;
                }

                if (me->GetZoneId() == 1497 && m_CanStartCirclePath)
                {
                    m_CanStartCirclePath = false;
                    me->SetRooted(true);

                    me->AddDelayedEvent([this]() -> void
                    {
                        G3D::Vector3 const Path_HagsBroomPreCircle[8] =
                        {
                            { 1717.285f, 348.2413f, 20.23495f },
                            { 1724.509f, 337.2031f, 11.80331f },
                            { 1735.102f, 314.3559f, -20.56344f },
                            { 1747.181f, 283.5851f, -40.61856f },
                            { 1752.826f, 255.1372f, -43.82628f },
                            { 1738.186f, 242.1267f, -47.13151f },
                            { 1704.476f, 239.9063f, -52.75056f },
                            { 1669.595f, 241.3403f, -52.75056f }
                        };

                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_PreCirclePathEnd, Path_HagsBroomPreCircle, 8);
                    }, 5000);
                }

                if (me->GetZoneId() == 1519 && m_FlyghtEnd && m_CanReturnToLand)
                {
                    m_CanReturnToLand = false;
                    me->SetRooted(true);
                    me->RemoveAura(eSpells::Spell_TeleportToUndercity);

                    me->AddDelayedEvent([this]() -> void
                    {
                        G3D::Vector3 const Path_HagsBroomReturnToLand[6] =
                        {
                            { -9023.413f, 435.974f, 139.6931f },
                            { -9037.161f, 433.6146f, 139.6931f },
                            { -9054.703f, 428.8993f, 133.0008f },
                            { -9083.785f, 418.1996f, 111.1895f },
                            { -9101.274f, 412.6528f, 100.0956f },
                            { -9110.066f, 411.3524f, 96.79972f }
                        };

                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothFlyPath(ePoints::Point_ReturnToLandPathEnd, Path_HagsBroomReturnToLand, 6);
                    }, 5000);
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_hags_broom_51710AI(p_Creature);
        }
};

/*######
## spell_quest_fear_no_evil
######*/

class spell_quest_fear_no_evil: public SpellScriptLoader
{
    public:
        spell_quest_fear_no_evil() : SpellScriptLoader("spell_quest_fear_no_evil") { }

        class spell_quest_fear_no_evil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_quest_fear_no_evil_SpellScript);

            void OnDummy(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster())
                    if (GetCaster()->ToPlayer())
                        GetCaster()->ToPlayer()->KilledMonsterCredit(50047, 0);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_quest_fear_no_evil_SpellScript::OnDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_quest_fear_no_evil_SpellScript();
        }
};

/*######
## spell_quest_extincteur
######*/

#define SPELL_VISUAL_EXTINGUISHER   96028

class spell_quest_extincteur: public SpellScriptLoader
{
    public:
        spell_quest_extincteur() : SpellScriptLoader("spell_quest_extincteur") { }

        class spell_quest_extincteur_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_quest_extincteur_AuraScript);

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    GetCaster()->AddAura(SPELL_VISUAL_EXTINGUISHER, GetCaster());
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (!GetCaster())
                    return;

                if (Creature* fire = GetCaster()->FindNearestCreature(42940, 5.0f, true))
                {
                    if (Player* player = GetCaster()->ToPlayer())
                        player->KilledMonsterCredit(42940, 0);

                    fire->ForcedDespawn();
                }
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (GetCaster())
                    GetCaster()->RemoveAurasDueToSpell(SPELL_VISUAL_EXTINGUISHER);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_quest_extincteur_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_quest_extincteur_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_quest_extincteur_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_quest_extincteur_AuraScript();
        }
};

#ifndef __clang_analyzer__
void AddSC_elwyn_forest()
{
    new npc_stormwind_infantry();
    new npc_stormwind_injured_soldier();
    new npc_training_dummy_start_zones();
    new npc_hags_broom_51710();
    new spell_quest_fear_no_evil();
    new spell_quest_extincteur();
}
#endif
