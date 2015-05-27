/*************************************************************************************
 **
 ** fu.c
 **
 ** version 1.0 created by : BCS
 ** Date : April 16, 2014
 ** Comments: FU DSE
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
#include <math.h>
#include <gsl/gsl_fit.h>


#include "dse.h"

/*******************************
********************************
 **
 ** fu_dse_prob
 **
 ** Perform only DSE for Optimal designs with highest probability 
 ** of finding the optimal solutions 
 **
  *******************************
 *******************************/
void fu_dse_prob(CDesign *first_design,char* process){


  /*
  ** 1.- Generate the smallest design setting FUs to 1:1:1 for all FUs
  */
  fu_dse_mallest_only(first_design,process);


  /*
  ** 2.- Compute the probability of finding pareto optimal designs based on
  **     other Pareto optimal designs found
  */
  probability_optimal(first_design);


  /*
  ** 3.- Perform DSE for only those designs with high probabilities of finding optimal
  **     designs
  */
  fu_dse_prob_adj(first_design, process);

}

/*******************************
 **
 **  fu_dse_smallest_only
 **
 ** Generate the smallest design setting FUs to 1:1:1 for all FUs
 **
 *******************************/
void fu_dse_mallest_only(CDesign *first_design,char* process){

  /* Variables declaration */  
  CDesign *design = NULL, *design_smallest=NULL;

  for(design=first_design; design; design=design->m_nextp){
    if(design->m_pareto_optimal == true){

      /* Copy .IFF and FCNT file from given design to current folder to be explored */
      copy_xplore_files(design, process);

      /* FU DSE for given design */
      design_smallest = fu_dse(design, process, true);

      /* If the are is the same -> delete */
      if(design_smallest != NULL){
	// Point each other 
	design->m_design_fu_single = design_smallest;
	design_smallest->m_design_fu_single = design;

	design->m_prob_max = (design->m_area-design_smallest->m_area)*(design_smallest->m_latency-design->m_latency);
	design->m_prob = design->m_prob_max; // by default -> explore this design pair

      }
    }
  }
}

/*******************************
 **
 **  fu_dse_prob_adj
 **
 ** Perform DSE for only those designs with high probabilities of finding optimal
 ** designs
 **
 *******************************/
void  fu_dse_prob_adj(CDesign *first_design, char *process){

 /* Variables declaration */  
  CDesign *design = NULL;
  float prob=0;

  for(design=first_design; design; design=design->m_nextp){
    if(design->m_pareto_optimal == true){
      if(design->m_design_fu_single != NULL){
	if(design->m_count < design->m_design_fu_single->m_count){
	  prob = (float)design->m_prob/design->m_prob_max;
	  if(prob > MAX_PROB_FU_XPLORE)

	    /* Copy .IFF and FCNT file from given design to current folder to be explored */
	    copy_xplore_files(design, process);

	  /* FU DSE for given design */
	  fu_dse(design, process, false);

	}
      }
    }
  }
}


/*******************************
 **
 ** probabililty_optimal
 **
 ** compute the probability of this design pair to find pareto optimal designs
 **
 *******************************/
void probability_optimal(CDesign *first_design){

 /* Variables declaration */  
  CDesign *design = NULL;


  /* Go through all designs and compute their */
  for(design=first_design; design; design=design->m_nextp){

    /* compute for only designs with smallest FU design and
       in this case for the original design only (not smallest) */
    if(design->m_design_fu_single != NULL)
      if(design->m_count < design->m_design_fu_single->m_count)
	design->m_prob=probability_optimal_single(first_design, design);
      
     
  }
}

/*******************************
 **
 ** probabililty_optimal_single
 **
 ** compute the probability of a single design pair to find pareto optimal design
 **
 *******************************/
