#include "xlswriter.h"
#include "ext/standard/php_string.h"
#include "conditional_format.h"
#include "format.h"

zend_class_entry *vtiful_conditional_format_ce;
static zend_object_handlers vtiful_conditional_format_handlers;

int le_xls_conditional_format;

/* {{{ Resource object creation and destruction */
static zend_object *vtiful_conditional_format_objects_new(zend_class_entry *ce)
{
    conditional_format_object *obj = ecalloc(1, sizeof(conditional_format_object) + zend_object_properties_size(ce));

    zend_object_std_init(&obj->zo, ce);
    object_properties_init(&obj->zo, ce);

    obj->zo.handlers = &vtiful_conditional_format_handlers;
    obj->format = calloc(1, sizeof(lxw_conditional_format));

    return &obj->zo;
}

static void vtiful_conditional_format_objects_free(zend_object *object)
{
    conditional_format_object *obj = php_vtiful_conditional_format_fetch_object(object);
    if (obj->format) {
        if (obj->format->value_string) {
            efree(obj->format->value_string);
        }
        free(obj->format);
    }
    zend_object_std_dtor(&obj->zo);
}

PHP_VTIFUL_API lxw_conditional_format *zval_get_conditional_format(zval *resource)
{
    conditional_format_object *obj = NULL;
    if ((obj = (conditional_format_object *)zend_fetch_resource(Z_RES_P(resource), VTIFUL_RESOURCE_NAME, le_xls_conditional_format)) == NULL) {
        return NULL;
    }
    return obj->format;
}
/* }}} */

/* {{{ Arguments information */
ZEND_BEGIN_ARG_INFO_EX(conditional_format_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_set_format_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, format, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_set_value_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_set_criteria_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, criteria)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_set_color_scale_arginfo, 0, 0, 3)
    ZEND_ARG_INFO(0, min_color)
    ZEND_ARG_INFO(0, mid_color)
    ZEND_ARG_INFO(0, max_color)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_set_data_bar_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, color)
    ZEND_ARG_INFO(0, only_data_bar)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_set_icon_style_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, style)
    ZEND_ARG_INFO(0, reverse)
    ZEND_ARG_INFO(0, icons_only)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(conditional_format_set_stop_if_true_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, stop)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ Methods implementation */
PHP_METHOD(vtiful_conditional_format, __construct)
{
    zend_long type;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(type)
    ZEND_PARSE_PARAMETERS_END();

    conditional_format_object *obj = Z_CONDITIONAL_FORMAT_P(getThis());
    obj->format->type = type;
}

