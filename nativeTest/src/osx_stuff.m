#include "osx_stuff.h"
#import <AppKit/AppKit.h>

#ifdef MACOS
void platformPostFix(void) { [NSApp activateIgnoringOtherApps:YES]; }
#endif