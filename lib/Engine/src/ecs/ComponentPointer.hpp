#pragma once

template<typename T>
class ComponentPtr {
public:
	ComponentPtr(T* pointer) : _pointer(pointer) { /* Empty */ }

	T* operator->() { return _pointer; }
    T& operator*() { return *_pointer; }

private:
    T* _pointer;
};