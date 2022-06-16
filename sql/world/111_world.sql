
delete from command where name in ('quest status', 'quest clearjournal');
INSERT INTO `command` (`name`, `security`, `security_animator`, `help`) VALUES
('quest status', '2', '2', 'Syntax: .quest status #ID'),
('quest clearjournal', '2', '2', '');