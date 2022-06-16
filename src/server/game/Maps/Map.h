////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_MAP_H
#define TRINITY_MAP_H

#include "Define.h"

#include "DB2Stores.h"
#include "GridDefines.h"
#include "Cell.h"
#include "Timer.h"
#include "SharedDefines.h"
#include "GridRefManager.h"
#include "MapRefManager.h"
#include "DynamicTree.h"
#include "GameObjectModel.h"
#include "Common.h"
#include "FunctionProcessor.hpp"
#include "QueryCallbackManager.h"

#include <bitset>
#include <functional>
#include <chrono>

class Unit;
class WorldPacket;
class InstanceScript;
class Group;
using GroupPtr = std::shared_ptr<Group>;
class InstanceSave;
class Object;
class WorldObject;
class TempSummon;
class Player;
class CreatureGroup;
struct ScriptInfo;
struct ScriptAction;
struct Position;
class Battleground;
class MapInstanced;
class InstanceMap;
class Transport;
class OutdoorPvP;
class OutdoorPvEScript;
class Battlefield;
class WildBattlePetZonePools;
namespace JadeCore { struct ObjectUpdater; }

struct ScriptAction
{
    uint64 sourceGUID;
    uint64 targetGUID;
    uint64 ownerGUID;                                       // owner of source if source is item
    ScriptInfo const* script;                               // pointer to static script data
};

union u_map_magic
{
    char asChar[4];
    uint32 asUInt;
};

// ******************************************
// Map file format defines
// ******************************************
struct map_fileheader
{
    u_map_magic mapMagic;
    u_map_magic versionMagic;
    u_map_magic buildMagic;
    uint32 areaMapOffset;
    uint32 areaMapSize;
    uint32 heightMapOffset;
    uint32 heightMapSize;
    uint32 liquidMapOffset;
    uint32 liquidMapSize;
    uint32 holesOffset;
    uint32 holesSize;
};

#define MAP_AREA_NO_AREA      0x0001

struct map_areaHeader
{
    uint32 fourcc;
    uint16 flags;
    uint16 gridArea;
};

#define MAP_HEIGHT_NO_HEIGHT            0x0001
#define MAP_HEIGHT_AS_INT16             0x0002
#define MAP_HEIGHT_AS_INT8              0x0004
#define MAP_HEIGHT_HAS_FLIGHT_BOUNDS    0x0008

struct map_heightHeader
{
    uint32 fourcc;
    uint32 flags;
    float  gridHeight;
    float  gridMaxHeight;
};

#define MAP_LIQUID_NO_TYPE    0x0001
#define MAP_LIQUID_NO_HEIGHT  0x0002

struct map_liquidHeader
{
    uint32 fourcc;
    uint8 flags;
    uint8 liquidFlags;
    uint16 liquidType;
    uint8  offsetX;
    uint8  offsetY;
    uint8  width;
    uint8  height;
    float  liquidLevel;
};

enum ZLiquidStatus
{
    LIQUID_MAP_NO_WATER     = 0x00000000,
    LIQUID_MAP_ABOVE_WATER  = 0x00000001,
    LIQUID_MAP_WATER_WALK   = 0x00000002,
    LIQUID_MAP_IN_WATER     = 0x00000004,
    LIQUID_MAP_UNDER_WATER  = 0x00000008
};

#define MAP_LIQUID_TYPE_NO_WATER    0x00
#define MAP_LIQUID_TYPE_WATER       0x01
#define MAP_LIQUID_TYPE_OCEAN       0x02
#define MAP_LIQUID_TYPE_MAGMA       0x04
#define MAP_LIQUID_TYPE_SLIME       0x08

#define MAP_ALL_LIQUIDS   (MAP_LIQUID_TYPE_WATER | MAP_LIQUID_TYPE_OCEAN | MAP_LIQUID_TYPE_MAGMA | MAP_LIQUID_TYPE_SLIME)

#define MAP_LIQUID_TYPE_DARK_WATER  0x10

struct LiquidData
{
    uint32 type_flags;
    uint32 entry;
    float  level;
    float  depth_level;
};

class GridMap
{
    uint32  _flags;
    union{
        float* m_V9;
        uint16* m_uint16_V9;
        uint8* m_uint8_V9;
    };
    union{
        float* m_V8;
        uint16* m_uint16_V8;
        uint8* m_uint8_V8;
    };
    int16* _maxHeight;
    int16* _minHeight;
    // Height level data
    float _gridHeight;
    float _gridIntHeightMultiplier;

    // Area data
    uint16* _areaMap;

    // Liquid data
    float _liquidLevel;
    uint16* _liquidEntry;
    uint8* _liquidFlags;
    float* _liquidMap;
    uint16 _gridArea;
    uint16 _liquidGlobalEntry;
    uint8 _liquidGlobalFlags;
    uint16 _liquidType;
    uint8 _liquidOffX;
    uint8 _liquidOffY;
    uint8 _liquidWidth;
    uint8 _liquidHeight;

    bool loadAreaData(FILE* in, uint32 offset, uint32 size);
    bool loadHeihgtData(FILE* in, uint32 offset, uint32 size);
    bool loadLiquidData(FILE* in, uint32 offset, uint32 size);

    // Get height functions and pointers
    typedef float (GridMap::*GetHeightPtr) (float x, float y) const;
    GetHeightPtr _gridGetHeight;
    float getHeightFromFloat(float x, float y) const;
    float getHeightFromUint16(float x, float y) const;
    float getHeightFromUint8(float x, float y) const;
    float getHeightFromFlat(float x, float y) const;

public:
    GridMap();
    ~GridMap();
    bool loadData(char* filaname);
    void unloadData();

