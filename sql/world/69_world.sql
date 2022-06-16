-- https://forum.wowcircle.com/showthread.php?t=1050906
delete from quest_template_objective where questid in (26274,26130);

delete from quest_template_objective where questid in (42672,42613,42600,42517,42481,42379,41125,41064,41047,41017,40995,40821,40698,40651,40276,39761,39722,39192,43263,43260,46744)
and objectid in (48616,48523,48521,48406,48430,48228,46860,46840,46833,46798,46763,53142,46403,46473,47008,46595,45896,46331,49441,49433,58025);
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(284568, 42672, 14, 0, 0, 48616, 1, 0, 0, 0, '', '', 1),
(284469, 42613, 14, 0, 0, 48523, 1, 0, 0, 0, '', '', 1),
(284468, 42600, 14, 0, 0, 48521, 1, 0, 0, 0, '', '', 1),
(284384, 42517, 14, 0, 0, 48406, 1, 0, 0, 0, '', '', 1),
(284397, 42481, 14, 0, 0, 48430, 1, 0, 0, 0, '', '', 1),
(284136, 42379, 14, 0, 0, 48228, 1, 0, 0, 0, '', '', 1),
(281906, 41125, 14, 0, 0, 46860, 1, 0, 0, 0, '', '', 1),
(281819, 41064, 14, 0, 0, 46840, 1, 0, 0, 0, '', '', 1),
(281760, 41047, 14, 0, 0, 46833, 1, 0, 0, 0, '', '', 1),
(281671, 41017, 14, 0, 0, 46798, 1, 0, 0, 0, '', '', 1),
(281616, 40995, 14, 0, 0, 46763, 1, 0, 0, 0, '', '', 1),
(281257, 40821, 14, 0, 0, 53142, 1, 0, 0, 0, '', '', 1),
(281075, 40698, 14, 0, 0, 46403, 1, 0, 0, 0, '', '', 1),
(281130, 40651, 14, 0, 0, 46473, 1, 0, 0, 0, '', '', 1),
(280579, 40276, 14, 0, 0, 47008, 1, 0, 0, 0, '', '', 1),
(279910, 39761, 14, 0, 0, 46595, 1, 0, 0, 0, '', '', 1),
(279888, 39722, 14, 0, 0, 45896, 1, 0, 0, 0, '', '', 1),
(279817, 39192, 14, 0, 0, 46331, 1, 0, 0, 0, '', '', 1),
(285139, 43263, 14, 0, 0, 49441, 1, 0, 0, 0, '', '', 1),
(285137, 43260, 14, 0, 0, 49433, 1, 0, 0, 0, '', '', 1),
(289544, 46744, 14, 0, 0, 58025, 1, 0, 0, 0, '', '', 1);

INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(255456, 26274, 0, 0, 0, 44175, 5, 0, 0, 0, '', '', 1),
(255455, 26274, 5, 0, -1, 172, 1, 0, 0, 0, '', '', 1);


-- https://forum.wowcircle.com/showthread.php?t=832207
delete from phase_definitions where zoneId = 139 and entry in (400035);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(139, 400035, 0, 400035, 0, '40745 quest');

delete from conditions where SourceGroup in (139) and SourceTypeOrReferenceId = 26 and sourceentry in (400035);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 139, 400035, 0, 0, 28, 0, 45147, 0, 0, 0, 0, '', 'Phase If quest 45147 completed'),
(26, 139, 400035, 0, 1, 8, 0, 45147, 0, 0, 0, 0, '', 'Phase If quest 45147 rewarded');

update quest_template set inprogressphaseid = 0 where id = 400035;


-- https://forum.wowcircle.com/showthread.php?t=1050908
update creature set phasemask = 1 where id = 103025;


-- https://forum.wowcircle.com/showthread.php?t=1050911
delete from smart_scripts where entryorguid = 993556;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(993556, 0, 0, 1, 10, 0, 100, 0, 0, 1, 3, 1000, 1000, 0, 62, 1220, 0, 0, 0, 0, 0, 7, 0, 0, 0, -1465.14, 1122.64, 313.859, 4.15003, 'teleport player'),
(993556, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 104848, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on link - give credit');


delete from creature where id = 97072;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(97072, 1220, 7679, 7679, 1, 1, 0, 0, 0, -1538.39, 1005.96, 313.734, 4.63915, 300, 0, 0, 4161, 0, 0);

update creature_template set npcflag = 3 where entry = 97072;


-- https://forum.wowcircle.com/showthread.php?t=1050916
delete from creature where id = 107806;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(107806, 0, 45, 324, 1, 1, 0, 0, 0, -1703.01, -1803.96, 101.708, 0.0224743, 300, 0, 0, 4161, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1050923
update creature_template set gossip_menu_id = 20554, ainame = 'SmartAI' where entry = 93465;

delete from gossip_menu_option where menu_id in (20554);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(20554, 0, 2, 'I want to travel fast', 4, 8192, 3409),
(20554, 1, 0, 'Send me to the Thorim Peak! ', 1, 1, 123847);

delete from gossip_menu where entry in (20554);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(20554, 7778);

delete from smart_scripts where entryorguid = 93465;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(93465, 0, 0, 0, 62, 0, 100, 0, 0, 20554, 1, 0, 0, 0, 85, 229431, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Grimwing - on gossip select - invoker cast');

delete from conditions where SourceGroup in (20554) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 20554, 1, 0, 0, 9, 0, 44775, 0, 0, 0, 0, '', 'Show gossip if 44775 quest incomplete');

delete from creature where id = 115360;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(115360, 1220, 7541, 7612, 1, 1, 0, 0, 0, 1915.56, 2057.92, 466.709, 5.25985, 300, 0, 0, 4161, 0, 0);

DELETE FROM `creature_questender` WHERE quest in (44775);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(115360, 44775);

update creature_template set npcflag = 2 where entry = 115360;