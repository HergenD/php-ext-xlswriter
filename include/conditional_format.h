#ifndef VTIFUL_CONDITIONAL_FORMAT_H
#define VTIFUL_CONDITIONAL_FORMAT_H

#include "xlswriter.h"

extern zend_class_entry *vtiful_conditional_format_ce;
extern int le_xls_conditional_format;

typedef struct {
    lxw_conditional_format *format;
    zend_object zo;
} conditional_format_object;

static inline conditional_format_object *php_vtiful_conditional_format_fetch_object(zend_object *obj) {
    return (conditional_format_object *)((char *)(obj) - XtOffsetOf(conditional_format_object, zo));
}

#define Z_CONDITIONAL_FORMAT_P(zv) php_vtiful_conditional_format_fetch_object(Z_OBJ_P(zv))
#define V_CONDITIONAL_FORMAT_FMT "format"

PHP_VTIFUL_API lxw_conditional_format *zval_get_conditional_format(zval *resource);
VTIFUL_STARTUP_FUNCTION(conditional_format);

#endif