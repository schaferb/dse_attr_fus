/*************************************************************************************
 **
 ** genetic.c
 **
 ** version 1.0 created by : BCS
 ** Date : April 11, 2014
 ** Comments: Attribute HLS Design Space Explorer using a Genetic Algorithm 
 **
 ************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit.h>

#include "dse.h"

CDesign *first_design = NULL;
int count_designs=1;
double c0=0, c1=0,c2=0, c3=0, c4=0, c5=0, cov00, cov01, cov11, chisq=0;


/*********************************************************
 **
 ** gen_predictive_multi
 **
 ** Create a predictive model to estimate the CWB area with multi variables
 **
 *********************************************************/
void gen_predictive_multi(CDesign *first_design){

  // Variables declaration
  CDesign *design= NULL;
  int i=0, n, p, designs_ok=0;

  gsl_matrix *x, *cov;
  gsl_vector *y, *c;
  gsl_multifit_linear_workspace *work;  

  designs_ok = count_designs_ok(first_design);
  if(designs_ok < min_designs_pred)
    return;

  n = designs_ok;
  p = 5+1; //area FUS, REG, dec, misc, mux + constant

  x = gsl_matrix_alloc(n,p);
  y = gsl_vector_alloc(n);

  c = gsl_vector_alloc(p);
  cov = gsl_matrix_alloc(p,p);

  // Enter data
  for(design=first_design;design;design=design->m_nextp){
    if(design->m_area_ise > 0){
      gsl_matrix_set(x, i, 0, 1.0);
      gsl_matrix_set(x, i, 1, design->m_area_fu);
      gsl_matrix_set(x, i, 2, design->m_area_reg);
      gsl_matrix_set(x, i, 3, design->m_area_mux);
      gsl_matrix_set(x, i, 4, design->m_area_dec);
      gsl_matrix_set(x, i, 5, design->m_area_misc);
      i++;
    }
  }

   // Enter Y data
  i=0;
  for(design=first_design;design;design=design->m_nextp){
    if(design->m_area_ise > 0){
      gsl_vector_set(y, i , design->m_area_ise);
      i++;
    }
  }






  // Allocate temporary work space for gsl
   work = gsl_multifit_linear_alloc(n, p);

  // Do the fitting
  gsl_multifit_linear(x, y, c, cov, &chisq, work);
  //  gsl_multifit_linear_free (work);

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

  c0 = C(0);
  c1 = C(1);
  c2 = C(2);
  c3 = C(3);
  c4 = C(4);
  c5 = C(5);

  // de-allocate memory
  //  gsl_matrix_free(x);
  //  gsl_matrix_free(cov);
  // gsl_vector_free(y);
             //gsl_vector_free(c);



}

/*********************************************************
 **
 ** gen_predictive
 **
 ** Create a predictive model to estimate the CWB area
 **
 *********************************************************/
