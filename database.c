#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define HANDLE_SIZE 32
#define COMMENT_SIZE 64

Database db_create() {
    Database db;
    db.size = 0;
    db.capacity = 4;
    db.records = malloc(db.capacity * sizeof(Record)); 
    if (!db.records) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    return db;
}


void db_append(Database *db, Record const *item) {
    if (db->size >= db->capacity) {
        db->capacity *= 2;
        db->records = realloc(db->records, db->capacity * sizeof(Record));  
        if (!db->records) {
            fprintf(stderr, "Memory allocation failed during resizing!\n");
            exit(1);
        }
    }

    
    db->records[db->size] = *item;
    db->size++;  
}


Record *db_index(Database *db, int index) {
    if (index >= 0 && index < db->size) {
        return &db->records[index];  
    } else {
        return NULL;  
    }
}


Record *db_lookup(Database *db, char const *handle) {
    for (int i = 0; i < db->size; i++) {
        if (strcmp(db->records[i].handle, handle) == 0) {
            return &db->records[i];  
        }
    }
    return NULL;  
}


void db_free(Database *db) {
    free(db->records);  
    db->records = NULL;
    db->size = 0;
    db->capacity = 0;
}


char *getTime(){
  time_t now = time(NULL); 

  struct tm *current = localtime(&now);

  char *str = malloc(200);

  strftime(str, 200,"%Y-%m-%d %H:%M:%S", current);

  return str;
}

Record parse_record(char const *line){
  Record record;
  record.handle[0] = '\0';
  record.follower_count = -1;
  record.comment[0] = '\0';
  record.date_last_modified = -1;

  
  char current[300]; 
  strcpy(current, line);

  const char *delimiter = ",";
  char *element; 
  

  element = strtok(current, delimiter);

  while(element != NULL){
   
    if(strlen(record.handle) == 0){
      strcpy(record.handle, element);      
    }

    else if(record.follower_count == -1){
      record.follower_count = strtol(element, NULL, 10);
    }
    else if(strlen(record.comment) == 0){
      strcpy(record.comment, element);
    }
    else{
      record.date_last_modified = strtol(element, NULL, 10);
    }
    element = strtok(NULL, delimiter); 
  }
return record;
}

void db_load_csv(Database *db, char const *path){
  
  FILE *file =  fopen(path, "r");

  if(file == NULL){
    puts("Failed to open file to read");
    return;
  }

  size_t length = 0;
  char *line = NULL;
  Record record;
  while(getline(&line, &length, file) != -1){
    record = parse_record(line);
    db_append(db, &record);
  }

  free(line);
  fclose(file);

}
void db_write_csv(Database *db, char const *path){
  
  FILE *file = fopen(path, "w");
  if(file == NULL){
    puts("Failed to open file to write");
    return;
  }

  Record record;
  for(int i = 0; i < db->size; i++){
    record = db->records[i];
    fprintf(file,"%s,", record.handle);
    fprintf(file, "%lu,", record.follower_count);
    fprintf(file, "%s,", record.comment);
    fprintf(file, "%lu\n", record.date_last_modified);
  }

  fclose(file);
  
}

