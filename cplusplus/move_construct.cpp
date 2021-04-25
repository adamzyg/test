#include <iostream>
using namespace std;
class demo{
public:
    demo():num(new int(0)){
        cout<<"construct!"<<endl;
    }

    demo(const demo &d):num(new int(*d.num)){
        cout<<"copy construct!"<<endl;
    }
    //添加移动构造函数
    demo(demo &&d):num(d.num){
        d.num = NULL;
        cout<<"move construct!"<<endl;
    }
    ~demo(){
        cout<<"class destruct!"<<endl;
    }
private:
    int *num;
};
demo get_demo(){
    return demo();
}
int main(){
    demo a = get_demo();
    return 0;
}

/*
 *
 * 可以看到，在之前 demo 类的基础上，我们又手动为其添加了一个构造函数。和其它构造函数不同，此构造函数使用右值引用形式的参数，又称为移动构造函数。并且在此构造函数中，num 指针变量采用的是浅拷贝的复制方式，同时在函数内部重置了 d.num，有效避免了“同一块对空间被释放多次”情况的发生。

在 Linux 系统中使用g++ demo.cpp -o demo.exe -std=c++0x -fno-elide-constructors命令执行此程序，输出结果为：
construct!
move construct!
class destruct!
move construct!
class destruct!
class destruct!

通过执行结果我们不难得知，当为 demo 类添加移动构造函数之后，使用临时对象初始化 a 对象过程中产生的 2 次拷贝操作，都转由移动构造函数完成。

我们知道，非 const 右值引用只能操作右值，程序执行结果中产生的临时对象（例如函数返回值、lambda 表达式等）既无名称也无法获取其存储地址，所以属于右值。当类中同时包含拷贝构造函数和移动构造函数时，如果使用临时对象初始化当前类的对象，编译器会优先调用移动构造函数来完成此操作。只有当类中没有合适的移动构造函数时，编译器才会退而求其次，调用拷贝构造函数。
在实际开发中，通常在类中自定义移动构造函数的同时，会再为其自定义一个适当的拷贝构造函数，由此当用户利用右值初始化类对象时，会调用移动构造函数；使用左值（非右值）初始化类对象时，会调用拷贝构造函数。


读者可能会问，如果使用左值初始化同类对象，但也想调用移动构造函数完成，有没有办法可以实现呢？

默认情况下，左值初始化同类对象只能通过拷贝构造函数完成，如果想调用移动构造函数，则必须使用右值进行初始化。C++11 标准中为了满足用户使用左值初始化同类对象时也通过移动构造函数完成的需求，新引入了 std::move() 函数，它可以将左值强制转换成对应的右值，由此便可以使用移动构造函数。
 */
