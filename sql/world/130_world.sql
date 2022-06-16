-- https://forum.wowcircle.com/showthread.php?t=1060812

UPDATE `quest_template` SET `RequiredClasses`='2991' WHERE (`Id`='24553');
UPDATE `quest_template` SET `RequiredClasses`='2991' WHERE (`Id`='24796');

UPDATE `quest_template` SET `RequiredClasses`='2991' WHERE (`Id`='24564');
UPDATE `quest_template` SET `RequiredClasses`='2991' WHERE (`Id`='24801');


-- https://forum.wowcircle.com/showthread.php?t=848051 

UPDATE `creature_template_wdb` SET `KillCredit1`='40793' WHERE (`Entry`='41634');


-- https://ru.wowhead.com/quest=42124
-- https://forum.wowcircle.com/showthread.php?t=708744

delete from conditions where SourceTypeOrReferenceId=26 and ConditionTypeOrReference=9 and ConditionValue1=42124;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('26', '7558', '20', '0', '0', '9', '0', '42124', '0', '0', '0', '0', '', 'wq42124');


delete from phase_definitions where zoneId=7558 and entry=20;
INSERT INTO phase_definitions (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
('7558', '20', '0', '1200', '0', '0', '0', 'wq42124');


UPDATE `creature` SET `phaseMask`='0',`phaseID`='1200' WHERE `id` in (106372,106384,106369,106358);

replace INTO `creature_template_addon` (`entry`, `bytes1`) VALUES
('106358', '50331648'),
('106372', '50331648');
  
replace INTO `creature_template_addon` (`entry`, `emote`) VALUES 
('106384', '468');
  
UPDATE `creature_template` SET `InhabitType`='4' WHERE (`entry`='106372');
  
delete from smart_scripts where source_type=0 and entryorguid in (106372,106384);
delete from smart_scripts where source_type=0 and id=5 and entryorguid in (106369,106358);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(106384, '', 0, 0, 0, 0, 0, 100, 0, 0, 2000, 2000, 15000, 15000, 0, 11, 7588, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(106384, '', 0, 1, 0, 4, 0, 30, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(106384, '', 0, 2, 0, 0, 0, 100, 0, 0, 5000, 8000, 8000, 10000, 0, 11, 220575, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),

(106369, '', 0, 5, 0, 1, 0, 100, 1, 0, 500, 1000, 0, 0, 0, 11, 210826, 0, 0, 0, 0, 0, 19, 106372, 30, 0, 0, 0, 0, 0, ""),
(106358, '', 0, 5, 0, 1, 0, 100, 1, 0, 500, 1000, 0, 0, 0, 11, 210826, 0, 0, 0, 0, 0, 19, 106372, 30, 0, 0, 0, 0, 0, ""),

(106372, '', 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 210830, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(106372, '', 0, 1, 2, 8, 0, 100, 0, 0, 210328, 0, 30000, 30000, 0, 33, 106372, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(106372, '', 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(106372, '', 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 209, 15, 15, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(106372, '', 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 41, 3000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

delete from quest_poi_points where QuestID=42124;
INSERT INTO quest_poi_points (QuestID, Idx1, Idx2, X, Y, VerifiedBuild) VALUES
('42124', '0', '0', '2241', '6790', '24367'),
('42124', '0', '1', '2314', '6825', '24367'),
('42124', '0', '2', '2317', '6836', '24367'),
('42124', '0', '3', '2290', '6871', '24367'),
('42124', '0', '4', '2217', '6896', '24367'),
('42124', '0', '5', '2214', '6897', '24367'),
('42124', '0', '6', '2180', '6898', '24367'),
('42124', '0', '7', '2109', '6883', '24367'),
('42124', '0', '8', '2122', '6855', '24367'),
('42124', '0', '9', '2149', '6827', '24367'),
('42124', '0', '10', '2177', '6803', '24367'),
('42124', '1', '0', '2241', '6794', '24367'),
('42124', '1', '1', '2294', '6803', '24367'),
('42124', '1', '2', '2318', '6830', '24367'),
('42124', '1', '3', '2299', '6870', '24367'),
('42124', '1', '4', '2184', '6911', '24367'),
('42124', '1', '5', '2067', '6908', '24367'),
('42124', '1', '6', '2123', '6799', '24367'),
('42124', '2', '0', '2255', '6834', '24367'),
('42124', '3', '0', '2164', '6849', '24367');


UPDATE `creature_text` SET `Sound`='61241', `BroadcastTextID`='121831' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='0');
UPDATE `creature_text` SET `Sound`='69162', `BroadcastTextID`='121832' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='1');
UPDATE `creature_text` SET `Sound`='69154', `BroadcastTextID`='121823' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='2');
UPDATE `creature_text` SET `Sound`='69158', `BroadcastTextID`='121827' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='3');
UPDATE `creature_text` SET `Sound`='69157', `BroadcastTextID`='121826' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='4');
UPDATE `creature_text` SET `Sound`='69160', `BroadcastTextID`='121830' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='5');
UPDATE `creature_text` SET `Sound`='69153', `BroadcastTextID`='121822' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='6');
UPDATE `creature_text` SET `Sound`='69155', `BroadcastTextID`='121824' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='7');
UPDATE `creature_text` SET `Sound`='69152', `BroadcastTextID`='121821' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='8');
UPDATE `creature_text` SET `Sound`='69163', `BroadcastTextID`='121833' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='9');
UPDATE `creature_text` SET `Sound`='69156', `BroadcastTextID`='121825' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='10');
UPDATE `creature_text` SET `Sound`='69159', `BroadcastTextID`='121828' WHERE (`Entry`='106358') AND (`GroupID`='0') AND (`ID`='11');

