
UPDATE `creature_template` SET `unit_flags`='0' WHERE (`entry`='21909');


-- Монастырь алого ордена, Залы алого ордена: Исправлен вход в подземелье
delete from areatrigger_teleport where id in (614, 45);
INSERT INTO `areatrigger_teleport` (`id`, `name`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES
('614', 'Scarlet Halls - Entrance', '1001', '820.743', '607.812', '13.6389', '0'),
('45', 'Scarlet Monastery - Entrance', '1004', '1124.57', '511.299', '0.9892', '1.53889');