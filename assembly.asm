.code

; Export the function with proper decoration
public direct_device_control

direct_device_control PROC
    mov r10, rcx        ; Move first parameter to r10 for syscall
    mov eax, 7          ; Syscall number for NtDeviceIoControlFile
    syscall
    ret
direct_device_control ENDP

END