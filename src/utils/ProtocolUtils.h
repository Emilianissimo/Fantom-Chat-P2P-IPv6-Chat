#ifndef PROTOCOLUTILS_H
#define PROTOCOLUTILS_H

#include "qendian.h"
#include <QByteArray>
#include <QNetworkInterface>

inline quint32 readUInt32(const QByteArray& bytes) {
    return qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(bytes.constData()));
}

inline QByteArray prependLength(const QByteArray& data) {
    QByteArray result;
    quint32 len = data.size();
    result.append(reinterpret_cast<const char*>(&len), 4);
    result.append(data);
    return result;
}

// Client and server from each peer has different ports (server is stable and client is dynamic)
// Also remove zone ID
inline QString stripPort(const QString& fullID) {
    QString addr = fullID.section(':', 0, -2);
    int zoneIndex = addr.indexOf('%');
    if (zoneIndex != -1) {
        addr = addr.left(zoneIndex);
    }
    return addr;
}

// Unique Identificator for chat list (contact list)
inline QString makeChatID(const QString& a, const QString& b){
    QString a_clean = a.section('%', 0, 0);
    QString b_clean = b.section('%', 0, 0);
    return (a_clean < b_clean) ? (a_clean + "__" + b_clean) : (b_clean + "__" + a_clean);
}

#endif // PROTOCOLUTILS_H
