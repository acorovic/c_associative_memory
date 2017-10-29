/**
 * @file associative_memory.h
 *
 *  Created on: Oct 19, 2017
 *      Author: rtrk
 */

#ifndef ASSOCIATIVE_MEMORY_H_
#define ASSOCIATIVE_MEMORY_H_

#ifndef NDEBUG
#include <stdio.h>
#endif
#include <assert.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
 * @brief Knuth constant used in hash function
 */
static const double knuth_const = 0.6180339;
static const int8_t hash_length = 20;

/**
 * @brief Hash_table structure, void** elements, char** keys, table size and flag to help working with open addressing
 * @warning Hash_table is using dynamically allocated memory!
 */
typedef struct hash_table {
	void** table;
	char** keys;
	int_least32_t table_size;
	int8_t* deleted_flag;
} hash_table_t;

/**
 * @brief Function to create hash_table
 * @param hash_table Pointer to hash_table which will be initialized
 * @param size Number of elements which hash_table can contain
 * @return 0 if successful, 1 unsuccessful
 */
int8_t create_hash_table(hash_table_t* hash_table, int_least32_t size);

/**
 * @brief Function to insert element to hash_table
 * @param hash_table Pointer to hash_table where element will be inserted
 * @param key Char array key for the element
 * @param element Pointer to the element which will be inserted, inserting it as void*
 * @param type_size Sizeof data type of the element
 * @return 0 if successful, 1 unsuccessful, -1 cannot find free space in table to insert it
 * @warning Not working for deep copy of the structures, eg. structures which contains pointers
 * @warning Putting already existing key in hash_table will lead to overwriting previous value
 */
int8_t insert_to_hash_table(hash_table_t* hash_table, const char key[], void* element, int16_t type_size);

/**
 * @brief Function to get element from hash_table which is connected to key
 * @param hash_table Pointer to hash_table where the search is performed
 * @param key Key of the element
 * @return void* pointer to the element if successful, NULL unsuccessful
 */
void* get_from_hash_table(const hash_table_t* hash_table, const char key[]);

/*
 * @brief Deleting element with the key from the table
 * @param hash_table Pointer to the hash_table from where element will be deleted
 * @param key Key of the element
 * @return 0 if successful, 1 unsuccessful
 */
int8_t delete_from_hash_table(hash_table_t* hash_table, const char key[]);

/*
 * @brief Delete whole hash_table with keys and elements
 * @param hash_table Pointer to the hash_table which will be deleted
 */
void delete_hash_table(hash_table_t* hash_table);



#endif /* ASSOCIATIVE_MEMORY_H_ */
