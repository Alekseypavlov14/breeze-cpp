#pragma once

namespace Shared {
  class Classes {
    public:
      // utility that checks instantiation
      template<class Candidate, class Target>
      static bool isInstanceOf(Candidate* instance) {
        // map candidate to target => the candidate is derived from target 
        return dynamic_cast<Target*>(instance) != nullptr;
      }
    
      // casts value type
      template<class Current, class Target>
      static Target* cast(Current* value) {
        return dynamic_cast<Target*>(value);
      }
  };
}
