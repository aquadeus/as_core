-- https://forum.wowcircle.com/showthread.php?t=1046902
update quest_template set flags = 0 where id = 26322;


-- https://forum.wowcircle.com/showthread.php?t=1047017
DELETE FROM `creature_questender` WHERE quest in (26378);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(42308, 26378);


-- https://forum.wowcircle.com/showthread.php?t=1047153
DELETE FROM `creature_questender` WHERE quest in (26508);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(8962, 26508);


-- https://forum.wowcircle.com/showthread.php?t=1047157
update quest_template set flags = 2162688, specialflags = 0 where id = 26512;

delete from quest_template_objective where questid = 26512;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`) VALUES 
(266334, 26512, 10, 0, 0, 6034, 1, 0, 0);

delete from smart_scripts where entryorguid = 6034 and source_type = 2;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(6034, 2, 0, 0, 46, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1014, 6034, 1, 10, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'AreaTrigger - On Trigger - Complete Quest');

delete from areatrigger_scripts where entry = 6034;
insert into areatrigger_scripts (`entry`, `scriptname`) values
(6034, 'SmartTrigger');


-- https://forum.wowcircle.com/showthread.php?t=1045501
delete from creature where id in (108576);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(108576, 1220, 7541, 7852, 1, 1, 0, 0, 0, 2894.85, 1267.01, 183.838, 0.11, 300, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

DELETE FROM `creature_questender` WHERE id in (108576);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(108576, 42736),
(108576, 42749);


-- https://forum.wowcircle.com/showthread.php?t=1048599
DELETE FROM `spell_area` WHERE `spell` in (76633) and area = 7790;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(76633, 7790, 38916, 39575, 0, 0, 2, 1, 74, 74);


-- https://forum.wowcircle.com/showthread.php?t=1046530
delete from smart_scripts where entryorguid in (268762,2687620,2687621,26876200,26876201);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(268762, 1, 0, 0, 70, 0, 100, 0, 2, 0, 0, 0, 0, 87, 26876200, 26876201, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mud - on state changed - Call random Actionlist'),

(26876200, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 12, 120570, 2, 30000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - summon creature'),
(26876200, 9, 1, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 99, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - set loot state'),

(26876201, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 33, 122093, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - give credit'),
(26876201, 9, 1, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 99, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - set loot state');

delete from gameobject where id = 268762;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(268762, 1220, 7543, 8643, 1, 0, 400025, -916.484, 2317.55, 0.849864, 2.77824, 0, 0, 0.983542, 0.180677, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -961.266, 2327.95, 0.861015, 3.85097, 0, 0, -0.937755, 0.347299, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -971.297, 2291.64, 0.548275, 4.41876, 0, 0, -0.802941, 0.596058, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1149.67, 2337.98, 1.39114, 3.53535, 0, 0, -0.980682, 0.195607, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1076.02, 2370.18, 0.845366, 4.86938, 0, 0, -0.64948, 0.760379, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1004.52, 2345.2, 1.21152, 4.67911, 0, 0, -0.718773, 0.695245, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1012.1, 2316.85, 0.0152126, 2.24922, 0, 0, 0.9021, 0.431528, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1193.03, 2372.4, 2.79577, 5.41716, 0, 0, -0.419607, 0.907706, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1231.2, 2400.27, 1.31095, 1.06025, 0, 0, 0.50564, 0.862745, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1202.37, 2384.02, 2.63725, 3.5254, 0, 0, -0.981643, 0.190729, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1276.7, 2421.24, 0.895981, 1.67009, 0, 0, 0.741326, 0.671145, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1190.23, 2356.96, 1.43024, 1.16513, 0, 0, 0.550167, 0.835055, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1162.49, 2367.61, 1.36301, 4.28814, 0, 0, -0.84013, 0.542385, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1264.75, 2411.08, 1.04817, 4.68018, 0, 0, -0.718404, 0.695626, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1239.79, 2311.91, 1.30953, 1.06726, 0, 0, 0.50866, 0.860967, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1226.36, 2290.7, 1.2612, 2.83299, 0, 0, 0.988119, 0.15369, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1203.62, 2309.03, 1.23586, 2.88268, 0, 0, 0.991633, 0.129093, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1018.27, 2345, 0.622169, 3.85097, 0, 0, -0.937755, 0.347299, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1170.18, 2351.84, 1.46546, 2.83299, 0, 0, 0.988119, 0.15369, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -915.358, 2299.71, 0.757362, 4.07897, 0, 0, -0.892162, 0.451715, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1076.02, 2370.18, 0.845366, 4.86938, 0, 0, -0.64948, 0.760379, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -971.297, 2291.64, 0.548275, 4.41876, 0, 0, -0.802941, 0.596058, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1004.52, 2345.2, 1.21152, 4.67911, 0, 0, -0.718773, 0.695245, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1018.27, 2345, 0.622169, 3.85097, 0, 0, -0.937755, 0.347299, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1046.43, 2385.63, 2.02225, 1.36153, 0, 0, 0.629387, 0.777092, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1059.91, 2347.82, 0.377993, 2.13081, 0, 0, 0.874984, 0.484152, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1083.08, 2376.62, 1.33995, 3.74412, 0, 0, -0.954962, 0.296729, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1249.55, 2321.57, 1.04475, 2.99841, 0, 0, 0.997438, 0.0715303, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1248.91, 2381.15, 1.11896, 3.12348, 0, 0, 0.999959, 0.00905627, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1190.23, 2356.96, 1.43024, 1.16513, 0, 0, 0.550167, 0.835055, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1201.89, 2336.4, 1.27654, 4.86109, 0, 0, -0.652627, 0.757679, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1170.18, 2351.84, 1.46546, 2.83299, 0, 0, 0.988119, 0.15369, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1100.55, 2327.62, 2.09436, 0.719828, 0, 0, 0.352194, 0.935927, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1250.53, 2352.72, 1.2181, 2.13081, 0, 0, 0.874984, 0.484152, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1193.03, 2372.4, 2.79577, 5.41716, 0, 0, -0.419607, 0.907706, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1231.93, 2328.15, 1.35403, 2.79338, 0, 0, 0.984881, 0.17323, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1226.97, 2332.92, 1.47825, 2.61501, 0, 0, 0.965539, 0.260258, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1137.99, 2357.39, 1.97967, 1.17294, 0, 0, 0.553426, 0.832898, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1234.23, 2376.68, 0.992771, 4.57269, 0, 0, -0.754733, 0.656032, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1225.41, 2401.64, 1.10828, 5.63825, 0, 0, -0.316906, 0.948457, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1114.1, 2360.55, 0.631957, 0.46273, 0, 0, 0.229306, 0.973354, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1181.3, 2321.81, 1.5781, 1.06751, 0, 0, 0.508769, 0.860903, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1205.4, 2301.82, 1.24694, 1.00432, 0, 0, 0.481319, 0.876545, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1103.9, 2335.86, 0.897569, 1.3129, 0, 0, 0.610311, 0.792162, 180, 100, 1, 0, '', 0, 26124),
(268762, 1220, 7543, 8643, 1, 0, 400025, -1215.94, 2361.95, 1.35689, 1.3242, 0, 0, 0.614774, 0.788704, 180, 100, 1, 0, '', 0, 26124);


-- https://forum.wowcircle.com/showthread.php?t=1046741
DELETE FROM `creature_queststarter` WHERE id = 83858 and quest in (35679,35680,35186,35185);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(83858, 35679),
(83858, 35680),
(83858, 35186),
(83858, 35185);

delete from conditions where sourceentry in (35679,35680,35186,35185) and SourceTypeOrReferenceId = 19;
-- INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
-- (19, 0, 34309, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered'),
-- (19, 0, 35843, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered'),
-- (19, 0, 34034, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered'),
-- (19, 0, 36136, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered'),
-- (19, 0, 35876, 0, 0, 40, 0, 3, 0, 0, 0, 0, '', 'this quest needs garrison lvl 3 to be offered');


-- https://forum.wowcircle.com/showthread.php?t=1045499
delete from creature where id = 92794;
INSERT INTO `creature` (`guid`, `linked_id`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(11784804, 'A37328A9FD9453243043B7C89A13E790EFB99EE3', 92794, 1220, 7558, 7667, 1, 4294967295, 0, 0, 1, 2902.13, 6402.63, 233.022, 2.4245, 30, 10, 0, 924436, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(11784790, 'A770E6620A1897AFABD5203B6F3DFAF08B60D232', 92794, 1220, 7558, 7667, 1, 4294967295, 0, 0, 1, 3002.19, 6496.94, 222.562, 1.22736, 30, 10, 0, 924436, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(11784789, '46374D27C62AF2D0B06F1DF41939303ACDAB2E7C', 92794, 1220, 7558, 7667, 1, 4294967295, 0, 0, 1, 2983.06, 6453.71, 222.006, 1.93814, 30, 10, 0, 924436, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(11784788, 'E5860BB59003B2A3C9210B87AA6EA301879D7CEC', 92794, 1220, 7558, 7667, 1, 4294967295, 0, 0, 1, 2932.18, 6462.61, 227.75, 1.58079, 30, 10, 0, 924436, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(11783490, '78DEC4C83838670A2E38C5EFEA74E660BA68F95D', 92794, 1220, 7558, 7677, 1, 4294967295, 0, 0, 1, 2941.45, 6511.79, 218.139, 5.77874, 30, 10, 0, 924436, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(11783376, 'D3062D0C958362614DE7C8C384BE678B31638C7F', 92794, 1220, 7558, 7667, 1, 4294967295, 0, 0, 1, 2940.03, 6415.98, 231.238, 1.6554, 30, 0, 0, 924436, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(11785214, 'BD1610FAE882414374A55180A496004A84A3E684', 92794, 1220, 7558, 7677, 1, 4294967295, 0, 0, 1, 2962.77, 6536.73, 226.005, 2.12258, 120, 10, 0, 2078534, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from smart_scripts where entryorguid = 110896 and action_param1 = 195041;


-- https://forum.wowcircle.com/showthread.php?t=1045495
update creature_template set unit_flags = 768 where entry = 89287;

delete from quest_template_objective where questid = 42021;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`) VALUES 
(286571, 42021, 0 , 3, 1, 89350, 1, 0, 0),
(283414, 42021, 14, 2, 6, 53711, 1, 0, 0),
(286570, 42021, 0 , 1, 3, 113644, 6, 0, 0),
(283413, 42021, 0 , 0, 0, 113643, 6, 0, 0);

delete from conditions where SourceEntry in (89350) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 3, 89350, 0, 0, 29, 0, 89287, 50, 0, 1, 0, '', 'allow smart - if near creature');