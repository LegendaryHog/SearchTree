#pragma once
#include <iterator>
#include <initializer_list>

namespace Container
{

namespace detail
{

template<typename T>
void construct(T* p, const T& rhs) {new (p) T{rhs};}

template<typename T>
void construct(T* p, T&& rhs) {new (p) T{std::move(rhs)};}

template<class T>
void destroy(T* p) {p->~T();}

template<std::forward_iterator It>
void destroy(It first, It last)
{
    while (first != last)
        destroy(&*first++);
}

template<typename T>
class VectorBuf
{
    using value_type = T;
    using pointer    = T*;
    using size_type  = typename std::size_t;
protected:
    size_type size_, used_ = 0;
    pointer data = nullptr;
protected:
    VectorBuf(size_type size = 0)
    :size_ {size}
     data_ {(size_ == 0) ? nullptr : static_cast<pointer>(::operator new(sizeof(value_type) * size_))}
    {}

    VectorBuf(const VectorBuf&)            = delete;
    VectorBuf& operator=(const VectorBuf&) = delete;

    void swap(VectorBuf& rhs) noexcept
    {
        std::swap(size_, rhs.size_);
        std::swap(used_, rhs.used_);
        std::swap(data_, rhs.data_);
    }

    VectorBuf(VectorBuf&& rhs) noexcept
    {
        swap(rhs);
    }

    VectorBuf& operator=(VectorBuf&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }

    virtual ~VectorBuf()
    {
        ::operator delete(data_);
    }
};

} // namespace detail

template<typename T>
class Vector final: private detail::VectorBuf<T>
{
    using value_type      = T;
    using pointer         = T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = typename std::size_t;
    using base            = detail::VectorBuf<T>;

    using base::size_;
    using base::used_;
    using base::data_;
public:
    Vector(size_type size = 0): base::VectorBuf(size) {}

    template<std::input_iterator InpIt>
    Vector(InpIt first, InpIt last): base::VectorBuf(std::distance(first, last))
    {
        while (first != last)
            construct(data_ + used_++, *first++);   
    }

    Vector(std::initializer_list<T> initlist)
    :Vector(initlist.begin(), initlist.end())
    {}

public:
    Vector(Vector&&)            = default;
    Vector& operator=(Vector&&) = default;

    Vector(const Vector& rhs): base::VectorBuf(rhs.used_)
    {
        for (size_type i = 0; i < used_; i++)
            detail::construct(data_ + i, rhs.data[i]);
    }

    Vcetor& operator=(const Vector& rhs)
    {
        auto cpy {rhs};
        std::swap(*this, cpy);
        return *this;
    }

    ~Vector()
    {
        destroy(data_, data_ + used_);
    }

public:
    size_type size() const {return used_;}
    size_type capacity() const {return size_};
    bool empty() const {return used_ == 0;}

    reference at(size_type index) &  
    {
        if(index >= used_)
            throw std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }
    const_reference at(size_type index) const&
    {
        if(index >= used_)
            throw std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }
    value_type at(size_type index) &&          
    {
        if(index >= used_)
            throw std::out_of_range{"try to get acces to element out of array"};
        return data_[index];
    }

public:
    void push(const value_type& val)
    {
        auto cpy {val};
        push(std::move(cpy));
    }

    void push(value_type&& val)
    {
        if (need_resize_up())
            resize_up();
        
        detail::construct(data_ + used_++, std::move(val));
    }
private:
    bool need_resize_up() const {used_ == size_};

    void resize_up()
    {
        Vector tmp (2 * size_ + 1);
        while (tmp.used_ < used_)
            tmp.push(std::move(data_[tmp.used_]));
        std::swap(*this, tmp);
    }
public:
    value_type top() const
    {
        if (empty())
            throw std::underflow_error{"try to get back from empty vector"};
        return data_[used_ - 1];
    }

    void pop()
    {
        if(empty())
            throw std::underflow_error{"try to pop element from empty vector"};
        used_--;
        destroy(data_ + used_);
    }

