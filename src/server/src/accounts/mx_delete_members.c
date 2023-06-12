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

void mx_delete_members(int id_chat) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;

    const char *delete_mem = "DELETE FROM members WHERE chat_id = ?1;";
    
    error = sqlite3_prepare_v2(db, delete_mem, -1, &stmt, NULL);
    prinerr_ok(error, "Error preparing delete memebers: ", sqlite3_errmsg(db), db);
   
    error = sqlite3_bind_int(stmt, 1, id_chat); //id chat
    prinerr_ok(error, "Error binding chat name parameter: ", sqlite3_errmsg(db), db);
    
    error = sqlite3_step(stmt);
    printerr_done(error, "Error executing delete members: ", sqlite3_errmsg(db), db);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

