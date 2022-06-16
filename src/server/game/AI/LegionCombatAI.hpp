////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef LEGIONCOMBATAI_HPP
# define LEGIONCOMBATAI_HPP

# include "ScriptedCreature.h"

enum eCombatAIEventsChecks
{
    MeleeNegative               = 0,
    RangedNegative              = 1,
    AoENegative                 = 2,
    RangedPositive              = 3,
    AoEPositive                 = 4,    ///< Alias: TargetSelf
    RangedHeal                  = 5,
    AoEHeal                     = 6,
    NonTankNegative             = 7,
    AoENonSelfPositive          = 8,
    MeleeTopAggro               = 9,
    AoENonSelfPositiveDispel    = 10,

    EventCheckMax
};

enum eEventFlags
{
    DisableTurn             = 0x1,
    CastAfterDeath          = 0x2,
    CastOnDeath             = 0x4,
    CastUnderHealthPct      = 0x8,
    DespawnAfterCast        = 0x10,
    DontCheckInCombat       = 0x20,
    DieAfterCast            = 0x40,
    DespawnSummonsAfterDie  = 0x80,
    DespawnAreaTriggers     = 0x100,
    CastAfterSpell          = 0x200,
    DisableMove             = 0x400,
    DontCastIfTankIsGone    = 0x800,
};

class LegionCombatAI : public ScriptedAI
{
    public:
        enum eCombatAITalks
        {
            Death = 0,
            Aggro = 1
        };

        struct EventContextData
        {
            bool Prevent = false;
        };

        explicit LegionCombatAI(Creature* p_Creature) : ScriptedAI(p_Creature),
            m_CanDie(false), m_LastUpdate(0), m_HasOffCombatEvents(false) {}

        void InitializeAI();
        virtual void Reset();
        void AttackStart(Unit* p_Victim, bool p_Melee = false);
        void UpdateAI(uint32 const p_Diff);
        void EnterCombat(Unit* p_Who);
        bool LaunchEvent(uint32 l_EventID, Unit* p_TopAggro = nullptr);
        virtual bool ExecuteEvent(uint32 p_EventID, Unit* p_Target);
        virtual bool AdditionalChecks(uint32 p_EventID) { return true; }
        void AddEvent(CombatAIEventData p_EventData) { m_EventData.push_back(p_EventData); m_EventContextData.push_back({}); }
        void ClearEvents() { m_EventData.clear(); m_EventContextData.clear(); }
        void OnSpellFinished(SpellInfo const* p_SpellInfo);
        void JustDied(Unit* p_Killer);
        void DamageTaken(Unit* p_Source, uint32& p_Damage, SpellInfo const* p_SpellInfo);
        virtual Unit* GetTarget(std::list<Unit*>& p_TargetList, uint32 p_EventID);
        typedef std::list<Unit*>(LegionCombatAI::*f_Check)(uint32 p_EventID);
        virtual bool CanChase() { return true; }
        virtual bool CanAutoAttack() { return true; }
        CombatAIEventData const& GetEventData(uint32 p_EventId) { return m_EventData[p_EventId - 1]; }

        std::list<Unit*> CheckMeleeNegative(uint32 p_EventID);
        std::list<Unit*> CheckRangedNegative(uint32 p_EventID);
        std::list<Unit*> CheckAoENegative(uint32 p_EventID);
        std::list<Unit*> CheckRangedPositive(uint32 p_EventID);
        std::list<Unit*> CheckAoEPositive(uint32 p_EventID);
        std::list<Unit*> CheckRangedHeal(uint32 p_EventID);
        std::list<Unit*> CheckAoEHeal(uint32 p_EventID);
        std::list<Unit*> CheckNonTankNegative(uint32 p_EventID);
        std::list<Unit*> CheckAoENonSelfPositive(uint32 p_EventID);
        std::list<Unit*> CheckMeleeTopAggro(uint32 p_EventID);
        std::list<Unit*> CheckAoENonSelfPositiveDispel(uint32 p_EventID);

        static int Permissible(const Creature*) { return PERMIT_BASE_NO; }

        void ResetEvents();

    private:
        std::vector<CombatAIEventData> m_EventData;
        std::vector<EventContextData> m_EventContextData;

        bool m_CanDie;
        uint32 m_LastUpdate;
        bool m_HasOffCombatEvents;
};

#endif ///< LEGIONCOMBATAI_HPP
