#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm> // std::random_access_iterator_tag
#include <cstddef> // size_t
#include <stdexcept> // std::out_of_range
#include <type_traits> // std::is_same
#include <iostream>

template <class T>
class Vector {
public:
    class iterator;
private:
    T* array;
    size_t _capacity, _size;

    // You may want to write a function that grows the vector
    void grow() {
        if (_capacity == 0){
            _capacity = 1;
        }
        else{
            _capacity *= 2;
        }
        T* temp = new T[_capacity];
        for(size_t i = 0; i < _size; i++){
            temp[i] = std::move(array[i]);
        }
        delete[] array;
        array = temp;

    }

public:
    Vector() noexcept: array(nullptr), _capacity(0), _size(0) {}
    Vector(size_t count, const T& value): array(new T[count]), _capacity(count), _size(count) {
        for (size_t i = 0; i < count; i++){
            array[i] = value;
        }
    }
    explicit Vector(size_t count): array(new T[count]), _capacity(count), _size(count) {
        for (size_t i = 0; i < count; i++){
            array[i] = T();
        }
    }
    Vector(const Vector& other): array(new T[other._capacity]), _capacity(other._capacity), _size(other._size) {
        for (size_t i = 0; i < _size; i++){
            array[i] = other.array[i];
        }
    }
    Vector(Vector&& other) noexcept: array(other.array), _capacity(other._capacity), _size(other._size) {
        other.array = nullptr;
        other._capacity = 0;
        other._size = 0;
    }

    ~Vector() {delete[] array;}

