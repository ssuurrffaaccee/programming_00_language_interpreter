#include "value_of.hpp"
#include <iostream>
int main(){
    try{
    auto exp =
    let("x", const_(0),
      begin(
        assign("x",
               add(var("x"),
                   const_(2)
               )
        ),
        assign("x",
               add(var("x"),
                   const_(2)
               )
        ),
        assign("x",
               add(var("x"),
                   const_(2)
               )
        ),
        print(var("x"))
      )
    );
    SPtr<Environment> env = std::make_shared<Environment>();
    SPtr<Store> store = std::make_shared<Store>();
    auto res = value_of(exp,env,store);
    std::cout<<to_string(res)<<"\n";
    }catch(MyExceptoin& e){
        std::cout<<e.what()<<'\n';
    }
    return 0;
}