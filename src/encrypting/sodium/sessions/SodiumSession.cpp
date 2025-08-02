#include "SodiumSession.h"
#include "../errors/SodiumCryptoError.h"
#include <sodium.h>

SodiumSession::SodiumSession(const QByteArray& txKey, const QByteArray& rxKey)
    : m_txKey(txKey), m_rxKey(rxKey) {}

QByteArray SodiumSession::encrypt(const QByteArray& plainText) const
{
    QByteArray nonce(crypto_secretbox_NONCEBYTES, Qt::Uninitialized);
    randombytes_buf(nonce.data(), crypto_secretbox_NONCEBYTES);

    QByteArray cipherText(crypto_secretbox_MACBYTES + plainText.size(), Qt::Uninitialized);

    if (
        crypto_secretbox_easy(
            reinterpret_cast<unsigned char*>(cipherText.data()),
            reinterpret_cast<const unsigned char*>(plainText.constData()), plainText.size(),
            reinterpret_cast<const unsigned char*>(nonce.constData()),
            reinterpret_cast<const unsigned char*>(m_txKey.constData())) != 0
        ){
        throw SodiumCryptoError("Encryption failed");
    }

    return nonce + cipherText;
}

QByteArray SodiumSession::decrypt(const QByteArray& cipherText) const
{
    if (cipherText.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
        throw SodiumCryptoError("Ciphertext too short to cointain nonce and MAC");
    }


    QByteArray nonce = cipherText.left(crypto_secretbox_NONCEBYTES);
    QByteArray encrypted = cipherText.mid(crypto_secretbox_NONCEBYTES);

    QByteArray plainText(encrypted.size() - crypto_secretbox_MACBYTES, Qt::Uninitialized);

    if (crypto_secretbox_open_easy(
            reinterpret_cast<unsigned char*>(plainText.data()),
            reinterpret_cast<const unsigned char*>(encrypted.constData()), encrypted.size(),
            reinterpret_cast<const unsigned char*>(nonce.constData()),
            reinterpret_cast<const unsigned char*>(m_rxKey.constData())) != 0) {
        throw SodiumCryptoError("Decryption failed: MAC check failed");
    }

    return plainText;
}
