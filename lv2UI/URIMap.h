#pragma once

#include <QHash>
#include <lilv/lilv.h>

class URIMap{
public:
    LV2_URID map(const QString &uri)
    {
        if (_uriHash.contains(uri)) {
            return _uriHash[uri];
        }

        _uriHash[uri] = _hashIndex;
        _keyHash[_hashIndex] = uri;
        return _hashIndex++;
    }

    QString unMap(LV2_URID val) { return _keyHash[val]; }

private:
    LV2_URID _hashIndex;
    QHash<QString, LV2_URID> _uriHash;
    QHash<LV2_URID, QString> _keyHash;
};
