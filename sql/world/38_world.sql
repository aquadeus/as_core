-- https://forum.wowcircle.net/showthread.php?t=809499
update creature_template set unit_flags = 768, flags_extra = 128, ainame = 'SmartAI' where entry in (31065,31068,31064,31066);
update creature set movementtype = 0, spawndist = 0 where id in (31065,31068,31064,31066);

update gameobject_template set ainame = 'SmartGameObjectAI' where entry in (193424);

delete from smart_scripts where entryorguid in (31065,31068,31064,31066,193424);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(193424, 1, 0, 0, 62, 0, 100, 0, 10111, 0, 0, 0, 0, 85, 58037, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Eye - on gossip select - invoker cast'),

(31065, 0, 0, 0, 60, 0, 100, 0, 0, 0, 5000, 5000, 0, 33, 31065, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Trigger - on update - give credit'),
(31068, 0, 0, 0, 60, 0, 100, 0, 0, 0, 5000, 5000, 0, 33, 31068, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Trigger - on update - give credit'),
(31064, 0, 0, 0, 60, 0, 100, 0, 0, 0, 5000, 5000, 0, 33, 31064, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Trigger - on update - give credit'),
(31066, 0, 0, 0, 60, 0, 100, 0, 0, 0, 5000, 5000, 0, 33, 31066, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Trigger - on update - give credit');

update creature_template set spell1 = 57882, spell2 = 58203, spell3 = 58282, spell4 = 58283 where entry = 30895;


-- https://forum.wowcircle.net/showthread.php?t=809861
update creature_template set flags_extra = 128 where entry in (35171,35192);

delete from conditions where SourceEntry in (68583,66860) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 68583, 0, 0, 31, 0, 3, 35171, 0, 0, 0, '', 'Cast only 35171'),
(13, 1, 66860, 0, 0, 31, 0, 3, 35192, 0, 0, 0, '', 'Cast only 35192');

delete from creature where id in (35171,35192);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(35171, 1, 16, 4801, 1, 1, 0, 0, 0, 4842.66, -6434.82, 18.4986, 5.4843, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35171, 1, 16, 4801, 1, 1, 0, 0, 0, 4883.34, -6533.65, 33.5021, 5.79547, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35171, 1, 16, 4801, 1, 1, 0, 0, 0, 4986.81, -6505.29, 62.5318, 1.3776, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35171, 1, 16, 4801, 1, 1, 0, 0, 0, 4950.42, -6407.65, 88.3426, 2.93269, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35171, 1, 16, 4801, 1, 1, 0, 0, 0, 4887.63, -6459.35, 108.314, 4.6562, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35171, 1, 16, 4801, 1, 1, 0, 0, 0, 4965.68, -6481.68, 134.148, 1.19651, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35192, 1, 16, 4801, 1, 1, 0, 0, 0, 4850.03, -6493.81, 25.7108, 5.1662, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35192, 1, 16, 4801, 1, 1, 0, 0, 0, 4928.9, -6546.9, 37.7329, 0.309458, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35192, 1, 16, 4801, 1, 1, 0, 0, 0, 4994.8, -6451.54, 82.3979, 1.84098, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35192, 1, 16, 4801, 1, 1, 0, 0, 0, 4899.54, -6435.67, 121.737, 4.23645, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35192, 1, 16, 4801, 1, 1, 0, 0, 0, 4921.19, -6505.42, 122.418, 6.16415, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(35192, 1, 16, 4801, 1, 1, 0, 0, 0, 4950.08, -6458.44, 159.152, 3.13644, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from gameobject where id in (195365,196407,195367);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(195365, 1, 16, 4801, 1, 1, 0, 4838.46, -6446.80, 19.1497, 4.74, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(195365, 1, 16, 4801, 1, 1, 0, 4884.25, -6476.94, 108.023, 5.00, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(195365, 1, 16, 4801, 1, 1, 0, 4993.52, -6433.77, 83.0947, 2.18, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(195365, 1, 16, 4801, 1, 1, 0, 4853.46, -6509.91, 25.5878, 5.09, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(195365, 1, 16, 4801, 1, 1, 0, 4951.17, -6547.52, 37.4788, 0.38, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(195365, 1, 16, 4801, 1, 1, 0, 4974.07, -6470.35, 133.652, 1.36, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1);

INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(196407, 1, 16, 4801, 1, 1, 0, 4861.09, -6516.77, 25.9545, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(196407, 1, 16, 4801, 1, 1, 0, 4831.11, -6427.08, 0.202199, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(196407, 1, 16, 4801, 1, 1, 0, 4899.54, -6435.67, 121.737, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(196407, 1, 16, 4801, 1, 1, 0, 4968.88, -6537.35, 36.4903, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(196407, 1, 16, 4801, 1, 1, 0, 4980.78, -6421.79, 82.7102, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(196407, 1, 16, 4801, 1, 1, 0, 4947.05, -6507.79, 120.857, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(195367, 1, 16, 4801, 1, 1, 0, 4968.15, -6460.51, 135.27, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(195367, 1, 16, 4801, 1, 1, 0, 4908.67, -6547.92, 31.5632, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(195367, 1, 16, 4801, 1, 1, 0, 4839.37, -6473.45, 18.9335, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(195367, 1, 16, 4801, 1, 1, 0, 4892.65, -6485.79, 108.739, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(195367, 1, 16, 4801, 1, 1, 0, 4918.4, -6407.51, 88.6822, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1),
(195367, 1, 16, 4801, 1, 1, 0, 4995.37, -6481.1, 62.8871, 0, 0, 0, 0, 1, 300, 255, 1, 0, '', 0, -1);

update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry in (36334,36361);

delete from smart_scripts where entryorguid in (36334,36361,3633400);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(36334, 0, 0, 0, 19, 0, 100, 0, 14296, 0, 0, 0, 0, 80, 3633400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Xilem - on quest accept - run tumed actionlist'),

(3633400, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Actionlist - talk'),
(3633400, 9, 1, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Timed Actionlist - talk'),

(36361, 0, 0, 1, 10, 0, 100, 0, 1, 18, 0, 0, 0, 33, 35171, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Xilem - on ooc los - give credit'),
(36361, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Xilem - on link - talk');

delete from conditions where SourceEntry in (36361) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 36361, 0, 0, 9, 0, 14296, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 36361, 0, 0, 1, 0, 68613, 0, 0, 0, 0, '', 'Smart Only If player has aura');

DELETE FROM `creature_text` WHERE `entry` in (36334,36361);
INSERT INTO `creature_text` (`entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `comment`) VALUES 
(36361, 0, 0, 'Чудесно! Ты прыгаешь даже быстрее кошки. В самом деле, это очень удачно!', 12, 0, 100, 0, 0, 0, 36376, ''),

(36334, 0, 0, 'В Волшебном замке расставлены 6 пьедесталов, коснувшись которых, можно получить заряд энергии.', 41, 0, 100, 0, 0, 0, 50228, ''),
(36334, 1, 0, 'Воспользовавшись энергетическими платформами, поднимитесь в Волшебный замок и прикоснитесь по пути ко всем пьедесталам.', 41, 0, 100, 0, 0, 0, 50229, '');


-- https://forum.wowcircle.net/showthread.php?t=681091
delete from smart_scripts where entryorguid in (96932);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(96932, 0, 0, 1, 10, 0, 100, 0, 1, 1, 0, 0, 0, 33, 96932, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Egg - on ooc los - give credit'),
(96932, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Egg - on link - die');

delete from conditions where SourceEntry in (96932) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 96932, 0, 0, 9, 0, 41955, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 96932, 0, 1, 9, 0, 38372, 0, 0, 0, 0, '', 'Smart Only If player has quest');