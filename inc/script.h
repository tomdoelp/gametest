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
		/* yeah this breaks encapsulation sue me */
		Tween<T>(T *var, T init, T fin, int frames, T threshold) : var(var), fin(fin), frames(frames), threshold(threshold) {
			*var = init;
			amount = (fin - *var) / frames;
		}
		~Tween<T>() {}
		bool step() {
			T diff = MAX(fin, *var) - MIN(fin, *var);
			if (diff > threshold) {
				*var += amount;
				return true;
			}
			else { 
				/* D U N */
				*var = fin;
				return false;
			}
		}


	private:
		T *var;
		T fin;
		T amount;
		/* TODO: check this instead of the actual value... allowing tween composition */
		T total_change;

		int frames;
		T threshold;
		int timer;
};

template <class T> class BounceTween : public GenericTween {
	public:
		BounceTween<T> (T *var, T init, T fin, int frames, T threshold) : var(var), init(init), fin(fin), frames(frames), threshold(threshold) {
			*var = init;
			amount = (fin - *var) / frames;
			direction = true;
		}
		~BounceTween<T>() {}
		bool step() {
			T diff;
			if (direction) {
				diff = MAX(fin, *var) - MIN(fin, *var);
				if (diff > threshold) {
					*var += amount;
				}
				else {
					*var = fin;
					direction = !direction;
					*var -= amount;
				}
			}
			else {
				diff = MAX(init, *var) - MIN(init, *var);
				if (diff > threshold) {
					*var -= amount;
				}
				else {
					*var = init;
					direction = !direction;
					*var += amount;
				}
			}

			return true;
		}
	private:
		T *var;
		T init;
		T fin;
		T amount;

		int frames;
		T threshold;
		int timer;

		bool direction;
};


/* LoopTween with stop function, pingpong, norepeatends, loop points separate from starting value, etc */

#endif
