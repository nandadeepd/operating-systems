code Main

  -- OS Class: Project 2, 3
  --
  -- Nandadeep Davuluru
  -- davuluru@pdx.edu
  -- This package contains the following:
  --     SimpleThreadExample
  --     MoreThreadExamples
  --     ProducerConsumer
  --     TestMutex
  --     Dining Philospohers
  --	 Sleeping Barber
  --	 Gaming Parlour



-----------------------------  SynchTest  ---------------------------------

  function main ()
      print ("Example Thread-based Programs...\n")

      InitializeScheduler ()

      -----  Uncomment any one of the following to perform the desired test  -----

      -- SimpleThreadExample ()
      -- MoreThreadExamples ()
      -- TestMutex ()
      -- ProducerConsumer ()
      -- DiningPhilosophers ()
      SleepingBarber()
      -- GamingParlour()

      ThreadFinish ()

    endFunction



-----------------------------  SimpleThreadExample  ---------------------------------

  var aThread: Thread     -- Dont put Thread objects on the stack, since the
                          -- routine that contains them may return!

  function SimpleThreadExample ()
    -- This code illustrates the basics of thread usage.
    --
    -- This code uses 2 threads.  Each thread loops a few times.
    -- Each loop iteration prints a message and executes a "Yield".
    -- This code illustrates the following operations:
    --     Thread creation
    --     Fork
    --     Yield
    --     Thread termination
    -- This code creates only one new thread; the currrent ("main") thread, which
    -- already exists, is the other thread.  Both the main thread and the newly
    -- created thread will call function "SimpleThreadFunction" to perform the looping.
    --
    -- Notice that timer interrupts will also cause "Yields" to be inserted at
    -- unpredictable places.  Thus, the threads will not simply alternate.
    --
    -- Things to experiment with:
    --    In TimerInterruptHandler (in Thread.c), uncomment "print ('_')".
    --    In TimerInterruptHandler (in Thread.c), comment out the call to
    --            Yield, which will suspend timeslicing.
    --    Edit .blitzrc (see "sim" command) and change TIME_SLICE value.
    --    In this function, comment out the call to "Yield".
    --    

      print ("Simple Thread Example...\n")
      aThread = new Thread
      aThread.Init ("Second-Thread")   -- Initialize, giving thread a name
      aThread.Fork (SimpleThreadFunction, 4)  -- Pass "4" as argument to the thread
      SimpleThreadFunction (7)                -- The main thread will loop 7 times
    endFunction

  function SimpleThreadFunction (cnt: int)
    -- This function will loop "cnt" times.  Each iteration will print a
    -- message and execute a "Yield", which will give the other thread a
    -- chance to run.
      var i: int
      for i = 1 to cnt
        print (currentThread.name)
        nl ()
        currentThread.Yield ()
      endFor
      ThreadFinish ()
    endFunction



-----------------------------  MoreThreadExamples  ---------------------------------

  var th1, th2, th3, th4, th5, th6: Thread

  function MoreThreadExamples ()
      var j: int
          oldStatus: int

      print ("Thread Example...\n")

      -- Create some thread objects (not on the heap).

      th1 = new Thread
      th2 = new Thread
      th3 = new Thread
      th4 = new Thread
      th5 = new Thread
      th6 = new Thread

      -- Initialize them.
      th1.Init ("thread-a")
      th2.Init ("thread-b")
      th3.Init ("thread-c")
      th4.Init ("thread-d")
      th5.Init ("thread-e")
      th6.Init ("thread-f")

      -- Start all threads running.  Each thread will execute the "foo"
      -- function, but each will be passed a different argument. 
      th1.Fork (foo, 1)
      th2.Fork (foo, 2)
      th3.Fork (foo, 3)
      th4.Fork (foo, 4)
      th5.Fork (foo, 5)
      th6.Fork (foo, 6)

      -- Print this thread''s name.  Note that we temporarily disable
      -- interrupts so that all printing will happen together.  Without
      -- this, the other threads might print in the middle, causing a mess.
      oldStatus = SetInterruptsTo (DISABLED)
        print ("\nThe currently running thread is ")
        print (currentThread.name)
        print ("\n")
        PrintReadyList ()
      oldStatus = SetInterruptsTo (oldStatus)

      for j = 1 to 10
        currentThread.Yield ()
        print ("\n..Main..\n")
      endFor

      -- Print the readyList at this point...
      PrintReadyList ()
      currentThread.Print()

