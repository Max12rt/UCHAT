#include "../../inc/main.h"

ScreenParams get_screen_size(GtkWindow *window) {
    ScreenParams screen;

    GdkSurface *surface = gtk_native_get_surface(gtk_widget_get_native((GtkWidget *)window));
    GdkDisplay *display = gdk_surface_get_display(surface);
    GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display, surface);

    GdkRectangle monitor_geometry;
    gdk_monitor_get_geometry(monitor, &monitor_geometry);

    screen.width = monitor_geometry.width;
    screen.height = monitor_geometry.height;

    return screen;
}
