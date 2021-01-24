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

/// An enum type for Instruction types. Instruction fall into either assign or
/// assume types.
enum InstClass {
  kAssign,
  kAssume
};

template <typename T>
using AdjacencyList = std::vector<std::vector<std::pair<int, T> > >;

using InstructionType = std::tuple<InstClass, z3::expr, z3::expr>;

struct InstructionComparator {
  bool operator()(const std::pair<std::string, InstructionType>& lhs,
                  const std::pair<std::string, InstructionType>& rhs) const;
};

struct z3comparator {
  bool operator()(const z3::expr& lhs, const z3::expr& rhs) const;
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

  //============================================================================
  // Old interface imported here. Maybe change this to a proper api interface in
  // future if possible.
  //============================================================================
  std::string mInitString;
  z3::context context_;
  std::map<std::string, z3::expr> mVarExprMap;
  std::map<std::string, std::tuple<z3::expr, z3::expr> > mRWLHRHMap;
  std::map<std::string, z3::expr> mAssumeLHRHMap;
  std::map<z3::expr, std::string, z3comparator> mRevAssumeLHRHMap;
  std::map<std::string, std::tuple<z3::expr, z3::expr, z3::expr> > mCASLHRHMap;
  std::map<std::string, z3::expr> mAssnMap;
  std::vector<std::string> mAllSyms;

  std::map<std::string, z3::expr>& GetAssnMapForAllProcesses();
  /**
   * A method to obtain thread_graphs_
   */
  std::vector<AdjacencyList<int> > GetAutomata();
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
  // z3::context context_;

  // Mapping from global variable names to their intial values' z3 expr
  std::map<std::string, z3::expr> global_var_init_map_;
  // Mapping from variable names to their corresponding z3 expr
  std::map<std::string, z3::expr> variable_expr_map_;

  std::map<std::pair<std::string, InstructionType>,
                                               int,
                                               InstructionComparator> inst_map_;
  std::vector<InstructionType> inst_list_;

  std::vector<std::string> thread_names_;
  std::vector<AdjacencyList<int> > thread_graphs_;

  void ParseGlobalVariables(llvm::Module&);
  void ParseThread(llvm::Function&);
  // Finds and returns an instruction if present in our vector
  unsigned FindInstruction(std::pair<std::string, InstructionType>);
  // Returns a distinct string name for an llvm Value in function named scope
  std::string ValueToVariable(const llvm::Value*, std::string scope);
  // Returns the z3 expression corresponding to an llvm Value
  z3::expr ValueToExpr(const llvm::Value*, std::string scope);
  bool AddVariable(std::string name);

  void MakeOldInterface();
};

#endif
