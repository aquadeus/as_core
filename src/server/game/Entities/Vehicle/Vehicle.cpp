////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "Unit.h"
#include "Util.h"
#include "WorldPacket.h"
#include "ScriptMgr.h"
#include "CreatureAI.h"
#include "ZoneScript.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "MoveSplineInit.h"

Vehicle::Vehicle(Unit* unit, VehicleEntry const* vehInfo, uint32 creatureEntry) : _me(unit), _vehicleInfo(vehInfo), _usableSeatNum(0), _creatureEntry(creatureEntry),
 _status(STATUS_NONE), _passengersSpawnedByAI(false), _canBeCastedByPassengers(false)
{
    for (uint32 i = 0; i < MAX_VEHICLE_SEATS; ++i)
    {
        if (uint32 seatId = _vehicleInfo->SeatID[i])
            if (VehicleSeatEntry const* veSeat = sVehicleSeatStore.LookupEntry(seatId))
            {
                Seats.insert(std::make_pair(i, VehicleSeat(veSeat)));
                if (veSeat->CanEnterOrExit())
                    ++_usableSeatNum;
            }
    }

    InitMovementInfoForBase();
}

Vehicle::~Vehicle()
{
    /// @Uninstall must be called before this.
    ASSERT(_status == STATUS_UNINSTALLING);
    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
        ASSERT(!itr->second.Passenger);
}

void Vehicle::Install()
{
    if (Creature* creature = _me->ToCreature())
    {
        switch (_vehicleInfo->PowerDisplayID[0])
        {
            case POWER_STEAM:
            case POWER_HEAT:
            case POWER_BLOOD:
            case POWER_OOZE:
            case POWER_WRATH:
                _me->setPowerType(POWER_ENERGY);
                _me->SetMaxPower(POWER_ENERGY, 100);
                break;
            case POWER_PYRITE:
                _me->setPowerType(POWER_ENERGY);
                _me->SetMaxPower(POWER_ENERGY, 50);
                break;
            default:
                for (uint32 i = 0; i < MAX_SPELL_VEHICLE; ++i)
                {
                    if (!creature->m_spells[i])
                        continue;

                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(creature->m_spells[i]);
                    if (!spellInfo)
                        continue;

                    for (auto itr : spellInfo->SpellPowers)
                    {
                        if (itr->PowerType == POWER_ENERGY)
                        {
                            _me->setPowerType(POWER_ENERGY);
                            _me->SetMaxPower(POWER_ENERGY, 100);
                            break;
                        }
                    }
                }
                break;
        }
    }

    _status = STATUS_INSTALLED;
    if (GetBase()->GetTypeId() == TYPEID_UNIT)
        sScriptMgr->OnInstall(this);
}

void Vehicle::InstallAllAccessories(bool evading)
{
    if (ArePassengersSpawnedByAI())
        return;

    if (GetBase()->IsPlayer() || !evading)
        RemoveAllPassengers();   // We might have aura's saved in the DB with now invalid casters - remove

    VehicleAccessoryList const* accessories = sObjectMgr->GetVehicleAccessoryList(this);
    if (!accessories)
        return;

    for (VehicleAccessoryList::const_iterator itr = accessories->begin(); itr != accessories->end(); ++itr)
        if (!evading || itr->IsMinion)  // only install minions on evade mode
            InstallAccessory(&(*itr));
}

void Vehicle::Uninstall(bool dismount/* = false*/)
{
    /// @Prevent recursive uninstall call. (Bad script in OnUninstall/OnRemovePassenger/PassengerBoarded hook.)
    if (_status == STATUS_UNINSTALLING)
    {
        sLog->outDebug(LOG_FILTER_VEHICLES, "Vehicle GuidLow: %u, Entry: %u attempts to uninstall, but already has STATUS_UNINSTALLING! "
            "Check Uninstall/PassengerBoarded script hooks for errors.", _me->GetGUIDLow(), _me->GetEntry());
        return;
    }
    _status = STATUS_UNINSTALLING;
    sLog->outDebug(LOG_FILTER_VEHICLES, "Vehicle::Uninstall Entry: %u, GuidLow: %u", _creatureEntry, _me->GetGUIDLow());

    /// Needed in case the vehicle have just been teleported and didn't moved
    RelocatePassengers();

    RemoveAllPassengers(dismount);

    if (GetBase() && GetBase()->GetTypeId() == TYPEID_UNIT)
        sScriptMgr->OnUninstall(this);
}

