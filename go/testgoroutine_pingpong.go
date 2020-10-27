package main

import "time"
import "fmt"

func main() {
	var Ball int
	table := make(chan int)
	go player(table, 1)
	go player(table, 2)
	go player(table, 3)

	fmt.Println("main write to channel", table, Ball)
	table <- Ball
	time.Sleep(3 * time.Second)
	endBall := <-table
	fmt.Println("main receive from channel", table, endBall)
}

func player(table chan int, index int) {
	for {
		ball := <-table
		fmt.Println("player", index, "receive ball", ball)
		ball++
		time.Sleep(100 * time.Millisecond)
		fmt.Println("player", index, "send ball", ball)
		table <- ball
	}
}

// 为什么这个会按照第一次执行的顺序挨着执行，而不出现乱序
// main write to channel 0xc420076060 0
// player 1 receive ball 0
// player 1 send ball 1
// player 2 receive ball 1
// player 2 send ball 2
// player 3 receive ball 2
// player 3 send ball 3
// player 1 receive ball 3
// player 1 send ball 4
// player 2 receive ball 4
// player 2 send ball 5
// player 3 receive ball 5
// player 3 send ball 6
// player 1 receive ball 6
// player 1 send ball 7
// player 2 receive ball 7
// player 2 send ball 8
// player 3 receive ball 8
// player 3 send ball 9
// player 1 receive ball 9
// player 1 send ball 10
// player 2 receive ball 10
// player 2 send ball 11
// player 3 receive ball 11
// player 3 send ball 12
// player 1 receive ball 12
// player 1 send ball 13
// player 2 receive ball 13
// player 2 send ball 14
// player 3 receive ball 14
// player 3 send ball 15
// player 1 receive ball 15
// player 1 send ball 16
// player 2 receive ball 16
// player 2 send ball 17
// player 3 receive ball 17
// player 3 send ball 18
// player 1 receive ball 18
// player 1 send ball 19
// player 2 receive ball 19
// player 2 send ball 20
// player 3 receive ball 20
// player 3 send ball 21
// player 1 receive ball 21
// player 1 send ball 22
// main receive from channel 0xc420076060 22
