-- https://forum.wowcircle.net/showthread.php?t=1064645
delete from creature where guid in (512163,512162);

delete from smart_scripts where entryorguid in (229057);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(229057, 1, 0, 1, 64, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 70859, 50, 0, 0, 0, 0, 0, 'Horde Banner - on click - data set 1 Thrall'),
(229057, 1, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 11, 78288, 50, 0, 0, 0, 0, 0, 'Horde Banner - on click - data set 2 Mague Khadgar');


-- https://forum.wowcircle.net/showthread.php?t=1064653
update creature set phaseMask = 0, phaseID = 32108 where id in (67639,67330,67357,67767,67939,67359,68198,66801,67765,67444,68166,67976,
67386,67988) and  zoneId = 6134;
-- update creature set phaseMask = 0, phaseID = 32108 where id in ();

delete from spell_target_position where id = 134083;
INSERT INTO `spell_target_position` (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`, `VerifiedBuild`) VALUES 
(134083, 2, 870, -2122.9768, 2523.7949, 4.4055, 5.83, 1);

delete from spell_linked_spell where spell_trigger in (-135954) and spell_effect in (134083);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(-135954, 134083, 0, 0, 0, 0, 0, '');

delete from phase_definitions where zoneid in (6134);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
(6134, 32108, 0, 32108, 1061, 0, 0, 'Terrain Swap Phase Horde'),
(6134, 32109, 0, 32108, 1062, 0, 0, 'Terrain Swap Phase Aliance'),

(6134, 321080, 0, 321080, 0, 0, 0, 'before terrain swap');

delete from conditions where SourceGroup in (6134) and SourceTypeOrReferenceId = 26 and sourceentry in (1,2,32109,32108,321080);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6134, 32108, 0, 0, 8, 0, 32108, 0, 0, 0, 0, '', 'phase if quest 32108 reward'),
(26, 6134, 32108, 0, 1, 8, 0, 32109, 0, 0, 0, 0, '', 'phase if quest 32109 reward'),

(26, 6134, 32109, 0, 0, 8, 0, 32108, 0, 0, 0, 0, '', 'phase if quest 32108 reward'),
(26, 6134, 32109, 0, 1, 8, 0, 32109, 0, 0, 0, 0, '', 'phase if quest 32109 reward'),

(26, 6134, 321080, 0, 0, 8, 0, 32108, 0, 0, 1, 0, '', 'phase if quest 32108 not reward'),
(26, 6134, 321080, 0, 0, 8, 0, 32109, 0, 0, 1, 0, '', 'phase if quest 32109 not reward');


-- https://forum.wowcircle.net/showthread.php?t=1064695
update creature_template set unit_flags = 768 where entry in (95261,95403);

update creature_template set flags_extra = 128 where entry in (95261);

delete from gossip_menu_option where menu_id in (95403);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(95403, 0, 0, 'Take the Windmaster\'s Totem.', 1, 1, 111851);

delete from gossip_menu where entry in (95403);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(95403, 95403);

delete from npc_text where id in (95403);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(95403, 100320, 100320);


-- https://forum.wowcircle.net/showthread.php?t=1062302
update creature_template set VehicleId = 5236, npcflag = 16777216 where entry = 116828;

delete from vehicle_template_accessory where entryoraura in (116828);
INSERT INTO `vehicle_template_accessory` (`EntryOrAura`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `description`) VALUES 
(116828, 116827, 0, 0, 7, 0, '');

delete from creature where id = 116827;

delete from npc_spellclick_spells where npc_entry in (116828);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(116828, 46598, 1, 0);

delete from creature_model_info where DisplayID = 74338;
INSERT INTO `creature_model_info` (`DisplayID`, `BoundingRadius`, `CombatReach`, `DisplayID_Other_Gender`, `VerifiedBuild`) VALUES 
(74338, 3, 3, 0, 0);


-- https://forum.wowcircle.net/showthread.php?t=1064701
delete from smart_scripts where entryorguid in (90417);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(90417, 0, 0, 0, 19, 0, 100, 0, 0, 39986, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'talk'),
(90417, 0, 1, 0, 19, 0, 100, 0, 0, 39987, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'talk'),
(90417, 0, 2, 0, 19, 0, 100, 0, 0, 39987, 0, 0, 0, 0, 11, 135030, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Arcane Cast Visual'),
(90417, 0, 3, 0, 10, 0, 100, 0, 0, 1, 20, 15000, 15000, 0, 1, 2, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'talk'),
(90417, 0, 4, 0, 62, 0, 100, 0, 0, 904170, 0, 0, 0, 0, 56, 136723, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Add item'),
(90417, 0, 5, 0, 62, 0, 100, 0, 0, 904170, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Close gossip'),
(90417, 0, 6, 0, 62, 0, 100, 0, 0, 904170, 0, 0, 0, 0, 1, 3, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'talk'),
(90417, 0, 7, 0, 52, 0, 100, 0, 0, 3, 90417, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 90418, 10, 0, 0, 0, 0, 0, 'Set data 1 on Modera'),
(90417, 0, 8, 0, 38, 0, 100, 0, 0, 0, 1, 100, 100, 0, 1, 4, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'talk'),
(90417, 0, 9, 0, 52, 0, 100, 0, 0, 4, 90417, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 11, 90418, 10, 0, 0, 0, 0, 0, 'Set data 2 on Modera'),
(90417, 0, 20, 0, 64, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 97655, 0, 0, 0, 0, 0, 18, 10, 0, 0, 0, 0, 0, 0, 'Quest_credit'),
(90417, 0, 30, 31, 62, 0, 100, 0, 0, 904170, 10, 0, 0, 0, 80, 9041700, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Gossip select - Actionlist - Self'),
(90417, 0, 31, 32, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 85, 209886, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Gossip select - Invoker cast - Player'),
(90417, 0, 32, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Gossip select - Close gossip  - Player'),

(90417, 0, 33, 0, 19, 0, 100, 0, 0, 41804, 0, 0, 0, 0, 12, 90417, 1, 120000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on quest accept - summon creature'),
(90417, 0, 34, 35, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 83, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on just summoned - remove npcflag'),
(90417, 0, 35, 36, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 1, 90417, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on link - start waypoint'),
(90417, 0, 36, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on link - talk'),
(90417, 0, 37, 38, 40, 0, 100, 1, 0, 7, 90417, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.25, 'khadgar - on waypoint reached - set orientation'),
(90417, 0, 38, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 31, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on link - talk'),
(90417, 0, 39, 0, 52, 0, 100, 1, 0, 31, 90417, 0, 0, 0, 1, 32, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on text over - talk'),
(90417, 0, 40, 41, 52, 0, 100, 1, 0, 32, 90417, 0, 0, 0, 1, 33, 8000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on text over - talk'),
(90417, 0, 41, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, -829.1351, 4654.6997, 767.6705, 0, 'khadgar - on link - move'),
(90417, 0, 42, 47, 52, 0, 100, 1, 0, 33, 90417, 0, 0, 0, 1, 34, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on text over - talk'),
(90417, 0, 43, 0, 52, 0, 100, 1, 0, 34, 90417, 0, 0, 0, 1, 35, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on text over - talk'),
(90417, 0, 44, 45, 52, 0, 100, 1, 0, 35, 90417, 0, 0, 0, 1, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on text over - talk'),
(90417, 0, 45, 46, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 33, 90417, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'khadgar - on link - give credit'),
(90417, 0, 46, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'khadgar - on link - despawn'),
(90417, 0, 47, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 4.39, 'khadgar - on link - set orientation');

delete from creature_text where Entry in (90417) and GroupID >= 30;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(90417, 30, 0, 'I have just the thing. Follow me.', 12, 0, 100, 0, 0, 0, 107388),
(90417, 31, 0, 'Hold on just a moment. I fear I\'m a bit of a pack rat.', 12, 0, 100, 0, 0, 0, 107389),
(90417, 32, 0, 'Now let\'s see... apexis crystals? No, no, those won\'t do at all.', 12, 0, 100, 0, 0, 0, 107390),
(90417, 33, 0, 'Hmm, I must\'ve put it over here.', 12, 0, 100, 0, 0, 0, 107391),
(90417, 34, 0, 'Arcane powder... soul shards... that\'s definitely not it...', 12, 0, 100, 0, 0, 0, 107392),
(90417, 35, 0, 'A-ha! Here we are... a crystallized soul. That ought to do the trick!', 12, 0, 100, 0, 0, 0, 107393),
(90417, 36, 0, 'Would you mind grabbing it? These crystals tend to have an adverse effect on non-demonic beings.', 12, 0, 100, 0, 0, 0, 107398);

delete from gameobject_template where entry in (248521);
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `data32`, `RequiredLevel`, `WorldEffectID`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES 
(248521, 3, 32954, 'Кристаллизованная душа', '', '', '', 0, 0, 1, 136385, 0, 0, 0, 0, 0, 43, 248521, 0, 0, 0, 0, 0, 0, 41804, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 1);

delete from waypoints where entry in (90417);
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`) VALUES 
(90417, 1, -848.774, 4648.38, 754.721),
(90417, 2, -848.112, 4658.01, 761.440),
(90417, 3, -846.992, 4664.16, 766.067),
(90417, 4, -844.415, 4666.94, 767.584),
(90417, 5, -838.018, 4666.94, 767.662),
(90417, 6, -832.795, 4663.91, 767.741),
(90417, 7, -833.489, 4661.06, 767.688);

