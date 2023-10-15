# deriv: 一键符号求导

---

## INTRODUCTION

对表达式求偏导.

## BUILD

> make

## USAGE

> derive [VARIABLE] (= [VALUE]) | [FORMULA]

`[VARIABLE]` 指定自变量.

`= [VALUE]` 可选. 将额外输出表达式在 `[VARIABLE]` = `[VALUE]` 处的值.

`[FORMULA]` 指定待求导表达式, 中缀表达式.

会输出 $\frac{\partial [FORMULA]}{\partial [VARIABLE]}$ . 

可用运算符有:

##### 单目
ln, sin, cos, tan, arcsin, arccos, arctan.

##### 双目
+, -, *, /, ^, log.

## SAMEPLE

下面的例子展示了对 $x ^ {sin(x)}$ 求导的结果.
> deriv x | ^ x sin x <br>
> x ^ sin(x) * ( cos(x) * ln(x) + sin(x)/x )

## TODO

支持自变量列表, 即对多个自变量分别求偏导.