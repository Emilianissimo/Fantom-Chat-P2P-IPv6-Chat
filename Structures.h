#pragma once

#include <QString>
#include <QTcpSocket>

struct PeerConnection {
    QString clientID;
    QTcpSocket* socket;
};
