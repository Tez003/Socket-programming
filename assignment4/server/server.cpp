#include <unordered_map>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include<bits/stdc++.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

using namespace std;
 

#define MAX_MSG_SIZE 100
#define INF 1e9
 
 

struct sockaddr_in servaddr, cli;

 
socklen_t client_len;
int num_clients ;

int connfd,len;

void list_fun(string username, int new_sock_fd){
    int bytes_transferred = 0, bytes_read, noBytes; 
	char msg_buf[MAX_MSG_SIZE];
    char mode = 'c';
	string msg_str;
	// find the size of the file
	int fd = open(("data/"  + username + "/history.txt").c_str(), O_RDONLY);
    struct stat file_stat;
    fstat(fd, &file_stat);
    int file_size = (int)file_stat.st_size;
    printf("%d", file_size);
    close(fd);

    msg_str = to_string(file_size).append(1,mode);
    if((noBytes = send(new_sock_fd, &msg_str[0], msg_str.length(), 0)) == -1){
        perror("send");
        exit(1);
    }

    recv(new_sock_fd, msg_buf, MAX_MSG_SIZE, 0);
	// open the file for the transfer
	
	strcpy(msg_buf, ("data/"  + username + "/history.txt").c_str());
	FILE* fp = fopen(msg_buf, mode == 'c' ? "r" : "rb"); 
	
	
	// start the file transfer
	while(bytes_transferred < file_size) {
		// read the data into msg_buf
		bytes_read = fread(msg_buf, 1, MAX_MSG_SIZE, fp);
		// send the data to the server
		noBytes = send(new_sock_fd, msg_buf, bytes_read, 0);
		bytes_transferred += noBytes;
		
	}
	fclose(fp);
	printf(" 100%% finished.\n");
	// receive the response from the server
	if((noBytes = recv(new_sock_fd, msg_buf, MAX_MSG_SIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[noBytes] = '\0';
	// print the response to the user
	if(strcmp(msg_buf, "NO") == 0)
		printf("There was a problem in uploading you file.\n");
	else
	    printf("File successfully uploaded. %d bytes transferrred.\n", file_size);
    

}


void get_fun(string username, int new_sock_fd){
    char msg_buf[MAX_MSG_SIZE], mode = 'b';
    int noBytes;
    string fpath, msg_str;
    string fname;

    if((noBytes = recv(new_sock_fd, msg_buf, MAX_MSG_SIZE, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[noBytes] = '\0';
    fpath = "data/";
    fname = msg_buf;
    printf("Result: %s\n", msg_buf);

    fstream file;
	file.open(fpath  + username + "/" + fname, ios::in);
	if(!file) {
        msg_str = "absent";
		printf("File doesn't exist.\n");
		
	}
    else{
        msg_str = "present";
    }
	file.close();

    if((noBytes = send(new_sock_fd, &msg_str[0], msg_str.length(), 0)) == -1) {
		perror("send");
		exit(1);
	}

     if((noBytes = recv(new_sock_fd, msg_buf, MAX_MSG_SIZE, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[noBytes] = '\0';
    printf("Result: %s\n", msg_buf);
    if(strcmp(msg_buf, "no") == 0){
        return;
    }
    if((noBytes = recv(new_sock_fd, &mode, 1, 0)) == -1) {
		perror("recv");
		exit(1);
	}

    //file size 
    int fd = open((fpath  + username + "/" + fname).c_str(), O_RDONLY);
    struct stat file_stat;
    fstat(fd, &file_stat);
    int file_size = (int)file_stat.st_size;
    printf("%d", file_size);
    close(fd);

    msg_str = to_string(file_size).append(1,mode);
    if((noBytes = send(new_sock_fd, &msg_str[0], msg_str.length(), 0)) == -1){
        perror("send");
        exit(1);
    }

    recv(new_sock_fd, msg_buf, MAX_MSG_SIZE, 0);
	// open the file for the transfer
	
	strcpy(msg_buf, (fpath  + username + "/" + fname).c_str());
	FILE* fp = fopen(msg_buf, mode == 'c' ? "r" : "rb"); 
	int bytes_transferred = 0, bytes_read;
	
	// start the file transfer
	while(bytes_transferred < file_size) {
		// read the data into msg_buf
		bytes_read = fread(msg_buf, 1, MAX_MSG_SIZE, fp);
		// send the data to the server
		noBytes = send(new_sock_fd, msg_buf, bytes_read, 0);
		bytes_transferred += noBytes;
		
	}
	fclose(fp);
	printf(" 100%% finished.\n");
	// receive the response from the server
	if((noBytes = recv(new_sock_fd, msg_buf, MAX_MSG_SIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[noBytes] = '\0';
	// print the response to the user
	if(strcmp(msg_buf, "NO") == 0)
		printf("There was a problem in uploading you file.\n");
	else
	    printf("File successfully uploaded. %d bytes transferrred.\n", file_size);
    

    
    





}


void put_fun(string username, int new_sock_fd){
    string file_name;
      char recv_msg[MAX_MSG_SIZE];
        string send_msg;
        int noBytes;


     if((noBytes = recv(new_sock_fd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
        {
            return ;
        }
         recv_msg[noBytes]='\0';
        if(strcmp(recv_msg,"absent") == 0){

                return;
            
        }
        
        recv_msg[noBytes]='\0';
         printf("Result: %s\n", recv_msg);
         
         file_name = recv_msg;

    char msg_buf[MAX_MSG_SIZE], t_mode;
	int numbytes;
	fstream file;
	file.open("data/" + username + "/" + file_name, ios::in);
	file ? strcpy(msg_buf, "RE") : strcpy(msg_buf, "OK"); 
	file.close();
	// send the response to the client
	if((numbytes = send(new_sock_fd, msg_buf, 2, 0)) == -1) {
		perror("send");
		exit(1);
	}
	// receive the response from the client
	if((numbytes = recv(new_sock_fd, msg_buf, MAX_MSG_SIZE, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[numbytes] = '\0';
	// if the file is asked not to replace on the server
	if(strcmp(msg_buf, "exit") == 0)
		return;
	// know the file_size
	int file_size;
	t_mode = msg_buf[numbytes - 1];
	msg_buf[numbytes - 1] = '\0';
	sscanf(msg_buf, "%d", &file_size);
	// send the response to the client
	strcpy(msg_buf, "OK");
	send(new_sock_fd, msg_buf, strlen(msg_buf), 0);
	// get the file
	int bytes_transferred = 0;
	FILE* fd;
	fd = fopen(("data/" + username + "/" + file_name).c_str(), t_mode == 'c' ? "w" : "wb");
	while(bytes_transferred < file_size) {
		numbytes = recv(new_sock_fd, msg_buf, MAX_MSG_SIZE, 0);
		bytes_transferred += numbytes;
		fwrite(msg_buf, numbytes, 1, fd);
	}
	// notify the client that the file is received
	strcpy(msg_buf, "OK");
	if((numbytes = send(new_sock_fd, msg_buf, 2, 0)) == -1) {
		perror("send");
		exit(1);
	}
	fclose(fd);
	strcpy(msg_buf, ("Received " + file_name + " for " + username + ".\n").c_str());
	printf("%s", msg_buf);
	FILE *fptr;
	fptr = fopen(("data/" + username + "/history.txt").c_str(), "a");
	file_name.append(1, '\n');
	fwrite(&file_name[0], file_name.length(), 1, fptr);
	fclose(fptr);
    return;

}


int command(string username, int new_sock_fd){
    char recv_msg[MAX_MSG_SIZE];
        string send_msg;
        int noBytes;

    send_msg = "enter command";

    if((noBytes = send(new_sock_fd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }

    if((noBytes = recv(new_sock_fd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
        {
            
            return 0;
        }
        
        recv_msg[noBytes]='\0';
         string cmd;
         cmd = recv_msg;
            printf("Result: %s\n", recv_msg);

        if(strcmp(recv_msg,"put") == 0){
            put_fun(username, new_sock_fd);
        }
        else if(strcmp(recv_msg, "get") == 0){
        get_fun(username,new_sock_fd);
        }
        else if(strcmp(recv_msg, "list") == 0){
        list_fun(username,new_sock_fd);
        }
        else if(strcmp(recv_msg,"exit") == 0){
            return 0;
        }
        

    return 1;

}

//****************************register function *****************************************//
void registr(int new_sock_fd)
{

         char recv_msg[MAX_MSG_SIZE];
        string send_msg;
        int noBytes;

        send_msg = "Enter your Login Username";
        if((noBytes = send(new_sock_fd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }
       
       if((noBytes = recv(new_sock_fd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
        {
            
            return ;
        }
        
        recv_msg[noBytes]='\0';
         string username;
         username = recv_msg;
            printf("Result: %s\n", recv_msg);
         
        
    

        send_msg = "Enter your Password";
        if((noBytes = send(new_sock_fd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }
       
       if((noBytes = recv(new_sock_fd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
        {
            
            return ;
        }
        
       recv_msg[noBytes]='\0';
         string password;
         password = recv_msg;
            printf("Result: %s\n", recv_msg);


        //********************************************************************//
        
        string ru,rp;

        
        
        ofstream reg("database.txt",ios::app);
        reg<<username<<' '<<password<<endl;
        string dirname = "data/" + username; 
	    mkdir(&dirname[0], 0777);
	    FILE* fd;
	    fd = fopen(("data/" + username + "/history.txt").c_str(), "w");
	    fclose(fd);
        send_msg = "\nRegistration Sucessful\n";
        if((noBytes = send(new_sock_fd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }

        return ;
        
        

}
//************************************** Login function *****************************//
int login( string username, string password, int new_sock_fd)
{
        int count = 0;
        string user,pass,u,p;
        string send_msg;
        int noBytes;
        
        ifstream input("database.txt");
        while(input>>u>>p)
        {
                if(u==username && p==password)
        
                {
                    

                   
                   count =1;
                   break;    
                        
                }

                
        }
        input.close();
        int n;
        if(count != 1){
                    int send_num = 0;
                    n=write(new_sock_fd,&send_num, sizeof(int));
                        if(n<0){
                        printf("Error writing");
                        return count;} 
                    return count;
                }
        else{
            int send_num = 1;
            printf("Login Successful");
                    n=write(new_sock_fd,&send_num, sizeof(int));
                        if(n<0){
                        printf("Error writing");
                        return count;} 
                    return count;

                } 
        

        
        return count;
}
/****************************** thread function ****************************/
void *process(void *args)
{
    int sockfd = *(int *)args;
    int new_sock_fd = accept(sockfd, (SA*)&cli,&client_len);

    printf("Connected\n");

 

    if(new_sock_fd == -1)
    {
    fprintf(stderr, "accept error\n");
    pthread_exit(0);
    }

 

   /* for(;;)
    {*/
        char recv_msg[MAX_MSG_SIZE];
        string send_msg;
        int noBytes;

        send_msg = "Enter your Login Details";
        if((noBytes = send(new_sock_fd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }
       
       if((noBytes = recv(new_sock_fd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
        {
            printf("Connection closed\n");
            return 0;
        }
        
        recv_msg[noBytes]='\0';
         string username;
         username = recv_msg;
            printf("Result: %s\n", recv_msg);
         
        
    

        send_msg = "Enter your Password";
        if((noBytes = send(new_sock_fd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }
       
       if((noBytes = recv(new_sock_fd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
        {
            printf("Connection closed\n");
            return 0;
        }
        
       recv_msg[noBytes]='\0';
         string password;
         password = recv_msg;
            printf("Result: %s\n", recv_msg);
        

        // *************************Login or register based on the details**************************************
int check;


        if(username == "Admin" && password.compare("admin")==0){
            registr(new_sock_fd);
            goto E;
        }

        else{
            
           check = login(username, password, new_sock_fd);
            
        }

        if(check ==1){
            
            send_msg = "Hi from Server";
            if((noBytes = send(new_sock_fd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }

            if((noBytes = recv(new_sock_fd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
            {
                printf("Connection closed\n");
                return 0;
            }
            
            recv_msg[noBytes]='\0';
                printf("Result: %s\n", recv_msg);
            

            int result =1;

            while(result != 0)
            result = command(username, new_sock_fd);

        }
        

     printf("Connection closed\n");
      
   E: close(new_sock_fd);

    pthread_exit(0);
}

 

//*********************************** Driver function ******************************************
int main()
{
    num_clients=50;
    
    pthread_t connections[num_clients];

 

        int sockfd;
        int opt=1;

// server socket creation
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd == -1)
        {
        printf("socket creation failed...\n"); //checks if socket is created or not
        exit(0);
        }
        else
        printf("Socket successfully created..\n");

 

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
        {
        perror("setsockopt");
        exit(EXIT_FAILURE);
        }

        bzero(&servaddr, sizeof(servaddr));

        // assigning IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);


        // Binding newly created socket to given IP and verification
        if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
        {
        printf("socket bind failed...\n");
        exit(0);
        }
        else
        printf("Socket successfully binded..\n");


        // Now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
        }
        else
        printf("Server listening..\n");
        client_len = sizeof(cli);

        int i;
        for(i = 0;i < num_clients;++i)
        pthread_create(&connections[i], NULL, process, &sockfd);

        for(i = 0;i < num_clients;++i)
        pthread_join(connections[i], NULL);


close(sockfd); // After chatting socket is closed

}