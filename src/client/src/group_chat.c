#include "../../inc/main.h"

static void on_delete_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *row = gtk_widget_get_parent(GTK_WIDGET(button));
    GtkWidget *list_box_row = gtk_widget_get_parent(row);
    GtkWidget *list_box = gtk_widget_get_parent(list_box_row);
    gtk_list_box_remove(GTK_LIST_BOX(list_box), list_box_row);
}

static void on_plus_button_clicked(GtkButton *button, gpointer user_data) {
    AppData *app_data = (AppData *) user_data;
    ThemeGroup *widgets = app_data->theme_widgets4;
    const gchar *username = gtk_editable_get_text(GTK_EDITABLE(widgets->username_entry));

    if (username && *username) {
        gboolean username_exists = FALSE;

        if (g_strcmp0(app_data->username, username) == 0) {
            gtk_label_set_text(GTK_LABEL(widgets->username_label), "Do not worry, you will be adding automatically");
            return;
        }

        for (GtkWidget *row = gtk_widget_get_first_child(GTK_WIDGET(widgets->list_box));
            row != NULL; row = gtk_widget_get_next_sibling(row)) {
            GtkWidget *box = gtk_widget_get_first_child(row);
            GtkWidget *label = gtk_widget_get_first_child(box);
            const gchar *label_text = gtk_label_get_text(GTK_LABEL(label));

            if (g_strcmp0(label_text, username) == 0) {
                username_exists = TRUE;
                break;
            }
        }

        if (username_exists) {
            gtk_label_set_text(GTK_LABEL(widgets->username_label), "Username already added");
            return;
        } else if (mx_check_unique_username(app_data, username)) {
            gtk_label_set_text(GTK_LABEL(widgets->username_label), "No such user (");
            return;
        } else {
            GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
            GtkWidget *label = gtk_label_new(username);
            GtkWidget *delete_button = gtk_button_new_from_icon_name("window-close-symbolic");

            gtk_widget_set_hexpand(delete_button, TRUE);
            gtk_widget_set_halign(delete_button, GTK_ALIGN_END);

            gtk_box_append(GTK_BOX(row), label);
            gtk_box_append(GTK_BOX(row), delete_button);

            gtk_list_box_append(GTK_LIST_BOX(widgets->list_box), row);
            gtk_editable_get_text(GTK_EDITABLE(widgets->username_entry));
        
            g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), NULL);
            gtk_label_set_text(GTK_LABEL(widgets->username_label), "");    
        }
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->username_label), "Please provide a username");
    }
}

static void on_popup_destroy(GtkWidget *popup, gpointer user_data) {
    AppData *app_data = (AppData *) user_data;
    gtk_window_close(GTK_WINDOW(app_data->theme_widgets4->popup));
}

static void create_group_chat_clicked(GtkButton *button, gpointer user_data) {
    AppData *app_data = (AppData *) user_data;
    ThemeGroup *widgets = app_data->theme_widgets4;

    const char* title = gtk_editable_get_text(GTK_EDITABLE(widgets->chat_title_entry));
    int count = 0;
    bool error = false;
    
    for (GtkWidget *row = gtk_widget_get_first_child(GTK_WIDGET(widgets->list_box));
        row != NULL; row = gtk_widget_get_next_sibling(row)) {
        count++;
    }

    if (title == NULL || mx_strlen(title) == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->chat_title_label), "Please provide a title");
        error = true;
    }

    if (count == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->username_label), "No people selected for a group chat");
        error = true;
    }

    if (count == 1) {
        gtk_label_set_text(GTK_LABEL(widgets->username_label), "Not enough people for a group chat");
        error = true;
    }

    if (error) {
        return;
    }

    char **usernames = g_malloc0((count + 2) * sizeof(char *));

    int i = 0;
    for (GtkWidget *row = gtk_widget_get_first_child(GTK_WIDGET(widgets->list_box));
        row != NULL; row = gtk_widget_get_next_sibling(row)) {
        GtkWidget *box = gtk_widget_get_first_child(row);
        GtkWidget *label = gtk_widget_get_first_child(box);
        const gchar *label_text = gtk_label_get_text(GTK_LABEL(label));
        usernames[i++] = g_strdup(label_text);
    }

    gtk_label_set_text(GTK_LABEL(widgets->username_label), "");
    gtk_label_set_text(GTK_LABEL(widgets->chat_title_label), "");

    usernames[i++] = g_strdup(app_data->username);
    usernames[i] = NULL;

    add_new_chat_to_DB(app_data, usernames, count + 1, title);

    for (int j = 0; j < count; j++) {
        g_free(usernames[j]);
    }
    g_free(usernames);

    on_popup_destroy(widgets->popup, app_data);
}

