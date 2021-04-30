/*******************************************************************************
   Create Tables
********************************************************************************/

CREATE TABLE Sala (
    id_sala INT NOT NULL,
    nome_da_sala VARCHAR(20) NOT NULL,
    id_mote INT NOT NULL,
    CONSTRAINT PK_Sala PRIMARY KEY (id_sala)
);

CREATE TABLE Mote (
    id_mote INT NOT NULL,   
    CONSTRAINT PK_Mote PRIMARY KEY (id_mote)
);

CREATE TABLE Sensor (
    id_sensor SERIAL NOT NULL,
    tipo VARCHAR(20) NOT NULL,
    id_mote INT NOT NULL,
    CONSTRAINT PK_Sensor PRIMARY KEY (id_sensor)
);

CREATE TABLE Valor_do_Sensor (
    id_valor SERIAL NOT NULL,
    valor_medido FLOAT(10),
    tempo TIMESTAMP,
    id_sensor SERIAL NOT NULL,
    CONSTRAINT PK_Valor_do_Sensor PRIMARY KEY (id_valor)
);

CREATE TABLE Regras (
    id_regras SERIAL NOT NULL,
    variavel VARCHAR(20),
    operacao VARCHAR(2),
    referencia NUMERIC(10,2),
    id_sensor SERIAL NOT NULL,
    id_sala SERIAL NOT NULL,
    id_atuador SERIAL NOT NULL,
    CONSTRAINT PK_Regras PRIMARY KEY (id_regras)

);

CREATE TABLE Atuador (
    id_atuador SERIAL NOT NULL,
    nome VARCHAR(20) NOT NULL,
    id_sala SERIAL NOT NULL,
    CONSTRAINT PK_Atuador PRIMARY KEY (id_atuador)
);

CREATE TABLE Estado_do_Atuador (
    id_estado SERIAL NOT NULL,
    estado BOOLEAN,
    tempo TIMESTAMP,
    id_atuador SERIAL NOT NULL,    
    CONSTRAINT PK_Estado_do_Atuador PRIMARY KEY (id_estado)
);

/*******************************************************************************
   Create Primary Key Unique Indexes
********************************************************************************/



/*******************************************************************************
   Create Foreign Keys
********************************************************************************/

ALTER TABLE Sala ADD CONSTRAINT FK_Sala_id_mote
    FOREIGN KEY (id_mote) REFERENCES Mote (id_mote) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE Sensor ADD CONSTRAINT FK_Sensor_id_mote
    FOREIGN KEY (id_mote) REFERENCES Mote (id_mote) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE Valor_do_Sensor ADD CONSTRAINT FK_ValorDoSensor_id_sensor
    FOREIGN KEY (id_sensor) REFERENCES Sensor (id_sensor) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE Regras ADD CONSTRAINT FK_Regra_id_sensor
    FOREIGN KEY (id_sensor) REFERENCES Sensor (id_sensor) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE Regras ADD CONSTRAINT FK_Regra_id_sala
    FOREIGN KEY (id_sala) REFERENCES Sala (id_sala) ON DELETE NO ACTION ON UPDATE NO ACTION;

ALTER TABLE Regras ADD CONSTRAINT FK_Regra_id_atuador
    FOREIGN KEY (id_atuador) REFERENCES Atuador (id_atuador) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE Atuador ADD CONSTRAINT FK_Atuador_id_sala
    FOREIGN KEY (id_sala) REFERENCES Sala (id_sala) ON DELETE NO ACTION ON UPDATE NO ACTION;



ALTER TABLE Estado_do_Atuador ADD CONSTRAINT FK_EstadoDoAtuador_id_atuador
    FOREIGN KEY (id_atuador) REFERENCES Atuador (id_atuador) ON DELETE NO ACTION ON UPDATE NO ACTION;
