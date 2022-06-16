////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "GarrisonFollower.hpp"
#include "GarrisonMgrConstants.hpp"
#include "DB2Stores.h"
#include "GarrisonMgr.hpp"
#include "Player.h"
#include "Object.h"

namespace MS { namespace Garrison
{
    /// Constructor
    GarrisonFollower::GarrisonFollower()
    {
        DatabaseID          = 0;
        FollowerID          = 0;
        Durability          = 1;
        Quality             = 0;
        Level               = 0;
        ItemLevelWeapon     = 0;
        ItemLevelArmor      = 0;
        XP                  = 0;
        CurrentBuildingID   = 0;
        CurrentMissionID    = 0;
        ZoneSupportSpellID  = 0;
        Flags               = 0;
        GarrClassSpecID     = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Follower can earn XP
    bool GarrisonFollower::CanXP() const
    {
        /// Troops can XP
        if ((this->Flags & FollowerFlags::isLimitedUse) != 0)
            return false;

        uint32_t l_MaxLevel = GetMaxLevelForExpansion(sGarrTypeStore.LookupEntry(GetEntry()->GarrisonType)->ExpansionRequired);

        if (Level < l_MaxLevel)
            return true;
        else if (GetEntry()->Type != eGarrisonFollowerTypeID::FollowerTypeIDShipyard && Level == l_MaxLevel && Quality < FollowerQualities::FOLLOWER_QUALITY_LEGENDARY)
            return true;

        return false;
    }

    uint32 GarrisonFollower::GetRequiredLevelUpXP() const
    {
        uint32_t l_MaxLevel = GetMaxLevelForExpansion(sGarrTypeStore.LookupEntry(GetEntry()->GarrisonType)->ExpansionRequired);
        if (GetEntry()->Type != eGarrisonFollowerTypeID::FollowerTypeIDShipyard)
        {
            if (Level < l_MaxLevel)
            {
                for (uint32 l_I = 0; l_I < sGarrFollowerLevelXPStore.GetNumRows(); ++l_I)
                {
                    const GarrFollowerLevelXPEntry* l_CurrentLevelData = sGarrFollowerLevelXPStore.LookupEntry(l_I);

                    if (l_CurrentLevelData && l_CurrentLevelData->Level == Level)
                    {
                        if (GarrFollowerEntry const* l_FollowerEntry = GetEntry())
                        {
                            if (l_CurrentLevelData->FollowerType == l_FollowerEntry->Type)
                                return l_CurrentLevelData->RequiredExperience;
                        }
                    }
                }
            }
            else if (Level == l_MaxLevel && Quality < FollowerQualities::FOLLOWER_QUALITY_LEGENDARY )
            {
                if (l_MaxLevel && sGarrTypeStore.LookupEntry(GetEntry()->GarrisonType)->ExpansionRequired == Expansion::EXPANSION_WARLORDS_OF_DRAENOR)
                    /// These values are not present in DB2
                    ///  60 000 XP for FOLLOWER_QUALITY_UNCOMMON -> FOLLOWER_QUALITY_RARE
                    /// 120 000 XP for FOLLOWER_QUALITY_RARE     -> FOLLOWER_QUALITY_EPIC
                    return Quality == FollowerQualities::FOLLOWER_QUALITY_UNCOMMON ? 60000 : 120000;
                else
                {
                    switch (Quality)
                    {
                        case FollowerQualities::FOLLOWER_QUALITY_NORMAL:
                            return 8000;
                        case FollowerQualities::FOLLOWER_QUALITY_UNCOMMON:
                            return 20000;
                        case FollowerQualities::FOLLOWER_QUALITY_RARE:
                            return 100000;
                        case FollowerQualities::FOLLOWER_QUALITY_EPIC:
                            return 200000;
                        default:
                            break;
                    }
                }
            }
        }
        else if (Quality < FOLLOWER_QUALITY_EPIC)
        {
            /// These values are not present in DB2
            ///   5 000 XP for FOLLOWER_QUALITY_UNCOMMON -> FOLLOWER_QUALITY_RARE
            /// 200 000 XP for FOLLOWER_QUALITY_RARE     -> FOLLOWER_QUALITY_EPI
            return Quality == FollowerQualities::FOLLOWER_QUALITY_UNCOMMON ? 5000 : 40000;
        }

        return 0;
    }

    /// Earn XP
    uint32 GarrisonFollower::EarnXP(uint32 p_XP, Player* p_SendUpdateTo /* = nullptr */)
    {
        if (!CanXP() || !p_XP)
            return 0;

        ByteBuffer l_Buff(150);
        Write(l_Buff);

        uint32 l_OldQuality = Quality;
        uint32 l_LearnedXP = _EarnXP(p_XP);

        if (Quality != l_OldQuality)
           const_cast<Manager*>(p_SendUpdateTo->GetGarrison())->GenerateFollowerAbilities(DatabaseID, false, true, true, true);

        if (l_LearnedXP && p_SendUpdateTo)
        {
            WorldPacket l_Update(SMSG_GARRISON_FOLLOWER_CHANGED_XP, 500);
            l_Update << uint32(l_LearnedXP);    ///< Earned XP
            l_Update << uint32(0);              ///< Type ??? if == 2 then GARRISON_FOLLOWER_XP_ADDED_SHIPMENT, == 4 GARRISON_FOLLOWER_XP_ADDED_ZONE_SUPPORT
            l_Update.append(l_Buff);            ///< Old Follower
            Write(l_Update);                    ///< New Follower
            p_SendUpdateTo->SendDirectMessage(&l_Update);
        }

        return l_LearnedXP;
    }

    uint32 GarrisonFollower::_EarnXP(uint32 p_XP)
    {
        uint32 l_MaxXP = GetRequiredLevelUpXP();
        uint32 l_MaxLevel = GetMaxLevelForExpansion(sGarrTypeStore.LookupEntry(GetEntry()->GarrisonType)->ExpansionRequired);

        if (!l_MaxXP)
            return 0;

        if (Level < l_MaxLevel)
        {
            if ((p_XP + this->XP) >= l_MaxXP)
            {
                uint32 l_Value = l_MaxXP - this->XP;
                this->XP = 0;
                this->Level++;

                return l_Value + _EarnXP(p_XP - l_Value);
            }
            else
            {
                this->XP += p_XP;
                return p_XP;
            }
        }
        else if (this->Level == l_MaxLevel && this->Quality < FollowerQualities::FOLLOWER_QUALITY_LEGENDARY)
        {
            if ((this->XP + p_XP) >= l_MaxXP)
            {
                uint32 l_Value = l_MaxXP - this->XP;
                this->XP = 0;
                this->Quality++;

                return l_Value + _EarnXP(p_XP - l_Value);
            }
            else
            {
                this->XP += p_XP;
                return p_XP;
            }
        }

        return 0;
    }

    GarrFollowerEntry const* GarrisonFollower::GetEntry() const
    {
        return sGarrFollowerStore.LookupEntry(FollowerID);
    }

    bool GarrisonFollower::IsShip() const
    {
        GarrFollowerEntry const* l_Entry = GetEntry();
        return l_Entry && l_Entry->Type == eGarrisonFollowerTypeID::FollowerTypeIDShipyard;
    }

    void GarrisonFollower::SendFollowerUpdate(WorldSession* p_Session) const
    {
        WorldPacket l_Data(SMSG_GARRISON_FOLLOWER_CHANGED_STATUS, 500);
        l_Data << uint32(ResultEnum::SUCCESS);
        Write(l_Data);
        p_Session->SendPacket(&l_Data);
    }

    void GarrisonFollower::SendFollowerUpdate(Player* p_Player) const
    {
        SendFollowerUpdate(p_Player->GetSession());
    }

    std::string GarrisonFollower::GetRealName(uint32 p_FactionIndex) const
    {
        auto l_Entry = GetEntry();

        if (!l_Entry)
            return "";

        if (ShipName.length())
            return ShipName;

        CreatureTemplate const* l_CreatureTemplate = sObjectMgr->GetCreatureTemplate(l_Entry->CreatureID[p_FactionIndex]);
        return l_CreatureTemplate ? l_CreatureTemplate->Name[0] : "";
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Write follower into a packet
    void GarrisonFollower::Write(ByteBuffer& p_Buffer) const
    {
        p_Buffer << uint64(this->DatabaseID);
        p_Buffer << uint32(this->FollowerID);
        p_Buffer << uint32(this->Quality);
        p_Buffer << uint32(this->Level);
        p_Buffer << uint32(this->ItemLevelWeapon);
        p_Buffer << uint32(this->ItemLevelArmor);
        p_Buffer << uint32(this->XP);
        p_Buffer << uint32(this->Durability);
        p_Buffer << uint32(this->CurrentBuildingID);
        p_Buffer << uint32(this->CurrentMissionID);
        p_Buffer << uint32(this->Abilities.size());
        p_Buffer << uint32(this->ZoneSupportSpellID);
        p_Buffer << uint32(this->Flags);

        for (uint32 l_Y = 0; l_Y < this->Abilities.size(); ++l_Y)
            p_Buffer << int32(this->Abilities[l_Y]);

        p_Buffer.WriteBits(this->ShipName.length(), 7);
        p_Buffer.FlushBits();
        p_Buffer.WriteString(this->ShipName);
    }

    GarrFollowerTypeEntry const* GarrisonFollower::GetFollowerType() const
    {
        return sGarrFollowerTypeStore.LookupEntry(GetEntry()->Type);
    }
}   ///< namespace Garrison
}   ///< namespace MS