void gen_predictive(CDesign *first_design){

  // Variables declaration
/*   CDesign *design = NULL; */
/*   double *x, *y;// *w; */
/*   // gsl_vector *x, *y; */
/*   int designs_ok=0, i=0; */

/*   designs_ok = count_designs_ok(first_design); */
/*   if(designs_ok < min_designs_pred) */
/*     return; */

/*    x=(double*)malloc(designs_ok*sizeof(int)); */
/*    y=(double*)malloc(designs_ok*sizeof(int)); */

/*   //x = gsl_vector_alloc(designs_ok); */
/*   //y = gsl_vector_alloc(designs_ok); */
  
/* //  w=(double*)malloc(designs_ok*sizeof(int)); */

/*   for(design=first_design;design;design=design->m_nextp){ */
/*     if(design->m_area_ise > 0){ */
/*        x[i]=design->m_area; */
/*        y[i]=design->m_area_ise; */
      
/*       //gsl_vector_set(x, i, design->m_area); */
/*       //gsl_vector_set(y, i, design->m_area_ise); */

/*       i++; */

/*     } */
/*   } */

/*   gsl_fit_linear(x, 1, y, 1, i,  */
/*                    &c0, &c1, &cov00, &cov01, &cov11,  */
/*                    &chisq); */

/*   printf ("# best fit: Y = %g + %g X\n", c0, c1); */
/*   printf ("# covariance matrix:\n"); */
/*   printf ("# [ %g, %g\n#   %g, %g]\n",  */
/*           cov00, cov01, cov01, cov11); */
/*   printf ("# chisq = %g\n", chisq); */

  //gsl_vector_free(x);
  //gsl_vector_free(y);
  //free(x);
  // free(y);


  // Variables declaration
  CDesign *design= NULL;
  int i=0, n, p, designs_ok=0;

  gsl_matrix *x, *cov;
  gsl_vector *y, *c;
  gsl_multifit_linear_workspace *work;  

  designs_ok = count_designs_ok(first_design);
  if(designs_ok < min_designs_pred)
    return;

  n = designs_ok;
  p = 2+1; //area CWB, area CWB^2 + constant

  x = gsl_matrix_alloc(n,p);
  y = gsl_vector_alloc(n);

  c = gsl_vector_alloc(p);
  cov = gsl_matrix_alloc(p,p);

  // Enter data
  for(design=first_design;design;design=design->m_nextp){
    if(design->m_area_ise > 0){
      gsl_matrix_set(x, i, 0, 1.0);
      gsl_matrix_set(x, i, 1, design->m_area);
      gsl_matrix_set(x, i, 2, design->m_area*design->m_area);
      i++;
    }
  }

   // Enter Y data
  i=0;
  for(design=first_design;design;design=design->m_nextp){
    if(design->m_area_ise > 0){
      gsl_vector_set(y, i , design->m_area_ise);
      i++;
    }
  }






  // Allocate temporary work space for gsl
   work = gsl_multifit_linear_alloc(n, p);

  // Do the fitting
  gsl_multifit_linear(x, y, c, cov, &chisq, work);
  //  gsl_multifit_linear_free (work);

#define C(i) (gsl_vector_get(c,(i)))
#define COV(i,j) (gsl_matrix_get(cov,(i),(j)))

  c0 = C(0);
  c1 = C(1);
  c2 = C(2);
  c3 = 0;
  c4 = 0;
  c5 = 0;

  // de-allocate memory
  //  gsl_matrix_free(x);
  //  gsl_matrix_free(cov);
  // gsl_vector_free(y);
  //gsl_vector_free(c);




}


/*********************************************************
 **
 ** count_designs_ok
 **
 **  count how many designs where generated correctly
 **
 *********************************************************/
int count_designs_ok(CDesign *first_design){

    // Variables declaration
    CDesign *design = NULL;
    int count=0;

    for(design=first_design;design;design=design->m_nextp){
      if(design->m_area_ise > 0)
	count++;
    }


    return count;

}


/*********************************************************
 **
 ** Genetic Algorithm 
 **
 ** Design space exploration based on Genetic Algorithm
 **
 *********************************************************/
void genetic_algorithm(CAttrlist *first_attr_list, int crossover, int mutation){


  /* Variables declaration */
  CDesign *parent1= NULL, *parent2 = NULL, *parentx= NULL, *offspring= NULL;
  int itr_family = 0, itr_num_families=0, num_attr=0;

  /* Count the number of total attributes */
  num_attr = count_attributes(first_attr_list);

  do{
    /* Generate parents */
    parent1 = gen_random_design(first_attr_list);
    if(parent1 == NULL)
      break;
    synthesize(parent1);

    parent2 = gen_random_design(first_attr_list);
    if(parent2 == NULL)
      break;
    synthesize(parent2);

    if(parent1 == NULL || parent2 == NULL){
      printf("\nCannot generate new random parent. Exiting");
      return;
    }

    do{
      /* Generate offspring */
      offspring = offspring_gen(parent1, parent2, crossover, mutation, num_attr);
      if(offspring == NULL)
	break;
      synthesize(offspring);

      /* Subsitute parent by offspring */
      parentx = offspring_better(parent1, parent2, offspring);

      /* if offspring is not better than parent for X times ==> generated new parents */
      if(parentx == NULL)
	itr_family ++; 
      else if(parentx == parent1){
	parent1 =offspring;
	itr_family = 0; 
      }

      else{
	parent2 = offspring;
 	itr_family = 0;
      }

      // Generate predictive models
      if(count_designs >= min_designs_pred && c0 ==0){
	if(flag_multi == false)
	  gen_predictive(first_design);
	else
	  gen_predictive_multi(first_design);
      }


    }while(itr_family < MAX_OFFSPRINGS);
 
    itr_family=0; 
    itr_num_families++;
 
   }while(itr_num_families < MAX_PARENTS_TRIES);


  /* identify the optimal designs */
  optimal_designs(first_design);

  if(fu_method == each_optimal){
    fu_dse_optimal(first_design, process);
  
    /* Re-check which designs after FU exploration are optimal */
    optimal_designs(first_design);
  }
  
  else if(fu_method == each_prob){
    fu_dse_prob(first_design, process);

    /* Re-check which designs after FU exploration are optimal */
    optimal_designs(first_design);

  }

}