PHP_METHOD(vtiful_conditional_format, setFormat)
{
    zval *format;
    zval *bg_color = NULL, *fg_color = NULL;
    HashTable *format_ht;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(format)
    ZEND_PARSE_PARAMETERS_END();

    conditional_format_object *obj = Z_CONDITIONAL_FORMAT_P(getThis());
    format_ht = Z_ARRVAL_P(format);

    if (!obj->format->format) {
        obj->format->format = calloc(1, sizeof(lxw_format));
    }

    if ((bg_color = zend_hash_str_find(format_ht, ZEND_STRL("bg_color"))) != NULL) {
        obj->format->format->bg_color = format_color_string_to_number(Z_STRVAL_P(bg_color));
    }

    if ((fg_color = zend_hash_str_find(format_ht, ZEND_STRL("font_color"))) != NULL) {
        obj->format->format->font_color = format_color_string_to_number(Z_STRVAL_P(fg_color));
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(vtiful_conditional_format, setValue)
{
    zval *value;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    conditional_format_object *obj = Z_CONDITIONAL_FORMAT_P(getThis());

    // Free existing value_string if it exists
    if (obj->format->value_string) {
        efree(obj->format->value_string);
        obj->format->value_string = NULL;
    }

    switch (Z_TYPE_P(value)) {
        case IS_LONG:
            obj->format->value = Z_LVAL_P(value);
            break;
        case IS_DOUBLE:
            obj->format->value = Z_DVAL_P(value);
            break;
        case IS_STRING:
            obj->format->value_string = estrndup(Z_STRVAL_P(value), Z_STRLEN_P(value));
            break;
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(vtiful_conditional_format, setCriteria)
{
    zend_long criteria;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(criteria)
    ZEND_PARSE_PARAMETERS_END();

    conditional_format_object *obj = Z_CONDITIONAL_FORMAT_P(getThis());
    obj->format->criteria = criteria;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(vtiful_conditional_format, setColorScale)
{
    zend_string *min_color, *mid_color, *max_color;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_STR(min_color)
        Z_PARAM_STR(mid_color)
        Z_PARAM_STR(max_color)
    ZEND_PARSE_PARAMETERS_END();

    conditional_format_object *obj = Z_CONDITIONAL_FORMAT_P(getThis());

    obj->format->min_color = format_color_string_to_number(ZSTR_VAL(min_color));
    obj->format->mid_color = format_color_string_to_number(ZSTR_VAL(mid_color));
    obj->format->max_color = format_color_string_to_number(ZSTR_VAL(max_color));

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(vtiful_conditional_format, setDataBar)
{
    zend_string *color;
    zend_bool only_data_bar = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STR(color)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(only_data_bar)
    ZEND_PARSE_PARAMETERS_END();

    conditional_format_object *obj = Z_CONDITIONAL_FORMAT_P(getThis());

    obj->format->bar_color = format_color_string_to_number(ZSTR_VAL(color));
    obj->format->bar_only = only_data_bar;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(vtiful_conditional_format, setIconStyle)
{
    zend_long style;
    zend_bool reverse = 0, icons_only = 0;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_LONG(style)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(reverse)
        Z_PARAM_BOOL(icons_only)
    ZEND_PARSE_PARAMETERS_END();

    conditional_format_object *obj = Z_CONDITIONAL_FORMAT_P(getThis());

    obj->format->icon_style = style;
    obj->format->reverse_icons = reverse;
    obj->format->icons_only = icons_only;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(vtiful_conditional_format, setStopIfTrue)
{
    zend_bool stop;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_BOOL(stop)
    ZEND_PARSE_PARAMETERS_END();

    conditional_format_object *obj = Z_CONDITIONAL_FORMAT_P(getThis());
    obj->format->stop_if_true = stop;

    RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/* {{{ Method entries */
static const zend_function_entry vtiful_conditional_format_methods[] = {
    PHP_ME(vtiful_conditional_format, __construct,     conditional_format_construct_arginfo,     ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(vtiful_conditional_format, setFormat,       conditional_format_set_format_arginfo,    ZEND_ACC_PUBLIC)
    PHP_ME(vtiful_conditional_format, setValue,        conditional_format_set_value_arginfo,     ZEND_ACC_PUBLIC)
    PHP_ME(vtiful_conditional_format, setCriteria,     conditional_format_set_criteria_arginfo,  ZEND_ACC_PUBLIC)
    PHP_ME(vtiful_conditional_format, setColorScale,   conditional_format_set_color_scale_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(vtiful_conditional_format, setDataBar,      conditional_format_set_data_bar_arginfo,  ZEND_ACC_PUBLIC)
    PHP_ME(vtiful_conditional_format, setIconStyle,    conditional_format_set_icon_style_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(vtiful_conditional_format, setStopIfTrue,   conditional_format_set_stop_if_true_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

/* {{{ VTIFUL_STARTUP_FUNCTION */
VTIFUL_STARTUP_FUNCTION(conditional_format)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Vtiful\\Kernel", "ConditionalFormat", vtiful_conditional_format_methods);
    vtiful_conditional_format_ce = zend_register_internal_class(&ce);
    vtiful_conditional_format_ce->create_object = vtiful_conditional_format_objects_new;

    memcpy(&vtiful_conditional_format_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    vtiful_conditional_format_handlers.offset = XtOffsetOf(conditional_format_object, zo);
    vtiful_conditional_format_handlers.free_obj = vtiful_conditional_format_objects_free;

    REGISTER_CLASS_PROPERTY_NULL(vtiful_conditional_format_ce, V_CONDITIONAL_FORMAT_FMT, ZEND_ACC_PRIVATE);

    /* Register constants */
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_CELL",          LXW_CONDITIONAL_TYPE_CELL)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_TEXT",          LXW_CONDITIONAL_TYPE_TEXT)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_TIME_PERIOD",   LXW_CONDITIONAL_TYPE_TIME_PERIOD)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_AVERAGE",       LXW_CONDITIONAL_TYPE_AVERAGE)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_DUPLICATE",     LXW_CONDITIONAL_TYPE_DUPLICATE)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_UNIQUE",        LXW_CONDITIONAL_TYPE_UNIQUE)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_TOP",           LXW_CONDITIONAL_TYPE_TOP)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_BOTTOM",        LXW_CONDITIONAL_TYPE_BOTTOM)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_BLANKS",        LXW_CONDITIONAL_TYPE_BLANKS)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_NO_BLANKS",     LXW_CONDITIONAL_TYPE_NO_BLANKS)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_FORMULA",       LXW_CONDITIONAL_TYPE_FORMULA)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_2_COLOR_SCALE", LXW_CONDITIONAL_TYPE_2_COLOR_SCALE)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_3_COLOR_SCALE", LXW_CONDITIONAL_TYPE_3_COLOR_SCALE)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_DATA_BAR",      LXW_CONDITIONAL_TYPE_DATA_BAR)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TYPE_ICON_SETS",     LXW_CONDITIONAL_TYPE_ICON_SETS)

    /* Criteria constants */
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_EQUAL_TO",                 LXW_CONDITIONAL_CRITERIA_EQUAL_TO)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_NOT_EQUAL_TO",              LXW_CONDITIONAL_CRITERIA_NOT_EQUAL_TO)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_GREATER_THAN",              LXW_CONDITIONAL_CRITERIA_GREATER_THAN)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_LESS_THAN",                 LXW_CONDITIONAL_CRITERIA_LESS_THAN)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_GREATER_THAN_OR_EQUAL_TO",  LXW_CONDITIONAL_CRITERIA_GREATER_THAN_OR_EQUAL_TO)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_LESS_THAN_OR_EQUAL_TO",     LXW_CONDITIONAL_CRITERIA_LESS_THAN_OR_EQUAL_TO)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_BETWEEN",                   LXW_CONDITIONAL_CRITERIA_BETWEEN)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_NOT_BETWEEN",               LXW_CONDITIONAL_CRITERIA_NOT_BETWEEN)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_TEXT_CONTAINING",           LXW_CONDITIONAL_CRITERIA_TEXT_CONTAINING)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_TEXT_NOT_CONTAINING",       LXW_CONDITIONAL_CRITERIA_TEXT_NOT_CONTAINING)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_TEXT_BEGINS_WITH",          LXW_CONDITIONAL_CRITERIA_TEXT_BEGINS_WITH)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "CRITERIA_TEXT_ENDS_WITH",            LXW_CONDITIONAL_CRITERIA_TEXT_ENDS_WITH)

    /* Time period criteria constants */
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_TODAY",         LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_TODAY)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_YESTERDAY",     LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_YESTERDAY)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_LAST_7_DAYS",   LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_LAST_7_DAYS)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_LAST_WEEK",     LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_LAST_WEEK)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_THIS_WEEK",     LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_THIS_WEEK)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_NEXT_WEEK",     LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_NEXT_WEEK)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_LAST_MONTH",    LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_LAST_MONTH)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_THIS_MONTH",    LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_THIS_MONTH)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "TIME_PERIOD_NEXT_MONTH",    LXW_CONDITIONAL_CRITERIA_TIME_PERIOD_NEXT_MONTH)

    /* Icon style constants */
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_STYLE_3_TRAFFIC_LIGHTS", LXW_CONDITIONAL_TYPE_ICON_TRAFFIC_LIGHTS_3)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_STYLE_3_SIGNS",          LXW_CONDITIONAL_TYPE_ICON_SIGNS_3)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_STYLE_4_TRAFFIC_LIGHTS", LXW_CONDITIONAL_TYPE_ICON_TRAFFIC_LIGHTS_4)
    REGISTER_CLASS_CONST_LONG(vtiful_conditional_format_ce, "ICON_STYLE_5_QUARTERS",       LXW_CONDITIONAL_TYPE_ICON_QUARTERS_5)

    return SUCCESS;
}
/* }}} */