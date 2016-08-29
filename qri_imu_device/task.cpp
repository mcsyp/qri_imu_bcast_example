#include "task.h"
Task::Task(){
	m_pfnTask = 0;
  m_pfnReset = 0;
	m_interval = TASK_DEFAULT_INTERVAL;
	m_lastTrigger = 0;
}
void Task::init(unsigned long us,task_func pfn,task_reset pfnReset){
	if(us<TASK_MIN_INTERVAL || pfn==0){
		return;
	}
	m_interval = us;
	m_pfnTask = pfn;
  m_pfnReset = pfnReset;
}

void Task::trigger(unsigned long currentUs)
{
	if(currentUs-m_lastTrigger > m_interval){
		if(m_pfnTask){
			(*m_pfnTask)();
		}
		m_lastTrigger = currentUs;
	}
}
void Task::reset()
{
  if(m_pfnReset){
    (*m_pfnReset)();
  }
}





