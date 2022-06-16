////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SCRIPTED_COMESTIC_AI_HPP_SCRIPTED_AI
# define SCRIPTED_COMESTIC_AI_HPP_SCRIPTED_AI

#include "Creature.h"
#include "CreatureAI.h"
#include "Common.h"

namespace MS { namespace AI
{

    struct CosmeticAI : public CreatureAI
    {
        public:
            /// Constructor
            explicit CosmeticAI(Creature * p_Creature) : CreatureAI(p_Creature)
            {
            }

            /// Destructor
            virtual ~CosmeticAI()
            {

            }

            /// Set AI obstacle manager enabled
            /// @p_Enabled : Is enabled
            void SetAIObstacleManagerEnabled(bool p_Enabled)
            {
                if (p_Enabled)
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_AI_OBSTACLE);
                else
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS + 1, UNIT_NPC_FLAG2_AI_OBSTACLE);
            }

        public:
            /// On AI Update
            /// @p_Diff : Time since last update
            virtual void UpdateAI(const uint32 p_Diff) override
            {
                while (!m_DelayedOperations.empty())
                {
                    m_DelayedOperations.front()();
                    m_DelayedOperations.pop();
                }
            }

            /// Called at waypoint reached or point movement finished
            /// @p_Type : Movement Type
            /// @p_ID   : Misc ID
            virtual void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type == POINT_MOTION_TYPE)
                {
                    if (m_OnPointReached.find(p_ID) != m_OnPointReached.end())
                    {
                        m_DelayedOperations.push([this, p_ID]() -> void
                        {
                            if (m_OnPointReached.find(p_ID) != m_OnPointReached.end())
                                m_OnPointReached[p_ID]();
                        });
                    }
                }
            }

        public:
            std::map<uint32, std::function<void()>>                 m_OnPointReached;           ///< Delayed operations
            std::queue<std::function<void()>>                       m_DelayedOperations;        ///< Delayed operations
    };

}   ///< namespace AI
}   ///< namespace MS

#endif  ///< SCRIPTED_COMESTIC_AI_HPP_SCRIPTED_AI
