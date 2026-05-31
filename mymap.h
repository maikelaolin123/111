#ifndef MYMAP_H
#define MYMAP_H

template <typename K, typename V>
class MyMap
{
private:
    struct Node
    {
        K key;
        V value;
    };

    Node *data;
    int capacity;
    int count;

    void expand()
    {
        int newCapacity = capacity * 2;
        Node *newData = new Node[newCapacity];

        for(int i = 0; i < count; i++)
        {
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    MyMap()
    {
        capacity = 10;
        count = 0;
        data = new Node[capacity];
    }

    MyMap(const MyMap<K, V> &other)
    {
        capacity = other.capacity;
        count = other.count;
        data = new Node[capacity];

        for(int i = 0; i < count; i++)
        {
            data[i] = other.data[i];
        }
    }

    MyMap<K, V>& operator=(const MyMap<K, V> &other)
    {
        if(this != &other)
        {
            delete[] data;

            capacity = other.capacity;
            count = other.count;
            data = new Node[capacity];

            for(int i = 0; i < count; i++)
            {
                data[i] = other.data[i];
            }
        }

        return *this;
    }

    ~MyMap()
    {
        delete[] data;
    }

    void insert(const K &key, const V &value)
    {
        for(int i = 0; i < count; i++)
        {
            if(data[i].key == key)
            {
                data[i].value = value;
                return;
            }
        }

        if(count >= capacity)
        {
            expand();
        }

        data[count].key = key;
        data[count].value = value;
        count++;
    }

    bool contains(const K &key) const
    {
        for(int i = 0; i < count; i++)
        {
            if(data[i].key == key)
            {
                return true;
            }
        }

        return false;
    }

    V value(const K &key, const V &defaultValue = V()) const
    {
        for(int i = 0; i < count; i++)
        {
            if(data[i].key == key)
            {
                return data[i].value;
            }
        }

        return defaultValue;
    }

    V& operator[](const K &key)
    {
        for(int i = 0; i < count; i++)
        {
            if(data[i].key == key)
            {
                return data[i].value;
            }
        }

        if(count >= capacity)
        {
            expand();
        }

        data[count].key = key;
        data[count].value = V();
        count++;

        return data[count - 1].value;
    }

    void remove(const K &key)
    {
        for(int i = 0; i < count; i++)
        {
            if(data[i].key == key)
            {
                for(int j = i; j < count - 1; j++)
                {
                    data[j] = data[j + 1];
                }

                count--;
                return;
            }
        }
    }

    int size() const
    {
        return count;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    void clear()
    {
        count = 0;
    }
};

#endif // MYMAP_H
