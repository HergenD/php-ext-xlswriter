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

#include "xlswriter.h"

zend_class_entry *vtiful_conditional_format_ce;

static zend_object_handlers conditional_format_handlers;

static zend_always_inline void *vtiful_conditional_format_object_alloc(size_t obj_size, zend_class_entry *ce) {
    void *obj = emalloc(obj_size);
    memset(obj, 0, obj_size);
    return obj;
}

PHP_VTIFUL_API zend_object *conditional_format_objects_new(zend_class_entry *ce)
{
    conditional_format_object *format = vtiful_conditional_format_object_alloc(sizeof(conditional_format_object), ce);

    zend_object_std_init(&format->zo, ce);
    object_properties_init(&format->zo, ce);

    format->ptr.conditional_format = NULL;
    format->zo.handlers = &conditional_format_handlers;

    return &format->zo;
}

static void conditional_format_objects_free(zend_object *object)
{
    conditional_format_object *intern = php_vtiful_conditional_format_fetch_object(object);

    if (intern->ptr.conditional_format != NULL) {
        // free by workbook
        intern->ptr.conditional_format = NULL;
    }

    zend_object_std_dtor(&intern->zo);
}

/* {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(conditional_format_construct_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_type_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_criteria_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, criteria)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_value_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_format_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_min_max_value_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_color_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, color)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_rule_type_arginfo, 0, 0, 2)
                ZEND_ARG_INFO(0, which)
                ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_bar_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_icon_style_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, style)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_bool_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_multi_range_arginfo, 0, 0, 1)
                ZEND_ARG_INFO(0, range)
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::__construct(resource $handle)
 */