int probability_optimal_single(CDesign *first_design, CDesign *design_prob){

 /* Variables declaration */  
  CDesign *design = NULL;
  CDesign *designs_inside[MAX_INSIDE];
  int x=0, count=0, prob=0;

  /* If design pair is inside another design pair and it was detected previously
  ** return 0 prob -> do not need to explore.
  */
  if(design_prob->m_prob == 0)
    return 0;
  else
    prob = design_prob->m_prob;


  for(x=0; x<MAX_INSIDE; x++)
    designs_inside[x] = NULL;



 /* Go through all designs and compute their */
  for(design=first_design; design; design=design->m_nextp){

    if(design->m_pareto_optimal == false)
      continue;

    else if(design == design_prob)
      continue;

    else if(design == design_prob->m_design_fu_single)
      continue;

    else if(design->m_latency > design->m_design_fu_single->m_latency)
      continue;

    else if(design->m_area < design->m_area && design->m_design_fu_single == NULL )
      continue;

    /*
    ** Possible scenarios :
    **       1.) Design pair which completely encloses current design pair
    **       2.) Design pair which overlap
    **       3.) Single design within the "bounding box of design and design_smallest
    */

    /* 1.) If design pair is inside of larger design pair ==> do not explore and exit immediately  */
    if(design->m_design_fu_single != NULL){
      if(design->m_latency < design_prob->m_latency){
	if(design->m_design_fu_single->m_area >= design_prob->m_design_fu_single->m_area){
	  design->m_prob = 0;
	  return 0;
	}
      }
    }
  
    /* Igore design pair if it is inside of current design and make its prob=0 so that it is not explored*/
    if(design->m_design_fu_single != NULL){
      if(design->m_latency > design_prob->m_latency && design->m_latency < design_prob->m_design_fu_single->m_latency){
	if(design->m_design_fu_single->m_area >= design_prob->m_design_fu_single->m_area){
	design->m_prob = 0;
	continue;
	}
      }
    }
    /* 2.) and 3.) */
    if(design->m_latency > design_prob->m_latency && design->m_latency< design_prob->m_design_fu_single->m_latency)
      designs_inside[count++]=design;

  }


  /* Compute the probability of finding an an optimal design */
  if(count == 0)
    return prob;

  /* sort designs within bounding box based on area
  ** and compute the probability  */
  sort_designs_box(designs_inside);
  prob = prob_box(designs_inside, design_prob);

  return prob;

}

/*******************************
 **
 ** sort_designs_box
 **
 ** Sort all the designs based on area
 **
 *******************************/
void sort_designs_box(CDesign *designs_inside[MAX_INSIDE]){

  /* Variables declaration */
  CDesign *tmp;
  int x=0,y=0;


  /* Sort by area from highest to lowest */
  for(x=0; x < MAX_INSIDE-1; x++){
    if(designs_inside[x] == NULL)
      break;
      
    for(y=x+1; y < MAX_INSIDE; y++);
	if(designs_inside[y]== NULL)
	  break;

	if(designs_inside[y]->m_area > designs_inside[x]->m_area){
	  tmp = designs_inside[x];
	  designs_inside[x] = designs_inside[y];
	  designs_inside[y] = tmp;
	}
  }
}

/*******************************
 **
 ** prob_box
 **
 ** compute probability box
 **
 *******************************/
int prob_box(CDesign *designs_inside[MAX_INSIDE], CDesign *design_prob){

  /* Variables declaration */
  int x=0, prob=0, area_limit, latency_limit;
  
  area_limit = design_prob->m_area;
  latency_limit  = design_prob->m_design_fu_single->m_latency;

  for(x=0; x< MAX_INSIDE; x++){
    if(designs_inside[x] == NULL)
      break;

    prob += ((area_limit-designs_inside[x]->m_area)*(latency_limit-designs_inside[x]->m_latency));
    area_limit= designs_inside[x]->m_area;
     
  }

  return(design_prob->m_prob_max-prob);

}


/******************************************
 ******************************************
 **
 ** fu_dse_optimal
 **
 ** Perform only DSE for Optimal designs
 **
 *******************************************
 *******************************************/
void fu_dse_optimal(CDesign *first_design,char* process){

 /* Variables declaration */  
  CDesign *design = NULL;


  /* Go through all designs and check which one is optimal */
  for(design=first_design; design; design=design->m_nextp){
    if(design->m_pareto_optimal == true){

      /* Copy .IFF and FCNT file from given design to current folder to be explored */
      copy_xplore_files(design, process);

      /* FU DSE for given design */
      fu_dse(design, process, false);

    }
  }

}



