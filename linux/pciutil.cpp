/************************************************************************/
/*                                                                         
/* Module: PCIUTIL.CPP                                                     
/*                                                                         
/* Purpose: Define a CPP interface to the PCI BIOS                           
/*                                                                         
/* Functions Defined:                                                      
/*                                                                         
/*    PCI_BIOS_PRESENT                                                     
/*    FIND_PCI_DEVICE                                                      
/*    FIND_PCI_CLASS_CODE                                                  
/*    GENERATE_SPECIAL_CYCLE                                               
/*    READ_CONFIGURATION_BYTE                                              
/*    READ_CONFIGURATION_WORD                                              
/*    READ_CONFIGURATION_DWORD                                             
/*    WRITE_CONFIGURATION_BYTE                                             
/*    WRITE_CONFIGURATION_WORD                                             
/*    WRITE_CONFIGURATION_DWORD                                            
/*    OUTPD                                                                
/*    INPD                                                                 
/*    INSB                                                                 
/*    INSW                                                                 
/*    INSD                                                                 
/*    OUTSB                                                                
/*    OUTSW                                                                
/*    OUTSD                                                                
/*                                                                         
/* Local Functions                                                         
/*                                                                         
/*    READ_CONFIGURATION_AREA                                              
/*    WRITE_CONFIGURATION_AREA                                             
/*                                                                         
/****************************************************************************

/****************************************************************************
/*                                                                          
/* Include Files                                                            
/*                                                                          
/****************************************************************************/
// #include <dos.h>
#include <stddef.h>
#include "pciutil.h"
#include "display.h"
#include <string.h>
#include <unistd.h>

static int read_configuration_area(byte function,
				   byte bus_number,
				   byte device_and_function,
				   byte register_number,
				   dword *data);

static int write_configuration_area(byte function,
				   byte bus_number,
				   byte device_and_function,
				   byte register_number,
				   dword value);

/****************************************************************************/
/*                                                                          */
/* Define macros to obtain individual bytes from a word register            */
/*                                                                          */
/****************************************************************************/

#define HIGH_BYTE(ax) (ax >> 8)
#define LOW_BYTE(ax) (ax & 0xff)


/****************************************************************************/
/*                                                                          */
/*  PCI_BIOS_PRESENT                                                        */
/*                                                                          */
/* Purpose: Determine the presence of the PCI BIOS                          */
/*                                                                          */
/* Inputs: None                                                             */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    byte *hardware_mechanism                                              */
/*       Identifies the hardware characteristics used by the platform.      */
/*       Value not assigned if NULL pointer.                                */
/*          Bit 0 - Mechanism #1 supported                                  */
/*          Bit 1 - Mechanism #2 supported                                  */
/*                                                                          */
/*    word *interface_level_version                                         */
/*      PCI BIOS Version - Value not assigned if NULL pointer.              */
/*              High Byte - Major version number                            */
/*              Low Byte  - Minor version number                            */
/*                                                                          */
/*      byte *last_pci_bus_number                                           */
/*          Number of last PCI bus in the system. Value not assigned if NULL*/
/*       pointer                                                            */
/*                                                                          */
/*    Return Value - Indicates presence of PCI BIOS                         */
/*       SUCCESSFUL - PCI BIOS Present                                      */
/*       NOT_SUCCESSFUL - PCI BIOS Not Present                              */
/*                                                                          */
/****************************************************************************/

int pci_bios_present(byte *hardware_mechanism,
			    word *interface_level_version,
			    byte *last_pci_bus_number)
{
   int ret_status = 1;          /* Function Return Status. */
   byte bios_present_status;/* Indicates if PCI bios present */
   dword pci_signature;     /* PCI Signature ('P', 'C', 'I', ' ') */
   word ax, bx, cx, flags;  /* Temporary variables to hold register values */
   return (ret_status);
}

void IOPermission(unsigned abase){
    if(ioperm(abase,0xff0,1) < 0)
    {
      TEXT_COLOR(RED);
      CPRINTF("NO IO PERMISSION\n");
      ENDWIN();
      _exit(1);
    }//if
}//IOPermission



