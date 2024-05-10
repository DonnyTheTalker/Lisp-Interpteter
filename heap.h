#pragma once

#include <unordered_set>
#include "lisp.h"
#include "object.h"

class Heap {
    friend class Interpreter;

public:
    static Heap& GetHeap();

public:
    Heap() = default;
    ~Heap();

public:
    template <class T, class... Args>
    requires std::is_convertible_v<T*, Object*> Object* Allocate(Args&&... args) {
        Object* obj = new T(std::forward<Args>(args)...);
        objects_.insert(obj);
        return obj;
    }

private:
    void DeleteUnmarked();
    void Unmark();
    void Clear();

private:
    std::unordered_set<Object*> objects_;
};
