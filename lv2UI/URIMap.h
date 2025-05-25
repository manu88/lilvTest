#pragma once

#include <QHash>
#include <lilv/lilv.h>

class URIMap{
public:


    LV2_URID map(const char* uri){
        if(_uriHash.contains(uri)){
            return _uriHash[uri];
        }
        _uriHash[uri] = _hashIndex;
        return _hashIndex++;
    }

private:
    LV2_URID _hashIndex;
    QHash<QString, LV2_URID> _uriHash;
};
