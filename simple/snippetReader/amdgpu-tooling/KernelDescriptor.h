#ifndef KERNEL_DESCRIPTOR_H
#define KERNEL_DESCRIPTOR_H

#include "third-party/AMDHSAKernelDescriptor.h"

#include "Elf_X.h"
#include "Symtab.h"

#include <ostream>

class KernelDescriptor {
public:
  KernelDescriptor(const Dyninst::SymtabAPI::Symbol *symbol,
                   const Dyninst::Elf_X *elfHeader);

  uint32_t getGroupSegmentFixedSize() const;
  void setGroupSegmentFixedSize(uint32_t value);

  uint32_t getPrivateSegmentFixedSize() const;
  void setPrivateSegmentFixedSize(uint32_t value);

  uint32_t getKernargSize() const;
  void setKernargSize(uint32_t value);

  // 4 reserved bytes

  int64_t getKernelCodeEntryByteOffset() const;
  void setKernelCodeEntryByteOffset(int64_t value);

  // 20 reserved bytes

  // ----- COMPUTE_PGM_RSRC3 begin -----
  //
  // work with the entire register
  uint32_t getCOMPUTE_PGM_RSRC3() const;
  //
  // work with individual contents in the COMPUTE_PGM_RSRC3
  //
  // GFX90A, GFX940 begin
  //
  // NOTE : THIS IS A 6 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC3_AccumOffset() const;
  void setCOMPUTE_PGM_RSRC3_AccumOffset(uint32_t value);

  // 10 reserved bits

  bool getCOMPUTE_PGM_RSRC3_TgSplit() const;
  void setCOMPUTE_PGM_RSRC3_TgSplit(bool value);

  // 15 reserved bits
  // GFX90A, GFX940 end
  //
  // GFX10, GFX11 begin
  //
  // NOTE: THIS IS A 4 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC3_SharedVgprCount() const;
  void setCOMPUTE_PGM_RSRC3_SharedVgprCount(uint32_t value);

  // NOTE: THIS IS A 6 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC3_InstPrefSize() const;
  void setCOMPUTE_PGM_RSRC3_InstPrefSize(uint32_t value);

  bool getCOMPUTE_PGM_RSRC3_TrapOnStart() const;
  void setCOMPUTE_PGM_RSRC3_TrapOnStart(bool value);

  bool getCOMPUTE_PGM_RSRC3_TrapOnEnd() const;
  void setCOMPUTE_PGM_RSRC3_TrapOnEnd(bool value);

  // 19 reserved bits

  bool getCOMPUTE_PGM_RSRC3_ImageOp() const;
  void setCOMPUTE_PGM_RSRC3_ImageOp(bool value);
  //
  // ----- COMPUTE_PGM_RSRC3 end -----

  // ----- COMPUTE_PGM_RSRC1 begin -----
  //
  // work with the entire register
  uint32_t getCOMPUTE_PGM_RSRC1() const;
  //
  // work with individual contents in the COMPUTE_PGM_RSRC1
  //
  // NOTE : THIS IS A 6 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC1_GranulatedWorkitemVgprCount() const;
  void setCOMPUTE_PGM_RSRC1_GranulatedWorkitemVgprCount(uint32_t value);

  // NOTE : THIS IS A 4 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC1_GranulatedWavefrontSgprCount() const;
  void setCOMPUTE_PGM_RSRC1_GranulatedWavefrontSgprCount(uint32_t value);

  // NOTE : THIS IS A 2 BIT VALUE MUST BE 0 AND CANT BE SET BY USER.
  uint32_t getCOMPUTE_PGM_RSRC1_Priority() const;

  // NOTE : THIS IS A 2 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC1_FloatRoundMode32() const;
  void setCOMPUTE_PGM_RSRC1_FloatRoundMode32(uint32_t value);

  // NOTE : THIS IS A 2 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC1_FloatRoundMode1664() const;
  void setCOMPUTE_PGM_RSRC1_FloatRoundMode1664(uint32_t value);

