// File describing force cut-off functions.
//
#ifndef _FCUT_H
#define _FCUT_H

enum class FCUT_TYPE // Contains allowed force cut-off types.
{
	CUBIC,
	TERSOFF
} ;

class FCUT 
{
public:

	double OFFSET; 


	// power used in the cutoff function.
	int  POWER = 3 ;

	// Type of cutoff-function employed.
	FCUT_TYPE TYPE ;

	// set the type of the cutoff function.
	void set_type(string s) ;

	// Default constructor.
	FCUT() ;

	// Convert a cut-off function to a string.
	string to_string() ;

	// Evaluate the cut-off function.
	void get_fcut(double & fcut, double & fcut_deriv, const double rlen, const double rmin, const double rmax) ;
    
    // Update fcut and fcut deriv based on poly order, if chains are used
    void get_chains(int poly_order, double & fcut, double & fcut_deriv);

	// Decide whether to proceed with a pair interaction.
	bool PROCEED(const double & rlen, const double & rmin, const double & rmax) ;

   	// Parse the input string and set parameters for the force cutoff
	void parse_input(string line) ;

	// Print out force cutoff parameters.
	void print_params() ;

	// Print an entry into the force field header file.
	void print_header(ostream &header) ;
} ;

#endif
