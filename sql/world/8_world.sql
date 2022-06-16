-- Расширим колонку event_flags
ALTER TABLE smart_scripts MODIFY event_flags int(10);

-- https://forum.wowcircle.net/showthread.php?t=854750
select max(guid) from creature into @CGUID;
 
DELETE FROM `spell_area` WHERE `spell` = 86739 AND `area` = 5474;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(86739, 5474, 27720, 27745, 0, 0, 2, 1, 66, 11),
(86739, 5474, 28885, 27745, 0, 0, 2, 1, 66, 11);
 
DELETE FROM `creature` WHERE  `id` = 46513;
 
-- ELM General Purpose Bunny 23837
DELETE FROM `creature` WHERE `guid` = @CGUID+0;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES 
(@CGUID+0, 23837, 0, 1, 65535, 0, 0, -5278.71, -4411.47, 402.900, 2.703514, 120, 0, 0, 1, 0, 0, 0, 0, 0);
 
-- Initiate Goldmine 46243
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE  `entry` = 46243;
 
DELETE FROM `smart_scripts` WHERE `entryorguid` = 46243 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(46243, 0, 0, 1, 62, 0, 100, 0, 12133, 0, 0, 0, 0, 85, 86625, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Initiate Goldminer - On Gossip Option 0 Selected - Cast Spell \'Mr. Goldmine\'s Wild Ride: Summon Mine Cart\''),
(46243, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 46472, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Initiate Goldminer - On Gossip Option 0 Selected - Quest Kill Credit'),
(46243, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Initiate Goldminer - On Gossip Option 0 Selected - Close Gossip'),
(46243, 0, 3, 0, 1, 0, 100, 0, 500, 1000, 60000, 60000, 0, 11, 79892, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Initiate Goldminer - Out Of Combat - Cast Spell \'Water Shield\''),
(46243, 0, 4, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 11, 78222, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Initiate Goldminer - On Respawn - Cast Spell \'Stoneskin Totem\'');
 
delete from gossip_menu_option where menu_id in (12133);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(12133, 0, 0, 'Mr. Goldmine, I\'m ready for that wild ride.', 1, 1, 46613);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 12133;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 12133, 0, 0, 0, 9, 0, 28885, 0, 0, 0, 0, '', ''),
(15, 12133, 0, 0, 1, 9, 0, 27720, 0, 0, 0, 0, '', '');
 
DELETE FROM `spell_target_position` WHERE `id` = 86625;
 
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 13 AND `SourceEntry` = 86625;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 86625, 0, 0, 31, 0, 3, 23837, @CGUID+0, 0, 0, '', '');
 
-- Initiate Goldminer 46472
UPDATE `creature_template` SET `faction` = 2167, `AIName` = 'SmartAI', `ScriptName` = '' WHERE  `entry` = 46472;
 
DELETE FROM `creature` WHERE  `id` = 46472;
 
