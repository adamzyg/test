package main

import (
    "fmt"
)

func main() {
    a := [2]int{1:10}
    p := new([3]int)
    q := &[3]int{2:10}

    p[2] = 10

    fmt.Println(a)
    fmt.Println(p)
    fmt.Println(q)

    c := make([]int, 3, 6)
    c = append(c, 1,2,3,4,5,6)
    fmt.Printf("%v %v\n", c, cap(c))

    d := a[1:]
    d = append(d, 6,7,8,9)
    fmt.Printf("%v %v\n", a, d)

    b := [6]int{1,2,3,4,5,6}
    e := b[1:4]
    f := b[2:5]
    fmt.Printf("%v %v %v\n", b, e, f)
    copy(e, f)
    fmt.Printf("%v %v %v\n", b, e, f)
}
