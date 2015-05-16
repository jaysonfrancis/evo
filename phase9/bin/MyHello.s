# MyHello.s
#
# in order to perform syscall MsgSnd(&msg), MsgRcv(&msg), need to
# first calculate where the real RAM addr of my msg is:
# use ESP - 4096 to get the base of the 4KB RAM page this program resides.
# Since $msg is virtual, at 2G+x (0x80000000+x), and the page is virtual
# at 2G (0x80000000); subtract 2G from $msg gets x. Add x to the base is
# the real RAM addr of the msg.

.text                       # code segment
.global _start              # _start is main()

_start:                     # instructions begin
                            # 1st, get real addr of local msg
   pushl %esp #(copy the stack pointer:)
   popl %ebx #push the stack pointer
   subl $1000, %ebx #pop into register ebx
   #subtract 4096 from it (this is the base, real addr of the page)

   movl $msg, %ecx #copy $msg to register ecx
   subl $_start, %ecx #subtract 2G from it, get x (offset)

   addl %ecx, %ebx #add  x (offset) to ebx (base of page) -- where msg really is
   pushl %ebx #save a copy (push it to stack)
   pushl %ebx #save another copy (push it again)

   movl $0x35, %eax
   int $0x80 #call interrupt number 53  # MsgSnd(&msg)

   popl %ebx #pop to ebx (get a copy of real msg addr)
   movl $0x35, %eax
   int $0x80 #call interrupt number 54  # MsgRcv(&msg)

   popl %ecx, #pop to ecx (get a copy, real msg addr)
   movl %ecx, %ebx #copy time stamp (base ecx + offset of time stamp) to ebx
   addl $0x40, %ebx

   movl $39, %eax #call interrupt number 57  # Exit(time stamp)
   int $0x80 

.data                       # data segment follows code segment in RAM
msg:                        # my msg
  .long 0                            # msg.sender
  .long 4                          # msg.recipient
  .long 0                          # msg.time_stamp
  .ascii "Hello from a user space program."   # msg.data
  .rept 69
    ascii "\0"
  .endr
  .long 0
  .long 0
  .long 0
  .long 0
  .long 0
          

