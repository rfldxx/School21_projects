-- интересные номера: 6 -> 8,  9 -> 10,  13

-- [  1 ] ======================================================================================================
CREATE or REPLACE FUNCTION f1() RETURNs TABLE (Peer1         transferredpoints.checkingpeer%TYPE,
                                               Peer2         transferredpoints.checkedpeer %TYPE,
                                               PointsAmount  transferredpoints.pointsamount%TYPE) AS $$
  SELECT t1.checkingpeer, t1.checkedpeer, t1.pointsamount - COALESCE(t2.pointsamount, 0)
  FROM      transferredpoints t1
  LEFT JOIN transferredpoints t2  ON  t2.checkingpeer = t1.checkedpeer AND t2.checkedpeer = t1.checkingpeer
  WHERE  (t2.checkingpeer ISNULL) OR (t1.checkingpeer < t1.checkedpeer); -- либо так (t2.id ISNULL) OR (t1.id < t2.id);
$$ LANGUAGE sql;

SELECT * FROM f1() ORDER by 3 DESC, 1, 2;



-- [  2 ] ======================================================================================================
CREATE or REPLACE FUNCTION f2() RETURNs TABLE (Peer checks.peer%TYPE, Task checks.task%TYPE, Xp xp.xpamount%TYPE) AS $$
  SELECT   peer, task, xpamount
  FROM   checks JOIN xp ON xp.check = checks.id;
$$ LANGUAGE sql;

SELECT * FROM f2() ORDER by 1, 2;



-- [  3 ] ======================================================================================================
CREATE or REPLACE FUNCTION f3(d timetracking.date%TYPE) RETURNs TABLE (Peer timetracking.peer%TYPE) AS $$
  SELECT   peer
  FROM   timetracking WHERE date = d AND state = 1 
  GROUP by 1  HAVING COUNT(*) = 1
$$ LANGUAGE sql;

SELECT * FROM f3('2022-01-03') ORDER by 1;



-- [  4 ] ======================================================================================================
CREATE or REPLACE FUNCTION f4() RETURNs TABLE (Peer         transferredpoints.checkedpeer %TYPE, 
                                               PointsChange transferredpoints.pointsamount%TYPE) AS $$
  SELECT checkedpeer,  SUM(x)
  FROM (
     SELECT checkingpeer, checkedpeer , -pointsamount as x FROM transferredpoints
    UNION ALL
     SELECT checkedpeer , checkingpeer, +pointsamount      FROM transferredpoints
   )
  GROUP by 1 ORDER by 2 DESC;
$$ LANGUAGE sql;

SELECT * FROM f4();



-- [  5 ] ======================================================================================================
CREATE or REPLACE FUNCTION f5() RETURNs TABLE (Peer         transferredpoints.checkedpeer %TYPE, 
                                               PointsChange transferredpoints.pointsamount%TYPE) AS $$
  SELECT peer2,  -SUM(pointsamount)
  FROM (
     SELECT *                            FROM f1()
    UNION ALL
     SELECT peer2, peer1, -pointsamount  FROM f1()
   )
  GROUP by 1 ORDER by 2 DESC;
$$ LANGUAGE sql;

SELECT * FROM f5();



-- [  6 ] ======================================================================================================
CREATE or REPLACE FUNCTION f6() RETURNs TABLE (day checks.date%TYPE, task checks.task%TYPE) AS $$
  with counter as (
    SELECT date, task, COUNT(*) as cc
    FROM   checks JOIN p2p ON "check" = checks.id
    GROUP by 1, 2
  )
  SELECT date, task
  from counter NATURAL JOIN (SELECT date, max(cc) as mx FROM counter GROUP by 1)
  WHERE cc = mx
$$ LANGUAGE sql;

SELECT * FROM f6() ORDER by day;



-- [  7 ] ======================================================================================================
CREATE or REPLACE PROCEDURE f7(prfx checks.task%TYPE, refcrsr refcursor) AS $$ 
BEGIN OPEN refcrsr FOR
  SELECT peer, max(mx) as day
  FROM (  SELECT peer, max(date) as mx
          FROM checks JOIN xp on "check" = checks.id
          WHERE task  in (SELECT title FROM tasks WHERE title LIKE prfx || '%')
          GROUP by 1, task    ) 
  GROUP by 1
  HAVING COUNT(*)  =  (SELECT COUNT(*) FROM tasks WHERE title LIKE prfx || '%')
  ORDER by 2 DESC;
