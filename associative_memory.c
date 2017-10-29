/*
 * associative_memory.c
 *
 *  Created on: Oct 19, 2017
 *      Author: rtrk
 */


#include "associative_memory.h"

static int_least32_t hash_function(const char key[], long long key_len, int_least32_t size);
static int8_t linear_probing(hash_table_t* table, const char key[], void* element, int_least32_t storage_index);
static int8_t search_for_element(const hash_table_t* table, const char key[], int_least32_t storage_index);

int8_t create_hash_table(hash_table_t* hash_table, int_least32_t size) {
	int_least32_t i;
	hash_table->table_size = size;

	hash_table->table = (void**) malloc(sizeof(void*) * size);
	hash_table->keys = (char**) malloc(sizeof(char*) * size);
	hash_table->deleted_flag = (int8_t*) malloc(sizeof(int8_t) * size);

	assert(hash_table->table != NULL);
	assert(hash_table->keys != NULL);
	assert(hash_table->deleted_flag != NULL);

	if ((hash_table->table == NULL) ||
		(hash_table->keys == NULL))
	{
		return 1;
	}
	/* Initialize elements and keys to NULL, deleted flag to 0 */
	for (i = 0; i < size; i++)
	{
		hash_table->table[i] = NULL;
		hash_table->keys[i] = NULL;
		hash_table->deleted_flag[i] = 0;
	}

	return 0;
}


int8_t insert_to_hash_table(hash_table_t* hash_table, const char key[], void* element, int16_t type_size) {
	long long key_len;
	int_least32_t storage_index;

	key_len = strlen(key);
	storage_index = hash_function(key, key_len, hash_table->table_size);
	assert((storage_index >= 0) && (storage_index < hash_table->table_size));

	/* space in table already taken */
	if (hash_table->keys[storage_index] != NULL)
	{
		storage_index = linear_probing(hash_table, key, element, storage_index);
	}

	if (storage_index == -1)
	{
		/* cannot find empty place int table to insert element */
		return -1;
	}

	hash_table->keys[storage_index] = (char*) malloc(key_len);
	assert(hash_table->keys[storage_index] != NULL);
	/* cannot allocate space */
	if (hash_table->keys[storage_index] == NULL)
	{
		return 1;
	}
	/* store key */
	memcpy(hash_table->keys[storage_index], key, key_len);

	hash_table->table[storage_index] = (void*) malloc(type_size);
	assert(hash_table->table[storage_index] != NULL);
	if (hash_table->table[storage_index] == NULL)
	{
		return 1;
	}
	/* store element */
	memcpy(hash_table->table[storage_index], element, type_size);
	/* clear delete flag */
	hash_table->deleted_flag[storage_index] = 0;

#ifndef NDEBUG
	printf("element: %s with key: %s inserted on index: %d \n", ((char*)element), key, storage_index);
#endif
	return 0;
}

void* get_from_hash_table(const hash_table_t* hash_table, const char key[]) {
	int_least32_t storage_index;
	/* calculate storage index based on key */
	storage_index = hash_function(key, strlen(key), hash_table->table_size);

	storage_index = search_for_element(hash_table, key, storage_index);

	if (storage_index == -1)
	{
#ifndef NDEBUG
		printf("Value with key: %s doesn't exists in hash_table\n", key);
#endif
		return NULL;
	}

	return hash_table->table[storage_index];
}

int8_t delete_from_hash_table(hash_table_t* hash_table, const char key[]) {
	int_least32_t storage_index;

	storage_index = hash_function(key, strlen(key), hash_table->table_size);

	/* Search for the element, starting from the storage index */
	storage_index = search_for_element((const hash_table_t*)hash_table, key, storage_index);

	if (storage_index == -1)
	{
#ifndef NDEBUG
		printf("Element with key %s doesn't exist in table!\n", key);
#endif
		return 1;
	} else
	{
		free(hash_table->keys[storage_index]);
		free(hash_table->table[storage_index]);
		hash_table->keys[storage_index] = NULL;
		hash_table->table[storage_index] = NULL;
		hash_table->deleted_flag[storage_index] = 1;

		return 0;
	}
}

static int8_t search_for_element(const hash_table_t* hash_table, const char key[], int_least32_t storage_index) {
	int_least32_t i = 0;
	/* Search whole table */
	while (i < hash_table->table_size)
	{
		if ((hash_table->keys[storage_index] == NULL) &&
			(hash_table->deleted_flag[storage_index] == 0))
		{
			/* No element on that storage index and there was't element before */
			return -1;
		} else if ((hash_table->keys[storage_index] == NULL) &&
				(hash_table->deleted_flag[storage_index] == 1))
		{
			/* No element on that storage index, but there was element before (continue search) */
			i++;
			if (++storage_index >= hash_table->table_size)
			{
				storage_index = 0;
			}
		} else
		{
			/* There is element, check if it has the same key */
			if (strcmp(hash_table->keys[storage_index], key) == 0)
			{
				/* Keys match */
				return storage_index;
			} else {
				/* Continue search */
				i++;
				if (++storage_index >= hash_table->table_size)
				{
					storage_index = 0;
				}
			}
		}
	}
	/* Element not found in whole table */
	return -1;
}

void delete_hash_table(hash_table_t* hash_table) {
	int_least32_t i;

	for (i = 0; i < hash_table->table_size; i++)
	{
		if (hash_table->keys[i] != NULL)
		{
			free(hash_table->keys[i]);
			free(hash_table->table[i]);
		}
	}

	free(hash_table->keys);
	free(hash_table->table);
	free(hash_table->deleted_flag);
}

static int_least32_t hash_function(const char key[], long long key_len, int_least32_t size) {
	int_fast8_t i;
	int_fast16_t hash_sum = 0;
	double multipcilation_res;
	long long hash_result;
	/* use just hash_lenght chars from key to calculate storage index */
	int8_t index_len = (key_len >= hash_length) ? hash_length : (int8_t) key_len;
	for (i = 0; i < index_len; i++)
	{
		/* multiplying with (i+1) each char val to remove indexing collision in words which have same letters */
		hash_sum += ((int8_t) key[i]) * (i+1);
	}

	multipcilation_res = hash_sum * knuth_const;
	multipcilation_res = (double) fmod(multipcilation_res, 1.0);
	hash_result = (long long) floor(((double)size) * multipcilation_res);

	return (int_least32_t)hash_result;
}

static int8_t linear_probing(hash_table_t* hash_table, const char key[], void* element, int_least32_t storage_index) {
	int_least32_t i = 0;

	/* Try to find free space in table */
	while (i < hash_table->table_size)
	{
		if(strcmp(hash_table->keys[storage_index], key) == 0) {
			/* Write over it */
#ifndef NDEBUG
			printf("Duplicate keys %s, writing over it! \n", key);
#endif
			free(hash_table->keys[storage_index]);
			free(hash_table->table[storage_index]);
			return storage_index;
		} else if(hash_table->keys[storage_index] == NULL)
		{
			/* Found free space, write on it */
#ifndef NDEBUG
			printf("Found free space in hash_map! \n");
#endif
			return storage_index;
		} else
		{
			/* Try to find new free space */
			i++;
			if( ++storage_index >= hash_table->table_size)
			{
				storage_index = 0;
			}
		}
	}
#ifndef NDEBUG
	printf("Table is full ! \n");
#endif
	/* Table is full */
	return -1;
}

