#include "../../inc/main.h"

void on_change_theme_button_clicked(GtkWidget *widget, AppData *app_data) {

    if (app_data->theme >= 5) {
        app_data->theme = -1;
    }

    app_data->theme = app_data->theme + 1;
    char *window = app_data->theme_window;
    
    if (mx_strcmp(window, "authorization") == 0) {
        set_authorization_theme(app_data);
    } else if (mx_strcmp(window, "chat") == 0) {
        set_chat_theme(app_data);
    }
}

void remove_and_apply_style(GtkWidget *widget, const char *old_class, const char *new_class, GtkCssProvider *provider) {
    if (widget == NULL) {
        // g_critical("Segmentation Hello )))");
        // g_warning("Segmentation Hello )))");
        return;
    }

    if (old_class != NULL) {
        gtk_widget_remove_css_class(widget, old_class);
    }

    if (new_class != NULL) {
        widget_styling(widget, new_class, provider);
    }
}

void set_authorization_theme(AppData *app_data) {
    GtkCssProvider *provider = app_data->provider;
    int temp = app_data->theme;
    ThemeAuthorization *widgets = app_data->theme_widgets2;

    // const char *default_theme_class[] = { "authorization-window", "au-button", "au-entry", "au-errors" };
    const char *black_theme_class[] = { "authorization-window_black", "au-button_black", "au-entry_black", "au-errors_black" };
    const char *green_theme_class[] = { "authorization-window-green", "au-button-green", "au-entry-green", "au-errors-green" };
    const char *pink_theme_class[] = { "authorization-window-pink", "au-button-pink", "au-entry-pink", "au-errors-pink" };
    const char *purple_theme_class[] = { "authorization-window-purple", "au-button-purple", "au-entry-purple", "au-errors-purple" };
    const char *blue_theme_class[] = { "authorization-window-blue", "au-button-blue", "au-entry-blue", "au-errors-blue" };
    const char *light_blue_theme_class[] = { "authorization-window-light_blue", "au-button-light_blue", "au-entry-light_blue", "au-errors-light_blue" };

    const char **old_class = NULL;
    const char **new_class = NULL;

    if (temp == 0) {
        old_class = light_blue_theme_class;
        new_class = green_theme_class;
    } else if (temp == 1) {
        old_class = green_theme_class;
        new_class = black_theme_class;
    } else if (temp == 2) {
        old_class = black_theme_class;
        new_class = pink_theme_class;
    } else if (temp == 3) {
        old_class = pink_theme_class;
        new_class = purple_theme_class;
    } else if (temp == 4) {
        old_class = purple_theme_class;
        new_class = blue_theme_class;
    } else if (temp == 5) {
        old_class = blue_theme_class;
        new_class = light_blue_theme_class;
    }

    remove_and_apply_style(widgets->main_box, old_class[0], new_class[0], provider);
    remove_and_apply_style(widgets->switch_to_login_button, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->switch_to_signup_button, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->login_button, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->signup_button, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->change_theme_button, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->signup_confirm_password_entry, old_class[2], new_class[2], provider);
    remove_and_apply_style(widgets->login_username_error, old_class[3], new_class[3], provider);
    remove_and_apply_style(widgets->login_password_error, old_class[3], new_class[3], provider);
    remove_and_apply_style(widgets->signup_username_error, old_class[3], new_class[3], provider);
    remove_and_apply_style(widgets->signup_password_error, old_class[3], new_class[3], provider);
    remove_and_apply_style(widgets->signup_confirm_password_error, old_class[3], new_class[3], provider);

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j += 2) {
            remove_and_apply_style(gtk_grid_get_child_at(GTK_GRID(widgets->login_form), i, j), old_class[2], new_class[2], provider);
            remove_and_apply_style(gtk_grid_get_child_at(GTK_GRID(widgets->signup_form), i, j), old_class[2], new_class[2], provider);
        }
    }
}


