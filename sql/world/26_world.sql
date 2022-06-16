-- https://forum.wowcircle.net/showthread.php?t=800019
delete from gameobject where id = 204343;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(204343, 646, 5042, 5330, 1, 1, 0, 502.102, 1649.45, 352.87, 2.6876, 0, 0, 0.974347, 0.22505, 180, 255, 0, 0, '', 0, -1),
(204343, 646, 5042, 5330, 1, 1, 0, 454.102, 1745.76, 353.297, 0.445601, 0, 0, 0.220962, 0.975282, 180, 255, 0, 0, '', 0, -1),
(204343, 646, 5042, 5330, 1, 1, 0, 410.243, 1728.32, 352.633, 0.82652, 0, 0, 0.401597, 0.915816, 180, 255, 0, 0, '', 0, -1),
(204343, 646, 5042, 5330, 1, 1, 0, 403.686, 1650.66, 350.858, -1.36306, 0, 0, -0.629981, 0.77661, 180, 255, 0, 0, '', 0, -1);

delete from creature where id in (43164,43165,43166,43167,43170) and areaid = 5330;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(43170, 646, 5042, 5330, 1, 1, 0, 26093, 1, 411.33, 1659.2, 348.967, 6.07376, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43170, 646, 5042, 5330, 1, 1, 0, 26093, 1, 457.47, 1727.42, 348.597, 5.16318, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43170, 646, 5042, 5330, 1, 1, 0, 26093, 1, 491.014, 1659.59, 348.37, 1.61631, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43170, 646, 5042, 5330, 1, 1, 0, 26093, 1, 420.792, 1718.1, 349.576, 0.312717, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),

