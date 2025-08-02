#ifndef ICRYPTOERROR_H
#define ICRYPTOERROR_H

#include <QString>

class ICryptoError : public std::runtime_error {
public:
    explicit ICryptoError(const std::string& message)
        : std::runtime_error(message) {}
    virtual ~ICryptoError() = default;
    virtual QString message() const = 0;
};

#endif // ICRYPTOERROR_H
