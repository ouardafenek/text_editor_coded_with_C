#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <assert.h>

#include "buffer.h"

Buffer* buffer_creation;
Buffer* buffer_text;

void setup_creation(){
	buffer_creation = newBuffer(10);
}

void teardown_creation(){
	freeBuffer(buffer_creation);
}

void setup_add_text(){
	buffer_text= newBuffer(10);
	addTextBuffer(buffer_text, "Bonjour\nHello, world!\nFin du buffer");
}

void teardown_add_text(){
	freeBuffer(buffer_text);
}


// ---------- TEST : buffer_creation ----------
START_TEST(test_creation){
	Buffer* buffer = newBuffer(10);

	ck_assert(buffer != NULL);
	free(buffer);

}
END_TEST


START_TEST(test_creation_vide){
	Buffer* buffer = newBuffer(10);

	ck_assert(strcmp(getTextBuffer(buffer), "") == 0);
	free(buffer);

}
END_TEST

START_TEST(test_creation_valeur_defaut){
	int nb_line_max = 10;
	Buffer* buffer = newBuffer(nb_line_max);

	ck_assert_int_eq(getCurLineBuffer(buffer), 0);
	ck_assert_int_eq(getCurColBuffer(buffer), 0);
	ck_assert_int_eq(getCurSelectLineBuffer(buffer), 0);
	ck_assert_int_eq(getCurSelectColBuffer(buffer), 0);
	ck_assert_int_eq(getNbLineBuffer(buffer), 1);

	free(buffer);

}
END_TEST


// ---------- TEST : buffer_add_text ----------
START_TEST(test_add_text){
	char* text = "Bonjour !";
	addTextBuffer(buffer_creation, text);
	ck_assert(strcmp(getTextBuffer(buffer_creation), text) == 0);
}
END_TEST

START_TEST(test_add_text_twice){
	char* text1 = "Hello, ";
	char* text2 = "World!";

	char* text_1_2= calloc(strlen(text1) + strlen(text2) + 1, 1);
	strcat(text_1_2, text1);
	strcat(text_1_2, text2);

	addTextBuffer(buffer_creation, text1);
	addTextBuffer(buffer_creation, text2);

	ck_assert(strcmp(getTextBuffer(buffer_creation), text_1_2) == 0);
}
END_TEST

START_TEST(test_add_text_new_line){
	char* text_multiline = "Bonjour !\nHello, world!\nDernière ligne";
	addTextBuffer(buffer_creation, text_multiline);

	ck_assert(strcmp(getTextBuffer(buffer_creation), text_multiline) == 0);
	ck_assert(strcmp(getLineBuffer(buffer_creation, 0), "Bonjour !") == 0);
	ck_assert(strcmp(getLineBuffer(buffer_creation, 1), "Hello, world!") == 0);
	ck_assert(strcmp(getLineBuffer(buffer_creation, 2), "Dernière ligne") == 0);
}
END_TEST


// ---------- test_remove_text ----------
START_TEST(test_remove_text){
	removeCharBuffer(buffer_text, 7);

	ck_assert_int_eq(getNbLineBuffer(buffer_text), 3);
	ck_assert(strcmp(getLineBuffer(buffer_text, 2), "Fin du") == 0);
}
END_TEST

START_TEST(test_remove_text_new_line){
	removeCharBuffer(buffer_text, 14);

	ck_assert_int_eq(getNbLineBuffer(buffer_text), 2);
	ck_assert(strcmp(getLineBuffer(buffer_text, 1), "Hello, world!") == 0);
}
END_TEST

START_TEST(test_remove_text_debut){
	moveCursorPositionBuffer(buffer_text, 0, 1);
	removeCharBuffer(buffer_text, 3);
	
	ck_assert_int_eq(getNbLineBuffer(buffer_text), 3);
	ck_assert(strcmp(getLineBuffer(buffer_text, 0), "onjour") == 0);
}
END_TEST


// ---------- test_selection ----------
START_TEST(test_selection){
	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_LEFT, 5);
	ck_assert_int_eq(getCurSelectLineBuffer(buffer_text), 2);
	ck_assert_int_eq(getCurSelectColBuffer(buffer_text), 8);
}
END_TEST

START_TEST(test_selection_line){
	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_UP, 1);
	ck_assert_int_eq(getCurSelectLineBuffer(buffer_text), 1);
	ck_assert_int_eq(getCurSelectColBuffer(buffer_text), 13);

	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_UP, 1);
	ck_assert_int_eq(getCurSelectLineBuffer(buffer_text), 0);
	ck_assert_int_eq(getCurSelectColBuffer(buffer_text), 7);

	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_RIGHT, 1);
	ck_assert_int_eq(getCurSelectLineBuffer(buffer_text), 1);
	ck_assert_int_eq(getCurSelectColBuffer(buffer_text), 0);
}
END_TEST