void set_chat_theme(AppData *app_data) {
    ThemeChat *widgets = app_data->theme_widgets1;
    int temp = app_data->theme;
    GtkCssProvider *provider = app_data->provider;
    //widget_styling(widgets->chat_info_label, "chat-label", provider);

    // const char *default_theme_class[] = { "chat-section", "chat-info-section", "search-section", "text-entry-section", "buttons-section", "send-button", "au-button", "placeholder", "chats-list-section", "message", "chat-label" };
    const char *black_theme_class[] = { "chat-section_black", "chat-info-section_black", "search-section_black", "text-entry-section_black", "buttons-section_black", "send-button_black", "au-button_black", "placeholder_black", "chats-list-section_black", "message_black", "chat-label_black" };
    const char *green_theme_class[] = { "chat-green", "chat-info-green", "search-green", "text-entry-green", "buttons-green", "send-button-green", "au-button-green", "placeholder-green", "chats-list-green", "message-green", "chat-label-green" };
    const char *pink_theme_class[] = { "chat-pink", "chat-info-pink", "search-pink", "text-entry-pink", "buttons-pink", "send-button-pink", "au-button-pink", "placeholder-pink", "chats-list-pink", "message-pink", "chat-label-pink" };
    const char *purple_theme_class[] = { "chat-purple", "chat-info-purple", "search-purple", "text-entry-purple", "buttons-purple", "send-button-purple", "au-button-purple", "placeholder-purple", "chats-list-purple", "message-purple", "chat-label-purple" };
    const char *blue_theme_class[] = { "chat-blue", "chat-info-blue", "search-blue", "text-entry-blue", "buttons-blue", "send-button-blue", "au-button-blue", "placeholder-blue", "chats-list-blue", "message-blue", "chat-label-blue" };
    const char *light_blue_theme_class[] = { "chat-light_blue", "chat-info-light_blue", "search-light_blue", "text-entry-light_blue", "buttons-light_blue", "send-button-light_blue", "au-button-light_blue", "placeholder-light_blue", "chats-list-light_blue", "message-light_blue", "chat-label-light_blue" };

    const char **old_class = NULL;
    const char **new_class = NULL;

    if (temp == 0) {
        old_class = light_blue_theme_class;
        new_class = green_theme_class;
    } else if (temp == 1) {
        old_class = green_theme_class;
        new_class = black_theme_class;
    } else if (temp == 2) {
        old_class = black_theme_class;
        new_class = pink_theme_class;
    } else if (temp == 3) {
        old_class = pink_theme_class;
        new_class = purple_theme_class;
    } else if (temp == 4) {
        old_class = purple_theme_class;
        new_class = blue_theme_class;
    } else if (temp == 5) {
        old_class = blue_theme_class;
        new_class = light_blue_theme_class;
    }
    
    remove_and_apply_style(widgets->list_box, old_class[0], new_class[0], provider);
    remove_and_apply_style(widgets->first_hgrid, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->left_vbox, old_class[2], new_class[2], provider);
    remove_and_apply_style(widgets->text_view, old_class[3], new_class[3], provider);
    remove_and_apply_style(widgets->icon_container, old_class[3], new_class[3], provider);
    remove_and_apply_style(widgets->bottom_hbox, old_class[4], new_class[4], provider);
    remove_and_apply_style(widgets->icon_button, old_class[5], new_class[5], provider);
    remove_and_apply_style(widgets->new_chat_1_button, old_class[6], new_class[6], provider);
    remove_and_apply_style(widgets->new_chat_2_button, old_class[6], new_class[6], provider);
    remove_and_apply_style(widgets->log_out_button, old_class[6], new_class[6], provider);
    remove_and_apply_style(widgets->change_theme_button, old_class[6], new_class[6], provider);
    remove_and_apply_style(widgets->placeholder, old_class[7], new_class[7], provider);
    remove_and_apply_style(widgets->left_list_box, old_class[8], new_class[8], provider);
    remove_and_apply_style(widgets->left_list_box, old_class[9], new_class[9], provider);
    remove_and_apply_style(widgets->chat_info_label, old_class[10], new_class[10], provider);
}

