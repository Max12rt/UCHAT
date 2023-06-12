#include "../../../inc/uchat.h"

int mx_get_count_pos_members(int chat_id) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int count_members = 0; 

    const char *users_table = "SELECT COUNT(*) FROM members "
                                "JOIN chats ON members.chat_id = chats.chat_id "
                                "WHERE chats.chat_id = ?1 AND members.user_id > 0;";
    
    error = sqlite3_prepare_v2(db, users_table, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not select count with members: ", sqlite3_errmsg(db), db);
    
    sqlite3_bind_int(stmt, 1, chat_id); // chat id

    if (sqlite3_step(stmt) == SQLITE_ROW) 
        count_members = sqlite3_column_int(stmt, 0);
    
    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return count_members;
}