    uint16 getArea(float x, float y) const;
    inline float getHeight(float x, float y) const {return (this->*_gridGetHeight)(x, y);}
    float getMinHeight(float x, float y) const;
    float getLiquidLevel(float x, float y) const;
    uint8 getTerrainType(float x, float y) const;
    ZLiquidStatus getLiquidStatus(float x, float y, float z, uint8 ReqLiquidType, LiquidData* data = 0);
};

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push, N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

struct InstanceTemplate
{
    uint32 Parent;
    uint32 ScriptId;
    bool AllowMount;
};

enum LevelRequirementVsMode
{
    LEVELREQUIREMENT_HEROIC = 70
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

typedef std::map<uint32/*leaderDBGUID*/, CreatureGroup*>        CreatureGroupHolderType;
typedef std::set<WorldObject*> BigObjectContainer;

class Map : public GridRefManager<NGridType>
{
    friend class MapReference;
    public:
        Map(uint32 p_Id, time_t, uint32 p_InstanceId, uint8 p_SpawnMode, uint32 p_InstanceZoneId, Map* p_Parent = nullptr);
        virtual ~Map();

        MapEntry const* GetEntry() const { return i_mapEntry; }

        // currently unused for normal maps
        bool CanUnload(uint32 diff)
        {
            if (!m_unloadTimer)
                return false;

            if (m_unloadTimer <= diff)
                return true;

            m_unloadTimer -= diff;
            return false;
        }

        virtual bool AddPlayerToMap(Player*, bool p_Switched = false);
        virtual void RemovePlayerFromMap(Player*, bool);
        template<class T> bool AddToMap(T *);
        template<class T> void RemoveFromMap(T *, bool);

        void SetUpdating(bool value) { m_IsUpdating = value; }
        bool IsUpdating() const { return m_IsUpdating; }
        bool CanUpdate() const;

        void VisitNearbyCellsOf(WorldObject* obj, TypeContainerVisitor<JadeCore::ObjectUpdater, GridTypeMapContainer> &gridVisitor, TypeContainerVisitor<JadeCore::ObjectUpdater, WorldTypeMapContainer> &worldVisitor);
        virtual void Update(const uint32, const uint32);

        float GetVisibilityRange(uint32 zoneId = 0, uint32 areaId = 0) const;
        uint32 GetCustomMapSize() const { return m_CustomMapSize; }
        void SetCustomMapSize(uint32 p_Size) { m_CustomMapSize = p_Size; }

        //function for setting up visibility distance for maps on per-type/per-Id basis
        virtual void InitVisibilityDistance();

        void PlayerRelocation(Player*, float x, float y, float z, float orientation);
        void CreatureRelocation(Creature* creature, float x, float y, float z, float ang, bool respawnRelocationOnFail = true);
        void GameObjectRelocation(GameObject* go, float x, float y, float z, float orientation, bool respawnRelocationOnFail = true);
        void AreaTriggerRelocation(AreaTrigger*, float x, float y, float z, float orientation);

        template<class T, class CONTAINER> void Visit(const Cell& cell, TypeContainerVisitor<T, CONTAINER> &visitor);
        template<class T, class CONTAINER> void VisitWithNearbies(const Cell& cell, TypeContainerVisitor<T, CONTAINER> &visitor);

        bool IsRemovalGrid(float x, float y) const
        {
            GridCoord p = JadeCore::ComputeGridCoord(x, y);
            return !getNGrid(p.x_coord, p.y_coord) || getNGrid(p.x_coord, p.y_coord)->GetGridState() == GRID_STATE_REMOVAL;
        }

        bool IsGridLoaded(float x, float y) const
        {
            return IsGridLoaded(JadeCore::ComputeGridCoord(x, y));
        }

        bool GetUnloadLock(const GridCoord &p) const { return getNGrid(p.x_coord, p.y_coord)->getUnloadLock(); }
        void SetUnloadLock(const GridCoord &p, bool on) { getNGrid(p.x_coord, p.y_coord)->setUnloadExplicitLock(on); }
        void LoadGrid(float x, float y);
        bool UnloadGrid(NGridType& ngrid, bool pForce);
        virtual void UnloadAll();

        void ResetGridExpiry(NGridType &grid, float factor = 1) const
        {
            grid.ResetTimeTracker(time_t(float(i_gridExpiry)*factor));
        }

        time_t GetGridExpiry(void) const { return i_gridExpiry; }
        uint32 GetId(void) const
        {
            if (i_mapEntry != nullptr)
                return i_mapEntry->MapID;
            return 0;
        }

        static bool ExistMap(uint32 mapid, int gx, int gy);
        static bool ExistVMap(uint32 mapid, int gx, int gy);

        static void InitStateMachine();
        static void DeleteStateMachine();

        ACE_RW_Thread_Mutex& GetMMapLock() const { return *(const_cast<ACE_RW_Thread_Mutex*>(&MMapLock)); }

        Map const* GetParent() const { return (m_parentMap? m_parentMap : this); }

        // some calls like isInWater should not use vmaps due to processor power
        // can return INVALID_HEIGHT if under z+2 z coord not found height
        float GetHeight(float x, float y, float z, bool checkVMap = true, float maxSearchDist = DEFAULT_HEIGHT_SEARCH, bool fromUnder = false) const;
        float GetMinHeight(float x, float y) const;

        ZLiquidStatus getLiquidStatus(float x, float y, float z, uint8 ReqLiquidType, LiquidData* data = 0) const;

        bool GetAreaInfo(float x, float y, float z, uint32 &mogpflags, int32 &adtId, int32 &rootId, int32 &groupId) const;

        bool IsOutdoors(float x, float y, float z) const;

