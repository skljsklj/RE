#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
int main(int argc, char *argv[])
{
	int fd = -1, ret;
	char *tmp1, tmp2[10], ch='t';
	char devname_head[50] =
	"/sys/devices/w1_bus_master1/28-00000";
	
	char devname_end[10] = "/w1_slave";
	char dev_name[100];
	long value;
	int integer, decimal;
	char buffer[100];
	int i,j;
	
	strcpy(dev_name, devname_head);
	strcat(dev_name, argv[1]);
	strcat(dev_name, devname_end);
	
if ((fd = open(dev_name, O_RDONLY)) < 0)
{
	perror("Greška pri otvaranju!");
	exit(1);
}
	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0)
{
	perror("Greška pri čitanju!");
	exit(1);
}
	tmp1 = strchr(buffer, ch);
	sscanf(tmp1, "t=%s", tmp2);
	value = atoi(tmp2);
	integer = value / 1000;
	decimal = value % 1000;
	
	printf("Temperatura je %d.%d\n", integer,decimal);
	
	close(fd);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
double temperatura(void) //očitavanje temperature
{
	FILE *ft;
	char tekst[100];
	ft=fopen("/sys/bus/w1/devices/28-00000???????/w1_slave","r");
	
	if(ft==NULL) return 0;
	int i=0;
	for(i=0;i<22;i++) //samo temperatura
	fscanf(ft,"%s", tekst);
	
	fclose(ft);
	
	//obrisati „t=”
	for(i=0;i<10;i++) tekst[i]=tekst[i+2];
	
	int temp=atoi(tekst); //prebaci u double
	double tem=(double)temp/1000;
	return tem;
};

void zapis(double temp) //log metod
{
	FILE * log;
	
	log=fopen("log","a+"); //otvoriti datoteku
	if(log==NULL) return;
	
	fprintf(log, "\n%.3f", temp);
	fclose(log);
};
int main(void)
{
	FILE * log;
	int cz=0, j=0;
	struct timespec ts1, ts2; //merenje vremena
	long czas;
	
	log=fopen("log","w");
	
	if(log==NULL) return 0;
	
	fprintf(log, "\n\t\t***Temperatura***\n
			Izmerena temperatura = %.3f \xC2\xB0 C\t C\n",
			temperatura());
	fclose(log);
	printf("\n\nPočetna Temp = %.3f \xC2\xB0 C", temperatura());
	while(1)
		{
			j++; //broj iteracija
			
			clock_gettime(CLOCK_REALTIME, &ts1);
			
			printf("\nMerenje br. %d.\tTemp = %.3f
			\xC2\xB0 C", j, temperatura());
			
			clock_gettime(CLOCK_REALTIME, &ts2);
			//trajanje pojedinacnog //merenja
			czas = (ts2.tv_nsec - ts1.tv_nsec)/1000000;
			
			if(czas<=0) czas=czas+999;
			
			/* Prethodni red koriguje povremenu gresku pri merenju
			kada se dobija rezultat koji je manji za 999ms u odnosu
			na stvarni */
			
			printf("\tTrajanje merenja: %ld ms.", czas);
			
			zapis(temperatura()); //upis u datoteku
		}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <wiringPi.h>

#define LED_PIN 0 // Broj pina na kojem je LED dioda spojena

int main(int argc, char *argv[])
{
	int fd = -1, ret;
	char *tmp1, tmp2[10], ch='t';
	char devname_head[50] = "/sys/devices/w1_bus_master1/28-00000";
	char devname_end[10] = "/w1_slave";
	char dev_name[100];
	long value;
	int integer, decimal;
	char buffer[100];
	int i,j;

	wiringPiSetup(); // Inicijalizacija wiringPi biblioteke
	pinMode(LED_PIN, OUTPUT); // Postavljanje pina na koji je LED spojena u modu izlaza

	while(1) // Beskonačna petlja za očitavanje temperature
	{
		strcpy(dev_name, devname_head);
		strcat(dev_name, argv[1]);
		strcat(dev_name, devname_end);

		if ((fd = open(dev_name, O_RDONLY)) < 0)
		{
			perror("Greška pri otvaranju!");
			exit(1);
		}
		ret = read(fd, buffer, sizeof(buffer));
		if (ret < 0)
		{
			perror("Greška pri čitanju!");
			exit(1);
		}
		tmp1 = strchr(buffer, ch);
		sscanf(tmp1, "t=%s", tmp2);
		value = atoi(tmp2);
		integer = value / 1000;
		decimal = value % 1000;

		printf("Temperatura je %d.%d\n", integer,decimal);

		if (integer >= 25) // Uključivanje LED diode ako je temperatura veća ili jednaka 25 stepeni Celzijusa
		{
			for (i = 0; i < 3; i++)
			{
				digitalWrite(LED_PIN, HIGH);
				delay(500);
				digitalWrite(LED_PIN, LOW);
				delay(500);
			}
		}
		else // Isključivanje LED diode ako je temperatura manja od 25 stepeni Celzijusa
		{
			digitalWrite(LED_PIN, LOW);
		}

		close(fd);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <wiringPi.h>

#define LED_PIN 0 // Broj pina na kojem je LED dioda spojena

int main(int argc, char *argv[])
{
	int fd = -1, ret;
	char *tmp1, tmp2[10], ch='t';
	char devname_head[50] = "/sys/devices/w1_bus_master1/28-00000";
	char devname_end[10] = "/w1_slave";
	char dev_name[100];
	long value;
	int integer, decimal;
	char buffer[100];
	int i,j;
	int alarm_threshold = 25; // Početna vrednost praga za temperaturni alarm

	wiringPiSetup(); // Inicijalizacija wiringPi biblioteke
	pinMode(LED_PIN, OUTPUT); // Postavljanje pina na koji je LED spojena u modu izlaza

	while(1) // Beskonačna petlja za očitavanje temperature
	{
		strcpy(dev_name, devname_head);
		strcat(dev_name, argv[1]);
		strcat(dev_name, devname_end);

		if ((fd = open(dev_name, O_RDONLY)) < 0)
		{
			perror("Greška pri otvaranju!");
			exit(1);
		}
		ret = read(fd, buffer, sizeof(buffer));
		if (ret < 0)
		{
			perror("Greška pri čitanju!");
			exit(1);
		}
		tmp1 = strchr(buffer, ch);
		sscanf(tmp1, "t=%s", tmp2);
		value = atoi(tmp2);
		integer = value / 1000;
		decimal = value % 1000;

		printf("Temperatura je %d.%d\n", integer,decimal);

		if (integer >= alarm_threshold) // Uključivanje LED diode ako je temperatura veća ili jednaka podešenom pragu alarma
		{
			for (i = 0; i < 3; i++)
			{
				digitalWrite(LED_PIN, HIGH);
				delay(500);
				digitalWrite(LED_PIN, LOW);
				delay(500);
			}
		}
		else // Isključivanje LED diode ako je temperatura manja od podešenog praga alarma
		{
			digitalWrite(LED_PIN, LOW);
		}

		close(fd);
	}

	return 0;
}
