#include "value_of.hpp"
#include <iostream>
int main(){
    try{
    auto exp =
    let("x",const_(10),
      let("y",const_(2),
        diff(var("x"),var("y"))
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