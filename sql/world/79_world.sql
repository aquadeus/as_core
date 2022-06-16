-- https://forum.wowcircle.com/showthread.php?t=1054468
update creature set spawnmask = 1 where map = 1519;

DELETE FROM `spell_area` WHERE `spell` in (68243) and area = 8022;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(68243, 8022, 41033, 41060, 0, 0, 2, 1, 74, 66);

update gameobject set state = 1, phasemask = 2, phaseid = 0 where id = 254245;

delete from spell_linked_spell where spell_trigger in (-203269) and spell_effect in (203459);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(-203269, 203459, 0, 0, 0, 0, 0, '');


-- https://forum.wowcircle.com/showthread.php?t=1054557
update creature_template set ainame = 'SmartAI' where entry in (90259);

delete from smart_scripts where entryorguid in (90259);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(90259, 0, 0, 0, 19, 0, 100, 0, 43424, 0, 0, 0, 0, 75, 219567, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Tyrosus - on quest accept - invoker cast');

DELETE FROM `spell_scene` WHERE MiscValue in (1362);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1669, 1362, 20);


-- https://forum.wowcircle.com/showthread.php?t=1054574
DELETE FROM `creature_queststarter` WHERE quest in (35907);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(83529, 35907);

update creature_template set npcflag = 2 where entry = 83529;

delete from phase_definitions where zoneId = 6722 and entry in (35283,35284);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6722, 35283, 0, 35283, 0, '35283 quest'),
(6722, 35284, 0, 35284, 0, '35284 quest');

delete from conditions where SourceGroup in (6722) and SourceTypeOrReferenceId = 26 and sourceentry in (35283,35284);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6722, 35283, 0, 0, 14, 0, 35283, 0, 0, 1, 0, '', 'Phase If quest 35283 not none'),
(26, 6722, 35284, 0, 0, 14, 0, 35284, 0, 0, 1, 0, '', 'Phase If quest 35284 not none');

update gameobject set phasemask = 0, phaseid = 35283 where id = 232384;
update gameobject set phasemask = 0, phaseid = 35284 where id = 232385;

