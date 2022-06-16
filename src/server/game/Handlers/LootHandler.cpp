////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "OutdoorPvPMgr.h"
#include "Common.h"
#include "WorldPacket.h"
#include "Log.h"
#include "Corpse.h"
#include "GameObject.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "WorldSession.h"
#include "LootMgr.h"
#include "Object.h"
#include "Group.h"
#include "World.h"
#include "Util.h"
#include "GuildMgr.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "ChallengeMgr.h"

void WorldSession::HandleLootItemOpcode(WorldPacket & p_RecvData)
{
    Loot* l_Loot = nullptr;

    uint64 l_LootGuid = m_Player->GetLootGUID();

    uint32 l_LootCount = 0;

    uint8 l_LootListID      = 0;
    uint8 l_LinkedLootSlot  = 255;

    std::vector<uint64> l_LootObjectGuids;

    p_RecvData >> l_LootCount;

    l_LootObjectGuids.resize(l_LootCount);

    for (uint32 l_I = 0; l_I < l_LootCount; l_I++)
    {
        p_RecvData.readPackGUID(l_LootObjectGuids[l_I]);
        p_RecvData >> l_LootListID;

        l_LinkedLootSlot = 0xFF;

        if (IS_GAMEOBJECT_GUID(l_LootGuid))
        {
            GameObject * l_GameObject = m_Player->GetMap()->GetGameObject(l_LootGuid);

            // not check distance for GO in case owned GO (fishing bobber case, for example) or Fishing hole GO
            if (!l_GameObject || ((l_GameObject->GetOwnerGUID() != m_Player->GetGUID() && l_GameObject->GetGoType() != GAMEOBJECT_TYPE_FISHINGHOLE) && !l_GameObject->IsWithinDistInMap(m_Player, INTERACTION_DISTANCE)))
            {
                m_Player->SendLootRelease(l_LootGuid);
                return;
            }

            uint64 l_LootContainerGuid = m_Player->GetLootContainerGuidForGameObject(l_GameObject);
            if (!l_LootContainerGuid)
            {
                m_Player->SendLootRelease(l_LootGuid);
                return;
            }

            l_Loot = &l_GameObject->m_LootContainers[l_LootContainerGuid];
        }
        else if (IS_ITEM_GUID(l_LootGuid))
        {
            Item * l_Item = m_Player->GetItemByGuid(l_LootGuid);

            if (!l_Item)
            {
                m_Player->SendLootRelease(l_LootGuid);
                return;
            }

            l_Loot = &l_Item->loot;
        }
        else if (IS_CORPSE_GUID(l_LootGuid))
        {
            Corpse * l_Bones = ObjectAccessor::GetCorpse(*m_Player, l_LootGuid);

            if (!l_Bones)
            {
                m_Player->SendLootRelease(l_LootGuid);
                return;
            }

            l_Loot = &l_Bones->loot;
        }
        else
        {
            Creature* l_Creature = GetPlayer()->GetMap()->GetCreature(l_LootGuid);

            bool l_IsLootAllowed = l_Creature && l_Creature->isAlive() == (m_Player->getClass() == CLASS_ROGUE && l_Creature->lootForPickPocketed);

            /// Check for Glyph of Fetch too
            if (!l_IsLootAllowed || (!l_Creature->IsWithinDistInMap(m_Player, INTERACTION_DISTANCE) && !m_Player->HasSpell(125050)))
            {
                m_Player->SendLootRelease(l_LootGuid);
                return;
            }

            uint64 l_LootContainerGuid = m_Player->GetLootContainerGuidForCreature(l_Creature);
            if (!l_LootContainerGuid)
            {
                m_Player->SendLootRelease(l_LootGuid);
                return;
            }

            l_Loot = &l_Creature->m_LootContainers[l_LootContainerGuid];

            if (l_Loot->isLinkedLoot(l_LootListID))
            {
                LinkedLootInfo linkedLootInfo = l_Loot->getLinkedLoot(l_LootListID);

                l_Creature = m_Player->GetCreature(*m_Player, linkedLootInfo.creatureGUID);

                if (!l_Creature)
                {
                    m_Player->SendLootRelease(l_LootGuid);
                    return;
                }

                l_LootContainerGuid = m_Player->GetLootContainerGuidForCreature(l_Creature);
                if (!l_LootContainerGuid)
                {
                    m_Player->SendLootRelease(l_LootGuid);
                    return;
                }

                l_Loot = &l_Creature->m_LootContainers[l_LootContainerGuid];
                l_LinkedLootSlot = l_LootListID;
                l_LootListID = linkedLootInfo.totalSlot;
            }
        }

        m_Player->StoreLootItem(l_LootListID, l_Loot, l_LinkedLootSlot);
    }
}

