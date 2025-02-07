/*
  +----------------------------------------------------------------------+
  | XlsWriter Extension                                                  |
  +----------------------------------------------------------------------+
  | Copyright (c) 2017-2018 The Viest                                    |
  +----------------------------------------------------------------------+
  | http://www.viest.me                                                  |
  +----------------------------------------------------------------------+
  | Author: viest <dev@service.viest.me>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_EXT_XLS_WRITER_CONDITIONAL_FORMAT_H
#define PHP_EXT_XLS_WRITER_CONDITIONAL_FORMAT_H

#include "common.h"

extern zend_class_entry *vtiful_conditional_format_ce;

VTIFUL_STARTUP_FUNCTION(conditional_format);

typedef struct _vtiful_conditional_format_object {
    xls_resource_conditional_format_t ptr;
    zend_object zo;
} conditional_format_object;

static inline conditional_format_object *php_vtiful_conditional_format_fetch_object(zend_object *obj) {
    return (conditional_format_object *)((char *)obj - XtOffsetOf(conditional_format_object, zo));
}

#define Z_COND_FORMAT_P(zv) php_vtiful_conditional_format_fetch_object(Z_OBJ_P(zv))

#endif