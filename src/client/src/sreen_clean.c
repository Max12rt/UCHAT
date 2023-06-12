#include <gtk/gtk.h>

void clear_container(GtkWindow *window) {
    GtkWidget *content_area = gtk_window_get_child(window);

    if (GTK_IS_WIDGET(content_area)) {
        GtkWidget *child;
        while ((child = gtk_widget_get_first_child(content_area)) != NULL) {
            gtk_widget_unparent(child);
        }
    }
}

void clear_list_box(GtkListBox *list_box) {
    GtkListBoxRow *row;

    while ((row = gtk_list_box_get_row_at_index(list_box, 0)) != NULL) {
        gtk_list_box_remove(list_box, GTK_WIDGET(row));
    }
}
