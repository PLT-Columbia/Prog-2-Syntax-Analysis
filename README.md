# Programming Assignment 2 (Syntax and Semantic Analysis)

### Course Summary

Course: COMS 4115 Programming Languages and Translators (Fall 2020)  
Website: https://www.rayb.info/fall2020  
University: Columbia University  
Instructor: Prof. Baishakhi Ray


### Logistics
* **Announcement Date:** Wednesday, September 30, 2020
* **Due Date:** Wednesday, October 14, 2020 by 11:59 PM. **No extensions!**
* **Total Points:** 100

### Assignment Objectives
1. You **will learn** about the AST structure of the code.
2. You will get familiar with different AST APIs provided by clang/llvm. 
3. You will get familiar with clang's ASTVisiter. 
4. You **will learn** how to traverse AST. 
5. You **will learn** how to make semantic reasoning based on AST. 

### Assignment
You have already implemented a (albeit very simple) lexical analyzer/ tokenizer that converts character sequence in 
code to token sequence. As you learned in the lecture, this token sequence is the processed by the parser and 
**Abstract Syntax Tree (AST)** is generated. In this assignment, we will use `clang` compiler to generate AST. 
We will learn how advanced analysis on the syntax tree through 2 different case studies. 

### Generating the Syntax Tree
In previous assignment, you have seen how to build LLVM. `clang` compiler is a part of LLVM package. 
For any code written in `C` (or `C++`), you can generate (and visualize) the AST using clang. 

First let the shell/terminal know where you built you llvm.
```sh
export LLVM_HOME=[base directory where you built your llvm];
```
We recommend adding this line to your `$HOME/.bashrc` file. 

Then try the following command,
```sh
$LLVM_HOME/build/bin/clang -cc1 -ast-dump gcd.c -I /usr/include/ \
        -I $LLVM_HOME/build/lib/clang/12.0.0/include/;
```

You should take some time to look at the output. Write a couple of sentences in the [outputs/Ast_Thoughts.txt](outputs/Ast_Thoughts.txt)
 file describing how you perceived the output, what information are available in that output.    
 
Alternatively, you can generate visual representation of the AST. 
Try running the following command. 
 
```sh
export LLVM_HOME=[base directory where you built your llvm];
$LLVM_HOME/build/bin/clang -cc1 -ast-view gcd.c -I /usr/include/ \
        -I $LLVM_HOME/build/lib/clang/12.0.0/include/;
```

This will generate a `dot` file in your `/tmp/` directory. Look out for the dot file name in your screen. Then run, 
```
dot -Tpdf /tmp/<dot file name> -o output/AstView.pdf
```
Look at the pdf file generated try to match that with output generated in the previous step. 

### Analyzing code based on the AST
AST is not just for viewing. You can do much more than that using AST. One example task that you can do by visiting 
AST is _Warn developers when a variable name does not follow a convention (e.g. camel casing)._ By traversing the AST, if you find a Variable Name, 
you can check whether that name matches with your convention. If not, you can generate an warning.

