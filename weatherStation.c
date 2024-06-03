#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define max 1000
#define MIN_HUMIDITY 0
#define MAX_HUMIDITY 100
#define MIN_PRESSURE 900
#define MAX_PRESSURE 3500
#define MIN_WIND_DIRECTION 0
#define MAX_WIND_DIRECTION 360
#define MIN_POSSIBLE_TEMP -274
#define MAX_REGISTROS 100

// registro encargado de almacenar todos los datos meteorologicos
typedef struct regDiario{
    long ddmmyyyy;
    int tmax;
    int tmin;
    int HUM;
    int PNM;
    int DV;
    int FF;
    int PP;
    bool borrado; 
} regDiario;

typedef struct fecha{
    int dd;
    int mm;
    int yyyy;
}TFecha;

typedef struct regDiarioC{
    TFecha ddmmyyyy;
    int tmax;
    int tmin;
    int HUM;
    int PNM;
    int DV;
    int FF;
    int PP;
    bool borrado; 
}regDiarioC;

//dato compuesto que almacena un arreglo de registros regDiario y la cantidad de elementos en el arreglo
typedef struct {
    regDiario* reg;
    int cant;
} TData;

typedef struct {
    regDiarioC* reg;
    int cant;
} TDatac;

//dato compuesto que indica el tipo de los nodos de la LSE usada en algunas de las funciones del programa
typedef struct nodo {
    regDiario info;
    struct nodo *next;
} TNodo;

//perfiles de funciones principales
int alta(char name[50]);
int baja(int ddmmyyyy,char name[50]);
void modificar(int ddmmyyyy,char name[50]);
void mostrar(char name[50]);
int busqueda(TData a, long fecha, int cant);
int busquedac(TDatac a,TFecha fecha,int ini, int fin);
TNodo* temperaturaMax(char name[50]);
TNodo* velocidadViento(char name[50]);
TNodo* precipitacionMax(char name[50]);

//perfiles de las funciones auxiliares
TNodo* crearNodo(regDiario reg);
int liberar(TNodo** head);
void swap(regDiario *x, regDiario *y);
TData arregloDeArchivo(char name[50]);
TDatac arregloDeArchivoC(char name[50]);
void quickSort(regDiario arr[], int low, int high);
int partition(regDiario arr[], int low, int high);

//perfiles de las funciones llamadas por el switch
void opcion5(char name[50]);
void opcion6(char name[50]);
void opcion7(char name[50]);
void opcion8(char name[50]);
void opcion9(char name[50]);

int main() {
    char nameAr[50];
    long fecha;
    int opcion;

    printf("Ingrese el nombre del archivo: ");
    scanf(" %s", nameAr);
    FILE* f = fopen(nameAr,"ab");
    fclose(f);
    do {
        //menu que se mostrara cada vez que el usuario ejecute el programa
        printf("\n-----------------------------------\n");
        printf("Alta de un registro diario (1)\n");
        printf("Suprimir un registro diario (2)\n");
        printf("Modificar un registro (3)\n");
        printf("Mostrar registros (4)\n");
        printf("Buscar registro y mostrar todos sus campos (5)\n");
        printf("Listar dias de maxima temperatura (6)\n");
        printf("Listar dias de maxima precipitacion (7)\n");
        printf("Listar dias de mayor a menor velocidad de viento (8)\n");
        printf("Realizar copia de seguridad del año en curso (9)\n");
        printf("Salir (10)");
        printf("\n-----------------------------------\n");
        printf("Ingrese una opcion: ");
        fflush(stdout); fflush(stdin);
        scanf("%d", &opcion);
        fflush(stdin);

        switch (opcion){
            case 1:
                alta(nameAr);    
                break;

            case 2:
                printf("\nIngrese la fecha (ddmmyyyy) del registro que desea borrar: ");
                scanf(" %ld", &fecha);
                baja(fecha,nameAr);
                break;

            case 3:
                printf("\n Ingrese la fecha (ddmmyyyy) del registro que desea modificar: ");
                scanf(" %ld", &fecha);
                modificar(fecha,nameAr);
                break;

            case 4: 
                mostrar(nameAr);
                break;

            case 5:
                opcion5(nameAr);
                break;

            case 6:
                opcion6(nameAr);
                break;

            case 7:
                opcion7(nameAr);
                break;

            case 8:
                opcion8(nameAr);
                break;
            
            case 9:
                opcion9(nameAr);
                break;

            case 10:
                return 0;
                break;

            default: // la opcion no esta entre 1 y 10.
                printf("\nOpcion invalida.\n");
                break;
        }
    } while (1);
    return 0;
}

