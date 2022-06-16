-- https://forum.wowcircle.net/showthread.php?t=1062447
-- https://ru.wowhead.com/quest=25112
-- https://ru.wowhead.com/quest=25111
-- https://ru.wowhead.com/quest=25115

UPDATE `creature_template_wdb` SET `KillCredit1`='0', `KillCredit2`='0' WHERE (`Entry`='5419');
UPDATE `creature_template_wdb` SET `KillCredit2`='0' WHERE (`Entry`='5429');
UPDATE `creature_template_wdb` SET `KillCredit2`='0' WHERE (`Entry`='5426');

UPDATE `creature_template` SET `unit_flags`='898', `AIName`='SmartAI' WHERE (`entry`='39696');

delete from smart_scripts where source_type=0 and entryorguid in (5419,39696,5429,5426);
delete from smart_scripts where source_type=9 and entryorguid in (39696*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(5419, '', 0, 0, 0, 0, 0, 100, 0, 0, 4000, 4500, 15000, 16000, 0, 11, 5106, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Cast Crystal Flash"),
(5419, '', 0, 1, 2, 8, 0, 100, 1, 0, 74175, 0, 0, 0, 0, 33, 39702, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(5419, '', 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 11, 74176, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(5419, '', 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 41, 6000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(5429, '', 0, 0, 0, 9, 0, 100, 0, 0, 0, 8, 19000, 22000, 0, 11, 24423, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Cast Demoralizing Screech on Close"),
(5429, '', 0, 1, 0, 0, 0, 100, 0, 0, 6000, 9000, 15000, 15500, 0, 11, 11021, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Cast Flamespit"),
(5429, '', 0, 2, 3, 8, 0, 100, 1, 0, 74175, 0, 0, 0, 0, 33, 40507, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(5429, '', 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 11, 74176, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(5429, '', 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 41, 6000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(5426, '', 0, 0, 0, 9, 0, 100, 0, 0, 0, 9, 15000, 17800, 0, 11, 82797, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, "Cast Leaping Bite on Close"),
(5426, '', 0, 2, 3, 8, 0, 100, 1, 0, 74175, 0, 0, 0, 0, 33, 40509, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(5426, '', 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 11, 74176, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(5426, '', 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 41, 6000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(39696, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 80, 39696*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(39696*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(39696*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(39696*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 1500, 1500, 0, 0, 0, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(39696*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=74175;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
('17', '0', '74175', '0', '0', '36', '1', '0', '0', '0', '1', '0', '', ''),
('17', '0', '74175', '0', '1', '31', '1', '3', '5426', '0', '0', '0', '', ''),
('17', '0', '74175', '0', '1', '31', '1', '3', '5429', '0', '0', '0', '', ''),
('17', '0', '74175', '0', '1', '31', '1', '3', '5419', '0', '0', '0', '', '');

-- https://forum.wowcircle.net/showthread.php?t=1062422

UPDATE `smart_scripts` SET `event_flags`='0' WHERE (`entryorguid`='26692') AND (`source_type`='0');

-- https://forum.wowcircle.net/showthread.php?t=1062375
-- https://ru.wowhead.com/quest=27008

delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=44839 and ConditionValue1=27008;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
('22', '1', '44839', '0', '1', '9', '0', '27008', '0', '0', '0', '0', '', 'H');

-- https://ru.wowhead.com/quest=28729

UPDATE `spell_target_position` SET `target_position_x`='10712.33', `target_position_y`='758.84', `target_position_z`='1321.637', `target_orientation`='2.3973' WHERE (`id`='65656');

UPDATE `creature_template` SET `speed_walk`='0.98', `unit_flags`='33555330', `flags_extra`='2' WHERE (`entry`='34574');

delete from `areatrigger_scripts` where entry=5466;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES ('5466', 'SmartTrigger');

UPDATE `creature` SET `modelid`='27823' WHERE (`id`='34575');

delete from smart_scripts where source_type=0 and entryorguid in (34575,34574);
delete from smart_scripts where source_type=2 and entryorguid in (5466);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(5466, '', 2, 0, 0, 46, 0, 100, 0, 0, 5466, 60000, 60000, 0, 0, 85, 65656, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(34574, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 1, 0, 52, 0, 100, 1, 0, 0, 34574, 0, 0, 0, 1, 1, 3500, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 2, 0, 52, 0, 100, 1, 0, 1, 34574, 0, 0, 0, 53, 0, 34574, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 3, 0, 40, 0, 100, 1, 0, 1, 34574, 0, 0, 0, 54, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 4, 0, 55, 0, 100, 1, 0, 1, 34574, 0, 0, 0, 1, 2, 3500, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""), -- 2000
(34574, '', 0, 5, 0, 40, 0, 100, 1, 0, 3, 34574, 0, 0, 0, 54, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 6, 0, 52, 0, 100, 1, 0, 2, 34574, 0, 0, 0, 1, 3, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 7, 0, 52, 0, 100, 1, 0, 3, 34574, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 8, 9, 58, 0, 100, 1, 0, 5, 34574, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.02, ""),
(34574, '', 0, 9, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 5, 2000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 10, 11, 52, 0, 100, 1, 0, 5, 34574, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34574, '', 0, 11, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 41, 1500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

delete from creature_text where Entry in (34574);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(34574, 0, 0, "%s fades into existence as you approach, nodding a subtle greeting.", 16, 0, 100, 0, 0, 0, 34832, ""),
(34574, 1, 0, "Much has changed for our people since the Battle of Mount Hyjal.", 12, 0, 100, 1, 0, 0, 34792, ""),
(34574, 2, 0, "Nordrassil lies a pale shadow of what it once was, its power used to defeat Archimonde and drive back the Burning Legion.", 12, 0, 100, 1, 0, 0, 34793, ""),
(34574, 3, 0, "Our immortality-the very essence of our beings!-was lost.", 12, 0, 100, 1, 0, 0, 34794, ""),
(34574, 4, 0, "The Betrayer was freed from his prison, and Shand'do Stormrage disappeared into the emerald dream", 12, 0, 100, 1, 0, 0, 34795, ""),
(34574, 5, 0, "A dark time for all.", 12, 0, 100, 1, 0, 0, 34796, ""),
(34574, 6, 0, "The Shade of the Kaldorei closes its eyes and fades away.", 16, 0, 100, 0, 0, 0, 34820, "");

Delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=5466 and SourceId=2;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('22', '1', '5466', '2', '0', '29', '0', '34574', '0', '0', '1', '0', '', ''),
('22', '1', '5466', '2', '0', '9', '0', '28729', '0', '0', '0', '0', '', '');

DELETE FROM `waypoints` WHERE `entry`=34574;
INSERT INTO `waypoints` (`entry`,`pointid`,`position_x`,`position_y`,`position_z`,`point_comment`) VALUES

(34574,1,10702.9423, 761.374, 1322.905,""),
(34574,2,10704.4101, 769.148, 1322.599,""),
(34574,3,10704.9922, 768.6823, 1322.6256,""),
(34574,4,10706.7061, 766.8176, 1321.7551,""),
(34574,5,10713.9882, 761.775, 1321.589,"");

-- https://ru.wowhead.com/quest=28731

UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='3515');
UPDATE `creature_template` SET `speed_walk`='0.7', `unit_flags`='33555330', `flags_extra`='2' WHERE (`entry`='34586');

delete from smart_scripts where source_type=0 and entryorguid in (3515,34586);
delete from smart_scripts where source_type=9 and entryorguid in (34586*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(3515, '', 0, 0, 0, 20, 0, 100, 0, 0, 28731, 0, 0, 0, 0, 12, 34586, 1, 34000, 0, 0, 1, 8, 0, 0, 0, 9735.6787, 951.6224, 1294.1696, 2.2, ""), 

(34586, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 80, 34586*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(34586*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 53, 0, 34586, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 7000, 7000, 0, 0, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 1500, 1500, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 8, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34586*100, '', 9, 9, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

DELETE FROM `waypoints` WHERE `entry`=34586;
INSERT INTO `waypoints` (`entry`,`pointid`,`position_x`,`position_y`,`position_z`,`point_comment`) VALUES
(34586, 1, 9734.7607, 953.3847, 1294.8176,  ''),
(34586, 2, 9733.3662, 955.1306, 1295.2303, '');

delete from creature_text where Entry in (34586);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(34586, 0, 0, "%s smiles at you knowingly as it approaches.", 16, 0, 100, 0, 0, 0, 34835, ""),
(34586, 1, 0, "With Malfurion Stormrage gone, the druids were left in disarray and in desperate need of a new leader.", 12, 0, 100, 1, 0, 0, 34797, ""),
(34586, 2, 0, "Arch Druid Fandral Staghelm roused the druids and convinced the Circle of Ancients in Darkshore that it was time for our people to rebuild.", 12, 0, 100, 0, 0, 0, 34798, ""),
(34586, 3, 0, "In an effort to regain immortality for our people, and with the approval of the Circle, Staghelm and the most powerful druids grew Teldrassil, upon whose boughs you now stand", 12, 0, 100, 0, 0, 0, 34799, ""),
(34586, 4, 0, "Though we have our new home, our immortal lives have not been restored.", 12, 0, 100, 1, 0, 0, 34800, ""),
(34586, 5, 0, "The Shade of the Kaldorei closes its eyes and fades away.", 16, 0, 100, 0, 0, 0, 34820, "");

-- https://ru.wowhead.com/quest=929

delete from `areatrigger_scripts` where entry=5467;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES ('5467', 'SmartTrigger');

UPDATE `creature_template` SET  `unit_flags`='33555330', `flags_extra`='2' WHERE (`entry`='34583');

delete from smart_scripts where source_type=0 and entryorguid in (34583);
delete from smart_scripts where source_type=2 and entryorguid in (5467);
delete from smart_scripts where source_type=9 and entryorguid in (34583*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(5467, '', 2, 0, 0, 46, 0, 100, 0, 0, 5467, 60000, 60000, 0, 0, 85, 65668, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(34583, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 80, 34583*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(34583*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 4000, 4000, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 9000, 9000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 8000, 8000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 7500, 7500, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 8, 0, 0, 0, 100, 0, 0, 8000, 8000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34583*100, '', 9, 9, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

delete from creature_text where Entry in (34583);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(34583, 0, 0, "%s stares into the moonwell, lost in distant memories.", 16, 0, 100, 0, 0, 0, 34819, ""),
(34583, 1, 0, "The Arch Druid approached the dragons for their blessings, as the dragons had placed on Nordrassil in ancient times.", 12, 0, 100, 0, 0, 0, 34801, ""),
(34583, 2, 0, "But Nozdormu, Lord of Time, refused to give his blessing, chiding the druid for his arrogance.", 12, 0, 100, 1, 0, 0, 34802, ""),
(34583, 3, 0, "In agreement, Alexstrasza also refused Staghelm, and without her blessing, Teldrassil's growth has been flawed and unpredictable.", 12, 0, 100, 1, 0, 0, 34803, ""),
(34583, 4, 0, "Staghelm retreated to his enclave in Darnassus, ever seeking a new direction by which to bless Teldrassil, and restore the immortality of our people.", 12, 0, 100, 1, 0, 0, 34804, ""),
(34583, 5, 0, "The Shade of the Kaldorei closes its eyes and fades away.", 16, 0, 100, 0, 0, 0, 34820, "");

Delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=5467 and SourceId=2;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('22', '1', '5467', '2', '0', '29', '0', '34583', '0', '0', '1', '0', '', ''),
('22', '1', '5467', '2', '0', '9', '0', '929', '0', '0', '0', '0', '', '');

-- https://ru.wowhead.com/quest=7383

delete from `areatrigger_scripts` where entry=5468;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES ('5468', 'SmartTrigger');

UPDATE `spell_target_position` SET `target_position_x`='10662.4', `target_position_y`='1857.03', `target_position_z`='1323.81', `target_orientation`='6.15' WHERE (`id`='65672');

UPDATE `creature_template` SET  `speed_walk`='0.6', `unit_flags`='33555330', `flags_extra`='2' WHERE (`entry`='34584');

delete from smart_scripts where source_type=0 and entryorguid in (34584);
delete from smart_scripts where source_type=2 and entryorguid in (5468);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(5468, '', 2, 0, 0, 46, 0, 100, 0, 0, 5468, 60000, 60000, 0, 0, 85, 65672, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(34584, '', 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 0, 34584, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 2, 0, 40, 0, 100, 1, 0, 3, 34584, 0, 0, 0, 54, 16000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 3, 0, 55, 0, 100, 1, 0, 3, 34584, 0, 0, 0, 1, 1, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 4, 0, 52, 0, 100, 1, 0, 1, 34584, 0, 0, 0, 1, 2, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 5, 0, 52, 0, 100, 1, 0, 2, 34584, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 6, 7, 40, 0, 100, 1, 0, 5, 34584, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.11, ""),
(34584, '', 0, 7, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 4, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 8, 0, 52, 0, 100, 1, 0, 4, 34584, 0, 0, 0, 1, 5, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 9, 10, 52, 0, 100, 1, 0, 5, 34584, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34584, '', 0, 10, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 41, 1500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

delete from creature_text where Entry in (34584);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(34584, 0, 0, "%s approaches the moonwell slowly, a familiar, thoughtful look upon its face.", 16, 0, 100, 0, 0, 0, 34834, ""),
(34584, 1, 0, "Over time, Staghelm only grew more disconnected from our people.", 12, 0, 100, 1, 0, 0, 34805, ""),
(34584, 2, 0, "Some pose that he blames us, his people, for the loss of his son, long ago during the War of the Shifting Sands.", 12, 0, 100, 1, 0, 0, 34806, ""),
(34584, 3, 0, "Others cite rumors of his escalating conflicts with Tyrande Whisperwind, the shining light of our people, our leader in Darnassus.", 12, 0, 100, 1, 0, 0, 34807, ""),
(34584, 4, 0, "But many druids of the Cenarion Circle stand firm that he wants only what is best for all of the Kaldorei, and serve him unerringly towards that end.", 12, 0, 100, 1, 0, 0, 34808, ""),
(34584, 5, 0, "Fandral Staghelm carries the burden of our sins and our follies, and in so doing deserves the respect of us all.", 12, 0, 100, 1, 0, 0, 34809, ""),
(34584, 6, 0, "The Shade of the Kaldorei closes its eyes and fades away.", 16, 0, 100, 2, 0, 0, 34820, "");

Delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=5468 and SourceId=2;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('22', '1', '5468', '2', '0', '29', '0', '34584', '0', '0', '1', '0', '', ''),
('22', '1', '5468', '2', '0', '9', '0', '7383', '0', '0', '0', '0', '', '');

delete from waypoints where entry=34584;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES 
('34584', '1', '10664.8', '1856.77', '1324.48', ''),
('34584', '2', '10667.6', '1858.06', '1325.06', ''),
('34584', '3', '10669.4', '1857.65', '1325.97', ''),
('34584', '4', '10669.5', '1860.1', '1325.92', ''),
('34584', '5', '10670.7', '1863.59', '1325.57', '');

-- https://ru.wowhead.com/quest=933

DELETE FROM `creature` WHERE (`guid`='11536823');

delete from `areatrigger_scripts` where entry=5469;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES ('5469', 'SmartTrigger');

UPDATE `spell_target_position` SET `target_position_x`='9556', `target_position_y`='1648', `target_position_z`='1300.953' WHERE (`id`='65674');

UPDATE `creature_template` SET  `speed_walk`='0.7', `unit_flags`='33555330', `flags_extra`='2' WHERE (`entry`='34585'); 
replace INTO `creature_template_addon` (`entry`, `bytes1`) VALUES ('34585', '1');

delete from smart_scripts where source_type=0 and entryorguid in (34585);
delete from smart_scripts where source_type=2 and entryorguid in (5469);
delete from smart_scripts where source_type=9 and entryorguid in (34585*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(5469, '', 2, 0, 0, 46, 0, 100, 0, 0, 5469, 60000, 60000, 0, 0, 85, 65674, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(34585, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 80, 34585*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(34585*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 2500, 2500, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 8000, 8000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 8000, 8000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 7500, 7500, 0, 0, 0, 91, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 97, 7, 7, 0, 0, 0, 0, 8, 0, 0, 0, 9556.72, 1653.03, 1299.5609,  0, ""),
(34585*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 1100, 1100, 0, 0, 0, 53, 0, 34585, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,  0, ""),
(34585*100, '', 9, 8, 0, 0, 0, 100, 0, 0, 3500, 3500, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.18, ""),
(34585*100, '', 9, 9, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 10, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 11, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 12, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 1, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(34585*100, '', 9, 13, 0, 0, 0, 100, 0, 0, 1500, 1500, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");


delete from creature_text where Entry in (34585);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(34585, 0, 0, "%s rests on the side of the moonwell in peaceful contemplation.", 16, 0, 100, 0, 0, 0, 34833, ""),
(34585, 1, 0, "In weeks past, all of those living atop Teldrassil have found their memories blurry, as if time itself seized a portion of their lives.", 12, 0, 100, 0, 0, 0, 34810, ""),
(34585, 2, 0, "We break from this haze to find that Fandral Staghelm and many of his closest followers have disappeared without a word.", 12, 0, 100, 0, 0, 0, 34811, ""),
(34585, 3, 0, "But in his place, Shan'do Stormrage awoke from the dream and returned to his beloved Tyrande Whisperwind.", 12, 0, 100, 0, 0, 0, 34812, ""),
(34585, 4, 0, "And with his return came Alexstrasza and Ysera, to at long last offer their blessings upon Teldrassil.", 12, 0, 100, 0, 0, 0, 34813, ""),
(34585, 5, 0, "Though a small corruption lingers, desperately longing to be cleansed, the boughs of Teldrassil itself seem changed. Growth has begun anew.", 12, 0, 100, 1, 0, 0, 34814, ""),
(34585, 6, 0, "None know what to expect next, but with Malfurion and Tyrande reunited, a new hope dawns on the Kaldorei.", 12, 0, 100, 1, 0, 0, 34815, ""),
(34585, 7, 0, "Hope for lasting relief from the shadow that has been cast on our people for so long.", 12, 0, 100, 1, 0, 0, 44123, ""),
(34585, 8, 0, "The Shade of the Kaldorei closes its eyes and fades away.", 16, 0, 100, 0, 0, 0, 34820, "");

Delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=5469 and SourceId=2;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('22', '1', '5469', '2', '0', '29', '0', '34585', '0', '0', '1', '0', '', ''),
('22', '1', '5469', '2', '0', '9', '0', '933', '0', '0', '0', '0', '', '');

DELETE FROM `waypoints` WHERE `entry`=34585;
INSERT INTO `waypoints` (`entry`,`pointid`,`position_x`,`position_y`,`position_z`,`point_comment`) VALUES
(34585, 1, 9559.2959, 1655.8757, 1299.7266,  '');

-- https://ru.wowhead.com/quest=6341 - теперь не будет отправлять в  Кратер Ун'Горо

DELETE FROM `smart_scripts` WHERE (`entryorguid`='40553') AND (`source_type`='0');

-- https://ru.wowhead.com/npc=48623

UPDATE `creature_template` SET `InhabitType`='4' WHERE (`entry`='48623');

-- https://forum.wowcircle.net/showthread.php?t=1061907
-- https://ru.wowhead.com/shadowfang-keep
-- 46964

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='1') AND (`SourceEntry`='21525');
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='1') AND (`SourceEntry`='44731');
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='1') AND (`SourceEntry`='22206');
delete from conditions where SourceTypeOrReferenceId=1003 and SourceEntry in (21525,44731,22206) and ConditionTypeOrReference in (2,12);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('1003', '0', '21525', '0', '0', '2', '0', '21525', '1', '0', '1', '0', '', ''),
('1003', '0', '21525', '0', '0', '12', '0', '2', '0', '0', '0', '0', '', ''),

('1003', '0', '44731', '0', '0', '2', '0', '44731', '1', '0', '1', '0', '', ''),
('1003', '0', '44731', '0', '0', '12', '0', '8', '0', '0', '0', '0', '', ''),

('1003', '0', '22206', '0', '0', '2', '0', '22206', '1', '0', '1', '0', '', ''),
('1003', '0', '22206', '0', '0', '12', '0', '8', '0', '0', '0', '0', '', '');

UPDATE `creature_template` SET `lootid`='4046964' WHERE (`entry`='4046964');

delete from creature_loot_template where entry in (46964,4046964);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(4046964, 1, 100, 0, 0, -63456, 2, 0, 0, 0),
(4046964, 71637, 5, 0, 0, 1, 1, 0, 0, 0),
(4046964, 52078, 100, 0, 0, 1, 1, 0, 0, 0),
(4046964, 22206, 50, 0, 0, 1, 1, 0, 0, 0),
(4046964, 44731, 50, 0, 0, 1, 1, 0, 0, 0),
(4046964, 21525, 100, 0, 0, 1, 1, 0, 0, 0),

(46964, 2, 100, 0, 0, -6220, 1, 0, 0, 0),
(46964, 21525, 100, 0, 0, 1, 1, 0, 0, 0),
(46964, 60877, -100, 0, 0, 1, 1, 0, 0, 0),
(46964, 60882, -100, 0, 0, 1, 1, 0, 0, 0);

delete from reference_loot_template where entry in (6220,63456);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(6220, 6220, 0, 1, 1, 1, 1, 0, 0, 0),
(6220, 3748, 0, 1, 1, 1, 1, 0, 0, 0),
(6220, 6318, 0, 1, 1, 1, 1, 0, 0, 0),
(6220, 6641, 0, 1, 1, 1, 1, 0, 0, 0),
(6220, 151071, 0, 1, 1, 1, 1, 0, 0, 0),
(6220, 151072, 0, 1, 1, 1, 1, 0, 0, 0),
(6220, 151073, 0, 1, 1, 1, 1, 0, 0, 0),

(63456, 63456, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 63457, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 63458, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 63459, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 63461, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 63462, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 63463, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 63464, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 63465, 0, 1, 1, 1, 1, 0, 0, 0),
(63456, 157617, 0, 1, 1, 1, 1, 0, 0, 0);

-- 46963
UPDATE `creature_template` SET `lootid`='4046963' WHERE (`entry`='4046963');

delete from creature_loot_template where entry in (46963,4046963);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(4046963, 1, 100, 0, 0, -63452, 1, 0, 0, 0),

(46963, 2, 100, 1, 0, -1292, 1, 0, 0, 0),
(46963, 60876, -100, 1, 0, 1, 1, 0, 0, 0),
(46963, 60881, -100, 1, 0, 1, 1, 0, 0, 0);

delete from reference_loot_template where entry in (1292,63452);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(63452, 63452, 0, 1, 1, 1, 1, 0, 0, 0),
(63452, 63450, 0, 1, 1, 1, 1, 0, 0, 0),
(63452, 63453, 0, 1, 1, 1, 1, 0, 0, 0),
(63452, 63454, 0, 1, 1, 1, 1, 0, 0, 0),
(63452, 63455, 0, 1, 1, 1, 1, 0, 0, 0),

(1292, 1292, 0, 1, 1, 1, 1, 0, 0, 0),
(1292, 6341, 0, 1, 1, 1, 1, 0, 0, 0),
(1292, 6642, 0, 1, 1, 1, 1, 0, 0, 0),
(1292, 3230, 0, 1, 1, 1, 1, 0, 0, 0),
(1292, 132566, 0, 1, 1, 1, 1, 0, 0, 0);

-- 4278
UPDATE `creature_template` SET `lootid`='404278' WHERE (`entry`='404278');

delete from creature_loot_template where entry in (4278,404278);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(404278, 1, 100, 1, 0, -63445, 1, 0, 0, 0),
(404278, 71715, 5, 0, 0, 1, 1, 0, 0, 0),

(4278, 2, 100, 1, 0, -3191, 1, 0, 0, 0),
(4278, 60879, -100, 1, 0, 1, 1, 0, 0, 0),
(4278, 60880, -100, 1, 0, 1, 1, 0, 0, 0);

delete from reference_loot_template where entry in (3191,63445);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(63445, 63445, 0, 1, 1, 1, 1, 0, 0, 0),
(63445, 63446, 0, 1, 1, 1, 1, 0, 0, 0),
(63445, 63447, 0, 1, 1, 1, 1, 0, 0, 0),
(63445, 63448, 0, 1, 1, 1, 1, 0, 0, 0),
(63445, 63449, 0, 1, 1, 1, 1, 0, 0, 0),

(3191, 3191, 0, 1, 1, 1, 1, 0, 0, 0),
(3191, 6320, 0, 1, 1, 1, 1, 0, 0, 0),
(3191, 151067, 0, 1, 1, 1, 1, 0, 0, 0),
(3191, 151068, 0, 1, 1, 1, 1, 0, 0, 0),
(3191, 151069, 0, 1, 1, 1, 1, 0, 0, 0),
(3191, 151070, 0, 1, 1, 1, 1, 0, 0, 0);

-- 3887
UPDATE `creature_template` SET `lootid`='403887' WHERE (`entry`='403887');

delete from creature_loot_template where entry in (403887,3887);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(403887, 1, 100, 1, 0, -63438, 1, 0, 0, 0),

(3887, 2, 100, 1, 0, -6319, 1, 0, 0, 0),
(3887, 60878, -100, 1, 0, 1, 1, 0, 0, 0),
(3887, 60885, -100, 1, 0, 1, 1, 0, 0, 0);

delete from reference_loot_template where entry in (63438,6319);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(63438, 63438, 0, 1, 1, 1, 1, 0, 0, 0),
(63438, 63439, 0, 1, 1, 1, 1, 0, 0, 0),
(63438, 63440, 0, 1, 1, 1, 1, 0, 0, 0),
(63438, 63441, 0, 1, 1, 1, 1, 0, 0, 0),
(63438, 63444, 0, 1, 1, 1, 1, 0, 0, 0),
 
(6319, 6319, 0, 1, 1, 1, 1, 0, 0, 0),
(6319, 6321, 0, 1, 1, 1, 1, 0, 0, 0),
(6319, 132568, 0, 1, 1, 1, 1, 0, 0, 0),
(6319, 132567, 0, 1, 1, 1, 1, 0, 0, 0),
(6319, 5943, 0, 1, 1, 1, 1, 0, 0, 0),
(6319, 5254, 0, 1, 1, 1, 1, 0, 0, 0);

-- 46962
UPDATE `creature_template` SET `lootid`='4046962' WHERE (`entry`='4046962');

delete from creature_loot_template where entry in (4046962,46962);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(4046962, 1, 100, 1, 0, -63433, 1, 0, 0, 0),

(46962, 2, 100, 1, 0, -6314, 1, 0, 0, 0);

delete from reference_loot_template where entry in (63433,6314);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 

(63433, 63433, 0, 1, 1, 1, 1, 0, 0, 0),
(63433, 63434, 0, 1, 1, 1, 1, 0, 0, 0),
(63433, 63435, 0, 1, 1, 1, 1, 0, 0, 0),
(63433, 63436, 0, 1, 1, 1, 1, 0, 0, 0),
(63433, 63437, 0, 1, 1, 1, 1, 0, 0, 0),

(6314, 6314, 0, 1, 1, 1, 1, 0, 0, 0),
(6314, 6323, 0, 1, 1, 1, 1, 0, 0, 0),
(6314, 6324, 0, 1, 1, 1, 1, 0, 0, 0);


UPDATE `creature_loot_template` SET `ChanceOrQuestChance`='-50' WHERE (`item`='60874');
UPDATE `creature_loot_template` SET `ChanceOrQuestChance`='-50' WHERE (`item`='60875');

delete from creature_loot_template where entry in (47132,47136) and item=60874;
delete from creature_loot_template where entry in (3877,3873) and item=60875;

INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(47132, 60874, -50, 1, 0, 1, 1, 0, 0, 0),
(47136, 60874, -50, 1, 0, 1, 1, 0, 0, 0),

(3877, 60875, -50, 1, 0, 1, 1, 0, 0, 0),
(3873, 60875, -50, 1, 0, 1, 1, 0, 0, 0);

-- https://ru.wowhead.com/quest=28401
-- https://ru.wowhead.com/quest=28465

DELETE FROM `creature_questender` WHERE (`id`='4087') AND (`quest`='27225');
DELETE FROM `creature_questender` WHERE (`id`='52292') AND (`quest`='27270');
DELETE FROM `creature_questender` WHERE (`id`='23127') AND (`quest`='27270');
DELETE FROM `creature_questender` WHERE (`id`='52292') AND (`quest`='28287');
DELETE FROM `creature_questender` WHERE (`id`='23127') AND (`quest`='28287');
delete from creature_questender where quest in (28301,27283,27280,27273,27279,27267,28298,28262,27265,27304,27298,27334,27331,28304,28164,27281,27266);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES 
(12042, 27283),
(3344, 28301),
(72939, 28301),
(72939, 27280),
(3344, 27280),
(12042, 27273),
(47233, 27279),
(918, 27267),
(13283, 27267),
(47233, 28298),
(13283, 28262),
(928, 27265),
(44725, 27304),
(23128, 27298),
(45339, 27334),
(44735, 27331),
(45339, 28304),
(45347, 28164),
(3353, 27281),
(44247, 27266);

UPDATE `creature_template` SET `flags_extra`='130' WHERE (`entry`='48516');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceEntry`=90292; 
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
('17', '0', '90292', '0', '0', '36', '1', '0', '0', '0', '1', '0', '', ''),
('17', '0', '90292', '0', '0', '31', '1', '3', '8909', '0', '0', '0', '', '');

delete from smart_scripts where source_type=0 and id=10 and entryorguid in (8909);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(8909, '', 0, 10, 0, 8, 0, 100, 1, 0, 90292, 0, 0, 0, 0, 47, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

UPDATE `quest_template` SET `RequiredRaces`='32' WHERE (`Id`='27331');
UPDATE `quest_template` SET `RequiredRaces`='256' WHERE (`Id`='28164'); 