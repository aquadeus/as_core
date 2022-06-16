-- https://ru.wowhead.com/quest=35947
-- https://ru.wowhead.com/quest=35959

-- https://ru.wowhead.com/npc=83706
-- https://ru.wowhead.com/npc=83749
-- https://ru.wowhead.com/npc=83706

UPDATE `creature_template` SET `unit_flags`='32768', `unit_flags2`='2048', `AIName`='SmartAI' WHERE (`entry`='83760');
UPDATE `creature_template` SET `unit_flags`='32768', `unit_flags2`='2048', `AIName`='SmartAI' WHERE (`entry`='83749');
UPDATE `creature_template` SET `unit_flags`='32768', `unit_flags2`='2048', `AIName`='SmartAI' WHERE (`entry`='83706');

UPDATE `creature_template_wdb` SET `KillCredit1`='0' WHERE (`Entry`='83760');
UPDATE `creature_template_wdb` SET `KillCredit1`='0' WHERE (`Entry`='83749');
UPDATE `creature_template_wdb` SET `KillCredit1`='0' WHERE (`Entry`='83706');

UPDATE `creature_template_addon` SET `auras`='' WHERE (`entry`='83760');

delete from smart_scripts where source_type=0 and entryorguid in (83760,83749,83706);   
delete from smart_scripts where source_type=9 and entryorguid in (83760*100,83760*100+1);   
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(83706, '', 0, 0, 0, 8, 0, 100, 0, 0, 167566, 0, 0, 0, 0, 87, 83760*100, 83760*100+1, 83760*100, 83760*100, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(83706, '', 0, 1, 0, 4, 0, 30, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(83706, '', 0, 2, 0, 6, 0, 30, 0, 0, 0, 0, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(83706, '', 0, 3, 0, 0, 0, 100, 0, 0, 4000, 6000, 10000, 15000, 0, 11, 32714, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83706, '', 0, 4, 0, 9, 0, 100, 0, 0, 8, 40, 60300, 60300, 0, 11, 175022, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),

(83749, '', 0, 0, 0, 8, 0, 100, 0, 0, 167566, 0, 0, 0, 0, 87, 83760*100, 83760*100+1, 83760*100, 83760*100, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(83749, '', 0, 1, 0, 4, 0, 30, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(83749, '', 0, 2, 0, 6, 0, 30, 0, 0, 0, 0, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(83749, '', 0, 3, 0, 0, 0, 100, 0, 0, 0, 500, 10000, 15000, 0, 11, 128252, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(83749, '', 0, 4, 0, 0, 0, 100, 0, 0, 2000, 3000, 9000, 12000, 0, 11, 175014, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),

(83760, '', 0, 0, 0, 8, 0, 100, 0, 0, 167566, 0, 0, 0, 0, 87, 83760*100, 83760*100+1, 83760*100, 83760*100, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(83760, '', 0, 1, 0, 4, 0, 30, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),
(83760, '', 0, 2, 0, 6, 0, 30, 0, 0, 0, 0, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(83760, '', 0, 3, 0, 4, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 175098, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760, '', 0, 4, 0, 0, 0, 100, 0, 0, 2000, 3000, 6000, 7000, 0, 11, 175105, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),

(83760*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 84, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, ""),

(83760*100+1, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 84, 1, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 2, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 18, 898, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 5, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 6, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 7, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 8, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 9, 0, 0, 0, 100, 0, 0, 4800, 4800, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 10, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 19, 898, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 11, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 12, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 83722, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 13, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83760*100+1, '', 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");


delete from creature_text where Entry in (83760,83749,83706);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(83706, 1, 0, "Who crafted the poison that killed the arakkoa?", 12, 0, 100, 0, 0, 0, 85209, "p"),
(83706, 1, 1, "Who is behind this? Where are they?", 12, 0, 100, 0, 0, 0, 85210, "p"),
(83706, 1, 2, "Who creates the poison on your blade. Tell me", 12, 0, 100, 0, 0, 0, 85211, "p"),
(83706, 1, 3, "I need answers. Tell me who created the poison you use.", 12, 0, 100, 0, 0, 0, 85212, "p"),
(83706, 1, 4, "You did not craft this poison. Tell me who did and this can all end.", 12, 0, 100, 0, 0, 0, 85213, "p"),
(83706, 1, 5, "Who is G.V.? His poisons killed the arakkoa. Tell me!", 12, 0, 100, 0, 0, 0, 85214, "p"),
(83706, 1, 6, "Who led the raid on the arakkoa. Where is he hiding?", 12, 0, 100, 0, 0, 0, 85215, "p"),
(83706, 1, 7, "Do not test my patience. Tell me who crafted the poison you wield.", 12, 0, 100, 0, 0, 0, 85216, "p"),
(83706, 1, 8, "Do not waste my time. Who is G.V. and where is he?", 12, 0, 100, 0, 0, 0, 85217, "p"),

(83706, 2, 0, "You call this pain? I've done worse to myself.", 12, 0, 100, 0, 0, 0, 85190, "n"),
(83706, 2, 1, "Your poison is pathetic.", 12, 0, 100, 0, 0, 0, 85191, "n"),
(83706, 2, 2, "Yes... the pain... it sustains me...", 12, 0, 100, 0, 0, 0, 85192, "n"),
(83706, 2, 3, "More... give me more!", 12, 0, 100, 0, 0, 0, 85193, "n"),
(83706, 2, 4, "My will is iron. I'll never talk.", 12, 0, 100, 0, 0, 0, 85194, "n"),
(83706, 2, 5, "It's going to take more than that, whelp.", 12, 0, 100, 0, 0, 0, 85195, "n"),
(83706, 2, 6, "I'll chew on your bones..", 12, 0, 100, 0, 0, 0, 85196, "n"),
(83706, 2, 7, "You want pain? I'll show you pain.", 12, 0, 100, 0, 0, 0, 85197, "n"), 
(83706, 2, 8, "I took my own hand. You are nothing!", 12, 0, 100, 0, 0, 0, 85198, "n"),
(83706, 2, 10, "Yes, more. Give me more!", 12, 0, 100, 0, 0, 0, 85199, "n"),
(83706, 2, 11, "Kargath will hang your corpse on his throne.", 12, 0, 100, 0, 0, 0, 85200, "n"), 
(83706, 2, 12, "This is your idea of poison? I bathe with worse.", 12, 0, 100, 0, 0, 0, 85201, "n"), 
(83706, 2, 13, "Let me give you a taste of real poison.", 12, 0, 100, 0, 0, 0, 85202, "n"), 
(83706, 2, 14, "I've given everything to be Shattered Hand. You'll get nothing from me.", 12, 0, 100, 0, 0, 0, 85203, "n"),
(83706, 2, 15, "I live for pain. Your efforts are weak.", 12, 0, 100, 0, 0, 0, 85204, "n"),
(83706, 2, 16, "You are too late. Our poisoned blades will be your end.", 12, 0, 100, 0, 0, 0, 85205, "n"),

(83706, 3, 0, "STOP! No more!", 12, 0, 100, 0, 0, 0, 85206, "n"), 
(83706, 4, 0, "Our master assassin's name is Gardul Venomshiv. He crafted the poison.", 12, 0, 100, 0, 0, 0, 85207, "n"), 
(83706, 5, 0, "He is at the camp just down the road right now. But you'll never stop...", 12, 0, 100, 0, 0, 0, 85208, "n"), 
(83706, 6, 0, "%s succumbs to the poison.", 16, 0, 100, 20, 0, 0, 85221, "n"), 

(83706, 0, 0, "You must be feeling lucky.", 12, 0, 100, 0, 0, 0, 91257, "n"),
(83706, 0, 1, "I'll give you a scar to be proud of!", 12, 0, 100, 0, 0, 0, 91250, "n"),
(83706, 0, 2, "You must be feeling lucky.", 12, 0, 100, 0, 0, 0, 91258, "n"),
(83706, 0, 3, "Good. Something to skin!", 12, 0, 100, 0, 0, 0, 91259, "n"),
(83706, 0, 4, "Let me hear you scream!", 12, 0, 100, 0, 0, 0, 91260, "n"),
(83706, 0, 5, "I've got an empty hook for you.", 12, 0, 100, 0, 0, 0, 91256, "n"),

(83706, 7, 0, "You will not leave here alive.", 12, 0, 100, 0, 0, 0, 91262, "n"), 
(83706, 7, 1, "This... this is true pain...", 12, 0, 100, 0, 0, 0, 91261, "n"),
(83706, 7, 2, "I have never felt... more alive...", 12, 0, 100, 0, 0, 0, 91263, "n"),
(83706, 7, 3, "Put me on a hook...", 12, 0, 100, 0, 0, 0, 91264, "n"),

(83749, 1, 0, "Who crafted the poison that killed the arakkoa?", 12, 0, 100, 0, 0, 0, 85209, "p"),
(83749, 1, 1, "Who is behind this? Where are they?", 12, 0, 100, 0, 0, 0, 85210, "p"),
(83749, 1, 2, "Who creates the poison on your blade. Tell me", 12, 0, 100, 0, 0, 0, 85211, "p"),
(83749, 1, 3, "I need answers. Tell me who created the poison you use.", 12, 0, 100, 0, 0, 0, 85212, "p"),
(83749, 1, 4, "You did not craft this poison. Tell me who did and this can all end.", 12, 0, 100, 0, 0, 0, 85213, "p"),
(83749, 1, 5, "Who is G.V.? His poisons killed the arakkoa. Tell me!", 12, 0, 100, 0, 0, 0, 85214, "p"),
(83749, 1, 6, "Who led the raid on the arakkoa. Where is he hiding?", 12, 0, 100, 0, 0, 0, 85215, "p"),
(83749, 1, 7, "Do not test my patience. Tell me who crafted the poison you wield.", 12, 0, 100, 0, 0, 0, 85216, "p"),
(83749, 1, 8, "Do not waste my time. Who is G.V. and where is he?", 12, 0, 100, 0, 0, 0, 85217, "p"),

(83749, 2, 0, "You call this pain? I've done worse to myself.", 12, 0, 100, 0, 0, 0, 85190, "n"),
(83749, 2, 1, "Your poison is pathetic.", 12, 0, 100, 0, 0, 0, 85191, "n"),
(83749, 2, 2, "Yes... the pain... it sustains me...", 12, 0, 100, 0, 0, 0, 85192, "n"),
(83749, 2, 3, "More... give me more!", 12, 0, 100, 0, 0, 0, 85193, "n"),
(83749, 2, 4, "My will is iron. I'll never talk.", 12, 0, 100, 0, 0, 0, 85194, "n"),
(83749, 2, 5, "It's going to take more than that, whelp.", 12, 0, 100, 0, 0, 0, 85195, "n"),
(83749, 2, 6, "I'll chew on your bones..", 12, 0, 100, 0, 0, 0, 85196, "n"),
(83749, 2, 7, "You want pain? I'll show you pain.", 12, 0, 100, 0, 0, 0, 85197, "n"), 
(83749, 2, 8, "I took my own hand. You are nothing!", 12, 0, 100, 0, 0, 0, 85198, "n"),
(83749, 2, 10, "Yes, more. Give me more!", 12, 0, 100, 0, 0, 0, 85199, "n"),
(83749, 2, 11, "Kargath will hang your corpse on his throne.", 12, 0, 100, 0, 0, 0, 85200, "n"), 
(83749, 2, 12, "This is your idea of poison? I bathe with worse.", 12, 0, 100, 0, 0, 0, 85201, "n"), 
(83749, 2, 13, "Let me give you a taste of real poison.", 12, 0, 100, 0, 0, 0, 85202, "n"), 
(83749, 2, 14, "I've given everything to be Shattered Hand. You'll get nothing from me.", 12, 0, 100, 0, 0, 0, 85203, "n"),
(83749, 2, 15, "I live for pain. Your efforts are weak.", 12, 0, 100, 0, 0, 0, 85204, "n"),
(83749, 2, 16, "You are too late. Our poisoned blades will be your end.", 12, 0, 100, 0, 0, 0, 85205, "n"),

(83749, 3, 0, "STOP! No more!", 12, 0, 100, 0, 0, 0, 85206, "n"), 
(83749, 4, 0, "Our master assassin's name is Gardul Venomshiv. He crafted the poison.", 12, 0, 100, 0, 0, 0, 85207, "n"), 
(83749, 5, 0, "He is at the camp just down the road right now. But you'll never stop...", 12, 0, 100, 0, 0, 0, 85208, "n"), 
(83749, 6, 0, "%s succumbs to the poison.", 16, 0, 100, 20, 0, 0, 85221, "n"), 

(83749, 0, 0, "You must be feeling lucky.", 12, 0, 100, 0, 0, 0, 91257, "n"),
(83749, 0, 1, "I'll give you a scar to be proud of!", 12, 0, 100, 0, 0, 0, 91250, "n"),
(83749, 0, 2, "You must be feeling lucky.", 12, 0, 100, 0, 0, 0, 91258, "n"),
(83749, 0, 3, "Good. Something to skin!", 12, 0, 100, 0, 0, 0, 91259, "n"),
(83749, 0, 4, "Let me hear you scream!", 12, 0, 100, 0, 0, 0, 91260, "n"),
(83749, 0, 5, "I've got an empty hook for you.", 12, 0, 100, 0, 0, 0, 91256, "n"),

(83749, 7, 0, "You will not leave here alive.", 12, 0, 100, 0, 0, 0, 91262, "n"), 
(83749, 7, 1, "This... this is true pain...", 12, 0, 100, 0, 0, 0, 91261, "n"),
(83749, 7, 2, "I have never felt... more alive...", 12, 0, 100, 0, 0, 0, 91263, "n"),
(83749, 7, 3, "Put me on a hook...", 12, 0, 100, 0, 0, 0, 91264, "n"),

(83760, 1, 0, "Who crafted the poison that killed the arakkoa?", 12, 0, 100, 0, 0, 0, 85209, "p"),
(83760, 1, 1, "Who is behind this? Where are they?", 12, 0, 100, 0, 0, 0, 85210, "p"),
(83760, 1, 2, "Who creates the poison on your blade. Tell me", 12, 0, 100, 0, 0, 0, 85211, "p"),
(83760, 1, 3, "I need answers. Tell me who created the poison you use.", 12, 0, 100, 0, 0, 0, 85212, "p"),
(83760, 1, 4, "You did not craft this poison. Tell me who did and this can all end.", 12, 0, 100, 0, 0, 0, 85213, "p"),
(83760, 1, 5, "Who is G.V.? His poisons killed the arakkoa. Tell me!", 12, 0, 100, 0, 0, 0, 85214, "p"),
(83760, 1, 6, "Who led the raid on the arakkoa. Where is he hiding?", 12, 0, 100, 0, 0, 0, 85215, "p"),
(83760, 1, 7, "Do not test my patience. Tell me who crafted the poison you wield.", 12, 0, 100, 0, 0, 0, 85216, "p"),
(83760, 1, 8, "Do not waste my time. Who is G.V. and where is he?", 12, 0, 100, 0, 0, 0, 85217, "p"),

(83760, 2, 0, "You call this pain? I've done worse to myself.", 12, 0, 100, 0, 0, 0, 85190, "n"),
(83760, 2, 1, "Your poison is pathetic.", 12, 0, 100, 0, 0, 0, 85191, "n"),
(83760, 2, 2, "Yes... the pain... it sustains me...", 12, 0, 100, 0, 0, 0, 85192, "n"),
(83760, 2, 3, "More... give me more!", 12, 0, 100, 0, 0, 0, 85193, "n"),
(83760, 2, 4, "My will is iron. I'll never talk.", 12, 0, 100, 0, 0, 0, 85194, "n"),
(83760, 2, 5, "It's going to take more than that, whelp.", 12, 0, 100, 0, 0, 0, 85195, "n"),
(83760, 2, 6, "I'll chew on your bones..", 12, 0, 100, 0, 0, 0, 85196, "n"),
(83760, 2, 7, "You want pain? I'll show you pain.", 12, 0, 100, 0, 0, 0, 85197, "n"), 
(83760, 2, 8, "I took my own hand. You are nothing!", 12, 0, 100, 0, 0, 0, 85198, "n"),
(83760, 2, 10, "Yes, more. Give me more!", 12, 0, 100, 0, 0, 0, 85199, "n"),
(83760, 2, 11, "Kargath will hang your corpse on his throne.", 12, 0, 100, 0, 0, 0, 85200, "n"), 
(83760, 2, 12, "This is your idea of poison? I bathe with worse.", 12, 0, 100, 0, 0, 0, 85201, "n"), 
(83760, 2, 13, "Let me give you a taste of real poison.", 12, 0, 100, 0, 0, 0, 85202, "n"), 
(83760, 2, 14, "I've given everything to be Shattered Hand. You'll get nothing from me.", 12, 0, 100, 0, 0, 0, 85203, "n"),
(83760, 2, 15, "I live for pain. Your efforts are weak.", 12, 0, 100, 0, 0, 0, 85204, "n"),
(83760, 2, 16, "You are too late. Our poisoned blades will be your end.", 12, 0, 100, 0, 0, 0, 85205, "n"),

(83760, 3, 0, "STOP! No more!", 12, 0, 100, 0, 0, 0, 85206, "n"), 
(83760, 4, 0, "Our master assassin's name is Gardul Venomshiv. He crafted the poison.", 12, 0, 100, 0, 0, 0, 85207, "n"), 
(83760, 5, 0, "He is at the camp just down the road right now. But you'll never stop...", 12, 0, 100, 0, 0, 0, 85208, "n"), 
(83760, 6, 0, "%s succumbs to the poison.", 16, 0, 100, 20, 0, 0, 85221, "n"), 

(83760, 0, 0, "You must be feeling lucky.", 12, 0, 100, 0, 0, 0, 91257, "n"),
(83760, 0, 1, "I'll give you a scar to be proud of!", 12, 0, 100, 0, 0, 0, 91250, "n"),
(83760, 0, 2, "You must be feeling lucky.", 12, 0, 100, 0, 0, 0, 91258, "n"),
(83760, 0, 3, "Good. Something to skin!", 12, 0, 100, 0, 0, 0, 91259, "n"),
(83760, 0, 4, "Let me hear you scream!", 12, 0, 100, 0, 0, 0, 91260, "n"),
(83760, 0, 5, "I've got an empty hook for you.", 12, 0, 100, 0, 0, 0, 91256, "n"),

(83760, 7, 0, "You will not leave here alive.", 12, 0, 100, 0, 0, 0, 91262, "n"), 
(83760, 7, 1, "This... this is true pain...", 12, 0, 100, 0, 0, 0, 91261, "n"),
(83760, 7, 2, "I have never felt... more alive...", 12, 0, 100, 0, 0, 0, 91263, "n"),
(83760, 7, 3, "Put me on a hook...", 12, 0, 100, 0, 0, 0, 91264, "n");


-- https://ru.wowhead.com/npc=83773
-- https://ru.wowhead.com/npc=83772


UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='83773');
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='83772');

delete from smart_scripts where source_type=0 and entryorguid in (83773,83772);   
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(83773, '', 0, 0, 0, 19, 0, 100, 0, 0, 36023, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83773, '', 0, 1, 0, 10, 0, 100, 0, 0, 1, 10, 75000, 75000, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83773, '', 0, 2, 0, 20, 0, 100, 0, 0, 35959, 0, 0, 0, 0, 28, 167565, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(83772, '', 0, 0, 0, 19, 0, 100, 0, 0, 35924, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83772, '', 0, 1, 0, 10, 0, 100, 0, 0, 1, 10, 75000, 75000, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(83772, '', 0, 2, 0, 20, 0, 100, 0, 0, 35947, 0, 0, 30000, 0, 28, 167565, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "");


delete from creature_text where Entry in (83773,83772);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(83773, 0, 0, "Give them a piece o' their own medicine.", 12, 0, 100, 0, 0, 0, 85293, ""),
(83773, 1, 0, "Keep yer volume down, their camp is right here.", 12, 0, 100, 0, 0, 0, 85511, ""),

(83772, 0, 0, "Show them no mercy.", 12, 0, 100, 0, 0, 0, 85292, ""),
(83772, 1, 0, "Stay down. Their camp is right here.", 12, 0, 100, 0, 0, 0, 85512, "");


-- https://ru.wowhead.com/npc=83740

UPDATE `creature_template` SET `lootid`='83740', `AIName`='SmartAI', `InhabitType`='4' WHERE (`entry`='83740');
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE (`entry`='83596');

UPDATE `creature_template_addon` SET `bytes1`='50331648', `bytes2`='0', `auras`='167503' WHERE (`entry`='83740');

delete from smart_scripts where source_type=0 and entryorguid in (-342967,-342966,-342960,-837404,-342961,-837403,-342964,-342963,-837400);
delete from smart_scripts where source_type=0 and entryorguid in (83596);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(83596, '', 0, 0, 0, 1, 0, 100, 1, 0, 500, 1000, 0, 0, 0, 11, 167400, 0, 19, 54638, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(-342967, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 19, 54638, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(-342966, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 10, 343427, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(-342960, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 10, 343419, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(-837404, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 10, 343428, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(-342961, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 10, 343429, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(-837403, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 10, 576380, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(-342964, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 10, 343425, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(-342964, '', 0, 1, 0, 0, 0, 100, 1, 0, 6000, 10000, 10000, 20000, 0, 11, 163337, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(-342963, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 10, 343426, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(-342963, '', 0, 1, 0, 0, 0, 100, 1, 0, 6000, 10000, 10000, 20000, 0, 11, 163337, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(-837400, '', 0, 1, 0, 0, 0, 100, 1, 0, 6000, 10000, 10000, 20000, 0, 11, 163337, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(-837400, '', 0, 0, 0, 1, 0, 100, 1, 0, 1000, 1000, 0, 0, 0, 86, 167607, 0, 10, 554638, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""); 


delete from creature where id=83740;
INSERT INTO `creature` (`guid`, `linked_id`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
('837404', '65CC9AB4EA5CA4793CFC0C42976EFA782497719C', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '207.948', '1317.73', '11.7365', '0.835825', '300', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('837403', 'A80FFEE922A5B5F4CC7D51A60DC30133A44EF9C4', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '166.582', '1368.01', '3.29975', '5.66602', '300', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('837402', '7FF67643A24B2E373DDC6199C13CD0F0492A3047', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '249.614', '1419.61', '10.8753', '4.54055', '300', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('837401', '7E6A026B7942CB1AA7A6F53ABB48FD6EFBFF43A5', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '209.873', '1366.45', '0.452583', '5.23091', '300', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('837400', 'A44C5B6BE44EAC2DF49720D42FA90CEA7896E931', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '194.24', '1346.85', '0.455988', '0.298612', '300', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('342967', '6FF12C5497B3479210768E8BFAE7E0C8ABC80860', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '310.486', '1341.88', '11.8042', '3.49177', '120', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('342966', '630B2747CE73A73BD30A7D86BAD79DF2C8E2F2D3', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '248.828', '1337.69', '2.92405', '4.1085', '120', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('342965', '9D736D3393C3CC4CE105DA4ECBC92ECE0D609DD9', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '284.786', '1317.62', '0.445502', '3.82058', '120', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('342964', '5247DDB1D91C05C8F7F756A26F0CDCA6F05EAEB4', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '332.434', '1310.78', '5.28228', '2.38744', '120', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('342963', '1F6239A20EBD09661FF47FD65A9058120FC98D30', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '329.488', '1302', '5.20497', '3.0597', '120', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('342962', '010D7A5BF0C099F4693DF545BF8EC27DF22BBDBB', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '229.748', '1286.31', '1.1462', '2.45683', '120', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('342961', '1938E5319A3F49B68EBB68857C24DAFFB69B7EAD', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '217.462', '1314.14', '10.0799', '1.23181', '120', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('342960', '2ECA8FED9AA05EF373D4CEDE7C8EF57D3CF7281B', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '212.585', '1299.27', '3.77627', '6.128', '120', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1'),
('342959', 'E055E1278BCA0EF0DEAEFF14F5702FC9DD4DAC30', '83740', '1116', '6722', '7308', '1', '1', '0', '0', '0', '253.512', '1322.45', '0.445537', '5.04233', '120', '0', '0', '1', '0', '0', '0', '0', '537166592', '2049', '8192', '0', '0', '0', '', '', '-1');

delete from creature where id=54638 and areaId in (7302,7308);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `MovementType`) VALUES 
('546384', '54638', '1116', '6722', '7302', '1', '1', '0', '491.41', '1344.12', '94.6256', '1.21157', '300', '0', '0'),
('546383', '54638', '1116', '6722', '7302', '1', '1', '0', '371.545', '1448.3', '61.4007', '2.05038', '300', '0', '0'),
('576380', '54638', '1116', '6722', '7308', '1', '1', '0', '167.111', '1368.79', '16.2568', '5.05189', '300', '0', '0'),
('343428', '54638', '1116', '6722', '7308', '1', '1', '0', '208.391', '1313.24', '22.6176', '0', '120', '0', '0'),
('343429', '54638', '1116', '6722', '7308', '1', '1', '0', '217.302', '1313.87', '16.6151', '0.164284', '120', '0', '0'),
('343427', '54638', '1116', '6722', '7308', '1', '1', '0', '249.29', '1336.19', '15.6732', '3.98388', '120', '0', '0'),
('343426', '54638', '1116', '6722', '7308', '1', '1', '0', '341.573', '1304.86', '6.90336', '2.94125', '120', '0', '0'),
('343425', '54638', '1116', '6722', '7308', '1', '1', '0', '340.18', '1300.53', '6.136', '2.9', '120', '0', '0'),
('343419', '54638', '1116', '6722', '7308', '1', '1', '0', '212.285', '1299.39', '16.7904', '0', '120', '0', '0'),
('343424', '54638', '1116', '6722', '7308', '1', '1', '0', '309.938', '1341.7', '17.3943', '3.40085', '120', '0', '0'),
('554638', '54638', '1116', '6722', '7308', '1', '1', '0', '185.153', '1346.47', '1.11442', '6.25355', '300', '0', '0'),
('546382', '54638', '1116', '6722', '7302', '1', '1', '0', '421.946', '1529.66', '142.327', '6.14152', '300', '0', '0'),
('546381', '54638', '1116', '6722', '7302', '1', '1', '0', '429.894', '1528.55', '141.616', '3.64787', '300', '0', '0'),
('546385', '54638', '1116', '6722', '7302', '1', '1', '0', '492.714', '1305.91', '99.5957', '0.253363', '300', '0', '0'),
('546386', '54638', '1116', '6722', '7302', '1', '1', '0', '328.611', '1432.8', '54.0413', '0.253363', '300', '0', '0'),
('546380', '54638', '1116', '6722', '7302', '1', '1', '0', '435.641', '1533.84', '142.51', '3.64787', '300', '0', '0');

DELETE FROM `creature_addon` WHERE (`linked_id`='5247DDB1D91C05C8F7F756A26F0CDCA6F05EAEB4');
DELETE FROM `creature_addon` WHERE (`linked_id`='1F6239A20EBD09661FF47FD65A9058120FC98D30');
DELETE FROM `creature_addon` WHERE (`linked_id`='A44C5B6BE44EAC2DF49720D42FA90CEA7896E931');
DELETE FROM `creature_addon` WHERE (`linked_id`='7FF67643A24B2E373DDC6199C13CD0F0492A3047');
DELETE FROM `creature_addon` WHERE (`linked_id`='E055E1278BCA0EF0DEAEFF14F5702FC9DD4DAC30');
DELETE FROM `creature_addon` WHERE (`linked_id`='9D736D3393C3CC4CE105DA4ECBC92ECE0D609DD9');
DELETE FROM `creature_addon` WHERE (`linked_id`='010D7A5BF0C099F4693DF545BF8EC27DF22BBDBB');
DELETE FROM `creature_addon` WHERE (`linked_id`='7E6A026B7942CB1AA7A6F53ABB48FD6EFBFF43A5');
insert INTO `creature_addon` (`linked_id`, `bytes1`, `auras`) VALUES 

('5247DDB1D91C05C8F7F756A26F0CDCA6F05EAEB4', '50331648', ''), -- 342964
('1F6239A20EBD09661FF47FD65A9058120FC98D30', '50331648', ''), -- 342963
('A44C5B6BE44EAC2DF49720D42FA90CEA7896E931', '50331648', ''), -- 837400
('7FF67643A24B2E373DDC6199C13CD0F0492A3047', '', '159474'), 
('E055E1278BCA0EF0DEAEFF14F5702FC9DD4DAC30', '', '159474'), 
('010D7A5BF0C099F4693DF545BF8EC27DF22BBDBB', '', '159474'), 
('9D736D3393C3CC4CE105DA4ECBC92ECE0D609DD9', '', '159474'), 
('7E6A026B7942CB1AA7A6F53ABB48FD6EFBFF43A5', '', '159474'); 

delete from conditions where SourceTypeOrReferenceId=13 and SourceEntry=167607;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
('13', '1', '167607', '0', '0', '31', '0', '3', '83740', '0', '0', '0', '', '');

-- https://forum.wowcircle.net/showthread.php?t=806486
-- https://ru.wowhead.com/npc=68   меню

UPDATE `gossip_menu_option` SET `action_poi_id`='539' WHERE (`menu_id`='421') AND (`id`='0');
UPDATE `gossip_menu_option` SET `action_poi_id`='540' WHERE (`menu_id`='421') AND (`id`='1');
UPDATE `gossip_menu_option` SET `action_poi_id`='541' WHERE (`menu_id`='421') AND (`id`='2');
UPDATE `gossip_menu_option` SET `action_poi_id`='542' WHERE (`menu_id`='421') AND (`id`='3');
UPDATE `gossip_menu_option` SET `action_menu_id`='444', `action_poi_id`='543' WHERE (`menu_id`='421') AND (`id`='4');
UPDATE `gossip_menu_option` SET `action_poi_id`='544' WHERE (`menu_id`='421') AND (`id`='5');
UPDATE `gossip_menu_option` SET `action_poi_id`='545' WHERE (`menu_id`='421') AND (`id`='6');
UPDATE `gossip_menu_option` SET `action_poi_id`='546' WHERE (`menu_id`='421') AND (`id`='7');
UPDATE `gossip_menu_option` SET `action_poi_id`='539' WHERE (`menu_id`='421') AND (`id`='9');
UPDATE `gossip_menu_option` SET `action_poi_id`='547' WHERE (`menu_id`='421') AND (`id`='11');
UPDATE `gossip_menu_option` SET `action_poi_id`='548' WHERE (`menu_id`='421') AND (`id`='15');
UPDATE `gossip_menu_option` SET `action_poi_id`='548' WHERE (`menu_id`='421') AND (`id`='12');
UPDATE `gossip_menu_option` SET `action_poi_id`='549' WHERE (`menu_id`='421') AND (`id`='13');
UPDATE `gossip_menu_option` SET `action_poi_id`='551' WHERE (`menu_id`='421') AND (`id`='16');
UPDATE `gossip_menu_option` SET `action_poi_id`='550' WHERE (`menu_id`='421') AND (`id`='14');
UPDATE `gossip_menu_option` SET `action_poi_id`='13' WHERE (`menu_id`='421') AND (`id`='10');
UPDATE `gossip_menu_option` SET `action_poi_id`='550' WHERE (`menu_id`='421') AND (`id`='8');
UPDATE `gossip_menu_option` SET `action_poi_id`='604' WHERE (`menu_id`='435') AND (`id`='0');
DELETE FROM `gossip_menu_option` WHERE (`menu_id`='435') AND (`id`='1');
UPDATE `gossip_menu_option` SET `action_poi_id`='552' WHERE (`menu_id`='435') AND (`id`='3');
UPDATE `gossip_menu_option` SET `action_poi_id`='18' WHERE (`menu_id`='11839') AND (`id`='0');
UPDATE `gossip_menu_option` SET `action_poi_id`='555' WHERE (`menu_id`='11839') AND (`id`='1');
UPDATE `gossip_menu` SET `text_id`='16601' WHERE (`entry`='11839') AND (`text_id`='11839'); 
UPDATE `gossip_menu_option` SET `action_menu_id`='17205', `action_poi_id`='552' WHERE (`menu_id`='435') AND (`id`='2');
UPDATE `npc_text` SET `BroadcastTextID0`='53079' WHERE (`ID`='17205');
UPDATE `gossip_menu_option` SET `action_poi_id`='19' WHERE (`menu_id`='11841') AND (`id`='0');
UPDATE `gossip_menu_option` SET `action_poi_id`='556' WHERE (`menu_id`='11841') AND (`id`='1');
UPDATE `gossip_menu_option` SET `action_poi_id`='30' WHERE (`menu_id`='435') AND (`id`='6');
UPDATE `gossip_menu_option` SET `action_poi_id`='457' WHERE (`menu_id`='401') AND (`id`='0');
UPDATE `gossip_menu_option` SET `action_poi_id`='570' WHERE (`menu_id`='11855') AND (`id`='0');
UPDATE `gossip_menu_option` SET `action_poi_id`='538' WHERE (`menu_id`='11855') AND (`id`='1');
UPDATE `gossip_menu_option` SET `action_poi_id`='533' WHERE (`menu_id`='401') AND (`id`='2');
UPDATE `gossip_menu_option` SET `action_poi_id`='534' WHERE (`menu_id`='401') AND (`id`='3');
UPDATE `gossip_menu_option` SET `action_poi_id`='534' WHERE (`menu_id`='401') AND (`id`='4');
UPDATE `gossip_menu_option` SET `action_poi_id`='535' WHERE (`menu_id`='401') AND (`id`='5');
UPDATE `gossip_menu_option` SET `action_poi_id`='536' WHERE (`menu_id`='401') AND (`id`='6');
UPDATE `gossip_menu_option` SET `action_poi_id`='537' WHERE (`menu_id`='401') AND (`id`='7');
UPDATE `gossip_menu_option` SET `action_poi_id`='538' WHERE (`menu_id`='401') AND (`id`='8');
UPDATE `gossip_menu_option` SET `action_poi_id`='603' WHERE (`menu_id`='401') AND (`id`='9');
UPDATE `gossip_menu_option` SET `action_poi_id`='23' WHERE (`menu_id`='435') AND (`id`='8');
UPDATE `gossip_menu_option` SET `action_poi_id`='553' WHERE (`menu_id`='435') AND (`id`='9');
UPDATE `gossip_menu_option` SET `action_poi_id`='557' WHERE (`menu_id`='11843') AND (`id`='0');
UPDATE `gossip_menu_option` SET `action_poi_id`='558' WHERE (`menu_id`='11843') AND (`id`='1');
UPDATE `gossip_menu_option` SET `action_poi_id`='559' WHERE (`menu_id`='11845') AND (`id`='0');
UPDATE `gossip_menu_option` SET `action_poi_id`='560' WHERE (`menu_id`='11845') AND (`id`='1');
UPDATE `gossip_menu_option` SET `action_poi_id`='561', `OptionBroadcastTextID`='44635' WHERE (`menu_id`='11845') AND (`id`='2');
UPDATE `npc_text` SET `BroadcastTextID0`='44640' WHERE (`ID`='7229');
UPDATE `gossip_menu_option` SET `action_poi_id`='562' WHERE (`menu_id`='11845') AND (`id`='3');
UPDATE `gossip_menu_option` SET `action_poi_id`='20' WHERE (`menu_id`='11845') AND (`id`='4');
UPDATE `gossip_menu_option` SET `action_poi_id`='563' WHERE (`menu_id`='11845') AND (`id`='5');
UPDATE `gossip_menu_option` SET `action_poi_id`='554' WHERE (`menu_id`='435') AND (`id`='13');
UPDATE `gossip_menu_option` SET `action_poi_id`='564' WHERE (`menu_id`='11846') AND (`id`='0');
UPDATE `gossip_menu_option` SET `action_poi_id`='565' WHERE (`menu_id`='11846') AND (`id`='1');
UPDATE `npc_text` SET `BroadcastTextID0`='44651', `Probability0`='60' WHERE (`ID`='11848');
UPDATE `gossip_menu_option` SET `action_poi_id`='566' WHERE (`menu_id`='11848') AND (`id`='1');
UPDATE `gossip_menu_option` SET `action_poi_id`='567' WHERE (`menu_id`='11848') AND (`id`='2');
UPDATE `gossip_menu_option` SET `action_poi_id`='568' WHERE (`menu_id`='11848') AND (`id`='3');
UPDATE `gossip_menu_option` SET `action_poi_id`='569' WHERE (`menu_id`='11848') AND (`id`='4');

delete from gossip_menu where entry=14951;
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
('14951', '1000');

-- https://forum.wowcircle.net/showthread.php?t=806744

UPDATE `creature_template_addon` SET `auras`='69196' WHERE (`entry`='42635');

-- https://forum.wowcircle.net/showthread.php?t=1044171
-- https://ru.wowhead.com/quest=31735
-- https://ru.wowhead.com/npc=66435

UPDATE `creature_template` SET `faction`='35', `unit_flags`='33587968', `unit_flags2`='71305216' WHERE (`entry`='66435');
UPDATE `creature_template` SET `unit_flags`='33554432',`AIName`='SmartAI', `flags_extra`='130' WHERE (`entry`='66383');
UPDATE `creature_template` SET `unit_flags`='33554432',`AIName`='SmartAI', `flags_extra`='130' WHERE (`entry`='66384');
UPDATE `creature_template` SET `faction`='83', `unit_flags`='898', `unit_flags2`='2048', `AIName`='SmartAI' WHERE (`entry`='66396');
UPDATE `creature_template_wdb` SET `KillCredit1`='0', `KillCredit2`='0' WHERE (`Entry`='66396');

delete from smart_scripts where source_type=0 and entryorguid in (66383,66384,66396);
delete from smart_scripts where source_type=9 and entryorguid in (66396*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(66383, '', 0, 0, 1, 8, 0, 100, 0, 0, 130546, 0, 0, 0, 0, 11, 130556, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(66383, '', 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 66383, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(66384, '', 0, 0, 1, 8, 0, 100, 0, 0, 130546, 0, 0, 0, 0, 11, 130556, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(66384, '', 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 66384, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(66396, '', 0, 1, 0, 10, 0, 100, 0, 0, 1, 15, 20000, 20000, 0, 80, 66396*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(66396*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 1500, 1500, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(66396*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(66396*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 130625, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(66396*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 33, 66276, 0, 0, 0, 0, 0, 18, 30, 0, 0, 0, 0, 0, 0, ""),
(66396*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");


delete from creature where id in (66383,66384,66396);
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`,`phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `MovementType`) VALUES 
('66383', '870', '5785', '5853', '1', '0','2000', '-774.729', '-1746.64', '13.4896', '1.57086', '300', '0', '0'),
('66384', '870', '5785', '5853', '1', '0','2001', '-819.686', '-1760.88', '12.0988', '6.25182', '300', '0', '0'),
('66396', '870', '5785', '5853', '1', '0','2002', '-833.278', '-1798.09', '4.66053', '1.60541', '30', '0', '0');

delete from gameobject where id in (215390,215389,215388);
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES 
('215390', '870', '5785', '5853', '1', '0', '2000', '-775.53', '-1745.11', '10.073', '1.52838', '0', '0', '0.691954', '0.721942', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2000', '-775.901', '-1746.49', '10.1542', '1.52838', '0', '0', '0.691954', '0.721942', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2000', '-774.909', '-1745.99', '11.5196', '1.52838', '0', '0', '0.691954', '0.721942', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2000', '-773.389', '-1745.69', '10.3401', '1.52838', '0', '0', '0.691954', '0.721942', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2000', '-774.796', '-1746', '10.2243', '1.52838', '0', '0', '0.691954', '0.721942', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2001', '-818.667', '-1760.69', '9.36661', '0.0808978', '0', '0', '0.0404379', '0.999182', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2000', '-773.767', '-1744.37', '10.2508', '1.65169', '0', '0', '0.735123', '0.677934', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2000', '-773.931', '-1745.48', '11.6355', '1.65169', '0', '0', '0.735123', '0.677934', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2000', '-774.298', '-1745.66', '12.9307', '1.65169', '0', '0', '0.735123', '0.677934', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2001', '-818.225', '-1761.1', '8.07121', '6.12846', '0', '0', '0.0772836', '-0.997009', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2001', '-819.453', '-1761.32', '7.97404', '0.150006', '0', '0', '0.0749325', '0.997189', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2001', '-819.093', '-1760.07', '7.95831', '0.150006', '0', '0', '0.0749325', '0.997189', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2001', '-820.655', '-1761.27', '7.92164', '0.150006', '0', '0', '0.0749325', '0.997189', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2001', '-820.447', '-1760', '7.89822', '0.150006', '0', '0', '0.0749325', '0.997189', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2001', '-819.964', '-1761.25', '9.26953', '0.150006', '0', '0', '0.0749325', '0.997189', '300', '0', '1'),
('215390', '870', '5785', '5853', '1', '0', '2001', '-819.12', '-1760.79', '10.6621', '0.150006', '0', '0', '0.0749325', '0.997189', '300', '0', '1'),
('215389', '870', '5785', '5853', '1', '0', '2000', '-782.292', '-1739.84', '11.2762', '1.67138', '-0.652159', '-0.711868', '-0.191526', '0.176776', '120', '255', '1'),
('215389', '870', '5785', '5853', '1', '0', '2001', '-815.055', '-1753.16', '10.4203', '0.0418854', '0.955334', '0.0125904', '0.294717', '0.0179157', '120', '255', '1'),
('215389', '870', '5785', '5853', '1', '0', '2001', '-815.325', '-1760.49', '10.7069', '0.12494', '0.954627', '0.0515108', '0.293192', '0.00850647', '120', '255', '1'),
('215389', '870', '5785', '5853', '1', '0', '2000', '-774.757', '-1740.27', '10.9369', '1.70549', '-0.640118', '-0.714569', '-0.214586', '0.183284', '120', '255', '1'),
('215388', '870', '5785', '5853', '1', '0', '2000', '-786.305', '-1736.47', '10.736', '1.60326', '0.571032', '0.426531', '0.577449', '0.39818', '120', '255', '1'),
('215388', '870', '5785', '5853', '1', '0', '2001', '-811.632', '-1749.19', '10.736', '0.0206252', '0.704766', '-0.10635', '0.122677', '0.690612', '120', '255', '1'),
('215388', '870', '5785', '5853', '1', '0', '2001', '-812.255', '-1756.27', '10.736', '0.0798762', '0.707607', '-0.0854273', '0.14308', '0.686675', '120', '255', '1'),
('215388', '870', '5785', '5853', '1', '0', '2000', '-779.203', '-1736.97', '10.736', '1.66251', '0.558146', '0.443258', '0.58899', '0.3809', '120', '255', '1');

delete from phase_definitions where zoneId=5785 and entry in (10,11,12);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
('5785', '10', '0', '2000', '0', '0', '0', 'q31735'),
('5785', '11', '0', '2001', '0', '0', '0', 'q31735'),
('5785', '12', '0', '2002', '0', '0', '0', 'q31735');

delete from conditions where SourceTypeOrReferenceId=26 and SourceGroup=5785 and SourceEntry in (10,11,12);
delete from conditions where SourceTypeOrReferenceId=22 and SourceEntry=66396 and ConditionTypeOrReference in (9,48);
delete from conditions where SourceTypeOrReferenceId=13 and SourceEntry=130546;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('22', '1', '66396', '0', '0', '9', '0', '31735', '0', '0', '0', '0', '', ''),
('22', '1', '66396', '0', '0', '48', '0', '269131', '0', '0', '1', '0', '', ''),

('13', '1', '130546', '0', '0', '31', '0', '3', '66383', '0', '0', '0', '', ''),
('13', '1', '130546', '0', '1', '31', '0', '3', '66384', '0', '0', '0', '', ''),

('26', '5785', '10', '0', '0', '48', '0', '269129', '0', '0', '1', '0', '', 'q31735'),
('26', '5785', '10', '0', '0', '9', '0', '31735', '0', '0', '0', '0', '', 'q31735'),
('26', '5785', '11', '0', '0', '48', '0', '269130', '0', '0', '1', '0', '', 'q31735'),
('26', '5785', '11', '0', '0', '9', '0', '31735', '0', '0', '0', '0', '', 'q31735'),
('26', '5785', '12', '0', '0', '9', '0', '31735', '0', '0', '0', '0', '', 'q31735');

delete from creature_template_addon where entry=66396;
INSERT INTO `creature_template_addon` (`entry`,`auras`) VALUES
('66396', '130624');

delete from creature_text where Entry=66396;
INSERT INTO `creature_text` (`Entry`, `Text`, `Type`, `Probability`, `Sound`, `BroadcastTextID`) VALUES 
('66396', 'By our sweat and your blood, this land WILL be ours!', '12', '100', '32473', '67592');

-- https://forum.wowcircle.net/showthread.php?t=1061904
-- https://ru.wowhead.com/quest=42859

UPDATE `creature_template` SET `gossip_menu_id`='19929' WHERE (`entry`='109409');
UPDATE `creature_template` SET `gossip_menu_id`='19933', `minlevel`='110', `maxlevel`='110', `unit_flags`='33536', `unit_flags2`='2048', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='110', `ScaleLevelMax`='110', `ScaleLevelDelta`='0' WHERE (`entry`='109442');

DELETE FROM `gossip_menu` WHERE (`entry`='1094090');
DELETE FROM `gossip_menu` WHERE (`entry`='1094420');
DELETE FROM `gossip_menu` WHERE (`entry`='19933');
DELETE FROM `npc_text` WHERE (`ID`='1094090');
DELETE FROM `npc_text` WHERE (`ID`='1094420');
DELETE FROM `gossip_menu_option` WHERE `menu_id`=1094090;
DELETE FROM `gossip_menu_option` WHERE `menu_id`=1094420;
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
('19933', '29623');

DELETE FROM `gossip_menu_option` WHERE `menu_id`=19929;
DELETE FROM `gossip_menu_option` WHERE `menu_id`=19933;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`) VALUES 
('19933', '0', '0', 'Я раздам это чародейское вино тем, кто в нем нуждается.', '1', '1', '0', '0', '0', '0', NULL, '115399', '0'),
('19929', '0', '0', 'I\'ve brought you some arcwine... drink up!', '1', '1', '0', '0', '0', '0', NULL, '115294', '0'),
('19929', '1', '0', '<Feed the Nightborne Wretch 5 |TINTERFACE\\ICONS\\INV_MISC_ANCIENT_MANA.BLP:15|t Ancient Mana.>', '0', '0', '0', '0', '0', '0', 'This action costs 5', '122365', '123413');

delete from smart_scripts where source_type=0 and entryorguid in (109409,109442);
delete from smart_scripts where source_type=9 and entryorguid in (1094091,1094092,1094090,1094093,109442);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(109409, '', 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 0, 0, 87, 1094091, 1094092, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(1094091, '', 9, 0, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 17, 403, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(1094092, '', 9, 0, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 17, 404, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(109409, '', 0, 1, 0, 62, 0, 100, 0, 0, 19929, 0, 0, 0, 0, 80, 1094090, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(1094090, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1094090, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(1094090, '', 9, 2, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 5, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(1094090, '', 9, 3, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 33, 109409, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1094090, '', 9, 4, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1094090, '', 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1094090, '', 9, 6, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 81, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(1094090, '', 9, 7, 0, 0, 0, 100, 0, 0, 300, 300, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(109442, '', 0, 0, 1, 62, 0, 100, 0, 0, 19933, 0, 0, 0, 0, 33, 109443, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(109442, '', 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(109442, '', 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "");


DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='15') AND (`SourceGroup`='1094090');
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='15') AND (`SourceGroup`='19929');
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='15') AND (`SourceGroup`='19933');
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('15', '19933', '0', '0', '0', '9', '0', '42859', '0', '0', '0', '0', '', ''),
('15', '19933', '0', '0', '0', '48', '0', '284954', '0', '0', '1', '0', '', ''),

('15', '19929', '0', '0', '0', '9', '0', '42859', '0', '0', '0', '0', '', ''),
('15', '19929', '1', '0', '0', '9', '0', '42859', '0', '0', '1', '0', '', '');

UPDATE `creature_text` SET `Emote`='1', `BroadcastTextID`='115295' WHERE (`Entry`='109409') AND (`GroupID`='0') AND (`ID`='0');
UPDATE `creature_text` SET `Emote`='1', `BroadcastTextID`='115296' WHERE (`Entry`='109409') AND (`GroupID`='0') AND (`ID`='1');
UPDATE `creature_text` SET `BroadcastTextID`='115370' WHERE (`Entry`='109442') AND (`GroupID`='0') AND (`ID`='0');

-- https://forum.wowcircle.net/showthread.php?t=1061899
-- https://ru.wowhead.com/quest=42828

delete from creature where id in (109158);
INSERT INTO `creature` (`guid`, `linked_id`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
('18392363', 'A91CA8A4387260B6E1E62FC6641B47994C2115EB', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1743.76', '4609.98', '102.524', '3.97884', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18392362', '84029E3FCB18754223802C3D1E2E7435B5FCCB88', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1733.33', '4624.14', '119.611', '5.2039', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18392358', '9902A5B08D773D0D35D18BF0BC51D5EB7DB6FD57', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1709.56', '4601.88', '106.22', '1.64477', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18384108', '72D822C07C6A002ECB779E4D9E9D8801D049E121', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1719.75', '4632.15', '126.84', '5.76305', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18384100', 'B202C203A94B0EAC03BB6C82D21EDB110D5F0661', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1736.33', '4631.14', '126.865', '0.992956', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18384099', 'BE0832815FE1BB2382E102BE43756A128E58932F', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1722.25', '4616.31', '126.411', '2.19464', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18384097', 'D5507BC2AABEC1CC1F0881689E7C974EDCB7EF0C', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1734.44', '4602.49', '133.576', '3.67697', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18384096', '7108D1F60DE43ACE6D54050C17161EFBCA589DB4', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1731.92', '4603.12', '126.374', '3.98171', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18384095', '8C915E73C76C7D8404229DD0DD2C287F1BF2ACCC', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1726.96', '4584.15', '126.143', '0', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18384094', '8668285484D1DF03CAC6922E97719409FFAE3AB7', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1739.26', '4593.47', '126.539', '4.3001', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18384093', '6D37551C165ED5DFA22736FD0512B1153B28B695', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1748.81', '4604.31', '126.121', '0.957264', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18380335', '9931F99B425E2497E53FD583DBB49142D15A9B6C', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1708.76', '4599.32', '128.117', '4.04428', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18383836', '2D9185A576FFACD348D1AFAF9962353D5FCD89BA', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1712.09', '4634.02', '133.226', '5.2381', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18378127', 'DBB0811EABEEDD7AB3DC16D78575BB4B43CED255', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1707.98', '4615.9', '97.7193', '2.21415', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18392405', '313B9A395F45CA9817512C5F0C0111460635678D', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1724.19', '4602.38', '128.368', '1.475', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18392404', '62816718C8D545CEDE29E4FCE0AEBB42482C5C8F', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1708.38', '4615.38', '126.012', '0', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18392402', 'D404F9BC561807BF66FF0291AF3EF6F17AB45371', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1743.82', '4610.2', '125.48', '0', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18392401', 'B46D0E76F901C8B0DF1C560FA3CD91ABE114877A', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1733.72', '4624.21', '126.325', '0.0289425', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18392288', 'D38CDE5222E2CEB2E61D5F6AD0DABEDB9A52D5DF', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1724.69', '4618.68', '134.728', '0.864514', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18380315', '653F940ACF77A909440BE7317C34C1D123CA3DFD', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1705.89', '4641.65', '133.199', '5.15865', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18380314', '96915C674147E7EAFF296233F0A06411D3B47745', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1705.69', '4621.29', '129.777', '4.9245', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18383556', '4A73772604ECCBB183B4A469776D30ABB6C1931D', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1721.15', '4610', '133.572', '1.56753', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18383554', '84345E9013D49524DF11BABDE4515A8999E6171E', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1726.69', '4613.16', '129.24', '6.26889', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18378616', 'D3E4E29136232690F8B455CD50E86E4248A53C8F', '109158', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1736.39', '4600.73', '96.4066', '2.88092', '120', '5', '0', '1', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1');

delete from smart_scripts where source_type=0 and entryorguid in (109158);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(109158, '', 0, 0, 0, 8, 0, 100, 0, 0, 217074, 0, 10000, 10000, 0, 41, 500, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

UPDATE `creature_template` SET `exp`='0', `faction`='2575', `npcflag`='16777216', `speed_fly`='0.6',`unit_flags`='33024', `unit_flags2`='2048', `AIName`='SmartAI', `InhabitType`='4',`HoverHeight`='1', `RegenHealth`='1', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='109158');

delete from npc_spellclick_spells where npc_entry=109158;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
('109158', '217074', '1', '0');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='18') AND (`SourceGroup`='109158');
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
('18', '109158', '217074', '0', '0', '9', '0', '42828', '0', '0', '0', '0', '', '');

UPDATE `quest_template` SET `PrevQuestId`='42488' WHERE (`Id`='42828');

-- https://ru.wowhead.com/npc=109411

UPDATE `creature_template` SET `faction`='2200', `speed_fly`='0.7', `unit_flags`='33040', `unit_flags2`='2048', `AIName`='', `InhabitType`='4',`HoverHeight`='1', `RegenHealth`='1', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='109411');

delete from creature where id in (109411);
delete from creature where id in (115677) and zoneId=7637;
INSERT INTO `creature` (`guid`, `linked_id`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 

('18380337', '5F922DD1D618F287205BA0E6752BCC21A0382722', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1728.2', '4612.43', '132.093', '4.03414', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18383837', '9964BBDFFDFAA0BF619605E5C646633E7CB4E0D0', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1727.73', '4622.08', '125.249', '0.398779', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18379545', '77008444D47289FF5A40FFA64C775D8251B2B7A1', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1706.73', '4610.87', '124.71', '0.575988', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18379538', '66083B874309138104D276A3DDBA9BBFFB842285', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1725.73', '4601.52', '133.577', '2.70108', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18380324', '5A0188FF1A58A9C8F3BEAF04AE594ACE99BFB887', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1740.22', '4620.87', '125.489', '0.110468', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18380313', '59B8361141C9379FB97075C18E9A8A2979DB97B5', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1713.19', '4603.33', '124.71', '3.95772', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18379852', 'AC9CF167D21F9DEFFCF0FAF941934D5E9864051F', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1740.76', '4612.22', '125.483', '5.70021', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18379847', 'AB58BEE364256B6D629A71E2B98B1AA8227EC45D', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1726.12', '4609.84', '133.656', '1.5708', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18379845', '83F47B3C3584E2ED9AC69155366667F2D38EAE0D', '109411', '1220', '7637', '7928', '1', '1', '0', '0', '0', '1722.69', '4609.02', '132.649', '0.236523', '120', '7', '0', '1039267', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1');

-- https://forum.wowcircle.net/showthread.php?t=1061872

-- https://ru.wowhead.com/quest=41877
-- https://ru.wowhead.com/quest=43309
-- https://ru.wowhead.com/quest=43317

delete from creature_queststarter where quest=41877;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES 
('97140', '41877'),
('112146', '41877');

delete from spell_scene where SceneScriptPackageID=1657 and MiscValue=1327;
INSERT INTO `spell_scene` (`SceneScriptPackageID`, `MiscValue`, `PlaybackFlags`, `TrigerSpell`, `MonsterCredit`, `ScriptName`, `Comment`) VALUES
('1657', '1327', '25', '0', '0', '', 'Lady Lunastre');

UPDATE `creature_template` SET `gossip_menu_id`='19783', `npcflag`='3', `AIName`='SmartAI', `RegenHealth`='1', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='110', `ScaleLevelMax`='110' WHERE (`entry`='107632');
UPDATE `creature_template` SET `faction`='2823', `speed_walk`='1', `unit_flags`='898', `unit_flags2`='2048', `AIName`='SmartAI', `RegenHealth`='1', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='110967');

delete from gossip_menu where entry=19783;
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
('19783', '29308');

delete from gossip_menu_option where menu_id=19783;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
('19783', '0', '0', 'You wished to speak to me?', '1', '1', '112392'),
('19783', '1', '0', 'I am ready to face Coryn.', '1', '1', '119387');

UPDATE `creature` SET `modelid`='70000' WHERE (`guid`='18386269');
UPDATE `creature_template_wdb` SET `KillCredit2`='0' WHERE (`Entry`='110354');

DELETE FROM `creature_addon` WHERE (`linked_id`='C15E16DF139CD3569EE1F082C732C438958B8054');
insert INTO `creature_addon` (`linked_id`, `auras`) VALUES 
('C15E16DF139CD3569EE1F082C732C438958B8054', '209898 209901');

DELETE FROM `quest_start_scripts` WHERE (`id`='43317');
UPDATE `quest_template` SET `StartScript`='0' WHERE (`Id`='43317');
UPDATE `quest_template` SET `RewardSpellCast`='0' WHERE (`Id`='43318');

delete from smart_scripts where source_type=0 and entryorguid in (107632,110967);
delete from smart_scripts where source_type=9 and entryorguid in (107632*100,107632*100+1,107632*100+2);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(107632, '', 0, 0, 1, 62, 0, 100, 0, 0, 19783, 0, 0, 0, 0, 33, 107632, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(107632, '', 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 85, 215465, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(107632, '', 0, 2, 0, 10, 0, 100, 0, 0, 1, 35, 45000, 45000, 0, 80, 107632*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(107632*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 12, 110967, 1, 7700, 0, 0, 0, 8, 0, 0, 0, 1155.87, 4247.71, 31.4296, 2.7, ""),
(107632*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 1500, 1500, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 1500, 1500, 0, 0, 0, 5, 66, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 4500, 4500, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 110967, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 200, 200, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 19, 110967, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 12, 110967, 1, 32000, 0, 0, 0, 8, 0, 0, 0, 1140.73, 4254.635, 21.3259, 1.05, ""),
(107632*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 200, 200, 0, 0, 0, 86, 52096 , 0, 19, 110967, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 8, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 84, 1, 0, 0, 0, 0, 0, 19, 110967, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 9, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 10, 0, 0, 0, 100, 0, 0, 7000, 7000, 0, 0, 0, 84, 2, 0, 0, 0, 0, 0, 19, 110967, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 11, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 12, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 84, 3, 0, 0, 0, 0, 0, 19, 110967, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 13, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 84, 4, 0, 0, 0, 0, 0, 19, 110967, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 107632, 0, 0, 0, 0, 0, 18, 40, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 15, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 110967, 0, 0, 0, 0, 0, 0, ""),
(107632*100, '', 9, 16, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 86, 52096 , 0, 19, 110967, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(110967, '', 0, 0, 0, 38, 0, 100, 1, 0, 1, 1, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1138.74, 4251.737, 21.271, 0, ""),

(107632, '', 0, 3, 0, 62, 0, 100, 0, 0, 19783, 1, 0, 0, 0, 80, 107632*100+1, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(107632*100+1, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 81, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+1, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+1, '', 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 5, 69, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+1, '', 9, 3, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 33, 107632, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+1, '', 9, 4, 0, 0, 0, 100, 0, 0, 500, 500, 0, 0, 0, 1, 7, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+1, '', 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 85, 52096, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+1, '', 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 5, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+1, '', 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 81, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+1, '', 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 62, 1220, 0, 0, 0, 0, 0, 7, 0, 0, 0, 1095.15, 4177.18, 179.352, 0.993356, ""),

(107632, '', 0, 4, 0, 10, 0, 100, 0, 0, 1, 8, 20000, 20000, 0, 80, 107632*100+2, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(107632*100+2, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+2, '', 9, 1, 0, 0, 0, 100, 0, 0, 6000, 6000, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 107598, 0, 0, 0, 0, 0, 0, ""),
(107632*100+2, '', 9, 2, 0, 0, 0, 100, 0, 0, 9000, 9000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(107632*100+2, '', 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 107632, 0, 0, 0, 0, 0, 18, 15, 0, 0, 0, 0, 0, 0, ""),

(107632, '', 0, 5, 0, 20, 0, 100, 0, 0, 43318, 0, 0, 0, 0, 1, 8, 5000, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(107632, '', 0, 6, 0, 52, 0, 100, 0, 0, 8, 107632, 0, 0, 0, 1, 9, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "");

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='15') AND (`SourceGroup`='19783');
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='19') AND (`SourceEntry`='43318');
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='22') AND (`SourceEntry`='107632') and SourceId=0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('15', '19783', '0', '0', '0', '9', '0', '41877', '0', '0', '0', '0', '', ''),
('15', '19783', '0', '0', '0', '23', '0', '8353', '0', '0', '0', '0', '', ''),

('15', '19783', '1', '0', '0', '9', '0', '43318', '0', '0', '0', '0', '', ''),
('15', '19783', '1', '0', '0', '23', '0', '8431', '0', '0', '0', '0', '', ''),
('19', '0', '43318', '0', '0', '23', '0', '8431', '0', '0', '0', '0', '', ''),

('22', '3', '107632', '0', '0', '9', '0', '43317', '0', '0', '0', '0', '', ''),
('22', '3', '107632', '0', '0', '23', '0', '8431', '0', '0', '0', '0', '', ''),

('22', '5', '107632', '0', '0', '9', '0', '43309', '0', '0', '0', '0', '', ''),
('22', '5', '107632', '0', '0', '23', '0', '8382', '0', '0', '0', '0', '', '');

delete from creature_text where Entry in (107632,110967,107598);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(107632, 0, 0, "There is a reason we live on the opposite side of the city, my friend.", 12, 0, 100, 274, 0, 67567, 116281, ""),
(107632, 1, 0, "I could change much for our people... yes...", 12, 0, 100, 1, 0, 67568, 116282, ""),
(107632, 2, 0, "Meet me within the Sanctum of Order. I must prepare for the ceremony.", 12, 0, 100, 1, 0, 68065, 119291, ""),
(107632, 3, 0, "I humbly accept this appointment. It is an honor beyond words.", 12, 0, 100, 0, 0, 66267, 116515, ""),
(107632, 4, 0, "My lord, you have lost your sense of decorum. It is already decided.", 12, 0, 100, 1, 0, 66265, 116519, ""),
(107632, 5, 0, "Tal'ashar it is. I name my friend here as my champion.", 12, 0, 100, 25, 0, 68147, 119381, ""),
(107632, 6, 0, "I am modifying your disguise to allow you to fight like one of us.", 15, 0, 100, 0, 0, 68150, 119388, ""),
(107632, 7, 0, "Win me a title, won't you?", 12, 0, 100, 1, 0, 68151, 119389, ""),
(107632, 8, 0, "WI wish this had not been necessary, but you have done well.", 12, 0, 100, 0, 0, 68152, 119395, ""),
(107632, 9, 0, "I will send you back to the tavern when you are ready.", 12, 0, 100, 0, 0, 68153, 119396, ""),

(110967, 0, 0, "Filthy backstabbing cretin! That title should have been mine!", 12, 0, 100, 603, 0, 66215, 116517, ""),
(110967, 1, 0, "You impinge upon my honor! You besmirch my name with baseless accusations!", 12, 0, 100, 603, 0, 66214, 116518, ""),
(110967, 2, 0, "I will reclaim my honor! I invoke the ancient rite- Tal'ashar!", 12, 0, 100, 603, 0, 66219, 116524, ""),
(110967, 3, 0, "I have no need to hide behind a champion. I will defend my own honor.", 12, 0, 100, 1, 0, 68148, 119382, ""),
(110967, 4, 0, "Meet me atop the Sanctum when you are ready to die, whelp.", 12, 0, 100, 603, 0, 68149, 119383, ""),

(107598, 0, 0, "Of course. I agree that this whole business is... distasteful. Even still, the sort of influence you would have...", 12, 0, 100, 1, 0, 67586, 116280, "");

DELETE FROM `creature` WHERE (`id`='110967');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1500' WHERE (`guid`='18392408');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1502' WHERE (`guid`='18392501');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1502' WHERE (`guid`='18392502');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1501' WHERE (`guid`='18392508');
UPDATE `creature` SET `phaseMask`='0', `phaseID`='1501' WHERE (`id`='110354');

delete from phase_definitions where zoneId=7637 and entry in (10,11,12);
INSERT INTO `phase_definitions` (`zoneId`, `entry`, `phasemask`, `phaseId`, `terrainswapmap`, `uiworldmapareaswap`, `flags`, `comment`) VALUES 
('7637', '10', '0', '1500', '0', '0', '0', 'q43317'),
('7637', '11', '0', '1501', '0', '0', '0', 'q43318'),
('7637', '12', '0', '1502', '0', '0', '0', 'q43309');  

delete from conditions where SourceTypeOrReferenceId=26 and SourceGroup=7637 and SourceEntry in (10,11,12);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('26', '7637', '10', '0', '0', '47', '0', '43317', '74', '0', '0', '0', '', 'q43317'),
('26', '7637', '11', '0', '0', '47', '0', '43318', '10', '0', '0', '0', '', 'q43317'),
('26', '7637', '12', '0', '0', '47', '0', '43309', '74', '0', '0', '0', '', 'q43309');

-- https://ru.wowhead.com/quest=43313

UPDATE `creature_template` SET `gossip_menu_id`='20112', `faction`='2855', `unit_flags`='33024', `unit_flags2`='2048', `RegenHealth`='1', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='110875');
UPDATE `creature_template` SET `gossip_menu_id`='20110', `faction`='2855', `unit_flags`='33024', `unit_flags2`='2048', `RegenHealth`='1', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='110876');
UPDATE `creature_template` SET `gossip_menu_id`='20111', `faction`='2855', `unit_flags`='33024', `unit_flags2`='34816', `RegenHealth`='1', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='110874');

delete from gossip_menu where entry in (20112,20110,20111);
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
('20112', '29892'),
('20110', '29890'),
('20111', '29891');

delete from gossip_menu_option where menu_id in (20112,20110,20111);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
('20112', '0', '0', "I overheard a rather crude joke at poor Ruven's expense while visiting the Stelleris family. I can't imagine you would want to hear it...", '1', '1', '119174'),
('20110', '0', '0', "Я слышала, что Корин Стеллерис начал переманивать к себе оставшихся слуг дома Весперос. Это правда?", '1', '1', '119173'),
('20111', '0', '0', "На улицах болтают, что возле его трупа нашли серебряный полумесяц. Это правда?", '1', '1', '119175');

replace INTO `creature_template_addon` (`entry`, `auras`) VALUES ('110876', '209887');
replace INTO `creature_template_addon` (`entry`, `bytes1`) VALUES ('110874', '1');

delete from smart_scripts where source_type=9 and entryorguid in (110875*100,110876*100,110874*100);
delete from smart_scripts where source_type=0 and entryorguid in (110875,110876,110874);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(110875, '', 0, 0, 0, 62, 0, 100, 0, 0, 20112, 0, 0, 0, 0, 80, 110875*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(110875*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 110875, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110875*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 110880, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110875*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110875*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 200, 200, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110875*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(110876, '', 0, 0, 0, 62, 0, 100, 0, 0, 20110, 0, 0, 0, 0, 80, 110876*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(110876*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 110876, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110876*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 110880, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110876*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110876*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 200, 200, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110876*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),

(110874, '', 0, 0, 0, 62, 0, 100, 0, 0, 20111, 0, 0, 0, 0, 80, 110874*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(110874, '', 0, 1, 0, 1, 0, 80, 0, 0, 1000, 2000, 10000, 15000, 0, 5, 18, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(110874*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 110874, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110874*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 110880, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110874*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110874*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(110874*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "");

delete from creature_text where Entry in (110875,110876,110874);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(110875, 0, 0, "Preposterous! A joke at my lord's expense? Only a madman would attempt it!", 12, 0, 100, 5, 0, 0, 119181, ""),
(110875, 1, 0, "... But if anyone would laugh, it would be THEM.", 12, 0, 100, 1, 0, 0, 119182, ""),

(110876, 0, 0, "Absolutely NOT! Why would he do such a thing? Has he no honor?!", 12, 0, 100, 5, 0, 0, 119177, ""),
(110876, 1, 0, "... Why WOULD he do such a thing...?", 12, 0, 100, 1, 0, 0, 119178, ""),

(110874, 0, 0, "That is the symbol of House Stelleris... but that could not be. Coryn was Ruven's dearest friend...", 12, 0, 100, 1, 0, 0, 119179, ""),
(110874, 1, 0, "No, that's simply impossible...", 12, 0, 100, 274, 0, 0, 119180, "");

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='15') AND (`SourceGroup`='20112');
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='15') AND (`SourceGroup`='20110');
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`='15') AND (`SourceGroup`='20111');
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('15', '20112', '0', '0', '0', '9', '0', '43313', '0', '0', '0', '0', '', ''),
('15', '20112', '0', '0', '0', '48', '0', '285460', '0', '0', '1', '0', '', ''),

('15', '20110', '0', '0', '0', '9', '0', '43313', '0', '0', '0', '0', '', ''),
('15', '20110', '0', '0', '0', '48', '0', '285462', '0', '0', '1', '0', '', ''),

('15', '20111', '0', '0', '0', '9', '0', '43313', '0', '0', '0', '0', '', ''),
('15', '20111', '0', '0', '0', '48', '0', '285461', '0', '0', '1', '0', '', '');


-- https://ru.wowhead.com/quest=41148

UPDATE `quest_template` SET `PrevQuestId`='40746' WHERE (`Id`='41148');

UPDATE `creature` SET `spawnMask`='0' WHERE (`id`='108385'); -- спрятать  
UPDATE `creature` SET `spawnMask`='0' WHERE (`id`='108388');
UPDATE `creature` SET `spawnMask`='0' WHERE (`id`='108387'); 
UPDATE `creature` SET `spawnMask`='0' WHERE (`id`='108386'); 

delete from creature where guid=18378593 and id=108807;
delete from creature where id in (108811,108812);
INSERT INTO `creature` (`guid`, `linked_id`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `npcflag2`, `unit_flags`, `unit_flags2`, `unit_flags3`, `dynamicflags`, `WorldEffectID`, `isActive`, `ScriptName`, `NameTag`, `VerifiedBuild`) VALUES 
('18378769', '3819D45D51EC06B26BBEF8A2F4D383E5B175518F', '108811', '1220', '7637', '8434', '1', '1', '0', '0', '0', '480.773', '3992.12', '3.233', '2.31567', '120', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1'),
('18378757', '7162BE3CA6063EC63982BA7B2FD62044332DB7C2', '108812', '1220', '7637', '8434', '1', '1', '0', '0', '0', '392.906', '4070.81', '1.3887', '3.91476', '120', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '', '', '-1');

UPDATE `creature_template` SET `gossip_menu_id`='19829' WHERE (`entry`='108346');
UPDATE `creature_template` SET `gossip_menu_id`='19830' WHERE (`entry`='108347');
UPDATE `creature_template` SET `gossip_menu_id`='19831' WHERE (`entry`='108345');
UPDATE `creature_template` SET `gossip_menu_id`='19832' WHERE (`entry`='108344');
UPDATE `creature_template` SET `AIName`='', `RegenHealth`='1', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='108810');
UPDATE `creature_template` SET `unit_flags`='898', `AIName`='', `RegenHealth`='1', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='108811');
UPDATE `creature_template` SET `unit_flags`='898', `AIName`='', `RegenHealth`='1', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='108812');

UPDATE `creature_template_addon` SET `emote`='420' WHERE (`entry`='108346');
UPDATE `creature_template_addon` SET `emote`='420' WHERE (`entry`='108347');
UPDATE `creature_template_addon` SET `emote`='420' WHERE (`entry`='108345');
UPDATE `creature_template_addon` SET `emote`='420' WHERE (`entry`='108344');
replace INTO `creature_template_addon` (`entry`, `bytes1`) VALUES ('108810', '8');
replace INTO `creature_template_addon` (`entry`, `bytes1`) VALUES ('108811', '8');
replace INTO `creature_template_addon` (`entry`, `bytes1`) VALUES ('108812', '8');

delete from gossip_menu where entry in (19829,19830,19831,19832); 
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
('19829', '29439'),
('19830', '29440'),
('19831', '29441'),
('19832', '29441');

delete from gossip_menu_option where menu_id in (19829,19830,19831,19832);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
('19829', '0', '0', '<Offer Mana Wine.>', '1', '1', '112433'),
('19830', '0', '0', '<Offer Mana Wine.>', '1', '1', '112433'),
('19831', '0', '0', '<Offer Mana Wine.>', '1', '1', '112433'),
('19832', '0', '0', '<Offer Mana Wine.>', '1', '1', '112433');

delete from smart_scripts where source_type=0 and entryorguid in (108347,108345,108344,108346);
delete from smart_scripts where source_type=9 and entryorguid in (108346*100,108347*100,108345*100,108344*100);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES

(108346, '', 0, 0, 0, 62, 0, 100, 0, 0, 19829, 0, 0, 0, 0, 80, 108346*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(108346*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 108808, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 108807, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 4000, 4000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 5, 407, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 5200, 5200, 0, 0, 0, 5, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 19, 108807, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 55296, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 9, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 33, 108346, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 10, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 107636, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 11, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 12, 0, 0, 0, 100, 0, 0, 15000, 15000, 0, 0, 0, 81, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108346*100, '', 9, 13, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 5, 420, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(108347, '', 0, 0, 0, 62, 0, 100, 0, 0, 19830, 0, 0, 0, 0, 80, 108347*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(108347*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 5, 407, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 5200, 5200, 0, 0, 0, 5, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 55296, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 5, 455, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 1000, 1000, 0, 0, 0, 33, 108347, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 107636, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 9, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 108810, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 10, 0, 0, 0, 100, 0, 0, 5000, 5000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 11, 0, 0, 0, 100, 0, 0, 15000, 15000, 0, 0, 0, 81, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108347*100, '', 9, 12, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 5, 420, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(108345, '', 0, 0, 1, 62, 0, 100, 0, 0, 19831, 0, 0, 0, 0, 80, 108345*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(108345*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 2500, 2500, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 5, 409, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 3, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 108811, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 3200, 3200, 0, 0, 0, 5, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 19, 108811, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 100, 100, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 55296, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 8, 0, 0, 0, 100, 0, 0, 4000, 4000, 0, 0, 0, 84, 1, 0, 0, 0, 0, 0, 19, 108811, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 9, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 5, 455, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 10, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 33, 108345, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 11, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 107636, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 12, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 13, 0, 0, 0, 100, 0, 0, 15000, 15000, 0, 0, 0, 81, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 5, 420, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108345*100, '', 9, 15, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(108344, '', 0, 0, 1, 62, 0, 100, 0, 0, 19832, 0, 0, 0, 0, 80, 108344*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),

(108344*100, '', 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 1, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 2, 0, 0, 0, 100, 0, 0, 1500, 1500, 0, 0, 0, 5, 409, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 19, 108812, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 5, 0, 0, 0, 100, 0, 0, 5200, 5200, 0, 0, 0, 5, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 6, 0, 0, 0, 100, 0, 0, 1800, 1800, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 55296, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 8, 0, 0, 0, 100, 0, 0, 3000, 3000, 0, 0, 0, 5, 455, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 9, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 33, 108344, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 10, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 33, 107636, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 11, 0, 0, 0, 100, 0, 0, 2000, 2000, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 12, 0, 0, 0, 100, 0, 0, 15000, 15000, 0, 0, 0, 81, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(108344*100, '', 9, 13, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 5, 420, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

delete from creature_text where Entry in (108808,108807,108346,108810,108345,108811,108344,108812,108347);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 

(108808, 0, 0, "Get up...", 12, 0, 100, 0, 0, 0, 113817, ""),
(108807, 0, 0, "Take a drink, darling. You will feel better.", 12, 0, 100, 0, 0, 0, 113818, ""),
(108346, 0, 0, "But you need...", 12, 0, 100, 0, 0, 0, 113820, ""),
(108807, 1, 0, "We will survive until tomorrow. Vanthir always takes care of us. You know that.", 12, 0, 100, 0, 0, 0, 113821, ""),
(108346, 1, 0, "Lunette nods and takes a drink.", 16, 0, 100, 0, 0, 0, 113822, ""),
(108346, 2, 0, "Much better. Thank you. Thank you so much.", 12, 0, 100, 0, 0, 0, 113823, ""),

(108347, 0, 0, "Nightwell be praised... I thought I was going to die.", 12, 0, 100, 0, 0, 0, 113831, ""),
(108347, 1, 0, "Ambrena shakily takes a drink.", 16, 0, 100, 0, 0, 0, 113836, ""),
(108810, 0, 0, "I will get her home. It is nice to know Vanthir still thinks of us", 12, 0, 100, 0, 0, 0, 113832, ""),
(108347, 2, 0, "It isn't far. Thank you.", 12, 0, 100, 0, 0, 0, 113834, ""),

(108345, 0, 0, "W-what?", 12, 0, 100, 0, 0, 0, 113839, ""),
(108811, 0, 0, "It is Arcwine. For you. Go on, brother.", 12, 0, 100, 0, 0, 0, 113840, ""),
(108345, 1, 0, "Clotaire tentatively sips at the wine, then takes a deep gulp.", 16, 0, 100, 0, 0, 0, 113841, ""),
(108811, 1, 0, "Hah. Not so fast! You may choke.", 12, 0, 100, 0, 0, 0, 113842, ""),
(108345, 2, 0, "Hush you... Thank Vanthir for me, won't you?", 12, 0, 100, 0, 0, 0, 113843, ""),

(108344, 0, 0, "He found more?", 12, 0, 100, 0, 0, 0, 113848, ""),
(108812, 0, 0, "Yes! Plenty for both of us today.", 12, 0, 100, 0, 0, 0, 113849, ""),
(108344, 1, 0, "Donatien takes a quick draught of the mana-wine.", 16, 0, 100, 0, 0, 0, 113851, ""),
(108344, 2, 0, "Vanthir came through again... how can he keep this up?", 12, 0, 100, 0, 0, 0, 113852, "");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=15 AND `SourceGroup` in (19829,19830,19831,19832);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('15', '19829', '0', '0', '0', '9', '0', '41148', '0', '0', '0', '0', '', ''),
('15', '19829', '0', '0', '0', '48', '0', '284334', '0', '0', '1', '0', '', ''),

('15', '19830', '0', '0', '0', '9', '0', '41148', '0', '0', '0', '0', '', ''),
('15', '19830', '0', '0', '0', '48', '0', '284337', '0', '0', '1', '0', '', ''),

('15', '19831', '0', '0', '0', '9', '0', '41148', '0', '0', '0', '0', '', ''),
('15', '19831', '0', '0', '0', '48', '0', '284335', '0', '0', '1', '0', '', ''),

('15', '19832', '0', '0', '0', '9', '0', '41148', '0', '0', '0', '0', '', ''),
('15', '19832', '0', '0', '0', '48', '0', '284336', '0', '0', '1', '0', '', '');


-- https://ru.wowhead.com/quest=41878 

UPDATE `creature_template` SET `gossip_menu_id`='19692', `flags_extra`='2', `RegenHealth`='1', `ScriptName`='' WHERE (`entry`='107225');

delete from gossip_menu where entry in (19692); 
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES 
('19692', '29185'),
('19692', '29184');

delete from gossip_menu_option where menu_id in (19692);
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `OptionBroadcastTextID`) VALUES 
('19692', '0', '0', '<Show your Sigil.>', '1', '1', '114966'),
('19692', '1', '0', 'I found a cask of wine!', '1', '1', '111355');

delete from smart_scripts where source_type=0 and entryorguid in (107225);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(107225, '', 0, 0, 1, 62, 0, 100, 0, 0, 19692, 0, 0, 0, 0, 85, 213225, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(107225, '', 0, 1, 2, 61, 0, 100, 0, 0, 19692, 0, 0, 0, 0, 33, 107225, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(107225, '', 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "");

delete from creature_text where Entry in (107225);
INSERT INTO `creature_text` (`Entry`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextID`, `comment`) VALUES 
(107225, 0, 0, "I see! You are the one I was waiting for. All I need is a full boat and I'll be on my way.", 12, 0, 100, 0, 0, 0, 111360, ""),
(107225, 1, 0, "Ah! This cask contains a large amount of arcwine. Go ahead and put it into the gondola.", 12, 0, 100, 0, 0, 0, 111356, "");

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=15 AND `SourceGroup` in (19692);
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=14 AND `SourceGroup` in (19692);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 

('15', '19692', '0', '0', '0', '9', '0', '41878', '0', '0', '0', '0', '', ''),
('15', '19692', '1', '0', '0', '9', '0', '40730', '0', '0', '0', '0', '', ''),

('14', '19692', '29185', '0', '0', '9', '0', '41878', '0', '0', '0', '0', '', ''),
('14', '19692', '29187', '0', '0', '8', '0', '41878', '0', '0', '0', '0', '', '');

-- https://forum.wowcircle.net/showthread.php?t=1061895

UPDATE `quest_template` SET `PrevQuestId`='41148' WHERE (`Id`='40947');
UPDATE `quest_template` SET `PrevQuestId`='41877' WHERE (`Id`='40746');
UPDATE `quest_template` SET `PrevQuestId`='41148' WHERE (`Id`='41878');
UPDATE `quest_template` SET `PrevQuestId`='42839' WHERE (`Id`='43969'); 
UPDATE `quest_template` SET `PrevQuestId`='42230' WHERE (`Id`='41877');
UPDATE `quest_template` SET `PrevQuestId`='43313' WHERE (`Id`='43317');
UPDATE `quest_template` SET `PrevQuestId`='43309' WHERE (`Id`='43311');
UPDATE `quest_template` SET `PrevQuestId`='43309' WHERE (`Id`='43310');
UPDATE `quest_template` SET `PrevQuestId`='42792' WHERE (`Id`='44052');
UPDATE `quest_template` SET `PrevQuestId`='42841' WHERE (`Id`='42792');
UPDATE `quest_template` SET `PrevQuestId`='43969' WHERE (`Id`='42840');
UPDATE `quest_template` SET `PrevQuestId`='42840' WHERE (`Id`='42841'); 
UPDATE `quest_template` SET `PrevQuestId`='42840' WHERE (`Id`='43352');
UPDATE `quest_template` SET `PrevQuestId`='43311' WHERE (`Id`='43315');
UPDATE `quest_template` SET `PrevQuestId`='43315' WHERE (`Id`='43313');
UPDATE `quest_template` SET `PrevQuestId`='43317' WHERE (`Id`='43318');


-- https://ru.wowhead.com/npc=112009

UPDATE `creature_template` SET `AIName`='', `flags_extra`='130', `ScriptName`='' WHERE (`entry`='112150');
UPDATE `creature_template` SET `speed_walk`='0.7', `speed_run`='1.14286', `speed_fly`='1.14286', `unit_flags`='33554688', `unit_flags2`='2048', `VehicleId`='4919', `AIName`='', `InhabitType`='3', `HoverHeight`='1', `flags_extra`='130', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='112009');
UPDATE `creature_template` SET `faction`='188', `speed_walk`='1.6', `speed_run`='1.42857', `speed_fly`='1.14286', `unit_flags`='768', `unit_flags2`='2048', `AIName`='', `HoverHeight`='2', `RegenHealth`='1', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='112014');
UPDATE `creature_template` SET `faction`='188', `speed_walk`='1', `speed_run`='1', `speed_fly`='1.14286', `unit_flags`='33587200', `unit_flags2`='2048', `AIName`='', `InhabitType`='4', `HoverHeight`='1', `RegenHealth`='1', `flags_extra`='2', `ScriptName`='', `ScaleLevelMin`='100', `ScaleLevelMax`='110' WHERE (`entry`='112017');

delete from npc_spellclick_spells where npc_entry=112009;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
('112009', '46598', '1', '0');

delete from vehicle_template_accessory where EntryOrAura=112009;
INSERT INTO `vehicle_template_accessory` (`EntryOrAura`, `accessory_entry`, `seat_id`, `offsetX`, `offsetY`, `offsetZ`, `offsetO`, `minion`, `description`, `summontype`, `summontimer`) VALUES 
('112009', '112014', '1', '0', '0', '2', '0', '1', '112009-112014', '8', '0'),
('112009', '112017', '0', '-0.00705206', '0.00724156', '1.32745', '0', '1', '112009-112017', '8', '0');

UPDATE `creature` SET `spawndist`='7', `MovementType`='1' WHERE (`id`='112009');

-- вернем

delete from eventobject_template where entry=1220;
INSERT INTO `eventobject_template` (`entry`, `name`, `radius`, `SpellID`, `WorldSafeLocID`, `ScriptName`) VALUES 
('1220', 'dalaran_QuestCredit', '5', '0', '0', 'SmartEventObject');

delete from eventobject where id=1220;
INSERT INTO `eventobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `PhaseId`, `position_x`, `position_y`, `position_z`, `orientation`) VALUES 
('1220', '1220', '1220', '7502', '8045', '1', '1', '0', '-786.307', '4420.22', '602.463', '0');

delete from smart_scripts where source_type=10 and entryorguid in (1220);
INSERT INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(1220, '', 10, 0, 0, 78, 0, 100, 0, 0, 1220, 0, 0, 0, 0, 33, 106815, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1220, '', 10, 1, 0, 78, 0, 100, 0, 0, 1220, 0, 0, 0, 0, 33, 107587, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1220, '', 10, 2, 0, 78, 0, 100, 0, 0, 1220, 0, 0, 0, 0, 33, 109750, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(1220, '', 10, 3, 0, 78, 0, 100, 0, 0, 1220, 0, 0, 0, 0, 33, 110409, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "");

