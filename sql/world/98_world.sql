--
update quest_template set InProgressPhaseID = Id, CompletedPhaseID = Id where Id = 41367;

update gameobject set phaseMask = 0, phaseId = 491001 where id in (247386,242502,202436) and map = 1480;
update creature set phaseMask = 0, phaseId = 491001 where id in (815866,815867,815868,100577) and map = 1480;

delete from quest_template_objective where QuestID = 41367;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`) VALUES 
(282297, 41367, 0, 0, 0, 104071, 1, 0, 0),
(282260, 41367, 1, 1, 1, 134857, 1, 0, 1),
(282263, 41367, 0, 2, 2, 132999, 1, 0, 0),
(296764, 41367, 0, 3, 3, 52562 , 1, 0, 0);

delete from creature where id in (103996,104014,104016,132999);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(103996, 1480, 7695, 7696, 4096, 0, 41367, 0, 0, 467.671, -2128.64, 840.857, 0.0898724, 300, 0, 0, 26996250, 0, 0, 0, 0, 537297794, 33554433, 8192, 0, 0, 0, '', '', -1);

delete from gameobject where id = 247792;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(247792, 1480, 7695, 7696, 4096, 0, 41367, 529.476, -2124.74, 840.857, 3.12076, 0, 0, 0.999946, 0.0104166, 300, 0, 1, 0, '', 0, -1);

delete from spell_loot_template where entry in (205910);
INSERT INTO `spell_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(205910, 134857, 100, 0, 0, 1, 1);

update creature_template set AIName = 'SmartAI' where entry in (103996,104014,104016,132999);
update creature_template set npcflag = 16777216 where entry in (103996);

delete from creature_template_addon where entry = 103996;

