////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_INSTANCE_DATA_H
#define TRINITY_INSTANCE_DATA_H

#include "ZoneScript.h"
#include "World.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "AchievementMgr.h"
#include "Reporter.hpp"
#include "EasyJSon.hpp"

#define OUT_SAVE_INST_DATA             sLog->outDebug(LOG_FILTER_TSCR, "Saving Instance Data for Instance %s (Map %d, Instance Id %d)", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_SAVE_INST_DATA_COMPLETE    sLog->outDebug(LOG_FILTER_TSCR, "Saving Instance Data for Instance %s (Map %d, Instance Id %d) completed.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA(a)          sLog->outDebug(LOG_FILTER_TSCR, "Loading Instance Data for Instance %s (Map %d, Instance Id %d). Input is '%s'", instance->GetMapName(), instance->GetId(), instance->GetInstanceId(), a)
#define OUT_LOAD_INST_DATA_COMPLETE    sLog->outDebug(LOG_FILTER_TSCR, "Instance Data Load for Instance %s (Map %d, Instance Id: %d) is complete.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA_FAIL        sLog->outError(LOG_FILTER_TSCR, "Unable to load Instance Data for Instance %s (Map %d, Instance Id: %d).", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())

class Map;
class Unit;
class Player;
class GameObject;
class Creature;
class Challenge;

typedef std::set<GameObject*> DoorSet;
typedef std::set<Creature*> MinionSet;

enum EncounterFrameType
{
    ENCOUNTER_FRAME_ENGAGE,
    ENCOUNTER_FRAME_DISENGAGE,
    ENCOUNTER_FRAME_UPDATE_PRIORITY,
    ENCOUNTER_FRAME_START_TIMER,
    ENCOUNTER_FRAME_START_OBJECTIVE,
    ENCOUNTER_FRAME_COMPLETE_OBJECTIVE,
    ENCOUNTER_FRAME_START,
    ENCOUNTER_FRAME_UPDATE_OBJECTIVE,
    ENCOUNTER_FRAME_END,
    ENCOUNTER_FRAME_IN_COMBAT_RESURRECTION,
    ENCOUNTER_FRAME_PHASE_SHIFT_CHANGED,
    ENCOUNTER_FRAME_GAIN_COMBAT_RESURRECTION_CHARGE,
    ENCOUNTER_FRAME_SET_ALLOWING_RELEASE
};

enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4,
    TO_BE_DECIDED = 5
};

enum DoorType
{
    DOOR_TYPE_ROOM          = 0,    // Door can open if encounter is not in progress
    DOOR_TYPE_PASSAGE       = 1,    // Door can open if encounter is done
    DOOR_TYPE_SPAWN_HOLE    = 2,    // Door can open if encounter is in progress, typically used for spawning places
    DOOR_TYPE_RANDOM        = 3,    // Door state is controlled by instance Script. Used for the Arcway Dungeon
    MAX_DOOR_TYPES
};

enum BoundaryType
{
    BOUNDARY_NONE = 0,
    BOUNDARY_N,
    BOUNDARY_S,
    BOUNDARY_E,
    BOUNDARY_W,
    BOUNDARY_NE,
    BOUNDARY_NW,
    BOUNDARY_SE,
    BOUNDARY_SW,
    BOUNDARY_MAX_X = BOUNDARY_N,
    BOUNDARY_MIN_X = BOUNDARY_S,
    BOUNDARY_MAX_Y = BOUNDARY_W,
    BOUNDARY_MIN_Y = BOUNDARY_E
};

typedef std::map<BoundaryType, float> BossBoundaryMap;

struct DoorData
{
    uint32 entry, bossId;
    DoorType type;
    uint32 boundary;
};

struct MinionData
{
    uint32 entry, bossId;
};

struct BossInfo
{
    BossInfo() : state(TO_BE_DECIDED) {}
    EncounterState state;
    DoorSet door[MAX_DOOR_TYPES];
    MinionSet minion;
    BossBoundaryMap boundary;
};

struct DoorInfo
{
    explicit DoorInfo(BossInfo* _bossInfo, DoorType _type, BoundaryType _boundary)
        : bossInfo(_bossInfo), type(_type), boundary(_boundary) {}
    BossInfo* bossInfo;
    DoorType type;
    BoundaryType boundary;
};

