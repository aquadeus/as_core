-- https://forum.wowcircle.com/showthread.php?t=1061382
update creature_template set AIName = 'SmartAI' where entry = 71500;

delete from smart_scripts where entryorguid in (71500, 7150000, 7150001, 7150002, 7150003, 7150004, 7150005);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(71500, 0, 0, 0, 54, 0, 100, 0, 10338, 0, 0, 0, 0, 87, 7150000, 7150001, 7150002, 7150003, 7150004, 7150005, 1, 0, 0, 0, 0, 0, 0, 0, 'snake - on just summoned - random timed actionlist'),

(7150000, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 0 , 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'timed actionlist - follow'),
(7150001, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 10, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'timed actionlist - follow'),
(7150002, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 20, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'timed actionlist - follow'),
(7150003, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 30, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'timed actionlist - follow'),
(7150004, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 40, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'timed actionlist - follow'),
(7150005, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 50, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'timed actionlist - follow');


-- https://forum.wowcircle.com/showthread.php?t=1061381
DELETE FROM `spell_scene` WHERE MiscValue in (198);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`, `TrigerSpell`, `MonsterCredit`, `ScriptName`, `Comment`) VALUES
(462, 198, 0, 0, 0, '', '');


-- https://forum.wowcircle.com/showthread.php?t=833335
delete from creature_loot_template where item in (139771);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(104636, 139771, 10, 1, 1, 1, 1, 0, 0, 0),
(100497, 139771, 10, 1, 1, 1, 1, 0, 0, 0),
(102672, 139771, 10, 1, 1, 1, 1, 0, 0, 0),
(102679, 139771, 10, 1, 1, 1, 1, 0, 0, 0),
(103769, 139771, 10, 1, 1, 1, 1, 0, 0, 0),
(105393, 139771, 10, 1, 1, 1, 1, 0, 0, 0),
(106087, 139771, 10, 1, 1, 1, 1, 0, 0, 0);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 1 AND `SourceEntry` = 139771;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(1, 104636, 139771, 0, 0, 9, 0, 43521, 0, 0, 0, 0, '', 'loot if quest incomplete'),
(1, 100497, 139771, 0, 0, 9, 0, 43521, 0, 0, 0, 0, '', 'loot if quest incomplete'),
(1, 102672, 139771, 0, 0, 9, 0, 43521, 0, 0, 0, 0, '', 'loot if quest incomplete'),
(1, 102679, 139771, 0, 0, 9, 0, 43521, 0, 0, 0, 0, '', 'loot if quest incomplete'),
(1, 103769, 139771, 0, 0, 9, 0, 43521, 0, 0, 0, 0, '', 'loot if quest incomplete'),
(1, 105393, 139771, 0, 0, 9, 0, 43521, 0, 0, 0, 0, '', 'loot if quest incomplete'),
(1, 106087, 139771, 0, 0, 9, 0, 43521, 0, 0, 0, 0, '', 'loot if quest incomplete');


-- https://forum.wowcircle.com/showthread.php?t=1043318
delete from creature_loot_template where entry in (56747,56541,56719,56884);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(56747, 144127, 11, 1, 0, 1, 1, 0, 0, 0),	
(56747, 144096, 11, 1, 0, 1, 1, 0, 0, 0),	
(56747, 144126, 11, 1, 0, 1, 1, 0, 0, 0),	
(56747, 143978, 11, 1, 0, 1, 1, 0, 0, 0),	
(56747, 143961, 11, 1, 0, 1, 1, 0, 0, 0),

(56719, 144130, 11, 1, 0, 1, 1, 0, 0, 0),	
(56719, 144107, 11, 1, 0, 1, 1, 0, 0, 0),	
(56719, 143985, 11, 1, 0, 1, 1, 0, 0, 0),	
(56719, 144099, 11, 1, 0, 1, 1, 0, 0, 0),	
(56719, 144131, 11, 1, 0, 1, 1, 0, 0, 0);

delete from gameobject_loot_template where entry in (41582,41584);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(41584, 144133, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 143990, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 144132, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 143962, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 143986, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 143981, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 144108, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 143979, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 144109, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 144103, 11, 1, 0, 1, 1, 0, 0, 0),	
(41584, 144215, 11, 1, 0, 1, 1, 0, 0, 0),

(41582, 144129, 11, 1, 0, 1, 1, 0, 0, 0),	
(41582, 144110, 11, 1, 0, 1, 1, 0, 0, 0),	
(41582, 144097, 11, 1, 0, 1, 1, 0, 0, 0),	
(41582, 144106, 11, 1, 0, 1, 1, 0, 0, 0),	
(41582, 144128, 11, 1, 0, 1, 1, 0, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=848042
delete from gossip_menu_option where menu_id = 13163;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES 
(13163, 0, 0, 'Let\'s go!', 1, 1, 0, 0, 0, 0, '', 21649, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1061579
UPDATE `creature_onkill_reputation` SET `RewOnKillRepValue1`='30' WHERE (`creature_id`='69383');


-- https://forum.wowcircle.com/showthread.php?t=1061568
-- https://forum.wowcircle.com/showthread.php?t=1061575
DELETE FROM `creature_questender` WHERE quest in (43502,42839);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(103155, 43502),
(107598, 42839);


-- https://forum.wowcircle.com/showthread.php?t=1061546
DELETE FROM `creature_queststarter` WHERE quest in (45399,45398,45331);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(119539, 45398),
(119539, 45399),
(119539, 45331);

DELETE FROM `creature_questender` WHERE quest in (45399,45398,45331);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(119539, 45398),
(119539, 45399),
(113695, 45331);

update creature_template set minlevel = 110, maxlevel = 110, faction = 14, RegenHealth = 1, lootid = entry where entry in (116798,116783,116779,116827,116828);

update quest_template set PrevQuestId = 45240, ExclusiveGroup = -45398, NextQuestId = 45331 where id in (45399,45398);

delete from creature_loot_template where entry in (116827);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(116827, 143676, 100, 0, 0, 1, 1, 0, 0, 0);

delete from creature where id in (116798,116783,116779,116827,116828);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(116779, 1692, 8529, 8529, 1, 1, 0, 0, 0, 958.065, 154.375, 628.156, 0.21683, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116779, 1692, 8529, 8529, 1, 1, 0, 0, 0, 960.465, 193.759, 628.156, 5.68714, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116779, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1018.51, 184.603, 628.156, 4.61902, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116779, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1029.22, 184.307, 628.156, 4.70934, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116779, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1073.87, 157.475, 628.156, 3.3506, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116779, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1076.77, 145.492, 628.156, 3.72445, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116779, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1002.45, 140.179, 628.156, 0.587568, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116783, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1006.26, 134.169, 628.157, 0.722648, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116783, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1079.14, 164.311, 628.157, 2.93199, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116783, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1035.39, 187.65, 628.157, 4.05746, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116783, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1084.09, 144.446, 628.157, 3.81164, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116783, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1013.42, 190.53, 628.157, 4.61978, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116783, 1692, 8529, 8529, 1, 1, 0, 0, 0, 959.074, 200.835, 628.157, 5.5599, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116783, 1692, 8529, 8529, 1, 1, 0, 0, 0, 953.375, 158.288, 628.184, 0.121015, 120, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1055.43, 115.868, 628.156, 4.76509, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1050.38, 113.404, 628.156, 5.62903, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1000.41, 206.118, 628.157, 4.7549, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 945.373, 183.607, 628.157, 2.48038, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 945.288, 136.726, 628.157, 2.71757, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 991.463, 178.883, 628.156, 5.25205, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 989.199, 171.389, 628.156, 0.963768, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 989.697, 155.668, 628.156, 2.35706, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116798, 1692, 8529, 8529, 1, 1, 0, 0, 0, 976.188, 168.363, 628.157, 5.5937, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116827, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1147.27, 299.304, 627.622, 3.98601, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(116828, 1692, 8529, 8529, 1, 1, 0, 0, 0, 1158.01, 311.439, 627.621, 4.00406, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature set MovementType = 1, spawndist = 8 where id = 116783;

update creature_template set AIName = 'SmartAI' where entry in (116779,116798,119575);

delete from spell_linked_spell where spell_trigger in (237550) and spell_effect in (245823);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(237550, 245823, 0, 0, 0, 0, 0, '');

delete from npc_spellclick_spells where npc_entry in (116779,116798);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(116779, 237550, 1, 0),
(116798, 237550, 1, 0);

delete from smart_scripts where entryorguid in (116779,116798,119575);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(119575, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'warrior - on just summoned - follow'),

(116779, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 42, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on update - set invincibility'),
(116779, 0, 1, 2, 2, 0, 100, 1, 0, 1, 0, 0, 0, 11, 31261, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on 1 hp - cast spell'),
(116779, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 18, 768, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - set unit flag'),
(116779, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - set faction'),
(116779, 0, 4, 5, 61, 0, 100, 1, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - evade'),
(116779, 0, 5, 6, 61, 0, 100, 1, 0, 0, 0, 0, 0, 82, 16777216, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - set npcflag'),
(116779, 0, 6, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 60000, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - despawn'),

(116798, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 42, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on update - set invincibility'),
(116798, 0, 1, 2, 2, 0, 100, 1, 0, 1, 0, 0, 0, 11, 31261, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on 1 hp - cast spell'),
(116798, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 18, 768, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - set unit flag'),
(116798, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 2, 35, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - set faction'),
(116798, 0, 4, 5, 61, 0, 100, 1, 0, 0, 0, 0, 0, 24, 0, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - evade'),
(116798, 0, 5, 6, 61, 0, 100, 1, 0, 0, 0, 0, 0, 82, 16777216, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - set npcflag'),
(116798, 0, 6, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 60000, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - despawn');


-- https://forum.wowcircle.com/showthread.php?t=1061591
DELETE FROM `creature_queststarter` WHERE quest in (40840);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(101513, 40840);

update quest_template set PrevQuestId = 40839 where id = 40840;