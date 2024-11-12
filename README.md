# Dia: Brilliance of Diamond

Dia is a strong typed functional programming language, focused on simplicity and portability.

```d
# The 'main' keyword is mandatory!
main =
  "Hello World!"
  .puts
```

The `diac` is a Dia programming language trans-compiler. It converts the Dia language source code into C++17 programming language code.

```cpp
#include<iostream>
#include<string>
#include<vector>

int main(int argc, char** argv) {
  std::cout<<"Hello World!"<<std::endl;
}
```

## Lemme brag about the dia!

The Dia programming language is...

* Easy -- As Simple as Lisp, Chain functions and Let it go!
* Solid -- Start error-proof programming with Dia!
* Strong -- Less code, Go Fast, Get advantages of functional programming!

The dia programming language compiler is...

* Fast -- Entire compiler is written in C, Output is in C++
* Compact -- Designed to be built in static binary

Use Dia programming language for...

* Code Golf
* Competitive Programming
* Embedded systems

## Build

If you would like to use CMake

```sh
mkdir build
cd build
cmake ..
make
```

If you would like to use ninja with CMake

```sh
mkdir build
cd build
cmake -G Ninja ..
ninja
```

If you prefer `meson` over CMake

```sh
meson setup builddir
cd builddir
ninja
```

### Termux (Android)

The Dia compiler is also intended to work on Termux.

Run

```
pkg install git make bison flex clang
```

to install necessary packages. Then type `make` to build the Dia compiler.

## QnA

### Why did you started this (kind of) project?

Just for fun!

That is the biggest reason I started this project. But there are some minor reasons:

* I wanted to make an easy-to-use functional programming language. Not too complicated like Haskell, and not too minimized like Lisp.
* The project was started to make several useful functions, `split` for example, to be implemented in C++
  * I came up with initial idea while solving several competitive programming challenges
  * The `lodash` project in JavaScript inspired me. Lodash is a JS library provides some useful functions while programming in JavaScript.
* I wanted to make my programming language tool to be used inside embedded system.
  * I wanted to make useful tools while anayli
* I wanted to make a programming language, whatever in any form. That is of the same mind that a kid want to make his own toy of his own.
  * As mentioned, I wanted my languages to be used in code golfing, and embedded system.

### Which programming language/concept influenced you the most?

* RxJS, the concept
* Ruby, the programming language design
