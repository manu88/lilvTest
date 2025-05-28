#include "plugins.h"
#include "uri.h"
#include <assert.h>
#include <gtk/gtk.h>
#include <lilv/lilv.h>
#include <lv2/atom/atom.h>
#include <lv2/urid/urid.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MACOS
#define LV2_PATH "/opt/homebrew/lib/lv2/"
#else
#define LV2_PATH "/usr/local/lib/aarch64-linux-gnu/lv2"
#endif

void OnDestroy(GtkWidget *pWidget, gpointer pData);

int main(int argc, char **argv) {
  plugins_ctx_init();
  plugins_print_all();

  const LilvPlugin *plug =
      plugins_get_plugin("http://lv2plug.in/plugins/eg-scope#Stereo");
  assert(plug);
  GtkWidget *pWindow;
  gtk_init(&argc, &argv);

  pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(OnDestroy), NULL);

  gtk_widget_show(pWindow);

  gtk_main();

  plugins_ctx_release();

  return EXIT_SUCCESS;
}

void OnDestroy(GtkWidget *pWidget, gpointer pData) { gtk_main_quit(); }

LilvWorld *world = NULL;
const LilvPlugin *pluginToTest = NULL;

int main2() {
  printf("Test Lilv\n");
  world = lilv_world_new();

  LilvNode *lv2_path = lilv_new_string(world, LV2_PATH);
  lilv_world_set_option(world, LILV_OPTION_LV2_PATH, lv2_path);
  lilv_node_free(lv2_path);
  lilv_world_load_all(world);

  const LilvPlugins *plugins = lilv_world_get_all_plugins(world);
  LILV_FOREACH(plugins, i, plugins) {
    const LilvPlugin *p = lilv_plugins_get(plugins, i);
    plugins_print_info(p);
    printf("\n\n");
  }

  URITable uri_table;
  uri_table_init(&uri_table);

  LV2_URID_Map mapHandle;
  mapHandle.map = uri_table_map;
  mapHandle.handle = &uri_table;
  LV2_Feature feat;
  feat.URI = LV2_URID__map;
  feat.data = &mapHandle;
  LV2_Feature *features[2] = {&feat, NULL};
  LilvInstance *instance =
      lilv_plugin_instantiate(pluginToTest, 48000.0, features);
  assert(instance);

  const size_t atom_capacity = 1024;

  LV2_Atom_Sequence seq_in = {{sizeof(LV2_Atom_Sequence_Body),
                               uri_table_map(&uri_table, LV2_ATOM__Sequence)},
                              {0, 0}};

  LV2_Atom_Sequence *seq_out =
      (LV2_Atom_Sequence *)malloc(sizeof(LV2_Atom_Sequence) + atom_capacity);

  seq_out->atom.size = atom_capacity;
  seq_out->atom.type = uri_table_map(&uri_table, LV2_ATOM__Chunk);

  lilv_instance_connect_port(instance, 0, &seq_in);
  lilv_instance_connect_port(instance, 1, seq_out);
  lilv_instance_activate(instance);
  lilv_instance_run(instance, 64);

  lilv_instance_free(instance);
  lilv_world_free(world);
  return 0;
}
