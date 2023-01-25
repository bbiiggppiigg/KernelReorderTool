#include "KernelDescriptor.h"

#include "Elf_X.h"

#include "third-party/AMDGPUFlags.h"

#include <cassert>
#include <iomanip>

using namespace llvm;
using namespace amdhsa;
using namespace Dyninst;
using namespace SymtabAPI;

void KernelDescriptor::readToKd(const uint8_t *rawBytes, size_t rawBytesLength,
                                size_t fromIndex, size_t numBytes,
                                uint8_t *data) {
  assert(rawBytes && "rawBytes must be non-null");
  assert(data && "data must be non-null");
  assert(fromIndex + numBytes <= rawBytesLength);

  for (size_t i = 0; i < numBytes; ++i) {
    size_t idx = fromIndex + i;
    data[i] = rawBytes[idx];
  }
}

bool KernelDescriptor::isGfx6() const {
  return (amdgpuMach >= EF_AMDGPU_MACH_AMDGCN_GFX600 &&
          amdgpuMach <= EF_AMDGPU_MACH_AMDGCN_GFX601) ||
         amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX602;
}

bool KernelDescriptor::isGfx7() const {
  return (amdgpuMach >= EF_AMDGPU_MACH_AMDGCN_GFX700 &&
          amdgpuMach <= EF_AMDGPU_MACH_AMDGCN_GFX704) ||
         amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX705;
}

bool KernelDescriptor::isGfx8() const {
  return (amdgpuMach >= EF_AMDGPU_MACH_AMDGCN_GFX801 &&
          amdgpuMach <= EF_AMDGPU_MACH_AMDGCN_GFX810) ||
         amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX805;
}

bool KernelDescriptor::isGfx9() const {
  return isGfx90aOr940() || (amdgpuMach >= EF_AMDGPU_MACH_AMDGCN_GFX900 &&
                             amdgpuMach <= EF_AMDGPU_MACH_AMDGCN_GFX90C);
}

bool KernelDescriptor::isGfx90aOr940() const {
  return amdgpuMach >= EF_AMDGPU_MACH_AMDGCN_GFX90A &&
         amdgpuMach <= EF_AMDGPU_MACH_AMDGCN_GFX940;
}

bool KernelDescriptor::isGfx9Plus() const { return isGfx9() || isGfx10Plus(); }

bool KernelDescriptor::isGfx10() const {
  return (amdgpuMach >= EF_AMDGPU_MACH_AMDGCN_GFX1010 &&
          amdgpuMach <= EF_AMDGPU_MACH_AMDGCN_GFX1033) ||
         amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX1013 ||
         amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX1034 ||
         amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX1035 ||
         amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX1036;
}

bool KernelDescriptor::isGfx10Plus() const { return isGfx10() || isGfx11(); }

bool KernelDescriptor::isGfx11() const {
  return amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX1100 ||
         amdgpuMach == EF_AMDGPU_MACH_AMDGCN_GFX1103 ||
         (amdgpuMach >= EF_AMDGPU_MACH_AMDGCN_GFX1101 &&
          amdgpuMach >= EF_AMDGPU_MACH_AMDGCN_GFX1102);
}

KernelDescriptor::KernelDescriptor(const Symbol *symbol, const Elf_X *elfHeader)
    : elfHdr(elfHeader) {
  assert(symbol && "symbol must be non-null");
  assert(elfHeader && "elfHeader must be non-null");
  assert(elfHeader->e_machine() == EM_AMDGPU && "must be dealing with AMDGPU");

  name = symbol->getMangledName();
  const size_t kdSize = 64;

  // This region corresponds to the section
  const Region *region = symbol->getRegion();
  const size_t regionSize = region->getDiskSize();

  assert(sizeof(kernel_descriptor_t) == kdSize);
  assert(regionSize >= kdSize);

  amdgpuMach = elfHdr->e_flags() & EF_AMDGPU_MACH; // llvm::EF_AMDGPU_MACH

  const Offset regionStartOffset = region->getDiskOffset();
  const size_t kdBeginIdx = symbol->getOffset() - regionStartOffset;
  const uint8_t *kdBytes =
      (const uint8_t *)region->getPtrToRawData() + kdBeginIdx;

  // We read from kdBytes to kdPtr as per the kernel descriptor format.
  uint8_t *kdPtr = (uint8_t *)&kdRepr;

  size_t idx = 0;
  while (idx != kdSize) {
    switch (idx) {
    case amdhsa::GROUP_SEGMENT_FIXED_SIZE_OFFSET:
      readToKd(kdBytes, kdSize, idx, sizeof(uint32_t), kdPtr + idx);
      idx += sizeof(uint32_t);
      break;

    case amdhsa::PRIVATE_SEGMENT_FIXED_SIZE_OFFSET:
      readToKd(kdBytes, kdSize, idx, sizeof(uint32_t), kdPtr + idx);
      idx += sizeof(uint32_t);
      break;

    case amdhsa::KERNARG_SIZE_OFFSET:
      readToKd(kdBytes, kdSize, idx, sizeof(uint32_t), kdPtr + idx);
      idx += sizeof(uint32_t);
      break;

    case amdhsa::RESERVED0_OFFSET:
      readToKd(kdBytes, kdSize, idx, 4 * sizeof(int8_t), kdPtr + idx);
      idx += 4 * sizeof(uint8_t);
      break;

    case amdhsa::KERNEL_CODE_ENTRY_BYTE_OFFSET_OFFSET:
      readToKd(kdBytes, kdSize, idx, sizeof(uint64_t), kdPtr + idx);
      idx += sizeof(uint64_t);
      break;

    case amdhsa::RESERVED1_OFFSET:
      readToKd(kdBytes, kdSize, idx, 20 * sizeof(uint8_t), kdPtr + idx);
      idx += 20 * sizeof(uint8_t);
      break;

    case amdhsa::COMPUTE_PGM_RSRC3_OFFSET:
      readToKd(kdBytes, kdSize, idx, sizeof(uint32_t), kdPtr + idx);
      idx += sizeof(uint32_t);
      break;

    case amdhsa::COMPUTE_PGM_RSRC1_OFFSET:
      readToKd(kdBytes, kdSize, idx, sizeof(uint32_t), kdPtr + idx);
      idx += sizeof(uint32_t);
      break;

    case amdhsa::COMPUTE_PGM_RSRC2_OFFSET:
      readToKd(kdBytes, kdSize, idx, sizeof(uint32_t), kdPtr + idx);
      idx += sizeof(uint32_t);
      break;

    case amdhsa::KERNEL_CODE_PROPERTIES_OFFSET:
      readToKd(kdBytes, kdSize, idx, sizeof(uint16_t), kdPtr + idx);
      idx += sizeof(uint16_t);
      break;

    case amdhsa::RESERVED2_OFFSET:
      readToKd(kdBytes, kdSize, idx, 6 * sizeof(uint8_t), kdPtr + idx);
      idx += 6 * sizeof(uint8_t);
      break;
    }
  }

  // FIXME: this assertion fails sometimes
  // assert(verify() && "Kernel descriptor must be well formed");
}

