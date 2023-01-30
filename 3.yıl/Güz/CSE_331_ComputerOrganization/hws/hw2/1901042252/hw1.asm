.data
promptPrefix: .asciiz "candidate sequence : ["
promptPostfix: .asciiz "]"
promptSize: .asciiz " ,size = "
newLine: .asciiz "\n"
comma: .asciiz ","
dash: .asciiz "-------------------------\n"

arr: .word 3, 10, 7, 9, 4, 11												# array
arraySize: .word 6																	# array size
temp: .space 40																			# temporary array
tempSize: .word 0																		# temporary array size
longest: .space 40																	# longest array
longestSize: .word 0																# longest array size

.text:

	la $a0, arr																				# load array adress
	addi $a1, $zero, 0																# $a1 = 0
	la $a2, temp																			# temporary array adress
	lw $a3, tempSize																	# $a3 = temporary size
	jal algo																					# call algo function


	# print dash
	li $v0, 4
	la $a0, dash
	syscall

	# print longest array
	la $a1, longest
	lw $a2, longestSize
	jal printArr

	# terminate
	li $v0, 10
	syscall


	algo:
		# $a0 arr
		# $a1 arrIndex
		# $a2 temp
		# $a3 tempSize
		addi $sp, $sp, -24
		sw $ra, 0($sp)
		sw $a0, 4($sp)
		sw $a1, 8($sp)
		sw $a2, 12($sp)
		sw $a3, 16($sp)

		move $s0, $a0																# arr
		move $s1, $a1																# arrIndex
		move $s2, $a2																# temp
		move $s3, $a3																# tempSize

		jal branchOne
		bne $v0, $zero, algoReturn

		jal branchTwo

		# call algo
		lw $a0, 4($sp) 															# get array adress
		addi $a0, $a0, 4														# increment array pointer
		lw $a1, 8($sp)															# get array index
		addi $a1, $a1, 1														# increment array index
		lw $a2, 12($sp)															# get temp array adress
		lw $a3, 16($sp)															# get tempSize
		jal algo

		# return
		lw $ra, 0($sp)
		lw $a0, 4($sp)
		lw $a1, 8($sp)
		lw $a2, 12($sp)
		lw $a3, 16($sp)
		addi $sp, $sp, 24
		jr $ra

	algoReturn:
		lw $ra, 0($sp)
		addi $sp, $sp, 24
		jr $ra

	# tempSize == 0 || *arr > temp[tempSize-1], C code line : 32
	branchTwo:
		addi $sp, $sp, -24
		sw $ra, 0($sp)
		sw $s0, 4($sp)
		sw $s1, 8($sp)
		sw $s2, 12($sp)
		sw $s3, 16($sp)

		jal tempSizeZero																	# tempSize == 0
		move $v1, $v0
		jal currentIndexItemGreaterThanTheLastItem				# *arr > temp[tempSize-1]
		or $v0, $v0, $v1																	# tempSize == 0 || *arr > temp[tempSize-1]

		beq $v0, $zero, branchTwoFalse										# return false
		# if true append and call algo
		jal appendTemp																		# temp[tempSize] = *arr

		# call algo
		lw $a0, 4($sp) 																		# get array adress
		addi $a0, $a0, 4																	# increment array pointer
		lw $a1, 8($sp)																		# get array index
		addi $a1, $a1, 1																	# increment array index
		lw $a2, 12($sp)																		# get temp array adress
		addi $a2, $a2, 4																	# increment temp array pointer
		lw $a3, 16($sp)																		# get tempSize
		addi $a3, $a3, 1																	# increment tempSize
		jal algo

		lw $ra, 0($sp)
		lw $s0, 4($sp)
		lw $s1, 8($sp)
		lw $s2, 12($sp)
		lw $s3, 16($sp)
		addi $sp, $sp, 24
		jr $ra

	# if (tempSize == 0 || *arr > temp[tempSize-1]) == false
	branchTwoFalse:
		# go back to algo function
		lw $ra, 0($sp)
		lw $s0, 4($sp)
		lw $s1, 8($sp)
		lw $s2, 12($sp)
		lw $s3, 16($sp)
		addi $sp, $sp, 24
		jr $ra		

	# C code line : 25
	branchOne:
		addi $sp, $sp, -12
		sw $ra, 0($sp)
		sw $s3, 4($sp)

		jal indexGreaterThanArrSize											# index >= arrSize
		beq $v0, $zero, branchOneElseFirst							# if !(index >= arrSize)

		# print temp
		la $a1, temp
		lw $a2, 4($sp)
		jal printArr																		# print temporary array

		jal tempSizeGreaterThanLargest									# if (tempSize > longestSize)
		beq $v0, $zero, branchOneElseSecond							# if !(tempSize > longestSize)

		jal changeLongest																# changeLongest function

		lw $ra, 0($sp)
		lw $s3, 4($sp)
		addi $sp, $sp, 12
		j true

	branchOneElseFirst:
		lw $ra, 0($sp)
		lw $s3, 4($sp)
		addi $sp, $sp, 12
		j false
	branchOneElseSecond:
		lw $ra, 0($sp)
		lw $s3, 4($sp)
		addi $sp, $sp, 12
		j true


	
	appendTemp:
		lw $t0, ($s0)																# get *arr
		sw $t0, ($s2)																# append it to temp
		jr $ra
	currentIndexItemGreaterThanTheLastItem:
		lw $t0, 0($s0)															# *arr
		lw $t1, -4($s2)															# temp[tempSize-1]
		bgt $t0, $t1, true													# *arr > temp[tempSize-1]
		j false
	tempSizeZero:
		beq $s3, $zero, true												# tempSize == 0
		j false
	tempSizeGreaterThanLargest:
		lw $t0, longestSize													# longestSize
		bgt $s3, $t0, true													# if (tempSize > longestSize) => true
		j false
	indexGreaterThanArrSize:
		lw $t0, arraySize														# arraySize
		bge $s1, $t0, true													# index >= arraySize
		j false
	true:
		li $v0, 0x11111111
		jr $ra
	false:
		li $v0, 0x00000000
		jr $ra
	changeLongest:
		addi $sp, $sp, -8
		sw $ra, 0($sp)
		
		addi $t0, $zero, 0													# counter = 0
		move $t1, $s3																# tempSize
		lw $t2, longestSize													# longestSize
		la $t3, temp																# temp adress
		la $t4, longest															# longest adress
		jal changeLongestLoop

		la $t2, longestSize													# longestSize adress
		sw $s3, ($t2)																# longestSize = tempSize

		lw $ra, 0($sp)
		addi $sp, $sp, 8
		jr $ra	
	changeLongestLoop:
		beq $t0, $t1, goBack												# termination condition
		lw $t5, 0($t3)															# get current temp value
		sw $t5, ($t4)																# save current temp value to longest
		addi $t0, $t0, 1														# increment counter by 1
		addi $t3, $t3, 4														# increment temp adress pointer by 4
		addi $t4, $t4, 4														# increment longest adress pointer by 4
		j changeLongestLoop													# repeat the loop
	goBack:
		jr $ra
	printArr:
		# $a1, array adress
		# $a2, arraySize
		addi $sp, $sp, -16
		sw $ra, 0($sp)
		sw $s0, 4($sp)
		sw $s1, 8($sp)

		la $s0, ($a1)																# load array adress
		move $s1, $a2																# load array size

		li $v0, 4
		la $a0, promptPrefix												# print prefix
		syscall

		addi $t0, $zero, 0
		jal printArrLoop														# print array

		li $v0, 4
		la $a0, promptPostfix												# print postfix
		syscall

		li $v0, 4
		la $a0, promptSize													# print ' size='
		syscall

		la $v0, 1
		move $a0, $s1																# print the actual size
		syscall

		li $v0, 4
		la $a0, newLine
		syscall

		lw $ra, 0($sp)															# load return adress back
		lw $s0, 4($sp)
		lw $s1, 8($sp)
		addi $sp, $sp, 16

		jr $ra																			# jump back

	printArrLoop:
		bge $t0, $s1, printArrLoopEnd								# if(index >= size), go to return adress

		li $v0, 1
		lw $a0, ($s0)																# load 4 byte integer
		syscall																			# print integer

		li $v0, 4
		la $a0, comma
		syscall																			# print comma

		addi $s0, $s0, 4														# increment array pointer by 4
		addi $t0, $t0, 1														# increment counter by 1

		j printArrLoop

	printArrLoopEnd:
		jr $ra
		
