--
delete from areatrigger_template where spellId = 203743;
INSERT INTO `areatrigger_template` (`entry`, `spellId`, `customEntry`, `VisualID`, `DecalPropertiesId`, `Radius`, `RadiusTarget`, `Height`, `HeightTarget`, `Float4`, `Float5`, `IsCheckingWithObjectSize`, `IsCheckingNeighbours`, `isMoving`, `Distance`, `MoveCurveID`, `ElapsedTime`, `MorphCurveID`, `FacingCurveID`, `ScaleCurveID`, `ShapeID1`, `ShapeID2`, `HasUnkBit50`, `UnkBit50`, `HasFollowsTerrain`, `HasAttached`, `HasAbsoluteOrientation`, `HasDynamicShape`, `HasFaceMovementDir`, `hasAreaTriggerBox`, `RollPitchYaw1X`, `RollPitchYaw1Y`, `RollPitchYaw1Z`, `TargetRollPitchYawX`, `TargetRollPitchYawY`, `TargetRollPitchYawZ`, `windX`, `windY`, `windZ`, `windSpeed`, `windType`, `polygon`, `comment`, `ScriptName`) VALUES 
(5971, 203743, 10659, 203743, 0, 5, 5, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Emerald Ring', '');

delete from areatrigger_actions where entry = 5971 and customentry = 10659;
INSERT INTO `areatrigger_actions` (`entry`, `summon_spellid`, `customEntry`, `id`, `moment`, `actionType`, `targetFlags`, `spellId`, `maxCharges`, `aura`, `hasspell`, `chargeRecoveryTime`, `hitMaxCount`, `amount`, `onDespawn`, `comment`) VALUES 
(5971, 203743, 10659, 0, 1, 0, 8, 203744, 1, 203740, 0, 0, 0, 0, 0, 'Emerald Ring');

delete from gossip_menu_option where menu_id in (19295);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(19295, 0, 0, 'I would like to attempt the flying challenge, please grant me your blessing.', 1, 1, 105556);

delete from gossip_menu where entry in (19295);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(19295, 28377);

delete from npc_text where id in (28377);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(28377, 105550, 105550);

delete from conditions where SourceGroup in (19295) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19295, 0, 0, 0, 9, 0, 41094, 0, 0, 0, 0, '', 'Show Gossip If Player Has 41094 quest');

update creature_template set gossip_menu_id = 19295, ainame = 'SmartAI' where entry in (97925);
update creature_template set ainame = 'SmartAI', inhabittype = 7, flags_extra = 128 where entry in (103076);

delete from smart_scripts where entryorguid in (97925,103076);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(97925, 0, 0, 0, 62, 0, 100, 0, 19295, 0, 0, 0, 0, 85, 203741, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Aviana - on gossip select - invoker cast'),

(103076, 0, 0, 0, 60, 0, 100, 0, 0, 0, 60000, 60000, 0, 11, 203743, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on update - cast spell');

delete from spell_linked_spell where spell_trigger in (-203740) and spell_effect in (-203744);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(-203740, -203744, 0, 0, 0, 0, 0, '');

delete from spell_script_names where spell_id = 203744;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(203744, 'spell_emerland_ring');

delete from creature_template_addon where entry in (103076);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `emote`, `bytes1`, `auras`) VALUES
(103076, 0, 0, 33554432, '');

delete from creature where id = 103076 or id = 101622 and map = 1220;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `dynamicflags`) VALUES 
(101622, 1220, 0, 0, 1, 1, 0, 0, 0, 3679.29, 5519.91, 825.083, 2.36, 120, 0, 0, 87, 0, 0, 0, 0, 0, 0),

