#include "../../../inc/uchat.h"

void mx_delete(int chat_id, int id_user) {
    int count_members = mx_get_count_pos_members(chat_id);

    if (count_members <= 2) 
        mx_delete_all(chat_id);
    
    else if (count_members > 2) {
        mx_add_message(id_user, chat_id, "One of the users left this chat");
        mx_delete_one_user_chat(chat_id, id_user);
    }
}
