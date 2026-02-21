#ifndef DB_H
#define DB_H

typedef struct Record { 
	char handle[32];  //Handle size:32
	unsigned long int follower_count;  
    	char comment[64];  // Comment size:64
    	unsigned long int date_last_modified;  

} Record;

typedef struct Database { 
	Record *records;  // Pointer to array of Record
    	int size;         // number of records in the database
    	int capacity;     // capacity of the records 
		     
} Database;

Database db_create();

void db_append(Database * db, Record const * item);

Record *db_index(Database * db, int index);

Record *db_lookup(Database * db, char const * handle);


void db_free(Database * db);

void db_load_csv(Database * db, char const * path);

void db_write_csv(Database * db, char const * path);

#endif
