/**
 * @file Program.h
 *
 * @brief Declares Program class
 *
 * @author Yaduraj Rao
 */
#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <vector>
#include <map>

#include "llvm/IR/Module.h"
#include "z3++.h"

template <typename T>
using adjacency_list = std::vector<std::vector<std::pair<int, T> > >;

/// An enum type for Instruction types. Instruction fall into either assign or
/// assume types.
enum InstType {
  kAssign,
  kAssume
};

/**
 * A program class as a representation of the llvm module
 *
 * Program will parse every function (thread) present in the code and create an
 * adjacency list representation of its automata. Program has a representation
 * for variables and instructions of the module. Variables and instructions are
 * stored as z3prover expressions. All variables are stored as z3prover integer
 * expressions. Instructions fall into either of the following categories:
 * assign instructions, where an lval operand is assigned a value and assume
 * instructions where a boolean z3 expression is evaluated as true to progress.
 * Instructions are numbered from 0 incrementally and data is stored in vectors.
 * Global variables and there initial values are stored seperately in a map.
 *
 * @author Yaduraj Rao
 */
class Program {
 public:
  /**
   * Default constructor for Program Class
   *
   * Constructor takes an llvm module and processes it into its class variables.
   * Constructor extracts global variables and then proceeds through parsing
   * every thread in the module and creating its corresponding automata
   * representation as an adjacency list.
   *
   * @param M llvm Module to parse into Program
   */
  explicit Program(llvm::Module& M);

  /**
   * A method that takes a variable as string and returns its z3 expression.
   *
   * @param name Name of the variable to convert
   * @return reference to z3 expression corresponding to the variable
   */
  z3::expr& GetVariableExpr(std::string name);

  /**
   * A method that takes a global variable and return its initializer expr.
   *
   * @param name Name of the global variable
   * @return z3 expression of its initializer
   */
  z3::expr GetGlobalInit(std::string name);

 private:
  z3::context context_;

  // Mapping from global variable names to their intial values' z3 expr
  std::map<std::string, z3::expr> global_var_init_map_;
  // Mapping from variable names to their corresponding z3 expr
  std::map<std::string, z3::expr> variable_expr_map_;

  // A vector of lval operands of all instructions. If it is an assume
  // instruction this value is set to "assume".
  std::vector<std::string> inst_lval_operands_;
  // A vector of z3 expressions of all instructions. In case of assume
  // instruction this is a boolean z3 expression.
  std::vector<z3::expr> inst_exprs_;
  std::vector<InstType> inst_types_;

  std::vector<std::string> thread_names_;
  std::vector<adjacency_list<int> > thread_graphs_;

  void ParseGlobalVariables(llvm::Module&);
  void ParseThread(llvm::Function&);

  // Returns a distinct string name for an llvm Value in function named scope
  std::string ValueToVariable(const llvm::Value*, std::string scope);

  // Returns the z3 expression corresponding to an llvm Value
  z3::expr ValueToExpr(const llvm::Value*, std::string scope);

  bool AddVariable(std::string name);
};

#endif
