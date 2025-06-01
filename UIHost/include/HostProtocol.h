#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define HOST_PROTOCOL_MAX_MSG_SIZE 100

#define HOST_PROTOCOL_VERSION 1

typedef enum {

  AppHostMsgType_Unknown = 0,
  AppHostMsgType_Hello = 1,
  AppHostMsgType_Goodbye = 1,

  AppHostMsgType_URIDMapRequest = 2,
  AppHostMsgType_URIDMapReply = 3,

  AppHostMsgType_URIDUnMapRequest = 4,
  AppHostMsgType_URIDUnMapReply = 5,

  AppHostMsgType_PortWriteRequest = 6,

} AppHostMsgType;

typedef struct {
  uint32_t msgSize;
  uint32_t type;
  // msg is just after will be a AppHostMsg_* struct, depending on
  // AppHostHeader.type
} AppHostHeader;

typedef struct {
  uint32_t protocolVersion;
} AppHostMsg_Hello;

typedef struct {
  uint32_t unused; // avoid size difference between C and C++
} AppHostMsg_Goodbye;

typedef struct {
  char uri[HOST_PROTOCOL_MAX_MSG_SIZE];
} AppHostMsg_URIDMapRequest;

typedef struct {
  uint32_t urid;
} AppHostMsg_URIDMapReply;

typedef struct {
  uint32_t urid;
} AppHostMsg_URIDUnMapRequest;

typedef struct {
  char uri[HOST_PROTOCOL_MAX_MSG_SIZE];
} AppHostMsg_URIDUnMapReply;

typedef struct {
  uint32_t portIndex;
  uint32_t bufferSize;
  uint32_t protocol;
  // void const *buffer
} AppHostMsg_PortWriteRequest;

#ifdef __cplusplus
}
#endif
