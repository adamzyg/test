package main

import (
	"fmt"
	"reflect"
)

const tagName = "Testing"

type Info struct {
	Name string `Testing:"-"`
	Age  int    `Testing:"age,min=17,max=60"`
	Sex  string `Testing:"sex,required"`
}

func main() {
	info := Info{
		Name: "benben",
		Age:  23,
		Sex:  "male",
	}

	//通过反射，我们获取变量的动态类型
	t := reflect.TypeOf(info)
	fmt.Println("Type:", t.Name())
	fmt.Println("Kind:", t.Kind())

	for i := 0; i < t.NumField(); i++ {
		field := t.Field(i) //获取结构体的每一个字段
		tag := field.Tag.Get(tagName)
		fmt.Printf("%d. %v (%v), tag: '%v'\n", i+1, field.Name, field.Type.Name(), tag)
	}
}