UPDATE `creature_text` SET `Sound`='61241', `BroadcastTextID`='121831' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='0');
UPDATE `creature_text` SET `Sound`='69162', `BroadcastTextID`='121832' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='1');
UPDATE `creature_text` SET `Sound`='69154', `BroadcastTextID`='121823' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='2');
UPDATE `creature_text` SET `Sound`='69158', `BroadcastTextID`='121827' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='3');
UPDATE `creature_text` SET `Sound`='69157', `BroadcastTextID`='121826' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='4');
UPDATE `creature_text` SET `Sound`='69160', `BroadcastTextID`='121830' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='5');
UPDATE `creature_text` SET `Sound`='69153', `BroadcastTextID`='121822' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='6');
UPDATE `creature_text` SET `Sound`='69155', `BroadcastTextID`='121824' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='7');
UPDATE `creature_text` SET `Sound`='69152', `BroadcastTextID`='121821' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='8');
UPDATE `creature_text` SET `Sound`='69163', `BroadcastTextID`='121833' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='9');
UPDATE `creature_text` SET `Sound`='69156', `BroadcastTextID`='121825' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='10');
UPDATE `creature_text` SET `Sound`='69159', `BroadcastTextID`='121828' WHERE (`Entry`='106384') AND (`GroupID`='0') AND (`ID`='11');

UPDATE `creature_text` SET `Sound`='61241', `BroadcastTextID`='121831' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='0');
UPDATE `creature_text` SET `Sound`='69162', `BroadcastTextID`='121832' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='1');
UPDATE `creature_text` SET `Sound`='69154', `BroadcastTextID`='121823' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='2');
UPDATE `creature_text` SET `Sound`='69158', `BroadcastTextID`='121827' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='3');
UPDATE `creature_text` SET `Sound`='69157', `BroadcastTextID`='121826' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='4');
UPDATE `creature_text` SET `Sound`='69160', `BroadcastTextID`='121830' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='5');
UPDATE `creature_text` SET `Sound`='69153', `BroadcastTextID`='121822' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='6');
UPDATE `creature_text` SET `Sound`='69155', `BroadcastTextID`='121824' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='7');
UPDATE `creature_text` SET `Sound`='69152', `BroadcastTextID`='121821' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='8');
UPDATE `creature_text` SET `Sound`='69163', `BroadcastTextID`='121833' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='9');
UPDATE `creature_text` SET `Sound`='69156', `BroadcastTextID`='121825' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='10');
UPDATE `creature_text` SET `Sound`='69159', `BroadcastTextID`='121828' WHERE (`Entry`='106369') AND (`GroupID`='0') AND (`ID`='11');

