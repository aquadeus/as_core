-- https://forum.wowcircle.com/showthread.php?t=1050156
update creature_template set ainame = '', scriptname = 'npc_westfall_homeless', gossip_menu_id = 11635 where entry in (42384,42386,42391,42383);

update creature_template_wdb set killcredit1 = 0 where killcredit1 in (42414,42415,42416,42417);

update creature_template_wdb set killcredit2 = 0 where killcredit2 in (42414,42415,42416,42417);

delete from conditions where SourceGroup in (11635) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 11635, 0, 0, 0, 9, 0, 26209, 0, 0, 0, 0, '', 'Send Gossip If Player Has Quest'),
(15, 11635, 1, 0, 0, 9, 0, 26209, 0, 0, 0, 0, '', 'Send Gossip If Player Has Quest');

delete from conditions where SourceEntry in (4238401) and SourceTypeOrReferenceId = 22;


delete from gossip_menu_option where menu_id in (11635);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `box_money`, `box_text`, `OptionBroadcastTextID`) VALUES 
(11635, 0, 0, 'Did you see who killed the Furlbrows?', 1, 1, 0, '', 42370),
(11635, 1, 0, 'Maybe a couple copper will loosen your tongue. Now tell me, did you see who killed the Furlbrows?', 1, 1, 2, 'Are you sure you want to give this hobo money?', 42371);

delete from locales_gossip_menu_option where menu_id in (11635);
INSERT INTO `locales_gossip_menu_option` (`menu_id`, `id`, `option_text_loc1`, `option_text_loc8`, `box_text_loc1`, `box_text_loc8`) VALUES 
(11635, 1, '', '', 'Are you sure you want to give this hobo money?', 'Вы уверены, что хотите дать этому бродяге денег?');

