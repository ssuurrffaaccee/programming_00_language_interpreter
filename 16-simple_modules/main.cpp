#include "value_of.hpp"
#include "checker.hpp"
#include <iostream>

int main(){
    try{
        auto exp =
            begin(
               module_("m0",
                      interface({
                                decl("u",make_int_type()),
                                decl("v",make_int_type())
                                }),
                      body({
                          impl("u",const_(44)),
                          impl("v",const_(33)),
                          })
                      ),
               module_("m1",
                      interface({
                                decl("u",make_int_type()),
                                decl("v",make_int_type()),
                                decl("incr",make_proc_type(make_int_type(),make_int_type()))
                                }),
                      body({
                          impl("u",const_(55)),
                          impl("v",const_(66)),
                          impl("c",const_(2)),
                          impl("incr",proc("x",make_int_type(),add(var("x"),var("c"))))
                          })
                      ),
                let("x",import_("m0","u"),
                    let("y",import_("m1","v"),
                       begin(
                          print(call(import_("m1","incr"),var("x"))),
                          print(call(import_("m1","incr"),var("y")))
                       )
                    )
                )
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