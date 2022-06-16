-- https://forum.wowcircle.net/showthread.php?t=1039747&p=7542037#post7542037
update creature set spawntimesecs = 900 where id = 83746;


-- https://forum.wowcircle.net/showthread.php?t=1039694
delete from conditions where sourceentry = 163949 and sourcetypeorreferenceid = 17;
delete from conditions where sourceentry = 163949 and sourcetypeorreferenceid = 13;

update creature_template_wdb set displayid1 = 56338, displayid2 = 56920, displayid3 = 0 where entry = 81288;

delete from conditions where SourceEntry in (163955,163959,163960) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 163955, 0, 0, 31, 0, 3, 81288, 0, 0, 0, '', 'Cast only 81288'),
(13, 1, 163959, 0, 0, 31, 0, 3, 81288, 0, 0, 0, '', 'Cast only 81288'),
(13, 1, 163960, 0, 0, 31, 0, 3, 81288, 0, 0, 0, '', 'Cast only 81288');


-- https://forum.wowcircle.net/showthread.php?t=890695


-- https://forum.wowcircle.net/showthread.php?t=1032661
update gameobject_template set ainame = 'SmartGameObjectAI', flags = 0 where entry in (246916,246886,246914,246913);

delete from smart_scripts where entryorguid in (246916,246886,246914,246913);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(246886, 1, 0, 0, 70, 0, 100, 0, 2, 0, 0, 0, 0, 33, 103040, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Seal - on gossip hello - give credit'),
(246916, 1, 0, 0, 70, 0, 100, 0, 2, 0, 0, 0, 0, 33, 103043, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Seal - on gossip hello - give credit'),
(246914, 1, 0, 0, 70, 0, 100, 0, 2, 0, 0, 0, 0, 33, 103042, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Seal - on gossip hello - give credit'),
(246913, 1, 0, 0, 70, 0, 100, 0, 2, 0, 0, 0, 0, 33, 103041, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Seal - on gossip hello - give credit');


-- https://forum.wowcircle.net/showthread.php?t=1033936
delete from playerchoice_response_reward_spellcast where choiceid = 51 and responseid in (119,120);
INSERT INTO `playerchoice_response_reward_spellcast` (`ChoiceId`, `ResponseId`, `Index`, `SpellId`, `VerifiedBuild`) VALUES 
(51, 119, 0, 160027, 26365),
(51, 120, 0, 159987, 26365);

DELETE FROM `spell_scene` WHERE MiscValue in (677);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1360, 677, 5);


-- https://forum.wowcircle.net/showthread.php?t=1033951
update creature_template set ainame = 'SmartAI', flags_extra = 128, inhabittype = 4 where entry in (80237,80245,80246, 80338);

delete from smart_scripts where entryorguid in (80237,80245,80246, 80338);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(80338, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Vehicle - on just summoned - invoker cast'),
(80338, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 53, 0, 80338, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Vehicle - on link - start waypoint'),
(80338, 0, 2, 3, 40, 0, 100, 1, 4, 80338, 0, 0, 0, 1014, 230949, 1, 2, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Vehicle - on waypoint reached - give credit'),
(80338, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Vehicle - on link - remove aura'),
(80338, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Vehicle - on link - despawn'),

(80237, 0, 0, 1, 8, 0, 100, 1, 162332, 0, 0, 0, 0, 33, 80237, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(80245, 0, 0, 1, 8, 0, 100, 1, 162332, 0, 0, 0, 0, 33, 80245, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(80246, 0, 0, 1, 8, 0, 100, 1, 162332, 0, 0, 0, 0, 33, 80246, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),

(80237, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - despawn'),
(80245, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - despawn'),
(80246, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - despawn');

update creature set spawntimesecs = 10 where id in (80237,80245,80246);

DELETE FROM `waypoints` WHERE `entry` = 80338;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES 
(80338, 1, 1684.25, 1715.62, 301.435, ''),
(80338, 2, 1684.78, 1715.97, 309.072, ''),
(80338, 3, 1685.01, 1715.32, 312.124, ''),
(80338, 4, 1685.68, 1713.56, 311.589, '');


-- https://forum.wowcircle.net/showthread.php?t=1034166
update creature_template set gossip_menu_id = 12054, ainame = 'SmartAI' where entry = 45752;

delete from smart_scripts where entryorguid in (45752);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(45752, 0, 0, 0, 62, 0, 100, 0, 12054, 0, 0, 0, 0, 85, 85360, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Portal - on gossip select - invoker cast');

delete from spell_target_position where id in (85360);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(85360, 0, -132.9060, 780.9426, 67.3825, 0.1);

delete from creature_template_addon where entry in (1888,1889,1913,1914,1915,3577,3578,45756);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(1888, 0, 0, 0, '85368'),
(1889, 0, 0, 0, '85368'),
(1913, 0, 0, 0, '85368'),
(1914, 0, 0, 0, '85368'),
(1915, 0, 0, 0, '85368'),
(3577, 0, 0, 0, '85368'),
(3578, 0, 0, 0, '85368'),
(45756, 0, 0, 0, '85368');

DELETE FROM `spell_area` WHERE `spell` in (85361,85368) AND `area` = 233;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(85361, 233, 27513, 27518, 0, 0, 2, 1, 74, 66),
(85368, 233, 27513, 27518, 0, 0, 2, 1, 74, 66);

delete from gossip_menu_option where menu_id in (12054);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(12054, 0, 0, '<Use the Ambermill Dimensional Portal.>', 1, 1, 45818);

delete from conditions where SourceGroup in (12054) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 12054, 0, 0, 0, 14, 0, 27513, 0, 0, 1, 0, '', 'Show Gossip if 27513 quest not none'),
(15, 12054, 0, 0, 0, 8, 0, 27518, 0, 0, 1, 0, '', 'Show Gossip if 27518 quest not rewarded');

delete from creature where id in (1888,1889,1913,1914,1915,3577,3578,45756);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `dynamicflags`, `isActive`) VALUES 
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -285.148, 851.974, 82.0907, 3.35103, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -270.948, 822.823, 77.9338, 3.57792, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -238.89, 799.74, 74.0273, 4.57533, 120, 6, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -220.79, 809.268, 75.4204, 2.15441, 120, 10, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -218.155, 1059.97, 60.3767, 1.78024, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -211.65, 851.255, 70.6363, 0.404251, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -207.892, 852.863, 70.0688, 0.404251, 120, 29, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -201.214, 1040.95, 61.9001, 3.58195, 120, 27, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -198.85, 887.17, 65.5853, 1.72727, 120, 6, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -198.337, 888.543, 65.6087, 5.46288, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -195.754, 971.795, 66.1401, 5.64866, 120, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -193.965, 1072.4, 59.6536, 1.51844, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -193.77, 1006.74, 65.5697, 0.188394, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -192.126, 862.055, 67.4897, 3.3321, 120, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -191.917, 912.896, 68.5854, 5.67087, 120, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -190.897, 809.269, 61.5557, 1.7151, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -190.47, 806.335, 61.4111, 1.92623, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -190.17, 862.432, 67.1164, 3.3321, 120, 12, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -186.491, 950.679, 65.6585, 4.72398, 120, 85, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -185.118, 917.802, 71.4343, 5.22835, 120, 5, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -182.312, 863.884, 65.8781, 3.27494, 120, 44, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -180.061, 925.8, 65.6263, 0.191986, 120, 59, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -174.997, 839.471, 60.7171, 0.981472, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -173.151, 838.723, 60.5193, 5.89813, 120, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -169.502, 1028.43, 65.3197, 5.43041, 120, 6, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -169.016, 784.352, 69.6126, 2.89725, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -168.542, 988.495, 65.0678, 0.0122612, 120, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -163.029, 952.173, 64.9598, 1.42287, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -161.802, 784.174, 64.8455, 5.98648, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -161.648, 790.971, 64.7645, 4.48706, 120, 6, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -161.331, 868.319, 63.3428, 5.69547, 120, 19, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -158.939, 889.795, 63.1306, 2.18874, 120, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -158.477, 889.714, 63.0005, 5.3076, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -156.997, 892.038, 63.2272, 0.857269, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -155.292, 922.174, 65.5152, 1.07129, 120, 10, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -153.444, 925.095, 65.7892, 0.200001, 120, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(3577, 0, 233, 130, 1, 1, 0, 0, 0, -152.904, 823.562, 64.1643, 5.91667, 120, 12, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -142.651, 866.67, 63.2742, 3.17378, 120, 83, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -139.214, 845.663, 61.7582, 1.69297, 120, 7, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -135.259, 1007.49, 68.0383, 4.70675, 120, 124, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -133.59, 856.971, 62.1782, 4.19972, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -133.23, 862.588, 62.6482, 3.42687, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -130.896, 858.57, 62.2567, 2.48637, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -127.269, 823.17, 63.665, 1.78024, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -122.977, 848.208, 61.7631, 1.78024, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -110.602, 817.226, 63.7868, 2.00713, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -108.266, 854.422, 61.4114, 3.19243, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -108.147, 820.506, 63.7263, 2.42745, 120, 34, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -106.848, 872.755, 62.4668, 4.54324, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -105.849, 874.503, 62.6685, 1.26607, 120, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -100.972, 781.799, 66.1066, 5.02655, 120, 9, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -100.745, 792.717, 66.1066, 0, 120, 9, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1889, 0, 233, 130, 1, 1, 0, 0, 0, -98.1084, 940.932, 68.2787, 4.90973, 120, 5, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -97.6216, 908.941, 65.5243, 1.68669, 120, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -97.332, 816.476, 63.8035, 3.00705, 120, 33, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -93.9931, 1069.11, 64.9521, 1.0821, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1889, 0, 233, 130, 1, 1, 0, 0, 0, -92.2086, 980.557, 68.3344, 5.20068, 120, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1889, 0, 233, 130, 1, 1, 0, 0, 0, -89.7953, 1021.91, 65.3576, 5.80213, 120, 7, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(3578, 0, 233, 130, 1, 1, 0, 0, 0, -88.4132, 822.215, 63.8491, 4.13643, 120, 10, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -80.7517, 878.116, 67.29, 2.68781, 120, 54, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -78.8102, 856.574, 64.0348, 4.6605, 120, 5, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -78.474, 886.813, 67.2909, 2.89725, 120, 10, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1889, 0, 233, 130, 1, 1, 0, 0, 0, -75.6944, 935.689, 69.4726, 2.98451, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -73.151, 804.127, 64.9749, 5.46288, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -71.1372, 1060.25, 64.7332, 1.02974, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1889, 0, 233, 130, 1, 1, 0, 0, 0, -65.724, 970.651, 67.3847, 2.82743, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1889, 0, 233, 130, 1, 1, 0, 0, 0, -64.6319, 1004.35, 66.4249, 3.57792, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -61.1929, 1033.52, 63.7353, 2.60452, 120, 34, 0, 0, 0, 2, 0, 0, 0, 0, 0),
(1914, 0, 233, 130, 1, 1, 0, 0, 0, -58.3646, 856.927, 65.5288, 1.39626, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -55.2735, 876.468, 65.7839, 2.03037, 120, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -48.8125, 787.385, 61.4825, 5.13127, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -46.9048, 918.023, 69.4405, 4.72656, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -46.7708, 1044.18, 64.4546, 0.698132, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1888, 0, 233, 130, 1, 1, 0, 0, 0, -42.7439, 827.13, 65.4627, 2.34294, 120, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0),
(1913, 0, 233, 130, 1, 1, 0, 0, 0, -20.6042, 815.479, 65.4455, 5.48033, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);


-- https://forum.wowcircle.net/showthread.php?t=1036347
update creature_template set ainame = 'SmartAI' where entry = 100323;

delete from creature where id = 100323;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(100323, 0, 10, 1098, 1, 1, 0, 0, 1, -10369.3, -1256.81, 35.9093, 0.00481149, 300, 0, 0, 3117801, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from smart_scripts where entryorguid in (100323);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(100323, 0, 0, 1, 10, 0, 100, 0, 1, 10, 0, 0, 0, 33, 102291, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Revil Cost - on ooc los - give credit'),
(100323, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Revil Cost - on link - talk'),
(100323, 0, 2, 0, 52, 0, 100, 0, 0, 100323, 0, 0, 0, 1, 1, 10000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Revil Cost - on text over - talk'),
(100323, 0, 3, 0, 52, 0, 100, 0, 1, 100323, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Revil Cost - on text over - talk');

delete from conditions where SourceEntry in (100323) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 100323, 0, 0, 9, 0, 40785, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 100323, 0, 0, 48, 0, 281202, 0, 0, 1, 0, '', 'Smart Only If player not complete criteria');

delete from creature_text where entry in (100323);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(100323, 0, 0, 'Сначала стая Ночной Погибели сходит с ума, а теперь сюда пожаловали друиды?', 12, 0, 100, 0, 0, 0, 104555),
(100323, 1, 0, 'Полагаю, Коса Элуны вернулась в Сумеречный лес, и Темные всадники рыщут где-то неподалеку.', 12, 0, 100, 0, 0, 0, 104556),
(100323, 2, 0, 'Скажи, что ты ищешь, друид. Возможно, я смогу помочь.', 12, 0, 100, 0, 0, 0, 104557);


-- https://forum.wowcircle.net/showthread.php?t=1039909
update quest_template set prevquestid = 35298 where id = 36062;


-- https://forum.wowcircle.net/showthread.php?t=1039905
delete from creature where id in (81784,81972);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(81784, 1116, 6722, 7164, 1, 1, 0, 0, 0, -1817, 1099.8, 45.423, 5.39656, 300, 0, 0, 0, 0, 0),
(81972, 1116, 6722, 7163, 1, 1, 0, 0, 0, -2346.37, 1136.96, 20.9261, 0.96636, 300, 0, 0, 0, 0, 0);

update quest_template set prevquestid = 35285 where id in (35089,35090);
update quest_template set prevquestid = 35090 where id = 35091;


-- https://forum.wowcircle.net/showthread.php?t=1036351
update creature_template set lootid = entry where entry in (82146,82147,82027,82029,82143,82028,82040,82038,82240,82235);

delete from creature_loot_template where item in (113106);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(82146, 113106, -100, 0, 0, 1, 1),
(82147, 113106, -100, 0, 0, 1, 1),
(82027, 113106, -100, 0, 0, 1, 1),
(82029, 113106, -100, 0, 0, 1, 1),
(82143, 113106, -100, 0, 0, 1, 1),
(82028, 113106, -100, 0, 0, 1, 1),
(82040, 113106, -100, 0, 0, 1, 1),
(82038, 113106, -100, 0, 0, 1, 1),
(82240, 113106, -100, 0, 0, 1, 1),
(82235, 113106, -100, 0, 0, 1, 1);

update creature_template set gossip_menu_id = 16652, ainame = 'SmartAI', npcflag = 1 where entry in (82110);
update creature_template set ainame = 'SmartAI' where entry in (82124);

delete from smart_scripts where entryorguid in (82110,82124);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(82110, 0, 0, 0, 62, 0, 100, 0, 16652, 0, 0, 0, 0, 85, 165238, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Elis Finn - on gossip select - invoker cast'),
(82124, 0, 0, 1, 19, 0, 100, 0, 35353, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Elis Finn - on quest accept - talk'),
(82124, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Elis Finn - on link - despawn');

delete from creature_text where entry in (82124);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(82124, 0, 0, 'К-кто ты? Где Брайан?', 12, 0, 100, 0, 0, 0, 83994);


DELETE FROM `spell_area` WHERE `spell` = 165240 AND `area` = 7208;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(165240, 7208, 35339, 35353, 0, 0, 2, 1, 66, 74);

delete from gossip_menu_option where menu_id in (16652);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(16652, 0, 0, 'Покажите сущности привидений Элис.', 1, 1, 83993);

delete from conditions where SourceGroup in (16652) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 16652, 0, 0, 0, 9, 0, 35339, 0, 0, 0, 0, '', 'Show Gossip if player has 35339	 quest'),
(15, 16652, 0, 0, 0, 2, 0, 113106, 6, 0, 0, 0, '', 'Show Gossip if player has 113106 item');

delete from gossip_menu where entry in (16652);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(16652, 24188);

delete from npc_text where id in (24188);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(24188, 83992, 83992);

delete from phase_definitions where zoneId = 6722 and entry in (1);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6722, 1, 0, 7208, 0, '35339 quest');

delete from conditions where SourceGroup in (6722) and SourceTypeOrReferenceId = 26 and sourceentry in (1);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6722, 1, 0, 0, 28, 0, 35339, 0, 0, 1, 0, '', 'Activate phase if quest not completed'),
(26, 6722, 1, 0, 0, 8, 0, 35339, 0, 0, 1, 0, '', 'Activate phase if quest not rewarded');

delete from creature where id = 82110;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(82110, 1116, 6722, 7208, 1, 0, 7208, 0, 1, -681.136, 2397.59, 41.6723, 0.785476, 300, 0, 0, 869816, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.net/showthread.php?t=1036370
update creature_template set ainame = 'SmartAI' where entry = 40639;


delete from smart_scripts where entryorguid in (40639);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(40639, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 33, 40639, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Megagex - on gossip hello - give credit');


-- https://forum.wowcircle.net/showthread.php?t=1040132
update quest_template set flags = 36766720, flags2 = 0, rewardspell1 = 0 where id = 32317;

update gameobject_template set flags = 0 where entry = 216327;

delete from quest_template_objective where questid = 32317;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(269829, 32317, 1, 0, 6, 92497, 1, 0, 0, 0, 'Obtain the Shadowmoon Fragment', '1304', 1),
(269612, 32317, 1, 0, 0, 92494, 1, 0, 0, 0, 'Obtain the Hellfire Fragment', '1301', 1),
(269613, 32317, 1, 0, 1, 92495, 1, 0, 0, 0, 'Obtain the Netherstorm Fragment', '1302', 1),
(269615, 32317, 1, 0, 4, 92496, 1, 0, 0, 0, 'Obtain the Blade\'s Edge Fragment', '1303', 1),
(269828, 32317, 0, 0, 3, 68783, 1, 0, 0, 0, 'Watch the Blade\'s Edge Memory', '', 1),
(269830, 32317, 0, 0, 5, 68780, 1, 0, 0, 0, 'Watch the Shadowmoon Memory', '', 1),
(269614, 32317, 0, 0, 2, 68782, 1, 0, 0, 0, 'Watch the Netherstorm Memory', '', 1);


DELETE FROM `spell_area` WHERE `spell` in (140793) AND `area` in (3520,3522,3523,3483);
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(140793, 3520, 32317, 32324, 0, 0, 2, 1, 66, 74),
(140793, 3523, 32317, 32324, 0, 0, 2, 1, 66, 74),
(140793, 3522, 32317, 32324, 0, 0, 2, 1, 66, 74),
(140793, 3483, 32317, 32324, 0, 0, 2, 1, 66, 74);


-- мапы в сум. нагорье
INSERT INTO phase_definitions (zoneId, entry, phasemask, phaseId, terrainswapmap, flags, comment) VALUES 
(4922, 1, 0, 0, 736, 0, 'Dragonmaw Port Default Swap'),
(4922, 2, 0, 0, 760, 0, 'Dragonmaw Port Swap');