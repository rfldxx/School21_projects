-- [  1 ] ======================================================================================================
-- процедурa уничтожает все таблицы текущей базы данных, имена которых начинаются с 'TableName'
-- (не уничтожая базу данны)

CREATE OR REPLACE PROCEDURE p1() AS $$
BEGIN
  EXECUTE format('DROP TABLE IF EXISTS %s CASCADE',
          COALESCE( (SELECT string_agg(table_name, ',') FROM INFORMATION_SCHEMA.TABLES
                     WHERE table_name LIKE 'tablename%' AND table_schema = 'public' ),
                    'tablename')  );
END;
$$ LANGUAGE plpgsql;


CREATE TABLE TableName  (i int);
CREATE TABLE TableNam1  (x varchar);
CREATE TABLE TableNama  (y text);
CREATE TABLE TableNamee (z bigint);

CALL p1();
--select * FROM INFORMATION_SCHEMA.TABLES ORDER by 2 DESC, 3;

DROP TABLE TableNam1, TableNama;



-- [  2 ] ======================================================================================================
-- процедурa выводит список имен и параметров всех скалярных SQL функций пользователя в текущей базе данных
-- вывод - кол-во таких элементов
CREATE or REPLACE PROCEDURE p2(out n int, out txt text) AS $$ 
  WITH inf AS (
          SELECT x.specific_name,  x.routine_name func, NOT z.proretset is_scalar,
          COALESCE(
            string_agg( CASE WHEN  y.parameter_mode='OUT' THEN 'OUT ' ELSE '' END  
                       || COALESCE(y.parameter_name || ' ', '')
                       || y.data_type, ', ' order by y.ordinal_position),
            ' '
          ) params

          FROM   INFORMATION_SCHEMA.routines x
          JOIN   pg_proc z on x.specific_name = proname || '_' || oid  -- ради одной!! колонки: proretset подключаем жуть
          JOIN INFORMATION_SCHEMA.parameters y ON x.specific_name = y.specific_name -- требуют только функции с параметрами (поэтому без LEFT)
          WHERE x.specific_schema = 'public' AND x.routine_type = 'FUNCTION'
          GROUP BY 1, 2, 3
        )
  SELECT COALESCE( count(*), 0 ), 
         COALESCE( string_agg( func || ' (' || params || ')', E'\n'), '')
  FROM inf WHERE is_scalar
$$ LANGUAGE sql;


CREATE TABLE xxx(i int, z text);
INSERT INTO  xxx VALUES (1, 'aacdsc'), (1, 'scmsk');

CREATE FUNCTION f1()      RETURNS xxx AS $$ SELECT * FROM xxx $$ LANGUAGE sql;
CREATE FUNCTION f1(x int) RETURNS xxx AS $$ SELECT * FROM xxx $$ LANGUAGE sql;
CREATE FUNCTION f1( real) RETURNS xxx AS $$ SELECT * FROM xxx $$ LANGUAGE sql;

CREATE FUNCTION f2(kk int, y varchar default 'test') RETURNS NUMERIC[] AS $$ SELECT ARRAY[1.0]        $$ LANGUAGE SQL;
CREATE FUNCTION f3()                                 RETURNS NUMERIC[] AS $$ SELECT ARRAY[i] FROM xxx $$ LANGUAGE SQL;

CREATE FUNCTION f4 (VARIADIC arr numeric[]) RETURNS int AS $$ SELECT 1 $$ LANGUAGE SQL;

CALL p2(1, 'bred');

DROP TABLE xxx CASCADE;
DROP FUNCTION f2, f3, f4;



-- [  3 ] ======================================================================================================
-- процедурa уничтожает все SQL DML триггеры в текущей базе данных
-- вывод - кол-во таких элементов
-- когда ровно это вставлено в p3(), то этот обреченый оптимизатор жалуется: db error: ERROR: "string_agg" is not a known variable
CREATE or REPLACE PROCEDURE p3_bred(out n int, out txt text) AS $$
   SELECT COUNT(*), string_agg('DROP trigger IF EXISTS ' || TRIGGER_NAME || ' on ' || event_object_table || ' CASCADE', e';\n')
   FROM information_schema.triggers WHERE trigger_schema = 'public'
$$ LANGUAGE SQL;
           
CREATE OR REPLACE PROCEDURE p3(OUT n int) AS $$
DECLARE	txxt text; -- мусорная перееменая
BEGIN
    CALL p3_bred(n, txxt);
    
    if txxt NOTNULL THEN  EXECUTE txxt;   end IF;
END;
$$ LANGUAGE plpgsql;


CREATE TABLE person (i int, x varchar);
  
CREATE OR REPLACE FUNCTION fnc_trg1 () RETURNS trigger AS $$ BEGIN RETURN NEW; END; $$ LANGUAGE plpgsql;
CREATE OR REPLACE  TRIGGER     trg1    AFTER   INSERT  ON person FOR EACH ROW  EXECUTE FUNCTION fnc_trg1();

CREATE OR REPLACE FUNCTION fnc_trg2 () RETURNS trigger AS $$ BEGIN RETURN OLD; END; $$ LANGUAGE plpgsql;
CREATE OR REPLACE  TRIGGER     trg2    AFTER   UPDATE  ON person FOR EACH ROW  EXECUTE FUNCTION fnc_trg2();

CREATE OR REPLACE FUNCTION fnc_trg3 () RETURNS trigger AS $$ BEGIN RETURN OLD; END; $$ LANGUAGE plpgsql;
CREATE OR REPLACE  TRIGGER     trg3    AFTER   DELETE  ON person FOR EACH ROW  EXECUTE FUNCTION fnc_trg3();

CALL p3(0);

DROP FUNCTION fnc_trg1, fnc_trg2, fnc_trg3;
DROP TABLE person;



-- [  4 ] ======================================================================================================
-- процедурa выводит   имена и описания   хранимых процедур и скалярных функций 
-- в тексте которых на языке SQL встречается строка, задаваемая параметром процедуры
-- в чем смысл задания? - калька второго номера
CREATE or REPLACE PROCEDURE p4(srch text, out txt text) AS $$ 
  WITH inf AS (
          SELECT x.routine_type typee, x.specific_name,  x.routine_name func, NOT z.proretset is_scalar,
          COALESCE(
            string_agg( CASE WHEN  y.parameter_mode='OUT' THEN 'OUT ' ELSE '' END  
                       || COALESCE(y.parameter_name || ' ', '')
                       || y.data_type, ', ' order by y.ordinal_position),
            ' '
          ) params

          FROM   INFORMATION_SCHEMA.routines x
          JOIN   pg_proc z on x.specific_name = proname || '_' || oid  -- ради одной!! колонки: proretset подключаем жуть
          LEFT JOIN INFORMATION_SCHEMA.parameters y ON x.specific_name = y.specific_name
          WHERE x.specific_schema = 'public' AND x.routine_name LIKE '%' || srch || '%'
          GROUP BY 1, 2, 3, 4
        )
  SELECT COALESCE( string_agg( '[' || LEFT(typee, 1) || ']: ' || func || ' (' || params || ')', E'\n'), '')
  FROM inf  WHERE  is_scalar
$$ LANGUAGE sql;

-- из 2 примеры
CALL p4('p', 'bred');

