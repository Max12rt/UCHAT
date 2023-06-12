#include "../../../inc/uchat.h"

void mx_prinerr_db(char *part, const char *err, sqlite3 *db){
    mx_printerr(part);
    mx_printerr(err);
    mx_printerr("\n");
    sqlite3_close(db);
}

