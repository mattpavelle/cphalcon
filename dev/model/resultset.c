
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
/**
 * Phalcon_Model_Resultset
 *
 * This component allows to Phalcon_Model_Base returns large resulsets with the minimum memory consumption
 * Resulsets can be traversed using a standard foreach or a while statement. If a resultset is serialized
 * it will dump all the rows into a big array. Then unserialize will retrieve the rows as they were before
 * serializing.
 *
 * 
 *
 */

/**
 * Phalcon_Model_Resultset constructor
 *
 * @param Phalcon_Model_Base $model
 * @param Phalcon_Model_Result $result
 * @param Phalcon_Model_Cache $cache
 */
PHP_METHOD(Phalcon_Model_Resultset, __construct){

	zval *model = NULL, *result = NULL, *cache = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &model, &result, &cache) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!cache) {
		PHALCON_INIT_VAR(cache);
		ZVAL_NULL(cache);
	}
	
	phalcon_update_property_zval(this_ptr, SL("_model"), model TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_result"), result TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_cache"), cache TSRMLS_CC);
	phalcon_update_property_long(this_ptr, SL("_type"), 1 TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Check whether internal resource has rows to fetch
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Model_Resultset, valid){

	zval *result = NULL, *row = NULL, *rows = NULL;
	zval *t0 = NULL, *t1 = NULL, *t2 = NULL, *t3 = NULL;
	zval *c0 = NULL, *c1 = NULL;
	zval *r0 = NULL, *r1 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_type"), PHALCON_NOISY TSRMLS_CC);
	if (zend_is_true(t0)) {
		PHALCON_ALLOC_ZVAL_MM(t1);
		phalcon_read_property(&t1, this_ptr, SL("_result"), PHALCON_NOISY TSRMLS_CC);
		PHALCON_CPY_WRT(result, t1);
		if (Z_TYPE_P(result) != IS_BOOL || (Z_TYPE_P(result) == IS_BOOL && Z_BVAL_P(result))) {
			PHALCON_INIT_VAR(c0);
			ZVAL_LONG(c0, 1);
			PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "setfetchmode", c0, PHALCON_NO_CHECK);
			
			PHALCON_ALLOC_ZVAL_MM(r0);
			PHALCON_CALL_METHOD_PARAMS_1(r0, result, "fetcharray", result, PHALCON_NO_CHECK);
			PHALCON_CPY_WRT(row, r0);
			if (zend_is_true(row)) {
				PHALCON_ALLOC_ZVAL_MM(r1);
				PHALCON_ALLOC_ZVAL_MM(t2);
				phalcon_read_property(&t2, this_ptr, SL("_model"), PHALCON_NOISY TSRMLS_CC);
				PHALCON_CALL_STATIC_PARAMS_2(r1, "phalcon_model_base", "dumpresult", t2, row);
				phalcon_update_property_zval(this_ptr, SL("_activeRow"), r1 TSRMLS_CC);
				PHALCON_MM_RESTORE();
				RETURN_TRUE;
			} else {
				PHALCON_INIT_VAR(c1);
				ZVAL_LONG(c1, 2);
				PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "setfetchmode", c1, PHALCON_NO_CHECK);
			}
		}
	} else {
		PHALCON_ALLOC_ZVAL_MM(t3);
		phalcon_read_property(&t3, this_ptr, SL("_rows"), PHALCON_NOISY TSRMLS_CC);
		PHALCON_CPY_WRT(rows, t3);
		Z_SET_ISREF_P(rows);
		
		PHALCON_INIT_VAR(row);
		PHALCON_CALL_FUNC_PARAMS_1(row, "current", rows);
		Z_UNSET_ISREF_P(rows);
		if (zend_is_true(row)) {
			Z_SET_ISREF_P(rows);
			PHALCON_CALL_FUNC_PARAMS_1_NORETURN("next", rows);
			Z_UNSET_ISREF_P(rows);
			phalcon_update_property_zval(this_ptr, SL("_activeRow"), row TSRMLS_CC);
			PHALCON_MM_RESTORE();
			RETURN_TRUE;
		}
	}
	PHALCON_MM_RESTORE();
	RETURN_FALSE;
}

/**
 * Returns current row in the resultset
 *
 * @return Phalcon_Model_Base
 */
