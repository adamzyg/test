package main

import (
	"fmt"
	// "time"
)

func main() {
	channel := make(chan bool)
	go func() {
		channel <- true
		// time.Sleep(1 * time.Second)
		fmt.Println("Go Go Go!!!")
		channel <- true
		// time.Sleep(1 * time.Second)
		close(channel) // you can try commit this line
	}()

	for v := range channel {
		fmt.Println("receive channel", v)
		// close(channel)
	}
}
