#include "ht.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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
void ht_destroy(ht* table)
{
	for (size_t i = 0; i < table->capacity; i++) {
		free((void*)table->entries[i].key);
	}
	free(table -> entries);
	free(table);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key (NUL-terminated).
static uint64_t hash_key(const char* key) 
{
	uint64_t hash = FNV_OFFSET;
	for (const char* p = key; *p; p++) {
		hash ^= (uint64_t)(unsigned char)(*p);
		hash *= FNV_PRIME;
	}
	return hash;
}

// Get item with given key (NUL-terminated) from hash table. Return
// value (which was set with ht_set), or NULL if key not found.
void* ht_get(ht* table, const char* key)
{
	uint64_t hashVal = hash_key(key);
	size_t index = hashVal % table->capacity;
	
	while (table->entries[index].key != NULL) {
		if (strcmp(table->entries[index].key, key) == 0)
			return table->entries[index].value;
		index++;

		if (index >= table->capacity)
			index = 0;
	}	
	
	return NULL;

}

static const char* ht_set_entry(ht_entry* entries, size_t capacity, const char* key, void* value, size_t* plength) {
	uint64_t hashVal = hash_key(key);
	size_t index = hashVal % capacity;

	while (entries[index].key != NULL) {
		if (strcmp(entries[index].key, key) == 0) {
			entries[index].value = value;
			return entries[index].key;
		}
		index++;
		if (index >= capacity) {
			index = 0;
		}	
	}

	if (plength != NULL) {
		key = strdup(key);
		if (key == NULL)
			return NULL;
		(*plength)++;
	}
	entries[index].key = key;
	entries[index].value = value;
	return key;
	
}

static bool ht_expand(ht* table) {
	size_t new_capacity = table->capacity * 2;
	if (new_capacity < table->capacity)
		return false; // overflow

	ht_entry* new_entries = calloc(new_capacity, sizeof(ht_entry));
	if (new_entries == NULL)
		return false;

	for (size_t i = 0; i < table->capacity; i++) {
		ht_entry entry = table->entries[i];
		if(entry.key != NULL) {
			ht_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
		}
	}

	free(table->entries);
	table->entries = new_entries;
	table->capacity = new_capacity;
	return true;
}


// Set item with given key (NUL-terminated) to value (which must not
// be NULL). If not already present in table, key is copied to newly
// allocated memory (keys are freed automatically when ht_destroy is
// called). Return address of copied key, or NULL if out of memory.
const char* ht_set(ht* table, const char* key, void* value)
{
	assert(value != NULL);
	if (value == NULL)
		return NULL;

	if (table->length >= table->capacity / 2) {
		if (!ht_expand(table))
			return NULL;
	}

	return ht_set_entry(table->entries, table->capacity, key, value, &table->length);
}

// Return number of items in hash table.
size_t ht_length(ht* table)
{
	return table->length;
}

// Return new hash table iterator (for use with ht_next).
hti ht_iterator(ht* table)
{
	hti it;
	it._table = table;
	it._index = 0;
	return it;
}

// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call ht_set during iteration.
bool ht_next(hti* it)
{
	ht* table = it->_table;
	while (it->_index < table->capacity) {
		size_t i = it->_index;
		it->_index++;
		if (table->entries[i].key != NULL) {
			ht_entry entry = table->entries[i];
			it->key = entry.key;
			it->value = entry.value;
			return true;
		}
	}
	return false;
}
