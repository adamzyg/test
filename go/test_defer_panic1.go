package main

import "fmt"
import "time"

func main() {
	b()
	defer fmt.Println("in main")
	go func() {
		defer fmt.Println("in goroutine")
		//panic("")
	}()

	time.Sleep(1 * time.Second)
}

func b() {
	for i := 0; i < 4; i++ {
		defer fmt.Println(i)
	}
}
