#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <postgresql/libpq-fe.h>

#define SALA_1 "frigorifico"
#define SALA_2 "armazenamento"

#define NUMBER_OF_MOTES 2

#define MAX_MSG_SIZE 75

#define VOLTAGE_SENSOR_ID 0
#define LIGHT_SENSOR_ID 1
#define CURRENT_SENSOR_ID 2
#define TEMPERATURE_SENSOR_ID 3
#define HUMIDITY_SENSOR_ID 4

#define COOLER_ID 1
#define HUMIDIFIER_ID 2
#define ILLUMINATION_ID 3
#define POWERSAVER_ID 4

#define WALL      "[0,0,0]"
#define FLOOR     "[200,200,200]"
#define ON        "[0,255,0]"
#define OFF       "[255,0,0]"
#define LIGHTON   "[255,255,0]"
#define LIGHTOFF  "[105,105,105]"

#define LOWTEMP   "[135,206,250]"
#define HIGHTEMP  "[255,80,40]"
#define HIGHHUM   "[0,0,205]"
#define LOWHUM    "[176,224,230]"
#define LOWLIGHT  "[0,0,0]"
#define BRIGHT    "[255,255,255]"
#define LOWPOWER  "[255,165,0]"
#define HIGHPOWER "[255,0,0]"

char sala[NUMBER_OF_MOTES][MAX_MSG_SIZE];


const char *channelRGBMatrix = "/tmp/ttyV10";
const char *channelMSGCreator = "/tmp/ttyV12";

char bytestream[MAX_MSG_SIZE];
char MsgConf[NUMBER_OF_MOTES][150]={""};

char tempsensor_color[NUMBER_OF_MOTES][20];
char humsensor_color[NUMBER_OF_MOTES][20];
char lightsensor_color[NUMBER_OF_MOTES][20];
char powersensor_color[NUMBER_OF_MOTES][20];

char cooler_color[NUMBER_OF_MOTES][20];
char humidifier_color[NUMBER_OF_MOTES][20];
char illumination_color[NUMBER_OF_MOTES][20];
char powersaver_color[NUMBER_OF_MOTES][20];


int cooler_state[NUMBER_OF_MOTES]={0,0};
int humidifier_state[NUMBER_OF_MOTES]={0,0};
int illumination_state[NUMBER_OF_MOTES]={0,0};
int powersaver_state[NUMBER_OF_MOTES]={0,0};


float tempmax[NUMBER_OF_MOTES], tempmin[NUMBER_OF_MOTES];
float hummax[NUMBER_OF_MOTES], hummin[NUMBER_OF_MOTES]; 
float lightmax[NUMBER_OF_MOTES], lightmin[NUMBER_OF_MOTES]; 
float powmax[NUMBER_OF_MOTES], powmin[NUMBER_OF_MOTES];

int slope[NUMBER_OF_MOTES][5] = {1,1,1,1,1,  1,1,1,1,1};     //variable slope=1 <=> slope>0  and slope=0 <=> slope<0

FILE *sensor_data_channel;
FILE *matrix_channel;


PGconn *conn;
PGresult *res;

const char *dbconn;

char buffer[250];

