-- https://forum.wowcircle.com/showthread.php?t=1050926
update creature_template_wdb set displayid1 = 62780, displayid2 = 0 where entry = 97939;

delete from smart_scripts where entryorguid in (97939,97940);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(97939, 0, 0, 0, 1, 0, 100, 0, 0, 0, 10000, 10000, 0, 12, 97940, 2, 30000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Portal - on ooc los - summon creature'),
(97940, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 53, 1, 97940, 0, 39990, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on just summoned - start waypoint'),
(97940, 0, 1, 0, 40, 0, 100, 0, 2, 97940, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Guard - on waypoint reached - despawn');

delete from conditions where SourceEntry in (97939) and SourceTypeOrReferenceId = 22;
-- INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
-- (22, 1, 97939, 0, 0, 9, 0, 39990, 0, 0, 0, 0, '', 'Smart if player has quest 39990');


-- https://forum.wowcircle.com/showthread.php?t=1052785
update creature_template set unit_flags = 768, faction = 7 where entry = 77737;

update quest_template set requiredraces = -1, prevquestid = 0 where id = 34351;
update quest_template set nextquestid = 34351 where id in (35249,34458);

update gameobject_template set flags = 0 where entry in (229034,229419,229420,229422,229027);


-- https://forum.wowcircle.com/showthread.php?t=1052792
DELETE FROM `creature_queststarter` WHERE quest in (35035,35025,35248);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(81202, 35025),
(81202, 35035),
(81202, 35248);


-- https://forum.wowcircle.com/showthread.php?t=1052365
delete from npc_spellclick_spells where npc_entry in (106619,106618);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(106618, 211450, 0, 0),
(106619, 211454, 0, 0);

delete from conditions where SourceGroup in (106618,106619) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 106618, 211450, 0, 0, 9, 0, 42517, 0, 0, 0, 0, '', 'Allow spellclick if player has Quest'),

(18, 106619, 211454, 0, 0, 9, 0, 42517, 0, 0, 0, 0, '', 'Allow spellclick if player has Quest'),
(18, 106619, 211454, 0, 1, 28, 0, 42517, 0, 0, 0, 0, '', 'Allow spellclick if player complete Quest');

update creature_template set ainame = 'SmartAI' where entry in (106618,106619);

delete from spell_target_position where id in (211454,211450);
INSERT INTO `spell_target_position` (`id`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES 
(211450, 1107, 3466.0815, 1497.8081, 436.6305, 2.2),
(211454, 1107, 3162.2234, 993.8894, 247.2272, 3.5);

delete from spell_script_names where spell_id = 224235;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(224235, 'spell_ritual_of_doom');

update creature_template set ainame = 'SmartAI', minlevel = 110, maxlevel = 110, faction = 14 where entry in (112387, 112381, 112388, 112379, 112377, 112385, 112396, 112399, 112378, 112389, 112395, 112398, 112384, 112376, 112394, 112391, 112375, 112374, 112380, 112397);

update quest_template_objective set amount = 0 where questid = 42517 and `type` = 14;

update creature_template set npcflag = 16777216, unit_flags = 768, unit_flags2 = 0 where entry in (106618,106619);

delete from smart_scripts where entryorguid in (106618,106619, 112387, 112381, 112388, 112379, 112377, 112385, 112396, 112399, 112378, 112389, 112395, 112398, 112384, 112376, 112394, 112391, 112375, 112374, 112380, 112397);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(106618, 0, 0, 0, 73, 0, 100, 0, 0, 0, 0, 0, 0, 62, 1107, 0, 0, 0, 0, 0, 7, 0, 0, 0, 3466.0815, 1497.808, 436.6305, 2.2, 'Portal - on spellclick - teleport'),
(106619, 0, 0, 0, 73, 0, 100, 0, 0, 0, 0, 0, 0, 62, 1107, 0, 0, 0, 0, 0, 7, 0, 0, 0, 3162.2234, 993.8894, 247.2272, 3.5, 'Portal - on spellclick - teleport'),

(112387, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112381, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112381, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112388, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112388, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112379, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112379, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112377, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112377, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112385, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112385, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112396, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112396, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112399, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112399, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112378, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112378, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112389, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112389, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112395, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112395, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112398, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112398, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112384, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112384, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112376, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112376, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112394, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112394, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112391, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112391, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112375, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112375, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112374, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112374, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112380, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112380, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit'),

(112397, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Demon - on just summoned - attack'),
(112397, 0, 1, 0, 6, 0, 100, 1, 0, 0, 0, 0, 0, 33, 108018, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'Demon - on death - give credit');

delete from creature where id = 106619;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(106619, 1107, 0, 0, 1, 1, 0, 0, 0, 3477.47, 1484.86, 434.857, 2.01853, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1053165
delete from gameobject_queststarter where quest = 11790;


-- https://forum.wowcircle.com/showthread.php?t=1053167
update gameobject_template set scriptname = '' where entry = 191229;


-- https://forum.wowcircle.com/showthread.php?t=1052675
DELETE FROM `creature_questender` WHERE quest in (35277);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(81920, 35277);

DELETE FROM `creature_queststarter` WHERE quest in (35277);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(81920, 35277);

delete from gameobject where id in (232062);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
(232062, 1116, 6722, 7197, 1, 1, 0, -368.5286, 2247.3320, 26.3475, 3.49, 0, 0, 0, 0, 120, 255, 1);

delete from playerchoice_response_reward_spellcast where choiceid = 57 and responseid in (131,132);
INSERT INTO `playerchoice_response_reward_spellcast` (`ChoiceId`, `ResponseId`, `Index`, `SpellId`, `VerifiedBuild`) VALUES 
(57, 131, 0, 164981, 26365), -- tavern
(57, 132, 0, 164983, 26365); -- trading post

DELETE FROM `spell_scene` WHERE MiscValue in (784);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1354, 784, 5);

delete from phase_definitions where zoneId = 6722 and entry in (35283,35284);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6722, 35283, 0, 35283, 0, '35283 quest'),
(6722, 35284, 0, 35284, 0, '35284 quest');

delete from conditions where SourceGroup in (6722) and SourceTypeOrReferenceId = 26 and sourceentry in (35283,35284);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6722, 35283, 0, 0, 14, 0, 35283, 0, 0, 1, 0, '', 'Phase If quest 35283 not none'),
(26, 6722, 35284, 0, 0, 14, 0, 35284, 0, 0, 1, 0, '', 'Phase If quest 35284 not none');

update gameobject set phasemask = 0, phaseid = 35283 where id = 232384;
update gameobject set phasemask = 0, phaseid = 35284 where id = 232385;

delete from creature where id IN ('89226', '89225', '88554', '84292', '88581', '88513', '88515', '82602', '88551', '88549', '88533', '88541');
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(82602, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -337.613, 2266.47, 28.3484, 4.37159, 120, 0, 0, 1, 0, 0, 1, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(84292, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -338.59, 2249.96, 28.3503, 2.03643, 120, 0, 0, 1, 0, 0, 2, 0, 33024, 2048, 0, 0, 0, 0, '', '', -1),
(88513, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -331.837, 2258.33, 28.2749, 2.0342, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88515, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -333.516, 2261.78, 28.311, 5.13666, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88533, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -345.14, 2259.67, 15.4575, 1.58512, 120, 0, 0, 1, 0, 0, 128, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88541, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -340.244, 2260.07, 15.4579, 3.94965, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88549, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -334.527, 2262.39, 39.8504, 5.10807, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88549, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -334.287, 2256.04, 39.7181, 4.37463, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88551, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -347.426, 2262.51, 28.3451, 4.23853, 120, 0, 0, 1, 0, 0, 0, 0, 536904448, 2048, 0, 0, 0, 0, '', '', -1),
(88551, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -355.451, 2243.88, 15.5131, 3.47642, 120, 0, 0, 1, 0, 0, 0, 0, 536904448, 2048, 0, 0, 0, 0, '', '', -1),
(88554, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -339.753, 2252.75, 28.49, 5.1372, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88554, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -342.014, 2251.52, 28.4531, 5.0101, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88554, 1116, 6722, 7197, 1, 0, 35283, 0, 0, -335.778, 2256.87, 37.3563, 2.74094, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(88581, 1116, 6722, 7197, 1, 0, 35283, 0, 1, -333.763, 2253.01, 28.3153, 2.47476, 120, 0, 0, 1, 0, 0, 0, 0, 33536, 2048, 0, 0, 0, 0, '', '', -1),
(89225, 1116, 6722, 7197, 1, 0, 35283, 61571, 0, -351.833, 2251.62, 27.8236, 5.42159, 300, 0, 0, 111657, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(89226, 1116, 6722, 7197, 1, 0, 35283, 61574, 0, -357.611, 2253.98, 27.7592, 2.45448, 300, 0, 0, 111657, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.com/showthread.php?t=1053332
update gameobject_template set data10 = 0 where entry = 231229; -- 163428
update gameobject_template set data10 = 0 where entry = 233046; -- 166365

delete from spell_area where spell in (163428,166365);
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(166365, 7149, 0, 0, 0, 0, 2, 0, 0, 0),
(163428, 7149, 0, 0, 0, 0, 2, 0, 0, 0);

update quest_template set flags = 0 where id = 35040;

DELETE FROM `creature_questender` WHERE quest in (35040);
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(82732, 35040);