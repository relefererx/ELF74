 
 
THUMB

  PUBLIC EightBitHistogram
  SECTION .text: CODE
  
  
Inicializa
  STR R11, [R3], #2
  ADD R12, #1
  CMP R12, #256
  
  BNE Inicializa
  
  BX LR
    
Fill

  MOV R3, R9
 
  LDRB R11, [R2], #1
  ADD R11, R11
  
  LDR R10, [R3, R11]
  ADD R10, #1
  
  ADD R3, R11
  STR R10, [R3]
  
  ADD R12, #1
  CMP R12, R6
  
  BNE Fill
  BX LR
 
 
EightBitHistogram
    MUL R6, R0, R1 
    MOV R9, R3
    
    CMP R6, #65536
    ITT GT
      MOVGT R0, #0
      BXGT LR
      
    MOV R11, #0
    MOV R12, #0
    
    PUSH {LR}
    BL Inicializa
    POP {LR}
    
    MOV R12, #0
    PUSH {LR}
    BL Fill
    POP {LR}
    
    MOV R0, R6
      
    BX LR
	
  END