-- [  СОЗДАНИЕ  ТАБЛИЦ ] =====================================================================================
create table tasks (
  title       VARCHAR  primary key,
  parenttask  VARCHAR,
  maxxp       NUMERIC
);

create table peers (
  nickname  VARCHAR  primary key ,
  birthday  date     not null
);

create table checks (
  id      BIGINT   primary key,
  peer    VARCHAR,
  task    VARCHAR,
  date    DATE     not NULL,
  
constraint fk_checks_peers  foreign key (peer) references  peers(nickname),
constraint fk_checks_tasks  foreign key (task) references  tasks(   title)
);


-- CHECKS
CREATE TYPE cond AS ENUM ('start', 'success', 'failure');

create table p2p (
  id            BIGINT   primary key,
  "check"       BIGINT,
  checkingpeer  varchar,
  state         cond,
  time          TIME     not NULL,
  
constraint fk_p2p_peers   foreign key (checkingpeer) references   peers(nickname),
constraint fk_p2p_checks  foreign key ("check"     ) references  checks(      id)
);

create table verter (
  id       BIGINT   primary key,
  "check"  BIGINT,
  state    cond,
  time     TIME     not NULL,

constraint fk_verter_checks  foreign key ("check") references  checks(id)
);

create table xp (
  id        BIGINT   primary key,
  "check"  	BIGINT,
  xpamount  NUMERIC,
  
constraint fk_xp_checks  foreign key ("check") references  checks(id)
);


-- ONLY PEARS
CREATE TABLE timetracking (
  id      BIGINT   primary key,
  peer    varchar,
  date    DATE     not NULL,
  time    TIME     not NULL,
  state   INTEGER,
  
constraint ch_state               check       (state in (1, 2)),
constraint fk_timetracking_peers  foreign key (peer) references  peers(nickname)
);

create table friends (
  id      BIGINT   primary key,
  peer1   VARCHAR,
  peer2   VARCHAR,
  
constraint fk_friends_peers1  foreign key (peer1)  references  peers(nickname),
constraint fk_friends_peers2  foreign key (peer2)  references  peers(nickname)
);

create table recommendations (
  id               BIGINT   primary key,
  peer  		   VARCHAR,
  recommendedpeer  VARCHAR,
  
constraint fk_recommendations_peers1  foreign key (           peer) references  peers(nickname),
constraint fk_recommendations_peers2  foreign key (recommendedpeer) references  peers(nickname)
);

create table transferredpoints (
  id            BIGINT   primary key,
  checkingpeer  VARCHAR,
  checkedpeer   VARCHAR,
  pointsamount  INTEGER,
  
constraint fk_transferredpoints_peers1  foreign key (checkingpeer) references  peers(nickname),
constraint fk_transferredpoints_peers2  foreign key (checkedpeer ) references  peers(nickname)
);




-- [ ТАБЛИЦА  <->  CSV ] =====================================================================================
CREATE OR REPLACE PROCEDURE import_table(tble text, csv_file text = NULL, delimetr char(1) = ',') AS $$
BEGIN
    IF csv_file IS NULL THEN csv_file := tble || '.csv'; END IF;
    EXECUTE format('COPY %s FROM %L WITH DELIMITER %L CSV HEADER', tble, csv_file, delimetr);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE PROCEDURE export_table (tble text, csv_file text = NULL, delimetr char(1) = ',') AS $$ 
BEGIN
    IF csv_file IS NULL THEN csv_file := tble || '.csv'; END IF;
    EXECUTE format('COPY %s TO %L WITH CSV DELIMITER %L HEADER', tble, csv_file, delimetr);
END;
$$ LANGUAGE plpgsql;




-- [ ЗАПОЛНЕНИЕ ТАБЛИЦ ] =====================================================================================
INSERT INTO peers VALUES 
    ('Himori', '2000-02-01'),
    ('Kuruho', '2000-02-04'),
    ('Kunori', '2000-02-05'),
    ('Reosao', '2000-02-01'),
    ('Tasami', '2000-02-03'),
    ('Ruriru', '2000-02-01'),
    ('Ubluwa', '2000-02-04'),
    ('Zatani', '2000-01-05'),
    ('Kaotku', '2000-02-01'),
    ('Ayachi', '2000-01-04');

    
