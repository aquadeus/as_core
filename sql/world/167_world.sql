-- https://forum.wowcircle.net/showthread.php?t=1064844
delete from areatrigger_actions where entry = 3184;
INSERT INTO `areatrigger_actions` (`entry`, `summon_spellId`, `customEntry`, `id`, `moment`, `actionType`, `targetFlags`, `spellId`, `maxCharges`, `aura`, `hasspell`, `chargeRecoveryTime`, `scale`, `hitMaxCount`, `amount`, `onDespawn`, `comment`) VALUES 
(3184, 0, 0, 0, 5, 0, 4096, 173015, 0, 0, 0, 0, 0, 0, 0, 0, '173015 Electrostatic Distortion');


-- https://forum.wowcircle.net/showthread.php?t=1064846
update quest_template set PrevQuestId = 33730 where id = 34962;
update quest_template set PrevQuestId = 34962 where id = 33731;


-- https://forum.wowcircle.net/showthread.php?t=1064867
update quest_template set StartScript = Id, Flags = 0 where Id = 27438;

delete from quest_start_scripts where id = 27438;
INSERT INTO `quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(27438, 3, 6, 0, 0, 0, -211.3203, 1292.4139, 42.3891, 5.55),
(27438, 2, 7, 27438, 0, 0, 0, 0, 0, 0);


-- https://forum.wowcircle.net/showthread.php?t=1064971
update creature_template_wdb set DisplayId1 = 15880, DisplayId2 = 0 where entry in (44889,44888,44890);

update creature_template set flags_extra = 128, unit_flags = 768, AIName = 'SmartAI', faction = 35 where entry in (44889,44888,44890);

update creature_template_addon set bytes1 = 0 where entry = 44835;

update creature set MovementType = 0, spawndist = 0 where id in (44888,44889,44890, 44835, 44886,44887,44885);

delete from smart_scripts where entryorguid in (44889,44888,44890);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(44888, 0, 0, 0, 10, 0, 100, 0, 0, 1, 5, 0, 0, 0, 85, 83745, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on ooc los - add aura'),
(44889, 0, 0, 0, 10, 0, 100, 0, 0, 1, 5, 0, 0, 0, 85, 83744, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on ooc los - add aura'),
(44890, 0, 0, 0, 10, 0, 100, 0, 0, 1, 5, 0, 0, 0, 85, 83746, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on ooc los - add aura');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` in (44889,44888,44890);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 44889, 0, 0, 9, 0, 27043, 0, 0, 0, 0, '', 'smart if quest incomplete'),
(22, 1, 44888, 0, 0, 9, 0, 27043, 0, 0, 0, 0, '', 'smart if quest incomplete'),
(22, 1, 44890, 0, 0, 9, 0, 27043, 0, 0, 0, 0, '', 'smart if quest incomplete'),

(22, 1, 44889, 0, 0, 48, 0, 266499, 0, 0, 1, 0, '', 'smart if objective incomplete'),
(22, 1, 44888, 0, 0, 48, 0, 266500, 0, 0, 1, 0, '', 'smart if objective incomplete'),
(22, 1, 44890, 0, 0, 48, 0, 266501, 0, 0, 1, 0, '', 'smart if objective incomplete');


-- https://forum.wowcircle.net/showthread.php?t=1064977
update quest_template set Flags = 0 where id = 27923;

delete from conditions where SourceEntry in (88646) and SourceTypeOrReferenceId = 17;
delete from conditions where SourceEntry in (88646) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 88646, 0, 0, 30, 0, 206684, 10, 0, 0, 0, '', 'cast only if near gameobject 206684'),
(17, 0, 88646, 0, 0, 30, 0, 206683, 10, 0, 1, 0, '', 'cast only if near no gameobject 206683'),

(17, 0, 88646, 0, 1, 30, 0, 206682, 10, 0, 0, 0, '', 'cast only if near gameobject 206682'),
(17, 0, 88646, 0, 1, 30, 0, 206683, 10, 0, 1, 0, '', 'cast only if near no gameobject 206683'),

(13, 1, 88646, 0, 0, 31, 0, 5, 206684, 0, 0, 0, '', 'cast into gameobject 206683'),
(13, 1, 88646, 0, 1, 31, 0, 5, 206682, 0, 0, 0, '', 'cast into gameobject 206683');


-- https://forum.wowcircle.net/showthread.php?t=1064980
update quest_template set ExclusiveGroup = -27923, NextQuestId = 28112 where Id in (27923,27924,28105);


-- https://forum.wowcircle.net/showthread.php?t=1065047
delete from quest_template_objective where QuestID = 37860;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(280840, 37860, 0, 0, 4, 100386, 1, 28, 0, 0, '', '', 21992),
(280839, 37860, 0, 0, 0, 100385, 1, 0, 0, 0, '', '', 21992),
(280838, 37860, 0, 0, 1, 100384, 1, 0, 0, 0, '', '', 21992),
(280837, 37860, 0, 0, 2, 100383, 1, 0, 0, 0, '', '', 21992),
(277088, 37860, 0, 0, 3, 90263, 1, 0, 0, 0, '', '', 21992);

