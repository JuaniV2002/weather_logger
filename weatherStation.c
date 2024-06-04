#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAX_HUMIDITY 100
#define MIN_HUMIDITY 0
#define MAX_PRESSURE 3500
#define MIN_PRESSURE 900
#define MAX_WIND_DIRECTION 360
#define MIN_WIND_DIRECTION 0
#define MIN_POSSIBLE_TEMP -274
#define MAX_REGISTROS 100

// Registro encargado de almacenar todos los datos meteorológicos
typedef struct regDiario {
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

typedef struct fecha {
    int dd;
    int mm;
    int yyyy;
} TFecha;

typedef struct regDiarioC {
    TFecha ddmmyyyy;
    int tmax;
    int tmin;
    int HUM;
    int PNM;
    int DV;
    int FF;
    int PP;
    bool borrado; 
} regDiarioC;

// Dato compuesto que almacena un arreglo de registros regDiario y la cantidad de elementos en el arreglo
typedef struct {
    regDiario* reg;
    int cant;
} TData;

typedef struct {
    regDiarioC* reg;
    int cant;
} TDatac;

// Dato compuesto que indica el tipo de los nodos de la LSE usada en algunas de las funciones del programa
typedef struct nodo {
    regDiario info;
    struct nodo *next;
} TNodo;

// Perfiles de funciones principales
int alta(char name[50]);
int baja(int ddmmyyyy, char name[50]);
void modificar(int ddmmyyyy, char name[50]);
void mostrar(char name[50]);
int busqueda(TData a, long fecha, int cant);
int busquedac(TDatac a, TFecha fecha, int ini, int fin);
TNodo* temperaturaMax(char name[50]);
TNodo* velocidadViento(char name[50]);
TNodo* precipitacionMax(char name[50]);

// Perfiles de las funciones auxiliares
TNodo* crearNodo(regDiario reg);
int liberar(TNodo** head);
void swap(regDiario *x, regDiario *y);
TData arregloDeArchivo(char name[50]);
TDatac arregloDeArchivoC(char name[50]);
void quickSort(regDiario arr[], int low, int high);
int partition(regDiario arr[], int low, int high);

// Perfiles de las funciones llamadas por el switch
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
    FILE* f = fopen(nameAr, "ab");
    if (f == NULL) {
        perror("No se pudo crear el archivo");
        return 1;
    }
    fclose(f);
    
    do {
        // Menú que se mostrará cada vez que el usuario ejecute el programa
        printf("\n-----------------------------------\n");
        printf("Alta de un registro diario (1)\n");
        printf("Suprimir un registro diario (2)\n");
        printf("Modificar un registro (3)\n");
        printf("Mostrar registros (4)\n");
        printf("Buscar registro y mostrar todos sus campos (5)\n");
        printf("Listar días de máxima temperatura (6)\n");
        printf("Listar días de máxima precipitación (7)\n");
        printf("Listar días de mayor a menor velocidad de viento (8)\n");
        printf("Realizar copia de seguridad del año en curso (9)\n");
        printf("Salir (10)");
        printf("\n-----------------------------------\n");
        printf("Ingrese una opción: ");
        fflush(stdout);
        fflush(stdin);
        scanf("%d", &opcion);
        fflush(stdin);

        switch (opcion) {
            case 1:
                alta(nameAr);    
                break;
            case 2:
                printf("\nIngrese la fecha (ddmmyyyy) del registro que desea borrar: ");
                scanf(" %ld", &fecha);
                baja(fecha, nameAr);
                break;
            case 3:
                printf("\nIngrese la fecha (ddmmyyyy) del registro que desea modificar: ");
                scanf(" %ld", &fecha);
                modificar(fecha, nameAr);
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
            default: // La opción no está entre 1 y 10.
                printf("\nOpción inválida.\n");
                break;
        }
    } while (1);
    return 0;
}

// Definición de las funciones principales
int alta(char name[50]) {
    regDiario datos;
    FILE *f = fopen(name, "ab");
    
    if (f == NULL) {
        perror("No se ha podido abrir el archivo");
        return -1;
    }

    // Se realiza la carga de datos
    printf("\nIngrese la fecha de hoy (ddmmyyyy): ");
    scanf("%ld", &datos.ddmmyyyy);
    
    printf("Ingrese la temperatura máxima del día: ");
    scanf("%d", &datos.tmax);
    
    printf("Ingrese la temperatura mínima del día: ");
    scanf("%d", &datos.tmin);
    
    do {
        printf("Ingrese el índice de humedad promedio del día (entre %d y %d): ", MIN_HUMIDITY, MAX_HUMIDITY);
        scanf("%d", &datos.HUM);
    } while (datos.HUM < MIN_HUMIDITY || datos.HUM > MAX_HUMIDITY);
    
    do {
        printf("Ingrese la presión atmosférica promedio (en hectopascales) del día (entre %d y %d): ", MIN_PRESSURE, MAX_PRESSURE);
        scanf("%d", &datos.PNM);
    } while (datos.PNM < MIN_PRESSURE || datos.PNM > MAX_PRESSURE);
    
    do {
        printf("Ingrese la dirección (en grados, de %d a %d) del viento más fuerte del día: ", MIN_WIND_DIRECTION, MAX_WIND_DIRECTION);
        scanf("%d", &datos.DV);
    } while (datos.DV < MIN_WIND_DIRECTION || datos.DV > MAX_WIND_DIRECTION);
    
    printf("Ingrese la máxima velocidad (km/h) de viento del día: ");
    scanf("%d", &datos.FF);
    
    printf("Ingrese la precipitación pluvial del día (en mm): ");
    scanf("%d", &datos.PP);
    
    datos.borrado = false;

    // Los datos almacenados son escritos al archivo
    if (fwrite(&datos, sizeof(datos), 1, f) != 1) {
        perror("Error al escribir en el archivo");
        fclose(f);
        return -1;
    }

    fclose(f);
    
    printf("\nRegistro añadido con éxito!\n");
    return 0;
}

