#include "../../inc/main.h"

int load_status = 0;
int active_id = 0;
GtkWidget *entry_box;
GtkWidget *main_label;
GtkWidget *chat_box;

static void truncate_label_text(GtkLabel *label, const gchar *text, gint max_length) {
    gchar *truncated_text = NULL;

    if (strlen(text) > max_length) {
        truncated_text = g_strndup(text, max_length);
        gchar *ellipses = "...";
        gchar *temp = g_strconcat(truncated_text, ellipses, NULL);
        gtk_label_set_text(label, temp);
        g_free(temp);
    } else {
        gtk_label_set_text(label, text);
    }
}

static void on_delete_button_clicked(GtkButton *button, gpointer user_data) {

    GtkListBoxRow *row = (GtkListBoxRow *) user_data;

    const gchar *id = (const gchar *)g_object_get_data(G_OBJECT(row), "id");
    const gchar *user_id = (const gchar *)g_object_get_data(G_OBJECT(row), "user-id");

    if (mx_atoi(id) == active_id) {
        load_status++;
        gtk_widget_set_visible(GTK_WIDGET(entry_box), FALSE);
        gtk_label_set_text(GTK_LABEL(main_label), "");
        clear_list_box((GtkListBox *) chat_box);
    }

    const char* json_string = "{\"type\":\"delete chat\",\"chat_id\":\"%s\",\"user_id\":%i}";
    int final_json_length = mx_strlen(json_string) + mx_strlen(id) + mx_strlen(user_id) + 1;
    char *final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, id, mx_atoi(user_id));

    //write(sock, final_json_string, final_json_length);
    SSL_write(ssl, final_json_string, final_json_length);
    mx_strdel(&final_json_string);
}

GtkWidget* create_chat_row(AppData *app_data, const gchar *username, const gchar *message, const gchar *time, const gchar *id) {
    GtkWidget *row = gtk_list_box_row_new();
    gtk_widget_set_size_request(row, -1, 50);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    char *img_path = "resources/imgs/privat.png";
    GtkWidget *image = gtk_image_new_from_file(img_path);
    GtkWidget *vbox_labels = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *label_username = gtk_label_new(NULL);
    GtkWidget *label_message = gtk_label_new(NULL);
    GtkWidget *label_time = gtk_label_new(NULL);
    GtkWidget *delete_button = gtk_button_new();
    GtkWidget *delete_button_image = gtk_image_new_from_file("resources/imgs/delete.png");

    gtk_button_set_child(GTK_BUTTON(delete_button), delete_button_image);

    truncate_label_text(GTK_LABEL(label_username), username, 15);
    truncate_label_text(GTK_LABEL(label_message), message, 15);

    gtk_widget_set_halign(label_username, GTK_ALIGN_START);
    gtk_widget_set_halign(label_message, GTK_ALIGN_START);
    gtk_widget_set_halign(label_time, GTK_ALIGN_START);

    // Set bold style for username label
    PangoAttrList *attr_list = pango_attr_list_new();
    PangoAttribute *attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
    pango_attr_list_insert(attr_list, attr);
    gtk_label_set_attributes(GTK_LABEL(label_username), attr_list);
    pango_attr_list_unref(attr_list);

    gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(image, 40, 40);
    gtk_widget_set_size_request(delete_button, 40, 40);

    gtk_box_append(GTK_BOX(vbox_labels), label_username);
    gtk_box_append(GTK_BOX(vbox_labels), label_message);
    gtk_box_append(GTK_BOX(vbox_labels), label_time);

    gtk_widget_set_hexpand(vbox_labels, TRUE);

    gtk_widget_set_margin_start(hbox, 10);
    gtk_widget_set_margin_end(hbox, 10);

    gtk_box_append(GTK_BOX(hbox), image);
    gtk_box_append(GTK_BOX(hbox), vbox_labels);
    gtk_box_append(GTK_BOX(hbox), delete_button);
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), hbox);

    g_object_set_data(G_OBJECT(row), "id", (gpointer)id);
    g_object_set_data(G_OBJECT(row), "user-id", (gpointer)mx_itoa(app_data->userID));
    g_object_set_data(G_OBJECT(row), "label1", (gpointer)gtk_label_get_text(GTK_LABEL(label_username)));
    g_object_set_data(G_OBJECT(row), "label2", (gpointer)gtk_label_get_text(GTK_LABEL(label_message)));

    widget_styling(delete_button, "delete-btn", app_data->provider);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), row);

    return row;
}

