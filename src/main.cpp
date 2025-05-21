#include <assert.h>
#include <lilv/lilv.h>
#include <stdio.h>
int main() {
  printf("Test Lilv\n");
  LilvWorld *world = lilv_world_new();

  LilvNode *lv2_path =
      lilv_new_string(world, "/usr/local/lib/aarch64-linux-gnu/lv2");

  lilv_world_set_option(world, LILV_OPTION_LV2_PATH, lv2_path);
  lilv_world_load_all(world);

  const LilvPlugins *plugins = lilv_world_get_all_plugins(world);

  LILV_FOREACH(plugins, i, plugins) {
    const LilvPlugin *p = lilv_plugins_get(plugins, i);
    printf("%s\n", lilv_node_as_uri(lilv_plugin_get_uri(p)));
  }

  printf("Test load specific plugin\n");
  LilvNode *bundle_uri =
      lilv_new_file_uri(world, NULL, "/usr/lib/lv2/atom.lv2");

  printf("bundle ptr %p\n", bundle_uri);
  lilv_world_load_bundle(world, bundle_uri);

  return 0;
}