////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITYCORE_AREATRIGGER_H
#define TRINITYCORE_AREATRIGGER_H

#include "Object.h"
#include <G3D/Vector3.h>

class Unit;
class SpellInfo;
class Spell;
class AreaTriggerEntityScript;
class AreaTriggerAI;

struct AreaTriggerData
{
    explicit AreaTriggerData() : m_DBData(true) { }

    uint32 Guid = 0;
    uint32 Entry;
    uint32 CustomEntry;
    uint32 SpellID;
    uint16 MapID;
    uint16 ZoneID = 0;
    uint16 AreaID = 0;
    uint32 PhaseMask = 1;
    float PosX;
    float PosY;
    float PosZ;
    float Orientation;
    uint32 SpawnMask = 1;
    bool m_DBData;
    uint32 PhaseID;
    float Radius = 0.0f;
    float RadiusTarget = 0.0f;
    float Height = 0.0f;
    float HeightTarget = 0.0f;
    uint32 DecalPropertiesId = 0;
    uint32 MorphCurveId = 0;
};

struct AreaTriggerPolygon
{
    std::unordered_map<uint32, G3D::Vector2> Vertices;
    std::unordered_map<uint32, G3D::Vector2> VerticesTarget;
    float Height = 0.0f;
    float HeightTarget = 0.0f;
};

enum eAreaTriggerCircleTypes
{
    CenterTypeNone,
    CenterTypeCaster,
    CenterTypeTarget,
    CenterTypeSource
};

struct AreaTriggerCircle
{
    bool Initialized = false;
    Optional<uint64> TargetGuid;
    Optional<G3D::Vector3> CenterPoint;
    uint32 TimeToTarget = 0;
    int32 dword2C = 0;
    uint32 dword30 = 0;
    float Radius = 0.0f;
    float BlendFormRadius = 0.0f;
    float InitialAngle = 0.0f;
    float ZOffset = 0.0f;
    bool Clockwise = false;
    bool byte45 = false;
    eAreaTriggerCircleTypes Type = eAreaTriggerCircleTypes::CenterTypeNone;
};

struct AreaTriggerSpline
{
    int32 TimeToTarget = 0;
    int32 ElapsedTimeForMovement = 0;
    std::vector<G3D::Vector3> VerticesPoints;
};

enum AreaTriggerActionMoment
{
    AT_ACTION_MOMENT_ENTER          = 0x0001, ///< when unit enters areatrigger
    AT_ACTION_MOMENT_LEAVE          = 0x0002, ///< when unit exits areatrigger
    AT_ACTION_MOMENT_UPDATE_TARGET  = 0x0004, ///< on areatrigger update target
    AT_ACTION_MOMENT_DESPAWN        = 0x0008, ///< when areatrigger despawn
    AT_ACTION_MOMENT_SPAWN          = 0x0010, ///< when areatrigger spawn
    AT_ACTION_MOMENT_REMOVE         = 0x0020, ///< when areatrigger remove
    /// Range should be = distance.
    AT_ACTION_MOMENT_ON_THE_WAY     = 0x0040, ///< when target is betwin source and dest points. For movement only. WARN! Should add AT_ACTION_MOMENT_ENTER flag too
    AT_ACTION_MOMENT_ON_STOP_MOVE   = 0x0080, ///< when target is betwin source and dest points. For movement only. WARN! Should add AT_ACTION_MOMENT_ENTER flag too
    AT_ACTION_MOMENT_ON_ACTIVATE    = 0x0100, ///< when areatrigger active
    AT_ACTION_MOMENT_ON_CAST_ACTION = 0x0200, ///< when areatrigger cast from aura tick
    AT_ACTION_MOMENT_UPDATE         = 0x0400, ///< on areatrigger update
    AT_ACTION_MOMENT_ON_DESPAWN     = 0x0800, ///< on areatrigger despawn use with AOE spell
    AT_ACTION_MOMENT_LEAVE_ALL      = 0x1000, ///< when all units exits areatrigger
    AT_ACTION_MOMENT_TARGET_REACHED = 0x2000  ///< When areatrigger reaches his destination
};

