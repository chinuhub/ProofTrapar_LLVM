/**
 * @file Program.cpp
 *
 * @brief Implements Program class
 *
 * @author Yaduraj Rao
 */
#include "Program.h"

#include <iostream>
#include <map>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "z3++.h"
#include "llvm/Support/raw_ostream.h"

#define LOCAL_DEBUG

using namespace llvm;

namespace {
// An intermediate representation of basic blocks before creating their automata
struct BasicBlockGraph {
  bool has_phi = false;
  bool has_branch = false;
  bool is_accepting = false;

  int first_nd;
  int last_nd;

  // A vector of non phi and non branch instructions of block in order
  std::vector<int> inst;
  // A map from assume instructions to their target blocks
  std::map<int, int> branch_map;
  // A map from incoming blocks to their corresponding instruction
  std::map<int, int> phi_map;
  // A map from incoming blocks to their target states
  std::map<int, int> phi_aut_map;
};

AdjacencyList<int> CreateAutGraph(std::vector<BasicBlockGraph> BB) {
  // Edges have integer labels for there corresponding instructions (symbols of
  // our automata). In case of epsilon transitions this label is -1.
  std::vector<std::vector<std::pair<int, int> > > aut_graph;
  // 0 is the start state, 1 is the accepting state and 2 is a dead state
  for (int i = 0; i < 3; i++) {
    aut_graph.emplace_back();
  }
  for (BasicBlockGraph& bb : BB) {
    if (bb.is_accepting || !bb.has_branch) continue;
    bb.first_nd = static_cast<int>(aut_graph.size() + bb.phi_map.size());
    for (auto iter = bb.phi_map.begin(); iter != bb.phi_map.end(); iter++) {
      int phi_nd_num = static_cast<int>(aut_graph.size());
      aut_graph.emplace_back();
      aut_graph.back().push_back(
        std::make_pair(
          bb.first_nd,
          iter->second
        )
      );
      bb.phi_aut_map.insert(
        std::make_pair(
          iter->first,
          phi_nd_num
        )
      );
    }
    aut_graph.emplace_back();
    int prev_nd = bb.first_nd;
    for (unsigned i = 0; i < bb.inst.size(); i++) {
      int cur = static_cast<int>(aut_graph.size());
      aut_graph[prev_nd].push_back(std::make_pair(cur, bb.inst[i]));
      aut_graph.emplace_back();
      prev_nd = cur;
    }
    bb.last_nd = prev_nd;
  }
  for (unsigned i = 0; i < BB.size(); i++) {
    BasicBlockGraph& bb = BB[i];
    if (bb.is_accepting || !bb.has_branch) continue;
    int from = bb.last_nd;
    for (auto iter = bb.branch_map.begin(); iter != bb.branch_map.end(); iter++) {
      int to;
      BasicBlockGraph& target_block = BB[iter->second];
      if (target_block.is_accepting) {
        to = 1;
      } else if (!target_block.has_branch) {
        to = 2;
      } else if (!target_block.has_phi) {
        to = target_block.first_nd;
      } else {
        auto iter2 = target_block.phi_aut_map.find(i);
        assert(iter2 != target_block.phi_aut_map.end());
        to = iter2->second;
      }
      aut_graph[from].push_back(
        std::make_pair(
          to,
          iter->first
        )
      );
    }
  }
  assert(!BB[0].has_phi);
  aut_graph[0].push_back(std::make_pair(BB[0].first_nd, -1));
#ifdef LOCAL_DEBUG
  for (unsigned i = 0; i < aut_graph.size(); i++) {
    std::cout << "From State " << i << " :" << std::endl;
    for (unsigned j = 0; j < aut_graph[i].size(); j++) {
      char temp = 'A';
      if (aut_graph[i][j].second < 26) temp = 'A' + aut_graph[i][j].second;
      else temp = 'a' + (aut_graph[i][j].second - 26);
      std::cout << aut_graph[i][j].first << ' ' << temp;
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
#endif
  return aut_graph;
}

}

bool InstructionComparator::operator()(const InstructionType& lhs,
                                       const InstructionType& rhs) const {
  if (std::get<0>(lhs) == kAssign && std::get<0>(rhs) == kAssume) {
    return false;
  } else if (std::get<0>(lhs) == kAssume && std::get<0>(rhs) == kAssign) {
    return true;
  } else {
    if ((std::get<1>(lhs)).hash() == (std::get<1>(rhs)).hash()) {
      return std::get<2>(lhs).hash() < std::get<2>(rhs).hash();
    } else {
      return std::get<1>(lhs).hash() < std::get<1>(rhs).hash();
    }
  }
}

Program::Program(Module& M) {
  ParseGlobalVariables(M);
  for (Function& Func : M) {
    ParseThread(Func);
  }
  MakeOldInterface();
}

z3::expr& Program::GetVariableExpr(std::string name) {
  auto iter = variable_expr_map_.find(name);
  assert(iter != variable_expr_map_.end());
  return iter->second;
}

z3::expr Program::GetGlobalInit(std::string name) {
  auto iter = global_var_init_map_.find(name);
  assert(iter != global_var_init_map_.end());
  return iter->second;
}

void Program::ParseGlobalVariables(Module& M) {
#ifdef LOCAL_DEBUG
    std::cout << "parsing global variables now" << std::endl;
#endif
  int count = 0;
  for (GlobalVariable& Gvar : M.globals()) {
    if (const ConstantInt* CI = dyn_cast<ConstantInt>(Gvar.getInitializer())) {
      std::string name = Gvar.getName().str();
      int64_t value = CI->getSExtValue();
#ifdef LOCAL_DEBUG
        std::cout << name << " = " << value << std::endl;
#endif
      z3::expr rexp = context_.int_val(value);
      AddVariable(name);
      global_var_init_map_.insert(std::make_pair(name, rexp));

      // Support for old interface
      std::string symname = std::to_string(count);
      mAllSyms.push_back(symname);
      ++count;
      mInitString += symname;
      std::pair<z3::expr, z3::expr> pr =
      std::make_pair(
        GetVariableExpr(name),
        rexp
      );
      mRWLHRHMap.insert(std::make_pair(symname, pr));
    }
  }

}

void Program::ParseThread(Function& Func) {
  std::string thread_name = Func.getName().str();
  if (thread_name == "__ASSERT") return;
  thread_names_.push_back(thread_name);
  std::vector<BasicBlockGraph> bb_automata;
#ifdef LOCAL_DEBUG
    std::cout << "parsing function named " << thread_name << " now" << std::endl;
#endif
  std::map<std::string, int> block_map;
  for (BasicBlock& BB : Func) {
    std::string block_name = ValueToVariable(&BB, thread_name);
    block_map.insert(
      std::make_pair(
        block_name,
        static_cast<int>(bb_automata.size())
      )
    );
    bb_automata.emplace_back();
  }
  int cur_block = 0;
  for (BasicBlock& BB : Func) {
    BasicBlockGraph& bb_struct = bb_automata[cur_block++];
#ifdef LOCAL_DEBUG
    std::cout << "Entering Block " << ValueToVariable(&BB, thread_name);
    std::cout << " now" << std::endl;
#endif
    for (Instruction& Inst : BB) {
      switch (Inst.getOpcode()) {
        case Instruction::Call: {
          bb_struct.is_accepting = true;
#ifdef LOCAL_DEBUG
          std::cout << "Accepting State Here" << std::endl;
#endif
          break;
        }
        case Instruction::Ret: {
          bb_struct.has_branch = true;
          z3::expr cond_expr = context_.bool_val(true);
          InstructionType inst =
          std::make_tuple(
            kAssume,
            context_.int_val(0),
            cond_expr
          );
          unsigned inst_num = FindInstruction(inst);
          bb_struct.branch_map.insert(
            std::make_pair(
              inst_num,
              0
            )
          );
          if (inst_num == inst_list_.size()) {
            inst_list_.push_back(inst);
            inst_map_.insert(
              std::make_pair(
                inst,
                inst_num
              )
            );
          }
#ifdef LOCAL_DEBUG
          std::cout << inst_num << ": Unconditional Branch Instruction";
          std::cout << "(Return Tranformed to Branch) to block 0" << std::endl;
#endif
          break;
        }
        case Instruction::Br: {
          bb_struct.has_branch = true;
          BranchInst* br_inst = dyn_cast<BranchInst>(&Inst);
          if (br_inst->isUnconditional()) {
            z3::expr cond_expr = context_.bool_val(true);
            InstructionType inst =
            std::make_tuple(
              kAssume,
              context_.int_val(0),
              cond_expr
            );
            unsigned inst_num = FindInstruction(inst);
            BasicBlock* bb_next = br_inst->getSuccessor(0);
            std::string bb_name =
                         ValueToVariable(dyn_cast<Value>(bb_next), thread_name);
            auto iter = block_map.find(bb_name);
            assert(iter != block_map.end());
            bb_struct.branch_map.insert(
              std::make_pair(
                inst_num,
                iter->second
              )
            );
            // if it is a new instruction insert in the instruction list
            if (inst_num == inst_list_.size()) {
              inst_list_.push_back(inst);
              inst_map_.insert(
                std::make_pair(
                  inst,
                  inst_num
                )
              );
            }
#ifdef LOCAL_DEBUG
            std::cout << inst_num << ": Unconditional Branch Instruction to ";
            std::cout << "block " << iter->second << std::endl;
#endif
          } else {
            Value* v_cond = br_inst->getCondition();
            z3::expr cond_expr =
                        ValueToExpr(v_cond, thread_name) != context_.int_val(0);
            InstructionType inst =
            std::make_tuple(
              kAssume,
              context_.int_val(0),
              cond_expr
            );
            unsigned inst_num = FindInstruction(inst);
            BasicBlock* bb_next = br_inst->getSuccessor(0);
            std::string bb_name =
                         ValueToVariable(dyn_cast<Value>(bb_next), thread_name);
            auto iter = block_map.find(bb_name);
            assert(iter != block_map.end());
            bb_struct.branch_map.insert(
              std::make_pair(
                inst_num,
                iter->second
              )
            );
            if (inst_num == inst_list_.size()) {
              inst_list_.push_back(inst);
              inst_map_.insert(
                std::make_pair(
                  inst,
                  inst_num
                )
              );
            }
#ifdef LOCAL_DEBUG
            std::cout << inst_num << ": Conditional Branch Instruction";
            std::cout << "\nIf " << ValueToVariable(v_cond, thread_name);
            std::cout << " then go to " << "block " << iter->second;
            std::cout << std::endl;
#endif
            cond_expr = (cond_expr == context_.bool_val(false));
            inst =
            std::make_tuple(
              kAssume,
              context_.int_val(0),
              cond_expr
            );
            inst_num = FindInstruction(inst);
            bb_next = br_inst->getSuccessor(1);
            bb_name = ValueToVariable(dyn_cast<Value>(bb_next), thread_name);
            iter = block_map.find(bb_name);
            assert(iter != block_map.end());
            bb_struct.branch_map.insert(
              std::make_pair(
                inst_num,
                iter->second
              )
            );
            if (inst_num == inst_list_.size()) {
              inst_list_.push_back(inst);
              inst_map_.insert(
                std::make_pair(
                  inst,
                  inst_num
                )
              );
            }
#ifdef LOCAL_DEBUG
            std::cout << inst_num << ": If not ";
            std::cout << ValueToVariable(v_cond, thread_name);
            std::cout << " then go to " << "block " << iter->second ;
            std::cout << std::endl;
#endif
          }
          break;
        }
        case Instruction::PHI: {
          bb_struct.has_phi = true;
          std::string lval_operand = ValueToVariable(&Inst, thread_name);
          AddVariable(lval_operand);
          z3::expr lval_expr = GetVariableExpr(lval_operand);
#ifdef LOCAL_DEBUG
          std::cout << "Caught a PHI one here: " << std::endl;
#endif
          PHINode* phi_node = dyn_cast<PHINode>(&Inst);
          std::vector<int> phi_inst_nums;
          std::vector<int> phi_inc_blocks;
          for (Use& U : phi_node->incoming_values()) {
            Value* v = U.get();
            z3::expr rhs_expr = ValueToExpr(v, thread_name);
            InstructionType inst =
            std::make_tuple(
              kAssign,
              lval_expr,
              rhs_expr
            );
            unsigned inst_num = FindInstruction(inst);
            phi_inst_nums.push_back(inst_num);
            if (inst_num == inst_list_.size()) {
              inst_list_.push_back(inst);
              inst_map_.insert(
                std::make_pair(
                  inst,
                  inst_num
                )
              );
            }
#ifdef LOCAL_DEBUG
            std::cout << inst_num << ": " << lval_operand << " = ";
            std::cout << ValueToVariable(v, thread_name) << std::endl;
#endif
          }
          for (BasicBlock* inc_bb : phi_node->blocks()) {
            std::string bb_name = ValueToVariable(inc_bb, thread_name);
            auto iter = block_map.find(bb_name);
            assert(iter != block_map.end());
            phi_inc_blocks.push_back(iter->second);
#ifdef LOCAL_DEBUG
            std::cout << "From block " << bb_name << std::endl;
#endif
          }
          for (unsigned i = 0; i < phi_inst_nums.size(); i++) {
            auto iter = bb_struct.phi_map.find(phi_inc_blocks[i]);
            if (iter == bb_struct.phi_map.end()) {
              bb_struct.phi_map.insert(
                std::make_pair(
                  phi_inc_blocks[i],
                  phi_inst_nums[i]
                )
              );
            }
          }
          break;
        }
        case Instruction::Load: {
          std::string lval_operand = ValueToVariable(&Inst, thread_name);
          AddVariable(lval_operand);
          z3::expr lval_expr = GetVariableExpr(lval_operand);
          Value* rf_val = Inst.getOperand(0);
          z3::expr rhs_expr = ValueToExpr(rf_val, thread_name);
          InstructionType inst =
          std::make_tuple(
            kAssign,
            lval_expr,
            rhs_expr
          );
          unsigned inst_num = FindInstruction(inst);
          bb_struct.inst.push_back(inst_num);
          if (inst_num == inst_list_.size()) {
            inst_list_.push_back(inst);
            inst_map_.insert(
              std::make_pair(
                inst,
                inst_num
              )
            );
          }
#ifdef LOCAL_DEBUG
          std::cout << inst_num << ": " << lval_operand << " = ";
          std::cout << ValueToVariable(rf_val, thread_name) << std::endl;
#endif
          break;
        }
        case Instruction::Store: {
          Value* lhs = Inst.getOperand(1);
          std::string lval_operand = ValueToVariable(lhs, thread_name);
          AddVariable(lval_operand);
          z3::expr lval_expr = GetVariableExpr(lval_operand);
          Value* rhs = Inst.getOperand(0);
          z3::expr rhs_expr = ValueToExpr(rhs, thread_name);
          InstructionType inst =
          std::make_tuple(
            kAssign,
            lval_expr,
            rhs_expr
          );
          unsigned inst_num = FindInstruction(inst);
          bb_struct.inst.push_back(inst_num);
          if (inst_num == inst_list_.size()) {
            inst_list_.push_back(inst);
            inst_map_.insert(
              std::make_pair(
                inst,
                inst_num
              )
            );
          }
#ifdef LOCAL_DEBUG
          std::cout << inst_num << ": " << lval_operand << " = ";
          std::cout << ValueToVariable(rhs, thread_name) << std::endl;
#endif
          break;
        }
        case Instruction::Add:
        case Instruction::Sub:
        case Instruction::Mul:
        case Instruction::ICmp: {
          std::string lval_operand = ValueToVariable(&Inst, thread_name);
          AddVariable(lval_operand);
          z3::expr lval_expr = GetVariableExpr(lval_operand);
          Value* op1 = Inst.getOperand(0);
          Value* op2 = Inst.getOperand(1);
          z3::expr op1_expr = ValueToExpr(op1, thread_name);
          z3::expr op2_expr = ValueToExpr(op2, thread_name);
          z3::expr rhs_expr = op1_expr + op2_expr;
#ifdef LOCAL_DEBUG
          std::cout << lval_operand << " = ";
          std::cout << ValueToVariable(op1, thread_name) << " ";
#endif
          switch (Inst.getOpcode()) {
            case Instruction::Add:
              rhs_expr = op1_expr + op2_expr;
#ifdef LOCAL_DEBUG
              std::cout << "+ ";
#endif
              break;
            case Instruction::Sub:
              rhs_expr = op1_expr - op2_expr;
#ifdef LOCAL_DEBUG
              std::cout << "- ";
#endif
              break;
            case Instruction::Mul:
              rhs_expr = op1_expr * op2_expr;
#ifdef LOCAL_DEBUG
              std::cout << "* ";
#endif
            case Instruction::ICmp: {
              CmpInst* cmpInst = dyn_cast<CmpInst>(&Inst);
              switch (cmpInst->getPredicate()) {
                case CmpInst::ICMP_EQ:
                  rhs_expr = z3::ite(
                                    op1_expr == op2_expr,
                                    context_.int_val(1),
                                    context_.int_val(0)
                                    );
#ifdef LOCAL_DEBUG
                  std::cout << "== ";
#endif
                  break;
                case CmpInst::ICMP_NE:
                  rhs_expr = z3::ite(
                                    op1_expr != op2_expr,
                                    context_.int_val(1),
                                    context_.int_val(0)
                                    );
#ifdef LOCAL_DEBUG
                  std::cout << "!= ";
#endif
                  break;
                case CmpInst::ICMP_SGT:
                  rhs_expr = z3::ite(
                                    op1_expr > op2_expr,
                                    context_.int_val(1),
                                    context_.int_val(0)
                                    );
#ifdef LOCAL_DEBUG
                  std::cout << "> ";
#endif
                  break;
                case CmpInst::ICMP_SGE:
                  rhs_expr = z3::ite(
                                    op1_expr >= op2_expr,
                                    context_.int_val(1),
                                    context_.int_val(0)
                                    );
#ifdef LOCAL_DEBUG
                  std::cout << ">= ";
#endif
                  break;
                case CmpInst::ICMP_SLT:
                  rhs_expr = z3::ite(
                                    op1_expr < op2_expr,
                                    context_.int_val(1),
                                    context_.int_val(0)
                                    );
#ifdef LOCAL_DEBUG
                  std::cout << "< ";
#endif
                  break;
                case CmpInst::ICMP_SLE:
                  rhs_expr = z3::ite(
                                    op1_expr <= op2_expr,
                                    context_.int_val(1),
                                    context_.int_val(0)
                                    );
#ifdef LOCAL_DEBUG
                  std::cout << "<= ";
#endif
                  break;
                default:
#ifdef LOCAL_DEBUG
                  std::cout << "Error: ICmp predicte: ";
                  std::cout << cmpInst->getPredicate() << " not handled";
                  std::cout << std::endl;
#endif
                  assert(false);
              }
            }
          }
          InstructionType inst =
          std::make_tuple(
            kAssign,
            lval_expr,
            rhs_expr
          );
          unsigned inst_num = FindInstruction(inst);
          bb_struct.inst.push_back(inst_num);
          if (inst_num == inst_list_.size()) {
            inst_list_.push_back(inst);
            inst_map_.insert(
              std::make_pair(
                inst,
                inst_num
              )
            );
          }
#ifdef LOCAL_DEBUG
          std::cout << ValueToVariable(op2, thread_name) << " :" << inst_num;
          std::cout << std::endl;
#endif
          break;
        }
        case Instruction::Alloca:
        case Instruction::Unreachable: {
          break;
        }
        default: {
#ifdef LOCAL_DEBUG
          std::cout << "Unhandled: " << Inst.getOpcodeName() << std::endl;
#endif
        }
      }
    }
  }

#ifdef LOCAL_DEBUG
  std::cout << "Total Instructions: " << inst_list_.size() << " ";
  std::cout << inst_map_.size() << std::endl;
#endif

  AdjacencyList<int> aut_graph = CreateAutGraph(bb_automata);
  thread_graphs_.push_back(aut_graph);
}

unsigned Program::FindInstruction(const InstructionType& inst) {
  return inst_list_.size();
  // Following code may be conficting with backend design
  auto iter = inst_map_.find(inst);
  if (iter == inst_map_.end()) {
    return inst_list_.size();
  } else {
    return iter->second;
  }
}

std::string Program::ValueToVariable(const Value* v, std::string scope) {
  std::string name;
  raw_string_ostream name_stream(name);
  v->printAsOperand(name_stream, false);
  if (name[0] == '@') {                                                         // a global variable
    return name.substr(1, name.size() - 1);
  } else if (name[0] == '%') {
    return scope + "::" + name.substr(1, name.size() - 1);
  } else {
    return name;
  }
}

z3::expr Program::ValueToExpr(const Value* v, std::string scope) {
  if (const ConstantInt* CI = dyn_cast<ConstantInt>(v)) {
    int64_t val = CI->getSExtValue();
    return context_.int_val(val);
  } else {
    std::string name = ValueToVariable(v, scope);
    return GetVariableExpr(name);
  }
}

bool Program::AddVariable(std::string name) {
  if (variable_expr_map_.find(name) != variable_expr_map_.end()) {
    return false;
  } else {
    z3::expr exp = context_.int_const(name.c_str());
    variable_expr_map_.insert(std::make_pair(name, exp));
    return true;
  }
}


std::vector<AdjacencyList<int> > Program::GetAutomata() {
  return thread_graphs_;
}

//==============================================================================
// Methods to transform to old interface
//==============================================================================
bool z3comparator::operator()(const z3::expr& lhs, const z3::expr& rhs) const {
  return (lhs.hash() < rhs.hash());
}

std::vector<std::string> Program::GetRegexOfAllProcesses() {
  return mProcessesregex;
}

std::map<std::string, z3::expr>& Program::GetAssnMapForAllProcesses() {
  return mAssnMap;
}

void Program::MakeOldInterface() {
  mVarExprMap = variable_expr_map_;
  // assert that we don't have more instructions than symbols available
  assert(inst_list_.size() + thread_names_.size() <= 52);
  unsigned i = 0;
  for (i = 0; i < inst_list_.size(); i++) {
    std::string sym;
    if (i < 26) sym += ('A' + i);
    else sym += ('a' + i - 26);
    mAllSyms.push_back(sym);
    if (std::get<0>(inst_list_[i]) == kAssign) {
      std::tuple<z3::expr, z3::expr> inst =
      std::make_tuple(
        std::get<1>(inst_list_[i]),
        std::get<2>(inst_list_[i])
      );
      mRWLHRHMap.insert(
        std::make_pair(
          sym,
          inst
        )
      );
    } else if (std::get<0>(inst_list_[i]) == kAssume) {
      z3::expr cond_expr = std::get<2>(inst_list_[i]);
      mAssumeLHRHMap.insert(
        std::make_pair(
          sym,
          cond_expr
        )
      );
      mRevAssumeLHRHMap.insert(
        std::make_pair(
          cond_expr,
          sym
        )
      );
    }
  }
  for (unsigned j = 0; j < thread_names_.size(); i++, j++) {
    std::string sym;
    if (i < 26) sym += ('A' + i);
    else sym += ('a' + i - 26);
    z3::expr assert_expr = context_.bool_val(false);
    mAssnMap.insert(
      std::make_pair(
        sym,
        assert_expr
      )
    );
  }
#ifdef LOCAL_DEBUG
  // Peterson's Example Regex
  mProcessesregex.push_back("ABC(DE(GK|FHIJL)MO)*(DE(GK|FHIJL))NPQR(SVWABC(DE(GK|FHIJL)MO)*(DE(GK|FHIJL))NPQR)*T");
  mProcessesregex.push_back("XYZ(ab(dh|cefgi)jl)*(ab(dh|cefgi))kmno(pstXYZ(ab(dh|cefgi)jl)*(ab(dh|cefgi))kmno)*q");
#endif
}
