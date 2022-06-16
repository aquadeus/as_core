
REPLACE INTO `smart_scripts` (`entryorguid`, `linked_id`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_spawnMask`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(91462, '0', 0, 0, 1, 62, 0, 100, 0, 0, 914620, 0, 0, 0, 0, 33, 91461, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 914620 selected - Action invoker: Give kill credit Kill Credit: Speak to Malfurion (91461) // '),
(91462, '0', 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 12, 764852, 5, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ' Linked - Self: Summon creature Malfurion Stormrage (764852) at Action invoker, moved by offset (0, 0, 0, 0) // '),
(91462, '0', 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ' Linked - Action invoker: Close gossip // '),
(91462, '0', 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 15, 40122, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'Linked - Area explored or event happens for quest 40122');

REPLACE INTO `quest_template_objective` (`ID`, `QuestID`, `Type`, `Order`, `Index`, `ObjectID`, `Amount`, `Flags`, `Flags2`, `ProgressBarWeight`, `Description`, `VisualEffects`, `VerifiedBuild`) VALUES 
(280339, 40122, 0, 0, 0, 91461, 1, 0, 0, 0, 'Speak to Malfurion', '3374', 21992);
