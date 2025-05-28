#include "plugins.h"
#include "uri.h"
#include <assert.h>
#include <lilv/lilv.h>
#include <lv2/atom/atom.h>
#include <lv2/urid/urid.h>
#include <string.h>

#ifdef MACOS
#define LV2_PATH "/opt/homebrew/lib/lv2/"
#else
#define LV2_PATH "/usr/local/lib/aarch64-linux-gnu/lv2"
#endif

typedef struct {
  LilvWorld *world;
  URITable uri_table;
} PluginsContext;

PluginsContext _ctx;

void plugins_ctx_init() {
  memset(&_ctx, 0, sizeof(PluginsContext));
  _ctx.world = lilv_world_new();
  LilvNode *lv2_path = lilv_new_string(_ctx.world, LV2_PATH);
  lilv_world_set_option(_ctx.world, LILV_OPTION_LV2_PATH, lv2_path);
  lilv_node_free(lv2_path);
  lilv_world_load_all(_ctx.world);

  uri_table_init(&_ctx.uri_table);
}

void plugins_ctx_release() { lilv_world_free(_ctx.world); }

const LilvPlugin *plugins_get_plugin(const char *uri) {
  const LilvPlugins *plugins = lilv_world_get_all_plugins(_ctx.world);
  LILV_FOREACH(plugins, i, plugins) {
    const LilvPlugin *p = lilv_plugins_get(plugins, i);
    if (strcmp(lilv_node_as_string(lilv_plugin_get_uri(p)), uri) == 0) {
      return p;
    }
  }
  return NULL;
}

void plugins_print_all() {
  const LilvPlugins *plugins = lilv_world_get_all_plugins(_ctx.world);
  LILV_FOREACH(plugins, i, plugins) {
    const LilvPlugin *p = lilv_plugins_get(plugins, i);
    plugins_print_info(p);
    printf("\n\n");
  }
}

void plugins_print_info(const LilvPlugin *p) {
  LilvNode *nodeName = lilv_plugin_get_name(p);
  const LilvPluginClass *nodeClass = lilv_plugin_get_class(p);

  const LilvNode *nodeClassLabel = lilv_plugin_class_get_label(nodeClass);
  uint32_t numPorts = lilv_plugin_get_num_ports(p);
  printf("plugin '%s' name = '%s' class '%s' ports=%i\n",
         lilv_node_as_string(lilv_plugin_get_uri(p)),
         lilv_node_as_string(nodeName), lilv_node_as_string(nodeClassLabel),
         numPorts);

  printf("Ports:\n");
  LilvNode *portConnectionOptional =
      lilv_new_uri(_ctx.world, LV2_CORE__connectionOptional);
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
    printf("\t port %i: %s %s %s name '%s' %s\n", i,
           (isAudio ? "audio" : "control"), (isInput ? "input" : "output"),
           (isAtom ? " (atom) " : ""), lilv_node_as_string(portName),
           (optional ? " Optional" : ""));
    lilv_node_free(portName);
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
