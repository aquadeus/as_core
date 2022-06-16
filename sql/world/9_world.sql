-- https://forum.wowcircle.net/showthread.php?t=1039688
update quest_template set prevquestid = 34808, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 34818;


-- https://forum.wowcircle.net/showthread.php?t=1039688
update quest_template set prevquestid = 0, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 35272;
update quest_template set prevquestid = 35272, nextquestid = 0, exclusivegroup = 0, nextquestidchain = 0 where id = 35275;

DELETE FROM `creature_queststarter` WHERE quest = 35272 and id = 81890;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(81890, 35272);