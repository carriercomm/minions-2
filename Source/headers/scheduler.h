/***************************************************************
 *           Minions Mud Server is copyright (C)2001           *
 *                    SonzoSoft Software                       *
 *                                                             *
 *         Programmed by Mark Richardson and David Brown       *
 *															   *
 *			 Mark Richardson	-	sinbaud@hotmail.com	       *
 *		       David Brown	    -	dcbrown73@yahoo.com	       *
 ***************************************************************/



class minionsEvent;

class scheduler
{ 
	// Create a multimap type that holds ptr to minionsEvent objects (and derivitives)
	typedef std::multimap<time_t, minionsEvent *> wStack;
	typedef std::vector<minionsEvent *> eStack;

    // Create multimaps for incoming and outgoing events stacks
	wStack waitStack;
	eStack execStack;

	// Create multimap's iterators
	wStack::iterator wsIter;  
	eStack::iterator esIter;  
	


public:

	scheduler();
	~scheduler();
	wStack getWaitStack();   
	eStack getExecStack();
	void pushWaitStack(time_t eTime, minionsEvent *mEvent);
//   void pushExecStack(time_t eTime, minionsEvent *mEvent);
	void doEvents();
	void checkEventsStatus();
	
};