/*
      -- Put this thread to sleep...
      oldStatus = SetInterruptsTo (DISABLED)
      print ("About to Sleep main thread...\n")
      currentThread.Sleep ()
      FatalError ("BACK FROM SLEEP !?!?!")
      -- Execution will never reach this point, since the current thread
      -- was not placed on any list of waiting threads.  Nothing in this
      -- code could ever move this thread back to the ready list.
*/

      ThreadFinish ()

    endFunction


  function foo (i: int)
      var j: int
 
      for j = 1 to 30
        printInt (i)

        if j == 20
          -- Next is an example of aborting all threads and shutting down...
          --   FatalError ("Whoops...(SAMPLE ERROR MESSAGE)")

          -- Next is an example of just quietly shutting down...
          --   RuntimeExit ()

          -- Next is an example of what happens if execution errors occur...
          --   i = j / 0         -- Generate an error
        endIf

        -- Call Yield so other threads can run.  This is not necessary,
        -- but it will cause more interleaving of the various threads,
        -- making this program''s output more interesting.
        currentThread.Yield ()
      endFor
    endFunction



-----------------------------  Test Mutex  ---------------------------------

  -- This code illustrates the ideas behind "critical sections" and "mutual
  -- exclusion".  This code creates several threads.  Each thread accesses
  -- some shared data (an integer) in a critical section.  A single lock
  -- is used to control access to the shared variable.  Each thread locks
  -- the mutex, computes a while, increments the integer, prints the new value,
  -- updates the shared copy, and unlocks the mutex.  Then it does some
  -- non-critical computation and repeats.

  var
    myLock: Mutex = new Mutex      -- Could also use "Mutex2" instead
    sharedInt: int = 0
    thArr: array [7] of Thread = new array of Thread {7 of new Thread }

  function TestMutex ()
      myLock.Init ()

      print ("\n-- You should see 70 lines, each consecutively numbered. --\n\n")

      thArr[0].Init ("LockTester-A")
      thArr[0].Fork (LockTester, 100)

      thArr[1].Init ("LockTester-B")
      thArr[1].Fork (LockTester, 200)

      thArr[2].Init ("LockTester-C")
      thArr[2].Fork (LockTester, 1)

      thArr[3].Init ("LockTester-D")
      thArr[3].Fork (LockTester, 50)

      thArr[4].Init ("LockTester-E")
      thArr[4].Fork (LockTester, 300)

      thArr[5].Init ("LockTester-F")
      thArr[5].Fork (LockTester, 1)

      thArr[6].Init ("LockTester-G")
      thArr[6].Fork (LockTester, 1)


      ThreadFinish ()
    endFunction

  function LockTester (waitTime: int)
    -- This function will do the following actions, several times in a loop:
    --     Lock the mutex
    --     Get the current value of the "sharedInt" variable
    --     Compute a new value by adding 1
    --     Wait a while (determined by parameter "waitTime") to simulate
    --        actions done within the critical section
    --     Print the thread''s name and the new value
    --     Update the "sharedInt" variable
    --     Unlock the mutex
    --     Wait a while (determined by parameter "waitTime") to simulate
    --        actions done outside of the critical section
      var
        i, j, k: int
      for i = 1 to 10

        -- Enter
        myLock.Lock()
        -- Critical Section
        j = sharedInt + 1                    -- read shared data
        for k = 1 to waitTime                -- do some computation
        endFor                               --
        printIntVar (currentThread.name, j)  -- print new data value
        sharedInt = j                        -- update shared data

        -- Leave
        myLock.Unlock()

        -- Perform non-critical work
        for k = 1 to waitTime
        endFor

      endFor
    endFunction



