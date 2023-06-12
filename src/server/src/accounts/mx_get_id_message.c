#include "../../../inc/uchat.h"

static void mx_prinerr_step(int code, char *part, const char *err, sqlite3 *db){
    if (code != SQLITE_ROW) {
        mx_prinerr_db(part, err, db);
    }
}

int mx_get_id_message(int chat_id, char *username, char *text, char *time) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt;
    int error = 0;
    int id = 0;
    const char *id_message = "SELECT messages_id FROM messages "
                            "JOIN members ON members.members_id = messages.members_id "
                            "JOIN users ON users.user_id = members.user_id "
                            "JOIN chats ON chats.chat_id = members.chat_id "    
                            "WHERE chats.chat_id = ?1 AND users.username = ?2 AND messages.text_messages = ?3 AND messages.time = ?4;";
    error = sqlite3_prepare_v2(db, id_message, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not find user with the following name: ", sqlite3_errmsg(db), db);

    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC); //username
    sqlite3_bind_text(stmt, 3, text, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, time, -1, SQLITE_STATIC);
    
    error = sqlite3_step(stmt);
    mx_prinerr_step(error, "Could not find user with the following name: ", sqlite3_errmsg(db), db);
    id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id > 0 ? id : -1;
}
