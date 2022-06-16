////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_CREATUREAI_H
#define TRINITY_CREATUREAI_H

#include "Creature.h"
#include "UnitAI.h"
#include "Common.h"
#include "InstanceScript.h"
#include "Player.h"
#include "WorldQuestMgr.h"

class WorldObject;
class Unit;
class Creature;
class Player;
class SpellInfo;

struct SpellDestination;

#define TIME_INTERVAL_LOOK   5000
#define VISIBILITY_RANGE    10000

//Spell targets used by SelectSpell
enum SelectTargetType
{
    SELECT_TARGET_DONTCARE = 0,                             //All target types allowed

    SELECT_TARGET_SELF,                                     //Only Self casting

    SELECT_TARGET_SINGLE_ENEMY,                             //Only Single Enemy
    SELECT_TARGET_AOE_ENEMY,                                //Only AoE Enemy
    SELECT_TARGET_ANY_ENEMY,                                //AoE or Single Enemy

    SELECT_TARGET_SINGLE_FRIEND,                            //Only Single Friend
    SELECT_TARGET_AOE_FRIEND,                               //Only AoE Friend
    SELECT_TARGET_ANY_FRIEND                                //AoE or Single Friend
};

//Spell Effects used by SelectSpell
enum SelectEffect
{
    SELECT_EFFECT_DONTCARE = 0,                             //All spell effects allowed
    SELECT_EFFECT_DAMAGE,                                   //Spell does damage
    SELECT_EFFECT_HEALING,                                  //Spell does healing
    SELECT_EFFECT_AURA                                      //Spell applies an aura
};

enum SCEquip
{
    EQUIP_NO_CHANGE = -1,
    EQUIP_UNEQUIP   = 0
};

enum CreatureSummonGroup
{
    CREATURE_SUMMON_GROUP_RESET,
    CREATURE_SUMMON_GROUP_COMBAT
};

class CreatureAI : public UnitAI
{
    protected:
        Creature* const me;

        bool UpdateVictim();
        bool UpdateVictimRanged();
        bool UpdateVictimWithGaze();

        void SetGazeOn(Unit* target);

        Creature* DoSummon(uint32 entry, Position const& pos, uint32 despawnTime = 30000, TempSummonType summonType = TEMPSUMMON_CORPSE_TIMED_DESPAWN);
        Creature* DoSummon(uint32 entry, WorldObject* obj, float radius = 5.0f, uint32 despawnTime = 30000, TempSummonType summonType = TEMPSUMMON_CORPSE_TIMED_DESPAWN);
        Creature* DoSummonFlyer(uint32 entry, WorldObject* obj, float flightZ, float radius = 5.0f, uint32 despawnTime = 30000, TempSummonType summonType = TEMPSUMMON_CORPSE_TIMED_DESPAWN);

    public:
        void Talk(uint8 id, uint64 WhisperGuid = 0, uint32 range = 0);
        void PersonalTalk(uint8 p_Id, uint64 p_ReceiverGuid, uint32 p_Delay = 0, uint32 p_Range = 0);
        void WhisperToPlayer(uint8 p_Id, uint64 p_WhisperGuid);
        void DelayTalk(uint32 p_DelayTime, uint8 p_ID, uint64 p_WhisperGuid = 0, uint32 p_Range = 0);
        void ZoneTalk(uint8 p_ID, uint64 p_WhipserGuid = 0);
        explicit CreatureAI(Creature* creature) : UnitAI(creature), me(creature), m_MoveInLineOfSight_locked(false), m_canSeeEvenInPassiveMode(false) {}

        virtual ~CreatureAI() {}

        /// == Reactions At =================================

        // Called if IsVisible(Unit* who) is true at each who move, reaction at visibility zone enter
        void MoveInLineOfSight_Safe(Unit* who);

        bool CanSeeEvenInPassiveMode() { return m_canSeeEvenInPassiveMode; }
        void SetCanSeeEvenInPassiveMode(bool canSeeEvenInPassiveMode) { m_canSeeEvenInPassiveMode = canSeeEvenInPassiveMode; }

        // Called in Creature::Update when deathstate = DEAD. Inherited classes may maniuplate the ability to respawn based on scripted events.
        virtual bool CanRespawn() { return true; }

