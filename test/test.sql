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


select * from student2 where score = 90.0;--QAQOvO fdafs select



select id, score, name from student2 where score >= 90.0;--QAQOvO fdafs 


create index xxx on student2(id);

select * from xxx;

select id from xxx;
-- drop table student2;
