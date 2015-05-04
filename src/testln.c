#include <list_node.h>

int main(int argc, char** argv)
{
  list_node ln = newListNode();

  pushNodeToListNode(&ln, 0);
  printListNode(ln);
  pushNodeToListNode(&ln, 1);
  printListNode(ln);
  addNodeToListNode(&ln, 1);
  addNodeToListNode(&ln, 1);
  printListNode(ln);

  return 0;
}
