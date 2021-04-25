#include <iostream>
using namespace std;

class demo{
public:
   demo():num(new int(0)){
      cout<<"construct!"<<endl;
   }
   //拷贝构造函数
   demo(const demo &d):num(new int(*d.num)){
      cout<<"copy construct!"<<endl;
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
如上所示，我们为 demo 类自定义了一个拷贝构造函数。该函数在拷贝 d.num 指针成员时，必须采用深拷贝的方式，即拷贝该指针成员本身的同时，还要拷贝指针指向的内存资源。否则一旦多个对象中的指针成员指向同一块堆空间，这些对象析构时就会对该空间释放多次，这是不允许的。

可以看到，程序中定义了一个可返回 demo 对象的 get_demo() 函数，用于在 main() 主函数中初始化 a 对象，其整个初始化的流程包含以下几个阶段：
1. 执行 get_demo() 函数内部的 demo() 语句，即调用 demo 类的默认构造函数生成一个匿名对象；
2. 执行 return demo() 语句，会调用拷贝构造函数复制一份之前生成的匿名对象，并将其作为 get_demo() 函数的返回值（函数体执行完毕之前，匿名对象会被析构销毁）；
3. 执行 a = get_demo() 语句，再调用一次拷贝构造函数，将之前拷贝得到的临时对象复制给 a（此行代码执行完毕，get_demo() 函数返回的对象会被析构）；
4. 程序执行结束前，会自行调用 demo 类的析构函数销毁 a。
zyg@linux-OptiPlex-3046:~/test/cplusplus$ g++ construct.cpp -o construct
zyg@linux-OptiPlex-3046:~/test/cplusplus$ ./construct
construct!
class destruct!
zyg@linux-OptiPlex-3046:~/test/cplusplus$ g++ construct.cpp -o construct  -fno-elide-constructors
zyg@linux-OptiPlex-3046:~/test/cplusplus$ ./construct
construct!                <-- 执行 demo()
copy construct!       <-- 执行 return demo()
class destruct!         <-- 销毁 demo() 产生的匿名对象
copy construct!       <-- 执行 a = get_demo()
class destruct!         <-- 销毁 get_demo() 返回的临时对象
class destruct!         <-- 销毁 a
 */