DELETE FROM `creature_text` WHERE `entry` = 46472;
INSERT INTO `creature_text` (`entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `comment`) VALUES 
(46472, 0, 0, 'Ooh, this is gonna be good!', 12, 0, 100, 0, 0, 0, 46672, 'Initiate Goldminer'),
(46472, 1, 0, 'Hang on!', 12, 0, 100, 0, 0, 0, 46673, 'Initiate Goldminer'),
(46472, 2, 0, 'OH NO!', 14, 0, 100, 0, 0, 0, 46674, 'Initiate Goldminer'),
(46472, 3, 0, 'My totems!!!', 14, 0, 100, 0, 0, 0, 46675, 'Initiate Goldminer');
 
DELETE FROM `smart_scripts` WHERE `entryorguid` = 46472 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(46472, 0, 0, 0, 23, 0, 100, 512, 46598, 0, 0, 0, 0, 41, 2000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Initiate Goldminer - On Has Aura - Despawn');
 
update creature_template set vehicleid = 1193 where entry = 46485;

-- Initiate Goldminer 46485
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE  `entry` = 46513;
 
DELETE FROM `smart_scripts` WHERE `entryorguid` = 46513 AND `source_type` = 0;
 
-- Mine Cart 46485
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE  `entry` = 46485;
 
DELETE FROM `smart_scripts` WHERE `entryorguid` = 46485 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 46485*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(46485, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 11, 87955, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Just Summon - Cast Spell \'Mr. Goldmine\'s Wild Ride: Mine Cart Visual\''),
(46485, 0, 1, 2, 61, 0, 100, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Just Summon -  Set Reactstate Passive'),
(46485, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 18, 33555330, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Just Summon -  Set Unit Flag \'UNIT_FLAG_NON_ATTACKABLE, UNIT_FLAG_NOT_ATTACKABLE_1, UNIT_FLAG_IMMUNE_TO_NPC, UNIT_FLAG_IMMUNE_TO_PC, UNIT_FLAG_NOT_SELECTABLE\''),
(46485, 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 80, 46485*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Respawn - Run Script'),
(46485, 0, 4, 0, 40, 0, 100, 1, 1, 46485, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 9, 46472, 0, 20, 0, 0, 0, 0, 'Mine Cart - On Waypoint 1 Reached - Say Line 0 \'Initiate Goldminer\''),
(46485, 0, 5, 6, 40, 0, 100, 1, 2, 46485, 0, 0, 0, 201, 1, 40, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Waypoint 2 Reached - Set Speed Run 40'),
(46485, 0, 6, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 9, 46472, 0, 20, 0, 0, 0, 0, 'Mine Cart - On Waypoint 2 Reached - Say Line 1 \'Initiate Goldminer\''),
(46485, 0, 7, 0, 40, 0, 100, 1, 16, 46485, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 9, 46472, 0, 20, 0, 0, 0, 0, 'Mine Cart - On Waypoint 16 Reached - Say Line 2 \'Initiate Goldminer\''),
(46485, 0, 8, 0, 40, 0, 100, 1, 17, 46485, 0, 0, 0, 97, 25, 10, 0, 0, 0, 0, 1, 0, 0, 0, -5446.46, -4504.80, 331.78, 25.0, 'Mine Cart - On Waypoint 17 Reached - Jump To Pos'),
(46485, 0, 9, 10, 40, 0, 100, 1, 20, 46485, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 9, 46472, 0, 20, 0, 0, 0, 0, 'Mine Cart - On Waypoint 20 Reached - Say Line 3 \'Initiate Goldminer\''),
(46485, 0, 10, 11, 61, 0, 100, 1, 0, 0, 0, 0, 0, 28, 86626, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Waypoint 20 Reached - Remove Aura \'Riding Mine Cart\''),
(46485, 0, 11, 12, 61, 0, 100, 1, 0, 0, 0, 0, 0, 28, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Waypoint 20 Reached - Remove Aura \'Ride Vehicle Hardcoded\''),
(46485, 0, 12, 13, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 46459, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Waypoint 20 Reached - Quest Credit'),
(46485, 0, 13, 14, 61, 0, 100, 1, 0, 0, 0, 0, 0, 97, 25, 10, 0, 0, 0, 0, 1, 0, 0, 0, -5459.50, -4441.22, 335.08, 0, 'Mine Cart - On Waypoint 20 Reached - Jump To Pos'),
(46485, 0, 14, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 85, 86730, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Waypoint 20 Reached - Invoker Cast Spell \'Mr. Goldmine\'s Wild Ride: Summon Initiate Goldmine QG\''),
(46485, 0, 15, 0, 40, 0, 100, 1, 21, 46485, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Waypoint 21 Reached - Despawn'),
(46485*100, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 0, 53, 1, 46485, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Script - Start WP'),
(46485*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 4, 23546, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Script - Play Sound Music'),
(46485*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 4, 11743, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Mine Cart - On Script - Play Sound Music');
 
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 46485;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(46485, 46598, 1, 0); 
 
DELETE FROM `vehicle_template_accessory` WHERE `entryoraura` = 46485;
INSERT INTO `vehicle_template_accessory` (`entryoraura`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES 
(46485, 46472, 1, 0, 'Initiate Goldminer', 6, 30000);
 
DELETE FROM `waypoints` WHERE `entry` = 46485;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES 
(46485, 1, -5278.71, -4411.47, 402.90, 'Mine Cart'),
(46485, 2, -5305.24, -4393.17, 403.44, 'Mine Cart'),
(46485, 3, -5316.06, -4387.74, 404.93, 'Mine Cart'),
(46485, 4, -5325.66, -4384.84, 404.85, 'Mine Cart'),
(46485, 5, -5335.68, -4384.96, 403.65, 'Mine Cart'),
(46485, 6, -5346.22, -4388.00, 403.90, 'Mine Cart'),
(46485, 7, -5357.22, -4393.89, 404.84, 'Mine Cart'),
(46485, 8, -5366.16, -4401.39, 405.11, 'Mine Cart'),
(46485, 9, -5370.59, -4407.97, 404.07, 'Mine Cart'),
(46485, 10, -5372.99, -4419.39, 403.38, 'Mine Cart'),
(46485, 11, -5372.11, -4433.13, 400.81, 'Mine Cart'),
(46485, 12, -5375.50, -4455.85, 392.54, 'Mine Cart'),
(46485, 13, -5384.29, -4474.79, 382.34, 'Mine Cart'),
(46485, 14, -5391.25, -4482.82, 377.02, 'Mine Cart'),
(46485, 15, -5408.18, -4494.45, 367.93, 'Mine Cart'),
(46485, 16, -5421.70, -4500.32, 363.12, 'Mine Cart'),
(46485, 17, -5435.97, -4502.91, 362.23, 'Mine Cart'),
(46485, 18, -5446.46, -4504.80, 331.78, 'Mine Cart'),
(46485, 19, -5452.11, -4476.55, 329.00, 'Mine Cart'),
(46485, 20, -5454.05, -4464.82, 328.64, 'Mine Cart'),
(46485, 21, -5457.14, -4452.32, 328.28, 'Mine Cart'),
(46485, 22, -5459.50, -4441.22, 335.08, 'Mine Cart');
 
-- Earthcaller Yevaa 46242
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE  `entry` = 46242;
 
DELETE FROM `smart_scripts` WHERE `entryorguid` = 46242 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(46242, 0, 0, 0, 1, 0, 100, 0, 500, 1000, 60000, 60000, 0, 11, 31765, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Earthcaller Yevaa - Out Of Combat - Cast Spell \'Lightning Shield\'');


-- https://forum.wowcircle.net/showthread.php?t=1033931
-- https://forum.wowcircle.net/showthread.php?t=880732&p=6909155#post6909155
update creature_template set ainame = 'SmartAI' where entry in (107389,107535);

delete from smart_scripts where entryorguid in (107389,107535);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(107389, 0, 0, 1, 38, 0, 100, 0, 1, 1, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Druid - on data set - remove aura'),
(107389, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 5000, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'Druid - on link - talk'),
(107389, 0, 2, 3, 52, 0, 100, 0, 0, 107389, 0, 0, 0, 89, 20, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Druid - on text over - random movement'),
(107389, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Druid - on link - despawn'),

(107535, 0, 0, 0, 1, 0, 100, 0, 0, 0, 10000, 10000, 0, 11, 213527, 0, 0, 0, 0, 0, 19, 107389, 20, 0, 0, 0, 0, 0, 'Eredar - ooc - cast spell'),
(107535, 0, 1, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 107389, 0, 0, 0, 0, 0, 0, 'Eredar - on death - set data'),
(107535, 0, 2, 0, 4, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Eredar - on aggro - talk'),
(107535, 0, 3, 0, 0, 0, 100, 0, 5000, 5000, 5000, 10000, 0, 11, 180395, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Eredar - ic - cast spell');

delete from creature where id in (107389,107535);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`) VALUES 
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3532.47, 5686.87, 313.109, 4.51741, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3454.32, 5727.83, 309.217, 1.82605, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 8164, 1, 1, 0, 0, 0, 3470.47, 5581.67, 318.931, 4.31897, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3583.88, 5688.55, 319.304, 4.65, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3473.12, 5683.46, 308.579, 1.46073, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3436.79, 5695.45, 310.825, 0.739331, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3403.88, 5671.96, 312.767, 4.68756, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 8266, 1, 1, 0, 0, 0, 3254.01, 5727.62, 321.202, 3.52112, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3400.65, 5569.06, 311.362, 1.6636, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3552.26, 5654.06, 312.964, 2.46867, 180, 0, 0, 0, 0, 0, 0),
(107389, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3451.77, 5645.42, 309.286, 2.4467, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3582.53, 5683.63, 318.426, 1.00841, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3549, 5657.98, 312.643, 5.11026, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3530.49, 5682.17, 311.757, 0.875809, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3454.06, 5732.91, 311.656, 4.46763, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 8164, 1, 1, 0, 0, 0, 3467.6, 5577.44, 319.539, 0.67738, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3448.59, 5649.42, 309.339, 5.08829, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3474.69, 5688.33, 309.085, 4.10232, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3404.19, 5666.47, 313.285, 1.73909, 180, 0, 0, 0, 0, 0, 0),
(107535, 1220, 7558, 7710, 1, 1, 0, 0, 0, 3441.18, 5698.06, 310.885, 3.38092, 180, 0, 0, 0, 0, 0, 0);

