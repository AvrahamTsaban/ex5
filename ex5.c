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

TVShow **findShow(char *name);
Season **findSeason(TVShow *show, char *name);
Episode **findEpisode(Season *season, char *name);

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
void *safeMalloc(size_t newSize);
void injectShow(TVShow *insertShow);
Pair DBPrev(Pair address, int size);
Pair DBNext(Pair address, int size);
int shrinkDefragDB();
int defragDB();
char safeGetChar();
void checkInitDB();

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

/*************
****Add*or****
****Delete****
*************/

void addShow() {
    // add a new show to the database
    printf("Enter the name of the show:\n");
    char *showName = getString();

    // check if show already exists
    if (findShow(showName) != NULL) {
        printf("Show already exists.\n");
        free(showName);
        showName = NULL;
        return;
    }

    // expand database if needed
    if (dbSize < 1 || database[dbSize - 1][dbSize - 1] != NULL) {
        expandDB();
    }

    // create new show with given name and NULL seasons
    TVShow *newShow = safeMalloc(sizeof(TVShow));
    newShow->name = showName;
    newShow->seasons = NULL;

    // inject show into database at correct lexicographical position
    injectShow(newShow);
}

void addSeason() {
    // get a show name and add a season to it
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **showPtr = findShow(showName);
    free(showName);
    showName = NULL;

    // check if show exists, otherwise print error and return
    if (showPtr == NULL) {
        printf("Show not found.\n");
        return;
    }

    // get season name and check if it already exists
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    if (findSeason(*showPtr, seasonName) != NULL) {
        printf("Season already exists.\n");
        free(seasonName);
        seasonName = NULL;
        return;
    }

    // create new season with given name and NULL episodes and next
    Season *tmp, *newSeason = safeMalloc(sizeof(Season));
    newSeason->name = seasonName;
    newSeason->episodes = NULL;
    newSeason->next = NULL;

    // get position to insert at
    int position;
    printf("Enter the position:\n");
    scanf(" %d", &position);
    getchar();

    // insert season at position: at head if position is 0 or list is empty, else traverse to position and insert
    newSeason->next = (*showPtr)->seasons;
    if (position == 0 || newSeason->next == NULL) {
        (*showPtr)->seasons = newSeason;
    } else {
        for(int i = 1; i < position && newSeason->next->next != NULL; ++i) {
            newSeason->next = newSeason->next->next;
        }
        tmp = newSeason->next->next;
        newSeason->next->next = newSeason;
        newSeason->next = tmp;
    }

}

void addEpisode() {
    // get a show name and add a season to it
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **showPtr = findShow(showName);
    free(showName);
    showName = NULL;

    // check if show exists, otherwise print error and return
    if (showPtr == NULL) {
        printf("Show not found.\n");
        return;
    }

    // get season name to add episode to
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season **seasonPtr = findSeason(*showPtr, seasonName);
    free(seasonName);
    seasonName = NULL;

    // check if season exists, otherwise print error and return
    if (seasonPtr == NULL) {
        printf("Season not found.\n");
        return;
    }

    // get episode name and check if it already exists
    printf("Enter the name of the episode:\n");
    char *episodeName = getString();
    if (findEpisode(*seasonPtr, episodeName) != NULL) {
        printf("Episode already exists.\n");
        free(episodeName);
        episodeName = NULL;
        return;
    }
    
    // create new episode with given name and NULL next
    Episode *tmp, *newEpisode = safeMalloc(sizeof(Episode));
    newEpisode->name = episodeName;
    newEpisode->next = NULL;

    // get episode length and validate it
    printf("Enter the length (xx:xx:xx):\n");
    char *length = getString();
    while (!validLength(length)) {
        printf("Invalid length, enter again:\n");
        free(length);
        length = getString();
    }
    newEpisode->length = length;

    // get position to insert at
    int position;
    printf("Enter the position:\n");
    scanf(" %d", &position);

    // insert episode at position: at head if position is 0 or list is empty, else traverse to position and insert
    newEpisode->next = (*seasonPtr)->episodes;
    if (position == 0 || newEpisode->next == NULL) {
        (*seasonPtr)->episodes = newEpisode;
    } else {
        for(int i = 1; i < position && newEpisode->next->next != NULL; ++i) {
            newEpisode->next = newEpisode->next->next;
        }
        tmp = newEpisode->next->next;
        newEpisode->next->next = newEpisode;
        newEpisode->next = tmp;
    }
}

