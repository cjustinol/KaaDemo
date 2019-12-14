#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

void sensorTmp007Convert(uint16_t rawAmbTemp,uint16_t rawObjTemp,
						float *tAmb,float *tObj)
{
	const float SCALE_LSB = 0.03125;
	float t;
	int it;
	it = (int)((rawObjTemp) >> 2);
	t = ((float)(it)) * SCALE_LSB;
	*tObj = t;
	it = (int)((rawAmbTemp) >> 2);
	t = (float)it;
	*tAmb = t * SCALE_LSB;
}


void reorderingValues( char all[4][2], uint16_t *object, uint16_t *enviroment)
{
	char obj_str[7]={'\0'};
	char env_str[7]={'\0'};
	int x;

	obj_str[0] = '0';
	obj_str[1] = 'x';
	obj_str[2] = all[1][0];
	obj_str[3] = all[1][1];
	obj_str[4] = all[0][0];
	obj_str[5] = all[0][1];

	env_str[0] = '0';
	env_str[1] = 'x';
	env_str[2] = all[3][0];
	env_str[3] = all[3][1];
	env_str[4] = all[2][0];
	env_str[5] = all[2][1];
	
	sscanf(obj_str, "%" SCNx16, object);
	sscanf(env_str, "%" SCNx16, enviroment);
}

int getTemperatures()
{
	FILE* fp;
	char* pch;
	char path[1035], ta[4][2], command[100];
	uint16_t object, enviroment;
	int k = 0;
	float a,b;
	static float tmp[2];

	//printf("MAC: %s\n",mac);
	strncpy(command,"gatttool -b ",100);
	strcat(command,"54:6C:0E:78:F0:83");
	strcat(command," --char-write-req -a 0x0027 -n 01");	

	popen(command, "r");
	sleep(2);
	
	strncpy(command,"gatttool -b ",100);
        strcat(command,"54:6C:0E:78:F0:83");
	strcat(command," --char-read -a 0x24");

	fp = popen(command, "r");
	
	if (fp == NULL) 
	{
		printf("Failed to run command\n" );
		exit(1);
	}
	/*Read the output a line at a time - output it.*/

	while (fgets(path, sizeof(path)-1, fp) != NULL) 
	{    
		strtok (path,":");
		pch = strtok (NULL,":");
		//printf("pch: %s\n",pch);
		while (pch != NULL && k<4)
		{	
			if (k==0)
				pch = strtok (pch, " ");
			else
				pch = strtok (NULL," ");
				
			strcpy(ta[k],pch);
            //printf("VA: %s\n",pch);
			//printf("k: %d\n",k);
			k++;
		}
	}
	
	if(k!=0)
	{	
		reorderingValues(ta,&object,&enviroment);
		sensorTmp007Convert(object, enviroment, &tmp[0], &tmp[1]);
		printf("objeto = %f, ambiente= %f\n", tmp[0], tmp[1]);
	}
	pclose(fp);
	return (int)tmp[1];
}

