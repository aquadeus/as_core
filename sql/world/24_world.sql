-- https://forum.wowcircle.net/showthread.php?t=821893
UPDATE `quest_template` SET `Flags` = 2293768, `NextQuestId` = 0, `NextQuestIdChain` = 0 WHERE  `Id` = 28229;
UPDATE `quest_template` SET `Flags` = 547946496, `PrevQuestId` = 28229, `NextQuestId` = 0, `ExclusiveGroup` = -28219, `NextQuestIdChain` = 0 WHERE  `Id` = 28220;
UPDATE `quest_template` SET `Flags` = 11075584, `PrevQuestId` = 28229, `NextQuestId` = 0, `ExclusiveGroup` = -28219, `NextQuestIdChain` = 0 WHERE  `Id` = 28219;
UPDATE `quest_template` SET `Flags` = 11075584, `PrevQuestId` = 28219, `NextQuestId` = 0, `ExclusiveGroup` = -28221, `NextQuestIdChain` = 0 WHERE  `Id` = 28221;
UPDATE `quest_template` SET `Flags` = 548208640, `PrevQuestId` = 28219, `NextQuestId` = 0, `ExclusiveGroup` = -28221, `NextQuestIdChain` = 0 WHERE  `Id` = 28222;
UPDATE `quest_template` SET `Flags` = 134807552, `PrevQuestId` = 28221 WHERE  `Id` = 28224;
UPDATE `quest_template` SET `Flags` = 786436, `SpecialFlags` = 2 WHERE  `Id` = 28228;

 DELETE FROM `quest_poi_points` WHERE `questId` IN (28229, 28219, 28220, 28221, 28222, 28224, 28228);
INSERT INTO `quest_poi_points` (`questId`, `idx1`, `idx2`, `x`, `y`) VALUES 
(28219, 0, 0, 6084, -1289),
(28219, 0, 1, 6119, -1215),
(28219, 0, 2, 6117, -1089),
(28219, 0, 3, 6041, -1074),
(28219, 0, 4, 5948, -1119),
(28219, 0, 5, 5957, -1263),
(28220, 0, 0, 6104, -1248),
(28220, 0, 1, 6123, -1114),
(28220, 0, 2, 6073, -1090),
(28220, 0, 3, 5976, -1119),
(28220, 0, 4, 5998, -1195),
(28220, 0, 5, 6017, -1234),
(28221, 0, 0, 6398, -1398),
(28221, 0, 1, 6598, -1214),
(28221, 0, 2, 6571, -1193),
(28221, 0, 3, 6349, -1151),
(28221, 0, 4, 6181, -1268),
(28221, 0, 5, 6175, -1333),
(28221, 0, 6, 6185, -1386),
(28221, 0, 7, 6314, -1396),
(28222, 0, 0, 6174, -1374),
(28222, 0, 1, 6288, -1369),
(28222, 0, 2, 6416, -1308),
(28222, 0, 3, 6457, -1183),
(28222, 0, 4, 6393, -1156),
(28222, 0, 5, 6283, -1123),
(28224, 0, 0, 6266, -1427),
(28228, 0, 0, 6221, -1152),
(28228, 1, 0, 6222, -1138),
(28229, 0, 0, 6221, -1152);

