The Latest Version is 1.5.2

---

# 算法简介

本算法可以用于快速生成一个二进制字符串或者用户输入的内容对应的键值，且键值具有抗逆向性。
本算法具有一定的兼容性。你可以使用指定版本号来防止数据的丢失。
本算法可以用于保存密码，但请注意**用户的盐和软件的胡椒需要开发者自行生成。如果用于高风险用途（例如暴露公网）请设置强密码并将PreLen设置的足够大！**

---

# 算法规划

本算法以后会推出内置的加密算法，可以用于二进制文件的加密。

---

# 基本用法（按字典序）

## c12c2

`c12c2`(ClassOne to ClassTwo)是一个内置的使用`stringstream`的类型转换器。
你可以使用：

```cpp
GXPass::c12c2<SourceClass,TargetClass>(data)
```

* 其中`data`是你需要转换类型的内容。
* `c12c2`会把转换完的结果作为返回值返回。

## charset

`GXPass::charset`是一个默认的字典，包含了可打印ASCII字符的大部分。
- 你可以更改它的内容作为你的程序的默认字典。

## computeSafePassLen

`computeSafePassLen`是一个用于计算安全的`fullsafe`输出长度的实现，用于动态调整算法速度。
你可以使用：

```cpp
int GXPass::computeSafePassLen(int inputLength, int minPassLen = 6)
```

* 其中`inputLength`是你的输入内容的长度。
* 其中`minPassLen`是设定的最小安全输入长度，默认是6。

## compile

`compile`会按照自己的算法把输入的内容计算成一个全部由数字组成的字符串，长度不固定。请注意，该算法仍然不具有可逆性。
你可以使用：

```cpp
std::string GXPass::compile(const std::string data, int version = -1)
```

**一般与下面的`number2ABC`一起使用**。

* 其中`data`是你需要运算的字符串。
* 其中`version`是你需要运算的版本，建议使用version=2及以上，以下版本逆向风险较高。

## fullsafe

`fullsafe`可用于生成具有安全性的加密形式。请注意，该操作会丢失密码信息，并且无法逆向获取密码。
你可以使用：

```cpp
std::string fullsafe(const std::string data, int version = -1, int PassLen = 256,int preLen = 256, const std::string& chars = charset,int minPassLen = 6)
```

* `data`：需要运算的字符串。
* `version`：**指定`compile`内核版本，以便防止数据丢失**
* `PassLen`：输出长度，有三种模式：
  * 正整数：确定的输出长度
  * -1：不确定输出长度，由生成内容决定
  * -2：不确定输出长度，使用输入长度和最小安全长度的曲线公式计算
* `preLen`：预热长度。安全性可以随着预热长度的增加而增加，但是会导致生成时间和空间增加
* `chars`：指定字符集，如果不设定，会保持默认
* `minPassLen`：最小安全长度，配合`PassLen=-2`使用时，如果小于该长度，会导致输出较长

## number2ABC

`number2ABC`会把输入的数字字符串内容转化为由A-Z的大写字母表示的字符串，提高可读性和通用性。
你可以使用：

```cpp
std::string GXPass::number2ABC(const std::string data)
```

* `data`：需要运算的纯数字字符串。
* **注意**：不会判断字符串是否合法，需要你自行保证输入合法性或上游算法保证无非数字内容。

## number2safestring

`number2safestring`是另一个`number2ABC`的实现，更安全且均匀。
你可以使用：

```cpp
std::string GXPass::number2safestring(
    std::string data,
    const std::string& chars = charset
)
```

* `data`：与`number2ABC`中的输入相同
* `chars`：使用的字符集，默认使用`charset`

## number2safestring\_Private\_CannotUse

`number2safestring_Private_CannotUse`是一个私有的逻辑实现，为了减少代码量创建的。

* **无需调用**，也没有任何意义

## sum dxsum1 dxsum2 kms

这些是内置的运算模块，您无需调用，也没有任何意义。

---
# 测试数据

下面是v1.6.0的部分测试数据。
## 雪崩测试
雪崩率42.4%（好像上一个版本的超高雪崩率是因为计算部分的代码写错了）
```
GXPass Full Strength Test
Threads: 16
Samples per test: 10000


[Avalanche] 100.00% | 10000/10000 | 35.00 ops/s

Average flipped bits: 434.78

All tests completed.
```
## 碰撞和均匀测试
PS:个人电脑跑不了太大太多，仅10000数据。

```
GXPass Full Strength Test
Threads: 16
Samples per test: 10000


[Distribution] 100.00% | 10000/10000 | 74.00 ops/s

Character Frequency:
v : 1.10%
e : 1.09%
% : 1.11%
2 : 1.07%
r : 1.09%
z : 1.10%
: : 1.11%
' : 1.08%
0 : 1.09%
) : 1.11%
L : 1.11%
] : 1.10%
W : 1.10%
Q : 1.10%
= : 1.09%
H : 1.10%
P : 1.10%
; : 1.09%
{ : 1.10%
* : 1.11%
G : 1.10%
C : 1.10%
< : 1.09%
| : 1.11%
N : 1.10%
1 : 1.10%
q : 1.10%
c : 1.11%
# : 1.11%
X : 1.11%
f : 1.11%
4 : 1.10%
_ : 1.09%
b : 1.10%
9 : 1.09%
y : 1.09%
, : 1.11%
( : 1.09%
h : 1.10%
Y : 1.10%
R : 1.10%
w : 1.10%
? : 1.11%
8 : 1.10%
d : 1.10%
U : 1.10%
T : 1.09%
I : 1.11%
J : 1.08%
a : 1.09%
Z : 1.11%
D : 1.10%
/ : 1.10%
o : 1.09%
O : 1.10%
- : 1.09%
m : 1.10%
^ : 1.11%
3 : 1.10%
s : 1.10%
M : 1.10%
A : 1.10%
n : 1.10%
E : 1.11%
@ : 1.11%
S : 1.10%
7 : 1.09%
k : 1.11%
g : 1.10%
x : 1.11%
F : 1.10%
B : 1.10%
} : 1.09%
> : 1.10%
V : 1.09%
! : 1.08%
+ : 1.09%
5 : 1.11%
i : 1.08%
l : 1.09%
j : 1.09%
$ : 1.11%
[ : 1.11%
t : 1.09%
K : 1.10%
" : 1.10%
u : 1.09%
p : 1.10%
& : 1.10%
. : 1.11%
6 : 1.11%
```
