
delete from trinity_string where entry in (13000, 13001, 13002, 13003, 13004, 13005);
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `content_loc9`, `content_loc10`) VALUES
('13000', 'Quest doesn\'t exist in DB.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Задание отсутствует в базе данных.', NULL, NULL),
('13001', 'Player has quest %u and not rewarded.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'У игрока взято и не завершено задание %u.', NULL, NULL),
('13002', 'Player has completed and not rewarded quest %u.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'У игрока выполнено и не сдано задание %u.', NULL, NULL),
('13003', 'Player has rewarded quest %u.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Игрок завершил задание %u.', NULL, NULL),
('13004', 'Player doesn\'t have a quest %u.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'У игрока отсутствует задание %u.', NULL, NULL),
('13005', 'Player has failed quest %u.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'У игрока провалено задание %u.', NULL, NULL);