/*********************************************************
 **
 ** count_attributes 
 **
 ** count the total number of attributes in the list
 **
 *********************************************************/
int count_attributes(CAttrlist *first_attr){

  /* Variables declaration */
  CAttrlist *attr;
  int count=0;

  for(attr=first_attr; attr!= NULL; attr=attr->m_nextp)
    count++;

  return count;
}



/*********************************************************
 **
 ** offspring_gen
 **
 ** checks if offsprings is better than any parent
 **
 *********************************************************/
CDesign *offspring_gen(CDesign *parent1, CDesign *parent2,int crossover, int mutation, int num_attr){


  /* Variables declaration */
  CDesign *offspring = NULL;
  int count_max = 0;


  do{

 
    /* Allocate memory for the new offspring design */
    offspring = gen_design();

    /* Perform Crossover */
    crossover_offspring(parent1, parent2, offspring, crossover, num_attr);


    /* Perform Mutation */
    mutation_offspring(offspring, mutation);

    offspring->m_hash = hash_design(offspring->m_first_attr);

    /* Check if offspring is unique */
    if(check_design_unique(first_design, offspring) == true){
      insert_design(first_design, offspring);
      break;
    }
    else{
      delete_designs(offspring);
      offspring = NULL;
      count_max++;
    }

  }while(count_max < MAX_OFFSPRINGS_TRIES);

  if(offspring == NULL){
    printf("\n**** NEW OFFSPRING could not be generated for given family");
    return NULL;
  }


  /* Return newly generated offspring */
  return offspring;

}

/*********************************************************
 **
 ** crossover_offspring
 **
 ** cross the offspring over
 **
 *********************************************************/
void crossover_offspring(CDesign *parent1, CDesign *parent2, CDesign *offspring, int crossover, int num_attr){

  /* Variables declaration */
  int probcross=0, probcopy=0, crosspoint=0;


  probcross = rand()%100;
  probcopy = rand()%2;

  /* Crossover */
  if(probcross < crossover){
    crosspoint = rand()%num_attr;
    if(crosspoint == 0)
      crosspoint++; /* if 0 is the same as copying the list */
    cross_attrs_list(parent1, parent2, offspring, crosspoint);
  }

  /* Copy one of both parents attribute lists */
  else{
    if(probcopy == 0)
      copy_attrs_list(parent1, offspring, -1);
    else
      copy_attrs_list(parent2, offspring, -1);

  }
}

/*********************************************************
**
** cross_attrs_list
**
** crossover of two attributes linked lists
**
*********************************************************/
void cross_attrs_list(CDesign *parent1, CDesign *parent2, CDesign *offspring, int crosspoint){

  /* Variables declaration */
  CAttr *attr = NULL, *first_tmp;
  int count =0;

  /* Copy first half of new attributes list */
  copy_attrs_list(parent1, offspring, crosspoint);


  /* Copy the second half of the attribute list */
  first_tmp = parent2->m_first_attr;
  for(attr = parent2->m_first_attr; attr; attr=attr->m_nextp){
    if(count == crosspoint)
      break;

    count++;
  }
 
  parent2->m_first_attr = attr;
  copy_attrs_list(parent2, offspring, -1);
  parent2->m_first_attr = first_tmp;
  
}


/*********************************************************
**
** copy_attrs_list
**
** Generate a unique new configuration
**
*********************************************************/
void copy_attrs_list(CDesign *parent, CDesign *offspring, int num){

  /* Variables declaration */
  CAttr *tattr= NULL, *tattr_new= NULL;
  int count =0;

  for(tattr= parent->m_first_attr; tattr; tattr=tattr->m_nextp){

    if(count == num)
      break;

    tattr_new = copy_attr(tattr);
    if(offspring->m_first_attr == NULL)
      offspring->m_first_attr = tattr_new;
    else
      insert_attr(offspring->m_first_attr, tattr_new);


    count ++;

  }

}

