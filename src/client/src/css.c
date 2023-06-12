#include "../../inc/main.h"

void widget_styling(GtkWidget *widget, const char *name_of_css_class, GtkCssProvider *provider) {
    gtk_widget_add_css_class(widget, name_of_css_class); 
    GdkDisplay *display = gtk_widget_get_display(widget);
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER); 
}
