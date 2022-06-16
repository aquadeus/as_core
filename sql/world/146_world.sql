

-- 

update quest_template set type=131 where id in (41555,41556,41551,41260,41261,41555,41554,41550,41242,41557,41558,41552,41549,41553,41259,41262);

UPDATE `world_quest_template` SET `areaID`='7618' WHERE (`entry`='41551');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282138');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282505');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282506');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282512');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282511');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='2787381');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282510');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='2787382');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282504');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282503');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282100');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282508');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282139');
UPDATE `quest_template_objective` SET `Flags`='1' WHERE (`ID`='282507');

UPDATE `creature_template` SET `lootid`='103784' WHERE (`entry`='103784');

delete from quest_poi where questID in (41262, 41558, 41557, 41554, 41553);
INSERT INTO`quest_poi` (`QuestID`, `BlobIndex`, `Idx1`, `ObjectiveIndex`, `QuestObjectiveID`, `QuestObjectID`, `MapID`, `WorldMapAreaId`, `Floor`, `Priority`, `Flags`, `WorldEffectID`, `PlayerConditionID`, `WoDUnk1`, `AlwaysAllowMergingBlobs`, `VerifiedBuild`) VALUES
('41262', '0', '1', '32', '0', '0', '1220', '1033', '0', '0', '2', '0', '0', '0', '0', '24742'),
('41262', '0', '0', '0', '282141', '135511', '1220', '1033', '0', '0', '0', '0', '0', '1241724', '0', '24742'),

('41558', '0', '1', '32', '0', '0', '1220', '1033', '0', '0', '2', '0', '0', '0', '0', '24015'),
('41558', '0', '0', '0', '282512', '135511', '1220', '1033', '0', '0', '0', '0', '0', '1241728', '0', '24015'),

('41557', '0', '1', '32', '0', '0', '1220', '1033', '0', '0', '2', '0', '0', '0', '0', '23857'),
('41557', '0', '0', '0', '282511', '135511', '1220', '1033', '0', '0', '0', '0', '0', '1241726', '0', '23857'),

('41554', '0', '0', '0', '282508', '135511', '1220', '1018', '0', '0', '0', '0', '0', '1147592', '0', '24430'),
('41554', '0', '1', '32', '0', '0', '1220', '1018', '0', '0', '0', '0', '0', '1147592', '0', '24430'),

('41553', '0', '0', '0', '282507', '135511', '1220', '1018', '0', '0', '0', '0', '0', '1147493', '0', '24015'),
('41553', '0', '1', '32', '0', '0', '1220', '1018', '0', '0', '0', '0', '0', '1147493', '0', '24015');


delete from quest_poi_points where questID in (41262, 41558, 41557, 41554, 41553);
INSERT INTO `quest_poi_points` (`QuestID`, `Idx1`, `Idx2`, `X`, `Y`, `VerifiedBuild`) VALUES
('41262', '0', '0', '1590', '2445', '24742'),
('41262', '1', '0', '1595', '2445', '24742'),

('41558', '0', '0', '1613', '5296', '24015'),
('41558', '1', '0', '1619', '5296', '24015'),

('41557', '0', '0', '2353', '4497', '23857'),
('41557', '1', '0', '2344', '4489', '23857'),

('41554', '0', '0', '3250', '6890', '24430'),
('41554', '1', '0', '3250', '6890', '24430'),

('41553', '1', '0', '2782', '5797', '24015'),
('41553', '0', '0', '2782', '5797', '24015');

