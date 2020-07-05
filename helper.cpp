#include <ctype.h>
#include <cerrno>
#include <cstdio>
#include "helper.hpp"
#include <fstream>
#include "regex"
#include "iostream"
#include "enums.hpp"
#ifdef gpp9
// gcc Version >= 9
#include "filesystem"
namespace filesystem = std::filesystem;
#else
// gcc Version < 9
#include "experimental/filesystem"
namespace filesystem = std::experimental::filesystem;
#endif // DEBUG


/* ----------------------------------------------------------------------- */
/*                         local auxiliary functions                       */
/* ----------------------------------------------------------------------- */

clock_t last_timer_reset;

int min_int( const int n1, const int n2 )
{
    if( n1 < n2 ) return n1;
    return n2;
}



/* ----------------------------------------------------------------------- */
/*                             read datafile                               */
/* ----------------------------------------------------------------------- */

void errhandler( int nLine, const char *szFile, const char *szString )
{
    int err = errno;

    fprintf( ERROUT, "%s:%d Error : %s", szFile, nLine, szString );
    fprintf( ERROUT, "\n" );
    
    /* if an error within the c-library occured, an error code can be   */
    /* found in the global variable err                                 */
    if( err != 0 )
    {
	fprintf( ERROUT, "C-Lib   errno    = %d\n", err);
	fprintf( ERROUT, "C-Lib   strerror = %s\n", strerror( err ) );
    }
    exit(1);
}


/*  for comfort */
#define READ_ERROR(szMessage, szVarName, szFileName, nLine) \
  { char szTmp[80]; \
    if( nLine ) \
	sprintf( szTmp, " %s  File: %s   Variable: %s  Line: %d", szMessage, szFileName, szVarName, nLine ); \
    else \
	sprintf( szTmp, " %s  File: %s   Variable: %s ", szMessage, szFileName, szVarName); \
    ERROR( szTmp ); \
  }
    

/* --------------------------------------------------------------------------*/
/* The function searches the datafile fh for the line defining the variable  */
/* szVarName and returns the respctive string including the value of the     */
/* variable. If there's no appropriate line within the datafile, the program */
/* stops with an error messsage.                                             */
/* ATTENTION: The pointer returned refers to a static variable within the    */
/* function. To maintain the string over several program calls, it has to be */
/* copied!!!                                                                 */
/*                                                                           */
char* find_string( const char* szFileName, const char *szVarName )
{ 
    int nLine = 0;
    int i;
    FILE *fh = NULL;
    
    static char szBuffer[MAX_LINE_LENGTH];	/* containes the line read  */
                                               /* from the datafile        */

    char* szLine = szBuffer;
    char* szValue = NULL;
    char* szName = NULL;

    /* open file */
    fh = fopen( szFileName, "rt" );
    if( fh == 0 ) 
	READ_ERROR("Could not open file", szVarName, szFileName, 0);

    /* searching */
    while( ! feof(fh) )
    {
	fgets( szLine, MAX_LINE_LENGTH, fh );
	++nLine;

	/* remove comments */
	for( i = 0; i < strlen(szLine); i++)
	    if( szLine[i] == '#' )
	    {
		szLine[i] = '\0'; /* Stringende setzen */
		break;
	    }

	/* remove empty lines */
	while( isspace( (int)*szLine ) && *szLine) ++szLine;
	if( strlen( szLine ) == 0) continue; 

	/* now, the name can be extracted */
	szName = szLine;
	szValue = szLine;
	while( (isalnum( (int)*szValue ) || *szValue == '_') && *szValue) ++szValue;
	
	/* is the value for the respective name missing? */
	if( *szValue == '\n' || strlen( szValue) == 0)  
	    READ_ERROR("wrong format", szName, szFileName, nLine);
	
	*szValue = 0;		/* complete szName! at the right place */
	++szValue;
        
	/* read next line if the correct name wasn't found */
	if( strcmp( szVarName, szName)) continue;

	/* remove all leading blnkets and tabs from the value string  */
	while( isspace( (int)*szValue) ) ++szValue;
	if( *szValue == '\n' || strlen( szValue) == 0)  
	    READ_ERROR("wrong format", szName, szFileName, nLine);
	
	fclose(fh);
	return szValue;
    }  
   
    READ_ERROR("variable not found", szVarName, szFileName, nLine);
    
    return NULL;		/* dummy to satisfy the compiler  */
} 