delete from creature where id in (100385,90263,100384,100383, 993537,993536,993538);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(100385, 1220, 0, 0, 1, 1, 0, 0, 0, 1195.8806, 6064.2139, 72.4116, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(100385, 1220, 0, 0, 1, 1, 0, 0, 0, 1196.2770, 6058.8486, 68.8052, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(90263, 1220, 0, 0, 1, 1, 0, 0, 0, 1136.5002, 6110.2368, 76.7322, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(90263, 1220, 0, 0, 1, 1, 0, 0, 0, 1136.5032, 6115.1499, 78.2691, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(100383, 1220, 0, 0, 1, 1, 0, 0, 0, 1184.8317, 6142.0840, 84.1080, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(100383, 1220, 0, 0, 1, 1, 0, 0, 0, 1189.5060, 6144.3589, 81.7647, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(100384, 1220, 0, 0, 1, 1, 0, 0, 0, 1237.3136, 6167.2705, 83.9244, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(100384, 1220, 0, 0, 1, 1, 0, 0, 0, 1241.8268, 6170.1870, 84.3703, 1.33013, 300, 0, 0, 1035180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set AIName = 'SmartAI', flags_extra = 128, unit_flags = 768 where entry in (100385,90263,100384,100383);

delete from smart_scripts where entryorguid in (100385,90263,100384,100383);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(100385, 0, 0, 0, 8, 0, 100, 0, 0, 179825, 0, 0, 0, 0, 33, 100385, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on spell hit - give credit'),
(100384, 0, 0, 0, 8, 0, 100, 0, 0, 179825, 0, 0, 0, 0, 33, 100384, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on spell hit - give credit'),
(100383, 0, 0, 0, 8, 0, 100, 0, 0, 179825, 0, 0, 0, 0, 33, 100383, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on spell hit - give credit'),
(90263, 0, 0, 0, 8, 0, 100, 0, 0, 179825, 0, 0, 0, 0, 33, 90263, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on spell hit - give credit');


-- https://forum.wowcircle.net/showthread.php?t=1065229
delete from quest_poi where QuestID = 37257;
INSERT INTO `quest_poi` (`QuestID`, `BlobIndex`, `Idx1`, `ObjectiveIndex`, `QuestObjectiveID`, `QuestObjectID`, `MapID`, `WorldMapAreaId`, `Floor`, `Priority`, `Flags`, `WorldEffectID`, `PlayerConditionID`, `WoDUnk1`, `AlwaysAllowMergingBlobs`, `VerifiedBuild`) VALUES 
(37257, 0, 0, -1, 0, 0, 1220, 1015, 0, 0, 0, 0, 0, 959101, 0, 21414),
(37257, 0, 1, 0, 276232, 120080, 1220, 1015, 0, 0, 0, 0, 0, 0, 0, 21414),
(37257, 0, 2, 32, 0, 0, 1220, 1015, 0, 0, 0, 0, 0, 959101, 0, 21414);

delete from quest_poi_points where QuestID = 37257;
INSERT INTO `quest_poi_points` (`QuestID`, `Idx1`, `Idx2`, `X`, `Y`, `VerifiedBuild`) VALUES (37257, 0, 0, 17, 6663, 21414),
(37257, 1, 0, 23, 6360, 21414),
(37257, 1, 1, 153, 6400, 21414),
(37257, 1, 2, 309, 6455, 21414),
(37257, 1, 3, 331, 6505, 21414),
(37257, 1, 4, 331, 6525, 21414),
(37257, 1, 5, 298, 6556, 21414),
(37257, 1, 6, 213, 6566, 21414),
(37257, 1, 7, 124, 6566, 21414),
(37257, 1, 8, 15, 6564, 21414),
(37257, 1, 9, 18, 6470, 21414),
(37257, 2, 0, 17, 6663, 21414);

delete from quest_template_objective where QuestID = 37257;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(276232, 37257, 1, 0, 0, 120080, 55, 0, 0, 0, '', '', 21992);


-- https://forum.wowcircle.net/showthread.php?t=1065231
delete from quest_template_objective where QuestID = 37486;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(276767, 37486, 0, 0, 0, 88091, 1, 1, 0, 0, '', '', 21992),
(3748600, 37486, 0, 0, 1, 3748600, 1, 28, 0, 0, '', '', 21992);

delete from quest_poi where QuestID = 37486;
INSERT INTO `quest_poi` (`QuestID`, `BlobIndex`, `Idx1`, `ObjectiveIndex`, `QuestObjectiveID`, `QuestObjectID`, `MapID`, `WorldMapAreaId`, `Floor`, `Priority`, `Flags`, `WorldEffectID`, `PlayerConditionID`, `WoDUnk1`, `AlwaysAllowMergingBlobs`, `VerifiedBuild`) VALUES 
(37486, 0, 0, -1, 0, 0, 1220, 1015, 0, 0, 0, 0, 0, 964630, 0, 21414),
(37486, 0, 1, 0, 276767, 88091, 1220, 1015, 0, 0, 0, 0, 0, 969462, 0, 21414),
(37486, 0, 2, 32, 0, 0, 1220, 1015, 0, 0, 0, 0, 0, 964630, 0, 21414);

delete from quest_poi_points where QuestID = 37486;
INSERT INTO `quest_poi_points` (`QuestID`, `Idx1`, `Idx2`, `X`, `Y`, `VerifiedBuild`) VALUES 
(37486, 0, 0, -140, 6419, 21414),
(37486, 1, 0, -323, 6159, 21414),
(37486, 2, 0, -140, 6419, 21414);

delete from creature_questender where quest = 37486 and id != 88867;


-- https://forum.wowcircle.net/showthread.php?t=1065233	
delete from creature_questender where quest = 37497 and id != 88867;