enum AreaTriggerActionType
{
    AT_ACTION_TYPE_CAST_SPELL                   = 0,    ///< Hit mask 0x00001
    AT_ACTION_TYPE_REMOVE_AURA                  = 1,    ///< Hit mask 0x00002
    AT_ACTION_TYPE_ADD_STACK                    = 2,    ///< Hit mask 0x00004
    AT_ACTION_TYPE_REMOVE_STACK                 = 3,    ///< Hit mask 0x00008
    AT_ACTION_TYPE_CHANGE_SCALE                 = 4,    ///< Hit mask 0x00010
    AT_ACTION_TYPE_SHARE_DAMAGE                 = 5,    ///< Hit mask 0x00020
    AT_ACTION_TYPE_APPLY_MOVEMENT_FORCE         = 6,    ///< Hit mask 0x00040
    AT_ACTION_TYPE_REMOVE_MOVEMENT_FORCE        = 7,    ///< Hit mask 0x00080
    AT_ACTION_TYPE_CHANGE_DURATION_ANY_AT       = 8,    ///< Hit mask 0x00100
    AT_ACTION_TYPE_CHANGE_AMOUNT_FROM_HEALT     = 9,    ///< Hit mask 0x00200
    AT_ACTION_TYPE_RE_PATCH                     = 10,   ///< Hit mask 0x00400
    AT_ACTION_TYPE_SET_AURA_CUSTOM_ADD          = 11,   ///< Hit mask 0x00800
    AT_ACTION_TYPE_SET_AURA_CUSTOM_REMOVE       = 12,   ///< Hit mask 0x01000
    AT_ACTION_TYPE_SLOW                         = 13,   ///< Hit mask 0x02000
    AT_ACTION_TYPE_UPDATE_TARGETS               = 14,   ///< Hit mask 0x04000
    AT_ACTION_TYPE_SET_IN_MOTION                = 15,   ///< Hit mask 0x08000
    AT_ACTION_TYPE_TARGETS_SCALE                = 16,   ///< Hit mask 0x10000
    AT_ACTION_TYPE_NO_TARGETS_SCALE             = 17,   ///< Hit mask 0x20000
    AT_ACTION_TYPE_SET_AURA_DURATION            = 18,   ///< Hit mask 0x40000
    AT_ACTION_TYPE_RE_SHAPE                     = 19,   ///< Hit mask 0x80000
    AT_ACTION_TYPE_NO_ACTION                    = 20,   ///< 

    AT_ACTION_TYPE_MAX
};

enum AreaTriggerTargetFlags
{
    AT_TARGET_FLAG_FRIENDLY             = 0x000001,             // casted on targets that are friendly to areatrigger owner
    AT_TARGET_FLAG_VALIDATTACK          = 0x000002,             // casted on targets that are valid attcak to areatrigger owner
    AT_TARGET_FLAG_OWNER                = 0x000004,             // casted only on areatrigger owner
    AT_TARGET_FLAG_PLAYER               = 0x000008,             // casted only on players
    AT_TARGET_FLAG_NOT_PET              = 0x000010,             // casted on everyone except pets
    AT_TARGET_FLAG_CAST_AT_SRC          = 0x000020,             // casted on areatrigger position as dest
    AT_TARGET_FLAG_CASTER_IS_TARGET     = 0x000040,             // casted on areatrigger caster is target
    AT_TARGET_FLAG_NOT_FULL_HP          = 0x000080,             // casted on targets if not full hp
    AT_TARGET_FLAG_ALWAYS_TRIGGER       = 0x000100,             // casted always at any action on owner
    AT_TARGET_FLAT_IN_FRONT             = 0x000200,             // WARNING! If target come from back he not get cast. ToDo it..
    AT_TARGET_FLAG_NOT_FULL_ENERGY      = 0x000400,             // casted on targets if not full enegy
    AT_TARGET_FLAG_GROUP_OR_RAID        = 0x000800,             // casted on targets that in group to areatrigger owner
    AT_TARGET_FLAG_HOSTILE              = 0x001000,             // casted on targets that are hostile to areatrigger owner
    AT_TARGET_FLAG_TARGET_IS_CASTER     = 0x002000,             // casted on areatrigger target is caster
    AT_TARGET_FLAG_CAST_AURA_TARGET     = 0x004000,             // casted on aura target
    AT_TARGET_FLAG_NOT_AURA_TARGET      = 0x008000,             // casted on target is not aura target
    AT_TARGET_FLAG_TARGET_IS_SUMMONER   = 0x010000,
    AT_TARGET_FLAG_NOT_OWNER            = 0x020000,
    AT_TARGET_FLAG_NPC_ENTRY            = 0x040000,
    AT_TARGET_FLAG_TARGET_PASSANGER     = 0x080000,
    AT_TARGET_FLAG_TARGET_PASSANGER_VEH = 0x100000,
    AT_TARGET_FLAG_TARGET_IS_CASTER_2   = 0x200000,
    AT_TARGET_FLAG_TARGET_CHECK_LOS     = 0x400000,

