
    ; - allow class.pointer.method(...)

    .x64
    .model flat, fastcall
    .code

    option win64:3

.class c1
    d db ?
    a proc :ptr
    .ends

.class c2
    d LPc1 ?
    a proc :ptr
    .ends

    .code

main proc a:ptr c2

  local l:c2
  local p:ptr c2

    l.a(0)
    l.d.a(0)
    p.a(0)
    p.d.a(0)
    a.a(0)
    a.d.a(0)

    ret

main endp

    end
