#pragma once

#include <atomic>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "taichi/ir/control_flow_graph.h"
#include "taichi/ir/ir.h"

TLANG_NAMESPACE_BEGIN

// IR passes
namespace irpass {

// TODO(#1243): Pass kernel to the relevant passes instead of doing this hack
namespace hack {
bool use_fast_math(IRNode *root);
}  // namespace hack

void re_id(IRNode *root);
void flag_access(IRNode *root);
bool die(IRNode *root);
bool simplify(IRNode *root, Kernel *kernel = nullptr);
bool cfg_optimization(
    IRNode *root,
    bool after_lower_access,
    const std::optional<ControlFlowGraph::LiveVarAnalysisConfig>
        &lva_config_opt = std::nullopt);
bool alg_simp(IRNode *root);
bool demote_operations(IRNode *root);
bool binary_op_simplify(IRNode *root);
bool whole_kernel_cse(IRNode *root);
void variable_optimization(IRNode *root, bool after_lower_access);
void extract_constant(IRNode *root);
bool unreachable_code_elimination(IRNode *root);
void full_simplify(IRNode *root,
                   bool after_lower_access,
                   Kernel *kernel = nullptr);
void print(IRNode *root, std::string *output = nullptr);
void lower_ast(IRNode *root);
void type_check(IRNode *root);
void loop_vectorize(IRNode *root);
void bit_loop_vectorize(IRNode *root);
void slp_vectorize(IRNode *root);
void vector_split(IRNode *root, int max_width, bool serial_schedule);
void replace_all_usages_with(IRNode *root, Stmt *old_stmt, Stmt *new_stmt);
bool check_out_of_bound(IRNode *root);
void make_thread_local(IRNode *root);
std::unique_ptr<ScratchPads> initialize_scratch_pad(OffloadedStmt *root);
void make_block_local(IRNode *root);
bool remove_loop_unique(IRNode *root);
bool remove_range_assumption(IRNode *root);
bool lower_access(IRNode *root, bool lower_atomic);
void auto_diff(IRNode *root, bool use_stack = false);
bool constant_fold(IRNode *root);
void offload(IRNode *root);
void replace_statements_with(IRNode *root,
                             std::function<bool(Stmt *)> filter,
                             std::function<std::unique_ptr<Stmt>()> generator);
void demote_dense_struct_fors(IRNode *root);
bool demote_atomics(IRNode *root);
void reverse_segments(IRNode *root);  // for autograd
void detect_read_only(IRNode *root);
void optimize_bit_struct_stores(IRNode *root);

// compile_to_offloads does the basic compilation to create all the offloaded
// tasks of a Taichi kernel. It's worth pointing out that this doesn't demote
// dense struct fors. This is a necessary workaround to prevent the async
// engine from fusing incompatible offloaded tasks.
void compile_to_offloads(IRNode *ir,
                         const CompileConfig &config,
                         bool verbose,
                         bool vectorize,
                         bool grad,
                         bool ad_use_stack);

void offload_to_executable(IRNode *ir,
                           const CompileConfig &config,
                           bool verbose,
                           bool lower_global_access,
                           bool make_thread_local,
                           bool make_block_local);
// compile_to_executable fully covers compile_to_offloads, but also does
// additional optimizations so that |ir| can be directly fed into codegen.
void compile_to_executable(IRNode *ir,
                           const CompileConfig &config,
                           bool vectorize,
                           bool grad,
                           bool ad_use_stack,
                           bool verbose,
                           bool lower_global_access = true,
                           bool make_thread_local = false,
                           bool make_block_local = false);

}  // namespace irpass

TLANG_NAMESPACE_END
