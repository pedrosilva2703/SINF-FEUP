/*******************************************************************************
   Create Tables
********************************************************************************/

CREATE TABLE sala (
    id_sala INT NOT NULL,
    nome_da_sala VARCHAR(20) NOT NULL,
    id_mote INT NOT NULL,
    CONSTRAINT PK_sala PRIMARY KEY (id_sala)
);

CREATE TABLE mote (
    id_mote INT NOT NULL,   
    CONSTRAINT PK_mote PRIMARY KEY (id_mote)
);

CREATE TABLE sensor (
    id_sensor SERIAL NOT NULL,
    tipo VARCHAR(20) NOT NULL,
    id_mote INT NOT NULL,
    CONSTRAINT PK_sensor PRIMARY KEY (id_sensor)
);

CREATE TABLE valor_do_sensor (
    id_valor SERIAL NOT NULL,
    valor_medido FLOAT(10),
    tempo TIMESTAMP,
    id_sensor SERIAL NOT NULL,
    CONSTRAINT PK_valor_do_sensor PRIMARY KEY (id_valor)
);

CREATE TABLE regras (
    id_regras SERIAL NOT NULL,
    variavel VARCHAR(20),
    operacao VARCHAR(2),
    referencia NUMERIC(10,2),
    id_sensor SERIAL NOT NULL,
    id_sala SERIAL NOT NULL,
    id_atuador SERIAL NOT NULL,
    CONSTRAINT PK_regras PRIMARY KEY (id_regras)

);

CREATE TABLE atuador (
    id_atuador SERIAL NOT NULL,
    nome VARCHAR(20) NOT NULL,
    id_sala SERIAL NOT NULL,
    CONSTRAINT PK_atuador PRIMARY KEY (id_atuador)
);

CREATE TABLE estado_do_atuador (
    id_estado SERIAL NOT NULL,
    estado BOOLEAN,
    tempo TIMESTAMP,
    id_atuador SERIAL NOT NULL,    
    CONSTRAINT PK_estado_do_atuador PRIMARY KEY (id_estado)
);

/*******************************************************************************
   Create Primary Key Unique Indexes
********************************************************************************/



/*******************************************************************************
   Create Foreign Keys
********************************************************************************/

ALTER TABLE sala ADD CONSTRAINT FK_sala_id_mote
    FOREIGN KEY (id_mote) REFERENCES mote (id_mote) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE sensor ADD CONSTRAINT FK_sensor_id_mote
    FOREIGN KEY (id_mote) REFERENCES mote (id_mote) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE valor_do_sensor ADD CONSTRAINT FK_valordosensor_id_sensor
    FOREIGN KEY (id_sensor) REFERENCES sensor (id_sensor) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE regras ADD CONSTRAINT FK_regra_id_sensor
    FOREIGN KEY (id_sensor) REFERENCES sensor (id_sensor) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE regras ADD CONSTRAINT FK_regra_id_sala
    FOREIGN KEY (id_sala) REFERENCES sala (id_sala) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE regras ADD CONSTRAINT FK_regra_id_atuador
    FOREIGN KEY (id_atuador) REFERENCES atuador (id_atuador) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE atuador ADD CONSTRAINT FK_atuador_id_sala
    FOREIGN KEY (id_sala) REFERENCES sala (id_sala) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE estado_do_atuador ADD CONSTRAINT FK_estadodoatuador_id_atuador
FOREIGN KEY (id_atuador) REFERENCES Atuador (id_atuador) ON DELETE NO ACTION ON UPDATE NO ACTION;
