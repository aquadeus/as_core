-- https://forum.wowcircle.com/showthread.php?t=1057139
ALTER TABLE `retail_mail_template_locales`
MODIFY COLUMN `col_0_ruRU` text CHARACTER SET utf8 COLLATE utf8_general_ci;

delete from retail_mail_template where ROW_ID = 500;
delete from retail_mail_template_locales where ROW_ID = 500;
INSERT INTO `retail_mail_template` (`ROW_ID`, `col_0`) VALUES 
(500, 'Dear $n.

It has come to my attention that you handled the delivery of a missing parcel. For that, you have my gratitude!

We are in somewhat of a crunch here in the mailroom, and I have need of a capable courier. 

If you are interested, please visit my office. You will find the entrance behind the Magus Commerce Exchange.

- The Postmaster'); 

INSERT INTO `retail_mail_template_locales` (`ROW_ID`, `col_0_ruRU`) VALUES 
(500, '$n!

Я заметил, что тебе удалось доставить потерянную посылку. Приношу свою благодарность!

У нас не хватает рук, и услуги хорошего курьера нам не помешают.

Если тебя заинтересовало мое предложение, жду тебя в офисе за Торговой палатой волшебников.

– Почтальон'); 


-- https://forum.wowcircle.com/showthread.php?t=1055368
delete from retail_achievement where ROW_ID = 13000;
INSERT INTO `retail_achievement` (`col_0`, `col_1`, `col_2`, `col_3`, `col_4`, `col_5`, `col_6`, `col_7`, `col_8`, `col_9`, `col_10`, `col_11`, `ROW_ID`, `col_13`, `col_14`, `BuildVerified`) VALUES 
(NULL, NULL, NULL, 1179648, 65535, 0, 15252, 57, 0, 255, 0, 0, 13000, 136243, 56429, NULL);