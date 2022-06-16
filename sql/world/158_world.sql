-- https://forum.wowcircle.net/showthread.php?t=1063323
update quest_template set PrevQuestId = 33335 where Id = 33340;
update quest_template set PrevQuestId = 33340 where Id = 33341;
update quest_template set PrevQuestId = 33341 where Id = 33342;
update quest_template set PrevQuestId = 33342 where Id = 33343;


-- https://forum.wowcircle.net/showthread.php?t=1063352
delete from locales_quest_template_objective where ID in (88680102) and locale = 8;
INSERT INTO `locales_quest_template_objective` (`ID`, `Locale`, `Description`, `VerifiedBuild`) VALUES 
(88680102, 8, 'Победите Даргрула Короля подземелий', 26124);


-- https://forum.wowcircle.net/showthread.php?t=1063357
delete from phase_definitions where zoneId = 7503 and entry in (39661,39498,42104,39025);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7503, 39025, 0, 39025, 0, '39025 quest'),
(7503, 42104, 0, 42104, 0, '42104 quest'),
(7503, 39661, 0, 39661, 0, '39661 quest'),
(7503, 39498, 0, 39498, 0, '39498 quest');

delete from conditions where SourceGroup in (7503) and SourceTypeOrReferenceId = 26 and sourceentry in (39661,39498,42104,39025);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7503, 39025, 0, 0, 8, 0, 39025, 0, 0, 0, 0, '', 'phase if quest 42104 reward'),
(26, 7503, 39025, 0, 1, 28, 0, 39025, 0, 0, 0, 0, '', 'phase if quest 42104 reward'),

(26, 7503, 42104, 0, 0, 8, 0, 42104, 0, 0, 0, 0, '', 'phase if quest 42104 reward'),

(26, 7503, 39498, 0, 0, 14, 0, 39498, 0, 0, 1, 0, '', 'phase if quest 39661 not none'),
(26, 7503, 39498, 0, 0, 8, 0, 42104, 0, 0, 1, 0, '', 'phase if quest 42104 not reward'),

(26, 7503, 39661, 0, 0, 14, 0, 39661, 0, 0, 1, 0, '', 'phase if quest 39661 not none'),
(26, 7503, 39661, 0, 0, 14, 0, 39488, 0, 0, 0, 0, '', 'phase if quest 39488 none');

delete from creature_template_addon where entry = 97662;

update creature set phaseMask = 0, phaseID = 39498 where id = 97662;

update creature set phaseMask = 0, phaseID = 39661 where id = 96520;

update creature set phaseMask = 0, phaseID = 39498 where id = 97871;

update creature set phaseMask = 0, phaseID = 39498 where guid = 11851663;

update creature set phaseMask = 0, phaseID = 39498 where guid = 18356239;

update quest_template set PrevQuestId = 39487, InProgressPhaseID = 0, CompletedPhaseID = 0 where Id = 39498;

