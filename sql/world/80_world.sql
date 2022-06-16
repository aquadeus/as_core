-- https://forum.wowcircle.com/showthread.php?t=1054629
delete from gameobject_template where entry = 250650;
INSERT INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `faction`, `flags`, `size`, `Data0`, `Data1`, `Data2`, `Data3`, `Data4`, `Data5`, `Data6`, `Data7`, `Data8`, `Data9`, `Data10`, `Data11`, `Data12`, `Data13`, `Data14`, `Data15`, `Data16`, `Data17`, `Data18`, `Data19`, `Data20`, `Data21`, `Data22`, `Data23`, `Data24`, `Data25`, `Data26`, `Data27`, `Data28`, `Data29`, `Data30`, `Data31`, `Data32`, `RequiredLevel`, `AIName`, `ScriptName`, `WorldEffectID`) VALUES 
(250650, 10, 34362, 'Witherbark Supplies', 'questinteract', '', 0, 0, 1, 2556, 0, 0, 180000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', 0);

delete from gameobject where id in (250650);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES 
(250650, 0, 0, 0, 1, 1, 0, -1711.86, -1703.56, 54.0428, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1725.70, -1743.57, 52.506, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1724.02, -1653.77, 52.6162, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1690.55, -1638.96, 59.5065, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1745.64, -1686.54, 54.1679, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1764.54, -1662.92, 53.4566, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1733.29, -1636.11, 52.5195, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1771.15, -1650.93, 53.2233, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1711.32, -1615.68, 60.2878, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1749.14, -1615.34, 52.8694, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1687.29, -1574.42, 55.0348, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1777.95, -1592.36, 52.9923, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1795.34, -1599.70, 53.3294, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1731.70, -1554.65, 55.9258, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1736.35, -1544.80, 55.7485, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1762.07, -1550.24, 60.9395, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1755.85, -1532.68, 60.9828, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1800.08, -1553.48, 52.5591, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1710.35, -1537.63, 56.8780, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1783.79, -1532.10, 99.3943, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1751.44, -1503.38, 60.9828, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1771.49, -1509.63, 64.9209, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1),
(250650, 0, 0, 0, 1, 1, 0, -1770.56, -1491.93, 99.3943, 2.46091, 0, 0, 0, 0, 120, 255, 1, 0, '', 0, -1);


-- https://forum.wowcircle.com/showthread.php?t=1054632
delete from creature where id in (107792, 107794, 108235, 107793, 107795, 108236);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1692.2, -1613.54, 59.589, 4.85519, 180, 9, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1703.46, -1645.58, 59.3761, 5.76165, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1714.93, -1537.15, 55.8401, 5.06768, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1724.97, -1667.23, 52.6378, 5.7638, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1724.99, -1661.36, 52.6424, 0.54805, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1737, -1638.1, 52.2865, 0.791939, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1747.65, -1622.74, 52.9394, 0.522467, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1748.57, -1504.8, 61.0662, 3.7268, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1765.34, -1621.18, 52.0779, 5.10904, 180, 6, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1783.09, -1565.94, 53.0149, 5.94176, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1652.49, -1747.47, 67.8323, 1.39971, 120, 5, 0, 0, 0, 1),
(107792, 0, 45, 324, 1, 0, 42534, 0, 0, -1669.76, -1741.55, 67.1006, 0.045588, 120, 5, 0, 0, 0, 1),
(107793, 0, 45, 324, 1, 0, 42534, 0, 0, -1690.31, -1590.43, 55.7644, 0.54805, 120, 5, 0, 0, 0, 1),
(107793, 0, 45, 324, 1, 0, 42534, 0, 0, -1714.02, -1540.38, 55.8354, 1.76836, 120, 5, 0, 0, 0, 1),
(107793, 0, 45, 324, 1, 0, 42534, 0, 0, -1652.76, -1740.3, 67.7759, 0.0710954, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1711.79, -1708.12, 54.1413, 5.75903, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1734.84, -1635.52, 52.4983, 4.09125, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1739.49, -1683.17, 52.9163, 3.00838, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1751.52, -1506.76, 61.0662, 0.42749, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1755.01, -1582.65, 51.9094, 4.71394, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1760.73, -1661.64, 53.533, 3.00838, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1761.64, -1665.38, 53.531, 5.81924, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1770.9, -1648.22, 53.337, 2.83271, 120, 5, 0, 0, 0, 1),
(107794, 0, 45, 324, 1, 0, 42534, 0, 0, -1721.44, -1736.98, 52.5677, 4.11533, 120, 5, 0, 0, 0, 1),
(107795, 0, 45, 324, 1, 0, 42534, 0, 0, -1696.19, -1579.27, 54.3704, 0.111388, 120, 5, 0, 0, 0, 1),
(107795, 0, 45, 324, 1, 0, 42534, 0, 0, -1726.92, -1701.67, 52.4222, 1.44466, 120, 5, 0, 0, 0, 1),
(107795, 0, 45, 324, 1, 0, 42534, 0, 0, -1728.45, -1697.62, 52.4635, 4.90826, 120, 5, 0, 0, 0, 1),
(107795, 0, 45, 324, 1, 0, 42534, 0, 0, -1742.85, -1498.78, 61.1333, 3.79667, 120, 5, 0, 0, 0, 1),
(107795, 0, 45, 324, 1, 0, 42534, 0, 0, -1790.95, -1555.5, 52.6424, 4.85318, 120, 5, 0, 0, 0, 1),
(108236, 0, 45, 325, 1, 0, 42534, 0, 0, -1760.42, -1537.98, 64.9336, 5.43589, 120, 0, 0, 0, 0, 0);

