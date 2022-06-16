-- Seething Shore BG HF Registry
DELETE from `_hotfixs` where Entry=894 and Hash = 1435332686;
INSERT INTO `_hotfixs` (`ID`, `Entry`, `Hash`, `Date`, `Comment`) VALUES
('13002', '894', '1435332686', '1623425454', 'Battlemaster list - Seething Shore');


ALTER TABLE `retail_battlemaster_list_locales`
MODIFY COLUMN `col_0_ruRU`  text CHARACTER SET utf8 COLLATE utf8_general_ci,
MODIFY COLUMN `col_1_ruRU`  text CHARACTER SET utf8 COLLATE utf8_general_ci;

replace INTO `retail_battlemaster_list_locales` (`ROW_ID`, `col_0_koKR`, `col_0_frFR`, `col_0_deDE`, `col_0_zhCN`, `col_0_zhTW`, `col_0_esES`, `col_0_esMX`, `col_0_ruRU`, `col_0_ptPT`, `col_0_itIT`, `col_1_koKR`, `col_1_frFR`, `col_1_deDE`, `col_1_zhCN`, `col_1_zhTW`, `col_1_esES`, `col_1_esMX`, `col_1_ruRU`, `col_1_ptPT`, `col_1_itIT`, `col_2_koKR`, `col_2_frFR`, `col_2_deDE`, `col_2_zhCN`, `col_2_zhTW`, `col_2_esES`, `col_2_esMX`, `col_2_ruRU`, `col_2_ptPT`, `col_2_itIT`, `col_3_koKR`, `col_3_frFR`, `col_3_deDE`, `col_3_zhCN`, `col_3_zhTW`, `col_3_esES`, `col_3_esMX`, `col_3_ruRU`, `col_3_ptPT`, `col_3_itIT`) VALUES
('894', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Бурлящий берег', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Битва за ресурсы', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