delete from gameobject_loot_template where entry in (248521);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(248521, 136385, -100, 1, 0, 1, 1, 0, 0, 0);

delete from gameobject where id = 248521;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(248521, 1220, 7502, 7502, 1, 0, 41804, -829.285, 4652.99, 768.774, 4.44178, 0, 0, 0.796026, -0.605262, 30, 0, 1, 0, '', 0, -1);

delete from phase_definitions where zoneid in (7502) and entry = 41804;
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
(7502, 41804, 0, 41804, 0, 0, 0, '41804 quest');

delete from conditions where SourceGroup in (7502) and SourceTypeOrReferenceId = 26 and sourceentry in (41804);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7502, 41804, 0, 0, 9, 0, 41804, 0, 0, 0, 0, '', 'phase if quest 41804 incomplete'),
(26, 7502, 41804, 0, 0, 48, 0, 282862, 0, 0, 0, 0, '', 'phase if 282862 criteria complete'),
(26, 7502, 41804, 0, 0, 28, 0, 41804, 0, 0, 1, 0, '', 'phase if quest 41804 not complete');

delete from quest_template_objective where QuestID = 41804;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(282861, 41804, 1, 0, 1, 136385, 1, 2, 0, 0, '', '4696', 21992),
(282862, 41804, 0, 0, 0, 90417, 1, 0, 0, 0, 'Follow Archmage Khadgar', '4691', 21992);


