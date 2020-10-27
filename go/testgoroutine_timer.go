package main

import "time"
import "fmt"

func timer(d int) <-chan int {
	c := make(chan int)
	go func() {
		time.Sleep(1 * time.Second)
		fmt.Println("timer write to channel:", c, d)
		c <- d
		close(c)
	}()
	return c
}

func main() {
	var read int
	for i := 0; i < 24; i++ {
		c := timer(i)
		read = <-c
		fmt.Println("main read from channel: ", c, read)
	}
}
