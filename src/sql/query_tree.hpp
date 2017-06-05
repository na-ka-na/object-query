
template <typename Field>
void Node<Field>::walkNode(
    Node<Field>* parent,
    Node<Field>& node,
    int& indent,
    StartNodeFn<Field>& startNodeFn,
    uint32_t indentInc,
    map<int, Node<Field>*>& endNodesMap) {
  startNodeFn(indent, node, parent);
  endNodesMap.emplace(-indent, &node);
  for (auto& e : node.children) {
    Node& child = e.second;
    indent += indentInc;
    walkNode(&node, child, indent, startNodeFn, indentInc, endNodesMap);
  }
}

template <typename Field>
void Node<Field>::walkNode(
    Node<Field>& root,
    StartNodeFn<Field>& startNodeFn,
    EndNodeFn<Field>& endNodeFn,
    uint32_t indentInc) {
  int indent = 0;
  map<int, Node<Field>*> endNodesMap;
  walkNode(nullptr, root, indent, startNodeFn, indentInc, endNodesMap);
  for (auto& e : endNodesMap) {
    int indent = -e.first;
    Node* node = e.second;
    endNodeFn(indent, *node);
  }
}
