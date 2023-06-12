#include "../../../inc/uchat.h"

int mx_get_count_chats(int user_id) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int count_chats = 0;

    const char *chats_table = "SELECT COUNT(chat_name) FROM chats " 
                                "JOIN members ON members.chat_id = chats.chat_id " 
                                "WHERE members.user_id = ?1;";
    
    // Prepare the SELECT statement
    error = sqlite3_prepare_v2(db, chats_table, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not select name of chats: ", sqlite3_errmsg(db), db);
   
    sqlite3_bind_int(stmt, 1, user_id); // id user

    if (sqlite3_step(stmt) == SQLITE_ROW) 
        count_chats = sqlite3_column_int(stmt, 0);
    
    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return count_chats;
}

