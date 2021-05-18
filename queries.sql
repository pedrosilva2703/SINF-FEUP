/****************Queries para o 3º sprint ***************/

/*
sinf2021a34
DuOHMMxK
*/

/*  Ex 1  */

SET search_path TO dba34;
SELECT tempo AS timestamp, valor_medido AS measurement FROM valor_do_sensor WHERE id_sensor=

SELECT tempo AS timestamp, valor_medido AS measurement FROM valor_do_sensor AS vs, sensor AS s  WHERE vs.id_sensor=s.id_sensor

SELECT tempo AS timestamp, valor_medido AS measurement FROM valor_do_sensor AS vs, sensor AS s, mote AS m, sala as sa WHERE sa.nome

SELECT id_mote AS im FROM sala WHERE nome_da_sala='frigorifico'
INTERSECT
SELECT id_sensor FROM sensor WHERE id_mote=im AND tipo='temperatura';


SELECT sala.nome_da_sala, sensor.id_mote, sensor.id_sensor
FROM sala, sensor
WHERE sala.nome_da_sala='frigorifico' AND sala.id_mote=sensor.id_mote


SELECT * FROM sala WHERE nome_da_sala='frigorifico'

SELECT * FROM sensor WHERE tipo='temperatura';

SELECT * FROM valor_do_sensor WHERE tempo BETWEEN '2020-01-01' AND '2021-09-09'