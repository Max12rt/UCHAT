#include "../../../inc/uchat.h"

static void mx_prinerr_step(int code, char *part, const char *err, sqlite3 *db){
    if (code != SQLITE_DONE) {
        mx_prinerr_db(part, err, db);
    }
}

int mx_add_new_user(char *username, char *hash) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int check = mx_check_nick(username);
    if (check == -1) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 0;
    }
    else if (check == 0) {     
        const char *add_user = "INSERT INTO users(username, password) VALUES (?1, ?2)";
        error = sqlite3_prepare_v2(db, add_user, -1, &stmt, NULL);
        mx_prinerr_tables(error, "Failed to prepare to add new user: ", sqlite3_errmsg(db), db);

        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC); //username
        sqlite3_bind_text(stmt, 2, hash, -1, SQLITE_STATIC); //hash password

        error = sqlite3_step(stmt);
        mx_prinerr_step(error, "Failed to add user: ", sqlite3_errmsg(db), db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    return mx_get_id(username);
}