void WorldSession::HandleLootMoneyOpcode(WorldPacket& /*recvData*/)
{
    Player* player = GetPlayer();

    uint64 guid = player->GetLootGUID();
    if (!guid)
        return;

    Loot* masterLoot = nullptr;
    std::list<Loot*> linkedLoots;
    bool shareMoney = true;

    std::string l_Source = "";

    switch (GUID_HIPART(guid))
    {
        case HIGHGUID_GAMEOBJECT:
        {
            GameObject* go = GetPlayer()->GetMap()->GetGameObject(guid);

            // do not check distance for GO if player is the owner of it (ex. fishing bobber)
            if (go && ((go->GetOwnerGUID() == player->GetGUID() || go->IsWithinDistInMap(player, INTERACTION_DISTANCE))))
            {
                uint64 l_LootContainerGuid = m_Player->GetLootContainerGuidForGameObject(go);
                if (!l_LootContainerGuid)
                    return;

                masterLoot = &go->m_LootContainers[l_LootContainerGuid];
                l_Source = "HandleLootMoneyOpcode - GOB - " + std::to_string(go->GetEntry());
            }

            break;
        }
        case HIGHGUID_CORPSE:                               // remove insignia ONLY in BG
        {
            Corpse* bones = ObjectAccessor::GetCorpse(*player, guid);

            /// Check for Glyph of Fetch too
            if (bones && (bones->IsWithinDistInMap(player, INTERACTION_DISTANCE) || m_Player->HasSpell(125050)))
            {
                masterLoot = &bones->loot;
                shareMoney = false;
            }

            l_Source = "HandleLootMoneyOpcode - Corpse";

            break;
        }
        case HIGHGUID_ITEM:
        {
            if (Item* item = player->GetItemByGuid(guid))
            {
                masterLoot = &item->loot;
                shareMoney = false;
                l_Source = "HandleLootMoneyOpcode - Item - " + std::to_string(item->GetEntry());
            }
            break;
        }
        case HIGHGUID_UNIT:
        case HIGHGUID_VEHICLE:
        {
            Creature* creature = player->GetMap()->GetCreature(guid);
            bool lootAllowed = creature && creature->isAlive() == (player->getClass() == CLASS_ROGUE && creature->lootForPickPocketed);

            /// Check for Glyph of Fetch too
            if (lootAllowed && (creature->IsWithinDistInMap(player, INTERACTION_DISTANCE) || m_Player->HasSpell(125050)))
            {
                uint64 l_LootContainerGuid = player->GetLootContainerGuidForCreature(creature);
                if (!l_LootContainerGuid)
                    return;

                l_Source = "HandleLootMoneyOpcode - Creature - " + std::to_string(creature->GetEntry());

                masterLoot = &creature->m_LootContainers[l_LootContainerGuid];
                if (creature->isAlive() || creature->GetMap()->GetDifficultyID() == Difficulty::DifficultyNone)
                    shareMoney = false;

                std::list<Creature*> linkedLootCreatures;
                CellCoord p(JadeCore::ComputeCellCoord(player->GetPositionX(), player->GetPositionY()));
                Cell cell(p);
                cell.SetNoCreate();

                JadeCore::AllDeadCreaturesInRange l_Check(player, 50.0f, creature->GetGUID());
                JadeCore::CreatureListSearcher<JadeCore::AllDeadCreaturesInRange> l_Searcher(player, linkedLootCreatures, l_Check);
                TypeContainerVisitor<JadeCore::CreatureListSearcher<JadeCore::AllDeadCreaturesInRange>, GridTypeMapContainer> l_CellSearcher(l_Searcher);
                TypeContainerVisitor<JadeCore::CreatureListSearcher<JadeCore::AllDeadCreaturesInRange>, WorldTypeMapContainer> l_WorldSearcher(l_Searcher);

                cell.Visit(p, l_CellSearcher, *(player->GetMap()), *player, 50.0f);
                cell.Visit(p, l_WorldSearcher, *(player->GetMap()), *player, 50.0f);

                for (auto itr : linkedLootCreatures)
                {
                    uint64 l_LootContainerGuid = player->GetLootContainerGuidForCreature(itr);
                    if (!l_LootContainerGuid)
                        continue;

                    Loot* l_Loot = &itr->m_LootContainers[l_LootContainerGuid];
                    if (!l_Loot->Gold)
                        continue;

                    linkedLoots.push_back(l_Loot);
                }
            }
            break;
        }
        default:
            return;                                         // unlootable type
    }

    if (masterLoot)
        linkedLoots.push_back(masterLoot);

    if (GroupPtr l_Group = player->GetGroup())
    {
        /// Personal loot golds are setup for each players, no need to share
        if (l_Group->GetLootMethod() == LootMethod::PERSONAL_LOOT)
            shareMoney = false;
    }

    for (auto loot : linkedLoots)
    {
        if (shareMoney && player->GetGroup())      //item, pickpocket and players can be looted only single player
        {
            GroupPtr group = player->GetGroup();

            std::vector<Player*> playersNear;
            group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Member)
            {
                Player* member = l_Member->player;
                if (!member)
                    return;

                if (player->IsWithinDistInMap(member, sWorld->getFloatConfig(CONFIG_GROUP_XP_DISTANCE), false))
                    playersNear.push_back(member);
            });

            bool l_IsRaid = player->GetMap()->IsRaid();
            uint32 l_GoldPerPlayer = uint32((loot->Gold) / (playersNear.size()));

            loot->NotifyMoneyRemoved();

            for (std::vector<Player*>::const_iterator i = playersNear.begin(); i != playersNear.end(); ++i)
            {
                (*i)->ModifyMoney(l_GoldPerPlayer, l_Source);
                (*i)->UpdateCriteria(CRITERIA_TYPE_LOOT_MONEY, l_GoldPerPlayer);

#ifndef CROSS
                /// @TODO: cross sync
                if ((*i)->HasAuraType(SPELL_AURA_DEPOSIT_BONUS_MONEY_IN_GUILD_BANK_ON_LOOT))
                {
                    if (Guild* guild = sGuildMgr->GetGuildById((*i)->GetGuildId()))
                    {
                        uint64 guildGold = uint64(CalculatePct(l_GoldPerPlayer, (*i)->GetTotalAuraModifier(SPELL_AURA_DEPOSIT_BONUS_MONEY_IN_GUILD_BANK_ON_LOOT)));
                        if (guildGold > MAX_MONEY_AMOUNT)
                            guildGold = MAX_MONEY_AMOUNT;

                        uint64 amount = guild->GetBankMoney();
                        uint64 after = amount + guildGold;
                        if (after > MAX_MONEY_AMOUNT)
                            guildGold = uint64(MAX_MONEY_AMOUNT) - amount; // do not cast MAX_MONEY_AMOUNT to uint since MAX_MONEY_AMOUNT is 9.999 billions, that more that uint32 max

                        if (guildGold)
                            guild->DepositMoney(guildGold);
                    }
                }
#endif

                WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
                data << uint32(l_GoldPerPlayer);
                data.WriteBit(playersNear.size() <= 1); // Controls the text displayed in chat. 0 is "Your share is..." and 1 is "You loot..."
                data.FlushBits();
                (*i)->GetSession()->SendPacket(&data);
            }
        }
        else
        {
            player->ModifyMoney(loot->Gold, l_Source);
            player->UpdateCriteria(CRITERIA_TYPE_LOOT_MONEY, loot->Gold);

            if (player->HasAuraType(SPELL_AURA_DEPOSIT_BONUS_MONEY_IN_GUILD_BANK_ON_LOOT))
            {
                /// @TODO: sync cross
                #ifndef CROSS
                if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId()))
                {
                    uint64 guildGold = uint64(CalculatePct(loot->Gold, player->GetTotalAuraModifier(SPELL_AURA_DEPOSIT_BONUS_MONEY_IN_GUILD_BANK_ON_LOOT)));
                    if (guildGold > MAX_MONEY_AMOUNT)
                        guildGold = MAX_MONEY_AMOUNT;

                    uint64 amount = guild->GetBankMoney();
                    uint64 after = amount + guildGold;
                    if (after > MAX_MONEY_AMOUNT)
                        guildGold = uint64(MAX_MONEY_AMOUNT) - amount; // do not cast MAX_MONEY_AMOUNT to uint since MAX_MONEY_AMOUNT is 9.999 billions, that more that uint32 max

                    if (guildGold)
                        guild->DepositMoney(guildGold);
                }
                #endif
            }

            loot->NotifyMoneyRemoved();

            WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4 + 1);
            data << uint32(loot->Gold);
            data.WriteBit(1);   // "You loot..."
            data.FlushBits();
            SendPacket(&data);
        }

        loot->Gold = 0;
    }
}

