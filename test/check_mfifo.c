#include "../src/mfile.c"
#include <check.h>
#include <stdio.h>

#define nom1 "mfifoNommeUnitTest_1"
#define nom1_shm "/mfifoNommeUnitTest_1"
#define nom2 "mfifoNommeUnitTest_2"
#define cap 1500
#define perm 0777
#define msg1 "Ceci est un message de test d'écriture (numéro 1)."
#define msg2 "Second test d'écriture"
#define msg3 "Troisieme test d'écriture"


/**
 teste la creation d'un objet mfifo
*/
START_TEST(test_mfifo_creation){
	mfifo * fifo1;// setup

	// test oracle
	fifo1 = mfifo_connect(nom1, O_CREAT|O_EXCL, perm, cap);
	if( fifo1 == NULL){
		ck_abort_msg("creation exclusive est nulle");
	}
	//ck_assert_msg( fifo1 != NULL, "la creation exclusive est nulle");
	
	ck_assert_str_eq(fifo1->nom, nom1_shm);
	ck_assert_int_eq(fifo1->capacity,cap);
	ck_assert_int_eq(fifo1->pid,-1);
	ck_assert_int_eq(strlen(fifo1->memory),0);
	//ck_assert( fifo1->memory == NULL);


	fifo1 = mfifo_connect(nom1, O_CREAT|O_EXCL, perm, cap);
	if( fifo1 != NULL){
		ck_abort_msg("creation exclusive, valeur attendue : NULL");
	}
}
END_TEST

/**
 teste la connexion d'un objet mfifo
*/
START_TEST(test_mfifo_connexion){
	mfifo * fifo1;mfifo * fifo2; //setup
	
	// test oracle
	fifo1 = mfifo_connect(nom1,O_CREAT,perm,cap);
	ck_assert_msg( fifo1 != NULL, "connexion O_CREAT");
	ck_assert_str_eq(fifo1->nom, nom1_shm);
	ck_assert_int_eq(fifo1->capacity,cap);


	fifo1 = mfifo_connect(nom1,0,perm,cap);
	ck_assert_msg( fifo1 != NULL, "connexion 0");
	ck_assert_str_eq(fifo1->nom, nom1_shm);
	ck_assert_int_eq(fifo1->capacity,cap);
	

	fifo2 = NULL;
	fifo2 = mfifo_connect(nom2,O_CREAT,perm,cap);
	ck_assert_msg( fifo2 != NULL,
	"connexion O_CREAT, valeur attendue : NULL,\nmais valeur trouvée: %p .",
	fifo2);


	// on suppprime le fifo 2
	mfifo_unlink(fifo2->nom);
	mfifo_disconnect(fifo2);

	// on test la connexion a un fifo inexistant 
	fifo2 = mfifo_connect(nom2,0,perm,cap);
	ck_assert_msg( fifo2 == NULL,
	"connexion 0, valeur attendue : NULL,\nmais valeur trouvée: %p .",
	fifo2);

}
END_TEST

/**
 teste l'ecriture dans un objet fifo
*/
START_TEST(test_mfifo_ecriture){
	mfifo * fifo1; int mem_utilisee; char *test_msg; //setup

	fifo1 = mfifo_connect(nom1, O_CREAT, perm, cap);
	if(fifo1 == NULL){
		ck_abort_msg("La connexion a échoué dans le test d'ecriture ?!");
	}

	mem_utilisee = mfifo_free_memory(fifo1);
	if( mem_utilisee < 0 || mem_utilisee > fifo1->capacity){
		ck_abort_msg("la taille de la mémoire utilisée est incohérente");
	}

	// test oracle


	int rs = mfifo_write(fifo1, msg1, cap +1 );
	// buffer de taille supérieur à la capacité du fifo
	ck_assert_int_eq(rs, -1);

	rs = mfifo_write(fifo1, msg1, strlen(msg1));
	// cas d'écriture qui est censé de fonctionner
	ck_assert_int_eq(rs, 0);

	ck_assert_int_eq(mfifo_free_memory(fifo1),
	(mem_utilisee - strlen(msg1)) );

	test_msg = strstr(fifo1->memory, msg1);
	ck_assert_msg( test_msg != NULL,
	"Le message écrit n'a pas été trouvé en mémoire");


	/* test du try write */

	rs = mfifo_trywrite(fifo1, msg2, cap +1);
	// buffer de taille supérieur à la capacité du fifo
	ck_assert_int_eq(rs, -1);

	rs = mfifo_trywrite(fifo1, msg2, mfifo_free_memory(fifo1)+1);
	// buffer de taille supérieur à la place libre dans le fifo
	ck_assert_int_eq(rs, -1);

	rs = mfifo_trywrite(fifo1, msg2, strlen(msg2));
	// cas d'écriture qui est censé fonctionné
	ck_assert_int_eq(rs, 0);
	test_msg = strstr(fifo1->memory, msg2);
	ck_assert_msg( test_msg != NULL,
	"Le try write n'a pas réussi à écrire dans le fifo");


	/* AJOUTER LES AUTRES TEST D ECRITURE*/

	rs = mfifo_write_partial(fifo1, msg2, -1);
	// buffer de taille négative
	ck_assert_int_eq(rs, -1);
}
END_TEST

/**
 teste la lecture d'un objet mfifo
*/
START_TEST(test_mfifo_lecture){
	mfifo * fifo1;//setup

	fifo1 = mfifo_connect(nom1, 0, perm, cap);
	if(fifo1 == NULL){
		ck_abort_msg("La connexion a échoué dans le test de lecture ?!");
	}

	int mem_utilisee = mfifo_free_memory(fifo1);
	if( mem_utilisee < 0 || mem_utilisee > fifo1->capacity){
		ck_abort_msg("la taille de la mémoire utilisée est incohérente");
	}

	char * buf = malloc(sizeof(char)*strlen(msg1));

	// test oracle

	int rs = mfifo_read(fifo1, buf, -1);
	// cas de lecture avec nombre d'octet négatif
	ck_assert_int_eq(rs,-1);

	rs = mfifo_read(fifo1, buf, strlen(msg1));
	// cas de lecture qui est censé fonctionner
	ck_assert_int_eq(rs, strlen(msg1));
	ck_assert_str_eq(buf,msg1);
	ck_assert_int_eq(mfifo_free_memory(fifo1),
	(mem_utilisee + strlen(msg1)) );
}
END_TEST


/**
 teste de suppression d'un objet fifo avec : unlink est disconnect
*/
START_TEST(test_mfifo_suppression){
	mfifo * fifo1; //setup
	fifo1 = mfifo_connect(nom1, O_CREAT, perm, cap);

	if(fifo1 == NULL){
		ck_abort_msg("La connexion a échoué dans le test de suppression ?!");
	}

	// test
	ck_assert(mfifo_unlink(fifo1->nom) != -1);
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
	suite_add_tcase(s,tc_ecriture);
	suite_add_tcase(s,tc_lecture);
	suite_add_tcase(s,tc_suppression);	

	return s;
}




int main(int argc, char const *argv[]){
 	/* code */
	Suite *s ;SRunner * sr;int failures;

	s = mfifo_nommee_suite();
	sr = srunner_create(s);

	srunner_run_all(sr,CK_VERBOSE);
	failures = srunner_ntests_failed(sr);
	srunner_free(sr);

 	return (failures == 0) ? EXIT_SUCCESS :EXIT_FAILURE;;
 }