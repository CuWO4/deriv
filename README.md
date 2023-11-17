# deriv: 一键符号求导

---

## INTRODUCTION

对表达式求偏导.

## BUILD

> `make`

## USAGE

> `derive [VARIABLE] [FORMULA]`

`[VARIABLE]` 指定自变量.

`[FORMULA]` 指定待求导表达式, 中缀表达式.

会输出 $\frac{\partial [FORMULA]}{\partial [VARIABLE]}$ . 

可用运算符有:

##### 单目

ln, sin, cos, tan, arcsin, arccos, arctan.

##### 双目

+, -, mul, /, ^.

(不适用*是因为它会被当作通配符)

## SAMEPLE

下面的例子展示了对 $x ^ {sin(2x)}$ 求导的结果.

> `deriv x ^ x sin mul 2 x` <br>
> `x ^ sin( 2 * x ) * ( sin( 2 * x ) / x +  ln( x ) * cos( 2 * x ) * 2 )`

使用 `make run` 命令查看几个结果.

## TODO

支持求值操作.