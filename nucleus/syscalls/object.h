/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <mutex>
#include <unordered_map>

class ObjectBase
{
    void* m_data;
    u32 m_type;

public:
    virtual void* getData()=0;
    virtual u32 getType()=0;
};

template <typename T>
class Object : public ObjectBase
{
    T* m_data;
    u32 m_type;

public:
    Object(T* data, const u32 type)
    {
        m_type = type;
        m_data = data;
    }

    ~Object()
    {
        delete m_data;
    }

    void* getData()
    {
        return m_data;
    }

    u32 getType()
    {
        return m_type;
    }
};

class ObjectManager
{
    std::unordered_map<u32, ObjectBase*> m_objects;
    std::mutex m_mutex;
    u32 m_current_id = 1;

public:
    ~ObjectManager()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_objects.clear();
    }

    // Iterator
    std::unordered_map<u32, ObjectBase*>::const_iterator begin() { return m_objects.begin(); }
    std::unordered_map<u32, ObjectBase*>::const_iterator end() { return m_objects.end(); }

    // Add a new object to the set and return the generated ID
    template <typename T>
    u32 add(T* data, const u32 type)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_objects[m_current_id] = new Object<T>(data, type);
        return m_current_id++;
    }

    // Get a pointer to the object data of a certain ID
    template <typename T>
    T* get(const u32 id)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_objects.find(id) == m_objects.end()) {
            return nullptr;
        }

        return (T*)m_objects[id]->getData();
    }

    // Remove an object from the set given its ID (deleting the object data)
    bool remove(const u32 id)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_objects.find(id) == m_objects.end()) {
            return false;
        }

        auto item = m_objects.find(id);
        m_objects.erase(item);
        return true;
    }

    // Test if a certain ID is present
    bool check(const u32 id)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_objects.find(id) != m_objects.end();
    }
};
