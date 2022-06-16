
delete from eventobject_template where entry in (13000, 13001);
INSERT INTO `eventobject_template` (`entry`, `name`, `radius`, `SpellID`, `WorldSafeLocID`, `ScriptName`) VALUES
('13000', 'Lower Blackrock Spire - Entrance', '3', '0', '3644', ''),
('13001', 'Lower Blackrock Spire - Exit', '2.5', '0', '4920', '');

delete from eventobject where id in (13000, 13001);
INSERT INTO `eventobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`) VALUES
('13000', '13000', '0', '46', '254', '1', '1', '0', '-7521.53', '-1334.51', '291.17', '0'),
('13001', '13001', '229', '1583', '1583', '2', '1', '0', '67.8434', '-325.126', '55.7875', '0');

delete from gameobject where id in (233436) and map in (0, 229, 1116);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES
('233436', '0', '46', '254', '1', '1', '0', '-7521.08', '-1334.46', '291.40', '0.100859', '0', '0', '0.0504083', '0.998729', '300', '0', '1', '0', '', '0', '-1'),
('233436', '229', '1583', '1583', '2', '1', '0', '67.84', '-325.12', '55.78', '3.14', '0', '0', '1', '0.0006326', '300', '0', '1', '0', '', '0', '-1'),
('233436', '1116', '6722', '6722', '1', '1', '0', '23.1059', '2523.26', '104.401', '0.508704', '0', '0', '0', '0', '120', '255', '1', '0', '', '0', '-1'),
('233436', '0', '46', '254', '1', '1', '0', '-7482.42', '-1323.46', '301.384', '0.118289', '0', '0', '0', '0', '300', '100', '1', '0', '', '0', '-1');


-- https://ru.wowhead.com/item=139489 https://ru.wowhead.com/item=139494 https://ru.wowhead.com/item=139496 https://ru.wowhead.com/item=139499 https://ru.wowhead.com/item=139498
-- https://ru.wowhead.com/item=139491 https://ru.wowhead.com/item=139490 https://ru.wowhead.com/item=139493 https://ru.wowhead.com/item=139492 https://ru.wowhead.com/item=139497

-- https://forum.wowcircle.com/showthread.php?t=1058658

delete from spell_script_names where spell_id in (201696, 205504, 205507, 205510, 205509, 205501, 205500, 205503, 205502, 205508);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
('201696', 'spell_gen_relearn_mining_quests'),
('205504', 'spell_gen_relearn_enchanting_quests'),
('205507', 'spell_gen_relearn_inscription_quests'),
('205510', 'spell_gen_relearn_tailoring_quests'),
('205509', 'spell_gen_relearn_leatherworking_quests'),
('205501', 'spell_gen_relearn_herbalism_quests'),
('205500', 'spell_gen_relearn_skinning_quests'),
('205503', 'spell_gen_relearn_blacksmithing_quests'),
('205502', 'spell_gen_relearn_alchemy_quests'),
('205508', 'spell_gen_relearn_jewelcrafting_quests');