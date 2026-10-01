#ifndef HASH_MAP
#define HASH_MAP
#include "storage.hpp"
#include "../include/hasher.hpp"
#include <vector>
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <chrono>
using namespace s21;
using namespace std;
template <typename T>
class Hash_Map : public Storage<T>{
    class HashNode: public Node<T>{
        private:
            unsigned long hash;
        public:
            HashNode(unsigned long hash, const T& value, const string& key)
            : Node<T>(key, value), hash(hash) {}
            unsigned long getHash() const { return hash; }
        friend class Hash_Map;
        
    };
    protected:
        using Storage<T>::size_;
        using Storage<T>::filled_;
    private:
        std::vector<std::forward_list<Node<T>*>>elements_;
        void rehash();
        void ForEach(const std::function<void(Node<T>&)>& func) override{
            for (auto& bucket : elements_){
                for (auto& el : bucket){
                    func(*el);
                }
            }
        };
    public:
        Hash_Map(){
            elements_.resize(size_);
        };
        ~Hash_Map() = default;
        bool set(const string key, const T& element,
             const std::optional<TimePoint> ttl = std::nullopt) override;
           Node<T>* getNode(const string key) override;
        bool del(const string key) override;
        template <typename F> 
        bool update(const string key, const F& element);
};

// Template implementations must be in header file
template <typename T>
bool Hash_Map<T>::set(const string key, const T& element, const std::optional<TimePoint> ttl){
    if (this->exists(key)) return false;
    unsigned long hash = Hasher::getHash(key.c_str());
    int index = hash % size_; //gettting index of our new element in the list
    HashNode* newEl = new HashNode(hash, element, key);
    newEl->setTTL(ttl);
    elements_[index].push_front(newEl);
    filled_++;
    if (filled_ > size_){
        rehash();
    }
    return true;
    
}

template <typename T>
void Hash_Map<T>::rehash(){
    size_*=2; 
    std::vector<std::forward_list<Node<T>*>> newElements_;
    newElements_.resize(size_);
    for (const auto &el: elements_){
        for (Node<T>* node: el){
            auto* hashNode = static_cast<HashNode*>(node);
            int index = hashNode->getHash() % size_;
            newElements_[index].push_front(node);

        }
    }
    elements_ = std::move(newElements_);
}

template <typename T>
Node<T>* Hash_Map<T>::getNode(const string key){
    unsigned long hash = Hasher::getHash(key.c_str());
    int index = hash % size_;
    for (Node<T>* node: elements_[index]){
        if (node->getKey() == key){
            return node;
        }
    }
    return nullptr;
}

template <typename T>
bool Hash_Map<T>::del(const string key){
    unsigned long hash = Hasher::getHash(key.c_str());
    int index = hash % size_;
    auto& bucket = elements_[index];
    auto previous = bucket.before_begin();
    for (auto current = bucket.begin(); current != bucket.end(); ++current){
        Node<T>* node = *current;
        if (node->getKey() == key){
            bucket.erase_after(previous);
            delete node;
            --filled_;
            return true;
        }
        ++previous;
    }

    return false;
}

template <typename T>
template <typename F> 
bool Hash_Map<T>::update(const string key, const F& element){

    unsigned long hash = Hasher::getHash(key.c_str());
    int index = hash % size_;
    for (Node<T>* node: elements_[index]){
        if (node->getKey() == key){
            T prev = node->getValue();
            prev = element;
            node->setValue(prev);
            return true;
        }
    }
    return false;
}

#endif