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
#define MAX_RECORDS 100

typedef struct Date {
    int dd;
    int mm;
    int yyyy;
} Date;

// Record that stores all daily meteorological data
typedef struct DailyRecord {
    Date date;           // DD/MM/YYYY
    int tempMax;
    int tempMin;
    int humidity;
    int pressure;
    int windDir;         // 0-360 degrees
    int windSpeed;       // km/h
    int precipitation;   // mm
    bool deleted;        // logical deletion flag
} DailyRecord;

// No separate C variant needed when working with CSV; keep a single DailyRecord type.

// Composite type that stores an array of DailyRecord and the number of elements
typedef struct {
    DailyRecord* rec;
    int count;
} DataArray;

// Global in-memory store and dirty flag for unsaved changes
static DataArray g_data = { NULL, 0 };
static bool g_dirty = false;
static char g_filename[256] = "weather.csv"; // default CSV file name

// Node type for the singly linked list used in some program functions
typedef struct Node {
    DailyRecord info;
    struct Node *next;
} Node;

// CSV I/O
DataArray loadFromCsv(const char* filename);
int saveToCsv(const char* filename, const DataArray a);

// Helpers on in-memory data
int findIndexByDate(const DataArray* a, Date date);
Node* maxTemperatureRecords(void);
Node* windSpeedRecords(void);
Node* maxPrecipitationRecords(void);

// Helper function prototypes
Node* createNode(DailyRecord rec);
int freeList(Node** head);
void swap(DailyRecord *x, DailyRecord *y);
void quickSort(DailyRecord arr[], int low, int high);
int partition(DailyRecord arr[], int low, int high);

// Menu actions (mutate g_data and g_dirty)
int addRecord(void);
int deleteRecord(void);
void updateRecord(void);
void showRecords(void);

// Prototypes of functions called by the switch menu
void option5_showRecordByDate(void);
void option6_listMaxTemperature(void);
void option7_listMaxPrecipitation(void);
void option8_listTop10WindSpeeds(void);

