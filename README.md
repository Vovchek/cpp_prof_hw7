
problems/questions to (re)viewers:

- command are duplicated at each subscriber class. This is due to "push" model implementation.
  Model may be rewritten to "pull", but setNextCommand method will have no sence to subscriber
  if it does not store bulk.
- because busines logic demands to flush incomlete block after encounting EOF,
  some termination sequence need to be commited before observable destruction.
  It cannot be done in destructor, because subscribers' references are destroyed
  before the user code in destructor is executed. Thus we must explicitly call
  terminate method. Can we get rid of it to flush at RAII style?
- console and file loggers have very much in common, so the code looks rather WET.
  Can someone give a hint how to DRY?
