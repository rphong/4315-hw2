To run:
<br/>g++ -std=c++11 *.cpp -o mypython 
<br/>./mypython <file.py>

Overview of the Interpreter:

![image](https://github.com/rphong/4315-hw2/assets/91210910/2c731960-ddfe-4cf0-888e-bd329fe1b8a8)

Scanner:
The scanner first reads in the source file and converts it into a series of Tokens, which keep track of the type, lexeme(literal), and line number. The full list of supported token types can be found in Token.hpp.
From there, the scanner sends the list of Tokens to Parser to convert.

Parser:
The parser takes in a series of statements based on the tokens. This is where syntax errors are caught if they happen to be present. The parser groups the tokens using a syntax tree, which allows 
for nested expressions to be parsed correctly. From here, the parser sends the list of Statements to the interpreter to interpret.

Interpreter:
This is where the code finally gets evaluated. This interpreter works by making use of the visitor pattern, which allows for the program to determine at runtime how to handle the expression/statement depending on its type.
While not completely necessary, the visitor pattern allows for a more modular design by decoupling the method that takes in the statement from the method that handles the statement depending on its type. 
Another note is that since we're evaluating a syntax tree, we need some of the accepting methods to have a return type, namely expressions. The implementation for adding a return type can be found in VisitorReturnVal.hpp
along with the guide that was used. 
