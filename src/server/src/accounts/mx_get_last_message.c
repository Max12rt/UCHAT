#include "../../../inc/uchat.h"

char *mx_get_last_message(int chat_id) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int count_mes = mx_check_messages(chat_id);
    char *message = "";
    if (count_mes > 0) {
        const char *last_message = "SELECT messages.text_messages FROM messages "
                                    "JOIN members ON messages.members_id = members.members_id "
                                    "JOIN chats ON members.chat_id = chats.chat_id "
                                    "WHERE chats.chat_id = ?1 ORDER BY messages.time DESC LIMIT 1";
    
        // Prepare the SELECT statement
        error = sqlite3_prepare_v2(db, last_message, -1, &stmt, NULL);
        mx_prinerr_tables(error, "Could not select name of chats: ", sqlite3_errmsg(db), db);
   
        sqlite3_bind_int(stmt, 1, chat_id); // chat id
        // Execute the statement and store the results in an array
        if (sqlite3_step(stmt) == SQLITE_ROW)
            message = mx_strdup((const char *)sqlite3_column_text(stmt, 0));
    }
    else {
        message = "";
    }

    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return message;
}

