package main

type Student struct {
	name string
}

func main() {
	n := map[string]*Student{"people": &Student{"zhoujielun"}}
	n["people"].name = "wuyanzu"

	m := map[string]Student{"people": Student{"zhoujielun"}}
	m["people"].name = "wuyanzu"
}
