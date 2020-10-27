package main

import "fmt"

type D = int

func main() {
	var v interface{}
	var d D = 100
	v = d

	switch i := v.(type) {
	case int:
		fmt.Println("it is an int:", i)
		// case D:
		// 	fmt.Println("it is D type:", i)
	}
}
