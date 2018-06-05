/**
 * sippak -- SIP command line utility.
 * Copyright (C) 2018, Stas Kobzar <staskobzar@modulis.ca>
 *
 * This file is part of sippak.
 *
 * sippak is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sippak is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sippak.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file sip_helper.c
 * @brief sippak helper for media and SDP management
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <pjmedia.h>
#include <pjmedia-codec.h>
#include "sippak.h"

#define NAME "media_helper"

pj_bool_t sippak_support_codec (codec_e codec)
{
  int supported = 0;

#if PJMEDIA_HAS_SPEEX_CODEC
  supported |= SIPPAK_CODEC_SPEEX;
#endif
#if PJMEDIA_HAS_ILBC_CODEC
  supported |= SIPPAK_CODEC_ILBC;
#endif
#if PJMEDIA_HAS_GSM_CODEC
  supported |= SIPPAK_CODEC_GSM;
#endif
#if PJMEDIA_HAS_G711_CODEC
  supported |= SIPPAK_CODEC_G711;
#endif
#if PJMEDIA_HAS_G722_CODEC
  supported |= SIPPAK_CODEC_G722;
#endif
#if PJMEDIA_HAS_INTEL_IPP
  supported |= SIPPAK_CODEC_IPP;
#endif
#if PJMEDIA_HAS_G7221_CODEC
  supported |= SIPPAK_CODEC_G7221;
#endif
#if PJMEDIA_HAS_L16_CODEC
  supported |= SIPPAK_CODEC_L16;
#endif
#if PJMEDIA_HAS_OPENCORE_AMRNB_CODEC || PJMEDIA_HAS_OPENCORE_AMRWB_CODEC
  supported |= SIPPAK_CODEC_OCAMR;
#endif
#if PJMEDIA_HAS_SILK_CODEC
  supported |= SIPPAK_CODEC_SILK;
#endif
#if PJMEDIA_HAS_OPUS_CODEC
  supported |= SIPPAK_CODEC_OPUS;
#endif
#if PJMEDIA_HAS_BCG729
  supported |= SIPPAK_CODEC_BCG729;
#endif
  return (supported & codec) == codec ? PJ_TRUE : PJ_FALSE;
}

pj_status_t sippak_set_media_codecs_cfg (const char *codecs,
                                         struct sippak_app *app)
{
  pj_status_t status;
  app->cfg.media.cnt = 3;
  app->cfg.media.codec[0] = SIPPAK_CODEC_SILK;
  app->cfg.media.codec[1] = SIPPAK_CODEC_G711;
  app->cfg.media.codec[2] = SIPPAK_CODEC_SPEEX;

  return PJ_SUCCESS; // PJ_CLI_EINVARG
}
