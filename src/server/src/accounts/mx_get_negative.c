#include "../../../inc/uchat.h"

int *mx_get_negative(int chat_id) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int count_negative = mx_get_count_negative_members(chat_id); 
    int *arr_neg = malloc(sizeof(int) * count_negative);

    const char *negative_id = "SELECT user_id FROM members "
                                "JOIN chats ON members.chat_id = chats.chat_id "
                                "WHERE chats.chat_id = ?1 AND members.user_id < 0;";
    
    error = sqlite3_prepare_v2(db, negative_id, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not select negative id: ", sqlite3_errmsg(db), db);
    
    sqlite3_bind_int(stmt, 1, chat_id); // chat id
    
    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < count_negative) {
        arr_neg[i] = sqlite3_column_int(stmt, 0);
        i++;
    }
    
    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return arr_neg;
}
