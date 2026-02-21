#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "database.h"
void db_list(Database *db);
void formatted_print_rec(Record *record);
void smaller_sort(Record *records, int size);
void swap(Record *a, Record *b);
int compare_rec_handles(const void *a, const void *b);

int main_loop(Database * db)
{
	char input[256];
	int unsaved_changes = 0;
	printf("Loaded %d records.\n", db->size);    
	while (1) {
		printf("> ");
		if (fgets(input, sizeof(input), stdin) == NULL) {
			break;
		}
	input[strcspn(input, "\n")] = '\0'; //removing newline
	
	char *command = strtok(input, " ");
	if (!command) continue;

	if (strcmp(input, "list")==0){ //function 'list'
		db_list(db);
    	} else if (strcmp(input, "add")==0){ // function 'add HANDLE FOLLOWERS'
		char *handle = strtok(NULL, " ");
		char *followers_str = strtok(NULL, " ");
		if ((!handle) || (!followers_str)) {
			printf("Error: usage: add HANDLE FOLLOWERS\n");
			continue;
		} else if (strlen(handle)>27) {
			printf("Error: handle too long.\n");
				continue;
		}
		// check if follower count is an integer
		char *first_non_int;
		unsigned long int followers = strtol(followers_str, &first_non_int, 10);
		if (*first_non_int != '\0') {
			printf("Error: follower count must be an integer.\n");
			continue;
		}
		// lookup if handle alr exists
		Record *record = db_lookup(db, handle);
		if (record) {
			printf("Error: handle %s already exists.\n", handle);
			continue;
		}
		printf("Comment> ");
		char comment[64];
		fgets(comment, sizeof(comment), stdin);
		comment[strcspn(comment, "\n")] = '\0'; //removing newline
		if (strchr(comment, ',')){
			printf("Error: comment cannot contain commas.\n");
			continue;
		}
		// create the record & update time last modified
		Record new_rec;
		strncpy(new_rec.handle, handle, 20);
		new_rec.follower_count = (unsigned long int)followers;
		strncpy(new_rec.comment, comment, 64);
		new_rec.date_last_modified = time(NULL);
		// add record to database
		db_append(db, &new_rec);
		unsaved_changes = 1;
	} else if (strcmp(input, "update")==0){
		char *handle = strtok(NULL, " ");
                char *followers_str = strtok(NULL, " ");
                if ((!handle) || (!followers_str)) {
                        printf("Error: usage: update HANDLE FOLLOWERS\n");
                        continue;
                } else if (strlen(handle)>20) {
                        printf("Error: handle too long.\n");
                                continue;
                }
                // check if follower count is an integer
                char *first_non_int;
                long followers = strtol(followers_str, &first_non_int, 10);
                if (*first_non_int != '\0') {
                        printf("Error: follower count must be an integer.\n");
                        continue;
                }
                // lookup if handle exists
		Record *record = db_lookup(db, handle);
                if (!record) {
                        printf("Error: no entry with handle %s.\n", handle);
			continue;
                }
		// if record exists, it's that one we want to update
		// prompt comment
		printf("Comment> ");
                char comment[64];
                fgets(comment, sizeof(comment), stdin);
                comment[strcspn(comment, "\n")] = '\0'; //removing newline
                if (strchr(comment, ',')){
                        printf("Error: comment cannot contain commas.\n");
                        continue;
                }
                // update the record & update time last modified
		record->follower_count = (unsigned long int)followers;
		strncpy(record->comment, comment, 64);
		record->date_last_modified = time(NULL);
		unsaved_changes = 1;
	} else if (strcmp(input, "sort")==0){
		// sort and write records into csv file when #recs small
		if (db->size<4) {
			smaller_sort(db->records,db->size);
		} else {
			qsort(db->records, db->size, sizeof(Record), compare_rec_handles);
		}		

		db_write_csv(db, "database.csv");
                printf("Wrote %d records.\n", db->size);
		unsaved_changes = 0;

	} else if (strcmp(input,"save")==0){
		db_write_csv(db, "database.csv");
		printf("Wrote %d records.\n", db->size);
		unsaved_changes = 0;
	} else if (strcmp(input, "exit")==0){
		char *force = strtok(NULL, " ");
		if ((unsaved_changes) && ((!force) || !(strcmp(force, "fr")==0))) {
				printf("Error: you did not save your changes. Use `exit fr` to force exiting anyway.\n");
				continue;
		}
		break;
	} else {
		printf("Error: Enter a real command.\n");
		continue;
	}
	}

	return 0;
}

void db_list(Database *db) 
{
	printf("HANDLE              | FOLLOWERS | LAST MODIFIED       | COMMENT\n");
  	printf("--------------------|-----------|---------------------|-------------------------\n");
	for (int i=0; i < db->size; i++){
		formatted_print_rec(&(db->records[i]));
	}
}

void formatted_print_rec(Record *record)
{
	// Format: HANDLE    | FOLLOWERS | LAST MODIFIED   | COMMENT
	struct tm *formatted_time;
	char time[20];
	formatted_time = localtime(&(record->date_last_modified));
	strftime(time, 20, "%Y-%m-%d %H:%M", formatted_time);
	
	if (strlen(record->handle) <= 20) {
		if (strlen(record->comment) > 25) {
			printf("%-20.20s| %-10lu| %-20s| %-22.22s...\n", record->handle, record->follower_count, time, record->comment);
		} else {
			printf("%-20.20s| %-10lu| %-20s| %-25.25s\n", record->handle, record->follower_count, time, record->comment);
		}
	} else {
		if (strlen(record->comment) > 25) {
                        printf("%-17.17s...| %-10lu...| %-20s| %-22.22s...\n", record->handle, record->follower_count, time, record->comment);
                } else {
			printf("%-17.17s...| %-10lu| %-20s| %-25.25s\n", record->handle, record->follower_count, time, record->comment);
		}
	}
        

}

void smaller_sort(Record *records, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (records[j].follower_count > records[j + 1].follower_count) {
                swap(&records[j], &records[j + 1]);
            }
        }
    }
}

void swap(Record *a, Record *b) {
    Record temp = *a;
    *a = *b;
    *b = temp;
}

	

int compare_rec_handles(const void *a, const void *b)
{
	const Record *rec1 = (const Record *)a;
	const Record *rec2 = (const Record *)b;
	return (strcmp(rec1->handle, rec2->handle));
}


int main()
{
    Database db = db_create();
    db_load_csv(&db, "database.csv");
    return main_loop(&db);
}