void cria_tabelas(){

    //Criar tabelas
    PQexec(conn, "CREATE TABLE IF NOT EXISTS sala             (id_sala INT NOT NULL, nome_da_sala VARCHAR(20) NOT NULL, id_mote INT NOT NULL, CONSTRAINT PK_Sala PRIMARY KEY (id_sala))");
    PQexec(conn, "CREATE TABLE IF NOT EXISTS mote             (id_mote INT NOT NULL, CONSTRAINT PK_Mote PRIMARY KEY (id_mote))");
    PQexec(conn, "CREATE TABLE IF NOT EXISTS sensor           (id_sensor INT NOT NULL, tipo VARCHAR(20) NOT NULL, id_mote INT NOT NULL, CONSTRAINT PK_sensor PRIMARY KEY (id_sensor))");
    PQexec(conn, "CREATE TABLE IF NOT EXISTS valor_do_sensor  (id_valor SERIAL NOT NULL, valor_medido FLOAT(10), tempo TIMESTAMP, id_sensor INT NOT NULL, CONSTRAINT PK_valor_do_sensor PRIMARY KEY (id_valor))");
    PQexec(conn, "CREATE TABLE IF NOT EXISTS regras           (id_regras INT NOT NULL, variavel VARCHAR(20),operacao VARCHAR(2), referencia NUMERIC(10,2), id_sensor INT NOT NULL, id_sala INT NOT NULL, id_atuador INT NOT NULL, CONSTRAINT PK_regras PRIMARY KEY (id_regras))");
    PQexec(conn, "CREATE TABLE IF NOT EXISTS atuador          (id_atuador INT NOT NULL, nome VARCHAR(20) NOT NULL, id_sala INT NOT NULL, CONSTRAINT PK_atuador PRIMARY KEY (id_atuador))");
    PQexec(conn, "CREATE TABLE IF NOT EXISTS estado_do_atuador(id_estado SERIAL NOT NULL, estado BOOLEAN, tempo TIMESTAMP, id_atuador INT NOT NULL, CONSTRAINT PK_estado_do_atuador PRIMARY KEY (id_estado))");

    //Foreign keys
    PQexec(conn, "ALTER TABLE sala ADD CONSTRAINT FK_sala_id_mote FOREIGN KEY (id_mote) REFERENCES mote (id_mote) ON DELETE NO ACTION ON UPDATE NO ACTION");
    PQexec(conn, "ALTER TABLE sensor ADD CONSTRAINT FK_sensor_id_mote FOREIGN KEY (id_mote) REFERENCES mote (id_mote) ON DELETE NO ACTION ON UPDATE NO ACTION");
    PQexec(conn, "ALTER TABLE valor_do_sensor ADD CONSTRAINT FK_valordosensor_id_sensor FOREIGN KEY (id_sensor) REFERENCES sensor (id_sensor) ON DELETE NO ACTION ON UPDATE NO ACTION");
    PQexec(conn, "ALTER TABLE regras ADD CONSTRAINT FK_regra_id_sensor FOREIGN KEY (id_sensor) REFERENCES sensor (id_sensor) ON DELETE NO ACTION ON UPDATE NO ACTION");
    PQexec(conn, "ALTER TABLE regras ADD CONSTRAINT FK_regra_id_sala FOREIGN KEY (id_sala) REFERENCES sala (id_sala) ON DELETE NO ACTION ON UPDATE NO ACTION");
    PQexec(conn, "ALTER TABLE regras ADD CONSTRAINT FK_regra_id_atuador FOREIGN KEY (id_atuador) REFERENCES atuador (id_atuador) ON DELETE NO ACTION ON UPDATE NO ACTION");
    PQexec(conn, "ALTER TABLE atuador ADD CONSTRAINT FK_atuador_id_sala FOREIGN KEY (id_sala) REFERENCES sala (id_sala) ON DELETE NO ACTION ON UPDATE NO ACTION");
    PQexec(conn, "ALTER TABLE estado_do_atuador ADD CONSTRAINT FK_estadodoatuador_id_atuador FOREIGN KEY (id_atuador) REFERENCES Atuador (id_atuador) ON DELETE NO ACTION ON UPDATE NO ACTION");

}

void setup_sensors_actuators_colors(void){
    
    for(int i=0; i<2; i++){
        strcpy(tempsensor_color[i],"");
	    strcpy(humsensor_color[i],"");
	    strcpy(lightsensor_color[i],"");
	    strcpy(powersensor_color[i],"");
	
	    strcpy(cooler_color[i],"");
	    strcpy(humidifier_color[i],"");
	    strcpy(illumination_color[i],"");
	    strcpy(powersaver_color[i],"");
	
	    strcpy(tempsensor_color[i],LOWTEMP);
	    strcpy(humsensor_color[i],LOWHUM);
	    strcpy(lightsensor_color[i],LOWLIGHT);
	    strcpy(powersensor_color[i],LOWPOWER);
	
	    strcpy(cooler_color[i],OFF);
	    strcpy(humidifier_color[i],OFF);
	    strcpy(illumination_color[i],LIGHTOFF);
	    strcpy(powersaver_color[i],OFF);
    }
}

void negative_slope(int sensor_number, int moteid){
    int i, j, cnt1=0, cnt2=0;
    for(i=0; i<strlen(MsgConf[moteid-1]); i++){
        if( MsgConf[moteid-1][i] == '[') cnt1++;
        if( cnt1 == (2+1+sensor_number) && MsgConf[moteid-1][i] == ',' ) cnt2++;
        if( cnt2 == 3 ){
            cnt1=0;
            cnt2=0;
            break;
        }
    }
    
    for(j=strlen(MsgConf[moteid-1]); j-1>i; j--){
        MsgConf[moteid-1][j]=MsgConf[moteid-1][j-1];
    }
    MsgConf[moteid-1][j] = '-';
    slope[moteid-1][sensor_number]=0;
}

