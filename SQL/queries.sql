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


/* Ex 1 */
SELECT tempo AS timestamp, valor_medido AS measurement
FROM valor_do_sensor
 JOIN sensor ON sensor.id_sensor=valor_do_sensor.id_sensor
 JOIN sala ON sala.id_mote=sensor.id_mote
WHERE sensor.tipo = 'temperatura' AND sala.nome_da_sala = 'armazenamento' AND tempo BETWEEN '2021-05-14 13:30:44' AND '2021-05-14 13:35:44'

/* Ex 2 */

(SELECT atuador.nome AS actuator, sala.nome_da_sala AS cell, estado_do_atuador.estado AS state
FROM estado_do_atuador
 JOIN atuador ON atuador.id_atuador=estado_do_atuador.id_atuador
 JOIN sala ON sala.id_sala=atuador.id_sala
WHERE sala.nome_da_sala = 'frigorifico'
ORDER BY estado_do_atuador.tempo DESC LIMIT 4)
UNION ALL
(SELECT atuador.nome AS actuator, sala.nome_da_sala AS cell, estado_do_atuador.estado AS state
FROM estado_do_atuador
 JOIN atuador ON atuador.id_atuador=estado_do_atuador.id_atuador
 JOIN sala ON sala.id_sala=atuador.id_sala
WHERE sala.nome_da_sala = 'armazenamento' 
ORDER BY estado_do_atuador.tempo DESC LIMIT 4)