struct MinionInfo
{
    explicit MinionInfo(BossInfo* _bossInfo) : bossInfo(_bossInfo) {}
    BossInfo* bossInfo;
};

typedef std::multimap<uint32 /*entry*/, DoorInfo> DoorInfoMap;
typedef std::map<uint32 /*entry*/, MinionInfo> MinionInfoMap;
typedef std::map<int8, std::vector<uint64>> DamageManagerMap;
typedef std::map<uint64, int8> PullDamageManagerMap;

struct RosterData
{
    RosterData()
    {
        GuidLow     = 0;
        Level       = 0;
        Class       = 0;
        SpecID      = 0;
        Role        = 0;
        ItemLevel   = 0;
    }

    uint32      GuidLow;
    std::string Name;
    uint8       Level;
    uint8       Class;
    uint32      SpecID;
    uint32      Role;
    uint32      ItemLevel;
};

typedef std::vector<RosterData> RosterDatas;

struct EncounterDatas
{
    EncounterDatas()
    {
        Expansion       = 0;
        RealmID         = 0;
        GuildID         = 0;
        GuildFaction    = 0;
        MapID           = 0;
        EncounterID     = 0;
        DifficultyID    = 0;
        StartTime       = 0;
        CombatDuration  = 0;
        EndTime         = 0;
        Success         = 0;
        BattleRezCount  = 0;
        BloodlustCount  = 0;
        EncounterHealth = 0;
    }

    uint32      Expansion;
    uint32      RealmID;
    uint32      GuildID;
    uint32      GuildFaction;
    std::string GuildName;
    uint32      MapID;
    uint32      EncounterID;
    uint32      DifficultyID;
    uint32      StartTime;
    uint32      CombatDuration;
    uint32      EndTime;
    bool        Success;
    uint32      BattleRezCount;
    uint32      BloodlustCount;
    uint64      EncounterHealth;
    RosterDatas RosterDatas;
};

enum eInstanceSpells
{
    SpellDetermination          = 139068,
    SpellPvEEventsDebug         = 193450,
    /// Heroism, Bloodlust...
    ShamanSated                 = 57724,
    HunterInsanity              = 95809,
    MageTemporalDisplacement    = 80354,
    ShamanExhaustion            = 57723,
    Bloodlust                   = 2825,
    Heroism                     = 32182,
    TempralDisplacement         = 80354,
    AncientHysteria             = 90355,
    /// Some other spells that must be reseted
    SpellSurrenderToMadness     = 193223,
    SpellArmyOfTheDead          = 42650,
    SpellDHLastResortDebuff     = 209261, ///< Debuf from Vengeance DH talent 'Last Resort' 209258
    SpellDHMetamorphosis        = 191427,
    SpellTimeWarp               = 80353,
    SpellDivineShield           = 642,
    SpellLayOnHands             = 633,
    SpellHandOfProtection       = 150630,
    SpellIceBlock               = 145533,
    SpellNetherwinds            = 160452,
    SpellGardianOfAnciensKing   = 86659,
    SpellFortifyingBrew         = 115203,
    SpellZenMeditation          = 115176,
    SpellPainSuppression        = 33206,
    SpellFeralBerserk           = 106951,
    SpellIncarnationOfKing      = 102543,
    SpellSurvivalInstinct       = 61336,
    SpellIncarnationOfUrsoc     = 102558,
    SpellCelestialAlignement    = 194223,
    SpellIncarnationOfElune     = 102560,
    SpellTranquility            = 740,
    SpellIncarnationTreeOfLife  = 33891,
    SpellShadowBlades           = 121471,
    SpellDancingRunes           = 49028,
    MaxSpellResetInInstance     = 29,
    /// Battle resurrection spells
    Rebirth                     = 20484,
    Soulstone                   = 20707,
    RaiseAlly                   = 61999,
    EternalGuardian             = 126393,
    GiftOfChiJi                 = 159931,
    DustOfLife                  = 159956,
    ArchbishopBenedictus        = 211319,
    Trueshot                    = 193526,
    SindragosaFury              = 190778,
    MaxBattleResSpells          = 6,
    WakernersLoyality           = 236200,
    TormentedSouls              = 216695,
    DeadwindHarvester           = 216708,
    ReapSouls                   = 216698
};