(43164, 646, 5042, 5330, 1, 1, 0, 0, 1, 502.102, 1649.45, 352.87, 2.6876, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43165, 646, 5042, 5330, 1, 1, 0, 0, 1, 454.102, 1745.76, 353.297, 0.445, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43166, 646, 5042, 5330, 1, 1, 0, 0, 1, 410.243, 1728.32, 352.633, 0.826, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(43167, 646, 5042, 5330, 1, 1, 0, 0, 1, 403.686, 1650.66, 350.858, -1.36, 300, 0, 0, 44679, 8338, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update creature_template set ainame = 'SmartAI', unit_flags = 0 where entry = 43170;
update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry in (43164,43165,43166,43167);

delete from smart_scripts where entryorguid in (43164,43165,43166,43167,43170);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(43164, 0, 0, 1, 38, 0, 100, 0, 0, 1, 0, 0, 0, 33, 43164, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Trigger - on set data - give credit'),
(43165, 0, 0, 1, 38, 0, 100, 0, 0, 1, 0, 0, 0, 33, 43165, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Trigger - on set data - give credit'),
(43166, 0, 0, 1, 38, 0, 100, 0, 0, 1, 0, 0, 0, 33, 43166, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Trigger - on set data - give credit'),
(43167, 0, 0, 1, 38, 0, 100, 0, 0, 1, 0, 0, 0, 33, 43167, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Trigger - on set data - give credit'),

(43164, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 20, 204343, 20, 0, 0, 0, 0, 0, 'Trigger - on link - activate gameobject'),
(43165, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 20, 204343, 20, 0, 0, 0, 0, 0, 'Trigger - on link - activate gameobject'),
(43166, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 20, 204343, 20, 0, 0, 0, 0, 0, 'Trigger - on link - activate gameobject'),
(43167, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 20, 204343, 20, 0, 0, 0, 0, 0, 'Trigger - on link - activate gameobject'),

(43170, 0, 0, 1, 8, 0, 100, 0, 80675, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 11, 0, 30, 0, 0, 0, 0, 0, 'Geomancer - on spell hit - set data'),
(43170, 0, 1, 0, 61, 0, 100, 0, 80675, 0, 0, 0, 0, 28, 82526, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Geomancer - on link - remove aura');

delete from conditions where SourceEntry in (80675) and SourceTypeOrReferenceId = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 80675, 0, 0, 31, 1, 3, 43170, 0, 0, 0, '', 'Cast only 43170');


-- https://forum.wowcircle.net/showthread.php?t=800229
update gameobject_template set ainame = 'SmartGameObjectAI' where entry = 206683;

delete from smart_scripts where entryorguid in (206683);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(206683, 1, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 1014, 206684, 1, 2, 0, 0, 0, 18, 10, 0, 0, 0, 0, 0, 0, 'Trigger - on set data - give credit');

delete from gameobject where id in (206684,206682);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(206684, 1, 5034, 5599, 1, 1, 0, -11082.7, -1407.14, 10.8068, 1.76278, 0, 0, 0.771625, 0.636078, 300, 100, 1, 0, '', 0, -1),
(206684, 1, 5034, 5599, 1, 1, 0, -11135.2, -1382.03, 10.8073, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206684, 1, 5034, 5599, 1, 1, 0, -11105.9, -1312.27, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206684, 1, 5034, 5599, 1, 1, 0, -11080.2, -1314.31, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206684, 1, 5034, 5599, 1, 1, 0, -11075.6, -1333.9, 10.8068, 1.01229, 0, 0, 0.484809, 0.87462, 300, 100, 1, 0, '', 0, -1),
(206684, 1, 5034, 5599, 1, 1, 0, -11041.4, -1375.85, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206684, 1, 5034, 5599, 1, 1, 0, -11145, -1449.7, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206684, 1, 5034, 5599, 1, 1, 0, -11102.6, -1422.79, 10.8068, 2.56563, 0, 0, 0.958819, 0.284016, 300, 100, 1, 0, '', 0, -1),
(206684, 1, 5034, 5599, 1, 1, 0, -11025.3, -1397.11, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11145, -1449.7, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11135.2, -1382.03, 10.8073, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11104.7, -1312.27, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11102.6, -1422.79, 10.8068, 2.56563, 0, 0, 0.958819, 0.284016, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11075.6, -1333.9, 10.8068, 1.01229, 0, 0, 0.484809, 0.87462, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11080.2, -1314.31, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11082.7, -1407.14, 10.8068, 1.76278, 0, 0, 0.771625, 0.636078, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11025.3, -1397.11, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1),
(206682, 1, 5034, 5599, 1, 1, 0, -11041.4, -1375.85, 10.8068, 1.44862, 0, 0, 0.66262, 0.748956, 300, 100, 1, 0, '', 0, -1);

delete from conditions where SourceEntry in (88646) and SourceTypeOrReferenceId = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 88646, 0, 0, 30, 0, 206684, 10, 0, 0, 0, '', 'Cast only if near gameobject 206684');


-- https://forum.wowcircle.net/showthread.php?t=799892
delete from phase_definitions where zoneId = 6662 and entry in (34568,34567,34558);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(6662, 34568, 0, 34568, 0, 'Armory outpost'),
(6662, 34567, 0, 34567, 0, 'Mage tower'),
(6662, 34558, 0, 34558, 0, 'Before 34558 quest');

delete from conditions where SourceGroup in (6662) and SourceTypeOrReferenceId = 26 and sourceentry in (34568,34567,34558);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 6662, 34568, 0, 1, 8, 0, 34568, 0, 0, 0, 0, '', 'Activate Phase if player reward quest 34568'),
(26, 6662, 34568, 0, 1, 8, 0, 34558, 0, 0, 0, 0, '', 'Activate Phase if player reward quest 34558'),
(26, 6662, 34568, 0, 0, 8, 0, 34568, 0, 0, 0, 0, '', 'Activate Phase if player reward quest 34568'),
(26, 6662, 34568, 0, 0, 28, 0, 34558, 0, 0, 0, 0, '', 'Activate Phase if player complete quest 34558'),

(26, 6662, 34567, 0, 0, 8, 0, 34567, 0, 0, 0, 0, '', 'Activate Phase if player reward quest 34567'),
(26, 6662, 34567, 0, 0, 8, 0, 34558, 0, 0, 0, 0, '', 'Activate Phase if player reward quest 34558'),
(26, 6662, 34567, 0, 1, 8, 0, 34567, 0, 0, 0, 0, '', 'Activate Phase if player reward quest 34567'),
(26, 6662, 34567, 0, 1, 28, 0, 34558, 0, 0, 0, 0, '', 'Activate Phase if player complete quest 34558'),

(26, 6662, 34558, 0, 0, 14, 0, 34558, 0, 0, 0, 0, '', 'Activate Phase if 34558 quest none'),
(26, 6662, 34558, 0, 1, 9, 0, 34558, 0, 0, 0, 0, '', 'Activate Phase if 34558 quest accepted');

delete from playerchoice_response_reward_spellcast where choiceid = 51 and responseid in (119,120);
INSERT INTO `playerchoice_response_reward_spellcast` (`ChoiceId`, `ResponseId`, `Index`, `SpellId`, `VerifiedBuild`) VALUES 
(51, 119, 0, 160027, 26365), -- 160026
(51, 120, 0, 160025, 26365); -- 159986

DELETE FROM `spell_scene` WHERE MiscValue in (677);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1360, 677, 5);

delete from gameobject where id in (231140,237628,237629,237630,231141,231810);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(231140, 1116, 6662, 7097, 1, 0, 34568, 3626.4, 1656.23, 175.975, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(237628, 1116, 6662, 7097, 1, 0, 34568, 3627.1, 1649.12, 201.54, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(237629, 1116, 6662, 7097, 1, 0, 34568, 3633.81, 1657.41, 201.54, 5.60251, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(237630, 1116, 6662, 7097, 1, 0, 34568, 3621.82, 1660.03, 196.642, 0.890114, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(231141, 1116, 6662, 7097, 1, 0, 34567, 3626.4, 1656.23, 175.975, 2.46091, 0, 0, 0.942641, 0.333807, 300, 255, 1, 0, '', 0, -1),
(231810, 1116, 6662, 7097, 1, 0, 34558, 3625.36, 1655.32, 175.975, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1);

update creature set phaseid = 34558, phasemask = 0 where id = 81242 and areaid = 7097;


-- https://forum.wowcircle.net/showthread.php?t=795126
update quest_template set flags = 36766720 where id in (41024,39424);