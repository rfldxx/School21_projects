-- НАДО НЕ ПОЛЕНИТЬСЯ И ПРОВЕРИТЬ!!! вроде сделано


-- [ 1) Написать процедуру добавления P2P проверки ] ======================================================
--       Параметры: ник проверяемого, ник проверяющего, название задания, статус проверки, время
--	 Если статус    «начало» - то запись такжe происходит добавление в Checks (дата сегодняшняя)
--	 Если статус не «начало» - то завершается проверка с незавершенным P2P этапом

CREATE or REPLACE PROCEDURE add_p2p(p1  peers.nickname%TYPE, p2    peers.nickname%TYPE,
                                    tsk    tasks.title%TYPE, cond       p2p.state%TYPE,
                                    t         p2p.time%TYPE) AS $$
DECLARE chk_id checks.id%TYPE;
BEGIN
    if( cond = 'start' ) THEN 
	INSERT INTO checks VALUES ( (SELECT max(id)+1 FROM checks), p1, tsk, now()::date );
	chk_id = (SELECT max(id) FROM checks);
    ELSE chk_id = (SELECT ID 
    	FROM (
    	    ( SELECT checks.id, checks.date  -- id всех проверок пира_1  пиром_2 задания tsk
              FROM   checks    JOIN p2p         ON    p2p.check = checks.id
              WHERE  peer = p1  and task = tsk AND checkingpeer = p2 ) 
	  EXCEPT 
	        ( SELECT checks.id, checks.date  -- id проверок за которые уже выставлен результат
              FROM   checks    JOIN p2p         ON    p2p.check = checks.id
              WHERE  peer = p1  and task = tsk AND checkingpeer = p2 AND state != 'start' )
          ORDER BY date, id LIMIT 1 -- находим самую давнейщую проверку подходящую под условия
      	));
    end if;
    
    INSERT INTO p2p VALUES ( (SELECT max(id)+1 FROM p2p), chk_id, p2, cond, t);
END;
$$ LANGUAGE plpgsql;




-- [ 2) Написать процедуру добавления проверки Verter'ом ] ======================================================
--       Параметры: ник проверяемого, название задания, статус проверки Verter'ом, время
--	 Ищется подходящая проверка с самым поздним (по времени) успешным P2P этапом
--       			    ~~~~~~~~~~~~~~~
--                                    ^- что это значит? берем самое новое или самое старое время?!
--                                       я брал самое недавнее, т.е. LIFO по сути
--                                       т.к. можно представить, что пир рестартнул проект не дожавшись вертера


CREATE or REPLACE PROCEDURE add_verter(p1  peers.nickname%TYPE,
                                       tsk    tasks.title%TYPE, cond  p2p.state%TYPE,
                                       t         p2p.time%TYPE) AS $$
DECLARE
    chk_id    checks.id%TYPE;
    old_const BOOLEAN  =   0;
BEGIN
   if(cond = 'start')
   THEN chk_id = (SELECT id from (SELECT checks.id, max(p2p.time)  -- ищем задания с успешно пройденым p2p  <=>  count('start') == count('success')
       				  -- + не запускался вертер (если old_const = 0)
       	          FROM   p2p  JOIN  checks  ON   checks.id =    p2p.check
                         LEFT JOIN  verter  on   checks.id = verter.check
                  WHERE peer  = p1  AND   task = tsk AND verter.id ISNULL
		  GROUP by date,   1 HAVING  SUM((p2p.state = 'start')::INT) = SUM((p2p.state = 'success')::INT)
                  ORDER by date DESC, 	2 DESC, 1 DESC    LIMIT  1  ));
                   
    -- без проверки на дурака: если  мы  добавляем результат работы вертера, то значит 
    --                         гарантируется, что в таблице есть соответствущий 'start'
    ELSE chk_id = (SELECT "check" 
       		   FROM (    (SELECT "check", date FROM verter JOIN checks ON checks.id = "check"
                              WHERE   peer  = p1    AND  task =  tsk )
		           EXCEPT
			     (SELECT "check", date FROM verter JOIN checks ON checks.id = "check" 
                              WHERE   peer  = p1    AND  task =  tsk AND  state  != 'start')
                           ORDER BY 2 DESC, 1 DESC  
                           LIMIT 1  ));
    end if;
    
    INSERT INTO verter VALUES ( (SELECT max(id)+1 FROM verter), chk_id, cond, t );
END;
$$ LANGUAGE plpgsql;




-- [ 3) Написать триггер по добавлению в P2P ] ============================================================
--       после добавления записи со статутом «начало», изменить соответствующую запись в TransferredPoints

CREATE FUNCTION fnc_trg_p2p () RETURNS TRIGGER AS $$
DECLARE p1 transferredpoints.checkedpeer%TYPE = (SELECT peer FROM checks WHERE id = NEW.check);
BEGIN
    UPDATE transferredpoints
    SET    pointsamount = pointsamount + 1
    WHERE  checkingpeer = NEW.checkingpeer AND checkedpeer = p1;

    IF NOT FOUND THEN
	INSERT INTO transferredpoints VALUES 
    	( (SELECT max(id) FROM transferredpoints) + 1, NEW.checkingpeer, p1, 1);
    END IF;
    
    RETURN new;
END;
$$ LANGUAGE plpgsql;


CREATE   TRIGGER trg_p2p   
AFTER  INSERT ON     p2p
FOR    EACH  ROW
WHEN  (NEW.state = 'start')
EXECUTE FUNCTION fnc_trg_p2p ();




-- [ 4) Написать триггер по проверки коректности добавляемой записи в XP ] ================================
--       Корректна, если:
--		Количество XP не превышает максимальное доступное для проверяемой задачи
--		Поле Check ссылается на успешную проверку
--	 Если запись не прошла проверку, не добавлять её в таблицу

CREATE OR REPLACE FUNCTION fnc_trg_xp () RETURNS TRIGGER AS $$
BEGIN
    if (  NEW.xpamount > (SELECT maxxp FROM tasks JOIN checks ON checks.id = NEW.check AND checks.task = title)  )
    THEN  RAISE  EXCEPTION 'High xp to project';
    END   IF;

    If ( COALESCE( (SELECT SUM((state = 'start')::INT) != SUM((state = 'success')::INT)
	            FROM   p2p    WHERE "check" = NEW.check ),
                    TRUE  )  )
    THEN  RAISE  EXCEPTION 'Some peer fail this project or were no prp checks';
    END   IF;
    
    If ( COALESCE( (SELECT SUM((state = 'start')::INT) != SUM((state = 'success')::INT)
	            FROM   verter WHERE "check" = NEW.check ),
                    FALSE )  )
    THEN  RAISE  EXCEPTION 'Verter fail this project';
    END   IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;


CREATE   TRIGGER trg_xp  
AFTER  INSERT ON     xp
FOR    EACH  ROW
EXECUTE FUNCTION fnc_trg_xp();