void Vehicle::Reset(bool evading /*= false*/)
{
    sLog->outDebug(LOG_FILTER_VEHICLES, "Vehicle::Reset Entry: %u, GuidLow: %u", _creatureEntry, _me->GetGUIDLow());
    if (_me->IsPlayer())
    {
        if (_usableSeatNum)
            _me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
    }
    else
    {
        ApplyAllImmunities();
        InstallAllAccessories(evading);
        if (_usableSeatNum)
            _me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    if (GetBase()->GetTypeId() == TYPEID_UNIT)
        sScriptMgr->OnReset(this);
}

void Vehicle::ApplyAllImmunities()
{
    // This couldn't be done in DB, because some spells have MECHANIC_NONE

    // Vehicles should be immune on Knockback ...
    _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);

    // Mechanical units & vehicles ( which are not Bosses, they have own immunities in DB ) should be also immune on healing ( exceptions in switch below )
    if (_me->ToCreature() && _me->ToCreature()->GetCreatureTemplate()->type == CREATURE_TYPE_MECHANICAL && !_me->ToCreature()->isWorldBoss())
    {
        // Heal & dispel ...
        _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
        _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
        _me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_HEAL, true);

        // ... Shield & Immunity grant spells ...
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SCHOOL_IMMUNITY, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_UNATTACKABLE, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SCHOOL_ABSORB, true);
        _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SHIELD, true);
        _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_IMMUNE_SHIELD, true);
        _me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);

        // ... Resistance, Split damage, Change stats ...
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_DAMAGE_SHIELD, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_SPLIT_DAMAGE_PCT, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_RESISTANCE, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STAT, true);
        _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, true);
    }

    // Different immunities for vehicles goes below
    switch (GetVehicleInfo()->ID)
    {
        // code below prevents a bug with movable cannons
        case 160: // Strand of the Ancients
        case 244: // Wintergrasp
        case 321: // Pilgrim's Bounty chairs
        case 510: // Isle of Conquest
            _me->SetControlled(true, UNIT_STATE_ROOT);
            // why we need to apply this? we can simple add immunities to slow mechanic in DB
            _me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
            break;
        default:
            break;
    }
}

