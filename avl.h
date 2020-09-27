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
            return -1 - _left->_height;
        else if (_right)
            return _right->_height - (-1);
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
            if (hinge->_left->getBF() <= 0)
                // Super left heavy
                return _rotateRight(hinge);
            else {
                hinge->_left = _rotateLeft(hinge->_left);
                hinge->setHeight();
                return _rotateRight(hinge);
            }
        } else {
            // Right heavy.
            assert(2 == bf);
            if (hinge->_right->getBF() >= 0)
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

    AVLNode<K, T>* _removeGreatest(AVLNode<K, T>* start, AVLNode<K, T>** ret) {
        if (start->_right == nullptr) {
            // we found the greatest.
            *ret = start;
            return start->_left;
        }
        
        start->_right = _removeGreatest(start->_right, ret);
        start->setHeight();
        return _fixRotations(start);
    }

    AVLNode<K, T>* _remove(const K& key, AVLNode<K, T> *curr,
                                                AVLNode<K, T>** ret) {
        if (curr == nullptr)
            return nullptr;

        if (equals(key, curr->_key)) {
            _size--;
            *ret = curr;
            if (curr->_left && curr->_right) {
                AVLNode<K, T>* predecessor = nullptr;
                curr->_left = _removeGreatest(curr->_left, &predecessor);
                predecessor->_left = curr->_left;
                predecessor->_right = curr->_right;
                curr = predecessor;
            } else if (curr->_left) {
                return curr->_left;
            } else {
                // if it's a leaf, curr->_right will be a nullptr anyway.
                return curr->_right;
            }
        } else if (less(key, curr->_key)) {
            curr->_left = _remove(key, curr->_left, ret);
        } else {
            curr->_right = _remove(key, curr->_right, ret);
        }

        curr->setHeight();
        return _fixRotations(curr);
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
            os << '<' << curr->_key << ", " << curr->_data << '>'
                << ':' << curr->_height << '$' << curr->getBF();
            if (curr->_left || curr->_right) {
                _printHelper(os, curr->_left, shift + 1);
                _printHelper(os, curr->_right, shift + 1);
            }
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

    bool _equals(AVLNode<K, T>* mine, AVLNode<K, T>* his) {
        return (mine == nullptr && his == nullptr)
            || (mine->_key == his->_key
                && mine->_data == his->_data
                && _equals(mine->_left, his->_left)
                && _equals(mine->_right, his->_right));
    }

public:
    explicit AVLTree() :
        _root(nullptr),
        _size(0) {}

    AVLTree(AVLTree<K, T, L>& other) {
        if (!other._size) {
            _root = nullptr;
            _size = 0;
            return;
        }

        // We are going to go through the other tree BFS style, and we are going
        // to use two parallel queues: one to hold the other tree's nodes, and
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

        int nodesInLastRow = 1 << (other.height());
        // i % nodesInLastRow == i & mod
        int mod = ~((-1) << other.height());

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

            *copy = new AVLNode<K, T>(orig->_height, orig->_key, orig->_data);

            if (orig->_left) {
                end++;
                otherNodes[end & mod] = orig->_left;
                myNodes[end & mod] = &((*copy)->_left);
            }
            if (orig->_right) {
                end++;
                otherNodes[end & mod] = orig->_right;
                myNodes[end & mod] = &((*copy)->_right);
            }
        }

        assert(end == other._size - 1);

        _size = other._size;
    }

    AVLTree(AVLTree<K, T, L>&& other) {
        _root = nullptr;
        _size = 0;

        std::swap(_root, other._root);
        std::swap(_size, other._size);
    }

    ~AVLTree() { clear(); }

    class Iterator : public std::iterator<std::bidirectional_iterator_tag,
                                            std::pair<const K&, T&>> {
    friend class AVLTree<K, T, L>;
    private:
        // We're going to be using the vector as a stack for in-order traversal.
        // The "bool" portion refers to whether we have covered a node. This is
        // necessary to figure out whether we need to examine it or not.
        std::vector<std::pair<AVLNode<K, T>*, bool>> _history;
        int _ptr;

        // The methods below are used to fill the Iterator correctly - 
        std::pair<AVLNode<K, T>*, bool>& _peek() {
            return _history[_ptr];
        }

        const std::pair<AVLNode<K, T>*, bool>& _cpeek() const {
            return _history[_ptr];
        }

        void _push(AVLNode<K, T>* node, bool covered) {
            _history.push_back(std::make_pair(node, covered));
            _ptr++;
        }

        void _pop() {
            _history.pop_back();
            _ptr--;
        }
        
    public:
        explicit Iterator() : _ptr(-1) {};

        Iterator& operator++() {
            if (_ptr < 0)
                throw std::runtime_error("Can't iterate past end().");

            // We have now covered this node.
            _peek().second = true;

            // First, attempt to seek out a child successor. We start by looking
            // at the current node's right child, and then we keep going all
            // the way down on the right child's left side.
            // As we make our way down, add to the stack and mark each node as
            // "not covered" (we haven't visited each yet).
            // Note that if the current node didn't have a right child, the
            // statement below and the loop immediately following it do nothing.
            AVLNode<K, T>* next = _peek().first->_right;

            while (next) {
                _push(next, false);
                next = next->_left;
            }

            // If our starting node had a right child, then the top of our
            // stack should hold a node that is "not covered", i.e.
            // _peek().second should be false. If that is the case, the loop
            // below will do nothing.
            // Conversely, if the starting node didn't have a right child, 
            // the loop above did nothing and we are still pointing to our
            // current node, that we have marked "covered". Thus, we can start
            // popping off all the covered nodes:
            while (_history.size() && _peek().second) {
                _pop();
            }

            return *this;
        }

        Iterator operator++(int) {
            auto ret = *this;
            ++(*this);
            return ret;
        }

        bool operator==(const Iterator& other) const {
            return _history.size() == other._history.size() &&
                (!_history.size() ? true : (_cpeek() == other._cpeek()));
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

        std::pair<const K&, T&> operator*() {
            if (!_history.size())
                throw std::runtime_error("Cannot dereference end() iterator.");

            return std::pair<const K&, T&>(_history[_ptr].first->_key,
                                    _history[_ptr].first->_data);
        }
    };

    AVLTree<K, T, L>& operator=(AVLTree<K, T, L> other) {
        other.swap(*this);
        return *this;
    }

    bool operator==(const AVLTree<K, T, L>& other) {
        return _equals(_root, other._root);
    }

    void swap(AVLTree<K, T, L>& other) {
        std::swap(_root, other._root);
        std::swap(_size, other._size);
    }

    void clear() {
        if (!_size)
            return;
        _clear(_root);
        _root = nullptr;
        _size = 0;
    }

    void add(K key, T data) {
        auto newNode = new AVLNode<K, T>(0, std::move(key), std::move(data));

        if (!_size) {
            _root = newNode;
            _size = 1;
            return;
        }

        _root = _add(_root, newNode);
    }

    T remove(const K& key) {
        if (!_size)
            throw std::runtime_error("Empty tree: element not found.");

        AVLNode<K, T>* found = nullptr;
        _root = _remove(key, _root, &found);

        if (!found)
            throw std::runtime_error("Element not found!");

        T data = std::move(found->_data);
        delete found;

        return data;
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
        return (_root) ? _root->_height : -1;
    }

    Iterator begin() {
        if (!_size)
            return end();

        Iterator ret;

        auto curr = _root;
        while (curr) {
            ret._push(curr, false);
            curr = curr->_left;
        }

        return ret;
    }

    Iterator end() {
        return Iterator();
    }

    Iterator find(const K& key) {
        if (!_size)
            return end();

        Iterator ret;
        
        auto curr = _root;
        while (curr) {
            ret._push(curr, false);
            if (equals(key, curr->_key))
                break;
            if (less(key, curr->_key))
                curr = curr->_left;
            else {
                ret._peek().second = true;
                curr = curr->_right;
            }
        }

        if (!curr) // not found
            return end();

        return ret;
    }

    friend std::ostream& operator<<(std::ostream& os, const AVLTree& tree) {
        tree._print(os);
        return os;
    }
};
} // namespace vk_data