        uint8 GetTerrainType(float x, float y) const;
        float GetWaterLevel(float x, float y) const;
        bool IsInWater(float x, float y, float z, LiquidData* data = 0) const;
        bool IsUnderWater(float x, float y, float z) const;

        virtual void SetObjectVisibility(float p_Visibility);

        uint32 GetAreaId(float x, float y, float z, bool *isOutdoors) const;
        uint32 GetAreaId(float x, float y, float z) const;

        uint32 GetZoneId(float x, float y, float z) const;
        void GetZoneAndAreaId(uint32& zoneid, uint32& areaid, float x, float y, float z) const;

        void MoveAllCreaturesInMoveList();
        void MoveAllGameObjectsInMoveList();
        void RemoveAllObjectsInRemoveList();
        virtual void RemoveAllPlayers();

        // used only in MoveAllCreaturesInMoveList and ObjectGridUnloader
        bool CreatureRespawnRelocation(Creature* c, bool diffGridOnly);
        bool GameObjectRespawnRelocation(GameObject* go, bool diffGridOnly);

        // assert print helper
        bool CheckGridIntegrity(Creature* c, bool moved) const;

        uint32 GetInstanceId() const { return i_InstanceId; }
        uint32 GetScenarioGuid() const { return m_ScenarioGuid; }
        uint8 GetSpawnMode() const { return (i_spawnMode); }
        void SetSpawnMode(uint8 spawnMode) { i_spawnMode = spawnMode; }
        void SetScenarioGuid(uint32 p_ScenarioGuid) { m_ScenarioGuid = p_ScenarioGuid; }
        virtual bool CanEnter(Player* /*player*/) { return true; }
        const char* GetMapName() const;

        // have meaning only for instanced map (that have set real difficulty)
        Difficulty GetDifficultyID() const { return Difficulty(GetSpawnMode()); }
        MapDifficultyEntry const* GetMapDifficulty() const;
        CriteriaLegacyRaidType GetLegacyRaidType() const;

        ItemContext GetLootItemContext(Player* p_Player = nullptr) const;

        uint32 GetMinPlayer() const;
        bool IsNeedRecalc() const;
        bool IsNeedRespawn(uint32 lastRespawn) const { return lastRespawn < m_respawnChallenge; }
        bool Instanceable() const { return i_mapEntry && i_mapEntry->Instanceable(); }
        bool IsDungeon() const { return i_mapEntry && i_mapEntry->IsDungeon(); }
        bool IsScenario() const { return i_mapEntry && i_mapEntry->IsScenario(); }
        bool IsNonRaidDungeon() const { return i_mapEntry && i_mapEntry->IsNonRaidDungeon(); }
        bool IsRaid() const { return i_mapEntry && i_mapEntry->IsRaid(); }

        bool IsRaidOrHeroicDungeon() const { return IsRaid() || IsHeroic(); }
        bool IsHeroic() const { return i_spawnMode == Difficulty::DifficultyHeroic || i_spawnMode == Difficulty::DifficultyRaidHeroic || i_spawnMode == Difficulty::Difficulty25HC || i_spawnMode == Difficulty::Difficulty10HC; }
        bool Is25ManRaid() const { return IsRaid() && (i_spawnMode == Difficulty::Difficulty25N || i_spawnMode == Difficulty::Difficulty25HC); }   // since 25man difficulties are 1 and 3, we can check them like that
        bool IsLFR() const { return i_spawnMode == Difficulty::DifficultyRaidTool || i_spawnMode == Difficulty::DifficultyRaidLFR; }
        bool IsChallengeMode() const { return i_spawnMode == Difficulty::DifficultyMythicKeystone; }
        bool IsMythic() const { return i_spawnMode == Difficulty::DifficultyRaidMythic || i_spawnMode == Difficulty::DifficultyMythic || i_spawnMode == Difficulty::DifficultyMythicKeystone; }
        bool IsHeroicOrMythic() const { return IsHeroic() || IsMythic(); }
        bool IsNormal() const { return i_spawnMode == Difficulty::DifficultyNormal || i_spawnMode == Difficulty::DifficultyRaidNormal; }

        void SendInstanceGroupSizeChanged() const;

        bool IsBattleground() const { return i_mapEntry && i_mapEntry->IsBattleground(); }
        bool IsBattleArena() const { return i_mapEntry && i_mapEntry->IsBattleArena(); }
        bool IsBattlegroundOrArena() const { return i_mapEntry && i_mapEntry->IsBattlegroundOrArena(); }

        uint32 Expansion() const { return i_mapEntry ? i_mapEntry->Expansion() : 0; }

        bool GetEntrancePos(int32 &mapid, float &x, float &y)
        {
            if (!i_mapEntry)
                return false;
            return i_mapEntry->GetEntrancePos(mapid, x, y);
        }

        void AddObjectToRemoveList(WorldObject* obj);
        void AddObjectToSwitchList(WorldObject* obj, bool on);
        virtual void DelayedUpdate(const uint32 diff);

        void resetMarkedCells() { marked_cells.reset(); }
        bool isCellMarked(uint32 pCellId) { return marked_cells.test(pCellId); }
        void markCell(uint32 pCellId) { marked_cells.set(pCellId); }

        bool HavePlayers() const { return !m_mapRefManager.isEmpty() || !m_LeavingPlayers.empty();  }
        uint32 GetPlayersCountExceptGMs() const;
        bool ActiveObjectsNearGrid(NGridType const& ngrid) const;

        void AddWorldObject(WorldObject* obj);
        void RemoveWorldObject(WorldObject* obj);

        void SendToPlayers(WorldPacket const* data) const;

