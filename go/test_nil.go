package main

import (
	"fmt"
    //"reflect"
)

func main() {
    var a *int
    if a == nil {
        fmt.Println("nil")
    }

    var s []int
    if s == nil {
        fmt.Println("nil")
    }

    var m map[string]int
    if m == nil {
        fmt.Println("nil")
    }

    var c chan int
    if c == nil {
        fmt.Println("nil")
    }
    c = make(chan int)
    go goroutine(c)
    c <- 1
    if c == nil {
        fmt.Println("c also is nil")
    }
    close(c)
    if <-c == 0 {
        fmt.Println("c is zero after close channel")
    }
    if c != nil {
        fmt.Println("after close c is ", c)
    }
    // 在关闭channel之后最好将其设置为nil，因为关闭的channel总能读到0；将其设置为nil后，这个channel将会永远阻塞
    c = nil
    //c <- 1

    err := do()
    fmt.Println(err == nil)
}

func goroutine(c chan int) {
    i := <-c
    fmt.Println("read i ", i, "from channel")
}

func do() error {   // error(*doError, nil)
    return nil  // nil of type *doError
}
