#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio_ext.h>

#define CDEV_PATH "/dev/m-dev"
#define BUFF_SIZE 1024
int fd,opt;
char write_buf[BUFF_SIZE];
char read_buf[BUFF_SIZE];


void Print_Menu(){

    printf("***** Please Enter Your Option *****\n");
    printf("*       1. Write Data              *\n");
    printf("*       2. Read Data               *\n");
    printf("*       3. Exit                    *\n");
    printf("************************************\n");
    printf(">>> ");
}


int main(void){
    printf("**************************************\n");
    printf("*** Open/Read/Write kernel program ***\n");

    fd = open(CDEV_PATH,O_RDWR);
    if(fd < 0){
        printf("Can't open device file: %s\n",CDEV_PATH);
        return -1;
    }
    while (1)
    {
        Print_Menu();
        scanf("%d",&opt);
        switch (opt)
        {
        case 1:
            printf("Please Enter the data to write into driver: ");
            scanf(" %[^\t\n]s",write_buf);
            printf("Data writing process ...");
            write(fd,write_buf,strlen(write_buf)+1);
            printf("Done!\n\n\n");
            break;
        case 2:
            printf("Data reading ...\n");
            read(fd,read_buf,BUFF_SIZE);
            printf("Done!\n");
            printf("Data: %s\n\n\n",read_buf);
        case 3:
            printf("Close the program\n");
            close(fd);
            exit(1);
            break;
        default:
            printf("Enter valid option = %c\n",opt);
            break;
        }
    }
    close(fd);
    return 0;
    
}