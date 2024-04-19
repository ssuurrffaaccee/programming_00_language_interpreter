#include "value_of.hpp"
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
        let_rec("fib","n",
            if_(is_zero(diff(var("n"),const_(0))),
                 const_(0),
                 if_(is_zero(diff(var("n"),const_(1))),
                     const_(1),
                     add(
                         call(var("fib"),
                               print(diff(var("n"),const_(1)))),
                         call(var("fib"),
                               diff(var("n"),const_(2)))
                     )
                 )
            ),
            call(var("fib"),const_(10))
        );
        SPtr<Environment> env = std::make_shared<Environment>();
        ContStack cont_stack;
        cont_stack.push(make_exp_cont(exp,env));
        auto res = value_of_cont(cont_stack);
        std::cout << to_string(res) << "\n";
    } catch (MyExceptoin &e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}