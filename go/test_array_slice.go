package main

import "fmt"

func main() {
	// 长度为3
	a := []int{1, 2, 3}
	// 单个元素
	{
		// 起始地址从2开始,正常操作,b的值为3
		b := a[2]
		fmt.Println("b:", b)
		// 起始地址从3开始,会出错,产生下标超出范围的崩溃,暂时注释掉
		//c := a[3]
		//fmt.Println("c:", c)
	}
	// 切片下标访问
	{
		// 起始地址从2开始,正常操作,b为1个元素的切片
		b := a[2:]
		fmt.Println("b:", b)
		// 起始地址从3开始,不会出错,c为空的切片
		c := a[3:]
		fmt.Println("c:", c)
		// 起始地址从4开始,会出错,产生下标超出范围的崩溃
		d := a[4:]
		fmt.Println("d:", d)
	}
}
