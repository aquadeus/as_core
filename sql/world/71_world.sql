-- https://forum.wowcircle.com/showthread.php?t=1050933
update creature_template set ainame = 'SmartAI' where entry = 93437;

delete from smart_scripts where entryorguid = 93437;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(93437, 0, 0, 0, 19, 0, 100, 0, 43407, 0, 0, 0, 0, 75, 219567, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Darion - on quest accept - apply aura');


-- https://forum.wowcircle.com/showthread.php?t=1050934
update gameobject_template set flags = 0 where entry = 254007;


-- https://forum.wowcircle.com/showthread.php?t=1051040
update creature set movementtype = 0, spawndist = 0 where id = 43814;

update creature_template_wdb set killcredit1 = 0, killcredit2 = 0 where entry = 43814;

delete from conditions where SourceEntry in (82058) and SourceTypeOrReferenceId = 17;
delete from conditions where SourceEntry in (82058) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 82058, 0, 0, 31, 1, 3, 43814, 0, 0, 0, '', 'Cast spell into 43814 creature'),
(17, 0, 82058, 0, 0, 38, 1, 20, 4, 0, 0, 0, '', 'Cast spell if target <= 20%hp');


-- https://forum.wowcircle.com/showthread.php?t=1051062
delete from spell_target_position where id in (232727);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(232727, 1692, 960.6117, -112.6863, 595.9792, 5.54);

delete from gameobject where id = 266886;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(266886, 1220, 7679, 7679, 1, 0, 45240, -1437.05, 1170.29, 319.033, 4.17281, 0, 0, 0.869992, -0.493066, 300, 0, 1, 0, '', 0, -1);

delete from creature where id = 119539;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(119539, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1055.5525, 110.3286, 628.4436, 2.19, 300, 0, 0, 4161, 0, 0);

update quest_template set inprogressphaseid = id, completedphaseid = id where id = 45240;

DELETE FROM `creature_questender` WHERE quest in (45240);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(119539, 45240);

update creature_template set npcflag = 2 where entry = 119539;


-- https://forum.wowcircle.com/showthread.php?t=1051069
delete from creature where id = 88276;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(88276, 1116, 6719, 6909, 1, 1, 0, 0, 0, -791.745, -649.599, 109.079, 5.1, 300, 0, 0, 4161, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1051125

delete from world_quest_template where entry in (42240,42743,42757,48957,43638,42783);
INSERT INTO `world_quest_template` (`entry`, `overrideAreaGroupID`, `areaID`, `enabled`, `overrideVariableID`, `overrideVariableValue`) VALUES 
(42240, -1, 0, 1, 0, 0),
(42743, -1, 0, 1, 0, 0),
(42757, -1, 0, 1, 0, 0),
(48957, -1, 0, 1, 0, 0),
(43638, -1, 7855, 1, 0, 0),
(42783, -1, 8079, 1, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1051153
delete from creature where id in (108578,108580);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(108578, 1220, 0, 0, 1, 1, 0, 0, 0, 3705.62, 899.967, 80.4194, 5.49912, 300, 0, 0, 4161, 0, 0),
(108580, 1220, 0, 0, 1, 1, 0, 0, 0, 3394.61, 1533.57, 60.4524, 2.38495, 300, 0, 0, 4161, 0, 0);

DELETE FROM `gameobject_questender` WHERE quest in (42739,42737);
INSERT INTO `gameobject_questender` (`id`, `quest`) VALUES
(251221, 42739),
(251218, 42737);


-- https://forum.wowcircle.com/showthread.php?t=1051156
update quest_template_objective set amount = 0 where objectid = 138111 and questid = 42738;

DELETE FROM `gameobject_questender` WHERE quest in (42738);
INSERT INTO `gameobject_questender` (`id`, `quest`) VALUES
(251220, 42738);


-- https://forum.wowcircle.com/showthread.php?t=1051164
update quest_template set prevquestid = 26209 where id in (26213,26214);