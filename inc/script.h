#ifndef SCRIPT_H
#define SCRIPT_H

#include "global.h"

class GenericTween {
	public:
		virtual ~GenericTween(){}
		virtual bool step(){ return false; }
};

template <class T> class Tween : public GenericTween {
	public:
		Tween<T>(T *var, T val, int frames, T threshold) : var(var), val(val), frames(frames), threshold(threshold) {
			amount = (val - *var) / frames;
			LOG("initial: " << *var << " final: " << val << " frames: " << frames << " amt: " << amount);
		}
		~Tween<T>() {}
		bool step() {
			if (val - *var < threshold) {
				*var += amount;
				return true;
			}
			else
				return false;
		}


	private:
		T *var;
		T val;
		T amount;

		int frames;
		T threshold;
		int timer;
};

#endif