/*********************************************************
 **
 ** mutation_offspring
 **
 ** Mutation the offspring
 **
 *********************************************************/
void mutation_offspring( CDesign *offspring, int mutation){

  /* Variables declaration */
  CAttr *attr= NULL,*attr_next= NULL, *attr_new=NULL;
  int mutate=0;

  srand(time(NULL));

  for(attr=offspring->m_first_attr; attr; attr=attr_next){
    attr_next= attr->m_nextp;

    mutate = rand()%100;
    if(mutate < mutation){

      attr_new=new_attr_rnd(attr->m_list->m_first_attr); /* find attribute */

      /* insert attribute in linked list */
      if(attr->m_prevp != NULL)
	attr->m_prevp->m_nextp = attr_new;
      
      attr_new->m_prevp = attr->m_prevp;

      if(attr->m_nextp != NULL)
	attr->m_nextp->m_prevp = attr_new;

      attr_new->m_nextp = attr->m_nextp;


      if(offspring->m_first_attr  == attr)
	offspring->m_first_attr = attr_new;
      delete_single_attr(attr);
    }
  }
    
 }




/*********************************************************
 **
 ** gen_random_design
 **
 ** Generate a unique new configuration
 **
 *********************************************************/
CDesign *gen_random_design(CAttrlist *first_attr_list){

  /* Variables declaration */
  CDesign *design_new= NULL;
  int count_max = 0;

  do{
  
    /* Generate random design */
    design_new = gen_random_design_single(first_attr_list);

    /* Check if design is unique */
    if(check_design_unique(first_design, design_new) == true){
      // insert design in design linked list and break
      if(first_design == NULL)
	first_design = design_new;
      else
	insert_design(first_design, design_new);
      break;
    }
    else{
      delete_designs(design_new);
      count_max++;
    }


  }while(count_max < MAX_PARENTS_TRIES);

  if(count_max >= MAX_PARENTS_TRIES)
    return NULL;

  else
    return design_new;

}


/*********************************************************
 **
 ** check_design_unique
 **
 ** Checks if the given design has been already generated
 ** or not (attribute list)
 **
 *********************************************************/
bool check_design_unique(CDesign *first, CDesign *design_new){

  /* Variables declaration */
  CDesign *design= NULL;

  if(first == NULL)
    return true;

  for(design=first; design; design=design->m_nextp){

    if(design->m_hash == design_new->m_hash)
      return false;
  }

  return true;
}

/*********************************************************
 **
 ** insert_design
 **
 ** insert design in linked list
 **
 *********************************************************/
void insert_design(CDesign *first, CDesign *design_new){

  /* Variables declaration */
  CDesign *design = NULL;

  for(design=first;design->m_nextp; design=design->m_nextp);
  
  
  design->m_nextp = design_new;
  design_new->m_prevp = design;



}


/*********************************************************
 **
 ** gen_random_design_single
 **
 ** Generate a unique new configuration
 **
 *********************************************************/
CDesign *gen_random_design_single(CAttrlist *first_attr_list){

  /* Variables declaration */
  CDesign *design= NULL;
  CAttrlist *attr= NULL;
  CAttr *tattr_new = NULL;


  /* Allocate memory for a design */
  design = gen_design();

  srand(time(NULL));
 

  /* Generate random attributes*/
  for(attr=first_attr_list; attr; attr=attr->m_nextp){

    /* Generate a new rnd attribute */
    tattr_new = new_attr_rnd(attr->m_first_attr);

    if(design->m_first_attr == NULL)
      design->m_first_attr = tattr_new;
    else
      insert_attr(design->m_first_attr, tattr_new);

  }


  /* Compute hash value for this new design -> characterizes 
  ** the uniquness of this combination of attributes */
  design->m_hash = hash_design(design->m_first_attr);


  return design;


}


/*********************************************************
 **
 ** new_attr_rnd
 **
 ** Select a new random attribute
 **
 *********************************************************/
