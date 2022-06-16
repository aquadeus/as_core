-- https://forum.wowcircle.net/showthread.php?t=1043324
update gameobject_template set flags = 0 where entry = 174626;


-- https://forum.wowcircle.net/showthread.php?t=801524
update gameobject_template set scriptname = '' where entry = 191229;


-- https://forum.wowcircle.net/showthread.php?t=803506
delete from creature where id in (18818,19009,21236,21237);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(18818, 530, 3483, 3483, 1, 1, 0, 0, 0, -159.8386, 2517.4131, 52.3794, 6.07375, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(19009, 530, 3483, 3483, 1, 1, 0, 0, 0, -178.3495, 2773.1482, 41.5732, 5.95157, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(21236, 530, 3483, 3483, 1, 1, 0, 0, 0, -170.7093, 2819.9402, 36.8523, 5.98648, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(21237, 530, 3483, 3483, 1, 1, 0, 0, 0, -154.1082, 2662.1008, 54.7223, 3.57792, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set ainame = 'SmartAI', unit_flags = 768, flags_extra = 128, inhabittype = 7 where entry in (18818,19009,21236,21237);

delete from smart_scripts where entryorguid in (18818,19009,21236,21237);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(18818, 0, 0, 1, 8, 0, 100, 0, 32979, 0, 0, 0, 0, 33, 18818, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(19009, 0, 0, 1, 8, 0, 100, 0, 32979, 0, 0, 0, 0, 33, 19009, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(21236, 0, 0, 1, 8, 0, 100, 0, 32979, 0, 0, 0, 0, 33, 21236, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(21237, 0, 0, 1, 8, 0, 100, 0, 32979, 0, 0, 0, 0, 33, 21237, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),

(18818, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 34386, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - cast spell'),
(19009, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 34386, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - cast spell'),
(21236, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 34386, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - cast spell'),
(21237, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 34386, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - cast spell');

delete from conditions where SourceEntry in (32979) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 32979, 0, 0, 31, 0, 3, 18818, 0, 0, 0, '', 'Cast only 18818'),
(13, 1, 32979, 0, 1, 31, 0, 3, 19009, 0, 0, 0, '', 'Cast only 19009'),
(13, 1, 32979, 0, 2, 31, 0, 3, 21236, 0, 0, 0, '', 'Cast only 21236'),
(13, 1, 32979, 0, 3, 31, 0, 3, 21237, 0, 0, 0, '', 'Cast only 21237');


-- https://forum.wowcircle.net/showthread.php?t=802394
delete from gameobject_template where entry in (201743,201781,201810,201744,202460);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `data32`, `RequiredLevel`, `WorldEffectID`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES 
(201743, 10, 6211, 'Field Banner', '', 'Capturing', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 93, 0, 0, 0, 0, 1, 0, 0, 0, 0, 70263, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11724, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'SmartGameObjectAI', '', 15595),
(201781, 10, 6211, 'Field Banner', '', 'Capturing', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 93, 0, 0, 0, 0, 1, 0, 0, 0, 0, 70365, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11724, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'SmartGameObjectAI', '', 15595),
(201810, 10, 6211, 'Field Banner', '', 'Capturing', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 93, 0, 0, 0, 0, 1, 0, 0, 0, 0, 70506, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11724, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'SmartGameObjectAI', '', 15595),
(201744, 5, 5652, 'Horde Field Banner', '', '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 15354),
(202460, 5, 5651, 'Alliance Field Banner', '', '', '', 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 16135);

-- 73301 70261
-- 73328 70364
-- 73329 70505

delete from spell_script_names where spell_id in (70263,70365,70506);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(70263, 'spell_gen_flag_capture'),
(70365, 'spell_gen_flag_capture'),
(70506, 'spell_gen_flag_capture');

delete from gameobject where id in (201743,201781,201810, 201748,202462);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
-- (201810, 1, 4709, 4850, 1, 1, 0, -2600.22, -2074.71, 97.6528, 1.0472, 0, 0, 0, 1, 120, 100, 1, 0, '', 0, -1),
(201781, 1, 4709, 4850, 1, 1, 0, -2129.87, -2080.86, 99.2929, 1.0472, 0, 0, 0, 1, 120, 100, 1, 0, '', 0, -1);
-- (201743, 1, 4709, 4850, 1, 1, 0, -2466.41, -2306.98, 100.23, 5.48033, 0, 0, 0, 1, 120, 100, 1, 0, '', 0, -1);

delete from event_scripts where id in (22815,23857);
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
-- h
(22815, 0, 10, 37667, 180000, 0, -2129.87, -2080.86, 99.2929, 3.246),
-- a
(23857, 0, 10, 39029, 180000, 0, -2129.87, -2080.86, 99.2929, 3.246);


delete from spell_linked_spell where spell_trigger in (73328,70364) and spell_effect in (70297,73321);
-- INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`, `actiontype`) VALUES 
-- (70364, 70297, 0, 0, 0, 0, 0, '', 0),
-- (73328, 73321, 0, 0, 0, 0, 0, '', 0);

update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry in (37667,39029);
update creature_template set ainame = 'SmartAI' where entry in (37659,39040, 37585,37706);
update creature_template_wdb set killcredit1 = 0 where killcredit1 in (37667,37668,39029,39030);
update creature_template_wdb set killcredit2 = 0 where killcredit2 in (37667,37668,39029,39030);

delete from smart_scripts where entryorguid in (201743,201781,201810, 37667,39029,3766700,3902900, 37659,39040, 37585,37706);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(37667, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 80, 3766700, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on update - run timed script'),
(37667, 0, 1, 2, 60, 0, 100, 1, 3000, 3000, 0, 0, 0, 11, 70288, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on update - cast spell'),
(37667, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 11, 70297, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - cast spell'),
(37667, 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 37667, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Trigger - on link - give credit'),


(3766700, 9, 0 , 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 50, 201744, 180, 0, 0, 0, 0, 8, 0, 0, 0, -2129.87, -2080.86, 99.2929, 1.047, 'Timed Script - summon gameobject'),
(3766700, 9, 1 , 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 50, 201748, 180, 0, 0, 0, 0, 8, 0, 0, 0, -2113.86, -2075.02, 97.1212, 0.558, 'Timed Script - summon gameobject'),
(3766700, 9, 2 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 50, 201748, 180, 0, 0, 0, 0, 8, 0, 0, 0, -2137.42, -2065.79, 98.7162, 1.832, 'Timed Script - summon gameobject'),
(3766700, 9, 3 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 50, 201748, 180, 0, 0, 0, 0, 8, 0, 0, 0, -2134.44, -2094.07, 98.729, 4.3284, 'Timed Script - summon gameobject'),
(3766700, 9, 4 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 37659, 1, 174000, 0, 0, 0, 8, 0, 0, 0, -2116.94, -2076.90, 97.3638, 0.51, 'Timed Script - summon creature'),
(3766700, 9, 5 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 37659, 1, 174000, 0, 0, 0, 8, 0, 0, 0, -2133.15, -2090.75, 98.9526, 4.27, 'Timed Script - summon creature'),
(3766700, 9, 6 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 37659, 1, 174000, 0, 0, 0, 8, 0, 0, 0, -2135.84, -2070.39, 99.1650, 1.83, 'Timed Script - summon creature'),
(3766700, 9, 7 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 37659, 1, 174000, 0, 0, 0, 8, 0, 0, 0, -2137.78, -2089.21, 99.1409, 4.05, 'Timed Script - summon creature'),
(3766700, 9, 8 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Timed Script - talk'),
(3766700, 9, 9 , 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2153.5120, -2086.8525, 96.6156, 0, 'Timed Script - summon creature'),
(3766700, 9, 10, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2112.9624, -2096.6606, 96.0484, 0, 'Timed Script - summon creature'),
(3766700, 9, 11, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2145.2632, -2102.4304, 97.3058, 0, 'Timed Script - summon creature'),
(3766700, 9, 12, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2133.3396, -2056.1099, 97.6597, 0, 'Timed Script - summon creature'),
(3766700, 9, 13, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2149.9326, -2067.6953, 96.8584, 0, 'Timed Script - summon creature'),
(3766700, 9, 14, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2115.8914, -2057.5947, 96.4374, 0, 'Timed Script - summon creature'),
(3766700, 9, 15, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2153.5120, -2086.8525, 96.6156, 0, 'Timed Script - summon creature'),
(3766700, 9, 16, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2112.9624, -2096.6606, 96.0484, 0, 'Timed Script - summon creature'),
(3766700, 9, 17, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2145.2632, -2102.4304, 97.3058, 0, 'Timed Script - summon creature'),
(3766700, 9, 18, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2133.3396, -2056.1099, 97.6597, 0, 'Timed Script - summon creature'),
(3766700, 9, 19, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37585, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2149.9326, -2067.6953, 96.8584, 0, 'Timed Script - summon creature'),
(3766700, 9, 20, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 33, 37668, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Timed Script - give credit'),
(3766700, 9, 21, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Timed Script - talk'),


(39029, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 80, 3902900, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on just summoned - run timed script'),
(39029, 0, 1, 2, 60, 0, 100, 1, 3000, 3000, 0, 0, 0, 11, 73322, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on update - cast spell'),
(39029, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 11, 73321, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - cast spell'),
(39029, 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 39029, 2, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Trigger - on link - give credit'),

(3902900, 9, 0 , 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 50, 202460, 180, 0, 0, 0, 0, 8, 0, 0, 0, -2129.87, -2080.86, 99.2929, 1.0472, 'Timed Script - summon gameobject'),
(3902900, 9, 1 , 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 50, 202462, 180, 0, 0, 0, 0, 8, 0, 0, 0, -2124.72, -2070.48, 98.824, 1.11701, 'Timed Script - summon gameobject'),
(3902900, 9, 2 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 50, 202462, 180, 0, 0, 0, 0, 8, 0, 0, 0, -2141.22, -2081.92, 99.4353, 3.3859, 'Timed Script - summon gameobject'),
(3902900, 9, 3 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 50, 202462, 180, 0, 0, 0, 0, 8, 0, 0, 0, -2121.5, -2091.06, 97.9053, 2.11185, 'Timed Script - summon gameobject'),
(3902900, 9, 4 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 39040, 1, 174000, 0, 0, 0, 8, 0, 0, 0, -2139.35, -2081.47, 99.3297, 3.38, 'Timed Script - summon creature'),
(3902900, 9, 5 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 39040, 1, 174000, 0, 0, 0, 8, 0, 0, 0, -2125.54, -2072.36, 98.9959, 1.08, 'Timed Script - summon creature'),
(3902900, 9, 6 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 39040, 1, 174000, 0, 0, 0, 8, 0, 0, 0, -2123.16, -2088.50, 98.2936, 5.22, 'Timed Script - summon creature'),
(3902900, 9, 7 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 39040, 1, 174000, 0, 0, 0, 8, 0, 0, 0, -2128.06, -2088.64, 98.9189, 5.12, 'Timed Script - summon creature'),
(3902900, 9, 8 , 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Timed Script - talk'),
(3902900, 9, 9 , 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2153.5120, -2086.8525, 96.6156, 0, 'Timed Script - summon creature'),
(3902900, 9, 10, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2112.9624, -2096.6606, 96.0484, 0, 'Timed Script - summon creature'),
(3902900, 9, 11, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2145.2632, -2102.4304, 97.3058, 0, 'Timed Script - summon creature'),
(3902900, 9, 12, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2133.3396, -2056.1099, 97.6597, 0, 'Timed Script - summon creature'),
(3902900, 9, 13, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2149.9326, -2067.6953, 96.8584, 0, 'Timed Script - summon creature'),
(3902900, 9, 14, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2115.8914, -2057.5947, 96.4374, 0, 'Timed Script - summon creature'),
(3902900, 9, 15, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2153.5120, -2086.8525, 96.6156, 0, 'Timed Script - summon creature'),
(3902900, 9, 16, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2112.9624, -2096.6606, 96.0484, 0, 'Timed Script - summon creature'),
(3902900, 9, 17, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2145.2632, -2102.4304, 97.3058, 0, 'Timed Script - summon creature'),
(3902900, 9, 18, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2133.3396, -2056.1099, 97.6597, 0, 'Timed Script - summon creature'),
(3902900, 9, 19, 0, 0, 0, 100, 0, 15000, 15000, 0, 0, 0, 12, 37706, 1, 180000, 0, 0, 0, 8, 0, 0, 0, -2149.9326, -2067.6953, 96.8584, 0, 'Timed Script - summon creature'),
(3902900, 9, 20, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 33, 39030, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Timed Script - give credit'),
(3902900, 9, 21, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Timed Script - talk'),

(37706, 0, 0, 1, 54, 0, 100, 1, 3000, 3000, 0, 0, 0, 11, 70288, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Defender - on just summoned - cast spell'),
(37706, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'Defender - on link - start attack'),

(37585, 0, 0, 1, 54, 0, 100, 1, 3000, 3000, 0, 0, 0, 11, 73322, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Defender - on just summoned - cast spell'),
(37585, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'Defender - on link - start attack'),


(37659, 0, 0, 0, 54, 0, 100, 1, 3000, 3000, 0, 0, 0, 11, 70288, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Defender - on just summoned - cast spell'),
(37659, 0, 1, 0, 1, 0, 100, 1, 0, 0, 0, 0, 0, 5, 333, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Defender - ooc - play emote'),

(39040, 0, 0, 0, 54, 0, 100, 1, 3000, 3000, 0, 0, 0, 11, 73322, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Defender - on just summoned - cast spell'),
(39040, 0, 1, 0, 1, 0, 100, 1, 0, 0, 0, 0, 0, 5, 333, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Defender - ooc - play emote');

DELETE FROM `creature_text` WHERE `entry` in (39029,37667);
INSERT INTO `creature_text` (`entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `comment`) VALUES 
(39029, 0, 0, 'Защищай холм!', 41, 0, 100, 0, 0, 0, 37214, ''),
(39029, 1, 0, 'Победа! Холм твой!', 41, 0, 100, 0, 0, 0, 37215, ''),
(37667, 0, 0, 'Защищай холм!', 41, 0, 100, 0, 0, 0, 37214, ''),
(37667, 1, 0, 'Победа! Холм твой!', 41, 0, 100, 0, 0, 0, 37215, '');


-- https://forum.wowcircle.net/showthread.php?t=825024
delete from spell_loot_template where entry in (228214);
INSERT INTO `spell_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(228214, 105, 100, 0, 0, -105, 1);

delete from reference_loot_template where entry in (105);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(105, 140582, 0, 0, 0, 1, 1),
(105, 140583, 0, 0, 0, 1, 1),
(105, 139809, 0, 0, 0, 1, 1),
(105, 139801, 0, 0, 0, 1, 1),
(105, 139866, 0, 0, 0, 1, 1),
(105, 139865, 0, 0, 0, 1, 1),
(105, 139811, 0, 0, 0, 1, 1),
(105, 139814, 0, 0, 0, 1, 1),
(105, 139802, 0, 0, 0, 1, 1),
(105, 139864, 0, 0, 0, 1, 1),
(105, 139808, 0, 0, 0, 1, 1),
(105, 140571, 0, 0, 0, 1, 1);


-- https://forum.wowcircle.net/showthread.php?t=803511
delete from conditions where SourceEntry in (36374) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 2, 36374, 0, 0, 31, 0, 3, 22401, 0, 0, 0, '', 'Cast only 22401'),
(13, 2, 36374, 0, 1, 31, 0, 3, 21182, 0, 0, 0, '', 'Cast only 21182'),
(13, 2, 36374, 0, 2, 31, 0, 3, 22402, 0, 0, 0, '', 'Cast only 22402'),
(13, 2, 36374, 0, 3, 31, 0, 3, 22403, 0, 0, 0, '', 'Cast only 22403');

update gameobject set id = 185293, spawntimesecs = -120 where id = 300120;

select guid from creature where id = 21173 and position_x like '-1162%' into @trigger1;
select guid from creature where id = 21173 and position_x like '-738%' into @trigger2;

update creature_template set unit_flags = 768, flags_extra = 128 where entry in (22401,21182,22402,22403);

update creature_template set ainame = 'SmartAI' where entry in (22406,22405,22404,21170,22401,21182,22402,22403,21173);

delete from smart_scripts where entryorguid in (22406,22405,22404,21170,2117303,2117302,2117301,2117300,22401,21182,22402,22403,21173);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 

(22406, 0, 6, 0, 40, 100, 0, 0, 15, 2240602, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22406, 0, 5, 0, 40, 100, 0, 0, 15, 2240601, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22406, 0, 4, 0, 40, 100, 0, 0, 15, 2240600, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22406, 0, 3, 0, 38, 100, 0, 0, 0, 3, 0, 0, 0, 53, 1, 2240602, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22406, 0, 2, 0, 38, 100, 0, 0, 0, 2, 0, 0, 0, 53, 1, 2240601, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22406, 0, 1, 0, 38, 100, 0, 0, 0, 1, 0, 0, 0, 53, 1, 2240600, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22406, 0, 0, 0, 11, 100, 0, 0, 0, 0, 0, 0, 0, 11, 36350, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Spawn - Add aura'),

(22405, 0, 8, 0, 40, 100, 0, 0, 14, 2240503, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22405, 0, 7, 0, 40, 100, 0, 0, 12, 2240502, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22405, 0, 6, 0, 40, 100, 0, 0, 13, 2240501, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22405, 0, 5, 0, 40, 100, 0, 0, 13, 2240500, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22405, 0, 4, 0, 38, 100, 0, 0, 0, 4, 0, 0, 0, 53, 1, 2240503, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22405, 0, 3, 0, 38, 100, 0, 0, 0, 3, 0, 0, 0, 53, 1, 2240502, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22405, 0, 2, 0, 38, 100, 0, 0, 0, 2, 0, 0, 0, 53, 1, 2240501, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22405, 0, 1, 0, 38, 100, 0, 0, 0, 1, 0, 0, 0, 53, 1, 2240500, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22405, 0, 0, 0, 11, 100, 0, 0, 0, 0, 0, 0, 0, 11, 36350, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Spawn - Add aura'),

(22404, 0, 8, 0, 40, 100, 0, 0, 12, 2240403, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22404, 0, 7, 0, 40, 100, 0, 0, 12, 2240402, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22404, 0, 6, 0, 40, 100, 0, 0, 11, 2240401, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22404, 0, 5, 0, 40, 100, 0, 0, 12, 2240400, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(22404, 0, 4, 0, 38, 100, 0, 0, 0, 4, 0, 0, 0, 53, 1, 2240403, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22404, 0, 3, 0, 38, 100, 0, 0, 0, 3, 0, 0, 0, 53, 1, 2240402, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22404, 0, 2, 0, 38, 100, 0, 0, 0, 2, 0, 0, 0, 53, 1, 2240401, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22404, 0, 1, 0, 38, 100, 0, 0, 0, 1, 0, 0, 0, 53, 1, 2240400, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(22404, 0, 0, 0, 11, 100, 0, 0, 0, 0, 0, 0, 0, 11, 36350, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Spawn - Add aura'),

(21170, 0, 8, 0, 40, 100, 0, 0, 11, 2117003, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(21170, 0, 7, 0, 40, 100, 0, 0, 11, 2117002, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(21170, 0, 6, 0, 40, 100, 0, 0, 10, 2117001, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(21170, 0, 5, 0, 40, 100, 0, 0, 10, 2117000, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Reach WP - Despawn'),
(21170, 0, 4, 0, 38, 100, 0, 0, 0, 4, 0, 0, 0, 53, 1, 2117003, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(21170, 0, 3, 0, 38, 100, 0, 0, 0, 3, 0, 0, 0, 53, 1, 2117002, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(21170, 0, 2, 0, 38, 100, 0, 0, 0, 2, 0, 0, 0, 53, 1, 2117001, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(21170, 0, 1, 0, 38, 100, 0, 0, 0, 1, 0, 0, 0, 53, 1, 2117000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On data set - Start WP movement'),
(21170, 0, 0, 0, 11, 100, 0, 0, 0, 0, 0, 0, 0, 11, 36350, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On Spawn - Add aura'),

(2117303, 9, 8, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 22406, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Foothill'),
(2117303, 9, 7, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39108, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Foothill'),
(2117303, 9, 6, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 19, 22406, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Foothill'),
(2117303, 9, 5, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39108, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Foothill'),
(2117303, 9, 4, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 19, 22406, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Foothill'),
(2117303, 9, 3, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39108, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Foothill'),
(2117303, 9, 2, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 22406, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Foothill'),
(2117303, 9, 1, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39108, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Foothill'),
(2117303, 9, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Reset data 0'),

(2117302, 9, 8, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 19, 22405, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Forge'),
(2117302, 9, 7, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39107, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Forge'),
(2117302, 9, 6, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 19, 22405, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Forge'),
(2117302, 9, 5, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39107, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Forge'),
(2117302, 9, 4, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 19, 22405, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Forge'),
(2117302, 9, 3, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39107, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Forge'),
(2117302, 9, 2, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 22405, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Forge'),
(2117302, 9, 1, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39107, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, Forge'),
(2117302, 9, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Reset data 0'),

(2117301, 9, 8, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 19, 22404, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, North'),
(2117301, 9, 7, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39106, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, North'),
(2117301, 9, 6, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 19, 22404, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, North'),
(2117301, 9, 5, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39106, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, North'),
(2117301, 9, 4, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 19, 22404, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, North'),
(2117301, 9, 3, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39106, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, North'),
(2117301, 9, 2, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 22404, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, North'),
(2117301, 9, 1, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 39106, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, North'),
(2117301, 9, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Reset data 0'),

(2117300, 9, 8, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 4, 0, 0, 0, 0, 19, 21170, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, South'),
(2117300, 9, 7, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 36302, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, South'),
(2117300, 9, 6, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 19, 21170, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, South'),
(2117300, 9, 5, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 36302, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, South'),
(2117300, 9, 4, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 19, 21170, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, South'),
(2117300, 9, 3, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 36302, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, South'),
(2117300, 9, 2, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 21170, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, South'),
(2117300, 9, 1, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 11, 36302, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ' Summon Honor Hold Gryphon Brigadier, South'),
(2117300, 9, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Reset data 0'),

(22403, 0, 2, 0, 61, 100, 0, 0, 0, 0, 0, 0, 0, 11, 34386, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower Foothill - On spell hit - Spawn fire'),
(22403, 0, 1, 2, 61, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 4, 0, 0, 0, 0, 10, @trigger2, 21173, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower Foothill - On link - Call Griphonriders'),
(22403, 0, 0, 1, 8, 100, 0, 0, 36374, 0, 0, 0, 0, 33, 22403, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower Foothill - on spell hit - give credit'),

(22402, 0, 2, 0, 61, 100, 0, 0, 0, 0, 0, 0, 0, 11, 34386, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower Forge - On spell hit - Spawn fire'),
(22402, 0, 1, 2, 61, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 10, @trigger2, 21173, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower Forge - On link - Call Griphonriders'),
(22402, 0, 0, 1, 8, 100, 0, 0, 36374, 0, 0, 0, 0, 33, 22402, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower Forge - on spell hit - give credit'),

(22401, 0, 2, 0, 61, 100, 0, 0, 0, 0, 0, 0, 0, 11, 34386, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower North - On spell hit - Spawn fire'),
(22401, 0, 1, 2, 61, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, @trigger2, 21173, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower North - On link - Call Griphonriders'),
(22401, 0, 0, 1, 8, 100, 0, 0, 36374, 0, 0, 0, 0, 33, 22401, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower North - on spell hit - give credit'),

(21182, 0, 2, 0, 61, 100, 0, 0, 0, 0, 0, 0, 0, 11, 34386, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On spell hit - Spawn fire'),
(21182, 0, 1, 2, 61, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, @trigger1, 21173, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On link - Call Griphonriders'),
(21182, 0, 0, 1, 8, 100, 0, 0, 36374, 0, 0, 0, 0, 33, 21182, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - on spell hit - give credit'),

(21173, 0, 3, 0, 38, 100, 0, 0, 0, 4, 0, 0, 0, 80, 2117303, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower Foothill - On dataset - load script'),
(21173, 0, 2, 0, 38, 100, 0, 0, 0, 3, 0, 0, 0, 80, 2117302, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower Forge - On dataset - load script'),
(21173, 0, 1, 0, 38, 100, 0, 0, 0, 2, 0, 0, 0, 80, 2117301, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower North - On dataset - load script'),
(21173, 0, 0, 0, 38, 100, 0, 0, 0, 1, 0, 0, 0, 80, 2117300, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Zeth\'Gor Quest Credit Marker, They Must Burn, Tower South - On dataset - load script');


delete from creature where id in (22401,21182,22402,22403);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(21182, 530, 3483, 3582, 1, 1, 0, 0, 0, -1153.7393, 2108.4656, 77.8835, 1.18682, 300, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(22401, 530, 3483, 3582, 1, 1, 0, 0, 0, -823.0888, 2032.1426, 49.2935, 1.18682, 300, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(22402, 530, 3483, 3582, 1, 1, 0, 0, 0, -897.4683, 1919.8066, 76.8173, 1.18682, 300, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(22403, 530, 3483, 3582, 1, 1, 0, 0, 0, -978.2192, 1877.3834, 102.0239, 1.18682, 300, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);