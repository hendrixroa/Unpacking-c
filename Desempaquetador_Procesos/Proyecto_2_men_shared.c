#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

struct timeval t0, t1;
double media=0.0;

char* concat(char *s1, char *s2);

long long int convert(char *num);

long long fsize(char *filename); 

int ClientProcess(char *pathfile);

void manejadorDeSenales(pid_t process);

char *get_filename_ext( char *filename); 
     
     
void  main(int  argc, char *argv[])
{
     int    ShmID;
     char   **ShmPTR;
     pid_t  pid;
     int    status;


     ShmID = shmget(IPC_PRIVATE, (argc-1)*sizeof(char*), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }

     ShmPTR = (char **) shmat(ShmID, NULL, 0);
     if ((char **) ShmPTR == NULL) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }     
     
     int i;
     
     //llenar la memoria compartida
     for(i=0;i<argc-1;i++){
		 ShmPTR[i] = argv[i+1]; 
	 }
     
     for(i=0;i<argc-1;i++){//mostrando..
     printf("Server has filled %s in shared memory...\n", ShmPTR[i]);
     }
     
     int j;

     (void) signal(SIGUSR1,manejadorDeSenales); 


    gettimeofday(&t0 , NULL);

     for(j=0; j < (argc-1); j++){

          pid= fork();
          if (pid == 0) {
          // tratamiento de hijo                         
          int ret = ClientProcess(ShmPTR[j]);
          exit(0);
           }                                  
          
          if (pid == -1) {

               perror("fallo en fork");
               exit(EXIT_FAILURE);

          }

 /* tratamiento del padre una vez lanzado su hijo. */
        if(pid > 0){ 
          pid = wait(NULL);

          if (pid == -1 && errno != ECHILD) {
               perror("fallo en wait");
               exit(EXIT_FAILURE);
          }

          gettimeofday(&t1, NULL);
          unsigned int ut1 = (t1.tv_sec*1000000) + t1.tv_usec;
          unsigned int ut0 = (t0.tv_sec*1000000) + t0.tv_usec;

          media += (ut1 - ut0); 

         }                   
     }//fin for que crea hijos

     
     shmdt((void *) ShmPTR);
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Parent exits... %f: \n",(media/100.0));
     

   exit(0);  
}


int ClientProcess(char *pathfile){
    
    FILE *file_d; // puntero archivo .pak
    long long int letter; // letra a obtener del archivo
    char *tam = (char *)malloc(2*sizeof(char)); 
    char size[8];         // buffer para obtener el tama#o
    char *end;            // caracter auxiliar 
    int acum=0;            
    int j=0;
    int posini=0;         // posicion inicial del nombre
    int posnom=32;        // posicion final del nombre
    int posnextfil=40;     // posicion al siguiente archivo
    char *mytam="",letra;   // buffer para el tama#o y lectura 
    char *le = (char *)malloc(2*sizeof(char));
    int wr;          
    long long int t=0;    // para el tama# auxiliar

    file_d = fopen(pathfile, "r");

    if(file_d==NULL){ //error de archivo retornamos de forma erronea
      return 1; 
    }else{           // sino existe ningun error con respecto a la apertura procedemos
 
        // Obtenemos el tama#o del .pak que se recibe por parametro 
       int sizepak = fsize(pathfile);
   
       int i;
       while(posini < sizepak){ 
    
          fseek( file_d, posini, SEEK_SET ); // me posiciono en la primera posicion del nombre del archivo
      letra = fgetc(file_d);             // se obtiene la informacion del mismo
  
      char *namefile = (char *)malloc(32*sizeof(char));
      int k=0;
  
      for(i=posini; i<posnom ;i++){ //aqui me situo para el nombre
        
        fseek( file_d, i, SEEK_SET ); //obtenemos en nombre de cada uno de los archivos
        letra = fgetc(file_d);
        namefile[k]=letra;  
        k++;
      }
  
      if(strcmp(namefile,"FIN")==0){ // caso especial donde se evalua se halla llegado al fin
        break;  
      }

      for(i=posnextfil-1;i>=posnom;i--){ //aqui me situo en el tamaño del archivo para leerlo
      
        fseek( file_d, i, SEEK_SET );
        char pl;  
        pl = fgetc(file_d);   
        sprintf(tam,"%x",pl); // convertimos lo leido en hexadecimal
    
        if(strcmp(tam,"0")!=0 && strlen(tam)==1){ // caso especial donde se tiene un cero a la izquierda
          
          mytam=concat(mytam,"0"); //se le concatena el cero
          mytam=concat(mytam,tam); // y luego el caracter leido

        }else{
        //caso especial en donde puede que se extiendan los caracteres en 8 bytes
          // para ello se verifica si el tamaño es superior 
          if(strlen(tam)> 2){
          char arr[2];
          char op = tam[strlen(tam)-1];
          char op1 = tam[strlen(tam)-2];
          char auxop[2];
          auxop[0]=op1;
          auxop[1]=op;
          mytam=concat(mytam,auxop);
          }else{      
            mytam=concat(mytam,tam);
        }
      } 
    }
      // Finalmente convertimos el string del tama#o en long long int
      t = convert(mytam);
      mytam="";
  
        // Cada archivo estara guardado en un directorio de salida      
        char *realpath, *finalname="";

        // Se obtiene la ruta absoluta actual 
        realpath = (char *)get_current_dir_name();
        //y se concatena el nombre de la carpeta a crear
        realpath = concat(realpath,"/Salida_Procesos/");

        struct stat st = {0};
  
      if (stat(realpath, &st) == -1) { //verifiquemos que no exista el directorio 
        mkdir(realpath, 0744);     // lo creamos 
      }

      // se obtiene la ruta final del archivo
      finalname = concat(finalname,realpath);
      finalname = concat(finalname,namefile);

        int fdest =  open(finalname, O_RDWR | O_CREAT | O_APPEND ,0666);

        if(fdest==-1){
          return 1;
        }
   
        if(t > 0){
          
          char buf[1],let;
          // leemos el contenido del archivo y escribimos en la nueva ruta  
          for(i=posnextfil;i<t+posnextfil;i++){
          fseek( file_d, i, SEEK_SET );
          let = fgetc(file_d);
          buf[0]= let;
          wr = write(fdest,buf,1);  
    
        }
  
        posini = t+posnextfil;
        posnom = (posini+32);
        posnextfil = (posini+40);
        memset(namefile, 0, strlen(namefile));
        memset(finalname, 0, strlen(finalname));
        memset(realpath, 0, strlen(realpath));
  
      }else{
          posini = t+posnextfil;
          posnom = (posini+32);
        posnextfil = (posini+40);
      }
    close(fdest);
    }
  } 
   fclose(file_d);
   return 0;

}

void manejadorDeSenales(pid_t process){
     kill(process,SIGTERM);
}
