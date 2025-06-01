#pragma once
#include "HostProtocol.h"
#include <glib.h>
#include <stdint.h>

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

uint32_t CommContext_MapRequest(CommContext *ctx, const char *uri);
char *CommContext_UnmapRequest(CommContext *ctx, uint32_t urid);

void CommContext_sendPortWrite(CommContext *ctx, uint32_t port_index,
                               uint32_t buffer_size, uint32_t protocol,
                               void const *buffer);