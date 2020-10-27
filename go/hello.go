package main

import "fmt"
import "strconv"

var (
	name = "adamzyg"
	sex  = "male"
)

type Newtype int

func main() {
	fmt.Println("Hello, World!" + name + sex)
	var a int = 65
	b := string(a)
	fmt.Println(b)
	c := strconv.Itoa(a)
	fmt.Println(c)

	{
		fmt.Println(100)
	}
}