delete from creature_text where entry in (107389,107535);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(107389, 0, 0, 'Спасибо тебе, $gгерой:воительница;!', 12, 0, 100, 0, 0, 0, 22649),
(107389, 0, 1, 'Спасибо, спасибо тебе!', 12, 0, 100, 0, 0, 0, 22645),
(107389, 0, 2, 'Спасибо. Я этого не забуду.', 12, 0, 100, 0, 0, 0, 22646),

(107535, 0, 0, 'Твоя смерть будет быстрой и мучительной!', 12, 0, 100, 0, 0, 0, 105124),
(107535, 0, 1, 'Познай истинную мощь!', 12, 0, 100, 0, 0, 0, 105125),
(107535, 0, 2, 'Как ты смеешь?!', 12, 0, 100, 0, 0, 0, 105126),
(107535, 0, 3, 'Моя родина – Аргус, величайший из миров Великой Тьмы.', 12, 0, 100, 0, 0, 0, 105127),
(107535, 0, 4, 'Вкуси магии Скверны!', 12, 0, 100, 0, 0, 0, 105128);


-- https://forum.wowcircle.net/showthread.php?t=1038122
update quest_template set prevquestid = 35003 where id = 35012;


-- https://forum.wowcircle.net/showthread.php?t=1038120
update quest_template set prevquestid = 35011 where id = 35013;