PHP_METHOD(Phalcon_Model_Resultset, current){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_activeRow"), PHALCON_NOISY TSRMLS_CC);
	
	RETURN_CHECK_CTOR(t0);
}

/**
 * Moves cursor to next row in the resultset
 *
 */
PHP_METHOD(Phalcon_Model_Resultset, next){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_pointer"), PHALCON_NOISY TSRMLS_CC);
	PHALCON_SEPARATE_NMO(t0);
	increment_function(t0);
	phalcon_update_property_zval(this_ptr, SL("_pointer"), t0 TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Gets pointer number of active row in the resultset
 *
 */
PHP_METHOD(Phalcon_Model_Resultset, key){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_pointer"), PHALCON_NOISY TSRMLS_CC);
	
	RETURN_CHECK_CTOR(t0);
}

/**
 * Rewinds resultset to its beginning
 *
 */
PHP_METHOD(Phalcon_Model_Resultset, rewind){

	zval *result = NULL;
	zval *t0 = NULL, *t1 = NULL, *t2 = NULL;
	zval *c0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_type"), PHALCON_NOISY TSRMLS_CC);
	if (zend_is_true(t0)) {
		PHALCON_ALLOC_ZVAL_MM(t1);
		phalcon_read_property(&t1, this_ptr, SL("_result"), PHALCON_NOISY TSRMLS_CC);
		PHALCON_CPY_WRT(result, t1);
		if (Z_TYPE_P(result) != IS_BOOL || (Z_TYPE_P(result) == IS_BOOL && Z_BVAL_P(result))) {
			phalcon_update_property_long(this_ptr, SL("pointer"), 1 TSRMLS_CC);
			
			PHALCON_INIT_VAR(c0);
			ZVAL_LONG(c0, 0);
			PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "dataseek", c0, PHALCON_NO_CHECK);
		}
	} else {
		PHALCON_ALLOC_ZVAL_MM(t2);
		phalcon_read_property(&t2, this_ptr, SL("_rows"), PHALCON_NOISY TSRMLS_CC);
		Z_SET_ISREF_P(t2);
		PHALCON_CALL_FUNC_PARAMS_1_NORETURN("reset", t2);
		Z_UNSET_ISREF_P(t2);
	}
	
	PHALCON_MM_RESTORE();
}

/**
 * Changes internal pointer to a specific position in the resultset
 *
 * @param int $position
 */
PHP_METHOD(Phalcon_Model_Resultset, seek){

	zval *position = NULL, *rows = NULL, *i = NULL;
	zval *r0 = NULL, *r1 = NULL;
	zval *t0 = NULL, *t1 = NULL, *t2 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &position) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_ALLOC_ZVAL_MM(r0);
	PHALCON_CALL_FUNC_PARAMS_1(r0, "intval", position);
	phalcon_update_property_zval(this_ptr, SL("_pointer"), r0 TSRMLS_CC);
	
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_type"), PHALCON_NOISY TSRMLS_CC);
	if (zend_is_true(t0)) {
		PHALCON_ALLOC_ZVAL_MM(t1);
		phalcon_read_property(&t1, this_ptr, SL("_result"), PHALCON_NOISY TSRMLS_CC);
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(t1, "dataseek", position, PHALCON_NO_CHECK);
	} else {
		PHALCON_ALLOC_ZVAL_MM(t2);
		phalcon_read_property(&t2, this_ptr, SL("_rows"), PHALCON_NOISY TSRMLS_CC);
		PHALCON_CPY_WRT(rows, t2);
		Z_SET_ISREF_P(rows);
		PHALCON_CALL_FUNC_PARAMS_1_NORETURN("reset", rows);
		Z_UNSET_ISREF_P(rows);
		
		PHALCON_INIT_VAR(i);
		ZVAL_LONG(i, 0);
		fs_fd08_0:
			
			PHALCON_INIT_VAR(r1);
			is_smaller_function(r1, i, position TSRMLS_CC);
			if (!zend_is_true(r1)) {
				goto fe_fd08_0;
			}
			Z_SET_ISREF_P(rows);
			PHALCON_CALL_FUNC_PARAMS_1_NORETURN("next", rows);
			Z_UNSET_ISREF_P(rows);
			PHALCON_SEPARATE(i);
			increment_function(i);
			goto fs_fd08_0;
		fe_fd08_0:
		if(0){}
	}
	
	PHALCON_MM_RESTORE();
}

