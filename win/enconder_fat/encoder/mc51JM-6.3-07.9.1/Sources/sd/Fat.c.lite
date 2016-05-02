/******************************************************************************
*                                                  
*  (c) copyright Freescale Semiconductor 2011
*  ALL RIGHTS RESERVED    
*
*  File Name:   Fat.c
*                                                                          
*  Description: Fat16 lite driver 
*                                                                                     
*  Assembler:   Codewarrior for HC(S)08 V6.3
*                                            
*  Version:     1.3                                                         
*                                                                                                                                                         
*  Author:      Jose Ruiz (SSE Americas)
*                                                                                       
*  Location:    Guadalajara,Mexico                                              
*                                                                                                                  
*                                                  
* UPDATED HISTORY:
*
* REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
* ---   ----------  ------        --------------------- 
* 1.0   2008.02.18  Jose Ruiz     Initial version
* 1.1   2008.05.02  Jose Ruiz     Initial version
* 1.2   2010.12.13  Santiago L    Corrected problem handling more of 16 files
*                                 Added Modify Function
* 1.3   2011.05.20  Carlos C.     Corrected problem handling large-size files    
* 1.4   2012.12.28  EQUISER       Added MBR parser to quickly find first partition instead of looking for EB 3C 90 
*                                 Corrected problem handling long filenames
*                                 Corrected problem when trying to close a file after deleting it
* 1.5   2013.01.18  EQUISER       Added functionality to file read function, now the function returns the amount of readed bytes of the data buffer provided, and stores the number of the last physical sector readed in a pointer provided.                                 
* 1.6   2013.02.04  EQUISER       Added function to get file information
* 1.7   2013.03.01  EQUISER       Added function to rename files
******************************************************************************/                                                                        
/* Freescale  is  not  obligated  to  provide  any  support, upgrades or new */
/* releases  of  the Software. Freescale may make changes to the Software at */
/* any time, without any obligation to notify or provide updated versions of */
/* the  Software  to you. Freescale expressly disclaims any warranty for the */
/* Software.  The  Software is provided as is, without warranty of any kind, */
/* either  express  or  implied,  including, without limitation, the implied */
/* warranties  of  merchantability,  fitness  for  a  particular purpose, or */
/* non-infringement.  You  assume  the entire risk arising out of the use or */
/* performance of the Software, or any systems you design using the software */
/* (if  any).  Nothing  may  be construed as a warranty or representation by */
/* Freescale  that  the  Software  or  any derivative work developed with or */
/* incorporating  the  Software  will  be  free  from  infringement  of  the */
/* intellectual property rights of third parties. In no event will Freescale */
/* be  liable,  whether in contract, tort, or otherwise, for any incidental, */
/* special,  indirect, consequential or punitive damages, including, but not */
/* limited  to,  damages  for  any loss of use, loss of time, inconvenience, */
/* commercial loss, or lost profits, savings, or revenues to the full extent */
/* such  may be disclaimed by law. The Software is not fault tolerant and is */
/* not  designed,  manufactured  or  intended by Freescale for incorporation */
/* into  products intended for use or resale in on-line control equipment in */
/* hazardous, dangerous to life or potentially life-threatening environments */
/* requiring  fail-safe  performance,  such  as  in the operation of nuclear */
/* facilities,  aircraft  navigation  or  communication systems, air traffic */
/* control,  direct  life  support machines or weapons systems, in which the */
/* failure  of  products  could  lead  directly to death, personal injury or */
/* severe  physical  or  environmental  damage  (High  Risk Activities). You */
/* specifically  represent and warrant that you will not use the Software or */
/* any  derivative  work of the Software for High Risk Activities.           */
/* Freescale  and the Freescale logos are registered trademarks of Freescale */
/* Semiconductor Inc.                                                        */ 
/*****************************************************************************/
/*****************************************************************************/
/*EQUISER: E Volentini, G. Rosemberg. Special Thanks to G. Onativia          */
/*****************************************************************************/
             
#pragma warn_implicitconv off 

/* Includes */
#include "Fat.h"

/* File Handlers */
WriteRHandler WHandler;
ReadRHandler RHandler;

/* File Buffers */
UINT8 ag8FATReadBuffer[512];
UINT8 ag8FATWriteBuffer[512];

/* Global Variables */
UINT16 u16FAT_Sector_Size;
UINT16 u16FAT_Cluster_Size;
UINT16 u16FAT_FAT_BASE;
UINT16 u16FAT_Root_BASE;
UINT16 u16FAT_Data_BASE;
UINT16 u16Main_Offset=0;


