#pragma once

#include <utility>
#include <stdexcept>
#include <queue>

namespace vk_data {
namespace {
template <class K, class T>
class AVLNode {
public:
	AVLNode<T> *_left;
	AVLNode<T> *_right;
	int _height;
	int _bf;
	K _key;
	T _data;

	explicit AVLNode(AVLNode<K, T> *left, AVLNode<K, T> *right, K key, T data) :
		_left(left),
		_right(right),
		_height(0),
		_bf(0),
		_key(std::move(key)),
		_data(std::move(data)) {}
};
} // namespace

template <class K, class T>
class AVLTree {
private:
	AVLNode<K, T> _root;
	int _size;

public:
	explicit AVLTree() :
		_root(nullptr),
		_size(0) {}

	AVLTree(AVLTree<K, T>& other) {
		std::queue<AVLNode<K, T>*> nodes;
	}

	AVLTree(AVLTree<K, T>&& other) {

	}

	~AVLTree() {
		clear();
	}

	AVLTree<K, T>& operator=(AVLTree<K, T> other) {
		other.swap(*this);
		return *this;
	}

	void swap(AVLTree<K, T>& other) {
		std::swap(_root, other._root);
		std::swap(_size, other._size);
	}

	void clear() {

	}

	void add(K key, T data) {

	}

	T remove(const K& key) {

	}

	T& get(const K& key) {

	}

	const T& get(const K& key) const {

	}

	bool contains(const K& key) const {

	}

	int size() const {
		return _size;
	}
};

} // namespace vk_data