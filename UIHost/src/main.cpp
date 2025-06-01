#include "HostProtocol.h"
#include "comm.h"
#include "glib.h"
#include "osx_stuff.h"
#include "plugins.h"
#include "uri.h"
#include <assert.h>
#include <cstdint>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <lilv/lilv.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/ui/ui.h>
#include <lv2/urid/urid.h>
#include <map>
#include <serd/serd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifdef LINUX
// stub for linux, no need to do anything
void platformPostFix(void) {}
#endif

#ifdef MACOS
#define LV2_PATH "/opt/homebrew/lib/lv2/"
#else
#define LV2_PATH "/usr/local/lib/aarch64-linux-gnu/lv2"
#endif

static PluginsContext ctx;
static CommContext commCtx;

static std::map<uint32_t, std::string> _unmapStash;

static void onSuilPortWrite(SuilController controller, uint32_t port_index,
                            uint32_t buffer_size, uint32_t protocol,
                            void const *buffer);

static void OnDestroy(GtkWidget *pWidget, gpointer pData) { gtk_main_quit(); }

static void onAppMsg(const AppHostHeader *header, const void *data) {
  printf("Got complete message\n");
  switch (header->type) {
  case AppHostMsgType_Goodbye:
    printf("Got goodbye\n");
    gtk_main_quit();
  }
}

static LV2_URID rpcURI_Map(LV2_URID_Map_Handle handle, const char *uri) {
  LV2_URID ret = CommContext_MapRequest(&commCtx, uri);
  _unmapStash[ret] = uri;
  return ret;
}

static const char *rpcURI_UnMap(LV2_URID_Map_Handle handle, LV2_URID urid) {
  if (_unmapStash.count(urid)) {
    return _unmapStash[urid].c_str();
  }
  printf("rpcURI_UnMap request for %i\n", urid);
  return CommContext_UnmapRequest(&commCtx, urid);
}

static void onSuilPortWrite(SuilController controller, uint32_t portIndex,
                            uint32_t bufferSize, uint32_t protocol,
                            void const *buffer) {
  PluginsContext *ctx = (PluginsContext *)controller;
  CommContext_sendPortWrite(&commCtx, portIndex, bufferSize, protocol, buffer);
#if 0
  const char *protocolName = ctx->unMapFunction(&ctx->uri_table, protocol);
  printf("_suilPortWriteFunc on protocol %u '%s' port index %u\n", protocol,
         protocolName, portIndex);

  if (strcmp(protocolName, LV2_ATOM__eventTransfer) == 0) {
    printf("\tEvent transfer buffer size %u\n", bufferSize);

    const LV2_Atom_Object *obj = (const LV2_Atom_Object *)buffer;
    LV2_ATOM_OBJECT_FOREACH(obj, iter) {
      const char *typeURI =
          ctx->unMapFunction(&ctx->uri_table, iter->value.type);
      const char *keyURI = ctx->unMapFunction(&ctx->uri_table, iter->key);
      printf("Key %i '%s' type %s\n", iter->key, keyURI, typeURI);
      if (strcmp(keyURI, "http://lv2plug.in/plugins/eg-scope#ui-spp") == 0) {
        const LV2_Atom_Int *val = (const LV2_Atom_Int *)&iter->value;
        printf("val %i\n", val->body);
      } else if (strcmp(keyURI, "http://lv2plug.in/plugins/eg-scope#ui-amp") ==
                 0) {
        const LV2_Atom_Float *val = (const LV2_Atom_Float *)&iter->value;
        printf("val %f\n", val->body);
      }
    }
  }
#endif
}

int main(int argc, char **argv) {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  const char *pluginURI =
      argc > 1 ? argv[1] : "http://lv2plug.in/plugins/eg-scope#Stereo";

  int fromHostFD = (argc > 2) ? atoi(argv[2]) : -1;
  int toHostFD = (argc > 3) ? atoi(argv[3]) : -1;

  CommContextInit(&commCtx, fromHostFD, toHostFD);
  commCtx.onMsg = onAppMsg;

  // send hello msg
  if (!CommContextSendHello(&commCtx)) {
    printf("Error while sending Hello\n");
  }

  plugins_ctx_init(&ctx);
  ctx.unMapFunction = rpcURI_UnMap;
  ctx.portWriteFunction = onSuilPortWrite;
  const LilvPlugin *plug = plugins_get_plugin(&ctx, pluginURI);
  if (!plug) {
    perror("no such plugin");
    plugins_ctx_release(&ctx);
    return 1;
  }
  assert(plug);

  LilvNode *pluginNameNode = lilv_plugin_get_name(plug);
  const char *pluginName = lilv_node_as_string(pluginNameNode);

  int newArgC = 1;
  char **newArgv = {(char **)&pluginName};
  gtk_init(&newArgC, &newArgv);

  const LilvNode *binaryURINode = NULL;
  const LilvNode *bundleURINode = NULL;

  const LilvUI *ui = NULL;
  LILV_API LilvUIs *uis = lilv_plugin_get_uis(plug);
  if (uis == NULL) {
    printf("This plugin does not provide a UI\n");
    plugins_ctx_release(&ctx);
    return 2;
  }
  LILV_FOREACH(uis, i, uis) {
    ui = lilv_uis_get(uis, i);
    const LilvNodes *uiClasses = lilv_ui_get_classes(ui);

    LILV_FOREACH(nodes, iter, uiClasses) {
      const LilvNode *classNode = lilv_nodes_get(uiClasses, iter);
      printf("UI class '%s'\n", lilv_node_as_string(classNode));
    }
    binaryURINode = lilv_ui_get_binary_uri(ui);
    bundleURINode = lilv_ui_get_bundle_uri(ui);
    break;
  }
  free(uis);

  LV2_URID_Map mapHandle;
  mapHandle.map = rpcURI_Map;
  mapHandle.handle = &ctx;
  LV2_Feature feat;
  feat.URI = LV2_URID__map;
  feat.data = &mapHandle;
  const LV2_Feature *const features[2] = {&feat, NULL};

  g_application_new(pluginName, G_APPLICATION_DEFAULT_FLAGS);

  SuilInstance *uiInstance = plugins_CreateInstance(&ctx, plug, ui, features);
  assert(uiInstance);

  GtkWidget *plugWin = (GtkWidget *)suil_instance_get_widget(uiInstance);
  assert(plugWin);

  GtkWidget *pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(OnDestroy), NULL);
  gtk_window_set_position(GTK_WINDOW(pWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(pWindow), pluginURI);
  gtk_container_add(GTK_CONTAINER(pWindow), plugWin);
  gtk_widget_show_all(pWindow);

  platformPostFix();

  GSource *source = CommContextCreateSource(&commCtx);
  g_source_attach(source, NULL);

  gtk_main();
  free(pluginNameNode);

  printf("Freeing SUIL instance\n");
  // FIXME: crashes
  // suil_instance_free(uiInstance);
  printf("release plugins\n");
  plugins_ctx_release(&ctx);
  return 0;
}