delete from creature_text where entry in (42384,42386,42391,42383);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`, `comment`) VALUES 
(42384, 0, 0, 'You rich punks are all the same!', 12, 0, 100, 0, 0, 42384, ''),
(42384, 0, 1, 'Livin\' like an animal is hard enough without your insults!', 12, 0, 100, 0, 0, 42386, ''),
(42384, 0, 2, 'I ain\'t tellin\' you a damn thing!', 12, 0, 100, 0, 0, 42385, ''),
(42384, 0, 3, 'Now you\'ve gone and done it! TIME FOR THE FIST!', 12, 0, 100, 0, 0, 42389, ''),
(42384, 0, 4, 'Maybe I can sell your liver for some gold.', 12, 0, 100, 0, 0, 42387, ''),
(42384, 0, 5, 'I TOLD YOU TO PAY UP, PAL!', 12, 0, 100, 0, 0, 42388, ''),
(42384, 1, 0, 'Clue #1', 12, 0, 100, 0, 0, 42380, ''),
(42384, 2, 0, 'Clue #2', 12, 0, 100, 0, 0, 42381, ''),
(42384, 3, 0, 'Clue #3', 12, 0, 100, 0, 0, 42382, ''),
(42384, 4, 0, 'Clue #4', 12, 0, 100, 0, 0, 42383, ''),

(42386, 0, 0, 'You rich punks are all the same!', 12, 0, 100, 0, 0, 42384, ''),
(42386, 0, 1, 'Livin\' like an animal is hard enough without your insults!', 12, 0, 100, 0, 0, 42386, ''),
(42386, 0, 2, 'I ain\'t tellin\' you a damn thing!', 12, 0, 100, 0, 0, 42385, ''),
(42386, 0, 3, 'Now you\'ve gone and done it! TIME FOR THE FIST!', 12, 0, 100, 0, 0, 42389, ''),
(42386, 0, 4, 'Maybe I can sell your liver for some gold.', 12, 0, 100, 0, 0, 42387, ''),
(42386, 0, 5, 'I TOLD YOU TO PAY UP, PAL!', 12, 0, 100, 0, 0, 42388, ''),
(42386, 1, 0, 'Clue #1', 12, 0, 100, 0, 0, 42380, ''),
(42386, 2, 0, 'Clue #2', 12, 0, 100, 0, 0, 42381, ''),
(42386, 3, 0, 'Clue #3', 12, 0, 100, 0, 0, 42382, ''),
(42386, 4, 0, 'Clue #4', 12, 0, 100, 0, 0, 42383, ''),

(42391, 0, 0, 'You rich punks are all the same!', 12, 0, 100, 0, 0, 42384, ''),
(42391, 0, 1, 'Livin\' like an animal is hard enough without your insults!', 12, 0, 100, 0, 0, 42386, ''),
(42391, 0, 2, 'I ain\'t tellin\' you a damn thing!', 12, 0, 100, 0, 0, 42385, ''),
(42391, 0, 3, 'Now you\'ve gone and done it! TIME FOR THE FIST!', 12, 0, 100, 0, 0, 42389, ''),
(42391, 0, 4, 'Maybe I can sell your liver for some gold.', 12, 0, 100, 0, 0, 42387, ''),
(42391, 0, 5, 'I TOLD YOU TO PAY UP, PAL!', 12, 0, 100, 0, 0, 42388, ''),
(42391, 1, 0, 'Clue #1', 12, 0, 100, 0, 0, 42380, ''),
(42391, 2, 0, 'Clue #2', 12, 0, 100, 0, 0, 42381, ''),
(42391, 3, 0, 'Clue #3', 12, 0, 100, 0, 0, 42382, ''),
(42391, 4, 0, 'Clue #4', 12, 0, 100, 0, 0, 42383, ''),

(42383, 0, 0, 'You rich punks are all the same!', 12, 0, 100, 0, 0, 42384, ''),
(42383, 0, 1, 'Livin\' like an animal is hard enough without your insults!', 12, 0, 100, 0, 0, 42386, ''),
(42383, 0, 2, 'I ain\'t tellin\' you a damn thing!', 12, 0, 100, 0, 0, 42385, ''),
(42383, 0, 3, 'Now you\'ve gone and done it! TIME FOR THE FIST!', 12, 0, 100, 0, 0, 42389, ''),
(42383, 0, 4, 'Maybe I can sell your liver for some gold.', 12, 0, 100, 0, 0, 42387, ''),
(42383, 0, 5, 'I TOLD YOU TO PAY UP, PAL!', 12, 0, 100, 0, 0, 42388, ''),
(42383, 1, 0, 'Clue #1', 12, 0, 100, 0, 0, 42380, ''),
(42383, 2, 0, 'Clue #2', 12, 0, 100, 0, 0, 42381, ''),
(42383, 3, 0, 'Clue #3', 12, 0, 100, 0, 0, 42382, ''),
(42383, 4, 0, 'Clue #4', 12, 0, 100, 0, 0, 42383, '');


-- https://forum.wowcircle.com/showthread.php?t=1050315
update quest_template set prevquestid = 26627 where id = 26653;




-- ==================================================
-- https://forum.wowcircle.net/showthread.php?t=849650
-- Рейд Огненные Просторы: Исправлен лут боссов в зависимости от сложности рейда:
 -- Бет'тилак https://ru.wowhead.com/npc=52498
 -- Повелитель Риолит https://ru.wowhead.com/npc=52558
 -- Алисразор https://ru.wowhead.com/npc=52530
 -- Шэннокс https://ru.wowhead.com/npc=53691
 -- Бейлрок https://ru.wowhead.com/npc=53494
 -- Мажордом https://ru.wowhead.com/npc=52571
-- ==================================================
-- == 52498 - Beth'tilac == --
DELETE FROM creature_template_difficulty WHERE entry = 52498;	-- 5990t
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52498,'LEGACY_MAN25_DIFFICULTY',53576);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52498,'LEGACY_MAN10_HEROIC_DIFFICULTY',53577);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52498,'LEGACY_MAN25_HEROIC_DIFFICULTY',53578);
UPDATE creature_template SET lootid = entry, dmg_multiplier = 35.3 WHERE entry IN (52498,53576,53577,53578);
-- --
DELETE FROM reference_loot_template WHERE entry IN (34390,34391,34394,71783);
INSERT INTO `reference_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(34390,68981,0,1,1,1,1),
(34390,70914,0,1,1,1,1),
(34390,70922,0,1,1,1,1),
(34390,71029,0,1,1,1,1),
(34390,71030,0,1,1,1,1),
(34390,71031,0,1,1,1,1),
(34390,71032,0,1,1,1,1),
(34390,71038,0,1,1,1,1),
(34390,71039,0,1,1,1,1),
(34390,71040,0,1,1,1,1),
(34390,71041,0,1,1,1,1),
(34390,71042,0,1,1,1,1),
(34390,71043,0,1,1,1,1),
(34390,71044,0,1,1,1,1),
-- --
(34391,69138,0,1,1,1,1),
(34391,71401,0,1,1,1,1),
(34391,71402,0,1,1,1,1),
(34391,71403,0,1,1,1,1),
(34391,71404,0,1,1,1,1),
(34391,71405,0,1,1,1,1),
(34391,71406,0,1,1,1,1),
(34391,71407,0,1,1,1,1),
(34391,71408,0,1,1,1,1),
(34391,71409,0,1,1,1,1),
(34391,71410,0,1,1,1,1),
(34391,71411,0,1,1,1,1),
(34391,71412,0,1,1,1,1),
(34391,71413,0,1,1,1,1),
-- --
(34394,71775,0,1,1,1,1),
(34394,71776,0,1,1,1,1),
(34394,71779,0,1,1,1,1),
(34394,71780,0,1,1,1,1),
(34394,71782,0,1,1,1,1),
(34394,71785,0,1,1,1,1),
(34394,71787,0,1,1,1,1),
-- --
(71783,71774,0,1,1,1,1),
(71783,71777,0,1,1,1,1),
(71783,71778,0,1,1,1,1),
(71783,71781,0,1,1,1,1),
(71783,71783,0,1,1,1,1),
(71783,71784,0,1,1,1,1),
(71783,71786,0,1,1,1,1);
-- --
DELETE FROM creature_loot_template WHERE entry IN (52498,53576,53577,53578);
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(52498,1,100,1,0,-34390,2),
(52498,2,2,1,0,-34394,1),
(52498,69237,50,1,0,1,2),
(52498,69815,-100,1,0,15,25),
(52498,71141,-75,1,0,1,1),
-- --
(53576,1,100,1,0,-34390,5),
(53576,2,2,1,0,-34394,1),
(53576,69237,50,1,0,1,2),
(53576,69815,-100,1,0,45,55),
(53576,71141,-75,1,0,1,3),
-- --
(53577,1,100,1,0,-34391,2),
(53577,2,2,1,0,-71783,1),
(53577,69237,50,1,0,1,2),
(53577,69815,-100,1,0,23,26),
(53577,71141,-75,1,0,1,3),
(53577,71617,15,1,0,1,1),
-- --
(53578,1,100,1,0,-34391,6),
(53578,2,2,1,0,-71783,1),
(53578,69237,50,1,0,1,2),
(53578,69815,-100,1,0,55,66),
(53578,71141,-75,1,0,1,3),
(53578,71617,15,1,0,1,1);

