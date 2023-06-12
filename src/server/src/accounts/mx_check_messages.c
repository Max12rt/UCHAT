#include "../../../inc/uchat.h"

int mx_check_messages(int chat_id) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int count_mes = 0;

    const char *ms = "SELECT COUNT(messages.messages_id) FROM messages "
                        "JOIN members ON members.members_id = messages.members_id "
                        "JOIN chats ON chats.chat_id = members.chat_id " 
                        "WHERE chats.chat_id = ?1;";
    
    // Prepare the SELECT statement
    error = sqlite3_prepare_v2(db, ms, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not select name of chats: ", sqlite3_errmsg(db), db);
   
    sqlite3_bind_int(stmt, 1, chat_id); 

    if (sqlite3_step(stmt) == SQLITE_ROW) 
        count_mes = sqlite3_column_int(stmt, 0);
    
    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return count_mes > 0 ? count_mes : 0;
}

