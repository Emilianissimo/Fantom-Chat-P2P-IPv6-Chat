#ifndef SODIUMKEYPAIR_H
#define SODIUMKEYPAIR_H

#include "../../interfaces/ICryptoKeyPair.h"

class SodiumKeyPair : public ICryptoKeyPair
{
public:
    SodiumKeyPair(const QByteArray& publicKey, const QByteArray& privateKey);

    QByteArray publicKey() const override;
    QByteArray privateKey() const override;

private:
    QByteArray m_publicKey;
    QByteArray m_privateKey;
};

#endif // SODIUMKEYPAIR_H
