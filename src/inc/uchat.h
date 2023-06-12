#ifndef uchat_h
#define uchat_h

#include "../../resources/libmx/inc/libmx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <event2/buffer.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <jansson.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <pthread.h>

#define MAX_CONNECTIONS 100

typedef struct {
    char *name;
    char *time;
    char *text;
} Messages;

// accounts
int mx_add_new_chat(char *name_chat);
int mx_add_member_id(int user, int chat);
int mx_add_message(int user, int chat, char *text);
int mx_add_new_user(char *username, char *hash);
int mx_check_credentials(char *username, char *hash);
int mx_check_members_id(int user, int chat);
int mx_check_messages(int chat_id);
int mx_check_nick(char *username);
void mx_delete_all(int chat_id);
void mx_delete_chat(int id_chat);
void mx_delete_members(int id_chat);
void mx_delete_messages(int id_chat);
void mx_delete_one_user_chat(int id_chat, int id_user);
void mx_delete(int chat_id, int id_user);
char **mx_get_chats(int user_id);
int mx_get_count_chats(int user_id);
int mx_get_count_messages(int chat_id);
int mx_get_count_negative_members(int chat_id);
int mx_get_count_pos_members(int chat_id);
int mx_get_count_users(void);
int mx_get_id(char *username);
char *mx_get_last_message(int chat_id);
int *mx_get_negative(int chat_id);
Messages *mx_get_nick_mess(int chat_id, int count_mess);
char **mx_get_usernames(void);
int mx_get_substring(char *str);
int mx_get_id_message(int chat_id, char *username, char *text, char *time);
char **mx_get_chat_id_mess(int user_id);
int mx_get_count_members(int chat_id);
void mx_delete_message(int id_message);
void mx_edit_message(int id_message, char *new_mess);
char *encrypt_data_stream(const char *plaintext, int plaintext_len, int *length);
void encode_base64(const unsigned char *input, int input_len, char *output, int *output_len);
char *decrypt_data_stream(const char *ciphertext, int ciphertext_len);
int decode_base64(const unsigned char *input, int input_len, unsigned char *output);

//database
void mx_create_db(void);
void mx_prinerr_db(char *part, const char *err, sqlite3 *db);
void mx_prinerr_tables(int code, char *part, const char *err, sqlite3 *db);
sqlite3 *mx_open_db(void);

#define KEY (const unsigned char*)"guthyc-roxme6-dAmmaj"
#define IV  (const unsigned char*)"dAmmaj-guthyc-roxme6"

#endif
