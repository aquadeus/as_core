-- https://forum.wowcircle.net/showthread.php?t=830913
delete from spell_loot_template where entry in (247761);
INSERT INTO `spell_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(247761, 129100, 100, 0, 0, 4, 4),
(247761, 104, 30, 0, 0, -104, 1);

delete from reference_loot_template where entry in (104);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(104, 151579, 0, 0, 0, 1, 1),
(104, 151718, 0, 0, 0, 1, 1),
(104, 151719, 0, 0, 0, 1, 1),
(104, 151720, 0, 0, 0, 1, 1),
(104, 151721, 0, 0, 0, 1, 1),
(104, 151722, 0, 0, 0, 1, 1);


-- https://forum.wowcircle.net/showthread.php?t=916224
delete from gossip_menu_option where menu_id in (7820);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(7820, 0, 1, 'I want to browse your goods', 3, 128, 2823),
(7820, 1, 3, 'Train me!', 5, 16, 3266);


-- https://forum.wowcircle.net/showthread.php?t=1037645
update creature_template set ainame = 'SmartAI', npcflag = 0 where entry in (59307);

delete from smart_scripts where entryorguid in (59307); 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(59307, 0, 0, 0, 8, 0, 100, 1, 115897, 0, 0, 0, 53, 1, 59307, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Yak - on spell hit - start waypoint'),
(59307, 0, 1, 0, 40, 0, 100, 1, 2, 59307, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Yak - on waypoint reached - despawn');

delete from waypoints where entry in (59307); 
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(59307, 1, 3051.81, 746.620, 527.487),
(59307, 2, 3043.81, 748.663, 527.483);

DELETE FROM `spell_area` WHERE `spell` = 115897 AND `area` = 6079;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(115897, 6079, 30492, 30492, 0, 0, 2, 1, 8, 66);

delete from conditions where SourceEntry in (115897) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 2, 115897, 0, 0, 31, 0, 3, 59307, 0, 0, 0, '', 'Cast only 59307');

update quest_template set sourcespellid = 116473 where id = 30492;


-- https://forum.wowcircle.net/showthread.php?t=1040878
update creature_template set unit_flags = 768, flags_extra = 128, ainame = '' where entry in (25402,25403,25404,25405);
update creature_template set unit_flags = 768 where entry in (25401);
delete from smart_scripts where entryorguid in (25402,25403,25404,25405);


delete from conditions where SourceEntry in (45502) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 3, 45502, 0, 0, 31, 0, 3, 25401, 0, 1, 0, '', 'Cast only not 25401'),
(13, 3, 45502, 0, 0, 31, 0, 3, 25402, 0, 1, 0, '', 'Cast only not 25402'),
(13, 3, 45502, 0, 0, 31, 0, 3, 25403, 0, 1, 0, '', 'Cast only not 25403'),
(13, 3, 45502, 0, 0, 31, 0, 3, 25404, 0, 1, 0, '', 'Cast only not 25404'),
(13, 3, 45502, 0, 0, 31, 0, 3, 25405, 0, 1, 0, '', 'Cast only not 25405');


-- https://forum.wowcircle.net/showthread.php?t=1040773
update creature_template set ainame = 'SmartAI', gossip_menu_id = 18905 where entry in (98725);
update creature_template set ainame = 'SmartAI', gossip_menu_id = 18907 where entry in (115287);

delete from gossip_menu_option where menu_id in (18907,18905);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES 
(18905, 0, 0, 'I\'d like to heal and revive my battle pets.', 1, 1, 0, 0, 0, 0, NULL, 64115, 0, 0),
(18905, 1, 0, 'Why are you called the Lioness?', 1, 1, 18906, 0, 0, 0, NULL, 87407, 0, 0),
(18905, 2, 1, 'Any pet stuff for sale?', 3, 128, 0, 0, 0, 0, NULL, 87415, 0, 0),
(18905, 3, 0, 'Namha told me you had some new pet toy ideas?', 1, 1, 34766, 0, 0, 0, NULL, 101722, 0, 0),

(18907, 0, 0, 'I\'d like to heal and revive my battle pets.', 1, 1, 0, 0, 0, 0, NULL, 64115, 0, 0),
(18907, 2, 1, 'Any pet stuff for sale?', 3, 128, 0, 0, 0, 0, NULL, 87415, 0, 0),
(18907, 3, 0, 'Namha told me you had some new pet toy ideas?', 1, 1, 34766, 0, 0, 0, NULL, 101722, 0, 0);

delete from smart_scripts where entryorguid in (98725,115287); 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(98725, 0, 0, 0, 62, 0, 100, 0, 18905, 3, 0, 0, 33, 99199, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Lio - on gossip select - give credit'),
(115287, 0, 0, 0, 62, 0, 100, 0, 18907, 3, 0, 0, 33, 99199, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Serra - on gossip select - give credit');

delete from conditions where SourceGroup in (18907,18905) and SourceTypeOrReferenceId = 15 and sourceentry = 3;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 18905, 3, 0, 0, 9, 0, 40200, 0, 0, 0, 0, '', 'Show Gossip if player has 40200 quest'),
(15, 18907, 3, 0, 0, 9, 0, 40241, 0, 0, 0, 0, '', 'Show Gossip if player has 40241 quest');


-- https://forum.wowcircle.net/showthread.php?t=661305
update creature_template set vehicleid = 989, spell1 = 81888, npcflag = 16777216, ainame = 'SmartAI', unit_flags = 0 where entry in (43734);
update creature_template set ainame = 'SmartAI' where entry in (43775);
update creature_template set npcflag = 16777216, unit_flags = 768 where entry in (43714);
update creature_template set vehicleid = 988, npcflag = 16777216 where entry in (43745);

delete from vehicle_template_accessory where entryoraura in (43734,43745);
INSERT INTO `vehicle_template_accessory` (`EntryOrAura`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `description`) VALUES 
(43734, 43745, 1, 1, 8, 0, ''),
(43745, 43744, 0, 1, 8, 0, '');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` in (43734,43745,43714);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(43714, 81808, 1, 0),
(43734, 46598, 1, 0),
(43745, 46598, 1, 0); 

