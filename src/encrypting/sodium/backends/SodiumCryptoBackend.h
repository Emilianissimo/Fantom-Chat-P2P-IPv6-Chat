#pragma once

#ifndef SODIUMCRYPTOBACKEND_H
#define SODIUMCRYPTOBACKEND_H

#include "../../interfaces/ICryptoBackend.h"
#include <sodium.h>
#include <QCryptographicHash>

class SodiumCryptoBackend : public ICryptoBackend
{
public:
    ICryptoKeyPair* generateKeyPair() override;
    ICryptoSession* createSession(
        const ICryptoKeyPair& selfKey, const QByteArray& peerPublicKey
    ) override;
    std::shared_ptr<ICryptoBackend> clone() const override;
};

#endif // SODIUMCRYPTOBACKEND_H
