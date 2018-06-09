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
#include <pjlib-util.h>
#include "sippak.h"

#define NAME "media_helper"
static codec_e codec_str_parse(pj_str_t *codec);
static pj_bool_t is_codec_set(codec_e *codecs, int cnt, codec_e codec);
static pj_status_t set_media_codecs(pjmedia_endpt *med_endpt,
    struct sippak_app *app, pjmedia_audio_codec_config *codec_cfg);

#if SIPPAK_UNIT_TESTS
/*
 * Forward declaration only for unit tests.
 * Stubs for pjmedia functions during unit tests.
 */
#ifndef PJMEDIA_HAS_SPEEX_CODEC
pj_status_t pjmedia_codec_speex_init_default(pjmedia_endpt *endpt) {return PJ_SUCCESS;}
#endif
pj_status_t pjmedia_codec_ilbc_init(pjmedia_endpt *endpt, int mode) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_gsm_init(pjmedia_endpt *endpt) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_g711_init(pjmedia_endpt *endpt) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_g722_init(pjmedia_endpt *endpt) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_ipp_init(pjmedia_endpt *endpt) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_g7221_init(pjmedia_endpt *endpt) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_l16_init(pjmedia_endpt *endpt, unsigned opt) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_opencore_amr_init(pjmedia_endpt *endpt, unsigned opt) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_silk_init(pjmedia_endpt *endpt) {return PJ_SUCCESS;}
pj_status_t pjmedia_codec_opus_init(pjmedia_endpt *endpt) {return PJ_SUCCESS;}
pj_status_t	pjmedia_codec_bcg729_init (pjmedia_endpt *endpt) {return PJ_SUCCESS;}
#endif

static codec_e codec_str_parse(pj_str_t *codec)
{
  if (pj_strnicmp2(codec, "speex", 5) == 0) {
    return SIPPAK_CODEC_SPEEX;
  } else if(pj_strnicmp2(codec, "ilbc", 4) == 0) {
    return SIPPAK_CODEC_ILBC;
  } else if(pj_strnicmp2(codec, "gsm", 3) == 0) {
    return SIPPAK_CODEC_GSM;
  } else if(pj_strnicmp2(codec, "g711", 4) == 0) {
    return SIPPAK_CODEC_G711;
  } else if(pj_strnicmp2(codec, "g722", 4) == 0) {
    return SIPPAK_CODEC_G722;
  } else if(pj_strnicmp2(codec, "ipp", 3) == 0) {
    return SIPPAK_CODEC_IPP;
  } else if(pj_strnicmp2(codec, "g722.1", 6) == 0) {
    return SIPPAK_CODEC_G7221;
  } else if(pj_strnicmp2(codec, "l16", 3) == 0) {
    return SIPPAK_CODEC_L16;
  } else if(pj_strnicmp2(codec, "amr", 3) == 0) {
    return SIPPAK_CODEC_OCAMR;
  } else if(pj_strnicmp2(codec, "silk", 4) == 0) {
    return SIPPAK_CODEC_SILK;
  } else if(pj_strnicmp2(codec, "opus", 4) == 0) {
    return SIPPAK_CODEC_OPUS;
  } else if(pj_strnicmp2(codec, "bcg729", 6) == 0) {
    return SIPPAK_CODEC_BCG729;
  } else if(pj_strnicmp2(codec, "all", 3) == 0) {
    return SIPPAK_CODEC_ALL;
  } else {
    return 0;
  }
}

static pj_bool_t is_codec_set(codec_e *codecs, int cnt, codec_e codec)
{
  for(int i = 0; i < cnt; i++) {
    if (codecs[i] == codec) {
      return PJ_TRUE;
    }
  }
  return PJ_FALSE;
}

