////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GarrisonMgr.hpp"
#include "DB2Stores.h"

namespace MS { namespace Garrison
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    bool Manager::CanAddTalent(GarrTalentEntry const* p_Entry, bool p_Replace /* = false */) const
    {
        auto l_Tree = GetGarrisonTalentsByClassAtRank(GarrisonType::GarrisonBrokenIsles, m_Owner->getClass(), p_Entry->Rank);
        if (!l_Tree)
            return false;

        if (IsResearchingTalent())
            return false;

        if (!p_Replace)
        {
            for (auto l_Talent : *l_Tree)
            {
                if (HasTalent(l_Talent->ID))
                    return false;
            }
        }

        if (!p_Replace && GetLastResearchedRankOfTalentTree(p_Entry->TalentTreeID) + 1 != p_Entry->Rank)
            return false;

        if (std::find_if(l_Tree->begin(), l_Tree->end(), [p_Entry](GarrTalentEntry const* p_Talent) -> bool { return p_Talent->ID == p_Entry->ID; }) == l_Tree->end())
            return false;

        uint32 l_CurrencyID = p_Replace ? p_Entry->ReplaceCurrencyID : p_Entry->ResearchCurrencyID;
        uint32 l_CurrencyCount = p_Replace ? p_Entry->ReplaceCurrencyCost : p_Entry->ResearchCurrencyCost;
        uint64 l_Gold = p_Replace ? p_Entry->ReplaceGoldCost : p_Entry->ResearchGoldCost;

        if (l_CurrencyID && !m_Owner->HasCurrency(l_CurrencyID, l_CurrencyCount))
            return false;

        if (l_Gold && !m_Owner->HasEnoughMoney(l_Gold))
            return false;

        return true;
    }

    bool Manager::HasATalentInRow(GarrTalentEntry const* p_Entry) const
    {
        auto l_Tree = GetGarrisonTalentsByClassAtRank(GarrisonType::GarrisonBrokenIsles, m_Owner->getClass(), p_Entry->Rank);
        if (!l_Tree)
            return false;

        for (auto l_Talent : *l_Tree)
        {
            if (HasTalent(l_Talent->ID))
                return true;
        }

        return false;
    }

    bool Manager::AddTalent(GarrTalentEntry const* p_Entry, bool p_TakeRequirements /* = true */, bool p_Replace /* = false */)
    {
        if (HasTalent(p_Entry->ID))
            return false;

        auto l_Tree = sGarrTalentTreeStore.LookupEntry(p_Entry->TalentTreeID);
        if (!l_Tree)
            return false;

        if (p_Replace)
        {
            auto l_RankTree = GetGarrisonTalentsByClassAtRank(GarrisonType::GarrisonBrokenIsles, m_Owner->getClass(), p_Entry->Rank);

            for (auto const& l_Entry : *l_RankTree)
            {
                if (HasTalent(l_Entry->ID))
                    RemoveTalent(l_Entry);
            }
        }

        if (p_TakeRequirements)
        {
            uint32 l_CurrencyID = p_Replace ? p_Entry->ReplaceCurrencyID : p_Entry->ResearchCurrencyID;
            int32 l_CurrencyCount = p_Replace ? p_Entry->ReplaceCurrencyCost : p_Entry->ResearchCurrencyCost;
            int64 l_Gold = p_Replace ? p_Entry->ReplaceGoldCost : p_Entry->ResearchGoldCost;

            if (l_CurrencyID)
                m_Owner->ModifyCurrency(l_CurrencyID, -l_CurrencyCount);

            if (l_Gold)
                m_Owner->ModifyMoney(-l_Gold, "MS::Garrison::Manager::AddTalent");
        }

        // Complete quests, requires class hall upgrade
        std::map<uint32, uint32> m_quest_criteria
        {  // quest  criteria
            { 42191, 49435 },
            { 41740, 50160 },
            { 42526, 50163 },
            { 42588, 50175 },
            { 42601, 50169 },
            { 42611, 50178 },
            { 42683, 50184 },
            { 42696, 50181 },
            { 42850, 50166 },
            { 43015, 50187 },
            { 43268, 50190 },
            { 43277, 50172 },
        };

        for (auto l_quest_criteria : m_quest_criteria)
            if (m_Owner->HasQuest(l_quest_criteria.first))
                m_Owner->QuestObjectiveSatisfy(l_quest_criteria.second, 1, QUEST_OBJECTIVE_TYPE_CRITERIA_TREE, m_Owner->GetGUID());


        GarrisonTalent l_Talent;
        l_Talent.TalentID = p_Entry->ID;
        l_Talent.StartResearchTime = time(nullptr);
        l_Talent.Flags = 0;
        l_Talent.GarrisonType = l_Tree->GarrisonType;
        m_Talents.push_back(l_Talent);

        int l_Index = 0;
        PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON_TALENT);
        l_Statement->setUInt32(l_Index++, m_ID);
        l_Statement->setUInt32(l_Index++, l_Talent.TalentID);
        l_Statement->setUInt32(l_Index++, l_Talent.StartResearchTime);
        l_Statement->setUInt32(l_Index++, l_Talent.Flags);
        CharacterDatabase.Execute(l_Statement);
        SendTalentUpdate(l_Talent);