    AT_TARGET_MASK_REQUIRE_TARGET =
        AT_TARGET_FLAG_FRIENDLY | AT_TARGET_FLAG_HOSTILE | AT_TARGET_FLAG_OWNER | AT_TARGET_FLAG_PLAYER |
        AT_TARGET_FLAG_NOT_PET  | AT_TARGET_FLAG_CASTER_IS_TARGET | AT_TARGET_FLAG_NOT_FULL_HP | AT_TARGET_FLAG_ALWAYS_TRIGGER | AT_TARGET_FLAT_IN_FRONT,
};

enum AreaTriggerMoveType
{
    AT_MOVE_TYPE_DEFAULT            = 0,
    AT_MOVE_TYPE_LIMIT              = 1,
    AT_MOVE_TYPE_SPIRAL             = 2,
    AT_MOVE_TYPE_BOOMERANG          = 3,
    AT_MOVE_TYPE_CHAGE_ROTATION     = 4,
    AT_MOVE_TYPE_RE_PATH            = 5,
    AT_MOVE_TYPE_RANDOM             = 6,
    AT_MOVE_TYPE_TO_TARGET          = 7,
    AT_MOVE_TYPE_RE_PATH_LOS        = 8,
    AT_MOVE_TYPE_PART_PATH          = 9,
    AT_MOVE_TYPE_STATIC_SPLINE      = 10,
    AT_MOVE_TYPE_RE_SHAPE           = 11,
    AT_MOVE_TYPE_CIRCLE             = 12,
    AT_MOVE_TYPE_SOURCE_ANGLE       = 13,
    AT_MOVE_TYPE_DEST               = 14,
    AT_MOVE_TYPE_BOUNCE_ON_COL      = 15,
    AT_MOVE_TYPE_THROUGH_WALLS      = 16
};

struct AreaTriggerAction
{
    uint32 id;
    AreaTriggerActionMoment moment;
    AreaTriggerActionType actionType;
    AreaTriggerTargetFlags targetFlags;
    uint32 spellId;
    int8 maxCharges;
    uint32 chargeRecoveryTime;
    int32 aura;
    int32 hasspell;
    float scale;
    int32 hitMaxCount;
    int32 amount;
    uint8 onDespawn;
};

typedef std::list<AreaTriggerAction> AreaTriggerActionList;

