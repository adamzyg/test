package main

import (
	"fmt"
	"reflect"
)

type User struct {
	Name string
	Id   int
	Age  int
}

func (user *User) Hello(name string) (a string, b int) {
	fmt.Println("Hello ", name, "my name is ", user.Name)
	user.Name = name
	a = user.Name
	b = user.Age
	return a, b
}

func Info(o interface{}) {
	t := reflect.TypeOf(o)
	fmt.Println("Type: ", t.Name())

	if k := t.Kind(); k != reflect.Struct {
		fmt.Println("the parameter o is not the struct")
		return
	}

	v := reflect.ValueOf(o)
	fmt.Println("Vaule:", v)

	for i := 0; i < t.NumField(); i++ {
		f := t.Field(i)
		val := v.Field(i).Interface()
		fmt.Printf("%6s: %v = %v\n", f.Name, f.Type, val)
	}

	fmt.Println("t method num: ", t.NumMethod())

	for i := 0; i < t.NumMethod(); i++ {
		m := t.Method(i)
		fmt.Printf("%6s: %v\n", m.Name, m.Type)
	}
}

func main() {
	a := User{"haha", 123, 19}
	a.Hello("zyg")
	Info(a)
	//Info(&a)  // note if you transfter the pointer, the num method of a is 0
}
