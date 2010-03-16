/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2010  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GRUB_BIDI_HEADER
#define GRUB_BIDI_HEADER	1

#include <grub/types.h>
#include <grub/mm.h>
#include <grub/misc.h>

struct grub_unicode_compact_range
{
  grub_uint32_t start:21;
  grub_uint32_t end:21;
  grub_uint8_t bidi_type:5;
  grub_uint8_t comb_type;
  grub_uint8_t bidi_mirror:1;
} __attribute__ ((packed));

enum grub_bidi_type
  {
    GRUB_BIDI_TYPE_L = 0,
    GRUB_BIDI_TYPE_LRE,
    GRUB_BIDI_TYPE_LRO,
    GRUB_BIDI_TYPE_R,
    GRUB_BIDI_TYPE_AL,
    GRUB_BIDI_TYPE_RLE,
    GRUB_BIDI_TYPE_RLO,
    GRUB_BIDI_TYPE_PDF,
    GRUB_BIDI_TYPE_EN,
    GRUB_BIDI_TYPE_ES,
    GRUB_BIDI_TYPE_ET,
    GRUB_BIDI_TYPE_AN,
    GRUB_BIDI_TYPE_CS,
    GRUB_BIDI_TYPE_NSM,
    GRUB_BIDI_TYPE_BN,
    GRUB_BIDI_TYPE_B,
    GRUB_BIDI_TYPE_S,
    GRUB_BIDI_TYPE_WS,
    GRUB_BIDI_TYPE_ON
  };

enum grub_comb_type
  {
    GRUB_UNICODE_COMB_NONE = 0,
    GRUB_UNICODE_COMB_OVERLAY = 1,
    GRUB_UNICODE_STACK_ATTACHED_BELOW = 202,
    GRUB_UNICODE_STACK_ATTACHED_ABOVE = 214,
    GRUB_UNICODE_COMB_ATTACHED_ABOVE_RIGHT = 216,
    GRUB_UNICODE_STACK_BELOW = 220,
    GRUB_UNICODE_STACK_ABOVE = 230,
    GRUB_UNICODE_COMB_ABOVE_RIGHT = 232,
    GRUB_UNICODE_COMB_YPOGEGRAMMENI = 240,
    /* If combining nature is indicated only by class and
       not "combining type".  */
    GRUB_UNICODE_COMB_ME = 253,
    GRUB_UNICODE_COMB_MC = 254,
    GRUB_UNICODE_COMB_MN = 255,
  };

/* This structure describes a glyph as opposed to character.  */
struct grub_unicode_glyph
{
  grub_uint32_t base;
  grub_uint16_t variant:9;
  grub_uint8_t attributes:1;
  grub_size_t ncomb;
  struct grub_unicode_combining {
    grub_uint32_t code;
    enum grub_comb_type type;
  } *combining;
};

#define GRUB_UNICODE_GLYPH_ATTRIBUTE_MIRROR 0x1
#define GRUB_UNICODE_COMBINING_GRAPHEME_JOINER 0x34f
#define GRUB_UNICODE_VARIATION_SELECTOR_1 0xfe00
#define GRUB_UNICODE_VARIATION_SELECTOR_16 0xfe0f
#define GRUB_UNICODE_VARIATION_SELECTOR_17 0xe0100
#define GRUB_UNICODE_VARIATION_SELECTOR_256 0xe01ef

extern struct grub_unicode_compact_range grub_unicode_compact[];

#define GRUB_UNICODE_MAX_CACHED_CHAR 0x20000
/*  Unicode mandates an arbitrary limit.  */
#define GRUB_BIDI_MAX_EXPLICIT_LEVEL 61

grub_ssize_t
grub_bidi_logical_to_visual (const grub_uint32_t *logical,
			     grub_size_t logical_len,
			     struct grub_unicode_glyph **visual_out,
			     grub_ssize_t (*getcharwidth) (const struct grub_unicode_glyph *visual),
			     grub_size_t max_width,
			     grub_size_t start_width);

enum grub_comb_type
grub_unicode_get_comb_type (grub_uint32_t c);
grub_size_t
grub_unicode_aglomerate_comb (const grub_uint32_t *in, grub_size_t inlen,
			      struct grub_unicode_glyph *out);

static inline struct grub_unicode_glyph *
grub_unicode_glyph_dup (const struct grub_unicode_glyph *in)
{
  struct grub_unicode_glyph *out = grub_malloc (sizeof (*out));
  if (!out)
    return NULL;
  grub_memcpy (out, in, sizeof (*in));
  out->combining = grub_malloc (in->ncomb * sizeof (*in));
  if (!out->combining)
    {
      grub_free (out);
      return NULL;
    }
  grub_memcpy (out->combining, in->combining, in->ncomb * sizeof (*in));
  return out;
}

static inline struct grub_unicode_glyph *
grub_unicode_glyph_from_code (grub_uint32_t code)
{
  struct grub_unicode_glyph *ret;
  ret = grub_malloc (sizeof (*ret));
  if (!ret)
    return NULL;

  ret->base = code;
  ret->variant = 0;
  ret->attributes = 0;
  ret->ncomb = 0;
  ret->combining = 0;

  return ret;
}

#endif
