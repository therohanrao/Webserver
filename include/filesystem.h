#ifndef ABA24C92_F9B3_4EA0_8058_F487DA85FB2F
#define ABA24C92_F9B3_4EA0_8058_F487DA85FB2F

#include <string>
#include <vector>
#include <boost/system/error_code.hpp>

// ? If concurrent code is needed, use boost::outcome in the future instead of err().

// An interface for filesystem operations.
// Just use boost's error_codes. You can plug your own in anyway.
class FileSystem
{
  public:
    virtual bool exists(std::string path) = 0;
    virtual bool is_regular_file(std::string path) = 0;
    virtual bool is_directory(std::string path) = 0;

    virtual std::string get(std::string filepath) = 0;
    virtual std::vector<std::string> list_dir(std::string dirpath) = 0;
    virtual bool write(std::string filepath, const std::string &body) = 0;
    virtual bool remove(std::string path) = 0;

    // Returns last error/status.
    virtual boost::system::error_code err() = 0;
};

#endif /* ABA24C92_F9B3_4EA0_8058_F487DA85FB2F */
