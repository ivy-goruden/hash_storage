#ifndef STORAGE
#define STORAGE
#include <array>
#include <forward_list>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include <functional>
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include <chrono>
namespace s21{
    using std::string;
    using TimePoint = std::chrono::system_clock::time_point;

    template <typename T>
    class Node{
        protected:
            string key;
            T element;
            std::optional<TimePoint> expireAt;
        public:
            Node(string k, T el): key(k), element(el) {}
            virtual ~Node() = default;
            bool operator<(const Node &other) const{
                return key<other.key;
            }
            bool operator>(const Node &other) const{
                return key>other.key;
            }
            const string& getKey() const { return key; }
            T& getValue() { return element; }
            const T& getValue() const { return element; }
            std::optional<TimePoint> getExpireAt() const { return expireAt; }
            void setValue(const T& value) { element = value; }
            void setTTL(const std::optional<TimePoint> &ttl){
                expireAt = ttl;
            }
            bool hasTTL() const { return expireAt.has_value(); }
            [[nodiscard]] bool IsExpired(const TimePoint &now) const{
                return expireAt.has_value() && now > expireAt.value();
            }
            [[nodiscard]] int getTTL(const TimePoint &now) const{
                if (!expireAt.has_value()) return -1;
                return std::chrono::duration_cast<std::chrono::seconds>(*expireAt - now).count();
            }
    };

    template <typename T>
    class Storage{
        protected:
            size_t size_ = 13;
            size_t filled_ = 0;
        public:
            Storage(){}
            virtual ~Storage() = default;
            virtual bool set(const string key, const T& element,
                             const std::optional<TimePoint> ttl = std::nullopt) = 0;
            virtual Node<T>* getNode(const string key) = 0;
            virtual bool del(const string key) = 0;
            virtual bool update(const string key, const T& element) = 0;
            virtual void ForEach(const std::function<void(Node<T>&)>& func) = 0;
        
        public:
            absl::StatusOr<T> get(const string key){
                Node<T>* node = getNode(key);
                if (node == nullptr) return absl::NotFoundError("No record with this key");
                return node->getValue();
            };
            bool exists(const string key){
                return getNode(key) != nullptr;
            }
            std::vector<string> keys(){
                std::vector<string> keys;
                ForEach([&] (Node<T>& node){
                    keys.push_back(node.getKey());
                });
                return keys;
            };
            absl::StatusOr<bool> rename(const string old_key, const string new_key){
                if (old_key == new_key) return true;
                if (exists(new_key)) return absl::AlreadyExistsError("Key already exists");
                Node<T>* oldNode = getNode(old_key);
                if (oldNode == nullptr) return absl::NotFoundError("No record with this key");
                T oldValue = oldNode->getValue();
                auto ttl = oldNode->getExpireAt();
                if (!set(new_key, oldValue, ttl)) return absl::AlreadyExistsError("Key already exists");
                del(old_key);
                return true;
            }
            absl::StatusOr<int> TTL(const string key){
                Node<T>* node = getNode(key);
                if (node == nullptr) return absl::NotFoundError("No record with this key");
                if (!node->hasTTL()) return absl::NotFoundError("No Expire record");
                return node->getTTL(std::chrono::system_clock::now());
            }
            std::vector<string> find(const T& value){
                std::vector<string> result;
                ForEach([&](Node<T>& node){
                    if (node.getValue() == value) result.push_back(node.getKey());
                });
                return result;
            }
            template <typename F>
            std::vector<string> find(const F& filter){
                std::vector<string> result;
                ForEach([&](Node<T>& node){
                    if (node.getValue() == filter) result.push_back(node.getKey());
                });
                return result;
            }
            std::vector<T> showall(){
                std::vector<T> result;
                ForEach([&](Node<T>& node){ result.push_back(node.getValue()); });
                return result;
            }
            void PurgeExpired(){
                const auto now = std::chrono::system_clock::now();
                std::vector<string> expiredKeys;
                ForEach([&](Node<T>& node){
                    if (node.IsExpired(now)) expiredKeys.push_back(node.getKey());
                });
                for (const auto& key : expiredKeys) del(key);
            };

    };
}
#endif