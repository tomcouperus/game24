struct CountState {
  unsigned char ops;
  unsigned char nums;
};

static void recurseCountTree(const SyntaxTree tree, const struct Node *cur,
                             struct CountState *state) {
  switch (cur->kind) {
  case node_number:
    ++state->nums;
    break;
  case node_operator:
    ++state->ops;
    assert(cur->v.op.lhs < all_count);
    assert(cur->v.op.rhs < all_count);
    recurseCountTree(tree, tree + cur->v.op.lhs, state);
    recurseCountTree(tree, tree + cur->v.op.rhs, state);
    break;
  }
}
