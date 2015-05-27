
#ifndef ADRS_FUNC_H
#define ADRS_FUNC_H

#define MAX_CHARS_WORD 256
#define MAX_CHARS_LINE 1028


/* Global Variables declaration */
typedef enum type_is {brute, genetic, sa, ant}type_is;


/* Design structure declaration */
typedef struct cdesign_struct{


  int m_area;
  float m_latency;

  struct cdesign_struct *m_prevp;
  struct cdesign_struct *m_nextp;

}CDesign;


/* functions declarations */


/* A */
void adrs();



/* B */


/* C */
void comp_adrs(CDesign *, CDesign *, float *, float*);

/* D */

void delete_data();
void delete_designs(CDesign *);


/* E */


/* F */


/* G */



/* I */
void insert_design_list(CDesign*, CDesign *);




/* P */
void print_version();

/* Q */


/* R */ 
void read_input_arguments(int argc, char *argv[]);
void read_log(char *);
void results_logout(float, float, float, float, float, float);

/* S */


/* V */



#endif
