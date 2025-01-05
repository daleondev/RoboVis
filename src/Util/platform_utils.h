#pragma once

#ifdef __linux__

#include <gtk/gtk.h>

namespace glfw {
    #define GLFW_EXPOSE_NATIVE_X11
    #include <GLFW/glfw3native.h>
}

extern "C" {
    typedef struct _GdkWindow GdkWindow;
    GdkWindow* gdk_x11_window_foreign_new_for_display(GdkDisplay* display, long unsigned int window);
    long unsigned int   gdk_x11_window_get_xid           (GdkWindow   *window);
}

static void platformInit()
{
    gtk_init(nullptr, nullptr);
}

static void platformUpdate()
{
    gtk_main_iteration_do(false);
}

static GtkWindow* gtk_window_new_from_gdk_window(GdkWindow* gdk_window)
{
    GtkWindow* gtk_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_widget_set_window(GTK_WIDGET(gtk_window), gdk_window);
    return gtk_window;
}

static std::string openFileDialog(GLFWwindow* glfwParent)
{
    auto x11Parent = glfw::glfwGetX11Window(glfwParent);
    GdkDisplay* gdkDisplay = gdk_display_get_default();
    GdkWindow* gdkParent = gdk_x11_window_foreign_new_for_display(gdkDisplay, x11Parent);
    GtkWindow* gtkParent = gtk_window_new_from_gdk_window(gdkParent);

    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open File",
                                         gtkParent,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "Cancel", GTK_RESPONSE_CANCEL,
                                         "Open", GTK_RESPONSE_ACCEPT,
                                         nullptr);

    std::string file = "";
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        file = filename;
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
    gtk_widget_destroy(GTK_WIDGET(gtkParent));
    return file;
}

#elif _WIN32

#endif