int baja(int ddmmyyyy, char name[50]) {
    regDiario datos;
    FILE *f = fopen(name, "r+b");
    int registrosBorrados = 0;
    
    if (f != NULL) {
        // Recorro el archivo hasta encontrar un registro con la misma fecha que el parámetro
        while (fread(&datos, sizeof(regDiario), 1, f) != 0) {
            if (datos.ddmmyyyy == ddmmyyyy && datos.borrado == false) {
                // Una vez encontrado, aplico el borrado lógico, escribiendo el cambio en el archivo
                datos.borrado = true;
                int pos = (ftell(f) - sizeof(regDiario));
                fseek(f, pos, SEEK_SET);
                
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
            printf("Registro borrado con éxito!\n");
        } else {
            printf("No se encontró ningún registro para borrar.\n");
        }
        
        return registrosBorrados;
    } else {
        perror("No se ha podido abrir el archivo");
        return -1;
    } 
}

void modificar(int ddmmyyyy, char name[50]) {
    regDiario nuevoReg;
    regDiario aux;
    FILE *f = fopen(name, "r+b");
    
    if (f != NULL) {
        // Recorro el archivo hasta encontrar un registro con la misma fecha que el parámetro
        while (fread(&aux, sizeof(regDiario), 1, f) != 0) {
            if (aux.ddmmyyyy == ddmmyyyy && aux.borrado == false) {
                // Una vez encontrado, reemplazo con nuevos datos
                printf("\nIngrese la nueva temperatura máxima del día: ");
                scanf("%d", &nuevoReg.tmax);
                
                printf("Ingrese la nueva temperatura mínima del día: ");
                scanf("%d", &nuevoReg.tmin);
                
                do {
                    printf("Ingrese el nuevo índice de humedad promedio del día (entre %d y %d): ", MIN_HUMIDITY, MAX_HUMIDITY);
                    scanf("%d", &nuevoReg.HUM);
                } while (nuevoReg.HUM < MIN_HUMIDITY || nuevoReg.HUM > MAX_HUMIDITY);
                
                do {
                    printf("Ingrese la nueva presión atmosférica promedio (en hectopascales) del día (entre %d y %d): ", MIN_PRESSURE, MAX_PRESSURE);
                    scanf("%d", &nuevoReg.PNM);
                } while (nuevoReg.PNM < MIN_PRESSURE || nuevoReg.PNM > MAX_PRESSURE);
                
                do {
                    printf("Ingrese la nueva dirección (en grados, de %d a %d) del viento más fuerte del día: ", MIN_WIND_DIRECTION, MAX_WIND_DIRECTION);
                    scanf("%d", &nuevoReg.DV);
                } while (nuevoReg.DV < MIN_WIND_DIRECTION || nuevoReg.DV > MAX_WIND_DIRECTION);
                
                printf("Ingrese la nueva máxima velocidad (km/h) de viento del día: ");
                scanf("%d", &nuevoReg.FF);
                
                printf("Ingrese la nueva precipitación pluvial del día (en mm): ");
                scanf("%d", &nuevoReg.PP);
                
                nuevoReg.borrado = false;
                nuevoReg.ddmmyyyy = ddmmyyyy;

                int pos = (ftell(f) - sizeof(regDiario));
                fseek(f, pos, SEEK_SET);
                
                if (fwrite(&nuevoReg, sizeof(nuevoReg), 1, f) != 1) {
                    perror("Error al escribir en el archivo");
                    fclose(f);
                    return;
                }
                
                break; // Deja de buscar una vez que se ha modificado el registro
            }
        }
        
        fclose(f);
        printf("\nRegistro modificado con éxito!\n");
    } else {
        perror("No se ha podido abrir el archivo");
    }
}

void mostrar(char name[50]) {
    FILE *f = fopen(name, "rb");
    regDiario datos;
    int i = 0;
    
    if (f != NULL) {
        printf("\nContenido del archivo %s:\n", name);
        while (fread(&datos, sizeof(regDiario), 1, f) != 0) {
            if (!datos.borrado) {
                printf("\nRegistro #%d:\n", i+1);
                printf("Fecha: %ld\n", datos.ddmmyyyy);
                printf("Temperatura máxima: %d\n", datos.tmax);
                printf("Temperatura mínima: %d\n", datos.tmin);
                printf("Humedad promedio: %d\n", datos.HUM);
                printf("Presión atmosférica promedio: %d\n", datos.PNM);
                printf("Dirección del viento: %d\n", datos.DV);
                printf("Velocidad del viento: %d\n", datos.FF);
                printf("Precipitación pluvial: %d\n", datos.PP);
                i++;
            }
        }
        fclose(f);
        if (i == 0) {
            printf("El archivo no contiene registros válidos.\n");
        }
    } else {
        perror("No se ha podido abrir el archivo");
    }
}

TNodo* temperaturaMax(char name[50]) {
    TData a = arregloDeArchivo(name);
    quickSort(a.reg, 0, a.cant - 1);

    TNodo* head = NULL;
    for (int i = 0; i < a.cant; i++) {
        if (a.reg[i].borrado == false) {
            TNodo* nuevoNodo = crearNodo(a.reg[i]);
            nuevoNodo->next = head;
            head = nuevoNodo;
        }
    }
    free(a.reg);
    return head;
}

TNodo* velocidadViento(char name[50]) {
    TData a = arregloDeArchivo(name);
    quickSort(a.reg, 0, a.cant - 1);

    TNodo* head = NULL;
    for (int i = 0; i < a.cant; i++) {
        if (a.reg[i].borrado == false) {
            TNodo* nuevoNodo = crearNodo(a.reg[i]);
            nuevoNodo->next = head;
            head = nuevoNodo;
        }
    }
    free(a.reg);
    return head;
}

TNodo* precipitacionMax(char name[50]) {
    TData a = arregloDeArchivo(name);
    quickSort(a.reg, 0, a.cant - 1);

    TNodo* head = NULL;
    for (int i = 0; i < a.cant; i++) {
        if (a.reg[i].borrado == false) {
            TNodo* nuevoNodo = crearNodo(a.reg[i]);
            nuevoNodo->next = head;
            head = nuevoNodo;
        }
    }
    free(a.reg);
    return head;
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

TNodo* crearNodo(regDiario reg) {
    TNodo* nodo = (TNodo*)malloc(sizeof(TNodo));
    if (nodo != NULL) {
        nodo->info = reg;
        nodo->next = NULL;
    }
    return nodo;
}

int liberar(TNodo** head) {
    if (head == NULL || *head == NULL) {
        return 0; // Error: Puntero nulo
    }
    TNodo* current = *head;
    TNodo* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
    return 1; // Memoria liberada correctamente
}

TDatac arregloDeArchivoC(char name[50]) {
    FILE *f = fopen(name, "rb");
    TDatac a;
    a.cant = 0;
    a.reg = NULL;
    
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        long fileSize = ftell(f);
        rewind(f);
        
        a.cant = fileSize / sizeof(regDiario);
        a.reg = (regDiarioC*)malloc(fileSize);
        
        if (a.reg != NULL) {
            fread(a.reg, sizeof(regDiarioC), a.cant, f);
        }
        
        fclose(f);
    }
    
    return a;
}

TData arregloDeArchivo(char name[50]) {
    FILE *f = fopen(name, "rb");
    TData a;
    a.cant = 0;
    a.reg = NULL;
    
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        long fileSize = ftell(f);
        rewind(f);
        
        a.cant = fileSize / sizeof(regDiario);
        a.reg = (regDiario*)malloc(fileSize);
        
        if (a.reg != NULL) {
            fread(a.reg, sizeof(regDiario), a.cant, f);
        }
        
        fclose(f);
    }
    
    return a;
}

