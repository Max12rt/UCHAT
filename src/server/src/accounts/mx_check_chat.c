#include "../../../inc/uchat.h"

static void mx_prinerr_step(int code, char *part, const char *err, sqlite3 *db){
    if (code != SQLITE_ROW) {
        mx_prinerr_db(part, err, db);
    }
}

int mx_check_chat(const char *name_chat) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int id = 0;
    const char *id_chat = "SELECT chat_id FROM chats WHERE chat_name = ?1";
    error = sqlite3_prepare_v2(db, id_chat, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not find chat with the following name: ", sqlite3_errmsg(db), db);

    sqlite3_bind_text(stmt, 1, name_chat, -1, SQLITE_STATIC); //name_chat
    
    error = sqlite3_step(stmt);
    mx_prinerr_step(error, "Could not find chat with the following name: ", sqlite3_errmsg(db), db);
    id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id > 0 ? id : -1;
}