bool KernelDescriptor::verify() const {
  for (int idx = 0; idx < 4; ++idx) {
    if (kdRepr.reserved0[idx] != 0)
      return false;
  }

  for (int idx = 0; idx < 20; ++idx) {
    if (kdRepr.reserved1[idx] != 0)
      return false;
  }

  for (int idx = 0; idx < 6; ++idx) {
    if (kdRepr.reserved2[idx] != 0)
      return false;
  }

  if (verifyCOMPUTE_PGM_RSRC3() == false)
    return false;

  if (verifyCOMPUTE_PGM_RSRC1() == false)
    return false;

  if (verifyCOMPUTE_PGM_RSRC2() == false)
    return false;

  // FIXME: this returns false
  // if (verifyKernelCodeProperties() == false)
  //   return false;

  return true;
}

uint32_t KernelDescriptor::getGroupSegmentFixedSize() const {
  return kdRepr.group_segment_fixed_size;
}

void KernelDescriptor::setGroupSegmentFixedSize(uint32_t value) {
  kdRepr.group_segment_fixed_size = value;
}

uint32_t KernelDescriptor::getPrivateSegmentFixedSize() const {
  return kdRepr.private_segment_fixed_size;
}

void KernelDescriptor::setPrivateSegmentFixedSize(uint32_t value) {
  kdRepr.private_segment_fixed_size = value;
}

uint32_t KernelDescriptor::getKernargSize() const {
  return kdRepr.kernarg_size;
}

void KernelDescriptor::setKernargSize(uint32_t value) {
  kdRepr.kernarg_size = value;
}

int64_t KernelDescriptor::getKernelCodeEntryByteOffset() const {
  return kdRepr.kernel_code_entry_byte_offset;
}

void KernelDescriptor::setKernelCodeEntryByteOffset(int64_t value) {
  kdRepr.kernel_code_entry_byte_offset = value;
}

#define GET_VALUE(MASK) ((fourByteBuffer & MASK) >> (MASK##_SHIFT))
#define SET_VALUE(MASK) (fourByteBuffer | ((value) << (MASK##_SHIFT)))
#define CLEAR_BITS(MASK) (fourByteBuffer & (~(MASK)))
#define CHECK_WIDTH(MASK) ((value) >> (MASK##_WIDTH) == 0)