int main() {
    int option;

    // Start with empty in-memory data; users can load CSV via option 9

    do {
        // Menu
        printf("\n-----------------------------------\n");
        printf("Add a daily record (1)\n");
        printf("Delete a daily record (2)\n");
        printf("Update a record (3)\n");
        printf("Show all records (4)\n");
        printf("Search record by date and show details (5)\n");
        printf("List days with maximum temperature (6)\n");
        printf("List days with maximum precipitation (7)\n");
    printf("List days by wind speed (descending) (8)\n");
    printf("Load data from CSV file (9)\n");
    printf("Exit (10)");
        printf("\n-----------------------------------\n");
        printf("Choose an option: ");
        fflush(stdout);
        if (scanf("%d", &option) != 1) {
            // invalid input: clear and continue
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        switch (option) {
            case 1:
                addRecord();    
                break;
            case 2:
                deleteRecord();
                break;
            case 3:
                updateRecord();
                break;
            case 4: 
                showRecords();
                break;
            case 5:
                option5_showRecordByDate();
                break;
            case 6:
                option6_listMaxTemperature();
                break;
            case 7:
                option7_listMaxPrecipitation();
                break;
            case 8:
                option8_listTop10WindSpeeds();
                break;
            case 9: {
                char fname[256];
                printf("\nEnter CSV file name to load: ");
                scanf(" %255s", fname);
                DataArray loaded = loadFromCsv(fname);
                // replace current data
                free(g_data.rec);
                g_data = loaded;
                strncpy(g_filename, fname, sizeof(g_filename)-1);
                g_filename[sizeof(g_filename)-1] = '\0';
                g_dirty = false;
                printf("Loaded %d record(s) from %s.\n", g_data.count, g_filename);
                break;
            }
            case 10: {
                if (g_dirty) {
                    char ans;
                    printf("\nSave changes to CSV file '%s'? (y/n): ", g_filename);
                    scanf(" %c", &ans);
                    if (ans == 'y' || ans == 'Y') {
                        if (saveToCsv(g_filename, g_data) != 0) {
                            printf("Error saving to CSV.\n");
                        } else {
                            printf("Saved successfully.\n");
                            g_dirty = false;
                        }
                    }
                }
                // free memory
                free(g_data.rec);
                return 0;
            }
            default:
                printf("\nInvalid option.\n");
                break;
        }
    } while (1);
}

// Main functions (operate on in-memory g_data)
int addRecord(void) {
    DailyRecord rec;

    // Data entry
    printf("\nEnter date (DD/MM/YYYY): ");
    if (scanf("%d/%d/%d", &rec.date.dd, &rec.date.mm, &rec.date.yyyy) != 3) {
        printf("Invalid date format.\n");
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return -1;
    }
    
    printf("Enter the day's maximum temperature: ");
    scanf("%d", &rec.tempMax);
    
    printf("Enter the day's minimum temperature: ");
    scanf("%d", &rec.tempMin);
    
    do {
        printf("Enter the day's average humidity index (%d to %d): ", MIN_HUMIDITY, MAX_HUMIDITY);
        scanf("%d", &rec.humidity);
    } while (rec.humidity < MIN_HUMIDITY || rec.humidity > MAX_HUMIDITY);
    
    do {
        printf("Enter the day's average atmospheric pressure in hPa (%d to %d): ", MIN_PRESSURE, MAX_PRESSURE);
        scanf("%d", &rec.pressure);
    } while (rec.pressure < MIN_PRESSURE || rec.pressure > MAX_PRESSURE);
    
    do {
        printf("Enter the direction (degrees, %d to %d) of the strongest wind: ", MIN_WIND_DIRECTION, MAX_WIND_DIRECTION);
        scanf("%d", &rec.windDir);
    } while (rec.windDir < MIN_WIND_DIRECTION || rec.windDir > MAX_WIND_DIRECTION);
    
    printf("Enter the day's maximum wind speed (km/h): ");
    scanf("%d", &rec.windSpeed);
    
    printf("Enter the day's precipitation (mm): ");
    scanf("%d", &rec.precipitation);
    
    rec.deleted = false;

    // append to g_data
    DailyRecord* newBuf = realloc(g_data.rec, sizeof(DailyRecord) * (g_data.count + 1));
    if (!newBuf) {
        printf("Memory allocation failed.\n");
        return -1;
    }
    g_data.rec = newBuf;
    g_data.rec[g_data.count] = rec;
    g_data.count++;
    g_dirty = true;

    printf("\nRecord added successfully!\n");
    return 0;
}

int deleteRecord(void) {
    Date date;
    printf("\nEnter the record date to delete (DD/MM/YYYY): ");
    if (scanf("%d/%d/%d", &date.dd, &date.mm, &date.yyyy) != 3) {
        printf("Invalid date format.\n");
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return -1;
    }

    int idx = findIndexByDate(&g_data, date);
    if (idx >= 0 && g_data.rec[idx].deleted == false) {
        g_data.rec[idx].deleted = true;
        g_dirty = true;
        printf("Record deleted successfully!\n");
        return 1;
    }
    printf("No record found to delete.\n");
    return 0;
}

void updateRecord(void) {
    Date date;
    printf("\nEnter the record date to update (DD/MM/YYYY): ");
    if (scanf("%d/%d/%d", &date.dd, &date.mm, &date.yyyy) != 3) {
        printf("Invalid date format.\n");
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return;
    }

    int idx = findIndexByDate(&g_data, date);
    if (idx < 0 || g_data.rec[idx].deleted) {
        printf("Record not found.\n");
        return;
    }

    DailyRecord updated = g_data.rec[idx];

    printf("\nEnter new maximum temperature: ");
    scanf("%d", &updated.tempMax);
    
    printf("Enter new minimum temperature: ");
    scanf("%d", &updated.tempMin);
    
    do {
        printf("Enter new average humidity index (%d to %d): ", MIN_HUMIDITY, MAX_HUMIDITY);
        scanf("%d", &updated.humidity);
    } while (updated.humidity < MIN_HUMIDITY || updated.humidity > MAX_HUMIDITY);
    
    do {
        printf("Enter new average atmospheric pressure in hPa (%d to %d): ", MIN_PRESSURE, MAX_PRESSURE);
        scanf("%d", &updated.pressure);
    } while (updated.pressure < MIN_PRESSURE || updated.pressure > MAX_PRESSURE);
    
    do {
        printf("Enter new direction (degrees, %d to %d) of the strongest wind: ", MIN_WIND_DIRECTION, MAX_WIND_DIRECTION);
        scanf("%d", &updated.windDir);
    } while (updated.windDir < MIN_WIND_DIRECTION || updated.windDir > MAX_WIND_DIRECTION);
    
    printf("Enter new maximum wind speed (km/h): ");
    scanf("%d", &updated.windSpeed);
    
    printf("Enter new precipitation (mm): ");
    scanf("%d", &updated.precipitation);
    
    g_data.rec[idx] = updated;
    g_dirty = true;
    printf("\nRecord updated successfully!\n");
}

void showRecords(void) {
    int shown = 0;
    printf("\nRecords in memory:\n");
    for (int i = 0; i < g_data.count; i++) {
        DailyRecord rec = g_data.rec[i];
        if (!rec.deleted) {
            printf("\nRecord #%d:\n", shown + 1);
            printf("Date: %02d/%02d/%04d\n", rec.date.dd, rec.date.mm, rec.date.yyyy);
            printf("Max temperature: %d\n", rec.tempMax);
            printf("Min temperature: %d\n", rec.tempMin);
            printf("Average humidity: %d\n", rec.humidity);
            printf("Average atmospheric pressure: %d\n", rec.pressure);
            printf("Wind direction: %d\n", rec.windDir);
            printf("Wind speed: %d\n", rec.windSpeed);
            printf("Precipitation: %d\n", rec.precipitation);
            shown++;
        }
    }
    if (shown == 0) {
        printf("No valid records to show.\n");
    }
}

Node* maxTemperatureRecords(void) {
    // Create a copy of active records, sort (currently by windSpeed per partition) and build list
    int nActive = 0;
    for (int i = 0; i < g_data.count; i++) if (!g_data.rec[i].deleted) nActive++;
    if (nActive == 0) return NULL;
    DailyRecord* tmp = (DailyRecord*)malloc(sizeof(DailyRecord) * nActive);
    if (!tmp) return NULL;
    int k = 0;
    for (int i = 0; i < g_data.count; i++) if (!g_data.rec[i].deleted) tmp[k++] = g_data.rec[i];
    quickSort(tmp, 0, nActive - 1);

    Node* head = NULL;
    for (int i = 0; i < nActive; i++) {
        Node* newNode = createNode(tmp[i]);
        newNode->next = head;
        head = newNode;
    }
    free(tmp);
    return head;
}

Node* windSpeedRecords(void) {
    int nActive = 0;
    for (int i = 0; i < g_data.count; i++) if (!g_data.rec[i].deleted) nActive++;
    if (nActive == 0) return NULL;
    DailyRecord* tmp = (DailyRecord*)malloc(sizeof(DailyRecord) * nActive);
    if (!tmp) return NULL;
    int k = 0;
    for (int i = 0; i < g_data.count; i++) if (!g_data.rec[i].deleted) tmp[k++] = g_data.rec[i];
    quickSort(tmp, 0, nActive - 1);

    Node* head = NULL;
    for (int i = 0; i < nActive; i++) {
        Node* newNode = createNode(tmp[i]);
        newNode->next = head;
        head = newNode;
    }
    free(tmp);
    return head;
}

Node* maxPrecipitationRecords(void) {
    int nActive = 0;
    for (int i = 0; i < g_data.count; i++) if (!g_data.rec[i].deleted) nActive++;
    if (nActive == 0) return NULL;
    DailyRecord* tmp = (DailyRecord*)malloc(sizeof(DailyRecord) * nActive);
    if (!tmp) return NULL;
    int k = 0;
    for (int i = 0; i < g_data.count; i++) if (!g_data.rec[i].deleted) tmp[k++] = g_data.rec[i];
    quickSort(tmp, 0, nActive - 1);

    Node* head = NULL;
    for (int i = 0; i < nActive; i++) {
        Node* newNode = createNode(tmp[i]);
        newNode->next = head;
        head = newNode;
    }
    free(tmp);
    return head;
}

// Helper functions

void quickSort (DailyRecord arr[], int low, int high) {
    if (low < high) {
        /* pi is partitioning index, arr[p] is now at right place */
        int pi = partition(arr, low, high);

        // Separately sort elements before partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int partition (DailyRecord arr[], int low, int high) {
    int pivot = arr[high].windSpeed; // pivot
    int i = (low - 1); // Index of smaller element

    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than or equal to pivot
        if (arr[j].windSpeed <= pivot) {
            i++; // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void swap (DailyRecord* a, DailyRecord* b) {
    DailyRecord t = *a;
    *a = *b;
    *b = t;
}

Node* createNode(DailyRecord rec) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node != NULL) {
        node->info = rec;
        node->next = NULL;
    }
    return node;
}

int freeList(Node** head) {
    if (head == NULL || *head == NULL) {
        return 0; // Error: null pointer
    }
    Node* current = *head;
    Node* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
    return 1; // Memory released successfully
}

// CSV load/save
DataArray loadFromCsv(const char* filename) {
    DataArray a = { NULL, 0 };
    FILE *f = fopen(filename, "r");
    if (!f) {
        // not an error; start with empty data
        return a;
    }
    char line[512];
    // optional header skip
    while (fgets(line, sizeof(line), f)) {
        // skip empty lines
        if (line[0] == '\n' || line[0] == '\0') continue;
        // detect header
        if (strstr(line, "date") || strstr(line, "Date")) {
            continue;
        }
        DailyRecord rec;
        int dd, mm, yyyy;
        int tmax, tmin, hum, pres, wdir, wspeed, pcp;
    int read = sscanf(line, "%d/%d/%d,%d,%d,%d,%d,%d,%d,%d",
                           &dd, &mm, &yyyy,
                           &tmax, &tmin, &hum, &pres, &wdir, &wspeed, &pcp);
        if (read == 10) {
            rec.date.dd = dd; rec.date.mm = mm; rec.date.yyyy = yyyy;
            rec.tempMax = tmax; rec.tempMin = tmin; rec.humidity = hum;
            rec.pressure = pres; rec.windDir = wdir; rec.windSpeed = wspeed; rec.precipitation = pcp;
            rec.deleted = false;
            DailyRecord* nb = realloc(a.rec, sizeof(DailyRecord) * (a.count + 1));
            if (!nb) { break; }
            a.rec = nb;
            a.rec[a.count++] = rec;
        }
    }
    fclose(f);
    return a;
}

int saveToCsv(const char* filename, const DataArray a) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Could not open CSV for writing");
        return -1;
    }
    // header
    fprintf(f, "date,tempMax,tempMin,humidity,pressure,windDir,windSpeed,precipitation\n");
    for (int i = 0; i < a.count; i++) {
        const DailyRecord *r = &a.rec[i];
        if (r->deleted) continue;
        fprintf(f, "%02d/%02d/%04d,%d,%d,%d,%d,%d,%d,%d\n",
                r->date.dd, r->date.mm, r->date.yyyy,
                r->tempMax, r->tempMin, r->humidity, r->pressure,
                r->windDir, r->windSpeed, r->precipitation);
    }
    fclose(f);
    return 0;
}

