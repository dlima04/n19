#include <AstNodes.h>
#include <ConManip.h>
#include <print>

/* The visual AST representation...
  |_ BinaryExpr <2:7> ::
  |  |_ LiteralExpr <3: 10> :: 3
  |  |_ LiteralExpr <3: 10> :: 10
  |_ ++ (prefix)
  |  |_ var
*/

auto n19::AstNode::print_(
  const uint32_t depth,
  const std::string& node_name ) const -> void
{
  for(uint32_t i = 0; i < depth; i++) {
    std::print("  |");
  }

  std::print(                   //---------------
    "{}{}_ {}{} ",              // The "title"
    manip_string(Con::Bold),    // Set bold
    manip_string(Con::Magenta), // Set fg: magenta
    node_name,                  // The AST node's name.
    manip_string(Con::Reset));  // Reset color
  std::print(                   //----------------
    "<{}{}{},{}{}{}> :: ",      // Line, position, address info.
    manip_string(Con::Yellow),  // Line number: Yellow.
    this->line_,                // Print line number.
    manip_string(Con::Reset),   // Reset color for ','.
    manip_string(Con::Yellow),  // File Position: Yellow.
    this->pos_,                 // Print file position.
    manip_string(Con::Reset)    // Reset color.
  );                            //----------------
}

auto n19::AstBranch::print(const uint32_t depth) const -> void {
  print_(depth, "BranchExpr");

}



