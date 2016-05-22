#include <pthread.h>
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

//concatena el contenido de s2 en s1
	char* concat(char *s1, char *s2);
	
	 //Retorna la extension de un archivo
	 char *get_filename_ext( char *filename);
	
	 long long int convert(char *num);
	
	 //Funcion que utiliza la llamada al sistema stat
	//Para obtener el tama#o del archivo
    long long fsize(char *filename);


int desempaquetado(char* nombre){

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
    long long int t=0;		// para el tama# auxiliar

    file_d = fopen(nombre, "r");

    if(file_d==NULL){ //error de archivo retornamos de forma erronea
    	return 1; 
    }else{           // sino existe ningun error con respecto a la apertura procedemos
 
    	  // Obtenemos el tama#o del .pak que se recibe por parametro	
   		 int sizepak = fsize(nombre);
   
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
  			realpath = concat(realpath,"/Salida_Hilos/");

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

	struct timeval t0 , t1;
	double media = 0.0;

void *unpack(void *threadid){
	
	char *path;
	path = (char *) threadid;

	 int ret = desempaquetado(path);

	
	
pthread_exit(0);
}


int main(int argc, char *argv[]) 
{

	pthread_t threads[(argc-1)];	
	int rc, t;

	printf("%d argc\n",argc );

	gettimeofday(&t0,NULL);
	for(t=0;t<argc-1;t++) {

		
		rc = pthread_create(&threads[t], NULL, unpack,(void *) argv[t+1]);
		if (rc) {
			return 1;
		}
	}

	for(t=0;t<argc-1;t++) {
		pthread_join(threads[t],NULL);
	}
	
	gettimeofday(&t1, NULL);
	unsigned int ut1 = (t1.tv_sec*1000000) + t1.tv_usec;
	unsigned int ut0 = (t0.tv_sec*1000000) + t0.tv_usec;


	printf("%f\n",(ut1-ut0/100.0)); 
	/*Tiempo medio en microsegundos*/

return 0;
}

