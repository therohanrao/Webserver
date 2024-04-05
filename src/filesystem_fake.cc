#include <iostream>

#include "filesystem_fake.h"

namespace fs = boost::filesystem;

bool FakeFileSystem::exists(std::string path)
{
    return files_.find(__fake::resolve(fs::path(path)).string()) != files_.end();
}

bool FakeFileSystem::is_regular_file(std::string path)
{
    auto path_str = __fake::resolve(fs::path(path)).string();
    auto file_it = files_.find(path_str);
    if (file_it == files_.end())
    {
        ec_.assign(sys::errc::no_such_file_or_directory, sys::system_category());
        return false;
    }
    else
    {
        ec_.clear();
        return file_it->second.type_ == __fake::FileType::RegularFile;
    }
}

bool FakeFileSystem::is_directory(std::string path)
{
    auto path_str = __fake::resolve(fs::path(path)).string();
    auto file_it = files_.find(path_str);
    if (file_it == files_.end())
    {
        ec_.assign(sys::errc::no_such_file_or_directory, sys::system_category());
        return false;
    }
    else
    {
        ec_.clear();
        return file_it->second.type_ == __fake::FileType::Directory;
    }
}

std::string FakeFileSystem::get(std::string filepath)
{
    auto abspath = __fake::resolve(fs::path(filepath));

    // If file at the absolute path was found, return its contents
    auto file_it = files_.find(abspath);
    if (file_it != files_.end())
    {
        ec_.clear();
        return file_it->second.contents_;
    }
    else
    {
        ec_.assign(sys::errc::no_such_file_or_directory, sys::system_category());
        return "";
    }
}

// List all the file/directory names in a directory.
std::vector<std::string> FakeFileSystem::list_dir(std::string dirpath_str)
{
    auto dirpath = __fake::resolve(fs::path(dirpath_str));

    std::vector<std::string> list;

    // Find every path for which the parent_path is exactly dirpath and gather their filenames.
    for (auto it = files_.begin(); it != files_.end(); ++it)
    {
        auto parent = it->first.parent_path();
        if (parent == dirpath)
        {
            list.push_back(it->first.filename().string());
        }
    }

    ec_.clear();
    return list;
}

// Returns true if the file was successfully written.
// Never returns false - writing to a map can't fail!
bool FakeFileSystem::write(std::string filepath, const std::string &body)
{
    auto abspath = __fake::resolve(fs::path(filepath));

    // create the subdirectories of the path
    auto parent = abspath.parent_path();
    while (!parent.empty())
    {
        if (!exists(parent.string()))
        {
            files_[parent] = {parent.filename().string(), __fake::FileType::Directory, ""};
        }
        parent = parent.parent_path();
    }

    // now create/overwrite the file
    files_[abspath] = {abspath.filename().string(), __fake::FileType::RegularFile, body};

    ec_.clear();
    return true;
}

// Returns true if something was removed; false otherwise.
bool FakeFileSystem::remove(std::string path)
{
    auto abspath = __fake::resolve(fs::path(path));

    auto file_it = files_.find(abspath);
    if (file_it == files_.end())
    {
        // Not found, nothing to remove
        ec_.clear();
        return false;
    }
    else if (file_it->second.type_ == __fake::FileType::RegularFile)
    {
        // Remove the regular file
        files_.erase(file_it);
        ec_.clear();
        return true;
    }
    else // if (file_it->second.type_ == __fake::FileType::Directory)
    {
        // Remove the directory
        files_.erase(file_it);
        auto it = files_.begin();
        // Remove its children (recursively)
        while (it != files_.end())
        {
            // Go through segment by segment in the path, matching
            auto path = it->first;
            auto d = abspath.begin(), p = path.begin();
            while (d != abspath.end() && p != path.end())
            {

                if (*d == *p)
                {
                    ++d;
                    ++p;
                }
                else
                {
                    break;
                }
            }
            // If we got here and d == abspath.end(), it means directory path d is a prefix of path p. Remove p
            if (d == abspath.end())
            {
                it = files_.erase(it);
            }
            else 
            {
                ++it;
            }
        }

        ec_.clear();
        return true;
    }
}

void FakeFileSystem::reset() 
{
    ec_.clear();
    files_.clear();
}


sys::error_code FakeFileSystem::err()
{
    auto ec = ec_;
    ec_.clear();
    return ec;
}

namespace __fake
{
fs::path resolve(const fs::path &p)
{
    auto abs = fs::absolute(p).lexically_normal();
    // If ends with a trailing slash, return parent_path
    return abs.filename() == "." ? abs.parent_path() : abs;
}
} // namespace __fake