void WorldSession::HandleLootOpcode(WorldPacket& p_RecvData)
{
    uint64 l_UnitGuid = 0;
    p_RecvData.readPackGUID(l_UnitGuid);

    /// Check possible cheat
    if (!m_Player->isAlive())
        return;

    m_Player->SendLoot(l_UnitGuid, LOOT_CORPSE);

    /// Interrupt cast
    if (m_Player->IsNonMeleeSpellCasted(false))
        m_Player->InterruptNonMeleeSpells(false);
}

void WorldSession::HandleLootReleaseOpcode(WorldPacket& p_RecvPacket)
{
    // cheaters can modify lguid to prevent correct apply loot release code and re-loot
    // use internal stored guid
    uint64 l_ObjectGuid;

    p_RecvPacket.readPackGUID(l_ObjectGuid);

    if (uint64 l_LootGuid = GetPlayer()->GetLootGUID())
        if (l_LootGuid == l_ObjectGuid)
            DoLootRelease(l_LootGuid);
}

void WorldSession::DoLootRelease(uint64 lguid)
{
    Player  *player = GetPlayer();
    Loot    *loot;

    if (player && player)
    {
        player->SetLootGUID(0);
        player->SendLootRelease(lguid);
        player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);
    }

    if (!player->IsInWorld())
        return;

    sOutdoorPvPMgr->HandleLootReleased(player, lguid);

    if (IS_GAMEOBJECT_GUID(lguid))
    {
        GameObject* go = GetPlayer()->GetMap()->GetGameObject(lguid);

        // not check distance for GO in case owned GO (fishing bobber case, for example) or Fishing hole GO
        if (!go)
            return;

        uint64 l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForGameObject(go);
        if (!l_LootContainerGuid)
            return;

        loot = &go->m_LootContainers[l_LootContainerGuid];

        if (go->GetGoType() == GAMEOBJECT_TYPE_CHALLENGE_MODE_REWARD)
            sChallengeMgr->DeleteOploteLoot(player->GetGUID(), true);

        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR)
        {
            // locked doors are opened with spelleffect openlock, prevent remove its as looted
            go->UseDoorOrButton();
        }
        else if (loot->isLooted() || go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE)
        {
            if (go->GetGoType() == GAMEOBJECT_TYPE_CHALLENGE_MODE_REWARD)
                sChallengeMgr->DeleteOploteLoot(player->GetGUID(), false);

            if (go->GetGoType() == GAMEOBJECT_TYPE_FISHINGHOLE)
            {                                               // The fishing hole used once more
                go->AddUse();                               // if the max usage is reached, will be despawned in next tick

                if (go->GetUseCount() >= urand(go->GetGOInfo()->fishingHole.minRestock, go->GetGOInfo()->fishingHole.maxRestock))
                    go->SetLootState(GO_JUST_DEACTIVATED);
                else
                    go->SetLootState(GO_READY);
            }
            else if (go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE)
            {
                go->SetLootState(GO_JUST_DEACTIVATED);
                loot->clear();
            }
            else
            {
                /// Instances case (dungeons/raids/scenarios)
                if (go->GetMap()->GetDifficultyID() != Difficulty::DifficultyNone)
                {
                    /// gob despawn after everyone in the dungeon/raid loot it (personal loot)
                    bool l_EveryoneLoot = true;
                    for (auto& l_Itr : go->m_LootContainers)
                    {
                        if (!l_Itr.second.isLooted())
                            l_EveryoneLoot = false;
                    }

                    if (l_EveryoneLoot)
                        go->SetLootState(GO_JUST_DEACTIVATED);
                    else
                        go->SetLootState(GO_ACTIVATED, player);

                }
                /// Open world
                else
                {
                    /// Allow other players to use it
                    go->SetLootState(GO_READY);

                    /// Empty our loot container
                    loot->clear();

                    /// Everyone can still loot it for 30 secs when the first player release it if it's a gathering node
                    /// @TODO: Some node have the go type GAMEOBJECT_TYPE_CHEST ... Maybe get the lockid / skillid required to identify them?
                    if (go->GetGoType() == GAMEOBJECT_TYPE_GATHERING_NODE && !go->GetLastLootTime())
                        go->SetLastLootTime(time(nullptr));
                    /// The chest should disappear all the respawn period for the looters
                    // else
                    {
                        go->SetLootState(GO_READY);

                        std::set<Player*> l_Looters;
                        l_Looters.insert(player);

                        if (IS_GROUP(l_LootContainerGuid))
                        {
                            if (GroupPtr l_Group = player->GetGroup())
                            {
                                l_Group->GetMemberSlots().foreach([&](Group::MemberSlotPtr l_Itr)
                                {
                                    Player* l_Member = l_Itr->player;
                                    if (!l_Member || l_Member->GetMap() != player->GetMap())
                                        return;

                                    l_Looters.insert(l_Member);
                                });
                            }
                        }

                        for (Player* l_Player : l_Looters)
                        {
                            go->AddToHideList(l_Player->GetGUID());
                            go->DestroyForPlayer(l_Player, false);

                            uint64 l_PlayerGuid = l_Player->GetGUID();
                            go->AddDelayedEvent
                            (
                                [l_PlayerGuid, go, l_LootContainerGuid]()->void
                                {
                                    go->RemoveFromHideList(l_PlayerGuid);
                                    go->m_LootContainers.erase(l_LootContainerGuid);
                                    go->UpdateObjectVisibility();
                                },
                                go->GetRespawnDelay() * IN_MILLISECONDS
                            );
                        }
                    }
                }
            }
        }
        else
        {
            /// Instances case (dungeons/raids/scenarios)
            /// All players of the group can still loot it, keep it activated
            if (go->GetMap()->GetDifficultyID() != Difficulty::DifficultyNone)
                go->SetLootState(GO_ACTIVATED, player);
            /// Everyone can still loot it for 30 secs when the first player release it if it's a gathering node
            /// @TODO: Some node have the go type GAMEOBJECT_TYPE_CHEST ... Maybe get the lockid / skillid required to identify them?
            else
            {
                if (go->GetGoType() == GAMEOBJECT_TYPE_GATHERING_NODE && !go->GetLastLootTime())
                    go->SetLastLootTime(time(nullptr));

                go->SetLootState(GO_READY);
            }
        }

        if (go->AI())
            go->AI()->OnLootReleased(player);
    }
    else if (IS_CORPSE_GUID(lguid))        // ONLY remove insignia at BG
    {
        Corpse* corpse = ObjectAccessor::GetCorpse(*player, lguid);
        /// Check for Glyph of Fetch too
        if (!corpse || (!corpse->IsWithinDistInMap(m_Player, INTERACTION_DISTANCE) && !m_Player->HasSpell(125050)))
            return;

        loot = &corpse->loot;

        if (loot->isLooted())
        {
            loot->clear();
            corpse->RemoveFlag(CORPSE_FIELD_DYNAMIC_FLAGS, CORPSE_DYNFLAG_LOOTABLE);
        }
    }
    else if (IS_ITEM_GUID(lguid))
    {
        Item* pItem = player->GetItemByGuid(lguid);
        if (!pItem)
            return;

        ItemTemplate const* proto = pItem->GetTemplate();

        // destroy only 5 items from stack in case prospecting and milling
        if (proto->Flags & (int32(ItemFlags::IS_PROSPECTABLE) | int32(ItemFlags::IS_MILLABLE)))
        {
            pItem->m_lootGenerated = false;
            pItem->loot.clear();

            uint32 count = pItem->GetCount();

            // >=5 checked in spell code, but will work for cheating cases also with removing from another stacks.
            if (count > 5)
                count = 5;

            player->DestroyItemCount(pItem, count, true);
        }
        else
            // FIXME: item must not be deleted in case not fully looted state. But this pre-request implement loot saving in DB at item save. Or cheating possible.
            player->DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
        return;                                             // item can be looted only single player
    }
    else
    {
        Creature* creature = GetPlayer()->GetMap()->GetCreature(lguid);

        bool lootAllowed = creature && creature->isAlive() == (player->getClass() == CLASS_ROGUE && creature->lootForPickPocketed);

        /// Check for Glyph of Fetch too
        if (!lootAllowed || (!creature->IsWithinDistInMap(m_Player, INTERACTION_DISTANCE) && !m_Player->HasSpell(125050)))
            return;

        uint64 l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForCreature(creature);
        if (!l_LootContainerGuid)
            return;

        loot = &creature->m_LootContainers[l_LootContainerGuid];
        if (loot->isLooted())
        {
            loot->clear();

            // Clear all linkedLoot looted
            std::set<uint64> alreadyClear;
            for (auto itr : loot->LinkedLoot)
            {
                if (alreadyClear.find(itr.second.creatureGUID) == alreadyClear.end())
                {
                    alreadyClear.insert(itr.second.creatureGUID);
                    if (Creature* c = creature->GetCreature(*creature, itr.second.creatureGUID))
                    {
                        uint64 l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForCreature(c);
                        if (!l_LootContainerGuid)
                            continue;

                        Loot* l_Loot = &c->m_LootContainers[l_LootContainerGuid];

                        if (!l_Loot->isLooted())
                            continue;

                        l_Loot->clear();
                        l_Loot->RemoveLooter(player->GetGUID());

                        c->ForceValuesUpdateAtIndex(OBJECT_FIELD_DYNAMIC_FLAGS);
                    }
                }
            }
            loot->LinkedLoot.clear();
        }

        bool l_CanRemoveLootFlag = true;
        for (auto& l_Itr : creature->m_LootContainers)
        {
            if (!l_Itr.second.isLooted())
            {
                l_CanRemoveLootFlag = false;
                break;
            }
        }

        if (l_CanRemoveLootFlag)
        {
            creature->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            if (!creature->isAlive())
                creature->AllLootRemovedFromCorpse();
        }
    }

    //Player is not looking at loot list, he doesn't need to see updates on the loot list
    loot->RemoveLooter(player->GetGUID());
}