        return true;
    }

    bool Manager::RemoveTalent(GarrTalentEntry const* p_Entry)
    {
        auto l_Iter = std::find_if(m_Talents.begin(), m_Talents.end(), [p_Entry](GarrisonTalent const& l_Talent) -> bool { return l_Talent.TalentID == p_Entry->ID; });

        if (l_Iter == m_Talents.end())
            return false;

        int l_Index = 0;
        PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_TALENT);
        l_Statement->setUInt32(l_Index++, m_ID);
        l_Statement->setUInt32(l_Index++, p_Entry->ID);
        CharacterDatabase.Execute(l_Statement);
        SendTalentUpdate(*l_Iter, true);

        ApplyTalentEffect(p_Entry, false,  true);
        m_Talents.erase(l_Iter);

        return true;
    }

    bool Manager::HasTalent(uint32 p_ID) const
    {
        return std::find_if(m_Talents.begin(), m_Talents.end(), [p_ID](GarrisonTalent const& l_Talent) -> bool { return l_Talent.TalentID == p_ID; }) != m_Talents.end();
    }

    bool Manager::IsResearchingTalent() const
    {
        for (auto const& l_Talent : m_Talents)
        {
            if (l_Talent.IsBeingResearched())
                return true;
        }

        return false;
    }

    int8 Manager::GetLastResearchedRankOfTalentTree(uint32 p_TalentTreeID) const
    {
        int8 l_Rank = -1;

        for (auto const& l_Talent : m_Talents)
        {
            auto l_Entry = sGarrTalentStore.LookupEntry(l_Talent.TalentID);

            if (l_Entry->TalentTreeID == p_TalentTreeID)
                l_Rank = std::max(static_cast<int8>(l_Entry->Rank), l_Rank);
        }

        return l_Rank;
    }

    std::vector<GarrisonTalent const*> Manager::GetGarrisonTalents(GarrisonType::Type p_Type) const
    {
        std::vector<GarrisonTalent const*> l_Talents;

        for (auto const& l_Talent : m_Talents)
        {
            if (l_Talent.GarrisonType == p_Type)
                l_Talents.push_back(&l_Talent);
        }

        return l_Talents;
    }

    void Manager::SendTalentUpdate(GarrisonTalent const& l_Talent, bool p_Remove /* = false */)
    {
        WorldPacket l_Data(SMSG_GARRISON_UPDATE_TALENT, 5 * 4);
        l_Data << uint32(p_Remove);
        l_Data << uint32(l_Talent.GarrisonType);
        l_Data << uint32(l_Talent.TalentID);
        l_Data << uint32(l_Talent.StartResearchTime);
        l_Data << uint32(l_Talent.Flags);
        m_Owner->SendDirectMessage(&l_Data);
    }

    void Manager::ApplyTalentEffect(GarrTalentEntry const* p_Entry, bool p_Apply, bool p_SendVisibilityUpdated)
    {
        uint32 l_SpellID = p_Entry->SpellID;
        bool l_UpdateRequired = false;

        if (p_Apply)
        {
            m_Owner->addSpell(p_Entry->SpellID, true, false, false, false);
            l_UpdateRequired = m_Owner->SetInPhase(WORKORDER_PHASE_ID(p_Entry->ID), false, true);
            AddPlayerGarrAbility(p_Entry->GarrAbilityID);
        }
        else
        {
            m_Owner->removeSpell(p_Entry->SpellID, false, false);
            l_UpdateRequired = m_Owner->SetInPhase(WORKORDER_PHASE_ID(p_Entry->ID), false, false);
            RemovePlayerGarrAbility(p_Entry->GarrAbilityID);
        }

        if (l_UpdateRequired && p_SendVisibilityUpdated)
            m_Owner->UpdateObjectVisibility(false);
    }

    bool Manager::HasResearchedTalent(uint32 p_ID) const
    {
        for (auto const& l_Talent : m_Talents)
        {
            if (l_Talent.TalentID == p_ID)
                return !l_Talent.IsBeingResearched();
        }
        return false;
    }

    /// Update talents
    void Manager::UpdateTalents()
    {
        for (auto& l_Talent : m_Talents)
        {
            if (l_Talent.IsBeingResearched())
            {
                if (l_Talent.GetEntry()->ResearchTime + l_Talent.StartResearchTime < time(nullptr))
                {
                    l_Talent.Flags |= GarrisonTalentFlags::Researched;
                    SendTalentUpdate(l_Talent);
                    ApplyTalentEffect(l_Talent.GetEntry(), true, true);
                }
            }
        }
    }

}   ///< namespace Garrison
}   ///< namespace MS
