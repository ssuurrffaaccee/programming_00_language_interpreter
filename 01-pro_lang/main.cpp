#include "value_of.hpp"
#include <iostream>
int main(){
    try{
    auto exp =
    let("double",
       proc("x",
          add(
              var("x"),
              var("x")
          )
       ),
       call(var("double"),
           call(var("double"),
                const_(2)
           )
       )
    );
    SPtr<Environment> env = std::make_shared<Environment>();
    auto res = value_of(exp,env);
    std::cout<<to_string(res)<<"\n";
    }catch(MyExceptoin& e){
        std::cout<<e.what()<<'\n';
    }
    return 0;
}