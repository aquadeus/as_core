////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "CharacterDatabase.h"

void CharacterDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_CHARACTERDATABASE_STATEMENTS);
    ///                               0           1            2                3                4           5            6          7          8               9                    10          11             12             13            14                    15               16                  17                18             19          20            21            22            23           24
#define SelectItemInstanceContent "ii.guid, ii.itemEntry, ii.creatorGuid, ii.giftCreatorGuid, ii.count, ii.duration, ii.charges, ii.flags, ii.enchantments, ii.randomPropertyId, ii.bonuses, ii.upgradeId, challengeMapID, challengeLevel, challengeAffix, challengeAffix1, challengeAffix2, challengeKeyIsCharded, artifactTier, ii.durability, ii.playedTime, ii.text, ii.custom_flags, ii.BagFlags, ii.AcquiredLevel," \
            "iit.itemModifiedAppearanceAllSpecs, iit.itemModifiedAppearanceSpec1, iit.itemModifiedAppearanceSpec2, iit.itemModifiedAppearanceSpec3, iit.itemModifiedAppearanceSpec4, " \
            "iit.spellItemEnchantmentAllSpecs, iit.spellItemEnchantmentSpec1, iit.spellItemEnchantmentSpec2, iit.spellItemEnchantmentSpec3, iit.spellItemEnchantmentSpec4," \
            "ig.gemItemId1, ig.gemBonuses1, ig.gemContext1, ig.gemScalingLevel1, ig.gemItemId2, ig.gemBonuses2, ig.gemContext2, ig.gemScalingLevel2, ig.gemItemId3, ig.gemBonuses3, ig.gemContext3, ig.gemScalingLevel3, irt.data"

    PREPARE_STATEMENT(CHAR_DEL_QUEST_POOL_SAVE, "DELETE FROM pool_quest_save WHERE pool_id = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_QUEST_POOL_SAVE, "INSERT INTO pool_quest_save (pool_id, quest_id) VALUES (?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_NONEXISTENT_GUILD_BANK_ITEM, "DELETE FROM guild_bank_item WHERE guildid = ? AND TabId = ? AND SlotId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_EXPIRED_BANS, "UPDATE character_banned SET active = 0 WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate <> bandate", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_GUID_BY_NAME, "SELECT guid FROM characters WHERE name = ? and BINARY name = ?", CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_CHECK_NAME, "SELECT 1 FROM characters WHERE name = ? AND BINARY name = ?", CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_CHECK_GUID, "SELECT 1 FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_SUM_CHARS, "SELECT COUNT(guid) FROM characters WHERE account = ?", CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_CREATE_INFO, "SELECT level, race, class FROM characters WHERE account = ? LIMIT 0, ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHARACTER_BAN, "INSERT INTO character_banned VALUES (?, UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+?, ?, ?, 1)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_CHARACTER_BAN, "UPDATE character_banned SET active = 0 WHERE guid = ? AND active != 0", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_CHARACTER_BAN, "DELETE cb FROM character_banned cb INNER JOIN characters c ON c.guid = cb.guid WHERE c.account = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_BANINFO, "SELECT FROM_UNIXTIME(bandate), unbandate-bandate, active, unbandate, banreason, bannedby FROM character_banned WHERE guid = ? ORDER BY bandate ASC", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_GUID_BY_NAME_FILTER, "SELECT guid, name FROM characters WHERE name LIKE CONCAT('%%', ?, '%%')", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_BANINFO_LIST, "SELECT bandate, unbandate, bannedby, banreason FROM character_banned WHERE guid = ? ORDER BY unbandate", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_BANNED_NAME, "SELECT characters.name FROM characters, character_banned WHERE character_banned.guid = ? AND character_banned.guid = characters.guid", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_MAIL_LIST_COUNT, "SELECT COUNT(id) FROM mail WHERE receiver = ? ", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_MAIL_LIST_INFO, "SELECT id, sender, (SELECT name FROM characters WHERE guid = sender) AS sendername, receiver, (SELECT name FROM characters WHERE guid = receiver) AS receivername,"
                     "SUBJECT, deliver_time, expire_time, money, has_items FROM mail WHERE receiver = ? ", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_MAIL_LIST_ITEMS, "SELECT itemEntry,count FROM item_instance WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_ENUM, "SELECT c.guid, c.name, c.race, c.class, c.gender, c.playerBytes, c.playerBytes2, c.level, c.zone, c.map, c.position_x, c.position_y, c.position_z, gm.guildid, c.playerFlags, c.at_login, cp.entry, cp.modelid, cp.level, c.equipmentCache, cb.guid, c.slot, ch.mapId, ch.zoneId, ch.posX, ch.posY, ch.posZ FROM characters AS c LEFT JOIN character_pet AS cp ON c.guid = cp.owner AND cp.slot = (CASE c.class WHEN 9 THEN 254 WHEN 8 THEN 254 WHEN 6 THEN 254 WHEN 7 THEN 101 ELSE c.currentpetslot END) LEFT JOIN guild_member AS gm ON c.guid = gm.guid LEFT JOIN character_banned AS cb ON c.guid = cb.guid AND cb.active = 1 LEFT JOIN character_homebind AS ch ON c.guid = ch.guid WHERE c.account = ? AND c.end_sales < UNIX_TIMESTAMP()", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_ENUM_DECLINED_NAME, "SELECT c.guid, c.name, c.race, c.class, c.gender, c.playerBytes, c.playerBytes2, c.level, c.zone, c.map, c.position_x, c.position_y, c.position_z, gm.guildid, c.playerFlags, c.at_login, cp.entry, cp.modelid, cp.level, c.equipmentCache, cb.guid, c.slot, ch.mapId, ch.zoneId, ch.posX, ch.posY, ch.posZ, cd.genitive FROM characters AS c LEFT JOIN character_pet AS cp ON c.guid = cp.owner AND cp.slot = (CASE c.class WHEN 9 THEN 254 WHEN 8 THEN 254 WHEN 7 THEN 101 ELSE c.currentpetslot END) LEFT JOIN character_declinedname AS cd ON c.guid = cd.guid LEFT JOIN guild_member AS gm ON c.guid = gm.guid LEFT JOIN character_banned AS cb ON c.guid = cb.guid AND cb.active = 1 LEFT JOIN character_homebind AS ch ON c.guid = ch.guid WHERE c.account = ? AND c.end_sales < UNIX_TIMESTAMP()", CONNECTION_ASYNC);

    // Pet
    PREPARE_STATEMENT(CHAR_SEL_PET_SLOTS, "SELECT owner, slot, id FROM character_pet WHERE owner = ?  AND slot >= ? AND slot <= ? ORDER BY slot", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_PET_SLOTS_DETAIL, "SELECT owner, slot, id, entry, level, name FROM character_pet WHERE owner = ? AND slot >= ? AND slot <= ? ORDER BY slot", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_PET_SLOT_BY_ID, "SELECT slot, entry, id FROM character_pet WHERE owner = ? AND id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UDP_CHAR_PET_SLOT_BY_SLOT, "UPDATE character_pet SET slot = ? WHERE owner = ? AND slot = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_PET_BY_SLOT, "DELETE FROM character_pet WHERE owner = ? AND (slot = ? OR slot > ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_PET_BY_ID, "DELETE FROM character_pet WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_INVALID_PET_SPELL, "DELETE FROM pet_spell WHERE spell = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_FREE_NAME, "SELECT guid, name FROM characters WHERE guid = ? AND account = ? AND (at_login & ?) = ? AND NOT EXISTS (SELECT NULL FROM characters WHERE name = ? AND BINARY name = ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GUID_RACE_ACC_BY_NAME, "SELECT guid, race, account FROM characters WHERE name = ? AND BINARY name = ?", CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_RACE, "SELECT race FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_LEVEL, "SELECT level FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_ZONE, "SELECT zone FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_NAME_DATA, "SELECT race, class, gender, level, at_login FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_POSITION_XYZ, "SELECT map, position_x, position_y, position_z FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_POSITION, "SELECT position_x, position_y, position_z, orientation, map, taxi_path FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_DAILY, "DELETE FROM character_queststatus_daily", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_WEEKLY, "DELETE FROM character_queststatus_weekly", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_MONTHLY, "DELETE FROM character_queststatus_monthly", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_SEASONAL, "DELETE FROM character_queststatus_seasonal WHERE event = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_DAILY_CHAR, "DELETE FROM character_queststatus_daily WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_WEEKLY_CHAR, "DELETE FROM character_queststatus_weekly WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_MONTHLY_CHAR, "DELETE FROM character_queststatus_monthly WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_SEASONAL_CHAR, "DELETE FROM character_queststatus_seasonal WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_QUEST_STATUS_BY_QUEST_ID, "DELETE FROM `character_queststatus` WHERE `quest` = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PVP_INFO, "DELETE FROM character_pvp_info WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PVP_INFO, "INSERT INTO character_pvp_info (guid, honor, honor_level, prestige) VALUE (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_PVP_INFO, "SELECT honor, honor_level, prestige FROM character_pvp_info WHERE guid = ?", CONNECTION_ASYNC);

    // Start LoginQueryHolder content
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER, "SELECT guid, account, name, race, class, gender, level, xp, money, playerBytes, playerBytes2, playerFlags, "
    "position_x, position_y, position_z, map, orientation, taximask, cinematic, totaltime, leveltime, rest_bonus, logout_time, is_logout_resting, resettalents_cost, "
    "resettalents_time, UNUSED_OLD_talentTree, trans_x, trans_y, trans_z, trans_o, transguid, extra_flags, stable_slots, at_login, zone, online, death_expire_time, taxi_path, DungeonDifficulty, "
    "totalKills, todayKills, yesterdayKills, chosenTitle, watchedFaction, "
    "health, power1, power2, power3, power4, power5, power6, instance_id, primarySpecialization, activespec, lootSpecID, UNUSED_OLD_specialization2, exploredZones, equipmentCache, knownTitles, actionBars, currentpetslot, grantableLevels, playerFlagsEx, RaidDifficulty, LegacyRaidDifficuly, lastbattlepet, xprate, end_sales, BackPackAutoSortDisabled, playerBytes3 FROM characters WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_GROUP_MEMBER, "SELECT guid FROM group_member WHERE memberGuid = ?", CONNECTION_BOTH)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_INSTANCE, "SELECT id, permanent, map, difficulty, extendState, resettime FROM character_instance LEFT JOIN instance ON instance = id WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_AURAS, "SELECT caster_guid, slot, spell, effect_mask, recalculate_mask, stackcount, maxduration, remaintime, remaincharges, castItemLevel FROM character_aura WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_AURAS_EFFECTS, "SELECT slot, effect, baseamount, amount FROM character_aura_effect WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_SPELL, "SELECT spell, active, disabled, IsMountFavorite FROM character_spell WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_QUESTSTATUS, "SELECT quest, status, explored, timer FROM character_queststatus WHERE guid = ? AND status <> 0", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_QUEST_OBJECTIVE_STATUS, "SELECT `objectiveId`, `amount` FROM `character_queststatus_objective` WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_DAILYQUESTSTATUS, "SELECT quest, time FROM character_queststatus_daily WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_WEEKLYQUESTSTATUS, "SELECT quest FROM character_queststatus_weekly WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_MONTHLYQUESTSTATUS, "SELECT quest FROM character_queststatus_monthly WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_SEASONALQUESTSTATUS, "SELECT quest, event FROM character_queststatus_seasonal WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_CHARACTER_DAILYQUESTSTATUS, "INSERT INTO character_queststatus_daily (guid, quest, time) VALUES (?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_CHARACTER_WEEKLYQUESTSTATUS, "INSERT INTO character_queststatus_weekly (guid, quest) VALUES (?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_CHARACTER_MONTHLYQUESTSTATUS, "INSERT INTO character_queststatus_monthly (guid, quest) VALUES (?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_CHARACTER_SEASONALQUESTSTATUS, "INSERT INTO character_queststatus_seasonal (guid, quest, event) VALUES (?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_REPUTATION, "SELECT faction, standing, flags FROM character_reputation WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_INVENTORY, "SELECT " SelectItemInstanceContent ", bag, slot FROM character_inventory ci JOIN item_instance ii ON ci.item = ii.guid LEFT JOIN item_instance_gems ig ON ii.guid = ig.itemGuid LEFT JOIN item_instance_transmog iit ON ii.guid = iit.itemGuid LEFT JOIN item_relic_talent irt ON ii.guid = irt.guid WHERE ci.guid = ? ORDER BY (ii.flags & 0x80000) ASC, bag ASC, slot ASC", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_ITEMS_WITHOUT_INVENTORY, "SELECT " SelectItemInstanceContent " FROM item_instance ii LEFT JOIN item_instance_gems ig ON ii.guid = ig.itemGuid LEFT JOIN item_instance_transmog iit ON ii.guid = iit.itemGuid LEFT JOIN item_relic_talent irt ON ii.guid = irt.guid WHERE ii.owner_guid = ? AND ii.guid NOT IN(SELECT item FROM character_inventory WHERE guid = ?) AND ii.guid NOT IN (SELECT itemguid FROM auctionhouse where itemowner = ?) AND ii.guid NOT IN (SELECT mail_items.item_guid FROM mail, mail_items WHERE mail.`id` = mail_items.`mail_id` AND (mail.receiver = ? OR mail.`sender` = ?))", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_ACTIONS, "SELECT a.button, a.action, a.type FROM character_action as a, characters as c WHERE a.guid = c.guid AND a.spec = c.activespec AND a.guid = ? ORDER BY button", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_MAILCOUNT, "SELECT COUNT(id) FROM mail WHERE receiver = ? AND (checked & 1) = 0 AND deliver_time <= ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_MAILDATE, "SELECT MIN(deliver_time) FROM mail WHERE receiver = ? AND (checked & 1) = 0", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_MAIL_COUNT, "SELECT COUNT(*) FROM mail WHERE receiver = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_SOCIALLIST, "SELECT friend, flags, note FROM account_social JOIN characters ON characters.guid = account_social.friend WHERE account_social.account_id = ? AND account != 0", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_HOMEBIND, "SELECT mapId, zoneId, posX, posY, posZ FROM character_homebind WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_SPELLCOOLDOWNS, "SELECT spell, item, time FROM character_spell_cooldown WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_DECLINEDNAMES, "SELECT genitive, dative, accusative, instrumental, prepositional FROM character_declinedname WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_GUILD, "SELECT g.guildid, g.name, g.leaderguid, g.EmblemStyle, g.EmblemColor, g.BorderStyle, g.BorderColor, g.BackgroundColor, g.info, g.motd, g.createdate, g.BankMoney, COUNT(gbt.guildid) FROM guild g LEFT JOIN guild_bank_tab gbt ON g.guildid = gbt.guildid GROUP BY g.guildid ORDER BY g.guildid ASC", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_GUILD_MEMBER, "SELECT guildid, rank FROM guild_member WHERE guid = ?", CONNECTION_BOTH)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_EQUIPMENTSETS, "SELECT setguid, setindex, name, iconname, ignore_mask, item0, item1, item2, item3, item4, item5, item6, item7, item8, "
    "item9, item10, item11, item12, item13, item14, item15, item16, item17, item18, AssignedSpecIndex FROM character_equipmentsets WHERE guid = ? ORDER BY setindex", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_BRACKET_DATA, "SELECT bracket, rating, bestRatingOfWeek, bestRatingOfPrevWeek, bestRatingOfSeason, matchMakerRating, dayGames, dayWins, weekGames, weekWins, prevWeekWins, prevWeekGames, seasonGames, seasonWins FROM character_bracket_data WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_BGDATA, "SELECT instanceId, team, joinX, joinY, joinZ, joinO, joinMapId, taxiStart, taxiEnd, mountSpell, lastActiveSpec, bgTypeId, isRandom FROM character_battleground_data WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_TALENTS, "SELECT spell, spec FROM character_talent WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_SKILLS, "SELECT skill, value, max FROM character_skills WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_BANNED, "SELECT guid FROM character_banned WHERE guid = ? AND active = 1",  CONNECTION_BOTH)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_BANNED_DURATION, "SELECT unbandate FROM character_banned WHERE guid = ? AND active = 1",  CONNECTION_BOTH)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_QUESTSTATUSREW, "SELECT quest FROM character_queststatus_rewarded WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_QUESTSTATUSREW_COUNT, "SELECT COUNT(*) FROM character_queststatus_rewarded WHERE quest = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_ACCOUNT_INSTANCELOCKTIMES, "SELECT instanceId, releaseTime FROM account_instance_times WHERE accountId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_NOMI_RECIPES, "SELECT recipe_item_id FROM character_nomi_recipes WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_PLAYERCHOICE_RESPONSE_REWARDED, "SELECT ChoiceId, ResponseId FROM character_playerchoice_response_rewarded WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_PLAYERCHOICE_RESPONSE_REWARDED, "INSERT INTO character_playerchoice_response_rewarded (Guid, ChoiceId, ResponseId) VALUES (?, ?, ?)", CONNECTION_ASYNC)
    // End LoginQueryHolder content

    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_ACTIONS_SPEC, "SELECT button, action, type FROM character_action WHERE guid = ? AND spec = ? ORDER BY button", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_MAILITEMS, "SELECT " SelectItemInstanceContent ", owner_guid FROM mail_items mi JOIN item_instance ii ON mi.item_guid = ii.guid LEFT JOIN item_instance_gems ig ON ii.guid = ig.itemGuid LEFT JOIN item_instance_transmog iit ON ii.guid = iit.itemGuid LEFT JOIN item_relic_talent irt ON ii.guid = irt.guid  WHERE mail_id = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_MAILITEMS, "SELECT " SelectItemInstanceContent ", owner_guid, mail_id FROM mail_items mi JOIN item_instance ii ON mi.item_guid = ii.guid LEFT JOIN item_instance_gems ig ON ii.guid = ig.itemGuid LEFT JOIN item_instance_transmog iit ON ii.guid = iit.itemGuid LEFT JOIN item_relic_talent irt ON ii.guid = irt.guid WHERE receiver = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_AUCTION_ITEMS, "SELECT " SelectItemInstanceContent " FROM auctionhouse ah JOIN item_instance ii ON ah.itemguid = ii.guid  LEFT JOIN item_instance_gems ig ON ii.guid = ig.itemGuid LEFT JOIN item_instance_transmog iit ON ii.guid = iit.itemGuid LEFT JOIN item_relic_talent irt ON ii.guid = irt.guid ", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_AUCTIONS, "SELECT id, auctioneerguid, itemguid, itemEntry, count, itemowner, buyoutprice, time, buyguid, lastbid, startbid, deposit FROM auctionhouse ah INNER JOIN item_instance ii ON ii.guid = ah.itemguid", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_INS_AUCTION, "INSERT INTO auctionhouse (id, auctioneerguid, itemguid, itemowner, buyoutprice, time, buyguid, lastbid, startbid, deposit) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_AUCTION, "DELETE FROM auctionhouse WHERE id = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_AUCTION_BY_TIME, "SELECT id FROM auctionhouse WHERE time <= ? ORDER BY TIME ASC", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_UPD_AUCTION_BID, "UPDATE auctionhouse SET buyguid = ?, lastbid = ? WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_MAIL, "INSERT INTO mail(id, messageType, stationery, mailTemplateId, sender, receiver, subject, body, has_items, expire_time, deliver_time, money, cod, checked) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_MAIL_BY_ID, "DELETE FROM mail WHERE id = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_MAIL_ITEM, "INSERT INTO mail_items(mail_id, item_guid, receiver) VALUES (?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_MAIL_ITEM, "DELETE FROM mail_items WHERE item_guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_INVALID_MAIL_ITEM, "DELETE FROM mail_items WHERE item_guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_EMPTY_EXPIRED_MAIL, "DELETE FROM mail WHERE expire_time < ? AND has_items = 0 AND body = ''", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_EXPIRED_MAIL, "SELECT id, messageType, sender, receiver, has_items, expire_time, cod, checked, mailTemplateId FROM mail WHERE expire_time < ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_EXPIRED_MAIL_ITEMS, "SELECT item_guid, itemEntry, mail_id FROM mail_items mi INNER JOIN item_instance ii ON ii.guid = mi.item_guid LEFT JOIN mail mm ON mi.mail_id = mm.id WHERE mm.id IS NOT NULL AND mm.expire_time < ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_UPD_MAIL_RETURNED, "UPDATE mail SET sender = ?, receiver = ?, expire_time = ?, deliver_time = ?, cod = 0, checked = ? WHERE id = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_MAIL_ITEM_RECEIVER, "UPDATE mail_items SET receiver = ? WHERE item_guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_ITEM_OWNER, "UPDATE item_instance SET owner_guid = ? WHERE guid = ?", CONNECTION_ASYNC)

    PREPARE_STATEMENT(CHAR_SEL_ITEM_REFUNDS, "SELECT player_guid, paidMoney, paidExtendedCost FROM item_refund_instance WHERE item_guid = ? AND player_guid = ? LIMIT 1", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_ITEM_BOP_TRADE, "SELECT allowedPlayers FROM item_soulbound_trade_data WHERE itemGuid = ? LIMIT 1", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_DEL_ITEM_BOP_TRADE, "DELETE FROM item_soulbound_trade_data WHERE itemGuid = ? LIMIT 1", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_ITEM_BOP_TRADE, "INSERT INTO item_soulbound_trade_data VALUES (?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_REP_INVENTORY_ITEM, "REPLACE INTO character_inventory (guid, bag, slot, item) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_REP_ITEM_INSTANCE, "REPLACE INTO item_instance (itemEntry, owner_guid, creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, bonuses, upgradeId, challengeMapID, challengeLevel, challengeAffix, challengeAffix1, challengeAffix2, challengeKeyIsCharded, artifactTier, durability, playedTime, text, custom_flags, BagFlags, AcquiredLevel, guid) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_ITEM_INSTANCE, "UPDATE item_instance SET itemEntry = ?, owner_guid = ?, creatorGuid = ?, giftCreatorGuid = ?, count = ?, duration = ?, charges = ?, flags = ?, enchantments = ?, randomPropertyId = ?, bonuses = ?, upgradeId = ?,  challengeMapID = ?, challengeLevel = ?, challengeAffix = ?, challengeAffix1 = ?, challengeAffix2 = ?, challengeKeyIsCharded = ?, artifactTier = ?, durability = ?, playedTime = ?, text = ?, custom_flags = ?, BagFlags = ?, AcquiredLevel = ? WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_ITEM_INSTANCE_BONUSES, "UPDATE item_instance SET bonuses = ? WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_ITEM_INSTANCE_ON_LOAD, "UPDATE item_instance SET duration = ?, flags = ?, durability = ? WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_ITEM_INSTANCE, "DELETE FROM item_instance WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_ITEM_INSTANCE_TRANSMOG, "INSERT INTO item_instance_transmog (itemGuid, itemModifiedAppearanceAllSpecs, itemModifiedAppearanceSpec1, itemModifiedAppearanceSpec2, itemModifiedAppearanceSpec3, itemModifiedAppearanceSpec4, "
        "spellItemEnchantmentAllSpecs, spellItemEnchantmentSpec1, spellItemEnchantmentSpec2, spellItemEnchantmentSpec3, spellItemEnchantmentSpec4) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_ITEM_INSTANCE_TRANSMOG, "DELETE FROM item_instance_transmog WHERE itemGuid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_ITEM_INSTANCE_TRANSMOG_BY_OWNER, "DELETE iit FROM item_instance_transmog iit LEFT JOIN item_instance ii ON iit.itemGuid = ii.guid WHERE ii.owner_guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_ITEM_INSTANCE_GEMS, "INSERT INTO item_instance_gems (itemGuid, gemItemId1, gemBonuses1, gemContext1, gemScalingLevel1, gemItemId2, gemBonuses2, gemContext2, gemScalingLevel2, gemItemId3, gemBonuses3, gemContext3, gemScalingLevel3) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_ITEM_INSTANCE_GEMS, "DELETE FROM item_instance_gems WHERE itemGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_ITEM_INSTANCE_GEMS_BY_OWNER, "DELETE iig FROM item_instance_gems iig LEFT JOIN item_instance ii ON iig.itemGuid = ii.guid WHERE ii.owner_guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GIFT_OWNER, "UPDATE character_gifts SET guid = ? WHERE item_guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GIFT, "DELETE FROM character_gifts WHERE item_guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_GIFT_BY_ITEM, "SELECT entry, flags FROM character_gifts WHERE item_guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_ACCOUNT_BY_NAME, "SELECT account FROM characters WHERE name = ? AND BINARY name = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_ACCOUNT_BY_GUID, "SELECT account FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_ACCOUNT_NAME_BY_GUID, "SELECT account, name, at_login FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_DEL_ACCOUNT_INSTANCE_LOCK_TIMES, "DELETE FROM account_instance_times WHERE accountId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_ACCOUNT_INSTANCE_LOCK_TIMES, "INSERT INTO account_instance_times (accountId, instanceId, releaseTime) VALUES (?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_NAME_CLASS, "SELECT name, class FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_NAME, "SELECT name FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_COUNT, "SELECT account, COUNT(guid) FROM characters WHERE account = ? GROUP BY account", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_NAME, "UPDATE characters set name = ?, at_login = at_login & ~ ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_DECLINED_NAME, "DELETE FROM character_declinedname WHERE guid = ?", CONNECTION_ASYNC);

    // Guild handling
    // 0: uint32, 1: string, 2: uint32, 3: string, 4: string, 5: uint64, 6-10: uint32, 11: uint64
    PREPARE_STATEMENT(CHAR_INS_GUILD, "INSERT INTO guild (guildid, name, leaderguid, info, motd, createdate, EmblemStyle, EmblemColor, BorderStyle, BorderColor, BackgroundColor, BankMoney) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GUILD, "DELETE FROM guild WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32
    // 0: string, 1: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_NAME, "UPDATE guild SET name = ? WHERE guildid = ?", CONNECTION_ASYNC);
    // 0: uint32, 1: uint32, 2: uint8, 4: string, 5: string
    PREPARE_STATEMENT(CHAR_INS_GUILD_MEMBER, "INSERT INTO guild_member (guildid, guid, rank, pnote, offnote) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GUILD_MEMBER, "DELETE FROM guild_member WHERE guid = ?", CONNECTION_ASYNC) // 0: uint32
    PREPARE_STATEMENT(CHAR_DEL_GUILD_MEMBERS, "DELETE FROM guild_member WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBERS_RANK, "UPDATE guild_member SET rank = ? WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC) // 0: uint32
    // 0: uint32, 1: uint8, 3: string, 4: uint32
    PREPARE_STATEMENT(CHAR_INS_GUILD_RANK, "INSERT INTO guild_rank (guildid, rid, rname, rights) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GUILD_RANKS, "DELETE FROM guild_rank WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32
    PREPARE_STATEMENT(CHAR_DEL_GUILD_RANK, "DELETE FROM guild_rank WHERE guildid = ? AND rid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_ID, "UPDATE guild_rank SET rid = ? WHERE guildid = ? AND rid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8
    PREPARE_STATEMENT(CHAR_INS_GUILD_BANK_TAB, "INSERT INTO guild_bank_tab (guildid, TabId) VALUES (?, ?)", CONNECTION_ASYNC) // 0: uint32, 1: uint8
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_TAB, "DELETE FROM guild_bank_tab WHERE guildid = ? AND TabId = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_TABS, "DELETE FROM guild_bank_tab WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32
    // 0: uint32, 1: uint8, 2: uint8, 3: uint32, 4: uint32
    PREPARE_STATEMENT(CHAR_INS_GUILD_BANK_ITEM, "INSERT INTO guild_bank_item (guildid, TabId, SlotId, item_guid) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_ITEM, "DELETE FROM guild_bank_item WHERE guildid = ? AND TabId = ? AND SlotId = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8, 2: uint8
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_ITEMS, "DELETE FROM guild_bank_item WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32
    PREPARE_STATEMENT(CHAR_SEL_GUILD_BANK_ITEMS, "SELECT " SelectItemInstanceContent " , guildid, TabId, SlotId FROM guild_bank_item gbi INNER JOIN item_instance ii ON gbi.guildid = ? AND gbi.item_guid = ii.guid LEFT JOIN item_instance_gems ig ON ii.guid = ig.itemGuid LEFT JOIN item_instance_transmog iit ON ii.guid = iit.itemGuid LEFT JOIN item_relic_talent irt ON ii.guid = irt.guid", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_INS_GUILD_BANK_RIGHT_DEFAULT, "INSERT INTO guild_bank_right (guildid, TabId, rid) VALUES (?, ?, ?)", CONNECTION_ASYNC) // 0: uint32, 1: uint8, 2: uint8
    // 0: uint32, 1: uint8, 2: uint8, 3: uint8, 4: uint32
    PREPARE_STATEMENT(CHAR_INS_GUILD_BANK_RIGHT, "INSERT INTO guild_bank_right (guildid, TabId, rid, gbright, SlotPerDay) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_RIGHT, "DELETE FROM guild_bank_right WHERE guildid = ? AND TabId = ? AND rid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8, 2: uint8
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_RIGHTS, "DELETE FROM guild_bank_right WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_RIGHTS_FOR_RANK, "DELETE FROM guild_bank_right WHERE guildid = ? AND rid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8
    PREPARE_STATEMENT(CHAR_UPD_GUILD_BANK_RIGHTS_ID, "UPDATE guild_bank_right SET rid = ? WHERE guildid = ? AND rid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8
    // 0-1: uint32, 2-3: uint8, 4-5: uint32, 6: uint16, 7: uint8, 8: uint64
    PREPARE_STATEMENT(CHAR_INS_GUILD_BANK_EVENTLOG, "INSERT INTO guild_bank_eventlog (guildid, LogGuid, TabId, EventType, PlayerGuid, ItemOrMoney, ItemStackCount, DestTabId, TimeStamp) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_EVENTLOG, "DELETE FROM guild_bank_eventlog WHERE guildid = ? AND LogGuid = ? AND TabId = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint32, 2: uint8
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_EVENTLOGS, "DELETE FROM guild_bank_eventlog WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32
    // 0-1: uint32, 2: uint8, 3-4: uint32, 5: uint8, 6: uint64
    PREPARE_STATEMENT(CHAR_INS_GUILD_EVENTLOG, "INSERT INTO guild_eventlog (guildid, LogGuid, EventType, PlayerGuid1, PlayerGuid2, NewRank, TimeStamp) VALUES (?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GUILD_EVENTLOG, "DELETE FROM guild_eventlog WHERE guildid = ? AND LogGuid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint32
    PREPARE_STATEMENT(CHAR_DEL_GUILD_EVENTLOGS, "DELETE FROM guild_eventlog WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_PNOTE, "UPDATE guild_member SET pnote = ? WHERE guid = ?", CONNECTION_ASYNC) // 0: string, 1: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_OFFNOTE, "UPDATE guild_member SET offnote = ? WHERE guid = ?", CONNECTION_ASYNC) // 0: string, 1: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_RANK, "UPDATE guild_member SET rank = ? WHERE guid = ?", CONNECTION_ASYNC) // 0: uint8, 1: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MOTD, "UPDATE guild SET motd = ? WHERE guildid = ?", CONNECTION_ASYNC) // 0: string, 1: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_INFO, "UPDATE guild SET info = ? WHERE guildid = ?", CONNECTION_ASYNC) // 0: string, 1: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_LEADER, "UPDATE guild SET leaderguid = ? WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_NAME, "UPDATE guild_rank SET rname = ? WHERE rid = ? AND guildid = ?", CONNECTION_ASYNC) // 0: string, 1: uint8, 2: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_RIGHTS, "UPDATE guild_rank SET rights = ? WHERE rid = ? AND guildid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8, 2: uint32
    // 0-5: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_EMBLEM_INFO, "UPDATE guild SET EmblemStyle = ?, EmblemColor = ?, BorderStyle = ?, BorderColor = ?, BackgroundColor = ? WHERE guildid = ?", CONNECTION_ASYNC)
    // 0: string, 1: string, 2: uint32, 3: uint8
    PREPARE_STATEMENT(CHAR_UPD_GUILD_BANK_TAB_INFO, "UPDATE guild_bank_tab SET TabName = ?, TabIcon = ? WHERE guildid = ? AND TabId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_BANK_MONEY, "UPDATE guild SET BankMoney = ? WHERE guildid = ?", CONNECTION_ASYNC) // 0: uint64, 1: uint32
    // 0: uint8, 1: uint32, 2: uint8, 3: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_BANK_EVENTLOG_TAB, "UPDATE guild_bank_eventlog SET TabId = ? WHERE guildid = ? AND TabId = ? AND LogGuid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_MONEY, "UPDATE guild_member SET BankRemMoney = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint32, 2: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_MONEY, "UPDATE guild_member SET BankResetTimeMoney = ?, BankRemMoney = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint32, 2: uint32, 3: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_RESET_TIME, "UPDATE guild_member SET BankResetTimeMoney = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_MONEY, "UPDATE guild_rank SET BankMoneyPerDay = ? WHERE rid = ? AND guildid = ?", CONNECTION_ASYNC) // 0: uint32, 1: uint8, 2: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_BANK_TAB_TEXT, "UPDATE guild_bank_tab SET TabText = ? WHERE guildid = ? AND TabId = ?", CONNECTION_ASYNC) // 0: string, 1: uint32, 2: uint8
    // 0: uint32, 1: uint32, 2: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_SLOTS0, "UPDATE guild_member SET BankRemSlotsTab0 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_SLOTS1, "UPDATE guild_member SET BankRemSlotsTab1 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_SLOTS2, "UPDATE guild_member SET BankRemSlotsTab2 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_SLOTS3, "UPDATE guild_member SET BankRemSlotsTab3 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_SLOTS4, "UPDATE guild_member SET BankRemSlotsTab4 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_SLOTS5, "UPDATE guild_member SET BankRemSlotsTab5 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_SLOTS6, "UPDATE guild_member SET BankRemSlotsTab6 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_REM_SLOTS7, "UPDATE guild_member SET BankRemSlotsTab7 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    // 0: uint32, 1: uint32, 2: uint32, 3: uint32
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_REM_SLOTS0, "UPDATE guild_member SET BankResetTimeTab0 = ?, BankRemSlotsTab0 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_REM_SLOTS1, "UPDATE guild_member SET BankResetTimeTab1 = ?, BankRemSlotsTab1 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_REM_SLOTS2, "UPDATE guild_member SET BankResetTimeTab2 = ?, BankRemSlotsTab2 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_REM_SLOTS3, "UPDATE guild_member SET BankResetTimeTab3 = ?, BankRemSlotsTab3 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_REM_SLOTS4, "UPDATE guild_member SET BankResetTimeTab4 = ?, BankRemSlotsTab4 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_REM_SLOTS5, "UPDATE guild_member SET BankResetTimeTab5 = ?, BankRemSlotsTab5 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_REM_SLOTS6, "UPDATE guild_member SET BankResetTimeTab6 = ?, BankRemSlotsTab6 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_MEMBER_BANK_TIME_REM_SLOTS7, "UPDATE guild_member SET BankResetTimeTab7 = ?, BankRemSlotsTab7 = ? WHERE guildid = ? AND guid = ?", CONNECTION_ASYNC)
    // 0: uint32, 1: uint8
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_TIME0, "UPDATE guild_member SET BankResetTimeTab0 = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_TIME1, "UPDATE guild_member SET BankResetTimeTab1 = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_TIME2, "UPDATE guild_member SET BankResetTimeTab2 = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_TIME3, "UPDATE guild_member SET BankResetTimeTab3 = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_TIME4, "UPDATE guild_member SET BankResetTimeTab4 = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_TIME5, "UPDATE guild_member SET BankResetTimeTab5 = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_TIME6, "UPDATE guild_member SET BankResetTimeTab6 = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_GUILD_RANK_BANK_TIME7, "UPDATE guild_member SET BankResetTimeTab7 = 0 WHERE guildid = ? AND rank = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHAR_DATA_FOR_GUILD, "SELECT name, level, class, zone, account FROM characters WHERE guid = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_LOAD_GUILD_NEWS, "SELECT id, eventType, playerGuid, data, guild_news_log.flags, date, itemEntry, randomPropertyId, bonuses FROM guild_news_log LEFT JOIN item_instance ON (eventType = 3 OR eventType = 4 OR eventType = 5) AND data = item_instance.guid WHERE guild = ? ORDER BY id DESC LIMIT 0, 200  ", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SAVE_GUILD_NEWS, "INSERT INTO guild_news_log (guild, id, eventType, playerGuid, data, flags, date) VALUES (?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GUILD_BANK_EVENT, "SELECT guildid, TabId, LogGuid, EventType, PlayerGuid, ItemOrMoney, ItemStackCount, DestTabId, TimeStamp FROM guild_bank_eventlog WHERE guildid = ? ORDER BY TimeStamp DESC, LogGuid DESC", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GUILD_BANK_TAB, "SELECT guildid, TabId, TabName, TabIcon, TabText FROM guild_bank_tab WHERE guildid = ? ORDER BY TabId ASC", CONNECTION_ASYNC);


    /// Guild Challenges
    PREPARE_STATEMENT(CHAR_LOAD_GUILD_CHALLENGES, "SELECT GuildId, ChallengeType, ChallengeCount FROM guild_challenges", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_INIT_GUILD_CHALLENGES, "INSERT INTO guild_challenges VALUES (?, ?, 0) ON DUPLICATE KEY UPDATE GuildId=GuildId", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_COMPLETE_GUILD_CHALLENGE, "UPDATE guild_challenges SET ChallengeCount = ? WHERE GuildId = ? AND ChallengeType = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REMOVE_GUILD_CHALLENGES, "DELETE FROM guild_challenges WHERE GuildId = ?", CONNECTION_ASYNC);

    /// Archaeology
    PREPARE_STATEMENT(CHAR_SEL_CHAR_ARCHAEOLOGY, "SELECT counts, projects FROM character_archaeology WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_ARCHAEOLOGY_PROJECTS, "SELECT project, count, first_date FROM character_archaeology_projects WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_CHAR_ARCHAEOLOGY_SITES, "SELECT map, sites FROM character_archaeology_sites WHERE guid = ?", CONNECTION_ASYNC);

    /// Chat channel handling
    PREPARE_STATEMENT(CHAR_SEL_CHANNEL, "SELECT announce, ownership, password, bannedList FROM channels WHERE name = ? AND team = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_INS_CHANNEL, "INSERT INTO channels(name, team, lastUsed) VALUES (?, ?, UNIX_TIMESTAMP())", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_CHANNEL, "UPDATE channels SET announce = ?, ownership = ?, password = ?, bannedList = ?, lastUsed = UNIX_TIMESTAMP() WHERE name = ? AND team = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_CHANNEL_USAGE, "UPDATE channels SET lastUsed = UNIX_TIMESTAMP() WHERE name = ? AND team = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_CHANNEL_OWNERSHIP, "UPDATE channels SET ownership = ? WHERE name LIKE ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_OLD_CHANNELS, "DELETE FROM channels WHERE ownership = 1 AND lastUsed + ? < UNIX_TIMESTAMP()", CONNECTION_ASYNC)

    /// Equipment sets
    PREPARE_STATEMENT(CHAR_UPD_EQUIP_SET, "UPDATE character_equipmentsets SET name=?, iconname=?, ignore_mask=?, type=?, item0=?, item1=?, item2=?, item3=?, item4=?, item5=?, item6=?, item7=?, item8=?, item9=?, item10=?, item11=?, item12=?, item13=?, item14=?, item15=?, item16=?, item17=?, item18=?, AssignedSpecIndex=? WHERE guid=? AND setguid=? AND setindex=?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_EQUIP_SET, "INSERT INTO character_equipmentsets (guid, setguid, setindex, name, iconname, ignore_mask, type, item0, item1, item2, item3, item4, item5, item6, item7, item8, item9, item10, item11, item12, item13, item14, item15, item16, item17, item18, AssignedSpecIndex) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_EQUIP_SET, "DELETE FROM character_equipmentsets WHERE setguid=?", CONNECTION_ASYNC);

    /// Auras
    PREPARE_STATEMENT(CHAR_INS_AURA, "INSERT INTO character_aura (guid, slot, caster_guid, item_guid, spell, effect_mask, recalculate_mask, stackcount, maxduration, remaintime, remaincharges, castItemLevel) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)

    PREPARE_STATEMENT(CHAR_INS_AURA_EFFECT, "INSERT INTO character_aura_effect (guid, slot, effect, baseamount, amount) "
    "VALUES (?, ?, ?, ?, ?)",  CONNECTION_ASYNC)

    PREPARE_STATEMENT(CHAR_DEL_CUF_PROFILE, "DELETE FROM cuf_profile WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CUF_PROFILE, "INSERT INTO cuf_profile (guid, name, data) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CUF_PROFILE, "SELECT name, data FROM cuf_profile WHERE guid = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_UPD_PET_DATA_OWNER, "UPDATE character_pet SET slot = ? WHERE slot = ? AND owner = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_PET_DATA_OWNER_ID, "UPDATE character_pet SET slot = ? WHERE slot = ? AND owner = ? AND id <> ?", CONNECTION_ASYNC);

    /// Currency
    PREPARE_STATEMENT(CHAR_SEL_PLAYER_CURRENCY, "SELECT currency, week_count, total_count, season_total, flags, weekCap, needResetCap FROM character_currency WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_PLAYER_CURRENCY, "UPDATE character_currency SET week_count = ?, total_count = ?, season_total = ?, flags = ?, weekCap = ?, needResetCap = ? WHERE guid = ? AND currency = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_PLAYER_CURRENCY, "REPLACE INTO character_currency (guid, currency, week_count, total_count, season_total, flags, weekCap, needResetCap) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_PLAYER_CURRENCY_CAPS, "SELECT currency, sourceType, sourceId, value FROM character_currency_caps WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_PLAYER_CURRENCY_CAPS, "REPLACE INTO character_currency_caps (guid, currency, sourceType, sourceId, value) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);

    /// Archaeology
    PREPARE_STATEMENT(CHAR_DEL_PLAYER_ARCHAEOLOGY, "DELETE FROM character_archaeology WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PLAYER_ARCHAEOLOGY, "INSERT INTO character_archaeology  (guid, counts, projects) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PLAYER_ARCHAEOLOGY_SITES, "DELETE FROM character_archaeology_sites WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PLAYER_ARCHAEOLOGY_SITES, "INSERT INTO character_archaeology_sites  (guid, map, sites) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PLAYER_ARCHAEOLOGY_PROJECTS, "DELETE FROM character_archaeology_projects WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PLAYER_ARCHAEOLOGY_PROJECTS, "INSERT INTO character_archaeology_projects (guid, project, count, first_date) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);

    /// Account data
    PREPARE_STATEMENT(CHAR_SEL_ACCOUNT_DATA, "SELECT type, time, data FROM account_data WHERE accountId = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_REP_ACCOUNT_DATA, "REPLACE INTO account_data (accountId, type, time, data) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_ACCOUNT_DATA, "DELETE FROM account_data WHERE accountId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_PLAYER_ACCOUNT_DATA, "SELECT type, time, data FROM character_account_data WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_REP_PLAYER_ACCOUNT_DATA, "REPLACE INTO character_account_data(guid, type, time, data) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_PLAYER_ACCOUNT_DATA, "DELETE FROM character_account_data WHERE guid = ?", CONNECTION_ASYNC)

    /// Tutorials
    PREPARE_STATEMENT(CHAR_SEL_TUTORIALS, "SELECT tut0, tut1, tut2, tut3, tut4, tut5, tut6, tut7 FROM account_tutorial WHERE accountId = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_SEL_HAS_TUTORIALS, "SELECT 1 FROM account_tutorial WHERE accountId = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_INS_TUTORIALS, "REPLACE INTO account_tutorial(tut0, tut1, tut2, tut3, tut4, tut5, tut6, tut7, accountId) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_TUTORIALS, "UPDATE account_tutorial SET tut0 = ?, tut1 = ?, tut2 = ?, tut3 = ?, tut4 = ?, tut5 = ?, tut6 = ?, tut7 = ? WHERE accountId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_TUTORIALS, "DELETE FROM account_tutorial WHERE accountId = ?", CONNECTION_ASYNC)

    /// Instance saves
    PREPARE_STATEMENT(CHAR_INS_INSTANCE_SAVE, "INSERT INTO instance (id, map, resettime, difficulty, completedEncounters, data) VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_INSTANCE_DATA, "UPDATE instance SET completedEncounters=?, data=? WHERE id=?", CONNECTION_ASYNC)

    /// Game event saves
    PREPARE_STATEMENT(CHAR_DEL_GAME_EVENT_SAVE, "DELETE FROM game_event_save WHERE eventEntry = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_GAME_EVENT_SAVE, "INSERT INTO game_event_save (eventEntry, state, next_start) VALUES (?, ?, ?)", CONNECTION_ASYNC)

    /// Game event condition saves
    PREPARE_STATEMENT(CHAR_DEL_ALL_GAME_EVENT_CONDITION_SAVE, "DELETE FROM game_event_condition_save WHERE eventEntry = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GAME_EVENT_CONDITION_SAVE, "DELETE FROM game_event_condition_save WHERE eventEntry = ? AND condition_id = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_INS_GAME_EVENT_CONDITION_SAVE, "INSERT INTO game_event_condition_save (eventEntry, condition_id, done) VALUES (?, ?, ?)", CONNECTION_ASYNC)

    /// Petitions
    PREPARE_STATEMENT(CHAR_SEL_PETITION, "SELECT ownerguid, name, type FROM petition WHERE petitionguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PETITION_SIGNATURE, "SELECT playerguid FROM petition_sign WHERE petitionguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_DEL_ALL_PETITION_SIGNATURES, "DELETE FROM petition_sign WHERE playerguid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PETITION_SIGNATURE, "DELETE FROM petition_sign WHERE playerguid = ? AND type = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_PETITION_BY_OWNER, "SELECT petitionguid FROM petition WHERE ownerguid = ? AND type = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PETITION_TYPE, "SELECT type FROM petition WHERE petitionguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PETITION_SIGNATURES, "SELECT ownerguid, (SELECT COUNT(playerguid) FROM petition_sign WHERE petition_sign.petitionguid = ?) AS signs, type FROM petition WHERE petitionguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PETITION_SIG_BY_ACCOUNT, "SELECT playerguid FROM petition_sign WHERE player_account = ? AND petitionguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PETITION_OWNER_BY_GUID, "SELECT ownerguid FROM petition WHERE petitionguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PETITION_SIG_BY_GUID, "SELECT ownerguid, petitionguid FROM petition_sign WHERE playerguid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PETITION_SIG_BY_GUID_TYPE, "SELECT ownerguid, petitionguid FROM petition_sign WHERE playerguid = ? AND type = ?", CONNECTION_SYNCH);

    /// Character arena data
    PREPARE_STATEMENT(CHAR_INS_CHARACTER_BRACKET_DATA, "INSERT INTO character_bracket_data (guid, bracket, rating, bestRatingOfWeek, bestRatingOfPrevWeek, bestRatingOfSeason, matchMakerRating, dayGames, dayWins, weekGames, weekWins, prevWeekWins, prevWeekGames, seasonGames, seasonWins) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_CHARACTER_BRACKET_DATA, "DELETE FROM character_bracket_data WHERE guid = ?", CONNECTION_ASYNC)

    // Character battleground data
    PREPARE_STATEMENT(CHAR_INS_PLAYER_BGDATA, "INSERT INTO character_battleground_data (guid, instanceId, team, joinX, joinY, joinZ, joinO, joinMapId, taxiStart, taxiEnd, mountSpell, lastActiveSpec, bgTypeId, isRandom) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_PLAYER_BGDATA, "DELETE FROM character_battleground_data WHERE guid = ?", CONNECTION_ASYNC)

    /// Character homebind
    PREPARE_STATEMENT(CHAR_INS_PLAYER_HOMEBIND, "INSERT INTO character_homebind (guid, mapId, zoneId, posX, posY, posZ) VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_UPD_PLAYER_HOMEBIND, "UPDATE character_homebind SET mapId = ?, zoneId = ?, posX = ?, posY = ?, posZ = ? WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_PLAYER_HOMEBIND, "DELETE FROM character_homebind WHERE guid = ?", CONNECTION_ASYNC)

    /// Corpse
    PREPARE_STATEMENT(CHAR_SEL_CORPSES, "SELECT posX, posY, posZ, orientation, mapId, displayId, itemCache, bytes1, bytes2, flags, dynFlags, time, corpseType, instanceId, phaseMask, corpseGuid, guid FROM corpse WHERE corpseType <> 0", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_INS_CORPSE, "INSERT INTO corpse (corpseGuid, guid, posX, posY, posZ, orientation, mapId, displayId, itemCache, bytes1, bytes2, flags, dynFlags, time, corpseType, instanceId, phaseMask) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_CORPSE, "DELETE FROM corpse WHERE corpseGuid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_PLAYER_CORPSES, "DELETE FROM corpse WHERE guid = ? AND corpseType <> 0", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_OLD_CORPSES, "DELETE FROM corpse WHERE corpseType = 0 OR time < (UNIX_TIMESTAMP(NOW()) - ?)", CONNECTION_ASYNC)

    // Creature respawn
    PREPARE_STATEMENT(CHAR_SEL_CREATURE_RESPAWNS, "SELECT guid, respawnTime FROM creature_respawn WHERE mapId = ? AND instanceId = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_REP_CREATURE_RESPAWN, "REPLACE INTO creature_respawn (guid, respawnTime, mapId, instanceId) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_CREATURE_RESPAWN, "DELETE FROM creature_respawn WHERE guid = ? AND mapId = ? AND instanceId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_CREATURE_RESPAWN_BY_INSTANCE, "DELETE FROM creature_respawn WHERE mapId = ? AND instanceId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_MAX_CREATURE_RESPAWNS, "SELECT MAX(respawnTime), instanceId FROM creature_respawn WHERE instanceId > 0 GROUP BY instanceId", CONNECTION_SYNCH)

    /// Gameobject respawn
    PREPARE_STATEMENT(CHAR_SEL_GO_RESPAWNS, "SELECT guid, respawnTime FROM gameobject_respawn WHERE mapId = ? AND instanceId = ?", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_REP_GO_RESPAWN, "REPLACE INTO gameobject_respawn (guid, respawnTime, mapId, instanceId) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GO_RESPAWN, "DELETE FROM gameobject_respawn WHERE guid = ? AND mapId = ? AND instanceId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GO_RESPAWN_BY_INSTANCE, "DELETE FROM gameobject_respawn WHERE mapId = ? AND instanceId = ?", CONNECTION_ASYNC)

    /// GM Tickets
    PREPARE_STATEMENT(CHAR_SEL_GM_TICKETS, "SELECT ticketId, guid, name, message, createTime, mapId, posX, posY, posZ, lastModifiedTime, closedBy, assignedTo, comment, completed, escalated, viewed, haveTicket, response, locale FROM gm_tickets", CONNECTION_SYNCH)
    PREPARE_STATEMENT(CHAR_REP_GM_TICKET, "REPLACE INTO gm_tickets (ticketId, guid, name, message, createTime, mapId, posX, posY, posZ, lastModifiedTime, closedBy, assignedTo, comment, completed, escalated, viewed, haveTicket, response, locale) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_GM_TICKET, "DELETE FROM gm_tickets WHERE ticketId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_PLAYER_GM_TICKETS, "DELETE FROM gm_tickets WHERE guid = ?", CONNECTION_ASYNC)

    ///  For loading and deleting expired auctions at startup
    PREPARE_STATEMENT(CHAR_SEL_EXPIRED_AUCTIONS, "SELECT id, auctioneerguid, itemguid, itemEntry, count, itemowner, buyoutprice, time, buyguid, lastbid, startbid, deposit FROM auctionhouse ah INNER JOIN item_instance ii ON ii.guid = ah.itemguid WHERE ah.time <= ?", CONNECTION_SYNCH)

    /// LFG Data
    PREPARE_STATEMENT(CHAR_INS_LFG_DATA, "INSERT INTO lfg_data (guid, dungeon, state) VALUES (?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_LFG_DATA, "DELETE FROM lfg_data WHERE guid = ?", CONNECTION_ASYNC)

    /// Player saving
    PREPARE_STATEMENT(CHAR_INS_CHARACTER, "INSERT INTO characters (guid, account, name, race, class, gender, level, xp, money, playerBytes, playerBytes2, playerBytes3, playerFlags, playerFlagsEx, "
    "map, instance_id, DungeonDifficulty, RaidDifficulty, LegacyRaidDifficuly, position_x, position_y, position_z, orientation, "
    "taximask, cinematic, "
    "totaltime, leveltime, rest_bonus, logout_time, is_logout_resting, resettalents_cost, resettalents_time, primarySpecialization, "
    "extra_flags, stable_slots, at_login, zone, "
    "death_expire_time, taxi_path, totalKills, "
    "todayKills, yesterdayKills, chosenTitle, watchedFaction, health, power1, power2, power3, "
    "power4, power5, power6, latency, activespec, lootSpecID, exploredZones, equipmentCache, knownTitles, actionBars, currentpetslot, grantableLevels, lastbattlepet, xprate, ilvl, BackPackAutoSortDisabled) VALUES "
    "(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_UPD_CHARACTER, "UPDATE characters SET name=?,race=?,class=?,gender=?,level=?,xp=?,money=?,playerBytes=?,playerBytes2=?,playerBytes3=?,playerFlags=?,playerFlagsEx=?,"
    "map=?,instance_id=?,DungeonDifficulty=?, RaidDifficulty = ?, LegacyRaidDifficuly = ?, position_x=?,position_y=?,position_z=?,orientation=?,taximask=?,cinematic=?,totaltime=?,leveltime=?,rest_bonus=?,"
    "logout_time=?,is_logout_resting=?,resettalents_cost=?,resettalents_time=?,primarySpecialization=?,extra_flags=?,stable_slots=?,at_login=?,zone=?,death_expire_time=?,taxi_path=?,"
    "totalKills=?,todayKills=?,yesterdayKills=?,chosenTitle=?,"
    "watchedFaction=?,health=?,power1=?,power2=?,power3=?,power4=?,power5=?, power6=?, latency=?,activespec=?,lootSpecID=?,exploredZones=?,"
    "equipmentCache=?,knownTitles=?,actionBars=?,currentpetslot=?,grantableLevels=?,online=?, lastbattlepet = ?, xprate = ?, ilvl = ?, BackPackAutoSortDisabled = ? WHERE guid=?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_UPD_ADD_AT_LOGIN_FLAG, "UPDATE characters SET at_login = at_login | ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_REM_AT_LOGIN_FLAG, "UPDATE characters set at_login = at_login & ~ ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_ALL_AT_LOGIN_FLAGS, "UPDATE characters SET at_login = at_login | ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_BUG_REPORT, "INSERT INTO bugreport (type, content) VALUES(?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_PETITION_NAME, "UPDATE petition SET name = ? WHERE petitionguid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PETITION_SIGNATURE, "INSERT INTO petition_sign (ownerguid, petitionguid, playerguid, player_account) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_ACCOUNT_ONLINE, "UPDATE characters SET online = 0 WHERE account = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_GROUP, "INSERT INTO groups (guid, leaderGuid, lootMethod, looterGuid, lootThreshold, icon1, icon2, icon3, icon4, icon5, icon6, icon7, icon8, groupType, difficulty, raiddifficulty, legacyraiddifficulty) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_GROUP_MEMBER, "INSERT INTO group_member (guid, memberGuid, memberFlags, subgroup, roles, class, specId) VALUES(?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GROUP_MEMBER, "DELETE FROM group_member WHERE memberGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GROUP_INSTANCE_PERM_BINDING, "DELETE FROM group_instance WHERE guid = ? AND instance = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GROUP_LEADER, "UPDATE groups SET leaderGuid = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GROUP_TYPE, "UPDATE groups SET groupType = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GROUP_MEMBER_SUBGROUP, "UPDATE group_member SET subgroup = ? WHERE memberGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GROUP_MEMBER_FLAG, "UPDATE group_member SET memberFlags = ? WHERE memberGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GROUP_MEMBER_ROLE, "UPDATE group_member SET roles = ? WHERE memberGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GROUP_MEMBER_SPEC, "UPDATE group_member SET specId = ? WHERE memberGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GROUP_DIFFICULTY, "UPDATE groups SET difficulty = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GROUP_RAID_DIFFICULTY, "UPDATE groups SET raiddifficulty = ?, legacyraiddifficulty = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_ALL_GM_TICKETS, "TRUNCATE TABLE gm_tickets", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_INVALID_SPELL, "DELETE FROM character_talent WHERE spell = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_DELETE_INFO, "UPDATE characters SET deleteInfos_Name = name, deleteInfos_Account = account, deleteDate = UNIX_TIMESTAMP(), name = '', account = 0 WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UDP_RESTORE_DELETE_INFO, "UPDATE characters SET name = ?, account = ?, deleteDate = NULL, deleteInfos_Name = NULL, deleteInfos_Account = NULL WHERE deleteDate IS NOT NULL AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_ZONE, "UPDATE characters SET zone = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_LEVEL, "UPDATE characters SET level = ?, xp = 0 WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GROUP_INSTANCE_BY_INSTANCE, "DELETE FROM group_instance WHERE instance = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GROUP_INSTANCE_BY_GUID, "DELETE FROM group_instance WHERE guid = ? AND instance = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_GROUP_INSTANCE, "REPLACE INTO group_instance (guid, instance, permanent) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_INSTANCE_RESETTIME, "UPDATE instance SET resettime = ? WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GLOBAL_INSTANCE_RESETTIME, "UPDATE instance_reset SET resettime = ? WHERE mapid = ? AND difficulty = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_ONLINE, "UPDATE characters SET online = 1 WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_NAME_AT_LOGIN, "UPDATE characters set name = ?, at_login = at_login & ~ ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_WORLDSTATE, "UPDATE worldstates SET value = ? WHERE entry = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_WORLDSTATE, "INSERT INTO worldstates (entry, value) VALUES (?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_INSTANCE_BY_INSTANCE_GUID, "DELETE FROM character_instance WHERE guid = ? AND instance = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_INSTANCE, "UPDATE character_instance SET instance = ?, permanent = ?, extendState = ? WHERE guid = ? AND instance = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_INSTANCE, "INSERT INTO character_instance (guid, instance, permanent, extendState) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GENDER_PLAYERBYTES, "UPDATE characters SET gender = ?, playerBytes = ?, playerBytes2 = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHARACTER_SKILL, "DELETE FROM character_skills WHERE guid = ? AND skill = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_ADD_CHARACTER_SOCIAL_FLAGS, "UPDATE account_social SET flags = flags | ? WHERE account_id = ? AND friend = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_REM_CHARACTER_SOCIAL_FLAGS, "UPDATE account_social SET flags = flags & ~ ? WHERE account_id = ? AND friend = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHARACTER_SOCIAL, "REPLACE INTO account_social (account_id, friend, flags) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHARACTER_SOCIAL, "DELETE FROM account_social WHERE account_id = ? AND friend = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHARACTER_SOCIAL_NOTE, "UPDATE account_social SET note = ? WHERE account_id = ? AND friend = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHARACTER_POSITION, "UPDATE characters SET position_x = ?, position_y = ?, position_z = ?, orientation = ?, map = ?, zone = ?, trans_x = 0, trans_y = 0, trans_z = 0, transguid = 0, taxi_path = '' WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_AURA_FROZEN, "SELECT characters.name FROM characters LEFT JOIN character_aura ON (characters.guid = character_aura.guid) WHERE character_aura.spell = 9454", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_ONLINE, "SELECT name, account, map, zone FROM characters WHERE online > 0", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_DEL_INFO_BY_GUID, "SELECT guid, deleteInfos_Name, deleteInfos_Account, deleteDate FROM characters WHERE deleteDate IS NOT NULL AND guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_DEL_INFO_BY_NAME, "SELECT guid, deleteInfos_Name, deleteInfos_Account, deleteDate FROM characters WHERE deleteDate IS NOT NULL AND deleteInfos_Name LIKE CONCAT('%%', ?, '%%') LIMIT 10", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_DEL_INFO, "SELECT guid, deleteInfos_Name, deleteInfos_Account, deleteDate FROM characters WHERE deleteDate IS NOT NULL", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHARS_BY_ACCOUNT_ID, "SELECT guid FROM characters WHERE account = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PINFO, "SELECT totaltime, level, money, account, race, class, map, zone FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PMONEY, "SELECT money FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PINFO_BANS, "SELECT unbandate, bandate = unbandate, bannedby, banreason FROM character_banned WHERE guid = ? AND active ORDER BY bandate ASC LIMIT 1", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_HOMEBIND, "SELECT mapId, zoneId, posX, posY, posZ FROM character_homebind WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_GUID_NAME_BY_ACC, "SELECT guid, name FROM characters WHERE account = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_POOL_QUEST_SAVE, "SELECT quest_id FROM pool_quest_save WHERE pool_id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_CUSTOMIZE_DATA, "SELECT at_login, race, class FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_CLASS_LVL_AT_LOGIN, "SELECT race, class, level, at_login, knownTitles, playerBytes, playerBytes2  FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_TITLES, "SELECT knownTitles FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_INSTANCE, "SELECT data, completedEncounters FROM instance WHERE map = ? AND id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PERM_BIND_BY_INSTANCE, "SELECT guid FROM character_instance WHERE instance = ? and permanent = 1", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PET_SPELL_LIST, "SELECT DISTINCT pet_spell.spell FROM pet_spell, character_pet WHERE character_pet.owner = ? AND character_pet.id = pet_spell.guid AND character_pet.id <> ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PET, "SELECT id FROM character_pet WHERE owner = ? AND id <> ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PETS, "SELECT id FROM character_pet WHERE owner = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_COD_ITEM_MAIL, "SELECT id, messageType, mailTemplateId, sender, subject, body, money, has_items FROM mail WHERE receiver = ? AND has_items <> 0 AND cod <> 0", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_SOCIAL, "SELECT DISTINCT account_id FROM account_social WHERE friend = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PET_AURA, "SELECT slot, caster_guid, spell, effect_mask, recalculate_mask, stackcount, maxduration, remaintime, remaincharges FROM pet_aura WHERE guid = ?",  CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_PET_AURA_EFFECT, "SELECT slot, effect, amount, baseamount FROM pet_aura_effect WHERE guid = ?",  CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_OLD_CHARS, "SELECT guid, deleteInfos_Account FROM characters WHERE deleteDate IS NOT NULL AND deleteDate < ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_MAIL, "SELECT id, messageType, sender, receiver, subject, body, has_items, expire_time, deliver_time, money, cod, checked, stationery, mailTemplateId FROM mail WHERE receiver = ? ORDER BY id DESC", CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PLAYERBYTES2, "SELECT playerBytes2 FROM characters WHERE guid = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_PET_SPELL, "SELECT spell, active FROM pet_spell WHERE guid = ?",  CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_PET_SPELL_COOLDOWN, "SELECT spell, time FROM pet_spell_cooldown WHERE guid = ?",  CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_SEL_PET_DECLINED_NAME, "SELECT genitive, dative, accusative, instrumental, prepositional FROM character_pet_declinedname WHERE id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_GUID_BY_NAME, "SELECT guid FROM characters WHERE name = ? AND BINARY name = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_AURA_FROZEN, "DELETE FROM character_aura WHERE spell = 9454 AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_INVENTORY_COUNT_ITEM, "SELECT COUNT(itemEntry) FROM character_inventory ci INNER JOIN item_instance ii ON ii.guid = ci.item WHERE itemEntry = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_MAIL_COUNT_ITEM, "SELECT COUNT(itemEntry) FROM mail_items mi INNER JOIN item_instance ii ON ii.guid = mi.item_guid WHERE itemEntry = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_AUCTIONHOUSE_COUNT_ITEM,"SELECT COUNT(itemEntry) FROM auctionhouse ah INNER JOIN item_instance ii ON ii.guid = ah.itemguid WHERE itemEntry = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_GUILD_BANK_COUNT_ITEM, "SELECT COUNT(itemEntry) FROM guild_bank_item gbi INNER JOIN item_instance ii ON ii.guid = gbi.item_guid WHERE itemEntry = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_INVENTORY_ITEM_BY_ENTRY, "SELECT ci.item, cb.slot AS bag, ci.slot, ci.guid, c.account, c.name FROM characters c INNER JOIN character_inventory ci ON ci.guid = c.guid INNER JOIN item_instance ii ON ii.guid = ci.item LEFT JOIN character_inventory cb ON cb.item = ci.bag WHERE ii.itemEntry = ? LIMIT ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_MAIL_ITEMS_BY_ENTRY, "SELECT mi.item_guid, m.sender, m.receiver, cs.account, cs.name, cr.account, cr.name FROM mail m INNER JOIN mail_items mi ON mi.mail_id = m.id INNER JOIN item_instance ii ON ii.guid = mi.item_guid INNER JOIN characters cs ON cs.guid = m.sender INNER JOIN characters cr ON cr.guid = m.receiver WHERE ii.itemEntry = ? LIMIT ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_AUCTIONHOUSE_ITEM_BY_ENTRY, "SELECT  ah.itemguid, ah.itemowner, c.account, c.name FROM auctionhouse ah INNER JOIN characters c ON c.guid = ah.itemowner INNER JOIN item_instance ii ON ii.guid = ah.itemguid WHERE ii.itemEntry = ? LIMIT ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_GUILD_BANK_ITEM_BY_ENTRY, "SELECT gi.item_guid, gi.guildid, g.name FROM guild_bank_item gi INNER JOIN guild g ON g.guildid = gi.guildid INNER JOIN item_instance ii ON ii.guid = gi.item_guid WHERE ii.itemEntry = ? LIMIT ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PET_BY_ENTRY, "SELECT id, entry, owner, modelid, level, exp, Reactstate, slot, name, renamed, curhealth, curmana, abdata, savetime, CreatedBySpell, PetType, specialization FROM character_pet WHERE owner = ? AND id = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PET_BY_ENTRY_AND_SLOT_2, "SELECT id, entry, owner, modelid, level, exp, Reactstate, slot, name, renamed, curhealth, curmana, abdata, savetime, CreatedBySpell, PetType, specialization FROM character_pet WHERE owner = ? AND entry = ? AND ((slot >= ? AND slot <= ?) OR slot > ?)", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PET_BY_SLOT, "SELECT id, entry, owner, modelid, level, exp, Reactstate, slot, name, renamed, curhealth, curmana, abdata, savetime, CreatedBySpell, PetType, specialization FROM character_pet WHERE owner = ? AND ((slot >= ? AND slot <= ?) AND slot = ?) ", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_REPUTATION_BY_FACTION, "DELETE FROM character_reputation WHERE guid = ? AND faction = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_REPUTATION_BY_FACTION, "INSERT INTO character_reputation (guid, faction, standing, flags) VALUES (?, ?, ? , ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_ITEM_REFUND_INSTANCE, "DELETE FROM item_refund_instance WHERE item_guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_ITEM_REFUND_INSTANCE, "INSERT INTO item_refund_instance (item_guid, player_guid, paidMoney, paidExtendedCost) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GROUP, "DELETE FROM groups WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GROUP_MEMBER_ALL, "DELETE FROM group_member WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_GIFT, "INSERT INTO character_gifts (guid, item_guid, entry, flags) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_INSTANCE_BY_INSTANCE, "DELETE FROM instance WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_INSTANCE_BY_INSTANCE, "DELETE FROM character_instance WHERE instance = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_EXPIRED_CHAR_INSTANCE_BY_MAP_DIFF, "DELETE FROM character_instance USING character_instance LEFT JOIN instance ON character_instance.instance = id WHERE (extendState = 0 OR permanent = 0) AND map = ? and difficulty = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GROUP_INSTANCE_BY_MAP_DIFF, "DELETE FROM group_instance USING group_instance LEFT JOIN instance ON group_instance.instance = id WHERE map = ? and difficulty = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_EXPIRED_INSTANCE_BY_MAP_DIFF, "DELETE FROM instance WHERE map = ? AND difficulty = ? AND (SELECT guid FROM character_instance WHERE extendState != 0 AND instance = id LIMIT 1) IS NULL", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_EXPIRE_CHAR_INSTANCE_BY_MAP_DIFF, "UPDATE character_instance LEFT JOIN instance ON character_instance.instance = id SET extendState = extendState-1 WHERE map = ? AND difficulty = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_INSTANCE_BY_MAP_DIFF, "DELETE FROM instance WHERE map = ? and difficulty = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_MAIL_ITEM_BY_ID, "DELETE FROM mail_items WHERE mail_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_PET_DECLINEDNAME, "DELETE FROM character_pet_declinedname WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_ADD_CHAR_PET_DECLINEDNAME, "INSERT INTO character_pet_declinedname (id, owner, genitive, dative, accusative, instrumental, prepositional) VALUES (?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_PET_NAME, "UPDATE character_pet SET name = ?, renamed = 1 WHERE owner = ? AND id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PETITION, "INSERT INTO petition (ownerguid, petitionguid, name, type) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PETITION_BY_GUID, "DELETE FROM petition WHERE petitionguid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PETITION_SIGNATURE_BY_GUID, "DELETE FROM petition_sign WHERE petitionguid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UDP_CHAR_PET_SLOT_BY_SLOT_EXCLUDE_ID, "UPDATE character_pet SET slot = ? WHERE owner = ? AND slot = ? AND id <> ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_PET_SLOT_BY_ID, "UPDATE character_pet SET slot = ? WHERE owner = ? AND id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PET_AURAS, "DELETE FROM pet_aura WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PET_AURAS_EFFECTS, "DELETE FROM pet_aura_effect WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PET_SPELLS, "DELETE FROM pet_spell WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PET_SPELL_COOLDOWNS, "DELETE FROM pet_spell_cooldown WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PET_SPELL_COOLDOWN, "INSERT INTO pet_spell_cooldown (guid, spell, time) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PET_SPELL_BY_SPELL, "DELETE FROM pet_spell WHERE guid = ? and spell = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PET_SPELL, "INSERT INTO pet_spell (guid, spell, active) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PET_AURA, "INSERT INTO pet_aura (guid, slot, caster_guid, spell, effect_mask, recalculate_mask, stackcount, maxduration, remaintime, remaincharges) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PET_AURA_EFFECT, "INSERT INTO pet_aura_effect (guid, slot, effect, baseamount, amount) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_DECLINED_NAME, "DELETE FROM character_declinedname WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_DECLINED_NAME, "INSERT INTO character_declinedname (guid, genitive, dative, accusative, instrumental, prepositional) VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_FACTION_OR_RACE, "UPDATE characters SET name = ?, race = ?, at_login = at_login & ~ ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_SKILL_LANGUAGES, "DELETE FROM character_skills WHERE skill IN (98, 113, 759, 111, 313, 109, 115, 315, 673, 137, 905, 906, 907) AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_SKILL_LANGUAGE, "REPLACE INTO `character_skills` (guid, skill, value, max) VALUES (?, ?, 300, 300)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_TAXI_PATH, "UPDATE characters SET taxi_path = '' WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_TAXIMASK, "UPDATE characters SET taximask = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_QUESTSTATUS, "DELETE FROM character_queststatus WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_SOCIAL_BY_ACCOUNT_ID, "DELETE FROM account_social WHERE account_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_SOCIAL_BY_FRIEND, "DELETE FROM account_social WHERE friend = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_INVENTORY_FACTION_CHANGE, "UPDATE item_instance ii, character_inventory ci SET ii.itemEntry = ? WHERE ii.itemEntry = ? AND ci.guid = ? AND ci.item = ii.guid", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_SPELL_BY_SPELL, "DELETE FROM character_spell WHERE spell = ? AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_SPELL_FACTION_CHANGE, "UPDATE character_spell SET spell = ? where spell = ? AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_REP_BY_FACTION, "DELETE FROM character_reputation WHERE faction = ? AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_REP_FACTION_CHANGE, "UPDATE character_reputation SET faction = ? where faction = ? AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_TITLES_FACTION_CHANGE, "UPDATE characters SET knownTitles = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_RES_CHAR_TITLES_FACTION_CHANGE, "UPDATE characters SET chosenTitle = 0 WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_SPELL_COOLDOWN, "DELETE FROM character_spell_cooldown WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHARACTER, "DELETE FROM characters WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_ACTION, "DELETE FROM character_action WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_AURA, "DELETE FROM character_aura WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_AURA_EFFECT, "DELETE FROM character_aura_effect WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_GIFT, "DELETE FROM character_gifts WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_INSTANCE, "DELETE FROM character_instance WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_INVENTORY, "DELETE FROM character_inventory WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_QUESTSTATUS_REWARDED, "DELETE FROM character_queststatus_rewarded WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_REPUTATION, "DELETE FROM character_reputation WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_SPELL, "DELETE FROM character_spell WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_MAIL, "DELETE FROM mail WHERE receiver = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_MAIL_ITEMS, "DELETE FROM mail_items WHERE receiver = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_PET_BY_OWNER, "DELETE FROM character_pet WHERE owner = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_PET_DECLINEDNAME_BY_OWNER, "DELETE FROM character_pet_declinedname WHERE owner = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_EQUIPMENTSETS, "DELETE FROM character_equipmentsets WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GUILD_EVENTLOG_BY_PLAYER, "DELETE FROM guild_eventlog WHERE PlayerGuid1 = ? OR PlayerGuid2 = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GUILD_BANK_EVENTLOG_BY_PLAYER, "DELETE FROM guild_bank_eventlog WHERE PlayerGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_QUESTSTATUS_DAILY, "DELETE FROM character_queststatus_daily WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_TALENT, "DELETE FROM character_talent WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_SKILLS, "DELETE FROM character_skills WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UDP_CHAR_MONEY, "UPDATE characters SET money = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_ACTION, "REPLACE INTO character_action (guid, spec, button, action, type) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_ACTION, "UPDATE character_action SET action = ?, type = ? WHERE guid = ? AND button = ? AND spec = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_ACTION_BY_BUTTON_SPEC, "DELETE FROM character_action WHERE guid = ? and button = ? and spec = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_INVENTORY_BY_ITEM, "DELETE FROM character_inventory WHERE item = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_INVENTORY_BY_BAG_SLOT, "DELETE FROM character_inventory WHERE bag = ? AND slot = ? AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_MAIL, "UPDATE mail SET has_items = ?, expire_time = ?, deliver_time = ?, money = ?, cod = ?, checked = ? WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_CHAR_QUESTSTATUS, "REPLACE INTO character_queststatus (guid, quest, status, explored, timer) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_QUESTSTATUS_BY_QUEST, "DELETE FROM character_queststatus WHERE guid = ? AND quest = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_CHAR_QUESTSTATUS_OBJECTIVE, "REPLACE INTO `character_queststatus_objective` (`guid`, `objectiveId`, `amount`) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_QUESTSTATUS_OBJECTIVE, "DELETE FROM `character_queststatus_objective` WHERE `guid` = ? AND `objectiveId` = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_QUESTSTATUS_OBJECTIVES, "DELETE FROM `character_queststatus_objective` WHERE `objectiveId` = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_QUESTSTATUS_OBJECTIVE_ALL, "DELETE FROM `character_queststatus_objective` WHERE `guid` = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_QUESTSTATUS, "INSERT IGNORE INTO character_queststatus_rewarded (guid, quest) VALUES (?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_QUESTSTATUS_REWARDED_BY_QUEST, "DELETE FROM character_queststatus_rewarded WHERE guid = ? AND quest = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_SKILL_BY_SKILL, "DELETE FROM character_skills WHERE guid = ? AND skill = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_SKILLS, "INSERT INTO character_skills (guid, skill, value, max) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UDP_CHAR_SKILLS, "UPDATE character_skills SET value = ?, max = ? WHERE guid = ? AND skill = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_SPELL, "REPLACE INTO character_spell (guid, spell, active, disabled, IsMountFavorite) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_STATS, "DELETE FROM character_stats WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_STATS, "INSERT INTO character_stats (guid, maxhealth, maxpower1, maxpower2, maxpower3, maxpower4, maxpower5, maxpower6, strength, agility, stamina, intellect, spirit, armor, resHoly, resFire, resNature, resFrost, resShadow, resArcane, blockPct, dodgePct, parryPct, critPct, rangedCritPct, spellCritPct, attackPower, rangedAttackPower, spellPower, resilience) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PETITION_BY_OWNER, "DELETE FROM petition WHERE ownerguid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PETITION_SIGNATURE_BY_OWNER, "DELETE FROM petition_sign WHERE ownerguid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PETITION_BY_OWNER_AND_TYPE, "DELETE FROM petition WHERE ownerguid = ? AND type = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PETITION_SIGNATURE_BY_OWNER_AND_TYPE, "DELETE FROM petition_sign WHERE ownerguid = ? AND type = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_TALENT_BY_SPELL_SPEC, "DELETE FROM character_talent WHERE guid = ? and spell = ? and spec = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_TALENT, "REPLACE INTO character_talent (guid, spell, spec) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_ACTION_EXCEPT_SPEC, "DELETE FROM character_action WHERE spec<>? AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHAR_PET_BY_ENTRY_AND_SLOT, "SELECT id, entry, owner, modelid, level, exp, Reactstate, slot, name, renamed, curhealth, curmana, abdata, savetime, CreatedBySpell, PetType, specialization FROM character_pet WHERE owner = ? AND slot = ?", CONNECTION_BOTH);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_LIST_SLOT, "UPDATE characters SET slot = ? WHERE guid = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_CHAR_VOID_STORAGE, "SELECT itemId, itemEntry, slot, creatorGuid, randomProperty, suffixFactor, bonuses, customFlags FROM character_void_storage WHERE playerGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_CHAR_VOID_STORAGE_ITEM, "REPLACE INTO character_void_storage (itemId, playerGuid, itemEntry, slot, creatorGuid, randomProperty, suffixFactor, bonuses, customFlags) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_CHAR_VOID_STORAGE_ITEM_BY_SLOT, "DELETE FROM character_void_storage WHERE slot = ? AND playerGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_VOID_STORAGE_ITEM, "DELETE FROM character_void_storage WHERE itemId = ?", CONNECTION_ASYNC);

    /// Guild Finder
    PREPARE_STATEMENT(CHAR_REP_GUILD_FINDER_APPLICANT, "REPLACE INTO guild_finder_applicant (guildId, playerGuid, availability, classRole, interests, comment, submitTime) VALUES(?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GUILD_FINDER_APPLICANT, "DELETE FROM guild_finder_applicant WHERE guildId = ? AND playerGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_GUILD_FINDER_GUILD_SETTINGS, "REPLACE INTO guild_finder_guild_settings (guildId, availability, classRoles, interests, level, listed, comment) VALUES(?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GUILD_FINDER_GUILD_SETTINGS, "DELETE FROM guild_finder_guild_settings WHERE guildId = ?", CONNECTION_ASYNC);

    /// Store
    PREPARE_STATEMENT(CHAR_LOAD_BOUTIQUE_ITEM,      "SELECT itemid, count, transaction FROM store_item WHERE guid = ?",                              CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_BOUTIQUE_ITEM,       "DELETE FROM store_item WHERE transaction = ?",                                                 CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_BOUTIQUE_ITEM,       "UPDATE store_item SET count = ? WHERE transaction = ?",                                        CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_LOAD_BOUTIQUE_LEVEL,     "SELECT level FROM store_level WHERE guid = ?",                                                 CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_BOUTIQUE_LEVEL,      "DELETE FROM store_level WHERE guid = ?",                                                       CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_LOAD_BOUTIQUE_GOLD,      "SELECT gold, transaction FROM store_gold WHERE guid = ? ORDER by gold",                        CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_BOUTIQUE_GOLD,       "DELETE FROM store_gold WHERE transaction = ?",                                                 CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_LOAD_BOUTIQUE_TITLE,     "SELECT title, transaction FROM store_title WHERE guid = ?",                                    CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_BOUTIQUE_TITLE,      "DELETE FROM store_title WHERE transaction = ?",                                                CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_LOAD_STORE_PROFESSION,    "SELECT skill, recipe FROM store_profession WHERE guid = ?",                                   CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_STORE_PROFESSION,     "DELETE FROM store_profession WHERE guid = ? AND skill = ?",                                   CONNECTION_ASYNC);

#ifndef CROSS
    /// Black Market
    PREPARE_STATEMENT(CHAR_INS_BLACKMARKET_AUCTION, "INSERT INTO blackmarket VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_BLACKMARKET_AUCTION, "DELETE FROM blackmarket WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_BLACKMARKET_AUCTION, "UPDATE blackmarket SET bid = ?, bidder = ?, bidderCount = ? WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_BLACKMARKET_AUCTIONS, "SELECT id, templateId, startTime, bid, bidder, bidderCount FROM blackmarket", CONNECTION_SYNCH);
#endif

    /// Artifacts
    PREPARE_STATEMENT(CHAR_LOAD_ARTIFACTS, "SELECT id, power, appearance_mod_id, tier FROM character_artifact WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_ARTIFACT, "INSERT INTO character_artifact (guid, id, power, appearance_mod_id, tier) VALUE (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_ARTIFACT, "DELETE FROM character_artifact WHERE guid = ? AND id = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_LOAD_ARTIFACT_POWERS, "SELECT id, power, rank FROM character_artifact_powers WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_ARTIFACT_POWER, "INSERT INTO character_artifact_powers (guid, id, power, rank) VALUE (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_ARTIFACT_POWERS, "DELETE FROM character_artifact_powers WHERE guid = ? and ID = ?", CONNECTION_ASYNC);

    /// PvP Talents
    PREPARE_STATEMENT(CHAR_LOAD_PVP_TALENTS, "SELECT spell, spec FROM character_pvp_talent WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_PVP_TALENTS, "DELETE FROM character_pvp_talent WHERE guid = ?", CONNECTION_ASYNC);

    //////////////////////////////////////////////////////////////////////////
    /// Garrison
    //////////////////////////////////////////////////////////////////////////
    PREPARE_STATEMENT(CHAR_INS_GARRISON,                 "INSERT INTO character_garrison(id, character_guid, draenor_num_follower_activation, draenor_num_follower_activation_regen_timestamp, draenor_today_mission_count, broken_isles_num_follower_activation, broken_isles_num_follower_activation_regen_timestamp, broken_isles_today_mission_count) VALUES (?,?,?,?,?,?,?,?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON,                 "SELECT id, draenor_num_follower_activation, draenor_num_follower_activation_regen_timestamp, draenor_today_mission_count, broken_isles_num_follower_activation, broken_isles_num_follower_activation_regen_timestamp, broken_isles_today_mission_count FROM character_garrison WHERE character_guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GARRISON,                 "UPDATE character_garrison SET draenor_num_follower_activation = ?, draenor_num_follower_activation_regen_timestamp = ?, draenor_today_mission_count = ?, broken_isles_num_follower_activation = ?, broken_isles_num_follower_activation_regen_timestamp = ?, broken_isles_today_mission_count = ? WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON,                 "DELETE FROM character_garrison WHERE character_guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_GARRISON_RESET_DAILY_MISSIONS,"UPDATE character_garrison SET draenor_today_mission_count = 0, broken_isles_today_mission_count = 0 WHERE 1", CONNECTION_BOTH);

    PREPARE_STATEMENT(CHAR_INS_GARRISON_MISSION,         "INSERT INTO character_garrison_mission(id, garrison_id, mission_id, offer_time, offer_max_duration, start_time, state, success_chance, duration, lethal) VALUES (?, ?, ?, ?, ?, ?, ?, 0, 0, 0)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_MISSION,         "SELECT id, mission_id, offer_time, offer_max_duration, start_time, state, success_chance, duration, lethal FROM character_garrison_mission WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GARRISON_MISSION,         "UPDATE character_garrison_mission SET mission_id = ?, offer_time = ?, offer_max_duration = ?, start_time = ?, state = ?, success_chance = ?, duration = ?, lethal = ? WHERE id = ? AND garrison_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_MISSION,         "DELETE FROM character_garrison_mission WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_MISSIONS,        "DELETE FROM character_garrison_mission WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid = ? LIMIT 1)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_INS_GARRISON_FOLLOWER,        "INSERT INTO character_garrison_follower(id, garrison_id, follower_id, level, xp, durability, quality, item_level_armor, item_level_weapon, current_mission_id, current_building_id, abilities, zone_support_spell_id, flags, ship_name) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_FOLLOWER,        "SELECT id, follower_id, level, xp, durability, quality, item_level_armor, item_level_weapon, current_mission_id, current_building_id, abilities, zone_support_spell_id, flags, ship_name FROM character_garrison_follower WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GARRISON_FOLLOWER,        "UPDATE character_garrison_follower SET follower_id = ?, level = ?, xp = ?, durability = ?, quality = ?, item_level_armor = ?, item_level_weapon = ?, current_mission_id = ?, current_building_id = ?, abilities = ?, zone_support_spell_id = ?, flags = ?, ship_name = ? WHERE id = ? AND garrison_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_FOLLOWER,        "DELETE FROM character_garrison_follower WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_FOLLOWERS,       "DELETE FROM character_garrison_follower WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid = ? LIMIT 1)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_INS_GARRISON_WORKORDER,       "INSERT INTO character_garrison_work_order(id, garrison_id, plot_instance_id, shipment_id, creation_time, complete_time) VALUES (?,?,?,?,?,?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_WORKORDER,       "SELECT id, plot_instance_id, shipment_id, creation_time, complete_time FROM character_garrison_work_order WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_WORKORDER,       "DELETE FROM character_garrison_work_order WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_WORKORDERS,      "DELETE FROM character_garrison_work_order WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid = ? LIMIT 1)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_INS_GARRISON_TALENT,          "INSERT INTO character_garrison_talent(garrison_id, talent_id, start_research_time, flags) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_TALENT,          "SELECT talent_id, start_research_time, flags FROM character_garrison_talent WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GARRISON_TALENT,          "UPDATE character_garrison_talent SET start_research_time = ?, flags = ? WHERE garrison_id = ? AND talent_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_TALENT,          "DELETE FROM character_garrison_talent WHERE garrison_id = ? AND talent_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_TALENTS,         "DELETE FROM character_garrison_talent WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid = ? LIMIT 1)", CONNECTION_ASYNC);

    //////////////////////////////////////////////////////////////////////////

    PREPARE_STATEMENT(CHAR_INS_GARRISON_DRAENOR,        "INSERT INTO character_garrison_draenor(id, level, blue_recipes, specializations, cache_last_usage) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_DRAENOR,        "SELECT level, blue_recipes, specializations, cache_last_usage FROM character_garrison_draenor WHERE id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GARRISON_DRAENOR,        "UPDATE character_garrison_draenor SET level = ?, blue_recipes = ?, specializations = ?, cache_last_usage = ? WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_DRAENOR,        "DELETE FROM character_garrison_draenor WHERE id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_INS_GARRISON_DRAENOR_BUILDING,       "INSERT INTO character_garrison_draenor_building(id, garrison_id, plot_instance_id, building_id, spec_id, time_built_start, time_built_end, follower_assigned, active, gathering_data) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_DRAENOR_BUILDING,       "SELECT id, plot_instance_id, building_id, spec_id, time_built_start, time_built_end, follower_assigned, active, gathering_data FROM character_garrison_draenor_building WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GARRISON_DRAENOR_BUILDING,       "UPDATE character_garrison_draenor_building SET plot_instance_id = ?, building_id = ?, spec_id = ?, time_built_start = ?, time_built_end = ?, follower_assigned = ?, active = ?, gathering_data = ? WHERE id = ? AND garrison_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_DRAENOR_BUILDING,       "DELETE FROM character_garrison_draenor_building WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_DRAENOR_BUILDINGS,      "DELETE FROM character_garrison_draenor_building WHERE garrison_id = (SELECT b.id FROM character_garrison b WHERE b.character_guid = ? LIMIT 1)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_ADD_GARRISON_DRAENOR_DAILY_TAVERN_DATA_CHAR, "REPLACE INTO character_garrison_draenor_daily_tavern_data (CharacterGuid, NpcEntry) VALUES (?,?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_DRAENOR_DAILY_TAVERN_DATA_CHAR, "SELECT CharacterGuid, NpcEntry FROM character_garrison_draenor_daily_tavern_data WHERE CharacterGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_DRAENOR_DAILY_TAVERN_DATA_CHAR, "DELETE FROM character_garrison_draenor_daily_tavern_data WHERE CharacterGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_DRAENOR_DAILY_TAVERN_DATA,      "DELETE FROM character_garrison_draenor_daily_tavern_data", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_ADD_GARRISON_DRAENOR_WEEKLY_TAVERN_DATA_CHAR, "REPLACE INTO character_garrison_draenor_weekly_tavern_data (CharacterGuid, FollowerID, Abilities) VALUES (?,?,?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_DRAENOR_WEEKLY_TAVERN_DATA_CHAR, "SELECT CharacterGuid, FollowerID, Abilities FROM character_garrison_draenor_weekly_tavern_data WHERE CharacterGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_DRAENOR_WEEKLY_TAVERN_DATA_CHAR, "DELETE FROM character_garrison_draenor_weekly_tavern_data WHERE CharacterGuid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_DRAENOR_WEEKLY_TAVERN_DATA,      "DELETE FROM character_garrison_draenor_weekly_tavern_data", CONNECTION_ASYNC);

    //////////////////////////////////////////////////////////////////////////

    PREPARE_STATEMENT(CHAR_INS_GARRISON_BROKEN_ISLES,        "INSERT INTO character_garrison_broken_isles(id, support_follower_db_id) VALUES (?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GARRISON_BROKEN_ISLES,        "SELECT support_follower_db_id FROM character_garrison_broken_isles WHERE id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_GARRISON_BROKEN_ISLES,        "UPDATE character_garrison_broken_isles SET support_follower_db_id = ? WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GARRISON_BROKEN_ISLES,        "DELETE FROM character_garrison_broken_isles WHERE id = (SELECT b.id FROM character_garrison b WHERE b.character_guid=?)", CONNECTION_ASYNC);

    //////////////////////////////////////////////////////////////////////////
    /// Battle pets
    PREPARE_STATEMENT(CHAR_UPD_LAST_BATTLEPET, "UPDATE characters SET lastbattlepet = ? WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_LAST_BATTLEPET, "SELECT lastbattlepet FROM characters WHERE guid = ? AND lastbattlepet != 0", CONNECTION_ASYNC);

    //////////////////////////////////////////////////////////////////////////
    /// Calendar
    PREPARE_STATEMENT(CHAR_REP_CALENDAR_EVENT, "REPLACE INTO calendar_events (id, creator, title, description, type, dungeon, eventtime, flags, time2) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CALENDAR_EVENT, "DELETE FROM calendar_events WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_CALENDAR_INVITE, "REPLACE INTO calendar_invites (id, event, invitee, sender, status, statustime, rank, text) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CALENDAR_INVITE, "DELETE FROM calendar_invites WHERE id = ?", CONNECTION_ASYNC);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// SpellCharges
    PREPARE_STATEMENT(CHAR_SEL_CHARGES_COOLDOWN, "SELECT categoryId, rechargeStart, rechargeEnd FROM character_spell_charges WHERE guid = ? AND rechargeEnd > UNIX_TIMESTAMP() ORDER BY rechargeEnd", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHARGES_COOLDOWN, "INSERT INTO character_spell_charges (guid, categoryId, rechargeStart, rechargeEnd) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHARGES_COOLDOWN, "DELETE FROM character_spell_charges WHERE guid = ?", CONNECTION_ASYNC);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Premades
    PREPARE_STATEMENT(CHAR_SEL_PREMADES, "SELECT transaction, templateId, faction, type FROM webshop_delivery_premade WHERE account = ? and delivery = 0", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_UPD_PREMADE_SUCESS, "UPDATE webshop_delivery_premade SET delivery = 1 WHERE transaction = ?", CONNECTION_ASYNC);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Encounter logs
    PREPARE_STATEMENT(CHAR_INS_GUILD_ENCOUNTER_DATA, "INSERT INTO guild_encounter_data (AttemptID, Expansion, RealmID, GuildID, GuildFaction, GuildName, MapID, EncounterID, DifficultyID, StartTime, CombatDuration, EndTime, Success, EncounterHealth, BattleRezCount, BloodlustCount) VALUE (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_GUILD_ENCOUNTER_ROSTER, "INSERT INTO guild_encounter_roster (AttemptID, Name, GuidLow, Level, Class, SpecID, Role, ItemLevel) VALUE (?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Achievements
    //////////////////////////////////////////////////////////////////////////
    PREPARE_STATEMENT(CHAR_DEL_INVALID_ACHIEV_PROGRESS_CRITERIA, "DELETE FROM character_achievement_progress WHERE criteria = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_INVALID_ACHIEV_PROGRESS_CRITERIA_GUILD, "DELETE FROM guild_achievement_progress WHERE criteria = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_INVALID_ACHIEVMENT, "DELETE FROM character_achievement WHERE achievement = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_INVALID_GUILD_ACHIEVMENT, "DELETE FROM guild_achievement WHERE achievement = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_ACHIEVEMENTS, "SELECT achievement, date FROM character_achievement WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_ACHIEVEMENTS_COUNT, "SELECT COUNT(*) FROM character_achievement WHERE achievement = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_CRITERIAPROGRESS, "SELECT criteria, counter, date FROM character_achievement_progress WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_ACHIEVEMENT, "DELETE FROM character_achievement WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_ACHIEVEMENT_PROGRESS, "DELETE FROM character_achievement_progress WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_ACHIEVEMENT, "INSERT INTO character_achievement (guid, achievement, date) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_ACHIEVEMENT_PROGRESS_BY_CRITERIA, "DELETE FROM character_achievement_progress WHERE guid = ? AND criteria = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_ACHIEVEMENT_PROGRESS, "INSERT INTO character_achievement_progress (guid, criteria, counter, date) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_ACHIEVEMENT_BY_ACHIEVEMENT, "DELETE FROM character_achievement WHERE achievement = ? AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_CHAR_ACHIEVEMENT, "UPDATE character_achievement SET achievement = ? where achievement = ? AND guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_ACHIEVEMENTS, "DELETE FROM character_achievement WHERE guid = ? AND achievement NOT IN (456,457,458,459,460,461,462,463,464,465,466,467,1400,1402,1404,1405,1406,1407,1408,1409,1410,1411,1412,1413,1414,1415,1416,1417,1418,1419,1420,1421,1422,1423,1424,1425,1426,1427,1463,3117,3259,4078,4576,4998,4999,5000,5001,5002,5003,5004,5005,5006,5007,5008,5381,5382,5383,5384,5385,5386,5387,5388,5389,5390,5391,5392,5393,5394,5395,5396,6433,6523,6524,6743,6744,6745,6746,6747,6748,6749,6750,6751,6752,6829,6859,6860,6861,6862,6863,6864,6865,6866,6867,6868,6869,6870,6871,6872,6873)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_CHAR_ACHIEVEMENT_COUNT, "REPLACE INTO character_achievement_count (guid, count) VALUES (?, ?)", CONNECTION_ASYNC);

    /// Guild
    PREPARE_STATEMENT(CHAR_DEL_GUILD_ACHIEVEMENT, "DELETE FROM guild_achievement WHERE guildId = ? AND achievement = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_GUILD_ACHIEVEMENT, "INSERT INTO guild_achievement (guildId, achievement, date, guids) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_GUILD_ACHIEVEMENT_CRITERIA, "DELETE FROM guild_achievement_progress WHERE guildId = ? AND criteria = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_GUILD_ACHIEVEMENT_CRITERIA, "INSERT INTO guild_achievement_progress (guildId, criteria, counter, date, completedGuid) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_ALL_GUILD_ACHIEVEMENTS, "DELETE FROM guild_achievement WHERE guildId = ? AND achievement NOT IN (5407,5408,5409,5410,5411,5985,6126,6628,6678,6679,6680,8257,8512,8513,9397,9399,10380)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_ALL_GUILD_ACHIEVEMENT_CRITERIA, "DELETE FROM guild_achievement_progress WHERE guildId = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_GUILD_ACHIEVEMENT, "SELECT achievement, date, guids FROM guild_achievement WHERE guildId = ?", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_SEL_GUILD_ACHIEVEMENT_CRITERIA, "SELECT criteria, counter, date, completedGuid FROM guild_achievement_progress WHERE guildId = ?", CONNECTION_SYNCH);
    //////////////////////////////////////////////////////////////////////////

    PREPARE_STATEMENT(CHAR_SEL_DAILY_LOOT_COOLDOWNS, "SELECT `entry` FROM `character_daily_loot_cooldown` WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_DAILY_LOOT_COOLDOWNS, "INSERT INTO `character_daily_loot_cooldown` (`guid`, `entry`) VALUE (?, ?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_BOSS_LOOTED, "SELECT `boss_entry`, `boss_model_id`, `boss_difficulty_id` FROM `characters_boss_looted` WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_BOSS_LOOTED, "INSERT INTO `characters_boss_looted` (`guid`, `boss_entry`, `boss_model_id`, `boss_difficulty_id`) VALUE (?, ?, ?, ?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_DEL_PERSONAL_INSTANCE_LOCKOUTS, "DELETE FROM character_personal_instance_lockout WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_PERSONAL_INSTANCE_LOCKOUTS, "SELECT map, difficulty, encounterMask FROM character_personal_instance_lockout WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_PERSONAL_INSTANCE_LOCKOUT,  "INSERT INTO character_personal_instance_lockout (guid, map, difficulty, encounterMask) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_WORLD_STATES, "SELECT `worldstate`, `value` FROM character_worldstates WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_WORLD_STATES, "REPLACE INTO `character_worldstates` (`guid`, `worldstate`, `value`) VALUES (?, ?, ?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_UPD_WORLD_STATE, "UPDATE `character_worldstates` SET `value` = ? WHERE `guid` = ? and `worldstate` = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_WORLD_STATE_DATA, "DELETE FROM `character_worldstates` WHERE `guid` = ? AND `worldstate` = ? and `value` = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_UPD_XP_RATE, "UPDATE characters SET xpRate = ? WHERE guid = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_REP_STATS, "REPLACE INTO `character_stats_wod` (`guid`, `strength`, `agility`, `stamina`, `intellect`, `critPct`, `haste`, `mastery`, `spirit`, `armorBonus`, `multistrike`, `leech`, `versatility`, `avoidance`, `attackDamage`, `attackPower`, `attackSpeed`, `spellPower`, `manaRegen`, `armor`, `dodgePct`, `parryPct`, `blockPct`, `ilvl`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_BORIS_IP, "SELECT count(*) from character_boris where ip = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_BORIS_MAC_ADDR, "SELECT guid FROM character_boris where macaddr = ?", CONNECTION_ASYNC)

    PREPARE_STATEMENT(CHAR_INS_EASTER_EVENT, "INSERT INTO character_easter_event (egg_guid, account_id) VALUES (?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_EASTER_EVENT, "SELECT egg_guid, account_id FROM character_easter_event WHERE egg_guid = ? AND account_id = ?", CONNECTION_ASYNC)

    //////////////////////////////////////////////////////////////////////////
    /// Glyphs
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_GLYPHS, "SELECT talentGroup, glyphId FROM character_glyphs WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHAR_GLYPHS, "DELETE FROM character_glyphs WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHAR_GLYPHS, "INSERT INTO character_glyphs VALUES(?, ?, ?)", CONNECTION_ASYNC);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// World Quests
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_WORLD_QUEST_REWARDS, "SELECT quest_id, type, id, amount, arg0, bonuses FROM character_world_quest_rewards WHERE guid = ? ORDER BY quest_id ASC", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHARACTER_WORLD_QUEST_REWARDS, "DELETE FROM character_world_quest_rewards WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHARACTER_WORLD_QUEST_REWARDS_BY_QUEST_ID, "DELETE FROM character_world_quest_rewards WHERE quest_id = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_WORLD_QUESTS, "SELECT quest_id, creation_time, duration FROM world_quests", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_WORLD_QUEST, "DELETE FROM world_quests WHERE quest_id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_WORLD_QUEST, "INSERT INTO world_quests (quest_id, creation_time, duration) VALUE (?, ?, ?)", CONNECTION_ASYNC);
    //////////////////////////////////////////////////////////////////////////

    // Challenge
    PREPARE_STATEMENT(CHAR_INS_CHALLENGE, "INSERT INTO challenge (`ID`, `GuildID`, `MapID`, `ChallengeID`, `RecordTime`, `Date`, `ChallengeLevel`, `TimerLevel`, `Affixes`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHALLENGE_MEMBER, "INSERT INTO challenge_member (`id`, `member`, `specID`, `ChallengeLevel`, `Date`, GuildId) VALUES (?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_INS_CHALLENGE_OPLOTE_LOOT, "REPLACE INTO challenge_oplote_loot (`guid`, `date`, `ChallengeLevel`) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHALLENGE_OPLOTE_LOOT, "DELETE FROM challenge_oplote_loot WHERE  date <= UNIX_TIMESTAMP()", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHALLENGE_OPLOTE_LOOT_BY_GUID, "DELETE FROM challenge_oplote_loot WHERE guid = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_LEGENDARIES, "SELECT points, count FROM character_legendaries WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_LEGENDARIES, "REPLACE character_legendaries (guid, points, count) VALUE (?, ?, ?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_REP_NOMI_RECIPES, "REPLACE INTO character_nomi_recipes (guid, recipe_item_id) VALUE (?, ?)", CONNECTION_ASYNC)

    PREPARE_STATEMENT(CHAR_INS_OR_UPD_CHARACTER_SELL_DATA, "INSERT INTO character_sell_data (guid, ilvl, price, class) VALUES (?, ?, ?, ?) ON DUPLICATE KEY UPDATE ilvl = ?, price = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_MAP_EVENTS, "SELECT GroupID, Id, StartTime, Duration, RepeatInterval FROM map_events ORDER BY StartTime", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_UPD_MAP_EVENTS, "UPDATE map_events SET StartTime = ?, Id = ? WHERE GroupID = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_CONTRIBUTIONS, "SELECT ContributionID, State, StartTime, Progress, OccurenceValue FROM contributions", CONNECTION_SYNCH);
    PREPARE_STATEMENT(CHAR_UPD_CONTRIBUTIONS, "UPDATE contributions SET  State = ?, StartTime = ?, Progress = ?, OccurenceValue = ? WHERE ContributionID = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_DEL_CHARACTER_REP_MODS, "DELETE FROM account_reputation_mods WHERE id = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_CHARACTER_REP_MODS, "REPLACE INTO account_reputation_mods (id, factionId, sourceId, modifier) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_CHARACTER_REP_MODS, "SELECT factionId, sourceId, modifier FROM account_reputation_mods WHERE id = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_PVP_SEASON_REWARD_RANKING, "SELECT bracket, rating FROM character_pvp_season_reward_ranking WHERE guid = ? AND rewarded = 0", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_PVP_SEASON_REWARD_RANKING_REWARDED, "UPDATE character_pvp_season_reward_ranking SET rewarded = 1 WHERE guid = ?", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_SEL_CHEST_TIMED_LOCKOUT, "SELECT chestGuid, lockoutExpireTime FROM character_chest_timed_lockout WHERE guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_CHEST_TIMED_LOCKOUT, "REPLACE INTO character_chest_timed_lockout (guid, chestGuid, lockoutExpireTime) VALUES (?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_CHEST_TIMED_LOCKOUT, "DELETE FROM character_chest_timed_lockout WHERE guid = ? AND chestGuid = ?", CONNECTION_ASYNC);

    //////////////////////////////////////////////////////////////////////////
    /// Arena Decay system
    PREPARE_STATEMENT(CHAR_SEL_ELIGIBLE_CHAR_FOR_DECAY_2S, "SELECT cbracket.guid FROM `character_bracket_data` AS cbracket INNER JOIN `character_worldstates` AS cws ON cws.guid = cbracket.`guid` AND cbracket.`bracket` = 0 AND cws.`worldstate` = 19 AND cws.`value` < 5 AND cbracket.`matchMakerRating` >= 2000;", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_SEL_ELIGIBLE_CHAR_FOR_DECAY_3S, "SELECT cbracket.guid FROM `character_bracket_data` AS cbracket INNER JOIN `character_worldstates` AS cws ON cws.guid = cbracket.`guid` AND cbracket.`bracket` = 1 AND cws.`worldstate` = 21 AND cws.`value` < 5 AND cbracket.`matchMakerRating` >= 2000;", CONNECTION_ASYNC);

    PREPARE_STATEMENT(CHAR_UPD_ELIGIBLE_CHAR_FOR_DECAY_2S, "UPDATE `character_bracket_data` cbracket, `character_worldstates` cws SET cbracket.`rating` = GREATEST(0, cbracket.`rating` - LEAST(100, 25 + (25 * cws.`value`))), cws.`value` = cws.`value` + 1 WHERE cbracket.guid = ? AND cbracket.bracket = 0 AND cbracket.guid = cws.guid AND cws.`worldstate` = 20;", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_UPD_ELIGIBLE_CHAR_FOR_DECAY_3S, "UPDATE `character_bracket_data` cbracket, `character_worldstates` cws SET cbracket.`rating` = GREATEST(0, cbracket.`rating` - LEAST(100, 25 + (25 * cws.`value`))), cws.`value` = cws.`value` + 1 WHERE cbracket.guid = ? AND cbracket.bracket = 1 AND cbracket.guid = cws.guid AND cws.`worldstate` = 22;", CONNECTION_ASYNC);
    //////////////////////////////////////////////////////////////////////////

    PREPARE_STATEMENT(CHAR_SEL_ITEM_RELIC_DATA, "SELECT data FROM item_relic_talent WHERE guid = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_REP_ITEM_RELIC_DATA, "REPLACE INTO item_relic_talent (guid, data) VALUES (?, ?)", CONNECTION_ASYNC)

    PREPARE_STATEMENT(CHAR_SEL_CORPSE_LOCATION, "SELECT mapId, posX, posY, posZ, orientation FROM corpse WHERE guid = ?", CONNECTION_ASYNC)

    PREPARE_STATEMENT(CHAR_REP_INSTANCE_GO_LOOTED, "REPLACE INTO instance_gameobject_looted (guid, instanceId) VALUE (?, ?)", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_SEL_INSTANCE_GO_LOOTED, "SELECT guid FROM instance_gameobject_looted where instanceId = ?", CONNECTION_ASYNC)
    PREPARE_STATEMENT(CHAR_DEL_INSTANCE_GO_LOOTED, "DELETE FROM instance_gameobject_looted where instanceId = ?", CONNECTION_ASYNC)

    PREPARE_STATEMENT(CHAR_SEL_OBJECT_LOCKOUT, "SELECT `ObjectDbGuid`, `QuestId`, `Invisible` FROM `character_object_lockout` WHERE `PlayerGuid` = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_REP_OBJECT_LOCKOUT, "REPLACE INTO `character_object_lockout` (`PlayerGuid`, `ObjectDbGuid`, `QuestId`, `Invisible`) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(CHAR_DEL_OBJECT_LOCKOUT, "DELETE FROM `character_object_lockout` WHERE `PlayerGuid` = ? AND `QuestId` = ?", CONNECTION_ASYNC);
}
