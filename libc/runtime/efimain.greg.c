/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/efi.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nt/efi.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"

struct EfiArgs {
  char *args[4096];
  char argblock[ARG_MAX];
};

static const EFI_GUID kEfiLoadedImageProtocol = LOADED_IMAGE_PROTOCOL;

/**
 * EFI Application Entrypoint.
 *
 * This entrypoint is mutually exclusive from WinMain since
 * Windows apps and EFI apps use the same PE binary format.
 * By default, we build binaries to support Windows. If you
 * want to your APE executable to boot on UEFI instead then
 * you need to run the following build command:
 *
 *     make -j8 CPPFLAGS=-DSUPPORT_VECTOR=251
 *
 * That'll remove all the Windows code and turn EFI on. You
 * can also remove by BIOS code too, by changing 251 to 249
 * but it shouldn't matter. Here's how to emulate EFI apps:
 *
 *     qemu-system-x86_64 \
 *       -bios OVMF.fd    \
 *       -nographic       \
 *       -net none        \
 *       -drive format=raw,file=fat:rw:o/tool/viz
 *     FS0:
 *     deathstar.com
 *
 * If you're using the amalgamated release binaries then it
 * should be possible to enable UEFI mode by having this at
 * the top of your main source file to hint the APE linker:
 *
 *     STATIC_YOINK("EfiMain");
 *     int main() { ... }
 *
 * @see libc/dce.h
 */
__msabi noasan EFI_STATUS EfiMain(EFI_HANDLE ImageHandle,
                                  EFI_SYSTEM_TABLE *SystemTable) {
  intptr_t argc;
  struct EfiArgs *ea;
  EFI_LOADED_IMAGE *img;
  extern char os asm("__hostos");
  os = UEFI;
  __efi_image_handle = ImageHandle;
  __efi_system_table = SystemTable;
  SystemTable->BootServices->AllocatePool(EfiConventionalMemory, sizeof(*ea),
                                          &ea);
  SystemTable->BootServices->HandleProtocol(ImageHandle,
                                            &kEfiLoadedImageProtocol, &img);
  argc = GetDosArgv(img->LoadOptions, ea->argblock, ARG_MAX, ea->args,
                    ARRAYLEN(ea->args));
  asm("push\t$0\n\t" /* auxv[0][1] */
      "push\t$0\n\t" /* auxv[0][0] */
      "push\t$0\n\t" /* envp[0] */
      "sub\t%2,%%rsp\n\t"
      "mov\t%%rsp,%%rdi\n\t"
      "rep movsb\n\t" /* argv */
      "push\t%0\n\t"  /* argc */
      "xor\t%%edi,%%edi\n\t"
      ".weak\t_start\n\t"
      "jmp\t_start"
      : /* no outputs */
      : "a"(argc), "S"(ea->args), "c"((argc + 1) * 8));
  unreachable;
}