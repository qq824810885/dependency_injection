# Qt版依赖注入容器

## 使用方法

- 将该仓库源码clone到要使用的项目下，然后在项目管理文件中包含
```
include(dependency_injection/dependency_injection.pri)
```

## 如何编写代码
所有需要自动注入的对象必须派生自QObject；目标对象也需派生自QObject，原因是容器的实现依赖了Qt的元对象系统。
> NOTE:被管理对象在容器中的key是对象类型名字，所以如果类型名字出现重复则注册对象时会失败，对象注册到容器中后，对象的内存管理则完全交由容器管理，不能再主动delete对象指针

- 引入头文件
```cpp
#include "dependency_injection/instantiationservice.h"
```
- 管理对象
```cpp
IInstantiationService *iis = new InstantiationService(nullptr);//一般在一个应用内只构造一个根容器对象
iis->registerSingleton<IFoo>(new Foo());
```

- 注入对象
```cpp
#include "dependency_injection/instantiationservice.h"
class Bar : public IBar{
    Q_OBJECT
public:
    explicit Bar(int a,int b,QObject *parent = nullptr) : IBar(parent){
    
    }
    double someMethod() override{
        if(foo){
            foo->someMethod();
        }
    }
    //定义可以被自动注入的字段，第一个参数为字段类型，第一个参数为字段的变量名称
    IOC_INJECTABLE(IFoo,foo)
private:
    IFoo *foo{nullptr};
};
```
- 自动装配对象
```cpp
IBar *c = iis->newObject<Bar>(1,2);//通过容器创建的对象会将对象中定义的可自动注入的字段注入相应类型的对象。
c->someMethod();
```

- 控制被管理对象的生效范围

某些情况下，我们注册到容器内的对象不想被全局共享，这种情况下可，通过容器的继承生成一个子容器，子容器里面注册的对象在父容器内查不到，而父容器内注册的对象可在子容器共享，这样需要全局共享的对象可放在根容器中，单独模板内的对象，可继承出来一个单独的容器对象，然后将对象注册到子容器对象中。示例代码如下：
```cpp
SharedPointer<IInstantiationService> childIIS = iis->inherited();
childIIS->registerSingleton<IFoo>(new Foo());//此时注册的对象在iis容器中不会被自动注入到其他对象里面
```