CAttr* new_attr_rnd(CAttr *first_attr){

  /* Variables declaration */
  CAttr *tattr = NULL, *tattr_new= NULL;;
    int num, x=0;

    num = rand()%first_attr->m_list->m_num;
    for(tattr = first_attr; tattr; tattr=tattr->m_nextp){
      if(x == num){
	tattr_new = copy_attr(tattr);
	break;
      }
      x++;
    }

    return tattr_new;

}


/*********************************************************
 **
 ** hash_design
 **
 ** computes the hash value of a given attribute list
 **
 *********************************************************/
int hash_design(CAttr *first_attr){

  /* Variables declaration */
  CAttr *attr = NULL;
  int hash = 0, pos=1;

  for(attr=first_attr; attr; attr=attr->m_nextp){
    hash += hash_attr(attr, pos);
    pos ++;
  }


  return hash;
}

/*********************************************************
 **
 ** hash_attr
 **
 ** computes the hash value of given attribute
 **
 *********************************************************/
int hash_attr(CAttr *attr, int pos){

  /* Variables declaration */
  
  int hash =0;

  hash = (hash_string(attr->m_value)*pos);
  //  hash += pos;


  return hash;

}

/*********************************************************
 **
 ** hash_string
 **
 ** computes the hash value of a given string
 **
 *********************************************************/
int hash_string(char *strg){

  /* Variables declaration */
  int x=0, val = 0;

  for(x=0; x < strlen(strg); x++){
    val += (int)strg[x];
  }

  return val;

}


/*********************************************************
 **
 ** gen_design
 **
 ** generate an empty design
 **
 *********************************************************/
CDesign *gen_design(){

  /* Variables declaration */
  CDesign *design=NULL;

  design = (CDesign*)malloc(sizeof(CDesign));

  design->m_count =count_designs;
  design->m_pareto_optimal = false;
  design->m_area = 0;
  design->m_area_fu = 0;
  design->m_area_reg = 0;
  design->m_area_mux = 0;
  design->m_area_dec = 0;
  design->m_area_misc = 0;

  design->m_area_ise = 0;
  design->m_area_pred = 0;

  design->m_latency = 0;
  design->m_hash = 0;

  design->m_prob_max=0;
  design->m_prob=0;

  design->m_first_attr = NULL;
  design->m_design_fu_single = NULL;

  design->m_prevp = NULL;
  design->m_nextp = NULL;

  count_designs ++;

  return design;
}

/*********************************************************
 **
 ** copy_attr
 **
 ** generate and copy attribute
 **
 *********************************************************/
CAttr *copy_attr(CAttr *attr){


  /* Variables declaration */
  CAttr *attr_new = NULL;

  attr_new = (CAttr *)malloc(sizeof(CAttr));
 
  attr_new->m_name = (char*)malloc(sizeof(char)*(strlen(attr->m_name)+1));
  strcpy(attr_new->m_name, attr->m_name);

  attr_new->m_value = (char*)malloc(sizeof(char)*(strlen(attr->m_value)+1));
  strcpy(attr_new->m_value, attr->m_value);

  attr_new->m_min = attr->m_min;
  attr_new->m_max = attr->m_max;
 
  attr_new->m_list = attr->m_list;
  attr_new->m_nextp = NULL;
  attr_new->m_prevp = NULL;

  return attr_new;

}


/*********************************************************
 **
 **synthesize
 **
 ** Synthesizes single design
 **
 *********************************************************/
