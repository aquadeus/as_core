////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GarrisonTalent.hpp"
#include "GarrisonMgrConstants.hpp"
#include "DB2Stores.h"
#include "GarrisonMgr.hpp"
#include "Player.h"
#include "Object.h"

namespace MS { namespace Garrison
{
    /// Constructor
    GarrisonTalent::GarrisonTalent()
    {
        TalentID            = 0;
        StartResearchTime   = 0;
        Flags               = 0;
        GarrisonType        = 0;
    }

}   ///< namespace Garrison
}   ///< namespace MS