#define GET_ITH_BIT_AFTER(MASK, i)                                             \
  (((fourByteBuffer & (1 << ((MASK##_WIDTH) + i - 1))) << (MASK##_SHIFT)) != 0)

// ----- COMPUTE_PGM_RSRC3 begin -----
//
//
uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC3() const {
  return kdRepr.compute_pgm_rsrc3;
}

// GFX90A, GFX940 begin
uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC3_AccumOffset() const {
  assert(isGfx90aOr940());
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  return GET_VALUE(COMPUTE_PGM_RSRC3_GFX90A_ACCUM_OFFSET);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC3_AccumOffset(uint32_t value) {
  assert(isGfx90aOr940());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC3_GFX90A_ACCUM_OFFSET);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC3_GFX90A_ACCUM_OFFSET) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc3 = SET_VALUE(COMPUTE_PGM_RSRC3_GFX90A_ACCUM_OFFSET);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC3_TgSplit() const {
  assert(isGfx90aOr940());
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  return GET_VALUE(COMPUTE_PGM_RSRC3_GFX90A_TG_SPLIT);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC3_TgSplit(bool value) {
  assert(isGfx90aOr940());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC3_GFX90A_TG_SPLIT);
  kdRepr.compute_pgm_rsrc3 = SET_VALUE(COMPUTE_PGM_RSRC3_GFX90A_TG_SPLIT);
}
//
// GFX90A, GFX940 end

// GFX10, GFX11 begin
uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC3_SharedVgprCount() const {
  assert(isGfx10Plus());
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  return GET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_SHARED_VGPR_COUNT);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC3_SharedVgprCount(uint32_t value) {
  assert(isGfx10Plus());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC3_GFX10_PLUS_SHARED_VGPR_COUNT);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC3_GFX10_PLUS_SHARED_VGPR_COUNT) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc3 =
      SET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_SHARED_VGPR_COUNT);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC3_InstPrefSize() const {
  assert(isGfx10Plus());
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  return GET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_INST_PREF_SIZE);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC3_InstPrefSize(uint32_t value) {
  assert(isGfx11());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC3_GFX10_PLUS_INST_PREF_SIZE);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC3_GFX10_PLUS_INST_PREF_SIZE) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc3 =
      SET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_INST_PREF_SIZE);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC3_TrapOnStart() const {
  assert(isGfx10Plus());
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  return GET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_TRAP_ON_START);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC3_TrapOnStart(bool value) {
  assert(isGfx11());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC3_GFX10_PLUS_TRAP_ON_START);
  kdRepr.compute_pgm_rsrc3 =
      SET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_TRAP_ON_START);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC3_TrapOnEnd() const {
  assert(isGfx10Plus());
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  return GET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_TRAP_ON_END);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC3_TrapOnEnd(bool value) {
  assert(isGfx11());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC3_GFX10_PLUS_TRAP_ON_END);
  kdRepr.compute_pgm_rsrc3 =
      SET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_TRAP_ON_END);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC3_ImageOp() const {
  assert(isGfx10Plus());
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  return GET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_IMAGE_OP);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC3_ImageOp(bool value) {
  assert(isGfx11());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC3_GFX10_PLUS_IMAGE_OP);
  kdRepr.compute_pgm_rsrc3 = SET_VALUE(COMPUTE_PGM_RSRC3_GFX10_PLUS_IMAGE_OP);
}
//
// GFX10, GFX11 end

bool KernelDescriptor::verifyCOMPUTE_PGM_RSRC3() const {
  // reserved and 0 for GFX6-9
  if (!isGfx90aOr940() && (!isGfx10Plus())) {
    if (kdRepr.compute_pgm_rsrc3 != 0)
      return false;
  }

  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc3;
  if (isGfx90aOr940()) {
    // next 10 bits after ACCUM_OFFSET are reserved and must be 0
    for (int i = 1; i <= 10; ++i) {
      bool bit = GET_ITH_BIT_AFTER(COMPUTE_PGM_RSRC3_GFX90A_ACCUM_OFFSET, i);
      if (bit)
        return false;
    }

    // next 15 bits after TG_SPLIT are reserved and must be 0
    for (int i = 1; i <= 15; ++i) {
      bool bit = GET_ITH_BIT_AFTER(COMPUTE_PGM_RSRC3_GFX90A_TG_SPLIT, i);
      if (bit)
        return false;
    }
  }

  if (isGfx10Plus()) {
    uint32_t instPrefSize = getCOMPUTE_PGM_RSRC3_InstPrefSize();
    if (isGfx10() && instPrefSize != 0)
      return false;

    // These are reserved and 0 for GFX10 and must be 0 for GFX11 as CP fills in
    // the value
    if (getCOMPUTE_PGM_RSRC3_TrapOnStart() != 0)
      return false;

    if (getCOMPUTE_PGM_RSRC3_TrapOnEnd() != 0)
      return false;

    // next 19 bits are reserved and must be 0
    for (int i = 1; i <= 19; ++i) {
      bool bit = GET_ITH_BIT_AFTER(COMPUTE_PGM_RSRC3_GFX10_PLUS_TRAP_ON_END, i);
      if (bit)
        return false;
    }

    bool imageOp = getCOMPUTE_PGM_RSRC3_ImageOp();
    if (isGfx10() && imageOp != 0)
      return false;
  }

  return true;
}
//
// ----- COMPUTE_PGM_RSRC3 end -----

// ----- COMPUTE_PGM_RSRC1 begin -----
//
//
uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC1() const {
  return kdRepr.compute_pgm_rsrc1;
}

uint32_t
KernelDescriptor::getCOMPUTE_PGM_RSRC1_GranulatedWorkitemVgprCount() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_GRANULATED_WORKITEM_VGPR_COUNT);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_GranulatedWorkitemVgprCount(
    uint32_t value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_GRANULATED_WORKITEM_VGPR_COUNT);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC1_GRANULATED_WORKITEM_VGPR_COUNT) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC1_GRANULATED_WORKITEM_VGPR_COUNT);
}

