#include "../../../inc/uchat.h"

static void mx_prinerr_step(int code, char *part, const char *err, sqlite3 *db){
    if (code != SQLITE_ROW) {
        mx_prinerr_db(part, err, db);
    }
}

int mx_check_members_id(int user, int chat) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int id = 0;
    const char *id_member = "SELECT members_id FROM members WHERE user_id = ?1 AND chat_id = ?2";
    error = sqlite3_prepare_v2(db, id_member, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not find member with the following id-user and id-chat: ", sqlite3_errmsg(db), db);

    sqlite3_bind_int(stmt, 1, user); // id user
    sqlite3_bind_int(stmt, 2, chat); // id chat
    
    error = sqlite3_step(stmt);
    mx_prinerr_step(error, "Could not find member with the following id-user and id-chat: ", sqlite3_errmsg(db), db);
    id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return id > 0 ? id : -1;
}