struct AreaTriggerInfo
{
    AreaTriggerInfo() : activationDelay(0), updateDelay(0), scalingDelay(1), maxCount(0),
        spellId(0), customEntry(0), isMoving(false), isCheckingWithObjectSize(true), isCheckingNeighbours(true), speed(0.0f), moveType(0), hitType(0),
        RadiusTarget(0.0f), LocationZOffsetTarget(0.0f), LocationZOffset(0.0f), Radius(0.0f),
        MoveCurveID(0), ElapsedTime(0), windX(0.0f), windY(0.0f), windZ(0.0f), windSpeed(0.0f), windType(0),
        MorphCurveID(0), FacingCurveID(0), ScaleCurveID(0), ShapeID1(0), ShapeID2(0), HasUnkBit50(false), UnkBit50(false), HasFollowsTerrain(0), HasAttached(0), HasAbsoluteOrientation(0),
        HasDynamicShape(0), HasFaceMovementDir(0), hasAreaTriggerBox(0), RollPitchYaw1X(0.0f), RollPitchYaw1Y(0.0f), RollPitchYaw1Z(0.0f),
        TargetRollPitchYawX(0.0f), TargetRollPitchYawY(0.0f), TargetRollPitchYawZ(0.0f), Distance(0.0f), DecalPropertiesId(0), AngleToCaster(0.0f),
        maxActiveTargets(0), Param(0.0f), waitTime(0), timeToTarget(0), ScriptId(0), VisualID(0) {}

    AreaTriggerPolygon Polygon;
    bool isMoving;
    bool isCheckingWithObjectSize;
    bool isCheckingNeighbours;
    uint32 spellId;
    uint32 DecalPropertiesId;
    uint32 activationDelay;
    uint32 updateDelay;
    uint32 scalingDelay;
    uint32 customEntry;
    uint8 maxCount;
    AreaTriggerActionList actions;
    float speed;
    uint32 moveType;
    uint32 waitTime;
    uint32 hitType;
    float Radius;
    float RadiusTarget;

    float LocationZOffset;
    float LocationZOffsetTarget;
    uint32 MoveCurveID;
    uint32 ElapsedTime;
    uint32 MorphCurveID;
    uint32 FacingCurveID;
    uint32 ScaleCurveID;
    uint32 ShapeID1;
    uint32 ShapeID2;
    bool HasUnkBit50;
    bool UnkBit50;
    uint32 HasFollowsTerrain;
    uint32 HasAttached;
    uint32 HasAbsoluteOrientation;
    uint32 HasDynamicShape;
    uint32 HasFaceMovementDir;
    uint32 hasAreaTriggerBox;
    uint32 windType;
    float windX;
    float windY;
    float windZ;
    float windSpeed;
    float RollPitchYaw1X;
    float RollPitchYaw1Y;
    float RollPitchYaw1Z;
    float TargetRollPitchYawX;
    float TargetRollPitchYawY;
    float TargetRollPitchYawZ;
    float Distance;
    uint32 polygon;
    float AngleToCaster;
    uint8 maxActiveTargets;
    float Param;
    uint32 ScriptId;
    uint32 timeToTarget;
    uint32 VisualID;
};

struct AreaTriggerMoveSplines
{
    uint32 m_move_id;
    uint32 m_path_id;
    float m_path_x;
    float m_path_y;
    float m_path_z;

    Position ToPosition();
};

typedef std::map<std::pair<uint32, uint32>, AreaTriggerMoveSplines> AreaTriggerMoveSplinesContainer;

class AreaTrigger : public WorldObject, public GridObject<AreaTrigger>
{
    private:
        struct ActionInfo
        {
            ActionInfo() : hitCount(0), charges(0), recoveryTime(0), amount(0), DespawnFromCharges(0), action(nullptr) { }
            ActionInfo(AreaTriggerAction const* _action) :
                hitCount(0), charges(_action->maxCharges), recoveryTime(_action->chargeRecoveryTime), amount(_action->amount), DespawnFromCharges(_action->onDespawn), action(_action) {}

            uint32 hitCount;
            uint8 charges;
            uint32 recoveryTime;
            int32 amount;
            uint8 DespawnFromCharges;
            AreaTriggerAction const* action;
        };
        typedef std::map<uint8, ActionInfo> ActionInfoMap;

    public:

        struct AreaTriggerSource
        {
            AreaTriggerSource() : x(0), y(0), z(0), o(0), map(nullptr), phasemask(0) {}
            AreaTriggerSource(float p_X, float p_Y, float p_Z, float p_O, Map* p_Map, uint32 p_Phasemask) :
                x(p_X), y(p_Y), z(p_Z), o(p_O), map(p_Map), phasemask(p_Phasemask) {}

