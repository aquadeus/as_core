-- https://forum.wowcircle.net/showthread.php?t=1061898
delete from creature_loot_template where entry = 108190;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `requiredAura`, `requiredClass`, `difficultyMask`) VALUES 
(108190, 138392, -30, 1, 0, 1, 1, 0, 0, 0);


-- https://forum.wowcircle.net/showthread.php?t=1062303
delete from smart_scripts where entryorguid in (116779,116798,119575);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(119575, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'warrior - on just summoned - follow'),

(116779, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 42, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on update - set invincibility'),
(116779, 0, 1, 2, 2, 0, 100, 1, 0, 1, 0, 0, 0, 11, 31261, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on 1 hp - cast spell'),
(116779, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 18, 768, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - set unit flag'),
(116779, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - set faction'),
(116779, 0, 4, 5, 61, 0, 100, 1, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - evade'),
(116779, 0, 5, 6, 61, 0, 100, 1, 0, 0, 0, 0, 0, 82, 16777216, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - set npcflag'),
(116779, 0, 6, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 60000, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on link - despawn'),
(116779, 0, 7, 0, 8, 0, 100, 1, 237550, 0, 0, 0, 0, 41, 0, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on s[ellhit - despawn'),

(116798, 0, 0, 0, 60, 0, 100, 1, 0, 0, 0, 0, 0, 42, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on update - set invincibility'),
(116798, 0, 1, 2, 2, 0, 100, 1, 0, 1, 0, 0, 0, 11, 31261, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on 1 hp - cast spell'),
(116798, 0, 2, 3, 61, 0, 100, 1, 0, 0, 0, 0, 0, 18, 768, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - set unit flag'),
(116798, 0, 3, 4, 61, 0, 100, 1, 0, 0, 0, 0, 0, 2, 35, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - set faction'),
(116798, 0, 4, 5, 61, 0, 100, 1, 0, 0, 0, 0, 0, 24, 0, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - evade'),
(116798, 0, 5, 6, 61, 0, 100, 1, 0, 0, 0, 0, 0, 82, 16777216, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - set npcflag'),
(116798, 0, 6, 0, 61, 0, 100, 1, 0, 0, 0, 0, 0, 41, 20000, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'berserk - on link - despawn'),
(116798	, 0, 7, 0, 8, 0, 100, 1, 237550, 0, 0, 0, 0, 41, 0, 0 , 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'tamer - on s[ellhit - despawn');


-- https://forum.wowcircle.net/showthread.php?t=1062302
update creature_template set VehicleId = 5236 where entry = 116828;

delete from vehicle_template_accessory where entryoraura in (116828);
INSERT INTO `vehicle_template_accessory` (`EntryOrAura`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `description`) VALUES 
(116828, 116827, 0, 0, 7, 0, '');

delete from creature where id = 116827;

delete from npc_spellclick_spells where npc_entry in (116828);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(116828, 46598, 1, 0);


-- https://forum.wowcircle.net/showthread.php?t=1061991
delete from creature where id = 91242;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(91242, 1116, 6721, 7520, 1, 1, 0, 0, 0, 8649.47, 428.792, 12.4305, 2.0178, 300, 0, 0, 435, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(91242, 1116, 6721, 7520, 1, 1, 0, 0, 0, 8680.76, 930.766, 56.5138, 0.0228808, 300, 0, 0, 435, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(91242, 1116, 6719, 7760, 1, 1, 0, 0, 0, 2158.22, 423.517, 16.5374, 5.90552, 300, 0, 0, 435, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);


-- https://forum.wowcircle.net/showthread.php?t=1061995
update creature set phaseMask = 3 where id = 108870 and guid = 11843126;


-- https://forum.wowcircle.net/showthread.php?t=1062188
update creature_template set gossip_menu_id = entry, AIName = 'SmartAI' where entry in (98102,94159,94138);
update creature_template set gossip_menu_id = 19545, AIName = 'SmartAI' where entry in (101513);

delete from gossip_menu_option where menu_id in (98102,94159,94138,19545);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`, `action_menu_id`) VALUES 
(19545, 0, 0, 'I\'m ready to make a decision.', 1, 1, 108894, 0),

(98102, 0, 0, 'Tell me about your plan.', 1, 1, 108896, 981020),
(94159, 0, 0, 'Tell me about your plan.', 1, 1, 108896, 941590),
(94138, 0, 0, 'Tell me about your plan.', 1, 1, 108896, 941380);

delete from gossip_menu where entry in (98102,94159,94138,981020,941380,941590);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
(98102, 98102),
(981020, 981020),

(94159, 94159),
(941590, 941590),

(94138, 94138),
(941380, 941380);

delete from npc_text where id in (98102,94159,94138,981020,941380,941590);
INSERT INTO `npc_text` (`ID`, `BroadcastTextID0`, `BroadcastTextID1`) VALUES 
(98102, 108903, 108903),
(981020, 108904, 108904),

(94159, 108899, 108899),
(941590, 108902, 108902),

(94138, 108895, 108895),
(941380, 108897, 108897);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 15 AND `SourceGroup` in (98102,94159,94138,19545);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 98102, 0, 0, 0, 9, 0, 40840, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),
(15, 94159, 0, 0, 0, 9, 0, 40840, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),
(15, 94138, 0, 0, 0, 9, 0, 40840, 0, 0, 0, 0, '', 'Gossip if quest incomplete'),

(15, 98102, 0, 0, 0, 48, 0, 283505, 0, 0, 1, 0, '', 'Gossip if criteria incomplete'),
(15, 94159, 0, 0, 0, 48, 0, 283504, 0, 0, 1, 0, '', 'Gossip if criteria incomplete'),
(15, 94138, 0, 0, 0, 48, 0, 281294, 0, 0, 1, 0, '', 'Gossip if criteria incomplete'),

(15, 19545, 0, 0, 0, 48, 0, 283505, 0, 0, 0, 0, '', 'Gossip if criteria complete'),
(15, 19545, 0, 0, 0, 48, 0, 283504, 0, 0, 0, 0, '', 'Gossip if criteria complete'),
(15, 19545, 0, 0, 0, 48, 0, 281294, 0, 0, 0, 0, '', 'Gossip if criteria complete'),
(15, 19545, 0, 0, 0, 48, 0, 283506, 0, 0, 1, 0, '', 'Gossip if criteria complete'),
(15, 19545, 0, 0, 0, 9, 0, 40840, 0, 0, 0, 0, '', 'Gossip if quest incomplete');

delete from smart_scripts where entryorguid in (98102,94159,94138);
delete from smart_scripts where entryorguid in (101513) and id = 1;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(101513, 0, 1, 0, 62, 0, 100, 0, 19545, 0, 0, 0, 0, 85, 201343, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'jorach - on gossip select - invoker cast'),

(98102, 0, 0, 0, 62, 0, 100, 0, 98102, 0, 0, 0, 0, 33, 98102, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'valeera - on gossip select - give credit'),
(94159, 0, 0, 0, 62, 0, 100, 0, 94159, 0, 0, 0, 0, 33, 94159, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'tetis - on gossip select - give credit'),
(94138, 0, 0, 0, 62, 0, 100, 0, 94138, 0, 0, 0, 0, 33, 94138, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'tess - on gossip select - give credit');

delete from playerchoice_response_reward_spellcast where choiceid = 262 and responseid in (568,569,570);
INSERT INTO `playerchoice_response_reward_spellcast` (`ChoiceId`, `ResponseId`, `Index`, `SpellId`, `VerifiedBuild`) VALUES 
(262, 568, 0, 201345, 26365),
(262, 569, 0, 201346, 26365),
(262, 570, 0, 201347, 26365);