//definicion de las funciones principales
int alta(char name[50]){
    regDiario datos;
    FILE *f = fopen(name, "a");
    
    if (f == NULL) {
        perror("No se ha podido abrir el archivo");
        return -1;
    }

    //se realiza la carga de datos
    printf("\nIngrese la fecha de hoy: ");
    scanf("%ld", &datos.ddmmyyyy);
    
    printf("Ingrese la temperatura maxima del dia: ");
    scanf("%d", &datos.tmax);
    
    printf("Ingrese la temperatura minima del dia: ");
    scanf("%d", &datos.tmin);
    
    do {
        printf("Ingrese el indice de humedad promedio del dia (entre %d y %d): ", MIN_HUMIDITY, MAX_HUMIDITY);
        scanf("%d", &datos.HUM);
    } while (datos.HUM < MIN_HUMIDITY || datos.HUM > MAX_HUMIDITY);
    
    do {
        printf("Ingrese la presion atmosferica promedio (en hectopascales) del dia (entre %d y %d): ", MIN_PRESSURE, MAX_PRESSURE);
        scanf("%d", &datos.PNM);
    } while (datos.PNM < MIN_PRESSURE || datos.PNM > MAX_PRESSURE);
    
    do {
        printf("Ingrese la direccion (en grados, de %d a %d) del viento mas fuerte del dia: ", MIN_WIND_DIRECTION, MAX_WIND_DIRECTION);
        scanf("%d", &datos.DV);
    } while (datos.DV < MIN_WIND_DIRECTION || datos.DV > MAX_WIND_DIRECTION);
    
    printf("Ingrese la maxima velocidad (km/h) de viento del dia: ");
    scanf("%d", &datos.FF);
    
    printf("Ingrese la precipitacion pluvial del dia (en mm): ");
    scanf("%d", &datos.PP);
    
    datos.borrado = false;

    //los datos almacenados son escritos al archivo
    if (fwrite(&datos, sizeof(datos), 1, f) != 1) {
        perror("Error al escribir en el archivo");
        fclose(f);
        return -1;
    }

    fclose(f);
    
    printf("\nRegistro añadido con exito!\n");
    return 0;
}

int baja(int ddmmyyyy,char name[50]){
    regDiario datos;
    FILE *f = fopen(name, "r+b");
    int registrosBorrados = 0;
    
    if (f != NULL){
        //recorro el archivo hasta encontrar un registro con la misma fecha que el parametro
        while (fread(&datos, sizeof(regDiario), 1, f) != 0) {
            if (datos.ddmmyyyy == ddmmyyyy && datos.borrado == false) {
                //una vez encontrado, aplico el borrado logico, escribiendo el cambio en el archivo
                datos.borrado = true;
                int pos = (ftell(f) - (sizeof(regDiario)));
                fseek(f,pos,SEEK_SET);
                
                if (fwrite(&datos, sizeof(datos), 1, f) != 1) {
                    perror("Error al escribir en el archivo");
                    fclose(f);
                    return -1;
                }
                
                registrosBorrados++;
                break; // Deja de buscar una vez que se ha borrado el registro
            }
        }
        
        fclose(f);
        
        if (registrosBorrados > 0) {
            printf("Registro borrado con exito!\n");
        } else {
            printf("No se encontró ningún registro para borrar.\n");
        }
        
        return registrosBorrados;
    } else {
        perror("No se ha podido abrir el archivo");
        return -1;
    } 
}

