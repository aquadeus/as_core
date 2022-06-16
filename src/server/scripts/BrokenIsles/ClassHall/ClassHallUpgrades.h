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

    class npc_class_hall_upgrades : public CreatureScript
    {
        public:
            /// Constructor
            npc_class_hall_upgrades();

            /// Called when a player opens a gossip dialog with the creature.
            /// @p_Player   : Source player instance
            /// @p_Creature : Target creature instance
            bool OnGossipHello(Player* p_Player, Creature* p_Creature) override;
            bool OnGossipSelect(Player* p_Player, Creature* p_Creature, uint32 /*sender*/, uint32 p_Action) override;
        private:
            struct npc_class_hall_upgradesAI : public GarrisonNPCAI
            {
                npc_class_hall_upgradesAI(Creature* p_Creature);
            };

            uint32 m_TroopsInTheFieldQuestIDs[12] = { 41741, 42210, 42525, 42586, 42602, 42610, 42681, 42687, 42849, 43014, 43267, 43276 };

        public:
            CreatureAI* GetAI(Creature* p_Creature) const override
            {
                return new npc_class_hall_upgradesAI(p_Creature);
            }

    };
}   ///< namespace GBrokenIsles
}   ///< namespace Garrison
}   ///< namespace MS