int findIndexByDate(const DataArray* a, Date date) {
    for (int i = 0; i < a->count; i++) {
        if (!a->rec[i].deleted &&
            a->rec[i].date.dd == date.dd &&
            a->rec[i].date.mm == date.mm &&
            a->rec[i].date.yyyy == date.yyyy) {
            return i;
        }
    }
    return -1;
}

// No binary search needed; linear scan is sufficient for in-memory operations

// Functions invoked by the switch menu
void option5_showRecordByDate (void) {
    Date date;
    printf("\nEnter date (DD/MM/YYYY): ");
    if (scanf("%d/%d/%d", &date.dd, &date.mm, &date.yyyy) != 3) {
        printf("Invalid date format.\n");
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return;
    }

    int p = findIndexByDate(&g_data, date);
    if (p != -1) {
        DailyRecord rec = g_data.rec[p];
        printf("\n-----------------------------------\n");
        printf("Record date: %02d/%02d/%04d\n", rec.date.dd, rec.date.mm, rec.date.yyyy);
        printf("Maximum temperature: %d C\n", rec.tempMax);
        printf("Minimum temperature: %d C\n", rec.tempMin);
        printf("Average humidity: %d%%\n", rec.humidity);
        printf("Atmospheric pressure: %d hPa\n", rec.pressure);
        printf("Direction of strongest wind (0 to 360): %d degrees\n", rec.windDir);
        printf("Maximum wind speed: %d km/h\n", rec.windSpeed);
        printf("Precipitation: %d mm", rec.precipitation);
        printf("\n-----------------------------------\n");
    } else {
        printf("The record does not exist.\n");
    }
}

