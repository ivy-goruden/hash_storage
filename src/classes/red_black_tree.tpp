namespace s21 {
	template <typename T>
	Color RedBlackTree<T>::colorOf(const NodeType* node) {
		return node == nullptr ? BLACK : node->color;
	}

	template <typename T>
	typename RedBlackTree<T>::NodeType* RedBlackTree<T>::minimum(NodeType* node) {
		while (node != nullptr && node->left != nullptr) node = node->left.get();
		return node;
	}

	template <typename T>
	std::unique_ptr<typename RedBlackTree<T>::NodeType>& RedBlackTree<T>::ownerOf(NodeType* node) {
		if (node->parent == nullptr) return root_;
		if (node->parent->left.get() == node) return node->parent->left;
		return node->parent->right;
	}

	template <typename T>
	void RedBlackTree<T>::replace(NodeType* oldNode, std::unique_ptr<NodeType> newNode) {
		NodeType* parent = oldNode->parent;
		if (newNode != nullptr) newNode->parent = parent;
		ownerOf(oldNode) = std::move(newNode);
	}

	template <typename T>
	RedBlackTree<T>::RedBlackTree() = default;

	template <typename T>
	RedBlackTree<T>::~RedBlackTree() = default;

	template <typename T>
	bool RedBlackTree<T>::set(const std::string key, const T& element,
							  const std::optional<TimePoint> ttl) {
		NodeType* parent = nullptr;
		NodeType* current = root_.get();
		while (current != nullptr) {
			parent = current;
			if (key < current->getKey()) current = current->left.get();
			else if (key > current->getKey()) current = current->right.get();
			else return false;
		}

		auto ownedNode = std::make_unique<NodeType>(key, element, parent == nullptr ? BLACK : RED);
		NodeType* node = ownedNode.get();
		node->setTTL(ttl);
		node->parent = parent;
		if (parent == nullptr) root_ = std::move(ownedNode);
		else if (key < parent->getKey()) parent->left = std::move(ownedNode);
		else parent->right = std::move(ownedNode);
		++this->filled_;
		validateRBNode(node);
		return true;
	}

	template <typename T>
	bool RedBlackTree<T>::del(const std::string key) {
		NodeType* target = static_cast<NodeType*>(getNode(key));
		if (target == nullptr) return false;

		auto& targetOwner = ownerOf(target);
		auto targetNode = std::move(targetOwner);
		NodeType* targetParent = targetNode->parent;
		NodeType* replacement = nullptr;
		NodeType* replacementParent = nullptr;
		Color removedColor;

		if (targetNode->left == nullptr || targetNode->right == nullptr) {
			removedColor = targetNode->color;
			auto child = targetNode->left != nullptr
						 ? std::move(targetNode->left)
						 : std::move(targetNode->right);
			replacement = child.get();
			replacementParent = targetParent;
			if (child != nullptr) child->parent = targetParent;
			targetOwner = std::move(child);
		} else {
			NodeType* successor = minimum(targetNode->right.get());
			removedColor = successor->color;
			NodeType* successorParent = successor->parent;
			auto successorNode = std::move(ownerOf(successor));
			auto successorChild = std::move(successorNode->right);
			replacement = successorChild.get();

			if (successorParent != target) {
				replacementParent = successorParent;
				if (successorChild != nullptr) successorChild->parent = successorParent;
				successorParent->left = std::move(successorChild);
				successorNode->right = std::move(targetNode->right);
				successorNode->right->parent = successorNode.get();
			} else {
				replacementParent = successorNode.get();
				if (successorChild != nullptr) successorChild->parent = successorNode.get();
				successorNode->right = std::move(successorChild);
			}

			successorNode->left = std::move(targetNode->left);
			successorNode->left->parent = successorNode.get();
			successorNode->parent = targetParent;
			successorNode->color = targetNode->color;
			targetOwner = std::move(successorNode);
		}

		--this->filled_;
		if (removedColor == BLACK) deleteFixup(replacement, replacementParent);
		return true;
	}

	template <typename T>
	void RedBlackTree<T>::deleteFixup(NodeType* node, NodeType* parent) {
		while (node != root_.get() && colorOf(node) == BLACK) {
			if (parent == nullptr) break;
			if (node == parent->left.get()) {
				NodeType* sibling = parent->right.get();
				if (colorOf(sibling) == RED) {
					sibling->color = BLACK;
					parent->color = RED;
					rotateLeft(parent);
					sibling = parent->right.get();
				}
				NodeType* near = sibling == nullptr ? nullptr : sibling->left.get();
				NodeType* far = sibling == nullptr ? nullptr : sibling->right.get();
				if (colorOf(near) == BLACK && colorOf(far) == BLACK) {
					if (sibling != nullptr) sibling->color = RED;
					node = parent;
					parent = node->parent;
				} else {
					if (colorOf(far) == BLACK) {
						if (near != nullptr) near->color = BLACK;
						if (sibling != nullptr) {
							sibling->color = RED;
							rotateRight(sibling);
						}
						sibling = parent->right.get();
						far = sibling == nullptr ? nullptr : sibling->right.get();
					}
					if (sibling != nullptr) sibling->color = parent->color;
					parent->color = BLACK;
					if (far != nullptr) far->color = BLACK;
					rotateLeft(parent);
					node = root_.get();
					parent = nullptr;
				}
			} else {
				NodeType* sibling = parent->left.get();
				if (colorOf(sibling) == RED) {
					sibling->color = BLACK;
					parent->color = RED;
					rotateRight(parent);
					sibling = parent->left.get();
				}
				NodeType* near = sibling == nullptr ? nullptr : sibling->right.get();
				NodeType* far = sibling == nullptr ? nullptr : sibling->left.get();
				if (colorOf(near) == BLACK && colorOf(far) == BLACK) {
					if (sibling != nullptr) sibling->color = RED;
					node = parent;
					parent = node->parent;
				} else {
					if (colorOf(far) == BLACK) {
						if (near != nullptr) near->color = BLACK;
						if (sibling != nullptr) {
							sibling->color = RED;
							rotateLeft(sibling);
						}
						sibling = parent->left.get();
						far = sibling == nullptr ? nullptr : sibling->left.get();
					}
					if (sibling != nullptr) sibling->color = parent->color;
					parent->color = BLACK;
					if (far != nullptr) far->color = BLACK;
					rotateRight(parent);
					node = root_.get();
					parent = nullptr;
				}
			}
		}
		if (node != nullptr) node->color = BLACK;
	}

	template <typename T>
	void RedBlackTree<T>::rotateLeft(NodeType* node) {
		if (node == nullptr || node->right == nullptr) return;
		auto& nodeOwner = ownerOf(node);
		auto pivot = std::move(nodeOwner->right);
		nodeOwner->right = std::move(pivot->left);
		if (nodeOwner->right != nullptr) nodeOwner->right->parent = node;
		pivot->parent = node->parent;
		pivot->left = std::move(nodeOwner);
		pivot->left->parent = pivot.get();
		nodeOwner = std::move(pivot);
	}

	template <typename T>
	void RedBlackTree<T>::rotateRight(NodeType* node) {
		if (node == nullptr || node->left == nullptr) return;
		auto& nodeOwner = ownerOf(node);
		auto pivot = std::move(nodeOwner->left);
		nodeOwner->left = std::move(pivot->right);
		if (nodeOwner->left != nullptr) nodeOwner->left->parent = node;
		pivot->parent = node->parent;
		pivot->right = std::move(nodeOwner);
		pivot->right->parent = pivot.get();
		nodeOwner = std::move(pivot);
	}

	template <typename T>
	typename RedBlackTree<T>::NodeType* RedBlackTree<T>::getUncle(NodeType* node) {
		if (node == nullptr || node->parent == nullptr || node->parent->parent == nullptr) return nullptr;
		NodeType* parent = node->parent;
		NodeType* grandparent = parent->parent;
		return grandparent->left.get() == parent ? grandparent->right.get() : grandparent->left.get();
	}

	template <typename T>
	void RedBlackTree<T>::validateRBNode(NodeType* node) {
		while (node != nullptr && node->parent != nullptr && node->parent->color == RED) {
			NodeType* parent = node->parent;
			NodeType* grandparent = parent->parent;
			if (parent == grandparent->left.get()) {
				NodeType* uncle = grandparent->right.get();
				if (colorOf(uncle) == RED) {
					parent->color = BLACK;
					uncle->color = BLACK;
					grandparent->color = RED;
					node = grandparent;
				} else {
					if (node == parent->right.get()) {
						node = parent;
						rotateLeft(node);
						parent = node->parent;
						grandparent = parent->parent;
					}
					parent->color = BLACK;
					grandparent->color = RED;
					rotateRight(grandparent);
				}
			} else {
				NodeType* uncle = grandparent->left.get();
				if (colorOf(uncle) == RED) {
					parent->color = BLACK;
					uncle->color = BLACK;
					grandparent->color = RED;
					node = grandparent;
				} else {
					if (node == parent->left.get()) {
						node = parent;
						rotateRight(node);
						parent = node->parent;
						grandparent = parent->parent;
					}
					parent->color = BLACK;
					grandparent->color = RED;
					rotateLeft(grandparent);
				}
			}
		}
		if (root_ != nullptr) root_->color = BLACK;
	}

	template <typename T>
	Node<T>* RedBlackTree<T>::getNode(const std::string key) {
		NodeType* current = root_.get();
		while (current != nullptr) {
			if (key < current->getKey()) current = current->left.get();
			else if (key > current->getKey()) current = current->right.get();
			else return current;
		}
		return nullptr;
	}

	template <typename T>
	void RedBlackTree<T>::ForEach(const std::function<void(Node<T>&)>& func) {
		forEachChildren(root_.get(), func);
	}

	template <typename T>
	void RedBlackTree<T>::forEachChildren(NodeType* parent,
										  const std::function<void(Node<T>&)>& func) {
		if (parent == nullptr) return;
		func(*parent);
		forEachChildren(parent->left.get(), func);
		forEachChildren(parent->right.get(), func);
	}
}