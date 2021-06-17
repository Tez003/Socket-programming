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

using namespace std;

#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
#define MAX_MSG_SIZE 100

 

int sockfd; 

void list_fun(string username){
    string msg_str;
    int noBytes;
	char msg_buf[MAX_MSG_SIZE];

     if((noBytes = recv(sockfd, msg_buf, MAX_MSG_SIZE, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[noBytes] = '\0';

    int file_size;
    char mode_t;
	mode_t = msg_buf[noBytes - 1];
	msg_buf[noBytes - 1] = '\0';
	sscanf(msg_buf, "%d", &file_size);

    strcpy(msg_buf, "OK");
	send(sockfd, msg_buf, strlen(msg_buf), 0);
	// get the file
   
	int bytes_transferred = 0;
	
	while(bytes_transferred < file_size) {
		noBytes = recv(sockfd, msg_buf, MAX_MSG_SIZE-1, 0);
		bytes_transferred += noBytes;
		msg_buf[noBytes] = '\0';
        printf("------------------- files present are-----------\n%s", msg_buf);
       
	}
	// notify the client that the file is received
	strcpy(msg_buf, "OK");
	if((noBytes = send(sockfd, msg_buf, 2, 0)) == -1) {
		perror("send");
		exit(1);
	}
	


}

void get_fun(string username){
       char msg_buf[MAX_MSG_SIZE], mode_t;
       int noBytes;
       string fname, msg_str;

       printf("Enter the name of the file: ");
       cin>>fname;

       if((noBytes = send(sockfd, &fname[0], fname.length(), 0)) == -1) {
		perror("send");
		exit(1);
	}

        if((noBytes = recv(sockfd, msg_buf, MAX_MSG_SIZE, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[noBytes] = '\0';
    printf("Server: %s\n",msg_buf );

    msg_str = "ok";
    if(strcmp(msg_buf, "absent") == 0){
        msg_str = "no";
    }

    
      if((noBytes = send(sockfd, &msg_str[0], msg_str.length(), 0)) == -1) {
		perror("send");
		exit(1);
	}

    if(msg_str == "no"){
        return;
    }

    printf("\nEntr the mode of transfer(b for binary/c for character modes respectively): ");
    cin>>mode_t;

    if((noBytes = send(sockfd, &mode_t, 1, 0)) == -1) {
		perror("send");
		exit(1);
	}   


     if((noBytes = recv(sockfd, msg_buf, MAX_MSG_SIZE, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[noBytes] = '\0';

    int file_size;
	mode_t = msg_buf[noBytes - 1];
	msg_buf[noBytes - 1] = '\0';
	sscanf(msg_buf, "%d", &file_size);

    strcpy(msg_buf, "OK");
	send(sockfd, msg_buf, strlen(msg_buf), 0);
	// get the file
    printf("Progress: ");
	int bytes_transferred = 0;
	FILE* fd;
	fd = fopen(( fname).c_str(), mode_t == 'c' ? "w" : "wb");
    double percentIndicator = 0.01, precision = 0.01;
	while(bytes_transferred < file_size) {
		noBytes = recv(sockfd, msg_buf, MAX_MSG_SIZE, 0);
		bytes_transferred += noBytes;
		fwrite(msg_buf, noBytes, 1, fd);
        while(bytes_transferred >= percentIndicator * file_size) {
			percentIndicator += precision;
			printf("#");
		}
	}
    printf("\n");
	// notify the client that the file is received
	strcpy(msg_buf, "OK");
	if((noBytes = send(sockfd, msg_buf, 2, 0)) == -1) {
		perror("send");
		exit(1);
	}
	fclose(fd);
	strcpy(msg_buf, ("Received " + fname + ".\n").c_str());
	printf("%s", msg_buf);




}

void put_fun( string username){

   char msg_buf[MAX_MSG_SIZE], t_mode = 'c';
	int numbytes;
	string msg_str, file_path, file_name;
	printf("Enter the path to the file: ");
	cin >> file_path;
	printf("Enter the name of the file: ");
	cin >> file_name;
	printf("Enter the mode of transfer (binary(b)/character(c)): ");

	scanf(" %c", &t_mode);
	fstream file;
	file.open(file_path + "/" + file_name, ios::in);
	if(!file) {
        msg_str = "absent";
		printf("File doesn't exist.\n");
		
	}
    else{
        msg_str = file_name;
    }
	file.close();
	// send the command to the server
	
	if((numbytes = send(sockfd, &msg_str[0], msg_str.length(), 0)) == -1) {
		perror("send");
		exit(1);
	}
    if(msg_str == "absent"){
        return;
    }
	// receive the response from the server
	if((numbytes = recv(sockfd, msg_buf, MAX_MSG_SIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[numbytes] = '\0';
	// ask the user what to do if file already exists
	if(strcmp(msg_buf, "RE") == 0) {
		printf("File with the same name already exists in server.\n Do you want to overwrite type r else type e to exit this command.??");
		char s;
		scanf(" %c", &s);
		if(s == 'e') {
			// send the reply to the server
			send(sockfd, "exit", 4, 0);
			return;
		}
	}
	// send the file_size to the server
	int fd = open((file_path + "/" + file_name).c_str(), O_RDONLY); 
    struct stat file_stat;
	fstat(fd, &file_stat);
	int file_size = (int)file_stat.st_size;
	printf("%d", file_size);
	close(fd);
	msg_str = to_string(file_size).append(1, t_mode);
	if((numbytes = send(sockfd, &msg_str[0], msg_str.length(), 0)) == -1) {
		perror("send");
		exit(1);
	}
	// receive the response for file_size
	recv(sockfd, msg_buf, MAX_MSG_SIZE, 0);
	// open the file for the transfer
	printf("Progress: ");
	strcpy(msg_buf, (file_path + "/" + file_name).c_str());
	FILE* fp = fopen(msg_buf, t_mode == 'c' ? "r" : "rb"); 
	int bytes_transferred = 0, bytes_read;
	double percentIndicator = 0.01, precision = 0.01;
	// start the file transfer
	while(bytes_transferred < file_size) {
		// read the data into msg_buf
		bytes_read = fread(msg_buf, 1, MAX_MSG_SIZE, fp);
		// send the data to the server
		numbytes = send(sockfd, msg_buf, bytes_read, 0);
		bytes_transferred += numbytes;
		while(bytes_transferred >= percentIndicator * file_size) {
			percentIndicator += precision;
			printf("#");
		}
	}
	fclose(fp);
	printf(" 100%% finished.\n");
	// receive the response from the server
	if((numbytes = recv(sockfd, msg_buf, MAX_MSG_SIZE-1, 0)) == -1) {
		perror("recv");
		exit(1);
	}
	msg_buf[numbytes] = '\0';
	// print the response to the user
	if(strcmp(msg_buf, "NO") == 0)
		printf("There was a problem in uploading you file.\n");
	else
	    printf("File successfully uploaded. %d bytes transferrred.\n", file_size);
    

}

int command(string username){
    char recv_msg[MAX_MSG_SIZE];
     string send_msg;

     int noBytes;

    if((noBytes = recv(sockfd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
    {
        perror("recv");
        return 0;
    }
    else
    {
        recv_msg[noBytes]='\0';
        printf("Server: %s\n", recv_msg);
    } 



     cin>>send_msg;

    if((noBytes = send(sockfd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }

    if(send_msg == "put"){
        put_fun(username);
    }
   else if(send_msg == "get"){
     get_fun(username);
    }
    else if(send_msg == "list"){
     list_fun(username);
    }
    else if(send_msg == "exit"){
        return 0;
    }
    return 1;
}

 
void registr(){
    char recv_msg[MAX_MSG_SIZE];
     string send_msg;

     int noBytes;
    
   /* printf("Enter operation and parameters: ");
    scanf("%[^\n]*c", send_msg);
    send(sockfd, send_msg, MAX_MSG_SIZE, 0);*/
    if((noBytes = recv(sockfd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
    {
        perror("recv");
        return;
    }
    else
    {
        recv_msg[noBytes]='\0';
        printf("Server: %s\n", recv_msg);
    } 

    cin>>send_msg;

    if((noBytes = send(sockfd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }



    if((noBytes = recv(sockfd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
    {
        perror("recv");
        return ;
    }
    else
    {   
        recv_msg[noBytes]='\0';
        printf("Server: %s\n", recv_msg);
    } 

    cin>>send_msg;

    if((noBytes = send(sockfd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }

    if((noBytes = recv(sockfd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
    {
        perror("recv");
        return ;
    }
    else
    {   
        recv_msg[noBytes]='\0';
        printf("Server: %s\n", recv_msg);
    }

    return ;
}

int login(string username,string password){

    char recv_msg[MAX_MSG_SIZE];
     string send_msg;

     int noBytes;
    int rcv_num;
   read(sockfd, &rcv_num, sizeof(int));

    if(rcv_num == 1){
        printf("Success");
        return 1;
    }

    return 0;
}


void* client_process(void* args) 
{ 
     char recv_msg[MAX_MSG_SIZE];
     string send_msg;

     int noBytes;
    
   /* printf("Enter operation and parameters: ");
    scanf("%[^\n]*c", send_msg);
    send(sockfd, send_msg, MAX_MSG_SIZE, 0);*/
    if((noBytes = recv(sockfd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
    {
        printf("Connection closed\n");
        return 0;
    }
    else
    {
        recv_msg[noBytes]='\0';
        printf("Server: %s\n", recv_msg);
    } 

    cin>>send_msg;

    if((noBytes = send(sockfd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }
    string username = send_msg;


    if((noBytes = recv(sockfd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
    {
        printf("Connection closed\n");
        return 0;
    }
    else
    {   
        recv_msg[noBytes]='\0';
        printf("Server: %s\n", recv_msg);
    } 

    cin>>send_msg;
    string password = send_msg;
    if((noBytes = send(sockfd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }




        // *************************Login or register based on the details**************************************

    int check;
        if(username == "Admin" && password.compare("admin")==0){
            registr();
            pthread_exit(NULL);
        }

        else{
            check = login(username, password);
            printf("\n Login Successful");
        }


        if(check == 1){

            

            
            if((noBytes = recv(sockfd, recv_msg, MAX_MSG_SIZE, 0)) == -1)
            {
                printf("Connection closed\n");
                return 0;
            }
            else
            {   
                recv_msg[noBytes]='\0';
                printf("Server: %s\n", recv_msg);
            } 


            cin>>send_msg;
            if((noBytes = send(sockfd, &send_msg[0], send_msg.length()*sizeof(char), 0)) == -1) {
                    perror("send");
                    exit(1);
                }

            int result =1;

            while(result != 0)
            result = command(username);
            
        }
    
  
            pthread_exit(NULL);
                 
            
}
 

int main() 
{ 
    pthread_t send,receive; 
    

 

    struct sockaddr_in servaddr, cli; 
  
// client socket creation and varification
    
        sockfd = socket(AF_INET, SOCK_STREAM, 0); 
        
        if (sockfd == -1) 
        { 
            printf("socket creation failed...\n"); 
            exit(0); 
        } 
        else
            printf("Socket successfully created..\n"); 
        bzero(&servaddr, sizeof(servaddr)); 
    
  
// assigning IP, PORT 
        servaddr.sin_family = AF_INET; 
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
        servaddr.sin_port = htons(PORT); 
    
  
  
// connecting the client socket to server socket 
        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
            printf("connection with the server failed...\n"); 
            exit(0); 
        } 
        else
            printf("Client welcomes you too..\n"); 
    
    pthread_create(&send, NULL, client_process, NULL); 
    //pthread_create(&receive, NULL, mrcv, NULL); 
    pthread_join(send, NULL); 
    //thread_join(send, NULL);
    
// close the socket 
    close(sockfd); 
    
} 