    value_type front()
    {
        if (empty())
            throw std::underflow_error{"try to get back from empty vector"};
        return data_[0];
    }

    class Iterator
    {
    public:
        using iterator_category = typename std::random_access_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

    private:
        pointer ptr_;
        
    public:
        explicit Iterator(pointer ptr = nullptr)
        :ptr_ {ptr}
        {}

        reference operator*()  const noexcept {return *ptr_;}
        pointer   operator->() const noexcept {return ptr_;}

        Iterator& operator++()
        {
            ptr_++;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp {*this};
            ++(*this);
            return tmp;
        }
            
        Iterator& operator--()
        {
            ptr_--;
            return *this;
        }

        Iterator operator--(int)
        {
            Iterator tmp {*this};
            --(*this);
            return tmp;
        }

        Iterator& operator+=(const difference_type& rhs)
        {
            ptr_ += rhs;
            return *this;
        }

        Iterator& operator-=(const difference_type& rhs)
        {
            ptr_ -= rhs;
            return *this;
        }

        reference operator[](const difference_type& diff) const
        {
            return *(Iterator{*this} += diff);
        }

        std::strong_ordering operator<=>(const Iterator& rhs) const = default;

        difference_type operator-(const Iterator& rhs) const
        {
            return ptr_ - rhs.ptr_;
        }

        Iterator operator+(const difference_type& diff) const
        {
            return (Iterator{*this} += diff);
        }

        Iterator operator-(const difference_type& diff) const
        {
            return (Iterator{*this} -= diff);
        }

        friend Iterator operator+(const difference_type& diff, const Iterator& itr)
        {
            return itr + diff;
        }
    }; //class Iterator

    class ConstIterator
    {
    public:
        using iterator_category = typename std::random_access_iterator_tag;
        using difference_type   = typename std::ptrdiff_t;
        using value_type        = T;
        using const_pointer     = const T*;
        using const_reference   = const T&;

    private:
        const_pointer ptr_;

    public:
        explicit ConstIterator(const_pointer ptr = nullptr)
        :ptr_ {ptr}
        {}

        const_reference operator*()  const noexcept {return *ptr_;}
        const_pointer   operator->() const noexcept {return ptr_;}

        ConstIterator& operator++()
        {
            ptr_++;
            return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator tmp {*this};
            ++(*this);
            return tmp;
        }
        
        ConstIterator& operator--()
        {
            ptr_--;
            return *this;
        }

        ConstIterator operator--(int)
        {
            ConstIterator tmp {*this};
            --(*this);
            return tmp;
        }

        ConstIterator& operator+=(const difference_type& rhs)
        {
            ptr_ += rhs;
            return *this;
        }

        ConstIterator& operator-=(const difference_type& rhs)
        {
            ptr_ -= rhs;
            return *this;
        }

        const_reference operator[](const difference_type& diff) const
        {
            return *(ConstIterator{*this} += diff);
        }

        std::strong_ordering operator<=>(const ConstIterator& rhs) const = default;

        difference_type operator-(const ConstIterator& rhs) const
        {
            return ptr_ - rhs.ptr_;
        }

        ConstIterator operator+(const difference_type& diff) const
        {
            return (ConstIterator{*this} += diff);
        }

        ConstIterator operator-(const difference_type& diff) const
        {
            return (ConstIterator{*this} -= diff);
        }

        friend ConstIterator operator+(const difference_type& diff, const ConstIterator& itr)
        {
            return itr + diff;
        }
    }; // class ConstIterator

    Iterator begin() {return Iterator{data_};}
    Iterator end()   {return Iterator{data_ + size_};}

    ConstIterator begin() const {return ConstIterator{data_};}
    ConstIterator end()   const {return ConstIterator{data_ + size_};}

    ConstIterator cbegin() const {return ConstIterator{data_};}
    ConstIterator cend()   const {return ConstIterator{data_ + size_};}
}; // class Vector

} // namespace Container