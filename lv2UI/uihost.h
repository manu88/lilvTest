#ifndef UIHOST_H
#define UIHOST_H

#include <suil/suil.h>

class UIHost
{
public:
    UIHost();
    ~UIHost();

private:
    SuilHost* _host;
};

#endif // UIHOST_H
