#include "filesystem_disk.h"
#include "gtest/gtest.h"

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <boost/unordered_set.hpp>

#include <vector>
#include <string>

using namespace std;
namespace sys = boost::system;
namespace bfs = boost::filesystem;

const string FS_DIRECTORY_PATH = "./FileSystem";
const string FS_FILE_PATH = FS_DIRECTORY_PATH + "/file";

class DiskFileSystemTest : public ::testing::Test {
protected:
    DiskFileSystem fs;
    sys::error_code ec;

    void TearDown() {
        fs.remove(FS_FILE_PATH);
        fs.remove(FS_DIRECTORY_PATH);
    }
};

TEST_F(DiskFileSystemTest, WriteFileSuccess) {
    string input = "Test";

    bool res = fs.write(FS_FILE_PATH, input);

    EXPECT_TRUE(res);
}

TEST_F(DiskFileSystemTest, GetFileSuccess) {
    string input = "Test2";


    fs.write(FS_FILE_PATH, input);
    string res = fs.get(FS_FILE_PATH);

    EXPECT_EQ(res, input);
}

TEST_F(DiskFileSystemTest, GetFileFail) {
    string res = fs.get(FS_FILE_PATH);
    sys::error_code ec = fs.err();

    EXPECT_EQ(res, "");
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}

TEST_F(DiskFileSystemTest, IsRegularFileSuccess) {
    string input = "Test";
    fs.write(FS_FILE_PATH, input);
    bool res = fs.is_regular_file(FS_FILE_PATH);

    EXPECT_TRUE(res);
}

TEST_F(DiskFileSystemTest, IsRegularFileFail) {
    string input = "Test";
    fs.write(FS_FILE_PATH, input);
    bool res = fs.is_regular_file(FS_DIRECTORY_PATH);

    EXPECT_FALSE(res);
}

TEST_F(DiskFileSystemTest, IsRegularFileNotFound) {
    bool res = fs.is_regular_file(FS_FILE_PATH);
    sys::error_code ec = fs.err();

    EXPECT_FALSE(res);
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}

TEST_F(DiskFileSystemTest, IsDirectorySuccess) {
    string input = "Test";
    fs.write(FS_FILE_PATH, input);
    bool res = fs.is_directory(FS_DIRECTORY_PATH);

    EXPECT_TRUE(res);
}

TEST_F(DiskFileSystemTest, IsDirectoryFail) {
    string input = "Test";
    fs.write(FS_FILE_PATH, input);
    bool res = fs.is_directory(FS_FILE_PATH);

    EXPECT_FALSE(res);
}

TEST_F(DiskFileSystemTest, IsDirectoryNotFound) {
    bool res = fs.is_directory(FS_FILE_PATH);
    sys::error_code ec = fs.err();

    EXPECT_FALSE(res);
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}

TEST_F(DiskFileSystemTest, ListDirectoriesSuccess) {
    string input = "Test";
    fs.write(FS_FILE_PATH+"1", input);
    fs.write(FS_FILE_PATH+"2", input);
    fs.write(FS_DIRECTORY_PATH+"/dir2/file", input);
    boost::unordered_set<string> expected_output({ "file1", "file2", "dir2" });
    vector<string> dir_list = fs.list_dir(FS_DIRECTORY_PATH);
    
    for (auto dir : dir_list) {
        EXPECT_TRUE(expected_output.find(dir) != expected_output.end());
    }
}

TEST_F(DiskFileSystemTest, RemoveFileSuccess) {
    string input = "Test";
    fs.write(FS_FILE_PATH, input);
    bool remove_res = fs.remove(FS_FILE_PATH);
    string get_res = fs.get(FS_FILE_PATH);
    sys::error_code ec = fs.err();
    
    EXPECT_TRUE(remove_res);
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}

TEST_F(DiskFileSystemTest, RemoveNotFound) {
    bool res = fs.remove("/PATH_NOT_FOUND");
    
    EXPECT_FALSE(res);
}

TEST_F(DiskFileSystemTest, RemoveDirectorySuccess) {
    string input = "Test";
    fs.write(FS_FILE_PATH, input);
    bool remove_res = fs.remove(FS_DIRECTORY_PATH);
    EXPECT_TRUE(remove_res);

    fs.get(FS_DIRECTORY_PATH);
    sys::error_code ec = fs.err();
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);

    fs.get(FS_FILE_PATH);
    ec = fs.err();
    EXPECT_EQ(ec, sys::errc::no_such_file_or_directory);
}