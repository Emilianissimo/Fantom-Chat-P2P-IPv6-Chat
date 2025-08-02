#ifndef SODIUMSESSION_H
#define SODIUMSESSION_H

#include "../../interfaces/ICryptoSession.h"

class SodiumSession : public ICryptoSession
{
public:
    SodiumSession(const QByteArray& txKey, const QByteArray& rxKey);

    QByteArray encrypt(const QByteArray& plainText) const override;
    QByteArray decrypt(const QByteArray& cipherText) const override;

private:
    QByteArray m_txKey;
    QByteArray m_rxKey;
};

#endif // SODIUMSESSION_H
