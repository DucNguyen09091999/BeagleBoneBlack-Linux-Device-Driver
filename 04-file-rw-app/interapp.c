//note: App bi loi. Chua fix duoc

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio_ext.h>
#include <errno.h>
#define CDEV_PATH "/dev/m-dev"
#define BUFF_SIZE 1024
int fd,opt;
char write_buf[BUFF_SIZE];
char read_buf[BUFF_SIZE];
int ret;

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
    printf("Starting device test program\n");

    fd = open(CDEV_PATH,O_RDWR);
    if(fd < 0){
        printf("Can't open device file: %s ,errorno: %d, %s\n",CDEV_PATH,errno,strerror(errno));
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
            ret = write(fd,write_buf,strlen(write_buf)+1);
            if(ret < 0){
                printf("Failed to write data into device, errno: %d, %s\n",errno,strerror(errno));
            }
            printf("Data write: %s\n",write_buf);
            printf("Done!\n\n\n");
            break;
        case 2:
            printf("Data reading ...\n");
            ret = read(fd,read_buf,BUFF_SIZE);
            if(ret < 0){
                printf("Failed to read data from device, errno: %d, %s\n",errno,strerror(errno));
            }
            printf("Data read: %s\n",read_buf);
            printf("Done!\n");
            printf("Data: %s\n\n\n",read_buf);
            break;
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