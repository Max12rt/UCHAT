#include "../../../inc/uchat.h"

static void prinerr_ok(int code, char *part, const char *err, sqlite3 *db) {
    if (code != SQLITE_OK) {
        mx_prinerr_db(part, err, db);
    }
}

static void printerr_done(int code, char *part, const char *err, sqlite3 *db) {
    if (code != SQLITE_DONE) {
        mx_prinerr_db(part, err, db);
    }
}

void mx_delete_one_user_chat(int id_chat, int id_user) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int members_id = mx_check_members_id(id_user, id_chat);
    
    const char *delete_one_user = "UPDATE members SET user_id = ?1 WHERE members_id = ?2";
    
    error = sqlite3_prepare_v2(db, delete_one_user, -1, &stmt, NULL);
    prinerr_ok(error, "Error preparing update user_id: ", sqlite3_errmsg(db), db);
    
    id_user *= -1;
    error = sqlite3_bind_int(stmt, 1, id_user); // members id
    error = sqlite3_bind_int(stmt, 2, members_id);
    prinerr_ok(error, "Error binding user_id parameter: ", sqlite3_errmsg(db), db);
    
    error = sqlite3_step(stmt);
    printerr_done(error, "Error executing update user_id: ", sqlite3_errmsg(db), db);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}