delete from conditions where SourceGroup in (43714) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 43714, 81808, 0, 0, 9, 0, 26708, 0, 0, 0, 0, '', 'Allow spellclick if player has quest');

delete from creature where id in (43714);
INSERT IGNORE INTO creature (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(43714, 0, 44, 5324, 1, 1, 0, 0, 0, -9646.64, -3454.94, 117.485, 1.93731, 60, 0, 0, 0, 0, 0, 0, 0, 0);

delete from smart_scripts where entryorguid in (43734,43775); 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(43775, 0, 0, 0, 6, 0, 100, 0, 0, 0, 0, 0, 33, 43775, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Mob - on death - give credit'),

(43734, 0, 0, 1, 60, 0, 100, 0, 0, 0, 10000, 10000, 75, 81874, 0, 0, 0, 0, 0, 19, 43745, 0, 0, 0, 0, 0, 0, 'Tank - ic - add aura'),
(43734, 0, 1, 0, 61, 0, 30, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 43744, 0, 0, 0, 0, 0, 0, 'Tank - on link - talk');

delete from conditions where SourceEntry in (43734) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 43734, 0, 0, 29, 1, 43775, 30, 0, 0, 0, '', 'Smart if near 43775 creature');

-- delete from conditions where SourceEntry in (81870) and SourceTypeOrReferenceId = 13;
-- INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
-- (13, 1, 81870, 0, 0, 31, 0, 3, 43775, 0, 0, 0, '', 'Cast only 43775'),
-- (13, 1, 81870, 0, 1, 31, 0, 3, 429, 0, 0, 0, '', 'Cast only 43775'),
-- (13, 1, 81870, 0, 2, 31, 0, 3, 431, 0, 0, 0, '', 'Cast only 43775'),
-- (13, 1, 81870, 0, 3, 31, 0, 3, 568, 0, 0, 0, '', 'Cast only 43775'),
-- (13, 1, 81870, 0, 4, 31, 0, 3, 433, 0, 0, 0, '', 'Cast only 43775'),
-- (13, 1, 81870, 0, 5, 31, 0, 3, 432, 0, 0, 0, '', 'Cast only 43775'),
-- (13, 1, 81870, 0, 6, 31, 0, 3, 434, 0, 0, 0, '', 'Cast only 43775');

delete from creature_text where entry in (43744);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(43744, 0, 0, 'AAHHHHHHHHH!!!!', 14, 0, 100, 0, 0, 0, 43822),
(43744, 0, 1, 'AAHHHHHHHHHHHHHHHHHH!!!!', 14, 0, 100, 0, 0, 0, 43823),
(43744, 0, 2, 'AAAAAAAAAAAHHHHHHHHHHHHH!!!!', 14, 0, 100, 0, 0, 0, 43824),
(43744, 0, 3, 'BLOOD AND THUNDER, RIGHT!?', 14, 0, 100, 0, 0, 0, 43825),
(43744, 0, 4, 'I CAN\'T FIND YOUR LEGS, JOEY! I CAN\'T FIND YOUR DAMNED LEGS!', 14, 0, 100, 0, 0, 0, 43826),
(43744, 0, 5, 'AAAHHHH!', 14, 0, 100, 0, 0, 0, 43827),
(43744, 0, 6, 'RUN TO YOUR DARK GODS, YOU FILTHY ANIMALS!', 14, 0, 100, 0, 0, 0, 43828),
(43744, 0, 7, 'THERE ARE NO FRIENDLY CIVILIANS!', 14, 0, 100, 0, 0, 0, 43829);

delete from phase_definitions where zoneId = 44;
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(44, 1, 0, 751, 0, '26708 quest');

delete from conditions where SourceGroup in (44) and SourceTypeOrReferenceId = 26;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 44, 1, 0, 0, 9, 0, 26708, 0, 0, 0, 0, '', 'Activate phase if player has quest');

