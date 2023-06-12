#include "../../../inc/uchat.h"

static void mx_prinerr_step(int code, char *part, const char *err, sqlite3 *db){
    if (code != SQLITE_ROW) {
        mx_prinerr_db(part, err, db);
    }
}

int mx_check_credentials(char *username, char *hash) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int id = 0;
    const char *id_user = "SELECT user_id FROM users WHERE username = ?1 AND password = ?2";
    error = sqlite3_prepare_v2(db, id_user, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not find user with the following name: ", sqlite3_errmsg(db), db);

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC); //username
    sqlite3_bind_text(stmt, 2, hash, -1, SQLITE_STATIC); //hash password
    
    error = sqlite3_step(stmt);
    mx_prinerr_step(error, "Could not find user with the following name: ", sqlite3_errmsg(db), db);
    id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id > 0 ? id : -1;
}

