# **General Overview of Source Code**

                            +----------------------------+
                            |        server_main.cc      +---------------------+
                            +-------------+--------------+           get parsed|
                                        |                            config    |
                                        |                                      |
                                        v                                      v
                         +----------------------------+         +-------------+--------------+
                    +----+          server.h          +----+    |      config_parser.h       |
                    |    +----------------------------+    |    +----------------------------+
                    |                                      |
     init a session |                      setup dispatcher|
     object for each|                      based on parsed |
     request        v                      config          v
      +-------------+--------------+        +--------------+--------------+
      |         session.h          +------->+        dispatcher.h         |
      +-------------+--------------+  get   +--------------+--------------+
                    |                 hdlr                 |
         get parsed |                         return an    |
         request    v                         instance of  v
      +-------------+--------------+        +--------------+--------------+
      |      request_parser.h      |        |  request_handler.h          |
      +----------------------------+        |  ++request_handler_static.h |
                                            |  ||request_handler_echo.h   |
                                            |  ||request_handler_error.h  |
                                            |  ||request_handler_status.h |
                                            |  ||request_handler_proxy.h  |
                                            |  ||request_handler_meme.h   |
                                            +-----------------------------+

The purpose of this program is to create a webserver that is able to parse a server config file, serve echo and static files, and display these files or gracefully error and notify the user by displaying and error message. The above diagram shows the overall layout of the code, which utilizes C++ and Boost library. The main file is *server_main.cc*, which initializes the server in *server_init.cc*. This initialization parses the config file with *config_parser.cc*. This server then receives requests, creating a session for each request, which delegates the request to a dispatcher who decides whether the request should be handled by static or echo handler.

## **Logging**

Although it is not shown in the above diagram, logging is used throughout the process to detail important information, warnings, and errors. 

### **Syslog_#.log**

Located in **src** folder, this file contains all of the webserver log details, such as parsing the server config file and processing the HTTP request.

## **Header Files Folder**

Inside of the **include** folder, all of the header files of the program are held here:

* `config_parser.h`: Parses the config and stores a map of location to a pair of `RequestHandler` (keyword after location path) and `NginxConfig`
* `request_handler.h`: Defines a `RequestHandler` virtual base class, which takes a `request`, `reply`, and config and properly updates the `reply`. This base class is inherited by multiple handlers:
    * `request_handler_echo.h`: Defines class `EchoHandler`. It returns a `reply` resembling the `request` given.
    * `request_handler_static.h`: Defines class `StaticHandler`. It returns a `reply` containing the requested file with corresponding MIME type formats.
    * `request_handler_error.h`: Defines class `ErrorHandler`. It sets a response with status not found and with body “File not found” when the request has an unknown target handler type.
* `server_init.h`: Initialize the server
* `server.h`: Defines class `server`. It initializes a server, starts the server and creates a new session to read and respond to the request.
* `session.h`: Defines class `session`. It reads the request through a tcp::socket, `socket`. It parses the request and creates an http boost request object with the parsed data. It calls the `request_handler_dispatcher` to create the appropriate handler type based on the request and populates an http boost response object by calling `handle_request` on the handler. Finally, it sends the response back through the socket to serve the data to the client. Once it's done reading and writing, the connection to the socket is closed.
* `utility.h`: Defines utility functions that don't belong to any class.
## **Source Files Folder**

The **src** folder contains the implementations for the header files. In addition, it includes the logging file *syslog_#.log* the server config file to be parsed.

## **Test Files Folder**

The **tests** folder has all of the unit test .cc files, which test the implementations of code in isolation. In addition, it holds the integration test bash script which tests the server as a whole, with all of its moving pieces. The actual and expected responses from each test in the integration test is held in this folder as well.

### **Test Configs**

The **test_configs** holds all the various config files to be used as unit tests for the config parser to ensure the config parser parses or errors properly.

### **Static Files**

The **folder1** and **folder2** hold various MIME type files, including a pdf, png, and zip. These are used in unit tests to ensure the static handler can handle all types of static files.

# **Building, Testing, and Running**

## **Build**

To build the source code using cmake:

    $ mkdir -p build
    $ cd build
    $ cmake ..
    $ make

## **Test**

To run the test:

Go to the `build` directory and run `make test` after building the source code, which is detailed above. 

### **Coverage**
To run all tests and generate a coverage report under `$[build_dir]/report/index.html`:

    $ mkdir -p build_coverage
    $ cd build_coverage
    $ cmake -DCMAKE_BUILD_TYPE=COVERAGE ..
    $ make coverage


## **Run**

After building the source code, to run the server, go to the root directory and run:

    ./build/bin/server ./src/server_config

# **How to Add a Request Handler**

To create a new handler, we will name the example handler as the `ExampleHandler`:

1. First you need to add your handler location and root to the server config:

    ```python
    # File: src/server_config
    location /example ExampleHandler {
        root /folder3;
    }
    ```

