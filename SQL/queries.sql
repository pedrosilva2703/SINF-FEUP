/****************Queries para o 3º sprint ***************/

/*
sinf2021a34
DuOHMMxK
SET search_path TO dba34;
*/

/************************************ EX 1 VERSAO FINAL ************************************/
SET search_path TO dba34;
SELECT tempo AS timestamp, valor_medido AS measurement
FROM valor_do_sensor
 JOIN sensor ON sensor.id_sensor=valor_do_sensor.id_sensor
 JOIN sala ON sala.id_mote=sensor.id_mote
WHERE sensor.tipo = 'temperatura' AND sala.nome_da_sala = 'armazenamento' AND tempo BETWEEN '2021-05-14 13:30:44' AND '2021-05-14 13:35:44'


/************************************ EX 2 VERSAO FINAL ************************************/

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



/************************************ EX 3 VERSAO FINAL ************************************/

SET search_path TO dba34;
UPDATE sala
SET id_mote= CASE
    WHEN sala.nome_da_sala='armazenamento' THEN (SELECT id_mote FROM sala WHERE sala.nome_da_sala<>'armazenamento')
    WHEN sala.nome_da_sala='frigorifico' THEN (SELECT id_mote FROM sala WHERE sala.nome_da_sala<>'frigorifico')
    END
RETURNING  sala.id_mote AS Sensor,  sala.nome_da_sala AS Cell


/*SET search_path TO dba34;
UPDATE sala
SET id_mote= CASE
    WHEN sala.nome_da_sala='armazenamento' THEN (SELECT id_mote FROM sala WHERE sala.nome_da_sala<>'armazenamento')
    WHEN sala.nome_da_sala='frigorifico' THEN (SELECT id_mote FROM sala WHERE sala.nome_da_sala<>'frigorifico')
    END
RETURNING
 (SELECT sensor.tipo FROM sensor WHERE sensor.id_mote=s.id_mote) AS Sensor,
 (SELECT sala.nome_da_sala FROM sala WHERE sala.id_mote=s.id_mote) AS Cell

(SELECT sensor.tipo AS Sensor, sala.nome_da_sala AS Cell
 FROM sensor
 JOIN sala ON sala.id_mote=sensor.id_mote
)*/
/*
SET search_path TO dba34;
UPDATE sala AS s
SET id_mote= CASE
    WHEN s.nome_da_sala='armazenamento' THEN (SELECT id_mote FROM sala WHERE sala.nome_da_sala<>'armazenamento')
    WHEN s.nome_da_sala='frigorifico' THEN (SELECT id_mote FROM sala WHERE sala.nome_da_sala<>'frigorifico')
    END
RETURNING
 (SELECT sensor.tipo FROM sensor WHERE sensor.id_mote=s.id_mote) AS Sensor,
 (SELECT sala.nome_da_sala FROM sala WHERE sala.id_mote=s.id_mote) AS Cell*/

/************************************ EX 4 VERSAO FINAL ************************************/
SET search_path TO dba34;

UPDATE regras AS r
SET referencia=29
    WHERE r.id_regras IN  (SELECT t1.c1 FROM (
                                SELECT regras.id_regras AS c1, sala.nome_da_sala AS c2, regras.referencia AS c3, sensor.tipo AS c4
                                FROM regras
                                    JOIN sensor ON regras.id_sensor=sensor.id_sensor
                                    JOIN sala ON sala.id_mote=sensor.id_mote
                                WHERE regras.operacao = '>=' AND
                                    sensor.tipo = 'temperatura' AND  sala.nome_da_sala = 'armazenamento'                                
                            ) AS t1 
                        )
RETURNING   r.id_regras AS Rule, 
            (SELECT sala.nome_da_sala FROM sala WHERE sala.id_sala=r.id_sala) AS Cell,
            r.referencia AS ReferenceValue,
            (SELECT sensor.tipo FROM sensor WHERE sensor.id_sensor=r.id_sensor) AS Sensor


/************************************ EX 5 VERSAO FINAL ************************************/


SET search_path TO dba34;
SELECT tempo AS timestamp, ((220*3600/1000)*valor_do_sensor.valor_medido) AS Energy
FROM valor_do_sensor
 JOIN sensor ON sensor.id_sensor=valor_do_sensor.id_sensor 
 JOIN sala ON sala.id_mote=sensor.id_mote
WHERE sensor.tipo='corrente' AND sala.nome_da_sala = 'armazenamento' AND tempo BETWEEN '2021-05-14 13:30:44' AND '2021-05-14 13:35:44'
ORDER BY tempo ASC


