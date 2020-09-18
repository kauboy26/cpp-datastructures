#pragma once

#include <utility>
#include <stdexcept>
#include <queue>
#include <cassert>
#include <functional>
#include <sstream>

namespace vk_data {
namespace {
template <class K, class T>
class AVLNode {
public:
	AVLNode<K, T> *_left;
	AVLNode<K, T> *_right;
	int _height;
	K _key;
	T _data;

	void setHeight() {
		if (_left && _right)
			_height = ((_left->_height > _right->_height) ?
						_left->_height : _right->_height) + 1;
		else if (_left)
			_height = _left->_height + 1;
		else if (_right)
			_height = _right->_height + 1;
		else
			_height = 0;
	}

	int getBF() {
		if (_left && _right)
			return _right->_height - _left->_height;
		else if (_left)
			return _left->_height - 1;
		else if (_right)
			return _right->_height + 1;
		else
			return 0;
	}

	explicit AVLNode(int height, K key, T data) :
		_left(nullptr),
		_right(nullptr),
		_height(height),
		_key(std::move(key)),
		_data(std::move(data)) {}
};
} // namespace

template <class K, class T, class L = std::less<K>>
class AVLTree {
private:
	L _less;
	AVLNode<K, T>* _root;
	int _size;
	int _height;

	bool less(const K& k1, const K& k2) const {
		return _less(k1, k2);
	}

	bool equals(const K& k1, const K& k2) const {
		return !_less(k1, k2) && !_less(k2, k1);
	}

	AVLNode<K, T>* _rotateRight(AVLNode<K, T>* hinge) {
		auto toReturn = hinge->_left;
		hinge->_left = toReturn->_right;
		hinge->setHeight();
		toReturn->_right = hinge;
		toReturn->setHeight();
		return toReturn;
	}

	AVLNode<K, T>* _rotateLeft(AVLNode<K, T>* hinge) {
		auto toReturn = hinge->_right;
		hinge->_right = toReturn->_left;
		hinge->setHeight();
		toReturn->_left = hinge;
		toReturn->setHeight();
		return toReturn;
	}

	AVLNode<K, T>* _fixRotations(AVLNode<K, T>* hinge) {
		int bf = hinge->getBF();
		if (-1 <= bf && bf <= 1)
			return hinge;
		else if (-2 == bf) {
			// Left heavy.
			if (hinge->_left->getBF() == -1)
				// Super left heavy
				return _rotateRight(hinge);
			else {
				hinge->_left = _rotateLeft(hinge->_left);
				hinge->setHeight();
				return _rotateRight(hinge);
			}
		} else {
			// Right heavy.
			if (hinge->_right->getBF() == 1)
				return _rotateLeft(hinge);
			else {
				hinge->_right = _rotateRight(hinge->_right);
				hinge->setHeight();
				return _rotateLeft(hinge);
			}
		}
	}

	AVLNode<K, T>* _add(AVLNode<K, T>* start, AVLNode<K, T>* newNode) {
		if (start == nullptr) {
			_size++;
			return newNode;
		}

		if (equals(newNode->_key, start->_key)) {
			start->_data = std::move(newNode->_data);
			delete newNode;
			return start;
		}

		if (less(newNode->_key, start->_key)) {
			start->_left = _add(start->_left, newNode);
		} else {
			start->_right = _add(start->_right, newNode);
		}

		start->setHeight();
		return _fixRotations(start);
	}

	void _print(std::ostream& os) const {
		if (!_size) {
			os << "<empty tree>";
			return;
		}

		os << '<' << _root->_key << ", " << _root->_data << '>';
		_printHelper(os, _root->_left, 1);
		_printHelper(os, _root->_right, 1);
	}

	void _printHelper(std::ostream& os, AVLNode<K, T>* curr, int shift) const {
		os << '\n';
		for (int i = 0; i < shift; i++)
			os << '\t';
		os << "|---";
		if (curr == nullptr)
			os << "<null>";
		else {
			os << '<' << curr->_key << ", " << curr->_data << '>';
			_printHelper(os, curr->_left, shift + 1);
			_printHelper(os, curr->_right, shift + 1);
		}
	}

