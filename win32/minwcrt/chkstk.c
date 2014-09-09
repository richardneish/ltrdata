/*
 * "Poor man's _chkstk()"
 *
 * Linked into minwcrt.lib and nullcrt.lib to support compilers that expect
 * a _chkstk() function in the runtime library. CRTDLL.DLL has no such
 * function, so we must keep a static copy in the default libraries.
 */

__declspec(naked) void __cdecl _chkstk(void)
{
  _asm
    {
      pop     edx             ; pop return address ;
      mov     ecx, 1000h      ; page size is 4K ;
loop1:
      cmp     eax, ecx        ; more than 4K left? ;
      jae     probe           ;
      mov     ecx, eax        ; less than 4K - make this our bump count ;
probe:
      sub     esp, ecx        ; bump stack down a bit ;
      mov     [esp], eax      ; probe this page ;
      sub     eax, ecx        ; decrement no. of bytes left ;
      jnz     loop1           ;
      jmp     edx             ; return to caller ;
    }
}
