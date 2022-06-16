////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GarrisonMgr.hpp"
#include "GarrisonBrokenIslesMgr.hpp"
#include "Player.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "DisableMgr.h"

namespace MS { namespace Garrison { namespace GBrokenIsles
{
    /// Constructor
    GarrisonBrokenIsles::GarrisonBrokenIsles(Manager* p_Manager, Player* p_Owner)
        : m_Manager(p_Manager), m_Owner(p_Owner), m_GarrisonScriptID(0), m_SupportFollowerDatabaseID(0)
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Create the garrison
    void GarrisonBrokenIsles::Create()
    {
        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GARRISON_BROKEN_ISLES);

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, m_Manager->GetGarrisonID());
        l_Stmt->setUInt32(l_Index++, m_SupportFollowerDatabaseID);

        CharacterDatabase.AsyncQuery(l_Stmt);

        Init();
    }
    /// Load
    bool GarrisonBrokenIsles::Load(PreparedQueryResult p_BrokenIslesGarrisonResult)
    {
        if (p_BrokenIslesGarrisonResult)
        {
            Field* l_Fields = p_BrokenIslesGarrisonResult->Fetch();

            m_SupportFollowerDatabaseID = l_Fields[0].GetUInt32();

            Init();

            if (!GetGarrisonSiteLevelEntry())
                return false;

            return true;
        }

        return false;
    }
    /// Save this garrison to DB
    void GarrisonBrokenIsles::Save(SQLTransaction p_Transaction)
    {
        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GARRISON_BROKEN_ISLES);

        uint32 l_Index = 0;
        l_Stmt->setUInt32(l_Index++, m_SupportFollowerDatabaseID);
        l_Stmt->setUInt32(l_Index++, m_Manager->GetGarrisonID());

        p_Transaction->Append(l_Stmt);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Delete garrison
    void GarrisonBrokenIsles::DeleteFromDB(uint64 p_PlayerGUID, SQLTransaction p_Transation)
    {
        PreparedStatement* l_Stmt;

        l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GARRISON_BROKEN_ISLES);
        l_Stmt->setUInt32(0, p_PlayerGUID);
        p_Transation->Append(l_Stmt);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update the garrison
    void GarrisonBrokenIsles::Update()
    {
        bool l_HasCommandBarAura    = m_Owner->HasAura(Globals::CommandBarSpell);
        bool l_IsInClassHall        = m_Owner->IsInAreaGroup(Globals::ClassHallAreaGroup);

        /// Update command bar, Class hall special UI
        if (!l_HasCommandBarAura && l_IsInClassHall)
            m_Owner->AddAura(Globals::CommandBarSpell, m_Owner);
        else if (l_HasCommandBarAura && !l_IsInClassHall)
            m_Owner->RemoveAura(Globals::CommandBarSpell);

        /// Update support follower
        if (m_SupportFollowerDatabaseID)
        {
            GarrisonFollower* l_SupportFollower = m_Manager->GetFollowerWithDatabaseID(m_SupportFollowerDatabaseID);

            if (l_SupportFollower && l_SupportFollower->ZoneSupportSpellID)
            {
                if (m_Owner->GetMapId() == Globals::FollowerSupportMapID)
                    m_Owner->AddAura(l_SupportFollower->ZoneSupportSpellID, m_Owner);
                else
                    m_Owner->RemoveAura(l_SupportFollower->ZoneSupportSpellID);
            }
            else
                l_SupportFollower = 0;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// When the garrison owner enter in the garrisson (@See Player::UpdateArea)
    void GarrisonBrokenIsles::OnPlayerEnter()
    {

    }
    /// When the garrison owner leave the garrisson (@See Player::UpdateArea)
    void GarrisonBrokenIsles::OnPlayerLeave()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get GarrSiteLevelEntry for current garrison
    const GarrSiteLevelEntry* GarrisonBrokenIsles::GetGarrisonSiteLevelEntry() const
    {
        return sGarrSiteLevelStore.LookupEntry(m_GarrisonLevelID);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Get support follower database id
    uint32 GarrisonBrokenIsles::GetSupportFollowerDatabaseID()
    {
        return m_SupportFollowerDatabaseID;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Init
    void GarrisonBrokenIsles::Init()
    {
        const GarrSiteLevelEntry* l_SiteEntry = sGarrSiteLevelStore.LookupEntry(Globals::DefaultSiteLevelID);

        if (l_SiteEntry == nullptr)
        {
            sLog->outAshran("GarrisonBrokenIsles::Init() not data found");
            assert(false && "GarrisonBrokenIsles::Init() not data found");
            return;
        }

        m_GarrisonLevelID   = l_SiteEntry->SiteLevelID;
        m_GarrisonSiteID    = l_SiteEntry->SiteID;
    }

}   ///< namespace GBrokenIsles
}   ///< namespace Garrison
}   ///< namespace MS

