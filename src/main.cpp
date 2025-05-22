#include <assert.h>
#include <lilv/lilv.h>
#include <lv2/urid/urid.h>
#include <stdio.h>
#include <string.h>

#ifdef MACOS
#define LV2_PATH "/opt/homebrew/lib/lv2/"
#else
#define LV2_PATH "/usr/local/lib/aarch64-linux-gnu/lv2"
#endif

const LilvPlugin *pluginToTest = nullptr;

static void printPluginInfos(const LilvPlugin *p) {
  LilvNode *nodeName = lilv_plugin_get_name(p);
  const LilvPluginClass *nodeClass = lilv_plugin_get_class(p);
  const LilvNode *nodeClassLabel = lilv_plugin_class_get_label(nodeClass);
  uint32_t numPorts = lilv_plugin_get_num_ports(p);
  printf("plugin name = '%s' class '%s' ports=%i\n",
         lilv_node_as_string(nodeName), lilv_node_as_string(nodeClassLabel),
         numPorts);

  printf("Ports:\n");
  for (uint32_t i = 0; i < numPorts; i++) {
    const LilvPort *port = lilv_plugin_get_port_by_index(p, i);
    LilvNode *portName = lilv_port_get_name(p, port);
    printf("\t port %i: name '%s'\n", i, lilv_node_as_string(portName));
    lilv_node_free(portName);

    const LilvNodes *portClasses = lilv_port_get_classes(p, port);
    LILV_FOREACH(nodes, pC, portClasses) {
      const LilvNode *portClass = lilv_nodes_get(portClasses, pC);
      const char *portClassName = lilv_node_as_string(portClass);
      if (strcmp(portClassName, LV2_CORE__ControlPort) == 0) {
        printf("\t\tControl\n");
      }
      if (strcmp(portClassName, LV2_CORE__InputPort) == 0) {
        printf("\t\tInput\n");
      } else if (strcmp(portClassName, LV2_CORE__OutputPort) == 0) {
        printf("\t\tOutput\n");
      } else {
        printf("\t\t '%s'\n", portClassName);
      }
    }
  }

  lilv_node_free(nodeName);

  printf("features:\n");
  LilvNodes *features = lilv_plugin_get_required_features(p);
  LILV_FOREACH(nodes, fI, features) {
    const LilvNode *nodeFeat = lilv_nodes_get(features, fI);
    const char *featName = lilv_node_as_string(nodeFeat);
    printf("\trequired feat: '%s'\n", featName);
  }
  lilv_nodes_free(features);

  features = lilv_plugin_get_optional_features(p);
  LILV_FOREACH(nodes, fI, features) {
    const LilvNode *nodeFeat = lilv_nodes_get(features, fI);
    const char *featName = lilv_node_as_string(nodeFeat);
    printf("\toptional feat: '%s'\n", featName);
  }
  lilv_nodes_free(features);
}

static LV2_URID doMap(LV2_URID_Map_Handle handle, const char *uri) {
  assert(handle == pluginToTest);
  printf("doMap called with uri '%s'\n", uri);
  return 0;
}

int main() {
  printf("Test Lilv\n");
  LilvWorld *world = lilv_world_new();

  LilvNode *lv2_path = lilv_new_string(world, LV2_PATH);

  lilv_world_set_option(world, LILV_OPTION_LV2_PATH, lv2_path);
  lilv_world_load_all(world);

  const LilvPlugins *plugins = lilv_world_get_all_plugins(world);

  LILV_FOREACH(plugins, i, plugins) {
    const LilvPlugin *p = lilv_plugins_get(plugins, i);
    printPluginInfos(p);
    if (pluginToTest == NULL) {
      pluginToTest = p;
    }
  }

  LV2_URID_Map mapHandle;
  mapHandle.map = doMap;
  mapHandle.handle = (void *)pluginToTest;
  LV2_Feature feat;
  feat.URI = LV2_URID__map;
  feat.data = &mapHandle;
  LV2_Feature *features[2] = {&feat, NULL};
  LilvInstance *instance =
      lilv_plugin_instantiate(pluginToTest, 48000.0, features);
  assert(instance);

  lilv_instance_free(instance);
  lilv_world_free(world);
  return 0;
}