static void on_destroy_clicked(GtkWidget *widget, gpointer user_data) {
    AppData *app_data = (AppData *) user_data;

    app_data->list_status = true;
    cleanup_app_data(app_data);

    if (app_data->app != NULL) {
        g_object_unref(app_data->app);
    }

    gtk_window_destroy(GTK_WINDOW(app_data->window));
    close(app_data->sock);
}

static void on_log_out_button_clicked(GtkWidget *widget, AppData *app_data) {
    app_data->left_list = true;

    // Destroy the main window
    app_data->list_status = true;
    cleanup_app_data(app_data);
    gtk_window_close(GTK_WINDOW(app_data->window));

    // Create and run a new GtkApplication instance for the authorization window
    app_data->app = gtk_application_new("com.example.AuthWindow", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app_data->app, "activate", G_CALLBACK(authorization), NULL);
    int status = g_application_run(G_APPLICATION(app_data->app), 0, NULL);
    
    app_data->list_status = true;
    close(app_data->sock);
    cleanup_app_data(app_data);
    gtk_window_close(GTK_WINDOW(app_data->window));

    if (app_data->app != NULL) {
        g_object_unref(app_data->app);
    }

    exit(status);
}


static void on_buffer_changed(GtkTextBuffer *buffer, GtkLabel *placeholder) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    gtk_widget_set_visible(GTK_WIDGET(placeholder), g_strcmp0(text, "") == 0);

    g_free(text);
}

static void delete_message(int id) {
    const char* json_string = "{\"type\":\"delete message\",\"message_id\":%i}";
    int final_json_length = strlen(json_string) + mx_strlen(mx_itoa(id)) + 1;
    char *final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, id);

    SSL_write(ssl, final_json_string, final_json_length);
    mx_strdel(&final_json_string);

    clear_list_box(chat_box);
}

static void edit_message(int id, char *new_text) {
    int len_text64;
    char *text64 = encrypt_data_stream(new_text, mx_strlen(new_text), &len_text64);

    const char* json_string = "{\"type\":\"edit message\",\"message_id\":%i,\"new_message\":\"%s\",\"len\":%i}";
    int final_json_length = strlen(json_string) + mx_strlen(mx_itoa(id)) + mx_strlen(text64) + mx_strlen(mx_itoa(len_text64)) + 1;
    char *final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, id, text64, len_text64);

    SSL_write(ssl, final_json_string, final_json_length);
    mx_strdel(&final_json_string);
}

static void on_menu_item_activate(GtkWidget *widget, gpointer data) {
    const gchar *action = gtk_widget_get_name(widget);

    if (g_strcmp0(action, "delete") == 0) {
        int message_id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "message_id"));
        g_print("Message ID: %d\n", message_id);
        delete_message(message_id);
    } else if (g_strcmp0(action, "edit") == 0) {
        // Implement your edit functionality here
    }
}

static void on_row_button_press(GtkGestureClick *gesture, int n_press, double x, double y, gpointer data) {
    GtkWidget *row = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    GtkWidget *popover = g_object_get_data(G_OBJECT(row), "popover");
    
    if (popover == NULL) {
        return;
    }

    GdkRectangle rect = {x, y, 1, 1};
    gtk_popover_set_pointing_to(GTK_POPOVER(popover), &rect);

    gtk_popover_popup(GTK_POPOVER(popover));
}