-- https://forum.wowcircle.net/showthread.php?t=1064780
DELETE FROM `creature_questender` WHERE quest in (32284);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(67866, 32284);

delete from quest_start_scripts where id = 32284;

update quest_template set StartScript = 0 where id = 32284;

delete from creature where guid = 263294;

delete from creature_template_addon where entry in (68023);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(68023, 0, 0, 0, '114305');

delete from creature where id in (67866,68023);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(67866, 870, 5841, 6059, 1, 1, 0, 0, 0, 1728.66, 354.261, 482.219, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(68023, 870, 5841, 6059, 1, 1, 0, 0, 0, 1729.27, 356.891, 482.445, 6.06149, 300, 0, 0, 831413632, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set AIName = 'SmartAI' where entry in (67866);

delete from smart_scripts where entryorguid in (67866);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(67866, 0, 0, 0, 10, 0, 100, 0, 0, 1, 10, 0, 0, 0, 33, 68720, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'chen - on ooc los - give credit');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` in (67866);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 67866, 0, 0, 9, 0, 32284, 0, 0, 0, 0, '', 'smart if quest incomplete');


-- https://forum.wowcircle.net/showthread.php?t=1064804
delete from phase_definitions where zoneid in (7637) and entry in (42836,42833);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
(7637, 42833, 0, 42833, 0, 0, 0, '42833 quest'),
(7637, 42836, 0, 42836, 0, 0, 0, '42836 quest');

delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and sourceentry in (42833,42836);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7637, 42833, 0, 0, 14, 0, 42833, 0, 0, 1, 0, '', 'phase if quest 42833 not none'),
(26, 7637, 42833, 0, 0, 28, 0, 42836, 0, 0, 1, 0, '', 'phase if quest 42836 not complete'),
(26, 7637, 42833, 0, 0, 8, 0, 42836, 0, 0, 1, 0, '', 'phase if quest 42836 not reward'),

(26, 7637, 42836, 0, 0, 28, 0, 42836, 0, 0, 0, 0, '', 'phase if quest 42836 complete'),
(26, 7637, 42836, 0, 1, 8, 0, 42836, 0, 0, 0, 0, '', 'phase if quest 42836 reward');

update creature set phaseMask = 0, phaseID = 42833 where guid = 11843126;
update creature set phaseMask = 0, phaseID = 42836 where guid = 11843129;


-- https://forum.wowcircle.net/showthread.php?t=1064805
update creature_template set AIName = 'SmartAI' where entry = 108870;

delete from smart_scripts where entryorguid in (108870) and id in (1,2);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(108870, 0, 1, 2, 19, 0, 100, 0, 0, 42838, 0, 0, 0, 0, 1004, 1746, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'sylverin - on quest accept - play scene'),
(108870, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 108872, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'sylverin - on link - give credit');


-- https://forum.wowcircle.net/showthread.php?t=1064807
update quest_template set PrevQuestId = 42834 where id = 42835;
update quest_template set PrevQuestId = 42835 where id = 42836;
update quest_template set PrevQuestId = 42836 where id = 42838;
update quest_template set PrevQuestId = 42838 where id = 44084;