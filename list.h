#pragma once

#include <utility>
#include <stdexcept>
#include <sstream>

namespace vk_data {

template <class T>
class LinkedList {
private:
	template <class E>
	struct LLNode {
		LLNode<E> *_next;
		T _data;

		explicit LLNode(LLNode<E> *next, T data) :
			_next(next),
			_data(std::move(data)) {}
};

private:
	int _size;
	LLNode<T> *_head;
	LLNode<T> *_tail;

	void swap(LinkedList<T>& other) {
		std::swap(_size, other._size);
		std::swap(_head, other._head);
		std::swap(_tail, other._tail);
	}

public:
	explicit LinkedList() :
		_size(0),
		_head(nullptr),
		_tail(nullptr) {}

	LinkedList(const LinkedList<T>& other) {
		_size = other._size;
		_head = nullptr;
		_tail = nullptr;

		if (!_size)
			return;

		LLNode<T> *prev = new LLNode<T>(nullptr, other._head->_data);
		LLNode<T> *curr = nullptr;
		LLNode<T> *otherCurr = other._head->_next;

		_head = prev;
		
		for (int i = 1; i < _size; i++) {
			curr = new LLNode<T>(nullptr, otherCurr->_data);
			prev->_next = curr;
			prev = curr;
			otherCurr = otherCurr->_next;
		}

		_tail = curr;
	}

	LinkedList(LinkedList<T>&& other) {
		_size = other._size;
		_head = other._head;
		_tail = other._tail;

		other._size = 0;
		other._head = nullptr;
		other._tail = nullptr;
	}

	~LinkedList() {
		clear();
	}

	LinkedList<T>& operator=(LinkedList<T> other) {
		other.swap(*this);
		return *this;
	}

	T& operator[](int idx) {
		if (idx < 0 || idx >= _size)
			throw std::runtime_error(
				"List index out of bounds: " + std::to_string(idx));

		LLNode<T> *curr = _head;
		for (int i = 0; i < idx; i++) {
			curr = curr->_next;
		}

		return curr->_data;
	}

	T& first() {
		if (!_size)
			throw std::runtime_error("List has no elements.");
		return _head->_data;
	}

	T& last() {
		if (!_size)
			throw std::runtime_error("List has no elements.");
		return _tail->_data;
	}

	const T& operator[](int idx) const {
		if (idx < 0 || idx >= _size)
			throw std::runtime_error(
				"List index out of bounds: " + std::to_string(idx));

		LLNode<T> curr = _head;
		for (int i = 0; i < idx; i++) {
			curr = curr->_next;
		}

		return curr->_data;
	}

	const T& first() const {
		if (!_size)
			throw std::runtime_error("List has no elements.");
		return _head->_data;
	}

	const T& last() const {
		if (!_size)
			throw std::runtime_error("List has no elements.");
		return _tail->_data;
	}

	void add(T data) {
		LLNode<T> *curr = new LLNode<T>(nullptr, std::move(data));

		if (!_size) {
			_head = curr;
			_tail = curr;
			_size++;
			return;
		}

		_tail->_next = curr;
		_tail = curr;
		_size++;
	}

	T pop() {
		if (!_size)
			throw std::runtime_error("List has no elements.");

		T data = std::move(_head->_data);
		LLNode<T> *next = _head->_next;
		delete _head;

		_head = next;
		_size--;

		if (!_size) {
			assert(_head == nullptr);
			_tail = nullptr;
		}

		return data;
	}

	void clear() {
		LLNode<T> *curr = _head;
		LLNode<T> *next;

		for (int i = 0; i < _size; i++) {
			next = curr->_next;
			delete curr;
			curr = next;
		}

		_size = 0;
		_head = nullptr;
		_tail = nullptr;
	}

	int size() const { return _size; }

	friend std::ostream& operator<<(std::ostream& os, const LinkedList& ll) {
		os << '[';
		auto curr = ll._head;
		while (curr != ll._tail) {
			os << curr->_data << ", ";
			curr = curr->_next;
		}

		if (curr != nullptr)
			os << curr->_data;

		os << ']';

		return os;
	}
};
} // namespace vk_data