        typedef MapRefManager PlayerList;
        PlayerList const& GetPlayers() const { return m_mapRefManager; }
        uint32 GetPlayerCount() const { return m_mapRefManager.getSize(); }

        void ForEachPlayer(std::function<void(Player*)> p_Func);

        //per-map script storage
        void ScriptsStart(std::map<uint32, std::multimap<uint32, ScriptInfo> > const& scripts, uint32 id, Object* source, Object* target);
        void ScriptCommandStart(ScriptInfo const& script, uint32 delay, Object* source, Object* target);

        // must called with AddToWorld
        template<class T>
        void AddToActive(T* obj);

        // must called with RemoveFromWorld
        template<class T>
        void RemoveFromActive(T* obj);

        template<class T> void SwitchGridContainers(T* obj, bool on);
        template<class NOTIFIER> void VisitAll(const float &x, const float &y, float radius, NOTIFIER &notifier, bool loadGrids = false);
        template<class NOTIFIER> void VisitFirstFound(const float &x, const float &y, float radius, NOTIFIER &notifier, bool loadGrids = false);
        template<class NOTIFIER> void VisitWorld(const float &x, const float &y, float radius, NOTIFIER &notifier, bool loadGrids = false);
        template<class NOTIFIER> void VisitGrid(const float &x, const float &y, float radius, NOTIFIER &notifier, bool loadGrids = false);
        CreatureGroupHolderType CreatureGroupHolder;

        void UpdateIteratorBack(Player* player);

        GameObject* SummonGameObject(uint32 p_Entry, Position const& p_Position, uint32 p_CustomFlags = 0, bool p_ActiveObject = false);

        TempSummon* SummonCreature(uint32 entry, Position const& pos, SummonPropertiesEntry const* properties = NULL, uint32 duration = 0, Unit* summoner = NULL, uint32 spellId = 0, uint32 vehId = 0, uint64 viewerGuid = 0, std::list<uint64>* viewersList = NULL, bool p_Big = false, bool p_ActiveObject = false, uint64 summonerGuid = 0);
        void SummonCreatureGroup(uint8 group, std::list<TempSummon*>* list = nullptr);
        Creature* GetCreature(uint64 guid);
        GameObject* GetGameObject(uint64 guid);
        Transport* GetTransport(uint64 guid);
        DynamicObject* GetDynamicObject(uint64 guid);
        AreaTrigger* GetAreaTrigger(uint64 p_Guid);
        Conversation* GetConversation(uint64 p_Guid);
        EventObject* GetEventObject(uint64 p_Guid);

        std::unordered_set<Corpse*> const* GetCorpsesInCell(uint32 cellId) const
        {
            auto itr = _corpsesByCell.find(cellId);
            if (itr != _corpsesByCell.end())
                return &itr->second;
            return nullptr;
        }

        Corpse* GetCorpseByPlayer(uint64 const& ownerGuid) const
        {
            auto itr = _corpsesByPlayer.find(ownerGuid);
            if (itr != _corpsesByPlayer.end())
                return itr->second;
            return nullptr;
        }

        MapInstanced* ToMapInstanced(){ if (Instanceable())  return reinterpret_cast<MapInstanced*>(this); else return NULL;  }
        const MapInstanced* ToMapInstanced() const { if (Instanceable())  return (const MapInstanced*)((MapInstanced*)this); else return NULL;  }

        InstanceMap* ToInstanceMap(){ if (IsDungeon())  return reinterpret_cast<InstanceMap*>(this); else return NULL;  }
        const InstanceMap* ToInstanceMap() const { if (IsDungeon())  return (const InstanceMap*)((InstanceMap*)this); else return NULL;  }
        float GetWaterOrGroundLevel(float x, float y, float z, float* ground = NULL, bool swim = false) const;
        float GetHeight(uint32 phasemask, float x, float y, float z, bool vmap = true, float maxSearchDist = DEFAULT_HEIGHT_SEARCH, bool fromUnder = false) const;
        float GetVmapHeight(float x, float y, float z) const;
        bool isInLineOfSight(float x1, float y1, float z1, float x2, float y2, float z2, uint32 phasemask) const;
        void Balance() { _dynamicTree.balance(); }
        void RemoveGameObjectModel(const GameObjectModel& model) { _dynamicTree.remove(model); }
        void InsertGameObjectModel(const GameObjectModel& model) { _dynamicTree.insert(model); }
        bool ContainsGameObjectModel(const GameObjectModel& model) const { return _dynamicTree.contains(model);}
        bool getObjectHitPos(uint32 phasemask, float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float &ry, float& rz, float modifyDist);

        virtual uint32 GetOwnerGuildId(uint32 /*team*/ = TEAM_OTHER) const { return 0; }
        /*
            RESPAWN TIMES
        */
        time_t GetLinkedRespawnTime(uint64 guid) const;
        time_t GetCreatureRespawnTime(uint32 dbGuid) const
        {
            std::unordered_map<uint32 /*dbGUID*/, time_t>::const_iterator itr = _creatureRespawnTimes.find(dbGuid);
            if (itr != _creatureRespawnTimes.end())
                return itr->second;

            return time_t(0);
        }

        time_t GetGORespawnTime(uint32 dbGuid) const
        {
            std::unordered_map<uint32 /*dbGUID*/, time_t>::const_iterator itr = _goRespawnTimes.find(dbGuid);
            if (itr != _goRespawnTimes.end())
                return itr->second;

            return time_t(0);
        }

        void SaveCreatureRespawnTime(uint32 dbGuid, time_t respawnTime);
        void RemoveCreatureRespawnTime(uint32 dbGuid);
        void SaveGORespawnTime(uint32 dbGuid, time_t respawnTime);
        void RemoveGORespawnTime(uint32 dbGuid);
        void LoadRespawnTimes();
        void DeleteRespawnTimes();

