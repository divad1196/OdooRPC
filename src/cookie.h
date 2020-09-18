#ifndef ODOO_COOKIE_H
#define ODOO_COOKIE_H

#include <string>
#include <vector>
#include <map>

using Cookie = std::pair<std::string, std::string>;

class CookieJar {
    using CookieMap = std::map<std::string, std::string>;

    public:
        CookieJar();

        /**
         * @param key
         * @param value
        */
        void add(const std::string& key, const std::string& value);


        /**
         * @param cookie
         * @brief split the string as key=value format and add it to the jar
        */
        void add(const std::string& cookie);


        void add(const Cookie& cookie);


        static Cookie split(const std::string& cookie);
        static std::string to_string(const Cookie& cookie);

        operator std::string();

        void from_header(const std::string& header);


    private:
        CookieMap _cookies;
};

#endif // ODOO_COOKIE_H