static void add_text_to_list_box(Message message, TextEntry *data) {

    if (mx_strcmp(message.text, "One of the users left this chat") == 0) {
        char *template = "User %s left this chat (";
        int len_temp = mx_strlen(template) + mx_strlen(message.name) + 1;
        char *result = (char *) malloc(len_temp);
        snprintf(result, len_temp, template, message.name);
       
        GtkWidget *alert = gtk_label_new(result);
        gtk_widget_set_halign(alert, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(alert, GTK_ALIGN_CENTER);
        widget_styling(alert, "alert", data->provider);

        int index = message.index;

        if (index == -1) {
            GtkListBoxRow *row = NULL;

            while ((row = gtk_list_box_get_row_at_index(chat_box, index)) != NULL) {
                index++;
            }
        }

        gtk_list_box_insert(GTK_LIST_BOX(data->list_box), alert, index);
        free(result);
        return;
    }

    GtkWidget *label = gtk_label_new(message.text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(label, 10);
    gtk_widget_set_margin_end(label, 10);
    gtk_widget_set_margin_top(label, 10);
    gtk_widget_set_margin_bottom(label, 10);

    GtkWidget *title = gtk_label_new(message.name);
    gtk_widget_set_valign(title, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(title, 10);
    gtk_widget_set_margin_end(title, 10);
    gtk_widget_set_margin_top(title, 10);

    // Set bold style for name label
    PangoAttrList *attr_list = pango_attr_list_new();
    PangoAttribute *attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
    pango_attr_list_insert(attr_list, attr);
    gtk_label_set_attributes(GTK_LABEL(title), attr_list);
    pango_attr_list_unref(attr_list);

    GtkWidget *time_label;

    if (message.time != NULL) {
        char *time = mx_strdup(message.time + mx_get_char_index(message.time, ' '));
        time_label = gtk_label_new(time);
        mx_strdel(&time);
    } else {
        time_label = gtk_label_new("Now");
    }

    gtk_widget_set_margin_start(time_label, 20);
    gtk_widget_set_margin_end(time_label, 20);

    GtkWidget *image = gtk_picture_new_for_filename("resources/imgs/new_user.png");
    gtk_widget_set_size_request(image, 40, 40);
    gtk_widget_set_margin_start(image, 10);
    gtk_widget_set_margin_end(image, 10);
    gtk_widget_set_margin_top(image, 10);
    gtk_widget_set_margin_bottom(image, 10);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append(GTK_BOX(vbox), title);
    gtk_box_append(GTK_BOX(vbox), label);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *frame = gtk_frame_new(NULL);

    if (message.owner) {
        gtk_widget_set_halign(title, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(hbox), image);
        gtk_box_append(GTK_BOX(hbox), vbox);
        gtk_widget_set_halign(frame, GTK_ALIGN_START);
        gtk_widget_set_valign(time_label, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(time_label, GTK_ALIGN_START);
    } else {
        gtk_widget_set_halign(title, GTK_ALIGN_END);
        gtk_box_append(GTK_BOX(hbox), vbox);
        gtk_box_append(GTK_BOX(hbox), image);
        gtk_widget_set_halign(frame, GTK_ALIGN_END);
        gtk_widget_set_valign(time_label, GTK_ALIGN_CENTER);
        gtk_widget_set_halign(time_label, GTK_ALIGN_END);
    }

    gtk_frame_set_child(GTK_FRAME(frame), hbox);
    widget_styling(frame, "message", data->provider);

    gtk_widget_set_margin_start(frame, 20);
    gtk_widget_set_margin_end(frame, 20);
    gtk_widget_set_margin_top(frame, 10);
    gtk_widget_set_margin_bottom(frame, 10);

    GtkWidget *outer_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append(GTK_BOX(outer_vbox), frame);
    gtk_box_append(GTK_BOX(outer_vbox), time_label);

    int index = message.index;

    if (index == -1) {
        GtkListBoxRow *row = NULL;

        while ((row = gtk_list_box_get_row_at_index(chat_box, index)) != NULL) {
            index++;
        }
    }

    if (message.owner == false) {
        GtkWidget *popover = gtk_popover_new();
        GtkWidget *popover_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

        GtkWidget *delete_button = gtk_button_new_with_label("Delete");
        gtk_widget_set_name(delete_button, "delete");
        g_object_set_data(G_OBJECT(delete_button), "message_id", GINT_TO_POINTER(message.id));
        g_signal_connect(delete_button, "clicked", G_CALLBACK(on_menu_item_activate), NULL);
        gtk_box_append(GTK_BOX(popover_box), delete_button);


        GtkWidget *edit_button = gtk_button_new_with_label("Edit");
        gtk_widget_set_name(edit_button, "edit");
        // g_object_set_data(G_OBJECT(edit_button), "message_id", GINT_TO_POINTER(message.id));
        // g_object_set_data(G_OBJECT(edit_button), "new_text", GINT_TO_POINTER(message.t));
        g_signal_connect(edit_button, "clicked", G_CALLBACK(on_menu_item_activate), NULL);
        gtk_box_append(GTK_BOX(popover_box), edit_button);

        gtk_popover_set_child(GTK_POPOVER(popover), popover_box);
        gtk_widget_set_parent(popover, outer_vbox);

        GtkGesture *gesture_click = gtk_gesture_click_new();
        g_signal_connect(gesture_click, "pressed", G_CALLBACK(on_row_button_press), NULL);
        gtk_widget_add_controller(outer_vbox, GTK_EVENT_CONTROLLER(gesture_click));
        g_object_set_data(G_OBJECT(outer_vbox), "popover", popover);
    }

    gtk_list_box_insert(GTK_LIST_BOX(data->list_box), outer_vbox, index);
}

static void on_icon_button_clicked(GtkButton *button, AppData *app_data) {
    GtkWidget *text_view_scroll = app_data->data->text_view_scroll;
    GtkTextView *text_view = GTK_TEXT_VIEW(app_data->data->text_view);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter start, end;

    // Get the start and end iterators for the text buffer
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Get the text from the buffer
    char *text = (char *) gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    text = mx_del_extra_spaces(text);

    if (load_status == 0 || mx_strlen(text) <= 0 || mx_strcmp(app_data->id_text, "-1") == 0) {
        return;
    }

    int user_id = app_data->userID;
    
    int len_text64;
    char *text64 = encrypt_data_stream(text, mx_strlen(text), &len_text64);

    const char* json_string = "{\"type\":\"add message\",\"chat_id\":%i,\"user_id\":%i,\"text\":\"%s\",\"len\":%i}";
    int final_json_length = strlen(json_string) + strlen(app_data->id_text) + 
                            mx_strlen(mx_itoa(user_id)) + mx_strlen(text64) + mx_strlen(mx_itoa(len_text64)) + 1;
    char *final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, mx_atoi(app_data->id_text), user_id, text64, len_text64);

    //write(sock, final_json_string, final_json_length);
    SSL_write(ssl, final_json_string, final_json_length);

    mx_strdel(&final_json_string);
    char new_message[2048];
    int message_length;

    message_length = SSL_read(ssl, new_message, sizeof(new_message));
    new_message[message_length] = '\0';

    json_error_t error;
    json_t *root = json_loads(new_message, 0, &error);
    json_t *json_message_id = json_object_get(root, "message_id");

    Message message;
    message.name = mx_strdup(app_data->username);
    message.time = NULL;
    message.text = mx_strdup(text);
    message.owner = false;
    message.id = json_integer_value(json_message_id);
    message.index = -1;
    
    add_text_to_list_box(message, app_data->data);
    
    mx_strdel(&message.text);
    mx_strdel(&message.name);
    mx_strdel(&message.time);
    g_free(text);

    // Clear the text buffer
    gtk_text_buffer_delete(buffer, &start, &end);

    // Return text viewer to the basic sizes
    gtk_widget_set_size_request(GTK_WIDGET(text_view_scroll), -1, START_TextViewerHeight);
}

static void on_insert_text(GtkTextBuffer *buffer, GtkTextIter *location, gchar *text, gint len, TextEntry *data) {
    gint current_char_count = gtk_text_buffer_get_char_count(buffer);

    if (current_char_count + len > MAX_CHARS) {
        g_signal_stop_emission_by_name(buffer, "insert-text");
    }

    GtkWidget *text_view_scroll = data->text_view_scroll;

    gint char_count = gtk_text_buffer_get_char_count(buffer);

    gint increment = 8;
    gint height = START_TextViewerHeight;

    height = height + char_count / 50 * increment;
    gtk_widget_set_size_request(GTK_WIDGET(text_view_scroll), -1, height);
    // TODO 
}

static int load(void *data) {
    AppData *app_data = (AppData *)data;

    if (load_status > 1) {
        load_status = 1;
        return 0;
    }

    int id = mx_atoi(app_data->id_text);

    const char* json_string = "{\"type\":\"load\",\"room id\":%i}";
    int final_json_length = strlen(json_string) + strlen(app_data->id_text) + 1;
    char *final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, id);

    //write(sock, final_json_string, final_json_length);
    SSL_write(ssl, final_json_string, final_json_length);

    mx_strdel(&final_json_string);
    char new_message[2048];
    //int message_length = read(sock, new_message, 2048);
    int message_length = SSL_read(ssl, new_message, sizeof(new_message));
    new_message[message_length] = '\0';

    json_error_t error;
    json_t *root = json_loads(new_message, 0, &error);
    json_t *messages = json_object_get(root, "messages");
    int count = json_integer_value(json_object_get(root, "count"));
    json_t *len_messages = json_object_get(root, "len messages");
    json_t *messages_id = json_object_get(root, "messages_id");

    // clear_list_box((GtkListBox *) app_data->data->list_box);
    int i = 0;
    for (; i < count; i++) {
        json_t *message_array = json_array_get(messages, i);
        json_t *messages_id_value = json_array_get(messages_id, i);
        json_t *message_array_len = json_array_get(len_messages, i);

        const char *name64 = json_string_value(json_array_get(message_array, 0));
        int len_name64 = json_integer_value(json_array_get(message_array_len, 0));
        const char *time64 = json_string_value(json_array_get(message_array, 1));
        int len_time64 = json_integer_value(json_array_get(message_array_len, 1));
        const char *text64 = json_string_value(json_array_get(message_array, 2));
        int len_text64 = json_integer_value(json_array_get(message_array_len, 2));

        int message_id = json_integer_value(messages_id_value);

        char *name = decrypt_data_stream(name64, len_name64);
        char *time = decrypt_data_stream(time64, len_time64);
        char *text = decrypt_data_stream(text64, len_text64);

        Message message;
        message.name = mx_strdup(name);
        message.time = mx_strdup(time);
        message.text = mx_strdup(text);
        message.id = message_id;
        message.index = i;

        message.owner = mx_strcmp(app_data->username, name);
        
        GtkListBoxRow *row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(chat_box), i);
        if (row != NULL) {
            GtkWidget *outer_vbox = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row));
            GtkWidget *frame = gtk_widget_get_first_child(outer_vbox);
            GtkWidget *hbox = gtk_frame_get_child(GTK_FRAME(frame));
            GtkWidget *vbox = gtk_widget_get_next_sibling(gtk_widget_get_first_child(hbox));
            GtkWidget *label = gtk_widget_get_next_sibling(gtk_widget_get_first_child(vbox));
            GtkWidget *time = gtk_widget_get_next_sibling(gtk_widget_get_first_child(vbox));
            const char *text2 = gtk_label_get_text(GTK_LABEL(label));
            const char *text3 = gtk_label_get_text(GTK_LABEL(time));
        
            if ((text2 && mx_strcmp(text, text2) != 0) || (text3 && mx_strcmp("Now", text3) == 0)) {
                gtk_list_box_remove(GTK_LIST_BOX(chat_box), GTK_WIDGET(row));
                add_text_to_list_box(message, app_data->data);
            }
        } else {
            add_text_to_list_box(message, app_data->data);
        }
        
        mx_strdel(&message.text);
        mx_strdel(&message.name);
        mx_strdel(&message.time);
    }

    GtkListBoxRow *row;

    while((row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(chat_box), i++)) != NULL) {
         gtk_list_box_remove(GTK_LIST_BOX(chat_box), GTK_WIDGET(row));
    }

    json_decref(root);
    return 1;
}

