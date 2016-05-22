#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "unpack.h"

//obtener el size de un archivo
    long long fsize(char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return (long long)st.st_size;

               perror("Error, No se pudo determinar el size del archivo");

    return -1;
     }

//Retorna la extension de un archivo
	 char *get_filename_ext( char *filename) {
	 	char *dot = strrchr(filename, '.');
	    if(!dot || dot == filename) return "";
	    	return dot + 1;
	}

//concatena el contenido de s2 en s1
     char* concat(char *s1, char *s2){

          int tam = strlen(s1)+strlen(s2)+1;
         char *result;
         result = (char *)malloc(tam);
         strcpy(result, s1);
         strcat(result, s2);
         return result;
     }

long long int convert(char *num){
     
     int i,j,tam=strlen(num);
    
    int pot = 0;
    int base = 1;
    long long int acum = 0;
    int digit;   
     for(i=tam-1;i>0;i--){
    if(num[i] >= '0' && num[i] <= '9'){
        
        char n[1];
        n[0] = num[i];
        acum = acum + (atoi(n)*base);

    } 
    
    if(num[i] >= 'a')
    {
        switch(num[i]){

            case 'a':
                digit = 10;
                break;

            case 'b':
                digit = 11;
                break;

            case 'c':
                digit = 12;
                break;

            case 'd':
                digit = 13;
                break;

            case 'e':
                digit = 14;
                break;

            case 'f':
                digit = 15;
                break;

        }
        acum = acum + (digit*base);       
    }
        base*=16;
   
   }

     return acum;
     }

