# Brainfuck Interpreter

## Description

Interpreter for the esoteric language [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) made in C.

## Requirements
gcc (My version is 11.4.0)

```Shell
sudo apt update && sudo apt install build-essential
```

## Usage

Clone the repository and go inside the folder

```Shell
git clone https://github.com/SrVariable/BFInterpreter.git && cd BFInterpreter
```

Compile the program:

```Shell
gcc -Wall -Wextra -Werror -pedantic -std=c11 main.c -o bfinterpreter
```

Run the program giving the brainfuck file.

```Shell
./bfinterpreter helloworld.bf
```

## Examples

### Hello World

> [!TIP]
>
> Having the ASCII table will help you to understand the values.

```Brainfuck
++++++++++ Increase the value of the pointer by 10. (ptr plus 0) = 10
[ Begin of the scope of the loop
It will end when the value of the pointer (ptr plus 0) is 0

    > Increase the pointer by 1 (ptr plus 1)
    +++++++ Increate the value of the pointer by 7
    > Increase the pointer by 1  (ptr plus 2)
    ++++++++++ Increase the value of the pointer by 10
    > Increase the pointer by 1 (ptr plus 3)
    +++ Increase the value of the pointer by 3
    > Increase the pointer by 1 (ptr plus 4)
    + Increase the value of the pointer by 1
    <<<< Decrease the pointer by 3 (ptr plus 0)
    - Decrease the value of the pointer by 1

] End of the scope of the loop
Now the values are:
(ptr plus 0) = 0
(ptr plus 1) = 70
(ptr plus 2) = 100
(ptr plus 3) = 30
(ptr plus 4) = 10

> Increase the pointer by 1 (ptr plus 1)
++ Increase the value of the pointer by 2 (ptr plus 1)
. Print the value of the pointer which is H
> Increase the pointer by 1 (ptr plus 2)
+ Increase the value of the pointer by 1 (ptr plus 2) = e
. Print the value of the pointer which is e
+++++++ Increase the value of the pointer by 7 (ptr plus 2) = l
.. Print the value of the pointer twice which is l
+++ Increase the value of the pointer by 3 (ptr plus 2) = o
. Print the value of the pointer which is o
> Increase the pointer by 1 (ptr plus 3)
++ Increase the value of the pointer by 2
. Print the value of the pointer which is a space
<< Decrease the pointer by 2 (ptr plus 1)
+++++++++++++++ Increase the value of the pointer by 15
. Print the value of the pointer which is W
> Increase the pointer by 1 (ptr plus 2)
. Print the value of the pointer which is o
+++ Increase the value of the pointer by 3
. Print the value of the pointer which is r
------ Decrease the value of the pointer by 6
. Print the value of the pointer which is l
-------- Decrease the value of the pointer by 8
. Print the value of the pointer which is d
>> Increase the value of the pointer by 2
. Print the value of the pointer which is \n
```

Output:

```
Hello World\n
```
