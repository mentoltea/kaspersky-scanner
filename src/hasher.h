#ifndef HASHER_H
#define HASHER_H

#ifndef PREFIX
    #define PREFIX
#endif

#include "logger.h"

#include <windows.h>
#include <wincrypt.h>

#include <string>
#include <sstream>
#include <exception>
#include <vector>
#include <iostream>


class MD5Hasher {
    public:
    MD5Hasher() ;
    ~MD5Hasher() ;
    
    MD5Hasher(const MD5Hasher& other) = delete;
    MD5Hasher& operator=(const MD5Hasher& other) = delete;

    void Hash(const void *data, const size_t size);

    std::vector< BYTE > GetHashBytes() const ;
    std::string GetHashString() const ;


    private:
    // Абстрагирование контекста для будущей возможности введения кросс-платформенности 
    struct Context {
        HCRYPTPROV hProv; // CryptAcquireContext & CryptReleaseContext
        HCRYPTHASH hHash; // CryptCreateHash & CryptDestroyHash
    };
    
    Context ctx;
};

class HasherCSPException: public std::exception {};
class HasherInitException: public std::exception {};
class HasherHashException: public std::exception {};
class HasherGetException: public std::exception {};

#endif // HASHER_H