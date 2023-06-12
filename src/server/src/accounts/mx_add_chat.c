#include "../../../inc/uchat.h"

int mx_add_new_chat(char *name_chat) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int chat_id = 0;

    const char *add_chat = "INSERT INTO chats (chat_name) VALUES (?1);";
    error = sqlite3_prepare_v2(db, add_chat, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Failed to prepare to add new chat: ", sqlite3_errmsg(db), db);

    sqlite3_bind_text(stmt, 1, name_chat, -1, SQLITE_STATIC); //name_chat
    
    error = sqlite3_step(stmt);
    chat_id = sqlite3_last_insert_rowid(db);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return chat_id;
}

