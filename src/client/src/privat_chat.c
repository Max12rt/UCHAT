#include "../../inc/main.h"

static bool check_title(AppData *app_data, const char *title) {
    const char* json_string = "{\"type\":\"chats list\",\"id\":%i}";
    int final_json_length = strlen(json_string) + strlen(mx_itoa(app_data->userID)) + 1;
    char *final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, app_data->userID);

    //write(sock, final_json_string, final_json_length);
    SSL_write(ssl, final_json_string, final_json_length);

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
        return false;
    }

    json_t *json_count = json_object_get(root, "count");
    json_t *json_list = json_object_get(root, "list");

    int count = json_integer_value(json_count);

    for (int i = 0; i < count; i++) {
        json_t *title_json = json_array_get(json_list, i);
        const char *j_title = json_string_value(title_json);
        int index = mx_get_char_index(j_title, ',');
        char *new_title = mx_strndup(j_title, index);

        if (mx_strcmp(title, new_title) == 0) {
            json_decref(root);
            mx_strdel(&new_title);
            return true;
        }

        mx_strdel(&new_title);
    }

    json_decref(root);

    return false;
}

static void on_popup_destroy(GtkWidget *popup, gpointer user_data) {
    AppData *app_data = (AppData *) user_data;
    gtk_window_close(GTK_WINDOW(app_data->theme_widgets3->popup));
}

static void create_new_chat_clicked(GtkButton *button, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    ThemePrivat *widgets = (ThemePrivat *) app_data->theme_widgets3;
    const gchar *title = gtk_editable_get_text(GTK_EDITABLE(widgets->chat_title_entry));
    const gchar *username = gtk_editable_get_text(GTK_EDITABLE(widgets->username_entry));
    gboolean errors = false;

    if (mx_strlen(username) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->username_label), "Error: Please provide a nickname");
        gtk_widget_set_visible(GTK_WIDGET(widgets->username_entry), TRUE);
        gtk_widget_grab_focus(GTK_WIDGET(widgets->username_entry));
        errors = true;
    } else if (mx_check_unique_username(app_data, username)) {
        gchar *message_text = g_strdup_printf("Error: User \"%s\" does not exist", username);
        gtk_label_set_text(GTK_LABEL(widgets->username_label), message_text);
        
        if (!errors) {
            gtk_widget_set_visible(GTK_WIDGET(widgets->username_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(widgets->username_entry));
            errors = true;
        }
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->username_label), "");
    }

    if (mx_strlen(title) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->chat_title_label), "Error: Please provide a title");
        
        if (!errors) {
            gtk_widget_set_visible(GTK_WIDGET(widgets->chat_title_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(widgets->chat_title_entry));
            errors = true;
        }
    } else if (check_title(app_data, title)) {
        gtk_label_set_text(GTK_LABEL(widgets->chat_title_label), "Error: Chat this such name already exists");
        
        if (!errors) {
            gtk_widget_set_visible(GTK_WIDGET(widgets->chat_title_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(widgets->chat_title_entry));
            errors = true;
        }
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->chat_title_label), "");
    }
    
    if (errors) {
        return;
    }

    bool temp = (mx_strcmp(username, app_data->username) == 0);

    char *usernames[] = {temp ? (char *) username : app_data->username, (char *) username};
    char *id = add_new_chat_to_DB(app_data, usernames, temp ? 1 : 2, title);
    
    if (mx_atoi(id) == -1) {
        error_window(app_data, "Server error", "Something went wrong, please, try again later", NULL);
    }

    GtkWidget *row = create_chat_row(app_data, title, "No messages yet", id, "00:00:00");
    gtk_list_box_insert(GTK_LIST_BOX(app_data->left_list_box), row, -1);

    on_popup_destroy(widgets->popup, app_data);
}

void create_new_private_chat(GtkButton *button, AppData *app_data) {
    GtkWidget *popup = gtk_window_new();
    gtk_window_set_resizable(GTK_WINDOW(popup), FALSE);
    GtkWidget *center_box = gtk_center_box_new();
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *username_entry = gtk_entry_new();
    GtkWidget *username_label = gtk_label_new(NULL);
    GtkWidget *chat_title_entry = gtk_entry_new();
    GtkWidget *chat_title_label = gtk_label_new(NULL);
    GtkWidget *create_chat_button = gtk_button_new_with_label("Create New Chat");

    gtk_window_set_title(GTK_WINDOW(popup), "Create New Private Chat");
    gtk_window_set_default_size(GTK_WINDOW(popup), 300, 240);
    gtk_window_set_modal(GTK_WINDOW(popup), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(popup), GTK_WINDOW(app_data->window));

    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Provide a username");
    gtk_entry_set_placeholder_text(GTK_ENTRY(chat_title_entry), "Provide a chat title");

    gtk_widget_set_margin_top(vbox, 25);
    gtk_widget_set_margin_bottom(vbox, 25);
    gtk_widget_set_margin_start(vbox, 25);
    gtk_widget_set_margin_end(vbox, 25);

    gtk_widget_set_size_request(username_entry, 200, -1);
    gtk_widget_set_size_request(username_label, 200, -1);
    gtk_widget_set_size_request(chat_title_entry, 200, -1);
    gtk_widget_set_size_request(chat_title_label, 200, -1);
    gtk_widget_set_size_request(create_chat_button, 200, -1);

    gtk_box_append(GTK_BOX(vbox), username_entry);
    gtk_box_append(GTK_BOX(vbox), username_label);
    gtk_box_append(GTK_BOX(vbox), chat_title_entry);
    gtk_box_append(GTK_BOX(vbox), chat_title_label);
    gtk_box_append(GTK_BOX(vbox), create_chat_button);

    gtk_center_box_set_center_widget(GTK_CENTER_BOX(center_box), vbox);
    gtk_window_set_child(GTK_WINDOW(popup), center_box);

    if (app_data->theme_widgets3 != NULL) {
        free(app_data->theme_widgets3);
    }

    ThemePrivat *theme = (ThemePrivat *) malloc(sizeof(ThemePrivat));
    theme->popup = popup;
    theme->username_entry = username_entry;
    theme->username_label = username_label;
    theme->chat_title_entry = chat_title_entry;
    theme->chat_title_label = chat_title_label;
    theme->create_chat_button = create_chat_button;

    app_data->theme_widgets3 = theme;

    set_privat_theme(app_data);
    g_signal_connect(create_chat_button, "clicked", G_CALLBACK(create_new_chat_clicked), app_data);
    g_signal_connect(popup, "destroy", G_CALLBACK(on_popup_destroy), app_data);
    gtk_window_present(GTK_WINDOW(popup));
}
