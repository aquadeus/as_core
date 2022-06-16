
-- https://ru.wowhead.com/quest=44058
-- https://forum.wowcircle.net/showthread.php?t=1064177
delete from creature_queststarter where quest=44058;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
('90250', '44058'),
('93568', '44058'),
('96746', '44058'),
('98002', '44058'),
('98092', '44058'),
('99179', '44058'),
('100635', '44058'),
('103023', '44058'),
('103732', '44058'),
('106199', '44058'),
('108515', '44058'),
('110564', '44058'),
('115784', '44058');


UPDATE `quest_template_objective` SET `Type`='0' WHERE (`ID`='286202');

update creature_template set scriptname='npc_suramar_volpin_44058' where entry = 112443;
update creature_template set scriptname='npc_suramar_nightborne_fox_hunter_44058' where entry = 112442;
update creature_template set ainame='SmartAI' where entry = 112433;
UPDATE `creature_template_wdb` SET `KillCredit1`='0', `KillCredit2`='0' WHERE (`Entry`='112442');

delete from creature_text where entry in (112443, 112442, 112433);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES
('112443', '0', '0', '<Волпин дрожит от страха>', '41', '0', '100', '0', '0', '0', '121388', ''),
('112443', '1', '0', '<Волпин прыгает от радости>', '41', '0', '100', '0', '0', '0', '121389', ''),

('112442', '0', '0', 'А ты кто? Где лорд Нимрод и леди Диана?', '12', '0', '100', '0', '0', '0', '121248', ''),
('112442', '0', '1', 'Прочь, это наша охота!', '12', '0', '100', '0', '0', '0', '121249', ''),
('112442', '0', '2', 'Эта лиса наша по праву!', '12', '0', '100', '0', '0', '0', '121250', ''),

('112433', '0', '0', 'How did you get this invitation? Never mind, the hunt\'s already started!', '12', '0', '100', '0', '0', '0', '121242', '');

delete from creature_template_addon where entry = 113107;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `auras`) VALUES 
('113107', '0', '68398', '0', '0', '0', '0', '0', '0', '');

delete from creature where id = 112443;
INSERT INTO `creature` (`linked_id`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES
('', '112443', '1220', '7637', '7637', '1', '0', '10000', '72174', '0', '2128.79', '4988.68', '144.881', '5.18379', '300', '0', '0', '20785340', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('', '112443', '1220', '7637', '7637', '1', '0', '10002', '72174', '0', '2128.79', '4988.68', '144.881', '5.18379', '300', '0', '0', '20785340', '0', '0', '16777216', '0', '0', '0', '0', '0', '0', '0', '', '', '-1');

delete from phase_definitions where zoneId = 7637 and entry IN (10000, 10002);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES
('7637', '10000', '0', '10000', '0', '0', '0', '44058 quest'),
('7637', '10002', '0', '10002', '0', '0', '0', '44058 quest');

delete from conditions where SourceTypeOrReferenceId=26 and SourceGroup=7637 and SourceEntry in (10000, 10002);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
('26', '7637', '10000', '0', '0', '9', '0', '44058', '0', '0', '0', '0', '', 'phase'),
('26', '7637', '10000', '0', '0', '48', '0', '286201', '0', '0', '1', '0', '', 'defend volpin'),

('26', '7637', '10002', '0', '0', '9', '0', '44058', '0', '0', '0', '0', '', 'phase'),
('26', '7637', '10002', '0', '0', '48', '0', '286201', '0', '0', '0', '0', '', 'defend volpin');

delete from smart_scripts where entryorguid = 112433 and source_type=0;
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
('112433', '', '0', '0', '0', '4', '0', '100', '0', '0', '0', '0', '0', '0', '0', '1', '1', '0', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', 'say');

