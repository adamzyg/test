package main

import (
	"fmt"
	"reflect"
	"runtime"
)

func callback() {
    fmt.Println("callback")
}

func main() {
    funcName := runtime.FuncForPC(reflect.ValueOf(callback).Pointer()).Name()
    //funcName := reflect.ValueOf(callback).Pointer()
    fmt.Println(funcName)
}