int busqueda(TData a, long fecha, int cant) {
    for (int i = 0; i < cant; i++) {
        if (a.reg[i].ddmmyyyy == fecha) {
            return i;
        }
    }
    return -1;
}

int busquedac(TDatac a, TFecha fecha, int ini, int fin) {
    if (ini <= fin) {
        int mid = (ini + fin) / 2;
        if (a.reg[mid].ddmmyyyy.dd == fecha.dd &&
            a.reg[mid].ddmmyyyy.mm == fecha.mm &&
            a.reg[mid].ddmmyyyy.yyyy == fecha.yyyy) {
            return mid;
        }
        if ((a.reg[mid].ddmmyyyy.yyyy > fecha.yyyy) ||
            (a.reg[mid].ddmmyyyy.yyyy == fecha.yyyy && a.reg[mid].ddmmyyyy.mm > fecha.mm) ||
            (a.reg[mid].ddmmyyyy.yyyy == fecha.yyyy && a.reg[mid].ddmmyyyy.mm == fecha.mm && a.reg[mid].ddmmyyyy.dd > fecha.dd)) {
            return busquedac(a, fecha, ini, mid - 1);
        }
        return busquedac(a, fecha, mid + 1, fin);
    }
    return -1;
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
        printf("Temperatura maxima de la fecha: %d grados celsius", aux->info.tmax);
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
        printf("Maxima velocidad de viento de la fecha: %d km/h", aux->info.FF);
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
