# Exercise 5 — Pointers, Dynamic Allocations, Linked Lists

## Overview

A TV show database management system implemented in C, using a dynamically resizing 2D array of pointers and hierarchical linked lists for seasons and episodes. The system supports add, delete, print, and search operations with automatic defragmentation and full memory management (zero leaks required).

## Author

Avraham Tsaban

## Compilation

```bash
gcc -Wall -Wextra -Werror -g -std=c99 ex5.c -o ex5.out
```

## Running

```bash
./ex5.out
```

Memory leak checking (recommended):

```bash
valgrind ./ex5.out
```

## Features

- **Show Management** — Add, retrieve, and delete TV shows stored in lexicographical order.
- **Season Management** — Add and remove seasons within shows with custom positioning.
- **Episode Management** — Add and remove episodes within seasons, with HH:MM:SS time format validation.
- **Dynamic Database** — 2D array that expands/shrinks automatically with defragmentation.
- **Full Memory Management** — All allocations tracked and freed; designed for zero bytes lost under valgrind.

## Menu

```
1. Add (show / season / episode)
2. Delete (show / season / episode)
3. Print (show / episode / array)
4. Exit
```

## Code Structure

All code is in a single file `ex5.c` (1018 lines). The database is a global 2D dynamic array (`TVShow ***database`) with hierarchical linked lists for seasons and episodes.

### Data Structures

```c
typedef struct Episode { char *name; char *length; struct Episode *next; } Episode;
typedef struct Season  { char *name; Episode *episodes; struct Season *next; } Season;
typedef struct TVShow  { char *name; Season *seasons; } TVShow;
typedef struct Pair    { int x; int y; } Pair;  // 2D grid coordinate
```

### CRUD Operations

| Function | Description |
|----------|-------------|
| `addShow()` | Creates a show, expands DB if needed, inserts in lexicographical order. |
| `addSeason()` | Inserts a season into a show's linked list at a given position. |
| `addEpisode()` | Inserts an episode into a season's list with time format validation. |
| `deleteShow()` | Frees a show and all children, defragments and possibly shrinks the DB. |
| `deleteSeason()` | Unlinks and frees a season from a show. |
| `deleteEpisode()` | Unlinks and frees an episode from a season. |
| `printShow()` | Prints show name, all seasons and episodes. |
| `printEpisode()` | Prints a single episode's name and length. |
| `printArray()` | Prints the 2D database grid (debug view). |

### Search Functions

All return pointer-to-pointer for efficient in-place deletion:

| Function | Description |
|----------|-------------|
| `findShow(name)` → `TVShow**` | Searches the DB for a show by name. |
| `findSeason(show, name)` → `Season**` | Searches a show's season linked list. |
| `findEpisode(season, name)` → `Episode**` | Searches a season's episode linked list. |

### Database Management

| Function | Description |
|----------|-------------|
| `expandDB()` | Grows the grid by 1 in each dimension, then defragments. |
| `shrinkDB()` | Defragments to fit a smaller grid, then shrinks by 1 in each dimension. |
| `defragDB()` | Forward defragmentation — consolidates entries from the start. |
| `shrinkDefragDB()` | Backward defragmentation — packs entries to fit within `(dbSize−1)²`. |
| `injectShow(show)` | Inserts show at correct lexicographical position, shifting subsequent entries. |
| `countShows()` | Counts non-NULL entries in the DB. |
| `checkInitDB()` | Lazily initializes the DB to a 1×1 grid on first use. |
| `DBNext(address, size)` → `Pair` | Next coordinate in column-major order (`(-1,-1)` at end). |
| `DBPrev(address, size)` → `Pair` | Previous coordinate in column-major order (`(-1,-1)` at start). |

### Memory Management

| Function | Description |
|----------|-------------|
| `safeMalloc(size)` | `malloc` wrapper — exits on failure after freeing everything. |
| `safeRealloc(ptr, size)` | `realloc` wrapper — exits on failure. |
| `freeEpisode(e)` | Frees episode name, length, and struct. |
| `freeSeason(s)` | Frees a season and all its episodes. |
| `freeShow(show)` | Frees a show and all its seasons/episodes. |
| `freeAll()` | Frees the entire database (all shows, rows, top-level pointer). |

### Input / Validation

| Function | Description |
|----------|-------------|
| `getString()` → `char*` | Dynamically reads a line from stdin (auto-expanding buffer). |
| `safeGetChar()` → `char` | Reads one character; returns `'\n'` on EOF. |
| `validLength(s)` → `int` | Validates string matches `HH:MM:SS` format. |
| `checkLengthChar(c, index)` → `int` | Checks character validity at a given position in `HH:MM:SS`. |

## Project Files

| File | Description |
|------|-------------|
| `ex5.c` | Source code |
| `ex5.example` | Reference Linux executable provided by the TA |
| `ex5_instructions.md` | Exercise instructions |
| `tests/` | Test files (`test1.tst` – `test6.tst`) provided by the TA |

## Code Ownership

The source code includes initial scaffolding and design by the TA: menus, data structures, function signatures, and overall architecture. The student implemented the function bodies. See the [original scaffolding commit](https://github.com/CSI-BIU/ex5/commit/23c7aa87d514e1e4afa317a73b2cf73f8a4e77f0) for details.

## Attribution

The exercise design, specifications, instructions, scaffolding code, and test files were created by **Eliyahu Houri**, the Teaching Assistant responsible for this assignment. The instructions file (`ex5_instructions.md`), the reference executable (`ex5.example`), and the `tests/` directory are his work. Any license in this repository applies only to the student's code implementation.
