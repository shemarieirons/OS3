// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str1[] = "howard.edu";
    char fixed_str2[] = "gobison.org"; 
    char input_str[100]; 
    char second_input[100]; 
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate:");
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process 
    else if (p > 0) 
    { 
  
        close(fd1[0]);  // Close reading end of pipes 
        close(fd2[1]);
  
        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str, strlen(input_str)+1); 
        
  
        // Wait for child to print the concatenated string 
        wait(NULL); 
  
        char received_str[200]; 
        read(fd2[0], received_str, 200); 
  
        close(fd2[0]); // Close writing end of pipes 
        close(fd1[1]); 
  
        // Concatenate a fixed string with it 
        int k = strlen(received_str); 
        int i; 
        for (i=0; i<strlen(fixed_str2); i++) 
            received_str[k++] = fixed_str2[i]; 
  
        received_str[k] = '\0';   // string ends with '\0' 
  
        printf("Output : %s\n", received_str); 
    } 
  
    // child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]); 
  
        // Read a string using first pipe 
        char concat_str[200]; 
        read(fd1[0], concat_str, 100); 
  
        // Concatenate a fixed string with it 
        int k = strlen(concat_str); 
        int i; 
        for (i=0; i<strlen(fixed_str1); i++) 
            concat_str[k++] = fixed_str1[i]; 
  
        concat_str[k] = '\0';   // string ends with '\0' 
  
        printf("Output : %s\n", concat_str); 
  
        printf("Input : "); 
        scanf("%s", second_input); 
  
        k = strlen(concat_str); 
        for (i=0; i<strlen(second_input); i++) 
            concat_str[k++] = second_input[i]; 
  
        concat_str[k] = '\0'; 
  
        write(fd2[1], concat_str, strlen(concat_str)+1); 
  
        // Close both reading ends 
        close(fd1[0]); 
        close(fd2[1]); 

  
        exit(0); 
    } 
} 