/**
 * Counts how many rows are in the resultset
 *
 * @return int
 */
PHP_METHOD(Phalcon_Model_Resultset, count){

	zval *count = NULL, *result = NULL;
	zval *t0 = NULL, *t1 = NULL, *t2 = NULL, *t3 = NULL, *t4 = NULL;
	zval *r0 = NULL, *r1 = NULL, *r2 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_count"), PHALCON_NOISY TSRMLS_CC);
	PHALCON_ALLOC_ZVAL_MM(r0);
	PHALCON_CALL_FUNC_PARAMS_1(r0, "is_null", t0);
	if (zend_is_true(r0)) {
		PHALCON_INIT_VAR(count);
		ZVAL_LONG(count, 0);
		
		PHALCON_ALLOC_ZVAL_MM(t1);
		phalcon_read_property(&t1, this_ptr, SL("_type"), PHALCON_NOISY TSRMLS_CC);
		if (zend_is_true(t1)) {
			PHALCON_ALLOC_ZVAL_MM(t2);
			phalcon_read_property(&t2, this_ptr, SL("_result"), PHALCON_NOISY TSRMLS_CC);
			PHALCON_CPY_WRT(result, t2);
			if (Z_TYPE_P(result) != IS_BOOL || (Z_TYPE_P(result) == IS_BOOL && Z_BVAL_P(result))) {
				PHALCON_ALLOC_ZVAL_MM(r1);
				PHALCON_CALL_METHOD(r1, result, "numrows", PHALCON_NO_CHECK);
				PHALCON_INIT_VAR(count);
				PHALCON_CALL_FUNC_PARAMS_1(count, "intval", r1);
			}
		} else {
			PHALCON_ALLOC_ZVAL_MM(t3);
			phalcon_read_property(&t3, this_ptr, SL("_rows"), PHALCON_NOISY TSRMLS_CC);
			PHALCON_ALLOC_ZVAL_MM(r2);
			phalcon_fast_count(r2, t3 TSRMLS_CC);
			PHALCON_CPY_WRT(count, r2);
		}
		
		phalcon_update_property_zval(this_ptr, SL("_count"), count TSRMLS_CC);
	}
	
	PHALCON_ALLOC_ZVAL_MM(t4);
	phalcon_read_property(&t4, this_ptr, SL("_count"), PHALCON_NOISY TSRMLS_CC);
	
	RETURN_CHECK_CTOR(t4);
}

/**
 * Checks whether offset exists in the resultset
 *
 * @param int $index
 * @return boolean
 */
PHP_METHOD(Phalcon_Model_Resultset, offsetExists){

	zval *index = NULL;
	zval *r0 = NULL, *r1 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_ALLOC_ZVAL_MM(r0);
	PHALCON_CALL_METHOD(r0, this_ptr, "count", PHALCON_NO_CHECK);
	PHALCON_INIT_VAR(r1);
	is_smaller_function(r1, index, r0 TSRMLS_CC);
	if (zend_is_true(r1)) {
		PHALCON_MM_RESTORE();
		RETURN_TRUE;
	}
	PHALCON_MM_RESTORE();
	RETURN_FALSE;
}

/**
 * Gets row in a specific position of the resultset
 *
 * @param int $index
 * @return Phalcon_Model_Base
 */
PHP_METHOD(Phalcon_Model_Resultset, offsetGet){

	zval *index = NULL;
	zval *r0 = NULL, *r1 = NULL, *r2 = NULL, *r3 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &index) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_ALLOC_ZVAL_MM(r0);
	PHALCON_CALL_METHOD(r0, this_ptr, "count", PHALCON_NO_CHECK);
	PHALCON_INIT_VAR(r1);
	is_smaller_function(r1, index, r0 TSRMLS_CC);
	if (zend_is_true(r1)) {
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(this_ptr, "seek", index, PHALCON_NO_CHECK);
		
		PHALCON_ALLOC_ZVAL_MM(r2);
		PHALCON_CALL_METHOD(r2, this_ptr, "valid", PHALCON_NO_CHECK);
		if (Z_TYPE_P(r2) != IS_BOOL || (Z_TYPE_P(r2) == IS_BOOL && Z_BVAL_P(r2))) {
			PHALCON_ALLOC_ZVAL_MM(r3);
			PHALCON_CALL_METHOD(r3, this_ptr, "current", PHALCON_NO_CHECK);
			RETURN_DZVAL(r3);
		} else {
			PHALCON_MM_RESTORE();
			RETURN_FALSE;
		}
	}
	PHALCON_THROW_EXCEPTION_STR(phalcon_model_exception_ce, "The index does not exist in the cursor");
	return;
}

