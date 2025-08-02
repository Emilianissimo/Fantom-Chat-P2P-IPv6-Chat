#include "SodiumKeyPair.h"

SodiumKeyPair::SodiumKeyPair(const QByteArray& publicKey, const QByteArray& privateKey)
    : m_publicKey(publicKey), m_privateKey(privateKey) {}

QByteArray SodiumKeyPair::publicKey() const {
    return m_publicKey;
}

QByteArray SodiumKeyPair::privateKey() const {
    return m_privateKey;
}