        bool HasAnyCreatureRespawnTime() const;

        static void DeleteRespawnTimesInDB(uint16 mapId, uint32 instanceId);

        void AddGameObjectTransport(GameObject* p_Transport) { _transportsGameObject.insert(p_Transport); }
        void DeleteGameObjectTransport(GameObject* p_Transport) { _transportsGameObject.erase(p_Transport); }

        void SendInitTransports(Player* player);
        void SendRemoveTransports(Player* player);

        void LoadAllGrids(float p_MinX, float p_MaxX, float p_MinY, float p_MaxY, Player* p_Player);

        void RemoveCreatureFromMoveList(Creature* p_Creature, bool p_Force = false);

        void AddScriptedCollisionGameObject(uint64 p_Guid) { m_ScriptedCollisionGobs.insert(p_Guid); }
        void RemoveScriptedCollisionGameObject(uint64 p_Guid) { m_ScriptedCollisionGobs.erase(p_Guid); }
        bool CollideWithScriptedGameObject(float p_X, float p_Y, float p_Z, float* p_OutZ = nullptr) const;

        uint32 GetInstanceZoneId() const
        {
            return m_InstanceZoneId;
        }

        void AddUpdateObject(Object* obj);
        void RemoveUpdateObject(Object* obj);

        void AddUpdateVisibilityObject(Unit* p_Unit)
        {
            _updateVisibilityObjects.insert(p_Unit);
        }

        void RemoveUpdateVisibilityObject(Unit* p_Unit)
        {
            _updateVisibilityObjects.erase(p_Unit);
        }

        void SendObjectUpdates(bool p_UseThreadPool);
        void ProcessUnitVisibilityUpdates(bool p_UseThreadPool);

        void AddTask(std::function<void()> p_Task)
        {
            m_Tasks.add(p_Task);
        }

        bool HasTasks() { return !m_Tasks.empty(); }

        FunctionProcessor m_Functions;

        std::unordered_map<uint32/*Entry*/, uint32/*Count*/> m_CreatureEntryCount;
        time_t m_respawnChallenge;

        std::chrono::time_point<std::chrono::steady_clock> m_UpdateEventsFunctionsTime;
        std::chrono::time_point<std::chrono::steady_clock> m_UpdateSpellsTime;
        std::chrono::time_point<std::chrono::steady_clock> m_UpdateMotionMasterTime;
        std::chrono::time_point<std::chrono::steady_clock> m_UpdateVisibilityTaskTime;
        std::chrono::time_point<std::chrono::steady_clock> m_UpdateVisibilityForcedTime;
        std::chrono::time_point<std::chrono::steady_clock> m_SpellSelectTargetsTime;

        std::vector<std::chrono::time_point<std::chrono::steady_clock>> m_UpdateSpellTime;
        std::chrono::time_point<std::chrono::steady_clock> m_SendPacketAroundTime;

        std::vector<std::chrono::time_point<std::chrono::steady_clock>> m_PlayersUpdateTime;

        std::set<uint32> m_TimeoutPlayerGUIDs;
        std::set<uint32> m_TimeoutAccountIDs;

        std::map <uint32, uint32> m_OpcodeBroadCastStats;

        void SetUnloadTimer(uint32 p_Timer) { m_unloadTimer = p_Timer; }

        void KickAllPlayersOutOfMap();
        bool KickPlayerOutOfMap(Player* p_Player);

        uint32 GetAverageDelay() { return m_AverageDiff; };

        void AddBigObject(WorldObject* p_WorldObject) { m_BigObjects.insert(p_WorldObject); }
        void RemoveBigObject(WorldObject* p_WorldObject) { m_BigObjects.erase(p_WorldObject); }
        BigObjectContainer const& GetBigObjects() const { return m_BigObjects; }

        void BroadcastMessages(bool p_UseThreadPool);

        bool m_GridsNotFullyLoadedInVisit;

        void AddLeavingPlayer(uint64 p_GUID) { m_LeavingPlayers.insert(p_GUID); }
        void RemoveLeavingPlayer(uint64 p_GUID) { m_LeavingPlayers.erase(p_GUID); }

        static Map* GetCurrentMapThread();
        static void SetLastAuraRemoved(uint32 p_SpellId);

        template<class T>
        void AddNearbyToGrid(T* object, Cell const& cell);

        void CloseMap() { m_Closed = true; }
        void OpenMap() { m_Closed = false; }
        bool IsClosed() { return m_Closed; }

        void AddCorpse(Corpse* p_Corpse);
        void RemoveCorpse(Corpse* p_Corpse);
        Corpse* ConvertCorpseToBones(uint64 p_PlayerGuid, bool p_Insignia =false);
        void RemoveOldCorpses();

        OutdoorPvP* GetOutdoorPvP() const { return m_OutdoorPvP; }
        OutdoorPvEScript* GetOudoorPvE() const{ return m_OutdoorPvE; }
        void SetOutoorPve(OutdoorPvEScript* p_OutdoorPve) { m_OutdoorPvE = p_OutdoorPve; }

        WildBattlePetZonePools* GetWildBattlePetPools() const { return m_WildBattlePetPools; }
        QueryCallbackManagerPtr GetQueryCallbackMgr() { return m_QueryCallbackMgr; }

        void SetLastUpdateTime(uint32 p_LastUpdatTime) { m_LastUpdateTime = p_LastUpdatTime; }
        uint32 GetLastUpdateTime() const { return m_LastUpdateTime; }
        uint32 GetLastDiff() const { return m_LastDiff; }

