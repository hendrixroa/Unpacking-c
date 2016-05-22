#ifndef _LIB_UNPACK

	/*MANEJO DE CADENAS*/ 
	//concatena s2 en s1
	char* concat( char *s1, char *s2 );

	//Retorna la extension de filename
	char *get_filename_ext( char *filename ); 
	
	//obtener el tamanio de los archivos
	long long fsize(char *filename);
	
	// Recibe un string y devuelve su equivalente en decimal
	long long int convert(char *num);


#endif
