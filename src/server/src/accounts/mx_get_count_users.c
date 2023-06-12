#include "../../../inc/uchat.h"

int mx_get_count_users(void) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;
    int count_users = 0; 

    const char *users_table = "SELECT COUNT(*) FROM users;";
    // Prepare the SELECT statement
    error = sqlite3_prepare_v2(db, users_table, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not select nickname: ", sqlite3_errmsg(db), db);
   
    if (sqlite3_step(stmt) == SQLITE_ROW) 
        count_users = sqlite3_column_int(stmt, 0);
    
    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return count_users;
}

