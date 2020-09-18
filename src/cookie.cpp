#include "cookie.h"

CookieJar::CookieJar() {

}

void CookieJar::add(const std::string& key, const std::string& value) {
    _cookies[key] = value;
}

void CookieJar::add(const std::string& cookie) {
    add(split(cookie));
}

void CookieJar::add(const Cookie& cookie) {
    add(cookie.first, cookie.second);
}


Cookie CookieJar::split(const std::string& cookie) {
    size_t split_token = cookie.find('=');

    if(split_token == -1)
        return Cookie(cookie, "");

    std::string key(cookie, 0, split_token);
    std::string value(cookie, split_token + 1, cookie.size() - split_token);
}

std::string CookieJar::to_string(const Cookie& cookie) {
    return cookie.first + "=" + cookie.second;
}

CookieJar::operator std::string() {
    if(_cookies.size() == 0)
        return "";

    CookieMap::iterator it = _cookies.begin();
    std::string jar = "Cookie: " + to_string(*it);
    ++it;
    while(it != _cookies.end()) {
        jar += "; ";
        jar += to_string(*it);
        ++it;
    }
}

void CookieJar::from_header(const std::string& header) {
    size_t header_end_index = header.find("\n\n");
    if(header_end_index == std::string::npos)
        return;

    size_t start_index = header.find("Set-Cookie: ") + 12;
    size_t end_index;
    while(start_index < header_end_index) {
        end_index = header.find_first_of("\n;", start_index);
        add(header.substr(start_index, end_index));
        start_index = header.find("Set-Cookie: ", end_index) + 12;
    }
}