void positive_slope(int sensor_number, int moteid){
    int i, j, cnt1=0, cnt2=0;
    for(i=0; i<strlen(MsgConf[moteid-1]); i++){
        if( MsgConf[moteid-1][i] == '[') cnt1++;
        if( cnt1 == (2+1+sensor_number) && MsgConf[moteid-1][i] == ',' ) cnt2++;
        if( cnt2 == 3 ){
            cnt1=0;
            cnt2=0;
            break;
        }
    }
    
    for(j=i+1; j<strlen(MsgConf[moteid-1]); j++){
        MsgConf[moteid-1][j]=MsgConf[moteid-1][j+1];
    }  
    slope[moteid-1][sensor_number]=1;  
}

int main(){
    float tempmax[NUMBER_OF_MOTES], tempmin[NUMBER_OF_MOTES];
    float hummax[NUMBER_OF_MOTES], hummin[NUMBER_OF_MOTES]; 
    float lightmax[NUMBER_OF_MOTES], lightmin[NUMBER_OF_MOTES]; 
    float powmax[NUMBER_OF_MOTES], powmin[NUMBER_OF_MOTES];
    
    tempmax[0]= 2.5;    tempmin[0]= 2;
    hummax[0]= 90;      hummin[0]=70;
    lightmax[0]=1500;   lightmin[0]=1500;
    powmax[0]=125;      powmin[0]=125;
    
    tempmax[1]= 23;    tempmin[1]= 18;
    hummax[1]= 30;      hummin[1]=30;
    lightmax[1]=1200;   lightmin[1]=1200;
    powmax[1]=160;      powmin[1]=160;

    char hxmoteid[4];
    char hxvoltage[4];
    char hxlight[4];
    char hxcurrent[4];
    char hxtemperature[4];
    char hxhumidity[4];


    char* endPtr;

    int moteid;
    double voltage, light, current, temperature, humidity, power;

    //******************** Set up Database connection *******************************//


    dbconn = "host = 'db.fe.up.pt' dbname = 'sinf2021a34' user = '---' password = '---'";
    conn = PQconnectdb(dbconn);

    
	
	if (!conn){
		//printf(stderr, "libpq error: PQconnectdb returned NULL. \n\n");
		PQfinish(conn);
		exit(1);
	}
	
	else if (PQstatus(conn) != CONNECTION_OK){
		//printf(stderr, "Connection to DB failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}

	else {
		printf("Connection OK \n");
		PQexec(conn,"SET search_path TO dba34,public");

        cria_tabelas();
        
        strcpy(sala[0], SALA_1);
        strcpy(sala[1], SALA_2);
      
        //Insere configuracoes (ids das motes, respetivas salas e respetivos sensores e atuadores)

        for(int i=0; i<NUMBER_OF_MOTES; i++){
            sprintf(buffer, "INSERT INTO mote VALUES (%d)", i+1);
            res=PQexec(conn, buffer);

            sprintf(buffer, "INSERT INTO sala VALUES (%d, '%s', %d)",i+1, sala[i], i+1);
            res=PQexec(conn, buffer);

            sprintf(buffer,"INSERT INTO sensor VALUES (%d, 'tensao', %d)", VOLTAGE_SENSOR_ID+1+5*i, i+1 );
            res=PQexec(conn, buffer);
            sprintf(buffer,"INSERT INTO sensor VALUES (%d, 'luminosidade', %d)", LIGHT_SENSOR_ID+1+5*i, i+1 );
            res=PQexec(conn, buffer);
            sprintf(buffer,"INSERT INTO sensor VALUES (%d, 'corrente', %d)", CURRENT_SENSOR_ID+1+5*i, i+1 );
            res=PQexec(conn, buffer);
            sprintf(buffer,"INSERT INTO sensor VALUES (%d, 'temperatura', %d)", TEMPERATURE_SENSOR_ID+1+5*i, i+1 );
            res=PQexec(conn, buffer);
            sprintf(buffer,"INSERT INTO sensor VALUES (%d, 'humidade', %d)", HUMIDITY_SENSOR_ID+1+5*i, i+1 );
            res=PQexec(conn, buffer);


            sprintf(buffer,"INSERT INTO atuador VALUES (%d, 'refrigerador', %d)", COOLER_ID+4*i, i+1 );
            res=PQexec(conn, buffer);
            sprintf(buffer,"INSERT INTO atuador VALUES (%d, 'humidificador', %d)", HUMIDIFIER_ID+4*i, i+1 );
            res=PQexec(conn, buffer);
            sprintf(buffer,"INSERT INTO atuador VALUES (%d, 'iluminacao', %d)", ILLUMINATION_ID+4*i, i+1 );
            res=PQexec(conn, buffer);
            sprintf(buffer,"INSERT INTO atuador VALUES (%d, 'powersaver', %d)", POWERSAVER_ID+4*i, i+1 );
            res=PQexec(conn, buffer);

        }


        //Verifica regras existentes
        for(int i=0; i<NUMBER_OF_MOTES; i++){
            
            //TEMPERATURA MAXIMA
            sprintf(buffer, "SELECT referencia FROM regras WHERE id_regras=%d", 1+(i*8) );
            res=PQexec(conn, buffer );
        
            if( PQgetisnull(res,0,0) ){
                sprintf(buffer,"INSERT INTO regras VALUES (%d, 'temperatura', '>=', %f, %d, %d, %d )", 1+(i*8), tempmax[i], TEMPERATURE_SENSOR_ID+1+(i*5), i+1, COOLER_ID+(4*i) );
                res=PQexec(conn, buffer);
            }           
            else{
                sprintf(buffer, "%s", PQgetvalue(res,0,0) );
                tempmax[i] = atof(buffer);
            }

            //TEMPERATURA MINIMA
            sprintf(buffer, "SELECT referencia FROM regras WHERE id_regras=%d", 2+(i*8) );
            res=PQexec(conn, buffer );

            if( PQgetisnull(res,0,0) ){
                sprintf(buffer,"INSERT INTO regras VALUES (%d, 'temperatura', '<', %f, %d, %d, %d )", 2+(i*8), tempmin[i], TEMPERATURE_SENSOR_ID+1+(i*5), i+1, COOLER_ID+(4*i) );
                res=PQexec(conn, buffer);
            }           
            else{
                sprintf(buffer, "%s", PQgetvalue(res,0,0) );
                tempmin[i] = atof(buffer);
            }


            //HUMIDADE MAXIMA
            sprintf(buffer, "SELECT referencia FROM regras WHERE id_regras=%d", 3+(i*8) );
            res=PQexec(conn, buffer );
        
            if( PQgetisnull(res,0,0) ){
                sprintf(buffer,"INSERT INTO regras VALUES (%d, 'humidade', '>=', %f, %d, %d, %d )", 3+(i*8), hummax[i], HUMIDITY_SENSOR_ID+1+(i*5), i+1, HUMIDIFIER_ID+(4*i) );
                res=PQexec(conn, buffer);
            }           
            else{
                sprintf(buffer, "%s", PQgetvalue(res,0,0) );
                hummax[i] = atof(buffer);
            }

            //HUMIDADE MINIMA
            sprintf(buffer, "SELECT referencia FROM regras WHERE id_regras=%d", 4+(i*8) );
            res=PQexec(conn, buffer );

            if( PQgetisnull(res,0,0) ){
                sprintf(buffer,"INSERT INTO regras VALUES (%d, 'humidade', '<', %f, %d, %d, %d )", 4+(i*8), hummin[i], HUMIDITY_SENSOR_ID+1+(i*5), i+1, HUMIDIFIER_ID+(4*i) );
                res=PQexec(conn, buffer);
            }           
            else{
                sprintf(buffer, "%s", PQgetvalue(res,0,0) );
                hummin[i] = atof(buffer);
            }


            //LUMINOSIDADE MAXIMA
            sprintf(buffer, "SELECT referencia FROM regras WHERE id_regras=%d", 5+(i*8) );
            res=PQexec(conn, buffer );
        
            if( PQgetisnull(res,0,0) ){
                sprintf(buffer,"INSERT INTO regras VALUES (%d, 'luminosidade', '>=', %f, %d, %d, %d )", 5+(i*8), lightmax[i], LIGHT_SENSOR_ID+1+(i*5), i+1, ILLUMINATION_ID+(4*i) );
                res=PQexec(conn, buffer);
            }           
            else{
                sprintf(buffer, "%s", PQgetvalue(res,0,0) );
                lightmax[i] = atof(buffer);
            }

            //LUMINOSIDADE MINIMA
            sprintf(buffer, "SELECT referencia FROM regras WHERE id_regras=%d", 6+(i*8) );
            res=PQexec(conn, buffer );

            if( PQgetisnull(res,0,0) ){
                sprintf(buffer,"INSERT INTO regras VALUES (%d, 'luminosidade', '<', %f, %d, %d, %d )", 6+(i*8), lightmin[i], LIGHT_SENSOR_ID+1+(i*5), i+1, ILLUMINATION_ID+(4*i) );
                res=PQexec(conn, buffer);
            }           
            else{
                sprintf(buffer, "%s", PQgetvalue(res,0,0) );
                lightmin[i] = atof(buffer);
            }


            //POTENCIA MAXIMA
            sprintf(buffer, "SELECT referencia FROM regras WHERE id_regras=%d", 7+(i*8) );
            res=PQexec(conn, buffer );
        
            if( PQgetisnull(res,0,0) ){
                sprintf(buffer,"INSERT INTO regras VALUES (%d, 'potencia', '>=', %f, %d, %d, %d )", 7+(i*8), powmax[i], VOLTAGE_SENSOR_ID+1+(i*5), i+1, POWERSAVER_ID+(4*i) );
                res=PQexec(conn, buffer);
            }           
            else{
                sprintf(buffer, "%s", PQgetvalue(res,0,0) );
                powmax[i] = atof(buffer);
            }

            //POTENCIA MINIMA
            sprintf(buffer, "SELECT referencia FROM regras WHERE id_regras=%d", 8+(i*8) );
            res=PQexec(conn, buffer );

            if( PQgetisnull(res,0,0) ){
                sprintf(buffer,"INSERT INTO regras VALUES (%d, 'potencia', '<', %f, %d, %d, %d )", 8+(i*8), powmin[i], VOLTAGE_SENSOR_ID+1+(i*5), i+1, POWERSAVER_ID+(4*i) );
                res=PQexec(conn, buffer);
            }           
            else{
                sprintf(buffer, "%s", PQgetvalue(res,0,0) );
                powmin[i] = atof(buffer);
            }




        }

	}




    //******************** Set up initial configuration of MsgCreatorConf.txt *******************************//

    FILE *fp1 = fopen("MsgCreatorConf.txt","w");
    fprintf(fp1,"-n 1 -l 100 -f 2 -c 1 -s [0,1,2,3,4] -d [['S',5,0.1,100],['C',500,2500,50],['C',0,50,1],['L',10.0,20.0,1],['L',50,100,1]] -i 1");
    fclose(fp1);

    FILE *fp2 = fopen("MsgCreator2/MsgCreatorConf.txt","w");
    fprintf(fp2,"-n 1 -l 100 -f 2 -c 1 -s [0,1,2,3,4] -d [['S',5,0.1,100],['C',500,2500,50],['C',0,50,1],['L',15.0,30.0,1],['L',20,40,1]] -i 2");
    fclose(fp2);


    sensor_data_channel = fopen(channelMSGCreator, "r");
    
    
    setup_sensors_actuators_colors();
    
    matrix_channel=fopen(channelRGBMatrix,"w");
    
	fprintf(matrix_channel,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                        "["WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,  					WALL,      WALL,	WALL,    WALL, 	                  WALL,     WALL,      WALL,                    WALL,  	WALL,\
                           WALL,        FLOOR,      FLOOR,      			FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,    FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      tempsensor_color[0],    FLOOR,      FLOOR,      cooler_color[0], 	    FLOOR,     WALL,	FLOOR,   tempsensor_color[1],     FLOOR,    FLOOR,     cooler_color[1],         FLOOR, 	WALL,\
                           WALL,        FLOOR,      humsensor_color[0],     FLOOR,      FLOOR,      humidifier_color[0], 	FLOOR,     WALL,	FLOOR,   humsensor_color[1],      FLOOR,    FLOOR,     humidifier_color[1],     FLOOR, 	WALL,\
                           WALL,        FLOOR,      lightsensor_color[0],   FLOOR,      FLOOR,      illumination_color[0], 	FLOOR,     WALL,	FLOOR,   lightsensor_color[1],    FLOOR,    FLOOR,     illumination_color[1],   FLOOR, 	WALL,\
                           WALL,        FLOOR,      powersensor_color[0],   FLOOR,      FLOOR,      powersaver_color[0], 	FLOOR,     WALL,	FLOOR,   powersensor_color[1],    FLOOR,    FLOOR,     powersaver_color[1],     FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,   				WALL,      WALL,	WALL,    WALL,                    WALL,     WALL,      WALL,                    WALL, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,   				WALL,      WALL,	WALL,    WALL,                    WALL,     WALL,      WALL,                    WALL, 	WALL"]\n");
                        
	fclose(matrix_channel);

    
    
    while(1){
        
        //------------------------------------- LER BYTE STREAM -------------------------//

        fgets(bytestream, MAX_MSG_SIZE, sensor_data_channel);
        puts(bytestream);

        for(int i=0; i<9; i++){
           if(i==0) strtok(bytestream, " ");
           else if(i==5) sprintf(hxmoteid, "%s", strcat( strtok(NULL, " ") , strtok(NULL, " ")  ) );
           else strtok(NULL, " "); 
        }

        //---------------------------------- CONVERTER DADOS DA BYTESTREAM -------------------------//
        
        
        moteid = (int)strtol(hxmoteid, &endPtr, 16);
        printf("MoteID: %d\n", moteid);

        sprintf(hxvoltage, "%s", strcat( strtok(NULL, " ") , strtok(NULL, " ")  ) );
        voltage = 2*1.5* (float)strtol(hxvoltage, &endPtr, 16) / 4096;
        printf("Voltage: %f V\n", voltage);

        sprintf(hxlight, "%s", strcat( strtok(NULL, " ") , strtok(NULL, " ")  ) );
        light = ( 0.625 * 1000000 * ( (float)strtol(hxlight, &endPtr, 16) / 4096) * 1.5 ) / 100000 * 1000;
        printf("Light: %f lux\n", light);

        sprintf(hxcurrent, "%s", strcat( strtok(NULL, " ") , strtok(NULL, " ")  ) );
        current = 0.769 * 100000 * ( ( (float)strtol(hxcurrent, &endPtr, 16) / 4096) * 1.5 / 100000 ) * 1000;
        printf("Current: %f A\n", current);

        sprintf(hxtemperature, "%s", strcat( strtok(NULL, " ") , strtok(NULL, " ")  ) );
        temperature = -39.6 + 0.01 * (float)strtol(hxtemperature, &endPtr, 16);
        printf("Temperature: %f C\n", temperature);
        
        sprintf(hxhumidity, "%s", strcat( strtok(NULL, " ") , strtok(NULL, " ")  ) );
        humidity = -2.0468 + 0.0367 * (float)strtol(hxhumidity, &endPtr, 16) + (-1.5955 * 0.000001 * (float)strtol(hxhumidity, &endPtr, 16) * (float)strtol(hxhumidity, &endPtr, 16) );
        printf("Relative humidity: %f %%\n", humidity);
      
		power = voltage * current;

		//-------------------------------------------------------------- MOTE 1 ------------------------------//
        


        if(moteid==1){

		    if(temperature >= 5){
			    strcpy(tempsensor_color[0],HIGHTEMP);
		    }
			
		    else{
			    strcpy(tempsensor_color[0],LOWTEMP);
		    }
		
		    if(humidity >= 80){
			    strcpy(humsensor_color[0],HIGHHUM);
			    }
		    else{
                strcpy(humsensor_color[0],LOWHUM);
		    }
		
		    if(light <= 1500){
			    strcpy(lightsensor_color[0],LOWLIGHT);
		    }
		
		    else{
			    strcpy(lightsensor_color[0],BRIGHT);
		    }
		
		    if(power >= 125){
			    strcpy(powersensor_color[0],HIGHPOWER);
		    }
		
		    else{
			    strcpy(powersensor_color[0],LOWPOWER);
		    }
		
		
        
		//--------------------------------------- VERIFICAR REGRAS, ALTERAR MsgCreatorConf.txt E SENSORES/ATUADORES DA RGBMATRIX ----------------------------------//
		
            fp1 = fopen("MsgCreatorConf.txt","r");
            fgets( MsgConf[moteid-1], 150 , fp1);
            fclose(fp1);

		    if(temperature>tempmax[moteid-1] && slope[moteid-1][TEMPERATURE_SENSOR_ID]==1 ){
                negative_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color[0],ON);
                cooler_state[moteid-1]=1;
		    }
            if(temperature<tempmin[moteid-1] && slope[moteid-1][TEMPERATURE_SENSOR_ID]==0){
                positive_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color[0],OFF);
                cooler_state[moteid-1]=0;
            }
        
		    if(humidity>=hummax[moteid-1] && slope[moteid-1][HUMIDITY_SENSOR_ID]==1 ){
                negative_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color[0],OFF);
                humidifier_state[moteid-1]=0;
		    }
            if(humidity<=hummin[moteid-1] && slope[moteid-1][HUMIDITY_SENSOR_ID]==0){
                positive_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color[0],ON);
                humidifier_state[moteid-1]=1;
            }
        
            if(light <= lightmin[moteid-1]){
			    strcpy(illumination_color[0],LIGHTON);
                illumination_state[moteid-1]=1;
		    }
		
		    if(light > lightmax[moteid-1]){
			    strcpy(illumination_color[0],LIGHTOFF);
                illumination_state[moteid-1]=0;
		    }
		
		    if(power >= powmax[moteid-1]){
			    strcpy(powersaver_color[0],ON);
                powersaver_state[moteid-1]=1;
		    }
		
		    if(power < powmin[moteid-1]){
			    strcpy(powersaver_color[0],OFF);
                powersaver_state[moteid-1]=0;
		    }	
		

    
            fp1 = fopen("MsgCreatorConf.txt", "w");
            fprintf(fp1,"%s",MsgConf[moteid-1]);
            fclose(fp1);

        }
       


        //-----------------------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------- MOTE 2 ------------------------------------------------------------//
		
        if(moteid==2){
           
		    if(temperature >= 20){
			    strcpy(tempsensor_color[1],HIGHTEMP);
		    }
			
		    else{
			    strcpy(tempsensor_color[1],LOWTEMP);
		    }
		
		    if(humidity >= 80){
			    strcpy(humsensor_color[1],HIGHHUM);
			    }
		    else{
                strcpy(humsensor_color[1],LOWHUM);
		    }
		
		    if(light <= 1200){
			    strcpy(lightsensor_color[1],LOWLIGHT);
		    }
		
		    else{
			    strcpy(lightsensor_color[1],BRIGHT);
		    }
		
		    if(power >= 160){
			    strcpy(powersensor_color[1],HIGHPOWER);
		    }
		
		    else{
			    strcpy(powersensor_color[1],LOWPOWER);
		    }
		
		
        
		//--------------------------------------- VERIFICAR REGRAS, ALTERAR MsgCreatorConf.txt E SENSORES/ATUADORES DA RGBMATRIX ----------------------------------//
		
            fp2 = fopen("MsgCreator2/MsgCreatorConf.txt","r");
            fgets( MsgConf[moteid-1], 150 , fp2);
            fclose(fp2);

		    if(temperature>=tempmax[moteid-1] && slope[moteid-1][TEMPERATURE_SENSOR_ID]==1 ){
                negative_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color[1],ON);
                cooler_state[moteid-1]=1;
		    }
            if(temperature<tempmin[moteid-1] && slope[moteid-1][TEMPERATURE_SENSOR_ID]==0){
                positive_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color[1],OFF);
                cooler_state[moteid-1]=0;
            }
        
		    if(humidity>=hummax[moteid-1] && slope[moteid-1][HUMIDITY_SENSOR_ID]==1 ){
                negative_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color[1],OFF);
                humidifier_state[moteid-1]=0;
		    }
            if(humidity<hummin[moteid-1] && slope[moteid-1][HUMIDITY_SENSOR_ID]==0){
                positive_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color[1],ON);
                humidifier_state[moteid-1]=1;
            }

		    if(light <= lightmin[moteid-1]){
			    strcpy(illumination_color[1],LIGHTON);
                illumination_state[moteid-1]=1;
		    }
		
		    if(light > lightmax[moteid-1]){
			    strcpy(illumination_color[1],LIGHTOFF);
                illumination_state[moteid-1]=0;
		    }
		
		    if(power >= powmax[moteid-1]){
			    strcpy(powersaver_color[1],ON);
                powersaver_state[moteid-1]=1;
		    }
		
		    if(power < powmin[moteid-1]){
			    strcpy(powersaver_color[1],OFF);
                powersaver_state[moteid-1]=0;
		    }
      
            fp2 = fopen("MsgCreator2/MsgCreatorConf.txt", "w");
            fprintf(fp2,"%s",MsgConf[moteid-1]);
            fclose(fp2);
            
        }
		

        strcpy(MsgConf[moteid-1],"");

		//--------------------------------- ATUALIZAR RGBMATRIX -------------------------------------//

        matrix_channel=fopen(channelRGBMatrix,"w");
    
	    fprintf(matrix_channel,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                        "["WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,  					WALL,      WALL,	WALL,    WALL, 	                  WALL,     WALL,      WALL,                    WALL,  	WALL,\
                           WALL,        FLOOR,      FLOOR,      			FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,    FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      tempsensor_color[0],    FLOOR,      FLOOR,      cooler_color[0], 	    FLOOR,     WALL,	FLOOR,   tempsensor_color[1],     FLOOR,    FLOOR,     cooler_color[1],          FLOOR, 	WALL,\
                           WALL,        FLOOR,      humsensor_color[0],     FLOOR,      FLOOR,      humidifier_color[0], 	FLOOR,     WALL,	FLOOR,   humsensor_color[1],      FLOOR,    FLOOR,     humidifier_color[1],      FLOOR, 	WALL,\
                           WALL,        FLOOR,      lightsensor_color[0],   FLOOR,      FLOOR,      illumination_color[0], 	FLOOR,     WALL,	FLOOR,   lightsensor_color[1],    FLOOR,    FLOOR,     illumination_color[1],    FLOOR, 	WALL,\
                           WALL,        FLOOR,      powersensor_color[0],   FLOOR,      FLOOR,      powersaver_color[0], 	FLOOR,     WALL,	FLOOR,   powersensor_color[1],    FLOOR,    FLOOR,     powersaver_color[1],      FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,   				WALL,      WALL,	WALL,    WALL,                    WALL,     WALL,      WALL,                    WALL, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,	FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,   				WALL,      WALL,	WALL,    WALL,                    WALL,     WALL,      WALL,                    WALL, 	WALL"]\n");
                        
	    fclose(matrix_channel);
	

        //--------------------------------- ATUALIZAR VALORES NA BASE DE DADOS -------------------------------------//
        

        //ATUALIZA VALORES DOS SENSORES

        sprintf(buffer,"INSERT INTO valor_do_sensor VALUES (DEFAULT, %f, CURRENT_TIMESTAMP, %d)", voltage, VOLTAGE_SENSOR_ID+1+5*(moteid-1) );
        res=PQexec(conn, buffer);

        sprintf(buffer,"INSERT INTO valor_do_sensor VALUES (DEFAULT, %f, CURRENT_TIMESTAMP, %d)", light, LIGHT_SENSOR_ID+1+5*(moteid-1) );
        res=PQexec(conn, buffer);

        sprintf(buffer,"INSERT INTO valor_do_sensor VALUES (DEFAULT, %f, CURRENT_TIMESTAMP, %d)", current, CURRENT_SENSOR_ID+1+5*(moteid-1) );
        res=PQexec(conn, buffer);

        sprintf(buffer,"INSERT INTO valor_do_sensor VALUES (DEFAULT, %f, CURRENT_TIMESTAMP, %d)", temperature, TEMPERATURE_SENSOR_ID+1+5*(moteid-1) );
        res=PQexec(conn, buffer);

        sprintf(buffer,"INSERT INTO valor_do_sensor VALUES (DEFAULT, %f, CURRENT_TIMESTAMP, %d)", humidity, HUMIDITY_SENSOR_ID+1+5*(moteid-1) );
        res=PQexec(conn, buffer);
       
        //ATUALIZA ESTADO DOS ATUADORES

        if(cooler_state[moteid-1]==1){
            sprintf(buffer,"INSERT INTO estado_do_atuador VALUES (DEFAULT, TRUE, CURRENT_TIMESTAMP, %d)", COOLER_ID+4*(moteid-1) );
        }
        else if(cooler_state[moteid-1]==0){
            sprintf(buffer,"INSERT INTO estado_do_atuador VALUES (DEFAULT, FALSE, CURRENT_TIMESTAMP, %d)", COOLER_ID+4*(moteid-1) );
        }
        res=PQexec(conn, buffer);

        if(humidifier_state[moteid-1]==1){
            sprintf(buffer,"INSERT INTO estado_do_atuador VALUES (DEFAULT, TRUE, CURRENT_TIMESTAMP, %d)", HUMIDIFIER_ID+4*(moteid-1) );
        }
        else if(humidifier_state[moteid-1]==0){
            sprintf(buffer,"INSERT INTO estado_do_atuador VALUES (DEFAULT, FALSE, CURRENT_TIMESTAMP, %d)", HUMIDIFIER_ID+4*(moteid-1) );
        }
        res=PQexec(conn, buffer);
        
        if(illumination_state[moteid-1]==1){
            sprintf(buffer,"INSERT INTO estado_do_atuador VALUES (DEFAULT, TRUE, CURRENT_TIMESTAMP, %d)", ILLUMINATION_ID+4*(moteid-1) );
        }
        else if(illumination_state[moteid-1]==0){
            sprintf(buffer,"INSERT INTO estado_do_atuador VALUES (DEFAULT, FALSE, CURRENT_TIMESTAMP, %d)", ILLUMINATION_ID+4*(moteid-1) );
        }
        res=PQexec(conn, buffer);

        if(powersaver_state[moteid-1]==1){
            sprintf(buffer,"INSERT INTO estado_do_atuador VALUES (DEFAULT, TRUE, CURRENT_TIMESTAMP, %d)", POWERSAVER_ID+4*(moteid-1) );
        }
        else if(powersaver_state[moteid-1]==0){
            sprintf(buffer,"INSERT INTO estado_do_atuador VALUES (DEFAULT, FALSE, CURRENT_TIMESTAMP, %d)", POWERSAVER_ID+4*(moteid-1) );
        }
        res=PQexec(conn, buffer);
		
    }
}
