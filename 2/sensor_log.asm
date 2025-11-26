section .data
    filename db 'sensor_log.txt', 0    ; filename in local director
    msg_total db 'Total sensor readings: ', 0
    msg_total_len equ $ - msg_total
    newline db 10                      ; Newline character...god I hate assembly
    
    ; This entire program really is just a line counting app. Almost 150 lines just to count lines...wow.
    ; Buffer size allocation because need to read that file somehow.
    buffer_size equ 4096
    buffer times buffer_size db 0
    
section .bss
    file_descriptor resd 1             ; Self-explanatory more so than other parts here.
    bytes_read resd 1                  
    line_count resd 1                  
    digit_buffer resb 16               

section .text
    global _start

_start:
    ; The file is opened using sys_open, reads into a buffer using sys_read, and then
    ; processed in memory to count lines of our given input file.
    
    ; Open up that file 
    mov eax, 5              
    mov ebx, filename       
    mov ecx, 0              
    mov edx, 0              
    int 0x80
    
    ; Check that file opened
    cmp eax, 0
    jl exit_error           
    
    mov [file_descriptor], eax  
    
    mov dword [line_count], 1   
    mov dword [bytes_read], 0

read_loop:
    ; Read part of file straight to that buffer
    mov eax, 3              
    mov ebx, [file_descriptor] 
    mov ecx, buffer         
    mov edx, buffer_size    
    int 0x80
    
    ; Check if read is successful
    cmp eax, 0
    jl close_file_error    
    je process_complete    
    
    mov [bytes_read], eax  
    ; We traversing the buffer byte by byte, counting  characters.
    mov esi, buffer         
    mov ecx, [bytes_read]   
    
process_buffer:
    mov al, [esi]
    cmp al, 10              
    jne next_char
    
    inc dword [line_count]
    
next_char:
    inc esi                 ; Moving to next character
    loop process_buffer     ; Continue until buffering is fully done.
    
    jmp read_loop           

process_complete:
    ; The program uses a main read loop that continues with each buffer. Conditional jumps handle errors and program flow.

    ; Closes the file
    mov eax, 6              ; sys_close
    mov ebx, [file_descriptor]
    int 0x80
    
    ; Display "Total sensor readings: " message
    mov eax, 4              ; sys_write
    mov ebx, 1              ; stdout
    mov ecx, msg_total
    mov edx, msg_total_len
    int 0x80
    
    ; Convert line count to string and display
    mov eax, [line_count]
    call print_number
    
    ; Print newline
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80
    
    ; Exit successfully
    mov eax, 1             
    mov ebx, 0              
    int 0x80

exit_error:
    ; Normal error handling for file opening 
    mov eax, 1
    mov ebx, 1             
    int 0x80

close_file_error:
    ; Closed file and exit error
    mov eax, 6
    mov ebx, [file_descriptor]
    int 0x80
    mov eax, 1
    mov ebx, 1
    int 0x80


; Converts integer in EAX to string outputs it through print
print_number:
    mov edi, digit_buffer + 15  
    mov byte [edi], 0           
    mov ebx, 10                 
    
.convert_loop:
    xor edx, edx                
    div ebx                     
    add dl, '0'                 
    dec edi                   
    mov [edi], dl               
    test eax, eax               
    jnz .convert_loop          
    
    ; Prints the number
    mov ecx, edi                ; Pointer to number string
    mov edx, digit_buffer + 16  ; Calculate length
    sub edx, edi
    mov eax, 4                  ; sys_write
    mov ebx, 1                  ; stdout
    int 0x80
    
    ret