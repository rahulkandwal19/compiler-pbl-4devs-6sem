#pragma once
#include <vector>
#include <map>
#include <string>

template<typename T>
class List {
public:
    std::vector<T> data;
    
    void add(T item) { data.push_back(item); }
    T get(int index) { return data[index]; }
    void set(int index, T item) { data[index] = item; }
    int size() { return (int)data.size(); }
    void clear() { data.clear(); }
    bool isEmpty() { return data.empty(); }
    int find(T item) {
        for (int i = 0; i < (int)data.size(); i++) {
            if (data[i] == item) return i;
        }
        return -1;
    }
    void remove(int index) {
        if (index >= 0 && index < (int)data.size()) {
            data.erase(data.begin() + index);
        }
    }
};

template<typename K, typename V>
class Map {
public:
    std::map<K, V> data;
    
    void put(K key, V value) { data[key] = value; }
    V get(K key) { 
        auto it = data.find(key);
        if (it != data.end()) return it->second;
        return V();
    }
    bool contains(K key) { return data.find(key) != data.end(); }
    void remove(K key) { data.erase(key); }
    int size() { return (int)data.size(); }
    void clear() { data.clear(); }
    bool isEmpty() { return data.empty(); }
};

template<typename T>
class Array {
public:
    std::vector<T> data;
    
    Array() {}
    Array(int size) { data.resize(size); }
    
    T get(int index) { return data[index]; }
    void set(int index, T item) { data[index] = item; }
    int size() { return (int)data.size(); }
};
