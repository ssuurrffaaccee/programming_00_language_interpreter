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
        {
           auto exp =
           begin(
               spawn(proc("dummy",
                    begin( 
                        print(const_(1)),
                        yield(),
                        print(const_(3)),
                        yield(),
                        print(const_(5))))),
               spawn(proc("dummy",
                    begin(
                        print(const_(2)),
                        yield(),
                        print(const_(4)),
                        yield(),
                        print(const_(6)))))
           );
           SPtr<Environment> env = std::make_shared<Environment>();
           SPtr<Continuation> end_cont = make_end_cont();
           auto res = value_of_cont(exp, env, end_cont);
           std::cout << to_string(res) << "\n";
        }
    } catch (MyExceptoin &e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}