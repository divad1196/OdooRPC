#include "odoorpc.h"
#include <curl/curl.h>
#include <string>
#include <cstring>


typedef size_t (*WriteCallback) (void*, size_t, size_t, void*);

std::string joinStrings(const std::vector<std::string>& string_list, const std::string& join);
size_t findUid(const std::string& authentication_values);

int _jsonrpc(
    const char* url,
    const char* method,
    const char* body,
    size_t body_size,
    WriteCallback write_call_back,
    void* call_back_data
);


OdooRPC::OdooRPC(
    const std::string& url,
    const std::string& database,
    const Credentials& credentials):    _url(url),
                                        _db(database),
                                        _creds(credentials) {

    autenticate();
}


void OdooRPC::forceUid(size_t uid) {
    _uid = std::to_string(uid);
    // TODO: check validity
}
void OdooRPC::forceUid(const std::string& uid) {
    forceUid(std::stoul(uid)); // ensure to have a number
}



std::string OdooRPC::raw_jsonrpc(
    const std::string& route,
    const std::string& http_method,
    const std::string& body
) const {
    std::string content;
    _jsonrpc(
        (_url + route).c_str(),
        http_method.c_str(),
        body.c_str(),
        body.size(),
        _retrieve_data,
        (void*)&content
    );
    return content;
}


std::string OdooRPC::raw_query(
    const std::string& model,
    const std::string& method,
    const std::vector<std::string>& arguments
) {

    const char BODY_TEMPLATE[] = R"({
        "jsonrpc": "2.0",
        "id": null,
        "method": "call",
        "params": {
            "service": "object",
            "method": "execute",
            "args": [
                "%s",
                "%s",
                "%s",
                "%s",
                "%s"
                %s
            ]
        }
    })";
    std::string arguments_string;
    for(const std::string& str: arguments) {
        arguments_string += "," + str;
    }

    size_t body_size =
        strlen(BODY_TEMPLATE)       +
        _db.size()                  +
        _uid.size()                 +
        _creds.getPassword().size() +
        model.size()                +
        method.size()               +
        arguments_string.size();

    char* body = new char[body_size];
    sprintf(
        body, BODY_TEMPLATE,
        _db.c_str(),
        _uid.c_str(),
        _creds.getPassword().c_str(),
        model.c_str(),
        method.c_str(),
        arguments_string.c_str()
    );

    std::string response_body = raw_jsonrpc(
        "/jsonrpc",
        "POST",
        body
    );

    delete[] body;

    return response_body;
}

void OdooRPC::autenticate() {

    const char BODY_TEMPLATE[] = R"({
        "jsonrpc": "2.0",
        "id": null,
        "method": "call",
        "params": {
            "db": "%s",
            "login": "%s",
            "password": "%s"
        }
    })";

    size_t body_size =
        strlen(BODY_TEMPLATE)       +
        _db.size()                  +
        _creds.getLogin().size()    +
        _creds.getPassword().size();


    char* body = new char[body_size];
    sprintf(
        body, BODY_TEMPLATE,
        _db.c_str(),
        _creds.getLogin().c_str(),
        _creds.getPassword().c_str()
    );

    std::string response_body = raw_jsonrpc(
        "/web/session/authenticate",
        "POST",
        body
    );

    forceUid(findUid(response_body));

    delete[] body;
}

size_t OdooRPC::_retrieve_data(void *buffer, size_t size, size_t nmemb, void* std_string) {
    static_cast<std::string*>(std_string)->append((char*)buffer, size * nmemb);
    return size * nmemb;
}



std::string joinStrings(const std::vector<std::string>& string_list, const std::string& join) {
    if(string_list.empty()) return "";

    std::string result = string_list[0];
    for(size_t index = 1; index < string_list.size(); ++index) {
        result += join + string_list[index];
    }
    return result;
}


size_t findUid(const std::string& authentication_values) {
    // Avoid using json lib,
    // Will be change if the library starts using json parser
    size_t start_index = authentication_values.find(R"("uid":)") + 6;
    return std::atol(authentication_values.c_str() + start_index);
}


int _jsonrpc(
    const char* url,
    const char* method,
    const char* body,
    size_t body_size,
    WriteCallback write_call_back,
    void* call_back_data
) {
    // Declarations
    CURLcode ret;
    CURL* hnd;
    struct curl_slist* slist1;
    slist1 = NULL;
    hnd = curl_easy_init();

    // Configurations
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");

    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, url);
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)body_size);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.58.0");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, method);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_call_back);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, call_back_data);
    curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1L);

    // Perform
    ret = curl_easy_perform(hnd);

    // Free
    curl_easy_cleanup(hnd);
    curl_slist_free_all(slist1);

    return (int)ret;
}