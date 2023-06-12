#include "../../../inc/uchat.h"

Messages *mx_get_nick_mess(int chat_id, int count_mess) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int count_members = mx_get_count_members(chat_id);
    if (count_mess > 0 && count_members > 0) {
        const char *nick_mess = "SELECT users.username, messages.time, messages.text_messages " 
                                "FROM messages "
                                "JOIN members ON messages.members_id = members.members_id "
                                "JOIN chats ON members.chat_id = chats.chat_id " 
                                "JOIN users ON users.user_id = ABS(members.user_id) "
                                "WHERE chats.chat_id = ?1 " 
                                "ORDER BY messages.time ASC;"; // DESC

        error = sqlite3_prepare_v2(db, nick_mess, -1, &stmt, NULL);
        mx_prinerr_tables(error, "Could not select name of chats: ", sqlite3_errmsg(db), db);
   
        sqlite3_bind_int(stmt, 1, chat_id); // chat id

        Messages *messages = malloc(sizeof(Messages) * count_mess);

        int i = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW && i < count_mess) {
            messages[i].name = mx_strdup((char*)sqlite3_column_text(stmt, 0));
            messages[i].time = mx_strdup((char*)sqlite3_column_text(stmt, 1));
            messages[i].text = mx_strdup((char*)sqlite3_column_text(stmt, 2));
            i++;
        }
        // char **message = malloc(sizeof(char *) * count_mess);
        // for (int i = 0; i < count_mess; i++)
        //     message[i] = malloc(4 * sizeof(char));

        // int i = 0;
        // while (sqlite3_step(stmt) == SQLITE_ROW && i < count_mess) {
        //     message[i * 3] = mx_strdup((char*)sqlite3_column_text(stmt, 0));
        //     message[i * 3 + 1] = mx_strdup((char*)sqlite3_column_text(stmt, 1));
        //     message[i * 3 + 2] = mx_strdup((char*)sqlite3_column_text(stmt, 2));
        //     i++;
        // }
        // for (int i = 0; i < count_mess; i++) {
        //     mx_printstr(message[i * 3]);
        //     mx_printstr("\n");
        //     mx_printstr(message[i * 3 + 1]);
        //     mx_printstr("\n");
        //     mx_printstr(message[i * 3 + 2]);
        //     mx_printstr("\n");
        // }

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return messages;
    }
    
    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return NULL;
}
