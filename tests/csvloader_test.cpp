#include "connector.h"
#include "gtest/gtest.h"

TEST(csv, load) {
    Table csv = {
        {"Element1", "Element2", "Element3"},
        {"10", "11"},
        {"20"},
        {"30", "31"},
        {"40", "41", "42"},
    };

    std::string filename = ".__csvloader_test__.csv";

    std::ofstream ofs(filename);
    char delimitor = ';';
    size_t l=0;
    for (Row &row: csv) {
        size_t i=0;
        for (Element &element: row) {
            ofs << element;
            if (i != row.size()-1) ofs << delimitor;
            i++;
        }
        if (l != csv.size()-1) ofs << std::endl;
        l++;
    }
    ofs.close();

    std::ifstream ifs(filename);
    Table table = read_csv(ifs, delimitor);

    ASSERT_EQ(table.size(), csv.size());
    for (size_t i=0; i<table.size(); i++) {
        ASSERT_EQ(table[i].size(), csv[i].size());
        for (size_t j=0; j<table[i].size(); j++) {
            EXPECT_EQ(table[i][j], csv[i][j]);
        }
    }

    SUCCEED();
}

TEST(csv, load_custom_delim) {
    Table csv = {
        {"Element1", "Element2", "Element3"},
        {"10", "11"},
        {"23"},
        {"33", "31"},
        {"43", "41", "42"},
        {"43", "51", "42"},
        {"43", "51", "42"},
        {"43", "51", "47"},
        {"43", "51", "g7"},
        {"4f", "41", "g7"},
        {"4f", "9j", "g7"},
        {"4f", "9j", "g2"},
        {"45", "9", "4k"},
        {"45", "9fj", "4ks"},
        {"45", "sf1", "4ks"},
        {"45", "sf1", "4ks"},
        {"40", "sdf1", "4s2"},
    };

    std::string filename = ".__csvloader_test__.csv";

    std::ofstream ofs(filename);
    char delimitor = '+';
    size_t l=0;
    for (Row &row: csv) {
        size_t i=0;
        for (Element &element: row) {
            ofs << element;
            if (i != row.size()-1) ofs << delimitor;
            i++;
        }
        if (l != csv.size()-1) ofs << std::endl;
        l++;
    }
    ofs.close();

    std::ifstream ifs(filename);
    Table table = read_csv(ifs, delimitor);

    ASSERT_EQ(table.size(), csv.size());
    for (size_t i=0; i<table.size(); i++) {
        ASSERT_EQ(table[i].size(), csv[i].size());
        for (size_t j=0; j<table[i].size(); j++) {
            EXPECT_EQ(table[i][j], csv[i][j]);
        }
    }

    SUCCEED();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}