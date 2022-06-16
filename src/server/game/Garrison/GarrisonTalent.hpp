////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"
#include "ByteBuffer.h"

namespace MS { namespace Garrison
{

    namespace GarrisonTalentFlags
    {
        enum : uint32
        {
            Researched  = 1
        };
    };

    /// Garrison talent
    class GarrisonTalent
    {
        public:
            /// Constructor
            GarrisonTalent();

        public:
            uint32 TalentID;            ///< Talent ID
            uint32 StartResearchTime;   ///< Start research time
            uint32 Flags;               ///< Flags
            uint32 GarrisonType;        ///< GarrisonType

            bool IsBeingResearched() const
            {
                return !(Flags & GarrisonTalentFlags::Researched);
            }

            GarrTalentEntry const* GetEntry() const
            {
                return sGarrTalentStore.LookupEntry(TalentID);
            }

    };

}   ///< namespace Garrison
}   ///< namespace MS