  // NOTE : THIS IS A 2 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC1_FloatDenormMode32() const;
  void setCOMPUTE_PGM_RSRC1_FloatDenormMode32(uint32_t value);

  // NOTE : THIS IS A 2 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC1_FloatDenormMode1664() const;
  void setCOMPUTE_PGM_RSRC1_FloatDenormMode1664(uint32_t value);

  // NOTE : THIS BIT MUST BE 0 AND CANT BE SET BY USER.
  bool getCOMPUTE_PGM_RSRC1_Priv() const;

  bool getCOMPUTE_PGM_RSRC1_EnableDx10Clamp() const;
  void setCOMPUTE_PGM_RSRC1_EnableDx10Clamp(bool value);

  // NOTE : THIS BIT MUST BE 0 AND CANT BE SET BY USER.
  bool getCOMPUTE_PGM_RSRC1_DebugMode() const;

  bool getCOMPUTE_PGM_RSRC1_EnableIeeeMode() const;
  void setCOMPUTE_PGM_RSRC1_EnableIeeeMode(bool value);

  // NOTE : THIS BIT MUST BE 0 AND CANT BE SET BY USER.
  bool getCOMPUTE_PGM_RSRC1_Bulky() const;

  // NOTE : THIS BIT MUST BE 0 AND CANT BE SET BY USER.
  bool getCOMPUTE_PGM_RSRC1_CdbgUser() const;

  bool getCOMPUTE_PGM_RSRC1_Fp16Ovfl() const;
  void setCOMPUTE_PGM_RSRC1_Fp16Ovfl(bool value);

  // 2 reserved bits

  bool getCOMPUTE_PGM_RSRC1_WgpMode() const;
  void setCOMPUTE_PGM_RSRC1_WgpMode(bool value);

  bool getCOMPUTE_PGM_RSRC1_MemOrdered() const;
  void setCOMPUTE_PGM_RSRC1_MemOrdered(bool value);

  bool getCOMPUTE_PGM_RSRC1_FwdProgress() const;
  void setCOMPUTE_PGM_RSRC1_FwdProgress(bool value);
  //
  // ----- COMPUTE_PGM_RSRC1 end -----

  // ----- COMPUTE_PGM_RSRC2 begin -----
  //
  // work with the entire register
  uint32_t getCOMPUTE_PGM_RSRC2() const;
  //
  // work with individual contents in the COMPUTE_PGM_RSRC2
  //
  bool getCOMPUTE_PGM_RSRC2_EnablePrivateSegment() const;
  void setCOMPUTE_PGM_RSRC2_EnablePrivateSegment(bool value);

  // NOTE : THIS IS A 5 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC2_UserSgprCount() const;
  void setCOMPUTE_PGM_RSRC2_UserSgprCount(uint32_t value);

  bool getCOMPUTE_PGM_RSRC2_EnableTrapHandler() const;