void WorldSession::HandleDoMasterLootRollOpcode(WorldPacket & p_Packet)
{
    uint64 l_ObjectGUID = 0;
    uint8 l_LootListID = 0;

    p_Packet.readPackGUID(l_ObjectGUID);
    p_Packet >> l_LootListID;

    if (!m_Player->GetGroup() || m_Player->GetGroup()->GetLooterGuid() != m_Player->GetGUID())
    {
        m_Player->SendLootRelease(GetPlayer()->GetLootGUID());
        return;
    }

    Loot * l_Loot = NULL;

    if (IS_CRE_OR_VEH_GUID(GetPlayer()->GetLootGUID()))
    {
        Creature* l_Creature = GetPlayer()->GetMap()->GetCreature(l_ObjectGUID);

        if (!l_Creature)
            return;

        uint64 l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForCreature(l_Creature);
        if (!l_LootContainerGuid)
            return;

        l_Loot = &l_Creature->m_LootContainers[l_LootContainerGuid];

        if (l_Loot->isLinkedLoot(l_LootListID))
        {
            LinkedLootInfo linkedLootInfo = l_Loot->getLinkedLoot(l_LootListID);
            l_Creature = GetPlayer()->GetCreature(*GetPlayer(), linkedLootInfo.creatureGUID);
            if (!l_Creature)
                return;

            l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForCreature(l_Creature);
            if (!l_LootContainerGuid)
                return;

            l_Loot = &l_Creature->m_LootContainers[l_LootContainerGuid];
            l_LootListID = linkedLootInfo.slot;
        }
    }
    else if (IS_GAMEOBJECT_GUID(GetPlayer()->GetLootGUID()))
    {
        GameObject* l_GameObject = GetPlayer()->GetMap()->GetGameObject(l_ObjectGUID);
        if (!l_GameObject)
            return;

        uint64 l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForGameObject(l_GameObject);
        if (!l_LootContainerGuid)
            return;

        l_Loot = &l_GameObject->m_LootContainers[l_LootContainerGuid];
    }

    if (!l_Loot)
        return;

    if (l_LootListID >= l_Loot->Items.size() + l_Loot->QuestItems.size())
    {
        sLog->outDebug(LOG_FILTER_LOOT, "MasterLootItem: Player %s might be using a hack! (slot %d, size %lu)", GetPlayer()->GetName(), l_LootListID, (unsigned long)l_Loot->Items.size());
        return;
    }

    LootItem& l_Item = l_LootListID >= l_Loot->Items.size() ? l_Loot->QuestItems[l_LootListID - l_Loot->Items.size()] : l_Loot->Items[l_LootListID];
    if (l_Item.alreadyAskedForRoll)
        return;

    l_Item.alreadyAskedForRoll = true;

    m_Player->GetGroup()->DoRollForAllMembers(l_ObjectGUID, l_LootListID, m_Player->GetMapId(), l_Loot, l_Item, m_Player);
}

