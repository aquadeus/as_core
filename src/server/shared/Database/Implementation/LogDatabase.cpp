#include "LogDatabase.h"

void LogDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_LOGDATABASE_STATEMENTS);

    PREPARE_STATEMENT(LOG_INSERT_MAP_DIFF_MESURE, "INSERT INTO map_diff_mesure (diff, map, zone, mesures) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_LOG_MONEY, "INSERT INTO log_money (guid, amount, source, newamount) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_LOOTBOX_LOG, "INSERT INTO lootbox_log (guid, timestamp, log) VALUES (?, CURRENT_TIMESTAMP(), ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_MAIL_LOG, "INSERT INTO log_mail (id, messageType, stationery, mailTemplateId, sender, receiver, subject, body, has_items, expire_time, deliver_time, money, cod, checked) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_NAME_LOG, "INSERT INTO log_rename (`guid`, `date`, `oldName`, `newName`) VALUES(? , NOW(), ? , ? )", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_GM_CHAT_LOG, "INSERT INTO log_gm_chat (`type`, `date`, `from_account_id`, `from_account_name`, `from_character_id`, `from_character_name`, `to_account_id`, `to_account_name`, `to_character_id`, `to_character_name`, `message`) VALUES (?, NOW(), ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_GM_LOG, "INSERT INTO log_gm (`date`, `gm_account_id`, `gm_account_name`, `gm_character_id`, `gm_character_name`, `gm_last_ip`, `sc_account_id`, `sc_account_name`, `sc_character_id`, `sc_character_name`, `command`) VALUES (NOW(), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_FACTION_CHANGE_LOG, "INSERT INTO `log_faction_change` (`id`, `guid`, `account`, `OldRace`, `NewRace`, `date`) VALUES (0, ?, ?, ?, ?, NOW())", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_AUCTION_HOUSE_SELL_LOG, "INSERT INTO auctionhouse_sell_log (time, itemEntry, itemBonuses, count, itemRandomSuffix, itemRandomProperty, price, botsell) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_ENCOUNTER_DAMAGE_LOG, "INSERT INTO encounter_damage_log (encounterId, encounterStartTime, logTime, attackerGuid, damage, spellId) VALUES (?, ?, ?, ?, ?, ?) ", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_ENCOUNTER_GROUP_DUMP_LOG, "INSERT INTO encounter_group_dump (encounterId, encounterStartTime, dumpTime, dump) VALUES (?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_INSERT_TIME_DIFF_LOG, "INSERT INTO time_diff_log (time, average, max, players) VALUES (UNIX_TIMESTAMP(), ?, ?, ?)", CONNECTION_ASYNC);

    PREPARE_STATEMENT(LOG_INSERT_ITEM_GM_LOG, "INSERT INTO item_gm_logs (item_guid, player_guid, item_entry, timestamp, itemBonuses) VALUES (?, ?, ?, ?, ?)", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_UPDATE_ITEM_GM_LOG, "UPDATE item_gm_logs SET itemBonuses = ? WHERE item_guid = ?", CONNECTION_ASYNC);
    PREPARE_STATEMENT(LOG_SELECT_ITEM_GM_LOG, "SELECT item_guid, player_guid, item_entry, timestamp, itemBonuses, context FROM item_gm_logs WHERE player_guid = ? AND item_entry = ?", CONNECTION_ASYNC);
}
