BITS 16
ORG 0x7C00
Entry:
    CLI

    MOV AX, 0x0000
    MOV DS, AX
    MOV ES, AX
    MOV SS, AX

    ; Setup a stack
    MOV SP, 0x7C00


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
        LODSB            ; NeosExecutive the next byte of the string into AL
        TEST AL, AL      ; Check if AL (the current character) is null (end of string)
        JZ NEXT_WECL     ; If it's null, exit the loop
        MOV AH, 0x0E     ; Teletype output
        INT 0x10         ; Print the character
        JMP WECL         ; Repeat the loop for the next character
    NEXT_WECL:
        MOV AX, 0x7C0
        MOV ES, AX
        ; Iterate through MBR partition table items
        MOV SI, 0x1BE    ; Set SI to the start of the partition table
        MOV CX, 0x04     ; Set CX to the number of partition table items
    LOOPPART:
        MOV DI, SI       ; Copy SI to DI
        MOV AL, BYTE ES:[DI+0]; Byte 0: Boot indicator
        CMP AL, 0x80     ; Check if the partition is bootable
        JNE NEXTPART    ; If not, skip to the next partition
        MOV AL, BYTE ES:[DI+4]; Byte 4: Partition type
        CMP AL, 0x83     ; Check if the partition is of type 0x83 (Linux)
        JE FOUNDEXT2     ; If so, we've found an EXT2 partition
    NEXTPART:
        ADD SI, 0x10     ; Increment SI by 16 bytes (the size of a partition table item)
        LOOP LOOPPART    ; Repeat the loop for the next partition table item
        JMP NOBOOTPART
    FOUNDEXT2:
        ; Stage 2 bootloader from ext2 partition to memory address 0x1000
        PUSH DI         ; Save the boot partition table item address
        MOV DH, BYTE ES:[DI+1]; Byte 1: Starting head
        MOV CX, WORD ES:[DI+2]; Byte 2: Starting sector and cylinder
        XOR AX, AX
        MOV ES, AX       ; ES:BX = 0:0x1000
        MOV BX, 0x1000   ; ES:BX = EXT2 bootlaoder memory address
        MOV AH, 0x02     ; Read disk sectors
        MOV AL, 0x02     ; Read 2 sectors (we **assume** bytes per sector is 512)
        MOV DL, 0x80     ; Drive 0

        INT 0x13         ; Call BIOS interrupt
        JC READFAIL      ; Read failure
        JNC JumpToStage2
    

    JumpToStage2:
        ; Verify LBA INT13 extensions are supported
        MOV AH, 0x41
        MOV BX, 0x55AA
        MOV DL, 0x80
        INT 0x13
        JC READFAIL      ; Read failure

        ; Jump to stage 2 bootloader (0x1000)
        JMP 0x1000

    READFAIL:
        MOV AH, 0x0E
        MOV AL, 'F'
        MOV BH, 0x00
        INT 0x10
        JMP SPIN
NOBOOTPART:
    MOV SI, NOBOOTABLEPARTMSG
    OUTNOTBOOTMSG:
        LODSB
        TEST AL, AL      ; Check if AL (the current character) is null (end of string)
        JZ SPIN     ; If it's null, exit the loop
        MOV AH, 0x0E     ; Teletype output
        INT 0x10         ; Print the character
        JMP OUTNOTBOOTMSG         ; Repeat the loop for the next character

SPIN:
    HLT
    JMP SPIN

; Welcome message
WELCOME db "Starting NEOS...", 0
NOBOOTABLEPARTMSG db "No bootable partition found!", 0

; Bootloader signature
TIMES 510 - ($ - $$) db 0
DW 0xAA55