UPDATE `creature` SET `modelid`='11686' WHERE (`id`='113937');

-- https://ru.wowhead.com/npc=85807
-- https://ru.wowhead.com/npc=85695

UPDATE `creature_template` SET `AIName`='SmartAI', `InhabitType`='4' WHERE (`entry`='85807');
UPDATE `creature_template` SET `AIName`='SmartAI', `InhabitType`='4' WHERE (`entry`='85695');

delete from smart_scripts where source_type=0 and entryorguid in (85807,85695);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(85807, '', 0, 0, 0, 2, 0, 100, 1, 0, 5, 15, 0, 0, 0, 11, 167109, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(85807, '', 0, 1, 0, 0, 0, 100, 0, 0, 3000, 5000, 8000, 12000, 0, 11, 167119, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),

(85695, '', 0, 0, 0, 2, 0, 100, 0, 0, 0, 30, 9000, 11000, 0, 11, 167130, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(85695, '', 0, 1, 0, 0, 0, 100, 0, 0, 3000, 5000, 8000, 12000, 0, 11, 167119, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "");


-- почистить шторм

DELETE FROM `gameobject` WHERE `id` in (243307,186738,185899,187267,187265,190942,208595);

-- https://ru.wowhead.com/quest=10974
-- https://forum.wowcircle.com/showthread.php?t=818856

UPDATE `gameobject` SET `spawntimesecs`='120' WHERE (`id`='185512');

UPDATE `gameobject_template` SET `data2`='15000', `AIName`='SmartGameObjectAI' WHERE (`entry`='185512');
UPDATE `creature_template` SET `faction`='14', `AIName`='SmartAI' WHERE (`entry`='22920');


delete from smart_scripts where source_type=1 and entryorguid in (185512);
delete from smart_scripts where source_type=0 and entryorguid in (22920);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(185512, '', 1, 0, 0, 70, 0, 100, 0, 0, 2, 0, 0, 0, 0, 12, 22920, 1, 80000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(22920, '', 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 30, 0, 0, 0, 0, 0, 0, ""),
(22920, '', 0, 1, 0, 0, 0, 100, 0, 0, 7000, 10000, 18000, 22000, 0, 11, 38777, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(22920, '', 0, 2, 0, 0, 0, 100, 0, 0, 2000, 4000, 12000, 15000, 0, 11, 38770, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "");

-- https://forum.wowcircle.com/showthread.php?t=818543

-- https://ru.wowhead.com/quest=10557
-- https://ru.wowhead.com/quest=10710
-- https://ru.wowhead.com/quest=10711
-- https://ru.wowhead.com/quest=10712


UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='21461');
UPDATE `creature_template` SET `InhabitType`='4', `flags_extra`='130' WHERE (`entry`='21930');
UPDATE `creature_template` SET `InhabitType`='4', `flags_extra`='130' WHERE (`entry`='21942');
UPDATE `creature_template` SET `InhabitType`='4', `flags_extra`='130' WHERE (`entry`='21413');
UPDATE `creature_template` SET `InhabitType`='4', `flags_extra`='130' WHERE (`entry`='21944');
UPDATE `creature_template` SET  `InhabitType`='4' WHERE (`entry`='21393');
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='21460');
UPDATE `creature_template` SET `InhabitType`='4',`AIName`='SmartAI' WHERE (`entry`='21234');


UPDATE `quest_template_locale` SET `AreaDescription`='' WHERE (`ID`='10712');
UPDATE `quest_template` SET `EndText`='' WHERE (`Id`='10712');

replace INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`) VALUES 
(258268, 10712, 0, 1, 1, 21461, 1, 0, 0, 0, "Launch to Ruuan Weald.", ''),
(258270, 10712, 1, 0, 0, 31123, 1, 0, 0, 0, "", '');


replace INTO locales_quest_template_objective (ID, Locale, Description, VerifiedBuild) VALUES 
('258268', '8', 'Отправляйтесь в чащобу Рууан.', '26124');

UPDATE `creature` SET `position_x`='1919.62', `position_y`='5581.82', `position_z`='270.612', `orientation`='5.09' WHERE guid=47027 and id=21234;

DELETE FROM `quest_start_scripts` WHERE `id` in (10557,10710,10712);


delete from smart_scripts where source_type=0 and entryorguid in (21461);
delete from smart_scripts where source_type=0 and entryorguid in (-47027);
delete from smart_scripts where source_type=9 and entryorguid in (21461*100,21461*100+1,21461*100+2,21461*100+3,21461*100+4,21461*100+5,21461*100+6);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(21461, '', 0, 0, 7, 62, 0, 100, 0, 0, 8304, 0, 0, 0, 0, 80, 21461*100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(21461*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 85, 37908, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36790, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36792, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36800, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 86, 37910, 0, 19, 21930, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),


(21461, '', 0, 1, 7, 62, 0, 100, 0, 0, 8454, 0, 0, 0, 0, 80, 21461*100+1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(21461*100+1, '', 9, 0, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 85, 37908, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+1, '', 9, 1, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+1, '', 9, 2, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36790, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+1, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36792, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+1, '', 9, 4, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36800, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+1, '', 9, 5, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 86, 37962, 0, 19, 21942, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+1, '', 9, 6, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+1, '', 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),

(21461, '', 0, 2, 7, 62, 0, 100, 0, 0, 8304, 2, 0, 0, 0, 80, 21461*100+2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(21461*100+2, '', 9, 0, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 85, 37908, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+2, '', 9, 1, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+2, '', 9, 2, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36790, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+2, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36792, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+2, '', 9, 4, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36800, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+2, '', 9, 5, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 86, 36812, 0, 19, 21413, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+2, '', 9, 6, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+2, '', 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),

(21461, '', 0, 3, 7, 62, 0, 100, 0, 0, 8304, 3, 0, 0, 0, 80, 21461*100+3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(21461*100+3, '', 9, 0, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 85, 37908, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+3, '', 9, 1, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+3, '', 9, 2, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36790, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+3, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36792, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+3, '', 9, 4, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36800, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+3, '', 9, 5, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 86, 37968, 0, 19, 21944, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+3, '', 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 21461, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+3, '', 9, 7, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+3, '', 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),

(21461, '', 0, 4, 7, 62, 0, 100, 0, 0, 8455, 1, 0, 0, 0, 80, 21461*100+4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(21461*100+4, '', 9, 0, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 85, 37908, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+4, '', 9, 1, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+4, '', 9, 2, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36790, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+4, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36792, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+4, '', 9, 4, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36800, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+4, '', 9, 5, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 86, 37962, 0, 19, 21942, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+4, '', 9, 6, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+4, '', 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),

(21461, '', 0, 5, 7, 62, 0, 100, 0, 0, 8455, 2, 0, 0, 0, 80, 21461*100+5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(21461*100+5, '', 9, 0, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 85, 37908, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+5, '', 9, 1, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+5, '', 9, 2, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36790, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+5, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36792, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+5, '', 9, 4, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36800, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+5, '', 9, 5, 0, 0, 0, 100, 0, 0, 3100, 3100, 0, 0, 0, 86, 36812, 0, 19, 21413, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+5, '', 9, 6, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+5, '', 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),

(21461, '', 0, 6, 7, 62, 0, 100, 0, 0, 8455, 3, 0, 0, 0, 80, 21461*100+6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(21461, '', 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(21461*100+6, '', 9, 0, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 85, 37908, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+6, '', 9, 1, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+6, '', 9, 2, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36790, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+6, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36792, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+6, '', 9, 4, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 75, 36800, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+6, '', 9, 5, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 86, 37968, 0, 19, 21944, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+6, '', 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 21461, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(21461*100+6, '', 9, 7, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),
(21461*100+6, '', 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 10, 47027, 0, 0, 0, 0, 0, 0, ""),

(-47027, '', 0, 0, 0, 38, 0, 100, 0, 0, 1, 1, 0, 0, 0, 11, 36795, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");


delete from conditions where SourceTypeOrReferenceId=13 and SourceEntry=36795;
delete from conditions where SourceTypeOrReferenceId=17 and SourceEntry=38170;
delete from conditions where SourceTypeOrReferenceId=15 and SourceGroup=8304;
delete from conditions where SourceTypeOrReferenceId=15 and SourceGroup=8455;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('13', '1', '36795', '0', '0', '31', '0', '3', '21393', '0', '0', '0', '', ''),

('17', '0', '38170', '0', '0', '1', '0', '37968', '1', '0', '0', '0', '', ''),

('15', '8304', '0', '0', '0', '9', '0', '10557', '0', '0', '0', '0', '', ''),
('15', '8304', '0', '0', '0', '1', '0', '37108', '0', '0', '1', '0', '', ''),

('15', '8304', '1', '0', '0', '9', '0', '10710', '0', '0', '0', '0', '', ''),
('15', '8304', '1', '0', '0', '1', '0', '37108', '0', '0', '1', '0', '', ''),

('15', '8304', '2', '0', '0', '9', '0', '10711', '0', '0', '0', '0', '', ''),
('15', '8304', '2', '0', '0', '1', '0', '37108', '0', '0', '1', '0', '', ''),

('15', '8304', '3', '0', '0', '9', '0', '10712', '0', '0', '0', '0', '', ''),
('15', '8304', '3', '0', '0', '1', '0', '37108', '0', '0', '1', '0', '', ''),

('15', '8455', '1', '0', '0', '8', '0', '10710', '0', '0', '0', '0', '', ''),
('15', '8455', '1', '0', '0', '1', '0', '37108', '0', '0', '1', '0', '', ''),

('15', '8455', '2', '0', '0', '8', '0', '10711', '0', '0', '0', '0', '', ''),
('15', '8455', '2', '0', '0', '1', '0', '37108', '0', '0', '1', '0', '', ''),

('15', '8455', '3', '0', '0', '8', '0', '10712', '0', '0', '0', '0', '', ''),
('15', '8455', '3', '0', '0', '1', '0', '37108', '0', '0', '1', '0', '', '');


replace INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `duration`, `comment`) VALUES 
('37908', '36801', '100', 'teleport');


delete from spell_script_names where spell_id in (37910, 37962, 36812, 37968);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
('37910', 'spell_gen_soaring_be'),
('37962', 'spell_gen_soaring_be'),
('37968', 'spell_gen_soaring_be'),
('36812', 'spell_gen_soaring_be');

delete from gossip_menu_option where menu_id in (8304,8455);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`) VALUES 

(8304, 0, 0, "I'm ready for my test flight!", 1, 1, 0, 0, 0, 0, "", 19667, 0),
(8304, 1, 0, "Take me to Singing Ridge!", 1, 1, 8454, 0, 0, 0, "", 19671, 0),
(8304, 2, 0, "Take me to Razaan's Landing!", 1, 1, 0, 0, 0, 0, "", 19668, 0),
(8304, 3, 0, "Take me to Ruuan Weald!", 1, 1, 0, 0, 0, 0, "", 19669, 0),
(8304, 4, 0, "I want to fly to an old location!", 1, 1, 8455, 0, 0, 0, "", 19735, 0),

(8455, 1, 0, "Take me to Singing Ridge.", 1, 1, 0, 0, 0, 0, "", 19671, 0),
(8455, 2, 0, "Take me to Razaan's Landing.", 1, 1, 0, 0, 0, 0, "", 19668, 0),
(8455, 3, 0, "Take me to Ruuan Weald.", 1, 1, 0, 0, 0, 0, "", 19669, 0);





