#include "Requests.h"
#include <QDebug>
#include <QHostInfo>


QString Requests::get(char* url, bool ipv6mode){
    CURL *curl;
    CURLcode response;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");

        if (ipv6mode && hasIPv6(url)){
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
        }

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        response = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (response != CURLE_OK){
            qDebug() << "curl_easy_perform() failed:" << curl_easy_strerror(response);
            return "Error";
        }

        return QString::fromStdString(readBuffer);
    }
    qDebug() << "There is not curl successfully inited";
    return "Error";
}

bool Requests::hasIPv6(const QString &hostname){
    QHostInfo info = QHostInfo::fromName(hostname);
    for (const QHostAddress &addr : info.addresses()) {
        if (addr.protocol() == QAbstractSocket::IPv6Protocol)
            return true;
    }
    return false;
}
