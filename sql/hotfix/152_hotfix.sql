drop table if exists retail_chr_races_locales;
CREATE TABLE `retail_chr_races_locales` (
`ROW_ID`  int(11) UNSIGNED NOT NULL AUTO_INCREMENT ,
`col_0_koKR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_frFR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_deDE`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_zhCN`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_zhTW`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_esES`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_esMX`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_ruRU`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_ptPT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_0_itIT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_koKR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_frFR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_deDE`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_zhCN`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_zhTW`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_esES`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_esMX`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_ruRU`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_ptPT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_1_itIT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_koKR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_frFR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_deDE`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_zhCN`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_zhTW`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_esES`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_esMX`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_ruRU`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_ptPT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_2_itIT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_koKR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_frFR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_deDE`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_zhCN`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_zhTW`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_esES`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_esMX`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_ruRU`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_ptPT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_3_itIT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_koKR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_frFR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_deDE`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_zhCN`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_zhTW`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_esES`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_esMX`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_ruRU`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_ptPT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_4_itIT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_koKR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_frFR`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_deDE`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_zhCN`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_zhTW`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_esES`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_esMX`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_ruRU`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_ptPT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
`col_5_itIT`  text CHARACTER SET utf8 COLLATE utf8_general_ci NULL ,
PRIMARY KEY (`ROW_ID`)
)
ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci
AUTO_INCREMENT=1
ROW_FORMAT=DYNAMIC
;


delete from retail_chr_races_locales where ROW_ID in (27,28,29,30);
INSERT INTO `retail_chr_races_locales` (`ROW_ID`, `col_0_koKR`, `col_0_frFR`, `col_0_deDE`, `col_0_zhCN`, `col_0_zhTW`, `col_0_esES`, `col_0_esMX`, `col_0_ruRU`, `col_0_ptPT`, `col_0_itIT`, `col_1_koKR`, `col_1_frFR`, `col_1_deDE`, `col_1_zhCN`, `col_1_zhTW`, `col_1_esES`, `col_1_esMX`, `col_1_ruRU`, `col_1_ptPT`, `col_1_itIT`, `col_2_koKR`, `col_2_frFR`, `col_2_deDE`, `col_2_zhCN`, `col_2_zhTW`, `col_2_esES`, `col_2_esMX`, `col_2_ruRU`, `col_2_ptPT`, `col_2_itIT`, `col_3_koKR`, `col_3_frFR`, `col_3_deDE`, `col_3_zhCN`, `col_3_zhTW`, `col_3_esES`, `col_3_esMX`, `col_3_ruRU`, `col_3_ptPT`, `col_3_itIT`, `col_4_koKR`, `col_4_frFR`, `col_4_deDE`, `col_4_zhCN`, `col_4_zhTW`, `col_4_esES`, `col_4_esMX`, `col_4_ruRU`, `col_4_ptPT`, `col_4_itIT`, `col_5_koKR`, `col_5_frFR`, `col_5_deDE`, `col_5_zhCN`, `col_5_zhTW`, `col_5_esES`, `col_5_esMX`, `col_5_ruRU`, `col_5_ptPT`, `col_5_itIT`) VALUES
('27', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Nb', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Nightborne', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ночнорожденный', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ночнорожденная', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'ночнорожденный', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'ночнорожденная', NULL, NULL),
('28', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ht', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'HighmountainTauren', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Таурен Крутогорья', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Тауренка Крутогорья', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'таурен Крутогорья', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'тауренка Крутогорья', NULL, NULL),
('29', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ve', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'VoidElf', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Эльф Бездны', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Эльфийка Бездны', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'эльф Бездны', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'эльфийка Бездны', NULL, NULL),
('30', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Ld', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'LightforgedDraenei', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Озаренный дреней', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Озаренная дренейка', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'озаренный дреней', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'озаренная дренейка', NULL, NULL);