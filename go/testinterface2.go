package main

import (
	"fmt"
)

//1
type I interface {    
    Get() int
    Set(int)
    print()
}

//2
type S struct {
    Age int
}

func(s S) Get()int {
    s.Age = 100
    return s.Age
}

func(s *S) Set(age int) {
    s.Age = age
}

func (s S) print() {
	fmt.Println(s.Age)
}

//3
func f(i I){
    i.Set(10)
    i.Get()
    i.print()
}

func main() {
    s := S{}
    f(&s)
    var t I
    t = &s
	f(t)  //4
}
