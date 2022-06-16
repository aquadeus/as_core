////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GameObjectAI.h"

//GameObjectAI::GameObjectAI(GameObject* g) : go(g) {}
int GameObjectAI::Permissible(const GameObject* go)
{
    if (go->GetAIName() == "GameObjectAI")
        return PERMIT_BASE_SPECIAL;
    return PERMIT_BASE_NO;
}

void GameObjectAI::UpdateOperations(uint32 const p_Diff)
{
    std::vector<std::function<void()>> l_OperationsReady;

    for (auto l_Iter = m_TimedDelayedOperations.begin(); l_Iter != m_TimedDelayedOperations.end();)
    {
        auto& l_Pair = *l_Iter;

        l_Pair.first -= p_Diff;

        if (l_Pair.first < 0)
        {
            l_OperationsReady.push_back(l_Pair.second);

            l_Iter = m_TimedDelayedOperations.erase(l_Iter);
        }
        else
            ++l_Iter;
    }

    for (auto l_Operation : l_OperationsReady)
        l_Operation();

    if (m_TimedDelayedOperations.empty() && !m_EmptyWarned)
    {
        m_EmptyWarned = true;
        LastOperationCalled();
    }
}

NullGameObjectAI::NullGameObjectAI(GameObject* g) : GameObjectAI(g) {}
