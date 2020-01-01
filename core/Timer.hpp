#ifndef TIMER_HPP
#define TIMER_HPP
class Timer{
public:
	Timer() : m_pause_for(0), m_time_left(0){}
	Timer(int pause_for,int time_left) : m_pause_for(pause_for), m_time_left(time_left){}
	bool operator()(){
		if (m_pause_for == 0){
			if (m_time_left > 0){
				m_time_left--;
			}
		} else {
			m_pause_for--;
		}
		return ((m_pause_for == 0)&&(m_time_left == 0));
	}
	int getDelay(){
		return m_pause_for;
	}
	int getTime(){
		return m_time_left;
	}
	void setTime(int time){
		m_time_left = time;
	}
	void setDelay(int delay){
		m_pause_for = delay;
	}
private:
	int m_pause_for = 0;
	int m_time_left = 0;
};
#endif
