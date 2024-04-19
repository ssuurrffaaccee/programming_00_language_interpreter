#include <iostream>

#include "value_of.hpp"
int main() {
  try {
    auto exp =
    let("x", mk_pair(const_(1),const_(2)),
      begin(
        print(var("x")),
        print(left(var("x"))),
        print(right(var("x"))),
        
        set_left(var("x"),const_(3)),
        set_right(var("x"),const_(4)),
        print(var("x")),

        set_left(var("x"),proc("n",add(var("n"),var("n")))),
        print(var("x")),
        print(call(left(var("x")),const_(4)))
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