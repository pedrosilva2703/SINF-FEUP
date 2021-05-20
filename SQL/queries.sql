/****************Queries para o 3º sprint ***************/

/*
sinf2021a34
DuOHMMxK
*/

/*  Ex 1  */

SET search_path TO dba34;


/* Ex 1 VERSAO FINAL */
SET search_path TO dba34;
SELECT tempo AS timestamp, valor_medido AS measurement
FROM valor_do_sensor
 JOIN sensor ON sensor.id_sensor=valor_do_sensor.id_sensor
 JOIN sala ON sala.id_mote=sensor.id_mote
WHERE sensor.tipo = 'temperatura' AND sala.nome_da_sala = 'armazenamento' AND tempo BETWEEN '2021-05-14 13:30:44' AND '2021-05-14 13:35:44'

/* Ex 2 VERSAO TROLHA */
/*
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


SET search_path TO dba34;
SELECT *
FROM estado_do_atuador, 
WHERE estado_do_atuador.id_estado IN
    (SELECT estado_do_atuador.id_atuador, MAX(estado_do_atuador.id_estado) as id_ultimos
    FROM estado_do_atuador
    GROUP BY estado_do_atuador.id_atuador
    ORDER BY estado_do_atuador.id_atuador ASC)

SET search_path TO dba34;
SELECT *
FROM estado_do_atuador, (SELECT estado_do_atuador.id_atuador, MAX(estado_do_atuador.id_estado) as id_ultimos
    FROM estado_do_atuador
    GROUP BY estado_do_atuador.id_atuador
    ORDER BY estado_do_atuador.id_atuador ASC) 


SET search_path TO dba34;

SELECT original.id_atuador,original.estado
FROM estado_do_atuador AS original, (SELECT estado_do_atuador.id_atuador AS id_ultimos_atuadores , MAX(estado_do_atuador.id_estado) AS id_ultimos_estados
                                        FROM estado_do_atuador
                                        GROUP BY estado_do_atuador.id_atuador
                                        ORDER BY estado_do_atuador.id_atuador ASC) AS ultimos
WHERE original.id_estado=ultimos.id_ultimos_estados
*/



/* EX 2 VERSAO FINAL */

SET search_path TO dba34;

SELECT atuador.nome AS Actuator, sala.nome_da_sala AS Cell, t1.estado AS State
FROM ( 
    SELECT original.id_atuador AS id_atuador, original.estado AS estado
    FROM estado_do_atuador AS original, (SELECT estado_do_atuador.id_atuador AS id_ultimos_atuadores , MAX(estado_do_atuador.id_estado) AS id_ultimos_estados
                                            FROM estado_do_atuador
                                            GROUP BY estado_do_atuador.id_atuador
                                            ORDER BY estado_do_atuador.id_atuador ASC
                                        ) AS ultimos
    WHERE original.id_estado=ultimos.id_ultimos_estados
    ) AS t1
 JOIN atuador ON atuador.id_atuador=t1.id_atuador
 JOIN sala ON sala.id_sala=atuador.id_sala



/* EX 4 */
UPDATE regras SET regras.referencia=28 WHERE regras.id_regras=9;
SELECT 






