
-- https://ru.wowhead.com/item=151307
delete from spell_script_names where spell_id in (251459, 251461);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
('251459', 'spell_item_call_of_the_void_stalker_trinket'),
('251461', 'spell_item_call_of_the_void_stalker_trinket');

delete from areatrigger_scripts where entry = 16113;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES 
('16113', 'areatrigger_item_coth_void_stalker');

delete from areatrigger_actions where entry = 16113;
INSERT INTO  `areatrigger_actions` (`entry`, `summon_spellId`, `customEntry`, `id`, `moment`, `actionType`, `targetFlags`, `spellId`, `maxCharges`, `aura`, `hasspell`, `chargeRecoveryTime`, `scale`, `hitMaxCount`, `amount`, `onDespawn`, `comment`) VALUES
('11356', '251035', '16113', '0', '1', '20', '2', '251034', '0', '0', '0', '0', '0', '0', '0', '0', 'Call of the Void Stalker');

delete from areatrigger_template where entry = 16113;
INSERT INTO `areatrigger_template` (`entry`, `spellId`, `customEntry`, `VisualID`, `DecalPropertiesId`, `Radius`, `RadiusTarget`, `Height`, `HeightTarget`, `Float4`, `Float5`, `IsCheckingWithObjectSize`, `IsCheckingNeighbours`, `isMoving`, `Distance`, `MoveCurveID`, `ElapsedTime`, `MorphCurveID`, `FacingCurveID`, `ScaleCurveID`, `ShapeID1`, `ShapeID2`, `HasUnkBit50`, `UnkBit50`, `HasFollowsTerrain`, `HasAttached`, `HasAbsoluteOrientation`, `HasDynamicShape`, `HasFaceMovementDir`, `hasAreaTriggerBox`, `RollPitchYaw1X`, `RollPitchYaw1Y`, `RollPitchYaw1Z`, `TargetRollPitchYawX`, `TargetRollPitchYawY`, `TargetRollPitchYawZ`, `windX`, `windY`, `windZ`, `windSpeed`, `windType`, `polygon`, `comment`, `ScriptName`) VALUES
('11356', '251035', '16113', '0', '0', '5', '5', '3', '3', '5', '5', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'Call of the Void Stalker', '');