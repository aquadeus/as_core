-- https://forum.wowcircle.com/showthread.php?t=1058086
delete from npc_text where id in (22900,22903,22904,22905,22906,22908,22909,22918,22920,22921,22922,22923,22924,22925,22926,22927,22928,22929,22931,229320,22934,22935,22936,22937,22939,22940,22941,22942,22943,22944,22945,22946);
INSERT INTO `npc_text` (`id`, `BroadcastTextID0`, `text0_1`) VALUES 
(22900, 76189, 'Let us test your knowledge of history, then! What are undead murlocs called?'),
(22903, 76195, 'Let us test your knowledge of history, then! White wolves were once the favored mounts of which orc clan?'),
(22904, 76196, 'Let us test your knowledge of history, then! What is the highest rank bestowed on a druid?'),
(22905, 76197, 'Let us test your knowledge of history, then! Who is the current leader of the gnomish people?'),
(22906, 76198, 'Let us test your knowledge of history, then! Malfurion Stormrage helped found this group which is the primary druidic organization of Azeroth'),
(22908, 76200, 'Let us test your knowledge of history, then! Before she was raised from the dead by Arthas to serve the Scourge, Sindragosa was a part of what dragonflight?'),
(22909, 76201, 'Let us test your knowledge of history, then! This structure, located in Zangarmarsh was controlled by naga who sought to drain a precious and limited resource: the water of Outland.'),
(22918, 76210, 'Let us test your knowledge of history, then! What did the dragon aspects give the night elves after the War of the Ancients?'),
(22920, 76212, 'Let us test your knowledge of history, then! Arthas\'s death knights were trained in a floating citadel that was taken by force when many of them rebelled against the Lich King. What was the fortress\'s name?'),
(22921, 76213, 'Let us test your knowledge of history, then! Thane Kurdran Wildhammer recently suffered a tragic loss when his valiant gryphon was killed in a fire. What was this gryphon\'s name?'),
(22922, 76214, 'Let us test your knowledge of history, then! This queen oversaw the evacuation of her people after the Cataclysm struck and the Forsaken attacked her nation.'),
(22923, 76215, 'Let us test your knowledge of history, then! Before Ripsnarl became a worgen, he had a family. What was his wife\'s name?'),
(22924, 76216, 'Let us test your knowledge of history, then! Who was the first death knight to be created on Azeroth?'),
(22925, 76217, 'Let us test your knowledge of history, then! Formerly a healthy paladin, this draenei fell ill after fighting the Burning Legion and becoming one of the Broken. He later became a powerful shaman.'),
(22926, 76218, 'Let us test your knowledge of history, then! Who were the three young twilight drakes guarding the twilight dragon eggs in the Obsidian Sanctum?'),
(22927, 76219, 'Let us test your knowledge of history, then! What is the name of Tirion Fordring\'s gray stallion?'),
(22928, 76220, 'Let us test your knowledge of history, then! This Horde ship was crafted by goblins. Originally intended to bring Thrall and Aggra to the Maelstrom, the ship was destroyed in a surprise attack by the Alliance.'),
(22929, 76221, 'Let us test your knowledge of history, then! Name the titan lore-keeper who was a member of the elite Pantheon'),
(22931, 76223, 'Let us test your knowledge of history, then! The draenei like to joke that in the language of the naaru, the word Exodar has this meaning'),
(229320, 76224, 'Let us test your knowledge of history, then! Whose tomb includes the inscription \"May the bloodied crown stay lost and forgotten\"?'),
(22934, 76226, 'Let us test your knowledge of history, then! Wich of these is the correct name for King Varian Wrynn\'s first wife?'),
(22935, 76227, 'Let us test your knowledge of history, then! Name the homeworld of the ethereals.'),
(22936, 76228, 'Let us test your knowledge of history, then! Before the original Horde formed, a highly contagious sickness began spreading rapidly among the orcs. What did the orcs call it?'),
(22937, 76229, 'Let us test your knowledge of history, then! This defender of the Scarlet Crusade was killed while slaying the dreadlord Beltheris'),
(22939, 76231, 'Let us test your knowledge of history, then! In the assault on Icecrown, Horde forces dishonorably attacked Alliance soldiers who were busy fighting Scourge and trying to capture this gate.'),
(22940, 76232, 'Let us test your knowledge of history, then! What phrase means \"Thank you\" in Draconic, the language of the dragons?'),
(22941, 76233, 'Let us test your knowledge of history, then! While working as a tutor, Stalvan Mistmantle became obsessed with one of his students, a young woman named Tilloa. What was the name of her younger brother?'),
(22942, 76234, 'Let us test your knowledge of history, then! Not long ago, this frail Zandalari troll sought to tame a direhorn. Although he journeyed to the Isle of Giants, he was slain in his quest. What was his name?'),
(22943, 76235, 'Let us test your knowledge of history, then! This emissary of the Horde felt that Silvermoon City was a little too bright and clean.'),
(22944, 76236, 'Let us test your knowledge of history, then! Succubus demons revel in causing anguish, and they serve the Legion by conducting nightmarish interrogations. What species is the succubus?'),
(22945, 76237, 'Let us test your knowledge of history, then! One name for this loa is \"Night\'s Friend\"'),
(22946, 76238, 'Let us test your knowledge of history, then! In Taur-ahe, the language of the tauren, what does lar\'kowi mean?');


