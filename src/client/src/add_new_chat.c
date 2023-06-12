#include "../../inc/main.h"

char *add_new_chat_to_DB(AppData *app_data, char **usernames, int usernames_count, const char *title) {
    // Create a JSON array for usernames
    json_t *usernames_array = json_array();
    json_t *lens_array = json_array();
    for (int i = 0; i < usernames_count; i++) {
        int temp_len = 0;
        char *temp_username = encrypt_data_stream(usernames[i], mx_strlen(usernames[i]), &temp_len);
        json_array_append_new(usernames_array, json_string(temp_username));
        json_array_append_new(lens_array, json_integer(temp_len));
    }

    // Convert the JSON array to a string
    char *usernames_json_string = json_dumps(usernames_array, JSON_COMPACT);
    int usernames_json_string_length = strlen(usernames_json_string);
    
    char *lens_json_string = json_dumps(lens_array, JSON_COMPACT);
    int len_user = mx_strlen(lens_json_string);
    
    // encryt title
    int len_enc2;
    char *title_cod = encrypt_data_stream(title, mx_strlen(title), &len_enc2);

    const char *json_string = "{\"type\":\"add room\",\"title\":\"%s\",\"len_t\":%i,\"usernames\":%s,\"len_u\":%s,\"count\":%i}";
    int final_json_length = mx_strlen(json_string) + len_enc2 + sizeof(len_enc2) +
                             usernames_json_string_length  + len_user +
                             mx_strlen(mx_itoa(usernames_count)) + 1;
    char *final_json_string = (char *)malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, title_cod, len_enc2, usernames_json_string, lens_json_string, usernames_count);
    
    //write(sock, final_json_string, final_json_length);
    SSL_write(ssl, final_json_string, final_json_length);
    
    mx_strdel(&title_cod);
    mx_strdel(&lens_json_string);
    mx_strdel(&usernames_json_string);
    json_decref(usernames_array);
    mx_strdel(&final_json_string);
    
    char new_message[2048];
    int message_length;

    //message_length = read(sock, new_message, 2048);
    message_length = SSL_read(ssl, new_message, sizeof(new_message));
    new_message[message_length] = '\0';
    printf("Receive message: %s\n", new_message);

    if (message_length == 0) {
        error_window(app_data, "Server error", NULL, NULL);
    }

    json_error_t error;
    json_t *root = json_loads(new_message, 0, &error);
    if (!root) {
        printf("Error parsing JSON string: %s\n", error.text);
        return NULL;
    }

    json_t *json_room_id = json_object_get(root, "chat_id");
    char *id = (char *) json_string_value(json_room_id);
    json_decref(root);

    return id;
}
