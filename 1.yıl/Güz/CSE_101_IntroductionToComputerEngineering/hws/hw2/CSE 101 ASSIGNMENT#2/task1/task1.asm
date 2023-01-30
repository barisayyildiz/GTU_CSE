load R0,64
load R1,42
load R2,1
load R3,255

xor R0,R0,R3
xor R1,R1,R3

move R3,R0
load R0,255

jmpEQ R1=R0,true
jmp false

true:
	addi R3,R3,R2
	addi R1,R1,R2
	jmp end
false:
	addi R1,R1,R2

end:
	move R0,R3

	halt