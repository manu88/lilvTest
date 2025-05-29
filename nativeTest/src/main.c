#include "plugins.h"
#include "uri.h"
#include <assert.h>
#include <gtk/gtk.h>
#include <lilv/lilv.h>
#include <lv2/atom/atom.h>
#include <lv2/ui/ui.h>
#include <lv2/urid/urid.h>
#include <serd/serd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MACOS
#define LV2_PATH "/opt/homebrew/lib/lv2/"
#else
#define LV2_PATH "/usr/local/lib/aarch64-linux-gnu/lv2"
#endif

PluginsContext ctx;

static void OnDestroy(GtkWidget *pWidget, gpointer pData) { gtk_main_quit(); }

int main(int argc, char **argv) {

  gtk_init(0, NULL);
  plugins_ctx_init(&ctx);
  plugins_print_all(&ctx);

  const char *pluginURI = "http://lv2plug.in/plugins/eg-scope#Stereo";
  const LilvPlugin *plug = plugins_get_plugin(&ctx, pluginURI);
  assert(plug);

  printf("Create SUIL instance for '%s'\n", pluginURI);

  const LilvNode *binaryURINode = NULL;
  const LilvNode *bundleURINode = NULL;

  const LilvUI *ui = NULL;
  LILV_API LilvUIs *uis = lilv_plugin_get_uis(plug);
  LILV_FOREACH(uis, i, uis) {
    ui = lilv_uis_get(uis, i);
    const LilvNodes *uiClasses = lilv_ui_get_classes(ui);

    LILV_FOREACH(nodes, iter, uiClasses) {
      const LilvNodes *classNode = lilv_nodes_get(uiClasses, iter);
      printf("UI class '%s'\n", lilv_node_as_string(classNode));
    }
    binaryURINode = lilv_ui_get_binary_uri(ui);
    bundleURINode = lilv_ui_get_bundle_uri(ui);
    break;
  }
  free(uis);
  printf("binary URI= '%s'  bundle URI='%s'\n",
         lilv_node_as_string(binaryURINode),
         lilv_node_as_string(bundleURINode));

  LV2_URID_Map mapHandle;
  mapHandle.map = uri_table_map;
  mapHandle.handle = &ctx.uri_table;
  LV2_Feature feat;
  feat.URI = LV2_URID__map;
  feat.data = &mapHandle;
  const LV2_Feature *const features[2] = {&feat, NULL};

  const char *bundle_uri = lilv_node_as_uri(lilv_ui_get_bundle_uri(ui));
  const char *binary_uri = lilv_node_as_uri(lilv_ui_get_binary_uri(ui));
  const char *bundle_path =
      (const char *)serd_file_uri_parse((const uint8_t *)bundle_uri, NULL);
  const char *binary_path =
      (const char *)serd_file_uri_parse((const uint8_t *)binary_uri, NULL);

  SuilInstance *uiInstance =
      suil_instance_new(ctx.host, &ctx, LV2_UI__GtkUI,
                        lilv_node_as_uri(lilv_plugin_get_uri(plug)),
                        lilv_node_as_uri(lilv_ui_get_uri(ui)), LV2_UI__GtkUI,
                        bundle_path, binary_path, features);
  assert(uiInstance);

  GtkWidget *plugWin = (GtkWidget *)suil_instance_get_widget(uiInstance);
  assert(plugWin);
  GtkWidget *pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(OnDestroy), NULL);
  gtk_window_set_position(GTK_WINDOW(pWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(pWindow), "GTK Plugin");
  gtk_container_add(GTK_CONTAINER(pWindow), plugWin);
  gtk_widget_show_all(pWindow);
  gtk_main();

  suil_instance_free(uiInstance);
  plugins_ctx_release(&ctx);
  return 0;
}
