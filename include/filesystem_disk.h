#ifndef D00785B4_6DFF_4577_92E2_B23958C84960
#define D00785B4_6DFF_4577_92E2_B23958C84960

#include <boost/system/error_code.hpp>
#include <string>
#include <vector>

#include "filesystem.h"

namespace sys = boost::system;

// An interface for filesystem operations on the disk.
class DiskFileSystem : public FileSystem
{
  public:
    virtual bool exists(std::string path);
    virtual bool is_regular_file(std::string path);
    virtual bool is_directory(std::string path);

    virtual std::string get(std::string filepath);
    virtual std::vector<std::string> list_dir(std::string dirpath);
    virtual bool write(std::string filepath, const std::string &body);
    virtual bool remove(std::string path);

    virtual sys::error_code err();

  private:
    sys::error_code ec_;
};

#endif /* D00785B4_6DFF_4577_92E2_B23958C84960 */
