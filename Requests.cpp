#include "Requests.h"
#include <QDebug>


QString Requests::get(char* url, bool ipv6mode){
    CURL *curl;
    CURLcode response;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (ipv6mode){
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
        }

        response = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (response != CURLE_OK){
            qWarning() << "curl_easy_perform() failed:" << curl_easy_strerror(response);
            return "Error";
        }

        return QString::fromStdString(readBuffer);
    }
    qWarning() << "There is not curl successfully inited";
    return "Error";
}
