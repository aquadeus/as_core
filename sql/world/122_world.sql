-- https://forum.wowcircle.com/showthread.php?t=1060147
DELETE FROM `creature_questender` WHERE id in (104406);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(104406, 40364),
(104406, 41618),
(104406, 41606);

DELETE FROM `creature_queststarter` WHERE id in (104406);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(104406, 40364),
(104406, 41618);


-- https://forum.wowcircle.com/showthread.php?t=1060151
DELETE FROM `creature_queststarter` WHERE id in (102334);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(102334, 40320),
(102334, 40334),
(102334, 40331);


-- https://forum.wowcircle.com/showthread.php?t=1060153
DELETE FROM `creature_questender` WHERE quest in (42839);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(107598, 42839);

DELETE FROM `creature_queststarter` WHERE quest in (42839);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(97140, 42839);

update quest_template set PrevQuestId = 44084 where id = 42839;


-- https://forum.wowcircle.com/showthread.php?t=1060158
DELETE FROM `creature_questender` WHERE quest in (43502);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(103155, 43502);

DELETE FROM `creature_queststarter` WHERE quest in (43502);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(107126, 43502);

update quest_template set PrevQuestId = 43362 where id = 43502;


-- https://forum.wowcircle.com/showthread.php?t=1060281
delete from gameobject where id in (211032,211035,211068,211064,211065,211067,211062,211063,211036,211052,211051,211054,211050,211053) and map = 870;

delete from creature where id = 59284;

update creature_template set AIName = 'SmartAI', unit_flags = 768 where entry in (59284);
update creature_template set AIName = 'SmartAI', flags_extra = 128, unit_flags = 768 where entry in (59276,59278,59279,59280);

delete from smart_scripts where entryorguid in (59276,59278,59279,59280,59284);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(59284, 0, 0, 1, 60, 0, 100, 1, 9000, 9000, 0, 0, 0, 11, 124556, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'barel - on event update - cast spell'),
(59284, 0, 1, 2, 61, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 59278, 10, 0, 0, 0, 0, 0, 'barel - on link - set data'),
(59284, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 59279, 10, 0, 0, 0, 0, 0, 'barel - on link - set data'),
(59284, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 59280, 10, 0, 0, 0, 0, 0, 'barel - on link - set data'),
(59284, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 59276, 10, 0, 0, 0, 0, 0, 'barel - on link - set data'),

(59278, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 33, 59278, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'trigger - on data set - give credit'),
(59279, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 33, 59279, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'trigger - on data set - give credit'),
(59280, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 33, 59280, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'trigger - on data set - give credit'),
(59276, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 0, 33, 59276, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'trigger - on data set - give credit');

delete from conditions where SourceEntry in (113897) and SourceTypeOrReferenceId = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 113897, 0, 0, 29, 0, 59278, 10, 0, 0, 0, '', 'cast near creature'),
(17, 0, 113897, 0, 1, 29, 0, 59279, 10, 0, 0, 0, '', 'cast near creature'),
(17, 0, 113897, 0, 2, 29, 0, 59280, 10, 0, 0, 0, '', 'cast near creature'),
(17, 0, 113897, 0, 3, 29, 0, 59276, 10, 0, 0, 0, '', 'cast near creature');


-- https://forum.wowcircle.com/showthread.php?t=1060285
delete from quest_start_scripts where id = 27003;
INSERT INTO `quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(27003, 0, 8, 44833, 0, 0, 0, 0, 0, 0),
(27003, 0, 6, 1, 0, 0, -10994.9355, -1255.9995, 13.2438, 4.69);


-- https://forum.wowcircle.com/showthread.php?t=1060514
update quest_template set StartScript = id where id = 25070;

delete from quest_start_scripts where id = 25070;
INSERT INTO `quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(25070, 0, 10, 39077, 300000, 0, -9774.2275, -2789.4038, 9.6214, 3.17);

delete from smart_scripts where entryorguid in (39077) and id in (3,4);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(39077, 0, 3, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, -9790.1562, -2789.6782, 9.6217, 0, 'guardian - on just summoned - move to point'),
(39077, 0, 4, 0, 34, 0, 100, 1, 0, 1, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 30, 0, 0, 0, 0, 0, 0, 'guardian - on movement inform - attack');

delete from creature where id = 39077;


-- https://forum.wowcircle.com/showthread.php?t=1060389
update creature_template set npcflag = 16777216, unit_flags2 = 0, unit_flags = 768 where entry = 107988;

update creature_template set npcflag = 16777216, AIName = 'SmartAI', spell1 = 214357, spell2 = 214476 where entry = 108081;

delete from npc_spellclick_spells where npc_entry in (107988,108082);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(107988, 214537, 1, 0),
(108082, 214537, 1, 0);

delete from conditions where SourceGroup in (107988,108082) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 108082, 214537, 0, 0, 9, 0, 42630, 0, 0, 0, 0, '', 'Spellclick if quest incomplete'),
(18, 107988, 214537, 0, 0, 9, 0, 42630, 0, 0, 0, 0, '', 'Spellclick if quest incomplete');

