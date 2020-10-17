#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum {
  number_count = 4,
  ops_count = number_count - 1,
  all_count = number_count + ops_count
};

enum OperatorKind { op_add, op_sub, op_mul, op_div };

struct Operator {
  enum OperatorKind kind;
  char lhs, rhs;
};
enum NodeKind { node_number, node_operator };
struct Node {
  enum NodeKind kind;
  union {
    int n;
    struct Operator op;
  } v;
};

typedef struct Node SyntaxTree[7];
typedef struct {
  int num;
  bool valid;
} EvalResult;

static EvalResult makeNumber(int number) {
  return (EvalResult){.num = number, .valid = true};
}
static EvalResult makeInvalid() {
  return (EvalResult){.num = -1, .valid = false};
}

static EvalResult evalSyntaxTree(const SyntaxTree tree,
                                 const struct Node *curNode) {
  switch (curNode->kind) {
  case node_number:
    return makeNumber(curNode->v.n);
  case node_operator: {
    EvalResult lhs = evalSyntaxTree(tree, tree + curNode->v.op.lhs),
               rhs = evalSyntaxTree(tree, tree + curNode->v.op.rhs);
    if (!lhs.valid || !rhs.valid) {
      return makeInvalid();
    }
    switch (curNode->v.op.kind) {
    case op_add:
      return makeNumber(lhs.num + rhs.num);
    case op_sub:
      return makeNumber(lhs.num - rhs.num);
    case op_mul:
      return makeNumber(lhs.num * rhs.num);
    case op_div:
      return rhs.num != 0 ? makeNumber(lhs.num / rhs.num) : makeInvalid();
    }
  }
  }
  __builtin_unreachable();
}

static void printSyntaxTreeImpl(const SyntaxTree tree,
                                const struct Node *curNode) {
  static const char opChars[4] = {'+', '-', '*', '/'};
  switch (curNode->kind) {
  case node_number:
    printf("%d", curNode->v.n);
    break;
  case node_operator:
    putchar('(');
    printSyntaxTreeImpl(tree, tree + curNode->v.op.lhs);
    printf(" %c ", opChars[curNode->v.op.kind]);
    printSyntaxTreeImpl(tree, tree + curNode->v.op.rhs);
    putchar(')');
  }
}
static void printSyntaxTree(const SyntaxTree tree, const struct Node *curNode) {
  printSyntaxTreeImpl(tree, curNode);
  putchar('\n');
}

static void incrementOperators(enum OperatorKind ops[3]) {
  ++ops[0];
  if (ops[0] != op_div + 1)
    return;
  ops[0] = op_add;
  ++ops[1];
  if (ops[1] != op_div + 1)
    return;
  ops[1] = op_add;
  ++ops[2];
  if (ops[2] != op_div + 1)
    return;
  ops[2] = op_add;
}

static void swap(char *a, char *b) {
  char c = *a;
  *a = *b;
  *b = c;
}

static bool compareOps(const enum OperatorKind lhs[ops_count],
                       const enum OperatorKind rhs[ops_count]) {
  for (int i = 0; i < ops_count; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

static void iterateAllSyntaxTrees(const int numbers[4],
                                  void (*callback)(const SyntaxTree tree,
                                                   const struct Node *root)) {
  SyntaxTree tree;
  static const enum OperatorKind finalOps[ops_count] = {op_div, op_div, op_div};
  enum OperatorKind ops[ops_count] = {op_add, op_add, op_add};

  for (; !compareOps(ops, finalOps); incrementOperators(ops)) {
#define FOR_VAR(name, top) for (int name = 0; name < top; ++name)
#define FOR_OPERAND(name, top)                                                 \
  FOR_VAR(name##_lhs, top) FOR_VAR(name##_rhs, top - 1)
    FOR_OPERAND(first, 4) FOR_OPERAND(second, 3) FOR_OPERAND(third, 2) {
      // Setup the tree
      for (int i = 0; i < 4; ++i) {
        tree[i] = (struct Node){.kind = node_number, {.n = numbers[i]}};
      }
      for (int i = 0; i < 3; ++i) {
        tree[i + 4] =
            (struct Node){.kind = node_operator, {.op = {ops[i], -1, -1}}};
      }
      char itab[7] = {0, 1, 2, 3, 4, 5, 6};
      int arenaRight = 4;
      int curNode = 4;
      struct Node *curOperator = tree + 4;
      curOperator->v.op.lhs = itab[first_lhs];
      swap(itab + first_lhs, itab + --arenaRight);
      curOperator->v.op.rhs = itab[first_rhs];
      swap(itab + first_rhs, itab + curNode++);
      ++curOperator;
      curOperator->v.op.lhs = itab[second_lhs];
      swap(itab + second_lhs, itab + --arenaRight);
      curOperator->v.op.rhs = itab[second_rhs];
      swap(itab + second_rhs, itab + curNode++);
      ++curOperator;
      curOperator->v.op.lhs = itab[third_lhs];
      swap(itab + third_lhs, itab + --arenaRight);
      curOperator->v.op.rhs = itab[third_rhs];
      swap(itab + third_rhs, itab + curNode++);
      ++curOperator;
      callback(tree, tree + 6);
    }
  }
}

static int count = 0;
static void checkAndPrintCallback(const SyntaxTree tree,
                                  const struct Node *root) {
  const EvalResult res = evalSyntaxTree(tree, root);
  if (res.valid && res.num == 24) {
    printSyntaxTree(tree, root);
    ++count;
  }
}

int main() {
  int numbers[number_count];
  for (int i = 0; i < number_count; ++i) {
    const int code = scanf("%d", numbers + i);
    if (code != 1) {
      fprintf(stderr, "error: Input is malformed, scanf() returned %d\n", code);
      return 1;
    }
  }
  iterateAllSyntaxTrees(numbers, checkAndPrintCallback);
  if (!count) {
    puts("No solutions!\n");
  }
  return 0;
}