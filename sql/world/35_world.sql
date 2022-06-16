-- https://forum.wowcircle.net/showthread.php?t=803403
update creature_template set vehicleid = 752, ainame = 'SmartAI', speed_fly = 2, 
inhabittype = 4, spell1 = 75560, spell2 = 73257, spell6 = 75991, npcflag = 0 where entry in (40505);
update creature_template set vehicleid = 781, ainame = 'SmartAI', inhabittype = 7 where entry in (40791);

update creature_template set ainame = 'SmartAI' where entry in (38571);
update creature set movementtype = 0, spawndist = 0 where id = 38571;

update creature_template set inhabittype = 4, vehicleid = 762, ainame = 'SmartAI', speed_walk = 5, speed_run = 5, speed_fly = 5 where entry in (40603);

update creature_template set npcflag = 16777216, unit_flags = 768 where entry in (40604);

delete from vehicle_template_accessory where EntryOrAura = 40505;
-- INSERT INTO `vehicle_template_accessory` (`EntryOrAura`, `accessory_entry`, `seat_id`, `offsetX`, `offsetY`, `offsetZ`, `offsetO`, `minion`, `description`, `summontype`, `summontimer`) VALUES 
-- (40505, 40511, 0, 1, -0.3, 0.8, 0, 0, '40505 - 40511', 6, 30000),
-- (40505, 38534, 1, -1, 0, -0.25, 0, 0, '40505 - 38534', 6, 30000);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` in (40505,40604,40603);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(40604, 75737, 1, 0),

(40505, 82842, 1, 0),
(40603, 46598, 1, 0);

delete from spell_target_position where id in (75737);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(75737, 1, -7090.1099, -3909.7900, 12.6611, 4.55);

delete from smart_scripts where entryorguid in (40505,40603,38571);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(40505, 0, 0, 0, 60, 0, 100, 0, 2000, 2000, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, -7090.11, -3909.79, 68.8996, 0, 'Baloon - on update - move to position'),
(40505, 0, 1, 0, 34, 0, 100, 0, 8, 1, 0, 0, 53, 1, 40505, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Baloon - point reached - start waypoint'),
(40505, 0, 2, 0, 40, 0, 100, 0, 40505, 8, 0, 0, 11, 75990, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Baloon - on waypoint reahced - cast spell'),
(40505, 0, 3, 0, 8, 0, 100, 1, 75991, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'Baloon - on spell hit - remove aura'),
-- (40505, 0, 4, 0, 28, 0, 100, 1, 0, 0, 0, 0, 85, 75730, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 'Baloon - on passenger removed - invoker cast spell'), -- падает сервер

(38571, 0, 0, 1, 8, 0, 100, 1, 72337, 0, 0, 0, 33, 38576, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Survivor - on spell hit - give credit'),
(38571, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Survivor - on link - despawn'),

(40603, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Rocket Pack - on just summoned - invoker cast'),
(40603, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, -7122.2021, -3904.9353, 73.7418, 0, 'Rocket Pack - on link - move to position'),
(40603, 0, 2, 3, 34, 0, 100, 1, 8, 1, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Rocket Pack - on point reached - remove aura'),
(40603, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 11, 75752, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Rocket Pack - on link - cast spell'),
(40603, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Rocket Pack - on link - despawn');

delete from waypoints where entry = 40505;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(40505, 1 , -7074.52, -4001.33, 70.5547),
(40505, 2 , -7063.33, -4059.16, 73.3285),
(40505, 3 , -7049.3, -4113.18, 77.3021),
(40505, 4 , -7023.96, -4194.47, 68.9969),
(40505, 5 , -7003.3, -4277.97, 62.9446),
(40505, 6 , -6985.05, -4368.78, 60.6358),
(40505, 7 , -6983.53, -4380.61, 60.9069),
(40505, 8 , -6970.98, -4445.99, 62.715), --
(40505, 9 , -6953.71, -4490.8, 64.4024),
(40505, 10, -6915.62, -4565.13, 66.7366),
(40505, 11, -6882.7, -4628.55, 67.6281),
(40505, 12, -6847.21, -4683.34, 67.3021),
(40505, 13, -6816.85, -4721.02, 66.6172),
(40505, 14, -6783.49, -4734.47, 63.4344),
(40505, 15, -6754.71, -4786.4, 60.5157),
(40505, 16, -6755.82, -4818.73, 62.3019),
(40505, 17, -6773.98, -4862.79, 65.6738),
(40505, 18, -6809.99, -4884.5, 66.8514),
(40505, 19, -6861.69, -4901.58, 64.6157),
(40505, 20, -6965.7, -4835.67, 61.7258),
(40505, 21, -7005.72, -4784.53, 61.3201),
(40505, 22, -7049.59, -4718.37, 61.6756),
(40505, 23, -7074.97, -4667.63, 65.5057),
(40505, 24, -7100.1, -4600.62, 65.8196),
(40505, 25, -7122.7, -4512.27, 66.0807),
(40505, 26, -7133.78, -4444.55, 67.9388),
(40505, 27, -7146.08, -4318.87, 72.8177),
(40505, 28, -7143.35, -4203.66, 74.2078),
(40505, 29, -7135.53, -4129.07, 72.5886),
(40505, 30, -7129.99, -4034.36, 49.8257),
(40505, 31, -7132.58, -3978.56, 6.90624);


-- https://forum.wowcircle.net/showthread.php?t=804002
update gameobject_template set flags = 0 where entry in (250373,250372,250374);


-- https://forum.wowcircle.net/showthread.php?t=804308
update creature_template set ainame = 'SmartAI', gossip_menu_id = 18723 where entry in (96813);
update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry in (110343);
update creature_template set ainame = 'SmartAI' where entry in (112477);

delete from gossip_menu_option where menu_id in (18723);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(18723, 0, 2, 'I want to travel fast', 4, 8192, 3409),
(18723, 1, 0, 'I\'am ready to go to Suramar.', 1, 1, 121011);

delete from gossip_menu where entry in (18723);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(18723, 12487);

delete from npc_text where id in (12487);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(12487, 98620, 98620);

delete from conditions where SourceGroup in (18723) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 18723, 1, 0, 0, 9, 0, 44009, 0, 0, 0, 0, '', 'Show Gossip If Player Has 44009 quest'),
(15, 18723, 1, 0, 1, 9, 0, 44257, 0, 0, 0, 0, '', 'Show Gossip If Player Has 44257 quest');

delete from smart_scripts where entryorguid in (96813) and id > 0;
delete from smart_scripts where entryorguid in (110343,112477);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(112477, 0, 0, 0, 60, 0, 100, 0, 0, 0, 5000, 5000, 33, 112493, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Yorg - on update - give credit'),
(112477, 0, 1, 0, 0, 0, 100, 0, 0, 5000, 0, 0, 11, 182706, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Yorg - ic - cast spell'),
(112477, 0, 2, 0, 0, 0, 100, 0, 30000, 30000, 60000, 60000, 11, 182667, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Yorg - ic - cast spell'),

(110343, 0, 0, 0, 10, 0, 100, 0, 1, 20, 5000, 5000, 33, 112404, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc los - give credit'),
(110343, 0, 1, 0, 10, 0, 100, 0, 1, 20, 5000, 5000, 33, 112404, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc los - give credit'),

(96813, 0, 1, 2, 62, 0, 100, 0, 18723, 1, 0, 0, 33, 112309, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Aludan - on gossip select - give credit'),
(96813, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 62, 1220, 0, 0, 0, 0, 0, 7, 0, 0, 0, 1133.5093, 1739.2412, 0.8936, 5.19, 'Aludan - on link - teleport nvoker');

delete from conditions where SourceEntry in (110343) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 110343, 0, 0, 48, 0, 286138, 0, 0, 1, 0, '', 'Smart if player not complete quest criteria'),
(22, 1, 110343, 0, 0, 9, 0, 44009, 0, 0, 0, 0, '', 'Smart if player not complete quest criteria'),
(22, 2, 110343, 0, 0, 48, 0, 286456, 0, 0, 1, 0, '', 'Smart if player not complete quest criteria'),
(22, 2, 110343, 0, 0, 9, 0, 44257, 0, 0, 0, 0, '', 'Smart if player not complete quest criteria');

update creature_template_wdb set killcredit1 = 0 where killcredit1 in (112404,112493);
update creature_template_wdb set killcredit2 = 0 where killcredit2 in (112404,112493);

delete from creature_template_addon where entry in (110343);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(110343, 0, 0, 0, '223638');


-- https://forum.wowcircle.net/showthread.php?t=804852
delete from creature where id in (19291,19292);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(19291, 530, 3483, 3483, 1, 1, 0, 0, 0, -145.554, 1511.28, 34.3641, 5.23599, 300, 0, 0, 4979, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(19292, 530, 3483, 3483, 1, 1, 0, 0, 0, -304.408, 1524.45, 37.9685, 2.49582, 300, 0, 0, 4979, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set ainame ='SmartAI', flags_extra = 128 where entry in (19291,19292);

delete from smart_scripts where entryorguid in (19291,19292);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(19291, 0, 0, 1, 8, 0, 100, 0, 33655, 0, 0, 0, 33, 19291, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(19291, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 50, 182090, 60, 0, 0, 0, 0, 8, 0, 0, 0, -157.287, 1519.64, 37.2855, 0, 'Trigger - on link - summon go'),
(19291, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 50, 182090, 60, 0, 0, 0, 0, 8, 0, 0, 0, -151.894, 1507.88, 63.9751, 0, 'Trigger - on link - summon go'),
(19291, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 50, 182090, 60, 0, 0, 0, 0, 8, 0, 0, 0, -151.036, 1519.64, 48.0284, 0, 'Trigger - on link - summon go'),
(19291, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 50, 182090, 60, 0, 0, 0, 0, 8, 0, 0, 0, -136.132, 1517.10, 47.6863, 0, 'Trigger - on link - summon go'),

(19292, 0, 0, 1, 8, 0, 100, 0, 33655, 0, 0, 0, 33, 19292, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(19292, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 50, 183816, 60, 0, 0, 0, 0, 8, 0, 0, 0, -319.317, 1530.54, 42.2322, 0, 'Trigger - on link - summon go'),
(19292, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 50, 183816, 60, 0, 0, 0, 0, 8, 0, 0, 0, -311.413, 1532.04, 51.9609, 0, 'Trigger - on link - summon go'),
(19292, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 50, 183816, 60, 0, 0, 0, 0, 8, 0, 0, 0, -293.8773, 1515.0649, 51.0154, 0, 'Trigger - on link - summon go'),
(19292, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 50, 183816, 60, 0, 0, 0, 0, 8, 0, 0, 0, -307.1225, 1512.0863, 39.6738, 0, 'Trigger - on link - summon go');

delete from gameobject where id = 183816 and areaid = 3483;
delete from gameobject where id = 182090 and areaid = 3483;


-- https://forum.wowcircle.net/showthread.php?t=804965
update creature_template set ainame = 'SmartAI', inhabittype = 4, speed_fly = 3, speed_run = 3, speed_walk = 3, vehicleid = 527, spell1 = 109322 where entry in (57927);
update creature_template set npcflag = 16777216 where entry in (57871);
update creature set movementtype = 0, spawndist = 0 where id in (57871,57872);
update creature_template set ainame = 'SmartAI', unit_flags = 768, flags_extra = 128 where entry = 57872;

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` in (57871);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(57871, 109469, 1, 0);

