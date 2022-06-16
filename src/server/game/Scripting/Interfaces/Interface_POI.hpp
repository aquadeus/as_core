////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SCRIPTING_INTERFACES_POI_HPP_INCLUDED
#define SCRIPTING_INTERFACES_POI_HPP_INCLUDED

#include "InterfaceBase.hpp"

/// POI Script Interface
class POIScript : public ScriptObjectImpl<false>
{
    protected:
        /// Constructor
        /// @p_Name : Script Name
        POIScript(const char* p_Name);

    public:
        /// Called just after any map is created
        /// @p_Map        : Map that have just been created
        /// @p_ZoneID     : Zone ID for which the map has been created
        virtual void OnCreateBaseMap(Map* p_Map, uint32 p_ZoneID)
        {
            UNUSED(p_Map);
            UNUSED(p_ZoneID);
        }

        /// Called just after any worldobject is created
        /// @p_WorldObject  : WorldObject that have just been created
        virtual void OnWorldObjectCreate(WorldObject const* p_WorldObject)
        {
            UNUSED(p_WorldObject);
        }

        /// Called at the end of Player::UpdateWorldQuests()
        /// @p_Player       : Concerned player
        /// @p_OldAreaID    : Area ID of the player at the last update
        /// @p_NewAreaID    : Area ID of the player now
        virtual void OnUpdateWorldQuests(Player* p_Player, uint32 p_OldAreaID, uint32 p_NewAreaID)
        {
            UNUSED(p_Player);
            UNUSED(p_OldAreaID);
            UNUSED(p_NewAreaID);
        }

        /// Called when a player logs in.
        /// @p_Player : Player instance
        virtual void OnPlayerLogin(Player* p_Player)
        {
            UNUSED(p_Player);
        }

        /// Called when a player switches to a new zone
        /// @p_Player    : Player instance
        /// @p_NewZoneID : New player zone ID
        /// @p_OldZoneID : Old player zone ID
        virtual void OnPlayerUpdateZone(Player* p_Player, uint32 p_NewZone, uint32 p_OldZone)
        {
            UNUSED(p_Player);
            UNUSED(p_OldZone);
            UNUSED(p_NewZone);
        }

        /// Called when a POI appears for everyone
        /// @p_AreaPOI      : POI that just appeared
        virtual void OnPOIAppears(AreaPOI const& p_AreaPOI)
        {
            UNUSED(p_AreaPOI);
        }

        /// Called when a POI disappears for everyone
        /// @p_AreaPOI      : POI that just appeared
        virtual void OnPOIDisappears(AreaPOI const& p_AreaPOI)
        {
            UNUSED(p_AreaPOI);
        }

        virtual void OnNextPOISelect(uint32 p_GroupId, std::set<uint32>& p_AvailableNextPOIs)
        {
            UNUSED(p_GroupId);
            UNUSED(p_AvailableNextPOIs);
        }

        /// Called when player rewards some quest
        /// @p_Player : Player instance
        /// @p_Quest  : Rewarded quest
        virtual void OnQuestReward(Player* p_Player, Quest const* p_Quest)
        {
            UNUSED(p_Player);
            UNUSED(p_Quest);
        }

        /// Called when player accepts some quest
        /// @p_Player : Player instance
        /// @p_Quest  : Accpeted quest
        virtual void OnQuestAccept(Player* p_Player, Quest const* p_Quest)
        {
            UNUSED(p_Player);
            UNUSED(p_Quest);
        }
};

#endif  ///< SCRIPTING_INTERFACES_POI_HPP_INCLUDED
