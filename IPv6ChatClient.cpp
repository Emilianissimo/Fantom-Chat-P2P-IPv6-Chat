#include "IPv6ChatClient.h"
#include <QDebug>
#ifdef _WIN32
#include <winsock2.h>
#endif


IPv6ChatClient::IPv6ChatClient(QObject* parent) : QObject(parent) {}

void IPv6ChatClient::connectToPeer(const QString& address, int port) {
    // Ability to connect to the peer by its address and port
    // UPD: Remmeber, each client - one socket
    QString clientID = QString("%1:%2")
        .arg(address)
        .arg(port);
    if (connections.contains(clientID)){
        qDebug() << "Client: Already connected to peer" << clientID;
        return;
    }

    QTcpSocket* socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &IPv6ChatClient::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &IPv6ChatClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &IPv6ChatClient::onDisconnected);

    socket->connectToHost(QHostAddress(address), port);
    if (!socket->waitForConnected(3000)){
        qDebug() << "Client: couldn't connect to the server" << address << ":" << port;
        socket->deleteLater();
        return;
    }
    qDebug() << "Client: connected to the server: " << address << ":" << port;
    QMutexLocker locker(&connectionsMutex);
    connections.insert(clientID, {clientID, socket});
    // Emiting to return clientID outside the thread
    emit peerConnected(clientID);
}



void IPv6ChatClient::sendMessage(const QString& clientID, const QByteArray& message) {
    // Sending the message, with ID identifier in the message
    // TODO: use identifier on handshakes
    if (!connections.contains(clientID)) {
        qDebug() << "Client: No connection to peer" << clientID;
        return;
    }

    QByteArray composedMessage = clientID.toUtf8() + '\0' + message;

    // Protocol prefix for server to determine length
    QByteArray lengthPrefix;
    QDataStream stream(&lengthPrefix, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << static_cast<quint32>(composedMessage.size());

    composedMessage = lengthPrefix + composedMessage;

    QTcpSocket* socket = connections[clientID].socket;
    socket->write(composedMessage);
    if (socket->waitForBytesWritten(3000)){
        qDebug() << "Client: message sent: " << composedMessage;
        emit messageSent(clientID, composedMessage);
    }else{
        qDebug() << "Client: message to" << clientID << "not sent";
    }
}

void IPv6ChatClient::onReadyRead() {
    // Slot for getting incoming data from my server
    // TODO: may be deleted in case of useless activity. We do not need to store data on client side, only UI/DB
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = socket->readAll();
    qDebug() << "Client: Received: " + data;
}

void IPv6ChatClient::onConnected() {
    qDebug() << "Client: Connected to server";
}

void IPv6ChatClient::onDisconnected() {
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    QString deadPeer;
    for (auto it = connections.begin(); it != connections.end(); ++it) {
        if (it.value().socket == socket) {
            deadPeer = it.key();
            break;
        }
    }
    if (!deadPeer.isEmpty()) {
        QMutexLocker locker(&connectionsMutex);
        qDebug() << "Client: Disconnected from peer" << deadPeer;
        connections.remove(deadPeer);
    }
    socket->deleteLater();
}