static void upload_chat(GtkListBox *list_box, GtkListBoxRow *row, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;

    // int row_index = gtk_list_box_row_get_index(row);
    gtk_widget_set_visible(GTK_WIDGET(entry_box), TRUE);

    const gchar *id_text = (gchar *)g_object_get_data(G_OBJECT(row), "id");
    const gchar *label1_text = (gchar *)g_object_get_data(G_OBJECT(row), "label1");
    // const gchar *label2_text = (gchar *)g_object_get_data(G_OBJECT(row), "label2");

    gtk_label_set_text(GTK_LABEL(app_data->chat_label), label1_text);
    clear_list_box((GtkListBox *) chat_box);

    active_id = mx_atoi(id_text);
    app_data->id_text = (char *) id_text;
    load_status++;
    g_timeout_add(1000, load, app_data);
}

// 0 - do not call function again, 1 - call again 
static int add_chats_list(void *data) {
    AppData *app_data = (AppData *)data;

    if (app_data->list_status || app_data->left_list) {
        return 0;
    }

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

    json_error_t error;
    json_t *root = json_loads(new_message, 0, &error);
    json_t *json_count = json_object_get(root, "count");
    json_t *json_list = json_object_get(root, "list");
    json_t *json_lens = json_object_get(root, "lens");

    int count = json_integer_value(json_count);
    clear_list_box(GTK_LIST_BOX(app_data->left_list_box));

    for (int i = 0; i < count; i++) {

        json_t *title_json = json_array_get(json_list, i);
        json_t *len_json = json_array_get(json_lens, i);

        int len_enc = json_integer_value(len_json);
        char *text_bef = (char *) json_string_value(title_json);
        char *text = decrypt_data_stream(text_bef, len_enc);

        // mx_strdel(&text_bef);
        int temp = mx_get_char_index(text, ',');
        char *title = mx_strndup(text, temp);
        text += temp + 1;
        temp = mx_get_char_index(text, ',');
        char *id = mx_strndup(text, temp);
        text += temp + 1;
        char *last_message = mx_strdup(text);
        
        if (last_message == NULL || mx_strcmp(last_message, "") == 0) {
            last_message = mx_strdup("No messages yet");
        }

        // GtkListBoxRow *row_temp = gtk_list_box_get_row_at_index(GTK_LIST_BOX(chat_box), i);
        // if (row_temp != NULL) {
        //     GtkWidget *hbox = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row_temp));
        //     GtkWidget *vbox_labels = gtk_widget_get_next_sibling(gtk_widget_get_first_child(hbox));
        //     GtkWidget *label_username = gtk_widget_get_first_child(vbox_labels);
        //     GtkWidget *label_message = gtk_widget_get_next_sibling(label_username);
        //     GtkWidget *label_time = gtk_widget_get_next_sibling(label_message);
        //     const char *old_last_message = gtk_label_get_text(GTK_LABEL(label_message));

        //     mx_printstr(">>>");
        //     mx_printstr(last_message);
        //     mx_printstr("<<<\n");
        //     mx_printstr(old_last_message);
        //     mx_printstr("<<<\n");

        //     if (old_last_message && mx_strcmp(last_message, old_last_message) != 0) {
        //         gtk_list_box_remove(GTK_LIST_BOX(app_data->left_list_box), GTK_WIDGET(row_temp));
        //         GtkWidget *row = create_chat_row(app_data, title, last_message, "00:00:00", id);
        //         gtk_list_box_insert(GTK_LIST_BOX(app_data->left_list_box), row, i);
        //     }
        // } else {
        //     GtkWidget *row = create_chat_row(app_data, title, last_message, "00:00:00", id);
        //     gtk_list_box_insert(GTK_LIST_BOX(app_data->left_list_box), row, i);
        // }

        GtkWidget *row = create_chat_row(app_data, title, last_message, "00:00:00", id);
        gtk_list_box_insert(GTK_LIST_BOX(app_data->left_list_box), row, i);

        mx_strdel(&title);
        mx_strdel(&last_message);
    }

    json_decref(root);
    return 1;
}

