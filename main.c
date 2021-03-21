#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_MOTES 2

#define MAX_MSG_SIZE 75

#define VOLTAGE_SENSOR_ID 0
#define LIGHT_SENSOR_ID 1
#define CURRENT_SENSOR_ID 2
#define TEMPERATURE_SENSOR_ID 3
#define HUMIDITY_SENSOR_ID 4

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


int slope[NUMBER_OF_MOTES][5] = {1,1,1,1,1,  1,1,1,1,1};     //variable slope=1 <=> slope>0  and slope=0 <=> slope<0

FILE *sensor_data_channel;
FILE *matrix_channel;


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
   
    char hxmoteid[4];
    char hxvoltage[4];
    char hxlight[4];
    char hxcurrent[4];
    char hxtemperature[4];
    char hxhumidity[4];


    char* endPtr;

    int moteid;
    double voltage, light, current, temperature, humidity, power;


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

		    if(temperature>2.5 && slope[moteid-1][TEMPERATURE_SENSOR_ID]==1 ){
                negative_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color[0],ON);
		    }
            if(temperature<2 && slope[moteid-1][TEMPERATURE_SENSOR_ID]==0){
                positive_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color[0],OFF);
            }
        
		    if(humidity>=90 && slope[moteid-1][HUMIDITY_SENSOR_ID]==1 ){
                negative_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color[0],OFF);
		    }
            if(humidity<=70 && slope[moteid-1][HUMIDITY_SENSOR_ID]==0){
                positive_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color[0],ON);
            }
        
            if(light <= 1500){
			    strcpy(illumination_color[0],LIGHTON);
		    }
		
		    if(light > 1500){
			    strcpy(illumination_color[0],LIGHTOFF);
		    }
		
		    if(power >= 125){
			    strcpy(powersaver_color[0],ON);
		    }
		
		    if(power < 125){
			    strcpy(powersaver_color[0],OFF);
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

		    if(temperature>=23 && slope[moteid-1][TEMPERATURE_SENSOR_ID]==1 ){
                negative_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color[1],ON);
		    }
            if(temperature<=18 && slope[moteid-1][TEMPERATURE_SENSOR_ID]==0){
                positive_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color[1],OFF);
            }
        
		    if(humidity>=31 && slope[moteid-1][HUMIDITY_SENSOR_ID]==1 ){
                negative_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color[1],OFF);
		    }
            if(humidity<29 && slope[moteid-1][HUMIDITY_SENSOR_ID]==0){
                positive_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color[1],ON);
            }
        
            if(light <= 1200){
			    strcpy(illumination_color[1],LIGHTON);
		    }
		
		    if(light > 1200){
			    strcpy(illumination_color[1],LIGHTOFF);
		    }
		
		    if(power >= 160){
			    strcpy(powersaver_color[1],ON);
		    }
		
		    if(power < 160){
			    strcpy(powersaver_color[1],OFF);
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
	
	
		
    }
}