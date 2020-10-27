package main

import (
	"fmt"
	"runtime"
	"sync"
)

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	wg := sync.WaitGroup{}
	wg.Add(10)

	for i := 1; i <= 10; i++ {
		go Go(&wg, i)
	}
	wg.Wait()
}

func Go(wg *sync.WaitGroup, index int) {
	sum := 0
	for i := 1; i <= 1000000; i++ {
		sum += i
	}
	fmt.Println(index, sum)
	wg.Done()
}
