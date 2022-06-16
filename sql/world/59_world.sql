-- https://forum.wowcircle.com/showthread.php?t=837855
update creature_template set dmg_multiplier = 5 where entry = 118489;
update creature_template set dmg_multiplier = 5 where entry = 118491;
update creature_template set dmg_multiplier = 1 where entry = 118492;
update creature_template set dmg_multiplier = 2 where entry = 119469;
update creature_template set dmg_multiplier = 5 where entry = 119472;
update creature_template set dmg_multiplier = 1 where entry = 122397;


-- https://forum.wowcircle.com/showthread.php?t=887261
update gameobject_template set displayid = 0 where entry = 176510;


-- https://forum.wowcircle.com/showthread.php?t=1047141
update creature_loot_template set chanceorquestchance = 100, lootmode = 1, groupid = 0 where item = 130171 and entry = 92180;

update creature_template set lootid = 92180 where entry = 92180;

update creature_template_wdb set vignetteid = 0 where entry = 92180; -- 784


-- https://forum.wowcircle.com/showthread.php?t=1046462
update creature_template set lootid = 70012 where entry in (69992,70015);


-- https://forum.wowcircle.com/showthread.php?t=1045243
update creature_template set lootid = entry where entry in (87846);

delete from creature_loot_template where entry in (87846);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(87846, 120945, 100, 0, 0, 1, 1),
(87846, 118099, 78, 0, 0, 1, 1),
(87846, 119389, 40, 0, 0, 1, 1),
(87846, 117491, 8, 0, 0, 1, 1),
(87846, 111387, 0.2, 0, 0, 1, 1),
(87846, 118338, 0.1, 0, 0, 1, 1),
(87846, 113321, 0.1, 0, 0, 1, 1),
(87846, 113429, 0.1, 0, 0, 1, 1),
(87846, 118336, 0.1, 0, 0, 1, 1),
(87846, 118337, 0.1, 0, 0, 1, 1);

delete from creature_loot_currency where creature_id in (87846);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(87846, 823, 2);


-- https://forum.wowcircle.com/showthread.php?t=1050089
delete from creature where id = 110482;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(110482, 1519, 8022, 8023, 1, 3, 0, 0, 0, 1630.31, 1419, 208.741, 3.28766, 300, 0, 0, 870, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1045242
update creature_template set lootid = entry where entry in (87788);

delete from creature_loot_template where entry in (87788);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(87788, 120945, 95, 0, 0, 1, 1),
(87788, 118099, 90, 0, 0, 3, 5),
(87788, 119405, 35, 0, 0, 1, 1),
(87788, 118100, 10, 0, 0, 1, 1),
(87788, 93194 , 6, 0, 0, 1, 1),
(87788, 117491, 5, 0, 0, 3, 3);

delete from creature_loot_currency where creature_id in (87788);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(87788, 823, 6);


-- https://forum.wowcircle.com/showthread.php?t=1045241
update creature_template set lootid = entry where entry in (87239);

delete from creature_loot_template where entry in (87239);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(87239, 120945, 97, 0, 0, 1, 1),
(87239, 113494, -2, 0, 0, 1, 1),
(87239, 113493, -2, 0, 0, 1, 1),
(87239, 113492, -2, 0, 0, 1, 1),
(87239, 119380, 1, 0, 0, 1, 1);

delete from creature_loot_currency where creature_id in (87239);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(87239, 823, 6);


-- https://forum.wowcircle.com/showthread.php?t=1045240
update creature_template set lootid = entry where entry in (86959);

delete from creature_loot_template where entry in (86959);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(86959, 120945, 97, 0, 0, 1, 1),
(86959, 113494, -2, 0, 0, 1, 1),
(86959, 113493, -2, 0, 0, 1, 1),
(86959, 113492, -2, 0, 0, 1, 1),
(86959, 119355, 40, 0, 0, 1, 1);

delete from creature_loot_currency where creature_id in (86959);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(86959, 823, 6);


-- https://forum.wowcircle.com/showthread.php?t=1045239
update creature_template set lootid = entry where entry in (87234);

delete from creature_loot_template where entry in (87234);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(87234, 120945, 97, 0, 0, 1, 1),
(87234, 113494, -2, 0, 0, 1, 1),
(87234, 113493, -2, 0, 0, 1, 1),
(87234, 113492, -2, 0, 0, 1, 1),
(87234, 119380, 40, 0, 0, 1, 1);

delete from creature_loot_currency where creature_id in (87234);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(87234, 823, 6);


-- https://forum.wowcircle.com/showthread.php?t=1044352
update creature_template set lootid = entry where entry in (84890);

delete from creature_loot_template where entry in (84890);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(84890, 118200, 100, 0, 0, 1, 1),
(84890, 111557, 1, 0, 0, 1, 3),
(84890, 107517, 2, 0, 0, 1, 1);

delete from creature_loot_currency where creature_id in (84890);
INSERT INTO `creature_loot_currency` (`creature_id`, `currencyid1`, `currencycount1`) VALUES 
(84890, 824, 18);


-- https://forum.wowcircle.com/showthread.php?t=1044242
delete from item_loot_template where entry in (150924,141344);
INSERT INTO `item_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(141344, 105, 100, 0, 0, -105, 1),
(150924, 106, 100, 0, 0, -106, 1);

delete from reference_loot_template where entry in (105, 106);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(105, 141343, 0, 0, 0, 1, 1),
(105, 141342, 0, 0, 0, 1, 1),
(105, 141341, 0, 0, 0, 1, 1),
(105, 141340, 0, 0, 0, 1, 1),
(105, 141339, 0, 0, 0, 1, 1),
(105, 141338, 0, 0, 0, 1, 1),

(106, 141992, 0, 0, 0, 1, 1),
(106, 141991, 0, 0, 0, 1, 1),
(106, 141990, 0, 0, 0, 1, 1),
(106, 141989, 0, 0, 0, 1, 1),
(106, 141988, 0, 0, 0, 1, 1),
(106, 141987, 0, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1043466
delete from gameobject_loot_template where entry in (49498);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(49498, -777, 100, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1043452
delete from creature_loot_template where entry in (50811) and item in (87600,87599,87596,87597,87595,87601,87602,87603,87598,792);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(50811, 792, 100, 1, 0, -792, 1);

delete from reference_loot_template where entry in (792);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(792, 87600, 0, 1, 0, 1, 1),
(792, 87599, 0, 1, 0, 1, 1),
(792, 87596, 0, 1, 0, 1, 1),
(792, 87597, 0, 1, 0, 1, 1),
(792, 87595, 0, 1, 0, 1, 1),
(792, 87601, 0, 1, 0, 1, 1),
(792, 87602, 0, 1, 0, 1, 1),
(792, 87603, 0, 1, 0, 1, 1),
(792, 87598, 0, 1, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1041789
delete from gameobject_loot_template where entry in (42911);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(42911, 86393, 100, 1, 0, 1, 1);