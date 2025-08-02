#ifndef ICRYPTOSESSION_H
#define ICRYPTOSESSION_H

#include <QByteArray>

class ICryptoSession {
public:
    virtual QByteArray encrypt(const QByteArray& plainText) const = 0;
    virtual QByteArray decrypt(const QByteArray& cipherText) const = 0;
    virtual ~ICryptoSession() {}
};

#endif // ICRYPTOSESSION_H
