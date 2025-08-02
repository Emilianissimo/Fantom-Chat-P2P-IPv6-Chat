#ifndef ICRYPTOERROR_H
#define ICRYPTOERROR_H

#include <QString>

class ICryptoError{
public:
    virtual ~ICryptoError() = default;
    virtual QString message() const = 0;
};

#endif // ICRYPTOERROR_H
