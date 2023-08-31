BITS 16
ORG 0x7C00

Entry:
    CLI

    MOV AX, 0x0000
    MOV DS, AX
    MOV ES, AX
    MOV SS, AX

    ; Clear the screen
    MOV AH, 0x06
    MOV AL, 0x00
    MOV BH, 0x07
    MOV CX, 0x0000
    MOV DH, 0x24
    INT 0x10

    ; Set cursor to top-left
    MOV AH, 0x02
    MOV BH, 0x00
    MOV DH, 0x00
    MOV DL, 0x00
    INT 0x10

    ; Define the null-terminated string
    MOV SI, WELCOME

    WECL:
        LODSB            ; Load the next byte of the string into AL
        TEST AL, AL      ; Check if AL (the current character) is null (end of string)
        JZ NEXT_WECL     ; If it's null, exit the loop
        MOV AH, 0x0E     ; Teletype output
        INT 0x10         ; Print the character
        JMP WECL         ; Repeat the loop for the next character

    NEXT_WECL:
        ; Load stage 2 bootloader to memory address 0x1000
        MOV AH, 0x02     ; Read disk sectors
        MOV AL, 0x08     ; Read 8 sectors (4K)
        MOV CH, 0x00     ; Cylinder 0
        MOV CL, 0x02     ; Sector 2
        MOV DH, 0x00     ; Head 0
        MOV DL, 0x80     ; Drive 0
        MOV BX, 0x1000   ; Load to memory address
        INT 0x13         ; Call BIOS interrupt

        ; Jump to stage 2 bootloader (0x1000)
        JMP 0x1000

        MOV AH, 0x0E
        MOV AL, 'F'
        MOV BH, 0x00
        INT 0x10
SPIN:
    HLT
    JMP SPIN


WELCOME db "Starting NEOS...", 0
TIMES 510 - ($ - $$) db 0
DW 0xAA55