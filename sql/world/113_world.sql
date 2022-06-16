-- https://forum.wowcircle.com/showthread.php?t=1043315
update creature set phaseMask = 0, phaseID = 30991 where id in (60455,60483);

update quest_template set InProgressPhaseID = 30991 where id = 30991;

update creature set phaseMask = 0, phaseID = 30992 where id in (62165);

update quest_template set InProgressPhaseID = 30992 where id = 30992;


-- https://forum.wowcircle.com/showthread.php?t=888656
delete from creature where id = 111786;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(111786, 1220, 7558, 7846, 1, 0, 35600, 0, 0, 3902.8, 7406.1, 28.9912, 4.86562, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from phase_definitions where zoneId = 7558 and entry in (35600);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7558, 35600, 0, 35600, 0, '356 class order trait');

delete from conditions where SourceGroup in (7558) and SourceTypeOrReferenceId = 26 and SourceEntry in (35600);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7558, 35600, 0, 0, 100, 0, 356, 1, 0, 0, 0, '', 'phase if class order trait learned');

update gameobject set phaseMask = 0, phaseID = 35600 where id in (250887,252965);


-- https://forum.wowcircle.com/showthread.php?t=1040493
delete from creature_onkill_reputation where creature_id = 75815;
-- INSERT INTO `creature_onkill_reputation` (`creature_id`, `RewOnKillRepFaction1`, `RewOnKillRepFaction2`, `MaxStanding1`, `IsTeamAward1`, `RewOnKillRepValue1`, `MaxStanding2`, `IsTeamAward2`, `RewOnKillRepValue2`, `TeamDependent`) VALUES 
-- (75815, 1445, 1731, 6, 0, 500, 6, 0, 500, 0);


-- https://forum.wowcircle.com/showthread.php?t=1027591	
delete from creature where id = 78777;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(78777, 870, 5840, 6295, 1, 1, 0, 0, 0, 1433.82, 375.613, 548.08, 2.29794, 300, 0, 0, 46975, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1058812
update creature_template set AIName = 'SmartAI' where entry in (76411);

delete from smart_scripts where entryorguid in (76411);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(76411, 0, 0, 0, 19, 0, 100, 0, 33815, 0, 0, 0, 0, 1004, 771, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Drektar - on quest accept - playe scene');

delete from phase_definitions where zoneId = 6720 and entry in (33815,34402);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6720, 33815, 0, 33815, 0, '33815 quest'),
(6720, 34402, 0, 34402, 0, '34402 quest');

delete from conditions where SourceGroup in (6720) and SourceTypeOrReferenceId = 26 and SourceEntry in (33815,34402);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6720, 34402, 0, 0, 14, 0, 33815, 0, 0, 1, 0, '', 'phase if quest not none'),
(26, 6720, 34402, 0, 0, 14, 0, 34402, 0, 0, 0, 0, '', 'phase if quest none'),
(26, 6720, 33815, 0, 0, 14, 0, 33815, 0, 0, 0, 0, '', 'phase if quest none');

