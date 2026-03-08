#pragma once

#include <curl/curl.h>
#include <string>
#include "parser.hpp"
 std::string postRequest(const std::string & url, const std::string & data) ;
  std::string getRequest(const std::string & url) ;
 size_t HttpCallback(void * contents, size_t size, size_t number, std::string * result);