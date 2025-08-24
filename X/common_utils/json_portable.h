#ifndef JSON_PORTABLE_H
#define JSON_PORTABLE_H

#if __has_include(<json/json.h>)
  #include <json/json.h>
#elif __has_include(<jsoncpp/json/json.h>)
  #include <jsoncpp/json/json.h>
#elif __has_include(<json/json.h>) // fallback if -I/usr/include/jsoncpp is passed
  #include <json/json.h>
#else
  #error "JsonCpp headers not found! Install libjsoncpp-dev (Ubuntu/WSL), jsoncpp (brew/Fedora/Arch), or vcpkg (Windows)."
#endif

#endif // JSON_PORTABLE_H
