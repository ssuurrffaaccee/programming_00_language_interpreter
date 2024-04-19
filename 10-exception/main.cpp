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
        let("p",
             proc("n",
                    try_(
                        if_(is_zero(diff(var("n"),const_(2))),
                            raise(str("two")),
                            var("n")
                         ),"e",
                         print(diff(var("n"),const_(1)))
                        )
                  ),
             call(var("p"),const_(2))
        );
        // try_(raise(str("exception")),"x",print(var("x")));
        // let_rec("fib","n",
        //     if_(is_zero(diff(var("n"),const_(0))),
        //          const_(0),
        //          if_(is_zero(diff(var("n"),const_(1))),
        //              const_(1),
        //              add(
        //                  call(var("fib"),
        //                        diff(var("n"),const_(1))),
        //                  call(var("fib"),
        //                        diff(var("n"),const_(2)))
        //              )
        //          )
        //     ),
        //     call(var("fib"),const_(10)) //stackoverflow
        // );
        SPtr<Environment> env = std::make_shared<Environment>();
        SPtr<Continuation> end_cont = make_end_cont();
        auto res = value_of_cont(exp, env, end_cont);
        std::cout << to_string(res) << "\n";
    } catch (MyExceptoin &e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}