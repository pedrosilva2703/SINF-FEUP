#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MSG_SIZE 75

#define VOLTAGE_SENSOR_ID 0
#define LIGHT_SENSOR_ID 1
#define CURRENT_SENSOR_ID 2
#define TEMPERATURE_SENSOR_ID 3
#define HUMIDITY_SENSOR_ID 4

const char *channelMSGCreator = "/tmp/ttyV12";

char bytestream[MAX_MSG_SIZE];
char MsgConf[150];

int slope[5] = {1,1,1,1,1};     //variable slope=1 <=> slope>0  and slope=0 <=> slope<0


FILE *sensor_data_channel;

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

    char* aux;

    double voltage, light, current, temperature, humidity;

    //Set up initial configuration of MsgCreatorConf.txt
    FILE *fp = fopen("MsgCreatorConf.txt","w");
    fprintf(fp,"-n 5 -l 100 -f 10 -c 1 -s [0,1,2,3,4] -d [['S',5,0.1,100],['C',500,2500,50],['U',0,50,1],['L',1.0,10.0,1],['L',50,100,1]] -i 1");
    fclose(fp);

    sensor_data_channel = fopen(channelMSGCreator, "r");
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
      
		
		//****************** VERIFICAR REGRAS E ALTERAR MsgCreatorConf.txt *****************//
		
        fp = fopen("MsgCreatorConf.txt","r");
        fgets( MsgConf, 150 , fp);
        fclose(fp);

		if(temperature>=5 && slope[TEMPERATURE_SENSOR_ID]==1 ){
            negative_slope(TEMPERATURE_SENSOR_ID);
		}
        if(temperature<=0 && slope[TEMPERATURE_SENSOR_ID]==0){
            positive_slope(TEMPERATURE_SENSOR_ID);
        }
        
		if(humidity>=90 && slope[HUMIDITY_SENSOR_ID]==1 ){
            negative_slope(HUMIDITY_SENSOR_ID);
		}
        if(humidity<=70 && slope[HUMIDITY_SENSOR_ID]==0){
            positive_slope(HUMIDITY_SENSOR_ID);
        }

        puts(MsgConf);
        fp = fopen("MsgCreatorConf.txt", "w");
        fprintf(fp,"%s",MsgConf);
        fclose(fp);
		
		

        
		
		
    }
}
