#ifndef SODIUMCRYPTOERROR_H
#define SODIUMCRYPTOERROR_H

#include "../../interfaces/ICryptoError.h"
#include <stdexcept>

class SodiumCryptoError : public std::runtime_error, public ICryptoError
{
public:
    explicit SodiumCryptoError(const std::string& message)
        : std::runtime_error(message) {}

    QString message() const override {
        return QString::fromStdString(what());
    }
};

#endif // SODIUMCRYPTOERROR_H
