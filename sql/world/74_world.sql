-- https://forum.wowcircle.com/showthread.php?t=1052066
-- https://forum.wowcircle.com/showthread.php?t=1052065
delete from world_quest_template where entry in (41896,42023);
INSERT INTO `world_quest_template` (`entry`, `overrideAreaGroupID`, `areaID`, `enabled`, `overrideVariableID`, `overrideVariableValue`) VALUES 
(41896, -1, 7618, 1, 0, 0),
(42023, -1, 8151, 1, 0, 0);

delete from world_quest_loot where quest_id in (42023,41896);
INSERT INTO `world_quest_loot` (`quest_id`, `group_id`, `chance`) VALUES 
(41896, 3, 100),
(42023, 3, 100);

update quest_template set flags = 35193088, questrewardid = 4 where id in (42023,41896);


-- https://forum.wowcircle.com/showthread.php?t=1050370
update quest_template set prevquestid = 40963 where id in (40964,40968);


-- https://forum.wowcircle.com/showthread.php?t=1052280
update quest_template set specialflags = 2, flags = 39845896, flags2 = 4194304 where id = 38834;

delete from smart_scripts where entryorguid in (89362) and id = 3;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(89362, 0, 3, 0, 10, 0, 100, 0, 1, 15, 0, 0, 0, 85, 189764, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, 'Kayn - on ooc los - invoker cast');

delete from conditions where SourceEntry in (89362) and SourceTypeOrReferenceId = 22 and sourcegroup = 4;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 4, 89362, 0, 0, 9, 0, 38834, 0, 0, 0, 0, '', 'Smart if player has quest');


-- https://forum.wowcircle.com/showthread.php?t=1052364
update quest_template set specialflags = 0 where id = 43341;


-- https://forum.wowcircle.com/showthread.php?t=1052674
delete from creature where id = 81913;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(81913, 1116, 6722, 7197, 1, 1, 0, 0, 0, -395.6861, 2252.2073, 26.1504, 3.6, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);