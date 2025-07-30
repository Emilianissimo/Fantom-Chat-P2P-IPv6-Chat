#include "IPv6ChatClient.h"
#include <QDebug>
#ifdef Q_OS_WIN
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
    connect(socket, &QTcpSocket::connected, this, &IPv6ChatClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &IPv6ChatClient::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &IPv6ChatClient::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &IPv6ChatClient::onSocketError);

    {
        QMutexLocker locker(&connectionsMutex);
        connections.insert(clientID, {clientID, socket});
    }

    socket->connectToHost(QHostAddress(address), port);
}

QString IPv6ChatClient::findClientID(QTcpSocket* socket)
{
    QMutexLocker locker(&connectionsMutex);
    for (auto it = connections.begin(); it != connections.end(); ++it) {
        if (it->socket == socket) return it.key();
    }
    return {};
}

void IPv6ChatClient::onSocketError(QAbstractSocket::SocketError)
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString clientID = findClientID(socket);
    qDebug() << "Client: couldn't connect to the server" << clientID;

    QString deadPeer = findClientID(socket);

    if (!deadPeer.isEmpty()) {
        QMutexLocker locker(&connectionsMutex);
        qDebug() << "Client: Disconnected from peer" << deadPeer;
        connections.remove(deadPeer);
    }

    handshakeStatus.remove(socket);
    socket->deleteLater();
    return;
}

void IPv6ChatClient::onConnected() {
    qDebug() << "Client: Connected to server";

    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    // Handshake
    qDebug() << "Client: Sending handshake";
    socket->write("HANDSHAKE\n");
}

void IPv6ChatClient::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QString line = QString::fromUtf8(data).trimmed();

    if (line.startsWith("HANDSHAKE_ACK")) {
        qDebug() << "Client: Handshake complete with" << line.section(' ', 1);
        handshakeStatus[socket] = true;

        // Emiting to return clientID outside the thread
        emit peerConnected(findClientID(socket));
    } else {
        qDebug() << "Client: Unexpected response during handshake: " << line;
        socket->disconnectFromHost();
    }
    // Only handshake may be read here.
    disconnect(socket, &QTcpSocket::readyRead, this, &IPv6ChatClient::onReadyRead);
}

void IPv6ChatClient::onDisconnected() {
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    QString deadPeer = findClientID(socket);

    if (!deadPeer.isEmpty()) {
        QMutexLocker locker(&connectionsMutex);
        qDebug() << "Client: Disconnected from peer" << deadPeer;
        connections.remove(deadPeer);
    }

    handshakeStatus.remove(socket);
    socket->deleteLater();
    emit peerDisconnected(deadPeer);
}

void IPv6ChatClient::sendMessage(const QString& selfHost, const QString& clientID, const QByteArray& message) {
    // Sending the message, with ID identifier in the message
    // TODO: use identifier on handshakes
    if (!connections.contains(clientID)) {
        qDebug() << "Client: No connection to peer" << clientID;
        emit peerDisconnected(clientID);
        return;
    }

    QByteArray composedMessage = selfHost.toUtf8() + '\0' + message;

    // Protocol prefix for server to determine length
    QByteArray lengthPrefix;
    QDataStream stream(&lengthPrefix, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << static_cast<quint32>(composedMessage.size());

    composedMessage = lengthPrefix + composedMessage;

    QTcpSocket* socket = connections[clientID].socket;

    if (!handshakeStatus.value(socket, false)) {
        qDebug() << "Client: Can't send, handshake not complete for" << clientID;
        return;
    }

    socket->write(composedMessage);
    if (socket->waitForBytesWritten(3000)){
        qDebug() << "Client: message sent: " << composedMessage;
        emit messageSent(clientID, message);
    }else{
        qDebug() << "Client: message to" << clientID << "not sent";
    }
}