/***************************************************************************************/
UINT32 LWordSwap(UINT32 u32DataSwap)
{
    UINT32 u32Temp;
    u32Temp= (u32DataSwap & 0xFF000000) >> 24;
    u32Temp+=(u32DataSwap & 0xFF0000)   >> 8;
    u32Temp+=(u32DataSwap & 0xFF00)     << 8;
    u32Temp+=(u32DataSwap & 0xFF)       << 24;
    return(u32Temp);    
}

/***************************************************************************************/
void FAT_Read_Master_Block(void)
{
    static MasterBoot_Entries *pMasterBoot; //Make static for debug
    static PartitionTable_Entries *PartitionTable; //Make static for debug
    
    /*
    Opcion 1: Partiton Loop
    Este bucle permite posisionarse en la primera particion sin tener que consultar el MBR para
    determinar su ubicacion. Para esto recorre secuancialmente el volumen, byte por byte, hasta
    encontrar el patron EB 3C 90, que identifica a la misma. Luego obtiene el sector correspond
    a la particion para contar con la informacion necesaria para poder montar el sistema de 
    archivos.
    + Simple.
    - Solo pueden accederse archivos ubicados en la primera particion.
    - El tiempo requerido para montar el FS es variable y esta sujeto a que tan proxima se 
      encuentre la primera particion al inicio del volumen (sector 0). Ademas, en caso de ubicarse
      otra particion en las direcciones mas bajas del volumen, esta podria contener archivos con
      el patron EB 3C 90 y provocar que este metodo no funcione.
    
    while(ag8FATReadBuffer[0]!= 0xEB || ag8FATReadBuffer[1]!=0x3C || ag8FATReadBuffer[2]!=0x90) //Recorre linealmente el volumen en busca del patron que identifica a la primera particion
    {
        GetPhysicalBlock(u16Main_Offset++,&ag8FATReadBuffer[0]);  //Si encuentra el patron, obtiene el bloque y lo carga en ag8FATReadBuffer
        __RESET_WATCHDOG();
    }
    u16Main_Offset--; //Corrige la direccion donde se encuentra la primera particion
    */
    
    /*
    Opcion 2: MBR Parser
    En lugar de recorrer linealmente el volumen hasta dar con la primera particion, se obtiene
    el primer bloque (sector 0, donde reside el MBR) y se analiza el campo del MBR donde se 
    especifica la dirección, en numero de bloques, de la primer particion.
    + No muy complejo.
    + El tiempo requerido para montar el FS es siempre el mismo, ya que no es necesario recorrer
      el volumen para obtener la direccion de las particiones, sino que se obtienen luego de interpretar 
      el MBR, ubicado siempre en el sector 0 del volumen.
    + Permite montar cualquier particion, no solo la primera, con leves cambios en el codigo.    
    */    
    GetPhysicalBlock(0,&ag8FATReadBuffer[0]); //Carga el sector0=MBR en ag8FATReadBuffer (arreglo de bytes sin estructura)
    PartitionTable=(PartitionTable_Entries*)ag8FATReadBuffer; //Carga el contenido del buffer ag8FATReadBuffer en la estructura PartitionTable_Entries, que posee los campos correspondientes a los campos del MBR 
    if(PartitionTable->MBRSignature!=0x55AA) return;  // Firma de cierre del MBR, sino error
    u16Main_Offset=(UINT16)LWordSwap(PartitionTable->Partition[0].LBAStart);  //Calcula direccion de la primera particion
    GetPhysicalBlock(u16Main_Offset,&ag8FATReadBuffer[0]);  //Obtiene bloque que contiene la primera particion
    if(ag8FATReadBuffer[0]!= 0xEB || ag8FATReadBuffer[1]!=0x3C || ag8FATReadBuffer[2]!=0x90) return;  //Verifica que el patron de inicio de la primera particion sea EB 3C 90, sino error  
   
    /*
    Carga informacion de la particion en la estructura correspondiente al MBR y en otras variables
    */
    pMasterBoot=(MasterBoot_Entries*)ag8FATReadBuffer;
    u16FAT_Cluster_Size=pMasterBoot->SectorsPerCluster; //Tamano del cluster en sectores, usualmente 16Sectores 
    u16FAT_Sector_Size=ByteSwap(pMasterBoot->BytesPerSector); //Tamano del sector, usualmente 512Bytes 
    u16FAT_FAT_BASE=  u16Main_Offset+ByteSwap(pMasterBoot->ReservedSectors);  //Sectores reservados para almacenar copias de respaldo de la FAT
    u16FAT_Root_BASE= (ByteSwap(pMasterBoot->SectorsPerFat)<<1)+u16FAT_FAT_BASE;  //Direccion del Root Directory
    u16FAT_Data_BASE= (ByteSwap(pMasterBoot->RootDirectoryEntries) >>4)+u16FAT_Root_BASE; //Direccion del primer archivo de la particion
    
    /*Es necesario aplicar un Byte Swap en ciertos campos del MBR para interpretarlos correctamente, ya que algunos por definicion estan escritos en Little Endian y otros en Big Endian*/

}
/***************************************************************************************/
/*void FAT_LS(void)
{
    UINT8 u8Counter;
    root_Entries *sFileStructure;                                   

    GetPhysicalBlock(u16FAT_Root_BASE,ag8FATReadBuffer);
    sFileStructure = (root_Entries*)&ag8FATReadBuffer[RootEntrySize];
    while(sFileStructure->FileName[0]!=FILE_Clear)
    {
        if(sFileStructure->FileName[0]!=FILE_Erased)
        {
            Terminal_Send_String((UINT8*)"\r\n");
            for(u8Counter=0;u8Counter<8;u8Counter++)
                if(sFileStructure->FileName[u8Counter]!=' ')
                    Terminal_Send_Byte(sFileStructure->FileName[u8Counter]);
            Terminal_Send_Byte('.');
            for(u8Counter=0;u8Counter<3;u8Counter++)
                if(sFileStructure->Extension[u8Counter]!=' ')
                    Terminal_Send_Byte(sFileStructure->Extension[u8Counter]);
        }
        sFileStructure++;    
    }
}*/
/***************************************************************************************/
void FAT_FileClose(void)
/*
Al cerrar el archivo se actualiza la informacion de todas las estructuras del FS que posean
referencias al mismo (entrada del archivo en Root Directory y FAT).
*/
{
  root_Entries *sFileStructure;
  UINT16 *pu16FATPointer;
  UINT8 u8Counter;
  volatile UINT32 u32Sector;
  volatile UINT16 u16Offset;
    
    /* Directory Entry*/
    u32Sector=WHandler.Dir_Entry/(u16FAT_Sector_Size>>5);
    u16Offset=WHandler.Dir_Entry%(u16FAT_Sector_Size>>5);
    
    GetPhysicalBlock(u16FAT_Root_BASE+u32Sector,ag8FATReadBuffer);
    sFileStructure=(root_Entries*)ag8FATReadBuffer;
    sFileStructure+=u16Offset;

    // FileName
    for(u8Counter=0;u8Counter<8;u8Counter++)
        sFileStructure->FileName[u8Counter]=WHandler.FileName[u8Counter];

    // Entension
    for(u8Counter=0;u8Counter<3;u8Counter++)
        sFileStructure->Extension[u8Counter]=WHandler.Extension[u8Counter];


    // Attributes
    sFileStructure->Attributes=0x20;
    sFileStructure->_Case=0x18;
    sFileStructure->MiliSeconds=0xC6;
    
    // Date & Time Information
    sFileStructure->CreationTime=0x2008;
    sFileStructure->CreationDate=0x2136;
    sFileStructure->AccessDate=0x2136;
    sFileStructure->ModificationTime=0x2008;
    sFileStructure->ModificationDate=0x2136;
    
    // Fat entry and file Size
    sFileStructure->ClusterNumber=ByteSwap(WHandler.BaseFatEntry);
    
    sFileStructure->SizeofFile=LWordSwap(WHandler.File_Size); 

    StorePhysicalBLock(u16FAT_Root_BASE+u32Sector,ag8FATReadBuffer);
    

    /* FAT Table */  
    if(WHandler.FileName[0] != FILE_Erased){
      
      u32Sector=WHandler.CurrentFatEntry/(u16FAT_Sector_Size>>1);
      u16Offset=WHandler.CurrentFatEntry%(u16FAT_Sector_Size>>1);

      GetPhysicalBlock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer);
          
      pu16FATPointer=(UINT16*)ag8FATReadBuffer;
      pu16FATPointer+=u16Offset;
      *pu16FATPointer=0xFFFF;     // Write Final Cluster  
   
      StorePhysicalBLock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer)
    }
}
/***************************************************************************************/
UINT16 FAT_SearchAvailableFAT(UINT16 u16CurrentFAT)
{
    UINT16 *pu16DataPointer;
    UINT16 u16FatEntry=0;
    UINT16 u16Sector=0;                   
    UINT16 u16byteSector;
    UINT16 u16SectorSize=0;
    
    u16Sector=u16FAT_FAT_BASE;
    while(u16Sector < (((u16FAT_Root_BASE-u16FAT_FAT_BASE)>>1)+u16Main_Offset))
    {        
        GetPhysicalBlock(u16Sector++,ag8FATReadBuffer);
        pu16DataPointer=(UINT16*)ag8FATReadBuffer;
        u16byteSector=0;
        u16SectorSize = ((u16FAT_Sector_Size)>> 1);        
        
        while(u16byteSector<u16SectorSize)
        {
            if(*pu16DataPointer==0x0000)
                if(u16FatEntry!=u16CurrentFAT)
                    return(u16FatEntry);
            pu16DataPointer++;
            u16FatEntry++;
            u16byteSector++;
        }
    }
    return(0);  // Return 0 if no more FAT positions available
}