INSERT INTO tasks VALUES 
	('CPP1_name1',          NULL,  300),
    ('CPP2_name2',  'CPP1_name1',  750),
    ('SQL0_name3',  'CPP2_name2',  800),
    
    ('AAA0_name4',  'CPP2_name2',  800),
    ('ZZZ0_name5',  'CPP2_name2',  800),
    ('SQL1_name6',  'SQL0_name3',  300),
    
    ('AAX1_name7',  'AAA0_name4',  500),
    ('AAX2_name8',  'AAX1_name7',  300),
    ('AAA1_name9',  'AAA0_name4',  500),
    ('CPP3_extra',  'CPP2_name2', 1500);



INSERT INTO friends VALUES
    ( (SELECT COALESCE(max(id),0) FROM friends) +  1, 'Himori', 'Kuruho'),
    ( (SELECT COALESCE(max(id),0) FROM friends) +  2, 'Himori', 'Kunori'),
    ( (SELECT COALESCE(max(id),0) FROM friends) +  3, 'Kuruho', 'Kunori'),
    ( (SELECT COALESCE(max(id),0) FROM friends) +  4, 'Kuruho', 'Reosao'),
    ( (SELECT COALESCE(max(id),0) FROM friends) +  5, 'Kuruho', 'Tasami'),
    ( (SELECT COALESCE(max(id),0) FROM friends) +  6, 'Reosao', 'Tasami'),
    ( (SELECT COALESCE(max(id),0) FROM friends) +  7, 'Ruriru', 'Ubluwa'),
    ( (SELECT COALESCE(max(id),0) FROM friends) +  8, 'Ruriru', 'Zatani'),
    ( (SELECT COALESCE(max(id),0) FROM friends) +  9, 'Ubluwa', 'Zatani'),
    ( (SELECT COALESCE(max(id),0) FROM friends) + 10, 'Ubluwa', 'Kaotku'),
    ( (SELECT COALESCE(max(id),0) FROM friends) + 11, 'Zatani', 'Kaotku'),
    ( (SELECT COALESCE(max(id),0) FROM friends) + 12, 'Kaotku', 'Ayachi');


INSERT INTO recommendations VALUES
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  1, 'Himori', 'Ruriru'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  2, 'Himori', 'Zatani'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  3, 'Zatani', 'Himori'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  4, 'Kaotku', 'Himori'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  5, 'Kunori', 'Ubluwa'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  6, 'Tasami', 'Kunori'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  7, 'Zatani', 'Kunori'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  8, 'Reosao', 'Tasami'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) +  9, 'Tasami', 'Reosao'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) + 10, 'Kuruho', 'Reosao'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) + 11, 'Ruriru', 'Zatani'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) + 12, 'Ayachi', 'Zatani'),
    ( (SELECT COALESCE(max(id),0) FROM recommendations) + 13, 'Kaotku', 'Ayachi');


INSERT INTO timetracking VALUES
-- x 9 Himori 3 Kuruho 2 Himori 2 Kuruho
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  1, 'Himori', '2022-01-01', '09:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  2, 'Himori', '2022-01-01', '10:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  3, 'Himori', '2022-01-01', '11:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  4, 'Kuruho', '2022-01-01', '12:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  5, 'Kuruho', '2022-01-01', '13:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  6, 'Himori', '2022-01-01', '14:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  7, 'Himori', '2022-01-01', '15:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  8, 'Kuruho', '2022-01-01', '16:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  9, 'Himori', '2022-01-01', '17:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 10, 'Kuruho', '2022-01-01', '18:10', 2),

-- x 9 Himori Kuruho Ayachi
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 11, 'Himori', '2022-01-02', '09:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 12, 'Kuruho', '2022-01-02', '10:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 13, 'Ayachi', '2022-01-02', '11:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 14, 'Himori', '2022-01-02', '12:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 15, 'Kuruho', '2022-01-02', '13:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 16, 'Ayachi', '2022-01-02', '14:10', 2),

