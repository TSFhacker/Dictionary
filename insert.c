#include "sqlite3.h"

#include <stdio.h>

int main(){
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("dict.db", &db);
    if(rc != SQLITE_OK){
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *sql = "DROP TABLE IF EXISTS Dict;"
    "CREATE TABLE Dict(English TEXT, Vietnamese TEXT);"
    "INSERT INTO Dict VALUES('School', 'Truong hoc');"
    "INSERT INTO Dict VALUES('Computer', 'May tinh');"
    "INSERT INTO Dict VALUES('Technology', 'Cong nghe');"
    "INSERT INTO Dict VALUES('Environment', 'Moi truong');"
    "INSERT INTO Dict VALUES('Love', 'Yeu');"
    "INSERT INTO Dict VALUES('Football', 'Bong da');"
    "INSERT INTO Dict VALUES('You', 'Ban');"
    "INSERT INTO Dict VALUES('I', 'Toi');"
    "INSERT INTO Dict VALUES('Money', 'Tien');";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_close(db);
    return 0;
}