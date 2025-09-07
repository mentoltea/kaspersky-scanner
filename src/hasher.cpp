#include "hasher.h"

MD5Hasher::MD5Hasher() {
    ctx.hProv = 0;
    if ( ! CryptAcquireContextW( &ctx.hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT ) ) {
        Logger.level(LEVEL_ERROR) << "Unable to acquire CSP context" << std::endl;
        throw HasherCSPException();
    }
    
    ctx.hHash = 0;
    if ( ! CryptCreateHash( ctx.hProv, CALG_MD5, 0, 0, &ctx.hHash ) ) {
        Logger.level(LEVEL_ERROR) << "Unable to create hash object" << std::endl;
        CryptReleaseContext( ctx.hProv , 0 );       
        throw HasherInitException();
    }
}

MD5Hasher::~MD5Hasher() {
    if (ctx.hHash) {
        CryptDestroyHash( ctx.hHash );
        ctx.hHash = 0;
    }
    if (ctx.hProv) {
        CryptReleaseContext( ctx.hProv , 0 );
        ctx.hProv = 0;
    }
}

void MD5Hasher::Hash(const void *data, const size_t size) {
    if ( ! CryptHashData( ctx.hHash, static_cast<const BYTE*>(data), size, 0) ) {
        Logger.level(LEVEL_ERROR) << "Hashing error" << std::endl;
        throw HasherHashException();
    }
}

std::vector< BYTE > MD5Hasher::GetHashBytes() const {
    DWORD cbHashSize = 0;
    DWORD dwCount = sizeof( DWORD );
    if( ! CryptGetHashParam( ctx.hHash, HP_HASHSIZE, (BYTE *)&cbHashSize, &dwCount, 0 ) ) {
        Logger.level(LEVEL_ERROR) << "Unable to get params of hash object" << std::endl;
        throw HasherGetException();
    }
    
    std::vector<BYTE> buffer( cbHashSize );
    if ( ! CryptGetHashParam( ctx.hHash, HP_HASHVAL, reinterpret_cast< BYTE*>( &buffer[0] ), &cbHashSize, 0) ) {
        Logger.level(LEVEL_ERROR) << "Unable to get params of hash object" << std::endl;
        throw HasherGetException();
    }

    return buffer;
}

std::string MD5Hasher::GetHashString() const {
    std::vector<BYTE> bytes = GetHashBytes();
    std::ostringstream ss;

    for (BYTE it: bytes) {
        ss.fill('0');
        ss.width(2);
        ss << std::hex << static_cast<const int>(it);
    }

    return ss.str();
}