  bool getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdX() const;
  void setCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdX(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdY() const;
  void setCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdY(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdZ() const;
  void setCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdZ(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupInfo() const;
  void setCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupInfo(bool value);

  // NOTE : THIS IS A 2 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC2_EnableVgprWorkitemId() const;
  void setCOMPUTE_PGM_RSRC2_EnableVgprWorkitemId(uint32_t value);

  // NOTE: USER CAN't SET THIS
  bool getCOMPUTE_PGM_RSRC2_EnableExceptionAddressWatch() const;

  bool getCOMPUTE_PGM_RSRC2_EnableExceptionMemory() const;

  // NOTE : THIS IS A 9 BIT VALUE
  uint32_t getCOMPUTE_PGM_RSRC2_GranulatedLdsSize() const;

  bool getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInvalidOperation() const;
  void
  getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInvalidOperation(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableExceptionFpDenormalSource() const;
  void setCOMPUTE_PGM_RSRC2_EnableExceptionFpDenormalSource(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpDivisionByZero() const;
  void setCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpDivisionByZero(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpOverflow() const;
  void setCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpOverflow(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpUnderflow() const;
  void setCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpUnderflow(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInexact() const;
  void setCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInexact(bool value);

  bool getCOMPUTE_PGM_RSRC2_EnableExceptionIntDivideByZero() const;
  void setCOMPUTE_PGM_RSRC2_EnableExceptionIntDivideByZero(bool value);

  // one reserved bit
  //
  // ----- COMPUTE_PGM_RSRC2 end -----

  // 7 bits after COMPUTE_PGM_RSRC2
  bool getKernelCodeProperty_EnableSgprPrivateSegmentBuffer() const;
  void setKernelCodeProperty_EnableSgprPrivateSegmentBuffer(bool value);

  bool getKernelCodeProperty_EnableSgprDispatchPtr() const;
  void setKernelCodeProperty_EnableSgprDispatchPtr(bool value);

  bool getKernelCodeProperty_EnableSgprQueuePtr() const;
  void setKernelCodeProperty_EnableSgprQueuePtr(bool value);

  bool getKernelCodeProperty_EnableSgprKernargSegmentPtr() const;
  void setKernelCodeProperty_EnableSgprKernargSegmentPtr(bool value);

  bool getKernelCodeProperty_EnableSgprDispatchId() const;
  void setKernelCodeProperty_EnableSgprDispatchId(bool value);

  bool getKernelCodeProperty_EnableSgprFlatScratchInit() const;
  void setKernelCodeProperty_EnableSgprFlatScratchInit(bool value);

  bool getKernelCodeProperty_EnablePrivateSegmentSize() const;
  void setKernelCodeProperty_EnablePrivateSegmentSize(bool value);

  // done with those 7 bits

  // 3 reserved bits

  bool getKernelCodeProperty_EnableWavefrontSize32() const;
  void setKernelCodeProperty_EnableWavefrontSize32(bool value);

  // TODO : the llvm definition doesn't have this in mono repo yet
  bool getKernelCodeProperty_UsesDynamicStack() const;
  void setKernelCodeProperty_UsesDynamicStack(bool value);

  void dump(std::ostream &os) const;
  void dumpDetailed(std::ostream &os) const;

  bool verify() const;

private:
  // read numBytes bytes starting at fromIndex in rawBytes into data
  void readToKd(const uint8_t *rawBytes, size_t rawBytesLength,
                size_t fromIndex, size_t numBytes, uint8_t *data);

  bool verifyCOMPUTE_PGM_RSRC3() const;
  void dumpCOMPUTE_PGM_RSRC3(std::ostream &os) const;
  void dumpCOMPUTE_PGM_RSRC3_Gfx90aOr940(std::ostream &os) const;
  void dumpCOMPUTE_PGM_RSRC3_Gfx10Plus(std::ostream &os) const;

  bool verifyCOMPUTE_PGM_RSRC1() const;
  void dumpCOMPUTE_PGM_RSRC1(std::ostream &os) const;

  bool verifyCOMPUTE_PGM_RSRC2() const;
  void dumpCOMPUTE_PGM_RSRC2(std::ostream &os) const;

  bool verifyKernelCodeProperties() const;
  void dumpKernelCodeProperties(std::ostream &os) const;

  bool isGfx6() const;
  bool isGfx7() const;
  bool isGfx8() const;
  bool isGfx9() const;
  bool isGfx90aOr940() const;
  bool isGfx9Plus() const;
  bool isGfx10() const;
  bool isGfx10Plus() const;
  bool isGfx11() const;

  bool supportsArchitectedFlatScratch() const;

  std::string name;
  // canonical kernel descriptor struct
  llvm::amdhsa::kernel_descriptor_t kdRepr;

  Dyninst::SymtabAPI::Object *object;

  const Dyninst::Elf_X *elfHdr;

  unsigned amdgpuMach;
};

#endif
