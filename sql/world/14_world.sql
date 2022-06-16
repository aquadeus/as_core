-- https://forum.wowcircle.net/showthread.php?t=1040047
delete from creature where id in (81959,83529);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(81959, 1116, 6722, 7197, 1, 1, 0, 0, 0, -368.95, 2240.32, 26.219, 1.6119, 300, 0, 0, 0, 0, 0),
(83529, 1116, 6722, 7197, 1, 1, 0, 0, 0, -364.899, 2252.17, 27.4552, 4.79016, 300, 0, 0, 0, 0, 0);


-- https://forum.wowcircle.net/showthread.php?t=1039224
update creature_template set ainame = 'SmartAI', unit_flags = 0, faction = 35, npcflag = 0, speed_walk = 2, speed_run = 2 where entry in (107998,108011,108001,108038,108041,108071,108074);
update creature_template set ainame = 'SmartAI' where entry in (107968);
update creature_template set npcflag = 16777216 where entry in (108001);

delete from smart_scripts where entryorguid in (107968,107998,108011,108001,108038,108041,108071,108074,10801100,10807100,10804100,10803800,10799800); 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(108074, 0, 0, 0, 10, 0, 100, 0, 1, 1, 5000, 5000, 33, 108074, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Imp - on ooc los - give credit'),
(108074, 0, 1, 0, 60, 0, 100, 1, 0, 0, 0, 0, 11, 214378, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on update - cast spell'),

(108071, 0, 0, 0, 10, 0, 100, 1, 1, 1, 0, 0, 80, 10807100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on ooc los - run timed script'),
(108071, 0, 1, 2, 60, 0, 100, 1, 0, 0, 0, 0, 11, 214510, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on update - cast spell'),
(108071, 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 11, 214378, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on link - cast spell'),
(10807100, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 97, 10, 10, 0, 0, 0, 0, 8, 0, 0, 0, -852.9496, 4380.465, 748.6802, 4.63649, 'Timed Script - jump to position'),
(10807100, 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - unaura'),
(10807100, 9, 2, 0, 0, 0, 100, 1, 3000, 3000, 0, 0, 33, 108071, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - give credit'),
(10807100, 9, 3, 0, 0, 0, 100, 1, 0, 0, 0, 0, 33, 107999, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - give credit'),
(10807100, 9, 4, 0, 0, 0, 100, 1, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - despawn'),

(108041, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 11, 214378, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on update - cast spell'),
(108041, 0, 1, 0, 10, 0, 100, 1, 1, 1, 0, 0, 80, 10804100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on ooc los - run timed script'),
(10804100, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 97, 10, 10, 0, 0, 0, 0, 8, 0, 0, 0, -772.851, 4478.71, 732.957, 0, 'Timed Script - jump to position'),
(10804100, 9, 1, 0, 0, 0, 100, 1, 2000, 2000, 0, 0, 53, 1, 108041, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - start waypoint movement'),
(10804100, 9, 2, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 33, 108041, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - give credit'),
(10804100, 9, 3, 0, 0, 0, 100, 1, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - despawn'),

(108038, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 11, 110674, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on update - cast spell'),
(108038, 0, 1, 2, 10, 0, 100, 1, 1, 1, 0, 0, 80, 10803800, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on ooc los - run timed script'),
(10803800, 9, 0, 0, 0, 0, 100, 1, 1, 1, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - unaura'),
(10803800, 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 53, 1, 108038, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - start waypoint movement'),
(10803800, 9, 2, 0, 0, 0, 100, 1, 12000, 12000, 0, 0, 33, 108038, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - give credit'),
(10803800, 9, 3, 0, 0, 0, 100, 1, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - despawn'),

(108001, 0, 0, 1, 54, 0, 100, 1, 3000, 3000, 0, 0, 53, 1, 108001, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on just summoned - start waypoint movement'),
(108001, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 41, 8000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on link - despawn'),
(108001, 0, 2, 0, 40, 0, 100, 1, 4, 108001, 0, 0, 33, 108001, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Imp - waypoint reached - give credit'),

(107968, 0, 0, 0, 19, 0, 100, 0, 42594, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Landalock - on quest accept - talk'),

(107998, 0, 0, 0, 10, 0, 100, 1, 1, 1, 0, 0, 80, 10799800, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on ooc los - run timed script'),
(10799800, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 97, 10, 10, 0, 0, 0, 0, 8, 0, 0, 0, -854.8842, 4636.5464, 749.4609, 4.63649, 'Timed Script - jump to position'),
(10799800, 9, 1, 0, 0, 0, 100, 1, 3000, 3000, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, -853.158, 4608.92, 750.309, 0, 'Timed Script - move to position'),
(10799800, 9, 2, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 33, 107998, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - give credit'),
(10799800, 9, 3, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - despawn'),

(108011, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 11, 214378, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on update - cast spell'),
(108011, 0, 1, 0, 10, 0, 100, 1, 1, 1, 0, 0, 80, 10801100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Imp - on ooc los - run timed script'),
(10801100, 9, 0, 0, 0, 0, 100, 1, 1, 1, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - unaura'),
(10801100, 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 53, 1, 108011, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - start waypoint movement'),
(10801100, 9, 2, 0, 0, 0, 100, 1, 10000, 10000, 0, 0, 33, 108011, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - give credit'),
(10801100, 9, 3, 0, 0, 0, 100, 1, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Script - despawn');

DELETE FROM `creature_text` WHERE `entry` = 107968;
INSERT INTO `creature_text` (`entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `comment`) VALUES 
(107968, 0, 0, 'Demon hunter, that imp just stole the grimoire. Catch it!', 12, 0, 100, 0, 0, 0, 113077, '');

delete from waypoints where entry in (107998,108011,108001,108038,108041,108074); 
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(108041, 1, -773.924, 4468.14, 734.067),
(108041, 2, -779.897, 4454.98, 735.642),
(108041, 3, -786.556, 4447.27, 736.930),
(108041, 4, -801.013, 4429.88, 738.644),

(108038, 1, -787.962, 4531.95, 730.795),
(108038, 2, -792.613, 4524.7, 730.081),
(108038, 3, -793.625, 4509.79, 730.757),
(108038, 4, -781.435, 4494.32, 731.809),
(108038, 5, -777.945, 4484.19, 732.596),
(108038, 6, -781.422, 4483.9, 732.857),
(108038, 7, -787.635, 4490.6, 733.995),
(108038, 8, -791.134, 4494.31, 735.01),
(108038, 9, -797.988, 4486.12, 735.014),
(108038, 10, -800.178, 4462.73, 735.014),
(108038, 11, -805.844, 4454.91, 739.423),
(108038, 12, -813.192, 4459.5, 743.556),
(108038, 13, -808.066, 4467.66, 745.907),
(108038, 14, -804.1, 4476.23, 746.012),
(108038, 15, -787.4063, 4477.874, 745.9586),

(108001, 1, -830.354, 4525.29, 734.593),
(108001, 2, -820.619, 4526.62, 729.604),
(108001, 3, -806.455, 4522.99, 729.769),
(108001, 4, -794.738, 4523.38, 729.474),
(108001, 5, -790.121, 4528.95, 730.801),

-- (107998, 1, -853.158, 4608.92, 750.309),

(108011, 1, -854.697, 4595.38, 748.857),
(108011, 2, -859.068, 4557.87, 727.858),
(108011, 3, -851.275, 4541.15, 728.624),
(108011, 4, -847.956, 4532.98, 730.535),
(108011, 5, -841.979, 4530.09, 732.504),
(108011, 6, -832.302, 4525.82, 734.585),
(108011, 7, -832.790, 4521.96, 734.545),
(108011, 8, -838.541, 4515.99, 734.603),
(108011, 9, -841.8871, 4513.618, 734.6334);

delete from event_scripts where id = 51455;
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(51455, 0, 10, 108001, 60000, 0, -841.8871, 4513.618, 734.6334, 0.5817931);

delete from creature where id in (107998,108011,108001,108038,108041,108071,108074);
INSERT IGNORE INTO creature (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(107998, 1220, 0, 0, 1, 0, 7502, 0, 0, -862.1493, 4655.789, 769.1949, 2.594733, 60, 0, 0, 44, 0, 0, 0, 0, 0),
(108011, 1220, 0, 0, 1, 0, 7503, 0, 0, -853.158, 4608.92, 750.309, 4.658507, 60, 0, 0, 44, 0, 0, 0, 0, 0),
(108001, 1220, 0, 0, 1, 0, 7504, 0, 0, -841.8871, 4513.618, 734.6334, 0.5817931, 60, 0, 0, 44, 0, 0, 0, 0, 0),
(108038, 1220, 0, 0, 1, 0, 7505, 0, 0, -791.3542, 4537.593, 731.8712, 2.168806, 60, 0, 0, 44, 0, 0, 0, 0, 0),
(108041, 1220, 0, 0, 1, 0, 7506, 0, 0, -787.4063, 4477.874, 745.9586, 0.1102197, 60, 0, 0, 44, 0, 0, 0, 0, 0),
(108071, 1220, 0, 0, 1, 0, 7507, 0, 0, -828.9913, 4398.113, 737.6154, 3.9512, 60, 0, 0, 44, 0, 0, 0, 0, 0),
(108074, 1220, 0, 0, 1, 0, 7508, 0, 0, -852.9496, 4380.465, 748.6802, 2.554864, 60, 0, 0, 44, 0, 0, 0, 0, 0);

update gameobject_template set data1 = 250993, flags = 0 where entry = 250993;
delete from gameobject where id = 250993;
INSERT IGNORE INTO gameobject (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phasemask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(250993, 1220, 7502, 0, 1, 0, 7509, -853.4011, 4380.764, 748.5969, 0, 0, 0, 0, 1, 60, 255, 1);

delete from gameobject_loot_template where entry in (250993);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(250993, 138139, -100, 0, 0, 1, 1);

delete from phase_definitions where zoneId = 7502 and entry in (4,5,6,7,8,9,10,11);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7502, 4 , 0, 7502, 0, '42594 quest'),
(7502, 5 , 0, 7503, 0, '42594 quest'),
(7502, 6 , 0, 7504, 0, '42594 quest'),
(7502, 7 , 0, 7505, 0, '42594 quest'),
(7502, 8 , 0, 7506, 0, '42594 quest'),
(7502, 9 , 0, 7507, 0, '42594 quest'),
(7502, 10, 0, 7508, 0, '42594 quest'),
(7502, 11, 0, 7509, 0, '42594 quest');

delete from conditions where SourceGroup in (7502) and SourceTypeOrReferenceId = 26 and SourceEntry in (4,5,6,7,8,9,10,11);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7502, 4, 0, 0, 48, 0, 284471, 0, 0, 1, 0, '', 'Activate phase if player not complete criteria'),
(26, 7502, 4, 0, 0, 9, 0, 42594, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7502, 5, 0, 0, 48, 0, 284471, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 5, 0, 0, 48, 0, 284472, 0, 0, 1, 0, '', 'Activate phase if player not complete criteria'),
(26, 7502, 5, 0, 0, 9, 0, 42594, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7502, 6, 0, 0, 48, 0, 284471, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 6, 0, 0, 48, 0, 284472, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 6, 0, 0, 48, 0, 284478, 0, 0, 1, 0, '', 'Activate phase if player not complete criteria'),
(26, 7502, 6, 0, 0, 9, 0, 42594, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7502, 7, 0, 0, 48, 0, 284471, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 7, 0, 0, 48, 0, 284472, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 7, 0, 0, 48, 0, 284478, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 7, 0, 0, 48, 0, 284491, 0, 0, 1, 0, '', 'Activate phase if player not complete criteria'),
(26, 7502, 7, 0, 0, 9, 0, 42594, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7502, 8, 0, 0, 48, 0, 284471, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 8, 0, 0, 48, 0, 284472, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 8, 0, 0, 48, 0, 284478, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 8, 0, 0, 48, 0, 284491, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 8, 0, 0, 48, 0, 284493, 0, 0, 1, 0, '', 'Activate phase if player not complete criteria'),
(26, 7502, 8, 0, 0, 9, 0, 42594, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7502, 9, 0, 0, 48, 0, 284471, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 9, 0, 0, 48, 0, 284472, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 9, 0, 0, 48, 0, 284478, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 9, 0, 0, 48, 0, 284491, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 9, 0, 0, 48, 0, 284493, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 9, 0, 0, 48, 0, 284505, 0, 0, 1, 0, '', 'Activate phase if player not complete criteria'),
(26, 7502, 9, 0, 0, 9, 0, 42594, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7502, 10, 0, 0, 48, 0, 284471, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 10, 0, 0, 48, 0, 284472, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 10, 0, 0, 48, 0, 284478, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 10, 0, 0, 48, 0, 284491, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 10, 0, 0, 48, 0, 284493, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 10, 0, 0, 48, 0, 284505, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 10, 0, 0, 48, 0, 284506, 0, 0, 1, 0, '', 'Activate phase if player not complete criteria'),
(26, 7502, 10, 0, 0, 9, 0, 42594, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7502, 11, 0, 0, 48, 0, 284471, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 11, 0, 0, 48, 0, 284472, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 11, 0, 0, 48, 0, 284478, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 11, 0, 0, 48, 0, 284491, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 11, 0, 0, 48, 0, 284493, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 11, 0, 0, 48, 0, 284505, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 11, 0, 0, 48, 0, 284506, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7502, 11, 0, 0, 9, 0, 42594, 0, 0, 0, 0, '', 'Activate phase if player has quest');

delete from npc_spellclick_spells where npc_entry in (108001);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(108001, 214379, 1, 0);


-- https://forum.wowcircle.net/showthread.php?t=1040492
update quest_template set prevquestid = 33721, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34948;

update quest_template set prevquestid = 34948, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 33720;
update quest_template set prevquestid = 34947, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34091;

DELETE FROM `creature_queststarter` WHERE id = 75924 and quest = 34948;
DELETE FROM `creature_queststarter` WHERE id = 75874 and quest = 34947;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(75924, 34948),
(75874, 34947);

DELETE FROM `gameobject_questender` WHERE id = 225726 and quest in (34948,34947);
INSERT INTO `gameobject_questender` (`id`, `quest`) VALUES
(225726, 34947),
(225726, 34948);

DELETE FROM `gameobject_queststarter` WHERE id = 225726 and quest in (33720,34091);
INSERT INTO `gameobject_queststarter` (`id`, `quest`) VALUES
(225726, 33720),
(225726, 34091);

delete from gameobject where id = 225726;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`) VALUES 
(225726, 1116, 6662, 6947, 1, 1, 0, 3773.67, 2258.14, 26.4302, 6.07543, 0, 0, 0, 0, 180, 255, 1, 0),
(225726, 1116, 6662, 6950, 1, 1, 0, 3334.68, 2148.67, 128.284, 3.06937, 0, 0, 0, 0, 180, 255, 1, 0);


-- https://forum.wowcircle.net/showthread.php?t=1040490
update quest_template set prevquestid = 34683, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 33754;
update quest_template set prevquestid = 33754, nextquestid = 34950, exclusivegroup = -33722, nextquestidchain = 34950 where id = 33722;
update quest_template set prevquestid = 33754, nextquestid = 34950, exclusivegroup = -33722, nextquestidchain = 34950 where id = 35226;
update quest_template set prevquestid = 0, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34950;

DELETE FROM `creature_queststarter` WHERE id = 75806 and quest in (33754,35226,33722);
DELETE FROM `creature_queststarter` WHERE id = 75958 and quest in (34950);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(75958, 34950),
(75806, 33722),
(75806, 35226),
(75806, 33754);


-- https://forum.wowcircle.net/showthread.php?t=1040489
update quest_template set prevquestid = 35102, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34577;


-- https://forum.wowcircle.net/showthread.php?t=1040484
update quest_template set prevquestid = 34569, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 35102;


-- https://forum.wowcircle.net/showthread.php?t=1040480
update quest_template set prevquestid = 34766, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34803;

DELETE FROM `creature_queststarter` WHERE id = 79921 and quest in (34766);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(79921, 34766);


-- https://forum.wowcircle.net/showthread.php?t=1037141
delete from creature where id in (45997);
INSERT IGNORE INTO creature (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(45997, 0, 130, 5480, 1, 0, 0, 0, 0, -814.366, 1324.42, 33.8695, 0.770683, 60, 0, 0, 0, 0, 0, 0, 0, 0);

update creature_template set ainame = 'SmartAI' where entry = 45997;

delete from smart_scripts where entryorguid in (45997); 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(45997, 0, 0, 0, 10, 0, 100, 1, 0, 20, 0, 0, 1, 0, 5000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lourne - on ooc los - talk'),
(45997, 0, 1, 0, 52, 0, 100, 1, 0, 45997, 0, 0, 1, 1, 5000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lourne - on text over - talk'),
(45997, 0, 2, 0, 52, 0, 100, 1, 1, 45997, 0, 0, 1, 2, 5000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lourne - on text over - talk'),
(45997, 0, 3, 4, 52, 0, 100, 1, 2, 45997, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lourne - on text over - talk'),
(45997, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 19, 768, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lourne - on link - remove unit flag'),
(45997, 0, 5, 0, 6, 0, 100, 1, 0, 0, 0, 0, 33, 46002, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Lourne - on death - give credit');

delete from creature_text where entry in (45997);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(45997, 0, 0, 'So the Banshee Queen has sent assassins?', 12, 0, 100, 0, 0, 0, 46072),
(45997, 1, 0, 'Godfrey? So the rumors are true.', 12, 0, 100, 0, 0, 0, 46074),
(45997, 2, 0, 'You would betray your kingdom out of spite?', 12, 0, 100, 0, 0, 0, 46075),
(45997, 3, 0, 'HAH! You\'ll have to kill me! DIE!', 12, 0, 100, 0, 0, 0, 46079);

delete from conditions where SourceEntry in (45997) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 45997, 0, 0, 9, 0, 27594, 0, 0, 0, 0, '', 'Smart if player has 27594 quest');