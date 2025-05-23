#include <assert.h>
#include <lilv/lilv.h>
#include <lv2/atom/atom.h>
#include <lv2/urid/urid.h>
#include <stdio.h>
#include <string.h>

#ifdef MACOS
#define LV2_PATH "/opt/homebrew/lib/lv2/"
#else
#define LV2_PATH "/usr/local/lib/aarch64-linux-gnu/lv2"
#endif

LilvWorld *world = nullptr;
const LilvPlugin *pluginToTest = nullptr;

static void printPluginInfos(const LilvPlugin *p) {
  LilvNode *nodeName = lilv_plugin_get_name(p);
  const LilvPluginClass *nodeClass = lilv_plugin_get_class(p);
  
  const LilvNode *nodeClassLabel = lilv_plugin_class_get_label(nodeClass);
  uint32_t numPorts = lilv_plugin_get_num_ports(p);
  printf("plugin '%s' name = '%s' class '%s' ports=%i\n",
         lilv_node_as_string(lilv_plugin_get_uri(p)),
         lilv_node_as_string(nodeName), lilv_node_as_string(nodeClassLabel),
         numPorts);

  printf("Ports:\n");
  LilvNode* portConnectionOptional =
      lilv_new_uri(world, LV2_CORE__connectionOptional);
  for (uint32_t i = 0; i < numPorts; i++) {
    const LilvPort *port = lilv_plugin_get_port_by_index(p, i);
    bool optional = lilv_port_has_property(p, port, portConnectionOptional);
    const LilvNodes *portClasses = lilv_port_get_classes(p, port);
    bool isInput = false;
    bool isAudio = false;
    bool isAtom = false;
    LILV_FOREACH(nodes, pC, portClasses) {
      const LilvNode *portClass = lilv_nodes_get(portClasses, pC);
      const char *portClassName = lilv_node_as_string(portClass);
      
      bool handled = false;
      if (strcmp(portClassName, LV2_ATOM__AtomPort) == 0) {
        isAtom = true;
        handled = true;
      }
      if (strcmp(portClassName, LV2_CORE__ControlPort) == 0) {
        isAudio = false;
        handled = true;
      } else if (strcmp(portClassName, LV2_CORE__AudioPort) == 0) {
        isAudio = true;
        handled = true;
      }
      if (strcmp(portClassName, LV2_CORE__InputPort) == 0) {
        isInput = true;
        handled = true;
      } else if (strcmp(portClassName, LV2_CORE__OutputPort) == 0) {
        isInput = false;
        handled = true;
      }
      if (!handled) {
        printf("\t\t '%s'\n", portClassName);
        assert(false);
      }
    }
    LilvNode *portName = lilv_port_get_name(p, port);
    printf("\t port %i: %s %s %sname '%s' %s\n", i,
           (isAudio ? "audio" : "control"), (isInput ? "input" : "output"),
           (isAtom ? " (atom) " : ""), lilv_node_as_string(portName),
           (optional? " Optional": ""));
    lilv_node_free(portName);



  }
  if (strcmp(lilv_node_as_string(nodeName), "Example Parameters") == 0) {
    pluginToTest = p;
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

static LV2_URID mapIndex = 0;
static LV2_URID doMap(LV2_URID_Map_Handle handle, const char *uri) {
  assert(handle == pluginToTest);
  printf("doMap called with uri '%s'\n", uri);
  return mapIndex++;
}

int main() {
  printf("Test Lilv\n");
  world = lilv_world_new();

  LilvNode *lv2_path = lilv_new_string(world, LV2_PATH);

  lilv_world_set_option(world, LILV_OPTION_LV2_PATH, lv2_path);
  lilv_world_load_all(world);

  const LilvPlugins *plugins = lilv_world_get_all_plugins(world);

  LILV_FOREACH(plugins, i, plugins) {
    const LilvPlugin *p = lilv_plugins_get(plugins, i);
    printPluginInfos(p);
    printf("\n\n");
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

  LV2_Atom_Int input[128];
  LV2_Atom_Int output[128];
  for (int i=0;i<128;i++){
    input[i].body = i;
    output[i].body = 0;
  }

  
  lilv_instance_connect_port(instance, 0, &input);
  lilv_instance_connect_port(instance, 1, &output);
  lilv_instance_activate(instance);
  lilv_instance_run(instance, 1);
  for (int i=0;i<128;i++){
    printf("%i: %i\n",i , output[i].body);
  }
  
  
  lilv_instance_free(instance);
  lilv_world_free(world);
  return 0;
}
