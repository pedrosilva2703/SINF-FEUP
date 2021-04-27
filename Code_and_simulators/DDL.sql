CREATE TABLE Sala (
    id_da_sala INT PRIMARY KEY,   /*SERIAL*/
    nome_da_sala VARCHAR(20)
);

CREATE TABLE Mote (
    id_da_mote SERIAL PRIMARY KEY NOT NULL,   
);

CREATE TABLE Sensor (
    id_sensor INT PRIMARY KEY NOT NULL, 
    tipo VARCHAR(20),
                                  /*FK*/
);

CREATE TABLE Valor do Sensor (
    id_do_valor SERIAL PRIMARY KEY,
    valor_medido FLOAT(25),
    tempo FLOAT(25),
                                    /*FK*/
);

CREATE TABLE Regras (
    variavel VARCHAR(20) PRIMARY KEY,   /*SERIAL*/
    maximo FLOAT(10),
    minimo FLOAT(10),
                                    /*FK*/
);

CREATE TABLE Atuador (
    id_atuador INT PRIMARY KEY NOT NULL,   /*SERIAL*/
    nome VARCHAR(10),
                                    /*FK*/
);

CREATE TABLE Estado_do_Atuador (
    id_do_estado SERIAL PRIMARY KEY,   /*SERIAL*/
    estado BOOLEAN,
    tempo FLOAT(10),
                                    /*FK*/
);







ALTER TABLE Sensor ADD CONSTRAINT