uint32_t
KernelDescriptor::getCOMPUTE_PGM_RSRC1_GranulatedWavefrontSgprCount() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_GRANULATED_WAVEFRONT_SGPR_COUNT);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_GranulatedWavefrontSgprCount(
    uint32_t value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer =
      CLEAR_BITS(COMPUTE_PGM_RSRC1_GRANULATED_WAVEFRONT_SGPR_COUNT);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC1_GRANULATED_WAVEFRONT_SGPR_COUNT) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC1_GRANULATED_WAVEFRONT_SGPR_COUNT);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC1_Priority() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_PRIORITY);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC1_FloatRoundMode32() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_32);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_FloatRoundMode32(uint32_t value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_32);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_32) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_32);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC1_FloatRoundMode1664() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_16_64);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_FloatRoundMode1664(uint32_t value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_16_64);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_16_64) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_16_64);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC1_FloatDenormMode32() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_FLOAT_DENORM_MODE_32);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_FloatDenormMode32(uint32_t value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_FLOAT_DENORM_MODE_32);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC1_FLOAT_DENORM_MODE_32) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC1_FLOAT_DENORM_MODE_32);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC1_FloatDenormMode1664() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_FLOAT_DENORM_MODE_16_64);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_FloatDenormMode1664(
    uint32_t value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_FLOAT_ROUND_MODE_16_64);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC1_FLOAT_DENORM_MODE_16_64) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC1_FLOAT_DENORM_MODE_16_64);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_Priv() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_PRIORITY);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_EnableDx10Clamp() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_ENABLE_DX10_CLAMP);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_EnableDx10Clamp(bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_ENABLE_DX10_CLAMP);
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC1_ENABLE_DX10_CLAMP);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_DebugMode() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_DEBUG_MODE);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_EnableIeeeMode() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_ENABLE_IEEE_MODE);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_EnableIeeeMode(bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_ENABLE_IEEE_MODE);
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC1_ENABLE_IEEE_MODE);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_Bulky() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_BULKY);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_CdbgUser() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_CDBG_USER);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_Fp16Ovfl() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_FP16_OVFL);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_Fp16Ovfl(bool value) {
  assert(isGfx9Plus());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_FP16_OVFL);
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC1_FP16_OVFL);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_WgpMode() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_WGP_MODE);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_WgpMode(bool value) {
  assert(isGfx10Plus());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_WGP_MODE);
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC1_WGP_MODE);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_MemOrdered() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_MEM_ORDERED);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_MemOrdered(bool value) {
  assert(isGfx10Plus());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_MEM_ORDERED);
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC1_MEM_ORDERED);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC1_FwdProgress() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  return GET_VALUE(COMPUTE_PGM_RSRC1_FWD_PROGRESS);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC1_FwdProgress(bool value) {
  assert(isGfx10Plus());
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC1_FWD_PROGRESS);
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC1_FWD_PROGRESS);
}

bool KernelDescriptor::verifyCOMPUTE_PGM_RSRC1() const {
  // PRIORITY must be 0 as CP is responsible for filling it
  uint32_t priority = getCOMPUTE_PGM_RSRC1_Priority();
  if (priority != 0)
    return false;

  // PRIV must be 0 as CP is responsible for filling it
  bool priv = getCOMPUTE_PGM_RSRC1_Priv();
  if (priv != 0)
    return false;

  // DEBUG_MODE must be 0 as CP is responsible for filling it
  bool debugMode = getCOMPUTE_PGM_RSRC1_DebugMode();
  if (debugMode != 0)
    return false;

  // BULKY must be 0 as CP is responsible for filling it
  bool bulky = getCOMPUTE_PGM_RSRC1_Bulky();
  if (bulky != 0)
    return false;

  // CDBG_USER must be 0 as CP is responsible for filling it
  bool cdbgUser = getCOMPUTE_PGM_RSRC1_CdbgUser();
  if (cdbgUser != 0)
    return false;

  // FP16_OVFL is reserved and must be 0 for GFX6-8
  bool fp16Ovfl = getCOMPUTE_PGM_RSRC1_Fp16Ovfl();
  if ((isGfx6() || isGfx7() || isGfx8()) && fp16Ovfl != 0)
    return false;

  // next 2 bits are reserved and must be 0
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc1;
  for (int i = 1; i <= 2; ++i) {
    bool bit = GET_ITH_BIT_AFTER(COMPUTE_PGM_RSRC1_FP16_OVFL, i);
    if (bit)
      return false;
  }

  // WGP_MODE is reserved and must be 0 for GFX6-9
  bool wgpMode = getCOMPUTE_PGM_RSRC1_WgpMode();
  if (!isGfx10Plus() && wgpMode != 0)
    return false;

  // MEM_ORDERED is reserved and must be 0 for GFX6-9
  bool memOrdered = getCOMPUTE_PGM_RSRC1_MemOrdered();
  if (!isGfx10Plus() && memOrdered != 0)
    return false;

  // FWD_PROGRESS is reserved and must be 0 for GFX6-9
  bool fwdProgress = getCOMPUTE_PGM_RSRC1_FwdProgress();
  if (!isGfx10Plus() && fwdProgress != 0)
    return false;

  return true;
}
//
// ----- COMPUTE_PGM_RSRC1 end -----

// ----- COMPUTE_PGM_RSRC2 begin -----
//
uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC2() const {
  return kdRepr.compute_pgm_rsrc2;
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnablePrivateSegment() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_PRIVATE_SEGMENT);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnablePrivateSegment(bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_PRIVATE_SEGMENT);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_PRIVATE_SEGMENT);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC2_UserSgprCount() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_USER_SGPR_COUNT);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_UserSgprCount(uint32_t value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC2_USER_SGPR_COUNT);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC2_USER_SGPR_COUNT) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(COMPUTE_PGM_RSRC2_USER_SGPR_COUNT);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableTrapHandler() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_TRAP_HANDLER);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdX() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_X);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdX(bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_X);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_X);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdY() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Y);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdY(bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Y);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Y);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdZ() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Z);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdZ(bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Z);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Z);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupInfo() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_INFO);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupInfo(
    bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_INFO);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_INFO);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableVgprWorkitemId() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_VGPR_WORKITEM_ID);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableVgprWorkitemId(
    uint32_t value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_VGPR_WORKITEM_ID);
  assert(CHECK_WIDTH(COMPUTE_PGM_RSRC2_ENABLE_VGPR_WORKITEM_ID) &&
         "value contains more bits than specificied");
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_VGPR_WORKITEM_ID);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableExceptionAddressWatch()
    const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_ADDRESS_WATCH);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableExceptionMemory() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_MEMORY);
}

