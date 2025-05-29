#include <stdlib.h>
#include <gtk/gtk.h> 

void OnDestroy(GtkWidget *pWidget, gpointer pData);

int main(int argc,char **argv)
{ 
    /* Déclaration du widget */
    GtkWidget *pWindow;
    gtk_init(&argc,&argv);
    
    /* Création de la fenêtre */
    pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(pWindow), "GTK2 widget");
    gtk_window_set_default_size(GTK_WINDOW(pWindow), 320, 200);
    /* Connexion du signal "destroy" */
    g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(OnDestroy), NULL);
    /* Affichage de la fenêtre */


    GtkWidget* label = gtk_label_new("label");
    gtk_container_add(GTK_CONTAINER(pWindow), label);
    gtk_widget_show_all(pWindow);
    /* Demarrage de la boucle évènementielle */
    gtk_main();
    
    return EXIT_SUCCESS;
}

void OnDestroy(GtkWidget *pWidget, gpointer pData)
{
    /* Arret de la boucle évènementielle */
    gtk_main_quit();
}