void synthesize(CDesign *design){

  /* Variables declaration */
  char synops[MAX_CHARS_LINE];
  double yf;

  /* 
  ** ATTR FILE
  ** Create the attribute file as a head file
  */
  gen_attr_file(design);

  /* 
  ** BDLPARS
  **Parse only very first design to create .IFF file
  */
 
  if(strstr(in_file,".sc") || strstr(in_file,".cpp")) 
    sprintf(synops,"scpars %s", in_file);
  else
    sprintf(synops,"bdlpars %s -DBDL", in_file);

  system(synops);
 

  /*
  ** BDLTRAN
  ** Synthesize the design
  **
  */
  sprintf(synops,"bdltran -c%d -s -lfl %s -lb %s %s.IFF -Zflib_fcnt_out -Zmlib_mcnt_out -Zflib_out_limit=L100:M100:S100", CLOCK*100, flib, blib, process);
  system(synops);
  
  sprintf(synops,"bdltran -c%d -s -lfl %s -lb %s -lfc %s-auto.FCNT -lml %s-auto.MLIB -lmc %s-auto.MCNT %s.IFF ", CLOCK*100, flib, blib, process, process, process, process);
  system(synops);


  /* Read the results */
  read_bdltran_syn_results(design, process);

  if(design->m_area == 0)
    return;


  /* Make a new folder and store synthes results there */
  sprintf(synops,"mkdir ./dse_%s/%s%d",process,  process, design->m_count);
  system(synops);

  if(flag_logic_synth==true){
    sprintf(synops," veriloggen %s_E.IFF",  process);
    system(synops);  


    // ********
    //*** Predictive model has been created
    if(flag_predictive == true && c1 != 0){

      if(flag_multi == false){
	yf = c0+c1*design->m_area_fu+c2*(design->m_area*design->m_area);
      }
      else{
	yf = c0+c1*design->m_area_fu+c2*design->m_area_reg+c3*design->m_area_mux+c4*design->m_area_dec+c5*design->m_area_misc;
      }
    
      design->m_area_ise = (int)yf;
      design->m_area_pred = (int)yf;

  }
     
    //********
    //*** FPGA
    else if(flag_fpga_asic == false){
      // Virtex 4
      if(strstr(flib, "fpga_v4.FLIB")){
	sprintf(synops," LSscrgen %s_E.IFF -syn_tool=ise -clock_period=%dns -fpga_family=virtex4 -fpga_device=XC4VSX35 -fpga_package=FF668 -fpga_speed_grade=-10",  process, CLOCK);
	system(synops);  
      }
      // Virtex 5
      else{
	sprintf(synops," LSscrgen %s_E.IFF -syn_tool=ise -clock_period=%dns -fpga_family=virtex5 -fpga_device=XC5VLX85 -fpga_package=FF676 -fpga_speed_grade=-1",  process, CLOCK);
	system(synops);  
      }
      
      sprintf(synops," xtclsh %s_E.tcl",  process);
      system(synops); 

      read_ise_synth_results(design, process);

      sprintf(synops,"mv  *.v *.tcl *.syr ./dse_%s/%s%d  ",process,process,design->m_count);
      system(synops); 
    }

    //********
    //***  ASIC design
    else{
	sprintf(synops," LSscrgen %s_E.IFF -syn_tool=dc -clock_period=%dns",  process, CLOCK);
	system(synops);  

	sprintf(synops," dc_shell -f %s_E.tcl",  process);
      system(synops);

    }
  }


  sprintf(synops,"mv *.IFF *.MLIB *.MCNT *.CSV *.QOR  attrs.h ./dse_%s/%s%d  ",process,process,design->m_count);
  system(synops);

  sprintf(synops,"mv %s-auto.FCNT  ./dse_%s/%s%d/%s-auto%d.FCNT  ",process,process, process, design->m_count, process, design->m_count);
   system(synops);


  /* Store results in log file */
  results_single(design);


  /*********************
   **
   ** Explore the FUs for the given generated design
   ** if the FUs should be explored for each design (default mode)
   ** -fu_each_itr
   **
   ********************/
  if(fu_method == each_design){
    copy_xplore_files(design, process);
    fu_dse(design, process, false);
  }
    

}

/*********************************************************
 **
 ** gen_attr_file
 **
 ** Generate a header file with the attributes 
 **
 *********************************************************/
void gen_attr_file(CDesign *design){

  /* Variables declaration */
  FILE *ptr_file = NULL;
  CAttr *attr= NULL;
  int x=1;

  ptr_file = fopen("attrs.h", "wt");
  if(ptr_file == NULL){
    printf("\n****** ERROR : Can't generated attrs.h ");
    return;
  }

  fprintf(ptr_file,"/* Automatic DSE for CyberWorkBench ");
  fprintf(ptr_file,"\n by PolyU DARClab DSE */");
  fprintf(ptr_file,"\n");


  for(attr=design->m_first_attr; attr; attr=attr->m_nextp){
    if(attr->m_name !=0 && attr->m_value != 0)
      fprintf(ptr_file,"\n#define ATTR%d Cyber %s=%s",x, attr->m_name, attr->m_value);
    x++;
  }

  fprintf(ptr_file,"\n");
  fclose(ptr_file);


}

