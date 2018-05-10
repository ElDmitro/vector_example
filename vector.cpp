#include <cstddef>
#include <algorithm>

template <typename T>
class Vector {
public:
    Vector() : _data(nullptr), _size(0), _capacity(0) {}

    Vector(size_t capacity) : _size(capacity), _capacity(capacity) {
        void * rawData = operator new[](sizeof(T) * _capacity);

        _data = reinterpret_cast<T*>(rawData);

        for (size_t i = 0; i < _size; ++i)
            new (_data + i) T();
    }

    Vector(const Vector& other) : _capacity(other._size), _size(other._size) {
        void * rawData = operator new[](sizeof(T) * _capacity);

        _data = reinterpret_cast<T*>(rawData);

        for (size_t i = 0; i < _size; ++i)
            new (_data + i) T(other[i]);
    }

    Vector& operator=(const Vector& other)  {
        if (_capacity < other._size) {
            free();

            _capacity = _size = other._size;
            void * rawData = operator new[](sizeof(T) * _capacity);

            _data = reinterpret_cast<T*>(rawData);

            for (size_t i = 0; i < _size; ++i)
                new (_data + i) T(other[i]);

            return *this;
        }

        size_t i = 0;

        for (; i < std::min(_size, other._size); ++i)
            _data[i] = other[i];

        for (; i < other._size; ++i)
            new (_data + i) T(other[i]);

        for (; i < _size; ++i)
            (_data + i)->~T();

        _size = other._size;

        return *this;
    }

    ~Vector() {
        free();
    }

    void push_back(const T& value) {
        if (_size == _capacity)
            reallocate();

        new (_data + _size) T(value);
        ++_size;
    }

    void push_back(T&& value) {
        if (_size == _capacity)
            reallocate();

        new (_data + _size) T(std::move(value));
        ++_size;
    }

    void resize(size_t count) {
        if (count == _size)
            return;

        if (count <= _capacity) {
            for (size_t i = _size; i < count; ++i)
                new (_data + i) T();

            for (size_t i = count; i < _size; ++i)
                (_data + i)->~T();

            _size = count;

            return;
        }

        void * rawData = operator new[](sizeof(T) * count);
        T * tmpData = reinterpret_cast<T*>(rawData);

        size_t i = 0;
        for (; i < std::min(count, _size); ++i)
            new (tmpData + i) T(_data[i]);

        for (; i < count; ++i)
            new (tmpData + i) T();

        free();

        _size = _capacity = count;

        _data = tmpData;
    }

    void reserve(size_t count) {
        if (count <= _capacity)
            return;

        void * rawData = operator new[](sizeof(T) * count);
        T * tmpData = reinterpret_cast<T*>(rawData);

        for (size_t i = 0; i < _size; ++i)
            new (tmpData + i) T(_data[i]);

        _capacity = count;

        free();

        _data = tmpData;
    }

    void pop_back() {
        (_data + _size)->~T();

        --_size;
    }

    size_t size() {
        return _size;
    }

    size_t size() const {
        return _size;
    }

    T& operator[](size_t i) {
        return _data[i];
    }

    size_t capacity() {
        return _capacity;
    }

    size_t capacity() const {
        return _capacity;
    }

    const T& operator[](size_t i) const {
        return _data[i];
    }

    T * begin() {
        return _data;
    }

    T * end() {
        return _data + _size;
    }

private:
    /*
     * Вспомогательный метод, который расширяет ёмкость при попытке
     * добавления нового элемента
     */
    void reallocate() {
        _capacity = (_capacity != 0)?_capacity*2:1;

        void* rawData = operator new[](sizeof(T) * _capacity);
        T * tmpData = reinterpret_cast<T*>(rawData);

        for (size_t i = 0; i < _size; ++i)
            new (tmpData + i) T(_data[i]);

        free();
        _data = tmpData;
    }

    /*
     * Деструктурирует активные элементы
     * Высвобождает далее сырую память
     */
    void free() {
        for (size_t i = 0; i < _size; ++i)
            (_data + i)->~T();

        operator delete[](_data);
    }

    T * _data;
    size_t _size;
    size_t _capacity;
};
