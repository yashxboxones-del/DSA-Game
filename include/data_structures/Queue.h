#pragma once

// =============================================================
// Queue.h
// Custom FIFO (First In, First Out) Queue data structure.
//
// In this college project, the Queue is used for:
//   - GAME EVENT SYSTEM: Storing and processing gameplay events
//     (coin pickups, enemy defeats, damage events, checkpoints)
//     in the exact sequence they occur.
//
// Principles demonstrated:
//   - FIFO (First In, First Out): First event enqueued is the first processed
//   - Linked-node implementation (O(1) enqueue and O(1) dequeue)
//   - Dynamic memory management without memory leaks
// =============================================================

#include <cstddef>
#include <stdexcept>
#include <string>

// -------------------------------------------------------------
// GameEvent Types
// Categorizes events generated during gameplay.
// -------------------------------------------------------------
enum class EventType
{
    COIN_COLLECTED,      // Player picked up a coin (value = points)
    ENEMY_DEFEATED,      // Player stomped an enemy (value = points)
    PLAYER_DAMAGED,      // Player took damage (value = damage amount)
    CHECKPOINT_REACHED,  // Player reached a checkpoint (value = checkpoint index)
    LEVEL_COMPLETED      // Player completed the level
};

// -------------------------------------------------------------
// GameEvent Struct
// Represents an event packet passed through the Queue.
// -------------------------------------------------------------
struct GameEvent
{
    EventType type;
    int value;
    std::string message;

    GameEvent(EventType t = EventType::COIN_COLLECTED, int val = 0, const std::string& msg = "")
        : type(t), value(val), message(msg)
    {
    }
};

// -------------------------------------------------------------
// Queue Template Class
// -------------------------------------------------------------
template <typename T>
class Queue
{
private:
    // Node structure
    struct Node
    {
        T data;
        Node* next;

        Node(const T& val)
            : data(val), next(nullptr)
        {
        }
    };

    Node* m_front; // Pointer to the front element (next to be dequeued)
    Node* m_rear;  // Pointer to the rear element (most recently enqueued)
    int m_size;    // Number of elements currently in the queue

public:
    // ---------------------------------------------------------
    // Constructor
    // ---------------------------------------------------------
    Queue()
        : m_front(nullptr), m_rear(nullptr), m_size(0)
    {
    }

    // ---------------------------------------------------------
    // Destructor
    // Frees all remaining nodes.
    // ---------------------------------------------------------
    ~Queue()
    {
        clear();
    }

    // ---------------------------------------------------------
    // Copy Constructor (Deep Copy)
    // ---------------------------------------------------------
    Queue(const Queue& other)
        : m_front(nullptr), m_rear(nullptr), m_size(0)
    {
        Node* current = other.m_front;
        while (current != nullptr)
        {
            enqueue(current->data);
            current = current->next;
        }
    }

    // ---------------------------------------------------------
    // Copy Assignment Operator (Deep Copy)
    // ---------------------------------------------------------
    Queue& operator=(const Queue& other)
    {
        if (this != &other)
        {
            clear();
            Node* current = other.m_front;
            while (current != nullptr)
            {
                enqueue(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    // ---------------------------------------------------------
    // enqueue()
    // Inserts a new element at the REAR of the queue in O(1) time.
    // ---------------------------------------------------------
    void enqueue(const T& value)
    {
        Node* newNode = new Node(value);

        if (m_rear == nullptr)
        {
            // Queue is currently empty
            m_front = newNode;
            m_rear = newNode;
        }
        else
        {
            m_rear->next = newNode;
            m_rear = newNode;
        }

        m_size++;
    }

    // ---------------------------------------------------------
    // dequeue()
    // Removes the element from the FRONT of the queue in O(1) time.
    // Throws std::underflow_error if the queue is empty.
    // ---------------------------------------------------------
    void dequeue()
    {
        if (isEmpty())
        {
            throw std::underflow_error("Cannot dequeue from an empty Queue");
        }

        Node* toDelete = m_front;
        m_front = m_front->next;

        if (m_front == nullptr)
        {
            m_rear = nullptr; // Queue is now empty
        }

        delete toDelete;
        m_size--;
    }

    // ---------------------------------------------------------
    // front()
    // Returns a reference to the element at the FRONT of the queue.
    // ---------------------------------------------------------
    T& front()
    {
        if (isEmpty())
        {
            throw std::underflow_error("Queue is empty: no front element");
        }
        return m_front->data;
    }

    const T& front() const
    {
        if (isEmpty())
        {
            throw std::underflow_error("Queue is empty: no front element");
        }
        return m_front->data;
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
    // Dequeues all elements until empty.
    // ---------------------------------------------------------
    void clear()
    {
        while (!isEmpty())
        {
            dequeue();
        }
    }
};
