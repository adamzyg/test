package main

import (
	"fmt"
	"runtime"
)

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	channel := make(chan int, 10)
	for i := 1; i <= 10; i++ {
		go Go(channel, i)
	}

	// var read int
	for i := 1; i <= 10; i++ {
		<-channel
	}
}

func Go(channel chan int, index int) {
	sum := 0
	for i := 1; i <= 1000000; i++ {
		sum += i
	}

	fmt.Println(index, sum)

	channel <- index
}
