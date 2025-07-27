#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QTcpSocket>

struct PeerConnection {
    QString clientID;
    QTcpSocket* socket;
};

struct Message {
    QString peerID;
    QString message;
    bool isIncoming;
};

Q_DECLARE_METATYPE(Message)
#endif
