#include "comm.h"
#include "HostProtocol.h"
#include "glib-unix.h"
#include "glib.h"
#include <assert.h>
#include <stdint.h>
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

uint32_t CommContext_MapRequest(CommContext *ctx, const char *uri) {
  AppHostHeader msgHeader;
  msgHeader.msgSize = strlen(uri) + 1; // include NULL byte
  assert(msgHeader.msgSize < HOST_PROTOCOL_MAX_MSG_SIZE);
  msgHeader.type = AppHostMsgType_URIDMapRequest;
  ssize_t nBytes = write(ctx->toHostFD, &msgHeader, sizeof(AppHostHeader));
  if (nBytes != sizeof(AppHostHeader)) {
    printf("Only send %zi bytes of header instead of %zi\n", nBytes,
           sizeof(AppHostHeader));
  }
  nBytes = write(ctx->toHostFD, uri, msgHeader.msgSize);
  if (nBytes != msgHeader.msgSize) {
    printf("Only send %zi bytes of data instead of %i \n", nBytes,
           msgHeader.msgSize);
  }

  // read reply
  msgHeader.msgSize = 0;
  msgHeader.type = 0;
  nBytes = read(ctx->fromHostFD, &msgHeader, sizeof(AppHostHeader));
  if (nBytes != sizeof(AppHostHeader)) {
    printf("Only read %zi bytes of header instead of %zi\n", nBytes,
           sizeof(AppHostHeader));
  }
  assert(msgHeader.type == AppHostMsgType_URIDMapReply);
  AppHostMsg_URIDMapReply reply;
  nBytes = read(ctx->fromHostFD, &reply, sizeof(AppHostMsg_URIDMapReply));
  if (nBytes != sizeof(AppHostMsg_URIDMapReply)) {
    printf("Only read %zi bytes of data instead of %zi\n", nBytes,
           sizeof(AppHostMsg_URIDMapReply));
  }
  return reply.urid;
}

char *CommContext_UnmapRequest(CommContext *ctx, uint32_t urid) {
  AppHostHeader msgHeader;
  msgHeader.msgSize = sizeof(AppHostMsg_URIDUnMapRequest);
  msgHeader.type = AppHostMsgType_URIDUnMapRequest;
  ssize_t nBytes = write(ctx->toHostFD, &msgHeader, sizeof(AppHostHeader));
  if (nBytes != sizeof(AppHostHeader)) {
    if (nBytes == -1) {
      perror("rpcURI_UnMap.header");
    }
    printf("Only wrote %zi bytes of header instead of %zi\n", nBytes,
           sizeof(AppHostHeader));
  }
  AppHostMsg_URIDUnMapRequest msg;
  msg.urid = urid;
  nBytes = write(ctx->toHostFD, &msg, sizeof(AppHostMsg_URIDUnMapRequest));
  if (nBytes != sizeof(AppHostMsg_URIDUnMapRequest)) {
    if (nBytes == -1) {
      perror("rpcURI_UnMap.msg");
    }
    printf("Only wrote %zi bytes of data instead of %zi\n", nBytes,
           sizeof(AppHostMsg_URIDUnMapRequest));
  }
  // read reply
  msgHeader.msgSize = 0;
  msgHeader.type = 0;
  nBytes = read(ctx->fromHostFD, &msgHeader, sizeof(AppHostHeader));
  if (nBytes != sizeof(AppHostHeader)) {
    if (nBytes == -1) {
      perror("rpcUnURIMap.header");
    }
    printf("Only read %zi bytes of header instead of %zi\n", nBytes,
           sizeof(AppHostHeader));
  }
  assert(msgHeader.type == AppHostMsgType_URIDUnMapReply);
  assert(msgHeader.msgSize < HOST_PROTOCOL_MAX_MSG_SIZE);
  AppHostMsg_URIDUnMapReply reply;
  nBytes = read(ctx->fromHostFD, &reply, sizeof(AppHostMsg_URIDUnMapReply));
  if (nBytes != sizeof(AppHostMsg_URIDUnMapReply)) {
    if (nBytes == -1) {
      perror("rpcUnURIMap.body");
    }
    printf("Only read %zi bytes of data instead of %zi\n", nBytes,
           sizeof(AppHostMsg_URIDUnMapReply));
  }
  return strdup(reply.uri); // FIXME: leak!
}

void CommContext_sendPortWrite(CommContext *ctx, uint32_t portIndex,
                               uint32_t bufferSize, uint32_t protocol,
                               void const *buffer) {
  AppHostHeader msgHeader;
  msgHeader.msgSize = sizeof(AppHostMsg_PortWriteRequest) + bufferSize;
  msgHeader.type = AppHostMsgType_PortWriteRequest;
  assert(msgHeader.msgSize < HOST_PROTOCOL_MAX_MSG_SIZE);

  ssize_t nBytes = write(ctx->toHostFD, &msgHeader, sizeof(AppHostHeader));
  if (nBytes != sizeof(AppHostHeader)) {
    if (nBytes == -1) {
      perror("CommContext_sendPortWrite.header");
    }
    printf("Only wrote %zi bytes of data instead of %zi\n", nBytes,
           sizeof(AppHostHeader));
  }

  AppHostMsg_PortWriteRequest msg;
  msg.portIndex = portIndex;
  msg.bufferSize = bufferSize;
  msg.protocol = protocol;
  nBytes = write(ctx->toHostFD, &msg, sizeof(AppHostMsg_PortWriteRequest));
  if (nBytes != sizeof(AppHostMsg_PortWriteRequest)) {
    if (nBytes == -1) {
      perror("CommContext_sendPortWrite.msg");
    }
    printf("Only wrote %zi bytes of data instead of %zi\n", nBytes,
           sizeof(AppHostMsg_PortWriteRequest));
  }

  nBytes = write(ctx->toHostFD, buffer, bufferSize);
  if (nBytes != bufferSize) {
    if (nBytes == -1) {
      perror("CommContext_sendPortWrite.buffer");
    }
    printf("Only wrote %zi bytes of data instead of %u\n", nBytes, bufferSize);
  }
}