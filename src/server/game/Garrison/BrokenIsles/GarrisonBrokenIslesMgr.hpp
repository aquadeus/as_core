////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once


#include "../Interfaces/Interface_GarrisonSite.hpp"
#include "../BrokenIsles/GarrisonBrokenIslesConstants.hpp"
#include "../GarrisonMgrStructures.hpp"
#include "../GarrisonMgrConstants.hpp"
#include "../GarrisonFollower.hpp"
#include "../GarrisonMission.hpp"
#include "../GarrisonWorkOrder.hpp"
#include "../GarrisonShipmentManager.hpp"

namespace MS { namespace Garrison { 

    class Manager;

 namespace GBrokenIsles
{
    /// Garrison manager class
    class GarrisonBrokenIsles
    {
        public:
            /// Constructor
            GarrisonBrokenIsles(Manager* p_Manager, Player* p_Owner);

            /// Create the garrison
            void Create();
            /// Load
            bool Load(PreparedQueryResult p_BrokenIslesGarrisonResult);
            /// Save this garrison to DB
            void Save(SQLTransaction p_Transaction);

            /// Delete garrison
            static void DeleteFromDB(uint64 p_PlayerGUID, SQLTransaction p_Transation);

            /// Update the garrison
            void Update();

            /// When the garrison owner enter in the garrisson (@See Player::UpdateArea)
            void OnPlayerEnter();
            /// When the garrison owner leave the garrisson (@See Player::UpdateArea)
            void OnPlayerLeave();

            /// Get GarrSiteLevelEntry for current garrison
            const GarrSiteLevelEntry* GetGarrisonSiteLevelEntry() const;

        public:
            /// Get support follower database id
            uint32 GetSupportFollowerDatabaseID();

        public:
            /// Replace garrison script
            void _SetGarrisonScriptID(uint32 p_ScriptID)
            {
                m_GarrisonScriptID = p_ScriptID;
            }

        private:
            /// Init
            void Init();

        private:
            Manager*    m_Manager;                      ///< Garrison manager
            Player*     m_Owner;                        ///< Garrison owner
            uint32      m_ID;                           ///< Garrison DB ID
            uint32      m_GarrisonLevelID;              ///< Garrison level ID
            uint32      m_GarrisonSiteID;               ///< Garrison site ID
            uint32      m_SupportFollowerDatabaseID;    ///< Designated support follower database id

            uint32 m_GarrisonScriptID;

    };

}   ///< namespace GBrokenIsles
}   ///< namespace Garrison
}   ///< namespace MS