void create_new_group_chat(GtkButton *button, AppData *app_data) {
    GtkWidget *popup = gtk_window_new();
    gtk_window_set_resizable(GTK_WINDOW(popup), FALSE);
    gtk_window_set_title(GTK_WINDOW(popup), "Creating new group chat");
    gtk_window_set_default_size(GTK_WINDOW(popup), 400, 450);
    gtk_window_set_modal(GTK_WINDOW(popup), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(popup), GTK_WINDOW(app_data->window));

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *chat_title_entry = gtk_entry_new();
    GtkWidget *chat_title_label = gtk_label_new(NULL);
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *username_entry = gtk_entry_new();
    GtkWidget *plus_button = gtk_button_new_from_icon_name("list-add-symbolic");
    GtkWidget *username_label = gtk_label_new(NULL);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *list_box = gtk_list_box_new();
    GtkWidget *create_chat_button = gtk_button_new_with_label("Create new chat");

    gtk_entry_set_placeholder_text(GTK_ENTRY(chat_title_entry), "Provide a chat title");
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Provide a username");

    gtk_widget_set_margin_start(vbox, 10);
    gtk_widget_set_margin_end(vbox, 10);
    gtk_widget_set_margin_top(vbox, 10);
    gtk_widget_set_margin_bottom(vbox, 10);

    gtk_widget_set_halign(chat_title_entry, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(chat_title_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(hbox, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(username_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(scroll, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(create_chat_button, GTK_ALIGN_CENTER);

    if (app_data->theme_widgets4 != NULL) {
        free(app_data->theme_widgets4);
    }

    ThemeGroup *theme = (ThemeGroup *) malloc(sizeof(ThemeGroup));
    theme->popup = popup;
    theme->username_entry = username_entry;
    theme->username_label = username_label;
    theme->chat_title_entry = chat_title_entry;
    theme->chat_title_label = chat_title_label;
    theme->plus_button = plus_button;
    theme->create_chat_button = create_chat_button;
    theme->scroll = scroll;
    theme->list_box = list_box;

    app_data->theme_widgets4 = theme;

    set_group_theme(app_data);
    g_signal_connect(plus_button, "clicked", G_CALLBACK(on_plus_button_clicked), app_data);
    g_signal_connect(create_chat_button, "clicked", G_CALLBACK(create_group_chat_clicked), app_data);

    gtk_box_append(GTK_BOX(hbox), username_entry);
    gtk_box_append(GTK_BOX(hbox), plus_button);

    gtk_box_append(GTK_BOX(vbox), chat_title_entry);
    gtk_box_append(GTK_BOX(vbox), chat_title_label);
    gtk_box_append(GTK_BOX(vbox), hbox);
    gtk_box_append(GTK_BOX(vbox), username_label);
    gtk_box_append(GTK_BOX(vbox), scroll);
    gtk_box_append(GTK_BOX(vbox), create_chat_button);
    gtk_window_set_child(GTK_WINDOW(popup), vbox);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_box);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll, 180, 200);

    gtk_widget_set_parent(vbox, GTK_WIDGET(popup));
    gtk_widget_set_visible(popup, TRUE);
    g_signal_connect(popup, "destroy", G_CALLBACK(on_popup_destroy), app_data);
}