            float x;
            float y;
            float z;
            float o;
            Map* map;
            uint32 phasemask;

            Position GetPosition() const { return Position(x, y, z, o); }
        };

        AreaTrigger();
        ~AreaTrigger();

        void BuildValuesUpdate(uint8 updatetype, ByteBuffer* data, Player* target) const override;

        void AddToWorld() override;
        void RemoveFromWorld() override;

        bool CreateAreaTrigger(uint32 guidlow, uint32 triggerEntry, Unit* caster, Unit* p_OriginalCaster, SpellInfo const* info, Position const& pos, Position const& posMove, Spell* spell = nullptr, uint64 targetGuid = 0, uint32 customEntry = 0);

        bool LoadFromDB(uint32 p_Guid, Map* p_Map) { return LoadAreaTriggerFromDB(p_Guid, p_Map); }
        bool LoadAreaTriggerFromDB(uint32 p_Guid, Map* p_Map);

        void FillVerticesPoint(Position* l_Source, float p_MinDist = 0.0f);
        void BounceAreaTrigger(float p_ForcedAngle = 0.0f, float p_MinDist = 0.0f);
        void Update(uint32 p_time) override;
        void UpdateScaleCurve(uint32 p_Diff);
        void UpdateMorphCurve(uint32 p_Diff);
        void UpdateMoveCurve();
        void UpdateAffectedList(uint32 p_time, AreaTriggerActionMoment actionM);
        void Remove(bool duration = true);
        void Despawn();
        uint32 GetSpellId() const { return GetUInt32Value(AREATRIGGER_FIELD_SPELL_ID); }
        void SetSpellId(uint32 spell) { return SetUInt32Value(AREATRIGGER_FIELD_SPELL_ID, spell); }
        ObjectGuid GetCasterGUID() const { return GetGuidValue(AREATRIGGER_FIELD_CASTER); }
        Unit* GetCaster() const;
        AreaTriggerSource GetSource() const;
        AreaTriggerSource SetSource(float p_X = 0, float p_Y = 0, float p_Z = 0, float p_O = 0, Map* p_Map = nullptr, uint32 p_Phasemask = 0);
        AreaTriggerSource SetSource(Position p_Position, Map* p_Map = nullptr, uint32 p_PhaseMask = 0);
        void SetAreatriggerSpellCasterGuid(uint64 p_AreatriggerSpellCasterGuid) { _areatriggerSpellCasterGuid = p_AreatriggerSpellCasterGuid; }
        uint64 GetAreatriggerSpellCasterGuid() { return _areatriggerSpellCasterGuid; }
        void SetTargetGuid(uint64 targetGuid) { _targetGuid = targetGuid; }
        uint64 GetTargetGuid() { return _targetGuid; }
        void SetCreatingEffectGuid(Guid128 p_Guid);
        int32 GetDuration() const { return _duration; }
        void SetDuration(int32 newDuration) { _duration = newDuration; }
        int32 GetTimeToTarget() const { return _spline.TimeToTarget; }
        void SetTimeToTarget(int32 timeToTarget) { _spline.TimeToTarget = timeToTarget; }
        void Delay(int32 delaytime) { SetDuration(GetDuration() - delaytime); }
        float GetVisualScale(bool max = false) const;
        float GetRadius() const;
        void SetRadius(float p_Radius);
        void CalculateRadius(Spell* spell = nullptr);
        uint32 GetCustomEntry() const { return atInfo.customEntry; }
        uint32 GetRealEntry() const { return _realEntry; }
        bool IsUnitAffected(uint64 guid) const;
        void AffectUnit(Unit* unit, AreaTriggerActionMoment actionM);
        void AffectOwner(AreaTriggerActionMoment actionM);
        void UpdateOnUnit(Unit* unit);
        void DoAction(Unit* unit, ActionInfo& action);
        void ActionOnDespawn();
        void TargetReached(Unit* p_Target);
        bool CheckActionConditions(AreaTriggerAction const& action, Unit* unit);
        void UpdateActionCharges(uint32 p_time);
        bool GetAreaTriggerCylinder() const { return _areaTriggerCylinder; }
        bool HasTargetRollPitchYaw() const { return atInfo.TargetRollPitchYawX != 0.0f || atInfo.TargetRollPitchYawY != 0.0f || atInfo.TargetRollPitchYawZ != 0.0f; }
        bool HasPolygon() const { return atInfo.polygon && !atInfo.hasAreaTriggerBox && !atInfo.Polygon.Vertices.empty(); }
        bool HasCircleData() const { return _CircleData.Initialized; }
        AreaTriggerCircle const* GetCircleData() const { return &_CircleData; };
        bool CheckValidateTargets(Unit* unit, AreaTriggerActionMoment actionM);