        // Called for reaction at stopping attack at no attackers or targets
        virtual void EnterEvadeMode();

        // Called for reaction at enter to combat if not in combat yet (enemy can be NULL)
        virtual void EnterCombat(Unit* p_Attacker)
        {
            UNUSED(p_Attacker);
        }

        // Called when the creature is despawned
        virtual void JustDespawned() {}

        // Called when the creature is killed
        virtual void JustDied(Unit* p_Killer)
        {
            UNUSED(p_Killer);
        }

        // Called when the creature kills a unit
        virtual void KilledUnit(Unit* p_Killed)
        {
            UNUSED(p_Killed);
        }

        // Called when the killer is rewarded with currencies
        virtual void CurrenciesRewarder(bool& p_Result)
        {
            UNUSED(p_Result);
        }

        virtual void StartCosmeticEvent(CosmeticEventEntry const& p_Entry, int32 p_Duration)
        {
            UNUSED(p_Entry);
            UNUSED(p_Duration);
        }

        virtual bool IgnoreFlyingMovementFlagsUpdate() { return false; }

        virtual bool CanScale() const { return true; }

        /// Called when check interact distance (for example: gossips)
        virtual bool CanByPassDistanceCheck() const { return false; }

        // Called when check LOS
        virtual bool CanBeTargetedOutOfLOS() { return false; }
        virtual bool CanTargetOutOfLOS() { return false; }
        virtual bool CanBeTargetedOutOfLOSXYZ(float l_X, float l_Y, float l_Z)
        {
            UNUSED(l_X);
            UNUSED(l_Y);
            UNUSED(l_Z);

            return false;
        }
        virtual bool CanTargetOutOfLOSXYZ(float l_X, float l_Y, float l_Z)
        {
            UNUSED(l_X);
            UNUSED(l_Y);
            UNUSED(l_Z);

            return false;
        }

        virtual bool CanFly() { return true; }

        /// Called when visibility is checked
        virtual bool CanBeSeenByPlayers() const;

        /// Called during stealth detection calculations
        virtual bool CanBeSeenForced() const { return false; }

        /// Called in Player::IsAtGroupRewardDistance
        virtual bool BypassXPDistanceIfNeeded(WorldObject* p_Object)
        {
            UNUSED(p_Object);

            return false;
        }

        // Called when the creature summon successfully other creature
        virtual void JustSummoned(Creature* p_Summon)
        {
            UNUSED(p_Summon);
        }

        /// Called when the creature successfully summons another creature with a cosmetic_event
        virtual void JustSummonedCosmetic(Creature* p_Summon, uint8 p_EventID)
        {
            UNUSED(p_Summon);
            UNUSED(p_EventID);
        }

        virtual void IsSummonedBy(Unit* p_Summoner)
        {
            UNUSED(p_Summoner);
        }

        virtual void IsSummonedBy(WorldObject* object) {}

        virtual void BeforeJustDied(Unit* killer) {}

        virtual void IsSummonedByGob(GameObject const* p_GameObject)
        {
            UNUSED(p_GameObject);
        }

        virtual void AfterSummon(Unit* p_Summoner, Unit* p_Target, uint32 p_SpellId)
        {
        }

        /// Called when the creature summons successfully AreaTriggers
        virtual void AreaTriggerCreated(AreaTrigger* p_AreaTrigger)
        {
            UNUSED(p_AreaTrigger);
        }

        /// Called when an AreaTrigger summoned by the creature despawns
        virtual void AreaTriggerDespawned(AreaTrigger* p_AreaTrigger, bool p_Removed)
        {
            UNUSED(p_AreaTrigger);
        }

        /// Called when an AreaTrigger is about to cast a spell on a unit
        virtual bool CheckAreaTriggerCastAction(AreaTrigger* p_AreaTrigger, Unit* p_Unit, SpellInfo const* p_SpellInfo)
        {
            UNUSED(p_AreaTrigger);
            UNUSED(p_Unit);
            UNUSED(p_SpellInfo);

            return true;
        }

        virtual void SummonedCreatureDespawn(Creature* p_Summon)
        {
            UNUSED(p_Summon);
        }

        virtual void SummonedCreatureDies(Creature* p_Summon, Unit* p_Killer)
        {
            UNUSED(p_Summon);
            UNUSED(p_Killer);
        }

