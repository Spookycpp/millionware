#pragma once
#include <cassert>

inline int UtlMemory_CalcNewAllocationCount(int allocation_count, int grow_size, int new_size, int bytes_item) {
    if (grow_size) {
        allocation_count = ((1 + ((new_size - 1) / grow_size)) * grow_size);
    }
    else {
        if (!allocation_count) {
            // Compute an allocation which is at least as big as a cache line...
            allocation_count = (31 + bytes_item) / bytes_item;
        }

        while (allocation_count < new_size) {
            allocation_count *= 2;
        }
    }

    return allocation_count;
}

template <class T, class I = int> class CUtlMemory {
  public:
    bool is_idx_valid(I i) const {
        long x = i;
        return (x >= 0) && (x < allocation_amount);
    }

    T &operator[](I i);
    const T &operator[](I i) const;

    T *Base() {
        return memory;
    }

    int number_allocation() const {
        return allocation_amount;
    }

    void grow(int num) {
        assert(num > 0);

        auto old_allocation_amount = allocation_amount;
        // Make sure we have at least number_allocation + num allocations.
        // Use the grow rules specified for this memory (in grow_size)
        int allocation_requested = allocation_amount + num;

        int new_allocation_count = UtlMemory_CalcNewAllocationCount(allocation_amount, grow_size, allocation_requested, sizeof(T));

        // if m_allocation_requested wraps index type I, recalculate
        if ((int) (I) new_allocation_count < allocation_requested) {
            if ((int) (I) new_allocation_count == 0 && (int) (I)(new_allocation_count - 1) >= allocation_requested) {
                --new_allocation_count; // deal w/ the common case of allocation_amount == MAX_USHORT + 1
            }
            else {
                if ((int) (I) allocation_requested != allocation_requested) {
                    // we've been asked to grow memory to a size s.t. the index type can't address the requested amount of memory
                    assert(0);
                    return;
                }
                while ((int) (I) new_allocation_count < allocation_requested) {
                    new_allocation_count = (new_allocation_count + allocation_requested) / 2;
                }
            }
        }

        allocation_amount = new_allocation_count;

        if (memory) {
            auto ptr = new uint8_t[allocation_amount * sizeof(T)];

            memcpy(ptr, memory, old_allocation_amount * sizeof(T));
            memory = (T *) ptr;
        }
        else {
            memory = (T *) new uint8_t[allocation_amount * sizeof(T)];
        }
    }

  protected:
    T *memory;
    int allocation_amount;
    int grow_size;
};

template <class T, class I> T &CUtlMemory<T, I>::operator[](I i) {
    assert(is_idx_valid(i));
    return memory[i];
}

template <class T, class I> const T &CUtlMemory<T, I>::operator[](I i) const {
    assert(is_idx_valid(i));
    return memory[i];
}

template <class T> void destruct(T *memory) {
    memory->~T();
}

template <class T> T *construct(T *memory) {
    return ::new (memory) T;
}

template <class T> T *copy_construct(T *memory, T const &src) {
    return ::new (memory) T(src);
}

template <class T, class A = CUtlMemory<T>> class CUtlVector {
    typedef A CAllocator;

    typedef T *iterator;
    typedef const T *const_iterator;

  public:
    T &operator[](int i);
    const T &operator[](int i) const;

    T &element(int i) {
        return memory[i];
    }

    const T &element(int i) const {
        assert(is_valid_index(i));
        return memory[i];
    }

    T *get_elements() {
        return elements;
    }

    T *base() {
        return memory.base();
    }

    int count() const {
        return size;
    }

    void clear() {
        for (int i = size; --i >= 0;) {
            destruct(&element(i));
        }

        size = 0;
    }

    bool is_valid_index(int i) const {
        return (i >= 0) && (i < size);
    }

    void shift_elements_right(int elem, int num = 1) {
        assert(is_valid_index(elem) || (size == 0) || (num == 0));
        int number_to_move = size - elem - num;
        if ((number_to_move > 0) && (num > 0))
            memmove(&element(elem + num), &element(elem), number_to_move * sizeof(T));
    }

    void shift_elements_left(int elem, int num = 1) {
        assert(is_valid_index(elem) || (size == 0) || (num == 0));
        int number_to_move = size - elem - num;
        if ((number_to_move > 0) && (num > 0))
            memmove(&element(elem), &element(elem + num), number_to_move * sizeof(T));
    }

    void grow_vector(int num = 1) {
        if (size + num > memory.number_allocation()) {
            memory.grow(size + num - memory.number_allocation());
        }

        size += num;
    }

    int insert_before(int elem) {
        // Can insert at the end
        assert((elem == count()) || is_valid_index(elem));

        grow_vector();
        shift_elements_right(elem);
        construct(&element(elem));
        return elem;
    }

    int add_to_head() {
        return insert_before(0);
    }

    int add_to_tail() {
        return insert_before(size);
    }

    int insert_before(int elem, const T &src) {
        // Can't insert something that's in the list... reallocation may hose us
        assert((base() == NULL) || (&src < base()) || (&src >= (base() + count())));

        // Can insert at the end
        assert((elem == count()) || is_valid_index(elem));

        grow_vector();
        shift_elements_right(elem);
        copy_construct(&element(elem), src);
        return elem;
    }

    int AddToTail(const T &src) {
        // Can't insert something that's in the list... reallocation may hose us
        return InsertBefore(size, src);
    }

    int Find(const T &src) const {
        for (int i = 0; i < count(); ++i) {
            if (element(i) == src)
                return i;
        }
        return -1;
    }

    void remove(int elem) {
        destruct(&element(elem));
        shift_elements_right(elem);
        --size;
    }

    bool find_and_remove(const T &src) {
        int elem = Find(src);
        if (elem != -1) {
            remove(elem);
            return true;
        }
        return false;
    }

    iterator begin() {
        return base();
    }

    const_iterator begin() const {
        return base();
    }

    iterator end() {
        return base() + count();
    }

    const_iterator end() const {
        return base() + count();
    }

  protected:
    CAllocator memory;
    int size;
    T *elements;
};

template <typename T, class A> T &CUtlVector<T, A>::operator[](int i) {
    assert(i < size);
    return memory[i];
}

template <typename T, class A> const T &CUtlVector<T, A>::operator[](int i) const {
    assert(i < size);
    return memory[i];
}