uint32 const g_InstanceSpellsReset[eInstanceSpells::MaxSpellResetInInstance] =
{
    eInstanceSpells::SpellSurrenderToMadness,
    eInstanceSpells::SpellArmyOfTheDead,
    eInstanceSpells::Trueshot,
    eInstanceSpells::SindragosaFury,
    eInstanceSpells::SpellDHMetamorphosis,
    eInstanceSpells::SpellTimeWarp,
    eInstanceSpells::SpellIceBlock,
    eInstanceSpells::SpellDivineShield,
    eInstanceSpells::SpellLayOnHands,
    eInstanceSpells::SpellHandOfProtection,
    eInstanceSpells::SpellIceBlock,
    eInstanceSpells::AncientHysteria,
    eInstanceSpells::SpellNetherwinds,
    eInstanceSpells::SpellGardianOfAnciensKing,
    eInstanceSpells::SpellFortifyingBrew,
    eInstanceSpells::SpellZenMeditation,
    eInstanceSpells::SpellPainSuppression,
    eInstanceSpells::Bloodlust,
    eInstanceSpells::Heroism,
    eInstanceSpells::SpellFeralBerserk,
    eInstanceSpells::SpellIncarnationOfKing,
    eInstanceSpells::SpellSurvivalInstinct,
    eInstanceSpells::SpellIncarnationOfUrsoc,
    eInstanceSpells::SpellCelestialAlignement,
    eInstanceSpells::SpellIncarnationOfElune,
    eInstanceSpells::SpellTranquility,
    eInstanceSpells::SpellIncarnationTreeOfLife,
    eInstanceSpells::SpellShadowBlades,
    eInstanceSpells::SpellDancingRunes
};

enum class Affixes : uint32
{
    Overflowing     = 1,
    Skittish        = 2,
    Volcanic        = 3,
    Necrotic        = 4,
    Teeming         = 5,
    Raging          = 6,
    Bolstering      = 7,
    Sanguine        = 8,
    Tyrannical      = 9,
    Fortified       = 10,
    Bursting        = 11,
    Grievous        = 12,
    Explosive       = 13,
    Quaking         = 14,
    Relentless      = 15,
    MaxAffixes      = 16
};

uint32 const g_BattleResSpells[eInstanceSpells::MaxBattleResSpells] =
{
    eInstanceSpells::Rebirth,
    eInstanceSpells::Soulstone,
    eInstanceSpells::RaiseAlly,
    eInstanceSpells::EternalGuardian,
    eInstanceSpells::GiftOfChiJi,
    eInstanceSpells::DustOfLife
};

static uint32 const ChallengeModeOrb = 246779;
static uint32 const ChallengeModeDoor = 239323;

class InstanceScript : public ZoneScript
{
    public:
        explicit InstanceScript(Map* p_Map);

        virtual ~InstanceScript() {}

        Map* instance;

        // On creation, NOT load.
        virtual void Initialize() {}

        // On load
        virtual void Load(char const* data) { LoadBossState(data); }

        // When save is needed, this function generates the data
        virtual std::string GetSaveData() { return GetBossSaveData(); }

        void SaveToDB();

        virtual void Update(uint32 p_Diff);
        void UpdateOperations(uint32 const p_Diff);

        // Used by the map's CanEnter function.
        // This is to prevent players from entering during boss encounters.
        virtual bool IsEncounterInProgress(int32 p_ExcludeBossID = -1) const;

        uint32 CountDefeatedEncounters() const;

        // Called when a player begins to enter the instance.
        virtual void BeforePlayerEnter(Player* /*player*/) {}

        // Called when a player successfully enters the instance.
        virtual void OnPlayerEnter(Player* p_Player);

        /// Called when a player successfully exits the instance
        virtual void OnPlayerExit(Player* p_Player);

        /// Called when we want handle the spells interactions in certains situations
        virtual bool IsInteractingPossible(Unit const* p_Caster, Unit const* p_Target, SpellInfo const* p_Spell) const
        {
            UNUSED(p_Caster);
            UNUSED(p_Target);
            UNUSED(p_Spell);
            return true;
        }

