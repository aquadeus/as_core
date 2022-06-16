-- https://forum.wowcircle.com/showthread.php?t=1061119
delete from spell_loot_template where entry in (228214);
INSERT INTO `spell_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(228214, 110, 100, 0, 0, -110, 1);

delete from reference_loot_template where entry in (110);
INSERT INTO `reference_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`) VALUES 
(110, 140582, 0, 0, 0, 1, 1),
(110, 140583, 0, 0, 0, 1, 1),
(110, 139809, 0, 0, 0, 1, 1),
(110, 139801, 0, 0, 0, 1, 1),
(110, 139866, 0, 0, 0, 1, 1),
(110, 139865, 0, 0, 0, 1, 1),
(110, 139811, 0, 0, 0, 1, 1),
(110, 139814, 0, 0, 0, 1, 1),
(110, 139802, 0, 0, 0, 1, 1),
(110, 139864, 0, 0, 0, 1, 1),
(110, 139808, 0, 0, 0, 1, 1),
(110, 140571, 0, 0, 0, 1, 1);


-- https://forum.wowcircle.com/showthread.php?t=1043451
delete from creature_loot_template where entry in (61398);
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(61398, 144152, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 143956, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 143995, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 144153, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 144155, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 143993, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 144154, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 144151, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 143994, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 144150, 11, 0, 0, 1, 1, 0, 0, 0),
(61398, 144214, 11, 0, 0, 1, 1, 0, 0, 0);


-- https://forum.wowcircle.com/showthread.php?t=1061385
DELETE FROM `spell_area` WHERE `spell` in (226957) and area = 7600;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(226957, 7600, 44464, 44464, 0, 0, 2, 1, 8, 66);

delete from smart_scripts where entryorguid = 113857 and action_param1 = 113857;

DELETE FROM `spell_scene` WHERE MiscValue in (1459);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`, `TrigerSpell`, `MonsterCredit`, `ScriptName`, `Comment`) VALUES
(1714, 1459, 0, 0, 0, '', '');

delete from spell_script_names where spell_id in (226956);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(226956, 'spell_gen_call_to_xera');


-- https://forum.wowcircle.com/showthread.php?t=1061387
DELETE FROM `spell_area` WHERE `spell` in (226969) and area = 1223;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(226969, 1223, 44480, 44480, 0, 0, 2, 1, 8, 66);

DELETE FROM `spell_scene` WHERE MiscValue in (1461);
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`, `TrigerSpell`, `MonsterCredit`, `ScriptName`, `Comment`) VALUES
(1726, 1461, 0, 0, 0, '', '');

delete from spell_script_names where spell_id in (228309);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES 
(228309, 'spell_gen_call_to_xera');

delete from spell_linked_spell where spell_trigger in (226968) and spell_effect in (228309);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(226968, 228309, 0, 0, 0, 0, 0, '');