-- x 7 Ayachi 3 Kaotku Himori Ruriru 3 Zatani
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 17, 'Ayachi', '2022-01-03', '07:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 18, 'Ayachi', '2022-01-03', '08:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 19, 'Ayachi', '2022-01-03', '09:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 20, 'Kaotku', '2022-01-03', '10:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 21, 'Himori', '2022-01-03', '11:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 22, 'Ruriru', '2022-01-03', '12:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 23, 'Ruriru', '2022-01-03', '13:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 24, 'Ruriru', '2022-01-03', '14:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 25, 'Zatani', '2022-01-03', '15:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 26, 'Ayachi', '2022-01-03', '16:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 27, 'Kaotku', '2022-01-03', '17:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 28, 'Himori', '2022-01-03', '18:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 29, 'Ruriru', '2022-01-03', '19:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 30, 'Zatani', '2022-01-03', '20:10', 2),

-- x 7 Ayachi Kaotku Zatani Ubluwa 2 Kaotku Zatani 2 Ubluwa Kaotku
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 31, 'Ayachi', '2022-01-04', '07:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 32, 'Kaotku', '2022-01-04', '08:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 33, 'Zatani', '2022-01-04', '09:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 34, 'Ubluwa', '2022-01-04', '10:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 35, 'Ubluwa', '2022-01-04', '11:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 36, 'Kaotku', '2022-01-04', '12:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 37, 'Zatani', '2022-01-04', '13:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 38, 'Zatani', '2022-01-04', '14:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 39, 'Ubluwa', '2022-01-04', '15:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 40, 'Kaotku', '2022-01-04', '16:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 41, 'Ayachi', '2022-01-04', '17:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 42, 'Kaotku', '2022-01-04', '18:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 43, 'Zatani', '2022-01-04', '19:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 44, 'Ubluwa', '2022-01-04', '20:10', 2),

-- x 6 Ayachi 3 Kunori 2 Reosao 3 Tasami 2 Reosao 2 Tasami
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 45, 'Ayachi', '2022-01-05', '06:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 46, 'Ayachi', '2022-01-05', '07:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 47, 'Ayachi', '2022-01-05', '08:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 48, 'Kunori', '2022-01-05', '09:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 49, 'Kunori', '2022-01-05', '10:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 50, 'Reosao', '2022-01-05', '11:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 51, 'Reosao', '2022-01-05', '12:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 52, 'Reosao', '2022-01-05', '13:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 53, 'Tasami', '2022-01-05', '14:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 54, 'Tasami', '2022-01-05', '15:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 55, 'Reosao', '2022-01-05', '16:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 56, 'Reosao', '2022-01-05', '17:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 57, 'Tasami', '2022-01-05', '18:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 58, 'Ayachi', '2022-01-05', '19:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 59, 'Reosao', '2022-01-05', '20:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 60, 'Tasami', '2022-01-05', '21:10', 2);


--   Himori 1 Zatani #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'CPP1_name1', '2022-01-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'failure', '01:10');


--   Ruriru 1 Kuruho #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ruriru', 'CPP1_name1', '2022-01-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho', 'failure', '01:10');


--   Ubluwa 1 Kunori -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'CPP1_name1', '2022-01-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 69);


--   Ayachi 1 Kaotku +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ayachi', 'CPP1_name1', '2022-01-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 258);


--   Kuruho 1 Tasami +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kuruho', 'CPP1_name1', '2022-01-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 291);


--   Himori 1 Zatani #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'CPP1_name1', '2022-01-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'failure', '01:10');


--   Himori 1 Kuruho +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'CPP1_name1', '2022-01-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 285);


--   Kaotku 1 Ruriru +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kaotku', 'CPP1_name1', '2022-01-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 279);


--   Ayachi 1 Reosao +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ayachi', 'CPP1_name1', '2022-01-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 285);


--   Reosao 1 Himori #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'CPP1_name1', '2022-01-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'failure', '01:10');


--   Tasami 1 Ruriru +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Tasami', 'CPP1_name1', '2022-01-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 258);


--   Zatani 1 Himori #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Zatani', 'CPP1_name1', '2022-01-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  9, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 10, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'failure', '01:10');


