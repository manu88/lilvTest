#pragma once
#include <lilv/lilv.h>

void plugins_ctx_init(void);
void plugins_ctx_release(void);

void plugins_print_info(const LilvPlugin *p);
void plugins_print_all(void);

const LilvPlugin *plugins_get_plugin(const char *uri);