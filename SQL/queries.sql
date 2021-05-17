/****************Queries para o 3º sprint ***************/

/*
sinf2021a34
DuOHMMxK
*/

/*  Ex 1  */

SET search_path TO dba34;
SELECT tempo AS timestamp, valor_medido AS measurement FROM valor_do_sensor WHERE id_sensor=

SELECT tempo AS timestamp, valor_medido AS measurement FROM valor_do_sensor AS vs, sensor AS s  WHERE vs.id_sensor=s.id_sensor