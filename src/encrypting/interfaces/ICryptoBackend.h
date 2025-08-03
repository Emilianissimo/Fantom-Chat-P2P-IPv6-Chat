#ifndef ICRYPTOBACKEND_H
#define ICRYPTOBACKEND_H

#include "ICryptoKeyPair.h"
#include "ICryptoSession.h"

class ICryptoBackend {
public:
    virtual ICryptoKeyPair* generateKeyPair() = 0;
    virtual ICryptoSession* createSession(
        const ICryptoKeyPair& selfKey, const QByteArray& peerPublicKey
    ) = 0;
    virtual std::shared_ptr<ICryptoBackend> clone() const = 0;
    virtual ~ICryptoBackend() {}
};

#endif // ICRYPTOBACKEND_H