        AreaTriggerInfo const& GetAreaTriggerInfo() const { return atInfo; }

        void SetMoveType(uint8 p_Type) { atInfo.moveType = p_Type; }

        void CastAction();
        bool UpdatePosition(ObjectGuid targetGuid);

        void MoveAreaTrigger(Position const p_Position, uint32 p_TimeToTargetOrSpeed = 0, bool p_IsSpeed = false, bool p_FollowTerrain = true);
        void MoveAreaTrigger(WorldLocation const* p_Dest, uint32 p_TimeToTargetOrSpeed = 0, bool p_IsSpeed = false, bool p_FollowTerrain = true);
        void MoveAreaTriggerCircle(Position const p_Center, float p_Radius, uint32 p_TimeToTargetOrSpeed = 0, bool p_IsSpeed = false, bool p_Clockwise = true);
        void UpdateSpeedTriggerCircle(uint32 p_TimeToTargetOrSpeed, bool p_IsSpeed = false);

        void ReShape(uint32 p_NewShape, bool p_UnkBit);

        void BindToCaster();
        void UnbindFromCaster();

        SpellInfo const*  GetSpellInfo() { return m_spellInfo; }

        //movement
        void UpdateMovement(uint32 diff);
        void UpdatePolygon();
        void UpdatePositionWithPathId(uint32 p_Time, Position* p_OutPos);
        void UpdatePositionForCircle(Position* p_OutPos);
        void UpdateRotation(uint32 diff);
        bool isMoving() const { return _canMove; }
        bool UseMoveCurveID() const { return m_UseMoveCurveID; }
        float getMoveSpeed() const { return _moveSpeed; }
        bool HasSpline() const { return !_spline.VerticesPoints.empty(); }
        bool IsInHeight(WorldObject* p_ObjectTarget);
        bool IsInPolygon(WorldObject* p_ObjectTarget);
        float CalculateRadiusPolygon();
        AreaTriggerSpline GetSplineInfo() const { return _spline; }
        GuidList* GetAffectedPlayers() { return &affectedPlayers; }
        GuidList* GetAffectedPlayersForAllTime() { return &affectedPlayersForAllTime; }
        float getCircleSpeed() const { return m_speedCircle; }

        /*
         * Cast spell by using unit trigger/dummy at areatrigger posiiton
         * Use areatrigger owner (caster) for ennemies/friends selection
         * @param p_Target  : Target of the spell we cast
         * @param p_SpellId : Id of spell to cast
         */
        void CastSpell(Unit* p_Target, uint32 p_SpellId);
        void CastSpellByOwnerFromATLocation(Unit* p_Target, uint32 p_SpellId);
        /// Script
        AreaTriggerEntityScript* GetScript() const { return m_Script; }
        void SetScript(AreaTriggerEntityScript* p_Script) { m_Script = p_Script; }

        void SetCastItemGuid(uint64 p_ItemGuid) { m_CastItemGuid = p_ItemGuid; }
        uint64 GetCastItemGuid() const { return m_CastItemGuid; }

        FunctionProcessor m_Functions;

        void SetBounceRef(Position const p_Pos) { m_BounceRef = p_Pos; }

        void SetWindTransPos(G3D::Vector3 p_Pos) { m_WindTransPos = p_Pos; }