uint32_t KernelDescriptor::getCOMPUTE_PGM_RSRC2_GranulatedLdsSize() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_GRANULATED_LDS_SIZE);
}

bool KernelDescriptor::
    getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInvalidOperation() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(
      COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_INVALID_OPERATION);
}

void KernelDescriptor::
    getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInvalidOperation(bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(
      COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_INVALID_OPERATION);
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(
      COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_INVALID_OPERATION);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableExceptionFpDenormalSource()
    const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_FP_DENORMAL_SOURCE);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableExceptionFpDenormalSource(
    bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer =
      CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_FP_DENORMAL_SOURCE);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_FP_DENORMAL_SOURCE);
}

bool KernelDescriptor::
    getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpDivisionByZero() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(
      COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_DIVISION_BY_ZERO);
}

void KernelDescriptor::
    setCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpDivisionByZero(bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer = CLEAR_BITS(
      COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_DIVISION_BY_ZERO);
  kdRepr.compute_pgm_rsrc1 = SET_VALUE(
      COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_DIVISION_BY_ZERO);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpOverflow()
    const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_OVERFLOW);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpOverflow(
    bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer =
      CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_OVERFLOW);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_OVERFLOW);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpUnderflow()
    const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_UNDERFLOW);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpUnderflow(
    bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer =
      CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_UNDERFLOW);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_UNDERFLOW);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInexact()
    const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_INEXACT);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInexact(
    bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer =
      CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_INEXACT);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_IEEE_754_FP_INEXACT);
}

bool KernelDescriptor::getCOMPUTE_PGM_RSRC2_EnableExceptionIntDivideByZero()
    const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  return GET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_INT_DIVIDE_BY_ZERO);
}

void KernelDescriptor::setCOMPUTE_PGM_RSRC2_EnableExceptionIntDivideByZero(
    bool value) {
  uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;
  fourByteBuffer =
      CLEAR_BITS(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_INT_DIVIDE_BY_ZERO);
  kdRepr.compute_pgm_rsrc1 =
      SET_VALUE(COMPUTE_PGM_RSRC2_ENABLE_EXCEPTION_INT_DIVIDE_BY_ZERO);
}

bool KernelDescriptor::verifyCOMPUTE_PGM_RSRC2() const {
  const uint32_t fourByteBuffer = kdRepr.compute_pgm_rsrc2;

  // ENABLE_TRAP_HANDLER must be 0 as CP is responsible for filling it
  bool enableTrapHandler = getCOMPUTE_PGM_RSRC2_EnableTrapHandler();
  if (enableTrapHandler != 0)
    return false;

  // ENABLE_EXCEPTION_ADDRESS_WATCH must be 0 as CP is responsible for filling
  // it
  bool enableExceptionAddressWatch =
      getCOMPUTE_PGM_RSRC2_EnableExceptionAddressWatch();
  if (enableExceptionAddressWatch != 0)
    return false;

  // ENABLE_EXCEPTION_MEMORY must be 0 as CP is responsible for filling it
  bool enableExceptionMemory = getCOMPUTE_PGM_RSRC2_EnableExceptionMemory();
  if (enableExceptionMemory != 0)
    return false;

  // ENABLE_GRANULATED_LDS_SIZE must be 0 as CP is responsible for filling it
  bool granulatedLdsSize = getCOMPUTE_PGM_RSRC2_GranulatedLdsSize();
  if (granulatedLdsSize != 0)
    return false;

  // the last bit is reserved and must be 0
  if ((fourByteBuffer & 1) != 0)
    return false;

  return true;
}
//
// ----- COMPUTE_PGM_RSRC2 end -----
//

#undef GET_VALUE
#undef SET_VALUE
#undef CLEAR_BITS
#undef CHECK_WIDTH
#undef GET_ITH_BIT_AFTER

#define GET_VALUE(MASK) ((twoByteBuffer & (MASK)) >> (MASK##_SHIFT))
#define SET_VALUE(MASK) (twoByteBuffer | ((value) << (MASK##_SHIFT)))
#define CLEAR_BITS(MASK) (twoByteBuffer & (~(MASK)))
#define CHECK_WIDTH(MASK) ((value) >> (MASK##_WIDTH) == 0)

#define GET_ITH_BIT_AFTER(MASK, i)                                             \
  (((twoByteBuffer & (1 << ((MASK##_WIDTH) + i - 1))) << (MASK##_SHIFT)) != 0)

// ----- KERNEL_CODE_PROPERTIES begin -----
//
bool KernelDescriptor::getKernelCodeProperty_EnableSgprPrivateSegmentBuffer()
    const {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_BUFFER);
}

void KernelDescriptor::setKernelCodeProperty_EnableSgprPrivateSegmentBuffer(
    bool value) {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer =
      CLEAR_BITS(KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_BUFFER);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_BUFFER);
}

bool KernelDescriptor::getKernelCodeProperty_EnableSgprDispatchPtr() const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_PTR);
}

void KernelDescriptor::setKernelCodeProperty_EnableSgprDispatchPtr(bool value) {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer = CLEAR_BITS(KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_PTR);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_PTR);
}

bool KernelDescriptor::getKernelCodeProperty_EnableSgprQueuePtr() const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_QUEUE_PTR);
}

void KernelDescriptor::setKernelCodeProperty_EnableSgprQueuePtr(bool value) {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer = CLEAR_BITS(KERNEL_CODE_PROPERTY_ENABLE_SGPR_QUEUE_PTR);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_QUEUE_PTR);
}

