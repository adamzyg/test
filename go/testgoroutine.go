package main

import (
	"fmt"
	// "time"
)

func main() {
	channel := make(chan bool)
	go func() {
		fmt.Println("Go Go Go")
		channel <- true
	}()
	// time.Sleep(1 * time.Second)
	var haha bool
	haha = <- channel
	fmt.Println(haha)
	close(channel)
}

func Go() {
	fmt.Println("Go Go Go!!!")
}