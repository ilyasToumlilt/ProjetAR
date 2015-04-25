#include "list_node.c"

node* makeNode(int id){
  node* ret = (node*)malloc(sizeof(node));
  ret->id = id;
  return ret;
}

int main(int argc, char** argv)
{
  list_node* ln = newListNode(makeNode(1), NULL);
  printListNode(ln);
  pushNodeToListNode(&ln, makeNode(2));
  pushNodeToListNode(&ln, makeNode(3));
  pushNodeToListNode(&ln, makeNode(4));
  pushNodeToListNode(&ln, makeNode(5));
  printListNode(ln);

  freeListNode(ln);

  return EXIT_SUCCESS;
}

