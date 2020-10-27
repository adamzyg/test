package main

import (
	"fmt"
)

func main() {
	channel := make(chan bool, 1)
	go func() {
		fmt.Println("Go Go Go!!!")
		<-channel
		close(channel)
	}()

	// note channel with cache is non-block
	// so main thread go end, don't wait the anymouse func read the channel
	channel <- true

}
