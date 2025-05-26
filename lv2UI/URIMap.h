#pragma once

#include <QHash>
#include <lilv/lilv.h>

class URIMap{
public:
    LV2_URID map(const char* uri){
        if (_uriHash.contains(uri)) {
            return _uriHash[uri];
        }

        _uriHash[uri] = _hashIndex;
        _keyHash[_hashIndex] = uri;
        qDebug("map create hash %i for '%s'", _hashIndex, uri);
        return _hashIndex++;
    }

    const char *unMap(LV2_URID val) { return _keyHash[val]; }

private:
    LV2_URID _hashIndex;
    QHash<QString, LV2_URID> _uriHash;
    QHash<LV2_URID, const char *> _keyHash;
};
