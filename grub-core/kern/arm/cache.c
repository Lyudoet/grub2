#include <grub/dl.h>
#include <grub/cache.h>
#include <grub/arm/system.h>

static enum
  {
    ARCH_UNKNOWN,
    ARCH_ARMV6,
    ARCH_ARMV7
  } type = ARCH_UNKNOWN;

grub_uint32_t grub_arch_cache_dlinesz;
grub_uint32_t grub_arch_cache_ilinesz;

/* Prototypes for asm functions.  */
void grub_arch_sync_caches_armv6 (void *address, grub_size_t len);
void grub_arch_sync_caches_armv7 (void *address, grub_size_t len);
void grub_arm_disable_caches_mmu_armv6 (void);
void grub_arm_disable_caches_mmu_armv7 (void);

static void
probe_caches (void)
{
  grub_uint32_t main_id, cache_type;

  /* Read Cache Type Register */
  asm volatile ("mrc 	p15, 0, %0, c0, c0, 0": "=r"(main_id));

  if (((main_id >> 12) & 0xf) == 0x0 || ((main_id >> 12) & 0xf) == 0x7
      || (((main_id >> 16) & 0x7) != 0x7))
    grub_fatal ("Unsupported ARM ID 0x%x", main_id);

  /* Read Cache Type Register */
  asm volatile ("mrc 	p15, 0, %0, c0, c0, 1": "=r"(cache_type));

  switch (cache_type >> 29)
    {
    case 0:
      grub_arch_cache_dlinesz = 8 << ((cache_type >> 12) & 3);
      grub_arch_cache_ilinesz = 8 << (cache_type & 3);
      type = ARCH_ARMV6;
      break;
    case 4:
      grub_arch_cache_dlinesz = 4 << ((cache_type >> 16) & 0xf);
      grub_arch_cache_ilinesz = 4 << (cache_type & 0xf);
      type = ARCH_ARMV7;
    default:
      grub_fatal ("Unsupported cache type 0x%x", cache_type);
    }
}

void
grub_arch_sync_caches (void *address, grub_size_t len)
{
  if (type == ARCH_UNKNOWN)
    probe_caches ();
  if (type == ARCH_ARMV6)
    grub_arch_sync_caches_armv6 (address, len);
  if (type == ARCH_ARMV7)
    grub_arch_sync_caches_armv7 (address, len);
}

void
grub_arm_disable_caches_mmu (void)
{
  if (type == ARCH_UNKNOWN)
    probe_caches ();
  if (type == ARCH_ARMV6)
    grub_arm_disable_caches_mmu_armv6 ();
  if (type == ARCH_ARMV7)
    grub_arm_disable_caches_mmu_armv7 ();
}
