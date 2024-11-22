#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;  //file descriptor
    ssize_t bytes_written;
    off_t offset;
    char buffer[1024];  //buffer to hold the daata
    int null_count = 0; //counter to track tyhe null characters 


    fd = open("check.txt", O_CREAT | O_WRONLY | O_TRUNC, 0600);  // creating the file and giving 0600 permission
    if (fd == -1) {
        printf("Failure, Cannot open and create the file"); //Printing error message
        return 1;
    }
    const char *t_1 = "University of Windsor\nUniversity of Windsor\n"; // writting University of windsor text two times in the file
    bytes_written = write(fd, t_1, strlen(t_1));
    if (bytes_written == -1) {
        printf("Failure, Cannot write in the file "); //Printing error message
        close(fd); // close function
        return 1;
    }
    close(fd); // closing the file

   

    fd = open("check.txt", O_RDWR); //open file check.txt in RDWR operation
    if (fd == -1) {
        printf("Failure, Cannot open the file"); //Printing error message
        return 1;
    }
    offset = lseek(fd, 90, SEEK_SET);  // Perform lseek operation
    if (offset == -1) {
        printf("Failure, Cannot seek in the file"); //Printing error message
        close(fd);
        return 1;
    }

    
    const char *t_2 = "Spring 2024\nSpring 2024\nSpring 2024\n";  // Writing 3 consecutive times "Spring 2024"
    bytes_written = write(fd, t_2, strlen(t_2));
    if (bytes_written == -1) {
        printf("Failure, Cannot write in the file");  //Printing error message
        close(fd);
        return 1;
    }
    close(fd);

    
    fd = open("check.txt", O_RDWR);  // opening te file with RDWR operattionn
    if (fd == -1) {
        printf("Failure, Cannot open the file");  //Printing error message
        return 1;
    }
    offset = lseek(fd, -50, SEEK_END);  // performing lseek operation again
    if (offset == -1) {
        printf("Failure, Cannot seek in the file");  //Printing error message
        close(fd);
        return 1;
    }


    const char *t_3 = "COMP 8567\n";  // writting "COMP8567" one time 
    bytes_written = write(fd, t_3, strlen(t_3));
    if (bytes_written == -1) {
        printf("Failure, Cannot write in the file");  //Printing error message
        close(fd);
        return 1;
    }
    lseek(fd, 0, SEEK_SET); //reset function
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);  //reading the content of the file
    if (bytes_read == -1) {
        printf("Failure, Cannot read the file");  //Printing error message
        close(fd);
        return 1;
    }
    for (int i = 0; i < bytes_read; i++) {  //function to replace null characters with "@"
        if (buffer[i] == '\0') { 
            buffer[i] = '@';
            null_count++; //counting the null characters
        }
    }
    lseek(fd, 0, SEEK_SET);
    bytes_written = write(fd, buffer, bytes_read); //write the modig\fied content back in the file 
    if (bytes_written == -1) {
        printf("Failure, Cannot write the modified content in the file");  //Printing error message
        close(fd);
        return 1;
    }
    printf("Total number of NULL characters replaced are : %d\n", null_count);  //printimnng the total null characters replaced 
    close(fd); //closing the file

    return 0;
}
