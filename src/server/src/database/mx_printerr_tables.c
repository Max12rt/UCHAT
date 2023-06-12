#include "../../../inc/uchat.h"

void mx_prinerr_tables(int code, char *part, const char *err, sqlite3 *db){
    if (code != SQLITE_OK) {
        mx_prinerr_db(part, err, db);
    }
}