void set_privat_theme(AppData *app_data) {
    ThemePrivat *widgets = app_data->theme_widgets3;
    int temp = app_data->theme;
    GtkCssProvider *provider = app_data->provider;

    // const char *default_theme_class[] = { "window", "au-entry", "au-errors", "au-button" };
    const char *black_theme_class[] = { "window_black", "au-entry_black", "au-errors_black", "au-button_black" };
    const char *green_theme_class[] = { "window-green", "au-entry-green", "au-errors-green", "au-button-green" };
    const char *pink_theme_class[] = { "window-pink", "au-entry-pink", "au-errors-pink", "au-button-pink" };
    const char *purple_theme_class[] = { "window-purple", "au-entry-purple", "au-errors-purple", "au-button-purple" };
    const char *blue_theme_class[] = { "window-blue", "au-entry-blue", "au-errors-blue", "au-button-blue" };
    const char *light_blue_theme_class[] = { "window-light_blue", "au-entry-light_blue", "au-errors-light_blue", "au-button-light_blue" };


    const char **old_class = NULL;
    const char **new_class = NULL;

    if (temp == 0) {
        old_class = light_blue_theme_class;
        new_class = green_theme_class;
    } else if (temp == 1) {
        old_class = green_theme_class;
        new_class = black_theme_class;
    } else if (temp == 2) {
        old_class = black_theme_class;
        new_class = pink_theme_class;
    } else if (temp == 3) {
        old_class = pink_theme_class;
        new_class = purple_theme_class;
    } else if (temp == 4) {
        old_class = purple_theme_class;
        new_class = blue_theme_class;
    } else if (temp == 5) {
        old_class = blue_theme_class;
        new_class = light_blue_theme_class;
    }

    remove_and_apply_style(widgets->popup, old_class[0], new_class[0], provider);
    remove_and_apply_style(widgets->username_entry, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->chat_title_entry, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->username_label, old_class[2], new_class[2], provider);
    remove_and_apply_style(widgets->chat_title_label, old_class[2], new_class[2], provider);
    remove_and_apply_style(widgets->create_chat_button, old_class[3], new_class[3], provider);

    app_data->theme1 = temp;
}

void set_group_theme(AppData *app_data) {
    ThemeGroup *widgets = app_data->theme_widgets4;
    int temp = app_data->theme;
    GtkCssProvider *provider = app_data->provider;

    // const char *default_theme_class[] = { "window", "au-entry", "au-errors", "au-button", "au-list_box", "scroll" };
    const char *black_theme_class[] = { "window_black", "au-entry_black", "au-errors_black", "au-button_black", "au-list_box_black", "scroll-black" };
    const char *green_theme_class[] = { "window-green", "au-entry-green", "au-errors-green", "au-button-green", "au-list_box-green", "scroll-green" };
    const char *pink_theme_class[] = { "window-pink", "au-entry-pink", "au-errors-pink", "au-button-pink", "au-list_box-pink", "scroll-pink" };
    const char *purple_theme_class[] = { "window-purple", "au-entry-purple", "au-errors-purple", "au-button-purple", "au-list_box-purple", "scroll-purple" };
    const char *blue_theme_class[] = { "window-blue", "au-entry-blue", "au-errors-blue", "au-button-blue", "au-list_box-blue", "scroll-blue" };
    const char *light_blue_theme_class[] = { "window-light_blue", "au-entry-light_blue", "au-errors-light_blue", "au-button-light_blue", "au-list_box-light_blue", "scroll-light_blue" };


    const char **old_class = NULL;
    const char **new_class = NULL;
    
    if (temp == 0) {
        old_class = light_blue_theme_class;
        new_class = green_theme_class;
    } else if (temp == 1) {
        old_class = green_theme_class;
        new_class = black_theme_class;
    } else if (temp == 2) {
        old_class = black_theme_class;
        new_class = pink_theme_class;
    } else if (temp == 3) {
        old_class = pink_theme_class;
        new_class = purple_theme_class;
    } else if (temp == 4) {
        old_class = purple_theme_class;
        new_class = blue_theme_class;
    } else if (temp == 5) {
        old_class = blue_theme_class;
        new_class = light_blue_theme_class;
    }

    remove_and_apply_style(widgets->popup, old_class[0], new_class[0], provider);
    remove_and_apply_style(widgets->username_entry, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->chat_title_entry, old_class[1], new_class[1], provider);
    remove_and_apply_style(widgets->username_label, old_class[2], new_class[2], provider);
    remove_and_apply_style(widgets->chat_title_label, old_class[2], new_class[2], provider);
    remove_and_apply_style(widgets->plus_button, old_class[3], new_class[3], provider);
    remove_and_apply_style(widgets->create_chat_button, old_class[3], new_class[3], provider);
    remove_and_apply_style(widgets->list_box, old_class[4], new_class[4], provider);
    remove_and_apply_style(widgets->scroll, old_class[5], new_class[5], provider);

    app_data->theme1 = temp;
}
