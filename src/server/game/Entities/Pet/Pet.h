////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef PET_H
#define PET_H

#include "ObjectDefines.h"
#include "Unit.h"
#include "TemporarySummon.h"

enum PetType
{
    SUMMON_PET              = 0,
    HUNTER_PET              = 1,
    MAX_PET_TYPE            = 4,
};

enum PetSpellState
{
    PETSPELL_UNCHANGED = 0,
    PETSPELL_CHANGED   = 1,
    PETSPELL_NEW       = 2,
    PETSPELL_REMOVED   = 3
};

enum PetSpellType
{
    PETSPELL_NORMAL = 0,
    PETSPELL_FAMILY = 1,
    PETSPELL_TALENT = 2,
};

struct PetSpell
{
    ActiveStates active;
    PetSpellState state;
    PetSpellType type;
};

enum ActionFeedback
{
    FEEDBACK_NONE            = 0,
    FEEDBACK_PET_DEAD        = 1,
    FEEDBACK_NOTHING_TO_ATT  = 2,
    FEEDBACK_CANT_ATT_TARGET = 3
};

enum PetTalk
{
    PET_TALK_SPECIAL_SPELL  = 0,
    PET_TALK_ATTACK         = 1
};

enum PetNameInvalidReason
{
    // custom, not send
    PET_NAME_SUCCESS                                        = 0,

    PET_NAME_INVALID                                        = 1,
    PET_NAME_NO_NAME                                        = 2,
    PET_NAME_TOO_SHORT                                      = 3,
    PET_NAME_TOO_LONG                                       = 4,
    PET_NAME_MIXED_LANGUAGES                                = 6,
    PET_NAME_PROFANE                                        = 7,
    PET_NAME_RESERVED                                       = 8,
    PET_NAME_THREE_CONSECUTIVE                              = 11,
    PET_NAME_INVALID_SPACE                                  = 12,
    PET_NAME_CONSECUTIVE_SPACES                             = 13,
    PET_NAME_RUSSIAN_CONSECUTIVE_SILENT_CHARACTERS          = 14,
    PET_NAME_RUSSIAN_SILENT_CHARACTER_AT_BEGINNING_OR_END   = 15,
    PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME              = 16
};

typedef std::unordered_map<uint32, PetSpell> PetSpellMap;
typedef std::vector<uint32> AutoSpellList;

#define ACTIVE_SPELLS_MAX           4

#define PET_FOLLOW_DIST  1.0f
#define PET_FOLLOW_ANGLE (M_PI/2)
#define PET_FOCUS_REGEN_INTERVAL 1 * IN_MILLISECONDS

class Player;
class PetQueryHolder;

class Pet : public Guardian
{
    public:
        explicit Pet(Player* owner, PetType type = MAX_PET_TYPE);
        virtual ~Pet();

        void AddToWorld();
        void RemoveFromWorld();

        PetType getPetType() const { return m_petType; }
        void setPetType(PetType type) { m_petType = type; }
        bool isControlled() const { return getPetType() == SUMMON_PET || getPetType() == HUNTER_PET; }
        bool isTemporarySummoned() const { return m_duration > 0; }

        bool IsPermanentPetFor(Player* owner);              // pet have tab in character windows and set UNIT_FIELD_PETNUMBER

        bool Create (uint32 p_Guidlow, Map* p_Map, uint32 p_PhaseMask, std::set<uint32> const& p_Phases, uint32 p_Entry);
        bool CreateBaseAtCreature(Creature* p_Creature, std::set<uint32> p_Phases);
        bool CreateBaseAtCreatureInfo(CreatureTemplate const* cinfo, Unit* owner);
        bool CreateBaseAtTamed(CreatureTemplate const* cinfo, Map* map, uint32 phaseMask, std::set<uint32> const& p_Phases);
        void LoadPetFromDB(Player* owner, uint32 petentry = 0, uint32 petnumber = 0, bool current = false, PetSlot slotID = PET_SLOT_UNK_SLOT, bool stampeded = false, PetQueryHolder* holder = nullptr, std::function<void(Pet*, bool)> p_Callback = [](Pet*, bool){});
        bool isBeingLoaded() const { return m_loading;}
        void SavePetToDB(PetSlot mode, bool stampeded = false, bool ownerLogout = false);
        void Remove(PetSlot mode, bool returnreagent = false, bool stampeded = false);
        static void DeleteFromDB(uint32 guidlow, uint32 realmid);

        void setDeathState(DeathState s);                   ///< Overwrite virtual Creature::setDeathState and Unit::setDeathState
        void Update(uint32 diff);                           ///< Overwrite virtual Creature::Update and Unit::Update

        uint8 GetPetAutoSpellSize() const { return m_autospells.size(); }
        uint32 GetPetAutoSpellOnPos(uint8 pos) const
        {
            if (pos >= m_autospells.size())
                return 0;
            else
                return m_autospells[pos];
        }

        void SetSlot(uint8 slot) { m_slot = slot; }
        uint8 GetSlot() { return m_slot; }

