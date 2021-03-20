#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
char MsgConf[150];

char tempsensor_color_1[20];
char humsensor_color_1[20];
char lightsensor_color_1[20];
char powersensor_color_1[20];
char tempsensor_color_2[20];
char humsensor_color_2[20];
char lightsensor_color_2[20];
char powersensor_color_2[20];

char cooler_color_1[20];
char humidifier_color_1[20];
char illumination_color_1[20];
char powersaver_color_1[20];
char cooler_color_2[20];
char humidifier_color_2[20];
char illumination_color_2[20];
char powersaver_color_2[20];





int slope[2][5] = {1,1,1,1,1,  1,1,1,1,1};     //variable slope=1 <=> slope>0  and slope=0 <=> slope<0

FILE *sensor_data_channel;
FILE *matrix_channel;


void setup_sensors_actuators_colors(void){
    //Mote ID 1
    strcpy(tempsensor_color_1,"");
	strcpy(humsensor_color_1,"");
	strcpy(lightsensor_color_1,"");
	strcpy(powersensor_color_1,"");
	
	strcpy(cooler_color_1,"");
	strcpy(humidifier_color_1,"");
	strcpy(illumination_color_1,"");
	strcpy(powersaver_color_1,"");
	
	strcpy(tempsensor_color_1,LOWTEMP);
	strcpy(humsensor_color_1,LOWHUM);
	strcpy(lightsensor_color_1,LOWLIGHT);
	strcpy(powersensor_color_1,LOWPOWER);
	
	strcpy(cooler_color_1,OFF);
	strcpy(humidifier_color_1,OFF);
	strcpy(illumination_color_1,LIGHTOFF);
	strcpy(powersaver_color_1,OFF);

    //Mote ID 2
    strcpy(tempsensor_color_2,"");
	strcpy(humsensor_color_2,"");
	strcpy(lightsensor_color_2,"");
	strcpy(powersensor_color_2,"");
	
	strcpy(cooler_color_2,"");
	strcpy(humidifier_color_2,"");
	strcpy(illumination_color_2,"");
	strcpy(powersaver_color_2,"");
	
	strcpy(tempsensor_color_2,LOWTEMP);
	strcpy(humsensor_color_2,LOWHUM);
	strcpy(lightsensor_color_2,LOWLIGHT);
	strcpy(powersensor_color_2,LOWPOWER);
	
	strcpy(cooler_color_2,OFF);
	strcpy(humidifier_color_2,OFF);
	strcpy(illumination_color_2,LIGHTOFF);
	strcpy(powersaver_color_2,OFF);

}

void negative_slope(int sensor_number, int moteid){
    int i, j, cnt1=0, cnt2=0;
    for(i=0; i<strlen(MsgConf); i++){
        if( MsgConf[i] == '[') cnt1++;
        if( cnt1 == (2+1+sensor_number) && MsgConf[i] == ',' ) cnt2++;
        if( cnt2 == 3 ){
            cnt1=0;
            cnt2=0;
            break;
        }
    }
    
    for(j=strlen(MsgConf); j-1>i; j--){
        MsgConf[j]=MsgConf[j-1];
    }
    MsgConf[j] = '-';
    slope[moteid-1][sensor_number]=0;
}

