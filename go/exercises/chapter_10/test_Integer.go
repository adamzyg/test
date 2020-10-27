package main

import "fmt"

type Integer int

func (self Integer) get() int {
    return int(self)
}

func main() {
    var a Integer = 10
    fmt.Println(a.get())
}
