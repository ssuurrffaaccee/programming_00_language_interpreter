#include "value_of.hpp"
#include <iostream>

int main(){
    try{
        {
            auto exp =
            let("x",const_(0),
                begin(
                    set("x",const_(2)),
                    print(var("x")),
                    set("x",const_(3)),
                    print(var("x"))
                )
            );
            SPtr<Environment> env = std::make_shared<Environment>();
            auto res = value_of(exp, env);
            std::cout << to_string(res) << "\n"; 
        }
        std::cout<<"============================\n";
        {
            auto exp =
            let("sum",proc({"x","y"},add(var("x"),var("y"))),
                let("x",const_(1),
                    let("y",const_(2),
                       print(call(var("sum"),{var("x"),var("y")}))
                    )
                )
            );
            SPtr<Environment> env = std::make_shared<Environment>();
            auto res = value_of(exp, env);
            std::cout << to_string(res) << "\n"; 
        }
        std::cout<<"============================\n";
        {
            auto exp =
            let("three",proc({},const_(3)),
                    let("y",const_(2),
                       print(add(call(var("three"),{}),var("y")))
                    )
            );
            SPtr<Environment> env = std::make_shared<Environment>();
            auto res = value_of(exp, env);
            std::cout << to_string(res) << "\n"; 
        }
        std::cout<<"============================\n";
        {
            auto exp =
            let("object",class_(),
                let("sum",class_(super("object"),
                                   field("x"),
                                   field("y"),
                                   method("constructor",begin(set("x",const_(0)),set("y",const_(0)))),
                                   method("setx",{"x_"},set("x",var("x_"))),
                                   method("sety",{"y_"},set("y",var("y_"))),
                                   method("call",{},add(var("x"),var("y")))
                                 ),
                           begin(
                              print(var("sum")),
                              print(new_("sum"))
                           )
                   )
            );
            SPtr<Environment> env = std::make_shared<Environment>();
            auto res = value_of(exp, env);
            std::cout << to_string(res) << "\n";
        }
        std::cout<<"============================\n";
        {
            auto exp =
            let("object",class_(),
                let("sum",class_(super("object"),
                                   field("x"),
                                   field("y"),
                                   method("constructor",begin(set("x",const_(0)),set("y",const_(0)))),
                                   method("setx",{"x_"},set("x",var("x_"))),
                                   method("sety",{"y_"},set("y",var("y_"))),
                                   method("set",{"x_","y_"},begin(send("@this","setx",{var("x_")}),send("@this","sety",{var("y_")}))),
                                   method("call",{},add(var("x"),var("y")))
                                 ),
                          let("sum_obj", new_("sum"),
                                        begin(
                                              send("sum_obj","setx",{const_(1)}),
                                              send("sum_obj","sety",{const_(2)}),
                                              print(send("sum_obj","call",{})),
                                              send("sum_obj","sety",{const_(4)}),
                                              print(send("sum_obj","call",{})),
                                              send("sum_obj","set",{const_(4),const_(7)}),
                                              print(send("sum_obj","call",{}))
                                             )
                             )
                   )
            );
            SPtr<Environment> env = std::make_shared<Environment>();
            auto res = value_of(exp, env);
            std::cout << to_string(res) << "\n";
        }
        std::cout<<"============================\n";
        {
            auto exp =
            let("object",class_(super("object"),method("get_id",{},const_(1))),
                let("sum",class_(super("object"),
                                   field("x"),
                                   field("y"),
                                   method("constructor",begin(set("x",const_(0)),set("y",const_(0)))),
                                   method("setx",{"x_"},set("x",var("x_"))),
                                   method("sety",{"y_"},set("y",var("y_"))),
                                   method("set",{"x_","y_"},begin(send("@this","setx",{var("x_")}),send("@this","sety",{var("y_")}))),
                                   method("call",{},add(var("x"),var("y")))
                                 ),
                          let("sum_obj", new_("sum"),
                                        begin(
                                              send("sum_obj","setx",{const_(1)}),
                                              send("sum_obj","sety",{const_(2)}),
                                              print(send("sum_obj","call",{})),
                                              send("sum_obj","sety",{const_(4)}),
                                              print(send("sum_obj","call",{})),
                                              send("sum_obj","set",{const_(4),const_(7)}),
                                              print(send("sum_obj","call",{})),
                                              print(send("sum_obj","get_id",{}))
                                             )
                             )
                   )
            );
            SPtr<Environment> env = std::make_shared<Environment>();
            auto res = value_of(exp, env);
            std::cout << to_string(res) << "\n";
        }
        std::cout<<"============================\n";
        {
            auto exp =
            let("object",class_(super("object"),method("get_id",{},const_(1))),
                let("sum",class_(super("object"),
                                   field("x"),
                                   field("y"),
                                   method("constructor",begin(set("x",const_(0)),set("y",const_(0)))),
                                   method("setx",{"x_"},set("x",var("x_"))),
                                   method("sety",{"y_"},set("y",var("y_"))),
                                   method("set",{"x_","y_"},begin(send("@this","setx",{var("x_")}),send("@this","sety",{var("y_")}))),
                                   method("call",{},add(var("x"),var("y"))),
                                   method("get_id",{},const_(2))
                                 ),
                          let("sum_obj", new_("sum"),
                                        begin(
                                              send("sum_obj","setx",{const_(1)}),
                                              send("sum_obj","sety",{const_(2)}),
                                              print(send("sum_obj","call",{})),
                                              send("sum_obj","sety",{const_(4)}),
                                              print(send("sum_obj","call",{})),
                                              send("sum_obj","set",{const_(4),const_(7)}),
                                              print(send("sum_obj","call",{})),
                                              print(send("sum_obj","get_id",{}))
                                             )
                             )
                   )
            );
            SPtr<Environment> env = std::make_shared<Environment>();
            auto res = value_of(exp, env);
            std::cout << to_string(res) << "\n";
        }
    } catch (MyExceptoin &e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}