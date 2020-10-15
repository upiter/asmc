; STRINGCCHCATNW.ASM--
;
; Copyright (c) The Asmc Contributors. All rights reserved.
; Consult your license regarding permissions and restrictions.
;

include strsafe.inc

    .code

StringCchCatNW proc pszDest:STRSAFE_LPWSTR, cchDest:size_t, pszSrc:STRSAFE_LPCWSTR, cchToAppend:size_t

  local hr:HRESULT
  local cchDestLength:size_t

    StringValidateDestAndLengthW(pszDest,
            cchDest,
            &cchDestLength,
            STRSAFE_MAX_CCH)

    .ifs (SUCCEEDED(eax))

        .if (cchToAppend > STRSAFE_MAX_LENGTH)

            mov eax,STRSAFE_E_INVALID_PARAMETER

        .else

            mov rcx,pszDest
            add rcx,cchDestLength
            mov rdx,cchDest
            sub rdx,cchDestLength

            StringCopyWorkerW(rcx, rdx, NULL, pszSrc, STRSAFE_MAX_LENGTH)
        .endif
    .endif
    ret

StringCchCatNW endp

    end