    Vector& operator=(const Vector& other) {
        if (this != &other){
            delete[] array;
            _capacity = other._capacity;
            _size = other._size;
            array = new T[_capacity];
            for (size_t i = 0; i < _size; i++){
                array[i] = other.array[i];
            }
        }
        return *this;
    }
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other){
            delete[] array;
            _capacity = other._capacity;
            _size = other._size;
            array = other.array;
            other.array = nullptr;
            other._capacity = 0;
            other._size = 0;
        }
        return *this;
    }

    iterator begin() noexcept { return iterator(array); }
    iterator end() noexcept { return iterator(array + _size); }

    [[nodiscard]] bool empty() const noexcept { return (_size == 0); }
    size_t size() const noexcept { return _size; }
    size_t capacity() const noexcept { return _capacity; }

    T& at(size_t pos) {
        if ((pos > _size - 1) || pos < 0){
            throw std::out_of_range("out of range");
        }else {
            return array[pos];
        }
    }
    const T& at(size_t pos) const {
        if ((pos > _size - 1) || pos < 0){
            throw std::out_of_range("out of range");
        }else {
            return array[pos];
        }
    }
    T& operator[](size_t pos) { return array[pos]; }
    const T& operator[](size_t pos) const { return array[pos]; }
    T& front() { return array[0]; }
    const T& front() const { return array[0]; }
    T& back() { return array[_size - 1]; }
    const T& back() const { return array[_size - 1]; }

    void push_back(const T& value) {
        if (_size == _capacity){grow();}
        array[_size] = value;
        _size += 1;
    }
    void push_back(T&& value) {
        if (_size == _capacity){grow();}
        insert(end(),std::move(value));
    }
    void pop_back() { erase(end());}

    iterator insert(iterator pos, const T& value) {
        size_t index = pos - begin();
        if (_size == _capacity){grow();}
        for (size_t i = (_size); i > (index); i--){
            array[i] = std::move(array[i-1]);
        }
        array[index] = value;
        ++_size;
        return begin() + index;
    }
    iterator insert(iterator pos, T&& value) {
        size_t index = pos - begin();
        if (_size == _capacity){grow();}
        for (size_t i = (_size); i > (index); i--){
            array[i] = std::move(array[i-1]);
        }
        array[index] = std::move(value);
        ++_size;
        return begin() + index;
    }
    iterator insert(iterator pos, size_t count, const T& value) {
        ptrdiff_t index = pos - begin();
        if (count == 0){return pos;}
        while((_size + count) > _capacity){
            grow();
        }
        for (ptrdiff_t i = _size - 1; i >= index; i--){
            array[i+count] = std::move(array[i]);
        }
        for (size_t i = index; i < index + count; i++){
            array[i] = value;
        }
        _size += count;
        return (begin() + index);

    }
    iterator erase(iterator pos) {
        size_t index = pos - begin();
        for (size_t i = index; i < _size - 1; i++){
            array[i] = std::move(array[i+1]);
        }
        _size--;
        if (_size == index){return end();}
        else{
            return begin() + index;
        }
    }
    iterator erase(iterator first, iterator last) {
        size_t first_index = first - begin();
        size_t last_index = last - begin();
        size_t diff = last_index - first_index;
        for (size_t i = last_index; i < _size; i++){
            array[i-diff] = std::move(array[i]);
        }
        _size -= diff;
        
        return begin() + first_index;
    }

    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    private:
        // Points to some element in the vector (or nullptr)
        T* _ptr;

    public:
        iterator() : _ptr(nullptr){}
        iterator(T* ptr) : _ptr(ptr){}

        // This assignment operator is done for you, please do not add more
        iterator& operator=(const iterator&) noexcept = default;

        [[nodiscard]] reference operator*() const noexcept { return *_ptr; }
        [[nodiscard]] pointer operator->() const noexcept { return _ptr; }

        // Prefix Increment: ++a
        iterator& operator++() noexcept {
            ++_ptr;
            return *this;
        }

        // Postfix Increment: a++
        // The value returned should be the iterator before it was moved forward in the container
        iterator operator++(int) noexcept {
            iterator temp = *this;
            _ptr++;
            return temp;
        }

        // Prefix Decrement: --a
        // The value returned should be the iterator moved backwards in the container
        iterator& operator--() noexcept {
            _ptr--;
            return *this;
        }

        // Postfix Decrement: a--
        // The value returned should be the iterator before it was moved backwards in the container
        iterator operator--(int) noexcept {
            iterator temp = *this;
            _ptr--;
            return temp;
        }

        // Should move the iterator forward by an offset and return the new iterator
        iterator& operator+=(difference_type offset) noexcept {
            _ptr += offset;
            return *this;
        }
        
        // Should return a copy of the iterator moved forward by an offset
        // NOTE: The iterator itself should not be moved, only the copy
        [[nodiscard]] iterator operator+(difference_type offset) const noexcept { 
            iterator temp = *this;
            return temp += offset;
        }
        
        // Should move the iterator backwards by an offset and return the new iterator
        iterator& operator-=(difference_type offset) noexcept {
            _ptr -= offset;
            return *this;
        }

        // Should return a copy of the iterator moved backwards by an offset
        // NOTE: The iterator itself should not be moved, only the copy
        [[nodiscard]] iterator operator-(difference_type offset) const noexcept {
            iterator temp = *this;
            temp -= offset;
            return temp;
        }

        // Should return the difference between two iterators. This should be the offset betweent the
        // elements in memory.
        [[nodiscard]] difference_type operator-(const iterator& rhs) const noexcept {
            return (_ptr - rhs._ptr);
        }

        // Should return the element at the location offset from the iterator
        [[nodiscard]] reference operator[](difference_type offset) const noexcept {
            iterator temp = *this;
            temp += offset;
            return *temp;
        }

        // Used for comparing iterators
        // NOTE: This should be used to compare the iterators, not the elements they point to
        // HINT: Because iterators mimic pointers, many if not all of these operations can be done
        //       on the underlying pointers
        [[nodiscard]] bool operator==(const iterator& rhs) const noexcept {return (_ptr == rhs._ptr);}
        [[nodiscard]] bool operator!=(const iterator& rhs) const noexcept {return (_ptr != rhs._ptr);}
        [[nodiscard]] bool operator<(const iterator& rhs) const noexcept {return (_ptr < rhs._ptr);}
        [[nodiscard]] bool operator>(const iterator& rhs) const noexcept {return (_ptr > rhs._ptr);}
        [[nodiscard]] bool operator<=(const iterator& rhs) const noexcept {return (_ptr <= rhs._ptr);}
        [[nodiscard]] bool operator>=(const iterator& rhs) const noexcept {return (_ptr >= rhs._ptr);}
    };


    void clear() noexcept {
        for(size_t i = 0; i < _size; i++){
            array[i] = 0;
        }
        _size = 0;
    }
};

// This ensures at compile time that the deduced argument _Iterator is a Vector<T>::iterator
// There is no way we know of to back-substitute template <typename T> for external functions
// because it leads to a non-deduced context
namespace {
    template <typename _Iterator>
    using is_vector_iterator = std::is_same<typename Vector<typename _Iterator::value_type>::iterator, _Iterator>;
}

template <typename _Iterator, bool _enable = is_vector_iterator<_Iterator>::value>
[[nodiscard]] _Iterator operator+(typename _Iterator::difference_type offset, _Iterator const& iterator) noexcept { return iterator + offset; }

#endif