/***************************************************************************************/
UINT16 FAT_Entry(UINT16 u16FatEntry,UINT16 u16FatValue, UINT8 u8Function)
{
    UINT16 *pu16DataPointer;
    
    UINT16 u16Block;
    UINT8 u8Offset;
    
    u16Block = u16FatEntry / (u16FAT_Sector_Size>>1);
    u8Offset = (UINT8)(u16FatEntry % (u16FAT_Sector_Size >>1));

    GetPhysicalBlock(u16FAT_FAT_BASE+u16Block,ag8FATReadBuffer);
    pu16DataPointer=(UINT16*)ag8FATReadBuffer;
    pu16DataPointer+=u8Offset;

    if(u8Function==NEXT_ENTRY)
        return(ByteSwap(*pu16DataPointer));
    
    if(u8Function==WRITE_ENTRY)
    {
        *pu16DataPointer=ByteSwap(u16FatValue);
        StorePhysicalBLock(u16FAT_FAT_BASE+u16Block,ag8FATReadBuffer);
        return(0x00);
    }
                
}

/***************************************************************************************/
void FAT_FileWrite(UINT8 *pu8DataPointer,UINT32 u32Size)
{
    UINT32 u32SectorToWrite;
    UINT8 *pu8ArrayPointer;
    UINT16 u16TempFat;
    UINT8  u8ChangeSector=1;

    while(u32Size)
    {
        if(u8ChangeSector)
        {
            u32SectorToWrite= u16FAT_Data_BASE + WHandler.ClusterIndex + (WHandler.CurrentFatEntry-2)*u16FAT_Cluster_Size;
            GetPhysicalBlock(u32SectorToWrite,ag8FATWriteBuffer); 
            pu8ArrayPointer=ag8FATWriteBuffer+WHandler.SectorIndex;
            u8ChangeSector=0;
        }
        
        while(WHandler.SectorIndex<u16FAT_Sector_Size  &&  u32Size)
        {
            u32Size--;    
            WHandler.SectorIndex++;
            WHandler.File_Size++;
            *pu8ArrayPointer++=*pu8DataPointer++;    
        }
        
        StorePhysicalBLock(u32SectorToWrite,ag8FATWriteBuffer);     // Write Buffer to Sector
    
        /* Check Sector Size */
        if(WHandler.SectorIndex == u16FAT_Sector_Size)
        {
            WHandler.SectorIndex=0;
            WHandler.ClusterIndex++;    
            u8ChangeSector=1;
        }
    
        /* Check Cluster Size */
        
        if(WHandler.ClusterIndex == u16FAT_Cluster_Size)  
        {
            //_BGND;
            WHandler.ClusterIndex=0;
            u16TempFat=FAT_SearchAvailableFAT(WHandler.CurrentFatEntry);   
             
            if(!u16TempFat)
            //_BGND;
            (void)FAT_Entry(WHandler.CurrentFatEntry,u16TempFat,WRITE_ENTRY);
            WHandler.CurrentFatEntry=u16TempFat;
            u8ChangeSector=1;
        }
        
    }
}
/***************************************************************************************/
UINT16 FAT_FileRead(UINT8 *pu8UserBuffer, UINT32 *pu32SectorReaded)
{
    UINT32 u32SectorToRead; 
    UINT16 u16BufferSize;

    if(RHandler.File_Size==0)
        return(0);
    
    //_BGND;
    
    u32SectorToRead= u16FAT_Data_BASE + ((RHandler.FAT_Entry-2)*u16FAT_Cluster_Size)+RHandler.SectorOffset;
    
    if(pu32SectorReaded!=0) *pu32SectorReaded=u32SectorToRead;
        
             
    GetPhysicalBlock(u32SectorToRead,pu8UserBuffer);
    

    if(RHandler.File_Size > u16FAT_Sector_Size)
    {
        RHandler.File_Size-=u16FAT_Sector_Size;
        u16BufferSize=512;
    }
    else
    {
        u16BufferSize=(UINT16)RHandler.File_Size;
        RHandler.File_Size=0;
    }
    
    if(RHandler.SectorOffset < (u16FAT_Cluster_Size)-1)
        RHandler.SectorOffset++;        
    else
    {
            RHandler.SectorOffset=0;
            RHandler.FAT_Entry = FAT_Entry(RHandler.FAT_Entry,0,NEXT_ENTRY); // Get Next FAT Entry
    }
    return(u16BufferSize);    
}
/***************************************************************************************/
UINT8 FAT_GetFileInfo(ReadRHandler* puFileHandler)
//Use after FAT_FileOpen(FILE_NAME, READ)
//FAT_FileRead altera la estructura ReadRHandler, siempre usar FAT_FileOpen primero para restaurar los valores adecuados de la estructura
{
    if(RHandler.File_Size==0) return(0);
    puFileHandler->FAT_Entry=RHandler.FAT_Entry;
    puFileHandler->SectorOffset=RHandler.SectorOffset;
    puFileHandler->Dir_Entry=RHandler.Dir_Entry;
    puFileHandler->File_Size=RHandler.File_Size;
    return(1);   
}

