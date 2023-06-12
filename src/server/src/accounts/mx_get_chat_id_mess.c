#include "../../../inc/uchat.h"

char **mx_get_chat_id_mess(int user_id) {
    int max_rows = mx_get_count_chats(user_id);
    char **chats = mx_get_chats(user_id);
    char **temp = chats;

    char **last_mess = malloc(sizeof(char *) * max_rows); 
    for (int i = 0; i < max_rows; i++) {
        int temp_int = mx_get_substring(temp[i]);
        last_mess[i] = mx_get_last_message(temp_int);
    }
    
    char **list_chats = malloc(sizeof(char *) * max_rows); 

    for (int i = 0; i < max_rows; i++) 
        list_chats[i] = mx_strjoin(mx_strjoin(chats[i], ","), last_mess[i]);
    
    // mx_strdel(&chats);
    // mx_strdel(&last_mess);
    return list_chats;
}
