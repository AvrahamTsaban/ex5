# TV Show Database Management System

## Overview

A robust C-based TV show database management system that enables users to manage a hierarchical collection of TV shows, seasons, and episodes. The system employs a 2D dynamic array with automatic memory management and intelligent defragmentation to maintain optimal performance and memory utilization.

## Features

### Core Functionality
- **Show Management**: Add, retrieve, and delete TV shows with lexicographical ordering
- **Season Management**: Manage seasons within shows with custom positioning
- **Episode Management**: Organize episodes within seasons with validation and time tracking
- **Database Operations**: Dynamic expansion/contraction with automatic defragmentation

### Technical Highlights
- Dynamic 2D array-based database with automatic scaling
- Intelligent defragmentation and memory consolidation
- Hierarchical linked-list structure for seasons and episodes
- Input validation for time format (HH:MM:SS)
- Comprehensive error handling and graceful failure modes

## Credits

This project includes initial scaffolding and design contributions from Eliyahu Houri, one of the course Teaching Assistants:
- **Contribution**: Menus, data structures, function signatures, and overall architecture design
- **Reference**: [View original contribution](https://github.com/CSI-BIU/ex5/commit/23c7aa87d514e1e4afa317a73b2cf73f8a4e77f0)

**Copyright Notice**: The intellectual property rights for the aforementioned contributions remain with their original author and are not claimed by the repository owner. All other code and documentation in this repository are provided under the terms specified in the accompanying license file. 

## Architecture

### Data Structures

#### Episode
```c
typedef struct Episode {
    char *name;           // Episode title
    char *length;         // Duration in HH:MM:SS format
    struct Episode *next; // Pointer to next episode in linked list
} Episode;
```

#### Season
```c
typedef struct Season {
    char *name;           // Season identifier
    Episode *episodes;    // Linked list of episodes
    struct Season *next;  // Pointer to next season
} Season;
```

#### TVShow
```c
typedef struct TVShow {
    char *name;          // Show name
    Season *seasons;     // Linked list of seasons
} TVShow;
```

#### Database Layout
- **Type**: 2D dynamic array of `TVShow*` pointers
- **Organization**: Row-major ordering for traversal
- **Capacity**: Expands/shrinks based on show count
- **Status**: NULL cells indicate empty slots

### Key Algorithms

#### Database Navigation
- `DBNext(Pair, int)`: Advance to next position in row-major order
- `DBPrev(Pair, int)`: Move to previous position in row-major order
- Returns `(-1, -1)` sentinel to indicate boundary conditions

#### Defragmentation
- **Forward Defragmentation** (`defragDB`): Consolidates used cells from database start
- **Backward Defragmentation** (`shrinkDefragDB`): Consolidates to smaller capacity
- Triggered automatically during show deletion and database resizing

#### Search Operations
All search functions return pointer-to-pointer, enabling efficient deletion without relocation:
- `findShow()`: Lexicographical search in database
- `findSeason()`: Linear search in season linked list
- `findEpisode()`: Linear search in episode linked list

## API Reference

### Add Operations

#### `void addShow()`
Adds a new TV show to the database.
- **Precondition**: Show name must be unique
- **Side Effects**: Triggers database expansion if capacity exceeded
- **Time Complexity**: O(n) for lexicographical insertion

#### `void addSeason()`
Adds a season to an existing show.
- **Precondition**: Show must exist
- **Parameters**: Season name, insertion position
- **Position**: 0 = head, n = tail

#### `void addEpisode()`
Adds an episode to a season.
- **Precondition**: Show and season must exist
- **Validation**: Episode length must be in HH:MM:SS format
- **Parameters**: Episode name, length, insertion position

### Delete Operations

#### `void deleteShow()`
Removes a show and all associated data.
- **Side Effects**: Triggers defragmentation and potential database shrinking
- **Memory**: Recursively frees all seasons and episodes

#### `void deleteSeason()`
Removes a season and its episodes.

#### `void deleteEpisode()`
Removes an episode from a season.

### Print Operations

#### `void printShow()`
Displays show hierarchy: name, seasons, and episodes.

#### `void printEpisode()`
Displays specific episode details.

#### `void printArray()`
Displays 2D database grid for debugging.

### Memory Management

#### `void *safeMalloc(size_t)`
Allocates memory with error checking.
- **Behavior**: Exits program on failure

#### `void *safeRealloc(void *, size_t)`
Reallocates memory with error checking.
- **Behavior**: Exits program on failure

#### Memory Cleanup Functions
- `freeEpisode(Episode*)`: Frees single episode
- `freeSeason(Season*)`: Recursively frees season and episodes
- `freeShow(TVShow*)`: Recursively frees show, seasons, and episodes
- `freeAll()`: Global cleanup on program termination

### Input Functions

#### `char *getString()`
Dynamically reads input string with auto-expansion.
- **Behavior**: Reads until newline or EOF
- **Returns**: Dynamically allocated, null-terminated string
- **Caller Responsibility**: Must free returned pointer

#### `int validLength(char *s)`
Validates time format (HH:MM:SS).
- **Format**: Exactly 8 characters: `dd:dd:dd`
- **Returns**: 1 if valid, 0 otherwise

## Usage

### Interactive Menu System

Main Menu:
```
1. Add (show/season/episode)
2. Delete (show/season/episode)
3. Print (show/episode/array)
4. Exit
```

### Example Session
```
Choose an option:
1. Add
Enter the name of the show:
Breaking Bad
Enter the name of the season:
Season 1
Enter the position:
0
Enter the name of the episode:
Pilot
Enter the length (xx:xx:xx):
00:58:00
```

## Performance Characteristics

| Operation | Time Complexity | Notes |
|-----------|-----------------|-------|
| Add Show | O(n) | Lexicographical search + insertion |
| Add Season/Episode | O(m) | Linear list traversal |
| Delete Show | O(n²) | Includes defragmentation |
| Find Show | O(n) | Database scan |
| Find Season/Episode | O(m) | List traversal |
| Database Resize | O(n²) | Defragmentation required |

## Error Handling

### Validation Checks
- Show/season/episode existence verification
- Duplicate name prevention
- Episode length format validation
- Memory allocation failure detection

### Recovery Strategies
- Graceful error messages to user
- Automatic database cleanup on fatal errors
- Program termination on unrecoverable failures

## Compilation and Testing

### Build
```bash
gcc -o ex5 ex5.c
```

### Testing
Test files available in `tests/` directory:
- `test1.tst` through `test6.tst`: Comprehensive functionality tests

## Implementation Notes

### Design Decisions

1. **Pointer-to-Pointer Returns**: Search functions return `**` to enable efficient in-place list modifications

2. **Row-Major Traversal**: Database uses row-major order for consistent traversal semantics

3. **Dual Defragmentation**: Separate forward and backward algorithms optimize for different scenarios

4. **Lazy Initialization**: Database initialized on first user operation (not at program start)

5. **Safe Memory Operations**: All allocations checked with explicit error handling

### Edge Cases Handled
- Empty database operations
- NULL pointer validations
- EOF during string input
- Zero-size reallocations
- Boundary conditions in grid traversal

## Future Enhancements

- Persistent storage (file I/O)
- Show rating system
- Advanced search and filtering
- Database export (JSON/CSV)
- Performance optimization for large datasets
