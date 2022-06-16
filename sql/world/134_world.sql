-- https://forum.wowcircle.com/showthread.php?t=1060812

DELETE FROM `creature` WHERE `id` in (29888,73400,107590); -- со снифов залетело 

-- чуть по заселенке

UPDATE `creature_template` SET `InhabitType`='4' WHERE (`entry`='88426');
UPDATE `creature_template` SET `InhabitType`='4' WHERE (`entry`='85255');

delete from creature where id=88426 and areaId=7302;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `curhealth`, `curmana`, `MovementType`) VALUES 
('88426', '1116', '6722', '7302', '1', '1', '0', '460.35', '1469.35', '105.827', '1.88056', '300', '0', '17', '0', '0'),
('88426', '1116', '6722', '7302', '1', '1', '0', '427.498', '1358.59', '81.9816', '5.25384', '300', '0', '17', '0', '0'),
('88426', '1116', '6722', '7302', '1', '1', '0', '465.528', '1335.26', '82.268', '2.64631', '300', '0', '17', '0', '0'),
('88426', '1116', '6722', '7302', '1', '1', '0', '485.114', '1531.03', '106.991', '3.00761', '300', '0', '17', '0', '0');


delete from creature_template_addon where entry=83772;
INSERT INTO `creature_template_addon` (`entry`, `bytes2`, `auras`) VALUES 
('83772', '1', '86603');

-- https://ru.wowhead.com/npc=83596

delete from creature where id=83596;
delete from creature where id=88426 and areaId=7302;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `MovementType`) VALUES 
('83596', '1116', '6722', '7302', '1', '1', '0', '429.955', '1528.48', '136.244', '5.18726', '300', '0', '0');

-- https://ru.wowhead.com/npc=83653 
-- https://ru.wowhead.com/npc=83654 

UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='83653');
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='83654');

delete from creature where id in (83653,83654);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `MovementType`) VALUES 
('83653', '1116', '6722', '7302', '1', '1', '0', '497.601', '1331.35', '83.2952', '2.73032', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '313.51', '1438.65', '34.944', '5.93081', '180', '5', '1'),
('83653', '1116', '6722', '7302', '1', '1', '0', '428.69', '1323.55', '87.2431', '1.12418', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '444.565', '1318.69', '81.2409', '1.23885', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '352.399', '1424.09', '48.8517', '1.05822', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '454.691', '1343.17', '81.4058', '1.62371', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '434.637', '1365.84', '77.9187', '5.61353', '180', '5', '1'),
('83653', '1116', '6722', '7302', '1', '1', '0', '366.476', '1436.52', '46.3117', '6.03372', '180', '5', '1'),
('83653', '1116', '6722', '7302', '1', '1', '0', '475.901', '1384.95', '98.9544', '2.78296', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '405.973', '1406.53', '59.3795', '2.17427', '180', '5', '1'),
('83653', '1116', '6722', '7302', '1', '1', '0', '418.951', '1417.61', '62.3997', '2.78296', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '440.226', '1403.06', '90.859', '0.803756', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '436.321', '1471.68', '106.783', '5.03156', '180', '5', '1'),
('83653', '1116', '6722', '7302', '1', '1', '0', '444.2', '1448.44', '99.0044', '2.01955', '180', '5', '1'),
('83653', '1116', '6722', '7302', '1', '1', '0', '461.219', '1452.1', '106.476', '3.85737', '180', '0', '0'),
('83653', '1116', '6722', '7302', '1', '1', '0', '422.939', '1458.51', '104.103', '5.39675', '180', '0', '0'),
('83654', '1116', '6722', '7302', '1', '1', '0', '331.653', '1438.43', '38.9715', '0.583027', '180', '5', '1'),
('83654', '1116', '6722', '7302', '1', '1', '0', '484.538', '1333.65', '82.7343', '0.56732', '180', '5', '1'),
('83654', '1116', '6722', '7302', '1', '1', '0', '467.479', '1376.92', '89.2379', '1.91428', '180', '0', '0'),
('83654', '1116', '6722', '7302', '1', '1', '0', '410.627', '1376.71', '71.8083', '0.253162', '180', '0', '0'),
('83654', '1116', '6722', '7302', '1', '1', '0', '450.589', '1422.1', '90.6418', '1.91035', '180', '5', '1'),
('83654', '1116', '6722', '7302', '1', '1', '0', '368.949', '1448.33', '46.1277', '5.43287', '180', '0', '0');

