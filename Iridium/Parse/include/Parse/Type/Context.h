#ifndef __IRIDIUM_TY_CONTEXT_H__
#define __IRIDIUM_TY_CONTEXT_H__

#include "Parse/Type/Type.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

namespace iridium {
namespace ty {
  class Context {
  public:
    Context();
    // if there are type errors, returns a vec of the error messages
    std::optional<std::vector<std::string>> TypeCheck();
  private:
    std::unordered_map<std::string, Type> m_Environment;
  };
}
}
#endif
