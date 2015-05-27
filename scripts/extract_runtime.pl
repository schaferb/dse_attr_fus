#! /usr/bin/perl
########################################################################################
# PolyU PROPRIETARY
#
# File Name               extract_runtime.pl
# Function                extracts the runtime of exploration runs from log file      
##
# Author                  Benjamin Carrion Schafer
# Date                    July 18, 2013
# Change history
#
#  Usage example: perl extract_runtime.pl -log <filename> -logout <name>
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
print "\n Welcome to PolyU Runtime Extractor CWB V1.0";
print "\n";
print "\n Written by Benjamin Carrion Schafer";
print "\n Copyright PolyU ";
print "\n Version 1.0, July 18, 2013";
print "\n";
print "\n -h for help";
print "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";

my $argc = @ARGV;
my $temp = "";
$logfile = "";
$logfile_out = "";




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

    # -log Specifies the Input log file
   if($temp eq "-log"){

      if ($i+1 <$argc){
	$logfile = $ARGV[$i+1]; 
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }

    # -logout Specifies the output log file
   if($temp eq "-logout"){

      if ($i+1 <$argc){
	$logfile_out = $ARGV[$i+1];
      }
      else{
	print "\n\t ERROR: Missing $temp argument. Check help menu \n";
	&help();
      }
    }


  }


    # Check that all needed  inputs where specified
    if($logfile eq ""){
   	print "\n\t ERROR: Missing argument <log file>  Arguments specified:\n";
        print "\n\nCheck help menu: \n";
	&help();   

      }




  ###############
  ##  Extract runtime of explored designs
  ##
  ## log file format:
  ## RUN 4 / 1
  ##DESIGN ./history/001a ave8 2013/07/18 15:27:20 ATTR 0020 OPTS 1         L:1 M:3 S:4 AREA    4482 LATENCY     2 CP_DELAY    1.62 CYCLES_SIM       0      OK
  ##DESIGN ./history/024a ave8 2013/07/18 15:29:05 ATTR -0023449 OPTS 1         L:1 M:3 S:1 AREA    6310 LATENCY    12 CP_DELAY    3.17 CYCLES_SIM       0  NOTPARETO
  ##DESIGN ./history/026a ave8 2013/07/18 15:29:20 ATTR -0022653 OPTS 1         L:1 M:3 S:1 AREA    6741 LATENCY    10 CP_DELAY    3.17 CYCLES_SIM       0  NOTPARETO
  ##DESIGN ./history/027a ave8 2013/07/18 15:29:28 ATTR -0023262 OPTS 1         L:1 M:3 S:2 AREA    6701 LATENCY    10 CP_DELAY    3.17 CYCLES_SIM       0  NOTPARETO


   &extract_designs();


}


&main();







 ###########################
 ## Extract the designs' synthesis runtime
 ##

sub extract_designs(){



   if($logfile_out eq ""){
     $report_file = "runtime_report_log.xls";
   }
   else{
     $report_file = "runtime_" . $logfile_out .".xls";     
   }
     



   # Generate report file to store a summary of the system
   open(REPORT_FILE_XLS, ">$report_file") || die("ERROR: Could not generate report file $report_file");

   print REPORT_FILE_XLS "\n\nRuntime Analysis";






   ###########################
   ## Read log file 
   ## 
   ##
   open(REPORT, $logfile) || die("ERROR: Could not open CWB report file $logfile");
   @log_file = <REPORT>;
  close(REPORT);


   $flag_bdltranerror2 = 0, $flag_bdltranerror1=0;
   $start=0;
   $end =0;
   
   $design;
   $time_start;
   $time_finish;
   
  foreach $log_line  (@log_file){

    chomp($log_line);

    if($log_line =~ /DESIGN/){


      @line = split(/\s+/, $log_line);

      @time_log = split(/:/, $line[4]);   


      if($start == 0){
	$start = ($time_log[0]*3600)+ ($time_log[1]*60) + $time_log[2];

#	if($log_line =~ /BDLTRANERROR/){
#	  $flag_bdltranerror1 = 1;
#	}

	if($line[13] eq 0){
	  $flag_bdltranerror1 = 1;
	}
	$design = $line[1];
	$time_start = $line[4];


      }
      else{
	$end = ($time_log[0]*3600) +($time_log[1]*60) +  $time_log[2];



	$runtime = $end - $start;
	
	$time_end = $line[4];

#	if($log_line =~ /BDLTRANERROR/){
#	 $ flag_bdltranerror2 = 1;
#	}
	if($line[13] eq 0){
	  $flag_bdltranerror2 = 1;
	}
	else{
	  $flag_bdltranerror2 = 0;
	}

	if($runtime > 0 and $flag_bdltranerror1 == 0 ){
	  print REPORT_FILE_XLS "\n$design\t$line[2]\t$line[4]\t$time_start\t$time_end";
	  print REPORT_FILE_XLS "\t$runtime";
	  print "\n$design $line[2] $line[4]  $end - $start = $runtime  $time_start  $time_end";
     

	}
	$start = $end;
	$design = $line[1];
	$flag_bdltranerror1=$flag_bdltranerror2;
	$flag_bdltranerror2 = 0;
	$time_start = $time_end;
	$time_end = 0;


      }



    }
  }


   print "\n\n";


   close(REPORT_FILE_XLS);


exit -1;

 }




#########################
##
## Print help
##
##
sub help()
{
  print "\nHELP MENU ";
  print "\nlog <name>\t: Name of log file to be analuzed";
  print "\nlogout <name>\t: Name of output xls report file";

  printf "\n\nCommand format is:\n\tperl extract_runtime -log run_attr.log -logout ave8 \n\n";
  exit -1;
    
}