        virtual void OnCustomValuesUpdate(Unit const* p_Unit, Player const* p_Player, uint32 & p_Value) const
        {
            UNUSED(p_Unit);
            UNUSED(p_Player);
            UNUSED(p_Value);
        }

        /// Called when the players go to receive the encounter id
        virtual bool IsValidEncounter(EncounterCreditType p_Type, Unit* p_Source) const
        {
            UNUSED(p_Type);
            UNUSED(p_Source);
            return true;
        }

        /// Called when a player died in the instance
        virtual void HandlePlayerKilled(Player* p_Player) { }

        virtual void onScenarionNextStep(uint32 newStep) {}

        virtual void OnStartChallenge(Item const* p_Item) { }

        // Handle open / close objects
        // use HandleGameObject(0, boolen, GO); in OnObjectCreate in instance scripts
        // use HandleGameObject(GUID, boolen, NULL); in any other script
        void HandleGameObject(uint64 guid, bool open, GameObject* go = NULL);

        // change active state of doors or buttons
        void DoUseDoorOrButton(uint64 guid, uint32 withRestoreTime = 0, bool useAlternativeState = false);

        // Respawns a GO having negative spawntimesecs in gameobject-table
        void DoRespawnGameObject(uint64 guid, uint32 timeToDespawn = MINUTE);

        // sends world state update to all players in instance
        void DoUpdateWorldState(uint32 worldstateId, uint32 worldstateValue);

        // Send Notify to all players in instance
        void DoSendNotifyToInstance(char const* format, ...);

        // Reset Achievement Criteria
        void DoResetCriteria(CriteriaTypes type, uint64 miscValue1 = 0, uint64 miscValue2 = 0, bool evenIfCriteriaComplete = false);

        // Complete Achievement for all players in instance
        void DoCompleteAchievement(uint32 achievement);

        // Update Achievement Criteria for all players in instance
        void DoUpdateCriteria(CriteriaTypes type, uint32 miscValue1 = 0, uint32 miscValue2 = 0, Unit* unit = NULL);

        // Start/Stop Timed Achievement Criteria for all players in instance
        void DoStartCriteriaTimer(CriteriaTimedTypes type, uint32 entry);
        void DoStopCriteriaTimer(CriteriaTimedTypes type, uint32 entry);

        /// Remove movement forces on all players for the specified source
        void DoRemoveForcedMovementsOnPlayers(uint64 p_Source = 0);

        // Remove Auras due to Spell on all players in instance
        void DoRemoveAurasDueToSpellOnPlayers(uint32 spell, uint64 p_Guid = 0);

        void DoRemoveStackOnPlayers(uint32 p_Spell);

        /// Cast spell on all players in instance
        void DoCastSpellOnPlayers(uint32 p_SpellID, Unit* p_Caster = nullptr, bool p_BreakFirst = false, bool p_IgnoreGM = false);

        void DoStartConversationOnPlayers(uint32 p_ConversationEntry);

        void DoStartMovie(uint32 p_MovieID);

        void DoSetAlternatePowerOnPlayers(int32 value);

        void DoModifyPlayerCurrencies(uint32 id, int32 value);

        void DoNearTeleportPlayers(const Position pos, bool casting = false);

        void DoKilledMonsterKredit(uint32 questId, uint32 entry, uint64 guid = 0);

        // Add aura on all players in instance
        void DoAddAuraOnPlayers(uint32 spell);

        /// Remove cooldown for spell on all players in instance
        void DoRemoveSpellCooldownOnPlayers(uint32 p_SpellID);

        /// Remove cooldowns equal or less than specified time to all players in instance
        void DoRemoveSpellCooldownWithTimeOnPlayers(uint32 p_MinRecoveryTime);

        // Remove auras with AURA_INTERRUPT_FLAG2_START_OF_ENCOUNTER flags from players
        void DoRemoveAurasWithEncounterStartFlags();

        /// Remove cooldowns to all players/pets in instance, with same rules as arena cooldown reset
        void DoRemoveSpellCooldownOnPlayers();

        /// Used to apply or unapply item set bonuses on all players in instance
        void HandleItemSetBonusesOnPlayers(bool p_Apply);

