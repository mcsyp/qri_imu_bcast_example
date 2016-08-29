#ifndef _TASK_H_
#define _TASK_H_

#define TASK_DEFAULT_INTERVAL 50000 //50ms
#define TASK_MIN_INTERVAL 5000 //2ms

typedef void (*task_func)();
typedef void (*task_reset)();
class Task{
public:
	Task();
	void init(unsigned long us, task_func pfn, task_reset pfnReset);
	void trigger(unsigned long currentUs);
  void reset();
protected:
	unsigned long m_interval;
	unsigned long m_lastTrigger;
	task_func m_pfnTask;
  task_reset m_pfnReset;
};

#endif





