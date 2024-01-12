.data
_t14: .word 0
_t13: .word 0
_t12: .word 0
_t10: .word 0
_t9: .word 0
_t7: .word 0
_t6: .word 0
_t4: .word 0
_t3: .word 0
_t1: .word 0
_v2: .word 0
_v1: .word 0
_prmpt: .asciiz "Enter an integer: "
_eol: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prmpt
  syscall
  li $v0, 5
  syscall
  jr $ra
write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _eol
  syscall
  move $v0, $0
  jr $ra
main:
  move $t0, $zero
  sw $t0, _v1
  move $t0, $zero
  sw $t0, _v2
  sw $t1, 0($t0)
Error: Unsupported operand type in loadVar2Reg

  (null)
  li $t1, 4
  add $t2, $t0, $t1
  sw $t2, _t1
  sw $t3, 0($t2)
  li $t0, 2
  li $t1, 4
  mul $t2, $t0, $t1
  sw $t2, _t3
Error: Unsupported operand type in loadVar2Reg

  (null)
  lw $t1, _t3
  add $t2, $t0, $t1
  sw $t2, _t4
  sw $t5, 0($t4)
  li $t0, 3
  li $t1, 4
  mul $t2, $t0, $t1
  sw $t2, _t6
Error: Unsupported operand type in loadVar2Reg

  (null)
  lw $t1, _t6
  add $t2, $t0, $t1
  sw $t2, _t7
  sw $t7, 0($t6)
  li $t0, 4
  li $t1, 4
  mul $t2, $t0, $t1
  sw $t2, _t9
Error: Unsupported operand type in loadVar2Reg

  (null)
  lw $t1, _t9
  add $t2, $t0, $t1
  sw $t2, _t10
  sw $s1, 0($s0)
label0:
  lw $t0, _v2
  li $t1, 5
  blt $t0, $t1, label1
  j label2
label1:
  lw $t0, _v2
  li $t1, 4
  mul $t2, $t0, $t1
  sw $t2, _t12
Error: Unsupported operand type in loadVar2Reg

  (null)
  lw $t1, _t12
  add $t2, $t0, $t1
  sw $t2, _t13
  lw $s2, 0($s3)
  lw $t0, _v1
  lw $t1, _t14
  add $t2, $t0, $t1
  sw $t2, _v1
  lw $t0, _v2
  li $t1, 1
  add $t2, $t0, $t1
  sw $t2, _v2
  j label0
label2:
  lw $a0, _v1
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  li $v0, 0
  jr $ra
