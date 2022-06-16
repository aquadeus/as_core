-- https://forum.wowcircle.net/showthread.php?t=1062826
-- https://ru.wowhead.com/quest=36029
-- https://ru.wowhead.com/quest=36028

UPDATE `quest_template` SET `PrevQuestId`='35947' WHERE (`Id`='36028');
UPDATE `creature` SET `spawntimesecs`='120' WHERE (`id`='83834');
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_gardul_venomshiv_q' WHERE (`entry`='83834'); 
UPDATE `creature_template` SET `unit_flags`='898', `AIName`='SmartAI', `flags_extra`='2', `ScaleLevelMin`='96', `ScaleLevelMax`='100' WHERE (`entry`='83899'); -- h
UPDATE `creature_template` SET `unit_flags`='898', `AIName`='SmartAI', `flags_extra`='2', `ScaleLevelMin`='96', `ScaleLevelMax`='100' WHERE (`entry`='83900'); -- a

delete from smart_scripts where source_type=0 and entryorguid in (83834,83899,83900);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(83899, '', 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83899, '', 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 1, 83899, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83899, '', 0, 2, 0, 40, 0, 100, 0, 0, 3, 83899, 0, 0, 0, 41, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(83900, '', 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83900, '', 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 1, 83899, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83900, '', 0, 2, 0, 40, 0, 100, 0, 0, 3, 83899, 0, 0, 0, 41, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

Delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=83834 and ConditionTypeOrReference=9;
Delete from conditions where SourceTypeOrReferenceId=26 and SourceGroup=6722 and SourceEntry in (12,13,14);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('22', '13', '83834', '0', '0', '9', '0', '36028', '0', '0', '0', '0', '', ''),
('22', '14', '83834', '0', '0', '9', '0', '36029', '0', '0', '0', '0', '', ''),

('26', '6722', '12', '0', '0', '47', '0', '36028', '66', '0', '0', '0', '', 'q36028'),
('26', '6722', '13', '0', '0', '47', '0', '36029', '66', '0', '0', '0', '', 'q36029'),

('26', '6722', '14', '0', '0', '9', '0', '36029', '0', '0', '0', '0', '', 'q36029/36028'),
('26', '6722', '14', '0', '1', '9', '0', '36028', '0', '0', '0', '0', '', 'q36029/36028');

delete from phase_definitions where zoneId=6722 and entry in (12,13,14);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
('6722', '12', '0', '1012', '0', '0', '0', 'q36028'), -- h
('6722', '13', '0', '1013', '0', '0', '0', 'q36029'),-- a
('6722', '14', '0', '1014', '0', '0', '0', 'q36029/36028');

DELETE FROM `creature` WHERE (`guid`='11572732') and id=83904; 
DELETE FROM `creature` WHERE (`id`='83900');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1012' WHERE (`id`='83903');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1013' WHERE (`id`='83904');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1014' WHERE (`id`='83834');

delete from creature_text where Entry in (83834,83899,83900);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(83834, 0, 0, "Traitors, all of you. You've led the enemy to our doorstep!", 12, 0, 100, 53, 0, 0, 85298, ""),
(83834, 1, 0, "Don't waste my time.", 12, 0, 100, 0, 0, 0, 85318, ""),

(83899, 0, 0, "The fool. He's going to lead us straight to his brewing operation. Follow me.", 12, 0, 100, 0, 0, 0, 85342, ""),
(83900, 0, 0, "The fool. He's going tae lead us straight to his brewin' operation. Come on.", 12, 0, 100, 0, 0, 0, 85347, "");


delete from waypoints where entry in (83899);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES 

('83899', '1', '10.7912', '1277.8', '12.3803', ''),
('83899', '2', '9.64647', '1264.92', '14.4099', ''),
('83899', '3', '7.05584', '1254.29', '19.2532', '');

delete from script_waypoint where entry=83834;
INSERT INTO `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES 
('83834', '0', '10.5057', '1265.61', '14.1752', '0', 'npc_gardul_venomshiv_q'),
('83834', '1', '8.94516', '1255.44', '18.3654', '0', 'npc_gardul_venomshiv_q'),
('83834', '2', '4.44836', '1249.56', '21.6646', '0', 'npc_gardul_venomshiv_q');

delete from spell_linked_spell where spell_effect=169172;
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `comment`) VALUES 
('-169171', '169172', 'Ядовитый удар');


-- https://ru.wowhead.com/quest=12536

UPDATE `creature_template` SET `speed_run`='2.23' WHERE (`entry`='28308');
 
DELETE FROM `quest_start_scripts` WHERE (`id`='12536');
UPDATE `quest_template` SET `StartScript`='0' WHERE (`Id`='12536');

UPDATE `smart_scripts` SET `action_param2`='3' WHERE (`entryorguid`='28298') AND (`source_type`='0') AND (`id`='0') AND (`link`='0');
delete from smart_scripts where source_type=0 and entryorguid in (28308);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(28308, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 1, 28308, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(28308, '', 0, 1, 2, 40, 0, 100, 0, 0, 46, 28308, 0, 0, 0, 15, 12536, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(28308, '', 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 85, 51189, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(28308, '', 0, 3, 0, 58, 0, 100, 1, 0, 47, 28308, 0, 0, 0, 41, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

DELETE FROM `waypoints` WHERE (`entry`='28308');
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES

(28308, 1, 5295.42, 4436.54, -97.6029, ""),
(28308, 2, 5277.78, 4429.43, -95.943, ""),
(28308, 3, 5249.84, 4405.99, -95.9316, ""),
(28308, 4, 5262.34, 4376.74, -95.6816, ""),
(28308, 5, 5266.96, 4365.82, -98.007, ""),
(28308, 6, 5277.71, 4366.32, -100.257, ""),
(28308, 7, 5289.4, 4366.66, -102.004, ""),
(28308, 8, 5296.4, 4365.91, -106.504, ""),
(28308, 9, 5320.15, 4364.41, -118.254, ""),
(28308, 10, 5336.5, 4355.77, -122.365, ""),
(28308, 11, 5346.25, 4344.52, -129.615, ""),
(28308, 12, 5349, 4341.52, -133.865, ""),
(28308, 13, 5354.97, 4338.85, -138.26, ""),
(28308, 14, 5369.59, 4331.1, -142.589, ""),
(28308, 15, 5379.34, 4324.6, -145.339, ""),
(28308, 16, 5403.84, 4309.1, -143.339, ""),
(28308, 17, 5416.49, 4307.27, -138.22, ""),
(28308, 18, 5450.89, 4304.03, -133.483, ""),
(28308, 19, 5481.03, 4297.37, -131.947, ""),
(28308, 20, 5492.86, 4290.07, -124.09, ""),
(28308, 21, 5499.21, 4278.65, -114.858, ""),
(28308, 22, 5507.46, 4266.4, -107.858, ""),
(28308, 23, 5552.31, 4261.59, -102.648, ""),
(28308, 24, 5585.06, 4261.27, -99.3233, ""),
(28308, 25, 5681.83, 4266.6, -99.0788, ""),
(28308, 26, 5729.49, 4278.95, -106.109, ""),
(28308, 27, 5754.78, 4289.87, -113.818, ""),
(28308, 28, 5836.03, 4365.12, -114.568, ""),
(28308, 29, 5842.55, 4370.52, -109.916, ""),
(28308, 30, 5855.8, 4379.77, -104.416, ""),
(28308, 31, 5859.8, 4382.52, -100.166, ""),
(28308, 32, 5863.8, 4385.27, -100.416, ""),
(28308, 33, 5897.42, 4408.65, -95.0272, ""),
(28308, 34, 5941.87, 4461.88, -95.2359, ""),
(28308, 35, 5961.31, 4536.65, -94.4528, ""),
(28308, 36, 5963.58, 4566.3, -96.6575, ""),
(28308, 37, 5970.63, 4621.86, -98.4376, ""),
(28308, 38, 5980.64, 4670.49, -97.9448, ""),
(28308, 39, 6021.18, 4757.59, -96.4865, ""),
(28308, 40, 6032.8, 4783.2, -94.8458, ""),
(28308, 41, 6052.3, 4810.95, -94.3458, ""),
(28308, 42, 6065.01, 4827.78, -94.0694, ""),
(28308, 43, 6065.26, 4840.78, -93.5694, ""),
(28308, 44, 6065.73, 4870.6, -94.3074, ""),
(28308, 45, 6119.71, 4901.46, -94.5622, ""),
(28308, 46, 6136.802, 4912.846, -94.969, ""),
(28308, 47, 6143.484, 4916, -94.977, "");

-- https://ru.wowhead.com/quest=12537

DELETE FROM `creature` WHERE (`id`='28273');
DELETE FROM `creature` WHERE (`id`='28253');

UPDATE `creature_template` SET `AIName`='SmartAI', `InhabitType`='4', `flags_extra`='2' WHERE (`entry`='28253');
UPDATE `creature_template` SET `AIName`='SmartAI', `InhabitType`='7' WHERE (`entry`='28273');

delete from gameobject where id in (190503,190502);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
('190502', '190502', '571', '3711', '4306', '1', '1', '0', '6206.04', '4950.96', '-98.8728', '6.07375', '0', '0', '0.938472', '-0.345356', '-300', '0', '1', '0', '', '0', '-1'),
('190503', '190503', '571', '3711', '4306', '1', '1', '0', '6205.2', '4951.75', '-98.8768', '3.83973', '0', '0', '-0.939692', '0.342021', '-120', '255', '1', '0', '', '0', '-1'),
('190504', '190503', '571', '3711', '4306', '1', '1', '0', '6207.12', '4950.45', '-98.8763', '5.48033', '0', '0', '-0.390731', '0.920505', '-120', '255', '1', '0', '', '0', '-1'),
('190501', '190503', '571', '3711', '4306', '1', '1', '0', '6206.69', '4951.79', '-98.9149', '2.74016', '0', '0', '0.979924', '0.19937', '-120', '255', '1', '0', '', '0', '-1');

Delete from conditions where SourceTypeOrReferenceId=13 and SourceEntry=51172;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('13', '1', '51172', '0', '0', '31', '0', '3', '28254', '0', '0', '0', '', ''),
('13', '1', '51172', '0', '1', '31', '0', '3', '28273', '0', '0', '0', '', '');

delete from  event_scripts where id in (18475,18474);
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 

(18475, 2, 9, 190502, 60, 0, 0, 0, 0, 0),
(18475, 2, 9, 190503, 60, 0, 0, 0, 0, 0),
(18475, 2, 9, 190504, 60, 0, 0, 0, 0, 0),
(18475, 2, 9, 190501, 60, 0, 0, 0, 0, 0),

(18474, 1, 10, 28273, 60000, 0, 6206.32, 4951.27, -96.669, 0.6),
(18474, 2, 10, 28253, 40000, 0, 6202.68, 4948.74, -86, 0); 


delete from smart_scripts where source_type=0 and entryorguid in (28253,28273);
delete from smart_scripts where source_type=9 and entryorguid in (28273*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(28253, '', 0, 0, 0, 60, 0, 100, 0, 0, 0, 1000, 3000, 4000, 0, 11, 51172, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(28273, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 80, 28273*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(28273*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 51150, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(28273*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 99, 2, 0, 0, 0, 0, 0, 20, 190502, 0, 0, 0, 0, 0, 0, ""),
(28273*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 1200, 1200, 0, 0, 0, 11, 51172, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(28273*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 99, 3, 0, 0, 0, 0, 0, 15, 190503, 10, 0, 0, 0, 0, 0, ""),
(28273*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 15, 12537, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, "");


-- https://ru.wowhead.com/quest=12540
-- https://ru.wowhead.com/quest=12570

DELETE FROM `creature_questender` WHERE (`id`='28082') AND (`quest`='12540');
DELETE FROM `creature_queststarter` WHERE (`id`='28027') AND (`quest`='12570');

DELETE FROM `quest_start_scripts` WHERE (`id`='12570');
DELETE FROM `quest_start_scripts` WHERE (`id`='12540');
UPDATE `quest_template` SET `StartScript`='0' WHERE (`Id`='12540');
UPDATE `quest_template` SET `StartScript`='0' WHERE (`Id`='12570');

UPDATE `creature_template` SET `AIName`='SmartAI', `flags_extra`='2' WHERE (`entry`='28327');
UPDATE `creature_template` SET `AIName`='SmartAI', `flags_extra`='2' WHERE (`entry`='28077');
UPDATE `creature_template` SET `gossip_menu_id`='9677', `npcflag`='3', `unit_flags`='33024', `unit_flags2`='2048', `AIName`='SmartAI' WHERE (`entry`='28217');

UPDATE `creature_template_addon` SET `auras`='' WHERE (`entry`='28217');
DELETE FROM creature_addon WHERE guid=282170;
INSERT INTO `creature_addon` (`guid`, `bytes1`, `auras`) VALUES 
('282170', '7', '51329');

DELETE FROM `creature` WHERE (`id`='28217');
DELETE FROM `creature` WHERE (`id`='28325');
INSERT INTO`creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES
('282171', '28217', '571', '3711', '4293', '1', '0', '1100', '0', '0', '5427.38', '4591.86', '-135.984', '0.50605', '300', '0', '0', '17277', '24400', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('282170', '28217', '571', '3711', '4293', '1', '0', '1101', '0', '0', '5427.38', '4591.86', '-135.984', '0.50605', '300', '0', '0', '17277', '24400', '0', '0', '0', '536903936', '2049', '0', '0', '0', '0', '', '', '-1');

delete from gossip_menu_option where menu_id=9677;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`) VALUES 
(9677, 0, 0, "<Reach down and pull the Injured Rainspeaker Oracle to its feet.>", 1, 1, 0, 0, 0, 0, "", 27732, 0),
(9677, 1, 0, "I am ready to travel to your village now.", 1, 1, 0, 0, 0, 0, "", 27823, 0);

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (28217,28325,28327,28077) AND `source_type`=0; 
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (28327*100) AND `source_type`=9; 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(28217, 0, 0, 15, 62, 0, 100, 0, 9677, 0, 1000, 1000, 0, 85, 51382, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 15, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(28325, 0, 0, 0, 0, 0, 100, 0, 5000, 9000, 9000, 14000, 0, 11, 34370, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(28325, 0, 1, 2, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(28325, 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(28325, 0, 3, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),

(28217, 0, 1, 0, 19, 0, 100, 0, 12570, 0, 0, 0, 0, 12, 28217, 3, 300000, 0, 0, 1, 8, 0, 0, 0, 5427.38, 4591.86, -135.984, 0.50605, ""),

(28217, 0, 2, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),

(28217, 0, 3, 4, 62, 0, 100, 1, 9677, 1, 2000, 2000, 0, 48, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 4, 5, 61, 0, 100, 1, 0, 0, 0, 0, 0, 83, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 5, 6, 61, 0, 100, 1, 0, 0, 0, 0, 0, 53, 1, 28217, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 6, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),

(28217, 0, 7, 0, 40, 0, 100, 1, 10, 28217, 0, 0, 0, 54, 4000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 8, 9, 55, 0, 100, 1, 10, 28217, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 9, 10, 61, 0, 100, 1, 0, 0, 0, 0, 0, 15, 12570, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 10, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 2, 3000, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 11, 12, 52, 0, 100, 1, 2, 28217, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 2.6, ""),
(28217, 0, 12, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 3, 500, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 13, 0, 52, 0, 100, 1, 3, 28217, 0, 0, 0, 11, 52100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(28217, 0, 16, 0, 58, 0, 100, 0, 16, 28217, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(28077, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(28077, 0, 1, 2, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(28077, 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 8000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(28217, 0, 14, 0, 20, 0, 100, 0, 12540, 15000, 15000, 0, 0, 12, 28327, 2, 18000, 0, 0, 0, 8, 0, 0, 0, 5421.59, 4590.52, -134.284, 0.1107,  ""),

(28327, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 80, 28327*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   ""),

(28327*100, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   ""),
(28327*100, 9, 1, 0, 0, 0, 100, 0, 1600, 1600, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   ""),
(28327*100, 9, 2, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   ""),
(28327*100, 9, 3, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   ""),
(28327*100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 53, 1, 28327, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   "");

Delete from phase_definitions where zoneId=3711 and entry in (10,11);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
('3711', '10', '0', '1100', '0', '0', '0', 'q12540'),
('3711', '11', '0', '1101', '0', '0', '0', 'q12570');

Delete from conditions where SourceTypeOrReferenceId=15 and SourceGroup=9677; 
Delete from conditions where SourceTypeOrReferenceId=26 and SourceGroup=3711 and SourceEntry in (10,11);
Delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=28217 and SourceId=0; 
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES

('26', '3711', '10', '0', '0', '28', '0', '12540', '0', '0', '0', '0', '', ''),
('26', '3711', '10', '0', '1', '8', '0', '12540', '0', '0', '0', '0', '', ''),
('26', '3711', '10', '0', '1', '8', '0', '12570', '0', '0', '1', '0', '', ''),
('26', '3711', '10', '0', '1', '9', '0', '12570', '0', '0', '1', '0', '', ''),
('26', '3711', '11', '0', '0', '8', '0', '12540', '0', '0', '1', '0', '', ''),
('26', '3711', '11', '0', '0', '28', '0', '12540', '0', '0', '1', '0', '', ''),

('15', '9677', '1', '0', '0', '1', '1', '51329', '0', '0', '1', '0', '', ''),
('15', '9677', '1', '0', '0', '9', '0', '12570', '0', '0', '0', '0', '', ''),
('15', '9677', '0', '0', '0', '9', '0', '12540', '0', '0', '0', '0', '', ''),

('22', '1', '28217', '0', '0', '29', '0', '28325', '65', '0', '1', '0', '', ""),
('22', '2', '28217', '0', '0', '43', '0', '28217', '0', '1', '1', '0', '', "");

DELETE FROM `creature_text` WHERE `Entry` IN (28217,28325,28327,28077);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(28077, 0, 0, "Dumb big-tongue lover! You not friend of Frenzyheart no more. Frenzyheart will get you good.", 14, 0, 100, 0, 0, 0, 27798, ""),

(28217, 0, 0, "We wait until you ready.", 12, 0, 100, 1, 0, 0, 27824, ""),
(28217, 0, 1, "Let us know when you ready.", 12, 0, 100, 1, 0, 0, 27825, ""),
(28217, 0, 2, "You tell us when it home time", 12, 0, 100, 1, 0, 0, 27826, ""),
(28217, 1, 0, "Home not far. Come!", 12, 0, 100, 1, 0, 0, 27753, ""),
(28217, 1, 1, "You tell us when it home time", 12, 0, 100, 1, 0, 0, 27751, ""),
(28217, 2, 0, "Thanks!", 12, 0, 100, 1, 0, 0, 27752, ""),
(28217, 3, 0, "Oh no! Some puppy-men followed", 12, 0, 100, 1, 0, 0, 27799, ""),

(28325, 0, 0, "You hear a low guttural growl from nearby.", 16, 0, 100, 0, 0, 0, 27831, ""),
(28325, 1, 0, "The Injured Rainspeaker Oracle rises clumsily to its feet.", 16, 0, 100, 0, 0, 0, 27785, ""),

(28327, 0, 0, "Shaman Vekjik comes out of the underbrush nearby.", 16, 0, 100, 0, 0, 0, 27738, ""),
(28327, 1, 0, "You help Rainspeaker! I saw you help Rainspeaker! Traitor thing!", 12, 0, 100, 0, 0, 0, 27739, ""),
(28327, 2, 0, "If you want to be friends with big-tongues, you not friends with Frenzyheart! I tell village! We no trust you anymore!", 12, 0, 100, 0, 0, 0, 27740, ""),
(28327, 3, 0, "Shaman Vekjik scurries off towards Frenzyheart Hill.", 16, 0, 100, 0, 0, 0, 27741, "");

DELETE FROM `waypoints` WHERE (`entry`='28217');
DELETE FROM `waypoints` WHERE (`entry`='28327');
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES
(28217, 1, 5443.58, 4612.76, -144.892, ""), 
(28217, 2, 5468.49, 4627.9, -142.998, ""), 
(28217, 3, 5474.02, 4630.78, -139.534, ""),
(28217, 4, 5497.92, 4634.08, -134.824, ""),
(28217, 5, 5521.39, 4644.39, -135.829, ""),
(28217, 6, 5545.91, 4647.8, -135.454, ""),
(28217, 7, 5572.07, 4657.78, -135.922, ""),
(28217, 8, 5596.03, 4641.15, -136.671, ""),
(28217, 9, 5621.28, 4617.26, -137.929, ""),
(28217, 10, 5638.37, 4593.01, -137.758, ""),
(28217, 11, 5646.49, 4581.31, -137.709, ""),
(28217, 12, 5671.93, 4588.72, -134.828, ""),
(28217, 13, 5689.64, 4590.14, -128.877, ""),
(28217, 14, 5696.5, 4589, -125.895, ""),
(28217, 15, 5707.49, 4580.08, -119.465, ""),
(28217, 16, 5720.89, 4569.48, -119.832, ""),

(28327, 1, 5400.349, 4592.01, -134.501, ""), 
(28327, 2, 5400.11, 4573.37, -131.222, ""),
(28327, 3, 5391.81, 4539.58, -129.807, "");


-- https://ru.wowhead.com/npc=28027

UPDATE `creature_template_addon` SET `auras`='50503' WHERE (`entry`='28027');

DELETE FROM `smart_scripts` WHERE `entryorguid` IN (28027) AND `source_type`=0; 
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(28027, 0, 0, 0, 0, 0, 100, 0, 5000, 10000, 12000, 16000, 0, 11, 53071, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),

(28027, 0, 2, 8, 62, 0, 100, 0, 9742, 0, 0, 0, 0, 85, 52547, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(28027, 0, 3, 8, 62, 0, 100, 0, 9742, 1, 0, 0, 0, 85, 52547, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(28027, 0, 4, 8, 62, 0, 100, 0, 9742, 2, 0, 0, 0, 85, 52548, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(28027, 0, 5, 8, 62, 0, 100, 0, 9742, 3, 0, 0, 0, 85, 52548, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(28027, 0, 6, 8, 62, 0, 100, 0, 9742, 4, 0, 0, 0, 85, 52549, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(28027, 0, 7, 8, 62, 0, 100, 0, 9742, 5, 0, 0, 0, 85, 52549, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(28027, 0, 8, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(28027, 0, 1, 0, 19, 0, 100, 0, 12574, 0, 0, 0, 0, 57, 38622, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(28027, 0, 9, 0, 20, 0, 100, 0, 12577, 0, 0, 0, 0, 57, 38623, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(28027, 0, 10, 0, 20, 0, 100, 0, 12581, 0, 0, 0, 0, 57, 38624, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "");


delete from gossip_menu_option where menu_id=9742;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`) VALUES 

(9742, 0, 0, "I need to find Lafoo, do you have his bug bag?", 1, 1, 0, 0, 0, 0, "", 28961, 0),
(9742, 1, 0, "I need to find Lafoo, do you have his bug bag?", 1, 1, 0, 0, 0, 0, "", 28961, 0),
(9742, 2, 0, "I need to find Jaloot, do you have his favorite crystal?", 1, 1, 0, 0, 0, 0, "", 28962, 0),
(9742, 3, 0, "I need to find Jaloot, do you have his favorite crystal?", 1, 1, 0, 0, 0, 0, "", 28962, 0),
(9742, 4, 0, "I need to find Moodle, do you have his stress ball?", 1, 1, 0, 0, 0, 0, "", 28963, 0),
(9742, 5, 0, "I need to find Moodle, do you have his stress ball?", 1, 1, 0, 0, 0, 0, "", 28963, 0);

Delete from conditions where SourceTypeOrReferenceId=15 and SourceGroup=9742; 
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES

('15', '9742', '0', '0', '1', '2', '0', '38622', '1', '0', '1', '0', '', ''),
('15', '9742', '0', '0', '2', '2', '0', '38622', '1', '0', '1', '0', '', ''),
('15', '9742', '0', '0', '3', '2', '0', '38622', '1', '0', '1', '0', '', ''),
('15', '9742', '0', '0', '3', '2', '0', '38623', '1', '0', '1', '0', '', ''),
('15', '9742', '0', '0', '3', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '0', '0', '0', '2', '0', '38622', '1', '0', '1', '0', '', ''),

('15', '9742', '1', '0', '0', '2', '0', '38622', '1', '0', '1', '0', '', ''),
('15', '9742', '1', '0', '0', '5', '0', '1105', '224', '0', '0', '0', '', ''),

('15', '9742', '2', '0', '3', '2', '0', '38622', '1', '0', '1', '0', '', ''),
('15', '9742', '2', '0', '3', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '2', '0', '2', '2', '0', '38623', '1', '0', '1', '0', '', ''),
('15', '9742', '2', '0', '3', '2', '0', '38623', '1', '0', '1', '0', '', ''),
('15', '9742', '2', '0', '1', '2', '0', '38623', '1', '0', '1', '0', '', ''),
('15', '9742', '2', '0', '0', '2', '0', '38623', '1', '0', '1', '0', '', ''),

('15', '9742', '3', '0', '0', '2', '0', '38623', '1', '0', '1', '0', '', ''),
('15', '9742', '3', '0', '0', '5', '0', '1105', '224', '0', '0', '0', '', ''),

('15', '9742', '4', '0', '0', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '4', '0', '1', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '4', '0', '2', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '4', '0', '3', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '4', '0', '4', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '4', '0', '5', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '4', '0', '6', '2', '0', '38624', '1', '0', '1', '0', '', ''),
('15', '9742', '4', '0', '6', '2', '0', '38622', '1', '0', '1', '0', '', ''),
('15', '9742', '4', '0', '6', '2', '0', '38623', '1', '0', '1', '0', '', ''),

('15', '9742', '5', '0', '0', '5', '0', '1105', '224', '0', '0', '0', '', ''),
('15', '9742', '5', '0', '0', '2', '0', '38624', '1', '0', '1', '0', '', '');

