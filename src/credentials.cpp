#include "credentials.h"


Credentials::Credentials(
    const std::string& login,
    const std::string& password):
                                    _login(login),
                                    _password(password) {

}

std::string Credentials::getLogin() const {
    return _login;
}

std::string Credentials::getPassword() const {
    return _password;
}

void Credentials::setLogin(const std::string& login) {
    _login = login;
}

void Credentials::setPassword(const std::string& password) {
    _password = password;
}