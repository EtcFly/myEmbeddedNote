
# GCC学习

### 命令参数
`-ansi` 在c模式相当于`-std=c90`, 在c++模式相当于`-std=c++98`,这将关闭某些与`ISO C90`（编译c代码时）或者标准c++（在编译c++代码时）不兼容的功能.   
`-std=` 确认语言标准, 仅当编译器为c或c++时才支持此选项。可能的值有 `c90` `c89` `iso9899:1990`（支持所有ISO C90程序（禁用了与ISO C90冲突的某些GNU扩展）。与-ansi对于C代码相同。）`iso9899：199409`(ISO C90在修订版1中进行了修改。) `c99` `c9x`,还有一些其他，详情可以查询gcc支持的标准。   
`-fallow-parameterless-variadic-functions` 接受不带命名参数的可变参数
`-fms-extensions` 接受一些非标准头文件构造,如 在C ++代码中，这允许结构中的成员名称类似于先前的类型声明  
```
typedef int UOW;   
struct ABC {   
  UOW UOW;   
};   
```   
仅在此选项中接受结构和联合中未命名字段的某些情况。
`-flax-vector-conversions` 允许进行隐式类型转换
`-funsigned-char` 让char为unsigned无符号类型,其等效于`-fno-signed-char`
`-fsigned-char` 让char为signed char有符号类型 其等效于`-fno-unsigned-char`
`-fdata-sections` 为每一个函数或者数据项闯将一个输出节，函数名或者数据项的名称就是这个输出文件节的名称。

