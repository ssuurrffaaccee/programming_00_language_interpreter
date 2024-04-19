#include "expression.hpp"
#include "type.hpp"
#include <string>
#include <unordered_map>
namespace module_helper {
struct Declaration {
  std::string name_;
  SPtr<Type> type_;
};
struct Interface {
  std::unordered_map<std::string, SPtr<Type>> decls_;
};
struct Implemntation {
  std::string name_;
  SPtr<Expression> exp_;
};
struct Body {
  std::string name_;
  std::unordered_map<std::string, SPtr<Expression>> impls_;
};
} // namespace module_helper
struct Module : public Expression {
  Module(){type_ = ExpressionType::MODULE_EXP} 
  std::string name_;
  std::unordered_map<std::string, std::pair<SPtr<Type>, SPtr<Expression>>>
      compoents_;
};
SPtr<Expression> module_(const std::string &name,
                         const SPtr<module_helper::Interface> interface,
                         const SPtr<module_helper::Body> &body) {
  Module *exp = new Module{};
  exp->name_ = name;
  return SPtr<Expression>{(Expression *)exp};
}

struct ImportExp : public Expression {
  ImportExp() { type_ = ExpressionType::IMPORT_EXP; }
  std::string model_name_;
  std::string component_name_;
};
// begin(
//    module_("m0",
//           interface(
//                     decl("u",int_type),
//                     decl("v",int_type)
//                    ),
//           body(
//               impl("u",const_(44)),
//               impl("v",const_(33)),
//               )
//           ),
//    module_("m1",
//           interface(
//                     decl("u",int_type),
//                     decl("v",int_type)
//                    ),
//           body(
//               impl("u",const_(55)),
//               impl("v",const_(66)),
//               impl("c",const_(2)),
//               impl("incr",proc("x",make_int_type(),add(var("x"),var("c"))))
//               )
//           ),
//     let("x",import_("m0","u"),
//         let("y",import_("m1","v"),
//            begin(
//               print(call(import_("m1","incr"),var("x"))),
//               print(call(import_("m1","incr"),var("y")))
//            )
//         )
//     )
// )