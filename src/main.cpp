#include <assert.h>
#include <lilv/lilv.h>
#include <stdio.h>

#ifdef MACOS
#define LV2_PATH "/opt/homebrew/lib/lv2/"
#else
#define LV2_PATH "/usr/local/lib/aarch64-linux-gnu/lv2"
#endif

static void printPluginInfos(const LilvPlugin *p) {
  LilvNode *nodeName = lilv_plugin_get_name(p);
  const LilvPluginClass *nodeClass = lilv_plugin_get_class(p);
  const LilvNode *nodeClassLabel = lilv_plugin_class_get_label(nodeClass);
  printf("plugin name = '%s' class '%s'\n", lilv_node_as_string(nodeName),
         lilv_node_as_string(nodeClassLabel));
  lilv_node_free(nodeName);

  LilvNodes *features = lilv_plugin_get_supported_features(p);
  LILV_FOREACH(nodes, fI, features) {
    const LilvNode *nodeFeat = lilv_nodes_get(features, fI);
    const char *featName = lilv_node_as_string(nodeFeat);
    printf("\tfeat: '%s'\n", featName);
  }
  lilv_nodes_free(features);
}

int main() {
  printf("Test Lilv\n");
  LilvWorld *world = lilv_world_new();

  LilvNode *lv2_path = lilv_new_string(world, LV2_PATH);

  lilv_world_set_option(world, LILV_OPTION_LV2_PATH, lv2_path);
  lilv_world_load_all(world);

  const LilvPlugins *plugins = lilv_world_get_all_plugins(world);

  const LilvPlugin *pluginToTest = nullptr;
  LILV_FOREACH(plugins, i, plugins) {
    const LilvPlugin *p = lilv_plugins_get(plugins, i);
    printPluginInfos(p);
    pluginToTest = p;
  }

#if 0
    LilvInstance *instance =
        lilv_plugin_instantiate(pluginToTest, 48000.0, NULL);
    assert(instance);

  printf("Test load specific plugin\n");
  LilvNode *bundle_uri =
      lilv_new_file_uri(world, NULL, "/usr/lib/lv2/atom.lv2");

  printf("bundle ptr %p\n", bundle_uri);
  lilv_world_load_bundle(world, bundle_uri);
#endif
  lilv_world_free(world);
  return 0;
}
