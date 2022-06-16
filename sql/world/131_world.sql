
-- https://ru.wowhead.com/quest=42005
update creature_template set ainame='SmartAI' where entry in (105736, 106285);

delete from smart_scripts where entryorguid in (106285, 105736) and source_type=0;
delete from smart_scripts where entryorguid=10573600 and source_type=9;
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
('106285', NULL, '0', '0', '1', '54', '0', '100', '0', '0', '0', '0', '0', '0', '0', '207', '0', '0', '0', '0', '0', '0', '23', '0', '0', '0', '0', '0', '0', '0', 'sum pers vis'),
('106285', NULL, '0', '1', '0', '61', '0', '100', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '23', '0', '0', '0', '0', '0', '0', '0', 'sum talk'),

('105736', NULL, '0', '0', '0', '0', '0', '100', '0', '0', '5000', '9000', '15000', '20000', '0', '11', '49841', '0', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '0', '0', 'oc cast'),
('105736', NULL, '0', '1', '0', '1', '0', '100', '0', '0', '20000', '20000', '20000', '20000', '0', '41', '100', '0', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '0', '0', 'ooc despawn'),
('105736', NULL, '0', '2', '0', '54', '0', '100', '0', '0', '0', '0', '0', '0', '0', '80', '10573600', '2', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', 'run ts'),
('105736', NULL, '0', '3', '0', '6', '0', '100', '0', '0', '0', '0', '0', '0', '0', '41', '3000', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', 'despawn'),
('10573600', NULL, '9', '0', '0', '0', '0', '100', '0', '0', '3000', '3000', '0', '0', '0', '49', '0', '0', '0', '0', '0', '0', '23', '0', '0', '0', '0', '0', '0', '0', 'attack');

delete from event_scripts where id in (50649, 54563);
INSERT INTO event_scripts (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
('50649', '0', '10', '106285', '15000', '0', '-103.25', '-4959.14', '310.918', '0.52'),
('54563', '0', '10', '106285', '15000', '0', '-130.528', '-5051.04', '312.923', '2.34');

replace INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES
('106285', '0', '0', 'Text', '41', '0', '100', '66', '0', '0', '108601', '');

replace INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `aiAnimKit`, `movementAnimKit`, `meleeAnimKit`, `auras`) VALUES
('106285', '0', '33554432', '1', '0', '0', '0', '0', '194413');

delete from conditions where SourceTypeOrReferenceId = 17 and SourceEntry=209575;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
('17', '0', '209575', '0', '0', '43', '0', '105736', '2', '2', '0', '0', '', NULL);