/**
 * Resulsets cannot be changed. It has only been implemented to meet the definition of the ArrayAccess interface
 *
 * @param int $index
 * @param Phalcon_Model_Base $value
 */
PHP_METHOD(Phalcon_Model_Resultset, offsetSet){

	zval *index = NULL, *value = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &index, &value) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_THROW_EXCEPTION_STR(phalcon_model_exception_ce, "Cursor is an immutable ArrayAccess object");
	return;
}

/**
 * Resulsets cannot be changed. It has only been implemented to meet the definition of the ArrayAccess interface
 *
 * @param int $offset
 */
PHP_METHOD(Phalcon_Model_Resultset, offsetUnset){

	zval *offset = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &offset) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_THROW_EXCEPTION_STR(phalcon_model_exception_ce, "Cursor is an immutable ArrayAccess object");
	return;
}

/**
 * Get first row in the resultset
 *
 * @return Phalcon_Model_Base
 */
PHP_METHOD(Phalcon_Model_Resultset, getFirst){

	zval *r0 = NULL, *r1 = NULL;

	PHALCON_MM_GROW();
	PHALCON_CALL_METHOD_NORETURN(this_ptr, "rewind", PHALCON_NO_CHECK);
	
	PHALCON_ALLOC_ZVAL_MM(r0);
	PHALCON_CALL_METHOD(r0, this_ptr, "valid", PHALCON_NO_CHECK);
	if (Z_TYPE_P(r0) != IS_BOOL || (Z_TYPE_P(r0) == IS_BOOL && Z_BVAL_P(r0))) {
		PHALCON_ALLOC_ZVAL_MM(r1);
		PHALCON_CALL_METHOD(r1, this_ptr, "current", PHALCON_NO_CHECK);
		RETURN_DZVAL(r1);
	}
	
	PHALCON_MM_RESTORE();
	RETURN_FALSE;
}

/**
 * Get last row in the resultset
 *
 * @return Phalcon_Model_Base
 */
PHP_METHOD(Phalcon_Model_Resultset, getLast){

	zval *r0 = NULL, *r1 = NULL, *r2 = NULL, *r3 = NULL;
	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(r0);
	PHALCON_CALL_METHOD(r0, this_ptr, "count", PHALCON_NO_CHECK);
	PHALCON_INIT_VAR(t0);
	ZVAL_LONG(t0, 1);
	PHALCON_ALLOC_ZVAL_MM(r1);
	sub_function(r1, r0, t0 TSRMLS_CC);
	PHALCON_CALL_METHOD_PARAMS_1_NORETURN(this_ptr, "seek", r1, PHALCON_NO_CHECK);
	
	PHALCON_ALLOC_ZVAL_MM(r2);
	PHALCON_CALL_METHOD(r2, this_ptr, "valid", PHALCON_NO_CHECK);
	if (Z_TYPE_P(r2) != IS_BOOL || (Z_TYPE_P(r2) == IS_BOOL && Z_BVAL_P(r2))) {
		PHALCON_ALLOC_ZVAL_MM(r3);
		PHALCON_CALL_METHOD(r3, this_ptr, "current", PHALCON_NO_CHECK);
		RETURN_DZVAL(r3);
	}
	
	PHALCON_MM_RESTORE();
	RETURN_FALSE;
}

/**
 * Tell if the resultset if fresh or an old cached
 *
 * @return boolean
 */
PHP_METHOD(Phalcon_Model_Resultset, isFresh){

	zval *t0 = NULL, *t1 = NULL;
	zval *r0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_type"), PHALCON_NOISY TSRMLS_CC);
	PHALCON_INIT_VAR(t1);
	ZVAL_LONG(t1, 1);
	PHALCON_ALLOC_ZVAL_MM(r0);
	is_equal_function(r0, t0, t1 TSRMLS_CC);
	
	RETURN_NCTOR(r0);
}

