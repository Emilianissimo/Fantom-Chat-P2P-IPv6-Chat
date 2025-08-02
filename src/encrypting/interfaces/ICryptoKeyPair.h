#ifndef ICRYPTOKEYPAIR_H
#define ICRYPTOKEYPAIR_H

#include <QByteArray>

class ICryptoKeyPair {
public:
    virtual QByteArray publicKey() const = 0;
    virtual QByteArray privateKey() const = 0;
    virtual ~ICryptoKeyPair() {}
};

#endif // ICRYPTOKEYPAIR_H