void modificar(int ddmmyyyy,char name[50]){
    regDiario nuevoReg;
    regDiario aux;
    FILE *f = fopen(name, "r+b");
    
    if(f != NULL){
        //recorro el archivo hasta encontrar un resgitro con la misma fecha que el parametro
        while (fread(&aux, sizeof(regDiario), 1, f) != 0) {
            if (aux.ddmmyyyy == ddmmyyyy) {
                //realizo la carga de los datos modificados en una variable auxiliar
                printf("Ingrese la fecha de hoy: \n");
                scanf("%ld", &nuevoReg.ddmmyyyy);
            
                printf("Ingrese la temperatura maxima del dia: \n");
                scanf("%d", &nuevoReg.tmax);
            
                printf("Ingrese la temperatura minima del dia: \n");
                scanf("%d", &nuevoReg.tmin);
            
                do {
                    printf("Ingrese el indice de humedad promedio del dia (entre %d y %d): \n", MIN_HUMIDITY, MAX_HUMIDITY);
                    scanf("%d", &nuevoReg.HUM);
                } while (nuevoReg.HUM < MIN_HUMIDITY || nuevoReg.HUM > MAX_HUMIDITY);
            
                do {
                    printf("Ingrese la presion atmosferica promedio (PNM) del dia (entre %d y %d): \n", MIN_PRESSURE, MAX_PRESSURE);
                    scanf("%d", &nuevoReg.PNM);
                } while (nuevoReg.PNM < MIN_PRESSURE || nuevoReg.PNM > MAX_PRESSURE);
            
                do {
                    printf("Ingrese la direccion (en grados, de %d a %d) del viento mas fuerte del dia: \n", MIN_WIND_DIRECTION, MAX_WIND_DIRECTION);
                    scanf("%d", &nuevoReg.DV);
                } while (nuevoReg.DV < MIN_WIND_DIRECTION || nuevoReg.DV > MAX_WIND_DIRECTION);
            
                printf("Ingrese la maxima velocidad de viento del dia: \n");
                scanf("%d", &nuevoReg.FF);
            
                printf("Ingrese la precipitacion pluvial del dia: \n");
                scanf("%d", &nuevoReg.PP);
                
                nuevoReg.borrado = false;

                //sobreescribo el registro con los datos antiguos con los nuevos datos
                int pos = (ftell(f) - (sizeof(regDiario)));
                fseek(f, pos, SEEK_SET);
                
                if (fwrite(&nuevoReg, sizeof(nuevoReg), 1, f) != 1) {
                    perror("Error al escribir en el archivo");
                    fclose(f);
                    return;
                }
                
                printf("Registro modificado con exito!\n");
                fclose(f);
                return;
            }
        }
    } else {
        perror("No se ha podido abrir el archivo");
    }
}

void mostrar(char name[50]){
    regDiario aux;
    FILE *f = fopen(name, "rb");
    
    if (f != NULL){
        //recorro el archivo mostrando los registros si su campo "borrado" es false
        while (fread(&aux, sizeof(regDiario), 1, f) != 0) {
            if (aux.borrado == false) {
                printf("\n-----------------------------------\n");
                printf("Fecha del registro: %ld\n", aux.ddmmyyyy);
                printf("Temperatura maxima de la fecha: %d grados celsius\n", aux.tmax);
                printf("Temperatura minima de la fecha: %d grados celsius\n", aux.tmin);
                printf("Humedad promedio de la fecha: %d%%\n", aux.HUM);
                printf("Presion atmosferica de la fecha: %d hectopascales\n", aux.PNM);
                printf("Direccion del viento con mayor intensidad (de 0 a 360): %d grados\n", aux.DV);
                printf("Maxima velocidad de viento de la fecha: %d km/h\n", aux.FF);
                printf("Precipitacion pluvial de la fecha: %d mm\n", aux.PP);
                printf("\n-----------------------------------\n");
            }
        }
        fclose(f);	
    } else {
        perror("No se ha podido abrir el archivo");
    }
}

TNodo* temperaturaMax(char name[50]){
    FILE* f;
    TNodo *aux, *aux2, *aux3;
    regDiario reg;
    int maxTemp = MIN_POSSIBLE_TEMP;

    f = fopen(name, "rb");
    
    if (f == NULL) {
        printf("El archivo esta vacio.");
        return NULL;
    }

    aux2 = NULL;
        
    //recorro el archivo y almaceno la temperatura maxima y creo la lista enlazada
    while (fread(&reg, sizeof(regDiario), 1, f) != 0) {
        if (reg.borrado == false) {
            if (reg.tmax > maxTemp) {
                maxTemp = reg.tmax;
                // liberar la lista anterior
                while (aux2 != NULL) {
                    aux = aux2;
                    aux2 = aux2->next;
                    free(aux);
                }
                aux = crearNodo(reg);
                aux->info = reg;
                aux2 = aux;
                aux3 = aux2;
            } else if (reg.tmax == maxTemp){
                aux = crearNodo(reg);
                aux->info = reg;
                aux3->next = aux;
                aux3 = aux;
            }
        }
    }

    fclose(f);
    //retorno la cabeza de la lista con las temperaturas maximas
    return aux2;
}

