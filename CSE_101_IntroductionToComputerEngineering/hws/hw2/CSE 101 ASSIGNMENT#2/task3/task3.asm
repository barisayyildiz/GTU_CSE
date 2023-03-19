load R0,16
load R1,Numbers;adresses the numbers
load R2,1
load R3,0;holds the sum
load R4,0;counter
load R5,255

sum:
	load R6,[R1]
	addi R1,R1,R2
	addi R3,R3,R6
	addi R4,R4,R2
jmpEQ R4=R0,pass
jmp sum

pass:
	move R7,R0
	xor R7,R7,R5
	addi R7,R7,R2
	load R8,0
	move R9,R3

control:
	jmpLE R9<=R0,check
	jmp divide

divide:
	load R0,16
	addi R9,R9,R7
	addi R8,R8,R2
	jmp control



check:
	jmpEQ R9=R0,divide
	jmp check2


;if our number in R9 in greater than 127, which is the biggest number two's complement notation
;it is going to be a negative number
;that's why we have to take two's complement of our number and compare with 0
;if it is greater than zero, we continue dividing
check2:
	move RA,R9
	xor RA,RA,R5
	addi RA,RA,R2
	load R0,0
	
	jmpEQ RA=R0,end
	

	move R0,RA
	load RA,0
	jmpLE RA<=R0,divide
	jmp end


end:
	move R0,R8
	halt


Numbers:db 18,42,5,7,1,6,23,7,9,10,11,32,5,3,9,33
	  db 0



