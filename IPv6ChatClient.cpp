#include "IPv6ChatClient.h"
#include <QDebug>
#include <winsock2.h>


IPv6ChatClient::IPv6ChatClient(QObject* parent) : QObject(parent) {}

void IPv6ChatClient::connectToPeer(const QString& address, int port) {
    // Ability to connect to the peer by its address and port
    // UPD: Remmeber, each client - one socket
    QString peerID = QString("%1:%2")
        .arg(address)
        .arg(port);
    if (connections.contains(peerID)){
        qDebug() << "Client: Already connected to peer" << peerID;
        return;
    }

    QTcpSocket* socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &IPv6ChatClient::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &IPv6ChatClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &IPv6ChatClient::onDisconnected);

    socket->connectToHost(address, port);
    if (socket->waitForConnected(3000)){
        qDebug() << "Client: connected to the server: " << address << ":" << port;
        QMutexLocker locker(&connectionsMutex);
        connections.insert(peerID, {socket, peerID});
    } else {
        qDebug() << "Client: couldn't connect to the server" << address << ":" << port;
        socket->deleteLater();
    }
}

void IPv6ChatClient::sendMessage(const QString& peerID, const QByteArray& message) {
    // Sending the message, with ID identifier in the message
    // TODO: use identifier on handshakes
    if (!connections.contains(peerID)) {
        qDebug() << "Client: No connection to peer" << peerID;
        return;
    }

    QByteArray composedMessage = peerID.toUtf8()+ ": " + message;

    QTcpSocket* socket = connections[peerID].socket;
    socket->write(composedMessage);
    if (socket->waitForBytesWritten(3000)){
        qDebug() << "Client: message sent: " << composedMessage;
    }else{
        qDebug() << "Client: message to" << peerID << "not sent";
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
