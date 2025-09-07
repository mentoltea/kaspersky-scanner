#include "connector.h"
#include "gtest/gtest.h"

#include <sstream>

TEST(bystream, SS_SS) {
    std::stringstream s1, s2;
    ByOStream by(s1, s2);

    by << "Something" << std::endl;
    by << "1 2 3 4" << 5 << 6 << std::endl;

    bool equal = true;
    char c1, c2;

    for (;;) {
        if (s1.eof() || s2.eof()) {
            if (s1.eof() != s2.eof()) {
                equal = false;
            }
            break;
        }
        s1 >> c1;
        s2 >> c2;
        if (c1 != c2) {
            equal = false;
            break;
        }
    }

    if (equal) {
        SUCCEED();
        return;
    }
    std::cout << s1.str() << std::endl;
    std::cout << s2.str() << std::endl;
    FAIL();
}

TEST(bystream, SS_OFS) {
    std::stringstream ss;
    std::string filename = ".__logtest__.txt";
    std::ofstream ofs(filename);

    ByOStream by(ofs, ss);

    by << "Something" << std::endl;
    by << "1 2 3 4" << 5 << 6 << std::endl;

    ofs.close();

    std::ifstream ifs(filename);

    bool equal = true;
    char c1, c2;

    for (;;) {
        if (ss.eof() || ifs.eof()) {
            if (ss.eof() != ifs.eof()) {
                equal = false;
            }
            break;
        }
        ss >> c1;
        ifs >> c2;
        if (c1 != c2) {
            equal = false;
            break;
        }
    }

    if (equal) {
        SUCCEED();
        return;
    }
    FAIL();
}

TEST(logstream, SS) {
    std::string str1 = "123";
    std::string str2 = "something";
    std::string str3 = "AbCdE";
    std::string str4 = "\\\t\n";

    std::stringstream s1, s2;

    LogStream stream(s1);
    stream << str1 << str2;
    stream.change_stream(s2);
    stream << str3 << str4;

    std::string res1 = s1.str();
    std::string res2 = s2.str();

    if (res1 == str1+str2 && res2 == str3+str4) {
        SUCCEED();
        return;
    }
    FAIL();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}