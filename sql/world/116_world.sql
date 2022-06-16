
-- https://ru.wowhead.com/npc=93536
UPDATE `creature_template` SET `trainer_type`='2' WHERE (`entry`='93536');

delete from gameobject_template where entry = 246710;
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `data32`, `RequiredLevel`, `WorldEffectID`, `AIName`, `ScriptName`, `VerifiedBuild`) VALUES
('246710', '3', '10803', 'Maquereau d’argent de Nomi', 'questinteract', 'Récupération', '', '0', '0', '0.8', '0', '0', '0', '0', '0', '0', '43', '246710', '1', '0', '0', '0', '0', '0', '40991', '0', '0', '0', '0', '0', '23645', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '64377', '1', '0', '0', '0', '', '', '26972');

delete from gameobject where id = 246710;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES
('246710', '1220', '7502', '7589', '1', '1', '0', '-803.233', '4298.17', '727.629', '0.197891', '-0.130851', '0.0669527', '0.107598', '0.983269', '20', '100', '1', '0', '', '0', '-1'),
('246710', '1220', '7502', '7502', '1', '1', '0', '-949.521', '4550.39', '729.972', '5.06586', '0', '0', '-0.571772', '0.820413', '20', '100', '1', '0', '', '0', '-1');



-- Cooking World Quests
-- https://ru.wowhead.com/quest=41242
-- https://ru.wowhead.com/quest=41260
-- https://ru.wowhead.com/quest=41262
-- https://ru.wowhead.com/quest=41549
-- https://ru.wowhead.com/quest=41550
-- https://ru.wowhead.com/quest=41553
-- https://ru.wowhead.com/quest=41554
-- https://ru.wowhead.com/quest=41557
-- https://ru.wowhead.com/quest=41558
-- https://ru.wowhead.com/quest=41259
-- https://ru.wowhead.com/quest=41261
-- https://ru.wowhead.com/quest=41551
-- https://ru.wowhead.com/quest=41552
-- https://ru.wowhead.com/quest=41555
-- https://ru.wowhead.com/quest=41556

delete from world_quest_template where entry in (41242, 41553, 41260, 41262, 41549, 41550, 41554, 41557, 41558, 41259, 41261, 41551, 41552, 41555, 41556);
INSERT INTO `world_quest_template` (`entry`, `overrideAreaGroupID`, `areaID`, `enabled`, `overrideVariableID`, `overrideVariableValue`) VALUES 
('41242', '-1', '7725', '1', '0', '0'),
('41553', '-1', '7804', '1', '0', '0'),
('41260', '-1', '8363', '1', '0', '0'),
('41262', '-1', '7917', '1', '0', '0'),
('41549', '-1', '7732', '1', '0', '0'),
('41550', '-1', '8051', '1', '0', '0'),
('41554', '-1', '8332', '1', '0', '0'),
('41557', '-1', '7637', '1', '0', '0'),
('41558', '-1', '7984', '1', '0', '0'),
('41259', '-1', '8297', '1', '0', '0'),
('41261', '-1', '8167', '1', '0', '0'),
('41551', '-1', '0', '1', '0', '0'),
('41552', '-1', '7334', '1', '0', '0'),
('41555', '-1', '8172', '1', '0', '0'),
('41556', '-1', '8375', '1', '0', '0');

delete from world_quest_loot_group where group_id = 131;
INSERT INTO `world_quest_loot_group` (`group_id`, `loot_type`, `loot_item_id`, `loot_count_min`, `loot_count_max`, `flags`) VALUES
('131', '1', '133680', '5', '20', '0');

delete from world_quest_loot where group_id=131;
INSERT INTO `world_quest_loot` (`quest_id`, `group_id`, `chance`) VALUES
('41242', '131', '100'),
('41553', '131', '100'),
('41260', '131', '100'),
('41262', '131', '100'),
('41549', '131', '100'),
('41550', '131', '100'),
('41554', '131', '100'),
('41557', '131', '100'),
('41558', '131', '100'),
('41259', '131', '100'),
('41261', '131', '100'),
('41551', '131', '100'),
('41552', '131', '100'),
('41555', '131', '100'),
('41556', '131', '100');

