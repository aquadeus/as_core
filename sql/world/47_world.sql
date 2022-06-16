-- https://forum.wowcircle.com/showthread.php?t=1044087
update gameobject_template set ainame = 'SmartGameObjectAI', data10 = 163638, flags = 0, data3 = 300000 where entry in (230889,230912,230913);

delete from smart_scripts where entryorguid in (230889,230912,230913);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(230889, 1, 0, 0, 70, 0, 100, 0, 2, 0, 0, 0, 0, 33, 80399, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Boulderthrower - on set state - give credit'),
(230912, 1, 0, 0, 70, 0, 100, 0, 2, 0, 0, 0, 0, 33, 80399, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Boulderthrower - on set state - give credit'),
(230913, 1, 0, 0, 70, 0, 100, 0, 2, 0, 0, 0, 0, 33, 80399, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Boulderthrower - on set state - give credit');

delete from gameobject where id in (231106,231105);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
(231106, 1116, 6662, 7131, 1, 1, 0, 1666.24, 1679.84, 304.696, 4.90578, 0, 0, 0, 0, 300, 100, 1),
(231105, 1116, 6662, 7131, 1, 1, 0, 1668.67, 1709.96, 297.93, 2.11023, 0, 0, 0, 0, 300, 100, 1); 

delete from creature where id = 80615;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`) VALUES 
(80615, 1116, 6662, 7131, 1, 1, 0, 0, 0, 1671.84, 1726.08, 292.139, 3.64975, 1, 0, 0, 364240);

delete from creature_text where entry = 80615;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(80615, 0, 0, 'How quaint. A wall! The Gordunni really have been disconnected from the empire for too long.', 12, 0, 100, 0, 0, 0, 82830),
(80615, 1, 0, 'This ought to get us over!', 12, 0, 100, 0, 0, 0, 82831);

update creature_template set ainame = 'SmartAI' where entry = 80615;

delete from smart_scripts where entryorguid in (80615);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(80615, 0, 0, 1, 10, 0, 100, 0, 1, 15, 0, 0, 0, 33, 80612, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'serena - on ooc los - give credit'),
(80615, 0, 1, 0, 61, 0, 100, 0, 1, 15, 0, 0, 0, 1, 0, 6000, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'serena - on link - talk'),
(80615, 0, 2, 0, 52, 0, 100, 0, 0, 80615, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'serena - on text over - talk');

delete from conditions where SourceEntry in (80615) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 80615, 0, 0, 9, 0, 34913, 0, 0, 0, 0, '', 'Smart if player has quest 34913'),
(22, 1, 80615, 0, 0, 48, 0, 273184, 0, 0, 1, 0, '', 'Smart if player not complete objective');

DELETE FROM `spell_area` WHERE `spell` in (162780);
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(162780, 7131, 34913, 34711, 0, 0, 2, 1, 64, 74),
(162780, 7176, 34913, 34711, 0, 0, 2, 1, 64, 74),
(162780, 7179, 34913, 34711, 0, 0, 2, 1, 64, 74);

update gameobject_template set size = 1, data1 = 0, flags = 0 where entry in (231106,231105);

delete from spell_target_position where id in (162793,162792);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(162792, 1116, 1667.9695, 1669.1227, 304.4747, 4.8),
(162793, 1116, 1666.6031, 1722.4209, 293.6241, 0.46);

update creature_template set npcflag = 2 where entry = 80617;

DELETE FROM `creature_queststarter` WHERE id = 80617;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(80617, 34912),
(80617, 34711);

DELETE FROM `creature_questender` WHERE id = 80617;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(80617, 34910),
(80617, 34909),
(80617, 34912),
(80617, 34911);


-- https://forum.wowcircle.com/showthread.php?t=1044159
delete from creature where id = 64249;

delete from npc_spellclick_spells where npc_entry = 64249;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(64249, 46598, 1, 0);

update creature_template set ainame = 'SmartAI', npcflag = 16777216, vehicleid = 2358, spell1 = 125452, inhabittype = 4, speed_fly = 2.5 where entry = 64249;

update creature_template set gossip_menu_id = 14381, ainame = 'SmartAI', npcflag = 1 where entry = 64244;

update creature_template set ainame = 'SmartAI' where entry = 64269;

delete from gossip_menu_option where menu_id in (14381);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(14381, 0, 0, 'Let\'s do this, Mishi!', 1, 1, 63998);

delete from gossip_menu where entry in (14381);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(14381, 20278);

delete from npc_text where id in (20278);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`) VALUES 
(20278, 63922);

delete from conditions where SourceGroup in (14381) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 14381, 0, 0, 0, 9, 0, 31303, 0, 0, 0, 0, '', 'Show gossip option if player has 31303');

delete from smart_scripts where entryorguid in (64244,64269,64249);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(64269, 0, 0, 1, 8, 0, 100, 1, 125456, 0, 0, 0, 0, 33, 64269, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(64269, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - despawn'),

(64249, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 53, 1, 642490, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mishi - on just summoned - start waypoint'),
(64249, 0, 1, 0, 40, 0, 100, 1, 25, 64249, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mishi - on waypoint reached - remove aura'),

(64244, 0, 0, 0, 62, 0, 100, 0, 14381, 0, 0, 0, 0, 85, 125374, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Mishi - on gossip select - invoker cast');

delete from spell_target_position where id in (125374);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(125374, 870, 538.0863, -1673.9762, 212.1014, 5.8);

delete from creature_template_addon where entry in (64269);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(64269, 0, 0, 0, '125406');

delete from creature where id = 64269;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 744.172, -1894.3, 58.6266, 1.81015, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 827.174, -1757.2, 56.8933, 6.26528, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 744.172, -1894.3, 58.6266, 1.81015, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 888.786, -1819.2, 64.0107, 5.24998, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 747.809, -1800.69, 56.4956, 0.802571, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 686.981, -1883.43, 56.2056, 1.76793, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 911.378, -1801.36, 64.6547, 5.34001, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 902.566, -1953.8, 60.7754, 3.8202, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 863.295, -1969.06, 59.7136, 3.66588, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 976, -1804.33, 65.3631, 5.2616, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 1004.41, -1800.91, 74.2074, 5.18211, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 761.262, -2063.78, 61.9125, 2.50105, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(64269, 870, 5785, 5876, 1, 1, 0, 21342, 0, 923.557, -1998.14, 62.7517, 3.80517, 30, 0, 0, 7987, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from waypoints where entry in (642490);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(642490, 7, 828.591, -1775.78, 101.927),
(642490, 6, 798.812, -1770.7, 104.266),
(642490, 5, 756.965, -1765.05, 113.115),
(642490, 4, 716.394, -1751.15, 128.313),
(642490, 3, 682.923, -1729.75, 146.483),
(642490, 2, 615.581, -1695.49, 188.091),
(642490, 1, 560.08, -1681.89, 209.875),
(642490, 8, 859.548, -1785.87, 100.194),
(642490, 9, 879.937, -1798.39, 99.6274),
(642490, 10, 900.321, -1827.71, 98.7152),
(642490, 11, 904.784, -1843.43, 98.406),
(642490, 12, 911.719, -1880.63, 98.7012),
(642490, 13, 910.01, -1916.04, 101.545),
(642490, 14, 887.141, -1950.92, 103.943),
(642490, 15, 857.666, -1966.97, 104.099),
(642490, 16, 822.839, -1970.91, 104.979),
(642490, 17, 795.545, -1960.81, 105.562),
(642490, 18, 751.391, -1931.09, 105.82),
(642490, 19, 729.904, -1906.6, 106.997),
(642490, 20, 712.677, -1881.07, 111.172),
(642490, 21, 680.357, -1842.88, 134.075),
(642490, 22, 625.519, -1782.96, 169.479),
(642490, 23, 586.884, -1737.04, 192.219),
(642490, 24, 543.015, -1685.17, 204.6),
(642490, 25, 528.408, -1668.97, 202.422);


-- https://forum.wowcircle.com/showthread.php?t=1044198
update creature_template set ainame = 'SmartAI', unit_flags = 768 where entry in (81698,81699,81704,81701,81733);

update creature_template set vehicleid = 3672 where entry in (81701);

delete from smart_scripts where entryorguid in (81698,81699,81704,81701,8169800,81733);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(81733, 0, 0, 1, 10, 0, 100, 1, 1, 5, 0, 0, 0, 33, 81733, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Duratan - on occ los - give credit'),
(81733, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 12, 81698, 1, 600000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Duratan - on link - summon creature'),
(81733, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Duratan - on link - despawn'),

(81698, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 53, 1, 816980, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Duratan - on just summoned - start waypoint'),
(81698, 0, 1, 2, 61, 0, 100, 1, 0, 0, 0, 0, 0, 12, 81699, 1, 300000, 0, 0, 0, 8, 0, 0, 0, 4148.6235, 2695.8838, 23.6033, 4.73, 'Duratan - on link - summon creature'),
(81698, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 12, 81704, 1, 600000, 0, 0, 0, 8, 0, 0, 0, 4152.2617, 2695.9724, 23.5996, 4.7, 'Duratan - on link - summon creature'),
(81698, 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 12, 81701, 1, 300000, 0, 0, 0, 8, 0, 0, 0, 4147.7656, 2715.1355, 0.0003, 2.59, 'Duratan - on link - summon creature'),
(81698, 0, 4, 0, 40, 0, 100, 1, 4, 816980, 0, 0, 0, 80, 8169800, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Duratan - on waypoint reached - timed action list'),
(81698, 0, 5, 6, 38, 0, 100, 0, 0, 1, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Duratan - on data set - talk'),
(81698, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 0, 53, 1, 816981, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Duratan - on link - start wypoint'),
(81698, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Duratan - on link - despawn'),

(8169800, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(8169800, 9, 1, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 81699, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(8169800, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(8169800, 9, 3, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 0, 84, 1, 0, 0, 0, 0, 0, 19, 81699, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(8169800, 9, 4, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(8169800, 9, 5, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 84, 2, 0, 0, 0, 0, 0, 19, 81699, 0, 0, 0, 0, 0, 0, 'Timed Action List - talk'),
(8169800, 9, 6, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 0, 33, 81699, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Timed Action List - give credtit'),
(8169800, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 81699, 0, 0, 0, 0, 0, 0, 'Timed Action List - set data'),
(8169800, 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 81704, 0, 0, 0, 0, 0, 0, 'Timed Action List - set data'),

(81699, 0, 0, 0, 38, 0, 100, 1, 0, 1, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 4150.0752, 2711.0483, 24.4092, 0, 'Orgrim - on set data - move to position'),
(81699, 0, 1, 2, 34, 0, 100, 1, 0, 1, 0, 0, 0, 97, 10, 10, 0, 0, 0, 0, 8, 0, 0, 0, 4147.7656, 2715.1355, 0.0003, 0, 'Orgrim - on point reached - jump to position'),
(81699, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 11, 46598, 0, 0, 0, 0, 0, 19, 81701, 0, 0, 0, 0, 0, 0, 'Orgrim - on link - cast spell'),
(81699, 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Orgrim - on link - despawn'),

(81701, 0, 0, 1, 8, 0, 100, 1, 46598, 0, 0, 0, 0, 69, 1, 1, 0, 0, 0, 0, 8, 0, 0, 0, 4130.2202, 2730.7915, 0.0013, 0, 'Boat - on spell hit - move to position'),
(81701, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Boat - on spell hit - move to position'),

(81704, 0, 0, 1, 38, 0, 100, 1, 0, 1, 0, 0, 0, 19, 768, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lieutenant - on set data - remove unit_flag'),
(81704, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 30, 0, 0, 0, 0, 0, 0, 'Lieutenant - on link - attack'),
(81704, 0, 2, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 81698, 0, 0, 0, 0, 0, 0, 'Lieutenant - on death - set data');

delete from waypoints where entry in (816980,816981);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES
(816981, 3, 4128.76, 2609.42, 23.6478),
(816981, 2, 4130.47, 2646.94, 23.6098),
(816981, 1, 4136.83, 2669.39, 23.6205),

(816980, 4, 4149.94, 2681.71, 23.6101),
(816980, 3, 4150, 2672.15, 24.0367),
(816980, 2, 4150.1, 2652.9, 25.3169),
(816980, 1, 4149.75, 2627.25, 26.8353);

delete from conditions where SourceEntry in (81733) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 81733, 0, 0, 9, 0, 35226, 0, 0, 0, 0, '', 'Smart if player has quest 34913'),
(22, 1, 81733, 0, 0, 48, 0, 273621, 0, 0, 1, 0, '', 'Smart if player not complete objective');

delete from creature_text where entry in (81698,81699);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 
(81698, 0, 0, 'Оргрим! Брат, что ты наделал?', 12, 0, 100, 5, 0, 45293, 83664, ''),
(81698, 1, 0, 'Очнись! Железная Орда уничтожит Дренор!', 12, 0, 100, 396, 0, 45294, 83666, ''),
(81698, 2, 0, 'Услышь голос разума! Остановись!', 12, 0, 100, 274, 0, 45295, 83684, ''),
(81698, 3, 0, 'Увидимся позже. Мне надо встретиться с Дрекой.', 12, 0, 100, 396, 0, 45296, 83668, ''),
(81699, 0, 0, 'Брат? Ты вступил в союз с захватчиками, Дуротан. Ты сделал свой выбор.', 12, 0, 100, 6, 0, 46082, 83665, ''),
(81699, 1, 0, 'Мои видения предрекают иное.', 12, 0, 100, 1, 0, 46083, 83667, ''),
(81699, 2, 0, 'Разум молчит, когда говорят пушки. Покончить с ними!', 12, 0, 100, 25, 0, 46084, 83685, '');

delete from creature where id = 81733;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(81733, 1116, 6662, 6947, 1, 1, 0, 0, 0, 4148.12, 2618.23, 26.9106, 1.53782, 300, 0, 0, 18986000, 0, 0);

delete from phase_definitions where zoneId = 6662 and entry = 35226;
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6662, 35226, 0, 35226, 0, '35226 quest');

delete from conditions where SourceGroup in (6662) and SourceTypeOrReferenceId = 26 and sourceentry = 35226;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6662, 35226, 0, 0, 9, 0, 35226, 0, 0, 0, 0, '', 'Phase If Player has Quest'),
(26, 6662, 35226, 0, 1, 28, 0, 35226, 0, 0, 0, 0, '', 'Phase If Player complete Quest');


-- https://forum.wowcircle.com/showthread.php?t=1044251
DELETE FROM `creature_queststarter` WHERE quest = 35189;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(81530, 35189);


-- https://forum.wowcircle.com/showthread.php?t=1044268
delete from gameobject where id in (230729,230728,230734,230735,230733);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
(230734, 1116, 6662, 7148, 1, 1, 0, 3180.85, 1775.9, 153.583, 0, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 7148, 1, 1, 0, 3187.35, 1764.87, 153.685, 4.67505, 0, 0, 0, 0, 180, 255, 1),
(230735, 1116, 6662, 7148, 1, 1, 0, 3183.19, 1776.23, 153.115, 0, 0, 0, 0, 0, 180, 255, 1),
(230735, 1116, 6662, 7148, 1, 1, 0, 3225.7, 1810.23, 153.967, 0, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 7148, 1, 1, 0, 3219.19, 1806.64, 153.756, 4.67505, 0, 0, 0, 0, 180, 255, 1),
(230734, 1116, 6662, 7148, 1, 1, 0, 3223.67, 1807.23, 154.318, 0, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 7148, 1, 1, 0, 3221.34, 1759.68, 150.418, 1.68603, 0, 0, 0, 0, 180, 255, 1),
(230733, 1116, 6662, 7148, 1, 1, 0, 3231.33, 1776.45, 151.09, 0, 0, 0, 0, 0, 180, 255, 1),
(230733, 1116, 6662, 7148, 1, 1, 0, 3222.49, 1745.68, 151.64, 0, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 7148, 1, 1, 0, 3244.25, 1770.85, 151, 0, 0, 0, 0, 0, 180, 255, 1),
(230729, 1116, 6662, 7148, 1, 1, 0, 3221.33, 1746.73, 157.089, 5.7672, 0, 0, 0, 0, 180, 255, 1),
(230733, 1116, 6662, 7148, 1, 1, 0, 3204.78, 1727.65, 161.05, 0, 0, 0, 0, 0, 180, 255, 1),
(230735, 1116, 6662, 7148, 1, 1, 0, 3252.95, 1741.78, 152.747, 0, 0, 0, 0, 0, 180, 255, 1),
(230729, 1116, 6662, 7148, 1, 1, 0, 3279.92, 1729.77, 169.829, 5.72231, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 7148, 1, 1, 0, 3284.36, 1729.97, 163.754, 1.67895, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 6662, 1, 1, 0, 3317.86, 1686.97, 155.639, 4.67505, 0, 0, 0, 0, 180, 255, 1),
(230734, 1116, 6662, 6662, 1, 1, 0, 3327.24, 1693.19, 156.689, 0, 0, 0, 0, 0, 180, 255, 1),
(230733, 1116, 6662, 6662, 1, 1, 0, 3297.29, 1679.1, 156.636, 0, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 6662, 1, 1, 0, 3316.44, 1688.35, 155.771, 0, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 6662, 1, 1, 0, 3324.38, 1687.31, 155.734, 4.67505, 0, 0, 0, 0, 180, 255, 1),
(230728, 1116, 6662, 7148, 1, 1, 0, 3196.12, 1706.19, 162.089, 2.13824, 0, 0, 0, 0, 180, 255, 1);

delete from gameobject_loot_template where entry in (230729,230728,230734,230735,230733);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(230728, 111735, -100, 0, 0, 1, 1),
(230729, 111735, -100, 0, 0, 1, 1),
(230733, 111736, -100, 0, 0, 1, 1),
(230734, 111736, -100, 0, 0, 1, 1),
(230735, 111736, -100, 0, 0, 1, 1);

update gameobject_template set data1 = entry, flags = 0 where entry in (230729,230728,230734,230735,230733);


-- https://forum.wowcircle.com/showthread.php?t=1044281
update creature_loot_template set chanceorquestchance = -100 where item = 140312;


-- https://forum.wowcircle.com/showthread.php?t=1044299
delete from creature_queststarter where id = 81961 and quest = 35339;