/********************************************************
 * Description : singleton template of boost
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef SINGLETON_HPP
#define SINGLETON_HPP


template <typename T>
class Singleton
{
public:
    static T & instance()
    {
        static T obj;
        creator.do_nothing();
        return obj;
    }

private:
    struct object_creator
    {
        object_creator()
        {
            Singleton<T>::instance();
        }

        inline void do_nothing() const
        {
            // do nothing
        }
    };

    static object_creator creator;
};

template <typename T> typename Singleton<T>::object_creator Singleton<T>::creator;


#endif // SINGLETON_HPP