-----------------------------  ProducerConsumer  ---------------------------------

  -- This code implements the consumer-producer task.  There are several
  -- "producers", several "consumers", and a single shared buffer.
  --
  -- The producers are named "A", "B", "C", etc.  Each producer is a thread which
  -- will loop 5 times.  For each iteration, the producer thread will add its
  -- character to a shared buffer.  For example, "Producer-B" will add 5 "B"s to
  -- the shared buffer.  Since the 5 producer threads will run concurrently, the
  -- characters will be added in an unpredictable order.  Regardless of the order,
  -- however, there will be five "A"s, five "B"s, five "C"s, etc.
  --
  -- There are several consumers.  Each consumer is a thread which executes an
  -- inifinite loop.  During each iteration of its loop, a consumer will remove
  -- whatever character is next in the buffer and will print it.
  --
  -- The shared buffer is a FIFO queue of characters.  The producers put characters
  -- in one end and the consumers take characters out the other end.  Think of a
  -- section of steel pipe.  The capacity of the buffer is limited to BUFFER_SIZE
  -- characters.
  --
  -- This code illustrates the mechanisms required to synchronize the producers,
  -- consumers, and the shared buffer.  Consumers must wait if the buffer is empty.
  -- Producers must wait if the buffer is full.  Furthermore, the buffer is a shared
  -- data structure.  (The buffer is implemented as an array with pointers to the
  -- next position to add or remove characters.)  No two threads are allowed to
  -- access these pointers simultaneously, or else errors may result.
  --
  -- To document what is happening, each producer will print a line when it adds
  -- a character to the buffer.  The line printed will include the buffer contents
  -- along with the name of the poducer.  Also, each time a consumer removes a
  -- character from the buffer, it will print a line, showing the buffer contents
  -- after the removal, along with the name of the consumer thread.  Each line of
  -- output is formated so that you can see the buffer growing and shrinking.  By
  -- reading the output vertically, you can also see what each thread does.
  --
  const
    BUFFER_SIZE = 5

  var
    buffer: array [BUFFER_SIZE] of char = new array of char {BUFFER_SIZE of '?'}
    bufferSize: int = 0
    bufferNextIn: int = 0
    bufferNextOut: int = 0
    thArray: array [8] of Thread = new array of Thread { 8 of new Thread }
    semEmpty: Semaphore = new Semaphore
    semFull: Semaphore = new Semaphore
    pLock: Mutex = new Mutex
    cLock: Mutex = new Mutex
    printMutex: Mutex = new Mutex

  function ProducerConsumer ()

      pLock.Init()                  -- Lock for exclusive access to all producer threads
      cLock.Init()                  -- Lock for exclusive access to all consumer threads
      printMutex.Init()             -- Lock for excusive print access to producer/consumer
      semFull.Init(0)               -- Initializing semFull to 0
      semEmpty.Init(BUFFER_SIZE)    -- 
      print ("     ")

      thArray[0].Init ("Consumer-1                               |      ")
      thArray[0].Fork (Consumer, 1)

      thArray[1].Init ("Consumer-2                               |          ")
      thArray[1].Fork (Consumer, 2)

      thArray[2].Init ("Consumer-3                               |              ")
      thArray[2].Fork (Consumer, 3)

      thArray[3].Init ("Producer-A         ")
      thArray[3].Fork (Producer, 1)

      thArray[4].Init ("Producer-B             ")
      thArray[4].Fork (Producer, 2)

      thArray[5].Init ("Producer-C                 ")
      thArray[5].Fork (Producer, 3)

      thArray[6].Init ("Producer-D                     ")
      thArray[6].Fork (Producer, 4)

      thArray[7].Init ("Producer-E                         ")
      thArray[7].Fork (Producer, 5)

      ThreadFinish ()
    endFunction

  function Producer (myId: int)
      var
        i: int
        c: char = intToChar ('A' + myId - 1)
      for i = 1 to 5
        -- Perform synchroniztion...
        semEmpty.Down()
        pLock.Lock()
        printMutex.Lock()
        -- Add c to the buffer
        buffer [bufferNextIn] = c
        bufferNextIn = (bufferNextIn + 1) % BUFFER_SIZE
        bufferSize = bufferSize + 1
        -- Print a line showing the state
        
        PrintBuffer (c)
        printMutex.Unlock()
        -- Perform synchronization...
        semFull.Up()
        pLock.Unlock()     
      endFor
    endFunction

  function Consumer (myId: int)
      var
        c: char
      while true
        -- Perform synchroniztion...
        semFull.Down()
        cLock.Lock()
        printMutex.Lock()                                   -- locking print so that one thread can access and print buffer state at a time
        -- Remove next character from the buffer
        c = buffer [bufferNextOut]
        bufferNextOut = (bufferNextOut + 1) % BUFFER_SIZE
        bufferSize = bufferSize - 1

        -- Print a line showing the state

        PrintBuffer (c)
        printMutex.Unlock()

        -- Perform synchronization...
        
        semEmpty.Up()
        cLock.Unlock()
        
        
      endWhile
    endFunction

  function PrintBuffer (c: char)
    --
    -- This method prints the buffer and what we are doing to it.  Each
    -- line should have
    --        <buffer>  <threadname> <character involved>
    -- We want to print the buffer as it was *before* the operation;
    -- however, this method is called *after* the buffer has been modified.
    -- To achieve the right order, we print the operation first, skip to
    -- the next line, and then print the buffer.  Assuming we start by
    -- printing an empty buffer first, and we are willing to end the output
    -- in the middle of a line, this prints things in the desired order.
    --
      var
        i, j: int
      -- Print the thread name, which tells what we are doing.
      print ("   ")
      print (currentThread.name)  -- Will include right number of spaces after name
      printChar (c)
      nl ()
      -- Print the contents of the buffer.
      j = bufferNextOut
      for i = 1 to bufferSize
        printChar (buffer[j])
        j = (j + 1) % BUFFER_SIZE
      endFor
      -- Pad out with blanks to make things line up.
      for i = 1 to BUFFER_SIZE-bufferSize
        printChar (' ')
      endFor
    endFunction