TNodo* velocidadViento(char name[50]) {
    TData res;
    int i;
    TNodo *aux, *aux2, *aux3;

    //en res, esta guardado todo el archivo pasado a un arreglo
    res = arregloDeArchivo(name);
    if (res.cant == 0) {
        printf("Error: res.reg is empty.\n");
        return NULL;
    }

    //aplico quickSort sobre el arreglo para ordenarlo
    quickSort(res.reg, 0, res.cant - 1);

    aux2 = NULL;

    //paso los datos del arreglo a una LSE para mostrar los dias con la maxima velocidad de viento
    for (i = 0; i < res.cant; i++) {
        aux = crearNodo(res.reg[i]);
        aux->info = res.reg[i];
        if (aux2 == NULL) {
            aux2 = aux;
            aux3 = aux2;
        } else {
            aux3->next = aux;
            aux3 = aux3->next;
        }
    }

    //libero la memoria del arreglo
    free(res.reg);

    //retorno la cabeza de la lista de las velocidades del viento
    return aux2;
}

TNodo* precipitacionMax (char name[50]){ 
    TData res;
    int i;
    TNodo *aux, *aux2, *aux3;

    //en res, esta guardado todo el archivo pasado a un arreglo
    res = arregloDeArchivo(name);
    if (res.cant == 0) {
        printf("Error: res.reg is empty.\n");
        return NULL;
    }

    //aplico quickSort sobre el arreglo para ordenarlo
    quickSort(res.reg, 0, res.cant - 1);

    aux2 = NULL;

    //paso los datos del arreglo a una LSE para mostrar los dias con la maxima precipitacion
    for (i = res.cant - 1; i >= 0; i--) {
        aux = crearNodo(res.reg[i]);
        aux->info = res.reg[i];
        if (aux2 == NULL) {
            aux2 = aux;
            aux3 = aux2;
        }else{
            aux3->next = aux;
            aux3 = aux3->next;
        }
    }

    //libero la memoria del arreglo
    free(res.reg);

    //retorno la cabeza de la lista con las precipitaciones maximas
    return aux2;
}

//definicion de las funciones auxiliares

