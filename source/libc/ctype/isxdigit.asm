include ctype.inc

	.code

	OPTION PROLOGUE:NONE, EPILOGUE:NONE

isxdigit PROC char:SINT
	movzx	eax,BYTE PTR [esp+4]
	mov	al, BYTE PTR _ctype[eax*2+2]
	and	eax,_HEX
	ret	4
isxdigit ENDP

	END

