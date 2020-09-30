# COMS W4115: Programming Assignment 2.2 (Syntax and Semantic Analysis)


## Course Summary

Course: COMS 4115 Programming Languages and Translators (Fall 2020)  
Website: https://www.rayb.info/fall2020  
University: Columbia University  
Instructor: Prof. Baishakhi Ray


## Logistics
* **Announcement Date:** Wednesday, September 30, 2020
* **Due Date:** Wednesday, October 14, 2020 by 11:59 PM. **No extensions!**
* **Total Points:** 100

## Grading Breakdown
* **Visualizing and Understanding AST:** 10
* **Task 1 (Detection of Recursive function):** 40
* **Task 2 (Implementation of Variable Redefinition Warning Policy):** 50 

## Assignment Objectives

From this assignment:

1. You **will learn** about the AST structure of the code.
2. You will get familiar with different AST APIs provided by LLVM/Clang. 
3. You will get familiar with Clang's ASTVisitor. 
4. You **will learn** how to traverse an AST. 
5. You **will learn** how to reason about code semantics from an AST. 
6. You **will learn** how to write a simple code reformatter. 

## Assignment

You have already implemented a (albeit very simple) lexical analyzer/tokenizer that converts character sequences from a code stream to token sequences. As you learned in lecture, this token sequence is processed by the parser, and an **abstract syntax tree (AST)** is generated. In this assignment, you will use the `clang` compiler to generate an AST. Further, you will investigate two different case studies to analyze an AST.

### Generating the AST (10 Points)
In Programming Assignment 2.1, you saw how to build LLVM. The Clang compiler front-end leverages the LLVM back-end infrastructure. For any code written in `C` or `C++`, you can generate (and visualize) the AST using Clang.

First, let the shell/terminal know where you built LLVM. We recommend adding the following line to your `$HOME/.bashrc` file:
```sh
export LLVM_HOME=<base directory where you built your llvm>;
```
Next, try the following command:
```sh
$LLVM_HOME/build/bin/clang -cc1 -ast-dump gcd.c -I /usr/include/ \
        -I $LLVM_HOME/build/lib/clang/12.0.0/include/;
```

You should spend some time looking at the output. Please write a few sentences in the [outputs/AST.txt](outputs/AST.txt) file describing the following:
* What information is being presented in the output
* What you find interesting about the output

Alternatively, you can generate a visual representation of the AST. Try running the following command:
 
```sh
export LLVM_HOME=[base directory where you built your llvm];
$LLVM_HOME/build/bin/clang -cc1 -ast-view gcd.c -I /usr/include/ \
        -I $LLVM_HOME/build/lib/clang/12.0.0/include/;
```

This will generate a `dot` file in your `/tmp/` directory. Locate the created `dot` file and identify the filename. Run the following: 
```
dot -Tpdf /tmp/<dot file filename> -o output/AstView.pdf
```
Take a look at the PDF file that is generated, and try to match that with the output generated in the previous step. 

### Analyzing the AST

The AST is not just for viewing purposes; there are many powerful things you can do with an AST. For example, you can warn developers when a variable name does not follow a convention (*e.g.*, camel casing) by traversing an AST; if you find or "visit" a variable name, you can verify whether that name matches the intended convention. Otherwise, you can generate a warning.

In this particular assignment, you will investigate how to analyze an AST to extract important information for two different tasks.

### Task 1: Recursive Functions (40 Points)

