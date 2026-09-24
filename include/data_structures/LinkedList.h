#pragma once

// =============================================================
// LinkedList.h
// Custom Singly Linked List data structure.
//
// In this college project, the Linked List is used for:
//   - ENEMY MANAGEMENT: Dynamically storing, traversing, and
//     removing enemies as they are defeated during gameplay.
//
// Concepts demonstrated:
//   - Nodes with data and next pointer
//   - Dynamic memory allocation (new/delete)
//   - Safe insertion and removal
//   - Linear traversal (O(N))
//   - Destructor ensuring zero memory leaks
// =============================================================

#include <cstddef>
#include <stdexcept>
#include <functional>

template <typename T>
class LinkedList
{
public:
    // ---------------------------------------------------------
    // Node structure
    // Each node stores an element of type T and a pointer to the next node.
    // ---------------------------------------------------------
    struct Node
    {
        T data;
        Node* next;

        Node(const T& val)
            : data(val), next(nullptr)
        {
        }
    };

    // ---------------------------------------------------------
    // Constructor
    // Initializes an empty list.
    // ---------------------------------------------------------
    LinkedList()
        : m_head(nullptr), m_tail(nullptr), m_size(0)
    {
    }

    // ---------------------------------------------------------
    // Destructor
    // Frees all allocated nodes to prevent memory leaks.
    // ---------------------------------------------------------
    ~LinkedList()
    {
        clear();
    }

    // ---------------------------------------------------------
    // Copy Constructor (Deep Copy)
    // ---------------------------------------------------------
    LinkedList(const LinkedList& other)
        : m_head(nullptr), m_tail(nullptr), m_size(0)
    {
        Node* current = other.m_head;
        while (current != nullptr)
        {
            insert(current->data);
            current = current->next;
        }
    }

    // ---------------------------------------------------------
    // Copy Assignment Operator (Deep Copy)
    // ---------------------------------------------------------
    LinkedList& operator=(const LinkedList& other)
    {
        if (this != &other)
        {
            clear();
            Node* current = other.m_head;
            while (current != nullptr)
            {
                insert(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    // ---------------------------------------------------------
    // insert() / push_back()
    // Appends an element to the end of the list in O(1) time
    // using the m_tail pointer.
    // ---------------------------------------------------------
    void insert(const T& value)
    {
        Node* newNode = new Node(value);

        if (m_head == nullptr)
        {
            m_head = newNode;
            m_tail = newNode;
        }
        else
        {
            m_tail->next = newNode;
            m_tail = newNode;
        }

        m_size++;
    }

    // ---------------------------------------------------------
    // removeAt()
    // Removes the element at the specified index (0-based).
    // Returns true if successfully removed, false if index invalid.
    // ---------------------------------------------------------
    bool removeAt(int index)
    {
        if (index < 0 || index >= m_size || m_head == nullptr)
        {
            return false;
        }

        Node* toDelete = nullptr;

        if (index == 0)
        {
            // Removing head
            toDelete = m_head;
            m_head = m_head->next;
            if (m_head == nullptr)
            {
                m_tail = nullptr;
            }
        }
        else
        {
            // Traverse to the node immediately before the one to delete
            Node* prev = m_head;
            for (int i = 0; i < index - 1; ++i)
            {
                prev = prev->next;
            }

            toDelete = prev->next;
            prev->next = toDelete->next;

            if (toDelete == m_tail)
            {
                m_tail = prev;
            }
        }

        delete toDelete;
        m_size--;
        return true;
    }

    // ---------------------------------------------------------
    // removeIf()
    // Removes all nodes satisfying the predicate function.
    // Crucial for removing defeated enemies:
    //   m_enemies.removeIf([](const Enemy& e) { return !e.isAlive(); });
    // Returns the number of elements removed.
    // ---------------------------------------------------------
    template <typename Predicate>
    int removeIf(Predicate pred)
    {
        int removedCount = 0;
        Node* current = m_head;
        Node* prev = nullptr;

        while (current != nullptr)
        {
            if (pred(current->data))
            {
                Node* toDelete = current;

                if (prev == nullptr)
                {
                    // Deleting head
                    m_head = current->next;
                    current = m_head;
                }
                else
                {
                    prev->next = current->next;
                    current = current->next;
                }

                if (toDelete == m_tail)
                {
                    m_tail = prev;
                }

                delete toDelete;
                m_size--;
                removedCount++;
            }
            else
            {
                prev = current;
                current = current->next;
            }
        }

        return removedCount;
    }

    // ---------------------------------------------------------
    // forEach()
    // Traverses the linked list and calls a function on each element.
    // Very clean and beginner-friendly for update/render loops.
    // ---------------------------------------------------------
    template <typename Func>
    void forEach(Func func)
    {
        Node* current = m_head;
        while (current != nullptr)
        {
            func(current->data);
            current = current->next;
        }
    }

    template <typename Func>
    void forEach(Func func) const
    {
        const Node* current = m_head;
        while (current != nullptr)
        {
            func(current->data);
            current = current->next;
        }
    }

    // ---------------------------------------------------------
    // get()
    // Returns reference to element at index (0-based).
    // ---------------------------------------------------------
    T& get(int index)
    {
        if (index < 0 || index >= m_size)
        {
            throw std::out_of_range("LinkedList index out of bounds");
        }

        Node* current = m_head;
        for (int i = 0; i < index; ++i)
        {
            current = current->next;
        }
        return current->data;
    }

    const T& get(int index) const
    {
        if (index < 0 || index >= m_size)
        {
            throw std::out_of_range("LinkedList index out of bounds");
        }

        const Node* current = m_head;
        for (int i = 0; i < index; ++i)
        {
            current = current->next;
        }
        return current->data;
    }

    // ---------------------------------------------------------
    // size() & isEmpty()
    // ---------------------------------------------------------
    int size() const
    {
        return m_size;
    }

    bool isEmpty() const
    {
        return m_size == 0;
    }

    // ---------------------------------------------------------
    // clear()
    // Deletes all nodes and resets pointers.
    // ---------------------------------------------------------
    void clear()
    {
        Node* current = m_head;
        while (current != nullptr)
        {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }

    // ---------------------------------------------------------
    // Direct pointer access for viva demonstration
    // ---------------------------------------------------------
    Node* getHead() { return m_head; }
    const Node* getHead() const { return m_head; }

private:
    Node* m_head;   // Pointer to the first node
    Node* m_tail;   // Pointer to the last node
    int m_size;     // Number of elements in the list
};
