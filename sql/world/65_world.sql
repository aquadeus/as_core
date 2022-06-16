-- https://forum.wowcircle.com/showthread.php?t=1050159
update quest_template set flags = 3211264 where id = 26646;


-- https://forum.wowcircle.com/showthread.php?t=1050160
update creature_template set ainame = 'SmartAI' where entry = 43611;

delete from smart_scripts where entryorguid in (43611);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(43611, 0, 0, 0, 19, 0, 100, 0, 26668, 0, 0, 0, 0, 15, 26668, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Jhon - on quest accept - complete quest');


-- https://forum.wowcircle.com/showthread.php?t=1050166
update quest_template set flags = 3211264 where id = 26693;


-- https://forum.wowcircle.com/showthread.php?t=1050199
delete from creature where id = 43733 and areaid = 5325;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(43733, 0, 44, 5325, 1, 1, 0, 0, 0, -9137.14, -3048.09, 108.722, 2.27091, 300, 0, 0, 4161, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1050204
delete from creature where id = 75959 and areaid = 7133;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(75959, 1116, 6662, 7133, 1, 1, 0, 0, 0, 2800.6860, 2518.2744, 121.0418, 0.58, 300, 0, 0, 0, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1050324
update gameobject set id = 187458, spawntimesecs = -300 where id = 300170;

delete from creature where id in (25308,25309,25310);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(25308, 571, 3537, 4025, 1, 1, 0, 0, 0, 3797.10, 6042.27, -134.122, 5.48696, 300, 0, 0, 0, 0, 0),
(25310, 571, 3537, 4025, 1, 1, 0, 0, 0, 3277.52, 6594.09, -0.0150133, 0.706, 300, 0, 0, 0, 0, 0),
(25309, 571, 3537, 4033, 1, 1, 0, 0, 0, 4133.5, 6261.69, 28.8527, 2.53936, 300, 0, 0, 0, 0, 0);

delete from conditions where SourceEntry in (45414) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 45414, 0, 0, 31, 0, 3, 25308, 0, 0, 0, '', 'Cast Spell Only 25308 Creature'),
(13, 1, 45414, 0, 1, 31, 0, 3, 25310, 0, 0, 0, '', 'Cast Spell Only 25310 Creature'),
(13, 1, 45414, 0, 2, 31, 0, 3, 25309, 0, 0, 0, '', 'Cast Spell Only 25309 Creature');

update creature_template set unit_flags = 768, flags_extra = 128 where entry in (25308,25309,25310);


-- https://forum.wowcircle.com/showthread.php?t=1050369
update creature_template set npcflag = 16777216, ainame = 'SmartAI' where entry in (102029,102031,102030);
update creature_template set ainame = 'SmartAI' where entry in (103347, 103348, 111539);

delete from npc_spellclick_spells where npc_entry in (102029,102031,102030);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(102029, 210226, 1, 0),
(102031, 210321, 1, 0),
(102030, 210322, 1, 0);

delete from creature where id in (103347, 103348, 111539);-- 102029 102031 102030

delete from smart_scripts where entryorguid in (102029,102031,102030, 103347, 103348, 111539);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(102029, 0, 0, 0, 8, 0, 100, 0, 210226, 0, 0, 0, 0, 12, 103347, 1, 10000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on link - summon creature'),
(102031, 0, 0, 0, 8, 0, 100, 0, 210321, 0, 0, 0, 0, 12, 103348, 1, 10000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on link - summon creature'),
(102030, 0, 0, 0, 8, 0, 100, 0, 210322, 0, 0, 0, 0, 12, 111539, 1, 10000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on link - summon creature'),

(103347, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on just summoned - talk'),
(103348, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on just summoned - talk'),
(111539, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on just summoned - talk'),

(103347, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 11, 25824, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on just summoned - cast spell'),
(103348, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 11, 25824, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on just summoned - cast spell'),
(111539, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 11, 25824, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Moonguard - on just summoned - cast spell');

delete from creature_text where entry in (103347, 103348, 111539);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`, `comment`) VALUES 
(103347, 0, 0, 'Всех не защитить...', 12, 0, 100, 0, 0, 105978, ''),
(103348, 0, 0, 'Стража больше нет.', 12, 0, 100, 0, 0, 105979, ''),
(111539, 0, 0, 'Я отправлюсь к моим павшим братьям и сестрам. Прощай, Лирея.', 12, 0, 100, 0, 0, 120114, '');

delete from conditions where SourceGroup in (102029,102031,102030) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 102029, 210226, 0, 0, 9, 0, 40883, 0, 0, 0, 0, '', 'Allow Spellclick if player has quest'),
(18, 102031, 210321, 0, 0, 9, 0, 40883, 0, 0, 0, 0, '', 'Allow Spellclick if player has quest'),
(18, 102030, 210322, 0, 0, 9, 0, 40883, 0, 0, 0, 0, '', 'Allow Spellclick if player has quest');


--
delete from quest_template_objective where objectid = 133957 and questid = 40968;
update quest_template_objective set `index` = 0 where objectid = 133956 and questid = 40968;

delete from creature_loot_template where item in (133957);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(111527, 133957, 100, 0, 0, 1, 1),
(111530, 133957, 100, 0, 0, 1, 1),
(102242, 133957, 100, 0, 0, 1, 1),
(102969, 133957, 100, 0, 0, 1, 1),
(101784, 133957, 100, 0, 0, 1, 1),
(101783, 133957, 100, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1050663
delete from creature where id = 315;

update creature_template set ainame = 'SmartAI', unit_flags = 768, minlevel = 60, maxlevel = 60 where entry in (315,43797);
update creature_template set faction = 14 where entry in (315);
update creature_template set faction = 35, dmg_multiplier = 5 where entry in (43797);

delete from event_scripts where id in (25759);
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(25759, 0, 10, 315  , 300000, 0, -10371.3984, -1252.2552, 35.9100, 5.43),
(25759, 0, 10, 43797, 300000, 0, -10361.3594, -1256.8706, 35.8572, 3.15);

delete from smart_scripts where entryorguid in (315, 43797, 31500);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(43797, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, -10366.5430, -1256.2069, 35.9098, 0, 'Tobias - on just summoned - move to position'),
(43797, 0, 1, 2, 38, 0, 100, 1, 0, 1, 0, 0, 0, 19, 768, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Tobias - on data set - remove unit_flags'),
(43797, 0, 2, 0, 61, 0, 100, 1, 0, 1, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 315, 0, 0, 0, 0, 0, 0, 'Tobias - on link - start attack'),
(43797, 0, 3, 0, 38, 0, 100, 1, 0, 2, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Tobias - on data set - despawn'),

(315, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, -10369.6074, -1253.9539, 35.9100, 0, 'Stalvan - on just summoned - move to position'),
(315, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 80, 31500, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Stalvan - on link - run timed action list'),
(315, 0, 2, 3, 6, 0, 100, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Stalvan - on death - talk'),
(315, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 19, 43797, 0, 0, 0, 0, 0, 0, 'Stalvan - on link - set data'),

(31500, 9, 0 , 0, 0, 0, 100, 1, 1500, 1500, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 1 , 0, 0, 0, 100, 1, 4000, 4000, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 43797, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 2 , 0, 0, 0, 100, 1, 2000, 2000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 3 , 0, 0, 0, 100, 1, 2000, 2000, 0, 0, 0, 84, 1, 0, 0, 0, 0, 0, 19, 43797, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 4 , 0, 0, 0, 100, 1, 4000, 4000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 5 , 0, 0, 0, 100, 1, 4000, 4000, 0, 0, 0, 84, 2, 0, 0, 0, 0, 0, 19, 43797, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 6 , 0, 0, 0, 100, 1, 4000, 4000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 7 , 0, 0, 0, 100, 1, 4000, 4000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 8 , 0, 0, 0, 100, 1, 4000, 4000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 9 , 0, 0, 0, 100, 1, 4000, 4000, 0, 0, 0, 84, 3, 0, 0, 0, 0, 0, 19, 43797, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(31500, 9, 10, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 19, 768, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - remove unit_flags'),
(31500, 9, 11, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 43797, 0, 0, 0, 0, 0, 0, 'Timed action list - set data'),
(31500, 9, 12, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 43797, 0, 0, 0, 0, 0, 0, 'Timed action list - start attack');

delete from gameobject where id = 204811;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(204811, 0, 10, 1098, 1, 1, 0, -10369.2480, -1254.3846, 35.9100, 2.77824, 0, 0, 0.983542, 0.180677, -180, 100, 1, 0, '', 0, 26124);

delete from creature_text where entry in (315,43797);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `BroadcastTextID`, `comment`) VALUES 
(315, 0, 0, 'My ring... Who holds my family ring... Tilloa, is that you?', 12, 0, 100, 0, 0, 43963, ''),
(315, 1, 0, 'Tobias...', 12, 0, 100, 0, 0, 43964, ''),
(315, 2, 0, 'It\'s all true, brother. Every word. You doubted it?', 12, 0, 100, 0, 0, 43965, ''),
(315, 3, 0, 'You know why!', 12, 0, 100, 0, 0, 43968, ''),
(315, 4, 0, 'Surely you\'ve felt anger. Anger so foul and vicious that it makes you want to tear someone to shreds...', 12, 0, 100, 0, 0, 43978, ''),
(315, 5, 0, 'Aren\'t you feeling it right now?', 12, 0, 100, 0, 0, 43979, ''),
(315, 6, 0, 'You see, brother... we\'re not so different...', 12, 0, 100, 0, 0, 43981, ''),

(43797, 0, 0, 'Brother!', 12, 0, 100, 0, 0, 43966, ''),
(43797, 1, 0, 'Tell me it\'s not true, brother. Tell me you didn\'t die a murderer!', 12, 0, 100, 0, 0, 43967, ''),
(43797, 2, 0, 'But why?! How could you?', 12, 0, 100, 0, 0, 43973, ''),
(43797, 3, 0, 'No... NO! STOP IT!', 14, 0, 100, 0, 0, 43980, '');

delete from conditions where SourceEntry in (82029) and SourceTypeOrReferenceId = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 82029, 0, 0, 29, 0, 315, 20, 0, 1, 0, '', 'cast only if near not creature');


-- https://forum.wowcircle.com/showthread.php?t=1050667
update creature set phaseid = 26727, phasemask = 0, movementtype = 0, spawndist = 0 where id = 43862;

update quest_template set inprogressphaseid = 26727 where id = 26727;


-- https://forum.wowcircle.com/showthread.php?t=1050674
update quest_template set prevquestid = 34034 where id = 34035;

DELETE FROM `creature_queststarter` WHERE quest in (34035);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(77928, 34035),
(78466, 34035);


-- https://forum.wowcircle.com/showthread.php?t=1050675
update quest_template set requiredraces = 824181832 where id in (35679,35185);
update quest_template set requiredraces = 234881970 where id in (35680,35186);


-- https://forum.wowcircle.com/showthread.php?t=1050684
delete from creature where id = 82695;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(82695, 1116, 6662, 7422, 1, 1, 0, 0, 0, 3117.57, 3307.82, 46.5451, 1.13318, 180, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from creature_template_addon where entry in (82695);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(82695, 0, 0, 0, '164141');

update creature_template set npcflag = 16777216, ainame = 'SmartAI' where entry = 82695;

delete from npc_spellclick_spells where npc_entry in (82695);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(82695, 166508, 1, 0);

delete from conditions where SourceGroup in (82695) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 82695, 166508, 0, 0, 9, 0, 35687, 0, 0, 0, 0, '', 'Allow Spellclick If Player Has Quest');

delete from event_scripts where id = 41377;
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(41377, 0, 10, 82662, 300000, 0, 3146.3979, 3366.7466, 53.3112, 4.27);

update creature_template set ainame = 'SmartAI' where entry = 82662;

update creature_template_wdb set killcredit1 = 0, killcredit2 = 0 where entry = 82662;

delete from smart_scripts where entryorguid in (82662, 82695);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(82662, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Socrethar - on just summoned - start attack'),
(82662, 0, 1, 2, 61, 0, 100, 1, 0, 0, 0, 0, 0, 33, 82673, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Socrethar - on link - give credit'),
(82662, 0, 2, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 82695, 0, 0, 0, 0, 0, 0, 'Socrethar - on link - set data'),

(82695, 0, 0, 1, 38, 0, 100, 1, 0, 1, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Banner - on data set - unaura'),
(82695, 0, 1, 2, 61, 0, 100, 1, 0, 1, 0, 0, 0, 83, 16777216, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Banner - on link - remove npcflag'),
(82695, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 30000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Banner - on link - despawn');


-- https://forum.wowcircle.com/showthread.php?t=1050801
-- https://forum.wowcircle.com/showthread.php?t=1050808
update creature_template set npcflag = 3 where entry = 108872;


-- https://forum.wowcircle.com/showthread.php?t=1050867
update creature_template set ainame = 'SmartAI' where entry in (993531, 993539);

delete from smart_scripts where entryorguid in (993531, 993539);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(993531, NULL, 0, 0, 1, 60, 0, 100, 1, 0, 0, 0, 0, 0, 0, 86, 59119, 0, 21, 10, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Ride Vehicle on nearby player'),
(993531, NULL, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 1, 993531, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Run waypoint on spawn'),
(993531, NULL, 0, 3, 0, 40, 0, 100, 0, 0, 16, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Despawn self'),
(993531, NULL, 0, 4, 0, 40, 0, 100, 0, 0, 7, 0, 0, 0, 0, 1004, 1588, 0, 0, 0, 0, 0, 18, 5, 0, 0, 0, 0, 0, 0, 'Send Scene'),
(993531, NULL, 0, 5, 0, 40, 0, 100, 0, 0, 3, 0, 0, 0, 0, 12, 103660, 5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Spawn Khadgar'),
(993531, NULL, 0, 6, 0, 40, 0, 100, 0, 0, 15, 0, 0, 0, 0, 33, 103607, 0, 0, 0, 0, 0, 18, 5, 0, 0, 0, 0, 0, 0, 'Give Killcredit'),
(993531, NULL, 0, 7, 0, 25, 0, 100, 0, 0, 0, 0, 0, 0, 0, 44, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set phasemask 1'),

(993539, NULL, 0, 0, 1, 60, 0, 100, 1, 0, 0, 0, 0, 0, 0, 86, 59119, 0, 21, 10, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Cast Ride Vehicle on nearby player'),
(993539, NULL, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 53, 1, 993531, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Run waypoint on spawn'),
(993539, NULL, 0, 3, 0, 40, 0, 100, 0, 0, 16, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Despawn self'),
(993539, NULL, 0, 4, 0, 40, 0, 100, 0, 0, 7, 0, 0, 0, 0, 1004, 1588, 0, 0, 0, 0, 0, 18, 5, 0, 0, 0, 0, 0, 0, 'Send Scene'),
(993539, NULL, 0, 5, 0, 40, 0, 100, 0, 0, 3, 0, 0, 0, 0, 12, 103660, 5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Spawn Khadgar'),
(993539, NULL, 0, 6, 0, 40, 0, 100, 0, 0, 15, 0, 0, 0, 0, 33, 103607, 0, 0, 0, 0, 0, 18, 5, 0, 0, 0, 0, 0, 0, 'Despawn self'),
(993539, NULL, 0, 7, 0, 25, 0, 100, 0, 0, 0, 0, 0, 0, 0, 44, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Set phasemask 1');

delete from conditions where sourceentry in (993531, 993539) and SourceTypeOrReferenceId = 22;