void deleteShow() {
    // get a show name and delete it from the database
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **showPtr = findShow(showName);
    free(showName);
    showName = NULL;

    // check if show exists, otherwise print error and return
    if (showPtr == NULL) {
        printf("Show not found.\n");
        return;
    }

    // free show memory and defragment database
    freeShow(*showPtr);
    *showPtr = NULL;
    while (defragDB()) {
        continue;
    }

    // shrink database if needed
    if (countShows() <= (dbSize - 1)*(dbSize - 1)) {
        shrinkDB();
    }
}

void deleteSeason() {
    // get a show name to delete a season from
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **showPtr = findShow(showName);
    free(showName);
    showName = NULL;

    // check if show exists, otherwise print error and return
    if (showPtr == NULL) {
        printf("Show not found.\n");
        return;
    }

    // get season name to delete
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season *tmp, **prevToSeasonPtr = findSeason(*showPtr, seasonName);
    free(seasonName);
    seasonName = NULL;

    // check if season exists, otherwise print error and return
    if (prevToSeasonPtr == NULL) {
        printf("Season not found.\n");
        return;
    }

    /* PrevToSeasonPtr points to the 'next' field of the previous season
    (or seasons field of the show), allowing us to NULLify or point it to the next season. */
    if ((*prevToSeasonPtr)->next == NULL) {
        freeSeason(*prevToSeasonPtr);
        *prevToSeasonPtr = NULL;
        return;
    } else {
        tmp = *prevToSeasonPtr;
        *prevToSeasonPtr = (*prevToSeasonPtr)->next;
        freeSeason(tmp);
        return;
    }
}

void deleteEpisode() {
    // get a show name to delete episode from
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **showPtr = findShow(showName);
    free(showName);
    showName = NULL;

    // check if show exists, otherwise print error and return
    if (showPtr == NULL) {
        printf("Show not found.\n");
        return;
    }

    // get season name to delete episode from
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season **seasonPtr = findSeason(*showPtr, seasonName);
    free(seasonName);
    seasonName = NULL;

    // check if season exists, otherwise print error and return
    if (seasonPtr == NULL) {
        printf("Season not found.\n");
        return;
    }

    // get episode name and check if it already exists
    printf("Enter the name of the episode:\n");
    char *episodeName = getString();
    Episode *tmp, **prevToEpisodePtr = findEpisode(*seasonPtr, episodeName);
    free(episodeName);
    episodeName = NULL;
    
    // check if episode exists, otherwise print error and return
    if (prevToEpisodePtr == NULL) {
        printf("Episode not found.\n");
        return;
    }
    
    /* prevToEpisodePtr points to the 'next' field of the previous episode
    (or episodes field of the season), allowing us to NULLify or point it to the next episode. */
    if ((*prevToEpisodePtr)->next == NULL) {
        freeEpisode(*prevToEpisodePtr);
        *prevToEpisodePtr = NULL;
        return;
    } else {
        tmp = *prevToEpisodePtr;
        *prevToEpisodePtr = (*prevToEpisodePtr)->next;
        freeEpisode(tmp);
        return;
    }
}

/*************
*****Print****
**************/

void printArray() {
    for (int r = 0; r < dbSize; ++r) {
        for (int c = 0; c < dbSize; ++c) {
            printf("[%s] ", database[c][r] ? database[c][r]->name : "NULL");
        }
        printf("\n");
    }
}

void printShow() {
    // get a show name and print its details
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **showPtr = findShow(showName);
    free(showName);
    showName = NULL;
    
    // check if show exists, otherwise print error and return
    if (showPtr == NULL) {
        printf("Show not found.\n");
        return;
    }
    
    // print show details: name, seasons and episodes
    printf("Name: %s\nSeasons:\n", (*showPtr)->name);
    Season *currentSeason = (*showPtr)->seasons;
    int i = 0;
    while (currentSeason != NULL) {
        printf("\tSeason %d: %s\n", i++, currentSeason->name);
        int j = 0;
        Episode *currentEpisode = currentSeason->episodes;
        while (currentEpisode != NULL) {
            printf("\t\tEpisode %d: %s (%s)\n", j++, currentEpisode->name, currentEpisode->length);
            currentEpisode = currentEpisode->next;
        }
        currentSeason = currentSeason->next;
    }
}