delete from creature where id = 43775;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9345.54, -3277.43, 93.442, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9337.34, -3229.1, 96.4974, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9164.48, -3231.65, 103.863, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9207.55, -3326.75, 103.87, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9354.77, -3280.97, 89.1927, 4.90438, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9127.94, -3321.34, 101.954, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9286.64, -3216.99, 103.643, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9145.05, -3213.44, 104.591, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9246.86, -3330.36, 100.665, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9178.73, -3336.89, 104.407, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9368.54, -3373.22, 89.4671, 4.27606, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9367.09, -3284.91, 88.5092, 5.15866, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9409.64, -3452.15, 96.6541, 3.65154, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9183.63, -3089.25, 96.02, 4.90438, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9174.1, -3088.11, 97.0812, 4.90438, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9187.46, -3091.38, 96.6448, 4.56754, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9373.05, -3336, 91.3901, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9370.36, -3305.42, 91.2201, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9359.01, -3356.42, 89.5515, 4.2237, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9305.55, -3203.17, 104.297, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9224.28, -3236.17, 101.394, 5.07891, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9151.42, -3261.89, 100.44, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9388.52, -3440.35, 92.8745, 3.53241, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9217.71, -3292.99, 100.645, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9343.21, -3341.18, 91.1825, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9166.68, -3103.24, 99.7124, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9359.31, -3410.78, 91.667, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9390.19, -3413.56, 90.0739, 3.15905, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9238.01, -3212.07, 101.294, 5.09636, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9393.9, -3442, 93.3975, 3.49053, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9326.17, -3356.03, 89.7875, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9275.25, -3380.79, 100.412, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9216.37, -3107.36, 107.399, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9095.2, -3236.75, 100.576, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9248.8, -3381.51, 102.367, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9315.23, -3390.24, 91.1435, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9367.21, -3284.67, 88.4956, 5.15866, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9182.07, -3275.93, 100.7, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9182.64, -3149.9, 98.6316, 5.06236, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9203.8, -3074.68, 99.8594, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9212.72, -3138.5, 105.42, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 70, 44, 1, 0, 751, 0, 0, -9187.3, -3090.32, 96.5402, 4.56754, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9322.68, -3374.75, 90.5005, 4.4981, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9315.42, -3336.18, 103.365, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9180.53, -3145.45, 98.6316, 5.20329, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9168.43, -3163.61, 101.349, 4.04916, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9215.23, -3374.18, 100.217, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9302.13, -3356.08, 107.135, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9411.25, -3396.15, 88.9633, 4.36332, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9393.41, -3373.34, 88.7495, 4.36332, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9269.44, -3347.52, 107.599, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9094.69, -3220.12, 101.502, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9380.52, -3434.2, 93.1971, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9405.83, -3379.12, 88.6653, 4.36332, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9375.09, -3403.22, 90.6029, 1.01965, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9380.29, -3453.93, 92.4473, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9402.91, -3391.56, 88.4249, 4.36332, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9367.59, -3283.86, 88.4508, 5.15866, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9322.85, -3375.54, 90.3218, 4.4981, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 1000, 44, 1, 0, 751, 0, 0, -9179.27, -3088.61, 96.1624, 4.90438, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9405.33, -3420.57, 89.7331, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9370.51, -3263.87, 88.4241, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9314.44, -3372.1, 93.7864, 4.90438, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9182.48, -3159.97, 99.2358, 4.63407, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9265.22, -3369.44, 100.322, 3.44539, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9259.36, -3365.69, 100.337, 3.65405, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9339.44, -3430.68, 95.2112, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9199.83, -3179.27, 103.138, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9178.09, -3155.97, 98.9773, 5.06182, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9390.44, -3442.86, 92.2985, 3.54228, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9307.14, -3379.74, 93.1598, 3.35201, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9393.91, -3388.25, 88.9348, 4.36332, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 5324, 44, 1, 0, 751, 0, 0, -9375.46, -3403.82, 90.7469, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43775, 0, 71, 44, 1, 0, 751, 0, 0, -9168.76, -3079.18, 100.661, 3.22886, 30, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature set spawndist = RAND()*(9-5+1)+5, movementtype = 1, spawntimesecs = 30 where id = 43775;