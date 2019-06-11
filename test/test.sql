create table student2(
	id int,
	name char(12) unique,
	score float,
	primary key(id)
);

insert into student2 values(1080100001,'name1',99);
insert into student2 values(1080100002,'name2',52.5);
insert into student2 values(1080100003,'name3',98.5);
insert into student2 values(1080100004,'name4',91.5);
insert into student2 values(1080100005,'name5',72.5);
insert into student2 values(1080100006,'name6',89.5);
insert into student2 values(1080100007,'name7',63);
insert into student2 values(1080100008,'name8',73.5);
insert into student2 values(1080100009,'name9',79.5);
insert into student2 values(1080100010,'name10',70.5);
insert into student2 values(1080100011,'name11',89.5);
insert into student2 values(1080100012,'name12',62);
insert into student2 values(1080100013,'name13',57.5);
insert into student2 values(1080100014,'name14',70.5);
insert into student2 values(1080100015,'name15',93.5);
insert into student2 values(1080100016,'name16',80);
insert into student2 values(1080100017,'name17',71.5);
insert into student2 values(1080100018,'name18',81.5);
insert into student2 values(1080100019,'name19',55);
insert into student2 values(1080100020,'name20',71.5);
insert into student2 values(1080100021,'name21',93.5);
insert into student2 values(1080100022,'name22',93.5);
insert into student2 values(1080100023,'name23',69.5);
insert into student2 values(1080100024,'name24',62.5);
insert into student2 values(1080100025,'name25',63);
insert into student2 values(1080100026,'name26',73.5);
insert into student2 values(1080100027,'name27',52);
insert into student2 values(1080100028,'name28',57.5);
insert into student2 values(1080100029,'name29',57.5);
insert into student2 values(1080100030,'name30',81.5);
insert into student2 values(1080100031,'name31',51.5);
insert into student2 values(1080100032,'name32',67.5);
insert into student2 values(1080100033,'name33',86);
insert into student2 values(1080100034,'name34',52);
insert into student2 values(1080100035,'name35',71.5);
insert into student2 values(1080100036,'name36',52.5);
insert into student2 values(1080100037,'name37',92);
insert into student2 values(1080100038,'name38',80.5);
insert into student2 values(1080100039,'name39',87);
insert into student2 values(1080100040,'name40',63.5);
insert into student2 values(1080100041,'name41',82.5);
insert into student2 values(1080100042,'name42',85.5);
insert into student2 values(1080100043,'name43',85.5);
insert into student2 values(1080100044,'name44',91);
insert into student2 values(1080100045,'name45',52.5);
insert into student2 values(1080100046,'name46',98.5);

select * from student2 where score = 90.0;--QAQOvO fdafs select

delete from student2 where score < 95.0;

select id, score, name from student2 where score >= 90.0;--QAQOvO fdafs 

drop table student2;
quit;
