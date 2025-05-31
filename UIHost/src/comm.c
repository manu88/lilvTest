#include "comm.h"
#include "HostProtocol.h"
#include <stdio.h>
#include <sys/_types/_ssize_t.h>
#include <unistd.h>

int CommContextInit(CommContext *ctx, int fromHostFD, int toHostFD) {
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
  AppHostMsgFrame msgFrame;
  AppHostMsg_Hello helloMsg;
  msgFrame.header.msgSize = sizeof(AppHostMsg_Hello);
  msgFrame.header.type = AppHostMsgType_Hello;
  helloMsg.protocolVersion = HOST_PROTOCOL_VERSION;

  ssize_t nBytes = write(ctx->toHostFD, &msgFrame, sizeof(AppHostMsgFrame));
  if (nBytes != sizeof(AppHostMsgFrame)) {
    if (nBytes == -1) {
      perror("write msg frame");
      return 0;
    } else {
      printf("Only wrote %zi/%zi msg frame\n", nBytes, sizeof(AppHostMsgFrame));
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
}