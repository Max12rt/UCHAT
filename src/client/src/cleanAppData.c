#include "../../inc/main.h"

void cleanup_app_data(AppData *app_data) {
    if (app_data->list_box) {
        GtkWidget *child;
        while ((child = gtk_widget_get_first_child(app_data->list_box))) {
            gtk_list_box_remove(GTK_LIST_BOX(app_data->list_box), child);
        }
    }
 
    if (app_data->app) {
        g_object_unref(app_data->app);
        app_data->app = NULL;
    }

    if (app_data->provider) {
        g_object_unref(app_data->provider);
        app_data->provider = NULL;
    }

    if (app_data->theme_widgets1 != NULL) {
        free(app_data->theme_widgets3);
    }

    if (app_data->theme_widgets2 != NULL) {
        free(app_data->theme_widgets4);
    }

}
