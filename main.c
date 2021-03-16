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


#define REFRIGERATOR_ON "[0,150,255]"


const char *channelRGBMatrix = "/tmp/ttyV10";
const char *channelMSGCreator = "/tmp/ttyV12";

char bytestream[MAX_MSG_SIZE];
char MsgConf[150];

char tempsensor_color[20];
char humsensor_color[20];
char lightsensor_color[20];
char powersensor_color[20];

char refrigerator_color[20];
char humidifier_color[20];
char illumination_color[20];
char powersaver_color[20];





int slope[5] = {1,1,1,1,1};     //variable slope=1 <=> slope>0  and slope=0 <=> slope<0


FILE *sensor_data_channel;
FILE *matrix_channel;

void negative_slope(int sensor_number){
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
    slope[sensor_number]=0;
}

void positive_slope(int sensor_number){
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
    slope[sensor_number]=1;  
}

int main(){
   

    char hxvoltage[4];
    char hxlight[4];
    char hxcurrent[4];
    char hxtemperature[4];
    char hxhumidity[4];

    char* endPtr;


    double voltage, light, current, temperature, humidity, power;
    
    strcpy(tempsensor_color,"");
	strcpy(humsensor_color,"");
	strcpy(lightsensor_color,"");
	strcpy(powersensor_color,"");
	
	strcpy(refrigerator_color,"");
	strcpy(humidifier_color,"");
	strcpy(illumination_color,"");
	strcpy(powersaver_color,"");
	
	
	strcpy(tempsensor_color,LOWTEMP);
	strcpy(humsensor_color,LOWHUM);
	strcpy(lightsensor_color,LOWLIGHT);
	strcpy(powersensor_color,LOWPOWER);
	
	strcpy(refrigerator_color,OFF);
	strcpy(humidifier_color,OFF);
	strcpy(illumination_color,LIGHTOFF);
	strcpy(powersaver_color,OFF);

    //******************** Set up initial configuration of MsgCreatorConf.txt *******************************//

    FILE *fp = fopen("MsgCreatorConf.txt","w");
    fprintf(fp,"-n 5 -l 100 -f 10 -c 1 -s [0,1,2,3,4] -d [['S',5,0.1,100],['C',500,2500,50],['C',0,50,1],['L',1.0,10.0,1],['L',50,100,1]] -i 1");
    fclose(fp);

    sensor_data_channel = fopen(channelMSGCreator, "r");

    
    
    matrix_channel=fopen(channelRGBMatrix,"w");
    
    fprintf(matrix_channel,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                            "["WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,  					WALL,      WALL,\
                               WALL,        FLOOR,      FLOOR,      			FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,\
                               WALL,        FLOOR,      tempsensor_color,       FLOOR,      FLOOR,      refrigerator_color, 	FLOOR,     WALL,\
                               WALL,        FLOOR,      humsensor_color,      	FLOOR,      FLOOR,      humidifier_color, 		FLOOR,     WALL,\
                               WALL,        FLOOR,      lightsensor_color,      FLOOR,      FLOOR,      illumination_color, 	FLOOR,     WALL,\
                               WALL,        FLOOR,      powersensor_color,    	FLOOR,      FLOOR,      powersaver_color, 		FLOOR,     WALL,\
                               WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,\
                               WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,   				WALL,      WALL"]\n");
                            
    fclose(matrix_channel);

    
    
    while(1){
        
        //****************** LER BYTE STREAM *****************//

        fgets(bytestream, MAX_MSG_SIZE, sensor_data_channel);
        puts(bytestream);

        for(int i=0; i<10; i++){
           if(i==0) strtok(bytestream, " ");
           else strtok(NULL, " "); 
        }

        //****************** CONVERTER DADOS DA BYTESTREAM *****************//

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
		
		//****************** ATUALIZAR GRÁFICAMENTE OS SENSORES NA MATRIZ *****************//
		
		if(temperature >= 4){
			strcpy(tempsensor_color,HIGHTEMP);
			}
			
		else{
			strcpy(tempsensor_color,LOWTEMP);
			}
		
		if(humidity >= 80){
			strcpy(humsensor_color,HIGHHUM);
			}
		else
		{strcpy(humsensor_color,LOWHUM);
			}
		
		if(light <= 1500){
			strcpy(lightsensor_color,LOWLIGHT);
		}
		
		else{
			strcpy(lightsensor_color,BRIGHT);
		}
		
		if(power >= 125){
			strcpy(powersensor_color,HIGHPOWER);
		}
		
		else{
			strcpy(powersensor_color,LOWPOWER);
		}
		
		
        
		//****************** VERIFICAR REGRAS E ALTERAR MsgCreatorConf.txt *****************//
		
        fp = fopen("MsgCreatorConf.txt","r");
        fgets( MsgConf, 150 , fp);
        fclose(fp);

		if(temperature>=5 && slope[TEMPERATURE_SENSOR_ID]==1 ){
            negative_slope(TEMPERATURE_SENSOR_ID);
            strcpy(refrigerator_color,ON);
		}
        if(temperature<=3 && slope[TEMPERATURE_SENSOR_ID]==0){
            positive_slope(TEMPERATURE_SENSOR_ID);
            strcpy(refrigerator_color,OFF);
        }
        
		if(humidity>=90 && slope[HUMIDITY_SENSOR_ID]==1 ){
            negative_slope(HUMIDITY_SENSOR_ID);
            strcpy(humidifier_color,OFF);
		}
        if(humidity<=70 && slope[HUMIDITY_SENSOR_ID]==0){
            positive_slope(HUMIDITY_SENSOR_ID);
            strcpy(humidifier_color,ON);
        }
        
        if(light <= 1500){
			strcpy(illumination_color,LIGHTON);
		}
		
		if(light > 1500){
			strcpy(illumination_color,LIGHTOFF);
		}
		
		if(power >= 125){
			strcpy(powersaver_color,ON);
		}
		
		if(power < 125){
			strcpy(powersaver_color,OFF);
		}
		
		
			
			
		

        puts(MsgConf);
        fp = fopen("MsgCreatorConf.txt", "w");
        fprintf(fp,"%s",MsgConf);
        fclose(fp);
		
		
		
		
		matrix_channel=fopen(channelRGBMatrix,"w");
		
		fprintf(matrix_channel,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
                            "["WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,  					WALL,      WALL,\
                               WALL,        FLOOR,      FLOOR,      			FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,\
                               WALL,        FLOOR,      tempsensor_color,       FLOOR,      FLOOR,      refrigerator_color, 	FLOOR,     WALL,\
                               WALL,        FLOOR,      humsensor_color,      	FLOOR,      FLOOR,      humidifier_color, 		FLOOR,     WALL,\
                               WALL,        FLOOR,      lightsensor_color,      FLOOR,      FLOOR,      illumination_color, 	FLOOR,     WALL,\
                               WALL,        FLOOR,      powersensor_color,    	FLOOR,      FLOOR,      powersaver_color, 		FLOOR,     WALL,\
                               WALL,        FLOOR,      FLOOR,    				FLOOR,      FLOOR,      FLOOR, 					FLOOR,     WALL,\
                               WALL,        WALL,       WALL,       			WALL,       WALL,       WALL,   				WALL,      WALL"]\n");
                            
		fclose(matrix_channel);

	

        
		
		
    }
}
