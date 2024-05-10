#include "heap.h"

void Heap::DeleteUnmarked() {
    std::unordered_set<Object*> marked;
    for (Object* obj : objects_) {
        if (obj->marked_) {
            marked.insert(obj);
        } else {
            delete obj;
        }
    }

    objects_ = std::move(marked);
    Unmark();
}

void Heap::Unmark() {
    for (auto obj : objects_) {
        obj->Unmark();
    }
}

Heap& Heap::GetHeap() {
    static Heap heap;
    return heap;
}

void Heap::Clear() {
    objects_.clear();
}

Heap::~Heap() {
    DeleteUnmarked();
}
