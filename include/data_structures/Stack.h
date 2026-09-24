#pragma once

// =============================================================
// Stack.h
// Custom LIFO (Last In, First Out) Stack data structure.
//
// In this college project, the Stack is used for:
//   - CHECKPOINT / PLAYER HISTORY: Storing snapshots of player
//     states (position, health, score) as checkpoints are reached.
//     When player respawns or triggers recovery, the most recent
//     checkpoint is popped from the TOP of the stack (LIFO).
//
// Principles demonstrated:
//   - LIFO (Last In, First Out): Most recently saved checkpoint is restored first
//   - Linked-node implementation (O(1) push and O(1) pop)
//   - Dynamic memory management without memory leaks
// =============================================================

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <stdexcept>
#include <string>

// -------------------------------------------------------------
// PlayerState Struct
// Snapshot of player progress stored in the checkpoint Stack.
// -------------------------------------------------------------
struct PlayerState
{
    sf::Vector2f position;
    int health;
    int score;
    std::string checkpointName;

    PlayerState(const sf::Vector2f& pos = {0.0f, 0.0f}, int hp = 3, int sc = 0, const std::string& name = "Start")
        : position(pos), health(hp), score(sc), checkpointName(name)
    {
    }
};

// -------------------------------------------------------------
// Stack Template Class
// -------------------------------------------------------------
template <typename T>
class Stack
{
private:
    // Node structure
    struct Node
    {
        T data;
        Node* next;

        Node(const T& val, Node* n = nullptr)
            : data(val), next(n)
        {
        }
    };

    Node* m_top; // Pointer to the top element of the stack
    int m_size;  // Number of elements in the stack

public:
    // ---------------------------------------------------------
    // Constructor
    // ---------------------------------------------------------
    Stack()
        : m_top(nullptr), m_size(0)
    {
    }

    // ---------------------------------------------------------
    // Destructor
    // Frees all remaining nodes.
    // ---------------------------------------------------------
    ~Stack()
    {
        clear();
    }

    // ---------------------------------------------------------
    // Copy Constructor (Deep Copy)
    // ---------------------------------------------------------
    Stack(const Stack& other)
        : m_top(nullptr), m_size(0)
    {
        // To maintain correct order in deep copy, we reverse-traverse or copy helper
        if (other.m_top != nullptr)
        {
            // Build temporary array/copy
            Node* current = other.m_top;
            // Count and gather
            T* tempArray = new T[other.m_size];
            int count = other.m_size;
            for (int i = count - 1; i >= 0; --i)
            {
                tempArray[i] = current->data;
                current = current->next;
            }
            // Push from bottom to top
            for (int i = 0; i < count; ++i)
            {
                push(tempArray[i]);
            }
            delete[] tempArray;
        }
    }

    // ---------------------------------------------------------
    // Copy Assignment Operator (Deep Copy)
    // ---------------------------------------------------------
    Stack& operator=(const Stack& other)
    {
        if (this != &other)
        {
            clear();
            if (other.m_top != nullptr)
            {
                Node* current = other.m_top;
                T* tempArray = new T[other.m_size];
                int count = other.m_size;
                for (int i = count - 1; i >= 0; --i)
                {
                    tempArray[i] = current->data;
                    current = current->next;
                }
                for (int i = 0; i < count; ++i)
                {
                    push(tempArray[i]);
                }
                delete[] tempArray;
            }
        }
        return *this;
    }

    // ---------------------------------------------------------
    // push()
    // Inserts a new element at the TOP of the stack in O(1) time.
    // ---------------------------------------------------------
    void push(const T& value)
    {
        Node* newNode = new Node(value, m_top);
        m_top = newNode;
        m_size++;
    }

    // ---------------------------------------------------------
    // pop()
    // Removes the element from the TOP of the stack in O(1) time.
    // Throws std::underflow_error if the stack is empty.
    // ---------------------------------------------------------
    void pop()
    {
        if (isEmpty())
        {
            throw std::underflow_error("Cannot pop from an empty Stack");
        }

        Node* toDelete = m_top;
        m_top = m_top->next;
        delete toDelete;
        m_size--;
    }

    // ---------------------------------------------------------
    // top()
    // Returns reference to the element at the TOP of the stack.
    // ---------------------------------------------------------
    T& top()
    {
        if (isEmpty())
        {
            throw std::underflow_error("Stack is empty: no top element");
        }
        return m_top->data;
    }

    const T& top() const
    {
        if (isEmpty())
        {
            throw std::underflow_error("Stack is empty: no top element");
        }
        return m_top->data;
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
    // Pops all elements until the stack is empty.
    // ---------------------------------------------------------
    void clear()
    {
        while (!isEmpty())
        {
            pop();
        }
    }
};
