-- https://forum.wowcircle.com/showthread.php?t=1044298
update creature_template set ainame = 'SmartAI', gossip_menu_id = 19152 where entry in (101076);

delete from gossip_menu_option where menu_id in (19152);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(19152, 0, 0, 'Я готов начать ритуал.', 1, 1, 103586),
(19152, 1, 0, 'Я готов начать ритуал.', 1, 1, 103586);

delete from gossip_menu where entry in (19152);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(19152, 28054);

delete from npc_text where id in (28054);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`) VALUES 
(28054, 103585);

delete from conditions where SourceGroup in (19152) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19152, 0, 0, 0, 9, 0, 40694, 0, 0, 0, 0, '', 'Show gossip option if player has 40694'),
(15, 19152, 0, 0, 0, 29, 0, 101256, 30, 0, 1, 0, '', 'Show gossip option if near no 101256 creature'),

(15, 19152, 1, 0, 0, 9, 0, 42366, 0, 0, 0, 0, '', 'Show gossip option if player has 40694');

delete from smart_scripts where entryorguid in (101076, 101256);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(101256, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 30, 0, 0, 0, 0, 0, 0, 'Wyrmtongue - on just summoned - start attack'),
(101256, 0, 1, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 33, 101253, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Wyrmtongue - on death - give credit'),

(101076, 0, 0, 0, 62, 0, 100, 0, 19152, 0, 0, 0, 0, 11, 199598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Kirtos - on gossip select - cast spell');

delete from spell_target_position where id in (199598);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(199598, 1220, 817.0400, 5187.7769, 37.4001, 5.16);

update creature_template set minlevel = 100, maxlevel = 110, faction = 14, ainame = 'SmartAI' where entry = 101256;


-- https://forum.wowcircle.com/showthread.php?t=1044303
update creature_template set ainame = 'SmartAI', gossip_menu_id = 19152, npcflag = 1 where entry in (100858);

update creature_template set minlevel = 100, maxlevel = 110, faction = 14, ainame = 'SmartAI' where entry = 107224;

update gameobject set phasemask = 0, phaseid = 42366 where id = 251435;

update gameobject set phasemask = 0, phaseid = 423660 where id = 251436;

delete from phase_definitions where zoneId = 7637 and entry in (423660,42366);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7637, 42366, 0, 42366, 0, '42366 quest'),
(7637, 423660, 0, 423660, 0, '42366 quest');

delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and sourceentry in (423660,42366);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7637, 42366, 0, 0, 9, 0, 42366, 0, 0, 0, 0, '', 'Phase If Player has Quest'),

(26, 7637, 423660, 0, 0, 9, 0, 42366, 0, 0, 0, 0, '', 'Phase If Player has Quest'),
(26, 7637, 423660, 0, 0, 48, 0, 284100, 0, 0, 0, 0, '', 'Phase If Player complete objective');

delete from spell_target_position where id in (217014,217015);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(217014, 1220, 667.6528, 4827.3501, -131.4951, 1.01),
(217015, 1220, 818.4158, 5185.4219, 37.3754, 4.82);

delete from smart_scripts where entryorguid in (100858, 107224);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(107224, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 49, 199598, 0, 0, 0, 0, 0, 21, 30, 0, 0, 0, 0, 0, 0, 'Kirtos - on just summoned - start attack'),
(107224, 0, 1, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 33, 107224, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Kirtos - on death - give credit'),

(100858, 0, 0, 1, 62, 0, 100, 0, 19152, 1, 0, 0, 0, 12, 107224, 1, 300000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Kirtos - on gossip select - summon creature'),
(100858, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Kirtos - on link - despawn');


-- https://forum.wowcircle.com/showthread.php?t=1043191
delete from waypoint_data where id = 3;

delete from creature_text where entry = 108075;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`) VALUES 
(108075, 0, 0, 'Я хочу к маме...', 12, 0, 100, 0, 0, 113217),
(108075, 1, 0, 'АЙ! НЕТ!', 12, 0, 100, 0, 0, 113218),
(108075, 2, 0, '...А-а!', 12, 0, 100, 0, 0, 113246),
(108075, 3, 0, 'Я... *всхлипывает* знаю эту улицу. Вроде...', 12, 0, 100, 0, 0, 113220),
(108075, 4, 0, 'Мм-м... Да. ДА! Это дом Вантира!', 12, 0, 100, 0, 0, 113221),
(108075, 5, 0, 'Пока!', 12, 0, 100, 0, 0, 113222);