delete from creature where id IN ('89226', '89225', '88554', '84292', '88581', '88513', '88515', '82602', '88551', '88549', '88533', '88541');
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(82602, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -337.613, 2266.47, 28.3484, 4.37159, 120, 0, 0, 1, 0, 0, 1, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(84292, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -338.59, 2249.96, 28.3503, 2.03643, 120, 0, 0, 1, 0, 0, 2, 0, 33024, 2048, 0, 0, 0, 0, '', '', -1),
(88513, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -331.837, 2258.33, 28.2749, 2.0342, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88515, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -333.516, 2261.78, 28.311, 5.13666, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88533, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -345.14, 2259.67, 15.4575, 1.58512, 120, 0, 0, 1, 0, 0, 128, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88541, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -340.244, 2260.07, 15.4579, 3.94965, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88549, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -334.527, 2262.39, 39.8504, 5.10807, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88549, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -334.287, 2256.04, 39.7181, 4.37463, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88551, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -347.426, 2262.51, 28.3451, 4.23853, 120, 0, 0, 1, 0, 0, 0, 0, 536904448, 2048, 0, 0, 0, 0, '', '', -1),
(88551, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -355.451, 2243.88, 15.5131, 3.47642, 120, 0, 0, 1, 0, 0, 0, 0, 536904448, 2048, 0, 0, 0, 0, '', '', -1),
(88554, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -339.753, 2252.75, 28.49, 5.1372, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88554, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -342.014, 2251.52, 28.4531, 5.0101, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88554, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -335.778, 2256.87, 37.3563, 2.74094, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88581, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -333.763, 2253.01, 28.3153, 2.47476, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(89225, 1116, 6722, 7197, 1, 0, 35283, 61571, 0, -351.833, 2251.62, 27.8236, 5.42159, 300, 0, 0, 111657, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(89226, 1116, 6722, 7197, 1, 0, 35283, 61574, 0, -357.611, 2253.98, 27.7592, 2.45448, 300, 0, 0, 111657, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from spell_area where spell in (170098);
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(170098, 7197, 35284, 0, 0, 0, 2, 1, 64, 0);

update creature_template set ainame = 'SmartAI' where entry = 84243;

delete from smart_scripts where entryorguid in (84243);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(84243, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Linny - on just summoned - follow'),
(84243, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Linny - on link - talk');

delete from creature_text where Entry = 84243;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(84243, 0, 0, 'Такого качества больше ни у кого нет!', 12, 0, 100, 0, 0, 0, 85090),
(84243, 0, 1, 'Торговаться нет времени! Бери или проваливай.', 12, 0, 100, 0, 0, 0, 85092),
(84243, 0, 2, 'Я лично провезла этот товар контрабандой!', 12, 0, 100, 0, 0, 0, 85091);


-- https://forum.wowcircle.com/showthread.php?t=1054583
update creature_template set ainame = 'SmartAI', npcflag = 1, gossip_menu_id = 17290 where entry in (88388);
update creature_template set ainame = 'SmartAI', npcflag = 1, gossip_menu_id = 17288 where entry in (88390);
update creature_template set ainame = 'SmartAI', npcflag = 1, gossip_menu_id = 17289 where entry in (88389);

delete from smart_scripts where entryorguid in (88388,88389,88390);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(88388, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 33, 88388, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Book - on gossip hello - give credit'),
(88390, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 33, 88390, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Book - on gossip hello - give credit'),
(88389, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 33, 88389, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Book - on gossip hello - give credit');

delete from creature where id IN (88388,88389,88390);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(88390, 1116, 6719, 6909, 1, 1, 0, 0, 1, -897.9453, -1133.5508, 83.2939, 3.04, 120, 0, 0, 1, 0, 0, 1, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88388, 1116, 6719, 6909, 1, 1, 0, 0, 1, -718.1875, -1175.5089, 76.7298, 0.14, 120, 0, 0, 1, 0, 0, 1, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88389, 1116, 6719, 6909, 1, 1, 0, 0, 1, -577.7100, -1088.9900, 114.6420, 0, 120, 0, 0, 1, 0, 0, 1, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1);

delete from gossip_menu where entry in (17288,17289,17290);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(17290, 25610),
(17288, 25609),
(17289, 25611);

delete from npc_text where id in (25610,25611,25609);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`) VALUES 
(25610, 90567),
(25609, 90568),
(25611, 90569);


-- https://forum.wowcircle.com/showthread.php?t=1054585
DELETE FROM `creature_queststarter` WHERE quest in (33724,34950,33728,33729,33730,34962,33731,33736);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(75924, 33724),
(75958, 34950),
(75808, 33736),
(75941, 33728),
(75941, 33729),
(75941, 33730),
(75968, 33730),
(75959, 34962),
(75805, 33731);


-- https://forum.wowcircle.com/showthread.php?t=1051125
delete from world_quest_template where entry in (46869,43638,42783,42757,42240,42743,48957);
INSERT INTO `world_quest_template` (`entry`, `overrideAreaGroupID`, `areaID`, `enabled`, `overrideVariableID`, `overrideVariableValue`) VALUES 
(46869, -1, 0, 1, 0, 0),
(43638, -1, 7855, 1, 0, 0),
(42783, -1, 8183, 1, 0, 0),
(42757, -1, 0, 1, 0, 0),
(42240, -1, 0, 1, 0, 0),
(42743, -1, 0, 1, 0, 0),
(48957, -1, 0, 1, 0, 0);


delete from world_quest_loot where quest_id in (46869,43638,42783,42757,42240,42743,48957);
INSERT INTO `world_quest_loot` (`quest_id`, `group_id`, `chance`) VALUES 
(46869, 1, 30),
(46869, 2, 30),
(46869, 3, 30),
(46869, 4, 30),
(46869, 5, 10),

(43638, 1, 30),
(43638, 2, 30),
(43638, 3, 30),
(43638, 4, 30),
(43638, 5, 10),

(42783, 1, 30),
(42783, 2, 30),
(42783, 3, 30),
(42783, 4, 30),
(42783, 5, 10),

(42757, 1, 30),
(42757, 2, 30),
(42757, 3, 30),
(42757, 4, 30),
(42757, 5, 10),

(42240, 1, 30),
(42240, 2, 30),
(42240, 3, 30),
(42240, 4, 30),
(42240, 5, 10),

(42743, 1, 30),
(42743, 2, 30),
(42743, 3, 30),
(42743, 4, 30),
(42743, 5, 10),

(48957, 1, 30),
(48957, 2, 30),
(48957, 3, 30),
(48957, 4, 30),
(48957, 5, 10);


-- https://forum.wowcircle.com/showthread.php?t=1054590
update quest_template set nextquestidchain = 0 where id in (33230,33232);