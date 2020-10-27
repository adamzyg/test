package main

import (
	"fmt"
    //"reflect"
)

func main() {
    str := "abcd"
    estr := fmt.Sprintf("%x", (uint32(str[0]) >> 4) & 0x0f)
    fmt.Println(estr)
    var astr []byte
    astr = str[1:3]
    fmt.Println(astr)
}

