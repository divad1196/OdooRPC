#ifndef ODOO_ODOORPC_H
#define ODOO_ODOORPC_H

#include "credentials.h"
#include <string>
#include <vector>

class OdooRPC {
    public:

        /**
         * @param url url of the Odoo server (must include the port)
         * @param database The database to which to connect
         * @param credentials User connection credentials
         * @brief Create the odoo rpc client
        */
        OdooRPC(
            const std::string& url,
            const std::string& database,
            const Credentials& credentials
        );

        /**
         * @param route Odoo model on which to query
         * @param http_method The http method to use
         * @param body the body of the request.
         * @brief Query Odoo server, Use it if you use a good json library, otherwise use raw_query
         * @return Response body as a strilng
        */
        std::string raw_jsonrpc(
            const std::string& route,
            const std::string& http_method,
            const std::string& body
        ) const;

        /**
         * @param model Odoo model on which to query
         * @param method Odoo model's method to use
         * @param arguments arguments to pass to the method (in the ordre of the python function)
         * @brief Query Odoo server
         * @return Response body as a strilng
        */
        std::string raw_query(
            const std::string& model,
            const std::string& method,
            const std::vector<std::string>& arguments
        );

        /**
         * @param uid id of the user in the database
         * @brief Manually set the user uid 
        */
        void forceUid(size_t uid);

        /**
         * @param uid id of the user in the database as a string
         * @brief Manually set the user uid 
        */
        void forceUid(const std::string& uid);

        /**
         * @brief Query Odoo to retrieve the user uid 
        */
        void autenticate();

    private:
        std::string _url;
        std::string _db;
        std::string _uid;
        Credentials _creds;

        static size_t _retrieve_data(void *buffer, size_t size, size_t nmemb, void* std_string);
};


#endif // ODOO_ODOORPC_H