PHP_METHOD(vtiful_conditional_format, __construct)
{
    zval *handle;
    conditional_format_object *obj;
    xls_resource_write_t *xls_res;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_RESOURCE(handle)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    xls_res = zval_get_resource(handle);
    obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format == NULL) {
        obj->ptr.conditional_format = calloc(1, sizeof(lxw_conditional_format));
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::type(int $type)
 */
PHP_METHOD(vtiful_conditional_format, type)
{
    zend_long type;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(type)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->type = type;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::criteria(int $criteria)
 */
PHP_METHOD(vtiful_conditional_format, criteria)
{
    zend_long criteria;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(criteria)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->criteria = criteria;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::value(mixed $value)
 */
PHP_METHOD(vtiful_conditional_format, value)
{
    zval *value = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (!obj->ptr.conditional_format) {
        return;
    }

    switch (Z_TYPE_P(value)) {
        case IS_LONG:
            obj->ptr.conditional_format->value = Z_LVAL_P(value);
            break;
        case IS_DOUBLE:
            obj->ptr.conditional_format->value = Z_DVAL_P(value);
            break;
        case IS_STRING:
            obj->ptr.conditional_format->value_string = Z_STRVAL_P(value);
            break;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::format(resource $format)
 */
PHP_METHOD(vtiful_conditional_format, format)
{
    zval *format_handle = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_RESOURCE(format_handle)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->format = zval_get_format(format_handle);
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::minValue(mixed $value)
 */
PHP_METHOD(vtiful_conditional_format, minValue)
{
    zval *value = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (!obj->ptr.conditional_format) {
        return;
    }

    switch (Z_TYPE_P(value)) {
        case IS_LONG:
            obj->ptr.conditional_format->min_value = Z_LVAL_P(value);
            break;
        case IS_DOUBLE:
            obj->ptr.conditional_format->min_value = Z_DVAL_P(value);
            break;
        case IS_STRING:
            obj->ptr.conditional_format->min_value_string = Z_STRVAL_P(value);
            break;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::maxValue(mixed $value)
 */
PHP_METHOD(vtiful_conditional_format, maxValue)
{
    zval *value = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (!obj->ptr.conditional_format) {
        return;
    }

    switch (Z_TYPE_P(value)) {
        case IS_LONG:
            obj->ptr.conditional_format->max_value = Z_LVAL_P(value);
            break;
        case IS_DOUBLE:
            obj->ptr.conditional_format->max_value = Z_DVAL_P(value);
            break;
        case IS_STRING:
            obj->ptr.conditional_format->max_value_string = Z_STRVAL_P(value);
            break;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::ruleType(string $which, int $type)
 */
PHP_METHOD(vtiful_conditional_format, ruleType)
{
    zend_long type;
    zend_string *which;

    ZEND_PARSE_PARAMETERS_START(2, 2)
            Z_PARAM_STR(which)
            Z_PARAM_LONG(type)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (!obj->ptr.conditional_format) {
        return;
    }

    if (strncmp(ZSTR_VAL(which), "min", 3) == 0) {
        obj->ptr.conditional_format->min_rule_type = type;
    } else if (strncmp(ZSTR_VAL(which), "mid", 3) == 0) {
        obj->ptr.conditional_format->mid_rule_type = type;
    } else if (strncmp(ZSTR_VAL(which), "max", 3) == 0) {
        obj->ptr.conditional_format->max_rule_type = type;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::minColor(int $color)
 */
PHP_METHOD(vtiful_conditional_format, minColor)
{
    zend_long color;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(color)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->min_color = color;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::midColor(int $color)
 */
PHP_METHOD(vtiful_conditional_format, midColor)
{
    zend_long color;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(color)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->mid_color = color;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::maxColor(int $color)
 */
PHP_METHOD(vtiful_conditional_format, maxColor)
{
    zend_long color;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(color)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->max_color = color;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::barOnly(bool $bar_only)
 */
PHP_METHOD(vtiful_conditional_format, barOnly)
{
    zend_bool bar_only;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_BOOL(bar_only)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->bar_only = bar_only;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::barColor(int $color)
 */
PHP_METHOD(vtiful_conditional_format, barColor)
{
    zend_long color;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(color)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->bar_color = color;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::barDirection(int $direction)
 */
PHP_METHOD(vtiful_conditional_format, barDirection)
{
    zend_long direction;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(direction)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->bar_direction = direction;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::barAxisPosition(int $position)
 */
PHP_METHOD(vtiful_conditional_format, barAxisPosition)
{
    zend_long position;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(position)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->bar_axis_position = position;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::iconStyle(int $style)
 */
PHP_METHOD(vtiful_conditional_format, iconStyle)
{
    zend_long style;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_LONG(style)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->icon_style = style;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::reverseIcons(bool $reverse)
 */
PHP_METHOD(vtiful_conditional_format, reverseIcons)
{
    zend_bool reverse;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_BOOL(reverse)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->reverse_icons = reverse;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::iconsOnly(bool $icons_only)
 */
PHP_METHOD(vtiful_conditional_format, iconsOnly)
{
    zend_bool icons_only;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_BOOL(icons_only)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->icons_only = icons_only;
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat::multiRange(string $range)
 */
PHP_METHOD(vtiful_conditional_format, multiRange)
{
    zend_string *range;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_STR(range)
    ZEND_PARSE_PARAMETERS_END();

    ZVAL_COPY(return_value, getThis());

    conditional_format_object *obj = Z_COND_FORMAT_P(getThis());

    if (obj->ptr.conditional_format) {
        obj->ptr.conditional_format->multi_range = ZSTR_VAL(range);
    }
}
/* }}} */

/* {{{ \Vtiful\Kernel\ConditionalFormat method entries
 */
zend_function_entry conditional_format_methods[] = {
        PHP_ME(vtiful_conditional_format, __construct,      conditional_format_construct_arginfo,      ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, type,            conditional_format_type_arginfo,           ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, criteria,        conditional_format_criteria_arginfo,       ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, value,           conditional_format_value_arginfo,          ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, format,          conditional_format_format_arginfo,         ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, minValue,        conditional_format_min_max_value_arginfo,  ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, maxValue,        conditional_format_min_max_value_arginfo,  ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, ruleType,        conditional_format_rule_type_arginfo,      ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, minColor,        conditional_format_color_arginfo,          ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, midColor,        conditional_format_color_arginfo,          ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, maxColor,        conditional_format_color_arginfo,          ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, barOnly,         conditional_format_bool_arginfo,           ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, barColor,        conditional_format_color_arginfo,          ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, barDirection,    conditional_format_bar_arginfo,            ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, barAxisPosition, conditional_format_bar_arginfo,            ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, iconStyle,       conditional_format_icon_style_arginfo,     ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, reverseIcons,    conditional_format_bool_arginfo,           ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, iconsOnly,       conditional_format_bool_arginfo,           ZEND_ACC_PUBLIC)
        PHP_ME(vtiful_conditional_format, multiRange,      conditional_format_multi_range_arginfo,    ZEND_ACC_PUBLIC)
        PHP_FE_END
};
/* }}} */

/* {{{ VTIFUL_STARTUP_FUNCTION
 */
VTIFUL_STARTUP_FUNCTION(conditional_format) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Vtiful\\Kernel", "ConditionalFormat", conditional_format_methods);
    ce.create_object = conditional_format_objects_new;
    vtiful_conditional_format_ce = zend_register_internal_class(&ce);

    memcpy(&conditional_format_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    conditional_format_handlers.offset   = XtOffsetOf(conditional_format_object, zo);
    conditional_format_handlers.free_obj = conditional_format_objects_free;

    // Types
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_CELL", LXW_CONDITIONAL_TYPE_CELL);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_TEXT", LXW_CONDITIONAL_TYPE_TEXT);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_TIME_PERIOD", LXW_CONDITIONAL_TYPE_TIME_PERIOD);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_AVERAGE", LXW_CONDITIONAL_TYPE_AVERAGE);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_DUPLICATE", LXW_CONDITIONAL_TYPE_DUPLICATE);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_UNIQUE", LXW_CONDITIONAL_TYPE_UNIQUE);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_TOP", LXW_CONDITIONAL_TYPE_TOP);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_BOTTOM", LXW_CONDITIONAL_TYPE_BOTTOM);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_BLANKS", LXW_CONDITIONAL_TYPE_BLANKS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_NO_BLANKS", LXW_CONDITIONAL_TYPE_NO_BLANKS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_ERRORS", LXW_CONDITIONAL_TYPE_ERRORS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_NO_ERRORS", LXW_CONDITIONAL_TYPE_NO_ERRORS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_FORMULA", LXW_CONDITIONAL_TYPE_FORMULA);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_2_COLOR_SCALE", LXW_CONDITIONAL_2_COLOR_SCALE);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_3_COLOR_SCALE", LXW_CONDITIONAL_3_COLOR_SCALE);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_DATA_BAR", LXW_CONDITIONAL_DATA_BAR);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_ICON_SETS", LXW_CONDITIONAL_TYPE_ICON_SETS);

    // Criteria
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_EQUAL_TO", LXW_CONDITIONAL_CRITERIA_EQUAL_TO);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_NOT_EQUAL_TO", LXW_CONDITIONAL_CRITERIA_NOT_EQUAL_TO);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_GREATER_THAN", LXW_CONDITIONAL_CRITERIA_GREATER_THAN);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_LESS_THAN", LXW_CONDITIONAL_CRITERIA_LESS_THAN);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_GREATER_THAN_OR_EQUAL_TO", LXW_CONDITIONAL_CRITERIA_GREATER_THAN_OR_EQUAL_TO);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_LESS_THAN_OR_EQUAL_TO", LXW_CONDITIONAL_CRITERIA_LESS_THAN_OR_EQUAL_TO);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_BETWEEN", LXW_CONDITIONAL_CRITERIA_BETWEEN);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_NOT_BETWEEN", LXW_CONDITIONAL_CRITERIA_NOT_BETWEEN);

    // Icon Sets
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_3_ARROWS_COLORED", LXW_CONDITIONAL_ICONS_3_ARROWS_COLORED);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_3_ARROWS_GRAY", LXW_CONDITIONAL_ICONS_3_ARROWS_GRAY);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_3_FLAGS", LXW_CONDITIONAL_ICONS_3_FLAGS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_3_TRAFFIC_LIGHTS_UNRIMMED", LXW_CONDITIONAL_ICONS_3_TRAFFIC_LIGHTS_UNRIMMED);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_3_TRAFFIC_LIGHTS_RIMMED", LXW_CONDITIONAL_ICONS_3_TRAFFIC_LIGHTS_RIMMED);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_3_SIGNS", LXW_CONDITIONAL_ICONS_3_SIGNS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_3_SYMBOLS_CIRCLED", LXW_CONDITIONAL_ICONS_3_SYMBOLS_CIRCLED);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_3_SYMBOLS_UNCIRCLED", LXW_CONDITIONAL_ICONS_3_SYMBOLS_UNCIRCLED);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_4_ARROWS_COLORED", LXW_CONDITIONAL_ICONS_4_ARROWS_COLORED);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_4_ARROWS_GRAY", LXW_CONDITIONAL_ICONS_4_ARROWS_GRAY);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_4_RED_TO_BLACK", LXW_CONDITIONAL_ICONS_4_RED_TO_BLACK);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_4_RATINGS", LXW_CONDITIONAL_ICONS_4_RATINGS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_4_TRAFFIC_LIGHTS", LXW_CONDITIONAL_ICONS_4_TRAFFIC_LIGHTS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_5_ARROWS_COLORED", LXW_CONDITIONAL_ICONS_5_ARROWS_COLORED);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_5_ARROWS_GRAY", LXW_CONDITIONAL_ICONS_5_ARROWS_GRAY);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_5_RATINGS", LXW_CONDITIONAL_ICONS_5_RATINGS);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_5_QUARTERS", LXW_CONDITIONAL_ICONS_5_QUARTERS);

// Bar Directions
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "BAR_DIRECTION_CONTEXT", LXW_CONDITIONAL_BAR_DIRECTION_CONTEXT);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "BAR_DIRECTION_RIGHT_TO_LEFT", LXW_CONDITIONAL_BAR_DIRECTION_RIGHT_TO_LEFT);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "BAR_DIRECTION_LEFT_TO_RIGHT", LXW_CONDITIONAL_BAR_DIRECTION_LEFT_TO_RIGHT);

    // Bar Axis Positions
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "BAR_AXIS_AUTOMATIC", LXW_CONDITIONAL_BAR_AXIS_AUTOMATIC);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "BAR_AXIS_MIDPOINT", LXW_CONDITIONAL_BAR_AXIS_MIDPOINT);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "BAR_AXIS_NONE", LXW_CONDITIONAL_BAR_AXIS_NONE);

    // Rule Types
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "RULE_TYPE_MINIMUM", LXW_CONDITIONAL_RULE_TYPE_MINIMUM);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "RULE_TYPE_NUMBER", LXW_CONDITIONAL_RULE_TYPE_NUMBER);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "RULE_TYPE_PERCENT", LXW_CONDITIONAL_RULE_TYPE_PERCENT);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "RULE_TYPE_PERCENTILE", LXW_CONDITIONAL_RULE_TYPE_PERCENTILE);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "RULE_TYPE_FORMULA", LXW_CONDITIONAL_RULE_TYPE_FORMULA);
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "RULE_TYPE_MAXIMUM", LXW_CONDITIONAL_RULE_TYPE_MAXIMUM);

    return SUCCESS;
}
/* }}} */