delete from creature where id in (103681, 103784, 103785, 103786, 103787);
INSERT INTO creature (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags3`, `dynamicflags`, `isActive`) VALUES
('103681', '1220', '7503', '8051', '1', '55000', '0', '0', '4075.45', '3611.61', '896.649', '5.56555', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103681', '1220', '7503', '7725', '1', '55000', '0', '0', '5415.39', '4629.69', '629.373', '3.96506', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103681', '1220', '7503', '7732', '1', '55000', '0', '0', '4651.88', '4699.01', '678.723', '2.53392', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103681', '1220', '7503', '7769', '1', '55000', '0', '0', '5263.17', '4734.84', '647.541', '0.777117', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103681', '1220', '7503', '7732', '1', '55000', '0', '0', '4544.73', '4687.94', '657.136', '1.5154', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103784', '1220', '7334', '7334', '1', '55000', '0', '0', '-1334.9', '5545.16', '2.38772', '0.558127', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103784', '1220', '7334', '7334', '1', '55000', '0', '0', '-1192.27', '5536.5', '2.82657', '1.6763', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103784', '1220', '7334', '8297', '1', '55000', '0', '0', '673.884', '7813.21', '-0.314825', '0.0599594', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103784', '1220', '7334', '7618', '1', '55000', '0', '0', '1296', '7595', '35.59', '0.815', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103785', '1220', '7558', '8332', '1', '55000', '0', '0', '3243.9', '6899.59', '2.51068', '0.503143', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103785', '1220', '7558', '7804', '1', '55000', '0', '0', '2782.16', '5797.23', '294.875', '5.53628', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103785', '1220', '7558', '8363', '1', '55000', '0', '0', '2520.79', '6289.21', '196.5', '2.68263', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103786', '1220', '7541', '7542', '1', '55000', '0', '0', '3411.14', '3052.11', '526.299', '4.11453', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103786', '1220', '7541', '8375', '1', '55000', '0', '0', '3440.19', '3061.69', '541.397', '1.96632', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103786', '1220', '7541', '8172', '1', '55000', '0', '0', '2833.39', '1973.12', '184.904', '0.0535984', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103786', '1220', '7541', '8167', '1', '55000', '0', '0', '4073.01', '1880.21', '157.609', '0.100721', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103787', '1220', '7637', '7917', '1', '55000', '0', '0', '1547.68', '2402.3', '1.06034', '2.82073', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103787', '1220', '7637', '7637', '1', '55000', '0', '0', '1473.3', '5287.99', '56.7165', '0.0011692', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103787', '1220', '7637', '7637', '1', '55000', '0', '0', '2352.37', '4501.11', '257.615', '4.98059', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0'),
('103787', '1220', '7637', '7984', '1', '55000', '0', '0', '1613.22', '5300.68', '72.8809', '5.71003', '180', '0', '0', '0', '0', '0', '0', '0', '0', '0');

update creature set phaseMask=0 where id in (103681, 103784, 103785, 103786, 103787);

delete from phase_definitions where zoneId in (7503, 7334, 7558, 7541, 7637) and phaseId=55000;
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES
('7503', '55000', '0', '55000', '0', '0', '0', 'World Quest Phase'),

('7334', '55000', '0', '55000', '0', '0', '0', 'World Quest Phase'),

('7558', '55000', '0', '55000', '0', '0', '0', 'World Quest Phase'),

('7541', '55000', '0', '55000', '0', '0', '0', 'World Quest Phase'),

('7637', '55000', '0', '55000', '0', '0', '0', 'World Quest Phase');

delete from conditions where SourceTypeOrReferenceId=26 and SourceGroup in (7503, 7334, 7558, 7541, 7637) and SourceEntry=55000;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
('26', '7503', '55000', '0', '0', '47', '0', '41553', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '1', '47', '0', '41260', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '2', '47', '0', '41262', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '3', '47', '0', '41549', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '4', '47', '0', '41550', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '5', '47', '0', '41554', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '6', '47', '0', '41557', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '7', '47', '0', '41558', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '8', '47', '0', '41259', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '9', '47', '0', '41261', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '10', '47', '0', '41551', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '11', '47', '0', '41552', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '12', '47', '0', '41555', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '13', '47', '0', '41556', '8', '0', '0', '0', '', NULL),
('26', '7503', '55000', '0', '14', '47', '0', '41242', '8', '0', '0', '0', '', NULL),

('26', '7334', '55000', '0', '0', '47', '0', '41553', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '1', '47', '0', '41260', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '2', '47', '0', '41262', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '3', '47', '0', '41549', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '4', '47', '0', '41550', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '5', '47', '0', '41554', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '6', '47', '0', '41557', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '7', '47', '0', '41558', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '8', '47', '0', '41259', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '9', '47', '0', '41261', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '10', '47', '0', '41551', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '11', '47', '0', '41552', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '12', '47', '0', '41555', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '13', '47', '0', '41556', '8', '0', '0', '0', '', NULL),
('26', '7334', '55000', '0', '14', '47', '0', '41242', '8', '0', '0', '0', '', NULL),

('26', '7558', '55000', '0', '0', '47', '0', '41553', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '1', '47', '0', '41260', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '2', '47', '0', '41262', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '3', '47', '0', '41549', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '4', '47', '0', '41550', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '5', '47', '0', '41554', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '6', '47', '0', '41557', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '7', '47', '0', '41558', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '8', '47', '0', '41259', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '9', '47', '0', '41261', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '10', '47', '0', '41551', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '11', '47', '0', '41552', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '12', '47', '0', '41555', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '13', '47', '0', '41556', '8', '0', '0', '0', '', NULL),
('26', '7558', '55000', '0', '14', '47', '0', '41242', '8', '0', '0', '0', '', NULL),

('26', '7541', '55000', '0', '0', '47', '0', '41553', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '1', '47', '0', '41260', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '2', '47', '0', '41262', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '3', '47', '0', '41549', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '4', '47', '0', '41550', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '5', '47', '0', '41554', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '6', '47', '0', '41557', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '7', '47', '0', '41558', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '8', '47', '0', '41259', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '9', '47', '0', '41261', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '10', '47', '0', '41551', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '11', '47', '0', '41552', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '12', '47', '0', '41555', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '13', '47', '0', '41556', '8', '0', '0', '0', '', NULL),
('26', '7541', '55000', '0', '14', '47', '0', '41242', '8', '0', '0', '0', '', NULL),

('26', '7637', '55000', '0', '0', '47', '0', '41553', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '1', '47', '0', '41260', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '2', '47', '0', '41262', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '3', '47', '0', '41549', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '4', '47', '0', '41550', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '5', '47', '0', '41554', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '6', '47', '0', '41557', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '7', '47', '0', '41558', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '8', '47', '0', '41259', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '9', '47', '0', '41261', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '10', '47', '0', '41551', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '11', '47', '0', '41552', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '12', '47', '0', '41555', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '14', '47', '0', '41242', '8', '0', '0', '0', '', NULL),
('26', '7637', '55000', '0', '13', '47', '0', '41556', '8', '0', '0', '0', '', NULL);


delete from creature_loot_template where item = 135511;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES
('103681', '135511', '-100', '1', '0', '1', '1', '0', '0', '0'),
('103787', '135511', '-100', '1', '0', '1', '1', '0', '0', '0'),
('103784', '135511', '-100', '1', '0', '1', '1', '0', '0', '0'),
('103786', '135511', '-100', '1', '0', '1', '1', '0', '0', '0'),
('103785', '135511', '-100', '1', '0', '1', '1', '0', '0', '0');


delete from item_loot_template where entry = 141700;
INSERT INTO `item_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES
('141700', '133846', '20', '1', '0', '1', '1', '0', '0', '0'),
('141700', '133866', '20', '1', '0', '1', '1', '0', '0', '0'),
('141700', '133824', '20', '1', '0', '1', '1', '0', '0', '0'),
('141700', '133844', '20', '1', '0', '1', '1', '0', '0', '0'),
('141700', '133864', '20', '1', '0', '1', '1', '0', '0', '0');

