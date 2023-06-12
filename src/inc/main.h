#include "../../resources/libmx/inc/libmx.h"
#include <gtk/gtk.h>
#include <glib.h>
// #include <event.h>
// #include <event2/event.h>
// #include <event2/bufferevent.h>
// #include <event2/listener.h>
// #include <event2/buffer.h>
// #include <event2/thread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gdk/gdk.h>
//#include <pthread.h>
#include <time.h>
#include <jansson.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#define CHAT_WINDOW_WIDTH 700
#define CHAT_WINDOW_HEIGHT 600
#define ANIMATION_STEPS 200
#define ANIMATION_INTERVAL 3
#define START_WIDTH 300
#define START_HEIGHT 450
#define START_TextViewerHeight 50
#define MAX_CHARS 512
#define MAX_MESSAGE 100 // error window height

typedef struct {
    GtkTextView *text_view;
    GtkWidget *text_view_scroll;
    GtkCssProvider *provider;
    GtkWidget *list_box;
} TextEntry;

typedef struct {
    int id;
    bool owner;
    char *text;
    char *name;
    char *time;
    int index;
} Message;

typedef struct {
    GtkWidget *main_box;
    GtkWidget *switch_to_login_button;
    GtkWidget *switch_to_signup_button;
    GtkWidget *login_button;
    GtkWidget *signup_button;
    GtkWidget *change_theme_button;
    GtkWidget *signup_confirm_password_entry;
    GtkWidget *login_username_error;
    GtkWidget *login_password_error;
    GtkWidget *signup_username_error;
    GtkWidget *signup_password_error;
    GtkWidget *signup_confirm_password_error;
    GtkWidget *login_form;
    GtkWidget *signup_form;
} ThemeAuthorization;

typedef struct {
    GtkWidget *list_box;
    GtkWidget *first_hgrid;
    GtkWidget *left_vbox;
    GtkWidget *text_view;
    GtkWidget *bottom_hbox;
    GtkWidget *left_list_box;
    GtkWidget *icon_button;
    GtkWidget *new_chat_1_button;
    GtkWidget *new_chat_2_button;
    GtkWidget *log_out_button;
    GtkWidget *chat_info_label;
    GtkWidget *change_theme_button;
    GtkWidget *placeholder;
    GtkWidget *icon_container;
} ThemeChat;

typedef struct {
    GtkWidget* popup;
    GtkWidget* username_entry;
    GtkWidget* username_label;
    GtkWidget* chat_title_entry;
    GtkWidget* chat_title_label;
    GtkWidget* create_chat_button;
} ThemePrivat;

typedef struct {
    GtkWidget *popup;
    GtkWidget *username_entry;
    GtkWidget *username_label;
    GtkWidget *chat_title_entry;
    GtkWidget *chat_title_label;
    GtkWidget *plus_button;
    GtkWidget *create_chat_button;
    GtkWidget *scroll;
    GtkWidget *list_box;
} ThemeGroup;

typedef struct {
    int sock;
    int userID;
    bool left_list;
    char *id_text;
    char *username;
    char *temp_username;
    int theme; // main
    int theme1; // private
    int theme2; // group
    char *theme_window;
    bool list_status;

    ThemeChat *theme_widgets1;
    ThemeAuthorization *theme_widgets2;
    ThemePrivat *theme_widgets3;
    ThemeGroup *theme_widgets4;

    GtkWidget *icon_button;
    GtkWidget *row;
    
    GtkApplication *app;
    GtkWidget *window;
    GtkCssProvider *provider;

    GtkWidget *stack;
    GtkWidget *login_username_entry;
    GtkWidget *login_password_entry;
    GtkWidget *login_username_error; // error1
    GtkWidget *login_password_error; // error2
    
    GtkWidget *signup_username_entry;
    GtkWidget *signup_password_entry;
    GtkWidget *signup_confirm_password_entry;
    GtkWidget *signup_username_error;         // error1
    GtkWidget *signup_password_error;         // error2
    GtkWidget *signup_confirm_password_error; // error3

    GtkWidget *main_box;
    GtkWidget *switch_to_login_button;
    GtkWidget *switch_to_signup_button;
    GtkWidget *login_button;
    GtkWidget *signup_button;
    GtkWidget *change_theme_button;
    GtkWidget *login_form;
    GtkWidget *signup_form;
    GtkWidget *first_hgrid;
    GtkWidget *text_view;
    GtkWidget *bottom_hbox;
    GtkWidget *new_chat_1_button;
    GtkWidget *new_chat_2_button;
    GtkWidget *log_out_button;
    GtkWidget *placeholder;

    TextEntry *data;

    // Chat
    char *top_element;
    char *last_message;
    int last_message_time;
    GtkWidget *left_vbox;
    GtkWidget *left_list_box;
    GtkWidget *list_box; // list all chats
    GtkWidget *search_entry;
    GtkWidget *chat_label;
} AppData;

typedef struct {
    int userID;
    GtkListBoxRow *row;
} AppDataTemp;

typedef struct {
    int width;
    int height;
} ScreenParams;

typedef struct {
    GtkWindow *window;
    float current_width;
    float current_height;
    float step_x;
    float step_y;
    int current_step;
    ScreenParams screen;
    gboolean is_valid;
} ResizeAnimationData;

void add_chat_widgets(AppData *app_data);

int  connect_to_server(void);
void create_chat_window(AppData *app_data);
void authorization(GtkApplication *app);
void widget_styling(GtkWidget *widget, const char *name_of_css_class, GtkCssProvider *provider);
void window_resize(AppData *app_data);
ScreenParams get_screen_size(GtkWindow *window);
void clear_container(GtkWindow *window);
void cleanup_app_data(AppData *app_data);
void clear_list_box(GtkListBox *list_box);
bool mx_check_unique_username(AppData *app_data, const char *username);
void error_window(AppData *app_data, char *title, char *message, char *username);
GtkWidget* create_chat_row(AppData *app_data, const gchar *username, const gchar *message, const gchar *time, const gchar *id);

void create_new_private_chat(GtkButton *button, AppData *app_data);
void create_new_group_chat(GtkButton *button, AppData *app_data);
char *add_new_chat_to_DB(AppData *app_data, char **usernames, int usernames_count, const char *title);
char *mx_generate_password_hash(const char *password);
char *encrypt_data_stream(const char *plaintext, int plaintext_len, int *length);
void encode_base64(const unsigned char *input, int input_len, char *output, int *output_len);
char *decrypt_data_stream(const char *ciphertext, int ciphertext_len);
int decode_base64(const unsigned char *input, int input_len, unsigned char *output);

void set_authorization_theme(AppData *app_data);
void set_chat_theme(AppData *app_data);
void set_privat_theme(AppData *app_data);
void set_group_theme(AppData *app_data);
void on_change_theme_button_clicked(GtkWidget *widget, AppData *app_data);

//for server
int sock;
SSL *ssl;
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8081
char* username_for_server[50];
char* password_for_server[50];
#define KEY (const unsigned char*)"guthyc-roxme6-dAmmaj"
#define IV  (const unsigned char*)"dAmmaj-guthyc-roxme6"