	void _clear(AVLNode<K, T>* start) {
		if (start == nullptr)
			return;

		auto left = start->_left;
		auto right = start->_right;

		delete start;

		_clear(left);
		_clear(right);
	}

public:
	explicit AVLTree() :
		_root(nullptr),
		_size(0),
		_height(0) {}

	AVLTree(AVLTree<K, T, L>& other) {
		if (!other._size) {
			_root = nullptr;
			_size = 0;
			_height = -1;
			return;
		}

		// We are going to go through the given tree BFS style, and we are going
		// to use two parallel queues - one to hold the other tree's nodes, and
		// one to hold the next nodes that we need to create / set.
		// The queues will have a maximum occupancy of 2 ^ height - i.e. it
		// will maximally filled at the time when it is completely filled by the
		// last row of the original tree. This last row will have 2 ^ height
		// elements. Thus the queue can be an array of size 2 ^ height,
		// i.e. 1 << height.
		// We are going to implement a queue that wraps around. "i" points to
		// the current element, and "end" points to the last element. When
		// i > end, the queue is empty. (Actually, since we wrap, i % array.size
		// points to the element)

		int nodesInLastRow = 1 << (other._height);
		// i % nodesInLastRow == i & mod
		int mod = ~((-1) << other._height);

		AVLNode<K, T>* otherNodes[nodesInLastRow] = { nullptr };
		AVLNode<K, T>** myNodes[nodesInLastRow] = { nullptr };

		int end = -1; // the end of the queue, tells us when to stop.
		int i = 0; // the thing we are pointing to in the queue right now.

		// Add the two trees' root nodes to the queues:
		end++;
		otherNodes[end] = other._root;
		myNodes[end] = &_root;

		for(; i <= end; i++) {
			auto orig = otherNodes[i & mod];
			auto copy = myNodes[i & mod];

			*copy = new AVLNode<K, T>(orig->_height, orig->_bf,
				orig->_key, orig->_data);

			if (orig->left) {
				end++;
				otherNodes[end & mod] = orig->left;
				myNodes[end & mod] = &((*copy)->left);
			}
			if (orig->right) {
				end++;
				otherNodes[end & mod] = orig->right;
				myNodes[end & mod] = &((*copy)->right);
			}
		}

		assert(end == other._size);

		_size = other._size;
		_height = other._height;
	}

	AVLTree(AVLTree<K, T, L>&& other) {
		_root = nullptr;
		_size = 0;
		_height = 0;

		std::swap(_root, other._root);
		std::swap(_size, other._size);
		std::swap(_height, other._height);
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
		std::swap(_height, other._height);
	}

	void clear() {
		if (!_size)
			return;
		_clear(_root);
		_root = nullptr;
		_size = 0;
		_height = 0;
	}

	void add(K key, T data) {
		auto newNode = new AVLNode<K, T>(0, std::move(key), std::move(data));

		if (!_size) {
			_root = newNode;
			_size = 1;
			_height = 0;
			return;
		}

		_root = _add(_root, newNode);
		_height = _root->_height;
	}

	T remove(const K& key) {
		if (!_size)
			throw std::runtime_error("Empty tree: element not found.");

		// VKTODO
		return _root->data;
	}

	T& get(const K& key) {
		auto curr = _root;
		while (curr != nullptr) {
			if (equals(key, curr->_key))
				break;
			if (less(key, curr->_key))
				curr = curr->_left;
			else
				curr = curr->_right;
		}

		if (curr == nullptr)
			throw std::runtime_error("Element not found.");

		return curr->_data;
	}

	const T& get(const K& key) const {
		auto curr = _root;
		while (curr != nullptr) {
			if (equals(key, curr->_key))
				break;
			if (less(key, curr->_key))
				curr = curr->_left;
			else
				curr = curr->_right;
		}

		if (curr == nullptr)
			throw std::runtime_error("Element not found.");

		return curr->_data;
	}

	bool contains(const K& key) const {
		auto curr = _root;
		while (curr != nullptr) {
			if (equals(key, curr->_key))
				return true;
			if (less(key, curr->_key))
				curr = curr->_left;
			else
				curr = curr->_right;
		}

		return false;
	}

	int size() const {
		return _size;
	}

	int height() const {
		return _height;
	}

	friend std::ostream& operator<<(std::ostream& os, const AVLTree& tree) {
		tree._print(os);
		return os;
	}
};
} // namespace vk_data