        bool IsLootedGameObject(uint32 p_DBGuid) const { return m_LootedGameobjects.count(p_DBGuid) != 0; }
        void SaveLootedGameObject(uint32 p_DBGuid);

        ACE_thread_t m_CurrentThreadId;
        uint32 m_LastCreatureUpdatedEntry;
        float m_LastCreatureUpdatedX;
        float m_LastCreatureUpdatedY;
        float m_LastCreatureUpdatedZ;

        void AddGridMapReference(const GridCoord &p)
        {
            ++GridMapReference[p.x_coord][p.y_coord];
            SetUnloadReferenceLock(GridCoord(63 - p.x_coord, 63 - p.y_coord), true);
        }

        void RemoveGridMapReference(GridCoord const& p)
        {
            --GridMapReference[p.x_coord][p.y_coord];
            if (!GridMapReference[p.x_coord][p.y_coord])
                SetUnloadReferenceLock(GridCoord(63 - p.x_coord, 63 - p.y_coord), false);
        }

        void AddObject(WorldObject* p_Object) { m_ObjectsInMap.insert(p_Object); }
        void RemoveObject(WorldObject* p_Object) { m_ObjectsInMap.erase(p_Object); }
        std::unordered_set<WorldObject*> const& GetObjectsOnMap() const { return m_ObjectsInMap; }

        void ApplyOnEveryPlayer(std::function<void(Player*)> function);

    private:
        void LoadMapAndVMap(int gx, int gy);
        void LoadVMap(int gx, int gy);
        void LoadMap(int gx, int gy, bool reload = false);
        void LoadMMap(int gx, int gy);
        GridMap* GetGrid(float x, float y);

        void SetTimer(uint32 t) { i_gridExpiry = t < MIN_GRID_DELAY ? MIN_GRID_DELAY : t; }

        void SendInitSelf(Player* player, bool p_Switched);

        bool CreatureCellRelocation(Creature* creature, Cell new_cell);
        bool GameObjectCellRelocation(GameObject* go, Cell new_cell);

        template<class T> void InitializeObject(T* obj);
        void AddCreatureToMoveList(Creature* c, float x, float y, float z, float ang);
        void AddGameObjectToMoveList(GameObject* go, float x, float y, float z, float ang);
        void RemoveGameObjectFromMoveList(GameObject* go);

        bool _creatureToMoveLock;
        std::vector<Creature*> _creaturesToMove;

        bool _gameObjectsToMoveLock;
        std::vector<GameObject*> _gameObjectsToMove;

        ACE_Based::LockedQueue<std::function<void()>, ACE_Thread_Mutex> m_Tasks;

        bool IsGridLoaded(const GridCoord &) const;
        void EnsureGridCreated(const GridCoord &);
        bool EnsureGridLoaded(Cell const&);
        void EnsureGridLoadedForActiveObject(Cell const&, WorldObject* object);

        void buildNGridLinkage(NGridType* pNGridType) { pNGridType->link(this); }

        NGridType* getNGrid(uint32 x, uint32 y) const
        {
            //ASSERT(x < MAX_NUMBER_OF_GRIDS && y < MAX_NUMBER_OF_GRIDS);
            if (x > MAX_NUMBER_OF_GRIDS && y > MAX_NUMBER_OF_GRIDS)
            {
                sLog->outError(LOG_FILTER_GENERAL, "CRASH::map::setNGrid() Invalid grid coordinates found: %d, %d!", x, y);
                return NULL;
            }
            return i_grids[x][y];
        }

        bool isGridObjectDataLoaded(uint32 x, uint32 y) const { return getNGrid(x, y) ? getNGrid(x, y)->isGridObjectDataLoaded() : false; }
        void setGridObjectDataLoaded(bool pLoaded, uint32 x, uint32 y) { getNGrid(x, y)->setGridObjectDataLoaded(pLoaded); }

        void setNGrid(NGridType* grid, uint32 x, uint32 y);
        void ScriptsProcess();

        void UpdateActiveCells(const float &x, const float &y, const uint32 t_diff);

    protected:

        void SetUnloadReferenceLock(const GridCoord &p, bool on)
        {
            if (NGridType* l_Grid = getNGrid(p.x_coord, p.y_coord))
                l_Grid->setUnloadReferenceLock(on);
        }

        ACE_Thread_Mutex Lock;
        ACE_RW_Thread_Mutex MMapLock;

        MapEntry const* i_mapEntry;
        uint8 i_spawnMode;
        uint32 i_InstanceId;
        uint32 m_ScenarioGuid;
        uint32 m_unloadTimer;
        float m_VisibleDistance;
        DynamicMapTree _dynamicTree;

        MapRefManager m_mapRefManager;
        MapRefManager::iterator m_mapRefIter;

        int32 m_VisibilityNotifyPeriod;

        typedef std::set<WorldObject*> ActiveNonPlayers;
        ActiveNonPlayers m_activeNonPlayers;
        ActiveNonPlayers::iterator m_activeNonPlayersIter;
        ACE_Recursive_Thread_Mutex m_activeNonPlayerLock;

        // Objects that must update even in inactive grids without activating them
        typedef std::set<GameObject*> TransportGameObjectContainer;
        TransportGameObjectContainer _transportsGameObject;
        TransportGameObjectContainer::iterator _transportsGameObjectUpdateIter;

        /// Objects that need to be seen from very large distance
        BigObjectContainer m_BigObjects;

        typedef std::set<Transport*> TransportsContainer;
        TransportsContainer _transports;
        TransportsContainer::iterator _transportsUpdateIter;

        std::unordered_set<uint64> m_ScriptedCollisionGobs;