-- https://forum.wowcircle.com/showthread.php?t=1058126
delete from creature_template_addon where entry in (107602);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(107602, 0, 8, 0, '211179');

update creature_template set AIName = 'SmartAI' where entry in (107554,107602,107597);

delete from creature_text where Entry in (107554,107602);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`) VALUES 
(107554, 0, 0, 'Nazgrim, your Deathlord calls! Feel the icy grip of your master and rise once more!', 12, 0, 100, 0, 0, 0, 112573),
(107554, 1, 0, 'Long enough, my friend. The Burning Legion has returned and you have been called to the battle.', 12, 0, 100, 0, 0, 0, 112588),
(107602, 0, 0, 'G\'Hanir is in danger? Then there is no time to waste.', 12, 0, 100, 0, 0, 0, 112584),
(107602, 1, 0, 'Say no more. I will sever their heads from their bodies...', 12, 0, 100, 0, 0, 0, 112587);

delete from smart_scripts where entryorguid in (107554,107602,10760200,107597);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(107597, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 12, 107602, 1, 25000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on just summoned - summon creature'),

(107602, 0, 0, 0, 54, 0, 100, 1, 0, 0, 0, 0, 0, 80, 10760200, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Nazgrim - on just summoned - timed action list'),

(10760200, 9, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 107554, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(10760200, 9, 1, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(10760200, 9, 2, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 84, 1, 0, 0, 0, 0, 0, 19, 107554, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(10760200, 9, 3, 0, 0, 0, 100, 1, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - talk'),
(10760200, 9, 4, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - remove aura'),
(10760200, 9, 5, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Timed action list - despawn'),

(107554, 0, 0, 0, 19, 0, 100, 0, 42484, 0, 0, 0, 0, 85, 213932, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Tasarian - on quest accept - invoker cast');

delete from spell_linked_spell where spell_trigger in (213633) and spell_effect in (-213932);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(213633, -213932, 0, 0, 0, 0, 0, '');

delete from spell_area where spell = 213932;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES 
(213932, 14, 0, 0, 0, 0, 2, 0, 0, 0);

delete from spell_scripts where id in (213633);

delete from spell_target_position where id = 213633;
INSERT INTO `spell_target_position` (`id`, `effIndex`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`, `VerifiedBuild`) VALUES 
(213633, 0, 1, 1180.0187, -4467.6528, 21.2995, 0.75, 1);


-- https://forum.wowcircle.com/showthread.php?t=1058274
update gameobject_template set AIName = 'SmartGameObjectAI' where entry in (252772,252774,252408,260494,260498,260495,260492,260493,260249,260248,260247);