void WorldSession::HandleMasterLootItemOpcode(WorldPacket & p_Packet)
{
    uint64 l_TargetGUID = 0;
    uint32 l_LootCount  = 0;

    p_Packet >> l_LootCount;
    p_Packet.readPackGUID(l_TargetGUID);

    if (l_LootCount > 40)
        return;

    std::vector<uint64> l_Objects(l_LootCount);
    std::vector<uint8>  l_LootListIDs(l_LootCount);

    for (uint32 l_I = 0; l_I < l_LootCount; ++l_I)
    {
        p_Packet.readPackGUID(l_Objects[l_I]);
        p_Packet >> l_LootListIDs[l_I];
    }

    if (!m_Player->GetGroup() || m_Player->GetGroup()->GetLooterGuid() != m_Player->GetGUID())
    {
        m_Player->SendLootRelease(GetPlayer()->GetLootGUID());
        return;
    }

    Player * l_Target = ObjectAccessor::FindPlayer(l_TargetGUID);
    if (!l_Target)
        return;

    for (uint32 l_I = 0; l_I < l_LootCount; ++l_I)
    {
        uint64 l_LootGUID = l_Objects[l_I];
        uint8 l_SlotID = l_LootListIDs[l_I];
        Loot* l_Loot = NULL;

        if (IS_CRE_OR_VEH_GUID(GetPlayer()->GetLootGUID()))
        {
            Creature* l_Creature = GetPlayer()->GetMap()->GetCreature(sObjectMgr->GetCreatureGUIDByLootViewGUID(l_LootGUID));
            if (!l_Creature)
                return;

            uint64 l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForCreature(l_Creature);
            if (!l_LootContainerGuid)
                return;

            l_Loot = &l_Creature->m_LootContainers[l_LootContainerGuid];

            if (l_Loot->isLinkedLoot(l_SlotID))
            {
                LinkedLootInfo l_LinkedLootInfo = l_Loot->getLinkedLoot(l_SlotID);
                l_Creature = GetPlayer()->GetCreature(*GetPlayer(), l_LinkedLootInfo.creatureGUID);

                if (!l_Creature)
                    return;

                l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForCreature(l_Creature);
                if (!l_LootContainerGuid)
                    return;

                l_Loot   = &l_Creature->m_LootContainers[l_LootContainerGuid];
                l_SlotID = l_LinkedLootInfo.slot;
            }

        }
        else if (IS_GAMEOBJECT_GUID(GetPlayer()->GetLootGUID()))
        {
            GameObject* l_GameObject = GetPlayer()->GetMap()->GetGameObject(GetPlayer()->GetLootGUID());
            if (!l_GameObject)
                return;

            uint64 l_LootContainerGuid = GetPlayer()->GetLootContainerGuidForGameObject(l_GameObject);
            if (!l_LootContainerGuid)
                return;

            l_Loot = &l_GameObject->m_LootContainers[l_LootContainerGuid];
        }

        if (!l_Loot)
            return;

        if (l_SlotID >= l_Loot->Items.size() + l_Loot->QuestItems.size())
        {
            sLog->outDebug(LOG_FILTER_LOOT, "MasterLootItem: Player %s might be using a hack! (slot %d, size %lu)", GetPlayer()->GetName(), l_SlotID, (unsigned long)l_Loot->Items.size());
            return;
        }

        LootItem& l_Item = l_SlotID >= l_Loot->Items.size() ? l_Loot->QuestItems[l_SlotID - l_Loot->Items.size()] : l_Loot->Items[l_SlotID];

        ItemPosCountVec l_Destination;
        InventoryResult l_Message = l_Target->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, l_Item.itemid, l_Item.count);

        if (l_Item.follow_loot_rules && !l_Item.AllowedForPlayer(l_Target))
            l_Message = EQUIP_ERR_CANT_EQUIP_EVER;

        if (l_Message != EQUIP_ERR_OK)
        {
            l_Target->SendEquipError(l_Message, NULL, NULL, l_Item.itemid);
            /// Send duplicate of error massage to master looter
            m_Player->SendEquipError(l_Message, NULL, NULL, l_Item.itemid);
            return;
        }

        /// List of players allowed to receive this item in trade
        AllowedLooterSet l_Looters = l_Item.GetAllowedLooters();

        std::vector<uint32> l_Bonuses = l_Item.itemBonuses;

        /// Not move item from loot to target inventory
        Item* l_NewItem = l_Target->StoreNewItem(l_Destination, l_Item.itemid, true, l_Item.randomPropertyId, l_Looters, l_Bonuses);

        if (l_NewItem != nullptr)
        {
            l_Target->SendNewItem(l_NewItem, uint32(l_Item.count), false, false, true);
            l_Target->UpdateCriteria(CRITERIA_TYPE_LOOT_ITEM, l_Item.itemid, l_Item.count);
            l_Target->UpdateCriteria(CRITERIA_TYPE_LOOT_TYPE, l_Item.itemid, l_Item.count, l_Loot->Type, l_Target);
            l_Target->UpdateCriteria(CRITERIA_TYPE_LOOT_EPIC_ITEM, l_Item.itemid, l_Item.count);

            /// Mark as looted
            l_Item.count = 0;
            l_Item.is_looted = true;

            l_Loot->NotifyItemRemoved(l_SlotID);
            --l_Loot->UnlootedCount;
        }
    }
}