delete from waypoints where entry in (108075);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(108075, 1, 748.102, 4193.96, 3.23327),
(108075, 2, 749.576, 4187.16, 2.52018),
(108075, 3, 748.59, 4180.87, 1.4936),
(108075, 4, 737.316, 4168.38, 1.822),
(108075, 5, 724.399, 4164.88, 7.2892),
(108075, 6, 715.12, 4163.78, 5.77352),
(108075, 7, 702.911, 4162.77, 1.82335),
(108075, 8, 686.06, 4161.06, 1.49318),
(108075, 9, 670.117, 4155.24, 1.49318),
(108075, 10, 658.281, 4144.67, 1.49318),
(108075, 11, 643.759, 4137.36, 1.49318),
(108075, 12, 622.977, 4134.44, 1.49318),
(108075, 13, 602.82, 4131.05, 1.50614),
(108075, 14, 589.098, 4126.21, 2.87808),
(108075, 15, 577.421, 4120.94, 6.6037),
(108075, 16, 556.574, 4111.83, 6.39427),
(108075, 17, 537.372, 4103.36, 6.39427),
(108075, 18, 519.917, 4095.31, 6.55606),
(108075, 19, 501.97, 4083.56, 1.5627),
(108075, 20, 483.152, 4070.82, 7.79814),
(108075, 21, 469.175, 4060.3, 1.82329),
(108075, 22, 410.983, 4029.41, 1.38847);

update creature_template set ainame = 'SmartAI', speed_walk = 0.8, speed_run = 0.7 where entry in (108063,108075);

