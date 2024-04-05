#ifndef FAE646C9_0156_427A_A756_ADF1068CA243
#define FAE646C9_0156_427A_A756_ADF1068CA243

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <map>
#include <string>
#include <vector>

#include "filesystem.h"

namespace fs = boost::filesystem;
namespace sys = boost::system;

namespace __fake
{
enum class FileType
{
    RegularFile,
    Directory,
};
struct File
{
    std::string name_;
    FileType type_;
    std::string contents_;
};

fs::path resolve(const fs::path &p);
} // namespace __fake

// An interface for filesystem operations on a fake disk.
// ! This is a *simple* filesystem for testing. It is not a comprehensive in-memory filesystem.
// ! It uses strings to implement directory structures. Be lenient with paths. Do not try to break it.
class FakeFileSystem : public FileSystem
{
  public:
    virtual bool exists(std::string path);
    virtual bool is_regular_file(std::string path);
    virtual bool is_directory(std::string path);
    virtual std::string get(std::string filepath);
    virtual std::vector<std::string> list_dir(std::string dirpath);
    virtual bool write(std::string filepath, const std::string &body);
    virtual bool remove(std::string path);
    virtual void reset();

    virtual sys::error_code err();

  private:
    // boost::filesystem::path is essentially string anyway - interchangeable.
    std::map<fs::path, __fake::File> files_;
    sys::error_code ec_;
};

#endif /* FAE646C9_0156_427A_A756_ADF1068CA243 */