-- == 52530 - Alysrazor == --
DELETE FROM creature_template_difficulty WHERE entry = 52530;	-- 52530, 54044, 54045, 54046
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52530,'LEGACY_MAN25_DIFFICULTY',54044);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52530,'LEGACY_MAN10_HEROIC_DIFFICULTY',54045);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52530,'LEGACY_MAN25_HEROIC_DIFFICULTY',54046);
UPDATE creature_template SET lootid = entry, dmg_multiplier = 17.7 WHERE entry IN (52530, 54044, 54045, 54046);
-- --
DELETE FROM reference_loot_template WHERE entry IN (34394,70733,71430,71686,71783);
INSERT INTO `reference_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(34394,71775,0,1,1,1,1),
(34394,71776,0,1,1,1,1),
(34394,71779,0,1,1,1,1),
(34394,71780,0,1,1,1,1),
(34394,71782,0,1,1,1,1),
(34394,71785,0,1,1,1,1),
(34394,71787,0,1,1,1,1),
-- --
(70733,68983,0,1,1,1,1),
(70733,70733,0,1,1,1,1),
(70733,70734,0,1,1,1,1),
(70733,70735,0,1,1,1,1),
(70733,70736,0,1,1,1,1),
(70733,70737,0,1,1,1,1),
(70733,70738,0,1,1,1,1),
(70733,70739,0,1,1,1,1),
(70733,70985,0,1,1,1,1),
(70733,70986,0,1,1,1,1),
(70733,70987,0,1,1,1,1),
(70733,70988,0,1,1,1,1),
(70733,70989,0,1,1,1,1),
(70733,70990,0,1,1,1,1),
-- --
(71430,69149,0,1,1,1,1),
(71430,71427,0,1,1,1,1),
(71430,71428,0,1,1,1,1),
(71430,71429,0,1,1,1,1),
(71430,71430,0,1,1,1,1),
(71430,71431,0,1,1,1,1),
(71430,71432,0,1,1,1,1),
(71430,71433,0,1,1,1,1),
(71430,71434,0,1,1,1,1),
(71430,71435,0,1,1,1,1),
(71430,71436,0,1,1,1,1),
(71430,71437,0,1,1,1,1),
(71430,71438,0,1,1,1,1),
(71430,71439,0,1,1,1,1),
-- --
(71686,71672,0,1,1,1,1),
(71686,71679,0,1,1,1,1),
(71686,71686,0,1,1,1,1),
-- --
(71783,71774,0,1,1,1,1),
(71783,71777,0,1,1,1,1),
(71783,71778,0,1,1,1,1),
(71783,71781,0,1,1,1,1),
(71783,71783,0,1,1,1,1),
(71783,71784,0,1,1,1,1),
(71783,71786,0,1,1,1,1);
-- --
DELETE FROM creature_loot_template WHERE entry IN (52530, 54044, 54045, 54046);
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(52530,2,2,1,0,-34394,1),
(52530,69237,50,1,0,1,2),
(52530,69815,-100,1,0,15,25),
(52530,70733,100,1,0,-70733,2),
(52530,71141,-75,1,0,1,1),
-- --
(54044,2,2,1,0,-34394,1),
(54044,69237,50,1,0,1,2),
(54044,69815,-100,1,0,45,55),
(54044,70733,100,1,0,-70733,5),
(54044,71141,-75,1,0,1,3),
(54044,71665,2,1,0,1,1),
-- --
(54045,2,2,1,0,-71783,1),
(54045,69237,50,1,0,1,2),
(54045,69815,-100,1,0,23,26),
(54045,71141,-75,1,0,1,1),
(54045,71430,100,1,0,-71430,2),
(54045,71617,15,1,0,1,1),
(54045,71665,4,1,0,1,1),
(54045,71686,100,1,0,-71686,1),
-- --
(54046,2,2,1,0,-71783,1),
(54046,69237,50,1,0,1,2),
(54046,69815,-100,1,0,55,66),
(54046,71141,-75,1,0,1,3),
(54046,71430,100,1,0,-71430,6),
(54046,71617,15,1,0,1,1),
(54046,71665,4,1,0,1,1),
(54046,71686,100,1,0,-71686,3);

-- == 52558 - Lord Rhyolith == -- 52558,52559,52560,52561
DELETE FROM creature_template_difficulty WHERE entry = 52558;	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52558,'LEGACY_MAN25_DIFFICULTY',52559);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52558,'LEGACY_MAN10_HEROIC_DIFFICULTY',52560);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52558,'LEGACY_MAN25_HEROIC_DIFFICULTY',52561);
UPDATE creature_template SET lootid = entry, dmg_multiplier = 21.9 WHERE entry IN (52558,52559,52560,52561);
-- --
DELETE FROM reference_loot_template WHERE entry IN (34392,34393,34394,71783);
INSERT INTO `reference_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(34392,70912,0,1,1,1,1),
(34392,70991,0,1,1,1,1),
(34392,70992,0,1,1,1,1),
(34392,70993,0,1,1,1,1),
(34392,71003,0,1,1,1,1),
(34392,71004,0,1,1,1,1),
(34392,71005,0,1,1,1,1),
(34392,71006,0,1,1,1,1),
(34392,71007,0,1,1,1,1),
(34392,71009,0,1,1,1,1),
(34392,71010,0,1,1,1,1),
(34392,71011,0,1,1,1,1),
(34392,71012,0,1,1,1,1),
-- --
(34393,71414,0,1,1,1,1),
(34393,71415,0,1,1,1,1),
(34393,71416,0,1,1,1,1),
(34393,71417,0,1,1,1,1),
(34393,71418,0,1,1,1,1),
(34393,71419,0,1,1,1,1),
(34393,71420,0,1,1,1,1),
(34393,71421,0,1,1,1,1),
(34393,71422,0,1,1,1,1),
(34393,71423,0,1,1,1,1),
(34393,71424,0,1,1,1,1),
(34393,71425,0,1,1,1,1),
(34393,71426,0,1,1,1,1),
-- --
(34394,71775,0,1,1,1,1),
(34394,71776,0,1,1,1,1),
(34394,71779,0,1,1,1,1),
(34394,71780,0,1,1,1,1),
(34394,71782,0,1,1,1,1),
(34394,71785,0,1,1,1,1),
(34394,71787,0,1,1,1,1),
-- --
(71783,71774,0,1,1,1,1),
(71783,71777,0,1,1,1,1),
(71783,71778,0,1,1,1,1),
(71783,71781,0,1,1,1,1),
(71783,71783,0,1,1,1,1),
(71783,71784,0,1,1,1,1),
(71783,71786,0,1,1,1,1);
-- --
DELETE FROM creature_loot_template WHERE entry IN (52558,52559,52560,52561);
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(52558,1,100,1,0,-34392,2),
(52558,2,2,1,0,-34394,1),
(52558,69237,50,1,0,1,2),
(52558,69815,-100,1,0,15,25),
(52558,71141,-75,1,0,1,1),
-- --
(52559,1,100,1,0,-34392,5),
(52559,2,2,1,0,-34394,1),
(52559,69237,50,1,0,1,2),
(52559,69815,-100,1,0,45,55),
(52559,71141,-75,1,0,1,3),
-- --
(52560,1,100,1,0,-34393,2),
(52560,2,2,1,0,-71783,1),
(52560,69237,50,1,0,1,2),
(52560,69815,-100,1,0,23,26),
(52560,71141,-75,1,0,1,3),
(52560,71617,15,1,0,1,1),
-- -- 
(52561,1,100,1,0,-34393,6),
(52561,2,2,1,0,-71783,1),
(52561,69237,50,1,0,1,2),
(52561,69815,-100,1,0,55,66),
(52561,71141,-75,1,0,1,3),
(52561,71617,15,1,0,1,1);