In this assignment, however, we will implement AST analysis for two different tasks. 
## Task 1 : Finding a recursive function. 
[Recursive functions](https://en.wikipedia.org/wiki/Recursion_(computer_science)) are often compact, 
easy to read, and easy to understand. But they often incur huge overhead due to 
the [call stack](https://en.wikipedia.org/wiki/Call_stack) maintenance, which is a major concern if you are developing for a resource constrained system. 
Your job in this task is to 
determine whether a given function is [direct recursive](https://www.educative.io/courses/recursion-for-coding-interviews-in-cpp/BnKojpzLl2W) 
or not. We will describe the tools that you will use for this task in [a later section](#getting-started-with-the-coding).

#### Example
```c
1. int recursive_factorial(int n1){
2.      if (n1 <= 1){
3.		    return 1;
4.	    }
5.	    return n1 * recursive_factorial(n1-1);
6. }
```
In this `C` code, the function `recursive_factorial` is a direct recursive function, since in its body, there is a call
to itself (line 5). In contrast, the following function is not recursive. Even though there is function call at 
line 4, the callee is not the function itself.
```c
1. int iterative_factorial(int n1){
2.	    int res = 1;
3.	    for(int i = 0; i <= n3; i++){
4.		    res = mult(res, i); // multiplication of res and i.
5.	    }
6.	    return res;
7. }
``` 

## Task 2 : Variable Redefinition Warning Policy.

According to `C` language specification, it is perfectly legal to re-define a variable with the same name in an inner scope.
For example, 
```c
1. void foo(){
2.      int i = 4, j = 5;
3.      for (i = 0; i < 6; i++){
4.          int j = 9;
5.          // Do something.
6.      }
7. }
```
Variable `j` is redefined in line 4, which was already defined in an outer scope (line 2). While such redefinition 
is legal, there is a high chance that it is a developer mistake, and we need to build a tool to warn them.
In the above example, we want to warn the developer, 
```c
Redefining variable : "j" at line 4 which is initially defined at line 2
```
We need to implement a policy (let us call it **Variable Redefinition Warning Policy**) that checks the developer
written code and warns them if there is a policy violation. 

The following case, however, **does not** have any variable redifinition, thus should not generate any warning. 
```c
 1. int gcd_recursive(int m2, int n2) {
 2.     int r = m2 % n2;
 3.     if (r != 0) {
 4.         int k4 = 0;
 5.  	    return gcd_recursive(n2,r);
 6.      }
 7.      else {
 8.  	    int k4 = n2;
 9.         return k4;
10.      }
11.  }
```
Even though `k4` variable is defined in line 4 and again in line 8. The earlier definition (at line 4) is not visible 
at line 8, i.e. those are in different scopes. 

#### Some points to consider for Task2
1. You may consider there will be no global variable.
2. Changing value of variable is **NOT** considered a definition. For instance, 
```c
1. void foo(){
2.      int i = 4, j = 5;
3.      for (i = 0; i < 6; i++){
4.          int j = 9;
5.          // Do something.
6.      }
7. }
```
In this example, at line 3, value of variable `i` is changed, but it still is the same variable, thus not a redefinition. A variable will be considered redefined, when another varible with the same name (with same or different data types) is **re-declared** in an inner scope.  

    
### Point Breakdown
* **Visualizing and Understanding AST** - 10
* **Task 1** (Detection of Recursive function) - 40
* **Task 2** (Implementation of Variable Redefinition Warning Policy) - 50 

## Getting Started with the Coding 
To implement the above two tasks, we will build a [clang tool](https://clang.llvm.org/docs/LibTooling.html) that 
uses [llvm/clang's Recursive AST Visitor API](https://clang.llvm.org/doxygen/classclang_1_1RecursiveASTVisitor.html).
We have provided all the setup code to get started. However, we strongly recommend students to go over the
API documentations of clang tooling and AST visitos and understand the basic workflow. 
 
### Steps
1. Create a folder named `clang-hw2` under `$LLVM_HOME/clang/tools`.
2. Copy the [ClangHw2.cpp](src/ClangHw2.cpp), and [CMakeLists.txt](src/CMakeLists.txt) files into 
`$LLVM_HOME/clang/tools/clang-hw2`.
3. Edit `$LLVM_HOME/clang/tools/CmakeLists.txt` file and add line `add_clang_subdirectory(clang-hw2)`. 
4. Now go to `$LLVM_HOME/build` and run `make`. When the build is successfully finished, it will generate 
a binary file named `clang-hw2` in `$LLVM_HOME/build/bin`. 
5. Now run this binary using the following command <br/>
`$LLVM_HOME/build/bin/clang-hw2 examples/gcd.c --`

### Deep into the code.

The [`FunctionVisitor`](src/ClangHw2.cpp#L36) class is a recursive AST visitor, which implements 3 visitors
for 3 different types of AST nodes. The [`VisitForStmt`](src/ClangHw2.cpp#L143) is called when clang's AST visitor
encounters a [`ForStmt`](https://clang.llvm.org/doxygen/classclang_1_1ForStmt.html) type AST node. You **DO NOT** 
have to do anything with this function. We are providing it to give you a head start in AST Visitor. 

* For Task 1, we implemented [`VisitFunctionDecl`](src/ClangHw2.cpp#L116), which calls helper function 
[`isRecursiveFunction`](src/ClangHw2.cpp#L64) and decides whether that function is recursive or not. You have 
to implement this `isRecursiveFunction`. 
* For Task 2, [`VisitFunctionDecl`](src/ClangHw2.cpp#L116) calls the function [`analyzeRedifinition`](src.ClangHw2.cpp#L69) 
inside the function. Inside the `analyzeRedifinition`, if you encounter a variable being re-defined, 
you should call the helper funtion [`printVarReDeclarationInformation`](src/ClangHw2.cpp#L28) with the variable name, 
line number where it was initially defined, and line number where it is being redefined. 

When you fully implemented both the tasks and run the tool with [`gcd.c`](gcd.c), you will see the following output
```
gcd_recursive - recursive
Redefining variable : "k4" at line 7 which is initially defined at line 2
```

***N.B.*** We have provided some other helper functions. You can print things as you are developing. However, make sure
you use the helper functions that we provided to print your output. Please **DO NOT** change any helper function 
or any other print formatting (also remove any print statement you added before submission).  


## Extra Credit - 50 points
We can do a lot of interesting things using AST. In task 1 you learned how to identify recursive functions. 
In this extra credit part, you will get some experience on automatic code formatting by analyzing an AST.
If there is function call in your code like 

`foo( 1,        2      , 3    ,    5)` 

you need to format it 

`foo (1, 2, 3, 5)`.

More specifically, you have to format the call expression as `<callee><space>(arg1,<space>arg2, ...,<space>argn)`. 
The `<space>` represents a single space character `' '`. Note that the callee and/or arguments of a function may also be a function call.


#### Code and Logistics 
From the `VisitFunctionDecl` function, we call [`analyzeCallExpressionReformat`](src/ClangHw2.cpp#L69) where, we do a [Depth First Search (DFS)](https://en.wikipedia.org/wiki/Depth-first_search) on the AST. While doing this DFS, if we encounter any [`CallExpr`]() type node, we call the [`formatFunctionCall`](src/ClangHw2.cpp#L79). 
You have to implement this function, so that the function call expression is formatted. 

Here are some examples:
1. This following code is very hard to read,
```c
 1. int bar(int k){
 2.     foo(
 3.   3,
 4.                     foo (
 5.     too(    ) ,
 6. foo(
 7.                   5  ,  too (   )
 8.        )            )
 9.                           );
10.        return 0;
11. }
```
However, when you run your reformatter tool, the function call to `foo` at line 2 should be formatted as
```c
foo (3, foo (too (), foo (5, too ())))
```
Now it much more easy to understand the function call. 

2. Another example,
```c
 1. typedef int (*FuncPtr)(int, int);
 2. 
 3. int addNum(int a, int b) {
 4.     return a + b;
 5. }
 6.
 7. int mulNum(int a, int b) {
 8.   return a * b;
 9. }
10.
11. FuncPtr getFunc(int op) {
12.     return op == 1 ? &addNum :
13.           op == 2 ? &mulNum :
14.           (FuncPtr)0;
15. }
16.
17. int main() {
18.     int ret = getFunc( 
19.           1 +   0   )(  5 , 6   );
20.     return 0;
21. }
```
Here, there is a function call at line 18. However, the callee is not a direct function, rather another `CallExpr`. You have to reformat that too. 
The formatted code that you should generate
```c
getFunc (1 +   0) (5, 6)
```

You may assume the following constraints:
1. You have to reformat only the `CallExpr` type node. If you encounter any other node (for instance, `1 +   0 ` in line 19 of example 2, which is a `BinaryExpr`), you should copy the code as is from the input source. We have provided a helper function [`getSource`](src/ClangHw2.cpp#L61) to copy 
the input code corresponding to a node. 

2. The callee or arguments of a function call will be either a pure function call or a pure non- function call. For example, we will not test this case:
```
foo(bar(3) + 1, 9 + bar(6))
```

3. We will only test C codes. You **don't** need to handle function calls in C++ like operator overloading, user defined literal, etc.


### Submission
1. Copy the completed `ClangHw2.cpp` file from your `$LLVM_HOME/clang/tools/clang-hw2/` directory to this projects `src`
folder. 
2. Fillup the writeup in [outputs/Ast_Thoughts.txt](outputs/Ast_Thoughts.txt). 
3. Copy the executable `clang-hw2` from your `$LLVM_HOME/build/bin` directory to this projects `outputs` directory.
4. Commit.
5. Push.

## Piazza
If you have any questions about this programming assignment, please post them in the Piazza forum for the course, and an instructor will reply to them as soon as possible. Any updates to the assignment itself will be available in Piazza.


## Disclaimer
This assignment belongs to Columbia University. It may be freely used for educational purposes.
