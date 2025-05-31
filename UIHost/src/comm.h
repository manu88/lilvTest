#pragma once
#include <glib.h>

typedef struct {
  int fromHostFD;
  int toHostFD;
  GSource *appSource;
} CommContext;

int CommContextInit(CommContext *ctx, int fromHostFD, int toHostFD);
int CommContextIsValid(const CommContext *ctx);
int CommContextSendHello(CommContext *ctx);

GSource *CommContextCreateSource(CommContext *ctx);