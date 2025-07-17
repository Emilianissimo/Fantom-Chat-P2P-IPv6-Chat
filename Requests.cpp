#include "Requests.h"
#include <QDebug>
#include <QHostInfo>


Requests::Requests(){
    curl = curl_easy_init();
    // Set certificate
    curl_version_info_data* vinfo = curl_version_info(CURLVERSION_NOW);

    if (vinfo && vinfo->ssl_version) {
        std::string sslBackend(vinfo->ssl_version);
        if (sslBackend.find("OpenSSL") != std::string::npos ||
            sslBackend.find("wolfSSL") != std::string::npos ||
            sslBackend.find("LibreSSL") != std::string::npos) {
            curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
        } else if (sslBackend.find("Schannel") != std::string::npos) {
            // Windows — Schannel no need CA
        } else if (sslBackend.find("SecureTransport") != std::string::npos) {
            // macOS — no need
        }
    }
}


Requests::~Requests(){
    if (curl) curl_easy_cleanup(curl);
}


QString Requests::get(char* url, bool ipv6mode){
    CURLcode response;
    std::string readBuffer;

    if(!curl){
        qDebug() << "There is not curl successfully inited";
        return "Error";
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    if (ipv6mode && hasIPv6(url)){
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
    }

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    response = curl_easy_perform(curl);

    if (response != CURLE_OK){
        qDebug() << "curl_easy_perform() failed:" << curl_easy_strerror(response);
        return "Error";
    }

    return QString::fromStdString(readBuffer);
}

bool Requests::hasIPv6(const QString &hostname){
    QHostInfo info = QHostInfo::fromName(hostname);
    for (const QHostAddress &addr : info.addresses()) {
        if (addr.protocol() == QAbstractSocket::IPv6Protocol)
            return true;
    }
    return false;
}