void Vehicle::RemoveAllPassengers(bool dismount/* = false*/)
{
    sLog->outDebug(LOG_FILTER_VEHICLES, "Vehicle::RemoveAllPassengers. Entry: %u, GuidLow: %u", _creatureEntry, _me->GetGUIDLow());

    // Passengers always cast an aura with SPELL_AURA_CONTROL_VEHICLE on the vehicle
    // We just remove the aura and the unapply handler will make the target leave the vehicle.
    // We don't need to iterate over Seats
    _me->RemoveAurasByType(SPELL_AURA_CONTROL_VEHICLE);

    if (dismount)
    {
        for (SeatMap::iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
        {
            if (itr->second.Passenger)
            {
                if (Unit* passenger = ObjectAccessor::GetUnit(*GetBase(), itr->second.Passenger))
                    passenger->ExitVehicle();
                else
                    itr->second.Passenger = 0;
            }
        }
    }
    // Following the above logic, this assertion should NEVER fail.
    // Even in 'hacky' cases, there should at least be VEHICLE_SPELL_RIDE_HARDCODED on us.
    // SeatMap::const_iterator itr;
    // for (itr = Seats.begin(); itr != Seats.end(); ++itr)
    //    ASSERT(!itr->second.passenger);
}

bool Vehicle::HasEmptySeat(int8 seatId) const
{
    SeatMap::const_iterator seat = Seats.find(seatId);
    if (seat == Seats.end())
        return false;
    return !seat->second.Passenger;
}

Unit* Vehicle::GetPassenger(int8 seatId) const
{
    SeatMap::const_iterator seat = Seats.find(seatId);
    if (seat == Seats.end())
        return NULL;

    return ObjectAccessor::GetUnit(*GetBase(), seat->second.Passenger);
}

int8 Vehicle::GetNextEmptySeat(int8 seatId, bool next) const
{
    SeatMap::const_iterator seat = Seats.find(seatId);
    if (seat == Seats.end())
        return -1;

    while (seat->second.Passenger || (!seat->second.SeatInfo->CanEnterOrExit() && !seat->second.SeatInfo->IsUsableByOverride()))
    {
        if (next)
        {
            ++seat;
            if (seat == Seats.end())
                seat = Seats.begin();
        }
        else
        {
            if (seat == Seats.begin())
                seat = Seats.end();
            --seat;
        }

        if (seat->first == seatId)
            return -1; // no available seat
    }

    return seat->first;
}

void Vehicle::InstallAccessory(VehicleAccessory const* accessory)
{
    /// @Prevent adding accessories when vehicle is uninstalling. (Bad script in OnUninstall/OnRemovePassenger/PassengerBoarded hook.)
    if (_status == STATUS_UNINSTALLING)
    {
        sLog->outDebug(LOG_FILTER_VEHICLES, "Vehicle GuidLow: %u, Entry: %u attempts to install accessory Entry: %u on seat %d with STATUS_UNINSTALLING! "
            "Check Uninstall/PassengerBoarded script hooks for errors.", _me->GetGUIDLow(), _me->GetEntry(), accessory->AccessoryEntry, (int32)accessory->SeatId);
        return;
    }

    sLog->outDebug(LOG_FILTER_VEHICLES, "Vehicle: Installing accessory entry %u on vehicle entry %u (seat:%i)", accessory->AccessoryEntry, GetCreatureEntry(), accessory->SeatId);

    if (Unit* passenger = GetPassenger(accessory->SeatId))
    {
        // already installed
        if (passenger->GetEntry() == accessory->AccessoryEntry)
        {
            ASSERT(passenger->GetTypeId() == TYPEID_UNIT);
            if (_me->GetTypeId() == TYPEID_UNIT)
            {
                if (_me->ToCreature()->IsInEvadeMode() && passenger->ToCreature()->IsAIEnabled)
                    passenger->ToCreature()->AI()->EnterEvadeMode();
                return;
            }
        }
        else
            passenger->ExitVehicle(); // this should not happen
    }

    VehicleAccessory l_Accessory = *accessory;

    if (_me->isSummon() && _me->MustBeVisibleOnlyForSomePlayers())
    {
        if (Unit* l_VehicleOwner = _me->GetAnyOwner())
        {
            if (_me->IsPlayerInPersonnalVisibilityList(l_VehicleOwner->GetObjectGuid()))
            {
                uint64 l_ViewerGuid = l_VehicleOwner->GetObjectGuid();
                Unit* l_Me = _me;
                uint32 l_VehicleID = GetVehicleInfo()->ID;
                _me->AddDelayedEvent([l_Me, l_Accessory, l_ViewerGuid, l_VehicleID]() -> void
                {
                    if (TempSummon* l_NewAccessory = l_Me->SummonCreature(l_Accessory.AccessoryEntry, *l_Me, TempSummonType(l_Accessory.SummonedType), l_Accessory.SummonTime, 0, l_ViewerGuid))
                    {
                        if (l_Accessory.IsMinion)
                            l_NewAccessory->AddUnitTypeMask(UNIT_MASK_ACCESSORY);

                        if (!l_Me->HandleSpellClick(l_NewAccessory, l_Accessory.SeatId))
                        {
                            l_NewAccessory->UnSummon();
                            return;
                        }

                        l_NewAccessory->m_movementInfo.t_pos = l_Accessory.Pos;
                        l_NewAccessory->m_movementInfo.VehicleRecID = l_VehicleID;

                        if (l_Me->GetVehicleKit() && l_Me->GetTypeId() == TYPEID_UNIT)
                            sScriptMgr->OnInstallAccessory(l_Me->GetVehicleKit(), l_NewAccessory);
                    }
                }, 1);
            }
        }
    }
    else
    {
        Unit* l_Me = _me;
        uint32 l_VehicleID = GetVehicleInfo()->ID;
        _me->AddDelayedEvent([l_Me, l_Accessory, l_VehicleID]() -> void
        {
            if (TempSummon* l_NewAccessory = l_Me->SummonCreature(l_Accessory.AccessoryEntry, *l_Me, TempSummonType(l_Accessory.SummonedType), l_Accessory.SummonTime))
            {
                if (l_Accessory.IsMinion)
                    l_NewAccessory->AddUnitTypeMask(UNIT_MASK_ACCESSORY);

                if (!l_Me->HandleSpellClick(l_NewAccessory, l_Accessory.SeatId))
                {
                    l_NewAccessory->UnSummon();
                    return;
                }

                l_NewAccessory->m_movementInfo.t_pos = l_Accessory.Pos;
                l_NewAccessory->m_movementInfo.VehicleRecID = l_VehicleID;

                if (l_Me->GetVehicleKit() && l_Me->GetTypeId() == TYPEID_UNIT)
                    sScriptMgr->OnInstallAccessory(l_Me->GetVehicleKit(), l_NewAccessory);
            }
        }, 1);
    }
}

bool Vehicle::CheckCustomCanEnter()
{
    switch (GetCreatureEntry())
    {
        case 46185:  ///< Sanitron
        case 56682:  ///< Keg in Stormstout Brewery
        case 100285: ///< Bat (Cordana Felsong)
        case 38928: ///< Sassy Hardwrench
        case 38869: ///< Sassy Hardwrench
            return true;
        default:
            break;
    }

    return false;
}

bool Vehicle::AddPassenger(Unit* unit, int8 seatId)
{
    /// @Prevent adding passengers when vehicle is uninstalling. (Bad script in OnUninstall/OnRemovePassenger/PassengerBoarded hook.)
    if (_status == STATUS_UNINSTALLING)
    {
        sLog->outDebug(LOG_FILTER_VEHICLES, "Passenger GuidLow: %u, Entry: %u, attempting to board vehicle GuidLow: %u, Entry: %u during uninstall! SeatId: %i",
            unit->GetGUIDLow(), unit->GetEntry(), _me->GetGUIDLow(), _me->GetEntry(), (int32)seatId);
        return false;
    }

    if (unit->GetVehicle() != this)
        return false;

    if (unit->IsPlayer() && unit->GetMap()->IsBattleArena())
        return false;

    SeatMap::iterator seat;
    if (seatId < 0) // no specific seat requirement
    {
        for (seat = Seats.begin(); seat != Seats.end(); ++seat)
            if (!seat->second.Passenger && (seat->second.SeatInfo->CanEnterOrExit() || seat->second.SeatInfo->IsUsableByOverride() || CheckCustomCanEnter()))
                break;

        if (seat == Seats.end()) // no available seat
            return false;
    }
    else
    {
        seat = Seats.find(seatId);
        if (seat == Seats.end())
            return false;

        if (seat->second.Passenger)
        {
            if (Unit* passenger = ObjectAccessor::GetUnit(*GetBase(), seat->second.Passenger))
                passenger->ExitVehicle();
            else
                seat->second.Passenger = 0;
        }

        ASSERT(!seat->second.Passenger);
    }

    if (!unit)
        return false;

    if (!_me)
        return false;

    if (!_vehicleInfo)
        return false;

    if (seat == Seats.end())
        return false;

    sLog->outDebug(LOG_FILTER_VEHICLES, "Unit %s enter vehicle entry %u id %u dbguid %u seat %d", unit->GetName(), _me->GetEntry(), _vehicleInfo->ID, _me->GetGUIDLow(), (int32)seat->first);

    seat->second.Passenger = unit->GetGUID();
    if (seat->second.SeatInfo->CanEnterOrExit())
    {
        ASSERT(_usableSeatNum);
        --_usableSeatNum;
        if (!_usableSeatNum)
        {
            if (_me->IsPlayer())
                _me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
            else
                _me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
    }

    if (seat->second.SeatInfo->m_flags && !(seat->second.SeatInfo->m_flags & VEHICLE_SEAT_FLAG_ALLOW_TURNING))
        unit->AddUnitState(UNIT_STATE_ONVEHICLE);


    bool l_NewTransportPosition = true;

    /// Happen when offets data are already set with vehicle_template_accessory
    if (unit->m_movementInfo.t_pos.m_positionX != 0.0f || unit->m_movementInfo.t_pos.m_positionY != 0.0f || unit->m_movementInfo.t_pos.m_positionZ != 0.0f || unit->m_movementInfo.t_pos.m_orientation != 0.0f)
        l_NewTransportPosition = false;

    VehicleSeatEntry const* veSeat = seat->second.SeatInfo;

    if (l_NewTransportPosition)
    {
        unit->m_movementInfo.t_pos.m_positionX = veSeat->AttachmentOffsetX;
        unit->m_movementInfo.t_pos.m_positionY = veSeat->AttachmentOffsetY;
        unit->m_movementInfo.t_pos.m_positionZ = veSeat->AttachmentOffsetZ;
        unit->m_movementInfo.t_pos.SetOrientation(0);
    }

    unit->m_movementInfo.t_time = 0; // 1 for player
    unit->m_movementInfo.t_seat = seat->first;
    unit->m_movementInfo.t_guid = _me->GetGUID();

    // Hackfix
    switch (veSeat->ID)
    {
        case 10882:
            unit->m_movementInfo.t_pos.m_positionX = 15.0f;
            unit->m_movementInfo.t_pos.m_positionY = 0.0f;
            unit->m_movementInfo.t_pos.m_positionZ = 30.0f;
            break;
        case 11227:
        case 11228:
            unit->m_movementInfo.t_pos.SetOrientation(M_PI / 2.0f);
            break;
        default:
            break;
    }

    if (_me->GetTypeId() == TYPEID_UNIT && unit->IsPlayer() &&
        seat->second.SeatInfo->m_flags & VEHICLE_SEAT_FLAG_CAN_CONTROL)
        ASSERT(_me->SetCharmedBy(unit, CHARM_TYPE_VEHICLE))

    if (_me->IsInWorld())
    {
        int l_SeatId = veSeat->ID;
        auto l_SendPacket = [unit, l_SeatId]() -> void
        {
            unit->SendClearTarget();                                 // SMSG_BREAK_TARGET

            Movement::MoveSplineInit init(unit);
            init.DisableTransportPathTransformations();
            init.MoveTo(unit->m_movementInfo.t_pos.m_positionX, unit->m_movementInfo.t_pos.m_positionY, unit->m_movementInfo.t_pos.m_positionZ, false, true);

            if (l_SeatId == 11227 || l_SeatId == 11228)
                init.SetFacing(M_PI / 2.0f);
            else
                init.SetFacing(0.0f);

            init.SetTransportEnter();
            init.Launch();

            unit->SetControlled(true, UNIT_STATE_ROOT);              // SMSG_FORCE_ROOT - In some cases we send SMSG_SPLINE_MOVE_ROOT here (for creatures)
        };

        /// Delay before sending those packets
        /// Done for Reins of the Grand Expedition Yak and etc.
        if (_me->IsPlayer() && unit->HasUnitTypeMask(UNIT_MASK_ACCESSORY))
        {
            unit->AddDelayedEvent(l_SendPacket, 500);
        }
        else
        {
            l_SendPacket();
        }

        if (_me->GetTypeId() == TYPEID_UNIT)
        {
            if (_me->ToCreature()->IsAIEnabled)
                _me->ToCreature()->AI()->PassengerBoarded(unit, seat->first, true);

            // update all passenger's positions
            //Passenger's spline OR vehicle movement will update positions
            //RelocatePassengers(_me->GetPositionX(), _me->GetPositionY(), _me->GetPositionZ(), _me->GetOrientation());
        }
    }

    if (GetBase()->GetTypeId() == TYPEID_UNIT)
        sScriptMgr->OnAddPassenger(this, unit, seatId);

    if (Creature* l_Passenger = unit->ToCreature())
    {
        if (l_Passenger->IsAIEnabled)
            l_Passenger->AI()->OnVehicleEntered(_me);
    }

    return true;
}

void Vehicle::RemovePassenger(Unit* unit)
{
    if (unit->GetVehicle() != this)
        return;

    SeatMap::iterator seat = GetSeatIteratorForPassenger(unit);
    ASSERT(seat != Seats.end());

    sLog->outDebug(LOG_FILTER_VEHICLES, "Unit %s exit vehicle entry %u id %u dbguid %u seat %d", unit->GetName(), _me->GetEntry(), _vehicleInfo->ID, _me->GetGUIDLow(), (int32)seat->first);

    seat->second.Passenger = 0;
    if (seat->second.SeatInfo->CanEnterOrExit())
    {
        if (!_usableSeatNum)
        {
            if (_me->IsPlayer())
                _me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_PLAYER_VEHICLE);
            else
                _me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
        ++_usableSeatNum;
    }

    unit->ClearUnitState(UNIT_STATE_ONVEHICLE);

    if (_me->GetTypeId() == TYPEID_UNIT && unit->IsPlayer() && seat->second.SeatInfo->m_flags & VEHICLE_SEAT_FLAG_CAN_CONTROL)
        _me->RemoveCharmedBy(unit);

    if (_me->IsInWorld())
    {
        if (_me->GetTransport())
        {
            unit->m_movementInfo.t_pos.Relocate(_me->m_movementInfo.t_pos);
            unit->m_movementInfo.t_time = _me->m_movementInfo.t_time;
            unit->m_movementInfo.t_seat = _me->m_movementInfo.t_seat;
            unit->m_movementInfo.t_guid = _me->m_movementInfo.t_guid;
        }
        else
        {
            unit->m_movementInfo.t_pos.Relocate(0, 0, 0, 0);
            unit->m_movementInfo.t_time = 0;
            unit->m_movementInfo.t_seat = 0;
            unit->m_movementInfo.t_guid = 0;
        }
    }

    // only for flyable vehicles
    if (unit->IsFlying())
        _me->DelayedCastSpell(unit, VEHICLE_SPELL_PARACHUTE, true, 1);

    if (_me->GetTypeId() == TYPEID_UNIT && _me->ToCreature()->IsAIEnabled)
        _me->ToCreature()->AI()->PassengerBoarded(unit, seat->first, false);

    if (Creature* l_Passenger = unit->ToCreature())
    {
        if (l_Passenger->IsAIEnabled)
            l_Passenger->AI()->OnVehicleExited(_me);
    }

    if (GetBase()->GetTypeId() == TYPEID_UNIT)
        sScriptMgr->OnRemovePassenger(this, unit);
}

//! Must be called after m_base::Relocate
void Vehicle::RelocatePassengers()
{
    ASSERT(_me->GetMap());

    // not sure that absolute position calculation is correct, it must depend on vehicle pitch angle
    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
    {
        if (Unit* passenger = ObjectAccessor::GetUnit(*GetBase(), itr->second.Passenger))
        {
            ASSERT(passenger->IsInWorld());

            float px, py, pz, po;
            passenger->m_movementInfo.t_pos.GetPosition(px, py, pz, po);
            CalculatePassengerPosition(px, py, pz, po);
            passenger->UpdatePosition(px, py, pz, po);
        }
    }
}

void Vehicle::Dismiss()
{
    if (GetBase()->GetTypeId() != TYPEID_UNIT)
        return;

    sLog->outDebug(LOG_FILTER_VEHICLES, "Vehicle::Dismiss Entry: %u, GuidLow %u", _creatureEntry, _me->GetGUIDLow());
    Uninstall();
    GetBase()->ToCreature()->DespawnOrUnsummon();
}

bool Vehicle::IsVehicleInUse() const
{
    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
        if (itr->second.Passenger)
            return true;

    return false;
}

void Vehicle::InitMovementInfoForBase()
{
    uint32 vehicleFlags = GetVehicleInfo()->Flags;

    if (vehicleFlags & VEHICLE_FLAG_NO_STRAFE)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_NO_STRAFE);

    if (vehicleFlags & VEHICLE_FLAG_NO_JUMPING)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_NO_JUMPING);

    if (vehicleFlags & VEHICLE_FLAG_FULLSPEEDTURNING)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_FULL_SPEED_TURNING);

    if (vehicleFlags & VEHICLE_FLAG_ALLOW_PITCHING)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING);

    if (vehicleFlags & VEHICLE_FLAG_FULLSPEEDPITCHING)
        _me->AddExtraUnitMovementFlag(MOVEMENTFLAG2_FULL_SPEED_PITCHING);

    if (vehicleFlags & VEHICLE_FLAG_FIXED_POSITION)
    {
        _me->RemoveUnitMovementFlag(MOVEMENTFLAG_MASK_MOVING);
        _me->AddUnitMovementFlag(MOVEMENTFLAG_ROOT);
    }
}

