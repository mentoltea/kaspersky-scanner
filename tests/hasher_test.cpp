#include "connector.h"
#include "gtest/gtest.h"

std::map< std::string, std::string > hash_pairs = {
    {"String for hash1", "0f1d170ecb209ca91aae0cb9da09eae1"},
    {"String for hash2", "3a82841274807f04bdbb355601f7e1ff"},
    {"AbCdEfG", "f84e652360fb872d03701d9dde213ca0"},
    {" 1  2   3    4", "99e09c2ec50db65651a73ac1e5893d8d"},
    {"hello world", "5eb63bbbe01eeed093cb22bb8f5acdc3"},
    {"The quick brown fox jumps over the lazy dog", "9e107d9d372bb6826bd81d3542a419d6"},
    {"Lorem ipsum dolor sit amet", "fea80f2db003d4ebc4536023814aa885"},
    {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", "437bba8e0bf58337674f4539e75186ac"},
    {"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b"},
    {"1234567890", "e807f1fcf82d132f9bb018ca6738a19f"},
    {"password123", "482c811da5d5b4bc6d497ffa98491e38"},
    {"test string", "6f8db599de986fab7a21625b7916589c"},
    {"md5 hash generator", "dbfadd0cc846ea34071279877b64691c"},
};

TEST(md5hash, hashing_full) {
    bool flag = true;
    for (auto &pair: hash_pairs) {
        const std::string &key = pair.first; 
        const char* data = key.c_str();
        size_t size = key.size();

        MD5Hasher hasher;
        hasher.Hash(data, size);
        std::string hashstring = hasher.GetHashString();

        if (hashstring != pair.second) {
            std::cout << "Key: " << key << std::endl;
            std::cout << "Expected: " << pair.second << std::endl;
            std::cout << "Got: " << hashstring << std::endl;
            flag = false;
        }
    }
    
    if (flag) {SUCCEED();return;}
    else {FAIL();return;}
}

TEST(md5hash, hashing_half) {
    bool flag = true;
    for (auto &pair: hash_pairs) {
        const std::string &key = pair.first;

        const char* data = key.c_str();
        size_t size = key.size();
        size_t halfsize = size/2;

        MD5Hasher hasher;
        hasher.Hash(data, halfsize);
        hasher.Hash(data + halfsize, size - halfsize);
        std::string hashstring = hasher.GetHashString();

        if (hashstring != pair.second) {
            std::cout << "Key: " << key << std::endl;
            std::cout << "Expected: " << pair.second << std::endl;
            std::cout << "Got: " << hashstring << std::endl;
            flag = false;
        }
    }

    if (flag) {SUCCEED();return;}
    else {FAIL();return;}
}

TEST(md5hash, hashing_third) {
    bool flag = true;
    for (auto &pair: hash_pairs) {
        const std::string &key = pair.first;

        const char* data = key.c_str();
        size_t size = key.size();
        size_t thirdsize = size/3;
        size_t twothirdsize = size*2/3;

        MD5Hasher hasher;
        hasher.Hash(data, thirdsize);
        hasher.Hash(data + thirdsize, twothirdsize - thirdsize);
        hasher.Hash(data + twothirdsize, size - twothirdsize);
        std::string hashstring = hasher.GetHashString();

        if (hashstring != pair.second) {
            std::cout << "Key: " << key << std::endl;
            std::cout << "Expected: " << pair.second << std::endl;
            std::cout << "Got: " << hashstring << std::endl;
            flag = false;
        }
    }

    if (flag) {SUCCEED();return;}
    else {FAIL();return;}
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}