-----------------------------  Dining Philosophers  ---------------------------------

  -- This code is an implementation of the Dining Philosophers problem.  Each
  -- philosopher is simulated with a thread.  Each philosopher thinks for a while
  -- and then wants to eat.  Before eating, he must pick up both his forks.
  -- After eating, he puts down his forks.  Each fork is shared between
  -- two philosophers and there are 5 philosophers and 5 forks arranged in a
  -- circle.
  --
  -- Since the forks are shared, access to them is controlled by a monitor
  -- called "ForkMonitor".  The monitor is an object with two "entry" methods:
  --     PickupForks (phil)
  --     PutDownForks (phil)
  -- The philsophers are numbered 0 to 4 and each of these methods is passed an integer
  -- indicating which philospher wants to pickup (or put down) the forks.
  -- The call to "PickUpForks" will wait until both of his forks are
  -- available.  The call to "PutDownForks" will never wait and may also
  -- wake up threads (i.e., philosophers) who are waiting.
  --
  -- Each philospher is in exactly one state: HUNGRY, EATING, or THINKING.  Each time
  -- a philosopher''s state changes, a line of output is printed.  The output is organized
  -- so that each philosopher has column of output with the following code letters:
  --           E    --  eating
  --           .    --  thinking
  --         blank  --  hungry (i.e., waiting for forks)
  -- By reading down a column, you can see the history of a philosopher.
  --
  -- The forks are not modeled explicitly.  A fork is only picked up
  -- by a philospher if he can pick up both forks at the same time and begin
  -- eating.  To know whether a fork is available, it is sufficient to simply
  -- look at the status''s of the two adjacent philosophers.  (Another way to state
  -- the problem is to forget about the forks altogether and stipulate that a
  -- philosopher may only eat when his two neighbors are not eating.)

  enum HUNGRY, EATING, THINKING
  var
    mon: ForkMonitor
    philospher: array [5] of Thread = new array of Thread {5 of new Thread }

  function DiningPhilosophers ()

      print ("Plato\n")
      print ("    Sartre\n")
      print ("        Kant\n")
      print ("            Nietzsche\n")
      print ("                Aristotle\n")

      mon = new ForkMonitor
      mon.Init ()
      mon.PrintAllStatus ()

      philospher[0].Init ("Plato")
      philospher[0].Fork (PhilosphizeAndEat, 0)

      philospher[1].Init ("Sartre")
      philospher[1].Fork (PhilosphizeAndEat, 1)

      philospher[2].Init ("Kant")
      philospher[2].Fork (PhilosphizeAndEat, 2)

      philospher[3].Init ("Nietzsche")
      philospher[3].Fork (PhilosphizeAndEat, 3)

      philospher[4].Init ("Aristotle")
      philospher[4].Fork (PhilosphizeAndEat, 4)

     endFunction

  function PhilosphizeAndEat (p: int)
    -- The parameter "p" identifies which philosopher this is.
    -- In a loop, he will think, acquire his forks, eat, and
    -- put down his forks.
      var
        i: int
      for i = 1 to 7
        -- Now he is thinking
        mon. PickupForks (p)
        -- Now he is eating
        mon. PutDownForks (p)
      endFor
    endFunction

  class ForkMonitor
    superclass Object
    fields
      status: array [5] of int             -- For each philosopher: HUNGRY, EATING, or THINKING
      monMutex: Mutex                      -- Mutex lock for the monitor
      conditionVar: array [5] of Condition -- condition variables for each philosopher
    methods
      Init ()
      PickupForks (p: int)
      PutDownForks (p: int)
      testStatus (i: int)
      PrintAllStatus ()
  endClass

  behavior ForkMonitor

    method Init ()
      
      -- Initialize so that all philosophers are THINKING.
      var i: int
      status = new array of int {5 of THINKING}
      mon.PrintAllStatus()
      -- Initialize the condition variables for each philospher
      conditionVar = new array [5] of Condition {5 of new Condition}
      for i = 0 to 4
        conditionVar[i].Init ()
      endFor

      -- Initialize mutex for the monitor
      monMutex = new Mutex
      monMutex.Init ()

      endMethod

    method PickupForks (p: int)
      -- This method is called when philosopher 'p' is wants to eat.
      -- lock the monitor mutex first.
      monMutex.Lock()
      -- change status[p] = HUNGRY, print status
      status[p] = HUNGRY

      mon.PrintAllStatus()
      mon.testStatus(p)


      if(status[p] != EATING) 
        conditionVar[p].Wait(&monMutex)
      endIf
      monMutex.Unlock()
      endMethod

    method PutDownForks (p: int)
      -- Lock the monitor mutex
      monMutex.Lock()
      -- This method is called when the philosopher 'p' is done eating.
      -- change status[p] = THINKING, print status
      status[p] = THINKING
      mon.PrintAllStatus()
      mon.testStatus((p-1) % 5)
      mon.testStatus((p+1) % 5)
      monMutex.Unlock()
      endMethod

    method testStatus(i: int)
      
      -- This method is written to check if the neighbors have put down/have a hold of the forks to eat
      -- If the neighbors are using the fork then current philosopher cannot proceed
      -- The mod 5 is used to avoid array index out of bounds exception when trying to access to the previous or next neighbor
      

      if status[(i+1) % 5] != EATING && status[(i-1) % 5] != EATING && status[i] == HUNGRY
        status[i] = EATING
        mon.PrintAllStatus ()
        conditionVar[i].Signal(&monMutex)
      endIf
    endMethod

    method PrintAllStatus ()
      -- Print a single line showing the status of all philosophers.
      --      '.' means thinking
      --      ' ' means hungry
      --      'E' means eating
      -- Note that this method is internal to the monitor.  Thus, when
      -- it is called, the monitor lock will already have been acquired
      -- by the thread.  Therefore, this method can never be re-entered,
      -- since only one thread at a time may execute within the monitor.
      -- Consequently, printing is safe.  This method calls the "print"
      -- routine several times to print a single line, but these will all
      -- happen without interuption.
        var
          p: int
        for p = 0 to 4
          switch status [p]
            case HUNGRY:
              print ("    ")
              break
            case EATING:
              print ("E   ")
              break
            case THINKING:
              print (".   ")
              break
          endSwitch
        endFor
        nl ()
      endMethod

  endBehavior

