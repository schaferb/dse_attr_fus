/*************************************************************************************
 **
 ** dse.c
 **
 ** version 1.0 created by : BCS
 ** Date : July 15, 2013
 ** Comments: HLS  Design Space Explorer executing different heurstics
 **
 **
 **
 ************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>


#include "adrs.h"


CDesign *first_attr = NULL;
CDesign *first_fus = NULL;
CDesign *first_opts = NULL;
CDesign *first_all = NULL;


bool verbose= false;
bool throughput=false;
char *log_file = 0;

int exploration_time=-1;

int main(int argc, char *argv[]){


  /* Read programs input arguments */
  read_input_arguments(argc, argv);


  /* print program version */
  print_version();

  /* Stop exploration if needed input arguments missing */

  if(log_file == 0){
    printf("\nMissing logfile file. \n\n");
    return  1;
  }    


  /* Main DSE function */
   adrs();
 
   printf("\nFinished successfuly generation of ADRS\nThroughput %s \n\n",throughput?"Yes":"No");


  /* Free memory for data structures created */
  delete_data();

  return 1;

}



/*************************
**************************
** 
** read_input_arguments
**
** Comments: Reads arguments passed by user
**
*************************
*************************/
 void read_input_arguments(int argc, char *argv[]){

    int x=0;

    /* Read all input arguments */
    for(x=0; x< argc; x++){

      /*    printf("\nARG[%d] = %s ",x, argv[x]); */


      /* READ attribute list for the given design */
      if(!strcmp(argv[x], "-log")){
	x++;	  
	log_file = (char*)malloc(strlen(argv[x])+1);
	strcpy(log_file, argv[x]);
	read_log(argv[x]);

      }


      else if(!strcmp(argv[x], "-verbose"))
	verbose = true;

      else if(!strcmp(argv[x], "-throughput"))
      throughput = true;




    else if(!strcmp(argv[x], "-v")){
	print_version();
	exit(EXIT_SUCCESS);

    }
      


    }
  }

/***********************
** 
** version
**
** Comments: prints the version of the tool
**
************************/
void print_version(){

  printf("\n\nHLS DSE ");
  printf("\n(c) PolyU 2013");
  printf("\nWritten by PolyU DARClab");
  printf("\nVersion 1.0\nBuild  %s %s\n\n", __TIME__, __DATE__);



}





/***********************
** 
** read_log
**
** Comments: reads the data stored in lib.info (attributes names and argument
**           for the given benchmark
**
************************/
void read_log(char *log_name){


  FILE *ptr_file = NULL;
  CDesign *design_new = NULL, *first_design = NULL;
  char line[MAX_CHARS_LINE], *token;
  int count = 0, count_state = 0, count_time=6;
  bool flag_start = false;


  if(throughput == false)
    count_time = 6;
  else
    count_time = 9;
  
  

  printf("\nLatency element = %d\n\n", count_time);

  printf("\nReading log fie %s ...", log_name);

  ptr_file = fopen(log_name, "rt");
  if(ptr_file == NULL){
    printf("\nCould not open file %s \n\n", log_name);
    exit(EXIT_FAILURE);
  }


  /* Read file data and generate structure linked list */
  while(fgets(line, sizeof line, ptr_file) != NULL){

    if(line[0] == '#')
      continue;

    count = 0;
    if(strstr(line, "NAME")){
      flag_start = true;
      count =0;
      continue;
    }

    if((strstr(line, "Input File") && first_design != NULL) || strstr(line, "ADRS")){
      flag_start = false;

      if(count_state == 0)
	first_attr = first_design;

      else if(count_state == 1)
	first_fus = first_design;

      else if(count_state == 2)
	first_opts = first_design;

      else if(count_state == 3){
	first_all = first_design;     
	break;
      }

      count_state++;
      first_design= NULL;
      continue;
    }

      if(flag_start == true){

	token = strtok(line,"\t");
	while (token != NULL){
       

	/* Generate a new design*/
	  if(count == 3){

	    design_new = (CDesign*) malloc(sizeof(CDesign));
	    design_new->m_area = atoi(token);

	  if(verbose)
	    printf("\nDesign Area =%d", design_new->m_area);

	    if(first_design == NULL)
	      first_design = design_new;
	    else
	      insert_design_list(first_design, design_new);     
	  
	}

	/* read latency */
	else if(count ==  count_time){
	  design_new->m_latency = atof(token);
       
	  if(verbose)
	    printf("\n       Latency =%0.2f", design_new->m_latency);
	  break;
	}

	count ++;
	token = strtok(NULL,"\t");
      
      }
    }
  }

  fclose(ptr_file);

  first_all = first_design;
}



/***********************
** 
** insert_design_list
**
** Comments: Inserts a new design into list
**
************************/
 void insert_design_list(CDesign *first_design, CDesign *design_new){

   CDesign *list = NULL;

   /* Go to the very last linked list element */
   for(list=first_design; list->m_nextp != NULL; list=list->m_nextp);

   
   /* insert process at the end of the list */
   list->m_nextp = design_new;
   design_new->m_prevp = list;

 }
    




