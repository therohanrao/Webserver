#include "filesystem_fake.h"

#include "gtest/gtest.h"
#include <boost/system/error_code.hpp>
#include <vector>
#include <string>

using namespace std;
namespace sys = boost::system;

class FakeFileSystemTest : public ::testing::Test {
protected:
    FakeFileSystem fs;

    void SetUp() {
        fs.reset();
    }
};

TEST_F(FakeFileSystemTest, WriteFileSuccess) {
    string input = "Test";
    fs.write("/path/file", input);
    bool res = fs.exists("/path/file");

    EXPECT_TRUE(res);
}

TEST_F(FakeFileSystemTest, GetFileSuccess) {
    string input = "Test";
    fs.write("/path/file", input);
    string res = fs.get("/path/file");

    EXPECT_EQ(res, input);
}

TEST_F(FakeFileSystemTest, GetFileFail) {
    string res = fs.get("/path/file");
    sys::error_code ec = fs.err();

    EXPECT_EQ(res, "");
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);

}

TEST_F(FakeFileSystemTest, IsRegularFileSuccess) {
    string input = "Test";
    fs.write("/path/file", input);
    bool res = fs.is_regular_file("/path/file");

    EXPECT_TRUE(res);
}

TEST_F(FakeFileSystemTest, IsRegularFileFail) {
    string input = "Test";
    fs.write("/path/file", input);
    bool res = fs.is_regular_file("/path");

    EXPECT_FALSE(res);
}

TEST_F(FakeFileSystemTest, IsRegularFileNotFound) {
    bool res = fs.is_regular_file("/path/file");
    sys::error_code ec = fs.err();

    EXPECT_FALSE(res);
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}

TEST_F(FakeFileSystemTest, IsDirectorySuccess) {
    string input = "Test";
    fs.write("/path/file", input);
    bool res = fs.is_directory("/path");

    EXPECT_TRUE(res);
}

TEST_F(FakeFileSystemTest, IsDirectoryFail) {
    string input = "Test";
    fs.write("/path/file", input);
    bool res = fs.is_directory("/path/file");

    EXPECT_FALSE(res);
}

TEST_F(FakeFileSystemTest, IsDirectoryNotFound) {
    bool res = fs.is_directory("/path");
    sys::error_code ec = fs.err();

    EXPECT_FALSE(res);
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}

TEST_F(FakeFileSystemTest, ListDirectoriesSuccess) {
    string input = "Test";
    fs.write("/path/file1", input);
    fs.write("/path/file2", input);
    fs.write("/path/path2/file1", input);
    vector<string> expected_output{ "file1", "file2", "path2" };

    vector<string> dir_list = fs.list_dir("/path");
    EXPECT_EQ(dir_list, expected_output);
}

TEST_F(FakeFileSystemTest, RemoveFileSuccess) {
    string input = "Test";
    fs.write("/path/file", input);
    bool remove_res = fs.remove("/path/file");
    string get_res = fs.get("/path/file");
    sys::error_code ec = fs.err();
    
    EXPECT_TRUE(remove_res);
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}

TEST_F(FakeFileSystemTest, RemoveNotFound) {
    bool res = fs.remove("/path/file");

    EXPECT_FALSE(res);
}

TEST_F(FakeFileSystemTest, RemoveDirectorySuccess) {
    string input = "Test";
    fs.write("/path/file", input);
    bool remove_res = fs.remove("/path");
    EXPECT_TRUE(remove_res);

    fs.get("/path");
    sys::error_code ec = fs.err();
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);

    fs.get("/path/file");
    ec = fs.err();
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}