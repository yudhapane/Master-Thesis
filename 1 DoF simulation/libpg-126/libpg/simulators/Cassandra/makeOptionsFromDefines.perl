#!/usr/bin/perl

if ($#ARGV + 1 < 2) {
    die "Usage: $0 className #defineFiles...\n";
}

$base = $ARGV[0];

shift;

print "Base = $base\n";

$pName="makeOptionsFromDefines";

open PARAMSC, ">$base.cc" 
    or die "Could not open $base.cc for write\n";
open PARAMSH, ">$base.hh" 
    or die  "Could not open $base.hh for write\n";
open CTEMPLATE, "$pName.cc.template" 
    or die "Could not open $base.cc for read\n";
open HTEMPLATE, "$pName.hh.template" 
    or die "Could not open $base.cc for read\n";
   
$optNum = 0;

## Process each line of the input header file
while (<ARGV>) {

    if (/NO-OPTION/) {
	# Do nothing if NO-OPTION appears anywhere in a line
    }
    elsif (/^\s*\#define ([A-Z_]+)\s+([-0-9]+)\s+(?:\/\/\s?(.*))?/) {
	# Read ints
	print "Int variable p$1 with default val $2\n";
	$params .= "    static int p$1;\n";
	$setParams .= "int ".$base."::p$1 = $2;\n";
	$optList .= "            {\"$1\", 1, 0, $optNum},\n";
	$cases .= "            case $optNum:\n";
	$cases .= "                p$1 = atoi(optarg); break;\n";
	$reflections .= "    {strdup(\"$1\"),\n";
        $reflections .= "     strdup(\"$3\"),\n";
	$reflections .= "     ".$base."Int,\n";
	$reflections .= "     &".$base."::p$1\n    },\n";
        $optNum++;
    } 
    elsif (/^\s*\#define ([A-Z_]+)\s+([-0-9.Ee]+)\s+(?:\/\/\s?(.*))?/) { 
        # Read doubles
	print "Double variable p$1 with default val $2\n";
	$params .= "    static double p$1;\n";
	$setParams .= "double ".$base."::p$1 = $2;\n";
	$optList .= "            {\"$1\", 1, 0, $optNum},\n";
	$cases .= "            case $optNum:\n";
	$cases .= "                p$1 = atof(optarg); break;\n";
	$reflections .= "    {strdup(\"$1\"),\n";
        $reflections .= "     strdup(\"$3\"),\n";
	$reflections .= "     ".$base."Double,\n";
	$reflections .= "     &".$base."::p$1\n    },\n";
        $optNum++;
    }
    elsif (/^\s*\#define ([A-Z_]+)\s+(".*")\s+(?:\/\/\s?(.*))?/) {  
        # Read strings
	print "String variable p$1 with default val $2\n";
	$params .= "    static char* p$1;\n";
	$setParams .= "char* ".$base."::p$1 = strdup($2);\n";
	$optList .= "            {\"$1\", 1, 0, $optNum},\n";
	$cases .= "            case $optNum:\n";
	$cases .= "                free(p$1);\n";
	$cases .= "                p$1 = strdup(optarg);\n";
        $cases .= "                break;\n";
	$reflections .= "    {strdup(\"$1\"),\n";
        $reflections .= "     strdup(\"$3\"),\n";
	$reflections .= "     ".$base."String,\n";
	$reflections .= "     &".$base."::p$1\n    },\n";
        $optNum++;
    }
    elsif (/^\s*\#define ([A-Z_]+)\s+(true|false)\s+(?:\/\/\s?(.*))?/) {  
        # Read booleans
	print "Boolean variable p$1 with default val $2\n";
	$params .= "    static bool p$1;\n";
	$setParams .= "bool ".$base."::p$1 = $2;\n";
	$optList .= "            {\"$1\", 1, 0, $optNum},\n";
	$cases .= "            case $optNum:\n";
	$cases .= "                p$1 = (strcasecmp(optarg, \"true\")==0 || strcasecmp(optarg, \"yes\")==0)?true:false;\n";
        $cases .= "                break;\n";
	$reflections .= "    {strdup(\"$1\"),\n";
        $reflections .= "     strdup(\"$3\"),\n";
	$reflections .= "     ".$base."Bool,\n";
	$reflections .= "     &".$base."::p$1\n    },\n";
        $optNum++;
    }
}


# Convert cc template to cc file
while (<CTEMPLATE>) {

    s/<SET_PARAMS>/$setParams/;
    s/<OPT_LIST>/$optList/;
    s/<CASES>/$cases/;
    s/<CNAME>/$base/g;
    s/<REFLECTIONS>/$reflections/;
    s/<NUM_OPTIONS>/$optNum/;
    print PARAMSC $_;

}


# Convert hh template to class file
while (<HTEMPLATE>) {

    s/<PARAMS>/$params/;
    s/<CNAME>/$base/;
    print PARAMSH $_;

}
    
