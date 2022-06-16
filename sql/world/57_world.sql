-- https://forum.wowcircle.com/showthread.php?t=1042754
update creature_model_info set combatreach = 5 where displayid = 13090;


-- https://forum.wowcircle.com/showthread.php?t=1046168
delete from spell_target_position where id in (254665);
INSERT INTO `spell_target_position` (`id`, `effindex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(254665, 1, 1669, -3544.7766, 9011.6680, -55.3187, 2.71);

delete from npc_spellclick_spells where npc_entry in (128342);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(128342, 254665, 1, 0);

delete from spell_script_names where spell_id = 254666;


-- команда
update command set security = 2, security_animator = 2 where name like 'lookup player ip';


-- https://forum.wowcircle.com/showthread.php?t=1048998
update creature set spawntimesecs = 180 where id = 16518;


-- https://forum.wowcircle.com/showthread.php?t=969309
delete from gossip_menu_option where menu_id in (18171);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(18171, 0, 0, '<Throw the crystals into the pool.>', 1, 1, 96138);

delete from gossip_menu where entry in (18171);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(18171, 26101);

delete from npc_text where id in (26101);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`) VALUES 
(26101, 92694);

delete from conditions where SourceGroup in (18171) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 18171, 0, 0, 0, 9, 0, 37853, 0, 0, 0, 0, '', 'Send Gossip If Player Has Quest'),
(15, 18171, 0, 0, 0, 2, 0, 122095, 6, 0, 0, 0, '', 'send gossip if player has item');

update creature set phaseid = 38443, phasemask = 0 where id = 993534;


-- https://forum.wowcircle.com/showthread.php?t=1048875
delete from quest_start_scripts where id in (38882,39122);