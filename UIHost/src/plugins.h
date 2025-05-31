#pragma once
#include "uri.h"
#include <lilv/lilv.h>
#include <suil/suil.h>

typedef struct {
  LilvWorld *world;
  URITable uri_table;
  SuilHost *host;
} PluginsContext;

void plugins_ctx_init(PluginsContext *ctx);
void plugins_ctx_release(PluginsContext *ctx);

void plugins_print_info(PluginsContext *ctx, const LilvPlugin *p);
void plugins_print_all(PluginsContext *ctx);

const LilvPlugin *plugins_get_plugin(PluginsContext *ctx, const char *uri);