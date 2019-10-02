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
	
  new.it_interval.tv_sec = 0; 		//for repeat
  new.it_interval.tv_usec = 0; 
  new.it_value.tv_sec = 10;			//first time
  new.it_value.tv_usec = 0;
   
  old.it_interval.tv_sec = 0;
  old.it_interval.tv_usec = 0;
  old.it_value.tv_sec = 0;
  old.it_value.tv_usec = 0;
   
  if (setitimer (ITIMER_REAL, &new, &old) < 0)
      printf("timer init failed\n");
  else
      printf("timer for 10 second is started\n");
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
      printf("timer init failed\n");
  else
      printf("old timer stopped \n");
  return EXIT_SUCCESS;
}

void alarm_to_reset (int sig)
{
	printf("Alarm event. signal number is:%d\n",sig);
	if (file_status==1)
	{
		fclose(frm);
		file_status=0;
	}
}

int main( int argc, char *argv[] )
{
	int c;
	char fname[200];

	signal (SIGALRM, alarm_to_reset);

	
	//FILE* frm;
	while((c = fgetc(stdin)) != EOF)
	{
		if(c==2)
		{
			printf("<STX> received\n");
			if(file_status==1)
			{
				fclose(frm);
				file_status=0;
			}
			stop_alarm();

			bzero(fname,200);
			strcpy(fname,"/root/inbox/");
			filepath(fname);
			frm=fopen(fname,"w");
			file_status=1;
			//printf("opening %s\n",fname);
			//fwrite((char*)&c,1,1,frm);
			//printf("%c\n",c);

			start_alarm();

		}
		
		else if(c==3 && file_status==1)
		{
			printf("<ETX> received\n");
			//fwrite((char*)&c,1,1,frm);
			fclose(frm);
			file_status=0;
		}
		
		else if(file_status==1)
		{
			fwrite((char*)&c,1,1,frm);
		}
	}	
}
