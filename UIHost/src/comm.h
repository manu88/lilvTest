#pragma once

typedef struct {
  int fromHostFD;
  int toHostFD;
} CommContext;

int CommContextInit(CommContext *ctx, int fromHostFD, int toHostFD);
int CommContextIsValid(const CommContext *ctx);
int CommContextSendHello(CommContext *ctx);