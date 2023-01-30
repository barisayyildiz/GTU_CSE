load R0,97
load R1,29
load R2,177
load R3,03
load R4,255
load R5,1

xor R0,R0,R4
xor R1,R1,R4
xor R2,R2,R4
xor R3,R3,R4

move R4,R0
load R0,255

jmpEQ R3=R0,condition1
jmp condition2

condition1:
	jmpEQ R2=R0,condition3
	jmp condition4

condition3:
	jmpEQ R1=R0,condition5
	jmp condition6

condition5:
	addi R4,R4,R5
	addi R1,R1,R5
	addi R2,R2,R5
	addi R3,R3,R5
	jmp end

condition6:
	addi R1,R1,R5
	addi R2,R2,R5
	addi R3,R3,R5
	jmp end

condition4:
	addi R2,R2,R5
	addi R3,R3,R5
	jmp end

condition2:
	addi R3,R3,R5
	jmp end

end:
	move R0,R4
	halt