/***********************
** 
** copy_xplore_files
**
** Comments: Copy .IFF and LIB/FCNT file from given design to current folder to be explored
**
************************/
void copy_xplore_files(CDesign *design, char *process){

  /* Variables declaration */
  char synops[MAX_CHARS_LINE];

  sprintf(synops,"cp ./dse_%s/%s%d/* . ",process,process, design->m_count);
  system(synops);

}


/*********************************
**********************************
** 
** fu_dse
**
** Comments: Functional Units DSE
**
**********************************
**********************************/
CDesign *fu_dse(CDesign *design, char *process, bool flag_smallest){

  /* Variables declaration */  
  CDesign *design_new = NULL;
  double yf, yf_err;
  bool flag_last = false;
  int area_new=0, area=0;
  char synops[MAX_CHARS_LINE];

  do{

    if(flag_logic_synth)
      area = design->m_area_ise;
    else
     area = design->m_area;      


  
    design_new = gen_design();


    /* Copy FCNT file reduce the number of FUs by x% * at the same time */
    flag_last = copy_fu_and_reduce(design, process, design_new->m_count, flag_smallest);

    /* Synthesize new design with new FU */
    synthesize_fu(design, process, design_new->m_count);

    /* Read synthesis results */
    read_bdltran_syn_results(design_new, process);

    if(design_new->m_area == 0)
      return design_new;

    if(flag_logic_synth==true){
      sprintf(synops," veriloggen %s_E.IFF",  process);
      system(synops);  


      if(flag_predictive == true && c1 != 0){
	if(flag_multi == false){
	  yf = c0+c1*design->m_area_fu+c2*(design->m_area*design->m_area); 	 
	    
	  }
	else{
	  yf = c0+c1*design->m_area_fu+c2*design->m_area_reg+c3*design->m_area_mux+c4*design->m_area_dec+c5*design->m_area_misc;
	}
	design_new->m_area_ise = (int)yf;
	design_new->m_area_pred = (int)yf;
      }

      else if(flag_fpga_asic == false){

	if(strstr(flib, "fpga_v4.FLIB")){
	  sprintf(synops," LSscrgen %s_E.IFF -syn_tool=ise -clock_period=%dns -fpga_family=virtex4 -fpga_device=XC4VSX35 -fpga_package=FF668 -fpga_speed_grade=-10",  process, CLOCK);
	  system(synops);  
	}
	else{
	  sprintf(synops," LSscrgen %s_E.IFF -syn_tool=ise -clock_period=%dns -fpga_family=virtex5 -fpga_device=XC5VLX85 -fpga_package=FF676 -fpga_speed_grade=-1",  process, CLOCK);
	  system(synops);  
	}
	
	sprintf(synops," xtclsh %s_E.tcl",  process);
	system(synops); 
	
	read_ise_synth_results(design_new, process);


	//  sprintf(synops,"mv  *.v *.tcl *.syr ./dse_%s/%s%d  ",process,process,design->m_count);
	//system(synops); 
      }
      else{
	sprintf(synops," LSscrgen %s_E.IFF -syn_tool=dc -clock_period=%dns",  process, CLOCK);
	system(synops);  

	sprintf(synops," dc_shell -f %s_E.tcl",  process);
	system(synops);

      }
    }


    if(flag_logic_synth)
      area_new = design_new->m_area_ise;
    else
      area_new = design_new->m_area;      



    /* In case of exploring only smallest design -> delete if FUs = 1:1:1 is larger
     * should never happen as FUs are reduced to smallest possible number */
    if(flag_smallest){
      if(area_new > area){
	delete_designs(design_new);
	design_new = NULL;
	break;
      }
    }

    /* Exit if bdltran synthesis error -> Not enought FUs */
    if(area_new == 0 || design_new->m_latency == 0){
      delete_designs(design_new);
      design_new = NULL;
      break;
    }
    
    else if(area_new == area && design_new->m_latency==design->m_latency){
      delete_designs(design_new);
      design_new = NULL;
      break;
    }

    /* Move results to new folder and continue until all FUs are 1:1:1 */
    else{
      store_results(design_new, process);
      insert_design(design, design_new);
    }

    if(flag_last == true)
      break;

    /* Break if only a smallest design should be generated */
    if(flag_smallest == true)
      break;

    design=design_new;// move design to decrement the FUs of the new FU

  }while(1);
  
  /* Return latest design */
  return design_new;

}