void read_string( const char* szFileName, const char* szVarName, char*   pVariable)
{
    char* szValue = NULL;	/* string containg the read variable value */

    if( szVarName  == 0 )  ERROR("null pointer given as variable name" );
    if( szFileName == 0 )  ERROR("null pointer given as filename" );
    if( pVariable  == 0 )  ERROR("null pointer given as variable" );

    if( szVarName[0] == '*' )
	szValue = find_string( szFileName, szVarName +1 );
    else
	szValue = find_string( szFileName, szVarName );
    
    if( sscanf( szValue, "%s", pVariable) == 0)
	READ_ERROR("wrong format", szVarName, szFileName,0);

    printf( "File: %s\t\t%s%s= %s\n", szFileName, 
	                              szVarName,
	                              &("               "[min_int( strlen(szVarName), 15)]), 
	                              pVariable );
}

void read_int( const char* szFileName, const char* szVarName, int* pVariable)
{
    char* szValue = NULL;	/* string containing the read variable value */

    if( szVarName  == 0 )  ERROR("null pointer given as varable name" );
    if( szFileName == 0 )  ERROR("null pointer given as filename" );
    if( pVariable  == 0 )  ERROR("null pointer given as variable" );

    if( szVarName[0] == '*' )
	szValue = find_string( szFileName, szVarName +1 );
    else
	szValue = find_string( szFileName, szVarName );
    
    if( sscanf( szValue, "%d", pVariable) == 0)
	READ_ERROR("wrong format", szVarName, szFileName, 0);

    printf( "File: %s\t\t%s%s= %d\n", szFileName, 
	                              szVarName,
	                              &("               "[min_int( strlen(szVarName), 15)]), 
	                              *pVariable );
}

void read_double( const char* szFileName, const char* szVarName, double* pVariable)
{
    char* szValue = NULL;	/* String mit dem eingelesenen Variablenwert */

    if( szVarName  == 0 )  ERROR("null pointer given as varable name" );
    if( szFileName == 0 )  ERROR("null pointer given as filename" );
    if( pVariable  == 0 )  ERROR("null pointer given as variable" );

    if( szVarName[0] == '*' )
	szValue = find_string( szFileName, szVarName +1 );
    else
	szValue = find_string( szFileName, szVarName );
    
    if( sscanf( szValue, "%lf", pVariable) == 0)
	READ_ERROR("wrong format", szVarName, szFileName, 0);

    printf( "File: %s\t\t%s%s= %f\n", szFileName, 
	                              szVarName,
	                              &("               "[min_int( strlen(szVarName), 15)]), 
	                              *pVariable );
}

/* --------------------------------------------------------------------------*/
/*         Definition of helper functions for reading parameter files        */
/* --------------------------------------------------------------------------*/
std::pair<double, int> read_file_regex(std::string filename, std::string sZName, read_type type) {

    // Definition of pair (Alternative is struct)
    std::pair<double, int> res;
    // File
    std::ifstream file(filename);
    // Line Variable is used for search
    std::string line;

    // Regex Variables
    std::smatch match;
    std::regex word("[a-zA-Z_]+");
    std::regex number_int("\\d+");
    std::regex number_float("\\d+\\.\\d{1,3}");

    // Open File and
    if (file.is_open()) {
        // read line by line
        while (std::getline(file, line)) {
            // Find parameter name
            std::regex_search(line, match, word);
            // Check if found string matches required parameter
            if (match.str(0) == sZName) {
                // Check if float is in line
                if (std::regex_search(line, match, number_float))
                    break;
                // Check if int is in line
                if (std::regex_search(line, match, number_int))
                    break;
            }
        }
        file.close();
    }
    // if the paramter value is not found the match variable has a length of zero
    if (match.length() == 0)
        std::cerr << "Parameter " << sZName <<  " not found" << std::endl;
    else {
        if (type == read_type::INT)
            std::get<int>(res) = std::stoi(match.str(0));
        else if (type == read_type::DOUBLE)
            std::get<double>(res) = std::stod(match.str(0));
        else
            std::cerr << "The provided value type is not defined" << std::endl;
    }
    return res;
}

void get_file_double(std::string filename, std::string sZName, double& value) {
    std::pair<double, int> res = read_file_regex(filename, sZName, read_type::DOUBLE);
    value = std::get<double>(res);
}

void get_file_int(std::string filename, std::string sZName, int& value) {
    std::pair<double, int> res = read_file_regex(filename, sZName, read_type::INT);
    value = std::get<int>(res);
}


/* ----------------------------------------------------------------------- */
/*                   write matrices to a file                              */
/* ----------------------------------------------------------------------- */

