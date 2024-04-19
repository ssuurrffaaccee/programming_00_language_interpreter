#include "value_of.hpp"
#include <iostream>
int main(){
    try{
    auto exp =
    let("x", new_ref(const_(0)),
      begin(
        set_ref(var("x"),
               add(deref(var("x")),
                   const_(2)
               )
        ),
        set_ref(var("x"),
               add(deref(var("x")),
                   const_(2)
               )
        ),
        set_ref(var("x"),
               add(deref(var("x")),
                   const_(2)
               )
        ),
        deref(var("x"))
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