bool KernelDescriptor::getKernelCodeProperty_EnableSgprKernargSegmentPtr()
    const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_KERNARG_SEGMENT_PTR);
}

void KernelDescriptor ::setKernelCodeProperty_EnableSgprKernargSegmentPtr(
    bool value) {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer =
      CLEAR_BITS(KERNEL_CODE_PROPERTY_ENABLE_SGPR_KERNARG_SEGMENT_PTR);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_KERNARG_SEGMENT_PTR);
}

bool KernelDescriptor::getKernelCodeProperty_EnableSgprDispatchId() const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_ID);
}

void KernelDescriptor::setKernelCodeProperty_EnableSgprDispatchId(bool value) {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer = CLEAR_BITS(KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_ID);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_ID);
}

bool KernelDescriptor::getKernelCodeProperty_EnableSgprFlatScratchInit() const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_FLAT_SCRATCH_INIT);
}

void KernelDescriptor::setKernelCodeProperty_EnableSgprFlatScratchInit(
    bool value) {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer =
      CLEAR_BITS(KERNEL_CODE_PROPERTY_ENABLE_SGPR_FLAT_SCRATCH_INIT);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_FLAT_SCRATCH_INIT);
}

bool KernelDescriptor::getKernelCodeProperty_EnablePrivateSegmentSize() const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_SIZE);
}

void KernelDescriptor::setKernelCodeProperty_EnablePrivateSegmentSize(
    bool value) {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer =
      CLEAR_BITS(KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_SIZE);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_SIZE);
}

bool KernelDescriptor::getKernelCodeProperty_EnableWavefrontSize32() const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_WAVEFRONT_SIZE32);
}

void KernelDescriptor::setKernelCodeProperty_EnableWavefrontSize32(bool value) {
  assert(isGfx10Plus());
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer = CLEAR_BITS(KERNEL_CODE_PROPERTY_ENABLE_WAVEFRONT_SIZE32);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_ENABLE_WAVEFRONT_SIZE32);
}

bool KernelDescriptor::getKernelCodeProperty_UsesDynamicStack() const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  return GET_VALUE(KERNEL_CODE_PROPERTY_USES_DYNAMIC_STACK);
}

void KernelDescriptor::setKernelCodeProperty_UsesDynamicStack(bool value) {
  uint16_t twoByteBuffer = kdRepr.kernel_code_properties;
  twoByteBuffer = CLEAR_BITS(KERNEL_CODE_PROPERTY_USES_DYNAMIC_STACK);
  kdRepr.kernel_code_properties =
      SET_VALUE(KERNEL_CODE_PROPERTY_USES_DYNAMIC_STACK);
}

bool KernelDescriptor::supportsArchitectedFlatScratch() const {
  switch (amdgpuMach) {
  case EF_AMDGPU_MACH_AMDGCN_GFX940:
  case EF_AMDGPU_MACH_AMDGCN_GFX1100:
  case EF_AMDGPU_MACH_AMDGCN_GFX1101:
  case EF_AMDGPU_MACH_AMDGCN_GFX1102:
  case EF_AMDGPU_MACH_AMDGCN_GFX1103:
    return true;
  default:
    return false;
  }
}

bool KernelDescriptor::verifyKernelCodeProperties() const {
  const uint16_t twoByteBuffer = kdRepr.kernel_code_properties;

  if (supportsArchitectedFlatScratch()) {
    if (getKernelCodeProperty_EnableSgprPrivateSegmentBuffer())
      return false;

    if (getKernelCodeProperty_EnableSgprFlatScratchInit())
      return false;
  }

  // next 3 bits after ENABLE_SGPR_PRIVATE_SEGMENT_SIZE are reserved and must be
  // 0
  for (int i = 1; i <= 3; ++i) {
    bool bit = GET_ITH_BIT_AFTER(
        KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_SIZE, i);
    if (bit)
      return false;
  }

  bool enableWavefronSize32 = getKernelCodeProperty_EnableWavefrontSize32();
  if (!isGfx10Plus() && enableWavefronSize32)
    return false;

  // next 4 bits after USES_DYNAMIC_STACK must be 0
  for (int i = 1; i <= 4; ++i) {
    bool bit = GET_ITH_BIT_AFTER(KERNEL_CODE_PROPERTY_USES_DYNAMIC_STACK, i);
    if (bit)
      return false;
  }

  return true;
}
//
// ----- KERNEL_CODE_PROPERTIES end -----
//

#undef GET_VALUE
#undef SET_VALUE
#undef CLEAR_BITS
#undef CHECK_WIDTH
#undef GET_ITH_BIT_AFTER

void KernelDescriptor::dump(std::ostream &os) const {
  os << name << '\n';
  os << std::hex;
  os << "GROUP_SEGMENT_FIXED_SIZE : "
     << "0x" << kdRepr.group_segment_fixed_size << '\n';
  os << "PRIVATE_SEGMENT_FIXED_SIZE : "
     << "0x" << kdRepr.private_segment_fixed_size << '\n';
  os << "KERNARG_SIZE : "
     << "0x" << kdRepr.kernarg_size << '\n';
  os << "KERNEL_CODE_ENTRY_BYTE_OFFSET : "
     << "0x" << kdRepr.kernel_code_entry_byte_offset << '\n';
  os << "COMPUTE_PGM_RSRC3 : "
     << "0x" << kdRepr.compute_pgm_rsrc3 << '\n';
  os << "COMPUTE_PGM_RSRC1 : "
     << "0x" << kdRepr.compute_pgm_rsrc1 << '\n';
  os << "COMPUTE_PGM_RSRC2 : "
     << "0x" << kdRepr.compute_pgm_rsrc2 << '\n';
  os << "KERNEL_CODE_PROPERTIES : " << kdRepr.kernel_code_properties << '\n';
  os << std::dec;
}