void write_matrix( const char* szFileName,       /* filename */
		   double **m,		       /* matrix */
		   int nrl,		       /* first column */
		   int nrh,		       /* last column */
		   int ncl,		       /* first row */
		   int nch,		       /* last row */
		 double xlength,	       /* size of the geometry in */
                                               /* x-direction */
		 double ylength,	       /* size of the geometry in */
                                               /* y-direction  */
		   int fFirst ) 	       /* 0 == append, else overwrite*/
{
   int i, j;
   FILE * fh = 0;
   int nSize = (nrh-nrl+1) * (nch-ncl+1);
   float *tmp = (float *)malloc( (size_t)(nSize * sizeof(float)));
   int k = 0;

   if( fFirst )				/* first call of the function ? */
   {
       fh = fopen( szFileName, "w");	/* overwrite file/write new file */
       if( fh == NULL )			/* opening failed ? */
       {
	   char szBuff[80];
	   sprintf( szBuff, "Outputfile %s cannot be created", szFileName );
	   ERROR( szBuff );
       }
       
/*       fprintf( fh,"%f\n%f\n%d\n%d\n%d\n%d\n", xlength, ylength, nrl, nrh, ncl, nch ); */
   }
   else
   {
       fh = fopen( szFileName ,"a");	/* append to the file */
       if( fh == NULL )			/* opening failed ? */
       {
	   char szBuff[80];
	   sprintf( szBuff, "Outputfile %s cannot be opened", szFileName );
	   ERROR( szBuff );
       }
   } 

   for( j = ncl; j <= nch; j++)
       for( i = nrl; i <= nrh; i++)
	   tmp[k++] = (float)m[i][j];

   fwrite( tmp, sizeof(float), nSize, fh);

   if( fclose(fh) )
   {
       char szBuff[80];
       sprintf( szBuff, "Outputfile %s cannot be closed", szFileName );
       ERROR( szBuff );
   };

   free( tmp );
}


void read_matrix( const char* szFileName,       /* filename */
		   double **m,		       /* matrix */
		   int nrl,		       /* first column */
		   int nrh,		       /* last column */
		   int ncl,		       /* first row */
		   int nch		       /* last row */
                  ) 	  
{
   int i, j;
   FILE * fh = 0;
   int nSize = (nrh-nrl+1) * (nch-ncl+1);
   float *tmp = (float *)malloc( (size_t)(nSize * sizeof(float)));
   int k = 0;

       fh = fopen( szFileName, "r");	/* overwrite file/write new file */
       if( fh == NULL )			/* opening failed ? */
       {
	   char szBuff[80];
	   sprintf( szBuff, "Can not read file %s !!!", szFileName );
	   ERROR( szBuff );
       }


   fread( tmp, sizeof(float), nSize, fh);

   for( j = ncl; j <= nch; j++)
       for( i = nrl; i <= nrh; i++)
	   m[i][j]=tmp[k++];

   if( fclose(fh) )
   {
       char szBuff[80];
       /*orig bug:
       sscanf( szBuff, "Inputfile %s cannot be closed", szFileName );*/
       sprintf( szBuff, "Inputfile %s cannot be closed", szFileName );
       ERROR( szBuff );
   };

   free( tmp );
}


/* ----------------------------------------------------------------------- */
/*                      general matrix functions                           */
/* ----------------------------------------------------------------------- */

/*  allocates storage for a matrix                                         */

/*double **matrix( int nrl, int nrh, int ncl, int nch )
{
   int i;
   int nrow = nrh - nrl + 1;	*//* compute number of lines *//*
   int ncol = nch - ncl + 1;	*//* compute number of columns *//*

   double **pArray  = (double **) malloc((size_t)( nrow * sizeof(double*)) );
   double  *pMatrix = (double *)  malloc((size_t)( nrow * ncol * sizeof( double )));

   if( pArray  == 0)  ERROR("Storage cannot be allocated");
   if( pMatrix == 0)  ERROR("Storage cannot be allocated");

   *//* first entry of the array points to the value corrected by the
      beginning of the column *//*
   pArray[0] = pMatrix - ncl;

   *//* compute the remaining array entries *//*
   for( i = 1; i < nrow; i++ )
   {
       pArray[i] = pArray[i-1] + ncol;
   }

   // return the value corrected by the beginning of a line
   return pArray - nrl;
}*/




/* deallocates the storage of a matrix  */
void free_matrix( double **m, int nrl, int nrh, int ncl, int nch )
{
   double **pArray  = m + nrl;
   double  *pMatrix = m[nrl]+ncl;

   free( pMatrix );
   free( pArray );
}

void init_matrix( double **m, int nrl, int nrh, int ncl, int nch, double a)
{
   int i,j;
   for( i = nrl; i <= nrh; i++){
       for( j = ncl; j <= nch; j++){
	   m[i][j] = a;
	   }
   }
}


