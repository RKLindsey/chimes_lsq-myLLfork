// Used by all classes to safely stop the calculation.
void stop_run(int stat) {
#ifdef USE_MPI
	MPI_Abort(MPI_COMM_WORLD,stat) ;
#else
	exit(stat) ;
#endif
}

class Vector {
public:
	double *vec ;
	int dim ;
	double shift ;

	Vector(int d1)
		{
			dim = d1 ;
			vec = new double[d1] ;
			shift = 0 ;
		}
	Vector(int d1, double val)
		{
			dim = d1 ;
			vec = new double[d1] ;
			shift = 0 ;
			for ( int j = 0 ; j < dim ; j++ ) {
				vec[j] = val ;
			}
		}
	Vector()
		{
			dim = 0 ;
			vec = NULL ;
			shift = 0 ;
		}
	~Vector()
		{
			
				delete[] vec ;
		}
	int size() const {
		return dim ;
	}
	void set(int i, double val) {
#ifdef DEBUG					
		if ( i >= dim ) {
			cout << "Vector set out of bounds" << endl ;
			exit(1) ;
		}
#endif					
		vec[i] = val ;
	}

	Vector &operator=(const Vector& in) {
		if ( dim !=in.dim ) {
			delete [] vec ;
			vec = new double [in.dim] ;
			dim = in.dim ;
		}
		for ( int j = 0 ; j < dim ; j++ ) {
			vec[j] = in.vec[j] ;
		}
		return *this ;
	}
		
	void realloc(int size) {
		// Reallocate the vector.
		
			delete [] vec ;
		vec = new double[size] ;
		dim = size ;
	}
	
	void read(ifstream &file, int dim0) 
		{
			dim = dim0 ;
			vec = new double[dim] ;
			for ( int i = 0 ; i < dim ; i++ ) {
				double val ;
				file >> val ;
				set(i, val) ;
			}
		}

	void read_sparse(ifstream &file)
		// Read a vector in sparse format.
		{
			string line ;

			clear() ;
			getline(file,line) ;

			size_t pos =  line.find('[') ;
			if ( pos == string::npos ) {
				cout << "Did not find '[' character in " + line << endl ;
				exit(1) ;
			}
			istringstream istr(line.substr(pos+1)) ;
			istr >> dim ;
			vec = new double[dim] ;
			for ( int i = 0 ; i < dim ; i++ ) {
				vec[i] = double{0} ;
			}
			int idx ;
			double val ;
			for ( int i = 0 ; i < dim ; i++ ) {
				getline(file, line) ;
				if ( line.find(']') != string::npos ) {
					break ;
				}
				istringstream istr(line) ;
				istr >> idx >> val ;
				if ( idx < dim && idx >= 0 ) 
					set(idx, val) ;
				else {
					cout << "Error reading sparse vector " << endl ;
					cout << line ;
					exit(1) ;
				}
			}
			// cout <<  "Last line: " + line << endl ;
		}

	void normalize()
		{
			shift = 0 ;
			for ( int i = 0 ; i < dim ; i++ ) {
				shift += vec[i] ;
			}
			shift /= dim;
			for ( int i = 0 ; i < dim ; i++ ) {
				vec[i] -= shift ;
			}
		}
	void check_norm()
		{
			double test = 0.0 ;
			for ( int i = 0 ; i < dim ; i++ ) {
				test += vec[i] ;
			}
			if ( fabs(test) > 1.0e-06 ) 
			{
			
#ifdef WARN_ONLY
				if ( RANK == 0 ) 
				{
					cout << "WARNING: vector was not normalized" << endl ;
					cout << "Test yielded value: " << fabs(test) << endl;
					cout << "Success criteria:   1.0e-6" << endl;
				}
				
#else			
				if ( RANK == 0 ) 
				{
					cout << "Error: vector was not normalized" << endl ;
					cout << "Test yielded value: " << fabs(test) << endl;
					cout << "Success criteria:   1.0e-6" << endl;
				}
				exit(1) ;
#endif				
			}
		}
	double get(int idx) const 
		{
#ifdef DEBUG			
			if ( idx >= dim ) {
				cout <<  "Vector out of bounds" << endl ;
				exit(1) ;
			}
#endif			
			return vec[idx] ;
		}
	void add(int idx, double val) 
		{
			vec[idx] += val ;
		}
	//void print() 
	//{
	//if ( RANK == 0 ) {
	//cout << "[" << endl ;
	//for ( int j = 0 ; j < dim ; j++ ) {
	//if ( fabs(vec[j]) > 0.0 ) 
	//cout << j << " " << vec[j] << endl ;
//}
	//cout << "]" << endl ;
//}
//}
	void print(ostream &of)
	// Print only non-zero values.
		{
			if ( RANK == 0 ) {
				of << "[" << endl ;
				for ( int j = 0 ; j < dim ; j++ ) {
					if ( fabs(vec[j]) > 0.0 ) 
						of << j << " " << vec[j] << endl ;
				}
				of << "]" << endl ;
			}
		}
	
		void print_sparse(ofstream &of)
		// Print only non-zero values.
		{
			if ( RANK == 0 ) {
				of << "[ " << dim << endl ;
				for ( int j = 0 ; j < dim ; j++ ) {
					if ( fabs(vec[j]) > 0.0 ) 
						of << j << " " << vec[j] << endl ;
				}
				of << "]" << endl ;
			}
		}

	void print_all(ostream &of)
	// Print all values.
		{
			if ( RANK == 0 ) {
				for ( int j = 0 ; j < dim ; j++ ) {
					of << j << " " << vec[j] << endl ;
				}
			}
		}
	void scale(Vector &out, double val) 
	// Scale the vector by the given value, put result in Out.
		{
			for ( int j = 0 ; j < dim ; j++ ) {
				out.set(j, val * vec[j] ) ;
			}
		}
	void scale(Vector &out, const Vector &vals) 
	// Multiply the vector by the given array of values, put result in Out.
		{
			for ( int j = 0 ; j < dim ; j++ ) {
				out.set(j, vals.get(j) * vec[j] ) ;
			}
		}
	double l1norm()
	// Returns L1 norm (sum of abs values).
		{
			double norm = 0 ;
			for ( int i = 0 ; i < dim ; i++ ) {
				norm += fabs(vec[i]) ;
			}
			return norm ;
		}
	void remove(int idx)
	// Remove the specified index from the vector.
		{
			if ( idx < 0 || idx >= dim ) {
				cout << "Error: bad index to remove from vector: " << idx << endl ;
			}
			for ( int i = idx ; i < dim - 1 ; i++ ) {
				vec[i] = vec[i+1] ;
			}
			--dim ;
		}

	void push(double val)
		// Add the value to the end of the vector.
	{
		double *newv = new double[dim+1] ;
		for ( int j = 0 ; j < dim ; j++ ) {
			newv[j] = vec[j] ;
		}
		newv[dim] = val ;
		delete [] vec ;
		vec = newv ;
		++dim ;
	}
	void add_mult(const Vector &in, double factor)
		// Set out = out + factor * in
	{
		if ( in.dim != dim ) {
			cout << "Error in add_mult: dim mismatch\n" ;
			exit(1) ;
		}
		for ( int k = 0 ; k < dim ; k++ ) {
			vec[k] += factor * in.get(k) ;
		}
	}
	void clear()
	// Clear all entries of the vector.
	{
		if ( dim > 0 ) 
			delete [] vec ;
		vec = NULL ;
		dim = 0 ;
	}
} ;