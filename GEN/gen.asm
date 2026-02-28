.586
.model flat, stdcall
includelib libucrt.lib
includelib kernel32.lib
includelib D:\AAP-2025Compiler-main\AAP-2025\AAP-2025\Debug\Lib.lib
ExitProcess PROTO :DWORD

EXTRN COMBINE: proc
EXTRN LEXCOMP: proc
EXTRN MEASURE: proc
EXTRN BREAKL: proc
EXTRN RANDOMIZE: proc
EXTRN EXPONENT: proc
EXTRN read: proc
EXTRN OutputInt: proc
EXTRN OutputStr: proc

.stack 4096

.const
	L1 DWORD 2
	L2 DWORD 5
	L3 DWORD 0
	L4 DWORD 1
	L5 DWORD 100
	L6 DWORD 42
	L7 BYTE "A", 0
	L8 BYTE "Hi", 0
	L9 DWORD 1
	L10 BYTE "there ", 0
	L11 DWORD 1
	L12 DWORD 5
	L13 DWORD 2
	L14 DWORD 4
	L15 DWORD 25
	L16 BYTE "abc", 0
	L17 BYTE "abd", 0
	L18 DWORD 16
	L19 BYTE "== works", 0
	L20 BYTE "== fail", 0
	L21 BYTE ">= works", 0
	L22 DWORD 3
	L23 BYTE "<= works", 0
	L24 BYTE "> works", 0
	L25 BYTE "< works", 0
	L26 BYTE "!= works", 0
	L27 DWORD 101
	L28 BYTE "switch case hit", 0
	L29 DWORD 102
	L30 BYTE "other case", 0
	L31 BYTE "switch default", 0
	L32 DWORD 3
	L33 DWORD 1
	L34 DWORD 41
	L35 BYTE "assignment ok", 0

.data
	buffer BYTE 256 dup(0)
	tmpsqrt SDWORD 0
	addsum DWORD 0
	loopshowi DWORD 0
	maindec DWORD 0
	mainhexv DWORD 0
	mainletter DWORD ?
	maintext DWORD ?
	mainflag DWORD 0
	mainlinked DWORD ?
	mainlen DWORD 0
	mainrandv DWORD 0
	mainpowv DWORD 0
	mainsq DWORD 0
	maincmp DWORD 0
	mainres DWORD 0

.code

ABS PROC value : SDWORD
	mov eax, value
	cdq
	xor eax, edx
	sub eax, edx
	ret 4
ABS ENDP

SQRTI PROC value : SDWORD
	finit
	fild value
	fsqrt
	fistp tmpsqrt
	mov eax, tmpsqrt
	ret 4
SQRTI ENDP

fn_add PROC adda : DWORD, addb : DWORD
	push adda
	push addb
	pop eax
	pop ebx
	add eax, ebx
	push eax
	pop addsum
	push addsum
	push L1
	pop eax
	pop ebx
	mul ebx
	push eax
	push L2
	pop ebx
	pop eax
	cdq
	idiv ebx
	push edx
	pop addsum
	push addsum
	jmp local0
local0:
	pop eax
	ret
fn_add ENDP

fn_loopshow PROC loopshown : DWORD
	push L3
	pop loopshowi
	mov eax, loopshowi
	cmp eax, loopshown
	jl cycle0
	jmp cyclenext0
cycle0:
	push loopshowi
	call OutputInt
	call BREAKL
	push loopshowi
	push L4
	pop eax
	pop ebx
	add eax, ebx
	push eax
	pop loopshowi
	mov eax, loopshowi
	cmp eax, loopshown
	jl cycle0
cyclenext0:
	call BREAKL
	ret
fn_loopshow ENDP

main PROC
	push L5
	pop maindec
	push L6
	pop mainhexv
	push offset L7
	pop mainletter
	push offset L8
	pop maintext
	push L9
	pop mainflag
	push maintext
	push offset L10
	push offset buffer
	call COMBINE
	push eax
	pop mainlinked
	push mainlinked
	call OutputStr
	call BREAKL
	push mainlinked
	call MEASURE;
	push eax
	pop mainlen
	push mainlen
	call OutputInt
	call BREAKL
	push L11
	push L12
	call RANDOMIZE;
	push eax
	pop mainrandv
	push mainrandv
	call OutputInt
	call BREAKL
	push L13
	push L14
	call EXPONENT;
	push eax
	pop mainpowv
	push mainpowv
	call OutputInt
	call BREAKL
	push L15
	call SQRTI;
	push eax
	pop mainsq
	push mainsq
	call OutputInt
	call BREAKL
	push offset L16
	push offset L17
	call LEXCOMP;
	push eax
	pop maincmp
	push maincmp
	call OutputInt
	call BREAKL
	mov eax, mainpowv
	cmp eax, L18
	jz m0
	jnz m1
	je m1
m0:
	push offset L19
	call OutputStr
	call BREAKL
	jmp e0
m1:
	push offset L20
	call OutputStr
	call BREAKL
e0:
	mov eax, maindec
	cmp eax, mainhexv
	jg m2
	jz m2
	jl m3
	je m3
m2:
	push offset L21
	call OutputStr
	call BREAKL
m3:
	mov eax, mainrandv
	cmp eax, L22
	jl m4
	jz m4
	jg m5
	je m5
m4:
	push offset L23
	call OutputStr
	call BREAKL
m5:
	mov eax, maindec
	cmp eax, mainhexv
	jg m6
	jl m7
	je m7
m6:
	push offset L24
	call OutputStr
	call BREAKL
m7:
	mov eax, mainhexv
	cmp eax, maindec
	jl m8
	jg m9
	je m9
m8:
	push offset L25
	call OutputStr
	call BREAKL
m9:
	mov eax, maindec
	cmp eax, mainpowv
	jnz m10
	jz m11
	je m11
m10:
	push offset L26
	call OutputStr
	call BREAKL
m11:
	push maindec
	pop eax
	cmp eax, 101
	jne swskip0_1
swcase0_1:
	push offset L28
	call OutputStr
	call BREAKL
	jmp swend0
swskip0_1:
	cmp eax, 102
	jne swskip0_2
swcase0_2:
	push offset L30
	call OutputStr
	call BREAKL
	jmp swend0
swskip0_2:
swdefault0:
	push offset L31
	call OutputStr
	call BREAKL
	jmp swend0
swend0:
	push maindec
	push mainhexv
	pop mainres
	push mainres
	call OutputInt
	call BREAKL
	push L32
	call fn_loopshow
	push maindec
	push L33
	pop ebx
	pop eax
	sub eax, ebx
	push eax
	pop maindec
	mov eax, maindec
	cmp eax, L34
	jz m12
	jnz m13
	je m13
m12:
	push offset L35
	call OutputStr
	call BREAKL
m13:
	push 0
	call ExitProcess
main ENDP
end main