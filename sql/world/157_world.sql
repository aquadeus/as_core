-- https://forum.wowcircle.net/showthread.php?t=1060389
update quest_template set InProgressPhaseID = Id where Id = 42630;
 

-- https://forum.wowcircle.net/showthread.php?t=1062761
delete from quest_poi where QuestID = 41179;
INSERT INTO `quest_poi` (`QuestID`, `BlobIndex`, `Idx1`, `ObjectiveIndex`, `QuestObjectiveID`, `QuestObjectID`, `MapID`, `WorldMapAreaId`, `Floor`, `Priority`, `Flags`, `WorldEffectID`, `PlayerConditionID`, `WoDUnk1`, `AlwaysAllowMergingBlobs`, `VerifiedBuild`) VALUES 
(41179, 0, 2, 32, 0, 0, 1220, 1018, 0, 0, 0, 0, 0, 1128905, 0, 21414),
(41179, 0, 1, 0, 282262, 104003, 1220, 1018, 0, 0, 0, 0, 0, 0, 0, 21414),
(41179, 0, 0, -1, 0, 0, 1220, 1018, 0, 0, 0, 0, 0, 1133826, 0, 21414);

delete from quest_poi_points where QuestID = 41179;
INSERT INTO `quest_poi_points` (`QuestID`, `Idx1`, `Idx2`, `X`, `Y`, `VerifiedBuild`) VALUES 
(41179, 0, 0, 3156, 6885, 21414),
(41179, 1, 0, 3175, 6878, 21414),
(41179, 1, 1, 3164, 6909, 21414),
(41179, 1, 2, 3139, 6905, 21414),
(41179, 1, 3, 3138, 6881, 21414),
(41179, 2, 0, 1896, 7390, 21414);

delete from quest_template_objective where QuestID = 41179;
delete from quest_template_objective where QuestID = 12788;
INSERT INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(282262, 41179, 0, 0, 1, 104003, 1, 0, 0, 0, 'Find the lost artifact', NULL, 24330),
(282261, 41179, 0, 0, 0, 104002, 5, 12, 0, 0, 'Dig Through Bad Rocks', NULL, 24330);

delete from locales_quest_template_objective where ID in (282262,282261) and locale = 8;
INSERT INTO `locales_quest_template_objective` (`ID`, `Locale`, `Description`, `VerifiedBuild`) VALUES 
(282262, 8, 'Найдите пропавший артефакт', 26124),
(282261, 8, 'Пробиться через Дурные камни', 26124);

delete from quest_template_locale where ID = 41179 and locale = 'ruRU';
INSERT INTO `quest_template_locale` (`ID`, `locale`, `LogTitle`, `LogDescription`, `QuestDescription`, `AreaDescription`, `PortraitGiverText`, `PortraitGiverName`, `PortraitTurnInText`, `PortraitTurnInName`, `QuestCompletionLog`, `OfferRewardText`, `VerifiedBuild`) VALUES 
(41179, 'ruRU', 'Некогда утерянное', 'Отправьтесь в место, обозначенное на карте, и разыщите там артефакт. Воспользуйтесь динамитом, чтобы высвободить артефакт.', 'Да, кстати, только вспомнил – у меня есть карта одного места, где, возможно, хранится нечто потенциально весьма ценное.

Вроде бы ничего, что обладает магической силой, там нет, и поэтому я и решил поручить это дело тебе.

Вот, возьми еще взрывчатки. Она поможет тебе ускорить поиски.', '', '', '', '', '', '', '<В маленьком сундучке вы нашли коробку с выгравированной на ней головой ворона.>', 26124);

update item_loot_template set groupid = 0, ChanceOrQuestChance = 100 where item = 131745 and entry = 136383;


-- https://forum.wowcircle.net/showthread.php?t=1062822
DELETE FROM `spell_scene` WHERE MiscValue in (580);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`, `TrigerSpell`, `MonsterCredit`, `ScriptName`, `Comment`) VALUES
(773, 580, 0, 0, 0, '', '');

DELETE FROM `spell_area` WHERE `spell` in (157509) and area = 7013;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(157509, 7013, 34381, 34319, 0, 0, 2, 1, 66, 64);

delete from creature where id in (78003,78428,78792);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(78003, 1116, 6720, 7013, 1, 0, 343810, 0, 1, 7315.76, 5099.91, 128.847, 4.76485, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(78428, 1116, 6720, 7013, 1, 0, 343811, 0, 0, 7317.89, 5090.59, 128.34, 3.46606, 300, 0, 0, 299132, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(78792, 1116, 6720, 7013, 1, 0, 343811, 54692, 0, 7312.97, 5088.8, 128.933, 0, 300, 0, 0, 489270, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update quest_template set InProgressPhaseID = Id * 10, CompletedPhaseID = 0, SourceSpellId = 158051, Flags = 0 where Id = 34381;

delete from phase_definitions where zoneId = 6720 and entry in (343811);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6720, 343811, 0, 343811, 0, '34381 quest');

delete from conditions where SourceGroup in (6720) and SourceTypeOrReferenceId = 26 and sourceentry in (343811);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6720, 343811, 0, 0, 8, 0, 34319, 0, 0, 1, 0, '', 'phase if quest 34319 not reward'),
(26, 6720, 343811, 0, 0, 8, 0, 34381, 0, 0, 0, 0, '', 'phase if quest 34381 reward'),
(26, 6720, 343811, 0, 1, 28, 0, 34381, 0, 0, 0, 0, '', 'phase if quest 34381 complete');

DELETE FROM `creature_queststarter` WHERE quest in (34318,34319);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(78428, 34318),
(78428, 34319);

DELETE FROM `creature_questender` WHERE quest in (34318,34319);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(78428, 34318),
(78821, 34319);

update quest_template set PrevQuestId = 34381 where Id = 34318;

update quest_template set PrevQuestId = 34318 where Id = 34319;


-- https://forum.wowcircle.net/showthread.php?t=1062823
update creature_template set AIName = 'SmartAI', flags_extra = 128 where entry = 78252;

delete from smart_scripts where entryorguid in (78252);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(78252, 0, 0, 0, 10, 0, 100, 0, 1, 20, 0, 0, 0, 33, 78252, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on ooc los - give credit');

delete from creature where id = 78252;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(78252, 1116, 6720, 7013, 1, 1, 0, 0, 0, 7375.17, 5051.21, 130.689, 5.67745, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` in (78252);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 78252, 0, 0, 9, 0, 34316, 0, 0, 0, 0, '', 'smart if quest incomplete'),
(22, 1, 78252, 0, 0, 48, 0, 272750, 0, 0, 1, 0, '', 'smart if criteria incomplete');


