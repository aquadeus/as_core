////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "ObjectMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "GameTables.h"
#include "Packets/ArtifactPackets.h"
#include "ScriptMgr.h"

void WorldSession::HandleArtifactLearnTrait(WorldPacket& p_RecvData)
{
    uint64 l_ArtifactGUID = 0;
    uint64 l_GameobjectGUID = 0;
    uint32 l_Count = 0;

    p_RecvData.readPackGUID(l_ArtifactGUID);
    p_RecvData.readPackGUID(l_GameobjectGUID);
    p_RecvData >> l_Count;

    Item* l_Artifact = m_Player->GetItemByGuid(l_ArtifactGUID);
    GameObject* l_GameObject =  m_Player->GetGameObjectIfCanInteractWith(l_GameobjectGUID, GAMEOBJECT_TYPE_ARTIFACT_FORGE);

    if (!l_Artifact || !l_GameObject || !l_Artifact->GetArtifactManager())
    {
        p_RecvData.rfinish();
        return;
    }

    for (uint32 l_I = 0; l_I < l_Count; ++l_I)
    {
        uint32 l_ID;
        uint8 l_Rank;

        p_RecvData >> l_ID;
        p_RecvData >> l_Rank;

        l_Artifact->GetArtifactManager()->AddOrUpgradeTrait(l_ID);
    }
}

void WorldSession::HandleArtifactSetAppearance(WorldPacket& p_RecvData)
{
    uint64 l_ArtifactGUID = 0;
    uint64 l_GameobjectGUID = 0;
    uint32 l_ArtifactAppearanceID = 0;

    p_RecvData.readPackGUID(l_ArtifactGUID);
    p_RecvData.readPackGUID(l_GameobjectGUID);
    p_RecvData >> l_ArtifactAppearanceID;

    Item* l_Artifact = m_Player->GetItemByGuid(l_ArtifactGUID);
    GameObject* l_GameObject =  m_Player->GetGameObjectIfCanInteractWith(l_GameobjectGUID, GAMEOBJECT_TYPE_ARTIFACT_FORGE);

    if (!l_Artifact || !l_GameObject || !l_Artifact->GetArtifactManager())
    {
        p_RecvData.rfinish();
        return;
    }

    l_Artifact->GetArtifactManager()->ChangeAppearance(l_ArtifactAppearanceID);
}

void WorldSession::HandleArtifactConfirmRespec(WorldPacket& p_RecvData)
{
    m_Player->SendGameError(GameError::ERR_SYSTEM_DISABLED);
    return;

    WorldPackets::Artifact::ConfirmRespec confirmArtifactRespec(p_RecvData);
    confirmArtifactRespec.Read();

    if (!m_Player->GetNPCIfCanInteractWith(confirmArtifactRespec.NpcGUID, UNIT_NPC_FLAG_ARTIFACT_POWER_RESPEC))
        return;

    MS::Artifact::Manager* l_Artifact = m_Player->GetCurrentlyEquippedArtifact();
    if (!l_Artifact)
        return;

    ArtifactLevelXPTableEntry const* l_Entry = g_ArtifactLevelXP.LookupEntry(l_Artifact->GetLevel() + 1);
    if (!l_Entry)
        return;

    uint64 l_XpCost = l_Artifact->ToItem()->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) == 1 ?  l_Entry->XP2 : l_Entry->XP;

    if (l_XpCost > l_Artifact->GetArtifactPower())
        return;

    uint64 l_NewAmount = l_Artifact->GetArtifactPower() - l_XpCost;
    for (uint32 l_I = 0; l_I <= l_Artifact->GetLevel(); ++l_I)
    {
        if (ArtifactLevelXPTableEntry const* l_Cost = g_ArtifactLevelXP.LookupEntry(l_I))
            l_NewAmount += uint64(l_Artifact->ToItem()->GetModifier(ItemModifiers::ITEM_MODIFIER_ARTIFACT_TIER) == 1 ? l_Entry->XP2 : l_Entry->XP);
    }

    l_Artifact->ResetPowers();
    l_Artifact->SetArtifactPower(l_NewAmount);

    m_Player->SaveToDB();
}

void WorldSession::SendOpenArtifactForge(uint64 p_ItemGUID, uint64 p_ForgeGUID)
{
    WorldPacket l_Data(SMSG_ARTIFACT_FORGE_OPENED);
    l_Data.appendPackGUID(p_ItemGUID);
    l_Data.appendPackGUID(p_ForgeGUID);
    SendPacket(&l_Data);
}

