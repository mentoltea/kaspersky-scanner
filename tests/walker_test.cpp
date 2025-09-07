#include "connector.h"
#include "gtest/gtest.h"

TEST(walker, hashfile) {
    std::string filecontent = "Adfsauifbds9uifgbfusljedgbsulyebgi;usebgiurets ertgfrefdg";
    std::string hash = "27469fa285d511d0dbb3875b567980c9";

    std::string filename = ".__walker_hash_file_test.txt";
    
    std::ofstream ofs(filename);
    ofs.write(filecontent.c_str(), filecontent.size());
    ofs.close();

    fs::path path(filename);
    size_t max_chunk = MAX_CHUNK_SIZE;
    std::string hashstr = hash_file(path, max_chunk);

    if (hash != hashstr) {
        FAIL();
        return;
    }
    SUCCEED();
}

TEST(walker, hashfile_small_chunk) {
    std::string filecontent = "srefgilbhjdhblfuidg silhdfsaj vdhlsauv adilhsyvfhajnkdsgf Adfsauifbds9uifgbfusljedgbsulyebgi;usebgiurets ertgfrefdg";
    std::string hash = "881ca3843c46379aa2f7f0f20a927794";

    std::string filename = ".__walker_hash_file_test.txt";
    
    std::ofstream ofs(filename);
    ofs.write(filecontent.c_str(), filecontent.size());
    ofs.close();

    fs::path path(filename);
    size_t max_chunk = 32;
    std::string hashstr = hash_file(path, max_chunk);

    if (hash != hashstr) {
        FAIL();
        return;
    }
    SUCCEED();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}