delete from smart_scripts where source_type=0 and entryorguid in (83653,83654);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(83653, '', 0, 0, 0, 9, 0, 100, 0, 0, 0, 20, 12000, 20000, 0, 11, 174436, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(83653, '', 0, 1, 0, 0, 0, 100, 0, 0, 2000, 5000, 13000, 15000, 0, 11, 174433, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),

(83654, '', 0, 0, 0, 9, 0, 100, 0, 0, 0, 5, 7000, 9000, 0, 11, 174438, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(83654, '', 0, 1, 0, 0, 0, 100, 0, 0, 1000, 2000, 8000, 11000, 0, 11, 174439, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "");


-- https://forum.wowcircle.com/showthread.php?t=1061556

-- https://ru.wowhead.com/quest=35924
-- https://ru.wowhead.com/quest=35926

UPDATE `creature_template` SET `dynamicflags`='64', `AIName`='SmartAI' WHERE (`entry`='83615'); 
UPDATE `creature_template` SET `npcflag`='16777216', `unit_flags`='536904448', `dynamicflags`='64', `AIName`='SmartAI', `InhabitType`='3' WHERE (`entry`='83633');

UPDATE `creature_template_addon` SET `bytes1`='0', `bytes2`='0' WHERE (`entry`='83633');
UPDATE `creature_template_addon` SET `auras`='167407 159474' WHERE (`entry`='83615');

delete from smart_scripts where source_type=0 and entryorguid in (83615,83633);
delete from smart_scripts where source_type=9 and entryorguid in (83615*100,83633*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

-- H
(83615, '', 0, 0, 0, 73, 0, 100, 0, 0, 0, 0, 0, 0, 0, 80, 83633*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

-- A
(83633, '', 0, 0, 0, 73, 0, 100, 0, 0, 0, 0, 0, 0, 0, 80, 83633*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(83633*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83633*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 65000, 65000, 0, 0, 0, 81, 16777216, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");


delete from npc_spellclick_spells where npc_entry=83615;
delete from npc_spellclick_spells where npc_entry=83633;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
('83615', '167424', '1', '0'), -- H
('83633', '167457', '1', '0'); -- A

delete from spell_script_names where spell_id in (167424,167457);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
('167424', 'spell_gen_explore_quest_assassins_mark'),
('167457', 'spell_gen_explore_quest_assassins_mark');

delete from conditions where SourceTypeOrReferenceId=18 and SourceGroup=83633;
delete from conditions where SourceTypeOrReferenceId=18 and SourceGroup=83615;
delete from conditions where SourceTypeOrReferenceId=26 and SourceGroup=6722 and SourceEntry in (10,11);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('18', '83615', '167424', '0', '0', '9', '0', '35924', '0', '0', '0', '0', '', 'H'),
('18', '83633', '167457', '0', '0', '9', '0', '35926', '0', '0', '0', '0', '', 'A'),

('26', '6722', '10', '0', '0', '6', '0', '67', '0', '0', '0', '0', '', 'H'),
('26', '6722', '11', '0', '0', '6', '0', '469', '0', '0', '0', '0', '', 'A');

delete from quest_template_objective where QuestID in (35924,35926);
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
('274203', '35926', '1', '0', '1', '113577', '1', '0', '0', '0', '', '2972', '1'),
('274193', '35924', '1', '0', '1', '113573', '1', '0', '0', '0', '', '2971', '1');

UPDATE `creature` SET `phaseMask`='0', `phaseID`='1010' WHERE (`id`='83608');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1010' WHERE (`id`='83615');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1010' WHERE (`id`='83772');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1011' WHERE (`id`='83609');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1011' WHERE (`id`='83773');

-- пока так 
delete from phase_definitions where zoneId=6722 and entry in (10,11);
INSERT INTO phase_definitions (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
('6722', '10', '0', '1010', '0', '0', '0', 'H'),
('6722', '11', '0', '1011', '0', '0', '0', 'A');

delete from creature where id=83633;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `curhealth`, `curmana`, `MovementType`) VALUES 
('83633', '1116', '6722', '7302', '1', '0', '1011', '328.483', '1432.81', '41.0383', '0.40309', '300', '0', '36424', '23578', '0'),
('83633', '1116', '6722', '7302', '1', '0', '1011', '371.559', '1447.65', '47.3286', '5.10865', '300', '0', '36424', '23578', '0'),
('83633', '1116', '6722', '7302', '1', '0', '1011', '434.95', '1479.02', '110.748', '4.70847', '300', '0', '36424', '23578', '0'),
('83633', '1116', '6722', '7302', '1', '0', '1011', '447.538', '1445.45', '98.0382', '1.98883', '300', '0', '36424', '23578', '0'),
('83633', '1116', '6722', '7302', '1', '0', '1011', '398.931', '1415.91', '55.2507', '4.08949', '300', '0', '36424', '23578', '0'),
('83633', '1116', '6722', '7302', '1', '0', '1011', '439.837', '1404.51', '91.3292', '2.9031', '300', '0', '36424', '23578', '0'),
('83633', '1116', '6722', '7302', '1', '0', '1011', '468.823', '1411.6', '97.2734', '4.87609', '300', '0', '36424', '23578', '0'),
('83633', '1116', '6722', '7302', '1', '0', '1011', '444.049', '1316.63', '81.3242', '3.18251', '300', '0', '36424', '23578', '0'),
('83633', '1116', '6722', '7302', '1', '0', '1011', '491.99', '1327.89', '84.7422', '3.09849', '300', '0', '36424', '23578', '0');


-- https://forum.wowcircle.com/showthread.php?t=1061554
-- https://ru.wowhead.com/quest=40890
-- https://ru.wowhead.com/quest=42454
-- https://ru.wowhead.com/quest=43349
-- https://ru.wowhead.com/quest=42213

delete from eventobject_template where entry=1220;
INSERT INTO `eventobject_template` (`entry`, `name`, `radius`, `SpellID`, `WorldSafeLocID`, `ScriptName`) VALUES 
('1220', 'dalaran_QuestCredit', '5', '0', '0', 'SmartEventObject');

delete from eventobject where id=1220;
INSERT INTO `eventobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`) VALUES 
('1220', '1220', '1220', '7502', '8045', '1', '1', '0', '-786.307', '4420.22', '602.463', '0');

UPDATE `gameobject_template` SET `AIName`='SmartGameObjectAI' WHERE `entry` in (246465,246466,246463);
UPDATE `creature_template_wdb` SET `DisplayId1`='63703', `DisplayId2`='0' WHERE (`Entry`='107590');
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='107590');
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='109739');

delete from smart_scripts where source_type=10 and entryorguid in (1220);
delete from smart_scripts where source_type=0 and entryorguid in (170035,106843,993710,107590,109739);
delete from smart_scripts where source_type=1 and entryorguid in (251992,246466,246465,246463);
delete from smart_scripts where source_type=9 and entryorguid in (106843*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(1220, '', 10, 0, 0, 78, 0, 100, 0, 0, 1220, 0, 0, 0, 0, 33, 106815, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1220, '', 10, 1, 0, 78, 0, 100, 0, 0, 1220, 0, 0, 0, 0, 33, 107587, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1220, '', 10, 2, 0, 78, 0, 100, 0, 0, 1220, 0, 0, 0, 0, 33, 109750, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1220, '', 10, 3, 0, 78, 0, 100, 0, 0, 1220, 0, 0, 0, 0, 33, 110409, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

-- в персонал видимость

(246466, '', 1, 0, 0, 20, 0, 100, 0, 0, 40890, 0, 0, 0, 0, 12, 109739, 1, 60000, 0, 0, 1, 8, 0, 0, 0, -830.89, 4456.24, 588.956, 2.293, ""), 

(109739, '', 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 0, 109739, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(109739, '', 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),

(109739, '', 0, 2, 0, 40, 0, 100, 1, 0, 3, 109739, 0, 0, 0, 54, 25000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(109739, '', 0, 3, 4, 55, 0, 100, 1, 0, 3, 109739, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 4.005557, ""),
(109739, '', 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 1, 10000, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(109739, '', 0, 5, 0, 52, 0, 100, 1, 0, 1, 109739, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(109739, '', 0, 6, 0, 40, 0, 100, 1, 0, 5, 109739, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(246465, '', 1, 0, 0, 20, 0, 100, 0, 0, 42213, 0, 0, 0, 0, 12, 106843, 1, 60000, 0, 0, 1, 8, 0, 0, 0, -843.65, 4481.35, 588.939, 4.1, ""),  

(106843, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 80, 106843*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(106843*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 52096, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(106843*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 2.7, ""),
(106843*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(106843*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(106843*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(106843*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(106843*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 2500, 2500, 0, 0, 0, 11, 52096, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(106843*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(246463, '', 1, 0, 0, 20, 0, 100, 0, 0, 42454, 0, 0, 0, 0, 12, 107590, 1, 60000, 0, 0, 1, 8, 0, 0, 0, -830.89, 4456.24, 588.956, 2.293, ""), 

(107590, '', 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 0, 0, 53, 0, 107590, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107590, '', 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(107590, '', 0, 2, 0, 40, 0, 100, 1, 0, 2, 107590, 0, 0, 0, 54, 35000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107590, '', 0, 3, 0, 55, 0, 100, 1, 0, 2, 107590, 0, 0, 0, 1, 1, 15000, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(107590, '', 0, 4, 5, 52, 0, 100, 1, 0, 1, 107590, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(107590, '', 0, 5, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 2, 12000, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(107590, '', 0, 6, 7, 52, 0, 100, 1, 0, 2, 107590, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(107590, '', 0, 7, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(107590, '', 0, 8, 0, 40, 0, 100, 1, 0, 4, 107590, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(170035, '', 0, 0, 1, 10, 0, 100, 0, 0, 1, 1, 1000, 1000, 0, 85, 52096, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Visual Spell"),
(170035, '', 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 62, 1220, 0, 0, 0, 0, 0, 7, 0, 0, 0, -786.307, 4420.22, 602.463, 2.54, "Teleport");


delete from waypoints where entry in (109739,993710,107590);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES

(109739, 1, -833.63, 4458.34, 588.855, "q40890"),
(109739, 2, -844.96, 4466, 588.849, "q40890"),
(109739, 3, -858.048, 4468.393, 588.856, "q40890"),
(109739, 4, -846.55, 4464.22, 588.849, "q40890"),
(109739, 5, -830.9, 4456.27, 588.957, "q40890"),

(107590, 1, -834.93, 4462.31, 588.773, "42454"),
(107590, 2, -835.03, 4476.08, 588.856, "42454"),
(107590, 3, -834.4, 4470.42, 588.773, "42454"),
(107590, 4, -831.42, 4456.88, 588.957, "42454");


UPDATE `creature_text` SET `BroadcastTextID`='115730' WHERE (`Entry`='109739') AND (`GroupID`='0') AND (`ID`='0');
UPDATE `creature_text` SET `Emote`='1', `BroadcastTextID`='115735' WHERE (`Entry`='109739') AND (`GroupID`='1') AND (`ID`='0');
UPDATE `creature_text` SET `BroadcastTextID`='115732' WHERE (`Entry`='109739') AND (`GroupID`='2') AND (`ID`='0');
UPDATE `creature_text` SET `BroadcastTextID`='110987' WHERE (`Entry`='106843') AND (`GroupID`='1') AND (`ID`='0');
UPDATE `creature_text` SET `Emote`='2', `BroadcastTextID`='110988' WHERE (`Entry`='106843') AND (`GroupID`='0') AND (`ID`='0');
UPDATE `creature_text` SET `Emote`='1', `BroadcastTextID`='110989' WHERE (`Entry`='106843') AND (`GroupID`='2') AND (`ID`='0');

delete from creature_text where Entry=107590;
delete from creature_text where Entry=993710;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 
(107590, 0, 0, "When you first set foot upon our shores, I prayed to the spirits asking if you could be trusted. By helping to reunite our tribes, you have proven yourself a true friend to our people.", 12, 0, 100, 0, 0, 61018, 112330, ""),
(107590, 1, 0, "For us, the Hammer of Khaz'goroth has always been a symbol of unity, a reminder of our proud history. But in your hands, it will serve an even greater purpose.", 12, 0, 100, 25, 0, 61020, 112334, ""),
(107590, 2, 0, "On behalf of the tribes of Highmountain, I entrust the hammer to you and your allies. May it strike true at the heart of the enemy!", 12, 0, 100, 25, 0, 61019, 112331, ""),
(107590, 3, 0, "The spirits whisper that our fates our intertwined, $n. My people stand ready to aid you in the war against the Legion.", 12, 0, 100, 16, 0, 61017, 112332, "");


DELETE FROM `creature_template` WHERE (`entry`='993710');
DELETE FROM `creature_template_wdb` WHERE (`Entry`='993710');

delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=170035 and ConditionTypeOrReference=9;


-- https://ru.wowhead.com/quest=24993
-- https://forum.wowcircle.com/showthread.php?t=1056164

UPDATE `creature_template` SET `AIName`='SmartAI', `flags_extra`='2' WHERE (`entry`='38976');
UPDATE `creature_template_wdb` SET `DisplayId1`='11686' WHERE (`Entry`='38976');

delete from smart_scripts where source_type=0 and entryorguid in (38967,38976);   
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(38967, '', 0, 0, 0, 1, 0, 100, 0, 0, 1000, 2000, 2000, 4000, 0, 11, 73179, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(38976, '', 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 73167, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(38976, '', 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 41, 2000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");


delete from creature where id=38967;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `MovementType`) VALUES 

('38967', '0', '85', '159', '1', '1', '1004', '2285.82', '333.954', '33.6391', '1.6098', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2276.49', '356.668', '33.7732', '2.22634', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2244.45', '365.784', '36.1437', '0.559723', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2275.24', '310.573', '33.5966', '3.43505', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2239.51', '350.528', '33.5808', '3.09735', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2208.36', '338.175', '33.6056', '4.70113', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2223.12', '300.878', '33.5742', '4.97994', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2229.59', '263.156', '33.4073', '5.97268', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2270.32', '253.895', '33.6892', '4.7663', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2301.27', '248.259', '33.594', '0.197641', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2300.08', '218.485', '33.318', '4.67809', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2254.82', '219.834', '33.5759', '1.52025', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2256.65', '288.686', '33.3787', '0.932772', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2315.81', '334.662', '37.382', '1.33882', '120', '7', '0', '1'),
('38967', '0', '85', '159', '1', '1', '1004', '2292.98', '366.464', '34.4955', '4.55974', '120', '7', '0', '1');


-- https://forum.wowcircle.com/showthread.php?t=1040637

UPDATE `quest_poi` SET `ObjectiveIndex`='1' WHERE (`QuestID`='33579') AND (`BlobIndex`='1') AND (`Idx1`='2');
UPDATE `quest_poi` SET `ObjectiveIndex`='1' WHERE (`QuestID`='33579') AND (`BlobIndex`='0') AND (`Idx1`='1'); 

-- https://forum.wowcircle.com/showthread.php?t=1061611

UPDATE `quest_poi` SET `ObjectiveIndex`='2' WHERE (`QuestID`='40614') AND (`BlobIndex`='0') AND (`Idx1`='2');
UPDATE `quest_poi` SET `ObjectiveIndex`='3' WHERE (`QuestID`='40614') AND (`BlobIndex`='0') AND (`Idx1`='3');

