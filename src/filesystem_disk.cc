#include <boost/filesystem.hpp>
#include <iostream>
#include <sstream>

#include "filesystem_disk.h"

namespace fs = boost::filesystem;

// * Reminder: canonical(p) checks for exists(p).

// Return if a node exists at path.
bool DiskFileSystem::exists(std::string path)
{
    return fs::exists(path, ec_);
}

// Return if a node exists at path and is a regular file.
bool DiskFileSystem::is_regular_file(std::string path)
{
    return exists(path) && fs::is_regular_file(path, ec_);
}

// Return if a node exists at path and is a directory file.
bool DiskFileSystem::is_directory(std::string path)
{
    return exists(path) && fs::is_directory(path, ec_);
}

// If file at filepath exists, then returns contents of file as string.
// If file does not exist or error occurs, return empty string and set error.
std::string DiskFileSystem::get(std::string filepath)
{
    auto path = fs::canonical(fs::path(filepath), ec_);
    if (ec_)
    {
        return "";
    }

    fs::ifstream file(path);
    if (file.is_open())
    {
        std::stringstream sstr;
        file >> sstr.rdbuf();
        return sstr.str();
    }
    else
    {
        ec_ = sys::errc::make_error_code(sys::errc::file_exists);
    }

    return "";
}

// If no error, returns vector of filenames in dirpath (directory).
// If error, returns empty vector and sets error.
std::vector<std::string> DiskFileSystem::list_dir(std::string dirpath)
{
    std::vector<std::string> list;
    auto path = fs::canonical(dirpath, ec_);
    if (ec_)
    {
        return list;
    }

    if (fs::is_directory(path, ec_))
    {
        auto dir_iter = fs::directory_iterator(path);
        while (dir_iter != fs::directory_iterator{})
        {
            list.push_back(dir_iter->path().filename().string());
            dir_iter++;
        }
    }
    return list;
}

// Overwrites a file or creates it if it doesn't exist.
bool DiskFileSystem::write(std::string filepath, const std::string &body)
{
    auto path = fs::path(filepath);

    auto openmode = std::ios::out;

    fs::path canon_path;
    if (fs::exists(path, ec_))
    {
        // Truncate mode - overwrite file
        openmode |= std::ios::trunc;
        canon_path = fs::canonical(path, ec_);
    }
    else
    {
        // Create all needed directories leading to the file
        auto parents = fs::absolute(path).parent_path();
        fs::create_directories(parents, ec_);
        if (ec_)
        {
            return false;
        }
        // Canonicalize the parent directories and append the filename to produce a truly canonical path.
        canon_path = fs::canonical(parents, ec_) / path.filename();
    }

    // If canonicalizing went wrong...
    if (ec_)
    {
        return false;
    }

    fs::ofstream file(canon_path, openmode);

    if (!file.is_open())
    {
        return false;
    }

    file << body;
    return true;
}

// Delete a file/directory and all its children.
bool DiskFileSystem::remove(std::string path)
{
    auto canon_path = fs::canonical(fs::path(path), ec_);
    if (ec_)
    {
        return false;
    }
    // remove_all returns uintmax_t; it can cast to bool but compare anyway
    return fs::remove_all(canon_path, ec_) > 0;
}

// * Clears and returns the last given error code.
sys::error_code DiskFileSystem::err()
{
    auto ec = ec_;
    ec_.clear();
    return ec;
}