VehicleSeatEntry const* Vehicle::GetSeatForPassenger(Unit const* passenger)
{
    if (!passenger)
        return NULL;

    SeatMap::iterator itr;
    for (itr = Seats.begin(); itr != Seats.end(); ++itr)
        if (itr->second.Passenger == passenger->GetGUID())
            return itr->second.SeatInfo;

    return NULL;
}

SeatMap::iterator Vehicle::GetSeatIteratorForPassenger(Unit* passenger)
{
    SeatMap::iterator itr;
    for (itr = Seats.begin(); itr != Seats.end(); ++itr)
        if (itr->second.Passenger == passenger->GetGUID())
            return itr;

    return Seats.end();
}

uint8 Vehicle::GetAvailableSeatCount() const
{
    uint8 ret = 0;
    SeatMap::const_iterator itr;
    for (itr = Seats.begin(); itr != Seats.end(); ++itr)
        if (!itr->second.Passenger && (itr->second.SeatInfo->CanEnterOrExit() || itr->second.SeatInfo->IsUsableByOverride()))
            ++ret;

    return ret;
}

void Vehicle::CalculatePassengerPosition(float& p_X, float& p_Y, float& p_Z, float& p_O)
{
    float l_InX     = p_X;
    float l_InY     = p_Y;
    float l_InZ     = p_Z;
    float l_InO     = p_O;

    Unit* l_Base    = GetBase();
    Position l_Pos  = *l_Base;

    if (Vehicle* l_ParentTransport = l_Base->GetVehicle())
    {
        l_Pos = l_Base->m_movementInfo.t_pos;
        l_ParentTransport->CalculatePassengerPosition(l_Pos.m_positionX, l_Pos.m_positionY, l_Pos.m_positionZ, l_Pos.m_orientation);
    }

    p_X = l_Pos.m_positionX + l_InX * std::cos(l_Pos.m_orientation) - l_InY * std::sin(l_Pos.m_orientation);
    p_Y = l_Pos.m_positionY + l_InY * std::cos(l_Pos.m_orientation) + l_InX * std::sin(l_Pos.m_orientation);
    p_Z = l_Pos.m_positionZ + l_InZ;
    p_O = l_Pos.m_orientation + l_InO;
}

void Vehicle::CalculatePassengerOffset(float& p_X, float& p_Y, float& p_Z, float& p_O)
{
    Unit* l_Base = GetBase();

    p_O -= l_Base->GetOrientation();
    p_Z -= l_Base->GetPositionZ();
    p_Y -= l_Base->GetPositionY();
    p_X -= l_Base->GetPositionX();

    float l_InX = p_X;
    float l_InY = p_Y;

    p_Y = (l_InY - l_InX * tan(l_Base->m_orientation)) / (cos(l_Base->m_orientation) + std::sin(l_Base->m_orientation) * tan(l_Base->m_orientation));
    p_X = (l_InX + l_InY * tan(l_Base->m_orientation)) / (cos(l_Base->m_orientation) + std::sin(l_Base->m_orientation) * tan(l_Base->m_orientation));
}
