

delete from spell_script_names where spell_id in (203808, 203843, 203844, 203845);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
('203808', 'spell_item_pvp_pennants_toy'),
('203843', 'spell_item_pvp_pennants_toy'),
('203844', 'spell_item_pvp_pennants_toy'),
('203845', 'spell_item_pvp_pennants_toy');



-- Q9671 -- Take item 24132 from Admiral's gossip

-- Not blizzlike `option_text` and `OptionBroadcastTextID`
delete from gossip_menu_option where menu_id=7399;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES
('7399', '0', '0', 'I\'ve lost your letter.', '1', '1', '0', '0', '0', '0', NULL, '0', '0', '0');

set @maxId = (select max(id) from smart_scripts where entryorguid=17240 and source_type=0) + 1;
delete from smart_scripts where entryorguid=17240 and event_type=62 and source_type=0;
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
('17240', NULL, '0', @maxId, '0', '62', '0', '100', '0', '0', '0', '0', '0', '0', '0', '56', '24132', '1', '0', '0', '0', '0', '7', '0', '0', '0', '0', '0', '0', '0', 'Gossip select - Additem');

delete from conditions where SourceTypeOrReferenceId=15 and SourceGroup=7399;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
('15', '7399', '0', '0', '0', '8', '0', '9671', '0', '0', '0', '0', '', 'gossip only if complete quest');