delete from smart_scripts where entryorguid in (103996,104014,104016,132999,247791);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(103996, '', 0, 0, 0, 10, 0, 100, 1, 0, 1, 50, 0, 0, 0, 12, 104016, 1, 600000, 0, 0, 0, 8, 0, 0, 0, 490.044, -2115.69, 840.857, 5.95758, 'Lich - on ooc los - summon creature'),
(103996, '', 0, 1, 0, 10, 0, 100, 1, 0, 1, 5, 0, 0, 0, 12, 104014, 1, 600000, 0, 0, 0, 8, 0, 0, 0, 471.832, -2129.56, 840.857, 0.24301, 'Lich - on ooc los - summon creature'),
(103996, '', 0, 2, 3, 73, 0, 100, 1, 0, 1, 5, 0, 0, 0, 12, 132999, 1, 600000, 0, 0, 0, 8, 0, 0, 0, 463.42, -2117.19, 840.857, 6.2356, 'Lich - on spell click - summon creature'),
(103996, '', 0, 3, 0, 61, 0, 100, 1, 0, 1, 5, 0, 0, 0, 83, 16777216, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Lich - on link - remove flag'),

(132999, '', 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 11, 251038, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Spell hit - Say Invoker'),
(132999, '', 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 4000, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Spell hit - Say Invoker'),
(132999, '', 0, 2, 0, 4, 0, 100, 1, 0, 0, 0, 0, 0, 0, 11, 236993, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Aggro - Cast - Victim'),
(132999, '', 0, 3, 0, 0, 0, 100, 0, 0, 4500, 8000, 4500, 8000, 0, 11, 252438, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'IC - Cast  - Target'),
(132999, '', 0, 4, 0, 0, 0, 100, 0, 0, 9000, 18000, 9000, 18000, 0, 11, 252730, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'IC - Cast  - Target'),
(132999, '', 0, 5, 0, 2, 0, 100, 1, 0, 0, 40, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Below 40% - Say - Invoker'),
(132999, '', 0, 6, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Death - say - invoker'),
(132999, '', 0, 7, 0, 52, 0, 100, 1, 0, 0, 132999, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 25, 30, 0, 0, 0, 0, 0, 0, 'Text over - Attack start'),

(104016, '', 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'Thief - on just summoned - attack'),
(104016, '', 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thief - on link - talk'),

(104014, '', 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'Thief - on just summoned - attack'),
(104014, '', 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Thief - on link - talk');

delete from spell_target_position where id = 205923;
INSERT INTO `spell_target_position` (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`, `VerifiedBuild`) VALUES 
(205923, 1, 1480, 521.1144, -2124.5166, 840.8576, 3.1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1055057
delete from npc_vendor where entry = 113843;
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES 
(113843, 0, 140670, 0, 0, 6097),
(113843, 1, 140671, 0, 0, 6097),
(113843, 2, 130151, 0, 0, 6099);


-- https://forum.wowcircle.com/showthread.php?t=1054011
delete from gameobject where id = 250536;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(250536, 1220, 7541, 8167, 1, 1, 0, 3913.85, 2133.56, 230.236, 4.53033, 0, 0, 0.768457, -0.639901, 300, 0, 1, 0, '', 0, -1);

DELETE FROM `gameobject_queststarter` WHERE id = 250536 and quest in (42445);
INSERT INTO `gameobject_queststarter` (`id`, `quest`) VALUES
(250536, 42445);


-- https://forum.wowcircle.com/showthread.php?t=880684
update creature_template set faction = 7, unit_flags = 768 where entry = 104406;

delete from creature_template_addon where entry in (104406);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(104406, 0, 0, 0, '196491');

DELETE FROM `creature_questender` WHERE id in (104406);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(104406, 40364),
(104406, 41618),
(104406, 41606);

DELETE FROM `creature_queststarter` WHERE id in (104406);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(104406, 40364),
(104406, 41618);

update quest_template set PrevQuestId = 41606 where id = 40364;
update quest_template set PrevQuestId = 40364 where id = 41618;


-- https://forum.wowcircle.com/showthread.php?t=1050664
delete from creature where id = 45739;
INSERT INTO `creature` (`guid`, `linked_id`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(131097, 'FC09B57D9881C3351832CA104B4F0C1CB66994C2', 45739, 0, 10, 576, 1, 1, 0, 0, 1, -10266.3, -1623.07, 95.9119, 4.55531, 14400, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1057023
delete from conditions where SourceTypeOrReferenceId = 26 and SourceGroup = 7541 and SourceEntry = 39652;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7541, 39652, 0, 0, 9, 0, 38624, 0, 0, 0, 0, '', 'Phase 39652 in zone 7541 if player not reward quest 38624');


-- https://forum.wowcircle.com/showthread.php?t=972878
delete from creature_template_addon where entry = 96084;


-- https://forum.wowcircle.com/showthread.php?t=1046465
update creature set spawntimesecs = 900 where id = 69161;


-- https://forum.wowcircle.com/showthread.php?t=1046187
delete from creature where guid = 21733;


-- https://forum.wowcircle.com/showthread.php?t=1045531
update creature_template set VehicleId = 0 where entry = 91759;


-- https://forum.wowcircle.com/showthread.php?t=1058130
update creature_template set AIName = 'SmartAI' where entry = 94179;
delete from smart_scripts where entryorguid in (94179);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(94179, 0, 0, 0, 10, 0, 100, 0, 1, 20, 0, 0, 0, 15, 38384, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc los - area explored or event happend');

delete from conditions where SourceEntry in (94179) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 94179, 0, 0, 9, 0, 38384, 0, 0, 0, 0, '', 'Smart if quest incomplete');


-- https://forum.wowcircle.com/showthread.php?t=1044265
delete from gameobject where id = 236935;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(236935, 1116, 6662, 7389, 1, 1, 0, 4146.24, 1651.84, 150.242, 0.553504, 0, 0, 0.273233, 0.961948, 300, 0, 1, 0, '', 0, -1);

update gameobject_template set data1 = entry where entry = 236935;

delete from gameobject_loot_template where entry in (236935);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(236935, -823, 100, 0, 0, 9, 9),
(236935, 120945, 100, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1044263
delete from gameobject where id = 227953;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(227953, 1116, 6662, 7389, 1, 1, 0, 3976.67, 1929.32, -1.70896, 3.24742, 0, 0, 0.9986, -0.0528887, 300, 0, 1, 0, '', 0, -1);

update gameobject_template set data1 = entry where entry = 227953;

delete from gameobject_loot_template where entry in (227953);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(227953, 117568, 100, 0, 0, 12, 20);


-- https://forum.wowcircle.com/showthread.php?t=1044261
delete from gameobject where id = 227951;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(227951, 1116, 6662, 7389, 1, 1, 0, 3900.16, 1962.73, 155.102, 2.80254, 0, 0, 0.985665, 0.168715, 300, 0, 1, 0, '', 0, -1);

update gameobject_template set data1 = entry where entry = 227951;

delete from gameobject_loot_template where entry in (227951);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(227951, 116117, 100, 0, 0, 1, 1);