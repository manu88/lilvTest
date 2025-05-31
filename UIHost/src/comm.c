#include "comm.h"
#include "glib-unix.h"
#include "glib.h"
#include <stdio.h>
#include <string.h>
#include <sys/_types/_ssize_t.h>
#include <unistd.h>

int CommContextInit(CommContext *ctx, int fromHostFD, int toHostFD) {
  memset(ctx->buffer, 0, HOST_PROTOCOL_MAX_MSG_SIZE);
  ctx->bufferIndex = 0;
  ctx->fromHostFD = fromHostFD;
  ctx->toHostFD = toHostFD;
  return 1;
}

int CommContextIsValid(const CommContext *ctx) {
  return ctx->fromHostFD != -1 && ctx->toHostFD != -1;
}

int CommContextSendHello(CommContext *ctx) {
  if (!CommContextIsValid(ctx)) {
    return 0;
  }
  AppHostHeader msgHeader;
  msgHeader.msgSize = sizeof(AppHostMsg_Hello);
  msgHeader.type = AppHostMsgType_Hello;
  AppHostMsg_Hello helloMsg;
  helloMsg.protocolVersion = HOST_PROTOCOL_VERSION;

  ssize_t nBytes = write(ctx->toHostFD, &msgHeader, sizeof(AppHostHeader));
  if (nBytes != sizeof(AppHostHeader)) {
    if (nBytes == -1) {
      perror("write msg frame");
      return 0;
    } else {
      printf("Only wrote %zi/%zi msg frame\n", nBytes, sizeof(AppHostHeader));
      return 0;
    }
  }
  nBytes = write(ctx->toHostFD, &helloMsg, sizeof(AppHostMsg_Hello));
  if (nBytes != sizeof(AppHostMsg_Hello)) {
    if (nBytes == -1) {
      perror("write msg body");
      return 0;
    } else {
      printf("Only wrote %zi/%zi msg body\n", nBytes, sizeof(AppHostMsg_Hello));
      return 0;
    }
  }
  return 1;
}

static gboolean onData(gint fd, GIOCondition condition, gpointer userData) {
  CommContext *ctx = (CommContext *)userData;
  AppHostHeader header;
  ssize_t nBytes = read(ctx->fromHostFD, &header, sizeof(AppHostHeader));

  if (nBytes == sizeof(AppHostHeader)) {
    printf("Got a frame: type %i size %i\n", header.type, header.msgSize);
    nBytes = read(ctx->fromHostFD, ctx->buffer, header.msgSize);
    if (nBytes == header.msgSize) {
      ctx->onMsg(&header, ctx->buffer);
    }
  } else {
    printf("read %zi bytes header from App\n", nBytes);
  }
  return TRUE;
}

GSource *CommContextCreateSource(CommContext *ctx) {
  printf("Create GSource\n");
  ctx->appSource = g_unix_fd_source_new(ctx->fromHostFD, G_IO_IN);
  g_source_set_callback(ctx->appSource, (GSourceFunc)onData, ctx, NULL);

  return ctx->appSource;
}