void WorldSession::SendArtifactRespec(uint64 p_Guid)
{
    MS::Artifact::Manager* l_Artifact = m_Player->GetCurrentlyEquippedArtifact();
    if (!l_Artifact || !l_Artifact->ToItem())
        return;

    WorldPackets::Artifact::RespecResult l_Packet;
    l_Packet.NpcGUID      = p_Guid;
    l_Packet.ArtifactGUID = l_Artifact->ToItem()->GetGUID();

    SendPacket(l_Packet.Write());
}

void WorldSession::HandleArtifactAddRelicTalent(WorldPacket& p_RecvData)
{
    uint64 l_ArtifactGuid;
    uint64 l_ForgeGuid;
    uint32 l_RelicSlot;
    uint8 l_TalentSlot;

    p_RecvData.readPackGUID(l_ArtifactGuid);
    p_RecvData.readPackGUID(l_ForgeGuid);
    p_RecvData >> l_RelicSlot;
    p_RecvData >> l_TalentSlot;

    if (l_RelicSlot < SOCK_ENCHANTMENT_SLOT || l_RelicSlot > SOCK_ENCHANTMENT_SLOT_3)
        return;

    l_RelicSlot -= SOCK_ENCHANTMENT_SLOT;

    if (!m_Player->GetGameObjectIfCanInteractWith(l_ForgeGuid, GameobjectTypes::GAMEOBJECT_TYPE_ARTIFACT_FORGE))
        return;

    Item* l_ArtifactItem = m_Player->GetItemByGuid(l_ArtifactGuid);
    if (!l_ArtifactItem)
        return;

    MS::Artifact::Manager* l_Artifact = m_Player->GetCurrentlyEquippedArtifact();
    if (!l_Artifact || l_Artifact->ToItem() != l_ArtifactItem)
        return;

    RelicTalentData* l_RelicTalent = l_ArtifactItem->GetRelicTalentData();

    /// This talent is already selected
    if (l_RelicTalent->PathMask[l_RelicSlot] & 1 << l_TalentSlot)
        return;

    /// Check if the player don't cheat by editing the packet
    switch (l_TalentSlot)
    {
        /// First tier
        case 0:
            /// Can't learn first talent if there is others talents already learn
            if (l_RelicTalent->PathMask[l_RelicSlot] != 0)
                return;
            break;
        /// Second tier
        case 1:
        case 2:
            /// Can't learn second tier talent if first one isn't learned yet
            if ((l_RelicTalent->PathMask[l_RelicSlot] & 1) == 0)
                return;
            /// Can't learn second tier talent if a second tier talent is already selected
            if (l_RelicTalent->PathMask[l_RelicSlot] & 6)
                return;
            break;
        /// Third tier
        case 3:
        case 4:
        case 5:
            /// Can't learn third tier talent if one of the second tier isn't learned yet
            if ((l_RelicTalent->PathMask[l_RelicSlot] & 6) == 0)
                return;
            /// Can't learn third tier talent if a third tier talent is already selected
            if (l_RelicTalent->PathMask[l_RelicSlot] & 0x38)
                return;
            break;
        /// Can't happen
        default:
            return;
    }

    RelicTalentEntry const* l_RelicTalentEntry = sRelicTalentStore.LookupEntry(l_RelicTalent->RelicTalent[l_RelicSlot][l_TalentSlot]);
    if (!l_RelicTalentEntry)
        return;

    if (l_RelicTalentEntry->ArtifactPowerLabel)
        l_Artifact->UpdatePowerBonusRankByType(l_RelicTalentEntry->ArtifactPowerLabel, 1, true);

    if (l_RelicTalentEntry->ArtifactPowerID)
        l_Artifact->UpdatePowerBonusRankById(l_RelicTalentEntry->ArtifactPowerID, 1, true);

    l_RelicTalent->PathMask[l_RelicSlot] |= 1 << l_TalentSlot;
    l_RelicTalent->NeedSave = true;

    l_ArtifactItem->SetState(ItemUpdateState::ITEM_CHANGED, m_Player);

    auto l_DynamicFields = l_RelicTalent->GetDynamicFieldStruct(l_RelicSlot);
    l_ArtifactItem->SetDynamicStructuredValue(ITEM_DYNAMIC_FIELD_RELIC_TALENT_DATA, sizeof(ItemDynamicFieldRelicTalentData) * l_RelicSlot, &l_DynamicFields);

    sScriptMgr->OnAddRelicTalent(m_Player);
}

