#pragma once
#include "HostProtocol.h"
#include <glib.h>

typedef void (*OnMsg)(const AppHostHeader *header, const void *data);

typedef struct {

  char buffer[HOST_PROTOCOL_MAX_MSG_SIZE];
  int bufferIndex;
  int fromHostFD;
  int toHostFD;
  GSource *appSource; // uses fromHostFD

  OnMsg onMsg;
} CommContext;

int CommContextInit(CommContext *ctx, int fromHostFD, int toHostFD);
int CommContextIsValid(const CommContext *ctx);
int CommContextSendHello(CommContext *ctx);

GSource *CommContextCreateSource(CommContext *ctx);