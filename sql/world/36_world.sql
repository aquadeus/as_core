-- https://forum.wowcircle.net/showthread.php?t=804958
delete from conditions where SourceEntry in (94714) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 94714, 0, 0, 31, 0, 5, 207735, 0, 0, 0, '', 'Cast only 207735');

delete from spell_linked_spell where spell_trigger in (94687) and spell_effect in (94688);
delete from spell_linked_spell where spell_trigger in (-94688) and spell_effect in (94689);
delete from spell_linked_spell where spell_trigger in (94714) and spell_effect in (94720);
delete from spell_linked_spell where spell_trigger in (-94720) and spell_effect in (93107);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`, `actiontype`) VALUES 
(94687, 94688, 0, 0, 0, 0, 0, '', 0),
(-94688, 94689, 0, 0, 0, 0, 0, '', 0),
(94714, 94720, 0, 0, 0, 0, 0, '', 0),
(-94720, 93107, 0, 0, 0, 0, 0, '', 0);

DELETE FROM `spell_area` WHERE `spell` in (94714);
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(94714, 5416, 28826, 28826, 0, 0, 2, 1, 8, 66),
(94714, 5416, 28805, 28805, 0, 0, 2, 1, 8, 66);

delete from spell_script_names where spell_id in (94720);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(94720, 'spell_gen_cinematic_timer');

update gameobject_template set flags = 4 where entry = 207735;


-- https://forum.wowcircle.net/showthread.php?t=798062
update creature_template set npcflag = 16777216, unit_flags = 768 where entry = 120277;


-- https://forum.wowcircle.net/showthread.php?t=805868
update creature_template set scriptname = '', ainame = 'SmartAI' where entry = 29445;

delete from smart_scripts where entryorguid in (29445);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(29445, 0, 0, 0, 62, 0, 100, 0, 0, 9926, 0, 0, 0, 0, 33, 30514, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Thorim - gossip select  - give quest credit');


-- https://forum.wowcircle.net/showthread.php?t=1040879&p=7561165#post7561165
delete from game_event_gameobject where guid in (select guid from gameobject where id in ('179968', '195164', '180353', '195664', '195198', '195212', '195194', '195196', '195195', '195200', '195191', '195192', '195197') and areaid = 7486)
and evententry = 11;
insert into game_event_gameobject (guid, evententry) values
(267057015, 11),
(267057016, 11),
(267057017, 11),
(267057018, 11),
(267057019, 11),
(267057006, 11),
(267057014, 11),
(267057013, 11),
(267057012, 11),
(267057011, 11),
(267057010, 11),
(267057009, 11),
(267057008, 11),
(267057007, 11),
(267057005, 11),
(267057004, 11),
(267057003, 11),
(267057002, 11),
(267057001, 11),
(267057000, 11),
(267056999, 11),
(267056998, 11),
(267057153, 11),
(267057147, 11),
(267057148, 11),
(267057149, 11),
(267057150, 11),
(267057151, 11),
(267057152, 11),
(267057155, 11),
(267057156, 11),
(267057154, 11),
(267057283, 11),
(267057288, 11),
(267057297, 11),
(267057300, 11),
(267057299, 11),
(267057298, 11),
(267057296, 11),
(267057295, 11),
(267057294, 11),
(267057293, 11),
(267057292, 11),
(267057291, 11),
(267057290, 11),
(267057289, 11),
(267057287, 11),
(267057286, 11),
(267057285, 11),
(267057284, 11),
(267057414, 11),
(267057460, 11),
(267057445, 11),
(267057430, 11),
(267057437, 11),
(267057369, 11),
(267057422, 11),
(267057452, 11),
(267057362, 11),
(267057363, 11),
(267057364, 11),
(267057396, 11),
(267057395, 11);


-- https://forum.wowcircle.net/showthread.php?t=808305
update creature_template set npcflag = 16777216, spell1 = 60288 where entry in (32254);
update creature_template set npcflag = 16777216 where entry in (34832);

delete from npc_spellclick_spells where npc_entry in (32254,34832);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(32254, 46598, 1, 0),
(34832, 46598, 1, 0);

delete from vehicle_template_accessory where entryoraura = 34832;
INSERT INTO `vehicle_template_accessory` (`EntryOrAura`, `accessory_entry`, `seat_id`, `offsetX`, `offsetY`, `offsetZ`, `offsetO`, `minion`, `description`, `summontype`, `summontimer`) VALUES 
(34832, 40942, 0, 0.181018, 0, 1.19389, 0, 1, '34832 - 40942', 8, 0);

delete from creature where id = 40942;

update creature_template_wdb set killcredit1 = 40942 where entry = 34832;

delete from conditions where SourceGroup in (32254) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 32254, 46598, 0, 0, 9, 0, 25614, 0, 0, 0, 0, '', 'Allow spellclick if player has quest');

delete from conditions where SourceEntry in (65951) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 65951, 0, 0, 31, 0, 3, 34832, 0, 0, 0, '', 'Cast only 34832'),
(13, 1, 65951, 0, 1, 31, 0, 3, 40942, 0, 0, 0, '', 'Cast only 40942');


-- https://forum.wowcircle.net/showthread.php?t=808768
update creature_template set gossip_menu_id = 10296, ainame = 'SmartAI' where entry in (32911);
update creature_template set gossip_menu_id = 10293, ainame = 'SmartAI' where entry in (33093);
update creature_template set gossip_menu_id = 10294, ainame = 'SmartAI' where entry in (33094);
update creature_template set gossip_menu_id = 10295, ainame = 'SmartAI' where entry in (33095);

delete from creature_template_addon where entry in (32911,33093,33094,33095);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(32911, 0, 0, 0, '87862'),
(33093, 0, 0, 0, '87862'),
(33094, 0, 0, 0, '87862'),
(33095, 0, 0, 0, '87862');

delete from smart_scripts where entryorguid in (32911,33093,33094,33095);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(32911, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 32911, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Survivor - on gossip hello - give credit'),
(33093, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 33093, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Survivor - on gossip hello - give credit'),
(33094, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 33094, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Survivor - on gossip hello - give credit'),
(33095, 0, 0, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 33095, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Survivor - on gossip hello - give credit');

delete from gossip_menu where entry in (10296,10293,10294,10295);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(10293, 14297),
(10294, 14298),
(10295, 14299),
(10296, 14300);


-- https://forum.wowcircle.net/showthread.php?t=807380
update creature_template set scriptname = '' where entry = 2548; -- npc_captain_keelhaul


-- https://forum.wowcircle.net/showthread.php?t=809156
update creature set movementtype = 0, spawndist = 0 where id = 31786;
update creature_template set npcflag = 16777216 where entry = 31736;
delete from smart_scripts where entryorguid in (31736);

update creature_template set spell1 = 59643 where entry = 31784;

delete from spell_script_names where spell_id in (59643,4338);

update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry in (31766);

delete from smart_scripts where entryorguid in (31766);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(31766, 0, 0, 2, 8, 0, 100, 0, 4338, 0, 0, 0, 0, 11, 4339, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - cast spell'),
(31766, 0, 1, 2, 8, 0, 100, 0, 59643, 0, 0, 0, 0, 11, 59642, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - cast spell'),
(31766, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 31766, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - link - give credit');

delete from creature where id in (31766) and areaid = 5330;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(31766, 571, 210, 4513, 1, 1, 0, 0, 1, 6913.9102, 2002.7000, 944.0310, 6.07376, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);