void WorldSession::HandleArtifactAttunePreviewRelic(WorldPacket& p_RecvData)
{
    uint64 l_RelicGuid;
    uint64 l_ForgeGuid;

    p_RecvData.readPackGUID(l_RelicGuid);
    p_RecvData.readPackGUID(l_ForgeGuid);

    if (!m_Player->GetGameObjectIfCanInteractWith(l_ForgeGuid, GameobjectTypes::GAMEOBJECT_TYPE_ARTIFACT_FORGE))
        return;

    Item* l_Relic = m_Player->GetItemByGuid(l_RelicGuid);
    if (!l_Relic || !l_Relic->GetTemplate()->IsArtifactRelic())
        return;

    RelicTalentData* l_RelicTalentData = l_Relic->GetRelicTalentData();
    if (!l_RelicTalentData->IsInitialized(0))
    {
        ///< Disallow rolling the same talent as the relic has by default
        uint32 l_RelicType = 0;
        if (GemPropertiesEntry const* l_GemProperty = sGemPropertiesStore.LookupEntry(l_Relic->GetTemplate()->GetGemProperties()))
        {
            if (SpellItemEnchantmentEntry const* l_GemEnchant = sSpellItemEnchantmentStore.LookupEntry(l_GemProperty->EnchantID))
            {
                for (uint32 i = 0; i < MAX_ENCHANTMENT_SPELLS; ++i)
                {
                    switch (l_GemEnchant->type[i])
                    {
                    case ITEM_ENCHANTMENT_TYPE_ARTIFACT_POWER_BONUS_RANK_BY_TYPE:
                        l_RelicType = l_GemEnchant->spellid[i];
                        break;
                    }

                    if (l_RelicType)
                        break;
                }
            }
        }

        l_RelicTalentData->GenerateTalents(0, l_RelicType);
        l_Relic->SetState(ITEM_CHANGED, m_Player);
    }

    auto l_DynamicFields = l_RelicTalentData->GetDynamicFieldStruct(0);
    l_Relic->SetDynamicStructuredValue(ITEM_DYNAMIC_FIELD_RELIC_TALENT_DATA, 0, &l_DynamicFields);
}

void WorldSession::HandleArtifactAttuneSocketedRelic(WorldPacket& p_RecvData)
{
    uint64 l_ArtifactGuid;
    uint64 l_ForgeGuid;
    uint32 l_Slot;

    p_RecvData.readPackGUID(l_ArtifactGuid);
    p_RecvData.readPackGUID(l_ForgeGuid);
    p_RecvData >> l_Slot;

    if (l_Slot < SOCK_ENCHANTMENT_SLOT || l_Slot > SOCK_ENCHANTMENT_SLOT_3)
        return;

    l_Slot -= SOCK_ENCHANTMENT_SLOT;

    if (!m_Player->GetGameObjectIfCanInteractWith(l_ForgeGuid, GameobjectTypes::GAMEOBJECT_TYPE_ARTIFACT_FORGE))
        return;

    MS::Artifact::Manager* l_Artifact = m_Player->GetCurrentlyEquippedArtifact();
    if (!l_Artifact || !l_Artifact->ToItem())
        return;

    Item* l_ArtifactItem = l_Artifact->ToItem();
    if (l_ArtifactItem->GetGUID() != l_ArtifactGuid)
        return;

    RelicTalentData* l_RelicTalent = l_ArtifactItem->GetRelicTalentData();
    ItemDynamicFieldGems const* l_Relic = l_ArtifactItem->GetGem(l_Slot);

    if (l_Relic && l_Relic->ItemId && !l_RelicTalent->IsInitialized(l_Slot))
    {
        ///< Disallow rolling the same talent as the relic has by default
        uint32 l_RelicType = 0;
        if (GemPropertiesEntry const* l_GemProperty = sGemPropertiesStore.LookupEntry(sObjectMgr->GetItemTemplate(l_Relic->ItemId)->GetGemProperties()))
        {
            if (SpellItemEnchantmentEntry const* l_GemEnchant = sSpellItemEnchantmentStore.LookupEntry(l_GemProperty->EnchantID))
            {
                for (uint32 i = 0; i < MAX_ENCHANTMENT_SPELLS; ++i)
                {
                    switch (l_GemEnchant->type[i])
                    {
                    case ITEM_ENCHANTMENT_TYPE_ARTIFACT_POWER_BONUS_RANK_BY_TYPE:
                        l_RelicType = l_GemEnchant->spellid[i];
                        break;
                    }

                    if (l_RelicType)
                        break;
                }
            }
        }

        l_RelicTalent->GenerateTalents(l_Slot, l_RelicType);
        l_ArtifactItem->SetState(ItemUpdateState::ITEM_CHANGED, m_Player);
    }

    auto l_DynamicFields = l_RelicTalent->GetDynamicFieldStruct(l_Slot);
    l_ArtifactItem->SetDynamicStructuredValue(ITEM_DYNAMIC_FIELD_RELIC_TALENT_DATA, sizeof(ItemDynamicFieldRelicTalentData) * l_Slot, &l_DynamicFields);
}