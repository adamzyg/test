package main

import "fmt"

func main() {
	defer fmt.Println("in main")
	defer func() {
		fmt.Println("In Goroutine")
		panic("panic again")
	}()

	panic("panic once")
}
