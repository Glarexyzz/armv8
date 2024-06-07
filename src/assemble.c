#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
// #include <unistd.h>

#define MAXLINELEN 1024

// Symbol table:

#define MAXSYMBOLLEN 64
typedef char symbol[MAXSYMBOLLEN];

typedef struct {
	symbol s; //Up to 64 characters
	unsigned v;
} symval;

#define MAXSYMBOLS 1024

symval symtab[MAXSYMBOLS];
int nsymbols = 0;


// init_symtab();
//	Initialise the symbol table.
//
void init_symtab( void )
{
	nsymbols = 0;
}
// typedef for line processor
typedef bool (*line_processor)(char *line, int lineno, FILE *outputFile);

// Prototypes
int get_sym( char *s );
void add_sym( char *s, unsigned v );
void show_symtab( FILE *out );
void error( char *line, int lineno, char *msg );
bool build_symtab(char *line, int lineno, FILE *outputFile);
void parse_file(FILE *, line_processor, FILE *);



//
// int v = get_sym( char *s );
//	Get (s,v) from the symbol table and return v.
//	Returns -1 if s is NOT already present.
//
int get_sym( char *s )
{
	for( int i=0; i<nsymbols; i++ )
	{
		if( strcmp( symtab[i].s, s ) == 0 )
		{
			return symtab[i].v;
		}
	}
	return -1;
}


//
// add_sym( char *s, unsigned v );
//	Add (s,e) to the symbol table.
//	Aborts if s is already present.
//
void add_sym( char *s, unsigned v )
{
	assert( get_sym( s ) == -1 );
	assert( nsymbols < MAXSYMBOLS );
	assert( strlen(s) < MAXSYMBOLLEN );
	strcpy( symtab[nsymbols].s, s );
	symtab[nsymbols].v        = v;
	nsymbols++;
}


// show_symtab( out );
//	Display every (name,value) pair in the symbol table.
//
void show_symtab( FILE *out )
{
	for( int i=0; i<nsymbols; i++ )
	{
    // Print the symbol table (4-digit hex value (2-bytes), symbol name)
		fprintf( out, "%04x  %s\n", symtab[i].v, symtab[i].s );
	}
}

int nerrors = 0;		// number of assembly time errors


// error( line, lineno, msg );
//	Produce a standard-formatted error
//
void error( char *line, int lineno, char *msg )
{
	fprintf( stderr, "Line %d: %s\n\t\t%s\n", lineno, line, msg );
	nerrors++;
}

bool isalpha_str(const char *str) {
    while (*str) {
        if (!isalpha(*str)) {
            return false; // Return 0 (false) if any character is not alphabetic
        }
        str++;
    }
    return true; // Return 1 (true) if all characters are alphabetic
}

// Return true if line is a label (so line should be skipped), false otherwise
// Logs an error if a label is duplicated, label is not only a-zA-Z
bool build_symtab(char *line, int lineno, FILE *_) {
  // Check if line is a label
  if (line[strlen(line)-1] == ':') {
    // Remove colon
    line[strlen(line)-1] = '\0';
    if (get_sym(line) != -1) {
      error(line, lineno, "Error: Duplicate label");
      nerrors++;
      return true;
    }
    if (!isalpha_str(line)){
      error(line, lineno, "Error: Label must only contain a-zA-Z");
      nerrors++;
      return true;
    }
    // Add symbol to symbol table
    add_sym(line, lineno);
    return true;
  }
  return false;
}

//returns an array of strings
char** parseInstruction(char *line) {
  //use strtok to tokenise and then maybe realloc?
  TODO();
}

bool process_line(char *line, int lineno, FILE *outputFile) {
  // Check if line is a label
  if (line[strlen(line)-1] == ':') {
    return true;
  // Check if line is a directive
  } else if (line[0] == '.') {

  // Otherwise line is instruction
  } else {
    char** parsedIns = parseInstruction(line);
    char* opc = parsedIns[0];

    if (opc == "add") {

    } else if (opc == "adds") {

    }
    //etc
  }
  fprintf(outputFile, "%d: %s", lineno, line);
  return false;
}

void parse_file(FILE *inputFile, line_processor processor, FILE *outputFile) {
  // Read input file line by line
  char line[MAXLINELEN];
  for ( int lineno = 0; fgets( line, MAXLINELEN, inputFile ) != NULL; lineno++ )
  {
    // Remove newline character
    int len = strlen(line);
		if( line[len-1] == '\n' )
		{
			line[len-1] = '\0';
		} else {
      fprintf( stderr, "Line %d too long\n", lineno );
      exit( EXIT_FAILURE );
    }
    // Skip empty lines and comments and build_symbol table if label then skip line for counting
    if (line[0] == '#' || line[0] == '\0' || processor(line, lineno, outputFile)) {
      lineno--; // Ignore blank lines
      continue;
    }
  }
}

int main(int argc, char **argv) {
  if (argc != 3){
    fprintf(stderr, "Usage: %s <file_in> <file_out>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  // Define input file
  const char *inputFileName = argv[1];
  FILE *inputFile = fopen(inputFileName, "r");

  if (inputFile == NULL) {
    fprintf(stderr, "Error: Cannot open file %s\n", inputFileName);
    return EXIT_FAILURE;
  }

  const char *outputFileName = argv[2];
  FILE *outputFile = fopen(outputFileName, "wb");

  if (outputFile == NULL) {
    fprintf(stderr, "Error: Cannot open file %s\n", outputFileName);
    return EXIT_FAILURE;
  }

  // Should make outputFile optional
  parse_file(inputFile, build_symtab, outputFile);
  fseek(inputFile, 0, SEEK_SET);
  parse_file(inputFile, process_line, outputFile);
  
  fclose(inputFile);

	if( nerrors == 0 )		// no errors? output the file
	{
		fputs( "\n",  outputFile);
    show_symtab(outputFile);
    return EXIT_SUCCESS;
	} else {
    fprintf( stderr, "%d errors\n", nerrors );
    unlink(outputFileName);
    return EXIT_FAILURE;
  }

  

  return EXIT_SUCCESS;
}
