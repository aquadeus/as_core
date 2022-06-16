-- https://forum.wowcircle.com/showthread.php?t=1055477
delete from conditions where SourceEntry in (211006) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 3, 211006, 0, 0, 31, 0, 3, 106418, 0, 0, 0, '', 'Cast only 106418');

update creature_template set ainame = 'SmartAI' where entry = 106418;

delete from smart_scripts where entryorguid in (106418,10641800);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(106418, 0, 0, 0, 8, 0, 100, 0, 211006, 0, 2500, 2500, 0, 80, 10641800, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Anari - on spell hit - run timed action list'),

(10641800, 9, 0, 0, 0, 0, 100, 0, 2500, 2500, 0, 0, 0, 33, 106418, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - give credit');


-- https://forum.wowcircle.com/showthread.php?t=1055484
DELETE FROM `creature_questender` WHERE quest in (13751) and id = 33760;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(33760, 13751);

delete from creature where id in (33760,33977);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(33760, 1, 331, 4693, 1, 0, 137980, 0, 0, 2456.8701, -2463.1299, 109.9590, 5.7, 180, 9, 0, 0, 0, 0),
(33977, 1, 331, 4693, 1, 0, 137981, 0, 0, 2456.8701, -2463.1299, 109.9590, 5.7, 180, 9, 0, 0, 0, 0);

delete from phase_definitions where zoneId = 331 and entry in (137980,137981);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(331, 137980, 0, 137980, 0, '13798 quest phase'),
(331, 137981, 0, 137981, 0, '13798 quest phase');

delete from conditions where SourceGroup in (331) and SourceTypeOrReferenceId = 26 and sourceentry in (137980,137981);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 331, 137980, 0, 0, 14, 0, 13798, 0, 0, 0, 0, '', 'Phase if 13798 quest none'),
(26, 331, 137981, 0, 0, 14, 0, 13798, 0, 0, 1, 0, '', 'Phase if 13798 quest not none');


-- https://forum.wowcircle.com/showthread.php?t=1055530
delete from gameobject where id in (301651);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(301651, 1220, 7637, 8232, 1, 1, 0, 2579.3176, 5553.4346, 178.0226, 2.46091, 0, 0, 0, 0, -120, 255, 1, 0, '', 0, -1);

update gameobject_template set data1 = 25 where entry = 301651;

update creature_template set unit_flags = 768 where entry = 106696;

-- https://forum.wowcircle.com/showthread.php?t=1055531
update quest_template set prevquestid = 41197 where id = 41473;


-- https://forum.wowcircle.com/showthread.php?t=1055532
update quest_template set prevquestid = 41473, nextquestid = 41478, exclusivegroup = -41474 where id in (41474,41475);


-- https://forum.wowcircle.com/showthread.php?t=1055536
update quest_template set prevquestid = 41478, nextquestid = 41469, exclusivegroup = -41474 where id in (41480,41479,41485);

update quest_template set prevquestid = 0 where id in (41469);


-- https://forum.wowcircle.com/showthread.php?t=1055539
update quest_template set prevquestid = 41494 where id in (42223);


-- https://forum.wowcircle.com/showthread.php?t=1055651
update quest_template set prevquestid = 40617 where id in (41215);


-- https://forum.wowcircle.com/showthread.php?t=1055655
update creature_template set minlevel = 100, maxlevel = 110, faction = 14, lootid = entry, regenhealth = 1 where entry = 103677;

update quest_template set prevquestid = 41216 where id in (41231);


-- https://forum.wowcircle.com/showthread.php?t=1055948
delete from smart_scripts where entryorguid in (88973,89088,240552,8897300);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(88973, '', 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 80, 8897300, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Athissa - on just summoned - run timed action list'),
(8897300, '', 9, 0, 0, 0, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 11, 178077, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Cast Frost Prison'),
(8897300, '', 9, 1, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Talk'),
(8897300, '', 9, 2, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 1, 889730, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Move'),
(8897300, '', 9, 3, 0, 0, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 75, 178077, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Cast Frost Prison'),
(8897300, '', 9, 4, 0, 0, 0, 100, 1, 0, 7000, 7000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Talk'),
(8897300, '', 9, 5, 0, 0, 0, 100, 1, 0, 7000, 7000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Talk'),
(8897300, '', 9, 6, 0, 0, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 75, 171505, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Cast Fate to black'),
(8897300, '', 9, 7, 0, 0, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 85, 178078, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Cast Teleport to Slave Cave'),
(89088, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 53, 1, 890880, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Move');

delete from spell_script_names where spell_id = 178714;
-- INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
-- (178714, 'spell_valsharah_summon_naga');

delete from spell_scripts where id in (178714);
INSERT INTO `spell_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(178714, 0, 10, 88973, 18000, 0, 282.311, 6267.37, -53.7189, 5.95773),
(178714, 0, 10, 89088, 18000, 0, 283.197, 6270.55, -53.7203, 5.91415);