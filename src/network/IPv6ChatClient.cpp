#include "IPv6ChatClient.h"
#include "../encrypting/interfaces/ICryptoError.h"
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

    socket->setProperty("clientID", clientID);
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

    QString clientID = socket->property("clientID").toString();
    qDebug() << "Client: couldn't connect to the server" << clientID;

    if (!clientID.isEmpty()) {
        QMutexLocker locker(&connectionsMutex);
        qDebug() << "Client: Disconnected from peer" << clientID;
        connections.remove(clientID);
    }

    handshakeStatus.remove(socket);
    socket->deleteLater();
    return;
}

void IPv6ChatClient::onConnected() {
    qDebug() << "Client: Connected to server";

    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString clientID = QString("%1:%2")
                           .arg(socket->peerAddress().toString())
                           .arg(socket->peerPort());

    {
        QMutexLocker locker(&connectionsMutex);
        connections.insert(clientID, {clientID, socket});
    }

    // Handshake
    qDebug() << "Client: Sending handshake";

    // Generate pair for this connection
    auto keyPair = cryptoBackend->generateKeyPair();
    clientKeys.insert(socket, keyPair);

    // Encode public key to Base64
    QString publicKeyBase64 = clientKeys[socket]->publicKey().toBase64();

    // Send handshake message with public key
    QString handashakeMessage = QString("HANDSHAKE %1\n").arg(publicKeyBase64);

    socket->write(handashakeMessage.toUtf8());
    qDebug() << "Client: Sent handshake with public key";
}

void IPv6ChatClient::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QString line = QString::fromUtf8(data).trimmed();

    try{
        if (line.startsWith("HANDSHAKE_ACK ")) {
            // Retrieving key and decoding it
            QString peerPublicKeyBase64 = line.section(' ', 1);
            QByteArray peerPublicKey = QByteArray::fromBase64(peerPublicKeyBase64.toUtf8());

            auto keyPair = clientKeys[socket];
            auto session = cryptoBackend->createSession(*keyPair, peerPublicKey);
            sessions.insert(socket, session);

            qDebug() << "Client: Handshake complete. Session keys established.";
            handshakeStatus[socket] = true;

            // Emiting to return clientID outside the thread
            emit peerConnected(findClientID(socket));
        } else {
            qDebug() << "Client: Unexpected response during handshake.";
            socket->disconnectFromHost();
        }
    } catch (const ICryptoError& ex) {
        qWarning() << "Client: Failed to perform handshake: " << ex.message();
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
    delete clientKeys[socket];
    clientKeys.remove(socket);
    delete sessions[socket];
    sessions.remove(socket);
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

    QTcpSocket* socket = connections[clientID].socket;
    if (!handshakeStatus.value(socket, false)) {
        qDebug() << "Client: Can't send, handshake not complete for" << clientID;
        return;
    }

    auto session = sessions.value(socket, nullptr);
    if (!session){
        qDebug() << "Client: No crypto session found for" << clientID;
    }

    QByteArray encryptedMessage;
    try{
        encryptedMessage = session->encrypt(message);
    } catch (const ICryptoError& ex){
        qWarning() << "Client: cannot encrypt message: " << ex.message();
        return;
    }

    qDebug() << "Client: Encrypted message: " << encryptedMessage;

    QByteArray composedMessage = selfHost.toUtf8() + '\0' + encryptedMessage;

    // Protocol prefix for server to determine length
    QByteArray lengthPrefix;
    QDataStream stream(&lengthPrefix, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << static_cast<quint32>(composedMessage.size());

    composedMessage = lengthPrefix + composedMessage;

    socket->write(composedMessage);
    if (socket->waitForBytesWritten(3000)){
        qDebug() << "Client: message sent: " << composedMessage;
        emit messageSent(clientID, message);
    }else{
        qDebug() << "Client: message to" << clientID << "not sent";
    }
}