/**
 * Serializing a resultset will dump all related rows into a big array
 *
 * @return string
 */
PHP_METHOD(Phalcon_Model_Resultset, serialize){

	zval *records = NULL, *data = NULL;
	zval *a0 = NULL, *a1 = NULL;
	zval *r0 = NULL, *r1 = NULL, *r2 = NULL;
	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(a0);
	array_init(a0);
	PHALCON_CPY_WRT(records, a0);
	PHALCON_CALL_METHOD_NORETURN(this_ptr, "rewind", PHALCON_NO_CHECK);
	ws_fd08_1:
		
		PHALCON_INIT_VAR(r0);
		PHALCON_CALL_METHOD(r0, this_ptr, "valid", PHALCON_NO_CHECK);
		if (Z_TYPE_P(r0) != IS_BOOL || (Z_TYPE_P(r0) == IS_BOOL && !Z_BVAL_P(r0))) {
			goto we_fd08_1;
		}
		PHALCON_INIT_VAR(r1);
		PHALCON_CALL_METHOD(r1, this_ptr, "current", PHALCON_NO_CHECK);
		phalcon_array_append(&records, r1, PHALCON_SEPARATE_PLZ TSRMLS_CC);
		PHALCON_CALL_METHOD_NORETURN(this_ptr, "next", PHALCON_NO_CHECK);
		goto ws_fd08_1;
	we_fd08_1:
	
	PHALCON_INIT_VAR(a1);
	array_init(a1);
	
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_cache"), PHALCON_NOISY TSRMLS_CC);
	phalcon_array_update_string(&a1, SL("cache"), &t0, PHALCON_SEPARATE_PLZ, PHALCON_COPY, PHALCON_NO_CTOR TSRMLS_CC);
	phalcon_array_update_string(&a1, SL("rows"), &records, PHALCON_SEPARATE_PLZ, PHALCON_COPY, PHALCON_NO_CTOR TSRMLS_CC);
	PHALCON_CPY_WRT(data, a1);
	
	PHALCON_ALLOC_ZVAL_MM(r2);
	PHALCON_CALL_FUNC_PARAMS_1(r2, "serialize", data);
	RETURN_DZVAL(r2);
}

/**
 * Unserializing a resultset will allow to only works on the rows present in the saved state
 *
 * @param string $data
 */
PHP_METHOD(Phalcon_Model_Resultset, unserialize){

	zval *data = NULL, *resultset = NULL;
	zval *r0 = NULL, *r1 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &data) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	phalcon_update_property_long(this_ptr, SL("_type"), 0 TSRMLS_CC);
	
	PHALCON_INIT_VAR(resultset);
	PHALCON_CALL_FUNC_PARAMS_1(resultset, "unserialize", data);
	if (Z_TYPE_P(resultset) == IS_ARRAY) { 
		PHALCON_ALLOC_ZVAL_MM(r0);
		phalcon_array_fetch_string(&r0, resultset, SL("rows"), PHALCON_NOISY TSRMLS_CC);
		phalcon_update_property_zval(this_ptr, SL("_rows"), r0 TSRMLS_CC);
		
		PHALCON_ALLOC_ZVAL_MM(r1);
		phalcon_array_fetch_string(&r1, resultset, SL("cache"), PHALCON_NOISY TSRMLS_CC);
		phalcon_update_property_zval(this_ptr, SL("_cache"), r1 TSRMLS_CC);
	} else {
		PHALCON_THROW_EXCEPTION_STR(phalcon_model_exception_ce, "Invalid serialization data");
		return;
	}
	
	PHALCON_MM_RESTORE();
}

/**
 * Returns the associated cache for the resultset
 *
 * @return Phalcon_Cache_Backend
 */
PHP_METHOD(Phalcon_Model_Resultset, getCache){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_cache"), PHALCON_NOISY TSRMLS_CC);
	
	RETURN_CHECK_CTOR(t0);
}

/**
 * Returns an instance of the model that is used to generate each of the results
 *
 * @return Phalcon_Model_Base
 */
PHP_METHOD(Phalcon_Model_Resultset, getSourceModel){

	zval *t0 = NULL;

	PHALCON_MM_GROW();
	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_model"), PHALCON_NOISY TSRMLS_CC);
	
	RETURN_CHECK_CTOR(t0);
}