/***************************************************************************************/
void FAT_FileNameOrganizer(UINT8 *pu8FileName,UINT8 *pu8Destiny)
{
    UINT8 u8Counter=0;    
    
    while(u8Counter<12)
    {
        if(*pu8FileName != '.')
            *pu8Destiny++=*pu8FileName++;
        else
        {
            if(u8Counter<8)
                *pu8Destiny++=0x20;
            else
                pu8FileName++;    
        }
        u8Counter++;
    }
}
/***************************************************************************************/
UINT8 FAT_FileOpen(UINT8 *pu8FileName,UINT8 u8Function)
{
    /*
    Opciones:
    - CREATE: Crea el archivo, requiere FAT_FileClose()
    - MODIFY: Abre el archivo para escritura. Escribe al final del archivo (append). Requiere FAT_FileClose()
    - OVERWRITE: Abre el archivo para escritura. Escribe desde el comienzo del archivo. Requiere FAT_FileClose()
    - DELETE: Elimina el archivo. Requiere FAT_FileClose()   
    */
    UINT16 u16Temporal;
    UINT8  u8FileName[11];
    UINT8  u8Counter=0;
    UINT8  u8Flag=False;
    UINT16 u16Index;
    UINT16 u16Block;
    UINT16 u16BlockNum=u16FAT_Data_BASE-u16FAT_Root_BASE;  //751 - 719 = 32
    UINT8  u8ErrorCode=ERROR_IDLE; 
    UINT8  *pu8Pointer;
    UINT16 *pu16FATPointer;
    UINT32 u32Sector;
    UINT16 u16Offset;
    root_Entries *sFileStructure;                                   
    
    FAT_FileNameOrganizer(pu8FileName,&u8FileName[0]);
    
    u16Block=0;
    
    while(u16Block < u16BlockNum && u8ErrorCode==ERROR_IDLE)  
    {
    
        GetPhysicalBlock(u16FAT_Root_BASE+u16Block,ag8FATReadBuffer);
        sFileStructure = (root_Entries*)ag8FATReadBuffer;

        u16Index=0;
        while(u16Index<u16FAT_Sector_Size && u8ErrorCode==ERROR_IDLE) //Recorre todos los archivos de la particion   
        {
            if(sFileStructure->FileName[0]!=FILE_Erased && sFileStructure->Attributes==AT_LFN){ 
              /*
              Se compara el primer caracter del nombre del archivo con el caracter especial 0xE5. FAT especifica que los nombres 
              de archivo que comienzan con este caracter fueron borrados.              
              Permite distinguir las entradas de nombre largo en el Root Directory y operar en consecuencia. 
              Las entradas tradicionales, o de nombre corto, ocupan 32Bytes, mientras que las de nombre largo ocupan 128Bytes, 
              por esta razon es necesario corregir el indice que se utiliza para recorrer el Root Directory,
              para que se desplaze mas o menos bytes de acuerdo al tipo de entrada que se trate.
              Funciona unicamente cuando todas las entradas de nombre largo se encuentran en el mismo sector. 
              */
              sFileStructure+=2;
              u16Index+=2*RootEntrySize;
              continue;
            }
                        
            /* If Read or Modify Function */
            if(u8Function==READ || u8Function==MODIFY|| u8Function==DELETE || u8Function==OVERWRITE)
            {
                if(sFileStructure->FileName[0]==FILE_Clear) 
                    u8ErrorCode=FILE_NOT_FOUND;
                
                if(sFileStructure->FileName[0] == u8FileName[0])
                {
                    u8Flag=True;
                    u8Counter=0;
                    while(u8Flag==True && u8Counter < 10)
                    //Verifica si el archivo correspondiente a la entrada actual en el Root Directory es el que se busca
                    {
                        u8Counter++;
                        if(sFileStructure->FileName[u8Counter] != u8FileName[u8Counter])
                            u8Flag=False;    
                    }
                    if(u8Flag==True)
                    {
                        /* If Read Function */
                        if(u8Function==READ)
                        {
                            RHandler.Dir_Entry=(u16Block*EntriesPerBlock)+((u16Index)/RootEntrySize);
                            RHandler.File_Size=LWordSwap(sFileStructure->SizeofFile);
                            RHandler.FAT_Entry=ByteSwap(sFileStructure->ClusterNumber);
                            RHandler.SectorOffset=0;
                            u8ErrorCode=FILE_FOUND;
                        } 
                        
                        /*If Delete Function //Leandro Martinez*/     
                        else if(u8Function==DELETE)
                        {
                            WHandler.FileName[0] = FILE_Erased;
                            WHandler.Dir_Entry=(u16Block*EntriesPerBlock)+((u16Index)/RootEntrySize);
                            WHandler.BaseFatEntry=ByteSwap(sFileStructure->ClusterNumber);
                            if(WHandler.BaseFatEntry != 0)
                            {
                                u16Temporal=WHandler.BaseFatEntry;
                                do
                                {
                                    WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                                    WHandler.BaseFatEntry=FAT_Entry(WHandler.CurrentFatEntry,0,NEXT_ENTRY);
                                    u32Sector=WHandler.CurrentFatEntry/(u16FAT_Sector_Size>>1);
                                    u16Offset=WHandler.CurrentFatEntry%(u16FAT_Sector_Size>>1);

                                    GetPhysicalBlock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer);
    
                                    pu16FATPointer=(UINT16*)ag8FATReadBuffer;
                                    pu16FATPointer+=u16Offset;
                                    *pu16FATPointer=0x0000;     // clear FAT entry    

                                    StorePhysicalBLock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer)                                
                                
                                }while(WHandler.BaseFatEntry!=0xFFFF); 
                                
                                
                                WHandler.BaseFatEntry=u16Temporal;
                                
                                u32Sector=WHandler.CurrentFatEntry/(u16FAT_Sector_Size>>1);
                                u16Offset=WHandler.CurrentFatEntry%(u16FAT_Sector_Size>>1);

                                GetPhysicalBlock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer);
    
                                pu16FATPointer=(UINT16*)ag8FATReadBuffer;
                                pu16FATPointer+=u16Offset;
                                *pu16FATPointer=0x0000;     // clear FAT entry    

                                StorePhysicalBLock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer)
                                                                
                                
                            }
                            
                              /*u32Sector=WHandler.BaseFatEntry/(u16FAT_Sector_Size>>1);
                                u16Offset=WHandler.BaseFatEntry%(u16FAT_Sector_Size>>1);

                                GetPhysicalBlock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer);
    
                                pu16FATPointer=(UINT16*)ag8FATReadBuffer;
                                pu16FATPointer+=u16Offset;
                                *pu16FATPointer=0x0000;     // clear FAT entry    

                                StorePhysicalBLock(u16FAT_FAT_BASE+u32Sector,ag8FATReadBuffer)

                                */
                            
                            //WHandler.BaseFatEntry = FILE_Index_Clear;
                            //WHandler.CurrentFatEntry = FILE_Index_Clear;
                            //WHandler.ClusterIndex = FILE_Index_Clear;
                            
                            
                        }
                        
                        /* If Overwrite Function */
                        else if (u8Function==OVERWRITE)
                        {
                            pu8Pointer=WHandler.FileName;
                            for(u8Counter=0;u8Counter<11;u8Counter++)
                                *pu8Pointer++=u8FileName[u8Counter];
                            WHandler.Dir_Entry=(u16Block*EntriesPerBlock)+((u16Index)/RootEntrySize);
                            WHandler.File_Size=LWordSwap(sFileStructure->SizeofFile);
                            WHandler.BaseFatEntry=ByteSwap(sFileStructure->ClusterNumber);
                            
                            if(WHandler.BaseFatEntry != 0)
                            {
                                u16Temporal=WHandler.BaseFatEntry;
                                do
                                {
                                    WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                                    WHandler.BaseFatEntry=FAT_Entry(WHandler.CurrentFatEntry,0,NEXT_ENTRY);
                                }while(WHandler.BaseFatEntry!=0xFFFF);
                                WHandler.BaseFatEntry=u16Temporal;
                            } 
                            else
                            {
                                WHandler.BaseFatEntry=FAT_SearchAvailableFAT(0);
                                WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                            }
                             /*****************/
                             /* Double Check */
                             /*****************/
                            
                            u8Counter=0;
                            u8ErrorCode=u16FAT_Cluster_Size;
                            while(u8ErrorCode != 0x01)
                            {
                                u8Counter++;
                                u8ErrorCode=u8ErrorCode>>1;
                            }
                            
                            u16Temporal=(UINT16)WHandler.File_Size % (u16FAT_Sector_Size<<u8Counter);
                            WHandler.ClusterIndex= u16Temporal/u16FAT_Sector_Size;
                            WHandler.SectorIndex=  0;//u16Temporal%u16FAT_Sector_Size;
                            u8ErrorCode=FILE_FOUND;
                        }
                        
                       /* If Modify Function */
                        else
                        {
                            pu8Pointer=WHandler.FileName;
                            for(u8Counter=0;u8Counter<11;u8Counter++)
                                *pu8Pointer++=u8FileName[u8Counter];
                            WHandler.Dir_Entry=(u16Block*EntriesPerBlock)+((u16Index)/RootEntrySize);
                            WHandler.File_Size=LWordSwap(sFileStructure->SizeofFile);
                            WHandler.BaseFatEntry=ByteSwap(sFileStructure->ClusterNumber);
                            
                            if(WHandler.BaseFatEntry != 0)
                            {
                                u16Temporal=WHandler.BaseFatEntry;
                                do
                                {
                                    WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                                    WHandler.BaseFatEntry=FAT_Entry(WHandler.CurrentFatEntry,0,NEXT_ENTRY);
                                }while(WHandler.BaseFatEntry!=0xFFFF);
                                WHandler.BaseFatEntry=u16Temporal;
                            } 
                            else
                            {
                                WHandler.BaseFatEntry=FAT_SearchAvailableFAT(0);
                                WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                            }
                             /*****************/
                             /* Double Check */
                             /*****************/
                            
                            u8Counter=0;
                            u8ErrorCode=u16FAT_Cluster_Size;
                            while(u8ErrorCode != 0x01)
                            {
                                u8Counter++;
                                u8ErrorCode=u8ErrorCode>>1;
                            }
                            
                            u16Temporal=(UINT16)WHandler.File_Size % (u16FAT_Sector_Size<<u8Counter);
                            WHandler.ClusterIndex= u16Temporal/u16FAT_Sector_Size;
                            WHandler.SectorIndex= u16Temporal%u16FAT_Sector_Size;
                            u8ErrorCode=FILE_FOUND;
                        }
                        
                    }
                }
            }

            /* If Write function */
            if(u8Function==CREATE)
            {
                if(sFileStructure->FileName[0]==FILE_Clear || sFileStructure->FileName[0]==FILE_Erased) 
                {
                    pu8Pointer=WHandler.FileName;
                    for(u8Counter=0;u8Counter<11;u8Counter++)
                        *pu8Pointer++=u8FileName[u8Counter];

                    WHandler.Dir_Entry=(u16Block*EntriesPerBlock)+((u16Index)/RootEntrySize);
                    WHandler.File_Size=0;
                    WHandler.BaseFatEntry=FAT_SearchAvailableFAT(0);
                    WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                    WHandler.ClusterIndex=0;
                    WHandler.SectorIndex=0;
        
                    if(WHandler.BaseFatEntry)
                        u8ErrorCode=FILE_CREATE_OK;
                    else
                        u8ErrorCode=NO_FAT_ENTRY_AVAIlABLE;
                }
            }
                      
            sFileStructure++; //Desplaza el puntero a la siguiente entrada en el Root Directory
            u16Index+=RootEntrySize;  //Desplaza el indice utilizado para recorrer el Root Directory
        }
        u16Block++;
    }
    if(u16BlockNum==u16Block)
        u8ErrorCode=NO_FILE_ENTRY_AVAILABLE;
    
    return(u8ErrorCode);

}

