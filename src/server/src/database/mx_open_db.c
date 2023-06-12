#include "../../../inc/uchat.h"

sqlite3 *mx_open_db(void) {
    //відкрита база даних
    sqlite3 *db = NULL;
    //відривається файл бази даних
    int code = sqlite3_open("uchat.db", &db);
    //перевірка відкритої бази даних
    if (code != SQLITE_OK) {
        mx_prinerr_db("Can not open database: ", sqlite3_errmsg(db), db);
    }
    return db;
}

