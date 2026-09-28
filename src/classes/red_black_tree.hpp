#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE
using namespace std;
#include "storage.hpp"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
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
        public:
            RedBlackTree{};
            ~RedBlackTree();
            void printTree();
            bool set(const string key, const T& element){
                if (elements_.empty()){
                    RBNode* newRBNode = new RBNode(key, element, BLACK);
                    elements_.push_back(newRBNode);
                    return true;
                }
                RBNode *P = nullptr;
                RBNode *newPlace = elements_[0];
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
                RBNode* newRBNode = new RBNode(key, element, RED);
                newRBNode->parent = P;
                newPlace = newRBNode;
                validateRBNode(newPlace);
                return true;
            }
            // absl::StatusOr<T> get(const string key){
            //     Node<T>* n = getNode(key);
            //     if (n == nullptr) return absl::NotFoundError("No record with this key");
            //     return n->element;
            // }
            bool del(const string key){
                Node<T>* n = getNode(key);
                if (n == nullptr) return false;
                
                return false;
            }
            bool update(const string key, const T& element)override{
                Node<T>* n = getNode(key);
                if (n == nullptr) return absl::NotFoundError("No record with this key");
                n->element = element;
            }
            template <typename F> 
            bool update(const string key, const F& element){
                Node<T>* n = getNode(key);
                if (n == nullptr) return absl::NotFoundError("No record with this key");
                T prev = n->getValue();
                prev = element;
                n->setValue(prev);
                return true;
            }
            void ForEach(const std::function<void(RBNode<T>&)>& func) override{
                for (auto &el : elements_){
                    func(el);
                }
            }
        private:
            std::vector<RBNode*> elements_;
            void ForEach(F&& func) override{
                for (auto& bucket : elements_) func(bucket);
            };
            template <typename T>
            void rotateLeft(RBNode<T>* n){
                if (n->right == nullptr) return; //нечего поворачивать
                RBNode* P = n;
                RBNode* R = n->right();
                RBNode* G = P->parent;
                R->parent = G;
                R->left = P;
                P->parent = R;
                if (G == nullptr) return;
                if (G->left == P){
                    G->left = R;
                }else{
                    G->right = R;
                }
            }
            template <typename T>
            void rotateRight(RBNode<T>* n){
                if (n->left == nullptr) return; //нечего поворачивать
                RBNode* P = n;
                RBNode* L = n->left;
                RBNode* G = P->parent;
                L->parent = G;
                L->right = P;
                P->parent = R;
                if (G == nullptr) return;
                if (G->left == P){
                    G->left = L;
                }else{
                    G->right = L;
                }
            }
            RBNode* getBrother(RBNode* n){
                if (n == nullptr || n->parent == nullptr) return nullptr;
                RBNode* P = n->parent;
                if (P->left == n){
                    return P->right;
                }
                return P->left;
            }

            RBNode* getUncle(RBNode* n){
                if (n == nullptr || n->parent == nullptr || n->parent->parent == nullptr) return nullptr;
                RBNode* P = n->parent;
                RBNode* G = P->parent;
                if (G->left == P){
                    return G->right;
                }
                return G->left;
            }
            void validateRBNode(RBNode* n){
                if (n == nullptr || n->color == BLACK) return;
                if (n->parent == nullptr){
                    n->color = BLACK;
                    return;
                }
                if (n->parent == BLACK) return;
                RBNode* U = getUncle(n);
                RBNode* P = n->parent;
                RBNode* G = P->parent;
                if (U->color == RED){
                    P->color = BLACK;
                    U->color = BLACK;
                    G->color = RED;
                    validateRBNode(G)
                }
                else{
                    left1 = false;
                    left2 = false;
                    if (P->left == n){
                        left1 = true;
                    }
                    if (G->left == P){
                        left2 = true;
                    }
                    if (left1 && left2){
                        rotateRight(G);
                        p->color = BLACK;
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
            template <typename T>
            Node<T>* getNode(const string key){
                if (elements_.empty()){
                    return nullptr;
                }
                RBNode *curRBNode = elements_[0];
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