/***********************
** 
** copy_fu_and_reduce
**
** Comments: Functional Units exploration
**
************************/
bool copy_fu_and_reduce(CDesign *design,char * process, int count, bool flag_smallest){

  /* Variables declaration */
  FILE *ptr_fu=NULL, *ptr_fu_new=NULL; 
  char line[MAX_CHARS_LINE], *pchr; 
  int num_fus =0, delta=1;
  bool flag_all_ones=true;


 
  /*Open FU file to read FUs to be decreased */
  sprintf(line,"%s-auto%d.FCNT", process, design->m_count);

  ptr_fu = fopen(line, "rt");
  if(ptr_fu == NULL){
    printf("\n**** ERROR :  Couldn't open file %s", line);
    return true;
  }

  sprintf(line,"%s-auto%d.FCNT", process, count);
  ptr_fu_new = fopen(line, "wt");
  if(ptr_fu == NULL){
    printf("\n**** ERROR :  Couldn't open file %s", line);
    return true;
  }


  while(fgets(line, MAX_CHARS_LINE,ptr_fu) != NULL){

  if(!strstr(line, "LIMIT"))
    fprintf(ptr_fu_new, "%s", line);

  /* If LIMIT keyword found ==> reduce FU limite by percent_fu_decrease */
  else{

    // Set directly to 1
    if(flag_smallest == true)
      fprintf(ptr_fu_new, "\tLIMIT\t1\n");			     

    else{

      pchr = strchr(line,'T');
      pchr = pchr+1;
      num_fus = atoi(pchr);

      /* reduce number of FUs */
      delta =  ceil(num_fus*percent_fu_decrease);
      if(delta < 1)
	delta = 1;
      num_fus -= delta;
      if(num_fus <= 1)
	num_fus = 1;
      else
	flag_all_ones = false; /* FU exploration termination condition */

      fprintf(ptr_fu_new, "\tLIMIT\t%d\n", num_fus);
    }
  }

  }

  fclose(ptr_fu);
  fclose(ptr_fu_new);

  return flag_all_ones;

}

/***********************
** 
** synthesize_fu
**
** Comments: Functional Units exploration
**
************************/
void synthesize_fu(CDesign *design, char *process, int count){

  /* Variables declaration */
  char synops[MAX_CHARS_LINE];

  /*
  ** BDLTRAN
  ** Synthesize the design
  **/
  sprintf(synops,"bdltran -c%d -s -lfl %s -lb %s -lfc %s-auto%d.FCNT -lml %s-auto.MLIB -lmc %s-auto.MCNT %s.IFF ", CLOCK*100,  flib, blib, process, count, process, process, process);
  system(synops);




}

/***********************
** 
** store_results
**
** Comments: Functional Units exploration
**
************************/
void store_results(CDesign *design, char *process){

  /* Variables declaration */
  char synops[MAX_CHARS_LINE];

  /* Store results in log file */
  results_single(design);


  /* Make a new folder and store synthes results there */
  sprintf(synops,"mkdir ./dse_%s/%s%d",process, process, design->m_count);
  system(synops);

  sprintf(synops,"mv *.CSV *.QOR ./dse_%s/%s%d  ",process, process,design->m_count);
  system(synops);

  sprintf(synops,"cp *.IFF *-auto%d.FCNT  *.MLIB *.MCNT ./dse_%s/%s%d  ",design->m_count,process, process, design->m_count);
  system(synops);

  if(flag_logic_synth==true){
    sprintf(synops,"mv  *.v *.tcl *.syr ./dse_%s/%s%d  ",process,process,design->m_count);
    system(synops); 
  }


}
