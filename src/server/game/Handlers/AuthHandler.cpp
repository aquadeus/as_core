////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ObjectMgr.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "WorldPacket.h"

uint8 gExpansionPerClass[] =
{
    MAX_EXPANSION,                      ///< CLASS_NONE
    EXPANSION_VANILLA,                  ///< CLASS_WARRIOR
    EXPANSION_VANILLA,                  ///< CLASS_PALADIN
    EXPANSION_VANILLA,                  ///< CLASS_HUNTER
    EXPANSION_VANILLA,                  ///< CLASS_ROGUE
    EXPANSION_VANILLA,                  ///< CLASS_PRIEST
    EXPANSION_WRATH_OF_THE_LICH_KING,   ///< CLASS_DEATH_KNIGHT
    EXPANSION_VANILLA,                  ///< CLASS_SHAMAN
    EXPANSION_VANILLA,                  ///< CLASS_MAGE
    EXPANSION_VANILLA,                  ///< CLASS_WARLOCK
    EXPANSION_MISTS_OF_PANDARIA,        ///< CLASS_MONK
    EXPANSION_VANILLA,                  ///< CLASS_DRUID
    EXPANSION_LEGION,                   ///< CLASS_DEMON_HUNTER
};

void WorldSession::WriteAuthResponse(WorldPacket& p_Data, uint8 p_AuthResult, bool p_Queued, uint32 p_QueuePosition, uint32 p_WaitTime)
{
    p_Data.Initialize(SMSG_AUTH_RESPONSE);

    CharacterTemplates const& l_CharacterTemplates = sObjectMgr->GetCharacterTemplates();
    bool l_PremadeAvailable = sWorld->getBoolConfig(CONFIG_TEMPLATES_ENABLED);

    uint32 l_Templates               = l_PremadeAvailable ? l_CharacterTemplates.size() : 0;
    uint32 l_VirtualRealmsCount         = 1;
    uint32 l_Time                       = time(nullptr);
    uint32 l_TimeRested                 = 0;
    uint32 l_RealmRaceCount             = 0;
    uint32 l_RealmClassCount            = 0;
    uint32 l_TimeSecondsUntilPCKick     = 0;
    uint32 l_AccountCurrency            = 0;

    uint16 l_NumPlayersHorde    = 0;
    uint16 l_NumPlayersAlliance = 0;

    /// Billing Info
    uint32 l_BillingPlan          = 0;
    uint32 l_TimeRemain           = 0;
    bool   l_InGameRoom           = false;

    bool l_IsExpansionTrial          = false;
    bool l_ForceCharacterTemplate    = false;
    bool l_IsVeteranTrial            = false;
    bool l_HasFreeCharacterMigration = false;

    for (uint32 l_I = 0; l_I < sizeof(gExpansionPerClass); ++l_I)
    {
        if (gExpansionPerClass[l_I] != MAX_EXPANSION)
            l_RealmClassCount++;
    }

    p_Data << uint32(p_AuthResult);                                         ///< Result
    p_Data.WriteBit(p_AuthResult == BattlenetRpcErrorCode::ERROR_OK);       ///< Has Success Info
    p_Data.WriteBit(p_Queued);                                              ///< Has Wait Info
    p_Data.FlushBits();

    if (p_AuthResult == BattlenetRpcErrorCode::ERROR_OK)
    {
        p_Data << uint32(g_RealmID);                                        ///< VirtualRealmAddress
        p_Data << uint32(l_VirtualRealmsCount);                             ///< VirtualRealmsCount
        p_Data << uint32(l_TimeRested);                                     ///< TimeRested
        p_Data << uint8(Expansion::EXPANSION_BATTLE_FOR_AZEROTH);           ///< ActiveExpansionLevel
        p_Data << uint8(Expansion::EXPANSION_BATTLE_FOR_AZEROTH);           ///< AccountExpansionLevel
        p_Data << uint32(l_TimeSecondsUntilPCKick);                         ///< TimeSecondsUntilPCKick
        p_Data << uint32(l_RealmClassCount);                                ///< AvailableRaces
        p_Data << uint32(l_Templates);                                      ///< Templates
        p_Data << uint32(l_AccountCurrency);                                ///< AccountCurrency
        p_Data << uint32(l_Time);                                           ///< Time

        for (uint32 l_I = 0; l_I < sizeof(gExpansionPerClass); ++l_I)
        {
            if (gExpansionPerClass[l_I] != MAX_EXPANSION)
            {
                p_Data << uint8(l_I);                                       ///< Race Or Class ID
                p_Data << uint8(gExpansionPerClass[l_I]);                   ///< Required Expansion
            }
        }

        p_Data.WriteBit(l_IsExpansionTrial);                                ///< Is Expansion Trial
        p_Data.WriteBit(l_ForceCharacterTemplate);                          ///< Force Character Template
        p_Data.WriteBit(l_NumPlayersHorde != 0);                            ///< Has Num Players Horde
        p_Data.WriteBit(l_NumPlayersAlliance != 0);                         ///< Has Num Players Alliance
        p_Data.FlushBits();

        p_Data << uint32(l_BillingPlan);                                    ///< BillingPlan
        p_Data << uint32(l_TimeRemain);                                     ///< TimeRemain
        p_Data.WriteBit(l_InGameRoom);                                      ///< InGameRoom check in function checking which lua event to fire when remaining time is near end - BILLING_NAG_DIALOG vs IGR_BILLING_NAG_DIALOG
        p_Data.WriteBit(l_InGameRoom);                                      ///< InGameRoom lua return from Script_GetBillingPlan
        p_Data.WriteBit(l_InGameRoom);                                      ///< InGameRoom ( not used anywhere in the client)

        if (l_NumPlayersHorde)
            p_Data << uint16(l_NumPlayersHorde);                            ///< NumPlayersHorde

        if (l_NumPlayersAlliance)
            p_Data << uint16(l_NumPlayersAlliance);                         ///< NumPlayersAlliance

        for (uint32 l_I = 0; l_I < l_VirtualRealmsCount; ++l_I)
        {
            p_Data << uint32(g_RealmID);                                    ///< Realm Address

            p_Data.WriteBit(true);                                          ///< IsLocal
            p_Data.WriteBit(false);                                         ///< IsInternal
            p_Data.WriteBits(sWorld->GetRealmName().size(), 8);             ///< RealmNameActual
            p_Data.WriteBits(sWorld->GetNormalizedRealmName().size(), 8);   ///< RealmNameNormalized
            p_Data.FlushBits();

            p_Data.WriteString(sWorld->GetRealmName());                     ///< RealmNameActual
            p_Data.WriteString(sWorld->GetNormalizedRealmName());           ///< RealmNameNormalized
        }

        for (auto& l_Itr : l_CharacterTemplates)
        {
            if (!l_PremadeAvailable)
                break;

            CharacterTemplate const* l_Template = l_Itr.second;
            p_Data << uint32(l_Template->m_ID);                             ///< TemplateSetID
            p_Data << uint32(2);                                            ///< Classes Count

            // Loop here
            // 3/5 = ally/horde
            {
                p_Data << uint8(l_Template->m_PlayerClass);                 ///< Class ID
                p_Data << uint8(3);                                         ///< Faction Group
                p_Data << uint8(l_Template->m_PlayerClass);                 ///< Class ID
                p_Data << uint8(5);                                         ///< Faction Group
            }

            p_Data.WriteBits(l_Template->m_Name.size(), 7);                 ///< Name
            p_Data.WriteBits(l_Template->m_Description.size(), 10);         ///< Description
            p_Data.FlushBits();

            p_Data.WriteString(l_Template->m_Name);                         ///< Name
            p_Data.WriteString(l_Template->m_Description);                  ///< Description
        }
    }

    if (p_Queued)
    {
        p_Data << uint32(p_QueuePosition);                                  ///< WaitCount
        p_Data << uint32(p_WaitTime);                                       ///< WaitTime
        p_Data.WriteBit(l_HasFreeCharacterMigration);                       ///< HasFreeCharacterMigration
        p_Data.FlushBits();
    }
}

void WorldSession::SendClientCacheVersion(uint32 version)
{
    WorldPacket data(SMSG_CACHE_VERSION, 4);
    data << uint32(time(nullptr));
    SendPacket(&data);
}