#include <array>
#include <memory>
#include <vector>
#include <list>
#include <cassert>
#include <iostream>

static const size_t k_numComponents = 5;

template<typename T>
struct ComponentWrapper
{
	ComponentWrapper() : _pointer(nullptr) { /* Empty */ }
	ComponentWrapper(T* pointer) : _pointer(pointer) { /* Empty */ }

	T* _pointer;
};

struct Component
{
	enum class Type { Used, Unused, Invalidated };

	Type type;
	ComponentWrapper<Component>* _usedPointer;
	ComponentWrapper<Component>* _unusedPointer;
	int value;

	void debugPrint() const {
		std::cout << value;

		if(_unusedPointer != nullptr && _usedPointer == nullptr) {
			std::cout << "-N-" << _unusedPointer->_pointer->value << " ";
		} else if(_usedPointer != nullptr && _unusedPointer == nullptr) {
			std::cout << "-U-" << _usedPointer->_pointer->value << " ";
		} else {
			std::cout << "-E-# ";
		}
	}

	void print() const {
		if (type == Component::Type::Unused) { std::cout << value << "-N "; }
		else if (type == Component::Type::Invalidated) { std::cout << value << "-I "; }
		else { std::cout << value << "-U "; }
	}
};

struct System 
{
	bool _needsReorder{false};
	std::array<Component, k_numComponents> _components;
	std::vector<ComponentWrapper<Component>> _unusedComponents;
	std::list<ComponentWrapper<Component>> _usedComponents;
	
	void initialize() {
		_usedComponents.clear();
		_unusedComponents.resize(k_numComponents);

		for(size_t i = 0; i < k_numComponents; i++) {
			_components[i].value = static_cast<int>(i);
			_components[i].type = Component::Type::Unused;
			_components[i]._usedPointer = nullptr;
			
			_unusedComponents[i]._pointer = &_components[_components.size() - 1 - i];
			_unusedComponents[i]._pointer->_unusedPointer = &_unusedComponents[i];
		}
	}


	ComponentWrapper<Component>* createComponent() {
		if(_unusedComponents.empty() || _usedComponents.size() == _components.size()) {
			return nullptr;
		}

		Component* component = _unusedComponents.back()._pointer;
		_unusedComponents.pop_back();
		_usedComponents.emplace_back(component);

		component->type = Component::Type::Used;
		component->_unusedPointer = nullptr;
		component->_usedPointer = &(_usedComponents.back());
		
		return component->_usedPointer;
	}

	void releaseComponent(ComponentWrapper<Component>* componentPtr) {
		if(componentPtr == nullptr) { return; }

		Component* component = componentPtr->_pointer;

		auto it = std::find_if(_usedComponents.cbegin(), _usedComponents.cend(), 
							   [&component](const ComponentWrapper<Component>& comp) {
			return component == comp._pointer;
		});
		if (it == _usedComponents.end()) { return; }

		component->type = Component::Type::Invalidated;
		component->_unusedPointer = &(_unusedComponents.emplace_back(component));
		component->_usedPointer = nullptr;
		_usedComponents.erase(it);

		_needsReorder = true;
	}

	void visit() {
		std::cout << "Debug - ";
		for (Component& component : _components) {
			component.debugPrint();
		}
		std::cout << "\n";

		std::cout << "All - ";
		for (Component& component : _components) {
			component.print();
		}
		std::cout << "\n";

		std::cout << "Unused - ";
		for(ComponentWrapper<Component>& compPtr : _unusedComponents) {
			compPtr._pointer->print();
		}
		std::cout << "\n";

		std::cout << "Used - ";
		for (ComponentWrapper<Component>& compPtr : _usedComponents) {
			compPtr._pointer->print();
		}
		std::cout << "\nNeeds reorder - " << _needsReorder;
		std::cout << "\n" << std::endl;

	}

	void reorder() {
		if (!_needsReorder) { return; }

		size_t lastUsedIndex = INT_MAX;
		for(size_t i = 0; i < _components.size(); i++) {
			if (_components[i].type == Component::Type::Unused) { break; }
			if (_components[i].type == Component::Type::Invalidated) {
				
				if(lastUsedIndex > _components.size()) {
					// Find the last used index
					lastUsedIndex = _components.size() - 1;
					for(size_t j = 0; j < _components.size(); j++) {
						if(_components[j].type == Component::Type::Unused) {
							lastUsedIndex = j - 1;
							break;
						}
					}
				}

				auto copy = _components[lastUsedIndex];
				_components[lastUsedIndex] = _components[i];
				_components[i] = copy;


				_components[i]._usedPointer->_pointer = &_components[i];
				_components[lastUsedIndex]._unusedPointer->_pointer = &_components[lastUsedIndex];
				_components[lastUsedIndex].type = Component::Type::Unused;

				lastUsedIndex--;
			}
		}
		_needsReorder = false;
	}
};

int main(int, char const **) {
	std::unique_ptr<System> system = std::make_unique<System>();
	system->initialize();

	std::cout << "Initial state\n";
	system->visit();


	auto component0 = system->createComponent();

	std::cout << "1 Create component\n";
	system->visit();

	auto component1 = system->createComponent();
	auto component2 = system->createComponent();
	auto component3 = system->createComponent();
	std::cout << "3 Create Components\n";
	system->visit();

	system->releaseComponent(component2);
	std::cout << "1 Release Component\n";
	system->visit();

	system->reorder();
	std::cout << "Reorder called\n";
	system->visit();

	component2 = system->createComponent();
	std::cout << "1 Create component\n";
	system->visit();

	system->releaseComponent(component1);
	auto component4 = system->createComponent();
	std::cout << "1 Release followed by a create\n";
	system->visit();

	auto component5 = system->createComponent();
	auto component6 = system->createComponent();
	system->releaseComponent(component6);
	system->visit();

	return 0;
}