-- == 52571 - Majordomo Staghelm == -- 52571,53856,53857,53858
DELETE FROM creature_template_difficulty WHERE entry = 52571;	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52571,'LEGACY_MAN25_DIFFICULTY',53856);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52571,'LEGACY_MAN10_HEROIC_DIFFICULTY',53857);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (52571,'LEGACY_MAN25_HEROIC_DIFFICULTY',53858);
UPDATE creature_template SET lootid = entry, dmg_multiplier = 31.6 WHERE entry IN (52571,53856,53857,53858);
DELETE FROM reference_loot_template WHERE entry IN (34394,71348,71470,71473,71674,71680,71783);
INSERT INTO `reference_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(34394,71775,0,1,1,1,1),
(34394,71776,0,1,1,1,1),
(34394,71779,0,1,1,1,1),
(34394,71780,0,1,1,1,1),
(34394,71782,0,1,1,1,1),
(34394,71785,0,1,1,1,1),
(34394,71787,0,1,1,1,1),
(71348,68926,0,1,1,1,1),
(71348,68927,0,1,1,1,1),
(71348,69897,0,1,1,1,1),
(71348,70920,0,1,1,1,1),
(71348,71313,0,1,1,1,1),
(71348,71344,0,1,1,1,1),
(71348,71346,0,1,1,1,1),
(71348,71347,0,1,1,1,1),
(71348,71348,0,1,1,1,1),
(71348,71349,0,1,1,1,1),
(71348,71350,0,1,1,1,1),
(71348,71351,0,1,1,1,1),
(71470,71467,0,1,1,1,1),
(71470,71468,0,1,1,1,1),
(71470,71469,0,1,1,1,1),
(71470,71470,0,1,1,1,1),
(71470,71471,0,1,1,1,1),
(71470,71472,0,1,1,1,1),
(71470,71474,0,1,1,1,1),
(71470,71475,0,1,1,1,1),
(71473,69111,0,1,1,1,1),
(71473,69112,0,1,1,1,1),
(71473,71466,0,1,1,1,1),
(71473,71473,0,1,1,1,1),
(71674,71674,0,1,1,1,1),
(71674,71681,0,1,1,1,1),
(71674,71688,0,1,1,1,1),
(71680,71673,0,1,1,1,1),
(71680,71680,0,1,1,1,1),
(71680,71687,0,1,1,1,1),
(71783,71774,0,1,1,1,1),
(71783,71777,0,1,1,1,1),
(71783,71778,0,1,1,1,1),
(71783,71781,0,1,1,1,1),
(71783,71783,0,1,1,1,1),
(71783,71784,0,1,1,1,1),
(71783,71786,0,1,1,1,1);
-- --
DELETE FROM creature_loot_template WHERE entry IN (52571,53856,53857,53858);
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(52571,2,2,1,0,-34394,1),
(52571,69237,50,1,0,1,2),
(52571,69815,-100,1,0,15,25),
(52571,71141,-75,1,0,1,1),
(52571,71348,100,1,0,-71348,2),
(52571,71674,100,1,0,-71674,1),
(52571,122304,0.5,1,0,1,1),
-- --
(53856,2,2,1,0,-34394,1),
(53856,69237,50,1,0,1,2),
(53856,69815,-100,1,0,45,55),
(53856,71141,-75,1,0,1,3),
(53856,71348,100,1,0,-71348,5),
(53856,71674,100,1,0,-71674,3),
(53856,122304,0.5,1,0,1,1),
-- --
(53857,2,2,1,0,-71783,1),
(53857,69237,50,1,0,1,2),
(53857,69815,-100,1,0,23,26),
(53857,71141,-75,1,0,1,1),
(53857,71470,100,1,0,-71470,1),
(53857,71473,100,1,0,-71473,1),
(53857,71617,15,1,0,1,1),
(53857,71680,100,1,0,-71680,1),
(53857,122304,0.5,1,0,1,1),
-- --
(53858,2,2,1,0,-71783,1),
(53858,69237,50,1,0,1,2),
(53858,69815,-100,1,0,55,66),
(53858,71141,-75,1,0,1,3),
(53858,71470,100,1,0,-71470,5),
(53858,71473,100,1,0,-71473,1),
(53858,71617,15,1,0,1,1),
(53858,71680,100,1,0,-71680,3),
(53858,122304,0.5,1,0,1,1);
-- --
UPDATE creature_loot_template SET requiredClass = 11 WHERE item = 122304;

-- == 53494 - Baleroc == -- 53494,53587,53588,53589
DELETE FROM creature_template_difficulty WHERE entry = 53494;	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (53494,'LEGACY_MAN25_DIFFICULTY',53587);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (53494,'LEGACY_MAN10_HEROIC_DIFFICULTY',53588);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (53494,'LEGACY_MAN25_HEROIC_DIFFICULTY',53589);
UPDATE creature_template SET lootid = entry, dmg_multiplier = 97 WHERE entry IN (53494,53587,53588,53589);
-- --
DELETE FROM reference_loot_template WHERE entry IN (34394,71314,71461,71683,71783);
INSERT INTO `reference_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(34394,71775,0,1,1,1,1),
(34394,71776,0,1,1,1,1),
(34394,71779,0,1,1,1,1),
(34394,71780,0,1,1,1,1),
(34394,71782,0,1,1,1,1),
(34394,71785,0,1,1,1,1),
(34394,71787,0,1,1,1,1),
-- --
(71314,68982,0,1,1,1,1),
(71314,70915,0,1,1,1,1),
(71314,70916,0,1,1,1,1),
(71314,70917,0,1,1,1,1),
(71314,71312,0,1,1,1,1),
(71314,71314,0,1,1,1,1),
(71314,71315,0,1,1,1,1),
(71314,71323,0,1,1,1,1),
(71314,71340,0,1,1,1,1),
(71314,71341,0,1,1,1,1),
(71314,71342,0,1,1,1,1),
(71314,71343,0,1,1,1,1),
(71314,71345,0,1,1,1,1),
-- --
(71461,69139,0,1,1,1,1),
(71461,71454,0,1,1,1,1),
(71461,71455,0,1,1,1,1),
(71461,71456,0,1,1,1,1),
(71461,71457,0,1,1,1,1),
(71461,71458,0,1,1,1,1),
(71461,71459,0,1,1,1,1),
(71461,71460,0,1,1,1,1),
(71461,71461,0,1,1,1,1),
(71461,71462,0,1,1,1,1),
(71461,71463,0,1,1,1,1),
(71461,71464,0,1,1,1,1),
(71461,71465,0,1,1,1,1),
-- --
(71683,71669,0,1,1,1,1),
(71683,71676,0,1,1,1,1),
(71683,71683,0,1,1,1,1),
(71783,71774,0,1,1,1,1),
(71783,71777,0,1,1,1,1),
(71783,71778,0,1,1,1,1),
(71783,71781,0,1,1,1,1),
(71783,71783,0,1,1,1,1),
(71783,71784,0,1,1,1,1),
(71783,71786,0,1,1,1,1);
-- --
DELETE FROM creature_loot_template WHERE entry IN (53494,53587,53588,53589);
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(53494,2,2,1,0,-34394,1),
(53494,69237,50,1,0,1,2),
(53494,69815,-100,1,0,15,25),
(53494,71141,-75,1,0,1,1),
(53494,71314,100,1,0,-71314,2),
-- --
(53587,2,2,1,0,-34394,1),
(53587,69237,50,1,0,1,2),
(53587,69815,-100,1,0,45,55),
(53587,71141,-75,1,0,1,3),
(53587,71314,100,1,0,-71314,5),
-- --
(53588,2,2,1,0,-71783,1),
(53588,69237,50,1,0,1,2),
(53588,69815,-100,1,0,23,26),
(53588,71141,-75,1,0,1,1),
(53588,71461,100,1,0,-71461,2),
(53588,71617,15,1,0,1,1),
(53588,71683,100,1,0,-71683,1),
-- --
(53589,2,2,1,0,-71783,1),
(53589,69237,50,1,0,1,2),
(53589,69815,-100,1,0,55,66),
(53589,71141,-75,1,0,1,3),
(53589,71461,100,1,0,-71461,6),
(53589,71617,15,1,0,1,1),
(53589,71683,100,1,0,-71683,3);

