#include "ht.h"
#include <stdlib.h>

// Hash table structure: create with ht_create, free with ht_destroy.
typedef struct {
	const char* key;
	void* value;
} ht_entry;

struct ht {
	ht_entry* entries;
	size_t capacity;
	size_t length;
};

#define INITIAL_CAPACITY 16

// Create hash table and return pointer to it, or NULL if out of memory.
ht* ht_create(void) 
{
	ht* table = malloc(sizeof(ht));
	if (table == NULL)
		return NULL;

	table -> capacity = INITIAL_CAPACITY;
	table -> length = 0;

	table -> entries = calloc(table->capacity, sizeof(ht_entry));
	if (table -> entries == NULL) {
		free(table);
		return NULL;
	}

	return table;
}
// Free memory allocated for hash table, including allocated keys.
void ht_destroy(ht* table);

// Get item with given key (NUL-terminated) from hash table. Return
// value (which was set with ht_set), or NULL if key not found.
void* ht_get(ht* table, const char* key);

// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when ht_destroy is
// called). Return address of copied key, or NULL if out of memory.
const char* ht_set(ht* table, const char* key, void* value);

// Return number of items in hash table.
size_t ht_length(ht* table);

// Return new hash table iterator (for use with ht_next).
hti ht_iterator(ht* table);

// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call ht_set during iteration.
bool ht_next(hti* it);
