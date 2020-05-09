#include <check.h>
#include "../src/mfile.h"

#define nom1 "mfifoNommeUnitTest_1"
#define nom2 "mfifoNommeUnitTest_2"
#define cap 1500
#define perm 0777
#define msg1 "Ceci est un message de test d'écriture (numéro 1)."


/**
 teste la creation d'un objet mfifo
*/
START_TEST(test_mfifo_creation){
	mfifo * fifo1;// setup

	// test oracle
	fifo1 = mfifo_connect(nom1, O_CREAT|O_EXCL, perm, cap);
	ck_assert_msg( fifo1 == NULL, "creation exclusive");
	
	ck_assert_str_eq(fifo1->nom, nom1);
	ck_assert_int_eq(fifo1->capacity,cap);
	ck_assert_int_eq(fifo1->pid,-1);
	ck_assert_int_eq(mfifo_free_memory(fifo1),fifo1->capacity);


	fifo1 = mfifo_connect(nom1, O_CREAT|O_EXCL, perm, cap);
	ck_assert_msg( fifo1 != NULL,
	"creation exclusive, valeur attendue : NULL,\nmais valeur trouvée: %p .",
	fifo1);
	
	free_mfifo(fifo1);
}
END_TEST

/**
 teste la connexion d'un objet mfifo
*/
START_TEST(test_mfifo_connexion){
	mfifo * fifo1;mfifo * fifo2; //setup
	
	// test oracle
	fifo1 = mfifo_connect(nom1,O_CREAT,perm,cap);
	ck_assert_msg( fifo1 == NULL, "connexion O_CREAT");
	ck_assert_str_eq(fifo1->nom, nom1);
	ck_assert_int_eq(fifo1->capacity,cap);


	fifo1 = mfifo_connect(nom1,0,perm,cap);
	ck_assert_msg( fifo1 == NULL, "connexion 0");
	ck_assert_str_eq(fifo1->nom, nom1);
	ck_assert_int_eq(fifo1->capacity,cap);

	fifo2 = NULL;
	fifo2 = mfifo_connect(nom2,O_CREAT,perm,cap);
	ck_assert_msg( fifo2 != NULL,
	"connexion O_CREAT, valeur attendue : NULL,\nmais valeur trouvée: %p .",
	fifo2);

	fifo2 = NULL;
	fifo2 = mfifo_connect(nom2,0,perm,cap);
	ck_assert_msg( fifo2 != NULL,
	"connexion 0, valeur attendue : NULL,\nmais valeur trouvée: %p .",
	fifo2);

	free_mfifo(fifo1);
}
END_TEST

/**
 teste l'ecriture dans un objet fifo
*/
START_TEST(test_mfifo_ecriture){
	mfifo * fifo1; int mem_utilisee; char *test_msg; //setup

	fifo1 = mfifo_connect(nom1, O_CREAT, perm, cap);
	if(fifo1 == NULL){
		ck_abort_msg("La connexion a échoué dans le test d'ecriture ??!!");
	}
	int mem_utilisee = mfifo_free_memory(fifo1);
	if( mem_utilisee < 0 || mem_utilisee > fifo1->capacity){
		ck_abort_msg("la taille de la mémoire utilisée est incohérente");
	}

	// test oracle
	mfifo_write(fifo1, msg1, strlen(msg1));
	ck_assert_int_eq(mfifo_free_memory(fifo1),
	(mem_utilisee + strlen(msg1)) );

	test_msg = strstr(fifo->memory,msg1);
	ck_assert_msg( test_msg == NULL,
	"Le message écrit n'a pas été trouvé en mémoire");
}
END_TEST

/**
 teste la lecture d'un objet mfifo
*/
START_TEST(test_mfifo_lecture){
	mfifo * fifo1; char * buf;//setup

	fifo1 = mfifo_connect(nom1, 0, perm, cap);
	if(fifo1 == NULL){
		ck_abort_msg("La connexion a échoué dans le test de lecture ??!!");
	}
	int mem_utilisee = mfifo_free_memory(fifo1);
	if( mem_utilisee < 0 || mem_utilisee > fifo1->capacity){
		ck_abort_msg("la taille de la mémoire utilisée est incohérente");
	}
	buf = malloc(strlen(msg1));

	// test oracle
	mfifo_read(fifo1, buf , strlen(buf));
	ck_assert_str_eq(buf,msg1);
	ck_assert_int_eq(mfifo_free_memory(fifo1),
	(mem_utilisee - strlen(msg1)) );
}
END_TEST


/**
 teste de suppression d'un objet fifo avec : unlink est disconnect
*/
START_TEST(test_mfifo_suppression){
	mfifo * fifo1; //setup
	fifo1 = mfifo_connect(nom1, O_CREAT, perm, cap);

	if(fifo1 == NULL){
		ck_abort_msg("La connexion a échoué dans le test de suppression ??!!");
	}

	// test
	ck_assert(mfifo_unlink(fifo1) == -1);
	ck_assert_int_eq(mfifo_disconnect(fifo1), 0);

}
END_TEST

Suite * mfifo_nommee_suite(void){
	TCase *tc_connect = tcase_create("Connect");
	tcase_add_test(tc_connect,test_mfifo_creation);
	tcase_add_test(tc_connect,test_mfifo_connexion);

	TCase *tc_lecture = tcase_create("Lecture");
	tcase_add_test(tc_lecture,test_mfifo_lecture);

	TCase *tc_ecriture = tcase_create("Ecriture");
	tcase_add_test(tc_ecriture,test_mfifo_ecriture);

	TCase *tc_suppression = tcase_create("Suppression");
	tcase_add_test(tc_suppression,test_mfifo_suppression);

	Suite *s = suite_create("Mfifo_Nommee");
	suite_add_tcase(s,tc_connect);
	suite_add_tcase(s,tc_lecture);
	suite_add_tcase(s,tc_ecriture);
	suite_add_tcase(s,tc_suppression);	

	return s;
}





int main(int argc, char const *argv[]){
 	/* code */
	Suite *s ;SRunner * sr;int failures;

	s = mfifo_nommee_suite();
	sr = srunner_create(s);

	srunner_run_all(sr,CK_VERBOSE);
	failures = srunner_n_tests_failed(sr);
	srunner_free(sr);

 	return (failures == 0) ? EXIT_SUCCESS :EXIT_FAILURE;;
 }