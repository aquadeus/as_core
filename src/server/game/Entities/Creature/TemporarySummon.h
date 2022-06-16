////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TEMPSUMMON_H
#define TEMPSUMMON_H

#include "Creature.h"

enum StatsIncreaseType
{
    INCREASE_HEALTH_PERCENT         = 1,
    INCREASE_MELEE_DAMAGE_PERCENT   = 2,
    INCREASE_RANGED_DAMAGE_PERCENT  = 3,
    INCREASE_ARMOR_PERCENT          = 13,
    INCREASE_MAGIC_DAMAGE_PERCENT   = 24
};

struct PetStatInfo;

enum SummonActionType
{
    SUMMON_ACTION_TYPE_DEFAULT,
    SUMMON_ACTION_TYPE_ROUND_HOME_POS,
    SUMMON_ACTION_TYPE_ROUND_SUMMONER
};

/// Stores data for temp summons
struct TempSummonData
{
    uint32 entry;           ///< Entry of summoned creature
    Position pos;           ///< Position, where should be creature spawned
    TempSummonType type;    ///< Summon type, see TempSummonType for available types
    uint8 count;            ///< Summon count for non default action
    uint8 actionType;       ///< Summon action type, option for any summon options
    uint32 time;            ///< Despawn time, usable only with certain temp summon types
    float distance;         ///< Distance from caster for non default action
};

class TempSummon : public Creature
{
    public:
        explicit TempSummon(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        virtual ~TempSummon() {}
        void Update(uint32 time);
        virtual void InitStats(uint32 lifetime, Player* p_PlayerSummoner = nullptr);
        virtual void InitSummon();
        virtual void UnSummon(uint32 msTime = 0);
        void RemoveFromWorld();
        void SetTempSummonType(TempSummonType type);
        void SaveToDB(uint32 /*mapid*/, uint32 /*spawnMask*/, uint32 /*phaseMask*/) {}
        Unit* GetSummoner() const;
        uint64 GetSummonerGUID() const { return m_summonerGUID; }
        uint64 GetSummonerObjectGUID() const { return m_summonerWObjectGUID; }
        TempSummonType const& GetSummonType() { return m_type; }
        uint32 GetTimer() { return m_timer; }
        void SetDuration(uint32 p_Duration);
        void SetSummonerObjectGuid(uint64 guid) { m_summonerWObjectGUID = guid; }

        const SummonPropertiesEntry* const m_Properties;
    private:
        TempSummonType m_type;
        uint32 m_timer;
        uint32 m_lifetime;
        uint64 m_summonerGUID;
        uint64 m_summonerWObjectGUID;
};

class Minion : public TempSummon
{
    public:
        Minion(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration, Player* p_PlayerSummoner = nullptr);
        void RemoveFromWorld();
        float GetFollowAngle() const { return m_followAngle; }
        void SetFollowAngle(float angle) { m_followAngle = angle; }
        bool IsPetGhoulOrAbomination() const { return GetEntry() == DeathKnightPet::Ghoul || GetEntry() == DeathKnightPet::Abomination;} // Ghoul may be guardian or pet
        bool IsPetGargoyle() const { return GetEntry() == DeathKnightPet::Gargoyle; }
        bool IsTreant() const { return GetEntry() == TreantGuardian || GetEntry() == TreantFeral || GetEntry() == TreantBalance || GetEntry() == TreantRestoration; }
        bool IsGuardianPet() const;
    protected:
        float m_followAngle;
};

class Guardian : public Minion
{
    public:
        Guardian(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration, Player* p_PlayerSummoner = nullptr);
        bool InitStatsForLevel(uint8 level);
        void InitSummon();

        bool UpdateStats(Stats stat);
        bool UpdateAllStats();
        void UpdateResistances(uint32 school);
        void UpdateArmor();
        void UpdateMaxHealth();
        void UpdateMaxPower(Powers power);
        void UpdateAttackPowerAndDamage(bool ranged = false);
        void UpdateDamagePhysical(WeaponAttackType attType, bool l_NoLongerDualWields = false);

        PetStatInfo const* GetPetStat(bool p_Force = false) const;

    protected:
        int32   m_bonusSpellDamage;
        float   m_statFromOwner[MAX_STATS];
};

class Puppet : public Minion
{
    public:
        Puppet(SummonPropertiesEntry const* properties, Unit* owner);
        void InitStats(uint32 duration, Player* p_PlayerSummoner = nullptr);
        void InitSummon();
        void Update(uint32 time);
        void RemoveFromWorld();
    protected:
        Player* m_owner;
};

class ForcedUnsummonDelayEvent : public BasicEvent
{
public:
    ForcedUnsummonDelayEvent(TempSummon& owner) : BasicEvent(), m_owner(owner) { }
    bool Execute(uint64 e_time, uint32 p_time);

private:
    TempSummon& m_owner;
};
#endif