--   Himori 2 Kuruho Ubluwa #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'CPP2_name2', '2022-01-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa', 'failure', '03:10');


--   Kaotku 2 Kunori #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kaotku', 'CPP2_name2', '2022-01-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori', 'failure', '01:10');


--   Reosao 1 Tasami +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'CPP1_name1', '2022-01-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 276);


--   Tasami 2 Himori Kuruho V -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Tasami', 'CPP2_name2', '2022-01-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  9, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 10, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 11, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 12, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 67);


--   Zatani 1 Ruriru +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Zatani', 'CPP1_name1', '2022-01-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 243);


--   Kunori 1 Ubluwa -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kunori', 'CPP1_name1', '2022-01-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 126);


--   Kunori 1 Reosao +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kunori', 'CPP1_name1', '2022-01-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 261);


--   Ubluwa 1 Ayachi +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'CPP1_name1', '2022-01-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 270);


--   Ruriru 1 Kaotku +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ruriru', 'CPP1_name1', '2022-01-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  9, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 10, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 297);


--   Kaotku 2 Zatani Ruriru V -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kaotku', 'CPP2_name2', '2022-01-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 11, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 12, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 13, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 14, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 22);


INSERT INTO timetracking VALUES
-- x 5 Zatani Kunori Tasami Zatani Himori 3 Ubluwa
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  1, 'Zatani', '2022-02-01', '05:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  2, 'Kunori', '2022-02-01', '06:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  3, 'Tasami', '2022-02-01', '07:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  4, 'Zatani', '2022-02-01', '08:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  5, 'Himori', '2022-02-01', '09:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  6, 'Himori', '2022-02-01', '10:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  7, 'Himori', '2022-02-01', '11:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  8, 'Ubluwa', '2022-02-01', '12:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) +  9, 'Kunori', '2022-02-01', '13:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 10, 'Tasami', '2022-02-01', '14:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 11, 'Himori', '2022-02-01', '15:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 12, 'Ubluwa', '2022-02-01', '16:10', 2),

-- x 10 Reosao Ubluwa Kunori 2 Tasami Ayachi 2 Kuruho
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 13, 'Reosao', '2022-02-02', '10:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 14, 'Ubluwa', '2022-02-02', '11:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 15, 'Kunori', '2022-02-02', '12:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 16, 'Kunori', '2022-02-02', '13:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 17, 'Tasami', '2022-02-02', '14:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 18, 'Ayachi', '2022-02-02', '15:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 19, 'Ayachi', '2022-02-02', '16:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 20, 'Kuruho', '2022-02-02', '17:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 21, 'Reosao', '2022-02-02', '18:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 22, 'Ubluwa', '2022-02-02', '19:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 23, 'Tasami', '2022-02-02', '20:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 24, 'Kuruho', '2022-02-02', '21:10', 2),

-- Kaotku 2 Kunori 2 Reosao 2 Kaotku Ruriru
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 25, 'Kaotku', '2022-02-03', '00:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 26, 'Kaotku', '2022-02-03', '01:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 27, 'Kunori', '2022-02-03', '02:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 28, 'Kunori', '2022-02-03', '03:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 29, 'Reosao', '2022-02-03', '04:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 30, 'Reosao', '2022-02-03', '05:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 31, 'Kaotku', '2022-02-03', '06:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 32, 'Ruriru', '2022-02-03', '07:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 33, 'Kaotku', '2022-02-03', '08:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 34, 'Ruriru', '2022-02-03', '09:10', 2),

-- Ruriru 3 Kunori 2 Ruriru Zatani Ayachi
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 35, 'Ruriru', '2022-02-04', '00:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 36, 'Ruriru', '2022-02-04', '01:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 37, 'Ruriru', '2022-02-04', '02:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 38, 'Kunori', '2022-02-04', '03:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 39, 'Kunori', '2022-02-04', '04:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 40, 'Ruriru', '2022-02-04', '05:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 41, 'Zatani', '2022-02-04', '06:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 42, 'Ayachi', '2022-02-04', '07:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 43, 'Zatani', '2022-02-04', '08:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 44, 'Ayachi', '2022-02-04', '09:10', 2),