void positive_slope(int sensor_number, int moteid){
    int i, j, cnt1=0, cnt2=0;
    for(i=0; i<strlen(MsgConf); i++){
        if( MsgConf[i] == '[') cnt1++;
        if( cnt1 == (2+1+sensor_number) && MsgConf[i] == ',' ) cnt2++;
        if( cnt2 == 3 ){
            cnt1=0;
            cnt2=0;
            break;
        }
    }
    
    for(j=i+1; j<strlen(MsgConf); j++){
        MsgConf[j]=MsgConf[j+1];
        puts(MsgConf);
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

    FILE *fp = fopen("MsgCreatorConf.txt","w");
    fprintf(fp,"-n 1 -l 100 -f 10 -c 1 -s [0,1,2,3,4] -d [['S',5,0.1,100],['C',500,2500,50],['C',0,50,1],['L',1.0,10.0,1],['L',50,100,1]] -i 1");
    fclose(fp);

    fp = fopen("MsgCreator2/MsgCreatorConf.txt","w");
    fprintf(fp,"-n 1 -l 100 -f 10 -c 1 -s [0,1,2,3,4] -d [['S',5,0.1,100],['C',500,2500,50],['C',0,50,1],['L',1.0,10.0,1],['L',50,100,1]] -i 2");
    fclose(fp);


    sensor_data_channel = fopen(channelMSGCreator, "r");
    
    
    setup_sensors_actuators_colors();
    
    matrix_channel=fopen(channelRGBMatrix,"w");
    
	fprintf(matrix_channel,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                        "["WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,  					WALL,      WALL,	WALL,    WALL, 	                  WALL,     WALL,      WALL,                    WALL,  	WALL,\
                           WALL,        FLOOR,      FLOOR,      			FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,    FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      tempsensor_color_1,     FLOOR,      FLOOR,      cooler_color_1, 	    FLOOR,     WALL,	FLOOR,   tempsensor_color_2,      FLOOR,    FLOOR,     cooler_color_2,          FLOOR, 	WALL,\
                           WALL,        FLOOR,      humsensor_color_1,      FLOOR,      FLOOR,      humidifier_color_1, 	FLOOR,     WALL,	FLOOR,   humsensor_color_2,       FLOOR,    FLOOR,     humidifier_color_2,      FLOOR, 	WALL,\
                           WALL,        FLOOR,      lightsensor_color_1,    FLOOR,      FLOOR,      illumination_color_1, 	FLOOR,     WALL,	FLOOR,   lightsensor_color_2,     FLOOR,    FLOOR,     illumination_color_2,    FLOOR, 	WALL,\
                           WALL,        FLOOR,      powersensor_color_1,    FLOOR,      FLOOR,      powersaver_color_1, 	FLOOR,     WALL,	FLOOR,   powersensor_color_2,     FLOOR,    FLOOR,     powersaver_color_2,      FLOOR, 	WALL,\
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
        //printf("%s\n", hxmoteid);

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

		    if(temperature >= 4){
			    strcpy(tempsensor_color_1,HIGHTEMP);
		    }
			
		    else{
			    strcpy(tempsensor_color_1,LOWTEMP);
		    }
		
		    if(humidity >= 80){
			    strcpy(humsensor_color_1,HIGHHUM);
			    }
		    else{
                strcpy(humsensor_color_1,LOWHUM);
		    }
		
		    if(light <= 1500){
			    strcpy(lightsensor_color_1,LOWLIGHT);
		    }
		
		    else{
			    strcpy(lightsensor_color_1,BRIGHT);
		    }
		
		    if(power >= 125){
			    strcpy(powersensor_color_1,HIGHPOWER);
		    }
		
		    else{
			    strcpy(powersensor_color_1,LOWPOWER);
		    }
		
		
        
		//--------------------------------------- VERIFICAR REGRAS, ALTERAR MsgCreatorConf.txt E SENSORES/ATUADORES DA RGBMATRIX ----------------------------------//
		
            fp = fopen("MsgCreatorConf.txt","r");
            fgets( MsgConf, 150 , fp);
            fclose(fp);

		    if(temperature>=5 && slope[moteid-1][TEMPERATURE_SENSOR_ID]==1 ){
                negative_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color_1,ON);
		    }
            if(temperature<=3 && slope[moteid-1][TEMPERATURE_SENSOR_ID]==0){
                positive_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color_1,OFF);
            }
        
		    if(humidity>=90 && slope[moteid-1][HUMIDITY_SENSOR_ID]==1 ){
                negative_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color_1,OFF);
		    }
            if(humidity<=70 && slope[moteid-1][HUMIDITY_SENSOR_ID]==0){
                positive_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color_1,ON);
            }
        
            if(light <= 1500){
			    strcpy(illumination_color_1,LIGHTON);
		    }
		
		    if(light > 1500){
			    strcpy(illumination_color_1,LIGHTOFF);
		    }
		
		    if(power >= 125){
			    strcpy(powersaver_color_1,ON);
		    }
		
		    if(power < 125){
			    strcpy(powersaver_color_1,OFF);
		    }	
		

            //puts(MsgConf);
            fp = fopen("MsgCreatorConf.txt", "w");
            fprintf(fp,"%s",MsgConf);
            fclose(fp);

        }
       


        //-----------------------------------------------------------------------------------------------------------------------------------//
		//-------------------------------------------------------------- MOTE 2 ------------------------------------------------------------//
		
        if(moteid==2){
            printf("ESTAMOS NA 2\n");
		    if(temperature >= 20){
			    strcpy(tempsensor_color_2,HIGHTEMP);
		    }
			
		    else{
			    strcpy(tempsensor_color_2,LOWTEMP);
		    }
		
		    if(humidity >= 80){
			    strcpy(humsensor_color_2,HIGHHUM);
			    }
		    else{
                strcpy(humsensor_color_2,LOWHUM);
		    }
		
		    if(light <= 1200){
			    strcpy(lightsensor_color_2,LOWLIGHT);
		    }
		
		    else{
			    strcpy(lightsensor_color_2,BRIGHT);
		    }
		
		    if(power >= 160){
			    strcpy(powersensor_color_2,HIGHPOWER);
		    }
		
		    else{
			    strcpy(powersensor_color_2,LOWPOWER);
		    }
		
		
        
		//--------------------------------------- VERIFICAR REGRAS, ALTERAR MsgCreatorConf.txt E SENSORES/ATUADORES DA RGBMATRIX ----------------------------------//
		
            fp = fopen("MsgCreator2/MsgCreatorConf.txt","r");
            fgets( MsgConf, 150 , fp);
            fclose(fp);

		    if(temperature>=18 && slope[moteid-1][TEMPERATURE_SENSOR_ID]==1 ){
                negative_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color_2,ON);
		    }
            if(temperature<=23 && slope[moteid-1][TEMPERATURE_SENSOR_ID]==0){
                positive_slope(TEMPERATURE_SENSOR_ID, moteid);
                strcpy(cooler_color_2,OFF);
            }
        
		    if(humidity>=30 && slope[moteid-1][HUMIDITY_SENSOR_ID]==1 ){
                negative_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color_2,OFF);
		    }
            if(humidity<=30 && slope[moteid-1][HUMIDITY_SENSOR_ID]==0){
                positive_slope(HUMIDITY_SENSOR_ID, moteid);
                strcpy(humidifier_color_2,ON);
            }
        
            if(light <= 1200){
			    strcpy(illumination_color_2,LIGHTON);
		    }
		
		    if(light > 1200){
			    strcpy(illumination_color_2,LIGHTOFF);
		    }
		
		    if(power >= 160){
			    strcpy(powersaver_color_2,ON);
		    }
		
		    if(power < 160){
			    strcpy(powersaver_color_2,OFF);
		    }	
		

            //puts(MsgConf);
            fp = fopen("MsgCreator2/MsgCreatorConf.txt", "w");
            fprintf(fp,"%s",MsgConf);
            fclose(fp);

        }
		
		
		//--------------------------------- ATUALIZAR RGBMATRIX -------------------------------------//

        matrix_channel=fopen(channelRGBMatrix,"w");
    
	    fprintf(matrix_channel,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                        "["WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,  					WALL,      WALL,	WALL,    WALL, 	                  WALL,     WALL,      WALL,                    WALL,  	WALL,\
                           WALL,        FLOOR,      FLOOR,      			FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,    FLOOR,   FLOOR,                   FLOOR,    FLOOR,     FLOOR,                   FLOOR, 	WALL,\
                           WALL,        FLOOR,      tempsensor_color_1,     FLOOR,      FLOOR,      cooler_color_1, 	    FLOOR,     WALL,	FLOOR,   tempsensor_color_2,      FLOOR,    FLOOR,     cooler_color_2,          FLOOR, 	WALL,\
                           WALL,        FLOOR,      humsensor_color_1,      FLOOR,      FLOOR,      humidifier_color_1, 	FLOOR,     WALL,	FLOOR,   humsensor_color_2,       FLOOR,    FLOOR,     humidifier_color_2,      FLOOR, 	WALL,\
                           WALL,        FLOOR,      lightsensor_color_1,    FLOOR,      FLOOR,      illumination_color_1, 	FLOOR,     WALL,	FLOOR,   lightsensor_color_2,     FLOOR,    FLOOR,     illumination_color_2,    FLOOR, 	WALL,\
                           WALL,        FLOOR,      powersensor_color_1,    FLOOR,      FLOOR,      powersaver_color_1, 	FLOOR,     WALL,	FLOOR,   powersensor_color_2,     FLOOR,    FLOOR,     powersaver_color_2,      FLOOR, 	WALL,\
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
