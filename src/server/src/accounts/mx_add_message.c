#include "../../../inc/uchat.h"

static void mx_prinerr_step(int code, char *part, const char *err, sqlite3 *db){
    if (code != SQLITE_DONE) {
        mx_prinerr_db(part, err, db);
    }
}

int mx_add_message(int user, int chat, char *text) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt;
    int error = 0;
    int message_id = 0;

    int member_id = mx_check_members_id(user, chat);
    const char *add_message = "INSERT INTO messages(members_id, text_messages) VALUES (?1, ?2)";
    error = sqlite3_prepare_v2(db, add_message, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Failed to prepare to add new message: ", sqlite3_errmsg(db), db);

    sqlite3_bind_int(stmt, 1, member_id); // member_id
    sqlite3_bind_text(stmt, 2, text, -1, SQLITE_STATIC); // text message

    error = sqlite3_step(stmt);
    message_id = sqlite3_last_insert_rowid(db);
    mx_prinerr_step(error, "Failed to add new message: ", sqlite3_errmsg(db), db);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return message_id;
}

