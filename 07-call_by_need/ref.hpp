#pragma once
#include <string>
struct Ref{
  int index_;
};
std::string to_string(const Ref &v){
  return std::string("REF:") + std::to_string(v.index_);
}