-- Ayachi Himori 4 Kuruho 2 Ubluwa Kaotku Tasami
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 45, 'Ayachi', '2022-02-05', '00:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 46, 'Himori', '2022-02-05', '01:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 47, 'Himori', '2022-02-05', '02:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 48, 'Himori', '2022-02-05', '03:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 49, 'Himori', '2022-02-05', '04:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 50, 'Kuruho', '2022-02-05', '05:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 51, 'Kuruho', '2022-02-05', '06:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 52, 'Ubluwa', '2022-02-05', '07:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 53, 'Kaotku', '2022-02-05', '08:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 54, 'Tasami', '2022-02-05', '09:10', 1),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 55, 'Ayachi', '2022-02-05', '10:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 56, 'Ubluwa', '2022-02-05', '11:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 57, 'Kaotku', '2022-02-05', '12:10', 2),
    ( (SELECT COALESCE(max(id),0) FROM timetracking) + 58, 'Tasami', '2022-02-05', '13:10', 2);


--   Himori 2 Ayachi Reosao V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'CPP2_name2', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 645);


--   Ubluwa 2 Zatani #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'CPP2_name2', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'failure', '01:10');


--   Himori 5 Ruriru #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'ZZZ0_name5', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'failure', '01:10');


--   Ayachi 2 Himori Kaotku V -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ayachi', 'CPP2_name2', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  9, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 10, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 11, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 12, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 450);


--   Ubluwa 2 Kunori Himori V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'CPP2_name2', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 13, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 14, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 15, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 16, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 645);


--   Reosao 2 Kuruho Ruriru V -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'CPP2_name2', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 17, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 18, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 19, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 20, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 390);


--   Himori 5 Ubluwa V #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'ZZZ0_name5', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 21, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 22, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'failure', '03:10');


--   Ruriru 2 Reosao Kunori V #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ruriru', 'CPP2_name2', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 23, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 24, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 25, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 26, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'failure', '05:10');


--   Himori 5 Reosao V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'ZZZ0_name5', '2022-02-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 27, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 28, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 768);


--   Kuruho 2 Kunori Himori V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kuruho', 'CPP2_name2', '2022-02-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 682);


--   Kunori 2 Kuruho Himori V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kunori', 'CPP2_name2', '2022-02-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 660);


--   Zatani 2 Kuruho #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Zatani', 'CPP2_name2', '2022-02-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  9, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 10, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho', 'failure', '01:10');


--   Reosao 3 Tasami V #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'SQL0_name3', '2022-02-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 11, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 12, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'failure', '03:10');


--   Ubluwa 3 Ruriru V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'SQL0_name3', '2022-02-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 13, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 14, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 696);


--   Zatani 2 Ubluwa Tasami V -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Zatani', 'CPP2_name2', '2022-02-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 15, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 16, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 17, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 18, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 217);


--   Himori 3 Ruriru V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'SQL0_name3', '2022-02-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 656);


--   Kuruho 4 Ruriru V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kuruho', 'AAA0_name4', '2022-02-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 720);


--   Reosao 3 Zatani V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'SQL0_name3', '2022-02-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 656);


--   Tasami 5 Ayachi V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Tasami', 'ZZZ0_name5', '2022-02-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 664);


--   Kunori 3 Ayachi #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kunori', 'SQL0_name3', '2022-02-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  9, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 10, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi', 'failure', '01:10');


--   Kunori 3 Himori V -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kunori', 'SQL0_name3', '2022-02-03');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 11, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 12, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 536);


--   Reosao  6 Tasami Kuruho +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'SQL1_name6', '2022-02-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Kuruho', 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 285);


--   Zatani 10 Himori #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Zatani', 'CPP3_extra', '2022-02-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'failure', '01:10');


--   Zatani 10 Ruriru Kaotku Ayachi V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Zatani', 'CPP3_extra', '2022-02-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  9, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 10, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi',   'start', '03:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 11, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku', 'success', '04:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 12, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi', 'success', '05:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '06:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '07:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 1335);


