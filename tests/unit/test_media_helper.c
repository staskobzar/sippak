#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <pjlib-util.h>
#include "sippak.h"

static void support_media_codec (void **state)
{
  (void) *state;
  assert_int_equal(PJ_TRUE, sippak_support_codec(SIPPAK_CODEC_SPEEX));
  assert_int_equal(PJ_TRUE, sippak_support_codec(SIPPAK_CODEC_G711));
  assert_int_equal(PJ_FALSE, sippak_support_codec(SIPPAK_CODEC_G7221));
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

static void fail_on_duplicated_codec (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;

  status = sippak_set_media_codecs_cfg ("g711,opus,silk,opus,G711", &app);
  assert_int_equal (PJ_SUCCESS, status);
  assert_int_equal (app.cfg.media.cnt, 3);
  assert_int_equal (app.cfg.media.codec[0], SIPPAK_CODEC_G711);
  assert_int_equal (app.cfg.media.codec[1], SIPPAK_CODEC_OPUS);
  assert_int_equal (app.cfg.media.codec[2], SIPPAK_CODEC_SILK);
}

static void parse_all_codecs_icase (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *codecs = "speex,ILbc,gsm,G711,g722,IPP,l16,AMR,silk,opus,bcg729";
  app.cfg.media.cnt = 0;

  status = sippak_set_media_codecs_cfg (codecs, &app);
  assert_int_equal (PJ_SUCCESS, status);
  assert_int_equal (app.cfg.media.cnt, 11);
  assert_int_equal (app.cfg.media.codec[0], SIPPAK_CODEC_SPEEX);
  assert_int_equal (app.cfg.media.codec[1], SIPPAK_CODEC_ILBC);
  assert_int_equal (app.cfg.media.codec[2], SIPPAK_CODEC_GSM);
  assert_int_equal (app.cfg.media.codec[3], SIPPAK_CODEC_G711);
  assert_int_equal (app.cfg.media.codec[4], SIPPAK_CODEC_G722);
  assert_int_equal (app.cfg.media.codec[5], SIPPAK_CODEC_IPP);
  assert_int_equal (app.cfg.media.codec[6], SIPPAK_CODEC_L16);
  assert_int_equal (app.cfg.media.codec[7], SIPPAK_CODEC_OCAMR);
  assert_int_equal (app.cfg.media.codec[8], SIPPAK_CODEC_SILK);
  assert_int_equal (app.cfg.media.codec[9], SIPPAK_CODEC_OPUS);
  assert_int_equal (app.cfg.media.codec[10], SIPPAK_CODEC_BCG729);
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
    cmocka_unit_test(parse_all_codecs_icase),
    cmocka_unit_test(fail_on_duplicated_codec),
  };
  return cmocka_run_group_tests_name("Media helper", tests, NULL, NULL);
}