/****************************************************************************/
/*                                                                          */
/*  FIND_PCI_DEVICE                                                         */
/*                                                                          */
/* Purpose: Determine the location of PCI devices given a specific Vendor   */
/*          Device ID and Index number.  To find the first device, specify  */
/*          0 for index, 1 in index finds the second device, etc.           */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word device_id                                                        */
/*       Device ID of PCI device desired                                    */
/*                                                                          */
/*    word vendor_id                                                        */
/*       Vendor ID of PCI device desired                                    */
/*                                                                          */
/*    word index                                                            */
/*       Device number to find (0 - (N-1))                                  */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    byte *bus_number                                                      */
/*       PCI Bus in which this device is found                              */
/*                                                                          */
/*    byte *device_and_function                                             */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       DEVICE_NOT_FOUND - Device not found                                */          
/*       BAD_VENDOR_ID - Illegal Vendor ID (0xffff)                         */
/*                                                                          */
/****************************************************************************/
int find_pci_device(word device_id,
		    word vendor_id,
		    word index,
		    byte *bus_number,
		    byte *device_and_function)
{
   int ret_status = NOT_SUCCESSFUL;     /* Function Return Status */
   word ax, bx, flags; /* Temporary variables to hold register values */
   FILE *fp;
   char *tmpfile = tempnam(".", "pcifindXXX");
   char cmd[100];
   char buf[BUFSIZ];
   char *pos;
   sprintf(cmd,"lspci -nn -d %x:%x > %s", vendor_id,device_id, tmpfile);
   system(cmd);                 // Create the file
   if( !(fp = fopen(tmpfile,"r")) ) {
     CPRINTF("Error, Can't read PCI device" );
     goto out_find_pci_device;
   } 
   if( !fgets(buf,BUFSIZ,fp) )
     goto out_find_pci_device;
   sprintf(cmd,"[%x:%x]", vendor_id, device_id);
   if( strstr(buf, cmd ) ) {
     // We have found the string, then we are ok //
     strcpy(cmd,":");
     pos = strstr(buf,cmd );
     strncpy(cmd,pos+1,2 );
     cmd[2] = 0;
     *bus_number = atoi(cmd);
     ret_status = SUCCESSFUL;

   }
  
   // lspci -nn -d 494f:
 out_find_pci_device:
   unlink( tmpfile );
   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  FIND_PCI_CLASS_CODE                                                     */
/*                                                                          */
/* Purpose: Returns the location of PCI devices that have a specific Class  */
/*          Code.                                                           */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word class_code                                                       */
/*       Class Code of PCI device desired                                   */
/*                                                                          */
/*    word index                                                            */
/*       Device number to find (0 - (N-1))                                  */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    byte *bus_number                                                      */
/*       PCI Bus in which this device is found                              */
/*                                                                          */
/*    byte *device_and_function                                             */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       DEVICE_NOT_FOUND - Device not found                                */          
/*                                                                          */
/****************************************************************************/

int find_pci_class_code(dword class_code,
		    word index,
		    byte *bus_number,
		    byte *device_and_function)
{
   int ret_status;     /* Function Return Status */
   word ax, bx, flags; /* Temporary variables to hold register values */
   
   /* Load entry registers for PCI BIOS */

   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  GENERATE_SPECIAL_CYCLE                                                  */
/*                                                                          */
/* Purpose: Generates a PCI special cycle                                   */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to generate cycle on                                       */
/*                                                                          */
/*    dword special_cycle_data                                              */
/*       Special Cycle Data to be generated                                 */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       DEVICE_NOT_FOUND - Device not found                                */          
/*                                                                          */
/****************************************************************************/
int generate_special_cycle(byte bus_number,
			   dword special_cycle_data)
{
   int ret_status; /* Function Return Status */
   word ax, flags; /* Temporary variables to hold register values */
   // ax = 1;
   // flags =2;

   // /* Call PCI BIOS Int 1Ah interface */
   // geninterrupt(0x1a);
   // /* First check if CARRY FLAG Set, if so, error has occurred */
   // if ((flags & CARRY_FLAG) == 0) {
   //    /* Get Return code from BIOS */
   //    ret_status = HIGH_BYTE(ax);
   // }
   // else {
   //    ret_status = NOT_SUCCESSFUL;
   // }

   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  READ_CONFIGURATION_BYTE                                                 */
/*                                                                          */
/* Purpose: Reads a byte from the configuration space of a specific device  */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to read configuration data from                            */
/*                                                                          */
/*    byte device_and_function                                              */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    byte register_number                                                  */
/*       Register Number of configuration space to read                     */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    byte *byte_read                                                       */
/*       Byte read from Configuration Space                                 */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       BAD_REGISTER_NUMBER - Invalid Register Number                      */          
/*                                                                          */
/****************************************************************************/
int read_configuration_byte(byte bus_number,
			    byte device_and_function,
			    byte register_number,
			    byte *byte_read)
{
   int ret_status; /* Function Return Status */
   dword data;

   /* Call read_configuration_area function with byte data */
   ret_status = read_configuration_area(READ_CONFIG_BYTE, 
					bus_number, 
					device_and_function, 
					register_number, 
					&data);
   if (ret_status == SUCCESSFUL) {
     *byte_read = (byte)(data & 0xff);
   }
   
   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  READ_CONFIGURATION_WORD                                                 */
/*                                                                          */
/* Purpose: Reads a word from the configuration space of a specific device  */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to read configuration data from                            */
/*                                                                          */
/*    byte device_and_function                                              */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    byte register_number                                                  */
/*       Register Number of configuration space to read                     */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    word *word_read                                                       */
/*       Word read from Configuration Space                                 */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       BAD_REGISTER_NUMBER - Invalid Register Number                      */          
/*                                                                          */
/****************************************************************************/
int read_configuration_word(byte bus_number,
			    byte device_and_function,
			    byte register_number,
			    word *word_read)
{
   int ret_status; /* Function Return Status */
   dword data;

   /* Call read_configuration_area function with word data */
   ret_status = read_configuration_area(READ_CONFIG_WORD, 
					bus_number, 
					device_and_function, 
					register_number, 
					&data);
   if (ret_status == SUCCESSFUL) {

      /* Extract word */
      *word_read = (word)(data & 0xffff);
   }
   
   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  READ_CONFIGURATION_DWORD                                                */
/*                                                                          */
/* Purpose: Reads a dword from the configuration space of a specific device */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to read configuration data from                            */
/*                                                                          */
/*    byte device_and_function                                              */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    byte register_number                                                  */
/*       Register Number of configuration space to read                     */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    dword *dword_read                                                     */
/*       Dword read from Configuration Space                                */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       BAD_REGISTER_NUMBER - Invalid Register Number                      */          
/*                                                                          */
/****************************************************************************/

int read_configuration_dword(byte bus_number,
			     byte device_and_function,
			     byte register_number,
			     dword *dword_read)
{
   int ret_status; /* Function Return Status */
   dword data;

   /* Call read_configuration_area function with dword data */
   ret_status = read_configuration_area(READ_CONFIG_DWORD, 
					bus_number, 
					device_and_function, 
					register_number, 
					&data);
   if (ret_status == SUCCESSFUL) {

      /* Extract dword */
      *dword_read = data;
   }
   
   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  READ_CONFIGURATION_AREA                                                 */
/*                                                                          */
/* Purpose: Reads a byte/word/dword from the configuration space of a       */
/*          specific device                                                 */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to read configuration data from                            */
/*                                                                          */
/*    byte device_and_function                                              */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    byte register_number                                                  */
/*       Register Number of configuration space to read                     */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    dword *dword_read                                                     */
/*       Dword read from Configuration Space                                */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       BAD_REGISTER_NUMBER - Invalid Register Number                      */          
/*                                                                          */
/****************************************************************************/

static int read_configuration_area(byte function, 
				   byte bus_number,
				   byte device_and_function,
				   byte register_number,
				   dword *data)
{
   int ret_status; /* Function Return Status */
   word ax, flags; /* Temporary variables to hold register values */
   dword ecx;      /* Temporary variable to hold ECX register value */
   
   /* Load entry registers for PCI BIOS */
   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  WRITE_CONFIGURATION_BYTE                                                */
/*                                                                          */
/* Purpose: Writes a byte to the configuration space of a specific device   */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to write configuration data to                             */
/*                                                                          */
/*    byte device_and_function                                              */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    byte register_number                                                  */
/*       Register Number of configuration space to write                    */
/*                                                                          */
/*    byte byte_to_write                                                    */
/*       Byte to write to Configuration Space                               */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       BAD_REGISTER_NUMBER - Invalid Register Number                      */          
/*                                                                          */
/****************************************************************************/

int write_configuration_byte(byte bus_number,
			     byte device_and_function,
			     byte register_number,
			     byte byte_to_write)
{
   int ret_status; /* Function Return Status */

   /* Call write_configuration_area function with byte data */
   ret_status = write_configuration_area(WRITE_CONFIG_BYTE, 
					 bus_number, 
					 device_and_function, 
					 register_number, 
					 byte_to_write);
   return (ret_status);
}


/****************************************************************************/
/*                                                                          */
/*  WRITE_CONFIGURATION_WORD                                                */
/*                                                                          */
/* Purpose: Writes a word to the configuration space of a specific device   */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to read configuration data from                            */
/*                                                                          */
/*    byte device_and_function                                              */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    byte register_number                                                  */
/*       Register Number of configuration space to read                     */
/*                                                                          */
/*    word word_to_write                                                    */
/*       Word to write to Configuration Space                               */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       BAD_REGISTER_NUMBER - Invalid Register Number                      */          
/*                                                                          */
/****************************************************************************/

int write_configuration_word(byte bus_number,
			     byte device_and_function,
			     byte register_number,
			     word word_to_write)
{
   int ret_status; /* Function Return Status */

   /* Call read_configuration_area function with word data */
   ret_status = write_configuration_area(WRITE_CONFIG_WORD, 
					 bus_number, 
					 device_and_function, 
					 register_number, 
					 word_to_write);
   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  WRITE_CONFIGURATION_DWORD                                               */
/*                                                                          */
/* Purpose: Reads a dword from the configuration space of a specific device */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to read configuration data from                            */
/*                                                                          */
/*    byte device_and_function                                              */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    byte register_number                                                  */
/*       Register Number of configuration space to read                     */
/*                                                                          */
/*    dword dword_to_write                                                  */
/*       Dword to write to Configuration Space                              */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       BAD_REGISTER_NUMBER - Invalid Register Number                      */          
/*                                                                          */
/****************************************************************************/

int write_configuration_dword(byte bus_number,
			      byte device_and_function,
			      byte register_number,
			      dword dword_to_write)
{
   int ret_status; /* Function Return Status */

   /* Call write_configuration_area function with dword data */
   ret_status = write_configuration_area(WRITE_CONFIG_DWORD, 
					 bus_number, 
					 device_and_function, 
					 register_number, 
					 dword_to_write);
   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  WRITE_CONFIGURATION_AREA                                                */
/*                                                                          */
/* Purpose: Writes a byte/word/dword to the configuration space of a        */
/*          specific device                                                 */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    byte bus_number                                                       */
/*       PCI bus to read configuration data from                            */
/*                                                                          */
/*    byte device_and_function                                              */
/*       Device Number in upper 5 bits, Function Number in lower 3 bits     */
/*                                                                          */
/*    byte register_number                                                  */
/*       Register Number of configuration space to read                     */
/*                                                                          */
/*    dword value                                                           */
/*       Value to write to Configuration Space                              */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    Return Value - Indicates presence of device                           */
/*       SUCCESSFUL - Device found                                          */
/*       NOT_SUCCESSFUL - BIOS error                                        */
/*       BAD_REGISTER_NUMBER - Invalid Register Number                      */          
/*                                                                          */
/****************************************************************************/

static int write_configuration_area(byte function, 
				    byte bus_number,
				    byte device_and_function,
				    byte register_number,
				    dword value)
{
   int ret_status; /* Function Return Status */
   word ax, flags; /* Temporary variables to hold register values */
   
   /* Load entry registers for PCI BIOS */

   return (ret_status);
}

/****************************************************************************/
/*                                                                          */
/*  OUTPD                                                                   */
/*                                                                          */
/* Purpose: Outputs a DWORD to a hardware port                              */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word port                                                             */
/*       hardware port to write DWORD to                                    */
/*                                                                          */
/*    dword value                                                           */
/*       value to be written to port                                        */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    None                                                                  */
/*                                                                          */
/****************************************************************************/

void outpd(word port, dword value)
{
   // _DX = port;
   // _EAX = value;
   // /* Since asm cannot generate OUT  DX, EAX must force in */
   // __emit__(0x66, 0xEF);
}

/****************************************************************************/
/*                                                                          */
/*  INPD                                                                    */
/*                                                                          */
/* Purpose: Inputs a DWORD from a hardware port                             */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word port                                                             */
/*       hardware port to write DWORD to                                    */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    dword value                                                           */
/*       value read from the port                                           */
/*                                                                          */
/****************************************************************************/

dword inpd(word port)
{
   // /* Set DX register to port number to be input from */
   // _DX = port; 
   // /* Since asm cannot generate IN  EAX, DX, must force in */
   // __emit__(0x66, 0xED);

   return(0);
}

/****************************************************************************/
/*                                                                          */
/*  INSB                                                                    */
/*                                                                          */
/* Purpose: Inputs a string of BYTEs from a hardware port                   */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word port                                                             */
/*       hardware port to read from                                         */
/*                                                                          */
/*    void *buf                                                             */
/*       Buffer to read data into                                           */
/*                                                                          */
/*    int count                                                             */
/*       Number of BYTEs to read                                            */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    None                                                                  */
/*                                                                          */
/****************************************************************************/

void insb(word port, void *buf, int count)
{
   // _ES = FP_SEG(buf);   /* Segment of buf */
   // _DI = FP_OFF(buf);   /* Offset of buf  */
   // _CX = count;         /* Number to read */
   // _DX = port;          /* Port           */
   // asm   REP INSB;
}

/****************************************************************************/
/*                                                                          */
/*  INSW                                                                    */
/*                                                                          */
/* Purpose: Inputs a string of WORDs from a hardware port                   */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word port                                                             */
/*       hardware port to read from                                         */
/*                                                                          */
/*    void *buf                                                             */
/*       Buffer to read data into                                           */
/*                                                                          */
/*    int count                                                             */
/*       Number of WORDs to read                                            */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    None                                                                  */
/*                                                                          */
/****************************************************************************/

void insw(word port, void *buf, int count)
{
   // _ES = FP_SEG(buf);   /* Segment of buf */
   // _DI = FP_OFF(buf);   /* Offset of buf  */
   // _CX = count;         /* Number to read */
   // _DX = port;          /* Port           */
   // asm   REP INSW;
}

/****************************************************************************/
/*                                                                          */
/*  INSB                                                                    */
/*                                                                          */
/* Purpose: Inputs a string of DWORDs from a hardware port                  */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word port                                                             */
/*       hardware port to read from                                         */
/*                                                                          */
/*    void *buf                                                             */
/*       Buffer to read data into                                           */
/*                                                                          */
/*    int count                                                             */
/*       Number of DWORDs to read                                           */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    None                                                                  */
/*                                                                          */
/****************************************************************************/

void insd(word port, void *buf, int count)
{
   // _ES = FP_SEG(buf);   /* Segment of buf */
   // _DI = FP_OFF(buf);   /* Offset of buf  */
   // _CX = count;         /* Number to read */
   // _DX = port;          /* Port           */
   // __emit__(0xf3, 0x66, 0x6D); /* asm  REP INSD */
}

/****************************************************************************/
/*                                                                          */
/*  OUTSB                                                                   */
/*                                                                          */
/* Purpose: Outputs a string of BYTEs to a hardware port                    */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word port                                                             */
/*       hardware port to write to                                          */
/*                                                                          */
/*    void *buf                                                             */
/*       Buffer to write data from                                          */
/*                                                                          */
/*    int count                                                             */
/*       Number of BYTEs to write                                           */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    None                                                                  */
/*                                                                          */
/****************************************************************************/

void outsb(word port, void *buf, int count)
{
   // _SI = FP_SEG(buf);   /* Segment of buf  */
   // _DI = FP_OFF(buf);   /* Offset of buf  */
   // _CX = count;         /* Number to read */
   // _DX = port;          /* Port           */
   // asm   REP OUTSB;
}

/****************************************************************************/
/*                                                                          */
/*  OUTSW                                                                   */
/*                                                                          */
/* Purpose: Outputs a string of WORDs to a hardware port                    */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word port                                                             */
/*       hardware port to write to                                          */
/*                                                                          */
/*    void *buf                                                             */
/*       Buffer to write data from                                          */
/*                                                                          */
/*    int count                                                             */
/*       Number of WORDs to write                                           */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    None                                                                  */
/*                                                                          */
/****************************************************************************/

void outsw(word port, void *buf, int count)
{
   // _SI = FP_SEG(buf);   /* Segment of buf  */
   // _DI = FP_OFF(buf);   /* Offset of buf  */
   // _CX = count;         /* Number to read */
   // _DX = port;          /* Port           */
   // asm   REP OUTSW;
}

/****************************************************************************/
/*                                                                          */
/*  OUTSD                                                                   */
/*                                                                          */
/* Purpose: Outputs a string of DWORDS to a hardware port                   */
/*                                                                          */
/* Inputs:                                                                  */
/*                                                                          */
/*    word port                                                             */
/*       hardware port to write to                                          */
/*                                                                          */
/*    void *buf                                                             */
/*       Buffer to write data from                                          */
/*                                                                          */
/*    int count                                                             */
/*       Number of DWORDs to write                                          */
/*                                                                          */
/* Outputs:                                                                 */
/*                                                                          */
/*    None                                                                  */
/*                                                                          */
/****************************************************************************/

void outsd(word port, void *buf, int count)
{
   // _SI = FP_SEG(buf);   /* Segment of buf  */
   // _DI = FP_OFF(buf);   /* Offset of buf  */
   // _CX = count;         /* Number to read */
   // _DX = port;          /* Port           */
   // __emit__(0xf3, 0x66, 0x6F); /* asm   REP OUTSD; */
}

void IOPermissions()
{
  if(iopl(3)<0)
  {
     endwin();
     fprintf(stderr, "NO IO PERMISSION\n");
     _exit(1);
  }
}
