#include "../../../inc/uchat.h"

// username - UNIQUE
static void create_table_users(sqlite3 *db, char *err_msg) {
    char *table_users = "CREATE TABLE IF NOT EXISTS users ("
                            "user_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                            "username TEXT UNIQUE NOT NULL, "
                            "password TEXT NOT NULL);";
    int code = sqlite3_exec(db, table_users, 0, 0, &err_msg);
    mx_prinerr_tables(code, "Can not create table 'users': ", err_msg, db);
}

static void create_table_chats(sqlite3 *db, char *err_msg) {
    char *table_chats = "CREATE TABLE IF NOT EXISTS chats ("
                            "chat_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                            "chat_name TEXT NOT NULL);";
    int code = sqlite3_exec(db, table_chats, 0, 0, &err_msg);
    mx_prinerr_tables(code, "Can not create table 'chats': ", err_msg, db);
    
    char *table_members = "CREATE TABLE IF NOT EXISTS members ("
                            "members_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                            "user_id INTEGER NOT NULL, "
                            "chat_id INTEGER NOT NULL, "
                            "FOREIGN KEY(user_id) REFERENCES users(user_id), "
                            "FOREIGN KEY(chat_id) REFERENCES chats(chat_id));";
    code = sqlite3_exec(db, table_members, 0, 0, &err_msg);
    mx_prinerr_tables(code, "Can not create table 'members': ", err_msg, db);
}

// IF NOT EXISTS
static void create_table_messages(sqlite3 *db, char *err_msg) {
    char *table_messages = "CREATE TABLE IF NOT EXISTS messages ("
                                "messages_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                                "members_id INTEGER NOT NULL, "
                                "text_messages TEXT NOT NULL, "
                                "time DATETIME DEFAULT CURRENT_TIMESTAMP, "
                                "FOREIGN KEY(members_id) REFERENCES members(members_id));";
    int code = sqlite3_exec(db, table_messages, 0, 0, &err_msg);
    mx_prinerr_tables(code, "Can not create table 'messages': ", err_msg, db);
}


void mx_create_db(void) {
    sqlite3 *db = mx_open_db();
    char *err_msg = 0;
    create_table_users(db, err_msg);
    create_table_chats(db, err_msg);
    create_table_messages(db, err_msg);
    sqlite3_free(err_msg);
    sqlite3_close(db);
}

