#include "post.hpp"

std::string postRequest(const std::string & url, const std::string & data) {
CURL * curl = curl_easy_init();

    if(!curl) {
        throw std::runtime_error("curl init error");
    }
    std::string buffer;
    struct curl_slist * headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpCallback);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, & buffer);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

    curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    return buffer;
}

size_t HttpCallback(void * contents, size_t size, size_t number, std::string * result) {
    size_t totalSize = size * number;
    result->append((char *) contents, totalSize);
    return totalSize;
}

std::string getRequest(const std::string & url) {
    CURL * curl = curl_easy_init();

    if(!curl) {
        throw std::runtime_error("curl init error");
    }
    std::string buffer;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpCallback);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, & buffer);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    
    CURLcode res = curl_easy_perform(curl);
    if(res == CURLE_OPERATION_TIMEDOUT) {
        curl_easy_cleanup(curl);
        throw std::runtime_error("Request timed out!");
    }
    curl_easy_cleanup(curl);

    return buffer;
}