/* allocates storage for a matrix */
int **imatrix( int nrl, int nrh, int ncl, int nch )
{
   int i;

   int nrow = nrh - nrl + 1;	/* compute number of rows */
   int ncol = nch - ncl + 1;	/* compute number of columns */
   
   int **pArray  = (int **) malloc((size_t)( nrow * sizeof( int* )) );
   int  *pMatrix = (int *)  malloc((size_t)( nrow * ncol * sizeof( int )));


   if( pArray  == 0)  ERROR("Storage cannot be allocated");
   if( pMatrix == 0)  ERROR("Storage cannot be allocated");

   /* first entry of the array points to the value corrected by the 
      beginning of the column */
   pArray[0] = pMatrix - ncl; 

   /* compute the remaining array entries */
   for( i = 1; i < nrow; i++ )
   {
       pArray[i] = pArray[i-1] + ncol;
   }

   /* return the value corrected by the beginning of a line */
   return pArray - nrl;
}

/* deallocates the storage of a matrix  */
void free_imatrix( int **m, int nrl, int nrh, int ncl, int nch )
{
   int **pArray  = m + nrl;
   int  *pMatrix = m[nrl]+ncl;

   free( pMatrix );
   free( pArray );
}

void init_imatrix( int **m, int nrl, int nrh, int ncl, int nch, int a)
{
   int i,j;
   for( i = nrl; i <= nrh; i++)
       for( j = ncl; j <= nch; j++)
	   m[i][j] = a;
}


int **read_pgm(const char *filename)
{
    FILE *input = NULL;
    char line[1024];
    int levels;
    int xsize, ysize;
    int i1, j1;
    int **pic = NULL;
    

    if ((input=fopen(filename,"rb"))==0)
    {
       char szBuff[80];
	   sprintf( szBuff, "Can not read file %s !!!", filename );
	   ERROR( szBuff );
    }

    /* check for the right "magic number" */
    if ( fread(line,1,3,input)!=3 )
    {
	    fclose(input);
	    ERROR("Error Wrong Magic field!");
    }

    /* skip the comments */
    do
    fgets(line,sizeof line,input);
    while(*line=='#');

    /* read the width and height */
    sscanf(line,"%d %d\n",&xsize,&ysize);

    printf("Image size: %d x %d\n", xsize,ysize);

    /* read # of gray levels */
    fgets(line,sizeof line,input);
    sscanf(line,"%d\n",&levels);

    /* allocate memory for image */
    pic = imatrix(0,xsize+1,0,ysize+1);
    printf("Image initialised...\n");

    /* read pixel row by row */
    for(j1=1; j1 < ysize+1; j1++)
    {
	    for (i1=1; i1 < xsize+1; i1++)
	    {
	        int byte;
            fscanf(input, "%d", &byte);

	        if (byte==EOF)
	        {
		        fclose(input);
		        ERROR("read failed");
	        }
	        else
	        {
		        pic[i1][ysize+1-j1] = byte;
		        printf("%d,%d: %d\n", i1,ysize+1-j1,byte);
	        }
	     }
    }
    for (i1 = 0; i1 < xsize+2; i1++)
    {
        pic[i1][0] = 0;
    }
    for (i1 = 0; i1 < xsize+2; i1++)
    {
        pic[i1][ysize+1] = 0;
    }
    for (j1 = 0; j1 < ysize+2; j1++)
    {
        pic[0][j1] = 0;
        pic[xsize+1][j1] = 0;
    }

    /* close file */
    fclose(input);
    
    return pic;
}

/* ----------------------------------------------------------------------- */
/*                      folder creation functions                          */
/* ----------------------------------------------------------------------- */

/* checks if a respective folder exists */

void check_dir_exists(std::string outputfolder) {
    // Check if the provided string contains a top level directory
    // i.e. "results/output" -> top level directory is results
    filesystem::path folder(outputfolder);

    if (filesystem::is_directory(folder)) {
        std::cout << "Folder: "<< folder <<  " already exists." << std::endl;
    }
    // if the directory is provided like "results", the top level directory doesn't need to be used
    else {
        std::cout << "Folder: "<< folder <<  " doesn't exists." << std::endl;
        if (filesystem::create_directory(folder))
            std::cout << "Folder: "<< folder <<  " created successfully." << std::endl;
    }
}



double maxElementOfMatrix(std::vector<std::vector<double>> M){

    double max = 0.0;

    for( const auto &element : M ){

        double temp_max = *std::minmax_element(element.begin(), element.end()).second;
        max = max < temp_max  ? temp_max: max;
    }

    return max;

}

