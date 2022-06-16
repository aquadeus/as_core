
delete from item_bonus_group_linked where itemEntry in (151576, 151587, 151588, 151589, 151590, 151577, 151578, 151571) and itemBonusGroup=28;
INSERT INTO `item_bonus_group_linked` (`itemEntry`, `itemBonusGroup`) VALUES 
('151576', '28'),
('151587', '28'),
('151588', '28'),
('151589', '28'),
('151590', '28'),
('151577', '28'),
('151578', '28'),
('151571', '28');

-- invis aura on all NPC's who has linked_id == ""  ???
DELETE FROM `creature_addon` WHERE (`linked_id`='\"\"');