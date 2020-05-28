#ifndef ODOO_CREDENTIALS_H
#define ODOO_CREDENTIALS_H

#include <string>


class Credentials {
    public:

        /**
         * @param login
         * @param password
        */
        Credentials(const std::string& login, const std::string& password);

        /**
         * @return the login value
        */
        std::string getLogin() const;

        /**
         * @return the clear password value
        */
        std::string getPassword() const;

        /**
         * @brief set the login value
        */
        void setLogin(const std::string& login);

        /**
         * @brief set the password value, it will be stored in clear
        */
        void setPassword(const std::string& password);

    private:
        std::string _login;
        std::string _password;
};

#endif // ODOO_CREDENTIALS_H