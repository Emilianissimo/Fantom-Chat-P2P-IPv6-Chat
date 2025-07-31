#ifndef REQUESTS_H
#define REQUESTS_H

#include <string>
#include <QString>
#include <curl/curl.h>

class Requests{
public:
    Requests();
    ~Requests();
    QString writeCertToTemp();
    QString get(char* url, bool ipv6mode = true);

private:
    CURL *curl;

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    bool hasIPv6(const QString &hostname);
};

#endif // REQUESTS_H
