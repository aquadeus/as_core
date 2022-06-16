-- https://forum.wowcircle.com/showthread.php?t=1045948
update creature_template set ainame = 'SmartAI' where entry in (107726,107732);

update creature_template set gossip_menu_id = 19806 where entry in (107726);

delete from gossip_menu_option where menu_id in (19806);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(19806, 0, 0, 'Your family is at the Hammer\'s Cradle.', 1, 1, 112943);

delete from gossip_menu where entry in (19806);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(19806, 29362);

delete from npc_text where id in (29362);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`) VALUES 
(29362, 112910);

delete from smart_scripts where entryorguid in (107726,107732);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(107726, 0, 0, 0, 62, 0, 100, 0, 19806, 0, 0, 0, 0, 85, 213937, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ethel - on gossip select - invoker cast'),

(107732, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 53, 0, 107732, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ethel - on gossip select - invoker cast'),
(107732, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ethel - on link - talk'),
(107732, 0, 2, 0, 60, 0, 100, 0, 60000, 60000, 60000, 60000, 0, 1, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ethel - on update - talk'),
(107732, 0, 3, 0, 40, 0, 100, 1, 15, 107732, 0, 0, 0, 33, 96084, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Ethel - on waypoint reached - give credit');

delete from conditions where SourceGroup in (19806) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19806, 0, 0, 0, 9, 0, 42590, 0, 0, 0, 0, '', 'Show gossip option if player has 42590 quest'),
(15, 19806, 0, 0, 0, 48, 0, 284435, 0, 0, 1, 0, '', 'Show gossip option if player not complete criteria');

delete from creature_text where entry in (107732);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`) VALUES 
(107732, 0, 0, 'Let\'s go see Moozy!', 12, 0, 100, 0, 0, 112954),
(107732, 1, 0, 'There was a time when young ones were nice to old ladies.', 12, 0, 100, 0, 0, 112948),
(107732, 1, 1, 'Have you seen Moozy\'s smile? Gets it from me, I suspect.', 12, 0, 100, 0, 0, 112957),
(107732, 1, 2, 'Ah, these old bones haven\'t moved in a while.', 12, 0, 100, 0, 0, 112952);

delete from waypoints where entry in (107732);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(107732, 1, 4274.84, 4280.04, 738.041),
(107732, 2, 4261.37, 4269.56, 738.154),
(107732, 3, 4246.34, 4271.05, 738.154),
(107732, 4, 4235.54, 4275.18, 740.208),
(107732, 5, 4216.73, 4285.69, 741.262),
(107732, 6, 4194.34, 4299.38, 745.848),
(107732, 7, 4168.35, 4315.04, 763.764),
(107732, 8, 4159.86, 4320.17, 770.243),
(107732, 9, 4147.3, 4326.27, 768.244),
(107732, 10, 4124.26, 4323.7, 768.478),
(107732, 11, 4097.94, 4312.62, 768.368),
(107732, 12, 4076.2, 4309.48, 768.349),
(107732, 13, 4063.73, 4314.29, 768.435),
(107732, 14, 4061.01, 4326.21, 768.147),
(107732, 15, 4062.6321, 4334.3916, 768.1019);


-- https://forum.wowcircle.com/showthread.php?t=1041010
update creature set spawnmask = 1 where map = 1519;

DELETE FROM `spell_area` WHERE `spell` in (68243) and area = 8022;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(68243, 8022, 41033, 41060, 0, 0, 2, 1, 74, 66);

update gameobject set state = 1, phasemask = 2, phaseid = 0 where id = 254245;

delete from spell_linked_spell where spell_trigger in (-203269) and spell_effect in (203459);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(-203269, 203459, 0, 0, 0, 0, 0, '');


-- https://forum.wowcircle.com/showthread.php?t=1046454
update quest_template set prevquestid = 40964, exclusivegroup = -41032, nextquestid = 40969 where id in (40967,41032,40965);


-- https://forum.wowcircle.com/showthread.php?t=1046457
delete from spell_linked_spell where spell_trigger in (-204195) and spell_effect in (204204);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(-204195, 204204, 0, 0, 0, 0, 0, '');

update creature_template_wdb set killcredit1 = 0 where killcredit1 in (101783);
update creature_template_wdb set killcredit2 = 0 where killcredit2 in (101783);
update creature_template_wdb set killcredit1 = 101783 where entry in (111527,101784,111530,111599);

update areatrigger_template set scriptname = 'spell_at_trap_rune' where entry = 6003;

