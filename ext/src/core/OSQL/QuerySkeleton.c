/***************************************************************************
 *   Copyright (C) 2007 by Denis M. Gabaidulin                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/
/* $Id$ */

#include "onphp_core.h"
#include "onphp_util.h"

#include "zend_exceptions.h"
#include "ext/standard/php_string.h"

#include "core/Exceptions.h"
#include "core/OSQL/QuerySkeleton.h"
#include "core/Logic/LogicalObject.h"
#include "core/DB/Dialect.h"

ONPHP_METHOD(QuerySkeleton, __construct)
{
	zval *where, *whereLogic;
	
	/* init */
	where = ONPHP_READ_PROPERTY(getThis(), "where");
	array_init(where);
	
	whereLogic = ONPHP_READ_PROPERTY(getThis(), "whereLogic");
	array_init(whereLogic);
}

ONPHP_METHOD(QuerySkeleton, __destruct)
{
	zval **data;
	
	if (
		SUCCESS
		== zend_hash_find(
			HASH_OF(this_ptr),
			"where",
			sizeof("where"),
			(void**) &data
		)
	) {
		zval_ptr_dtor(data);
	}
	
	if (
		SUCCESS
		== zend_hash_find(
			HASH_OF(this_ptr),
			"whereLogic",
			sizeof("whereLogic"),
			(void**) &data
		)
	) {
		zval_ptr_dtor(data);
	}
}

ONPHP_METHOD(QuerySkeleton, where)
{
	zval *where, *whereLogic, *exp, *logic;
	
	if (
		zend_parse_parameters(
			ZEND_NUM_ARGS() TSRMLS_CC,
			"z|z",
			&exp,
			&logic
		)
		== FAILURE
	) {
		WRONG_PARAM_COUNT;
	}
	
	where = ONPHP_READ_PROPERTY(getThis(), "where");
	
	if (
		Z_TYPE_P(where) != IS_NULL
		&& Z_TYPE_P(logic) == IS_NULL
	) {
		zend_throw_exception_ex(
			onphp_ce_WrongArgumentException,
			0 TSRMLS_CC,
			"you have to specify expression logic"
		);
		return;
	} else {
		if (
			Z_TYPE_P(where) == IS_NULL
			&& Z_TYPE_P(logic) != IS_NULL
		) {
			logic = NULL;
		}
	
		whereLogic = ONPHP_READ_PROPERTY(getThis(), "whereLogic");
		
		add_next_index_zval(whereLogic, logic);
		add_next_index_zval(where, exp);
	}
	
	RETURN_ZVAL(getThis(), 1, 0);
}

ONPHP_METHOD(QuerySkeleton, andWhere)
{
	zval *exp, *logic, *retval;
	
	MAKE_STD_ZVAL(logic);
	ZVAL_STRING(logic, "AND", 1);
	
	if (
		zend_parse_parameters(
			ZEND_NUM_ARGS() TSRMLS_CC,
			"z",
			&exp
		)
		== FAILURE
	) {
		WRONG_PARAM_COUNT;
	}
	
	zend_call_method_with_2_params(
			&getThis(),
			onphp_ce_QuerySkeleton,
			NULL,
			"where",
			&retval,
			exp,
			logic
		);
	
	if (EG(exception)) {
		return;
	}
	
	RETURN_ZVAL(retval, 1, 0);
}

ONPHP_METHOD(QuerySkeleton, orWhere)
{
	zval *exp, *logic, *retval;
	
	MAKE_STD_ZVAL(logic);
	ZVAL_STRING(logic, "OR", 1);
	
	if (
		zend_parse_parameters(
			ZEND_NUM_ARGS() TSRMLS_CC,
			"z",
			&exp
		)
		== FAILURE
	) {
		WRONG_PARAM_COUNT;
	}
	
	zend_call_method_with_2_params(
			&getThis(),
			onphp_ce_QuerySkeleton,
			NULL,
			"where",
			&retval,
			exp,
			logic
		);
	
	if (EG(exception)) {
		return;
	}
	
	RETURN_ZVAL(retval, 1, 0);
}

ONPHP_METHOD(QuerySkeleton, toDialectString)
{
	zval *where, *whereLogic, *dialect;
	
	if (
		zend_parse_parameters(
			ZEND_NUM_ARGS() TSRMLS_CC,
			"z",
			&dialect
		)
		== FAILURE
	) {
		WRONG_PARAM_COUNT;
	}
	
	where = ONPHP_READ_PROPERTY(getThis(), "where");
	whereLogic = ONPHP_READ_PROPERTY(getThis(), "whereLogic");
	
	if (Z_TYPE_P(where) != IS_NULL) {
		zval *outputLogic, *exp = NULL;
		zval **data;
		int i, array_count, retval_len;
		char *retval = NULL;

		MAKE_STD_ZVAL(outputLogic);
		ZVAL_FALSE(outputLogic);
		
		smart_str clause = {0};
		smart_str_appendl(&clause, " WHERE", 6);
		
		array_count = zend_hash_num_elements(Z_ARRVAL_P(where));
		
		for (i = 0; i < array_count; i++) {
			if (
				zend_hash_index_find(
					Z_ARRVAL_P(where),
					i,
					(void **)&data
				) == SUCCESS
			) {
		    	zend_call_method_with_1_params(
					data,
					Z_OBJCE_PP(data),
					NULL,
					"todialectstring",
					&exp,
					dialect
				);
				
				if (EG(exception)) {
					return;
				}
				
				if (exp) {
					if (
						zend_hash_index_find(
							Z_ARRVAL_P(whereLogic),
							i,
							(void **)&data
						) == SUCCESS
					) {
						onphp_append_zval_to_smart_string(&clause, (zval*)data);
						smart_str_appendl(&clause, " ", 1);
						onphp_append_zval_to_smart_string(&clause, exp);
						smart_str_appendl(&clause, ".", 1);
						
						ZVAL_TRUE(outputLogic);
					}
					
					ZVAL_FREE(exp);
				}
				
				if (
					!Z_BVAL_P(outputLogic)
					&& (
						zend_hash_index_find(
							Z_ARRVAL_P(whereLogic),
							i + 1,
							(void **)&data
						) == SUCCESS
					)
				) {
					data = NULL;
				}
		   	}
		}
		
		retval = (char*) php_trim(clause.c, clause.len, " ", 1, NULL, 2);
		smart_str_0(&clause);
		retval_len = strlen(retval);
		RETURN_STRINGL(retval, retval_len, 0);
	}
	
	return;
}

static ONPHP_ARGINFO_LOGICAL_OBJECT;
static ONPHP_ARGINFO_LOGICAL_OBJECT_AND_ONE;
static ONPHP_ARGINFO_DIALECT;

zend_function_entry onphp_funcs_QuerySkeleton[] = {
	ONPHP_ME(QuerySkeleton, __construct, NULL, ZEND_ACC_PUBLIC)
	ONPHP_ME(QuerySkeleton, __destruct, NULL, ZEND_ACC_PUBLIC)
	ONPHP_ME(QuerySkeleton, where, arginfo_logical_object_and_one, ZEND_ACC_PUBLIC)
	ONPHP_ME(QuerySkeleton, andWhere, arginfo_logical_object, ZEND_ACC_PUBLIC)
	ONPHP_ME(QuerySkeleton, orWhere, arginfo_logical_object, ZEND_ACC_PUBLIC)
	ONPHP_ME(QuerySkeleton, toDialectString, arginfo_dialect, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};