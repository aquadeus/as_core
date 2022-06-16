////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SCRIPTING_INTERFACES_CONTRIBUTION_HPP_INCLUDED
#define SCRIPTING_INTERFACES_CONTRIBUTION_HPP_INCLUDED

#include "InterfaceBase.hpp"

/// POI Script Interface
class ContributionScript : public ScriptObjectImpl<false>
{
    protected:
        /// Constructor
        /// @p_Name : Script Name
        ContributionScript(const char* p_Name);

    public:

        /// Called when a Contribution change of state
        /// @p_Contribution : Contribution that has changed of state
        virtual void OnChangeState(Contribution const* p_Contribution)
        {
            UNUSED(p_Contribution);
        }

        /// Called when a player logs in.
        /// @p_Player : Player instance
        virtual void OnPlayerLogin(Player* p_Player)
        {
            UNUSED(p_Player);
        }

        /// Called when a player enters the map.
        /// @p_Map    : Context map
        /// @p_Player : Entered player instance
        virtual void OnPlayerEnterMap(Player* p_Player, Map* p_Map)
        {
            UNUSED(p_Map);
            UNUSED(p_Player);
        }

        /// Called just after any worldobject is created
        /// @p_WorldObject  : WorldObject that have just been created
        virtual void OnWorldObjectCreate(WorldObject const* p_WorldObject)
        {
            UNUSED(p_WorldObject);
        }

        /// Called when ContriutionMgr Updates
        virtual void Update(uint32 const p_GameTime)
        {
            UNUSED(p_GameTime);
        }

        /// Called when command .reload quest_template is used
        virtual void OnReloadQuestTemplates()
        {

        }

        /// Called when a player contributes
        /// @p_Player           : Contributing Player
        /// @p_p_Contribution   : Concerned contribution
        virtual void OnPlayerContribute(Player* p_Player, Contribution const* p_Contribution)
        {
            UNUSED(p_Player);
            UNUSED(p_Contribution);
        }
};

#endif  ///< SCRIPTING_INTERFACES_CONTRIBUTION_HPP_INCLUDED