void option6_listMaxTemperature(void){
    // get the list with maximum temperatures
    Node* listTempMax = maxTemperatureRecords();  

    if (listTempMax == NULL) {
        printf("Could not generate the max temperature list.\n");
        return;
    }

    // iterate and show
    Node* aux = listTempMax;

    while(aux != NULL){
        printf("\n-----------------------------------\n");
        printf("Record date with maximum temperature: %02d/%02d/%04d\n", aux->info.date.dd, aux->info.date.mm, aux->info.date.yyyy);
        printf("Maximum temperature: %d C", aux->info.tempMax);
        printf("\n-----------------------------------\n");
        aux = aux->next;
    } 

    if (!freeList(&listTempMax)) {
        printf("Error freeing the list memory.\n");
    }
}

void option7_listMaxPrecipitation(void){
    // list with maximum precipitation (sorted)
    Node* listPreciMax = maxPrecipitationRecords();

    if (listPreciMax == NULL) {
        printf("Could not generate the max precipitation list.\n");
        return;
    }

    Node* aux = listPreciMax;
    int i = 1;

    // show top 10 precipitation entries            
    while((aux != NULL) && (i <= 10)){
        printf("\n-----------------------------------\n");
        printf("Record date: %02d/%02d/%04d\n", aux->info.date.dd, aux->info.date.mm, aux->info.date.yyyy);
        printf("Precipitation: %d mm", aux->info.precipitation);
        printf("\n-----------------------------------\n");
        aux = aux->next;
        i++;
    } 

    if (!freeList(&listPreciMax)) {
        printf("Error freeing the list memory.\n");
    }
}

void option8_listTop10WindSpeeds(void){
    // list of wind speeds (sorted, descending)
    Node* listWind = windSpeedRecords();

    if (listWind == NULL) {
        printf("Could not generate the wind speed list.\n");
        return;
    }

    Node* aux = listWind;
    int i = 1;

    // show top 10 wind speed entries            
    while((aux != NULL) && (i <= 10)){
        printf("\n-----------------------------------\n");
        printf("Record date: %02d/%02d/%04d\n", aux->info.date.dd, aux->info.date.mm, aux->info.date.yyyy);
        printf("Direction of strongest wind (0 to 360): %d degrees\n", aux->info.windDir);
        printf("Maximum wind speed: %d km/h", aux->info.windSpeed);
        printf("\n-----------------------------------\n");
        aux = aux->next;
        i++;
    } 

    if (!freeList(&listWind)) {
        printf("Error freeing the list memory.\n");
    }
}

// (Removed old backup logic; CSV save-on-exit is used instead.)