------------------------------ Sleeping Barbers Problem -----------------------------------

  var 
  	sb: SleepingBarberClass
  	myThreads: array [21] of Thread = new array of Thread {21 of new Thread}

  function SleepingBarber()

  	-- Main Barber shop simulation starts here

  	-- print("Chairs\t\tB\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\t16\t17\t18\t19\t20\n")

  	-- Init and Fork barber, customer threads here
  	sb = new SleepingBarberClass
  	sb.Init()
  	sb.spawn()
  endFunction

  function Barber()
  	-- wrapper function for barber
  	sb.Barber()
  endFunction

  function Customer(wait: int)
  	-- wrapper function for customers
  	sb.Customer(wait)
  endFunction

  behavior SleepingBarberClass

  	method Init()
  		-- Setup the barber shop with 5 chairs and none waiting initially
  		chairs = 5
  		-- THE global variable of number of people waiting
  		waiting = 0
  		-- Initialize the synchronization primitives
  		customers = new Semaphore
  		barbers = new Semaphore
  		mutexLock = new Mutex
  		mutexLock.Init()
  		customers.Init(0)
  		barbers.Init(0)

  	endMethod


  	method Barber()

  		while true 
  			customers.Down()
  			mutexLock.Lock()
  			waiting = waiting - 1
  			barbers.Up()
  			mutexLock.Unlock()
  			sb.waitForCut() -- need to change function name
  		endWhile
  	endMethod


  	method Customer(wait: int)

  		-- adding wait time and yielding to other threads here also to prevent all customers from storming the place. 
    	var i: int = 0
  		for i = 0 to 300
  			currentThread.Yield()
  		endFor

  		mutexLock.Lock()

    	print("space = ")
    	printInt(chairs - waiting)
    	print("  ")

  		if waiting < chairs
  			waiting = waiting + 1
  			print(currentThread.name)
  			print(" is Seated")
  			nl()
  			customers.Up()
  			mutexLock.Unlock()
  			barbers.Down()
  			-- he has to wake up the barber and get his haircut
  			sb.cutHair(wait) -- need to change function name
  		else
  			print(currentThread.name)
  			print(" is Leaving")
  			nl()
  			mutexLock.Unlock() -- give up and go. 
  		endIf

  	endMethod

  	method spawn()
  		var i: int
  		-- method to spawn threads for stress testing the barber shop
  		myThreads[1].Init("Customer-1")
  		myThreads[2].Init("Customer-2")
		myThreads[3].Init("Customer-3")
		myThreads[4].Init("Customer-4")
		myThreads[5].Init("Customer-5")
		myThreads[6].Init("Customer-6")
		myThreads[7].Init("Customer-7")
		myThreads[8].Init("Customer-8")
		myThreads[9].Init("Customer-9")
		myThreads[10].Init("Customer-10")
		myThreads[11].Init("Customer-11")
		myThreads[12].Init("Customer-12")
		myThreads[13].Init("Customer-13")
		myThreads[14].Init("Customer-14")
		myThreads[15].Init("Customer-15")
		myThreads[16].Init("Customer-16")
		myThreads[17].Init("Customer-17")
		myThreads[18].Init("Customer-18")
		myThreads[19].Init("Customer-19")
		myThreads[20].Init("Customer-20")

  		for i = 1 to 4
  			myThreads[i].Fork(Customer, i*50)
  		endFor

		-- barber enters after some customers have entered and wait in chairs. 
		myThreads[0].Init("Barber")
		myThreads[0].Fork(Barber,1)

		-- continue adding customers to the shop
		for i = 5 to 20
			myThreads[i].Fork(Customer,i*50)
		endFor
		
		-- ThreadFinish routine called to kill them
		ThreadFinish()
  	endMethod


  	method waitForCut()

  		-- haircut basically lasts for a while
  		var i: int = 0
  		print(currentThread.name)
  		print(" cutting hair\n")
  		for i = 0 to 800
  			currentThread.Yield()
  		endFor
  		print("haircut done.\n")
  	endMethod

  	method cutHair(wait: int)
  		var i: int = 0
  		for i = 0 to 1000
  			currentThread.Yield()
  		endFor
  		print(currentThread.name)
  		print(" got his haircut.\n")
  	endMethod




  endBehavior


  ------------------------------------ Gaming Parlour ------------------------------------------

  var 
  	frontDesk: FrontDeskMonitor
  	gamingGroup: array [8] of Thread = new array of Thread {8 of new Thread }

  function GamingParlour()

	frontDesk = new FrontDeskMonitor
	frontDesk.Init()

	-- Each gaming group takes in a parameter which is the number of dice it needs to play!

	gamingGroup[0].Init ("Backgammon - A")
    gamingGroup[0].Fork (Play, 4)

    gamingGroup[1].Init ("Backgammon - B")
    gamingGroup[1].Fork (Play, 4)

    gamingGroup[2].Init ("Risk - C")
    gamingGroup[2].Fork (Play, 5)

    gamingGroup[3].Init ("Risk - D")
    gamingGroup[3].Fork (Play, 5)

    gamingGroup[4].Init ("Monopoly - E")
    gamingGroup[4].Fork (Play, 2)

    gamingGroup[5].Init ("Monopoly - F")
    gamingGroup[5].Fork (Play, 2)

    gamingGroup[6].Init ("Pictionary - G")
    gamingGroup[6].Fork (Play, 1)

    gamingGroup[7].Init ("Pictionary - H")
    gamingGroup[7].Fork (Play, 1)



  endFunction

 
  function Play(dice: int)  
  	 -- Wrapper function that interacts with thread init and makes the thread(groups) Request and return the dice
  	var i: int 
  	for i = 0 to 4
  		-- requests for dice
  		frontDesk.Request(dice)

  		-- plays
  		currentThread.Yield()

  		-- returns the dice
  		frontDesk.Return(dice)
  	endFor
  endFunction


  behavior FrontDeskMonitor
  	-- all methods of the monitor
  	method Init()

  		-- Initialize the resources and groups at desk
  		numberOfDiceAvailable = 8
  		numberOfWaitingGroups = 0
  		-- Initialize mutex and condition variables 
  		deskMutex = new Mutex
  		deskMutex.Init()
  		deskCondition = new Condition
    	deskCondition.Init ()
    	groupCondition = new Condition
    	groupCondition.Init()


  	endMethod

  	method PrintStatus(str: String, count: int)

		-- This method prints the current thread''s name and the arguments. -- It also prints the current number of dice available.
		
  		print(currentThread.name)
  		print(" ")
  		print(str)
  		print(" ")
  		printInt(count)
  		nl()
		print ("------------------------------ Number of dice now avail = ")
		printInt(numberOfDiceAvailable)
		nl ()
		
  	endMethod

  	method Request(numberOfDice: int) 

  		deskMutex.Lock()
  		self.PrintStatus("requests", numberOfDice)

  		-- group arrived at the desk, standing in queue for their turn to request dice
  		numberOfWaitingGroups = numberOfWaitingGroups + 1

  		-- check the desk queue for number of groups. If @ head go to next block of code, else wait till head
  		if numberOfWaitingGroups > 1
  			deskCondition.Wait(&deskMutex) -- wait till group reaches head
  		endIf

  		-- check availability of dice
  		while numberOfDiceAvailable < numberOfDice
  			groupCondition.Wait(&deskMutex) -- wait till sufficient resource is available, starve till then..
  		endWhile

  		-- give them the dice.
  		numberOfDiceAvailable = numberOfDiceAvailable - numberOfDice

  		-- group serviced, take em out. 
  		numberOfWaitingGroups = numberOfWaitingGroups - 1

  		-- Signal next group waiting at the desk. Can we do this since groups with different requests are waiting?
  		deskCondition.Signal(&deskMutex)

  		self.PrintStatus("proceeds with ", numberOfDice)
  		deskMutex.Unlock()
  		

  	endMethod

  	method Return(numberOfDice: int)

  		-- a group will come and return the dice after it played. 
  		deskMutex.Lock()
  		
  		numberOfDiceAvailable = numberOfDiceAvailable + numberOfDice
  		self.PrintStatus("releases and adds back", numberOfDice)

  		-- wake up waiting threads
  		groupCondition.Signal(&deskMutex)

  		deskMutex.Unlock()

  	endMethod

  endBehavior


endCode