START_TEST(test_selection_remove){
	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_LEFT, 5);
	removeCharBuffer(buffer_text, 1);

	ck_assert_int_eq(getNbLineBuffer(buffer_text), 3);
	ck_assert(strcmp(getLineBuffer(buffer_text, 2), "Fin du b") == 0);
}
END_TEST

START_TEST(test_selection_remove_line){
	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_UP, 1);
	removeCharBuffer(buffer_text, 1);

	ck_assert_int_eq(getNbLineBuffer(buffer_text), 2);
	ck_assert(strcmp(getLineBuffer(buffer_text, 1), "Hello, world!") == 0);
}
END_TEST

START_TEST(test_selection_get_select){
	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_LEFT, 5);

	ck_assert(strcmp(getSelectionZoneBuffer(buffer_text), "uffer") == 0);
}
END_TEST

START_TEST(test_selection_add_text){
	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_LEFT, 5);
	addTextBuffer(buffer_text, "allon");

	ck_assert(strcmp(getLineBuffer(buffer_text, 2), "Fin du ballon") == 0);
}
END_TEST


// ---------- test_copy_paste ----------
START_TEST(test_copy_paste_copy){
	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_LEFT, 5);
	copyBuffer(buffer_text);

	ck_assert(strcmp(getClipBoardBuffer(buffer_text), "uffer") == 0);
}
END_TEST

START_TEST(test_copy_paste_no_copy){
	ck_assert(strcmp(getClipBoardBuffer(buffer_text), "") == 0);
}
END_TEST

START_TEST(test_copy_paste_paste){
	moveCursorSelectBuffer(buffer_text, BUFFER_CUR_LEFT, 5);
	copyBuffer(buffer_text);
	moveCursorPositionBuffer(buffer_text, 0, 0);
	pasteBuffer(buffer_text);

	ck_assert(strcmp(getLineBuffer(buffer_text, 0), "ufferBonjour") == 0);
	ck_assert_int_eq(getCurColBuffer(buffer_text), 5);
}
END_TEST


int main(){

	Suite* suite;
	SRunner* runner;

	suite = suite_create("BUFFER");

	TCase* t_case_creation = tcase_create("buffer_creation");
	tcase_add_test(t_case_creation, test_creation);
	tcase_add_test(t_case_creation, test_creation_vide);
	tcase_add_test(t_case_creation, test_creation_valeur_defaut);

	suite_add_tcase(suite, t_case_creation);


	TCase* t_case_add_text = tcase_create("buffer_add_text");
	tcase_add_checked_fixture(t_case_add_text, setup_creation, teardown_creation);
	tcase_add_test(t_case_add_text, test_add_text);
	tcase_add_test(t_case_add_text, test_add_text_twice);
	tcase_add_test(t_case_add_text, test_add_text_new_line);

	suite_add_tcase(suite, t_case_add_text);


	TCase* t_case_remove_text = tcase_create("buffer_remove_text");
	tcase_add_checked_fixture(t_case_remove_text, setup_add_text, teardown_add_text);
	tcase_add_test(t_case_remove_text, test_remove_text);
	tcase_add_test(t_case_remove_text, test_remove_text_new_line);
	tcase_add_test(t_case_remove_text, test_remove_text_debut);

	suite_add_tcase(suite, t_case_remove_text);


	TCase* t_case_selection_text = tcase_create("buffer_selection");
	tcase_add_checked_fixture(t_case_selection_text, setup_add_text, teardown_add_text);
	tcase_add_test(t_case_selection_text, test_selection);
	tcase_add_test(t_case_selection_text, test_selection_line);
	tcase_add_test(t_case_selection_text, test_selection_remove);
	tcase_add_test(t_case_selection_text, test_selection_remove_line);
	tcase_add_test(t_case_selection_text, test_selection_get_select);
	tcase_add_test(t_case_selection_text, test_selection_add_text);

	suite_add_tcase(suite, t_case_selection_text);


	TCase* t_case_copy_paste = tcase_create("buffer_copy_paste");
	tcase_add_checked_fixture(t_case_copy_paste, setup_add_text, teardown_add_text);
	tcase_add_test(t_case_copy_paste, test_copy_paste_copy);
	tcase_add_test(t_case_copy_paste, test_copy_paste_no_copy);
	tcase_add_test(t_case_copy_paste, test_copy_paste_paste);

	runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);

	return (srunner_ntests_failed(runner) == 0 ? 0 : 1);
}
