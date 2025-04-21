#include "IPv6ChatClient.h"
#include <QDebug>

IPv6ChatClient::IPv6ChatClient(const QString& serverAddress, int port, QObject* parent): QObject(parent) {
    connect(&socket, &QTcpSocket::readyRead, this, &IPv6ChatClient::onReadyRead);
    connect(&socket, &QTcpSocket::connected, this, &IPv6ChatClient::onConnected);
    connect(&socket, &QTcpSocket::disconnected, this, &IPv6ChatClient::onDisconnected);

    socket.connectToHost(serverAddress, port);
    if (socket.waitForConnected(10)){
        qDebug() << "Client: connected to the server: " << serverAddress << ":" << port;
    } else {
        qDebug() << "Client couldn't connect to the server" << serverAddress << ":" << port;
    }
}

void IPv6ChatClient::sendMessage(const QString& senderID, const QByteArray& message) {
    QByteArray composedMessage = senderID.toUtf8()+ ": " + message;
    socket.write(composedMessage);
    if (socket.waitForBytesWritten(10)){
        qDebug() << "Client: message sent: " << composedMessage;
    }else{
        qDebug() << "Client: message is not sent";
    }
}

void IPv6ChatClient::onReadyRead() {
    QByteArray data = socket.readAll();
    qDebug() << "Client: Received: " + data;
}

void IPv6ChatClient::onConnected() {
    qDebug() << "Client: Connected to server";
}

void IPv6ChatClient::onDisconnected() {
    qDebug() << "Client: Disconnected from server";
}