(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3683.13, 5527.48, 375.083, 3.7413, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3674.73, 5529.63, 430.083, 5.6924, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3683.13, 5527.48, 470.476, 2.9551, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3687.01, 5523.79, 534.190, 2.1050, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3690.99, 5521.12, 586.324, 2.8898, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3687.19, 5521.28, 633.620, 2.1042, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3679.29, 5519.91, 670.083, 5.6924, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3700.51, 5506.77, 703.125, 6.2127, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3696.08, 5508.65, 755.059, 2.8892, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0),
(103076, 1220, 0, 0, 1, 0, 41094, 0, 0, 3679.29, 5519.91, 800.083, 5.6924, 120, 5, 0, 87, 0, 0, 0, 0, 0, 0);

delete from phase_definitions where zoneId in (7558) and entry in (41094);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `comment`) VALUES 
(7558, 41094, 0, 41094, '41094 quest');

delete from conditions where SourceGroup in (7558) and SourceTypeOrReferenceId = 26 and sourceentry = 41094;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7558, 41094, 0, 0, 9, 0, 41094, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7558, 41094, 0, 1, 28, 0, 41094, 0, 0, 0, 0, '', 'Activate phase if player complete quest');

delete from conditions where SourceEntry in (203741) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 203741, 0, 0, 31, 0, 3, 101622, 0, 0, 0, '', 'Cast only 101622');


