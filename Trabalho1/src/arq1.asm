;programa que faz o fatorial de um número 
;o usuario deve digitar o número 
SECTION TEXT  
	 INPUT N   
	 LOAD N   
FAT: SUB ONE
	 JMPZ FIM
	 STORE AUX
	 MULT N
	 STORE N
	 LOAD AUX
	 JMP FAT
FIM: OUTPUT N
	 STOP
SECTION DATA
AUX: SPACE
N: SPACE
ONE: CONST 1
