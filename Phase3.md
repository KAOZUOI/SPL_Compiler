# Phase 3

## Basic
We generate IR code by translate.c firstly, and optimize it in a separated part optimize.c.  
Our translation adopts the naming convention that variable names follow the pattern $tn$ or $vn$, and $labeln$ for label names.  
Finally all of IR codes are connected by a linkedlist defined in tac.c.  

## Optional Features
##### Struct
Structure variables can appear in the program, and they can be declared as function parameters. Still, assignment
operations will not be directly performed on a structure variable.  
Test:  
```
struct Student {
    int ID;
    int score;
};

int main() {
    struct Student x, y;
    x.ID = 0;
    x.score = 90;
    y.ID = 1;
    y.score = 100;
    write(x.ID);
    write(x.score);
    write(y.ID);
    write(y.score);
    return 0;
}
``` 
Result:  
```
FUNCTION main :
DEC v0 8
DEC v1 8
t0 := &v0
*t0 := #0
t2 := &v0 + #4
*t2 := #90
t4 := &v1
*t4 := #1
t6 := &v1 + #4
*t6 := #100
t8 := &v0
t9 := *t8
WRITE t9
t10 := &v0 + #4
t11 := *t10
WRITE t11
t12 := &v1
t13 := *t12
WRITE t13
t14 := &v1 + #4
t15 := *t14
WRITE t15
RETURN #0
```  

##### Multi-dimensional arrays
1-D arrays can be declared as function parameters, and multi-dimensional arrays can be defined as local variables.  
Test:  
```
struct Num {
    int id;
    int matrix[10][10];
};

int add (int a, int b) {
    return a + b;
}

struct Num test(int row, int col) {
    struct Num x;
    x.id = 1;
    x.matrix[0][0]= add(row, col);
    x.matrix[0][1]= add(row, col + 1);
    return x;
}
```  
Result:  
```
FUNCTION add :
PARAM v0
PARAM v1
t0 := v0 + v1
RETURN t0

FUNCTION test :
PARAM v2
PARAM v3
DEC v4 404
t1 := &v4
*t1 := #1
ARG v3
ARG v2
t3 := CALL add
t4 := &v4 + #4
*t4 := t3
t6 := v3 + #1
ARG t6
ARG v2
t7 := CALL add
t8 := &v4 + #4
t9 := t8 + #4
*t9 := t7
RETURN &v4
```  
## IR optimize

1. 判断块前是否有无条件跳转的 `GOTO` 指令，如果有则不可达。不可达块中出现 `LABEL` 则认为该不可达块结束，后面的块恢复为可达。  
Test:  
```
int main() {
    if (1>2) {
        int a=0;
        a=1+a;
        write(a);
    } else {
        write(- 1);
    }

    return 0;
}

```
Result:  
Raw:  
```
FUNCTION main :
IF #1 <= #2 GOTO label0
v0 := #0
v0 := #1 + v0
WRITE v0
GOTO label2
LABEL label0 :
t1 := #0 - #1
WRITE t1
LABEL label2 :
RETURN #0

```  
Optimized:  
```
FUNCTION main :
GOTO label0
LABEL label0 :
t1 := #0 - #1
WRITE t1
LABEL label2 :
RETURN #0

```  
2. `IF` 指令跳转如果恒为真，则检查跳转的label,如果未定义，则后方的块都不可达知道找到label定义，然后删除 `IF` 和对应label。否则不处理。

3. `IF` 指令条件取反，节省GOTO指令

