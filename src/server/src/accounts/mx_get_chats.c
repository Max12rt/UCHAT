#include "../../../inc/uchat.h"

char **mx_get_chats(int user_id) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;

    const char *chats_table = "SELECT chats.chat_name, chats.chat_id FROM chats "
                                "JOIN members ON members.chat_id = chats.chat_id "
                                "JOIN users ON members.user_id = users.user_id "
                                "WHERE users.user_id = ?1;";
    
    // Prepare the SELECT statement
    error = sqlite3_prepare_v2(db, chats_table, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not select name of chats: ", sqlite3_errmsg(db), db);
   
    sqlite3_bind_int(stmt, 1, user_id); // id user

    // Execute the statement and store the results in an array
    int max_rows = mx_get_count_chats(user_id);
    
    char **chats = malloc(sizeof(char *) * max_rows);
    int num_rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && num_rows < max_rows) {
        char *chat = mx_strdup((char*)sqlite3_column_text(stmt, 0));
        char *id = mx_strdup((char*)sqlite3_column_text(stmt, 1));
        chats[num_rows] = mx_strjoin(mx_strjoin(chat, ","), id);
        num_rows++;
        free(chat);
        free(id);
    }
    
    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return chats;
}
