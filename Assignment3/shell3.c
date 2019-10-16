    #include<stdio.h>
    #include<sys/types.h>
    #include<sys/wait.h>
    #include<stdlib.h>
    #include<string.h>
    #include<unistd.h>
    #include<fcntl.h> 
    #include<time.h>
    #include<sys/ipc.h>
    #include<sys/shm.h>
    int log_flag=0;
    FILE *commandlog,*outputlog;
    int *status_for_exec;
    int shmid;
    int *res;
    /*function for the command line interpreter*/
    void interpreter(char input[200],char *envp[])
    {
     
        time_t T =time(NULL);
        struct tm *obj;
        obj=localtime(&T);
        int no_of_pipes=0;
        for(int i=0;i<strlen(input);i++)
        {
            if(input[i]=='|')
                no_of_pipes++;
        }
        
        /*if(no_of_pipes==0)
        {
            return;
        }*/
        int fd[no_of_pipes][2];
        
     
     
     
        char *words[no_of_pipes+1];
        char *tok;
        tok=strtok(input,"|");
        int k=0;
        while(tok!=NULL)
        {
            words[k++]=tok;
            //printf("strtok  :: %s\n",tok);
            tok=strtok(0,"|");
        }
        
     
     
        char *write;
        int x=0;
        
        write=words[0];
        
        char *w_words[20],*r_words[20];
        //char *tok;
        tok=strtok(write," ");
        while(tok!=NULL)
        {
            w_words[x++]=tok;
            //printf("%s\n",tok);
            tok=strtok(0," ");
        }
        w_words[x]=NULL;
     
        
        if(no_of_pipes==0)
        {
            int pid;
            pid=fork();
            if(pid==0)
            {
                res=(int *)shmat(shmid,NULL,0);
                int op = creat("results.txt",0777) ;
                dup2(op,1) ; 
                close(op) ;
                (*res)=execvp(w_words[0],w_words);
                if(*res==-1)
                {
                    printf("Command %s not found...\n",w_words[0]);
                    exit(5);
                }
            }
            else
            {
                waitpid(-1,NULL,0);
                char result[20];
                if((*res)!=-1)
                    strcpy(result,"Success");
                else
                {
                    strcpy(result,"Fail");
                }
                FILE * ptr = fopen("results.txt","r") ; 
                if(log_flag==1)
                {
                    int f = 0 ;
     
                fprintf(commandlog, "\n----------------------\n" );
                fprintf(outputlog,"\n-----------------------\n");
                    while(w_words[f]!=NULL)
                    {
                        fprintf(commandlog, "%s ",w_words[f]);
                        fprintf(outputlog,"%s ",w_words[f]) ; 
                        f++ ;
                    }
                    fprintf(commandlog, "%d:%d:%d %d:%d:%d %s\n",obj->tm_mday,obj->tm_mon,obj->tm_year+1900,obj->tm_hour,obj->tm_min,obj->tm_sec,result);
                    
                }
                char temp[300] ;
                while(fscanf(ptr,"%[^\n]\n",temp)!=EOF)
                {
                    printf("%s\n",temp);
                    if(log_flag == 1)
                        fprintf(outputlog, "\n\t%s",temp);
                }
                fclose(ptr) ;
            }
        }
        if((*res)==-1 || no_of_pipes == 0)
            return;
        
     
        int status;
        int pid=fork();
        if(pid==0)
        {
            res=(int *)shmat(shmid,NULL,0);
            fd[0][0]=creat("results.txt",0777);
            fd[0][1]=creat("results1.txt",0777);
            dup2(fd[0][0],1);
            
            (*res)=execvp(w_words[0],w_words);
        }
        else{ 
            waitpid(-1,NULL,0);
            char result[20];
            if((*res)!=-1)
                strcpy(result,"Success");
            else
            {
                strcpy(result,"Fail");
            }
            FILE * ptr = fopen("results.txt","r") ; 
            if(log_flag==1)
            {
                int f = 0 ;
                fprintf(commandlog, "\n----------------------\n" );
                fprintf(outputlog,"\n-----------------------\n");
                while(w_words[f]!=NULL)
                {
                    fprintf(commandlog, "%s ",w_words[f]);
                    fprintf(outputlog,"%s ",w_words[f]) ; 
                    f++ ;
                }
                fprintf(commandlog, "%d:%d:%d %d:%d:%d %s\n",obj->tm_mday,obj->tm_mon,obj->tm_year+1900,obj->tm_hour,obj->tm_min,obj->tm_sec,result);
                
            }
            char temp[300] ;
            while(fscanf(ptr,"%[^\n]\n",temp)!=EOF)
            {
                //if(no_of_pipes)
                    //printf("%s\n",c);
                if(log_flag == 1)
                    fprintf(outputlog, "\n\t%s",temp);
            }
            fclose(ptr) ;
        }
        
        if((*res)==-1 )
            return;
     
        for(int i=1;i<=no_of_pipes;i++)
        {
            char *write;
            int x=0;
            
            write=words[i];
            
            
            char *w_words[20];
            char *tok;
            tok=strtok(write," ");
            while(tok!=NULL)
            {
                w_words[x++]=tok;
                //printf("%s\n",tok);
                tok=strtok(0," ");
            }
            w_words[x]=NULL;
     
            int temp_pid;
            temp_pid=fork();
            if(temp_pid==0)
            {
                res=(int *)shmat(shmid,NULL,0);
                if(i%2==0)
                {
                    fd[i-1][0]=open("results1.txt",O_RDONLY);
                    fd[i-1][1]=open("results.txt",O_WRONLY);
                    dup2(fd[i-1][0],0);
                    dup2(fd[i-1][1],1);
                }
                else
                {
                    fd[i-1][0]=open("results1.txt",O_WRONLY);
                    fd[i-1][1]=open("results.txt",O_RDONLY);
                    dup2(fd[i-1][0],1);
                    dup2(fd[i-1][1],0);
                }
                (*res)=execvp(w_words[0],w_words);
            }
            else
            {
                waitpid(-1,NULL,0);
                char result[20];
            if((*res)!=-1)
                strcpy(result,"Success");
            else
            {
                strcpy(result,"Fail");
            }
            FILE * ptr ; 
            if(i%2 == 1)
                ptr = fopen("results1.txt","r") ; 
            else
                ptr = fopen("results.txt","r") ; 
            if(log_flag==1)
            {
                int f = 0 ;
                fprintf(commandlog, "\n----------------------\n" );
                fprintf(outputlog,"\n-----------------------\n");
                while(w_words[f]!=NULL)
                {
                    fprintf(commandlog, "%s ",w_words[f]);
                    fprintf(outputlog,"%s ",w_words[f]) ; 
                    f++ ;
                }
                fprintf(commandlog, "%d:%d:%d %d:%d:%d %s\n",obj->tm_mday,obj->tm_mon,obj->tm_year+1900,obj->tm_hour,obj->tm_min,obj->tm_sec,result);
                
            }
            char temp[300] ;
            while(fscanf(ptr,"%[^\n]\n",temp)!=EOF)
            {
                if(no_of_pipes == i)
                    printf("%s\n",temp);
                if(log_flag == 1)
                    fprintf(outputlog, "\n\t%s",temp);
            }
            fclose(ptr) ;
            }
            if(*res == -1)
                return ;        
        }
        /*code for removing intermediate files*/
     
        //printf("Exiting succesfully...\n");
        
    }
     
    int main(int c,char *argv[],char *envp[])
    {
        shmid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
        res=(int *)shmat(shmid,NULL,0);
        commandlog=fopen("command.log","w");
        outputlog=fopen("output.log","w");
        fclose(commandlog) ;
        fclose(outputlog) ;
        char input[200];
        gets(input);
        while(strcmp(input,"entry")!=0)
        {
            printf("Command line interpreter not started\n");
            gets(input);
        }
        printf("************************WELCOME TO THE CUSTOM TERMINAL************************\n");
        printf("Version: 1.0.1\n");
        printf("Creator:Jineet Desai\n");
        printf("*******************************************************************************\n");
     
        while(1)
        {
            printf(">");
            gets(input);
     
            if(strcmp(input,"exit")==0)
            {
                printf("Exiting the terminal......\n");
                break;
            }
            else if(strcmp(input,"log")==0)
            {
                commandlog=fopen("command.log","a+");
                outputlog=fopen("output.log","a+");
                log_flag=1;
            }
            else if(strcmp(input,"unlog")==0)
            {
                fclose(commandlog);
                fclose(outputlog);
                log_flag=0;
            }
            else if(strcmp(input,"viewcmdlog")==0)
            {
                fseek(commandlog,0,SEEK_SET) ; 
                char c;
                while((c=fgetc(commandlog))!=EOF)
                {
                    //printf("%s\n","hello");
                    printf("%c",c);
                }
                printf("\n");
                fseek(commandlog,0,SEEK_END);
            }
            else if(strcmp(input,"viewoutlog")==0)
            {
                fseek(outputlog,0,SEEK_SET);
                char c;
                while((c=fgetc(outputlog))!=EOF)
                {
                    printf("%c",c);
                }
                printf("\n");
                fseek(outputlog,0,SEEK_END) ;
            }
            else 
            {
                //printf("Other commands still in progress\n");
                int no_of_spaces=0;
                for(int i=0;i<strlen(input);i++)
                {
                    if(input[i]==' ')
                        no_of_spaces++;
                }
                char words[no_of_spaces+1][100];
                int row=0;
                int col=0;
                for(int i=0;i<strlen(input);i++)
                {
                    if(input[i]==' ' || input[i]=='\0')
                    {
                        words[row++][col]='\0';
                        col=0;
                    }
                    else
                        words[row][col++]=input[i];
                }
                if(strcmp(words[0],"changedir")==0)
                {
                    int status=chdir(words[1]);
                    if(status==0)
                    {
                        printf("Directory changed successfully....\n");
                        char path[200];
                        getcwd(path,200);
                        printf("Current path is %s\n",path);
                    }
                    else
                    {
                        printf("Invalid path to changedir...\n");
                    }
                }
                else
                {
                    interpreter(input,envp);
                }
            }
        }
        return 0;
     
    }