void printEpisode() {
    // get a show name to print an episode from
    printf("Enter the name of the show:\n");
    char *showName = getString();
    TVShow **showPtr = findShow(showName);
    free(showName);
    showName = NULL;

    // check if show exists, otherwise print error and return
    if (showPtr == NULL) {
        printf("Show not found.\n");
        return;
    }

    // get season name to print an episode from
    printf("Enter the name of the season:\n");
    char *seasonName = getString();
    Season **seasonPtr = findSeason(*showPtr, seasonName);
    free(seasonName);
    seasonName = NULL;

    // check if season exists, otherwise print error and return
    if (seasonPtr == NULL) {
        printf("Season not found.\n");
        return;
    }

    // get episode name and check if it already exists
    printf("Enter the name of the episode:\n");
    char *episodeName = getString();
    Episode **prevToEpisodePtr = findEpisode(*seasonPtr, episodeName);
    free(episodeName);
    episodeName = NULL;
    
    // check if episode exists, otherwise print error and return
    if (prevToEpisodePtr == NULL) {
        printf("Episode not found.\n");
        return;
    }

    // print episode details
    printf("Name: %s\n", (*prevToEpisodePtr)->name);
    printf("Length: %s\n", (*prevToEpisodePtr)->length);
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

void freeEpisode(Episode *e) {
    // free episode e. Assumes e is not NULL. calling function is responsible for unlinking e from list.
    free(e->length);
    free(e->name);
    free(e);
}

void freeSeason(Season *s) {
    // free s and all its episodes. Assumes s is not NULL. calling function is responsible for unlinking s from list.
    Episode *current = s->episodes;
    while (current != NULL) {
        Episode *next = current->next;
        freeEpisode(current);
        current = next;
    }

    free(s->name);   
    free(s);
}

void freeShow(TVShow *show) {
    /* free show and all its seasons and episodes. Assumes show is not NULL. calling function is responsible for defragmenting DB. */
    Season *current = show->seasons;
    while (current != NULL) {
        Season *next = current->next;
        freeSeason(current);
        current = next;
    }

    free(show->name);
    free(show);
}

void freeAll() {
    // if database is NULL, nothing to free
    if (database == NULL) {
        return;
    }

    // free all memory allocated inside database and database itself
    for (Pair cell = {0,0}; cell.x != -1 && database[cell.x][cell.y] != NULL; cell = DBNext(cell, dbSize)) {
        freeShow(database[cell.x][cell.y]);
    }

    for (int r = 0; r < dbSize; ++r) {
        free(database[r]);
    }

    free (database);
    database = NULL;
    dbSize = 0;
}

/***Show DB***
 *Management*/

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
    // free entire database to avoid zero-size reallocations
    if (dbSize == 1) {
        free(database[0]);
        free(database);
        database = NULL;
        return;
    }

    // defragment database to size dbSize - 1
    while(shrinkDefragDB()) {
        continue;
    }

    // shrink database size
    --dbSize;

    // free last row and column
    free(database[dbSize]);
    database = safeRealloc(database, (unsigned)dbSize * sizeof(TVShow**));
    for (int i = 0; i < dbSize; ++i) {
        database[i] = safeRealloc(database[i], (unsigned)dbSize * sizeof(TVShow*));
    }
}

int shrinkDefragDB() {
    /* Defragment used locations to size dbSize - 1. 
    Assuming grid is defragmented at size dbSize (by deleteShow), and there are enough used cells to fill dbSize - 1. */
    Pair target = {dbSize - 2, dbSize - 2};
    Pair source;

    // find first empty cell from DB end
    while (target.x != -1 && database[target.x][target.y] != NULL) {
        target = DBPrev(target, dbSize - 1);
    }

    source = target;
    // find first non-empty cell before target
    while (source.x != -1 && database[source.x][source.y] == NULL) {
        source = DBPrev(source, dbSize);
    }

    // if no more non-empty cells, return 0 to indicate defragmentation is complete
    if (source.x == -1) {
        return 0;
    }

    // move show from source to target and NULLify source
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
    // expand database size.
    ++dbSize;
    
    // allocate new row and column and NULLify them.
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
    // Defragment used locations to size dbSize. 
    Pair target = {0, 0};
    Pair source;

    // find first empty cell from DB start
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

    // move show from source to target and NULLify source
    database[target.x][target.y] = database[source.x][source.y];
    database[source.x][source.y] = NULL;

    // return 1 to indicate there may be more defragmentation to do
    return 1;
}