delete from smart_scripts where entryorguid in (57872,57927);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(57927, 0, 0, 0, 54, 0, 100, 1, 109322, 0, 0, 0, 11, 109476, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Serpent - on just summoned - cast spell'),

(57872, 0, 0, 1, 8, 0, 100, 1, 109322, 0, 0, 0, 33, 57872, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(57872, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - despawn');

delete from creature_template_addon where entry in (57872);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(57872, 0, 0, 0, '130773');

delete from spell_target_position where id in (109469);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(109469, 870, 817.516, -2327.31, 135.061, 4.65665);




-- ======================================
-- https://forum.wowcircle.net/showthread.php?t=1027553
-- Добавлен лут НИП Ордос https://ru.wowhead.com/npc=72057
-- ======================================
UPDATE creature_template SET lootid = 72057, dmg_multiplier = 10 WHERE entry = 72057;
UPDATE creature_loot_template SET item = 72057, lootmode = 1, groupid = 0, maxcount = 1 WHERE entry = 72057;
UPDATE reference_loot_template SET lootmode = 1, groupid = 1 WHERE entry = 72057;

-- ======================================
-- https://forum.wowcircle.net/showthread.php?t=964302
-- Исправлены статы НИП Лиана Отчаяние Тьмы https://ru.wowhead.com/npc=98801
-- ======================================
UPDATE creature_template SET minlevel = 110, maxlevel = 110, speed_fly = 1.14286, dmg_multiplier = 1 WHERE entry = 99514;
UPDATE creature SET curhealth = 10392670 WHERE guid = 18384692;

-- ============================================
-- https://forum.wowcircle.net/showthread.php?t=1041089
-- Исправлен лут у НИП Сквернодрев https://ru.wowhead.com/npc=80204
-- ============================================
DELETE FROM creature_loot_template WHERE entry = 80204;
INSERT INTO creature_loot_template (entry,item,ChanceOrQuestChance,lootmode,groupid,mincountOrRef,maxcount,requiredAura,requiredClass,difficultyMask) VALUES
(80204,112373,100,1,0,1,1,0,0,0),
(80204,118798,10,1,0,1,1,0,0,0);
UPDATE creature_template SET lootid = 80204 WHERE entry = 80204;


-- =================================
-- https://forum.wowcircle.net/showthread.php?t=870494
-- Исправлен лут Потрепанная кукла https://ru.wowhead.com/item=130214 с Безумный Хенрик https://ru.wowhead.com/npc=95221
-- =================================
UPDATE creature_loot_template SET ChanceOrQuestChance = 100 WHERE entry = 95221 AND item = 130214;

-- =================================
-- https://forum.wowcircle.net/showthread.php?t=867376
-- Исправлен количество Блестящий оберег для питомца https://ru.wowhead.com/item=116415 с Горка товаров для питомцев https://ru.wowhead.com/item=144345
-- =================================
UPDATE item_loot_template SET mincountOrRef = 6, maxcount = 28 WHERE entry = 144345 AND item = 116415;

-- ===============================
-- https://forum.wowcircle.net/showthread.php?t=832846
-- теперь итем Безукоризненный сувенир из осколка Ночного Колодца https://ru.wowhead.com/item=140200 только при взятом квесте.
-- ===============================
UPDATE creature_loot_template SET ChanceOrQuestChance = -10 WHERE item = 140200 AND ChanceOrQuestChance = 10;

-- ======================================
-- https://forum.wowcircle.net/showthread.php?t=825389&p=7553866#post7553866
-- Исправлен лут BoE вещей с трэша рейда Гробница Саргераса
-- ======================================
UPDATE reference_loot_template SET lootmode = 1, groupid = 1 WHERE entry = 1171540;
-- --
UPDATE creature_template set lootid = entry WHERE entry IN (121011,117123,117154,120473,121004,121399,120516,120697,120720,120182,120719,117776,117782,120191,120449,120201,120158,120194,120463,120482,121790,117769,120153);
-- --
DELETE FROM creature_loot_template WHERE mincountOrRef = -1171540;
INSERT INTO `creature_loot_template` (`entry`,`item`,`ChanceOrQuestChance`,`lootmode`,`groupid`,`mincountOrRef`,`maxcount`,`requiredAura`,`requiredClass`,`difficultyMask`) VALUES 
(121011,1,0.5,1,0,-1171540,1,0,0,0),
(117123,1,0.5,1,0,-1171540,1,0,0,0),
(117154,1,0.5,1,0,-1171540,1,0,0,0),
(120473,1,0.5,1,0,-1171540,1,0,0,0),
(121004,1,0.5,1,0,-1171540,1,0,0,0),
(121399,1,0.5,1,0,-1171540,1,0,0,0),
(120516,1,0.5,1,0,-1171540,1,0,0,0),
(120697,1,0.5,1,0,-1171540,1,0,0,0),
(120720,1,0.5,1,0,-1171540,1,0,0,0),
(120182,1,0.5,1,0,-1171540,1,0,0,0),
(120719,1,0.5,1,0,-1171540,1,0,0,0),
(117776,1,0.5,1,0,-1171540,1,0,0,0),
(117782,1,0.5,1,0,-1171540,1,0,0,0),
(120191,1,0.5,1,0,-1171540,1,0,0,0),
(120449,1,0.5,1,0,-1171540,1,0,0,0),
(120201,1,0.5,1,0,-1171540,1,0,0,0),
(120158,1,0.5,1,0,-1171540,1,0,0,0),
(120194,1,0.5,1,0,-1171540,1,0,0,0),
(120463,1,0.5,1,0,-1171540,1,0,0,0),
(120482,1,0.5,1,0,-1171540,1,0,0,0),
(121790,1,0.5,1,0,-1171540,1,0,0,0),
(117769,1,0.5,1,0,-1171540,1,0,0,0),
(120153,1,0.5,1,0,-1171540,1,0,0,0);
-- --
DELETE FROM creature_loot_template WHERE item IN (select item FROM reference_loot_template where entry = 1171540) AND entry = 120158;