        void GivePetXP(uint32 xp);
        void GivePetLevel(uint8 level);
        void SynchronizeLevelWithOwner();
        bool HaveInDiet(ItemTemplate const* item) const;
        uint32 GetCurrentFoodBenefitLevel(uint32 itemlevel);
        void SetDuration(int32 dur) { m_duration = dur; }
        int32 GetDuration() { return m_duration; }

        void ToggleAutocast(SpellInfo const* spellInfo, bool apply);

        bool HasSpell(uint32 spell) const;

        void LearnPetPassives();
        void CastPetAuras(bool current);
        void CastPetAura(PetAura const* aura);
        bool IsPetAura(Aura const* aura);

        void _LoadSpellCooldowns(PreparedQueryResult result);
        void _SaveSpellCooldowns(SQLTransaction& trans);
        void _LoadAuras(PreparedQueryResult auraResult, PreparedQueryResult auraEffectResult, uint32 timediff);
        void _SaveAuras(SQLTransaction& trans);
        void _LoadSpells(PreparedQueryResult result);
        void _SaveSpells(SQLTransaction& trans);

        /// Add spell for the pet
        /// @p_SpellID       : Spell ID to add
        /// @p_ActiveState   : Spell activation state
        /// @p_PetSpellState : Spell state
        /// @p_PetSpellType  : Spell type (Normal, Family, Talent)
        bool AddSpell(uint32 p_SpellID, ActiveStates p_ActiveState = ActiveStates::ACT_DECIDE, PetSpellState p_PetSpellState = PetSpellState::PETSPELL_NEW, PetSpellType p_PetSpellType = PetSpellType::PETSPELL_NORMAL);
        /// Learn spell for the pet
        /// @p_SpellID : Spell ID to learn
        bool LearnSpell(uint32 p_SpellID);
        /// Learn spells for the pet
        /// @p_SpellsToLearn : Spell IDs to learn
        void LearnSpells(std::unordered_set<uint32> const& p_SpellsToLearn);

        /// Remove a spell for the pet
        /// @p_SpellID          : Spell ID to unlearn
        /// @p_LearnPrev        : Should learn old rank on delete ?
        /// @p_ClearActionBar   : Clean action bar at remove ?
        bool RemoveSpell(uint32 p_SpellID, bool p_LearnPrev, bool p_ClearActionBar = true);
        /// Unlearn a spell for the pet
        /// @p_SpellID          : Spell ID to unlearn
        /// @p_LearnPrev        : Should learn old rank on delete ?
        /// @p_ClearActionBar   : Clean action bar at remove ?
        bool UnlearnSpell(uint32 p_SpellID, bool p_LearnPrev, bool p_ClearActionBar = true);
        /// Unlearn a set for spell to the pet
        /// @p_SpellsToUnlearn  : Spells ID to unlearn
        /// @p_LearnPrev        : Should learn old rank on delete ?
        /// @p_ClearActionBar   : Clean action bar at remove ?
        void UnlearnSpells(std::unordered_set<uint32> const& p_SpellsToUnlearn, bool p_LearnPrev, bool p_ClearActionBar = true);

        void InitLevelupSpellsForLevel();

        void learnSpellHighRank(uint32 spellid);
        void CleanupActionBar();
        virtual void ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs, uint32 p_SpellID = 0);

        PetSpellMap     m_spells;
        AutoSpellList   m_autospells;
        bool            m_Stampeded;

        void InitPetCreateSpells();

        std::bitset<MAX_AURAS> const& GetAuraUpdateMaskForRaid() const { return m_auraRaidUpdateMask; }
        void SetAuraUpdateMaskForRaid(uint8 p_Slot, bool p_Apply) { m_auraRaidUpdateMask.set(p_Slot, p_Apply); }
        void ResetAuraUpdateMaskForRaid() { m_auraRaidUpdateMask.reset(); }

        DeclinedName const* GetDeclinedNames() const { return m_declinedname; }

        bool    m_removed;                                  // prevent overwrite pet state in DB at next Pet::Update if pet already removed(saved)

        Player* GetOwner() { return m_owner; }

        uint32 GetSpecialization() const { return m_specialization; }
        void SetSpecialization(uint16 spec);
        void LearnSpecializationSpells();
        void RemoveSpecializationSpells(bool clearActionBar);

        bool NeedToChase(Unit* p_Victim);

    protected:
        Player* m_owner;
        PetType m_petType;
        int32   m_duration;                                 // time until unsummon (used mostly for summoned guardians and not used for controlled pets)
        std::bitset<MAX_AURAS> m_auraRaidUpdateMask;
        bool    m_loading;
        uint32  m_RegenPowerTimer;
        uint32  m_specialization;
        uint8   m_slot;

        DeclinedName *m_declinedname;

    private:
        void SaveToDB(uint32, uint32, uint32)                // override of Creature::SaveToDB     - must not be called
        {
            ASSERT(false);
        }
        void DeleteFromDB()                                 // override of Creature::DeleteFromDB - must not be called
        {
            ASSERT(false);
        }
};
#endif