delete from creature where id in (70859,76411,78288,78272);
INSERT INTO `creature` (`guid`, `linked_id`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(11574649, '880A0A012E549EF5368F6DFDF1F73DD054B83D4B', 78272, 1116, 6720, 7765, 1, 0, 34402, 0, 0, 5435.08, 4949.21, 64.6807, 0.747362, 300, 0, 0, 8089200, 0, 0, 0, 0, 0, 2048, 0, 0, 0, 0, '', '', -1),
(11574651, '3D810A69CC087499DDA41D8DC73059941A999925', 70859, 1116, 6720, 7765, 1, 1, 0, 0, 0, 5608.66, 4525.13, 120.713, 0.139781, 300, 0, 0, 32356800, 0, 0, 0, 0, 0, 2048, 0, 0, 0, 0, '', '', -1), 
(512084, 'A3B01940DD666A11FDC9AC5B1BA71BEEA48694B5', 70859, 1116, 6720, 7765, 1, 0, 33815, 53057, 0, 5537.37, 5011.88, 12.8556, 5.22385, 300, 0, 0, 117424800, 3020500, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(512162, 'C76C926B356D2BC2FAD66D8EFAE48A0A8F977D4D', 70859, 1116, 6720, 7004, 1, 1, 0, 53057, 0, 5611.71, 4525.86, 120.264, 0.194927, 300, 0, 0, 117424800, 3020500, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(513122, '8DFCB15DBC541EF531F8C3A81294CE82F0C17E5B', 70859, 1116, 6720, 6864, 1, 1, 0, 53057, 0, 6810.67, 5852.39, 259.358, 4.07147, 300, 0, 0, 117424800, 3020500, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(18438538, '35E5F1F5F3138ED2F2B23BE0E5D0AC210EC7D2BA', 76411, 1152, 7004, 7096, 2, 65535, 0, 0, 0, 5582.05, 4535.62, 133.83, 5.30001, 300, 0, 0, 1087270, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(18438537, 'EF29F9026661ABFA7332980DBC8CFF0110E028A1', 76411, 1116, 6720, 7765, 1, 0, 33815, 0, 0, 5540.09, 5012.13, 12.8852, 5.04821, 10, 0, 0, 269640, 0, 0, 0, 0, 0, 2048, 0, 0, 0, 0, '', '', -1), 
(11574652, 'F6910883C179C9E11E9AD6DA9F0B29C173267048', 78288, 1116, 6720, 7765, 1, 1, 0, 0, 0, 5608.56, 4522.49, 120.479, 0.023548, 300, 0, 0, 978540, 0, 0, 0, 0, 0, 2048, 0, 0, 0, 0, '', '', -1), 
(512083, '9A7F73D1FC44FEBA2280A34EC5891F35097DE460', 78288, 1116, 6720, 7765, 1, 0, 33815, 56765, 0, 5536.29, 5009.91, 12.9158, 5.38051, 300, 0, 0, 782830, 75846, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1), 
(512163, 'DFA732A376AEE540DB0D5B61BE106B0D92BE488C', 78288, 1116, 6720, 7004, 1, 1, 0, 56765, 0, 5612.45, 4523.49, 119.756, 0.29847, 300, 0, 0, 782830, 75846, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1058978
update creature_template set npcflag = 16777216 where entry = 94321;

delete from npc_spellclick_spells where npc_entry in (94324,94321);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(94324, 46598, 1, 0),
(94321, 186671, 1, 0);

delete from conditions where SourceGroup in (94324,94321) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 94321, 186671, 0, 0, 9, 0, 39247, 0, 0, 0, 0, '', 'Spellclick if quest incomplete'),
(18, 94321, 186671, 0, 1, 9, 0, 41119, 0, 0, 0, 0, '', 'Spellclick if quest incomplete');

delete from smart_scripts where entryorguid in (94324,94321);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(94324, '', 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 0, 0, 85, 59119, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'vehicle - on just summoned - invoker cast'),
(94324, '', 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 53, 1, 943240, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Run waypoint on spawn'),
(94324, '', 0, 2, 0, 40, 0, 100, 0, 0, 3, 0, 0, 0, 0, 62, 1498, 0, 0, 0, 0, 0, 18, 5, 0, 0, 771.776, 4663.98, 181.43, 0.7028, 'Teleport player'),
(94324, '', 0, 3, 0, 40, 0, 100, 0, 0, 3, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Despawn self');


-- https://forum.wowcircle.com/showthread.php?t=1059056
update creature_template set gossip_menu_id = entry, AIName = 'SmartAI', npcflag = 3 where entry = 108576;

delete from gossip_menu_option where menu_id in (108576);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(108576, 0, 0, 'Я готов. Выманивай демона.', 1, 1, 114355);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 108576;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 108576, 0, 0, 0, 9, 0, 42749, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),
(15, 108576, 0, 0, 0, 29, 0, 108663, 50, 0, 1, 0, '', 'Gossip if near no npc');

delete from gossip_menu where entry in (108576);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(108576, 108576);

delete from npc_text where id in (108576);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(108576, 114337, 114337);

update creature_template set AIName = 'SmartAI' where entry = 108663;

delete from smart_scripts where entryorguid in (108576,108663);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(108576, '', 0, 0, 1, 62, 0, 100, 0, 0, 108576, 0, 0, 0, 0, 12, 108663, 1, 600000, 0, 0, 0, 8, 0, 0, 0, 2698.8406, 1157.4867, 198.1302, 0.78, 'malace - on gossip select - summon creature'),
(108576, '', 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'malace - on link - talk'),
(108576, '', 0, 2, 0, 10, 0, 100, 0, 0, 1, 20, 0, 0, 0, 33, 108648, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'malace - on ooc los - give credit'),
(108663, '', 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'lohaber - on just summoned - attack'),
(108663, '', 0, 1, 0, 60, 0, 100, 1, 0, 5000, 5000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'lohaber - ic - talk');

delete from creature_text where entry in (108576,108663);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`, `comment`) VALUES 
(108576, 0, 0, 'Лохабер! Эй, демон, выходи. Если, конечно, не струсишь.', 14, 0, 100, 0, 0, 114356, ''),
(108663, 0, 0, 'МНЕ НЕВЕДОМ СТРАХ! Я убью вас собственными руками. А тебе, $p, особый привет от Карии и Варедиса.', 14, 0, 100, 0, 0, 114357, '');

update quest_template set PrevQuestId = 42736 where id = 42749;