//
// Created by dj on 1/14/26.
//

#ifndef CMAKESFMLPROJECT_CLIENT_H
#define CMAKESFMLPROJECT_CLIENT_H
#include <cstdlib>
#include <string>

class Client {
    public:
    Client();
    ~Client();
    private:
    static std::string _getUsername();

};


#endif //CMAKESFMLPROJECT_CLIENT_H