delete from conditions where SourceEntry in (204204,222218) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 204204, 0, 0, 31, 0, 3, 101783, 0, 0, 0, '', 'Cast Spell Only 101783 Creature'),
(13, 1, 204204, 0, 1, 31, 0, 3, 111527, 0, 0, 0, '', 'Cast Spell Only 111527 Creature'),
(13, 1, 204204, 0, 2, 31, 0, 3, 111599, 0, 0, 0, '', 'Cast Spell Only 111599 Creature'),
(13, 1, 204204, 0, 3, 31, 0, 3, 101784, 0, 0, 0, '', 'Cast Spell Only 101784 Creature'),
(13, 1, 204204, 0, 4, 31, 0, 3, 111530, 0, 0, 0, '', 'Cast Spell Only 111530 Creature'),

(13, 3, 222218, 0, 0, 31, 0, 3, 101783, 0, 0, 0, '', 'Cast Spell Only 101783 Creature'),
(13, 3, 222218, 0, 1, 31, 0, 3, 111527, 0, 0, 0, '', 'Cast Spell Only 111527 Creature'),
(13, 3, 222218, 0, 2, 31, 0, 3, 111599, 0, 0, 0, '', 'Cast Spell Only 111599 Creature'),
(13, 3, 222218, 0, 3, 31, 0, 3, 101784, 0, 0, 0, '', 'Cast Spell Only 101784 Creature'),
(13, 3, 222218, 0, 4, 31, 0, 3, 111530, 0, 0, 0, '', 'Cast Spell Only 111530 Creature');


-- https://forum.wowcircle.com/showthread.php?t=1046189
update quest_template set prevquestid = 14323 where id in (14324);


-- https://forum.wowcircle.com/showthread.php?t=1046736
DELETE FROM `creature_queststarter` WHERE quest in (34414,34415);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(78515, 34414),
(78513, 34415);

update quest_template set requiredraces = 234881970 where id = 34414;
update quest_template set requiredraces = 824181832 where id = 34415;


-- https://forum.wowcircle.com/showthread.php?t=1046737
update creature_template set ainame = 'SmartAI', gossip_menu_id = 16356 where entry = 78515;
update creature_template set ainame = 'SmartAI', gossip_menu_id = 16366, npcflag = 1 where entry = 78740;

delete from gossip_menu_option where menu_id in (16356,16366);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(16356, 0, 0, 'I am ready to confront Kaelynara.', 1, 1, 81134),
(16366, 0, 0, 'I am ready to return to the Jorune Mine.', 1, 1, 81135);

delete from gossip_menu where entry in (16356,16366);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(16356, 23693),
(16366, 23693);

delete from npc_text where id in (23693);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`) VALUES 
(23693, 81063);

delete from smart_scripts where entryorguid in (78515,78740);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(78515, 0, 0, 1, 62, 0, 100, 0, 16356, 0, 0, 0, 0, 33, 78703, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Elandra - On Gossip Select - Give Credit'),
(78515, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 62, 1116, 0, 0, 0, 0, 0, 7, 0, 0, 0, 2531.43, 2720.72, 234.953, 0.68, 'Elandra - On Link - Teleport Invoker'),
(78740, 0, 0, 0, 62, 0, 100, 0, 16366, 0, 0, 0, 0, 62, 1116, 0, 0, 0, 0, 0, 7, 0, 0, 0, 2419.11, 2758.66, 115.222, 5.36, 'Elandra - On Gossip Select - Teleport Invoker');

delete from conditions where SourceGroup in (16356) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 16356, 0, 0, 0, 9, 0, 34447, 0, 0, 0, 0, '', 'Show Gossip If Player Has 34447 quest');

delete from phase_definitions where zoneId = 6662 and entry in (34447,34448);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6662, 34448, 0, 34448, 0, '34448 quest'),
(6662, 34447, 0, 34447, 0, '34447 quest');

delete from conditions where SourceGroup in (6662) and SourceTypeOrReferenceId = 26 and sourceentry in (34447,34448);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6662, 34448, 0, 0, 28, 0, 34448, 0, 0, 0, 0, '', 'Phase If Player has quest 34448'),
(26, 6662, 34447, 0, 0, 28, 0, 34447, 0, 0, 0, 0, '', 'Phase If Player has quest 34447');

delete from creature where id in (78740);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(78740, 1116, 6662, 0, 1, 0, 34447, 0, 0, 2532.1699, 2726.3101, 235.0370, 0, 300, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature set phasemask = 0, phaseid = 34448 where id = 78741;


-- https://forum.wowcircle.com/showthread.php?t=1046895
delete from conditions where SourceEntry in (79423) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 79423, 0, 0, 31, 0, 3, 42342, 0, 0, 0, '', 'Cast Spell Only 42342 Creature');