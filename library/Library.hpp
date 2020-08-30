#include <string>
#include <string_view>
#include <iostream>
#include <functional>


struct HelloProvider{
    std::string sentence{"Hello, world!"};
    std::string_view operator()(){
        return sentence;
    }
};

struct HelloConsumer{
    void operator()(std::function<std::string_view()> provider){
        std::cout << std::invoke(provider) << std::endl;
    }
};