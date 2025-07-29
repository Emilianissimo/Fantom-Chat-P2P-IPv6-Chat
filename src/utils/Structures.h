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
    QString clientID;
    QString message;
    bool isIncoming;
};

struct Contact {
    QString chatID;
    QString clientID;
    QString lastMessage;
    int order;
    bool isActive;
};

Q_DECLARE_METATYPE(Message)
Q_DECLARE_METATYPE(Contact);
#endif
