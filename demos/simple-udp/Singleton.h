#pragma once

namespace yinpsoft
{
class NonCopyable
{
protected:
    NonCopyable() {}
    ~NonCopyable() {}

private: // emphasize the following members are private
    NonCopyable(const NonCopyable &);
    NonCopyable &operator=(const NonCopyable &);
};

template <class T>
class Singleton : public NonCopyable
{
private:
    static T &instance;
    // include this to provoke instantiation at pre-execution time
    static void use(T const &) {}
    static T &get_instance()
    {
        static T t;
        // refer to instance, causing it to be instantiated (and
        // initialized at startup on working compilers)
        // assert(! detail::singleton_wrapper<T>::m_is_destroyed);
        use(instance);
        return static_cast<T &>(t);
    }

public:
    static T &get_mutable_instance()
    {
        return get_instance();
    }
    static const T &get_const_instance()
    {
        return get_instance();
    }
};
}; // namespace yinpsoft