DELETE FROM `gameobject` WHERE `id` = 207063;
INSERT INTO `gameobject` (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
(207063, 1, 1, 1, 6282.77, -1123.01, 371.131, 2.05949, 0, 0, 0.857168, 0.515037, 120, 255, 1),
(207063, 1, 1, 1, 6369.22, -1327.92, 375.717, 2.49582, 0, 0, 0.948324, 0.317305, 120, 255, 1),
(207063, 1, 1, 1, 6342.94, -1256.97, 374.217, 2.05949, 0, 0, 0.857168, 0.515037, 120, 255, 1),
(207063, 1, 1, 1, 6416.35, -1307.89, 380.111, 2.74016, 0, 0, 0.979924, 0.199371, 120, 255, 1),
(207063, 1, 1, 1, 6393.17, -1155.77, 387.513, 1.01229, 0, 0, 0.484809, 0.874620, 120, 255, 1),
(207063, 1, 1, 1, 6456.61, -1182.84, 401.868, 2.72271, 0, 0, 0.978147, 0.207914, 120, 255, 1),
(207063, 1, 1, 1, 6272.73, -1341.75, 371.245, 2.77507, 0, 0, 0.983255, 0.182237, 120, 255, 1),
(207063, 1, 1, 1, 6232.03, -1361.31, 376.999, 3.47321, 0, 0, 0.986285, -0.16505, 120, 255, 1),
(207063, 1, 1, 1, 6287.60, -1369.11, 370.530, 2.79252, 0, 0, 0.984807, 0.173652, 120, 255, 1),
(207063, 1, 1, 1, 6173.76, -1373.54, 385.671, 5.46288, 0, 0, 0.398749, -0.91706, 120, 255, 1),
(207063, 1, 1, 1, 6211.91, -1290.37, 374.419, 2.77507, 0, 0, 0.983255, 0.182237, 120, 255, 1);

UPDATE `gameobject_template` SET `AIName` = 'SmartGameObjectAI', `ScriptName` = '' WHERE  `entry` = 207063;
 
DELETE FROM `smart_scripts` WHERE `entryorguid` = 207063 AND `source_type` = 1;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(207063, 1, 0, 0, 60, 0, 100, 0, 5000, 5000, 10000, 10000, 0, 45, 1, 1, 0, 0, 0, 0, 19, 48042, 5, 0, 0, 0, 0, 0, 'Rich Irontree Soil - Out of Combat - Set Data 1 1'),
(207063, 1, 1, 0, 38, 0, 100, 1, 1, 1, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Rich Irontree Soil - On Data Set 1 1 - Despawn');
 
UPDATE `creature_template` SET `minlevel` = 47, `maxlevel` = 47, `faction` = 2140, `AIName` = 'SmartAI', `ScriptName` = '' WHERE  `entry` = 48042;
 
DELETE FROM `smart_scripts` WHERE `entryorguid` = 48042 AND `source_type` = 0;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 48042*100 AND `source_type` = 9;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(48042, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 207, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Sapling Protector - On Just Summon - Set Visibility Invoker'),
(48042, 0, 1, 0, 38, 0, 100, 0, 1, 1, 5000, 5000, 0, 69, 1, 0, 0, 0, 0, 0, 20, 207063, 5, 0, 0, 0, 0, 0, 'Sapling Protector - On Data Set 1 1 - Move to Pos'),
(48042, 0, 2, 0, 34, 0, 100, 0, 8, 1, 0, 0, 0, 80, 48042*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Sapling Protector - On Reached Point 1 - Run Script'),
(48042, 0, 3, 0, 19, 0, 100, 0, 28224, 0, 0, 0, 0, 85, 89357, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Sapling Protector - On Quest Accept - Cast Spell \'Summon Fluorishing Protector\''),
(48042*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 11, 89543, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Sapling Protector - On Script - Cast Spell \'Rich Soil\''),
(48042*100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 20, 207063, 5, 0, 0, 0, 0, 0, 'Sapling Protector - On Script - Set Data 1 1'),
(48042*100, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 0, 33, 48200, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Sapling Protector - On Script - Quest Credit'),
(48042*100, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Sapling Protector - On Script - Follow Invoker');
 
 
-- https://forum.wowcircle.net/showthread.php?t=791465
update quest_template set method = 2 where id = 38058;


-- https://forum.wowcircle.net/showthread.php?t=792501
update creature_template set ainame = 'SmartAI' where entry = 109222;

DELETE FROM `spell_scene` WHERE MiscValue in (1362);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1669, 1362, 20);

delete from spell_linked_spell where spell_trigger in (219634) and spell_effect in (219410);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(219634, 219410, 0, 0, 0, 0, 0, '');

delete from smart_scripts where entryorguid in (109222);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(109222, 0, 0, 0, 19, 0, 100, 0, 43415, 0, 0, 0, 75, 219567, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Meryl - On quest accept - apply aura');

DELETE FROM `creature_questender` WHERE quest in (43415) and id in (109222,102700);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(109222, 43415),
(102700, 43415);


-- https://forum.wowcircle.net/showthread.php?t=790501
delete from spell_target_position where id in (213076);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(213076, 1220, -892.5218, 4668.2993, 955.9161, 1.18);

update gameobject_template set flags = 0 where entry = 250417;

update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry in (107444,107443);

delete from smart_scripts where entryorguid in (107444,107443,107423);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(107444, 0, 0, 0, 10, 0, 100, 0, 1, 5, 0, 0, 33, 107444, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On ooc los - give credit'),
(107443, 0, 0, 0, 10, 0, 100, 0, 1, 5, 0, 0, 33, 107443, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - On ooc los - give credit');

delete from conditions where SourceEntry in (107444,107443) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 107444, 0, 0, 48, 0, 284188, 0, 0, 1, 0, '', 'Smart if player not complete quest criteria'),
(22, 1, 107443, 0, 0, 48, 0, 284655, 0, 0, 1, 0, '', 'Smart if player not complete quest criteria');

delete from creature where id in (107443,107444);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(107443, 1220, 7637, 8215, 1, 1, 0, 0, 0, 151.604, 4608.31, -118.517, 0, 180, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107443, 1220, 7502, 7597, 1, 1, 0, 0, 0, -888.995, 4677.48, 954.607, 1.19492, 300, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107444, 1220, 7637, 8215, 1, 1, 0, 0, 0, 180.575, 4706.88, -90.7568, 0, 180, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107444, 1220, 7502, 7598, 1, 1, 0, 0, 0, -892.078, 4669.15, 955.916, 1.21456, 300, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


delete from phase_definitions where zoneId = 7502 and entry in (42423);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7502, 42423, 0, 42423, 0, '42423 quest phase');

delete from conditions where SourceGroup in (7502) and SourceTypeOrReferenceId = 26 and sourceentry = 42423;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7502, 42423, 0, 0, 9, 0, 42423, 0, 0, 0, 0, '', 'Activate Phase if player has quest 42423');

update gameobject set phaseid = 42423, phasemask = 0 where id = 250412;


-- https://forum.wowcircle.net/showthread.php?t=794107
update creature set phasemask = 3 where id = 74877;


-- https://forum.wowcircle.net/showthread.php?t=795987
-- update creature_template set unit_flags = 768 where entry in (24640);
update creature_template set ainame = 'SmartAI' where entry in (24640,24015);

delete from smart_scripts where entryorguid in (24640,2464000,24015) or entryorguid in (24015) and id = 3;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(24640, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 80, 2464000, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Banner - on just summoned - run timed script'),
(24640, 0, 1, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 6, 11429, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Banner - on death - fail quest'),

(2464000, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - set state'),
(2464000, 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 103, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - set rooted'),
(2464000, 9, 2, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 12, 24015, 1, 60000, 0, 0, 0, 8, 0, 0, 0, 1485, -5331, 195, 0, 'Timed Script - summon creature'),
(2464000, 9, 3, 0, 0, 0, 100, 1, 15000, 15000, 0, 0, 0, 12, 24015, 1, 60000, 0, 0, 0, 8, 0, 0, 0, 1529, -5303, 198.5, 3.5, 'Timed Script - summon creature'),
(2464000, 9, 4, 0, 0, 0, 100, 1, 15000, 15000, 0, 0, 0, 12, 24015, 1, 60000, 0, 0, 0, 8, 0, 0, 0, 1486, -5304, 197, 5.5, 'Timed Script - summon creature'),
(2464000, 9, 5, 0, 0, 0, 100, 1, 15000, 15000, 0, 0, 0, 15, 11429, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Timed Script -  complete quest'),
(2464000, 9, 6, 0, 0, 0, 100, 1, 2000, 2000, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - despawn'),

(24015, 0, 3, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'Defender - on just summoned - start attack');

update creature_template_wdb set killcredit2 = 0 where entry = 23655;


-- https://forum.wowcircle.net/showthread.php?t=796006
update creature_template_wdb set killcredit2 = 0 where entry = 23760;

update creature_template set ainame = 'SmartAI', unit_flags = 768, flags_extra = 128, inhabittype = 7 where entry in (23921,23922,23923,23924);

delete from creature where guid = 519060 and id = 23924;

delete from smart_scripts where entryorguid in (23921,23922,23923,23924);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(23921, 0, 0, 0, 8, 0, 100, 0, 42564, 0, 0, 0, 0, 33, 23921, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(23922, 0, 0, 0, 8, 0, 100, 0, 42564, 0, 0, 0, 0, 33, 23922, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(23923, 0, 0, 0, 8, 0, 100, 0, 42564, 0, 0, 0, 0, 33, 23923, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(23924, 0, 0, 0, 8, 0, 100, 0, 42564, 0, 0, 0, 0, 33, 23924, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit');

delete from conditions where SourceEntry in (42564) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 42564, 0, 0, 31, 0, 3, 23921, 0, 0, 0, '', 'Cast only 23921'),
(13, 1, 42564, 0, 1, 31, 0, 3, 23922, 0, 0, 0, '', 'Cast only 23922'),
(13, 1, 42564, 0, 2, 31, 0, 3, 23923, 0, 0, 0, '', 'Cast only 23923'),
(13, 1, 42564, 0, 3, 31, 0, 3, 23924, 0, 0, 0, '', 'Cast only 23924');

delete from creature where id in (23921,23922,23923,23924);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(23921, 571, 495, 3994, 1, 1, 0, 0, 0, 864.646, -4329.36, 184.876, 3.7001, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(23922, 571, 495, 3994, 1, 1, 0, 0, 0, 989.351, -4306.56, 178.896, 2.28638, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(23923, 571, 495, 3994, 1, 1, 0, 0, 0, 1091.54, -4492.57, 194.704, 2.4501, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(23924, 571, 495, 3994, 1, 1, 0, 0, 0, 793.878, -4507.29, 196.865, 2.18166, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.net/showthread.php?t=797550
delete from npc_vendor where item = 127043;


-- https://forum.wowcircle.net/showthread.php?t=799500
DELETE FROM `spell_scene` WHERE MiscValue in (1681);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1825, 1681, 26);

delete from spell_linked_spell where spell_trigger in (240982) and spell_effect in (231434);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`, `actiontype`) VALUES 
(240982, 231434, 0, 0, 0, 0, 0, '', 1);