#include "translation_of.hpp"
#include "nameless_value_of.hpp"
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
    SPtr<StaticEnvironment> static_env = std::make_shared<StaticEnvironment>();
    auto nameless_exp = translation_of(exp,static_env);
    SPtr<NamelessEnvironment> nameless_env = std::make_shared<NamelessEnvironment>();
    auto res = value_of(nameless_exp,nameless_env);
    std::cout<<to_string(res)<<'\n';
    }catch(MyExceptoin& e){
        std::cout<<e.what()<<'\n';
    }
    return 0;
}