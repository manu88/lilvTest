//
//  uri.h
//  lilvTest
//
//  Created by Manuel Deneu on 23/05/2025.
//

#pragma once
#include <lv2/urid/urid.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char** uris;
  size_t n_uris;
} URITable;

static void
uri_table_init(URITable* table)
{
  table->uris   = NULL;
  table->n_uris = 0;
}

static void
uri_table_destroy(URITable* table)
{
  for (size_t i = 0; i < table->n_uris; ++i) {
    free(table->uris[i]);
  }

  free(table->uris);
}

static LV2_URID
uri_table_map(LV2_URID_Map_Handle handle, const char* uri)
{
  URITable* table = (URITable*)handle;
  for (size_t i = 0; i < table->n_uris; ++i) {
    if (!strcmp(table->uris[i], uri)) {
      return i + 1;
    }
  }

  const size_t len = strlen(uri);
  table->uris = (char**)realloc(table->uris, ++table->n_uris * sizeof(char*));
  table->uris[table->n_uris - 1] = (char*)malloc(len + 1);
  memcpy(table->uris[table->n_uris - 1], uri, len + 1);
  printf("URI MAP: add '%s' %zu\n", uri ,table->n_uris);
  return table->n_uris;
}

static const char*
uri_table_unmap(LV2_URID_Map_Handle handle, LV2_URID urid)
{
  URITable* table = (URITable*)handle;
  if (urid > 0 && urid <= table->n_uris) {
    return table->uris[urid - 1];
  }
  return NULL;
}

static void uri_table_print(LV2_URID_Map_Handle handle){
  URITable* table = (URITable*)handle;
  for(size_t i=0; i< table->n_uris; i++){
    printf("%zi: '%s'\n", i, table->uris[i]);
  }
}
