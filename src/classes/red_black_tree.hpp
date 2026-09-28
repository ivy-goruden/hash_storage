#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE
using namespace std;
#include "storage.hpp"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <stack>
namespace s21{
    enum Color{
        BLACK,
        RED
    };
    template <typename T>
    class RBNode: public Node<T>{
        private:
            Color color;
            RBNode<T>* parent;
            RBNode<T>* left;
            RBNode<T>* right;
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
    class RedBlackTree: public Storage<T>{
        private:
            RBNode<T>* root_ = nullptr;
        public:
            RedBlackTree() = default;
            ~RedBlackTree() = default;
            void printTree();
            bool set(const string key, const T& element, const std::optional<TimePoint> ttl = std::nullopt) override{
                if (!root_){
                    RBNode<T>* newRBNode = new RBNode<T>(key, element, BLACK);
                    newRBNode->setTTL(ttl);
                    root_ = newRBNode;
                    return true;
                }
                RBNode<T>* P = nullptr;
                RBNode<T>* newPlace = root_;
                while (newPlace != nullptr){
                    if (key > newPlace->key){
                        P = newPlace;
                        newPlace = newPlace->right;
                    }
                    else if (key < newPlace->key){
                        P = newPlace;
                        newPlace = newPlace->left;
                    }
                    else{
                        return false;
                    }
                }
                RBNode<T>* newRBNode = new RBNode<T>(key, element, RED);
                newRBNode->parent = P;
                if (key < P->getKey()) {
                    P->left = newRBNode;
                } else {
                    P->right = newRBNode;
                }
                validateRBNode(newRBNode);
                return true;
            }
            bool del(const string key){
                Node<T>* n = getNode(key);
                if (n == nullptr) return false;
                
                return false;
            }
            bool update(const string key, const T& element)override{
                Node<T>* n = getNode(key);
                if (n == nullptr) return false;
                n->setValue(element);
                return true;
            }
            template <typename F> 
            bool update(const string key, const F& element){
                Node<T>* n = getNode(key);
                if (n == nullptr) return false;
                T prev = n->getValue();
                prev = element;
                n->setValue(prev);
                return true;
            }
            void ForEach(const std::function<void(Node<T>&)>& func) override{
                std::stack<RBNode<T>*> toVisit;
                if (root_ != nullptr){
                    toVisit.push(root_);
                }
                while(!toVisit.empty()){
                    RBNode<T>* curNode = toVisit.top();
                    toVisit.pop();
                    func(*curNode);
                    if (curNode->left){
                        toVisit.push(curNode->left);
                    }
                    if (curNode->right){
                        toVisit.push(curNode->right);
                    }
                }
            };
        private:
            void rotateLeft(RBNode<T>* n){
                if (n->right == nullptr) return; //нечего поворачивать
                RBNode* P = n;
                RBNode* R = n->right();
                RBNode* G = P->parent;
                R->parent = G;
                R->left = P;
                R->parent = P->parent;
                P->parent = R;
                if (G == nullptr) return;
                if (G->left == P){
                    G->left = R;
                }else{
                    G->right = R;
                }
            }
            void rotateRight(RBNode<T>* n){
                if (n->left == nullptr) return; //нечего поворачивать
                RBNode<T>* P = n;
                RBNode<T>* L = n->left;
                RBNode<T>* G = P->parent;
                L->parent = G;
                L->right = P;
                P->parent = L;
                if (G == nullptr) return;
                if (G->left == P){
                    G->left = L;
                }else{
                    G->right = L;
                }
            }
            RBNode<T>* getBrother(RBNode<T>* n){
                if (n == nullptr || n->parent == nullptr) return nullptr;
                RBNode<T>* P = n->parent;
                if (P->left == n){
                    return P->right;
                }
                return P->left;
            }

            RBNode<T>* getUncle(RBNode<T>* n){
                if (n == nullptr || n->parent == nullptr || n->parent->parent == nullptr) return nullptr;
                RBNode<T>* P = n->parent;
                RBNode<T>* G = P->parent;
                if (G->left == P){
                    return G->right;
                }
                return G->left;
            }
            void validateRBNode(RBNode<T>* n){
                if (n == nullptr || n->color == BLACK) return;
                if (n->parent == nullptr){
                    n->color = BLACK;
                    return;
                }
                if (n->parent->color == BLACK) return;
                RBNode<T>* U = getUncle(n);
                RBNode<T>* P = n->parent;
                RBNode<T>* G = P->parent;
                if (U != nullptr && U->color == RED) {
                    P->color = BLACK;
                    U->color = BLACK;
                    G->color = RED;
                    validateRBNode(G);
                }
                else{
                    bool left1 = false;
                    bool left2 = false;
                    if (P->left == n){
                        left1 = true;
                    }
                    if (G->left == P){
                        left2 = true;
                    }
                    if (left1 && left2){
                        rotateRight(G);
                        P->color = BLACK;
                        G->color = RED;
                    }
                    else if (left1 && !left2){
                        rotateLeft(P);
                        rotateRight(G);
                        n->color = BLACK;
                        G->color = RED;
                    }
                    else if (!left1 && !left2){
                        rotateLeft(G);
                        P->color = BLACK;
                        G->color = RED;
                    }
                    else{
                        rotateRight(P);
                        rotateLeft(G);
                        n->color = BLACK;
                        G->color = RED;
                    }
                }
            }
            Node<T>* getNode(const string key){
                RBNode<T>* curRBNode = root_;
                while (curRBNode != nullptr){
                    if (key > curRBNode->key){
                        curRBNode = curRBNode->right;
                    }
                    else if (key < curRBNode->key){
                        curRBNode = curRBNode->left;
                    }
                    else{
                        return curRBNode;
                    }
                }
                return nullptr;
            }

    };
}
#endif