delete from conditions where SourceEntry in (214538) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 214538, 0, 0, 31, 0, 3, 107988, 0, 0, 0, '', 'cast into creature');

delete from smart_scripts where entryorguid in (108081);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(108081, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'launcher - on just summoned - invoker cast'),
(108081, 0, 1, 0, 28, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'launcher - on passenger remove - despawn');

update quest_template set Method = 2 where id = 42630;

delete from conditions where SourceEntry in (214473,214479) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 7, 214473, 0, 0, 31, 0, 3, 108052, 0, 0, 0, '', 'cast into creature'),
(13, 7, 214473, 0, 1, 31, 0, 3, 108060, 0, 0, 0, '', 'cast into creature'),
(13, 7, 214479, 0, 0, 31, 0, 3, 108052, 0, 0, 0, '', 'cast into creature'),
(13, 7, 214479, 0, 1, 31, 0, 3, 108060, 0, 0, 0, '', 'cast into creature');

delete from spell_target_position where id = 214538;
INSERT INTO `spell_target_position` (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`, `VerifiedBuild`) VALUES 
(214538, 0, 1220, 4163.9268, 4398.5674, 768.4413, 0.3, 1);

delete from creature where id in (108052,108060);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4245.84, 4447.4, 788.569, 3.70744, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4213.95, 4433.43, 792.226, 3.78206, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4203.16, 4407.23, 781.028, 3.4836, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4203.01, 4409.18, 794.223, 3.81347, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4213.85, 4393.98, 794.223, 3.49931, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4219.57, 4409.03, 793.948, 3.50324, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4207.96, 4426.84, 792.428, 3.731, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4247.47, 4394.18, 790.053, 3.46397, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4240.96, 4367.56, 787.561, 2.99666, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4232.8, 4408.94, 793.184, 3.40506, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4224.08, 4415.73, 778.973, 3.43255, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4210.34, 4425.37, 777.07, 3.53466, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4206.75, 4436.31, 780.321, 3.85274, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4220.88, 4400.3, 779.456, 3.3815, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4227.08, 4373.4, 779.456, 3.10661, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4246.61, 4409.38, 789.491, 3.3815, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4252.45, 4390.69, 789.693, 3.28725, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4254.06, 4419.79, 780.451, 3.40899, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4224.56, 4397.71, 787.256, 3.46789, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4214.2, 4372.71, 784.263, 3.16551, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4201.57, 4396.23, 780.44, 3.38935, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4197.45, 4413.37, 780.44, 3.64461, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4243.51, 4451.52, 792.515, 3.6878, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4237.16, 4433.19, 791.021, 3.64461, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108052, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4242.26, 4374.88, 789.857, 3.16159, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4221.46, 4431.2, 783.868, 3.58255, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4240.45, 4414.31, 819.293, 3.43882, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4236.09, 4440.14, 812.714, 3.71371, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4265.55, 4440.24, 797.233, 3.49538, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4271.28, 4424.9, 797.233, 3.51108, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4268.31, 4403.76, 795.141, 3.47574, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4272.93, 4395.65, 795.437, 3.27547, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4264.47, 4382.52, 793.07, 3.2951, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4264.21, 4384.25, 783.445, 3.2362, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4262.79, 4399.11, 787.104, 3.2362, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4257.53, 4415.92, 786.478, 3.39013, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4253.09, 4433.4, 786.478, 3.59748, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4258.78, 4392.64, 798.141, 3.32651, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4238.25, 4409.29, 793.053, 3.35007, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4225.29, 4420.31, 798.246, 3.65245, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4260, 4381.39, 796.39, 3.30609, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4261.82, 4365.96, 796.39, 3.06262, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4244.67, 4385.16, 811.125, 3.18828, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4245.36, 4399.44, 819.119, 3.25033, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4229.46, 4449.43, 801.018, 3.58176, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4263.78, 4431.04, 802.897, 3.51108, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4268.73, 4413.06, 802.897, 3.40112, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4215.18, 4389.96, 777.8, 3.65009, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4227.1, 4415.73, 783.868, 3.39405, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4227.75, 4397.74, 778.328, 3.48908, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4231.3, 4382.76, 784.34, 3.35949, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4213.87, 4407.72, 774.058, 3.38698, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4209.78, 4421.55, 779.553, 3.49301, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4237.85, 4432.12, 801.018, 3.65638, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4246.6, 4409.34, 801.018, 3.47181, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(108060, 1220, 7503, 7731, 1, 0, 42630, 0, 0, 4245.4, 4374.99, 797.083, 3.00057, 10, 0, 0, 1039267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=963977
delete from creature_template_addon where entry = 74547;


-- https://forum.wowcircle.com/showthread.php?t=963949
DELETE FROM `creature_questender` WHERE id in (114719);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(114719, 48375),
(114719, 48376);