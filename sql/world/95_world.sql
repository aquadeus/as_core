
-- Karazhan Moroes Keys
UPDATE `creature_template` SET `npcflag`='16777216' WHERE (`entry`='117578');


delete from npc_spellclick_spells where npc_entry = 117578;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES ('117578', '233981', '1', '0');

-- DK Shipments - https://ru.wowhead.com/item=139888
delete from gameobject where id = 252771;
INSERT INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `phaseID`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `ScriptName`, `custom_flags`, `VerifiedBuild`) VALUES
('252771', '1220', '7679', '7679', '1', '0', '9087040', '-1413.84', '1169.24', '276.687', '0.749221', '0', '0', '0.36591', '0.93065', '300', '0', '1', '0', '', '0', '-1'),
('252771', '1220', '7679', '7679', '1', '0', '9087043', '-1413.84', '1169.24', '276.687', '0.749221', '0', '0', '0.36591', '0.93065', '300', '0', '1', '0', '', '0', '-1');
