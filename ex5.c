/***********
 ID: 207088733
 NAME: Avraham Tsaban
 TASK: ex5
 LANG: C
***********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Episode {
    char *name;
    char *length;
    struct Episode *next;
} Episode;

typedef struct Season {
    char *name;
    Episode *episodes;
    struct Season *next;
} Season;

typedef struct TVShow {
    char *name;
    Season *seasons;
} TVShow;

TVShow ***database = NULL;
int dbSize = 0;

typedef struct Pair {
    int x;
    int y;
} Pair;

#define BASE_STR_LEN 16

char *getString();
int getInt();

int validLength(char *s);
int countShows();

void shrinkDB();
void expandDB();

void freeEpisode(Episode *e);
void freeSeason(Season *s);
void freeShow(TVShow *show);
void freeAll();

TVShow *findShow(char *name);
Season *findSeason(TVShow *show, char *name);
Episode *findEpisode(Season *season, char *name);

void addShow();
void addSeason();
void addEpisode();

void deleteShow();
void deleteSeason();
void deleteEpisode();

void printEpisode();
void printShow();
void printArray();

void *safeRealloc(void *ptr, size_t newSize);
void searchDB(char *showName);
void injectShow(TVShow *insertShow);
Pair DBPrev(Pair address, int size);
Pair DBNext(Pair address, int size);
int shrinkDefragDB();
int defragDB();

void addMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Add a TV show\n");
    printf("2. Add a season\n");
    printf("3. Add an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: addShow(); break;
        case 2: addSeason(); break;
        case 3: addEpisode(); break;
    }
}

void deleteMenu() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Delete a TV show\n");
    printf("2. Delete a season\n");
    printf("3. Delete an episode\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: deleteShow(); break;
        case 2: deleteSeason(); break;
        case 3: deleteEpisode(); break;
    }
}

void printMenuSub() {
    int choice;
    printf("Choose an option:\n");
    printf("1. Print a TV show\n");
    printf("2. Print an episode\n");
    printf("3. Print the array\n");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1: printShow(); break;
        case 2: printEpisode(); break;
        case 3: printArray(); break;
    }
}

void mainMenu() {
    printf("Choose an option:\n");
    printf("1. Add\n");
    printf("2. Delete\n");
    printf("3. Print\n");
    printf("4. Exit\n");
}

int main() {
    int choice;
    do {
        mainMenu();
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1: addMenu(); break;
            case 2: deleteMenu(); break;
            case 3: printMenuSub(); break;
            case 4: freeAll(); break;
        }
    } while (choice != 4);
    return 0;
}

void addShow() {
    printf("Enter the name of the show:\n");
    char *showName = getString();
    if (findShow(showName) != NULL) {
        printf("Show already exists.\n");
        free(showName);
        return;
    }
    if (dbSize < 1 || database[dbSize - 1][dbSize - 1] != NULL) {
        expandDB();
    }
    TVShow *newShow = safeMalloc(sizeof(TVShow));
    newShow->name = showName;
    injectShow(newShow);
}



/************
***Helpers***
************/

/***Memory**/

void *safeRealloc(void *ptr, size_t newSize) {
    void *newPtr = realloc(ptr, newSize);
    if (!newPtr && newSize > 0) {
        free(ptr);
        freeAll();
        exit(1);
    }
    return newPtr;
}

void *safeMalloc(size_t newSize) {
    void *ptr = malloc(newSize);
    if (ptr == NULL) {
        freeAll();
        exit(1);
    }
    return ptr;
}



/*DB Management*/

Pair DBPrev(Pair address, int size) {
    if (--address.x >= 0) {
        return address;
    } else if (--address.y >= 0) {
        address.x = size - 1;
        return address;
    } else {
        address.x = -1;
        address.y = -1;
        return address;
    }
}

void shrinkDB() {
    // defragment database to size dbSize - 1
    while(shrinkDefragDB()) {
        continue;
    }
    // shrink database size
    --dbSize;
    free(database[dbSize]);
    for (int i = 0; i < dbSize; ++i) {
        database[i] = safeRealloc(database[i], (unsigned)dbSize * sizeof(TVShow*));
    }
    database = safeRealloc(database, (unsigned)dbSize * sizeof(TVShow**));
}