/*********************************************************
 **
 ** read_bdltran_syn_results
 **
 ** read the synthesis results 
 **
 *********************************************************/
void read_bdltran_syn_results(CDesign *design, char *process){

  /* Variables declaration */
  FILE *ptr_file = NULL;
  char name[MAX_CHARS_WORD], line[MAX_CHARS_LINE];
  char delims[] =",", *result = NULL;
  int x=0;

  sprintf(name, "%s.CSV", process);

  ptr_file = fopen(name, "rt");
  if(ptr_file == NULL){
    printf("\n****** ERROR : Can't open %s.CSV ",process);
    return;
  }


  if(fgets(line, MAX_CHARS_LINE,ptr_file) == NULL)
    return; // file is empty -> bdltran error
  fgets(line, MAX_CHARS_LINE,ptr_file);

  result = strtok(line, delims);
  design->m_area = atoi(result);
  
  while( result != NULL ) {
    x++;
    
    if(x==3)
      design->m_area_fu = atoi(result);
    else if(x==4)
      design->m_area_reg = atoi(result);      
    else if(x==5)
      design->m_area_mux = atoi(result);      
    else if(x==6)
      design->m_area_dec = atoi(result);      
    else if(x==12)
      design->m_area_misc = atoi(result);      

    else if(x ==19){
      design->m_latency = atoi(result);    
      break;
    }


    result = strtok( NULL, delims );
  }


 fclose(ptr_file);


}

/*********************************************************
 **
 ** read_ISE_synth_results
 **
 ** read the synthesis results 
 **
 *********************************************************/
void read_ise_synth_results(CDesign *design, char *process){

  /* Variables declaration */
  FILE *ptr_file = NULL;
  char name[MAX_CHARS_WORD], line[MAX_CHARS_LINE];
  char delims[] =" ", *result = NULL;


  sprintf(name, "%s.syr", process);

  ptr_file = fopen(name, "rt");
  if(ptr_file == NULL){
    printf("\n****** ERROR : Can't open %s.syr ",process);
    return;
  }


  while(fgets(line, MAX_CHARS_LINE, ptr_file) != NULL){
	 
    if(strstr(flib, "fpga_v4")){
      if(strstr(line, "Number of 4 input LUTs:")){
	result = strtok(line, delims);
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	design->m_area_ise += atoi(result);
	break;
      }
      else if(strstr(line, "Number of Slice Flip Flops:")){
	result = strtok(line, delims);
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	design->m_area_ise += atoi(result);
      }


    }
    else{
      if(strstr(line, "Number of Slice LUTs:")){
	result = strtok(line, delims);
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	design->m_area_ise += atoi(result);
	break;
      }
      else if(strstr(line, "Number of Slice Registers:")){
	result = strtok(line, delims);
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	result = strtok( NULL, delims );
	design->m_area_ise += atoi(result);
      }
    }
  }


 fclose(ptr_file);

}


/*********************************************************
 **
 ** offspring_better
 **
 ** checks if offsprings is better than any parent
 **
 *********************************************************/
CDesign *offspring_better(CDesign *parent1, CDesign *parent2,CDesign* offspring){


  /* Variables declaration */
  int choose=0;

  choose =   rand()%2;
  if(choose == 0){

    // Subsistute if bdltran error in any of the parent designs
    if(parent1->m_area <= 0)
      return parent1;
    else if(parent2->m_area <= 0)
      return parent2;

    else if(offspring->m_area < parent1->m_area && offspring->m_latency < offspring->m_latency)
      return parent1;
    else if(offspring->m_area < parent2->m_area && offspring->m_latency < parent2->m_latency)
      return parent2;
  }

  else{

    // Subsistute if bdltran error in any of the parent designs
    if(parent2->m_area <= 0)
      return parent2;
    else if(parent1->m_area <= 0)
      return parent1;

    else if(offspring->m_area < parent2->m_area && offspring->m_latency < parent2->m_latency)
      return parent2;
    else if(offspring->m_area < parent1->m_area && offspring->m_latency < offspring->m_latency)
      return parent1;
  }

  return NULL;

}

