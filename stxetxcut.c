#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>

// to check inbox every 2 sec
//while (ls -l) do sleep 2; done

// Test alarm by following command
// cat|./a.out
// (echo -e  ; cat) | ./a.out
// to send 2nd STX, press ctrl+shift+u (underlined u is seen), then write 003

int file_counter=1;
FILE* frm = NULL;
int file_status=0;
char fname[200];
char file_data[10000];
int alarm_seconds=10;
int debug=0;
char inbox[200];

void current_date_time(char* buffer)
{
	time_t rawtime;
	struct tm * timeinfo;
	//char buffer [80];
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime(buffer,80,"%F:%T",timeinfo);
	//return buffer;
}

void filepath(char* prefix)
{
	char dt[200];
	current_date_time(dt);
	strcat(prefix,dt);
	//following to ensure that multiple files of same name are not created in one second
	sprintf(prefix,"%s:%02d",prefix,file_counter);
	if(file_counter>=99)
	{
		file_counter=1;
	}
	else
	{
		file_counter=file_counter+1;
	}
}

int start_alarm(void)
{
	struct itimerval old;
	struct itimerval new;
	
  new.it_interval.tv_sec = 0; 				//for repeat
  new.it_interval.tv_usec = 0; 
  new.it_value.tv_sec = alarm_seconds;		//first time
  new.it_value.tv_usec = 0;
   
  old.it_interval.tv_sec = 0;
  old.it_interval.tv_usec = 0;
  old.it_value.tv_sec = 0;
  old.it_value.tv_usec = 0;
   
  if (setitimer (ITIMER_REAL, &new, &old) < 0)
	{
      printf("timer init failed.exiting\n");
      exit(-1);
	}
  else
	{
		if(debug==1)
		{
			printf("timer for %d second is started\n",alarm_seconds);
		}
	}
  return EXIT_SUCCESS;
}

int stop_alarm(void)
{
	struct itimerval old;
	struct itimerval new;
	
  new.it_interval.tv_sec = 0; 		//for repeat
  new.it_interval.tv_usec = 0; 
  new.it_value.tv_sec = 00;			//first time
  new.it_value.tv_usec = 0;
   
  old.it_interval.tv_sec = 0;
  old.it_interval.tv_usec = 0;
  old.it_value.tv_sec = 0;
  old.it_value.tv_usec = 0;
   
  if (setitimer (ITIMER_REAL, &new, &old) < 0)
  {
      printf("timer init failed. Exiting\n");
      exit(-1);
  }
  else
  {
		if(debug==1)
		{
			printf("old timer stopped \n");
		}
  }
  return EXIT_SUCCESS;
}

void alarm_to_reset (int sig)
{
	if(debug==1)
	{
		printf("Alarm event. signal number is:%d\n",sig);
	}
	frm=fopen(fname,"w");
	if(frm != NULL)
	{ 
		fwrite(file_data,strlen(file_data),1,frm);
		fclose(frm);
		if(debug==1)
		{
			printf("file %s written\n",fname);
		}
	}
	else
	{
		printf("File %s can not be created. Exiting\n",fname);
		exit(-1);
	}
}

int main( int argc, char *argv[] )
{
	int c;
	int opt;
	strcpy(inbox,"/root/inbox/");

	
	//t = time
	//d = debug
	//
	while ((opt = getopt(argc, argv, "t:dp:h")) != -1) 
	{
		   switch (opt) 
		   {
			case 'd':
			   debug=1;
			   break;
			case 't':
			   alarm_seconds = atoi(optarg);
			   break;
			case 'p':
				strcpy(inbox,optarg);
				break;
			case 'h':
				printf("Usage: \n -d to print useful debug information \n -t <seconds, default 10> time for alarm \n -p <inbox folder>, default /root/inbox\n");
				exit(0);
		   }
	}

	if(debug==1)
	{
		printf("debug=%d, alarm_seconds=%d, inbox=%s\n",debug,alarm_seconds,inbox);
	}
	
	signal (SIGALRM, alarm_to_reset);	
	
	while((c = fgetc(stdin)) != EOF)
	{
		if(c==2)
		{
			if(debug==1)
			{
				printf("<STX> received\n");
			}
			stop_alarm();
			bzero(fname,200);
			bzero(file_data,10000);
			strcpy(fname,inbox);
			filepath(fname);
			start_alarm();
		}
		
		else if(c==3)
		{
			if(debug==1)
			{
				printf("<ETX> received\n");
			}
			//fwrite((char*)&c,1,1,frm);
			frm=fopen(fname,"w");
			if(frm != NULL)
			{ 
				fwrite(file_data,strlen(file_data),1,frm);
				fclose(frm);
				if(debug==1)
				{
					printf("file %s written\n",fname);
				}
			}
			else
			{
				printf("File %s can not be created. Exiting\n",fname);
				exit(-1);
			}
		}
		
		else
		{
			char small[5];
			small[0]=c;
			small[1]=0;
			strcat(file_data,small);	
		}
	}	
}
