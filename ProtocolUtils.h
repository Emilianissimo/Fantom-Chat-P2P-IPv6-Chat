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

QString getLocalIPv6Address()
{
    const auto interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& iface : interfaces) {
        if (!(iface.flags() & QNetworkInterface::IsUp) ||
            !(iface.flags() & QNetworkInterface::IsRunning) ||
            (iface.flags() & QNetworkInterface::IsLoopBack)) {
            continue;
        }

        for (const QNetworkAddressEntry& entry : iface.addressEntries()) {
            const QHostAddress& ip = entry.ip();
            if (ip.protocol() == QAbstractSocket::IPv6Protocol &&
                !ip.isLoopback()) {

                if (ip.toString().startsWith("fe80"))
                    return ip.toString() + "%" + iface.name();

                return ip.toString();
            }
        }
    }

    return "";
}

#endif // PROTOCOLUTILS_H