-- https://forum.wowcircle.net/showthread.php?t=1062840
update quest_template set Flags = 0 where id = 34402;

delete from creature where id = 78407;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(78407, 1116, 6720, 7004, 1, 4294967295, 0, 0, 0, 5623.11, 4526.72, 119.269, 0.0132985, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set AIName = 'SmartAI', flags_extra = 128 where entry = 78407;

delete from smart_scripts where entryorguid in (78272, 78407);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(78407, 0, 0, 0, 10, 0, 100, 0, 0, 1, 20, 0, 0, 0, 33, 78407, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'trigger - on ooc los - give credit'),

(78272, 0, 0, 0, 10, 0, 100, 0, 0, 1, 10, 100, 200, 0, 33, 78272, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Target->KC 78272'),
(78272, 0, 1, 2, 10, 0, 100, 0, 0, 1, 10, 8000, 9000, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Target->Text 0'),
(78272, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 0, 3, 0, 0, 0, 0, 11, 76411, 10, 0, 0, 0, 0, 0, 'Link->Force NPC 76411'),
(78272, 0, 3, 0, 19, 0, 100, 0, 0, 34402, 0, 0, 0, 0, 1004, 778, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'durotan - on quest accept - play scene');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` in (78407);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 78407, 0, 0, 9, 0, 34402, 0, 0, 0, 0, '', 'smart if quest incomplete');


-- https://forum.wowcircle.net/showthread.php?t=1062851
update quest_template set PrevQuestId = 33332 where Id = 33333;
update quest_template set PrevQuestId = 33333 where Id = 33334;


-- https://forum.wowcircle.net/showthread.php?t=1062853
update quest_template set PrevQuestId = 33161 where Id = 33336;
update quest_template set PrevQuestId = 33336 where Id = 33338;


-- https://forum.wowcircle.net/showthread.php?t=1062932
DELETE FROM `creature_questender` WHERE quest in (41069);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(95240, 41069);


-- https://forum.wowcircle.net/showthread.php?t=1062939
DELETE FROM `creature_queststarter` WHERE quest in (41804);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(90417, 41804);

update quest_template set NextQuestId = 41804 where id in (41803,40247);


-- https://forum.wowcircle.net/showthread.php?t=1062957
delete from creature where guid = 18357032;

DELETE FROM `spell_area` WHERE `spell` in (190369) and area = 7786;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(190369, 7786, 39488, 39498, 0, 0, 2, 1, 74, 74);

DELETE FROM `creature_queststarter` WHERE quest in (39488,39487,39498);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(96520, 39488),
(96038, 39487),
(96038, 39498);

DELETE FROM `creature_questender` WHERE quest in (39488,39487,39498);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(96038, 39488),
(96038, 39487),
(97662, 39498);

delete from phase_definitions where zoneId = 7503 and entry in (39661);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7503, 39661, 0, 39661, 0, '39661 quest');

delete from conditions where SourceGroup in (7503) and SourceTypeOrReferenceId = 26 and sourceentry in (39661);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7503, 39661, 0, 0, 14, 0, 39661, 0, 0, 1, 0, '', 'phase if quest 39661 not none'),
(26, 7503, 39661, 0, 0, 14, 0, 39488, 0, 0, 0, 0, '', 'phase if quest 39488 none');

delete from creature_template_addon where entry = 97662;

update creature set phaseMask = 0, phaseID = 39498 where id = 97662;

update creature set phaseMask = 0, phaseID = 39661 where id = 96520;

update quest_template set PrevQuestId = 39661 where Id = 39488;

update quest_template set PrevQuestId = 0 where Id = 39487;

update quest_template set PrevQuestId = 39487, InProgressPhaseID = ID, CompletedPhaseID = ID where Id = 39498;

delete from creature where id in (96038,965201);