#include <iostream>

#include "value_of.hpp"

int main() {
  try {
    auto exp =
    let_rec("infinite_loop","x",
           call(var("infinite_loop"),diff(const_(1),var("x"))),
           let("f",proc("z",const_(11)),
              print(call(var("f"),call(var("infinite_loop"),const_(0))))
           )
    );
    SPtr<Environment> env = std::make_shared<Environment>();
    SPtr<Store> store = std::make_shared<Store>();
    auto res = value_of(exp,env,store);
    std::cout<<to_string(res)<<"\n";
  } catch (MyExceptoin& e) {
    std::cout << e.what() << '\n';
  }
  return 0;
}