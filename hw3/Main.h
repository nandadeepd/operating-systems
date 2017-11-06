-- CS 510 Operating Systems 
-- Project 2,3 code
-- Nandadeep Davuluru
-- davuluru@pdx.edu
-- Prototypes for the entire project


header Main

  uses System, Thread, Synch

  functions
    main ()
    
  class FrontDeskMonitor
  	superclass Object
  	fields
  		-- all class fields.
  		deskMutex: Mutex
  		printingMutex: Mutex
  		deskCondition: Condition
  		groupCondition: Condition
  		numberOfDiceAvailable: int
  		numberOfWaitingGroups: int
  	methods
  		-- all monitor methods
  		Init()
  		Request(numberOfDice: int)
  		Return(numberOfDice: int)
  		PrintStatus(str: String, count: int)
  endClass

  class SleepingBarberClass
  	superclass Object
  	fields
  		waiting: int
  		chairs: int
  		customers: Semaphore
  		barbers: Semaphore
  		mutexLock: Mutex
  		
  	methods
  		Init()
  		Barber()
  		Customer(wait: int)
  		spawn()
  		waitForCut()
  		cutHair(wait: int)
  	endClass

endHeader