update quest_template set inprogressphaseid = 42534 where id = 42534;

update creature_template set minlevel = 110, maxlevel = 110, regenhealth = 1, faction = 14, lootid = entry where entry in (107792, 107794, 108235, 107793, 107795, 108236);


update creature_loot_template set chanceorquestchance = -100 where entry = 108236;


delete from creature_loot_template where item = -138142;


-- https://forum.wowcircle.com/showthread.php?t=1054641
update creature_template set minlevel = 100, maxlevel = 110, faction = 14, lootid = entry where entry = 103677;


-- https://forum.wowcircle.com/showthread.php?t=1054645
delete from npc_spellclick_spells where npc_entry in (100179);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(100179, 197069, 1, 0);

delete from conditions where SourceGroup in (100179) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 100179, 197069, 0, 0, 9, 0, 41499, 0, 0, 0, 0, '', 'Allow spellclick if player has quest');

update creature_template set ainame = 'SmartAI' where entry = 100179;

delete from smart_scripts where entryorguid in (100179);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(100179, 0, 0, 1, 8, 0, 100, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Inkubator - on spellhit - die'),
(100179, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 33, 104370, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Inkubator - on link - give credit');


-- https://forum.wowcircle.com/showthread.php?t=1054649
update gameobject_template set flags = 0, data1 = entry where entry in (251046,251034,251035);

delete from gameobject_loot_template where entry in (251046,251034,251035);
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(251046, 138195, -100, 0, 0, 1, 1),	
(251034, 138193, -100, 0, 0, 1, 1),
(251035, 138194, -100, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1054664

delete from creature where id in (107554);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`) VALUES 
(107554, 1, 14, 14, 1, 1, 0, 0, 0, 1185.0000, -4464.0000, 21.0000, 3.64, 180, 9, 0, 0, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1055052
delete from spell_linked_spell where spell_trigger in (201200) and spell_effect in (201195,225473);
delete from spell_linked_spell where spell_trigger in (-201195) and spell_effect in (-225473);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`, `actiontype`) VALUES 
(-201195, -225473, 0, 0, 0, 0, 0, '', 0),

(201200, 201195, 0, 0, 0, 0, 0, '', 0),
(201200, 225473, 0, 0, 0, 0, 0, '', 0);

delete from smart_scripts where entryorguid in (102017,102142,10201700);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(102017, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 11, 201194, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on just summoned - cast'),
(102017, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 11, 203032, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on link - cast'),
(102017, 0, 2, 0, 23, 1, 100, 1, 203032, 0, 0, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Despawn self');

delete from event_scripts where id in (48379);
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(48379, 0, 10, 102017, 300000, 0, 1641.8400, 4743.4502, 138.6540, 3.246);


-- https://forum.wowcircle.com/showthread.php?t=1050923
delete from smart_scripts where entryorguid = 93465;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(93465, 0, 0, 0, 62, 0, 100, 0, 0, 20554, 1, 0, 0, 0, 85, 229431, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Grimwing - on gossip select - invoker cast');


-- https://forum.wowcircle.com/showthread.php?t=1055059
delete from smart_scripts where entryorguid in (102545);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(102545, 0, 0, 1, 54, 0, 100, 1, 0, 0, 0, 0, 0, 11, 202581, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on just summoned - cast spell'),
(102545, 0, 1, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 89, 20, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - random move'),
(102545, 0, 2, 6, 1, 0, 4, 0, 1000, 1000, 1000, 1000, 0, 12, 106799, 2, 30000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc - summon creature'),
(102545, 0, 3, 6, 1, 0, 4, 0, 30000, 30000, 30000, 30000, 0, 12, 106799, 2, 120000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc - summon creature'),
(102545, 0, 4, 6, 1, 0, 4, 0, 1000, 1000, 1000, 1000, 0, 12, 106690, 2, 120000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc - summon creature'),
(102545, 0, 5, 6, 1, 0, 4, 0, 1000, 1000, 1000, 1000, 0, 12, 106810, 2, 120000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on ooc - summon creature'),
(102545, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - die');


-- https://forum.wowcircle.com/showthread.php?t=1055237
DELETE FROM `spell_area` WHERE `spell` in (201208) and area = 7502;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(201208, 7502, 0, 40832, 0, 0, 2, 1, 0, 74);

update creature_template set ainame = 'SmartAI' where entry in (102018,101513);
update creature_template set ainame = 'SmartAI', flags_extra = 128 where entry in (102086);

delete from smart_scripts where entryorguid in (97004,102018,102086,101513);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(102018, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Courier - on just summoned - follow'),

(102086, 0, 0, 0, 60, 0, 100, 0, 0, 0, 1000, 1000, 0, 33, 102086, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, 'Trigger - on update - give credit'),
(101513, 0, 0, 0, 60, 0, 100, 0, 0, 0, 1000, 1000, 0, 33, 102098, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, 'Jorah - on update - give credit'),

(97004, 0, 0, 2, 62, 0, 100, 0, 19540, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 15, 251033, 30, 0, 0, 0, 0, 0, 'Red - on gossip select - activate gameobject'),
(97004, 0, 1, 3, 62, 0, 100, 0, 19540, 1, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 15, 251033, 30, 0, 0, 0, 0, 0, 'Red - on gossip select - activate gameobject'),
(97004, 0, 2, 4, 61, 0, 10, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Red - on link - talk'),
(97004, 0, 3, 5, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Red - on link - talk'),
(97004, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Red - on link - close gossip'),
(97004, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Red - on link - close gossip'),
(97004, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 105907, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Red - on link - give credit');

delete from gossip_menu_option where menu_id in (19540);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`, `action_menu_id`) VALUES 
(19540, 2, 1, 'I want to browse your goods.', 3, 128, 3370, 0),
(19540, 1, 0, 'The raven calls.', 1, 1, 108809, 0),
(19540, 0, 0, '<Lay your insignia on the table.>', 1, 1, 108796, 0);

delete from spell_linked_spell where spell_trigger in (201253) and spell_effect in (201264);
delete from spell_linked_spell where spell_trigger in (-201253) and spell_effect in (201264);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`, `actiontype`) VALUES 
(-201253, 201264, 0, 0, 0, 0, 0, '', 0);

delete from creature where id = 102086;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(102086, 1220, 7502, 8011, 1, 1, 0, 0, 0, -932.163, 4433.24, 706.103, 4.02888, 300, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from creature_template_addon where entry = 101513;

delete from conditions where SourceEntry in (201208) and SourceTypeOrReferenceId = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 201208, 0, 0, 15, 0, 8, 0, 0, 0, 98, '', 'Cast spell if rogue'),
(17, 0, 201208, 0, 0, 27, 0, 98, 3, 0, 0, 98, '', 'Cast spell only if >= 98 lvl');

delete from conditions where SourceGroup in (19540) and SourceTypeOrReferenceId = 15;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 19540, 0, 0, 0, 28, 0, 40832, 0, 0, 0, 0, '', 'Show gossip if 40832 quest incomplete'),
(15, 19540, 0, 0, 1, 8, 0, 40832, 0, 0, 0, 0, '', 'Show gossip if 40832 quest incomplete'),
(15, 19540, 1, 0, 0, 9, 0, 40832, 0, 0, 0, 0, '', 'Show gossip if 40832 quest incomplete');


-- https://forum.wowcircle.com/showthread.php?t=1055244
delete from smart_scripts where entryorguid in (102343,102363);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(102343, 0, 0, 1, 62, 0, 100, 0, 102343, 0, 0, 0, 0, 33, 102340, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Gossip select->KC 102340'),
(102343, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 12, 102363, 8, 0, 0, 0, 0, 8, 0, 0, 0, 2602.88, -527.323, 88.9997, 4.88775, 'Link->Summon NPC 102363 Alonsus'),

(102363, 0, 0, 10, 54, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 12000, 0, 0, 0, 0, 21, 90, 0, 0, 0, 0, 0, 0, 'Faol - on just summoned - talk'),
(102363, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 2605.41, -532.449, 89, 0, 'Faol - on link - move to position'),
(102363, 0, 2, 3, 52, 0, 100, 0, 0, 102363, 0, 0, 0, 1, 1, 12000, 0, 0, 0, 0, 21, 90, 0, 0, 0, 0, 0, 0, 'Faol - on text over - talk'),
(102363, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 69, 2, 0, 0, 0, 0, 0, 8, 0, 0, 0, 2600.68, -536.839, 89, 0, 'Faol - on link - move to position'),
(102363, 0, 4, 0, 34, 0, 100, 0, 0, 2, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.220254, 'Faol - on point reached - set orientation'),
(102363, 0, 5, 6, 52, 0, 100, 0, 1, 102363, 0, 0, 0, 1, 2, 12000, 0, 0, 0, 0, 21, 90, 0, 0, 0, 0, 0, 0, 'Faol - on text over - talk'),
(102363, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 69, 3, 0, 0, 0, 0, 0, 8, 0, 0, 0, 2603.12, -534.743, 89, 0, 'Faol - on link - move to position'),
(102363, 0, 7, 0, 40, 0, 100, 0, 0, 3, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.78465, 'Faol - on point reached - set orientation'),
(102363, 0, 8, 9, 52, 0, 100, 0, 2, 102363, 0, 0, 0, 33, 102341, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, 'Faol - on text over - give credit'),
(102363, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Faol - on link - despawn'),
(102363, 0, 10, 1, 61, 0, 100, 0, 0, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Faol - on link - set walk');

delete from spell_area where spell in (92284) and area = 173;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(92284, 173, 40705, 0, 0, 0, 2, 1, 66, 0);

delete from waypoints where entry in (102363,1023630,1023631);


-- https://forum.wowcircle.com/showthread.php?t=1055261
update quest_template set nextquestidchain = 0 where id = 33161;


-- https://forum.wowcircle.com/showthread.php?t=1055371
update quest_template set prevquestid = 41760 where id = 41762;


-- https://forum.wowcircle.com/showthread.php?t=1055372
update quest_template set prevquestid = 42229 where id = 44672;

DELETE FROM `creature_queststarter` WHERE quest in (44672);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(97140, 44672);


-- https://forum.wowcircle.com/showthread.php?t=1055437
update creature_template set ainame = 'SmartAI', unit_flags = 768 where entry = 34328;

delete from smart_scripts where entryorguid in (34328);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(34328, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 0, 11, 65202, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Presence - on just summoned - cast'),
(34328, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 33, 34373, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Presence - on link - give credit');

delete from conditions where SourceEntry in (65188) and SourceTypeOrReferenceId = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 65188, 0, 0, 31, 1, 3, 2071, 0, 0, 0, '', 'Cast spell into 2071 creature'),
(17, 0, 65188, 0, 1, 31, 1, 3, 2237, 0, 0, 0, '', 'Cast spell into 2237 creature'),
(17, 0, 65188, 0, 2, 31, 1, 3, 34302, 0, 0, 0, '', 'Cast spell into 34302 creature'),
(17, 0, 65188, 0, 3, 31, 1, 3, 34318, 0, 0, 0, '', 'Cast spell into 34318 creature');


-- https://forum.wowcircle.com/showthread.php?t=1055471
update quest_template set flags = 0 where id = 41149;

replace INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(100059, 0, 0, 0, '204889');

delete from npc_spellclick_spells where npc_entry in (100059);
REPLACE INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(100059, 204867, 1, 0);

update creature_template set ainame = 'SmartAI', npcflag = 16777216, unit_Flags = 768, faction = 35, dynamicflags = 0, unit_flags2 = 0 where entry = 100059;

delete from smart_scripts where entryorguid in (100059);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(100059, 0, 0, 1, 8, 0, 100, 0, 204867, 0, 30000, 30000, 0, 33, 100059, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Ward - on spell hit - give credit'),
(100059, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 11, 204868, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Ward - on link - cast');

delete from conditions where SourceGroup in (100059) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 100059, 204867, 0, 0, 9, 0, 41149, 0, 0, 0, 0, '', 'Allow spellclick if quest incomplete');


-- https://forum.wowcircle.com/showthread.php?t=1055472
update gameobject set phasemask = 1 where id = 245327;

update gameobject_template set data2 = 0, data3 = 60000 where entry = 245327;

delete from event_scripts where id = 245327;