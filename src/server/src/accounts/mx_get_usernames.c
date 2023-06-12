#include "../../../inc/uchat.h"

char **mx_get_usernames(void) {
    sqlite3 *db = mx_open_db();
    sqlite3_stmt *stmt = NULL;
    int error = 0;

    const char *usernames_table = "SELECT username FROM users;";
    // Prepare the SELECT statement
    error = sqlite3_prepare_v2(db, usernames_table, -1, &stmt, NULL);
    mx_prinerr_tables(error, "Could not select nickname: ", sqlite3_errmsg(db), db);
   

   // Execute the statement and store the results in an array
    int max_rows = mx_get_count_users();
    
    char **names = malloc(sizeof(char *) * max_rows);
    int num_rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && num_rows < max_rows) {
        names[num_rows] = mx_strdup((const char*)sqlite3_column_text(stmt, 0));
        num_rows++;
    }
    
    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return names;
}