int shrinkDefragDB() {
    /* Defragment used locations to size dbSize - 1. 
    Assuming grid is defragmented at size dbSize (by deleteShow), and there are enough used cells to fill dbSize - 1.
    First, find first empty cell from DB end */
    Pair target = {dbSize - 1, dbSize - 1};
    Pair source;
    while (target.x != -1 && database[target.x][target.y] != NULL) {
        target = DBPrev(target, dbSize - 1);
    }
    // find first non-empty cell before target
    source = target;
    while (source.x != -1 && database[source.x][source.y] == NULL) {
        source = DBPrev(source, dbSize);
    }
    // if no more non-empty cells, return 0 to indicate defragmentation is complete
    if (source.x == -1) {
        return 0;
    }
    database[target.x][target.y] = database[source.x][source.y];
    database[source.x][source.y] = NULL;
    // return 1 to indicate there may be more defragmentation to do
    return 1;
}

Pair DBNext(Pair address, int size) {
    if (++address.x < size) {
        return address;
    } else if (++address.y < size) {
        address.x = 0;
        return address;
    } else {
        address.x = -1;
        address.y = -1;
        return address;
    }
}

void expandDB() {
    // expand database size. allocate new row and column and NULLify them.
    ++dbSize;
    database = safeRealloc(database, (unsigned)dbSize * sizeof(TVShow**));
    for (int r = 0; r < dbSize - 1; ++r) {
        database[r] = safeRealloc(database[r], (unsigned)dbSize * sizeof(TVShow*));
        database[r][dbSize - 1] = NULL;
    }
    database[dbSize - 1] = safeMalloc((unsigned)dbSize * sizeof(TVShow*));
    for (int c = 0; c < dbSize; ++c) {
        database[dbSize - 1][c] = NULL;
    }
    // defragment database to size dbSize
    while(defragDB()) {
        continue;
    }
}

int defragDB() {
    /* Defragment used locations to size dbSize. 
    First, find first empty cell from DB start */
    Pair target = {0, 0};
    Pair source;
    while (target.x != -1 && database[target.x][target.y] != NULL) {
        target = DBNext(target, dbSize);
    }
    // find first non-empty cell after target
    source = target;
    while (source.x != -1 && database[source.x][source.y] == NULL) {
        source = DBNext(source, dbSize);
    }
    // if no more non-empty cells, return 0 to indicate defragmentation is complete
    if (source.x == -1) {
        return 0;
    }
    database[target.x][target.y] = database[source.x][source.y];
    database[source.x][source.y] = NULL;
    // return 1 to indicate there may be more defragmentation to do
    return 1;
}

TVShow *findShow(char *name) {
    for (Pair addr = {0,0}; addr.x != -1; addr = DBNext(addr, dbSize)) {
        if (strcmp(name, database[addr.x][addr.y]->name) == 0) {
            return &database[addr.x][addr.y];
        } else {
            continue;
        }
    }
    return NULL;
}

void injectShow(TVShow *insertShow) {
    Pair addr = {0,0};
    // iterate database until finding correct lexicographical position to insert
    while (strcmp(insertShow->name, database[addr.x][addr.y]->name) > 0) {
        if (database[addr.x][addr.y] == NULL) {
            // we have got to the end of used cells, insert here
            database[addr.x][addr.y] = insertShow;
            return;
        }
        /* if (addr.x == -1 || strcmp(insertShow->name, database[addr.x][addr.y]->name) == 0)
            Should not happen, since we checked for duplicates and free space before calling injectShow. 
            Same for dbSize == 0 */
        addr = DBNext(addr, dbSize);
    }
    /* Iteratively shift all subsequent shows by one position.
    Assuming there is always space at the end of the database (handled in addShow). */ 
    TVShow *nextPtr;
    do {
        nextPtr = database[addr.x][addr.y];
        database[addr.x][addr.y] = insertShow;
        insertShow = nextPtr;
        addr = DBNext(addr, dbSize);
        } while (nextPtr != NULL && addr.x != -1);
    return;
}

/***Input***/

char *getString() {
    size_t currentLen = 0, capacity = BASE_STR_LEN;
    char *str = safeMalloc(capacity * sizeof(char)), newChar ;
    // char newChar = (char)getchar(); while caring for EOF
    int inp = getchar();
    if (inp == EOF) {
        newChar = '\n';
    } else {
        newChar = (char)inp;
    }
    while (newChar != '\n') { 
        // append newChar to str
        str[currentLen] = newChar;
        ++currentLen;
        // check if need to expand str
        if (currentLen >= capacity) {
            capacity *= 2;
            str = safeRealloc(str, capacity * sizeof(char));
        }
        newChar = (char)getchar();
    } 
    str[currentLen] = '\0';
    return str;    
}