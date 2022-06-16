-- https://forum.wowcircle.com/showthread.php?t=815224
DELETE FROM `item_script_names` WHERE id in (139487,139484,139486,139488,140150,140148,140152,140154);
INSERT INTO `item_script_names` (`Id`, `ScriptName`) VALUES
(139487, 'item_cache_of_nightmarish_treasures'),
(139484, 'item_cache_of_nightmarish_treasures'),
(139486, 'item_cache_of_nightmarish_treasures'),
(139488, 'item_cache_of_nightmarish_treasures'),
(140150, 'item_cache_of_nightborne_treasures'),
(140148, 'item_cache_of_nightborne_treasures'),
(140152, 'item_cache_of_nightborne_treasures'),
(140154, 'item_cache_of_nightborne_treasures');


-- https://forum.wowcircle.com/showthread.php?t=1060713
delete from gossip_menu_option where menu_id in (10338);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(10338, 0, 0, 'Come now, friend. You chop and I will protect you from the elves, ok?', 1, 1, 33545);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` = 10338;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 10338, 0, 0, 0, 9, 0, 13640, 0, 0, 0, 0, '', 'Gossip if quest incomplete');

delete from gossip_menu where entry in (10338);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(10338, 10338);

delete from npc_text where id in (10338);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(10338, 33542, 33542);

update creature_template set gossip_menu_id = 10338, AIName = 'SmartAI' where entry in (33440);

update creature_template set AIName = 'SmartAI' where entry in (33458);

delete from smart_scripts where entryorguid in (33440,33458,3344000);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(33458, 0, 0, 1, 54, 0, 100, 0, 10338, 0, 0, 0, 0, 11, 58506, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'stalker - on just summoned - cast'),
(33458, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'stalker - on link - move point'),
(33458, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'stalker - on link - talk'),

(33440, 0, 0, 0, 62, 0, 100, 0, 10338, 0, 0, 0, 0, 80, 3344000, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'peon - on gossip select - run timed action list'),

(3344000, 9, 0 , 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 83, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - remove npc flag'),
(3344000, 9, 1 , 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 89, 15, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - random move'),
(3344000, 9, 2 , 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(3344000, 9, 3 , 0, 0, 0, 100, 1, 10000, 10000, 0, 0, 0, 103, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - set root'),
(3344000, 9, 4 , 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 5, 234, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - play emote'),
(3344000, 9, 5 , 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - talk'),
(3344000, 9, 6 , 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 11, 63052, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - cast'),
(3344000, 9, 7 , 0, 0, 0, 100, 1, 10000, 10000, 0, 0, 0, 11, 63043, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - cast'),
(3344000, 9, 8 , 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - play emote'),
(3344000, 9, 9 , 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 103, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - set root'),
(3344000, 9, 10, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 89, 15, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - random move'),
(3344000, 9, 11, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - random move'),
(3344000, 9, 12, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'timed action list - despawn');

delete from creature_text where entry in (33440,33458);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`, `comment`) VALUES 
(33458, 0, 0, 'Ashenvale belongs to the Kaldorei!', 12, 0, 100, 0, 0, 34912, ''),
(33458, 0, 1, 'Death to the Horde!', 12, 0, 100, 0, 0, 33337, ''),
(33458, 0, 2, 'No longer shall we tolerate your presence here, orc!', 12, 0, 100, 0, 0, 33576, ''),
(33458, 0, 3, 'The Horde is not welcome in our lands, orc! Now die!', 12, 0, 100, 0, 0, 33579, ''),

(33440, 0, 0, 'Watch my back, $r!', 12, 0, 100, 0, 0, 33575, ''),
(33440, 0, 1, 'Here I go! Keep an eye out....', 12, 0, 100, 0, 0, 33573, ''),
(33440, 0, 2, 'I have bad feeling about this....', 12, 0, 100, 0, 0, 33574, ''),
(33440, 0, 3, 'I\'m trusting you... Don\'t let me die!', 12, 0, 100, 0, 0, 33570, ''),
(33440, 1, 0, 'You keep a lookout, $r! The elfs are everywhere!', 12, 0, 100, 0, 0, 33553, ''),
(33440, 1, 1, 'You not gunna let me die, huh? Okay, then...', 12, 0, 100, 0, 0, 33551, ''),
(33440, 1, 2, 'You stay close, $r. And don\'t let elfs near me!', 12, 0, 100, 0, 0, 33550, ''),
(33440, 1, 3, 'Hmm. You don\'t look very tough... but okay.', 12, 0, 100, 0, 0, 33549, ''),
(33440, 2, 0, 'There you go, $r.', 12, 0, 100, 0, 0, 33571, '');

update gameobject_template set flags = 0 where entry = 194349;


-- https://forum.wowcircle.com/showthread.php?t=1060771
update quest_template set PrevQuestId = 24558 where id = 24559;


-- https://forum.wowcircle.com/showthread.php?t=1060908
update creature_template set unit_flags = 768, flags_extra = 128 where entry in (25156,25157,25154);


-- https://forum.wowcircle.com/showthread.php?t=1060911
delete from spell_target_position where id = 46841;
INSERT INTO `spell_target_position` (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`, `VerifiedBuild`) VALUES 
(46841, 0, 530, 12891.8633, -6874.6372, 9.0136, 0.37, 1);


-- https://forum.wowcircle.com/showthread.php?t=963674
delete from creature where id = 85062;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(85062, 1116, 6722, 7219, 1, 1, 0, 0, 1, -1602.26, 889.468, 0.0830243, 5.97853, 300, 0, 0, 108727, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);