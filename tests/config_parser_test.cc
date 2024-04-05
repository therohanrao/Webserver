#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include "gtest/gtest.h"
#include <map>

#include "config_parser.h"

class NginxConfigParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

// Tests for different inputs & formats
TEST_F(NginxConfigParserTest, SimpleConfig) {
  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ConfigSerialization) {
    bool success = parser.Parse("example_config", &out_config);

    const int EXAMPLE_CONFIG_NUM_LINES = 7; //number of lines in example config at the time of writing this code
    const char FIRST_CHAR_OF_TEST_CONFIG = 'f'; //first character in example_config at the time of writing this code
    const char LAST_CHAR_OF_TEST_CONFIG = '}'; //last character in example_config at the time of writing this code

    const int INDEX_OF_FIRST_CHAR = 14; //index of first char in serialized string (there are 14 leading spaces)
    

    std::string serialized = out_config.ToString(EXAMPLE_CONFIG_NUM_LINES);
    
    const int INDEX_OF_LAST_CHAR = serialized.length()-2; //index of last char is followed by newline, so must go back 1 more
    
    
    EXPECT_TRUE(serialized[INDEX_OF_FIRST_CHAR] == FIRST_CHAR_OF_TEST_CONFIG &&
                serialized[INDEX_OF_LAST_CHAR] == LAST_CHAR_OF_TEST_CONFIG);
}

TEST_F(NginxConfigParserTest, EOFConfig) {
  bool success = parser.Parse("eof_config", &out_config);

  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, EmptyBracketConfig) {
  bool success = parser.Parse("empty_brackets_config", &out_config);

  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, EmptyStatementConfig) {
 bool success = parser.Parse("empty_statement_config", &out_config);

 EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, CommentConfig) {
 bool success = parser.Parse("comment_config", &out_config);

 EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, NestedBlocksConfig) {
 bool success = parser.Parse("nested_blocks_config", &out_config);

 EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, MultipleNestedBlocksConfig) {
 bool success = parser.Parse("multiple_nested_blocks_config", &out_config);

 EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ConsecutiveNestedBlocksConfig) {
 bool success = parser.Parse("consecutive_nested_blocks_config", &out_config);

 EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, SINGLE_QUOTE) {
    bool success = parser.Parse("single_quote", &out_config);

    EXPECT_TRUE(success);
}

// Test for get_port() function
TEST_F(NginxConfigParserTest, GetPortNum) {
 bool success = parser.Parse("get_port_num", &out_config);
 int port = out_config.get_port();

 EXPECT_TRUE(port==80);
}

TEST_F(NginxConfigParserTest, PortNumFromChildBlock) {
 bool success = parser.Parse("get_port_num_child", &out_config);
 int port = out_config.get_port();

 EXPECT_TRUE(port==80);
}

TEST_F(NginxConfigParserTest, GetNullPortNum) {
 bool success = parser.Parse("empty_config", &out_config);
 int port = out_config.get_port();

 EXPECT_TRUE(port==-1);
}

TEST_F(NginxConfigParserTest, GetNestedPortNum) {
 bool success = parser.Parse("get_nested_port", &out_config);
 int port = out_config.get_port();

 EXPECT_TRUE(port==80);
}

TEST_F(NginxConfigParserTest, MissingPortNum) {
 bool success = parser.Parse("missing_port", &out_config);
 int port = out_config.get_port();

 EXPECT_TRUE(port==-1);
}

TEST_F(NginxConfigParserTest, GetNegativePortNum) {
 bool success = parser.Parse("negative_port", &out_config);
 int port = out_config.get_port();

 EXPECT_TRUE(port==-1);
}

TEST_F(NginxConfigParserTest, MissingSemicolon) {
 bool success = parser.Parse("missing_semicolon_port", &out_config);
 int port = out_config.get_port();

 EXPECT_TRUE(port==-1);
}

TEST_F(NginxConfigParserTest, ExtractLocationMappings) {
 std::map<std::string, std::string> gt_location_map; //gt = ground truth
 std::map<std::string, std::pair<std::string, std::shared_ptr<NginxConfig>>> test_location_map;
 std::string static_path1 = "/static1", static_path2 = "/static2", echo_path = "/echo";
 
 gt_location_map[static_path1] = "StaticHandler";
 gt_location_map[static_path2] = "StaticHandler";
 gt_location_map[echo_path] = "EchoHandler";
 
 bool success = parser.Parse("static_config", &out_config);
 int error;
 test_location_map = out_config.get_location_mapping(error);

 EXPECT_EQ(gt_location_map[static_path1], test_location_map[static_path1].first);
 EXPECT_EQ(gt_location_map[static_path2], test_location_map[static_path2].first);
 EXPECT_EQ(gt_location_map[echo_path], test_location_map[echo_path].first);
}

TEST_F(NginxConfigParserTest, ExtractLocationNoServer) {
 std::map<std::string, std::pair<std::string, std::shared_ptr<NginxConfig>>> test_location_map; //gt = ground truth

 bool success = parser.Parse("static_config_no_server_block", &out_config);

 int error;
 test_location_map = out_config.get_location_mapping(error);

 EXPECT_TRUE(error);
}

TEST_F(NginxConfigParserTest, GetRootFromChildBlock) {
 std::map<std::string, std::pair<std::string, std::shared_ptr<NginxConfig>>> test_location_map; //gt = ground truth

 std::string static_path1 = "/static1", static_path2 = "/static2";
 std::string gt_root1 = "./folder1", gt_root2 = "./folder2";
 bool success = parser.Parse("static_config", &out_config);

 int error;
 test_location_map = out_config.get_location_mapping(error);

 std::string root1 = test_location_map[static_path1].second->get_root();
 std::string root2 = test_location_map[static_path2].second->get_root();

 EXPECT_EQ(root1, gt_root1);
 EXPECT_EQ(root2, gt_root2);
}

TEST_F(NginxConfigParserTest, GetMaxThreadsFromBlock)
{
 int threads = 0; //gt
 bool success = parser.Parse("static_config", &out_config);

 int parsed_threads = out_config.get_max_threads();

 EXPECT_EQ(threads, parsed_threads);
}

TEST_F(NginxConfigParserTest, GetMaxThreadsError)
{
 bool success = parser.Parse("static_config_threads_malformed", &out_config);

 int parsed_threads = out_config.get_max_threads();

 EXPECT_TRUE(parsed_threads == -1);
}
