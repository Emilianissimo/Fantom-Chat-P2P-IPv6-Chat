#ifndef SODIUMCRYPTOERROR_H
#define SODIUMCRYPTOERROR_H

#include "../../interfaces/ICryptoError.h"

class SodiumCryptoError : public ICryptoError
{
public:
    explicit SodiumCryptoError(const std::string& message)
        : ICryptoError(message) {}

    QString message() const override {
        return QString::fromStdString(what());
    }
};

#endif // SODIUMCRYPTOERROR_H
