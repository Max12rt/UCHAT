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

void mx_edit_message(int id_message, char *new_mess) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;

    const char *delete_messages = "UPDATE messages SET text_messages = ?1 WHERE messages_id = ?2;";
    error = sqlite3_prepare_v2(db, delete_messages, -1, &stmt, NULL);
    prinerr_ok(error, "Error preparing delete messages: ", sqlite3_errmsg(db), db);
   
    sqlite3_bind_text(stmt, 1, new_mess, -1, SQLITE_STATIC); // new text
    sqlite3_bind_int(stmt, 2, id_message); // id messages

    
    error = sqlite3_step(stmt);
    printerr_done(error, "Error executing delete messages: ", sqlite3_errmsg(db), db);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