        /// Used to apply or unapply gem bonuses on all player in instance
        void HandleGemBonusesOnPlayers(bool p_Apply);

        /// Do combat stop on all players in instance
        void DoCombatStopOnPlayers();

        /// Teleport players to instance entrance
        void RepopPlayersAtGraveyard(bool p_ForceGraveyard = false);

        /// Respawn specified creature, or all creatures
        void RespawnCreature(uint64 p_Guid = 0);

        /// Play scene to all players in instance
        void PlaySceneForPlayers(Position const p_Pos, uint32 p_ScenePackageID);

        /// Send weather to players in instance
        void SendWeatherToPlayers(WeatherState p_Weather, float p_Intensity, Player* p_Target = nullptr, bool p_Abrupt = false) const;

        /// Send Scenario Event Update to all players in the instance
        void SendScenarioEventToAllPlayers(uint32 p_EventID, uint32 p_Count = 1);

        /// Called when a player starts falling
        virtual void HandlePlayerStartFalling(Player* p_FallingPlr) { }

        /// Called when UpdateEncounterState is called, before DoAddBossLootedOnPlayers
        virtual void BeforeAddBossLooted(uint32 p_CreatureID)
        {
            UNUSED(p_CreatureID);
        }

        // Return wether server allow two side groups or not
        bool ServerAllowsTwoSideGroups() { return sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP); }

        virtual bool SetBossState(uint32 id, EncounterState state);
        EncounterState GetBossState(uint32 id) const { return id < m_Bosses.size() ? m_Bosses[id].state : TO_BE_DECIDED; }
        BossBoundaryMap const* GetBossBoundary(uint32 id) const { return id < m_Bosses.size() ? &m_Bosses[id].boundary : NULL; }

        // Achievement criteria additional requirements check
        // NOTE: not use this if same can be checked existed requirement types from AchievementCriteriaRequirementType
        virtual bool CheckAchievementCriteriaMeet(uint32 /*criteria_id*/, Player const* /*source*/, Unit const* /*target*/ = NULL, uint64 /*miscvalue1*/ = 0);

        // Checks boss requirements (one boss required to kill other)
        virtual bool CheckRequiredBosses(uint32 /*bossId*/, Player const* /*player*/ = NULL) const;

        // Checks encounter state at kill/spellcast
        void UpdateEncounterState(EncounterCreditType type, uint32 creditEntry, Unit* source);

        /// Add boss looted on players
        void DoAddBossLootedOnPlayers(Unit* p_Source);

        uint32 GetCurrentEncounter() const { return m_EncounterID; }

        void StartEncounter(uint32 p_EncounterID, Creature* p_Source = nullptr);
        void SendEncounterStart(uint32 p_EncounterID, Player* p_Target = nullptr);
        void SendEncounterEnd(uint32 p_EncounterID, bool p_Success);
        uint32 GetEncounterIDForBoss(Creature* p_Boss) const;
        uint32 GetEncounterIDForChest(uint32 p_Entry) const;
        void HandleVantusRune(uint32 p_EncounterId, bool p_Apply);

        void SaveEncounterLogs(Creature* p_Creature, uint32 p_EncounterID);

        // Used only during loading
        void SetCompletedEncountersMask(uint32 newMask) { m_CompletedEncounters = newMask; }

        // Returns completed encounters mask for packets
        uint32 GetCompletedEncounterMask() const { return m_CompletedEncounters; }

        virtual void OnGameObjectRemove(GameObject* p_Go);

        virtual void OnGameObjectStateChanged(GameObject const* p_GameObject, uint32 p_State) { }

        virtual void CreatureDies(Creature* /*creature*/, Unit* /*killer*/) {}

        // For use in InstanceScript
        void OnPlayerEnterForScript(Player* player);
        void OnPlayerLeaveForScript(Player* player);
        void OnPlayerDiesForScript(Player* player);
        void OnCreatureCreateForScript(Creature* creature);
        void OnCreatureRemoveForScript(Creature* creature);
        void OnCreatureUpdateDifficulty(Creature* creature);
        void EnterCombatForScript(Creature* creature, Unit* enemy);
        void CreatureDiesForScript(Creature* creature, Unit* killer);
        void OnGameObjectCreateForScript(GameObject* go);
        void OnGameObjectRemoveForScript(GameObject* go);

        /// Called when falling damage are calculated for player
        virtual bool IsPlayerImmuneToFallDamage(Player* /*p_Player*/) const { return false; }

        /// Called when trying to find a path
        virtual bool IsInNoPathArea(Position const& /*p_Position*/) const { return false; }

        /// Called when player is being resurrected
        virtual void RelocateResPosIfNeeded(float& p_X, float& p_Y, float& p_Z, float& p_O, uint32& p_MapID, Player* p_Player) { }

        /// Called when player is leaving LFG group
        virtual bool PreventTeleportOutIfNeeded(Player* p_Player) const { return false; }

        virtual bool IsRaidBoss(uint32 creature_entry) { return false; }

        /// Add timed delayed operation
        /// @p_Timeout  : Delay time
        /// @p_Function : Callback function
        void AddTimedDelayedOperation(uint32 p_Timeout, std::function<void()> && p_Function)
        {
            m_EmptyWarned = false;
            m_TimedDelayedOperations.push_back(std::pair<uint32, std::function<void()>>(p_Timeout, p_Function));
        }

        /// Called after last delayed operation was deleted
        /// Do whatever you want
        virtual void LastOperationCalled() { }

        std::list<std::pair<int32, std::function<void()>>>      m_TimedDelayedOperations;   ///< Delayed operations
        bool                                                    m_EmptyWarned;              ///< Warning when there are no more delayed operations

        void SendEncounterUnit(uint32 type, Unit* unit = NULL, uint8 param1 = 0, uint8 param2 = 0);

        bool IsReleaseAllowed() const { return m_ReleaseAllowed; }

        void setScenarioStep(uint32 step) { scenarioStep = step; }
        uint32 getScenarionStep() const { return scenarioStep; }

        // Called when a creature is killed by a player
        virtual void OnCreatureKilled(Creature* /*p_Creature*/, Player* /*p_Player*/) { }

        /// Called when an aura is applied on a player
        virtual void OnPlayerAuraApplied(Player* p_Player, uint32 p_SpellID)
        {
            UNUSED(p_Player);
            UNUSED(p_SpellID);
        }

        /// Called when an aura is removed from a player
        virtual void OnPlayerAuraRemoved(Player* p_Player, uint32 p_SpellID, AuraRemoveMode p_RemoveMode)
        {
            UNUSED(p_Player);
            UNUSED(p_SpellID);
            UNUSED(p_RemoveMode);
        }

        /// Called when a quest is rewarded for a player
        virtual void OnQuestRewarded(Player* p_Player, Quest const* p_Quest)
        {
            UNUSED(p_Player);
            UNUSED(p_Quest);
        }

        // Check if all players are dead (except gamemasters)
        virtual bool IsWipe();

        virtual void FillInitialWorldStates(ByteBuffer& /*data*/) {}

        /// Called when WorldObject::IsWithinLOSInMap is called
        virtual bool IsLineOfSightDisabled() const { return false; }

        void UpdatePhasing();
        void UpdateCreatureGroupSizeStats();

        void SetDisabledBosses(uint32 p_DisableMask) { m_DisabledMask = p_DisableMask; }

        BossInfo* GetBossInfo(uint32 p_ID)
        {
            if (p_ID < m_Bosses.size())
                return &m_Bosses[p_ID];

            return nullptr;
        }

        void UpdateDamageManager(uint64 p_CallerGuid, int32 p_Damage, bool p_Heal = false);
        void AddToDamageManager(Creature* p_Creature, uint8 p_PullNum = 0);
        void RemoveFromDamageManager(Creature* p_Creature, uint8 p_PullNum = 0);
        bool CheckDamageManager() { return m_InitDamageManager; }

        void SetPullDamageManager(uint64 p_Guid, uint8 p_PullID) { m_PullDamageManager[p_Guid] = p_PullID; }
        int8 GetPullDamageManager(uint64 p_Guid) const;

        DamageManagerMap m_DamageManager;
        PullDamageManagerMap m_PullDamageManager;
        bool m_InitDamageManager;

        void HandleBloodlustApplied(uint64 p_Guid);

        void HandleRespawnAtLoading(uint64 p_Guid, uint32 p_BossID);

        void PlayCosmeticEvent(uint32 p_EventId, uint64 p_OriginGUID = 0, bool p_SummonsFromGuid = false);

        //////////////////////////////////////////////////////////////////////////
        /// Combat Resurrection - http://wow.gamepedia.com/Resurrect#Combat_resurrection
        void ResetCombatResurrection();
        void StartCombatResurrection();
        void StartChallengeCombatResurrection();
        void UpdateCombatResurrection(uint32 const p_Diff);
        bool CanUseCombatResurrection() const;
        void ConsumeCombatResurrectionCharge();

        uint32 m_InCombatResCount;
        uint32 m_MaxInCombatResCount;
        uint32 m_CombatResChargeTime;
        uint32 m_NextCombatResChargeTime;
        //////////////////////////////////////////////////////////////////////////

        /// Challenge
        void SetChallenge(Challenge* challenge) { m_Challenge = challenge; }
        Challenge* GetChallenge() const { return m_Challenge; }
        bool IsChallenge() const { return m_Challenge != nullptr; }

        void ResetChallengeMode();

        void AddChallengeModeChests(uint64 chestGuid, uint8 chestLevel) { m_ChallengeChestGuids[chestLevel] = chestGuid; }
        uint64 GetChellngeModeChests(uint8 chestLevel) { return m_ChallengeChestGuids[chestLevel]; }

        void AddChallengeModeDoor(uint64 doorGuid) { m_ChallengeDoorGuids.push_back(doorGuid); }
        void AddChallengeModeOrb(uint64 orbGuid) { m_ChallengeOrbGuid = orbGuid; }

        virtual bool IsValidTargetForAffix(Unit const* /**/, Affixes const /**/)
        {
            return true;
        }

        uint32 GetEncounterTime() const { return uint32(time(nullptr)) - m_EncounterTime; }

        std::vector<uint64> m_ChallengeDoorGuids;
        std::vector<uint64> m_ChallengeChestGuids;
        uint64 m_ChallengeOrbGuid;
        uint64 m_ChallengeChestFirst;
        uint64 m_ChallengeChestSecond;
        uint64 m_ChallengeChestThird;
        Challenge* m_Challenge;

        FunctionProcessor m_Functions;

        uint32 GetEncounterCount() const { return m_Bosses.size(); }

    protected:
        void SetBossNumber(uint32 p_Number);
        void LoadDoorData(DoorData const* data);
        void LoadMinionData(MinionData const* data);

        void AddDoor(GameObject* door, bool add);
        void AddMinion(Creature* minion, bool add);

        void UpdateDoorState(GameObject* door, bool p_WithDelay = false);
        void UpdateMinionState(Creature* minion, EncounterState state);

        std::string LoadBossState(char const* data);
        std::string GetBossSaveData();
    private:
        std::vector<BossInfo> m_Bosses;
        DoorInfoMap doors;
        MinionInfoMap minions;
        uint32 m_CompletedEncounters; // completed encounter mask, bit indexes are DungeonEncounter.db2 boss numbers, used for packets
        uint32 m_EncounterTime;
        uint32 m_DisabledMask;
        uint32 scenarioStep;
        uint32 m_EncounterID;

        std::set<std::pair<uint64, uint32>> m_BloodlustGuids;
        EncounterDatas m_EncounterDatas;

        bool m_ReleaseAllowed;
};

#define CHECK_REQUIRED_BOSSES(p_BossID) \
if (InstanceScript* p_Instance = me->GetInstanceScript()) \
{ \
    if (!p_Instance->CheckRequiredBosses(p_BossID)) \
    { \
        EnterEvadeMode(); \
        p_Instance->DoCombatStopOnPlayers(); \
        return; \
    } \
}

#define CHECK_REQUIRED_BOSSES_AND_TELEPORT(p_BossID, p_TeleportPos) \
if (InstanceScript* p_Instance = me->GetInstanceScript()) \
{ \
    if (!p_Instance->CheckRequiredBosses(p_BossID)) \
    { \
        EnterEvadeMode(); \
        p_Instance->DoCombatStopOnPlayers(); \
        p_Instance->DoNearTeleportPlayers(p_TeleportPos); \
        return; \
    } \
}

#endif
