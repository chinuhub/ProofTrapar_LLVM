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

adjacency_list<int> CreateAutGraph(std::vector<BasicBlockGraph> BB) {
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
      std::cout << aut_graph[i][j].first << ' ' << aut_graph[i][j].second;
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
#endif
  return aut_graph;
}

}

Program::Program(Module& M) {
  ParseGlobalVariables(M);
  for (Function& Func : M) {
    ParseThread(Func);
  }
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
          inst_types_.push_back(kAssume);
          inst_lval_operands_.push_back("assume");
          z3::expr cond_expr = context_.bool_val(true);
          bb_struct.branch_map.insert(
            std::make_pair(
              static_cast<int>(inst_exprs_.size()),
              0
            )
          );
          inst_exprs_.push_back(cond_expr);
          break;
        }
        case Instruction::Br: {
          bb_struct.has_branch = true;
          BranchInst* br_inst = dyn_cast<BranchInst>(&Inst);
          if (br_inst->isUnconditional()) {
            inst_types_.push_back(kAssume);
            inst_lval_operands_.push_back("assume");
            z3::expr cond_expr = context_.bool_val(true);
            BasicBlock* bb_next = br_inst->getSuccessor(0);
            std::string bb_name =
                         ValueToVariable(dyn_cast<Value>(bb_next), thread_name);
            auto iter = block_map.find(bb_name);
            assert(iter != block_map.end());
            bb_struct.branch_map.insert(
              std::make_pair(
                static_cast<int>(inst_exprs_.size()),
                iter->second
              )
            );
            inst_exprs_.push_back(cond_expr);
#ifdef LOCAL_DEBUG
            std::cout << "Unconditional Branch Instruction to ";
            std::cout << iter->second << " block" << std::endl;
#endif
          } else {
            inst_types_.push_back(kAssume);
            inst_types_.push_back(kAssume);
            inst_lval_operands_.push_back("assume");
            inst_lval_operands_.push_back("assume");
            Value* v_cond = br_inst->getCondition();
            z3::expr cond_expr =
                        ValueToExpr(v_cond, thread_name) != context_.int_val(0);
            BasicBlock* bb_next = br_inst->getSuccessor(0);
            std::string bb_name =
                         ValueToVariable(dyn_cast<Value>(bb_next), thread_name);
            auto iter = block_map.find(bb_name);
            assert(iter != block_map.end());
            bb_struct.branch_map.insert(
              std::make_pair(
                static_cast<int>(inst_exprs_.size()),
                iter->second
              )
            );
            inst_exprs_.push_back(cond_expr);
#ifdef LOCAL_DEBUG
            std::cout << "Conditional Branch Instruction" << std::endl;
            std::cout << "If " << ValueToVariable(v_cond, thread_name);
            std::cout << " then go to " << iter->second << " block";
            std::cout << std::endl;
#endif
            cond_expr = (cond_expr == context_.bool_val(false));
            bb_next = br_inst->getSuccessor(1);
            bb_name = ValueToVariable(dyn_cast<Value>(bb_next), thread_name);
            iter = block_map.find(bb_name);
            assert(iter != block_map.end());
            bb_struct.branch_map.insert(
              std::make_pair(
                static_cast<int>(inst_exprs_.size()),
                iter->second
              )
            );
            inst_exprs_.push_back(cond_expr);
#ifdef LOCAL_DEBUG
            std::cout << "If not " << ValueToVariable(v_cond, thread_name);
            std::cout << " then go to " << iter->second << " block";
            std::cout << std::endl;
#endif
          }
          break;
        }
        case Instruction::PHI: {
          bb_struct.has_phi = true;
          std::string lval_operand = ValueToVariable(&Inst, thread_name);
          AddVariable(lval_operand);
#ifdef LOCAL_DEBUG
          std::cout << "Caught a PHI one here: " << std::endl;
          std::cout << lval_operand << std::endl;
#endif
          PHINode* phi_node = dyn_cast<PHINode>(&Inst);

          std::vector<int> phi_inst_nums;
          std::vector<int> phi_inc_blocks;

          for (Use& U : phi_node->incoming_values()) {
            Value* v = U.get();
            z3::expr v_expr = ValueToExpr(v, thread_name);
            phi_inst_nums.push_back(static_cast<int>(inst_exprs_.size()));
            inst_types_.push_back(kAssign);
            inst_lval_operands_.push_back(lval_operand);
            inst_exprs_.push_back(v_expr);
#ifdef LOCAL_DEBUG
            std::cout << ValueToVariable(v, thread_name) << " ";
#endif
          }
#ifdef LOCAL_DEBUG
          std::cout << std::endl;
#endif
          for (BasicBlock* inc_bb : phi_node->blocks()) {
            std::string bb_name = ValueToVariable(inc_bb, thread_name);
            auto iter = block_map.find(bb_name);
            assert(iter != block_map.end());
            phi_inc_blocks.push_back(iter->second);
#ifdef LOCAL_DEBUG
            std::cout << bb_name << " ";
#endif
          }
#ifdef LOCAL_DEBUG
          std::cout << std::endl;
#endif
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
          bb_struct.inst.push_back(static_cast<int>(inst_exprs_.size()));
          std::string lval_operand = ValueToVariable(&Inst, thread_name);
          AddVariable(lval_operand);
          inst_types_.push_back(kAssign);
          inst_lval_operands_.push_back(lval_operand);
          Value* rf_val = Inst.getOperand(0);
          z3::expr rhs_expr = ValueToExpr(rf_val, thread_name);
          inst_exprs_.push_back(rhs_expr);
#ifdef LOCAL_DEBUG
          std::cout << lval_operand << " = ";
          std::cout << ValueToVariable(rf_val, thread_name) << std::endl;
#endif
          break;
        }
        case Instruction::Store: {
          bb_struct.inst.push_back(static_cast<int>(inst_exprs_.size()));
          Value* lhs = Inst.getOperand(1);
          std::string lval_operand = ValueToVariable(lhs, thread_name);
          AddVariable(lval_operand);
          inst_types_.push_back(kAssign);
          inst_lval_operands_.push_back(lval_operand);
          Value* rhs = Inst.getOperand(0);
          z3::expr rhs_expr = ValueToExpr(rhs, thread_name);
          inst_exprs_.push_back(rhs_expr);
#ifdef LOCAL_DEBUG
          std::cout << lval_operand << " = ";
          std::cout << ValueToVariable(rhs, thread_name) << std::endl;
#endif
          break;
        }
        case Instruction::Add:
        case Instruction::Sub:
        case Instruction::Mul:
        case Instruction::ICmp: {
          bb_struct.inst.push_back(static_cast<int>(inst_exprs_.size()));
          std::string lval_operand = ValueToVariable(&Inst, thread_name);
          AddVariable(lval_operand);
          inst_types_.push_back(kAssign);
          inst_lval_operands_.push_back(lval_operand);
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
          inst_exprs_.push_back(rhs_expr);
#ifdef LOCAL_DEBUG
          std::cout << ValueToVariable(op2, thread_name) << std::endl;
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

  // A bit of sanity checking
  assert(inst_types_.size() == inst_exprs_.size());
  assert(inst_types_.size() == inst_lval_operands_.size());

  adjacency_list<int> aut_graph = CreateAutGraph(bb_automata);
  return;

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