/*********************************************************
 **
 ** optima_designs
 **
 ** identify the Pareto-optimal designs
 **
 *********************************************************/
void optimal_designs(CDesign *first_design){

  /* Variables declaration */
  CDesign *design= NULL;
  char synops[MAX_CHARS_LINE];
  double yf;

  // reset any previous optimal designs
  for(design=first_design; design; design=design->m_nextp)
     design->m_pareto_optimal = false;

   for(design=first_design; design; design=design->m_nextp){
    design->m_pareto_optimal = optimal_single(first_design, design);
  
    if(flag_logic_synth == false){

      if(design->m_pareto_optimal == true){

	sprintf(synops,"cp ./dse_%s/%s%d/%s_E.IFF .",process,  process, design->m_count, process);
	system(synops);


	sprintf(synops," veriloggen %s_E.IFF",  process);
	system(synops);  



	// Logic Synthesis
	if(flag_fpga_asic == false){


	  if(flag_predictive == true && c1 != 0){
	    if(flag_multi == false){
	 	yf = c0+c1*design->m_area_fu+c2*(design->m_area*design->m_area); 
	    }
	    else{
	      yf = c0+c1*design->m_area_fu+c2*design->m_area_reg+c3*design->m_area_mux+c4*design->m_area_dec+c5*design->m_area_misc;
	    }
	 
	    design->m_area_ise = (int)yf;
	    design->m_area_pred = (int)yf;
	  }


	  else if(strstr(flib, "fpga_v4.FLIB")){
	    sprintf(synops," LSscrgen %s_E.IFF -syn_tool=ise -clock_period=%dns -fpga_family=virtex4 -fpga_device=XC4VSX35 -fpga_package=FF668 -fpga_speed_grade=-10",  process, CLOCK);
	    system(synops);  
	  }
	  else{
	    sprintf(synops," LSscrgen %s_E.IFF -syn_tool=ise -clock_period=%dns -fpga_family=virtex5 -fpga_device=XC5VLX85 -fpga_package=FF676 -fpga_speed_grade=-1",  process, CLOCK);
	    system(synops);  
	  }
	  
	  sprintf(synops," xtclsh %s_E.tcl",  process);
	  system(synops); 

	  read_ise_synth_results(design, process);

	  sprintf(synops,"mv  *.v *.tcl *.syr ./dse_%s/%s%d  ",process,process,design->m_count);
	  system(synops); 
	}
	// ASIC design
	else{
	  sprintf(synops," LSscrgen %s_E.IFF -syn_tool=dc -clock_period=%dns",  process, CLOCK);
	  system(synops);  
	  
	  sprintf(synops," dc_shell -f %s_E.tcl",  process);
	  system(synops);
	  
	}
      }

    }
   }
  // Set to print ISE area at the report file
  // flag_logic_synth =true;


}
/*********************************************************
 **
 ** optimal_single
 **
 ** check if given design is optimal or not
 **
 *********************************************************/
bool optimal_single(CDesign *first_design, CDesign *design_chk){

  /* Variables declaration */
  CDesign *design= NULL;
  int area_chk = 0;

  if(flag_logic_synth == true)
    area_chk = design_chk->m_area_ise;
  else
    area_chk = design_chk->m_area;


  // Return false if the area or latency = 0 -> probably bdltran error
  if(area_chk == 0 || design_chk->m_latency ==0)
    return false;


  for(design=first_design;design; design=design->m_nextp){
    if(design == design_chk)
      continue;
    else if(design->m_area <= 0 || design->m_latency == 0)
      continue;
    

    if(flag_logic_synth==true){
      if(design->m_area_ise < area_chk && design->m_latency <= design_chk->m_latency)
	return false;
      // Avoid marking as Pareto optimal designs with same results
      else if(design->m_area_ise == area_chk && design->m_latency == design_chk->m_latency){
	if(design_chk->m_count > design->m_count)
	  return false;
      }
    }

    else{

      if(design->m_area < area_chk && design->m_latency <= design_chk->m_latency)
	return false;
      // Avoid marking as Pareto optimal designs with same results
      else if(design->m_area == area_chk && design->m_latency == design_chk->m_latency){
	if(design_chk->m_count > design->m_count)
	  return false;
      }
    }
	    

  }

    return true;

}
