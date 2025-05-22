#include <assert.h>
#include <lilv/lilv.h>
#include <stdio.h>

static void printPluginInfo(const LilvPlugin *p) {
  const LilvNode *nodeURI = lilv_plugin_get_uri(p);
  const LilvNode *nodeName = lilv_plugin_get_name(p);
  printf("URI: '%s' Name: '%s'\n", lilv_node_as_uri(nodeURI),
         lilv_node_as_string(nodeName));

  const LilvNodes* features = lilv_plugin_get_supported_features(p);
}

int main() {
  printf("Test Lilv\n");
  LilvWorld *world = lilv_world_new();

  LilvNode *lv2_path =
      lilv_new_string(world, "/usr/local/lib/aarch64-linux-gnu/lv2");

  lilv_world_set_option(world, LILV_OPTION_LV2_PATH, lv2_path);
  lilv_world_load_all(world);

  const LilvPlugins *plugins = lilv_world_get_all_plugins(world);

  const LilvPlugin *pluginToTest = nullptr;
  LILV_FOREACH(plugins, i, plugins) {
    const LilvPlugin *p = lilv_plugins_get(plugins, i);
    printPluginInfo(p);
    pluginToTest = p;
  }

  LilvInstance *instance = lilv_plugin_instantiate(pluginToTest, 48000.0, NULL);
  assert(instance);
#if 0
  printf("Test load specific plugin\n");
  LilvNode *bundle_uri =
      lilv_new_file_uri(world, NULL, "/usr/lib/lv2/atom.lv2");

  printf("bundle ptr %p\n", bundle_uri);
  lilv_world_load_bundle(world, bundle_uri);
#endif
  lilv_world_free(world);
  return 0;
}