        // Called when hit by a spell
        virtual void SpellHit(Unit* p_Caster, SpellInfo const* p_SpellInfo)
        {
            UNUSED(p_Caster);
            UNUSED(p_SpellInfo);
        }

        /// Called when the creature summon successfully gameobject
        virtual void JustSummonedGO(GameObject* p_GameObject)
        {
            UNUSED(p_GameObject);
        }

        // Called when spell hits a target
        virtual void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo)
        {
            UNUSED(p_Target);
            UNUSED(p_SpellInfo);
        }

        /// Called when spell hits a destination
        virtual void SpellHitDest(SpellDestination const* p_Dest, SpellInfo const* p_SpellInfo, SpellEffectHandleMode p_Mode)
        {
            UNUSED(p_Dest);
            UNUSED(p_SpellInfo);
            UNUSED(p_Mode);
        }

        /// Called when spell miss a target
        virtual void SpellMissTarget(Unit* p_Target, SpellInfo const* p_SpellInfo, SpellMissInfo p_MissInfo)
        {
            UNUSED(p_Target);
            UNUSED(p_SpellInfo);
            UNUSED(p_MissInfo);
        }

        /// Called when successful cast a spell
        virtual void OnSpellCasted(SpellInfo const* p_SpellInfo)
        {
            UNUSED(p_SpellInfo);
        }

        /// Called when a spell is finished
        virtual void OnSpellFinished(SpellInfo const* p_SpellInfo)
        {
            UNUSED(p_SpellInfo);
        }

        /// Called when a spell is finished with Success
        virtual void OnSpellFinishedSuccess(SpellInfo const* p_SpellInfo)
        {
            UNUSED(p_SpellInfo);
        }

        virtual void OnSpellInterrupted(SpellInfo const* p_SpellInfo)
        {
            UNUSED(p_SpellInfo);
        }

        /// Called when an aura is removed
        virtual void OnRemoveAura(uint32 p_SpellID, AuraRemoveMode p_RemoveMode)
        {
            UNUSED(p_SpellID);
            UNUSED(p_RemoveMode);
        }

        /// Called when an aura is added
        virtual void OnAddAura(Aura* p_AuraApplication, bool p_OnReapply)
        {
            UNUSED(p_AuraApplication);
            UNUSED(p_OnReapply);
        }

        // Called when the creature is target of hostile action: swing, hostile spell landed, fear/etc)
        //virtual void AttackedBy(Unit* attacker);
        virtual bool IsEscorted() { return false; }
        virtual bool IsPassived() { return false; }

        // Called when creature is spawned or respawned (for reseting variables)
        virtual void JustRespawned() { Reset(); }

        /// Called at waypoint finished
        virtual void LastWPReached() { }

        // Called at waypoint reached or point movement finished
        virtual void MovementInform(uint32 p_Type, uint32 p_ID)
        {
            UNUSED(p_Type);
            UNUSED(p_ID);
        }

        void OnCharmed(bool apply);

        /// Enable/Disable fly mode
        void SetFlyMode(bool fly);

        /// Enable/Disable all classical immunities
        void ApplyAllImmunities(bool p_Apply);

        // Called at reaching home after evade
        virtual void JustReachedHome() { }

        void DoZoneInCombat(Creature* creature = NULL, float maxRangeToNearestTarget = 50.0f);
        void DoAttackerAreaInCombat(Unit* attacker, float range, Unit* pUnit = NULL);

        // Called at text emote receive from player
        virtual void ReceiveEmote(Player* p_Player, uint32 p_EmoteID)
        {
            UNUSED(p_Player);
            UNUSED(p_EmoteID);
        }

        // Called when owner takes damage
        virtual void OwnerDamagedBy(Unit* p_Attacker)
        {
            UNUSED(p_Attacker);
        }

        // Called when owner attacks something
        virtual void OwnerAttacked(Unit* p_Target)
        {
            UNUSED(p_Target);
        }

        /// Called when owner gets interrupted
        virtual void OwnerInterrupted(SpellInfo const* p_SpellInfo)
        {
            UNUSED(p_SpellInfo);
        }

        // Called when a creature regen one of his power
        virtual void RegeneratePower(Powers p_Power, int32& p_Value)
        {
            UNUSED(p_Power);
            UNUSED(p_Value);
        }

        // Called when a creature has one of hi power modified
        virtual void PowerModified(Powers p_Power, int32 p_Value)
        {
            UNUSED(p_Power);
            UNUSED(p_Value);
        }

        // Called when a creature has one of his power set
        virtual void SetPower(Powers p_Power, int32 p_Value)
        {
            UNUSED(p_Power);
            UNUSED(p_Value);
        }

        /// Called when taunted
        virtual void OnTaunt(Unit* p_Taunter)
        {
            UNUSED(p_Taunter);
        }

        /// Called when a casting time is calculated
        virtual void OnCalculateCastingTime(SpellInfo const* p_SpellInfo, int32& p_CastingTime)
        {
            UNUSED(p_SpellInfo);
            UNUSED(p_CastingTime);
        }

        // Called at any threat added from any attacker (before threat apply)
        virtual void OnAddThreat(Unit* p_Victim, float& p_Threat, SpellSchoolMask p_SchoolMask, SpellInfo const* p_ThreatSpell)
        {
            UNUSED(p_Victim);
            UNUSED(p_Threat);
            UNUSED(p_SchoolMask);
            UNUSED(p_ThreatSpell);
        }

        /// Called After melee attack done
        virtual void OnMeleeAttackDone(Unit* p_Victim, DamageInfo* p_DamageInfo, WeaponAttackType p_AttackType)
        {
            UNUSED(p_Victim);
            UNUSED(p_DamageInfo);
            UNUSED(p_AttackType);
        }

        /// Called when calculating melee damage done
        virtual void OnCalculateMeleeDamage(Unit* p_Victim, uint32* p_Damage, DamageInfo* p_DamageInfo)
        {
            UNUSED(p_Victim);
            UNUSED(p_Damage);
            UNUSED(p_DamageInfo);
        }

        /// Called when at HandleGarrisonGetShipmentInfoOpcode() is received
        virtual int OnShipmentIDRequest(Player* p_Player)
        {
            UNUSED(p_Player);
            return -1;
        }

        /// == Triggered Actions Requested ==================

        // Called when creature attack expected (if creature can and no have current victim)
        // Note: for reaction at hostile action must be called AttackedBy function.
        //virtual void AttackStart(Unit*) {}

        // Called at World update tick
        //virtual void UpdateAI(const uint32 /*diff*/) {}

        /// == State checks =================================

        // Is unit visible for MoveInLineOfSight
        //virtual bool IsVisible(Unit*) const { return false; }

        // called when the corpse of this creature gets removed
        virtual void CorpseRemoved(uint32& p_RespawnDelay)
        {
            UNUSED(p_RespawnDelay);
        }

        // Called when victim entered water and creature can not enter water
        //virtual bool canReachByRangeAttack(Unit*) { return false; }

        /// == Fields =======================================

        // Pointer to controlled by AI creature
        //Creature* const me;

        virtual void PassengerBoarded(Unit* p_Passenger, int8 p_SeatID, bool p_Apply)
        {
            UNUSED(p_Passenger);
            UNUSED(p_SeatID);
            UNUSED(p_Apply);
        }

        virtual void OnVehicleExited(Unit* p_Vehicle)
        {
            UNUSED(p_Vehicle);
        }

        virtual void OnVehicleEntered(Unit* p_Vehicle)
        {
            UNUSED(p_Vehicle);
        }

        /// Called before _ExitVehicle with ExitPos parameter
        virtual void OnExitVehicle(Unit* p_Vehicle, Position& p_ExitPos)
        {
            UNUSED(p_Vehicle);
            UNUSED(p_ExitPos);
        }

        ///< Called before spellclick effects will be casted
        virtual bool CanUseSpellClick(Unit*  p_Clicker, uint32 p_SpellClicked)
        {
            UNUSED(p_Clicker);
            return true;
        }

        virtual void OnSpellClick(Unit* p_Clicker)
        {
            UNUSED(p_Clicker);
        }

        virtual void NonInterruptCast(Unit* p_Caster, uint32 p_SpellID, uint32 p_SchoolMask)
        {
            UNUSED(p_Caster);
            UNUSED(p_SpellID);
            UNUSED(p_SchoolMask);
        }

        virtual void OnAreaTriggerCast(Unit* p_Caster, Unit* p_Target, uint32 p_SpellID)
        {
            UNUSED(p_Caster);
            UNUSED(p_Target);
            UNUSED(p_SpellID);
        }

        virtual void OnInterruptCast(Unit* p_Caster, SpellInfo const* p_SpellInfo, SpellInfo const* p_CurrSpellInfo, uint32 p_SchoolMask)
        {
            UNUSED(p_Caster);
            UNUSED(p_SpellInfo);
            UNUSED(p_CurrSpellInfo);
            UNUSED(p_SchoolMask);
        }

        virtual bool CanSeeAlways(WorldObject const* p_Object)
        {
            UNUSED(p_Object);
            return false;
        }

        /// Called when the attack distance range is calculated
        virtual void OnCalculateAttackDistance(float& p_AttackDistance)
        {
            UNUSED(p_AttackDistance);
        }

        /// Called when movement velocity is calculated
        virtual void OnCalculateMoveSpeed(float& p_Velocity)
        {
            UNUSED(p_Velocity);
        }

        /// Called when Creature::_GetHealthModForDiff() is called
        virtual void GetMythicHealthMod(float& p_Modifier)
        {
            UNUSED(p_Modifier);
        }

        /// Called when Creature::_GetHealthModPersonal is called
        virtual bool GetHealthModPersonal(float& p_Modifier, uint32 p_Count)
        {
            UNUSED(p_Modifier);
            UNUSED(p_Count);

            return false;
        }

        /// Called when selecting AoE targets
        virtual void FilterTargets(std::list<WorldObject*>& p_Targets, Spell const* p_Spell, SpellEffIndex p_EffIndex)
        {
            UNUSED(p_Targets);
            UNUSED(p_Spell);
            UNUSED(p_EffIndex);
        }

        /// Called when someone is stealing some buffs, return true to bypass the steal of the spell
        virtual bool OnSpellSteal(Unit* p_Caster, uint32 p_SpellID)
        {
            UNUSED(p_Caster);
            UNUSED(p_SpellID);

            return false;
        }

        /// Called when Unit::CalculateDamage and Creature::UpdateDamagePhysical
        virtual void GetDamageMultiplier(float& p_Modifier)
        {
            UNUSED(p_Modifier);
        }

        /// Called when Unit::CanHaveThreatList is called
        virtual bool CanHaveThreatList() const
        {
            return true;
        }

        void UpdateOperations(uint32 const p_Diff);

        /// Add timed delayed operation
        /// @p_Timeout  : Delay time
        /// @p_Function : Callback function
        void AddTimedDelayedOperation(uint32 p_Timeout, std::function<void()> && p_Function)
        {
            m_EmptyWarned = false;
            m_TimedDelayedOperations.push_back(std::pair<uint32, std::function<void()>>(p_Timeout, p_Function));
        }

        /// Deletes all pending timed delayed operations
        void DeleteOperations()
        {
            m_TimedDelayedOperations.clear();
        }

        /// Called after last delayed operation was deleted
        /// Do whatever you want
        virtual void LastOperationCalled() { }

        void ClearDelayedOperations()
        {
            m_TimedDelayedOperations.clear();
            m_EmptyWarned = false;
        }

        bool IsInDisable();
        bool IsInControl();

        std::list<std::pair<int32, std::function<void()>>>      m_TimedDelayedOperations;   ///< Delayed operations
        bool                                                    m_EmptyWarned;              ///< Warning when there are no more delayed operations

    protected:
        virtual void MoveInLineOfSight(Unit* p_Who);

        bool _EnterEvadeMode();

    private:
        bool m_MoveInLineOfSight_locked;
        bool m_canSeeEvenInPassiveMode;
};

enum Permitions
{
    PERMIT_BASE_NO                 = -1,
    PERMIT_BASE_IDLE               = 1,
    PERMIT_BASE_REACTIVE           = 100,
    PERMIT_BASE_PROACTIVE          = 200,
    PERMIT_BASE_FACTION_SPECIFIC   = 400,
    PERMIT_BASE_SPECIAL            = 800
};

#endif
