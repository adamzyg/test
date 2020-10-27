package main

import (
	"fmt"
	"time"
)

func main() {
	channel := make(chan int)
	go foo(channel)
	go bar(channel)
	time.Sleep(1 * time.Second)
}

func foo(channel chan int) {
	// first block foo for read channel
	var read int
	read = <-channel
	fmt.Println("foo read from channel", read)

	fmt.Println("foo write to chanel 2")
	channel <- 2
}

func bar(channel chan int) {
	// first write to channel
	fmt.Println("bar write to chanel 1")
	channel <- 1
	// note: if you put the fmt.Println after write to channel,
	// maybe it will be block, so the thread handover to foo
	// fmt.Println("bar write to chanel 1")

	var read int
	read = <-channel
	fmt.Println("bar read from channel", read)
	close(channel)
}