static pj_status_t set_media_codecs(pjmedia_endpt *med_endpt,
                                    struct sippak_app *app,
                                    pjmedia_audio_codec_config *codec_cfg
                                    )
{
  pj_status_t status;

  for(int i = 0; i < app->cfg.media.cnt; i++) {
    switch(app->cfg.media.codec[i]) {
#if PJMEDIA_HAS_SPEEX_CODEC
      case SIPPAK_CODEC_SPEEX:
        status = pjmedia_codec_speex_init_default(med_endpt);
        break;
#endif
#if PJMEDIA_HAS_ILBC_CODEC
      case SIPPAK_CODEC_ILBC:
        status = pjmedia_codec_ilbc_init(med_endpt, codec_cfg->ilbc.mode);
        break;
#endif
#if PJMEDIA_HAS_GSM_CODEC
      case SIPPAK_CODEC_GSM:
        status = pjmedia_codec_gsm_init(med_endpt);
        break;
#endif
#if PJMEDIA_HAS_G711_CODEC
      case SIPPAK_CODEC_G711:
        status = pjmedia_codec_g711_init(med_endpt);
        break;
#endif
#if PJMEDIA_HAS_G722_CODEC
      case SIPPAK_CODEC_G722:
        status = pjmedia_codec_g722_init(med_endpt);
        break;
#endif
#if PJMEDIA_HAS_INTEL_IPP
      case SIPPAK_CODEC_IPP:
        status = pjmedia_codec_ipp_init(med_endpt);
        break;
#endif
#if PJMEDIA_HAS_G7221_CODEC
      case SIPPAK_CODEC_G7221:
        status = pjmedia_codec_g7221_init(med_endpt);
        break;
#endif
#if PJMEDIA_HAS_L16_CODEC
      case SIPPAK_CODEC_L16:
        status = pjmedia_codec_l16_init(med_endpt, 0);
        break;
#endif
#if PJMEDIA_HAS_OPENCORE_AMRNB_CODEC || PJMEDIA_HAS_OPENCORE_AMRWB_CODEC
      case SIPPAK_CODEC_OCAMR:
        status = pjmedia_codec_opencore_amr_init(med_endpt, 0);
        break;
#endif
#if PJMEDIA_HAS_SILK_CODEC
      case SIPPAK_CODEC_SILK:
        status = pjmedia_codec_silk_init(med_endpt);
        break;
#endif
#if PJMEDIA_HAS_OPUS_CODEC
      case SIPPAK_CODEC_OPUS:
        status = pjmedia_codec_opus_init(med_endpt);
        break;
#endif
#if PJMEDIA_HAS_BCG729
      case SIPPAK_CODEC_BCG729:
        status = pjmedia_codec_bcg729_init(med_endpt);
        break;
#endif
      default:
        PJ_LOG(1, (NAME, "Unknown codec with id '%d'", app->cfg.media.codec[i]));
        return PJ_EINVAL;
        break;
    }
    PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
  }
  return PJ_SUCCESS;
}

PJ_DEF(pj_bool_t) sippak_support_codec (codec_e codec)
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

PJ_DEF(pj_status_t) sippak_set_media_codecs_cfg (char *codecs_str,
                                         struct sippak_app *app)
{
  pj_status_t status;
  pj_ssize_t found_idx = 0;
  pj_str_t token;
  pj_str_t delim = pj_str(",");
  pj_str_t codecs = pj_str(codecs_str);
  codec_e codec;
  int i = 0;

  for (
      found_idx = pj_strtok (&codecs, &delim, &token, 0);
      found_idx != codecs.slen;
      found_idx = pj_strtok (&codecs, &delim, &token, found_idx + token.slen)
      )
  {
    codec = codec_str_parse(&token);
    if (codec == SIPPAK_CODEC_ALL) {
      app->cfg.media.cnt = NUM_CODECS_AVAIL;
      return PJ_SUCCESS;
    }
    if (codec == 0 || sippak_support_codec(codec) == PJ_FALSE) {
      PJ_LOG(1, (NAME, "Codec \"%.*s\" is not supported.", token.slen, token.ptr));
      return PJ_CLI_EINVARG;
    }
    if (is_codec_set(app->cfg.media.codec, i, codec)) {
      PJ_LOG(2, (NAME, "Codec \"%.*s\" is already set.", token.slen, token.ptr));
      continue;
    }
    app->cfg.media.codec[i++] = codec;
  }

  app->cfg.media.cnt = i;

  return PJ_SUCCESS;
}

PJ_DEF(pj_status_t) sippak_set_media_sdp (struct sippak_app *app,
                                pjmedia_sdp_session **sdp)
{
  pj_status_t status;
  pjmedia_sock_info sock_info;
  pjmedia_transport *med_transport;
  pjmedia_endpt *med_endpt;
  pjmedia_transport_info med_tpinfo;
  pjmedia_sdp_session *sdp_sess;
  pjmedia_audio_codec_config codec_cfg;

  pjmedia_audio_codec_config_default(&codec_cfg);

  status = pjmedia_endpt_create(&app->cp->factory, NULL, 1, &med_endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  if (app->cfg.media.cnt == NUM_CODECS_AVAIL) {
    status = pjmedia_codec_register_audio_codecs(med_endpt, &codec_cfg);
  } else {
    status = set_media_codecs(med_endpt, app, &codec_cfg);
  }
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjmedia_transport_udp_create(med_endpt,
      NAME,     // transport name
      app->cfg.media.rtp_port,    // rtp port
      0,        // options
      &med_transport);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pjmedia_transport_info_init(&med_tpinfo);

  status = pjmedia_transport_get_info(med_transport, &med_tpinfo);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pj_memcpy(&sock_info, &med_tpinfo.sock_info, sizeof(pjmedia_sock_info));

  status = pjmedia_endpt_create_sdp( med_endpt,
      app->pool,
      1, // # of streams
      &sock_info,  // rtp sock
      &sdp_sess); // local sdp
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  // session name (subject)
  sdp_sess->name = pj_str("sippak");

  *sdp = sdp_sess;
  return status;
}