        std::vector<AreaTriggerMoveSplines>& GetSplineData() { return _splineData; }
        bool IsCheckingTargetWithObjectSize() const;
        bool CanSearchDeadUnits() const { return m_CanAffectDead; }
        void SetCanSearchDead(bool p_Apply) { m_CanAffectDead = p_Apply; }
        void SetCurveChanged(bool p_Apply) { m_CurveChanged = p_Apply; }

        uint32 GetLiveTime() const { return _liveTime; }
        bool IsOnRemove() const { return _on_remove; }
        bool IsOnDespawn() const { return _on_despawn; }
        void SetOnDespawn(bool p_Apply) { _on_despawn = p_Apply; }

        WorldLocation const GetDestination() const { return _dest; }

        void SetCustomData(int32 data) { m_customData = data; }
        int32 GetCustomData() { return m_customData; }
        void ModCustomData(int32 data) { m_customData += data; }

        void Relocate(Position const& p_Pos) override;
        void UpdateNeighbours();
        void AddNeighbour(uint64 p_AreaTriggerGUID);
        void RemoveNeighbour(uint64 p_AreaTriggerGUID);
        void RemoveFromNeighbours();

        void GetOverlappingAreatriggers(std::list<AreaTrigger*>& p_AreaTriggers);

        void GetAttackableUnitListInPositionRange(std::list<Unit*> &list, Position const p_Position, float fMaxSearchRange, bool p_ObjectSize = true, bool p_AliveCheck = true) const override;

        void DisableScaling() { atInfo.Param = 0; }

        void SetCircleAreaTrigger(float p_Radius);

        void AI_Initialize();
        void AI_Destroy();

        AreaTriggerAI* AI();

    private:
        bool _HasActionsWithCharges(AreaTriggerActionMoment action = AT_ACTION_MOMENT_ENTER);
        bool HasScalingAction() const;
        void FillCustomData(Unit* caster);
        bool _IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D) const override;

    protected:
        AreaTriggerSource _source;
        Unit* _caster;
        uint64 _areatriggerSpellCasterGuid;
        uint64 _targetGuid;
        int32 _duration;
        uint32 _activationDelay;
        int32 m_updatePeriodic;
        int32 _updateDelay;
        uint32 m_ScalingDelay;
        uint32 _liveTime;
        GuidList affectedPlayers;
        GuidList affectedPlayersForAllTime;
        float _radius;
        float _range;
        AreaTriggerInfo atInfo;
        ActionInfoMap _actionInfo;
        uint32 _realEntry;
        float _moveSpeed;
        uint32 _moveTime;
        uint32 _nextMoveTime;
        uint32 _waitTime;
        bool _on_unload;
        bool _on_despawn;
        bool _on_remove;
        uint32 _hitCount;
        bool _areaTriggerCylinder;
        uint32 m_currentNode;
        float m_moveDistanceMax;
        float m_moveDistance;
        float m_moveLength;
        float m_moveAngleLos;
        bool _canMove;
        bool m_UseMoveCurveID;
        float m_speedCircle;

        AreaTriggerEntityScript* m_Script;

        AreaTriggerSpline _spline;
        std::vector<AreaTriggerMoveSplines> _splineData;
        float _length;
        std::vector<uint32> _times;
        uint32 _MoveElapsedDuration;
        uint32 m_MoveAreatriggerDelay;
        uint32 m_UpdateMoveTime;
        bool _IsMoving;
        AreaTriggerCircle _CircleData;
        WorldLocation _dest;
        std::unordered_map<uint32, G3D::Vector2> m_Polygon;

        SpellInfo const* m_spellInfo;

        uint64 m_CastItemGuid;

        bool m_CurveChanged;
        bool m_CanAffectDead;

        Position m_BounceRef;
        Position m_BounceSrc;
        Position m_BouncePos;

        Position m_LastMovePos;
        uint32 m_LastMoveCheckTimer;

        G3D::Vector3 m_WindTransPos;

        uint64 m_DBTableGuid;

        int32 m_customData;

        std::set<uint64> m_Neighbours;

        std::unique_ptr<AreaTriggerAI> _ai;
};
#endif