--   Ubluwa 10 Zatani Reosao #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'CPP3_extra', '2022-02-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 13, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 14, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 15, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 16, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'failure', '03:10');


--   Reosao  6 Zatani Himori +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'SQL1_name6', '2022-02-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 17, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 18, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 19, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 20, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 246);


--   Kuruho  7 Himori Kaotku +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kuruho', 'AAX1_name7', '2022-02-04');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 21, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 22, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 23, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 24, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku', 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 410);


--   Kuruho  8 Himori Kunori V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kuruho', 'AAX2_name8', '2022-02-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '05:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 294);


--   Kunori 10 Himori Reosao Ruriru V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Kunori', 'CPP3_extra', '2022-02-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '03:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  9, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '04:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 10, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '05:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '06:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '07:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 1335);


--   Reosao  6 Tasami Zatani -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'SQL1_name6', '2022-02-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 11, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 12, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 13, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 14, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 21);


--   Reosao  6 Tasami Kunori +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Reosao', 'SQL1_name6', '2022-02-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 15, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 16, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 17, (SELECT COALESCE(max(id),0) FROM checks), 'Tasami', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 18, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori', 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 279);


--   Ubluwa 10 Zatani Ruriru Ayachi V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'CPP3_extra', '2022-02-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 19, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 20, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 21, (SELECT COALESCE(max(id),0) FROM checks), 'Zatani', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 22, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi',   'start', '03:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 23, (SELECT COALESCE(max(id),0) FROM checks), 'Ruriru', 'success', '04:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 24, (SELECT COALESCE(max(id),0) FROM checks), 'Ayachi', 'success', '05:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '06:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '07:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 1440);


--   Ayachi  4 Himori #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ayachi', 'AAA0_name4', '2022-02-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 25, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 26, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'failure', '01:10');


--   Ayachi  4 Kaotku V -
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ayachi', 'AAA0_name4', '2022-02-05');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 27, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) + 28, (SELECT COALESCE(max(id),0) FROM checks), 'Kaotku', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 408);


--   Himori 5 Ubluwa V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'ZZZ0_name5', '2022-03-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Ubluwa', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 656);


--   Ruriru 2 Reosao Kunori V #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ruriru', 'CPP2_name2', '2022-03-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori',   'start', '01:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  5, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '02:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  6, (SELECT COALESCE(max(id),0) FROM checks), 'Kunori', 'success', '03:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '04:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'failure', '05:10');


--   Himori 5 Reosao V +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Himori', 'ZZZ0_name5', '2022-03-01');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  7, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  8, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '01:10');
INSERT INTO verter VALUES ( (SELECT COALESCE(max(id),0) FROM verter) + 1, (SELECT COALESCE(max(id),0) FROM checks),   'start', '02:10'),
                          ( (SELECT COALESCE(max(id),0) FROM verter) + 2, (SELECT COALESCE(max(id),0) FROM checks), 'success', '03:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 712);


--   Ubluwa 1 Reosao +
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'CPP1_name1', '2022-03-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  1, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  2, (SELECT COALESCE(max(id),0) FROM checks), 'Reosao', 'success', '01:10');
INSERT INTO     xp VALUES ( (SELECT COALESCE(max(id),0) FROM xp) + 1, (SELECT COALESCE(max(id),0) FROM checks), 279);


--   Ubluwa 1 Himori #
INSERT INTO checks VALUES ( (SELECT COALESCE(max(id),0) FROM checks) + 1, 'Ubluwa', 'CPP1_name1', '2022-03-02');
INSERT INTO    p2p VALUES
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  3, (SELECT COALESCE(max(id),0) FROM checks), 'Himori',   'start', '00:10'),
    ( (SELECT COALESCE(max(id),0) FROM p2p) +  4, (SELECT COALESCE(max(id),0) FROM checks), 'Himori', 'failure', '01:10');






    



INSERT INTO transferredpoints
SELECT ROW_NUMBER() OVER() AS id, *
FROM (  SELECT checkingpeer, peer, COUNT(*)
        FROM checks JOIN p2p ON "check" = checks.id and state != 'start'
        GROUP by 1, 2  );
    