void add_chat_widgets(AppData *app_data) {
    GtkWindow *window = GTK_WINDOW(app_data->window);
    gtk_window_set_title(GTK_WINDOW(window), "Chat window");
    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy_clicked), app_data);

    // Create a horizontal grid box
    GtkWidget *grid_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), grid_box);

    // Left part
    GtkWidget *left_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(left_vbox, FALSE);
    gtk_widget_set_vexpand(left_vbox, TRUE);
    gtk_widget_set_size_request(left_vbox, 250, 60);
    gtk_box_append(GTK_BOX(grid_box), left_vbox);

    // New chats
    GtkWidget *new_room_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *new_chat_1_button = gtk_button_new_with_label("Private chat");
    GtkWidget *new_chat_2_button = gtk_button_new_with_label("Group chat");
    gtk_box_append(GTK_BOX(new_room_box), new_chat_1_button);
    gtk_box_append(GTK_BOX(new_room_box), new_chat_2_button);
    gtk_box_append(GTK_BOX(left_vbox), new_room_box);

    gtk_widget_set_margin_start(new_chat_1_button, 5);
    gtk_widget_set_margin_end(new_chat_1_button, 5);
    gtk_widget_set_margin_top(new_chat_1_button, 5);
    gtk_widget_set_margin_bottom(new_chat_1_button, 10);

    gtk_widget_set_margin_start(new_chat_2_button, 5);
    gtk_widget_set_margin_end(new_chat_2_button, 5);
    gtk_widget_set_margin_top(new_chat_2_button, 5);
    gtk_widget_set_margin_bottom(new_chat_2_button, 10);

    // List box
    GtkWidget *left_list_box = gtk_list_box_new();
    GtkWidget *left_list_box_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(left_list_box_scroll), left_list_box);
    gtk_widget_set_hexpand(left_list_box_scroll, TRUE);
    gtk_widget_set_vexpand(left_list_box_scroll, TRUE);
    gtk_box_append(GTK_BOX(left_vbox), left_list_box_scroll);

    // Disable horizontal scrolling
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(left_list_box_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    // Horizontal box
    GtkWidget *bottom_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(bottom_hbox, TRUE);
    gtk_widget_set_vexpand(bottom_hbox, FALSE);
    gtk_widget_set_size_request(bottom_hbox, -1, 50);
    gtk_box_append(GTK_BOX(left_vbox), bottom_hbox);

    // Create the log out button
    GtkWidget *log_out_button = gtk_button_new();
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *image = gtk_image_new_from_icon_name("system-log-out-symbolic");
    GtkWidget *label = gtk_label_new("Log out");
    gtk_widget_set_hexpand(log_out_button, TRUE);
    gtk_widget_set_vexpand(log_out_button, FALSE);
    gtk_widget_set_size_request(log_out_button, -1, 50);
    gtk_widget_set_margin_start(log_out_button, 5);
    gtk_widget_set_margin_end(log_out_button, 5);
    gtk_widget_set_margin_top(log_out_button, 5);
    gtk_widget_set_margin_bottom(log_out_button, 10);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    gtk_box_append(GTK_BOX(button_box), image);
    gtk_box_append(GTK_BOX(button_box), label);
    gtk_button_set_child(GTK_BUTTON(log_out_button), button_box);

    GtkWidget *change_theme_button = gtk_button_new_with_label("Switch");
    gtk_widget_set_margin_start(change_theme_button, 5);
    gtk_widget_set_margin_end(change_theme_button, 5);
    gtk_widget_set_margin_top(change_theme_button, 5);
    gtk_widget_set_margin_bottom(change_theme_button, 10);

    gtk_box_append(GTK_BOX(bottom_hbox), log_out_button);
    gtk_box_append(GTK_BOX(bottom_hbox), change_theme_button);

    // Create right part - vertical box (3 elements)
    GtkWidget *right_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(right_vbox, TRUE);
    gtk_widget_set_vexpand(right_vbox, TRUE);
    gtk_box_append(GTK_BOX(grid_box), right_vbox);

    // 1-st element: horizontal grid with 2 elements
    GtkWidget *first_hgrid = gtk_grid_new();
    gtk_widget_set_hexpand(first_hgrid, TRUE);
    gtk_widget_set_vexpand(first_hgrid, FALSE);
    gtk_widget_set_size_request(first_hgrid, -1, 60);
    gtk_box_append(GTK_BOX(right_vbox), first_hgrid);

    GtkWidget *chat_info_label = gtk_label_new(NULL);
    gtk_widget_set_halign(chat_info_label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(chat_info_label, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(chat_info_label, TRUE);
    gtk_widget_set_vexpand(chat_info_label, TRUE);
    gtk_grid_attach(GTK_GRID(first_hgrid), chat_info_label, 0, 0, 1, 1);

    app_data->chat_label = chat_info_label;

    // 2-nd element: scroll widget and opportunity to add rows (list box)
    GtkWidget *scroll_window = gtk_scrolled_window_new();
    GtkWidget *list_box = gtk_list_box_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_window), list_box);
    gtk_widget_set_hexpand(scroll_window, TRUE);
    gtk_widget_set_vexpand(scroll_window, TRUE);
    gtk_box_append(GTK_BOX(right_vbox), scroll_window);

    // Set minimum width
    gtk_widget_set_size_request(scroll_window, 400, -1);

    // Disable horizontal scrolling
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    // 3-rd element: horizontal box with 2 elements
    GtkWidget *third_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_append(GTK_BOX(right_vbox), third_hbox);

    // 1) Text viewer with opportunity to scroll it
    GtkWidget *text_view_scroll = gtk_scrolled_window_new();
    g_object_set(text_view_scroll, "kinetic-scrolling", FALSE, NULL);

    GtkWidget *overlay = gtk_overlay_new();
    GtkWidget *text_view = gtk_text_view_new();
    gtk_overlay_set_child(GTK_OVERLAY(overlay), text_view);

    GtkWidget *placeholder = gtk_label_new("Type message here...");
    gtk_widget_set_halign(placeholder, GTK_ALIGN_START);
    gtk_widget_set_valign(placeholder, GTK_ALIGN_START);
    gtk_widget_set_margin_start(placeholder, 10);
    gtk_widget_set_margin_top(placeholder, 10);
    gtk_widget_set_sensitive(placeholder, FALSE); // Set gray tone for placeholder text
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), placeholder);

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);

    int desired_height = 40; // Set this to the height you desire
    gtk_widget_set_size_request(overlay, -1, desired_height);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(text_view_scroll), overlay);
    gtk_widget_set_hexpand(text_view_scroll, TRUE);
    gtk_widget_set_vexpand(text_view_scroll, FALSE);
    gtk_widget_set_size_request(text_view_scroll, -1, desired_height);
    gtk_widget_set_size_request(text_view, -1, desired_height);

    gtk_box_append(GTK_BOX(third_hbox), text_view_scroll);

    TextEntry *data = g_malloc0(sizeof(TextEntry));
    data->text_view = GTK_TEXT_VIEW(text_view);
    data->text_view_scroll = text_view_scroll;
    data->provider = app_data->provider;
    data->list_box = list_box;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    g_signal_connect(buffer, "insert-text", G_CALLBACK(on_insert_text), data);
    g_signal_connect(buffer, "changed", G_CALLBACK(on_buffer_changed), placeholder);

    // Disable horizontal scrolling
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(text_view_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(text_view), 10); 
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(text_view), 10);

    // 2) Icon-button
    GtkWidget *icon_image = gtk_picture_new_for_filename("resources/imgs/send.png");
    GtkWidget *icon_button = gtk_button_new();
    gtk_button_set_child(GTK_BUTTON(icon_button), icon_image);

    // Create a container box for the icon button
    GtkWidget *icon_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(icon_container, FALSE);
    gtk_widget_set_vexpand(icon_container, FALSE);
    gtk_widget_set_size_request(icon_image, 40, 40);
    gtk_widget_set_size_request(icon_button, 40, 40);
    gtk_widget_set_size_request(icon_container, 40, 40);
    gtk_box_append(GTK_BOX(icon_container), icon_button);
    gtk_box_append(GTK_BOX(third_hbox), icon_container);
    gtk_widget_set_visible(GTK_WIDGET(third_hbox), FALSE);

    app_data->icon_button = icon_button;
    app_data->data = data;
    app_data->id_text = "-1";
    g_signal_connect(icon_button, "clicked", G_CALLBACK(on_icon_button_clicked), app_data);

    gtk_list_box_set_selection_mode(GTK_LIST_BOX(left_list_box), GTK_SELECTION_NONE);
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box), GTK_SELECTION_NONE);

    ThemeChat *theme = (ThemeChat *) malloc(sizeof(ThemeChat));
    theme->list_box = list_box;
    theme->first_hgrid = first_hgrid;
    theme->left_vbox = left_vbox;
    theme->text_view = text_view;
    theme->bottom_hbox = bottom_hbox;
    theme->left_list_box = left_list_box;
    theme->icon_button = icon_button;
    theme->new_chat_1_button = new_chat_1_button;
    theme->new_chat_2_button = new_chat_2_button;
    theme->log_out_button = log_out_button;
    theme->chat_info_label = chat_info_label;
    theme->change_theme_button = change_theme_button;
    theme->placeholder = placeholder;
    theme->icon_container = icon_container;

    entry_box = third_hbox;
    main_label = chat_info_label;
    chat_box = list_box;

    app_data->left_vbox = left_vbox;
    app_data->left_list_box = left_list_box;
    app_data->left_list = false;
    app_data->theme_widgets1 = theme;
    app_data->theme_window = "chat";
    app_data->list_status = false;

    g_timeout_add(1000, add_chats_list, app_data);
    g_signal_connect(new_chat_1_button, "clicked", G_CALLBACK(create_new_private_chat), app_data);
    g_signal_connect(new_chat_2_button, "clicked", G_CALLBACK(create_new_group_chat), app_data);
    g_signal_connect(left_list_box, "row-activated", G_CALLBACK(upload_chat), app_data); // upload chat
    g_signal_connect(log_out_button, "clicked", G_CALLBACK(on_log_out_button_clicked), app_data);
    g_signal_connect(change_theme_button, "clicked", G_CALLBACK(on_change_theme_button_clicked), app_data);

    set_chat_theme(app_data);
}

void create_chat_window(AppData *app_data) {
    free(app_data->theme_widgets2);
    clear_container(GTK_WINDOW(app_data->window));
    window_resize(app_data);
    add_chat_widgets(app_data);
}
