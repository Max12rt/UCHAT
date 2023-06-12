#include "../../inc/main.h"

static GtkWidget *create_form_grid(const gchar *username_placeholder, const gchar *password_placeholder, GtkWidget **username_entry, GtkWidget **password_entry, GtkWidget **username_error, GtkWidget **password_error, GtkCssProvider *provider, bool marging_top) {

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

    *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(*username_entry), username_placeholder);
    gtk_grid_attach(GTK_GRID(grid), *username_entry, 1, 0, 1, 1);

    *password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(*password_entry), password_placeholder);
    gtk_entry_set_input_purpose(GTK_ENTRY(*password_entry), GTK_INPUT_PURPOSE_PASSWORD);
    gtk_entry_set_visibility(GTK_ENTRY(*password_entry), FALSE);
    gtk_grid_attach(GTK_GRID(grid), *password_entry, 1, 2, 1, 1);

    *username_error = gtk_label_new(NULL);
    gtk_widget_set_halign(*username_error, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), *username_error, 1, 1, 1, 1);

    *password_error = gtk_label_new(NULL);
    gtk_widget_set_halign(*password_error, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), *password_error, 1, 3, 1, 1);

    if (marging_top) {
        gtk_widget_set_margin_top(*username_entry, 45);
    }

    widget_styling(*username_entry, "au-entry", provider);
    widget_styling(*password_entry, "au-entry", provider);

    return grid;
}

static void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    AppData *app_data = (AppData *) user_data;
    
    GtkEntry *username_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "username"));
    GtkEntry *password_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "password"));
    GtkLabel *error1 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "error1"));
    GtkLabel *error2 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "error2"));

    const char *username = gtk_editable_get_text(GTK_EDITABLE(username_entry));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(password_entry));
    bool errors = false;

    // Check if either the username or password is empty
    if (mx_strlen(username) == 0) {
        gtk_label_set_text(error1, "Error: Please enter a username");
        gtk_widget_set_visible(GTK_WIDGET(username_entry), TRUE);
        gtk_widget_grab_focus(GTK_WIDGET(username_entry));
        errors = true;
    } else {
        gtk_label_set_text(error1, "");
    }
    
    if (mx_strlen(password) == 0) {
        gtk_label_set_text(error2, "Error: Please enter a password");
        
        if (!errors) {
            gtk_widget_set_visible(GTK_WIDGET(password_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(password_entry));
            errors = true;
        }
    } else {
        gtk_label_set_text(error2, "");
    }

    if (errors) return;

    const char* json_string = "{\"type\":\"Authentication\",\"name\":\"%s\",\"password\":\"%s\",\"len\":%i}";

    char *pass_hash = mx_generate_password_hash(password);
    int len_enc;
    char *name = encrypt_data_stream(username, mx_strlen(username), &len_enc);

    int final_json_length = mx_strlen(json_string) + mx_strlen(name) + mx_strlen(pass_hash) + sizeof(len_enc) + 1;
    char *final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, name, pass_hash, len_enc);

    //write(sock, final_json_string, final_json_length);
    SSL_write(ssl, final_json_string, final_json_length);

    mx_strdel(&final_json_string);
    mx_strdel(&pass_hash);

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
        return;
    }

    json_t *id_json = json_object_get(root, "ID");
    bool passed = false;
    int id_value = 0;
    if (id_json && json_is_integer(id_json)) {
        id_value = json_integer_value(id_json);
        passed = id_value > 0;
    }

    json_decref(root);
    mx_strcpy(new_message, "");

    if (passed) {
        gtk_label_set_text(error1, "");
        gtk_label_set_text(error2, "");
        app_data->userID = id_value;
        app_data->username = mx_strdup(username);
        gtk_window_set_title(GTK_WINDOW(app_data->window), "Loading");
        create_chat_window(app_data);
    } else {
        gtk_label_set_text(error2, "Error: Incorrect credentials");
        gtk_widget_set_visible(GTK_WIDGET(password_entry), TRUE);
        gtk_widget_grab_focus(GTK_WIDGET(password_entry));
    }
}

