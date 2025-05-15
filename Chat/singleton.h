#ifndef SINGLETON_H
#define SINGLETON_H
#include"global.h"

template <typename T>
class singleton{
protected:
    singleton()=default;
    singleton(const singleton<T>&)=delete;
    singleton& operator = (const singleton<T>& st)=delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> getinstance()
    {
        static std::once_flag s_flag;
        std::call_once(s_flag,[&](){
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }
    void printaddress()
    {
        std::cout<<_instance.get()<<std::endl;
    }
    ~singleton()
    {
        std::cout<<"this is singleton destruct"<<std::endl;
    }
};
template <typename T>
std::shared_ptr<T>singleton<T>::_instance = nullptr;

#endif // SINGLETON_H
