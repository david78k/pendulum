import math._

object HelloWorld extends App {
   	println("Hello, World!")
	
	var num = 10
	println("mathFunction(" + num + ") " + mathFunction(num))
	println("intRoot23(" + num + ") " + intRoot23(num))
	println("log base E = " + E)

	def mathFunction(num: Int): Int = {
		var numSquare: Int = num*num
		return (math.cbrt(numSquare) + math.log(numSquare)).asInstanceOf[Int]
	}

	def intRoot23(num: Int) = {
		val numSquare = num*num
		println(numSquare)
		println(cbrt(numSquare))
		println(log(numSquare))
		(cbrt(numSquare) + log(numSquare)).toInt
	}
}
