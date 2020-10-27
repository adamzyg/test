package main

import (
    "fmt"
)

type MyInterface interface {
    foo()
}

type MyStruct struct {
   ii int64
}

func (m MyStruct) foo() {
   fmt.Println("foo: before plus", m.ii);
   m.ii ++
   fmt.Println("foo: after plus", m.ii)
}

func (m MyStruct) foo1() {
  fmt.Println("foo1")
}

func Hello(p MyInterface) {
   p.foo();
}

func main() {
   m := MyStruct { 10 }
   Hello(m)
   Hello(&m)

   var n *MyStruct
   n = &m
   n.foo()
   n.foo1()
   Hello(n)

   fmt.Println(m.ii);
}