-- https://forum.wowcircle.net/showthread.php?t=1032659
delete from smart_scripts where entryorguid in (78423);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(78423, 0, 0, 1, 62, 0, 100, 0, 16863, 0, 0, 0, 0, 85, 177076, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Khadgar - on gossip select - invoker cast'),
(78423, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Khadgar - on link - close gossip');

delete from spell_linked_spell where spell_trigger in (168956) and spell_effect in (167771);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(168956, 167771, 0, 0, 0, 0, 0, '');


-- https://forum.wowcircle.net/showthread.php?t=950135
update creature_template set ainame = 'AggressorAI' where entry = 86039;

delete from creature where id = 86039;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(86039, 1265, 7025, 7044, 1, 0, 3443914, 0, 0, 4065.76, -1955.65, 27.86, 1.46356, 120, 0, 0, 0, 0, 0);


-- https://forum.wowcircle.net/showthread.php?t=1033736
update creature_template set npcflag = 2, ainame = 'SmartAI', gossip_menu_id = 19156 where entry = 101195;

delete from gossip_menu_option where menu_id in (19156);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(19156, 0, 0, 'I am ready.', 1, 1, 104314);

delete from conditions where SourceGroup in (19156) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19156, 0, 0, 0, 9, 0, 43409, 0, 0, 0, 0, '', 'Show Gossip if player has 43409 quest');

delete from gossip_menu where entry in (19156);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(19156, 28059);

delete from npc_text where id in (28059);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(28059, 111820, 111820);

delete from smart_scripts where entryorguid in (101195) and id > 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(101195, 0, 1, 2, 62, 0, 100, 0, 19156, 0, 0, 0, 0, 85, 221473, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Rensar - on gossip select - invoker cast'),
(101195, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 0, 75, 219567, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Rensar - on link - add aura'),
(101195, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Rensar - on link - close gossip');
-- (101195, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 208, 12364, 3959.23, 7508.06, 52.9821, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Rensar - on link - summon areatrigger');

delete from conversation_creature where entry = 3149;
delete from conversation_data where entry = 3149;
delete from conversation_actor where entry = 3149;
INSERT INTO `conversation_creature` (`entry`, `id`, `creatureId`, `creatureGuid`, `unk1`, `unk2`, `duration`) VALUES 
(3149, 1, 101195, 0, 0, 0, 10536);

INSERT INTO `conversation_data` (`entry`, `id`, `idx`, `textId`, `unk1`, `unk2`) VALUES 
(3149, 6647, 1, 1000, 0, 1),
(3149, 6648, 2, 10997, 0, 1),
(3149, 8185, 3, 19898, 0, 1),
(3149, 8704, 0, 0, 0, 1);

INSERT INTO `conversation_actor` (`entry`, `id`, `actorId`, `creatureId`, `displayId`, `unk1`, `unk2`, `unk3`, `duration`) VALUES 
(3149, 0, 53609, 101195, 67016, 0, 1, 0, 4821);

-- delete from areatrigger_template where customEntry = 12364;
-- INSERT INTO `areatrigger_template` (`entry`, `spellId`, `customEntry`, `VisualID`, `DecalPropertiesId`, `Radius`, `RadiusTarget`, `Height`, `HeightTarget`, `Float4`, `Float5`, `IsCheckingWithObjectSize`, `IsCheckingNeighbours`, `isMoving`, `Distance`, `MoveCurveID`, `ElapsedTime`, `MorphCurveID`, `FacingCurveID`, `ScaleCurveID`, `ShapeID1`, `ShapeID2`, `HasUnkBit50`, `UnkBit50`, `HasFollowsTerrain`, `HasAttached`, `HasAbsoluteOrientation`, `HasDynamicShape`, `HasFaceMovementDir`, `hasAreaTriggerBox`, `RollPitchYaw1X`, `RollPitchYaw1Y`, `RollPitchYaw1Z`, `TargetRollPitchYawX`, `TargetRollPitchYawY`, `TargetRollPitchYawZ`, `windX`, `windY`, `windZ`, `windSpeed`, `windType`, `polygon`, `comment`, `ScriptName`) VALUES 
-- (0, 230836, 12364, 230836, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Test', '');

-- delete from areatrigger_actions where customentry = 12364;
-- INSERT INTO `areatrigger_actions` (`entry`, `summon_spellid`, `customEntry`, `id`, `moment`, `actionType`, `targetFlags`, `spellId`, `maxCharges`, `aura`, `hasspell`, `chargeRecoveryTime`, `hitMaxCount`, `amount`, `onDespawn`, `comment`) VALUES 
-- (0, 0, 12364, 0, 1, 0, 8, 219567, 1, 0, 0, 0, 0, 0, 0, 'test');

update creature set npcflag = 3 where id = 101195 and position_x like '3957%';

DELETE FROM `spell_scene` WHERE MiscValue in (1362);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`) VALUES
(1669, 1362, 20);

delete from spell_linked_spell where spell_trigger in (219634) and spell_effect in (219410);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(219634, 219410, 0, 0, 0, 0, 0, '');


-- https://forum.wowcircle.net/showthread.php?t=1033753
update creature_template set ainame = 'SmartAI', minlevel = 110, maxlevel = 110, regenhealth = 1 where entry in (112550,112548,112544,112549,112551,112546);

delete from creature_template_addon where entry in (112548);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(112548, 0, 33554432, 0, ''); 

update gameobject_template set ainame = 'SmartGameObjectAI' where entry in (253933,253934);

update creature_template set inhabittype = 7 where entry = 112548;
update creature_template set faction = 7 where entry = 112550;
delete from smart_scripts where entryorguid in (112550,112551, 253933, 253934);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(253933, 1, 0, 1, 60, 0, 100, 0, 0, 0, 1000, 1000, 33, 112544, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, 'Gameobject trigger - on update - give credit'),
(253934, 1, 0, 1, 60, 0, 100, 0, 0, 0, 1000, 1000, 33, 112546, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, 'Gameobject trigger - on update - give credit'),
(253933, 1, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 33, 106020, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, 'Gameobject trigger - on link - give credit'),
(253934, 1, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 33, 106020, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, 'Gameobject trigger - on link - give credit'),

(112550, 0, 0, 1, 8, 0, 100, 0, 8921, 0, 0, 0, 33, 112548, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit - give credit'),
(112550, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 33, 106020, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - give credit'),
(112550, 0, 2, 3, 60, 0, 100, 1, 0, 0, 0, 0, 42, 79999, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on update - set invincibility'),
(112550, 0, 3, 0, 61, 0, 100, 1, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - set state'),

(112551, 0, 0, 1, 53, 0, 100, 0, 1, 5000000, 5000, 5000, 33, 112549, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on heal received - give credit'),
(112551, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 33, 106020, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - give credit');

delete from gameobject where id in (253935,253932,253933,253934);
INSERT INTO gameobject (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(253935, 1540, 7979, 0, 3, 0, 1642.238, 1622.83, 8.3008980, 2.891088, 0.351133, 0.480054, -0.798482, 0.0931677, 7200, 255, 1),
(253934, 1540, 7979, 0, 3, 0, 1582.517, 1591.944, 16.80448, 0.254516, 0.015360, 0.522401, 0.0589666, 0.85052, 7200, 255, 1),
(253933, 1540, 7979, 0, 3, 0, 1714.964, 1618.116, 11.2924, 0.3577498, 0.176464, -0.51616, 0.2007320, 0.813722, 7200, 255, 1),
(253932, 1540, 7979, 0, 3, 0, 1603.031, 1534.818, 14.74721, 4.500773, -0.51297, -0.08600, -0.741990, 0.422983, 7200, 255, 1);

delete from creature where id in (112550,112551,112544,112546,112548,112549);
INSERT INTO creature (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(112550, 1540, 7979, 7979, 3, 1, 0, 0, 0, 1640.714, 1620.476, 6.923208, 0.007812117, 7200, 0, 0, 0, 0, 0, 0, 0, 0),
(112551, 1540, 7979, 7979, 3, 1, 0, 0, 0, 1605.62, 1537.042, 13.54635, 0, 7200, 0, 0, 0, 0, 0, 0, 0, 0),

(112544, 1540, 7979, 7979, 3, 0, 6123, 0, 0, 1717.139, 1613.786, 10.30191, 3.419625, 7200, 0, 0, 0, 0, 0, 0, 0, 0),
(112546, 1540, 7979, 7979, 3, 0, 6124, 0, 0, 1586.611, 1587.319, 15.02902, 0.5153494, 7200, 0, 0, 0, 0, 0, 0, 0, 0),
(112548, 1540, 7979, 7979, 3, 0, 6125, 0, 0, 1644.321, 1617.019, 9.263924, 3.710902, 7200, 0, 0, 0, 0, 0, 0, 0, 0),
(112549, 1540, 7979, 7979, 3, 0, 6126, 0, 0, 1602.854, 1540.068, 13.65915, 5.643214, 7200, 0, 0, 0, 0, 0, 0, 0, 0);

delete from phase_definitions where zoneId = 7979 and entry in (3,4,5,6);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 

(7979, 6, 0, 6123, 0, '43991 quest'),
(7979, 5, 0, 6124, 0, '43991 quest'),
(7979, 4, 0, 6125, 0, '43991 quest'),
(7979, 3, 0, 6126, 0, '43991 quest');

delete from conditions where SourceGroup in (7979) and SourceTypeOrReferenceId = 26 and sourceentry in (3,4,5,6);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7979, 3, 0, 0, 9, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7979, 3, 0, 0, 48, 0, 286245, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7979, 3, 0, 1, 8, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player reward quest'),
(26, 7979, 3, 0, 2, 28, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player complete quest'),

(26, 7979, 4, 0, 0, 9, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7979, 4, 0, 0, 48, 0, 286244, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7979, 4, 0, 1, 8, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player reward quest'),
(26, 7979, 4, 0, 2, 28, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player complete quest'),


(26, 7979, 5, 0, 0, 9, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7979, 5, 0, 0, 48, 0, 286243, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7979, 5, 0, 1, 8, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player reward quest'),
(26, 7979, 5, 0, 2, 28, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player complete quest'),

(26, 7979, 6, 0, 0, 9, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player has quest'),
(26, 7979, 6, 0, 0, 48, 0, 286073, 0, 0, 0, 0, '', 'Activate phase if player complete criteria'),
(26, 7979, 6, 0, 1, 8, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player reward quest'),
(26, 7979, 6, 0, 2, 28, 0, 43991, 0, 0, 0, 0, '', 'Activate phase if player complete quest');