2. Define the class for the new handler as `ExampleHandler`. This `ExampleHandler` needs to use the public inheritance of the `RequestHandler` base class and override the `handle_request` method. This header file *request_handler_example.h* should be inside the **include** folder and the *request_handler_example.cc* file should be inside the **src** folder.

    ```C++
    // File: src/request_handler_example.h
    #ifndef REQUEST_HANDLER_EXAMPLE_H
    #define REQUEST_HANDLER_EXAMPLE_H

    #include <boost/filesystem.hpp>
    #include <boost/filesystem/fstream.hpp>
    #include <iostream>
    #include <map>

    #include "config_parser.h"
    #include "request_handler.h"


    // Inherit base class
    class ExampleHeader : public RequestHandler {
    public:
        // Override the pure virtual function
        void handle_request(std::string request, std::string &reply, NginxConfig config_);
    };

    #endif  // REQUEST_HANDLER_EXAMPLE_H
    ```

    In the cc file, write the implementations to the methods from the header file:

    ```C++
    #include <iostream>

    // include the example handler header file
    #include "request_handler_example.h"
    #include "config_parser.h"

    void ExampleHandler::handle_request(std::string request, std::string &reply, NginxConfig config_) {
        // Implementation of handle_request
    }
    ```

3. Setting up a new `HandlerFactory`


     To create a new HandlerFactory for the server, we must edit 4 files:
    

    ### `include/request_handler_factory.h:`
    
    * A header declaration that inherits from `RequestHandlerFactory` and overloads `RequestHandlerFactory::create(...)` 
    ```c++ 
    class ExampleHandlerFactory : public RequestHandlerFactory {
    public:
        virtual RequestHandler* create(const std::string& location, const std::string& url);
    };
    ```

    ### `src/request_handler_factory.cc:`
    * An implementation for the overloaded function and the respective `#include "request_handler_example.h"`
    ```c++ 
    #include "request_handler_example.h"

    RequestHandler* ExampleHandlerFactory::create(const std::string& location, const std::string& url) {
    return new ExampleHandler(...);
    }
    ```

    ### `include/utils.h:`
    * a const string that corresponds to the keyword after  
    `location /path ExampleHandler` in the server's config
    ```c++ 
    const std::string kExampleHandler = "ExampleHandler";
    ```

    ### `src/utils.cc:`
    * add to the chain of if else statements with your `kExampleHandler` and `ExampleHandlerFactory()`
    ```c++ 
    RequestHandlerFactory *createHandlerFactory(const std::string &name, std::shared_ptr<NginxConfig> config) {
    .
    ..
    ...
    
    if (name == kErrorHandler)
        return new ErrorHandlerFactory();
    if (name == kExampleHandler)
        return new ExampleHandlerFactory();
    else
        return nullptr;
    }
    ```

4. Setting up a new handler in our Dispatcher

    To set up a new handler in our dispatcher we don't have to do anything! If you run into include errors try including your new handler's header file to `src/request_handler_dispatcher.cc` and modify the respective linking in CMakeLists.txt 

    > **Note:** the `ErrorHandler` is currently always added to `RequestHandlerDispatcher::routes_` in the constructor regardless of config. Change accordingly if this is not your intended behavior.



5. Unit and Integration test(s)

    To test the next example handler, we will add a new file in `tests/request_handler_example_test.cc`:

    ```C++
    #include "gtest/gtest.h"
    #include <iostream>
    #include <boost/beast.hpp>

    #include "config_parser.h"
    #include "request_handler_echo.h"
    #include "request_handler_static.h"

    // Createe our test fixture class that can used for all tests
    class RequestHandlerExampleTest : public ::testing::Test
    {
    protected:
        // add any variables necessary for testing
        NginxConfigParser config_parser;
        NginxConfig config;
        char c;
        std::ifstream f;
        http::request<http::string_body> req;
        http::response<http::string_body> response;
    };

    // add any functions necessary for testing, this one tests if responses are equal (e.g the result, version, and body are equal)
    bool checkResponseEqual(const http::response<http::string_body> &response1, const http::response<http::string_body> &response2)
    {
        if (response1.result() != response2.result())
        {
            return false;
        }

        if (response1.version() != response2.version())
        {
            return false;
        }

        if (response1.body() != response2.body())
        {
            return false;
        }

        return true;
    }

    // Write your unit test and name it, using the RequestHandlerExampleTest fixture class
    TEST_F(RequestHandlerExampleTest, ExampleTest)
    {

    }
    ```

6. Setting up CMakeLists.txt with new Example Handler

    In *CMakeLists.txt*, we will add the new example header source file, putting it below the last existing `add_library()` call in *CMakeLists.txt*:

    ```python
    # File: CMakeLists.txt
    add_library(request_handler_example_lib src/request_handler_example.cc src/request_handler_echo.cc src/request_handler_static.cc src/request_handler_error.cc src/config_parser.cc)
    ```

    Here, we add any other sources files that the example handler has dependencies on, such as *request_handler_echo.cc*, *request_handler_static.cc*, *config_parser.cc*, etc

    Next, we will add the test.cc file in our **tests** folder made earlier to our *CMakeLists.txt*, putting it below the last existing `add_executable()` call in *CMakeLists.txt*:

    ```python
    # File: CMakeLists.txt
    add_executable(request_handler_example_test tests/request_handler_example_test.cc)
    ```

    After that, we will link the test.cc file with any libraries that the file depends on, putting it below the last existing `target_link_libraries()` call in *CMakeLists.txt*:

    **Note**: it is necessary that **EVERY** test file link the three Boost libraries as well as gtest_main.

    ```python
    # File: CMakeLists.txt
    target_link_libraries(request_handler_example_test request_handler_example_lib config_parser_lib request_handler_echo_lib request_handler_static_lib request_handler_error_lib Boost::system Boost::log_setup Boost::log gtest_main)
    ```
    Lastly, we add the test executable to the gtest so that it can be ran, putting it below the last existing `gtest_discover_tests()` call in *CMakeLists.txt*:

    ```python
    # File: CMakeLists.txt
    gtest_discover_tests(request_handler_example_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_configs)
    ```