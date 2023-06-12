#include "../../../inc/uchat.h"

void mx_delete_all(int chat_id) {
   mx_delete_messages(chat_id);
   mx_delete_members(chat_id);
   mx_delete_chat(chat_id);
}
