-- https://forum.wowcircle.net/showthread.php?t=812241
update creature_template set npcflag = 16777216, unit_flags = 768 where entry = 89630;

delete from npc_spellclick_spells where npc_entry = 89630;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(89630, 179052, 1, 0);

delete from conditions where SourceGroup in (89630) and SourceTypeOrReferenceId = 18;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(18, 89630, 179052, 0, 0, 9, 0, 40102, 0, 0, 0, 0, '', 'SC If player Has Quest');

delete from event_scripts where id in (51143);
INSERT INTO `event_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES 
(51143, 0, 8, 89630, 0, 0, 0, 0, 0, 0);


-- https://forum.wowcircle.net/showthread.php?t=812243
update quest_template set method = 2 where id = 40216;

update creature_template set ainame = 'SmartAI', gossip_menu_id = 18927 where entry = 94434;

delete from spell_linked_spell where spell_trigger in (196679) and spell_effect in (196761,192396,195713);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `duration`, `hastalent2`, `chance`, `cooldown`, `comment`, `actiontype`) VALUES 
(196679, 196761, 0, 0, 0, 0, 0, '', 0),
(196679, 192396, 0, 0, 0, 0, 0, '', 0),
(196679, 195713, 0, 0, 0, 0, 0, '', 0);

delete from conditions where SourceEntry in (196761,192396,195713) and SourceTypeOrReferenceId = 17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17, 0, 196761, 0, 0, 48, 0, 280401, 0, 0, 0, 98, '', 'Cast if criteria completed'),
(17, 0, 192396, 0, 0, 48, 0, 280776, 0, 0, 0, 98, '', 'Cast if criteria completed'),
(17, 0, 195713, 0, 0, 48, 0, 280777, 0, 0, 0, 98, '', 'Cast if criteria completed');

delete from gossip_menu_option where menu_id in (18927);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
(18927, 0, 0, 'Идем на охоту!>', 1, 1, 99155);

update creature_template set ainame = 'SmartAI' where entry in (94151,94198,99481,94149,94238,98808);

delete from smart_scripts where entryorguid in (94434, 94151,94198,99481,94149,94238,98808);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(94434, 0, 0, 1, 62, 0, 100, 0, 18927, 0, 0, 0, 0, 33, 99034, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Addi - on gossip select - give credit'),
(94434, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 85, 188723, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Addi - on link - invoker cast'),

(94151, 0, 0, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 85, 196679, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Animal - on death - invoker cast'),
(94198, 0, 0, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 85, 196679, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Animal - on death - invoker cast'),
(99481, 0, 0, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 85, 196679, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Animal - on death - invoker cast'),
(94149, 0, 0, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 85, 196679, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Animal - on death - invoker cast'),
(94238, 0, 0, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 85, 196679, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Animal - on death - invoker cast'),
(98808, 0, 0, 0, 6, 0, 100, 0, 0, 0, 0, 0, 0, 85, 196679, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Animal - on death - invoker cast');

delete from conditions where SourceEntry in (94151,94198,99481,94149,94238,98808) and SourceTypeOrReferenceId = 22;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 94151, 0, 0, 9, 0, 40216, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 94198, 0, 0, 9, 0, 40216, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 99481, 0, 0, 9, 0, 40216, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 94149, 0, 0, 9, 0, 40216, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 94238, 0, 0, 9, 0, 40216, 0, 0, 0, 0, '', 'Smart Only If player has quest'),
(22, 1, 98808, 0, 0, 9, 0, 40216, 0, 0, 0, 0, '', 'Smart Only If player has quest'),

(22, 1, 94151, 0, 0, 1, 0, 188723, 0, 0, 0, 0, '', 'Smart Only If player has aura'),
(22, 1, 94198, 0, 0, 1, 0, 188723, 0, 0, 0, 0, '', 'Smart Only If player has aura'),
(22, 1, 99481, 0, 0, 1, 0, 188723, 0, 0, 0, 0, '', 'Smart Only If player has aura'),
(22, 1, 94149, 0, 0, 1, 0, 188723, 0, 0, 0, 0, '', 'Smart Only If player has aura'),
(22, 1, 94238, 0, 0, 1, 0, 188723, 0, 0, 0, 0, '', 'Smart Only If player has aura'),
(22, 1, 98808, 0, 0, 1, 0, 188723, 0, 0, 0, 0, '', 'Smart Only If player has aura');


-- https://forum.wowcircle.net/showthread.php?t=813629
delete from creature where id in (49211,49215,49216,49227);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(49211, 1, 5034, 5669, 1, 1, 0, 36415, 0, -9264.3, 460.67, 242.899, 3.54302, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(49215, 1, 5034, 5669, 1, 1, 0, 36415, 0, -9302.12, 498.385, 242.895, 3.54302, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(49216, 1, 5034, 5669, 1, 1, 0, 36415, 0, -9335.44, 457.955, 242.875, 3.54302, 300, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

delete from creature_template_addon where entry in (49211,49215,49216);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(49211, 0, 0, 0, '83323 91758 63048'),
(49215, 0, 0, 0, '83323 91758 63048'),
(49216, 0, 0, 0, '83323 91758 63048');

update creature_template set ainame = 'SmartAI' where entry in (49211,49215,49216);

delete from smart_scripts where entryorguid in (49211,49215,49216);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(49211, 0, 0, 1, 8, 0, 100, 0, 91957, 0, 0, 0, 0, 33, 49221, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Flare - on spell hit - give credit'),
(49215, 0, 0, 1, 8, 0, 100, 0, 91957, 0, 0, 0, 0, 33, 49222, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Flare - on spell hit - give credit'),
(49216, 0, 0, 1, 8, 0, 100, 0, 91957, 0, 0, 0, 0, 33, 49223, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Flare - on spell hit - give credit'),

(49211, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 12, 49227, 1, 60000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Flare - on link - summon creature'),
(49215, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 12, 49227, 1, 60000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Flare - on link - summon creature'),
(49216, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 12, 49227, 1, 60000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Flare - on link - summon creature'),

(49211, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Flare - on link - despawn'),
(49215, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Flare - on link - despawn'),
(49216, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Flare - on link - despawn');

DELETE FROM `spell_area` WHERE `spell` in (83322) and area = 5669;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(83322, 5669, 28622, 28622, 0, 0, 2, 1, 8, 66);

delete from conditions where SourceEntry in (91957) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 1, 91957, 0, 0, 31, 0, 3, 49211, 0, 0, 0, '', 'Cast only 49211'),
(13, 1, 91957, 0, 1, 31, 0, 3, 49215, 0, 0, 0, '', 'Cast only 49215'),
(13, 1, 91957, 0, 2, 31, 0, 3, 49216, 0, 0, 0, '', 'Cast only 49216');


-- https://forum.wowcircle.net/showthread.php?t=810681
update quest_template set flags = 36766720 where id in (42636);


-- https://forum.wowcircle.net/showthread.php?t=814916
delete from npc_spellclick_spells where npc_entry in (35129,35196);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(35196, 66778, 1, 0),
(35129, 46598, 1, 0);

update creature_template set npcflag = 16777216, ainame = 'SmartAI' where entry in (35129);

delete from smart_scripts where entryorguid in (35129);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(35129, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 85, 46598, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Crosher - on just summoned - invoker cast'),
(35129, 0, 1, 0, 28, 0, 100, 1, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Crosher - on passenger remove - despawn');

delete from conditions where SourceEntry in (66795) and SourceTypeOrReferenceId = 13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 2, 66795, 0, 0, 31, 0, 5, 195361, 0, 0, 0, '', 'Cast only 195361');