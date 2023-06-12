#include "../../inc/main.h"

bool mx_check_unique_username(AppData *app_data, const char *username) {
    const char* json_string = "{\"type\":\"Unique username\",\"name\":\"%s\",\"len\":%i}";

    int len_enc;
    char *name = encrypt_data_stream(username, mx_strlen(username), &len_enc);
    int final_json_length = strlen(json_string) + strlen(name) + sizeof(len_enc) + 1;
    char* final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, name, len_enc);

    //write(sock, final_json_string, final_json_length);
    SSL_write(ssl, final_json_string, final_json_length);

    final_json_string = NULL;
    json_string = NULL;

    //read from server part
    char new_message[2048];
    int message_length;

    //message_length = read(sock, new_message, 2048);
    message_length = SSL_read(ssl, new_message, sizeof(new_message));
    new_message[message_length] = '\0';
    printf("Receive message: %s\n", new_message);

    if (message_length == 0) {
        error_window(app_data, "Server error", NULL, NULL);
        // exit(1);
    }

    json_error_t error;
    json_t *root = json_loads(new_message, 0, &error);
    if (!root) {
        printf("Error parsing JSON string: %s\n", error.text);
        return false;
    }

    bool unique = false;
    json_t *unique_json = json_object_get(root, "Unique username");
    if (unique_json && json_is_boolean(unique_json)) {
        unique = json_boolean_value(unique_json);
    }

    json_decref(root);
    mx_strcpy(new_message, "");
    return unique;
}