void quickSort (regDiario arr[], int low, int high) {
    if (low < high) {
        /* pi is partitioning index, arr[p] is now at right place */
        int pi = partition(arr, low, high);

        // Separately sort elements before partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int partition (regDiario arr[], int low, int high) {
    int pivot = arr[high].FF; // pivot
    int i = (low - 1); // Index of smaller element

    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than or equal to pivot
        if (arr[j].FF <= pivot) {
            i++; // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void swap (regDiario* a, regDiario* b) {
    regDiario t = *a;
    *a = *b;
    *b = t;
}

TNodo* crearNodo (regDiario reg) {
    //funcion que se encarga de crear un nodo de tipo TNodo, que almacenara un registro regDiario
    TNodo* a;

    a = (TNodo *) malloc(sizeof(TNodo));

    if (a == NULL) {
        printf("Error al asignar memoria para el nuevo nodo.");
        return NULL;
    }

    a->info = reg;
    a->next = NULL;

    return a;
}

int liberar(TNodo** head) {
    TNodo* current = *head;
    TNodo* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;

    return 1; // Return 1 if the memory was successfully freed
}

TDatac arregloDeArchivoC (char name[50]) {
    TDatac aux;
    regDiario reg;
    int i;
    FILE *f;

    f = fopen(name,"rb");

    if (f == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return aux;
    }

    // Allocate memory for aux.reg
    aux.reg = (regDiarioC*) malloc(MAX_REGISTROS * sizeof(regDiario));
    if (aux.reg == NULL) {
        printf("No se pudo asignar memoria.\n");
        fclose(f);
        return aux;
    }

    i = 0;

    //leo los registros del archivo y los paso a un arreglo
    while (i < MAX_REGISTROS && fread(&reg, sizeof(reg), 1, f) == 1 && reg.borrado == false) {
        aux.reg[i].ddmmyyyy.dd = reg.ddmmyyyy/1000000;
        aux.reg[i].ddmmyyyy.mm = (reg.ddmmyyyy/10000) - (aux.reg[i].ddmmyyyy.dd*100);
        aux.reg[i].ddmmyyyy.yyyy = reg.ddmmyyyy - (aux.reg[i].ddmmyyyy.mm*10000);
        aux.reg[i].tmax = reg.tmax;
        aux.reg[i].tmin = reg.tmin;
        aux.reg[i].HUM = reg.HUM;
        aux.reg[i].PNM = reg.PNM;
        aux.reg[i].DV = reg.DV;
        aux.reg[i].FF = reg.FF;
        aux.reg[i].PP = reg.PP;
        i++;
    }

    if (ferror(f)) {
        printf("Error al leer el archivo.\n");
    }

    aux.cant = i;
    fclose(f);
    return aux;
}

TData arregloDeArchivo (char name[50]) {
    TData aux;
    regDiario reg;
    int i;
    FILE *f;

    f = fopen(name,"rb");

    if (f == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return aux;
    }

    // Allocate memory for aux.reg
    aux.reg = (regDiario*) malloc(MAX_REGISTROS * sizeof(regDiario));
    if (aux.reg == NULL) {
        printf("No se pudo asignar memoria.\n");
        fclose(f);
        return aux;
    }

    i = 0;

    //leo los registros del archivo y los paso a un arreglo
    while (i < MAX_REGISTROS && fread(&reg, sizeof(reg), 1, f) == 1 && reg.borrado == false) {
        aux.reg[i] = reg;
        i++;
    }

    if (ferror(f)) {
        printf("Error al leer el archivo.\n");
    }

    aux.cant = i;
    fclose(f);
    return aux;
}

int busqueda (TData a, long fecha, int i) {
    if (i < 0) {
        return -1;
    } else {
        //si fecha es igual a la fecha del registro actual, devuelvo su indice
        if (a.reg[i].ddmmyyyy == fecha) {
            return i;
        } else {
            //sino, llamo recursivamente decrementando a i para acotar la busqueda
            return busqueda(a, fecha, i - 1);
        }
    }
} 

int busquedac (TDatac a, TFecha fecha, int ini, int fin) {
    if (ini > fin) return -1;

    int indiceMitad = floor((ini + fin) / 2);

    TFecha fechaMitad = a.reg[indiceMitad].ddmmyyyy;
    if ((fecha.dd == fechaMitad.dd) && (fecha.mm == fechaMitad.mm)) {
        return indiceMitad + 1;
    }

    if ((fecha.mm < fechaMitad.mm) || ((fecha.dd < fechaMitad.dd) && (fecha.mm == fechaMitad.mm))) {
        fin = indiceMitad - 1;
    } else {
        ini = indiceMitad + 1;
    }
    return busquedac(a,fecha,ini,fin);
} 

//definicion de las funciones llamdas por el switch
void opcion5 (char name[50]) {

    TDatac regArc;
    TFecha fechac;
    regDiario datos;
    FILE* g = NULL;

    printf("\n Ingrese el dia del registro(1-31):");
    scanf(" %d", &fechac.dd);  
    printf("\n Ingrese el mes del registro(1-12):");
    scanf(" %d", &fechac.mm);  
    printf("\n Ingrese el año del registro(2022):");
    scanf(" %d", &fechac.yyyy);  

    //en a guardo el archivo en forma de arreglo
    regArc = arregloDeArchivoC(name);
    //aplico la busqueda sobre el arrelgo con el archivo
    int p = busquedac(regArc, fechac, 0, regArc.cant-1);
    printf("%d", p);
    
    if (p != -1) {
        g = fopen(name,"rb");
        if (g == NULL) {
            printf("No se pudo abrir el archivo.\n");
            return;
        }
        fseek(g, (p * (sizeof(regDiario)) - (sizeof(regDiario))), SEEK_SET);
        
        //muestro los datos del registro que se corresponden a la fecha indicada
        if (fread(&datos, sizeof(regDiario), 1, g) != 1) {
            printf("Error al leer el archivo.\n");
        } else {
            printf("\n-----------------------------------\n");
            printf("Fecha del registro: %ld\n", datos.ddmmyyyy);
            printf("Temperatura maxima de la fecha: %d grados celsius\n", datos.tmax);
            printf("Temperatura minima de la fecha: %d grados celsius\n", datos.tmin);
            printf("Humedad promedio de la fecha: %d%%\n", datos.HUM);
            printf("Presion atmosferica de la fecha: %d hectopascales\n", datos.PNM);
            printf("Direccion del viento con mayor intensidad (de 0 a 360): %d grados\n", datos.DV);
            printf("Maxima velocidad de viento de la fecha: %d km/h\n", datos.FF);
            printf("Precipitacion pluvial de la fecha: %d mm", datos.PP);
            printf("\n-----------------------------------\n");
        }
    } else {
        printf("El registro no existe.");
    }
    if (g != NULL) {
        fclose(g);
    }
}

void opcion6(char name[50]){
    //en un puntero, guardo la lista con las temperaturas maximas
    TNodo* listTempMax = temperaturaMax(name);  

    if (listTempMax == NULL) {
        printf("No se pudo generar la lista de temperaturas maximas.\n");
        return;
    }

    //le paso la direccion de memoria de la cabeza de la lista de temperaturas maximas a un puntero auxiliar
    TNodo* aux = listTempMax;

    //muestro todas las fechas de las temperaturas maximas          
    while(aux != NULL){
        printf("\n-----------------------------------\n");
        printf("Fecha del registro con maxima temperatura: %ld\n", aux->info.ddmmyyyy);
        printf("\n-----------------------------------\n");
        aux = aux->next;
    } 

    if (!liberar(&listTempMax)) {
        printf("Error al liberar la memoria de la lista.\n");
    }
}

void opcion7(char name[50]){
    //en un puntero, guardo la lista con las maximas precipitaciones (ordenadas, de menor a mayor)
    TNodo* listPreciMax = precipitacionMax(name);

    if (listPreciMax == NULL) {
        printf("No se pudo generar la lista de precipitaciones maximas.\n");
        return;
    }

    //en un puntero auxiliar, guardo la cabeza de la lista de maximas precipitaciones
    TNodo* aux = listPreciMax;
    int i = 1;

    //muestro las primeras 10 mayores precipitaciones            
    while((aux != NULL) && (i <= 10)){
        printf("\n-----------------------------------\n");
        printf("Fecha del registro: %ld\n", aux->info.ddmmyyyy);
        printf("Precipitacion pluvial de la fecha: %d mm", aux->info.PP);
        printf("\n-----------------------------------\n");
        aux = aux->next;
        i++;
    } 

    if (!liberar(&listPreciMax)) {
        printf("Error al liberar la memoria de la lista.\n");
    }
}

void opcion8(char name[50]){
    //en un puntero, guardo la lista de la velocidad del viento (ordenada, de mayor a menor)
    TNodo* listVientoVel = velocidadViento(name);

    if (listVientoVel == NULL) {
        printf("No se pudo generar la lista de velocidades de viento.\n");
        return;
    }

    //en un puntero auxiliar, guardo la cabeza de la lista de la velocidad del viento
    TNodo* aux = listVientoVel;
    int i = 1;

    //muestro las primeras 10 mayores velocidades de viento de la lista            
    while((aux != NULL) && (i <= 10)){
        printf("\n-----------------------------------\n");
        printf("Fecha del registro: %ld\n", aux->info.ddmmyyyy);
        printf("Direccion del viento con mayor intensidad (de 0 a 360): %d grados\n", aux->info.DV);
        printf("Maxima velocidad de viento de la fecha: %d km/h\n", aux->info.FF);
        printf("\n-----------------------------------\n");
        aux = aux->next;
        i++;
    } 

    if (!liberar(&listVientoVel)) {
        printf("Error al liberar la memoria de la lista.\n");
    }
}

void opcion9(char name[50]){
    //abro ambos archivos (el actual, y el cual en donde voy a guardar la copia de seguridad)
    char name2[50];
    strcpy(name2, "copia_seguridad_");
    strcat(name2,name);

    FILE* f = fopen(name,"rb");
    if (f == NULL) {
        printf("No se pudo abrir el archivo %s.\n", name);
        return;
    }

    FILE* g = fopen(name2,"wb+");
    if (g == NULL) {
        printf("No se pudo abrir el archivo %s.\n", name2);
        fclose(f);
        return;
    }

    regDiario datos;

    //a los registros que no estan borrados (borrado == false), los guarda en el nuevo archivo
    while(fread(&datos,sizeof(regDiario),1,f) != 0){
        if(datos.borrado == false){
            fwrite(&datos,sizeof(regDiario),1,g);
        }
    }
    fclose(f);
    fclose(g);
}