int countShows() {
    int amount = 0;
    for (Pair addr = {0,0}; addr.x != -1; addr = DBNext(addr, dbSize)) {
        if (database[addr.x][addr.y] == NULL) {
            return amount;
        } else {
            ++amount;
        }
    }
    return amount;
}

void checkInitDB() {
    // database[0][0] availability is crucial for other functions, so initialize database to size 1x1
    if (database == NULL) {
        dbSize = 1;
        database = safeMalloc(sizeof(TVShow**));
        database[0] = safeMalloc(sizeof(TVShow*));
        database[0][0] = NULL;
    }
}

TVShow **findShow(char *name) {
    /* this function (and others) crucially depends on database[0][0] being initialized. 
    This function is called first in most user-facing functions (excluding printArray and freeAll), 
    so that we only have to call checkInitDB here () */
    checkInitDB();

    // search database for show with name 'name'. return pointer to pointer to show if found, NULL otherwise.
    for (Pair addr = {0,0}; addr.x != -1 && database[addr.x][addr.y] != NULL; addr = DBNext(addr, dbSize)) {
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
    int cmp;
    do {
        if (database[addr.x][addr.y] == NULL) {
            // we have got to the end of used cells, insert here
            database[addr.x][addr.y] = insertShow;
            return;
        }
        /* if (addr.x == -1 || strcmp(insertShow->name, database[addr.x][addr.y]->name) == 0)
            Should not happen, since we checked for duplicates and free space before calling injectShow. 
            Same for dbSize == 0 */
        cmp = strcmp(insertShow->name, database[addr.x][addr.y]->name);
        if (cmp > 0) {
            addr = DBNext(addr, dbSize);
        } else if (cmp < 0) {
            break;
        } 
    } while (cmp > 0);

    // Shift subsequent shows, Assuming there is space at the end (handled in addShow)
    TVShow *nextPtr;
    do {
        nextPtr = database[addr.x][addr.y];
        database[addr.x][addr.y] = insertShow;
        insertShow = nextPtr;
        addr = DBNext(addr, dbSize);
        } while (nextPtr != NULL && addr.x != -1);
    return;
}

/**Episode***
*and*Season**
*Management*/

Season **findSeason(TVShow *show, char *name) {
    /* Search show's seasons for season with name 'name'.
    Return the address of the previous season's 'next' field (or show's seasons) if found,
    providing a ** to the season. Otherwise, return NULL. */
    Season **current = &(show->seasons);
    while (*current != NULL) {
        if (strcmp(name, (*current)->name) == 0) {
            return current;
        }
        current = &((*current)->next);
    }
    return NULL;
}

Episode **findEpisode(Season *season, char *name) {
    /* Search season's episodes for episode with name 'name'.
    Return the address of the previous episode's 'next' field (or season's episodes) if found,
    providing a ** to the episode. Otherwise, return NULL. */
    Episode **current = &(season->episodes);
    while (*current != NULL) {
        if (strcmp(name, (*current)->name) == 0) {
            return current;
        }
        current = &((*current)->next);
    }
    return NULL;
}


/***Input***/

char *getString() {
    // dynamically read string from stdin until newline
    size_t currentLen = 0, capacity = BASE_STR_LEN;
    char *str = safeMalloc(capacity * sizeof(char)), newChar ;
    
    // read characters until newline and append to str
    newChar = safeGetChar();
    while (newChar != '\n') { 
        // append newChar to str
        str[currentLen] = newChar;
        ++currentLen;
        // check if need to expand str
        if (currentLen >= capacity) {
            capacity *= 2;
            str = safeRealloc(str, capacity * sizeof(char));
        }
        newChar = safeGetChar();
    } 

    // null-terminate str and return
    str[currentLen] = '\0';
    return str;    
}

char safeGetChar() {
    // get a character from stdin, returning '\n' on EOF or null character
    int inp = getchar();
    if (inp == EOF || inp == '\0') {
        return '\n';
    } else {
        return (char)inp;
    }
}

int validLength(char *s) {
    // validate that s is in format xx:xx:xx with proper ranges
    if (s == NULL) {
        return 0;
    }
    if (strlen(s) != 8) {
        return 0;
    }
    for (short i = 0; i < 8; ++i) {
        if ((i < 2 || i == 4 || i == 7) && (s[i] > '9' || s[i] < '0')) {
            return 0;
        } else if ((i == 2 || i == 5) && (s[i] != ':')) {
            return 0;
        } else if ((i == 3 || i == 6) && (s[i] > '5' || s[i] < '0')) {
            return 0;
        } 
    }
    return 1;
}