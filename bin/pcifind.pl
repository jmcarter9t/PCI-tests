#!/bin/sh
eval 'exec perl -w -x $0 ${1+"$@"} ;'
#! -*- perl -*-  (Tell perl and emacs that this is a perl script)

#ptest.plx
#must have execution privilages or be in super user mode to run this program. Cannot have a file named aciotemp.txt or it will be removed.

use strict;
use warnings;
my $bus = 0;
my $card = 0;
my $cardname = 0;
my $irq = 0;
my $ioport = 0;
my $iocase = 0;  #count io ports for each card
my $iocount = 0; #multiple com ports
my $count = 0;	#count number of cards
print "\n\n\nPCI CARD INFORMATION:\n";
#shell script to read pci bus
#and create temporary file 

# Attempt to find the full pathname of the lspci program, defaulting
# to just lspci without a path, if not found.

my @lspci_paths = ("/sbin", "/usr/sbin", "/bin", "/usr/bin");
my $lspci = "lspci";
foreach (@lspci_paths) {
    my $lspci_path = $_ . "/lspci";
    if(-x $lspci_path) {
	$lspci = $lspci_path;
	last;
    }
}

`$lspci -vv -n -d 494f:* >> aciotemp.txt`;
open FILE, "aciotemp.txt" or die$!;
while (<FILE>) {
if (/(:\w\w\w\w)/){
	if ($card ne $1){
		$count++;
		$iocount = 0;
		$card = $1;
		chomp;
		
		#match card type to find name
		if ($card eq ':0c50'){
			$cardname = "PCI-DIO-24H";
			$iocase = "DIO";
		}
		if ($card eq ':0c51'){
			$cardname = "PCI-DIO-24D";
			$iocase = "DIO";
		}
		if ($card eq ':0e51'){
			$cardname = "PCI-DIO-24H(C)";
			$iocase = "DIO";
		}	
		if ($card eq ':0e52'){
			$cardname = "PCI-DIO-24D(C)";
			$iocase = "DIO";
		}	
		if ($card eq ':0e50'){
			$cardname = "PCI-DIO-24S";
			$iocase = "DIO";
		}	
		if ($card eq ':0c60'){
			$cardname = "PCI-DIO-48(H)";
			$iocase = "DIO";
		}
		if ($card eq ':0e60'){
			$cardname = "PCI-DIO-48S(H)";
			$iocase = "DIO";
		}
		if ($card eq ':0e61'){
		  $cardname = "P104-DIO-24S";
		  $iocase = "DIO";
		}
		if ($card eq ':0c68'){
			$cardname = "PCI-DIO-72";
			$iocase = "DIO";
		}	
		if ($card eq ':0c70'){
			$cardname = "PCI-DIO-96";
			$iocase = "DIO";
		}	
		if ($card eq ':2c50'){
			$cardname = "PCI-DIO-96CT";
			$iocase = "DIO";
		}
		if ($card eq ':2c58'){
			$cardname = "PCI-DIO-96C3";
			$iocase = "DIO";
		}
		if ($card eq ':0c78'){
			$cardname = "PCI-DIO-120";
			$iocase = "DIO";
		}
		if ($card eq ':10c9'){
			$cardname = "PCI-COM-1S";
			$iocase = "COM";
		}
		if ($card eq ':1088'){
			$cardname = "PCI-COM232/1";
			$iocase = "COM";
		}
		if ($card eq ':10d0'){
			$cardname = "PCI-COM2S";
			$iocase = "COM";
		}
		if ($card eq ':1090'){
			$cardname = "PCI-COM232/2";
			$iocase = "COM";
		}
		if ($card eq ':1058'){
			$cardname = "PCI-COM422/4";
			$iocase = "COM";
		}	
		if ($card eq ':1059'){
			$cardname = "PCI-COM485/4";
			$iocase = "COM";
		}
		if ($card eq ':10a8'){
		  $cardname = "P104-COM232-8";
		  $iocase = "COM";
		}
		if ($card eq ':1068'){
			$cardname = "PCI-COM422/8";
			$iocase = "8COM";
		}	
		if ($card eq ':1069'){
			$cardname = "PCI-COM485/8";
			$iocase = "8COM";
		}	
		if ($card eq ':10e8'){
			$cardname = "PCI-COM-8SM";
			$iocase = "8COM";
		}
		if ($card eq ':1148'){
			$cardname = "PCI-ICM-1S";
			$iocase = "COM";
		}	
		if ($card eq ':1050'){
			$cardname = "PCI-422/485-2";
			$iocase = "COM";
		}	
		if ($card eq ':1150'){
			$cardname = "PCI-ICM-2S";
			$iocase = "COM";
		}	
		if ($card eq ':1158'){
			$cardname = "PCI-ICM422/4";
			$iocase = "COM";
		}	
		if ($card eq ':1159'){
			$cardname = "PCI-ICM485/4";
			$iocase = "COM";
		}	
		if ($card eq ':aca8'){
			$cardname = "PCI-AI12-16";
			$iocase = "AD";
		}	
		if ($card eq ':aca9'){
			$cardname = "PCI-AI12-16A";
			$iocase = "AD";
		}	
		if ($card eq ':eca8'){
			$cardname = "PCI-AIO12-16";
			$iocase = "AD";
		}	
		if ($card eq ':eca9'){
			$cardname = "PCI-A12-16";
			$iocase = "AD";
		}
		if ($card eq ':ecaa'){
			$cardname = "PCI-A12-16A";
			$iocase = "AD";
		}
		if ($card eq ':ece8'){
		  $cardname = "LPCI-A16-16A";
		  $iocase = "AD";
		}
		if ($card eq ':8efo'){
		  $cardname = "P104-FAS16-16";
		  $iocase = "AD";
		}
		if ($card eq ':ece8'){
			$cardname = "PCI-A16-16";
			$iocase = "AD";
		}
		if ($card eq ':6cb0'){
			$cardname = "PCI-DA12-16";
			$iocase = "DACCAL";
		}	
		if ($card eq ':6ca8'){
			$cardname = "PCI-DA12-8";
			$iocase = "DACCAL";
		}	
		if ($card eq ':6ca0'){
			$cardname = "PCI-DA12-6";
			$iocase = "DACCAL";
		}	
		if ($card eq ':6c98'){
			$cardname = "PCI-DA12-4";
			$iocase = "DACCAL";
		}	
		if ($card eq ':6c90'){
			$cardname = "PCI-DA12-2";
			$iocase = "DACCAL";
		}	
		if ($card eq ':6cb1'){
			$cardname = "PCI-DA12-16V";
			$iocase = "DACCAL";
		}	
		if ($card eq ':6ca9'){
			$cardname = "PCI-DA12-8V";
			$iocase = "DACCAL";
		}	
		if ($card eq ':5ed0'){
			$cardname = "PCI-DAC";
			$iocase = "DAC";
		}
    if ($card eq ':0f01'){
      $cardname = "LPCI-IIRO-8";
      $iocase = "DIO";
    }	
		if ($card eq ':0f00'){
			$cardname = "PCI-IIRO-8";
			$iocase = "DIO";
		}	
		if ($card eq ':0f08'){
			$cardname = "PCI-IIRO-16";
			$iocase = "DIO";
		}
    if ($card eq ':0920'){
      $cardname = "PCI-IDI-48";
      $iocase = "DIO";
    }
    if ($card eq ':0520'){
      $cardname = "PCI-IDO-48";
      $iocase = "DIO";
    }	
		if ($card eq ':0dc8'){
			$cardname = "PCI-IDIO-16";
			$iocase = "DIO";
		}
		if ($card eq ':1250'){
			$cardname = "PCI-WDG-2S";
			$iocase = "WDG";
		}	
		if ($card eq ':22c0'){
			$cardname = "PCI-WDG-CSM";
			$iocase = "WDG";
		}
		if ($card eq ':12d0'){
			$cardname = "PCI-WDG-IMPAC";
			$iocase = "WDG";
		}
	print "\nDAQ CARD ", $count, "\n";
	print "NAME ", $cardname, $card;
	}
}

if (/(^\w\w:\w\w\.\w)/){
	my $bus = $1;
	$bus =~ /(^\w\w)/;
	$bus = $1;
	print "\nBUS ", $bus, "\n";
}
if (/(^\w\w:\w\w\.\w)/){
	my $dev = $1;
	$dev =~ /(\w\w\.)/;
	$dev = $1;
	$dev =~ /(^\w\w)/;
	$dev = $1;
	print "DEVICE: ", $dev, "\n";
}
if (/(IRQ [0-9A-Fa-f]{1,2})/){
	$irq = $1;
	print $irq, "\n";
	$card = 0; #new bus requies new card search
}
if ((/(Region [2-9]: I\/O ports at \w{4})/)&&($iocase eq "8COM")){
	$ioport = $1;
	/([0-9A-Fa-f]{4})/;
	$ioport = hex($1); #decimal from hex from string
	my $iohex = sprintf("%x",$ioport);
	my $count8 = 0;
	my @letter = ('A'..'H');
	print "COM ", $letter[$count8], " Base Address: ", $iohex," ", "\n";
#produce next 7 com ports
	until(++$count8 == 8)
	{
		$ioport += 8;
		$iohex = sprintf("%x",$ioport);
		print "COM ", $letter[$count8], " Base Address: ", $iohex, "\n";
	}
}	
if ((/(Region [2-9]: I\/O ports at \w{4})/)&&($iocase ne "8COM")){
	$ioport = $1;
	$iocount++;
	if ($iocase eq "COM"){
		if ($iocount == 1){
		$ioport =~ s/Region [2-9]: I\/O ports at/COM A Base Address: /i;
		}
		if ($iocount == 2){
		$ioport =~ s/Region [2-9]: I\/O ports at/COM B Base Address: /i;
		}
		if ($iocount == 3){
		$ioport =~ s/Region [2-9]: I\/O ports at/COM C Base Address: /i;
		}
		if ($iocount == 4){
		$ioport =~ s/Region [2-9]: I\/O ports at/COM D Base Address: /i;
		}
	}
	if ($iocase eq "DIO"){
		$ioport =~ s/Region [2-9]: I\/O ports at/DIO Base Address: /i;
	}
	if ($iocase eq "WDG"){
		$ioport =~ s/Region [2-9]: I\/O ports at/WDG Base Address: /i;
	}
	if ($iocase eq "DAC"){
		$ioport =~ s/Region [2-9]: I\/O ports at/DAC Base Address:/i;
	}
	if ($iocase eq "DACCAL"){
		$ioport =~ s/Region [2-9]: I\/O ports at/DACCAL Base Address: /i;
	}
	print $ioport, "\n";
}
}#end while

#remove temp file
`rm aciotemp.txt`;
if ($count == 0){
	print "\nNO 494F PCI-CARDS DETECTED\n"
} 
print "\nend PCIFind program\n\n\n";


