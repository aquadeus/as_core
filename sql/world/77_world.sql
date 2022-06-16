-- https://forum.wowcircle.com/showthread.php?t=1053940
update spell_scene set scriptname = 'sceneTrigger_blingtrons_circuit_design_tutorial' where miscvalue = 1167;


-- https://forum.wowcircle.com/showthread.php?t=1054006
update creature set phasemask = 0 , phaseid = 41216 where id = 111019;

delete from phase_definitions where zoneId = 7637 and entry in (41216);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7637, 41216, 0, 41216, 0, '41216 quest');

delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and sourceentry in (41216);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7637, 41216, 0, 0, 28, 0, 41216, 0, 0, 0, 0, '', 'Phase If quest 41216 completed'),
(26, 7637, 41216, 0, 1, 8, 0, 41216, 0, 0, 0, 0, '', 'Phase If quest 41216 rewarded');


-- https://forum.wowcircle.com/showthread.php?t=1053986
update creature_template set ainame = 'SmartAI', gossip_menu_id = 14961 where entry = 66258;
update creature_template set ainame = 'SmartAI', npcflag = 0 where entry = 66138;
update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry = 67060;

delete from gossip_menu_option where menu_id in (14961);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(14961, 0, 0, 'Я хочу вызвать тебя на бой.', 1, 1, 67350);

delete from conditions where SourceGroup in (14961) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 14961, 0, 0, 0, 9, 0, 31834, 0, 0, 0, 0, '', 'Show gossip if 31834 quest incomplete');

delete from gossip_menu where entry in (14961);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(14961, 21152);

delete from npc_text where id in (21152);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(21152, 67382, 67382);

delete from creature_text where Entry in (66258, 66138);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(66258, 0, 0, 'Welcome.  Speak with me when you are ready to begin.', 12, 0, 100, 0, 0, 0, 67991),
(66138, 0, 0, 'As a monk, your reflexes are among your greatest resources.  Often, the ability to think and move quickly can save your life.', 12, 0, 100, 0, 0, 0, 67434),
(66138, 1, 0, 'Bow to me and we shall begin.', 12, 0, 100, 0, 0, 0, 67433),
(66138, 2, 0, 'Master Cheng is burning you alive!  Move!', 41, 0, 100, 0, 0, 0, 67311),
(66138, 3, 0, 'Well done.  Remember that swiftness of mind and body can keep you alive.', 12, 0, 100, 0, 0, 0, 67386);

delete from smart_scripts where entryorguid in (66138,66258,67060);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(67060, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 11, 131832, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on just summoned - cast spell'),

(66138, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 5000, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Roll - on just summoned - talk'),
(66138, 0, 1, 0, 52, 0, 100, 1, 0, 66138, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 3942.1147, 1845.6406, 904.3334, 0, 'Roll - on text over - move to position'),
(66138, 0, 2, 3, 34, 0, 100, 1, 0, 1, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1.62, 'Roll - on point reached - set orientation'),
(66138, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - talk'),
(66138, 0, 4, 7, 61, 0, 100, 1, 0, 0, 0, 0, 0, 42, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - set invincibility'),
(66138, 0, 5, 6, 22, 0, 100, 1, 17, 0, 0, 0, 0, 5, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on receive emote - play emote'),
(66138, 0, 6, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 2, 14, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - set faction'),
(66138, 0, 7, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - set home position'),
(66138, 0, 8, 9, 2, 0, 100, 1, 0, 10, 0, 0, 0, 33, 66138, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Roll - on 10% hp - give credit'),
(66138, 0, 9, 10, 61, 0, 100, 1, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - talk'),
(66138, 0, 10, 11, 61, 0, 100, 1, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - set faction'),
(66138, 0, 11, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - despawn'),
(66138, 0, 12, 13, 0, 0, 100, 0, 10000, 10000, 30000, 30000, 0, 11, 130073, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Roll - in combat - cast spell'),
(66138, 0, 13, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - talk'),

(66258, 0, 0, 1, 62, 0, 100, 0, 14961, 0, 0, 0, 0, 85, 130276, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Roll - on gossip select - invoker cast'),
(66258, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Roll - on link - despawn'),
(66258, 0, 2, 0, 10, 0, 100, 0, 1, 20, 120000, 120000, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Roll - on ooc los - talk');

delete from conditions where SourceEntry in (130276) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 2, 130276, 0, 0, 31, 0, 3, 66258, 0, 0, 0, '', 'Cast spell into 66258 creature');

update creature set phaseid = 0, phasemask = 1, spawntimesecs = 60 where id = 66258;


-- https://forum.wowcircle.com/showthread.php?t=1054075
update quest_template set nextquestid = 25316 where id = 49855; 


-- https://forum.wowcircle.com/showthread.php?t=1054009
delete from spell_linked_spell where spell_trigger in (205779,205778,207934, -207931,-207935,205781);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(205781, 207931, 0, 0, 0, 0, 0, ''),
(205779, -207931, 0, 0, 0, 0, 0, ''),
(-207931, 207935, 0, 0, 0, 0, 0, ''),

(207934, -207935, 0, 0, 0, 0, 0, ''),
(-207935, 207936, 0, 0, 0, 0, 0, ''),

(205778, -207936, 0, 0, 0, 0, 0, '');

update creature_template set ainame = 'SmartAI', gossip_menu_id = 18758 where entry = 92183;

delete from smart_scripts where entryorguid in (92183) and id in (49, 48);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(92183, 0, 48, 49, 62, 0, 100, 0, 18758, 2, 0, 0, 0, 85, 205781, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Alard - on gossip select - invoker cast'),
(92183, 0, 49, 0, 61, 0, 100, 0, 18758, 2, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Alard - link - close gossip');

delete from gossip_menu_option where menu_id in (18758) and id = 2;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(18758, 2, 0, 'Alard, could you help me cast a new set of keys?', 1, 1, 106477);

delete from conditions where SourceGroup in (18758) and SourceTypeOrReferenceId = 15 and sourceentry = 2;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 18758, 2, 0, 0, 9, 0, 41185, 0, 0, 0, 0, '', 'Show gossip if 41185 quest incomplete'),
(15, 18758, 2, 0, 0, 1, 0, 207931, 0, 0, 1, 0, '', 'Show gossip if has aura 207931');


-- https://forum.wowcircle.com/showthread.php?t=1053988
delete from conditions where SourceGroup in (14961) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 14961, 0, 0, 0, 9, 0, 31834, 0, 0, 0, 0, '', 'Show gossip if 31834 quest incomplete'),
(15, 14961, 0, 0, 1, 9, 0, 31840, 0, 0, 0, 0, '', 'Show gossip if 31840 quest incomplete');


-- https://forum.wowcircle.com/showthread.php?t=1054162
delete from conditions where SourceEntry in (202039) and SourceTypeOrReferenceId = 17;