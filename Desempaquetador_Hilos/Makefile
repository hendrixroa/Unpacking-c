# Este Makefile se provee como ejemplo para su modificacion. Puede utilizarlo
# en su proyecto de forma directa llenando los campos faltantes o puede
# extenderlo segun las necesidades de su solucion.
#
# GDSO 2-2014

# Coloque aqui el nombre del archivo ejecutable de su proyecto.
TARGET = Proyecto_2_Hilos
# Coloque aqui los nombres de todos los archivos compilados con extension .o
# necesarios para su proyecto.
OBJECTS = Proyecto_2_Hilos.o

LDLIBS = -lm -lpthread 
 
# Punto de entrada para make si se ejecuta sin parametros.
all: Proyecto_2_Hilos

# Esta regla compila el proyecto completo una vez que todos los archivos objeto
# ya han sido compilados.
TARGET: $(OBJECTS)
		gcc -o $(TARGET) $(OBJECTS) $(LDLIBS)
        
Proyecto_2_Hilos:	Proyecto_2_Hilos.o unpack.o
unpack.o: unpack.c unpack.h
Proyecto_2_Hilos.o: Proyecto_2_Hilos.c unpack.h

# Coloque aqui las lineas necesarias para compilar los archivos objeto que
# definio en $(OBJECTS)
# Ejemplo: Si definio arriba "OBJECTS = main.o" debe entonces agregar una linea
# como la siguiente:
# main.o: main.c

# Esta regla sustituye las banderas que se pasan al compilador por banderas
# utiles para depuracion.
debug: CFLAGS = -g -Wall -lpthread -D_DEBUG
debug: all

# Esta regla permite instalar el proyecto como un servicio de sistema asumiendo
# que el archivo init.sh tenga el formato correcto. Debe ser ejecutada como
# usuario root.
install: $(TARGET)
	cp $(TARGET) /sbin/$(TARGET)
	cp init_Hilos.sh /etc/init.d/$(TARGET)
	chmod 755 /etc/init.d/$(TARGET)
	

# Esta regla desinstala el proyecto. Debe ser ejecutada como usuario root
# despues de haber ejecutado la regla install.
uninstall:
	update-rc.d -f exceptd remove
	$(RM) /sbin/$(TARGET) /etc/init.d/$(TARGET)

# Esta regla limpia los archivos creados por la compilacion.
clean:
	$(RM) $(TARGET) *.o
