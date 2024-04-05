// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <utility> // for std::pair
#include <vector>


#include "config_parser.h"

void remove_trailing_slash(std::string& path) {
    while (path.length() > 1 && path.back() == '/')
        path.pop_back();
}

std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}

// function to return port number based on config
int NginxConfig::get_port() {
  for (const auto& statement : statements_) {
    if (statement->child_block_.get() == nullptr) {
      if (statement->tokens_.size() == 2 && statement->tokens_[0] == "port") {
        int ret = atoi(statement->tokens_[1].c_str());
        if (ret >= 0 && ret <= 0xffff) {
            return ret;
        } else {
            return -1;
        }
      }
    } else if (statement->child_block_.get() != nullptr) {
        int ret;
        if ((ret = statement->child_block_->get_port()) != -1) {
          return ret;
        }
    }
  }
  return -1;
}

std::string NginxConfig::get_root() {
  for (const auto& statement : statements_) {
    int num_tokens_per_statement = 2;
    int index_first_token = 0;
    int index_second_token = 1;
    if (statement->tokens_.size() == num_tokens_per_statement && statement->tokens_[index_first_token] == "root") {
      std::string ret = statement->tokens_[index_second_token].c_str();
      return ret;
    }
  }
  return "";
}

std::string NginxConfig::get_data_path() {
  for (const auto& statement : statements_) {
    int num_tokens_per_statement = 2;
    int index_first_token = 0;
    int index_second_token = 1;
    if (statement->tokens_.size() == num_tokens_per_statement && statement->tokens_[index_first_token] == "data_path") {
      std::string ret = statement->tokens_[index_second_token].c_str();
      return ret;
    }
  }
  return "";
}

int NginxConfig::get_max_threads()
{
  int max_int_hex = 0xffff;
  for (const auto &statement : statements_)
  {
    if (statement->child_block_.get() == nullptr)
    {
      if (statement->tokens_.size() == 2 && statement->tokens_[0] == "threads")
      {
          int num_threads = atoi(statement->tokens_[1].c_str());
          if (num_threads >= 0 && num_threads <= max_int_hex)
          {
            return num_threads;
          }
          else
          {
            return -1;
          }
      }
    }
    else if (statement->child_block_.get() != nullptr)
    {
      int num_threads;
      if ((num_threads = statement->child_block_->get_max_threads()) != -1)
      {
          return num_threads;
      }
    }
  }
  return -1;
}

//0 = no error, 1 = syntax error
std::map<std::string, std::pair<std::string, std::shared_ptr<NginxConfig>>> NginxConfig::get_location_mapping(int& error) {
  
  int num_tokens_per_statement = 3; // 'location /path HandlerType'
  int index_first_token = 0;
  int index_second_token = 1;
  int index_third_token = 2;

  std::map<std::string, std::pair<std::string, std::shared_ptr<NginxConfig>>> location_map;
  if (statements_[0]->tokens_[index_first_token] == "server") {

    for (const auto& statement : statements_[0]->child_block_->statements_) {
        if (statement->tokens_.size() != num_tokens_per_statement || statement->tokens_[index_first_token] != "location") {
            continue;
        }
        
        std::string remote_path = statement->tokens_[index_second_token].c_str();
        std::string handler_type = statement->tokens_[index_third_token].c_str();
        std::shared_ptr<NginxConfig> location_config = std::make_shared<NginxConfig>(*statement->child_block_); //deep copy unqiueptr -> sharedptr
        
        remove_trailing_slash(remote_path);

        location_map[remote_path] = {handler_type, location_config};
    }
  } else { error = 1; return location_map; }
  error = 0;
  return location_map;
}


const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        *value += c;
        if (c == '\'') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '"') {
          return TOKEN_TYPE_NORMAL;
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL) {
        if (last_token_type != TOKEN_TYPE_NORMAL) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END && last_token_type != TOKEN_TYPE_START_BLOCK && last_token_type != TOKEN_TYPE_END_BLOCK) {
        // Error.
        break;
      }
      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK && last_token_type != TOKEN_TYPE_START) {
        // Error.
        break;
      }
      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  printf ("Bad transition from %s to %s\n",
          TokenTypeAsString(last_token_type),
          TokenTypeAsString(token_type));
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    printf ("Failed to open config file: %s\n", file_name);
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}