-- https://forum.wowcircle.net/showthread.php?t=1038113
update quest_template set prevquestid = 0, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34827;
update quest_template set prevquestid = 34827, nextquestid = 34830, exclusivegroup = -34828, nextquestidchain = 0 where id = 34828;
update quest_template set prevquestid = 34827, nextquestid = 34830, exclusivegroup = -34828, nextquestidchain = 0 where id = 34829;
update quest_template set prevquestid = 0, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34830;
update quest_template set prevquestid = 34830, nextquestid = 34942, exclusivegroup = -34882, nextquestidchain = 0 where id = 34882;
update quest_template set prevquestid = 34830, nextquestid = 34942, exclusivegroup = -34882, nextquestidchain = 0 where id = 34883;
update quest_template set prevquestid = 0, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34942;


-- https://forum.wowcircle.net/showthread.php?t=1034234
DELETE FROM `creature_queststarter` WHERE quest = 34849 and id = 80161;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(80161, 34849);

update quest_template set prevquestid = 34818, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 34850 where id = 34849;
update quest_template set prevquestid = 34849, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 34866 where id = 34850;
update quest_template set prevquestid = 34850, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34866;

update quest_template set prevquestid = 34951, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 34955 where id = 34954;
update quest_template set prevquestid = 34954, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 34956 where id = 34955;
update quest_template set prevquestid = 34955, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34956;