DELETE FROM `spell_scene` WHERE MiscValue in (1092);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`, `TrigerSpell`, `MonsterCredit`, `ScriptName`, `Comment`) VALUES
(1477, 1092, 0, 0, 0, '', '');

update creature set phaseMask = 0, phaseID = 42104 where guid in (18355914);

update creature set phaseMask = 0, phaseID = 39498 where guid in (18355912);

update quest_template set CompletedPhaseID = 0, InProgressPhaseID = 0 where Id = 42104;


-- https://forum.wowcircle.net/showthread.php?t=1063360
update creature set phaseMask = 0, phaseID = 39025 where guid in (18355929,18355800,11851364);

update creature set id = 97553 where id in (975531);

update quest_template set CompletedPhaseID = 0, InProgressPhaseID = 0 where Id = 39025;


-- https://forum.wowcircle.net/showthread.php?t=1063402
update quest_template set InProgressPhaseID = Id where Id = 30321;

update quest_template set InProgressPhaseID = 30321 where Id = 30052;

update creature set phaseMask = 0, phaseID = 30321 where id in (57308,57306);

update gameobject set phaseMask = 0, phaseID = 30321 where id in (210056, 210057);

update creature_template set AIName = 'SmartAI', npcflag = 16777216, faction = 7, unit_flags = 768 where entry in (57308,57306);

delete from smart_scripts where entryorguid in (57308, 57306);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(57308, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 0, 103, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'weed - on update - set root'),
(57308, 0, 1, 3, 8, 0, 100, 1, 0, 108229, 0, 0, 0, 0, 33, 59524, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'weed - on spell hit - give credit'),
(57308, 0, 2, 3, 8, 0, 100, 1, 0, 108157, 0, 0, 0, 0, 33, 59524, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'weed - on spell hit - give credit'),
(57308, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 33, 57358, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'weed - on spell hit - give credit'),
(57308, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'weed - on link - die'),

(57306, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 0, 103, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'weed - on update - set root'),
(57306, 0, 1, 3, 8, 0, 100, 1, 0, 108229, 0, 0, 0, 0, 33, 59524, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'weed - on spell hit - give credit'),
(57306, 0, 2, 3, 8, 0, 100, 1, 0, 108157, 0, 0, 0, 0, 33, 59524, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'weed - on spell hit - give credit'),
(57306, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 33, 57358, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'weed - on spell hit - give credit'),
(57306, 0, 4, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'weed - on link - die');

delete from spell_linked_spell where spell_trigger in (108229) and spell_effect in (-108237);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(108229, -108237, 0, 0, 0, 0, 0, '');

delete from npc_spellclick_spells where npc_entry in (57308,57306);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(57306, 108157, 1, 0),
(57308, 108157, 1, 0);


-- https://forum.wowcircle.net/showthread.php?t=1063417
delete from gameobject where guid in (106382,106383);

delete from phase_definitions where zoneid in (6134);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
(6134, 1, 0, 0, 1061, 0, 0, 'Terrain Swap Phase Horde'),
(6134, 2, 0, 0, 1062, 0, 0, 'Terrain Swap Phase Aliance');

delete from conditions where SourceGroup in (6134) and SourceTypeOrReferenceId = 26 and sourceentry in (1,2);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6134, 1, 0, 0, 8, 0, 32108, 0, 0, 0, 0, '', 'phase if quest 32108 reward'),
(26, 6134, 1, 0, 1, 8, 0, 32109, 0, 0, 0, 0, '', 'phase if quest 32109 reward'),

(26, 6134, 2, 0, 0, 8, 0, 32108, 0, 0, 0, 0, '', 'phase if quest 32108 reward'),
(26, 6134, 2, 0, 1, 8, 0, 32109, 0, 0, 0, 0, '', 'phase if quest 32109 reward');

update quest_template set PrevQuestId = 32249, NextQuestIdChain = 0, NextQuestId = 0 where Id = 32250;
update quest_template set PrevQuestId = 32250, NextQuestIdChain = 0, NextQuestId = 0 where Id = 32108;

update creature_template set npcflag = 16777216 where entry in (67927,67926);

delete from npc_spellclick_spells where npc_entry in (67927,67926);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(67927, 134041, 1, 0),
(67926, 134049, 1, 0);

delete from conditions where SourceGroup in (67927,67926) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 67927, 134041, 0, 0, 9, 0, 32250, 0, 0, 0, 0, '', 'spellclick if quest incomplete'),
(18, 67926, 134049, 0, 0, 9, 0, 32250, 0, 0, 0, 0, '', 'spellclick if quest incomplete');

update creature set npcflag = 0 where id in (67927,67926);

DELETE FROM `spell_area` WHERE `spell` in (134064) and area = 6612;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(134064, 6612, 32250, 32108, 0, 0, 2, 1, 74, 74);


-- https://forum.wowcircle.net/showthread.php?t=1063659
update gameobject_template set flags = 0 where entry in (211510);

update creature_template set unit_flags = 768, AIName = 'SmartAI' where entry = 60694;

delete from smart_scripts where entryorguid in (60694);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(60694, 0, 0, 0, 8, 0, 100, 0, 0, 117472, 0, 0, 0, 0, 33, 60720, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'lao - on spell hit - give credit');

update creature set phaseMask = 0, phaseID = 307940 where id = 60694;
update creature set phaseMask = 0, phaseID = 307941 where id = 60785;

delete from phase_definitions where zoneid in (5841);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
(5841, 307940, 0, 307940, 0, 0, 0, '30794 quest'),
(5841, 307941, 0, 307941, 0, 0, 0, '30794 quest');

delete from conditions where SourceGroup in (5841) and SourceTypeOrReferenceId = 26 and sourceentry in (307940,307941);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 5841, 307940, 0, 0, 14, 0, 30794, 0, 0, 0, 0, '', 'phase if quest 30794 none'),
(26, 5841, 307940, 0, 1, 9, 0, 30794, 0, 0, 0, 0, '', 'phase if quest 30794 incomplete'),

(26, 5841, 307941, 0, 0, 28, 0, 30794, 0, 0, 0, 0, '', 'phase if quest 30794 complete'),
(26, 5841, 307941, 0, 1, 8, 0, 30794, 0, 0, 0, 0, '', 'phase if quest 30794 reward');

DELETE FROM `creature_questender` WHERE quest in (30794);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(60785, 30794);

delete from conditions where SourceEntry in (117472) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 3, 117472, 0, 0, 31, 0, 3, 60694, 0, 0, 0, '', 'cast only 60694');


-- https://forum.wowcircle.net/showthread.php?t=1063663
update quest_template set InProgressPhaseID = Id  where Id = 31011;

delete from creature where id in (62299,62362,63516);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4360.33, 918.883, 109.073, 0.210422, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4357.87, 906, 109.035, 5.53706, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4430.1, 878.092, 107.811, 5.60497, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43067, 0, 4436.52, 937.899, 99.125, 2.34297, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43067, 0, 4373.09, 920.909, 108.44, 1.95747, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4372.45, 910.867, 108.011, 0.388478, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4393.06, 936.978, 116.015, 1.58106, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43067, 0, 4377.05, 936.856, 107.874, 4.17866, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4443.75, 960.17, 102.994, 3.16108, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4425.19, 956.456, 104.14, 0.956204, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4290.79, 920.497, 120.371, 0.932598, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43067, 0, 4374.1, 966.834, 111.07, 4.85774, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43066, 0, 4280.35, 878.71, 122.25, 2.54032, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43067, 0, 4395.77, 901.594, 105.127, 5.44466, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62299, 870, 5841, 6169, 1, 0, 31011, 43067, 0, 4353.71, 943.759, 112.437, 2.54032, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62362, 870, 5841, 6169, 1, 0, 31011, 44803, 0, 4409.15, 808.243, 92.1389, 1.76497, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62362, 870, 5841, 6169, 1, 0, 31011, 44804, 0, 4406.65, 808.81, 92.9187, 2.35448, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62362, 870, 5841, 6169, 1, 0, 31011, 44803, 0, 4421.37, 801.713, 87.169, 2.82007, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62362, 870, 5841, 6169, 1, 0, 31011, 44804, 0, 4457.55, 735.696, 54.1516, 1.77445, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(62362, 870, 5841, 6169, 1, 0, 31011, 44804, 0, 4452.82, 738.129, 55.6521, 1.77445, 300, 4, 0, 271376, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(63516, 870, 5841, 6169, 1, 0, 31011, 25769, 0, 4447.69, 1009.86, 81.4319, 0.674406, 300, 4, 0, 814128, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(63516, 870, 5841, 6169, 1, 0, 31011, 25769, 0, 4422.86, 1020.1, 92.6296, 2.85622, 300, 4, 0, 814128, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(63516, 870, 5841, 6169, 1, 0, 31011, 25769, 0, 4411.86, 1019.27, 95.019, 1.94066, 300, 4, 0, 814128, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(63516, 870, 5841, 6169, 1, 0, 31011, 25769, 0, 4492, 1003.69, 57.6816, 3.23013, 300, 0, 0, 814128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);