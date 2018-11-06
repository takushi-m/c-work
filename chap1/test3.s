.intel_syntax noprefix
.global _plus,_main

_plus:
  add rsi, rdi
  mov rax, rsi
  ret

_main:
  mov rdi, 3
  mov rsi, 4
  call _plus
  ret
