#include "../../../inc/uchat.h"

static void mx_prinerr_step(int code, char *part, const char *err, sqlite3 *db){
    if (code != SQLITE_DONE) {
        mx_prinerr_db(part, err, db);
    }
}

int mx_add_member_id(int user, int chat) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    const char *add_member = "INSERT INTO members(user_id, chat_id) VALUES (?1, ?2)";
    error = sqlite3_prepare_v2(db, add_member, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Failed to prepare to add new member: ", sqlite3_errmsg(db), db);

    sqlite3_bind_int(stmt, 1, user); //user id
    sqlite3_bind_int(stmt, 2, chat); //chat id

    error = sqlite3_step(stmt);
    mx_prinerr_step(error, "Failed to add member: ", sqlite3_errmsg(db), db);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return mx_check_members_id(user, chat);
}
