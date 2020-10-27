package main

import "fmt"
import "time"

func main() {
	defer fmt.Println("in main")
	go func() {
		defer fmt.Println("in goroutine")
		panic("")
	}()

	time.Sleep(1 * time.Second)
}