-- == 53691 - Shannox == -- 53691,53979,54079,54080
DELETE FROM creature_template_difficulty WHERE entry = 53691;	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (53691,'LEGACY_MAN25_DIFFICULTY',53979);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (53691,'LEGACY_MAN10_HEROIC_DIFFICULTY',54079);	
INSERT INTO creature_template_difficulty (entry,difficulty,difficulty_entry) VALUES (53691,'LEGACY_MAN25_HEROIC_DIFFICULTY',54080);
UPDATE creature_template SET lootid = entry, dmg_multiplier = 97 WHERE entry IN (53691,53979,54079,54080);
-- --
DELETE FROM reference_loot_template WHERE entry IN (34394,34395,34396,71671,71783);
INSERT INTO `reference_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(34394,71775,0,1,1,1,1),
(34394,71776,0,1,1,1,1),
(34394,71779,0,1,1,1,1),
(34394,71780,0,1,1,1,1),
(34394,71782,0,1,1,1,1),
(34394,71785,0,1,1,1,1),
(34394,71787,0,1,1,1,1),
-- --
(34395,70913,0,1,1,1,1),
(34395,71013,0,1,1,1,1),
(34395,71014,0,1,1,1,1),
(34395,71018,0,1,1,1,1),
(34395,71019,0,1,1,1,1),
(34395,71020,0,1,1,1,1),
(34395,71021,0,1,1,1,1),
(34395,71022,0,1,1,1,1),
(34395,71023,0,1,1,1,1),
(34395,71024,0,1,1,1,1),
(34395,71025,0,1,1,1,1),
(34395,71026,0,1,1,1,1),
(34395,71027,0,1,1,1,1),
(34395,71028,0,1,1,1,1),
-- --
(34396,71440,0,1,1,1,1),
(34396,71441,0,1,1,1,1),
(34396,71442,0,1,1,1,1),
(34396,71443,0,1,1,1,1),
(34396,71444,0,1,1,1,1),
(34396,71445,0,1,1,1,1),
(34396,71446,0,1,1,1,1),
(34396,71447,0,1,1,1,1),
(34396,71448,0,1,1,1,1),
(34396,71449,0,1,1,1,1),
(34396,71450,0,1,1,1,1),
(34396,71451,0,1,1,1,1),
(34396,71452,0,1,1,1,1),
(34396,71453,0,1,1,1,1),
-- --
(71671,71671,0,1,1,1,1),
(71671,71678,0,1,1,1,1),
(71671,71685,0,1,1,1,1),
-- --
(71783,71774,0,1,1,1,1),
(71783,71777,0,1,1,1,1),
(71783,71778,0,1,1,1,1),
(71783,71781,0,1,1,1,1),
(71783,71783,0,1,1,1,1),
(71783,71784,0,1,1,1,1),
(71783,71786,0,1,1,1,1);
-- --
DELETE FROM creature_loot_template WHERE entry IN (53691,53979,54079,54080);
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`) VALUES 
(53691,1,100,1,0,-34395,2),
(53691,2,2,1,0,-34394,1),
(53691,69237,50,1,0,1,2),
(53691,69815,-100,1,0,15,25),
(53691,71141,-75,1,0,1,1),
(53691,71716,15,1,0,1,1),
-- --
(53979,1,100,1,0,-34395,5),
(53979,2,2,1,0,-34394,1),
(53979,69237,50,1,0,1,2),
(53979,69815,-100,1,0,45,55),
(53979,71141,-75,1,0,1,3),
(53979,71716,15,1,0,1,1),
-- --
(54079,1,100,1,0,-34396,2),
(54079,2,2,1,0,-71783,1),
(54079,69237,50,1,0,1,2),
(54079,69815,-100,1,0,23,26),
(54079,71141,-75,1,0,1,3),
(54079,71617,15,1,0,1,1),
(54079,71671,100,1,0,-71671,1),
(54079,71716,25,1,0,1,1),
-- --
(54080,1,100,1,0,-34396,6),
(54080,2,2,1,0,-71783,1),
(54080,69237,50,1,0,1,2),
(54080,69815,-100,1,0,55,66),
(54080,71141,-75,1,0,1,3),
(54080,71617,15,1,0,1,1),
(54080,71671,100,1,0,-71671,3),
(54080,71716,25,1,0,1,1);