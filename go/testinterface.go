package main

import (
	"fmt"
)

type Connector interface {
	Connect()
}

type USB interface {
	Name() string
	Connector
}

type PhoneConnector struct {
	name string
}

func (pc *PhoneConnector) Name() string { // you can try modify the receiver pc *Phonconnector
	fmt.Println("In func Name")
	return pc.name
}

func (pc *PhoneConnector) Connect() {
	fmt.Println("Connect ", pc.Name())
}

func Disconnect(usb USB) { // maybe parameter: usb interface{}
	// if pc,ok := usb.(PhoneConnector); ok {
	// 	fmt.Println("Disconnected: ", pc.name)
	// 	//return
	// }

	switch v:=usb.(type) {
	case *PhoneConnector:
		fmt.Println("Disconnected: ", v.name)
	default:
		fmt.Println("Unknwon device Disconnected.")
	}

	// fmt.Println("Unknwon device Disconnected.")
}

func main() {
	a := PhoneConnector{"my phone"}
	a.Connect()
	Disconnect(&a)

	var b USB
	b = &a
	// 
	a.name = "new phone"
	b.Connect()
	Disconnect(b)

	a.Connect()
	Disconnect(&a)

	var c interface{}
	fmt.Println(c == nil)

	var p *int
	c = p
	fmt.Println(c == nil)
}