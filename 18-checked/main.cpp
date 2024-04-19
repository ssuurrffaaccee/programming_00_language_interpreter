#include "value_of.hpp"
#include "checker.hpp"
#include <iostream>

/*  
python:
def fib(n)
  if n -1 == 0:
     return 0
  else:
      if n -2 == 0:
          return 1
      else:
           return fib(n-1)+fib(n-2)
*/
int main(){
    try{
        auto exp =
        let_rec("fib","n",make_int_type(),make_int_type(),
            if_(is_zero(diff(var("n"),const_(0))),
                 const_(0),
                 if_(is_zero(diff(var("n"),const_(1))),
                     const_(1),
                     add(
                         call(var("fib"),
                               diff(var("n"),const_(1))),
                         call(var("fib"),
                               diff(var("n"),const_(2)))
                     )
                 )
            ),
            call(var("fib"),const_(10))
        );
        SPtr<TypeEnvironment> type_env = std::make_shared<TypeEnvironment>();
        auto final_type = type_of(exp,type_env);
        std::cout <<"type: "<< to_string(final_type) << "\n";
        SPtr<Environment> env = std::make_shared<Environment>();
        auto res = value_of(exp, env);
        std::cout <<"res: "<< to_string(res) << "\n";
    } catch (MyExceptoin &e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}