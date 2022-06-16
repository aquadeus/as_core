
-- https://ru.wowhead.com/npc=27659 https://ru.wowhead.com/npc=27657 https://ru.wowhead.com/npc=27658
delete from gossip_menu_option where menu_id =9708;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`, `OptionBroadcastTextID`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES
('9708', '0', '0', 'So where do we go from here?', '1', '1', '9575', '0', '0', '0', '', '28427', '0', '0');

UPDATE `gossip_menu_option` SET `option_id`='1', `npc_option_npcflag`='1' WHERE (`menu_id`='9575');

update creature_template set scriptname='npc_verdisa_belgaristrasz_eternos' where entry in (27659, 27657, 27658);