end;
$$ LANGUAGE plpgsql;

CALL f7('CPP', 'bred');
FETCH ALL IN "bred";



-- [  8 ] ======================================================================================================
CREATE or REPLACE FUNCTION f8() RETURNs TABLE (peer friends.peer1%TYPE, RecommendedPeer friends.peer1%TYPE) AS $$
  with counter as (
      SELECT peer1 as pp, recommendedpeer, COUNT(*) as cc
      FROM (   SELECT peer1, peer2 FROM friends
             UNION ALL
               SELECT peer2, peer1 FROM friends  )
      JOIN recommendations ON (peer = peer2 AND recommendedpeer != peer1)
      GROUP by 1, 2
  )
  SELECT pp, recommendedpeer
  from counter NATURAL JOIN (SELECT pp, max(cc) as mx FROM counter GROUP by 1)
  WHERE cc = mx
$$ LANGUAGE sql;

SELECT DENSE_RANK() OVER (ORDER by peer), * FROM f8() ORDER by 1;



-- [  9 ] ======================================================================================================
CREATE or REPLACE PROCEDURE f9(prfx1 checks.task%TYPE, prfx2 checks.task%TYPE,
out StartedBlock1 real, out StartedBlock2 real, out StartedBothBlocks real, out DidntStartAnyBlock real) AS $$
  WITH inf AS (
    SELECT nickname, max((task LIKE prfx1 || '%')::int) blk1, max((task LIKE prfx2 || '%')::int) blk2
    FROM   peers    JOIN checks ON nickname = peer
    GROUP  by nickname
  )
  SELECT any_value(mlt)*sum(blk1), any_value(mlt)*sum(blk2), any_value(mlt)*sum( (blk1=1 AND blk2=1) :: int), 
         any_value(mlt)*SUM( (blk1 = 0 and blk2 = 0) :: int) 
  FROM inf, (SELECT 100::real / (SELECT COUNT(*) FROM peers) as mlt)
$$ LANGUAGE sql;

CALL f9('SQL', 'AAA', 1, 1, 1, 1); -- отвратительнейший синтаксис x2



-- [  10 ] ======================================================================================================
CREATE or REPLACE FUNCTION f10 (out SuccessfulChecks  real, out UnsuccessfulCheck real) AS $$
  WITH inf AS (
      SELECT max((xp.id NOTNULL)::int) p1, max((xp.id ISNULL)::int) p2
      FROM checks  LEFT JOIN xp ON "check" = checks.id
   	  JOIN  peers  on   nickname = peer AND to_char(birthday, 'dd mm') = to_char(date, 'dd mm')
      GROUP BY nickname
  )
  SELECT any_value(mlt) * sum(p1),  any_value(mlt) * sum(p2)
  FROM   inf, (SELECT 100::real / (SELECT COUNT(*) FROM peers) as mlt)
$$ LANGUAGE sql;

SELECT * FROM f10();



-- [  11 ] ======================================================================================================
CREATE or REPLACE PROCEDURE f11(tsk1 checks.task%TYPE, tsk2 checks.task%TYPE, tsk3 checks.task%TYPE ,refcrsr refcursor) AS $$ 
BEGIN OPEN refcrsr FOR
  WITH inf AS (
      SELECT peer, MAX( (task=tsk1)::int ) t1, MAX( (task=tsk2)::int ) t2, MAX( (task=tsk3)::int ) t3
    -- человек должен сдать задание  =>  запись об этом человеке содержится в xp
    --                               =>  можно делать просто JOIN xp (а не LEFT JOIN xp)
      FROM checks JOIN xp on "check" = checks.id
      GROUP by peer
  )
  SELECT peer FROM inf WHERE t1=1 AND t2=1 AND t3=0;
end;
$$ LANGUAGE plpgsql;

-- дебильнейший синтаксис
CALL f11('CPP1_name1', 'CPP2_name2', 'SQL0_name3', 'bred');
FETCH ALL IN "bred";



-- [  12 ] ======================================================================================================
CREATE or REPLACE FUNCTION f12() RETURNs TABLE (task tasks.title%TYPE, PrevCount int) AS $$ 
    WITH RECURSIVE r AS (
        SELECT title as task, 0 as PrevCount FROM tasks WHERE parenttask IS NULL
      UNION ALL
        SELECT title, PrevCount+1 FROM tasks JOIN r on parenttask = task
      )
    SELECT * FROM r;