static void on_signup_button_clicked(GtkButton *button, gpointer user_data) {
    AppData *app_data = (AppData *) user_data;

    GtkEntry *username_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "username"));
    GtkEntry *password_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "password1"));
    GtkEntry *password_confirm_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "password2"));
    GtkLabel *error1 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "error1"));
    GtkLabel *error2 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "error2"));
    GtkLabel *error3 = GTK_LABEL(g_object_get_data(G_OBJECT(button), "error3"));

    const char *username = gtk_editable_get_text(GTK_EDITABLE(username_entry));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(password_entry));
    const char *password_confirm = gtk_editable_get_text(GTK_EDITABLE(password_confirm_entry));
    bool errors = false;

    // Check if either the username or password is empty
    if (mx_strlen(username) == 0) {
        gtk_label_set_text(error1, "Error: Please enter a username");
        gtk_widget_set_visible(GTK_WIDGET(username_entry), TRUE);
        gtk_widget_grab_focus(GTK_WIDGET(username_entry));
        errors = true;
    } else {
        gtk_label_set_text(error1, "");
    }
    
    if (mx_strlen(password) == 0) {
        gtk_label_set_text(error2, "Error: Please enter a password");
        
        if (!errors) {
            gtk_widget_set_visible(GTK_WIDGET(password_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(password_entry));
            errors = true;
        }
    } else if (mx_strlen(password) < 8) {
        gtk_label_set_text(error2, "Error: Password is too short");
        
        if (!errors) { 
            gtk_widget_set_visible(GTK_WIDGET(password_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(password_entry));
            errors = true;
        }
    } else {
        gtk_label_set_text(error2, "");
    }

    if (mx_strlen(password_confirm) == 0) {
        gtk_label_set_text(error3, "Error: Please enter a confirmation");
        
        if (!errors) {
            gtk_widget_set_visible(GTK_WIDGET(password_confirm_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(password_confirm_entry));
            errors = true;
        }
    } else if (mx_strlen(password_confirm) < 8) {
        gtk_label_set_text(error3, "Error: Password is too short");
        
        if (!errors) {
            gtk_widget_set_visible(GTK_WIDGET(password_confirm_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(password_confirm_entry));
            errors = true;
        }
    } else if (mx_strcmp(password, password_confirm) != 0) {
        gtk_label_set_text(error3, "Error: Passwords do not matched");
        
        if (!errors) {
            gtk_widget_set_visible(GTK_WIDGET(password_confirm_entry), TRUE);
            gtk_widget_grab_focus(GTK_WIDGET(password_confirm_entry));
            errors = true;
        }
    } else {
        gtk_label_set_text(error3, "");
    }

    if (errors) return;

    if (mx_check_unique_username(app_data, username) == false) {
        gtk_label_set_text(error1, "Error: Username is already taken");
        gtk_widget_set_visible(GTK_WIDGET(username_entry), TRUE);
        gtk_widget_grab_focus(GTK_WIDGET(username_entry));
        return;
    }

    const char* json_string = "{\"type\":\"Registration\",\"name\":\"%s\",\"password\":\"%s\",\"len\":%i}";

    char *pass_hash = mx_generate_password_hash(password);
    int len_enc;
    char *gen_name = encrypt_data_stream(username, mx_strlen(username), &len_enc);
   
    int final_json_length = strlen(json_string) + mx_strlen(gen_name) +  strlen(pass_hash) + sizeof(len_enc) + 1;
    char *final_json_string = (char*) malloc(final_json_length * sizeof(char));
    snprintf(final_json_string, final_json_length, json_string, gen_name, pass_hash, len_enc);

    //write(sock, final_json_string, final_json_length);
    SSL_write(ssl, final_json_string, final_json_length);

    mx_strdel(&final_json_string);
    mx_strdel(&pass_hash);

    //read from server part
    char new_message[2048];
    int message_length;

    //message_length = read(sock, new_message, 2048);
    message_length = SSL_read(ssl, new_message, sizeof(new_message));
    new_message[message_length] = '\0';

    if (message_length == 0) {
        error_window(app_data, "Server error", NULL, NULL);
        // exit(1);
    }

    json_error_t error;
    json_t *root = json_loads(new_message, 0, &error);
    if (!root) {
        printf("Error parsing JSON string: %s\n", error.text);
        return;
    }

    bool passed = false;
    int id_value = 0;
    json_t *id_json = json_object_get(root, "ID");
    if (id_json && json_is_integer(id_json)) {
        id_value = json_integer_value(id_json);
        printf("ID value: %d\n", id_value);
        passed = id_value > 0;
    }

    json_decref(root);
    mx_strcpy(new_message, "");

    if (passed) {
        gtk_label_set_text(error1, "");
        gtk_label_set_text(error2, "");
        app_data->userID = id_value;
        app_data->username = mx_strdup(username);
        gtk_window_set_title(GTK_WINDOW(app_data->window), "Loading");
        create_chat_window(app_data);
    }
}

static void switch_to_login_form(GtkButton *button, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), "login_form");
}

static void switch_to_signup_form(GtkButton *button, gpointer user_data) {
    AppData *app_data = (AppData *)user_data;
    gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), "signup_form");
}

