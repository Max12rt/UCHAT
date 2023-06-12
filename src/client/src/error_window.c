#include "../../inc/main.h"

static void on_ok_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *popup_window = GTK_WIDGET(user_data);
    gtk_window_destroy(GTK_WINDOW(popup_window));
}

void error_window(AppData *app_data, char *title, char *message, char *username) {
    GtkWidget *popup_window = gtk_window_new();
    GtkWidget *message_label = gtk_label_new(NULL);
    GtkWidget *ok_button = gtk_button_new_with_label("OK");
    GtkWidget *centered_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gchar *message_text;

    bool server_error = (mx_strcmp(title, "Server error") == 0);

    if (mx_strcmp(title, "User not found") == 0) {
        message_text = g_strdup_printf(message, username);
        gtk_label_set_text(GTK_LABEL(message_label), message_text);
        g_free(message_text);
    } else if (server_error) {
        message_text = g_strdup_printf("Server is disconnected\n Please, try again later");
        gtk_label_set_text(GTK_LABEL(message_label), message_text);
        g_free(message_text);
    } else {
        gtk_label_set_text(GTK_LABEL(message_label), message);
    }
    
    gtk_window_set_title(GTK_WINDOW(popup_window), title);
    gtk_window_set_resizable(GTK_WINDOW(popup_window), FALSE);
    gtk_window_set_modal(GTK_WINDOW(popup_window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(popup_window), GTK_WINDOW(app_data->window));
    gtk_window_set_default_size(GTK_WINDOW(popup_window), 250, MAX_MESSAGE);

    g_signal_connect(ok_button, "clicked", G_CALLBACK(on_ok_button_clicked), popup_window);

    gtk_widget_set_valign(centered_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(centered_box, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(ok_button, 80, -1);

    gtk_box_append(GTK_BOX(centered_box), message_label);
    gtk_box_append(GTK_BOX(centered_box), ok_button);
    gtk_window_set_child(GTK_WINDOW(popup_window), centered_box);
   
    widget_styling(popup_window, "error-window", app_data->provider);
    widget_styling(message_label, "error-message", app_data->provider);
    widget_styling(ok_button, "au-button", app_data->provider);
   
    gtk_window_present(GTK_WINDOW(popup_window));
   
    if (server_error) {
        connect_to_server();
    }
}
