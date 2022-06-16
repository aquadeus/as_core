-- https://forum.wowcircle.com/showthread.php?t=1053333
update creature_template set unit_flags = 768, ainame = 'SmartAI' where entry = 79593;

delete from creature where id = 79593;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(79593, 1265, 7025, 7042, 1, 16, 0, 0, 0, 4614.04, -2473.62, 14.1183, 1.62095, 60, 0, 0, 326181, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from smart_scripts where entryorguid in (79593);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(79593, 0, 0, 0, 60, 0, 100, 0, 0, 0, 5000, 5000, 0, 33, 79593, 0, 0, 0, 0, 0, 18, 10, 0, 0, 0, 0, 0, 0, 'Ankova - on update - give credit');

delete from conditions where SourceEntry in (79593) and SourceTypeOrReferenceId = 22;
-- INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
-- (22, 1, 79593, 0, 0, 9, 0, 34431, 0, 0, 0, 0, '', 'Allow Smart - If player has quest'),
-- (22, 1, 79593, 0, 0, 48, 0, 272929, 0, 0, 1, 0, '', 'Allow Smart - objective not completed'),

-- (22, 1, 79593, 0, 1, 9, 0, 34737, 0, 0, 0, 0, '', 'Allow Smart - If player has quest'),
-- (22, 1, 79593, 0, 1, 48, 0, 272984, 0, 0, 1, 0, '', 'Allow Smart - objective not completed');

delete from creature_template_addon where entry in (79593);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(79593, 0, 0, 0, '29266');


-- https://forum.wowcircle.com/showthread.php?t=1053337
update gameobject set state = 1 where id in (231815,231816,231817,231818,231819,231820);

update creature_template set lootid = entry where entry in (81367,81357);


-- https://forum.wowcircle.com/showthread.php?t=1053330
delete from creature_text where Entry = 78950;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(78950, 0, 0, 'You and I against an entire clan? Sounds like my kind of fight.', 12, 0, 100, 0, 0, 0, 83856),
(78950, 1, 0, 'With any luck, we will escape before they can complete their transformation ritual.', 12, 0, 100, 0, 0, 0, 83888),
(78950, 2, 0, 'It looks like Khadgar and the rest are on the move. Our work is nearly done, champion!', 12, 0, 100, 0, 0, 0, 83861);

update creature_template set ainame = 'SmartAI' where entry = 78950;

delete from smart_scripts where entryorguid in (78556,78950);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(78950, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Ariok - on just summaned - talk'),

(78556, 0, 0, 1, 62, 0, 100, 0, 16641, 0, 0, 0, 85, 159404, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ariok - On Gossip Select - Invoker Cast'),
(78556, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 33, 78556, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ariok - Link - Give Credit');


-- https://forum.wowcircle.com/showthread.php?t=1053341
update creature_template_wdb set displayid1 = 52540, displayid2 = 0 where entry = 80886;


-- https://forum.wowcircle.com/showthread.php?t=1053538
delete from creature where id = 61021 and position_x like '1397%';
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(61021, 870, 0, 0, 1, 1, 0, 0, 0, 1397.9744, 2689.6438, 322.0177, 5.2, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1053327
update gameobject_template set data1 = 233432 where entry = 233432;

delete from gameobject_loot_template where entry in (233432);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(233432, 113585, 100, 0, 0, 1, 3),
(233432, 112449, 100, 0, 0, 3, 8);


-- https://forum.wowcircle.com/showthread.php?t=1052673
delete from reference_loot_template where item = 116771;

delete from creature_loot_template where item in (116771);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(83746, 116771, 1, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1052298
update gameobject_loot_template set chanceorquestchance = 100 where item = 102225;


-- https://forum.wowcircle.com/showthread.php?t=1050205
update creature_template set lootid = entry where entry in (80725);

delete from creature_loot_template where entry in (80725);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(80725, 114227, 100, 0, 0, 1, 1);

delete from creature_loot_currency where creature_id in (80725);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(80725, 824, 19);


-- https://forum.wowcircle.com/showthread.php?t=1050206
update creature_template set lootid = entry where entry in (88583);

delete from creature_loot_template where entry in (88583);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(88583, 119414, 40, 0, 0, 1, 1),
(88583, 120945, 100, 0, 0, 1, 1);

delete from creature_loot_currency where creature_id in (88583);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(88583, 823, 6);


-- https://forum.wowcircle.com/showthread.php?t=1050685
update creature_template set lootid = entry where entry in (81639);

delete from creature_loot_template where entry in (81639);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(81639, 103994, -7, 0, 0, 1, 1),
(81639, 117551, 40, 0, 0, 1, 1),
(81639, 120945, 100, 0, 0, 1, 1);

delete from creature_loot_currency where creature_id in (81639);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(81639, 823, 6);