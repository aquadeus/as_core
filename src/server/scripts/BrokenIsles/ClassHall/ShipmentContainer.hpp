////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

# include "BrokenIslesPCH.h"

namespace MS { namespace Garrison { namespace GBrokenIsles {

    /// Garrison shipment container generic script
    class go_classhall_shipment_container : public GameObjectScript
    {
        public:
            /// Constructor
            go_classhall_shipment_container();

            /// Called when a GameObjectAI object is needed for the GameObject.
            /// @p_GameObject : GameObject instance
            GameObjectAI* GetAI(GameObject * p_GameObject) const override;

            struct ShipmentCurrency
            {
                uint32 CurrencyID     = 0;
                uint32 CurrencyAmount = 0;
            };

            struct ItemReward
            {
                uint32 ItemID;
                uint32 ItemCount;

                ItemReward()
                {
                    ItemID = 0;
                    ItemCount = 0;
                }
            };

            struct go_classhall_shipment_containerAI : public GameObjectAI
            {
                /// Constructor
                go_classhall_shipment_containerAI(GameObject * p_GameObject);

                /// Called when a player opens a gossip dialog with the GameObject.
                /// @p_Player     : Source player instance
                bool GossipHello(Player * p_Player) override;
            };

    };

}   ///< namespace GBrokenIsles
}   ///< namespace Garrison
}   ///< namespace MS
