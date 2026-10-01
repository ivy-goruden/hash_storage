#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE
#include "storage.hpp"

namespace s21 {
    enum Color {
        BLACK,
        RED
    };

    template <typename T>
    class RBNode: public Node<T>{
        private:
            Color color;
            RBNode<T>* parent;
            std::unique_ptr<RBNode<T>> left;
            std::unique_ptr<RBNode<T>> right;
        public:
           RBNode(const string& key, const T& value, Color color)
            : Node<T>(key, value),
            color(color),
            parent(nullptr),
            left(nullptr),
            right(nullptr) {}
                
        template <typename> friend class RedBlackTree;
    };

    template <typename T>
    class RedBlackTree : public Storage<T> {
    private:
        using NodeType = RBNode<T>;
        std::unique_ptr<NodeType> root_;

        static Color colorOf(const NodeType* node);
        static NodeType* minimum(NodeType* node);
        std::unique_ptr<NodeType>& ownerOf(NodeType* node);
        void replace(NodeType* oldNode, std::unique_ptr<NodeType> newNode);
        void deleteFixup(NodeType* node, NodeType* parent);
        void rotateLeft(NodeType* node);
        void rotateRight(NodeType* node);
        NodeType* getUncle(NodeType* node);
        void validateRBNode(NodeType* node);
        void forEachChildren(NodeType* parent, const std::function<void(Node<T>&)>& func);

    public:
        RedBlackTree();
        ~RedBlackTree() override;
        void printTree();
        bool set(const std::string key, const T& element,
                 const std::optional<TimePoint> ttl = std::nullopt) override;
        Node<T>* getNode(const std::string key) override;
        bool del(const std::string key) override;

        template <typename F>
        bool update(const std::string key, const F& element) {
            Node<T>* node = getNode(key);
            if (node == nullptr) return false;
            T value = node->getValue();
            value = element;
            node->setValue(value);
            return true;
        }

        void ForEach(const std::function<void(Node<T>&)>& func) override;
    };
}
#include "red_black_tree.tpp"
#endif