/***************************************************************************************/
UINT8 FAT_FileRename(UINT8 *pu8FileName,UINT8 *pu8NewName)
{
    UINT16 u16Temporal;
    UINT8  u8FileName[11];  //No almacena el punto
    UINT8  u8NewName[11];   //No almacena el punto
    UINT8  u8Counter=0;
    UINT8  u8Flag=False;
    UINT16 u16Index;
    UINT16 u16Block;
    UINT16 u16BlockNum=u16FAT_Data_BASE-u16FAT_Root_BASE;  //751 - 719 = 32
    UINT8  u8ErrorCode=ERROR_IDLE; 
    UINT8  *pu8Pointer;
    //UINT16 *pu16FATPointer;
    //UINT32 u32Sector;
    //UINT16 u16Offset;
    root_Entries *sFileStructure;       
    
    FAT_FileNameOrganizer(pu8FileName,&u8FileName[0]);
    FAT_FileNameOrganizer(pu8NewName, &u8NewName[0]);
    
    u16Block=0;
    
    while(u16Block < u16BlockNum && u8ErrorCode==ERROR_IDLE)  
    {
    
        GetPhysicalBlock(u16FAT_Root_BASE+u16Block,ag8FATReadBuffer);
        sFileStructure = (root_Entries*)ag8FATReadBuffer;

        u16Index=0;
        while(u16Index<u16FAT_Sector_Size && u8ErrorCode==ERROR_IDLE) //Recorre todos los archivos de la particion   
        {
            if(sFileStructure->FileName[0]!=FILE_Erased && sFileStructure->Attributes==AT_LFN){ 
              /*
              Se compara el primer caracter del nombre del archivo con el caracter especial 0xE5. FAT especifica que los nombres 
              de archivo que comienzan con este caracter fueron borrados.              
              Permite distinguir las entradas de nombre largo en el Root Directory y operar en consecuencia. 
              Las entradas tradicionales, o de nombre corto, ocupan 32Bytes, mientras que las de nombre largo ocupan 128Bytes, 
              por esta razon es necesario corregir el indice que se utiliza para recorrer el Root Directory,
              para que se desplaze mas o menos bytes de acuerdo al tipo de entrada que se trate.
              Funciona unicamente cuando todas las entradas de nombre largo se encuentran en el mismo sector. 
              */
              sFileStructure+=2;
              u16Index+=2*RootEntrySize;
              continue;
            }
            
            if(sFileStructure->FileName[0]==FILE_Clear) 
                u8ErrorCode=FILE_NOT_FOUND;
            
            if(sFileStructure->FileName[0] == u8FileName[0])
            {
                u8Flag=True;
                u8Counter=0;
                while(u8Flag==True && u8Counter < 10)
                //Verifica si el archivo correspondiente a la entrada actual en el Root Directory es el que se busca
                {
                    u8Counter++;
                    if(sFileStructure->FileName[u8Counter] != u8FileName[u8Counter])
                        u8Flag=False;    
                }
                if(u8Flag==True)
                {
                    /* Based on "If Modify Function" from FAT_FileOpen */
                    pu8Pointer=WHandler.FileName;
                    for(u8Counter=0;u8Counter<8;u8Counter++)
                        //*pu8Pointer++=u8FileName[u8Counter];
                        *pu8Pointer++=u8NewName[u8Counter];
                    
                    pu8Pointer=WHandler.Extension;                    
                    for(u8Counter=8;u8Counter<11;u8Counter++)
                        *pu8Pointer++=u8NewName[u8Counter];
                    
                    WHandler.Dir_Entry=(u16Block*EntriesPerBlock)+((u16Index)/RootEntrySize);
                    WHandler.File_Size=LWordSwap(sFileStructure->SizeofFile);
                    WHandler.BaseFatEntry=ByteSwap(sFileStructure->ClusterNumber);
                    
                    if(WHandler.BaseFatEntry != 0)
                    {
                        u16Temporal=WHandler.BaseFatEntry;
                        do
                        {
                            WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                            WHandler.BaseFatEntry=FAT_Entry(WHandler.CurrentFatEntry,0,NEXT_ENTRY);
                        }while(WHandler.BaseFatEntry!=0xFFFF);
                        WHandler.BaseFatEntry=u16Temporal;
                    } 
                    else
                    {
                        WHandler.BaseFatEntry=FAT_SearchAvailableFAT(0);
                        WHandler.CurrentFatEntry=WHandler.BaseFatEntry;
                    }
                     /*****************/
                     /* Double Check */
                     /*****************/
                    
                    u8Counter=0;
                    u8ErrorCode=u16FAT_Cluster_Size;
                    while(u8ErrorCode != 0x01)
                    {
                        u8Counter++;
                        u8ErrorCode=u8ErrorCode>>1;
                    }
                    
                    u16Temporal=(UINT16)WHandler.File_Size % (u16FAT_Sector_Size<<u8Counter);
                    WHandler.ClusterIndex= u16Temporal/u16FAT_Sector_Size;
                    WHandler.SectorIndex= u16Temporal%u16FAT_Sector_Size;
                    u8ErrorCode=FILE_FOUND;

                }
            }

            sFileStructure++; //Desplaza el puntero a la siguiente entrada en el Root Directory
            u16Index+=RootEntrySize;  //Desplaza el indice utilizado para recorrer el Root Directory
        }
        u16Block++;
    }
    if(u16BlockNum==u16Block)
        u8ErrorCode=NO_FILE_ENTRY_AVAILABLE;
    
    return(u8ErrorCode);

}


#pragma warn_implicitconv on
