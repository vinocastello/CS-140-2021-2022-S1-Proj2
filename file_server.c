#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 12

pthread_t *tid;

typedef struct arguments
{   
    char command[50];
    char path[50];
    char string[50];
}ARGS;

int get_random()
{
    // Returns a pseudo-random integer
    // range is [0,100)
    
	int r = rand()%100;
	return r;
}

void random_sleep(int op)
{   
    int random = get_random();
    if(op == 1) // sleep for 1 or 6 seconds
    {   
        if(random < 20)
        {
            sleep(6);
        }
        else
        {
            sleep(1);
        }
    }
    else if(op == 0) // random delay between 7 to 10 seconds
    {
        if(random < 25)
        {
            sleep(7);
        }
        else if(random >= 25 && random < 50)
        {
            sleep(8);
        }
        else if(random >= 50 && random < 75)
        {
            sleep(9);
        }
        else if(random >= 75)
        {
            sleep(10);
        }
    }
    
}

void readfile(ARGS *arguments)
{   
    FILE *f = fopen(arguments->path,"r");;
    //Error exit if there was a problem opening the above file 
    if (f == NULL) { 
        fprintf(stderr, "Error opening file %s \n", arguments->path); 
        return; 
    } 
 
    //Seek to the end of the file and error exit if fseek() indicates an error condition 
    if (fseek(f, 0, SEEK_END) == -1) { 
	    fprintf(stderr, "Error seeking in file %s", arguments->path); 
        return;	 
    } 
 
    //Get the position of the end of the file 
    int file_size = ftell(f); 
    //Reset the file position back to the beginning of the file 
    rewind(f); 
    //Allocate a character buffer the size of the file plus one more byte for the null terminator 
    char *buf = malloc(file_size + 1); 

    //Error exit if malloc couldn't allocate memory for the buffer 
    if (buf == NULL) { 
        fprintf(stderr, "Error allocating memory for file %s \n", arguments->path); 
        return; 
    } 

    if (fread((void *) buf, file_size, 1, f) <= 0) {
        if(strcmp(arguments->command,"read") == 0)
        {   
            FILE *output = fopen("read.txt","a");
            fprintf(output,"%s %s:\tFILE DNE\n",arguments->command,arguments->path);
            fclose(output);
        }
        else if(strcmp(arguments->command,"empty") == 0)
        {
            FILE *output = fopen("empty.txt","a");
            fprintf(output,"%s %s:\tFILE ALREADY EMPTY\n",arguments->command,arguments->path);
            fclose(output);
        } 
        free(buf); 
        return; 
    } 

    //Set the null terminator at the end of the buffer 
    buf[file_size] = '\0'; 
    if(strcmp(arguments->command,"read") == 0)
    {   
        FILE *output = fopen("read.txt","a");
        fprintf(output,"%s %s:\t%s\n",arguments->command,arguments->path,buf);
        fclose(output);
    }
    else if(strcmp(arguments->command,"empty") == 0)
    {
        FILE *output = fopen("empty.txt","a");
        fprintf(output,"%s %s:\t%s\n",arguments->command,arguments->path,buf);
        fclose(output);
    }

    //Cleanup
	free(buf); 
    fclose(f);
}

void worker_write(ARGS *arguments)
{   
    ARGS *args = (ARGS *)arguments;
    random_sleep(1);
    FILE *fp_write = fopen(args->path,"a");
    fprintf(fp_write,"%s",args->string);
    usleep((25*strlen(args->string))*1000); // multiply to 1000 to get 25 ms
    fclose(fp_write);
    free(args);
    pthread_exit(NULL);
}

void worker_read(void *arguments)
{   
    ARGS *args = (ARGS *)arguments;
    random_sleep(1);
    if(access(args->path,F_OK) == 0)
    {   
        readfile(args);
    }
    else
    {   
        FILE *output = fopen("read.txt","a");
        fprintf(output,"%s %s:\tFILE DNE\n",args->command,args->path);
        fclose(output);
    }

    free(args);
    pthread_exit(NULL);
}

void worker_empty(void *arguments)
{   
    ARGS *args = (ARGS *)arguments;
    random_sleep(1);
    if(access(args->path,F_OK) == 0)
    {   
        readfile(args);
        FILE *fp = fopen(args->path,"w");
        fclose(fp);
        random_sleep(0);
    }
    else
    {   
        FILE *output = fopen("empty.txt","a");
        fprintf(output,"%s %s:\tFILE ALREADY EMPTY\n",args->command,args->path);
        fclose(output);
    }

    free(args);
    pthread_exit(NULL);
}

void save_command(char *buf)
{
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    FILE *commands = fopen("commands.txt","a");
    fprintf(commands,"%02d/%02d/%04d %02d:%02d:%02d %s\n",tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,tm.tm_hour, tm.tm_min, tm.tm_sec,buf);
    fclose(commands);
}

int main()
{   
    srand(time(0));
    tid = malloc(sizeof(pthread_t)*NUM_THREADS);
    int thread_counter = 0;
    void *fs[] = {worker_write, worker_read, worker_empty};
    char buf[150];
    printf("$ ");
    scanf(" %[^\n]s",buf);
    save_command(buf);
    while(1)
    {   
        if(strcmp(buf,"exit") == 0)
        {
            break;
        }
        else
        {   
            char cmd[50];
            char path[50];
            char string[50];
            sscanf(buf,"%s %s %s",cmd,path,string);
            if(strcmp(cmd,"write") == 0)
            {
                ARGS *args = (ARGS *)malloc(sizeof(ARGS));
                strcpy(args->command,cmd);
                strcpy(args->path,path);
                strcpy(args->string,string);
                pthread_create(&tid[thread_counter],NULL,fs[0],(void *) args);
                thread_counter++;
            }
            else if(strcmp(cmd,"read") == 0)
            {
                ARGS *args = (ARGS *)malloc(sizeof(ARGS));
                strcpy(args->command,cmd);
                strcpy(args->path,path);
                pthread_create(&tid[thread_counter],NULL,fs[1],(void *) args);
                thread_counter++;
            }   
            else if(strcmp(cmd,"empty") == 0)
            {
                ARGS *args = (ARGS *)malloc(sizeof(ARGS));
                strcpy(args->command,cmd);
                strcpy(args->path,path);
                pthread_create(&tid[thread_counter],NULL,fs[2],(void *) args);
                thread_counter++;
            }
            else if(strcmp(cmd,"thread_count") == 0)
            {
                printf("thread_counter = %d\n",thread_counter);
            }
            else
            {
                printf("wrong command!\n");
            }
            
            printf("$ ");
            scanf(" %[^\n]s",buf);
            save_command(buf);
        }
    }
    free(tid);
    return 0;
}
// CS 140 A.Y. 2021-2022 S1 Project 2: File server using thread
// Author: Hans Gabriel H. De Castro (2019-03184)