void KernelDescriptor::dumpDetailed(std::ostream &os) const {
  const char *space = "    ";
  const char *indent = "  ";

  os << "----- detailed dump for " << name << "  begin ---- \n\n";

  os << indent << "-- GROUP_SEGMENT_FIXED_SIZE : " << std::hex << "0x"
     << kdRepr.group_segment_fixed_size << space << std::dec
     << kdRepr.group_segment_fixed_size << '\n'
     << '\n';

  os << indent << "-- PRIVATE_SEGMENT_FIXED_SIZE : " << std::hex << "0x"
     << kdRepr.private_segment_fixed_size << space << std::dec
     << kdRepr.private_segment_fixed_size << '\n'
     << '\n';

  os << indent << "-- KERNARG_SIZE : " << std::hex << "0x"
     << kdRepr.kernarg_size << space << std::dec << kdRepr.kernarg_size << '\n'
     << '\n';

  os << indent << "-- KERNEL_CODE_ENTRY_BYTE_OFFSET : " << std::hex << "0x"
     << kdRepr.kernel_code_entry_byte_offset << space << std::dec
     << kdRepr.kernel_code_entry_byte_offset << '\n'
     << '\n';

  dumpCOMPUTE_PGM_RSRC3(os);
  os << '\n';

  dumpCOMPUTE_PGM_RSRC1(os);
  os << '\n';

  dumpCOMPUTE_PGM_RSRC2(os);
  os << '\n';

  dumpKernelCodeProperties(os);
  os << '\n';

  os << "----- detailed dump for " << name << "  end ---- \n";
}

void KernelDescriptor::dumpCOMPUTE_PGM_RSRC3(std::ostream &os) const {
  const char *indent = "    ";
  os << "  -- COMPUTE_PGM_RSRC3 begin\n";

  if (isGfx90aOr940())
    dumpCOMPUTE_PGM_RSRC3_Gfx90aOr940(os);

  else if (isGfx10Plus())
    dumpCOMPUTE_PGM_RSRC3_Gfx10Plus(os);

  else
    os << indent << getCOMPUTE_PGM_RSRC3() << '\n';

  os << "  -- COMPUTE_PGM_RSRC3 end\n";
}

void KernelDescriptor::dumpCOMPUTE_PGM_RSRC3_Gfx90aOr940(
    std::ostream &os) const {
  assert(isGfx90aOr940());
  const char *indent = "    ";

  os << indent << "ACCUM_OFFSET : " << getCOMPUTE_PGM_RSRC3_AccumOffset()
     << '\n';

  os << indent << "TG_SPLIT : " << getCOMPUTE_PGM_RSRC3_TgSplit() << '\n';
}

void KernelDescriptor::dumpCOMPUTE_PGM_RSRC3_Gfx10Plus(std::ostream &os) const {
  assert(isGfx10Plus());
  const char *indent = "    ";

  os << indent
     << "SHARED_VGPR_COUNT : " << getCOMPUTE_PGM_RSRC3_SharedVgprCount()
     << '\n';

  os << indent << "INST_PREF_SIZE : " << getCOMPUTE_PGM_RSRC3_InstPrefSize()
     << '\n';

  os << indent << "TRAP_ON_START : " << getCOMPUTE_PGM_RSRC3_TrapOnStart()
     << '\n';

  os << indent << "TRAP_ON_END : " << getCOMPUTE_PGM_RSRC3_TrapOnEnd() << '\n';
  os << indent << "IMAGE_OP : " << getCOMPUTE_PGM_RSRC3_ImageOp() << '\n';
}

void KernelDescriptor::dumpCOMPUTE_PGM_RSRC1(std::ostream &os) const {
  const char *indent = "    ";
  os << "  -- COMPUTE_PGM_RSRC1 begin\n";

  os << indent << "GRANULATED_WORKITEM_VGPR_COUNT : "
     << getCOMPUTE_PGM_RSRC1_GranulatedWorkitemVgprCount() << '\n';

  os << indent << "GRANULATED_WAVEFRONT_SGPR_COUNT : "
     << getCOMPUTE_PGM_RSRC1_GranulatedWavefrontSgprCount() << '\n';

  os << indent << "PRIORITY : " << getCOMPUTE_PGM_RSRC1_Priority() << '\n';

  os << indent
     << "FLOAT_ROUND_MODE_32 : " << getCOMPUTE_PGM_RSRC1_FloatRoundMode32()
     << '\n';

  os << indent
     << "FLOAT_ROUND_MODE_16_64 : " << getCOMPUTE_PGM_RSRC1_FloatRoundMode1664()
     << '\n';

  os << indent
     << "FLOAT_DENORM_MODE_32 : " << getCOMPUTE_PGM_RSRC1_FloatDenormMode32()
     << '\n';

  os << indent << "FLOAT_DENORM_MODE_16_64 : "
     << getCOMPUTE_PGM_RSRC1_FloatDenormMode1664() << '\n';

  os << indent << "PRIV : " << getCOMPUTE_PGM_RSRC1_Priv() << '\n';

  os << indent
     << "ENABLE_DX10_CLAMP : " << getCOMPUTE_PGM_RSRC1_EnableDx10Clamp()
     << '\n';

  os << indent << "ENABLE_IEEE_MODE : " << getCOMPUTE_PGM_RSRC1_EnableIeeeMode()
     << '\n';

  os << indent << "BULKY : " << getCOMPUTE_PGM_RSRC1_Bulky() << '\n';

  os << indent << "CDBG_USER : " << getCOMPUTE_PGM_RSRC1_CdbgUser() << '\n';

  os << indent << "FP16_OVFL : " << getCOMPUTE_PGM_RSRC1_Fp16Ovfl() << '\n';

  os << indent << "WGP_MODE : " << getCOMPUTE_PGM_RSRC1_WgpMode() << '\n';

  os << indent << "MEM_ORDERED : " << getCOMPUTE_PGM_RSRC1_MemOrdered() << '\n';

  os << indent << "FWD_PROGRESS : " << getCOMPUTE_PGM_RSRC1_FwdProgress()
     << '\n';

  os << "  -- COMPUTE_PGM_RSRC1 end\n";
}

