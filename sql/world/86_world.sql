-- https://forum.wowcircle.com/showthread.php?t=1050750
DELETE FROM `creature_questender` WHERE quest in (48445);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(121230, 48445);

DELETE FROM `creature_queststarter` WHERE quest in (48445);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(121230, 48445);

update quest_template set PrevQuestId = 48443 where id = 48445;
update quest_template set PrevQuestId = 48441, method = 2 where id = 48910;

DELETE FROM `creature_questender` WHERE quest in (48910);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(119388, 48910);

DELETE FROM `creature_queststarter` WHERE quest in (48910);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(119388, 48910);

-- https://forum.wowcircle.com/showthread.php?t=1054595
update gameobject_template set data1 = 221670 where entry = 220986;


-- https://forum.wowcircle.com/showthread.php?t=854733
update creature_loot_template set chanceorquestchance = 1, groupid = 0 where item = 97961;


-- https://forum.wowcircle.com/showthread.php?t=857857
update creature_template set lootid = entry where entry in (116180,114323,116601,115751,115025,115031,116600,115846);

delete from creature_loot_template where item in (142487);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `difficultyMask`) VALUES 
(116180, 142487, -45088, 1, 0, 1, 2, 0),
(114323, 142487, -45088, 1, 0, 30, 60, 122880),
(116601, 142487, -45088, 1, 0, 1, 2, 0),
(115751, 142487, -45088, 1, 0, 1, 2, 0),
(115025, 142487, -45088, 1, 0, 1, 2, 0),
(115031, 142487, -45088, 1, 0, 1, 2, 0),
(116600, 142487, -45088, 1, 0, 1, 2, 0),
(115846, 142487, -45088, 1, 0, 1, 2, 0);


-- https://forum.wowcircle.com/showthread.php?t=869932
update creature set position_z = 267, spawndist = 0, movementtype = 0 where id = 32448 and map = 616;

update gameobject_loot_template set item = mincountOrRef * -1 where entry in (26097,26094) and mincountOrRef < 0;

update gameobject_loot_template set ChanceOrQuestChance = 4 where item in (43953,43952);


-- https://forum.wowcircle.com/showthread.php?t=880773
delete from creature_loot_template where item in (136851);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(106275, 136851, 100, 0, 0, 1, 1);

DELETE FROM `creature_questender` WHERE quest in (40762) and id = 101082;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(101082, 40762);


-- https://forum.wowcircle.com/showthread.php?t=1034633
delete from creature_loot_template where entry in (15552);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(15552, 142266, 100, 0, 0, 5, 10),
(15552, 142264, 67, 0, 0, 1, 1),
(15552, 142263, 59, 0, 0, 1, 1),
(15552, 142262, 13, 0, 0, 1, 1),
(15552, 142265, 3, 0, 0, 1, 1);