/*****************************************************************
******************************************************************
**
** DSE
**
** Comments: Main ADRS calculation function
**
*******************************************************************
*******************************************************************/
void adrs(){

  /* Variables declarations  */

  float adrs_attrsa=0, adrs_attrsl=0, adrs_fusa=0,adrs_fusl=0, adrs_optsa=0, adrs_optsl=0;

  /* Comput the ADRS for the designs */
  /* ATTRS */
  comp_adrs(first_all, first_attr, &adrs_attrsa, &adrs_attrsl);

  /* FUs */
  comp_adrs(first_all, first_fus, &adrs_fusa, &adrs_fusl);

  /* OPTS */
  comp_adrs(first_all, first_opts, &adrs_optsa, &adrs_optsl);


  /*
  ** print results on screen and on log file
  */

  results_logout(adrs_attrsa, adrs_attrsl, adrs_fusa, adrs_fusl, adrs_optsa, adrs_optsl);


 }

/******************************************************************
**
** comp_adrs
**
** Comments: computes the ADRS for two given pareto fronts
**
*******************************************************************/
void comp_adrs(CDesign *first_all, CDesign *first_sub, float *adrs_area, float *adrs_latency){


  /* Variables declaration */
  CDesign *design,*designsub, *design_closest=NULL;
  float adrsa=0, adrsl=0;
  int count = 0;

  
  for(design=first_all; design; design=design->m_nextp){
    count ++;

    for(designsub = first_sub; designsub; designsub=designsub->m_nextp){

      if(design_closest == NULL)
	design_closest = designsub;

      else{
	if(abs(design->m_area-design_closest->m_area) > abs(design->m_area-designsub->m_area))
	  design_closest = designsub;
      }
    }


    adrsa += (float)abs(design->m_area-design_closest->m_area)/design->m_area;
    adrsl += (float)abs(design->m_latency-design_closest->m_latency)/design->m_latency;
  }

  *adrs_area = (adrsa)/count;
  *adrs_latency = (adrsl)/count;

}



/****************************************************************
 ****************************************************************
** 
** results_logout
**
** Comments: print out the results on screen and log file
**
****************************************************************
****************************************************************/
void results_logout(float adrs_attrsa,float adrs_attrsl, float adrs_fusa, float adrs_fusl, float adrs_optsa, float adrs_optsl){


   FILE *ptr_file = NULL;




   ptr_file = fopen(log_file, "at");
   if(ptr_file == NULL){
     printf("\nCannot create report file %s \n", log_file);
     exit(EXIT_FAILURE);
   }

   fprintf(ptr_file,"\n\nADRS Summary: ");
   fprintf(ptr_file,"\nTYPE\tADRS AREA\tADRS Latency\tADRS");
   fprintf(ptr_file,"\nALL\t%f\t%f\t%f",adrs_attrsa, adrs_attrsl, (adrs_attrsa> adrs_attrsl)?adrs_attrsa:adrs_attrsl);
   fprintf(ptr_file,"\nOPTIMAL\t%f\t%f\t%f",adrs_fusa, adrs_fusl, (adrs_fusa> adrs_fusl)?adrs_fusa:adrs_fusl);
   fprintf(ptr_file,"\nPROB\t%f\t%f\t%f",adrs_optsa, adrs_optsl, (adrs_optsa> adrs_optsl)?adrs_optsa:adrs_optsl);


   printf("\n\nADRS Summary: ");
   printf("\nTYPE\tADRS AREA\tADRS Latency\tADRS");
   printf("\nALL\t%f\t%f\t%f",adrs_attrsa, adrs_attrsl, (adrs_attrsa> adrs_attrsl)?adrs_attrsa:adrs_attrsl);
   printf("\nOPTIMAL\t%f\t%f\t%f",adrs_fusa, adrs_fusl, (adrs_fusa> adrs_fusl)?adrs_fusa:adrs_fusl);
   printf("\nPROB\t%f\t%f\t%f",adrs_optsa, adrs_optsl, (adrs_optsa> adrs_optsl)?adrs_optsa:adrs_optsl);


   fclose(ptr_file);
}





/********************************************************
*********************************************************
** 
** delete_data
**
** Comments:frees the allocated memory of the DSE data structure
**
*********************************************************
*********************************************************/
void delete_data(t){

  /* Delete the generated designs */
  delete_designs(first_attr);
  delete_designs(first_fus);
  delete_designs(first_opts);
  delete_designs(first_all);

}


/*************************
** 
** delete_designs
**
** Comments:frees the allocated memory for the generated designs during DSE
**
**************************/
void delete_designs(CDesign *first_design){

  CDesign *design = NULL, *design_next = NULL;


  for(design=first_design; design; design=design->m_nextp){

    design_next = design->m_nextp;

    free(design);


  }


}