delete from smart_scripts where entryorguid in (108063,108075);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(108063, 0, 0, 0, 19, 0, 100, 0, 42486, 0, 0, 0, 0, 85, 213948, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Corine - on quest accept - invoker cast'),

(108075, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 53, 1, 108075, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Corine - on just summoned - start waypoint'),
(108075, 0, 1, 0, 40, 0, 100, 0, 1, 108075, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Corine - on waypoint reached - talk'),
(108075, 0, 2, 0, 40, 0, 100, 0, 7, 108075, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Corine - on waypoint reached - talk'),
(108075, 0, 3, 0, 40, 0, 100, 0, 12, 108075, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Corine - on waypoint reached - talk'),
(108075, 0, 4, 0, 40, 0, 100, 0, 17, 108075, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Corine - on waypoint reached - talk'),
(108075, 0, 5, 0, 40, 0, 100, 0, 19, 108075, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Corine - on waypoint reached - talk'),
(108075, 0, 6, 7, 40, 0, 100, 0, 21, 108075, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Corine - on waypoint reached - talk'),
(108075, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 108063, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Corine - on link - give credit'),
(108075, 0, 8, 0, 40, 0, 100, 0, 22, 108075, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Corine - on waypoint reached - despawn');


-- https://forum.wowcircle.com/showthread.php?t=1044319
update creature_template set ainame = 'SmartAI', flags_extra = 128, inhabittype = 4 where entry in (41304,41303);

delete from smart_scripts where entryorguid in (41304,41303);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(41304, 0, 0, 1, 10, 0, 100, 0, 1, 50, 0, 0, 0, 33, 41304, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on occ los - give credit'),
(41304, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - talk'),

(41303, 0, 0, 1, 10, 0, 100, 0, 1, 50, 0, 0, 0, 33, 41303, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on occ los - give credit'),
(41303, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - talk');

delete from conditions where SourceEntry in (41304,41303) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 41304, 0, 0, 9, 0, 25754, 0, 0, 0, 0, '', 'Smart if player has quest 25754'),
(22, 1, 41304, 0, 0, 48, 0, 266984, 0, 0, 1, 0, '', 'Smart if player not complete objective'),

(22, 1, 41303, 0, 0, 9, 0, 25754, 0, 0, 0, 0, '', 'Smart if player has quest 25754'),
(22, 1, 41303, 0, 0, 48, 0, 266985, 0, 0, 1, 0, '', 'Smart if player not complete objective');

delete from creature_text where entry in (41304,41303);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`) VALUES 
(41304, 0, 0, 'The naga priestesses look to be summoning a near endless stream of reinforcements!', 41, 0, 100, 0, 0, 44614),
(41303, 0, 0, 'The assault looks like it did almost no damage at all. They\'re already back in formation!', 41, 0, 100, 0, 0, 44615);


delete from creature where id in (41304,41303);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`) VALUES 
(41304, 0, 5144, 4967, 1, 1, 0, 0, 0, -6784.0898, 5031.7266, -562.1673, 3.64975, 1, 0, 0, 364240),
(41303, 0, 5144, 4967, 1, 1, 0, 0, 0, -7104.0830, 5261.5903, -581.1221, 3.64975, 1, 0, 0, 364240);


-- https://forum.wowcircle.com/showthread.php?t=1044376
update quest_template set prevquestid = 34943 where id = 34894;
update quest_template set prevquestid = 34894 where id = 34932;
update quest_template set prevquestid = 34932 where id = 34941;


-- https://forum.wowcircle.com/showthread.php?t=1044332
delete from creature where id in (106222);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`) VALUES 
(106222, 1540, 7979, 7979, 1, 0, 42044, 0, 0, 1660.9113, 1272.7129, 2.2045, 1.2, 180, 0, 0, 0);

update creature_template set minlevel = 100, maxlevel = 110, faction = 14, regenhealth = 1 where entry = 106222;

delete from phase_definitions where zoneId = 7979 and entry = 42044;
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7979, 42044, 0, 42044, 0, '42044 quest');

delete from conditions where SourceGroup in (7979) and SourceTypeOrReferenceId = 26 and sourceentry = 42044;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7979, 42044, 0, 0, 9, 0, 42044, 0, 0, 0, 0, '', 'Phase If Player has quest'),
(26, 7979, 42044, 0, 1, 28, 0, 42044, 0, 0, 0, 0, '', 'Phase If Player has quest');


-- https://forum.wowcircle.com/showthread.php?t=1044335
update creature_template set faction = 14, minlevel = 100, maxlevel = 110, scalelevelmin = 100, scalelevelmax = 110, regenhealth = 1 where entry in (106213,106214,107479);

delete from creature where id in (106213,106214,107479);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1693.04, 1385.87, 2.18035, 1.89426, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1716.27, 1373.51, 2.55644, 2.27125, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1731.42, 1348.24, 2.41983, 2.2202, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1736.43, 1328.03, 2.45063, 1.0853, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1717.8, 1328.16, 2.45063, 5.78983, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1702.17, 1326.66, 2.02847, 0.62584, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1682.3, 1319.16, 1.65688, 6.1629, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1664.72, 1332.16, 2.55257, 4.83165, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1688.5, 1342.7, 2.0086, 3.86954, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1694.96, 1361.97, 3.01878, 5.55029, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1676.72, 1379.89, 3.01805, 4.91805, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1684.3, 1404.83, 3.01325, 4.67065, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1684.51, 1436.49, 3.01706, 5.00837, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1672.47, 1470.1, 4.26795, 5.20079, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1656.24, 1529.15, 6.66896, 5.87624, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1679.04, 1538.83, 3.84225, 1.07352, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1672.87, 1558.71, 4.4759, 2.78962, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1660.89, 1541.1, 6.37087, 4.4311, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1673.09, 1496.64, 5.02971, 4.95339, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1649.38, 1498.55, 9.09488, 1.81573, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1655.09, 1510.54, 7.27284, 3.24908, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1642.74, 1515.27, 8.759, 4.89449, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106213, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1642.86, 1524.87, 7.39829, 0.492331, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106214, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1674.63, 1466.45, 4.2298, 1.74504, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106214, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1687.17, 1417.82, 3.00387, 1.80787, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106214, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1662.35, 1317.9, 2.05516, 0.417715, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106214, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1698.23, 1311.3, 2.33692, 3.02916, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106214, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1727.56, 1339.07, 2.44931, 3.95593, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(106214, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1698.97, 1374.15, 2.16115, 6.05294, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107479, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1619.75, 1507.66, 11.824, 0.0682091, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107479, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1622.49, 1539.17, 9.79017, 4.6353, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107479, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1639.23, 1572.17, 5.79019, 6.265, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107479, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1658.29, 1594.39, 4.56131, 2.87601, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107479, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1678.54, 1570.81, 4.11532, 2.49117, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107479, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1696.08, 1548.17, 2.73898, 1.44659, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(107479, 1540, 7979, 7979, 1, 0, 42041, 0, 0, 1671.06, 1527.47, 4.85024, 0.963566, 300, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from phase_definitions where zoneId = 7979 and entry = 42041;
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7979, 42041, 0, 42041, 0, '42041 quest');

delete from conditions where SourceGroup in (7979) and SourceTypeOrReferenceId = 26 and sourceentry = 42041;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7979, 42041, 0, 0, 9, 0, 42041, 0, 0, 0, 0, '', 'Phase If Player has quest'),
(26, 7979, 42041, 0, 1, 28, 0, 42041, 0, 0, 0, 0, '', 'Phase If Player has quest');


-- https://forum.wowcircle.com/showthread.php?t=1044327
delete from creature where id = 104241;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(104241, 1220, 7846, 8076, 1, 1, 0, 0, 0, 3975.16, 7382.27, 23.8677, 1.88674, 60, 0, 0, 1299083776, 49500000, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from gossip_menu_option where menu_id in (19393);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(19393, 0, 0, 'Я готов начать ритуал.', 1, 1, 103586);

delete from gossip_menu where entry in (19393);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(19393, 28578);

delete from npc_text where id in (28578);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`) VALUES 
(28578, 106713);

update creature_template set ainame = 'SmartAI' where entry = 104241;

delete from smart_scripts where entryorguid in (104241); 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(104241, 0, 0, 1, 62, 0, 100, 0, 19393, 0, 0, 0, 85, 205944, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Malfurion - on gossip select - invoker cast'),
(104241, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Malfurion - on link - despawn'),
(104241, 0, 2, 0, 10, 0, 100, 0, 1, 10, 0, 0, 33, 101195, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Malfurion - on ooc los - give credit');

delete from conditions where SourceGroup in (19393) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19393, 0, 0, 0, 9, 0, 41332, 0, 0, 0, 0, '', 'Show gossip option if player has 41332 quest'),
(15, 19393, 0, 0, 0, 48, 0, 282326, 0, 0, 0, 0, '', 'Show gossip option if player complete objective');

delete from spell_linked_spell where spell_trigger in (-206353) and spell_effect in (206300);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(-206353, 206300, 0, 0, 0, 0, 0, '');

delete from conditions where SourceEntry in (104241) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 3, 104241, 0, 0, 9, 0, 41332, 0, 0, 0, 0, '', 'Smart if player has quest 41332'),
(22, 3, 104241, 0, 0, 48, 0, 282326, 0, 0, 1, 0, '', 'Smart if player not complete objective');