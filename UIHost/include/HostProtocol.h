#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define HOST_PROTOCOL_VERSION 1

typedef enum {

  AppHostMsgType_Unknown = 0,
  AppHostMsgType_Hello = 1,
  AppHostMsgType_Goodbye = 1,
} AppHostMsgType;

typedef struct {
  uint32_t msgSize;
  uint32_t type;
} AppHostHeader;

typedef struct {
  AppHostHeader header;
  // msg is just after will be a AppHostMsg_* struct, depending on
  // AppHostHeader.type
} AppHostMsgFrame;

typedef struct {
  uint32_t protocolVersion;
} AppHostMsg_Hello;

typedef struct {
    uint32_t unused; // avoid size difference between C and C++
} AppHostMsg_Goodbye;

#ifdef __cplusplus
}
#endif