        std::set<uint64> m_LeavingPlayers;
        OutdoorPvP* m_OutdoorPvP;
        OutdoorPvEScript* m_OutdoorPvE;
        Battlefield* m_BattleField;
        uint32 m_CustomMapSize;

        QueryCallbackManagerPtr m_QueryCallbackMgr;


    private:

        bool m_IsUpdating;

        bool m_BigObjectsLoaded;

        uint32 m_InstanceZoneId;

        Player* _GetScriptPlayerSourceOrTarget(Object* source, Object* target, const ScriptInfo* scriptInfo) const;
        Creature* _GetScriptCreatureSourceOrTarget(Object* source, Object* target, const ScriptInfo* scriptInfo, bool bReverse = false) const;
        Unit* _GetScriptUnit(Object* obj, bool isSource, const ScriptInfo* scriptInfo) const;
        Player* _GetScriptPlayer(Object* obj, bool isSource, const ScriptInfo* scriptInfo) const;
        Creature* _GetScriptCreature(Object* obj, bool isSource, const ScriptInfo* scriptInfo) const;
        WorldObject* _GetScriptWorldObject(Object* obj, bool isSource, const ScriptInfo* scriptInfo) const;
        void _ScriptProcessDoor(Object* source, Object* target, const ScriptInfo* scriptInfo) const;
        GameObject* _FindGameObject(WorldObject* pWorldObject, uint32 guid) const;

        time_t i_gridExpiry;

        //used for fast base_map (e.g. MapInstanced class object) search for
        //InstanceMaps and BattlegroundMaps...
        Map* m_parentMap;