[Recursive functions](https://en.wikipedia.org/wiki/Recursion_(computer_science)) are often compact, easy to read, and easy to understand. However, they often incur huge overhead due to [call stack](https://en.wikipedia.org/wiki/Call_stack) maintenance, which is a major concern if you are developing for a resource-constrained system. Your task here is to determine whether a given function is [direct recursive](https://www.educative.io/courses/recursion-for-coding-interviews-in-cpp/BnKojpzLl2W) 
or not. We will describe the tools that you will use for this task in a [later section](#getting-started).

#### Example
```c
1. int recursive_factorial(int n1){
2.      if (n1 <= 1){
3.		    return 1;
4.	    }
5.	    return n1 * recursive_factorial(n1 - 1);
6. }
```
In this `C` code snippet, the function `recursive_factorial` is a direct recursive function, since there is a call to itself inside the body (line 5). In contrast, the following function is not direct recursive; even though there is function call at 
line 4, the callee is not the function itself:
```c
1. int iterative_factorial(int n1){
2.	    int res = 1;
3.	    for(int i = 0; i <= n3; i++){
4.		    res = mult(res, i); // multiplication of res and i.
5.	    }
6.	    return res;
7. }
``` 

### Task 2: Variable Redefinition Warning Policy (50 Points)

According to `C` language specifications, it is perfectly legal to redefine a variable with the same name in an inner scope. For example:
```c
1. void foo() {
2.      int i = 4, j = 5;
3.      for (i = 0; i < 6; i++){
4.          int j = 9;
5.          // Do something
6.      }
7. }
```
Variable `j` is redefined in line 4, which was already defined in an outer scope (line 2). While such a redefinition is legal, there is a high chance that it is a developer mistake, and we need to build a tool to warn her.

In the above example, we want to warn the developer as follows: 
```c
Redefining variable : "j" at line 4 which is initially defined at line 2
```
In order to that, we need to implement a policy (let us call it **Variable Redefinition Warning Policy**) that checks the developer-written code and warns her if there is a policy violation.

As another example, note that the following case, however, **does not** have any variable redefinition and should not generate any warning:
```c
 1. int gcd_recursive(int m2, int n2) {
 2.     int r = m2 % n2;
 3.     if (r != 0) {
 4.         int k4 = 0;
 5.  	    return gcd_recursive(n2,r);
 6.     }
 7.     else {
 8.  		int k4 = n2;
 9.     	return k4;
10.  	}
11.  }
```
Even though the `k4` variable is defined in line 4 and again in line 8, the earlier definition (at line 4) is not visible at line 8, *i.e.*, these variables are in different scopes.

#### Points to Consider for Task 2
1. You may assume there will be no global variables.
2. Changing the value of a variable is **NOT** considered a redefinition. For instance:
```c
1. void foo() {
2.      int i = 4, j = 5;
3.      for (i = 0; i < 6; i++){
4.          int j = 9;
5.          // Do something
6.      }
7. }
```
In this example, at line 3, the value of variable `i` is changed, but because the variable is still the same, this is not a redefinition. A variable will be considered redefined when another variable with the same name (same or different data types) is re-declared in an inner scope.

## Getting Started
To implement the above two tasks, you will build a [Clang tool](https://clang.llvm.org/docs/LibTooling.html) that uses [llvm/clang's Recursive AST Visitor API](https://clang.llvm.org/doxygen/classclang_1_1RecursiveASTVisitor.html). We have provided all the setup code to get started. However, we strongly recommend that you go over the API documentation of Clang tooling and AST visitors and understand the basic workflow. 
 
### Steps
1. Create a folder named `clang-hw2` under `$LLVM_HOME/clang/tools`.
2. Copy the [ClangHw2.cpp](src/ClangHw2.cpp), and [CMakeLists.txt](src/CMakeLists.txt) files into 
`$LLVM_HOME/clang/tools/clang-hw2`.
3. Edit `$LLVM_HOME/clang/tools/CmakeLists.txt` file and add line `add_clang_subdirectory(clang-hw2)`. 
4. Now go to `$LLVM_HOME/build` and run `make`. When the build is successfully finished, it will generate 
a binary file named `clang-hw2` in `$LLVM_HOME/build/bin`. 
5. Now run this binary using the following command <br/>
`$LLVM_HOME/build/bin/clang-hw2 examples/gcd.c --`

### About the Code

The [`FunctionVisitor`](src/ClangHw2.cpp#L36) class is a recursive AST visitor, which implements three visitors for three different types of AST nodes. The [`VisitForStmt`](src/ClangHw2.cpp#L143) is called when Clang's ASTVisitor encounters a [`ForStmt`](https://clang.llvm.org/doxygen/classclang_1_1ForStmt.html) type of AST node. You **DO NOT** have to do anything with this function; we are providing it to give you a head start with ASTVisitor. 

Here are some other notes about the tasks:
* For Task 1, we implemented [`VisitFunctionDecl`](src/ClangHw2.cpp#L116), which calls the helper function [`isRecursiveFunction`](src/ClangHw2.cpp#L64) and decides whether that function is direct recursive or not. All you have to do is implement this `isRecursiveFunction`.
* For Task 2, [`VisitFunctionDecl`](src/ClangHw2.cpp#L116) calls the function [`analyzeRedifinition`](src/ClangHw2.cpp#L69). Inside `analyzeRedifinition`, if you encounter a variable that is redefined, you should call the helper function [`printVarReDeclarationInformation`](src/ClangHw2.cpp#L28) with the variable name, the line number where it was initially defined, and the line number where it is being redefined.
* We have provided some other helper functions. You can add print statements as you are developing your code. However, please make sure you use the helper functions that we provided to print your output. Please **DO NOT** change any print formatting or function prototypes of any helper function, and please make sure to remove any print statements you have added prior to submission.

When you have fully implemented both tasks and have run the tool with [`gcd.c`](gcd.c), you will see the following output:
```
gcd_recursive - recursive
Redefining variable : "k4" at line 7 which is initially defined at line 2
```


## Extra Credit (50 Points)

We can perform a lot of interesting operations using an AST. In task 1, you learned how to identify recursive functions. In this extra credit section of the assignment, you will gain some experience on automatic code formatting by analyzing an AST.

Suppose you have a function call in your code such as the following:
```c
foo( 1,        2      , 3    ,    5)
```
Then, you will need to format it as:
```c
foo (1, 2, 3, 5)
```
More specifically, you will need to format the call expression as `<callee><space>(<arg1>,<space><arg2>,<space>...,<space><argn>)` for all `n` arguments. 
The `<space>` represents a single space character `' '`. Note that the callee and/or arguments of a function may also be function calls themselves, and you will also need to figure out how to reformat those nested function calls whenever they arise. 


### Code and Logistics 
From the `VisitFunctionDecl` function, we call [`analyzeCallExpressionReformat`](src/ClangHw2.cpp#L69) to perform a [depth-first search (DFS)](https://en.wikipedia.org/wiki/Depth-first_search) on the AST. While performing DFS, if we encounter any [`CallExpr`](https://clang.llvm.org/doxygen/classclang_1_1CallExpr.html) node, we call the [`formatFunctionCall`](src/ClangHw2.cpp#L79) function. You have to implement this function so that the function call expression is reformatted.

### Examples

The following code is very difficult to read:
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

However, when you run your reformatter tool, the function call to `foo` at line 2 should be formatted as:
```c
foo (3, foo (too (), foo (5, too ())))
```
Now, it is definitely much easier to understand the function call!

Here is another example:
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
19.           1+0   )(  5 , 6   );
20.     return 0;
21. }
```
Here, there is a function call at line 18. However, the callee is not a direct function; instead, it is another `CallExpr` node. You have to reformat that function call as well.

The reformatted code that you should generate is:
```c
getFunc (1+0) (5, 6)
```

You may consider the following constraints:
1. You have to reformat only the `CallExpr` node. If you encounter any other node (for instance, `1+0 ` in line 19 of the second example is a [`BinaryOperator` node](https://clang.llvm.org/doxygen/classclang_1_1BinaryOperator.html)), you should copy the code as is from the input source. We have provided a helper function [`getSource`](src/ClangHw2.cpp#L61) to copy the input code corresponding to a node.
2. The callee or arguments of a function call will be either a pure function call or a pure non-function call, *i.e.*, there will not be a mixture of functions and non-functions involved in binary expressions, conditional expressions, etc. As an example, we will **NOT** test the following case:
```
foo(bar(3) + 1, 9 + bar(6))
```
3. We will only test C code inputs. You **DO NOT** need to handle function calls in C++ or C++-specific functionality (including operator overloading or user-defined literals, etc.).
4. This problem may look like a simple character "parsing and formatting" problem, but you **have to** use the template code we provided. You cannot modify any of our function prototypes.


## Submission

Please follow these steps for submission:

1. Copy the completed `ClangHw2.cpp` file from your `$LLVM_HOME/clang/tools/clang-hw2/` directory to the project's `src` folder. 
2. Complete the write-up in [outputs/AST.txt](outputs/AST.txt). 
3. Copy the executable `clang-hw2` from your `$LLVM_HOME/build/bin` directory to the project's `outputs` directory.
4. Commit your code.
5. Push the code to the master branch.

## Piazza
If you have any questions about this programming assignment, please post them in the Piazza forum for the course, and an instructor will reply to them as soon as possible. Any updates to the assignment itself will be available in Piazza.


## Disclaimer
This assignment belongs to Columbia University. It may be freely used for educational purposes.