void KernelDescriptor::dumpCOMPUTE_PGM_RSRC2(std::ostream &os) const {
  const char *indent = "    ";
  os << "  -- COMPUTE_PGM_RSRC2 begin\n";

  os << indent << "ENABLE_PRIVATE_SEGMENT : "
     << getCOMPUTE_PGM_RSRC2_EnablePrivateSegment() << '\n';

  os << indent << "USER_SGPR_COUNT : " << getCOMPUTE_PGM_RSRC2_UserSgprCount()
     << '\n';

  os << indent
     << "ENABLE_TRAP_HANDLER : " << getCOMPUTE_PGM_RSRC2_EnableTrapHandler()
     << '\n';

  os << indent << "ENABLE_SGPR_WORKGROUP_ID_X : "
     << getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdX() << '\n';

  os << indent << "ENABLE_SGPR_WORKGROUP_ID_Y : "
     << getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdY() << '\n';

  os << indent << "ENABLE_SGPR_WORKGROUP_ID_Z : "
     << getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdZ() << '\n';

  os << indent << "ENABLE_SGPR_WORKGROUP_INFO : "
     << getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupInfo() << '\n';

  os << indent << "ENABLE_VGPR_WORKITEM_ID : "
     << getCOMPUTE_PGM_RSRC2_EnableVgprWorkitemId() << '\n';

  os << indent << "ENABLE_EXCEPTION_ADDRESS_WATCH : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionAddressWatch() << '\n';

  os << indent << "ENABLE_EXCEPTION_MEMORY : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionMemory() << '\n';

  os << indent
     << "GRANULATED_LDS_SIZE : " << getCOMPUTE_PGM_RSRC2_GranulatedLdsSize()
     << '\n';

  os << indent << "ENABLE_EXCEPTION_IEEE_754_FP_INVALID_OPERATION : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInvalidOperation() << '\n';

  os << indent << "ENABLE_EXCEPTION_FP_DENORMAL_SOURCE : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionFpDenormalSource() << '\n';

  os << indent << "ENABLE_EXCEPTION_IEEE_754_FP_DIVISION_BY_ZERO : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpDivisionByZero() << '\n';

  os << indent << "ENABLE_EXCEPTION_IEEE_754_FP_OVERFLOW : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpOverflow() << '\n';

  os << indent << "ENABLE_EXCEPTION_IEEE_754_FP_UNDERFLOW : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpUnderflow() << '\n';

  os << indent << "ENABLE_EXCEPTION_IEEE_754_FP_INEXACT : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionIeee754FpInexact() << '\n';

  os << indent << "ENABLE_EXCEPTION_INT_DIVIDE_BY_ZERO : "
     << getCOMPUTE_PGM_RSRC2_EnableExceptionIntDivideByZero() << '\n';

  os << "  -- COMPUTE_PGM_RSRC2 end\n";
}

void KernelDescriptor::dumpKernelCodeProperties(std::ostream &os) const {
  const char *indent = "    ";
  os << "  -- Kernel code properties begin\n";

  os << indent << "ENABLE_SGPR_PRIVATE_SEGMENT_BUFFER : "
     << getKernelCodeProperty_EnableSgprPrivateSegmentBuffer() << '\n';

  os << indent << "ENABLE_SGPR_DISPATCH_PTR : "
     << getKernelCodeProperty_EnableSgprDispatchPtr() << '\n';

  os << indent
     << "ENABLE_SGPR_QUEUE_PTR : " << getKernelCodeProperty_EnableSgprQueuePtr()
     << '\n';

  os << indent << "ENABLE_SGPR_KERNARG_SEGMENT_PTR : "
     << getKernelCodeProperty_EnableSgprKernargSegmentPtr() << '\n';

  os << indent << "ENABLE_SGPR_DISPATCH_ID : "
     << getKernelCodeProperty_EnableSgprDispatchId() << '\n';

  os << indent << "ENABLE_SGPR_FLAT_SCRATCH_INIT : "
     << getKernelCodeProperty_EnableSgprFlatScratchInit() << '\n';

  os << indent << "ENABLE_PRIVATE_SEGMENT_SIZE : "
     << getKernelCodeProperty_EnablePrivateSegmentSize() << '\n';

  os << indent << "ENABLE_WAVEFRONT_SIZE_32 : "
     << getKernelCodeProperty_EnableWavefrontSize32() << '\n';

  os << indent
     << "USES_DYNAMIC_STACK : " << getKernelCodeProperty_UsesDynamicStack()
     << '\n';

  os << "  -- Kernel code properties end\n";
}
