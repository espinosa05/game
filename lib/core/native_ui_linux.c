#include <core/native_ui.h>
#include <core/utils.h>
#include <gtk/gtk.h>



void ShowMessageWindow(GtkApplication *gtkApp,
                       gpointer userData)
{
}

void IMPL_AssertGUI(char *assertString, usz line, char *file, char *func,
                    char *msg, char *detailedErrorFmt, ...)
{
    GtkApplication *gtkApp;
    GtkWindow *window;

    gtkApp = gtk_application_new("native.ui", G_APPLICATION_FLAGS);
    g_signal_connect(gtkApp, "activate", G_CALLBACK(ShowMessageWindow));


}