delete from smart_scripts where entryorguid in (252772,252774,252408,260494,260498,260495,260492,260493,260249,260248,260247);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(252772, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(252774, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(252408, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(260494, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(260498, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(260495, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(260492, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(260493, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(260249, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(260248, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast'),
(260247, 1, 0, 0, 70, 0, 100, 1, 2, 0, 0, 0, 0, 85, 228183, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Kristal - on just deactivated - invoker cast');


-- https://forum.wowcircle.com/showthread.php?t=1058081
DELETE FROM `creature_queststarter` WHERE quest in (40839);
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(101513, 40839);

delete from spell_linked_spell where spell_trigger in (210001) and spell_effect in (210518);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(210001, 210518, 0, 0, 0, 0, 0, '');


-- https://forum.wowcircle.com/showthread.php?t=1058269
delete from smart_scripts where entryorguid in (993619,101576,101574);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(101576, 0, 0, 1, 10, 100, 0, 0, 1, 1, 1000, 1000, 0, 85, 200330, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trap - on ooc los - invoker cast'),
(101576, 0, 1, 2, 61, 100, 0, 0, 0, 0, 0, 0, 0, 12, 993619, 2, 50000, 0, 0, 0, 8, 0, 0, 0, 1664.11, 3899.86, 168.246, 2, 'Trap - on link - summon creature'),
(101576, 0, 2, 0, 61, 100, 0, 0, 0, 0, 0, 0, 0, 120, 78, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Trap - on link - remove mount aura'),

(101574, 0, 0, 1, 1, 100, 1, 0, 30000, 30000, 100, 100, 0, 28, 228427, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Trigger - ooc - remove aura'),
(101574, 0, 1, 2, 61, 100, 1, 0, 0, 0, 0, 0, 0, 28, 200330, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - remove aura'),
(101574, 0, 2, 3, 61, 100, 1, 0, 0, 0, 0, 0, 0, 62, 1220, 0, 0, 0, 0, 0, 23, 0, 0, 0, -32.7256, 2352.84, 0, 1.35, 'Trigger - on link - teleport'),
(101574, 0, 3, 0, 61, 100, 1, 0, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on link - die'),
(101574, 0, 4, 0, 8, 100, 1, 0, 200170, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Trigger - on spell hit despawn'),

(993619, 0, 0, 0, 1, 100, 1, 0, 100, 100, 0, 0, 0, 53, 0, 9936190, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - ooc - start waypoint'),
(993619, 0, 1, 0, 1, 100, 1, 0, 100, 100, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - ooc - talk'),
(993619, 0, 2, 0, 1, 100, 1, 0, 6000, 6000, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.65, 'Okuleth - ooc - set orientation'),
(993619, 0, 3, 0, 1, 100, 1, 0, 6000, 6000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - ooc - talk'),
(993619, 0, 4, 0, 1, 100, 1, 0, 11500, 11500, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - ooc - talk'),
(993619, 0, 5, 0, 1, 100, 1, 0, 11500, 11500, 0, 0, 0, 5, 25, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - ooc - play emote'),
(993619, 0, 6, 7, 8, 100, 1, 0, 200170, 0, 0, 0, 0, 1, 3, 4000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on spell hit - talk'),
(993619, 0, 7, 8, 61, 100, 1, 0, 0, 0, 0, 0, 0, 5, 463, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on link - play emote'),
(993619, 0, 8, 0, 61, 100, 1, 0, 0, 0, 0, 0, 0, 33, 101499, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on link - give credit'),
(993619, 0, 11, 0, 52, 100, 1, 0, 3, 993619, 0, 0, 0, 1, 4, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on link - talk'),
(993619, 0, 12, 0, 52, 100, 1, 0, 4, 993619, 0, 0, 0, 1, 5, 6000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on link - talk'),
(993619, 0, 13, 0, 52, 100, 1, 0, 4, 993619, 0, 0, 0, 53, 1, 9936191, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on text over - start waypoint'),
(993619, 0, 14, 0, 40, 100, 1, 0, 2, 9936191, 0, 0, 0, 54, 4000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - waypoint reached - pause waypoint'),
(993619, 0, 15, 16, 40, 100, 1, 0, 3, 9936191, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.65, 'Okuleth - waypoint reached - set orientation'),
(993619, 0, 16, 0, 61, 100, 1, 0, 0, 0, 0, 0, 0, 1, 6, 3000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on link - talk'),
(993619, 0, 17, 18, 52, 100, 1, 0, 6, 993619, 0, 0, 0, 1, 7, 1000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - on text over - talk'),
(993619, 0, 18, 19, 61, 100, 1, 0, 0, 0, 0, 0, 0, 5, 463, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - link - play emote'),
(993619, 0, 19, 0, 61, 100, 1, 0, 0, 0, 0, 0, 0, 28, 200330, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'Okuleth - on link - remove aura'),
(993619, 0, 20, 0, 40, 100, 1, 0, 3, 9936191, 0, 0, 0, 54, 4000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - waypoint reached - pause waypoint'),
(993619, 0, 21, 0, 40, 100, 1, 0, 4, 9936191, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Okuleth - aypoint reached - despawn');

delete from creature_text where entry = 993619;
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 
(993619, 0, 0, 'That is far enough.', 12, 0, 100, 0, 0, 67418, 103939, 'Oculeth talk'),
(993619, 1, 0, 'Well, you are not from the palace... and you are no demon, at least not entirely.', 12, 0, 100, 0, 0, 67419, 103940, 'Oculeth talk'),
(993619, 2, 0, 'Identify yourself, before I teleport you to the depths of the Great Sea.', 12, 0, 100, 0, 0, 67420, 103943, 'Oculeth talk'),
(993619, 3, 0, 'What is this? A coin...', 12, 0, 100, 0, 0, 67421, 103949, 'Oculeth emote'),
(993619, 4, 0, 'The sign of the dusk lily... This is the First Arcanist\'s seal! Is she alive?!', 12, 0, 100, 0, 0, 67422, 103950, 'Oculeth talk'),
(993619, 5, 0, 'Then there is still a chance... Come quickly! I need your help.', 12, 0, 100, 0, 0, 67423, 103969, 'Oculeth talk'),
(993619, 6, 0, 'Right. The trap.', 12, 0, 100, 0, 0, 67424, 103951, 'Oculeth talk'),
(993619, 7, 0, 'N\'eth ana!', 12, 0, 100, 0, 0, 67425, 103967, 'Oculeth talk');

delete from spell_linked_spell where spell_trigger in (200170) and spell_effect in (-228427);
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `hastalent`, `hastalent2`, `chance`, `cooldown`, `comment`) VALUES 
(200170, -228427, 0, 0, 0, 0, 0, '');

delete from conditions where SourceTypeOrReferenceId = 22 and SourceEntry in (101576,993619);
delete from conditions where SourceTypeOrReferenceId = 13 and SourceEntry in (200170);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(22, 1, 101576, 0, 0, 31, 0, 4, 0, 0, 0, 0, '', 'SAI only works if Target is a player'),
(22, 1, 101576, 0, 0, 9, 0, 40011, 0, 0, 0, 0, '', 'SAI only works if Target has quest 40011'),
(22, 1, 101576, 0, 0, 1, 0, 200330, 2, 0, 1, 0, '', 'SAI only works if Target does not Telemancers trap aura');


-- https://forum.wowcircle.com/showthread.php?t=1058273
delete from creature where id = 101499;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
(101499, 1220, 7637, 7966, 1, 0, 407471, 0, 0, 1700.95, 3919.66, 167.901, 3.70909, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1),
(101499, 1220, 7637, 7966, 1, 0, 407470, 0, 0, 1664.11, 3899.86, 168.246, 2.04944, 120, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', '', -1);

update quest_template set SpecialFlags = 0 where id in (90000,90002,90003,90001);

delete from phase_definitions where zoneId = 7637 and entry in (407471,407470);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `flags`, `comment`) VALUES 
(7637, 407471, 0, 407471, 0, '40747 quest'),
(7637, 407470, 0, 407470, 0, '40747 quest');

delete from conditions where SourceGroup in (7637) and SourceTypeOrReferenceId = 26 and SourceEntry in (407471,407470);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(26, 7637, 407471, 0, 0, 28, 0, 40747, 0, 0, 0, 0, '', 'Phase if quest complete'),
(26, 7637, 407471, 0, 1, 8, 0, 40747, 0, 0, 0, 0, '', 'Phase if quest reward'),

(26, 7637, 407470, 0, 0, 28, 0, 40747, 0, 0, 1, 0, '', 'Phase if quest not complete'),
(26, 7637, 407470, 0, 0, 8, 0, 40747, 0, 0, 1, 0, '', 'Phase if quest not reward');

update gameobject_template set flags = 0 where entry = 246229;

delete from spell_area where spell = 64574 and area = 7966;
delete from creature where id = 993620;