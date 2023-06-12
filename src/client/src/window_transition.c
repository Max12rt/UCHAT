#include "../../inc/main.h"

static gboolean animate_window_resize(gpointer user_data) {
    ResizeAnimationData *data = (ResizeAnimationData *)user_data;

    if (data->current_width >= CHAT_WINDOW_WIDTH && data->current_height >= CHAT_WINDOW_HEIGHT) {
        g_free(data);
        return FALSE;
    }

    if (data->current_width < CHAT_WINDOW_WIDTH) {
        data->current_width = data->current_width + data->step_x;
    }
    
    if (data->current_height < CHAT_WINDOW_HEIGHT) {
        data->current_height = data->current_height + data->step_y;
    }

    gtk_window_set_default_size(data->window, data->current_width, data->current_height);
    data->current_step++;
    // TODO center

    return TRUE;
}

void window_resize(AppData *app_data) {
    GtkWindow *window = GTK_WINDOW(app_data->window);

    ResizeAnimationData *data = g_new(ResizeAnimationData, 1);
    data->window = window;
    data->current_step = 0;
    data->current_width = START_WIDTH;
    data->current_height = START_HEIGHT;
    data->step_x = (float)(CHAT_WINDOW_WIDTH - START_WIDTH) / (float)ANIMATION_STEPS;
    data->step_y = (float)(CHAT_WINDOW_HEIGHT - START_HEIGHT) / (float)ANIMATION_STEPS;
    data->screen = get_screen_size(window);
    data->is_valid = TRUE;

    // g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), data);
    g_timeout_add(ANIMATION_INTERVAL, animate_window_resize, data);
}
