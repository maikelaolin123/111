#ifndef MYVECTOR_H
#define MYVECTOR_H

template <typename T>
class MyVector
{
private:
    T *data;
    int capacity;
    int count;

    void expand()
    {
        int newCapacity = capacity * 2;
        T *newData = new T[newCapacity];

        for(int i = 0; i < count; i++)
        {
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    MyVector()
    {
        capacity = 10;
        count = 0;
        data = new T[capacity];
    }

    MyVector(const MyVector<T> &other)
    {
        capacity = other.capacity;
        count = other.count;
        data = new T[capacity];

        for(int i = 0; i < count; i++)
        {
            data[i] = other.data[i];
        }
    }

    MyVector<T>& operator=(const MyVector<T> &other)
    {
        if(this != &other)
        {
            delete[] data;

            capacity = other.capacity;
            count = other.count;
            data = new T[capacity];

            for(int i = 0; i < count; i++)
            {
                data[i] = other.data[i];
            }
        }

        return *this;
    }

    ~MyVector()
    {
        delete[] data;
    }

    void push_back(const T &value)
    {
        if(count >= capacity)
        {
            expand();
        }

        data[count] = value;
        count++;
    }

    void append(const T &value)
    {
        push_back(value);
    }

    void insert(int index, const T &value)
    {
        if(index < 0)
        {
            index = 0;
        }

        if(index > count)
        {
            index = count;
        }

        if(count >= capacity)
        {
            expand();
        }

        for(int i = count; i > index; i--)
        {
            data[i] = data[i - 1];
        }

        data[index] = value;
        count++;
    }

    int size() const
    {
        return count;
    }

    int length() const
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

    T& operator[](int index)
    {
        return data[index];
    }

    const T& operator[](int index) const
    {
        return data[index];
    }

    const T& at(int index) const
    {
        return data[index];
    }

    void removeAt(int index)
    {
        if(index < 0 || index >= count)
        {
            return;
        }

        for(int i = index; i < count - 1; i++)
        {
            data[i] = data[i + 1];
        }

        count--;
    }

    T* begin()
    {
        return data;
    }

    T* end()
    {
        return data + count;
    }

    const T* begin() const
    {
        return data;
    }

    const T* end() const
    {
        return data + count;
    }
};

#endif // MYVECTOR_H
