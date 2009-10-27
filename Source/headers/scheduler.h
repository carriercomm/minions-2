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
	typedef std::multimap<time_t, minionsEvent *> eventStack;

    // Create multimaps for incoming and outgoing events stacks
	eventStack waitStack;
	eventStack execStack;

	// Create multimap's iterators
	eventStack::iterator wsIter;  
	eventStack::iterator esIter;  
	


public:

	scheduler();
	~scheduler();
	eventStack getWaitStack();   
	eventStack getExecStack();
	void pushWaitStack(time_t eTime, minionsEvent *mEvent);
    void pushExecStack(time_t eTime, minionsEvent *mEvent);
	
};