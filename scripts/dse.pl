#! /usr/bin/perl
########################################################################################
# PolyU PROPRIETARY
#
# File Name               dse.pl
# Function                performs the DSE for the design passed generating its ifx file       
##
# Author                  Benjamin Carrion Schafer
# Date                    June 28, 2013
# Change history
#
#  Usage example: perl dse.pl -sc ../benchmarks/filter_FIR/fir.cpp -attrs -fus -global -lfl <flib> -lb <blib> -num <itr> -process <name>
#                 OR
#                 perl dse.pl -bdl ../benchmarks/ave8/ave8.c -fus -lfl <flib> -lb <blib> -num <itr> -process <name>
#
#
########################################################################################

#use strict;
use warnings;
use diagnostics;
use POSIX;
use File::Basename;



sub main () {

print "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
print "\n Welcome to PolyU's DSE for CWB V1.0";
print "\n";
print "\n Written by Benjamin Carrion Schafer";
print "\n Copyright PolyU ";
print "\n Version 1.0, June 27, 2013";
print "\n";
print "\n -h for help";
print "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";

my $argc = @ARGV;
my $temp = "";
$input_file = "";
$process_name ="";
$bdl_systemc = "BDL";
$num_itr = 10;

$flib ="";
$blib ="";


$attrs = "no";
$fus = "no";
$global = "no";
$freq = "no";
$delay = "no";
$sim = "no";
$keep_all = "no";
$bits = 0;


#######  READ  input arguments
for ($i=0 ;$i< $argc; $i++)
  {

    $temp = $ARGV[$i];
   
    if(!$temp){
      print "\n\t ERROR: Missing arguments needed. Check help menu \n";
      &help();
    }

    # Version print
    if($temp eq "-v") {
      exit -1; # version printed in header 
    }

    # -h Print the help menu
   if($temp eq "-h"){
     &help();
    }

    # -sc Specifies the Input file to be synthesized (SystemC or BDL)
   if($temp eq "-sc"){
     $bdl_systemc = "SYSTEMC";
      if ($i+1 <$argc){
	$input_file = $ARGV[$i+1]; 
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }

    # -bdl Specifies the Input file to be synthesized (SystemC or BDL)
   if($temp eq "-bdl"){
     $bdl_systemc = "BDL";
      if ($i+1 <$argc){
	$input_file = $ARGV[$i+1]; 
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }

   if($temp eq "-freq"){
	$freq = "yes";
	}		

   if($temp eq "-delay"){
	$delay = "yes";
	}		



   if($temp eq "-sim"){
	$sim = "yes";
	}

   if($temp eq "-keep_all"){
	$keep_all = "yes";
	}



    # -explore attributes
   if($temp eq "-attrs"){
	$attrs = "yes"; 
    }

    # -explore fus
   if($temp eq "-fus"){
	$fus = "yes"; 
    }

    # -explore global options
   if($temp eq "-global"){
	$global = "yes"; 
    }

    # -process Specifies the process name of the Input file to be synthesized (SystemC or BDL)
   if($temp eq "-bits"){
      if ($i+1 <$argc){
	$bits = $ARGV[$i+1]; 
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }


    # -process Specifies the process name of the Input file to be synthesized (SystemC or BDL)
   if($temp eq "-process"){
      if ($i+1 <$argc){
	$process_name = $ARGV[$i+1]; 
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }


    # -lfl specifies the FLIB file to be used
   if($temp eq "-lfl"){

      if ($i+1 <$argc){
	$flib = $ARGV[$i+1]; 
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }

    # -lb specifies the BLIB file to be used
   if($temp eq "-lb"){

      if ($i+1 <$argc){
	$blib = $ARGV[$i+1]; 
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }



    # -num specifies the number of DSE designs to be explored
   if($temp eq "-num"){

      if ($i+1 <$argc){
	$num_itr = $ARGV[$i+1]; 
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }


  }


    # Check that all needed  inputs where specified
    if(($input_file eq "") || ($flib eq "") || ($blib eq "")){
   	print "\n\t ERROR: Missing arguments (<sc|bdl>, flib,  blib). Arguments specified:\n";
	print "\n sc|bdl\t= $input_file";
#	print "\n process\t= $process_name";
	print "\n FLIB\t= $flib";
	print "\n BLIB\t= $blib";
        print "\n\nCheck help menu: \n";
	&help();   

      }


# Check input file extension vs. BDL|systemC type for possible inconsistencies
if(($bdl_systemc eq "BDL") && ($input_file =~ /.cpp$/)){
  print "\nERROR: File extension is .cpp but selected BDL type as (-bdl). Use -sc option \n";
}
elsif(($bdl_systemc eq "BDL") && ($input_file =~ /.sc$/)){
  print "\nERROR: File extension is .scbut selected BDL type as (-bdl). Use -sc option \n";
} 

elsif(($bdl_systemc eq "SYSTEMC") && ($input_file =~ /.c$/)){
  print "\nERROR: File extension is .c but selected SystemC type as (-sc). Use -bdl option \n";
} 



  ###############
  ##  Explore the different patitioned filter/synth benchmark stages
  ##
   &explore_designs();


}


&main();







 ###########################
 ## Synthesize each of the partitions and generate top module
 ##
 ## 1.) PARSE the design
 ## 2.) Synthesize
 ## 2.1) DSE (only FUs)
 ## 3.) Generate synthesis script
 ## 4.) Synthesize - PAR
 ## 5.) Compare CWB synthesis report with Xilinx's
 ##

sub explore_designs(){

   my $time_start;
   my $time_end=0;

   my $time_start_tmp;
   my $time_end_tmp;

   my $time_hls_total=0;
   my $time_dse_total=0;
   my $time_xilinx_total=0; 

   my $time_all=0;



   if($bdl_systemc eq "BDL"){
     $fileonly = basename($input_file,  ".c");
   }
   else{
     $fileonly = basename($input_file,  ".cpp");
   }

   $flibonly = basename($flib,  ".FLIB");

   $process_name =  $fileonly;


   $report_file= "report_" . $process_name ."_" . $flibonly . ".rpt";
   $report_file_xls= "report_" . $process_name ."_" . $flibonly  . ".xls";  


   # Generate report file to store a summary of the system
   open(REPORT_FILE, ">> $report_file") || die("ERROR: Could not generate report file $report_file");
   open(REPORT_FILE_XLS, ">> $report_file_xls") || die("ERROR: Could not generate report file $report_file_xls");

   print REPORT_FILE_XLS "\nInput File :\t$input_file";
   print REPORT_FILE_XLS "\nProcess :\ $process_name";
   print REPORT_FILE_XLS "\nFLIB :\t $flib";
   print REPORT_FILE_XLS "\nBLIB :\t $blib";



   ###########################
   # scpars of bdlpars input file to generate .IFF file
   ##
   if($bdl_systemc eq "BDL"){
      $cmd = "bdlpars $input_file -DBDL"; 
      system( $cmd);
   }
    else{
      $cmd = "scpars $input_file";
      system( $cmd);
    }


   #########################
   ## Run cwbexplorer
   ## 
   $time_start_tmp = time();


   system("rm -rf ./history"); 
   cwbexplorer();


   $time_end_tmp = time();
   $time_dse_total = $time_end_tmp-$time_start_tmp;



   ###########################
   ## Read CWB and Altera/Xilinx report files 
   ## and store results in report file
   ##
   ## CWB REPORT
   read_CWB_rpt();
   



   print "\n\n";

   close(REPORT_FILE);
   close(REPORT_FILE_XLS);





exit -1;

 }



#########################
##
## Perform DSE for each filter split
##
#########################
sub  cwbexplorer()
{

    # Generate the input configuration file for cwbexplorer   
    $cwbexplorer_file = $process_name . ".ifx";

 
     # ---- Create .IFX file for explorer
     open(EXPLORER_FILE, "> $cwbexplorer_file") || die("ERROR: Could not generate cwbexplorer file  $cwbexplorer_file ");

    print EXPLORER_FILE "#---------------------------------------";
    print EXPLORER_FILE "\n# $cwbexplorer_file ";
    print EXPLORER_FILE "\n# Input file for cwbexlorer - CWB Explorer";
    print EXPLORER_FILE "\n#";
    print EXPLORER_FILE "\n# File generated automatically by sharing.pl script ";
    print EXPLORER_FILE "\n#";
    print EXPLORER_FILE "\n#--------------------------------------";
    print EXPLORER_FILE "\n";
    print EXPLORER_FILE "\nEXP_IFF\t". $process_name . ".IFF # input file to be explored ";
    print EXPLORER_FILE "\nEXP_BASE_OPT\t -c1500 -s -EE  -lfl $flib -lb $blib ";


    if($attrs eq "no"){
      print EXPLORER_FILE "\nEXP_ATTR\t NO";
    }
    else{
      print EXPLORER_FILE "\nEXP_ATTR\t YES";
      print EXPLORER_FILE "\nEXP_FIX_ATTR\t NO";
  #    print EXPLORER_FILE "\nEXP_HEURISTIC\t gentech";
    }


    if($global eq "no"){
      print EXPLORER_FILE "\nEXP_OPT\t NO";
    }
    else{
      print EXPLORER_FILE "\nEXP_OPT\t YES";
      print EXPLORER_FILE "\nEXP_FIX_OPT\t NO";

     if($freq eq "yes"){
    	 print EXPLORER_FILE "\nEXP_FREQ";
   	 }
	}

    if($fus eq "no"){
      print EXPLORER_FILE "\nEXP_FU\t NO";
    }
    else{
      print EXPLORER_FILE "\nEXP_FU\t YES";
    }

    print EXPLORER_FILE "\nEXP_DEL\t YES";
    print EXPLORER_FILE "\nEXP_NUM_DESIGNS\t  $num_itr";

    if($sim eq "yes"){
      print EXPLORER_FILE "\nEXP_SIM\t100";
    }


    if($delay eq "yes"){
      print EXPLORER_FILE "\nEXP_DELAY";
    }


    if($keep_all eq "yes"){
      print EXPLORER_FILE "\nEXP_NO_PARETO";        
    }


  #  print EXPLORER_FILE "\nEXP_RTL_OUT\t vhdl";

    print EXPLORER_FILE "\n";

    close(EXPLORER_FILE);

    # ----- Execute CWBEXPLORER
    $cmd = "cwbexplorer -if $cwbexplorer_file";
    system ($cmd);
}


#########################
##
## Read data from CWB report and write to overall report
##
#########################
sub read_CWB_rpt()
{


  $start_write  = 0;
  $start_results  = 0;
  $latency = 0;
  $frequency = 0;
  $throughput = 0;
  $count = 0;

  # ----- Extract summary CyberWorkBench's synthesis report (bdltran)
  $cwbexplorer_rpt = "./history/" .  $process_name .".rpt ";

#  unless (-e $cwbexplorer_rpt) {
#    print "\n\ncwb $cwbexplorer_rpt RETURNING\n";
#    return;
#  }

  print "\n\ncwb $cwbexplorer_rpt\n";
  if($bits > 0){
    print REPORT_FILE "\nOutput port bitwidth $bits";
  }

  open(CWBEXPLORER_RPT, $cwbexplorer_rpt) || die("ERROR: Could not open CWB report file $cwbexplorer_rpt");
  @cwbexplorer_file = <CWBEXPLORER_RPT>;
  close(CWBEXPLORER_RPT);


  foreach $cwbexplorer_line  (@cwbexplorer_file){

    chomp($cwbexplorer_line);

    if($cwbexplorer_line =~ /Input options:/){
      $start_write = 1;
    }

    elsif($cwbexplorer_line =~ /Detailed design info:/){
      $start_write = 1;
      last;
    }


    if($start_write == 1){
      print REPORT_FILE "\n$cwbexplorer_line";
 
      if($cwbexplorer_line =~ /Exploration results summary:/){
	$start_results = 1;
      }

   
      if($start_results == 1){
	@line = split(/\s+/, $cwbexplorer_line);
	  print REPORT_FILE_XLS "\n";

	$count = 0;
	foreach $word (@line){
	  print REPORT_FILE_XLS "$word\t";

	  if($bits > 0 && $cwbexplorer_line =~ /a\//){
	    if($count == 6){
	      $latency = $word;
	    }
	    elsif($count == 7){
	      $frequency = (1/$word)*1000;
	    }
	  }
	  $count ++;
	}

	# Compute and print the throughput
	if($bits > 0 && $cwbexplorer_line =~ /a\//){
	  print "\nLINE $cwbexplorer_line";
	  print "\nFrequency :$frequency\tLatency: $latency";

	  print REPORT_FILE_XLS "$frequency\t";
	  $throughput = ($bits*$frequency)/$latency;
	  print "\tThroughput: $throughput";
	  print REPORT_FILE_XLS "$throughput\t";
	
	}

      }
      else{
	print REPORT_FILE_XLS "\n$cwbexplorer_line"; 
      }
    }
  }
}


#########################
##
## Print help
##
##
sub help()
{
  print "\nHELP MENU ";
  print "\nsc|bdl <filename>\t: Name of input SystemC or BDL file";
  print "\nprocess <process_name>\t: Name of top process";
  print "\nflib <lib_path>\t: Name and path of FLIB library";
  printf "\nblib <lib_path>\t: Name and path of BLIB library";
  printf "\nnum : Number of FUs exploration steps (optional, Default $num_itr)";
  printf "\nattrs : explore synthesis attributes)";
  printf "\nfus : explore FUs)";
  printf "\nglobal : explore global options)";
  printf "\n\nCommand format is:\n\tperl dse.pl -sc|bdl <file> -process <processname> -lfl <flib> -lb <blib> -attrs -fus -global \n\n";
  exit -1;
    
}
