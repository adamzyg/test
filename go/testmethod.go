package main

import (
    "fmt"
)

type Myint int

type MyStruct struct {
    a int
    b string
}

func main() {
    var a Myint = 1
    a.Increase()
    fmt.Println(a)

    myst := MyStruct{a:10, b:"hello"}
    myst.hello(20)
    fmt.Println(myst)
}

func (ms *MyStruct) hello(a int) {
    ms.a = a
}


func (a Myint) Increase() {
    a = 100
}