        NGridType* i_grids[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
        GridMap* GridMaps[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
        std::bitset<TOTAL_NUMBER_OF_CELLS_PER_MAP*TOTAL_NUMBER_OF_CELLS_PER_MAP> marked_cells;

        bool i_scriptLock;
        std::set<WorldObject*> i_objectsToRemove;
        std::map<WorldObject*, bool> i_objectsToSwitch;
        std::set<WorldObject*> i_worldObjects;

        std::unordered_map<uint32/*cellId*/, std::unordered_set<Corpse*>> _corpsesByCell;
        std::unordered_map<uint64, Corpse*> _corpsesByPlayer;

        typedef std::multimap<time_t, ScriptAction> ScriptScheduleMap;
        ScriptScheduleMap m_scriptSchedule;

        // Type specific code for add/remove to/from grid
        template<class T>
        void AddToGrid(T* object, Cell const& cell);

        template<class T>
        void DeleteFromWorld(T*);

        template<class T>
        void AddToActiveHelper(T* obj)
        {
            m_activeNonPlayerLock.acquire();
            m_activeNonPlayers.insert(obj);
            m_activeNonPlayerLock.release();
        }

        template<class T>
        void RemoveFromActiveHelper(T* obj)
        {
            m_activeNonPlayerLock.acquire();
            // Map::Update for active object in proccess
            if (m_activeNonPlayersIter != m_activeNonPlayers.end())
            {
                ActiveNonPlayers::iterator itr = m_activeNonPlayers.find(obj);
                if (itr == m_activeNonPlayers.end())
                {
                    m_activeNonPlayerLock.release();
                    return;
                }
                if (itr == m_activeNonPlayersIter)
                    ++m_activeNonPlayersIter;
                m_activeNonPlayers.erase(itr);
            }
            else
                m_activeNonPlayers.erase(obj);
            m_activeNonPlayerLock.release();
        }

        bool m_Closed;

        uint32 m_AverageDiff;
        uint32 m_TotalDiff;
        uint8  m_TickCount;

        std::unordered_map<uint32 /*dbGUID*/, time_t> _creatureRespawnTimes;
        std::unordered_map<uint32 /*dbGUID*/, time_t> _goRespawnTimes;
        std::unordered_set<uint32 /*dbGUID*/> m_LootedGameobjects;

        std::unordered_set<Object*> _updateObjects;
        std::unordered_set<Unit*>   _updateVisibilityObjects;
        std::recursive_mutex m_UpdateObjectsLock;
        std::mutex m_UpdateVisibilityObjectsLock;

        typedef std::unordered_map<uint64, Corpse*> Player2CorpsesMapType;

        uint32 m_CorpseCleanupTimer;

        WildBattlePetZonePools* m_WildBattlePetPools;
        uint32 m_WildBattlePetPoolsUpdateTimer;

        uint32 m_LastUpdateTime;
        uint32 m_LastDiff;

        uint16 GridMapReference[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];

        std::unordered_set<WorldObject*> m_ObjectsInMap;
};

/*template<class T, bool log>
class ThreadGuard
{
public:
    ThreadGuard()
    {
        m_Data = new T();
        m_Unit = nullptr;
    }

    ~ThreadGuard()
    {
        delete m_Data;
    }

    void Initialize(Unit* p_Unit)
    {
        m_Unit = p_Unit;
    }

    template<class T, bool log>
    T* operator->();

private:
    T * m_Data;
    Unit* m_Unit;
};*/

enum InstanceResetMethod
{
    INSTANCE_RESET_ALL,
    INSTANCE_RESET_CHANGE_DIFFICULTY,
    INSTANCE_RESET_GLOBAL,
    INSTANCE_RESET_GROUP_DISBAND,
    INSTANCE_RESET_GROUP_JOIN,
    INSTANCE_RESET_RESPAWN_DELAY
};

class InstanceMap : public Map
{
    public:
        InstanceMap(uint32 id, time_t, uint32 InstanceId, uint8 SpawnMode, Map* _parent);
        ~InstanceMap();
        bool AddPlayerToMap(Player*, bool p_Switched = false);
        void RemovePlayerFromMap(Player*, bool);
        void Update(const uint32, const uint32);
        void CreateInstanceData(bool load);
        bool Reset(uint8 method);
        uint32 GetScriptId() { return i_script_id; }
        InstanceScript* GetInstanceScript() { return i_data; }
        void PermBindAllPlayers(Player* source);
        void UnloadAll();
        bool CanEnter(Player* player);
        void SendResetWarnings(uint32 timeLeft) const;
        void SetResetSchedule(bool on);

        /* this checks if any players have a permanent bind (included reactivatable expired binds) to the instance ID
        it needs a DB query, so use sparingly */
        bool HasPermBoundPlayers() const;

        uint32 GetMaxPlayers() const;
        uint32 GetMaxResetDelay() const;

        virtual void InitVisibilityDistance();

        void ResetDelayIfNeeded();
        void IncrementDelayCount(uint32 p_DelayDiff);
        void SetUnloadAfterEmpty();

    private:
        bool m_resetAfterUnload;
        bool m_unloadWhenEmpty;
        InstanceScript* i_data;
        uint32 i_script_id;

        struct DelayInfo
        {
            uint32 m_Counter;
            uint32 m_DelaySum;
            uint32 m_LastTime;
        };

        DelayInfo m_Delay;
        bool m_CanEnterUnload;
};

class BattlegroundMap : public Map
{
    public:
        BattlegroundMap(uint32 id, time_t, uint32 InstanceId, Map* _parent, uint8 spawnMode);
        ~BattlegroundMap();

        bool AddPlayerToMap(Player*, bool p_Switched = false);
        void RemovePlayerFromMap(Player*, bool);
        bool CanEnter(Player* player);
        void SetUnload();
        void Update(const uint32, const uint32);
        //void UnloadAll(bool pForce);
        void RemoveAllPlayers();

        virtual void InitVisibilityDistance();
        Battleground* GetBG() { return m_bg; }
        void SetBG(Battleground* bg) { m_bg = bg; }
    private:
        Battleground* m_bg;
};

template<class T, class CONTAINER>
inline void Map::Visit(Cell const& cell, TypeContainerVisitor<T, CONTAINER>& visitor)
{
    const uint32 x = cell.GridX();
    const uint32 y = cell.GridY();
    const uint32 cell_x = cell.CellX();
    const uint32 cell_y = cell.CellY();

    if (!cell.NoCreate() || IsGridLoaded(GridCoord(x, y)))
    {
        EnsureGridLoaded(cell);
        getNGrid(x, y)->VisitGrid(cell_x, cell_y, visitor);
    }
    else
        m_GridsNotFullyLoadedInVisit = true;
}

template<class T, class CONTAINER>
inline void Map::VisitWithNearbies(Cell const& cell, TypeContainerVisitor<T, CONTAINER>& visitor)
{
    const uint32 x = cell.GridX();
    const uint32 y = cell.GridY();
    const uint32 cell_x = cell.CellX();
    const uint32 cell_y = cell.CellY();

    if (!cell.NoCreate() || IsGridLoaded(GridCoord(x, y)))
    {
        EnsureGridLoaded(cell);
        getNGrid(x, y)->VisitGridWithNearbies(cell_x, cell_y, visitor);
    }
}

template<class NOTIFIER>
inline void Map::VisitAll(float const& x, float const& y, float radius, NOTIFIER& notifier, bool loadGrids)
{
    CellCoord p(JadeCore::ComputeCellCoord(x, y));
    Cell cell(p);
    if (!loadGrids)
        cell.SetNoCreate();

    TypeContainerVisitor<NOTIFIER, WorldTypeMapContainer> world_object_notifier(notifier);
    cell.Visit(p, world_object_notifier, *this, radius, x, y);
    TypeContainerVisitor<NOTIFIER, GridTypeMapContainer >  grid_object_notifier(notifier);
    cell.Visit(p, grid_object_notifier, *this, radius, x, y);
}

// should be used with Searcher notifiers, tries to search world if nothing found in grid
template<class NOTIFIER>
inline void Map::VisitFirstFound(const float &x, const float &y, float radius, NOTIFIER &notifier, bool loadGrids)
{
    CellCoord p(JadeCore::ComputeCellCoord(x, y));
    Cell cell(p);
    if (!loadGrids)
        cell.SetNoCreate();

    TypeContainerVisitor<NOTIFIER, WorldTypeMapContainer> world_object_notifier(notifier);
    cell.Visit(p, world_object_notifier, *this, radius, x, y);
    if (!notifier.i_object)
    {
        TypeContainerVisitor<NOTIFIER, GridTypeMapContainer >  grid_object_notifier(notifier);
        cell.Visit(p, grid_object_notifier, *this, radius, x, y);
    }
}

template<class NOTIFIER>
inline void Map::VisitWorld(const float &x, const float &y, float radius, NOTIFIER &notifier, bool loadGrids)
{
    CellCoord p(JadeCore::ComputeCellCoord(x, y));
    Cell cell(p);
    if (!loadGrids)
        cell.SetNoCreate();

    TypeContainerVisitor<NOTIFIER, WorldTypeMapContainer> world_object_notifier(notifier);
    cell.Visit(p, world_object_notifier, *this, radius, x, y);
}

template<class NOTIFIER>
inline void Map::VisitGrid(const float &x, const float &y, float radius, NOTIFIER &notifier, bool loadGrids)
{
    CellCoord p(JadeCore::ComputeCellCoord(x, y));
    Cell cell(p);
    if (!loadGrids)
        cell.SetNoCreate();

    TypeContainerVisitor<NOTIFIER, GridTypeMapContainer >  grid_object_notifier(notifier);
    cell.Visit(p, grid_object_notifier, *this, radius, x, y);
}
#endif
