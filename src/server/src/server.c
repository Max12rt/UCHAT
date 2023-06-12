#include "../../inc/uchat.h"

//#define NUM_THREADS 10

//int i = 0;
void *handle_client(void *arg) {
    int client = *(int*)arg;
    SSL *ssl = (SSL*)arg;

    if (ssl > 0) mx_printstr("ssl accepted!\n");

    // Handle incoming messages
    while (1) {
        char buf[2048];
        int bytes = SSL_read(ssl, buf, sizeof(buf) - 1);
        if(bytes <= 0) {
            //sleep(1000);
            continue;
        }
        //printf("bytes = %d\n", bytes);
        //if(bytes > 0){
            buf[bytes] = '\0';
            printf("Received data: %s\n", buf);
            //mx_printstr("bytes > 0\n");
            // Обробка вхідних даних

            json_error_t error;
            json_t* json = json_loads(buf, 0, &error);
            json_t *response_json = json_object();

            // Get values from the JSON object
            const char* message_type = json_string_value(json_object_get(json, "type"));
            printf("message type: %s\n", message_type);

            char *response_str = NULL;

            // Sing in / up
            bool type1 = (mx_strcmp(message_type, "Registration") == 0);
            bool type2 = (mx_strcmp(message_type, "Authentication") == 0);
            bool type3 = (mx_strcmp(message_type, "Unique username") == 0);
            bool type4 = (mx_strcmp(message_type, "chats list") == 0);
            bool type5 = (mx_strcmp(message_type, "add room") == 0);
            bool type6 = (mx_strcmp(message_type, "delete chat") == 0);
            bool type7 = (mx_strcmp(message_type, "load") == 0);
            bool type8 = (mx_strcmp(message_type, "add message") == 0);
            bool type9 = (mx_strcmp(message_type, "delete message") == 0);
            bool type10 = (mx_strcmp(message_type, "edit message") == 0);
            bool type11 = (mx_strcmp(message_type, "LOGOUT") == 0);

            if (type11) {
                //bufferevent_free(buf);
                printf("Connection closed\n");
            }

            // With responce to client
            if (type1 || type2 || type3 || type4 || type5 || type7 || type8) {
                char **chats_list = NULL;

                if (type1 || type2) {
                    const char* username_change = json_string_value(json_object_get(json, "name"));
                    char* password = (char *) json_string_value(json_object_get(json, "password"));
                    int len_enc = json_integer_value(json_object_get(json, "len"));
                    char* username = (char *) decrypt_data_stream(username_change, len_enc);
                        
                    int id_user = 0;
                        
                    if(type1){
                        // printf("Received password: 1");
                        id_user = mx_add_new_user(username, password);
                    }

                    if(type2){
                        // printf("Received password: 2");
                        id_user = mx_check_credentials(username, password);
                    }

                    // Create a JSON object to send back to the client
                    json_object_set_new(response_json, "ID", json_integer(id_user));

                } else if (type3) {
                    bool username_taken = true;
                    char **all_names = mx_get_usernames();

                    if (all_names != NULL && all_names[0] != NULL) {
                        const char* name = json_string_value(json_object_get(json, "name"));
                        int len_enc = json_integer_value(json_object_get(json, "len"));
                        const char* username = decrypt_data_stream(name, len_enc);

                        for (int i = 0; i < mx_get_count_users(); i++) {
                            if (mx_strcmp(username, all_names[i]) == 0) {
                                username_taken = false;
                                break;
                            }
                        }
                    }

                    // Create a JSON object to send back to the client
                    json_object_set_new(response_json, "Unique username", json_boolean(username_taken));

                } else if (type4) {
                    int id = json_integer_value(json_object_get(json, "id"));
                    chats_list = mx_get_chat_id_mess(id);

                    json_t *json_array_result = json_array();
                    json_t *json_array_lens = json_array();
                    int count = mx_get_count_chats(id);

                    printf("Count of chats: %d\n", count);

                    for (int i = 0; i < count; i++) {
                        int temp_len = 0;
                        char *temp = encrypt_data_stream(chats_list[i], mx_strlen(chats_list[i]), &temp_len); 
          
                        json_array_append_new(json_array_result, json_string(temp));
                        json_array_append_new(json_array_lens, json_integer(temp_len));
                        free(temp); 
                    }
                    json_object_set_new(response_json, "count", json_integer(count));
                    json_object_set_new(response_json, "list", json_array_result);
                    json_object_set_new(response_json, "lens", json_array_lens);
                } else if (type5) {
                    char *title_be = (char *) json_string_value(json_object_get(json, "title"));
                    int len_tit = json_integer_value(json_object_get(json, "len_t"));
                    char *title = (char *) decrypt_data_stream(title_be, len_tit);

                    json_t *usernames_array = json_object_get(json, "usernames");
                    json_t *len_usernames = json_object_get(json, "len_u");
                    int usernames_count = json_integer_value(json_object_get(json, "count"));
                    char **usernames = malloc(usernames_count * sizeof(char *));
            
                    for (int i = 0; i < usernames_count; i++) {
                        json_t *username_value = json_array_get(usernames_array, i);
                        json_t *len_value = json_array_get(len_usernames, i);
                        int len_u = json_integer_value(len_value);
                        const char *username_be = json_string_value(username_value);
                        const char *username = decrypt_data_stream(username_be, len_u);
                        usernames[i] = mx_strdup(username);
                    }

                    int room_id = mx_add_new_chat(title);
                    for (int i = 0; i < usernames_count; i++) {
                        mx_add_member_id(mx_get_id(usernames[(int) i]), room_id);
                        free(usernames[(int) i]);
                    }

                    // Create a JSON object to send back to the client
                    json_object_set_new(response_json, "chat_id", json_string(mx_itoa(room_id)));
                    free(usernames);
                } else if (type7) {
                    int chat_id = json_integer_value(json_object_get(json, "room id"));

                    int count = mx_get_count_messages(chat_id);
                    Messages *messages = mx_get_nick_mess(chat_id, count);
                        
                    if (messages == NULL) {
                        json_object_set_new(response_json, "count", json_integer(count));
                        json_object_set_new(response_json, "messages", NULL);
                        json_object_set_new(response_json, "len messages", NULL);
                    }

                    json_t *array_messages = json_array();
                    json_t *array_messages_id = json_array();
                    json_t *array_messages_len = json_array();

                    for (int i = 0; i < count; i++) {
                        char *name_original = messages[i].name;
                        char *time_original = messages[i].time;
                        char *text_original = messages[i].text;

                        int len_name;
                        char *name = encrypt_data_stream(name_original, mx_strlen(name_original), &len_name);

                        int len_time;
                        char *time = encrypt_data_stream(time_original, mx_strlen(time_original), &len_time);

                        int len_text;
                        char *text = encrypt_data_stream(text_original, mx_strlen(text_original), &len_text);

                        mx_printstr("Name:");
                        mx_printstr(name_original);
                        mx_printstr("\nText:");
                        mx_printstr(text_original);
                        mx_printstr("\nTime:");
                        mx_printstr(time_original);
                        mx_printstr("\n");
                        mx_printstr("1+\n");
                        int message_id = mx_get_id_message(chat_id, name_original, text_original, time_original);
                        mx_printstr("2+\n");
                        mx_printint(message_id);
                        mx_printstr("\n");
                        json_t *temp_array = json_array();
                        json_t *temp_array_len = json_array();

                        json_array_append_new(temp_array, json_string(name));
                        json_array_append_new(temp_array, json_string(time));
                        json_array_append_new(temp_array, json_string(text));

                        json_array_append_new(temp_array_len, json_integer(len_name));
                        json_array_append_new(temp_array_len, json_integer(len_time));
                        json_array_append_new(temp_array_len, json_integer(len_text));
                        json_array_append_new(array_messages, temp_array);
                        json_array_append_new(array_messages_id, json_integer(message_id));
                        json_array_append_new(array_messages_len, temp_array_len);
                    }
                    free(messages);
                    // Create a JSON object to send back to the client
                    json_object_set_new(response_json, "count", json_integer(count));
                    json_object_set_new(response_json, "messages", array_messages);
                    json_object_set_new(response_json, "len messages", array_messages_len);
                    json_object_set_new(response_json, "messages_id", array_messages_id);
                } else if (type8) {
                    int chat_id = json_integer_value(json_object_get(json, "chat_id"));
                    int user_id = json_integer_value(json_object_get(json, "user_id"));
                    
                    char *message64 = (char *) json_string_value(json_object_get(json, "text"));
                    int len_message64 = json_integer_value(json_object_get(json, "len"));
                    char *message = (char *) decrypt_data_stream(message64, len_message64);
                    
                    int message_id = mx_add_message(user_id, chat_id, message);
                    json_object_set_new(response_json, "message_id", json_integer(message_id));
                }
                // Serialize the JSON object to a string
                response_str = json_dumps(response_json, JSON_COMPACT);
                // printf("res = %s\n",response_str);
                SSL_write(ssl, response_str, strlen(response_str));
                free(response_str);
            } else if (type6) {
                char *chat_id = (char *) json_string_value(json_object_get(json, "chat_id"));
                int user_id = json_integer_value(json_object_get(json, "user_id"));
                mx_delete(mx_atoi(chat_id), user_id);
            } else if (type9) {
                int message_id = json_integer_value(json_object_get(json, "message_id"));
                mx_delete_message(message_id);
            } else if (type10) {
                int message_id = json_integer_value(json_object_get(json, "message_id"));
                char *new_message64 = (char *) json_string_value(json_object_get(json, "new_message"));
                int len_new_message64 = json_integer_value(json_object_get(json, "len"));
                char *new_text = (char *) decrypt_data_stream(new_message64, len_new_message64);
                mx_edit_message(message_id, new_text);
            }

            json_decref(json);
            json_decref(response_json);
    }

    // Clean up
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client);

    //i--;
    pthread_exit(NULL);
}

int main(void)
{
    mx_create_db();

    SSL_CTX *ssl_ctx;
    int server;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    SSL_library_init();

    ssl_ctx = SSL_CTX_new(TLSv1_2_server_method());
    // Load the server certificate and private key
    SSL_CTX_use_certificate_file(ssl_ctx, "resources/cert.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ssl_ctx, "resources/key.pem", SSL_FILETYPE_PEM);

    // Create a server socket
    server = socket(AF_INET, SOCK_STREAM, 0);

    // Bind the server socket to the specified port
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8081);
    bind(server, (struct sockaddr*)&addr, sizeof(addr));

    // Listen for incoming connections
    if (listen(server, MAX_CONNECTIONS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Main server loop
    while (1) {
        //if (i >= NUM_THREADS) continue;
        int client = accept(server, (struct sockaddr*)&addr, &len);
        SSL *ssl = SSL_new(ssl_ctx);
        SSL_set_fd(ssl, client);
        SSL_accept(ssl);

        pthread_t thread;
        //i++;
        int rc = pthread_create(&thread, NULL, handle_client, (void*)ssl);
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Clean up
    SSL_CTX_free(ssl_ctx);
    close(server);

    return 0;
}