$$ LANGUAGE sql;

SELECT * FROM f12();



-- [  13 ] ======================================================================================================
-- гениально: https://stackoverflow.com/questions/53484179/sql-server-how-to-find-maximum-length-continuous-range
CREATE or REPLACE PROCEDURE f13(n INTEGER, refcrsr refcursor) AS $$ 
BEGIN OPEN refcrsr FOR
  WITH used_data AS (
          SELECT 0 check_id, date, '00:00' proverka_t, true fail FROM checks GROUP by date
      UNION ALL
          SELECT  checks.id, date, MIN(time),  any_value( (COALESCE(100*xpamount, 0) < 80*maxxp) ) fail
          FROM p2p JOIN checks on p2p.check = checks.id 
              LEFT JOIN     xp ON  xp.check = checks.id
                   JOIN  tasks ON     title = task
          GROUP BY checks.id
  ),   stairs   AS  (
          SELECT *, SUM(fail::int) OVER (PARTITION by date ORDER by proverka_t, check_id) cc FROM  used_data
  )
  SELECT DISTINCT date   from stairs   GROUP by date, cc   HAVING COUNT(*) - 1 >= n;
end;
$$ LANGUAGE plpgsql;

CALL f13(3, 'bred');
FETCH ALL IN bred;



-- [  14 ] ======================================================================================================
-- балл за задание  -  xp с "check" равным check_id: максимальным (date, time) начала p2p этапа
-- берем first_value, потому что там каикие-то замутки с last_value
--                    плюс когда мы сортируем time desc то равные NULL значения оказывается наверху 
--                                  (т.е. те проверки, у которых еще не было p2p - они самые свежие)  
CREATE or REPLACE FUNCTION f14() RETURNs TABLE (peer checks.peer%TYPE, XP int) AS $$ 
    WITH find_id_of_last AS (
      SELECT DISTINCT peer, task, 
            FIRST_VALUE(checks.id) OVER (PARTITION by peer, task ORDER by date DESC, time DESC, id DESC) as check_id
      FROM checks LEFT JOIN (SELECT "check", MIN(time) as time FROM p2p GROUP by 1) ON id = "check" -- начало p2p этапа
    )
    SELECT peer, sum(COALESCE(xpamount, 0))  
    FROM  find_id_of_last LEFT JOIN xp on check_id = xp.check 
    GROUP by 1   ORDER by 2   DESC LIMIT 1
$$ LANGUAGE sql;

SELECT * FROM f14();



-- [  15 ] ======================================================================================================
CREATE or REPLACE PROCEDURE f15(tt timetracking.time%TYPE, n INTEGER, refcrsr refcursor) AS $$ 
BEGIN OPEN refcrsr FOR
    WITH inf as (
      SELECT peer, date, min(time) as time FROM timetracking GROUP BY 1, 2
    )
    SELECT peer  FROM inf  WHERE time < tt
    GROUP by 1 HAVING COUNT(*) >= n;
end;
$$ LANGUAGE plpgsql;

CALL f15('12:00', 3, 'bred');
FETCH ALL IN bred;



-- [  16 ] ======================================================================================================
CREATE or REPLACE PROCEDURE f16(n INTEGER, m INTEGER, refcrsr refcursor) AS $$ 
DECLARE curr date  = NOW()::date; --'2022-02-05';
BEGIN OPEN refcrsr FOR
    WITH inf AS (
      SELECT peer, date, COUNT(*) dd FROM timetracking WHERE date <= curr AND curr < date + n GROUP by 1, 2
    )  
    SELECT peer  FROM inf  GROUP by 1  HAVING sum(dd-2) > 2*m;
end;
$$ LANGUAGE plpgsql;

CALL f16(60, 3, 'bred');
FETCH ALL IN bred;



-- [  17 ] ======================================================================================================
CREATE or REPLACE FUNCTION f17() RETURNs TABLE(Month VARCHAR, EarlyEntries int) AS $$ 
    WITH inf AS (
        SELECT to_char( date, 'Month') mm, min(time) < '12:00' cc
        FROM   timetracking JOIN peers ON peer = nickname AND to_char(date, 'mm') = to_char(birthday, 'mm')
        GROUP  by peer, date
    )  
    SELECT  mm, ROUND(SUM(cc::int) * 100::real / count(*))  FROM inf  GROUP by 1 
$$ LANGUAGE sql;

SELECT * FROM f17();