static void on_window_destroy(GtkWidget *widget, gpointer data) {
    AppData *app_data = data;
    free(app_data->theme_widgets2);
    g_free(app_data);
}

void authorization(GtkApplication *app) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "src/client/css/main.css");

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Authorization window");
    gtk_window_set_default_size(GTK_WINDOW(window), START_WIDTH, START_HEIGHT);
    gtk_widget_set_size_request(window, START_WIDTH, START_HEIGHT);

    AppData *app_data = g_new(AppData, 1);
    app_data->app = app;
    app_data->provider = provider;
    app_data->window = window;
    app_data->theme_window = "authorization";
    app_data->theme = 0;

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_widget_set_hexpand(main_box, TRUE);
    gtk_widget_set_vexpand(main_box, TRUE);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    // gtk_widget_set_size_request(vbox, 200, 250); // TODO
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(vbox, TRUE);
    gtk_widget_set_vexpand(vbox, TRUE);

    app_data->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app_data->stack), GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(app_data->stack), 1000);

    GtkWidget *login_form = create_form_grid("Username", "Password", &app_data->login_username_entry, &app_data->login_password_entry, &app_data->login_username_error, &app_data->login_password_error, provider, true);
    GtkWidget *signup_form = create_form_grid("Username", "Password", &app_data->signup_username_entry, &app_data->signup_password_entry, &app_data->signup_username_error, &app_data->signup_password_error, provider, false);

    app_data->signup_confirm_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app_data->signup_confirm_password_entry), "Confirm password");
    gtk_entry_set_input_purpose(GTK_ENTRY(app_data->signup_confirm_password_entry), GTK_INPUT_PURPOSE_PASSWORD);
    gtk_entry_set_visibility(GTK_ENTRY(app_data->signup_confirm_password_entry), FALSE);
    gtk_grid_attach(GTK_GRID(signup_form), app_data->signup_confirm_password_entry, 1, 4, 1, 1);

    GtkWidget *confirm_password_error = gtk_label_new(NULL);
    gtk_widget_set_halign(confirm_password_error, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(signup_form), confirm_password_error, 1, 5, 1, 1);
    app_data->signup_confirm_password_error = confirm_password_error;

    GtkWidget *login_button = gtk_button_new_with_label("Sign in");
    gtk_widget_set_size_request(login_button, 200, 40);

    // Set the data for the login button
    g_object_set_data(G_OBJECT(login_button), "username", app_data->login_username_entry);
    g_object_set_data(G_OBJECT(login_button), "password", app_data->login_password_entry);
    g_object_set_data(G_OBJECT(login_button), "error1", app_data->login_username_error);
    g_object_set_data(G_OBJECT(login_button), "error2", app_data->login_password_error);

    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), app_data);
    gtk_grid_attach(GTK_GRID(login_form), login_button, 1, 4, 1, 1);

    GtkWidget *signup_button = gtk_button_new_with_label("Sign up");
    gtk_widget_set_size_request(signup_button, 200, 40);

    // Set the data for the register button
    g_object_set_data(G_OBJECT(signup_button), "username", app_data->signup_username_entry);
    g_object_set_data(G_OBJECT(signup_button), "password1", app_data->signup_password_entry);
    g_object_set_data(G_OBJECT(signup_button), "password2", app_data->signup_confirm_password_entry);
    g_object_set_data(G_OBJECT(signup_button), "error1", app_data->signup_username_error);
    g_object_set_data(G_OBJECT(signup_button), "error2", app_data->signup_password_error);
    g_object_set_data(G_OBJECT(signup_button), "error3", app_data->signup_confirm_password_error);

    g_signal_connect(signup_button, "clicked", G_CALLBACK(on_signup_button_clicked), app_data);
    gtk_grid_attach(GTK_GRID(signup_form), signup_button, 1, 6, 1, 1);

    gtk_stack_add_titled(GTK_STACK(app_data->stack), login_form, "login_form", "Sing In");
    gtk_stack_add_titled(GTK_STACK(app_data->stack), signup_form, "signup_form", "Sign Up");

    // Create buttons to switch between login and sign up forms
    GtkWidget *switch_to_login_button = gtk_button_new_with_label("Sing In");
    GtkWidget *switch_to_signup_button = gtk_button_new_with_label("Sign Up");
    gtk_widget_set_margin_start(switch_to_signup_button, 10);

    GtkWidget *change_theme_button = gtk_button_new_with_label("Change Theme");

    ThemeAuthorization *theme = (ThemeAuthorization *) malloc(sizeof(ThemeAuthorization));

    theme->main_box = main_box;
    theme->switch_to_login_button = switch_to_login_button;
    theme->switch_to_signup_button = switch_to_signup_button;
    theme->login_button = login_button;
    theme->signup_button = signup_button;
    theme->change_theme_button = change_theme_button;
    theme->signup_confirm_password_entry = app_data->signup_confirm_password_entry;
    theme->login_username_error = app_data->login_username_error;
    theme->login_password_error = app_data->login_password_error;
    theme->signup_username_error = app_data->signup_username_error;
    theme->signup_password_error = app_data->signup_password_error;
    theme->signup_confirm_password_error = app_data->signup_confirm_password_error;
    theme->login_form = login_form;
    theme->signup_form = signup_form;

    app_data->theme_widgets2 = theme;
    set_authorization_theme(app_data);
    g_signal_connect(change_theme_button, "clicked", G_CALLBACK(on_change_theme_button_clicked), app_data);

    // Connect the buttons to switch the stack
    g_signal_connect(switch_to_login_button, "clicked", G_CALLBACK(switch_to_login_form), app_data);
    g_signal_connect(switch_to_signup_button, "clicked", G_CALLBACK(switch_to_signup_form), app_data);

    // Create an hbox to hold the buttons
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    gtk_widget_set_halign(switch_to_login_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(switch_to_signup_button, GTK_ALIGN_CENTER);

    gtk_widget_set_halign(hbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(hbox, GTK_ALIGN_CENTER);

    gtk_box_append(GTK_BOX(hbox), switch_to_login_button);
    gtk_box_append(GTK_BOX(hbox), switch_to_signup_button);

    // Add margin to the hbox
    gtk_widget_set_margin_top(hbox, 20);
    gtk_widget_set_margin_bottom(hbox, 20);

    // Align form widgets
    gtk_widget_set_margin_start(app_data->login_username_entry, 20);
    gtk_widget_set_margin_end(app_data->login_username_entry, 20);
    gtk_widget_set_margin_start(app_data->login_password_entry, 20);
    gtk_widget_set_margin_end(app_data->login_password_entry, 20);

    gtk_widget_set_halign(app_data->login_username_error, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(app_data->login_password_error, GTK_ALIGN_CENTER);

    gtk_widget_set_margin_start(login_button, 20);
    gtk_widget_set_margin_end(login_button, 20);
    gtk_widget_set_margin_bottom(login_button, 10);

    gtk_widget_set_margin_start(app_data->signup_username_entry, 20);
    gtk_widget_set_margin_end(app_data->signup_username_entry, 20);
    gtk_widget_set_margin_start(app_data->signup_password_entry, 20);
    gtk_widget_set_margin_end(app_data->signup_password_entry, 20);
    gtk_widget_set_margin_start(app_data->signup_confirm_password_entry, 20);
    gtk_widget_set_margin_end(app_data->signup_confirm_password_entry, 20);

    gtk_widget_set_halign(app_data->signup_username_error, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(app_data->signup_password_error, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(app_data->signup_confirm_password_error, GTK_ALIGN_CENTER);

    gtk_widget_set_margin_start(signup_button, 20);
    gtk_widget_set_margin_end(signup_button, 20);
    gtk_widget_set_margin_bottom(signup_button, 35);

    gtk_widget_set_margin_start(change_theme_button, 20);
    gtk_widget_set_margin_end(change_theme_button, 20);
    gtk_widget_set_margin_top(change_theme_button, 20);

    // Add hbox and stack to vbox
    gtk_box_append(GTK_BOX(vbox), change_theme_button);
    gtk_box_append(GTK_BOX(vbox), hbox);
    gtk_box_append(GTK_BOX(vbox), app_data->stack);
    gtk_box_append(GTK_BOX(main_box), vbox);

    gtk_window_set_child(GTK_WINDOW(window), main_box);

    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), app_data);
    gtk_widget_set_visible(window, TRUE);
}
