#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

#ifndef PJMEDIA_HAS_SPEEX_CODEC
  #define PJMEDIA_HAS_SPEEX_CODEC
#endif
#ifndef PJMEDIA_HAS_G711_CODEC
  #define PJMEDIA_HAS_G711_CODEC
#endif
#ifdef PJMEDIA_HAS_BCG729
  #undef PJMEDIA_HAS_BCG729
#endif
#ifdef PJMEDIA_HAS_INTEL_IPP
  #undef PJMEDIA_HAS_INTEL_IPP
#endif
#ifndef PJMEDIA_HAS_G722_CODEC
  #define PJMEDIA_HAS_G722_CODEC
#endif
#ifndef PJMEDIA_HAS_SILK_CODEC
  #define PJMEDIA_HAS_SILK_CODEC
#endif

static void support_media_codec (void **state)
{
  (void) *state;
  assert_int_equal(PJ_TRUE, sippak_support_codec(SIPPAK_CODEC_SPEEX));
  assert_int_equal(PJ_FALSE, sippak_support_codec(SIPPAK_CODEC_BCG729));
  assert_int_equal(PJ_TRUE, sippak_support_codec(SIPPAK_CODEC_G711));
  assert_int_equal(PJ_FALSE, sippak_support_codec(SIPPAK_CODEC_IPP));
}

static void set_codecs_array_ordered (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;

  status = sippak_set_media_codecs_cfg ("silk,g711,speex", &app);
  assert_int_equal (PJ_SUCCESS, status);
  assert_int_equal (app.cfg.media.cnt, 3);
  assert_int_equal (app.cfg.media.codec[0], SIPPAK_CODEC_SILK);
  assert_int_equal (app.cfg.media.codec[1], SIPPAK_CODEC_G711);
  assert_int_equal (app.cfg.media.codec[2], SIPPAK_CODEC_SPEEX);
}

static void invalid_codec_value_given (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;

  status = sippak_set_media_codecs_cfg ("silk,foo,speex", &app);
  assert_int_equal (PJ_CLI_EINVARG, status);
}

static void set_use_all_codecs (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;

  status = sippak_set_media_codecs_cfg ("all", &app);
  assert_int_equal (PJ_SUCCESS, status);
  assert_int_equal (NUM_CODECS_AVAIL, app.cfg.media.cnt);
}

static void set_use_all_codecs_from_mix (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;

  status = sippak_set_media_codecs_cfg ("g711,all,silk", &app);
  assert_int_equal (PJ_SUCCESS, status);
  assert_int_equal (NUM_CODECS_AVAIL, app.cfg.media.cnt);
}

static void parse_all_codecs (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  // TODO: test
}

int main(int argc, const char *argv[])
{
  pj_log_set_level(0); // do not print pj debug on init
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(support_media_codec),
    cmocka_unit_test(set_codecs_array_ordered),
    cmocka_unit_test(invalid_codec_value_given),
    cmocka_unit_test(set_use_all_codecs),
    cmocka_unit_test(set_use_all_codecs_from_mix),
  };
  return cmocka_run_group_tests_name("Media helper", tests, NULL, NULL);
}
